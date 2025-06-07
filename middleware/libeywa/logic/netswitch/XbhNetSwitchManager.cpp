#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhNetSwitchManager"

#include "XbhLog.h"
#include "XbhNetSwitchManager.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "XbhService.h"


XbhNetSwitchManager*               XbhNetSwitchManager::mInstance = XBH_NULL;
XbhMutex                           XbhNetSwitchManager::mLock;


XBH_S32 XbhNetSwitchManager::setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->setPortEnable(source, enable);
    }
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->getPortEnable(source, enable);
    }
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_RJ45PortLedS
        s32Ret = XbhService::getModuleInterface()->getRJ45PlugState(port, pbStatus);
    #else
        
        if(m_pXbhNetSwitchTask != NULL)
        {
            s32Ret = m_pXbhNetSwitchTask->getRJ45PlugState(port, pbStatus);
        }
        
    #endif
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->setRJ45PortLedStatus(port, enable);
    }
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhNetSwitchManager::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhNetSwitchManager::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->getChipExist(enable);
    }
    return  s32Ret;
}

XBH_S32 XbhNetSwitchManager::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->getUpgradeState(s32State);
    }
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::setChipWol(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->setChipWol(bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhNetSwitchManager::setWolPwer(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchTask != NULL)
    {
        s32Ret = m_pXbhNetSwitchTask->setWolPwer(bEnable);
    }
    return s32Ret;
}

XbhNetSwitchManager::XbhNetSwitchManager()
{
    XLOGD(" netic begin ");
    m_pXbhNetSwitchTask = XBH_NULL;
    m_pXbhNetSwitchTask = XbhNetSwitchTask::getInstance();

#ifdef XBH_BOARD_RJ45PortLedS
        XbhMWThread::run(XbhMWThread::REPEAT);
#endif

    XLOGD(" netic end ");
}

XbhNetSwitchManager::~XbhNetSwitchManager()
{
    if(m_pXbhNetSwitchTask != XBH_NULL)
    {
        delete m_pXbhNetSwitchTask;
    }
    m_pXbhNetSwitchTask = XBH_NULL;
}

XbhNetSwitchManager* XbhNetSwitchManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhNetSwitchManager();
    }
    return mInstance;
}

void XbhNetSwitchManager::run(const void* arg)
{
    #ifdef XBH_BOARD_RJ45PortLedS
    XBH_BOOL pbStatus;
    getRJ45PlugState(1,&pbStatus);
    XbhService::getModuleInterface()->setRJ45LEDStatus(pbStatus);
    usleep(100*1000);
    #endif
}


void XbhNetSwitchManager::exit()
{
    XbhMWThread::exit();
}


