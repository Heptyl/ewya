#ifndef XBH_CHIP_AP8224_H
#define XBH_CHIP_AP8224_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAudioCodecInterface.h"

class ChipAp8224 : public XbhMWThread, public XbhAudioCodecInterface
{
public:
    /**
     *     
     * iicBus;           //device iic bus，不使用时填-1
     * iicAddr;          //device iic address
     * uart;             //如果使用串口通信，填使用的soc的uart的id，不使用时填-1
     * pGpio;            //供电引脚，没有就填-1
     * pLevel;           //工作时的电平状态
     * rGpio;            //复位引脚，没有就填-1
     * rLevel;           //工作时的电平状态
     * 模拟mic的插入检测引脚，一般用来处理插入MIC时的噪声,没有就填-1
     * mic_plug_gpio;    //mic plug in pin
     * mic_plug_level;   //mic plug in level
     * 使用codec做hp输出时使用,没有就填-1
     * hp_plug_gpio;     //headphone plug in pin
     * hp_plug_level;    //headphone plug in level
     * hp_mute_gpio;     //headphone mute pin
     * hp_mute_level;    //headphone mute level
    */
    ChipAp8224(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 uart, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mic_plug_gpio, XBH_S32 mic_plug_level, XBH_S32 hp_plug_gpio, XBH_S32 hp_plug_level, XBH_S32 hp_mute_gpio, XBH_S32 hp_mute_level);
    ~ChipAp8224();

    XBH_S32 resetChip();
    XBH_S32 getMicPlugIn(XBH_BOOL* enable);
    XBH_S32 getHeadphonePlugIn(XBH_BOOL* enable);
    XBH_S32 setMuteMicIn(XBH_BOOL mute);
    XBH_S32 getMuteMicIn(XBH_BOOL* mute);
    XBH_S32 setSpeakerOut(XBH_BOOL mute);
    XBH_S32 getSpeakerOut(XBH_BOOL* mute);
    XBH_S32 setMuteDAC0(XBH_BOOL mute);
    XBH_S32 getMuteDAC0(XBH_BOOL* mute);
    XBH_S32 setHeadphoneMute(XBH_BOOL mute);
    XBH_S32 getHeadphoneMute(XBH_BOOL* mute);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);

    XBH_S32 setLineOutGain(XBH_S32 gain);
    XBH_S32 getLineOutGain(XBH_S32* gain);
    XBH_S32 setMicInGain(XBH_S32 gain);
    XBH_S32 getMicInGain(XBH_S32* gain);
    XBH_S32 setMicHowling(XBH_BOOL enable);
    XBH_S32 getMicHowling(XBH_BOOL* enable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getChipExist(XBH_BOOL* enable);
public:

    //固件升级前SPI flah 的执行结果
    enum XBH_FLASH_WRITE_STATE_E {
        FLASH_UPGRADE_SUCCESS   = 0x01, //此帧数据升级成功，继续接收升级数据
        FLASH_UPGRADE_FAIL      = 0x02, //此帧数据升级失败，请重发当前升级数据
        FLASH_RESEND_TIMEOUT    = 0x03, //当前帧数据重发次数超过5次，升级失败，终止升级操作
        FLASH_UPGRADE_FINISH    = 0x04, //所有升级代码接收完成，MCU准备重启启动升级操作
        FLASH_CONNECT_TIMEOUT   = 0x05, //软件升级通讯超时（5秒）错误
        FLASH_WRITE_REPEAT      = 0x06, //此帧数据是重复升级数据帧
        FLASH_REWRIT_TIMEOUT    = 0x0A, //此帧数据写SPI Flash失败，请重发当前升级数据
        FLASH_WRITE_FAIL        = 0x0B //此帧数据写SPI Flash失败超过5次，终止软件升级操作
    };


private:
    void run(const void* arg);
    XBH_S32 init();
    XBH_S32 Ap8224_Write(XBH_U8 *buf, XBH_U8 len);
    XBH_S32 Ap8224_Read(XBH_U8 *buf, XBH_U8 ctrl, XBH_U8 len);
    XBH_S32 Ap8224_LoadEffect();
    XBH_S32 Ap8224_Mic4Mute(XBH_BOOL mute);
    //for upgradeFirmware
    XBH_BOOL Ap8224_SendFileByIIC(XBH_U32 offset, XBH_U8 *buff, XBH_U32 lenght, XBH_U8 *pRepyFlag);
    XBH_S32 Ap8224_SendFileSizeByIIC(XBH_U32 filesize);
    XBH_S32 i2c_data_write_nbytes(XBH_U32 u32RegAddr, XBH_U32 RegAddrLen, XBH_U8 *p_data, XBH_U32 data_len);
    XBH_S32 i2c_data_read_nbytes(XBH_U32 u32RegAddr, XBH_U32 RegAddrLen, XBH_U8 *p_data, XBH_U32 data_len);
    XBH_U16 GetCRC16NBS(XBH_U8* data, XBH_U32 length);
    XBH_S32 getFwAndEffsetVer(XBH_CHAR *FWVer, XBH_CHAR *EffsetVer);
    XBH_S32 getPresetFileData(XBH_U32* u32FileLen, XBH_U8** pFileBuff);
    XBH_S32 downloadPresetFirewareData(XBH_U8* pFileBuff, XBH_U32 u32FileLen);
    XBH_S32 startUpgradeFireware(XBH_VOID);
    XBH_S32 waitAP8224Reboot();
    XBH_S32 mState;
    XBH_CHAR mFileName[80];
    XBH_U8 m_u8TxBuff[256];
    XBH_U8 m_u8RxBuff[256];
    XBH_BOOL mForceUpgrade;

    static XbhMutex mLock;

    XBH_S32 mI2cBus;            //device iic bus，不使用时填-1
    XBH_S32 mI2cAddr;           //device iic address
    XBH_S32 mUart;              //如果使用串口通信，填使用的soc的uart的id，不使用时填-1
    //复位或者上电控制引脚
    XBH_S32 mPgpio;             //供电引脚，没有就填-1
    XBH_S32 mPLevel;            //工作时的电平状态
    XBH_S32 mRgpio;             //复位引脚，没有就填-1
    XBH_S32 mRLevel;            //工作时的电平状态
    //模拟mic的插入检测引脚，一般用来处理插入MIC时的噪声,没有就填-1
    XBH_S32 mMic_plug_gpio;     //mic plug in pin
    XBH_S32 mMic_plug_level;    //mic plug in level
    //使用codec做hp输出时使用,没有就填-1
    XBH_S32 mHp_plug_gpio;      //headphone plug in pin
    XBH_S32 mHp_plug_level;     //headphone plug in level
    XBH_S32 mHp_mute_gpio;      //headphone mute pin
    XBH_S32 mHp_mute_level;     //headphone mute level

    XBH_BOOL m_bInitDone;
    XBH_BOOL m_bChipExsit;
    XBH_BOOL m_bMicMute;
    XBH_BOOL m_bHpMute;
};

#endif //XBH_CHIP_AP8224_H
