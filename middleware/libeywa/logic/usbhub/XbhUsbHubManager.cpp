#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhUsbHubManager"

#include "XbhLog.h"
#include "XbhUsbHubManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

XbhUsbHubManager*               XbhUsbHubManager::mInstance = XBH_NULL;
XbhMutex                        XbhUsbHubManager::mLock;

/**
 * 获取固件版本
 */
XBH_S32 XbhUsbHubManager::getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbHubTask != NULL)
    {
        s32Ret = m_pXbhUsbHubTask->getFirmwareVersion(devType, strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhUsbHubManager::upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbHubTask != NULL)
    {
        s32Ret = m_pXbhUsbHubTask->upgradeFirmware(devType, strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级的状态
 */
XBH_S32 XbhUsbHubManager::getUpgradeState(XBH_S32 devType, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbHubTask != NULL)
    {
        s32Ret = m_pXbhUsbHubTask->getUpgradeState(devType, s32State);
    }
    return  s32Ret;
}

XbhUsbHubManager::XbhUsbHubManager()
{
    XLOGD(" XbhUsbHubManager begin ");
    m_pXbhUsbHubTask = XBH_NULL;
    m_pXbhUsbHubTask = XbhUsbHubTask::getInstance();
    XLOGD(" XbhUsbHubManager end ");
}

XbhUsbHubManager::~XbhUsbHubManager()
{
    if(m_pXbhUsbHubTask != XBH_NULL)
    {
        delete m_pXbhUsbHubTask;
    }
    m_pXbhUsbHubTask = XBH_NULL;
}

XbhUsbHubManager* XbhUsbHubManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhUsbHubManager();
    }
    return mInstance;
}

