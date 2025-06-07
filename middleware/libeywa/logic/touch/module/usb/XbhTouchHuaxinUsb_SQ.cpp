#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchHuaxinUsb_SQ"

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

#include "XbhTouchHuaxinUsb_SQ.h"

XbhTouchHuaxinUsb_SQ*          XbhTouchHuaxinUsb_SQ::mInstance = XBH_NULL;
XbhMutex                       XbhTouchHuaxinUsb_SQ::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchHuaxinUsb_SQ::setSourceTouchState(XBH_BOOL enable)
{
    XBH_U8 data[17] = {0xFC, 0x30, 0x08, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    data[5] = {0x01};
    if (enable) {
        data[6] = {0x02};
    } else {
        data[6] = {0x01};
    }
    return sendCommand(data, 17);
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchHuaxinUsb_SQ::getSourceTouchState(XBH_BOOL *enable)
{
    //*enable = isSrcTouchEnable;
    //XLOGD("getSourceTouchState enable = %d ",*enable);
    return XBH_SUCCESS;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 XbhTouchHuaxinUsb_SQ::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_U8 data[17] = {0xfc, 0xb4, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int xt = 0, yt = 0, wt = 0, ht = 0;
    if(resolution == 0)
    {    
        xt = (x * 32767) / 1920;
        yt = (y * 32767) / 1080;
        wt = (w * 32767) / 1920;
        ht = (h * 32767) / 1080;
    }
    else if(resolution == 1)
    {
        xt = (x * 32767) / 3840;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 3840;
        ht = (h * 32767) / 2160;
    }
    else if(resolution == 2)
    {
        xt = (x * 32767) / 5120;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 5120;
        ht = (h * 32767) / 2160;
    }
    //id
    data[5] = id;
    //x1
    data[6] = (xt & 0xff);
    data[7] = (xt >> 8);
    //y1
    data[8] = (yt & 0xff);
    data[9] = (yt >> 8);
    //x2
    data[10] = ((xt + wt) & 0xff);
    data[11] = ((xt + wt) >> 8);
    //y2
    data[12] = ((yt + ht) & 0xff);
    data[13] = ((yt + ht) >> 8);
    return sendCommand(data, 17);
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchHuaxinUsb_SQ::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_U8 data[17] = {0xfc, 0xb4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    data[5] = id;
    return sendCommand(data, 17);
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
*/
XBH_S32 XbhTouchHuaxinUsb_SQ::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U8 data[6] = {0xFC, 0xB4, 0x06, 0x01, 0x00, 0x02};
    XBH_S32 yt;
    if(strcmp(touchtype, "k2_touch") == 0)
    {
        XLOGD("XbhLangoUsb setTouchVerticalOffset offset:%d\n",y);
        if(y == 0)
        {
            data[5] = 0x00;
            XLOGD("setScreenOffsetHV: reset touch offset");
        }
        else
        {
            if (resolution == 0)
            {
                yt = 1080 / y -1;
            }
            else if (resolution == 1)
            {
                yt = 2160 / y - 1;
            }
            else if(resolution == 2)
            {
                yt = 2160 / y - 1;
            }
            data[5] = (yt & 0xff);
        }
    }
    else
    {
        XBH_U8 new_data[6] = {0xFC, 0x30, 0x32, 0x01, 0x00, 0x00};
        memcpy(data, new_data, sizeof(new_data));
        if(resolution == 0)
        {
            //if the ui is 2k
            switch (y) {
                case 0:
                    // 不降
                    data[5] = 0x00;
                    break;
                case 360:
                    // 降1/3
                    data[5] = 0x03;
                    break;
                case 540:
                    // 降1/2
                    data[5] = 0x01;
                    break;
            }
        }
        else if(resolution == 1)
        {
            //if the ui is 4k
            int yt = y;
            XLOGD(" ZYL:%d \n",yt);
            switch(y) {
                case 0:
                    // 不降
                    data[5] = 0x00;
                    break;
                case 720:
                    // 降1/3
                    data[5] = 0x03;
                    break;
                case 1080:
                    // 降1/2
                    data[5] = 0x01;
                    break;
            }
        }
        else if(resolution == 2)
        {
            //if the ui is 5k
            switch (y) {
                case 0:
                    // 不降
                    data[5] = 0x00;
                    break;
                case 720:
                    // 降1/3
                    data[5] = 0x03;
                    break;
                case 1080:
                    // 降1/2
                    data[5] = 0x01;
                    break;
            }
        }
    }

    return sendCommand(data, 6);
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchHuaxinUsb_SQ::sendKeyToTp(XBH_U32 keyCode)
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

XBH_S32 XbhTouchHuaxinUsb_SQ::openTp(void)
{
    XBH_S32 fd = -1;
    char fileName[16] = "0";
    struct hidraw_devinfo dinfo;

    for (XBH_S32 i = 0; i < HID_MAX_LENGTH; i++) {
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_RDWR);
        if (fd >= 0) {
            if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0) {
                XLOGI("openHuaXinTouchHid  hidraw%d, vid: 0x%x, pid: 0x%x\n", i, dinfo.vendor, dinfo.product);
                if(( dinfo.vendor == 0x2FE9  || dinfo.vendor == 0x2e3d  || dinfo.vendor == 0x2309 ) && (dinfo.product <= 0x7FFF )) {
                    return fd;
                } else {
                    close(fd);
                     fd = -1;
                }
            }
        }
    }
    return fd;
}

XBH_S32 XbhTouchHuaxinUsb_SQ::sendCommand(XBH_U8* data, XBH_S32 len)
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

XBH_S32 XbhTouchHuaxinUsb_SQ::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    return XBH_SUCCESS;
}

XbhTouchHuaxinUsb_SQ::XbhTouchHuaxinUsb_SQ(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mTpName));
    property_get("persist.vendor.xbh.bezel.name", touchtype, "null");
    if(strcmp(touchtype, "k2_touch_legacy") == 0)
    {
        property_set("vendor.xbh.touch.k2","1");
    }
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
        usleep(1000*1000); //1000 ms for driver init
    }
    mFd = openTp();
}

XbhTouchHuaxinUsb_SQ::~XbhTouchHuaxinUsb_SQ()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
        close(mFd);
    }
}

XbhTouchInterface *XbhTouchHuaxinUsb_SQ::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchHuaxinUsb_SQ(ko, tpName);
    }
    return mInstance;
}

