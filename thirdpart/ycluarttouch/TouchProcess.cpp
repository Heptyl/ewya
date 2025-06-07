#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
/* Unix */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <unistd.h>
/* Linux */
#include <errno.h>
#include <linux/hidraw.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <linux/version.h>
#include <pthread.h>

#include "cutils/properties.h"
#include <utils/Log.h>

#include "SerialProcess.h"
#include "TouchProcess.h"
//===========================
// for debug
#define TAG "xbh_mw@TouchProcess"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
//===========================

#define HEAD_1 0x1f
#define HEAD_2 0xf7

#define TP_MAX_XC 32767
#define TP_MAX_YC 32767

#define MAX_CONTACTS 40    // 最大40个触电
#define MAX_PACKAGE_LEN 67 // 包长 67个字节
#define MAX_HEAD_LEN 6     // 包头6个字节

#pragma pack()
struct touchData
{
    unsigned char idx;
    unsigned char *data;
};
struct touchData mTouchData;

struct TPoint
{
    unsigned char id;
    unsigned char status;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned char type;
    unsigned int pressure;
    unsigned int orientation;
};

static int uppoint = 0;
static int allPoint = 0;
static int recvPoints = 0;

static struct TPoint mPoints[MAX_CONTACTS];
static char reportToolType[MAX_CONTACTS] = {0}; // 0:no report,1:reported
static char ps[MAX_CONTACTS] = {0};             // press status:0:up,1:down

/*
 * MT_TOOL types
 */
#define MT_TOOL_FINGER 0
#define MT_TOOL_PEN 1
#define MT_TOOL_ERASER 2
#define MT_TOOL_NONE 3

#define MT_TOOL_TYPE_MAX MT_TOOL_NONE

//==========================
// 全局变量定义
pthread_t sthread;
int gFd = -1;
int uinp_fd = 0;

XBH_BOOL isSrcTouchEnable;

static XBH_U8 CalcCheckSum(XBH_U8 *buffer, XBH_U8 size)
{
    XBH_U8 sum = 0x00;

    while (size)
    {
        sum += buffer[--size];
    }

    return sum;
}

static bool device_writeEvent(int fd, uint16_t type, uint16_t keycode, int32_t value)
{
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = type;
    ev.code = keycode;
    ev.value = value;
    if (write(fd, &ev, sizeof(struct input_event)) < 0)
    {
        char *mesg = strerror(errno);
        LOGI("TouchProcess uinput errormag info :%s\n", mesg);
        return false;
    }
    return true;
}

static void work_handler(struct touchData *touchData)
{
    int i = 0;
    int k = 0;
    if (touchData->data[65] > 0)
    {
        allPoint = touchData->data[65];
        uppoint = 0;
        recvPoints = 0;
    }
    k = allPoint - recvPoints;
    if (k > 6)
    {
        k = 6;
    }
    for (i = 0; i < k; i++)
    {
        mPoints[recvPoints].id = touchData->data[(10 * i) + 6];
        mPoints[recvPoints].status = touchData->data[(10 * i) + 5];
        mPoints[recvPoints].x = (int)(touchData->data[10 * i + 8] << 8) + touchData->data[10 * i + 7];
        mPoints[recvPoints].y = (int)(touchData->data[10 * i + 10] << 8) + touchData->data[10 * i + 9];
        mPoints[recvPoints].width = (int)(touchData->data[10 * i + 12] << 8) + touchData->data[10 * i + 11];
        mPoints[recvPoints].height = (int)(touchData->data[10 * i + 14] << 8) + touchData->data[10 * i + 13];
        mPoints[recvPoints].type = MT_TOOL_FINGER;
        recvPoints++;
    }
    if (recvPoints < allPoint)
    {
        ;
    }
    else
    {
        for (i = 0; i < allPoint; i++)
        {
            // press or up
            if (mPoints[i].status == 0x02)
            {
                // press
                if (ps[mPoints[i].id] == 0)
                {
                    ps[mPoints[i].id] = 1;
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, mPoints[i].id); // report down
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].type); // pen,finger
                }
                else
                {
                    uppoint++;
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1); // report up
                    ps[mPoints[i].id] = 0;
                }
            }
            else if (mPoints[i].status == 0x03)
            {
                if (ps[mPoints[i].id] == 0)
                {
                    ps[mPoints[i].id] = 1;
                }
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);
            }
        }
    }
    if (uppoint >= allPoint)
    {
        for (i = 0; i < MAX_CONTACTS; i++)
        {
            if (ps[i] != 0)
            {
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, i);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1); // report up
                ps[i] = 0;
            }
        }
        device_writeEvent(uinp_fd, EV_KEY, BTN_TOUCH, 0);
    }
    else
    {
        device_writeEvent(uinp_fd, EV_KEY, BTN_TOUCH, 1);
    }
    device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
}

XBH_S32 createTouchScreen()
{
    struct uinput_user_dev uinp;
    struct input_event event;

    LOGI("createTouchScreen...");
    uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinp_fd < 0)
    {
        LOGI("TouchProcess   Unable to open /dev/uinput");
        LOGI("err: %s", strerror(errno));
        return XBH_FAILURE;
    }

    // configure touch device event properties
    memset(&uinp, 0, sizeof(uinp));
    strncpy(uinp.name, "Xbh_TouchScreen", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 0x0100;
    uinp.id.vendor = 0x365f;
    uinp.id.product = 0x4003;
    uinp.id.bustype = BUS_USB;
    uinp.absmin[ABS_MT_SLOT] = 0;
    uinp.absmax[ABS_MT_SLOT] = MAX_CONTACTS;
    uinp.absmin[ABS_MT_TOUCH_MAJOR] = 0;
    uinp.absmax[ABS_MT_TOUCH_MAJOR] = TP_MAX_XC;
    uinp.absmin[ABS_MT_POSITION_X] = 0;
    uinp.absmax[ABS_MT_POSITION_X] = TP_MAX_XC;
    uinp.absmin[ABS_MT_POSITION_Y] = 0;
    uinp.absmax[ABS_MT_POSITION_Y] = TP_MAX_YC;
    uinp.absmin[ABS_MT_TRACKING_ID] = 0;
    uinp.absmax[ABS_MT_TRACKING_ID] = 65535;
    uinp.absmin[ABS_MT_TOOL_TYPE] = 0;
    uinp.absmax[ABS_MT_TOOL_TYPE] = MT_TOOL_TYPE_MAX + 1;
    // Setup the uinput device
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOOL_RUBBER);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
    // Touch
    ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_SLOT);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
    ioctl(uinp_fd, UI_SET_ABSBIT, ABS_MT_TOOL_TYPE);
    ioctl(uinp_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);

    /* Create input device into input sub-system */
    write(uinp_fd, &uinp, sizeof(uinp));
    ioctl(uinp_fd, UI_DEV_CREATE);
    LOGI("create XbhTouchScreen ok!");
    return XBH_SUCCESS;
}

XBH_S32 getHeadData(XBH_UCHAR *data, XBH_BOOL *touchData)
{
    // 还需要继续获取的数据长度
    XBH_S32 remainLen = 0;
    // 设置第二路USB数据开关时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xfe && data[4] == 0x81 && data[5] == 0x02)
    {
        LOGD(" set source touch  ack ");
        remainLen = 4;
        *touchData = XBH_FALSE;
    }
    // 添加不穿透区域时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x81 && data[5] == 0x02)
    {
        LOGD(" add touch area ack ");
        remainLen = 4;
        *touchData = XBH_FALSE;
    }
    // 删除不穿透区域时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x82 && data[5] == 0x02)
    {
        LOGD(" del touch area ack ");
        remainLen = 4;
        *touchData = XBH_FALSE;
    }
    // 查询不穿透区域时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x83 && data[5] == 0x0A)
    {
        LOGD(" que touch area ack ");
        remainLen = 8;
        *touchData = XBH_FALSE;
    }
#if 0
    //查询当前有效区域数与ID时返回值
    if(data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x84 )
    {
        remainLen = 8;
        *touchData = XBH_FALSE;
    }
#endif

    // 触摸数据
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0x43 && data[3] == 0x00 && data[4] == 0x02)
    {
        remainLen = 61;
        *touchData = XBH_TRUE;
    }
    return remainLen;
}

void *touchDataProcess_thread(void *arg)
{
    XBH_UNUSED(arg);
    memset(mPoints, 0, sizeof(mPoints));
    mTouchData.data = (unsigned char *)malloc(MAX_PACKAGE_LEN);
    mTouchData.idx = 0;
    memset(mTouchData.data, 0, sizeof(mTouchData));
    memset(reportToolType, 0, sizeof(reportToolType));

    LOGI("touchDataProcess_thread start ....");
    XBH_S32 datalen = 0;               // 收取到数据有效字节
    XBH_BOOL isTouchData = XBH_FALSE;  // 判断当前是否是touch数据，如果是命令数据就不需要做报点处理
    XBH_U8 checkSum = 0;               // 完整一包触摸数据的checksum
    XBH_BOOL needFindHead = XBH_FALSE; // 当获取到错误的数据时，需要重新去找数据包头
    while (1)
    {
        // 当数据发生错乱时，需要重新找到数据包头，此时需要一个个字节的接收
        if (needFindHead)
        {
            datalen = treadn(gFd, &mTouchData.data[0], 1, 1);
            if (mTouchData.data[0] == HEAD_1)
            {
                datalen = treadn(gFd, &mTouchData.data[1], 1, 1);
                // 接收到头部
                if (mTouchData.data[1] == HEAD_2)
                {
                    // 收到头部后就连续接收4个字节，并判断是否是触摸数据
                    datalen = treadn(gFd, &mTouchData.data[2], 4, 1);
                    if (mTouchData.data[2] == MAX_PACKAGE_LEN && mTouchData.data[3] == 0x00 && mTouchData.data[4] == 0x02)
                    {
                        LOGI(" find head !!!!!");
                        datalen = MAX_HEAD_LEN;
                        needFindHead = XBH_FALSE;
                    }
                }
            }
        }
        else
        {
            // 先获取头部6个字节的数据
            datalen = treadn(gFd, mTouchData.data, MAX_HEAD_LEN, 1);
        }
        if (datalen >= MAX_HEAD_LEN)
        {
            isTouchData = XBH_FALSE;
            // 获取这包数据剩余未获取的数据，数据从数组的第六个字节开始放置
            datalen = getHeadData(mTouchData.data, &isTouchData);
            // 头部普配错误，重新找数据头
            if (datalen == 0)
            {
                checkSum = 0;
                needFindHead = XBH_TRUE;
                LOGI(" head data has error, need to find head !!!!!");
                continue;
            }
            treadn(gFd, &mTouchData.data[6], datalen, 1);
            if (isTouchData)
            {
                // LOGD(" work_handler begin ");
                checkSum = 0;
                for (int i = 0; i < (MAX_PACKAGE_LEN - 1); i++)
                {
                    checkSum += mTouchData.data[i];
                }
                if (checkSum != mTouchData.data[MAX_PACKAGE_LEN - 1])
                {
                    checkSum = 0;
                    needFindHead = XBH_TRUE;
                    LOGI(" touch data has error, need to find head !!!!!");
                    continue;
                }
                // 触摸上报
                work_handler(&mTouchData);
            }
        }
#if 0
        else
        {
            LOGI("touchDataProcess_thread get datalen :%d erro \n",datalen);
        }
#endif
    }
    return (void *)0;
}

XBH_S32 initProcess(const XBH_CHAR *path, XBH_U32 bandrate)
{
    // 打开串口触摸
    gFd = openSerialPort(path, bandrate);
    if (gFd != -1)
    {
        // 创建虚拟的触摸设备节点
        if (createTouchScreen() == XBH_SUCCESS)
        {
            pthread_attr_t process_attr;
            pthread_attr_init(&process_attr);
            pthread_attr_setdetachstate(&process_attr, PTHREAD_CREATE_DETACHED);
            if (pthread_create(&sthread, &process_attr, touchDataProcess_thread, (void *)NULL) != 0)
            {
                LOGI("Failed to create TouchProcess thread!!!!!!!");
            }
            return XBH_SUCCESS;
        }
    }
    return XBH_FAILURE;
}

/**
 * 设置信号源的触摸开关。
 * Byte0 Byte1 Byte2 Byte3 Byte4 Byte5 Byte6 Byte7 Byte8
 * 1F    F7    FC    30    08    01    00    01    校验和    开启USB触摸
 * 1F    F7    FC    30    08    01    00    00    校验和    关闭USB触摸
 * enable 开关状态
 */
XBH_S32 setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
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

    ret = sendCommand(buf, 9);
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
XBH_S32 getSourceTouchState(XBH_BOOL *enable)
{
    *enable = isSrcTouchEnable;
    return XBH_SUCCESS;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_U8 setPrPackage[17] = {0x1F, 0xF7, 0xFC, 0xB4, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    XBH_S32 xt = 0, yt = 0, wt = 0, ht = 0;
    if (resolution == 0)
    {
        xt = (x * 32767) / 1920;
        yt = (y * 32767) / 1080;
        wt = ((x + w) * 32767) / 1920;
        ht = ((y + h) * 32767) / 1080;
    }
    else if (resolution == 1)
    {
        xt = (x * 32767) / 3840;
        yt = (y * 32767) / 2160;
        wt = ((x + w) * 32767) / 3840;
        ht = ((y + h) * 32767) / 2160;
    }
    else if (resolution == 2)
    {
        xt = (x * 32767) / 5120;
        yt = (y * 32767) / 2160;
        wt = ((x + w) * 32767) / 5120;
        ht = ((y + h) * 32767) / 2160;
    }

    setPrPackage[7] = id;

    setPrPackage[8] = xt & 0xff;        // x1L
    setPrPackage[9] = (xt >> 8) & 0xff; // x1H

    setPrPackage[10] = yt & 0xff;        // y1L
    setPrPackage[11] = (yt >> 8) & 0xff; // y1H

    setPrPackage[12] = wt & 0xff;        // x2L
    setPrPackage[13] = (wt >> 8) & 0xff; // x2H

    setPrPackage[14] = ht & 0xff;        // y2L
    setPrPackage[15] = (ht >> 8) & 0xff; // y2H

    setPrPackage[16] = CalcCheckSum(setPrPackage, 16);

    return sendCommand(setPrPackage, 17);
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    if (id >= 0)
    {
        XBH_U8 delPrPackage[9] = {0x1F, 0xF7, 0xFC, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00};

        delPrPackage[7] = id;
        delPrPackage[8] = CalcCheckSum(delPrPackage, 8);
        return sendCommand(delPrPackage, 9);
    }
    return XBH_FAILURE;
}

/**
 *  触摸框 设置屏幕下移
 */
XBH_S32 setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_U8 setPrPackage[17] = {0x1F, 0xF7, 0xFC, 0xC5, 0x01, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    XBH_S32 x1t = 0, y1t = 0, x2t = 0, y2t = 0;

    // x, w, h 固定值是因为仅实现了屏幕下移, 小窗的缩放没法兼容
    if (resolution == 0)
    {
        x = 0;
        w = 1920;
        h = 1080;
        x1t = (x * 32767) / 1920;
        y1t = (y * 32767) / 1080;
        x2t = ((x + w) * 32767) / 1920;
        y2t = ((y + h) * 32767) / 1080;
    }
    else if (resolution == 1)
    {
        x = 0;
        w = 3840;
        h = 2160;
        x1t = (x * 32767) / 3840;
        y1t = (y * 32767) / 2160;
        x2t = ((x + w) * 32767) / 3840;
        y2t = ((y + h) * 32767) / 2160;
    }
    else if (resolution == 2)
    {
        x = 0;
        w = 5120;
        h = 2160;
        x1t = (x * 32767) / 5120;
        y1t = (y * 32767) / 2160;
        x2t = ((x + w) * 32767) / 5120;
        y2t = ((y + h) * 32767) / 2160;
    }

    LOGI("setTouchScaleRect (%d, %d) (%d, %d)", x, y, x + w, y + h);
    LOGI("setTouchScaleRect Touch (0x%04x, 0x%04x) (0x%04x, 0x%04x)", x1t, y1t, x2t, y2t);

    setPrPackage[8] = x1t & 0xff;        // x1L
    setPrPackage[9] = (x1t >> 8) & 0xff; // x1H

    setPrPackage[10] = y1t & 0xff;        // y1L
    setPrPackage[11] = (y1t >> 8) & 0xff; // y1H

    setPrPackage[12] = x2t & 0xff;        // x2L
    setPrPackage[13] = (x2t >> 8) & 0xff; // x2H

    setPrPackage[14] = y2t & 0xff;        // y2L
    setPrPackage[15] = (y2t >> 8) & 0xff; // y2H

    setPrPackage[16] = CalcCheckSum(setPrPackage, 16);

    return sendCommand(setPrPackage, 17);
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 sendKeyToTp(XBH_U32 keyCode)
{
    return XBH_FAILURE;
}

/**
 * 发送数据到触摸框
 */
XBH_S32 sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    return XBH_FAILURE;
}

/**
 * 设置触摸框的防误触功能开关
 */
XBH_S32 setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    return ret;
}

/**
 * 获取触摸框的防误触功能开关
 */
XBH_S32 getPalmRejection(XBH_BOOL *enable)
{
    return XBH_FAILURE;
}

/**
 * 获取触摸框的连接状态
 */
XBH_S32 getTouchPanelConnect(XBH_BOOL *enable)
{
    return XBH_FAILURE;
}
