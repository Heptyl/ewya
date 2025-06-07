#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPdIcManager"
#include "XbhLog.h"

#include "XbhPdIcManager.h"
#include "XbhPdIcTask.h"

XbhPdIcManager*              XbhPdIcManager::mInstance = XBH_NULL;
XbhMutex                          XbhPdIcManager::mLock;

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhPdIcManager::resetChip(XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->resetChip(devType);
    }
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhPdIcManager::getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->getFirmwareVersion(devType, strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhPdIcManager::upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->upgradeFirmware(devType, strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级的状态
 */
XBH_S32 XbhPdIcManager::getUpgradeState(XBH_S32 devType, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->getUpgradeState(devType, s32State);
    }
    return  s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhPdIcManager::getChipExist(XBH_S32 devType, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->getChipExist(devType, enable);
    }
    return  s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhPdIcManager::getChipInitDone(XBH_S32 devType, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->getChipInitDone(devType, enable);
    }
    return  s32Ret;
}
/**
 * 设置type-c功率
 */
XBH_S32 XbhPdIcManager::setTypecCapacity( XBH_U32 devType, XBH_U8 data)
{

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhPdIcTask != NULL)
    {
        s32Ret = m_pXbhPdIcTask->setTypecCapacity(devType, data);
    }
    return s32Ret;
}

XbhPdIcManager::XbhPdIcManager()
{
    XLOGD(" begin ");
    m_pXbhPdIcTask = XBH_NULL;
    m_pXbhPdIcTask = XbhPdIcTask::getInstance();
    XLOGD(" end ");
}

XbhPdIcManager::~XbhPdIcManager()
{
}

XbhPdIcManager* XbhPdIcManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhPdIcManager();
    }
    return mInstance;
}


