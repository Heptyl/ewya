#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchInterface"

#include "XbhLog.h"
#include "XbhTouchInterface.h"

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchInterface::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchInterface::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 XbhTouchInterface::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchInterface::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置信源的触摸区域，可实现屏幕下移功能的windows触摸下移
 */
XBH_S32 XbhTouchInterface::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置触摸缩放区域
 */
XBH_S32 XbhTouchInterface::setTouchScalingRegion(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchInterface::sendKeyToTp(XBH_U32 keyCode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 发送数据到触摸框
*/
XBH_S32 XbhTouchInterface::sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchInterface::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取触摸框的防误触功能开关
*/
XBH_S32 XbhTouchInterface::getPalmRejection(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取触摸框的连接状态
*/
XBH_S32 XbhTouchInterface::getTouchPanelConnect(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取触摸框的名称
*/
XBH_S32 XbhTouchInterface::getTouchPanelName(XBH_CHAR *name)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取触摸框驱动版本
*/
XBH_S32 XbhTouchInterface::getTpDriverVersion(XBH_CHAR *version)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取触摸框固件版本
*/
XBH_S32 XbhTouchInterface::getTpFirmwareVersion(XBH_CHAR *version)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}