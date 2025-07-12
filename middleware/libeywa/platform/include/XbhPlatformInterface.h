#ifndef XBH_PLATFORM_INTERFACE_H
#define XBH_PLATFORM_INTERFACE_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include <list>
#include <string>
#include <termios.h>

class XbhPlatformInterface
{
public:
    /******************************************************************************PICTURE***********************************************************/
    /**
     * 设置图像模式
     * param[in] enPicMode. 图像模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPictureMode(XBH_PICMODE_E enPicMode);
    /**
     * 获取图像模式
     * param[out] enPicMode. 图像模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPictureMode(XBH_PICMODE_E* enPicMode);
    /**
     * 设置图像亮度
     * param[in] u32Value. 图像亮度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBrightness(XBH_U32 u32Value);
    /**
     * 获取图像亮度
     * param[out] u32Value. 图像亮度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBrightness(XBH_U32* u32Value);
    /**
     * 设置图像对比度值
     * param[in] u32Value. 图像对比度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setContrast(XBH_U32 u32Value);
    /**
     * 获取图像对比度值
     * param[out] u32Value. 图像对比度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getContrast(XBH_U32* u32Value);
    /**
     * 设置图像饱和度值
     * param[in] u32Value. 图像饱和度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSaturation(XBH_U32 u32Value);
    /**
     * 获取图像饱和度值
     * param[out] u32Value. 图像饱和度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSaturation(XBH_U32* u32Value);
    /**
     * 设置图像锐度是否可调节
     * param[in] u32Value.  0 关闭锐度调节，1 打开锐度调节
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSharpEnable(XBH_U32 u32Value);
    /**
     * 设置图像锐度值
     * param[in] u32Value. 图像锐度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSharpness(XBH_U32 u32Value);
    /**
     * 获取图像锐度值
     * param[out] u32Value. 图像锐度值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSharpness(XBH_U32* u32Value);
    /**
     * 设置图像色调值
     * param[in] u32Value. 图像色调值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHue(XBH_U32 u32Value);
    /**
     * 获取图像色调值
     * param[out] u32Value. 图像色调值，范围0~100
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHue(XBH_U32* u32Value);
    /**
     * 设置图像背光值
     * param[in] u32Value. 图像背光值，范围0~100
     * param[in] enPanel. 屏幕序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBacklight(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel);
    /**
     * 获取图像背光值
     * param[out] u32Value. 图像背光值，范围0~100
     * param[in] enPanel. 屏幕序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBacklight(XBH_U32* u32Value, XBH_PANEL_NUM_E enPanel);
    /**
     * 设置临时图像背光值，数值不保存
     * param[in] u32Value. 图像背光值，范围0~100
     * param[in] enPanel. 屏幕序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBacklightWithOutSave(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel);
    /**
     * 获取背光不保存值的值,也是实际正式生效的PWM对应的背光值
     * param[in] enPanel. 屏幕序号
     * retval u32Value. 图像背光值，范围0~100
    */
    virtual XBH_S32 getBacklightWithOutSave(XBH_U32* value, XBH_PANEL_NUM_E enPanel);
    /**
     * 设置图像色温模式
     * param[in] enColorTemp. 色温模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setColorTempMode(XBH_COLORTEMP_E enColorTemp);
    /**
     * 获取图像色温模式
     * param[out] enColorTemp. 色温模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getColorTempMode(XBH_COLORTEMP_E *enColorTemp);
    /**
     * 设置图像降噪等级
     * param[in] enNrLevel. 降噪等级
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setNR(XBH_LEVEL_E enNrLevel);
    /**
     * 获取图像降噪等级
     * param[out] enNrLevel. 降噪等级
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getNR(XBH_LEVEL_E* enNrLevel);
    /**
     * 设置图像的游戏模式开关
     * param[in] bEnable. XBH_TRUE：打开游戏模式，XBH_FALSE：关闭游戏模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGameModeEnable(XBH_BOOL bEnable);
    /**
     * 获取图像的游戏模式开关
     * param[out] bEnable. XBH_TRUE：打开游戏模式，XBH_FALSE：关闭游戏模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGameModeEnable(XBH_BOOL* bEnable);
    /**
     * 设置图像的PC模式，有的方案又叫Graphic模式，与之对应的是Video模式，通常接入的是RGB信号时打开PC模式，
     * 反之就是video模式，默认为系统自动判断。PC模式时图像重显率为100%，并且降低了锐度和关闭了部分PQ效果，
     * 在接PC信号的时候看得比较明显。
     * param[in] enPcMode. PC模式的模式序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPCMode(XBH_PCMODE_E enPcMode);
    /**
     * 获取图像的PC模式
     * param[out] enPcMode. PC模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPCMode(XBH_PCMODE_E* enPcMode);
    /**
     * 设置HDMI信号的色域范围
     * param[in] enColorRange. 色域模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHDMIColorRange(XBH_COLOR_RANGE_E enColorRange);
    /**
     * 获取HDMI信号的色域范围
     * param[out] enColorRange. 色域模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHDMIColorRange(XBH_COLOR_RANGE_E* enColorRange);
    /**
     * 设置图像的动态对比度开关
     * param[in] bEnable. XBH_TRUE：打开动态对比度，XBH_FALSE：关闭动态对比度
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setDCREnable(XBH_BOOL bEnable);
    /**
     * 获取图像的动态对比度开关
     * param[out] bEnable. XBH_TRUE：打开动态对比度，XBH_FALSE：关闭动态对比度
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDCREnable(XBH_BOOL* bEnable);
    /**
     * 设置图像的运动补偿等级
     * param[in] enMemcLevel. 运动补偿等级
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setMEMCLevel(XBH_LEVEL_E enMemcLevel);
    /**
     * 获取图像的运动补偿等级
     * param[out] enMemcLevel. 运动补偿等级
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getMEMCLevel(XBH_LEVEL_E* enMemcLevel);
    /**
     * 设置图像的画面显示比例
     * param[in] enAspect. 画面显示比例
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAspect(XBH_ASPECT_E enAspect);
    /**
     * 获取图像的画面显示比例
     * param[out] enAspect. 画面显示比例
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAspect(XBH_ASPECT_E* enAspect);
    /**
     * 获取图像的画面的平均亮度值，一般用来做能源之星功能用
     * param[out] u32Value. 画面的平均亮度值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAvgPixelLuma(XBH_U32* u32Value);
    /**
     * 设置图像的gamma，一般预置3组gamma
     * param[in] u32Value. gamma组的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGammaGroup(XBH_S32 s32Value);
    /**
     * 获取图像的gamma组的序号
     * param[out] u32Value. gamma组的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGammaGroup(XBH_S32* s32Value);
    /**
     * 设置对应模式的具体色温数据
     * param[in] enColorTemp. 色温模式
     * param[in] data. 色温参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    /**
     * 获取对应模式的具体色温数据
     * param[in] enColorTemp. 色温模式
     * param[out] data. 色温参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    /**
     * 保存对应模式的具体色温数据，保存到cusdata分区
     * param[in] enColorTemp. 色温模式
     * param[in] data. 色温参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    /**
     * 获取android对应模式的具体色温数据
     * param[in] enColorTemp. 色温模式
     * param[out] data. 色温参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    /**
     * 保存android对应模式的具体色温数据，保存到cusdata分区
     * param[in] enColorTemp. 色温模式
     * param[in] data. 色温参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 saveColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    /**
     * 读取对应模式的具体色温数据，从cusdata分区读取
     * param[in] enColorTemp. 色温模式
     * param[out] data. 色温参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 loadColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);

    /******************************************************************************AUDIO***********************************************************/
    /**
     * 设置声音模式
     * param[in] enSoundMode. 声音模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSoundMode(XBH_SOUND_MODE_E enSoundMode);
    /**
     * 获取声音模式
     * param[out] enSoundMode. 声音模式的序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSoundMode(XBH_SOUND_MODE_E* enSoundMode);
    /**
     * 设置各个声音通道的音量大小
     * param[in] enAudioChannel. 声音通道，u32Value音量大小
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setVolume(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_U32 u32Value);
    /**
     * 获取各个声音通道的音量大小
     * param[out] enAudioChannel. 声音通道，u32Value音量大小
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVolume(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_U32* u32Value);
    /**
     * 设置音频输出通路模式
     * param[in] enAudioOutput. 声音输出的通路模式
     * XBH_AUDIO_OUTPUT_DET,        根据接入的设备将音频输出切换到设备上
     * XBH_AUDIO_OUTPUT_BOTH,       Speaker跟外接设备同时出声
     * XBH_AUDIO_OUTPUT_EXTERNAL,   只有外置设备出声音
     * XBH_AUDIO_OUTPUT_INTERNAL,   只有Speaker出声音
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput);
    /**
     * 获取音频输出通路模式
     * param[out] enAudioOutput. 音频输出通路模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAudioOutput(XBH_AUDIO_OUTPUT_E* enAudioOutput);
    /**
     * 设置音频通路的静音开关
     * param[in] enAudioChannel. 声音通道，bEnable 静音开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable);
    /**
     * 获取音频通路的静音开关
     * param[out] enAudioChannel. 声音通道，bEnable 静音开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable);
    /**
     * 设置同轴的输出模式
     * param[in] enSpdifMode. 输出模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSpdifMode(XBH_SPDIF_MODE_E enSpdifMode);
    /**
     * 获取同轴的输出模式
     * param[out] enSpdifMode. 输出模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSpdifMode(XBH_SPDIF_MODE_E* enSpdifMode);
    /**
     * 设置同轴的开关
     * param[in] bEnable. 同轴开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSpdifEnable(XBH_BOOL bEnable);
    /**
     * 获取同轴的开关
     * param[out] bEnable. 同轴开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSpdifEnable(XBH_BOOL* bEnable);
    /**
     * 设置ARC输出的模式
     * param[in] enArcMode. ARC输出的模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setARCMode(XBH_ARC_MODE_E enArcMode);
    /**
     * 获取ARC输出的模式
     * param[out] enArcMode. ARC输出的模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getARCMode(XBH_ARC_MODE_E* enArcMode);
    /**
     * 设置ARC输出的开关
     * param[in] enArcMode. ARC输出的开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setARCEnable(XBH_BOOL bEnable);
    /**
     * 获取ARC输出的开关
     * param[out] enArcMode. ARC输出的开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getARCEnable(XBH_BOOL* bEnable);
    /**
     * 设置音频输出的前置增益
     * param[in] s32Value. 音频增益值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPreScale(XBH_S32 s32Value);
    /**
     * 获取音频输出的前置增益
     * param[out] s32Value. 音频增益值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPreScale(XBH_S32* s32Value);
    /**
     * 设置音频输出的前置增益
     * param[in] enEqMode. EQ阶段
     * param[out] s32Value. 增益值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value);
    /**
     * 获取音频输出的前置增益
     * param[in] enEqMode. EQ阶段
     * param[out] s32Value. 增益值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value);
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
    virtual XBH_S32 getBass(XBH_S32* s32Value);
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
    virtual XBH_S32 getTreble(XBH_S32* s32Value);
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
    virtual XBH_S32 getBalance(XBH_S32* s32Value);
    /**
     * 设置line out的模式，通常接入耳机和line out时需要不同的音量大小，通过这个开关来决定输出幅度
     * param[in] enLineOutMode 输出模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setLineOutMode(XBH_LINEOUT_MODE_E enLineOutMode);
    /**
     * 获取line out的模式
     * param[out] enLineOutMode 输出模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getLineOutMode(XBH_LINEOUT_MODE_E* enLineOutMode);
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
     * 设置功放端的声音模式
     * param[in] mode 声音模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAmpMode(XBH_S32 mode);
    /**
     * 获取功放端的声音模式
     * param[out] mode 声音模式
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAmpMode(XBH_S32* mode);

    /******************************************************************************VGA***********************************************************/
    /**
     * 设置VGA的RGB采样的ADC自动校准值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 autoAdc(XBH_VOID);
    /**
     * 设置VGA的图像位置相位等自动校准值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 autoAdjust(XBH_VOID);
    /**
     * 设置VGA图像的水平位置
     * param[in] u32Value 位置参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHPosition(XBH_U32 u32Value);
    /**
     * 获取VGA图像的水平位置
     * param[out] u32Value 位置参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHPosition(XBH_U32* u32Value);
    /**
     * 设置VGA图像的垂直位置
     * param[in] u32Value 位置参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setVPosition(XBH_U32 u32Value);
    /**
     * 获取VGA图像的垂直位置
     * param[out] u32Value 位置参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVPosition(XBH_U32* u32Value);
    /**
     * 设置VGA图像的时钟位置
     * param[in] u32Value 时钟参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setClock(XBH_U32 u32Value);
    /**
     * 获取VGA图像的时钟位置
     * param[out] u32Value 位置参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getClock(XBH_U32* u32Value);
    /**
     * 设置VGA图像的相位位置
     * param[in] u32Value 相位参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPhase(XBH_U32 u32Value);
    /**
     * 获取VGA图像的相位位置
     * param[out] u32Value 位置参数
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPhase(XBH_U32* u32Value);
    /******************************************************************************CEC**************************************************************/
    /**
     * 设置CEC的总开关
     * param[in] enable 开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setCecEnable(XBH_BOOL enable);
    /**
     * 获取CEC的总开关
     * param[out] enable 开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCecEnable(XBH_BOOL* enable);
    /**
     * 设置跟随CEC设备自动关机的开关
     * param[in] enable 开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setCecAutoPowerOffEnable(XBH_BOOL enable);
    /**
     * 获取跟随CEC设备自动关机的开关
     * param[out] enable 开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCecAutoPowerOffEnable(XBH_BOOL* enable);
    /**
     * 设置跟随CEC设备自动开机的开关
     * param[in] enable 开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setCecAutoWakeUpEnable(XBH_BOOL enable);
    /**
     * 获取跟随CEC设备自动开机的开关
     * param[out] enable 开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCecAutoWakeUpEnable(XBH_BOOL* enable);
    /******************************************************************************SOURCE***********************************************************/
    /**
     * 设置信源窗口的位置和大小
     * param[in] stRect 窗口位置大小， windows 主窗口/子窗口
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setWindowRect(XBH_RECT_S stRect, XBH_WINDOW_NUM_E windows = XBH_WINDOW_MAIN);
    /**
     * 获取信源窗口的位置和大小
     * param[in] windows 主窗口/子窗口
     * param[out] stRect 窗口位置大小
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getWindowRect(XBH_RECT_S* stRect, XBH_WINDOW_NUM_E windows = XBH_WINDOW_MAIN);
    /**
     * 获取信源的timing信息
     * param[in] windows 主窗口/子窗口
     * param[out] timing 信号信息
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getTimingInfo(XBH_TIMING_INFO_S* timing, XBH_WINDOW_NUM_E windows = XBH_WINDOW_MAIN);
    /**
     * 设置信源的EDID类型
     * param[in] enType EDID类型，
     *           bd_idx bd_idx source.json中的bd_idx
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx);
    /**
     * 获取信源的EDID类型
     * param[in] bd_idx bd_idx source.json中的bd_idx
     * param[out] enType EDID类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType);
    /**
    * 获取接入的ops的系统类型
    * param[in] enSource 指定的信源通道
    * param[out] type XBH_TRUE android，XBH_FALSE windows
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsSystemType(XBH_BOOL *type, XBH_SOURCE_E enSource);
    /**
     * 设置OPS的电源控制引脚，当开机状态时执行此接口会让OPS关机，关机的时候执行此即可会让OPS开机
     * param[in] enSource 指定的信源通道
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setOpsPowerCtrl(XBH_SOURCE_E enSource = XBH_SOURCE_OPS1);
    /**
     * 获取OPS是否是开机状态
     * param[in] enSource 指定的信源通道
     * param[out] bEnable XBH_TRUE 开机状态，XBH_FALSE 关机状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsPowerStatus(XBH_BOOL* bEnable, XBH_SOURCE_E enSource = XBH_SOURCE_OPS1);
    /**
     * 获取OPS是否是插入的状态
     * param[in] enSource 指定的信源通道
     * param[out] bEnable XBH_TRUE 插入OPS，XBH_FALSE 未插入OPS
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsDetStatus(XBH_BOOL* bEnable, XBH_SOURCE_E enSource = XBH_SOURCE_OPS1);
    /**
     * 设置OPS的供电状态（19V供电）
     * param[in] enSource 指定的信源通道
     * param[in] bEnable XBH_TRUE 给OPS供电，XBH_FALSE 给OPS断电
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource = XBH_SOURCE_OPS1);
    /**
     * 设置gsv27xx系列CEC物理地址
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGsv27xxPhysicalAddr(XBH_VOID);
    /**
     * 获取OPS的供电状态（19V供电）
     * param[in] enSource 指定的信源通道
     * param[out] bEnable XBH_TRUE OPS已经供电，XBH_FALSE OPS未供电
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsPowerEnable(XBH_BOOL* bEnable, XBH_SOURCE_E enSource = XBH_SOURCE_OPS1);
    /**
     * 设置HDMI TX
     * param[in] enInfo hdmi tx的信息类型
     * param[in] u32Data 信息的具体数据
     * 当选择XBH_HDMI_TX_TIMING时 u32Data 0：480P，1：1080P， 2：21600P ，0xFF Auto
     * 当选择XBH_HDMI_TX_HDCP_ONOFF时 u32Data 0：关闭HDCP校验，1：开启HDCP校验
     * 当选择XBH_HDMI_TX_VIDEO_MUTE时 u32Data 0：图像UNMUTE，1：图像MUTE
     * 当选择XBH_HDMI_TX_AUDIO_MUTE时 u32Data 0：声音UNMUTE，1：声音MUTE
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data);
    /**
     * 获取HDMI TX的信息
     * param[in] enInfo hdmi tx的信息类型
     * param[out] u32Data 信息的具体数据
     * 当选择XBH_HDMI_TX_TIMING时 u32Data 0：480P，1：1080P， 2：21600P ，0xFF Auto
     * 当选择XBH_HDMI_TX_HDCP_ONOFF时 u32Data 0：关闭HDCP校验，1：开启HDCP校验
     * 当选择XBH_HDMI_TX_VIDEO_MUTE时 u32Data 0：图像UNMUTE，1：图像MUTE
     * 当选择XBH_HDMI_TX_AUDIO_MUTE时 u32Data 0：声音UNMUTE，1：声音MUTE
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info);

    /**
     * 设置OPS reset
    */
    virtual XBH_S32 setOpsResetEnable(XBH_BOOL bEnable);

    /**
    * 设置HDMI TX/TYPEC TX 模式
    * param[in] 按照HDMI TX还是TYPEC TX输出。0：hdmi tx， 1： typec tx
    */
    virtual XBH_S32 setVideoTxMode(XBH_S32 value);

    /**
    * 获取HDMI TX/TYPEC TX 模式
    * param[out] 按照HDMI TX还是TYPEC TX输出。0：hdmi tx， 1： typec tx
    */
    virtual XBH_S32 getVideoTxMode(XBH_S32* value);
    /**
     * get hdmi rx audio sample freq
    */
    virtual XBH_S32 getHdmiRxAudioSampleFreq(XBH_U32 *u32Data);
    /**
     * get hdmi rx edid invaild
    */
    virtual XBH_S32 getEdidInvaild(XBH_HDMIRX_EDID_TYPE_E edidType, XBH_U8 *edidInvaild);
    /**
     * get hdmi rx audio locked status
    */
    virtual XBH_S32 getHdmiRxAudioLocked(XBH_U32 *u32Data);
    /******************************************************************************SYSTEM***********************************************************/
    /**
     * 获取唤醒原因
     * param[out] stWake 唤醒源的具体信息
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getWakeUpReason(XBH_WAKEUP_S *stWake);
    /**
     * 设置bootloader的系统属性
     * param[in] key 属性名
     * param[in] value 属性值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBootEnv(const XBH_CHAR* key, XBH_CHAR* value);
    /**
     * 获取bootloader的系统属性
     * param[in] key 属性名
     * param[out] value 属性值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBootEnv(const XBH_CHAR* key, XBH_CHAR* value);
    /**
     * 设置vendor层的系统属性
     * param[in] key 属性名
     * param[in] value 属性值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setVendorProperties(const XBH_CHAR* key, XBH_CHAR* value);
    /**
     * 设置上电模式
     * param[in] enMode 上电模式的类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPoweronMode(XBH_POWERMODE_E enMode);
    /**
     * 获取上电模式
     * param[out] enMode 上电模式的类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPoweronMode(XBH_POWERMODE_E* enMode);
    /**
     * 设置唤醒类型的开关
     * param[in] enWakeUpType 唤醒源
     * param[in] bEnable 唤醒开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable);
    /**
     * 获取唤醒类型的开关
     * param[in] enWakeUpType 唤醒源
     * param[out] bEnable 唤醒开关
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable);
    /**
     * 设置故障检测的开关
     * param[in] bEnable 检测开关
     * param[in] enDetectType 检测类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setErrorDetectState(XBH_BOOL bEnable, XBH_U32 enDetectType);
    /**
     * 获取故障检测的开关
     * param[out] bEnable 检测开关
     * param[out] enDetectType 检测类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getErrorDetectState(XBH_BOOL* bEnable, XBH_U32* enDetectType);
    /**
     * 获取故障码
     * param[out] u32ErrorCode
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getErrorCode(XBH_U32* u32ErrorCode);
    /**
     * 清除故障码
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 clearErrorCode(void);
    /**
     * 设置系统运行的时长，此记录不会因为升级而被擦除掉
     * param[in] time 运行时间
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setChipRunTime(XBH_S32 time);
    /**
     * 获取系统运行的时长，此记录不会因为升级而被擦除掉
     * param[out] time 运行时间
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getChipRunTime(XBH_S32* time);
    /**
     * 设置开机logo
     * param[in] path 开机logo资源的路径
     * retval true:success, false:failure
    */
    virtual XBH_S32 setBootLogo(const XBH_CHAR* path);
    /**
     * 设置开机视频
     * param[in] level 开机视频资源的路径
     * retval true:success, false:failure
    */
    virtual XBH_S32 setBootVideo(const XBH_CHAR* path);
    /**
     * 设置开机动画
     * param[in] level 开机动画资源的路径
     * retval true:success, false:failure
    */
    virtual XBH_S32 setBootAnimation(const XBH_CHAR* path);
    /**
     * 加载板控文件和属性
     * param[out] boolean. 是否成功
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 loadEdlaCust(XBH_BOOL* enable);
    /**
     * 获取版控版本
     * param[out] strSn 板控版本
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getProjectIdVersion(XBH_CHAR* str);
    /**
     * 设置待机模式
     * @param mode 待机模式
     *           0,      正常关机，系统功耗最低
     *           1,      熄屏模式，只关闭屏幕，声音等
     *           2       STR关机模式
     * @return 执行是否成功
     */
    virtual XBH_S32 setStandbyMode(XBH_U32 u32Mode);
    /**
     * 获取待机模式
     * @return 待机模式
     *           0,      正常关机，系统功耗最低
     *           1,      熄屏模式，只关闭屏幕，声音等
     *           2       STR关机模式
     */
    virtual XBH_S32 getStandbyMode(XBH_U32 *u32Mode);
    /**
     * 关机时设置eywa释放资源和操作设备
     * retval true:success, false:failure
    */
    virtual XBH_S32 setOnStop(XBH_VOID);
    /**
     * 信号源的屏幕下移
     *
     * param[in]
     *           x: 横轴偏移量
     *           y: 纵轴偏移量
     * retval true:success, false:failure
     */
    virtual XBH_S32 setSourceScreenOffset(XBH_S32 x, XBH_S32 y);
    /**
     * 获取信号源屏幕下移偏移量
     */
    virtual XBH_S32 getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y);
    /**
     * state为1时原生系统亮屏的时候会调用此接口，实现eywa客制化定制逻辑
     * state为0时原生系统灭屏的时候会调用此接口，实现eywa客制化定制逻辑
     */
    virtual XBH_S32 setOnResume(XBH_S32 state);
    /**
     * 打开或关闭手动喂硬件看门狗模式。
     *
     * @param bEnable :  true：打开，false：关闭
     * @return boolean 是否成功
     */
    virtual XBH_S32 setWatchDogManualModeEnable(XBH_BOOL bEnable);
    /**
     * 获取手动喂硬件看门狗模式。
     *
     * @param bEnable :  true：打开，false：关闭
     * @return boolean 是否成功
     */
    virtual XBH_S32 getWatchDogManualModeEnable(XBH_BOOL *bEnable);
    /**
     * 打开或关闭硬件看门狗。
     *
     * @param bEnable :  true：打开，false：关闭
     * @return boolean 是否成功
     */
    virtual XBH_S32 setWatchDogEnable(XBH_BOOL bEnable);
    /**
     * 设置硬件看门狗超时时间(单位秒),
     * @param u32TimeOutSec 超时时间
     */
    virtual XBH_S32 setWatchDogTimeOut(XBH_U32 u32TimeOutSec);
    /**
     * 获取硬件看门狗超时时间(单位秒),
     * @return u32TimeOutSec 超时时间
     */
    virtual XBH_S32 getWatchDogTimeOut(XBH_U32 *u32TimeOutSec);
    /**
     * 喂硬件看门狗一次，对看门狗计数进行复位操作。(560不支持，系统上500ms喂狗一次）
     *
     * @return boolean 是否成功
     */
    virtual XBH_S32 watchDogPing(XBH_VOID);
    /**
    * 设置monitorId，唯一ID.用于控制多台设备绑定唯一id，遥控器单独控制设备，与deviceid功能独立
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setMonitorId(XBH_S32 monitorId);
    /**
    * 获取设置monitorId，唯一ID.用于控制多台设备绑定唯一id，遥控器单独控制设备，与deviceid功能独立
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getMonitorId(XBH_S32* monitorId);
    /*
    * 获取eth0 speed
    * param[in] port口序号
    * param[out] net speed获取返回值 
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getEthPortSpeed(XBH_S32 port, XBH_S32* value);
        /**
    * 获取HardwareVer
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHardwareVer(XBH_S32* hardwareVer);
    /**
    * 获取UsbInfo
    * retval 0:success,-1:failure
    */
    virtual std::string getUsbDeviceInfos(const std::string& input);
    /******************************************************************************FACTORY***********************************************************/
    /**
     * 设置当前信源画面的重显率数据
     * param[in] stRect 重显率数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setOverscan(XBH_RECT_S* stRect);
    /**
     * 获取当前信源画面的重显率数据
     * param[out] stRect 重显率数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOverscan(XBH_RECT_S* stRect);
    /**
     * 设置SOC输出的tese pattern
     * param[in] pattern test pattern 序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setTestPattern(XBH_PATTERN_E pattern, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取SOC输出的tese pattern
     * param[out] pattern test pattern 序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getTestPattern(XBH_PATTERN_E *pattern, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置主副屏的屏参序号
     * param[in] enPanel 主副屏类型
     * param[in] s32Value 屏参序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPanelIndex(XBH_S32 s32Value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取主副屏的屏参序号
     * param[in] enPanel 主副屏类型
     * param[out] s32Value 屏参序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPanelIndex(XBH_S32* s32Value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置主副屏的背光PWM频率
     * param[in] enPanel 主副屏类型
     * param[in] u32PwmF PWM频率
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPwmFrequency(XBH_U32 u32PwmF, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取主副屏的背光PWM频率
     * param[in] enPanel 主副屏类型
     * param[out] u32PwmF PWM频率
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPwmFrequency(XBH_U32* u32PwmF, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置主副屏的背光PWM极性翻转
     * param[in] enPanel 主副屏类型
     * param[in] bEnable XBH_TRUE 翻转极性，XBH_FALSE 不翻转极性
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取主副屏的背光PWM极性翻转
     * param[in] enPanel 主副屏类型
     * param[out] bEnable XBH_TRUE 翻转极性，XBH_FALSE 不翻转极性
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置串口debug的开关
     * param[in] bEnable XBH_TRUE 打开串口debug，XBH_FALSE 关闭串口debug
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setDebugEnable(XBH_BOOL bEnable);
    /**
     * 获取串口debug的开关
     * param[out] bEnable XBH_TRUE 打开串口debug，XBH_FALSE 关闭串口debug
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDebugEnable(XBH_BOOL* bEnable);
    /**
     * 写入有线网的MAC地址
     * param[in] macType
     *          0 板载Ethernet
     *          1 OPS
     *          2 SDM
     *          3 Usbc
     * param[in] strMacAddress 有线网的MAC地址
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress);
    /**
     * 获取有线网的MAC地址
     * param[in] macType
     *          0 板载Ethernet
     *          1 OPS
     *          2 SDM
     *          3 Usbc
     * param[out] strMacAddress 有线网的MAC地址
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress);
    /**
     * 写入serial number
     * param[in] value serial number
     * retval true:success, false:failure
    */
    virtual XBH_S32 setSn(const XBH_CHAR* strSn);
    /**
     * 获取serial number
     * param[out] strSn sn码
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSn(XBH_CHAR* strSn);
    /**
    * 写入分区数据
    * param[in] pBuff 要写入的实际数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setCustProductInfo(const XBH_CHAR* pBUf);
    /**
    * 读取分区数据
    * param[in] pBuff 要读取的实际数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCustProductInfo(XBH_CHAR* pBUff);
    /**
    * 获取指定通道的唤醒引脚状态
    * param[in] enSource 要指定通道
    *param[out] bEnable 唤醒信号状态
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsWakeSoc(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    /**
    * 获取指定通道的ops运行状态
    * param[in] enSource 指定通道
    *param[out] bEnable :ture 低电平,false 高电平
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsOperationStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    /**
    * 设置指定通道的recovery控制引脚电平
    * param[in] enSource 指定通道
    *param[in] bEnable :ture 低电平,false 高电平
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setOpsRecovery(XBH_BOOL bEnable, XBH_SOURCE_E enSource);
    /**
     * 写入HDCP
     * param[in] strPath 写入HDCP的文件路径
     * param[in] type    写入HDCP的类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdcpKey(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type);
    /**
     * 获取写入HDCP的状态
     * param[in] type    写入HDCP的类型
     * param[out] bEnable 是否已经写入
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable);
    /**
    * 获取HDCP的值
    * param[out] XBH_CHAR pBuff
    * param[in] type    HDCP的类型
    * param[in] source  信源的类型
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdcpKeydata(XBH_SOURCE_E source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff);
    /**
     * 写入HDCP filename
     * param[in] strPath 写入HDCP的文件路径
     * param[in] type    写入HDCP的类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdcpKeyName(const XBH_CHAR* name, XBH_HDCP_TYPE_E type);
    /**
     * 获取写入HDCP的filename
     * param[in] type    写入HDCP的类型
     * param[out] filename
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* name);
    /**
     * 写入HDMI EDID
     * param[in] strPath 写入edid的文件路径
     * param[in] idx     写入信源序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdmiEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx);
    /**
     * 获取写入hdmi edid的状态
     * param[in] idx     写入信源序号
     * param[out] bEnable 是否已经写入
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmiEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable);
    /**
     * 获取写入HDCP的filename
     * param[in] type    写入HDCP的类型
     * param[out] filename
     *                    0x104: BOARD_LT8668SX_1
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdcpExtendIc(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type, XBH_S32 devType);
    /**
     * 获取写入HDCP的状态
     * param[in] type    写入HDCP的类型
     * param[out] bEnable 是否已经写入
     *                    0x104: BOARD_LT8668SX_1
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdcpExtIcStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable, XBH_S32 devType);
    /**
     * 写入HDCP filename
     * param[in] strPath 写入HDCP的文件路径
     * param[in] type    写入HDCP的类型
     *                    0x104: BOARD_LT8668SX_1
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdcpExtIcKeyName(const XBH_CHAR* name, XBH_S32 type, XBH_S32 devType);
    /**
     * 获取写入HDCP的filename
     * param[in] type    写入HDCP的类型
     * param[out] filename
     *                    0x104: BOARD_LT8668SX_1
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdcpExtIcKeyName(XBH_CHAR* name, XBH_S32 type, XBH_S32 devType);
    /**
     * 写入VGA EDID
     * param[in] strPath 写入edid的文件路径
     * param[in] idx     写入信源序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx);
    /**
     * 获取写入VGA edid的状态
     * param[in] idx     写入信源序号
     * param[out] bEnable 是否已经写入
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable);
    /**
     * 获取写入VGA edid的checksum
     * param[in] idx     写入信源序号
     * param[out] checksum  vga checksum值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum);
    /**
     * 打印写入EDID
    */
    virtual XBH_S32 dumpEdid(XBH_SOURCE_E idx);
    /**
     * 写入google key
     * param[in] strPath 写入google key的文件路径
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGoogleKey(const XBH_CHAR* strPath);
    /**
     * 获取写入Google key的状态
     * param[out] bEnable 是否已经写入
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGoogleKeyStatus(XBH_BOOL* bEnable);
        /**
     * 写入Googlekey filename
     * param[in] strPath 写入Google的文件路径
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGoogleKeyName(const XBH_CHAR* name);
    /**
     * 获取写入Googlekey的filename
     * param[out] filename
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGoogleKeyName(XBH_CHAR* name);
    /**
     * 写入Attention key
     * param[in] strPath 写入attention key的文件路径
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAttentionKey(const XBH_CHAR* strPath);
    /**
     * 获取写入Attention key的状态
     * param[out] bEnable 是否已经写入
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAttentionKeyStatus(XBH_BOOL* bEnable);

    /**
     * 获取RKP crs
     * param[out] read_buffer json格式字符串
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getRkpCsrInfo(XBH_CHAR* read_buffer, int buffer_size);

    /**
     * setAgingModeState
     * @param 
     * @return 
     */
    virtual XBH_S32 setAgingModeState(XBH_S32 u32Value);
    /**
     * getAgingModeState
     * @return 
     */
    virtual XBH_S32 getAgingModeState(XBH_S32 *u32Value);

    /**
     * 升级U盘固件
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 upgradeSystemFirmware(XBH_BOOL* bEnable);
    /**
    * set device id, 广告机部分客户的需求
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setDeviceId(const XBH_CHAR* strDeviceId);
    /**
    * get device id, 广告机部分客户的需求
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDeviceId(XBH_CHAR* strDeviceId);
    /**
    * set usb config, edla认证测试需求 0:usbc source 1:otg
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setUsbcConfig(XBH_S32 type);
    /**
    * get usb config, edla认证测试需求0:usbc source 1:otg
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbcConfig(XBH_S32* type);
    /**
     * 获取OTG端口是HOST模式还是DEVICE模式, TYPE-B以及TYPE-C in/out端子均可连接SOC OTG，因方案各异
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHostOrDeviceStatus(XBH_S32* type);
    /**
     * 设置OTG端口是HOST模式还是DEVICE模式, TYPE-B以及TYPE-C in/out端子均可连接SOC OTG，因方案各异
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHostOrDeviceStatus(XBH_S32 type);
    /**
     * setMokaPModeEnableToCusdata
     * @param 
     * @return 
     */
    virtual XBH_S32 setMokaPModeEnableToCusdata(XBH_S32 u32Value);
    /**
     * getMokaPModeEnableFromCusdata
     * @return 
     */
    virtual XBH_S32 getMokaPModeEnableFromCusdata(XBH_S32 *u32Value);
    /**
     * getUSBCForwardReverseInsertionDet
     * @return 
     */
     virtual XBH_S32 getUSBCForwardReverseInsertionDet(XBH_SOURCE_E idx, XBH_S32 *u32Value);

     /**
     * setMultiUser
     * @return
     */
     virtual XBH_S32 setMultiUser(const XBH_CHAR* status);

     /**
     * getMultiUser
     * @return
     */
     virtual XBH_S32 getMultiUser(XBH_CHAR* status);

     /**
     * setVSPage
     * @return
     */
     virtual XBH_S32 setVSPage(const XBH_CHAR* status);

     /**
     * getVSPage
     * @return
     */
     virtual XBH_S32 getVSPage(XBH_CHAR* status);

     /**
     * setCustomSKU
     * @return
     */
     virtual XBH_S32 setCustomSKU(const XBH_CHAR* status);

     /**
     * getCustomSKU
     * @return
     */
     virtual XBH_S32 getCustomSKU(XBH_CHAR* status);

     /**
     * setBootMode
     * @return
     */
     virtual XBH_S32 setBootMode(const XBH_CHAR* status);

     /**
     * getBootMode
     * @return
     */
     virtual XBH_S32 getBootMode(XBH_CHAR* status);

    /**
    * 写入HISI的HVB的key信息，并使能安全芯片
    * 只支持一次写入，写入后无法撤消
    * 使用能安全芯片后，无法退回，key不对的软件将无法升级
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSecurityHvbKey();

    /**
    * 获取hisi芯片的安全芯片使能状态
    * param[out] u32Status 使用能状态 0：未使用，1：已使能
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSecurityHvbKeyStatus(XBH_U32 *u32Status);

    /******************************************************************************DEVICE***********************************************************/
    /**
     * 设置IO输出的电平
     * param[in] u32GpioNumber IO引脚的序号
     * param[in] u32GpioValue 电平值 0：低电平，1：高电平
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    /**
     * 获取IO输出的电平
     * param[in] u32GpioNumber IO引脚的序号
     * param[out] u32GpioValue 电平值 0：低电平，1：高电平
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue);
    /**
     * 获取IO输入的电平
     * param[in] u32GpioNumber IO引脚的序号
     * param[out] u32GpioValue 电平值 0：低电平，1：高电平
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue);
    /**
     * 设置RTC的数据
     * param[in] RTC的时间数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setRtcTime(XBH_RTC_INFO_S* stRtcInfo);
    /**
     * 获取RTC的数据
     * param[out] RTC的时间数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getRtcTime(XBH_RTC_INFO_S* stRtcInfo);
    /**
     * 设置RTC的数据
     * param[in] RTC的时间数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setRtcAlarmTime(XBH_RTC_ALARM_INFO_S* stRtcAlarmInfo);
    /**
     * 设置I2c Bus的lock状态，当需要对IC进行升级时，可以先锁定IIC防止操作异常
     * param[in] iicBus 需要锁住的iic bus
     * param[in] lock 是否要锁住
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setIICLock(XBH_U32 iicBus, XBH_BOOL lock);
    /**
     * 通过IIC写入数据
     * param[in] u32I2cNum iic bus
     * param[in] u8DevAddress 设备地址
     * param[in] u32RegAddr 设备寄存器地址
     * param[in] u32RegAddrCount 设备寄存器长度
     * param[in] u32Length 写入的数据长度
     * param[in] u8Data 写入的数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    /**
     * 通过IIC读取数据
     * param[in] u32I2cNum iic bus
     * param[in] u8DevAddress 设备地址
     * param[in] u32RegAddr 设备寄存器地址
     * param[in] u32RegAddrCount 设备寄存器长度
     * param[in] u32Length 读取的数据长度
     * param[out] u8Data 读取的数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    /**
     * 设置屏幕背光开关（不推荐使用）
     * param[in] bEnable. XBH_TRUE：打开背光，XBH_FALSE：关闭背光
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBacklightEnable(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕背光开关（不推荐使用）
     * param[out] bEnable. XBH_TRUE：打开背光，XBH_FALSE：关闭背光
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBacklightEnable(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置屏幕开关，跟开关背光的区别是，设置屏幕开关会在控制背光的基础上同时控制TCON的供电，并且之间有时序控制
     * 只关闭背光，有些屏幕还能看到屏幕上面有很暗的画面。非特殊屏幕，推荐在关闭屏幕时采用setPanelPower接口
     * param[in] bEnable. XBH_TRUE：点亮屏幕，XBH_FALSE：熄灭屏幕
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕开关
     * param[out] bEnable. XBH_TRUE：点亮屏幕，XBH_FALSE：熄灭屏幕
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置指示灯的亮灯状态
     * param[in] enState. LED指示灯的亮灯状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    /**
     * 获取指示灯的亮灯状态
     * param[out] enState. LED指示灯的亮灯状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getLedPwrStatus(XBH_LED_LIGHT_E* enState);
    /**
     * 获取有线网络插入状态
     * param[out] bEnable. 有线网插入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getEthPlugStatus(XBH_BOOL* bEnable);
    /**
     * 根据ADC通路获取数值
     * param[in] enChannel. ADC通路序号
     * param[out] bEnable. 有线网插入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32* u32Value);

    /**
     * 环境光传感器是否存在
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getLightSensorExist(XBH_BOOL* bEnable);

    /**
     * 人体红外传感器是否存在，Passive Infrared Sensor
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getPirSensorExist(XBH_BOOL* bEnable);

    /**
     * 霍尔传感器是否存在
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);

    /**
     * 空气传感器是否存在
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getAirSensorExist(XBH_BOOL* bEnable);

    /**
     * 主板温度传感器是否存在
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getTemperatureSensorExist(XBH_BOOL* bEnable);

    /**
     * 重力传感器是否存在
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getGSensorExist(XBH_BOOL* bEnable);

    /**
     * 获取光线传感器的数据
     * param[out] s32Value 光线传感器的数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getLightSensorValue(XBH_S32* s32Value);

    /**
     * 获取距离传感器原始数据
     * param[out] s32Value 传感器的数据，人体靠近触发
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getPirSensorValue(XBH_S32* s32Value);

    /**
     * 功率传感器是否存在
     * param[out] bEnable. 是否存在
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getPowerSensorExist(XBH_BOOL* bEnable);

    /**
     * 获取功率传感器的数据
     * param[out] s32Value 功率传感器的数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPowerSensorValue(XBH_FLOAT* s32Value);

    /**
     * 获取霍尔传感器的组合电平值（PenA + PenB）
     * param[out] s32Value 返回两笔电平状态的叠加值（2: 两只笔都在面板上 1：一只笔在面板上  0: 没有笔在面板上）
     * retval 0:success,-1:failure
   */
    virtual XBH_S32 getHallSensorValue(XBH_S32* s32Value);

    /**
     * 获取霍尔传感器的笔状态编码值（PenA + PenB = 16进制）
     * param[out] s32Value 编码值 (低字节bit0: PenA状态（0:抬起, 1:接触） 高字节bit8: PenB状态（0:抬起, 1:接触）)
     * retval 0:success,-1:failure
   */
    virtual XBH_S32 getHallSensorPen(XBH_S32* s32Value);

    /**
     * 获取空气传感器原始数据
     * param[out] fHumidity    环境湿度
     * param[out] fTemperature 环境温度
     * param[out] fTvoc        环境TVOC值
     * param[out] fPm2p5       环境pm2.5
     * param[out] fPm1p0       环境pm1.0
     * param[out] fPm10        环境pm10
     * param[out] fCo2         环境Co2
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2);


    /**
      * 获取NTC温度
     * param[in] s32Value 0:主板NTC 1:前置板NTC,etc
     * param[out] fValue 传感器的数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getNTCValue(XBH_S32 s32Value, XBH_FLOAT* fValue);
    /**
     * 获取主板的工作温度
     * param[out] s32Value 传感器的数据
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getTemperatureSensorValue(XBH_FLOAT* fValue);

    /**
     * 获取重力传感器原始数据
     * param[out] s32Value 传感器的数据
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue);
    /**
     * 升级扩展IC
     * @param filename  升级文件绝对路径
     * @param force     是否强制升级
     * @param devType   对应设备类型
     *                    0x000: FRONT_DEV6563_1
     *                    0x001: FRONT_DEV6563_2
     *                    0x002: FRONT_DEV6563_3
     *                    0x003: FRONT_DEV6563_3
     *                    0x004: BOARD_DEV6563_1
     *                    0x005: BOARD_DEV6563_2
     *                    0x006: BOARD_DEV6563_3
     *                    0x007: BOARD_DEV6563_4
     *                    0x008: BOARD_GSV2705_1
     *                    0x009: BOARD_GSV2705_2
     *                    0x00a: BOARD_GSV2715_1
     *                    0x00b: BOARD_GSV2715_2
     *                    0x100: BOARD_LT8711
     *                    0x101: FRONT_LT8711
     *                    0x102: BOARD_LT8711UXE1
     *                    0x103: FRONT_LT8711UXE1
     *                    0x200: ITE68411
     *                    0x300: FRONT_VL105
     *                    0x301: BOARD_VL105
     *                    0x400: MCU
     *                    0x500: AUDIO_CODEC
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);

    /**
     * 扩展IC 是否正在升级状态
     *
     * @param devType   对应设备类型
     *                    0x000: FRONT_DEV6563_1
     *                    0x001: FRONT_DEV6563_2
     *                    0x002: FRONT_DEV6563_3
     *                    0x003: FRONT_DEV6563_3
     *                    0x004: BOARD_DEV6563_1
     *                    0x005: BOARD_DEV6563_2
     *                    0x006: BOARD_DEV6563_3
     *                    0x007: BOARD_DEV6563_4
     *                    0x008: BOARD_GSV2705_1
     *                    0x009: BOARD_GSV2705_2
     *                    0x00a: BOARD_GSV2715_1
     *                    0x00b: BOARD_GSV2715_2
     *                    0x100: BOARD_LT8711
     *                    0x101: FRONT_LT8711
     *                    0x102: BOARD_LT8711UXE1
     *                    0x103: FRONT_LT8711UXE1
     *                    0x200: ITE68411
     *                    0x300: FRONT_VL105
     *                    0x301: BOARD_VL105
     *                    0x400: MCU
     *                    0x500: AUDIO_CODEC
     * @return 0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
     */
    virtual XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);

    /**
     * 设置 HDMITX使用的密钥版本
     *
     * @param hdcpTxEncryptionMode   密钥版本
     *                    0: XBH_HDCPTX_ENCRYPTION_1P4
     *                    1: XBH_HDCPTX_ENCRYPTION_2P2
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 setHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E hdcpTxEncryptionMode);
    /**
     * 获取 HDMITX使用的密钥版本
     *
     * retval hdcpTxEncryptionMode 0: 1.4  2: 2.2
     */
    virtual XBH_S32 getHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E *hdcpTxEncryptionMode);

    /**
     * 获取扩展IC当前版本
     *
     * @param devType   对应设备类型
     *                    0x000: FRONT_DEV6563_1
     *                    0x001: FRONT_DEV6563_2
     *                    0x002: FRONT_DEV6563_3
     *                    0x003: FRONT_DEV6563_3
     *                    0x004: BOARD_DEV6563_1
     *                    0x005: BOARD_DEV6563_2
     *                    0x006: BOARD_DEV6563_3
     *                    0x007: BOARD_DEV6563_4
     *                    0x100: BOARD_LT8711
     *                    0x101: FRONT_LT8711
     *                    0x102: BOARD_LT8711UXE1
     *                    0x103: FRONT_LT8711UXE1
     *                    0x200: ITE68411
     *                    0x300: FRONT_VL105
     *                    0x301: BOARD_VL105
     *                    0x400: MCU
     *                    0x500: AUDIO_CODEC
     * @return 扩展IC当前版本
     */
    virtual XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);

    /**
     * 设置DDR的展频
     * param[in] ddrIndex. ddr的序号
     * param[in] sscInfo.  展频信息
     * retval boolean 设置是否成功
    */
    virtual XBH_S32 setDdrSSC(XBH_SSC_INFO_S sscInfo, XBH_S32 ddrIndex);

    /**
     * 获取DDR的展频
     * param[in] ddrIndex. ddr的序号
     * retval sscInfo.  展频信息
    */
    virtual XBH_S32 getDdrSSC(XBH_SSC_INFO_S* sscInfo, XBH_S32 ddrIndex);

    /**
     * 设置屏幕展频
     * param[in] panelIdx. 屏幕序号
     * param[in] sscInfo.  展频信息
     * retval boolean 设置是否成功
    */
    virtual XBH_S32 setPanelSSC(XBH_SSC_INFO_S sscInfo, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕的展频
     * param[in] panelIdx. 屏幕的序号
     * retval sscInfo.  展频信息
    */
    virtual XBH_S32 getPanelSSC(XBH_SSC_INFO_S* sscInfo, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕的名称
     * param[in] panelIdx. 屏幕的序号
     * retval String.  屏幕名称
    */
    virtual XBH_S32 getPanelName(XBH_CHAR* name, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕的物理分辨率
     * param[in] panelIdx. 屏幕的序号
     * retval String.  屏幕物理分辨率字符串，例如 3840x2160
    */
    virtual XBH_S32 getPanelResolution(XBH_CHAR* resolution, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置屏幕的选择角度，通常调用framework里面的接口实现UI的旋转，原厂的接口实现视频的旋转
     * param[in] value. 屏幕的旋转角度value 0,1,2,3 分别表示旋转 0, 90, 180, 270
     * param[in] panelIdx. 屏幕的序号
     * retval boolean. 设置是否成功
    */
    virtual XBH_S32 setPanelRotation(XBH_S32 value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);

    /**
     * 获取屏幕的旋转方向
     * param[in] panelIdx. 屏幕的序号
     * retval int. 屏幕的旋转方向
    */
    virtual XBH_S32 getPanelRotation(XBH_S32* value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置屏幕单分屏还是双分屏
     * param[in] type. 0: 单分屏       1: 双分屏
     * param[in] panelIdx. 屏幕的序号
     * retval boolean. 设置是否成功
    */
    virtual XBH_S32 setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕是单分屏还是双分ping
     * param[in] panelIdx. 屏幕的序号
     * retval int. 分屏类型
    */
    virtual XBH_S32 getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 设置Hdmi tx vedio mute状态
     * param[in] type. 0: unmute video       1: mute video
     * retval boolean. 设置是否成功
    */
    virtual XBH_S32 setHdmiTxVideoMuteState(XBH_BOOL state);
    /**
     * 获取Hdmi tx vedio mute状态
     * param[in] type. 0: unmute video       1: mute video
     * retval boolean. 设置是否成功
    */
    virtual XBH_S32 getHdmiTxVideoMuteState(XBH_BOOL *state);
    /**
     * 设置屏参的奇偶和map参数
     * param[in] type. 111 第一位代表是否启用，第二位代表奇偶，第三位表示mapp 
     * retval boolean. 设置是否成功
    */
    virtual XBH_S32 setPanelMapSwap(XBH_S32 type);
    /**
    * 获取屏参的奇偶和map参数
    * param[in] type. 111 第一位代表是否启用，第二位代表奇偶，第三位表示mapp 
    * retval int. 屏参参数
    */
    virtual XBH_S32 getPanelMapSwap(XBH_S32* type);
    /**
     * 设置屏幕单的镜像模式
     * param[in] value. true：镜像     false：非镜像
     * param[in] panelIdx. 屏幕的序号
     * retval boolean. 设置是否成功
    */
    virtual XBH_S32 setPanelFlipMirror(XBH_BOOL value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    /**
     * 获取屏幕的镜像模式
     * param[in] panelIdx. 屏幕的序号
     * retval boolean. 镜像模式
    */
    virtual XBH_S32 getPanelFlipMirror(XBH_BOOL* value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);

    /**
     * 获取OPS/SDM是否被硬件锁牢（硬件螺丝）
     * param[in] srcIdx. OPS/SDM的通道序号
     * retval boolean. 是否锁定
    */
    virtual XBH_S32 getOpsLockStatus(XBH_BOOL* value, XBH_SOURCE_E srcIdx = XBH_SOURCE_OPS1);
    /**
     * 设置屏幕是否开启像素移动
     * param[in] enable. 开关像素移动
     * retval boolean. 是否成功
    */
    virtual XBH_S32 setPixelShiftEnable(XBH_BOOL value);
    /**
     * 获取屏幕是否开启像素移动
     * retval boolean. 是否开启
    */
    virtual XBH_S32 getPixelShiftEnable(XBH_BOOL* value);
    /**
     * 设置图形的亮度
     * param[in] value. 亮度值
     * retval boolean. 是否成功
    */
    virtual XBH_S32 setGraphicBrightness(XBH_S32 value);
    /**
     * 获取图形的亮度
     * retval int. 亮度
    */
    virtual XBH_S32 getGraphicBrightness(XBH_S32* value);
    /**
     * 设置图形的对比度
     * param[in] value. 对比度值
     * retval boolean. 是否成功
    */
    virtual XBH_S32 setGraphicContrast(XBH_S32 value);
    /**
     * 获取图形的对比度
     * retval int. 对比度
    */
    virtual XBH_S32 getGraphicContrast(XBH_S32* value);
    /**
     * 设置图形的饱和度
     * param[in] value. 饱和度值
     * retval boolean. 是否成功
    */
    virtual XBH_S32 setGraphicSaturation(XBH_S32 value);
    /**
     * 获取图形的饱和度
     * retval int. 饱和度
    */
    virtual XBH_S32 getGraphicSaturation(XBH_S32* value);
    /**
     * 设置图形的色温模式
     * param[in] value. 色温模式值
     * retval boolean. 是否成功
    */
    virtual XBH_S32 setGraphicColorTempMode(XBH_COLORTEMP_E value);
    /**
     * 获取图形的色温模式
     * retval XbhColorTempMode. 色温模式
    */
    virtual XBH_S32 getGraphicColorTempMode(XBH_COLORTEMP_E* value);
    /**
     * 设置是否开机能源之星
     * param[in] enable. 开关
     * retval boolean. 是否成功
    */
    virtual XBH_S32 setEnergyStarEnable(XBH_BOOL enable);
    /**
     * 获取是否开机能源之星
     * retval boolean. 是否开关
    */
    virtual XBH_S32 getEnergyStarEnable(XBH_BOOL* enable);
    /**
     * 设置Adc rgb的增益和偏移值（VGA,YPBPR）
     * param[in] srcIdx. 信源序号
     * param[in] offset. 增益和偏移值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setAdcGainOffset(XBH_SOURCE_E srcIdx, XBH_GAIN_OFFSET_DATA_S offset);
    /**
     * 获取Adc rgb的增益和偏移值（VGA,YPBPR）
     * param[in] srcIdx. 信源序号
     * retval XbhGainOffsetItem. 增益和偏移值
    */
    virtual XBH_S32 getAdcGainOffset(XBH_SOURCE_E srcIdx, XBH_GAIN_OFFSET_DATA_S* offset);
    /**
     * 设置USB锁定类型
     * param[in] u32UsbType. 锁定类型，0 - 不锁定， 1 - 锁定前置USB， 2, - 锁定侧边USB，3 - 锁定全部
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setUsbLockMode(XBH_U32 u32UsbType);
    /**
     * 获取USB锁定类型
     * param[in] u32UsbType 当前锁定类型
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbLockMode(XBH_U32 *u32UsbType);
    /**
     * 获取OPS的供电是否处于电流过大的状态
     * param[in] srcIdx. 信号源ID
     * param[out] boolean. 是否成功
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOpsOvercurrentStatus(XBH_U32 srcIdx, XBH_BOOL* enable);
    /**
     * 根据OPS过流状态设置前置type-c功率
     * 部分机型的OPS跟type-c使用的是同一路供电，为了保证OPS的正常运行，需要限制typec对外的充电功率
     * param[in] value. 功率值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value);
    /**
    * 获取nfc设备是否存在
    * retval boolean. true 存在， false 不存在
    */
    virtual XBH_S32 isNfcExist(XBH_BOOL *enable);
    /**
    * 使能nfc
    * param[in] enable. true 打开， false 关闭
    * retval boolean. 是否成功
    */
    virtual XBH_S32 setNfcEnable(XBH_BOOL enable);
    /**
    * 获取nfc使能状态
    * retval boolean. true 打开， false 关闭
    */
    virtual XBH_S32 getNfcEnable(XBH_BOOL *enable);
    /**
    * 获取nfc id信息
    * retval XbhNfcIdInfo. ID信息和长度
    */
    virtual XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    /**
    * 添加nfc id信息
    * retval boolean. 是否成功
    */
    virtual XBH_S32 addNfcId(XBH_U8 *data, XBH_U32 length);
    /**
    * 删除nfc id信息
    * retval boolean. 是否成功
    */
    virtual XBH_S32 delNfcId(XBH_U8 *data, XBH_U32 length);
    /**
    * 删除所有nfc id信息
    * retval boolean. 是否成功
    */
    virtual XBH_S32 delAllNfcId();
   /**
     * 设置背光最大值
     * param[in] state. 背光反向的最大值
     * retval true:success, false:failure
    */
    virtual XBH_S32 setBrightMax(XBH_U32 u32Value);
    /**
     * 获取背光最大值
     * retval boolean. 背光最大值状态
    */
    virtual XBH_S32 getBrightMax(XBH_U32 *u32Value);
   /**
     * 设置背光最小值
     * param[in] state. 背光反向的最小值
     * retval true:success, false:failure
    */
    virtual XBH_S32 setBrightMin(XBH_U32 u32Value);
    /**
     * 获取背光最小值
     * retval boolean. 背光最小值状态
    */
    virtual XBH_S32 getBrightMin(XBH_U32 *u32Value);
        /**
    * 设置MCUfatt模式
    */
    virtual XBH_S32 setMcuFattMode(XBH_S32 mode);
    /**
    * 获取MCU fatt模式
    */
    virtual XBH_S32 getMcuFattMode(XBH_S32 *mode);
    /**
    * mcu 外置iic数据转发
    */
    virtual XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    /**
    * mcu 外置iic数据获取
    */
    virtual XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    /**
    * 使能/禁用keypad
    */
    virtual XBH_S32 setFattKeypadenbale(XBH_S32 keypad_enable);
    /**
    * 获取keypad信息
    */
    virtual XBH_S32 getFattKeypadInfo(XBH_S32* keypad_enable, XBH_S32* key_count, XBH_S32* key_index);
    /**
    * mcu 外置uart配置
    */
    virtual XBH_S32 setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud);
    /**
    * mcu 外置uart数据转发
    */
    virtual XBH_S32 setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    /**
    * mcu 外置uart数据接收
    */
    virtual XBH_S32 getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    /**
     * 写入Project ID
     * param[in] value Project ID
     * retval true:success, false:failure
    */
    virtual XBH_S32 setProjectId(const XBH_CHAR* strProjectId);
    /**
     * 获取Project ID
     * param[out] strProjectId Project ID
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getProjectId(XBH_CHAR* strProjectId);
    /**
     * Tcon检测中获取mcu传过来的数值
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getTCONValue(XBH_S32* value);
    /**
     * 设置升降支架
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setLiftbracketOperate(XBH_S32 value);
    /**
     * 获取升降支架是否有节点
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getLiftbracketExist(XBH_S32* value);
    /**
     * 通过iic设置背光
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setBacklightData(XBH_U32 data);
    /**
    * 使用GPIO的名字来设置GPIO的方向和值
    * param[in] gpioName. gpio名字
    * param[in] gpioDirection. gpio方向 1:输出 2.输入
    * param[in] u32GpioValue. gpio输出值
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32 gpioDirection, XBH_U32 u32GpioValue);
    /**
    * 使用GPIO的名字来设获取GPIO的值
    * param[in] gpioName. gpio名字
    * param[out] u32GpioValue. gpio获取返回值 
    * retval 0:success,-1:failure
    */    
    virtual XBH_S32 getGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32* u32GpioValue);
    /**
     * 设置电源灯亮使能
     * true时表示开机的时候要亮灯，待机的时候也亮灯；false的时候表示开机的时候要灭灯，待机的时候也要灭灯
     * param[in] bEnable. XBH_TRUE：打开，XBH_FALSE：关闭
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setPowerLedLightEnable(XBH_BOOL bEnable);
    /**
     * 获取电源灯亮使能
     * true时表示开机的时候要亮灯，待机的时候也亮灯；false的时候表示开机的时候要灭灯，待机的时候也要灭灯
     * param[out] bEnable. XBH_TRUE：打开，XBH_FALSE：关闭
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getPowerLedLightEnable(XBH_BOOL* bEnable);
    /**
     * 设置关机操作标志
     * 当执行关机操作时，设置为true；当没有执行关机操作时，设置为false
     * param[in] enable. XBH_TRUE：表示执行了关机操作，XBH_FALSE：表示没有执行关机操作
     * retval 0: success, -1: failure
     */
    virtual XBH_S32 setShutdownFlag(XBH_BOOL enable);
    /**
    * 设置动态功率
    * param[in] enSource 指定的信源通道
    * param[in] pdability 充电瓦数
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);

     /**
     * 设置wifi频道代码
     * param[in] wifi_type wifi模组类型
     * param[in] channel wifi频道代码
     * retval 0:success,-1:failure
     */
    virtual XBH_S32 setWifiChanelPlan(XBH_WIFI_TYPE wifiType, const XBH_CHAR* channel);

     /**
     * 写入SFT工厂数据
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSftData(const XBH_CHAR* data);

    /**
     * 获取SFT工厂数据
     * param[out] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSftData(XBH_CHAR* data);

     /**
     * 工厂需要存储产品的 RkpStatus
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setRkpStatus(const XBH_CHAR* data);

    /**
    * 工厂需要存储产品的 RkpStatus
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getRkpStatus(XBH_CHAR* data);

    /**
    * 板控升级前的准备工作 prepareIncreaseBinUpgrade
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 prepareIncreaseBinUpgrade();

    /**
     * smart工厂需要存储产品的PN码，一个系列对应一个PN码
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSmartPnData(const XBH_CHAR* data);

    /**
    * smart工厂需要存储产品的PN码，一个系列对应一个PN码
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSmartPnData(XBH_CHAR* data);

    /**
     * smart工厂需要存储产品的SN码，一个系列对应一个SN码
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSmartSnData(const XBH_CHAR* data);

    /**
    * smart工厂需要存储产品的SN码，一个系列对应一个SN码
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSmartSnData(XBH_CHAR* data);

    /**
     * SMART希望罗盘关闭的状态能永久保存，复位不被清除
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setCompassPresist(const XBH_CHAR* data);

    /**
    * SMART希望罗盘关闭的状态能永久保存，复位不被清除
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCompassPresist(XBH_CHAR* data);

    /**
     * smart工厂需要存储底板的PN码，一个系列对应一个PN码
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSmartScalerPnData(const XBH_CHAR* data);

    /**
    * smart工厂需要存储底板的PN码，一个系列对应一个PN码
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSmartScalerPnData(XBH_CHAR* data);

    /**
     * smart工厂需要存储底板的SN码，一个系列对应一个SN码
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSmartScalerSnData(const XBH_CHAR* data);

    /**
    * smart工厂需要存储底板的SN码，一个系列对应一个SN码
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSmartScalerSnData(XBH_CHAR* data);

    /**
     * smart工厂需要存储核心板的PN码，一个系列对应一个PN码
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSmartCorePnData(const XBH_CHAR* data);

    /**
    * smart工厂需要存储核心板的PN码，一个系列对应一个PN码
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSmartCorePnData(XBH_CHAR* data);

    /**
     * smart工厂需要存储核心板的SN码，一个系列对应一个SN码
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSmartCoreSnData(const XBH_CHAR* data);

    /**
    * smart工厂需要存储核心板的SN码，一个系列对应一个SN码
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSmartCoreSnData(XBH_CHAR* data);

    /**
    * 获取天线的信号强度
    * param[out] ver 数据, 数据格式如下：
    * point[0]=-49, point[1]=-36 
    * 如果是多根天线, 返回实际天线数量
    * point[0]=-49, point[1]=-36, point[2]=-49, point[3]=-36 
    * 范围: 0~-110db
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getAntennaRssi(XBH_CHAR* ver);

   /**
     * 设置工厂MAC地址
     * param[in] macType 地址类型
     * param[in] data 数据
     * retval 0:success,-1:failure
    */
   virtual XBH_S32 saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType, const XBH_CHAR* data);

   /**
    * 获取工厂MAC地址
    * param[out] macType 地址类型
    * param[out] data 数据
    * retval 0:success,-1:failure
    */
   virtual XBH_S32 getFactoryMac(XBH_MAC_ADDRESS_TYPE macType, XBH_CHAR* data);

   /**
    * 获取GoogleKey/Attestation/Device_ID序列号
    * param[out] deviceID 数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getGoogleKeyID(XBH_CHAR* deviceID);

    /**
     * 设置工厂SN
     * param[in] materialType 物料类型  0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
     * param[in] snInfo 数据
     * retval 0:success,-1:failure
    */
   virtual XBH_S32 burnFactorySN(XBH_MATERAL_TYPE materialType, const XBH_CHAR* snInfo);

   /**
    * 获取工厂SN
    * param[out] materialType 物料类型  0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
    * param[out] snInfo 数据
    * retval 0:success,-1:failure
    */
   virtual XBH_S32 getBurnFactorySN(XBH_MATERAL_TYPE materialType, XBH_CHAR* snInfo);

   /**
    * 设置5450芯片充电功率
    * pdability:充电功率
    * enSource 信号源
    * retval 0:success,-1:failure
    */
   virtual XBH_S32 setTypeCPdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);
    /**
    * 设置单独每一路信源的EDID类型
    * param[in] enType EDID类型，
    *           u32SourceId 信源的ID
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId);

    /**
    * 获取单独每一路信源的EDID类型
    * param[in] u32SourceId 信源的ID
    * param[out] enType EDID类型
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSourceEdidType(XBH_U32 u32SourceId, XBH_HDMIRX_EDID_TYPE_E *enType);

    /**
    * 写入分区数据
    * param[in] pBuff 要写入的实际数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setFlashEepData(std::string pBuff);

    /**
    * 读取分区数据
    * param[in] pBuff 要读取的实际数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFlashEepData(std::string &pBuff);
/**********************************************************************************************OPS Bypass************************************************************************************************/
    /**
    * 获取OPS回传的数据情况
    * param[out] OPS回传数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOPSBypassDataLen(XBH_U8* u8Data);

    /**
    * OPS透传数据
    * param[in] OPS类型
    * param[in] 数据长度
    * param[out] OPS回传数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 SendOPSBypassData(XBH_SOURCE_E type, XBH_U8 u8Len, XBH_U8* u8Data);

    /*
    * 获取edid机型名
    * param[in] pBuff 要读取的实际数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getEdidModelName(std::string &pBuff);

    /*
    * 获取edid序列号
    * param[in] serialArr 序列号buff
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getEdidSn(std::string &pBuff);
    /**
    * 将作为参考的RTC数据保存到不可擦除分区
    * param[in] RTC数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setReferRTCInfo(const XBH_CHAR* pBuff);
    /**
    * 从不可擦除分区读取作为参考的RTC数据
    * param[out] 回传数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getReferRTCInfo(XBH_CHAR* pBuff);

    /**
    * 通过读取节点获取KTC触摸框必要信息绕过EDLA权限问题
    * param[out] 回传数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getKtcTouchInfo(XBH_CHAR* pBuff);
    /**
    * 通过去读取lic_status去获取授权的状态
    * param[out] 回传数据
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getMicLicenseState(XBH_S32* status);

    /**
     * 获取EMMC版本
     * @param pu32Version [out] 返回EMMC版本号(16进制)
     * @return XBH_SUCCESS 成功，XBH_FAILURE 失败
     */
    virtual XBH_S32 getEmmcVersion(XBH_U32 *pu32Version);

    /**
     * 获取EMMC寿命（返回MLC和SLC中的最大值）
     * @param pu32LifeTime [out] 返回EMMC寿命值(16进制)
     * @return XBH_SUCCESS 成功，XBH_FAILURE 失败
     */
    virtual XBH_S32 getEmmcLifeTime(XBH_U32 *pu32LifeTime);

    /*
    * 升级扩展IC
    * @param data      dataflow
    * @param force     是否强制升级
    * @param devType   对应设备类型    
    */
    virtual XBH_S32 upgradeExtendIcByData(XBH_U8 *data, XBH_U32 dataLen,XBH_BOOL force, XBH_U32 devType);
public:
    XbhPlatformInterface(){};
    virtual ~XbhPlatformInterface(){};

    XBH_S32 mLockIIC[8];
};

#endif //XBH_PLATFORM_INTERFACE_H
