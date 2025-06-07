#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioToI2cManager"

#include "XbhLog.h"
#include "XbhAudioToI2sManager.h"

XbhAudioToI2sManager*              XbhAudioToI2sManager::mInstance = XBH_NULL;
XbhMutex                           XbhAudioToI2sManager::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 XbhAudioToI2sManager::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sTask != NULL)
    {
        s32Ret = m_pXbhAudioToI2sTask->resetChip();
    }
    return  s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhAudioToI2sManager::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sTask != NULL)
    {
        s32Ret = m_pXbhAudioToI2sTask->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhAudioToI2sManager::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sTask != NULL)
    {
        s32Ret = m_pXbhAudioToI2sTask->getChipExist(enable);
    }
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhAudioToI2sManager::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sTask != NULL)
    {
        s32Ret = m_pXbhAudioToI2sTask->getFirmwareVersion(strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioToI2sManager::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sTask != NULL)
    {
        s32Ret = m_pXbhAudioToI2sTask->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioToI2sManager::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sTask != NULL)
    {
        s32Ret = m_pXbhAudioToI2sTask->getUpgradeState(s32State);
    }
    return  s32Ret;
}

XbhAudioToI2sManager::XbhAudioToI2sManager()
{
    XLOGD(" begin ");
    m_pXbhAudioToI2sTask = XBH_NULL;
    m_pXbhAudioToI2sTask = XbhAudioToI2sTask::getInstance();
    XLOGD(" end ");
}

XbhAudioToI2sManager::~XbhAudioToI2sManager()
{
    if(m_pXbhAudioToI2sTask != XBH_NULL)
    {
        delete m_pXbhAudioToI2sTask;
    }
    m_pXbhAudioToI2sTask = XBH_NULL;
}

XbhAudioToI2sManager* XbhAudioToI2sManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioToI2sManager();
    }
    return mInstance;
}

