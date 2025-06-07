#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAVOutManager"

#include "XbhLog.h"
#include "XbhAVOutManager.h"

XbhAVOutManager*             XbhAVOutManager::mInstance = XBH_NULL;
XbhMutex                     XbhAVOutManager::mLock;

/**
 * 复位芯片
*/
XBH_S32 XbhAVOutManager::resetChip(XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->resetChip(devType);
    }
    return  s32Ret;
}

/**
 * 恢复芯片工作状态
*/
XBH_S32 XbhAVOutManager::resumeChip(XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->resumeChip(devType);
    }
    return  s32Ret;
}

/**
 * 设置HDMI(DP)TX的音频mute状态
*/
XBH_S32 XbhAVOutManager::setHdmiTxAudioMute(XBH_S32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->setHdmiTxAudioMute(devType, bEnable);
    }
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的音频mute状态
*/
XBH_S32 XbhAVOutManager::getHdmiTxAudioMute(XBH_S32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getHdmiTxAudioMute(devType, info);
    }
    return  s32Ret;
}
/**
 * 设置HDMI(DP)TX的视频mute状态
*/
XBH_S32 XbhAVOutManager::setHdmiTxVideoMute(XBH_S32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->setHdmiTxVideoMute(devType, bEnable);
    }
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的视频mute状态
*/
XBH_S32 XbhAVOutManager::getHdmiTxVideoMute(XBH_S32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getHdmiTxVideoMute(devType, info);
    }
    return  s32Ret;
}
/**
 * 设置HDMI(DP)TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 XbhAVOutManager::setHdmiTxTiming(XBH_S32 devType, XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->setHdmiTxTiming(devType, value);
    }
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 XbhAVOutManager::getHdmiTxTiming(XBH_S32 devType, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getHdmiTxTiming(devType, info);
    }
    return  s32Ret;
}
/**
 * 设置HDMI(DP)TX的HDCP开关状态
 * 0：OFF，1：ON
*/
XBH_S32 XbhAVOutManager::setHdmiTxHdcp(XBH_S32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->setHdmiTxHdcp(devType, bEnable);
    }
    return  s32Ret;
}
/**
 * 开启/关闭 vbo tx 差分信号
*/
XBH_S32 XbhAVOutManager::setVboTxDiffSignals(XBH_S32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->setVboTxDiffSignals(devType, bEnable);
    }
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的HDCP开关状态
*/
XBH_S32 XbhAVOutManager::getHdmiTxHdcp(XBH_S32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getHdmiTxHdcp(devType, info);
    }
    return  s32Ret;
}
/**
* 设置Vx1 Tx的Tcon模式
*/
XBH_S32 XbhAVOutManager::setVx1TxTconMode(XBH_S32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->setVx1TxTconMode(devType, bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhAVOutManager::getChipExist(XBH_S32 devType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getChipExist(devType, bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhAVOutManager::getChipInitDone(XBH_S32 devType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getChipInitDone(devType, bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhAVOutManager::getFirmwareVersion(XBH_U32 devType, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getFirmwareVersion(devType, strVersion);
    }
    return  s32Ret;
}

XBH_S32 XbhAVOutManager::upgradeFirmware(XBH_U32 devType, const  XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhAVOutTask != NULL)
    {
    s32Ret = m_pXbhAVOutTask->upgradeFirmware(devType, strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

XBH_S32 XbhAVOutManager::getUpgradeState(XBH_U32 devType,     XBH_S32 *pState)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getUpgradeState(devType, pState);
    }
    return s32Ret;
}
/**
 * 获取HDMI(DP)TX的设备接入情况
*/
XBH_S32 XbhAVOutManager::getHdmiTxPlugState(XBH_S32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAVOutTask != NULL)
    {
        s32Ret = m_pXbhAVOutTask->getHdmiTxPlugState(devType, info);
    }
    return  s32Ret;
}

XbhAVOutManager::XbhAVOutManager()
{
    XLOGD(" begin ");
    m_pXbhAVOutTask = XBH_NULL;
    m_pXbhAVOutTask = XbhAVOutTask::getInstance();
    XLOGD(" end ");
}

XbhAVOutManager::~XbhAVOutManager()
{
    if(m_pXbhAVOutTask != XBH_NULL)
    {
        delete m_pXbhAVOutTask;
    }
    m_pXbhAVOutTask = XBH_NULL;
}

XbhAVOutManager* XbhAVOutManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAVOutManager();
    }
    return mInstance;
}

