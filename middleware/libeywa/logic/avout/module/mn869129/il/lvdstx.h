/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __LVDSTX_H__
#define __LVDSTX_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef enum _lvdstx_data_format {
	LVDSTX_DATA_FORMAT_VESA_8BIT,				// data mapping for VESA 8bit
	LVDSTX_DATA_FORMAT_VESA_10BIT,				// data mapping for VESA 10bit
	LVDSTX_DATA_FORMAT_JEIDA_8BIT,				// data mapping for JEIDA 8bit
	LVDSTX_DATA_FORMAT_JEIDA_10BIT,				// data mapping for JEIDA 10bit
	LVDSTX_DATA_FORMAT_VESA_JEIDA_COMPOSITE,	// data mapping for VESA/JEIDA composite
	LVDSTX_DATA_FORMAT_MAX
} lvdstx_data_format;
//------------------------------------------------------------------------------
typedef struct _lvdstx_config {
	system_port			port;
	lvdstx_data_format	format;
} lvdstx_config;
//------------------------------------------------------------------------------
typedef struct _lvdstx_skew_ch {
	uint8_t lane_a;
	uint8_t lane_b;
	uint8_t lane_c;
	uint8_t lane_d;
	uint8_t lane_e;
	uint8_t clk;
} lvdstx_skew_ch;
//------------------------------------------------------------------------------
typedef struct _lvdstx_skew_param {
	system_port port;
	lvdstx_skew_ch ch0;
	lvdstx_skew_ch ch1;
	lvdstx_skew_ch ch2;
	lvdstx_skew_ch ch3;
} lvdstx_skew_param;
//------------------------------------------------------------------------------
typedef system_video lvdstx_video;
typedef system_mute lvdstx_mute;
//------------------------------------------------------------------------------
uint32_t lvdstx_init(lvdstx_config *p_conf);
uint32_t lvdstx_exit(system_port port);
uint32_t lvdstx_set_video_format(lvdstx_video *p_video);
uint32_t lvdstx_video_mute(lvdstx_mute *p_mute);
uint32_t lvdstx_set_skew_param(lvdstx_skew_param *p_skew);
uint32_t lvdstx_get_skew_param(lvdstx_skew_param *p_skew);
uint32_t lvdstx_abort(void);
//------------------------------------------------------------------------------
#pragma pack()

#endif  // __LVDSTX_H__
