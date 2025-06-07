#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchIsolutionXSdk"

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

#include "XbhTouchIsolutionXSdk.h"

XbhTouchIsolutionXSdk*          XbhTouchIsolutionXSdk::mInstance = XBH_NULL;
XbhMutex                         XbhTouchIsolutionXSdk::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchIsolutionXSdk::setSourceTouchState(XBH_BOOL enable)
{
    XBH_U8 data[6] = {0xa1,0xaa,0x02,0x02,0x00,0x00};
    if (enable) {
        data[2] = {0x02};
    } else {
        data[2] = {0x03};
    }
    return sendCommand(data, 64);
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchIsolutionXSdk::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_SUCCESS;
    return ret;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 XbhTouchIsolutionXSdk::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XbhMutex::Autolock lock(mLockObject);
    XBH_S32 ret = XBH_FAILURE;
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchIsolutionXSdk::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XbhMutex::Autolock lock(mLockObject);
    XBH_S32 ret = XBH_FAILURE;
    return ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchIsolutionXSdk::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
* 获取触摸框的防误触功能开关
*/
XBH_S32 XbhTouchIsolutionXSdk::getPalmRejection(XBH_BOOL* enable)
{
    return  XBH_FAILURE;
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
* x,y,w,h 当前区域的矩形参数
* resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
*/
XBH_S32 XbhTouchIsolutionXSdk::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XbhMutex::Autolock lock(mLockObject);
    return XBH_FAILURE;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchIsolutionXSdk::sendKeyToTp(XBH_U32 keyCode)
{
    return XBH_FAILURE;
}

XBH_S32 XbhTouchIsolutionXSdk::openTp(void) 
{
    XBH_S32 fd = -1;
    char fileName[16] = "0";
    struct hidraw_devinfo dinfo;

    // fd = open(XBH_TOUCH_DATA_NODE, O_RDWR);
    // if (fd < 0)
    // {
    //     XLOGD("[%s:%d] open %s failed !!!!!, errno = %d ", __func__, __LINE__, XBH_TOUCH_DATA_NODE, fd);
    //     return XBH_FAILURE;
    // }
    // else
    // {
    //     mFd = fd;
    //     XLOGD("open Success!!!");
    // }
    XLOGD("getTpFirmwareVersion openTp start!!!");
    for (XBH_S32 i = 0; i < 5; i++)
    {
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_RDWR);
        XLOGD("open %s %d %s", fileName, fd, strerror(errno));
        if (fd >= 0) {
            if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0)
            {
                XLOGI("openTouchHid  hidraw%d, vid: 0x%x, pid: 0x%x\n", i, dinfo.vendor, dinfo.product);
                if(( dinfo.vendor == 0x28e1 ) && (dinfo.product <= 0x7FFF ))
                {
                    mFdCmd = fd;
                    XLOGD("openTouchHid Success!!!");
                    break;
                }
                else
                {
                    close(fd);
                    fd = -1;
                }
            }
        }
    }

    return fd;
}

/**
 *  写数据接口
 */
XBH_S32 XbhTouchIsolutionXSdk::writData(XBH_S32 fd, XBH_U8 *data, XBH_S32 len)
{
    XBH_S32 ret = -1;
    if(fd > 0)
    {
        ret = write(fd, data, len);
        if(ret <= 0)
        {
            xbhTouchStat = XBH_TOUCH_OPEN_DATA_NODE;
            XLOGE("writData (mFd = %d )write error!!!  ret = %d", mFd, ret);
        }
    }
    return ret;
}

/**
 *  读数据接口
 */
XBH_S32 XbhTouchIsolutionXSdk::readData(XBH_S32 fd, XBH_U8 *data, XBH_S32 len)
{
    XBH_S32 ret = 0 ;
    XBH_S32 num = 0 ;
    XBH_S32 wait_seconds = 1;
    if (wait_seconds > 0 && data != NULL && len > 0)
    {
        fd_set read_fdset;
        struct  timeval timeout;
        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do
        {
            ret = select(fd + 1 , &read_fdset,  NULL ,  NULL , &timeout);
        } while(ret <  0);

        if  (ret == 0)
        {
            ret = - 1 ;
        }
        else if(ret == 1)
        {
            num = read(fd,data,len);
            if (num < 0)
            {
                xbhTouchStat = XBH_TOUCH_OPEN_DATA_NODE;
            }
            return num;
        }
    }
    return  ret;
}

XbhTouchIsolutionXSdk::XbhTouchIsolutionXSdk(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XLOGD("XbhTouchIsolutionXSdk create");

    property_set("persist.vendor.xbh.touch.product","Ist_X");
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
    mFd = openTp();
}

XbhTouchIsolutionXSdk::~XbhTouchIsolutionXSdk()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
        if (mTouchHandle)
        {
            dlclose(mTouchHandle);
        }
        if (mFd >= 0)
        {
            close(mFd);
        }
        if (mFdCmd >= 0)
        {
            close(mFdCmd);
        }
    }
}

XbhTouchInterface *XbhTouchIsolutionXSdk::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL)
    {
        mInstance = new XbhTouchIsolutionXSdk(ko, tpName);
    }
    return mInstance;
}


XBH_S32 XbhTouchIsolutionXSdk::sendCommand(XBH_U8* data, XBH_S32 len)
{
    XBH_S32 reTrySend = 10;
    XBH_S32 ret = XBH_FAILURE;
    if (mFd > 0)
    {
        do
        {
            ret = write(mFd, data, 64);
            if (ret > 0)
            {
                return XBH_SUCCESS;
            }
            else
            {
                close(mFd);
                mFd = openTp();
            }
            reTrySend --;
        } while(reTrySend > 0);
    }
    else
    {
        close(mFd);
        mFd = openTp();
    }
    return ret;
}