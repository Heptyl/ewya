/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __SYSTEM_CTRL_H__
#define __SYSTEM_CTRL_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
#define SYSTEM_CTRL_TX_UPDATE_REG_NONE			0x00
#define SYSTEM_CTRL_TX_UPDATE_REG_ALLUPD		0x80
#define SYSTEM_CTRL_TX_UPDATE_REG_AVIIUPD		0x40
#define SYSTEM_CTRL_TX_UPDATE_REG_AUDIUPD		0x20
#define SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD		0x10
#define SYSTEM_CTRL_TX_UPDATE_REG_HVSIUPD		0x08
#define SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD		0x04
#define SYSTEM_CTRL_TX_UPDATE_REG_AUDUPD		0x02
#define SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD		0x01
#define SYSTEM_CTRL_TX_UPDATE_REG_ALL			SYSTEM_CTRL_TX_UPDATE_REG_ALLUPD  | \
												SYSTEM_CTRL_TX_UPDATE_REG_AVIIUPD | \
												SYSTEM_CTRL_TX_UPDATE_REG_AUDIUPD | \
												SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD  | \
												SYSTEM_CTRL_TX_UPDATE_REG_HVSIUPD | \
												SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD  | \
												SYSTEM_CTRL_TX_UPDATE_REG_AUDUPD  | \
												SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD
//------------------------------------------------------------------------------
#define SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST		0x80
#define SYSTEM_CTRL_TX_SOFT_RESET_AUDSRST		0x40
#define SYSTEM_CTRL_TX_SOFT_RESET_UPCSRST		0x20
#define SYSTEM_CTRL_TX_SOFT_RESET_HDCPRST		0x10
#define SYSTEM_CTRL_TX_SOFT_RESET_CCSRST		0x08
#define SYSTEM_CTRL_TX_SOFT_RESET_FIFORST		0x04
#define SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST		0x02
#define SYSTEM_CTRL_TX_SOFT_RESET_AVCSRST		0x01
//------------------------------------------------------------------------------
typedef enum _system_ctrl_audio {
	SYSTEM_CTRL_AUDIO_NONE,
	SYSTEM_CTRL_AUDIO_HDMI_RX,
	SYSTEM_CTRL_AUDIO_HDMI_TX,
	SYSTEM_CTRL_AUDIO_SPDIF_RX,
	SYSTEM_CTRL_AUDIO_SPDIF_TX,
	SYSTEM_CTRL_AUDIO_I2S_RX,
	SYSTEM_CTRL_AUDIO_I2S_TX,
	SYSTEM_CTRL_AUDIO_MAX
} system_ctrl_audio;
//------------------------------------------------------------------------------
typedef enum _system_ctrl_vbo_lane {
	SYSTEM_CTRL_VBO_LANE_0,
	SYSTEM_CTRL_VBO_LANE_1,
	SYSTEM_CTRL_VBO_LANE_2,
	SYSTEM_CTRL_VBO_LANE_4,
	SYSTEM_CTRL_VBO_LANE_8,
	SYSTEM_CTRL_VBO_LANE_MAX
} system_ctrl_vbo_lane;
//------------------------------------------------------------------------------
typedef enum _system_ctrl_lvds_link {
	SYSTEM_CTRL_LVDS_LINK_0,
	SYSTEM_CTRL_LVDS_LINK_1,
	SYSTEM_CTRL_LVDS_LINK_2,
	SYSTEM_CTRL_LVDS_LINK_4,
	SYSTEM_CTRL_LVDS_LINK_MAX
} system_ctrl_lvds_link;
//------------------------------------------------------------------------------
typedef enum _system_ctrl_polarity {
	SYSTEM_CTRL_POL_P = 0,			// Positive
	SYSTEM_CTRL_POL_N = 1,			// Negative
	SYSTEM_CTRL_POL_MAX
} system_ctrl_polarity;
//------------------------------------------------------------------------------
#define SYSTEM_CTRL_VBO_FREQ_297000KHZ_P2L		(297000/2)		// KHz per lane
#define SYSTEM_CTRL_LVDS_FREQ_148500KHZ			148500
//------------------------------------------------------------------------------
uint32_t system_ctrl_init(system_port port);
uint32_t system_ctrl_exit(system_port port);
uint32_t system_ctrl_check_vborx_init(system_port port);
uint32_t system_ctrl_check_vbotx_init(system_port port);
uint32_t system_ctrl_check_hdmirx_init(system_port port);
uint32_t system_ctrl_check_hdmitx_init(system_port port);
uint32_t system_ctrl_check_lvdstx_init(system_port port);
uint32_t system_ctrl_set_color_format(system_video *p_video);
uint32_t system_ctrl_set_color_format_HDMI(system_video *p_video);
uint32_t system_ctrl_tx_init(system_port port);
uint32_t system_ctrl_tx_video_mute(system_mute *p_mute);
uint32_t system_ctrl_tx_video_avmute(system_mute *p_mute);
uint32_t system_ctrl_tx_video_black(system_mute *p_mute);
uint32_t system_ctrl_tx_update_reg(system_port port, uint8_t reg);
uint32_t system_ctrl_tx_soft_reset(system_port port, uint8_t rst);
uint32_t system_ctrl_tx_set_video_format(system_video *p_video);
uint32_t system_ctrl_get_vbo_lane_count(system_video_format vfmt, system_ctrl_vbo_lane *p_cnt);
uint32_t system_ctrl_get_lvds_link_count(system_video_format vfmt, system_ctrl_lvds_link *p_cnt);
uint32_t system_ctrl_get_tx_glue_hstart(system_port port, system_video_format vfmt, uint32_t freq,
										uint8_t *p_hstart70, uint8_t *p_hstart98);
uint32_t system_ctrl_get_rx_glue_hstart(system_video_format vfmt, uint8_t *p_hstart70, uint8_t *p_hstart98);
uint32_t system_ctrl_get_pixel_freq(system_video_format vfmt, uint32_t *p_pixel_freq);
uint32_t system_ctrl_get_sync_polarity(system_video_format vfmt, system_ctrl_polarity *p_hpol,
										system_ctrl_polarity *p_vpol);
uint32_t system_ctrl_get_hdmirx_sync_polarity(system_port port, system_ctrl_polarity *p_hpol,
											system_ctrl_polarity *p_vpol);
uint32_t system_ctrl_audio_init(system_port port, system_ctrl_audio type);
uint32_t system_ctrl_audio_exit(system_port port, system_ctrl_audio type);
uint32_t system_ctrl_external_audio_mute(system_mute *p_mute);
uint32_t system_ctrl_audio_check_param(system_audio *p_audio);
uint32_t system_ctrl_audio_if2sf(uint8_t aif, system_audio_sf *p_sf);
uint32_t system_ctrl_audio_sf2if(system_audio_sf sf, uint8_t *p_aif);
uint32_t system_ctrl_audio_if2ss(uint8_t aif, system_audio_ss *p_ss);
uint32_t system_ctrl_audio_ss2if(system_audio_ss ss, uint8_t *p_aif);
uint32_t system_ctrl_audio_cbit2sf(uint8_t cbit, system_audio_sf *p_sf);
uint32_t system_ctrl_audio_sf2cbit(system_audio_sf sf, uint8_t *p_cbit);
uint32_t system_ctrl_audio_cbit2ss(uint8_t cbit, system_audio_ss *p_ss);
uint32_t system_ctrl_audio_ss2cbit(system_audio_ss ss, uint8_t *p_cbit);
uint32_t system_ctrl_audio_get_output_sf(system_audio_sf sf, system_audio_sf_ratio sf_ratio,
											uint8_t *p_output);
uint8_t system_ctrl_is_audio_hbr(system_audio_sf sf);
uint8_t is_video_format_1080p_60hz(system_video_format vfmt);
uint8_t is_video_format_1080p_50hz(system_video_format vfmt);
uint8_t is_video_format_1080p_30hz(system_video_format vfmt);
uint8_t is_video_format_1080p_25hz(system_video_format vfmt);
uint8_t is_video_format_1080p_24hz(system_video_format vfmt);
uint8_t is_video_format_1080p(system_video_format vfmt);
uint8_t is_video_format_4k2k_60hz(system_video_format vfmt);
uint8_t is_video_format_4k2k_50hz(system_video_format vfmt);
uint8_t is_video_format_4k2k_30hz(system_video_format vfmt);
uint8_t is_video_format_4k2k_25hz(system_video_format vfmt);
uint8_t is_video_format_4k2k_24hz(system_video_format vfmt);
uint8_t is_video_format_4k2k(system_video_format vfmt);
uint8_t is_video_format_pc(system_video_format vfmt);
uint8_t is_color_format_yuv420(system_color_format cfmt);
uint8_t is_color_format_yuv422(system_color_format cfmt);
uint8_t is_color_format_yuv444(system_color_format cfmt);
uint8_t is_color_format_rgb(system_color_format cfmt);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __SYSTEM_CTRL_H__
