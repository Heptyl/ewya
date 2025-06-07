#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLiftbracketTask"
#include "XbhLog.h"

#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cutils/properties.h"
#include "errno.h"
#include <iostream>
#include <dirent.h>
#include "XbhLiftbracketTask.h"
#include "XbhLiftbracket.h"

XbhLiftbracketTask*             XbhLiftbracketTask::mInstance = NULL;
XbhMutex                        XbhLiftbracketTask::mLock;

//---------------- custom device interface start ----------------

XBH_S32 XbhLiftbracketTask::get_usb_pvid_exist(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketInterface[0] != NULL)
    {
        s32Ret = m_pXbhLiftbracketInterface[0]->get_usb_pvid_exist();
    }
    return s32Ret;
}

void XbhLiftbracketTask::init()
{
    
}

/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
XBH_BOOL XbhLiftbracketTask::openSerialPort()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketInterface[0] != NULL)
    {
        s32Ret = m_pXbhLiftbracketInterface[0]->openSerialPort();
    }
    return s32Ret;
}

XBH_S32 XbhLiftbracketTask::readCommand(int fd, XBH_U8* pbuf, XBH_S32 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketInterface[0] != NULL)
    {
        s32Ret = m_pXbhLiftbracketInterface[0]->readCommand(fd,pbuf,len);
    }
    return s32Ret;
}

XBH_U8 XbhLiftbracketTask::sendCmd(int fd, const XBH_U8* cmd, int len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketInterface[0] != NULL)
    {
        s32Ret = m_pXbhLiftbracketInterface[0]->sendCmd(fd,cmd,len);
    }
    return s32Ret;
}

XBH_S32 XbhLiftbracketTask::setLiftbracketOperate(int operat)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // if(m_pXbhLiftbracketInterface[0] != NULL)
    // {
        s32Ret = XbhLiftbracket::getInstance()->setLiftbracketOperate(operat);
    // }
    return s32Ret;
}

XBH_S32 XbhLiftbracketTask::getLiftbracketExist(int *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // if(m_pXbhLiftbracketInterface[0] != NULL)
    // {
        s32Ret = XbhLiftbracket::getInstance()->getLiftbracketExist(status);
    // }
    return s32Ret;
}

void XbhLiftbracketTask::run(const void* arg)
{
    m_pXbhLiftbracketInterface[0] = static_cast<XbhLiftbracketInterface*>(XbhLiftbracket::getInstance());
}

XbhLiftbracketTask::XbhLiftbracketTask()
{
    XLOGD(" begin ");
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhLiftbracketTask::~XbhLiftbracketTask()
{

}


XbhLiftbracketTask* XbhLiftbracketTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhLiftbracketTask();
    }
    return mInstance;
}

