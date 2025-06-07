#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioAmplifierTask"

#include "XbhLog.h"
#include "XbhAudioAmplifierTask.h"
#include "XbhService.h"

#include "ChipAd82120_Stereo.h"
#include "ChipAd82120_Subwoofer.h"
#include "ChipAcm8625_Stereo.h"
#include "ChipAcm8625_Subwoofer.h"
#include "ChipTas5805m_Stereo.h"
#include "ChipTas5805m_Subwoofer.h"

XbhAudioAmplifierTask*           XbhAudioAmplifierTask::mInstance = XBH_NULL;
XbhMutex                         XbhAudioAmplifierTask::mLock;

/**
 * 复位功放芯片
*/
XBH_S32 XbhAudioAmplifierTask::resetChip(XBH_AUDIO_CHANNEL_E channel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->resetChip();
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->resetChip();
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->resetChip();
        }
    }
    return  s32Ret;
}

/**
 * 设置speaker mute的状态
*/
XBH_S32 XbhAudioAmplifierTask::setMute(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_SPEAKER)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->setMute(mute);
        }
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->setMute(mute);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->setMute(mute);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->setMute(mute);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->setMute(mute);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->setMute(mute);
        }
    }
    return  s32Ret;
}

/**
 * 获取speaker 的mute状态
*/
XBH_S32 XbhAudioAmplifierTask::getMute(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->getMute(mute);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->getMute(mute);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->getMute(mute);
        }
    }
    return  s32Ret;
}

/**
 * 设置功放输出的增益值
*/
XBH_S32 XbhAudioAmplifierTask::setGain(XBH_AUDIO_CHANNEL_E channel, XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->setGain(gain);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->setGain(gain);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->setGain(gain);
        }
    }
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 XbhAudioAmplifierTask::getGain(XBH_AUDIO_CHANNEL_E channel, XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->getGain(gain);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->getGain(gain);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->getGain(gain);
        }
    }
    return  s32Ret;
}

/**
 * 设置输出的音频频率直通，不对频率做抑制，主要是针对subwoofer
 * 工厂测试的音频是1K HZ，subwoofer抑制了高频信号，导致无法测试到声音输出
*/
XBH_S32 XbhAudioAmplifierTask::setDirectPass(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_SPEAKER)
    {   
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->setDirectPass(enable);
        }
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->setDirectPass(enable);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->setDirectPass(enable);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->setDirectPass(enable);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->setDirectPass(enable);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->setDirectPass(enable);
        }
    }
    return  s32Ret;
}
/**
 * 获取输出的音频频率直通的状态
*/
XBH_S32 XbhAudioAmplifierTask::getDirectPass(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->getDirectPass(enable);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->getDirectPass(enable);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->getDirectPass(enable);
        }
    }
    return  s32Ret;
}
/**
 * 获取amp是否初始化完成
*/
XBH_S32 XbhAudioAmplifierTask::getChipInitDone(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_SPEAKER)
    {
        XBH_BOOL param0 = XBH_TRUE;
        XBH_BOOL param1 = XBH_TRUE;
        XBH_BOOL param2 = XBH_TRUE;
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->getChipInitDone(&param0);
        }
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->getChipInitDone(&param1);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->getChipInitDone(&param2);
        }
        *enable = param0|param1|param2;
    }
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->getChipInitDone(enable);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->getChipInitDone(enable);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->getChipInitDone(enable);
        }
    }
    return  s32Ret;
}
/**
 * 获取amp是否初始化完成
*/
XBH_S32 XbhAudioAmplifierTask::getChipExist(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(channel == XBH_AUDIO_CHANNEL_STEREO)
    {
        if(m_pXbhAudioAmplifierInterface[0] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[0]->getChipExist(enable);
        }
    }
    if(channel == XBH_AUDIO_CHANNEL_SUBWOOFER)
    {
        if(m_pXbhAudioAmplifierInterface[1] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[1]->getChipExist(enable);
        }
        if(m_pXbhAudioAmplifierInterface[2] != NULL)
        {
            s32Ret = m_pXbhAudioAmplifierInterface[2]->getChipExist(enable);
        }
    }
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhAudioAmplifierTask::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioAmplifierTask::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioAmplifierTask::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getUpgradeState(s32State);
    }
    return s32Ret;
}
/**
 * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setAvcEnable(XBH_BOOL bEnable)
{   

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setAvcEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[1] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[1]->setAvcEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[2] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[2]->setAvcEnable(bEnable);
    }
    return  s32Ret; 
}

/**
 * 获取AVC(自动电压控制)开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getAvcEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getAvcEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[1] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[1]->getAvcEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[2] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[2]->getAvcEnable(bEnable);
    }
    return  s32Ret;
}

/**
* 设置EQ曲线(频率分布曲线)开关，可以用来打开或者关闭该曲线用于测试
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setEqEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setEqEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[1] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[1]->setEqEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[2] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[2]->setEqEnable(bEnable);
    }
    return  s32Ret; 
}

/**
* 获取EQ曲线(频率分布曲线)开关
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getEqEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getEqEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[1] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[1]->getEqEnable(bEnable);
    }
    if(m_pXbhAudioAmplifierInterface[2] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[2]->getEqEnable(bEnable);
    }
    return  s32Ret;
}

/**
* 设置低音增益
* param[in] s32Value 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setBass(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setBass(s32Value);
    }
    return  s32Ret;
}

/**
 * 获取低音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getBass(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getBass(s32Value);
    }
    return  s32Ret;
}

/**
 * 设置高音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setTreble(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setTreble(s32Value);
    }
    return  s32Ret;
}

/**
 * 获取高音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getTreble(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getTreble(s32Value);
    }
    return  s32Ret;
}

/**
 * 设置左右声道平衡
 * param[in] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setBalance(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setBalance(s32Value);
    }
    return  s32Ret;
}

/**
 * 获取左右声道平衡
 * param[out] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getBalance(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getBalance(s32Value);
    }
    return  s32Ret;
}

/**
 * 设置声音模式
 * param[in] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setSoundMode(enSoundMode);
    }
    return  s32Ret;
}

/**
 * 获取声音模式
 * param[out] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getSoundMode(XBH_SOUND_MODE_E *enSoundMode)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getSoundMode(enSoundMode);
    }
    return  s32Ret;
}

/**
 * 设置环绕声开关
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::setSurroundEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->setSurroundEnable(bEnable);
    }
    return  s32Ret;
}

/**
 * 获取环绕声开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierTask::getSurroundEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierInterface[0]->getSurroundEnable(bEnable);
    }
    return  s32Ret;
}

void XbhAudioAmplifierTask::run(const void* arg)
{   
    XBH_BOOL bInitDone = XBH_FALSE;
    XbhService::mAmpCreated = XBH_FALSE;
    for (int i = 0; i < 3; i++)
    {
        switch(mList1[i])
        {
            case AD82120_STEREO:
                m_pXbhAudioAmplifierInterface[i] = new ChipAd82120_Stereo(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel, 
                    mList2[i].mute_gpio, mList2[i].mute_level);
                break;
            case AD82120_SUBWOOFER:
                m_pXbhAudioAmplifierInterface[i] = new ChipAd82120_Subwoofer(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel, 
                    mList2[i].mute_gpio, mList2[i].mute_level);
                m_pXbhAudioAmplifierInterface[i]->getChipInitDone(&bInitDone);
                if(bInitDone == XBH_FALSE)
                {   
                    XLOGD(" AD82120_SUBWOOFER init %#x error!!!\n", mList2[i].iicAddr);
                    delete m_pXbhAudioAmplifierInterface[i];
                    m_pXbhAudioAmplifierInterface[i] = NULL;                   
                    break;
                }
                break;
            case ACM8625_STEREO:
                m_pXbhAudioAmplifierInterface[i] = new ChipAcm8625_Stereo(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel,
                    mList2[i].mute_gpio, mList2[i].mute_level);
                break;
            case ACM8625_SUBWOOFER:
                m_pXbhAudioAmplifierInterface[i] = new ChipAcm8625_Subwoofer(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel,
                    mList2[i].mute_gpio, mList2[i].mute_level);
                m_pXbhAudioAmplifierInterface[i]->getChipInitDone(&bInitDone);
                if(bInitDone == XBH_FALSE)
                {   
                    XLOGD(" ACM8625_SUBWOOFER init %#x error!!!\n", mList2[i].iicAddr);
                    delete m_pXbhAudioAmplifierInterface[i];
                    m_pXbhAudioAmplifierInterface[i] = NULL;                   
                    break;
                }
                break;
            case TAS5805M_STEREO:
                m_pXbhAudioAmplifierInterface[i] = new ChipTas5805m_Stereo(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel,
                    mList2[i].mute_gpio, mList2[i].mute_level);
                break;
            case TAS5805M_SUBWOOFER:
                m_pXbhAudioAmplifierInterface[i] = new ChipTas5805m_Subwoofer(mList2[i].iicBus, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel,
                    mList2[i].mute_gpio, mList2[i].mute_level);
                m_pXbhAudioAmplifierInterface[i]->getChipInitDone(&bInitDone);
                if(bInitDone == XBH_FALSE)
                {   
                    XLOGD(" TAS5805M_SUBWOOFER init %#x error!!!\n", mList2[i].iicAddr);
                    delete m_pXbhAudioAmplifierInterface[i];
                    m_pXbhAudioAmplifierInterface[i] = NULL;                   
                    break;
                }
                break;
            default:
                break;
        }
    }
    XbhService::mAmpCreated = XBH_TRUE;
}

XbhAudioAmplifierTask::XbhAudioAmplifierTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getAmpTable(mList1);
    XbhService::getModuleInterface()->getAmpOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhAudioAmplifierTask::~XbhAudioAmplifierTask()
{
}

XbhAudioAmplifierTask* XbhAudioAmplifierTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioAmplifierTask();
    }
    return mInstance;
}

