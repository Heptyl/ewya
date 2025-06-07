#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPowerSensorManager"

#include "XbhLog.h"
#include "XbhPowerSensorManager.h"

XbhPowerSensorManager*                  XbhPowerSensorManager::mInstance = XBH_NULL;
XbhMutex                            XbhPowerSensorManager::mLock;

XBH_S32 XbhPowerSensorManager::getPowerSensorValue(XBH_FLOAT* s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPowerSensorTask != NULL)
    {
        s32Ret = m_pXbhPowerSensorTask->getPowerSensorValue(s32State);
    }
    return s32Ret;
}


XBH_S32 XbhPowerSensorManager::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPowerSensorTask != NULL)
    {
        s32Ret = m_pXbhPowerSensorTask->getChipExist(bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhPowerSensorManager::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPowerSensorTask != NULL)
    {
        s32Ret = m_pXbhPowerSensorTask->getChipInitDone(bEnable);
    }
    return s32Ret;
}

XbhPowerSensorManager::XbhPowerSensorManager()
{
    XLOGD(" begin ");
    m_pXbhPowerSensorTask = XBH_NULL;
    m_pXbhPowerSensorTask = XbhPowerSensorTask::getInstance();
    XLOGD(" end ");
}


XbhPowerSensorManager::~XbhPowerSensorManager()
{
    if(m_pXbhPowerSensorTask != XBH_NULL)
    {
        delete m_pXbhPowerSensorTask;
    }
    m_pXbhPowerSensorTask = XBH_NULL;
}

XbhPowerSensorManager* XbhPowerSensorManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhPowerSensorManager();
    }
    return mInstance;
}

