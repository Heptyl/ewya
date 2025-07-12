#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioAmplifierInterface"

#include "XbhLog.h"
#include "XbhAudioAmplifierInterface.h"

/**
 * 复位功放芯片
*/
XBH_S32 XbhAudioAmplifierInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置speaker mute的状态
*/
XBH_S32 XbhAudioAmplifierInterface::setMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取speaker mute的状态
*/
XBH_S32 XbhAudioAmplifierInterface::getMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置功放输出的增益值
*/
XBH_S32 XbhAudioAmplifierInterface::setGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 XbhAudioAmplifierInterface::getGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置输出的音频频率直通，不对频率做抑制，主要是针对subwoofer
 * 工厂测试的音频是1K HZ，subwoofer抑制了高频信号，导致无法测试到声音输出
*/
XBH_S32 XbhAudioAmplifierInterface::setDirectPass(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取输出的音频频率直通的状态
*/
XBH_S32 XbhAudioAmplifierInterface::getDirectPass(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取amp是否初始化完成
*/
XBH_S32 XbhAudioAmplifierInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhAudioAmplifierInterface::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhAudioAmplifierInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioAmplifierInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioAmplifierInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setAvcEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取AVC(自动电压控制)开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getAvcEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置EQ曲线(频率分布曲线)开关，可以用来打开或者关闭该曲线用于测试
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setEqEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取EQ曲线(频率分布曲线)开关
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getEqEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 设置音频输出的前置增益
* param[in] enEqMode. EQ阶段
* param[out] s32Value. 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 获取音频输出的前置增益
* param[in] enEqMode. EQ阶段
* param[out] s32Value. 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 设置低音增益
* param[in] s32Value 增益值
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setBass(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取低音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getBass(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置高音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setTreble(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取高音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getTreble(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置左右声道平衡
 * param[in] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setBalance(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取左右声道平衡
 * param[out] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getBalance(XBH_S32 *s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置声音模式
* param[in] s32Value
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取声音模式
* param[out] s32Value
* retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getSoundMode(XBH_SOUND_MODE_E *enSoundMode)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置环绕声开关
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::setSurroundEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取环绕声开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAudioAmplifierInterface::getSurroundEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
