/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __VBOTX_H__
#define __VBOTX_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef struct _vbotx_config {
	system_config_retry		hot_plug_detect;
	system_config_retry		pll_lock_detect;
} vbotx_config;
//------------------------------------------------------------------------------
typedef system_video vbotx_video;
typedef system_mute vbotx_mute;
//------------------------------------------------------------------------------
uint32_t vbotx_configure(vbotx_config *p_conf);
uint32_t vbotx_init(system_config *p_conf);
uint32_t vbotx_exit(system_port port);
uint32_t vbotx_set_video_format(vbotx_video *p_video);
uint32_t vbotx_video_mute(vbotx_mute *p_mute);
uint32_t vbotx_abort(void);
uint32_t vbotx_en(system_port port, int onoff);
void vbotx_Parm(uint8_t* pVb1data);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __VBOTX_H__
