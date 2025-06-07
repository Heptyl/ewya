#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipStk3210"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipStk3210.h"

XbhMutex                 ChipStk3210::mLock;

/**
 * 复位芯片
*/
XBH_S32 ChipStk3210::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data = 0x03;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, 0x80, 1, 1, &data);
    data = 0x3f;
    s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, 0x00, 1, 1, &data);
    if (s32Ret < 0)
    {
        XLOGE(" reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipStk3210::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[2];
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, 0x13, 1, 2, val);
    if(s32Ret < 0)
    {
        XLOGE(" read reg 0x13 error !!!");
        *s32State = -1;
        return  XBH_SUCCESS;
    }
    //The most significant bit (MSB) is accessed at register 13h, and the least significant bit (LSB) is accessed at register 14h
    *s32State = ((val[0] << 8) | val[1]);

    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipStk3210::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *bEnable = XBH_FALSE;
        return s32Ret;
    }
    *bEnable = XBH_TRUE;
    return  XBH_SUCCESS;
}

XBH_S32 ChipStk3210::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipStk3210::getFirmwareVersion(XBH_CHAR* strVersion)
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
 * 进行固件升级
 */
XBH_S32 ChipStk3210::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
 * 获取固件升级时的状态
 */
XBH_S32 ChipStk3210::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipStk3210::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 val[2];
    XBH_U8 data = 0x3f;
    //read product id
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, 0x3e, 1, 1, val);
    if (s32Ret < 0) {
        XLOGE("[TSL2561] Enable AEN and PON failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);
    //init ic
    s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, 0x00, 1, 1, &data);
    if (s32Ret < 0)
    {
        XLOGE(" init failed!\n");
        return XBH_FAILURE;
    }
    XLOGD("lightsensor stk3120 init success !\n");
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipStk3210::ChipStk3210(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x48;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipStk3210::~ChipStk3210()
{

}
