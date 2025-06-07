/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __OPTS_H__
#define __OPTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "hv_print.h"

#pragma pack(1)
//
// main command & option
//
typedef enum _cmd_main_id {
	CMD_MAIN_ID_NONE,
	CMD_MAIN_ID_UNKNOWN,
	CMD_MAIN_ID_REG,
	CMD_MAIN_ID_PRESET,
	CMD_MAIN_ID_CONFIG,
	CMD_MAIN_ID_INIT,
	CMD_MAIN_ID_READ,
	CMD_MAIN_ID_WRITE,
	CMD_MAIN_ID_SET,
	CMD_MAIN_ID_GET,
	CMD_MAIN_ID_RESET,
	CMD_MAIN_ID_SELECT,
	CMD_MAIN_ID_MUTE,
	CMD_MAIN_ID_UNMUTE,
	CMD_MAIN_ID_ABORT,
	CMD_MAIN_ID_SLEEP,
	CMD_MAIN_ID_TIME,
	CMD_MAIN_ID_AUTO,
	CMD_MAIN_ID_MISC,
	CMD_MAIN_ID_EXIT,
	CMD_MAIN_ID_HELP,
	CMD_MAIN_ID_QUIT,
	CMD_MAIN_ID_OPTION,
	CMD_MAIN_ID_FIG,
	CMD_MAIN_ID_SUB_I2C,
	CMD_MAIN_ID_MAX
} cmd_main_id;

typedef enum _opt_target {
	OPT_TARGET_NONE,
	OPT_TARGET_VIDEO,
	OPT_TARGET_AUDIO,
	OPT_TARGET_EDID,
	OPT_TARGET_CHIP,
	OPT_TARGET_BUS,
	OPT_TARGET_SCDC,
	OPT_TARGET_HDCP,
	OPT_TARGET_SPD_IF,
	OPT_TARGET_HDR_IF,
	OPT_TARGET_STATUS,
	OPT_TARGET_FORMAT,
	OPT_TARGET_RETRY,
	OPT_TARGET_CLEAR,
	OPT_TARGET_SKEW,
	OPT_TARGET_VCDB,
	OPT_TARGET_MAX
} opt_target;

typedef enum _opt_target2 {
	OPT_TARGET2_NONE,
	OPT_TARGET2_SRAM,
	OPT_TARGET2_VIA,
	OPT_TARGET2_CDR_LOCK,
	OPT_TARGET2_HOT_PLUG,
	OPT_TARGET2_PLL_LOCK,
	OPT_TARGET2_INIT_AUTH,
	OPT_TARGET2_SG,
	OPT_TARGET2_HDCP,
	OPT_TARGET2_SINK,
	OPT_TARGET2_MAX
} opt_target2;

typedef enum _opt_port {
	OPT_PORT_NONE,
	OPT_PORT_HDMI,
	OPT_PORT_VBO,
	OPT_PORT_LVDS,
	OPT_PORT_SPDIF,
	OPT_PORT_I2S,
	OPT_PORT_HDCP,
	OPT_PORT_MAX
} opt_port;

typedef enum _opt_stat {
	OPT_STAT_HPD,
	OPT_STAT_HDCP,
	OPT_STAT_PORT,
	OPT_STAT_AV,
	OPT_STAT_MAX
} opt_stat;

typedef enum _opt_txrx {
	OPT_TXRX_NONE,
	OPT_TXRX_TX,
	OPT_TXRX_RX,
	OPT_TXRX_MAX
} opt_txrx;

typedef enum _opt_init_opt {
	OPT_INIT_OPT_NONE,
	OPT_INIT_OPT_INVERSION_ON,
	OPT_INIT_OPT_INVERSION_OFF,
	OPT_INIT_OPT_VESA08,
	OPT_INIT_OPT_VESA10,
	OPT_INIT_OPT_JEIDA08,
	OPT_INIT_OPT_JEIDA10,
	OPT_INIT_OPT_COMPOSITE,
	OPT_INIT_OPT_MAX
} opt_init_opt;

typedef enum _opt_resolution {
	OPT_RESOLUTION_NONE,
	OPT_RESOLUTION_UHD,
	OPT_RESOLUTION_FHD,
	OPT_RESOLUTION_FHDI,
	OPT_RESOLUTION_DCI4K,
	OPT_RESOLUTION_FWXGA,
	OPT_RESOLUTION_720,
	OPT_RESOLUTION_480,
	OPT_RESOLUTION_480I,
	OPT_RESOLUTION_576,
	OPT_RESOLUTION_576I,
	OPT_RESOLUTION_640X400,
	OPT_RESOLUTION_720X400,
	OPT_RESOLUTION_VGA,
	OPT_RESOLUTION_SVGA,
	OPT_RESOLUTION_848X480,
	OPT_RESOLUTION_XGA,
	OPT_RESOLUTION_XGAP,
	OPT_RESOLUTION_WXGA,
	OPT_RESOLUTION_WSXGA,
	OPT_RESOLUTION_QVGA,
	OPT_RESOLUTION_SXGA,
	OPT_RESOLUTION_1360X768,
	OPT_RESOLUTION_SXGAP,
	OPT_RESOLUTION_WXGAP,
	OPT_RESOLUTION_UXGA,
	OPT_RESOLUTION_WSXGAP,
	OPT_RESOLUTION_WUXGA,
	OPT_RESOLUTION_1792x1344,
	OPT_RESOLUTION_1856x1392,
	OPT_RESOLUTION_1920x1440,
	OPT_RESOLUTION_WQXGA,
	OPT_RESOLUTION_QXGA,
	OPT_RESOLUTION_WQHD,
	OPT_RESOLUTION_1920x2160,
	OPT_RESOLUTION_FHDPC,
	OPT_RESOLUTION_MAX
} opt_resolution;

typedef enum _opt_fps {
	OPT_FPS_NONE,
	OPT_FPS_24P,
	OPT_FPS_25P,
	OPT_FPS_30P,
	OPT_FPS_50P,
	OPT_FPS_60P,
	OPT_FPS_60PRB,
	OPT_FPS_120P,
	OPT_FPS_120PRB,
	OPT_FPS_56P,
	OPT_FPS_65P,
	OPT_FPS_70P,
	OPT_FPS_72P,
	OPT_FPS_75P,
	OPT_FPS_85P,
	OPT_FPS_144P,
	OPT_FPS_240P,
	OPT_FPS_MAX
} opt_fps;

typedef enum _opt_color_format {
	OPT_COLOR_FORMAT_NONE,
	OPT_COLOR_FORMAT_YCBCR420_BT601,
	OPT_COLOR_FORMAT_YCBCR422_BT601,
	OPT_COLOR_FORMAT_YCBCR444_BT601,
	OPT_COLOR_FORMAT_YCBCR420_BT709,
	OPT_COLOR_FORMAT_YCBCR422_BT709,
	OPT_COLOR_FORMAT_YCBCR444_BT709,
	OPT_COLOR_FORMAT_YCBCR420_BT2020,
	OPT_COLOR_FORMAT_YCBCR422_BT2020,
	OPT_COLOR_FORMAT_YCBCR444_BT2020,
	OPT_COLOR_FORMAT_XVYCC422_BT601,
	OPT_COLOR_FORMAT_XVYCC444_BT601,
	OPT_COLOR_FORMAT_XVYCC422_BT709,
	OPT_COLOR_FORMAT_XVYCC444_BT709,
	OPT_COLOR_FORMAT_RGB_FULL1,
	OPT_COLOR_FORMAT_RGB_FULL2,
	OPT_COLOR_FORMAT_RGB_LIMITED,
	OPT_COLOR_FORMAT_RGB_BT2020,
	OPT_COLOR_FORMAT_MAX
} opt_color_format;

typedef enum _opt_deep_color {
	OPT_DEEP_COLOR_NONE,
	OPT_DEEP_COLOR_8,
	OPT_DEEP_COLOR_10,
	OPT_DEEP_COLOR_12,
	OPT_DEEP_COLOR_MAX
} opt_deep_color;

typedef enum _opt_freq {
	OPT_FREQ_NONE,
	OPT_FREQ_RSH,
	OPT_FREQ_32,
	OPT_FREQ_44_1,
	OPT_FREQ_48,
	OPT_FREQ_88_2,
	OPT_FREQ_96,
	OPT_FREQ_176_4,
	OPT_FREQ_192,
	OPT_FREQ_64,
	OPT_FREQ_128,
	OPT_FREQ_256,
	OPT_FREQ_352_8,
	OPT_FREQ_384,
	OPT_FREQ_512,
	OPT_FREQ_705_6,
	OPT_FREQ_768,
	OPT_FREQ_1024,
	OPT_FREQ_1411_2,
	OPT_FREQ_1536,
	OPT_FREQ_MAX
} opt_freq;

typedef enum _opt_freq_ratio {
	OPT_FREQ_RATIO_NONE,
	OPT_FREQ_RATIO_1_4,
	OPT_FREQ_RATIO_1_2,
	OPT_FREQ_RATIO_1_1,
	OPT_FREQ_RATIO_2_1,
	OPT_FREQ_RATIO_4_1,
	OPT_FREQ_RATIO_MAX
} opt_freq_ratio;

typedef enum _opt_cc {
	OPT_CC_NONE,
	OPT_CC_RSH,
	OPT_CC_2CH,
	OPT_CC_3CH,
	OPT_CC_4CH,
	OPT_CC_5CH,
	OPT_CC_6CH,
	OPT_CC_7CH,
	OPT_CC_8CH,
	OPT_CC_MAX
} opt_cc;

typedef enum _opt_ss {
	OPT_SS_NONE,
	OPT_SS_RSH,
	OPT_SS_16BIT,
	OPT_SS_20BIT,
	OPT_SS_24BIT,
	OPT_SS_MAX
} opt_ss;

typedef enum _opt_em {
	OPT_EM_NONE,
	OPT_EM_MSB,
	OPT_EM_LSB,
	OPT_EM_MAX
} opt_em;

typedef enum _opt_po {
	OPT_PO_NONE,
	OPT_PO_LOW,
	OPT_PO_HIGH,
	OPT_PO_MAX
} opt_po;

typedef enum _opt_if {
	OPT_IF_NONE,
	OPT_IF_DISABLE,
	OPT_IF_ENABLE,
	OPT_IF_MAX
} opt_if;

typedef enum _opt_sp {
	OPT_SP_NONE,
	OPT_SP_LEFT,
	OPT_SP_RIGHT,
	OPT_SP_MAX
} opt_sp;

typedef enum _opt_mute_opt {
	OPT_MUTE_OPT_NONE,
	OPT_MUTE_OPT_AVMUTE,
	OPT_MUTE_OPT_BLACK,
	OPT_MUTE_OPT_MAX
} opt_mute_opt;

typedef enum _opt_hdcp_ver {
	OPT_HDCP_VER_NONE,
	OPT_HDCP_VER_14,
	OPT_HDCP_VER_22,
	OPT_HDCP_VER_AUTO,
	OPT_HDCP_VER_MAX
} opt_hdcp_ver;

typedef enum _opt_repeater {
	OPT_REPEATER_NONE,
	OPT_REPEATER_OFF,
	OPT_REPEATER_ON,
	OPT_REPEATER_MAX
} opt_repeater;

typedef enum _opt_dvi_mode {
	OPT_DVI_MODE_DISABLE,
	OPT_DVI_MODE_ENABLE,
	OPT_DVI_MODE_MAX
} opt_dvi_mode;

typedef struct _opts_main {
	opt_target			target;
	opt_target2			target2;
	opt_port			port;
	opt_txrx			txrx;
	opt_stat			stat;
	opt_init_opt		init_opt;
	opt_resolution		resolution;
	opt_fps				fps;
	opt_color_format	color_format;
	opt_deep_color		deep_color;
	opt_mute_opt		mute_opt;
	opt_cc				if_cc;
	opt_freq			if_sf;
	opt_ss				if_ss;
	uint8_t				if_ca;
	uint8_t				if_lsv;
	opt_freq			cb_sf;
	opt_ss				cb_ss;
	opt_freq_ratio		sf_ratio;
	opt_em				i2s_em;
	opt_po				i2s_po;
	opt_if				i2s_if;
	opt_sp				i2s_sp;
	opt_hdcp_ver		hdcp_ver;
	opt_repeater		repeater;
	opt_dvi_mode		dvi_mode;
	uint8_t				port_num;
	uint8_t				chip_num;
	uint8_t				bus_num;
	uint32_t			times;
	uint32_t			msec;
	uint8_t				oaddr;
	uint8_t				data;
	cmd_main_id			id_help;
} opts_main;

#define MAX_TOK_NUM						16
#define MAX_TOK_SIZE					11
typedef struct _cmds_main {
	char				str[MAX_LINE];
	char				tok[MAX_TOK_NUM][MAX_TOK_SIZE];
	cmd_main_id			id;
	opts_main			opts;
} cmds_main;

//
// i2c command & option
//
#define MAX_I2C_WRITE_SIZE				16
typedef enum _cmd_i2c_id {
	CMD_I2C_ID_NONE,
	CMD_I2C_ID_UNKNOWN,
	CMD_I2C_ID_I2CR,
	CMD_I2C_ID_I2CW,
	CMD_I2C_ID_I2CP,
	CMD_I2C_ID_SLEEP,
	CMD_I2C_ID_WAIT,
	CMD_I2C_ID_HELP,
	CMD_I2C_ID_QUIT,
	CMD_I2C_ID_MAX
} cmd_i2c_id;

typedef struct _opts_i2c {
	uint8_t		oaddr;
	uint8_t		saddr;
	uint16_t	size;
	uint8_t		data[MAX_I2C_WRITE_SIZE];
	uint8_t		mask;
	uint32_t	msec;
	uint8_t		saddr2;
	uint8_t		oaddr2;
	uint8_t		p_saddr;
	uint8_t		p_oaddr;
	uint8_t		p_rd;
	uint8_t		p_wr;
	cmd_i2c_id	id_help;
} opts_i2c;

typedef struct _cmds_i2c {
	char				str[MAX_LINE];
	cmd_i2c_id			id;
	opts_i2c			opts;
} cmds_i2c;

#pragma pack()

void del_line_comment(char *s);
void opts_main_usage(cmds_main *cmd);
void opts_main_parsing(cmds_main *cmd);
void opts_i2c_usage(cmds_i2c *cmd);
void opts_i2c_parsing(cmds_i2c *cmd);

#ifdef __cplusplus
}
#endif

#endif // __OPTS_H__
