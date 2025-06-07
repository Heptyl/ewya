
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipCS5803AQ"
#include "XbhLog.h"

#include <cutils/properties.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ChipCs5803aq.h"
#include "XbhService.h"

XbhMutex                       ChipCS5803AQ::mLock;

#define I2C_SLAVE_ADDR      0x41
#define MAX_DATA_LENGTH (1024 * 1024)    //buffer最大长度，升级文件大小不超过1M

static unsigned char g_data_buffer[MAX_DATA_LENGTH]={0};
static unsigned int  g_data_buffer_len = 0;
static unsigned int  g_cs5803_flash_addr = 0;
static unsigned int  g_cs5803_count = 0;
static unsigned int  g_download_check_count = 0;

XBH_S32 ChipCS5803AQ::i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, u8RegAddr, 1, 1, &reg_value);
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

/**
 * 复位当前转换芯片
 */
XBH_S32 ChipCS5803AQ::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // if (mRGpio != -1) {
    //     XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
    //     usleep(10 * 1000);
    //     XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
    //     usleep(10 * 1000);
    //     XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);

    //     usleep(2000*1000);  //保险起见，预留2s等ic稳定
    //     s32Ret = XBH_SUCCESS;
    // }
    return s32Ret;
}


XBH_S32 ChipCS5803AQ::open_upgrade_file(const char *upgrade_file_path)
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

/**
 * 获取固件版本
 */
XBH_S32 ChipCS5803AQ::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("===> ChipCS5803AQ getFirmwareVersion");

    if (mState == CS5803_RUNNING)
    {
        XLOGE("Warning: CS5803AQ is updating...");
        return XBH_FAILURE;
    }

    XBH_U8 RDat, ver_l, ver_h;
    i2c_data_read(CS5803AQ_GET_IC_ID, 3, &RDat, 1);
    if(RDat != 0x58)
    {
        XLOGE("===> getFirmwareVersion strVersion is NULL");
        return XBH_FAILURE;
    }

    i2c_data_read(CS5803AQ_GET_IC_VER_L, 3, &ver_l, 1);
    i2c_data_read(CS5803AQ_GET_IC_VER_H, 3, &ver_h, 1);

    XLOGD("===> CS5803AQ version = [%02X %02X]!\n",ver_h, ver_l);
    sprintf(strVersion,"%02X%02X", ver_h, ver_l);
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 ChipCS5803AQ::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == CS5803_RUNNING)
    {
        XLOGW("Warning: cs5803 is updating...");
        return XBH_FAILURE;
    }

    if (strFilePath == XBH_NULL)
    {
        XLOGE("Error: fileName is XBH_NULL");
        return XBH_FAILURE;
    }

    // //set force upgrade flag
    // mForceUpgrade = bForceUpgrade;
    // //mI2cNum = I2C_CHANNEL;

    // //set upgrade file name
    // memset(mFileName, 0x00, sizeof(mFileName));
    // if (strlen(strFilePath) >= sizeof(mFileName))
    // {
    //     XLOGE("Error: fileName is too long: %d", strlen(strFilePath));
    //     return XBH_FAILURE;
    // }
    // strncpy(mFileName, strFilePath, strlen(strFilePath));

    // XbhMWThread::run(XbhMWThread::ONCE);
    return XBH_FAILURE;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 ChipCS5803AQ::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGE("%s %d \n", __func__, __LINE__);

    return s32Ret;
}

void ChipCS5803AQ::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32State = XBH_FALSE;

    //set task status
    mState = CS5803_RUNNING;

    // //读chip ID
    // s32Ret = getChipExist((XBH_BOOL*)&s32State);
    // if(XBH_TRUE != s32State){ //读chip ID
    //     XLOGE("\n===> %s get chip id fail!\n", __FUNCTION__);
    //     mState = CS5803_FAILURE;
    //     return;
    // }

    // s32Ret = open_upgrade_file(mFileName);
    // if(XBH_SUCCESS != s32Ret){
    //     XLOGE("\n===> %s init fail, file:%s\n", __FUNCTION__, mFileName);
    //     mState = CS5803_FAILURE;
    //     return;
    // }

    // //获取版本信息
    // s32Ret = checkVersion();
    // if(XBH_SUCCESS != s32Ret){
    //     XLOGE("\n===> %s get version fail!\n", __FUNCTION__);
    //     mState = CS5803_FAILURE;
    //     return;
    // }

    // //升级固件
    // s32Ret = cs5803_load_code_to_flash();
    // if(XBH_SUCCESS != s32Ret){
    //     XLOGE("\n===> %s load code to flash fail!\n", __FUNCTION__);
    //     resetChip();
    //     mState = CS5803_FAILURE;
    //     return;
    // }

    // resetChip();
    mState = CS5803_SUCCESS;

}

//---------------- custom device interface end ----------------

ChipCS5803AQ::ChipCS5803AQ(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mState = CS5803_IDLE;
    mForceUpgrade = XBH_FALSE;

    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    XLOGD(" end ");
}

ChipCS5803AQ::~ChipCS5803AQ()
{

}