/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __DDC_H__
#define __DDC_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
#define DDC_MAX_BUF_SIZE				256
//------------------------------------------------------------------------------
#define HDMITX_DDC_MAX_BUF_SIZE			DDC_MAX_BUF_SIZE
//------------------------------------------------------------------------------
typedef struct _hdmitx_ddc {
	system_port	port;							// port number
	uint8_t		saddr;							// slave address
	uint8_t		oaddr;							// offset address
	uint16_t	length;							// data length (read/write)
	uint8_t		data[HDMITX_DDC_MAX_BUF_SIZE];	// DDC buffer data (read/write)
} hdmitx_ddc;
//------------------------------------------------------------------------------
#define HDMIRX_EDID_MAX_BUF_SIZE		DDC_MAX_BUF_SIZE
//------------------------------------------------------------------------------
typedef struct _hdmirx_edid {
	system_port port;							// port number
	uint8_t		*p_edid0;						// pointer to EDID data of port 0
	uint8_t		*p_edid1;						// pointer to EDID data of port 1
	uint8_t		*p_edid2;						// pointer to EDID data of port 2
	uint8_t		*p_edid3;						// pointer to EDID data of port 3
} hdmirx_edid;
//------------------------------------------------------------------------------
typedef hdmitx_ddc hdmitx_scdc;
typedef hdmitx_ddc hdmitx_edid;
//------------------------------------------------------------------------------
uint32_t hdmitx_write_ddc(hdmitx_ddc *p_ddc);
uint32_t hdmitx_read_ddc(hdmitx_ddc *p_ddc);
uint32_t hdmitx_write_scdc(hdmitx_scdc *p_scdc);
uint32_t hdmitx_read_scdc(hdmitx_scdc *p_scdc);
uint32_t hdmitx_read_edid(hdmitx_edid *p_edid);
uint32_t hdmirx_write_edid(hdmirx_edid *p_edid);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __DDC_H__
