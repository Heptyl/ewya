/***********************************************************************
 * Copyright 2022 by ASL Xiamen Technology Co.,Ltd ("ASL").
 * The Software is proprietary, confidential, and valuable to ASL, 
 * All rights are reserved to ASL, including but not limited to copyrights,
 * patents, trademarks, and other similar rights.
 * The Software shall be kept strictly in confidence, and shall not be
 * distributed this software for any purpose to any third party.
 * The Software is provided "AS IS" without any warranty of any kind, 
 * express, implied, statutory or otherwise.
 * Release Version 1.0 -- 2023.11.29
***********************************************************************/

typedef unsigned char u8;
typedef unsigned int u16;

//Chip Device_Address
#define ASL_DEV_I2C_ADDRESS 0x6A  //ASL DEV 
#define SPI_Address_Length 3

//new FW version for update
#define New_FW_Ver 0x0006
//The data for upgrading is converted from bin file or directly read from the file system.
const u8 SPI_Flash_data[1888] = {0};

//=======================Readme============================
//The entry for upgrading is function: SPI_Flash_update()
//Porting code needs to implement the following functions:
//=========================================================

//ASL I2C read&write funcion,for detail, please refer “ApplicationNote--ASL_I2C_Read_Write_V1.1.pdf”
//Writ register funcition: ASL_DEV_I2C_ADDRESS(W)-->BLOCK_ID-->offsetH-->offsetL-->data0,data1,data2...
//Read register funcition: ASL_DEV_I2C_ADDRESS(W)-->BLOCK_ID-->offsetH-->offsetL-->ASL_DEV_I2C_ADDRESS(R)-->data0,data1,data2...

//The following functions are used to implement read or write one register from ASL chip
u8 ASL_I2C2_Read_Reg(u8 BLOCK_ID, u16 offset, u8 *pData);
u8 ASL_I2C2_Write_Reg(u8 BLOCK_ID, u16 offset, u8 wData);

//The following functions are used to implement continuous read and write of ASL registers.
u8 ASL_I2C2_Read_Reg_Len(u8 BLOCK_ID, u16 offset, u8 *d, u8 Num);
u8 ASL_I2C2_Write_Reg_Len(u8 BLOCK_ID, u16 offset, u8 *d, u8 Num);

//delay function
void delay_us(u16 delay);
void delay_ms(u16 delay);
