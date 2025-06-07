#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhDp2HdmiInterface"

#include "XbhLog.h"
#include "XbhDp2HdmiInterface.h"

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhDp2HdmiInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 XbhDp2HdmiInterface::setRxHpd(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhDp2HdmiInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhDp2HdmiInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhDp2HdmiInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhDp2HdmiInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhDp2HdmiInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
