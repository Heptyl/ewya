#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhUsbc2HdmiInterface"

#include "XbhLog.h"
#include "XbhUsbc2HdmiInterface.h"

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhUsbc2HdmiInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 XbhUsbc2HdmiInterface::setRxHpd(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhUsbc2HdmiInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhUsbc2HdmiInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
    * dataflow进行固件升级
*/
XBH_S32 XbhUsbc2HdmiInterface::upgradeFirmwareByData(XBH_U8 *data, XBH_U32 dataLen, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhUsbc2HdmiInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhUsbc2HdmiInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhUsbc2HdmiInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 限制type-c功率达到OPS过流保护效果
 */
XBH_S32 XbhUsbc2HdmiInterface::setTypecCapacity(XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置switch进入特殊的模式
 */
XBH_S32 XbhUsbc2HdmiInterface::setSpecificMode()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 根据OPS插入状态，动态设置PD功率
 */
XBH_S32 XbhUsbc2HdmiInterface::setChargingPower(XBH_PDCAPACITY_POWER data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}