#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipLtrx140a"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipLtrx140a.h"
#include <cutils/properties.h>

XbhMutex                 ChipLtrx140a::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipLtrx140a::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = Ltrx140a_Write(0x81, 0x01);
    if (s32Ret < 0) {
        XLOGE("[Ltrx140a] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipLtrx140a::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[2];
    XBH_U8 status;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    Ltrx140a_Read(LTRX140A_ALS_STATUS_REG,&status);
    if(!((status & 0x40) == 0x00))
    {
        XLOGE(" read error !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    Ltrx140a_Read(Ltrx140a_REG_ALS_DATA_L, &val[0]);
    Ltrx140a_Read(Ltrx140a_REG_ALS_DATA_H, &val[1]);
    *s32State = ((val[1] << 8) | val[0]);

    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipLtrx140a::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipLtrx140a::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipLtrx140a::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipLtrx140a::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipLtrx140a::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipLtrx140a::Ltrx140a_Write(XBH_U8 reg, XBH_U8 data)
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

XBH_S32 ChipLtrx140a::Ltrx140a_Read(XBH_U8 reg, XBH_U8* data)
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

XBH_S32 ChipLtrx140a::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 val[1];
    // Enable AEN and PON
    s32Ret = Ltrx140a_Write(LTRX140A_ALS_CONFIG_REG, 0x00);
    if (s32Ret < 0) {
        XLOGE("[Ltrx140a] Enable  ALS_CONFIG Register failed!\n");
        return XBH_FAILURE;
    }
    s32Ret = Ltrx140a_Write(LTRX140A_MAIN_CTRL_REG, 0x18);
    if (s32Ret < 0) {
        XLOGE("[Ltrx140a] Enable  Main Control Register failed!\n");
        return XBH_FAILURE;
    }
    s32Ret = Ltrx140a_Write(LTRX140A_CMD_REGISTER, 0x21);
    if (s32Ret < 0) {
        XLOGE("[Ltrx140a] Enable AEN and PON failed!\n");
        return XBH_FAILURE;
    }

    XBH_CHAR type[PROPERTY_VALUE_MAX] = {0};
    property_get("ro.product.xbh.customer.code", type, "0");
    if(strcmp(type, "IYM") == 0)
    {
        #if (TYPE_BOARD == XMR3576B)
        s32Ret = Ltrx140a_Write(LTRX140A_CMD_REGISTER, 0x25);
        #endif
    }

    XLOGD("lightsensor Ltrx140a init success !\n");
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipLtrx140a::ChipLtrx140a(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x23;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipLtrx140a::~ChipLtrx140a()
{

}
