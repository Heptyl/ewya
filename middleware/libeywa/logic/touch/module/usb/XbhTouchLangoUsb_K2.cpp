#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchLangoUsb_K2"

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

#include "XbhTouchLangoUsb_K2.h"

XbhTouchLangoUsb_K2*          XbhTouchLangoUsb_K2::mInstance = XBH_NULL;
XbhMutex                      XbhTouchLangoUsb_K2::mLockObject;

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchLangoUsb_K2::setSourceTouchState(XBH_BOOL enable)
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
XBH_S32 XbhTouchLangoUsb_K2::getSourceTouchState(XBH_BOOL *enable)
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
XBH_S32 XbhTouchLangoUsb_K2::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
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
XBH_S32 XbhTouchLangoUsb_K2::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_U8 data[17] = {0xfc, 0xb4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    data[5] = id;
    return sendCommand(data, 17);
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
*/
XBH_S32 XbhTouchLangoUsb_K2::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    const struct { XBH_S32 Width; XBH_S32 Height; }  ResolutionTabs[] = { {1920,1080},{3840,2160},{5210,2160} };        //分辨率表格
    const struct { XBH_S32 YValue[3]; } VerticalOffset[] = {{0,540,360},{0,1080,720},{0,1080,720}};        //垂直偏移表格

    XLOGD("XbhLangoUsb Touch Scale.resolution:%d x:%d y:%d w:%d h:%d \n", resolution,x,y,w,h);
    if (resolution >= sizeof(ResolutionTabs) / sizeof(ResolutionTabs[0]))
    {
        XLOGD("XbhLangoUsb Error:Resolution is not supported\n");
        return XBH_FAILURE;
    }

    XBH_S32 DisplayHeight = ResolutionTabs[resolution].Height;
    XBH_S32 DisplayWidth  = ResolutionTabs[resolution].Width;

    if ((w == DisplayWidth) && (x==0)  &&((h + y) == DisplayHeight))
    {
        for (int i = 0; i < sizeof(VerticalOffset) / sizeof(VerticalOffset[0]); i++)
            {
                if (y == VerticalOffset[resolution].YValue[i])
                {
                    h = h + y; //纠正shuju
                    break;
                }
            }
    }

    XBH_U8 ScalablePacket[18] = { 0xFC, 0xB4, 0x0F, 0x0D, 0x00 };

    ScalablePacket[5] = 1;
    ScalablePacket[6] = (DisplayWidth & 0xFF);
    ScalablePacket[7] = (DisplayWidth >> 8);
    ScalablePacket[8] = (DisplayHeight & 0xFF);
    ScalablePacket[9] = (DisplayHeight >> 8);
    ScalablePacket[10] = (w & 0xFF);
    ScalablePacket[11] = (w >> 8);
    ScalablePacket[12] = (h & 0xFF);
    ScalablePacket[13] = (h >> 8);
    ScalablePacket[14] = (x & 0xFF);
    ScalablePacket[15] = (x >> 8);
    ScalablePacket[16] = (y & 0xFF);
    ScalablePacket[17] = (y >> 8);  
    return sendCommand ((XBH_U8 *)ScalablePacket, 18L);
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchLangoUsb_K2::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    for (int i = 7; i >= 0; i--)
    {
        XLOGD("%d", (enable >> i) & 1);  // 输出每一位的值
    }
    XBH_U8 data[6] = {0xFC, 0xB4, 0x10, 0x01, 0x00, 0x00};
    if(enable)
    {
        data[5] = 0x01;
        return sendCommand(data, 6);
    }
    else
    {
        data[5] = 0x00;
        return sendCommand(data, 6);
    }
    return ret;
}

/**
* 获取触摸框的防误触功能开关
*/
XBH_S32 XbhTouchLangoUsb_K2::getPalmRejection(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;

    *enable = XBH_FALSE;

    return ret;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchLangoUsb_K2::sendKeyToTp(XBH_U32 keyCode)
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

XBH_S32 XbhTouchLangoUsb_K2::openTp(void)
{
    XBH_S32 fd = -1;
    char fileName[16] = "0";
    struct hidraw_devinfo dinfo;

    for (XBH_S32 i = 0; i < HID_MAX_LENGTH; i++)
    {
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_RDWR);
        if (fd >= 0) {
            if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0)
            {
                XLOGI("openTouchHid  hidraw%d, vid: 0x%x, pid: 0x%x\n", i, dinfo.vendor, dinfo.product);
                if(( dinfo.vendor == 0x365f ) && (dinfo.product == 0x4003 ))
                {
                    return fd;
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

XBH_S32 XbhTouchLangoUsb_K2::sendCommand(XBH_U8* data, XBH_S32 len)
{

#if 0
    char str[128] = { 0 };
    int count = 0;
    for (int i = 0; i < len; i++)
    {
        count += sprintf(str + count, "%02X ", data[i]);
    }
    XLOGD("XbhLangoUsb Send Command: %s", str);
#endif

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

XbhTouchLangoUsb_K2::XbhTouchLangoUsb_K2(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    property_set("vendor.xbh.touch.k2.start","1");
    usleep(1000*1000); //1000 ms for driver init
    mFd = openTp();
}

XbhTouchLangoUsb_K2::~XbhTouchLangoUsb_K2()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
        close(mFd);
    }
}

XbhTouchInterface *XbhTouchLangoUsb_K2::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchLangoUsb_K2(ko, tpName);
    }
    return mInstance;
}

