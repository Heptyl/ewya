#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <errno.h>
#include <dirent.h>

#include <utils/Log.h>
#include "mst_typedef.h"
#include "MS928x.h"

#define TAG "xbh_mw@MS9282"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) 
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) 
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) 

 // MS928X chip reset and initialization.
UINT8 MS9282_MS928X_Init(SYSPARAM_T* pstSysParam);

// Determines the running status of the current system.
UINT8 MS9282_MS928X_Poll(void);

//     
void MS9282_MS928X_SetVideo(void);
// Note: This function should be called after the MS928X_SetVideo function.
void MS9282_MS928X_SetAudio(void);  
    
// Operations
void MS9282_MS928X_Video_Mute(BOOL bmuteON);
void MS9282_MS928X_Audio_Mute(BOOL bmuteON);
    
// Returns TRUE if the sink device connected.
BOOL MS9282_MS928X_HPD_Detect(void);

// Returns TRUE if MS928x chip exists.
BOOL MS9282_MS928X_IsValid(void);

// Switch I2S/SPDIF audio.
UINT8 MS9282_MS928X_UpdateAudio(AUDIOPARAM_T* pstAudParam);   

// Get current input video mode.
// Returns the corresponding VIC code. Returns 0xFF if no signal detected.
UINT8 MS9282_MS928X_GetInputVideoMode(void);

// HDMI DDC access enable
void MS9282_MS928X_DDC_Enable(BOOL b_enable);

// Horizontal or vertical pixel shift
void MS9282_MS928X_ShiftConfig(UINT8 u8_h_position, UINT8 u8_v_position);

// Do auto phase, or set manual phase
UINT8 MS9282_MS928X_PhaseConfig(BOOL b_auto_enable, UINT8 u8_phase_sel);

// Adjust brightness
void MS9282_MS928X_SetVideoBrightness(INT8 i8_brightness);

// Adjust contrast
void MS9282_MS928X_SetVideoContrast(UINT8 u8_contrast);

// Adjust tx data driver(max = 0xf)
void MS9282_MS928X_SetHdmiDataDriver(UINT8 u8_main_driver, UINT8 u8_post_driver);

/**********************************************************************************接口移植*****************************************************************************************/
//复位芯片，可不用实现
VOID Sys_Reset(VOID);

//延时操作
VOID Delay_ms(UINT8 u8ms);
VOID Delay_us(UINT8 u8us);

//IIC操作，iic 速率设置，可不用实现
VOID I2C_Sel_Speed(UINT8 u8speed);

//IIC操作，双字节寄存器地址
UINT8 I2C16_ReadByte(UINT8 u8addr, UINT16 u16index);
VOID I2C16_WriteByte(UINT8 u8addr, UINT16 u16index, UINT8 u8value);

//IIC操作，单字节地址，连续读取多个字节
BOOL I2C_ReadBytes(UINT8 u8addr, UINT8 u8index, UINT8 *pIn, UINT8 u8len);

//IIC操作，单字节地址，一次读写一个字节
UINT8 I2C_ReadByte(UINT8 u8addr, UINT8 u8index);
VOID I2C_WriteByte(UINT8 u8addr, UINT8 u8index, UINT8 u8value); 

