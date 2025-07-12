#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchTimeLinkH3Sdk"

#include "XbhLog.h"
#include "cutils/properties.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/hidraw.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "XbhTouchInterfaceAuto.h"
#include "XbhTouchTimeLinkH3Sdk.h"

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <dlfcn.h>//RTLD_LAZY  dlsym
//#include <ctype.h>

/**
 * 适配机型:
 * - Promethean APLE XM.R3567.B
 * - TCL XM.R3567.B
 *
 * 协议: 天英双USB协议V0.1-20240423(2025/4/11)
 *
 */

XbhTouchTimeLinkH3Sdk *XbhTouchTimeLinkH3Sdk::mInstance = XBH_NULL;
XbhMutex XbhTouchTimeLinkH3Sdk::mLockObject;

// Android下的控制USB
#define TIMELINK_CTL_VID 0x0d48
#define TIMELINK_CTL_PID 0x001d

// PC触摸数据USB
#define TIMELINK_PC_VID 0x0d48
#define TIMELINK_PC_PID 0x001b

static XBH_S32 mFd;
static XBH_S32 reTrySend = 10;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::setSourceTouchState(XBH_BOOL enable)
{
    XLOGD("setSourceTouchState enable = %d", enable);

    XBH_U8 data[9] = {0xFC, 0x30, 0x08, 0x02, 0x00, 0x00, 0x02, 0x00, 0x00};
    if (enable)
    {
        data[6] = {0x02};
    }
    else
    {
        data[6] = {0x01};
    }

    XBH_S32 ret = sendCommand(data, 9);
    if (ret == XBH_SUCCESS)
    {
        isSrcTouchEnable = enable;
    }
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::getSourceTouchState(XBH_BOOL *enable)
{
    *enable = isSrcTouchEnable;
    XLOGD("getSourceTouchState enable = %d ", *enable);
    return XBH_SUCCESS;
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
XBH_S32 XbhTouchTimeLinkH3Sdk::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XLOGD("setNonThroughTouchRegion id = %d resolution = %d x = %d y = %d w = %d h = %d ", id, resolution, x, y, w, h);
    XBH_U8 data[17] = {0xFC, 0xB4, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int xt = 0, yt = 0, wt = 0, ht = 0;
    if (resolution == 0)
    {
        xt = (x * 32767) / 1920;
        yt = (y * 32767) / 1080;
        wt = (w * 32767) / 1920;
        ht = (h * 32767) / 1080;
    }
    else if (resolution == 1)
    {
        xt = (x * 32767) / 3840;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 3840;
        ht = (h * 32767) / 2160;
    }
    else if (resolution == 2)
    {
        xt = (x * 32767) / 5120;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 5120;
        ht = (h * 32767) / 2160;
    }

    // id
    data[5] = id;
    // x1
    data[6] = (xt & 0xff);
    data[7] = (xt >> 8);
    // y1
    data[8] = (yt & 0xff);
    data[9] = (yt >> 8);
    // x2
    data[10] = ((xt + wt) & 0xff);
    data[11] = ((xt + wt) >> 8);
    // y2
    data[12] = ((yt + ht) & 0xff);
    data[13] = ((yt + ht) >> 8);

    return sendCommand(data, 17);
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XLOGD("deleteNonThroughTouchRegion id = %d ", id);
    if (id >= 0)
    {
        XBH_U8 data[17] = {0xFC, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        data[5] = id;
        return sendCommand(data, 17);
    }
    return XBH_FAILURE;
}

/**
 * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
 * ToucchVersion >= 1.4.6
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XLOGD("setTouchScaleRect resolution = %d x = %d y = %d w = %d h = %d ", resolution, x, y, w, h);
    XBH_U8 data[17] = {0xFC, 0xC3, 0x02, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int xt = 0, yt = 0, wt = 0, ht = 0;
    if (resolution == 0)
    {
        xt = (x * 32767) / 1920;
        yt = (y * 32767) / 1080;
        wt = (w * 32767) / 1920;
        ht = (h * 32767) / 1080;
    }
    else if (resolution == 1)
    {
        xt = (x * 32767) / 3840;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 3840;
        ht = (h * 32767) / 2160;
    }
    else if (resolution == 2)
    {
        xt = (x * 32767) / 5120;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 5120;
        ht = (h * 32767) / 2160;
    }

    XLOGD("setTouchScaleRect xt = %d yt = %d wt = %d ht = %d ", xt, yt, wt, ht);

    // x1
    data[5] = (xt & 0xff);
    data[6] = (xt >> 8);
    // y1
    data[7] = (yt & 0xff);
    data[8] = (yt >> 8);
    // x2
    data[9] = ((xt + wt) & 0xff);
    data[10] = ((xt + wt) >> 8);
    // y2
    data[11] = ((yt + ht) & 0xff);
    data[12] = ((yt + ht) >> 8);

    return sendCommand(data, 17);
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::sendKeyToTp(XBH_U32 keyCode)
{
    XLOGD("sendKeyToTp keyCode = %d unimplemented", keyCode);
    return XBH_FAILURE;
}

/**
 * 设置触摸框的防误触功能开关
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::setPalmRejection(XBH_BOOL enable)
{
    XLOGD("setPalmRejection enable = %d", enable);

    XBH_U8 data[9] = {0xFC, 0xD0, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (enable)
    {
        data[5] = {0x01};
    }
    else
    {
        data[5] = {0x00};
    }

    XBH_S32 ret = sendCommand(data, 9);
    if (ret == XBH_SUCCESS)
    {
        isSrcTouchEnable = enable;
    }
    return ret;
}

/**
 * 获取触摸框的防误触功能开关
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::getPalmRejection(XBH_BOOL *enable)
{
    *enable = isPalmRejectionEnable;
    XLOGD("getPalmRejection enable = %d ", *enable);
    return XBH_SUCCESS;
}

/**
 * 发送数据到触摸框
 */
XBH_S32 XbhTouchTimeLinkH3Sdk::sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    XLOGD("sendDataToTp length = %d ", length);

    if (length >= 64)
    {
        XLOGE("sendDataToTp failed, length(%d) >=64 ", length);
        return XBH_FAILURE;
    }

    XBH_U8 data[64] = {0x00};

    for (XBH_U32 i = 0; i < 64; i++)
    {
        data[i] = (touchData[i] & 0xff);
    }

    XBH_S32 ret = sendCommand(data, length);
    if (ret != XBH_SUCCESS)
    {
        XLOGE("sendDataToTp sendCommand failed");
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

// vendor modules for usb + usb
XBH_S32 XbhTouchTimeLinkH3Sdk::insmod(const char *filename, const char *args)
{
    XLOGD("insmod filename = %s args = %s", filename, args);
    XBH_S32 ret;
    XBH_S32 fd;

    fd = TEMP_FAILURE_RETRY(open(filename, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd < 0)
    {
        XLOGE("Failed to open %s", filename);
        return -1;
    }

    ret = syscall(__NR_finit_module, fd, args, 0);

    close(fd);
    if (ret < 0)
    {
        XLOGE("finit_module return: %d", ret);
    }

    return ret;
}

XBH_S32 XbhTouchTimeLinkH3Sdk::sendCommand(XBH_U8 *data, XBH_S32 len)
{
    if (!isInitTouch)
    {
        XLOGE("sendCommand error, isInitTouch = %d", isInitTouch);
        return XBH_FAILURE;
    }

    XBH_S32 ret = XBH_FAILURE;

    do
    {
        if (mFd < 0)
        {
            XLOGE("sendCommand error, mFd = %d, reopen touch panel", mFd);
            close(mFd);
            usleep(100 * 1000);
            mFd = openTp();
        }

        ret = write(mFd, data, len);
        usleep(50 * 1000);
        if (ret > 0)
        {
            reTrySend = 10;
            return XBH_SUCCESS;
        }
        else
        {
            mFd = -1;
        }
        reTrySend--;
    } while (reTrySend > 0);

    return ret;
}

XBH_S32 XbhTouchTimeLinkH3Sdk::readCommand(XBH_U8 *pbuf, XBH_S32 len)
{
    if (!isInitTouch)
    {
        XLOGE("sendCommand error, isInitTouch = %d", isInitTouch);
        return XBH_FAILURE;
    }

    if (mFd < 0)
    {
        XLOGE("sendCommand error, mFd = %d, reopen", mFd);
        close(mFd);
        mFd = openTp();
    }

    XBH_S32 ret = read(mFd, pbuf, len);
    if (ret <= 0)
    {
        XLOGE("readCommand error, errno = %d ", errno);
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhTouchTimeLinkH3Sdk::openTp(void)
{
    XBH_S32 fd = -1;
    char fileName[16] = "0";
    int desc_size;
    struct hidraw_devinfo dinfo;
    struct hidraw_report_descriptor desc;

    memset(&dinfo, 0x0, sizeof(dinfo));
    memset(&desc, 0x0, sizeof(desc));

    for (XBH_S32 i = 0; i < HID_MAX_LENGTH; i++)
    {
        fd = -1;
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_WRONLY | O_NONBLOCK);
        if (fd < 0)
        {
            XLOGE("open hidraw%d failed, %s", i, strerror(errno));
            continue;
        }

        if (ioctl(fd, HIDIOCGRAWINFO, &dinfo))
        {
            XLOGE("ioctl hidraw%d HIDIOCGRAWINFO failed, %s", i, strerror(errno));
            close(fd);
            continue;
        }

        XLOGI("open hidraw%d  ID %04x:%04x", i, dinfo.vendor, dinfo.product);
        if (dinfo.vendor != TIMELINK_CTL_VID || dinfo.product != TIMELINK_CTL_PID)
        {
            close(fd);
            continue;
        }

        // Identify the correct hidraw device among two or more using the report_descriptor.
        XLOGD("find touch hidraw%d  ID %04x:%04x", i, dinfo.vendor, dinfo.product);
        if (ioctl(fd, HIDIOCGRDESCSIZE, &desc_size))
        {
            XLOGE("ioctl hidraw%d HIDIOCGRDESCSIZE failed, %s", i, strerror(errno));
            close(fd);
            continue;
        }

        desc.size = desc_size;
        if (ioctl(fd, HIDIOCGRDESC, &desc))
        {
            XLOGE("ioctl hidraw%d HIDIOCGRDESC failed, %s", i, strerror(errno));
            close(fd);
            continue;
        }

        XLOGI("touch hidraw%d report_descriptor.value[1]: 0x%02x", i, desc.value[1]);
        if (desc.value[1] == 0x00) // this hidraw we can use
        {
            return fd;
        }
    }

    XLOGE("touch hidraw not found.");
    return -1;
}

XbhTouchTimeLinkH3Sdk::XbhTouchTimeLinkH3Sdk(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    // property_set("vendor.xbh.touch.timelink","1");
    property_set("persist.vendor.xbh.touch.product", "TimeLinkH3");
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mTpName));
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

    mFd = openTp();

    mTouchHandle = dlopen("/vendor/lib64/libtimelinkh3touch.so", RTLD_LAZY);
    if(!mTouchHandle)
    {
        XLOGE("open libtimelinkh3touch.so fail.");
        return;
    }

    mInitProcess = (void (*)(void))dlsym(mTouchHandle, "initTouchProcess");

    if(mInitProcess)
    {
        mInitProcess();
    }
    else
    {
        XLOGE("Init touchProcess fail.");
    }

    isInitTouch = XBH_TRUE;
    isSrcTouchEnable = XBH_FALSE;
}

XbhTouchTimeLinkH3Sdk::~XbhTouchTimeLinkH3Sdk()
{
    if (mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchTimeLinkH3Sdk *XbhTouchTimeLinkH3Sdk::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL)
    {
        mInstance = new XbhTouchTimeLinkH3Sdk(ko, tpName);
    }
    return mInstance;
}
