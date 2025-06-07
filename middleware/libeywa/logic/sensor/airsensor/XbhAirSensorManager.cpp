#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAirSensorManager"

#include "XbhLog.h"
#include "XbhAirSensorManager.h"

XbhAirSensorManager*                  XbhAirSensorManager::mInstance = XBH_NULL;
XbhMutex                            XbhAirSensorManager::mLock;

XBH_S32 XbhAirSensorManager::getCo2Value()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getCo2Value();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorManager::getVocValue()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getVocValue();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorManager::getHumidityValue()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getHumidityValue();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorManager::getTemperatureValue()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getTemperatureValue();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorManager::getPm25Value()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getPm25Value();
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorManager::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getChipExist(bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhAirSensorManager::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAirSensorTask != NULL)
    {
        s32Ret = m_pXbhAirSensorTask->getChipInitDone(bEnable);
    }
    return s32Ret;
}

XbhAirSensorManager::XbhAirSensorManager()
{
    XLOGD(" begin ");
    m_pXbhAirSensorTask = XBH_NULL;
    m_pXbhAirSensorTask = XbhAirSensorTask::getInstance();
    XLOGD(" end ");
}


XbhAirSensorManager::~XbhAirSensorManager()
{
    if(m_pXbhAirSensorTask != XBH_NULL)
    {
        delete m_pXbhAirSensorTask;
    }
    m_pXbhAirSensorTask = XBH_NULL;
}

XbhAirSensorManager* XbhAirSensorManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAirSensorManager();
    }
    return mInstance;
}

