#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioCodecTask"

#include "XbhLog.h"
#include "XbhAudioCodecTask.h"
#include "XbhService.h"

#include "ChipAp8224.h"
#include "ChipAp8232.h"

XbhAudioCodecTask*              XbhAudioCodecTask::mInstance = XBH_NULL;
XbhMutex                        XbhAudioCodecTask::mLock;

/**
 * 复位codec芯片
*/
XBH_S32 XbhAudioCodecTask::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->resetChip();
    }
    return s32Ret;
}

/**
 * 获取mic的插入状态
*/
XBH_S32 XbhAudioCodecTask::getMicPlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getMicPlugIn(enable);
    }
    return s32Ret;
}

/**
 * 获取耳机的插入状态
*/
XBH_S32 XbhAudioCodecTask::getHeadphonePlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getHeadphonePlugIn(enable);
    }
    return s32Ret;
}

/**
 * 设置mic是否静音
*/
XBH_S32 XbhAudioCodecTask::setMuteMicIn(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setMuteMicIn(mute);
    }
    return s32Ret;
}

/**
 * 获取mic的静音状态
*/
XBH_S32 XbhAudioCodecTask::getMuteMicIn(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getMuteMicIn(mute);
    }
    return s32Ret;
}

/**
 * 设置codec输出到speaker的那一路是否静音
*/
XBH_S32 XbhAudioCodecTask::setSpeakerOut(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setSpeakerOut(mute);
    }
    return s32Ret;
}

/**
 * 获取输出到speaker的那一路是否静音
*/
XBH_S32 XbhAudioCodecTask::getSpeakerOut(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getSpeakerOut(mute);
    }
    return s32Ret;
}

/**
 * 设置line out是否静音
*/
XBH_S32 XbhAudioCodecTask::setMuteDAC0(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setMuteDAC0(mute);
    }
    return s32Ret;
}

/**
 * 获取line out是否静音
*/
XBH_S32 XbhAudioCodecTask::getMuteDAC0(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getMuteDAC0(mute);
    }
    return s32Ret;
}

/**
 * 设置耳机输出是否静音
*/
XBH_S32 XbhAudioCodecTask::setHeadphoneMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setHeadphoneMute(mute);
    }
    return s32Ret;
}

/**
 * 获取耳机输出是否静音
*/
XBH_S32 XbhAudioCodecTask::getHeadphoneMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getHeadphoneMute(mute);
    }
    return s32Ret;
}

/**
 * 设置line out的增益
*/
XBH_S32 XbhAudioCodecTask::setLineOutGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setLineOutGain(gain);
    }
    return s32Ret;
}

/**
 * 获取line out的增益
*/
XBH_S32 XbhAudioCodecTask::getLineOutGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getLineOutGain(gain);
    }
    return s32Ret;
}

/**
 * 设置mic in的增益
*/
XBH_S32 XbhAudioCodecTask::setMicInGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setMicInGain(gain);
    }
    return s32Ret;
}

/**
 * 获取mic in的增益
*/
XBH_S32 XbhAudioCodecTask::getMicInGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getMicInGain(gain);
    }
    return s32Ret;
}

/**
 * 设置MIC的防啸叫功能开关
*/
XBH_S32 XbhAudioCodecTask::setMicHowling(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->setMicHowling(enable);
    }
    return s32Ret;
}

/**
 * 获取MIC的防啸叫功能开关
*/
XBH_S32 XbhAudioCodecTask::getMicHowling(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getMicHowling(enable);
    }
    return s32Ret;
}

/**
 * 获取codec是否初始化完成
*/
XBH_S32 XbhAudioCodecTask::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getChipInitDone(enable);
    }
    return s32Ret;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhAudioCodecTask::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getChipExist(enable);
    }
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhAudioCodecTask::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioCodecTask::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioCodecTask::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioCodecInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioCodecInterface[0]->getUpgradeState(s32State);
    }
    return s32Ret;
}

void XbhAudioCodecTask::run(const void* arg)
{
    XbhService::mCodecCreated = XBH_FALSE;
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case AP8224:
                m_pXbhAudioCodecInterface[i] = new ChipAp8224(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].uart, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel, 
                    mList2[i].mic_plug_gpio, mList2[i].mic_plug_level, mList2[i].hp_plug_gpio, mList2[i].hp_plug_level, mList2[i].hp_mute_gpio, mList2[i].hp_mute_level);
                break;
            case AP8232:
                m_pXbhAudioCodecInterface[i] = new ChipAp8232(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].uart, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel, 
                    mList2[i].mic_plug_gpio, mList2[i].mic_plug_level, mList2[i].hp_plug_gpio, mList2[i].hp_plug_level, mList2[i].hp_mute_gpio, mList2[i].hp_mute_level);
                break;
            default:
                break;
        }
    }
    XbhService::mCodecCreated = XBH_TRUE;
}

XbhAudioCodecTask::XbhAudioCodecTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getCodecTable(mList1);
    XbhService::getModuleInterface()->getCodecOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhAudioCodecTask::~XbhAudioCodecTask()
{
}

XbhAudioCodecTask* XbhAudioCodecTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioCodecTask();
    }
    return mInstance;
}

