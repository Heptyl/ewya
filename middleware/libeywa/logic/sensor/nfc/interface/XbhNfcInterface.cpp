#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhNfcInterface"

#include "XbhLog.h"
#include "XbhNfcInterface.h"

/**
 * 复位芯片
*/
XBH_S32 XbhNfcInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 检测Nfc是否存在
*/
XBH_S32 XbhNfcInterface::getChipExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhNfcInterface::getChipInitDone(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 使能nfc
*/
XBH_S32 XbhNfcInterface::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 得到NFC状态
*/
XBH_S32 XbhNfcInterface::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Nfc 卡ID
*/
XBH_S32 XbhNfcInterface::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 添加NFC 卡
*/
XBH_S32 XbhNfcInterface::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 删除NFC 卡
*/
XBH_S32 XbhNfcInterface::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 删除所有NFC卡
*/
XBH_S32 XbhNfcInterface::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 选择NFC模块
*/
XBH_S32 XbhNfcInterface::switchNfcType()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置NFC低功耗模式
*/
XBH_S32 XbhNfcInterface::setNfcLowPower(const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取NFC低功耗模式
*/
XBH_S32 XbhNfcInterface::getNfcLowPower(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取NFC唤醒的卡ID
*/
XBH_S32 XbhNfcInterface::getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
