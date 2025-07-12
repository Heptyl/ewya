#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioCodecInterface"

#include "XbhLog.h"
#include "XbhAudioCodecInterface.h"

/**
 * 复位codec芯片
*/
XBH_S32 XbhAudioCodecInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取mic的插入状态
*/
XBH_S32 XbhAudioCodecInterface::getMicPlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取耳机的插入状态
*/
XBH_S32 XbhAudioCodecInterface::getHeadphonePlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置mic是否静音
*/
XBH_S32 XbhAudioCodecInterface::setMuteMicIn(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取mic的静音状态
*/
XBH_S32 XbhAudioCodecInterface::getMuteMicIn(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置codec输出到speaker的那一路是否静音
*/
XBH_S32 XbhAudioCodecInterface::setSpeakerOut(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取输出到speaker的那一路是否静音
*/
XBH_S32 XbhAudioCodecInterface::getSpeakerOut(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置line out是否静音
*/
XBH_S32 XbhAudioCodecInterface::setMuteDAC0(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取line out是否静音
*/
XBH_S32 XbhAudioCodecInterface::getMuteDAC0(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置耳机输出是否静音
*/
XBH_S32 XbhAudioCodecInterface::setHeadphoneMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取耳机输出是否静音
*/
XBH_S32 XbhAudioCodecInterface::getHeadphoneMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置line out的增益
*/
XBH_S32 XbhAudioCodecInterface::setLineOutGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取line out的增益
*/
XBH_S32 XbhAudioCodecInterface::getLineOutGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置mic in的增益
*/
XBH_S32 XbhAudioCodecInterface::setMicInGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取mic in的增益
*/
XBH_S32 XbhAudioCodecInterface::getMicInGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置MIC的防啸叫功能开关
*/
XBH_S32 XbhAudioCodecInterface::setMicHowling(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取MIC的防啸叫功能开关
*/
XBH_S32 XbhAudioCodecInterface::getMicHowling(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhAudioCodecInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhAudioCodecInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhAudioCodecInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioCodecInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioCodecInterface::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置line out在线切换模式
 */
XBH_S32 XbhAudioCodecInterface::setLineOutMode(XBH_LINEOUT_MODE_E enLineOutMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取line out当前模式
 */
XBH_S32 XbhAudioCodecInterface::getLineOutMode(XBH_LINEOUT_MODE_E* enLineOutMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
