#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchTimeLinkUart"

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


#include "cutils/properties.h"
#include "XbhTouchTimeLinkUart.h"

#define DEV_TTY_ACM0 "/dev/ttyS1"
#define DEV_TTY_ACM1 "/dev/ttyS2"
#define DEV_TTY_ACM2 "/dev/ttyS3"
#define DEV_TTY_ACM3 "/dev/ttyS4"

XbhTouchTimeLinkUart*           XbhTouchTimeLinkUart::mInstance = XBH_NULL;
XbhMutex                        XbhTouchTimeLinkUart::mLockObject;

static XBH_S32 mFd;
static speed_t getBaudrate(XBH_S32 baudrate)
{
    switch(baudrate) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return B0;
    }
}

static XBH_BOOL openSerialPort(const char *path, XBH_S32 baudrate)
{
	mFd = open(path, O_RDWR|O_NOCTTY|O_NONBLOCK);

	if (-1 == mFd)
	{
		XLOGE("TouchProcess  Can't Open Serial Port");
		return XBH_FALSE;
	}

    speed_t speed;
    struct termios cfg;
    XBH_S32 ret = 0;
    speed = getBaudrate(baudrate);

    if (speed == B0)
    {
		XLOGE("TouchProcess  Can't getBaudrate");
		return XBH_FALSE;
    }

    ret = tcgetattr(mFd, &cfg);
    if (ret != 0)
    {
        close(mFd);
        mFd = -1;
        return XBH_FALSE;
    }

    cfmakeraw(&cfg);
    memset(&cfg, 0, sizeof(struct termios));
    cfg.c_cflag = speed | CS8 | CREAD | CLOCAL;
    cfg.c_iflag = IGNPAR;
    cfg.c_oflag = 0;
    cfg.c_lflag = 0;
    cfg.c_cc[VINTR] = 0;
    cfg.c_cc[VQUIT] = 0;
    cfg.c_cc[VERASE] = 0;
    cfg.c_cc[VKILL] = 0;
    cfg.c_cc[VEOF] = 0;
    cfg.c_cc[VTIME] = 0;
    cfg.c_cc[VMIN] = 1;
    cfg.c_cc[VSWTC] = 0;
    cfg.c_cc[VSTART] = 0;
    cfg.c_cc[VSTOP] = 1;
    cfg.c_cc[VSUSP] = 0;
    cfg.c_cc[VEOL] = 0;
    cfg.c_cc[VREPRINT] = 0;
    cfg.c_cc[VDISCARD] = 0;
    cfg.c_cc[VWERASE] = 0;
    cfg.c_cc[VLNEXT] = 0;
    cfg.c_cc[VEOL2] = 0;

    tcflush(mFd, TCIFLUSH);
    ret = tcsetattr(mFd, TCSANOW, &cfg);

    if (ret != 0)
    {
		close(mFd);
		mFd = -1;
		XLOGE("TouchProcess  Can't tcsetattr");
		return XBH_FALSE;
    }

	XLOGE("openSerialPort:  %s, baudrate = %d ok",path,baudrate);

    return XBH_TRUE;
}

static XBH_U8 CalcCheckSum(XBH_U8* buffer, XBH_U8 size)
{
    XBH_U8 sum = 0x00;

    while (size) {sum += buffer[--size];}

    return sum;
}

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchTimeLinkUart::setSourceTouchState(XBH_BOOL enable)
{
    XLOGD("setSourceTouchState enable = %d", enable);
    
    XBH_U8 buf[9];

    buf[0] = 0x1F;
    buf[1] = 0xF7;
    buf[2] = 0xFC;
    buf[3] = 0x30;
    buf[4] = 0x08;
    buf[5] = 0x01;
    buf[6] = 0x00;
    buf[7] = enable ? 0x01 : 0x00;
    buf[8] = CalcCheckSum(buf, 8);

    XBH_S32 ret = sendCommand(buf, 9);
    if(ret == XBH_SUCCESS)
    {
        isSrcTouchEnable = enable;
    }
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchTimeLinkUart::getSourceTouchState(XBH_BOOL *enable)
{
    *enable = isSrcTouchEnable;
    XLOGD("getSourceTouchState enable = %d ",*enable);
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
XBH_S32 XbhTouchTimeLinkUart::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XLOGD("setNonThroughTouchRegion id = %d resolution = %d x = %d y = %d w = %d h = %d ",id, resolution, x, y, w, h);
        //cmd: 1F F7 FC B4 01 09 00 id x1L x1H y1L y1H x2L x2H y2L y2H checksum
    XBH_U8 setPrPackage[17] = {0x1F, 0xF7, 0xFC, 0xB4, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    XBH_S32 xt = 0, yt = 0, wt = 0, ht = 0;
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
    else if (resolution == 1)
    {
        xt = (x * 32767) / 5120;
        yt = (y * 32767) / 2160;
        wt = (w * 32767) / 5120;
        ht = (h * 32767) / 2160;
    }

    setPrPackage[7] = id;

    setPrPackage[8] = xt & 0xff; // x1L
    setPrPackage[9] = (xt >> 8) & 0xff; //x1H

    setPrPackage[10] = yt & 0xff; //y1L
    setPrPackage[11] = (yt >> 8) & 0xff; //y1H

    setPrPackage[12] = wt & 0xff; // x2L
    setPrPackage[13] = (wt >> 8) & 0xff; //x2H

    setPrPackage[14] = ht & 0xff; //y2L
    setPrPackage[15] = (ht >> 8) & 0xff; //y2H

    setPrPackage[16] = CalcCheckSum(setPrPackage, 16);

    return sendCommand(setPrPackage, 17);
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchTimeLinkUart::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XLOGD("deleteNonThroughTouchRegion id = %d ",id);
    if(id >=0 )
    {
        XBH_U8 delPrPackage[9] = {0x1F,0xF7,0xFC,0xB4, 0x02,0x01,0x00,0x00,0x00};

        delPrPackage[7] = id;
        delPrPackage[8] = CalcCheckSum(delPrPackage, 8);
        return sendCommand(delPrPackage, 9);
    }
    return XBH_FAILURE;
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
*/
XBH_S32 XbhTouchTimeLinkUart::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    return XBH_FAILURE;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchTimeLinkUart::sendKeyToTp(XBH_U32 keyCode)
{
    XLOGD("super sendKeyToTp keyCode = %d ", keyCode);
    return XBH_FAILURE;
}

XBH_S32 XbhTouchTimeLinkUart::insmod(XBH_U8 port)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char cmd[64] ={0};

    snprintf(cmd, sizeof(cmd), "tp_uart_port=%d", port);
    s32Ret = insmod("/vendor/lib/modules/xbhtouch-cfg.ko", cmd);

    if (s32Ret == XBH_FAILURE)
    {
        XLOGE("insmod port %d error\n", port);
        return XBH_FAILURE;
    }

    XLOGI("insmod port %d, ret:%d", port, s32Ret);
    return s32Ret;
}

//vendor modules for usb + usb
XBH_S32 XbhTouchTimeLinkUart::insmod(const char *filename, const char *args)
{
    XBH_S32 ret;
    XBH_S32 fd;

    fd = TEMP_FAILURE_RETRY(open(filename, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd < 0) {
        XLOGE("Failed to open %s", filename);
        return -1;
    }

    ret = syscall(__NR_finit_module, fd, args, 0);

    close(fd);
    if (ret < 0) {
        XLOGE("finit_module return: %d", ret);
    }

    return ret;
}

XBH_S32 XbhTouchTimeLinkUart::sendCommand(const XBH_U8* cmd,XBH_S32 len)
{
    if(!isInitTouch)
    {
        XLOGE("sendCommand error, isInitTouch = %d ",isInitTouch);
        return XBH_FAILURE;
    }

    if(mFd != -1)
    {
        XBH_S32 ret = write(mFd, cmd, len);
        if(ret <= 0)
        {
            XLOGE("sendCommand  error!!!  errno = %d ",errno);
            return XBH_FAILURE;
        }
        else
        {
            //XBH_S32 i = 0;
            XLOGI("sendCommand: write sucess ret = %d ",ret);
            //for(i = 0;i< len; i++)
            //{
            //    XLOGI("%02x\n",cmd[i]);
            //}
        }
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhTouchTimeLinkUart::readCommand(XBH_U8* pbuf, XBH_S32 len)
{
    if(!isInitTouch)
    {
        XLOGE("readCommand error, isInitTouch = %d ",isInitTouch);
        return XBH_FAILURE;
    }
    
    if(mFd != -1)
    {
        XBH_S32 ret = read(mFd, pbuf, len);
        if(ret <= 0)
        {
            XLOGE("readCommand  error!!!  errno = %d ",errno);
            return XBH_FAILURE;
        }
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhTouchTimeLinkUart::Init(XBH_VOID)
{
    XBH_S32 s32BaudRate;
    char uartPath[PROPERTY_VALUE_MAX];

    property_get("persist.vendor.xbh.touchuart.path", uartPath, "null");
    s32BaudRate = property_get_int32("persist.vendor.xbh.touchuart.baudrate", 115200);

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
    else
    {
        XLOGE("init error ,not found tty device !!!! ");
        return XBH_FAILURE;
    }
    isInitTouch = XBH_TRUE;

    return XBH_SUCCESS;
}

XbhTouchTimeLinkUart::XbhTouchTimeLinkUart(XBH_CHAR *ko, XBH_CHAR *tpName)
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
    mFd = -1;
    Init();
}

XbhTouchTimeLinkUart::~XbhTouchTimeLinkUart()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchInterface *XbhTouchTimeLinkUart::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchTimeLinkUart(ko, tpName);
    }
    return mInstance;
}
