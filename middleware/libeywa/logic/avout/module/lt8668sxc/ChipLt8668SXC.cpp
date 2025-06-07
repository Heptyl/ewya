#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipLt8668SXC"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhService.h"
#include "ChipLt8668SXC.h"
#include <cutils/properties.h>
#include "XbhKernelMsgManager.h"

#define Flash_Addr_CRC              0x007FFF
#define FW_FLASH_ADDR               0x00
#define FW2_FLASH_ADDR              0x40000

#define MAX_UX_FIRMWARE_LENGTH      60000

#define MAJOR_VERSION       0x01
#define MIDDLE_VERSION      0x00
#define MINOR_VERSION       0x02

#define MAX_DATA_LENGTH (256 * 1024)    //buffer最大长度，升级文件大小不超过256k

static unsigned char g_data_buffer[MAX_DATA_LENGTH]={0};
static unsigned int  g_data_buffer_len = 0;
static unsigned int  g_lt8668sxc_flash_addr = 0;
static unsigned int  g_lt8668sxc_count=0;
static unsigned int  g_download_check_count = 0;
static unsigned char g_crc_val = 0;

//开机后获取版本号
static XBH_CHAR gVersion[16] = {0};

ChipLt8668SXC::ChipLt8668SXC(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" start ");
    this->mI2cNumber = i2cBus;
    this->mI2cAddress = Addr;
    this->mPowerGpio = pGpio;
    this->mPowerLevel = pLevel;
    this->mResetGpio = rGpio;
    this->mResetLevel = rLevel;
    if(mPowerGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mPowerGpio, mPowerLevel);
        usleep(100*1000);
    }
    getFirmwareVersion(gVersion);
    XLOGD(" end ");
}

ChipLt8668SXC::~ChipLt8668SXC()
{
    XLOGD("~ end");
}

XBH_S32 ChipLt8668SXC::resetChip(XBH_BOOL hw)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 chip_id[2] = {0};
    s32Ret |= i2c_data_write(0xff, 0xe0);
    s32Ret |= i2c_data_write(0xee, 0x01);
    s32Ret |= i2c_data_write(0xff, 0xe1);
    s32Ret |= i2c_data_read(0x00, 1, chip_id, 2);
    XLOGD("===> chip_id = [0x%02X%02X]!\n",chip_id[1], chip_id[0]);
    s32Ret |= i2c_data_write(0xff, 0xe0);
    s32Ret |= i2c_data_write(0xee, 0x00);
    if(chip_id[0] == 0x00 &&  chip_id[1] == 0x00)
    {
        XLOGE("===> Find chip fail");
        *enable = XBH_FALSE;
    }
    else
    {
        XLOGE("===> Find chip ok");
        *enable = XBH_TRUE;
    }
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == XBH_UPGRADE_RUNNING)
    {
        XLOGW("Warning: lt8668sxc is updating...");
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

XBH_S32 ChipLt8668SXC::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(strVersion == XBH_NULL)
    {
        XLOGD("===> getFirmwareVersion strVersion is NULL");
        return XBH_FAILURE;
    }
    if (mState == XBH_UPGRADE_RUNNING)
    {
        XLOGW("Warning: is updating...");
        return XBH_FAILURE;
    }
    XbhKernelMsgManager::getInstance()->sendMsgToKernel(0, 1, 0, 0, 0);
    usleep(1000 * 1000);//等待kernel不再操作外挂芯片的IIC
    unsigned char version_info[5] = {0};
    i2c_data_write(0xff, 0xe0);
    //i2c_data_write(0xee, 0x01);
    //i2c_data_write(0xff, 0xe0);
    i2c_data_read(0x89, 1, version_info, 5);
    XLOGD("===> checkVersion version [%02X %02X %02X %02x %02X]!\n", version_info[0], version_info[1], version_info[2], version_info[3], version_info[4]);
    sprintf(strVersion, "%02X%02X%02X%02X%02X", version_info[0], version_info[1], version_info[2], version_info[3], version_info[4]);
    property_set(LT8668SXC_VERSION, strVersion);
    XbhKernelMsgManager::getInstance()->sendMsgToKernel(0, 0, 0, 0, 0);
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::getUpgradeState(XBH_S32 *pstat)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *pstat = mState;
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, u8RegAddr, 1, 1, &reg_value);
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::lt8668sxc_i2c_write_data_to_flash(int Addr, unsigned char data)
{
    int addr1,addr2,addr3;
    addr1=Addr;
    addr2=Addr>>8;
    addr3=Addr>>16;
    i2c_data_write(0xFF, 0xE1);//configure parameter
    i2c_data_write(0x03, 0x3F);
    i2c_data_write(0x03, 0xFF);//reset

    i2c_data_write(0xFF, 0xE0);//configure parameter
    i2c_data_write(0x5A, 0x04);//write enable
    i2c_data_write(0x5A, 0x00);

    i2c_data_write(0x5E, 0xDF);//iic data to fifo
    i2c_data_write(0x5A, 0x20);
    i2c_data_write(0x5A, 0x00);
    i2c_data_write(0x58, 0x21);
    i2c_data_write(0x59, data );//data

    i2c_data_write(0x5B, addr3);//addr
    i2c_data_write(0x5C, addr2);//addr
    i2c_data_write(0x5D, addr1);//addr
    i2c_data_write(0x5A, 0x10);
    i2c_data_write(0x5A, 0x00);
    usleep(10*1000);  //sleep 10ms
    return XBH_SUCCESS;
}

XBH_S32 ChipLt8668SXC::lt8668sxc_i2c_read_data_from_flash(int Addr)
{
    int addr1,addr2,addr3;
    unsigned char read_data[1]={0xFF};
    addr1=Addr;
    addr2=Addr>>8;
    addr3=Addr>>16;
    i2c_data_write( 0xFF , 0xE0 );
    i2c_data_write( 0x5E , 0x5F );
    i2c_data_write( 0x5A , 0x20 );
    i2c_data_write( 0x5A , 0x00 );
    i2c_data_write( 0x5B , addr3 );//addr
    i2c_data_write( 0x5C , addr2 );//addr
    i2c_data_write( 0x5D , addr1 );//addr
    i2c_data_write( 0x5A , 0x10 );
    i2c_data_write( 0x5A , 0x00 );
    i2c_data_write( 0x58 , 0x21 );
    i2c_data_write( 0x5A , 0x10 );//0x10
    i2c_data_write( 0x5A , 0x00 );
    i2c_data_write( 0x58 , 0x21 );
    usleep(1*1000);
    i2c_data_read(0x5F,1,read_data,1);

    return read_data[0];
}

XBH_S32 ChipLt8668SXC::open_upgrade_file(const char *upgrade_file_path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD(" ===> \n");
    FILE *fp = XBH_NULL;
    fp = fopen(upgrade_file_path, "r");
    if(fp != XBH_NULL)
    {
        fseek(fp,0,SEEK_END);
        g_data_buffer_len = ftell(fp);        //计算文件大小
        fseek(fp,0,SEEK_SET);
        XLOGD("==> read upgrade file size : %d \n", g_data_buffer_len);
        if(g_data_buffer_len < MAX_DATA_LENGTH)
        {
            fread(g_data_buffer, 1, g_data_buffer_len, fp);
            g_crc_val = lt8668sxc_get_crc(g_data_buffer, g_data_buffer_len);
            XLOGD("==> read upgrade file crc value : 0x%02x\n", g_crc_val);
            s32Ret = XBH_SUCCESS;
        }
        else
        {
            XLOGE("==> upgrade file size more than %d bytes.\n", MAX_DATA_LENGTH);
        }
        fclose(fp);
    }

    if (g_data_buffer_len <= MAX_UX_FIRMWARE_LENGTH){
        XLOGE("==> Please check if the firmware is correct.\n");
        s32Ret = XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 ChipLt8668SXC::lt8668sxc_i2c_to_flash_config(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD(" ===> \n");
    //configure parameter
    i2c_data_write(0xFF, 0xE0);
    i2c_data_write(0xEE, 0x01);
    i2c_data_write(0x5E, 0xC1);
    i2c_data_write(0x58, 0x00);
    i2c_data_write(0x59, 0x50);
    i2c_data_write(0x5A, 0x10);
    i2c_data_write(0x5A, 0x00);
    i2c_data_write(0x58, 0x21);
    usleep(2 * 1000);
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::lt8668sxc_flash_block_erase(int Addr)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD(" ===> \n");
    unsigned char addr1,addr2,addr3;
    g_lt8668sxc_flash_addr = Addr;
    addr1=g_lt8668sxc_flash_addr;
    addr2=g_lt8668sxc_flash_addr>>8;
    addr3=g_lt8668sxc_flash_addr>>16;

    i2c_data_write(0xFF, 0xE0);
    i2c_data_write(0xEE, 0x01);
    i2c_data_write(0x5A, 0x04 );
    i2c_data_write(0x5A, 0x00 );
    i2c_data_write(0x5B, addr3 );//addr
    i2c_data_write(0x5C, addr2 );//addr
    i2c_data_write(0x5D, addr1 );//addr
    i2c_data_write(0x5A, 0x01 );//block erase
    i2c_data_write(0x5A, 0x00 );
    usleep(500 * 1000);  //sleep 500ms
#if 1
    i2c_data_write(0xFF, 0xE0);
    i2c_data_write(0xEE, 0x01);
    i2c_data_write(0x5A, 0x04 );
    i2c_data_write(0x5A, 0x00 );
    i2c_data_write(0x5B, addr3 );//addr
    i2c_data_write(0x5C, 0x80 );//addr
    i2c_data_write(0x5D, addr1 );//addr
    i2c_data_write(0x5A, 0x01 );//block erase
    i2c_data_write(0x5A, 0x00 );
    usleep(500 * 1000);  //sleep 500ms
#endif
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::lt8668sxc_download_core(int Addr)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD(" ===> \n");
    unsigned char addr1, addr2, addr3;
    g_lt8668sxc_flash_addr = Addr;
    g_lt8668sxc_count=0;
    i2c_data_write(0xFF,0xE0);
    i2c_data_write(0xEE,0x01);
    while((g_lt8668sxc_count + 32) < g_data_buffer_len)
    {
        addr1 = g_lt8668sxc_flash_addr;
        addr2 = g_lt8668sxc_flash_addr >> 8;
        addr3 = g_lt8668sxc_flash_addr >> 16;
        i2c_data_write( 0xFF , 0xE1 );//configure parameter
        i2c_data_write( 0x03 , 0x3F );
        i2c_data_write( 0x03 , 0xFF );//reset
        i2c_data_write( 0xFF , 0xE0 );//configure parameter
        i2c_data_write( 0x5A , 0x04 );//write enable
        i2c_data_write( 0x5A , 0x00 );

        i2c_data_write( 0x5E , 0xDF );//iic data to fifo
        i2c_data_write( 0x5A , 0x20 );
        i2c_data_write( 0x5A , 0x00 );
        i2c_data_write( 0x58 , 0x21 );

        s32Ret = i2c_data_write_nbytes(0x59, 1, g_data_buffer + g_lt8668sxc_count, 32);

        i2c_data_write( 0x5B , addr3 );//addr
        i2c_data_write( 0x5C , addr2 );//addr
        i2c_data_write( 0x5D , addr1 );//addr
        i2c_data_write( 0x5A , 0x10 );
        i2c_data_write( 0x5A , 0x00 );
        g_lt8668sxc_flash_addr = g_lt8668sxc_flash_addr + 0x20;
        g_lt8668sxc_count = g_lt8668sxc_count + 32;
    }
    addr1 = g_lt8668sxc_flash_addr;
    addr2 = g_lt8668sxc_flash_addr >> 8;
    addr3 = g_lt8668sxc_flash_addr >> 16;
    
    i2c_data_write( 0xFF , 0xE1 );//configure parameter
    i2c_data_write( 0x03 , 0x3F );
    i2c_data_write( 0x03 , 0xFF );//reset
    i2c_data_write( 0xFF , 0xE0 );//configure parameter
    i2c_data_write( 0x5A , 0x04 );//write enable
    i2c_data_write( 0x5A , 0x00 );

    i2c_data_write( 0x5E , 0xDF );//iic data to fifo
    i2c_data_write( 0x5A , 0x20 );
    i2c_data_write( 0x5A , 0x00 );
    i2c_data_write( 0x58 , 0x21 );

    s32Ret = i2c_data_write_nbytes(0x59, 1, g_data_buffer + g_lt8668sxc_count, g_data_buffer_len - g_lt8668sxc_count);
    i2c_data_write( 0x5B , addr3 );//addr
    i2c_data_write( 0x5C , addr2 );//addr
    i2c_data_write( 0x5D , addr1 );//addr
    i2c_data_write( 0x5A , 0x10 );
    i2c_data_write( 0x5A , 0x00 );
    return s32Ret;
}

//逐个字节比较
XBH_S32 ChipLt8668SXC::lt8668sxc_read_byte_compare(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    unsigned char *buffer = (unsigned char *)malloc(data_len*sizeof(unsigned char));
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, u32RegAddr, RegAddrLen, data_len, buffer);
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

XBH_S32 ChipLt8668SXC::lt8668sxc_read_flash(int Addr)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD(" ===> \n");
    unsigned char addr1,addr2,addr3;
    g_lt8668sxc_flash_addr=Addr;
    g_lt8668sxc_count=0;
    g_download_check_count=0;
    i2c_data_write(0xFF,0xE0);
    i2c_data_write(0xEE,0x01);
    while((g_lt8668sxc_count + 32) < g_data_buffer_len)
    {
        addr1=g_lt8668sxc_flash_addr;
        addr2=g_lt8668sxc_flash_addr>>8;
        addr3=g_lt8668sxc_flash_addr>>16;
        i2c_data_write( 0x5E , 0x5F );
        i2c_data_write( 0x5A , 0x20 );
        i2c_data_write( 0x5A , 0x00 );
        i2c_data_write( 0x5B , addr3 );//addr
        i2c_data_write( 0x5C , addr2 );//addr
        i2c_data_write( 0x5D , addr1 );//addr
        i2c_data_write( 0x5A , 0x10 );
        i2c_data_write( 0x5A , 0x00 );
        i2c_data_write( 0x58 , 0x21 );
        s32Ret = lt8668sxc_read_byte_compare(0x5f, 1, g_data_buffer + g_lt8668sxc_count, 32);
        g_lt8668sxc_flash_addr = g_lt8668sxc_flash_addr+0x20;
        g_lt8668sxc_count = g_lt8668sxc_count+32;
    }
    addr1=g_lt8668sxc_flash_addr;
    addr2=g_lt8668sxc_flash_addr>>8;
    addr3=g_lt8668sxc_flash_addr>>16;
    i2c_data_write( 0x5E , 0x5F );
    i2c_data_write( 0x5A , 0x20 );
    i2c_data_write( 0x5A , 0x00 );
    i2c_data_write( 0x5B , addr3 );//addr
    i2c_data_write( 0x5C , addr2 );//addr
    i2c_data_write( 0x5D , addr1 );//addr
    i2c_data_write( 0x5A , 0x10 );
    i2c_data_write( 0x5A , 0x00 );
    i2c_data_write( 0x58 , 0x21 );
    s32Ret = lt8668sxc_read_byte_compare(0x5f, 1, g_data_buffer + g_lt8668sxc_count, g_data_buffer_len - g_lt8668sxc_count);
    if(g_download_check_count == 0)
    {
        s32Ret = XBH_SUCCESS;
        XLOGD("\n===> UXE lt8668sxc_read_flash  Download Code Success!!\n");
    }
    else
    {
        mState = XBH_UPGRADE_FAILURE;
        s32Ret = XBH_FAILURE;
        XLOGD("\n===> UXE lt8668sxc_read_flash  Download Code Fail!!\n");
    }
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::lt8668sxc_load_code_to_flash(int Addr)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    unsigned char data;
    if(Addr == 0x00)
    {
        XLOGD(" start update main firmeware ===> \n");
    }
    else
    {
        XLOGD(" start update backup firmeware ===> \n");
    }
    lt8668sxc_i2c_to_flash_config();       //写入配置
    lt8668sxc_flash_block_erase(Addr);     //擦除flash
    lt8668sxc_download_core(Addr);         //下载代码
    usleep(10*1000);

    if(XBH_SUCCESS != lt8668sxc_read_flash(Addr))
    {
        return XBH_FAILURE;
    }
    #if 0
    lt8668sxc_i2c_write_data_to_flash(Flash_Addr_CRC, g_crc_val);
    data = lt8668sxc_i2c_read_data_from_flash(Flash_Addr_CRC);
    XLOGD("==>lt8668sxc_loadCustomerCode_to_flash g_crc_val = 0x%02x data = 0x%02x \r\n",g_crc_val, data);
    if(g_crc_val == data)
    {
        XLOGD("Erase and Write crc to flash is OK!  \r\n");
    }
    else
    {
        XLOGD("Erase and Write crc to flash is Fail!  \r\n");
        s32Ret = XBH_FAILURE;
    }
    #endif
    return s32Ret;
}

XBH_S32 ChipLt8668SXC::checkVersion()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("===> start checkVersion \n");
    unsigned char version_info[4] = {0};
    i2c_data_write(0xff, 0xe0);
    //i2c_data_write(0xee, 0x01);
    //i2c_data_write(0xff, 0xe0);
    i2c_data_read(0x80, 1, version_info, 4);
    sprintf(gVersion, "%02X%02X%02X%02X", version_info[0], version_info[1], version_info[2], version_info[3]);
    XLOGD("===> checkVersion gVersion = %s \n", gVersion);
    char value[PROP_VALUE_MAX]; 
    property_get(LT8668SXC_VERSION, value, "unknown");
    XLOGD("===> get version persist.vendor.xbh.ver.lt8668sxc  = %s \n", value);
    //版本一致无需升级
    if(strcmp(gVersion, value) == 0)
    {
        //未开启强制升级
        if(!mForceUpgrade)
        {
            XLOGD("\nThis program is already up-to-date and does not need to be upgraded.\n");
            return XBH_FAILURE;
        }
        else
        {
            XLOGE("\nThis program is already up-to-date,but force upgrade flag is opened, continue upgrade...\n");
        }
    }
    return s32Ret;
}


/**
 *******************************************************************************
 * @brief   bit reverse
 * @param   [in] inVal	- in data
 * @param   [in] bits   - inverse bit
 * @return  out data
 * @note
 *******************************************************************************
 */
unsigned int ChipLt8668SXC::bitsreverse(unsigned int inVal, unsigned char bits)
{
    unsigned int outVal = 0;
    unsigned char i;

    for(i=0; i<bits; i++)
    {
        if(inVal & (1 << i)) outVal |= 1 << (bits - 1 - i);
    }

    return outVal;
}

/**
 *******************************************************************************
 * @brief   get crc value
 * @param   [in] type   - CRC config
 * @param   [in] *buf   - data buffer
 * @param   [in] bufLen - data len
 * @return  crc value
 * @note
 *******************************************************************************
 */
unsigned int ChipLt8668SXC::getcrc(CrcInfoTypeS type, unsigned char *buf, unsigned int bufLen)
{
    unsigned char width  = type.Width;
    unsigned int  poly   = type.Poly;
    unsigned int  crc    = type.CrcInit;
    unsigned int  xorout = type.XorOut;
    unsigned char refin  = type.RefIn;
    unsigned char refout = type.RefOut;
    unsigned char n;
    unsigned int  bits;
    unsigned int  data;
    unsigned char i;

    n    =  (width < 8) ? 0 : (width-8);
    crc  =  (width < 8) ? (crc<<(8-width)) : crc;
    bits =  (width < 8) ? 0x80 : (1 << (width-1));
    poly =  (width < 8) ? (poly<<(8-width)) : poly;
    while(bufLen--)
    {
        data = *(buf++);
        if(refin == true)
            data = bitsreverse(data, 8);
        crc ^= (data << n);
        for(i=0; i<8; i++)
        {
            if(crc & bits)
            {
                crc = (crc << 1) ^ poly;
            }
            else
            {
                crc = crc << 1;
            }
        }
    }
    crc = (width<8) ? (crc>>(8-width)) : crc;
    if(refout == true)
        crc = bitsreverse(crc, width);
    crc ^= xorout;

    return (crc & ((2<<(width-1)) - 1));
}

unsigned char ChipLt8668SXC::lt8668sxc_get_crc(unsigned char *upgradeData, XBH_U32 len)
{
    CrcInfoTypeS type =
    {
        .Width = 8,
        .Poly  = 0x31,
        .CrcInit = 0,
        .XorOut = 0,
        .RefOut = false,
        .RefIn = false,
    };
    XBH_U32 crc_size = MAX_DATA_LENGTH - 1;
    unsigned char default_val = 0xFF;

    type.CrcInit = getcrc(type, upgradeData, len);

    crc_size -= len;
    while(crc_size--)
    {
        type.CrcInit = getcrc(type, &default_val, 1);
    }
    return type.CrcInit;
}

void ChipLt8668SXC::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32State = XBH_FALSE;
    XbhKernelMsgManager::getInstance()->sendMsgToKernel(0, 1, 0, 0, 0);
    usleep(1000 * 1000);//等待kernel不再操作外挂芯片的IIC
    mState = XBH_UPGRADE_RUNNING; //set task status
    //读chip ID
    getChipExist((XBH_BOOL*)&s32State);
    if(XBH_TRUE != s32State)
    { 
        goto endUpdate;
    }

    s32Ret = open_upgrade_file(mFileName);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("===> init fail, file : %s\n", mFileName);
        goto endUpdate;
    }

    //获取版本信息
    if(XBH_SUCCESS != checkVersion())
    { 
        XLOGE("\n===> get version fail!\n");
        goto endUpdate;
    }
    XLOGD("\n===> start update firmware !!!!!!!!!!! \n");
    //此芯片支持双分区固件，当主分区出现异常时，会加载backup分区的固件，因此升级时也需要升级两个分区
    //升级主分区固件
    if(XBH_SUCCESS == lt8668sxc_load_code_to_flash(FW_FLASH_ADDR))
    {
        //升级backup区的固件
        if(XBH_SUCCESS == lt8668sxc_load_code_to_flash(FW2_FLASH_ADDR))
        {
            XLOGD("\n===> update firmware sucess!!!!!! \n");
            resetChip(XBH_TRUE);
            mState = XBH_UPGRADE_SUCCESS;
            XbhKernelMsgManager::getInstance()->sendMsgToKernel(0, 0, 0, 0, 0);
            return;
        }
    }
endUpdate:
    XLOGE("\n===> update firmware fail!\n");
    resetChip(XBH_TRUE);
    mState = XBH_UPGRADE_FAILURE;
    XbhKernelMsgManager::getInstance()->sendMsgToKernel(0, 0, 0, 0, 0);
    return;
}
