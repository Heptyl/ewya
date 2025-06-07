#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipALS199"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipALS199.h"

XbhMutex                 ChipALS199::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipALS199::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = ALS199_Write(ALS199_REG_SYSTEM_CTRL, ALS199_CONF_SWRST);
    if (s32Ret < 0) {
        XLOGE("[ALS199] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipALS199::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[4] = {0};
    if(!m_bInitDone)
    {
        XLOGE("chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    ALS199_Read(0x88, &val[0]);
    ALS199_Read(0x89, &val[1]);
    ALS199_Read(0x8A, &val[2]);
    ALS199_Read(0x8B, &val[3]);
    *s32State = ((val[1] << 8) | val[0]);
    return  XBH_SUCCESS;
}

/** 
 * 获取lightsensor是否存在
 */
XBH_S32 ChipALS199::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipALS199::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipALS199::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipALS199::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipALS199::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipALS199::ALS199_Write(XBH_U8 reg, XBH_U8 data)
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

XBH_S32 ChipALS199::ALS199_Read(XBH_U8 reg, XBH_U8* data)
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

XBH_S32 ChipALS199::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    usleep(100 * 1000);

    s32Ret = ALS199_Write(0x80, 0x01);
    if (s32Ret < 0) {
        XLOGE("[ALS199] enable failed!\n");
        return XBH_FAILURE;
    }
    usleep(10 * 1000);
    s32Ret = ALS199_Write(0x85, 0x1b);
    if (s32Ret < 0) {
        XLOGE("[ALS199] setting failed!\n");
        return XBH_FAILURE;
    }
    
    m_bInitDone = XBH_TRUE;
    XLOGD("lightsensor ALS199 init success !\n");
    return s32Ret;
}

ChipALS199::ChipALS199(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x29;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipALS199::~ChipALS199()
{

}


