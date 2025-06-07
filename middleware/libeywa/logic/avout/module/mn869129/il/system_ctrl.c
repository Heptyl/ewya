/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include <stdint.h>
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "hv_version.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// This is used to invert TMDS data channel polarity.
//#define CONFIG_TX_TMDS_DATA_POLARITY_INVERSION
// This is used to swap TMDS data channel CH.0 for TX CH.2.
//#define CONFIG_TX_TMDS_DATA_CHANNEL_SWAP
//------------------------------------------------------------------------------
// sysctrl_set_rx_tx_saddr()
#define SYSCTRL_SET_RX_TX_SADDR_LEN								4
#define SYSCTRL_SET_RX_TX_SADDR_SADRSET_00_R0SA_BASE			0x02
#define SYSCTRL_SET_RX_TX_SADDR_SADRSET_01_T0SA_BASE			0x03
#define SYSCTRL_SET_RX_TX_SADDR_OFFSET							0x03
#define SYSCTRL_SET_RX_TX_SADDR_SADRSET_02_DEF					0x00
#define SYSCTRL_SET_RX_TX_SADDR_HIFSET_T0HSEL_IF0				0x00
#define SYSCTRL_SET_RX_TX_SADDR_HIFSET_T0HSEL_IF1				0x04
#define SYSCTRL_SET_RX_TX_SADDR_HIFSET_R0HSEL_IF0				0x00
#define SYSCTRL_SET_RX_TX_SADDR_HIFSET_R0HSEL_IF1				0x01
//------------------------------------------------------------------------------
// sysctrl_get_rx_tx_saddr()
#define SYSCTRL_GET_RX_TX_SADDR_LEN								2
#define SYSCTRL_GET_RX_TX_SADDR_SADRSET_00_R0SA_MASK			0x0F
#define SYSCTRL_GET_RX_TX_SADDR_SADRSET_01_T0SA_MASK			0x0F
//------------------------------------------------------------------------------
// sysctrl_set_intr_pin()
#define SYSCTRL_SET_INTR_PIN_LEN								2
#define SYSCTRL_SET_INTR_PIN_INTASSGN1_T0IRQSEL_NIRQ0			0x00
#define SYSCTRL_SET_INTR_PIN_INTASSGN1_R0IRQSELA_NIRQ0			0x00
#define SYSCTRL_SET_INTR_PIN_INTASSGN2_R0IRQSELB_NIRQ0			0x00
#define SYSCTRL_SET_INTR_PIN_INTASSGN2_CCIRQSEL_NIRQ0			0x00
//------------------------------------------------------------------------------
// sysctrl_release_all_power_down()
#define SYSCTRL_RELEASE_ALL_POWER_DOWN_NUM						1
#define SYSCTRL_RELEASE_ALL_POWER_DOWN_SWPDWN2_ALLPDWN_NOR		0x00
#define SYSCTRL_RELEASE_ALL_POWER_DOWN_SWPDWN2_RXC0DWN_NOR		0x00
#define SYSCTRL_RELEASE_ALL_POWER_DOWN_SWPDWN2_MASK				0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_power_tx()
#define SYSCTRL_SET_POWER_TX_NUM								1
#define SYSCTRL_SET_POWER_TX_TXSYSCFG1_TP0SWPDWN_NOR			0x00
#define SYSCTRL_SET_POWER_TX_TXSYSCFG1_TP0SWPDWN_DWN			0x10
#define SYSCTRL_SET_POWER_TX_TXSYSCFG1_MASK						0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_power_vborx()
#define SYSCTRL_SET_POWER_VBORX_NUM								4
#define SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_NPD_PWR_ON			0x01
#define SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_NPD_PWR_DOWN		0x00
#define SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_MASK				0x000000FF
#define SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_NPD_X_PWR_ON		0x0F
#define SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_NPD_X_PWR_DOWN		0x00
#define SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_MASK				0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_power_lvdsvbotx()
#define SYSCTRL_SET_POWER_VBOTX_NUM								4
#define SYSCTRL_SET_POWER_VBOTX_PAGNUM_VBO_LINK					0x01
#define SYSCTRL_SET_POWER_VBOTX_PAGNUM_LVDSVBO_PHY				0x02
#define SYSCTRL_SET_POWER_VBOTX_LVDSLINKPAGESEL_MASK			0x000000FF
#define SYSCTRL_SET_POWER_VBOTX_VBOCTL21_NPOWDVBO_ENA			0x00
#define SYSCTRL_SET_POWER_VBOTX_VBOCTL21_NPOWDVBO_DIS			0x01
#define SYSCTRL_SET_POWER_VBOTX_VBOCTL21_MASK					0x000000FF
#define SYSCTRL_SET_POWER_VBOTX_ENTCNT_3_NPDOWN_DOWN			0x00
#define SYSCTRL_SET_POWER_VBOTX_ENTCNT_3_NPDOWN_ON				0x10
#define SYSCTRL_SET_POWER_VBOTX_ENTCNT_3_MASK					0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_power_hdmirx()
#define SYSTEM_CTRL_SET_POWER_HDMIRX_NUM						7
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX3SWPDWN_NOR		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX3SWPDWN_DWN		0x08
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX2SWPDWN_NOR		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX2SWPDWN_DWN		0x04
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX1SWPDWN_NOR		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX1SWPDWN_DWN		0x02
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX0SWPDWN_NOR		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX0SWPDWN_DWN		0x01
#define SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_MASK				0x000000FF
#define SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_NPDX_NOR		0x01
#define SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_NPDX_DWN		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_MASK			0x000000FF
#define SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_PORT_NUM		4
#define SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_OFFSET			0x20
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_RXPLL0EN_MAN		0x80
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_RXPLL0EN_AUT		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_DIVR0_DEF			0x18
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_MASK				0x000000FF
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_ICPMR0_DEF		0x80
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_FOUTSELR0_DEF		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_FDIVR0_DEF		0x02
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_NPDOWNR0_NOR		0x00
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_NPDOWNR0_DWN		0x01
#define SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_MASK				0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_power_hdmitx()
#define SYSCTRL_SET_POWER_HDMITX_NUM							2
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDTX_NOR			0x40
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDTX_DWN			0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NRS_SUSP_NOR		0x20
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NRS_SUSP_SUS		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_SWINGNPD_NOR		0x04
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_SWINGNPD_DWN		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDPLL_NOR			0x02
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDPLL_DWN			0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDCKO_DEF			0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_MASK				0x000000FF
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDBGR_NOR		0x80
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDBGR_DWN		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT2_NOR		0x40
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT2_DWN		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT1_NOR		0x20
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT1_DWN		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT0_NOR		0x10
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT0_DWN		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NRSTPLL_NOR		0x08
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NRSTPLL_RST		0x00
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_TXPRM00_DEF		0x03
#define SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_MASK				0x000000FF
//------------------------------------------------------------------------------
// system_ctrl_init()
#define SYSTEM_CTRL_INIT_SADRSET_00_R0SA_DEF					0x00
#define SYSTEM_CTRL_INIT_SADRSET_00_R0SA_MASK					0x0F
#define SYSTEM_CTRL_INIT_SADRSET_01_T0SA_DEF					0x00
#define SYSTEM_CTRL_INIT_SADRSET_01_T0SA_MASK					0x0F
//------------------------------------------------------------------------------
// system_ctrl_check_vborx_init()
#define SYSTEM_CTRL_CHECK_VBORX_INIT_NUM						1
#define SYSTEM_CTRL_CHECK_VBORX_INIT_SADRSET_00_R0SA_DEF		SYSTEM_CTRL_INIT_SADRSET_00_R0SA_DEF
#define SYSTEM_CTRL_CHECK_VBORX_INIT_MISC_VBOREN_VBORX			0x10
#define SYSTEM_CTRL_CHECK_VBORX_INIT_MISC_VBOREN_MASK			0x10
//------------------------------------------------------------------------------
// system_ctrl_check_hdmirx_init()
#define SYSTEM_CTRL_CHECK_HDMIRX_INIT_NUM						1
#define SYSTEM_CTRL_CHECK_HDMIRX_INIT_SADRSET_00_R0SA_DEF		SYSTEM_CTRL_INIT_SADRSET_00_R0SA_DEF
#define SYSTEM_CTRL_CHECK_HDMIRX_INIT_MISC_VBOREN_HDMIRX		0x00
#define SYSTEM_CTRL_CHECK_HDMIRX_INIT_MISC_VBOREN_MASK			0x10
#define SYSTEM_CTRL_CHECK_HDMIRX_INIT_PADAINCTRL1_RXLINK			0x02
#define SYSTEM_CTRL_CHECK_HDMIRX_INIT_PADAINCTRL1_TXAIL0SEL_MASK	0x02
//------------------------------------------------------------------------------
// system_ctrl_check_hdmitx_init()
#define SYSTEM_CTRL_CHECK_HDMITX_INIT_NUM						1
#define SYSTEM_CTRL_CHECK_HDMITX_INIT_SADRSET_01_T0SA_DEF		SYSTEM_CTRL_INIT_SADRSET_01_T0SA_DEF
#define SYSTEM_CTRL_CHECK_HDMITX_INIT_CTSGEN_2_DEFAULT			0x00
#define SYSTEM_CTRL_CHECK_HDMITX_INIT_CTSGEN_2_MASK				0x0C
//------------------------------------------------------------------------------
// system_ctrl_check_vbotx_init()
#define SYSTEM_CTRL_CHECK_VBOTX_INIT_NUM						1
#define SYSTEM_CTRL_CHECK_VBOTX_INIT_SADRSET_01_T0SA_DEF		SYSTEM_CTRL_INIT_SADRSET_01_T0SA_DEF
#define SYSTEM_CTRL_CHECK_VBOTX_INIT_SWMODE4_01_VBO				0x20
#define SYSTEM_CTRL_CHECK_VBOTX_INIT_SWMODE4_01_MASK			0x00000060
//------------------------------------------------------------------------------
// system_ctrl_check_lvdstx_init()
#define SYSTEM_CTRL_CHECK_LVDSTX_INIT_NUM						1
#define SYSTEM_CTRL_CHECK_LVDSTX_INIT_SADRSET_01_T0SA_DEF		SYSTEM_CTRL_INIT_SADRSET_01_T0SA_DEF
#define SYSTEM_CTRL_CHECK_LVDSTX_INIT_SWMODE4_01_LVDS			0x60
#define SYSTEM_CTRL_CHECK_LVDSTX_INIT_SWMODE4_01_MASK			SYSTEM_CTRL_CHECK_VBOTX_INIT_SWMODE4_01_MASK
//------------------------------------------------------------------------------
// sysctrl_get_rx_cf()
#define SYSCTRL_GET_RX_CF_NUM									1
#define SYSCTRL_CSCMOD3_ICFMT_MASK								0x000000F0
#define SYSCTRL_CSCMOD3_OCFMT_MASK								0x0000000F
//------------------------------------------------------------------------------
// sysctrl_set_rx_cf()
#define SYSCTRL_SET_RX_CF_NUM									1
//------------------------------------------------------------------------------
// sysctrl_get_tx_cf()
#define SYSCTRL_GET_TX_CF_NUM									1
#define SYSCTRL_CFMT_ICFMT_MASK									0x000000F0
#define SYSCTRL_CFMT_OCFMT_MASK									0x0000000F
//------------------------------------------------------------------------------
// sysctrl_set_tx_cf()
#define SYSCTRL_SET_RX_CF_NUM									1
//------------------------------------------------------------------------------
// sysctrl_set_rx_soft_reset1()
#define SYSCTRL_SET_RX_SOFT_RESET1_NUM							1
#define SYSCTRL_RSRST1_VIFRST									0x08
#define SYSCTRL_RSRST1_MASK										0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_rx_cf_converter_update()
#define SYSCTRL_SET_RX_CF_CONVERTER_UPDATE_NUM					1
#define SYSCTRL_CSCUPD_ICFASET_AUTO								0x00
#define SYSCTRL_CSCUPD_ICFASET_ICFMT							0x80
#define SYSCTRL_CSCUPD_ICFASET_MASK								0x00000080
//------------------------------------------------------------------------------
// sysctrl_set_rxpll0en()
#define SYSCTRL_SET_RXPLL0EN_NUM								1
#define SYSCTRL_RXPLL0_01_RXPLL0EN_AUTO							0x00
#define SYSCTRL_RXPLL0_01_RXPLL0EN_MANUAL						0x80
#define SYSCTRL_RXPLL0_01_RXPLL0EN_MASK							0x00000080
//------------------------------------------------------------------------------
// sysctrl_set_rxpll00en()
#define SYSCTRL_SET_RXPLL00EN_NUM								1
#define SYSCTRL_RXPLL00_03_RXPLL00EN_AUTO						0x00
#define SYSCTRL_RXPLL00_03_RXPLL00EN_MANUAL						0x08
#define SYSCTRL_RXPLL00_03_RXPLL00EN_MASK						0x00000008
//------------------------------------------------------------------------------
// sysctrl_set_rx0_sen()
#define SYSCTRL_SET_RX0_SEN_NUM									1
#define SYSCTRL_CKSELEN_RX0_SEN_AUTO							0x00
#define SYSCTRL_CKSELEN_RX0_SEN_MANUAL							0x02
#define SYSCTRL_CKSELEN_RX0_SEN_MASK							0x00000002
//------------------------------------------------------------------------------
// sysctrl_get_rxpll0en()
#define SYSCTRL_GET_RXPLL0EN_NUM								1
//------------------------------------------------------------------------------
// sysctrl_get_rxpll00en()
#define SYSCTRL_GET_RXPLL00EN_NUM								1
//------------------------------------------------------------------------------
// sysctrl_get_rx0_sen()
#define SYSCTRL_GET_RX0_SEN_NUM									1
//------------------------------------------------------------------------------
// system_ctrl_set_color_format()
#define SYSTEM_CTRL_ICFMT_BIT									4
//------------------------------------------------------------------------------
// sysctrl_tmds_transmitter_init()
#define SYSCTRL_TMDS_TX_INIT_NUM								3
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_DATINV_NOR			0x00
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_DATINV_REV			0x02
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_CHSWP_NOR				0x00
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_CHSWP_SWP				0x01
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_MASK					0x000000FF
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_02_SCRMOD_HDMI2			0x30
#define SYSCTRL_TMDS_TX_INIT_TXENCSET4_02_SCRMOD_MASK			0x00000030
#define SYSCTRL_TMDS_TX_INIT_INTMSK_0A_IRAMSK_ALL				0xFF
#define SYSCTRL_TMDS_TX_INIT_INTMSK_0A_IRAMSK_MASK				0x000000FF
//------------------------------------------------------------------------------
// sysctrl_transmitter_link_init()
#define SYSCTRL_TX_LINK_INIT_LEN								7
#define SYSCTRL_TX_LINK_INIT_TINTMSK_00_INTS0MSK_FF				0xFF
#define SYSCTRL_TX_LINK_INIT_TINTMSK_01_INTS1MSK_00				0x00
#define SYSCTRL_TX_LINK_INIT_TINTMSK_02_INTS2MSK_0F				0x0F
#define SYSCTRL_TX_LINK_INIT_TINTMSK_03_INTS3MSK_00				0x00
#define SYSCTRL_TX_LINK_INIT_TINTMSK_03_INTS4MSK_F0				0xF0
#define SYSCTRL_TX_LINK_INIT_TINTMSK_05_INTS5MSK_00				0x00
#define SYSCTRL_TX_LINK_INIT_TINTMSK_06_INTS6MSK_FF				0xFF
//------------------------------------------------------------------------------
// system_ctrl_tx_video_mute()
#define SYSTEM_CTRL_TX_VIDEO_MUTE_NUM							1
#define SYSTEM_CTRL_TX_VIDEO_MUTE_VMUTECNT_MUTE_ON				0x80
#define SYSTEM_CTRL_TX_VIDEO_MUTE_VMUTECNT_MUTE_OFF				0x00
#define SYSTEM_CTRL_TX_VIDEO_MUTE_VMUTECNT_MASK					0x00000080
//------------------------------------------------------------------------------
// system_ctrl_tx_video_avmute()
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_NUM							2
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_CLEAR_AVMUTE_ON	0x10
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_CLEAR_AVMUTE_OFF	0x00
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_CLEAR_AVMUTE_MASK	0x00000010
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_SET_AVMUTE_ON		0x01
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_SET_AVMUTE_OFF	0x00
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_SET_AVMUTE_MASK	0x00000001
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_UPDCTRL_CSCUPD_UPD			0x01
#define SYSTEM_CTRL_TX_VIDEO_AVMUTE_UPDCTRL_CSCUPD_MASK			0x00000001
//------------------------------------------------------------------------------
// system_ctrl_tx_video_black()
#define SYSTEM_CTRL_TX_VIDEO_BLACK_NUM							2
#define SYSTEM_CTRL_TX_VIDEO_BLACK_LVDSLINKPAGESEL_GLUE			0x03
#define SYSTEM_CTRL_TX_VIDEO_BLACK_LVDSLINKPAGESEL_MASK			0x000000FF
#define SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZERVRST_NORMAL	0x00
#define SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZERVRST_RELEASE	0x20
#define SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZERVRST_MASK	0x00000020
#define SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZER_NORMAL		0x00
#define SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZER_BLACK		0x10
#define SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZER_MASK		0x00000010
//------------------------------------------------------------------------------
// system_ctrl_tx_update_reg()
#define SYSTEM_CTRL_TX_UPDATE_REG_NUM							1
#define SYSTEM_CTRL_TX_UPDATE_REG_UPDCTRL_CLEARED				0x00
#define SYSTEM_CTRL_TX_UPDATE_REG_UPDCTRL_MASK					0x000000FF
#define SYSTEM_CTRL_TX_UPDATE_REG_RETRY_MAX						300
#define SYSTEM_CTRL_TX_UPDATE_REG_RETRY_INTERVAL				1
//------------------------------------------------------------------------------
// system_ctrl_tx_soft_reset()
#define SYSTEM_CTRL_TX_SOFT_RESET_NUM							1
#define SYSTEM_CTRL_TX_SOFT_RESET_TSRST_CLEARED					0x00
#define SYSTEM_CTRL_TX_SOFT_RESET_TSRST_MASK					0x000000FF
//------------------------------------------------------------------------------
// sysctrl_get_vesa_dmt_id()
#define SYSCTRL_PF_25000KHZ										25000
#define SYSCTRL_PF_32000KHZ										32000
#define SYSCTRL_PF_33000KHZ										33000
#define SYSCTRL_PF_39000KHZ										39000
#define SYSCTRL_PF_48000KHZ										48000
#define SYSCTRL_PF_64000KHZ										64000
#define SYSCTRL_PF_78000KHZ										78000
#define SYSCTRL_PF_98000KHZ										98000
#define SYSCTRL_PF_128000KHZ									128000
#define SYSCTRL_PF_156000KHZ									156000
#define SYSCTRL_PF_192000KHZ									192000
#define SYSCTRL_PF_256000KHZ									256000
#define SYSCTRL_PF_312000KHZ									312000
#define SYSCTRL_PF_384000KHZ									384000
#define SYSCTRL_PF_512000KHZ									512000
#define SYSCTRL_PF_600000KHZ									600000
#define SYSCTRL_PCFMT_25_32MHZ									4
#define SYSCTRL_PCFMT_32_33MHZ									1
#define SYSCTRL_PCFMT_33_39MHZ									3
#define SYSCTRL_PCFMT_39_48MHZ									9
#define SYSCTRL_PCFMT_48_64MHZ									10
#define SYSCTRL_PCFMT_64_78MHZ									16
#define SYSCTRL_PCFMT_78_98MHZ									18
#define SYSCTRL_PCFMT_98_128MHZ									20
#define SYSCTRL_PCFMT_128_156MHZ								36
#define SYSCTRL_PCFMT_156_192MHZ								37
#define SYSCTRL_PCFMT_192_256MHZ								45
#define SYSCTRL_PCFMT_256_312MHZ								56
#define SYSCTRL_PCFMT_312_384MHZ								67
#define SYSCTRL_PCFMT_384_512MHZ								78
#define SYSCTRL_PCFMT_512_600MHZ								80
//------------------------------------------------------------------------------
// sysctrl_set_vformat()
#define SYSCTRL_SET_VFORMAT_LEN									7
#define SYSCTRL_VIDEOFMT_01_VOFMTSEL_USE_VOFMT					0x80
#define SYSCTRL_HDMIFMT_HDMIFMT_2D								0x00
#define SYSCTRL_HDMIFMT_HDMIFMT_4K2K							0x01
#define SYSCTRL_HDMIFMT_VF_MODE_MODE1							0x00
#define SYSCTRL_3DFMT_V3D_EXT_0									0x00
#define SYSCTRL_3DFMT_VSTC_3D_0									0x00
#define SYSCTRL_HDMIVIC_HDMI_VIC_NOR							0x00
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_30HZ						0x01
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_25HZ						0x02
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_24HZ						0x03
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_24HZ					0x04
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_25HZ					0x05
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_30HZ					0x06
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_60HZ						0x07
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_50HZ						0x08
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_60HZ					0x09
#define SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_50HZ					0x0A
#define SYSCTRL_PCFMT_PCFMT_DEF									0x00
#define SYSCTRL_VSELECT_00_UPCBPS_USE_UPC						0x00
#define SYSCTRL_VSELECT_00_UPCBPS_NOT_USE_UPC					0x80
#define SYSCTRL_VSELECT_00_VCIBPS_USE_VCI						0x00
#define SYSCTRL_VSELECT_00_VCIBPS_NOT_USE_VCI					0x40
#define SYSCTRL_VSELECT_00_SCLKSEL_NOR							0x00
//------------------------------------------------------------------------------
// sysctrl_set_de_mode()
#define SYSCTRL_SET_DE_MODE_NUM									1
#define SYSCTRL_SYNCCNT_DEF										0x10
#define SYSCTRL_SYNCCNT_MASK									0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_super_resolution()
#define SYSCTRL_SET_SUPER_RESOLUTION_NUM						1
#define SYSCTRL_SRNON_SRNON_ENA									0x80
#define SYSCTRL_SRNON_SRNON_DIS									0x00
#define SYSCTRL_SRNON_MASK										0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_downscaler()
#define SYSCTRL_SET_DOWNSCALER_NUM								1
#define SYSCTRL_SWMODE4_00_DNCON_ON								0x80
#define SYSCTRL_SWMODE4_00_DNCON_OFF							0x00
#define SYSCTRL_SWMODE4_00_MASK									0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_480p_downscaler()
#define SYSCTRL_SET_480P_DOWNSCALER_NUM							1
#define SYSCTRL_DNCCTL2_SDDNCF_FROM_4K							0x20
#define SYSCTRL_DNCCTL2_SDDNCF_FROM_1080P						0x00
#define SYSCTRL_DNCCTL2_SDDNCE_ON								0x10
#define SYSCTRL_DNCCTL2_SDDNCE_OFF								0x00
#define SYSCTRL_DNCCTL2_MASK									0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_deep_color()
#define SYSCTRL_SET_DEEP_COLOR_NUM								3
#define SYSCTRL_VINCNT_BITWIDTH_MASK							0x00000003
#define SYSCTRL_OUTWSET_OUTBW_MASK								0x00000003
#define SYSCTRL_TDPCMODE_00_DEEPCON_NOR							0x00
#define SYSCTRL_TDPCMODE_00_DEEPCON_DEEPCOLOR					0x80
#define SYSCTRL_TDPCMODE_00_DEFPHASE_PHASE0						0x00
#define SYSCTRL_TDPCMODE_00_DEFPHASE_PHASE1						0x40
#define SYSCTRL_TDPCMODE_00_DEEPCSRST_NOR						0x00
#define SYSCTRL_TDPCMODE_00_DEEPCSRST_RST						0x20
#define SYSCTRL_TDPCMODE_00_DEEPCSRST_MASK						0x20
#define SYSCTRL_TDPCMODE_00_MASK								0x000000FF
//------------------------------------------------------------------------------
// sysctrl_srst_deep_color_ctrl()
#define SYSCTRL_SRST_DEEP_COLOR_CTRL_NUM						1
//------------------------------------------------------------------------------
// system_ctrl_get_vbo_lane_count()
#define SYSTEM_CTRL_PIXEL_FREQ_300000KHZ						300000
#define SYSTEM_CTRL_PIXEL_FREQ_150000KHZ						150000
#define SYSTEM_CTRL_PIXEL_FREQ_75000KHZ							75000
//------------------------------------------------------------------------------
// system_ctrl_get_tx_glue_hstart()
#define SYSTEM_CTRL_GLUECTL2_HSTART70_MASK						0x000000FF
#define SYSTEM_CTRL_GLUECTL3_HSTART98_BIT						8
#define SYSTEM_CTRL_GLUECTL3_HSTART98_MASK						0x00000300
//------------------------------------------------------------------------------
// system_ctrl_get_hdmirx_sync_polarity()
#define SYSCTRL_GET_HDMIRX_SYNC_POLARITY_NUM					1
#define SYSCTRL_VIDEO_CRRSYNC_00_CRRHPL_MASK					0x00000001
#define SYSCTRL_VIDEO_CRRSYNC_00_CRRVPL_MASK					0x00000002
//------------------------------------------------------------------------------
// sysctrl_set_pad_mode()
#define SYSCTRL_SET_PAD_MODE_NUM								1
#define SYSCTRL_SET_PAD_MODE_NO_EXTERNAL_AUDIO					0x00
#define SYSCTRL_SET_PAD_MODE_EXTERNAL_AUDIO_OUTPUT				0x04
#define SYSCTRL_SET_PAD_MODE_EXTERNAL_AUDIO_INPUT				0x08
#define SYSCTRL_SET_PAD_MODE_PADMDCTRL_MASK						0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_pad_input1()
#define SYSCTRL_SET_PAD_INPUT1_NUM								1
#define SYSCTRL_PADAINCTRL1_ARXL1SEL_NO_INPUT					0x00
#define SYSCTRL_PADAINCTRL1_ARXL1SEL_EXTERNAL_INPUT				0x80
#define SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK						0x00000080
#define SYSCTRL_PADAINCTRL1_TXAIL0SEL_NO_INPUT					0x00
#define SYSCTRL_PADAINCTRL1_TXAIL0SEL_RX_LINK_INPUT				0x02
#define SYSCTRL_PADAINCTRL1_TXAIL0SEL_MASK						0x00000002
//------------------------------------------------------------------------------
// sysctrl_set_pad_input2()
#define SYSCTRL_SET_PAD_INPUT2_NUM								1
#define SYSCTRL_PADAINCTRL2_ACH8INSEL_EXTERNAL_INPUT_OFF		0x00
#define SYSCTRL_PADAINCTRL2_ACH8INSEL_EXTERNAL_INPUT_ON			0x01
#define SYSCTRL_PADAINCTRL2_MASK								0x000000FF
//------------------------------------------------------------------------------
// sysctrl_set_pad_output()
#define SYSCTRL_SET_PAD_OUTPUT_NUM								1
#define SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF		0x05
#define SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_ON		0x04
#define SYSCTRL_PADAOUTCTRL1_MASK								0x000000FF
//------------------------------------------------------------------------------
// sysctrl_init_audio_tx_link()
#define SYSCTRL_INIT_AUDIO_TX_LINK_NUM							1
#define SYSCTRL_TXACTL1_DEF										0x04
#define SYSCTRL_TXACTL1_MASK									0x000000FF
//------------------------------------------------------------------------------
// sysctrl_select_input_if()
#define SYSCTRL_SELECT_INPUT_IF_NUM								1
#define SYSCTRL_TXACTL5_RXSEL_SPDIF								0x00
#define SYSCTRL_TXACTL5_RXSEL_I2S								0x01
#define SYSCTRL_TXACTL5_RXSEL_MASK								0x00000001
//------------------------------------------------------------------------------
// system_ctrl_audio_if2sf()
// system_ctrl_audio_sf2if()
#define SYSTEM_CTRL_AUDIO_IF_SF_RSH								0x00	// Refer to Stream Header
#define SYSTEM_CTRL_AUDIO_IF_SF_32KHZ							0x04	//   32  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_IF_SF_44KHZ							0x08	//   44.1kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_IF_SF_48KHZ							0x0C	//   48  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_IF_SF_88KHZ							0x10	//   88.2kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_IF_SF_96KHZ							0x14	//   96  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_IF_SF_176KHZ							0x18	//  176.4kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_IF_SF_192KHZ							0x1C	//  192  kHz (AS, OBA, DST)
//------------------------------------------------------------------------------
// system_ctrl_audio_if2ss()
// system_ctrl_audio_ss2if()
#define SYSTEM_CTRL_AUDIO_IF_SS_RSH								0x00	// Refer to Stream Header
#define SYSTEM_CTRL_AUDIO_IF_SS_16BIT							0x01	// 16 bit
#define SYSTEM_CTRL_AUDIO_IF_SS_20BIT							0x02	// 20 bit
#define SYSTEM_CTRL_AUDIO_IF_SS_24BIT							0x03	// 24 bit
//------------------------------------------------------------------------------
// system_ctrl_audio_cbit2sf()
// system_ctrl_audio_sf2cbit()
#define SYSTEM_CTRL_AUDIO_CBIT_SF_32KHZ							0x03	//   32  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_44KHZ							0x00	//   44.1kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_48KHZ							0x02	//   48  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_88KHZ							0x08	//   88.2kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_96KHZ							0x0A	//   96  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_176KHZ						0x0C	//  176.4kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_192KHZ						0x0E	//  192  kHz (AS, OBA, DST)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_64KHZ							0x0B	//   64  kHz (AS w/ 3D/Multi-Stream)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_128KHZ						0x8B	//  128  kHz (AS w/ 3D/Multi-Stream)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_256KHZ						0x4B	//  256  kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_352KHZ						0x0D	//  352.8kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_384KHZ						0x05	//  384  kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_512KHZ						0xCB	//  512  kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_705KHZ						0x8D	//  705.6kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_768KHZ						0x09	//  768  kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_1024KHZ						0xC5	// 1024  kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_1411KHZ						0x4D	// 1411.2kHz (HBR)
#define SYSTEM_CTRL_AUDIO_CBIT_SF_1536KHZ						0x45	// 1536  kHz (HBR)
//------------------------------------------------------------------------------
// system_ctrl_audio_cbit2ss()
// system_ctrl_audio_ss2cbit()
#define SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT						0x00
#define SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT						0x01
#define SYSTEM_CTRL_AUDIO_CBIT_SS_16BIT							(0x02 | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_18BIT							(0x04 | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_19BIT							(0x08 | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_20BIT_0						(0x0A | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_17BIT							(0x0C | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_20BIT_1						(0x02 | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_22BIT							(0x04 | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_23BIT							(0x08 | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_24BIT							(0x0A | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT)
#define SYSTEM_CTRL_AUDIO_CBIT_SS_21BIT							(0x0C | SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT)
//------------------------------------------------------------------------------
// SLEEP() time
#define SYSCTRL_SLEEP_TIME_200MSEC								200
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL													0
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
typedef enum _sysctrl_vform {
	SYSCTRL_VFORM_NOT_4K2K,
	SYSCTRL_VFORM_4K2K,
	SYSCTRL_VFORM_PC
} sysctrl_vform;
//------------------------------------------------------------------------------
typedef struct _system_ctrl_timing_info {
	uint32_t pixel_freq;
	uint16_t hsync;
	uint16_t hback;
	system_ctrl_polarity hpol;
	system_ctrl_polarity vpol;
} system_ctrl_timing_info;
//------------------------------------------------------------------------------
const system_ctrl_timing_info timing_infos[SYSTEM_VFORM_MAX] = {
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_UNUSED,
	{ 25175,  96,  48, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_640x480P_5994or60_4v3 = 1,
	{ 27000,  62,  60, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480P_5994or60_4v3,
	{ 27000,  62,  60, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480P_5994or60_16v9,
	{ 74250,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_5994or60_16v9,
	{ 74250,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080I_5994or60_16v9,
	{ 27000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480I_5994or60_4v3,
	{ 27000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480I_5994or60_16v9,
	{ 27000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x240P_5994or60_4v3,
	{ 27000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x240P_5994or60_16v9,
	{ 54000, 248, 228, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x480I_5994or60_4v3,			// 10
	{ 54000, 248, 228, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x480I_5994or60_16v9,
	{ 54000, 248, 228, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x240P_5994or60_4v3,
	{ 54000, 248, 228, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x240P_5994or60_16v9,
	{ 54000, 124, 120, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_1440x480P_5994or60_4v3,
	{ 54000, 124, 120, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_1440x480P_5994or60_16v9,
	{148500,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_5994or60_16v9,
	{ 27000,  64,  68, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576P_50_4v3,
	{ 27000,  64,  68, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576P_50_16v9,
	{ 74250,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_50_16v9,
	{ 74250,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080I_50_16v9,				// 20
	{ 27000, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576I_50_4v3,
	{ 27000, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576I_50_16v9,
	{ 27000, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x288P_50_4v3,
	{ 27000, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x288P_50_16v9,
	{ 54000, 252, 276, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x576I_50_4v3,
	{ 54000, 252, 276, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x576I_50_16v9,
	{ 54000, 252, 276, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x288P_50_4v3,
	{ 54000, 252, 276, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x288P_50_16v9,
	{ 54000, 128, 136, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_1440x576P_50_4v3,
	{ 54000, 128, 136, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_1440x576P_50_16v9,				// 30
	{148500,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_50_16v9,
	{ 74250,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_2398or24_16v9,
	{ 74250,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_25_16v9,
	{ 74250,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_2997or30_16v9,
	{108000, 248, 240, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x480P_5994or60_4v3,
	{108000, 248, 240, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x480P_5994or60_16v9,
	{108000, 256, 272, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x576P_50_4v3,
	{108000, 256, 272, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_2880x576P_50_16v9,
	{ 72000, 168, 184, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_1920x1080I_T1250_50_16v9,
	{148500,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080I_100_16v9,			// 40
	{148500,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_100_16v9,
	{ 54000,  64,  68, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576P_100_4v3,
	{ 54000,  64,  68, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576P_100_16v9,
	{ 54000, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576I_100_4v3,
	{ 54000, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576I_100_16v9,
	{148500,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080I_11988or120_16v9,
	{148500,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_11988or120_16v9,
	{ 54000,  62,  60, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480P_11988or120_4v3,
	{ 54000,  62,  60, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480P_11988or120_16v9,
	{ 54000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480I_11988or120_4v3,		// 50
	{ 54000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480I_11988or120_16v9,
	{ 10800,  64,  68, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576P_200_4v3,
	{ 10800,  64,  68, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576P_200_16v9,
	{ 10800, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576I_200_4v3,
	{ 10800, 126, 138, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x576I_200_16v9,
	{108000,  62,  60, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480P_23976or240_4v3,
	{108000,  62,  60, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480P_23976or240_16v9,
	{108000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480I_23976or240_4v3,
	{108000, 124, 114, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_720x480I_23976or240_16v9,
	{ 59400,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_2398or24_16v9,		// 60
	{ 74250,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_25_16v9,
	{ 74250,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1280x720P_2997or30_16v9,
	{297000,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_11988or120_16v9,
	{297000,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1920x1080P_100_16v9,			// 64 (0x40)
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},													// 70
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},													// 80
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},													// 90
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{297000,  88, 296, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_3840x2160P_2398or24_16v9 = 93,
	{297000,  88, 296, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_3840x2160P_25_16v9,
	{297000,  88, 296, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_3840x2160P_2997or30_16v9,
	{594000,  88, 296, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_3840x2160P_50_16v9,
	{594000,  88, 296, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_3840x2160P_5994or60_16v9,
	{297000,  88, 296, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_4096x2160P_2398or24_256v135,
	{297000,  88, 128, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_4096x2160P_25_256v135,
	{297000,  88, 128, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_4096x2160P_2997or30_256v135,	// 100
	{594000,  88, 128, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_4096x2160P_50_256v135,
	{594000,  88, 128, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_4096x2160P_5994or60_256v135,
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},													// 110
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},													// 120
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},
	{ 85500, 143, 213, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_1366x768P_5979or60_16v9 = 129,
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_VESA_DMT_ID_UNUSED,			// 130
	{ 31500,  64,  96, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_EGA_640x350_85,
	{ 31500,  64,  96, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_640x400_85,
	{ 35500,  72, 108, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_720x400_85,
	{ 25175,  96,( 40+8), SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},// SYSTEM_VFORM_PC_VGA_640x480_60,				// not supported
	{ 31500,  40,(120+8), SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},// SYSTEM_VFORM_PC_VGA_640x480_72,
	{ 31500,  64, 120, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_VGA_640x480_75,
	{ 36000,  56,  80, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_VGA_640x480_85,
	{ 36000,  72, 128, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SVGA_800x600_56,
	{ 40000, 128,  88, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SVGA_800x600_60,
	{ 50000, 120,  64, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SVGA_800x600_72,				// 140
	{ 49500,  80, 160, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SVGA_800x600_75,
	{ 56250,  64, 152, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SVGA_800x600_85,
	{ 73250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_SVGA_800x600_120_RB,
	{ 33750, 112, 112, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_848x480_60,
	{ 44900, 176,  56, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_XGA_1024x768_43,				// not supported
	{ 65000, 136, 160, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_XGA_1024x768_60,
	{ 75000, 136, 144, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_XGA_1024x768_70,
	{ 78750,  96, 176, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_XGA_1024x768_75,
	{ 94500,  96, 208, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_XGA_1024x768_85,
	{115500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_XGA_1024x768_120_RB,			// 150
	{108000, 128, 256, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_XGAp_1152x864_75,
	{ 68250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WXGA_1280x768_60_RB,
	{ 79500, 128, 192, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGA_1280x768_60,
	{102250, 128, 208, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGA_1280x768_75,
	{117500, 136, 216, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGA_1280x768_85,
	{140250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WXGA_1280x768_120_RB,
	{ 71000,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WSXGA_1280x800_60_RB,
	{ 83500, 128, 200, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WSXGA_1280x800_60,
	{106500, 128, 208, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WSXGA_1280x800_75,
	{122500, 136, 216, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WSXGA_1280x800_85,			// 160
	{146250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WSXGA_1280x800_120_RB,
	{108000, 112, 312, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_QVGA_1280x960_60,
	{148500, 160, 224, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_QVGA_1280x960_85,
	{175500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_QVGA_1280x960_120_RB,
	{108000, 112, 248, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SXGA_1280x1024_60,
	{135000, 144, 248, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SXGA_1280x1024_75,
	{157500, 160, 224, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SXGA_1280x1024_85,
	{187250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_SXGA_1280x1024_120_RB,
	{ 85500, 112, 256, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1360x768_60,
	{148250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_1360x768_120_RB,				// 170
	{101000,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_SXGAp_1400x1050_60_RB,
	{121750, 144, 232, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SXGAp_1400x1050_60,
	{156000, 144, 248, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SXGAp_1400x1050_75,
	{179500, 152, 256, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_SXGAp_1400x1050_85,
	{208000,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_SXGAp_1400x1050_120_RB,
	{ 88750,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WXGAp_1440x900_60_RB,
	{106500, 152, 232, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGAp_1440x900_60,
	{136750, 152, 248, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGAp_1440x900_75,
	{157000, 152, 256, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGAp_1440x900_85,
	{182750,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WXGAp_1440x900_120_RB,		// 180
	{162000, 192, 304, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_UXGA_1600x1200_60,
	{175500, 192, 304, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_UXGA_1600x1200_65,
	{189000, 192, 304, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_UXGA_1600x1200_70,
	{202500, 192, 304, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_UXGA_1600x1200_75,
	{229500, 192, 304, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_UXGA_1600x1200_85,
	{268250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_UXGA_1600x1200_120_RB,
	{119000,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WSXGAp_1680x1050_60_RB,
	{146250, 176, 280, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WSXGAp_1680x1050_60,
	{187000, 176, 296, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WSXGAp_1680x1050_75,
	{214750, 176, 304, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WSXGAp_1680x1050_85,			// 190
	{245500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WSXGAp_1680x1050_120_RB,
	{204750, 200, 328, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1792x1344_60,
	{261000, 216, 352, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1792x1344_75,
	{333250,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_1792x1344_120_RB,
	{218250, 224, 352, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1856x1392_60,
	{288000, 224, 352, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1856x1392_75,
	{356500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_1856x1392_120_RB,
	{154000,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WUXGA_1920x1200_60_RB,
	{193250, 200, 336, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WUXGA_1920x1200_60,
	{245250, 208, 344, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WUXGA_1920x1200_75,			// 200
	{281250, 208, 352, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WUXGA_1920x1200_85,
	{317000,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WUXGA_1920x1200_120_RB,		// not supported
	{234000, 208, 344, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1920x1440_60,
	{297000, 224, 352, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1920x1440_75,
	{380500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_1920x1440_120_RB,
	{268500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WQXGA_2560x1600_60_RB,
	{348500, 280, 472, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WQXGA_2560x1600_60,
	{443250, 280, 488, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WQXGA_2560x1600_75,
	{505250, 280, 488, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WQXGA_2560x1600_85,
	{552750,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WQXGA_2560x1600_120_RB,		// 210
	{ 85500, 143, 213, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_FWXGA_1366x768_60,
	{148500,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_FHD_1920x1080_60,
	{108000,  80,  96, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WXGApp_1600x900_60_RB,
	{156750,  80,  96, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_QWXGA_2048x1152_60_RB,
	{ 74250,  40, 220, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_HD_1280x720_60,
	{ 72000,  56,  64, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB,
	{556744,  32,  40, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_DCI4K_4096x2160_60_RB,		// not supported
	{556188,  32,  40, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_DCI4K_4096x2160_5994_RB,		// not supported
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_VESA_DMT_ID_MAX,
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_VARIANT_UNUSED,				// 220
	{265000, 276, 272, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_QXGA_2048x1536_60,
	{241500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_WQHD_2560x1440_60_RB,
	{297000,  80, 440, SYSTEM_CTRL_POL_N, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_WQHD_2560x1440_60,
	{297000,  44, 148, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_1920x2160_60,
	{285500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_FHD_1920x1080_120_1,
	{346500,  32,  80, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_N},	// SYSTEM_VFORM_PC_FHD_1920x1080_144_1,
	{556800,  32,  40, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P},	// SYSTEM_VFORM_PC_FHD_1920x1080_240_1,
	{     0,   0,   0, SYSTEM_CTRL_POL_P, SYSTEM_CTRL_POL_P}	// SYSTEM_VFORM_PC_VARIANT_MAX,
};
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rx_tx_saddr(system_port port);
static uint32_t sysctrl_get_rx_tx_saddr(system_port port, uint8_t *p_rx, uint8_t *p_tx);
static uint32_t sysctrl_set_intr_pin(system_port port);
static uint32_t sysctrl_release_all_power_down(system_port port);
static uint32_t sysctrl_init_config(system_port port);
static uint32_t sysctrl_set_power_tx(system_power *p_power);
static uint32_t sysctrl_set_power_vborx(system_power *p_power);
static uint32_t sysctrl_set_power_lvdsvbotx(system_power *p_power);
static uint32_t sysctrl_set_power_hdmirx(system_power *p_power);
static uint32_t sysctrl_set_power_hdmitx(system_power *p_power);
static uint32_t sysctrl_block_power_on(system_port port);
static uint32_t sysctrl_block_power_off(system_port port);
static uint32_t sysctrl_get_rx_cf(system_port port, uint8_t *p_cf);
static uint32_t sysctrl_set_rx_cf(system_port port, uint8_t cf);
static uint32_t sysctrl_get_tx_cf(system_port port, uint8_t *p_cf);
static uint32_t sysctrl_set_tx_cf(system_port port, uint8_t cf);
static uint32_t sysctrl_set_rx_soft_reset1(system_port port, uint8_t rst);
static uint32_t sysctrl_set_rx_cf_converter_update(system_port port, uint8_t upd);
static uint32_t sysctrl_convert_bt2020(system_color_format *p_rxcf, system_color_format *p_txcf);
static uint32_t sysctrl_set_rxpll0en(system_port port, uint8_t en);
static uint32_t sysctrl_set_rxpll00en(system_port port, uint8_t en);
static uint32_t sysctrl_set_rx0_sen(system_port port, uint8_t en);
static uint32_t sysctrl_get_rxpll0en(system_port port, uint8_t *p_en);
static uint32_t sysctrl_get_rxpll00en(system_port port, uint8_t *p_en);
static uint32_t sysctrl_get_rx0_sen(system_port port, uint8_t *p_en);
static uint32_t sysctrl_set_clock_mode_hdmirx_420_10_12bit(system_video *p_video);
static uint32_t sysctrl_tmds_transmitter_init(system_port port);
static uint32_t sysctrl_transmitter_link_init(system_port port);
static uint32_t sysctrl_get_vesa_dmt_id(system_video *p_video, sysctrl_vform rx_vf,
										sysctrl_vform tx_vf, uint8_t *p_id);
static uint32_t sysctrl_set_vformat(system_video *p_video, sysctrl_vform rx_vf, sysctrl_vform tx_vf);
static uint32_t sysctrl_check_vform(system_video_format vf, uint8_t rxtx);
static uint32_t sysctrl_set_de_mode(system_port port);
static uint32_t sysctrl_set_super_resolution(system_port port, sysctrl_vform rx_vf, sysctrl_vform tx_vf);
static uint32_t sysctrl_set_downscaler(system_port port, sysctrl_vform rx_vf, sysctrl_vform tx_vf);
static uint32_t sysctrl_set_480p_downscaler(system_video *p_video);
static uint32_t sysctrl_get_deep_color_mode(system_video *p_video, uint8_t *p_deepc);
static uint32_t sysctrl_set_deep_color(system_video *p_video);
static uint32_t sysctrl_srst_deep_color_ctrl(system_port port);
static uint32_t sysctrl_set_pad_mode(system_port port, uint8_t mode);
static uint32_t sysctrl_set_pad_input1(system_port port, uint8_t in1, uint8_t mask1);
static uint32_t sysctrl_set_pad_input2(system_port port, uint8_t in2);
static uint32_t sysctrl_set_pad_output(system_port port, uint8_t out);
static uint32_t sysctrl_init_audio_tx_link(system_port port);
static uint32_t sysctrl_select_input_if(system_port port, uint8_t type);
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rx_tx_saddr(system_port port)
{
	system_block_access baccess;
	uint8_t data[SYSCTRL_SET_RX_TX_SADDR_LEN];
	uint8_t rx_saddr, tx_saddr;
	uint32_t ret;

	rx_saddr = SYSCTRL_SET_RX_TX_SADDR_SADRSET_00_R0SA_BASE +
				(port.chip_num * SYSCTRL_SET_RX_TX_SADDR_OFFSET); //0x02
	tx_saddr = SYSCTRL_SET_RX_TX_SADDR_SADRSET_01_T0SA_BASE +
				(port.chip_num * SYSCTRL_SET_RX_TX_SADDR_OFFSET); //0x03

	// rx_saddr = SYSCTRL_SET_RX_TX_SADDR_SADRSET_00_R0SA_BASE + 0x04;
	// 			//(port.chip_num * SYSCTRL_SET_RX_TX_SADDR_OFFSET); //0x02
	// tx_saddr = SYSCTRL_SET_RX_TX_SADDR_SADRSET_01_T0SA_BASE + 0x04;
	// 			//(port.chip_num * SYSCTRL_SET_RX_TX_SADDR_OFFSET); //0x03
				
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM2_SADRSET_00_ADDR,
						SYSCTRL_SET_RX_TX_SADDR_LEN, data);         
	// R0SA: Rx slave address
	SET_BLOCK_DATA(data, 0, rx_saddr);										// 0x1080
	// T0SA: Tx slave address
	SET_BLOCK_DATA(data, 1, tx_saddr);										// 0x1081
	// default (padding)
	SET_BLOCK_DATA(data, 2, SYSCTRL_SET_RX_TX_SADDR_SADRSET_02_DEF);		// 0x1082
	// T0HSEL: HOST IF 0
	// R0HSEL: HOST IF 0
	SET_BLOCK_DATA(data, 3, SYSCTRL_SET_RX_TX_SADDR_HIFSET_T0HSEL_IF0 |
							SYSCTRL_SET_RX_TX_SADDR_HIFSET_R0HSEL_IF0);		// 0x1083

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_rx_tx_saddr(system_port port, uint8_t *p_rx, uint8_t *p_tx)
{
	system_block_access baccess;
	uint8_t data[SYSCTRL_GET_RX_TX_SADDR_LEN] = {0};
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM2_SADRSET_00_ADDR,
						SYSCTRL_GET_RX_TX_SADDR_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	if (p_rx)	*p_rx = data[0] & SYSCTRL_GET_RX_TX_SADDR_SADRSET_00_R0SA_MASK;	// 0x1080: Rx saddr
	if (p_tx)	*p_tx = data[1] & SYSCTRL_GET_RX_TX_SADDR_SADRSET_01_T0SA_MASK;	// 0x1081: Tx saddr

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_intr_pin(system_port port)
{
	system_block_access baccess;
	uint8_t data[SYSCTRL_SET_INTR_PIN_LEN];
	uint32_t ret;


	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_INTASSGN1_ADDR,
						SYSCTRL_SET_INTR_PIN_LEN, data);
	// T0IRQSEL: NIRQ0
	// R0IRQSELA: NIRQ0
	SET_BLOCK_DATA(data, 0, SYSCTRL_SET_INTR_PIN_INTASSGN1_T0IRQSEL_NIRQ0 |
							SYSCTRL_SET_INTR_PIN_INTASSGN1_R0IRQSELA_NIRQ0);	// 0x107C
	// R0IRQSELB: NIRQ0
	// CCIRQSEL: NIRQ0
	SET_BLOCK_DATA(data, 1, SYSCTRL_SET_INTR_PIN_INTASSGN2_R0IRQSELB_NIRQ0 |
							SYSCTRL_SET_INTR_PIN_INTASSGN2_CCIRQSEL_NIRQ0);		// 0x107D

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_release_all_power_down(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_RELEASE_ALL_POWER_DOWN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_RELEASE_ALL_POWER_DOWN_NUM);
	// ALLPDWN: normal
	// RXC0DWN: normal
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SWPDWN2_ADDR,
						SYSCTRL_RELEASE_ALL_POWER_DOWN_SWPDWN2_ALLPDWN_NOR |
							SYSCTRL_RELEASE_ALL_POWER_DOWN_SWPDWN2_RXC0DWN_NOR,
						SYSCTRL_RELEASE_ALL_POWER_DOWN_SWPDWN2_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_init_config(system_port port)
{
	uint32_t ret;

	// set I2C slave address mapping & host IF assignment
	ret = sysctrl_set_rx_tx_saddr(port);
	CHECK_ERROR(ret);
	// set interrupt pin assignment
	ret = sysctrl_set_intr_pin(port);
	CHECK_ERROR(ret);
	// release all power down
	ret = sysctrl_release_all_power_down(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_power_tx(system_power *p_power)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_POWER_TX_NUM];
	uint8_t tp0swpdwn;
	uint32_t ret;

	if (p_power == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_power->onoff == SYSTEM_ONOFF_ON) {
		tp0swpdwn = SYSCTRL_SET_POWER_TX_TXSYSCFG1_TP0SWPDWN_NOR;
	} else {
		tp0swpdwn = SYSCTRL_SET_POWER_TX_TXSYSCFG1_TP0SWPDWN_DWN;
	}

	SET_ACCESS(access, &(p_power->port), cmd, SYSCTRL_SET_POWER_TX_NUM);
	// TP0SWPDWN: Tx port
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TXSYSCFG1_ADDR,
						tp0swpdwn,
						SYSCTRL_SET_POWER_TX_TXSYSCFG1_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_power_vborx(system_power *p_power)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_POWER_VBORX_NUM];
	uint8_t npd, npdx;
	uint32_t ret;

	if (p_power == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_power->onoff == SYSTEM_ONOFF_ON) {
		npd = SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_NPD_PWR_ON;
		npdx = SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_NPD_X_PWR_ON;
	} else {
		npd = SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_NPD_PWR_DOWN;
		npdx = SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_NPD_X_PWR_DOWN;
	}

	SET_ACCESS(access, &(p_power->port), cmd, SYSCTRL_SET_POWER_VBORX_NUM);
	// PHY0: NPD: on or down
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY0_P0_ENTCNT1_0_ADDR,
						npd,
						SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_MASK);
	// PHY0: NPD_0,1,2,3: on or down
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_PHY0_P0_ENTCNT2_0_ADDR,
						npdx,
						SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_MASK);
	// PHY1: NPD: on or down
	SET_COMMAND(cmd, 2, VIDEOIF_VBORX_PHY1_P1_ENTCNT1_0_ADDR,
						npd,
						SYSCTRL_SET_POWER_VBORX_PX_ENTCNT1_0_MASK);
	// PHY1: NPD_0,1,2,3: on or down
	SET_COMMAND(cmd, 3, VIDEOIF_VBORX_PHY1_P1_ENTCNT2_0_ADDR,
						npdx,
						SYSCTRL_SET_POWER_VBORX_PX_ENTCNT2_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_power_lvdsvbotx(system_power *p_power)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_POWER_VBOTX_NUM];
	uint32_t ret;
	uint8_t npowdvbo, npdown;

	if (p_power == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_power->onoff == SYSTEM_ONOFF_ON) {
		npowdvbo = SYSCTRL_SET_POWER_VBOTX_VBOCTL21_NPOWDVBO_DIS;
		npdown  = SYSCTRL_SET_POWER_VBOTX_ENTCNT_3_NPDOWN_ON;
	} else {
		npowdvbo = SYSCTRL_SET_POWER_VBOTX_VBOCTL21_NPOWDVBO_ENA;
		npdown  = SYSCTRL_SET_POWER_VBOTX_ENTCNT_3_NPDOWN_DOWN;
	}

	SET_ACCESS(access, &(p_power->port), cmd, SYSCTRL_SET_POWER_VBOTX_NUM);
	// page: 1
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSLINKPAGESEL_ADDR,
						SYSCTRL_SET_POWER_VBOTX_PAGNUM_VBO_LINK,
						SYSCTRL_SET_POWER_VBOTX_LVDSLINKPAGESEL_MASK);
	// NPOWDVBO: enable or disable power down
	SET_COMMAND(cmd, 1, VIDEOIF_VBO_LINK_VBOCTL21_ADDR,
						npowdvbo,
						SYSCTRL_SET_POWER_VBOTX_VBOCTL21_MASK);
	// page: 2
	SET_COMMAND(cmd, 2, VIDEOIF_LVDS_LINK_LVDSLINKPAGESEL_ADDR,
						SYSCTRL_SET_POWER_VBOTX_PAGNUM_LVDSVBO_PHY,
						SYSCTRL_SET_POWER_VBOTX_LVDSLINKPAGESEL_MASK);
	// NPDOWN: power down or on
	SET_COMMAND(cmd, 3, VIDEOIF_LVDVB_PHY_ENTCNT_3_ADDR,
						npdown,
						SYSCTRL_SET_POWER_VBOTX_ENTCNT_3_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_power_hdmirx(system_power *p_power)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_SET_POWER_HDMIRX_NUM];
	int i;
	uint8_t syscfg19, tmdsrxp, rxpll0_01, rxpll0_00;
	uint32_t ret;

	if (p_power == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_power->onoff == SYSTEM_ONOFF_ON) {
		syscfg19	= SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX3SWPDWN_NOR   |
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX2SWPDWN_NOR |
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX1SWPDWN_NOR |
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX0SWPDWN_NOR;
		tmdsrxp		= SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_NPDX_NOR;
		rxpll0_01	= SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_RXPLL0EN_MAN    |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_DIVR0_DEF;
		rxpll0_00	= SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_ICPMR0_DEF      |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_FOUTSELR0_DEF |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_FDIVR0_DEF    |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_NPDOWNR0_NOR;
	} else {
		syscfg19	= SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX3SWPDWN_DWN   |
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX2SWPDWN_DWN |
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX1SWPDWN_DWN |
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_RX0SWPDWN_DWN;
		tmdsrxp		= SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_NPDX_DWN;
		rxpll0_01	= SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_RXPLL0EN_MAN    |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_DIVR0_DEF;
		rxpll0_00	= SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_ICPMR0_DEF      |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_FOUTSELR0_DEF |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_FDIVR0_DEF    |
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_NPDOWNR0_DWN;
	}

	SET_ACCESS(access, &(p_power->port), cmd, SYSTEM_CTRL_SET_POWER_HDMIRX_NUM);
	// RX*SWPDWN: Rx port * TMDS software power down control
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG19_ADDR,
						syscfg19,
						SYSTEM_CTRL_SET_POWER_HDMIRX_SYSCFG19_MASK);
	// NPD*: Rx port * TMDS power down control
	for (i=0; i<SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_PORT_NUM; i++) {
		SET_COMMAND(cmd, (i+1),
				(VIDEOIF_TMDS_TMDSRXP0_3_ADDR + SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_OFFSET * i),
				 tmdsrxp,
				 SYSTEM_CTRL_SET_POWER_HDMIRX_TMDSRXPX_3_MASK);
	}
	// RXPLL0EN: manual setting
	// DIVR0: default (padding)
	SET_COMMAND(cmd, 5, VIDEOIF_CKGEN_RXPLL0_01_ADDR,
						rxpll0_01,
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_1_MASK);
	// ICPMR0: default (padding)
	// FOUTSELR0: default (padding)
	// FDIVR0: default (padding)
	// NPDOWNR0: Rx PLL0 poser down control
	SET_COMMAND(cmd, 6, VIDEOIF_CKGEN_RXPLL0_00_ADDR,
						rxpll0_00,
						SYSTEM_CTRL_SET_POWER_HDMIRX_RXPLL0_0_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_power_hdmitx(system_power *p_power)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_POWER_HDMITX_NUM];
	uint8_t tmdstxp0_1, tmdstxp0_2;
	uint32_t ret;

	if (p_power == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_power->onoff == SYSTEM_ONOFF_ON) {
		tmdstxp0_1 = SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDTX_NOR       |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NRS_SUSP_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_SWINGNPD_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDPLL_NOR   |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDCKO_DEF;
		tmdstxp0_2 = SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDBGR_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT2_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT1_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT0_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NRSTPLL_NOR |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_TXPRM00_DEF;
	} else {
		tmdstxp0_1 = SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDTX_DWN       |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NRS_SUSP_SUS |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_SWINGNPD_DWN |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDPLL_DWN   |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_NPDCKO_DEF;
		tmdstxp0_2 = SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDBGR_DWN     |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT2_DWN |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT1_DWN |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NPDDAT0_DWN |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_NRSTPLL_RST |
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_TXPRM00_DEF;
	}

	SET_ACCESS(access, &(p_power->port), cmd, SYSCTRL_SET_POWER_HDMITX_NUM);
	// NPDTX: TMDS core all power down (Tx port0) control
	// NRS_SUSP: TMDS receiver sense block suspend (Tx port0) control
	// SWINGNPD: TMDS signal swing contrl block power down (Tx port) control
	// NPDPLL: TMDS PLL power down (Tx port0) control
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSTXP0_1_ADDR,
						tmdstxp0_1,
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_1_MASK);
	// NPDBGR: TMDS band gap reference block power down (Tx port0) control
	// NPDDAT2: TMDS data channel2 power down (Tx port0) control
	// NPDDAT1: TMDS data channel1 power down (Tx port0) control
	// NPDDAT0: TMDS data channel0 power down (Tx port0) control
	// NRSTPLL: TMDS PLL reset (Tx port0) control
	// TXPRM00: TMDS Tx parameter00 (Tx port0) control
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_TMDSTXP0_2_00_ADDR,
						tmdstxp0_2,
						SYSCTRL_SET_POWER_HDMITX_TMDSTXP0_2_00_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_block_power_on(system_port port)
{
	system_power vborx, lvdsvbotx;
	system_power hdmirx, hdmitx;
	system_power tx;
	uint32_t ret;

#if (1)|| defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2)
	vborx.onoff  = SYSTEM_ONOFF_ON;
	lvdsvbotx.onoff = SYSTEM_ONOFF_ON;
	hdmirx.onoff = SYSTEM_ONOFF_ON;
	hdmitx.onoff = SYSTEM_ONOFF_ON;
#elif defined(CONFIG_HV_SC1H24BJ2)
	vborx.onoff  = SYSTEM_ONOFF_OFF;
	lvdsvbotx.onoff = SYSTEM_ONOFF_OFF;
	hdmirx.onoff = SYSTEM_ONOFF_ON;
	hdmitx.onoff = SYSTEM_ONOFF_ON;
#elif defined(CONFIG_HV_SC1H25BJ2)
	vborx.onoff  = SYSTEM_ONOFF_OFF;
	lvdsvbotx.onoff = SYSTEM_ONOFF_ON;
	hdmirx.onoff = SYSTEM_ONOFF_ON;
	hdmitx.onoff = SYSTEM_ONOFF_OFF;
#elif defined(CONFIG_HV_SC1H26BJ2)
	vborx.onoff  = SYSTEM_ONOFF_OFF;
	lvdsvbotx.onoff = SYSTEM_ONOFF_ON;
	hdmirx.onoff = SYSTEM_ONOFF_ON;
	hdmitx.onoff = SYSTEM_ONOFF_OFF;
#elif defined(CONFIG_HV_SC1H27BJ2)
	vborx.onoff  = SYSTEM_ONOFF_ON;
	lvdsvbotx.onoff = SYSTEM_ONOFF_OFF;
	hdmirx.onoff = SYSTEM_ONOFF_OFF;
	hdmitx.onoff = SYSTEM_ONOFF_ON;
#elif defined(CONFIG_HV_SC1H28BJ2)
	vborx.onoff  = SYSTEM_ONOFF_ON;
	lvdsvbotx.onoff = SYSTEM_ONOFF_ON;
	hdmirx.onoff = SYSTEM_ONOFF_OFF;
	hdmitx.onoff = SYSTEM_ONOFF_OFF;
#elif defined(CONFIG_HV_SC1H29BJ2)
	vborx.onoff  = SYSTEM_ONOFF_ON;
	lvdsvbotx.onoff = SYSTEM_ONOFF_ON;
	hdmirx.onoff = SYSTEM_ONOFF_OFF;
	hdmitx.onoff = SYSTEM_ONOFF_OFF;
#endif
	vborx.port = port;
	lvdsvbotx.port = port;
	hdmirx.port = port;
	hdmitx.port = port;
	// Tx port
	if (lvdsvbotx.onoff == SYSTEM_ONOFF_ON || hdmitx.onoff == SYSTEM_ONOFF_ON) {
		tx.onoff = SYSTEM_ONOFF_ON;
		tx.port = port;
		ret = sysctrl_set_power_tx(&tx);
		CHECK_ERROR(ret);
	}
	// VBO Rx
	ret = sysctrl_set_power_vborx(&vborx);
	CHECK_ERROR(ret);
	// LVDS/VBO Tx
	ret = sysctrl_set_power_lvdsvbotx(&lvdsvbotx);
	CHECK_ERROR(ret);
	// HDMI Rx
	ret = sysctrl_set_power_hdmirx(&hdmirx);
	CHECK_ERROR(ret);
	// HDMI Tx
	ret = sysctrl_set_power_hdmitx(&hdmitx);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_block_power_off(system_port port)
{
	system_power power;
	uint32_t ret;

	power.port  = port;
	power.onoff = SYSTEM_ONOFF_OFF;

	// individual power on
	switch (port.rxtx_type) {
		case SYSTEM_RXTX_TYPE_RX:
			switch (port.port_type) {
				case SYSTEM_PORT_TYPE_HDMI:							// HDMI Rx
					ret = sysctrl_set_power_hdmirx(&power);
					CHECK_ERROR(ret);
					break;
				case SYSTEM_PORT_TYPE_VBO:							// VBO Rx
					ret = sysctrl_set_power_vborx(&power);
					CHECK_ERROR(ret);
					break;
				default:
					ret = ERR_NG;
					break;
			}
			break;
		case SYSTEM_RXTX_TYPE_TX:
			switch (port.port_type) {
				case SYSTEM_PORT_TYPE_HDMI:							// HDMI Tx
					ret = sysctrl_set_power_hdmitx(&power);
					CHECK_ERROR(ret);
					break;
				case SYSTEM_PORT_TYPE_VBO:							// VBO Tx
				case SYSTEM_PORT_TYPE_LVDS:							// LVDS Tx
					ret = sysctrl_set_power_lvdsvbotx(&power);
					CHECK_ERROR(ret);
					break;
				default:
					ret = ERR_NG;
					break;
			}
			break;
		default:
			ret = ERR_NG;
			break;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_init(system_port port)
{
	uint8_t rx_addr, tx_addr;
	system_power power;
	uint32_t ret;

	// check rx tx slave address
	ret = sysctrl_get_rx_tx_saddr(port, &rx_addr, &tx_addr);
    printf("%d, %d\n",rx_addr,tx_addr);
	CHECK_ERROR(ret);
	if (rx_addr == SYSTEM_CTRL_INIT_SADRSET_00_R0SA_DEF ||
		tx_addr == SYSTEM_CTRL_INIT_SADRSET_01_T0SA_DEF) {
		// initial configuration
		ret = sysctrl_init_config(port);
		CHECK_ERROR(ret);
		// power on function blocks
		ret = sysctrl_block_power_on(port);
		CHECK_ERROR(ret);
	} else {
		// individual power on
		power.port = port;
        printf("power on %d\n",port);
		power.onoff = SYSTEM_ONOFF_ON;
		switch (port.rxtx_type) {
			case SYSTEM_RXTX_TYPE_RX:
				switch (port.port_type) {
					case SYSTEM_PORT_TYPE_HDMI:						// HDMI Rx
						ret = sysctrl_set_power_hdmirx(&power);
						CHECK_ERROR(ret);
						break;
					case SYSTEM_PORT_TYPE_VBO:						// VBO Rx
						ret = sysctrl_set_power_vborx(&power);
						CHECK_ERROR(ret);
						break;
					default:
						ret = ERR_NG;
						break;
				}
				break;
			case SYSTEM_RXTX_TYPE_TX:
				switch (port.port_type) {
					case SYSTEM_PORT_TYPE_HDMI:						// HDMI Tx
						ret = sysctrl_set_power_hdmitx(&power);
						CHECK_ERROR(ret);
						break;
					case SYSTEM_PORT_TYPE_VBO:						// VBO Tx
					case SYSTEM_PORT_TYPE_LVDS:						// LVDS Tx
						ret = sysctrl_set_power_lvdsvbotx(&power);
						CHECK_ERROR(ret);
						break;
					default:
						ret = ERR_NG;
						break;
				}
				break;
			default:
				ret = ERR_NG;
				break;
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_exit(system_port port)
{
	uint32_t ret;

	ret = sysctrl_block_power_off(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_check_vborx_init(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_CHECK_VBORX_INIT_NUM];
	uint8_t rx_addr;
	uint32_t ret;

	// check rx slave address
	ret = sysctrl_get_rx_tx_saddr(port, &rx_addr, NULL);
	CHECK_ERROR(ret);
	if (rx_addr == SYSTEM_CTRL_CHECK_VBORX_INIT_SADRSET_00_R0SA_DEF) {
		// cannot call vborx_init()
		return ERR_VBORX_INIT;
	}
	MES(" check rx_addr: 0x%x", rx_addr);

	SET_ACCESS(access, &(port), cmd, SYSTEM_CTRL_CHECK_VBORX_INIT_NUM);
	// check VBORX or HDMIRX selected
	SET_COMMAND(cmd, 0, VIDEOIF_CC_MISC_ADDR,
						0,
						SYSTEM_CTRL_CHECK_VBORX_INIT_MISC_VBOREN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	if (cmd[0].data != SYSTEM_CTRL_CHECK_VBORX_INIT_MISC_VBOREN_VBORX) {
		// cannot call vborx_init()
		return ERR_VBORX_INIT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_check_vbotx_init(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_CHECK_VBOTX_INIT_NUM];
	uint8_t tx_addr;
	uint32_t ret;

	// check tx slave address
	ret = sysctrl_get_rx_tx_saddr(port, NULL, &tx_addr);
	CHECK_ERROR(ret);
	if (tx_addr == SYSTEM_CTRL_CHECK_VBOTX_INIT_SADRSET_01_T0SA_DEF) {
		// cannot call vbotx_init()
		return ERR_VBOTX_INIT;
	}

	SET_ACCESS(access, &(port), cmd, SYSTEM_CTRL_CHECK_VBOTX_INIT_NUM);
	// check VBOTX selected
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR,
						0,
						SYSTEM_CTRL_CHECK_VBOTX_INIT_SWMODE4_01_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	if (cmd[0].data != SYSTEM_CTRL_CHECK_VBOTX_INIT_SWMODE4_01_VBO) {
		// cannot call vbotx_init()
		return ERR_VBOTX_INIT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_check_hdmirx_init(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_CHECK_HDMIRX_INIT_NUM];
	uint8_t rx_addr;
	uint32_t ret;

	// check rx slave address
	ret = sysctrl_get_rx_tx_saddr(port, &rx_addr, NULL);
	CHECK_ERROR(ret);
	if (rx_addr == SYSTEM_CTRL_CHECK_HDMIRX_INIT_SADRSET_00_R0SA_DEF) {
		// cannot call hdmirx_init()
		return ERR_HDMIRX_INIT;
	}

	SET_ACCESS(access, &(port), cmd, SYSTEM_CTRL_CHECK_HDMIRX_INIT_NUM);
	// check VBORX or HDMIRX selected
	SET_COMMAND(cmd, 0, VIDEOIF_CC_MISC_ADDR,
						0,
						SYSTEM_CTRL_CHECK_HDMIRX_INIT_MISC_VBOREN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	if (cmd[0].data != SYSTEM_CTRL_CHECK_HDMIRX_INIT_MISC_VBOREN_HDMIRX) {
		// cannot call hdmirx_init()
		return ERR_HDMIRX_INIT;
	}

	SET_ACCESS(access, &(port), cmd, SYSTEM_CTRL_CHECK_HDMIRX_INIT_NUM);
	// check select RX LINK of Audio data
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADAINCTRL1_ADDR,
						0,
						SYSTEM_CTRL_CHECK_HDMIRX_INIT_PADAINCTRL1_TXAIL0SEL_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	if (cmd[0].data != SYSTEM_CTRL_CHECK_HDMIRX_INIT_PADAINCTRL1_RXLINK) {
		// cannot call hdmirx_init()
		return ERR_HDMIRX_INIT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_check_hdmitx_init(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_CHECK_HDMITX_INIT_NUM];
	uint8_t tx_addr;
	uint32_t ret;

	// check tx slave address
	ret = sysctrl_get_rx_tx_saddr(port, NULL, &tx_addr);
	CHECK_ERROR(ret);
	if (tx_addr == SYSTEM_CTRL_CHECK_HDMITX_INIT_SADRSET_01_T0SA_DEF) {
		// cannot call hdmitx_init()
		return ERR_HDMITX_INIT;
	}

	SET_ACCESS(access, &(port), cmd, SYSTEM_CTRL_CHECK_HDMITX_INIT_NUM);
	// check digital PLL auto reset mode
	SET_COMMAND(cmd, 0, VIDEOIF_INF_CTSGEN_02_ADDR,
						0,
						SYSTEM_CTRL_CHECK_HDMITX_INIT_CTSGEN_2_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	if (cmd[0].data == SYSTEM_CTRL_CHECK_HDMITX_INIT_CTSGEN_2_DEFAULT) {
		// cannot call hdmitx_init()
		return ERR_HDMITX_INIT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_check_lvdstx_init(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_CHECK_LVDSTX_INIT_NUM];
	uint8_t tx_addr;
	uint32_t ret;

	// check tx slave address
	ret = sysctrl_get_rx_tx_saddr(port, NULL, &tx_addr);
	CHECK_ERROR(ret);
	if (tx_addr == SYSTEM_CTRL_CHECK_LVDSTX_INIT_SADRSET_01_T0SA_DEF) {
		// cannot call lvdstx_init()
		return ERR_LVDSTX_INIT;
	}

	SET_ACCESS(access, &(port), cmd, SYSTEM_CTRL_CHECK_LVDSTX_INIT_NUM);
	// check LVDSTX selected
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR,
						0,
						SYSTEM_CTRL_CHECK_LVDSTX_INIT_SWMODE4_01_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	if (cmd[0].data != SYSTEM_CTRL_CHECK_LVDSTX_INIT_SWMODE4_01_LVDS) {
		// cannot call lvdstx_init()
		return ERR_LVDSTX_INIT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_rx_cf(system_port port, uint8_t *p_cf)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_GET_RX_CF_NUM];
	uint32_t ret;

	if (p_cf == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, SYSCTRL_GET_RX_CF_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CC_CSCMOD3_ADDR,
						0,
						SYSCTRL_CSCMOD3_ICFMT_MASK
						| SYSCTRL_CSCMOD3_OCFMT_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_cf = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rx_cf(system_port port, uint8_t cf)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RX_CF_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RX_CF_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CC_CSCMOD3_ADDR,
						cf,
						SYSCTRL_CSCMOD3_ICFMT_MASK
						| SYSCTRL_CSCMOD3_OCFMT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_tx_cf(system_port port, uint8_t *p_cf)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_GET_TX_CF_NUM];
	uint32_t ret;

	if (p_cf == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, SYSCTRL_GET_TX_CF_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CSC_CFMT_ADDR,
						0,
						SYSCTRL_CFMT_ICFMT_MASK
						| SYSCTRL_CFMT_OCFMT_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_cf = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_tx_cf(system_port port, uint8_t cf)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RX_CF_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RX_CF_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CSC_CFMT_ADDR,
						cf,
						SYSCTRL_CFMT_ICFMT_MASK
						| SYSCTRL_CFMT_OCFMT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rx_soft_reset1(system_port port, uint8_t rst)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RX_SOFT_RESET1_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RX_SOFT_RESET1_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_RSRST1_ADDR,
						rst,
						SYSCTRL_RSRST1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rx_cf_converter_update(system_port port, uint8_t upd)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RX_CF_CONVERTER_UPDATE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RX_CF_CONVERTER_UPDATE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CC_CSCUPD_ADDR,
						upd,
						SYSCTRL_CSCUPD_ICFASET_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_convert_bt2020(system_color_format *p_rxcf, system_color_format *p_txcf)
{
	uint8_t is_bt2020 = 0;

	if (p_rxcf == NULL || p_txcf == NULL) {
		return ERR_NULL_POINTER;
	}

	// check BT.2020 of Rx
	if (*p_rxcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT2020
		|| *p_rxcf == SYSTEM_COLOR_YCBCR422_ITU_R_BT2020
		|| *p_rxcf == SYSTEM_COLOR_YCBCR444_ITU_R_BT2020
		|| *p_rxcf == SYSTEM_COLOR_RGB_ITU_R_BT2020)
	{
		is_bt2020++;
	}
	// check BT.2020 of Tx
	if (*p_txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT2020
		|| *p_txcf == SYSTEM_COLOR_YCBCR422_ITU_R_BT2020
		|| *p_txcf == SYSTEM_COLOR_YCBCR444_ITU_R_BT2020
		|| *p_txcf == SYSTEM_COLOR_RGB_ITU_R_BT2020)
	{
		is_bt2020++;
	}
	if (is_bt2020 == 0) {
		// not BT.2020
		return ERR_OK;
	} else {
		// convert BT.2020 to YCbCr BT.709 or RGB Full Range1
		if (*p_rxcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT2020) {
			*p_rxcf = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;
		} else if (*p_rxcf == SYSTEM_COLOR_YCBCR422_ITU_R_BT2020) {
			*p_rxcf = SYSTEM_COLOR_YCBCR422_ITU_R_BT709;
		} else if (*p_rxcf == SYSTEM_COLOR_YCBCR444_ITU_R_BT2020) {
			*p_rxcf = SYSTEM_COLOR_YCBCR444_ITU_R_BT709;
		} else if (*p_rxcf == SYSTEM_COLOR_RGB_ITU_R_BT2020) {
			*p_rxcf = SYSTEM_COLOR_RGB_FULL_RANGE1;
		}
		if (*p_txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT2020) {
			*p_txcf = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;
		} else if (*p_txcf == SYSTEM_COLOR_YCBCR422_ITU_R_BT2020) {
			*p_txcf = SYSTEM_COLOR_YCBCR422_ITU_R_BT709;
		} else if (*p_txcf == SYSTEM_COLOR_YCBCR444_ITU_R_BT2020) {
			*p_txcf = SYSTEM_COLOR_YCBCR444_ITU_R_BT709;
		} else if (*p_txcf == SYSTEM_COLOR_RGB_ITU_R_BT2020) {
			*p_txcf = SYSTEM_COLOR_RGB_FULL_RANGE1;
		}
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rxpll0en(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RXPLL0EN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RXPLL0EN_NUM);
	// 0x1E41: RXPLL0EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL0_01_ADDR,
						en,
						SYSCTRL_RXPLL0_01_RXPLL0EN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rxpll00en(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RXPLL00EN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RXPLL00EN_NUM);
	// 0x1E45: RXPLL00EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL00_03_ADDR,
						en,
						SYSCTRL_RXPLL00_03_RXPLL00EN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_rx0_sen(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_RX0_SEN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_RX0_SEN_NUM);
	// 0x1E7F: RX0_SEN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						en,
						SYSCTRL_CKSELEN_RX0_SEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_rxpll0en(system_port port, uint8_t *p_en)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_GET_RXPLL0EN_NUM];
	uint32_t ret;

	if (p_en == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, SYSCTRL_GET_RXPLL0EN_NUM);
	// 0x1E41: RXPLL0EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL0_01_ADDR,
						0,
						SYSCTRL_RXPLL0_01_RXPLL0EN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_en = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_rxpll00en(system_port port, uint8_t *p_en)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_GET_RXPLL00EN_NUM];
	uint32_t ret;

	if (p_en == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, SYSCTRL_GET_RXPLL00EN_NUM);
	// 0x1E45: RXPLL00EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL00_03_ADDR,
						0,
						SYSCTRL_RXPLL00_03_RXPLL00EN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_en = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_rx0_sen(system_port port, uint8_t *p_en)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_GET_RX0_SEN_NUM];
	uint32_t ret;

	if (p_en == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, SYSCTRL_GET_RX0_SEN_NUM);
	// 0x1E7F: RX0_SEN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						0,
						SYSCTRL_CKSELEN_RX0_SEN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_en = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_clock_mode_hdmirx_420_10_12bit(system_video *p_video)
{
	system_port port;
	uint8_t rxpll0en, rxpll00en, rx0_sen;
	uint8_t softreset = 0;
	uint32_t ret = ERR_OK;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;

	if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_HDMI) {				// HDMI-Rx
		if (is_color_format_yuv420(p_video->rx_color_format)) {				// HDMI-Rx YCbCr420
			if ((p_video->rx_deep_color == SYSTEM_DEEP_COLOR_10BIT)			// HDMI-Rx 10bit
				|| (p_video->rx_deep_color == SYSTEM_DEEP_COLOR_12BIT)		// HDMI-Rx 12bit
				|| (p_video->rx_deep_color == SYSTEM_DEEP_COLOR_MAX)) {		// HDMI-Rx 16bit
				// get current setting
				ret = sysctrl_get_rxpll0en(port, &rxpll0en);
				CHECK_ERROR(ret);
				ret = sysctrl_get_rxpll00en(port, &rxpll00en);
				CHECK_ERROR(ret);
				ret = sysctrl_get_rx0_sen(port, &rx0_sen);
				CHECK_ERROR(ret);
				// it is overwrite of hdmirx_set_rxpll() and hdmirx_set_rxcore().
				if (is_color_format_yuv420(p_video->tx_color_format)) {		// Tx YCbCr420
					// auto mode
					if (rxpll0en || rxpll00en || rx0_sen) {
						ret = sysctrl_set_rxpll0en(port, SYSCTRL_RXPLL0_01_RXPLL0EN_AUTO);
						CHECK_ERROR(ret);
						ret = sysctrl_set_rxpll00en(port, SYSCTRL_RXPLL00_03_RXPLL00EN_AUTO);
						CHECK_ERROR(ret);
						ret = sysctrl_set_rx0_sen(port, SYSCTRL_CKSELEN_RX0_SEN_AUTO);
						CHECK_ERROR(ret);
						softreset++;
					}
				} else {
					// manual mode
					if (!rxpll0en || !rxpll00en || !rx0_sen) {
						ret = sysctrl_set_rxpll0en(port, SYSCTRL_RXPLL0_01_RXPLL0EN_MANUAL);
						CHECK_ERROR(ret);
						ret = sysctrl_set_rxpll00en(port, SYSCTRL_RXPLL00_03_RXPLL00EN_MANUAL);
						CHECK_ERROR(ret);
						ret = sysctrl_set_rx0_sen(port, SYSCTRL_CKSELEN_RX0_SEN_MANUAL);
						CHECK_ERROR(ret);
						softreset++;
					}
				}
				if (softreset) {
					// soft reset video I/F
					ret = sysctrl_set_rx_soft_reset1(port, SYSCTRL_RSRST1_VIFRST);
					CHECK_ERROR(ret);
					// wait for 200ms
					SLEEP(SYSCTRL_SLEEP_TIME_200MSEC);
				}
			}
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_set_color_format(system_video *p_video)
{
	system_port port;
	system_color_format rxcf, txcf;
	uint8_t rxcfmt_set, txcfmt_set, rxcfmt_get, txcfmt_get;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;
	rxcf = p_video->rx_color_format;
	txcf = p_video->tx_color_format;

	// check invalid color format
	if (rxcf == SYSTEM_COLOR_UNUSED1 || rxcf == SYSTEM_COLOR_UNUSED2
		|| rxcf == SYSTEM_COLOR_UNUSED3 || rxcf >= SYSTEM_COLOR_MAX) {
		return ERR_HDMITX_COLOR_FORMAT;
	}
	if (txcf == SYSTEM_COLOR_UNUSED1 || txcf == SYSTEM_COLOR_UNUSED2
		|| txcf == SYSTEM_COLOR_UNUSED3 || txcf >= SYSTEM_COLOR_MAX) {
		return ERR_HDMITX_COLOR_FORMAT;
	}
	// check and convert BT.2020
	ret = sysctrl_convert_bt2020(&rxcf, &txcf);
	CHECK_ERROR(ret);
	// check video formats and set RXPLL manual mode & RXCORE clock selector
	// when HDMI Rx input and no color space conversion from 420 10/12bit,
	// RXPLL0EN, RXPLL00EN and RX0_SEN must be set auto mode.
	// in other cases, not care
	ret = sysctrl_set_clock_mode_hdmirx_420_10_12bit(p_video);
	CHECK_ERROR(ret);
	// set color format conversion rule
	if ((rxcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT601 && txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT709)
		|| (rxcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT709 && txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT601)) {
		// conversion between 420 BT.601 and 420 BT.709 is not supported in the same Link.
		// therefore 420 is converted into another color format once.
		// 0x2272: ICFMT: 420 (BT.601), OCFMT: RGB
		// 0x30C2: ICFMT: RGB,          OCFMT: 420 (BT.709)
		rxcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | SYSTEM_COLOR_RGB_FULL_RANGE1;
		txcfmt_set = (SYSTEM_COLOR_RGB_FULL_RANGE1 << SYSTEM_CTRL_ICFMT_BIT) | txcf;
	} else if ((txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT601 || txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT709)
				&& (rxcf != txcf)) {
		// conversion into 420 BT.601/709
		// 0x2272: ICFMT: rxcf,         OCFMT: rxcf
		// 0x30C2: ICFMT: rxcf,         OCFMT: txcf
		rxcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | rxcf;
		txcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | txcf;
	} else {
		// 0x2272: ICFMT: rxcf,         OCFMT: txcf
		// 0x30C2: ICFMT: txcf,         OCFMT: txcf
		rxcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | txcf;
		txcfmt_set = (txcf << SYSTEM_CTRL_ICFMT_BIT) | txcf;
	}
	// check current ICFMT and OCFMT setting
	ret = sysctrl_get_rx_cf(port, &rxcfmt_get);
	CHECK_ERROR(ret);
	ret = sysctrl_get_tx_cf(port, &txcfmt_get);
	CHECK_ERROR(ret);
	if (rxcfmt_set != rxcfmt_get) {
		// update rx ICFMT and OCFMT of 0x2272
		ret = sysctrl_set_rx_cf(port, rxcfmt_set);
		CHECK_ERROR(ret);
		// disable auto setting of input color format according to AVI InfoFrame
		ret = sysctrl_set_rx_cf_converter_update(port, SYSCTRL_CSCUPD_ICFASET_ICFMT);
		CHECK_ERROR(ret);
		// when only ICFMT is changed, especially switching between 420 and others,
		// VIFRST must be performed
		if ((rxcfmt_set & SYSCTRL_CSCMOD3_ICFMT_MASK)
			!= (rxcfmt_get & SYSCTRL_CSCMOD3_ICFMT_MASK)) {
			// soft reset video I/F
			ret = sysctrl_set_rx_soft_reset1(port, SYSCTRL_RSRST1_VIFRST);
			CHECK_ERROR(ret);
			// wait for 200ms
			SLEEP(SYSCTRL_SLEEP_TIME_200MSEC);
		}
	}
	if (txcfmt_set != txcfmt_get) {
		// update rx ICFMT and OCFMT of 0x30C2
		ret = sysctrl_set_tx_cf(port, txcfmt_set);
		CHECK_ERROR(ret);
		// ALLUPD, VIFUPD and CLKUPD must be performed when 420 conversion.
	}

	return ret;
}


//------------------------------------------------------------------------------
uint32_t system_ctrl_set_color_format_HDMI(system_video *p_video)
{
	system_port port;
	system_color_format rxcf, txcf;
	uint8_t rxcfmt_set, txcfmt_set, rxcfmt_get, txcfmt_get;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;
	rxcf = p_video->rx_color_format;
	txcf = p_video->tx_color_format;

	// check invalid color format
	if (rxcf == SYSTEM_COLOR_UNUSED1 || rxcf == SYSTEM_COLOR_UNUSED2
		|| rxcf == SYSTEM_COLOR_UNUSED3 || rxcf >= SYSTEM_COLOR_MAX) {
		return ERR_HDMITX_COLOR_FORMAT;
	}
	if (txcf == SYSTEM_COLOR_UNUSED1 || txcf == SYSTEM_COLOR_UNUSED2
		|| txcf == SYSTEM_COLOR_UNUSED3 || txcf >= SYSTEM_COLOR_MAX) {
		return ERR_HDMITX_COLOR_FORMAT;
	}
	// check and convert BT.2020
	ret = sysctrl_convert_bt2020(&rxcf, &txcf);
	CHECK_ERROR(ret);
	// check video formats and set RXPLL manual mode & RXCORE clock selector
	// when HDMI Rx input and no color space conversion from 420 10/12bit,
	// RXPLL0EN, RXPLL00EN and RX0_SEN must be set auto mode.
	// in other cases, not care
	ret = sysctrl_set_clock_mode_hdmirx_420_10_12bit(p_video);
	CHECK_ERROR(ret);
	// set color format conversion rule
	if ((rxcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT601 && txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT709)
		|| (rxcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT709 && txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT601)) {
		// conversion between 420 BT.601 and 420 BT.709 is not supported in the same Link.
		// therefore 420 is converted into another color format once.
		// 0x2272: ICFMT: 420 (BT.601), OCFMT: RGB
		// 0x30C2: ICFMT: RGB,          OCFMT: 420 (BT.709)
		rxcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | SYSTEM_COLOR_RGB_FULL_RANGE1;
		txcfmt_set = (SYSTEM_COLOR_RGB_FULL_RANGE1 << SYSTEM_CTRL_ICFMT_BIT) | txcf;
	} else if ((txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT601 || txcf == SYSTEM_COLOR_YCBCR420_ITU_R_BT709)
				&& (rxcf != txcf)) {
		// conversion into 420 BT.601/709
		// 0x2272: ICFMT: rxcf,         OCFMT: rxcf
		// 0x30C2: ICFMT: rxcf,         OCFMT: txcf
		rxcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | rxcf;
		txcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | txcf;
	} else {
		// 0x2272: ICFMT: rxcf,         OCFMT: txcf
		// 0x30C2: ICFMT: txcf,         OCFMT: txcf
		rxcfmt_set = (rxcf << SYSTEM_CTRL_ICFMT_BIT) | txcf;
		txcfmt_set = (txcf << SYSTEM_CTRL_ICFMT_BIT) | txcf;
	}
	// check current ICFMT and OCFMT setting
	ret = sysctrl_get_rx_cf(port, &rxcfmt_get);
	CHECK_ERROR(ret);
	ret = sysctrl_get_tx_cf(port, &txcfmt_get);
	CHECK_ERROR(ret);
	if (rxcfmt_set != rxcfmt_get) {
		// update rx ICFMT and OCFMT of 0x2272
		ret = sysctrl_set_rx_cf(port, rxcfmt_set);
		CHECK_ERROR(ret);
		// disable auto setting of input color format according to AVI InfoFrame
		ret = sysctrl_set_rx_cf_converter_update(port, SYSCTRL_CSCUPD_ICFASET_ICFMT);
		CHECK_ERROR(ret);
		// when only ICFMT is changed, especially switching between 420 and others,
		// VIFRST must be performed
		if ((rxcfmt_set & SYSCTRL_CSCMOD3_ICFMT_MASK)
			!= (rxcfmt_get & SYSCTRL_CSCMOD3_ICFMT_MASK)) {
			// soft reset video I/F
			ret = sysctrl_set_rx_soft_reset1(port, 0x00);
			CHECK_ERROR(ret);
			// wait for 200ms
			SLEEP(SYSCTRL_SLEEP_TIME_200MSEC);
		}
	}
	if (txcfmt_set != txcfmt_get) {
		// update rx ICFMT and OCFMT of 0x30C2
		ret = sysctrl_set_tx_cf(port, txcfmt_set);
		CHECK_ERROR(ret);
		// ALLUPD, VIFUPD and CLKUPD must be performed when 420 conversion.
	}

	return ret;
}


//------------------------------------------------------------------------------
static uint32_t sysctrl_tmds_transmitter_init(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_TMDS_TX_INIT_NUM];
	uint8_t datinv, chswp;
	uint32_t ret;

	// TMDS data polarity inversion (depending on PCB)
#ifdef CONFIG_TX_TMDS_DATA_POLARITY_INVERSION
	datinv = SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_DATINV_REV;
#else // CONFIG_TX_TMDS_DATA_POLARITY_INVERSION
	datinv = SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_DATINV_NOR;
#endif //CONFIG_TX_TMDS_DATA_POLARITY_INVERSION
	// TMDS data channel swap (depending on PCB)
#ifdef CONFIG_TX_TMDS_DATA_CHANNEL_SWAP
	chswp = SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_CHSWP_SWP;
#else // CONFIG_TX_TMDS_DATA_CHANNEL_SWAP
	chswp = SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_CHSWP_NOR;
#endif // CONFIG_TX_TMDS_DATA_CHANNEL_SWAP

	SET_ACCESS(access, &(port), cmd, SYSCTRL_TMDS_TX_INIT_NUM);
	// DATINV: normal or reversed
	// CHSWP: normal or swapped
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_TXENCSET4_00_ADDR,
						datinv | chswp,
						SYSCTRL_TMDS_TX_INIT_TXENCSET4_00_MASK);
	// set scramble mode
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM2_TXENCSET4_02_ADDR,
						SYSCTRL_TMDS_TX_INIT_TXENCSET4_02_SCRMOD_HDMI2,
						SYSCTRL_TMDS_TX_INIT_TXENCSET4_02_SCRMOD_MASK);
	// set interrupt mask
	SET_COMMAND(cmd, 2, VIDEOIF_SYSTEM_INTMSK_0A_ADDR,
						SYSCTRL_TMDS_TX_INIT_INTMSK_0A_IRAMSK_ALL,
						SYSCTRL_TMDS_TX_INIT_INTMSK_0A_IRAMSK_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_transmitter_link_init(system_port port)
{
	system_block_access baccess;
	uint8_t data[SYSCTRL_TX_LINK_INIT_LEN];
	uint32_t ret;

	// set interrupt mask
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_IRQ_TINTMSK_00_ADDR,
						SYSCTRL_TX_LINK_INIT_LEN, data);
	SET_BLOCK_DATA(data, 0, SYSCTRL_TX_LINK_INIT_TINTMSK_00_INTS0MSK_FF);		// 0x3098
	SET_BLOCK_DATA(data, 1, SYSCTRL_TX_LINK_INIT_TINTMSK_01_INTS1MSK_00);		// 0x3099
	SET_BLOCK_DATA(data, 2, SYSCTRL_TX_LINK_INIT_TINTMSK_02_INTS2MSK_0F);		// 0x309A
	SET_BLOCK_DATA(data, 3, SYSCTRL_TX_LINK_INIT_TINTMSK_03_INTS3MSK_00);		// 0x309B
	SET_BLOCK_DATA(data, 4, SYSCTRL_TX_LINK_INIT_TINTMSK_03_INTS4MSK_F0);		// 0x309C
	SET_BLOCK_DATA(data, 5, SYSCTRL_TX_LINK_INIT_TINTMSK_05_INTS5MSK_00);		// 0x309D
	SET_BLOCK_DATA(data, 6, SYSCTRL_TX_LINK_INIT_TINTMSK_06_INTS6MSK_FF);		// 0x309E
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_init(system_port port)
{
	uint32_t ret;

	// TMDS Transmitter Initialization (2.1.5)
	ret = sysctrl_tmds_transmitter_init(port);
	CHECK_ERROR(ret);
	// Transmitter Link Initialization (2.1.6)
	ret = sysctrl_transmitter_link_init(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_video_mute(system_mute *p_mute)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_TX_VIDEO_MUTE_NUM];
	uint8_t onoff;
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &p_mute->port, cmd, SYSTEM_CTRL_TX_VIDEO_MUTE_NUM);
	if (p_mute->mute == SYSTEM_TX_MUTE_ON) {
		onoff = SYSTEM_CTRL_TX_VIDEO_MUTE_VMUTECNT_MUTE_ON;
	} else if(p_mute->mute == SYSTEM_TX_MUTE_OFF) {
		onoff = SYSTEM_CTRL_TX_VIDEO_MUTE_VMUTECNT_MUTE_OFF;
	} else {
		return ERR_NG;
	}
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_VMUTECNT_ADDR,
						onoff,
						SYSTEM_CTRL_TX_VIDEO_MUTE_VMUTECNT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_video_avmute(system_mute *p_mute)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_TX_VIDEO_AVMUTE_NUM];
	uint8_t clear_avmute, set_avmute;
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_mute->mute == SYSTEM_TX_MUTE_ON_AVMUTE) {
		clear_avmute = SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_CLEAR_AVMUTE_OFF;
		set_avmute   = SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_SET_AVMUTE_ON;
	} else if (p_mute->mute == SYSTEM_TX_MUTE_OFF_AVMUTE) {
		clear_avmute = SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_CLEAR_AVMUTE_ON;
		set_avmute   = SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_SET_AVMUTE_OFF;
	} else {
		return ERR_NG;
	}

	SET_ACCESS(access, &p_mute->port, cmd, SYSTEM_CTRL_TX_VIDEO_AVMUTE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_GCPPKT_03_ADDR,
						clear_avmute                                            |
							set_avmute,
						SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_CLEAR_AVMUTE_MASK |
							SYSTEM_CTRL_TX_VIDEO_AVMUTE_GCPPKT_03_SET_AVMUTE_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM_UPDCTRL_ADDR,
						SYSTEM_CTRL_TX_VIDEO_AVMUTE_UPDCTRL_CSCUPD_UPD,
						SYSTEM_CTRL_TX_VIDEO_AVMUTE_UPDCTRL_CSCUPD_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_video_black(system_mute *p_mute)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_TX_VIDEO_BLACK_NUM];
	uint8_t outzervrst, outzer;
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_mute->mute == SYSTEM_TX_MUTE_ON_BLACK) {
		outzervrst = SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZERVRST_NORMAL;
		outzer     = SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZER_BLACK;
	} else if (p_mute->mute == SYSTEM_TX_MUTE_OFF_BLACK) {
		outzervrst = SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZERVRST_RELEASE;
		outzer     = SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZER_NORMAL;
	} else {
		return ERR_NG;
	}
	SET_ACCESS(access, &p_mute->port, cmd, SYSTEM_CTRL_TX_VIDEO_BLACK_NUM);
	// page: 3
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSLINKPAGESEL_ADDR,
						SYSTEM_CTRL_TX_VIDEO_BLACK_LVDSLINKPAGESEL_GLUE,
						SYSTEM_CTRL_TX_VIDEO_BLACK_LVDSLINKPAGESEL_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_LVDVB_GLUE_GLUECTL28_ADDR,
						outzervrst                                 |
							outzer,
						SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZERVRST_MASK |
							SYSTEM_CTRL_TX_VIDEO_BLACK_GLUECTL28_OUTZER_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_update_reg(system_port port, uint8_t reg)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_TX_UPDATE_REG_NUM];
	uint32_t i;
	uint32_t ret;

	// 0x3011: perform updates
	SET_ACCESS(access, &port, cmd, SYSTEM_CTRL_TX_UPDATE_REG_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_UPDCTRL_ADDR,
						reg,
						SYSTEM_CTRL_TX_UPDATE_REG_UPDCTRL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	// 0x3011: update wait
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_UPDCTRL_ADDR,
						0,
						SYSTEM_CTRL_TX_UPDATE_REG_UPDCTRL_MASK);
	for (i=0; i<SYSTEM_CTRL_TX_UPDATE_REG_RETRY_MAX; i++) {
		ret = lf9129_device_read(&access);
		CHECK_ERROR(ret);
		if (cmd[0].data == SYSTEM_CTRL_TX_UPDATE_REG_UPDCTRL_CLEARED) {
			break;
		}
		SLEEP(SYSTEM_CTRL_TX_UPDATE_REG_RETRY_INTERVAL);
	}
	if (i == SYSTEM_CTRL_TX_UPDATE_REG_RETRY_MAX) {
		return ERR_TX_UPDATE_CONTROL_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_soft_reset(system_port port, uint8_t rst)
{
	system_reg_access access;
	system_command cmd[SYSTEM_CTRL_TX_SOFT_RESET_NUM];
	uint32_t ret;

	// perform soft reset
	SET_ACCESS(access, &port, cmd, SYSTEM_CTRL_TX_SOFT_RESET_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TSRST_ADDR,
						rst,
						SYSTEM_CTRL_TX_SOFT_RESET_TSRST_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	// reset wait
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TSRST_ADDR,
						SYSTEM_CTRL_TX_SOFT_RESET_TSRST_CLEARED,
						SYSTEM_CTRL_TX_SOFT_RESET_TSRST_MASK);

	ret = device_read_wait(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_vesa_dmt_id(system_video *p_video, sysctrl_vform rx_vf,
										sysctrl_vform tx_vf, uint8_t *p_id)
{
	system_video_format vfmt;
	uint32_t freq;

	if (p_video == NULL || p_id == NULL) {
		return ERR_NULL_POINTER;
	}

	if (rx_vf == tx_vf) {
		vfmt = p_video->tx_video_format;
	} else if ((rx_vf != tx_vf) && tx_vf == SYSCTRL_VFORM_PC) {
		vfmt = p_video->tx_video_format;
	} else if ((rx_vf != tx_vf) && rx_vf == SYSCTRL_VFORM_PC) {
		vfmt = p_video->rx_video_format;
	} else {
		vfmt = SYSTEM_VFORM_UNUSED;
	}

	if ((vfmt > SYSTEM_VFORM_PC_VESA_DMT_ID_UNUSED)
			&& (vfmt < SYSTEM_VFORM_PC_VESA_DMT_ID_MAX)) {
		*p_id = vfmt - SYSTEM_VFORM_PC_VESA_DMT_ID_UNUSED;
	}else if ((vfmt > SYSTEM_VFORM_PC_VARIANT_UNUSED)
				&& (vfmt < SYSTEM_VFORM_PC_VARIANT_MAX)) {
		freq = timing_infos[vfmt].pixel_freq;
		if (freq >= SYSCTRL_PF_25000KHZ && freq < SYSCTRL_PF_32000KHZ) {
			*p_id = SYSCTRL_PCFMT_25_32MHZ;
		} else if (freq >= SYSCTRL_PF_32000KHZ && freq < SYSCTRL_PF_33000KHZ) {
			*p_id = SYSCTRL_PCFMT_32_33MHZ;
		} else if (freq >= SYSCTRL_PF_33000KHZ && freq < SYSCTRL_PF_39000KHZ) {
			*p_id = SYSCTRL_PCFMT_33_39MHZ;
		} else if (freq >= SYSCTRL_PF_39000KHZ && freq < SYSCTRL_PF_48000KHZ) {
			*p_id = SYSCTRL_PCFMT_39_48MHZ;
		} else if (freq >= SYSCTRL_PF_48000KHZ && freq < SYSCTRL_PF_64000KHZ) {
			*p_id = SYSCTRL_PCFMT_48_64MHZ;
		} else if (freq >= SYSCTRL_PF_64000KHZ && freq < SYSCTRL_PF_78000KHZ) {
			*p_id = SYSCTRL_PCFMT_64_78MHZ;
		} else if (freq >= SYSCTRL_PF_78000KHZ && freq < SYSCTRL_PF_98000KHZ) {
			*p_id = SYSCTRL_PCFMT_78_98MHZ;
		} else if (freq >= SYSCTRL_PF_98000KHZ && freq < SYSCTRL_PF_128000KHZ) {
			*p_id = SYSCTRL_PCFMT_98_128MHZ;
		} else if (freq >= SYSCTRL_PF_128000KHZ && freq < SYSCTRL_PF_156000KHZ) {
			*p_id = SYSCTRL_PCFMT_128_156MHZ;
		} else if (freq >= SYSCTRL_PF_156000KHZ && freq < SYSCTRL_PF_192000KHZ) {
			*p_id = SYSCTRL_PCFMT_156_192MHZ;
		} else if (freq >= SYSCTRL_PF_192000KHZ && freq < SYSCTRL_PF_256000KHZ) {
			*p_id = SYSCTRL_PCFMT_192_256MHZ;
		} else if (freq >= SYSCTRL_PF_256000KHZ && freq < SYSCTRL_PF_312000KHZ) {
			*p_id = SYSCTRL_PCFMT_256_312MHZ;
		} else if (freq >= SYSCTRL_PF_312000KHZ && freq < SYSCTRL_PF_384000KHZ) {
			*p_id = SYSCTRL_PCFMT_312_384MHZ;
		} else if (freq >= SYSCTRL_PF_384000KHZ && freq < SYSCTRL_PF_512000KHZ) {
			*p_id = SYSCTRL_PCFMT_384_512MHZ;
		} else if (freq >= SYSCTRL_PF_512000KHZ && freq < SYSCTRL_PF_600000KHZ) {
			*p_id = SYSCTRL_PCFMT_512_600MHZ;
		} else {
			return ERR_NG;
		}
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_vformat(system_video *p_video, sysctrl_vform rx_vf, sysctrl_vform tx_vf)
{
	system_block_access baccess;
	uint8_t data[SYSCTRL_SET_VFORMAT_LEN];
	uint8_t vifmt, vofmt, hdmifmt, hdmi_vic, upcbps, vcibps, pcfmt;
	system_video_format vform4k2k;
	uint32_t vform4k2k_err;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// Video Input Format: when 4K or PC format, set 0
	vifmt = (rx_vf == SYSCTRL_VFORM_4K2K) ? SYSTEM_VFORM_UNUSED
			: (rx_vf == SYSCTRL_VFORM_PC) ? SYSTEM_VFORM_UNUSED
			: p_video->rx_video_format;
	// Video Output Format: when 4K or PC format, set 0
	vofmt = (tx_vf == SYSCTRL_VFORM_4K2K) ? SYSTEM_VFORM_UNUSED
			: (tx_vf == SYSCTRL_VFORM_PC) ? SYSTEM_VFORM_UNUSED
			: p_video->tx_video_format;
	// HDMI Format Mode Select
	hdmifmt = (rx_vf == SYSCTRL_VFORM_4K2K || tx_vf == SYSCTRL_VFORM_4K2K)
				? SYSCTRL_HDMIFMT_HDMIFMT_4K2K
				: (rx_vf == SYSCTRL_VFORM_PC || tx_vf == SYSCTRL_VFORM_PC)
				? SYSCTRL_HDMIFMT_HDMIFMT_2D
				: SYSCTRL_HDMIFMT_HDMIFMT_2D;
	// check 4K format and whether to use up-scaler
	if (rx_vf == SYSCTRL_VFORM_4K2K && tx_vf == SYSCTRL_VFORM_4K2K) {
		if (p_video->rx_video_format != p_video->tx_video_format) {
			return ERR_HDMITX_VIDEO_FORMAT;
		}
		vform4k2k = p_video->rx_video_format;
		vform4k2k_err = ERR_HDMIRX_VIDEO_FORMAT;
		// UPC Bypass & VCI Bypass not used (not use 4K2K upscaler)
		upcbps = SYSCTRL_VSELECT_00_UPCBPS_NOT_USE_UPC;
		vcibps = SYSCTRL_VSELECT_00_VCIBPS_NOT_USE_VCI;
	} else if (rx_vf == SYSCTRL_VFORM_4K2K && tx_vf != SYSCTRL_VFORM_4K2K) {
		vform4k2k = p_video->rx_video_format;
		vform4k2k_err = ERR_HDMIRX_VIDEO_FORMAT;
		// UPC Bypass & VCI Bypass not used (not use 4K2K upscaler)
		upcbps = SYSCTRL_VSELECT_00_UPCBPS_NOT_USE_UPC;
		vcibps = SYSCTRL_VSELECT_00_VCIBPS_NOT_USE_VCI;
	} else if (rx_vf != SYSCTRL_VFORM_4K2K && tx_vf == SYSCTRL_VFORM_4K2K) {
		vform4k2k = p_video->tx_video_format;
		vform4k2k_err = ERR_HDMITX_VIDEO_FORMAT;
		// UPC Bypass & VCI Bypass used (use 4K2K upscaler)
		upcbps = SYSCTRL_VSELECT_00_UPCBPS_USE_UPC;
		vcibps = SYSCTRL_VSELECT_00_VCIBPS_USE_VCI;
	} else {
		vform4k2k = SYSTEM_VFORM_UNUSED;
		vform4k2k_err = ERR_HDMITX_VIDEO_FORMAT;
		// UPC Bypass & VCI Bypass not used (not use 4K2K upscaler)
		upcbps = SYSCTRL_VSELECT_00_UPCBPS_NOT_USE_UPC;
		vcibps = SYSCTRL_VSELECT_00_VCIBPS_NOT_USE_VCI;
	}
	// HDMI Video Format (HDMI_VIC)
	if (vform4k2k == SYSTEM_VFORM_UNUSED) {
		hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_NOR;
	} else {
		switch (vform4k2k) {
			case SYSTEM_VFORM_3840x2160P_2398or24_16v9:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_24HZ;
				break;
			case SYSTEM_VFORM_3840x2160P_25_16v9:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_25HZ;
				break;
			case SYSTEM_VFORM_3840x2160P_2997or30_16v9:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_30HZ;
				break;
			case SYSTEM_VFORM_3840x2160P_50_16v9:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_50HZ;
				break;
			case SYSTEM_VFORM_3840x2160P_5994or60_16v9:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_60HZ;
				break;
			case SYSTEM_VFORM_4096x2160P_2398or24_256v135:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_24HZ;
				break;
			case SYSTEM_VFORM_4096x2160P_25_256v135:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_25HZ;
				break;
			case SYSTEM_VFORM_4096x2160P_2997or30_256v135:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_30HZ;
				break;
			case SYSTEM_VFORM_4096x2160P_50_256v135:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_50HZ;
				break;
			case SYSTEM_VFORM_4096x2160P_5994or60_256v135:
				hdmi_vic = SYSCTRL_HDMIVIC_HDMI_VIC_4K_SMPTE_60HZ;
				break;
			default:
				return vform4k2k_err;
		}
	}
	// check PC format
	if (rx_vf == SYSCTRL_VFORM_PC || tx_vf == SYSCTRL_VFORM_PC) {
		ret = sysctrl_get_vesa_dmt_id(p_video, rx_vf, tx_vf, &pcfmt);
		CHECK_ERROR(ret);
	} else {
		pcfmt = SYSCTRL_PCFMT_PCFMT_DEF;
	}

	SET_BLOCK_ACCESS(baccess, &(p_video->tx_port), VIDEOIF_VIDEO_VIDEOFMT_00_ADDR,
						SYSCTRL_SET_VFORMAT_LEN, data);

	SET_BLOCK_DATA(data, 0, vifmt);													// 0x3070
	SET_BLOCK_DATA(data, 1, vofmt
							| SYSCTRL_VIDEOFMT_01_VOFMTSEL_USE_VOFMT);				// 0x3071
	SET_BLOCK_DATA(data, 2, hdmifmt
							| SYSCTRL_HDMIFMT_VF_MODE_MODE1);						// 0x3072
	SET_BLOCK_DATA(data, 3, SYSCTRL_3DFMT_V3D_EXT_0 | SYSCTRL_3DFMT_VSTC_3D_0);		// 0x3073
	SET_BLOCK_DATA(data, 4, hdmi_vic);												// 0x3074
	SET_BLOCK_DATA(data, 5, pcfmt);													// 0x3075
	SET_BLOCK_DATA(data, 6, upcbps
							| vcibps
							| SYSCTRL_VSELECT_00_SCLKSEL_NOR);						// 0x3076

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_check_vform(system_video_format vf, uint8_t rxtx)
{
	if ((vf <= SYSTEM_VFORM_UNUSED)
		|| ((vf > SYSTEM_VFORM_1920x1080P_100_16v9)
			&& (vf < SYSTEM_VFORM_3840x2160P_2398or24_16v9))
		|| ((vf > SYSTEM_VFORM_4096x2160P_5994or60_256v135)
			&& (vf < SYSTEM_VFORM_1366x768P_5979or60_16v9))
		|| (vf >= SYSTEM_VFORM_MAX)) {
		return (rxtx == SYSTEM_RXTX_TYPE_RX) ? ERR_HDMIRX_VIDEO_FORMAT : ERR_HDMITX_VIDEO_FORMAT;
	}
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_de_mode(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_DE_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_DE_MODE_NUM);
	// DE Select: internally generated DE
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_SYNCCNT_ADDR,
						SYSCTRL_SYNCCNT_DEF,
						SYSCTRL_SYNCCNT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_super_resolution(system_port port, sysctrl_vform rx_vf, sysctrl_vform tx_vf)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_SUPER_RESOLUTION_NUM];
	uint8_t srnon;
	uint32_t ret;

	srnon = (rx_vf != SYSCTRL_VFORM_4K2K && tx_vf == SYSCTRL_VFORM_4K2K)
			? SYSCTRL_SRNON_SRNON_ENA
			: SYSCTRL_SRNON_SRNON_DIS;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_SUPER_RESOLUTION_NUM);
	// Super Resolution Function Enable
	SET_COMMAND(cmd, 0, VIDEOIF_UPC_SRNON_ADDR,
						srnon,
						SYSCTRL_SRNON_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_downscaler(system_port port, sysctrl_vform rx_vf, sysctrl_vform tx_vf)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_DOWNSCALER_NUM];
	uint8_t dncon;
	uint32_t ret;

	dncon = (rx_vf == SYSCTRL_VFORM_4K2K && tx_vf != SYSCTRL_VFORM_4K2K)
			? SYSCTRL_SWMODE4_00_DNCON_ON
			: SYSCTRL_SWMODE4_00_DNCON_OFF;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_DOWNSCALER_NUM);
	// DownScaler On
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE4_00_ADDR,
						dncon,
						SYSCTRL_SWMODE4_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_480p_downscaler(system_video *p_video)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_480P_DOWNSCALER_NUM];
	uint8_t sddncf, sddnce;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_video->tx_video_format == SYSTEM_VFORM_720x480P_5994or60_4v3) {
		if (p_video->rx_video_format == SYSTEM_VFORM_3840x2160P_5994or60_16v9) {
			// 4K to 480p 60Hz
			sddncf = SYSCTRL_DNCCTL2_SDDNCF_FROM_4K;
			sddnce = SYSCTRL_DNCCTL2_SDDNCE_ON;
		} else if (p_video->rx_video_format == SYSTEM_VFORM_1920x1080P_5994or60_16v9) {
			// 1080p to 480p 60Hz
			sddncf = SYSCTRL_DNCCTL2_SDDNCF_FROM_1080P;
			sddnce = SYSCTRL_DNCCTL2_SDDNCE_ON;
		} else {
			sddncf = SYSCTRL_DNCCTL2_SDDNCF_FROM_1080P;
			sddnce = SYSCTRL_DNCCTL2_SDDNCE_OFF;
		}
	} else {
		// 480p down conversion OFF
		sddncf = SYSCTRL_DNCCTL2_SDDNCF_FROM_1080P;
		sddnce = SYSCTRL_DNCCTL2_SDDNCE_OFF;
	}

	SET_ACCESS(access, &(p_video->tx_port), cmd, SYSCTRL_SET_480P_DOWNSCALER_NUM);
	// SDDNCF: 4K/1080p to 480p
	// SDDNCE: 480p down conversion ON/OFF
	SET_COMMAND(cmd, 0, VIDEOIF_DNC_DNCCTL2_ADDR,
						sddncf | sddnce,
						SYSCTRL_DNCCTL2_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_get_deep_color_mode(system_video *p_video, uint8_t *p_deepc)
{
	if (p_video == NULL || p_deepc == NULL) {
		return ERR_NULL_POINTER;
	}

	// when 422, DEEPC must be set by 8bit even if 10/12 bit width are specified
	switch (p_video->tx_color_format) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:
			*p_deepc = SYSTEM_DEEP_COLOR_8BIT;
			break;
		default:
			*p_deepc = p_video->tx_deep_color;
			break;
	}
	// when 4K60p/4K50p rgb/444 10bit is specified for VBO Tx,
	// it is over the video bandwidth for HDMI setting.
	// therefore 10bit is changed to 8bit.
	// NOTE: 0x70:0x09 DEEPCON, DEEPC are the settings for HDMI
	if (p_video->tx_port.port_type == SYSTEM_PORT_TYPE_VBO) {				// if VBO-Tx
		if ((is_video_format_4k2k_60hz(p_video->tx_video_format))			// if 4K60p or
			|| (is_video_format_4k2k_50hz(p_video->tx_video_format))) {		// if 4K50p
			if ((is_color_format_yuv444(p_video->tx_color_format))			// if 444 or
				|| (is_color_format_rgb(p_video->tx_color_format))) {		// if RGB
				*p_deepc = SYSTEM_DEEP_COLOR_8BIT;
			}
		}
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_deep_color(system_video *p_video)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_DEEP_COLOR_NUM];
	uint8_t deepc;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// for DEEPCON and DEEPC of 0x3009
	ret = sysctrl_get_deep_color_mode(p_video, &deepc);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &(p_video->tx_port), cmd, SYSCTRL_SET_DEEP_COLOR_NUM);
	// Bit Width Select (input)
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_VINCNT_ADDR,
						p_video->rx_deep_color,
						SYSCTRL_VINCNT_BITWIDTH_MASK);
	// Output Bit Width
	SET_COMMAND(cmd, 1, VIDEOIF_CSC_OUTWSET_ADDR,
						p_video->tx_deep_color,
						SYSCTRL_OUTWSET_OUTBW_MASK);
	// Deep Color Mode
	SET_COMMAND(cmd, 2, VIDEOIF_SYSTEM_TDPCMODE_00_ADDR,
						((deepc == SYSTEM_DEEP_COLOR_8BIT)
							? SYSCTRL_TDPCMODE_00_DEEPCON_NOR
							: SYSCTRL_TDPCMODE_00_DEEPCON_DEEPCOLOR)
						| SYSCTRL_TDPCMODE_00_DEFPHASE_PHASE0
						| SYSCTRL_TDPCMODE_00_DEEPCSRST_NOR
						| deepc,
						SYSCTRL_TDPCMODE_00_MASK);

	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_srst_deep_color_ctrl(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SRST_DEEP_COLOR_CTRL_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SRST_DEEP_COLOR_CTRL_NUM);
	// 0x3009: DEEPCSRST: reset
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TDPCMODE_00_ADDR,
						SYSCTRL_TDPCMODE_00_DEEPCSRST_RST,
						SYSCTRL_TDPCMODE_00_DEEPCSRST_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_tx_set_video_format(system_video *p_video)
{
	sysctrl_vform rx_vf, tx_vf;
	uint32_t ret;

	// check rx/tx video format
	ret = sysctrl_check_vform(p_video->rx_video_format, SYSTEM_RXTX_TYPE_RX);
	CHECK_ERROR(ret);
	ret = sysctrl_check_vform(p_video->tx_video_format, SYSTEM_RXTX_TYPE_TX);
	CHECK_ERROR(ret);
	// 4K or PC format ?
	rx_vf = is_video_format_4k2k(p_video->rx_video_format)
			? SYSCTRL_VFORM_4K2K
			: is_video_format_pc(p_video->rx_video_format)
			? SYSCTRL_VFORM_PC
			: SYSCTRL_VFORM_NOT_4K2K;
	tx_vf = is_video_format_4k2k(p_video->tx_video_format)
			? SYSCTRL_VFORM_4K2K
			: is_video_format_pc(p_video->tx_video_format)
			? SYSCTRL_VFORM_PC
			: SYSCTRL_VFORM_NOT_4K2K;
	// Set Video Format (4.1.4 3)
	// Set Upscaler Enable (4.1.4 5)
	ret = sysctrl_set_vformat(p_video, rx_vf, tx_vf);
	CHECK_ERROR(ret);
	// Set DE Mode (4.1.4 4)
	ret = sysctrl_set_de_mode(p_video->tx_port);
	CHECK_ERROR(ret);
	// Set Super Resolution Function Enable (4.1.4 6)
	ret = sysctrl_set_super_resolution(p_video->tx_port, rx_vf, tx_vf);
	CHECK_ERROR(ret);
	// set downscaler
	ret = sysctrl_set_downscaler(p_video->tx_port, rx_vf, tx_vf);
	CHECK_ERROR(ret);
	// set 480p downscaler
	ret = sysctrl_set_480p_downscaler(p_video);
	CHECK_ERROR(ret);
	// Set Input Video Bit Width (4.1.4 2)
	// Set Output Bit Width (4.1.5 2)
	// Set DeepColor Mode (4.1.5 3)
	ret = sysctrl_set_deep_color(p_video);
	CHECK_ERROR(ret);
	// soft reset of deep color controller
	ret = sysctrl_srst_deep_color_ctrl(p_video->tx_port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_vbo_lane_count(system_video_format vfmt, system_ctrl_vbo_lane *p_cnt)
{
// NOTE: the following condition is only 297MHz.
//       the condition is different in other cases.
	uint32_t freq;

	if (p_cnt == NULL) {
		return ERR_NULL_POINTER;
	}
	if (vfmt <= SYSTEM_VFORM_UNUSED || vfmt >= SYSTEM_VFORM_MAX) {
		return ERR_INVALID_INDEX;
	}
	freq = timing_infos[vfmt].pixel_freq;
	if (freq == 0) {
		return ERR_INVALID_DATA;
	}
	if (freq <= SYSTEM_CTRL_PIXEL_FREQ_75000KHZ) {
		*p_cnt = SYSTEM_CTRL_VBO_LANE_1;
	} else if ((freq > SYSTEM_CTRL_PIXEL_FREQ_75000KHZ) && (freq <= SYSTEM_CTRL_PIXEL_FREQ_150000KHZ)) {
		*p_cnt = SYSTEM_CTRL_VBO_LANE_2;
	} else if ((freq > SYSTEM_CTRL_PIXEL_FREQ_150000KHZ) && (freq <= SYSTEM_CTRL_PIXEL_FREQ_300000KHZ)) {
		*p_cnt = SYSTEM_CTRL_VBO_LANE_4;
	} else if (freq > SYSTEM_CTRL_PIXEL_FREQ_300000KHZ) {
		*p_cnt = SYSTEM_CTRL_VBO_LANE_8;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_lvds_link_count(system_video_format vfmt, system_ctrl_lvds_link *p_cnt)
{
// NOTE: the following condition is only 148.5MHz.
//       the condition is different in 135MHz and other cases.
	uint32_t freq;

	if (p_cnt == NULL) {
		return ERR_NULL_POINTER;
	}
	if (vfmt <= SYSTEM_VFORM_UNUSED || vfmt >= SYSTEM_VFORM_MAX) {
		return ERR_INVALID_INDEX;
	}
	freq = timing_infos[vfmt].pixel_freq;
	if (freq == 0) {
		return ERR_INVALID_DATA;
	}
	if (freq <= SYSTEM_CTRL_PIXEL_FREQ_150000KHZ) {
		*p_cnt = SYSTEM_CTRL_LVDS_LINK_1;
	} else if ((freq > SYSTEM_CTRL_PIXEL_FREQ_150000KHZ) && (freq <= SYSTEM_CTRL_PIXEL_FREQ_300000KHZ)) {
		*p_cnt = SYSTEM_CTRL_LVDS_LINK_2;
	} else if (freq > SYSTEM_CTRL_PIXEL_FREQ_300000KHZ) {
		*p_cnt = SYSTEM_CTRL_LVDS_LINK_4;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_tx_glue_hstart(system_port port, system_video_format vfmt, uint32_t freq,
										uint8_t *p_hstart70, uint8_t *p_hstart98)
{
	system_ctrl_vbo_lane vcnt;
	system_ctrl_lvds_link lcnt;
	uint32_t cnt;
	uint32_t hstart, hs_a, hs_b;
	uint32_t ret;

	if (p_hstart70 == NULL || p_hstart98 == NULL) {
		return ERR_NULL_POINTER;
	}
	if (vfmt <= SYSTEM_VFORM_UNUSED || vfmt >= SYSTEM_VFORM_MAX) {
		return ERR_INVALID_INDEX;
	}
	if (freq <= 0) {
		return ERR_INVALID_DATA;
	}

	if (port.port_type == SYSTEM_PORT_TYPE_VBO) {
		ret = system_ctrl_get_vbo_lane_count(vfmt, &vcnt);
		CHECK_ERROR(ret);
		switch (vcnt) {
			// NOTE: VBO-Tx cannot transmit by using 1 lane
			case SYSTEM_CTRL_VBO_LANE_1:	cnt=2;	break;
			case SYSTEM_CTRL_VBO_LANE_2:	cnt=2;	break;
			case SYSTEM_CTRL_VBO_LANE_4:	cnt=4;	break;
			case SYSTEM_CTRL_VBO_LANE_8:	cnt=8;	break;
			default:								return ERR_NG;
		}
		if (vcnt == SYSTEM_CTRL_VBO_LANE_4 || vcnt == SYSTEM_CTRL_VBO_LANE_8) {
			hstart = (timing_infos[vfmt].hsync + timing_infos[vfmt].hback) / (cnt / 2);
		} else if (vcnt == SYSTEM_CTRL_VBO_LANE_1 || vcnt == SYSTEM_CTRL_VBO_LANE_2) {
			if (timing_infos[vfmt].pixel_freq == 0) {
				return ERR_INVALID_DATA;
			}
			hs_a = (timing_infos[vfmt].hsync + timing_infos[vfmt].hback) / (cnt / 2);
			hs_b = (timing_infos[vfmt].hsync / (cnt / 2))
					* ((float)freq / (timing_infos[vfmt].pixel_freq / (cnt / 2)));
			if (hs_a > hs_b) {
				hstart = hs_a;
			} else {
				hstart = hs_b * 12 / 10;
			}
		} else {
			return ERR_NG;
		}
	} else if (port.port_type == SYSTEM_PORT_TYPE_LVDS) {
		ret = system_ctrl_get_lvds_link_count(vfmt, &lcnt);
		CHECK_ERROR(ret);
		switch (lcnt) {
			case SYSTEM_CTRL_LVDS_LINK_1:	cnt=1;	break;
			case SYSTEM_CTRL_LVDS_LINK_2:	cnt=2;	break;
			case SYSTEM_CTRL_LVDS_LINK_4:	cnt=4;	break;
			default:								return ERR_NG;
		}
		if (lcnt == SYSTEM_CTRL_LVDS_LINK_2 || lcnt == SYSTEM_CTRL_LVDS_LINK_4) {
			hstart = (timing_infos[vfmt].hsync + timing_infos[vfmt].hback) / cnt;
		} else if (lcnt == SYSTEM_CTRL_LVDS_LINK_1) {
			if (timing_infos[vfmt].pixel_freq == 0) {
				return ERR_INVALID_DATA;
			}
			hs_a = (timing_infos[vfmt].hsync + timing_infos[vfmt].hback) / cnt;
			hs_b = (timing_infos[vfmt].hsync / cnt)
					* ((float)freq / (timing_infos[vfmt].pixel_freq / cnt));
			if (hs_a > hs_b) {
				hstart = hs_a;
			} else {
				hstart = hs_b * 12 / 10;
			}
		} else {
			return ERR_NG;
		}
	} else {
		return ERR_INVALID_INDEX;
	}

	*p_hstart70 = (uint8_t)(hstart & SYSTEM_CTRL_GLUECTL2_HSTART70_MASK);
	*p_hstart98 = (uint8_t)((hstart & SYSTEM_CTRL_GLUECTL3_HSTART98_MASK) >> SYSTEM_CTRL_GLUECTL3_HSTART98_BIT);

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_rx_glue_hstart(system_video_format vfmt, uint8_t *p_hstart70, uint8_t *p_hstart98)
{
	system_ctrl_vbo_lane vcnt;
	uint32_t hstart;
	uint32_t ret;

	if (p_hstart70 == NULL || p_hstart98 == NULL) {
		return ERR_NULL_POINTER;
	}
	if (vfmt <= SYSTEM_VFORM_UNUSED || vfmt >= SYSTEM_VFORM_MAX) {
		return ERR_INVALID_INDEX;
	}

	ret = system_ctrl_get_vbo_lane_count(vfmt, &vcnt);
	CHECK_ERROR(ret);
	switch (vcnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
			hstart = (timing_infos[vfmt].hsync + timing_infos[vfmt].hback) * 2;
			break;
		case SYSTEM_CTRL_VBO_LANE_2:
		case SYSTEM_CTRL_VBO_LANE_4:
		case SYSTEM_CTRL_VBO_LANE_8:
			hstart = timing_infos[vfmt].hsync + timing_infos[vfmt].hback;
			break;
		default:
			return ERR_NG;
	}

	*p_hstart70 = (uint8_t)(hstart & SYSTEM_CTRL_GLUECTL2_HSTART70_MASK);
	*p_hstart98 = (uint8_t)((hstart & SYSTEM_CTRL_GLUECTL3_HSTART98_MASK) >> SYSTEM_CTRL_GLUECTL3_HSTART98_BIT);

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_pixel_freq(system_video_format vfmt, uint32_t *p_pixel_freq)
{
	if (p_pixel_freq == NULL) {
		return ERR_NULL_POINTER;
	}
	if (vfmt <= SYSTEM_VFORM_UNUSED || vfmt >= SYSTEM_VFORM_MAX) {
		return ERR_INVALID_INDEX;
	}

	*p_pixel_freq = timing_infos[vfmt].pixel_freq;

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_sync_polarity(system_video_format vfmt, system_ctrl_polarity *p_hpol,
										system_ctrl_polarity *p_vpol)
{
	if (p_hpol == NULL || p_vpol == NULL) {
		return ERR_NULL_POINTER;
	}
	if (vfmt <= SYSTEM_VFORM_UNUSED || vfmt >= SYSTEM_VFORM_MAX) {
		return ERR_INVALID_INDEX;
	}

	*p_hpol = timing_infos[vfmt].hpol;
	*p_vpol = timing_infos[vfmt].vpol;

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_get_hdmirx_sync_polarity(system_port port, system_ctrl_polarity *p_hpol,
											system_ctrl_polarity *p_vpol)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_GET_HDMIRX_SYNC_POLARITY_NUM];
	uint32_t ret;

	if (p_hpol == NULL || p_vpol == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, SYSCTRL_GET_HDMIRX_SYNC_POLARITY_NUM);
	// CRRHPL,CRRVPL: 1:Positive, 0:Negative
	// 0x2208
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_CRRSYNC_00_ADDR,
						0,
						SYSCTRL_VIDEO_CRRSYNC_00_CRRHPL_MASK
						| SYSCTRL_VIDEO_CRRSYNC_00_CRRVPL_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	// CRRHPL: 1:Positive, 0:Negative
	*p_hpol = (cmd[0].data & SYSCTRL_VIDEO_CRRSYNC_00_CRRHPL_MASK)
				? SYSTEM_CTRL_POL_P
				: SYSTEM_CTRL_POL_N;
	// CRRVPL: 1:Positive, 0:Negative
	*p_vpol = (cmd[0].data & SYSCTRL_VIDEO_CRRSYNC_00_CRRVPL_MASK)
				? SYSTEM_CTRL_POL_P
				: SYSTEM_CTRL_POL_N;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_pad_mode(system_port port, uint8_t mode)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_PAD_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_PAD_MODE_NUM);
	// external audio pin control (none|output|input)
	// 0x1088
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADMDCTRL_ADDR,
						mode,
						SYSCTRL_SET_PAD_MODE_PADMDCTRL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_pad_input1(system_port port, uint8_t in1, uint8_t mask1)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_PAD_INPUT1_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_PAD_INPUT1_NUM);

	// NOTE: ARXL1SEL and TXAIL0SEL are exclusive relationship.

	// ARXL1SEL: external audio data to be input to Tx link
	// TXAIL0SEL: Rx link audio data to be input to Tx link
	//  0x1098
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADAINCTRL1_ADDR,
						in1,
						mask1);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_pad_input2(system_port port, uint8_t in2)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_PAD_INPUT2_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_PAD_INPUT2_NUM);
	// external audio input select
	// 0x1099
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADAINCTRL2_ADDR,
						in2,
						SYSCTRL_PADAINCTRL2_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_set_pad_output(system_port port, uint8_t out)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SET_PAD_OUTPUT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SET_PAD_OUTPUT_NUM);
	// external audio output select
	// 0x109A
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_PADAOUTCTRL1_ADDR,
						out,
						SYSCTRL_PADAOUTCTRL1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_init_audio_tx_link(system_port port)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_INIT_AUDIO_TX_LINK_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_INIT_AUDIO_TX_LINK_NUM);
	// init audio tx link
	//   ACREN: stop packet
	//   ASPEN: stop packet
	//   TXMUTE: mute
	// 0x30A8
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXACTL1_ADDR,
						SYSCTRL_TXACTL1_DEF,
						SYSCTRL_TXACTL1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t sysctrl_select_input_if(system_port port, uint8_t type)
{
	system_reg_access access;
	system_command cmd[SYSCTRL_SELECT_INPUT_IF_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, SYSCTRL_SELECT_INPUT_IF_NUM);
	// select input audio I/F
	//   RXSEL: IEC60958(SPDIF) or 3-wired serial interface(I2S)
	// 0x30AC
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_TXACTL5_ADDR,
						type,
						SYSCTRL_TXACTL5_RXSEL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_init(system_port port, system_ctrl_audio type)
{
	uint32_t ret;

	switch (type) {
		case SYSTEM_CTRL_AUDIO_HDMI_RX:
			// external audio to Tx link: no external input
			// Rx link audio to Tx link: input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_NO_INPUT
													| SYSCTRL_PADAINCTRL1_TXAIL0SEL_RX_LINK_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK
													| SYSCTRL_PADAINCTRL1_TXAIL0SEL_MASK);
			CHECK_ERROR(ret);
			// external audio input I/F: I2S
			// NOTE: when PCFMT, 0x30AC [0] RXSEL must be set by 1
			ret = sysctrl_select_input_if(port, SYSCTRL_TXACTL5_RXSEL_I2S);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_HDMI_TX:
			// init audio tx link
			ret = sysctrl_init_audio_tx_link(port);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_SPDIF_RX:
			// external audio pin control: input
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_EXTERNAL_AUDIO_INPUT);
			CHECK_ERROR(ret);
			// external audio to Tx link: external input
			// Rx link audio to Tx link: no input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_EXTERNAL_INPUT
													| SYSCTRL_PADAINCTRL1_TXAIL0SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK
													| SYSCTRL_PADAINCTRL1_TXAIL0SEL_MASK);
			CHECK_ERROR(ret);
			// external audio input: ON
			ret = sysctrl_set_pad_input2(port, SYSCTRL_PADAINCTRL2_ACH8INSEL_EXTERNAL_INPUT_ON);
			CHECK_ERROR(ret);
			// external audio output: OFF
			ret = sysctrl_set_pad_output(port, SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF);
			CHECK_ERROR(ret);
			// external audio input I/F: SPDIF
			ret = sysctrl_select_input_if(port, SYSCTRL_TXACTL5_RXSEL_SPDIF);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_SPDIF_TX:
			// external audio pin control: output
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_EXTERNAL_AUDIO_OUTPUT);
			CHECK_ERROR(ret);
			// external audio to Tx link: no external input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK);
			CHECK_ERROR(ret);
			// external audio input: OFF
			ret = sysctrl_set_pad_input2(port, SYSCTRL_PADAINCTRL2_ACH8INSEL_EXTERNAL_INPUT_OFF);
			CHECK_ERROR(ret);
			// external audio output: OFF (initialization)
			ret = sysctrl_set_pad_output(port, SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_I2S_RX:
			// external audio pin control: input
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_EXTERNAL_AUDIO_INPUT);
			CHECK_ERROR(ret);
			// external audio to Tx link: external input
			// Rx link audio to Tx link: no input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_EXTERNAL_INPUT
													| SYSCTRL_PADAINCTRL1_TXAIL0SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK
													| SYSCTRL_PADAINCTRL1_TXAIL0SEL_MASK);
			CHECK_ERROR(ret);
			// external audio input: ON
			ret = sysctrl_set_pad_input2(port, SYSCTRL_PADAINCTRL2_ACH8INSEL_EXTERNAL_INPUT_ON);
			CHECK_ERROR(ret);
			// external audio output: OFF
			ret = sysctrl_set_pad_output(port, SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF);
			CHECK_ERROR(ret);
			// external audio input I/F: I2S
			ret = sysctrl_select_input_if(port, SYSCTRL_TXACTL5_RXSEL_I2S);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_I2S_TX:
			// external audio pin control: output
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_EXTERNAL_AUDIO_OUTPUT);
			CHECK_ERROR(ret);
			// external audio to Tx link: no external input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK);
			CHECK_ERROR(ret);
			// external audio input: OFF
			ret = sysctrl_set_pad_input2(port, SYSCTRL_PADAINCTRL2_ACH8INSEL_EXTERNAL_INPUT_OFF);
			CHECK_ERROR(ret);
			// external audio output: OFF (initialization)
			ret = sysctrl_set_pad_output(port, SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF);
			CHECK_ERROR(ret);
			break;
		default:
			ret = ERR_NG;
			break;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_exit(system_port port, system_ctrl_audio type)
{
	uint32_t ret;

	switch (type) {
		case SYSTEM_CTRL_AUDIO_HDMI_RX:
			// Rx link audio to Tx link: no input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_TXAIL0SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_TXAIL0SEL_MASK);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_HDMI_TX:
			ret = ERR_OK;
			break;
		case SYSTEM_CTRL_AUDIO_SPDIF_RX:
			// external audio to Tx link: no external input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK);
			CHECK_ERROR(ret);
			// external audio pin control: none
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_NO_EXTERNAL_AUDIO);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_SPDIF_TX:
			// external audio pin control: none
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_NO_EXTERNAL_AUDIO);
			CHECK_ERROR(ret);
			// external audio output: OFF
			ret = sysctrl_set_pad_output(port, SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_I2S_RX:
			// external audio to Tx link: no external input
			ret = sysctrl_set_pad_input1(port, SYSCTRL_PADAINCTRL1_ARXL1SEL_NO_INPUT,
												SYSCTRL_PADAINCTRL1_ARXL1SEL_MASK);
			CHECK_ERROR(ret);
			// external audio pin control: none
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_NO_EXTERNAL_AUDIO);
			CHECK_ERROR(ret);
			break;
		case SYSTEM_CTRL_AUDIO_I2S_TX:
			// external audio pin control: none
			ret = sysctrl_set_pad_mode(port, SYSCTRL_SET_PAD_MODE_NO_EXTERNAL_AUDIO);
			CHECK_ERROR(ret);
			// external audio output: OFF
			ret = sysctrl_set_pad_output(port, SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF);
			CHECK_ERROR(ret);
			break;
		default:
			ret = ERR_NG;
			break;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_external_audio_mute(system_mute *p_mute)
{
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	ret = sysctrl_set_pad_output(p_mute->port,
								((p_mute->mute == SYSTEM_TX_MUTE_ON)
									? SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_OFF 
									: SYSCTRL_PADAOUTCTRL1_RXAOP0OFF_EXTERNAL_OUTPUT_ON));
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_check_param(system_audio *p_audio)
{
	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_audio->cb_sampling_freq == SYSTEM_AUDIO_SF_768KHZ) {
		if (p_audio->sampling_freq_ratio == SYSTEM_AUDIO_SF_RATIO_1_1) {
			// Check OK
		} else {
			return ERR_HDMITX_AUDIO_COMBINATION;
		}
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_if2sf(uint8_t aif, system_audio_sf *p_sf)
{
	if (p_sf == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert audio infoframe sampling freq into system_audio_sf
	switch (aif) {
		case SYSTEM_CTRL_AUDIO_IF_SF_RSH:				// Refer t Stream Header
			*p_sf = SYSTEM_AUDIO_SF_RSH;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_32KHZ:				//   32  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_32KHZ;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_44KHZ:				//   44.1kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_44KHZ;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_48KHZ:				//   48  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_48KHZ;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_88KHZ:				//   88.2kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_88KHZ;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_96KHZ:				//   96  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_96KHZ;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_176KHZ:			//  176.4kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_176KHZ;		break;
		case SYSTEM_CTRL_AUDIO_IF_SF_192KHZ:			//  192  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_192KHZ;		break;
		default:
			return ERR_HDMIRX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_sf2if(system_audio_sf sf, uint8_t *p_aif)
{
	if (p_aif == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert system_audio_sf into audio infoframe sampling freq
	switch (sf) {
		case SYSTEM_AUDIO_SF_RSH:						// Refer t Stream Header
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_RSH;		break;
		case SYSTEM_AUDIO_SF_32KHZ:						//   32  kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_32KHZ;		break;
		case SYSTEM_AUDIO_SF_44KHZ:						//   44.1kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_44KHZ;		break;
		case SYSTEM_AUDIO_SF_48KHZ:						//   48  kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_48KHZ;		break;
		case SYSTEM_AUDIO_SF_88KHZ:						//   88.2kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_88KHZ;		break;
		case SYSTEM_AUDIO_SF_96KHZ:						//   96  kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_96KHZ;		break;
		case SYSTEM_AUDIO_SF_176KHZ:					//  176.4kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_176KHZ;	break;
		case SYSTEM_AUDIO_SF_192KHZ:					//  192  kHz (AS, OBA, DST)
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SF_192KHZ;	break;
		default:
			return ERR_HDMIRX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_if2ss(uint8_t aif, system_audio_ss *p_ss)
{
	if (p_ss == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert audio infoframe sample size into system_audio_ss
	switch (aif) {
		case SYSTEM_CTRL_AUDIO_IF_SS_RSH:				// Refer to Stream Header
			*p_ss = SYSTEM_AUDIO_SS_RSH;		break;
		case SYSTEM_CTRL_AUDIO_IF_SS_16BIT:				// 16 bit
			*p_ss = SYSTEM_AUDIO_SS_16BIT;		break;
		case SYSTEM_CTRL_AUDIO_IF_SS_20BIT:				// 20 bit
			*p_ss = SYSTEM_AUDIO_SS_20BIT;		break;
		case SYSTEM_CTRL_AUDIO_IF_SS_24BIT:				// 24 bit
			*p_ss = SYSTEM_AUDIO_SS_24BIT;		break;
		default:
			return ERR_HDMIRX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_ss2if(system_audio_ss ss, uint8_t *p_aif)
{
	if (p_aif == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert system_audio_ss into audio infoframe sampling size
	switch (ss) {
		case SYSTEM_AUDIO_SS_RSH:						// Refer to Stream Header
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SS_RSH;		break;
		case SYSTEM_AUDIO_SS_16BIT:						// 16 bit
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SS_16BIT;		break;
		case SYSTEM_AUDIO_SS_20BIT:						// 20 bit
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SS_20BIT;		break;
		case SYSTEM_AUDIO_SS_24BIT:						// 24 bit
			*p_aif = SYSTEM_CTRL_AUDIO_IF_SS_24BIT;		break;
		default:
			return ERR_HDMIRX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_cbit2sf(uint8_t cbit, system_audio_sf *p_sf)
{
	if (p_sf == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert cbit sampling freq into system_audio_sf
	switch (cbit) {
		case SYSTEM_CTRL_AUDIO_CBIT_SF_32KHZ:			//   32  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_32KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_44KHZ:			//   44.1kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_44KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_48KHZ:			//   48  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_48KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_88KHZ:			//   88.2kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_88KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_96KHZ:			//   96  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_96KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_176KHZ:			//  176.4kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_176KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_192KHZ:			//  192  kHz (AS, OBA, DST)
			*p_sf = SYSTEM_AUDIO_SF_192KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_64KHZ:			//   64  kHz (AS w/ 3D/Multi-Stream)
			*p_sf = SYSTEM_AUDIO_SF_64KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_128KHZ:			//  128  kHz (AS w/ 3D/Multi-Stream)
			*p_sf = SYSTEM_AUDIO_SF_128KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_256KHZ:			//  256  kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_256KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_352KHZ:			//  352.8kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_352KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_384KHZ:			//  384  kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_384KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_512KHZ:			//  512  kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_512KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_705KHZ:			//  705.6kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_705KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_768KHZ:			//  768  kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_768KHZ;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_1024KHZ:			// 1024  kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_1024KHZ;	break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_1411KHZ:			// 1411.2kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_1411KHZ;	break;
		case SYSTEM_CTRL_AUDIO_CBIT_SF_1536KHZ:			// 1536  kHz (HBR)
			*p_sf = SYSTEM_AUDIO_SF_1536KHZ;	break;
		default:
			return ERR_HDMIRX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_sf2cbit(system_audio_sf sf, uint8_t *p_cbit)
{
	if (p_cbit == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert system_audio_sf into sampling freq
	switch (sf) {
		case SYSTEM_AUDIO_SF_32KHZ:						//   32  kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_32KHZ;		break;
		case SYSTEM_AUDIO_SF_44KHZ:						//   44.1kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_44KHZ;		break;
		case SYSTEM_AUDIO_SF_48KHZ:						//   48  kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_48KHZ;		break;
		case SYSTEM_AUDIO_SF_88KHZ:						//   88.2kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_88KHZ;		break;
		case SYSTEM_AUDIO_SF_96KHZ:						//   96  kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_96KHZ;		break;
		case SYSTEM_AUDIO_SF_176KHZ:					//  176.4kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_176KHZ;		break;
		case SYSTEM_AUDIO_SF_192KHZ:					//  192  kHz (AS, OBA, DST)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_192KHZ;		break;
		case SYSTEM_AUDIO_SF_64KHZ:						//   64  kHz (AS w/ 3D/Multi-Stream)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_64KHZ;		break;
		case SYSTEM_AUDIO_SF_128KHZ:					//  128  kHz (AS w/ 3D/Multi-Stream)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_128KHZ;		break;
		case SYSTEM_AUDIO_SF_256KHZ:					//  256  kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_256KHZ;		break;
		case SYSTEM_AUDIO_SF_352KHZ:					//  352.8kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_352KHZ;		break;
		case SYSTEM_AUDIO_SF_384KHZ:					//  384  kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_384KHZ;		break;
		case SYSTEM_AUDIO_SF_512KHZ:					//  512  kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_512KHZ;		break;
		case SYSTEM_AUDIO_SF_705KHZ:					//  705.6kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_705KHZ;		break;
		case SYSTEM_AUDIO_SF_768KHZ:					//  768  kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_768KHZ;		break;
		case SYSTEM_AUDIO_SF_1024KHZ:					// 1024  kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_1024KHZ;	break;
		case SYSTEM_AUDIO_SF_1411KHZ:					// 1411.2kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_1411KHZ;	break;
		case SYSTEM_AUDIO_SF_1536KHZ:					// 1536  kHz (HBR)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SF_1536KHZ;	break;
		default:
			return ERR_HDMITX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_cbit2ss(uint8_t cbit, system_audio_ss *p_ss)
{
	if (p_ss == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert cbit sample size into system_audio_ss
	switch (cbit) {
		case SYSTEM_CTRL_AUDIO_CBIT_SS_16BIT:					// 16 bit
			*p_ss = SYSTEM_AUDIO_SS_16BIT;			break;
		case SYSTEM_CTRL_AUDIO_CBIT_SS_20BIT_0:					// 20 bit (0)
			*p_ss = SYSTEM_AUDIO_SS_20BIT_0;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SS_20BIT_1:					// 20 bit (1)
			*p_ss = SYSTEM_AUDIO_SS_20BIT_1;		break;
		case SYSTEM_CTRL_AUDIO_CBIT_SS_24BIT:					// 24 bit
			*p_ss = SYSTEM_AUDIO_SS_24BIT;			break;
		case SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT:				// max 20 bit
			*p_ss = SYSTEM_AUDIO_MAX_20BIT;			break;
		case SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT:				// max 24 bit
			*p_ss = SYSTEM_AUDIO_MAX_24BIT;			break;
		default:
			return ERR_HDMIRX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_ss2cbit(system_audio_ss ss, uint8_t *p_cbit)
{
	if (p_cbit == NULL) {
		return ERR_NULL_POINTER;
	}
	// convert system_audio_ss into cbit sample size
	switch (ss) {
		case SYSTEM_AUDIO_SS_16BIT:									// 16 bit
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SS_16BIT;		break;
		case SYSTEM_AUDIO_SS_20BIT:									// 20 bit
		case SYSTEM_AUDIO_SS_20BIT_0:								// 20 bit (0)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SS_20BIT_0;	break;
		case SYSTEM_AUDIO_SS_20BIT_1:								// 20 bit (1)
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SS_20BIT_1;	break;
		case SYSTEM_AUDIO_SS_24BIT:									// 24 bit
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SS_24BIT;		break;
		case SYSTEM_AUDIO_MAX_20BIT:								// max 20 bit
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_20BIT;	break;
		case SYSTEM_AUDIO_MAX_24BIT:								// max 24 bit
			*p_cbit = SYSTEM_CTRL_AUDIO_CBIT_SS_MAX_24BIT;	break;
		default:
			return ERR_HDMITX_AUDIO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t system_ctrl_audio_get_output_sf(system_audio_sf sf, system_audio_sf_ratio sf_ratio,
											uint8_t *p_output)
{
	uint8_t data;

	if (p_output == NULL) {
		return ERR_NULL_POINTER;
	}
	*p_output = 0;

	switch (sf) {
	case SYSTEM_AUDIO_SF_32KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_32KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:
			data = SYSTEM_AUDIO_SF_64KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_4_1:
			data = SYSTEM_AUDIO_SF_128KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_1_2, SYSTEM_AUDIO_SF_RATIO_1_4
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_44KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_44KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:
			data = SYSTEM_AUDIO_SF_88KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_4_1:
			data = SYSTEM_AUDIO_SF_176KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_1_2, SYSTEM_AUDIO_SF_RATIO_1_4
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_48KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_48KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:
			data = SYSTEM_AUDIO_SF_96KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_4_1:
			data = SYSTEM_AUDIO_SF_192KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_1_2, SYSTEM_AUDIO_SF_RATIO_1_4
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_88KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_88KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:
			data = SYSTEM_AUDIO_SF_44KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:
			data = SYSTEM_AUDIO_SF_176KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_1_4, SYSTEM_AUDIO_SF_RATIO_4_1
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_96KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_96KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:
			data = SYSTEM_AUDIO_SF_48KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:
			data = SYSTEM_AUDIO_SF_192KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_1_4, SYSTEM_AUDIO_SF_RATIO_4_1
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_176KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_176KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:
			data = SYSTEM_AUDIO_SF_88KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_4:
			data = SYSTEM_AUDIO_SF_44KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_2_1, SYSTEM_AUDIO_SF_RATIO_4_1
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_192KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_192KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:
			data = SYSTEM_AUDIO_SF_96KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_4:
			data = SYSTEM_AUDIO_SF_48KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_2_1, SYSTEM_AUDIO_SF_RATIO_4_1
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_64KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_64KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:
			data = SYSTEM_AUDIO_SF_32KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_2_1:
			data = SYSTEM_AUDIO_SF_128KHZ;	break;
		default: // SYSTEM_AUDIO_SF_RATIO_1_4, SYSTEM_AUDIO_SF_RATIO_4_1
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_128KHZ:
		switch (sf_ratio) {
		case SYSTEM_AUDIO_SF_RATIO_1_1:
			data = SYSTEM_AUDIO_SF_128KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_2:
			data = SYSTEM_AUDIO_SF_64KHZ;	break;
		case SYSTEM_AUDIO_SF_RATIO_1_4:
			data = SYSTEM_AUDIO_SF_32KHZ;	break;
		default:  //SYSTEM_AUDIO_SF_RATIO_2_1, SYSTEM_AUDIO_SF_RATIO_4_1
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	case SYSTEM_AUDIO_SF_768KHZ:
		if (sf_ratio == SYSTEM_AUDIO_SF_RATIO_1_1) {
			data = SYSTEM_AUDIO_SF_768KHZ;
		} else {
			return ERR_HDMIRX_AUDIO_COMBINATION;
		}
		break;
	default:  //SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SF_MAX
		return ERR_HDMIRX_AUDIO_COMBINATION;
	}
	*p_output = data;

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint8_t system_ctrl_is_audio_hbr(system_audio_sf sf)
{
	switch (sf) {
		case SYSTEM_AUDIO_SF_32KHZ:					//   32  kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_44KHZ:					//   44.1kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_48KHZ:					//   48  kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_88KHZ:					//   88.2kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_96KHZ:					//   96  kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_176KHZ:				//  176.4kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_192KHZ:				//  192  kHz (AS, OBA, DST)
		case SYSTEM_AUDIO_SF_64KHZ:					//   64  kHz (AS w/ 3D/Multi-Stream)
		case SYSTEM_AUDIO_SF_128KHZ:				//  128  kHz (AS w/ 3D/Multi-Stream)
			return 0;
		case SYSTEM_AUDIO_SF_256KHZ:				//  256  kHz (HBR)
		case SYSTEM_AUDIO_SF_352KHZ:				//  352.8kHz (HBR)
		case SYSTEM_AUDIO_SF_384KHZ:				//  384  kHz (HBR)
		case SYSTEM_AUDIO_SF_512KHZ:				//  512  kHz (HBR)
		case SYSTEM_AUDIO_SF_705KHZ:				//  705.6kHz (HBR)
		case SYSTEM_AUDIO_SF_768KHZ:				//  768  kHz (HBR)
		case SYSTEM_AUDIO_SF_1024KHZ:				// 1024  kHz (HBR)
		case SYSTEM_AUDIO_SF_1411KHZ:				// 1411.2kHz (HBR)
		case SYSTEM_AUDIO_SF_1536KHZ:				// 1536  kHz (HBR)
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_1080p_60hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_1920x1080P_5994or60_16v9:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_1080p_50hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_1920x1080P_50_16v9:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_1080p_30hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_1920x1080P_2997or30_16v9:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_1080p_25hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_1920x1080P_25_16v9:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_1080p_24hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_1920x1080P_2398or24_16v9:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_1080p(system_video_format vfmt)
{
	if ((is_video_format_1080p_60hz(vfmt))
	 || (is_video_format_1080p_50hz(vfmt))
	 || (is_video_format_1080p_30hz(vfmt))
	 || (is_video_format_1080p_25hz(vfmt))
	 || (is_video_format_1080p_24hz(vfmt))) {
		return 1;
	} else {
		return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_4k2k_60hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_3840x2160P_5994or60_16v9:
		case SYSTEM_VFORM_4096x2160P_5994or60_256v135:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_4k2k_50hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_3840x2160P_50_16v9:
		case SYSTEM_VFORM_4096x2160P_50_256v135:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_4k2k_30hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_3840x2160P_2997or30_16v9:
		case SYSTEM_VFORM_4096x2160P_2997or30_256v135:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_4k2k_25hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_3840x2160P_25_16v9:
		case SYSTEM_VFORM_4096x2160P_25_256v135:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_4k2k_24hz(system_video_format vfmt)
{
	switch (vfmt) {
		case SYSTEM_VFORM_3840x2160P_2398or24_16v9:
		case SYSTEM_VFORM_4096x2160P_2398or24_256v135:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_4k2k(system_video_format vfmt)
{
	if ((is_video_format_4k2k_60hz(vfmt))
	 || (is_video_format_4k2k_50hz(vfmt))
	 || (is_video_format_4k2k_30hz(vfmt))
	 || (is_video_format_4k2k_25hz(vfmt))
	 || (is_video_format_4k2k_24hz(vfmt))) {
		return 1;
	} else {
		return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_video_format_pc(system_video_format vfmt)
{
	if (((vfmt > SYSTEM_VFORM_PC_VESA_DMT_ID_UNUSED) && (vfmt < SYSTEM_VFORM_PC_VESA_DMT_ID_MAX))
		|| ((vfmt > SYSTEM_VFORM_PC_VARIANT_UNUSED) && (vfmt < SYSTEM_VFORM_PC_VARIANT_MAX))) {
		return 1;
	} else {
		return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_color_format_yuv420(system_color_format cfmt)
{
	switch (cfmt) {
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:
		case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_color_format_yuv422(system_color_format cfmt)
{
	switch (cfmt) {
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:
		case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:
		case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:
			return 1;
		default :
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_color_format_yuv444(system_color_format cfmt)
{
	switch (cfmt) {
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:
		case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:
		case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:
			return 1;
		default :
			return 0;
	}
}
//------------------------------------------------------------------------------
uint8_t is_color_format_rgb(system_color_format cfmt)
{
	switch (cfmt) {
		case SYSTEM_COLOR_RGB_FULL_RANGE1:
		case SYSTEM_COLOR_RGB_LIMITED_RANGE:
		case SYSTEM_COLOR_RGB_FULL_RANGE2:
		case SYSTEM_COLOR_RGB_ITU_R_BT2020:
			return 1;
		default :
			return 0;
	}
}
//------------------------------------------------------------------------------
