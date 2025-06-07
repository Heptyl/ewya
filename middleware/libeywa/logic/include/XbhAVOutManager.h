#ifndef __XBH_AV_OUT_MANAGER_H__
#define __XBH_AV_OUT_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAVOutTask.h"

/**
    音视频输出管理类，视频转VBO/HDMI/DP都属于这个类来管理
 */
class XbhAVOutManager
{
public:
    static XbhAVOutManager* getInstance();

    /**
     * 复位芯片
    */
    XBH_S32 resetChip(XBH_S32 devType);
    /**
     * 恢复芯片工作状态
    */
    XBH_S32 resumeChip(XBH_S32 devType);
    /**
     * 设置HDMI(DP)TX的音频mute状态
    */
    XBH_S32 setHdmiTxAudioMute(XBH_S32 devType, XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的音频mute状态
    */
    XBH_S32 getHdmiTxAudioMute(XBH_S32 devType, XBH_BOOL* info);
    /**
     * 设置HDMI(DP)TX的视频mute状态
    */
    XBH_S32 setHdmiTxVideoMute(XBH_S32 devType, XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的视频mute状态
    */
    XBH_S32 getHdmiTxVideoMute(XBH_S32 devType, XBH_BOOL* info);
    /**
     * 设置HDMI(DP)TX的timing
     * 0：480P，1：1080P， 2：2160P ，0xFF Auto
    */
    XBH_S32 setHdmiTxTiming(XBH_S32 devType, XBH_U8 value);
    /**
     * 获取HDMI(DP)TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 getHdmiTxTiming(XBH_S32 devType, XBH_CHAR* info);
    /**
     * 设置HDMI(DP)TX的HDCP开关状态
     * 0：OFF，1：ON
    */
    XBH_S32 setHdmiTxHdcp(XBH_S32 devType, XBH_BOOL bEnable);
    /**
     * 开启/关闭 vbo tx 差分信号
    */
    XBH_S32 setVboTxDiffSignals(XBH_S32 devType, XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的HDCP开关状态
    */
    XBH_S32 getHdmiTxHdcp(XBH_S32 devType, XBH_BOOL* info);
    /**
     * 获取HDMI(DP)TX的设备接入情况
    */
    XBH_S32 getHdmiTxPlugState(XBH_S32 devType, XBH_BOOL* info);
    /**
    * 设置Vx1 Tx的Tcon模式
     */
    XBH_S32 setVx1TxTconMode(XBH_S32 devType, XBH_BOOL bEnable);

    XBH_S32 getChipExist(XBH_S32 devType, XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_S32 devType, XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_U32 devType, XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(XBH_U32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_U32 devType, XBH_S32 * s32State);

    ~XbhAVOutManager();

private:
    XbhAVOutManager();
    static XbhAVOutManager *mInstance;
    static XbhMutex mLock;

    XbhAVOutTask *m_pXbhAVOutTask;
};

#endif
