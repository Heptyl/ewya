#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioAmplifierManager"

#include "XbhLog.h"
#include "XbhAudioAmplifierManager.h"

XbhAudioAmplifierManager*           XbhAudioAmplifierManager::mInstance = XBH_NULL;
XbhMutex                            XbhAudioAmplifierManager::mLock;

/**
 * 复位功放芯片
*/
XBH_S32 XbhAudioAmplifierManager::resetChip(XBH_AUDIO_CHANNEL_E channel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->resetChip(channel);
    }
    return  s32Ret;
}

/**
 * 设置speaker 的mute的状态
*/
XBH_S32 XbhAudioAmplifierManager::setMute(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setMute(channel, mute);
    }
    return  s32Ret;
}

/**
 * 获取speaker mute的状态
*/
XBH_S32 XbhAudioAmplifierManager::getMute(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getMute(channel, mute);
    }
    return  s32Ret;
}

/**
 * 设置功放输出的增益值
*/
XBH_S32 XbhAudioAmplifierManager::setGain(XBH_AUDIO_CHANNEL_E channel, XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setGain(channel, gain);
    }
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 XbhAudioAmplifierManager::getGain(XBH_AUDIO_CHANNEL_E channel, XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getGain(channel, gain);
    }
    return  s32Ret;
}

/**
 * 设置输出的音频频率直通，不对频率做抑制，主要是针对subwoofer
 * 工厂测试的音频是1K HZ，subwoofer抑制了高频信号，导致无法测试到声音输出
*/
XBH_S32 XbhAudioAmplifierManager::setDirectPass(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setDirectPass(channel, enable);
    }
    return  s32Ret;
}
/**
 * 获取输出的音频频率直通的状态
*/
XBH_S32 XbhAudioAmplifierManager::getDirectPass(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getDirectPass(channel, enable);
    }
    return  s32Ret;
}
/**
 * 获取amp是否初始化完成
*/
XBH_S32 XbhAudioAmplifierManager::getChipInitDone(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getChipInitDone(channel, enable);
    }
    return  s32Ret;
}
/**
* 获取chip是否存在
*/
XBH_S32 XbhAudioAmplifierManager::getChipExist(XBH_AUDIO_CHANNEL_E channel, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getChipExist(channel, enable);
    }
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhAudioAmplifierManager::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getFirmwareVersion(strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioAmplifierManager::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioAmplifierManager::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getUpgradeState(s32State);
    }
    return  s32Ret;
}

/**
 * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setAvcEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setAvcEnable(bEnable);
    }
    return  s32Ret;
}

/**
 * 获取AVC(自动电压控制)开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getAvcEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getAvcEnable(bEnable);
    }
    return  s32Ret;
}

/**
* 设置EQ曲线(频率分布曲线)开关，可以用来打开或者关闭该曲线用于测试
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setEqEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setEqEnable(bEnable);
    }
    return  s32Ret;
}

/**
* 获取EQ曲线(频率分布曲线)开关
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getEqEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getEqEnable(bEnable);
    }
    return  s32Ret;
}

/**
* 设置音频输出的前置增益
* param[in] enEqMode. EQ阶段
* param[out] s32Value. 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setAudioEq(enEqMode, s32Value);
    }
    return  s32Ret;
}

/**
* 获取音频输出的前置增益
* param[in] enEqMode. EQ阶段
* param[out] s32Value. 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getAudioEq(enEqMode, s32Value);
    }
    return  s32Ret;
}

/**
* 设置低音增益
* param[in] s32Value 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setBass(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setBass(s32Value);
    }
    return  s32Ret;
}

/**
 * 获取低音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getBass(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getBass(s32Value);
    }
    return  s32Ret;
}

/**
 * 设置高音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setTreble(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setTreble(s32Value);
    }
    return  s32Ret;
}

/**
 * 获取高音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getTreble(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getTreble(s32Value);
    }
    return  s32Ret;
}

/**
 * 设置左右声道平衡
 * param[in] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setBalance(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setBalance(s32Value);
    }
    return  s32Ret;
}

/**
 * 获取左右声道平衡
 * param[out] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getBalance(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getBalance(s32Value);
    }
    return  s32Ret;
}
/**
 * 设置声音模式
 * param[in] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setSoundMode(enSoundMode);
    }
    return  s32Ret;
}

/**
 * 获取声音模式
 * param[out] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getSoundMode(XBH_SOUND_MODE_E *enSoundMode)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getSoundMode(enSoundMode);
    }
    return  s32Ret;
}

/**
 * 设置环绕声开关
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::setSurroundEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->setSurroundEnable(bEnable);
    }
    return  s32Ret;
}

/**
 * 获取环绕声开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierManager::getSurroundEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioAmplifierTask != NULL)
    {
        s32Ret = m_pXbhAudioAmplifierTask->getSurroundEnable(bEnable);
    }
    return  s32Ret;
}

XbhAudioAmplifierManager::XbhAudioAmplifierManager()
{
    XLOGD(" begin ");
    m_pXbhAudioAmplifierTask = XBH_NULL;
    m_pXbhAudioAmplifierTask = XbhAudioAmplifierTask::getInstance();
    XLOGD(" end ");
}

XbhAudioAmplifierManager::~XbhAudioAmplifierManager()
{
    if(m_pXbhAudioAmplifierTask != XBH_NULL)
    {
        delete m_pXbhAudioAmplifierTask;
    }
    m_pXbhAudioAmplifierTask = XBH_NULL;
}

XbhAudioAmplifierManager* XbhAudioAmplifierManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioAmplifierManager();
    }
    return mInstance;
}

