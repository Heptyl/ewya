#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchInterfaceUsb"

#include "XbhLog.h"
#include "cutils/properties.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "XbhTouchInterfaceUsb.h"
//#include "XbhTouchHuaxinUsb.h"
//#include "XbhTouchKtcUsb.h"

XbhTouchInterfaceUsb*          XbhTouchInterfaceUsb::mInstance = XBH_NULL;
XbhMutex                       XbhTouchInterfaceUsb::mLockObject;

/**
 * 设置信号源的触摸开关。
 * onoff 开关状态
 */
XBH_S32 XbhTouchInterfaceUsb::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->setSourceTouchState(enable);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL");
    }
    XLOGD("setSourceTouchState enable = %d , ret = %d", enable, ret);
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchInterfaceUsb::getSourceTouchState(XBH_BOOL *enable)
{
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        m_pXbhTouchInterfaceUsb->getSourceTouchState(enable);
        XLOGD("getSourceTouchState enable = %d ", *enable);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL ");
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
XBH_S32 XbhTouchInterfaceUsb::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->setNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL");
    }
    XLOGD("setNonThroughTouchRegion id = %d resolution = %d x = %d y = %d w = %d h = %d , ret = %d", id, resolution, x, y, w, h, ret);
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchInterfaceUsb::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->deleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL");
    }
    XLOGD("deleteNonThroughTouchRegion id = %d , ret = %d", id, ret);
    return ret;
}

/**
 * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
 */
XBH_S32 XbhTouchInterfaceUsb::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->setTouchScaleRect(resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL");
    }
    XLOGD("setTouchScaleRect resolution = %d x = %d y = %d w = %d h = %d , ret = %d", resolution, x, y, w, h, ret);
    return ret;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchInterfaceUsb::sendKeyToTp(XBH_U32 keyCode)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->sendKeyToTp(keyCode);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL");
    }
    XLOGD("sendKeyToTp keyCode = %d , ret = %d", keyCode, ret);
    return ret;
}

/**
* 发送数据到触摸框
*/
XBH_S32 XbhTouchInterfaceUsb::sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->sendDataToTp(length, touchData);
    }
    else
    {
        XLOGD("m_pXbhTouchInterfaceUsb is NULL ");
    }
    XLOGD("sendDataToTp length = %d , ret = %d", length, ret);
    return XBH_FAILURE;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchInterfaceUsb::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->setPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUsb is NULL");
    }
    return ret;
}

/**
* 获取触摸框的防误触功能开关
*/
XBH_S32 XbhTouchInterfaceUsb::getPalmRejection(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->getPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUsb is NULL");
    }
    return ret;
}

/**
* 获取触摸框的连接状态
*/
XBH_S32 XbhTouchInterfaceUsb::getTouchPanelConnect(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->getTouchPanelConnect(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUsb is NULL");
    }
    return ret;
}

/**
* 获取触摸框的名称
*/
XBH_S32 XbhTouchInterfaceUsb::getTouchPanelName(XBH_CHAR *name)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        ret = m_pXbhTouchInterfaceUsb->getTouchPanelName(name);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceUsb is NULL");
    }
    return ret;
}

XBH_S32 XbhTouchInterfaceUsb::Init(XBH_VOID)
{
    char tpProduct[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.xbh.touch.product", tpProduct, "Common");
/*
    if(strcmp(tpVendor, "Common") == 0) {

    } else if(strcmp(tpVendor, "HuaXin") == 0) {
        while(m_pXbhTouchInterfaceUsb == XBH_NULL) {
            usleep(1000 * 1000);//wait for huaxin driver init
            m_pXbhTouchInterfaceUsb = XbhTouchHuaxinUsb::getInstance();
            if(m_pXbhTouchInterfaceUsb != XBH_NULL || runTimer >= 5) {
                XLOGI("XbhTouchInterfaceUsb: %d", runTimer);
                break;
            }
            runTimer++;
        }
    } else if(strcmp(tpVendor, "Ktc") == 0) {
        m_pXbhTouchInterfaceUsb = XbhTouchKtcUsb::getInstance();
    }
*/
    return XBH_SUCCESS;
}

XbhTouchInterfaceUsb::XbhTouchInterfaceUsb()
{
    XLOGD(" begin ");
    Init();
    XLOGD(" end ");
}

XbhTouchInterfaceUsb::~XbhTouchInterfaceUsb()
{
    if(m_pXbhTouchInterfaceUsb != NULL)
    {
        delete m_pXbhTouchInterfaceUsb;
        m_pXbhTouchInterfaceUsb = NULL;
    }
}

XbhTouchControlInterface *XbhTouchInterfaceUsb::getInstance()
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchInterfaceUsb();
    }
    return mInstance;
}
