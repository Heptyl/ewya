#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhHdmiSwitchInterface"

#include "XbhLog.h"
#include "XbhHdmiSwitchInterface.h"

/**
 * 给指定的芯片上下电
 * onoff：XBH_TRUE 上电，XBH_FALSE 下电
 */
XBH_S32 XbhHdmiSwitchInterface::setPowerChip(XBH_BOOL onoff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 复位当前转换芯片
 * hw: 采用硬复位还是软复位
 */
XBH_S32 XbhHdmiSwitchInterface::resetChip(XBH_BOOL hw)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 XbhHdmiSwitchInterface::setRxHpd(XBH_U8 port, XBH_BOOL high)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhHdmiSwitchInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 * strFilePath: 升级文件的路径
 */
XBH_S32 XbhHdmiSwitchInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
*获取固件升级状态
 0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败

*/
XBH_S32 XbhHdmiSwitchInterface::getUpgradeState(XBH_S32 *pState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
* 设置OPS Charge Power
*/
XBH_S32 XbhHdmiSwitchInterface::setChargingPower(XBH_U8 data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 烧录 HDCP KEY
 * strFilePath: 升级文件的路径
 */
XBH_S32 XbhHdmiSwitchInterface::setHdcpKey(const XBH_CHAR* strFilePath)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 读取Hdcp Key
 */
XBH_S32 XbhHdmiSwitchInterface::getHdmiSwitchHdcpKey(XBH_U32 devType, XBH_CHAR* hdcpValue, XBH_HDCP_TYPE_E hdcpType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 读取 HDCP KEY NAME
 */
XBH_S32 XbhHdmiSwitchInterface::getHdcpKeyName(XBH_CHAR* strHdcpFileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 烧录 HDCP KEY NAME
 */
XBH_S32 XbhHdmiSwitchInterface::setHdcpKeyName(const XBH_CHAR* strHdcpFileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 切换switch的port
 * port: 指定的port
 */
XBH_S32 XbhHdmiSwitchInterface::setActivePort(XBH_U8 port)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取switch当前切换到的port
 * port: 指定的port
 */
XBH_S32 XbhHdmiSwitchInterface::getActivePort(XBH_U8* port)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取switch上面连接的port的信号状态 
 * （部分场景下使用，比如有些dp转换过来的信号无法导致det pin为高电平就需要switch来判断）
 * port: 指定的port
 * status：信号状态
 */
XBH_S32 XbhHdmiSwitchInterface::getRxPortStatus(XBH_U8 port, XBH_U8* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


 /**
 * 获取switch上面输出的TxPort port的信号状态
 * （部分场景下使用，需要根据TxPort port   处理上一级得信号或者通知， 或者复位）
 * status：信号状态
 */
 XBH_S32 XbhHdmiSwitchInterface::getTxPortStatus(XBH_U8* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhHdmiSwitchInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhHdmiSwitchInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置switch进入特殊的模式
 */
XBH_S32 XbhHdmiSwitchInterface::setSpecificMode()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置switch portX 物理地址
 */
XBH_S32 XbhHdmiSwitchInterface::setPhysicalAddr(XBH_U8 port, XBH_U16 cecAddr)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置switch portX edid
 */
XBH_S32 XbhHdmiSwitchInterface::updateEdid(XBH_U8 edid_data[256],XBH_U8 port)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置type-c的VBUS复位
*/
XBH_S32 XbhHdmiSwitchInterface::setTypecReset()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
