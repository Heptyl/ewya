/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __MISCVBORX_H__
#define __MISCVBORX_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef enum _vborx_hsmode {
	VBORX_HSMODE_EXTERNAL_INPUT,			// external input
	VBORX_HSMODE_INTERNAL_GENERATION,		// internal generation
	VBORX_HSMODE_MAX
} vborx_hsmode;
//------------------------------------------------------------------------------
typedef struct _vborx_hsync {
	system_port		port;
	vborx_hsmode	hsmode;
	uint16_t		htotal_length;			// H total length, when hsmode=1
	uint16_t		hsync_width;			// H sync width, when hsmode=1
} vborx_hsync;
//------------------------------------------------------------------------------
uint32_t vborx_set_hsync(vborx_hsync *p_hsync);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __MISCVBORX_H__
