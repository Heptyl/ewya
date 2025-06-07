#ifndef XBH_CHIP_ACM8625_STEREO_H
#define XBH_CHIP_ACM8625_STEREO_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhAudioAmplifierInterface.h"
#include "ChipAcm8625_FileParse.h"

class ChipAcm8625_Stereo : public XbhAudioAmplifierInterface
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
    ChipAcm8625_Stereo(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mute_gpio, XBH_S32 mute_level);
    ~ChipAcm8625_Stereo();

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

    XBH_S32 setEqEnable(XBH_BOOL enable);
    /**
     * 获取amp是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
private:
    XBH_S32 init();
    XBH_S32 Acm8625_Write(XBH_U8 *buf, XBH_U8 len);
    XBH_S32 Acm8625_Read(XBH_U8 *buf, XBH_U8 ctrl);
    XBH_S32 WriteIniConfig(const XBH_CHAR *fileName);
    XBH_S32 ParseIniConfig(const XBH_CHAR *section, XBH_CHAR *fileBuf, amp8625RegMap *pRegMap, XBH_U32 maxRegSize);
    XBH_S32 LoadIniFileBuf(const XBH_CHAR *fileName, XBH_CHAR *buf, XBH_S32 *fileSize, XBH_S32 bufSize);
    XBH_S32 writeRegMap(amp8625RegMap *reg_map, XBH_S32 iicBus, XBH_S32 iicAddr);
    
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

    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_ACM8625_STEREO_H
