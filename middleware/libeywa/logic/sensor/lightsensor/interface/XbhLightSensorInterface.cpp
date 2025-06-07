#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLightSensorInterface"

#include "XbhLog.h"
#include "XbhLightSensorInterface.h"

/**
 * 复位light sensor芯片
*/
XBH_S32 XbhLightSensorInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 XbhLightSensorInterface::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 XbhLightSensorInterface::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否初始化完成
 */
XBH_S32 XbhLightSensorInterface::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhLightSensorInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhLightSensorInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhLightSensorInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

