#ifndef __XBH_AV_OUT_INTERFACE_H__
#define __XBH_AV_OUT_INTERFACE_H__

#include "XbhType.h"

class XbhAVOutInterface
{
public:
    /**
     * 复位芯片
    */
    virtual XBH_S32 resetChip();
    /**
     * 恢复芯片工作状态
    */
    virtual XBH_S32 resumeChip();
    /**
     * 设置HDMI(DP)TX的音频mute状态
    */
    virtual XBH_S32 setHdmiTxAudioMute(XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的音频mute状态
    */
    virtual XBH_S32 getHdmiTxAudioMute(XBH_BOOL* info);
    /**
     * 设置HDMI(DP)TX的视频mute状态
    */
    virtual XBH_S32 setHdmiTxVideoMute(XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的视频mute状态
    */
    virtual XBH_S32 getHdmiTxVideoMute(XBH_BOOL* info);
    /**
     * 设置HDMI(DP)TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    virtual XBH_S32 setHdmiTxTiming(XBH_U8 value);
    /**
     * 获取HDMI(DP)TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    virtual XBH_S32 getHdmiTxTiming(XBH_CHAR* info);
    /**
     * 设置HDMI(DP)TX的HDCP开关状态
     * 0：OFF，1：ON
    */
    virtual XBH_S32 setHdmiTxHdcp(XBH_BOOL bEnable);
    /**
     * 开启/关闭 vbo tx 差分信号
    */
    virtual XBH_S32 setVboTxDiffSignals(XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的HDCP开关状态
    */
    virtual XBH_S32 getHdmiTxHdcp(XBH_BOOL* info);
    /**
    * 设置Vx1 Tx的Tcon模式
     */
    virtual XBH_S32 setVx1TxTconMode(XBH_BOOL bEnable);
    /**
     * 判断当前转换芯片是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* bEnable);
    /**
    * 判断当前转换芯片是否初始化完成
     */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 判断当前转换芯片的版本
    */
    virtual XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
     * 转换芯片升级固件
    */
    virtual XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     * 获取转换芯片的升级状态
    */
    virtual XBH_S32 getUpgradeState(XBH_S32 *pState);
    /**
     * 获取HDMI(DP)TX的设备接入情况
    */
    virtual XBH_S32 getHdmiTxPlugState(XBH_BOOL* info);


public:
    XbhAVOutInterface(){};
    virtual ~XbhAVOutInterface(){};
};

#endif
