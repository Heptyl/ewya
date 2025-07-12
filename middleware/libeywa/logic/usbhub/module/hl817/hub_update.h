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


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <libusb/libusb.h>


#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR		    (0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE	    0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER		    0x03

#define USB_ENDPOINT_IN			0x80
#define USB_ENDPOINT_OUT		0x00

#define Fail_State				0x00
#define Success_State			0x01

#define	VID_VLI					0x2109
#define	PID_102					0x0102
#define	PID_103					0x0103
#define	PID_104					0x0104
#define	PID_105					0x0105
#define	PID_106					0x0106
#define	PID_107					0x0107

#define	SIZE_102				32*1024
#define	SIZE_103				32*1024
#define	SIZE_104				48*1024
#define	SIZE_105				48*1024
#define	SIZE_106				32*1024
#define	SIZE_107				50*1024

#define SIZE_64KB				(64*1024)	// = 0x 01 00 00
#define SIZE_128KB				(128*1024)	// = 0x 02 00 00
#define SIZE_256KB				(256*1024)	// = 0x 04 00 00
#define poly 				    0xA001 		//0x100B //0x1021 //0x8005

#define MAX_PATH				256

#define Sleep(msec)				usleep(msec * 1000)

typedef struct _DEVICE_DATA {
	uint8_t dev_idx;
	libusb_device *dev;
	uint16_t fw_ver;
} DEVICE_DATA, *PDEVICE_DATA;

//	LIBUSB_SUCCESS = 0
//	LIBUSB_ERROR_IO = -1
//	LIBUSB_ERROR_INVALID_PARAM = -2
//	LIBUSB_ERROR_ACCESS = -3
//	LIBUSB_ERROR_NO_DEVICE = -4
//	LIBUSB_ERROR_NOT_FOUND = -5
//	LIBUSB_ERROR_BUSY = -6
//	LIBUSB_ERROR_TIMEOUT = -7
//	LIBUSB_ERROR_OVERFLOW = -8
//	LIBUSB_ERROR_PIPE = -9
//	LIBUSB_ERROR_INTERRUPTED = -10
//	LIBUSB_ERROR_NO_MEM = -11
//	LIBUSB_ERROR_NOT_SUPPORTED = -12
//	LIBUSB_ERROR_OTHER = -99

int getVersion(int vid, int pid, int portnum, char* strVersion1);
int updateFirmware(int pid ,int vid, int portnum, char* path);
