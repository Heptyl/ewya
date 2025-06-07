#ifndef __XBH_AV_OUT_TASK_H__
#define __XBH_AV_OUT_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include "XbhAVOutInterface.h"

class XbhAVOutTask : public XbhMWThread
{
public:
    static XbhAVOutTask* getInstance();

    /**
     * 复位芯片
    */
    XBH_S32 resetChip(XBH_U32 devType);
    /**
     * 恢复芯片工作状态
    */
    XBH_S32 resumeChip(XBH_U32 devType);
    /**
     * 设置HDMI(DP)TX的音频mute状态
    */
    XBH_S32 setHdmiTxAudioMute(XBH_U32 devType, XBH_BOOL bEnable);
    /**
     * 开启/关闭 vbo tx 差分信号
    */
    XBH_S32 setVboTxDiffSignals(XBH_U32 devType, XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的音频mute状态
    */
    XBH_S32 getHdmiTxAudioMute(XBH_U32 devType, XBH_BOOL* info);
    /**
     * 设置HDMI(DP)TX的视频mute状态
    */
    XBH_S32 setHdmiTxVideoMute(XBH_U32 devType, XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的视频mute状态
    */
    XBH_S32 getHdmiTxVideoMute(XBH_U32 devType, XBH_BOOL* info);
    /**
     * 设置HDMI(DP)TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 setHdmiTxTiming(XBH_U32 devType, XBH_U8 value);
    /**
     * 获取HDMI(DP)TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 getHdmiTxTiming(XBH_U32 devType, XBH_CHAR* info);
    /**
     * 设置HDMI(DP)TX的HDCP开关状态
     * 0：OFF，1：ON
    */
    XBH_S32 setHdmiTxHdcp(XBH_U32 devType, XBH_BOOL bEnable);
    /**
     * 获取HDMI(DP)TX的HDCP开关状态
    */
    XBH_S32 getHdmiTxHdcp(XBH_U32 devType, XBH_BOOL* info);
    /**
     * 获取HDMI(DP)TX的设备接入情况
    */
    XBH_S32 getHdmiTxPlugState(XBH_U32 devType, XBH_BOOL* info);
    /**
    * 设置Vx1 Tx的Tcon模式
     */
    XBH_S32 setVx1TxTconMode(XBH_U32 devType, XBH_BOOL bEnable);
    XBH_S32 getChipExist(XBH_U32 devType, XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_U32 devType, XBH_BOOL* enable);
    /**
     * 获取固件版本
    */
    XBH_S32 getFirmwareVersion(XBH_U32 devType, XBH_CHAR* strVersion);
    /**
     * 进行固件升级
    */
    XBH_S32 upgradeFirmware(XBH_U32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     *获取固件升级状态
      0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
    */
    XBH_S32 getUpgradeState(XBH_U32 devType, XBH_S32 *pState);

    ~XbhAVOutTask();

private:
    void run(const void* arg);
    XbhAVOutTask();
    XbhAVOutInterface *getCurrIC(XBH_S32 devType);
    static XbhAVOutTask *mInstance;
    static XbhMutex mLock;

    XbhAVOutInterface *m_pXbhAVOutInterface[4];

    XBH_U8 mList1[4];
    COMMON_OPT_TABLE mList2[4];
};

#endif
