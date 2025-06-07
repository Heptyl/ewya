#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLightSensorManager"

#include "XbhLog.h"
#include "XbhLightSensorManager.h"

XbhLightSensorManager*             XbhLightSensorManager::mInstance = XBH_NULL;
XbhMutex                           XbhLightSensorManager::mLock;

/**
 * 复位light sensor芯片
*/
XBH_S32 XbhLightSensorManager::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->resetChip();
    }
    return  s32Ret;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 XbhLightSensorManager::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->getLightSensorValue(s32State);
    }
    return  s32Ret;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 XbhLightSensorManager::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->getChipExist(bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhLightSensorManager::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->getChipInitDone(bEnable);
    }
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhLightSensorManager::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->getFirmwareVersion(strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhLightSensorManager::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhLightSensorManager::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->getUpgradeState(s32State);
    }
    return  s32Ret;
}

/**
 * 执行光感模块初始化
 */
XBH_S32 XbhLightSensorManager::executeLightSensorInit()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorTask != NULL)
    {
        s32Ret = m_pXbhLightSensorTask->executeLightSensorInit();
    }
    return  s32Ret;
}


XbhLightSensorManager::XbhLightSensorManager()
{
    XLOGD(" begin ");
    m_pXbhLightSensorTask = XBH_NULL;
    m_pXbhLightSensorTask = XbhLightSensorTask::getInstance();
    XLOGD(" end ");
}

XbhLightSensorManager::~XbhLightSensorManager()
{
    if(m_pXbhLightSensorTask != XBH_NULL)
    {
        delete m_pXbhLightSensorTask;
    }
    m_pXbhLightSensorTask = XBH_NULL;
}

XbhLightSensorManager* XbhLightSensorManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhLightSensorManager();
    }
    return mInstance;
}

