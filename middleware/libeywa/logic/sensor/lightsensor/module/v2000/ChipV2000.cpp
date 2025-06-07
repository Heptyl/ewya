#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipV2000"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipV2000.h"

XbhMutex                 ChipV2000::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipV2000::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = V2000_Write(V2000_REG_SWRST, 0x01);
    if (s32Ret < 0) {
        XLOGE("[V2000] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipV2000::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[2];
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    V2000_Read(V2000_REG_ALS_DATA_L, &val[0]);
    V2000_Read(V2000_REG_ALS_DATA_H, &val[1]);
    *s32State = ((val[1] << 8) | val[0]);

    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipV2000::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipV2000::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipV2000::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipV2000::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipV2000::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipV2000::V2000_Write(XBH_U8 reg, XBH_U8 data)
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

XBH_S32 ChipV2000::V2000_Read(XBH_U8 reg, XBH_U8* data)
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

XBH_S32 ChipV2000::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 val[1];
    // check product id
    V2000_Read(V2000_REG_PRODUCT_ID, &val[0]);

    if(V2000_PRODUCT_ID != val[0])
    {
        XLOGE("[V2000] not found!\n");
        return XBH_FAILURE;
    }

    // Write any data to this register will reset the chip.
    s32Ret = V2000_Write(V2000_REG_SWRST, 0x01);
    if (s32Ret < 0)
    {
        XLOGE("[V2000] reset failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);

    // enable als
    s32Ret = V2000_Write(V2000_REG_SYSTEM_CTRL, V2000_CONF_EN_ALS);
    if (s32Ret < 0)
    {
        XLOGE("[V2000] enable als failed!\n");
        return XBH_FAILURE;
    }
    m_bInitDone = XBH_TRUE;
    XLOGD("lightsensor V2000 init success !\n");
    return XBH_SUCCESS;
}

ChipV2000::ChipV2000(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x57;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipV2000::~ChipV2000()
{

}
