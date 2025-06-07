#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhUsbc2HdmiManager_InputSource"
#include "XbhLog.h"

#include "XbhUsbc2HdmiManager.h"
#include "XbhUsbc2HdmiTask.h"

XbhUsbc2HdmiManager*              XbhUsbc2HdmiManager::mInstance = XBH_NULL;
XbhMutex                          XbhUsbc2HdmiManager::mLock;

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhUsbc2HdmiManager::resetChip(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->resetChip(src);
    }
    return  s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 XbhUsbc2HdmiManager::setRxHpd(XBH_SOURCE_E src, XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->setRxHpd(src, value);
    }
    return  s32Ret;
}

XBH_S32 XbhUsbc2HdmiManager::setSpecificMode(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->setSpecificMode(src);
    }

    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhUsbc2HdmiManager::getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->getFirmwareVersion(src, strVersion);
    }
    return  s32Ret;
}

/**
 * 根据OPS插入状态，动态设置PD功率
 */
XBH_S32 XbhUsbc2HdmiManager::setChargingPower(XBH_SOURCE_E src, XBH_PDCAPACITY_POWER data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->setChargingPower(src, data);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhUsbc2HdmiManager::upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->upgradeFirmware(src, strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级的状态
 */
XBH_S32 XbhUsbc2HdmiManager::getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->getUpgradeState(src, s32State);
    }
    return  s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhUsbc2HdmiManager::getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->getChipExist(src, enable);
    }
    return  s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhUsbc2HdmiManager::getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->getChipInitDone(src, enable);
    }
    return  s32Ret;
}

/**
 限制type-c功率达到OPS过流保护效果
 */
XBH_S32 XbhUsbc2HdmiManager::setTypecCapacity(XBH_SOURCE_E src, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhUsbc2HdmiTask != NULL)
    {
        s32Ret = m_pXbhUsbc2HdmiTask->setTypecCapacity(src, value);
    }
    return s32Ret;
}

XbhUsbc2HdmiManager::XbhUsbc2HdmiManager()
{
    XLOGD(" begin ");
    m_pXbhUsbc2HdmiTask = XBH_NULL;
    m_pXbhUsbc2HdmiTask = XbhUsbc2HdmiTask::getInstance();
    XLOGD(" end ");
}

XbhUsbc2HdmiManager::~XbhUsbc2HdmiManager()
{
}

XbhUsbc2HdmiManager* XbhUsbc2HdmiManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhUsbc2HdmiManager();
    }
    return mInstance;
}


