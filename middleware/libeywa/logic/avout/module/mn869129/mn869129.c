#include "mn869129.h"

#include "reg_access.c"
#include "vbotx.c"
#include "vborx.c"
#include "hv_error.c"
#include "hdmitx.c"
#include "hdmirx.c"
#include "ddc.c"
#include "hdcp.c"
#include "spdif.c"
#include "system_ctrl.c"
#include "miscvborx.c"
#include "mischdmitx.c"
#include "LF9129_main.c"
#include "system_common.h"

XBH_S32 g_mI2cBus = -1;
XBH_S32 g_mI2cDev = -1;
XBH_BOOL g_mInitEnd = XBH_FALSE;
typedef system_mute generalMute;
static XBH_BOOL mCurrent_hdmitx_video_mute = XBH_FALSE; // 0: unmute, 1: mute
static XBH_BOOL mCurrent_hdmitx_audio_mute = XBH_FALSE; // 0: unmute, 1: mute


XBH_S32 Mn869219_resetHDMI(XBH_VOID)
{
    XBH_S32 ret = 0;

    MES("reset hdmi st ", ret);
    LF9129_reset_hdmi();
    MES("reset hdmi end ", ret);

    return ret;
}

XBH_S32 Mn869219_Init(XBH_VOID)
{
    XBH_S32 ret = 0;

    ret = LF9129_Init();
    g_mInitEnd = XBH_TRUE;
    MES("Init ret = ", ret);
    LF9129_open_hdmi();

    return ret;
}

XBH_S32 Mn869129_spdif(XBH_VOID)
{
    XBH_S32 ret = 0;
    ret = LF9129_initSPDIF();
    return ret;
}

XBH_S32 Mn869219_Check_State(XBH_VOID)
{
    XBH_S32 ret = 0;
    ret = LF9129_CheckState_Loop();
    return ret;
}

/*
* 设置HDMI TX的timing
* 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 Mn869219_Set_VideoFormat(XBH_S32 Format)
{
    XBH_S32 ret = 0;

    MES("%s, %d, Set Video Format = %d, Eg[ 0: 480P, 1: 1080P, 2: 2160P]", __func__, __LINE__, Format);

    if(Format == 0) 
    {
        ret = LF9129_Set_VideoFormat(SYSTEM_VFORM_720x480P_5994or60_4v3);
    }
    else if(Format == 1)
    {
        ret = LF9129_Set_VideoFormat(SYSTEM_VFORM_1920x1080P_5994or60_16v9);
    }
    else if(Format == 2)
    {
        ret = LF9129_Set_VideoFormat(SYSTEM_VFORM_3840x2160P_5994or60_16v9);
    }
    else
    {
        MES("Video format Auto");
    }
    ERR(API_ERROR_OCCURRED, "Mn869219_Set_VideoFormat", hv_strerr(ret));
    ret = !ret;
    return ret;
}

/*
* 获取HDMI TX的timing
* 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 Mn869219_Get_VideoFormat(XBH_CHAR* info)
{
    XBH_S32 ret = 0;
    MES("%s, %d, Set Video Format = %d, Eg[ 0: 480P, 1: 1080P, 2: 2160P]", __func__, __LINE__, *info);
    ret = LF9129_Get_VideoFormat(info);
    return ret;
}

/**
* 设置HDMI TX视频mute状态
    */
XBH_S32 Mn869219_Set_HdmiTxVedioMute(XBH_BOOL bEnable)
{
    XBH_S32 ret = 0;
    system_mute vedioMute;
    vedioMute.port = txp;

    if(bEnable)
    {
        vedioMute.mute = SYSTEM_TX_MUTE_ON_AVMUTE;
        ret = hdmitx_video_mute(&vedioMute);
        MES("%s, %d, Mute HDMI Tx video & audio", __func__, __LINE__);
        mCurrent_hdmitx_video_mute = XBH_TRUE;
    }
    else
    {
        vedioMute.mute = SYSTEM_TX_MUTE_OFF_AVMUTE;
        ret = hdmitx_video_mute(&vedioMute);
        MES("%s, %d, Unmute HDMI Tx video & audio", __func__, __LINE__);
        mCurrent_hdmitx_video_mute = XBH_FALSE;
    }

    return ret;
}

XBH_S32 Mn869219_Set_HdmiTxVedioMuteALL(XBH_BOOL bEnable)
{
    XBH_S32 ret = 0;
    system_mute vedioMute;
    vedioMute.port = txp;

    if(bEnable)
    {
        MES("%s, %d, Mute System video & audio", __func__, __LINE__);
        vedioMute.mute = SYSTEM_TX_MUTE_ON;
        ret = hdmitx_video_mute(&vedioMute);
        mCurrent_hdmitx_video_mute = XBH_TRUE;
    }
    else
    {
        MES("%s, %d, Unmute System video & audio", __func__, __LINE__);
        vedioMute.mute = SYSTEM_TX_MUTE_OFF;
        ret = hdmitx_video_mute(&vedioMute);
        mCurrent_hdmitx_video_mute = XBH_FALSE;
    }
    return ret;
}

/**
* 获取HDMI TX视频mute状态
    */
XBH_S32 Mn869219_Get_HdmiTxVedioMute(XBH_BOOL* info)
{
    *info = mCurrent_hdmitx_video_mute;
    MES("%s, %d, Get hdmi tx vedio Mute = %d", __func__, __LINE__, *info);
    return 0;
}

/**
* 设置HDMI TX的音频mute状态
    */
XBH_S32 Mn869219_Set_HdmiTxAudioMute(XBH_BOOL bEnable)
{
    XBH_S32 ret = 0;
    system_mute audioMute;
    audioMute.port = txp_spdif;

    if(bEnable)
    {
        MES("%s, %d, Mute hdmi tx Audio", __func__, __LINE__);
        audioMute.mute = SYSTEM_TX_MUTE_ON;
        ret = hdmitx_audio_mute(&audioMute);
        mCurrent_hdmitx_audio_mute = XBH_TRUE;
    }
    else
    {
        MES("%s, %d, Unmute hdmi tx Audio", __func__, __LINE__);
        audioMute.mute = SYSTEM_TX_MUTE_OFF;
        ret = hdmitx_audio_mute(&audioMute);
        mCurrent_hdmitx_audio_mute = XBH_FALSE;
    }
    return ret;
}

/**
* 获取HDMI TX的音频mute状态
    */
XBH_S32 Mn869219_Get_HdmiTxAudioMute(XBH_BOOL* info)
{
    *info = mCurrent_hdmitx_audio_mute;
    MES("%s, %d, Get mute hdmi tx Audio = %d", __func__, __LINE__, *info);

    return 0;
}

/**
* 设置HDMI TX的HDCP开关状态
    */
XBH_S32 Mn869219_Set_HdmiTxHDCPMute(XBH_BOOL bEnable)
{
    XBH_S32 ret = 0;

    if(bEnable)
    {
        MES("%s, %d, Mute hdmi tx HDCP ", __func__, __LINE__);
        ret = LF9129_HDCP_Mute(SYSTEM_TX_MUTE_ON);
    }
    else
    {
        MES("%s, %d, Unmute hdmi tx HDCP", __func__, __LINE__);
        ret = LF9129_HDCP_Mute(SYSTEM_TX_MUTE_OFF);
    }

    return ret;
}

/**
* 获取HDMI TX的HDCP开关状态
    */
XBH_S32 Mn869219_Get_HdmiTxHDCPMute(XBH_BOOL* info)
{
    XBH_S32 ret = 0;

    ret = LF9129_Get_HDCP_Mute(info);
    MES("%s, %d, Get mute hdmi tx HDCP = %d", __func__, __LINE__, *info);
    
    return ret;
}

XBH_S32 Mn869219_detect(XBH_VOID)
{
    XBH_S32 ret = 0;
    if (LF9129_detect())
    {
        MES("not found");
    }
    return ret;
}

XBH_S32 Mn869219_SetI2c(XBH_S32 i2cBus, XBH_S32 devAddr)
{
    XBH_S32 ret = 0;
    MES(" == Mn869219 SetI2c i2cBus  = %ld \n", i2cBus);
    g_mI2cBus = i2cBus;
    g_mI2cDev = devAddr;
    return ret;
}

#if 1
// C语言调用
#ifdef __cplusplus
extern "C" {
#endif
    XBH_S32 XbhC_setI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    XBH_S32 XbhC_getI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
#ifdef __cplusplus
}
#endif
#endif
XBH_U32 reg_write(XBH_U8 slaveaddr, XBH_U8 regaddr, XBH_U32 len, XBH_U8 *data)
{
    slaveaddr = (slaveaddr >> 1 & 0xff);
    if(XbhC_setI2cData(g_mI2cBus, slaveaddr, regaddr, 1, len, data) == -1)
    {
        LOGE(" I2C_WriteBytes error !!!!!");
        return 0;
    }
    return 0;
}

XBH_U32 reg_read(XBH_U8 slaveaddr, XBH_U8 regaddr, XBH_U32 len, XBH_U8 *data)
{
    slaveaddr = (slaveaddr >> 1 & 0xff);
    if(XbhC_getI2cData(g_mI2cBus, slaveaddr, regaddr, 1, len, data) == -1)
    {

        // LOGE(" I2C_ReadBytes error !!!!!");
        return 0;
    }
    return 0;
}

XBH_VOID SLEEP_Delay_ms(XBH_U32 x)
{
    usleep(x*1000);
}

