#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhMcuInterface"

#include "XbhLog.h"
#include "XbhMcuInterface.h"

/**
 * 复位light sensor芯片
*/
XBH_S32 XbhMcuInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhMcuInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhMcuInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhMcuInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhMcuInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhMcuInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置fatt模式
 */
XBH_S32 XbhMcuInterface::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取fatt模式
 */
XBH_S32 XbhMcuInterface::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * mcu 外置iic数据转发
 */
XBH_S32 XbhMcuInterface::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * mcu 外置iic数据获取
 */
XBH_S32 XbhMcuInterface::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhMcuInterface::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhMcuInterface::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhMcuInterface::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
