#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchIsolutionUart_G"

#include "XbhLog.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <dlfcn.h> //RTLD_LAZY  dlsym
// #include <ctype.h>

#include "XbhTouchIsolutionUart_G.h"
#include "cutils/properties.h"

XbhTouchIsolutionUart_G *XbhTouchIsolutionUart_G::mInstance = XBH_NULL;
XbhMutex XbhTouchIsolutionUart_G::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchIsolutionUart_G::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if (mSetSourceTouchState)
    {
        ret = mSetSourceTouchState(enable ? 1 : 0);
    }
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchIsolutionUart_G::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if (mGetSourceTouchState)
    {
        ret = mGetSourceTouchState(enable);
    }
    return ret;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 * 设置不透传的区域，坐标范围为0~32767，传进来的数值需要是已经被转换过的
 * 2K UI  xt = (x*32767)/1920
 * yt = (y*32767)/1080
 * wt = (w*32767)/1920
 * ht = (h*32767)/1080
 * 4K UI  xt = (x*32767)/3840
 * yt = (y*32767)/2160
 * wt = (w*32767)/3840
 * ht = (h*32767)/2160
 */
XBH_S32 XbhTouchIsolutionUart_G::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if (mSetNonThroughTouchRegion)
    {
        ret = mSetNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchIsolutionUart_G::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if (mDeleteNonThroughTouchRegion)
    {
        ret = mDeleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    return ret;
}

/**
 * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
 */
XBH_S32 XbhTouchIsolutionUart_G::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if (mSetTouchScaleRect)
    {
        ret = mSetTouchScaleRect(resolution, x, y, w, h);
    }
    return ret;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchIsolutionUart_G::sendKeyToTp(XBH_U32 keyCode)
{
    XBH_S32 ret = XBH_FAILURE;
    if (mSendKeyToTp)
    {
        ret = mSendKeyToTp(keyCode);
    }
    return ret;
}

/**
 * 设置触摸框防误触开关
 */
XBH_S32 XbhTouchIsolutionUart_G::setPalmRejection(XBH_BOOL enable)
{
    XLOGE("setPalmRejection unsupported.");
    return XBH_FAILURE;
}

/**
 *  获取触摸框防误触开关
 */
XBH_S32 XbhTouchIsolutionUart_G::getPalmRejection(XBH_BOOL *enable)
{
    XLOGE("setPalmRejection unsupported.");
    return XBH_FAILURE;
}

XBH_S32 XbhTouchIsolutionUart_G::Init(XBH_VOID)
{
    XBH_S32 s32BaudRate;
    XBH_CHAR uartPath[PROPERTY_VALUE_MAX];

    property_get("persist.vendor.xbh.touchuart.path", uartPath, "null");
    s32BaudRate = property_get_int32("persist.vendor.xbh.touchuart.baudrate", 115200);

    mTouchHandle = dlopen("/vendor/lib64/libistguarttouch.so", RTLD_LAZY);
    if (!mTouchHandle)
    {
        XLOGE("open libistguarttouch.so fail.");
        return 0;
    }

    mInitProcess = (XBH_S32 (*)(const XBH_CHAR *, XBH_U32))dlsym(mTouchHandle, "initProcess");
    mSetSourceTouchState = (XBH_S32 (*)(XBH_BOOL))dlsym(mTouchHandle, "setSourceTouchState");
    mGetSourceTouchState = (XBH_S32 (*)(XBH_BOOL *))dlsym(mTouchHandle, "getSourceTouchState");
    mSetNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32, XBH_S32, XBH_S32, XBH_S32, XBH_S32, XBH_S32))dlsym(mTouchHandle, "setNonThroughTouchRegion");
    mDeleteNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32, XBH_S32, XBH_S32, XBH_S32, XBH_S32, XBH_S32))dlsym(mTouchHandle, "deleteNonThroughTouchRegion");
    mSetTouchScaleRect = (XBH_S32 (*)(XBH_S32, XBH_S32, XBH_S32, XBH_S32, XBH_S32))dlsym(mTouchHandle, "setTouchScaleRect");
    mSendKeyToTp = (XBH_S32 (*)(XBH_U32))dlsym(mTouchHandle, "sendKeyToTp");

    if (mInitProcess)
    {
        mInitProcess(uartPath, s32BaudRate);
    }
    else
    {
        XLOGE("Init touchProcess fail.");
    }

    isInitTouch = XBH_TRUE;

    return XBH_SUCCESS;
}

XbhTouchIsolutionUart_G::XbhTouchIsolutionUart_G(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mKoName));
    if (ko != XBH_NULL)
    {
        memcpy(mKoName, ko, strlen(ko));
    }
    else
    {
        memcpy(mKoName, "", strlen(""));
    }
    if (tpName != XBH_NULL)
    {
        memcpy(mTpName, tpName, strlen(tpName));
    }
    else
    {
        memcpy(mTpName, "", strlen(""));
    }
    Init();
}

XbhTouchIsolutionUart_G::~XbhTouchIsolutionUart_G()
{
    if (mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchInterface *XbhTouchIsolutionUart_G::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL)
    {
        mInstance = new XbhTouchIsolutionUart_G(ko, tpName);
    }
    return mInstance;
}
