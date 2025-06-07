#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAirSensorTask"

#include "XbhLog.h"
#include "XbhAirSensorTask.h"
#include "XbhService.h"

#include "ChipAm1008.h"
#include "ChipmcuAm1008.h"

XbhAirSensorTask*                 XbhAirSensorTask::mInstance = XBH_NULL;
XbhMutex                        XbhAirSensorTask::mLock;

XBH_S32 XbhAirSensorTask::getCo2Value()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getCo2Value();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorTask::getVocValue()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getVocValue();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorTask::getHumidityValue()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getHumidityValue();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorTask::getTemperatureValue()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getTemperatureValue();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorTask::getPm25Value()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getPm25Value();
    }
    return s32Ret;
}

/**
 * 获取chip是否存在
 */
XBH_S32 XbhAirSensorTask::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getChipExist(bEnable);
    }
    return s32Ret;
}

/**
 * 获取chip是否初始化完成
 */
XBH_S32 XbhAirSensorTask::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhAirSensorInterface[0]->getChipInitDone(bEnable);
    }
    return s32Ret;
}

void XbhAirSensorTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case AM1008:
                m_pXbhAirSensorInterface[i] = new ChipAm1008(mList2[i].uart, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case MCUAM1008:
                m_pXbhAirSensorInterface[i] = new ChipmcuAm1008(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhAirSensorTask::XbhAirSensorTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getAirSensorTable(mList1);
    XbhService::getModuleInterface()->getAirSensorOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhAirSensorTask::~XbhAirSensorTask()
{
}

XbhAirSensorTask* XbhAirSensorTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAirSensorTask();
    }
    return mInstance;
}

