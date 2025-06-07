#ifndef _MN869129_H_
#define _MN869129_H_

#include <utils/Log.h>
#include "XbhType.h"
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


//VBO 单分屏
#define VBO_SINGLE_DIVIDE_SECTION   0
//VBO 双分屏
#define VBO_DOUBLE_DIVIDE_SECTION   1

#define TAG "Mn869129"
#define LOGD(...)      __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGI(...)      __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGE(...)      __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)

#define MES(...)       __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define MESn(...)      __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define MESi(...)      __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define MESin(...)     __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define MESii(...)     __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define MESiin(...)    __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define MESiii(...)    __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define WAR(...)       __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define ERR(...)       __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)


XBH_S32 Mn869219_Init(XBH_VOID);

XBH_S32 Mn869219_resetHDMI(XBH_VOID);

XBH_S32 Mn869219_Check_State(XBH_VOID);

XBH_S32 Mn869219_Set_VideoFormat(int Format);

XBH_S32 Mn869219_Get_VideoFormat(XBH_CHAR* info);

XBH_S32 Mn869219_Set_HdmiTxAudioMute(XBH_BOOL bEnable);

XBH_S32 Mn869219_Get_HdmiTxVedioMute(XBH_BOOL* info);

XBH_S32 Mn869219_Set_HdmiTxVedioMute(XBH_BOOL bEnable);

XBH_S32 Mn869219_Get_HdmiTxAudioMute(XBH_BOOL* info);

XBH_S32 Mn869219_Set_HdmiTxHDCPMute(XBH_BOOL bEnable);

XBH_S32 Mn869219_Get_HdmiTxHDCPMute(XBH_BOOL* info);

XBH_S32 Mn869219_Set_HdmiTxVedioMuteALL(XBH_BOOL bEnable);

XBH_S32 Mn869219_getVersion(XBH_VOID);

XBH_S32 Mn869219_SetI2c(XBH_S32 i2cBus, XBH_S32 devAddr);

XBH_U32 reg_read(XBH_U8 slaveaddr, XBH_U8 regaddr, XBH_U32 len, XBH_U8 *data);

XBH_U32 reg_write(XBH_U8 slaveaddr, XBH_U8 regaddr, XBH_U32 len, XBH_U8 *data);

XBH_VOID SLEEP_Delay_ms(XBH_U32 x);

XBH_S32 Mn869129_spdif(XBH_VOID);

#define LF9129_CMD_Delay(x)         SLEEP_Delay_ms(x*100)

#endif