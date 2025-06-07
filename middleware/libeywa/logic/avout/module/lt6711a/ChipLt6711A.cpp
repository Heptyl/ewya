
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipLt6711A"
#include "XbhLog.h"

#include <cutils/properties.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ChipLt6711A.h"
#include "XbhService.h"

XbhMutex                       ChipLt6711A::mLock;

#define I2C_SLAVE_ADDR      0x41
#define MAX_DATA_LENGTH (1024 * 1024)    //buffer最大长度，升级文件大小不超过1M

static unsigned char g_data_buffer[MAX_DATA_LENGTH]={0};
static unsigned int  g_data_buffer_len = 0;
static unsigned int  g_lt6711_flash_addr = 0;
static unsigned int  g_lt6711_count = 0;
static unsigned int  g_download_check_count = 0;

XBH_S32 ChipLt6711A::i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipLt6711A::i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, u8RegAddr, 1, 1, &reg_value);
    return s32Ret;
}

XBH_S32 ChipLt6711A::i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

/**
 * 复位当前转换芯片
 */
XBH_S32 ChipLt6711A::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (mRGpio != -1) {
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        usleep(10 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
        usleep(10 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);

        usleep(2000*1000);  //保险起见，预留2s等ic稳定
        s32Ret = XBH_SUCCESS;
    }
    return s32Ret;
}

XBH_S32 ChipLt6711A::checkVersion()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    unsigned char version_info[6] = {0};
    i2c_data_write(0xff, 0x80);
    i2c_data_read(0xD0, 1, version_info, 6);
    XLOGD("===> lt6711 version = [%02X %02X %02X %02x %02x %02x]!\n",version_info[0], version_info[1], version_info[2],version_info[3],version_info[4],version_info[5]);

    char propVal[20] = {0};
    sprintf(propVal,"%02X%02X%02X%02X%02X%02X", version_info[0], version_info[1], version_info[2], version_info[3], version_info[4], version_info[5]);
    return s32Ret;
}

XBH_S32 ChipLt6711A::open_upgrade_file(const char *upgrade_file_path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("\n===> %s\n",__FUNCTION__);

    FILE *fp = XBH_NULL;
    fp = fopen(upgrade_file_path, "r");
    if(fp != XBH_NULL)
    {
        fseek(fp,0,SEEK_END);
        g_data_buffer_len = ftell(fp);        //计算文件大小
        fseek(fp,0,SEEK_SET);
        XLOGD("==> read upgrade file size:%d\n",g_data_buffer_len);
        if(g_data_buffer_len < MAX_DATA_LENGTH)
        {
            fread(g_data_buffer, 1, g_data_buffer_len, fp);
            s32Ret = XBH_SUCCESS;
        }
        else
            XLOGD("==> upgrade file size more than %d bytes.\n",MAX_DATA_LENGTH);
        fclose(fp);
    }
    else{
        XLOGD("==> open upgrade file fail!\n");
    }

    return s32Ret;
}

XBH_S32 ChipLt6711A::lt6711_i2c_to_flash_config(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("\n===> %s\n",__FUNCTION__);
    //iic setting
    i2c_data_write(0xff, 0x80);
    i2c_data_write(0xee, 0x01);
	usleep(2*100);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x5e, 0xdf);
    //config parameter
    i2c_data_write(0x58, 0x01);
    i2c_data_write(0x59, 0x51);
    i2c_data_write(0x5a, 0x92);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x58, 0x21);
    i2c_data_write(0xff, 0x81);
    i2c_data_write(0x08, 0xbf);
	usleep(2*100);
    i2c_data_write(0x08, 0xff);
    usleep(2*1000);
    return s32Ret;
}

XBH_S32 ChipLt6711A::lt6711_flash_block_erase(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("\n===> %s\n",__FUNCTION__);
    i2c_data_write(0xFF, 0x80);
    i2c_data_write(0xEE, 0x01);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x5a, 0x86);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x5b, 0x00);
    i2c_data_write(0x5c, 0x00);
    i2c_data_write(0x5d, 0x00);
    i2c_data_write(0x5a, 0x83);
    i2c_data_write(0x5a, 0x82);
    usleep(200*1000);
    return s32Ret;
}

XBH_S32 ChipLt6711A::lt6711_download_core(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("\n===> %s\n",__FUNCTION__);
	unsigned  addr1,addr2,addr3;
    g_lt6711_flash_addr = 0;
    g_lt6711_count = 0;
	i2c_data_write(0xff, 0x80);
	i2c_data_write(0xee, 0x01);
    i2c_data_write(0xff, 0x81);
	i2c_data_write(0x08, 0xbf);
	i2c_data_write(0x08, 0xff);
	while( (g_lt6711_count+32) < g_data_buffer_len)
	{
		addr1=g_lt6711_flash_addr;
        addr2=g_lt6711_flash_addr>>8;
        addr3=g_lt6711_flash_addr>>16;
        i2c_data_write(0xff, 0x80);
        i2c_data_write(0x5a, 0x86);
        i2c_data_write(0x5a, 0x82);
        i2c_data_write(0x5E, 0xDF);
        i2c_data_write(0x5a, 0xA2);
        i2c_data_write(0x5a, 0x82);
        i2c_data_write(0x58, 0x21);
        s32Ret = i2c_data_write_nbytes(0x59, 1, g_data_buffer + g_lt6711_count, 32);
        i2c_data_write(0x5b, addr3);
        i2c_data_write(0x5c, addr2);
        i2c_data_write(0x5d, addr1);
        i2c_data_write(0x5e, 0xC0);
        i2c_data_write(0x5a, 0x92);
        i2c_data_write(0x5a, 0x82);
        g_lt6711_flash_addr = g_lt6711_flash_addr + 0x20;
        g_lt6711_count = g_lt6711_count + 32;
	}
    addr1=g_lt6711_flash_addr;
    addr2=g_lt6711_flash_addr>>8;
    addr3=g_lt6711_flash_addr>>16;
    i2c_data_write(0x5a, 0x86);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x5E, 0xDF);
    i2c_data_write(0x5a, 0xa2);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x58, 0x21);
    s32Ret = i2c_data_write_nbytes(0x59, 1, g_data_buffer + g_lt6711_count, g_data_buffer_len - g_lt6711_count);
    i2c_data_write(0x5b, addr3);
    i2c_data_write(0x5c, addr2);
    i2c_data_write(0x5d, addr1);
    i2c_data_write(0x5e, 0xc0);
    i2c_data_write(0x5a, 0x92);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0xff, 0x81);
    i2c_data_write(0x08, 0xbf);
	usleep(200);
    i2c_data_write(0x08,0xff);
    return s32Ret;
}

//逐个字节比较
XBH_S32 ChipLt6711A::lt6711_read_byte_compare(unsigned int u32RegAddr, unsigned int RegAddrLen,unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    unsigned char *buffer = (unsigned char *)malloc(data_len*sizeof(unsigned char));
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, buffer);
    for(int i = 0; i < data_len; i++)
    {
        if(buffer[i] != p_data[i]) //如果数据不同
        {
            g_download_check_count++;
        }
    }
    free(buffer);
    buffer = NULL;
    return s32Ret;
}

XBH_S32 ChipLt6711A::lt6711_read_flash(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("\n===> %s\n", __FUNCTION__);
	unsigned char addr1, addr2, addr3;
	g_lt6711_flash_addr = 0;
	g_lt6711_count = 0;
	g_download_check_count = 0;
    i2c_data_write(0xff, 0x80);
    i2c_data_write(0xee, 0x01);
    i2c_data_write(0x5a, 0x86);
    i2c_data_write(0x5a, 0x82);
    //wren
    i2c_data_write(0x5a, 0x86);
    i2c_data_write(0x5a, 0x82);
    while((g_lt6711_count+32) < g_data_buffer_len)
    {
        addr1=g_lt6711_flash_addr;
        addr2=g_lt6711_flash_addr>>8;
        addr3=g_lt6711_flash_addr>>16;
        i2c_data_write(0x5e, 0x5f);
        i2c_data_write(0x5a, 0xa2);
        i2c_data_write(0x5a, 0x82);
        i2c_data_write(0x5b, addr3);
        i2c_data_write(0x5c, addr2);
        i2c_data_write(0x5d, addr1);
        i2c_data_write(0x5a, 0x92);
        i2c_data_write(0x5a, 0x82);
        i2c_data_write(0x58, 0x21);
        s32Ret = lt6711_read_byte_compare(0x5f, 1, g_data_buffer + g_lt6711_count, 32);
        g_lt6711_flash_addr = g_lt6711_flash_addr + 0x20;
        g_lt6711_count = g_lt6711_count + 32;
    }
    addr1=g_lt6711_flash_addr;
    addr2=g_lt6711_flash_addr>>8;
    addr3=g_lt6711_flash_addr>>16;
    i2c_data_write(0x5e, 0x5f);
    i2c_data_write(0x5a, 0xa2);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x5b, addr3);
    i2c_data_write(0x5c, addr2);
    i2c_data_write(0x5d, addr1);
    i2c_data_write(0x5a, 0x92);
    i2c_data_write(0x5a, 0x82);
    i2c_data_write(0x58, 0x21);
    s32Ret = lt6711_read_byte_compare(0x5f, 1, g_data_buffer+g_lt6711_count, g_data_buffer_len - g_lt6711_count);
    //fifo rst_n
    i2c_data_write(0xff, 0x81);
    i2c_data_write(0x08, 0xbf);
    usleep(200);
    i2c_data_write(0x08, 0xff);
    if(g_download_check_count == 0)
    {
        s32Ret = XBH_SUCCESS;
        XLOGD("\n===> lt6711 Download Code Success!!\n");
    }
    else
    {
        mState = LT6711_FAILURE;
        s32Ret = XBH_FAILURE;
        XLOGD("\n===> lt6711 Download Code Fail!!\n");
    }
    return s32Ret;
}

XBH_S32 ChipLt6711A::lt6711_load_code_to_flash(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("\n===> %s\n", __FUNCTION__);
    lt6711_i2c_to_flash_config();   //写入配置
    lt6711_flash_block_erase();     //擦除flash
    lt6711_download_core();         //下载代码
    usleep(10*1000);

    // do check_sum
    if(XBH_SUCCESS != lt6711_read_flash()) {
        return XBH_FAILURE;
    }
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipLt6711A::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(strVersion == XBH_NULL) {
        XLOGD("===> getFirmwareVersion strVersion is NULL");
        return XBH_FAILURE;
    }
    if (mState == LT6711_RUNNING)
    {
        XLOGW("Warning: ite6711 is updating...");
        return XBH_FAILURE;
    }
    unsigned char version_info[6] = {0};
    i2c_data_write(0xff, 0x80);
    i2c_data_read(0xD0, 1, version_info, 6);
    XLOGD("===> lt6711 version = [%02X %02X %02X %02x %02x %02x]!\n",version_info[0], version_info[1], version_info[2],version_info[3],version_info[4],version_info[5]);
    sprintf(strVersion,"%02X%02X%02X%02X%02X%02X", version_info[0], version_info[1], version_info[2], version_info[3], version_info[4], version_info[5]);
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 ChipLt6711A::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == LT6711_RUNNING)
    {
        XLOGW("Warning: lt6711 is updating...");
        return XBH_FAILURE;
    }

    if (strFilePath == XBH_NULL)
    {
        XLOGE("Error: fileName is XBH_NULL");
        return XBH_FAILURE;
    }

    //set force upgrade flag
    mForceUpgrade = bForceUpgrade;
    //mI2cNum = I2C_CHANNEL;

    //set upgrade file name
    memset(mFileName, 0x00, sizeof(mFileName));
    if (strlen(strFilePath) >= sizeof(mFileName))
    {
        XLOGE("Error: fileName is too long: %d", strlen(strFilePath));
        return XBH_FAILURE;
    }
    strncpy(mFileName, strFilePath, strlen(strFilePath));

    XbhMWThread::run(XbhMWThread::ONCE);
    return XBH_SUCCESS;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 ChipLt6711A::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

XBH_S32 ChipLt6711A::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGE("%s %d \n", __func__, __LINE__);
    unsigned char chip_id[2] = {0};
    s32Ret |= i2c_data_write(0xff, 0x80);
    s32Ret |= i2c_data_write(0xee, 0x01);
    s32Ret |= i2c_data_write(0xff, 0x81);
    s32Ret |= i2c_data_read(0x00, 1, chip_id, 2);
    XLOGD("===> lt6711 chip_id = [0x%02X%02X]!\n", chip_id[0], chip_id[1]);
    if(chip_id[0] != 0x18 ||  chip_id[1] != 0x02)
    {
        XLOGE("===> Find chip fail");
        *enable = XBH_FALSE;
    }else{
        XLOGE("===> Find chip ok");
        *enable = XBH_TRUE;
    }
    return s32Ret;
}

void ChipLt6711A::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32State = XBH_FALSE;

    //set task status
    mState = LT6711_RUNNING;

    //读chip ID
    s32Ret = getChipExist((XBH_BOOL*)&s32State);
    if(XBH_TRUE != s32State){ //读chip ID
        XLOGE("\n===> %s get chip id fail!\n", __FUNCTION__);
        mState = LT6711_FAILURE;
        return;
    }

    s32Ret = open_upgrade_file(mFileName);
    if(XBH_SUCCESS != s32Ret){
        XLOGE("\n===> %s init fail, file:%s\n", __FUNCTION__, mFileName);
        mState = LT6711_FAILURE;
        return;
    }

    //获取版本信息
    s32Ret = checkVersion();
    if(XBH_SUCCESS != s32Ret){
        XLOGE("\n===> %s get version fail!\n", __FUNCTION__);
        mState = LT6711_FAILURE;
        return;
    }

    //升级固件
    s32Ret = lt6711_load_code_to_flash();
    if(XBH_SUCCESS != s32Ret){
        XLOGE("\n===> %s load code to flash fail!\n", __FUNCTION__);
        resetChip();
        mState = LT6711_FAILURE;
        return;
    }

    resetChip();
    mState = LT6711_SUCCESS;

}

//---------------- custom device interface end ----------------

ChipLt6711A::ChipLt6711A(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mState = LT6711_IDLE;
    mForceUpgrade = XBH_FALSE;

    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    XLOGD(" end ");
}

ChipLt6711A::~ChipLt6711A()
{

}