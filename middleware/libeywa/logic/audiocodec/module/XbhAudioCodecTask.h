#ifndef __XBH_AUDIO_CODEC_TASK_H__
#define __XBH_AUDIO_CODEC_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include "XbhAudioCodecInterface.h"

class XbhAudioCodecTask : public XbhMWThread
{
public:
    static XbhAudioCodecTask* getInstance();
    /**
     * 复位codec芯片
    */
    XBH_S32 resetChip();
    /**
     * 获取mic的插入状态
    */
    XBH_S32 getMicPlugIn(XBH_BOOL* enable);
    /**
     * 获取耳机的插入状态
    */
    XBH_S32 getHeadphonePlugIn(XBH_BOOL* enable);
    /**
     * 设置mic是否静音
    */
    XBH_S32 setMuteMicIn(XBH_BOOL mute);
    /**
     * 获取mic的静音状态
    */
    XBH_S32 getMuteMicIn(XBH_BOOL* mute);
    /**
     * 设置codec输出到speaker的那一路是否静音
    */
    XBH_S32 setSpeakerOut(XBH_BOOL mute);
    /**
     * 获取输出到speaker的那一路是否静音
    */
    XBH_S32 getSpeakerOut(XBH_BOOL* mute);
    /**
     * 设置line out是否静音
    */
    XBH_S32 setMuteDAC0(XBH_BOOL mute);
    /**
     * 获取line out是否静音
    */
    XBH_S32 getMuteDAC0(XBH_BOOL* mute);
    /**
     * 设置耳机输出是否静音
    */
    XBH_S32 setHeadphoneMute(XBH_BOOL mute);
    /**
     * 获取耳机输出是否静音
    */
    XBH_S32 getHeadphoneMute(XBH_BOOL* mute);
    /**
     * 设置line out的增益
    */
    XBH_S32 setLineOutGain(XBH_S32 gain);
    /**
     * 获取line out的增益
    */
    XBH_S32 getLineOutGain(XBH_S32* gain);
    /**
     * 设置mic in的增益
    */
    XBH_S32 setMicInGain(XBH_S32 gain);
    /**
     * 获取mic in的增益
    */
    XBH_S32 getMicInGain(XBH_S32* gain);
    /**
     * 设置MIC的防啸叫功能开关
    */
    XBH_S32 setMicHowling(XBH_BOOL enable);
    /**
     * 获取MIC的防啸叫功能开关
    */
    XBH_S32 getMicHowling(XBH_BOOL* enable);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_S32 * s32State);
    /**
     * 设置line out在线切换模式
     */
    XBH_S32 setLineOutMode(XBH_LINEOUT_MODE_E enLineOutMode);
    /**
     * 获取line out当前模式
     */
    XBH_S32 getLineOutMode(XBH_LINEOUT_MODE_E* enLineOutMode);
    
    ~XbhAudioCodecTask();

private:
    void run(const void* arg);
    XbhAudioCodecTask();
    static XbhAudioCodecTask *mInstance;
    static XbhMutex mLock;

    XbhAudioCodecInterface *m_pXbhAudioCodecInterface[2];

    XBH_U8 mList1[2];
    CODEC_OPT_TABLE mList2[2];
};

#endif
