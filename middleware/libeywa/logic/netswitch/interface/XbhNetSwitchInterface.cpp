#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhNetSwitchInterface"

#include "XbhLog.h"
#include "XbhNetSwitchInterface.h"

/**
 * 设置交换机的port的开关状态，通常是设置OPS，TYPEC端口的网络开关
 * 连接Android的端口通常不需要操作
*/
XBH_S32 XbhNetSwitchInterface::setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取交换机的port的开关状态，通常是获取OPS，TYPEC端口的网络开关
*/
XBH_S32 XbhNetSwitchInterface::getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取交换机的RJ45的连接状态，通常是输入输出口
*/
XBH_S32 XbhNetSwitchInterface::getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置交换机的输入输出的RJ45端口的指示灯工作状态
*/
XBH_S32 XbhNetSwitchInterface::setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhNetSwitchInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhNetSwitchInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhNetSwitchInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhNetSwitchInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhNetSwitchInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置chip网络唤醒使能
 */
XBH_S32 XbhNetSwitchInterface::setChipWol(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置wol端口电压
 */
XBH_S32 XbhNetSwitchInterface::setWolPwer(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

