/*
 * Copyright (C) 2021 VIA Labs, Inc. 
 *
 * Module: 
 * Hub_update.h
 *
 * Author: 
 * Sherlock Chu <SherlockChu@via-labs.com>
 *
 * Revision History:
 *
 * 20210826
 *
 */

#include "hub_update.h"
#include <android/log.h>
#define LOG_TAG  "xbh_mw@ChipHl817_hub_update"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static ssize_t gbl_devcnt = 0;
static libusb_device **gbl_devs = NULL;

static const uint8_t SPI_DataBase[14][9] = {	// Sherlock_20160823, Follow SpiFlash.ini
//	[0],			[1],			[2],			[3],			[4],			[5],			[6],			[7]			[8]
//    Read_ID,	Write_En,		Write_Sts, 	Chip_Erase, 	Read_Data,	Read_Sts,	Page_Pro,	Page_Size,	Sector_Erase
	{0x9F,		0x06, 		0x01, 		0xC7, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// 0:A25LXXX
	{0x15,		0x06, 		0x01, 		0x62, 		0x03, 		0x05, 		0x02,		0x20,		0x00},	// 1:AT25F512A/B
	{0x9F,		0x06, 		0x01, 		0x60, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// 2:EN25FXXX
	{0x9F,		0x06, 		0x01, 		0xC7, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// 3:GD25QXXX
	
	{0x9F,		0x06, 		0x01, 		0xC7, 		0x03, 		0x05, 		0x02,		0x20,		0x00},	// 4:M25PxxA/xx
	{0x9F,		0x06, 		0x01, 		0x60, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// 5:MX25Lxxx/xxxC/xxxE
	{0x9F,		0x06, 		0x01, 		0x60, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// 6:MX25Lxxx1E
	{0x90,		0x06, 		0x01, 		0x60, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// 7:PCT/SST25VFxxx/xxxA
	
	{0x90,		0x06, 		0x01, 		0xC7, 		0x03, 		0x05, 		0x02,		0x20,		0xD7},	// 8:PM25LDxxx
	{0xAB,		0x06, 		0x01, 		0xC7, 		0x03, 		0x05, 		0x02,		0x20,		0xD7},	// 9:PM25LVxxx
	{0x9F,		0x06, 		0x01, 		0xC7, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// A:W25XxxBV/W25XxxCL
	{0x9F,		0x06,		0x01,		0xC7,		0x03,		0x05,		0x02,		0x20,		0x20},	// B:FM25Fxxx

	{0x9F,		0x06, 		0x01, 		0x60, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// C:KH25LxxxxE
	{0x9F,		0x06, 		0x01, 		0x60, 		0x03, 		0x05, 		0x02,		0x20,		0x20},	// D:MX25Vxxx

};

static int gDbgMsg = 0;
static int gLogFile = 0;		// XX
static int gUpdateMode = 0;	// XX
static int DeviceNum=0;	// XX
static int Hub1VID=0;
static int Hub1PID=0;
static int PortNums=-1;
static int gHubPDShareSPI = 0;
static int gHubHubShareSPI = 0;	// XX
static int gReadVerOnly = 0;
static int gReadBinOnly = 0;	// XX
static int gBackUpOnly = 0;
static int gI2CTestOnly = 0;	// XX
static char Hub1Bin[MAX_PATH];
static char Hub1ShareBin[MAX_PATH];	// XX
static char PD1Bin[MAX_PATH];
static int	TempVID, TempPID;
static int	Ver0, Ver1, Ver2, Ver3;
static int gPath[7] = {0};
static int gSendCmdOnly = 0;
static uint32_t	gCmd1 = 0;
static uint32_t	gCmd2 = 0;

//-------------------------------------------------------------------------------------------------

static char* str_replace_char( char *src, char find, char replace, uint16_t loop ) 
{
	uint16_t i;
	char *tmp;

	if ( !src )
	{
		return NULL;
	}

	tmp = src;   
	for ( i = 0; i < loop; i++ )
	{

		tmp = strchr( tmp, find );
		if ( !tmp )
		{
			return NULL;
		}
	}

	if ( tmp && ( tmp != src ) )
	{
		*tmp = replace;
	}

	return tmp;
}

//-------------------------------------------------------------------------------------------------

/* Get_Nth_Argu_From_String. "cmd.exe in1 in2 in3", N=2, Get_Token_Is_in1 */
static char* str_find_token( char *src, uint16_t ntoken )
{
	uint16_t i;

	if ( !src || !strlen( src ) )
	{
		return NULL;
	}
	else if ( ntoken == 1 )
	{
		return src;
	}

	for ( i = 1; i < ntoken; i++ )
	{

		/* Locate_First_Occurrence_Of_Char_In_String_And_Returns_A_Pointer_To_The_Char_In_String */    
		src = strchr( src, ' ' );
		if (!src)
		{
			return NULL;
		}

		do
		{
			src++;
		} while ( *src == ' ' );
	}

	return src;
}

//-------------------------------------------------------------------------------------------------

/* If_String_Has_More_Than_1_Space, Shorten_It_To_1_Space */
static void str_mix_space( char *src )
{
	uint16_t i, istr, nstr;
	bool is_space_prior = false;

	if ( !src )
	{
		return;
	}

	nstr = strlen( src );
	for ( i = 0, istr = 0; i < nstr; i++ )
	{
		if ( isspace( src[i] ) )
		{      
			if ( i == ( nstr - 1 ) )
			{	/* Delete_Space_If_It_Is_Last_Character_In_String */
				if ( is_space_prior )
				{
					istr--;
				}  

				break;
			}
			else if ( is_space_prior )
			{	/* Not_Add_Space_If_Previous_Is_Space */
				continue;
			}

			is_space_prior = true;
			src[istr++] = ' ';
		}
		else
		{
			is_space_prior = false;
			src[istr++] = src[i];
		}
	}

	src[istr] = '\0'; 
}

//-------------------------------------------------------------------------------------------------

static int str_endless_compare( char *cmp1, char *cmp2 )
{	// Return: 0_Is_Match, Others_Are_Not_Match
	size_t i, ncmp1, ncmp2, nmin;

	if ( !cmp1 || !cmp2 )
	{
		return -1;
	}

	ncmp1 = strlen( cmp1 );
	ncmp2 = strlen( cmp2 );
	if ( ncmp1 < ncmp2 )	nmin = ncmp1;
	else					nmin = ncmp2;

	for ( i = 0; i < nmin; i++ )
	{
		if ( tolower( cmp1[i] ) != tolower( cmp2[i] ) )
		{
			return -1;
		}
	}

	if ( ncmp1 > ncmp2 )
	{
		return isspace( cmp1[i] ) ? 0 : -1;
	}
	else if ( ncmp2 > ncmp1 )
	{
		return isspace( cmp2[i] ) ? 0 : -1;
	}
	else
	{
		return 0;
	}
}

//-------------------------------------------------------------------------------------------------

static void str_lower( char* source )
{
	uint16_t i, istr, nstr;
	bool is_space_prior = false;

	if ( !source )
	{
		return;
	}

	nstr = strlen( source );
	for ( i = 0, istr = 0; i < nstr; i++ )
	{        
		if ( isspace(source[i] ) )
		{

			if ( is_space_prior )
			{
				continue;
			}
			else if ( ( source[i] == '\n' ) && ( i == nstr - 1 ) )
			{
				continue;
			}

			is_space_prior = true;   
			source[istr++] = ' ';
		}
		else
		{
			is_space_prior = false;
			source[istr++] = ( char )tolower( source[i] );
		}
	}

	source[istr] = '\0'; 
}

//-------------------------------------------------------------------------------------------------

//	Search_"*str"_from_"*src".
//	Return_string_after_"*str", Or_"NULL"
static char* str_endless_str( char* src, char* str )
{
	char *ret_str;
	char *tmp_source, *tmp_str, *token;
	size_t source_length, str_length;

	ret_str = NULL;
	tmp_source = NULL;
	tmp_str = NULL;
    
	if ( !src || !str )
	{
		goto Fail;
	}

	source_length = strlen( src );
	str_length = strlen( str );
	if ( !source_length || !str_length )
	{
		goto Fail;
	}

	tmp_source = malloc( source_length + 1 ); // 1: EOS
	tmp_str = malloc( str_length + 1 );
	memset(tmp_source, 0, ( source_length + 1 ));
	memset(tmp_str, 0, ( str_length + 1 ));
//	tmp_source = ( char* )mem_zalloc( source_length + 1 ); // 1: EOS
//	tmp_str = ( char* )mem_zalloc( str_length + 1 );
	if ( !tmp_source || !tmp_str )
	{
		goto Fail;
	}

	strcpy( tmp_source, src );
	strcpy( tmp_str, str );
	str_lower( tmp_source );
	str_lower( tmp_str );
        
	token = strstr( tmp_source, tmp_str );
	if ( !token )
	{
		goto Fail;
	}

	ret_str = src + ( token - tmp_source );

 Fail:

	if ( tmp_source )
	{
		free( tmp_source );
	}

	if (tmp_str)
	{
		free(tmp_str);
	}

	return ret_str;
}

//-------------------------------------------------------------------------------------------------

static int str_get_ptn_str( char *source, char *pattern, char *value )
{
	char *file_mark;

	if ( !source || !pattern )
	{
		return LIBUSB_ERROR_INVALID_PARAM;
	}

	if ( file_mark = str_endless_str( source, pattern ) )
	{
		char* eq_mark;

		if ( eq_mark = strchr( ( char* )file_mark, '=' ) )
		{
			// bypass '='
			eq_mark++;

			// go to first graph char.
			while ( !isgraph( *eq_mark ) )
			{
				eq_mark++;
			}

			while ( *eq_mark )
			{
				if ( isgraph( *eq_mark ) )
				{
					unsigned i = 0;

					do
					{
						value[i++] = *eq_mark++;
					} while ( isgraph( *eq_mark ) );

					if ( i )
					{
						value[i] = '\0';
						return LIBUSB_SUCCESS;
					}
				}
				eq_mark++;
			}
		}
	}

	return LIBUSB_ERROR_NOT_FOUND;
}

//-------------------------------------------------------------------------------------------------

static int str_get_ptn_int( char* source, char* pattern, int base, int *output )
{
	int ret;
	long tmp;
	char ptn_str[64];

	ret = str_get_ptn_str( source, pattern, ptn_str );
	if ( ret == LIBUSB_SUCCESS )
	{
		tmp = strtol( ptn_str, NULL, base );
		*output = ( int )tmp;
	}

	return ret;
}

//-------------------------------------------------------------------------------------------------

static int str_get_ptn_uint( char* source, char* pattern, int base, uint32_t *output )
{
	int ret;
	long tmp;
	char ptn_str[64];

	ret = str_get_ptn_str( source, pattern, ptn_str );
	if ( ret == LIBUSB_SUCCESS )
	{
		tmp = strtol( ptn_str, NULL, base );
		*output = ( uint32_t )tmp;
	}

	return ret;
}
//-------------------------------------------------------------------------------------------------

/* Send_Libusb_Ctrl_Tx, Return: LIBUSB_SUCCESS_Is_OK, Others_Are_NG */
/*
static int issue_spi_cmd(	libusb_device_handle *devh, 
							uint8_t request_type, 
							uint8_t bRequest, 
							uint16_t wValue, 
							uint16_t wIndex,
							void *data, 
							uint16_t wLength, 
							unsigned int timeout	)
{
	int ret;
    
	ret = libusb_control_transfer(	devh, request_type, bRequest, wValue, wIndex, 
								(unsigned char*)data, wLength, timeout		);

//	prltrace( prl_verbose, "%p reqtype %x req %x value %.2x index %.2x "\
//			"length %x mtimeout %d %s(%d)\n", 
//			devh, request_type, bRequest, wValue, wIndex, wLength, timeout, 
//			( ret < 0 ) ? libusb_error_name(ret) : "LIBUSB_SUCCESS", ret			);

	return ( ( ret >= 0 ) ? LIBUSB_SUCCESS : ret );
}
*/
//-------------------------------------------------------------------------------------------------

uint32_t I2CWriteCmd( libusb_device_handle *devh, uint8_t bRequest, uint16_t Length, uint8_t *pData )
{
	uint32_t Status = Fail_State;
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								bRequest,
								0x00,
								0x00,
								pData,
								Length,
								1000
							);

	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "I2C-WriteCmd %02X Fail %d\n", bRequest, ret );

	return Status;
}
//---------------------------------------------------------------------------------------------------------------

uint32_t I2CReadCmd( libusb_device_handle *devh, uint8_t bRequest, uint8_t SlaveAdr, uint16_t Length, uint8_t *pData )
{
	uint32_t Status = Fail_State;
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
								bRequest,
								0x00,
								SlaveAdr,
								pData,
								Length,
								1000
							);
	
	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "I2C-ReadCmd %02X Fail %d\n", bRequest, ret );

	return Status;
}

//-------------------------------------------------------------------------------------------------

uint32_t Write_Reg( libusb_device_handle *devh, uint8_t bRegClass, uint8_t bRegAddr, uint8_t Value )
{
	uint32_t Status = Fail_State;
	uint8_t Data[0x2];
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								bRegClass,
								bRegAddr,
								Value,
								Data,
								0x00,
								1000
							);


	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Write-Reg %02X-%02X Fail\n", bRegClass, bRegAddr );

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t Read_Reg(libusb_device_handle *devh, uint8_t bRegClass, uint8_t bRegAddr, uint8_t *Value)
{
	uint32_t Status = Fail_State;
	uint8_t Data[0x2];
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
								bRegClass,
								bRegAddr,
								0x00,
								Data,
								0x01,
								1000
							);


	if ( ret >= 0 )
	{
		Value[0] = Data[0];
		Status = Success_State;
	}
	else
		LOGD( "Read-Reg %02X-%02X Fail\n", bRegClass, bRegAddr );

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Read_Status( libusb_device_handle *devh, uint8_t bReadSts, uint16_t Length, uint8_t * Data)
{
	uint32_t Status = Fail_State;
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
								0xC1,
								bReadSts,
								0x00,
								Data,
								Length,
								1000
							);


	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Read_Status_FAIL\n" );

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Read_Data( libusb_device_handle *devh, uint8_t bReadData,  uint32_t DataAddr, uint32_t BufferOffset, uint32_t Length, uint8_t * Data)
{
	uint32_t Status = Fail_State;
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
								0xC4,
								((DataAddr >> 8) & 0xFF00) | bReadData, // Read_Data CMD is 0x03,
								((DataAddr << 8) & 0xFF00) | ((DataAddr >> 8) & 0x00FF),
								Data+BufferOffset,
								Length,
								1000
							);


	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Read_Status_FAIL\n" );

	return Status;
}

//-------------------------------------------------------------------------------------------------

uint32_t SPI_Write_Status( libusb_device_handle *devh, uint8_t bWriteSts, uint8_t TargetSts)
{
	uint32_t Status = Fail_State;
	int ret;
	uint8_t Data[0x2];

	Data[0] = TargetSts;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								0xD1,
								bWriteSts,
								0x00,
								Data,
								0x01,
								1000
							);


	Sleep(100); // Sherlock_20150925, Fix For GD & EN SPI Flash
	
	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Write_Status_FAIL\n" );

	return Status;

}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Write_Enable( libusb_device_handle *devh, uint8_t bWriteEn )
{
	uint32_t Status = Fail_State;
	int ret;
	uint8_t Data[0x2];

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								0xD1,
								bWriteEn,
								0x00,
								Data,
								0x00,
								1000
							);
	
	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGE( "Write_Enable_FAIL\n" );

	return Status;

}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Chip_Erase( libusb_device_handle *devh, uint8_t bChipEr )
{
	uint32_t Status = Fail_State;
	int ret;
	uint8_t Data[0x2];

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								0xD1,
								bChipEr,
								0x00,
								Data,
								0x00,
								8000
							);

	
	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Chip_Erase_FAIL\n" );

	return Status;

}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Sector_Erase( libusb_device_handle *devh, uint8_t bSectorEr, uint32_t DataAddr )
{
	uint32_t Status = Fail_State;
	int ret;
	uint8_t Data[0x2];

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								0xD4,
								((DataAddr >> 8) & 0xFF00) | bSectorEr, // Sector_Erase CMD is 0x20 or 0xD7
								((DataAddr << 8) & 0xFF00) | ((DataAddr >> 8) & 0x00FF),
								Data,
								0x00,
								1000
							);


	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Sector_Erase_FAIL\n" );

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Write_Data( libusb_device_handle *devh, uint8_t bPageProg, uint32_t DataAddr, uint32_t BufferOffset, uint32_t Length, uint8_t * Data )
{
	uint32_t Status = Fail_State;
	int ret;

	ret = libusb_control_transfer(	devh,
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
								0xD4,
								((DataAddr >> 8) & 0xFF00) | bPageProg, // Page_Program CMD is 0x02
								((DataAddr << 8) & 0xFF00) | ((DataAddr >> 8) & 0x00FF),
								Data+BufferOffset,
								Length,
								1000
							);


	if ( ret >= 0 )
	{
		Status = Success_State;
	}
	else
		LOGD( "Write_Data_FAIL\n" );

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_Wait_Finish( libusb_device_handle *devh, uint8_t bReadSts, uint32_t SleepMs)
{
	uint32_t Status = Fail_State;
	int ret;
	uint32_t idx, Cnt = 0, ReadyCnt = 2;
	uint8_t Data[0x2];

	for(idx=0; idx<1000; idx++)
	{
		Data[0] = 0x7F;

		ret = libusb_control_transfer(	devh,
									USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
									0xC1,
									bReadSts,
									0x00,
									Data,
									0x01,
									1000
								);

		if ( ret < 0 ) // Cmd_Fail, goto exit
		{
			goto Exit;
		}

		if ( ( Data[0] & 0x03 ) == 0x00 )	Cnt++;
		else							Cnt = 0;

		if ( Cnt >= ReadyCnt )
		{	// Must Get bit[1:0]=00b "TWICE" for Success
			Status = Success_State;
			goto Exit;
		}

		if ( SleepMs )
			Sleep(500); // =0.5s

	}
	
Exit:
	if ( idx > ReadyCnt )
	{
		LOGD("  Wait_Cnt=%d\n", idx);
	}

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_WrStatus_Exe( libusb_device_handle *devh, uint8_t SPI_Type, uint8_t bTargetStatus, uint8_t MSG)
{	//WriteSts_Read_Check Execution Together
	uint32_t	Status = Fail_State; // Fail_State
	uint8_t 	bWriteEn, bWriteSts, bReadSts, bSSTWriteSts_0, Cnt;
	uint8_t	buf[2];

	bWriteEn = SPI_DataBase[SPI_Type][1];
	bWriteSts = SPI_DataBase[SPI_Type][2];
	bReadSts = SPI_DataBase[SPI_Type][5];
	bSSTWriteSts_0 = 0x50; // Special Command Flow of SST

	for ( Cnt=0; Cnt<3; Cnt++ ) // Retry 3 Times
	{
		// ---------- SPI_Write_Status Process ----------
		Status = SPI_Write_Enable(devh, bWriteEn);

		if ( SPI_Type == 7 ) // SST Special Only
		{
			if(MSG)	LOGD("SST Special  SPI_WriteEN 0x50\n");
			Status = SPI_Write_Status(devh, bSSTWriteSts_0, bTargetStatus); // 0x00
		}

		Status = SPI_Write_Status(devh, bWriteSts, bTargetStatus); // 0x00

		buf[0] = 3;
		Status = SPI_Read_Status(devh, bReadSts, 1, buf);
		if ( MSG )	LOGD("	SPI_Status_%d = 0x%02X\n", Cnt, buf[0]);

		if ( buf[0] == bTargetStatus ) // 0x00
		{
			Status = Success_State;
			break;
		}
	}

	return Status;
}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_SeRdCmp_Exe( libusb_device_handle *devh, uint8_t SPI_Type, uint32_t SPIStartAddr, uint8_t MSG) 
{	// SectorErase_Read_Compare Execution Together. Always 0x1000(4K) Bytes
	uint32_t	Status = Fail_State; // Fail_State
	uint32_t 	TotalByte, RemainLen, TransferLen, AddrOffset, MaxFWTxSize = 0x20;
	uint32_t	idx;
	uint8_t 	*pByte, bWriteEn, bReadData, bReadSts, bSectorEr;
	uint8_t	*CtrlDataBuf;

	bWriteEn = SPI_DataBase[SPI_Type][1];
	bReadData = SPI_DataBase[SPI_Type][4];
	bReadSts = SPI_DataBase[SPI_Type][5];
	bSectorEr = SPI_DataBase[SPI_Type][8];

	TotalByte = 0x1000;
	CtrlDataBuf = (uint8_t *)malloc(TotalByte);

	TotalByte = 0x20; // Sherlock_20180202, Just Check 32 Bytes for 0xFF


	// ---------- Sector_Erase_SPI_Process ----------
	if ( MSG )	LOGD("SeRdCmp_Se\n");

	Status = SPI_Write_Enable(devh, bWriteEn);
	if ( Status != Success_State )	goto END_OF_SeRdCmp;
	if ( MSG )	LOGD("	SeRdCmp_Wen_OK\n");

	Status = SPI_Sector_Erase(devh, bSectorEr, SPIStartAddr);
	if ( Status != Success_State )	goto END_OF_SeRdCmp;
	if ( MSG )	LOGD("	SeRdCmp_Se_OK\n");

	Status = SPI_Wait_Finish(devh, bReadSts, 1); // Wait_Erase_Done
	if ( Status != Success_State ) 
	{
		LOGD("SeRdCmp_Wait_Fail\n");
		goto END_OF_SeRdCmp;
	}


	//---------- Read_SPI_Data_Process ----------
	if ( MSG )	LOGD("SeRdCmp_Rd\n");
	memset(CtrlDataBuf, 0xFF, TotalByte);

	RemainLen=TotalByte;
	AddrOffset=0;
	while(RemainLen!=0)
	{
		if(RemainLen > MaxFWTxSize)
			TransferLen = MaxFWTxSize;
		else
			TransferLen = RemainLen;

		// --- Read_SPI_Data ---
		Status = SPI_Read_Data(devh, bReadData, SPIStartAddr+AddrOffset, AddrOffset, TransferLen, CtrlDataBuf);
		if(Status != Success_State) 
		{
			LOGD("SeRdCmp_Rd_Error\n");
			goto END_OF_SeRdCmp;
		}

		RemainLen-=TransferLen;
		AddrOffset+=TransferLen;
	}


	// ---------- Compare_SPI_Data_Process ---------- 
	if ( MSG )	LOGD("SeRdCmp_Cmp\n");

	pByte = (uint8_t *)CtrlDataBuf;
	for ( idx = 0; idx < TotalByte; idx++ )
	{
		if( pByte[idx] != 0xFF )
		{
			Status = Fail_State;
			if((idx & 0x001F) == 0x0000)
			{	
				LOGD("  Compare_Data_Error_at_%06X, Tx_%02X != Rx_%02X\n", SPIStartAddr+idx, 0xFF , pByte[idx]);
			}
		}
	}
	if ( Status != Success_State ) 
	{
		LOGD("SeRdCmp_Cmp_Error\n");
		goto END_OF_SeRdCmp;
	}


END_OF_SeRdCmp:

	if ( Status == Success_State )
	{
		if ( MSG )	LOGD("==  SeRdCmp_PASS.  ==\n");
	}
	else
	{
		LOGD("\n=======================\n");
		LOGD("==  SeRdCmp_FAIL.  ==\n");
		LOGD("=======================\n\n");
	}

	if ( CtrlDataBuf )
		free( CtrlDataBuf );
	return Status;

}

// ------------------------------------------------------------------------------------------------

uint32_t SPI_WrRdCmp_Exe( libusb_device_handle *devh, uint8_t SPI_Type, uint32_t SPIStartAddr, uint32_t DataStartAddr, uint32_t Length, uint8_t * buf, uint8_t MSG) 
{	// Write_Read_Compare Execution Together
	uint32_t	Status = Fail_State;
	uint32_t 	TotalByte, RemainLen, TransferLen, AddrOffset, MaxFWTxSize = 0x20;
	uint32_t	idx;
	uint8_t 	bWriteEn, bReadData, bPageProg;
	uint8_t	*CtrlRxDataBuf;

	bWriteEn = SPI_DataBase[SPI_Type][1];
	bReadData = SPI_DataBase[SPI_Type][4];
	bPageProg = SPI_DataBase[SPI_Type][6];

	TotalByte = Length;
	CtrlRxDataBuf = (uint8_t *)malloc(SIZE_128KB);

	// ---------- Write_SPI_Data_Process ----------
	if ( MSG )	LOGD("WrRdCmp_Wr\n");

	RemainLen=TotalByte;
	AddrOffset=0;
	while(RemainLen!=0)
	{
		if(RemainLen > MaxFWTxSize)
			TransferLen = MaxFWTxSize;
		else
			TransferLen = RemainLen;

		Status = SPI_Write_Enable(devh, bWriteEn);
		if(Status != Success_State) 	goto END_OF_WrRdCmp;

		Status = SPI_Write_Data(devh, bPageProg, SPIStartAddr+AddrOffset, DataStartAddr+AddrOffset, TransferLen, buf);

		Sleep(2); // Sherlock_20190416, Some_SPI_Need_More_Time// =0.8ms

		if((AddrOffset & 0x0FFF) == 0x0000)
		{
			if(MSG)
			{
				LOGD("    TxDataBuf[0x%06X]: %02X,%02X,%02X,%02X\n", SPIStartAddr+AddrOffset, (uint8_t)buf[DataStartAddr+AddrOffset],
				(uint8_t)buf[DataStartAddr+AddrOffset+1], (uint8_t)buf[DataStartAddr+AddrOffset+2], (uint8_t)buf[DataStartAddr+AddrOffset+3]);
			}
		}
		if(Status != Success_State) 
		{
			LOGD("WrRdCmp_Wr_Error\n");
			goto END_OF_WrRdCmp;
		}

		RemainLen-=TransferLen;
		AddrOffset+=TransferLen;

		if((AddrOffset & 0x0FFF) == 0x0000)
			LOGD("\r    Writing...%01d0%%", (AddrOffset * 10) / TotalByte); // display_10_20_30...
	}
	LOGD("\r 				   \r"); // Clean_progress


	//---------- Read_SPI_Data_Process ----------
	if ( MSG )	LOGD("WrRdCmp_Rd\n");
	memset(CtrlRxDataBuf, 0xFF, SIZE_128KB);

	RemainLen=TotalByte;
	AddrOffset=0;
	while(RemainLen!=0)
	{
		if(RemainLen > MaxFWTxSize)
			TransferLen = MaxFWTxSize;
		else
			TransferLen = RemainLen;

		Status = SPI_Read_Data(devh, bReadData, SPIStartAddr+AddrOffset, DataStartAddr+AddrOffset, TransferLen, CtrlRxDataBuf);
		if(Status != Success_State) 
		{
			LOGD("WrRdCmp_Rd_Error\n");
			goto END_OF_WrRdCmp;
		}

		RemainLen-=TransferLen;
		AddrOffset+=TransferLen;

		if((AddrOffset & 0x0FFF) == 0x0000)
			LOGD("\r    Checking...%01d0%% 	", (AddrOffset * 10) / TotalByte); // display_10_20_30...
	}
	LOGD("\r 				   \r"); // Clean_progress


	// ---------- Compare_SPI_Data_Process ---------- 
	if ( MSG )
		LOGD("WrRdCmp_Cmp\n");

	for(idx = 0; idx < TotalByte; idx++)
	{
		if( buf[DataStartAddr + idx] != CtrlRxDataBuf[DataStartAddr + idx] )
		{
			Status = Fail_State;
			if((idx & 0x0FFF) == 0x0000)
			{
				LOGD("  Compare_Error_at_SPI_%06X, Tx_%02X != Rx_%02X\n", SPIStartAddr+idx, (uint8_t)buf[DataStartAddr+idx] , (uint8_t)CtrlRxDataBuf[DataStartAddr+idx]);
			}
		}
	}
	if(Status != Success_State) 
	{
		LOGD("WrRdCmp_Cmp_Error\n");
		goto END_OF_WrRdCmp;
	}


END_OF_WrRdCmp:

	if(Status == Success_State)
	{
		if ( MSG )
			LOGD("=	WrRdCmp_PASS.   =\n");
	}
	else
	{
		LOGD("\n=======================\n");
		LOGD("=	WrRdCmp_FAIL.   =\n");
		LOGD("=======================\n\n");
	}

	if(CtrlRxDataBuf)
		free(CtrlRxDataBuf);
	return Status;
}

//-------------------------------------------------------------------------------------------------

uint32_t DIRECT_TABLE_CRC32(uint8_t * ptr, uint32_t len) 
{
	uint32_t  crc = 0; //0xffffffff; 
	uint8_t * p= ptr;
	uint32_t i, j;

	// �ͦ�CRC32 ���q�� , �ĤG���O04C11DB7
	uint32_t gx = 0x04c11db7;
	uint32_t nData32;
	uint32_t nAccum32;
	uint32_t table[256];

	for(i = 0; i < 256; i++ )
	{
		nData32 = ( unsigned long )( i << 24 );
		nAccum32 = 0;
		for(j = 0; j < 8; j++ )
		{
			if( ( nData32 ^ nAccum32 ) & 0x80000000L )
				nAccum32 = ( nAccum32 << 1 ) ^ gx;
			else
				nAccum32 <<= 1;
			nData32 <<= 1;
		}
		table[i] = nAccum32;
	}

	for( i=0; i<len; i++ )
	{
		crc = ( crc << 8 ) ^ table[( crc >> 24 ) ^ (uint8_t)(*(p+i))];
	}

	return crc;
}

// ------------------------------------------------------------------------------------------------

uint8_t GetCrc8(uint8_t *ptr, uint32_t len, uint8_t MSG)	//X8+X2+X+1
{
	uint8_t *data = ptr;
	uint32_t crc = 0;
	uint32_t i, j;
	for (j = len; j; j--, data++)
	{
		crc ^= (*data << 8);
		for(i = 8; i; i--)
		{
			if (crc & 0x8000)
				crc ^= (0x1070 << 3);
			crc <<= 1;
		}
	}
	if ( MSG )
		LOGD("GetCrc8 = 0x%02X\n", (crc >> 8));

	return (uint8_t)(crc >> 8);
}

//-------------------------------------------------------------------------------------------------

// to generate a CRC for csData
int Get_CRC_16(uint8_t * buffer, uint32_t dwSize)
{
	// Be sure to use unsigned variables,
	// because negative values introduce high bits
	// where zero bits are required.
	unsigned short  crc=0xFFFF;//(0xffff);
	int len;
	uint8_t i;
//	unsigned char* buffer;

	len = dwSize;
	// Save the text in the buffer.
//	buffer = (unsigned char*)(LPCTSTR)csData;

	for (; len>0; len--)               /* Step through bytes in memory */
	{
		crc = (unsigned short)(crc^ (*buffer++));      /* Fetch byte from memory, XOR into CRC top byte*/
		for (i=0; i<8; i++)              /* Prepare to rotate 8 bits */
		{
			if (crc & 0x0001)              /* b15 is set... */
			{
				crc = (crc >>1) ^ poly;     
			}
			else 
			{
				crc >>=1;              /* just rotate */ 
			}
			/* and ensure CRC remains 16-bit value */
		} 

	}  /* Loop until num=0 */
    
	crc = ~crc;
	//data = crc;
	//crc = (crc << 8) | (data >> 8 & 0xff);

	return(crc);  

}

// ------------------------------------------------------------------------------------------------

uint32_t Verify_CRC32( uint8_t * Data, uint32_t LengthWithCRC32 )
{
	uint32_t Length = LengthWithCRC32 -4;
	uint32_t CRC32 = 0;
	uint8_t	MSG = 0;

	if ( ( LengthWithCRC32 < 4 ) || ( LengthWithCRC32 % 4 ) )	// Protection
		return Fail_State;

	CRC32 = DIRECT_TABLE_CRC32(Data, Length);
	if ( MSG )	LOGD("    Get_CRC32: 0x%X \n", CRC32);
	if ( 	( Data[Length] != (uint8_t)CRC32 )			||
		( Data[Length+1] != (uint8_t)(CRC32>>8) ) 	||
		( Data[Length+2] != (uint8_t)(CRC32>>16) ) 	||
		( Data[Length+3] != (uint8_t)(CRC32>>24) )	)
	{
		return Fail_State;
	}

	return Success_State;
}

// ------------------------------------------------------------------------------------------------
//    Sherlock_20210823						817B0	817B0	817C0
//    Reg[0x808F]:		ProjID_1		e.g.: 0x35	0x35	0x35	0x35
//    Reg[0x808E]:		ProjID_2		e.g.: 0x18	0x38	0x38	0x38
//    Reg[0x808D]		--
//    Reg[0x808C]:		Version		e.g.: 0xF0	0xB0	0xB0	0xC0
//    Reg[0x6051]:		Package		e.g.: 0x05	0x03	0x03	0x03
//
//    Bin[0x801E]:		ProjID_1		e.g.: 0x35	0x0F		0x35	0x35
//    Bin[0x801F]:		ProjID_2		e.g:. 0x18	0x00	0x38	0x38
//    Bin[0x8020]		--
//    Bin[0x8021]:		Version		e.g.: 0xF0	0x30	0xB0	0xC0
//    Bin[AddOfs]:		Package		e.g.: 0x05	0x03	0x03	0x02

uint32_t doesHubFwMatchIC( libusb_device_handle *devh, uint8_t * BinData, uint8_t MSG )
{
	uint32_t	Status = Fail_State;
	uint8_t	Project1_Reg, Project2_Reg, Version_Reg, Package_Reg;
	uint8_t	Project1_Bin, Project2_Bin, Version_Bin, Package_Bin;
	uint8_t	BinVer1;        // Temp_VL82x_Bin_Package_type
	uint8_t	cdb[8];
	uint32_t	Addr;

	LOGD("    check_matching\n");

	// ---------- PREPARE_INFORMATION ----------
	// Prepare_Chip_Info_From_Register
	Status = Read_Reg(devh, 0xF8, 0x8F, cdb);
	Project1_Reg = cdb[0];
	Status = Read_Reg(devh, 0xF8, 0x8E, cdb);
	Project2_Reg = cdb[0];
	Status = Read_Reg(devh, 0xF8, 0x8C, cdb);
	Version_Reg= cdb[0];
	Status = Read_Reg(devh, 0xF6, 0x51, cdb);
	Package_Reg = (cdb[0] >> 1) & 0x07;

	// Prepare_Firmware_Info_From_BinFile
	Project1_Bin = BinData[0x801E];
	Project2_Bin = BinData[0x801F];
	Version_Bin = BinData[0x8021];
	Addr = 0x2000 + BinData[0x8018] * 0x100 + BinData[0x8019];
	BinVer1 = BinData[Addr] & 0x07;
	if ( ( BinVer1 == 0x05 ) || ( BinVer1 == 0x07 ) )	// Is_VL819
		Package_Bin = BinVer1;
	else											// Is_VL821_VL822
		Package_Bin = ( ( BinVer1 & 0x03 ) << 1 ) | ( ( BinVer1 & 0x04 ) >> 2 );
	if ( MSG )
	{
		LOGD("    Reg ID=0x%02X%02X, Ver=0x%02X, Pkt=0x%02X \n", Project1_Reg, Project2_Reg, Version_Reg, Package_Reg);
		LOGD("    Bin ID=0x%02X%02X, Ver=0x%02X, Pkt=0x%02X \n", Project1_Bin, Project2_Bin, Version_Bin, Package_Bin);
	}


	// ---------- VT3538_CASE ----------
	if ( ( Project1_Reg == 0x35 ) && ( Project2_Reg == 0x38 ) )
	{
		if ( ( Project1_Bin == 0x35 ) && ( Project2_Bin == 0x38 ) && ( Version_Reg == Version_Bin ) ) // Basic_Rule
			return Success_State;
		else if ( ( BinData[0] == 0x05 ) && ( BinData[1] == 0x38 ) && ( Version_Reg == 0xB0 ) && ( Version_Bin != 0xC0 ) ) // Chip_B0, Bin_Not_C0
		{
			if ( ( ( Package_Reg >> 1 ) & 0x01 ) == BinVer1 )	// B0_Reg_PKT = (Reg[F651]>>2) & 0x1;
				return Success_State;						// B0_Bin_PKT = BinData[Addr] & 0x07;
		}

		goto MISMATCH_DATA;
	}


	// ---------- VT3518_CASE ----------
	if ( ( Project1_Reg == 0x35 ) && ( Project2_Reg == 0x18 ) )
	{
		if ( ( Project1_Bin != 0x35 ) || ( Project2_Bin != 0x18 ) )
			goto MISMATCH_DATA;

		// Judge_3518_Package
		if ( ( Version_Reg != 0xF0 ) && ( Version_Bin != 0xF0 ) ) // 3518 & Not_F0, Return_OK
			return Success_State;
		if ( ( Version_Reg == 0xF0 ) && ( Version_Bin != 0xF0 ) )	goto MISMATCH_DATA;
		if ( ( Version_Reg != 0xF0 ) && ( Version_Bin == 0xF0 ) )	goto MISMATCH_DATA;

		if ( Package_Reg == Package_Bin )    // 3518 F0 & Package_Match, Return_OK
			return Success_State;

		goto MISMATCH_DATA;
	}


	// ---------- OTHER_CASE ----------
	if ( ( Project1_Reg == Project1_Bin ) && ( Project2_Reg == Project2_Bin ) && ( Version_Reg == Version_Bin ) ) // Basic_Rule
		return Success_State;

	goto MISMATCH_DATA;

MISMATCH_DATA:
	LOGD("    Reg ID=0x%02X%02X, Ver=0x%02X, Pkt=0x%02X \n", Project1_Reg, Project2_Reg, Version_Reg, Package_Reg);
	LOGD("    Bin ID=0x%02X%02X, Ver=0x%02X, Pkt=0x%02X \n", Project1_Bin, Project2_Bin, Version_Bin, Package_Bin);
	return Fail_State;
}

//-------------------------------------------------------------------------------------------------
/*
uint32_t doesFwMatchIC( libusb_device_handle *devh, uint8_t * Data )
{
	uint32_t Status = Fail_State;
	uint8_t Value;
	Status = Read_Reg( devh, (uint8_t)0xF8, (uint8_t)0x8C, &Value);
	if ( Data[1] == Value )
		Status = Success_State;
	return Status;
}
*/
//-------------------------------------------------------------------------------------------------

// Sherlock_20181119, New Function For Update PD Firmware To Single Hub
uint32_t PD_FW_Update_Exe( libusb_device_handle *devh, uint8_t SPI_Type )
{
	uint32_t	Status = Fail_State;
	uint8_t	*TxDataBuf, *RxDataBuf;
	FILE		*FWBinFile=NULL;
	uint32_t	TotalByte, idx, erasing;
	uint8_t	bReadData;
	uint32_t	Write1_Start_Adr, Write2_Start_Adr;
	uint8_t	MSG = gDbgMsg;
	uint16_t	CRC16;
	bool	FW1OK = false;
//	errno_t	err;
//	SYSTEMTIME st;
//	GetLocalTime(&st);

	LOGD( "\nPD_FW_Update_Start...\n" );

	bReadData = SPI_DataBase[SPI_Type][4];
	if( SPI_DataBase[SPI_Type][8] == 0 )
	{
		LOGD( "Protection : SPI_No_SectorErase_CMD.\n" );
		return Fail_State;
	}

	TxDataBuf = (uint8_t *)malloc( SIZE_128KB );
	RxDataBuf = (uint8_t *)malloc( SIZE_128KB );
	memset( TxDataBuf, 0, SIZE_128KB );
	memset( RxDataBuf, 0, SIZE_128KB );


	// ---------- 1. Read_PDBin_File_into_TxDataBuf[128K]_&_Check_FW_Size/Info ----------
//	err = fopen_s(&FWBinFile, PD1Bin, "rb");
//	err = fopen_s(&FWBinFile, "PD_FW.bin", "r");
	FWBinFile = fopen( PD1Bin, "rb" );
//	FWBinFile = _tfopen( PD1Bin, _T("rb") ); //Open_PDFW_With_Path
	
	if ( !FWBinFile )
	{
		LOGD("Protection : FW_Open_Fail.\n");
		goto END_OF_PD_FWUPDATE;
	}

	fseek( FWBinFile, 0, SEEK_END );
	TotalByte = ( uint32_t )ftell( FWBinFile );
	if ( TotalByte > SIZE_128KB )
	{
		LOGD("Protection : PDFW_Size_Over.\n");
		if ( FWBinFile )
			fclose( FWBinFile );
		goto END_OF_PD_FWUPDATE;
	}

	fseek( FWBinFile, 0, SEEK_SET );
	fread( TxDataBuf, 1, TotalByte, FWBinFile );
	if ( FWBinFile )
		fclose( FWBinFile );

	if ( TxDataBuf[0]  != 0x02 )
	{
		LOGD("Protection : PDFW_Data[0]_Error.\n");
		goto END_OF_PD_FWUPDATE;
	}

//	Status = VL10x_Check_FW( TxDataBuf[0x1003], TotalByte );
//	if ( Status != Success_State )
//	{
//		showMsg("Protection: Bin_Not_Match:PID=%04X, FID=%X, Size=%04X\n", TempPID, TxDataBuf[0x1003]&0x0F, TotalByte );
//		Status = Fail_State;
//		goto END_OF_PD_FWUPDATE;
//	}
	CRC16 = (uint16_t)Get_CRC_16(TxDataBuf, TotalByte - 2);
	if ( ( TxDataBuf[TotalByte-1] != (uint8_t)(CRC16>>8) ) || ( TxDataBuf[TotalByte-2] != (uint8_t)(CRC16) ) )
	{
		LOGD("Protection: Bin_CRC16=%04X\n", CRC16);
		Status = Fail_State;
		goto END_OF_PD_FWUPDATE;
	}


	// ---------- 2. Read_SPI_0x20000_into_RxBuf[]_&_Check_FW ----------
	LOGD( "    Checking_Current_2wh_Data\n" );
	Status = SPI_Read_Data( devh, bReadData, 0x20000, 0x00, 0x20, RxDataBuf );
	if ( RxDataBuf[0] == 0x05 )
	{
		LOGD("Protection : SPI_Size_Error.\n");
		goto END_OF_PD_FWUPDATE;
	}

	LOGD("    Check device firmware ...\n");
	for ( idx = 0; idx < TotalByte; idx+=0x20 )
	{
		Status = SPI_Read_Data( devh, bReadData, (0x20000 + idx), idx, 0x20, RxDataBuf);
	}
	CRC16 = (uint16_t)Get_CRC_16(RxDataBuf, TotalByte - 2);
	if ( ( RxDataBuf[TotalByte-1] == (uint8_t)(CRC16>>8) ) && ( RxDataBuf[TotalByte-2] == (uint8_t)(CRC16) ) )
	{
		FW1OK = true;
		LOGD("    Firmwware_1_OK ...\n");
	}

	if ( FW1OK )
	{
		if ( TotalByte == 0x8000 )	Write1_Start_Adr = 0x28000;
		else						Write1_Start_Adr = 0x30000;
		Write2_Start_Adr = 0x20000;
	}
	else
	{
		Write1_Start_Adr = 0x20000;
		if ( TotalByte == 0x8000 )	Write2_Start_Adr = 0x28000;
		else						Write2_Start_Adr = 0x30000;
	}


	// ---------- 3. Update_PDFW_To_0x28000_Area ----------
	LOGD("    Write_FW_to_%X ...\n", Write1_Start_Adr);
	for ( idx = 0; idx < TotalByte; idx+=0x1000 )
	{
		Status = SPI_SeRdCmp_Exe( devh, SPI_Type, (Write1_Start_Adr + idx), MSG );
		if ( Status != Success_State )
		{
			LOGD( "W1_SectorErase_%06X_Fail\n", idx );
			goto END_OF_PD_FWUPDATE;
		}
		erasing = ( idx * 10 ) / TotalByte;
		LOGD( "\r    erasing...%01d0%%", erasing ); // display_10_20_30...
	}
	LOGD("\r                    \r"); // Clean_progress

	Status = SPI_WrRdCmp_Exe( devh, SPI_Type, Write1_Start_Adr, 0x00, TotalByte, TxDataBuf, MSG );
	if ( Status != Success_State )
	{
		LOGD( "Write_FW1_Fail\n" );
		goto END_OF_PD_FWUPDATE;
	}

	
	// ---------- 4. Update_PDFW_To_0x20000_Area ----------
	LOGD("    Write_FW_to_%X ...\n", Write2_Start_Adr);
	for ( idx = 0; idx < TotalByte; idx+=0x1000 )
	{
		Status = SPI_SeRdCmp_Exe( devh, SPI_Type, (Write2_Start_Adr + idx), MSG );
		if ( Status != Success_State )
		{
			LOGD( "W2_SectorErase_%06X_Fail\n", idx );
			goto END_OF_PD_FWUPDATE;
		}
		erasing = ( idx * 10 ) / TotalByte;
		LOGD( "\r    erasing...%01d0%%", erasing ); // display_10_20_30...
	}
	LOGD("\r                    \r"); // Clean_progress

	Status = SPI_WrRdCmp_Exe( devh, SPI_Type, Write2_Start_Adr, 0x00, TotalByte, TxDataBuf, MSG );
	if ( Status != Success_State )
	{
		LOGD( "Write_FW2_Fail\n" );
		goto END_OF_PD_FWUPDATE;
	}


END_OF_PD_FWUPDATE:

//	TotalByte = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	GetLocalTime(&st);
//	idx = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	showMsg("  Update_Use_Time = %dm %ds", (idx-TotalByte)/60, (idx-TotalByte)%60);


	if ( Status == Success_State )
	{
		LOGD("\n===========================\n");
		LOGD("=    PD_FW_Update_PASS.   =\n");
		LOGD("===========================\n");
	}
	else // Just Show Msg
	{
		LOGD("\n===========================\n");
		LOGD("=    PD_FW_Update_FAIL.   =\n");
		LOGD("===========================\n");
	}

	if ( TxDataBuf )
		free( TxDataBuf );
	if ( RxDataBuf )
		free( RxDataBuf );

	return Status;

}

//-------------------------------------------------------------------------------------------------

// Sherlock_20210609, Read_SPI_to_PDFWBackup.bin_PDFWBackupAll.bin
uint32_t PD_FW_Backup_Exe( libusb_device_handle *devh, uint8_t SPI_Type )
{
	uint32_t	Status = Fail_State;
	uint8_t	*RxDataBuf;
	FILE		*FWBkFile1=NULL;
	FILE		*FWBkFile2=NULL;
	uint32_t	TotalByte=SIZE_64KB, idx;
	uint8_t	bReadData;
//	errno_t	err;
//	SYSTEMTIME st;
//	GetLocalTime(&st);

	LOGD( "\nPD_FW_Backup_Start...\n" );

	bReadData = SPI_DataBase[SPI_Type][4];
	RxDataBuf = (uint8_t *)malloc( SIZE_128KB );
	memset( RxDataBuf, 0, SIZE_128KB );


	// ---------- 1. Open/Create_Target_Dump_Bin_Files ----------
//	err = fopen_s(&FWBkFile1, "PDFWBackup.bin", "wb");
	FWBkFile1 = fopen("PDFWBackup.bin", "wb");
	if ( !FWBkFile1 )
	{
		LOGD("Protection : Bk1_Open_Fail.\n");
		goto END_OF_PD_FWBACKUP;
	}
//	err = fopen_s(&FWBkFile2, "PDFWBackupAll.bin", "wb");
	FWBkFile2 = fopen("PDFWBackupAll.bin", "wb");
	if ( !FWBkFile2 )
	{
		LOGD("Protection : Bk2_Open_Fail.\n");
		goto END_OF_PD_FWBACKUP;
	}

/*
	// ---------- 2. Read_SPI_0x20000~0x40000_into_RxBuf[]_&_Check_CRC ----------
	if (( TempPID == PID_104 ) || ( TempPID == PID_105))		TotalByte = SIZE_104;	// 48K
	else if ( TempPID == PID_107 )							TotalByte = SIZE_107;	// 50K
	else														TotalByte = SIZE_103;	// 32K

	if ( TotalByte == SIZE_103 )		FW2_Start_Offset = 0x8000;	// Ofs_32K
	else							FW2_Start_Offset = 0x10000;	// Ofs_64K
*/	
	for ( idx = 0; idx < SIZE_128KB; idx+=0x20 )	// Read_All_128K
	{
		Status = SPI_Read_Data( devh, bReadData, (0x20000 + idx), idx, 0x20, RxDataBuf);
	}
/*
	CRC16 = (USHORT)Get_CRC_16(RxDataBuf, TotalByte - 2);
	if ( ( RxDataBuf[TotalByte-1] == HIBYTE(CRC16) ) && ( RxDataBuf[TotalByte-2] == LOBYTE(CRC16) ) )
		showMsg("  Firmwware_1_OK ...\n");

	CRC16 = (USHORT)Get_CRC_16((RxDataBuf + FW2_Start_Offset), TotalByte - 2);
	if ( ( RxDataBuf[FW2_Start_Offset+TotalByte-1] == HIBYTE(CRC16) ) && ( RxDataBuf[FW2_Start_Offset+TotalByte-2] == LOBYTE(CRC16) ) )
		showMsg("  Firmwware_2_OK ...\n");
*/
	// ---------- 3. Dump_Data_To_Files ----------
	fwrite(RxDataBuf, 1, TotalByte, FWBkFile1);	// Only_Backup_FW1
	fclose(FWBkFile1);
	fwrite(RxDataBuf, 1, SIZE_128KB, FWBkFile2);	// Backup_0x20000~0x40000
	fclose(FWBkFile2);


END_OF_PD_FWBACKUP:

//	TotalByte = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	GetLocalTime(&st);
//	idx = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	showMsg("    Backup_Use_Time = %dm %ds", (idx-TotalByte)/60, (idx-TotalByte)%60);


	if ( Status == Success_State )
	{
		LOGD("\n===========================\n");
		LOGD("=    PD_FW_Backup_PASS.   =\n");
		LOGD("===========================\n");
	}
	else // Just Show Msg
	{
		LOGD("\n===========================\n");
		LOGD("=    PD_FW_Backup_FAIL.   =\n");
		LOGD("===========================\n");
	}

	if ( RxDataBuf )
		free( RxDataBuf );

	return Status;
}

//-------------------------------------------------------------------------------------------------
// Sherlock_20180202, New Function For Update 1 Hub With Protect FW1
uint32_t Hub_FW_Update_Exe( libusb_device_handle *devh, uint8_t SPI_Type )
{
	uint32_t	Status = Fail_State;//, TempSts;
	uint8_t	*TxDataBuf, TempByteBuf[0x20];
	uint8_t	*SPIDataBuf, *HeaderBuf;
	FILE		*FWBinFile=NULL;
	uint32_t	TotalByte, RemainLen, TransferLen, AddrOffset, idx, erasing, MaxFWTxSize = 0x20;
	uint8_t	bReadData, bCheckSum;
	uint32_t	FW1_Length, FW2_Length, FW2_Start_Adr, BinFW_Start_Adr, HD1_Index = 0xFF;
	uint8_t	Update_HD1 = 0, Write_Default_FW1 = 0, MSG = gDbgMsg;
//	errno_t	err;
//	SYSTEMTIME st;
//	GetLocalTime(&st);

	bReadData = SPI_DataBase[SPI_Type][4];
	if( SPI_DataBase[SPI_Type][8] == 0 )
	{
		LOGE( "Protection : SPI_No_SectorErase_CMD.\n" );
		return Fail_State;
	}

	TxDataBuf = (uint8_t *)malloc( SIZE_128KB );
	SPIDataBuf =(uint8_t *)malloc( SIZE_128KB );
	HeaderBuf = (uint8_t *)malloc( 0x20 );
	memset( TxDataBuf, 0, SIZE_128KB );
	memset( SPIDataBuf, 0, SIZE_128KB );
	memset( HeaderBuf, 0, 0x20 );


	// ---------- 1. Read Bin_File into TxDataBuf[128K] & Check Size/Info ----------
//	err = fopen_s(&FWBinFile, Hub1Bin, "rb");
//	err = fopen_s(&FWBinFile, "Hub_FW.bin", "rb");
	FWBinFile = fopen( Hub1Bin, "rb" );
//	FWBinFile = _tfopen( Hub1Bin,  _T("rb") ); // Open_HubFW_With_Path
	if ( !FWBinFile )
	{
		LOGE("Protection : FW_Open_Fail.\n");
		goto END_OF_HUB_FWUPDATE;
	}

	fseek( FWBinFile, 0, SEEK_END );
	TotalByte = ( uint32_t )ftell( FWBinFile );
	if ( TotalByte > SIZE_128KB )
	{
		LOGE("Protection : FW_Size_Over.\n");
		if ( FWBinFile )
			fclose( FWBinFile );
		goto END_OF_HUB_FWUPDATE;
	}

	fseek( FWBinFile, 0, SEEK_SET );
	fread( TxDataBuf, 1, TotalByte, FWBinFile );
	if ( FWBinFile )
		fclose( FWBinFile );
//	if ( ( TxDataBuf[13] & 0x02) == 0x00 )	
//	{
//		LOGD("Bin_File Not Support Billboard_Update.\n");
//		goto END_OF_HUB_FWUPDATE;	Skip Check Billboard Flag?
//	}
//	pByte = (uint8_t *)CtrlDataBuf;
//	for(idx=0; idx<(int)TotalByte; idx++)
//	{
//		pByte[idx] = TxDataBuf[idx];
//	}
/*
	if ( TempPID == 0x8830 ) // Sherlock_20210121
	{
		Status = Do3566ChallengeExe( WinusbHandle );
		Write_Default_FW1 = 1;
		goto DEFAULT_FW_DL;
	}
*/
//	Status = doesFwMatchIC( WinusbHandle, TxDataBuf );
	Status = doesHubFwMatchIC( devh, TxDataBuf, MSG );
	if ( Status != Success_State )
	{
		LOGE("    BinFile doesn't match IC.\n");
		goto END_OF_HUB_FWUPDATE;
	}

	// ---------- 2. Read Original_SPI into SPIBuf[128K] And Check Header1 ----------
	LOGD( "    Checking_Current_Firmware...\n" );
	RemainLen = SIZE_128KB;
	AddrOffset = 0;
	while( RemainLen != 0 )
	{
		if ( RemainLen > MaxFWTxSize )
			TransferLen = MaxFWTxSize;
		else
			TransferLen = RemainLen;

		Status = SPI_Read_Data( devh, bReadData, AddrOffset, AddrOffset, TransferLen, SPIDataBuf );
		if( Status != Success_State ) 
		{
			LOGE( "Protection : SPI_Check_Read_Fail.\n");
			goto END_OF_HUB_FWUPDATE;
		}

		RemainLen-=TransferLen;
		AddrOffset+=TransferLen;
	}
	
	if((SPIDataBuf[1] == 0x07) || ((SPIDataBuf[1] == 0x18) && (SPIDataBuf[0x1E] == 0x1))) // V1 Use XOR
	{
		LOGE("Protection : Not_Support_Version\n");
		goto END_OF_HUB_FWUPDATE;
	}

	for ( idx = 0; idx < 0x20; idx++ )
		TempByteBuf[idx] = (uint8_t)SPIDataBuf[idx];

	bCheckSum = GetCrc8( TempByteBuf, 0x20-1, MSG );
	if ( ( bCheckSum == (uint8_t)SPIDataBuf[0x20-1] ) && ( (uint8_t)SPIDataBuf[0x1C] == 0xFF ) )	// CheckSum OK && Pre_Pointer Is Root Header
	{	// In Most Cases, [0x1C] Is 0xFF, But Still Need To Check This
		HD1_Index = 0;
		if ( (uint8_t)SPIDataBuf[0x01] != TxDataBuf[0x01] )
		{
			LOGD("Protection : Firmware_Mismatch.\n");
		}

		if ( (uint8_t)SPIDataBuf[0x1D] != 0x80 ) // Next_Pointer Is NOT Our Target, We Need To Change It
		{
			Update_HD1 = 1;

			// Always Prepare Header1 Buffer While Set Update_HD1_Flag
			for( idx = 0; idx<0x20; idx++ )
				HeaderBuf[idx] = SPIDataBuf[idx];

			HeaderBuf[0x1D] = (uint8_t)0x80;							// Set Correct Next_Pointer

			for ( idx = 0; idx < 0x20; idx++ )
				TempByteBuf[idx] = (uint8_t)HeaderBuf[idx];

			bCheckSum = GetCrc8( TempByteBuf, 0x20-1, MSG );
			HeaderBuf[0x20-1] = (uint8_t)bCheckSum;			
		}
		
		LOGD( "    Found_HD1_at_%02X_With_Update_%d\n", HD1_Index, ( uint8_t )Update_HD1 );

	}
	else // HD1 Not In Index 0, Search Root_Header In Backup_Zone
	{
		for( idx = 0; idx<0x20; idx++ )
		 	HeaderBuf[idx] = SPIDataBuf[0x1800 + idx];

		for ( idx = 0; idx < 0x20; idx++ )
			TempByteBuf[idx] = (uint8_t)HeaderBuf[idx];

		bCheckSum = GetCrc8( TempByteBuf, 0x20-1, MSG );
		if ( ( bCheckSum == (uint8_t)HeaderBuf[0x20-1] ) && ( (uint8_t)HeaderBuf[0x1C] == 0xFF ) )	// CheckSum_OK && PrePoint_Is_Root_Header
		{
			HD1_Index = 0xC0;
			Update_HD1 = 1;
			if ( (uint8_t)HeaderBuf[0x01] != TxDataBuf[0x01] )
			{
				LOGE("Protection : Firmware_Mismatch..\n");
				goto END_OF_HUB_FWUPDATE;
			}

			// Always Prepare Header1 Buffer While Set Update_HD1_Flag
			for( idx = 0; idx<0x20; idx++ )
				HeaderBuf[idx] = SPIDataBuf[0x1800 + idx];
			HeaderBuf[0x1D] = (uint8_t)0x80;			// Set Correct Next_Pointer
			
			for ( idx = 0; idx < 0x20; idx++ )
				TempByteBuf[idx] = (uint8_t)HeaderBuf[idx];

			bCheckSum = GetCrc8( TempByteBuf, 0x20-1, MSG );
			HeaderBuf[0x20-1] = (uint8_t)bCheckSum;	

			LOGD( "    Found_HD1_at_%02X_With_Update_%d\n", HD1_Index, ( uint8_t )Update_HD1 );
		}
		else
		{
			LOGD( "Protection : No_Default_FW\n" );
			Write_Default_FW1 = 1;
			goto DEFAULT_FW_DL;
		}
	}


	// ---------- 3. Update Header_1 If Need----------
	if ( Update_HD1 )
	{
		LOGD( "    Updating_Firmware_Header_1...\n" );
		if ( HD1_Index == 0 ) // We Need To Backup Header1 at 0x1800 Before Update Index_0
		{
			if ( MSG )
				LOGD( "BackUp_Header_1\n" );
			
			Status = SPI_SeRdCmp_Exe( devh, SPI_Type, 0x1000, MSG );
			if ( Status != Success_State )
			{
				LOGE( "Backup_Header1_SectorErase_Fail\n" );
				goto END_OF_HUB_FWUPDATE;
			}

			Status = SPI_WrRdCmp_Exe( devh, SPI_Type, 0x1800, 0, 0x20, HeaderBuf, MSG );
			if ( Status != Success_State )
			{
				LOGE( "Backup_Header1_WriteRead_Fail\n" );
				goto END_OF_HUB_FWUPDATE;
			}
		}

		if ( MSG )
			LOGD( "Update_Header_1\n" );

		Status = SPI_SeRdCmp_Exe( devh, SPI_Type, 0, MSG );
		if ( Status != Success_State )
		{
			LOGE( "Update_Header1_SectorErase_Fail\n" );
			goto END_OF_HUB_FWUPDATE;
		}

		Status = SPI_WrRdCmp_Exe( devh, SPI_Type, 0, 0, 0x20, HeaderBuf, MSG );
		if ( Status != Success_State )
		{
			LOGE( "Update_Header1_WriteRead_Fail\n" );
			goto END_OF_HUB_FWUPDATE;
		}

		if ( MSG )
		{
			for ( idx=0; idx<0x20; idx+=8 )
			{
				LOGD("  Header_1 = %02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,\n",
					HeaderBuf[idx+0],HeaderBuf[idx+1],HeaderBuf[idx+2],HeaderBuf[idx+3],
					HeaderBuf[idx+4],HeaderBuf[idx+5],HeaderBuf[idx+6],HeaderBuf[idx+7]);
			}
		}
	}


	// ---------- 4. Update FW_2 To FW_2_Area ----------
	// Here, We Must have a Correct Header1 at SPI_Index_0, So We Can Get FW1_Len 
	LOGD( "    Updating_Firmware_Data...\n" );
	Status = SPI_Read_Data( devh, bReadData, 0, 0, 0x20, HeaderBuf );
	FW1_Length = ((((uint32_t)HeaderBuf[6])&0xFF)<<8) + (((uint8_t)HeaderBuf[7])&0xFF);
	if( ( FW1_Length < 0x1000) || ( FW1_Length > 0xF000) ) // Protection
	{
		LOGD( "FW1_Size_%04X_Abnormal\n", FW1_Length );
		goto END_OF_HUB_FWUPDATE;	
	}

//	FW2_Start_Adr = ( FW1_Length + 0xFFF ) & 0xF000;	// Makes Size as 0xK000
//	FW2_Start_Adr = 0x2000 + FW2_Start_Adr;			// Add Header Zone
	FW2_Start_Adr = 0x10000;
	FW2_Length =  ((((uint32_t)TxDataBuf[6])&0xFF) << 8) + TxDataBuf[7] + 12; // 12_Bytes_For_FW_date_2019xxxx
	BinFW_Start_Adr = (((uint32_t)TxDataBuf[4])<<8) + (uint8_t)SPIDataBuf[5];
	if ( MSG )
	{
		LOGD( "FW2_Start_Adr_%06X\n", FW2_Start_Adr );
		LOGD( "FW2_Length_0x%04X\n", FW2_Length );
	}

	for ( idx=FW2_Start_Adr; idx<( FW2_Start_Adr+FW2_Length ); idx+=0x1000 )
	{
		Status = SPI_SeRdCmp_Exe( devh, SPI_Type, idx, MSG );
		if ( Status != Success_State )
		{
			LOGD( "FW2_SectorErase_%06X_Fail\n", idx );
			goto END_OF_HUB_FWUPDATE;
		}
		erasing = ( ( idx - FW2_Start_Adr ) * 10 ) / FW2_Length;
		LOGD( "\r    erasing...%01d0%%", erasing ); // display_10_20_30...
	}
	LOGD("\r                    \r"); // Clean_progress

	Status = SPI_WrRdCmp_Exe( devh, SPI_Type, FW2_Start_Adr, BinFW_Start_Adr, FW2_Length, TxDataBuf, MSG );
	if ( Status != Success_State )
	{
		LOGD( "Write FW2 Fail\n" );
		goto END_OF_HUB_FWUPDATE;
	}

	// ---------- 5. Update Header_2 To Header_Index_0x80 ----------
	LOGD( "    Updating_Firmware_Header_2...\n" );
	for( idx=0; idx<0x20; idx++ )
		HeaderBuf[idx] = (uint8_t)TxDataBuf[idx];

	HeaderBuf[0x05] = ( uint8_t )( FW2_Start_Adr );			// FW Start Address_L
	HeaderBuf[0x04] = ( uint8_t )( FW2_Start_Adr >> 8 );		// FW Start Address_M
	HeaderBuf[0x0C] = ( uint8_t )( FW2_Start_Adr >> 16 );		// FW Start Address_H
	HeaderBuf[0x1C] = ( uint8_t )0x00;						// Pre_Pointer
	HeaderBuf[0x1D] = ( uint8_t )0xFF;						// Next_Pointer

	for ( idx = 0; idx < 0x20; idx++ )
		TempByteBuf[idx] = (uint8_t)HeaderBuf[idx];
	
	bCheckSum = GetCrc8(TempByteBuf, 0x20-1, MSG);
	HeaderBuf[0x20-1] = (uint8_t)bCheckSum;		

	Status = SPI_SeRdCmp_Exe( devh, SPI_Type, 0x1000, MSG );
	if ( Status != Success_State )
	{
		LOGD("Update_Header2_SectorErase_Fail\n");
		goto END_OF_HUB_FWUPDATE;
	}
	
	Status = SPI_WrRdCmp_Exe( devh, SPI_Type, 0x1000, 0, 0x20, HeaderBuf, MSG );
	if ( Status != Success_State )
	{
		LOGD( "Update_Header2_WriteRead_Fail\n" );
		goto END_OF_HUB_FWUPDATE;
	}

	if ( MSG )
	{
		for ( idx=0; idx<0x20; idx+=8 )
		{
			LOGD( "  Header_2 = %02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,\n",
				HeaderBuf[idx+0],HeaderBuf[idx+1],HeaderBuf[idx+2],HeaderBuf[idx+3],
				HeaderBuf[idx+4],HeaderBuf[idx+5],HeaderBuf[idx+6],HeaderBuf[idx+7] );
		}
	}


DEFAULT_FW_DL:

	// ---------- 6. If NO Header1 Or Rom Code Update, Download Bin File Directly----------
	if ( Write_Default_FW1 )
	{
		LOGD( "Default_Firmware_Download...\n" );

		for ( idx=0; idx<TotalByte; idx+=0x1000 )
		{
			Status = SPI_SeRdCmp_Exe( devh, SPI_Type, idx, MSG );
			if ( Status != Success_State )
			{
				LOGD( "Bin_SectorErase_%06X_Fail\n", idx );
				goto END_OF_HUB_FWUPDATE;
			}
		}
	
		Status = SPI_WrRdCmp_Exe( devh, SPI_Type, 0, 0, TotalByte, TxDataBuf, MSG ); // Sherlock_20180323
		if(Status != Success_State)
		{
			LOGD( "Bin_WriteRead_Fail\n" );
			goto END_OF_HUB_FWUPDATE;
		}		
	}


	if ( gHubPDShareSPI )
	{
		if ( Status == Success_State )
		{
			Status = PD_FW_Update_Exe( devh, SPI_Type );
		}
	}

END_OF_HUB_FWUPDATE:

//	TotalByte = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	GetLocalTime(&st);
//	RemainLen = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	LOGD( "    Use_Time = %d seconds \n", (RemainLen-TotalByte)  );

	if ( Status == Success_State )
	{
		LOGD("\n===========================\n");
		LOGD("=   Hub_FW_Update_PASS.   =\n");
		LOGD("===========================\n");
	}
	else // Just Show Msg
	{
		LOGD("\n===========================\n");
		LOGD("=   Hub_FW_Update_FAIL.   =\n");
		LOGD("===========================\n");
	}


	if ( TxDataBuf )
		free( TxDataBuf );
	if ( SPIDataBuf )
		free( SPIDataBuf );
	if ( HeaderBuf )
		free( HeaderBuf );
	return Status;

}

//-------------------------------------------------------------------------------------------------

// Sherlock_20210609, Read_SPI_to_HubFWBackup.bin
uint32_t Hub_FW_Backup_Exe( libusb_device_handle *devh, uint8_t SPI_Type )
{
	uint32_t	Status = Fail_State;
	uint8_t	*RxDataBuf;
	FILE		*FWBkFile1=NULL;
	uint32_t	TotalByte, idx;
	uint8_t	bReadData, bCRC8;
//	errno_t	err;
//	SYSTEMTIME st;
//	GetLocalTime(&st);

	LOGD( "\nHub_FW_Backup_Start...\n" );

	bReadData = SPI_DataBase[SPI_Type][4];
	RxDataBuf = (uint8_t *)malloc( SIZE_128KB );
	memset( RxDataBuf, 0, SIZE_128KB );


	// ---------- 1. Open/Create_Target_Dump_Bin_Files ----------
//	err = fopen_s(&FWBkFile1, "HubFWBackup.bin", "wb");
	FWBkFile1 = fopen("HubFWBackup.bin", "wb");
	if ( !FWBkFile1 )
	{
		LOGD("Protection : Bk1_Open_Fail.\n");
		goto END_OF_Hub_FWBACKUP;
	}


	// ---------- 2. Read_SPI_0x0~0x20000_into_RxBuf[]_&_Check_CRC8_CRC32 ----------
	for ( idx = 0; idx < SIZE_128KB; idx+=0x20 )	// Read_All_128K
	{
		Status = SPI_Read_Data( devh, bReadData, idx, idx, 0x20, RxDataBuf);
	}

	bCRC8 = GetCrc8( &RxDataBuf[0], 0x20-1, 1 );
	if ( bCRC8 == RxDataBuf[0x20-1] )
	{
		TotalByte = RxDataBuf[6] * 0x100 + RxDataBuf[7];
		Status = Verify_CRC32(&RxDataBuf[0x2000],TotalByte);
		if (Status == Success_State )
			LOGD("    Default_FW_Check_OK ...\n");
		else
			LOGD("    Default_FW_Check_NG ...\n");
	}
	else
		LOGD("    Default_Header_NG ...\n");

	bCRC8 = GetCrc8( &RxDataBuf[0x1000], 0x20-1, 1 );
	if ( bCRC8 == RxDataBuf[0x1020-1] )
	{
		TotalByte = RxDataBuf[0x1006] * 0x100 + RxDataBuf[0x1007];
		Status = Verify_CRC32(&RxDataBuf[0x10000],TotalByte);
		if (Status == Success_State )
			LOGD("    Update_FW_Check_OK ...\n");
		else
			LOGD("    Update_FW_Check_NG ...\n");
	}
	else
		LOGD("    Update_Header_NG ...\n");

	
	// ---------- 3. Dump_Data_To_Files ----------
	fwrite(RxDataBuf, 1, SIZE_128KB, FWBkFile1);	// Only_Backup_FW1
	fclose(FWBkFile1);

	if ( gHubPDShareSPI )
		PD_FW_Backup_Exe( devh, SPI_Type );


END_OF_Hub_FWBACKUP:

//	TotalByte = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	GetLocalTime(&st);
//	idx = ( (st.wHour * 60) + st.wMinute) * 60 + st.wSecond;
//	LOGD("    Backup_Use_Time = %dm %ds", (idx-TotalByte)/60, (idx-TotalByte)%60);


	if ( Status == Success_State )
	{
		LOGD("\n===========================\n");
		LOGD("=    Hub_FW_Backup_PASS.  =\n");
		LOGD("===========================\n");
	}
	else // Just Show Msg
	{
		LOGD("\n===========================\n");
		LOGD("=    Hub_FW_Backup_FAIL.  =\n");
		LOGD("===========================\n");
	}

	if ( RxDataBuf )
		free( RxDataBuf );

	return Status;
}

//-------------------------------------------------------------------------------------------------

bool GetPDFWVersion(  libusb_device_handle *devh, uint8_t MSG )
{
	uint32_t	Status;
	uint32_t	Address = 0, idx;
	uint16_t	CurrVID;
	uint8_t	buf[0x20];
	bool		found = false;

	if ( MSG )
		LOGD( "Get_PD_FW.\n" );

	for ( idx=0; idx<3; idx++ )
	{
		if ( idx == 0 ) 		Address = 0x21003;
		else if ( idx == 1 )	Address = 0x29003;
		else if ( idx == 2 )	Address = 0x31003;
		Status = SPI_Read_Data( devh, 0x03, Address, 0x00, 0x20, buf );

		CurrVID = ( (uint16_t)buf[5] << 8 ) | (uint8_t)buf[4];
		if ( ( CurrVID == 0x2109 ) || ( CurrVID == 0x17EF ) || ( CurrVID == 0x04E8 ) || ( CurrVID == 0x1E68 ) || ( CurrVID == 0x2D1C ) )
		{
			found = true;
			Ver0 = (uint8_t)buf[3];
			Ver1 = (uint8_t)buf[2];
			Ver2 = (uint8_t)buf[1];
			Ver3 = (uint8_t)buf[0];
	
			LOGD( "=========================================\n" );
			LOGD( "=     PD FW Version is %02X.%02X.%02X.%02X      =\n", (uint8_t)buf[0], (uint8_t)buf[1], (uint8_t)buf[2], (uint8_t)buf[3]);
			LOGD( "=========================================\n" );
			break;
		}
	}

	if ( !found )
		LOGD( "PD firmware not found.\n" );

	return found;

}

//-------------------------------------------------------------------------------------------------

bool GetFWVersion( libusb_device_handle *devh, uint8_t MSG, char* strVersion)
{
	int		ret;
	uint8_t	Data[0x2];
	bool		found = false;

	ret = libusb_control_transfer(	devh, 
								USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN, 
								0x9F, 
								0x00, 
								0x00, 
								Data, 
								0x02, 
								1000	);
	if ( ret >= 0 )
	{
		found = true;
		LOGD( "\n=========================================\n" );
		LOGD( "=       Hub[0] FW Version is %02X%02X       =\n", Data[1], Data[0]);
		LOGD( "=========================================\n\n" );
		sprintf(strVersion, "%02X%02X", Data[1], Data[0]);
	}
	else
		LOGD( "liusb failed %s(%d)\n", libusb_error_name(ret), ret );

	if ( !found )
		LOGD( "Hub firmware not found.\n" );

	if ( gHubPDShareSPI )
	{
		if( ret )
		{
			found = GetPDFWVersion( devh, MSG );
		}
	}

	return found;

}

//-------------------------------------------------------------------------------------------------

uint32_t SendHubCommand( libusb_device_handle *devh, uint8_t MSG )
{
	uint32_t	Status = Fail_State;
	int		idx, ret;
	uint8_t	request_type;
	uint8_t	bRequest;
	uint16_t	wValue; 
	uint16_t	wIndex;
	uint16_t	wLength;
	uint8_t	Data[0x20] = {0};

	request_type = (uint8_t)( gCmd1 >> 24 );
	bRequest = (uint8_t)( gCmd1 >> 16 );
	wValue = (uint16_t)(( gCmd1 >> 8 ) & 0x00FF) | (uint16_t)( ( gCmd1 << 8 ) & 0xFF00 );
	wIndex = (uint16_t)(( gCmd2>> 24 ) & 0x00FF) | (uint16_t)( ( gCmd2 >> 8 ) & 0xFF00 );
	wLength = (uint16_t)(( gCmd2 >> 8 ) & 0x00FF) | (uint16_t)( ( gCmd2 << 8 ) & 0xFF00 );

	if ( wLength > 0x20 )
	{
		LOGD("Error, Length_Over_32\n");
		wLength = 0x20;
	}

	ret = libusb_control_transfer(	devh, 
								request_type, 
								bRequest, 
								wValue, 
								wIndex, 
								Data, 
								wLength, 
								1000	);
	if ( ret >= 0 )
	{
		Status = Success_State;
		if ( request_type & 0x80 )
		{
			for ( idx = 0; idx < wLength; idx++ )
			{
				LOGD( "Get_Value_%d = 0x%02X\n", idx, Data[idx] );
			}
		
		}
	}
	else
		LOGD( "liusb failed %s(%d)\n", libusb_error_name(ret), ret );


	return Status;

}


//-------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------

static int getInputParam( int argc, char *argv[] )
{
	int ret, ptn_ret;
	int prlvl;
	int iargc, argv_size, i, istr, Idx, iOnOff = 0, icmd1 = -1, icmd2 = -1, ibus = -1;
	char *serialized_argu;
	bool is_space_prior;
	char temp_char[64];

	memset(temp_char, 0, sizeof( temp_char ) );

	gDbgMsg = 0;
	gLogFile = 1;
	gUpdateMode = 0;
	DeviceNum = 1 ;
	gHubPDShareSPI = 0;
	gHubHubShareSPI = 0;
	Hub1Bin[MAX_PATH] = '\0';
	Hub1ShareBin[MAX_PATH] = '\0';
	PD1Bin[MAX_PATH] = '\0';
	gI2CTestOnly = 0;

//	/* default */
//	serialized_argu = NULL;
	ret = LIBUSB_SUCCESS;

	/* any argument? */
	if ( argc <= 1 )
	{
		return LIBUSB_ERROR_INVALID_PARAM;
	}

	/* */
	argv_size = 0;

	for ( iargc = 1; iargc < argc; iargc++ )
	{
		argv_size = argv_size + strlen( argv[iargc] ) + 1;
	}
	argv_size++;

	serialized_argu = ( char* )malloc( argv_size );
	if ( !serialized_argu )
	{
		return LIBUSB_ERROR_NO_MEM;
	}
	serialized_argu[0] = '\0';
    
	for ( iargc = 1; iargc < argc; iargc++ )
	{
		strcat( serialized_argu, argv[iargc] );
		strcat( serialized_argu, " " );
	}


	/* normalize arguemnts string. delete un-print char and */
	/* twice space char								*/
	is_space_prior = false;
	for ( i = 0, istr = 0; i < argv_size; i++ )
	{  

		if ( !isprint( serialized_argu[i] ) )	// Sherlock_20180425, Fix Length Bug
		{
			continue;
		}

		if ( isspace(serialized_argu[i] ) )
		{
			if ( is_space_prior )
			{
				continue;
			}
			is_space_prior = true;
			serialized_argu[istr++] = ' ';
		}
		else
		{
			is_space_prior = false;
//			serialized_argu[istr++] = ( char )tolower( serialized_argu[i] );
			serialized_argu[istr++] = ( char )( serialized_argu[i] ); // Sherlock_20180425
		}
	}
	serialized_argu[istr] = '\0';

	ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-vid", 16, &Hub1VID);
	ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-pid", 16, &Hub1PID);
	ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-hub1bin", Hub1Bin );
	ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-pd1bin", PD1Bin );
	ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-readveronly", 16, &gReadVerOnly);
	ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-readbinonly", 16, &gReadBinOnly);
	ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-backuponly", 16, &gBackUpOnly);

	ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-path", temp_char );
	if ( ptn_ret == 0 )
	{
		Idx = 0;
		for ( i = 0; i < 13; i++ )
		{
			if ( ( '1' <= temp_char[i] ) && ( temp_char[i] <= '9' ) )
			{
				gPath[Idx] = temp_char[i] - '0';
				Idx++;
				if (Idx>=7)	break;	// Protection
			}
			else if ( ( temp_char[i] == '.' ) || ( temp_char[i] == '_' ) )
				continue;
			else
				break;
		}
	}

	ptn_ret = str_get_ptn_uint( serialized_argu, (char*)"-cmd1", 16, &gCmd1 );
	if ( ptn_ret == LIBUSB_SUCCESS )
	{
		ptn_ret = str_get_ptn_uint( serialized_argu, (char*)"-cmd2", 16, &gCmd2 );
		if ( ptn_ret == LIBUSB_SUCCESS )
			gSendCmdOnly = 1;
	}

	if ( serialized_argu )
	{
		free( serialized_argu );
	}

	if ( gDbgMsg )
	{
		LOGD("gHubPDShareSPI = %d\n", gHubPDShareSPI );
		LOGD("gReadVerOnly = %d\n", gReadVerOnly );
		LOGD("gReadBinOnly = %d\n", gReadBinOnly );
		LOGD("gBackUpOnly = %d\n", gBackUpOnly );
		LOGD("gI2CTestOnly = %d\n", gI2CTestOnly );
	}
    
	return ret;

}

//-------------------------------------------------------------------------------------------------

static int SearchTargetDevice( DEVICE_DATA * DeviceData )//struct cmd_argu *cargu )
{
	int ret, i, j;
	libusb_device *dev;
	uint8_t path[8], bus; 
	uint8_t is_matched;

	/* */
//	cargu->list_dev.dev_idx = 0;
//	cargu->list_dev.dev = NULL;
	DeviceData->dev = NULL;
	DeviceData->dev_idx = 0;
	DeviceData->fw_ver = 0;

	if ( gbl_devs )
	{
		libusb_free_device_list(gbl_devs, 1);
		gbl_devs = NULL;
		gbl_devcnt = 0;
	}
    
	gbl_devcnt = libusb_get_device_list(NULL, &gbl_devs);
	if ( gbl_devcnt < 0 )
	{
		LOGD("libusb_get_device_list failed %zd\n", gbl_devcnt);
		return (int)LIBUSB_ERROR_NO_DEVICE;
	}

	LOGD("\n");
	LOGD("   device vid :pid  bcdDevice      (address)    bus    path\n");
	LOGD("----------------------------------------------------------------\n");
   
	for(i = 0; i < gbl_devcnt; i++)
	{
		struct libusb_device_descriptor desc;

		dev = gbl_devs[i];
		if (!dev)
		{
			LOGD("null dev (%d)\n", i);
			break;
		}

		ret = libusb_get_device_descriptor(dev, &desc);
		if (ret < 0)
		{
			LOGD("failed to get device descriptor %d\n", ret);
			break;
		}

		// billboard 层级
		memset(path, 0, sizeof( path ));
		ret = libusb_get_port_numbers( dev, path, sizeof( path ) );
		LOGD("port number=%d\n", ret);
		if(PortNums != -1) {
			is_matched = ( ( desc.idVendor == Hub1VID ) && ( desc.idProduct == Hub1PID ) && (ret == PortNums) );
		} else {
			is_matched = ( ( desc.idVendor == Hub1VID ) && ( desc.idProduct == Hub1PID ));
		}
		
//		is_matched = (	( cargu->list_dev.is_comp_id) && 
//						( desc.idVendor == cargu->list_dev.vid) && 
//						( desc.idProduct == cargu->list_dev.pid)		);

#if 1 //( Support_Multi_Hubs == 1 ) // Sherlock_20180510, Confirm_Port_Path
//		if ( ( is_matched ) && ( cargu->list_dev.port_path[0] != 0 ) ) // Need_To_Confirm_Port_Path
		if ( ( is_matched ) && ( gPath[0] != 0 ) ) // Need_To_Confirm_Port_Path
		{
			// Notice:	port_path[0] Is PortIndex from root Hub (Host)
			//			port_path[1] Is PortIndex from 1st Layer VLI Hub			
			//			port_path[2] Is PortIndex from 2nd Layer VLI Hub
			//			port_path[3] Is PortIndex from 3rd Layer VLI Hub
			//			...
			//			path[0] Is PortIndex from root Hub (Host)
			//			path[1] Is PortIndex from 1st Layer VLI Hub
			//			path[2] Is PortIndex from 2nd Layer VLI Hub
			//			path[3] Is PortIndex from 3rd Layer VLI Hub
			//			...
			
//			mem_zero( path, sizeof( path ) );
			memset(path, 0, sizeof( path ));
			ret = libusb_get_port_numbers( dev, path, sizeof( path ) );
			LOGD("          [Multi-Hub] get_path_number = %d\n", ret );

			if ( ret > 0 )
			{
				for ( j = 0; j < 7; j++ )
				{
					if ( gPath[j] == 0 )
						break;
					if ( gPath[j] != path[j] ) // Sherlock_20210615
					{
						is_matched = 0;
						LOGD( "          [Dev %d] target_path[%d]_%d != path[%d]_%d\n", 
													i, j, gPath[j], j, path[j] );
						break;
					}
				}
			}
			else
				is_matched = 0;
		}
#endif
#if 0 //( Support_Multi_Bus == 1 )
		 if ( ( is_matched ) && ( cargu->list_dev.target_BUS != -1 ) ) // Need_To_Confirm_BUS_Index
		 {
		 	bus = libusb_get_bus_number(dev);
			if ( bus != cargu->list_dev.target_BUS )
			{
				is_matched = 0;
				prltrace( prl_information, "		 [Dev %d] current_BUS_%d != target_BUS_%d\n",
												i, bus, cargu->list_dev.target_BUS );
			}
			else
			{
				prltrace( prl_information, "		 [Dev %d] current_BUS_%d == target_BUS_%d\n",
												i, bus, cargu->list_dev.target_BUS );
			}
			
		 }
#endif
		if ( is_matched )
		{
			DeviceData->dev = dev;
			DeviceData->dev_idx = i;
			DeviceData->fw_ver = desc.bcdDevice;
		LOGD("------ matched ------");
//			cargu->list_dev.dev_idx = i;
//			cargu->list_dev.dev = dev;
//			cargu->list_dev.fw_ver = desc.bcdDevice; // Sherlock_20180612
		}
        
		LOGD(	"(%c)%06d %04x:%04x bcdDevice %04x (address %02d) bus %d,",
					is_matched ? '*' : ' ', 
					i, desc.idVendor, desc.idProduct, desc.bcdDevice,
					libusb_get_device_address(dev),
					libusb_get_bus_number(dev));

		ret = libusb_get_port_numbers(dev, path, sizeof(path));
		if (ret > 0)
		{
			LOGD(" path %d", path[0]);
            
			for (j = 1; j < ret; j++)
			{
				LOGD(".%d", path[j]);
			}
		}
            
		LOGD("\n");
	}

//	if (cargu->list_dev.is_comp_id && (!cargu->list_dev.dev))
	if ( !DeviceData->dev )
	{
		ret = LIBUSB_ERROR_NOT_FOUND;
		LOGD("   Target_Device_Not_Found.\n");
	}
	else
		ret = LIBUSB_SUCCESS;

	LOGD("----------------------------------------------------------------\n");

	return ret;
}

//-------------------------------------------------------------------------------------------------
int getVersion(int pid ,int vid, int portNums, char* strVersion) {
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-vid", 16, &Hub1VID);
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-pid", 16, &Hub1PID);
	// ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-hub1bin", Hub1Bin );
	// ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-pd1bin", PD1Bin );
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-readveronly", 16, &gReadVerOnly);
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-readbinonly", 16, &gReadBinOnly);
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-backuponly", 16, &gBackUpOnly);
	// ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-path", temp_char );
	Hub1VID = pid;
	Hub1PID = vid;
	PortNums = portNums;
	int ret;
	DEVICE_DATA DeviceData;
	libusb_device_handle *devh;
	uint32_t Status = Fail_State;
	ret = libusb_init( NULL );
	if ( ret < 0 )
	{
		LOGD( "libusb_init failed %d\n", ret );
		goto Finish;
	}
	ret = SearchTargetDevice( &DeviceData );
	if ( ret != LIBUSB_SUCCESS )
	{
		LOGD( "target device not found %d\n", ret );
		goto Finish;
	}

	// Open_Device_Handle
	ret = libusb_open( DeviceData.dev, &devh );
	if (ret < 0)
	{
		LOGD( "liusb_open failed %s(%d)\n", libusb_error_name(ret), ret );
		goto Cleanup;
	}
	Status = (uint32_t)GetFWVersion( devh, 0, strVersion);
	if ( devh )
	{
		libusb_close( devh );
	}
	goto Cleanup;
	
Cleanup:
	
	libusb_exit( NULL );
	
Finish:
	
	return ret;

}

int updateFirmware(int pid ,int vid, int portNums, char* path) {
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-vid", 16, &Hub1VID);
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-pid", 16, &Hub1PID);
	// ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-hub1bin", Hub1Bin );
	// ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-pd1bin", PD1Bin );
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-readveronly", 16, &gReadVerOnly);
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-readbinonly", 16, &gReadBinOnly);
	// ptn_ret = str_get_ptn_int( serialized_argu, (char*)"-backuponly", 16, &gBackUpOnly);
	// ptn_ret = str_get_ptn_str( serialized_argu, (char*)"-path", temp_char );
	LOGD("path=%s", path);
	Hub1VID = pid;
	Hub1PID = vid;
	PortNums = portNums;
	if (snprintf(Hub1Bin, MAX_PATH, "%s", path) >= MAX_PATH) {
    	// 处理截断错误
		return Fail_State;
	}

	int ret;
	DEVICE_DATA DeviceData;
	libusb_device_handle *devh;
	uint32_t Status = Fail_State;
	ret = libusb_init( NULL );
	if ( ret < 0 )
	{
		LOGE( "libusb_init failed %d\n", ret );
		goto Finish;
	}
	ret = SearchTargetDevice( &DeviceData );
	if ( ret != LIBUSB_SUCCESS )
	{
		LOGE( "target device not found %d\n", ret );
		goto Finish;
	}

	// Open_Device_Handle
	ret = libusb_open( DeviceData.dev, &devh );
	if (ret < 0)
	{
		LOGE( "liusb_open failed %s(%d)\n", libusb_error_name(ret), ret );
		goto Cleanup;
	}
	ret = Hub_FW_Update_Exe( devh, 0 );
	if ( devh )
	{
		libusb_close( devh );
	}
	goto Cleanup;
	
Cleanup:
	
	libusb_exit( NULL );
	
Finish:
	
	return ret;

}


// int main( int argc, char *argv[] )
// {
// 	DEVICE_DATA DeviceData;
// 	libusb_device_handle *devh;
// 	unsigned char data[10];
// 	uint32_t Status = Fail_State;

// 	int ret, idx;
// 	LOGD("========================================\n");
// 	LOGD("=   VLI Hub Linux Console Tool v 0.0   =\n");
// 	LOGD("=                                      =\n");
// 	LOGD("=              Sherlock Chu            =\n");
// 	LOGD("=               2021.08.26             =\n");
// 	LOGD("========================================\n");

// 	getInputParam( argc, argv );

// 	ret = libusb_init( NULL );
// 	if ( ret < 0 )
// 	{
// 		LOGD( "libusb_init failed %d\n", ret );
// 		goto Finish;
// 	}
// 	ret = SearchTargetDevice( &DeviceData );
// 	if ( ret != LIBUSB_SUCCESS )
// 	{
// 		LOGD( "target device not found %d\n", ret );
// 		goto Finish;
// 	}

// 	// Open_Device_Handle
// 	ret = libusb_open( DeviceData.dev, &devh );
// 	if (ret < 0)
// 	{
// 		LOGD( "liusb_open failed %s(%d)\n", libusb_error_name(ret), ret );
// 		goto Cleanup;
// 	}

// 	if ( gSendCmdOnly )
// 		Status = SendHubCommand( devh, 0 );
// 	else if ( gReadVerOnly )
// 		Status = (uint32_t)GetFWVersion( devh, 0 );
// 	else if ( gBackUpOnly )
// 		Status = Hub_FW_Backup_Exe( devh, 0 );
// 	else
// 		Status = Hub_FW_Update_Exe( devh, 0 );

// 	if ( devh )
// 	{
// 		libusb_close( devh );
// 	}

// 	goto Cleanup;
	
// Cleanup:
	
// 	libusb_exit( NULL );
	
// Finish:
	
// 	return ret;
// }

