#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipTsl2572"
#include "XbhLog.h"
#include <algorithm>

#include "XbhService.h"
#include "ChipTsl2572.h"

XbhMutex                 ChipTsl2572::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipTsl2572::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    s32Ret = TSL2572_Write(0xA0, 0x03);
    if (s32Ret < 0) {
        XLOGE("[TSL2572] reset failed!\n");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 ChipTsl2572::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 val[4];
    XBH_U32 c0,c1;
    XBH_FLOAT cpl,lux1,lux2;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *s32State = 0;
        return s32Ret;
    }
    TSL2572_Read(TSL2572_REG_ALS_DATA1_L, &val[0]);
    TSL2572_Read(TSL2572_REG_ALS_DATA1_H, &val[1]);

    TSL2572_Read(TSL2572_REG_ALS_DATA2_L, &val[2]);
    TSL2572_Read(TSL2572_REG_ALS_DATA2_H, &val[3]);
    
    c0 = ((val[1] << 8) | val[0]);
    c1 = ((val[3] << 8) | val[2]);

    cpl = 174.72 * (float)1 / 60.0;

    lux1 = ((float)c0 - (1.87 * (float)c1)) / cpl;
    lux2 = ((0.63 * (float)c0) - (float)c1) / cpl;

    if(lux1 <= 0 || lux2 <=0){
        *s32State = 0;
        XLOGE(" lux error !!!!!");
        return  XBH_SUCCESS;
    }

    cpl = std::max(lux1, lux2);
    *s32State = cpl;
    return  XBH_SUCCESS;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 ChipTsl2572::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipTsl2572::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipTsl2572::getFirmwareVersion(XBH_CHAR* strVersion)
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
XBH_S32 ChipTsl2572::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
XBH_S32 ChipTsl2572::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipTsl2572::TSL2572_Write(XBH_U8 reg, XBH_U8 data)
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

XBH_S32 ChipTsl2572::TSL2572_Read(XBH_U8 reg, XBH_U8* data)
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

XBH_S32 ChipTsl2572::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 val[1];
    // check product id
    TSL2572_Read(TSL2572_REG_PRODUCT_ID | TSL2572_CMD_REGISTER, &val[0]);

    if(TSL25721_V != val[0]) {
        XLOGE("[TSL2572] not found!\n");
        return XBH_FAILURE;
    }

    //Set gain 120X
    s32Ret = TSL2572_Write(TSL2572_CMD_REGISTER | TSL2572_TYPE_INC | TSL2572_CMDS_CONTROL, 0x00);
    if (s32Ret < 0) {
        XLOGE("[TSL2572] Set gain failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);

    // Set timing register
    s32Ret = TSL2572_Write(TSL2572_CMD_REGISTER | TSL2572_TYPE_INC | TSL2572_CMDS_CONFIG, 0x00);
    if (s32Ret < 0)
    {
        XLOGE("[TSL2572] Set timing register failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);

    // Enable AEN and PON
    s32Ret = TSL2572_Write(TSL2572_CMD_REGISTER | TSL2572_TYPE_INC | TSL2572_CMDS_ALS_TIMING, 0xC0);
    if (s32Ret < 0)
    {
        XLOGE("[TSL2572] Enable AEN and PON failed!\n");
        return XBH_FAILURE;
    }
    usleep(20 * 1000);
    
    // Enable AEN and PON
    s32Ret = TSL2572_Write(TSL2572_CMD_REGISTER | TSL2572_TYPE_INC | TSL2572_CMDS_ENABLE, 0x03);
    if (s32Ret < 0)
    {
        XLOGE("[TSL2572] Enable AEN and PON failed!\n");
        return XBH_FAILURE;
    }
    XLOGD("lightsensor TSL2572 init success !\n");
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipTsl2572::ChipTsl2572(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = 0x39;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipTsl2572::~ChipTsl2572()
{

}
