#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLiftbracketInterface"

#include "XbhLog.h"
#include "XbhLiftbracketInterface.h"

XBH_S32 XbhLiftbracketInterface::get_usb_pvid_exist(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_BOOL XbhLiftbracketInterface::openSerialPort()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhLiftbracketInterface::readCommand(int fd, XBH_U8* pbuf, XBH_S32 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_U8 XbhLiftbracketInterface::sendCmd(int fd, const XBH_U8* cmd, int len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhLiftbracketInterface::setLiftbracketOperate(int operat)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhLiftbracketInterface::getLiftbracketExist(int *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

