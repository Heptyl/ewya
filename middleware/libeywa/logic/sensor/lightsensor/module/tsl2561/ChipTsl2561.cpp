#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipTsl2561"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipTsl2561.h"

XbhMutex                 ChipTsl2561::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipTsl2561::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = TSL2561_Write(0xA0, 0x03);
    if (s32Ret < 0) {
        XLOGE("[TSL2561] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipTsl2561::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[2];
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    TSL2561_Read(TSL2561_REG_ALS_DATA_L, &val[0]);
    TSL2561_Read(TSL2561_REG_ALS_DATA_H, &val[1]);
    *s32State = ((val[1] << 8) | val[0]);

    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipTsl2561::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipTsl2561::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipTsl2561::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipTsl2561::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipTsl2561::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipTsl2561::TSL2561_Write(XBH_U8 reg, XBH_U8 data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // XLOGD("[%s][%d]reg: 0x%x,  data: 0x%x\n",__func__,__LINE__, reg, data);
    if(mI2cBus == -1)
    {
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, reg, 1, 1, &data);
    return s32Ret;
}

XBH_S32 ChipTsl2561::TSL2561_Read(XBH_U8 reg, XBH_U8* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // XLOGD("[%s][%d]reg: 0x%x,  data: 0x%x\n",__func__,__LINE__, reg, data);
    if(mI2cBus == -1)
    {
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, reg, 1, 1, data);
    return s32Ret;
}

XBH_S32 ChipTsl2561::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    // Enable AEN and PON
    s32Ret = TSL2561_Write(TSL2561_CMD_REGISTER, 0x03);
    if (s32Ret < 0) {
        XLOGE("[TSL2561] Enable AEN and PON failed!\n");
        return XBH_FAILURE;
    }
    XLOGD("lightsensor TSL2561 init success !\n");
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipTsl2561::ChipTsl2561(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x49;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipTsl2561::~ChipTsl2561()
{

}
