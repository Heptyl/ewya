#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipTas5805m_Subwoofer"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipTas5805m_Subwoofer.h"
#include <iostream>
#include <fstream>
XbhMutex                 ChipTas5805m_Subwoofer::mLock;

/* register address */
#define REG_PAGE		0x00
#define REG_DEVICE_STATE	0x03
#define REG_STATE_REPORT	0x16
#define REG_GLOBAL_FAULT1	0x17
#define REG_GLOBAL_FAULT2	0x18
#define REG_GLOBAL_FAULT3	0x19

/* DEVICE_STATE register values */
#define DEVICE_STATE_DEEP_SLEEP	0x00
#define DEVICE_STATE_SLEEP	0x01
#define DEVICE_STATE_HIZ	0x02
#define DEVICE_STATE_PLAY	0x03

#define DEVICE_STATE_MUTE	0x02

#define REG_DEVICE_AVC	      0x05
#define DEVICE_STATE_AVC      0x46
#define DEVICE_STATE_UNAVC    0xC6

#define REG_PAGE_EQ1             0x06
#define REG_PAGE_EQ2             0x07
#define DEVICE_STATE_UNEQ        0x66
#define REG_ADRESS_UNEQ1         0xB0
#define REG_ADRESS_UNEQ2         0xE0

//tas5805 eq parameter
#define TAS5805_I2C_EQ_ADDR             0x66
#define TAS5805_EQ_OFF                  0x87
#define TAS5805_EQ_ON                   0x86

/**
 * 复位功放芯片
*/
XBH_S32 ChipTas5805m_Subwoofer::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

/**
 * 设置speaker 软mute的状态
*/
XBH_S32 ChipTas5805m_Subwoofer::setMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    data[0] =mute ? DEVICE_STATE_MUTE : DEVICE_STATE_PLAY;
    XLOGD("setMute addr: %#x,MUTE :%d\n",mI2cAddr,mute);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
    return  s32Ret;
}

/**
 * 获取speaker mute的状态
*/
XBH_S32 ChipTas5805m_Subwoofer::getMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
    if (data[0] == DEVICE_STATE_PLAY)
    {
        *mute = XBH_FALSE;
    }
    else
    {
        *mute = XBH_TRUE;
    }
    return  s32Ret;
}

/**
 * 设置功放输出的增益值
*/
XBH_S32 ChipTas5805m_Subwoofer::setGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 ChipTas5805m_Subwoofer::getGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取amp是否初始化完成
*/
XBH_S32 ChipTas5805m_Subwoofer::getChipInitDone(XBH_BOOL* enable)
{
    if(m_bInitDone)
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取chip是否存在
*/
XBH_S32 ChipTas5805m_Subwoofer::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipTas5805m_Subwoofer::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 进行固件升级
 */
XBH_S32 ChipTas5805m_Subwoofer::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 获取固件升级时的状态
 */
XBH_S32 ChipTas5805m_Subwoofer::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::Tas5805m_Write(XBH_U8 *buf, XBH_U8 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::Tas5805m_Read(XBH_U8 *buf, XBH_U8 ctrl)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::setAvcEnable(XBH_BOOL isAvcEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::getAvcEnable(XBH_BOOL *isAvcEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::setDirectPass(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;   
}

XBH_S32 ChipTas5805m_Subwoofer::getDirectPass(XBH_BOOL* enable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret; 
}

XBH_S32 ChipTas5805m_Subwoofer::writeRegMap(amp5805RegMap *reg_map, XBH_S32 iicBus, XBH_S32 iicAddr)
{
    XBH_S32 ret = XBH_FAILURE;
    return XBH_SUCCESS;
}

XBH_S32 ChipTas5805m_Subwoofer::ParseIniConfig(const char *section, char *fileBuf, amp5805RegMap *pRegMap, XBH_U32 maxRegSize)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return XBH_FAILURE;
}


XBH_S32 ChipTas5805m_Subwoofer::LoadIniFileBuf(const XBH_CHAR *fileName, XBH_CHAR *buf, XBH_S32 *fileSize, XBH_S32 bufSize)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::WriteIniConfig(const XBH_CHAR *fileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Subwoofer::setEqEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    data[0] = (enable ? TAS5805_EQ_ON : TAS5805_EQ_OFF);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, TAS5805_I2C_EQ_ADDR, 0x01, 0x01, data);

    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" setEqEnable setI2cData error!!");
    }
    XLOGD(" setEqEnable:%d,s32Ret = %d,data[0]=%#x", enable,s32Ret,data[0]);
    return s32Ret;   
}

XBH_S32 ChipTas5805m_Subwoofer::getEqEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};

    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, TAS5805_I2C_EQ_ADDR, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" getEqEnable getIICData error!!");
    }
    if(data[0] != TAS5805_EQ_OFF)
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    XLOGD(" getEqEnable:%d,s32Ret = %d,data[0]=%#x", *enable,s32Ret,data[0]);
    return s32Ret;   
}

XBH_S32 ChipTas5805m_Subwoofer::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" chip address = %#x is not init!!!!",mI2cAddr);
        return s32Ret;
    }
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipTas5805m_Subwoofer::ChipTas5805m_Subwoofer(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mute_gpio, XBH_S32 mute_level)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    mPgpio = pGpio;
    mPLevel = pLevel;
    mRgpio = rGpio;
    mRLevel = rLevel;
    mMute_gpio = mute_gpio;
    mMute_level = mute_level;
    init();
    XLOGD(" end ");
}

ChipTas5805m_Subwoofer::~ChipTas5805m_Subwoofer()
{

}

