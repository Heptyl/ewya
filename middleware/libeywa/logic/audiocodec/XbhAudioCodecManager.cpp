#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioCodecManager"

#include "XbhLog.h"
#include "XbhAudioCodecManager.h"

XbhAudioCodecManager*              XbhAudioCodecManager::mInstance = XBH_NULL;
XbhMutex                           XbhAudioCodecManager::mLock;

/**
 * 复位codec芯片
*/
XBH_S32 XbhAudioCodecManager::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->resetChip();
    }
    return  s32Ret;
}

/**
 * 获取mic的插入状态
*/
XBH_S32 XbhAudioCodecManager::getMicPlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getMicPlugIn(enable);
    }
    return  s32Ret;
}

/**
 * 获取耳机的插入状态
*/
XBH_S32 XbhAudioCodecManager::getHeadphonePlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getHeadphonePlugIn(enable);
    }
    return  s32Ret;
}

/**
 * 设置mic是否静音
*/
XBH_S32 XbhAudioCodecManager::setMuteMicIn(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setMuteMicIn(mute);
    }
    return  s32Ret;
}

/**
 * 获取mic的静音状态
*/
XBH_S32 XbhAudioCodecManager::getMuteMicIn(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getMuteMicIn(mute);
    }
    return  s32Ret;
}

/**
 * 设置codec输出到speaker的那一路是否静音
*/
XBH_S32 XbhAudioCodecManager::setSpeakerOut(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setSpeakerOut(mute);
    }
    return  s32Ret;
}

/**
 * 获取输出到speaker的那一路是否静音
*/
XBH_S32 XbhAudioCodecManager::getSpeakerOut(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getSpeakerOut(mute);
    }
    return  s32Ret;
}

/**
 * 设置line out是否静音
*/
XBH_S32 XbhAudioCodecManager::setMuteDAC0(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setMuteDAC0(mute);
    }
    return  s32Ret;
}

/**
 * 获取line out是否静音
*/
XBH_S32 XbhAudioCodecManager::getMuteDAC0(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getMuteDAC0(mute);
    }
    return  s32Ret;
}

/**
 * 设置耳机输出是否静音
*/
XBH_S32 XbhAudioCodecManager::setHeadphoneMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setHeadphoneMute(mute);
    }
    return  s32Ret;
}

/**
 * 获取耳机输出是否静音
*/
XBH_S32 XbhAudioCodecManager::getHeadphoneMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getHeadphoneMute(mute);
    }
    return  s32Ret;
}

/**
 * 设置line out的增益
*/
XBH_S32 XbhAudioCodecManager::setLineOutGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setLineOutGain(gain);
    }
    return s32Ret;
}

/**
 * 获取line out的增益
*/
XBH_S32 XbhAudioCodecManager::getLineOutGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getLineOutGain(gain);
    }
    return s32Ret;
}

/**
 * 设置mic in的增益
*/
XBH_S32 XbhAudioCodecManager::setMicInGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setMicInGain(gain);
    }
    return s32Ret;
}

/**
 * 获取mic in的增益
*/
XBH_S32 XbhAudioCodecManager::getMicInGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getMicInGain(gain);
    }
    return s32Ret;
}

/**
 * 设置MIC的防啸叫功能开关
*/
XBH_S32 XbhAudioCodecManager::setMicHowling(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->setMicHowling(enable);
    }
    return s32Ret;
}

/**
 * 获取MIC的防啸叫功能开关
*/
XBH_S32 XbhAudioCodecManager::getMicHowling(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getMicHowling(enable);
    }
    return s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhAudioCodecManager::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getChipInitDone(enable);
    }
    return s32Ret;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhAudioCodecManager::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getChipExist(enable);
    }
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhAudioCodecManager::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getFirmwareVersion(strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioCodecManager::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioCodecManager::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecTask != NULL)
    {
        s32Ret = m_pXbhAudioCodecTask->getUpgradeState(s32State);
    }
    return  s32Ret;
}

XbhAudioCodecManager::XbhAudioCodecManager()
{
    XLOGD(" begin ");
    m_pXbhAudioCodecTask = XBH_NULL;
    m_pXbhAudioCodecTask = XbhAudioCodecTask::getInstance();
    XLOGD(" end ");
}

XbhAudioCodecManager::~XbhAudioCodecManager()
{
    if(m_pXbhAudioCodecTask != XBH_NULL)
    {
        delete m_pXbhAudioCodecTask;
    }
    m_pXbhAudioCodecTask = XBH_NULL;
}

XbhAudioCodecManager* XbhAudioCodecManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioCodecManager();
    }
    return mInstance;
}

