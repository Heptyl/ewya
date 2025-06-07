/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __MISCVBOTX_H__
#define __MISCVBOTX_H__

#include <stdint.h>
#include "system_common.h"
#include "vbotx.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef struct _vbotx_sg_video {
	system_port			port;
	system_video_format	video_format;
	system_deep_color	deep_color;
} vbotx_sg_video;
//------------------------------------------------------------------------------
uint32_t vbotx_set_sg_format(vbotx_sg_video *p_video);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __MISCVBOTX_H__
