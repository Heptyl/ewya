#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchIsolutionUsb_E_HHT"

#include "XbhLog.h"
#include "cutils/properties.h"
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/hidraw.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include "XbhTouchIsolutionUsb_E_HHT.h"

XbhTouchIsolutionUsb_E_HHT*          XbhTouchIsolutionUsb_E_HHT::mInstance = XBH_NULL;
XbhMutex                         XbhTouchIsolutionUsb_E_HHT::mLockObject;

/**
 * 初始化touch
 * enable 开关状态
 */
XBH_S32 XbhTouchIsolutionUsb_E_HHT::initTouchProcess(XBH_VOID)
{
    mTouchHandle = dlopen("/vendor/lib64/libhhttouchprocess.so", RTLD_LAZY);
    if(!mTouchHandle)
    {
        XLOGE("open libhhttouchprocess.so fail.");
        return 0;
    }

    mInitProcess = (XBH_VOID (*)(const XBH_CHAR* , XBH_S32 ))dlsym(mTouchHandle, "InitProcess");
    mSetTouchState = (XBH_S32 (*)(XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "setTouchState");
    mGetTouchState = (XBH_S32 (*)(XBH_S32 ))dlsym(mTouchHandle, "getTouchState");
    mSetNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "setNonThroughTouchRegion");
    mRemoveNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32 ,XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "removeNonThroughTouchRegion");
    mRemoveNonThroughTouchRegionById = (XBH_S32 (*)(XBH_S32 ))dlsym(mTouchHandle, "removeNonThroughTouchRegionById");
    mIsTouchPanelOk = (XBH_S32 (*)(XBH_VOID))dlsym(mTouchHandle, "isTouchPanelOk");
    mSetScreenOffsetHV = (XBH_S32 (*)(XBH_S32 ,XBH_S32 ,XBH_S32 ))dlsym(mTouchHandle, "setScreenOffsetHV");
    mSetPalmType = (XBH_S32 (*)(XBH_S32 ))dlsym(mTouchHandle, "setPalmType");

    if(mInitProcess)
    {
        mInitProcess("/dev/ttyS4", 115200);
    }
    else
    {
        XLOGE("Init touchProcess fail.");
    }
    
    return XBH_SUCCESS;
}

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchIsolutionUsb_E_HHT::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetTouchState)
    {
        ret = mSetTouchState(0, enable ? 1 : 0);
    }
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchIsolutionUsb_E_HHT::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mGetTouchState)
    {
        *enable = mGetTouchState(0);
        ret = XBH_SUCCESS;
    }
    return ret;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 XbhTouchIsolutionUsb_E_HHT::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetNonThroughTouchRegion)
    {
        ret = mSetNonThroughTouchRegion(id, x, y, w, h, resolution);
    }
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchIsolutionUsb_E_HHT::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mRemoveNonThroughTouchRegion)
    {
        ret = mRemoveNonThroughTouchRegion(id, x, y, w, h, resolution);
    }
    return ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchIsolutionUsb_E_HHT::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mSetPalmType)
    {
        s32Ret = mSetPalmType(enable ? 1 : 0);
    }
    if(XBH_SUCCESS == s32Ret)
    {
        property_set("persist.vendor.xbh.palmtouchtype", enable ? "1" : "0");
    }
    return  s32Ret;
}

/**
* 获取触摸框的防误触功能开关
*/
XBH_S32 XbhTouchIsolutionUsb_E_HHT::getPalmRejection(XBH_BOOL* enable)
{
    char prop[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.palmtouchtype", prop, "0");
    if(strcmp(prop,"1") == 0)
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    return  XBH_SUCCESS;
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
*/
XBH_S32 XbhTouchIsolutionUsb_E_HHT::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    return XBH_FAILURE;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchIsolutionUsb_E_HHT::sendKeyToTp(XBH_U32 keyCode)
{
    return XBH_FAILURE;
}

static XBH_S32 insmod(const XBH_CHAR *filename, const XBH_CHAR *args)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_S32 fd;
    fd = TEMP_FAILURE_RETRY(open(filename, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd < 0) 
    {
        XLOGE("Failed to open %s", filename);
        return XBH_FAILURE;
    }
    ret = syscall(__NR_finit_module, fd, args, 0);
    if (ret < 0) 
    {
        XLOGE("finit_module return: %d", ret);
    }
    close(fd);
    return ret;
}

XBH_S32 XbhTouchIsolutionUsb_E_HHT::openTp(void) 
{
    XBH_S32 fd = -1;
    return fd;
}

XBH_S32 XbhTouchIsolutionUsb_E_HHT::sendCommand(XBH_U8* data, XBH_S32 len) 
{
    return XBH_FAILURE;
}

XBH_S32 XbhTouchIsolutionUsb_E_HHT::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    return XBH_SUCCESS;
}

XbhTouchIsolutionUsb_E_HHT::XbhTouchIsolutionUsb_E_HHT(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mTpName));
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
    if(strcmp(mKoName, "") != 0)
    {
        loadModules();
    }
    initTouchProcess();
    //usleep(2000*1000); //2000 ms for driver init
    //mFd = openTp();
}

XbhTouchIsolutionUsb_E_HHT::~XbhTouchIsolutionUsb_E_HHT()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
        dlclose(mTouchHandle);
        close(mFd);
    }
}

XbhTouchInterface *XbhTouchIsolutionUsb_E_HHT::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL)
    {
        mInstance = new XbhTouchIsolutionUsb_E_HHT(ko, tpName);
    }
    return mInstance;
}

