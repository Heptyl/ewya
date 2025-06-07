#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchInterfaceUart"

#include "XbhLog.h"
#include <string.h>
#include <stdio.h>
#include "cutils/properties.h"
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "XbhTouchInterfaceUart.h"
#include "XbhTouchCommonUart.h"
#include "XbhTouchTimeLinkUart.h"

XbhTouchInterfaceUart*      XbhTouchInterfaceUart::mInstance = XBH_NULL;
XbhMutex                    XbhTouchInterfaceUart::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchInterfaceUart::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart ->setSourceTouchState(enable);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL");
    }
    XLOGD("setSourceTouchState enable = %d , ret = %d", enable, ret);
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchInterfaceUart::getSourceTouchState(XBH_BOOL *enable)
{
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        m_pXbhTouchInterfaceUart->getSourceTouchState(enable);
        XLOGD("getSourceTouchState enable = %d ", *enable);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL ");
        *enable = 0;
    }
    return XBH_SUCCESS;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 XbhTouchInterfaceUart::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart ->setNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL");
    }
    XLOGD("setNonThroughTouchRegion id = %d resolution = %d x = %d y = %d w = %d h = %d , ret = %d",id, resolution, x, y, w, h, ret);
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchInterfaceUart::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart ->deleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL");
    }
    XLOGD("deleteNonThroughTouchRegion id = %d , ret = %d", id, ret);
    return ret;
}

/**
 * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
 */
XBH_S32 XbhTouchInterfaceUart::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        return m_pXbhTouchInterfaceUart ->setTouchScaleRect(resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchInterfaceUart::sendKeyToTp(XBH_U32 keyCode)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart->sendKeyToTp(keyCode);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL");
    }
    XLOGD("sendKeyToTp keyCode = %d , ret = %d", keyCode, ret);
    return ret;
}

/**
* 发送数据到触摸框
*/
XBH_S32 XbhTouchInterfaceUart::sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart->sendDataToTp(length, touchData);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUart is NULL ");
    }
    XLOGD("sendDataToTp length = %d , ret = %d", length, ret);
    return XBH_FAILURE;
}

/**
 * 设置信源的触摸区域，可实现屏幕下移功能的windows触摸下移
 */
XBH_S32 XbhTouchInterfaceUart::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart->setPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUart is NULL");
    }
    return ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchInterfaceUart::getPalmRejection(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart->getPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUart is NULL");
    }
    return ret;
}

/**
* 获取触摸框的连接状态
*/
XBH_S32 XbhTouchInterfaceUart::getTouchPanelConnect(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart->getTouchPanelConnect(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUart is NULL");
    }
    return ret;
}

/**
* 获取触摸框的名称
*/
XBH_S32 XbhTouchInterfaceUart::getTouchPanelName(XBH_CHAR *name)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        ret = m_pXbhTouchInterfaceUart->getTouchPanelName(name);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUart is NULL");
    }
    return ret;
}

XBH_S32 XbhTouchInterfaceUart::Init(XBH_VOID)
{
    char tpProduct[PROPERTY_VALUE_MAX];
    m_pXbhTouchInterfaceUart = XBH_NULL;
    //fatt模式时不打开串口触摸
    if (property_get_bool("vendor.xbh.fattmode", XBH_FALSE))
    {
        return XBH_FAILURE;
    }

    property_get("persist.vendor.xbh.touch.product", tpProduct, "Common");
    if(strcmp(tpProduct, "Common") == 0)
    {
        m_pXbhTouchInterfaceUart = XbhTouchCommonUart::getInstance();
    }
    else if(strcmp(tpProduct, "TimeLink") == 0)
    {
        m_pXbhTouchInterfaceUart = XbhTouchTimeLinkUart::getInstance();
    }

    return XBH_SUCCESS;
}

XbhTouchInterfaceUart::XbhTouchInterfaceUart()
{
    Init();
}

XbhTouchInterfaceUart::~XbhTouchInterfaceUart()
{
    if(m_pXbhTouchInterfaceUart != NULL)
    {
        delete m_pXbhTouchInterfaceUart;
        m_pXbhTouchInterfaceUart = NULL;
    }
}

XbhTouchControlInterface *XbhTouchInterfaceUart::getInstance()
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchInterfaceUart();
    }
    return mInstance;
}
