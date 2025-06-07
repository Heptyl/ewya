/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "vborx.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "hv_version.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// vborx_select_page()
#define VBORX_PAGE_SELECT_LEN								2
#define VBORX_PAGE_SELECT_PAGECTRL_ATONCE_SINGLE			0x00
#define VBORX_PAGE_SELECT_PAGECTRL_ATONCE_BOTH				0x01
#define VBORX_PAGE_SELECT_PAGECTRL_PAGESEL_DEF				0x00
//------------------------------------------------------------------------------
// vborx_cdrlock_hotplug_negate()
#define VBORX_CDRLOCK_HOTPLUG_NEGATE_NUM					1
#define VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_ASSERT			0x00
#define VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_NEGATE			0x01
#define VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_MASK				0x00000001
#define VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_ASSERT			0x00
#define VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_NEGATE			0x02
#define VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_MASK				0x00000002
#define VBORX_LOCK_HTPD_DETPINCTRL_0_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_hvsync_polarity_setting()
#define VBORX_HVSYNC_POLARITY_NUM							2
#define VBORX_HVSYNC_POLARITY_SELCTRL9_0_INVERTING			0x0F
#define VBORX_HVSYNC_POLARITY_SELCTRL9_0_NOT_INVERTING		0x00
#define VBORX_HVSYNC_POLARITY_SELCTRL9_0_MASK				0x000000FF
#define VBORX_HVSYNC_POLARITY_SELCTRLA_0_INVERTING			VBORX_HVSYNC_POLARITY_SELCTRL9_0_INVERTING
#define VBORX_HVSYNC_POLARITY_SELCTRLA_0_NOT_INVERTING		VBORX_HVSYNC_POLARITY_SELCTRL9_0_NOT_INVERTING
#define VBORX_HVSYNC_POLARITY_SELCTRLA_0_MASK				VBORX_HVSYNC_POLARITY_SELCTRL9_0_MASK
//------------------------------------------------------------------------------
// vborx_path_setting()
#define VBORX_PATH_SETTING_NUM								1
#define VBORX_PATH_SETTING_MISC_VBOREN_VBO					0x10
#define VBORX_PATH_SETTING_MISC_MASK						0x000000FF
//------------------------------------------------------------------------------
// vborx_clock_setting_whole()
#define VBORX_CLOCK_SETTING_WHOLE_NUM						1
#define VBORX_CLOCK_SETTING_WHOLE_LEN						4
#define VBORX_CLOCK_SETTING_CKSELEN_RX0_SEN_MANUAL			0x02
#define VBORX_CLOCK_SETTING_CKSELEN_RX0_SEN_MASK			0x00000002
#define VBORX_CLOCK_SETTING_RXCORE0_00_CKSELRX0_07_00		0x00
#define VBORX_CLOCK_SETTING_RXCORE0_01_CKSELRX0_15_08		0x00
#define VBORX_CLOCK_SETTING_RXCORE0_02_CKSELRX0_23_16		0x00
#define VBORX_CLOCK_SETTING_RXCORE0_03_CKSELRX0_32_24		0x20
//------------------------------------------------------------------------------
// vborx_clock_setting_lane()
#define VBORX_CLOCK_SETTING_LANE_LEN						2
#define VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK03_LANE0		0x00
#define VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK47_LANE4		0x00
#define VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK03_345_4BYTE	0x10
#define VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK47_345_4BYTE	0x40
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SEL41_RX			0x00
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_L0_1_2			0x04
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_L0_1_4			0x08
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_L0_1_8			0x0C
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SEL21_R0			0x10
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SEL21_L0			0x00
#define VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_HP				0x00
//------------------------------------------------------------------------------
#define VBORX_PHYX_OFFSET									0x40
//------------------------------------------------------------------------------
// vborx_phyx_param_setting()
#define VBORX_PHYX_PARAM_SETTING_NUM						3
#define VBORX_PHYX_PARAM_SETTING_EQ_CTRL_LEN				2
#define VBORX_PHYX_PARAM_SETTING_CDR_CTRL_LEN				3
#define VBORX_PHYX_PARAM_SETTING_MAX_LEN					VBORX_PHYX_PARAM_SETTING_CDR_CTRL_LEN
#define VBORX_PHYX_PARAM_SETTING_PX_ENTCNT3_0_MODE_01		0x01
#define VBORX_PHYX_PARAM_SETTING_PX_ENTCNT3_0_MASK			0x000000FF
#define VBORX_PHYX_PARAM_SETTING_PX_SPDCNT_0_SPEED_01		0x01
#define VBORX_PHYX_PARAM_SETTING_PX_SPDCNT_0_SPEED_MASK		0x000000FF
#define VBORX_PHYX_PARAM_SETTING_PX_TRMCNT_0_RT_RX_11_0F	0x0F
#define VBORX_PHYX_PARAM_SETTING_PX_TRMCNT_0_RT_RX_11_MASK	0x000000FF
#define VBORX_PHYX_PARAM_SETTING_PX_EQCNT_0_EQ_CTL_11_00	0x00
#define VBORX_PHYX_PARAM_SETTING_PX_EQCNT_1_EQ_CTL_11_03	0x03
#define VBORX_PHYX_PARAM_SETTING_PX_EQCNT_1_DESKEWTRIM_00	0x00
#define VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_0_CDR_CTRL_50	0x50
#define VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_0_CDR_CM_08		0x08
#define VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_0_CDR_CLMP_00	0x00
#define VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_1_CPF_TRIM_A0	0xA0
#define VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_1_CPD_TRIM_03	0x03
#define VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_2_DIRECT_TRIM_03	0x03
//------------------------------------------------------------------------------
// vborx_pixel_pll_setting()
#define VBORX_PIXEL_PLL_SETTING_LEN							3
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_0_DIVN_08			0x08
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_0_DIVN_10			0x10
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FOUTSEL			0x00
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FDIV_30			0x30
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FDIV_10			0x10
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_1_ICPM_08			0x08
#define VBORX_PIXEL_PLL_SETTING_PLLCTL_2_NPOWD_01			0x01
//------------------------------------------------------------------------------
// vborx_get_gluectl0_1()
#define VBORX_GET_GLUECTL0_1_NUM							1
#define VBORX_GLUECTL0_1_MASK								0x000000FF
//------------------------------------------------------------------------------
// vborx_link_setting1()
#define VBORX_LINK_SETTING1_LEN								4
#define VBORX_GLUECTL0_0_CKMODE_2LANE						0x00
#define VBORX_GLUECTL0_0_CKMODE_4LANE						0x01
#define VBORX_GLUECTL0_0_CKMODE_8LANE						0x03
#define VBORX_GLUECTL0_0_LANE_SEL_DEF						0x00
#define VBORX_GLUECTL0_0_SRESET_DEF							0x00
#define VBORX_GLUECTL0_0_FIFO_RST_DEF						0x00
#define VBORX_GLUECTL0_0_IPMODE_RR_GG_BB					0x00
#define VBORX_GLUECTL0_0_IPMODE_RR_GB_BG					0x20
#define VBORX_GLUECTL0_0_IPMODE_RG_GR_BB					0x40
#define VBORX_GLUECTL0_0_IPMODE_RG_GB_BR					0x60
#define VBORX_GLUECTL0_0_IPMODE_RB_GG_BR					0x80
#define VBORX_GLUECTL0_0_IPMODE_RB_GR_BG					0xA0
#define VBORX_GLUECTL0_2_HSTART_720P						0x08
#define VBORX_GLUECTL0_2_HSTART_FWXGA						0xC8
#define VBORX_GLUECTL0_2_HSTART_1080P						0xC0
#define VBORX_GLUECTL0_2_HSTART_UHD_24_25_30P				0x80
#define VBORX_GLUECTL0_2_HSTART_UHD_50_60P					0x80
#define VBORX_GLUECTL0_2_HSTART_DCI4K_24_25_30P				0xD8
#define VBORX_GLUECTL0_2_HSTART_DCI4K_50_60P				0xD8
#define VBORX_GLUECTL0_3_HSTART_720P						0x02
#define VBORX_GLUECTL0_3_HSTART_FWXGA						0x02
#define VBORX_GLUECTL0_3_HSTART_1080P						0x00
#define VBORX_GLUECTL0_3_HSTART_UHD_24_25_30P				0x01
#define VBORX_GLUECTL0_3_HSTART_UHD_50_60P					0x01
#define VBORX_GLUECTL0_3_HSTART_DCI4K_24_25_30P				0x00
#define VBORX_GLUECTL0_3_HSTART_DCI4K_50_60P				0x00
#define VBORX_GLUECTL0_3_TWO_LINE_MODE_2LINE				0x20
#define VBORX_GLUECTL0_3_TWO_LINE_MODE_4LINE				0x00
#define VBORX_GLUECTL0_3_HS_POL_DEF							0x00
#define VBORX_GLUECTL0_3_VS_POL_DEF							0x00
//------------------------------------------------------------------------------
// vborx_phyx_setting1()
#define VBORX_PHYX_SETTING1_NUM								3
#define VBORX_PHYX_SETTING1_PX_SPDCNT_1_LOOP_ACTIVATE_FREQ	0x02
#define VBORX_PHYX_SETTING1_PX_SPDCNT_1_LOOP_ACTIVATE_MASK	0x000000FF
#define VBORX_PHYX_SETTING1_PX_ENTCNT1_1_EN_P_IREF_11_DIS	0x00
#define VBORX_PHYX_SETTING1_PX_ENTCNT1_1_ANA_EN_11_ENA		0x02
#define VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MAIN_EN_11_NORMAL	0x00
#define VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MAIN_EN_11_NARROW	0x04
#define VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MASK				0x000000FF
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_BGAP_V12_EN_11_ENA	0x02
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_START_BIAS_11_ENA	0x04
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_RX_TERM_EN_11_ENA	0x08
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_EB11_EQ_ENA		0x10
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_DESKEWEN_ENA		0x40
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_DESKEWAUTO_DIS_DIS	0x80
#define VBORX_PHYX_SETTING1_PX_ENTCNT3_0_MASK				0x000000FF
//------------------------------------------------------------------------------
// vborx_cdr_reset()
#define VBORX_CDR_RESET_NUM									1
#define VBORX_CDR_RESET_LINKRSTCTRL_3_CD0RSR_BIT			0x00
#define VBORX_CDR_RESET_LINKRSTCTRL_3_CD1RSR_BIT			0x01
#define VBORX_CDR_RESET_LINKRSTCTRL_3_CD2RSR_BIT			0x02
#define VBORX_CDR_RESET_LINKRSTCTRL_3_CD3RSR_BIT			0x03
#define VBORX_CDR_RESET_LINKRSTCTRL_3_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_lane_reset()
#define VBORX_LANE_RESET_NUM								1
#define VBORX_LANE_RESET_LINKRSTCTRL_0_PHDRSR_BIT			0x00
#define VBORX_LANE_RESET_LINKRSTCTRL_0_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_cdr_lane_mode()
#define VBORX_CDR_LANE_MODE_NUM								1
#define VBORX_CDR_LANE_MODE_LEN								2
#define VBORX_CDR_LANE_MODE_MODECTRL_0_CLKLNKSEL_LANE0		0x00
#define VBORX_CDR_LANE_MODE_MODECTRL_1_BYTEMODESEL_4BYTE	0x01
#define VBORX_CDR_LANE_MODE_LANEN_0_LANEEN_4_8LANE			0x0F
#define VBORX_CDR_LANE_MODE_LANEN_0_LANEEN_2LANE			0x03
#define VBORX_CDR_LANE_MODE_LANEN_0_LANEEN_1LANE			0x01
#define VBORX_CDR_LANE_MODE_LANEN_0_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_select_input_lane()
#define VBORX_SELECT_INPUT_LANE_NUM							1
#define VBORX_SELCTRLB_1_LANE0								0x00
#define VBORX_SELCTRLB_1_LANE1								0x01
#define VBORX_SELCTRLB_1_MASK								0x000000FF
//------------------------------------------------------------------------------
// vborx_cdr_lock_setting()
#define VBORX_CDR_LOCK_SETTING_LEN							6
#define VBORX_CDR_LOCK_SETTING_CDRCTR0_0_MAXLSR_03			0x03
#define VBORX_CDR_LOCK_SETTING_CDRCTR0_1_LSLKTR_20			0x20
#define VBORX_CDR_LOCK_SETTING_CDRCTR0_2_LSLLKR_00			0x00
#define VBORX_CDR_LOCK_SETTING_CDRCTR0_2_LSLCKR_00			0x00
#define VBORX_CDR_LOCK_SETTING_CDRCTR0_3_LSTHMR_01			0x01
#define VBORX_CDR_LOCK_SETTING_CDRCTR1_0_LSLTGR_7_0_74		0x74
#define VBORX_CDR_LOCK_SETTING_CDRCTR1_1_LSLTGR_15_8_01		0x01
//------------------------------------------------------------------------------
// vborx_irq_stat_clearing()
#define VBORX_IRQ_STAT_CLEARING_NUM							1
#define VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_CDRLIRQ_CLR		0x01
#define VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_CDRUIRQ_CLR		0x02
#define VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_LINKUIRQ_CLR	0x04
#define VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_LINKDIRQ_CLR	0x08
#define VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_MASK			0x000000FF
//------------------------------------------------------------------------------
// vborx_irq_mask_setting()
#define VBORX_IRQ_MASK_SETTING_NUM							1
#define VBORX_IRQ_MASK_SETTING_IRQMASK_0_CDRLEN_OUT			0x01
#define VBORX_IRQ_MASK_SETTING_IRQMASK_0_CDRUEN_OUT			0x02
#define VBORX_IRQ_MASK_SETTING_IRQMASK_0_LINKUEN_OUT		0x04
#define VBORX_IRQ_MASK_SETTING_IRQMASK_0_LINKDEN_OUT		0x08
#define VBORX_IRQ_MASK_SETTING_IRQMASK_0_MASK				0x000000FF
//------------------------------------------------------------------------------
// vborx_phyx_setting2()
#define VBORX_PHYX_SETTING2_NUM								2
#define VBORX_PHYX_SETTING2_PX_ENTCNT3_0_EN_P_CDR_11_ENA	0x20
#define VBORX_PHYX_SETTING2_PX_ENTCNT3_0_MASK				0x000000FF
#define VBORX_PHYX_SETTING2_PX_ENTCNT_1_1_RSTB_RST			0x08
#define VBORX_PHYX_SETTING2_PX_ENTCNT_1_1_RSTB_MASK			0x00000008
//------------------------------------------------------------------------------
// vborx_hotplug_assert()
#define VBORX_HOTPLUG_ASSERT_NUM							1
#define VBORX_HOTPLUG_ASSERT_DETPINCTRL_0_HTPDN_ASSERT		VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_ASSERT
#define VBORX_HOTPLUG_ASSERT_DETPINCTRL_0_HTPDN_NEGATE		VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_NEGATE
#define VBORX_HOTPLUG_ASSERT_DETPINCTRL_0_HTPDN_MASK		VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_MASK
//------------------------------------------------------------------------------
// vborx_cdrlock_detect()
#define VBORX_CDRLOCK_DETECT_NUM							1
#define VBORX_CDRLOCK_DETECT_IRQSTATUS_0_CDRLIRQ_DETECT		0x01
#define VBORX_CDRLOCK_DETECT_IRQSTATUS_0_CDRLIRQ_MASK		0x00000001
//------------------------------------------------------------------------------
// vborx_cdrlock_clear()
#define VBORX_CDRLOCK_CLEAR_NUM								1
#define VBORX_CDRLOCK_CLEAR_IRQSTATUS_0_CDRLIRQ_CLEAR		VBORX_CDRLOCK_DETECT_IRQSTATUS_0_CDRLIRQ_DETECT
#define VBORX_CDRLOCK_CLEAR_IRQSTATUS_0_CDRLIRQ_MASK		VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_MASK
//------------------------------------------------------------------------------
// vborx_linkup_detect()
#define VBORX_LINKUP_DETECT_NUM								1
#define VBORX_LINKUP_DETECT_IRQSTATUS_0_LINKUIRQ_DETECT		0x04
#define VBORX_LINKUP_DETECT_IRQSTATUS_0_LINKUIRQ_MASK		0x00000004
#define VBORX_LINKUP_DETECT_RETRY_MAX						300
#define VBORX_LINKUP_DETECT_RETRY_INTERVAL					10
//------------------------------------------------------------------------------
// vborx_linkup_clear()
#define VBORX_LINKUP_CLEAR_NUM								1
#define VBORX_LINKUP_CLEAR_IRQSTATUS_0_LINKUIRQ_CLEAR		VBORX_LINKUP_DETECT_IRQSTATUS_0_LINKUIRQ_DETECT
#define VBORX_LINKUP_CLEAR_IRQSTATUS_0_LINKUIRQ_MASK		VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_MASK
//------------------------------------------------------------------------------
// vborx_cdrlock_assert()
#define VBORX_CDRLOCK_ASSERT_NUM							3
#define VBORX_CDRLOCK_ASSERT_PX_SPDCNT_1_F_LOOP_ACT_PHASE	0x00
#define VBORX_CDRLOCK_ASSERT_PX_SPDCNT_1_MASK				0x000000FF
#define VBORX_CDRLOCK_ASSERT_DETPINCTRL_0_LOCKN_ASSERT		VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_ASSERT
#define VBORX_CDRLOCK_ASSERT_DETPINCTRL_0_LOCKN_NEGATE		VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_NEGATE
#define VBORX_CDRLOCK_ASSERT_DETPINCTRL_0_LOCKN_MASK		VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_MASK
//------------------------------------------------------------------------------
// vborx_set_hstart_of_link_setting1()
#define VBORX_SET_HSTART_OF_LINK_SETTING1_LEN				2
//------------------------------------------------------------------------------
// vborx_pg_aspect_ratio_h()
#define VBORX_PG_ASPECT_RATIO_H_LEN							7
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_0_30		0x30
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_1_00		0x00
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_2_26		0x26
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_0_02	0x02
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_1_E0	0xE0
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_2_01	0x01
#define VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_3_0B	0x0B
//------------------------------------------------------------------------------
// vborx_pg_aspect_ratio_v1()
#define VBORX_PG_ASPECT_RATIO_V1_LEN						4
#define VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_0_0A	0x0A
#define VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_1_00	0x00
#define VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_2_28	0x28
#define VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_3_00	0x00
//------------------------------------------------------------------------------
// vborx_pg_aspect_ratio_v2()
#define VBORX_PG_ASPECT_RATIO_V2_LEN						6
#define VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_0_14	0x14
#define VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_1_00	0x00
#define VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_2_02	0x02
#define VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_3_00	0x00
#define VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL2_0_00	0x00
#define VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL2_1_00	0x00
//------------------------------------------------------------------------------
// vborx_pg_ope()
#define VBORX_PG_OPE_NUM									1
#define VBORX_PG_OPE_VBO_PG_LANEXINCSEL_0_4PIXEL			0x03
#define VBORX_PG_OPE_VBO_PG_LANEXINCSEL_0_MASK				0x000000FF
#define VBORX_PG_OPE_LEN									12
#define VBORX_PG_OPE_VBO_PG_R_CTRL_0_10						0x10
#define VBORX_PG_OPE_VBO_PG_R_CTRL_1_00						0x00
#define VBORX_PG_OPE_VBO_PG_R_CTRL_2_00						0x00
#define VBORX_PG_OPE_VBO_PG_R_CTRL_3_00						0x00
#define VBORX_PG_OPE_VBO_PG_G_CTRL_0_08						0x08
#define VBORX_PG_OPE_VBO_PG_G_CTRL_1_00						0x00
#define VBORX_PG_OPE_VBO_PG_G_CTRL_2_00						0x00
#define VBORX_PG_OPE_VBO_PG_G_CTRL_3_00						0x00
#define VBORX_PG_OPE_VBO_PG_B_CTRL_0_04						0x04
#define VBORX_PG_OPE_VBO_PG_B_CTRL_1_00						0x00
#define VBORX_PG_OPE_VBO_PG_B_CTRL_2_00						0x00
#define VBORX_PG_OPE_VBO_PG_B_CTRL_3_00						0x00
//------------------------------------------------------------------------------
// vborx_pg_use()
#define VBORX_PG_USE_NUM									1
#define VBORX_PG_USE_PG_SET_0_VBOPGEN_BIT					0x00
#define VBORX_PG_USE_MASK									0x000000FF
//------------------------------------------------------------------------------
// vborx_set_txpll00en()
#define VBORX_SET_TXPLL00EN_NUM								1
#define VBORX_TXPLL00_01_TXPLL00EN_AUTO						0x00
#define VBORX_TXPLL00_01_TXPLL00EN_MASK						0x00000080
//------------------------------------------------------------------------------
// vborx_set_tx0_sen()
#define VBORX_SET_TX0_SEN_NUM								1
#define VBORX_CKSELEN_TX0_SEN_AUTO							0x00
#define VBORX_CKSELEN_TX0_SEN_MASK							0x00000004
//------------------------------------------------------------------------------
// vborx_set_tmds_ch_polarity()
#define VBORX_SET_TMDS_CH_POLARITY_NUM						1
#define VBORX_TXENCSET4_00_DATINV_NOR						0x00
#define VBORX_TXENCSET4_00_CHSWP_NOR						0x00
#define VBORX_TXENCSET4_00_DATINV_MASK						0x02
#define VBORX_TXENCSET4_00_CHSWP_MASK						0x01
//------------------------------------------------------------------------------
// vborx_tmds_analog_init()
#define VBORX_TMDS_ANALOG_INIT_NUM							6
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_01_TXPRM0EN_MAN	0x80
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_01_MASK			0x000000FF
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_05_TXPRM1EN_MAN	0x80
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_05_MASK			0x000000FF
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_1_66				0x66
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_1_MASK				0x000000FF
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_00_8E				0x8E
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_00_MASK			0x000000FF
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_01_40				0x40
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_01_MASK			0x000000FF
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_02_00				0x00
#define VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_02_MASK			0x000000FF
//------------------------------------------------------------------------------
// vborx_tmds_analog_param()
#define VBORX_TMDS_ANALOG_PARAM_LEN							6
#define VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0A_11			0x11
#define VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0B_DA			0xDA
#define VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0C_04			0x04
#define VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0D_87			0x87
#define VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0E_1A			0x1A
#define VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0F_90			0x90
//------------------------------------------------------------------------------
// vborx_tmds_analog_reset()
#define VBORX_TMDS_ANALOG_RESET_NUM							1
#define VBORX_TMDS_ANALOG_RESET_TXSYSCFG1_TP0SWPDWN_NOR		0x00
#define VBORX_TMDS_ANALOG_RESET_TXSYSCFG1_TP0ENCSRST_RST	0x02
#define VBORX_TMDS_ANALOG_RESET_TXSYSCFG1_MASK				0x000000FF
//------------------------------------------------------------------------------
// vborx_hdmitx_init()
#define VBORX_HDMITX_INIT_NUM								3
#define VBORX_HDMITX_INIT_TDPCMODE_00_DEEPCON_NORMAL		0x00
#define VBORX_HDMITX_INIT_TDPCMODE_00_DEEPHASE_DEF_PHASE_1	0x40
#define VBORX_HDMITX_INIT_TDPCMODE_00_DEEPCSRST_NORMAL		0x00
#define VBORX_HDMITX_INIT_TDPCMODE_00_DEEPC_8BIT			0x00
#define VBORX_HDMITX_INIT_TDPCMODE_00_MASK					0x000000FF
#define VBORX_HDMITX_INIT_SWMODE4_00_DNCON_OFF				0x00
#define VBORX_HDMITX_INIT_SWMODE4_00_MASK					0x000000FF
#define VBORX_HDMITX_INIT_TXENCSET4_02_SCRMOD_HDMI2			0x30
#define VBORX_HDMITX_INIT_TXENCSET4_02_SCRVER_HDMI2			0x08
#define VBORX_HDMITX_INIT_TXENCSET4_02_SCRON_OFF			0x00
#define VBORX_HDMITX_INIT_TXENCSET4_02_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_hdmitx_link()
#define VBORX_HDMITX_LINK_NUM1								4
#define VBORX_HDMITX_LINK_NUM2								1
#define VBORX_HDMITX_LINK_MAX_NUM							VBORX_HDMITX_LINK_NUM1
#define VBORX_HDMITX_LINK_CFGDBG_00_UPDHSC_8TIMES			0x80
#define VBORX_HDMITX_LINK_CFGDBG_00_MASK					0x000000FF
#define VBORX_HDMITX_LINK_CSCMOD3_ICFMT_RGB_FR				0x80
#define VBORX_HDMITX_LINK_CSCMOD3_OCFMT_RGB_FR				0x08
#define VBORX_HDMITX_LINK_CSCMOD3_MASK						0x000000FF
#define VBORX_HDMITX_LINK_TSYSCTRL_HDMI_HDMI				0x80
#define VBORX_HDMITX_LINK_TSYSCTRL_MASK						0x000000FF
#define VBORX_HDMITX_LINK_VMUTECNT_CCVMUTE_NORMAL			0x00
#define VBORX_HDMITX_LINK_VMUTECNT_MASK						0x000000FF
#define VBORX_HDMITX_LINK_CFMT_ICFMT_RGB_FR					0x80
#define VBORX_HDMITX_LINK_CFMT_OCFMT_RGB_FR					0x08
#define VBORX_HDMITX_LINK_CFMT_MASK							0x000000FF
#define VBORX_HDMITX_LINK_OUTWSET_OUTBW_2TAP				0x00
#define VBORX_HDMITX_LINK_OUTWSET_MASK						0x000000FF
#define VBORX_HDMITX_LINK_INREPSET_INREP_REPETITION_ADDED	0x03
#define VBORX_HDMITX_LINK_INREPSET_MASK						0x000000FF
#define VBORX_HDMITX_LINK_LEN1								2
#define VBORX_HDMITX_LINK_LEN2								7
#define VBORX_HDMITX_LINK_MAX_LEN							VBORX_HDMITX_LINK_LEN2
#define VBORX_HDMITX_LINK_VIDEOFMT_00_00					0x00
#define VBORX_HDMITX_LINK_VIDEOFMT_01_80					0x80
#define VBORX_HDMITX_LINK_HDMIFMT_01						0x01
#define VBORX_HDMITX_LINK_3DFMT_00							0x00
#define VBORX_HDMITX_LINK_HDMIVIC_07						0x07
#define VBORX_HDMITX_LINK_PCFMT_00							0x00
#define VBORX_HDMITX_LINK_VSELECT_00_C0						0xC0
//------------------------------------------------------------------------------
// vborx_hdmitx_link_update()
#define VBORX_HDMITX_LINK_UPDATE_NUM						1
#define VBORX_HDMITX_LINK_UPDATE_UPDCTRL_ALL_UPDATE			0xFF
#define VBORX_HDMITX_LINK_UPDATE_UPDCTRL_ALL_CLEARED		0x00
#define VBORX_HDMITX_LINK_UPDATE_UPDCTRL_MASK				0x000000FF
//------------------------------------------------------------------------------
// vborx_hdmitx_link_tsrst()
#define VBORX_HDMITX_LINK_TSRST_NUM							1
#define VBORX_HDMITX_LINK_TSRST_TSRST_CA					0xCA
#define VBORX_HDMITX_LINK_TSRST_TSRST_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_crc_check_setting()
#define VBORX_CRC_CHECK_SETTING_NUM							1
#define VBORX_CRC_CHECK_SETTING_ENCDBG_06_CRC_MODE_IN		0x40
#define VBORX_CRC_CHECK_SETTING_ENCDBG_06_CRC_ENA_ENA		0x80
#define VBORX_CRC_CHECK_SETTING_MASK						0x000000FF
#define VBORX_CRC_CHECK_SETTING_LEN							6
#define VBORX_CRC_CHECK_SETTING_TXENCSET2_00_CHA_15_08		0x1D
#define VBORX_CRC_CHECK_SETTING_TXENCSET2_01_CHA_07_00		0xE4
#define VBORX_CRC_CHECK_SETTING_TXENCSET2_02_CHB_15_08		0x22
#define VBORX_CRC_CHECK_SETTING_TXENCSET2_03_CHB_07_00		0xE3
#define VBORX_CRC_CHECK_SETTING_TXENCSET2_04_CHC_15_08		0x22
#define VBORX_CRC_CHECK_SETTING_TXENCSET2_05_CHC_07_00		0x7F
//------------------------------------------------------------------------------
// vborx_crc_check_setting_off()
#define VBORX_CRC_CHECK_SETTING_OFF_NUM						1
#define VBORX_CRC_CHECK_SETTING_OFF_ENCDBG_06_CRC_MODE_OUT	0x00
#define VBORX_CRC_CHECK_SETTING_OFF_ENCDBG_06_CRC_ENA_DIS	0x00
#define VBORX_CRC_CHECK_SETTING_OFF_MASK					0x000000FF
//------------------------------------------------------------------------------
// vborx_crc_error_check()
#define VBORX_CRC_ERROR_CHECK_NUM							1
#define VBORX_CRC_ERROR_CHECK_CRCMON_06_CRC_ERROR_MASK		0x00000001
//------------------------------------------------------------------------------
// vborx_crc_check_retry()
#define VBORX_CRC_CHECK_RETRY_NUM							2
#define VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_EN_P_IREF_11_DIS	VBORX_PHYX_SETTING1_PX_ENTCNT1_1_EN_P_IREF_11_DIS
#define VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_ANA_EN_11_ENA	VBORX_PHYX_SETTING1_PX_ENTCNT1_1_ANA_EN_11_ENA
#define VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_MAIN_EN_11_NOR	VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MAIN_EN_11_NORMAL
#define VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_RSTB_RST_00		0x00
#define VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_RSTB_RST_01		VBORX_PHYX_SETTING2_PX_ENTCNT_1_1_RSTB_RST
#define VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_MASK				VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MASK
//------------------------------------------------------------------------------
// vborx_npd3_setting()
#define VBORX_NPD3_SETTING_NUM								1
#define VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_0_PWR_ON		0x01
#define VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_1_PWR_ON		0x02
#define VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_2_PWR_ON		0x04
#define VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_3_BIT			0x03
#define VBORX_NPD3_SETTING_P1_ENTCNT2_0_MASK				0x000000FF
//------------------------------------------------------------------------------
// vborx_clock_switch_setting()
#define VBORX_CLOCK_SWITCH_SETTING_NUM						2
#define VBORX_CLOCK_SWITCH_SETTING_CDRCLK03_LANE0			VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK03_LANE0
#define VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_LANE4			VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK47_LANE4
#define VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_LANE7			0x0C
#define VBORX_CLOCK_SWITCH_SETTING_CDRCLK03_345_4BYTE		VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK03_345_4BYTE
#define VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_345_4BYTE		VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK47_345_4BYTE
#define VBORX_CLOCK_SWITCH_SETTING_VBORX_CLK_00_MASK		0x000000FF
//------------------------------------------------------------------------------
// vborx_boot_seq_pushd()
#define VBORX_BOOT_SEQ_PUSHD_NUM							11
#define VBORX_BOOT_SEQ_PUSHD_MASK							0x000000FF
//------------------------------------------------------------------------------
// vborx_boot_seq_popd()
#define VBORX_BOOT_SEQ_POPD_NUM								VBORX_BOOT_SEQ_PUSHD_NUM
#define VBORX_BOOT_SEQ_POPD_MASK							VBORX_BOOT_SEQ_PUSHD_MASK
//------------------------------------------------------------------------------
// vborx_boot_seq()
#define VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX					300
//------------------------------------------------------------------------------
#define VBORX_CONFIG_RETRY_CDR_LOCK_TIMES_DEFAULT			300
#define VBORX_CONFIG_RETRY_CDR_LOCK_INTERVAL_DEFAULT		10
//------------------------------------------------------------------------------
// sleep() time (ms)
#define VBORX_SLEEP_TIME_10MSEC								10
#define VBORX_SLEEP_TIME_1MSEC								1
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL												0
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
typedef enum _vborx_page {
	VBORX_PAGE_VBO_LINK0 = 0,
	VBORX_PAGE_VBO_LINK1 = 1,
	VBORX_PAGE_BOTH
} vborx_page;
//------------------------------------------------------------------------------
typedef enum _vborx_phy {
	VBORX_PHY_0 = 0,
	VBORX_PHY_1 = 1
} vborx_phy;
//------------------------------------------------------------------------------
typedef enum _vborx_reset_ctrl {
	VBORX_RESET_CTRL_RELEASE = 0,
	VBORX_RESET_CTRL_RESET   = 1
} vborx_reset_ctrl;
//------------------------------------------------------------------------------
typedef enum _vborx_pg {
	VBORX_PG_NOT_USE = 0,
	VBORX_PG_USE = 1
} vborx_pg;
//------------------------------------------------------------------------------
typedef enum _vborx_crc_err {
	VBORX_CRC_NO_ERROR = 0,
	VBORX_CRC_ERROR = 1
} vborx_crc_err;
//------------------------------------------------------------------------------
typedef enum _vborx_npd3 {
	VBORX_NPD3_OFF = 0,
	VBORX_NPD3_ON = 1
} vborx_npd3;
//------------------------------------------------------------------------------
typedef enum _vborx_ctrl_retry {
	VBORX_CTRL_RETRY_GET,
	VBORX_CTRL_RETRY_SET,
	VBORX_CTRL_RETRY_MAX
} vborx_ctrl_retry;
//------------------------------------------------------------------------------
typedef enum _vborx_ctrl_req {
	VBORX_CTRL_REQ_GET,
	VBORX_CTRL_REQ_SET,
	VBORX_CTRL_REQ_MAX
} vborx_ctrl_req;
//------------------------------------------------------------------------------
static uint32_t vborx_select_page(system_port port, vborx_page page);
static uint32_t vborx_cdrlock_hotplug_negate(system_port port);
static uint32_t vborx_init_setting(system_port port);
static uint32_t vborx_hvsync_polarity_setting(system_port port, system_inversion hvsync_polarity);
static uint32_t vborx_path_setting(system_port port);
static uint32_t vborx_clock_setting_whole(system_port port);
static uint32_t vborx_clock_setting_lane(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_clock_setting(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_phyx_param_setting(system_port port, vborx_phy phy);
static uint32_t vborx_phy_param_setting(system_port port);
static uint32_t vborx_pixel_pll_setting(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_get_gluectl0_1(system_port port, uint8_t *p_ctl);
static uint32_t vborx_link_setting1(system_port port, system_video_format vf, system_ctrl_vbo_lane cnt);
static uint32_t vborx_phyx_setting1(system_port port, vborx_phy phy);
static uint32_t vborx_phy_setting1(system_port port);
static uint32_t vborx_cdr_reset(system_port port, vborx_reset_ctrl rst);
static uint32_t vborx_lane_reset(system_port port, vborx_reset_ctrl rst);
static uint32_t vborx_cdr_lane_mode(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_select_input_lane(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_cdr_lock_setting(system_port port);
static uint32_t vborx_irq_stat_clearing(system_port port);
static uint32_t vborx_irq_mask_setting(system_port port);
static uint32_t vborx_link_setting2(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_phyx_setting2(system_port port, vborx_phy phy);
static uint32_t vborx_phy_setting2(system_port port);
static uint32_t vborx_hotplug_assert(system_port port);
static uint32_t vborx_cdrlock_detect(system_port port);
static uint32_t vborx_cdrlock_clear(system_port port);
static uint32_t vborx_cdrlock_seq(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_linkup_detect(system_port port);
static uint32_t vborx_linkup_clear(system_port port);
static uint32_t vborx_cdrlock_assert(system_port port);
static uint32_t vborx_linkup(system_port port, system_ctrl_vbo_lane cnt);
static uint32_t vborx_set_hstart_of_link_setting1(system_port port);
static uint32_t vborx_pg_aspect_ratio_h(system_port port);
static uint32_t vborx_pg_aspect_ratio_v1(system_port port);
static uint32_t vborx_pg_aspect_ratio_v2(system_port port);
static uint32_t vborx_pg_aspect_ratio(system_port port);
static uint32_t vborx_pg_ope(system_port port);
static uint32_t vborx_pg_use(system_port port, vborx_pg pg);
static uint32_t vborx_internal_pg_on(system_port port);
static uint32_t vborx_internal_pg_off(system_port port);
static uint32_t vborx_set_txpll00en(system_port port, uint8_t en);
static uint32_t vborx_set_tx0_sen(system_port port, uint8_t en);
static uint32_t vborx_set_tmds_ch_polarity(system_port port);
static uint32_t vborx_tmds_analog_init(system_port port);
static uint32_t vborx_tmds_analog_param(system_port port);
static uint32_t vborx_tmds_analog_reset(system_port port);
static uint32_t vborx_tmds_analog_setting(system_port port);
static uint32_t vborx_hdmitx_init(system_port port);
static uint32_t vborx_hdmitx_link(system_port port);
static uint32_t vborx_hdmitx_link_update(system_port port);
static uint32_t vborx_hdmitx_link_tsrst(system_port port);
static uint32_t vborx_hdmitx_link_setting(system_port port);
static uint32_t vborx_hdmitx_setting(system_port port);
static uint32_t vborx_crc_check_setting(system_port port);
static uint32_t vborx_crc_check_setting_off(system_port port);
static void vborx_wait_1frame(void);
static uint32_t vborx_crc_error_check(system_port port, vborx_crc_err *p_err);
static uint32_t vborx_crc_check_retry(system_port port);
static uint32_t vborx_npd3_setting(system_port port, vborx_npd3 npd3);
static uint32_t vborx_clock_switch_setting(system_port port);
static uint32_t vborx_boot_seq_pushd(system_port port, uint8_t *p_data);
static uint32_t vborx_boot_seq_popd(system_port port, uint8_t *p_data);
static uint32_t vborx_boot_seq(system_port port);
static uint32_t vborx_ctrl_retry_cdr_lock(system_config_retry *p_retry, vborx_ctrl_retry ctrl);
static uint32_t vborx_ctrl_req_abort(system_request *p_req, vborx_ctrl_req ctrl);
static system_request vborx_ctrl_get_abort_req(void);
static void vborx_ctrl_set_abort_req(system_request req);
//------------------------------------------------------------------------------
// abort request check
#define CHECK_ABORT() {												\
	if (vborx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		vborx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_VBORX_ABORT;										\
	}																\
}
//------------------------------------------------------------------------------
static uint32_t vborx_select_page(system_port port, vborx_page page)
{
	system_block_access baccess;
	uint8_t data[VBORX_PAGE_SELECT_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_PAGESEL_ADDR,
						VBORX_PAGE_SELECT_LEN, data);
	switch (page) {
		case VBORX_PAGE_VBO_LINK0:
		case VBORX_PAGE_VBO_LINK1:
			SET_BLOCK_DATA(data, 0, page);										// 0x2CFC
			SET_BLOCK_DATA(data, 1, VBORX_PAGE_SELECT_PAGECTRL_ATONCE_SINGLE);	// 0x2CFD
			break;
		case VBORX_PAGE_BOTH:
			SET_BLOCK_DATA(data, 0, VBORX_PAGE_SELECT_PAGECTRL_PAGESEL_DEF);	// 0x2CFC
			SET_BLOCK_DATA(data, 1, VBORX_PAGE_SELECT_PAGECTRL_ATONCE_BOTH);	// 0x2CFD
			break;
		default:
			return ERR_NG;
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdrlock_hotplug_negate(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CDRLOCK_HOTPLUG_NEGATE_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_CDRLOCK_HOTPLUG_NEGATE_NUM);
	// negate CDR lock and hot plug in order to init
	// HTPDN: negate
	// LOCKN: negate
	// 0x2C00
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_DETPINCTRL_0_ADDR,
						VBORX_LOCK_HTPD_DETPINCTRL_0_HTPDN_NEGATE
						| VBORX_LOCK_HTPD_DETPINCTRL_0_LOCKN_NEGATE,
						VBORX_LOCK_HTPD_DETPINCTRL_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t vborx_init_setting(system_port port)
{
	uint32_t ret;

	// negate CDR lock and hot plug
	ret = vborx_cdrlock_hotplug_negate(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hvsync_polarity_setting(system_port port, system_inversion hvsync_polarity)
{
	system_reg_access access;
	system_command cmd[VBORX_HVSYNC_POLARITY_NUM];
	uint8_t outhinven, outvinven;
	uint32_t ret;

	// page: 0,1

	if (hvsync_polarity == SYSTEM_INVERSION_ON) {
		outhinven = VBORX_HVSYNC_POLARITY_SELCTRL9_0_INVERTING;
		outvinven = VBORX_HVSYNC_POLARITY_SELCTRLA_0_INVERTING;
	} else {
		outhinven = VBORX_HVSYNC_POLARITY_SELCTRL9_0_NOT_INVERTING;
		outvinven = VBORX_HVSYNC_POLARITY_SELCTRLA_0_NOT_INVERTING;
	}

	SET_ACCESS(access, &port, cmd, VBORX_HVSYNC_POLARITY_NUM);
	// Hsync
	// 0x2C74
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_SELCTRL9_0_ADDR,
						outhinven,
						VBORX_HVSYNC_POLARITY_SELCTRL9_0_MASK);
	// Vsync
	// 0x2C78
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_LINK0_SELCTRLA_0_ADDR,
						outvinven,
						VBORX_HVSYNC_POLARITY_SELCTRLA_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_path_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_PATH_SETTING_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_PATH_SETTING_NUM);
	// 0x227B
	SET_COMMAND(cmd, 0, VIDEOIF_CC_MISC_ADDR,
						VBORX_PATH_SETTING_MISC_VBOREN_VBO,
						VBORX_PATH_SETTING_MISC_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_clock_setting_whole(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CLOCK_SETTING_WHOLE_NUM];
	system_block_access baccess;
	uint8_t data[VBORX_CLOCK_SETTING_WHOLE_LEN];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_CLOCK_SETTING_WHOLE_NUM);
	// RXCORE0 clock selector: manual setting
	// 0x1E7F
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						VBORX_CLOCK_SETTING_CKSELEN_RX0_SEN_MANUAL,
						VBORX_CLOCK_SETTING_CKSELEN_RX0_SEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_00_ADDR,
						VBORX_CLOCK_SETTING_WHOLE_LEN, data);
	// clock 1,2,3,4
	SET_BLOCK_DATA(data, 0, VBORX_CLOCK_SETTING_RXCORE0_00_CKSELRX0_07_00);		// 0x1E65
	SET_BLOCK_DATA(data, 1, VBORX_CLOCK_SETTING_RXCORE0_01_CKSELRX0_15_08);		// 0x1E66
	SET_BLOCK_DATA(data, 2, VBORX_CLOCK_SETTING_RXCORE0_02_CKSELRX0_23_16);		// 0x1E67
	SET_BLOCK_DATA(data, 3, VBORX_CLOCK_SETTING_RXCORE0_03_CKSELRX0_32_24);		// 0x1E68
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_clock_setting_lane(system_port port, system_ctrl_vbo_lane cnt)
{
	system_block_access baccess;
	uint8_t data[VBORX_CLOCK_SETTING_LANE_LEN];
	uint8_t sd_l0;
	uint32_t ret;

	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			sd_l0 = VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_L0_1_2;			// 1/2 freq
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			sd_l0 = VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_L0_1_4;			// 1/4 freq
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			sd_l0 = VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_L0_1_8;			// 1/8 freq
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_TMDS_VBORX_CLOCK_0_00_ADDR,
						VBORX_CLOCK_SETTING_LANE_LEN, data);
	// CDRCLK03: use lane0 clock
	// CDRCLK47: use lane4 clock
	// CDRCLK03_345: lane0-3: 4byte mode
	// CDRCLK47_345: lane4-7: 4byte mode
	SET_BLOCK_DATA(data, 0, VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK03_LANE0
							| VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK47_LANE4
							| VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK03_345_4BYTE
							| VBORX_CLOCK_SETTING_VBORX_CLK_00_CDRCLK47_345_4BYTE);	// 0x1E3C
	// reference clock for pixel clock: lane0-3 clock
	// SRAM read clock:
	// VBO-Rx clock
	// 480p down coverter clock: normal
	// hybrid pll output divider: divide by 1
	SET_BLOCK_DATA(data, 1, VBORX_CLOCK_SETTING_VBORX_CLK_01_SEL41_RX
							| sd_l0
							| VBORX_CLOCK_SETTING_VBORX_CLK_01_SEL21_R0
							| VBORX_CLOCK_SETTING_VBORX_CLK_01_SEL21_L0
							| VBORX_CLOCK_SETTING_VBORX_CLK_01_SD_HP);				// 0x1E3D
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_clock_setting(system_port port, system_ctrl_vbo_lane cnt)
{
	uint32_t ret;

	// whole setting
	ret = vborx_clock_setting_whole(port);
	CHECK_ERROR(ret);
	// lane setting
	ret = vborx_clock_setting_lane(port, cnt);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_phyx_param_setting(system_port port, vborx_phy phy)
{
	system_reg_access access;
	system_command cmd[VBORX_PHYX_PARAM_SETTING_NUM];
	system_block_access baccess;
	uint8_t data[VBORX_PHYX_PARAM_SETTING_MAX_LEN];
	uint32_t offset;
	uint32_t ret;

	offset = (uint32_t)phy * VBORX_PHYX_OFFSET;

	SET_ACCESS(access, &port, cmd, VBORX_PHYX_PARAM_SETTING_NUM);
	// 0x2E08: MODE: 0x1
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY0_P0_ENTCNT3_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_PX_ENTCNT3_0_MODE_01,
						VBORX_PHYX_PARAM_SETTING_PX_ENTCNT3_0_MASK);
	// 0x2E0C: SPEED: 0x1
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_PHY0_P0_SPDCNT_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_PX_SPDCNT_0_SPEED_01,
						VBORX_PHYX_PARAM_SETTING_PX_SPDCNT_0_SPEED_MASK);
	// 0x2E14: RT_[0-3]_RX_11: 0xF
	SET_COMMAND(cmd, 2, VIDEOIF_VBORX_PHY0_P0_TRMCNT_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_PX_TRMCNT_0_RT_RX_11_0F,
						VBORX_PHYX_PARAM_SETTING_PX_TRMCNT_0_RT_RX_11_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	// equalizer control
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_PHY0_P0_EQCNT_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_EQ_CTRL_LEN, data);
	// EQ_CTL_11[3:0]: 0x0
	SET_BLOCK_DATA(data, 0, VBORX_PHYX_PARAM_SETTING_PX_EQCNT_0_EQ_CTL_11_00);		// 0x2E10
	// EQ_CTL_11[5:4]: 0x3
	// DESKEW_TRIM[3:0]: 0x0
	SET_BLOCK_DATA(data, 1, VBORX_PHYX_PARAM_SETTING_PX_EQCNT_1_EQ_CTL_11_03
							| VBORX_PHYX_PARAM_SETTING_PX_EQCNT_1_DESKEWTRIM_00);	// 0x2E11
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	// CDR control
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_PHY0_P0_CDRCNT_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_CDR_CTRL_LEN, data);
	// CDR_VCO_CTRL: 0x5
	// CDRVCO_CM: 0x2
	// CDRVCO_CLMP: 0x0
	SET_BLOCK_DATA(data, 0, VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_0_CDR_CTRL_50
							| VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_0_CDR_CM_08
							| VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_0_CDR_CLMP_00);	// 0x2E18
	// CPF_TRIM: 0xa
	// CPD_TRIM: 0x3
	SET_BLOCK_DATA(data, 1, VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_1_CPF_TRIM_A0
							| VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_1_CPD_TRIM_03);	// 0x2E19
	// DIRECT_TRIM: 0x3
	SET_BLOCK_DATA(data, 2, VBORX_PHYX_PARAM_SETTING_PX_CDRCNT_2_DIRECT_TRIM_03);	// 0x2E1A
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_phy_param_setting(system_port port)
{
	uint32_t ret;

	// PHY0 parameter
	ret = vborx_phyx_param_setting(port, VBORX_PHY_0);
	CHECK_ERROR(ret);
	// PHY1 parameter
	ret = vborx_phyx_param_setting(port, VBORX_PHY_1);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pixel_pll_setting(system_port port, system_ctrl_vbo_lane cnt)
{
	system_block_access baccess;
	uint8_t data[VBORX_PIXEL_PLL_SETTING_LEN];
	uint8_t divn, fdiv;
	uint32_t ret;

	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			divn = VBORX_PIXEL_PLL_SETTING_PLLCTL_0_DIVN_08;				// 0x08
			fdiv = VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FDIV_30;				// divide by 4
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			divn = VBORX_PIXEL_PLL_SETTING_PLLCTL_0_DIVN_10;				// 0x10
			fdiv = VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FDIV_30;				// divide by 4
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			divn = VBORX_PIXEL_PLL_SETTING_PLLCTL_0_DIVN_10;				// 0x10
			fdiv = VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FDIV_10;				// divide by 2
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_GLUE_PLLCTL_0_ADDR,
						VBORX_PIXEL_PLL_SETTING_LEN, data);
	// P_DIVN:
	SET_BLOCK_DATA(data, 0, divn);												// 0x2EB0
	// P_FOUTSEL: 0x0
	// P_FDIV:
	// P_ICPM: 0x8
	SET_BLOCK_DATA(data, 1, VBORX_PIXEL_PLL_SETTING_PLLCTL_1_FOUTSEL
							| fdiv
							| VBORX_PIXEL_PLL_SETTING_PLLCTL_1_ICPM_08);		// 0x2EB1
	// P_NPOWD: 0x1
	SET_BLOCK_DATA(data, 2, VBORX_PIXEL_PLL_SETTING_PLLCTL_2_NPOWD_01);			// 0x2EB2
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_get_gluectl0_1(system_port port, uint8_t *p_ctl)
{
	system_reg_access access;
	system_command cmd[VBORX_GET_GLUECTL0_1_NUM];
	uint32_t ret;

	if (p_ctl == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, VBORX_GET_GLUECTL0_1_NUM);
	// 0x2E81
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_GLUE_GLUECTL0_1_ADDR,
						0,
						VBORX_GLUECTL0_1_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_ctl = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_link_setting1(system_port port, system_video_format vf, system_ctrl_vbo_lane cnt)
{
	system_block_access baccess;
	uint8_t data[VBORX_LINK_SETTING1_LEN];
	uint8_t ckmode, hstart_7_0, hstart_9_8, two_line_mode;
	uint8_t gluectl;
	uint32_t ret;

	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			ckmode = VBORX_GLUECTL0_0_CKMODE_2LANE;					// 2 lane
			two_line_mode = VBORX_GLUECTL0_3_TWO_LINE_MODE_2LINE;	// stored 2 lines
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			ckmode = VBORX_GLUECTL0_0_CKMODE_4LANE;					// 4 lane
			two_line_mode = VBORX_GLUECTL0_3_TWO_LINE_MODE_2LINE;	// stored 2 lines
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			ckmode = VBORX_GLUECTL0_0_CKMODE_8LANE;					// 8 lane
			two_line_mode = VBORX_GLUECTL0_3_TWO_LINE_MODE_4LINE;	// stored 4 lines
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}
	ret = system_ctrl_get_rx_glue_hstart(vf, &hstart_7_0, &hstart_9_8);
	CHECK_ERROR(ret);

	// get 0x2E81 register value
	ret = vborx_get_gluectl0_1(port, &gluectl);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_GLUE_GLUECTL0_0_ADDR,
						VBORX_LINK_SETTING1_LEN, data);
	// ckmode:
	// lane_sel: 0-3 lane (default)
	// sreset: normal (default)
	// fifo_rst: normail (default)
	// ipmode: r-b,g-g,b-r
	SET_BLOCK_DATA(data, 0, ckmode
							| VBORX_GLUECTL0_0_LANE_SEL_DEF
							| VBORX_GLUECTL0_0_SRESET_DEF
							| VBORX_GLUECTL0_0_FIFO_RST_DEF
							| VBORX_GLUECTL0_0_IPMODE_RB_GG_BR);	// 0x2E80
	// set read value
	SET_BLOCK_DATA(data, 1, gluectl);								// 0x2E81
	// hstart[7:0]:
	SET_BLOCK_DATA(data, 2, hstart_7_0);							// 0x2E82
	// hstart[9:8]:
	// two_line_mode:
	// hs_pol: positive (default)
	// vs_pol: positive (default)
	SET_BLOCK_DATA(data, 3, hstart_9_8
							| two_line_mode
							| VBORX_GLUECTL0_3_HS_POL_DEF
							| VBORX_GLUECTL0_3_VS_POL_DEF);			// 0x2E83
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_phyx_setting1(system_port port, vborx_phy phy)
{
	system_reg_access access;
	system_command cmd[VBORX_PHYX_SETTING1_NUM];
	uint32_t offset;
	uint32_t ret;

	offset = (uint32_t)phy * VBORX_PHYX_OFFSET;

	SET_ACCESS(access, &port, cmd, VBORX_PHYX_SETTING1_NUM);
	// F_LOOP_ACTIVATE: frequency lock
	// 0x2E0D
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY0_P0_SPDCNT_1_ADDR + offset,
						VBORX_PHYX_SETTING1_PX_SPDCNT_1_LOOP_ACTIVATE_FREQ,
						VBORX_PHYX_SETTING1_PX_SPDCNT_1_LOOP_ACTIVATE_MASK);
	// NPD: power on
	//   already done by system_ctrl_init

	// NPD_0: power on
	// NPD_1: power on
	// NPD_2: power on
	// NPD_3: power on
	//   already done by system_ctrl_init

	// EN_P_IREF_11: disable
	// ANA_EN_11: enable
	// MAIN_EN_11: normal
	// 0x2E01
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_PHY0_P0_ENTCNT1_1_ADDR + offset,
						VBORX_PHYX_SETTING1_PX_ENTCNT1_1_EN_P_IREF_11_DIS
						| VBORX_PHYX_SETTING1_PX_ENTCNT1_1_ANA_EN_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MAIN_EN_11_NORMAL,
						VBORX_PHYX_SETTING1_PX_ENTCNT1_1_MASK);
	// MODE: 0x1 (rewrite because of omitting to read for mask write)
	// BGAP_V12_EN_11: enable
	// START_BIAS_11: 0x1
	// RX_TERM_EN_11: enable
	// EB11_EQ: enable
	// DESKEW_EN: enable
	// DESKEW_AUTO_DIS: disable
	// 0x2E08
	SET_COMMAND(cmd, 2, VIDEOIF_VBORX_PHY0_P0_ENTCNT3_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_PX_ENTCNT3_0_MODE_01
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_BGAP_V12_EN_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_START_BIAS_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_RX_TERM_EN_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_EB11_EQ_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_DESKEWEN_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_DESKEWAUTO_DIS_DIS,
						VBORX_PHYX_SETTING1_PX_ENTCNT3_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_phy_setting1(system_port port)
{
	uint32_t ret;

	// PHY0 setting
	ret = vborx_phyx_setting1(port, VBORX_PHY_0);
	CHECK_ERROR(ret);
	// PHY1 setting
	ret = vborx_phyx_setting1(port, VBORX_PHY_1);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdr_reset(system_port port, vborx_reset_ctrl rst)
{
	system_reg_access access;
	system_command cmd[VBORX_CDR_RESET_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_CDR_RESET_NUM);
	// 0x2C1B: CDRxRSR: reset or release
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_LINKRSTCTRL_3_ADDR,
						((uint32_t)rst << VBORX_CDR_RESET_LINKRSTCTRL_3_CD0RSR_BIT)
						| ((uint32_t)rst << VBORX_CDR_RESET_LINKRSTCTRL_3_CD1RSR_BIT)
						| ((uint32_t)rst << VBORX_CDR_RESET_LINKRSTCTRL_3_CD2RSR_BIT)
						| ((uint32_t)rst << VBORX_CDR_RESET_LINKRSTCTRL_3_CD3RSR_BIT),
						VBORX_CDR_RESET_LINKRSTCTRL_3_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_lane_reset(system_port port, vborx_reset_ctrl rst)
{
	system_reg_access access;
	system_command cmd[VBORX_LANE_RESET_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_LANE_RESET_NUM);
	// 0x2C18: PHDRSR: all reset or relase
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_LINKRSTCTRL_0_ADDR,
						(uint32_t)rst << VBORX_LANE_RESET_LINKRSTCTRL_0_PHDRSR_BIT,
						VBORX_LANE_RESET_LINKRSTCTRL_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdr_lane_mode(system_port port, system_ctrl_vbo_lane cnt)
{
	system_reg_access access;
	system_command cmd[VBORX_CDR_LANE_MODE_NUM];
	system_block_access baccess;
	uint8_t data[VBORX_CDR_LANE_MODE_LEN];
	uint8_t laneen;
	uint32_t ret;

	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
			laneen = VBORX_CDR_LANE_MODE_LANEN_0_LANEEN_1LANE;
			break;
		case SYSTEM_CTRL_VBO_LANE_2:
			laneen = VBORX_CDR_LANE_MODE_LANEN_0_LANEEN_2LANE;
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
		case SYSTEM_CTRL_VBO_LANE_8:
			laneen = VBORX_CDR_LANE_MODE_LANEN_0_LANEEN_4_8LANE;
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}

	// page: 0,1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_MODECTRL_0_ADDR,
						VBORX_CDR_LANE_MODE_LEN, data);
	// CLKLNKSEL: CDR clock from lane0
	SET_BLOCK_DATA(data, 0, VBORX_CDR_LANE_MODE_MODECTRL_0_CLKLNKSEL_LANE0);	// 0x2C58
	// BYTEMODESEL: 4 byte mode
	SET_BLOCK_DATA(data, 1, VBORX_CDR_LANE_MODE_MODECTRL_1_BYTEMODESEL_4BYTE);	// 0x2C59
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, VBORX_CDR_LANE_MODE_NUM);
	// 0x2C5C: LANEEN
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_LANEN_0_ADDR,
						laneen,
						VBORX_CDR_LANE_MODE_LANEN_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_select_input_lane(system_port port, system_ctrl_vbo_lane cnt)
{
	system_reg_access access;
	system_command cmd[VBORX_SELECT_INPUT_LANE_NUM];
	uint8_t inlane1sel;
	uint32_t ret;

	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
			inlane1sel = VBORX_SELCTRLB_1_LANE0;
			break;
		case SYSTEM_CTRL_VBO_LANE_2:
		case SYSTEM_CTRL_VBO_LANE_4:
		case SYSTEM_CTRL_VBO_LANE_8:
			inlane1sel = VBORX_SELCTRLB_1_LANE1;
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_SELECT_INPUT_LANE_NUM);
	// 0x2C7D
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_SELCTRLB_1_ADDR,
						inlane1sel,
						VBORX_SELCTRLB_1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdr_lock_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBORX_CDR_LOCK_SETTING_LEN];
	uint32_t ret;

	// page: 0,1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_CDRCTR0_0_ADDR,
						VBORX_CDR_LOCK_SETTING_LEN, data);
	// MAXLSR: 0x3
	SET_BLOCK_DATA(data, 0, VBORX_CDR_LOCK_SETTING_CDRCTR0_0_MAXLSR_03);		// 0x2C30
	// LSLKTR: 0x20
	SET_BLOCK_DATA(data, 1, VBORX_CDR_LOCK_SETTING_CDRCTR0_1_LSLKTR_20);		// 0x2C31
	// LSLCKR: 0x0
	// LSLLKR: 0x0
	SET_BLOCK_DATA(data, 2, VBORX_CDR_LOCK_SETTING_CDRCTR0_2_LSLLKR_00
							| VBORX_CDR_LOCK_SETTING_CDRCTR0_2_LSLCKR_00);		// 0x2C32
	// LSTHMR: 0x1
	SET_BLOCK_DATA(data, 3, VBORX_CDR_LOCK_SETTING_CDRCTR0_3_LSTHMR_01);		// 0x2C33
	// LSLTGR[7:0]: 0x74
	SET_BLOCK_DATA(data, 4, VBORX_CDR_LOCK_SETTING_CDRCTR1_0_LSLTGR_7_0_74);	// 0x2C34
	// LSLTGR[15:8]: 0x01
	SET_BLOCK_DATA(data, 5, VBORX_CDR_LOCK_SETTING_CDRCTR1_1_LSLTGR_15_8_01);	// 0x2C35
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_irq_stat_clearing(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_IRQ_STAT_CLEARING_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_IRQ_STAT_CLEARING_NUM);
	// CDRLIRQ: clear
	// CDRUIRQ: clear
	// LINKUIRQ: clear
	// LINKDIRQ: clear
	// 0x2C10
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_IRQSTATUS_0_ADDR,
						VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_CDRLIRQ_CLR
						| VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_CDRUIRQ_CLR
						| VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_LINKUIRQ_CLR
						| VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_LINKDIRQ_CLR,
						VBORX_IRQ_STAT_CLEARING_IRQSTATUS_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_irq_mask_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_IRQ_MASK_SETTING_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_IRQ_MASK_SETTING_NUM);
	// CDRLEN: output
	// CDRUEN: output
	// LINKUEN: output
	// LINKDEN: output
	// 0x2C14
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_IRQMASK_0_ADDR,
						VBORX_IRQ_MASK_SETTING_IRQMASK_0_CDRLEN_OUT
						| VBORX_IRQ_MASK_SETTING_IRQMASK_0_CDRUEN_OUT
						| VBORX_IRQ_MASK_SETTING_IRQMASK_0_LINKUEN_OUT
						| VBORX_IRQ_MASK_SETTING_IRQMASK_0_LINKDEN_OUT,
						VBORX_IRQ_MASK_SETTING_IRQMASK_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_link_setting2(system_port port, system_ctrl_vbo_lane cnt)
{
	uint32_t ret;

	// page: 0,1

	// reset of CDR lock detector
	ret = vborx_cdr_reset(port, VBORX_RESET_CTRL_RESET);
	CHECK_ERROR(ret);
	// reset of lane processor
	ret = vborx_lane_reset(port, VBORX_RESET_CTRL_RESET);
	CHECK_ERROR(ret);
	// set mode of CDR lock detector and lane processor
	ret = vborx_cdr_lane_mode(port, cnt);
	CHECK_ERROR(ret);
	// select input lane
	ret = vborx_select_input_lane(port, cnt);
	CHECK_ERROR(ret);
	// set up CDR lock detector
	ret = vborx_cdr_lock_setting(port);
	CHECK_ERROR(ret);
	// clear up IRQ status
	ret = vborx_irq_stat_clearing(port);
	CHECK_ERROR(ret);
	// set IRQ mask invalid
	ret = vborx_irq_mask_setting(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_phyx_setting2(system_port port, vborx_phy phy)
{
	system_reg_access access;
	system_command cmd[VBORX_PHYX_SETTING2_NUM];
	uint32_t offset;
	uint32_t ret;

	offset = (uint32_t)phy * VBORX_PHYX_OFFSET;

	SET_ACCESS(access, &port, cmd, VBORX_PHYX_SETTING2_NUM);
	// MODE: 0x1 (rewrite because of omitting to read for mask write)
	// BGAP_V12_EN_11: enable (rewrite because of omitting to read for mask write)
	// START_BIAS_11: 0x1 (rewrite because of omitting to read for mask write)
	// RX_TERM_EN_11: enable (rewrite because of omitting to read for mask write)
	// EB11_EQ: enable (rewrite because of omitting to read for mask write)
	// EN_P_CDR_11: 0x1
	// DESKEW_EN: enable (rewrite because of omitting to read for mask write)
	// DESKEW_AUTO_DIS: disable (rewrite because of omitting to read for mask write)
	// 0x2E08, 0x2E48
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY0_P0_ENTCNT3_0_ADDR + offset,
						VBORX_PHYX_PARAM_SETTING_PX_ENTCNT3_0_MODE_01
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_BGAP_V12_EN_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_START_BIAS_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_RX_TERM_EN_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_EB11_EQ_ENA
						| VBORX_PHYX_SETTING2_PX_ENTCNT3_0_EN_P_CDR_11_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_DESKEWEN_ENA
						| VBORX_PHYX_SETTING1_PX_ENTCNT3_0_DESKEWAUTO_DIS_DIS,
						VBORX_PHYX_SETTING2_PX_ENTCNT3_0_MASK);
	// RSTB: reset
	// 0x2E01, 0x2E41
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_PHY0_P0_ENTCNT1_1_ADDR + offset,
						VBORX_PHYX_SETTING2_PX_ENTCNT_1_1_RSTB_RST,
						VBORX_PHYX_SETTING2_PX_ENTCNT_1_1_RSTB_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_phy_setting2(system_port port)
{
	uint32_t ret;

	// PHY0 setting
	ret = vborx_phyx_setting2(port, VBORX_PHY_0);
	CHECK_ERROR(ret);
	// PHY1 setting
	ret = vborx_phyx_setting2(port, VBORX_PHY_1);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hotplug_assert(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_HOTPLUG_ASSERT_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_HOTPLUG_ASSERT_NUM);
	// 0x2C00: HTPDN: 0x0
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_DETPINCTRL_0_ADDR,
						VBORX_HOTPLUG_ASSERT_DETPINCTRL_0_HTPDN_ASSERT,
						VBORX_HOTPLUG_ASSERT_DETPINCTRL_0_HTPDN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdrlock_detect(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CDRLOCK_DETECT_NUM];
	uint32_t i;
	system_config_retry retry_cdr_lock;
	uint32_t ret;

	// get retry info
	ret = vborx_ctrl_retry_cdr_lock(&retry_cdr_lock, VBORX_CTRL_RETRY_GET);
	CHECK_ERROR(ret);

	// page: 0 or 1

	SET_ACCESS(access, &port, cmd, VBORX_CDRLOCK_DETECT_NUM);
	// 0x2C10: CDRLIRQ: if 0x1, detected
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_IRQSTATUS_0_ADDR,
						0,
						VBORX_CDRLOCK_DETECT_IRQSTATUS_0_CDRLIRQ_MASK);
	// retry CDR lock detection
	for (i=0; i<retry_cdr_lock.times; i++) {
		CHECK_ABORT();
		ret = lf9129_device_read(&access);
		CHECK_ERROR(ret);
		if (cmd[0].data == VBORX_CDRLOCK_DETECT_IRQSTATUS_0_CDRLIRQ_DETECT) {
			break;
		}
		SLEEP(retry_cdr_lock.interval);
	}
	if (i == retry_cdr_lock.times) {
		return ERR_VBORX_CDR_LOCK_DETECT_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdrlock_clear(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CDRLOCK_CLEAR_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_CDRLOCK_CLEAR_NUM);
	// 0x2C10: CDRLIRQ: 0x1
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_IRQSTATUS_0_ADDR,
						VBORX_CDRLOCK_CLEAR_IRQSTATUS_0_CDRLIRQ_CLEAR,
						VBORX_CDRLOCK_CLEAR_IRQSTATUS_0_CDRLIRQ_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdrlock_seq(system_port port, system_ctrl_vbo_lane cnt)
{
	uint32_t ret;

	// page: 0,1

	// release reset of CDR lock detector
	MES("Func: %s, Line: %d release reset of CDR lock detector start", __func__, __LINE__);
	ret = vborx_cdr_reset(port, VBORX_RESET_CTRL_RELEASE);
	MES("Func: %s, Line: %d release reset of CDR lock detector end", __func__, __LINE__);
	CHECK_ERROR(ret);
	// page: 0
	MES("Func: %s, Line: %d select page: 0 start", __func__, __LINE__);
	ret = vborx_select_page(port, VBORX_PAGE_VBO_LINK0);
	MES("Func: %s, Line: %d select page: 0 end", __func__, __LINE__);
	CHECK_ERROR(ret);
	// detect IRQ of CDR lock for link0
	MES("Func: %s, Line: %d detect IRQ of CDR lock for link0 start", __func__, __LINE__);
	ret = vborx_cdrlock_detect(port);
	MES("Func: %s, Line: %d detect IRQ of CDR lock for link0 end", __func__, __LINE__);
	CHECK_ERROR(ret);
	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
		case SYSTEM_CTRL_VBO_LANE_4:
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			// page: 1
			ret = vborx_select_page(port, VBORX_PAGE_VBO_LINK1);
			CHECK_ERROR(ret);
			// detect IRQ of CDR lock for link1
			MES("Func: %s, Line: %d detect IRQ of CDR lock for link1 start", __func__, __LINE__);
			ret = vborx_cdrlock_detect(port);
			MES("Func: %s, Line: %d detect IRQ of CDR lock for link1 end", __func__, __LINE__);
			CHECK_ERROR(ret);
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}
	// page: 0,1
	MES("Func: %s, Line: %d vborx_select_page sss", __func__, __LINE__);
	ret = vborx_select_page(port, VBORX_PAGE_BOTH);
	CHECK_ERROR(ret);
	// release reset of lane processor
	MES("Func: %s, Line: %d release reset of lane processor", __func__, __LINE__);
	ret = vborx_lane_reset(port, VBORX_RESET_CTRL_RELEASE);
	CHECK_ERROR(ret);
	// clear up IRQ status of CDR lock
	MES("Func: %s, Line: %d clear up IRQ status of CDR lock", __func__, __LINE__);
	ret = vborx_cdrlock_clear(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_linkup_detect(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_LINKUP_DETECT_NUM];
	uint32_t i;
	uint32_t ret;

	// page: 0 or 1

	SET_ACCESS(access, &port, cmd, VBORX_LINKUP_DETECT_NUM);
	// 0x2C10: LINKUIRQ: if 0x1, detected
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_IRQSTATUS_0_ADDR,
						0,
						VBORX_LINKUP_DETECT_IRQSTATUS_0_LINKUIRQ_MASK);
	for (i=0; i<VBORX_LINKUP_DETECT_RETRY_MAX; i++) {
		CHECK_ABORT();
		ret = lf9129_device_read(&access);
		CHECK_ERROR(ret);
		if (cmd[0].data == VBORX_LINKUP_DETECT_IRQSTATUS_0_LINKUIRQ_DETECT) {
			break;
		}
		SLEEP(VBORX_LINKUP_DETECT_RETRY_INTERVAL);
	}
	if (i == VBORX_LINKUP_DETECT_RETRY_MAX) {
		return ERR_VBORX_LINK_UP_DETECT_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_linkup_clear(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_LINKUP_CLEAR_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_LINKUP_CLEAR_NUM);
	// 0x2C10: LINKUIRQ: 0x1
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_IRQSTATUS_0_ADDR,
						VBORX_LINKUP_CLEAR_IRQSTATUS_0_LINKUIRQ_CLEAR,
						VBORX_LINKUP_CLEAR_IRQSTATUS_0_LINKUIRQ_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_cdrlock_assert(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CDRLOCK_ASSERT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_CDRLOCK_ASSERT_NUM);
	// 0x2E0D: F_LOOP_ACTIVATE: phase lock
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY0_P0_SPDCNT_1_ADDR,
						VBORX_CDRLOCK_ASSERT_PX_SPDCNT_1_F_LOOP_ACT_PHASE,
						VBORX_CDRLOCK_ASSERT_PX_SPDCNT_1_MASK);
	// 0x2E4D: F_LOOP_ACTIVATE: phase lock
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_PHY1_P1_SPDCNT_1_ADDR,
						VBORX_CDRLOCK_ASSERT_PX_SPDCNT_1_F_LOOP_ACT_PHASE,
						VBORX_CDRLOCK_ASSERT_PX_SPDCNT_1_MASK);
	// page: 0,1

	// 0x2C00: LOCKN: assert (0x0)
	SET_COMMAND(cmd, 2, VIDEOIF_VBORX_LINK0_DETPINCTRL_0_ADDR,
						VBORX_CDRLOCK_ASSERT_DETPINCTRL_0_LOCKN_ASSERT,
						VBORX_CDRLOCK_ASSERT_DETPINCTRL_0_LOCKN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_linkup(system_port port, system_ctrl_vbo_lane cnt)
{
	uint32_t ret;

	// page: 0
	ret = vborx_select_page(port, VBORX_PAGE_VBO_LINK0);
	CHECK_ERROR(ret);
	// detect IRQ of link up for link0
	ret = vborx_linkup_detect(port);
	CHECK_ERROR(ret);
	switch (cnt) {
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
		case SYSTEM_CTRL_VBO_LANE_4:
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			// page: 1
			ret = vborx_select_page(port, VBORX_PAGE_VBO_LINK1);
			CHECK_ERROR(ret);
			// detect IRQ of link up for link1
			ret = vborx_linkup_detect(port);
			CHECK_ERROR(ret);
			break;
		default:
			return ERR_VBORX_RECEIVE_FORMAT;
	}
	// page: 0,1
	ret = vborx_select_page(port, VBORX_PAGE_BOTH);
	CHECK_ERROR(ret);
	// clear up IRQ status of link up
	ret = vborx_linkup_clear(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_set_hstart_of_link_setting1(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBORX_SET_HSTART_OF_LINK_SETTING1_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_GLUE_GLUECTL0_2_ADDR,
						VBORX_SET_HSTART_OF_LINK_SETTING1_LEN, data);
	// for CRC check settings
	SET_BLOCK_DATA(data, 0, VBORX_GLUECTL0_2_HSTART_UHD_50_60P);	// 0x2E82
	SET_BLOCK_DATA(data, 1, VBORX_GLUECTL0_3_HSTART_UHD_50_60P);	// 0x2E83
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pg_aspect_ratio_h(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBORX_PG_ASPECT_RATIO_H_LEN];
	uint32_t ret;

	// page: 0,1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_VBO_PG_HPARAMCTRL0_0_ADDR,
						VBORX_PG_ASPECT_RATIO_H_LEN, data);
	// Hbp: 96
	SET_BLOCK_DATA(data, 0, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_0_30);	// 0x2CB8
	SET_BLOCK_DATA(data, 1, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_1_00);	// 0x2CB9
	// Htotal: 1100
	SET_BLOCK_DATA(data, 2, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_2_26);	// 0x2CBA
	SET_BLOCK_DATA(data, 3, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_0_02);	// 0x2CBB
	// Hactive: 960
	SET_BLOCK_DATA(data, 4, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_1_E0);	// 0x2CBC
	SET_BLOCK_DATA(data, 5, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_2_01);	// 0x2CBD
	// Hwidth: 22
	SET_BLOCK_DATA(data, 6, VBORX_PG_ASPECT_RATIO_H_VBO_PG_HPARAMCTRL0_3_3_0B);	// 0x2CBE
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pg_aspect_ratio_v1(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBORX_PG_ASPECT_RATIO_V1_LEN];
	uint32_t ret;

	// page: 0,1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_VBO_PG_VPARAMCTRL0_0_ADDR,
						VBORX_PG_ASPECT_RATIO_V1_LEN, data);
	// Vbp: 10
	SET_BLOCK_DATA(data, 0, VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_0_0A);	// 0x2CEC
	SET_BLOCK_DATA(data, 1, VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_1_00);	// 0x2CED
	// Vtotal: 40
	SET_BLOCK_DATA(data, 2, VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_2_28);	// 0x2CEE
	SET_BLOCK_DATA(data, 3, VBORX_PG_ASPECT_RATIO_V1_VBO_PG_VPARAMCTRL0_3_00);	// 0x2CEF
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pg_aspect_ratio_v2(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBORX_PG_ASPECT_RATIO_V2_LEN];
	uint32_t ret;

	// page: 0,1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_VBO_PG_VPARAMCTRL1_0_ADDR,
						VBORX_PG_ASPECT_RATIO_V2_LEN, data);
	// Vactive: 20
	SET_BLOCK_DATA(data, 0, VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_0_14);	// 0x2CB0
	SET_BLOCK_DATA(data, 1, VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_1_00);	// 0x2CB1
	// Vwidth: 2
	SET_BLOCK_DATA(data, 2, VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_2_02);	// 0x2CB2
	// default (padding)
	SET_BLOCK_DATA(data, 3, VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL1_3_00);	// 0x2CB3
	// Vfp: 0
	SET_BLOCK_DATA(data, 4, VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL2_0_00);	// 0x2CB4
	SET_BLOCK_DATA(data, 5, VBORX_PG_ASPECT_RATIO_V2_VBO_PG_VPARAMCTRL2_1_00);	// 0x2CB5
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pg_aspect_ratio(system_port port)
{
	uint32_t ret;

	// page: 0,1

	// H parameter contrl
	ret = vborx_pg_aspect_ratio_h(port);
	CHECK_ERROR(ret);
	// V parameter contrl 1
	ret = vborx_pg_aspect_ratio_v1(port);
	CHECK_ERROR(ret);
	// V parameter contrl 2
	ret = vborx_pg_aspect_ratio_v2(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pg_ope(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_PG_OPE_NUM];
	system_block_access baccess;
	uint8_t data[VBORX_PG_OPE_LEN];
	uint32_t ret;

	// page: 0,1

	// PG output signal setting
	SET_ACCESS(access, &port, cmd, VBORX_PG_OPE_NUM);
	// 0x2CE8: VBO_PG_LANE0INCSEL: 4 pixel increased
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_VBO_PG_LANEXINCSEL_0_ADDR,
						VBORX_PG_OPE_VBO_PG_LANEXINCSEL_0_4PIXEL,
						VBORX_PG_OPE_VBO_PG_LANEXINCSEL_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	// incremental value setting
	// initial value setting
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBORX_LINK0_VBO_PG_R_CTRL_0_ADDR,
						VBORX_PG_OPE_LEN, data);
	SET_BLOCK_DATA(data, 0, VBORX_PG_OPE_VBO_PG_R_CTRL_0_10);		// 0x2C88 (R inc)
	SET_BLOCK_DATA(data, 1, VBORX_PG_OPE_VBO_PG_R_CTRL_1_00);		// 0x2C89 (R inc)
	SET_BLOCK_DATA(data, 2, VBORX_PG_OPE_VBO_PG_R_CTRL_2_00);		// 0x2C8A (R init)
	SET_BLOCK_DATA(data, 3, VBORX_PG_OPE_VBO_PG_R_CTRL_3_00);		// 0x2C8B (R init)
	SET_BLOCK_DATA(data, 4, VBORX_PG_OPE_VBO_PG_G_CTRL_0_08);		// 0x2C8C (G inc)
	SET_BLOCK_DATA(data, 5, VBORX_PG_OPE_VBO_PG_G_CTRL_1_00);		// 0x2C8D (G inc)
	SET_BLOCK_DATA(data, 6, VBORX_PG_OPE_VBO_PG_G_CTRL_2_00);		// 0x2C8E (G init)
	SET_BLOCK_DATA(data, 7, VBORX_PG_OPE_VBO_PG_G_CTRL_3_00);		// 0x2C8F (G init)
	SET_BLOCK_DATA(data, 8, VBORX_PG_OPE_VBO_PG_B_CTRL_0_04);		// 0x2C90 (B inc)
	SET_BLOCK_DATA(data, 9, VBORX_PG_OPE_VBO_PG_B_CTRL_1_00);		// 0x2C91 (B inc)
	SET_BLOCK_DATA(data, 10, VBORX_PG_OPE_VBO_PG_B_CTRL_2_00);		// 0x2C92 (B init)
	SET_BLOCK_DATA(data, 11, VBORX_PG_OPE_VBO_PG_B_CTRL_3_00);		// 0x2C93 (B init)
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_pg_use(system_port port, vborx_pg pg)
{
	system_reg_access access;
	system_command cmd[VBORX_PG_USE_NUM];
	uint32_t ret;

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_PG_USE_NUM);
	// 0x2C80: vbopgen: PG off
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_LINK0_VBO_PG_SET_0_ADDR,
						(uint32_t)pg << VBORX_PG_USE_PG_SET_0_VBOPGEN_BIT,
						VBORX_PG_USE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_internal_pg_on(system_port port)
{
	uint32_t ret;

	// page: 0,1

	// set Hstart of link setting1
	ret = vborx_set_hstart_of_link_setting1(port);
	CHECK_ERROR(ret);
	// set H/V aspect ratio of internal PG (4400x40)
	ret = vborx_pg_aspect_ratio(port);
	CHECK_ERROR(ret);
	// set operations of internal PG
	ret = vborx_pg_ope(port);
	CHECK_ERROR(ret);
	// PG on
	ret = vborx_pg_use(port, VBORX_PG_USE);
	CHECK_ERROR(ret);
	// H/Vsync polarity setting: inversion
	ret = vborx_hvsync_polarity_setting(port, SYSTEM_INVERSION_ON);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_internal_pg_off(system_port port)
{
	uint32_t ret;

	// page: 0,1

	// PG off
	ret = vborx_pg_use(port, VBORX_PG_NOT_USE);
	CHECK_ERROR(ret);

	// note: OUTHINVEN*, OUTVINVEN* shuld be set later

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_set_txpll00en(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[VBORX_SET_TXPLL00EN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_SET_TXPLL00EN_NUM);
	// 0x1E47: TXPLL00EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_TXPLL00_01_ADDR,
						en,
						VBORX_TXPLL00_01_TXPLL00EN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_set_tx0_sen(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[VBORX_SET_TX0_SEN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_SET_TX0_SEN_NUM);
	// 0x1E7F: TX0_SEN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						en,
						VBORX_CKSELEN_TX0_SEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_set_tmds_ch_polarity(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_SET_TMDS_CH_POLARITY_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_SET_TMDS_CH_POLARITY_NUM);
	// DATINV: normal
	// CHSWP: normal
	// 0x10C5
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_TXENCSET4_00_ADDR,
						VBORX_TXENCSET4_00_DATINV_NOR
						| VBORX_TXENCSET4_00_CHSWP_NOR,
						VBORX_TXENCSET4_00_DATINV_MASK
						| VBORX_TXENCSET4_00_CHSWP_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_tmds_analog_init(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_TMDS_ANALOG_INIT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_TMDS_ANALOG_INIT_NUM);
	// 0x1E0B: TXPRM0EN: manual control
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSTXP0_3_01_ADDR,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_01_TXPRM0EN_MAN,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_01_MASK);
	// 0x1E0F: TXPRM1EN: manual control
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_TMDSTXP0_3_05_ADDR,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_05_TXPRM1EN_MAN,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_3_05_MASK);
	// NPDTX: normal
	// NRS_SUSP: normal
	// SWINGNPD: normal
	// NPDPLL: normal
	// NPDCKO: power down
	// 0x1E00
	SET_COMMAND(cmd, 2, VIDEOIF_TMDS_TMDSTXP0_1_ADDR,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_1_66,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_1_MASK);
	// NPDBGR: normal
	// NPDDAT2: power down
	// NPDDAT1: power down
	// NPDDAT0: poser down
	// NRSTPLL: normal
	// TXPRM00: 0x6
	// 0x1E02
	SET_COMMAND(cmd, 3, VIDEOIF_TMDS_TMDSTXP0_2_00_ADDR,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_00_8E,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_00_MASK);
	// 0x1E04: TXPRM01: 0x4
	SET_COMMAND(cmd, 4, VIDEOIF_TMDS_TMDSTXP0_2_01_ADDR,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_01_40,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_01_MASK);
	// 0x1E08: TXPRM02: 0x00
	SET_COMMAND(cmd, 5, VIDEOIF_TMDS_TMDSTXP0_2_02_ADDR,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_02_00,
						VBORX_TMDS_ANALOG_INIT_TMDSTXP0_2_02_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_tmds_analog_param(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBORX_TMDS_ANALOG_PARAM_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_TMDS_TMDSTXP0_3_00_ADDR,
						VBORX_TMDS_ANALOG_PARAM_LEN, data);
	// CKDIV4_EN: enable
	// TXPRM10: 0x1
	SET_BLOCK_DATA(data, 0, VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0A_11);		// 0x1E0A
	// TXPRM0EN: 0x1
	// TXPRM03: 0x5A
	SET_BLOCK_DATA(data, 1, VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0B_DA);		// 0x1E0B
	// TXPRM04: 0x04
	SET_BLOCK_DATA(data, 2, VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0C_04);		// 0x1E0C
	// TXPRM05: 0x87
	SET_BLOCK_DATA(data, 3, VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0D_87);		// 0x1E0D
	// TXPRM11: 0x1A
	SET_BLOCK_DATA(data, 4, VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0E_1A);		// 0x1E0E
	// TXPRM1EN: enable
	// TXPRM12: 0x10
	SET_BLOCK_DATA(data, 5, VBORX_TMDS_ANALOG_PARAM_TMDSTXP0_3_0F_90);		// 0x1E0F
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_tmds_analog_reset(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_TMDS_ANALOG_RESET_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_TMDS_ANALOG_RESET_NUM);
	// TP0SWPDWN: normal
	// TP0ENCSRST: reset
	// 0x1034
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TXSYSCFG1_ADDR,
						VBORX_TMDS_ANALOG_RESET_TXSYSCFG1_TP0SWPDWN_NOR
						| VBORX_TMDS_ANALOG_RESET_TXSYSCFG1_TP0ENCSRST_RST,
						VBORX_TMDS_ANALOG_RESET_TXSYSCFG1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_tmds_analog_setting(system_port port)
{
	uint32_t ret;

	// TMDS Tx analog init
	ret = vborx_tmds_analog_init(port);
	CHECK_ERROR(ret);
	// TMDS Tx analog parameter
	ret = vborx_tmds_analog_param(port);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// sleep 10 (at least)
	SLEEP(VBORX_SLEEP_TIME_10MSEC);
	// TMDS Tx analog encoder reset
	ret = vborx_tmds_analog_reset(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hdmitx_init(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_HDMITX_INIT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_HDMITX_INIT_NUM);
	// DEEPCON: normal
	// DEEPHASE: default phase = 1
	// DEEPCSRST: normal
	// DEEPC: 8bit
	// 0x3009
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TDPCMODE_00_ADDR,
						VBORX_HDMITX_INIT_TDPCMODE_00_DEEPCON_NORMAL
						| VBORX_HDMITX_INIT_TDPCMODE_00_DEEPHASE_DEF_PHASE_1
						| VBORX_HDMITX_INIT_TDPCMODE_00_DEEPCSRST_NORMAL
						| VBORX_HDMITX_INIT_TDPCMODE_00_DEEPC_8BIT,
						VBORX_HDMITX_INIT_TDPCMODE_00_MASK);
	// 0x10B0: DNCON: down scaler off
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM2_SWMODE4_00_ADDR,
						VBORX_HDMITX_INIT_SWMODE4_00_DNCON_OFF,
						VBORX_HDMITX_INIT_SWMODE4_00_MASK);
	// SCRMOD: HDMI2.0 standard scramble mode
	// SCRVER: HDMI2.0 standard scramble version
	// SCRON: scramble off
	// 0x10C7
	SET_COMMAND(cmd, 2, VIDEOIF_SYSTEM2_TXENCSET4_02_ADDR,
						VBORX_HDMITX_INIT_TXENCSET4_02_SCRMOD_HDMI2
						| VBORX_HDMITX_INIT_TXENCSET4_02_SCRVER_HDMI2
						| VBORX_HDMITX_INIT_TXENCSET4_02_SCRON_OFF,
						VBORX_HDMITX_INIT_TXENCSET4_02_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hdmitx_link(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_HDMITX_LINK_MAX_NUM];
	system_block_access baccess;
	uint8_t data[VBORX_HDMITX_LINK_MAX_LEN];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_HDMITX_LINK_NUM1);
	// 0x3ED0: UPDHSC: 8 times
	SET_COMMAND(cmd, 0, VIDEOIF_DEBUG_CFGDBG_00_ADDR,
						VBORX_HDMITX_LINK_CFGDBG_00_UPDHSC_8TIMES,
						VBORX_HDMITX_LINK_CFGDBG_00_MASK);
	// ICFMT: RGB full range
	// OCFMT: RGB full range
	// 0x2272
	SET_COMMAND(cmd, 1, VIDEOIF_CC_CSCMOD3_ADDR,
						VBORX_HDMITX_LINK_CSCMOD3_ICFMT_RGB_FR
						| VBORX_HDMITX_LINK_CSCMOD3_OCFMT_RGB_FR,
						VBORX_HDMITX_LINK_CSCMOD3_MASK);
	// 0x3005: HDMI: HDMI
	SET_COMMAND(cmd, 2, VIDEOIF_SYSTEM_TSYSCTRL_ADDR,
						VBORX_HDMITX_LINK_TSYSCTRL_HDMI_HDMI,
						VBORX_HDMITX_LINK_TSYSCTRL_MASK);
	// 0x305F: CCVMUTE: normal
	SET_COMMAND(cmd, 3, VIDEOIF_VIDEO_VMUTECNT_ADDR,
						VBORX_HDMITX_LINK_VMUTECNT_CCVMUTE_NORMAL,
						VBORX_HDMITX_LINK_VMUTECNT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CSC_CFMT_ADDR, VBORX_HDMITX_LINK_LEN1, data);
	// ICFMT: RGB full range
	// OCFMT: RGB full range
	SET_BLOCK_DATA(data, 0, VBORX_HDMITX_LINK_CFMT_ICFMT_RGB_FR
							| VBORX_HDMITX_LINK_CFMT_OCFMT_RGB_FR);		// 0x30C2
	// OUTBW: 2tap
	SET_BLOCK_DATA(data, 1, VBORX_HDMITX_LINK_OUTWSET_OUTBW_2TAP);		// 0x30C3
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_VIDEOFMT_00_ADDR, VBORX_HDMITX_LINK_LEN2, data);
	// VIFMT: 0
	SET_BLOCK_DATA(data, 0, VBORX_HDMITX_LINK_VIDEOFMT_00_00);			// 0x3070
	// VOFMTSEL: use VOFMT
	// VOFMT: 0
	SET_BLOCK_DATA(data, 1, VBORX_HDMITX_LINK_VIDEOFMT_01_80);			// 0x3071
	// VF_MODE: mode 1
	// HDMIFMT: 4K2K
	SET_BLOCK_DATA(data, 2, VBORX_HDMITX_LINK_HDMIFMT_01);				// 0x3072
	// default (padding)
	SET_BLOCK_DATA(data, 3, VBORX_HDMITX_LINK_3DFMT_00);				// 0x3073
	// HDMI_VIC: 4K2K 59.94/60Hz (3840x2160)
	SET_BLOCK_DATA(data, 4, VBORX_HDMITX_LINK_HDMIVIC_07);				// 0x3074
	// default (padding)
	SET_BLOCK_DATA(data, 5, VBORX_HDMITX_LINK_PCFMT_00);				// 0x3075
	// UPCBPS: not use UPC
	// VCIBPS: not use VCL
	// SCLKSEL: normal
	SET_BLOCK_DATA(data, 6, VBORX_HDMITX_LINK_VSELECT_00_C0);			// 0x3076
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, VBORX_HDMITX_LINK_NUM2);
	// 0x3054: INREP: add repetition
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_INREPSET_ADDR,
						VBORX_HDMITX_LINK_INREPSET_INREP_REPETITION_ADDED,
						VBORX_HDMITX_LINK_INREPSET_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hdmitx_link_update(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_HDMITX_LINK_UPDATE_NUM];
	uint32_t ret;

	CHECK_ABORT();

	// all update
	SET_ACCESS(access, &port, cmd, VBORX_HDMITX_LINK_UPDATE_NUM);
	// ALLUPD: update
	// AVIIUPD: update
	// AUDIUPD: update
	// CLKUPD: update
	// HVSIUPD: update
	// VIFUPD: update
	// AUDUPD: update
	// CSCUPD: update
	// 0x3011
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_UPDCTRL_ADDR,
						VBORX_HDMITX_LINK_UPDATE_UPDCTRL_ALL_UPDATE,
						VBORX_HDMITX_LINK_UPDATE_UPDCTRL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	// check all cleared
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_UPDCTRL_ADDR,
						VBORX_HDMITX_LINK_UPDATE_UPDCTRL_ALL_CLEARED,
						VBORX_HDMITX_LINK_UPDATE_UPDCTRL_MASK);
	ret = device_read_wait(&access);			// read_wait
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hdmitx_link_tsrst(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_HDMITX_LINK_TSRST_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_HDMITX_LINK_TSRST_NUM);
	// VIFSRST: reset
	// AUDSRST: reset
	// UPCSRST: normal
	// HDCPRST: normal
	// CCSRST: reset
	// FIFOSRST: normal
	// ENCSRST: reset
	// AVCSRST: normal
	// 0x3006
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TSRST_ADDR,
						VBORX_HDMITX_LINK_TSRST_TSRST_CA,
						VBORX_HDMITX_LINK_TSRST_TSRST_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hdmitx_link_setting(system_port port)
{
	uint32_t ret;

	// HDMI Tx init
	ret = vborx_hdmitx_init(port);
	CHECK_ERROR(ret);
	// HDMI Tx link setting
	ret = vborx_hdmitx_link(port);
	CHECK_ERROR(ret);
	// HDMI Tx link update
	ret = vborx_hdmitx_link_update(port);
	CHECK_ERROR(ret);
	// HDMI Tx link TSRST
	ret = vborx_hdmitx_link_tsrst(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_hdmitx_setting(system_port port)
{
	uint32_t ret;

	// set tx pll setting disable
	ret = vborx_set_txpll00en(port, VBORX_TXPLL00_01_TXPLL00EN_AUTO);
	CHECK_ERROR(ret);
	ret = vborx_set_tx0_sen(port, VBORX_CKSELEN_TX0_SEN_AUTO);
	CHECK_ERROR(ret);
	// set TMDS channel swap and polarity inversion
	ret = vborx_set_tmds_ch_polarity(port);
	CHECK_ERROR(ret);
	// TMDS Tx analog parameter setting
	ret = vborx_tmds_analog_setting(port);
	CHECK_ERROR(ret);
	// HDMI Tx link setting
	ret = vborx_hdmitx_link_setting(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_crc_check_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CRC_CHECK_SETTING_NUM];
	system_block_access baccess;
	uint8_t data[VBORX_CRC_CHECK_SETTING_LEN];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_CRC_CHECK_SETTING_NUM);
	// CRC_MODE: compare DE inside
	// CRC_ENA: enable
	// 0x3EE6
	SET_COMMAND(cmd, 0, VIDEOIF_DEBUG_ENCDBG_06_ADDR,
						VBORX_CRC_CHECK_SETTING_ENCDBG_06_CRC_MODE_IN
						| VBORX_CRC_CHECK_SETTING_ENCDBG_06_CRC_ENA_ENA,
						VBORX_CRC_CHECK_SETTING_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM2_TXENCSET2_00_ADDR,
						VBORX_CRC_CHECK_SETTING_LEN, data);
	// CRC expected value (CHA)
	SET_BLOCK_DATA(data, 0, VBORX_CRC_CHECK_SETTING_TXENCSET2_00_CHA_15_08);	// 0x10BB
	SET_BLOCK_DATA(data, 1, VBORX_CRC_CHECK_SETTING_TXENCSET2_01_CHA_07_00);	// 0x10BC
	// CRC expected value (CHB)
	SET_BLOCK_DATA(data, 2, VBORX_CRC_CHECK_SETTING_TXENCSET2_02_CHB_15_08);	// 0x10BD
	SET_BLOCK_DATA(data, 3, VBORX_CRC_CHECK_SETTING_TXENCSET2_03_CHB_07_00);	// 0x10BE
	// CRC expected value (CHC)
	SET_BLOCK_DATA(data, 4, VBORX_CRC_CHECK_SETTING_TXENCSET2_04_CHC_15_08);	// 0x10BF
	SET_BLOCK_DATA(data, 5, VBORX_CRC_CHECK_SETTING_TXENCSET2_05_CHC_07_00);	// 0x10C0
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_crc_check_setting_off(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CRC_CHECK_SETTING_OFF_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_CRC_CHECK_SETTING_OFF_NUM);
	// CRC_MODE: compare DE outside
	// CRC_ENA: disable
	// 0x3EE6
	SET_COMMAND(cmd, 0, VIDEOIF_DEBUG_ENCDBG_06_ADDR,
						VBORX_CRC_CHECK_SETTING_OFF_ENCDBG_06_CRC_MODE_OUT
						| VBORX_CRC_CHECK_SETTING_OFF_ENCDBG_06_CRC_ENA_DIS,
						VBORX_CRC_CHECK_SETTING_OFF_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static void vborx_wait_1frame(void)
{
	// need to wait more than one frame (4400x40) time (500usec)
	SLEEP(VBORX_SLEEP_TIME_1MSEC);
}
//------------------------------------------------------------------------------
static uint32_t vborx_crc_error_check(system_port port, vborx_crc_err *p_err)
{
	system_reg_access access;
	system_command cmd[VBORX_CRC_ERROR_CHECK_NUM];
	uint32_t ret;

	if (p_err == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, VBORX_CRC_ERROR_CHECK_NUM);
	// 0x3EFE
	SET_COMMAND(cmd, 0, VIDEOIF_DEBUG_CRCMON_06_ADDR,
						0,
						VBORX_CRC_ERROR_CHECK_CRCMON_06_CRC_ERROR_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_err = (vborx_crc_err)cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_crc_check_retry(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CRC_CHECK_RETRY_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_CRC_CHECK_RETRY_NUM);
	// EN_P_IREF_11: disable
	// ANA_EN_11: enable
	// MAIN_EN_11: normal
	// PHY1 RSTB: 0x0
	// 0x2E41
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY1_P1_ENTCNT1_1_ADDR,
						VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_EN_P_IREF_11_DIS
						| VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_ANA_EN_11_ENA
						| VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_MAIN_EN_11_NOR
						| VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_RSTB_RST_00,
						VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_MASK);
	// EN_P_IREF_11: disable
	// ANA_EN_11: enable
	// MAIN_EN_11: normal
	// PHY1 RSTB: 0x1
	// 0x2E41
	SET_COMMAND(cmd, 1, VIDEOIF_VBORX_PHY1_P1_ENTCNT1_1_ADDR,
						VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_EN_P_IREF_11_DIS
						| VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_ANA_EN_11_ENA
						| VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_MAIN_EN_11_NOR
						| VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_RSTB_RST_01,
						VBORX_CRC_CHECK_RETRY_P1_ENTCNT1_1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_npd3_setting(system_port port, vborx_npd3 npd3)
{
	system_reg_access access;
	system_command cmd[VBORX_NPD3_SETTING_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_NPD3_SETTING_NUM);
	// NPD_0: power on
	// NPD_1: power on
	// NPD_2: power on
	// NPD_3: power on/off
	// 0x2E44
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_PHY1_P1_ENTCNT2_0_ADDR,
						VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_0_PWR_ON
						| VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_1_PWR_ON
						| VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_2_PWR_ON
						| ((uint32_t)npd3 << VBORX_NPD3_SETTING_P1_ENTCNT2_0_NPD_3_BIT),
						VBORX_NPD3_SETTING_P1_ENTCNT2_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_clock_switch_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBORX_CLOCK_SWITCH_SETTING_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBORX_CLOCK_SWITCH_SETTING_NUM);
	// CDRCLK03: use lane0 clock
	// CDRCLK47: use lane7 clock
	// CDRCLK03_345: lane0-3: 4byte mode
	// CDRCLK47_345: lane4-7: 4byte mode
	// 0x1E3C
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_VBORX_CLOCK_0_00_ADDR,
						VBORX_CLOCK_SWITCH_SETTING_CDRCLK03_LANE0
						| VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_LANE7
						| VBORX_CLOCK_SWITCH_SETTING_CDRCLK03_345_4BYTE
						| VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_345_4BYTE,
						VBORX_CLOCK_SWITCH_SETTING_VBORX_CLK_00_MASK);
	// CDRCLK03: use lane0 clock
	// CDRCLK47: use lane4 clock
	// CDRCLK03_345: lane0-3: 4byte mode
	// CDRCLK47_345: lane4-7: 4byte mode
	// 0x1E3C
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_VBORX_CLOCK_0_00_ADDR,
						VBORX_CLOCK_SWITCH_SETTING_CDRCLK03_LANE0
						| VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_LANE4
						| VBORX_CLOCK_SWITCH_SETTING_CDRCLK03_345_4BYTE
						| VBORX_CLOCK_SWITCH_SETTING_CDRCLK47_345_4BYTE,
						VBORX_CLOCK_SWITCH_SETTING_VBORX_CLK_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_boot_seq_pushd(system_port port, uint8_t *p_data)
{
	system_reg_access access;
	system_command cmd[VBORX_BOOT_SEQ_PUSHD_NUM];
	int32_t i;
	uint32_t ret;

	if (p_data == NULL) {
		return ERR_NULL_POINTER;
	}

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_BOOT_SEQ_PUSHD_NUM);
	// 
	SET_COMMAND(cmd, 0, VIDEOIF_CC_CSCMOD3_ADDR,              0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x2272
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM_TXSYSCFG1_ADDR,        0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x1034
	SET_COMMAND(cmd, 2, VIDEOIF_TMDS_TMDSTXP0_1_ADDR,         0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x1E00
	SET_COMMAND(cmd, 3, VIDEOIF_TMDS_TMDSTXP0_2_00_ADDR,      0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x1E02
	SET_COMMAND(cmd, 4, VIDEOIF_VIDEO_VMUTECNT_ADDR,          0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x305F
	SET_COMMAND(cmd, 5, VIDEOIF_VBORX_LINK0_SELCTRL9_0_ADDR,  0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x2C74
	SET_COMMAND(cmd, 6, VIDEOIF_VBORX_LINK0_SELCTRLA_0_ADDR,  0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x2C78
	SET_COMMAND(cmd, 7, VIDEOIF_VBORX_GLUE_GLUECTL0_2_ADDR,   0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x2E82
	SET_COMMAND(cmd, 8, VIDEOIF_VBORX_GLUE_GLUECTL0_3_ADDR,   0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x2E83
	SET_COMMAND(cmd, 9, VIDEOIF_VBORX_PHY1_P1_ENTCNT1_1_ADDR, 0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x2E41
	SET_COMMAND(cmd, 10, VIDEOIF_SYSTEM2_TXENCSET4_00_ADDR,   0, VBORX_BOOT_SEQ_PUSHD_MASK);	// 0x10C5
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	for (i=0; i<VBORX_BOOT_SEQ_PUSHD_NUM; i++) {
		p_data[i] = cmd[i].data;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_boot_seq_popd(system_port port, uint8_t *p_data)
{
	system_reg_access access;
	system_command cmd[VBORX_BOOT_SEQ_POPD_NUM];
	uint32_t ret;

	if (p_data == NULL) {
		return ERR_NULL_POINTER;
	}

	// page: 0,1

	SET_ACCESS(access, &port, cmd, VBORX_BOOT_SEQ_POPD_NUM);
	// 
	SET_COMMAND(cmd, 0, VIDEOIF_CC_CSCMOD3_ADDR,              p_data[0], VBORX_BOOT_SEQ_POPD_MASK);	// 0x2272
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM_TXSYSCFG1_ADDR,        p_data[1], VBORX_BOOT_SEQ_POPD_MASK);	// 0x1034
	SET_COMMAND(cmd, 2, VIDEOIF_TMDS_TMDSTXP0_1_ADDR,         p_data[2], VBORX_BOOT_SEQ_POPD_MASK);	// 0x1E00
	SET_COMMAND(cmd, 3, VIDEOIF_TMDS_TMDSTXP0_2_00_ADDR,      p_data[3], VBORX_BOOT_SEQ_POPD_MASK);	// 0x1E02
	SET_COMMAND(cmd, 4, VIDEOIF_VIDEO_VMUTECNT_ADDR,          p_data[4], VBORX_BOOT_SEQ_POPD_MASK);	// 0x305F
	SET_COMMAND(cmd, 5, VIDEOIF_VBORX_LINK0_SELCTRL9_0_ADDR,  p_data[5], VBORX_BOOT_SEQ_POPD_MASK);	// 0x2C74
	SET_COMMAND(cmd, 6, VIDEOIF_VBORX_LINK0_SELCTRLA_0_ADDR,  p_data[6], VBORX_BOOT_SEQ_POPD_MASK);	// 0x2C78
	SET_COMMAND(cmd, 7, VIDEOIF_VBORX_GLUE_GLUECTL0_2_ADDR,   p_data[7], VBORX_BOOT_SEQ_POPD_MASK);	// 0x2E82
	SET_COMMAND(cmd, 8, VIDEOIF_VBORX_GLUE_GLUECTL0_3_ADDR,   p_data[8], VBORX_BOOT_SEQ_POPD_MASK);	// 0x2E83
	SET_COMMAND(cmd, 9, VIDEOIF_VBORX_PHY1_P1_ENTCNT1_1_ADDR, p_data[9], VBORX_BOOT_SEQ_POPD_MASK);	// 0x2E41
	SET_COMMAND(cmd, 10, VIDEOIF_SYSTEM2_TXENCSET4_00_ADDR,  p_data[10], VBORX_BOOT_SEQ_POPD_MASK);	// 0x10C5
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_boot_seq(system_port port)
{
	uint8_t backupd[VBORX_BOOT_SEQ_PUSHD_NUM] = {0};
	uint32_t i;
	vborx_crc_err err;
	uint32_t ret;

	// save backup data
	MES("Func: %s, Line: %d vborx_boot_seq_pushd(port, backupd); st", __func__, __LINE__);
	ret = vborx_boot_seq_pushd(port, backupd);
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	CHECK_ERROR(ret);
	// internal pg setting (2.4.1)
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	ret = vborx_internal_pg_on(port);
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	CHECK_ERROR(ret);
	// HDMI.Tx setting (2.4.2)
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	ret = vborx_hdmitx_setting(port);
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	CHECK_ERROR(ret);
	// CRC check setting (2.4.3)
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	ret = vborx_crc_check_setting(port);
	MES("Func: %s, Line: %d ", __func__, __LINE__);
	CHECK_ERROR(ret);

	for (i=0; i<VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX; i++) {
		CHECK_ABORT();
		// wait 1Frame (2.4.4)
		vborx_wait_1frame();
		// CRC error check (2.4.5)
		ret = vborx_crc_error_check(port, &err);
		CHECK_ERROR(ret);
		if (err == VBORX_CRC_ERROR) {
			MES("Func: %s, Line: %d err == VBORX_CRC_ERROR", __func__, __LINE__);
			break;
		} else {
			// CRC check retry (2.4.6)
			MES("Func: %s, Line: %d CRC check retry (2.4.6)", __func__, __LINE__);
			ret = vborx_crc_check_retry(port);
			CHECK_ERROR(ret);
		}
	}
	if (i == VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX) {
		MES("Func: %s, Line: %d i == VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX 1", __func__, __LINE__);
		return ERR_VBORX_BOOT_SEQ_RETRY;
	}
	// NPD3 OFF (2.4.7)
	ret = vborx_npd3_setting(port, VBORX_NPD3_OFF);
	CHECK_ERROR(ret);

	for (i=0; i<VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX; i++) {
		CHECK_ABORT();
		// clock switch setting (2.4.8)
		ret = vborx_clock_switch_setting(port);
		CHECK_ERROR(ret);
		// wait 1Frame (2.4.9)
		vborx_wait_1frame();
		// CRC error check (2.4.10)
		ret = vborx_crc_error_check(port, &err);
		CHECK_ERROR(ret);
		if (err == VBORX_CRC_ERROR) {
			;
		} else {
			break;
		}
	}
	if (i == VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX) {
		MES("Func: %s, Line: %d i == VBORX_BOOT_SEQ_CRC_CHECK_RETRY_MAX 2", __func__, __LINE__);
		return ERR_VBORX_BOOT_SEQ_RETRY;
	}

	// NPD3 ON (2.4.11)
	ret = vborx_npd3_setting(port, VBORX_NPD3_ON);
	CHECK_ERROR(ret);
	// internal PG setting off (2.4.12)
	ret = vborx_internal_pg_off(port);
	CHECK_ERROR(ret);
	// CRC check setting off (2.4.13)
	ret = vborx_crc_check_setting_off(port);
	CHECK_ERROR(ret);
	// set RX CSC (2.4.14)
	// restore backup data
	ret = vborx_boot_seq_popd(port, backupd);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_ctrl_req_abort(system_request *p_req, vborx_ctrl_req ctrl)
{
	static system_request vborx_abort_req = SYSTEM_REQUEST_NONE;

	if (p_req == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == VBORX_CTRL_REQ_GET) {
		*p_req =vborx_abort_req ;
	} else if (ctrl == VBORX_CTRL_REQ_SET) {
		vborx_abort_req = *p_req;
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static system_request vborx_ctrl_get_abort_req(void)
{
	system_request req;

	if (vborx_ctrl_req_abort(&req, VBORX_CTRL_REQ_GET)) {
		return SYSTEM_REQUEST_NONE;
	}

	return req;
}
//------------------------------------------------------------------------------
static void vborx_ctrl_set_abort_req(system_request req)
{
	if (vborx_ctrl_req_abort(&req, VBORX_CTRL_REQ_SET)) {
		return;
	}
}
//------------------------------------------------------------------------------
static uint32_t vborx_ctrl_retry_cdr_lock(system_config_retry *p_retry, vborx_ctrl_retry ctrl)
{
	static system_config_retry vborx_config_retry_cdr_lock = {
		SYSTEM_UPDATE_DISABLE,
		VBORX_CONFIG_RETRY_CDR_LOCK_TIMES_DEFAULT,
		VBORX_CONFIG_RETRY_CDR_LOCK_INTERVAL_DEFAULT
	};

	if (p_retry == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == VBORX_CTRL_RETRY_GET) {
		p_retry->times = vborx_config_retry_cdr_lock.times;
		p_retry->interval = vborx_config_retry_cdr_lock.interval;
	} else if (ctrl == VBORX_CTRL_RETRY_SET) {
		if (p_retry->update == SYSTEM_UPDATE_ENABLE) {
			vborx_config_retry_cdr_lock.update = SYSTEM_UPDATE_ENABLE;
			vborx_config_retry_cdr_lock.times = p_retry->times;
			vborx_config_retry_cdr_lock.interval = p_retry->interval;
		}
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
uint32_t vborx_configure(vborx_config *p_conf)
{
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// set retry info for CDR lock detection
	ret = vborx_ctrl_retry_cdr_lock(&(p_conf->cdr_lock_detect), VBORX_CTRL_RETRY_SET);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vborx_init(system_config *p_conf)
{
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// system control initialization
	ret = system_ctrl_init(p_conf->port);
	CHECK_ERROR(ret);
	// page: 0,1
	ret = vborx_select_page(p_conf->port, VBORX_PAGE_BOTH);
	CHECK_ERROR(ret);
	// initial setting
	ret = vborx_init_setting(p_conf->port);
	CHECK_ERROR(ret);
	// select of VBO-Rx path (2.1.1)
	ret = vborx_path_setting(p_conf->port);
	CHECK_ERROR(ret);
	// H/Vsync polarity setting
	ret = vborx_hvsync_polarity_setting(p_conf->port, p_conf->hvsync_polarity);
	CHECK_ERROR(ret);
	// init abort request
	vborx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vborx_exit(system_port port)
{
	uint32_t ret;

	// negate CDR lock and hot plug
	ret = vborx_cdrlock_hotplug_negate(port);
	CHECK_ERROR(ret);
	// system control exit
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vborx_set_video_format(vborx_video *p_video)
{
	system_port port;
	system_ctrl_vbo_lane cnt;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;

	// check vborx_init()
	ret = system_ctrl_check_vborx_init(port);
	CHECK_ERROR(ret);
	MES("vborx_set_video_format system_ctrl_check_vborx_init");
	// page: 0,1
	ret = vborx_select_page(port, VBORX_PAGE_BOTH);
	CHECK_ERROR(ret);
	// negate CDR lock and hot plug
	ret = vborx_cdrlock_hotplug_negate(port);
	CHECK_ERROR(ret);
	// get vbo lane count
	ret =  system_ctrl_get_vbo_lane_count(p_video->rx_video_format, &cnt);
	CHECK_ERROR(ret);
	// clock setting in initialization (2.1.2)
	ret = vborx_clock_setting(port, cnt);
	CHECK_ERROR(ret);
	// PHY parameter setting in initialization (2.1.3)
	ret = vborx_phy_param_setting(port);
	CHECK_ERROR(ret);
	// pixel clock PLL setting in initialization (2.1.4)
	ret = vborx_pixel_pll_setting(port, cnt);
	CHECK_ERROR(ret);
	// LINK setting in initialization 1 (2.1.5)
	ret = vborx_link_setting1(port, p_video->rx_video_format, cnt);
	CHECK_ERROR(ret);
	// PHY setting in initialization 1 (2.1.6)
	ret = vborx_phy_setting1(port);
	CHECK_ERROR(ret);
	// LINK setting in initialization 2 (2.1.7)
	ret = vborx_link_setting2(port, cnt);
	CHECK_ERROR(ret);
	// PHY setting in initialization 2 (2.1.8)
	ret = vborx_phy_setting2(port);
	CHECK_ERROR(ret);
	// hot plug asserting (2.2)
	ret = vborx_hotplug_assert(port);
	CHECK_ERROR(ret);
	MES("vborx_set_video_format vborx_hotplug_assert");
	// CDR lock sequence (2.3)
	MES("vborx_set_video_format vborx_cdrlock_seq start");
	ret = vborx_cdrlock_seq(port, cnt);
	MES("vborx_set_video_format vborx_cdrlock_seq end");
	CHECK_ERROR(ret);
	if (cnt == SYSTEM_CTRL_VBO_LANE_8) {
		// boot sequence (8lane mode only) (2.4)
		MES("vborx_set_video_format boot sequence (8lane mode only)");
		ret = vborx_boot_seq(port);
		MES("Func: %s, Line: %d vborx_boot_seq(port); end", __func__, __LINE__);
		CHECK_ERROR(ret);
	}
	// CDR lock asserting (2.5)
	MES("Func: %s, Line: %d CDR lock asserting (2.5)", __func__, __LINE__);
	ret = vborx_cdrlock_assert(port);
	CHECK_ERROR(ret);
	// link up & normal mode (2.6)
	MES("Func: %s, Line: %d link up & normal mode (2.6)", __func__, __LINE__);
	ret = vborx_linkup(port, cnt);
	MES("vborx_set_video_format vborx_linkup");
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// set video color format
	ret = system_ctrl_set_color_format(p_video);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vborx_abort(void)
{
	// abort request
	vborx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

	return ERR_OK;
}
//------------------------------------------------------------------------------
