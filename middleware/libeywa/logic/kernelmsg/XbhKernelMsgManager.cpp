#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhKernelMsgManager"

#include "XbhLog.h"
#include "XbhKernelMsgManager.h"
#include "XbhSourceManager.h"
#include "XbhService.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


XbhKernelMsgManager*       XbhKernelMsgManager::mInstance = NULL;
XbhMutex                   XbhKernelMsgManager::mLock;

//来自kernel的消息数据
struct MsgData
{
    XBH_CHAR data0;
    XBH_CHAR data1;
    XBH_CHAR data2;
    XBH_CHAR data3;
    XBH_CHAR data4;
};

static XBH_S32 mFd;
static XBH_CHAR mValueTemp[5];

/*
获取消息时，必须要指定获取消息的类型，消息类型如下：
msgType;    data[0] = 0 input event   当前只被xbhshell接收
            data[0] = 1 usb eumn error
            data[0] = 2 gpio irq  data[1]:gpio num  data[2] gpio value
            data[0] = 3 fatt keypad  data[1]:keypad enable  data[2]:key_count  data[3]:key_index
            
input event         当前只在xbhshell中获取，用来处理新增按键事件的转发
usb eumn error      处理kernel中枚举USB失败时的事件
gpio irq            GPIO中断发生
*/
void XbhKernelMsgManager::run(const void* arg)
{
    XBH_U32 msgType;
    XBH_U32 size;
    MsgData data;
    XBH_SOURCE_E src;
    XBH_BOOL bEnable;
    if(getMsg(&msgType, &data, &size, this))
    {
        XLOGD("run data0 = %d data1 = %d data2 = %d data3 = %d data4 = %d\n", data.data0, data.data1, data.data2, data.data3, data.data4);
        switch(data.data0)
        {
            case 1:
            break;
            case 2:
            {
                src = XBH_SOURCE_E::XBH_SOURCE_BUTT;
                XbhService::getModuleInterface()->getSrcDetByDetGpio(data.data1, data.data2, &src, &bEnable);
                if(src != XBH_SOURCE_E::XBH_SOURCE_BUTT)
                {
                    ;//XbhSourceManager::getInstance()->pushSourcePlugStatus(src, bEnable);
                }
                break;
            }
            case 3:
            {
                if(data.data1 == XBH_KERNEL_SEND_TO_EYWA_SCREEN_OFF) // kernel send screen OF
                {
                    XLOGD("Mn869129 this is 100 getPlatformInterface()->setOnResume(XBH_KERNEL_SCREEN_OFF)\n");
                    XbhService::getPlatformInterface()->setOnResume(XBH_KERNEL_SCREEN_OFF); // reset vbo to hdmi chip
                    break;
                }
                else if(data.data1 == XBH_KERNEL_SEND_TO_EYWA_SCREEN_ON) // kernel send screen ON
                {
                    XLOGD("Mn869129 this is 101 getPlatformInterface()->setOnResume(XBH_KERNEL_SCREEN_ON)\n");
                    XbhService::getPlatformInterface()->setOnResume(XBH_KERNEL_SCREEN_ON); // resume vbo to hdmi chip
                    break;
                }
                XbhService::getModuleInterface()->saveFattKeypadInfo(data.data1, data.data2, data.data3);
                break;
            }
            default:
                break;
        }
    }
}

/*
向kernel发送消息时，必须要指定获取消息的类型，消息类型如下：
msgType;    data0 = 0 外挂芯片进行升级 data1 = 1  开始升级 data1 = 0 停止升级
            data0 = 1 fatt mode, 禁用按键板和遥控正常发送事件，fatt模式下按键和遥控发送特殊的事件
            data0 = 2  内核操作GPIO口, data1 = gpio num, data2 = gpio value
            data0 = 3
*/
XBH_S32 XbhKernelMsgManager::sendMsgToKernel(XBH_U8 data0, XBH_U8 data1, XBH_U8 data2, XBH_U8 data3, XBH_U8 data4)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    MsgData data = {data0, data1, data2, data3, data4};
    ssize_t len = write(mFd, &data, sizeof(MsgData));
    if(len < 0)
    {
        XLOGE("write erro !!!! ");
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_VOID XbhKernelMsgManager::xbh_msg_handler(int num)
{
    //设置msgType = 1;表示只接受msgType >= 1的消息
    mValueTemp[0] = 1;
    XBH_S32 ret = read(mFd, &mValueTemp, 5);
    //XLOGD("xbh_msg_handler read count = %d ",ret);
    if(ret <= 0)
    {
        return ;
    }
    //XLOGD("xbh_msg_handler value[0] = %d value[1] = %d value[2] = %d value[3] = %d value[4] = %d\n", mValueTemp[0], mValueTemp[1], mValueTemp[2], mValueTemp[3], mValueTemp[4]);
    MsgData data {mValueTemp[0], mValueTemp[1], mValueTemp[2], mValueTemp[3], mValueTemp[4]};
    XbhKernelMsgManager::getInstance()->postMsg(0, &data, sizeof(data), 0, XbhKernelMsgManager::getInstance());
}

XbhKernelMsgManager::XbhKernelMsgManager()
{
    XLOGD(" begin ");
    mFd = open("/dev/xbh_input_fasync", O_RDWR);
    if(mFd >= 0)
    {
        XLOGD(" mFd = %d ",mFd);
        signal(SIGIO, xbh_msg_handler);
        fcntl(mFd, F_SETOWN, getpid());
        fcntl(mFd, F_SETFL, fcntl(mFd, F_GETFL) | FASYNC);
    }
    else
    {
        XLOGE("Error %d: Failed to open file\n", errno);
    }
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhKernelMsgManager::~XbhKernelMsgManager()
{
}

XbhKernelMsgManager* XbhKernelMsgManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhKernelMsgManager();
    }
    return mInstance;
}

