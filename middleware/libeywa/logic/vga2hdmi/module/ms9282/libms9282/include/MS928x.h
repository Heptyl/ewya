/**
******************************************************************************
* @file    MS928X.h
* @author  
* @version V2.2.0
* @date    15-Oct-2013
* @brief   Definitions for MS928X library. 
* @history 11-Nov-2013 Update for MS928X.
*
* Copyright (c) 2009-2013, MacroSilicon Technology Co.,Ltd.
******************************************************************************/

#ifndef _MACROSILICON_TECH_MS928X_H_
#define _MACROSILICON_TECH_MS928X_H_

#include "mst_typedef.h"
// MS928X SDK version.
#define MS928X_MAJOR_VER            (0x02)
#define MS928X_MINOR_VER            (0x07)

#define MS928X_I2C_ADDR_92          (0x49)
#define MS928X_I2C_ADDR_9A          (0x4D)

//
// Video Input sources.
//
typedef enum _ms928x_video_input_format
{
    IN_ANALOG_VGA           = 0x1,
    IN_ANALOG_YPBPR         = 0x2,
    IN_ANALOG_RGBS          = 0x3,
    IN_ANALOG_RGB_CS        = 0x4,
    IN_DIGITAL_RGB          = 0x5,
    IN_DIGITAL_YCBCR444     = 0x6,
    IN_DIGITAL_YCBCR422     = 0x7
}eVideoSource;

typedef enum _ms928x_output_color_space
{
    OUT_CS_RGB      = 0,
    OUT_CS_YCBCR444 = 1,
    OUT_CS_YCBCR422 = 2
}eOutColorSpace;

// Video Configurations
enum _ms928x_system_configuration
{
    SYS_CFG_DEFAULT             = 0,        // Default configuration.
    // Video
    SYS_CFG_SIG_CHANNEL_1       = 0x01,     // Select channel1 according to system board design
    SYS_CFG_SIG_SYNC_SOG_1      = 0x02,     // Select SYNC1 and SOG1
    SYS_CFG_OUT_COLOR_MODE      = 0x04,     // output color space user define flag
    SYS_CFG_OUT_MODE_DVI        = 0x08,     // Output sink Mode user define flag, default DVI

    //
    SYS_CFG_VFE_PLL_OPTION      = 0x10,     // option VFE pll flag

    SYS_CFG_DDC_DISABLED        = 0x20,     // hdmi tx ddc unuse flag
    
    // For audio
    SYS_CFG_RIGHT_JUSTIFY       = 0x40,     // 0:Left-justify, 1:Right-Justify;

    //
    SYS_CFG_VP_OPTION           = 0x80,     // option vertical position precise flag

    // For Digital-In
    SYS_CFG_DDR                 = 0x0100,   // no use
    SYS_CFG_EMBEDDED_SYNC       = 0x0200,   // no use, For BT.656 

    SYS_CFG_USER_VIC            = 0x0400,   // user define VIC

    SYS_CFG_AUTO_ADJUST_DISABLE = 0x0800,   // disable auto phase and border detection
    
    SYS_CFG_MONO_COLOR_ENABLE   = 0x1000,   // enable mono color function

    SYS_CFG_ADC_HIGH_BANDWIDTH  = 0x2000,   // adc select high bandwidth

    SYS_CFG_ADDPLL_HIGH_REFCLK  = 0x4000,   // addpll select high reference clock

    SYS_CFG_RESERVED            = 0x8000    // Reserved.     
};

//
// Define the severity status code.
//
#define STATE_SUCCESS                   (0)   
#define STATE_FATAL_ERR                 (0x80)
#define STATE_CHIP_NOT_EXIST            (0x81)
#define STATE_HPD_ERR                   (0x82)
#define STATE_READ_EDID_ERR             (0x83)
#define STATE_MODE_NOT_CHANGED          (0x84)
#define STATE_NO_INPUT_SIGNAL           (0x85)
#define STATE_INVALID_PARAMETER         (0x86)
#define STATE_UNKNOWN                   (0xFF)

//
// Audio Module, please refer to HDMI 1.3a spec.
//
#define MS928X_AUDIO_DEFAULT            (0x0)   // Default audio: I2S_48K_2CH_16BITS    

// Select audio source.
#define MS928X_AUD_MODE_I2S             (0x1)
#define MS928X_AUD_MODE_SPDIF           (0x2)

// Sample length
#define MS928X_AUD_LEN_16               (0x2)
#define MS928X_AUD_LEN_24               (0xB)
// Sample Channel
#define MS928X_AUD_CH_2CH               (0x1)
#define MS928X_AUD_CH_8CH               (0x7)  
// Sample rate 
#define MS928X_AUD_RATE_44K1            (0x0)
#define MS928X_AUD_RATE_48K             (0x2)
#define MS928X_AUD_RATE_32K             (0x3)
#define MS928X_AUD_RATE_88K2            (0x8)
#define MS928X_AUD_RATE_96K             (0xA)
#define MS928X_AUD_RATE_176K4           (0xC)
#define MS928X_AUD_RATE_192K            (0xE)

// 
// Defines interface for video/audio
// 
typedef struct _ms928x_video_param
{
    eVideoSource u8sourceIn;
}VIDEOPARAM_T;

typedef struct _ms928x_audio_param
{    
    UINT8        u8audMode;
    UINT8        u8audRate;
    UINT8        u8audBits;
    UINT8        u8audChannel;
}AUDIOPARAM_T;

// system parameters.
typedef struct _ms928x_system_param
{
    UINT8           u8chipAddr;
    UINT16          u16sysConfig;
    VIDEOPARAM_T    videoParam;
    AUDIOPARAM_T    audioParam;    
    UINT8           u8userVIC;     // [Digital-In Only]. User will be responsible for setting the VIC.   
    UINT8           u8HDMIType;    // DVI : 0;  HDMI: 1
    UINT8           u8HDMIColor;   //refer to eOutColorSpace
    UINT8           u8Phase;       //from 0~31
    UINT8           u8MnccGain;    //set color fix range: 0x10~0x1F(16~31)
}SYSPARAM_T;

#ifdef __cplusplus
extern "C" {
#endif

extern VOID   MS928X_SetI2c(INT32 i2cBus, INT32 devAddr);

// MS928X chip reset and initialization.
UINT8   MS928X_Init(SYSPARAM_T* pstSysParam);

// Determines the running status of the current system.
UINT8   MS928X_Poll(VOID);

//     
VOID    MS928X_SetVideo(VOID);
// Note: This function should be called after the MS928X_SetVideo function.
VOID    MS928X_SetAudio(VOID);  
    
// Operations
VOID    MS928X_Video_Mute(BOOL bmuteON);
VOID    MS928X_Audio_Mute(BOOL bmuteON);
    
// Returns TRUE if the sink device connected.
BOOL    MS928X_HPD_Detect(VOID);

// Returns TRUE if MS928x chip exists.
BOOL    MS928X_IsValid(VOID);

// Switch I2S/SPDIF audio.
UINT8   MS928X_UpdateAudio(AUDIOPARAM_T* pstAudParam);   

// Get current input video mode.
// Returns the corresponding VIC code. Returns 0xFF if no signal detected.
UINT8   MS928X_GetInputVideoMode(VOID);

// HDMI DDC access enable
VOID    MS928X_DDC_Enable(BOOL b_enable);

// Horizontal or vertical pixel shift
VOID    MS928X_ShiftConfig(UINT8 u8_h_position, UINT8 u8_v_position);

// Do auto phase, or set manual phase
UINT8   MS928X_PhaseConfig(BOOL b_auto_enable, UINT8 u8_phase_sel);

// Adjust brightness
VOID    MS928X_SetVideoBrightness(INT8 i8_brightness);

// Adjust contrast
VOID    MS928X_SetVideoContrast(UINT8 u8_contrast);

// Adjust tx data driver(max = 0xf)
VOID  MS928X_SetHdmiDataDriver(UINT8 u8_main_driver, UINT8 u8_post_driver);

#ifdef __cplusplus
}
#endif

#endif // _MACROSILICON_TECH_MS928X_H_

