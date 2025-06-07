#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipJSA1141"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipJSA1141.h"

XbhMutex                 ChipJSA1141::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipJSA1141::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = JSA1141_Write(JSA1141_REG_SYSTEM_CTRL, JSA1141_CONF_SWRST);
    if (s32Ret < 0) {
        XLOGE("[JSA1141] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipJSA1141::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[2];
    XBH_U8 data_status = 0;

    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        init();
        *s32State = last_value;
        return s32Ret;
    }

    JSA1141_Read(JSA1141_REG_DATA_STAT, &data_status);
    usleep(30);
    if (data_status)
    {
        JSA1141_Read(JSA1141_REG_ALS_DATA_L, &val[0]);
        JSA1141_Read(JSA1141_REG_ALS_DATA_H, &val[1]);
        *s32State = ((val[1] << 8) | val[0]);
        last_value = *s32State;
        gettimeofday(&start, NULL);
    }
    else
    {
        *s32State = last_value;
    }
    gettimeofday(&end, NULL);

    if ((start.tv_sec > end.tv_sec) || ((end.tv_sec - start.tv_sec) > 10))
    {
        init();
    }

    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipJSA1141::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipJSA1141::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipJSA1141::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipJSA1141::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipJSA1141::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipJSA1141::JSA1141_Write(XBH_U8 reg, XBH_U8 data)
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

XBH_S32 ChipJSA1141::JSA1141_Read(XBH_U8 reg, XBH_U8* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mI2cBus == -1)
    {
        return s32Ret;
    }
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, reg, 1, 1, data);
    //XLOGD("[%s][%d]reg: 0x%x,  data: 0x%x\n",__func__,__LINE__, reg, *data);
    return s32Ret;
}

XBH_S32 ChipJSA1141::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 val[2];
    // check product id
    JSA1141_Read(JSA1141_REG_PRODUCT_ID_L, &val[0]);
    JSA1141_Read(JSA1141_REG_PRODUCT_ID_H, &val[1]);

    if(JSA1141_PRODUCT_ID != ((val[1] << 8)|val[0])) {
        XLOGE("[JSA1141] not found!\n");
        return XBH_FAILURE;
    }

    // reset
    s32Ret = JSA1141_Write(JSA1141_REG_SYSTEM_CTRL, JSA1141_CONF_SWRST);
    if (s32Ret < 0) {
        XLOGE("[JSA1141] reset failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);

    // gain
    s32Ret = JSA1141_Write(JSA1141_REG_ALS_GAIN, 0x08);
    if (s32Ret < 0) {
        XLOGE("[JSA1141] set gain failed!\n");
        return XBH_FAILURE;
    }

    // integration time
    s32Ret = JSA1141_Write(JSA1141_REG_ALS_TIME, 0xf3);
    if (s32Ret < 0) {
        XLOGE("[JSA1141] set int time failed!\n");
        return XBH_FAILURE;
    }

    // enable als
    s32Ret = JSA1141_Write(JSA1141_REG_SYSTEM_CTRL, 0x01);
    if (s32Ret < 0) {
        XLOGE("[JSA1141] enable als failed!\n");
        return XBH_FAILURE;
    }
    m_bInitDone = XBH_TRUE;
    XLOGD("lightsensor JSA1141 init success !\n");
    return XBH_SUCCESS;
}

ChipJSA1141::ChipJSA1141(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x38;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipJSA1141::~ChipJSA1141()
{

}


