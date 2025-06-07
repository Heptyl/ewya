#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchZyUart"

#include "XbhLog.h"
#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dlfcn.h>//RTLD_LAZY  dlsym
//#include <ctype.h>

#include "cutils/properties.h"
#include "XbhTouchZyUart.h"

XbhTouchZyUart*         XbhTouchZyUart::mInstance = XBH_NULL;
XbhMutex                    XbhTouchZyUart::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchZyUart::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetSourceTouchState)
    {
        ret = mSetSourceTouchState(enable ? 1 : 0);
    }
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchZyUart::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mGetSourceTouchState)
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
XBH_S32 XbhTouchZyUart::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetNonThroughTouchRegion) 
    {
        ret = mSetNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    return ret;

}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchZyUart::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mDeleteNonThroughTouchRegion)
    {
        ret = mDeleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    return ret;
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
*/
XBH_S32 XbhTouchZyUart::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetTouchScaleRect)
    {
        ret = mSetTouchScaleRect(resolution, x, y, w, h);
    }
    return ret;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchZyUart::sendKeyToTp(XBH_U32 keyCode)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSendKeyToTp)
    {
        ret = mSendKeyToTp(keyCode);
    }
    return ret;
}

/**
 * 设置触摸框防误触开关
 */ 
XBH_S32 XbhTouchZyUart::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetPalmRejection)
    {
        ret = mSetPalmRejection(enable ? 1 : 0);
    }
    return ret;
}

/**
 *  获取触摸框防误触开关
 */ 
XBH_S32 XbhTouchZyUart::getPalmRejection(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mGetPalmRejection)
    {
        ret = mGetPalmRejection(enable);
    }
    return ret;
}

XBH_S32 XbhTouchZyUart::Init(XBH_VOID)
{
    XBH_S32 s32BaudRate;
    XBH_CHAR uartPath[PROPERTY_VALUE_MAX];

    property_get("persist.vendor.xbh.touchuart.path", uartPath, "null");
    s32BaudRate = property_get_int32("persist.vendor.xbh.touchuart.baudrate", 115200);

    mTouchHandle = dlopen("/vendor/lib64/libzyuarttouch.so", RTLD_LAZY);
    if(!mTouchHandle)
    {
        XLOGE("open libzyuarttouch.so fail.");
        return 0;
    }

    mInitProcess = (XBH_S32 (*)(const XBH_CHAR* , XBH_U32 ))dlsym(mTouchHandle, "initProcess");
    mSetSourceTouchState = (XBH_S32 (*)(XBH_BOOL ))dlsym(mTouchHandle, "setSourceTouchState");
    mGetSourceTouchState = (XBH_S32 (*)(XBH_BOOL* ))dlsym(mTouchHandle, "getSourceTouchState");
    mSetNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "setNonThroughTouchRegion");
    mDeleteNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32 ,XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "deleteNonThroughTouchRegion");
    mSetTouchScaleRect = (XBH_S32 (*)(XBH_S32 ,XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "setTouchScaleRect");
    mSendKeyToTp = (XBH_S32 (*)(XBH_U32 ))dlsym(mTouchHandle, "sendKeyToTp");
    mSetPalmRejection = (XBH_S32 (*)(XBH_BOOL ))dlsym(mTouchHandle, "setPalmRejection");
    mGetPalmRejection = (XBH_S32 (*)(XBH_BOOL* ))dlsym(mTouchHandle, "getPalmRejection");


    if(mInitProcess)
    {
        mInitProcess(uartPath, s32BaudRate);
    }
    else
    {
        XLOGE("Init touchProcess fail.");
    }

    
#if 0
    if(strcmp(uartPath, DEV_TTY_ACM0) == 0)
    {
        insmod(0);
        openSerialPort(DEV_TTY_ACM0, s32BaudRate);
    }
    else if(strcmp(uartPath, DEV_TTY_ACM1) == 0)
    {
        insmod(1);
        openSerialPort(DEV_TTY_ACM1, s32BaudRate);
    }
    else if(strcmp(uartPath, DEV_TTY_ACM2) == 0)
    {
        insmod(2);
        openSerialPort(DEV_TTY_ACM2, s32BaudRate);
    }
    else if(strcmp(uartPath, DEV_TTY_ACM3) == 0)
    {
        insmod(3);
        openSerialPort(DEV_TTY_ACM3, s32BaudRate);
    }
    else if(strcmp(uartPath, DEV_TTY_ACM4) == 0)
    {
        insmod(4);
        openSerialPort(DEV_TTY_ACM4, s32BaudRate);
    }
    else
    {
        XLOGE("init error ,not found tty device !!!! ");
        return XBH_FAILURE;
    }
#endif
    isInitTouch = XBH_TRUE;

    return XBH_SUCCESS;
}

XbhTouchZyUart::XbhTouchZyUart(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mKoName));
    if(ko != XBH_NULL)
    {
        memcpy(mKoName, ko, strlen(ko));
    }
    else
    {
        memcpy(mKoName, "", strlen(""));
    }
    if(tpName != XBH_NULL)
    {
        memcpy(mTpName, tpName, strlen(tpName));
    }
    else
    {
        memcpy(mTpName, "", strlen(""));
    }
    Init();
}

XbhTouchZyUart::~XbhTouchZyUart()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchInterface *XbhTouchZyUart::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchZyUart(ko, tpName);
    }
    return mInstance;
}
