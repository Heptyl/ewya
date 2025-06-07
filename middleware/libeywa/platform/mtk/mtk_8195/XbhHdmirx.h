/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#ifndef _HDMIRX_H_
#define  _HDMIRX_H_
#include "XbhMtk_8195.h"

#define VENDORSPEC_INFOFRAME_TYPE 0x01
#define AVI_INFOFRAME_TYPE 0x02
#define SPD_INFOFRAME_TYPE 0x03
#define AUDIO_INFOFRAME_TYPE 0x04
#define MPEG_INFOFRAME_TYPE 0x05

#define VENDORSPEC_INFOFRAME_VER 0x01
#define AVI_INFOFRAME_VER 0x02
#define SPD_INFOFRAME_VER 0x01
#define AUDIO_INFOFRAME_VER 0x01
#define MPEG_INFOFRAME_VER 0x01

#define VENDORSPEC_INFOFRAME_LEN 8
#define AVI_INFOFRAME_LEN 13
#define SPD_INFOFRAME_LEN 25
#define AUDIO_INFOFRAME_LEN 10
#define MPEG_INFOFRAME_LEN 10

#define ACP_PKT_LEN 9
#define ISRC1_PKT_LEN 16
#define ISRC2_PKT_LEN 16

#define HDMI_RX_HDR_TYPE   4
#define HDMI_HDR_TYPE_MASK 0x7
#define HDR_2084_FLAG	2
#define HDR_HLG_FLAG	3

enum HDMIRX_CS {
	HDMI_CS_RGB = 0,
	HDMI_CS_YUV444,
	HDMI_CS_YUV422,
	HDMI_CS_YUV420
};

enum HdmiRxDP {
	HDMIRX_BIT_DEPTH_8_BIT = 0,
	HDMIRX_BIT_DEPTH_10_BIT,
	HDMIRX_BIT_DEPTH_12_BIT,
	HDMIRX_BIT_DEPTH_16_BIT
};

enum HdmiRxClrSpc {
	HDMI_RX_CLRSPC_UNKNOWN,
	HDMI_RX_CLRSPC_YC444_601,
	HDMI_RX_CLRSPC_YC422_601,
	HDMI_RX_CLRSPC_YC420_601,

	HDMI_RX_CLRSPC_YC444_709,
	HDMI_RX_CLRSPC_YC422_709,
	HDMI_RX_CLRSPC_YC420_709,

	HDMI_RX_CLRSPC_XVYC444_601,
	HDMI_RX_CLRSPC_XVYC422_601,
	HDMI_RX_CLRSPC_XVYC420_601,

	HDMI_RX_CLRSPC_XVYC444_709,
	HDMI_RX_CLRSPC_XVYC422_709,
	HDMI_RX_CLRSPC_XVYC420_709,

	HDMI_RX_CLRSPC_sYCC444_601,
	HDMI_RX_CLRSPC_sYCC422_601,
	HDMI_RX_CLRSPC_sYCC420_601,

	HDMI_RX_CLRSPC_Adobe_YCC444_601,
	HDMI_RX_CLRSPC_Adobe_YCC422_601,
	HDMI_RX_CLRSPC_Adobe_YCC420_601,

	HDMI_RX_CLRSPC_RGB,
	HDMI_RX_CLRSPC_Adobe_RGB,

	HDMI_RX_CLRSPC_BT_2020_RGB_non_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_RGB_const_luminous,

	HDMI_RX_CLRSPC_BT_2020_YCC444_non_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC422_non_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC420_non_const_luminous,

	HDMI_RX_CLRSPC_BT_2020_YCC444_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC422_const_luminous,
	HDMI_RX_CLRSPC_BT_2020_YCC420_const_luminous
};

enum HdmiRxRange {
	HDMI_RX_RGB_FULL,
	HDMI_RX_RGB_LIMT,
	HDMI_RX_YCC_FULL,
	HDMI_RX_YCC_LIMT
};

enum HdmiRxHdrType {
	HDMI_RX_SDR,
	HDMI_RX_HDR_2084,
	HDMI_RX_HDR_HLG
};

enum HdmiRxSpc {
	HDMI_RX_BT709,
	HDMI_RX_BT601,
	HDMI_RX_BT2020_NON_CONST,
	HDMI_RX_BT2020_CONST
};

// struct HDMIRX_DEV_INFO {
// 	unsigned char hdmirx5v;
// 	unsigned char hpd;
// 	unsigned int power_on;
// 	unsigned char state;
// 	unsigned char vid_locked;
// 	unsigned char aud_locked;
// 	unsigned char hdcp_version;
// 	unsigned char hdcp14_decrypted;
// 	unsigned char hdcp22_decrypted;
// };

struct HDMIRX_VID_PARA {
	enum HDMIRX_CS cs;
	enum HdmiRxDP dp;
	enum HdmiRxClrSpc ClrSpc;
	enum HdmiRxRange Range;
	unsigned int htotal;
	unsigned int vtotal;
	unsigned int hactive;
	unsigned int vactive;
	unsigned int is_pscan;
	unsigned char hdmi_mode;
	unsigned int frame_rate;
	unsigned int pixclk;
	unsigned int tmdsclk;
	unsigned char is_40x;
	unsigned int id;
};

struct hdr10InfoPkt {
	unsigned char type;
	unsigned char fgValid;
	unsigned char u1InfoData[32];
};

union Audio_InfoFrame {
	struct {
		unsigned char Type;
		unsigned char Ver;
		unsigned char Len;

		unsigned char AudioChannelCount : 3;
		unsigned char RSVD1 : 1;
		unsigned char AudioCodingType : 4;

		unsigned char SampleSize : 2;
		unsigned char SampleFreq : 3;
		unsigned char Rsvd2 : 3;

		unsigned char FmtCoding;

		unsigned char SpeakerPlacement;

		unsigned char Rsvd3 : 3;
		unsigned char LevelShiftValue : 4;
		unsigned char DM_INH : 1;
	} info;
	struct {
		unsigned char AUD_HB[3];
		unsigned char AUD_DB[AUDIO_INFOFRAME_LEN];
	} pktbyte;
};

struct RX_REG_AUDIO_CHSTS {
	unsigned char rev : 1;
	unsigned char IsLPCM : 1;
	unsigned char CopyRight : 1;
	unsigned char AdditionFormatInfo : 3;
	unsigned char ChannelStatusMode : 2;
	unsigned char CategoryCode;
	unsigned char SourceNumber : 4;
	unsigned char ChannelNumber : 4;
	unsigned char SamplingFreq : 4;
	unsigned char ClockAccuary : 2;
	unsigned char rev2 : 2;
	unsigned char WorldLen : 4;
	unsigned char OriginalSamplingFreq : 4;
};

enum RX_MCLK_FREQUENCY_T {
	RX_MCLK_128FS,
	RX_MCLK_192FS,
	RX_MCLK_256FS,
	RX_MCLK_384FS,
	RX_MCLK_512FS,
	RX_MCLK_768FS
};

struct AUDIO_CAPS {
	unsigned char AudioFlag;
	unsigned char AudSrcEnable;
	unsigned char SampleFreq;
	unsigned char ChStat[5];
	union Audio_InfoFrame AudInf;
	struct RX_REG_AUDIO_CHSTS AudChStat;
};

struct AUDIO_INFO {
	unsigned char u1HBRAudio;
	unsigned char u1DSDAudio;
	unsigned char u1RawSDAudio;
	unsigned char u1PCMMultiCh;
	unsigned char u1FsDec;
	unsigned char u1I2sChanValid;
	unsigned char u1I2sCh0Sel;
	unsigned char u1I2sCh1Sel;
	unsigned char u1I2sCh2Sel;
	unsigned char u1I2sCh3Sel;
	unsigned char u1I2sCh4Sel;
	enum RX_MCLK_FREQUENCY_T u1MCLKRatio;
};

struct HDMIRX_AUD_INFO {
	struct AUDIO_CAPS caps;
	struct AUDIO_INFO info;
};

enum HDMIRX_NOTIFY_T {
	HDMI_RX_PWR_5V_CHANGE = 0,
	HDMI_RX_TIMING_LOCK,
	HDMI_RX_TIMING_UNLOCK,
	HDMI_RX_AUD_LOCK,
	HDMI_RX_AUD_UNLOCK,
	HDMI_RX_ACP_PKT_CHANGE,
	HDMI_RX_AVI_INFO_CHANGE,
	HDMI_RX_AUD_INFO_CHANGE,
	HDMI_RX_HDR_INFO_CHANGE,
	HDMI_RX_EDID_CHANGE,
	HDMI_RX_HDCP_VERSION,
	HDMI_RX_PLUG_IN, /// 11
	HDMI_RX_PLUG_OUT, /// 12
};

enum HDMI_SWITCH_NO {
	HDMI_SWITCH_INIT = 0,
	HDMI_SWITCH_1,
	HDMI_SWITCH_2,
	HDMI_SWITCH_3,
	HDMI_SWITCH_4
};

#define HDMI_IOW(num, dtype)  _IOW('H', num, dtype)
#define HDMI_IOR(num, dtype) _IOR('H', num, dtype)
#define HDMI_IOWR(num, dtype) _IOWR('H', num, dtype)
#define HDMI_IO(num) _IO('H', num)

#define MTK_HDMIRX_VID_INFO HDMI_IOWR(1, struct HDMIRX_VID_PARA)
#define MTK_HDMIRX_AUD_INFO HDMI_IOWR(2, struct HDMIRX_AUD_INFO)
#define MTK_HDMIRX_ENABLE HDMI_IOW(3, unsigned int)
#define MTK_HDMIRX_DEV_INFO HDMI_IOWR(4, struct HDMIRX_DEV_INFO)
#define MTK_HDMIRX_SWITCH HDMI_IOW(5, unsigned int)
#define MTK_HDMIRX_PKT HDMI_IOWR(6, struct hdr10InfoPkt)
#define MTK_HDMIRX_EDID HDMI_IOW(7, unsigned int)
#define MTK_HDMIRX_AVI HDMI_IOWR(8, struct hdr10InfoPkt)
#define MTK_HDMIRX_HPD HDMI_IOW(9, unsigned int)
#define MTK_HDMIRX_FORCE5V HDMI_IOW(10, unsigned int)

#define CP_MTK_HDMIRX_VID_INFO HDMI_IOWR(1, struct HDMIRX_VID_PARA)
#define CP_MTK_HDMIRX_AUD_INFO HDMI_IOWR(2, struct HDMIRX_AUD_INFO)
#define CP_MTK_HDMIRX_ENABLE HDMI_IOW(3, unsigned int)
#define CP_MTK_HDMIRX_DEV_INFO HDMI_IOWR(4, struct HDMIRX_DEV_INFO)
#define CP_MTK_HDMIRX_SWITCH HDMI_IOW(5, unsigned int)
#define CP_MTK_HDMIRX_PKT HDMI_IOWR(6, struct hdr10InfoPkt)
#define CP_MTK_HDMIRX_EDID HDMI_IOW(7, unsigned int)
#define CP_MTK_HDMIRX_AVI HDMI_IOWR(8, struct hdr10InfoPkt)
#define CP_MTK_HDMIRX_HPD HDMI_IOW(9, unsigned int)
#define CP_MTK_HDMIRX_FORCE5V HDMI_IOW(10, unsigned int)

#endif
