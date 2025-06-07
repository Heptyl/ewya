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

#define TAG "xbh_mw@TouchProcess_istuarttouch"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)

/**
 * 富创通(Isolution Ist)为炫视 睿屏定做的协议, G框 F框.
 *
 * 参考文档:
 *  - 炫视 触摸串口（带宽度和穿透）和上拉及 PC 界面下拉协议
 *  - 炫视 串口快捷键协议
 */

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

/**
 * 接收到的帧类型
 */
enum RecvType
{
    RecvUnknow = -1,
    RecvTouchData = 0,
    RecvHotkey = 1,
    RecvSetSourceStateAck = 2,
    RecvAddTouchAreaAck = 3,
    RecvDelTouchAreaAck = 4,
    RecvQueryTouchAreaAck = 5,
    RecvQueryTouchAreaIdAck = 6,
};

/*
 * MT_TOOL types
 */
#define MT_TOOL_FINGER 0
#define MT_TOOL_PEN 1
#define MT_TOOL_ERASER 2
#define MT_TOOL_NONE 3

#define MT_TOOL_TYPE_MAX MT_TOOL_NONE

/**
 * Hotkey
 * 白板相关的按键，一般为 快捷按键板
 */
#define KEYCODE_WB_ADD_PAGE 744   // 新增一页
#define KEYCODE_WB_BLUE_PEN 745   // 蓝色笔
#define KEYCODE_WB_BLACK_PEN 746  // 黑色笔
#define KEYCODE_WB_RED_PEN 747    // 红色笔
#define KEYCODE_WB_YELLOW_PEN 748 // 荧光笔（黄色笔）
#define KEYCODE_WB_ERASER 749     // 橡皮擦
#define KEYCODE_WB_UNDO 750       // 撤回

//==========================
// 全局变量定义
pthread_t sthread;
int gFd = -1;
int uinp_fd = 0;

XBH_BOOL isSrcTouchEnable;
XBH_BOOL isPalmRejectionEnable;

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
        LOGE("failed to write intput event :%s", strerror(errno));
        return false;
    }
    return true;
}

void sendKeyEvent(unsigned short keyCode, int isDown)
{
    struct input_event event;
    if (uinp_fd <= 0)
    {
        LOGI("sendKeyEvent: uinp_fd <= 0\n");
        return;
    }
    /* report key event */
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = keyCode;
    event.value = isDown;
    write(uinp_fd, &event, sizeof(event));

    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    write(uinp_fd, &event, sizeof(event));
}

static void convertTouchData(struct touchData *touchData)
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

static void convertHotkey(struct touchData *touchData)
{
    unsigned char readKeyCode = touchData->data[3];
    unsigned char readSide = touchData->data[5];
    uint16_t keyCode = 0x00;

    switch (readKeyCode)
    {
    case 0x01:
    case 0x08:
        keyCode = KEYCODE_WB_ADD_PAGE;
        break;
    case 0x02:
    case 0x09:
        keyCode = KEYCODE_WB_BLUE_PEN;
        break;
    case 0x03:
    case 0x0A:
        keyCode = KEYCODE_WB_BLACK_PEN;
        break;
    case 0x04:
    case 0x0B:
        keyCode = KEYCODE_WB_RED_PEN;
        break;
    case 0x05:
    case 0x0C:
        keyCode = KEYCODE_WB_YELLOW_PEN;
        break;
    case 0x06:
    case 0x0D:
        keyCode = KEYCODE_WB_ERASER;
        break;
    case 0x07:
    case 0x0E:
        keyCode = KEYCODE_WB_UNDO;
        break;
    default:
        LOGE("unknow keycode: %d 0x%02X", readKeyCode, readKeyCode);
        return;
    }

    LOGI("sendKeyEvent: rawCode = %d keyCode = %d", readKeyCode, keyCode);

    sendKeyEvent(keyCode, 1);
    device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);

    sendKeyEvent(keyCode, 0);
    device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
}

XBH_S32 createTouchScreen()
{
    struct uinput_user_dev uinp;

    LOGI("createTouchScreen...");
    uinp_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (uinp_fd < 0)
    {
        LOGE("open /dev/uinput failed (%s)", strerror(errno));
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
    // Hotkey
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_ADD_PAGE);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_BLUE_PEN);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_BLACK_PEN);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_RED_PEN);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_YELLOW_PEN);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_ERASER);
    ioctl(uinp_fd, UI_SET_KEYBIT, KEYCODE_WB_UNDO);
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

XBH_S32 getHeadData(XBH_UCHAR *data, RecvType *recvType)
{
    // 还需要继续获取的数据长度
    XBH_S32 remainLen = 0;
    // 设置第二路USB数据开关时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xfe && data[4] == 0x81 && data[5] == 0x02)
    {
        // LOGD(" <= set source touch ack ");
        remainLen = 4;
        *recvType = RecvSetSourceStateAck;
    }
    // 添加不穿透区域时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x81 && data[5] == 0x02)
    {
        // LOGD(" <= add touch area ack ");
        remainLen = 4;
        *recvType = RecvAddTouchAreaAck;
    }
    // 删除不穿透区域时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x82 && data[5] == 0x02)
    {
        // LOGD(" <= del touch area ack ");
        remainLen = 4;
        *recvType = RecvDelTouchAreaAck;
    }
    // 查询不穿透区域时的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x83 && data[5] == 0x0A)
    {
        // LOGD(" <= query touch area ack ");
        remainLen = 8;
        *recvType = RecvQueryTouchAreaAck;
    }
    // 查询当前有效区域数与ID的返回值
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0xfc && data[3] == 0xb4 && data[4] == 0x84)
    {
        // LOGD(" <= query touch area id ack ");
        remainLen = data[5];
        *recvType = RecvQueryTouchAreaIdAck;
    }

    // 触摸数据
    if (data[0] == 0x1f && data[1] == 0xf7 && data[2] == 0x43 && data[3] == 0x00 && data[4] == 0x02)
    {
        // LOGD(" <= touch data ");
        remainLen = 61;
        *recvType = RecvTouchData;
    }

    // 快捷键
    if (data[0] == 0xf1 && data[1] == 0x91 && data[2] == 0x01 && data[4] == 0x02)
    {
        // LOGD(" <= hotkey ");
        remainLen = 0;
        *recvType = RecvHotkey;
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
    XBH_U8 checkSum = 0;               // 完整一包触摸数据的checksum
    XBH_BOOL needFindHead = XBH_FALSE; // 当获取到错误的数据时，需要重新去找数据包头
    RecvType recvType = RecvUnknow;    // 接收到的数据类型
    while (1)
    {
        // 当数据发生错乱时，需要重新找到数据包头，此时需要一个个字节的接收
        if (needFindHead)
        {
            datalen = treadn(gFd, &mTouchData.data[0], 1, 1);
            if (mTouchData.data[0] == HEAD_1)
            {
                datalen = treadn(gFd, &mTouchData.data[1], 1, 1);
                if (mTouchData.data[1] == HEAD_2)
                {
                    // 收到头部后就连续接收4个字节，并判断是否是触摸数据
                    datalen = treadn(gFd, &mTouchData.data[2], 4, 1);
                    if (mTouchData.data[2] == MAX_PACKAGE_LEN && mTouchData.data[3] == 0x00 && mTouchData.data[4] == 0x02)
                    {
                        LOGD(" find head !!!!!");
                        datalen = MAX_HEAD_LEN;
                        needFindHead = XBH_FALSE;
                    }
                }
            }
            else if (mTouchData.data[0] == 0xf1) // 快捷键
            {
                datalen = treadn(gFd, &mTouchData.data[1], 1, 1);
                if (mTouchData.data[1] == 0x91)
                {
                    datalen = treadn(gFd, &mTouchData.data[2], 4, 1);
                    if (mTouchData.data[2] == 0x01 && mTouchData.data[4] == 0x02)
                    {
                        LOGD(" find hotkey head");
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

        if (datalen < MAX_HEAD_LEN)
        {
#if 1
            LOGW("touchDataProcess_thread get datalen :%d erro \n", datalen);
#endif
            continue;
        }

        recvType = RecvUnknow;
        // 获取这包数据剩余未获取的数据，数据从数组的第六个字节开始放置
        datalen = getHeadData(mTouchData.data, &recvType);

        // 头部普配错误，重新找数据头
        if (recvType == RecvUnknow)
        {
            checkSum = 0;
            needFindHead = XBH_TRUE;
            LOGW(" head data has error, need to find head !");
            continue;
        }
        treadn(gFd, &mTouchData.data[6], datalen, 1);

        // 判断当前是否是touch数据或者快捷键数据，如果是命令数据就不需要做报点处理
        if (recvType == RecvTouchData)
        {
            checkSum = 0;
            for (int i = 0; i < (MAX_PACKAGE_LEN - 1); i++)
            {
                checkSum += mTouchData.data[i];
            }
            if (checkSum != mTouchData.data[MAX_PACKAGE_LEN - 1])
            {
                checkSum = 0;
                needFindHead = XBH_TRUE;
                LOGW(" touch data has error, need to find head !!!!!");
                continue;
            }
            // 解析数据, 触摸上报
            convertTouchData(&mTouchData);
        }
        else if (recvType == RecvHotkey)
        {
            // 解析数据, 快捷键上报
            convertHotkey(&mTouchData);
        }
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
                LOGE("Failed to create TouchProcess thread!!!!!!!");
            }
            return XBH_SUCCESS;
        }
    }
    return XBH_FAILURE;
}

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 * 协议内容:
 * Byte0	Byte1	Byte2	Byte3	Byte4	Byte5	Byte6	Byte7	        Byte8	        Byte9
 * 1F	F7	FC	30	08	02	00	USB1(off/on)	USB2(off/on)	累加和
 * 1.双USB触摸接口方案，USB1: 安卓触摸使用，USB2: PC触摸使用
 * 2.USB1:  0x01表示关闭；0x02表示打开；USB2:  0x01表示关闭；0x02表示打开
 * 3.USB+UART触摸接口，USB1为0x00，表示没有此设备
 * 协议在线文档：
 * https://doc.weixin.qq.com/doc/w3_ABUA9wb6AIgWbOLrk0GSR00JMjKLO?scode=ABUAPgfAABA79KnY92AAkA9wb6AIg&version=4.1.32.6005&platform=win
 */
XBH_S32 setSourceTouchState(XBH_BOOL enable)
{
    LOGD("setSourceTouchState enable = %d", enable);
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
    buf[8] = CalcCheckSum(buf, 9);

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
    LOGD("getSourceTouchState enable = %d", isSrcTouchEnable);
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
    LOGD("setNonThroughTouchRegion %d %d (%d, %d) (%d, %d)", id, resolution, x, y, w, h);
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
    LOGD("deleteNonThroughTouchRegion %d %d (%d, %d) (%d, %d)", id, resolution, x, y, w, h);
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
 * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
 */
XBH_S32 setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    LOGD("setTouchScaleRect %d (%d, %d) (%d, %d)", resolution, x, y, w, h);
    XBH_U8 TouchScalePackage[9] = {0x1F, 0xF7, 0xFC, 0x30, 0x32, 0x01, 0x00, 0x00, 0x00};
    if (resolution == 0)
    {
        // if the ui is 2k
        switch (y)
        {
        case 0:
            // 不降
            TouchScalePackage[6] = 0x00;
            TouchScalePackage[7] = 0x00;
            break;
        case 270:
            // 降1/4
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x04;
        case 360:
            // 降1/3
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x03;
            break;
        case 540:
            // 降1/2
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x02;
            break;
            break;
        }
    }
    else if (resolution == 1)
    {
        // if the ui is 4k
        switch (y)
        {
        case 0:
            // 不降
            TouchScalePackage[6] = 0x00;
            TouchScalePackage[7] = 0x00;
            break;
        case 540:
            // 降1/4
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x04;
        case 720:
            // 降1/3
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x03;
            break;
        case 1080:
            // 降1/2
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x02;
            break;
        }
    }
    else if (resolution == 2)
    {
        // if the ui is 5k
        switch (y)
        {
        case 0:
            // 不降
            TouchScalePackage[6] = 0x00;
            TouchScalePackage[7] = 0x00;
            break;
        case 540:
            // 降1/4
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x04;
        case 720:
            // 降1/3
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x03;
            break;
        case 1080:
            // 降1/2
            TouchScalePackage[6] = 0x01;
            TouchScalePackage[7] = 0x02;
            break;
        }
    }
    TouchScalePackage[8] = CalcCheckSum(TouchScalePackage, 8);
    return sendCommand(TouchScalePackage, 9);
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 sendKeyToTp(XBH_U32 keyCode)
{
    LOGW("sendKeyToTp unsupported");
    return XBH_FAILURE;
}

/**
 * 发送数据到触摸框
 */
XBH_S32 sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    LOGW("sendDataToTp unsupported");
    return XBH_FAILURE;
}
