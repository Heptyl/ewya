/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HDMIRX_H__
#define __HDMIRX_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef enum _hdmirx_port_status_connection {
	HDMIRX_PORT_STATUS_CONNECTION_INACTIVE,
	HDMIRX_PORT_STATUS_CONNECTION_ACTIVE
} hdmirx_port_status_connection;
//------------------------------------------------------------------------------
typedef enum _hdmirx_av_status_format {
	HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED,
	HDMIRX_AV_STATUS_FORMAT_CHANGED
} hdmirx_av_status_format;
//------------------------------------------------------------------------------
typedef enum _hdmirx_av_status_mute {
	HDMIRX_AV_STATUS_MUTE_ON,
	HDMIRX_AV_STATUS_MUTE_OFF
} hdmirx_av_status_mute;
//------------------------------------------------------------------------------
typedef struct _hdmirx_port_status {
	system_port						port;			// port number
	hdmirx_port_status_connection	port0;
	hdmirx_port_status_connection	port1;
	hdmirx_port_status_connection	port2;
	hdmirx_port_status_connection	port3;
} hdmirx_port_status;
//------------------------------------------------------------------------------
typedef struct _hdmirx_av_status {
	system_port						port;			// port number
	hdmirx_av_status_format			video_format;
	hdmirx_av_status_mute			video_mute;
	hdmirx_av_status_format			audio_format;
	hdmirx_av_status_mute			audio_mute;
	hdmirx_av_status_format			spd_format;
	hdmirx_av_status_mute			spd_mute;
	hdmirx_av_status_format			hdr_format;
	hdmirx_av_status_mute			hdr_mute;
} hdmirx_av_status;
//------------------------------------------------------------------------------
typedef struct _hdmirx_pixel_clock_freq {
	system_port						port;			// port number
	uint32_t						pixel_clock_freq;
} hdmirx_pixel_clock_freq;
//------------------------------------------------------------------------------
typedef system_video hdmirx_video;
typedef system_audio hdmirx_audio;
typedef system_spd_infoframe hdmirx_spd_infoframe;
typedef system_hdr_infoframe hdmirx_hdr_infoframe;
//------------------------------------------------------------------------------
uint32_t hdmirx_init(system_port port);
uint32_t hdmirx_exit(system_port port);
uint32_t hdmirx_get_port_status(hdmirx_port_status *p_status);
uint32_t hdmirx_get_av_status(hdmirx_av_status *p_status);
uint32_t hdmirx_select_port(system_port port);
uint32_t hdmirx_get_video_format(hdmirx_video *p_video);
uint32_t hdmirx_get_audio_format(hdmirx_audio *p_audio);
uint32_t hdmirx_get_spd_infoframe(hdmirx_spd_infoframe *p_spd);
uint32_t hdmirx_get_hdr_infoframe(hdmirx_hdr_infoframe *p_hdr);
uint32_t hdmirx_get_pixel_clock_frequency(hdmirx_pixel_clock_freq *p_pcf);
uint32_t hdmirx_abort(void);
//------------------------------------------------------------------------------
#pragma pack()

#endif  // __HDMIRX_H__

