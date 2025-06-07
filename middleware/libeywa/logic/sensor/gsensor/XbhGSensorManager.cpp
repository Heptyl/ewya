#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhGSensorManager"

#include "XbhLog.h"
#include "XbhGSensorManager.h"

XbhGSensorManager*                  XbhGSensorManager::mInstance = XBH_NULL;
XbhMutex                            XbhGSensorManager::mLock;


XBH_S32 XbhGSensorManager::Get_GSensor_XData()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorTask != NULL)
    {
        s32Ret = m_pXbhGSensorTask->Get_GSensor_XData();
    }
    return s32Ret;
}

XBH_S32 XbhGSensorManager::Get_GSensor_YData()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorTask != NULL)
    {
        s32Ret = m_pXbhGSensorTask->Get_GSensor_YData();
    }
    return s32Ret;
}

XBH_S32 XbhGSensorManager::Get_GSensor_ZData()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorTask != NULL)
    {
        s32Ret = m_pXbhGSensorTask->Get_GSensor_ZData();
    }
    return s32Ret;
}

XBH_S32 XbhGSensorManager::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorTask != NULL)
    {
        s32Ret = m_pXbhGSensorTask->getChipExist(bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhGSensorManager::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhGSensorTask != NULL)
    {
        s32Ret = m_pXbhGSensorTask->getChipInitDone(bEnable);
    }
    return s32Ret;
}

XbhGSensorManager::XbhGSensorManager()
{
    XLOGD(" begin ");
    m_pXbhGSensorTask = XBH_NULL;
    m_pXbhGSensorTask = XbhGSensorTask::getInstance();
    XLOGD(" end ");
}


XbhGSensorManager::~XbhGSensorManager()
{
    if(m_pXbhGSensorTask != XBH_NULL)
    {
        delete m_pXbhGSensorTask;
    }
    m_pXbhGSensorTask = XBH_NULL;
}

XbhGSensorManager* XbhGSensorManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhGSensorManager();
    }
    return mInstance;
}

