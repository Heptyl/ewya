#ifndef XBH_CHIP_MN869129_H
#define XBH_CHIP_MN869129_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAVOutInterface.h"
#include <hardware/board.h>
#include "XbhService.h"
extern "C" {
#include "mn869129.h"
}

class ChipMn869129 : public XbhMWThread, public XbhAVOutInterface
{
//---------------- custom device interface start ----------------
public:
    /**
     * i2cBus       使用的IIC序号
     * Addr         芯片IIC地址
     * pGpio        芯片供电引脚
     * pLevel       供电工作时的电平状态
     * rGpio        芯片复位引脚
     * rLevel       复位工作时的电平状态
    */
    ChipMn869129(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipMn869129();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip();
    XBH_S32 resumeChip();
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
    * 设置HDMI TX的音频mute状态
     */
    XBH_S32 setHdmiTxAudioMute(XBH_BOOL bEnable);
    /**
    * 获取HDMI TX的音频mute状态
    */
    XBH_S32 getHdmiTxAudioMute(XBH_BOOL* info);
    /**
    * 设置HDMI TX的视频mute状态
     */
    XBH_S32 setHdmiTxVideoMute(XBH_BOOL bEnable);
    /**
    * 获取HDMI TX的视频mute状态
    */
    XBH_S32 getHdmiTxVideoMute(XBH_BOOL* info);
    /*
    * 设置HDMI TX的timing
    * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 setHdmiTxTiming(XBH_U8 value);
    /*
    * 获取HDMI TX的timing
    * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 getHdmiTxTiming(XBH_CHAR* info);
    /**
    * 设置HDMI TX的HDCP开关状态
     */
    XBH_S32 setHdmiTxHdcp(XBH_BOOL bEnable);
    /**
    * 获取HDMI TX的HDCP开关状态
    */
    XBH_S32 getHdmiTxHdcp(XBH_BOOL* info);

//---------------- custom device interface end ----------------
private:
    void run(const void* arg);
    static XbhMutex mLock;

    XBH_U32 mI2cNum;
    XBH_U32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;

    XBH_S32 mInitRet;
};

#endif //XBH_CHIP_MN869129_H
