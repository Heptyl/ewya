/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "hdmitx.h"
#include "ddc.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "hv_version.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// hdmitx_select_audio_ch()
#define HDMITX_SELECT_AUDIO_CH_NUM							1
#define HDMITX_SELECT_AUDIO_CH_TXMSTCTL_MASK				0x000000FF
static hdmitx_update_mode update_mode = HDMITX_UPDATE_MODE_DISABLE;
static uint32_t Is_UHD = 1;

//------------------------------------------------------------------------------
// hdmitx_init_audio_trans_setting()
#define HDMITX_INIT_AUDIO_TRANS_NUM							3
#define HDMITX_INIT_AUDIO_TRANS_TXAIFDBG_CTSMODSEL_CHANGE	0x80
#define HDMITX_INIT_AUDIO_TRANS_TXAIFDBG_MASK				0x000000FF
#define HDMITX_INIT_AUDIO_TRANS_CTSGEN_02_DPLLRST_NOR		0x00
#define HDMITX_INIT_AUDIO_TRANS_CTSGEN_02_PLLARSTMOD_RST	0x0C
#define HDMITX_INIT_AUDIO_TRANS_CTSGEN_02_MASK				0x000000FF
#define HDMITX_INIT_AUDIO_TRANS_ASTBCTL_ASTBFCNT_FIFO		0x08
#define HDMITX_INIT_AUDIO_TRANS_ASTBCTL_STBMODCTL_ON		0x02
#define HDMITX_INIT_AUDIO_TRANS_ASTBCTL_MASK				0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_infena()
#define HDMITX_SET_INFENA_NUM								1
//------------------------------------------------------------------------------
// hdmitx_set_pktena()
#define HDMITX_SET_PKTENA_NUM								1
//------------------------------------------------------------------------------
// hdmitx_set_tmds_ch_power()
#define HDMITX_SET_TMDS_CH_POWER_NUM						2
#define HDMITX_SET_TMDS_CH_POWER_ON							1
#define HDMITX_SET_TMDS_CH_POWER_OFF						0
#define HDMITX_TMDSTXP0_1_NPDCKO_DWN						0x00
#define HDMITX_TMDSTXP0_1_NPDCKO_NOR						0x01
#define HDMITX_TMDSTXP0_1_NPDCKO_MASK						0x00000001
#define HDMITX_TMDSTXP0_2_00_NPDDATX_DWN					0x00
#define HDMITX_TMDSTXP0_2_00_NPDDATX_NOR					0x70
#define HDMITX_TMDSTXP0_2_00_NPDDATX_MASK					0x00000070
//------------------------------------------------------------------------------
// hdmitx_set_scdc_source_version()
#define HDMITX_SET_SCDC_SOURCE_VERSION_ADDR					0x02
#define HDMITX_SET_SCDC_SOURCE_VERSION_LENGTH				0x01
//------------------------------------------------------------------------------
// hdmitx_set_scdc_tmds_config()
#define HDMITX_SET_SCDC_TMDS_CONFIG_ADDR					0x20
#define HDMITX_SET_SCDC_TMDS_CONFIG_LENGTH					0x01
//------------------------------------------------------------------------------
// hdmitx_set_scdcs()
#define HDMITX_VIDEO_FORM_SET_VERSION_4K2K					0x01
#define HDMITX_VIDEO_FORM_SET_CONFIG_4K2K					0x03
#define HDMITX_VIDEO_FORM_SET_VERSION_OTHER					0x00
#define HDMITX_VIDEO_FORM_SET_CONFIG_OTHER					0x00
//------------------------------------------------------------------------------
// hdmitx_set_txpll00en()
#define HDMITX_SET_TXPLL00EN_NUM							1
#define HDMITX_TXPLL00_01_TXPLL00EN_AUTO					0x00
#define HDMITX_TXPLL00_01_TXPLL00EN_MANUAL					0x80
#define HDMITX_TXPLL00_01_TXPLL00EN_MASK					0x00000080
//------------------------------------------------------------------------------
// hdmitx_set_tx0_sen()
#define HDMITX_SET_TX0_SEN_NUM								1
#define HDMITX_CKSELEN_TX0_SEN_AUTO							0x00
#define HDMITX_CKSELEN_TX0_SEN_MANUAL						0x04
#define HDMITX_CKSELEN_TX0_SEN_MASK							0x00000004
//------------------------------------------------------------------------------
// hdmitx_get_tx0_sen()
#define HDMITX_GET_TX0_SEN_NUM								1
//------------------------------------------------------------------------------
// hdmitx_set_tmds_pll()
#define HDMITX_SET_TMDS_PLL_TXPLL00_LEN						2
#define HDMITX_SET_TMDS_PLL_RXTXCORE0_LEN					7
#define HDMITX_SET_TMDS_PLL_MAX_LEN							HDMITX_SET_TMDS_PLL_RXTXCORE0_LEN
#define HDMITX_TMDS_PLL_OADDR_NUM							9
#define HDMITX_TMDS_PLL_NUM									2
//------------------------------------------------------------------------------
// hdmitx_select_hdmi_dvi()
#define HDMITX_SELECT_HDMI_DVI_NUM							1
#define HDMITX_SELECT_HDMI_DVI_TSYSCTRL_HDMI_HDMI			0x80
#define HDMITX_SELECT_HDMI_DVI_TSYSCTRL_HDMI_DVI			0x00
#define HDMITX_SELECT_HDMI_DVI_TSYSCTRL_MASK				0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_scramble_mode()
#define HDMITX_SET_SCRAMBLE_MODE_NUM						1
#define HDMITX_TXENCSET4_02_SCRMOD_ENA						0x30
#define HDMITX_TXENCSET4_02_SCRMOD_DIS						0x00
#define HDMITX_TXENCSET4_02_SCRVER_ENA						0x08
#define HDMITX_TXENCSET4_02_SCRVER_DIS						0x00
#define HDMITX_TXENCSET4_02_SCRON_ON						0x01
#define HDMITX_TXENCSET4_02_SCRON_OFF						0x00
#define HDMITX_TXENCSET4_02_MASK							0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_tmds_setting_enable()
#define HDMITX_SET_TMDS_ENABLE_NUM							2
#define HDMITX_SET_TMDS_ENABLE_TMDSTXP0_3_TXPRMXEN_MAN		0x80
#define HDMITX_SET_TMDS_ENABLE_TMDSTXP0_3_MASK				0x00000080
//------------------------------------------------------------------------------
// hdmitx_set_tmds_transmitter_setting()
#define HDMITX_SET_TMDS_TRANS_NUM							4
#define HDMITX_SET_TMDS_TRANS_JITTER_LEN					4
#define HDMITX_SET_TMDS_TRANS_PARAM_LEN						6
#define HDMITX_SET_TMDS_TRANS_MAX_LEN						HDMITX_SET_TMDS_TRANS_PARAM_LEN
#define HDMITX_SET_TMDS_TRANS_MASK							0x000000FF
#define HDMITX_TMDS_DATA_OADDR_NUM							14
#define HDMITX_TMDS_DATA_NUM								9
#define HDMITX_TMDS_TRANS_RATE_100GBPS						1000
#define HDMITX_TMDS_TRANS_RATE_155GBPS						1550
#define HDMITX_TMDS_TRANS_RATE_200GBPS						2000
#define HDMITX_TMDS_TRANS_RATE_225GBPS						2250
#define HDMITX_TMDS_TRANS_RATE_250GBPS						2500
#define HDMITX_TMDS_TRANS_RATE_300GBPS						3000
#define HDMITX_TMDS_TRANS_RATE_450GBPS						4500
#define HDMITX_TMDS_TRANS_RATE_500GBPS						5000
#define HDMITX_TMDS_TRANS_RATE_600GBPS						6000
#define HDMITX_TMDS_TRANS_RATE_340GBPS						3400
//------------------------------------------------------------------------------
// hdmitx_reset_tmds_encode()
#define HDMITX_RESET_TMDS_ENCODE_NUM						1
#define HDMITX_TXSYSCFG1_TP0SWPDWN_NOR						0x00
#define HDMITX_TXSYSCFG1_TP0ENCSRST_RST						0x02
#define HDMITX_TXSYSCFG1_TP0TMDSSRST_NOR					0x00
#define HDMITX_TXSYSCFG1_MASK								0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_avi_infoframe()
#define HDMITX_SET_AVI_INFOFRAME_LEN						5
//------------------------------------------------------------------------------
// hdmitx_avi_y()
#define HDMITX_AVI_Y_RGB									0x00
#define HDMITX_AVI_Y_YCBCR422								0x20
#define HDMITX_AVI_Y_YCBCR444								0x40
#define HDMITX_AVI_Y_YCBCR420								0x60
//------------------------------------------------------------------------------
// hdmitx_avi_a()
#define HDMITX_AVI_A_NO_ACTIVE_FORMAT						0x00
#define HDMITX_AVI_A_ACTIVE_FORMAT							0x10
//------------------------------------------------------------------------------
// hdmitx_avi_b()
#define HDMITX_AVI_B_NO_DATA								0x00
#define HDMITX_AVI_B_VERT_BAR								0x04
#define HDMITX_AVI_B_HORIZ_BAR								0x08
#define HDMITX_AVI_B_VERT_HORIZ_BAR							0x0C
//------------------------------------------------------------------------------
// hdmitx_avi_s()
#define HDMITX_AVI_S_NO_DATA								0x00
#define HDMITX_AVI_S_OVERSCANNED							0x01
#define HDMITX_AVI_S_UNDERSCANNED							0x02
//------------------------------------------------------------------------------
// hdmitx_avi_c()
#define HDMITX_AVI_C_NO_DATA								0x00
#define HDMITX_AVI_C_SMPTE_170M								0x40
#define HDMITX_AVI_C_ITU_R_BT709							0x80
#define HDMITX_AVI_C_EXTENDED								0xC0
//------------------------------------------------------------------------------
// hdmitx_avi_m()
#define HDMITX_AVI_M_NO_DATA								0x00
#define HDMITX_AVI_M_4_3									0x10
#define HDMITX_AVI_M_16_9									0x20
//------------------------------------------------------------------------------
// hdmitx_avi_r()
#define HDMITX_AVI_R_SAME_AS_M								0x08
#define HDMITX_AVI_R_4_3									0x09
#define HDMITX_AVI_R_16_9									0x0A
#define HDMITX_AVI_R_14_9									0x0B
//------------------------------------------------------------------------------
// hdmitx_avi_itc()
#define HDMITX_AVI_ITC_NO_DATA								0x00
//------------------------------------------------------------------------------
// hdmitx_avi_ec()
#define HDMITX_AVI_EC_RESERVED								0x70
#define HDMITX_AVI_EC_XVYCC601								0x00
#define HDMITX_AVI_EC_XVYCC709								0x10
#define HDMITX_AVI_EC_SYCC601								0x20
#define HDMITX_AVI_EC_ADOBEYCC601							0x30
#define HDMITX_AVI_EC_ADOBERGB								0x40
#define HDMITX_AVI_EC_BT2020_YCCBCCRC						0x50
#define HDMITX_AVI_EC_BT2020_RGB_YCBCR						0x60
//------------------------------------------------------------------------------
// hdmitx_avi_q()
#define HDMITX_AVI_Q_DEFAULT								0x00
#define HDMITX_AVI_Q_LIMITED_RANGE							0x04
#define HDMITX_AVI_Q_FULL_RANGE								0x08
#define HDMITX_VCDB_QS_NO_DATA								0x00
#define HDMITX_VCDB_QS_SELECTABLE							0x40
#define HDMITX_VCDB_QS_MASK									0x40
//------------------------------------------------------------------------------
// hdmitx_avi_sc()
#define HDMITX_AVI_SC_NO_KNOWN								0x00
#define HDMITX_AVI_SC_HORIZ_SCALED							0x01
#define HDMITX_AVI_SC_VERT_SCALED							0x02
#define HDMITX_AVI_SC_VERT_HORIZ_BAR						0x03
//------------------------------------------------------------------------------
// hdmitx_avi_vic()
#define HDMITX_AVI_VIC_SEE_HDMI_VS_INFOFRAME				0x00
#define HDMITX_AVI_VIC_NO_VIC								0x00
//------------------------------------------------------------------------------
// hdmitx_avi_yq()
#define HDMITX_AVI_YQ_LIMITED_RANGE							0x00
#define HDMITX_AVI_YQ_FULL_RANGE							0x40
//------------------------------------------------------------------------------
// hdmitx_avi_cn()
#define HDMITX_AVI_CN_GRAPHICS								0x00
#define HDMITX_AVI_CN_PHOTO									0x01
#define HDMITX_AVI_CN_CHINEMA								0x20
#define HDMITX_AVI_CN_GAME									0x30
//------------------------------------------------------------------------------
// hdmitx_avi_pr()
#define HDMITX_AVI_PR_NO_REPETITION							0x00
#define HDMITX_AVI_PR_2_TIMES								0x01
#define HDMITX_AVI_PR_4_TIMES								0x03
//------------------------------------------------------------------------------
// hdmitx_select_page_of_vsif()
#define HDMITX_SELECT_PAGE_OF_VSIF_NUM						1
#define HDMITX_SELECT_PAGE_OF_VSIF_MASK						0x000000FF
#define HDMITX_VSIF_PAGE_C0_HDMI_VS							0x00
#define HDMITX_VSIF_PAGE_C0_HDMI_FORUM_VS					0x10
#define HDMITX_VSIF_PAGE_A0_VS0								0x00
#define HDMITX_VSIF_PAGE_A0_VS1								0x01
#define HDMITX_VSIF_PAGE_A0_VS2								0x02
#define HDMITX_VSIF_PAGE_A0_VS3								0x03
//------------------------------------------------------------------------------
// hdmitx_set_hdmi_vic()
#define HDMITX_SET_HDMI_VIC_NUM								2
#define HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIDEO_FORMAT_NONE	0x00
#define HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIDEO_FORMAT_EXT	0x20
#define HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIDEO_FORMAT_MASK	0x000000FF
#define HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIC_MASK			0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_audio_infoframe()
#define HDMITX_SET_AUDIO_INFOFRAME_LEN						14
#define HDMITX_SET_AUDIO_INFOFRAME_CHKSUM_INIT				0x00
#define HDMITX_SET_AUDIO_INFOFRAME_CT_RSH					0x00
#define HDMITX_SET_AUDIO_INFOFRAME_CC_MASK					0x07
#define HDMITX_SET_AUDIO_INFOFRAME_SF_MASK					0x1C
#define HDMITX_SET_AUDIO_INFOFRAME_SS_MASK					0x03
#define HDMITX_SET_AUDIO_INFOFRAME_REF_STREAM_HDR			0x00
#define HDMITX_SET_AUDIO_INFOFRAME_DM_INH_PERMITTED			0x00
#define HDMITX_SET_AUDIO_INFOFRAME_LSV0_BIT					0x03
#define HDMITX_SET_AUDIO_INFOFRAME_LSV0_3_MASK				0x78
#define HDMITX_SET_AUDIO_INFOFRAME_LFEPBL0_1_UNKNOWN		0x00
#define HDMITX_SET_AUDIO_INFOFRAME_NONE						0x00
//------------------------------------------------------------------------------
// hdmitx_update_infoframe()
#define HDMITX_UPDATE_INFOFRAME_NUM							1
#define HDMITX_UPDATE_INFOFRAME_PKTUPD_00_ALL_CLEAR			0x00
#define HDMITX_UPDATE_INFOFRAME_PKTUPD_00_MASK				0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_720p_clock_selector()
#define HDMITX_SET_720P_CLOCK_SELECTOR_NUM					2
#define HDMITX_TXCORE0_02_44								0x44
#define HDMITX_TXCORE0_02_MASK								0x000000FF
#define HDMITX_TXCORE0_04_04								0x04
#define HDMITX_TXCORE0_04_MASK								0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_720p()
#define HDMITX_SET_720P_PRE_NUM								7
#define HDMITX_SET_720P_POST_NUM							2
#define HDMITX_SET_720P_MAX_NUM								HDMITX_SET_720P_PRE_NUM
#define HDMITX_PADVINCTRL1_00_DEF							0x00
#define HDMITX_PADVINCTRL1_00_10							0x10
#define HDMITX_PADVINCTRL1_00_MASK							0x000000FF
#define HDMITX_SWMODE4_01_DEF								0x00
#define HDMITX_SWMODE4_01_01								0x01
#define HDMITX_SWMODE4_01_MASK								0x00000001
#define HDMITX_INREPSET_DEF									0x00
#define HDMITX_INREPSET_01									0x01
#define HDMITX_INREPSET_MASK								0x000000FF
#define HDMITX_SYNCCNT_00									0x00
#define HDMITX_SYNCCNT_MASK									0x000000FF
#define HDMITX_VIDEOFMT_00_10								0x10
#define HDMITX_VIDEOFMT_00_MASK								0x000000FF
#define HDMITX_VIDEOFMT_01_90								0x90
#define HDMITX_VIDEOFMT_01_MASK								0x000000FF
#define HDMITX_PCFMT_PCFMT_DEF								0x00
#define HDMITX_PCFMT_PCFMT_MASK								0x000000FF
#define HDMITX_VIDEOFMT_01_VOFMTSEL_USE_VOFMT				0x80
//------------------------------------------------------------------------------
// hdmitx_reset_720p()
#define HDMITX_RESET_720P_NUM								2
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_clock_gate()
#define HDMITX_SET_4K_480P_CLOCK_GATE_1E57_LEN				6
#define HDMITX_SET_4K_480P_CLOCK_GATE_1E60_LEN				3
#define HDMITX_SET_4K_480P_CLOCK_GATE_MAX_LEN				HDMITX_SET_4K_480P_CLOCK_GATE_1E57_LEN
#define HDMITX_SET_4K_480P_CLOCK_GATE_V_NUM					(HDMITX_SET_4K_480P_CLOCK_GATE_1E57_LEN \
																+ HDMITX_SET_4K_480P_CLOCK_GATE_1E60_LEN)
#define HDMITX_VIDEOIF_1E57_ADDR							0x00001E57
#define HDMITX_1E57_40										0x40
#define HDMITX_1E58_81										0x81
#define HDMITX_1E59_08										0x08
#define HDMITX_1E5A_07										0x07
#define HDMITX_1E5B_00										0x00
#define HDMITX_1E5C_00										0x00
#define HDMITX_1E60_80										0x80
#define HDMITX_1E61_10										0x10
#define HDMITX_1E62_06										0x06
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_clock()
#define HDMITX_SET_4K_480P_CLOCK_NUM						3
#define HDMITX_SET_4K_480P_CLOCK_V_NUM						(HDMITX_SET_TX0_SEN_NUM \
																+ HDMITX_SET_4K_480P_CLOCK_NUM)
#define HDMITX_TXCORE0_03_44								0x44
#define HDMITX_TXCORE0_03_MASK								0x000000FF
#define HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_480P_ENA		0x20
#define HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_MASK			0x00000020
#define HDMITX_VIDEOIF_1E3E_ADDR							0x00001E3E
#define HDMITX_1E3E_18										0x18
#define HDMITX_1E3E_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_1001_1000_clock()
#define HDMITX_SET_4K_480P_1001_1000_NUM					2
#define HDMITX_SET_4K_480P_1001_1000_1E40_LEN				6
#define HDMITX_SET_4K_480P_1001_1000_1E65_LEN				2
#define HDMITX_SET_4K_480P_1001_1000_MAX_LEN				HDMITX_SET_4K_480P_1001_1000_1E40_LEN
#define HDMITX_SET_4K_480P_1001_1000_V_NUM					(HDMITX_SET_4K_480P_1001_1000_1E40_LEN \
																+ HDMITX_SET_4K_480P_1001_1000_1E65_LEN)
#define HDMITX_RXPLL0_01_RXPLL0EN_MANUAL					0x80
#define HDMITX_RXPLL0_01_MASK								0x000000FF
#define HMDITX_RXPLL00_03_RXPLL00EN_MANUAL					0x08
#define HMDITX_RXPLL00_03_MASK								0x000000FF
#define HDMITX_1E40_83										0x83
#define HDMITX_1E41_98										0x98
#define HDMITX_1E42_D1										0xD1
#define HDMITX_1E43_57										0x57
#define HDMITX_1E44_E9										0xE9
#define HDMITX_1E45_0B										0x0B
#define HDMITX_1E65_18										0x18
#define HDMITX_1E66_08										0x08
//------------------------------------------------------------------------------
// hdmitx_get_4k_480p_preset()
#define HDMITX_GET_4K_480P_PRESET_NUM						4
#define HDMITX_PKTENA_MASK									0x000000FF
#define HDMITX_INFENA_MASK									0x000000FF
#define HDMITX_VMUTECNT_MASK								0x000000FF
#define HDMITX_TXACTL1_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_preset()
#define HDMITX_SET_4K_480P_PRESET_NUM						4
#define HDMITX_PKTENA_DEF									0x00
#define HDMITX_INFENA_DEF									0x00
#define HDMITX_VMUTECNT_DUMMY								0xD0
#define HDMITX_TXACTL1_DEF									0x04
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_crc()
#define HDMITX_SET_4K_480P_CRC_NUM							1
#define HDMITX_ENCDBG_06_CRC_ENA_ENA						0x80
#define HDMITX_ENCDBG_06_CRC_ENA_DIS						0x00
#define HDMITX_ENCDBG_06_MASK								0x000000FF
#define HDMITX_SET_4K_480P_CRC_ON							1
#define HDMITX_SET_4K_480P_CRC_OFF							0
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_crc_expected_value()
#define HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN			6
//------------------------------------------------------------------------------
// hdmitx_set_4k_480p_sync_delay()
#define HDMITX_SET_4K_480P_SYNC_DELAY_NUM					2
#define HDMITX_GLUECTL2_0_SY_DLY_DEF						0x00
#define HDMITX_GLUECTL2_0_SY_DLY_80							0x80
#define HDMITX_GLUECTL2_0_SY_DLY_81							0x81
#define HDMITX_GLUECTL2_0_MASK								0x000000FF
#define HDMITX_GLUECTL3_2_SDMODE_DEF						0x00
#define HDMITX_GLUECTL3_2_SDMODE_REG_SETUP					0x08
#define HDMITX_GLUECTL3_2_MASK								0x000000FF
//------------------------------------------------------------------------------
// hdmitx_reset_4k_480p_1001_1000()
#define HDMITX_RESET_4K_480P_1001_1000_NUM					2
#define HDMITX_1E42_D0										0xD0
#define HDMITX_RXPLL00_00_MASK								0x000000FF
//------------------------------------------------------------------------------
// hdmitx_get_4k_480p_crc_error()
#define HDMITX_GET_4K_480P_CRC_ERROR_NUM					1
#define HDMITX_CRCMON_06_CRC_ERROR_NO_ERROR					0x00
#define HDMITX_CRCMON_06_CRC_ERROR_MASK						0x00000001
//------------------------------------------------------------------------------
// hdmitx_ctrl_4k_480p_ctrl_crc_expected_value()
#define HDMITX_CRC_DETECTED_CNT								(HDMITX_4K_480P_OUTPUT_SEQ_DETECT_RETRY_MAX \
															* HDMITX_4K_480P_OUTPUT_SEQ_CRC_CHECK_RETRY_MAX)
//------------------------------------------------------------------------------
// hdmitx_4k_480p_output_seq()
#define HDMITX_4K_480P_OUTPUT_SEQ_DETECT_RETRY_MAX			5
#define HDMITX_4K_480P_OUTPUT_SEQ_1001_1000_RETRY_MAX		1000
#define HDMITX_4K_480P_OUTPUT_SEQ_CRC_CHECK_RETRY_MAX		3
//------------------------------------------------------------------------------
// hdmitx_set_2k_480p_clock1()
#define HDMITX_SET_2K_480P_CLOCK1_NUM						1
#define HDMITX_SET_2K_480P_CLOCK1_1E58_LEN					3
#define HDMITX_SET_2K_480P_CLOCK1_V_NUM						(HDMITX_SET_TX0_SEN_NUM \
																+ HDMITX_SET_2K_480P_CLOCK1_NUM \
																+ HDMITX_SET_2K_480P_CLOCK1_1E58_LEN)
#define HDMITX_PADEN_MASK									0x000000FF
#define HDMITX_VIDEOIF_1E58_ADDR							0x00001E58
//------------------------------------------------------------------------------
// hdmitx_set_2k_480p_1001_1000_clock()
#define HDMITX_SET_2K_480P_1001_1000_V_NUM					HDMITX_SET_4K_480P_1001_1000_V_NUM
#define HDMITX_1E66_32										0x32
//------------------------------------------------------------------------------
// hdmitx_set_2k_480p_dc_clock()
#define HDMITX_SET_2K_480P_DC_CLOCK_NUM						7
#define HDMITX_SET_2K_480P_DC_CLOCK_V_NUM					HDMITX_SET_2K_480P_DC_CLOCK_NUM
#define HDMITX_1E3E_07										0x07
#define HDMITX_1E6D_20										0x20
#define HDMITX_1E6D_MASK									0x000000FF
#define HDMITX_1E6B_4C										0x4C
#define HDMITX_1E6B_MASK									0x000000FF
#define HDMITX_1E47_20										0x20
#define HDMITX_1E47_MASK									0x000000FF
#define HDMITX_1E6F_02										0x02
#define HDMITX_1E6F_MASK									0x000000FF
#define HDMITX_1E70_92										0x92
#define HDMITX_1E70_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_2k_480p_clock2()
#define HDMITX_SET_2K_480P_CLOCK2_NUM						2
#define HDMITX_SET_2K_480P_CLOCK2_V_NUM						HDMITX_SET_2K_480P_CLOCK2_NUM
#define HDMITX_1E3E_1F										0x1F
#define HDMITX_1E6E_CC										0xCC
#define HDMITX_1E6E_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_2k_480p_pll01()
#define HDMITX_SET_2K_480P_PLL01_NUM						1
#define HDMITX_SET_2K_480P_PLL01_1E48_LEN					2
#define HDMITX_SET_2K_480P_PLL01_V_NUM						HDMITX_SET_2K_480P_PLL01_1E48_LEN
#define HDMITX_TXPLL01_01_TXPLL01EN							0x80
#define HDMITX_1E48_83										0x83
#define HDMITX_1E49_90										0x90
#define HDMITX_1E49_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmitx_init_reg_stack()
#define HDMITX_REG_STACK_INIT_VAL							0x00
//------------------------------------------------------------------------------
// hdmitx_sg_setting()
#define HDMITX_SG_SETTING_NUM								4
#define HDMITX_SCLKSEL_NOR									0x00
#define HDMITX_SCLKSEL_SYSCLK								0x04
#define HDMITX_SCLKSEL_MASK									0x04
#define HDMITX_TCLKSET0_TSTGEN0_NOR							0x00
#define HDMITX_TCLKSET0_TSTGEN0_SYSCLK_16_16				0x01
#define HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_8					0x02
#define HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_4					0x03
#define HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_2					0x04
#define HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_1					0x05
#define HDMITX_TCLKSET0_TSTGEN0_MASK						0x0F
#define HDMITX_SWMODE4_01_OSDVSRC0_RXCC0					0x00
#define HDMITX_SWMODE4_01_OSDVSRC0_TXVIDEO0					0x01
#define HDMITX_SWMODE4_01_OSDVSRC0_MASK						0x01
#define HDMITX_VMUTECNT_DUMON_NOR							0x00
#define HDMITX_VMUTECNT_DUMON_DUMMY							0x40
#define HDMITX_VMUTECNT_DUMON_MASK							0x40
#define HDMITX_VMUTECNT_VMUTE_NOR							0x00
#define HDMITX_VMUTECNT_VMUTE_ASYNC_BLANK					0x01
#define HDMITX_VMUTECNT_VMUTE_BLANK							0x02
#define HDMITX_VMUTECNT_VMUTE_RAMP_A						0x04
#define HDMITX_VMUTECNT_VMUTE_RAMP_B						0x08
#define HDMITX_VMUTECNT_VMUTE_TOGGLE						0x0C
#define HDMITX_VMUTECNT_VMUTE_COLOR_BAR_RGB					0x0A
#define HDMITX_VMUTECNT_VMUTE_COLOR_BAR_444					0x0E
#define HDMITX_VMUTECNT_VMUTE_MASK							0x0F
//------------------------------------------------------------------------------
// hdmitx_set_sg_cform()
#define HDMITX_SET_SG_CFORM_NUM								1
#define HDMITX_SET_SG_CFORM_CFMT_IOCFMT_MASK				0x0F
#define HDMITX_SET_SG_CFORM_CFMT_ICFMT_BIT					0x04
#define HDMITX_SET_SG_CFORM_CFMT_MASK						0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_audio_format()
#define HDMITX_SET_AUDIO_FORMAT_NUM							5
#define HDMITX_TXACTL2_ENDMODE_MSB							0x00
#define HDMITX_TXACTL2_ENDMODE_LSB							0x80
#define HDMITX_TXACTL2_POLLR_LOW							0x00
#define HDMITX_TXACTL2_POLLR_HIGH							0x40
#define HDMITX_TXACTL2_ADIIS_DIS							0x00
#define HDMITX_TXACTL2_ADIIS_ENA							0x20
#define HDMITX_TXACTL2_ADFB_LEFT							0x10
#define HDMITX_TXACTL2_ADFB_RIGHT							0x00
#define HDMITX_TXACTL2_AUTOFS_TXFS							0x08
#define HDMITX_TXACTL2_CHSEL_CHNUM							0x02
#define HDMITX_TXACTL2_MASK									0x000000FF
#define HDMITX_TXACTL4_SELMCK_INTERNAL						0x80
#define HDMITX_TXACTL4_SELMCK_MASK							0x00000080
#define HDMITX_TXACTL5_HBRON_DIS							0x00
#define HDMITX_TXACTL5_HBRON_ENA							0x20
#define HDMITX_TXACTL5_HBRON_MASK							0x00000020
#define HDMITX_TXACTL5_NSEL_AUTO							0x00
#define HDMITX_TXACTL5_NSEL_MASK							0x00000004
#define HDMITX_TXACTL7_CHNUM_RSV							0x00
#define HDMITX_TXACTL7_CHNUM_2CH							0x01
#define HDMITX_TXACTL7_CHNUM_3CH							0x02
#define HDMITX_TXACTL7_CHNUM_4CH							0x03
#define HDMITX_TXACTL7_CHNUM_5CH							0x04
#define HDMITX_TXACTL7_CHNUM_6CH							0x05
#define HDMITX_TXACTL7_CHNUM_7CH							0x06
#define HDMITX_TXACTL7_CHNUM_8CH							0x07
#define HDMITX_TXACTL7_CHNUM_MASK							0x00000007
#define HDMITX_TXACTL8_ODSMPMODE_2							0x00
#define HDMITX_TXACTL8_ODSMPMODE_4							0x80
#define HDMITX_TXACTL8_ODSMPMODE_MASK						0x00000080
#define HDMITX_TXACTL8_ODSMPSEL_LPCM_DOWN					0x00
#define HDMITX_TXACTL8_ODSMPSEL_LPCM_OVER					0x40
#define HDMITX_TXACTL8_ODSMPSEL_MASK						0x00000040
#define HDMITX_TXACTL8_ODSMPON_DIS							0x00
#define HDMITX_TXACTL8_ODSMPON_ENA							0x20
#define HDMITX_TXACTL8_ODSMPON_MASK							0x00000020
#define HDMITX_TXACTL8_ADBITIN_16BIT						0x00
#define HDMITX_TXACTL8_ADBITIN_20BIT						0x04
#define HDMITX_TXACTL8_ADBITIN_24BIT						0x06
#define HDMITX_TXACTL8_ADBITIN_MASK							0x0000000E
//------------------------------------------------------------------------------
// hdmitx_audio_mute()
#define HDMITX_AUDIO_MUTE_NUM								1
#define HDMITX_AUDIO_MUTE_TXACTL1_ACREN_DIS					0x00
#define HDMITX_AUDIO_MUTE_TXACTL1_ACREN_ENA					0x80
#define HDMITX_AUDIO_MUTE_TXACTL1_ASPEN_DIS					0x00
#define HDMITX_AUDIO_MUTE_TXACTL1_ASPEN_ENA					0x40
#define HDMITX_AUDIO_MUTE_TXACTL1_TXMUTE_MUTE				0x04
#define HDMITX_AUDIO_MUTE_TXACTL1_TXMUTE_UNMUTE				0x00
#define HDMITX_AUDIO_MUTE_TXACTL1_MASK						0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_spd_infoframe()
#define HDMITX_SET_SPD_INFOFRAME_LEN						29
#define HDMITX_SET_SPD_INFOFRAME_CHKSUM_INIT				0x00
#define HDMITX_SET_SPD_INFOFRAME_DIS						0xFF
//------------------------------------------------------------------------------
// hdmitx_set_hdr_infoframe()
#define HDMITX_SET_HDR_INFOFRAME_LEN						30
#define HDMITX_SET_HDR_INFOFRAME_CHKSUM_INIT				0x00
#define HDMITX_SET_HDR_INFOFRAME_MASK						0x000000FF
#define HDMITX_SET_HDR_INFOFRAME_8BIT_MAX					0x00000100
#define HDMITX_SET_HDR_INFOFRAME_DIS						0xFF
//------------------------------------------------------------------------------
// hdmitx_get_hpd_status()
#define HDMITX_HPD_GET_STATUS_NUM							1
#define HDMITX_HPD_GET_STATUS_TXSYSCFG12_MASK				0x000000FF
//------------------------------------------------------------------------------
// hdmitx_set_hpd_intr()
#define HDMITX_SET_HPD_INTR_NUM								1
#define HDMITX_INTREG10_MASK								0x000000FF
//------------------------------------------------------------------------------
// hdmitx_get_hpd_intr()
#define HDMITX_GET_HPD_INTR_NUM								1
//------------------------------------------------------------------------------
// hdmitx_set_txmstctl()
#define HDMITX_SET_TXMSTCTL_NUM								1
#define HDMITX_TXMSTCTL_MSTCBMODE_ALL_ST0					0x00
#define HDMITX_TXMSTCTL_MSTCBMODE_MASK						0x00000004
#define HDMITX_TXMSTCTL_CBPAGE_ST0							0x00
#define HDMITX_TXMSTCTL_CBPAGE_MASK							0x00000003
//------------------------------------------------------------------------------
// hdmitx_set_audio_cbit()
#define HDMITX_SET_AUDIO_CBIT_LEN							8
#define HDMITX_TXCBIT_MODE_0								0x00
#define HDMITX_TXCBIT_EMPHASIS_2CH_WO_PRE					0x00
#define HDMITX_TXCBIT_COPYRIGHT								0x00
#define HDMITX_TXCBIT_LPCM_LPCM								0x00
#define HDMITX_TXCBIT_LPCM_OTHER							0x02
#define HDMITX_TXCBIT_CBIT_0_CONSUMER						0x00
#define HDMITX_TXCBIT_CATEGORY_DEF							0x00
#define HDMITX_TXCBIT_SOURCE_NUM_DEF						0x00
#define HDMITX_TXCBIT_CH_NUMBER_DEF							0x00
#define HDMITX_TXCBIT_CLKACCR_LV2							0x00
#define HDMITX_TXCBIT_TXFS_EXT_DEF							0x00
#define HDMITX_TXCBIT_ORGFS_DEF								0x00
#define HDMITX_TXCBIT_5_DEF									0x00
#define HDMITX_TXCBIT_6_DEF									0x00
#define HDMITX_TXCBIT_7_DEF									0x00
//------------------------------------------------------------------------------
#define HDMITX_INFOFRAME_TYPE								0x80
//------------------------------------------------------------------------------
#define INFOFRAME_VERSION_SPD								0x01
#define INFOFRAME_VERSION_HDR								0x01
#define INFOFRAME_VERSION_AVI								0x02
#define INFOFRAME_VERSION_AUDIO								0x01
#define INFOFRAME_LENGTH_SPD								0x19
#define INFOFRAME_LENGTH_HDR								0x1A
#define INFOFRAME_LENGTH_AVI								0x0D
#define INFOFRAME_LENGTH_AUDIO								0x0A
//------------------------------------------------------------------------------
// SLEEP() time
#define HDMITX_SLEEP_TIME_17MSEC							17
#define HDMITX_SLEEP_TIME_80MSEC							80
#define HDMITX_SLEEP_TIME_2MSEC								2
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL											0
#endif // NULL
//------------------------------------------------------------------------------
// error check & if (c) is a error, return (c)
#define CHECK_ERROR(c) {						\
	if ((c) != ERR_OK) {						\
		return (c);								\
	}											\
}
// register access setting
#define SET_ACCESS(a, p, c, n) {				\
	(a).p_port = (p);							\
	(a).p_com = (c);							\
	(a).count = (n);							\
}
// command setting
#define SET_COMMAND(c, i, a, d, m) {			\
	(c)[(i)].addr = (a);						\
	(c)[(i)].data = (d);						\
	(c)[(i)].mask = (m);						\
}
// resigter block access setting
#define SET_BLOCK_ACCESS(b, p, a, l, d) {		\
	(b).p_port = (p);							\
	(b).addr = (a);								\
	(b).length = (l);							\
	(b).p_data = (d);							\
}
// block data setting
#define SET_BLOCK_DATA(b, i, d) {				\
	(b)[(i)] = (d);								\
}
//------------------------------------------------------------------------------
const uint8_t tmds_pll[HDMITX_TMDS_PLL_NUM][HDMITX_TMDS_PLL_OADDR_NUM] = {
//0x1E:
//   0x46  0x47  0x6A  0x6B  0x6C  0x6D  0x6E  0x6F  0x70
	{0x83, 0x94, 0x24, 0xE4, 0x30, 0x00, 0x44, 0x01, 0x90},
	{0x83, 0x98, 0x24, 0xE4, 0x30, 0x00, 0x44, 0x01, 0x90}
};
//------------------------------------------------------------------------------
const uint8_t tmds_data[HDMITX_TMDS_DATA_NUM][HDMITX_TMDS_DATA_OADDR_NUM] = {
//0x1E:
//   0x00  0x02  0x04  0x08  0x0A  0x0B  0x0C  0x0D  0x0E  0x0F  0x1C  0x1D  0x1E  0x1F
	{0x66, 0x88, 0x00, 0x01, 0x00, 0xa2, 0x05, 0x02, 0x10, 0x90, 0xf4, 0x31, 0xf4, 0x21},
	{0x66, 0x88, 0x00, 0x01, 0x00, 0xa2, 0x05, 0x02, 0x10, 0x90, 0xe8, 0x33, 0xf4, 0x21},
	{0x66, 0x8d, 0x00, 0x01, 0x00, 0xca, 0x07, 0x09, 0x10, 0x90, 0xd0, 0x37, 0xf4, 0x21},
	{0x66, 0x8d, 0x00, 0x01, 0x01, 0xca, 0x07, 0x09, 0x1a, 0x90, 0xd0, 0x37, 0xf4, 0x21},
	{0x66, 0x8d, 0x00, 0x01, 0x01, 0xcc, 0x06, 0x09, 0x1a, 0x90, 0xd0, 0x37, 0xf4, 0x21},
	{0x66, 0x8d, 0x00, 0x01, 0x01, 0xca, 0x05, 0x89, 0x1a, 0x90, 0xd0, 0x37, 0xf4, 0x21},
	{0x66, 0x8e, 0x00, 0x00, 0x11, 0x8a, 0x05, 0x06, 0x1b, 0x90, 0xd0, 0x37, 0x90, 0x11},
	{0x66, 0x8e, 0x00, 0x00, 0x11, 0x8c, 0x04, 0x06, 0x1b, 0x90, 0xd0, 0x37, 0x90, 0x11},
	{0x66, 0x8e, 0x00, 0x00, 0x11, 0x8a, 0x04, 0x87, 0x1b, 0x90, 0xbc, 0x37, 0x4a, 0x11}
};
//------------------------------------------------------------------------------
typedef struct _hdmitx_expected_crc {
	uint8_t detected_cnt;
	uint8_t detected_crc[HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN];
} hdmitx_expected_crc;
//------------------------------------------------------------------------------
typedef enum _hdmitx_audio_ch {
	HDMITX_AUDIO_CH0 = 0,
	HDMITX_AUDIO_CH1,
	HDMITX_AUDIO_CH2,
	HDMITX_AUDIO_CH3
} hdmitx_audio_ch;
//------------------------------------------------------------------------------
typedef enum _infoframe_type {
	INFOFRAME_TYPE_RESERVED = 0,
	INFOFRAME_TYPE_VENDOR_SPECIFIC,
	INFOFRAME_TYPE_AVI,
	INFOFRAME_TYPE_SPD,
	INFOFRAME_TYPE_AUDIO,
	INFOFRAME_TYPE_MPEG,
	INFOFRAME_TYPE_NTSC_VBI,
	INFOFRAME_TYPE_DRM
} infoframe_type;
//------------------------------------------------------------------------------
typedef enum _hdmitx_tmds_pll_index {
	HDMITX_PLL_IDX_10BIT = 0,
	HDMITX_PLL_IDX_12BIT = 1
} hdmitx_tmds_pll_index;
//------------------------------------------------------------------------------
typedef enum _hdmitx_tmds_pll_index_oaddr {
	HDMITX_OADDR_IDX_0X46 = 0,
	HDMITX_OADDR_IDX_0X47 = 1,
	HDMITX_OADDR_IDX_0X6A = 2,
	HDMITX_OADDR_IDX_0X6B = 3,
	HDMITX_OADDR_IDX_0X6C = 4,
	HDMITX_OADDR_IDX_0X6D = 5,
	HDMITX_OADDR_IDX_0X6E = 6,
	HDMITX_OADDR_IDX_0X6F = 7,
	HDMITX_OADDR_IDX_0X70 = 8
} hdmitx_tmds_pll_index_oaddr;
//------------------------------------------------------------------------------
typedef enum _hdmitx_tmds_data_index {
	HDMITX_RATE_IDX_100GBPS = 0,
	HDMITX_RATE_IDX_155GBPS = 1,
	HDMITX_RATE_IDX_200GBPS = 2,
	HDMITX_RATE_IDX_225GBPS = 3,
	HDMITX_RATE_IDX_250GBPS = 4,
	HDMITX_RATE_IDX_300GBPS = 5,
	HDMITX_RATE_IDX_450GBPS = 6,
	HDMITX_RATE_IDX_500GBPS = 7,
	HDMITX_RATE_IDX_600GBPS = 8
} hdmitx_tmds_data_index;
//------------------------------------------------------------------------------
typedef enum _hdmitx_tmds_data_index_oaddr {
	HDMITX_OADDR_IDX_0X00 = 0,
	HDMITX_OADDR_IDX_0X02 = 1,
	HDMITX_OADDR_IDX_0X04 = 2,
	HDMITX_OADDR_IDX_0X08 = 3,
	HDMITX_OADDR_IDX_0X0A = 4,
	HDMITX_OADDR_IDX_0X0B = 5,
	HDMITX_OADDR_IDX_0X0C = 6,
	HDMITX_OADDR_IDX_0X0D = 7,
	HDMITX_OADDR_IDX_0X0E = 8,
	HDMITX_OADDR_IDX_0X0F = 9,
	HDMITX_OADDR_IDX_0X1C = 10,
	HDMITX_OADDR_IDX_0X1D = 11,
	HDMITX_OADDR_IDX_0X1E = 12,
	HDMITX_OADDR_IDX_0X1F = 13
} hdmitx_tmds_data_index_oaddr;
//------------------------------------------------------------------------------
typedef enum _hdmitx_pktupd {
	HDMITX_PKTUPD_NONE    = 0x00,
	HDMITX_PKTUPD_FREEUPD = 0x01,	// free packet update (including HDR)
	HDMITX_PKTUPD_HVSUPD  = 0x04,	// HDMI vendor specific infoframe update
	HDMITX_PKTUPD_MPEGUPD = 0x08,	// MPEG source infoframe update
	HDMITX_PKTUPD_AIUPD   = 0x10,	// audio infoframe update
	HDMITX_PKTUPD_SPDUPD  = 0x20,	// SPD infoframe update
	HDMITX_PKTUPD_AVIUPD  = 0x40,	// AVI infoframe update
	HDMITX_PKTUPD_HFVSUPD = 0x80	// HDMI forum vendor specific infoframe update
} hdmitx_pktupd;
//------------------------------------------------------------------------------
typedef enum _hdmitx_infena {
	HDMITX_INFENA_NONE   = 0x00,
	HDMITX_INFENA_FREEEN = 0x01,	// free packet enable (including HDR)
	HDMITX_INFENA_HVSEN  = 0x04,	// HDMI vendor specific infoframe packet enable
	HDMITX_INFENA_MPEGEN = 0x08,	// MPEG source infoframe packet enable
	HDMITX_INFENA_AIEN   = 0x10,	// audio infoframe packet enable
	HDMITX_INFENA_SPDEN  = 0x20,	// SPD infoframe packet enable
	HDMITX_INFENA_AVIEN  = 0x40,	// AVI packet enable
	HDMITX_INFENA_HFVSEN = 0x80		// HDMI forum vendor specific infoframe packet enable
} hdmitx_infena;
//------------------------------------------------------------------------------
typedef enum _hdmitx_pktena {
	HDMITX_PKTENA_NONE    = 0x00,
	HDMITX_PKTENA_NULEN   = 0x01,	// NULL packet transmit
	HDMITX_PKTENA_GAMUTEN = 0x02,	// gamut metadata packet enable
	HDMITX_PKTENA_IRSC2EN = 0x04,	// international standard recording code packet 2 enable
	HDMITX_PKTENA_IRSC1EN = 0x08,	// international standard recording code packet 1 enable
	HDMITX_PKTENA_ACPEN   = 0x10,	// audio content protection packet enable
	HDMITX_PKTENA_GCPEN   = 0x20,	// general control packet enable
	HDMITX_PKTENA_AMETAEN = 0x40	// audio metadata packet enable
} hdmitx_pktena;
//------------------------------------------------------------------------------
typedef enum _hdmitx_hdmi_vic {
	HDMITX_HDMI_VIC_NONE       = 0x00,
	HDMITX_HDMI_VIC_4K_30HZ    = 0x01,
	HDMITX_HDMI_VIC_4K_25HZ    = 0x02,
	HDMITX_HDMI_VIC_4K_24HZ    = 0x03,
	HDMITX_HDMI_VIC_4K_SMPTE_24HZ = 0x04
} hdmitx_hdmi_vic;
//------------------------------------------------------------------------------
typedef enum _hdmitx_ctrl_req {
	HDMITX_CTRL_REQ_GET,
	HDMITX_CTRL_REQ_SET,
	HDMITX_CTRL_REQ_MAX
} hdmitx_ctrl_req;
//------------------------------------------------------------------------------
typedef enum _hdmitx_ctrl_crc {
	HDMITX_CTRL_CRC_GET,
	HDMITX_CTRL_CRC_SET,
	HDMITX_CTRL_CRC_CLEAR,
	HDMITX_CTRL_CRC_MAX
} hdmitx_ctrl_crc;
//------------------------------------------------------------------------------
typedef enum _hdmitx_ctrl_val {
	HDMITX_CTRL_VAL_GET,
	HDMITX_CTRL_VAL_SET,
	HDMITX_CTRL_VAL_MAX
} hdmitx_ctrl_val;
//------------------------------------------------------------------------------
static uint32_t hdmitx_select_audio_ch(system_port port, hdmitx_audio_ch ch);
static uint32_t hdmitx_init_audio_trans_setting(system_port port);
static uint32_t hdmitx_set_infena(system_port port, uint8_t infena, uint32_t mask);
static uint32_t hdmitx_set_pktena(system_port port, uint8_t pktena, uint32_t mask);
static uint32_t hdmitx_set_tmds_ch_power(system_port port, uint8_t onoff);
static uint32_t hdmitx_get_tmds_trans_rate(system_video_format vf, system_color_format cf,
											system_deep_color dc, uint32_t *p_rate);
static uint32_t hdmitx_set_scdc_source_version(system_port port, uint8_t bit7_0);
static uint32_t hdmitx_set_scdc_tmds_config(system_port port, uint8_t bit7_0);
static uint32_t hdmitx_check_scdc_access(system_port port);
static uint32_t hdmitx_set_scdcs(hdmitx_video *p_video);
static uint32_t hdmitx_set_txpll00en(system_port port, uint8_t en);
static uint32_t hdmitx_set_tx0_sen(system_port port, uint8_t en);
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H27BJ2)
static uint32_t hdmitx_get_tx0_sen(system_port port, uint8_t *p_en);
#endif
static uint32_t hdmitx_set_tmds_pll(system_port port, hdmitx_tmds_pll_index idx);
static uint32_t hdmitx_set_tmds(hdmitx_video *p_video, uint8_t *p_tx0_sen);
static uint32_t hdmitx_select_hdmi_dvi(system_port port, hdmitx_dvi_mode mode);
static uint32_t hdmitx_set_scramble_mode(hdmitx_video *p_video);
static uint32_t hdmitx_disable_scramble_mode(hdmitx_video *p_video);
static uint32_t hdmitx_set_tmds_setting_enable(system_port port);
static uint32_t hdmitx_set_tmds_transmitter_setting(hdmitx_video *p_video);
static uint32_t hdmitx_reset_tmds_encode(system_port port);
static uint32_t hdmitx_avi_y(hdmitx_video *p_video, uint8_t *y);
static uint32_t hdmitx_avi_a(hdmitx_video *p_video, uint8_t *a);
static uint32_t hdmitx_avi_b(hdmitx_video *p_video, uint8_t *b);
static uint32_t hdmitx_avi_s(hdmitx_video *p_video, uint8_t *s);
static uint32_t hdmitx_avi_c(hdmitx_video *p_video, uint8_t *c);
static uint32_t hdmitx_avi_m(hdmitx_video *p_video, uint8_t *m);
static uint32_t hdmitx_avi_r(hdmitx_video *p_video, uint8_t *r);
static uint32_t hdmitx_avi_itc(hdmitx_video *p_video, uint8_t *itc);
static uint32_t hdmitx_avi_ec(hdmitx_video *p_video, uint8_t *ec);
static uint32_t hdmitx_avi_q(hdmitx_video *p_video, uint8_t *q);
static uint32_t hdmitx_avi_sc(hdmitx_video *p_video, uint8_t *sc);
static uint32_t hdmitx_avi_vic(hdmitx_video *p_video, uint8_t *vic);
static uint32_t hdmitx_avi_yq(hdmitx_video *p_video, uint8_t *yq);
static uint32_t hdmitx_avi_cn(hdmitx_video *p_video, uint8_t *cn);
static uint32_t hdmitx_avi_pr(hdmitx_video *p_video, uint8_t *pr);
static uint32_t hdmitx_set_avi_infoframe(hdmitx_video *p_video);
static uint32_t hdmitx_disable_avi_infoframe(hdmitx_video *p_video);
static uint32_t hdmitx_select_page_of_vsif(system_port port, uint8_t page);
static uint32_t hdmitx_set_hdmi_vic(system_port port, uint8_t hdmi_vic);
static uint32_t hdmitx_set_hdmi_vs_infoframe(hdmitx_video *p_video);
static uint32_t hdmitx_disable_hdmi_vs_infoframe(hdmitx_video *p_video);
static uint32_t hdmitx_set_audio_infoframe(hdmitx_audio *p_audio);
static uint32_t hdmitx_update_infoframe(system_port port, uint8_t upd);
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H27BJ2)
static uint8_t hdmitx_is_vborx_720p(hdmitx_video *p_video);
static uint32_t hdmitx_set_720p_clock_selector(system_port port);
static uint32_t hdmitx_set_720p(hdmitx_video *p_video);
static uint32_t hdmitx_reset_720p(system_port port);
static uint8_t hdmitx_is_vborx_4k_to_480p(hdmitx_video *p_video);
static uint32_t hdmitx_set_4k_480p_clock_gate(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_4k_480p_clock_gate(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_4k_480p_clock(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_4k_480p_clock(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_4k_480p_1001_1000_clock(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_4k_480p_1001_1000_clock(system_port port, uint8_t *p_r);
static uint32_t hdmitx_get_4k_480p_preset(system_port port, uint8_t *p_pktena, uint8_t *p_infena,
											uint8_t *p_vmutecnt, uint8_t *p_txactl1);
static uint32_t hdmitx_set_4k_480p_preset(system_port port, uint8_t pktena, uint8_t infena,
											uint8_t vmutecnt, uint8_t txactl1);
static uint32_t hdmitx_set_4k_480p_crc(system_port port, uint8_t on);
static uint32_t hdmitx_set_4k_480p_crc_expected_value(system_port port, uint8_t *p_exp);
static uint32_t hdmitx_get_4k_480p_crc_mon_value(system_port port, uint8_t *p_mon);
static uint32_t hdmitx_set_4k_480p_sync_delay(system_port port, uint8_t sy_dly, uint8_t sdmode);
static uint32_t hdmitx_reset_4k_480p_1001_1000(system_port port);
static uint32_t hdmitx_get_4k_480p_crc_error(system_port port, uint8_t *p_err);
static uint32_t hdmitx_ctrl_4k_480p_ctrl_crc_expected_value(uint8_t *p_crc, hdmitx_ctrl_crc ctrl);
static uint32_t hdmitx_4k_480p_output_seq(system_port port);
static uint32_t hdmitx_4k_480p_output_seq_reset(system_port port);
static uint32_t hdmitx_set_4k_480p(system_port port, uint8_t *p_cg, uint8_t *p_c, uint8_t *p_1001);
static uint32_t hdmitx_reset_4k_480p(system_port port, uint8_t *p_cg, uint8_t *p_c, uint8_t *p_1001);
static uint8_t hdmitx_is_vborx_2k_to_480p(hdmitx_video *p_video);
static uint32_t hdmitx_set_2k_480p_clock1(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_2k_480p_clock1(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_2k_480p_1001_1000_clock(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_2k_480p_1001_1000_clock(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_2k_480p_dc_clock(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_2k_480p_dc_clock(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_2k_480p_clock2(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_2k_480p_clock2(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_2k_480p_pll01(system_port port, uint8_t *p_w);
static uint32_t hdmitx_get_2k_480p_pll01(system_port port, uint8_t *p_r);
static uint32_t hdmitx_set_1080p60_clock(system_port port);
static uint32_t hdmitx_set_4k_clock(system_port port);
static uint32_t hdmitx_TMDS_clock_en(system_port port,uint32_t en);


static uint32_t hdmitx_2k_480p_output_seq(system_port port);
static uint32_t hdmitx_2k_480p_output_seq_reset(system_port port);
static uint32_t hdmitx_set_2k_480p(system_port port, uint8_t *p_c1, uint8_t *p_1001,
										uint8_t *p_dc, uint8_t *p_c2, uint8_t *p_pll01);
static uint32_t hdmitx_reset_2k_480p(system_port port, uint8_t *p_c1, uint8_t *p_1001,
										uint8_t *p_dc, uint8_t *p_c2, uint8_t *p_pll01);
static void hdmitx_init_reg_stack(uint8_t *p_s, uint8_t size);
static uint8_t hdmitx_is_reg_stacked(uint8_t *p_s, uint8_t size);
static uint32_t hdmitx_set_vborx_comb(hdmitx_video *p_video, uint8_t tx0_sen, uint8_t *p_allupd);
#endif
static uint32_t hdmitx_sg_setting(hdmitx_video *p_video, system_sg *p_sg);
static uint32_t hdmitx_copy_video_param(system_video *dest, hdmitx_video *src);
static uint8_t hdmitx_is_dvi_mode(hdmitx_dvi_mode mode);
static uint32_t hdmitx_set_txmstctl(system_port port, uint8_t mode, uint8_t page);
static uint32_t hdmitx_get_down_over_sf(system_audio_sf sf, system_audio_sf_ratio ratio,
										system_audio_sf *p_sf);
static uint32_t hdmitx_set_audio_cbit(hdmitx_audio *p_audio);
extern uint32_t hdmitx_hdcp_init(system_port port);
static uint32_t hdmitx_ctrl_req_abort(system_request *p_req, hdmitx_ctrl_req ctrl);
system_request hdmitx_ctrl_get_abort_req(void);
void hdmitx_ctrl_set_abort_req(system_request req);
static uint32_t hdmitx_ctrl_val_vcdb(uint8_t *p_val, hdmitx_ctrl_val ctrl);
//------------------------------------------------------------------------------
// abort request check
#define CHECK_ABORT() {												\
	if (hdmitx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		hdmitx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_HDMITX_ABORT;									\
	}																\
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_select_audio_ch(system_port port, hdmitx_audio_ch ch)
{
	system_reg_access access;
	system_command cmd[HDMITX_SELECT_AUDIO_CH_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SELECT_AUDIO_CH_NUM);

	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXMSTCTL_ADDR,
						ch,
						HDMITX_SELECT_AUDIO_CH_TXMSTCTL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_init_audio_trans_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMITX_INIT_AUDIO_TRANS_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_INIT_AUDIO_TRANS_NUM);
	// CTSMODSEL: change
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXAIFDBG_ADDR,
						HDMITX_INIT_AUDIO_TRANS_TXAIFDBG_CTSMODSEL_CHANGE,
						HDMITX_INIT_AUDIO_TRANS_TXAIFDBG_MASK);
	// DPLLRST: normal
	// PLLARSTMOD: reset
	SET_COMMAND(cmd, 1, VIDEOIF_INF_CTSGEN_02_ADDR,
						HDMITX_INIT_AUDIO_TRANS_CTSGEN_02_DPLLRST_NOR
							| HDMITX_INIT_AUDIO_TRANS_CTSGEN_02_PLLARSTMOD_RST,
						HDMITX_INIT_AUDIO_TRANS_CTSGEN_02_MASK);
	// ASTBFCNT: FIFO
	// STBMODCTL: ON
	SET_COMMAND(cmd, 2, VIDEOIF_PKT_ASTBCTL_ADDR,
						HDMITX_INIT_AUDIO_TRANS_ASTBCTL_ASTBFCNT_FIFO
							| HDMITX_INIT_AUDIO_TRANS_ASTBCTL_STBMODCTL_ON,
						HDMITX_INIT_AUDIO_TRANS_ASTBCTL_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_infena(system_port port, uint8_t infena, uint32_t mask)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_INFENA_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_INFENA_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_INFENA_ADDR,
						infena,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_pktena(system_port port, uint8_t pktena, uint32_t mask)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_PKTENA_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_PKTENA_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTENA_ADDR,
						pktena,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_tmds_ch_power(system_port port, uint8_t onoff)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_TMDS_CH_POWER_NUM];
	uint8_t idx00, idx02;
	uint32_t ret;

	// when ON, 0x1E02 -> 0x1E00, and when OFF 0x1E00 -> 0x1E02
	idx00 = (onoff) ? 1 : 0;
	idx02 = (onoff) ? 0 : 1;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_TMDS_CH_POWER_NUM);
	// 0x1E00: NPDCKO (TMDS clock channel power down)
	SET_COMMAND(cmd, idx00, VIDEOIF_TMDS_TMDSTXP0_1_ADDR,
							(onoff)
								? HDMITX_TMDSTXP0_1_NPDCKO_NOR
								: HDMITX_TMDSTXP0_1_NPDCKO_DWN,
							HDMITX_TMDSTXP0_1_NPDCKO_MASK);
	// 0x1E02: NPDDAT0/NPDDAT1/NPDDAT2 (TMDS data channel power down)
	SET_COMMAND(cmd, idx02, VIDEOIF_TMDS_TMDSTXP0_2_00_ADDR,
							(onoff)
								? HDMITX_TMDSTXP0_2_00_NPDDATX_NOR
								: HDMITX_TMDSTXP0_2_00_NPDDATX_DWN,
							HDMITX_TMDSTXP0_2_00_NPDDATX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_tmds_trans_rate(system_video_format vf, system_color_format cf,
											system_deep_color dc, uint32_t *p_rate)
{
	uint32_t rate;
	uint32_t ret;
	if (p_rate == NULL) {
		return ERR_NULL_POINTER;
	}

	if (((vf >= SYSTEM_VFORM_640x480P_5994or60_4v3)
			&& (vf <= SYSTEM_VFORM_1920x1080P_100_16v9))
		|| ((vf >= SYSTEM_VFORM_3840x2160P_2398or24_16v9)
			&& (vf <= SYSTEM_VFORM_4096x2160P_5994or60_256v135))
		|| ((vf >= SYSTEM_VFORM_1366x768P_5979or60_16v9)
			&& (vf <= SYSTEM_VFORM_1366x768P_5979or60_16v9))
		|| ((vf > SYSTEM_VFORM_PC_VESA_DMT_ID_UNUSED)
			&& (vf < SYSTEM_VFORM_PC_VESA_DMT_ID_MAX))
		|| ((vf > SYSTEM_VFORM_PC_VARIANT_UNUSED)
			&& (vf < SYSTEM_VFORM_PC_VARIANT_MAX))) {
		ret = system_ctrl_get_pixel_freq(vf, &rate);
		CHECK_ERROR(ret);
		rate = rate * 10;					// rate (KHz) x 10 (Kbps)
	} else {
		return ERR_HDMITX_VIDEO_FORMAT;
	}

	if (is_color_format_yuv420(cf)) {			// YUV420
		switch (dc) {
			case SYSTEM_DEEP_COLOR_8BIT:	rate = rate * 1 / 2;	break;	// rate x 0.5
			case SYSTEM_DEEP_COLOR_10BIT:	rate = rate * 5 / 8;	break;	// rate x 0.625
			case SYSTEM_DEEP_COLOR_12BIT:	rate = rate * 3 / 4;	break;	// rate x 0.75
			default:												return ERR_HDMITX_DEEP_COLOR;
		}
	} else if (is_color_format_yuv422(cf)) {	// YUV422
		rate = rate * 1 / 1;												// rate x 1.0
	} else {									// YUV444/RGB
		switch (dc) {
			case SYSTEM_DEEP_COLOR_8BIT:	rate = rate * 1 / 1;	break;	// rate x 1.0
			case SYSTEM_DEEP_COLOR_10BIT:	rate = rate * 5 / 4;	break;	// rate x 1.25
			case SYSTEM_DEEP_COLOR_12BIT:	rate = rate * 6 / 4;	break;	// rate x 1.5
			default:												return ERR_HDMITX_DEEP_COLOR;
		}
	}
	*p_rate = rate / 1000;		// rate / 1000 (Mbps)

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_scdc_source_version(system_port port, uint8_t bit7_0)
{
	hdmitx_scdc scdc;
	uint32_t ret;

	// set source version
	scdc.port    = port;
	scdc.oaddr   = HDMITX_SET_SCDC_SOURCE_VERSION_ADDR;
	scdc.length  = HDMITX_SET_SCDC_SOURCE_VERSION_LENGTH;
	scdc.data[0] = bit7_0;

	ret = hdmitx_write_scdc(&scdc);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_scdc_tmds_config(system_port port, uint8_t bit7_0)
{
	hdmitx_scdc scdc;
	uint32_t ret;

	// set TMDS config
	scdc.port    = port;
	scdc.oaddr   = HDMITX_SET_SCDC_TMDS_CONFIG_ADDR;
	scdc.length  = HDMITX_SET_SCDC_TMDS_CONFIG_LENGTH;
	scdc.data[0] = bit7_0;

	ret = hdmitx_write_scdc(&scdc);
	CHECK_ERROR(ret);

	// The Source shall allow a minimum of 1ms and a maximum of 100ms from the time
	// the TMDS_Bit_Clock_Ratio bit is written until resuming transmission of TMDS
	// clock and data at the updated data rete.

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_check_scdc_access(system_port port)
{
	hdmitx_scdc scdc;
	uint32_t ret;

	// read source version in order to check whether the Sink supports SCDC functionality.
	scdc.port   = port;
	scdc.oaddr  = HDMITX_SET_SCDC_SOURCE_VERSION_ADDR;
	scdc.length = HDMITX_SET_SCDC_SOURCE_VERSION_LENGTH;

	ret = hdmitx_read_scdc(&scdc);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_scdcs(hdmitx_video *p_video)
{
	uint32_t rate;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS bit rate
	ret = hdmitx_get_tmds_trans_rate(p_video->tx_video_format,
										p_video->tx_color_format,
										p_video->tx_deep_color,
										&rate);
	CHECK_ERROR(ret);
	if (rate > HDMITX_TMDS_TRANS_RATE_600GBPS) {
		return ERR_HDMITX_VIDEO_DATA;
	}
	if (rate > HDMITX_TMDS_TRANS_RATE_340GBPS) {
		// check whether the Sink supports SCDC functionality.
		ret = hdmitx_check_scdc_access(p_video->tx_port);
		if (ret == ERR_DDCTX_DDC_ACCESS_RETRY) {
			// Sink does not support SCDC functionality.
			return ERR_OK;
		}
		CHECK_ERROR(ret);
		// set Source Version
		ret = hdmitx_set_scdc_source_version(p_video->tx_port, HDMITX_VIDEO_FORM_SET_VERSION_4K2K);
		CHECK_ERROR(ret);
		// set TMDS config
		ret = hdmitx_set_scdc_tmds_config(p_video->tx_port, HDMITX_VIDEO_FORM_SET_CONFIG_4K2K);
		CHECK_ERROR(ret);
	} else {
		// check whether the Sink supports SCDC functionality.
		ret = hdmitx_check_scdc_access(p_video->tx_port);
		if (ret == ERR_DDCTX_DDC_ACCESS_RETRY) {
			// Sink does not support SCDC functionality.
			return ERR_OK;
		}
		CHECK_ERROR(ret);
		// clear Source Version
		ret = hdmitx_set_scdc_source_version(p_video->tx_port, HDMITX_VIDEO_FORM_SET_VERSION_OTHER);
		CHECK_ERROR(ret);
		// clear TMDS config
		ret = hdmitx_set_scdc_tmds_config(p_video->tx_port, HDMITX_VIDEO_FORM_SET_CONFIG_OTHER);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_txpll00en(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_TXPLL00EN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_TXPLL00EN_NUM);
	// 0x1E47: TXPLL00EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXPLL00_01_ADDR,
						en,
						HDMITX_TXPLL00_01_TXPLL00EN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_tx0_sen(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_TX0_SEN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_TX0_SEN_NUM);
	// 0x1E7F: TX0_SEN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						en,
						HDMITX_CKSELEN_TX0_SEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H27BJ2)
static uint32_t hdmitx_get_tx0_sen(system_port port, uint8_t *p_en)
{
	system_reg_access access;
	system_command cmd[HDMITX_GET_TX0_SEN_NUM];
	uint32_t ret;

	if (p_en == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_GET_TX0_SEN_NUM);
	// TXCORE0 Clock Selector Enable Setting
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						0,
						HDMITX_CKSELEN_TX0_SEN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_en = cmd[0].data;

	return ret;
}
#endif
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_tmds_pll(system_port port, hdmitx_tmds_pll_index idx)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_TMDS_PLL_MAX_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_TXPLL00_00_ADDR,
						HDMITX_SET_TMDS_PLL_TXPLL00_LEN, data);
	SET_BLOCK_DATA(data, 0, tmds_pll[idx][HDMITX_OADDR_IDX_0X46]);		// 0x1E46
	SET_BLOCK_DATA(data, 1, tmds_pll[idx][HDMITX_OADDR_IDX_0X47]);		// 0x1E47
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_05_ADDR,
						HDMITX_SET_TMDS_PLL_RXTXCORE0_LEN, data);
	SET_BLOCK_DATA(data, 0, tmds_pll[idx][HDMITX_OADDR_IDX_0X6A]);		// 0x1E6A
	SET_BLOCK_DATA(data, 1, tmds_pll[idx][HDMITX_OADDR_IDX_0X6B]);		// 0x1E6B
	SET_BLOCK_DATA(data, 2, tmds_pll[idx][HDMITX_OADDR_IDX_0X6C]);		// 0x1E6C
	SET_BLOCK_DATA(data, 3, tmds_pll[idx][HDMITX_OADDR_IDX_0X6D]);		// 0x1E6D
	SET_BLOCK_DATA(data, 4, tmds_pll[idx][HDMITX_OADDR_IDX_0X6E]);		// 0x1E6E
	SET_BLOCK_DATA(data, 5, tmds_pll[idx][HDMITX_OADDR_IDX_0X6F]);		// 0x1E6F
	SET_BLOCK_DATA(data, 6, tmds_pll[idx][HDMITX_OADDR_IDX_0X70]);		// 0x1E70
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}

void hdmitx_set_update_mode(hdmitx_update_mode mode,uint32_t isUHD)
{
	update_mode = mode;
	Is_UHD = isUHD;
}


//------------------------------------------------------------------------------
static uint32_t hdmitx_set_tmds(hdmitx_video *p_video, uint8_t *p_tx0_sen)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// whether down conversion or not
	if ((is_video_format_4k2k_60hz(p_video->rx_video_format)
			|| is_video_format_4k2k_50hz(p_video->rx_video_format))
		&& (is_video_format_1080p_60hz(p_video->tx_video_format)
			|| is_video_format_1080p_50hz(p_video->tx_video_format))
		&& (p_video->tx_deep_color == SYSTEM_DEEP_COLOR_10BIT)) {
		// Rx: 4K 50/60p, Tx: 1080p 10bit
		// Set Tx PLL Setting Enable (4.1.7 3)
		ret = hdmitx_set_txpll00en(p_video->tx_port, HDMITX_TXPLL00_01_TXPLL00EN_MANUAL);
		CHECK_ERROR(ret);
		
		if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
		{
			ret = hdmitx_set_tx0_sen(p_video->tx_port, HDMITX_CKSELEN_TX0_SEN_MANUAL);
			*p_tx0_sen = HDMITX_CKSELEN_TX0_SEN_MANUAL;
		}
		else
		{
			ret = hdmitx_set_tx0_sen(p_video->tx_port, 0x06);
			*p_tx0_sen = 0x06;
		}

		CHECK_ERROR(ret);
		// Set Tx PLL Setting (4.1.7 4)
		ret = hdmitx_set_tmds_pll(p_video->tx_port, HDMITX_PLL_IDX_10BIT);
		CHECK_ERROR(ret);
	} else if ((is_video_format_4k2k_60hz(p_video->rx_video_format)
			|| is_video_format_4k2k_50hz(p_video->rx_video_format))
		&& (is_video_format_1080p_60hz(p_video->tx_video_format)
			|| is_video_format_1080p_50hz(p_video->tx_video_format))
		&& (p_video->tx_deep_color == SYSTEM_DEEP_COLOR_12BIT)) {
		// Rx: 4K 50/60p, Tx: 1080p 12bit
		// Set Tx PLL Setting Enable (4.1.7 3)
		ret = hdmitx_set_txpll00en(p_video->tx_port, HDMITX_TXPLL00_01_TXPLL00EN_MANUAL);
		CHECK_ERROR(ret);

		if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
		{
			ret = hdmitx_set_tx0_sen(p_video->tx_port, HDMITX_CKSELEN_TX0_SEN_MANUAL);
			*p_tx0_sen = HDMITX_CKSELEN_TX0_SEN_MANUAL;
		}
		else
		{
			ret = hdmitx_set_tx0_sen(p_video->tx_port, 0x06);
			*p_tx0_sen = 0x06;
		}
		CHECK_ERROR(ret);
		// Set Tx PLL Setting (4.1.7 4)
		ret = hdmitx_set_tmds_pll(p_video->tx_port, HDMITX_PLL_IDX_12BIT);
		CHECK_ERROR(ret);
	} else {
		// no down conversion
		// Set Tx PLL Setting Enable (4.1.7 3)
		if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
		{
			ret = hdmitx_set_txpll00en(p_video->tx_port, HDMITX_TXPLL00_01_TXPLL00EN_AUTO);
			CHECK_ERROR(ret);
			ret = hdmitx_set_tx0_sen(p_video->tx_port, HDMITX_CKSELEN_TX0_SEN_AUTO);
			CHECK_ERROR(ret);
			*p_tx0_sen = HDMITX_CKSELEN_TX0_SEN_AUTO;
		}
		else
		{
			ret = hdmitx_set_txpll00en(p_video->tx_port, HDMITX_TXPLL00_01_TXPLL00EN_MANUAL);
			CHECK_ERROR(ret);
			ret = hdmitx_set_tx0_sen(p_video->tx_port, 0x06);
			CHECK_ERROR(ret);
			*p_tx0_sen = 0x06;
		}
	}
	CHECK_ABORT();
	// Clock Update (4.1.7 5)
	if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
	{
		ret = system_ctrl_tx_update_reg(p_video->tx_port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD);
		CHECK_ERROR(ret);
	}
	else
	{
		if(is_video_format_1080p_60hz(p_video->tx_video_format)||(p_video->tx_video_format == SYSTEM_VFORM_720x480P_5994or60_4v3))
		{
			hdmitx_set_1080p60_clock(p_video->tx_port);
		}
		if(is_video_format_4k2k_60hz(p_video->tx_video_format))
		{
			hdmitx_set_4k_clock(p_video->tx_port);
		}	

	}
	// Set TMDS Setting Enable (4.1.7 6)
	
	if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
	{
		ret = hdmitx_set_tmds_setting_enable(p_video->tx_port);
		CHECK_ERROR(ret);
	}
	// Set TMDS Transmitter Setting (4.1.7 7)
	ret = hdmitx_set_tmds_transmitter_setting(p_video);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_select_hdmi_dvi(system_port port, hdmitx_dvi_mode mode)
{
	system_reg_access access;
	system_command cmd[HDMITX_SELECT_HDMI_DVI_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SELECT_HDMI_DVI_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TSYSCTRL_ADDR,
						(mode == HDMITX_DVI_MODE_DISABLE)
							? HDMITX_SELECT_HDMI_DVI_TSYSCTRL_HDMI_HDMI
							: HDMITX_SELECT_HDMI_DVI_TSYSCTRL_HDMI_DVI,
						HDMITX_SELECT_HDMI_DVI_TSYSCTRL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_scramble_mode(hdmitx_video *p_video)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_SCRAMBLE_MODE_NUM];
	uint32_t rate;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS bit rate
	ret = hdmitx_get_tmds_trans_rate(p_video->tx_video_format,
										p_video->tx_color_format,
										p_video->tx_deep_color,
										&rate);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &p_video->tx_port, cmd, HDMITX_SET_SCRAMBLE_MODE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_TXENCSET4_02_ADDR,
						(rate > HDMITX_TMDS_TRANS_RATE_340GBPS)
						? (HDMITX_TXENCSET4_02_SCRMOD_ENA
							| HDMITX_TXENCSET4_02_SCRVER_ENA
							| HDMITX_TXENCSET4_02_SCRON_ON)
						: (HDMITX_TXENCSET4_02_SCRMOD_DIS
							| HDMITX_TXENCSET4_02_SCRVER_DIS
							| HDMITX_TXENCSET4_02_SCRON_OFF),
						HDMITX_TXENCSET4_02_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_disable_scramble_mode(hdmitx_video *p_video)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_SCRAMBLE_MODE_NUM];
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &p_video->tx_port, cmd, HDMITX_SET_SCRAMBLE_MODE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_TXENCSET4_02_ADDR,
						HDMITX_TXENCSET4_02_SCRMOD_DIS
						| HDMITX_TXENCSET4_02_SCRVER_DIS
						| HDMITX_TXENCSET4_02_SCRON_OFF,
						HDMITX_TXENCSET4_02_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_tmds_setting_enable(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_TMDS_ENABLE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_TMDS_ENABLE_NUM);
	// 0x1E0B: TXPRM0EN
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSTXP0_3_01_ADDR,
						HDMITX_SET_TMDS_ENABLE_TMDSTXP0_3_TXPRMXEN_MAN,
						HDMITX_SET_TMDS_ENABLE_TMDSTXP0_3_MASK);
	// 0x1E0F: TXPRM1EN
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_TMDSTXP0_3_05_ADDR,
						HDMITX_SET_TMDS_ENABLE_TMDSTXP0_3_TXPRMXEN_MAN,
						HDMITX_SET_TMDS_ENABLE_TMDSTXP0_3_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_tmds_transmitter_setting(hdmitx_video *p_video)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_TMDS_TRANS_NUM];
	system_command cmd1[3];
	system_command cmd2[2];
	system_block_access baccess;
	uint8_t data[HDMITX_SET_TMDS_TRANS_MAX_LEN];
	uint32_t rate;
	int8_t idx;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS transmitter rate
	ret = hdmitx_get_tmds_trans_rate(p_video->tx_video_format,
										p_video->tx_color_format,
										p_video->tx_deep_color,
										&rate);
	CHECK_ERROR(ret);
	// get TMDS transmitter setting data index
	if (rate < HDMITX_TMDS_TRANS_RATE_100GBPS)			idx = HDMITX_RATE_IDX_100GBPS;	// <= 1.0Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_155GBPS)	idx = HDMITX_RATE_IDX_155GBPS;	// <= 1.55Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_200GBPS)	idx = HDMITX_RATE_IDX_200GBPS;	// <= 2.0Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_225GBPS)	idx = HDMITX_RATE_IDX_225GBPS;	// <= 2.25Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_250GBPS)	idx = HDMITX_RATE_IDX_250GBPS;	// <= 2.5Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_300GBPS)	idx = HDMITX_RATE_IDX_300GBPS;	// <= 3.0Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_450GBPS)	idx = HDMITX_RATE_IDX_450GBPS;	// <= 4.5Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_500GBPS)	idx = HDMITX_RATE_IDX_500GBPS;	// <= 5.0Gbps
	else if (rate <= HDMITX_TMDS_TRANS_RATE_600GBPS)	idx = HDMITX_RATE_IDX_600GBPS;	// <= 6.0Gbps
	else												return ERR_HDMITX_VIDEO_DATA;

	SET_BLOCK_ACCESS(baccess, &(p_video->tx_port), VIDEOIF_TMDS_TMDSTXP0_4_00_ADDR,
						HDMITX_SET_TMDS_TRANS_JITTER_LEN, data);
	if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
	{
		SET_BLOCK_DATA(data, 0, tmds_data[idx][HDMITX_OADDR_IDX_0X1C]);		// 0x1E1C
		SET_BLOCK_DATA(data, 1, tmds_data[idx][HDMITX_OADDR_IDX_0X1D]);		// 0x1E1D
		SET_BLOCK_DATA(data, 2, tmds_data[idx][HDMITX_OADDR_IDX_0X1E]);		// 0x1E1E
		SET_BLOCK_DATA(data, 3, tmds_data[idx][HDMITX_OADDR_IDX_0X1F]);		// 0x1E1F
	}
	else if(update_mode == HDMITX_UPDATE_MODE_DISABLE)
	{
		//Jitter filter Setting
		if(is_video_format_1080p_60hz(p_video->tx_video_format))
		{
			SET_BLOCK_DATA(data, 0, 0xe8); 	// 0x1E1C
			SET_BLOCK_DATA(data, 1, 0x33); 	// 0x1E1D
			SET_BLOCK_DATA(data, 2, 0xf4); 	// 0x1E1E
			SET_BLOCK_DATA(data, 3, 0x21); 	// 0x1E1F
		}
		if(is_video_format_4k2k_60hz(p_video->tx_video_format))
		{
			if(Is_UHD)
			{
				SET_BLOCK_DATA(data, 0, tmds_data[idx][HDMITX_OADDR_IDX_0X1C]);	 // 0x1E1C
				SET_BLOCK_DATA(data, 1, tmds_data[idx][HDMITX_OADDR_IDX_0X1D]);	 // 0x1E1D
				SET_BLOCK_DATA(data, 2, tmds_data[idx][HDMITX_OADDR_IDX_0X1E]);	 // 0x1E1E
				SET_BLOCK_DATA(data, 3, tmds_data[idx][HDMITX_OADDR_IDX_0X1F]);	 // 0x1E1F
			} 
			else //This new change is NG,so not use.
			{
				SET_BLOCK_DATA(data, 0, 0x00); 	// 0x1E1C
				SET_BLOCK_DATA(data, 1, 0x00); 	// 0x1E1D
				SET_BLOCK_DATA(data, 2, 0x00); 	// 0x1E1E
				SET_BLOCK_DATA(data, 3, 0x20); 	// 0x1E1F
			}
		}
		if(p_video->tx_video_format == SYSTEM_VFORM_720x480P_5994or60_4v3)
		{
			SET_BLOCK_DATA(data, 0, 0xF4); 	// 0x1E1C
			SET_BLOCK_DATA(data, 1, 0x31); 	// 0x1E1D
			SET_BLOCK_DATA(data, 2, 0xF4); 	// 0x1E1E
			SET_BLOCK_DATA(data, 3, 0x21); 	// 0x1E1F
		}
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &(p_video->tx_port), cmd, HDMITX_SET_TMDS_TRANS_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSTXP0_1_ADDR,
						tmds_data[idx][HDMITX_OADDR_IDX_0X00],
						HDMITX_SET_TMDS_TRANS_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_TMDSTXP0_2_00_ADDR,
						tmds_data[idx][HDMITX_OADDR_IDX_0X02],
						HDMITX_SET_TMDS_TRANS_MASK);
	SET_COMMAND(cmd, 2, VIDEOIF_TMDS_TMDSTXP0_2_01_ADDR,
						tmds_data[idx][HDMITX_OADDR_IDX_0X04],
						HDMITX_SET_TMDS_TRANS_MASK);
	SET_COMMAND(cmd, 3, VIDEOIF_TMDS_TMDSTXP0_2_02_ADDR,
						tmds_data[idx][HDMITX_OADDR_IDX_0X08],
						HDMITX_SET_TMDS_TRANS_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &(p_video->tx_port), VIDEOIF_TMDS_TMDSTXP0_3_00_ADDR,
						HDMITX_SET_TMDS_TRANS_PARAM_LEN, data);
	SET_BLOCK_DATA(data, 0, tmds_data[idx][HDMITX_OADDR_IDX_0X0A]);		// 0x1E0A
	SET_BLOCK_DATA(data, 1, tmds_data[idx][HDMITX_OADDR_IDX_0X0B]);		// 0x1E0B
	SET_BLOCK_DATA(data, 2, tmds_data[idx][HDMITX_OADDR_IDX_0X0C]);		// 0x1E0C
	SET_BLOCK_DATA(data, 3, tmds_data[idx][HDMITX_OADDR_IDX_0X0D]);		// 0x1E0D
	SET_BLOCK_DATA(data, 4, tmds_data[idx][HDMITX_OADDR_IDX_0X0E]);		// 0x1E0E
	SET_BLOCK_DATA(data, 5, tmds_data[idx][HDMITX_OADDR_IDX_0X0F]);		// 0x1E0F
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	if(update_mode == HDMITX_UPDATE_MODE_DISABLE)
	{

		//The PLL of Deep Color
		SET_ACCESS(access, &(p_video->tx_port), cmd1, 3);
		SET_COMMAND(cmd1, 0, VIDEOIF_CKGEN_TXPLL01_01_ADDR,0x80,0xff);
		SET_COMMAND(cmd1, 1, VIDEOIF_CKGEN_TXPLL01_00_ADDR,0x81,0xff);
		SET_COMMAND(cmd1, 2, VIDEOIF_CKGEN_TXPLL01_01_ADDR,0x80,0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);

		//Clock Update
		SET_ACCESS(access, &(p_video->tx_port), cmd2, 2);
		SET_COMMAND(cmd2, 0, 0x00001E78,0x80,0xff);//No found in register SPEC
		SET_COMMAND(cmd2, 1, 0x00001E7F,0x06,0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		system_ctrl_tx_update_reg(p_video->tx_port, 0x10);
	}
	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_reset_tmds_encode(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMITX_RESET_TMDS_ENCODE_NUM];
	system_command cmd1[1];
	uint32_t ret;

	CHECK_ABORT();
	// TMDS encoder soft reset must be performed,
	// after 0x1E00 bit0 NPDCKO has become normal operation
	// sleep 80ms
	SLEEP(HDMITX_SLEEP_TIME_80MSEC);
	if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
	{
		SET_ACCESS(access, &port, cmd, HDMITX_RESET_TMDS_ENCODE_NUM);
		// 0x1034: TP0SWPDWN: normal
		//         TP0ENCSRST: reset
		//         TP0TMDSSRST: normal
		SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TXSYSCFG1_ADDR,
							HDMITX_TXSYSCFG1_TP0SWPDWN_NOR
								| HDMITX_TXSYSCFG1_TP0ENCSRST_RST
								| HDMITX_TXSYSCFG1_TP0TMDSSRST_NOR,
							HDMITX_TXSYSCFG1_MASK);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
	}
	else
	{
		system_ctrl_tx_update_reg(port, 0x66);
		
		SET_ACCESS(access, &port, cmd1, 1);
		SET_COMMAND(cmd1, 0, VIDEOIF_SYSTEM_TSRST_ADDR,0x46,0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		SLEEP(1);
		
		SET_ACCESS(access, &port, cmd1, 1);
		SET_COMMAND(cmd1, 0, VIDEOIF_SYSTEM_TDPCMODE_00_ADDR,0x20,0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		SLEEP(1);

		SET_ACCESS(access, &port, cmd1, 1);
		SET_COMMAND(cmd1, 0, VIDEOIF_DNC_DNCCTL1_ADDR,0x40,0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		SLEEP(1);

		SET_ACCESS(access, &port, cmd1, 1);
		SET_COMMAND(cmd1, 0, VIDEOIF_DNC_DNCCTL1_ADDR,0x00,0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		SLEEP(1);
		
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_y(hdmitx_video *p_video, uint8_t *y)
{
	switch (p_video->tx_color_format) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:		*y = HDMITX_AVI_Y_YCBCR422;		break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:		*y = HDMITX_AVI_Y_YCBCR422;		break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:		*y = HDMITX_AVI_Y_YCBCR444;		break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:		*y = HDMITX_AVI_Y_YCBCR444;		break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:		*y = HDMITX_AVI_Y_YCBCR422;		break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:		*y = HDMITX_AVI_Y_YCBCR422;		break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:		*y = HDMITX_AVI_Y_YCBCR444;		break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:		*y = HDMITX_AVI_Y_YCBCR444;		break;
		case SYSTEM_COLOR_RGB_FULL_RANGE1:			*y = HDMITX_AVI_Y_RGB;			break;
		case SYSTEM_COLOR_RGB_LIMITED_RANGE:		*y = HDMITX_AVI_Y_RGB;			break;
		case SYSTEM_COLOR_RGB_FULL_RANGE2:			*y = HDMITX_AVI_Y_RGB;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:		*y = HDMITX_AVI_Y_YCBCR420;		break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:		*y = HDMITX_AVI_Y_YCBCR420;		break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:	*y = HDMITX_AVI_Y_YCBCR420;		break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:	*y = HDMITX_AVI_Y_YCBCR422;		break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:	*y = HDMITX_AVI_Y_YCBCR444;		break;
		case SYSTEM_COLOR_RGB_ITU_R_BT2020:			*y = HDMITX_AVI_Y_RGB;			break;
		default:								return ERR_HDMITX_COLOR_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_a(hdmitx_video *p_video, uint8_t *a)
{
	*a = HDMITX_AVI_A_ACTIVE_FORMAT;
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_b(hdmitx_video *p_video, uint8_t *b)
{
	*b = HDMITX_AVI_B_NO_DATA;
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_s(hdmitx_video *p_video, uint8_t *s)
{
#if defined(CONFIG_TX_AVIIF_SCAN_INFO_NO_DATA)
	*s = HDMITX_AVI_S_NO_DATA;
#elif defined(CONFIG_TX_AVIIF_SCAN_INFO_OVERSCAN)
	*s = HDMITX_AVI_S_OVERSCANNED;
#elif defined(CONFIG_TX_AVIIF_SCAN_INFO_UNDERSCAN)
	*s = HDMITX_AVI_S_UNDERSCANNED;
#else // CONFIG_TX_AVIIF_SCAN_INFO_XXX
	*s = HDMITX_AVI_S_OVERSCANNED;
#endif // CONFIG_TX_AVIIF_SCAN_INFO_XXX
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_c(hdmitx_video *p_video, uint8_t *c)
{
	switch (p_video->tx_color_format) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:		*c = HDMITX_AVI_C_SMPTE_170M;	break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:		*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:		*c = HDMITX_AVI_C_SMPTE_170M;	break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:		*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:		*c = HDMITX_AVI_C_ITU_R_BT709;	break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:		*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:		*c = HDMITX_AVI_C_ITU_R_BT709;	break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:		*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_RGB_FULL_RANGE1:			*c = HDMITX_AVI_C_NO_DATA;		break;
		case SYSTEM_COLOR_RGB_LIMITED_RANGE:		*c = HDMITX_AVI_C_NO_DATA;		break;
		case SYSTEM_COLOR_RGB_FULL_RANGE2:			*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:		*c = HDMITX_AVI_C_SMPTE_170M;	break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:		*c = HDMITX_AVI_C_ITU_R_BT709;	break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:	*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:	*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:	*c = HDMITX_AVI_C_EXTENDED;		break;
		case SYSTEM_COLOR_RGB_ITU_R_BT2020:			*c = HDMITX_AVI_C_EXTENDED;		break;
		default:								return ERR_HDMITX_COLOR_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_m(hdmitx_video *p_video, uint8_t *m)
{
	switch (p_video->tx_video_format) {
		case SYSTEM_VFORM_640x480P_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480P_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080I_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x480I_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480I_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x240P_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x240P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_2880x480I_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_2880x480I_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_2880x240P_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_2880x240P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1440x480P_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_1440x480P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x576P_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x576P_50_16v9:				*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080I_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x576I_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x576I_50_16v9:				*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x288P_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x288P_50_16v9:				*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_2880x576I_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_2880x576I_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_2880x288P_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_2880x288P_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1440x576P_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_1440x576P_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_2398or24_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_25_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_2997or30_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_2880x480P_5994or60_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_2880x480P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_2880x576P_50_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_2880x576P_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080I_T1250_50_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080I_100_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_100_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x576P_100_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x576P_100_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x576I_100_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x576I_100_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080I_11988or120_16v9:	*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_11988or120_16v9:	*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x480P_11988or120_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480P_11988or120_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x480I_11988or120_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480I_11988or120_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x576P_200_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x576P_200_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x576I_200_4v3:				*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x576I_200_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x480P_23976or240_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480P_23976or240_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_720x480I_23976or240_4v3:		*m = HDMITX_AVI_M_4_3;		break;
		case SYSTEM_VFORM_720x480I_23976or240_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_2398or24_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_25_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1280x720P_2997or30_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_11988or120_16v9:	*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_1920x1080P_100_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_3840x2160P_2398or24_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_3840x2160P_25_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_3840x2160P_2997or30_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_3840x2160P_50_16v9:			*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_3840x2160P_5994or60_16v9:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_4096x2160P_2398or24_256v135:	*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_4096x2160P_25_256v135:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_4096x2160P_2997or30_256v135:	*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_4096x2160P_50_256v135:		*m = HDMITX_AVI_M_16_9;		break;
		case SYSTEM_VFORM_4096x2160P_5994or60_256v135:	*m = HDMITX_AVI_M_16_9;		break;
		default:										*m = HDMITX_AVI_M_NO_DATA;	break;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_r(hdmitx_video *p_video, uint8_t *r)
{
	*r = HDMITX_AVI_R_SAME_AS_M;
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_itc(hdmitx_video *p_video, uint8_t *itc)
{
	*itc = HDMITX_AVI_ITC_NO_DATA;
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_ec(hdmitx_video *p_video, uint8_t *ec)
{
	switch (p_video->tx_color_format) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:		*ec = HDMITX_AVI_EC_XVYCC709;			break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:		*ec = HDMITX_AVI_EC_XVYCC709;			break;
		case SYSTEM_COLOR_RGB_FULL_RANGE1:			*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_RGB_LIMITED_RANGE:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_RGB_FULL_RANGE2:			*ec = HDMITX_AVI_EC_ADOBERGB;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:		*ec = HDMITX_AVI_EC_XVYCC601;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:	*ec = HDMITX_AVI_EC_BT2020_RGB_YCBCR;	break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:	*ec = HDMITX_AVI_EC_BT2020_RGB_YCBCR;	break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:	*ec = HDMITX_AVI_EC_BT2020_RGB_YCBCR;	break;
		case SYSTEM_COLOR_RGB_ITU_R_BT2020:			*ec = HDMITX_AVI_EC_BT2020_RGB_YCBCR;	break;
		default:								return ERR_HDMITX_COLOR_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_q(hdmitx_video *p_video, uint8_t *q)
{
	uint8_t vcdb;
	uint32_t ret;

	switch (p_video->tx_color_format) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_RGB_FULL_RANGE1:
			// get Byte#3 of Video Capability Data Block
			ret = hdmitx_ctrl_val_vcdb(&vcdb, HDMITX_CTRL_VAL_GET);
			CHECK_ERROR(ret);
			if ((vcdb & HDMITX_VCDB_QS_MASK) == HDMITX_VCDB_QS_SELECTABLE) {
				*q = HDMITX_AVI_Q_FULL_RANGE;
			} else {
				*q = HDMITX_AVI_Q_DEFAULT;
			}
																						break;
		case SYSTEM_COLOR_RGB_LIMITED_RANGE:
			// get Byte#3 of Video Capability Data Block
			ret = hdmitx_ctrl_val_vcdb(&vcdb, HDMITX_CTRL_VAL_GET);
			CHECK_ERROR(ret);
			if ((vcdb & HDMITX_VCDB_QS_MASK) == HDMITX_VCDB_QS_SELECTABLE) {
				*q = HDMITX_AVI_Q_LIMITED_RANGE;
			} else {
				*q = HDMITX_AVI_Q_DEFAULT;
			}
																						break;
		case SYSTEM_COLOR_RGB_FULL_RANGE2:			*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:		*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:	*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:	*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:	*q = HDMITX_AVI_Q_DEFAULT;			break;
		case SYSTEM_COLOR_RGB_ITU_R_BT2020:			*q = HDMITX_AVI_Q_DEFAULT;			break;
		default:								return ERR_HDMITX_COLOR_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_sc(hdmitx_video *p_video, uint8_t *sc)
{
	*sc = HDMITX_AVI_SC_NO_KNOWN;
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_vic(hdmitx_video *p_video, uint8_t *vic)
{
	system_video_format vform;

	vform = p_video->tx_video_format;

	if (vform >= SYSTEM_VFORM_640x480P_5994or60_4v3 && vform <= SYSTEM_VFORM_1920x1080P_100_16v9) {
		*vic = vform;
	} else if (vform == SYSTEM_VFORM_3840x2160P_2398or24_16v9) {
		*vic = HDMITX_AVI_VIC_SEE_HDMI_VS_INFOFRAME;
	} else if (vform == SYSTEM_VFORM_3840x2160P_25_16v9) {
		*vic = HDMITX_AVI_VIC_SEE_HDMI_VS_INFOFRAME;
	} else if (vform == SYSTEM_VFORM_3840x2160P_2997or30_16v9) {
		*vic = HDMITX_AVI_VIC_SEE_HDMI_VS_INFOFRAME;
	} else if (vform == SYSTEM_VFORM_3840x2160P_50_16v9) {
		*vic = vform;
	} else if (vform == SYSTEM_VFORM_3840x2160P_5994or60_16v9) {
		*vic = vform;
	} else if (vform == SYSTEM_VFORM_4096x2160P_2398or24_256v135) {
		*vic = HDMITX_AVI_VIC_SEE_HDMI_VS_INFOFRAME;
	} else if (vform == SYSTEM_VFORM_4096x2160P_25_256v135) {
		*vic = vform;
	} else if (vform == SYSTEM_VFORM_4096x2160P_2997or30_256v135) {
		*vic = vform;
	} else if (vform == SYSTEM_VFORM_4096x2160P_50_256v135) {
		*vic = vform;
	} else if (vform == SYSTEM_VFORM_4096x2160P_5994or60_256v135) {
		*vic = vform;
	} else {
		*vic = HDMITX_AVI_VIC_NO_VIC;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_yq(hdmitx_video *p_video, uint8_t *yq)
{
	switch (p_video->tx_color_format) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_RGB_FULL_RANGE1:			*yq = HDMITX_AVI_YQ_FULL_RANGE;		break;
		case SYSTEM_COLOR_RGB_LIMITED_RANGE:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_RGB_FULL_RANGE2:			*yq = HDMITX_AVI_YQ_FULL_RANGE;		break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:		*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:	*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:	*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:	*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		case SYSTEM_COLOR_RGB_ITU_R_BT2020:			*yq = HDMITX_AVI_YQ_LIMITED_RANGE;	break;
		default:								return ERR_HDMITX_COLOR_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_cn(hdmitx_video *p_video, uint8_t *cn)
{
	*cn = HDMITX_AVI_CN_GRAPHICS;
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_avi_pr(hdmitx_video *p_video, uint8_t *pr)
{
	switch (p_video->tx_video_format) {
		case SYSTEM_VFORM_640x480P_5994or60_4v3:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480P_5994or60_4v3:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480P_5994or60_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1280x720P_5994or60_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080I_5994or60_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480I_5994or60_4v3:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 6
		case SYSTEM_VFORM_720x480I_5994or60_16v9:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 7
		case SYSTEM_VFORM_720x240P_5994or60_4v3:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 8
		case SYSTEM_VFORM_720x240P_5994or60_16v9:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 9
		case SYSTEM_VFORM_2880x480I_5994or60_4v3:		*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 10
		case SYSTEM_VFORM_2880x480I_5994or60_16v9:		*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 11
		case SYSTEM_VFORM_2880x240P_5994or60_4v3:		*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 12
		case SYSTEM_VFORM_2880x240P_5994or60_16v9:		*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 13
		case SYSTEM_VFORM_1440x480P_5994or60_4v3:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 14
		case SYSTEM_VFORM_1440x480P_5994or60_16v9:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 15
		case SYSTEM_VFORM_1920x1080P_5994or60_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576P_50_4v3:				*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576P_50_16v9:				*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1280x720P_50_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080I_50_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576I_50_4v3:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 21
		case SYSTEM_VFORM_720x576I_50_16v9:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 22
		case SYSTEM_VFORM_720x288P_50_4v3:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 23
		case SYSTEM_VFORM_720x288P_50_16v9:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 24
		case SYSTEM_VFORM_2880x576I_50_4v3:				*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 25
		case SYSTEM_VFORM_2880x576I_50_16v9:			*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 26
		case SYSTEM_VFORM_2880x288P_50_4v3:				*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 27
		case SYSTEM_VFORM_2880x288P_50_16v9:			*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 28
		case SYSTEM_VFORM_1440x576P_50_4v3:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 29
		case SYSTEM_VFORM_1440x576P_50_16v9:			*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 30
		case SYSTEM_VFORM_1920x1080P_50_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080P_2398or24_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080P_25_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080P_2997or30_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_2880x480P_5994or60_4v3:		*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 35
		case SYSTEM_VFORM_2880x480P_5994or60_16v9:		*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 36
		case SYSTEM_VFORM_2880x576P_50_4v3:				*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 37
		case SYSTEM_VFORM_2880x576P_50_16v9:			*pr = HDMITX_AVI_PR_4_TIMES;		break;	// 38
		case SYSTEM_VFORM_1920x1080I_T1250_50_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080I_100_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1280x720P_100_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576P_100_4v3:				*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576P_100_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576I_100_4v3:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 44
		case SYSTEM_VFORM_720x576I_100_16v9:			*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 45
		case SYSTEM_VFORM_1920x1080I_11988or120_16v9:	*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1280x720P_11988or120_16v9:	*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480P_11988or120_4v3:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480P_11988or120_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480I_11988or120_4v3:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 50
		case SYSTEM_VFORM_720x480I_11988or120_16v9:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 51
		case SYSTEM_VFORM_720x576P_200_4v3:				*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576P_200_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x576I_200_4v3:				*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 54
		case SYSTEM_VFORM_720x576I_200_16v9:			*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 55
		case SYSTEM_VFORM_720x480P_23976or240_4v3:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480P_23976or240_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_720x480I_23976or240_4v3:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 58
		case SYSTEM_VFORM_720x480I_23976or240_16v9:		*pr = HDMITX_AVI_PR_2_TIMES;		break;	// 59
		case SYSTEM_VFORM_1280x720P_2398or24_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1280x720P_25_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1280x720P_2997or30_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080P_11988or120_16v9:	*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_1920x1080P_100_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_3840x2160P_2398or24_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_3840x2160P_25_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_3840x2160P_2997or30_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_3840x2160P_50_16v9:			*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_3840x2160P_5994or60_16v9:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_4096x2160P_2398or24_256v135:	*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_4096x2160P_25_256v135:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_4096x2160P_2997or30_256v135:	*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_4096x2160P_50_256v135:		*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		case SYSTEM_VFORM_4096x2160P_5994or60_256v135:	*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
		default:										*pr = HDMITX_AVI_PR_NO_REPETITION;	break;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_avi_infoframe(hdmitx_video *p_video)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_AVI_INFOFRAME_LEN];
	uint8_t y=0, a=0, b=0, s=0;
	uint8_t c=0, m=0, r=0;
	uint8_t itc=0, ec=0, q=0, sc=0;
	uint8_t vic=0;
	uint8_t yq=0, cn=0, pr=0;
	uint32_t ret = ERR_OK;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// data byte 1
	ret |= hdmitx_avi_y(p_video, &y);
	ret |= hdmitx_avi_a(p_video, &a);
	ret |= hdmitx_avi_b(p_video, &b);
	ret |= hdmitx_avi_s(p_video, &s);
	// data byte 2
	ret |= hdmitx_avi_c(p_video, &c);
	ret |= hdmitx_avi_m(p_video, &m);
	ret |= hdmitx_avi_r(p_video, &r);
	// data byte 3
	ret |= hdmitx_avi_itc(p_video, &itc);
	ret |= hdmitx_avi_ec(p_video, &ec);
	ret |= hdmitx_avi_q(p_video, &q);
	ret |= hdmitx_avi_sc(p_video, &sc);
	// data byte 4
	ret |= hdmitx_avi_vic(p_video, &vic);
	// data byte 5
	ret |= hdmitx_avi_yq(p_video, &yq);
	ret |= hdmitx_avi_cn(p_video, &cn);
	ret |= hdmitx_avi_pr(p_video, &pr);

	if (ret == ERR_OK) {
		SET_BLOCK_ACCESS(baccess, &(p_video->tx_port),
							VIDEOIF_INF_AVIINF_04_ADDR, HDMITX_SET_AVI_INFOFRAME_LEN, data);
		SET_BLOCK_DATA(data, 0, (y | a | b | s));		// 0x3224
		SET_BLOCK_DATA(data, 1, (c | m | r));			// 0x3225
		SET_BLOCK_DATA(data, 2, (itc | ec | q | sc));	// 0x3226
		SET_BLOCK_DATA(data, 3, vic);					// 0x3227
		SET_BLOCK_DATA(data, 4, (yq | cn | pr));		// 0x3228

		ret = device_block_write(&baccess);
		CHECK_ERROR(ret);
		// update AVI infoframe packet (autocleared)
		ret = hdmitx_update_infoframe(p_video->tx_port, HDMITX_PKTUPD_AVIUPD);
		CHECK_ERROR(ret);
	}

	// enable AVI infoframe packet
	ret = hdmitx_set_infena(p_video->tx_port,
							(ret == ERR_OK)
								? HDMITX_INFENA_AVIEN
								: HDMITX_INFENA_NONE,
							HDMITX_INFENA_AVIEN);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_disable_avi_infoframe(hdmitx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// disable AVI infoframe packet
	ret = hdmitx_set_infena(p_video->tx_port,
							HDMITX_INFENA_NONE,
							HDMITX_INFENA_AVIEN);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_select_page_of_vsif(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[HDMITX_SELECT_PAGE_OF_VSIF_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SELECT_PAGE_OF_VSIF_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_VSIPAGE_00_ADDR, page, HDMITX_SELECT_PAGE_OF_VSIF_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_hdmi_vic(system_port port, uint8_t hdmi_vic)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_HDMI_VIC_NUM];
	uint8_t hvf;
	uint32_t ret;

	if (hdmi_vic == HDMITX_HDMI_VIC_NONE) {
		hvf = HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIDEO_FORMAT_NONE;
	} else {
		hvf = HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIDEO_FORMAT_EXT;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_HDMI_VIC_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_HVSINF0_07_ADDR, hvf, HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIDEO_FORMAT_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_INF_HVSINF0_08_ADDR, hdmi_vic, HDMITX_SET_HDMI_VIC_HVSINF0_HDMI_VIC_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_hdmi_vs_infoframe(hdmitx_video *p_video)
{
	uint8_t hdmi_vic;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// When 4K2K 24/25/30Hz and 4K2K(SMPTE) 24Hz,
	// HDMI vendor specific infoFrame is necessary.
	if (p_video->tx_video_format == SYSTEM_VFORM_3840x2160P_2398or24_16v9) {
		hdmi_vic = HDMITX_HDMI_VIC_4K_24HZ;
	} else if (p_video->tx_video_format == SYSTEM_VFORM_4096x2160P_2398or24_256v135) {
		hdmi_vic = HDMITX_HDMI_VIC_4K_SMPTE_24HZ;
	} else if (p_video->tx_video_format == SYSTEM_VFORM_3840x2160P_25_16v9) {
		hdmi_vic = HDMITX_HDMI_VIC_4K_25HZ;
	} else if (p_video->tx_video_format == SYSTEM_VFORM_3840x2160P_2997or30_16v9) {
		hdmi_vic = HDMITX_HDMI_VIC_4K_30HZ;
	} else {
		hdmi_vic = HDMITX_HDMI_VIC_NONE;
	}
	// page(0xc0): HDMI vendor specific infoframe
	ret = hdmitx_select_page_of_vsif(p_video->tx_port, HDMITX_VSIF_PAGE_C0_HDMI_VS);
	CHECK_ERROR(ret);
	// set HDMI_Video_Format and HDMI_VIC
	ret = hdmitx_set_hdmi_vic(p_video->tx_port, hdmi_vic);
	CHECK_ERROR(ret);
	// update HDMI vendor specific infoframe packet (autocleared)
	ret = hdmitx_update_infoframe(p_video->tx_port, HDMITX_PKTUPD_HVSUPD);
	CHECK_ERROR(ret);
	// enable HDMI vendor specific infoframe packet
	ret = hdmitx_set_infena(p_video->tx_port,
							(hdmi_vic == HDMITX_HDMI_VIC_NONE)
								? HDMITX_INFENA_NONE
								: HDMITX_INFENA_HVSEN,
							HDMITX_INFENA_HVSEN);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_disable_hdmi_vs_infoframe(hdmitx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// disable HDMI vendor specific infoframe packet
	ret = hdmitx_set_infena(p_video->tx_port,
							HDMITX_INFENA_NONE,
							HDMITX_INFENA_HVSEN);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_audio_infoframe(hdmitx_audio *p_audio)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_AUDIO_INFOFRAME_LEN];
	uint8_t sf, ss, lsv, db1, db2;
	uint32_t ret;

	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_BLOCK_ACCESS(baccess, &(p_audio->port), VIDEOIF_INF_AUDINF_00_ADDR,
						HDMITX_SET_AUDIO_INFOFRAME_LEN, data);
	// infoframe type code + HDMI code: 0x04 + 0x80
	SET_BLOCK_DATA(data, 0, INFOFRAME_TYPE_AUDIO | HDMITX_INFOFRAME_TYPE);		// 0x3260
	// version
	SET_BLOCK_DATA(data, 1, INFOFRAME_VERSION_AUDIO);							// 0x3261
	// length of infoframe
	SET_BLOCK_DATA(data, 2, INFOFRAME_LENGTH_AUDIO);							// 0x3262
	// checksum (auto checksum)
	SET_BLOCK_DATA(data, 3, HDMITX_SET_AUDIO_INFOFRAME_CHKSUM_INIT);			// 0x3263
	// CT0-3: Refer to Stream Header
	// CC0-2: if_channel_count
	db1 = HDMITX_SET_AUDIO_INFOFRAME_CT_RSH
			| (p_audio->if_channel_count & HDMITX_SET_AUDIO_INFOFRAME_CC_MASK);
	SET_BLOCK_DATA(data, 4, db1);												// 0x3264
	// SF0-2: if_sampling_freq
	// SS0-1: if_sample_size
	ret = system_ctrl_audio_sf2if(p_audio->if_sampling_freq, &sf);
	CHECK_ERROR(ret);
	ret = system_ctrl_audio_ss2if(p_audio->if_sample_size, &ss);
	CHECK_ERROR(ret);
	db2 = ((sf & HDMITX_SET_AUDIO_INFOFRAME_SF_MASK)
			| (ss & HDMITX_SET_AUDIO_INFOFRAME_SS_MASK));
	SET_BLOCK_DATA(data, 5, db2);												// 0x3265
	// CXT0-4: Refer to Audio Coding Type (CT) field in Data Byte 1
	SET_BLOCK_DATA(data, 6, HDMITX_SET_AUDIO_INFOFRAME_REF_STREAM_HDR);			// 0x3266
	// CA0-7: if_channel_number
	SET_BLOCK_DATA(data, 7, p_audio->if_channel_number);						// 0x3267
	// DM_INH: Permitted or no information about any assertion of this
	// LSV0-3: if_level_shift_value
	// LFEPBL0-1: Unknown or refer to other information
	lsv = (p_audio->if_level_shift_value << HDMITX_SET_AUDIO_INFOFRAME_LSV0_BIT)
			& HDMITX_SET_AUDIO_INFOFRAME_LSV0_3_MASK;
	SET_BLOCK_DATA(data, 8, HDMITX_SET_AUDIO_INFOFRAME_DM_INH_PERMITTED
							| lsv
							| HDMITX_SET_AUDIO_INFOFRAME_LFEPBL0_1_UNKNOWN);	// 0x3268
	// 0 padding
	SET_BLOCK_DATA(data, 9, HDMITX_SET_AUDIO_INFOFRAME_NONE);					// 0x3269
	SET_BLOCK_DATA(data, 10, HDMITX_SET_AUDIO_INFOFRAME_NONE);					// 0x326A
	SET_BLOCK_DATA(data, 11, HDMITX_SET_AUDIO_INFOFRAME_NONE);					// 0x326B
	SET_BLOCK_DATA(data, 12, HDMITX_SET_AUDIO_INFOFRAME_NONE);					// 0x326C
	SET_BLOCK_DATA(data, 13, HDMITX_SET_AUDIO_INFOFRAME_NONE);					// 0x326D
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);
	// update audio infoframe packet (autocleared)
	ret = hdmitx_update_infoframe(p_audio->port, HDMITX_PKTUPD_AIUPD);
	CHECK_ERROR(ret);
	// enable audio infoframe packet
	ret = hdmitx_set_infena(p_audio->port, HDMITX_INFENA_AIEN, HDMITX_INFENA_AIEN);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_update_infoframe(system_port port, uint8_t upd)
{
	system_reg_access access;
	system_command cmd[HDMITX_UPDATE_INFOFRAME_NUM];
	uint32_t ret;

	CHECK_ABORT();

	// infoframe update
	SET_ACCESS(access, &port, cmd, HDMITX_UPDATE_INFOFRAME_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTUPD_00_ADDR,
						upd,
						HDMITX_UPDATE_INFOFRAME_PKTUPD_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);
	// check autocleared
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTUPD_00_ADDR,
						HDMITX_UPDATE_INFOFRAME_PKTUPD_00_ALL_CLEAR,
						HDMITX_UPDATE_INFOFRAME_PKTUPD_00_MASK);
	ret = device_read_wait(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H27BJ2)
static uint8_t hdmitx_is_vborx_720p(hdmitx_video *p_video)
{
	system_video_format rxvf, txvf;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// VBO-Rx: 1280x720p 25/30/50/60Hz, 1920x1080I 50/60Hz, 1080P 24/25/30Hz (pixel freq: 74.25MHz)
	//         1366x768 60(RB)Hz (FWXGA)
	// HDMI-Tx: same format
	if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_VBO) {
		rxvf = p_video->rx_video_format;
		txvf = p_video->tx_video_format;
		if (   ((rxvf == SYSTEM_VFORM_1280x720P_25_16v9) && (txvf == SYSTEM_VFORM_1280x720P_25_16v9))
			|| ((rxvf == SYSTEM_VFORM_1280x720P_2997or30_16v9) && (txvf == SYSTEM_VFORM_1280x720P_2997or30_16v9))
			|| ((rxvf == SYSTEM_VFORM_1280x720P_50_16v9) && (txvf == SYSTEM_VFORM_1280x720P_50_16v9))
			|| ((rxvf == SYSTEM_VFORM_1280x720P_5994or60_16v9) && (txvf == SYSTEM_VFORM_1280x720P_5994or60_16v9))
			|| ((rxvf == SYSTEM_VFORM_1920x1080I_50_16v9) && (txvf == SYSTEM_VFORM_1920x1080I_50_16v9))
			|| ((rxvf == SYSTEM_VFORM_1920x1080I_5994or60_16v9) && (txvf == SYSTEM_VFORM_1920x1080I_5994or60_16v9))
			|| ((rxvf == SYSTEM_VFORM_1920x1080P_2398or24_16v9) && (txvf == SYSTEM_VFORM_1920x1080P_2398or24_16v9))
			|| ((rxvf == SYSTEM_VFORM_1920x1080P_25_16v9) && (txvf == SYSTEM_VFORM_1920x1080P_25_16v9))
			|| ((rxvf == SYSTEM_VFORM_1920x1080P_2997or30_16v9) && (txvf == SYSTEM_VFORM_1920x1080P_2997or30_16v9))
			|| ((rxvf == SYSTEM_VFORM_1366x768P_5979or60_16v9) && (txvf == SYSTEM_VFORM_1366x768P_5979or60_16v9))
			|| ((rxvf == SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB) && (txvf == SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB))
		) {
			return 1;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_720p_clock_selector(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_720P_CLOCK_SELECTOR_NUM];
	uint32_t ret;

	// 0x1E7F: manual: overwritten
	ret = hdmitx_set_tx0_sen(port, HDMITX_CKSELEN_TX0_SEN_MANUAL);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, HDMITX_SET_720P_CLOCK_SELECTOR_NUM);
	// 0x1E6D: if 0x1E7F bit2 = 0b1
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXCORE0_02_ADDR,
						HDMITX_TXCORE0_02_44,
						HDMITX_TXCORE0_02_MASK);
	// 0x1E6F: if 0x1E7F bit2 = 0b1
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_TXCORE0_04_ADDR,
						HDMITX_TXCORE0_04_04,
						HDMITX_TXCORE0_04_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_720p(hdmitx_video *p_video)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_720P_MAX_NUM];
	system_port port;
	uint8_t vifmt, vofmt;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->tx_port;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_720P_PRE_NUM);
	// 0x1089
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADVINCTRL1_00_ADDR,
						HDMITX_PADVINCTRL1_00_10,
						HDMITX_PADVINCTRL1_00_MASK);
	// 0x10B1
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR,
						HDMITX_SWMODE4_01_01,
						HDMITX_SWMODE4_01_MASK);
	// 0x3054
	SET_COMMAND(cmd, 2, VIDEOIF_VIDEO_INREPSET_ADDR,
						HDMITX_INREPSET_01,
						HDMITX_INREPSET_MASK);
	// 0x304F: overwritten
	SET_COMMAND(cmd, 3, VIDEOIF_VIDEO_SYNCCNT_ADDR,
						HDMITX_SYNCCNT_00,
						HDMITX_SYNCCNT_MASK);
	// 0x3070: overwritten
	SET_COMMAND(cmd, 4, VIDEOIF_VIDEO_VIDEOFMT_00_ADDR,
						HDMITX_VIDEOFMT_00_10,
						HDMITX_VIDEOFMT_00_MASK);
	// 0x3071: overwritten
	SET_COMMAND(cmd, 5, VIDEOIF_VIDEO_VIDEOFMT_01_ADDR,
						HDMITX_VIDEOFMT_01_90,
						HDMITX_VIDEOFMT_01_MASK);
	// 0x3075: overwritten
	SET_COMMAND(cmd, 6, VIDEOIF_VIDEO_PCFMT_ADDR,
						HDMITX_PCFMT_PCFMT_DEF,
						HDMITX_PCFMT_PCFMT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	CHECK_ABORT();
	// Update Register Status
	// CLKUPD
	ret = system_ctrl_tx_update_reg(port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Software Reset
	// VIFSRST, CCSRST, ENCSRST
	ret = system_ctrl_tx_soft_reset(port, SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_CCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST);
	CHECK_ERROR(ret);
	// set TXCORE0 clock selector
	ret = hdmitx_set_720p_clock_selector(port);
	CHECK_ERROR(ret);

	// set video format again to remove audio noise
	vifmt = (p_video->rx_video_format == SYSTEM_VFORM_1366x768P_5979or60_16v9)
			? SYSTEM_VFORM_1280x720P_5994or60_16v9
			: (p_video->rx_video_format == SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB)
			? SYSTEM_VFORM_1280x720P_5994or60_16v9
			: p_video->rx_video_format;
	vofmt = (p_video->tx_video_format == SYSTEM_VFORM_1366x768P_5979or60_16v9)
			? SYSTEM_VFORM_1280x720P_5994or60_16v9
			: (p_video->tx_video_format == SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB)
			? SYSTEM_VFORM_1280x720P_5994or60_16v9
			: p_video->tx_video_format;
	SET_ACCESS(access, &port, cmd, HDMITX_SET_720P_POST_NUM);
	// 0x3070: overwritten
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_VIDEOFMT_00_ADDR,
						vifmt,
						HDMITX_VIDEOFMT_00_MASK);
	// 0x3071: overwritten
	SET_COMMAND(cmd, 1, VIDEOIF_VIDEO_VIDEOFMT_01_ADDR,
						vofmt | HDMITX_VIDEOFMT_01_VOFMTSEL_USE_VOFMT,
						HDMITX_VIDEOFMT_01_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	CHECK_ABORT();
	// Update Register Status
	// CLKUPD, VIFUPD, CSCUPD
	ret = system_ctrl_tx_update_reg(port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
											| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
											| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Software Reset
	// VIFSRST, CCSRST, ENCSRST, AVCSRST
	ret = system_ctrl_tx_soft_reset(port, SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_CCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_AVCSRST);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_reset_720p(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMITX_RESET_720P_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_RESET_720P_NUM);
	// 0x1089
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADVINCTRL1_00_ADDR,
						HDMITX_PADVINCTRL1_00_DEF,
						HDMITX_PADVINCTRL1_00_MASK);
	// 0x10B1
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR,
						HDMITX_SWMODE4_01_DEF,
						HDMITX_SWMODE4_01_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint8_t hdmitx_is_vborx_4k_to_480p(hdmitx_video *p_video)
{
	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// VBO-Rx: 2160p 60Hz
	// HDMI-Tx: 480p 60Hz
	if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_VBO) {
		if ((p_video->rx_video_format == SYSTEM_VFORM_3840x2160P_5994or60_16v9)
			&& (p_video->tx_video_format == SYSTEM_VFORM_720x480P_5994or60_4v3)) {
			return 1;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_clock_gate(system_port port, uint8_t *p_w)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_4K_480P_CLOCK_GATE_MAX_LEN];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_BLOCK_ACCESS(baccess, &port, HDMITX_VIDEOIF_1E57_ADDR,
						HDMITX_SET_4K_480P_CLOCK_GATE_1E57_LEN, data);
	SET_BLOCK_DATA(data, 0, p_w[0]);		// 0x1E57
	SET_BLOCK_DATA(data, 1, p_w[1]);		// 0x1E58
	SET_BLOCK_DATA(data, 2, p_w[2]);		// 0x1E59
	SET_BLOCK_DATA(data, 3, p_w[3]);		// 0x1E5A
	SET_BLOCK_DATA(data, 4, p_w[4]);		// 0x1E5B
	SET_BLOCK_DATA(data, 5, p_w[5]);		// 0x1E5C
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_PAD_ADDR,
						HDMITX_SET_4K_480P_CLOCK_GATE_1E60_LEN, data);
	SET_BLOCK_DATA(data, 0, p_w[6]);		// 0x1E60
	SET_BLOCK_DATA(data, 1, p_w[7]);		// 0x1E61
	SET_BLOCK_DATA(data, 2, p_w[8]);		// 0x1E62
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_4k_480p_clock_gate(system_port port, uint8_t *p_r)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E57 - 0x1E5C
	SET_BLOCK_ACCESS(baccess, &port, HDMITX_VIDEOIF_1E57_ADDR,
						HDMITX_SET_4K_480P_CLOCK_GATE_1E57_LEN,
						p_r);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);
	// 0x1E60 - 0x1E62
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_PAD_ADDR,
						HDMITX_SET_4K_480P_CLOCK_GATE_1E60_LEN,
						&p_r[HDMITX_SET_4K_480P_CLOCK_GATE_1E57_LEN]);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_clock(system_port port, uint8_t *p_w)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_4K_480P_CLOCK_NUM];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E7F: manual: overwritten
	ret = hdmitx_set_tx0_sen(port, p_w[0]);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, HDMITX_SET_4K_480P_CLOCK_NUM);
	// 0x1E6E: if 0x1E7F bit2 = 0b1
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXCORE0_03_ADDR, p_w[1], HDMITX_TXCORE0_03_MASK);
	// 0x1E3D:
	//   5: sel21_l0_t7ckr: down converter clock for 480p
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_VBORX_CLOCK_0_01_ADDR,
						p_w[2],
						HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_MASK);
	// 0x1E3E
	SET_COMMAND(cmd, 2, HDMITX_VIDEOIF_1E3E_ADDR, p_w[3], HDMITX_1E3E_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_4k_480p_clock(system_port port, uint8_t *p_r)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_4K_480P_CLOCK_NUM];
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E7F
	ret = hdmitx_get_tx0_sen(port, &p_r[0]);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, HDMITX_SET_4K_480P_CLOCK_NUM);
	// 0x1E6E
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXCORE0_03_ADDR, 0, HDMITX_TXCORE0_03_MASK);
	// 0x1E3D:
	//   5: sel21_l0_t7ckr: down converter clock for 480p
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_VBORX_CLOCK_0_01_ADDR,
						0,
						HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_MASK);
	// 0x1E3E
	SET_COMMAND(cmd, 2, HDMITX_VIDEOIF_1E3E_ADDR, 0, HDMITX_1E3E_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	p_r[1] = cmd[0].data;
	p_r[2] = cmd[1].data;
	p_r[3] = cmd[2].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_1001_1000_clock(system_port port, uint8_t *p_w)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_4K_480P_1001_1000_NUM];
	system_block_access baccess;
	uint8_t data[HDMITX_SET_4K_480P_1001_1000_MAX_LEN];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_4K_480P_1001_1000_NUM);
	// 0x1E41: RXPLL0EN: manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL0_01_ADDR, p_w[1], HDMITX_RXPLL0_01_MASK);
	// 0x1E45: RXPLL00EN: manual
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_RXPLL00_03_ADDR, p_w[5], HMDITX_RXPLL00_03_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXPLL0_00_ADDR,
						HDMITX_SET_4K_480P_1001_1000_1E40_LEN, data);
	SET_BLOCK_DATA(data, 0, p_w[0]);		// 0x1E40
	SET_BLOCK_DATA(data, 1, p_w[1]);		// 0x1E41
	SET_BLOCK_DATA(data, 2, p_w[2]);		// 0x1E42
	SET_BLOCK_DATA(data, 3, p_w[3]);		// 0x1E43
	SET_BLOCK_DATA(data, 4, p_w[4]);		// 0x1E44
	SET_BLOCK_DATA(data, 5, p_w[5]);		// 0x1E45
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_00_ADDR,
						HDMITX_SET_4K_480P_1001_1000_1E65_LEN, data);
	SET_BLOCK_DATA(data, 0, p_w[6]);		// 0x1E65
	SET_BLOCK_DATA(data, 1, p_w[7]);		// 0x1E66
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_4k_480p_1001_1000_clock(system_port port, uint8_t *p_r)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E40 - 0x1E45
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXPLL0_00_ADDR,
						HDMITX_SET_4K_480P_1001_1000_1E40_LEN,
						p_r);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);
	// 0x1E65 - 0x1E66
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_00_ADDR,
						HDMITX_SET_4K_480P_1001_1000_1E65_LEN,
						&p_r[HDMITX_SET_4K_480P_1001_1000_1E40_LEN]);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_4k_480p_preset(system_port port, uint8_t *p_pktena, uint8_t *p_infena,
											uint8_t *p_vmutecnt, uint8_t *p_txactl1)
{
	system_reg_access access;
	system_command cmd[HDMITX_GET_4K_480P_PRESET_NUM];
	uint32_t ret;

	if (p_pktena == NULL || p_infena == NULL || p_vmutecnt == NULL || p_txactl1 == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_GET_4K_480P_PRESET_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTENA_ADDR, 0, HDMITX_PKTENA_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_INF_INFENA_ADDR, 0, HDMITX_INFENA_MASK);
	SET_COMMAND(cmd, 2, VIDEOIF_VIDEO_VMUTECNT_ADDR, 0, HDMITX_VMUTECNT_MASK);
	SET_COMMAND(cmd, 3, VIDEOIF_AUDIO_TXACTL1_ADDR, 0, HDMITX_TXACTL1_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_pktena = cmd[0].data;
	*p_infena = cmd[1].data;
	*p_vmutecnt = cmd[2].data;
	*p_txactl1 = cmd[3].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_preset(system_port port, uint8_t pktena, uint8_t infena,
											uint8_t vmutecnt, uint8_t txactl1)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_4K_480P_PRESET_NUM];
	uint32_t ret;
	if(update_mode == HDMITX_UPDATE_MODE_DISABLE)
	{
		vmutecnt = 0x10;
	}
	SET_ACCESS(access, &port, cmd, HDMITX_SET_4K_480P_PRESET_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTENA_ADDR, pktena, HDMITX_PKTENA_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_INF_INFENA_ADDR, infena, HDMITX_INFENA_MASK);
	SET_COMMAND(cmd, 2, VIDEOIF_VIDEO_VMUTECNT_ADDR, vmutecnt, HDMITX_VMUTECNT_MASK);
	SET_COMMAND(cmd, 3, VIDEOIF_AUDIO_TXACTL1_ADDR, txactl1, HDMITX_TXACTL1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_crc(system_port port, uint8_t on)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_4K_480P_CRC_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_4K_480P_CRC_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_DEBUG_ENCDBG_06_ADDR,
						(on)
							? HDMITX_ENCDBG_06_CRC_ENA_ENA
							: HDMITX_ENCDBG_06_CRC_ENA_DIS,
						HDMITX_PKTENA_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_crc_expected_value(system_port port, uint8_t *p_exp)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN];
	uint32_t ret;

	if (p_exp == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM2_TXENCSET2_00_ADDR,
						HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN, data);
	SET_BLOCK_DATA(data, 0, p_exp[1]);	// 0x10BB
	SET_BLOCK_DATA(data, 1, p_exp[0]);	// 0x10BC
	SET_BLOCK_DATA(data, 2, p_exp[3]);	// 0x10BD
	SET_BLOCK_DATA(data, 3, p_exp[2]);	// 0x10BE
	SET_BLOCK_DATA(data, 4, p_exp[5]);	// 0x10BF
	SET_BLOCK_DATA(data, 5, p_exp[4]);	// 0x10C0
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_4k_480p_crc_mon_value(system_port port, uint8_t *p_mon)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_mon == NULL) {
		return ERR_NULL_POINTER;
	}
	// 0x3EF8 - 0x3EFD
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_DEBUG_CRCMON_00_ADDR,
						HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN,
						p_mon);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p_sync_delay(system_port port, uint8_t sy_dly, uint8_t sdmode)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_4K_480P_SYNC_DELAY_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_4K_480P_SYNC_DELAY_NUM);
	// 0x2E88
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_GLUE_GLUECTL2_0_ADDR,
						sy_dly,
						HDMITX_GLUECTL2_0_MASK);
	// 0x2E8E
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_GLUE_GLUECTL3_2_ADDR,
						sdmode,
						HDMITX_GLUECTL3_2_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_reset_4k_480p_1001_1000(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMITX_RESET_4K_480P_1001_1000_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_RESET_4K_480P_1001_1000_NUM);
	// 0x1E42: NPDOWNR00: off
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL00_00_ADDR,
						HDMITX_1E42_D0,
						HDMITX_RXPLL00_00_MASK);
	// 0x1E42: NPDOWNR00: on
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_RXPLL00_00_ADDR,
						HDMITX_1E42_D1,
						HDMITX_RXPLL00_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_4k_480p_crc_error(system_port port, uint8_t *p_err)
{
	system_reg_access access;
	system_command cmd[HDMITX_GET_4K_480P_CRC_ERROR_NUM];
	uint32_t ret;

	if (p_err == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_GET_4K_480P_CRC_ERROR_NUM);
	// 0x3EFE: CRC_ERROR
	SET_COMMAND(cmd, 0, VIDEOIF_DEBUG_CRCMON_06_ADDR,
						0,
						HDMITX_CRCMON_06_CRC_ERROR_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_err = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_ctrl_4k_480p_ctrl_crc_expected_value(uint8_t *p_crc, hdmitx_ctrl_crc ctrl)
{
	static hdmitx_expected_crc hdmitx_expected_crc_value[HDMITX_CRC_DETECTED_CNT];
	uint8_t i, j;
	uint8_t idx, cnt;

	if (p_crc == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == HDMITX_CTRL_CRC_GET) {
		idx = 0;
		cnt = hdmitx_expected_crc_value[idx].detected_cnt;
		for (i=idx+1; i<HDMITX_CRC_DETECTED_CNT; i++) {
			if (cnt < hdmitx_expected_crc_value[i].detected_cnt) {
				idx = i;
				cnt = hdmitx_expected_crc_value[i].detected_cnt;
			}
		}
		for (j=0; j<HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN; j++) {
			p_crc[j] = hdmitx_expected_crc_value[idx].detected_crc[j];
		}
	} else if (ctrl == HDMITX_CTRL_CRC_SET) {
		for (i=0; i<HDMITX_CRC_DETECTED_CNT; i++) {
			if (hdmitx_expected_crc_value[i].detected_cnt == 0) {
				for (j=0; j<HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN; j++) {
					hdmitx_expected_crc_value[i].detected_crc[j] = p_crc[j];
				}
				hdmitx_expected_crc_value[i].detected_cnt = hdmitx_expected_crc_value[i].detected_cnt + 1;
				break;
			} else {
				for (j=0; j<HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN; j++) {
					if (hdmitx_expected_crc_value[i].detected_crc[j] != p_crc[j]) {
						break;
					}
				}
				if (j == HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN) {
					hdmitx_expected_crc_value[i].detected_cnt = hdmitx_expected_crc_value[i].detected_cnt + 1;
					break;
				}
			}
		}
	} else if (ctrl == HDMITX_CTRL_CRC_CLEAR) {
		for (i=0; i<HDMITX_CRC_DETECTED_CNT; i++) {
			hdmitx_expected_crc_value[i].detected_cnt = 0;
			for (j=0; j<HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN; j++) {
				hdmitx_expected_crc_value[i].detected_crc[j] = 0;
			}
		}
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_4k_480p_output_seq(system_port port)
{
	uint8_t pktena=0, infena=0, vmutecnt=0, txactl1=0, err=0;
	uint32_t i, j, err_cnt;
	uint8_t crc[HDMITX_SET_4K_480P_CRC_EXPECTED_VALUE_LEN] = {0};
	uint32_t ret;

	// pop PKTENA, INFENA, VMUTECNT, TXACTL1
	ret = hdmitx_get_4k_480p_preset(port, &pktena, &infena, &vmutecnt, &txactl1);
	CHECK_ERROR(ret);
	// set PKTENA, INFENA, VMUTECNT, TXACTL1 for CRC check
	ret = hdmitx_set_4k_480p_preset(port, HDMITX_PKTENA_DEF, HDMITX_INFENA_DEF,
											HDMITX_VMUTECNT_DUMMY, HDMITX_TXACTL1_DEF);
	CHECK_ERROR(ret);
	// set CRC check ON
	ret = hdmitx_set_4k_480p_crc(port, HDMITX_SET_4K_480P_CRC_ON);
	CHECK_ERROR(ret);
	// set CRC expected value for dummy all 0
	ret = hdmitx_set_4k_480p_crc_expected_value(port, crc);
	CHECK_ERROR(ret);
	// set Sync Delay mode
	ret = hdmitx_set_4k_480p_sync_delay(port, HDMITX_GLUECTL2_0_SY_DLY_80,
												HDMITX_GLUECTL3_2_SDMODE_REG_SETUP);
	CHECK_ERROR(ret);
	// clear stored expected value
	ret = hdmitx_ctrl_4k_480p_ctrl_crc_expected_value(crc, HDMITX_CTRL_CRC_CLEAR);
	CHECK_ERROR(ret);
	// detect CRC expected value
	for (i=0; i<HDMITX_4K_480P_OUTPUT_SEQ_DETECT_RETRY_MAX; i++) {
		CHECK_ABORT();
		// 1001/1000 reset
		ret = hdmitx_reset_4k_480p_1001_1000(port);
		CHECK_ERROR(ret);
		for (j=0; j<HDMITX_4K_480P_OUTPUT_SEQ_CRC_CHECK_RETRY_MAX; j++) {
			CHECK_ABORT();
			// wait for 1 frame or more (17msec)
			SLEEP(HDMITX_SLEEP_TIME_17MSEC);
			// get CRC monitor value
			ret = hdmitx_get_4k_480p_crc_mon_value(port, crc);
			CHECK_ERROR(ret);
			// store expected value
			ret = hdmitx_ctrl_4k_480p_ctrl_crc_expected_value(crc, HDMITX_CTRL_CRC_SET);
			CHECK_ERROR(ret);
		}
	}
	// get stored expected value
	ret = hdmitx_ctrl_4k_480p_ctrl_crc_expected_value(crc, HDMITX_CTRL_CRC_GET);
	CHECK_ERROR(ret);
	// set CRC expected value
	ret = hdmitx_set_4k_480p_crc_expected_value(port, crc);

	for (i=0; i<HDMITX_4K_480P_OUTPUT_SEQ_1001_1000_RETRY_MAX; i++) {
		CHECK_ABORT();
		err_cnt = 0;
		// 1001/1000 reset
		ret = hdmitx_reset_4k_480p_1001_1000(port);
		CHECK_ERROR(ret);

		for (j=0; j<HDMITX_4K_480P_OUTPUT_SEQ_CRC_CHECK_RETRY_MAX; j++) {
			CHECK_ABORT();
			// wait for 1 frame or more (17msec)
			SLEEP(HDMITX_SLEEP_TIME_17MSEC);
			// check CRC
			ret = hdmitx_get_4k_480p_crc_error(port, &err);
			CHECK_ERROR(ret);
			if (err == HDMITX_CRCMON_06_CRC_ERROR_NO_ERROR) {
				// no CRC error
				break;
			} else {
				// CRC error
				err_cnt++;
			}
		}
		if (err_cnt == HDMITX_4K_480P_OUTPUT_SEQ_CRC_CHECK_RETRY_MAX) {
			// 480p down conversion output sequence OK
			break;
		}
	}
	if (i == HDMITX_4K_480P_OUTPUT_SEQ_1001_1000_RETRY_MAX) {
		// 480p down conversion output sequence NG
		return ERR_HDMITX_480DS_DOWNCON_SEQ_RETRY;
	}

	// set Sync Delay mode
	ret = hdmitx_set_4k_480p_sync_delay(port, HDMITX_GLUECTL2_0_SY_DLY_81,
												HDMITX_GLUECTL3_2_SDMODE_REG_SETUP);
	CHECK_ERROR(ret);
	// set CRC check OFF
	ret = hdmitx_set_4k_480p_crc(port, HDMITX_SET_4K_480P_CRC_OFF);
	CHECK_ERROR(ret);
	// push PKTENA, INFENA, VMUTECNT, TXACTL1
	ret = hdmitx_set_4k_480p_preset(port, pktena, infena, vmutecnt, txactl1);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_4k_480p_output_seq_reset(system_port port)
{
	uint32_t ret;

	// set Sync Delay mode
	ret = hdmitx_set_4k_480p_sync_delay(port, HDMITX_GLUECTL2_0_SY_DLY_DEF,
												HDMITX_GLUECTL3_2_SDMODE_DEF);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_4k_480p(system_port port, uint8_t *p_cg, uint8_t *p_c, uint8_t *p_1001)
{
	uint8_t w_cg[HDMITX_SET_4K_480P_CLOCK_GATE_V_NUM];
	uint8_t w_c[HDMITX_SET_4K_480P_CLOCK_V_NUM];
	uint8_t w_1001[HDMITX_SET_4K_480P_1001_1000_V_NUM];
	uint32_t ret;

	if (p_cg == NULL || p_c == NULL || p_1001 == NULL) {
		return ERR_NULL_POINTER;
	}

	// pop clock gate
	ret = hdmitx_get_4k_480p_clock_gate(port, p_cg);
	CHECK_ERROR(ret);
	// pop clock
	ret = hdmitx_get_4k_480p_clock(port, p_c);
	CHECK_ERROR(ret);
	// pop 1001/1000 clock
	ret = hdmitx_get_4k_480p_1001_1000_clock(port, p_1001);
	CHECK_ERROR(ret);
	// for hdmitx_set_4k_480p_clock_gate
	w_cg[0] = HDMITX_1E57_40;
	w_cg[1] = HDMITX_1E58_81;
	w_cg[2] = HDMITX_1E59_08;
	w_cg[3] = HDMITX_1E5A_07;
	w_cg[4] = HDMITX_1E5B_00;
	w_cg[5] = HDMITX_1E5C_00;
	w_cg[6] = HDMITX_1E60_80;
	w_cg[7] = HDMITX_1E61_10;
	w_cg[8] = HDMITX_1E62_06;
	// for hdmitx_set_4k_480p_clock
	w_c[0] = HDMITX_CKSELEN_TX0_SEN_MANUAL;
	w_c[1] = HDMITX_TXCORE0_03_44;
	//w_c[2] = HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_480P_ENA;
	w_c[2] = 0x3C;
	w_c[3] = HDMITX_1E3E_18;
	// for hdmitx_set_4k_480p_1001_1000_clock
	w_1001[0] = HDMITX_1E40_83;
	w_1001[1] = HDMITX_1E41_98;
	w_1001[2] = HDMITX_1E42_D1;
	w_1001[3] = HDMITX_1E43_57;
	w_1001[4] = HDMITX_1E44_E9;
	w_1001[5] = HDMITX_1E45_0B;
	w_1001[6] = HDMITX_1E65_18;
	w_1001[7] = HDMITX_1E66_08;
	// set clock gate
	ret = hdmitx_set_4k_480p_clock_gate(port, w_cg);
	CHECK_ERROR(ret);
	// set clock
	ret = hdmitx_set_4k_480p_clock(port, w_c);
	CHECK_ERROR(ret);
	// set 1001/1000 clock
	ret = hdmitx_set_4k_480p_1001_1000_clock(port, w_1001);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	SLEEP(HDMITX_SLEEP_TIME_2MSEC);

	if(update_mode == HDMITX_UPDATE_MODE_DISABLE)
	{
		// Update Register Status
		// CLKUPD, VIFUPD, CSCUPD
		
		ret = system_ctrl_tx_update_reg(port, 0x66);
		CHECK_ERROR(ret);
		CHECK_ABORT();
		// Software Reset
		// VIFSRST, CCSRST, ENCSRST, AVCSRST
		ret = system_ctrl_tx_soft_reset(port, 0x46);
		CHECK_ERROR(ret);

	}
	else
	{
		// Update Register Status
		// CLKUPD, VIFUPD, CSCUPD
		
		ret = system_ctrl_tx_update_reg(port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
												| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
												| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD);
		CHECK_ERROR(ret);
		CHECK_ABORT();
		// Software Reset
		// VIFSRST, CCSRST, ENCSRST, AVCSRST
		ret = system_ctrl_tx_soft_reset(port, SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST
												| SYSTEM_CTRL_TX_SOFT_RESET_CCSRST
												| SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST
												| SYSTEM_CTRL_TX_SOFT_RESET_AVCSRST);
		CHECK_ERROR(ret);
	}
	// output sequence
	ret = hdmitx_4k_480p_output_seq(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_reset_4k_480p(system_port port, uint8_t *p_cg, uint8_t *p_c, uint8_t *p_1001)
{
	uint32_t ret;

	if (p_cg == NULL || p_c == NULL || p_1001 == NULL) {
		return ERR_NULL_POINTER;
	}

	// push clock gate
	ret = hdmitx_set_4k_480p_clock_gate(port, p_cg);
	CHECK_ERROR(ret);
	// push clock
	ret = hdmitx_set_4k_480p_clock(port, p_c);
	CHECK_ERROR(ret);
	// push 1001/1000 clock
	ret = hdmitx_set_4k_480p_1001_1000_clock(port, p_1001);
	CHECK_ERROR(ret);
	// output sequence reset
	ret = hdmitx_4k_480p_output_seq_reset(port);
	CHECK_ERROR(ret);
	CHECK_ABORT();

	if(update_mode == HDMITX_UPDATE_MODE_DISABLE)
	{
		// Update Register Status
		// CLKUPD, VIFUPD, CSCUPD
		
		ret = system_ctrl_tx_update_reg(port, 0x66);
		CHECK_ERROR(ret);
		CHECK_ABORT();
		// Software Reset
		// VIFSRST, CCSRST, ENCSRST, AVCSRST
		ret = system_ctrl_tx_soft_reset(port, 0x46);
		CHECK_ERROR(ret);

	}
	else
	{
		// Update Register Status
		// CLKUPD, VIFUPD, CSCUPD
		ret = system_ctrl_tx_update_reg(port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
												| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
												| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD);
		CHECK_ERROR(ret);
		CHECK_ABORT();
		// Software Reset
		// VIFSRST, CCSRST, ENCSRST, AVCSRST
		ret = system_ctrl_tx_soft_reset(port, SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST
												| SYSTEM_CTRL_TX_SOFT_RESET_CCSRST
												| SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST
												| SYSTEM_CTRL_TX_SOFT_RESET_AVCSRST);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint8_t hdmitx_is_vborx_2k_to_480p(hdmitx_video *p_video)
{
	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// VBO-Rx: 1080p 60Hz
	// HDMI-Tx: 480p 60Hz
	if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_VBO) {
		if ((p_video->rx_video_format == SYSTEM_VFORM_1920x1080P_5994or60_16v9)
			&& (p_video->tx_video_format == SYSTEM_VFORM_720x480P_5994or60_4v3)) {
			return 1;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_2k_480p_clock1(system_port port, uint8_t *p_w)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_CLOCK1_NUM];
	system_block_access baccess;
	uint8_t data[HDMITX_SET_2K_480P_CLOCK1_1E58_LEN];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E7F: manual: overwritten
	ret = hdmitx_set_tx0_sen(port, p_w[0]);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_CLOCK1_NUM);
	// 0x1E62
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_PADEN_ADDR, p_w[1], HDMITX_PADEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, HDMITX_VIDEOIF_1E58_ADDR,
						HDMITX_SET_2K_480P_CLOCK1_1E58_LEN, data);
	SET_BLOCK_DATA(data, 0, p_w[2]);		// 0x1E58
	SET_BLOCK_DATA(data, 1, p_w[3]);		// 0x1E59
	SET_BLOCK_DATA(data, 2, p_w[4]);		// 0x1E5A
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_2k_480p_clock1(system_port port, uint8_t *p_r)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_CLOCK1_NUM];
	system_block_access baccess;
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E7F
	ret = hdmitx_get_tx0_sen(port, &p_r[0]);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_CLOCK1_NUM);
	// 0x1E62
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_PADEN_ADDR, 0, HDMITX_PADEN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	p_r[1] = cmd[0].data;

	// 0x1E58 - 0x1E5A
	SET_BLOCK_ACCESS(baccess, &port, HDMITX_VIDEOIF_1E58_ADDR,
						HDMITX_SET_2K_480P_CLOCK1_1E58_LEN,
						&p_r[HDMITX_SET_TX0_SEN_NUM + HDMITX_SET_2K_480P_CLOCK1_NUM]);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_2k_480p_1001_1000_clock(system_port port, uint8_t *p_w)
{
	return hdmitx_set_4k_480p_1001_1000_clock(port, p_w);
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_2k_480p_1001_1000_clock(system_port port, uint8_t *p_r)
{
	return hdmitx_get_4k_480p_1001_1000_clock(port, p_r);
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_2k_480p_dc_clock(system_port port, uint8_t *p_w)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_DC_CLOCK_NUM];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_DC_CLOCK_NUM);
	// 0x1E3D:
	//   5: sel21_l0_t7ckr: down converter clock for 480p
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_VBORX_CLOCK_0_01_ADDR,
						p_w[0],
						HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_MASK);
	// 0x1E3E
	SET_COMMAND(cmd, 1, HDMITX_VIDEOIF_1E3E_ADDR, p_w[1], HDMITX_1E3E_MASK);
	// 0x1E6D
	SET_COMMAND(cmd, 2, VIDEOIF_CKGEN_TXCORE0_02_ADDR, p_w[2], HDMITX_1E6D_MASK);
	// 0x1E6B
	SET_COMMAND(cmd, 3, VIDEOIF_CKGEN_TXCORE0_00_ADDR, p_w[3], HDMITX_1E6B_MASK);
	// 0x1E47
	SET_COMMAND(cmd, 4, VIDEOIF_CKGEN_TXPLL00_01_ADDR, p_w[4], HDMITX_1E47_MASK);
	// 0x1E6F
	SET_COMMAND(cmd, 5, VIDEOIF_CKGEN_TXCORE0_04_ADDR, p_w[5], HDMITX_1E6F_MASK);
	// 0x1E70
	SET_COMMAND(cmd, 6, VIDEOIF_CKGEN_TXCORE0_05_ADDR, p_w[6], HDMITX_1E70_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}

static uint32_t hdmitx_TMDS_clock_en(system_port port,uint32_t en)
{
	system_reg_access access;
	system_command cmd[2];
	uint32_t ret;


	SET_ACCESS(access, &port, cmd, 2);

	if(en == 0)
	{
		// 0x1E5B
		SET_COMMAND(cmd, 0, 0x00001E5B, 0x20, 0xff);  //no found in SPEC
		// 0x1E00
		SET_COMMAND(cmd, 1, 0x00001E00, 0x66, 0xff);
	}
	else if(en == 1)
	{
		// 0x1E5B
		SET_COMMAND(cmd, 0, 0x00001E5B, 0x00, 0xff);  //no found in SPEC
		// 0x1E00
		SET_COMMAND(cmd, 1, 0x00001E00, 0x67, 0xff);

	}

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);
	SLEEP(10);

	return ret;
}


static uint32_t hdmitx_set_4k_clock(system_port port)
{
	system_reg_access access;
	system_command cmd[7];
	uint32_t ret;


	SET_ACCESS(access, &port, cmd, 6);

	// 0x1E6B
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXCORE0_00_ADDR, 0x64, HDMITX_1E6B_MASK);
	// 0x1E6C
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_TXCORE0_01_ADDR, 0x30, HDMITX_1E6B_MASK);
	// 0x1E6D
	SET_COMMAND(cmd, 2, VIDEOIF_CKGEN_TXCORE0_02_ADDR, 0x00, HDMITX_1E6D_MASK);
	// 0x1E6E
	SET_COMMAND(cmd, 3, VIDEOIF_CKGEN_TXCORE0_03_ADDR, 0x40, HDMITX_1E6F_MASK);
	// 0x1E6F
	SET_COMMAND(cmd, 4, VIDEOIF_CKGEN_TXCORE0_04_ADDR, 0x00, HDMITX_1E6F_MASK);
	// 0x1E70
	SET_COMMAND(cmd, 5, VIDEOIF_CKGEN_TXCORE0_05_ADDR, 0x92, HDMITX_1E70_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}

static uint32_t hdmitx_set_1080p60_clock(system_port port)
{
	system_reg_access access;
	system_command cmd[7];
	uint32_t ret;


	SET_ACCESS(access, &port, cmd, 6);

	// 0x1E6B
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXCORE0_00_ADDR, 0xe4, HDMITX_1E6B_MASK);
	// 0x1E6C
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_TXCORE0_01_ADDR, 0x30, 0xff);
	// 0x1E6D
	SET_COMMAND(cmd, 2, VIDEOIF_CKGEN_TXCORE0_02_ADDR, 0x00, HDMITX_1E6D_MASK);
	// 0x1E6E
	SET_COMMAND(cmd, 3, VIDEOIF_CKGEN_TXCORE0_03_ADDR, 0x44, 0xff);
	// 0x1E6F
	SET_COMMAND(cmd, 4, VIDEOIF_CKGEN_TXCORE0_04_ADDR, 0x02, HDMITX_1E6F_MASK);
	// 0x1E70
	SET_COMMAND(cmd, 5, VIDEOIF_CKGEN_TXCORE0_05_ADDR, 0x92, HDMITX_1E70_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;

}



//------------------------------------------------------------------------------
static uint32_t hdmitx_get_2k_480p_dc_clock(system_port port, uint8_t *p_r)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_DC_CLOCK_NUM];
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_DC_CLOCK_NUM);
	// 0x1E3D:
	//   5: sel21_l0_t7ckr: down converter clock for 480p
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_VBORX_CLOCK_0_01_ADDR,
						0,
						HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_MASK);
	// 0x1E3E
	SET_COMMAND(cmd, 1, HDMITX_VIDEOIF_1E3E_ADDR, 0, HDMITX_1E3E_MASK);
	// 0x1E6D
	SET_COMMAND(cmd, 2, VIDEOIF_CKGEN_TXCORE0_02_ADDR, 0, HDMITX_1E6D_MASK);
	// 0x1E6B
	SET_COMMAND(cmd, 3, VIDEOIF_CKGEN_TXCORE0_00_ADDR, 0, HDMITX_1E6B_MASK);
	// 0x1E47
	SET_COMMAND(cmd, 4, VIDEOIF_CKGEN_TXPLL00_01_ADDR, 0, HDMITX_1E47_MASK);
	// 0x1E6F
	SET_COMMAND(cmd, 5, VIDEOIF_CKGEN_TXCORE0_04_ADDR, 0, HDMITX_1E6F_MASK);
	// 0x1E70
	SET_COMMAND(cmd, 6, VIDEOIF_CKGEN_TXCORE0_05_ADDR, 0, HDMITX_1E70_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	p_r[0] = cmd[0].data;
	p_r[1] = cmd[1].data;
	p_r[2] = cmd[2].data;
	p_r[3] = cmd[3].data;
	p_r[4] = cmd[4].data;
	p_r[5] = cmd[5].data;
	p_r[6] = cmd[6].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_2k_480p_clock2(system_port port, uint8_t *p_w)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_CLOCK2_NUM];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_CLOCK2_NUM);
	// 0x1E3E
	SET_COMMAND(cmd, 0, HDMITX_VIDEOIF_1E3E_ADDR, p_w[0], HDMITX_1E3E_MASK);
	// 0x1E6E
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_TXCORE0_03_ADDR, p_w[1], HDMITX_1E6E_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_2k_480p_clock2(system_port port, uint8_t *p_r)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_CLOCK2_NUM];
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_CLOCK2_NUM);
	// 0x1E3E
	SET_COMMAND(cmd, 0, HDMITX_VIDEOIF_1E3E_ADDR, 0, HDMITX_1E3E_MASK);
	// 0x1E6E
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_TXCORE0_03_ADDR, 0, HDMITX_1E6E_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	p_r[0] = cmd[0].data;
	p_r[1] = cmd[1].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_2k_480p_pll01(system_port port, uint8_t *p_w)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_2K_480P_PLL01_NUM];
	system_block_access baccess;
	uint8_t data[HDMITX_SET_2K_480P_PLL01_1E48_LEN];
	uint32_t ret;

	if (p_w == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMITX_SET_2K_480P_PLL01_NUM);
	// 0x1E49:
	//   7: TXPLL01EN: TXPLL01 manual mode enable
	//                 when only TXPLL01EN=1, 0x1E48 bit3-0 and 0x1E49 bit6-0 are writable
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXPLL01_01_ADDR,
						HDMITX_TXPLL01_01_TXPLL01EN,
						HDMITX_1E49_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_TXPLL01_00_ADDR,
						HDMITX_SET_2K_480P_PLL01_1E48_LEN, data);
	SET_BLOCK_DATA(data, 0, p_w[0]);		// 0x1E48
	SET_BLOCK_DATA(data, 1, p_w[1]);		// 0x1E49
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_2k_480p_pll01(system_port port, uint8_t *p_r)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_r == NULL) {
		return ERR_NULL_POINTER;
	}

	// 0x1E48 - 0x1E49
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_TXPLL01_00_ADDR,
						HDMITX_SET_2K_480P_PLL01_1E48_LEN,
						p_r);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_2k_480p_output_seq(system_port port)
{
	return hdmitx_4k_480p_output_seq(port);
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_2k_480p_output_seq_reset(system_port port)
{
	return hdmitx_4k_480p_output_seq_reset(port);
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_2k_480p(system_port port, uint8_t *p_c1, uint8_t *p_1001,
										uint8_t *p_dc, uint8_t *p_c2, uint8_t *p_pll01)
{
	uint8_t w_c1[HDMITX_SET_2K_480P_CLOCK1_V_NUM];
	uint8_t w_1001[HDMITX_SET_2K_480P_1001_1000_V_NUM];
	uint8_t w_dc[HDMITX_SET_2K_480P_DC_CLOCK_V_NUM];
	uint8_t w_c2[HDMITX_SET_2K_480P_CLOCK2_V_NUM];
	uint8_t w_pll01[HDMITX_SET_2K_480P_PLL01_V_NUM];
	uint32_t ret;

	if (p_c1 == NULL || p_1001 == NULL || p_dc == NULL || p_c2 == NULL || p_pll01 == NULL) {
		return ERR_NULL_POINTER;
	}

	// pop clock1
	ret = hdmitx_get_2k_480p_clock1(port, p_c1);
	CHECK_ERROR(ret);
	// pop 1001/1000 clock
	ret = hdmitx_get_2k_480p_1001_1000_clock(port, p_1001);
	CHECK_ERROR(ret);
	// pop dc clock
	ret = hdmitx_get_2k_480p_dc_clock(port, p_dc);
	CHECK_ERROR(ret);
	// pop clock2
	ret = hdmitx_get_2k_480p_clock2(port, p_c2);
	CHECK_ERROR(ret);
	// pop pll01
	ret = hdmitx_get_2k_480p_pll01(port, p_pll01);
	CHECK_ERROR(ret);
	// for hdmitx_set_2k_480p_clock1
	w_c1[0] = HDMITX_CKSELEN_TX0_SEN_MANUAL;
	w_c1[1] = HDMITX_1E62_06;
	w_c1[2] = HDMITX_1E58_81;
	w_c1[3] = HDMITX_1E59_08;
	w_c1[4] = HDMITX_1E5A_07;
	// for hdmitx_set_2k_480p_1001_1000_clock
	w_1001[0] = HDMITX_1E40_83;
	w_1001[1] = HDMITX_1E41_98;
	w_1001[2] = HDMITX_1E42_D1;
	w_1001[3] = HDMITX_1E43_57;
	w_1001[4] = HDMITX_1E44_E9;
	w_1001[5] = HDMITX_1E45_0B;
	w_1001[6] = HDMITX_1E65_18;
	w_1001[7] = HDMITX_1E66_32;
	// for hdmitx_set_2k_480p_dc_clock
	w_dc[0] = HDMITX_VBORX_CLOCK_0_01_SEL21_L0_T7CKR_480P_ENA;
	w_dc[1] = HDMITX_1E3E_07;
	w_dc[2] = HDMITX_1E6D_20;
	w_dc[3] = HDMITX_1E6B_4C;
	w_dc[4] = HDMITX_1E47_20;
	w_dc[5] = HDMITX_1E6F_02;
	w_dc[6] = HDMITX_1E70_92;
	// for hdmitx_set_2k_480p_clock2
	w_c2[0] = HDMITX_1E3E_1F;
	w_c2[1] = HDMITX_1E6E_CC;
	// for hdmitx_set_2k_480p_pll01
	w_pll01[0] = HDMITX_1E48_83;
	w_pll01[1] = HDMITX_1E49_90;
	// set clock1
	ret = hdmitx_set_2k_480p_clock1(port, w_c1);
	CHECK_ERROR(ret);
	// set 1001/1000 clock
	ret = hdmitx_set_2k_480p_1001_1000_clock(port, w_1001);
	CHECK_ERROR(ret);
	// set dc clock
	ret = hdmitx_set_2k_480p_dc_clock(port, w_dc);
	CHECK_ERROR(ret);
	// set clock2
	ret = hdmitx_set_2k_480p_clock2(port, w_c2);
	CHECK_ERROR(ret);
	// set pll01
	ret = hdmitx_set_2k_480p_pll01(port, w_pll01);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	SLEEP(HDMITX_SLEEP_TIME_2MSEC);
	// Update Register Status
	// CLKUPD, VIFUPD, CSCUPD
	ret = system_ctrl_tx_update_reg(port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
											| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
											| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Software Reset
	// VIFSRST, CCSRST, ENCSRST, AVCSRST
	ret = system_ctrl_tx_soft_reset(port, SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_CCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_AVCSRST);
	CHECK_ERROR(ret);
	// output sequence
	ret = hdmitx_2k_480p_output_seq(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_reset_2k_480p(system_port port, uint8_t *p_c1, uint8_t *p_1001,
										uint8_t *p_dc, uint8_t *p_c2, uint8_t *p_pll01)
{
	uint32_t ret;

	if (p_c1 == NULL || p_1001 == NULL || p_dc == NULL || p_c2 == NULL || p_pll01 == NULL) {
		return ERR_NULL_POINTER;
	}

	// push clock1
	ret = hdmitx_set_2k_480p_clock1(port, p_c1);
	CHECK_ERROR(ret);
	// push 1001/1000 clock
	ret = hdmitx_set_2k_480p_1001_1000_clock(port, p_1001);
	CHECK_ERROR(ret);
	// push dc clock
	ret = hdmitx_set_2k_480p_dc_clock(port, p_dc);
	CHECK_ERROR(ret);
	// push clock2
	ret = hdmitx_set_2k_480p_clock2(port, p_c2);
	CHECK_ERROR(ret);
	// push pll01
	ret = hdmitx_set_2k_480p_pll01(port, p_pll01);
	CHECK_ERROR(ret);
	// output sequence reset
	ret = hdmitx_2k_480p_output_seq_reset(port);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Update Register Status
	// CLKUPD, VIFUPD, CSCUPD
	ret = system_ctrl_tx_update_reg(port, SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
											| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
											| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Software Reset
	// VIFSRST, CCSRST, ENCSRST, AVCSRST
	ret = system_ctrl_tx_soft_reset(port, SYSTEM_CTRL_TX_SOFT_RESET_VIFSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_CCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_ENCSRST
											| SYSTEM_CTRL_TX_SOFT_RESET_AVCSRST);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static void hdmitx_init_reg_stack(uint8_t *p_s, uint8_t size)
{
	int i;
	for (i=0; i<size; i++) {
		p_s[i] = HDMITX_REG_STACK_INIT_VAL;
	}
}
//------------------------------------------------------------------------------
static uint8_t hdmitx_is_reg_stacked(uint8_t *p_s, uint8_t size)
{
	int i;
	for (i=0; i<size; i++) {
		if (p_s[i] != HDMITX_REG_STACK_INIT_VAL) {
			return 1;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_vborx_comb(hdmitx_video *p_video, uint8_t tx0_sen, uint8_t *p_allupd)
{
	static uint8_t s_4k_cg[HDMITX_SET_4K_480P_CLOCK_GATE_V_NUM] = { 0 };
	static uint8_t s_4k_c[HDMITX_SET_4K_480P_CLOCK_V_NUM] = { 0 };
	static uint8_t s_4k_1001[HDMITX_SET_4K_480P_1001_1000_V_NUM] = { 0 };
	static uint8_t s_2k_c1[HDMITX_SET_2K_480P_CLOCK1_V_NUM] = { 0 };
	static uint8_t s_2k_1001[HDMITX_SET_2K_480P_1001_1000_V_NUM] = { 0 };
	static uint8_t s_2k_dc[HDMITX_SET_2K_480P_DC_CLOCK_V_NUM] = { 0 };
	static uint8_t s_2k_c2[HDMITX_SET_2K_480P_CLOCK2_V_NUM] = { 0 };
	static uint8_t s_2k_pll01[HDMITX_SET_2K_480P_PLL01_V_NUM] = { 0 };
	uint32_t ret;

	if (p_video == NULL || p_allupd == NULL) {
		return ERR_NULL_POINTER;
	}

	*p_allupd = SYSTEM_CTRL_TX_UPDATE_REG_ALLUPD;

	// 720p 25/30/50/60Hz, 1366x768 60Hz(RB), 1080I 50/60Hz, 1080P 24/25/30Hz
	// when VBO Rx 1Lane - HDMI Tx bridge
	if (hdmitx_is_vborx_720p(p_video)) {
		ret = hdmitx_set_720p(p_video);
		CHECK_ERROR(ret);
		// NOTE: 0x3054 INREP[1:0] is updated with 0x00 though is not update
		//       when ALLUPD and VIFUPD are set in 0x3011
		*p_allupd = SYSTEM_CTRL_TX_UPDATE_REG_NONE;
	} else {
		ret = hdmitx_reset_720p(p_video->tx_port);
		CHECK_ERROR(ret);
	}
	// 4K(2160p) to 480p
	if (hdmitx_is_vborx_4k_to_480p(p_video)) {
		ret = hdmitx_set_4k_480p(p_video->tx_port, s_4k_cg, s_4k_c, s_4k_1001);
		CHECK_ERROR(ret);
	} else {
		if (hdmitx_is_reg_stacked(s_4k_cg, HDMITX_SET_4K_480P_CLOCK_GATE_V_NUM)
			|| hdmitx_is_reg_stacked(s_4k_c, HDMITX_SET_4K_480P_CLOCK_V_NUM)
			|| hdmitx_is_reg_stacked(s_4k_1001, HDMITX_SET_4K_480P_1001_1000_V_NUM)) {
			s_4k_c[0] = tx0_sen;
			ret = hdmitx_reset_4k_480p(p_video->tx_port, s_4k_cg, s_4k_c, s_4k_1001);
			CHECK_ERROR(ret);
			hdmitx_init_reg_stack(s_4k_cg, HDMITX_SET_4K_480P_CLOCK_GATE_V_NUM);
			hdmitx_init_reg_stack(s_4k_c, HDMITX_SET_4K_480P_CLOCK_V_NUM);
			hdmitx_init_reg_stack(s_4k_1001, HDMITX_SET_4K_480P_1001_1000_V_NUM);
		}
	}
	// 2K(1080p) to 480p
	if (hdmitx_is_vborx_2k_to_480p(p_video)) {
		ret = hdmitx_set_2k_480p(p_video->tx_port, s_2k_c1, s_2k_1001,
									s_2k_dc, s_2k_c2, s_2k_pll01);
		CHECK_ERROR(ret);
	} else {
		if (hdmitx_is_reg_stacked(s_2k_c1, HDMITX_SET_2K_480P_CLOCK1_V_NUM)
			|| hdmitx_is_reg_stacked(s_2k_1001, HDMITX_SET_2K_480P_1001_1000_V_NUM)
			|| hdmitx_is_reg_stacked(s_2k_dc, HDMITX_SET_2K_480P_DC_CLOCK_V_NUM)
			|| hdmitx_is_reg_stacked(s_2k_c2, HDMITX_SET_2K_480P_CLOCK2_V_NUM)
			|| hdmitx_is_reg_stacked(s_2k_pll01, HDMITX_SET_2K_480P_PLL01_V_NUM)) {
			s_2k_c1[0] = tx0_sen;
			ret = hdmitx_reset_2k_480p(p_video->tx_port, s_2k_c1, s_2k_1001,
										s_2k_dc, s_2k_c2, s_2k_pll01);
			CHECK_ERROR(ret);
			hdmitx_init_reg_stack(s_2k_c1, HDMITX_SET_2K_480P_CLOCK1_V_NUM);
			hdmitx_init_reg_stack(s_2k_1001, HDMITX_SET_2K_480P_1001_1000_V_NUM);
			hdmitx_init_reg_stack(s_2k_dc, HDMITX_SET_2K_480P_DC_CLOCK_V_NUM);
			hdmitx_init_reg_stack(s_2k_c2, HDMITX_SET_2K_480P_CLOCK2_V_NUM);
			hdmitx_init_reg_stack(s_2k_pll01, HDMITX_SET_2K_480P_PLL01_V_NUM);
		}
	}

	return ret;
}
#endif
//------------------------------------------------------------------------------
static uint32_t hdmitx_sg_setting(hdmitx_video *p_video, system_sg *p_sg)
{
	system_reg_access access;
	system_command cmd[HDMITX_SG_SETTING_NUM];
	uint8_t sysclk, tstgen0, osdvsrc0, dumon, vmute;
	uint32_t ret;

	if (p_video == NULL || p_sg == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_sg->onoff == SYSTEM_ONOFF_OFF) {
		sysclk = HDMITX_SCLKSEL_NOR;
		tstgen0 = HDMITX_TCLKSET0_TSTGEN0_NOR;
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H27BJ2)
		// 720p 25/30/50/60Hz, 1366x768 60Hz(RB), 1080I 50/60Hz, 1080P 24/25/30Hz
		// when VBO Rx 1Lane - HDMI Tx bridge
		if (hdmitx_is_vborx_720p(p_video)) {
			osdvsrc0 = HDMITX_SWMODE4_01_OSDVSRC0_TXVIDEO0;
		} else {
			osdvsrc0 = HDMITX_SWMODE4_01_OSDVSRC0_RXCC0;
		}
#else
		osdvsrc0 = HDMITX_SWMODE4_01_OSDVSRC0_RXCC0;
#endif
		dumon = HDMITX_VMUTECNT_DUMON_NOR;
		vmute = HDMITX_VMUTECNT_VMUTE_NOR;
	} else if (p_sg->onoff == SYSTEM_ONOFF_ON) {
		sysclk = HDMITX_SCLKSEL_SYSCLK;
		switch (p_video->tx_video_format) {
			case SYSTEM_VFORM_720x480P_5994or60_4v3:
			case SYSTEM_VFORM_720x480P_5994or60_16v9:
				tstgen0 = HDMITX_TCLKSET0_TSTGEN0_SYSCLK_16_16;	break;
			case SYSTEM_VFORM_1280x720P_50_16v9:
			case SYSTEM_VFORM_1280x720P_5994or60_16v9:
			case SYSTEM_VFORM_1920x1080P_2398or24_16v9:
			case SYSTEM_VFORM_1920x1080P_25_16v9:
			case SYSTEM_VFORM_1920x1080P_2997or30_16v9:
				tstgen0 = HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_8;	break;
			case SYSTEM_VFORM_1920x1080P_50_16v9:
			case SYSTEM_VFORM_1920x1080P_5994or60_16v9:
				tstgen0 = HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_4;	break;
			case SYSTEM_VFORM_3840x2160P_2398or24_16v9:
			case SYSTEM_VFORM_3840x2160P_25_16v9:
			case SYSTEM_VFORM_3840x2160P_2997or30_16v9:
			case SYSTEM_VFORM_4096x2160P_2398or24_256v135:
			case SYSTEM_VFORM_4096x2160P_25_256v135:
			case SYSTEM_VFORM_4096x2160P_2997or30_256v135:
				tstgen0 = HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_2;	break;
			case SYSTEM_VFORM_3840x2160P_50_16v9:
			case SYSTEM_VFORM_3840x2160P_5994or60_16v9:
			case SYSTEM_VFORM_4096x2160P_50_256v135:
			case SYSTEM_VFORM_4096x2160P_5994or60_256v135:
				tstgen0 = HDMITX_TCLKSET0_TSTGEN0_SYSCLK_22_1;	break;
			default:											return ERR_HDMITX_VIDEO_FORMAT;
		}
		osdvsrc0 = HDMITX_SWMODE4_01_OSDVSRC0_TXVIDEO0;
		dumon = HDMITX_VMUTECNT_DUMON_DUMMY;
		switch (p_sg->sg_format) {
			case SYSTEM_SG_FORMAT_NONE:
				vmute = HDMITX_VMUTECNT_VMUTE_NOR;				break;
			case SYSTEM_SG_FORMAT_ASYNC_BLANK:
				vmute = HDMITX_VMUTECNT_VMUTE_ASYNC_BLANK;		break;
			case SYSTEM_SG_FORMAT_BLANK:
				vmute = HDMITX_VMUTECNT_VMUTE_BLANK;			break;
			case SYSTEM_SG_FORMAT_RAMP_A:
				vmute = HDMITX_VMUTECNT_VMUTE_RAMP_A;			break;
			case SYSTEM_SG_FORMAT_RAMP_B:
				vmute = HDMITX_VMUTECNT_VMUTE_RAMP_B;			break;
			case SYSTEM_SG_FORMAT_TOGGLE:
				vmute = HDMITX_VMUTECNT_VMUTE_TOGGLE;			break;
			case SYSTEM_SG_FORMAT_COLOR_BAR_RGB:
				vmute = HDMITX_VMUTECNT_VMUTE_COLOR_BAR_RGB;	break;
			case SYSTEM_SG_FORMAT_COLOR_BAR_444:
				vmute = HDMITX_VMUTECNT_VMUTE_COLOR_BAR_444;	break;
			default:											return ERR_NG;
		}
	} else {
		return ERR_NG;
	}

	SET_ACCESS(access, &(p_video->tx_port), cmd, HDMITX_SG_SETTING_NUM);
	// select SYSCLK
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_VSELECT_00_ADDR, sysclk, HDMITX_SCLKSEL_MASK);
	// set TMDS clock setting when SYSCLK enabled
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_TCLKSET0_ADDR, tstgen0, HDMITX_TCLKSET0_TSTGEN0_MASK);
	// select OSD0 video source
	SET_COMMAND(cmd, 2, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR, osdvsrc0, HDMITX_SWMODE4_01_OSDVSRC0_MASK);
	// dummy sync transmit on/off
	// video mute source
	SET_COMMAND(cmd, 3, VIDEOIF_VIDEO_VMUTECNT_ADDR,
						dumon | vmute,
						HDMITX_VMUTECNT_DUMON_MASK | HDMITX_VMUTECNT_VMUTE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_copy_video_param(system_video *dest, hdmitx_video *src)
{
	if (dest == NULL || src == NULL) {
		return ERR_NULL_POINTER;
	}

	dest->rx_port         = src->rx_port;
	dest->tx_port         = src->tx_port;
	dest->rx_video_format = src->rx_video_format;
	dest->rx_color_format = src->rx_color_format;
	dest->rx_deep_color   = src->rx_deep_color;
	dest->tx_video_format = src->tx_video_format;
	dest->tx_color_format = src->tx_color_format;
	dest->tx_deep_color   = src->tx_deep_color;

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint8_t hdmitx_is_dvi_mode(hdmitx_dvi_mode mode)
{
	if (mode == HDMITX_DVI_MODE_ENABLE) {
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_txmstctl(system_port port, uint8_t mode, uint8_t page)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_TXMSTCTL_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_TXMSTCTL_NUM);

	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXMSTCTL_ADDR,
						mode
						| page,
						HDMITX_TXMSTCTL_MSTCBMODE_MASK
						| HDMITX_TXMSTCTL_CBPAGE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_get_down_over_sf(system_audio_sf sf, system_audio_sf_ratio ratio,
										system_audio_sf *p_sf)
{
	if (p_sf == NULL) {
		return ERR_NULL_POINTER;
	}

	switch (sf) {
		case SYSTEM_AUDIO_SF_32KHZ:						//   32  kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_32KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_2_1:	*p_sf = SYSTEM_AUDIO_SF_64KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_4_1:	*p_sf = SYSTEM_AUDIO_SF_128KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_44KHZ:						//   44.1kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_44KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_2_1:	*p_sf = SYSTEM_AUDIO_SF_88KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_4_1:	*p_sf = SYSTEM_AUDIO_SF_176KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_48KHZ:						//   48  kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_48KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_2_1:	*p_sf = SYSTEM_AUDIO_SF_96KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_4_1:	*p_sf = SYSTEM_AUDIO_SF_192KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_88KHZ:						//   88.2kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_2:	*p_sf = SYSTEM_AUDIO_SF_44KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_88KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_2_1:	*p_sf = SYSTEM_AUDIO_SF_176KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_96KHZ:						//   96  kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_2:	*p_sf = SYSTEM_AUDIO_SF_48KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_96KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_2_1:	*p_sf = SYSTEM_AUDIO_SF_192KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_176KHZ:					//  176.4kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_4:	*p_sf = SYSTEM_AUDIO_SF_44KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_2:	*p_sf = SYSTEM_AUDIO_SF_88KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_176KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_192KHZ:					//  192  kHz (AS, OBA, DST)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_4:	*p_sf = SYSTEM_AUDIO_SF_48KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_2:	*p_sf = SYSTEM_AUDIO_SF_96KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_192KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_64KHZ:						//   64  kHz (AS w/ 3D/Multi-Stream)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_2:	*p_sf = SYSTEM_AUDIO_SF_32KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_64KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_2_1:	*p_sf = SYSTEM_AUDIO_SF_128KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		case SYSTEM_AUDIO_SF_128KHZ:					//  128  kHz (AS w/ 3D/Multi-Stream)
			switch (ratio) {
				case SYSTEM_AUDIO_SF_RATIO_1_4:	*p_sf = SYSTEM_AUDIO_SF_32KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_2:	*p_sf = SYSTEM_AUDIO_SF_64KHZ;	break;
				case SYSTEM_AUDIO_SF_RATIO_1_1:	*p_sf = SYSTEM_AUDIO_SF_128KHZ;	break;
				default:						return ERR_HDMITX_AUDIO_FORMAT;
			}
			break;
		default:
			return ERR_HDMITX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_audio_cbit(hdmitx_audio *p_audio)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_AUDIO_CBIT_LEN];
	system_audio_sf sf;
	uint8_t hbr, txfs, adbit;
	uint32_t ret;

	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	// set mode and page of cbit
	// page(0x70b0): 0
	ret = hdmitx_set_txmstctl(p_audio->port, HDMITX_TXMSTCTL_MSTCBMODE_ALL_ST0,
								HDMITX_TXMSTCTL_CBPAGE_ST0);
	CHECK_ERROR(ret);
	// set channel status bit
	SET_BLOCK_ACCESS(baccess, &(p_audio->port), VIDEOIF_AUDIO_TXCBIT_ST0_00_ADDR,
						HDMITX_SET_AUDIO_CBIT_LEN, data);
	// MODE, EMPHASIS, COPYRIGHT, LPCM, CBIT_0
	hbr = system_ctrl_is_audio_hbr(p_audio->cb_sampling_freq);
	SET_BLOCK_DATA(data, 0, HDMITX_TXCBIT_MODE_0
							| HDMITX_TXCBIT_EMPHASIS_2CH_WO_PRE
							| HDMITX_TXCBIT_COPYRIGHT
							| (hbr) ? HDMITX_TXCBIT_LPCM_OTHER : HDMITX_TXCBIT_LPCM_LPCM
							| HDMITX_TXCBIT_CBIT_0_CONSUMER);			// 0x30B0
	// CATEGORY
	SET_BLOCK_DATA(data, 1, HDMITX_TXCBIT_CATEGORY_DEF);				// 0x30B1
	// SOURCE_NUM, CH_NUMBER
	SET_BLOCK_DATA(data, 2, HDMITX_TXCBIT_SOURCE_NUM_DEF
							| HDMITX_TXCBIT_CH_NUMBER_DEF);				// 0x30B2
	// TXFS_EXT, CLKACCR, TXFS
	// get down-/over-sampling frequence
	ret = hdmitx_get_down_over_sf(p_audio->cb_sampling_freq, p_audio->sampling_freq_ratio, &sf);
	CHECK_ERROR(ret);
	ret = system_ctrl_audio_sf2cbit(sf, &txfs);
	CHECK_ERROR(ret);
	SET_BLOCK_DATA(data, 3, HDMITX_TXCBIT_TXFS_EXT_DEF
							| HDMITX_TXCBIT_CLKACCR_LV2
							| txfs);									// 0x30B3
	// ORGFS, ADBIT
	ret = system_ctrl_audio_ss2cbit(p_audio->cb_sample_size, &adbit);
	CHECK_ERROR(ret);
	SET_BLOCK_DATA(data, 4, HDMITX_TXCBIT_ORGFS_DEF
							| adbit);									// 0x30B4
	// TXFS_COEF, CBIT_43, CGMS_A_VAL, CGMS_A
	SET_BLOCK_DATA(data, 5, HDMITX_TXCBIT_5_DEF);						// 0x30B5
	// CBIT_BYTE6
	SET_BLOCK_DATA(data, 6, HDMITX_TXCBIT_6_DEF);						// 0x30B6
	// CBIT_BYTE7
	SET_BLOCK_DATA(data, 7, HDMITX_TXCBIT_7_DEF);						// 0x30B7
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_ctrl_req_abort(system_request *p_req, hdmitx_ctrl_req ctrl)
{
	static system_request hdmitx_abort_req = SYSTEM_REQUEST_NONE;

	if (p_req == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == HDMITX_CTRL_REQ_GET) {
		*p_req =hdmitx_abort_req ;
	} else if (ctrl == HDMITX_CTRL_REQ_SET) {
		hdmitx_abort_req = *p_req;
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
system_request hdmitx_ctrl_get_abort_req(void)
{
	system_request req;

	if (hdmitx_ctrl_req_abort(&req, HDMITX_CTRL_REQ_GET)) {
		return SYSTEM_REQUEST_NONE;
	}

	return req;
}
//------------------------------------------------------------------------------
void hdmitx_ctrl_set_abort_req(system_request req)
{
	if (hdmitx_ctrl_req_abort(&req, HDMITX_CTRL_REQ_SET)) {
		return;
	}
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_ctrl_val_vcdb(uint8_t *p_val, hdmitx_ctrl_val ctrl)
{
	static uint8_t hdmitx_sink_edid_vcdb = HDMITX_VCDB_QS_NO_DATA;

	if (p_val == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == HDMITX_CTRL_VAL_GET) {
		*p_val = hdmitx_sink_edid_vcdb;
	} else if (ctrl == HDMITX_CTRL_VAL_SET) {
		hdmitx_sink_edid_vcdb = *p_val;
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_video_format_and_sg(hdmitx_video *p_video, system_sg *p_sg)
{
	system_port port;
	uint8_t allupd = SYSTEM_CTRL_TX_UPDATE_REG_ALLUPD;
	uint8_t upd = SYSTEM_CTRL_TX_UPDATE_REG_NONE;
	system_video s_video;
	uint8_t tx0_sen;
	uint32_t ret;

	if (p_video == NULL || p_sg == NULL) {
		return ERR_NULL_POINTER;
	}
	if(p_video->tx_video_format == SYSTEM_VFORM_3840x2160P_5994or60_16v9)
	{
		Is_UHD = 1;
	}
	else
	{
		Is_UHD = 0;
	}
	port = p_video->tx_port;
	ret = hdmitx_copy_video_param(&s_video, p_video);
	CHECK_ERROR(ret);

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(port);
	CHECK_ERROR(ret);
	// TMDS clock and data channel power off (4.2 3)
	ret = hdmitx_set_tmds_ch_power(port, HDMITX_SET_TMDS_CH_POWER_OFF);
	CHECK_ERROR(ret);
	// Set HDMI Mode (4.1.3 1) or DVI Mode (4.1.2 1)
	ret = hdmitx_select_hdmi_dvi(port, p_video->tx_dvi_mode);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Set Video Input/Output Parameter (4.1.3 2) (4.1.4) (4.1.5)
	ret = system_ctrl_tx_set_video_format(&s_video);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Set Color Space Parameter (4.1.3 3) (4.1.5)
	if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
	{
		ret = system_ctrl_set_color_format(&s_video);
	}
	else
	{
		ret = system_ctrl_set_color_format_HDMI(&s_video);
	}
	CHECK_ERROR(ret);
	// Set TMDS Transmitter Parameter (4.1.3 7) (4.1.7)
	ret = hdmitx_set_tmds(p_video, &tx0_sen);
	CHECK_ERROR(ret);
	if (hdmitx_is_dvi_mode(p_video->tx_dvi_mode)) {
		// disable Scramble Mode (4.1.3. 10)
		ret = hdmitx_disable_scramble_mode(p_video);
		CHECK_ERROR(ret);
	} else {
		// Set Scramble Mode (4.1.3 10)
		ret = hdmitx_set_scramble_mode(p_video);
		CHECK_ERROR(ret);
	}
	// Reset TMDS Encode (4.1.3 11)
	ret = hdmitx_reset_tmds_encode(p_video->tx_port);
	CHECK_ERROR(ret);
	if (hdmitx_is_dvi_mode(p_video->tx_dvi_mode)) {
		// disable general control packet (GCP)
		ret = hdmitx_set_pktena(port, HDMITX_PKTENA_NONE, HDMITX_PKTENA_GCPEN);
		CHECK_ERROR(ret);
	} else {
		// enable general control packet (GCP)
		ret = hdmitx_set_pktena(port, HDMITX_PKTENA_GCPEN, HDMITX_PKTENA_GCPEN);
		CHECK_ERROR(ret);
	}
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H27BJ2)
	// set VBO-Rx combination
	//   - 4K/1080p to 480p down conversion
	//   - 720p/1366x768 through
	ret = hdmitx_set_vborx_comb(p_video, tx0_sen, &allupd);
	CHECK_ERROR(ret);
#else
	allupd = SYSTEM_CTRL_TX_UPDATE_REG_ALLUPD;
#endif
	// set sg setting if necessary
	ret = hdmitx_sg_setting(p_video, p_sg);
	CHECK_ERROR(ret);
	if (hdmitx_is_dvi_mode(p_video->tx_dvi_mode)) {
		upd = SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
				| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
				| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD;
	} else {
		upd = SYSTEM_CTRL_TX_UPDATE_REG_AVIIUPD
				| SYSTEM_CTRL_TX_UPDATE_REG_CLKUPD
				| SYSTEM_CTRL_TX_UPDATE_REG_HVSIUPD
				| SYSTEM_CTRL_TX_UPDATE_REG_VIFUPD
				| SYSTEM_CTRL_TX_UPDATE_REG_CSCUPD;
	}
	CHECK_ABORT();
	// register update
	// (ALLUPD), (AVIIUPD), CLKUPD, (HVSIUPD), VIFUPD, CSCUPD
	if(update_mode == HDMITX_UPDATE_MODE_ENABLE)
	{
		ret = system_ctrl_tx_update_reg(port, (allupd | upd));
		CHECK_ERROR(ret);
	}
	else
	{
		ret = system_ctrl_tx_update_reg(port, 0x10);
		CHECK_ERROR(ret);
	}
	//TMDS_clock stop
	hdmitx_TMDS_clock_en(port,0);

	//TMDS_clock go
	hdmitx_TMDS_clock_en(port,1);

	if (hdmitx_is_dvi_mode(p_video->tx_dvi_mode)) {
		// disable AVI InfoFrame
		ret = hdmitx_disable_avi_infoframe(p_video);
		CHECK_ERROR(ret);
		// disable HDMI Vendor Specific InfoFrame
		ret = hdmitx_disable_hdmi_vs_infoframe(p_video);
		CHECK_ERROR(ret);
	} else {
		// Set AVI InfoFrame
		ret = hdmitx_set_avi_infoframe(p_video);
		CHECK_ERROR(ret);
		// Set HDMI Vendor Specific InfoFrame
		ret = hdmitx_set_hdmi_vs_infoframe(p_video);
		CHECK_ERROR(ret);
	}
	// set SCDCS
	ret = hdmitx_set_scdcs(p_video);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// The Source shall allow a minimum of 1ms and a maximum of 100ms from the time
	// the TMDS_Bit_Clock_Ratio bit is written until resuming transmission of TMDS
	// clock and data at the updated data rete.
	SLEEP(HDMITX_SLEEP_TIME_80MSEC);
	// TMDS clock and data channel power on (4.1.3 12)
	ret = hdmitx_set_tmds_ch_power(port, HDMITX_SET_TMDS_CH_POWER_ON);
	CHECK_ERROR(ret);
	// Reset TMDS Encode (4.1.3 11)
	ret = hdmitx_reset_tmds_encode(p_video->tx_port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_sg_on(system_port port)
{
	hdmitx_video vform;
	system_sg sg;
	uint32_t ret;

	vform.rx_port = vform.tx_port = port;
	vform.rx_video_format = vform.tx_video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
	vform.rx_color_format = vform.tx_color_format = SYSTEM_COLOR_RGB_FULL_RANGE1;
	vform.rx_deep_color = vform.tx_deep_color = SYSTEM_DEEP_COLOR_8BIT;
	sg.onoff = SYSTEM_ONOFF_ON;
	sg.sg_format = SYSTEM_SG_FORMAT_BLANK;

	ret = hdmitx_set_video_format_and_sg(&vform, &sg);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_sg_off(system_port port)
{
	hdmitx_video vform;
	system_sg sg;
	uint32_t ret;

	vform.rx_port = vform.tx_port = port;
	sg.onoff = SYSTEM_ONOFF_OFF;

	ret = hdmitx_sg_setting(&vform, &sg);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_init(system_port port)
{
	uint32_t ret;

	// system control initialization
	ret = system_ctrl_init(port);
	CHECK_ERROR(ret);
	// set audio output
	ret = system_ctrl_audio_init(port, SYSTEM_CTRL_AUDIO_HDMI_TX);
	CHECK_ERROR(ret);
	// initialize audio transmit setting
	ret = hdmitx_init_audio_trans_setting(port);
	CHECK_ERROR(ret);
	// audio channel: 0 (default)
	ret = hdmitx_select_audio_ch(port, HDMITX_AUDIO_CH0);
	CHECK_ERROR(ret);
	// TMDS Transmitter Initialization (2.1.5)
	// Transmitter Link Initialization (2.1.6)
	ret = system_ctrl_tx_init(port);
	CHECK_ERROR(ret);
	// init abort request
	hdmitx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);
	// init HDCP Tx
	ret = hdmitx_hdcp_init(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_exit(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_exit(port, SYSTEM_CTRL_AUDIO_HDMI_TX);
	CHECK_ERROR(ret);
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_video_format(hdmitx_video *p_video)
{
	system_sg sg;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	sg.onoff = SYSTEM_ONOFF_OFF;
	ret = hdmitx_set_video_format_and_sg(p_video, &sg);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_audio_format(hdmitx_audio *p_audio)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_AUDIO_FORMAT_NUM];
	uint8_t hbr, data;
	uint32_t ret;

	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_audio->port);
	CHECK_ERROR(ret);

	// check audio param
	ret = system_ctrl_audio_check_param(p_audio);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &(p_audio->port), cmd, HDMITX_SET_AUDIO_FORMAT_NUM);
	// Set Audio Interface Parameter (4.1.6 2-1)
	// 0x30A9: [7]: ENDMODE
	//         [6]: POLLR
	//         [5]: ADIIS
	//         [4]: ADFB
	//         [3]: AUTOFS
	//         [1]: CHSEL
	switch (p_audio->i2s.endian_mode) {
		case SYSTEM_AUDIO_EM_MSB:		data = HDMITX_TXACTL2_ENDMODE_MSB;	break;
		case SYSTEM_AUDIO_EM_LSB:		data = HDMITX_TXACTL2_ENDMODE_LSB;	break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	switch (p_audio->i2s.polarity) {
		case SYSTEM_AUDIO_PO_LOW:		data |= HDMITX_TXACTL2_POLLR_LOW;	break;
		case SYSTEM_AUDIO_PO_HIGH:		data |= HDMITX_TXACTL2_POLLR_HIGH;	break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	switch (p_audio->i2s.i2s_format) {
		case SYSTEM_AUDIO_IF_DISABLE:	data |= HDMITX_TXACTL2_ADIIS_DIS;	break;
		case SYSTEM_AUDIO_IF_ENABLE:	data |= HDMITX_TXACTL2_ADIIS_ENA;	break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	switch (p_audio->i2s.start_pos) {
		case SYSTEM_AUDIO_SP_RIGHT:		data |= HDMITX_TXACTL2_ADFB_RIGHT;	break;
		case SYSTEM_AUDIO_SP_LEFT:		data |= HDMITX_TXACTL2_ADFB_LEFT;	break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	data |= HDMITX_TXACTL2_AUTOFS_TXFS;
	data |= HDMITX_TXACTL2_CHSEL_CHNUM;
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXACTL2_ADDR,
						data,
						HDMITX_TXACTL2_MASK);
	// Set Audio Interface Parameter (4.1.6 2-1)
	// 0x30AB: SELMCK: internal master clock
	SET_COMMAND(cmd, 1, VIDEOIF_AUDIO_TXACTL4_ADDR,
						HDMITX_TXACTL4_SELMCK_INTERNAL,
						HDMITX_TXACTL4_SELMCK_MASK);
	// Set Audio Interface Select (4.1.6 3)
	// 0x30AC: HBRON: ON/OFF
	//         RXSEL: is selected by spdifrx_init() and i2srx_init()
	hbr = system_ctrl_is_audio_hbr(p_audio->cb_sampling_freq);
	data = (hbr) ? HDMITX_TXACTL5_HBRON_ENA : HDMITX_TXACTL5_HBRON_DIS;
	// Set N Parameter of ACR Packet (4.1.6 8)
	// 0x30AC NSEL: automatic setting or TXFS_ST(0x30B3)
	data |= HDMITX_TXACTL5_NSEL_AUTO;
	SET_COMMAND(cmd, 2, VIDEOIF_AUDIO_TXACTL5_ADDR,
						data,
						HDMITX_TXACTL5_HBRON_MASK
						| HDMITX_TXACTL5_NSEL_MASK);
	// Set Audio Channel Number (4.1.6 4)
	// 0x30AE: CHNUM: (when CHSEL=1)
	// Set Input Master Clock Frequency (4.1.6 5)
	// 0x30AE: DIVMCK: not used (default)
	switch (p_audio->if_channel_count) {
		case SYSTEM_AUDIO_CC_RSH:		data = HDMITX_TXACTL7_CHNUM_RSV;	break;
		case SYSTEM_AUDIO_CC_2CH:		data = HDMITX_TXACTL7_CHNUM_2CH;	break;
		case SYSTEM_AUDIO_CC_3CH:		data = HDMITX_TXACTL7_CHNUM_3CH;	break;
		case SYSTEM_AUDIO_CC_4CH:		data = HDMITX_TXACTL7_CHNUM_4CH;	break;
		case SYSTEM_AUDIO_CC_5CH:		data = HDMITX_TXACTL7_CHNUM_5CH;	break;
		case SYSTEM_AUDIO_CC_6CH:		data = HDMITX_TXACTL7_CHNUM_6CH;	break;
		case SYSTEM_AUDIO_CC_7CH:		data = HDMITX_TXACTL7_CHNUM_7CH;	break;
		case SYSTEM_AUDIO_CC_8CH:		data = HDMITX_TXACTL7_CHNUM_8CH;	break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	SET_COMMAND(cmd, 3, VIDEOIF_AUDIO_TXACTL7_ADDR,
						data,
						HDMITX_TXACTL7_CHNUM_MASK);
	// set down-/over-sampling
	// 0x30AF:[7]: ODSMPMODE
	//        [6]: ODSMPSEL
	//        [5]: ODSMPON
	switch (p_audio->sampling_freq_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_4:	data = HDMITX_TXACTL8_ODSMPMODE_4
												| HDMITX_TXACTL8_ODSMPSEL_LPCM_DOWN
												| HDMITX_TXACTL8_ODSMPON_ENA;
										break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:	data = HDMITX_TXACTL8_ODSMPMODE_2
												| HDMITX_TXACTL8_ODSMPSEL_LPCM_DOWN
												| HDMITX_TXACTL8_ODSMPON_ENA;
										break;
		case SYSTEM_AUDIO_SF_RATIO_1_1:	data = HDMITX_TXACTL8_ODSMPMODE_2
												| HDMITX_TXACTL8_ODSMPSEL_LPCM_DOWN
												| HDMITX_TXACTL8_ODSMPON_DIS;
										break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:	data = HDMITX_TXACTL8_ODSMPMODE_2
												| HDMITX_TXACTL8_ODSMPSEL_LPCM_OVER
												| HDMITX_TXACTL8_ODSMPON_ENA;
										break;
		case SYSTEM_AUDIO_SF_RATIO_4_1:	data = HDMITX_TXACTL8_ODSMPMODE_4
												| HDMITX_TXACTL8_ODSMPSEL_LPCM_OVER
												| HDMITX_TXACTL8_ODSMPON_ENA;
										break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	// Set Audio Input Bit Width (4.1.6 6)
	// 0x30AF: ADBITIN:
	switch (p_audio->cb_sample_size) {
		case SYSTEM_AUDIO_SS_16BIT:
		case SYSTEM_AUDIO_MAX_20BIT:
		case SYSTEM_AUDIO_MAX_24BIT:	data |= HDMITX_TXACTL8_ADBITIN_16BIT;	break;
		case SYSTEM_AUDIO_SS_20BIT:
		case SYSTEM_AUDIO_SS_20BIT_0:
		case SYSTEM_AUDIO_SS_20BIT_1:	data |= HDMITX_TXACTL8_ADBITIN_20BIT;	break;
		case SYSTEM_AUDIO_SS_24BIT:		data |= HDMITX_TXACTL8_ADBITIN_24BIT;	break;
		default:						return ERR_HDMITX_AUDIO_FORMAT;
	}
	SET_COMMAND(cmd, 4, VIDEOIF_AUDIO_TXACTL8_ADDR,
						data,
						HDMITX_TXACTL8_ODSMPMODE_MASK
						| HDMITX_TXACTL8_ODSMPSEL_MASK
						| HDMITX_TXACTL8_ODSMPON_MASK
						| HDMITX_TXACTL8_ADBITIN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);
	// Set Output Channel Status Bit (4.1.6 7)
	// 0x30B0-0x30B7
	ret = hdmitx_set_audio_cbit(p_audio);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// Software Reset
	// AUDSRST, FIFORST
	ret = system_ctrl_tx_soft_reset(p_audio->port, SYSTEM_CTRL_TX_SOFT_RESET_AUDSRST
													| SYSTEM_CTRL_TX_SOFT_RESET_FIFORST);
	CHECK_ERROR(ret);
	// Set Audio InfoFrame
	ret = hdmitx_set_audio_infoframe(p_audio);
	CHECK_ERROR(ret);
	CHECK_ABORT();

	if(update_mode == HDMITX_UPDATE_MODE_DISABLE)
	{
		ret = system_ctrl_tx_update_reg(p_audio->port, 0x66);
		CHECK_ERROR(ret);
	}
	else
	{
		// Update Register Status
		// ALLUPD, AUDIUPD, AUDUPD
		ret = system_ctrl_tx_update_reg(p_audio->port,
									SYSTEM_CTRL_TX_UPDATE_REG_ALLUPD
									| SYSTEM_CTRL_TX_UPDATE_REG_AUDIUPD
									| SYSTEM_CTRL_TX_UPDATE_REG_AUDUPD);
		CHECK_ERROR(ret);
	}
	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_video_mute(hdmitx_mute *p_mute)
{
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_mute->port);
	CHECK_ERROR(ret);

	if (p_mute->mute == SYSTEM_TX_MUTE_ON || p_mute->mute == SYSTEM_TX_MUTE_OFF) {
		ret = system_ctrl_tx_video_mute((system_mute *)p_mute);
		CHECK_ERROR(ret);
	} else if (p_mute->mute == SYSTEM_TX_MUTE_ON_AVMUTE ||
				p_mute->mute == SYSTEM_TX_MUTE_OFF_AVMUTE) {
		ret = system_ctrl_tx_video_avmute((system_mute *)p_mute);
		CHECK_ERROR(ret);
	} else {
		return ERR_NG;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_audio_mute(hdmitx_mute *p_mute)
{
	system_reg_access access;
	system_command cmd[HDMITX_AUDIO_MUTE_NUM];
	uint8_t mute;
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_mute->port);
	CHECK_ERROR(ret);

	if (p_mute->mute == SYSTEM_TX_MUTE_ON) {
		mute = HDMITX_AUDIO_MUTE_TXACTL1_ACREN_DIS  |
				HDMITX_AUDIO_MUTE_TXACTL1_ASPEN_DIS |
				HDMITX_AUDIO_MUTE_TXACTL1_TXMUTE_MUTE;
	} else if (p_mute->mute == SYSTEM_TX_MUTE_OFF) {
		mute = HDMITX_AUDIO_MUTE_TXACTL1_ACREN_ENA  |
				HDMITX_AUDIO_MUTE_TXACTL1_ASPEN_ENA |
				HDMITX_AUDIO_MUTE_TXACTL1_TXMUTE_UNMUTE;
	} else {
		return ERR_NG;
	}

	SET_ACCESS(access, &(p_mute->port), cmd, HDMITX_AUDIO_MUTE_NUM);
	// ACREN: Audio Clock Regeneration Packet Enable
	// ASPEN: Audio Sample Packet Enable
	// Transmit Audio Mute
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXACTL1_ADDR,
						mute,
						HDMITX_AUDIO_MUTE_TXACTL1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_spd_infoframe(hdmitx_spd_infoframe *p_spd)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_SPD_INFOFRAME_LEN];
	uint32_t i, j;
	uint32_t ret;

	if (p_spd == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_spd->port);
	CHECK_ERROR(ret);

	if (p_spd->source_infomation == HDMITX_SET_SPD_INFOFRAME_DIS) {
		// disable SPD infoframe packet
		ret = hdmitx_set_infena(p_spd->port, HDMITX_INFENA_NONE, HDMITX_INFENA_SPDEN);
		CHECK_ERROR(ret);
	} else {
		SET_BLOCK_ACCESS(baccess, &(p_spd->port), VIDEOIF_INF_SPDINF_00_ADDR,
							HDMITX_SET_SPD_INFOFRAME_LEN, data);
		// infoframe type code + HDMI code: 0x03 + 0x80
		SET_BLOCK_DATA(data, 0, INFOFRAME_TYPE_SPD | HDMITX_INFOFRAME_TYPE);	// 0x3240
		// version
		SET_BLOCK_DATA(data, 1, INFOFRAME_VERSION_SPD);							// 0x3241
		// length of infoframe
		SET_BLOCK_DATA(data, 2, INFOFRAME_LENGTH_SPD);							// 0x3242
		// checksum (auto checksum)
		SET_BLOCK_DATA(data, 3, HDMITX_SET_SPD_INFOFRAME_CHKSUM_INIT);			// 0x3243
		// Vendor Name
		for (i=0, j=4; i<SYSTEM_SPD_INFOFRAME_VN_BYTE_SIZE; i++, j++) {
			SET_BLOCK_DATA(data, j, p_spd->vendor_name[i]);						// 0x3244-0x324B
		}
		// Product Description
		for (i=0, j=12; i<SYSTEM_SPD_INFOFRAME_PD_BYTE_SIZE; i++, j++) {
			SET_BLOCK_DATA(data, j, p_spd->product_description[i]);				// 0x324C-0x325B
		}
		// Source Information
		SET_BLOCK_DATA(data, 28, p_spd->source_infomation);						// 0x325C
		ret = device_block_write(&baccess);
		CHECK_ERROR(ret);
		// update SPD infoframe packet (autocleared)
		ret = hdmitx_update_infoframe(p_spd->port, HDMITX_PKTUPD_SPDUPD);
		CHECK_ERROR(ret);
		// enable SPD infoframe packet
		ret = hdmitx_set_infena(p_spd->port, HDMITX_INFENA_SPDEN, HDMITX_INFENA_SPDEN);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_hdr_infoframe(hdmitx_hdr_infoframe *p_hdr)
{
	system_block_access baccess;
	uint8_t data[HDMITX_SET_HDR_INFOFRAME_LEN];
	uint32_t i, j, total;
	uint32_t ret;

	if (p_hdr == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_hdr->port);
	CHECK_ERROR(ret);

	if (p_hdr->eotf == HDMITX_SET_HDR_INFOFRAME_DIS) {
		// disable free packet (including HDR)
		ret = hdmitx_set_infena(p_hdr->port, HDMITX_INFENA_NONE, HDMITX_INFENA_FREEEN);
		CHECK_ERROR(ret);
	} else {
		SET_BLOCK_ACCESS(baccess, &(p_hdr->port), VIDEOIF_PKT_FREPKT_00_ADDR,
							HDMITX_SET_HDR_INFOFRAME_LEN, data);
		// infoframe type code + HDMI code: 0x07 + 0x80
		SET_BLOCK_DATA(data, 0, INFOFRAME_TYPE_DRM | HDMITX_INFOFRAME_TYPE);	// 0x3EA0
		// version
		SET_BLOCK_DATA(data, 1, INFOFRAME_VERSION_HDR);							// 0x3EA1
		// length of infoframe
		SET_BLOCK_DATA(data, 2, INFOFRAME_LENGTH_HDR);							// 0x3EA2
		// checksum (default)
		SET_BLOCK_DATA(data, 3, HDMITX_SET_HDR_INFOFRAME_CHKSUM_INIT);			// 0x3EA3
		// EOTF
		SET_BLOCK_DATA(data, 4, p_hdr->eotf);									// 0x3EA4
		// Static_Metadata_Descriptor_ID
		SET_BLOCK_DATA(data, 5, p_hdr->static_metadata_descriptor_id);			// 0x3EA5
		// Static_Metadata_Descriptor
		for (i=0, j=6; i<SYSTEM_HDR_INFOFRAME_SMD_BYTE_SIZE; i++, j++) {
			SET_BLOCK_DATA(data, j, p_hdr->static_metadata_descriptor[i]);		// 0x3EA6-0x3EBD
		}
		// calculate checksum
		total = 0;
		for (i=0; i<HDMITX_SET_HDR_INFOFRAME_LEN; i++) {
			total += data[i];
		}
		total &= HDMITX_SET_HDR_INFOFRAME_MASK;
		// reset checksum (calculated)
		SET_BLOCK_DATA(data, 3, HDMITX_SET_HDR_INFOFRAME_8BIT_MAX - total);		// 0x3EA3
		ret = device_block_write(&baccess);
		CHECK_ERROR(ret);
		// update free packet (autocleared)
		ret = hdmitx_update_infoframe(p_hdr->port, HDMITX_PKTUPD_FREEUPD);
		CHECK_ERROR(ret);
		// enable free packet (including HDR)
		ret = hdmitx_set_infena(p_hdr->port, HDMITX_INFENA_FREEEN, HDMITX_INFENA_FREEEN);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_get_hpd_status(system_port port, uint8_t *p_status)
{
	system_reg_access access;
	system_command cmd[HDMITX_HPD_GET_STATUS_NUM];
	uint32_t ret;

	if (p_status == NULL) {
		return ERR_NULL_POINTER;
	}
	CHECK_ABORT();

	SET_ACCESS(access, &port, cmd, HDMITX_HPD_GET_STATUS_NUM);
	// 0x1040
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TXSYSCFG12_ADDR, 0, HDMITX_HPD_GET_STATUS_TXSYSCFG12_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_status = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_hpd_intr(system_port port, uint8_t intr)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_HPD_INTR_NUM];
	uint32_t ret;

	CHECK_ABORT();

	SET_ACCESS(access, &port, cmd, HDMITX_SET_HPD_INTR_NUM);
	// 0x10FE
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG10_00_ADDR, intr, HDMITX_INTREG10_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_get_hpd_intr(system_port port, uint8_t *p_intr)
{
	system_reg_access access;
	system_command cmd[HDMITX_GET_HPD_INTR_NUM];
	uint32_t ret;

	if (p_intr == NULL) {
		return ERR_NULL_POINTER;
	}
	CHECK_ABORT();

	SET_ACCESS(access, &port, cmd, HDMITX_GET_HPD_INTR_NUM);
	// 0x10FE
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG10_00_ADDR, 0, HDMITX_INTREG10_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_intr = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_set_sink_edid_vcdb(hdmitx_edid_vcdb *p_vcdb)
{
	uint32_t ret;

	if (p_vcdb == NULL) {
		return ERR_NULL_POINTER;
	}

	// set Byte#3 of Video Capability Data Block
	ret = hdmitx_ctrl_val_vcdb(&(p_vcdb->byte3), HDMITX_CTRL_VAL_SET);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_abort(void)
{
	// abort request
	hdmitx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

	return ERR_OK;
}
//------------------------------------------------------------------------------
