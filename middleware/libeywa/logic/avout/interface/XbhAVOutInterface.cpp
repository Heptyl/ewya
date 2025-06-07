#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAVOutInterface"

#include "XbhLog.h"
#include "XbhAVOutInterface.h"

/**
 * 复位芯片
*/
XBH_S32 XbhAVOutInterface::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 恢复芯片
*/
XBH_S32 XbhAVOutInterface::resumeChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置HDMI(DP)TX的音频mute状态
*/
XBH_S32 XbhAVOutInterface::setHdmiTxAudioMute(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的音频mute状态
*/
XBH_S32 XbhAVOutInterface::getHdmiTxAudioMute(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置HDMI(DP)TX的视频mute状态
*/
XBH_S32 XbhAVOutInterface::setHdmiTxVideoMute(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的视频mute状态
*/
XBH_S32 XbhAVOutInterface::getHdmiTxVideoMute(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置HDMI(DP)TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 XbhAVOutInterface::setHdmiTxTiming(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 XbhAVOutInterface::getHdmiTxTiming(XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置HDMI(DP)TX的HDCP开关状态
 * 0：OFF，1：ON
*/
XBH_S32 XbhAVOutInterface::setHdmiTxHdcp(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 开启/关闭 vbo tx 差分信号
*/
XBH_S32 XbhAVOutInterface::setVboTxDiffSignals(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI(DP)TX的HDCP开关状态
*/
XBH_S32 XbhAVOutInterface::getHdmiTxHdcp(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 设置Vx1 Tx的Tcon模式
*/
XBH_S32 XbhAVOutInterface::setVx1TxTconMode(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhAVOutInterface::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhAVOutInterface::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhAVOutInterface::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhAVOutInterface::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhAVOutInterface::getUpgradeState(XBH_S32 *pState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhAVOutInterface::getHdmiTxPlugState(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
