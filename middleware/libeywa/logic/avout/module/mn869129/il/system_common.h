/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __SYSTEM_COMMON_H__
#define __SYSTEM_COMMON_H__

#include <stdint.h>

#pragma pack(1)
//------------------------------------------------------------------------------
#define SYSTEM_CHIP_NUMBER_0		0	// I2C slave address: 0x10, 0x20, 0x30
#define SYSTEM_CHIP_NUMBER_1		1	// I2C slave address: 0x40, 0x50, 0x60
#define SYSTEM_CHIP_NUMBER_2		2	// I2C slave address: 0x70, 0x80, 0x90
#define SYSTEM_CHIP_NUMBER_3		3	// I2C slave address: 0xA0, 0xB0, 0xC0
#define SYSTEM_CHIP_MAX_NUMBER		4
#define SYSTEM_RXTX_TYPE_RX			0
#define SYSTEM_RXTX_TYPE_TX			1
#define SYSTEM_PORT_TYPE_HDMI		0
#define SYSTEM_PORT_TYPE_VBO		1
#define SYSTEM_PORT_TYPE_LVDS		2
#define SYSTEM_BUS_NUMBER_0			0
#define SYSTEM_BUS_NUMBER_1			1
#define SYSTEM_PORT_NUMBER_0		0
#define SYSTEM_PORT_NUMBER_1		1
#define SYSTEM_PORT_NUMBER_2		2
#define SYSTEM_PORT_NUMBER_3		3
#define SYSTEM_PORT_MAX_NUMBER		4
//------------------------------------------------------------------------------
typedef struct _system_port {
	uint8_t port_num  : 2;				// bit[1:0]  port number (0-3)
	uint8_t bus_num   : 1;				// bit[2]    serial bus number (0-1)
	uint8_t port_type : 2;				// bit[4:3]  port type (HDMI=0, VBO=1, LVDS=2)
	uint8_t rxtx_type : 1;				// bit[5]    RxTx type (Rx=0, Tx=1)
	uint8_t chip_num  : 2;				// bit[7:6]  chip number (0-3)
} system_port;
//------------------------------------------------------------------------------
typedef struct _system_command {
	uint32_t addr;						// address (0x0000ssoo: ss=slave, oo=offset)
	uint32_t data;						// write/read data
	uint32_t mask;						// write/read mask
} system_command;
//------------------------------------------------------------------------------
typedef struct _system_reg_access {
	system_port		*p_port;			// pointer to system_port
	uint32_t		count;				// number of system_command
	system_command	*p_com;				// pointer to system_command
} system_reg_access;
//------------------------------------------------------------------------------
typedef struct _system_block_access {
	system_port		*p_port;			// ponter to system_port
	uint32_t		addr;				// address (0x0000ssoo: ss=slave, oo=offset)
	uint32_t		length;				// write/read length
	uint8_t			*p_data;			// ponter to write/read data
} system_block_access;
//------------------------------------------------------------------------------
typedef enum _system_video_format {
	SYSTEM_VFORM_UNUSED,
	SYSTEM_VFORM_640x480P_5994or60_4v3 = 1,
	SYSTEM_VFORM_720x480P_5994or60_4v3,
	SYSTEM_VFORM_720x480P_5994or60_16v9,
	SYSTEM_VFORM_1280x720P_5994or60_16v9,
	SYSTEM_VFORM_1920x1080I_5994or60_16v9,
	SYSTEM_VFORM_720x480I_5994or60_4v3,
	SYSTEM_VFORM_720x480I_5994or60_16v9,
	SYSTEM_VFORM_720x240P_5994or60_4v3,
	SYSTEM_VFORM_720x240P_5994or60_16v9,
	SYSTEM_VFORM_2880x480I_5994or60_4v3,		// 10
	SYSTEM_VFORM_2880x480I_5994or60_16v9,
	SYSTEM_VFORM_2880x240P_5994or60_4v3,
	SYSTEM_VFORM_2880x240P_5994or60_16v9,
	SYSTEM_VFORM_1440x480P_5994or60_4v3,
	SYSTEM_VFORM_1440x480P_5994or60_16v9,
	SYSTEM_VFORM_1920x1080P_5994or60_16v9,
	SYSTEM_VFORM_720x576P_50_4v3,
	SYSTEM_VFORM_720x576P_50_16v9,
	SYSTEM_VFORM_1280x720P_50_16v9,
	SYSTEM_VFORM_1920x1080I_50_16v9,			// 20
	SYSTEM_VFORM_720x576I_50_4v3,
	SYSTEM_VFORM_720x576I_50_16v9,
	SYSTEM_VFORM_720x288P_50_4v3,
	SYSTEM_VFORM_720x288P_50_16v9,
	SYSTEM_VFORM_2880x576I_50_4v3,
	SYSTEM_VFORM_2880x576I_50_16v9,
	SYSTEM_VFORM_2880x288P_50_4v3,
	SYSTEM_VFORM_2880x288P_50_16v9,
	SYSTEM_VFORM_1440x576P_50_4v3,
	SYSTEM_VFORM_1440x576P_50_16v9,				// 30
	SYSTEM_VFORM_1920x1080P_50_16v9,
	SYSTEM_VFORM_1920x1080P_2398or24_16v9,
	SYSTEM_VFORM_1920x1080P_25_16v9,
	SYSTEM_VFORM_1920x1080P_2997or30_16v9,
	SYSTEM_VFORM_2880x480P_5994or60_4v3,
	SYSTEM_VFORM_2880x480P_5994or60_16v9,
	SYSTEM_VFORM_2880x576P_50_4v3,
	SYSTEM_VFORM_2880x576P_50_16v9,
	SYSTEM_VFORM_1920x1080I_T1250_50_16v9,
	SYSTEM_VFORM_1920x1080I_100_16v9,			// 40
	SYSTEM_VFORM_1280x720P_100_16v9,
	SYSTEM_VFORM_720x576P_100_4v3,
	SYSTEM_VFORM_720x576P_100_16v9,
	SYSTEM_VFORM_720x576I_100_4v3,
	SYSTEM_VFORM_720x576I_100_16v9,
	SYSTEM_VFORM_1920x1080I_11988or120_16v9,
	SYSTEM_VFORM_1280x720P_11988or120_16v9,
	SYSTEM_VFORM_720x480P_11988or120_4v3,
	SYSTEM_VFORM_720x480P_11988or120_16v9,
	SYSTEM_VFORM_720x480I_11988or120_4v3,		// 50
	SYSTEM_VFORM_720x480I_11988or120_16v9,
	SYSTEM_VFORM_720x576P_200_4v3,
	SYSTEM_VFORM_720x576P_200_16v9,
	SYSTEM_VFORM_720x576I_200_4v3,
	SYSTEM_VFORM_720x576I_200_16v9,
	SYSTEM_VFORM_720x480P_23976or240_4v3,
	SYSTEM_VFORM_720x480P_23976or240_16v9,
	SYSTEM_VFORM_720x480I_23976or240_4v3,
	SYSTEM_VFORM_720x480I_23976or240_16v9,
	SYSTEM_VFORM_1280x720P_2398or24_16v9,		// 60
	SYSTEM_VFORM_1280x720P_25_16v9,
	SYSTEM_VFORM_1280x720P_2997or30_16v9,
	SYSTEM_VFORM_1920x1080P_11988or120_16v9,
	SYSTEM_VFORM_1920x1080P_100_16v9,			// 64 (0x40)
	// 65-92: non-support
	// 4K2K video format
	SYSTEM_VFORM_3840x2160P_2398or24_16v9 = 93,
	SYSTEM_VFORM_3840x2160P_25_16v9,
	SYSTEM_VFORM_3840x2160P_2997or30_16v9,
	SYSTEM_VFORM_3840x2160P_50_16v9,
	SYSTEM_VFORM_3840x2160P_5994or60_16v9,
	SYSTEM_VFORM_4096x2160P_2398or24_256v135,
	SYSTEM_VFORM_4096x2160P_25_256v135,
	SYSTEM_VFORM_4096x2160P_2997or30_256v135,	// 100
	SYSTEM_VFORM_4096x2160P_50_256v135,
	SYSTEM_VFORM_4096x2160P_5994or60_256v135,
	// 103-107: non-support
	// 129-144: extended video format
	SYSTEM_VFORM_1366x768P_5979or60_16v9 = 129,
	SYSTEM_VFORM_PC_VESA_DMT_ID_UNUSED,			// 130
	SYSTEM_VFORM_PC_EGA_640x350_85,
	SYSTEM_VFORM_PC_640x400_85,
	SYSTEM_VFORM_PC_720x400_85,
	SYSTEM_VFORM_PC_VGA_640x480_60,				// not supported
	SYSTEM_VFORM_PC_VGA_640x480_72,
	SYSTEM_VFORM_PC_VGA_640x480_75,
	SYSTEM_VFORM_PC_VGA_640x480_85,
	SYSTEM_VFORM_PC_SVGA_800x600_56,
	SYSTEM_VFORM_PC_SVGA_800x600_60,
	SYSTEM_VFORM_PC_SVGA_800x600_72,			// 140
	SYSTEM_VFORM_PC_SVGA_800x600_75,
	SYSTEM_VFORM_PC_SVGA_800x600_85,
	SYSTEM_VFORM_PC_SVGA_800x600_120_RB,
	SYSTEM_VFORM_PC_848x480_60,
	SYSTEM_VFORM_PC_XGA_1024x768_43,			// not supported
	SYSTEM_VFORM_PC_XGA_1024x768_60,
	SYSTEM_VFORM_PC_XGA_1024x768_70,
	SYSTEM_VFORM_PC_XGA_1024x768_75,
	SYSTEM_VFORM_PC_XGA_1024x768_85,
	SYSTEM_VFORM_PC_XGA_1024x768_120_RB,		// 150
	SYSTEM_VFORM_PC_XGAp_1152x864_75,
	SYSTEM_VFORM_PC_WXGA_1280x768_60_RB,
	SYSTEM_VFORM_PC_WXGA_1280x768_60,
	SYSTEM_VFORM_PC_WXGA_1280x768_75,
	SYSTEM_VFORM_PC_WXGA_1280x768_85,
	SYSTEM_VFORM_PC_WXGA_1280x768_120_RB,
	SYSTEM_VFORM_PC_WSXGA_1280x800_60_RB,
	SYSTEM_VFORM_PC_WSXGA_1280x800_60,
	SYSTEM_VFORM_PC_WSXGA_1280x800_75,
	SYSTEM_VFORM_PC_WSXGA_1280x800_85,			// 160
	SYSTEM_VFORM_PC_WSXGA_1280x800_120_RB,
	SYSTEM_VFORM_PC_QVGA_1280x960_60,
	SYSTEM_VFORM_PC_QVGA_1280x960_85,
	SYSTEM_VFORM_PC_QVGA_1280x960_120_RB,
	SYSTEM_VFORM_PC_SXGA_1280x1024_60,
	SYSTEM_VFORM_PC_SXGA_1280x1024_75,
	SYSTEM_VFORM_PC_SXGA_1280x1024_85,
	SYSTEM_VFORM_PC_SXGA_1280x1024_120_RB,
	SYSTEM_VFORM_PC_1360x768_60,
	SYSTEM_VFORM_PC_1360x768_120_RB,			// 170
	SYSTEM_VFORM_PC_SXGAp_1400x1050_60_RB,
	SYSTEM_VFORM_PC_SXGAp_1400x1050_60,
	SYSTEM_VFORM_PC_SXGAp_1400x1050_75,
	SYSTEM_VFORM_PC_SXGAp_1400x1050_85,
	SYSTEM_VFORM_PC_SXGAp_1400x1050_120_RB,
	SYSTEM_VFORM_PC_WXGAp_1440x900_60_RB,
	SYSTEM_VFORM_PC_WXGAp_1440x900_60,
	SYSTEM_VFORM_PC_WXGAp_1440x900_75,
	SYSTEM_VFORM_PC_WXGAp_1440x900_85,
	SYSTEM_VFORM_PC_WXGAp_1440x900_120_RB,		// 180
	SYSTEM_VFORM_PC_UXGA_1600x1200_60,
	SYSTEM_VFORM_PC_UXGA_1600x1200_65,
	SYSTEM_VFORM_PC_UXGA_1600x1200_70,
	SYSTEM_VFORM_PC_UXGA_1600x1200_75,
	SYSTEM_VFORM_PC_UXGA_1600x1200_85,
	SYSTEM_VFORM_PC_UXGA_1600x1200_120_RB,
	SYSTEM_VFORM_PC_WSXGAp_1680x1050_60_RB,
	SYSTEM_VFORM_PC_WSXGAp_1680x1050_60,
	SYSTEM_VFORM_PC_WSXGAp_1680x1050_75,
	SYSTEM_VFORM_PC_WSXGAp_1680x1050_85,		// 190
	SYSTEM_VFORM_PC_WSXGAp_1680x1050_120_RB,
	SYSTEM_VFORM_PC_1792x1344_60,
	SYSTEM_VFORM_PC_1792x1344_75,
	SYSTEM_VFORM_PC_1792x1344_120_RB,
	SYSTEM_VFORM_PC_1856x1392_60,
	SYSTEM_VFORM_PC_1856x1392_75,
	SYSTEM_VFORM_PC_1856x1392_120_RB,
	SYSTEM_VFORM_PC_WUXGA_1920x1200_60_RB,
	SYSTEM_VFORM_PC_WUXGA_1920x1200_60,
	SYSTEM_VFORM_PC_WUXGA_1920x1200_75,			// 200
	SYSTEM_VFORM_PC_WUXGA_1920x1200_85,
	SYSTEM_VFORM_PC_WUXGA_1920x1200_120_RB,		// not supported
	SYSTEM_VFORM_PC_1920x1440_60,
	SYSTEM_VFORM_PC_1920x1440_75,
	SYSTEM_VFORM_PC_1920x1440_120_RB,
	SYSTEM_VFORM_PC_WQXGA_2560x1600_60_RB,
	SYSTEM_VFORM_PC_WQXGA_2560x1600_60,
	SYSTEM_VFORM_PC_WQXGA_2560x1600_75,
	SYSTEM_VFORM_PC_WQXGA_2560x1600_85,
	SYSTEM_VFORM_PC_WQXGA_2560x1600_120_RB,		// 210
	SYSTEM_VFORM_PC_FWXGA_1366x768_60,
	SYSTEM_VFORM_PC_FHD_1920x1080_60,
	SYSTEM_VFORM_PC_WXGApp_1600x900_60_RB,
	SYSTEM_VFORM_PC_QWXGA_2048x1152_60_RB,
	SYSTEM_VFORM_PC_HD_1280x720_60,
	SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB,
	SYSTEM_VFORM_PC_DCI4K_4096x2160_60_RB,		// not supported
	SYSTEM_VFORM_PC_DCI4K_4096x2160_5994_RB,	// not supported
	SYSTEM_VFORM_PC_VESA_DMT_ID_MAX,
	SYSTEM_VFORM_PC_VARIANT_UNUSED,				// 220
	SYSTEM_VFORM_PC_QXGA_2048x1536_60,
	SYSTEM_VFORM_PC_WQHD_2560x1440_60_RB,
	SYSTEM_VFORM_PC_WQHD_2560x1440_60,
	SYSTEM_VFORM_PC_1920x2160_60,
	SYSTEM_VFORM_PC_FHD_1920x1080_120_1,
	SYSTEM_VFORM_PC_FHD_1920x1080_144_1,
	SYSTEM_VFORM_PC_FHD_1920x1080_240_1,
	SYSTEM_VFORM_PC_VARIANT_MAX,
	SYSTEM_VFORM_MAX
} system_video_format;
//------------------------------------------------------------------------------
typedef enum _system_color_format {
	SYSTEM_COLOR_YCBCR422_ITU_R_BT601 = 0,
	SYSTEM_COLOR_XVYCC422_ITU_R_BT601,
	SYSTEM_COLOR_YCBCR444_ITU_R_BT601,
	SYSTEM_COLOR_XVYCC444_ITU_R_BT601,
	SYSTEM_COLOR_YCBCR422_ITU_R_BT709,
	SYSTEM_COLOR_XVYCC422_ITU_R_BT709,
	SYSTEM_COLOR_YCBCR444_ITU_R_BT709,
	SYSTEM_COLOR_XVYCC444_ITU_R_BT709,
	SYSTEM_COLOR_RGB_FULL_RANGE1,
	SYSTEM_COLOR_UNUSED1,
	SYSTEM_COLOR_RGB_LIMITED_RANGE,
	SYSTEM_COLOR_UNUSED2,
	SYSTEM_COLOR_RGB_FULL_RANGE2,
	SYSTEM_COLOR_YCBCR420_ITU_R_BT601,
	SYSTEM_COLOR_UNUSED3,
	SYSTEM_COLOR_YCBCR420_ITU_R_BT709,
	SYSTEM_COLOR_YCBCR420_ITU_R_BT2020,
	SYSTEM_COLOR_YCBCR422_ITU_R_BT2020,
	SYSTEM_COLOR_YCBCR444_ITU_R_BT2020,
	SYSTEM_COLOR_RGB_ITU_R_BT2020,
	SYSTEM_COLOR_MAX
} system_color_format;
//------------------------------------------------------------------------------
typedef enum _system_deep_color {
	SYSTEM_DEEP_COLOR_8BIT,
	SYSTEM_DEEP_COLOR_10BIT,
	SYSTEM_DEEP_COLOR_12BIT,
	SYSTEM_DEEP_COLOR_MAX
} system_deep_color;
//------------------------------------------------------------------------------
typedef struct _system_video {
	system_port			rx_port;			// Rx port number
	system_port			tx_port;			// Tx port number
	system_video_format	rx_video_format;	// Rx video format
	system_color_format	rx_color_format;	// Rx color format
	system_deep_color	rx_deep_color;		// Rx deep color
	system_video_format	tx_video_format;	// Tx video format
	system_color_format	tx_color_format;	// Tx color format
	system_deep_color	tx_deep_color;		// Tx deep color
} system_video;
//------------------------------------------------------------------------------
typedef enum _system_audio_cc {			// channel count
	SYSTEM_AUDIO_CC_RSH,					// Refer to Stream Header
	SYSTEM_AUDIO_CC_2CH,					// 2CH
	SYSTEM_AUDIO_CC_3CH,					// 3CH
	SYSTEM_AUDIO_CC_4CH,					// 4CH
	SYSTEM_AUDIO_CC_5CH,					// 5CH
	SYSTEM_AUDIO_CC_6CH,					// 6CH
	SYSTEM_AUDIO_CC_7CH,					// 7CH
	SYSTEM_AUDIO_CC_8CH,					// 8CH
	SYSTEM_AUDIO_CC_MAX
} system_audio_cc;
//------------------------------------------------------------------------------
typedef enum _system_audio_sf {			// sample frequency
	SYSTEM_AUDIO_SF_RSH,					// Refer to Stream Header
	SYSTEM_AUDIO_SF_32KHZ,					//   32  kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_44KHZ,					//   44.1kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_48KHZ,					//   48  kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_88KHZ,					//   88.2kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_96KHZ,					//   96  kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_176KHZ,					//  176.4kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_192KHZ,					//  192  kHz (AS, OBA, DST)
	SYSTEM_AUDIO_SF_64KHZ,					//   64  kHz (AS w/ 3D/Multi-Stream)
	SYSTEM_AUDIO_SF_128KHZ,					//  128  kHz (AS w/ 3D/Multi-Stream)
	SYSTEM_AUDIO_SF_256KHZ,					//  256  kHz (HBR)
	SYSTEM_AUDIO_SF_352KHZ,					//  352.8kHz (HBR)
	SYSTEM_AUDIO_SF_384KHZ,					//  384  kHz (HBR)
	SYSTEM_AUDIO_SF_512KHZ,					//  512  kHz (HBR)
	SYSTEM_AUDIO_SF_705KHZ,					//  705.6kHz (HBR)
	SYSTEM_AUDIO_SF_768KHZ,					//  768  kHz (HBR)
	SYSTEM_AUDIO_SF_1024KHZ,				// 1024  kHz (HBR)
	SYSTEM_AUDIO_SF_1411KHZ,				// 1411.2kHz (HBR)
	SYSTEM_AUDIO_SF_1536KHZ,				// 1536  kHz (HBR)
	SYSTEM_AUDIO_SF_MAX
} system_audio_sf;
//------------------------------------------------------------------------------
typedef enum _system_audio_ss {			// sample size
	SYSTEM_AUDIO_SS_RSH,					// Refer to Stream Header
	SYSTEM_AUDIO_SS_16BIT,					// 16 bit
	SYSTEM_AUDIO_SS_20BIT,					// 20 bit
	SYSTEM_AUDIO_SS_24BIT,					// 24 bit
	SYSTEM_AUDIO_SS_20BIT_0,				// 20 bit for cbit
	SYSTEM_AUDIO_SS_20BIT_1,				// 20 bit for cbit
	SYSTEM_AUDIO_MAX_20BIT,					// max 20 bit for cbit
	SYSTEM_AUDIO_MAX_24BIT,					// max 24 bit for cbit
	SYSTEM_AUDIO_SS_MAX
} system_audio_ss;
//------------------------------------------------------------------------------
typedef enum _system_audio_sf_ratio {	// sampling frequency ratio
	SYSTEM_AUDIO_SF_RATIO_1_4,				// 1/4 down sampling
	SYSTEM_AUDIO_SF_RATIO_1_2,				// 1/2 down sampling
	SYSTEM_AUDIO_SF_RATIO_1_1,				// x1 normal sampling
	SYSTEM_AUDIO_SF_RATIO_2_1,				// x2 over sampling
	SYSTEM_AUDIO_SF_RATIO_4_1,				// x4 over sampling
	SYSTEM_AUDIO_SF_RATIO_MAX
} system_audio_sf_ratio;
//------------------------------------------------------------------------------
typedef enum _system_audio_em {			// endian mode
	SYSTEM_AUDIO_EM_MSB,					// MSB first
	SYSTEM_AUDIO_EM_LSB,					// LSB first
	SYSTEM_AUDIO_EM_MAX
} system_audio_em;
//------------------------------------------------------------------------------
typedef enum _system_audio_po {			// polarity LR
	SYSTEM_AUDIO_PO_LOW,					// L-ch=Low
	SYSTEM_AUDIO_PO_HIGH,					// L-ch=High
	SYSTEM_AUDIO_PO_MAX
} system_audio_po;
//------------------------------------------------------------------------------
typedef enum _system_audio_if {			// I2S format
	SYSTEM_AUDIO_IF_DISABLE,				// disable
	SYSTEM_AUDIO_IF_ENABLE,					// enable
	SYSTEM_AUDIO_IF_MAX
} system_audio_if;
//------------------------------------------------------------------------------
typedef enum _system_audio_sp {			// starting position of audio data
	SYSTEM_AUDIO_SP_RIGHT,					// Right justified
	SYSTEM_AUDIO_SP_LEFT,					// Left justified
	SYSTEM_AUDIO_SP_MAX
} system_audio_sp;
//------------------------------------------------------------------------------
typedef struct _system_audio_i2s {
	system_audio_em			endian_mode;
	system_audio_po			polarity;
	system_audio_if			i2s_format;
	system_audio_sp			start_pos;
} system_audio_i2s;
//------------------------------------------------------------------------------
typedef struct _system_audio {
	system_port				port;
	system_audio_cc			if_channel_count;		// audio infoframe
	system_audio_sf			if_sampling_freq;		// audio infoframe
	system_audio_ss			if_sample_size;			// audio infoframe
	uint8_t					if_channel_number;		// audio infoframe
	uint8_t					if_level_shift_value;	// audio infoframe
	system_audio_sf			cb_sampling_freq;		// channel status bits
	system_audio_ss			cb_sample_size;			// channel status bits
	system_audio_sf_ratio	sampling_freq_ratio;
	system_audio_i2s		i2s;
} system_audio;
//------------------------------------------------------------------------------
typedef enum _system_inversion {
	SYSTEM_INVERSION_NONE,
	SYSTEM_INVERSION_ON,
	SYSTEM_INVERSION_OFF,
	SYSTEM_INVERSION_MAX
} system_inversion;
//------------------------------------------------------------------------------
typedef struct _system_config {
	system_port			port;
	system_inversion	hvsync_polarity;
} system_config;
//------------------------------------------------------------------------------
typedef enum _system_tx_mute {
	SYSTEM_TX_MUTE_NONE,
	SYSTEM_TX_MUTE_ON,
	SYSTEM_TX_MUTE_OFF,
	SYSTEM_TX_MUTE_ON_AVMUTE,			// used by hdmitx
	SYSTEM_TX_MUTE_OFF_AVMUTE,			// used by hdmitx
	SYSTEM_TX_MUTE_ON_BLACK,			// used by vbotx and lvdstx
	SYSTEM_TX_MUTE_OFF_BLACK,			// used by vbotx and lvdstx
	SYSTEM_TX_MUTE_MAX
} system_tx_mute;
//------------------------------------------------------------------------------
typedef struct _system_mute {
	system_port		port;
	system_tx_mute	mute;
} system_mute;
//------------------------------------------------------------------------------
typedef enum _system_update {
	SYSTEM_UPDATE_DISABLE,
	SYSTEM_UPDATE_ENABLE,
	SYSTEM_UPDATE_MAX
} system_update;
//------------------------------------------------------------------------------
typedef struct _system_config_retry {
	system_update	update;				// whether or not to update by the following
	uint32_t		times;				// maximum number of times for the retry operation
	uint32_t		interval;			// retry interval (millisecond)
} system_config_retry;
//------------------------------------------------------------------------------
#define SYSTEM_SPD_INFOFRAME_VN_BYTE_SIZE			8
#define SYSTEM_SPD_INFOFRAME_PD_BYTE_SIZE			16
//------------------------------------------------------------------------------
typedef struct _system_spd_infoframe {
	system_port		port;
	uint8_t			vendor_name[SYSTEM_SPD_INFOFRAME_VN_BYTE_SIZE];
	uint8_t			product_description[SYSTEM_SPD_INFOFRAME_PD_BYTE_SIZE];
	uint8_t			source_infomation;
} system_spd_infoframe;
//------------------------------------------------------------------------------
#define SYSTEM_HDR_INFOFRAME_SMD_BYTE_SIZE			24
//------------------------------------------------------------------------------
typedef struct _system_hdr_infoframe {
	system_port		port;
	uint8_t			eotf;
	uint8_t			static_metadata_descriptor_id;
	uint8_t			static_metadata_descriptor[SYSTEM_HDR_INFOFRAME_SMD_BYTE_SIZE];
} system_hdr_infoframe;
//------------------------------------------------------------------------------
typedef enum _system_onoff {
	SYSTEM_ONOFF_NONE,
	SYSTEM_ONOFF_ON,
	SYSTEM_ONOFF_OFF,
	SYSTEM_ONOFF_MAX
} system_onoff;
//------------------------------------------------------------------------------
typedef struct _system_power {
	system_port		port;
	system_onoff	onoff;
} system_power;
//------------------------------------------------------------------------------
typedef enum _system_sg_format {
	SYSTEM_SG_FORMAT_NONE,
	SYSTEM_SG_FORMAT_ASYNC_BLANK,		// blank (asynchronous)
	SYSTEM_SG_FORMAT_BLANK,				// blank (Vsync synchronous)
	SYSTEM_SG_FORMAT_RAMP_A,			// ramp A (Vsync synchronous)
	SYSTEM_SG_FORMAT_RAMP_B,			// ramp B (Vsync synchronous)
	SYSTEM_SG_FORMAT_TOGGLE,			// toggle (Vsync synchronous)
	SYSTEM_SG_FORMAT_COLOR_BAR_RGB,		// color bar by RGB (Vsync synchronous)
	SYSTEM_SG_FORMAT_COLOR_BAR_444,		// color bar by YCbCr444 (Vsync synchronous)
	SYSTEM_SG_FORMAT_MAX
} system_sg_format;
//------------------------------------------------------------------------------
typedef struct _system_sg {
	system_onoff		onoff;
	system_sg_format	sg_format;
} system_sg;
//------------------------------------------------------------------------------
typedef enum _system_request {
	SYSTEM_REQUEST_NONE,
	SYSTEM_REQUEST_ABORT,
	SYSTEM_REQUEST_NOCHECK,
	SYSTEM_REQUEST_MAX
} system_request;
//------------------------------------------------------------------------------
#pragma pack()

#endif // __SYSTEM_COMMON_H__
