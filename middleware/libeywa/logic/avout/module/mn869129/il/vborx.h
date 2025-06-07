/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __VBORX_H__
#define __VBORX_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef struct _vborx_config {
	system_config_retry		cdr_lock_detect;
} vborx_config;
//------------------------------------------------------------------------------
typedef system_video vborx_video;
//------------------------------------------------------------------------------
uint32_t vborx_configure(vborx_config *p_conf);
uint32_t vborx_init(system_config *p_conf);
uint32_t vborx_exit(system_port port);
uint32_t vborx_set_video_format(vborx_video *p_video);
uint32_t vborx_abort(void);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __VBORX_H__
