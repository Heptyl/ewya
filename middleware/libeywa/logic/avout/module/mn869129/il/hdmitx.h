/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HDMITX_H__
#define __HDMITX_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef enum _hdmitx_dvi_mode {
	HDMITX_DVI_MODE_DISABLE,
	HDMITX_DVI_MODE_ENABLE,
	HDMITX_DVI_MODE_MAX
} hdmitx_dvi_mode;


typedef enum _hdmitx_update_mode {
	HDMITX_UPDATE_MODE_DISABLE,
	HDMITX_UPDATE_MODE_ENABLE,
	HDMITX_UPDATE_MODE_MAX
} hdmitx_update_mode;


//------------------------------------------------------------------------------
typedef struct _hdmitx_video {
	system_port			rx_port;			// Rx port number
	system_port			tx_port;			// Tx port number
	system_video_format	rx_video_format;	// Rx video format
	system_color_format	rx_color_format;	// Rx color format
	system_deep_color	rx_deep_color;		// Rx deep color
	system_video_format	tx_video_format;	// Tx video format
	system_color_format	tx_color_format;	// Tx color format
	system_deep_color	tx_deep_color;		// Tx deep color
	hdmitx_dvi_mode		tx_dvi_mode;		// whether to enable DVI output mode for HDMI Tx
} hdmitx_video;
//------------------------------------------------------------------------------
typedef struct _hdmitx_edid_vcdb {
	system_port			port;
	uint8_t				byte3;				// Byte#3 of Video Capability Data Block
} hdmitx_edid_vcdb;
//------------------------------------------------------------------------------
typedef system_audio hdmitx_audio;
typedef system_mute hdmitx_mute;
typedef system_spd_infoframe hdmitx_spd_infoframe;
typedef system_hdr_infoframe hdmitx_hdr_infoframe;
//------------------------------------------------------------------------------
uint32_t hdmitx_init(system_port port);
uint32_t hdmitx_exit(system_port port);
uint32_t hdmitx_set_video_format(hdmitx_video *p_video);
uint32_t hdmitx_set_audio_format(hdmitx_audio *p_audio);
uint32_t hdmitx_video_mute(hdmitx_mute *p_mute);
uint32_t hdmitx_audio_mute(hdmitx_mute *p_mute);
uint32_t hdmitx_set_spd_infoframe(hdmitx_spd_infoframe *p_spd);
uint32_t hdmitx_set_hdr_infoframe(hdmitx_hdr_infoframe *p_hdr);
uint32_t hdmitx_get_hpd_status(system_port port, uint8_t *p_status);
uint32_t hdmitx_set_hpd_intr(system_port port, uint8_t intr);
uint32_t hdmitx_get_hpd_intr(system_port port, uint8_t *p_intr);
uint32_t hdmitx_set_sink_edid_vcdb(hdmitx_edid_vcdb *p_vcdb);
uint32_t hdmitx_abort(void);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __HDMITX_H__
