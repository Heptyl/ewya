
/***********************************************************************
 * Copyright 2022 by ASL Xiamen Technology Co.,Ltd ("ASL").
 * The Software is proprietary, confidential, and valuable to ASL, 
 * All rights are reserved to ASL, including but not limited to copyrights,
 * patents, trademarks, and other similar rights.
 * The Software shall be kept strictly in confidence, and shall not be
 * distributed  this  software for any purpose to any third party.
 * The Software is provided "AS IS" without any warranty of any kind, 
 * express, implied, statutory or otherwise.
 * Release Version 1.3 -- 2024.10.23
***********************************************************************/

#include "I2C_intf.h"


u8 wdata[16];
void  Erase_SPIFlash_I2C(u32 CurAddr);

bool I2C_Wait_RW_DONE(u32 Timout)
{
	u8 rdata=1;
	while(0!=rdata)
		{
		ASL_I2C2_Read_Reg(0x20,0x3f,&rdata); //bit0=RW_DONE
		Timout--;
		if	(0==Timout)
			{
			return false;
			}
		}
	
	return true;
}


//SPI Flash I2C write_done_check
bool I2C_write_done_check(int Timeout)
{
	unsigned char write_done= 0x01;

	while(write_done==0x01)
	{
	ASL_I2C2_Write_Reg(0x20,0x25,0x80); //General instruction type enable
	ASL_I2C2_Read_Reg(0x20,0x38, &write_done);
	write_done = write_done & 0x01;
	if (Timeout-- == 0) return false;
	delay_us(10);
	}
	return true;
}


bool SPI_Flash_Protect_I2C(bool Protect_EN)
{
	BYTE ProtectValue = Protect_EN;
	if (offset==0x10000) {ProtectValue =Protect_EN? 0x7C : 0x04;}
	else if (offset==0x20000) {ProtectValue =Protect_EN? 0x7C : 0x08;}
	else {ProtectValue =Protect_EN? 0x7C : 0x00;}

   ASL_I2C2_Write_Reg(0x20,0x36,0x06);		 //General instruction type Write_Enable for WEL bit
   ASL_I2C2_Write_Reg(0x20,0x25,0x40);		 //General instruction type enable
   
	//SPI flash protection 
   ASL_I2C2_Write_Reg(0x20,0x34,ProtectValue);  //STATUS_REGISTER_IN
   ASL_I2C2_Write_Reg(0x20,0x25,0x04); //WRITE_STATUS_EN=1

   return I2C_write_done_check(100);
}


void I2C_Flash_Erase(u32 StartAddr)
{
	ASL_I2C2_Write_Reg(0x20,0x36,0x06); //General instruction type
	ASL_I2C2_Write_Reg(0x20,0x25,0x40); //General instruction type enable

	//erase SPI flash
	//0 Flash_addr
	if (3==SPI_Address_Length) //3 byte address
		{
		wdata[0]  = StartAddr & 0x0000ff;
		wdata[1]  = (StartAddr & 0x00ff00)>>8;
		wdata[2]  = (StartAddr & 0xff0000)>>16;
		ASL_I2C2_Write_Reg(0x20,0x20,wdata[0]);
		ASL_I2C2_Write_Reg(0x20,0x21,wdata[1]);
		ASL_I2C2_Write_Reg(0x20,0x22,wdata[2]);
		}
	else //2byte address
		{
		wdata[0]  = StartAddr & 0x0000ff;
		wdata[1]  = (StartAddr & 0x00ff00)>>8;
		ASL_I2C2_Write_Reg(0x20,0x20,wdata[0]);
		ASL_I2C2_Write_Reg(0x20,0x21,wdata[1]);
		}

	//1Flash Erase type
	ASL_I2C2_Write_Reg(0x20,0x37,0x20);    //sector Earse
	//2Flash Erase En
	ASL_I2C2_Write_Reg(0x20,0x25,0x20);

	I2C_write_done_check(100);
}


//read spi data by I2C mode
//ReadAddr :start address
//pBuffer  :read buffer
//NumToRead: read count
bool I2C2_SPI_Flash_Read(u32 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
//	int data_addr_in_buffer=0;
//	u8 rdata;
	//u8 write_done;
	u8 ReadCount=0x20; //fixed=32
	u8 flash_data_addr = 0x60;
	
	while(NumToRead)
		{
		//0Flash address
		if (3==SPI_Address_Length) //3 byte address
		{
			wdata[0]  = ReadAddr & 0x0000ff;
			wdata[1]  = (ReadAddr & 0x00ff00)>>8;
			wdata[2]  = (ReadAddr & 0xff0000)>>16;
			ASL_I2C2_Write_Reg(0x20,0x20,wdata[0]);
			ASL_I2C2_Write_Reg(0x20,0x21,wdata[1]);
			ASL_I2C2_Write_Reg(0x20,0x22,wdata[2]);
			//1flash length
			ASL_I2C2_Write_Reg(0x20,0x23,ReadCount-1);				
		}
		else //2byte address
		{
			wdata[0]  = ReadAddr & 0x0000ff;
			wdata[1]  = (ReadAddr & 0x00ff00)>>8;
			ASL_I2C2_Write_Reg(0x20,0x20,wdata[0]);
			ASL_I2C2_Write_Reg(0x20,0x21,wdata[1]);
			//1flash length
			ASL_I2C2_Write_Reg(0x20,0x22,ReadCount-1);
			ASL_I2C2_Write_Reg(0x20,0x23,0x00);	
		}
		
		//2 Flash read_en
		ASL_I2C2_Write_Reg(0x20,0x25,0x02);
			I2C_Wait_RW_DONE(100);
		ASL_I2C2_Read_Reg_Len(0x20, flash_data_addr, pBuffer, ReadCount);

		pBuffer = pBuffer + ReadCount;
		ReadAddr= ReadAddr + ReadCount;

		//for  last data<32
		if (NumToRead>=ReadCount)
			{				
			NumToRead = NumToRead - ReadCount;
			}
		else
			{
			NumToRead=ReadCount;
			}
	}
	return true;
}

//write SPI FLASH by I2C
//WriteAddr: write address(24bit)
//pBuffer: data buffer
//NumByteToWrite: write count(max 65535)

bool I2C2_SPI_Flash_Write(u32 WriteAddr,u8 *pBuffer,u16 NumByteToWrite)
{
	u32 j=0;
	u8 flash_len=32;

	u8 data_addr =0x00;
	for (j=0;j<NumByteToWrite;)
		{
		Erase_SPIFlash_I2C(WriteAddr);

		if (3==SPI_Address_Length) //new mode for 3 byte address
		{
			wdata[0]  = WriteAddr & 0x0000ff;
			wdata[1]  = (WriteAddr & 0x00ff00)>>8;
			wdata[2]  = (WriteAddr & 0xff0000)>>16;
			ASL_I2C2_Write_Reg(0x20,0x20,wdata[0]);
			ASL_I2C2_Write_Reg(0x20,0x21,wdata[1]);
			ASL_I2C2_Write_Reg(0x20,0x22,wdata[2]);
			//1flash length
			ASL_I2C2_Write_Reg(0x20,0x23,0x1f);
		}
		else //old mode for 2byte address
		{
			wdata[0]  = WriteAddr & 0x0000ff;
			wdata[1]  = (WriteAddr & 0x00ff00)>>8;
			ASL_I2C2_Write_Reg(0x20,0x20,wdata[0]);
			ASL_I2C2_Write_Reg(0x20,0x21,wdata[1]);
			//1flash length
			ASL_I2C2_Write_Reg(0x20,0x22,0x1f);
			ASL_I2C2_Write_Reg(0x20,0x23,0x00); 
		}
		
		ASL_I2C2_Write_Reg_Len(0x20,data_addr, &pBuffer[j], flash_len);
		j=j+flash_len;
		WriteAddr = WriteAddr +0x20;

		ASL_I2C2_Write_Reg(0x20,0x36,0x06);         //General instruction type
		ASL_I2C2_Write_Reg(0x20,0x25,0x40);         //General instruction type enable
		delay_ms(1);
		//3flash write_en
		ASL_I2C2_Write_Reg(0x20,0x25,0x01);         // flash write enable

		//delay and make sure all data has been wrote.
        I2C_write_done_check(100);

	  }
	     return true;


}

u32 Erased_Address_I2C=0; //Erased address+1
u32 Each_Time_Erase_Size=0x1000; //erase block size 

void Erase_Flash_init_I2C(u32 EAddr)
{
	Erased_Address_I2C=EAddr;
}


void  Erase_SPIFlash_I2C(u32 CurAddr)
{
	if (CurAddr<Erased_Address_I2C) 
	{
		return;
	}

	I2C_Flash_Erase(CurAddr);
	Erased_Address_I2C+=Each_Time_Erase_Size;

}

u32 Offset=0;

//init chip before flash update
bool Chip_init_5803()
{
u8 RDat;
u16 CurVer;
//check chip ID first
ASL_I2C2_Read_Reg(0x01,0x03,&RDat); 	//# expect value is. 0x58
if (0x58==RDat)
	{
	printf("---Read CS5803 chipID ok.\r\n");
	}
else	
	{
	printf("---Read CS5803 Chip ID error.Get : 0x%02x.\r\n",RDat);
	return false;
	}
//Read version
ASL_I2C2_Read_Reg(0x01,0xC2,&RDat); 
CurVer=RDat;
ASL_I2C2_Read_Reg(0x01,0xC1,&RDat); 
CurVer = CurVer | (RDat<<8);

printf("---Current Version is : 0x%04x.\r\n",CurVer);

//init chip
	ASL_I2C2_Write_Reg(0x10,0x4f,0x00);  //;disable SPI mode
	ASL_I2C2_Write_Reg(0x20,0x30,0x11); //external flash

//init offset
	Offset=0x00;

	return true;	
}

 bool SPI_Flash_update_5803()
 {  
    u8 read_back_data[32];   // this data arrary is to save the data read back from flash
    u16 write_len;
	u8 RDat;
	u16 ChipID,CurVer;
	u32 i,j;

	if (false==Chip_init_5803()) return false;

	Erase_Flash_init_I2C(Offset); //init flash erase addrese
//#---------------Check Flash ID--------------------------------
	printf("Step1:\r\n");
    ASL_I2C2_Write_Reg(0x20,0x25,0x08);     //# Read status Enable before read back from SPI flash
    ASL_I2C2_Read_Reg(0x20,0x39,&RDat);     //# expect value is. 0x0B for XTX, 0x85 for PURAN
    printf("---Flash manufactory ID 0x%02X.\r\n",RDat);
    ASL_I2C2_Read_Reg(0x20,0x3A,&RDat);     //# expect value is, 0x10
    printf("---Flash Chip ID 0x%02X.\r\n",RDat);
	
	printf("---SPI flash protect disable.\r\n");
	SPI_Flash_Protect_I2C(0);

    write_len = sizeof(SPI_Flash_data)/32;


	printf("Step2: Flash update and verify ......");
    for(i=0;i<write_len;i++)
        {
        // 32bytes data will be write into flash per time
        I2C2_SPI_Flash_Write(Offset+i*32, (u8 *)&SPI_Flash_data[(i*32)],32);   
        I2C2_SPI_Flash_Read(Offset+i*32, read_back_data, 32);
		
        for(j=0;j<32;j++)
        	{
	        if(SPI_Flash_data[i*32+j] !=read_back_data[j])
	        	{
	            printf("\r\n---Verify Flash data error, address=0x%x. \r\n",Offset+i*32);
				return false;
				}
	    	}
		
		printf(".");
		}

	printf("Done.\r\n\r\n");
	
	printf("Step3:\r\n");
	printf("---SPI flash protect enable.\r\n");
	SPI_Flash_Protect_I2C(1);
	
	printf("Step4: Reset and check Version...\r\n");

	ASL_I2C2_Write_Reg(0x10,0x42,0x00); //reset chip
	delay_ms(100);
	//check CRC
	//ASL_I2C2_Read_Reg(0x03,0x04,&RDat); 
	ASL_I2C2_Read_Reg(0x03,0x18,&RDat); //20241023,0x18=外部flash，04=内部flash
	if (RDat!=0x80)
		{
		printf("---flash CRC error: 0x%02x, udpate fail.\r\n",RDat);	
		return false;
		}
	//Read version
	ASL_I2C2_Read_Reg(0x01,0xC2,&RDat); 
	CurVer=RDat;
	ASL_I2C2_Read_Reg(0x01,0xC1,&RDat);
	CurVer = CurVer | (RDat<<8);
	printf("---New Version is : 0x%04x.\r\n",CurVer);
	if (New_FW_Ver !=CurVer)
		{
		printf("---Version is nor right: 0x%04x, udpate fail.\r\n",CurVer);	
		return false;
		}

	printf(">>>>>>>>>SPI Flash Update Success<<<<<<<<<<\r\n");

	return true;

 }
 
