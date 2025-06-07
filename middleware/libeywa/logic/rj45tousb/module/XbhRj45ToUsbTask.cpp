#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhRj45ToUsbTask"

#include "XbhLog.h"
#include "XbhRj45ToUsbTask.h"
#include "XbhPartitionData.h"
#include "XbhService.h"
#include "ax88179.h"
#include "ax88279.h"

XbhRj45ToUsbTask*              XbhRj45ToUsbTask::mInstance = XBH_NULL;
XbhMutex                       XbhRj45ToUsbTask::mLock;

XBH_S32 XbhRj45ToUsbTask::syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType)
{
     XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhRj45ToUsbInterface[0] != NULL)
    {
        s32Ret = m_pXbhRj45ToUsbInterface[0]->syncBuildinEthMacAddress(strPath, devNode, devType);
    }
    return s32Ret;
}


void XbhRj45ToUsbTask::run(const void* arg)
{
    for (int i = 0; i < 1; i++)
    {
        switch(mList1[i])
        {
            case AX88179:
                m_pXbhRj45ToUsbInterface[i] = ax88179::getInstance();
                break;
            case AX88279:
                m_pXbhRj45ToUsbInterface[i] = ax88279::getInstance();
                break;
            default:
                break;
        }
    }
    
}

XbhRj45ToUsbTask::XbhRj45ToUsbTask()
{
    XLOGD(" XbhRj45ToUsbTask begin ");
    XbhService::getModuleInterface()->getRj45ToUsbTable(mList1);
    XbhService::getModuleInterface()->getRj45ToUsbOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" XbhRj45ToUsbTask end ");
}

XbhRj45ToUsbTask::~XbhRj45ToUsbTask()
{

}

XbhRj45ToUsbTask* XbhRj45ToUsbTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhRj45ToUsbTask();
    }
    return mInstance;
}