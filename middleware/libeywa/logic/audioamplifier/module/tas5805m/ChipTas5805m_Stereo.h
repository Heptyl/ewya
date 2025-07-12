#ifndef XBH_CHIP_TAS5805M_STEREO_H
#define XBH_CHIP_Tas5805M_STEREO_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhAudioAmplifierInterface.h"
#include "ChipTas5805m_FileParse.h"

enum XBH_SOUND_TYPE
{
    XBH_SOUND_TYPE_BASS    = 0,
    XBH_SOUND_TYPE_TREBLE  = 1,
    XBH_SOUND_TYPE_BALANCE = 2,
};

enum XBH_AMP_WRITE_STATUS
{
    XBH_AMP_IDLE        = 0,   //空闲
    XBH_AMP_RUNNING     = 1,   //工作中
    XBH_AMP_SUCCESS     = 2,   //执行成功
    XBH_AMP_FAILURE     = 3,   //执行失败
};

class ChipTas5805m_Stereo : public XbhAudioAmplifierInterface
{
public:
    /**
     * iicBus;           //device iic bus
     * iicAddr;          //device iic address
     * pGpio;            //供电引脚，没有就填-1
     * pLevel;           //工作时的电平状态
     * rGpio;            //复位引脚，没有就填-1
     * rLevel;           //工作时的电平状态
     * mute_gpio;        //功放的MUTE引脚
     * mute_level;       //功放MUTE时的电平
    */
    ChipTas5805m_Stereo(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mute_gpio, XBH_S32 mute_level);
    ~ChipTas5805m_Stereo();

    XBH_S32 resetChip();
    XBH_S32 setMute(XBH_BOOL mute);
    XBH_S32 getMute(XBH_BOOL* mute);
    XBH_S32 setGain(XBH_S32 gain);
    XBH_S32 getGain(XBH_S32* gain);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);

    XBH_S32 setAvcEnable(XBH_BOOL isAvcEnable);
    XBH_S32 getAvcEnable(XBH_BOOL* isAvcEnable);
    /**
     * 获取amp是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    /**
    /**
    * 设置EQ曲线(频率分布曲线)开关，可以用来打开或者关闭该曲线用于测试
    * param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
    * retval 0:success,-1:failure
    */
    XBH_S32 setEqEnable(XBH_BOOL enable);
    /* 获取EQ曲线(频率分布曲线)开关
    * param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
    * retval 0:success,-1:failure
    */
    XBH_S32 getEqEnable(XBH_BOOL *enable);
    /**
    * 设置音频输出的前置增益
    * param[in] enEqMode. EQ阶段
    * param[out] s32Value. 增益值
    * retval 0:success,-1:failure
    */
    XBH_S32 setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value);
    /**
    * 获取音频输出的前置增益
    * param[in] enEqMode. EQ阶段
    * param[out] s32Value. 增益值
    * retval 0:success,-1:failure
    */
    XBH_S32 getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value);
    /**
     * 设置低音增益
    * param[in] s32Value 增益值
    * retval 0:success,-1:failure
    */
    XBH_S32 setBass(XBH_S32 s32Value);
    /**
     * 获取低音增益
    * param[out] s32Value 增益值
    * retval 0:success,-1:failure
    */
    XBH_S32 getBass(XBH_S32 *s32Value);
    /**
    * 设置高音增益
    * param[in] s32Value 增益值
    * retval 0:success,-1:failure
    */
    XBH_S32 setTreble(XBH_S32 s32Value);
    /**
    * 获取高音增益
    * param[out] s32Value 增益值
    * retval 0:success,-1:failure
    */
    XBH_S32 getTreble(XBH_S32 *s32Value);
    /**
    * 设置左右声道平衡
    * param[in] s32Value 平衡值 【-50~+50】
    * retval 0:success,-1:failure
    */
    XBH_S32 setBalance(XBH_S32 s32Value);
    /**
    * 获取左右声道平衡
    * param[out] s32Value 平衡值 【-50~+50】
    * retval 0:success,-1:failure
    */
    XBH_S32 getBalance(XBH_S32 *s32Value);
    /**
    * 设置声音模式
    * param[in] s32Value
    * retval 0:success,-1:failure
    */
    XBH_S32 setSoundMode(XBH_SOUND_MODE_E enSoundMode);
    /**
    * 获取声音模式
    * param[out] s32Value
    * retval 0:success,-1:failure
    */
    XBH_S32 getSoundMode(XBH_SOUND_MODE_E *enSoundMode);

    XBH_S32 matchlevel(XBH_SOUND_TYPE Type, XBH_S32 s32Value);
    XBH_S32 frqGainLevel(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value);
    XBH_S32 setEqReg(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value);
    /**
     * 设置环绕声开关
     * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
     * retval 0:success,-1:failure
    */
    XBH_S32 setSurroundEnable(XBH_BOOL bEnable);
    /**
     * 获取环绕声开关
     * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
     * retval 0:success,-1:failure
    */
    XBH_S32 getSurroundEnable(XBH_BOOL* bEnable);
private:
    XBH_S32 init();
    XBH_S32 Tas5805m_Write(XBH_U8 *buf, XBH_U8 len);
    XBH_S32 Tas5805m_Read(XBH_U8 *buf, XBH_U8 ctrl);
    XBH_S32 WriteIniConfig(const XBH_CHAR *fileName);
    XBH_S32 ParseIniConfig(const XBH_CHAR *section, XBH_CHAR *fileBuf, amp5805RegMap *pRegMap, XBH_U32 maxRegSize);
    XBH_S32 LoadIniFileBuf(const XBH_CHAR *fileName, XBH_CHAR *buf, XBH_S32 *fileSize, XBH_S32 bufSize);
    XBH_S32 writeRegMap(amp5805RegMap *reg_map, XBH_S32 iicBus, XBH_S32 iicAddr);
    XBH_S32 setEQfiles(XBH_CHAR EQ_Files[256]);

    static XbhMutex mLock;

    XBH_S32 mI2cBus;            //device iic bus，不使用时填-1
    XBH_S32 mI2cAddr;           //device iic address
    //复位或者上电控制引脚
    XBH_S32 mPgpio;             //供电引脚，没有就填-1
    XBH_S32 mPLevel;            //工作时的电平状态
    XBH_S32 mRgpio;             //复位引脚，没有就填-1
    XBH_S32 mRLevel;            //工作时的电平状态
    XBH_S32 mMute_gpio;         //mute pin
    XBH_S32 mMute_level;        //mute level
    XBH_S32 mWrite_amp_status;  //寄存器写eq状态

    XBH_BOOL m_bInitDone;
    XBH_BOOL mSurroundSoundStatus;    //保存环绕音的使能状态
};

#endif //XBH_CHIP_Tas5805m_STEREO_H
