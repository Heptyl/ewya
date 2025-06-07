#ifndef __XBH_AUDIO_AMPLIFIER_INTERFACE_H__
#define __XBH_AUDIO_AMPLIFIER_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhAudioAmplifierInterface
{
public:
    /**
     * 复位功放芯片
    */
    virtual XBH_S32 resetChip();
    /**
     * 设置speaker mute的状态
    */
    virtual XBH_S32 setMute(XBH_BOOL mute);
    /**
     * 获取speaker mute的状态
    */
    virtual XBH_S32 getMute(XBH_BOOL* mute);
    /**
     * 设置功放输出的增益值
    */
    virtual XBH_S32 setGain(XBH_S32 gain);
    /**
     * 获取功放输出的增益值
    */
    virtual XBH_S32 getGain(XBH_S32* gain);
    /**
     * 设置输出的音频频率直通，不对频率做抑制，主要是针对subwoofer
     * 工厂测试的音频是1K HZ，subwoofer抑制了高频信号，导致无法测试到声音输出
    */
    virtual XBH_S32 setDirectPass(XBH_BOOL enable);
    /**
     * 获取输出的音频频率直通的状态
    */
    virtual XBH_S32 getDirectPass(XBH_BOOL* enable);
    /**
     * 获取amp是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* bEnable);
    /**
     * 获取固件版本
     */
    virtual XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
     * 进行固件升级
     */
    virtual XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     * 获取固件升级时的状态
     */
    virtual XBH_S32 getUpgradeState(XBH_S32 * s32State);
    /**
    * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
    * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAvcEnable(XBH_BOOL bEnable);
    /**
    * 获取AVC(自动电压控制)开关
    * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAvcEnable(XBH_BOOL* bEnable);

    /**
    * 设置EQ曲线(频率分布曲线)开关，可以用来打开或者关闭该曲线用于测试
    * param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setEqEnable(XBH_BOOL bEnable);
    /**
    * 获取EQ曲线(频率分布曲线)开关
    * param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getEqEnable(XBH_BOOL* bEnable);
    /**
    * 设置低音增益
    * param[in] s32Value 增益值
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBass(XBH_S32 s32Value);
    /**
    * 获取低音增益
    * param[out] s32Value 增益值
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBass(XBH_S32 *s32Value);
    /**
    * 设置高音增益
    * param[in] s32Value 增益值
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setTreble(XBH_S32 s32Value);
    /**
    * 获取高音增益
    * param[out] s32Value 增益值
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getTreble(XBH_S32 *s32Value);
    /**
    * 设置左右声道平衡
    * param[in] s32Value 平衡值 【-50~+50】
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBalance(XBH_S32 s32Value);
    /**
    * 获取左右声道平衡
    * param[out] s32Value 平衡值 【-50~+50】
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBalance(XBH_S32 *s32Value);
    /**
    * 设置声音模式
    * param[in] s32Value
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSoundMode(XBH_SOUND_MODE_E enSoundMode);
    /**
    * 获取声音模式
    * param[out] s32Value
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSoundMode(XBH_SOUND_MODE_E *enSoundMode);
    /**
    * 设置环绕声开关
    * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSurroundEnable(XBH_BOOL bEnable);
    /**
    * 获取环绕声开关
    * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSurroundEnable(XBH_BOOL* bEnable);
public:
    XbhAudioAmplifierInterface(){};
    virtual ~XbhAudioAmplifierInterface(){};
};

#endif
