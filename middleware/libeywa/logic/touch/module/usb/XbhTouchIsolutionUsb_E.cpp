#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchIsolutionUsb_E"

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

#include "XbhTouchIsolutionUsb_E.h"

XbhTouchIsolutionUsb_E*          XbhTouchIsolutionUsb_E::mInstance = XBH_NULL;
XbhMutex                         XbhTouchIsolutionUsb_E::mLockObject;

static XBH_BOOL initStatus = XBH_FALSE;
static XBH_CHAR readTouchBuff[Solution_MAX_FRAME_LENGTH * Solution_MAX_FRAME_NUMBER] = {0};
static XBH_U32 totalPointNums = 0;

static XBH_BOOL SourceEnableFlag = XBH_TRUE;
static XBH_U32 SourceCheckTime = 0;
static XBH_S32 XbhTouchScaleRect_Y = 0;


typedef struct XBH_NonThroughTouchData
{
    XBH_BOOL status;
    XBH_BOOL is4k;
    XBH_S32 x1;
    XBH_S32 y1;
    XBH_S32 x2;
    XBH_S32 y2;
}XBH_NonThroughTouchData;
static XBH_NonThroughTouchData  NonThroughTouchData[10];
static XBH_U32 XBHPiontNonThroughStatus[24] = {0};

/**
 * 初始化touch
 * enable 开关状态
 */
XBH_S32 XbhTouchIsolutionUsb_E::initTouchProcess(XBH_VOID)
{
    return XBH_SUCCESS;
}

/**
 * 设置信号源的触摸开关。
 * enable 开关状态
 */
XBH_S32 XbhTouchIsolutionUsb_E::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_SUCCESS;
    XLOGD("setSourceTouchState enable:%d", enable);
    xbhTouchStat = enable ? XBH_TOUCH_RUNNING_IN_SOURCE : XBH_TOUCH_RUNNING_IN_ANDROID ;
    XLOGD("setSourceTouchState xbhTouchStat:%d", xbhTouchStat);
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchIsolutionUsb_E::getSourceTouchState(XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_SUCCESS;
    *enable = ((xbhTouchStat == XBH_TOUCH_RUNNING_IN_SOURCE) ? XBH_TRUE : XBH_FALSE);
    ret = XBH_SUCCESS;
    return ret;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
 */
XBH_S32 XbhTouchIsolutionUsb_E::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XbhMutex::Autolock lock(mLockObject);
    XBH_S32 ret = XBH_SUCCESS;
    XLOGD("setNonThroughTouchRegion id:%d", id);
    NonThroughTouchData[id].status = XBH_TRUE;
    switch (resolution)
    {
        case 0:
            NonThroughTouchData[id].is4k = XBH_FALSE;
            NonThroughTouchData[id].x1 = x * XBH_TOUCH_WIDTH / XBH_SOC_OSD_WIDTH_FULL;
            NonThroughTouchData[id].y1 = y * XBH_TOUCH_HEIGTH / XBH_SOC_OSD_HEIGTH_FULL;
            NonThroughTouchData[id].x2 = (x + w) * XBH_TOUCH_WIDTH / XBH_SOC_OSD_WIDTH_FULL;
            NonThroughTouchData[id].y2 = (y + h) * XBH_TOUCH_HEIGTH / XBH_SOC_OSD_HEIGTH_FULL;
            break;
        case 2:
            NonThroughTouchData[id].is4k = XBH_TRUE;
            NonThroughTouchData[id].x1 = x * XBH_TOUCH_WIDTH / XBH_SOC_OSD_WIDTH_5K;
            NonThroughTouchData[id].y1 = y * XBH_TOUCH_HEIGTH / XBH_SOC_OSD_HEIGTH_5K;
            NonThroughTouchData[id].x2 = (x + w) * XBH_TOUCH_WIDTH / XBH_SOC_OSD_WIDTH_5K;
            NonThroughTouchData[id].y2 = (y + h) * XBH_TOUCH_HEIGTH / XBH_SOC_OSD_HEIGTH_5K;
            break;
        default:
        case 1:
            NonThroughTouchData[id].is4k = XBH_TRUE;
            NonThroughTouchData[id].x1 = x * XBH_TOUCH_WIDTH / XBH_SOC_OSD_WIDTH_4K;
            NonThroughTouchData[id].y1 = y * XBH_TOUCH_HEIGTH / XBH_SOC_OSD_HEIGTH_4K;
            NonThroughTouchData[id].x2 = (x + w) * XBH_TOUCH_WIDTH / XBH_SOC_OSD_WIDTH_4K;
            NonThroughTouchData[id].y2 = (y + h) * XBH_TOUCH_HEIGTH / XBH_SOC_OSD_HEIGTH_4K;
            break;
    }
    XLOGD("setNonThroughTouchRegion resolution:%d (%d:%d:%d:%d)", resolution, x, y, w, h);

    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchIsolutionUsb_E::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XbhMutex::Autolock lock(mLockObject);
    XBH_S32 ret = XBH_SUCCESS;
    NonThroughTouchData[id].status = XBH_FALSE;
    return ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchIsolutionUsb_E::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
* 获取触摸框的防误触功能开关
*/
XBH_S32 XbhTouchIsolutionUsb_E::getPalmRejection(XBH_BOOL* enable)
{
    return  XBH_FAILURE;
}

/**
* 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
* x,y,w,h 当前区域的矩形参数
* resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
*/
XBH_S32 XbhTouchIsolutionUsb_E::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XbhMutex::Autolock lock(mLockObject);
     switch (resolution)
     {
         case 0:
             XbhTouchScaleRect_Y =XBH_TOUCH_HEIGTH * y / XBH_SOC_OSD_HEIGTH_FULL;
             break;
         case 2:
             XbhTouchScaleRect_Y =XBH_TOUCH_HEIGTH * y / XBH_SOC_OSD_HEIGTH_5K;
             break;
         default:
         case 1:
             XbhTouchScaleRect_Y =XBH_TOUCH_HEIGTH * y / XBH_SOC_OSD_HEIGTH_4K;
             break;
    }
    XLOGD("setTouchScaleRect y:%d XbhTouchScaleRect_Y %d", y, XbhTouchScaleRect_Y);
    return XBH_SUCCESS;
}

/**
 * 发送按键给触摸框，由触摸框转发
 */
XBH_S32 XbhTouchIsolutionUsb_E::sendKeyToTp(XBH_U32 keyCode)
{
    if ((keyCode >> 8) & 0xff) {
        //0xa1 0xaa 0x21 0x03 cmd 0x00 key
        XBH_U8 data[] = {0xa1, 0xaa, 0x21, 0x03, 0x00, 0x00, 0x00};
        data[4] = (keyCode >> 8) & 0xff;
        data[6] = keyCode & 0xff;
        writData(mFdCmd, (XBH_U8 *)data, sizeof(data));
        XLOGD("sendKeyToTp 0x%04x", keyCode);
        return XBH_SUCCESS;
    } else {
        //0xa1 0xaa 0x20 0x02 0xaa key
    }
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

XBH_S32 XbhTouchIsolutionUsb_E::openTp(void) 
{
    XBH_S32 fd = -1;
    char fileName[16] = "0";
    struct hidraw_devinfo dinfo;

    fd = open(XBH_TOUCH_DATA_NODE, O_RDWR);
    if (fd < 0)
    {
        XLOGD("[%s:%d] open %s failed !!!!!, errno = %d ", __func__, __LINE__, XBH_TOUCH_DATA_NODE, fd);
        return XBH_FAILURE;
    }
    else
    {
        mFd = fd;
        XLOGD("open Success!!!");
    }

    for (XBH_S32 i = 0; i < HID_MAX_LENGTH; i++)
    {
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_RDWR);
        //XLOGD("open %s %d %s", fileName, fd, strerror(errno));
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

    return XBH_SUCCESS;
}

/**
 *  写数据接口
 */
XBH_S32 XbhTouchIsolutionUsb_E::writData(XBH_S32 fd, XBH_U8 *data, XBH_S32 len)
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
XBH_S32 XbhTouchIsolutionUsb_E::readData(XBH_S32 fd, XBH_U8 *data, XBH_S32 len)
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


XBH_S32 XbhTouchIsolutionUsb_E::sendCommand(XBH_U8* data, XBH_S32 len) 
{
    return XBH_FAILURE;
}

XBH_S32 XbhTouchIsolutionUsb_E::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    return XBH_SUCCESS;
}

/**
 * 信源下触摸的数据是否要转发给信源处理
 */
XBH_BOOL XbhTouchIsolutionUsb_E::IsNonThroughData(XBH_S32 id, XBH_U32 status, XBH_S32 x, XBH_S32 y)
{
    XBH_U8 i = 0;
    XBH_BOOL ret = XBH_FALSE;
    mLockObject.lock();
    //XLOGD("IsNonThroughData: %d(%d, %d)%d", id, x, y,status);
    if ((status & 0x0f) == Solution_PointStateDown)
    {
        if (XBHPiontNonThroughStatus[id] == 0)
        {
            // 第一点判断是否透传
            for (i = 0; i < 10; i++)
            {
                if (NonThroughTouchData[i].status == XBH_TRUE)
                {
                    // XLOGD("p1(%d, %d); p(%d, %d); p2(%d, %d)", NonThroughTouchData[i].x1, NonThroughTouchData[i].y1, x, y, NonThroughTouchData[i].x2, NonThroughTouchData[i].y2);
                    if ((NonThroughTouchData[i].x1 <= x && x <= NonThroughTouchData[i].x2) && (NonThroughTouchData[i].y1 <= y && y <= NonThroughTouchData[i].y2))
                    {
                        // XLOGD("Down NonThrough 1");
                        break;
                    }
                }
            }
            XBHPiontNonThroughStatus[id] = (i == 10 ? 2 : 1);
            ret = i == 10 ? XBH_FALSE : XBH_TRUE;
            XLOGD("Down Through 2");
        }
        else
        {
            ret = XBHPiontNonThroughStatus[id] == 2 ? XBH_FALSE : XBH_TRUE;
            XLOGD("Down ThroughStatus %d", XBHPiontNonThroughStatus[id]);
        }
    }
    else if ((status & 0x0f) == Solution_PointStateUp)
    {
        XBHPiontNonThroughStatus[id] = 0;
        XLOGD("Down Through Point Up");
    }
    else
    {
        ret = XBHPiontNonThroughStatus[id] == 2 ? XBH_FALSE : XBH_TRUE;
        XBHPiontNonThroughStatus[id] = 0;
    }
    mLockObject.unlock();
    return ret;
}


void XbhTouchIsolutionUsb_E::run(const void* arg)
{
    XBH_U32 u32Ret = 0;
    XBH_S32 s32ret = 0;
    XBH_U32 x = 0, y = 0;
    XBH_U32 i = 0, j = 0;
    XBH_U32 frames = 0;
    XBH_U32 points = 0;
    XBH_CHAR * buffOfframe = XBH_NULL;
    XBH_CHAR Sendbuff[Solution_MAX_FRAME_NUMBER][Solution_MAX_FRAME_LENGTH] = {0};
    XBH_U32 send_frame = 0;
    XBH_U32 send_point = 0;
    XBH_U32 send_points = 0;
    switch (xbhTouchStat)
    {
        case XBH_TOUCH_OPEN_DATA_NODE:
            usleep(2000 * 1000);
            if (XBH_FAILURE == openTp())
            {
                XLOGE("[%s:%d] openTp %s error!!!", __func__, __LINE__, XBH_TOUCH_DATA_NODE);
                xbhTouchStat = XBH_TOUCH_OPEN_DATA_NODE;
                break;
            }
            xbhTouchStat = XBH_TOUCH_RUNNING_IN_SOURCE;
        break;
        case XBH_TOUCH_RUNNING_IN_ANDROID:
            if (SourceEnableFlag != XBH_FALSE)
            {
                SourceEnableFlag = XBH_FALSE;
                // writData(g_touchDataWriteFd, CloseSourceTouch, 6);
            }
            usleep(1500 * 1000);
        break;
        case XBH_TOUCH_RUNNING_IN_SOURCE:
            // usleep(100 * 1000);
            if (mFd < 0)
            {
                XLOGI("[%s:%d] g_touchDataReadFd Failed %d!!!", __func__, __LINE__, mFd);
                xbhTouchStat = XBH_TOUCH_OPEN_DATA_NODE;
                break;
            }
            if (!SourceEnableFlag)
            {
                // XLOGE("[%s:%d] OpenSourceTouch...", __func__, __LINE__);
                // writData(g_touchDataWriteFd, OpenSourceTouch, 6);
                SourceEnableFlag = XBH_TRUE;
                for (i = 0; i < 20; i++)
                {
                    XBHPiontNonThroughStatus[i] = 0;
                }
            }
            do
            {
                if (xbhTouchStat != XBH_TOUCH_RUNNING_IN_SOURCE)
                {
                    // switch out from source mode，return!
                    return ;
                }
                (readTouchBuff + frames * Solution_MAX_FRAME_LENGTH)[PT_NU] = 0;
                readData(mFd, (XBH_U8 *)(readTouchBuff + frames * Solution_MAX_FRAME_LENGTH), Solution_MAX_FRAME_LENGTH);
                if ((frames == 0) && (readTouchBuff[PT_NU] == 0))
                {
                    // nothing was read, continue reading;
                    continue;
                }
                frames++;
            }while (frames < ((readTouchBuff[PT_NU] + 5) / 6));

            send_frame = 0;
            send_point = 0;
            send_points = 0;
            points = readTouchBuff[PT_NU];
            for (i = 0; i < frames; i++)
            {
                buffOfframe = readTouchBuff + i * Solution_MAX_FRAME_LENGTH;
                if (buffOfframe[PG_ID] == Solution_PackageID)
                {
                    for (j = 0; j < MIN_OF_TOW_VALUE(points, 6); j++)
                    {
                        x = buffOfframe[PT_XL + j * Solution_POINTDDATALENGTH] & 0x00ff;
                        x = x | ((buffOfframe[PT_XH + j * Solution_POINTDDATALENGTH] << 8));
                        y = buffOfframe[PT_YL + j * Solution_POINTDDATALENGTH] & 0x00ff;
                        y = y | ((buffOfframe[PT_YH + j * Solution_POINTDDATALENGTH] << 8));
                        if (!IsNonThroughData(buffOfframe[PT_ID + j * Solution_POINTDDATALENGTH], buffOfframe[PT_ST + j * Solution_POINTDDATALENGTH], x, y))
                        {
                            // XLOGD("Is Through Data");
                            Sendbuff[send_frame][PT_ST + send_point * Solution_POINTDDATALENGTH] = buffOfframe[PT_ST + j * Solution_POINTDDATALENGTH];
                            Sendbuff[send_frame][PT_ID + send_point * Solution_POINTDDATALENGTH] = buffOfframe[PT_ID + j * Solution_POINTDDATALENGTH];
                            // 边界值矫正
                            if (x > XBH_TOUCH_WIDTH)
                            {
                                x = XBH_TOUCH_WIDTH;
                            }
                            if (x < 0)
                            {
                                x = 0;
                            }
                            Sendbuff[send_frame][PT_XL + send_point * Solution_POINTDDATALENGTH] = x & 0x00FF;
                            Sendbuff[send_frame][PT_XH + send_point * Solution_POINTDDATALENGTH] = x >> 8;
                            y = y - XbhTouchScaleRect_Y;
                            if (y > XBH_TOUCH_HEIGTH)
                            {
                                y = XBH_TOUCH_HEIGTH;
                            }
                            if (y < 0)
                            {
                                y = 0;
                            }
                            Sendbuff[send_frame][PT_YL + send_point * Solution_POINTDDATALENGTH] = y &  0x00FF;
                            Sendbuff[send_frame][PT_YH + send_point * Solution_POINTDDATALENGTH] = y >> 8;
                            Sendbuff[send_frame][PT_WL + send_point * Solution_POINTDDATALENGTH] = buffOfframe[PT_WL + j * Solution_POINTDDATALENGTH];
                            Sendbuff[send_frame][PT_WH + send_point * Solution_POINTDDATALENGTH] = buffOfframe[PT_WH + j * Solution_POINTDDATALENGTH];
                            Sendbuff[send_frame][PT_HL + send_point * Solution_POINTDDATALENGTH] = buffOfframe[PT_HL + j * Solution_POINTDDATALENGTH];
                            Sendbuff[send_frame][PT_HH + send_point * Solution_POINTDDATALENGTH] = buffOfframe[PT_HH + j * Solution_POINTDDATALENGTH];
                            send_point++;
                            send_points++;
                            if (send_point >= Solution_POINTNUMMAX)
                            {
                                send_point = 0;
                                send_frame++;
                            }
                        }
                    }
                    points = points - j;
                }
                else
                {
                    XLOGE("package error on %d frame: ID:0x%02X, Point number: %d", buffOfframe[PG_ID], buffOfframe[PT_NU], i);
                }
            }

            if (send_points > 0)
            {
                i = 0;
                points = send_points;
                do
                {
                    buffOfframe = Sendbuff[i];
                    buffOfframe[PG_ID] = Solution_PackageID;
                    buffOfframe[PT_NU] = (i == 0) ? send_points : 0x00;
                    j = MIN_OF_TOW_VALUE(points, 6);
                    points = points - j;
                    writData(mFd, (XBH_U8 *)(buffOfframe), Solution_MAX_FRAME_LENGTH);
                    i++;
                } while (i < ((send_points + 5) / 6));
            }
        break;
        default:
            xbhTouchStat = XBH_TOUCH_OPEN_DATA_NODE;
        break;
    }
}


XBH_S32 XbhTouchIsolutionUsb_E::getTpFirmwareVersion(XBH_CHAR *version){
    XBH_S32 ret = XBH_FAILURE;
    if (version != NULL) {
        FILE* file = fopen("/sys/bus/usb/drivers/touchE/drvinfo", "r");
        char line[128];  // 用于读取每行的缓冲区
        const char* prefix = "DEVICE_VERSION:";  // 要查找的前缀
        // 初始化 version 为一个空字符串
        version[0] = '\0';

        if (file != nullptr)
        {
            XLOGD("getTpFirmwareVersion open");
            // 逐行读取文件内容
            while (fgets(line, sizeof(line), file))
            {
                // 查找包含 "DEVICE_VERSION" 的行
                if (strstr(line, prefix))
                {
                    // 查找 "DEVICE_VERSION:" 后面的版本号
                    char* pos = strstr(line, prefix);
                    if (pos != nullptr)
                    {
                        // 跳过 "DEVICE_VERSION:" 和空格（如果有）
                        pos += strlen(prefix);  // 跳过 "DEVICE_VERSION:" 字符串

                        // 跳过可能的空格
                        while (*pos == ' ' || *pos == '\t')
                        {
                            pos++;
                        }

                        // 将版本号从 "V" 开始的部分复制到 version
                        strncpy(version, pos, 127);  // 限制最大字符数
                        // sprintf (version, "v.%02x%02x%02x", 1,1,1);
                        version[127] = '\0';  // 确保字符串结束

                        // 去除可能存在的换行符
                        size_t len = strlen(version);
                        if (len > 0 && version[len - 1] == '\n')
                        {
                            version[len - 1] = '\0';  // 删除换行符
                        }

                        ret = XBH_SUCCESS;
                    }
                    break;  // 找到版本号后退出循环
                }
            }
            fclose(file);  // 关闭文件
        } else
        {
            // 如果文件打开失败，输出错误信息
            XLOGE("getTpFirmwareVersion fail");
        }
    }
    return ret;
}

XbhTouchIsolutionUsb_E::XbhTouchIsolutionUsb_E(XBH_CHAR *ko, XBH_CHAR *tpName)
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

    XbhMWThread::run(XbhMWThread::REPEAT);
}

XbhTouchIsolutionUsb_E::~XbhTouchIsolutionUsb_E()
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

XbhTouchInterface *XbhTouchIsolutionUsb_E::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL)
    {
        mInstance = new XbhTouchIsolutionUsb_E(ko, tpName);
    }
    return mInstance;
}

