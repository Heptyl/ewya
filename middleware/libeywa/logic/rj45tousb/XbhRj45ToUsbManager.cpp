#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhRj45ToUsbManager"

#include "XbhLog.h"
#include "XbhRj45ToUsbManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

XbhRj45ToUsbManager*               XbhRj45ToUsbManager::mInstance = XBH_NULL;
XbhMutex                           XbhRj45ToUsbManager::mLock;

XBH_S32 XbhRj45ToUsbManager::syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhRj45ToUsbTask != NULL)
    {
        s32Ret = m_pXbhRj45ToUsbTask->syncBuildinEthMacAddress(strPath, devNode, devType);
    }
    return s32Ret;
}

XbhRj45ToUsbManager::XbhRj45ToUsbManager()
{
    XLOGD(" XbhRj45ToUsbManager begin ");
    m_pXbhRj45ToUsbTask = XBH_NULL;
    m_pXbhRj45ToUsbTask = XbhRj45ToUsbTask::getInstance();
    XLOGD(" XbhRj45ToUsbManager end ");
}

XbhRj45ToUsbManager::~XbhRj45ToUsbManager()
{
    if(m_pXbhRj45ToUsbTask != XBH_NULL)
    {
        delete m_pXbhRj45ToUsbTask;
    }
    m_pXbhRj45ToUsbTask = XBH_NULL;
}

XbhRj45ToUsbManager* XbhRj45ToUsbManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhRj45ToUsbManager();
    }
    return mInstance;
}

