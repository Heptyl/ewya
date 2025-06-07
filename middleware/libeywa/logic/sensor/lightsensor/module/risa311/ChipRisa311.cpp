#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipRisa311"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipRisa311.h"

XbhMutex                 ChipRisa311::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipRisa311::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = RISA311_Write(RISA311_REG_SYSTEM_CTRL, 0x20);
    if (s32Ret < 0) {
        XLOGE("[RISA311] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipRisa311::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value0 = 0;
    XBH_S32 value1 = 0;
    XBH_U8 val[4];
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    RISA311_Read(RISA311_REG_ALS_DATA0_L, &val[0]);
    RISA311_Read(RISA311_REG_ALS_DATA0_H, &val[1]);
    RISA311_Read(RISA311_REG_ALS_DATA1_L, &val[2]);
    RISA311_Read(RISA311_REG_ALS_DATA1_H, &val[3]);
    value0 = (((val[1] & 0xff) << 8) | val[0]);
    value1 = (((val[3] & 0xff) << 8) | val[2]);
    *s32State = (value0 + value1) / 2;

    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipRisa311::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipRisa311::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipRisa311::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipRisa311::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipRisa311::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipRisa311::RISA311_Write(XBH_U8 reg, XBH_U8 data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // XLOGD("[%s][%d]reg: 0x%x,  data: 0x%x\n",__func__,__LINE__, reg, data);
    if(mI2cBus == -1)
    {
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus,mI2cAddr, reg, 1, 1, &data);
    return s32Ret;
}

XBH_S32 ChipRisa311::RISA311_Read(XBH_U8 reg, XBH_U8* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // XLOGD("[%s][%d]reg: 0x%x,  data: 0x%x\n",__func__,__LINE__, reg, data);
    if(mI2cBus == -1)
    {
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus,mI2cAddr, reg, 1, 1, data);
    return s32Ret;
}

XBH_S32 ChipRisa311::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 val[1];

    // The manufacturer ID reads A1h or A5h.
    RISA311_Read(RISA311_REG_PRODUCT_ID, &val[0]);

    if(val[0] != RISA311_PRODUCT_ID_1 && val[0] != RISA311_PRODUCT_ID_2) {
        XLOGE("[RISA311] not found!\n");
        return XBH_FAILURE;
    }

    // Set Channel 0 Wait Time Regist
    s32Ret = RISA311_Write(RISA311_REG_ATIME0, 0x01);
    if (s32Ret < 0) {
        XLOGE("[RISA311] reset failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);

    s32Ret = RISA311_Write(RISA311_REG_ATIME1, 0x01);
    if (s32Ret < 0) {
        XLOGE("[RISA311] reset failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);

    // Set Wait Time Regist
    s32Ret = RISA311_Write(RISA311_REG_WAIT_TIME, 0x01);
    if (s32Ret < 0) {
        XLOGE("[RISA311] enable als failed!\n");
        return XBH_FAILURE;
    }

    // Turn On A311Axx internal RC oscillator
    s32Ret = RISA311_Write(RISA311_REG_SYSTEM_CTRL, 0x01);
    if (s32Ret < 0) {
        XLOGE("[RISA311] enable als failed!\n");
        return XBH_FAILURE;
    }

    // Set Work Mode
    s32Ret = RISA311_Write(RISA311_REG_SYSTEM_CTRL, 0x03);
    if (s32Ret < 0) {
        XLOGE("[RISA311] enable als failed!\n");
        return XBH_FAILURE;
    }
    XLOGD("lightsensor RISA311 init success !\n");
    m_bInitDone = XBH_TRUE;
    return XBH_SUCCESS;
}

ChipRisa311::ChipRisa311(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
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

ChipRisa311::~ChipRisa311()
{

}
