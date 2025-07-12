
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipRts5450M"
#include "XbhLog.h"

#include <cutils/properties.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ChipRts5450M.h"
#include "XbhService.h"


XbhMutex                       ChipRts5450M::mLock;


#define FW_FLASH_ADDR               0x00000
#define FW2_FLASH_ADDR              0x20000
#define MAX_UX_FIRMWARE_LENGTH      (0x10000)

#define MAJOR_VERSION       0x01
#define MIDDLE_VERSION      0x00
#define MINOR_VERSION       0x02

#define MAX_DATA_LENGTH (192 * 1024)    //buffer最大长度，升级文件大小不超过192
#define MAX_KEY_LENGTH  1024           //最大key长度

static unsigned char g_data_buffer[MAX_DATA_LENGTH]={0};
static unsigned int  g_data_buffer_len = 0;
static unsigned char g_read_buffer[MAX_DATA_LENGTH]={0};
static unsigned int  g_read_buffer_len = 0;
static unsigned char g_rts5450_read_key[MAX_KEY_LENGTH] = {0}; //存放读取到的KEY
static unsigned char g_crc_val = 0;

//---------------- custom device interface start ----------------
/**
 * 复位当前转换芯片
 */
XBH_S32 ChipRts5450M::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (mRGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        usleep(50 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
        s32Ret = XBH_SUCCESS;
    }
    return s32Ret;
}

/**
 * 设置type-c功率
 */
XBH_S32 ChipRts5450M::setTypecCapacity(XBH_U8 value)
{
    XLOGD("ChipRts5450M setTypecCapacity Power Level is %d\n",value);
    XBH_S32 s32Ret = XBH_FAILURE;
    switch (value)
    {
        case XBH_PDCAPACITY_65W:// 65w
            s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, 0x08, 1 , sizeof(RTS5450_SET_PDO_65W), RTS5450_SET_PDO_65W);
           break;
        case XBH_PDCAPACITY_75W:// 75w
            s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, 0x08, 1 , sizeof(RTS5450_SET_PDO_75W), RTS5450_SET_PDO_75W);
            break;
        case XBH_PDCAPACITY_85W:// 85w
            s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, 0x08, 1 , sizeof(RTS5450_SET_PDO_85W), RTS5450_SET_PDO_85W);
            break;
        case XBH_PDCAPACITY_90W:// 90w
            s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, 0x08, 1 , sizeof(RTS5450_SET_PDO_90W), RTS5450_SET_PDO_90W);
            break;
        case XBH_PDCAPACITY_96W:// 96w
            s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, 0x08, 1 , sizeof(RTS5450_SET_PDO_96W), RTS5450_SET_PDO_96W);
            break;
        default:
            break;
    }
    XLOGD("ChipRts5450M s32Ret  is %d \n",s32Ret);
    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 ChipRts5450M::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(strVersion == XBH_NULL)
    {
        XLOGD("===> getFirmwareVersion strVersion is NULL");
        return XBH_FAILURE;
    }
    if (mState == RTS5450_RUNNING)
    {
        XLOGW("Warning: rts5450 is updating...");
        return XBH_FAILURE;
    }


    s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s vendor_cmd_enable fail\n",__FUNCTION__);
        return XBH_FAILURE;
    }

    unsigned char ic_status[23] = {0};
    int i = 0;
    XBH_U8 get_num = 0x14;

    s32Ret = i2c_data_write_nbytes(RTS5450_GET_IC_STATUS, 4, &get_num, 1);
    usleep(100);  //sleep 100us
    for(i = 0; i < 10; i++)
    {
        s32Ret = i2c_data_read(ic_status, 1);
        usleep(1000);  //sleep 1ms
        if(ic_status[0] == 0x01)
        {
            break;
        }
    }

    if(ic_status[0] == 0x01)
    {
        // 发两次，防止偶尔出现 read 80 获取到的返回值不对
        s32Ret = i2c_data_write_nbytes(RTS5450_GET_IC_STATUS, 4, &get_num, 1);
        for(i = 0; i < 10; i++)
        {
            s32Ret = i2c_data_read(ic_status, 1);
            usleep(1000);  //sleep 1ms
            if(ic_status[0] == 0x01)
            {
                break;
            }
        }
    }

    if(ic_status[0] == 0x01)
    {
        i2c_data_read(0x80, 1, ic_status, 22);
        sprintf(strVersion, "%02X%02X%02X", ic_status[3], ic_status[4], ic_status[5]);
        XLOGD("===> checkVersion version [%02X %02X %02X]!\n", ic_status[3], ic_status[4], ic_status[5]);

        int i = 0;
        for(i = 0; i < 21; i++)
        {
            XLOGD("===> get ic result %02X!\n", ic_status[i]);
        }
    }
    else
    {
        XLOGD("===> get ic status fail!\n");
        return XBH_FAILURE;
    }

    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 ChipRts5450M::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == RTS5450_RUNNING)
    {
        XLOGW("Warning: RTS5450 is updating...");
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
XBH_S32 ChipRts5450M::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

XBH_S32 ChipRts5450M::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipRts5450M::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 ChipRts5450M::i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipRts5450M::i2c_data_read(unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->readI2cData(mI2cNum, mI2cAddr, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipRts5450M::i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, u8RegAddr, 1, 1, &reg_value);
    return s32Ret;
}

XBH_S32 ChipRts5450M::i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("i2c_data_write_nbytes %u, ret %u\n", mI2cNum, mI2cAddr);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
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
unsigned int ChipRts5450M::bitsreverse(unsigned int inVal, unsigned char bits)
{
    unsigned int outVal = 0;
    unsigned char i;

    for(i=0; i<bits; i++)
    {
        if(inVal & (1 << i)) outVal |= 1 << (bits - 1 - i);
    }

    return outVal;
}

XBH_S32 ChipRts5450M::open_upgrade_file(const char *upgrade_file_path)
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
            // g_crc_val = rts5450_get_crc(g_data_buffer, g_data_buffer_len);
            // XLOGD("==> read upgrade file crc value:0x%02x\n",g_crc_val);
            s32Ret = XBH_SUCCESS;
        }
        else
            XLOGE("==> upgrade file size more than %d bytes.\n",MAX_DATA_LENGTH);
        fclose(fp);
    }

    if (g_data_buffer_len <= MAX_UX_FIRMWARE_LENGTH)
    {
        XLOGE("==> Please check if the firmware is correct.\n");
        s32Ret = XBH_FAILURE;
    }

    return s32Ret;
}


XBH_S32 ChipRts5450M::MP_SM_Status(XBH_U8 get_num, XBH_U8 get_result, XBH_U8* ic_status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int i = 0;

    s32Ret = i2c_data_write_nbytes(RTS5450_GET_IC_STATUS, 4, &get_num, 1);
    usleep(100);  //sleep 100us
    for(i = 0; i < 200; i++)
    {
        s32Ret = i2c_data_read(ic_status, 1);
        usleep(2*1000); //2ms
        if(ic_status[0] == 0x01)
        {
            break;
        }
        else if (ic_status[0] == 0x03)
        {
            XLOGD("===> get ic status 03 error!\n");
            return XBH_FAILURE;
        }
    }


    if(ic_status[0] == 0x01 && get_result == 1)
    {
        i2c_data_read(0x80, 1, ic_status, get_num+1);

        XLOGD("===> ic_status version[%02X %02X] status[%02X %02X]!\n", ic_status[3], ic_status[4], ic_status[1], ic_status[15]);

        int i = 0;
        XLOGD("===> get ic status !\n");
        for(i = 0; i <= get_num; i++) {
            XLOGD("%02X ", ic_status[i]);
        }
        XLOGD("<=== get ic status !\n");
        return XBH_SUCCESS;
    }
    else if(ic_status[0] == 0x01) {
        return XBH_SUCCESS;
    }
    else
    {
        XLOGD("===> get ic status timeout!\n");
        return XBH_FAILURE;
    }
}

XBH_S32 ChipRts5450M::MP_SM_Enable_VDCMD(XBH_U8 status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int i = 0;
    unsigned char result[2] = {0};
    s32Ret = i2c_data_write_nbytes(RTS5450_SET_VENDOR_CMD, 4, &status, 1);
    usleep(100);  //sleep 100us

    for(i = 0; i < 200; i++)
    {
        s32Ret = i2c_data_read(result, 1);
        XLOGD("===> ic MP_SM_Enable_VDCMD %02X!\n", result[0]);
        if(result[0] == 0x01)
        {
            usleep(1000); //1ms
            return XBH_SUCCESS;
        }
        else if (result[0] == 0x03)
        {
            return XBH_FAILURE;
        }
        usleep(2000); //2ms
    }
    return XBH_FAILURE;
}

XBH_S32 ChipRts5450M::MP_SM_FlashErase()
{

    unsigned char send_data[4] = {0x03, 0xda, 0x0b, 0x00};
    unsigned char result[2] = {0};

    XBH_S32 s32Ret = XBH_FAILURE;
    int i = 0;
    s32Ret = i2c_data_write_nbytes(0x03, 1, send_data, 4);
    usleep(100);  //sleep 100us

    for(i = 0; i < 200; i++)
    {
        s32Ret = i2c_data_read(result, 1);
        XLOGD("===> ic MP_SM_FlashErase %02X!\n", result[0]);
        if(result[0] == 0x01)
        {
            usleep(1000); //1ms
            return XBH_SUCCESS;
        }
        else if (result[0] == 0x03)
        {
            XLOGE("===> ic MP_SM_FlashErase 03!\n");
            return XBH_FAILURE;
        }
        usleep(2000); //2ms
    }
    XLOGE("===> ic MP_SM_FlashErase time out!\n");
    return XBH_FAILURE;
}

XBH_S32 ChipRts5450M::MP_SM_FlashWrite(XBH_U8 status, XBH_U8 run_brank)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 i = 0, j, send_len;
    XBH_U8 writeUnitLen = 29;
    XBH_U32 g_data_offset = 0;
    XBH_U32 addr_offset = 0;
    // XBH_U16 dw_flash_addr = (0x10-run_brank)*0x2000;

    // cmd(1) + sen_len(1) + addr(2) + count(1) + data(n<=29)
    unsigned char write_data[34] = {0};
    unsigned char result[2] = {0};
    if(run_brank == RTS5450_RUN_BANK0)
    {
        write_data[0] = SMBUS_WRITE_FLASH_128_192K;
    }
    else if (run_brank == RTS5450_RUN_BANK1)
    {
        write_data[0] = SMBUS_WRITE_FLASH_0_64K;
    }
    else
    {
        return XBH_FAILURE;
    }

    write_data[1] = 32;
    while(RTS5450_FLASH_SIZE - addr_offset > 0)
    {
        if((addr_offset&0XFFF) == 0xFF9)
        {
            writeUnitLen = 7;
            write_data[1] = 10;
            XLOGD("===> FlashWrite1 ADDR[%02X%02X] \n", (addr_offset>>8)&0xFF, addr_offset&0XFF);
        }
        else
        {
            writeUnitLen = 29;
            write_data[1] = 32;
        }
        write_data[2] = addr_offset&0XFF;
        write_data[3] = (addr_offset>>8)&0xFF;
        write_data[4] = writeUnitLen;

        memset(write_data+5, 0, writeUnitLen);
        memcpy(write_data+5, g_data_buffer + g_data_offset, writeUnitLen);
        send_len = (write_data[1]) + 2;

        s32Ret = i2c_data_write_nbytes(write_data[0], 1, write_data+1, send_len-1);
        usleep(100);  //sleep 100us
        for(j = 0; j < 200; j++)
        {
            s32Ret = i2c_data_read(result, 1);
            if(result[0]==1)
            {
                break;
            }
            usleep(2*1000); //2ms
        }
        if(result[0] != 1)
        {
            XLOGD("===> FlashWrite1 fail %d !!!\n", addr_offset); 
            return XBH_FAILURE;
        }

        g_data_offset += writeUnitLen;
        addr_offset += writeUnitLen;
        // XLOGD("===> FlashWrite1[%02X %02X %02X %02X %02X] %02X\n", 
        // write_data[0], write_data[1], write_data[2], write_data[3], write_data[4], result[0]);
        usleep(2*1000); //2ms
    }

    usleep(30*1000); //30ms
    writeUnitLen = 29;
    addr_offset = 0;
    write_data[1] = 32;
    if(run_brank == RTS5450_RUN_BANK0)
    {
        write_data[0] = SMBUS_WRITE_FLASH_192_256K;
    }
    else if (run_brank == RTS5450_RUN_BANK1)
    {
        write_data[0] = SMBUS_WRITE_FLASH_64_128K;
    }
    else
    {
        return XBH_FAILURE;
    }

    while(RTS5450_FLASH_SIZE - addr_offset > 0)
    {
        if((addr_offset&0XFFF) == 0xFF9)
        {
            writeUnitLen = 7;
            write_data[1] = 10;
            XLOGD("===> FlashWrite2 ADDR[%02X%02X] \n", (addr_offset>>8)&0xFF, addr_offset&0XFF);
        }
        else
        {
            writeUnitLen = 29;
            write_data[1] = 32;
        }
        write_data[2] = addr_offset&0XFF;
        write_data[3] = (addr_offset>>8)&0xff;
        write_data[4] = writeUnitLen;
        memset(write_data+5, 0, writeUnitLen);
        memcpy(write_data+5, g_data_buffer + g_data_offset, writeUnitLen);
        send_len = (write_data[1]) + 2;

        s32Ret = i2c_data_write_nbytes(write_data[0], 1, write_data+1, send_len-1);
        usleep(100);  //sleep 100us
        for(j = 0; j < 200; j++)
        {
            s32Ret = i2c_data_read(result, 1);
            if(result[0]==1)
            {
                break;
            }
            usleep(2*1000); //2ms
        }
        if(result[0] != 1)
        {
            XLOGD("===> FlashWrite2 fail %d !!!\n", addr_offset); 
            return XBH_FAILURE;
        }

        g_data_offset += writeUnitLen;
        addr_offset += writeUnitLen;
        // XLOGD("===> FlashWrite2[%02X %02X %02X %02X %02X %02X %02X %02X %02X] %02X\n", 
        // write_data[0], write_data[1], write_data[2], write_data[3], write_data[4], result[0]);
        usleep(2*1000); //2ms
    }

    return XBH_SUCCESS;
}

XBH_S32 ChipRts5450M::MP_SM_FlashRead(XBH_U8 status, XBH_U8 run_brank)
{
    XLOGD("===> ic MP_SM_FlashRead!\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 writeUnitLen = 0x1f;
    XBH_U32 g_data_offset = 0;
    XBH_U32 read_offset = 0;
    unsigned char send_data[5] = {SMBUS_READ_FLASH_128_192K, 0x03, 0x0, 0x0, writeUnitLen};
    unsigned char result[2] = {0};
    unsigned char result_data[0x21] = {0};

    int j = 0;
    int i = 0;

    if(run_brank == RTS5450_RUN_BANK0)
    {
        send_data[0] = SMBUS_READ_FLASH_128_192K;
    }
    else if (run_brank == RTS5450_RUN_BANK1)
    {
        send_data[0] = SMBUS_READ_FLASH_0_64K;
    }
    else
    {
        return XBH_FAILURE;
    }
    for(j = 0; j < RTS5450_FLASH_SIZE/writeUnitLen; j++)
    {
        s32Ret = i2c_data_write_nbytes(send_data[0], 1, send_data+1, 4);
        usleep(100);  //sleep 100us
        for(j = 0; j < 200; j++)
        {
            s32Ret = i2c_data_read(result, 1);
            if(result[0]==1)
            {
                break;
            }
            usleep(2*1000); //2ms
        }
        if(result[0] == 0x01)
        {
            memset(result_data, 0 , 0x20);
            usleep(5*1000); //5ms
            i2c_data_read(0x80, 1, result_data, 0x20);
            // XLOGD("===> ic      send[%02X %02X %02X %02X %02X]\n", send_data[0], send_data[1],
            //  send_data[2], send_data[3], send_data[4]);
            // XLOGD("===> ic flashRead[%02X %02X %02X %02X] %02X!\n", 
            // result_data[0], result_data[1], result_data[2], result_data[3], result[0]);

            if(result_data[0] == 3)
            {
                XLOGE("read2 80 = 3 !!!");
                return XBH_FAILURE;
            }
            else if (result_data[0] != send_data[4])
            {
                XLOGE("read2 [%02X, %02X, %02X] !!!", result_data[0], result_data[1], result_data[2]);
                return XBH_FAILURE;
            }

            int i = 0;
            for(i = 0; i < writeUnitLen; i++)
            {
                XLOGD("===> ic flash1Read[%d][%02X==%02X]!\n", read_offset, result_data[i+1], g_data_buffer[g_data_offset+i]);
                if(result_data[i+1] != g_data_buffer[g_data_offset+i])
                {
                    XLOGE("read %02x != ic write %02x !!!", result_data[i+1], g_data_buffer[g_data_offset+i]);
                    return XBH_FAILURE;
                }
            }

        }
        else
        {
            XLOGE("===> get ic status fail!\n");
            return XBH_FAILURE;
        }
        g_data_offset += writeUnitLen;
        read_offset += writeUnitLen;
        send_data[2] = read_offset&0xff;
        send_data[3] = (read_offset>>8)&0xff;

        usleep(5*1000); //5ms
    }

    if(RTS5450_FLASH_SIZE % writeUnitLen)
    {
        writeUnitLen = RTS5450_FLASH_SIZE % writeUnitLen;
        send_data[2] = read_offset&0XFF;
        send_data[3] = (read_offset>>8)&0xff;
        send_data[4] = writeUnitLen;


        s32Ret = i2c_data_write_nbytes(send_data[0], 1, send_data+1, 4);
        usleep(100);  //sleep 100us
        for(j = 0; j < 200; j++)
        {
            s32Ret = i2c_data_read(result, 1);
            if(result[0]==1)
            {
                break;
            }
            usleep(2*1000); //2ms
        }
        if(result[0] == 0x01)
        {
            memset(result_data, 0 , 0x20);
            usleep(5*1000); //5ms
            i2c_data_read(0x80, 1, result_data, 0x20);

            int i = 0;
            for(i = 0; i < writeUnitLen; i++)
            {
                XLOGD("===> ic flash1Read[%02X==%02X]!\n", result_data[i+1], g_data_buffer[g_data_offset+i]);
                if(result_data[i+1] != g_data_buffer[g_data_offset+i])
                {
                    XLOGE("read %02x != ic write %02x !!!", result_data[i+1], g_data_buffer[g_data_offset+i]);
                    return XBH_FAILURE;
                }
            }

        }
        else
        {
            XLOGE("===> ic read1 fail!\n");
            return XBH_FAILURE;
        }
        g_data_offset += writeUnitLen;
        read_offset += writeUnitLen;
    }

    usleep(20*1000); //20*1000
    writeUnitLen = 0x1f;
    read_offset = 0;
    send_data[2] = 0;
    send_data[3] = 0;
    send_data[4] = writeUnitLen;
    if(run_brank == RTS5450_RUN_BANK0)
    {
        send_data[0] = SMBUS_READ_FLASH_192_256K;
    }
    else if (run_brank == RTS5450_RUN_BANK1)
    {
        send_data[0] = SMBUS_READ_FLASH_64_128K;
    }
    else
    {
        return XBH_FAILURE;
    }
    for(j = 0; j < RTS5450_FLASH_SIZE/writeUnitLen; j++)
    {
        s32Ret = i2c_data_write_nbytes(send_data[0], 1, send_data+1, 4);
        usleep(100);  //sleep 100us
        for(j = 0; j < 200; j++)
        {
            s32Ret = i2c_data_read(result, 1);
            if(result[0]==1)
            {
                break;
            }
            usleep(2*1000); //2ms
        }

        if(result[0] == 0x01)
        {
            memset(result_data, 0 , 0x20);
            usleep(2*1000); //2ms
            i2c_data_read(0x80, 1, result_data, 0x20);
            // XLOGD("===> ic      send[%02X %02X %02X %02X %02X]\n", send_data[0], send_data[1],
            //  send_data[2], send_data[3], send_data[4]);
            // XLOGD("===> ic flashRead[%02X %02X %02X %02X] %02X!\n", 
            // result_data[0], result_data[1], result_data[2], result_data[3], result[0]);

            if(result_data[0] == 3)
            {
                XLOGE("read2 80 = 3 !!!");
                return XBH_FAILURE;
            }
            else if (result_data[0] != send_data[4])
            {
                XLOGE("read2 [%02X, %02X, %02X] !!!", result_data[0], result_data[1], result_data[2]);
                return XBH_FAILURE;
            }

            int i = 0;
            for(i = 0; i < writeUnitLen; i++)
            {
                XLOGD("===> ic flash2Read[[%d]][%02X==%02X]!\n", read_offset, result_data[i+1], g_data_buffer[g_data_offset+i]);
                if(result_data[i+1] != g_data_buffer[g_data_offset+i])
                {
                    XLOGE("read2 %02x != ic write %02x !!!", result_data[i+1], g_data_buffer[g_data_offset+i]);
                    return XBH_FAILURE;
                }
            }

        }
        else
        {
            XLOGE("===> get flashRead %02x fail!\n", SMBUS_READ_FLASH_192_256K);
            return XBH_FAILURE;
        }
        g_data_offset += writeUnitLen;
        read_offset += writeUnitLen;
        send_data[2] = read_offset&0xff;
        send_data[3] = (read_offset>>8)&0xff;

        usleep(5*1000); //5ms
    }

    return XBH_SUCCESS;
}

XBH_S32 ChipRts5450M::MP_SM_Validation()
{
    unsigned char send_data[2] = {0x01, 0x01};
    unsigned char result[2] = {0};
    XBH_S32 s32Ret = XBH_FAILURE;
    int i = 0;

    s32Ret = i2c_data_write_nbytes(0x16, 1, send_data, 2);
    usleep(100);  //sleep 100us
    for(i = 0; i < 100; i++)
    {
        s32Ret = i2c_data_read(result, 1);
        usleep(1000);  //sleep 1ms
        if(result[0] == 1)
        {
            break;
        }
        else if (result[0] == 0x03)
        {
            return XBH_FAILURE;
        }
    }
    XLOGD("===> ic MP_SM_Validation %02X!\n", result[0]);

    return result[0] == 1 ? XBH_SUCCESS: XBH_FAILURE;
}

XBH_S32 ChipRts5450M::MP_SM_Reset2Flash()
{
    unsigned char send_data[4] = {0x03, 0xda, 0x0b, 0x01};
    unsigned char result[2] = {0};
    int i = 0;
    XBH_S32 s32Ret = XBH_FAILURE;

    s32Ret = i2c_data_write_nbytes(0x05, 1, send_data, 4);
    usleep(100);  //sleep 100us
    for(i = 0; i < 100; i++)
    {
        s32Ret = i2c_data_read(result, 1);
        usleep(1000);  //sleep 1ms
        if(result[0] == 0x01)
        {
            break;
        }
        else if (result[0] == 0x03)
        {
            return XBH_FAILURE;
        }
    }

    return XBH_SUCCESS;
}

void ChipRts5450M::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    mState = RTS5450_RUNNING; //set task status

    s32Ret = open_upgrade_file(mFileName);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s init fail, file:%s\n",__FUNCTION__, mFileName);
        mState = RTS5450_FAILURE;
        return;
    }

    XLOGD("===> %s SMBUS_ENABLE\n",__FUNCTION__);
    s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s SMBUS_ENABLE fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }
    // s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s vendor_cmd_enable2 fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }

    // 字节15表示当前运行的bank, 如果值为0x10，表示在bank1运行，需要在bank0编程
    // 如果ic_status 为0，表示处于ROM，需要擦除bank0/bank1, 参考附录A
    // 如果ic_status 为1，表示处于MCM
    unsigned char ic_status[32] = {0};
    XLOGD("===> %s get_ic_status\n",__FUNCTION__);
    s32Ret = MP_SM_Status(0x14, 1,ic_status);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    // memset(ic_status, 0, 32);
    // s32Ret = MP_SM_Status(0x1f, 1,ic_status);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }

    // s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s vendor_cmd_enable2 fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }

    // memset(ic_status, 0, 32);
    // s32Ret = MP_SM_Status(0x1f, 1,ic_status);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }
    // memset(ic_status, 0, 32);
    // s32Ret = MP_SM_Status(0x1f, 1,ic_status);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }
    // memset(ic_status, 0, 32);
    // s32Ret = MP_SM_Status(0x1f, 1,ic_status);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }
    memset(ic_status, 0, 32);
    s32Ret = MP_SM_Status(0x14, 1,ic_status);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    usleep(1000 * 1000); //1s

    XLOGD("===> %s flash_enable\n",__FUNCTION__);
    s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE | RTS5450_VDCMD_FLASH_ENABLE);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s flash_enable fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    // if(ic_status[1] == RTS5450_ROM_CODE)
    // {
    //     s32Ret = MP_SM_FlashErase();
    //     XLOGD("===> %s smbus_erase_flash\n",__FUNCTION__);
    //     if(XBH_SUCCESS != s32Ret)
    //     {
    //         XLOGE("\n===> %s flash_access_enable fail\n",__FUNCTION__);
    //         mState = RTS5450_FAILURE;
    //         return;
    //     }
    //     // 调用擦除分区代码
    // }

    XLOGD("===> %s MP_SM_FlashWrite\n",__FUNCTION__);
    s32Ret = MP_SM_FlashWrite(ic_status[1], ic_status[15]);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s MP_SM_FlashWrite fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    XLOGD("===> %s flash_access_disable\n",__FUNCTION__);
    s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s flash_access_disable fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    if(ic_status[1] == RTS5450_ROM_CODE)
    {
        // // read buff 对比验证
        // s32Ret = MP_SM_FlashRead(ic_status[1], ic_status[15]);
        // if(XBH_SUCCESS != s32Ret)
        // {
        //     XLOGE("\n===> %s ISP_VALIDATION fail\n",__FUNCTION__);
        //     mState = RTS5450_FAILURE;
        //     return;
        // }
    }
    else
    {
        // SMBUS_ISP_VALIDATION验证
        s32Ret = MP_SM_Validation();
        if(XBH_SUCCESS != s32Ret)
        {
            XLOGE("\n===> %s ISP_VALIDATION fail\n",__FUNCTION__);
            mState = RTS5450_FAILURE;
            return;
        }
    }

    // XLOGD("===> %s get_ic_status\n",__FUNCTION__);
    // memset(ic_status, 0, 32);
    // s32Ret = MP_SM_Status(0x14, 1, ic_status);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }

    MP_SM_Reset2Flash();
    usleep(5000 * 1000); //5s


    XLOGD("===> %s flash_access_disable\n",__FUNCTION__);
    s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_DISABLE);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s flash_access_disable fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    usleep(300 * 1000); //300ms
    XLOGD("===> %s flash_access_disable\n",__FUNCTION__);
    s32Ret = MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("\n===> %s flash_access_disable fail\n",__FUNCTION__);
        mState = RTS5450_FAILURE;
        return;
    }

    // usleep(300 * 1000); //300ms
    // XLOGD("===> %s get_ic_status\n",__FUNCTION__);
    // memset(ic_status, 0, 32);
    // s32Ret = MP_SM_Status(0x1f, 1, ic_status);
    // if(XBH_SUCCESS != s32Ret)
    // {
    //     XLOGE("\n===> %s get_ic_status fail\n",__FUNCTION__);
    //     mState = RTS5450_FAILURE;
    //     return;
    // }

    XLOGD("===> %s upgradeFirmware success\n",__FUNCTION__);
    mState = RTS5450_SUCCESS;
}

//---------------- custom device interface end ----------------

ChipRts5450M::ChipRts5450M(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mState = RTS5450_IDLE;
    mForceUpgrade = XBH_FALSE;

    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    //power on
    if (mPGpio != -1) {
        XbhService::getModuleInterface()->setGpioOutputValue(mPGpio, mPLevel);
        usleep(50 * 1000);
    }
    //reset
    if (mRGpio != -1) {
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        usleep(50 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
    }
    MP_SM_Enable_VDCMD(RTS5450_VDCMD_SMBUS_ENABLE);//初始化过程后对芯片发送指令，使能SUMBUS协议，方便能及时响应动态功率调正
    XLOGD(" end ");
}

ChipRts5450M::~ChipRts5450M()
{

}
