
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipMn869129_InputSource"
#include "XbhLog.h"


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "ChipMn869129.h"
#include "XbhService.h"

XbhMutex                     ChipMn869129::mLock;
XBH_S32                      xbh_vbo2hdmi_reset_key = 1;
XBH_U8                       xbh_mn869129_active = 0;
XBH_S32                      panelDiv= 0;
XBH_BOOL                     mAVMute= 0;
XBH_U8                       mMaxVideoFormat= 0;
XBH_U8                       mCurrent_hdmitx_video_format = 0;
static XBH_U8                reset_state = 0;


#ifndef ENABLE_MN869129_VBO_INIT_IN_EYWA
#define ENABLE_MN869129_VBO_INIT_IN_EYWA            0
#endif

XBH_S32 ChipMn869129::resetChip()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(!mInitRet) // 证明有外挂Mn869129 IC，才会执行此函数
    {
        Mn869219_Set_HdmiTxVedioMuteALL(true);
        usleep(20 * 1000);
        Mn869219_resetHDMI();
        XLOGD("Myh reset gpio number is %d\n", mRGpio);
        s32Ret |= XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        XLOGD("MYH Mn869129 reset chip\n");
        reset_state = 1;
    }
    else
    {
        XLOGD("MYH Mn869129 not active\n");
    }

    return s32Ret;
}

XBH_S32 ChipMn869129::resumeChip()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (reset_state && (!mInitRet)) // 证明有外挂Mn869129 IC，且之前执行过resetChip才会执行此函数
    {
        XLOGD("MYH Mn869129 resume chip  st");

        // usleep(700* 1000);

        s32Ret |= XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);

        usleep(20* 1000);
        Mn869219_Init();

        XLOGD("MYH Mn869129 resume chip end\n");
        reset_state = 0;
    }
    else
    {
        XLOGD("Mn869129 not reset\n");
    }

    return s32Ret;
}

XBH_S32 ChipMn869129::getChipExist(XBH_BOOL* bEnable)
{
    if(mInitRet == XBH_SUCCESS)
    {
        *bEnable = XBH_TRUE;
    }
    else
    {
        *bEnable = XBH_FALSE;
    }
    return XBH_SUCCESS;
}

XBH_S32 ChipMn869129::getChipInitDone(XBH_BOOL* bEnable)
{
    return XBH_SUCCESS;
}

XBH_S32 ChipMn869129::getFirmwareVersion(XBH_CHAR* strVersion)
{
    return XBH_SUCCESS;
}

/**
* 设置HDMI TX的音频mute状态
    */
XBH_S32 ChipMn869129::setHdmiTxAudioMute(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Set_HdmiTxAudioMute(bEnable);
    return  s32Ret;
}

/**
* 获取HDMI TX的音频mute状态
    */
XBH_S32 ChipMn869129::getHdmiTxAudioMute(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Get_HdmiTxAudioMute(info);
    return  s32Ret;
}

/**
* 设置HDMI TX的视频mute状态
    */
XBH_S32 ChipMn869129::setHdmiTxVideoMute(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Set_HdmiTxVedioMute(bEnable);
    XLOGD("Mn869129 ready set avmute state\n");
    XbhService::getPlatformInterface()->setHdmiTxVideoMuteState(bEnable);
    usleep(50 * 1000);
    XbhService::getPlatformInterface()->getHdmiTxVideoMuteState(&mAVMute);
    XLOGD("Mn869129 avmute state is %d\n", mAVMute);
    return  s32Ret;
}

/**
* 获取HDMI TX的视频mute状态
    */
XBH_S32 ChipMn869129::getHdmiTxVideoMute(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Get_HdmiTxVedioMute(info);
    return  s32Ret;
}

/*
* 设置HDMI TX的timing
* 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 ChipMn869129::setHdmiTxTiming(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if( (value <= mMaxVideoFormat) && (value != mCurrent_hdmitx_video_format) )
    {
        s32Ret = Mn869219_Set_VideoFormat(value) ? XBH_SUCCESS : XBH_FAILURE;
    }
    else
    {
        XLOGD("%s, %d, set value greater than max support video format\n", __func__, __LINE__);
    }

    return  s32Ret;
}

/*
* 获取HDMI TX的timing
* 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 ChipMn869129::getHdmiTxTiming(XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Get_VideoFormat(info);
    return  s32Ret;
}

/**
* 设置HDMI TX的HDCP开关状态
    */
XBH_S32 ChipMn869129::setHdmiTxHdcp(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Set_HdmiTxHDCPMute(bEnable);
    return  s32Ret;
}

/**
* 获取HDMI TX的HDCP开关状态
    */
XBH_S32 ChipMn869129::getHdmiTxHdcp(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = Mn869219_Get_HdmiTxHDCPMute(info);
    return  s32Ret;
}

void ChipMn869129::run(const void* arg)
{
    Mn869219_Check_State();
    usleep(500 * 1000);
}

ChipMn869129::ChipMn869129(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    mInitRet = XBH_FAILURE;
    XLOGD(" begin ");
    XBH_S32 s32Ret = XBH_FAILURE;
    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    Mn869219_SetI2c(mI2cNum, mI2cAddr);

    XBH_U8 buf[4] = {0};
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, 0x02, 1, 2, buf);
    if(buf[0] == 0x04 && buf[1] == 0x05)
    {
        mInitRet = XBH_SUCCESS;
        XLOGD("Mn869129 get device id Success\n");
    }
    else
    {
        XLOGE("Mn869129 get device id Failure\n");
        return ;
    }
    // if (pGpio != -1) {
    //     XbhService::getModuleInterface()->setGpioOutputValue(pGpio, pLevel);
    //     usleep(50 * 1000);
    // }

    // if (rGpio != -1) {
    //     XbhService::getModuleInterface()->setGpioOutputValue(rGpio, !rLevel);
    //     usleep(50 * 1000);
    //     XbhService::getModuleInterface()->setGpioOutputValue(rGpio, rLevel);
    // }
    // XbhService::getModuleInterface()->setSocFreeRun(XBH_TRUE);
    XLOGD("Mn869129 ready get panel div\n");
    XbhService::getPlatformInterface()->getMultiPanel(&panelDiv, XBH_PANEL_MAIN);
    XLOGD("Mn869129 panel div is %d\n", panelDiv);

    XLOGD("Mn869129 ready get avmute state\n");
    XbhService::getPlatformInterface()->getHdmiTxVideoMuteState(&mAVMute);
    XLOGD("Mn869129 avmute state is %d\n", mAVMute);

    // 起初在Eywa中加载驱动，但是由于Eywa启动太慢，导致开机没信号阶段整机会红绿屏，故将驱动移植到bootloader中
    if(ENABLE_MN869129_VBO_INIT_IN_EYWA)
    {
        Mn869219_Init();
    }
    else
    {
        Mn869129_spdif();
    }
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

ChipMn869129::~ChipMn869129()
{

}

