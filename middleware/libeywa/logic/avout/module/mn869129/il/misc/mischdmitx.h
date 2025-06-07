/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __MISCHDMITX_H__
#define __MISCHDMITX_H__

#include <stdint.h>
#include "system_common.h"
#include "hdmitx.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef struct _hdmitx_sg_video {
	system_port			port;
	system_video_format	video_format;
	system_color_format	color_format;
	system_deep_color	deep_color;
	system_sg_format	sg_format;
	hdmitx_dvi_mode		dvi_mode;
} hdmitx_sg_video;
//------------------------------------------------------------------------------
typedef struct _hdmitx_avi_infoframe {
	system_port port;
	union {
		uint8_t databyte1;
		struct {
			uint8_t s0:1;
			uint8_t s1:1;
			uint8_t b0:1;
			uint8_t b1:1;
			uint8_t a0:1;
			uint8_t y0:1;
			uint8_t y1:1;
			uint8_t y2:1;
		};
	};
	union {
		uint8_t databyte2;
		struct {
			uint8_t r0:1;
			uint8_t r1:1;
			uint8_t r2:1;
			uint8_t r3:1;
			uint8_t m0:1;
			uint8_t m1:1;
			uint8_t c0:1;
			uint8_t c1:1;
		};
	};
	union {
		uint8_t databyte3;
		struct {
			uint8_t sc0:1;
			uint8_t sc1:1;
			uint8_t q0:1;
			uint8_t q1:1;
			uint8_t ec0:1;
			uint8_t ec1:1;
			uint8_t ec2:1;
			uint8_t itc:1;
		};
	};
	union {
		uint8_t databyte4;
		uint8_t vic;
	};
	union {
		uint8_t databyte5;
		struct {
			uint8_t pr0:1;
			uint8_t pr1:1;
			uint8_t pr2:1;
			uint8_t pr3:1;
			uint8_t cn0:1;
			uint8_t cn1:1;
			uint8_t yq0:1;
			uint8_t yq1:1;
		};
	};
} hdmitx_avi_infoframe;
//------------------------------------------------------------------------------
uint32_t hdmitx_set_sg_format(hdmitx_sg_video *p_video);
uint32_t hdmitx_set_avi_infoframe_misc(hdmitx_avi_infoframe *p_avi);
uint32_t hdmitx_get_avi_infoframe(hdmitx_avi_infoframe *p_avi);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __MISCHDMITX_H__
