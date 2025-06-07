#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchCommonUsb"

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

#include "XbhTouchCommonUsb.h"

XbhTouchCommonUsb*          XbhTouchCommonUsb::mInstance = XBH_NULL;
XbhMutex                       XbhTouchCommonUsb::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchCommonUsb::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mSetTouchState) {
        ret = mSetTouchState(0, enable ? 1 : 0);
    }
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchCommonUsb::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mGetTouchState) {
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
XBH_S32 XbhTouchCommonUsb::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
      XBH_S32 ret = XBH_FAILURE;
    if(mSetNonThroughTouchRegion) {
        ret = mSetNonThroughTouchRegion(id, x, y, w, h, resolution);
    }
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchCommonUsb::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(mRemoveNonThroughTouchRegion) {
        ret = mRemoveNonThroughTouchRegionById(id);
    }
    return ret;
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
*/
XBH_S32 XbhTouchCommonUsb::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    return XBH_FAILURE;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchCommonUsb::sendKeyToTp(XBH_U32 keyCode)
{
    // if(keyCode == 2){ //Ctrl+Alt+Shift+Y
    //     XBH_U8 data[7] = {0xfc, 0xad, 0x01, 0x02, 0x00, 0x07, 0x1c};
    //     return sendCommand(data, 7);
    // }
    // if(keyCode == 1){//Ctrl+Alt+Shift+Z
    //     XBH_U8 data[7] = {0xfc, 0xad, 0x01, 0x02, 0x00, 0x07, 0x1d};
    //     return sendCommand(data, 7);
    // }
    return XBH_FAILURE;
}

static XBH_S32 insmod(const XBH_CHAR *filename, const XBH_CHAR *args)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_S32 fd;

    fd = TEMP_FAILURE_RETRY(open(filename, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd < 0)
    {
        XLOGE("Failed to open %s, err: %s", filename,strerror(errno));
        return XBH_FAILURE;
    }

    ret = syscall(__NR_finit_module, fd, args, 0);
    if (ret < 0)
    {
        XLOGE("finit_module return: %d, err:%s", ret,strerror(errno));
    }
    close(fd);
    return ret;
}

XBH_S32 XbhTouchCommonUsb::openTp(XBH_VOID)
{
    XBH_S32 fd = -1;
    return fd;
}

XBH_S32 XbhTouchCommonUsb::sendCommand(XBH_U8* data, XBH_S32 len)
{
    return XBH_FAILURE;
}

XBH_S32 XbhTouchCommonUsb::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    return XBH_SUCCESS;
}

/**
 * 初始化touch
 * enable 开关状态
 */
XBH_S32 XbhTouchCommonUsb::initTouchProcess(XBH_U32 vid,XBH_U32 pid)
{
    mTouchHandle = dlopen("/vendor/lib64/libxbhtouchprocess.so", RTLD_LAZY);
    if(!mTouchHandle)
    {
        XLOGE("open /vendor/lib64/libxbhtouchprocess.so failed!!!");
        return XBH_FAILURE;
    }

    mInitProcess = (XBH_VOID (*)(XBH_U32, XBH_U32))dlsym(mTouchHandle, "InitProcess");
    mSetTouchState = (XBH_S32 (*)(XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "setTouchState");
    mGetTouchState = (XBH_S32 (*)(XBH_S32 ))dlsym(mTouchHandle, "getTouchState");
    mSetNonThroughTouchRegion = (XBH_S32 (*)(XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 , XBH_S32 ))dlsym(mTouchHandle, "setNonThroughTouchRegion");
    mRemoveNonThroughTouchRegionById = (XBH_S32 (*)(XBH_S32 ))dlsym(mTouchHandle, "removeNonThroughTouchRegionById");
    mIsTouchPanelOk = (XBH_S32 (*)(XBH_VOID))dlsym(mTouchHandle, "isTouchConnected");
    mSetPalmType = (XBH_S32 (*)(XBH_S32 ))dlsym(mTouchHandle, "setPalmType");

    if(mInitProcess)
    {
        mInitProcess(vid,pid);
    }
    else
    {
        XLOGE("initTouchProcess failed !!!!");
    }

    return XBH_SUCCESS;
}


XbhTouchCommonUsb::XbhTouchCommonUsb(XBH_CHAR *ko, XBH_CHAR *tpName,XBH_U32 vid,XBH_U32 pid)
{
    XLOGI("XbhTouchCommonUsb: vid=0x%04x, pid=0x%04x\n",vid,pid);
    mVid = vid;
    mPid = pid;
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

    XLOGI("XbhTouchCommonUsb: mTpName=%s, mKoName=%s\n",mTpName,mKoName);
    if(strcmp(mKoName, "") != 0)
    {
        loadModules();
        usleep(1000*1000); //1000 ms for driver init
    }
    initTouchProcess(vid,pid);
}

XbhTouchCommonUsb::~XbhTouchCommonUsb()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        dlclose(mTouchHandle);
        mInstance = NULL;
        mTouchHandle = NULL;
    }

}

XbhTouchInterface *XbhTouchCommonUsb::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName,XBH_U32 vid,XBH_U32 pid)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchCommonUsb(ko, tpName,vid,pid);
    }
    return mInstance;
}

