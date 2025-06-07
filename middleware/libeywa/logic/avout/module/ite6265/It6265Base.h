#ifndef _IT6265_BASE_H_
#define _IT6265_BASE_H_
#include "XbhType.h"

typedef XBH_U8 u8;
typedef XBH_U16 u16;
typedef XBH_U32 u32;

//xbh patch start
#define hdmi_out_mute_mode          0
#define hdmi_out_sound_mute_mode    1
#define hdmi_out_hdcp_enable        0
#define hdmi_out_resolution         1
// #define DEBUG_SHOW_REG_RW           1
//xbh patch end

typedef struct
{
    u32 state[5];
    u32 count[2];
    u8 buffer[64];
} SHA1_CTX;

//--------------------  it6265_drv.h start   ---------------------------

typedef enum SysState_Type {
    SYS_Unplug = 0,
    SYS_HPD,
    SYS_WaitRxSen,
    SYS_RxSen,
    SYS_Unknown
} SysState_Type ;

typedef enum VidState_Type {
    Vid_Reset = 0,
    Vid_WaitForVidIn,
    Vid_VidInStable,
    Vid_VidOutSetup,
	Vid_VidPG,
    Vid_VidOutEnable,
    Vid_Unknown
} VidState_Type ;

typedef enum AudState_Type {
    Aud_Reset = 0,
    Aud_WaitForAudIn,
    Aud_AudInStable,
    Aud_AudOutSetup,
    Aud_AudOutEnable,
    Aud_Unknown
} AudState_Type ;

typedef enum HdcpState_Type {
    CP_Reset = 0,
    CP_Going,
    CP_Done,
    CP_Fail,
    CP_Unknown
} HdcpState_Type ;

typedef enum hdcp_cmd_type
{
    HDCP_NONE,
    HDCP_START,
    HDCP_AUTH_RETRY,
    HDCP_POLLING_BCAPS,
    HDCP_KSV_CHK,
}hdcp_cmd_type;

typedef enum avi_cmd_type
{
    AVICMD_NONE,
    AVICMD_WAIT_FOR_AUDIO,
    AVICMD_SYS_UNPLUG,
    AVICMD_SHOW_AUD_INFO,
    AVICMD_CHECK_VIDEO_IN_STABLE,
}avi_cmd_type;

typedef struct {
    u16 HActive ;
    u16 VActive ;
    u16 HTotal ;
    u16 VTotal ;
    u32 PCLK ;
    u16 HFrontPorch ;
    u16 HSyncWidth ;
    u16 HBackPorch ;
    u16 VFrontPorch ;
    u16 VSyncWidth ;
    u16 VBackPorch ;
    u32 FrameRate ;
    u8 ScanMode ;
    u8 VPolarity ;
    u8 HPolarity ;
    u8 PixRpt;
    u8 PicAR;
    char *format;
    u16 VIC;
} VTiming ;

struct _sink_cap {
    u8 YCCQCap;
    u8 RGBQCap;
    u8 ExtColCap;
    u8 HDR_EOTF;
    u8 HDMIVer;
    u8 max_TMDS1;
    u8 max_TMDS2;
    u8 max_TMDS_clk;

    // support 420
    u8 yuv420_svd_or_cmdb;

    // 2.0
    u8 scdc_present;
    u8 rr_capable;
    u8 lte_340M_csc_scramble;
    u8 independent_view;
    u8 daul_view2;
    u8 osd_3d_disparity;
    u8 dc_48bit_420;
    u8 dc_36bit_420;
    u8 dc_30bit_420;

    // 1.4
    //u8 support_ai;
    u8 dc_48bit;
    u8 dc_36bit;
    u8 dc_30bit;
    u8 dc_y444;
    //u8 daul_view1;
};

typedef struct it6265dev {
	struct {
        u8 InColorMode;
        u8 OutColorMode;
        u8 EnHDMI;
        u8 ColorDepth;
        u8 YCbCrCoef;
        u8 DynRange;
        u8 ColorClip;
        u8 EnInDDR;
        u8 En24BDDR;
        u8 EnBlueScrMute;
        u8 EnExtCol;
        u8 ChkExtCol;
        u8 ExtColFmt;

        u8 En3D;
        u8 EnDV3D;
        u8 EnOSD3D;
        u8 EnIVS3D;
        u8 Sel3DFmt;

        u8 En4Kx2K;

        // Dynamic Range and Mastering InfoFrame
        u8 EnDRMInfo;
        u8 DRMEOTF;
        u8 DRMZERO;

        // Audio Option
        u8 AudFullPkt;
        u8 AudEn_ori;
        u8 AudSel;
        u8 AudFmt;
        u8 AudCh;
        u8 AudType;
        u8 AudTypFmt;
        u8 AudTypSWL;
        u8 EnhSpdifMode;
        u8 EnNoAud2Mute;
        u8 EnMCLKSample;
        u8 MCLKFreq;
        u8 EnhAudCTS;
        u8 AudCTSOpt;

        u8 EnTDM;
        u8 TDMCh;
        u8 En3DAud;
        u8 EnMSAud;

        // VBORX Option
        u8 EnLxCLKInv ;
        u8 EnSCLKInv ;
        u8 EnPCLKInv ;
        u8 SCLKDly ;
        u8 EnH2InLBBypass ;
        u8 EnRxDEOnly ;
        u8 EnPiRecParaChgInt ;
		u8 EnDVMode;
        u8 SwapModeSel;

        // hdcp
        u8 DisR0ShortRead;
        u8 DisRiShortRead;
        u8 EnHDCP;

        u8 EnRepWaitTxMgm;  // whwn HDCP2Tx as a repeater, wait upstream Tx send Mgm
        u8 EnRepWaitTxEks;  // whwn HDCP2Tx as a repeater, wait upstream Tx send Eks

        u8 EnHDCP2ListSRMChk;
        u8 EnHDCP2ATC;

        u8 DDCSpeed ;
        u8 EnHDCP1p1;

        u8 EnH2Enc ;
        u8 EnH2Scr ;
        u8 H2ClkRatio ;
        u8 SCDCFlagByte ;
        u8 FlagPollSel ;

        u8 EnDESSC;
        u8 CKSOURCE;

        u8 EnChgAVIPR;
        u8 DisLockPR;

        u8 EnDEOnly;

        u8 AutoVIC;
        u8 IsHdcpRepeater;

        u8 keep_vborx_connection;
        u8 keep_vborx_connection_prev;

        u8 EnIntPG;
    }cfg;

    struct _sink_cap sink;

	struct {
        u32 PCLK;
        u32 RCLK;
        u32 ICLK;
        u32 VCLK;
        u32 TXCLK;
        u8 PixRpt;

        u8 RegPCLKDiv2;

        u8 VBOByteMode;
        u8 VBOLaneNum;

        u8 bytenum;
        u8 lanenum;

        u8 AFESPEED;
        u8 HDMI_VIC;

        VidState_Type VState;
        AudState_Type AState;
        HdcpState_Type HdcpState;
        SysState_Type SysState;

        u16 HActive;
        u16 VActive;

        u8 HDCP2Sel;
        u8 RxHDMIMode;
        u8 HDCPFireCnt;
        u8 param_chg_cnt;
        u8 fifo_err_count;
        u8 fifo_err_handled;
        u8 rid;

        enum hdcp_cmd_type hdcp_cmd;
        u16 hdcp_cmd_count;
        u16 hdcp_cmd_tick;
        u16 hdcp_cmd_timeout;

        enum avi_cmd_type avi_cmd;
        u16 avi_cmd_tick;
        u16 avi_cmd_timeout;
        u8 sys_tick_inc;
        u8 sys_tick_enabled;

        #if ENABLE_CEC
        u8 cec_time_unit;
        u8 phy_addrL;
        u8 phy_addrH;
        #endif

        u8 rx_key[5];
        u8 hdcp2_fail_count;
        u16 hdcp2_fail_status;
        u16 hdcp2_fail_status_prev;

        u8 reset_vborx;
        u8 prev_hpd;
        u8 hdcp_going;
        u8 prev_clock_ratio;

    }vars;

    VTiming input;

    #if ENABLE_CALLBACK
    it6265_callbacks cb;
    it6265_callback_data cb_data;
    #endif

    u8 data_buffer[64+80*4]; // for RxID list, edid parsing and sha calc

} IT6265_DEV_CTX, *pIT6265_DEV_CTX;

//extern struct it6265dev g_dev;

//--------------------  it6265_drv.h end     ---------------------------
//--------------------  it6265.h start       ---------------------------
typedef enum it6265_video_fmt
{
    VF_4K2K60,
    VF_4K2K30,
    VF_1080p,
    VF_720p,
    VF_480p,

}it6265_video_fmt;
//--------------------  it6265_drv.h end     ---------------------------

//--------------------  it6265_cec.c start   ---------------------------

typedef struct
{
    u8 Size;
    u8 Header;
    u8 Opcode;
    u8 operand[16];
}CEC_FRAME, *P_CEC_FRAME;

typedef enum _SystemInfoFeature_StateType
{
    sCECVersioin            =0x9E,
    sGetCECVersion            =0x9F,
    sGivePhysicalAddress    =0x83,    // follower:All
    sGetMenuLanguage        =0x91,    // follower:TV
    sReportPhysicalAddress    =0x84,    // follower:TV
    sSetMenuLanguage        =0x32,    // follower:All,        Initiator:TV
    sSIFnone
} SystemInfoFeature_StateType;


typedef enum _SystemAudioControl_StateType
{
    sComesOutOfStandby=0,
    sSetSystemAudioModeToTV,
    sWaitingFeatureAbort,
    sSetSystemAudioModeToAll,
    sSACnone
} SystemAudioControl_StateType;


typedef enum _OneTouchPlay_StateType
{
    sActiveSource,
    sImageViewOn,
    sTextViewOn,
    sOTNone
} OneTouchPlay_StateType;

typedef enum _LogicAddressing_StateType
{
    sReportPhysicalAddressTransfer,
    sReportPhysicalAddressReceived,
    sPollingMessage,
    sPollingResult,
    sLANone
} LogicAddressing_StateType;


typedef enum _PollingMessage_StateType
{
    sPollingMessage1,
    sPollingMessage2,
    sPollingMessage3,
    sReportPhyAdr,
    sReportDeviceVendorID,
    sPMnone
} PollingMessage_StateType;

typedef enum _CEC_Logic_DEVICE_ID_t
{
    DEVICE_ID_TV=0,
    DEVICE_ID_RECORDING1,        // 1
    DEVICE_ID_RECORDING2,        // 2
    DEVICE_ID_TUNER1,            // 3
    DEVICE_ID_PLAYBACK1,        // 4
    DEVICE_ID_AUDIO,            // 5
    DEVICE_ID_TUNER2,            // 6
    DEVICE_ID_TUNER3,            // 7
    DEVICE_ID_PLAYBACK2,        // 8
    DEVICE_ID_RECORDING3,        // 9
    DEVICE_ID_TUNER4,            // 10
    DEVICE_ID_PLAYBACK3,        // 11
    DEVICE_ID_RESERVED1,        // 12
    DEVICE_ID_RESERVED2,        // 13
    DEVICE_ID_FREEUSE,            // 14
    DEVICE_ID_BROADCAST,        // 15
    DEVICE_ID_MAX
} CEC_Logic_DEVICE_ID_t;

typedef enum _CEC_DEVICE_TYPE
{
    CEC_DEV_TYPE_TV=0,
    CEC_DEV_TYPE_RECORDING_DEV=1,
    CEC_DEV_TYPE_RESERVED=2,
    CEC_DEV_TYPE_TUNER=3,
    CEC_DEV_TYPE_PLAYBACK_DEV=4,
    CEC_DEV_TYPE_AUDIO_SYS=5,
    CEC_DEV_TYPE_PURE_CEC_SWITCH=6,
    CEC_DEV_TYPE_VIDEO_PROCESSOR=7
} CEC_DEVICE_TYPE;

 typedef struct {
//     CEC_Logic_DEVICE_ID_t    LogicDevice;
    u8                        PhyicalAddr1;
    u8                        PhyicalAddr2;
    u8                        Active;
 }CECDevice, *pCECDevice;

typedef enum{
    eUnrecognizedOpcode = 0,
    eNotIncorrectModeToRespond = 1,
    eCannotProvideSource = 2,
    eInvalidOperand = 3,
    eRefused = 4,
    eUnableToDetermine = 5
}CecAbortReson;

typedef enum _CEC_COMMAND
{
    eFeatureAbort            =0x00,

// One Touch Play Feature
    eActiveSource            =0x82,    // follower:TV, switch    --> Broadcst , Directly address
    eImageViewOn            =0x04,    // follower:TV, switch    --> Broadcst
    eTextViewOn            =0x0D,    // follower:TV

// Routing Control Feature
    eRoutingChange            =0x80,    // follower:TV, switch    --> Broadcst
    eRoutingInformation        =0x81,    // follower:switch        --> Broadcst
    eRequestActiveSource    =0x85,    // follower:switch        --> Broadcst     , AVR to request active source when Audio ocntrol feature is active
    eSetStreamPath            =0x86,    // follower:switch        --> Broadcst
    eInactiveSource            =0x9D,    //
    eStandBy                =0x36,    // follower:All            --> Broadcst

// System Information Feature
    eCECVersioin            =0x9E,
    eGetCECVersion            =0x9F,
    eGivePhysicalAddress    =0x83,    // follower:All
    eGetMenuLanguage        =0x91,    // follower:TV
    eReportPhysicalAddress    =0x84,    // follower:TV
    eSetMenuLanguage        =0x32,    // follower:All,        Initiator:TV

// Vendor Specific Commands Feature
    eDeviceVendorID        =0x87,
    eGiveDeviceVendorID    =0x8C,
    eVendorCommand        =0x89,
    eGiveDevicePowerStatus    =0x8F,
    eReportPowerStatus        =0x90,
    eVendorCommandWithID =0xA0,
    eRemoteButtonDown        =0x8A,
    eRemoteButtonUp        =0x8B,

// Deck Control Feature
    ePlay                    =0x41,
    eDeckControl            =0x42,
    eDeckStatus                =0x1B,
    eGiveDeckStatus            =0x1A,
// other
    eUserPressed            =0x44,
    eUserReleased            =0x45,
    eAudioModeRequest        =0x70,
    eGiveAudioStatus        =0x71,
    eSetSystemAudioMode    =0x72,
    eReportAudioStatus        =0x7A,
    eGiveSystemAudIoModeStatus    =0x7D,
    eSystemAudIoModeStatus        =0x7E,
    eReportAudioDescriptor     =0xA3,
    eRequestAudioDescriptor =0xA4,

// ACR
    eInitiateARC                =0xC0,
    eReportARCInitiated        =0xC1,
    eReportARCTerminated    =0xC2,
    eRequestARCInitiation    =0xC3,
    eRequestARCTermination    =0xC4,
    eTerminateARC            =0xC5,

// HEC
    eCDC                    =0xF8,

// Polling Mesage
    ePollingMessage,

    eAbort                    =0xff,

    eCECNone

} eCEC_CMD;

typedef enum _CDC_COMMAND
{
    eHEC_InquirState            =0x00,
    eHEC_ReportState            =0x01,
    eHEC_SetStateAdjacent        =0x02,
    eHEC_SetState                =0x03,
    eHEC_RequestDeactivation    =0x04,
    eHEC_NotifyAlive            =0x05,
    eHEC_Discover                =0x06,
    eHEC_HPDSetState            =0x10,
    eHEC_HPDReportState        =0x11
}eCDC_CMD;

typedef union {
    struct
    {
        u8     AudioVolumeStatus    :7;    //bit 0~6    Volume status
        u8    AudioMuteStatus        :1;    //bit 7     Mute status
    }Map;

    u8 AStatus;
}CECAudioStatus;

typedef union {
    struct
    {
        u8     AudioFormatCode    :6;    //bit 0~5    Audio Format Code
        u8    AudioFormatID        :2;    //bit 6~7     Audio Format ID
    }Map;

    u8 AStatus;
}CECAudioFormatID;

typedef enum {
    eDirectly=0,
    eBroadcast=1,
    eBoth=2
}HeaderType;

typedef enum{
    eCDCFeature = 0,
    eOTPFeature,
    eRoutingCtlFeature,
    eStandByFeature,
    eSysInfoFeature,
    eVenSpecCmdFeature,
    eRemoteCtlPassthroughFeature,
    ePowerStatusFeature,
    eSysAudioCtlFeature,
    eACRFeature,
    eHECFeature,
    eDeckCtlFeature,
    eNoneFeature
}CECOpCodeType;

typedef struct {
    eCEC_CMD cmd;
    HeaderType header;
    u8    size;
    CECOpCodeType eType;
    char *CmdString;
} CECcmdFormat;

/* Circular buffer object */
typedef struct {
    int         size;    /* maximum number of elements           */
    int         start;   /* index of oldest element              */
    int         end;     /* index at which to write new element  */
    CEC_FRAME   *elems;  /* vector of elements                   */
} circular_buffer;
//--------------------  it6265_cec.c end     ---------------------------
#endif