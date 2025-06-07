#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhGSensorTask"

#include "XbhLog.h"
#include "XbhGSensorTask.h"
#include "XbhService.h"

#include "ChipMc3416.h"

XbhGSensorTask*                 XbhGSensorTask::mInstance = XBH_NULL;
XbhMutex                        XbhGSensorTask::mLock;


XBH_S32 XbhGSensorTask::Get_GSensor_XData()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhGSensorInterface[0]->Get_GSensor_XData();
    }
    return s32Ret;
}

XBH_S32 XbhGSensorTask::Get_GSensor_YData()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhGSensorInterface[0]->Get_GSensor_YData();
    }
    return s32Ret;
}

XBH_S32 XbhGSensorTask::Get_GSensor_ZData()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhGSensorInterface[0]->Get_GSensor_ZData();
    }
    return s32Ret;
}

XBH_S32 XbhGSensorTask::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhGSensorInterface[0]->getChipExist(bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhGSensorTask::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhGSensorInterface[0]->getChipInitDone(bEnable);
    }
    return s32Ret;
}

void XbhGSensorTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case MC3416:
                m_pXbhGSensorInterface[i] = new ChipMc3416(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhGSensorTask::XbhGSensorTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getGSensorTable(mList1);
    XbhService::getModuleInterface()->getGSensorOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhGSensorTask::~XbhGSensorTask()
{
}

XbhGSensorTask* XbhGSensorTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhGSensorTask();
    }
    return mInstance;
}

