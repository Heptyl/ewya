#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchManager"

#include "XbhTouchManager.h"
#include "XbhService.h"
#include "XbhLog.h"
#include "cutils/properties.h"
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

XbhTouchManager*        XbhTouchManager::mInstance = XBH_NULL;
XbhMutex                XbhTouchManager::mLockObject;

void XbhTouchManager::run(const void* arg)
{
    while (XbhService::getModuleInterface()->mIsBootCompleted != 1)
    {
        usleep(500*1000); /* sleep 500ms */
    }

    //Init touch device
    XBH_S32 s32TouchType = property_get_int32("persist.vendor.xbh.touch.type", 0);

    if(s32TouchType == XBH_TP_IF_AUTO_CTRL)
    {
        m_pXbhTouchControlInterface = XbhTouchInterfaceAuto::getInstance();
    }
    else if (s32TouchType == XBH_TP_IF_UART_CTRL)
    {
        m_pXbhTouchControlInterface = XbhTouchInterfaceUart::getInstance();
    }
    else if (s32TouchType == XBH_TP_IF_USB_CTRL)
    {
        m_pXbhTouchControlInterface = XbhTouchInterfaceUsb::getInstance();
    }
    else if(s32TouchType == XBH_TP_IF_MCU_CTRL)
    {
    }
    else if(s32TouchType == XBH_TP_IF_SDK_CTRL)
    {
        m_pXbhTouchControlInterface = XbhTouchInterfaceSdk::getInstance();
    }
}

/**
 * 设置信号源的触摸开关。
 * onoff 开关状态
 */
XBH_S32 XbhTouchManager::setSourceTouchState(XBH_BOOL enable)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("setSourceTouchState enable = %d", enable);
        return m_pXbhTouchControlInterface->setSourceTouchState(enable);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchManager::getSourceTouchState(XBH_BOOL *enable)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        m_pXbhTouchControlInterface->getSourceTouchState(enable);
        XLOGD("getSourceTouchState enable = %d ", *enable);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
        *enable = 0;
    }
    return XBH_SUCCESS;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2: 5210x2160
 */
XBH_S32 XbhTouchManager::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("setNonThroughTouchRegion id = %d resolution = %d x = %d y = %d w = %d h = %d ",id, resolution, x, y, w, h);
        return m_pXbhTouchControlInterface->setNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchManager::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("deleteNonThroughTouchRegion id = %d ",id);
        return m_pXbhTouchControlInterface->deleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
 */
XBH_S32 XbhTouchManager::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("setTouchScaleRect resolution = %d x = %d y = %d w = %d h = %d ",resolution, x, y, w, h);
        return m_pXbhTouchControlInterface->setTouchScaleRect(resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 设置触摸缩放区域
 */
XBH_S32 XbhTouchManager::setTouchScalingRegion(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("setTouchScaleRegion resolution = %d x = %d y = %d w = %d h = %d ",resolution, x, y, w, h);
        return m_pXbhTouchControlInterface->setTouchScalingRegion(resolution, x, y, w, h);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchManager::sendKeyToTp(XBH_U32 keyCode)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("sendKeyToTouch keyCode = %d ", keyCode);
        return m_pXbhTouchControlInterface->sendKeyToTp(keyCode);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
* 发送数据到触摸框
*/
XBH_S32 XbhTouchManager::sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        XLOGD("sendDataToTp length = %d ", length);
        return m_pXbhTouchControlInterface->sendDataToTp(length, touchData);
    }
    else
    {
        XLOGD("m_pXbhTouchControlInterface is NULL ");
    }
    return XBH_FAILURE;
}

/**
 * 设置信源的触摸区域，可实现屏幕下移功能的windows触摸下移
 */
XBH_S32 XbhTouchManager::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchControlInterface != NULL)
    {
        ret = m_pXbhTouchControlInterface->setPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchControlInterface is NULL");
    }
    return ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchManager::getPalmRejection(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchControlInterface != NULL)
    {
        ret = m_pXbhTouchControlInterface->getPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchControlInterface is NULL");
    }
    return ret;
}

/**
* 获取触摸框的连接状态
*/
XBH_S32 XbhTouchManager::getTouchPanelConnect(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchControlInterface != NULL)
    {
        ret = m_pXbhTouchControlInterface->getTouchPanelConnect(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchControlInterface is NULL");
    }
    return ret;
}

/**
* 获取触摸框的名称
*/
XBH_S32 XbhTouchManager::getTouchPanelName(XBH_CHAR *name)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchControlInterface != NULL)
    {
        ret = m_pXbhTouchControlInterface->getTouchPanelName(name);
    }
    else
    {
        XLOGE("m_pXbhTouchControlInterface is NULL");
    }
    return ret;
}

/**
* 获取触摸框的驱动版本
*/
XBH_S32 XbhTouchManager::getTpDriverVersion(XBH_CHAR *version)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchControlInterface != NULL)
    {
        ret = m_pXbhTouchControlInterface->getTpDriverVersion(version);
    }
    else
    {
        XLOGE("m_pXbhTouchControlInterface is NULL");
    }
    return ret;
}

/**
* 获取触摸框的固件版本
*/
XBH_S32 XbhTouchManager::getTpFirmwareVersion(XBH_CHAR *version)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchControlInterface != NULL)
    {
        ret = m_pXbhTouchControlInterface->getTpFirmwareVersion(version);
    }
    else
    {
        XLOGE("m_pXbhTouchControlInterface is NULL");
    }
    return ret;
}

XbhTouchManager::XbhTouchManager()
{
    m_pXbhTouchControlInterface = XBH_NULL;
    XbhMWThread::run(XbhMWThread::ONCE);
}

XbhTouchManager::~XbhTouchManager()
{
    if(m_pXbhTouchControlInterface != NULL)
    {
        delete m_pXbhTouchControlInterface;
    }
    m_pXbhTouchControlInterface = XBH_NULL;
}

XbhTouchManager *XbhTouchManager::getInstance()
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchManager();
    }
    return mInstance;
}
