#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPlatformInterface"

#include "XbhLog.h"
#include "XbhPlatformInterface.h"
#include "XbhPartitionData.h"

/******************************************************************************PICTURE***********************************************************/
/**
 * 设置图像模式
 * param[in] enPicMode. 图像模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPictureMode(XBH_PICMODE_E enPicMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像模式
 * param[out] enPicMode. 图像模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPictureMode(XBH_PICMODE_E* enPicMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像亮度
 * param[in] u32Value. 图像亮度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setBrightness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像亮度
 * param[out] u32Value. 图像亮度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getBrightness(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像对比度值
 * param[in] u32Value. 图像对比度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setContrast(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像对比度值
 * param[out] u32Value. 图像对比度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getContrast(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像饱和度值
 * param[in] u32Value. 图像饱和度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSaturation(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像饱和度值
 * param[out] u32Value. 图像饱和度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSaturation(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像锐度调节开关
 * param[in] u32Value. 0 关闭锐度调节，1 打开锐度调节
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSharpEnable(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像锐度值
 * param[in] u32Value. 图像锐度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSharpness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像锐度值
 * param[out] u32Value. 图像锐度值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSharpness(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像色调值
 * param[in] u32Value. 图像色调值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHue(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像色调值
 * param[out] u32Value. 图像色调值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHue(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像背光值
 * param[in] u32Value. 图像背光值，范围0~100
 * param[in] enPanel. 屏幕序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setBacklight(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像背光值
 * param[in] enPanel. 屏幕序号
 * param[out] u32Value. 图像背光值，范围0~100
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getBacklight(XBH_U32* u32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置临时图像背光值，数值不保存
 * param[in] u32Value. 图像背光值，范围0~100
 * param[in] enPanel. 屏幕序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setBacklightWithOutSave(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement V1");
    return  s32Ret;
}

/**
* 写入分区数据
* param[in] pBuff 要写入的实际数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setFlashEepData(std::string pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 读取分区数据
* param[in] pBuff 要读取的实际数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getFlashEepData(std::string &pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取背光不保存值的值,也是实际正式生效的PWM对应的背光值
 * param[in] enPanel. 屏幕序号
 * retval u32Value. 图像背光值，范围0~100
*/
XBH_S32 XbhPlatformInterface::getBacklightWithOutSave(XBH_U32* u32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 设置图像色温模式
 * param[in] enColorTemp. 色温模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setColorTempMode(XBH_COLORTEMP_E enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像色温模式
 * param[out] enColorTemp. 色温模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getColorTempMode(XBH_COLORTEMP_E *enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像降噪等级
 * param[in] enNrLevel. 降噪等级
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setNR(XBH_LEVEL_E enNrLevel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像降噪等级
 * param[out] enNrLevel. 降噪等级
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getNR(XBH_LEVEL_E *enNrLevel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像的游戏模式开关
 * param[in] bEnable. XBH_TRUE：打开游戏模式，XBH_FALSE：关闭游戏模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGameModeEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的游戏模式开关
 * param[out] bEnable. XBH_TRUE：打开游戏模式，XBH_FALSE：关闭游戏模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGameModeEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像的PC模式，有的方案又叫Graphic模式，与之对应的是Video模式，通常接入的是RGB信号时打开PC模式，
 * 反之就是video模式，默认为系统自动判断。PC模式时图像重显率为100%，并且降低了锐度和关闭了部分PQ效果，
 * 在接PC信号的时候看得比较明显。
 * param[in] enPcMode. PC模式的模式序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPCMode(XBH_PCMODE_E enPcMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的PC模式
 * param[out] enPcMode. PC模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPCMode(XBH_PCMODE_E *enPcMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置HDMI信号的色域范围
 * param[in] enColorRange. 色域模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHDMIColorRange(XBH_COLOR_RANGE_E enColorRange)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI信号的色域范围
 * param[out] enColorRange. 色域模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHDMIColorRange(XBH_COLOR_RANGE_E *enColorRange)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像的动态对比度开关
 * param[in] bEnable. XBH_TRUE：打开动态对比度，XBH_FALSE：关闭动态对比度
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setDCREnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的动态对比度开关
 * param[out] bEnable. XBH_TRUE：打开动态对比度，XBH_FALSE：关闭动态对比度
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getDCREnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像的运动补偿等级
 * param[in] enMemcLevel. 运动补偿等级
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setMEMCLevel(XBH_LEVEL_E enMemcLevel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的运动补偿等级
 * param[out] enMemcLevel. 运动补偿等级
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getMEMCLevel(XBH_LEVEL_E *enMemcLevel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像的画面显示比例
 * param[in] enAspect. 画面显示比例
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAspect(XBH_ASPECT_E enAspect)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的画面显示比例
 * param[out] enAspect. 画面显示比例
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAspect(XBH_ASPECT_E *enAspect)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的画面的平均亮度值，一般用来做能源之星功能用
 * param[out] u32Value. 画面的平均亮度值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAvgPixelLuma(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置图像的gamma，一般预置3组gamma
 * param[in] u32Value. gamma组的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGammaGroup(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取图像的gamma组的序号
 * param[out] u32Value. gamma组的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGammaGroup(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 保存对应模式的具体色温数据，保存到cusdata分区
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::saveColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 读取对应模式的具体色温数据，从cusdata分区读取
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::loadColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/******************************************************************************AUDIO***********************************************************/
/**
 * 设置声音模式
 * param[in] enSoundMode. 声音模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取声音模式
 * param[out] enSoundMode. 声音模式的序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSoundMode(XBH_SOUND_MODE_E* enSoundMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置各个声音通道的音量大小
 * param[in] enAudioChannel. 声音通道，u32Value音量大小
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setVolume(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取各个声音通道的音量大小
 * param[out] enAudioChannel. 声音通道，u32Value音量大小
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getVolume(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置音频输出通路模式
 * param[in] enAudioOutput. 声音输出的通路模式
 * XBH_AUDIO_OUTPUT_DET,        根据接入的设备将音频输出切换到设备上
 * XBH_AUDIO_OUTPUT_BOTH,       Speaker跟外接设备同时出声
 * XBH_AUDIO_OUTPUT_EXTERNAL,   只有外置设备出声音
 * XBH_AUDIO_OUTPUT_INTERNAL,   只有Speaker出声音
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取音频输出通路模式
 * param[out] enAudioOutput. 音频输出通路模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 设置音频通路的静音开关
 * param[in] enAudioChannel. 声音通道，bEnable 静音开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取音频通路的静音开关
 * param[out] enAudioChannel. 声音通道，bEnable 静音开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置同轴的输出模式
 * param[in] enSpdifMode. 输出模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSpdifMode(XBH_SPDIF_MODE_E enSpdifMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取同轴的输出模式
 * param[out] enSpdifMode. 输出模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSpdifMode(XBH_SPDIF_MODE_E* enSpdifMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置同轴的开关
 * param[in] bEnable. 同轴开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSpdifEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取同轴的开关
 * param[out] bEnable. 同轴开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSpdifEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置ARC输出的模式
 * param[in] enArcMode. ARC输出的模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setARCMode(XBH_ARC_MODE_E enArcMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取ARC输出的模式
 * param[out] enArcMode. ARC输出的模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getARCMode(XBH_ARC_MODE_E* enArcMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置ARC输出的开关
 * param[in] enArcMode. ARC输出的开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setARCEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取ARC输出的开关
 * param[out] enArcMode. ARC输出的开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getARCEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置音频输出的前置增益
 * param[in] s32Value. 音频增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPreScale(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取音频输出的前置增益
 * param[out] s32Value. 音频增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPreScale(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置音频输出的前置增益
 * param[in] enEqMode. EQ阶段，s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
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
XBH_S32 XbhPlatformInterface::getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value)
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
XBH_S32 XbhPlatformInterface::setBass(XBH_S32 s32Value)
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
XBH_S32 XbhPlatformInterface::getBass(XBH_S32* s32Value)
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
XBH_S32 XbhPlatformInterface::setTreble(XBH_S32 s32Value)
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
XBH_S32 XbhPlatformInterface::getTreble(XBH_S32* s32Value)
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
XBH_S32 XbhPlatformInterface::setBalance(XBH_S32 s32Value)
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
XBH_S32 XbhPlatformInterface::getBalance(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置line out的模式，通常接入耳机和line out时需要不同的音量大小，通过这个开关来决定输出幅度
 * param[in] enLineOutMode 输出模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setLineOutMode(XBH_LINEOUT_MODE_E enLineOutMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取line out的模式
 * param[out] enLineOutMode 输出模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getLineOutMode(XBH_LINEOUT_MODE_E* enLineOutMode)
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
XBH_S32 XbhPlatformInterface::setSurroundEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhPlatformInterface::getSurroundEnable(XBH_BOOL* bEnable)
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
XBH_S32 XbhPlatformInterface::setAvcEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhPlatformInterface::getAvcEnable(XBH_BOOL* bEnable)
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
XBH_S32 XbhPlatformInterface::setEqEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhPlatformInterface::getEqEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置功放端的声音模式
 * param[in] mode 声音模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAmpMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取功放端的声音模式
 * param[out] mode 声音模式
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAmpMode(XBH_S32* mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 通过去读取lic_status去获取授权的状态
 * param[out] 授权的状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getMicLicenseState(XBH_S32* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/******************************************************************************VGA***********************************************************/
/**
 * 设置VGA的RGB采样的ADC自动校准值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::autoAdc(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置VGA的图像位置相位等自动校准值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::autoAdjust(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置VGA图像的水平位置
 * param[in] u32Value 位置参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHPosition(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取VGA图像的水平位置
 * param[out] u32Value 位置参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHPosition(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置VGA图像的垂直位置
 * param[in] u32Value 位置参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setVPosition(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取VGA图像的垂直位置
 * param[out] u32Value 位置参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getVPosition(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置VGA图像的时钟位置
 * param[in] u32Value 时钟参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setClock(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取VGA图像的时钟位置
 * param[out] u32Value 位置参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getClock(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置VGA图像的相位位置
 * param[in] u32Value 相位参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPhase(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取VGA图像的相位位置
 * param[out] u32Value 位置参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPhase(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/******************************************************************************CEC**************************************************************/
/**
 * 设置CEC的总开关
 * param[in] enable 开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setCecEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取CEC的总开关
 * param[out] enable 开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getCecEnable(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置跟随CEC设备自动关机的开关
 * param[in] enable 开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setCecAutoPowerOffEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取跟随CEC设备自动关机的开关
 * param[out] enable 开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getCecAutoPowerOffEnable(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置跟随CEC设备自动开机的开关
 * param[in] enable 开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setCecAutoWakeUpEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取跟随CEC设备自动开机的开关
 * param[out] enable 开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getCecAutoWakeUpEnable(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/******************************************************************************SOURCE***********************************************************/
/**
 * 设置信源窗口的位置和大小
 * param[in] stRect 窗口位置大小， windows 主窗口/子窗口
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setWindowRect(XBH_RECT_S stRect, XBH_WINDOW_NUM_E windows)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取信源窗口的位置和大小
 * param[in] windows 主窗口/子窗口
 * param[out] stRect 窗口位置大小
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getWindowRect(XBH_RECT_S* stRect, XBH_WINDOW_NUM_E windows)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取信源的timing信息
 * param[in] windows 主窗口/子窗口
 * param[out] timing 信号信息
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getTimingInfo(XBH_TIMING_INFO_S* timing, XBH_WINDOW_NUM_E windows)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置信源的EDID类型
 * param[in] enType EDID类型，
 *           bd_idx bd_idx source.json中的bd_idx
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取信源的EDID类型
 * param[in] bd_idx bd_idx source.json中的bd_idx
 * param[out] enType EDID类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 设置单独每一路信源的EDID类型
 * param[in] enType EDID类型，
 *           u32SourceId 信源的ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取单独每一路信源的EDID类型
 * param[in] u32SourceId 信源的ID
 * param[out] enType EDID类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSourceEdidType(XBH_U32 u32SourceId, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取接入的ops的系统类型
 * param[in] enSource 指定的信源通道
 * param[out] type XBH_TRUE android，XBH_FALSE windows
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOpsSystemType(XBH_BOOL *type, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置OPS的电源控制引脚，当开机状态时执行此接口会让OPS关机，关机的时候执行此即可会让OPS开机
 * param[in] enSource 指定的信源通道
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OPS是否是开机状态
 * param[in] enSource 指定的信源通道
 * param[out] bEnable XBH_TRUE 开机状态，XBH_FALSE 关机状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOpsPowerStatus(XBH_BOOL* bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OPS是否是插入的状态
 * param[in] enSource 指定的信源通道
 * param[out] bEnable XBH_TRUE 插入OPS，XBH_FALSE 未插入OPS
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOpsDetStatus(XBH_BOOL* bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置OPS的供电状态（19V供电）
 * param[in] enSource 指定的信源通道
 * param[in] bEnable XBH_TRUE 给OPS供电，XBH_FALSE 给OPS断电
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置动态功率
 * param[in] enSource 指定的信源通道
 * param[in] pdability 充电瓦数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置gsv27xx系列CEC物理地址
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGsv27xxPhysicalAddr(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OPS的供电状态（19V供电）
 * param[in] enSource 指定的信源通道
 * param[out] bEnable XBH_TRUE OPS已经供电，XBH_FALSE OPS未供电
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOpsPowerEnable(XBH_BOOL* bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置HDMI TX
 * param[in] enInfo hdmi tx的信息类型
 * param[in] u32Data 信息的具体数据
 * 当选择XBH_HDMI_TX_TIMING时 u32Data 0：480P，1：1080P， 2：2160P ，0xFF Auto
 * 当选择XBH_HDMI_TX_HDCP_ONOFF时 u32Data 0：关闭HDCP校验，1：开启HDCP校验
 * 当选择XBH_HDMI_TX_VIDEO_MUTE时 u32Data 0：图像UNMUTE，1：图像MUTE
 * 当选择XBH_HDMI_TX_AUDIO_MUTE时 u32Data 0：声音UNMUTE，1：声音MUTE
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI TX的信息
 * param[in] enInfo hdmi tx的信息类型
 * param[out] info 信息的具体数据, 需要兼容获取版本号，这里使用字符串
 * 当选择XBH_HDMI_TX_TIMING时 u32Data 0：480P，1：1080P， 2：2160P ，0xFF Auto
 * 当选择XBH_HDMI_TX_HDCP_ONOFF时 u32Data 0：关闭HDCP校验，1：开启HDCP校验
 * 当选择XBH_HDMI_TX_VIDEO_MUTE时 u32Data 0：图像UNMUTE，1：图像MUTE
 * 当选择XBH_HDMI_TX_AUDIO_MUTE时 u32Data 0：声音UNMUTE，1：声音MUTE
 * 当选择XBH_HDMI_TX_CHIP_ACTIVE, u32Data 0：芯片不在线，1：芯片在线
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置OPS reset
*/
XBH_S32 XbhPlatformInterface::setOpsResetEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置HDMI TX/TYPEC TX 模式
* param[in] 按照HDMI TX还是TYPEC TX输出。0：hdmi tx， 1： typec tx
*/
XBH_S32 XbhPlatformInterface::setVideoTxMode(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取HDMI TX/TYPEC TX 模式
* param[out] 按照HDMI TX还是TYPEC TX输出。0：hdmi tx， 1： typec tx
*/
XBH_S32 XbhPlatformInterface::getVideoTxMode(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* get hdmi rx audio sample freq
*/
XBH_S32 XbhPlatformInterface::getHdmiRxAudioSampleFreq(XBH_U32 *u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* get hdmi rx edid invaild
*/
XBH_S32 XbhPlatformInterface::getEdidInvaild(XBH_HDMIRX_EDID_TYPE_E edidType, XBH_U8 *edidInvaild)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* get hdmi rx audio locked status
*/
XBH_S32 XbhPlatformInterface::getHdmiRxAudioLocked(XBH_U32 *u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/******************************************************************************SYSTEM***********************************************************/
/**
 * 获取唤醒源
 * param[out] stWake 唤醒源的具体信息
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置bootloader的系统属性
 * param[in] key 属性名
 * param[in] value 属性值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setBootEnv(const XBH_CHAR* key, XBH_CHAR* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取bootloader的系统属性
 * param[in] key 属性名
 * param[out] value 属性值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getBootEnv(const XBH_CHAR* key, XBH_CHAR* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置vendor层的系统属性
 * param[in] key 属性名
 * param[in] value 属性值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setVendorProperties(const XBH_CHAR* key, XBH_CHAR* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置上电模式
 * param[in] enMode 上电模式的类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPoweronMode(XBH_POWERMODE_E enMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取上电模式
 * param[out] enMode 上电模式的类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPoweronMode(XBH_POWERMODE_E* enMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置唤醒类型的开关
 * param[in] enWakeUpType 唤醒源
 * param[in] bEnable 唤醒开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取唤醒类型的开关
 * param[in] enWakeUpType 唤醒源
 * param[out] bEnable 唤醒开关
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置系统运行的时长，此记录不会因为升级而被擦除掉
 * param[in] time 运行时间
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setChipRunTime(XBH_S32 time)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取系统运行的时长，此记录不会因为升级而被擦除掉
 * param[out] time 运行时间
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getChipRunTime(XBH_S32* time)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置开机logo
 * param[in] path 开机logo资源的路径
 * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setBootLogo(const XBH_CHAR* path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置开机视频
 * param[in] level 开机视频资源的路径
 * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setBootVideo(const XBH_CHAR* path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置开机动画
 * param[in] level 开机动画资源的路径
 * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setBootAnimation(const XBH_CHAR* path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置板控升级
 * @return int 当前锁定类型
*/
XBH_S32 XbhPlatformInterface::loadEdlaCust(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 获取版控版本
 * param[out] strSn 板控版本
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getProjectIdVersion(XBH_CHAR* str)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置待机模式
 * @param mode 待机模式
 *           0,      正常关机，系统功耗最低
 *           1,      熄屏模式，只关闭屏幕，声音等
 *           2       STR关机模式
 * @return 执行是否成功
 */
XBH_S32 XbhPlatformInterface::setStandbyMode(XBH_U32 u32Mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 获取待机模式
 * @return 待机模式
 *           0,      正常关机，系统功耗最低
 *           1,      熄屏模式，只关闭屏幕，声音等
 *           2       STR关机模式
 */
XBH_S32 XbhPlatformInterface::getStandbyMode(XBH_U32 *u32Mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 关机时设置eywa释放资源和操作设备
 * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setOnStop(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 信号源的屏幕下移
 *
 * param[in]
 *           x: 横轴偏移量
 *           y: 纵轴偏移量
 * retval true:success, false:failure
 */
XBH_S32 XbhPlatformInterface::setSourceScreenOffset(XBH_S32 x, XBH_S32 y)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 获取信号源屏幕下移偏移量
 */
XBH_S32 XbhPlatformInterface::getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 用于息屏亮屏后重置VBO to HDMI IC Mn869129芯片状态, 使其恢复出图
 */
XBH_S32 XbhPlatformInterface::setOnResume(XBH_S32 state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

//WatchDog
XBH_S32 XbhPlatformInterface::setWatchDogManualModeEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

XBH_S32 XbhPlatformInterface::getWatchDogManualModeEnable(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

XBH_S32 XbhPlatformInterface::setWatchDogEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

XBH_S32 XbhPlatformInterface::setWatchDogTimeOut(XBH_U32 u32TimeOutSec)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

XBH_S32 XbhPlatformInterface::getWatchDogTimeOut(XBH_U32 *u32TimeOutSec)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

XBH_S32 XbhPlatformInterface::watchDogPing(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
 * 设置monitorId，唯一ID.用于控制多台设备绑定唯一id，遥控器单独控制设备，与deviceid功能独立
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setMonitorId(XBH_S32 monitorId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取monitorId，唯一ID.用于控制多台设备绑定唯一id，遥控器单独控制设备，与deviceid功能独立
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getMonitorId(XBH_S32* monitorId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/*
* 获取eth0 speed
* param[in] port口序号
* param[out] net speed获取返回值 
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getEthPortSpeed(XBH_S32 port, XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/*
* 获取hardwareVer
* param[in] hardwareVer
* param[out] hardwareVer获取返回值 
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHardwareVer(XBH_S32* hardwareVer)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取getUsbInfos
 * retval 0:success,-1:failure
*/
std::string XbhPlatformInterface::getUsbDeviceInfos(const std::string& input)
{
    XLOGW("this func is not implement");
    return "[]";
}

/**
 * 设置故障检测的开关
 * param[in] bEnable 检测开关
 * param[in] u32DetectType 检测类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setErrorDetectState(XBH_BOOL bEnable, XBH_U32 u32DetectType)
{
    XBH_U32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取故障检测的开关
 * param[out] bEnable 检测开关
 * param[out] u32DetectType 检测类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getErrorDetectState(XBH_BOOL* bEnable, XBH_U32* u32DetectType)
{
    XBH_U32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取故障码
 * param[out] u32ErrorCode
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getErrorCode(XBH_U32* u32ErrorCode)
{
    XBH_U32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 清除故障码
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::clearErrorCode(void)
{
    XBH_U32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/******************************************************************************FACTORY***********************************************************/
/**
 * 设置当前信源画面的重显率数据
 * param[in] stRect 重显率数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setOverscan(XBH_RECT_S* stRect)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取当前信源画面的重显率数据
 * param[out] stRect 重显率数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOverscan(XBH_RECT_S* stRect)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置SOC输出的tese pattern
 * param[in] pattern test pattern 序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setTestPattern(XBH_PATTERN_E pattern, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取SOC输出的tese pattern
 * param[out] pattern test pattern 序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getTestPattern(XBH_PATTERN_E* pattern, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置主副屏的屏参序号
 * param[in] enPanel. 屏幕序号
 * param[in] s32Value 屏参序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPanelIndex(XBH_S32 s32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取主副屏的屏参序号
 * param[in] enPanel. 屏幕序号
 * param[out] s32Value 屏参序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPanelIndex(XBH_S32* s32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置主副屏的背光PWM频率
 * param[in] enPanel. 屏幕序号
 * param[in] u32PwmF PWM频率
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPwmFrequency(XBH_U32 u32PwmF, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取主副屏的背光PWM频率
 * param[in] enPanel. 屏幕序号
 * param[out] u32PwmF PWM频率
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPwmFrequency(XBH_U32* u32PwmF, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置主副屏的背光PWM极性翻转
 * param[in] enPanel. 屏幕序号
 * param[in] bEnable XBH_TRUE 翻转极性，XBH_FALSE 不翻转极性
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取主副屏的背光PWM极性翻转
 * param[in] enPanel. 屏幕序号
 * param[out] bEnable XBH_TRUE 翻转极性，XBH_FALSE 不翻转极性
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
    * 设置背光最大值
    * param[in] state. 背光反向的最大值
    * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setBrightMax(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
    * 获取背光最大值
    * retval boolean. 背光最大值状态
*/
XBH_S32 XbhPlatformInterface::getBrightMax(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
    * 设置背光最小值
    * param[in] state. 背光反向的最小值
    * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setBrightMin(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
    * 获取背光最小值
    * retval boolean. 背光最小值状态
*/
XBH_S32 XbhPlatformInterface::getBrightMin(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置串口debug的开关
 * param[in] bEnable XBH_TRUE 打开串口debug，XBH_FALSE 关闭串口debug
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setDebugEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取串口debug的开关
 * param[out] bEnable XBH_TRUE 打开串口debug，XBH_FALSE 关闭串口debug
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getDebugEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入有线网的MAC地址
 * param[in] macType
 *          0 板载Ethernet
 *          1 OPS
 *          2 SDM
 *          3 Usbc
 *          4 RNDIS
 * param[in] strMacAddress 有线网的MAC地址
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取有线网的MAC地址
 * param[in] macType
 *          0 板载Ethernet
 *          1 OPS
 *          2 SDM
 *          3 Usbc
 *          4 RNDIS
 * param[out] strMacAddress 有线网的MAC地址
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入serial number
 * param[in] value serial number
 * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取serial number
 * param[out] strSn sn码
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 写入分区数据
* param[in] pBuff 要写入的实际数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setCustProductInfo(const XBH_CHAR* pBUff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 读取分区数据
* param[in] pBuff 要读取的实际数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getCustProductInfo(XBH_CHAR* pBUff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 获取指定通道的唤醒引脚状态
* param[in] enSource 要指定通道
*param[out] bEnable 唤醒信号状态
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOpsWakeSoc(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
 /**
    * 获取指定通道的ops运行状态
    * param[in] enSource 指定通道
    *param[out] bEnable :ture 低电平,false 高电平
    * retval 0:success,-1:failure
    */
XBH_S32 XbhPlatformInterface::getOpsOperationStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 设置指定通道的recovery控制引脚电平
* param[in] enSource 指定通道
*param[in] bEnable :ture 低电平,false 高电平
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setOpsRecovery(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入HDCP
 * param[in] strPath 写入HDCP的文件路径
 * param[in] type    写入HDCP的类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdcpKey(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}



/**
 * 获取写入HDCP的状态
 * param[in] type    写入HDCP的类型
 * param[out] bEnable 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取hdmi switch HDCP的值
* param[out] XBH_CHAR pBuff
* param[in] type    HDCP的类型
* param[in] source  信源的类型
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdcpKeydata(XBH_SOURCE_E  source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入hdcp file name
 * param[in] hdcpType
 * param[in] hdcpfilename
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("setHdcpKeyName this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDCP filename
 * param[in] hdcpType
 * param[out] filename
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("getHdcpKeyName this func is not implement");
    return  s32Ret;
}

/**
 * 写入HDMI EDID
 * param[in] strPath 写入edid的文件路径
 * param[in] idx     写入信源序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdmiEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取写入hdmi edid的状态
 * param[in] idx     写入信源序号
 * param[out] bEnable 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdmiEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入HDCP
 * param[in] strPath 写入HDCP的文件路径
 * param[in] type    写入HDCP的类型
 * param[in] devType 对应设备类型
 *                    0x104: BOARD_LT8668SX_1
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdcpExtendIc(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取写入HDCP的状态
 * param[in] type    写入HDCP的类型
 * param[out] bEnable 是否已经写入
 *                    0x104: BOARD_LT8668SX_1
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdcpExtIcStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入hdcp file name
 * param[in] hdcpType
 * param[in] hdcpfilename
 *                    0x104: BOARD_LT8668SX_1
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setHdcpExtIcKeyName(const XBH_CHAR* strHdcpFileName,XBH_S32 hdcpType, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("setHdcpKeyName this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDCP filename
 * param[in] hdcpType
 * param[out] filename
 *                    0x104: BOARD_LT8668SX_1
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getHdcpExtIcKeyName(XBH_CHAR* strHdcpFileName, XBH_S32 hdcpType, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("getHdcpKeyName this func is not implement");
    return  s32Ret;
}

/**
 * 写入VGA EDID
 * param[in] strPath 写入edid的文件路径
 * param[in] idx     写入信源序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取写入VGA edid的状态
 * param[in] idx     写入信源序号
 * param[out] bEnable 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取写入VGA edid的checksum
 * param[in] idx     写入信源序号
 * param[out] checksum  vga checksum值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 打印写入的EDID
*/
XBH_S32 XbhPlatformInterface::dumpEdid(XBH_SOURCE_E idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入google key
 * param[in] strPath 写入google key的文件路径
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGoogleKey(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取写入Google key的状态
 * param[out] bEnable 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGoogleKeyStatus(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 写入Google key file name
 * param[in] Google key filename
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGoogleKeyName(const XBH_CHAR* strGooglekeyFileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("setGoogleKeyName this func is not implement");
    return  s32Ret;
}

/**
 * 获取Googlekey filename
 * param[out] filename
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGoogleKeyName(XBH_CHAR* strGooglekeyFileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("getGoogleKeyName this func is not implement");
    return  s32Ret;
}

/**
 * 写入Attention key
 * param[in] strPath 写入attention key的文件路径
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAttentionKey(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取写入Attention key的状态
 * param[out] bEnable 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAttentionKeyStatus(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
     * 获取RKP crs
     * param[out] read_buffer json格式字符串
     * retval 0:success,-1:failure
    */
XBH_S32 XbhPlatformInterface::getRkpCsrInfo(XBH_CHAR* read_buffer, int buffer_size)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * setAgingModeState
 * param[in] state
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setAgingModeState(XBH_S32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_AGING_MODE_STATE_LEN] = {0};
    if (u32Value == 1)
    {
        s8Buff[0] = 1;
    }
    else
    {
        s8Buff[0] = 0;
    }
    s32Ret = XbhPartitionData::getInstance()->setAgingModeState((XBH_VOID *)s8Buff);
    XLOGD("setAgingModeState s8Buff[0]= %d \n", s8Buff[0]);
    return  s32Ret;
}

/**
 * getAgingModeState
 * param[out] state 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAgingModeState(XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_AGING_MODE_STATE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getAgingModeState((XBH_VOID *)s8Buff);
    XLOGD("getAgingModeState s8Buff[0]= %d \n", s8Buff[0]);
    *u32Value =  s8Buff[0] == 1 ? 1 : 0;
    return  s32Ret;
}

/**
 * setMokaPModeEnableToCusdata
 * param[in] state
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setMokaPModeEnableToCusdata(XBH_S32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * getMokaPModeEnableFromCusdata
 * param[out] state 是否已经写入
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getMokaPModeEnableFromCusdata(XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 升级U盘固件
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::upgradeSystemFirmware(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* set device id, 广告机部分客户的需求
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setDeviceId(const XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* get device id, 广告机部分客户的需求
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getDeviceId(XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* get  USBC Forward and Reverse Insertion Det, 
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getUSBCForwardReverseInsertionDet(XBH_SOURCE_E idx, XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* set default multiuser enable status
*/
XBH_S32 XbhPlatformInterface::setMultiUser(const XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* get default multiuser enable status
*/
XBH_S32 XbhPlatformInterface::getMultiUser(XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* set default vspage enable status
*/
XBH_S32 XbhPlatformInterface::setVSPage(const XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* get default vspage enable status
*/
XBH_S32 XbhPlatformInterface::getVSPage(XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* set default sku status
*/
XBH_S32 XbhPlatformInterface::setCustomSKU(const XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* get default sku enable status
*/
XBH_S32 XbhPlatformInterface::getCustomSKU(XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* set default bootmode status
*/
XBH_S32 XbhPlatformInterface::setBootMode(const XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* get default bootmode status
*/
XBH_S32 XbhPlatformInterface::getBootMode(XBH_CHAR* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 将HVB KEY写入到hisi的芯片并使能安全芯片
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSecurityHvbKey()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取hisi芯片的安全芯片使能状态
* param[out] u32Status 使用能状态 0：未使用，1：已使能
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSecurityHvbKeyStatus(XBH_U32 *u32Status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/******************************************************************************DEVICE***********************************************************/
/**
 * 设置IO输出的电平
 * param[in] u32GpioNumber IO引脚的序号
 * param[in] u32GpioValue 电平值 0：低电平，1：高电平
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取IO输出的电平
 * param[in] u32GpioNumber IO引脚的序号
 * param[out] u32GpioValue 电平值 0：低电平，1：高电平
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取IO输入的电平
 * param[in] u32GpioNumber IO引脚的序号
 * param[out] u32GpioValue 电平值 0：低电平，1：高电平
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置RTC的数据
 * param[in] RTC的时间数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setRtcTime(XBH_RTC_INFO_S* stRtcInfo)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取RTC的数据
 * param[out] RTC的时间数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getRtcTime(XBH_RTC_INFO_S* stRtcInfo)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置RTC的数据
 * param[in] RTC的时间数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setRtcAlarmTime(XBH_RTC_ALARM_INFO_S* stRtcAlarmInfo)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置I2c Bus的lock状态，当需要对IC进行升级时，可以先锁定IIC防止操作异常
 * param[in] iicBus 需要锁住的iic bus
 * param[in] lock 是否要锁住
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setIICLock(XBH_U32 iicBus, XBH_BOOL lock)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

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
XBH_S32 XbhPlatformInterface::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 通过IIC读取寄存器数据
 * param[in] u32I2cNum iic bus
 * param[in] u8DevAddress 设备地址
 * param[in] u32RegAddr 设备寄存器地址
 * param[in] u32RegAddrCount 设备寄存器长度
 * param[in] u32Length 读取的数据长度
 * param[out] u8Data 读取的数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置屏幕背光开关（不推荐使用）
 * param[in] bEnable. XBH_TRUE：打开背光，XBH_FALSE：关闭背光
 * param[in] enPanel. 屏幕序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setBacklightEnable(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取屏幕背光开关（不推荐使用）
 * param[out] bEnable. XBH_TRUE：打开背光，XBH_FALSE：关闭背光
 * param[in] enPanel. 屏幕序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getBacklightEnable(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置屏幕开关，跟开关背光的区别是，设置屏幕开关会在控制背光的基础上同时控制TCON的供电，并且之间有时序控制
 * 只关闭背光，有些屏幕还能看到屏幕上面有很暗的画面。非特殊屏幕，推荐在关闭屏幕时采用setPanelPower接口
 * param[in] bEnable. XBH_TRUE：点亮屏幕，XBH_FALSE：熄灭屏幕
 * param[in] enPanel. 屏幕序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取屏幕开关
 * param[out] bEnable. XBH_TRUE：点亮屏幕，XBH_FALSE：熄灭屏幕
 * param[in] enPanel. 屏幕序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置指示灯的亮灯状态
 * param[in] enState. LED指示灯的亮灯状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取指示灯的亮灯状态
 * param[out] enState. LED指示灯的亮灯状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取有线网络插入状态
 * param[out] bEnable. 有线网插入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getEthPlugStatus(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 根据ADC通路获取数值
 * param[in] enChannel. ADC通路序号
 * param[out] bEnable. 有线网插入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 环境光传感器是否存在
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 人体红外传感器是否存在，Passive Infrared Sensor
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 霍尔传感器是否存在
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 空气传感器是否存在
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getAirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 主板温度传感器是否存在
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getTemperatureSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 重力传感器是否存在
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getGSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取光线传感器的数据
 * param[out] s32Value 光线传感器的数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取距离传感器原始数据
 * param[out] s32Value 传感器的数据，人体靠近触发
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getPirSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取霍尔传感器的组合电平值（PenA + PenB）
 * param[out] s32Value 返回两笔电平状态的叠加值（2: 两只笔都在面板上 1：一只笔在面板上  0: 没有笔在面板上）
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取霍尔传感器的笔状态编码值（PenA + PenB = 16进制）
 * param[out] s32Value 编码值 (低字节bit0: PenA状态（0:抬起, 1:接触） 高字节bit8: PenB状态（0:抬起, 1:接触）)
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getHallSensorPen(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 功率传感器是否存在
 * param[out] bEnable. 是否存在
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getPowerSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取功率传感器的数据
 * param[out] s32Value 功率传感器的数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPowerSensorValue(XBH_FLOAT* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

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
XBH_S32 XbhPlatformInterface::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取NTC温度
 * param[in] s32Value 0:主板NTC 1:前置板NTC,etc
 * param[out] fValue 传感器的数据
 * retval 0:success,-1:failure
 */
 XBH_S32 XbhPlatformInterface::getNTCValue(XBH_S32 s32Value, XBH_FLOAT* fValue)
 {
     XBH_S32 s32Ret = XBH_FAILURE;
     XLOGW("this func is not implement");
     return  s32Ret;
 }

/**
 * 获取主板的工作温度
 * param[out] s32Value 传感器的数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getTemperatureSensorValue(XBH_FLOAT* fValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取重力传感器原始数据
 * param[out] s32Value 传感器的数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

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
 *                    0x00c: BOARD_GSV6712_1
 *                    0x00d: BOARD_GSV2712
 *                    0x00e: FRONT_GSV2712
 *                    0x00f: BOARD_GSV2202E
 *                    0x010: FRONT_GSV2202E
 *                    0x011: BOARD_GSV2702
 *                    0x012: FRONT_GSV2702
 *                    0x100: BOARD_LT8711
 *                    0x101: FRONT_LT8711
 *                    0x102: BOARD_LT8711UXE1
 *                    0x103: FRONT_LT8711UXE1
 *                    0x104: BOARD_LT8668SX_1
 *                    0x105: BOARD_LT8668SXC_1
 *                    0x200: ITE68411
 *                    0x201: MN869129
 *                    0x202: ITE6265
 *                    0x203: ITE6563OUT
 *                    0x204: LT6711A
 *                    0x300: FRONT_VL105
 *                    0x301: BOARD_VL105
 *                    0x400: MCU
 *                    0x500: AUDIO_CODEC
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 升级扩展IC
 * @param data      dataflow
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
 *                    0x00c: BOARD_GSV6712_1
 *                    0x00d: BOARD_GSV2712
 *                    0x00e: FRONT_GSV2712
 *                    0x00f: BOARD_GSV2202E
 *                    0x010: FRONT_GSV2202E
 *                    0x011: BOARD_GSV2702
 *                    0x012: FRONT_GSV2702
 *                    0x100: BOARD_LT8711
 *                    0x101: FRONT_LT8711
 *                    0x102: BOARD_LT8711UXE1
 *                    0x103: FRONT_LT8711UXE1
 *                    0x104: BOARD_LT8668SX_1
 *                    0x105: BOARD_LT8668SXC_1
 *                    0x200: ITE68411
 *                    0x201: MN869129
 *                    0x202: ITE6265
 *                    0x203: ITE6563OUT
 *                    0x204: LT6711A
 *                    0x300: FRONT_VL105
 *                    0x301: BOARD_VL105
 *                    0x400: MCU
 *                    0x500: AUDIO_CODEC
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::upgradeExtendIcByData(XBH_U8 *data, XBH_U32 dataLen, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}



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
 *                    0x00c: BOARD_GSV6712_1
 *                    0x00d: BOARD_GSV2712
 *                    0x00e: FRONT_GSV2712
 *                    0x00f: BOARD_GSV2202E
 *                    0x010: FRONT_GSV2202E
 *                    0x011: BOARD_GSV2702
 *                    0x012: FRONT_GSV2702
 *                    0x100: BOARD_LT8711
 *                    0x101: FRONT_LT8711
 *                    0x102: BOARD_LT8711UXE1
 *                    0x103: FRONT_LT8711UXE1
 *                    0x104: BOARD_LT8668SX_1
 *                    0x105: BOARD_LT8668SXC_1
 *                    0x200: ITE68411
 *                    0x201: MN869129
 *                    0x202: ITE6265
 *                    0x203: ITE6563OUT
 *                    0x204: LT6711A
 *                    0x300: FRONT_VL105
 *                    0x301: BOARD_VL105
 *                    0x400: MCU
 *                    0x500: AUDIO_CODEC
 * @return 0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
 */
XBH_S32 XbhPlatformInterface::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置 HDMITX使用的密钥版本
 *
 * @param hdcpTxEncryptionMode   密钥版本
 *                    0: XBH_HDCPTX_ENCRYPTION_1P4
 *                    1: XBH_HDCPTX_ENCRYPTION_2P2
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::setHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E hdcpTxEncryptionMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取 HDMITX使用的密钥版本
 *
 * retval hdcpTxEncryptionMode 0: 1.4  2: 2.2
 */
XBH_S32 XbhPlatformInterface::getHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E *hdcpTxEncryptionMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

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
 *                    0x008: BOARD_GSV2705_1
 *                    0x009: BOARD_GSV2705_2
 *                    0x00a: BOARD_GSV2715_1
 *                    0x00b: BOARD_GSV2715_2
 *                    0x00c: BOARD_GSV6712_1
 *                    0x00d: BOARD_GSV2712
 *                    0x00e: FRONT_GSV2712
 *                    0x00f: BOARD_GSV2202E
 *                    0x010: FRONT_GSV2202E
 *                    0x011: BOARD_GSV2702
 *                    0x012: FRONT_GSV2702
 *                    0x100: BOARD_LT8711
 *                    0x101: FRONT_LT8711
 *                    0x102: BOARD_LT8711UXE1
 *                    0x103: FRONT_LT8711UXE1
 *                    0x104: BOARD_LT8668SX_1
 *                    0x105: BOARD_LT8668SXC_1
 *                    0x200: ITE68411
 *                    0x201: MN869129
 *                    0x202: ITE6265
 *                    0x203: ITE6563OUT
 *                    0x204: LT6711A
 *                    0x300: FRONT_VL105
 *                    0x301: BOARD_VL105
 *                    0x400: MCU
 *                    0x500: AUDIO_CODEC
 * @return 扩展IC当前版本
 */
XBH_S32 XbhPlatformInterface::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置DDR的展频
 * param[in] ddrIndex. ddr的序号
 * param[in] sscInfo.  展频信息
 * retval boolean 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setDdrSSC(XBH_SSC_INFO_S sscInfo, XBH_S32 ddrIndex)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取DDR的展频
 * param[in] ddrIndex. ddr的序号
 * retval sscInfo.  展频信息
*/
XBH_S32 XbhPlatformInterface::getDdrSSC(XBH_SSC_INFO_S* sscInfo, XBH_S32 ddrIndex)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置屏幕展频
 * param[in] panelIdx. 屏幕序号
 * param[in] sscInfo.  展频信息
 * retval boolean 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setPanelSSC(XBH_SSC_INFO_S sscInfo, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕的展频
 * param[in] panelIdx. 屏幕的序号
 * retval sscInfo.  展频信息
*/
XBH_S32 XbhPlatformInterface::getPanelSSC(XBH_SSC_INFO_S* sscInfo, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕的名称
 * param[in] panelIdx. 屏幕的序号
 * retval String.  屏幕名称
*/
XBH_S32 XbhPlatformInterface::getPanelName(XBH_CHAR* name, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕的物理分辨率
 * param[in] panelIdx. 屏幕的序号
 * retval String.  屏幕物理分辨率字符串，例如 3840x2160
*/
XBH_S32 XbhPlatformInterface::getPanelResolution(XBH_CHAR* resolution, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置屏幕的选择角度，通常调用framework里面的接口实现UI的旋转，原厂的接口实现视频的旋转
 * param[in] value. 屏幕的旋转角度value 0,1,2,3 分别表示旋转 0, 90, 180, 270
 * param[in] panelIdx. 屏幕的序号
 * retval boolean. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setPanelRotation(XBH_S32 value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕的旋转方向
 * param[in] panelIdx. 屏幕的序号
 * retval int. 屏幕的旋转方向
*/
XBH_S32 XbhPlatformInterface::getPanelRotation(XBH_S32* value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置屏幕单分屏还是双分屏
 * param[in] type. 0: 单分屏       1: 双分屏
 * param[in] panelIdx. 屏幕的序号
 * retval boolean. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕是单分屏还是双分ping
 * param[in] panelIdx. 屏幕的序号
 * retval int. 分屏类型
*/
XBH_S32 XbhPlatformInterface::getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置Hdmi tx vedio mute状态
 * param[in] type. 0: unmute video       1: mute video
 * retval boolean. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setHdmiTxVideoMuteState(XBH_BOOL state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("Mn869129 this func is not implement");
    return  s32Ret;
}
/**
 * 获取Hdmi tx vedio mute状态
 * param[in] type. 0: unmute video       1: mute video
 * retval boolean. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::getHdmiTxVideoMuteState(XBH_BOOL *state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("Mn869129 this func is not implement");
    return  s32Ret;
}

/**
 * 设置屏参的奇偶和map参数
 * param[in] type. 111 第一位代表是否启用，第二位代表奇偶，第三位表示mapp 
 * retval boolean. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setPanelMapSwap(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏参的奇偶和map参数
 * param[in] type. 111 第一位代表是否启用，第二位代表奇偶，第三位表示mapp 
 * retval int. 屏参参数
*/
XBH_S32 XbhPlatformInterface::getPanelMapSwap(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置usbc是otg还是信源
 * param[in] type. 0是信源, 1是otg
 * retval boolean. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setUsbcConfig(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取usbc是otg还是信源
 * param[in] type. 0是信源, 1是otg
 * retval int. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::getUsbcConfig(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OTG端口是HOST模式还是DEVICE模式, TYPE-B以及TYPE-C in/out端子均可连接SOC OTG，因方案各异
 * param[in] type. 0是HOST, 1是DEVICE
 * retval int. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::getHostOrDeviceStatus(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置OTG端口是HOST模式还是DEVICE模式, TYPE-B以及TYPE-C in/out端子均可连接SOC OTG，因方案各异
 * param[in] type. 0是HOST, 1是DEVICE
 * retval int. 设置是否成功
*/
XBH_S32 XbhPlatformInterface::setHostOrDeviceStatus(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置屏幕单的镜像模式
 * param[in] value. true：镜像     false：非镜像
 * param[in] panelIdx. 屏幕的序号
 * retval boolean. 设置是否成功
*/


XBH_S32 XbhPlatformInterface::setPanelFlipMirror(XBH_BOOL value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕的镜像模式
 * param[in] panelIdx. 屏幕的序号
 * retval boolean. 镜像模式
*/
XBH_S32 XbhPlatformInterface::getPanelFlipMirror(XBH_BOOL* value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OPS/SDM是否被硬件锁牢（硬件螺丝）
 * param[in] srcIdx. OPS/SDM的通道序号
 * retval boolean. 是否锁定
*/
XBH_S32 XbhPlatformInterface::getOpsLockStatus(XBH_BOOL* value, XBH_SOURCE_E srcIdx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置屏幕是否开启像素移动
 * param[in] enable. 开关像素移动
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setPixelShiftEnable(XBH_BOOL value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取屏幕是否开启像素移动
 * retval boolean. 是否开启
*/
XBH_S32 XbhPlatformInterface::getPixelShiftEnable(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置图形的亮度
 * param[in] value. 亮度值
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setGraphicBrightness(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取图形的亮度
 * retval int. 亮度
*/
XBH_S32 XbhPlatformInterface::getGraphicBrightness(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置图形的对比度
 * param[in] value. 对比度值
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setGraphicContrast(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取图形的对比度
 * retval int. 对比度
*/
XBH_S32 XbhPlatformInterface::getGraphicContrast(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置图形的饱和度
 * param[in] value. 饱和度值
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setGraphicSaturation(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取图形的饱和度
 * retval int. 饱和度
*/
XBH_S32 XbhPlatformInterface::getGraphicSaturation(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置图形的色温模式
 * param[in] value. 色温模式值
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setGraphicColorTempMode(XBH_COLORTEMP_E value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取图形的色温模式
 * retval XbhColorTempMode. 色温模式
*/
XBH_S32 XbhPlatformInterface::getGraphicColorTempMode(XBH_COLORTEMP_E* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置是否开机能源之星
 * param[in] enable. 开关
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setEnergyStarEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取是否开机能源之星
 * retval boolean. 是否开关
*/
XBH_S32 XbhPlatformInterface::getEnergyStarEnable(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置Adc rgb的增益和偏移值（VGA,YPBPR）
 * param[in] srcIdx. 信源序号
 * param[in] offset. 增益和偏移值
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setAdcGainOffset(XBH_SOURCE_E srcIdx, XBH_GAIN_OFFSET_DATA_S offset)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Adc rgb的增益和偏移值（VGA,YPBPR）
 * param[in] srcIdx. 信源序号
 * retval XbhGainOffsetItem. 增益和偏移值
*/
XBH_S32 XbhPlatformInterface::getAdcGainOffset(XBH_SOURCE_E srcIdx, XBH_GAIN_OFFSET_DATA_S* offset)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置USB锁定类型
 * param[in] u32UsbType. 锁定类型，0 - 不锁定， 1 - 锁定前置USB， 2, - 锁定侧边USB，3 - 锁定全部
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setUsbLockMode(XBH_U32 u32UsbType) {
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 获取USB锁定类型
 * @return int 当前锁定类型
*/
XBH_S32 XbhPlatformInterface::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}
/**
 * 获取OPS的供电是否处于电流过大的状态
 * param[in] srcIdx. 信号源ID
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::getOpsOvercurrentStatus(XBH_U32 srcIdx, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
 * 根据OPS过流状态设置前置type-c功率
 * param[in] value. 功率值
 * retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 获取nfc设备是否存在
* retval boolean. true 存在， false 不存在
*/
XBH_S32 XbhPlatformInterface::isNfcExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 使能nfc
* param[in] enable. true 打开， false 关闭
* retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 获取nfc使能状态
* retval boolean. true 打开， false 关闭
*/
XBH_S32 XbhPlatformInterface::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 获取nfc id信息
* retval XbhNfcIdInfo. ID信息和长度
*/
XBH_S32 XbhPlatformInterface::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 添加nfc id信息
* retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 删除nfc id信息
* retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
* 删除所有nfc id信息
* retval boolean. 是否成功
*/
XBH_S32 XbhPlatformInterface::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**
 * 设置fatt模式
 */
XBH_S32 XbhPlatformInterface::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取fatt模式
 */
XBH_S32 XbhPlatformInterface::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * mcu 外置iic数据转发
 */
XBH_S32 XbhPlatformInterface::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * mcu 外置iic数据获取
 */
XBH_S32 XbhPlatformInterface::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 使能/禁用按键板
 */
XBH_S32 XbhPlatformInterface::setFattKeypadenbale(XBH_S32 keypad_enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取keypad信息
 */
XBH_S32 XbhPlatformInterface::getFattKeypadInfo(XBH_S32* keypad_enable, XBH_S32* key_count, XBH_S32* key_index)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhPlatformInterface::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhPlatformInterface::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhPlatformInterface::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入Project ID
 * param[in] value Project ID
 * retval true:success, false:failure
*/
XBH_S32 XbhPlatformInterface::setProjectId(const XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Project ID
 * param[out] strProjectId Project ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getProjectId(XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * Tcon检测中获取mcu传过来的数值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getTCONValue(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置升降支架
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setLiftbracketOperate(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取升降支架是否有节点
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getLiftbracketExist(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 通过iic设置背光
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setBacklightData(XBH_U32 data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 使用GPIO的名字来设置GPIO的方向和值
 * param[in] gpioName. gpio名字
 * param[in] gpioDirection. gpio方向 1:输出 2.输入
 * param[in] u32GpioValue. gpio输出值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32 gpioDirection, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 使用GPIO的名字来设获取GPIO的值
 * param[in] gpioName. gpio名字
 * param[out] u32GpioValue. gpio获取返回值 
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32* u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置电源灯亮使能
 * true时表示开机的时候要亮灯，待机的时候也亮灯；false的时候表示开机的时候要灭灯，待机的时候也要灭灯
 * param[in] bEnable. XBH_TRUE：打开，XBH_FALSE：关闭
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setPowerLedLightEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取电源灯亮使能
 * param[out] bEnable. XBH_TRUE：打开，XBH_FALSE：关闭
 * true时表示开机的时候要亮灯，待机的时候也亮灯；false的时候表示开机的时候要灭灯，待机的时候也要灭灯
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getPowerLedLightEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置关机操作标志
 * 当执行关机操作时，设置为true；当没有执行关机操作时，设置为false
 * param[in] enable. XBH_TRUE：表示执行了关机操作，XBH_FALSE：表示没有执行关机操作
 * retval 0: success, -1: failure
 */
XBH_S32 XbhPlatformInterface::setShutdownFlag(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/***
 * 设置wifi频道代码
 * param[in] wifi_type wifi模组类型
 * param[in] channel wifi频道代码
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::setWifiChanelPlan(XBH_WIFI_TYPE wifiType, const XBH_CHAR* channel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 写入SFT工厂数据
 * param[in] data 数据
 * retval  0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSftData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取SFT工厂数据
 * param[out] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getSftData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 工厂需要存储产品的RkpStatus
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setRkpStatus(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 工厂需要存储产品的RkpStatus
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getRkpStatus(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 板控升级前的准备工作 prepareIncreaseBinUpgrade
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::prepareIncreaseBinUpgrade()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSmartPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的PN码，一个系列对应一个PN码
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getSmartPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* smart工厂需要存储产品的SN码，一个系列对应一个SN码
* param[in] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSmartSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的SN码，一个系列对应一个SN码
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getSmartSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* SMART希望罗盘关闭的状态能永久保存，复位不被清除
* param[in] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setCompassPresist(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * SMART希望罗盘关闭的状态能永久保存，复位不被清除
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getCompassPresist(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* smart工厂需要存储底板的PN码，一个系列对应一个PN码
* param[in] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSmartScalerPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储底板的PN码，一个系列对应一个PN码
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getSmartScalerPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* smart工厂需要存储底板的SN码，一个系列对应一个SN码
* param[in] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSmartScalerSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储底板的SN码，一个系列对应一个SN码
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getSmartScalerSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* smart工厂需要存储核心板的PN码，一个系列对应一个PN码
* param[in] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSmartCorePnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储核心板的PN码，一个系列对应一个PN码
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getSmartCorePnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* smart工厂需要存储核心板的SN码，一个系列对应一个SN码
* param[in] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setSmartCoreSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * smart工厂需要存储核心板的SN码，一个系列对应一个SN码
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getSmartCoreSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取天线的信号强度
* param[out] ver 数据, 数据格式如下：
* point[0]=-49, point[1]=-36 
* 如果是多根天线, 返回实际天线数量
* point[0]=-49, point[1]=-36, point[2]=-49, point[3]=-36 
* 范围: 0~-110db
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getAntennaRssi(XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置工厂MAC地址
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType, const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取工厂MAC地址
 * param[out] data 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getFactoryMac(XBH_MAC_ADDRESS_TYPE macType, XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 获取GoogleKey/Attestation/Device_ID序列号
 * param[out] deviceID 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getGoogleKeyID(XBH_CHAR* deviceID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 设置工厂SN
 * param[in] materialrype 物料类型 0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
 * param[in] snInfo 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::burnFactorySN(XBH_MATERAL_TYPE materialrype, const XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取工厂SN
 * param[out] materialrype 物料类型 0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
 * param[out] snInfo 数据
 * retval 0:success,-1:failure
 */
XBH_S32 XbhPlatformInterface::getBurnFactorySN(XBH_MATERAL_TYPE materialrype, XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhPlatformInterface::setTypeCPdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/*
* 获取edid机型名
* param[in] pBuff 要读取的实际数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getEdidModelName(std::string &pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/*
* 获取edid序列号
* param[in] serialArr 序列号buff
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getEdidSn(std::string &pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**********************************************************************************************OPS Bypass************************************************************************************************/
/**
* 获取OPS回传的数据情况
* param[out] OPS回传数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getOPSBypassDataLen(XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* OPS透传数据
* param[in] OPS类型
* param[in] 数据长度
* param[out] OPS回传数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::SendOPSBypassData(XBH_SOURCE_E type, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 将RTC数据保存到不可擦除分区
* param[in] RTC数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::setReferRTCInfo(const XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 从不可擦除分区读取RTC数据
* param[out] 回传数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getReferRTCInfo(XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 通过读取节点获取KTC触摸框必要信息绕过EDLA权限问题
* param[out] 回传数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhPlatformInterface::getKtcTouchInfo(XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取EMMC版本
 * @param pu32Version [out] 返回EMMC版本号(16进制)
 * @return XBH_SUCCESS 成功，XBH_FAILURE 失败
 */
XBH_S32 XbhPlatformInterface::getEmmcVersion(XBH_U32 *pu32Version)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取EMMC寿命（返回MLC和SLC中的最大值）
 * @param pu32LifeTime [out] 返回EMMC寿命值(16进制)
 * @return XBH_SUCCESS 成功，XBH_FAILURE 失败
 */
XBH_S32 XbhPlatformInterface::getEmmcLifeTime(XBH_U32 *pu32LifeTime)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
