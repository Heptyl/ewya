#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLiftbracketManager"
#include "XbhLog.h"
#include "XbhLiftbracketManager.h"

XbhLiftbracketManager*             XbhLiftbracketManager::mInstance = XBH_NULL;
XbhMutex                           XbhLiftbracketManager::mLock;

//---------------- custom device interface start ----------------


XBH_S32 XbhLiftbracketManager::get_usb_pvid_exist(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketTask != NULL)
    {
        s32Ret = m_pXbhLiftbracketTask->get_usb_pvid_exist();
    }
    return  s32Ret;
}

void XbhLiftbracketManager::init()
{

}

/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
XBH_BOOL XbhLiftbracketManager::openSerialPort()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketTask != NULL)
    {
        s32Ret = m_pXbhLiftbracketTask->openSerialPort();
    }
    return  s32Ret;
}

XBH_S32 XbhLiftbracketManager::readCommand(int fd, XBH_U8* pbuf, XBH_S32 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketTask != NULL)
    {
        s32Ret = m_pXbhLiftbracketTask->readCommand(fd,pbuf,len);
    }
    return  s32Ret;
}

XBH_U8 XbhLiftbracketManager::sendCmd(int fd, const XBH_U8* cmd, int len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketTask != NULL)
    {
        s32Ret = m_pXbhLiftbracketTask->sendCmd(fd,cmd,len);
    }
    return  s32Ret;
}

XBH_S32 XbhLiftbracketManager::setLiftbracketOperate(int operat)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketTask != NULL)
    {
        s32Ret = m_pXbhLiftbracketTask->setLiftbracketOperate(operat);
    }
    return  s32Ret;
}

XBH_S32 XbhLiftbracketManager::getLiftbracketExist(int *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLiftbracketTask != NULL)
    {
        s32Ret = m_pXbhLiftbracketTask->getLiftbracketExist(status);
    }
    return  s32Ret;
}

XbhLiftbracketManager::XbhLiftbracketManager()
{
    XLOGD(" begin ");
    m_pXbhLiftbracketTask = XBH_NULL;
    m_pXbhLiftbracketTask = XbhLiftbracketTask::getInstance();
    XLOGD(" end ");
}

XbhLiftbracketManager::~XbhLiftbracketManager()
{
    if(m_pXbhLiftbracketTask != XBH_NULL)
    {
        delete m_pXbhLiftbracketTask;
    }
    m_pXbhLiftbracketTask = XBH_NULL;
}


XbhLiftbracketManager* XbhLiftbracketManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhLiftbracketManager();
    }
    return mInstance;
}

