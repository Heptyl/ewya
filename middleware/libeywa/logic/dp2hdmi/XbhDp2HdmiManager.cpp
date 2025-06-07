#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhDp2HdmiManager_InputSource"
#include "XbhLog.h"

#include "XbhDp2HdmiManager.h"
#include "XbhDp2HdmiTask.h"

XbhDp2HdmiManager*              XbhDp2HdmiManager::mInstance = XBH_NULL;
XbhMutex                        XbhDp2HdmiManager::mLock;

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhDp2HdmiManager::resetChip(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->resetChip(src);
    }
    return  s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 XbhDp2HdmiManager::setRxHpd(XBH_SOURCE_E src, XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->setRxHpd(src, value);
    }
    return  s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhDp2HdmiManager::getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->getChipInitDone(src, enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhDp2HdmiManager::getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->getChipExist(src, enable);
    }
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhDp2HdmiManager::getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->getFirmwareVersion(src, strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhDp2HdmiManager::upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->upgradeFirmware(src, strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级的状态
 */
XBH_S32 XbhDp2HdmiManager::getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhDp2HdmiTask != NULL)
    {
        s32Ret = m_pXbhDp2HdmiTask->getUpgradeState(src, s32State);
    }
    return  s32Ret;
}

XbhDp2HdmiManager::XbhDp2HdmiManager()
{
    XLOGD(" begin ");
    m_pXbhDp2HdmiTask = XBH_NULL;
    m_pXbhDp2HdmiTask = XbhDp2HdmiTask::getInstance();
    XLOGD(" end ");
}

XbhDp2HdmiManager::~XbhDp2HdmiManager()
{
}

XbhDp2HdmiManager* XbhDp2HdmiManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhDp2HdmiManager();
    }
    return mInstance;
}


