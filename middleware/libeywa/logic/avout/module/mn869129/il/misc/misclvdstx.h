/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __MISCLVDSTX_H__
#define __MISCLVDSTX_H__

#include <stdint.h>
#include "system_common.h"
#include "lvdstx.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef struct _lvdstx_sg_video {
	system_port			port;
	system_video_format	video_format;
	system_deep_color	deep_color;
} lvdstx_sg_video;
//------------------------------------------------------------------------------
uint32_t lvdstx_set_sg_format(lvdstx_sg_video *p_video);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __MISCLVDSTX_H__
