/******************************************************************************
* @file    ms928x_mpi.h
* @author  
* @version V1.0.0
* @date    11-Nov-2014
* @brief   MacroSilicon Programming Interface.
*
* Copyright (c) 2009-2014, MacroSilicon Technology Co.,Ltd.
******************************************************************************/
#ifndef __MACROSILICON_MS928X_PROGRAMMING_INTERFACE_H__
#define __MACROSILICON_MS928X_PROGRAMMING_INTERFACE_H__
#include "mst_typedef.h"


#ifdef __cplusplus
extern "C" {
#endif


extern VOID Sys_Reset(VOID);

// Delay a period of time.
extern VOID Delay_ms(UINT8 u8ms);
extern VOID Delay_us(UINT8 u8us);

//
extern VOID I2C_Sel_Speed(UINT8 u8speed);
extern UINT8 I2C16_ReadByte(UINT8 u8addr, UINT16 u16index);
extern VOID I2C16_WriteByte(UINT8 u8addr, UINT16 u16index, UINT8 u8value);

//
extern BOOL I2C_ReadBytes(UINT8 u8addr, UINT8 u8index, UINT8 *pIn, UINT8 u8len);
extern UINT8  I2C_ReadByte(UINT8 u8addr, UINT8 u8index);
extern VOID I2C_WriteByte(UINT8 u8addr, UINT8 u8index, UINT8 u8value); 


#ifdef __cplusplus
}
#endif


#endif  // __MACROSILICON_MS928X_PROGRAMMING_INTERFACE_H__
