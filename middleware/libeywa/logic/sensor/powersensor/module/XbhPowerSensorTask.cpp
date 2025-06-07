#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPowerSensorTask"

#include "XbhLog.h"
#include "XbhPowerSensorTask.h"
#include "XbhService.h"

#include "ChipBl0942.h"

XbhPowerSensorTask*                 XbhPowerSensorTask::mInstance = XBH_NULL;
XbhMutex                        XbhPowerSensorTask::mLock;

XBH_S32 XbhPowerSensorTask::getPowerSensorValue(XBH_FLOAT * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPowerSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhPowerSensorInterface[0]->getPowerSensorValue(s32State);
    }
    return s32Ret;
}



/**
 * 获取chip是否存在
 */
XBH_S32 XbhPowerSensorTask::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPowerSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhPowerSensorInterface[0]->getChipExist(bEnable);
    }
    return s32Ret;
}

/**
 * 获取chip是否初始化完成
 */
XBH_S32 XbhPowerSensorTask::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPowerSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhPowerSensorInterface[0]->getChipInitDone(bEnable);
    }
    return s32Ret;
}

void XbhPowerSensorTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case Bl0942:
                XLOGD("kkk: power sensor begin\n");
                m_pXbhPowerSensorInterface[i] = new ChipBl0942(mList2[i].uart, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhPowerSensorTask::XbhPowerSensorTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getPowerSensorTable(mList1);
    XbhService::getModuleInterface()->getPowerSensorOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhPowerSensorTask::~XbhPowerSensorTask()
{
}

XbhPowerSensorTask* XbhPowerSensorTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhPowerSensorTask();
    }
    return mInstance;
}

