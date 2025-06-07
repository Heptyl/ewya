/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "vbotx.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "hv_version.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// vbotx_select_page()
#define VBOTX_PAGE_SELECT_NUM								1
#define VBOTX_PAGE_SELECT_MASK								0x000000FF
//------------------------------------------------------------------------------
// vbotx_pll_fraction_freq_dividing_setting()
#define VBOTX_PLL_SSC_FFD_SETTING_LEN						4
#define VBOTX_PLL_SSCCNT1_0_MIN_0X1B						0x1B
#define VBOTX_PLL_SSCCNT1_1_KIN_0_0X00						0x00
#define VBOTX_PLL_SSCCNT1_2_KIN_1_0X00						0x00
#define VBOTX_PLL_SSCCNT1_3_KIN_2_0X08						0x08
//------------------------------------------------------------------------------
// vbotx_pll_delta_sigma_mode_setting()
#define VBOTX_PLL_SSC_DSIGMODE_SETTING_NUM					1
#define VBOTX_PLL_SSCCNT2_3_DSIGMODE_2ND					0x00
#define VBOTX_PLL_SSCCNT2_3_DSIGMODE_MASK					0x00000010
//------------------------------------------------------------------------------
// vbotx_pll_analog_ctrl_setting()
#define VBOTX_PLL_ANALOG_CTRL_LEN							4
#define VBOTX_PLL_PLLANACNT_0_REG1_255						0x0D
#define VBOTX_PLL_PLLANACNT_0_REG2_IREFX14					0xE0
#define VBOTX_PLL_PLLANACNT_1_REG2_IREFX14					0x00
#define VBOTX_PLL_PLLANACNT_1_REG3_FR3						0x30
#define VBOTX_PLL_PLLANACNT_2_REG4_FR3						0x03
#define VBOTX_PLL_PLLANACNT_2_VCOLMT_0B100					0x40
#define VBOTX_PLL_PLLANACNT_3_DIVL_NORMAL					0x00
#define VBOTX_PLL_PLLANACNT_3_CP2EN_DISABLE_HALVING			0x00
#define VBOTX_PLL_PLLANACNT_3_CP2EN2_1CHARGE_PUMP			0x00
//------------------------------------------------------------------------------
// vbotx_pll_digital_ctrl_setting()
#define VBOTX_PLL_DIGITAL_CTRL_LEN							2
#define VBOTX_PLL_PLLDIGCNT_0_FIDIV_DR1						0x00
#define VBOTX_PLL_PLLDIGCNT_0_PDIV_DR4						0x10
#define VBOTX_PLL_PLLDIGCNT_1_QDIV_DR1						0x00
//------------------------------------------------------------------------------
// vbotx_pll_reset_release_of_delta_sigma_circuit()
#define VBOTX_PLL_SSC_RESET_CTRL_DELTA_SIGMA_NUM			1
#define VBOTX_PLL_SSCCNT2_3_NRSTDS_RESET_RELEASE			0x40
#define VBOTX_PLL_SSCCNT2_3_NRSTDS_MASK						0x00000040
#define VBOTX_SLEEP_TIME_20MSEC								20
//------------------------------------------------------------------------------
// vbotx_phy_clock_provision_to_lane_setting()
#define VBOTX_PHY_CLOCK_PROVISION_NUM						1
#define VBOTX_PHY_PHYRSV_RSV_LANE8_15						0x0C
#define VBOTX_PHY_PHYRSV_RSV_MASK							0x0000000C
//------------------------------------------------------------------------------
// vbotx_phy_driver_output_amplitude_setting()
#define VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN				12
#define VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_VBO_300			0x55
#define VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_VBO_500			0xDD
//------------------------------------------------------------------------------
// vbotx_phy_driver_output_common_mode_voltage_alignment_setting()
#define VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM		1
#define VBOTX_PHY_DRVCNT_1_NVCOM080_1						0x40
#define VBOTX_PHY_DRVCNT_1_NVCOM120_1						0x20
#define VBOTX_PHY_DRVCNT_1_NVCOM125_0						0x00
#define VBOTX_PHY_DRVCNT_1_NPORTEN_0						0x00
#define VBOTX_PHY_DRVCNT_1_LVDSEN_0							0x00
#define VBOTX_PHY_DRVCNT_1_MASK								0x0000007C
//------------------------------------------------------------------------------
// vbotx_phy_dc_test_mode_setting()
#define VBOTX_PHY_DC_TEST_MODE_NUM							1
#define VBOTX_PHY_DRVCNT_3_DRVMEN_BIT						4
#define VBOTX_PHY_DRVCNT_3_DRVMEN_MASK						0x00000010
//------------------------------------------------------------------------------
// vbotx_phy_vco_low_speed_oscillation_setting()
#define VBOTX_PHY_VCO_LOW_SPEED_OSCILLATION_NUM				1
#define VBOTX_PHY_PLLDIGCNT_1_OFFSETON_BIT					4
#define VBOTX_PHY_PLLDIGCNT_1_OFFSETON_MASK					0x00000010
//------------------------------------------------------------------------------
// vbotx_phy_macro_ctrl_setting()
#define VBOTX_PHY_MACRO_CTRL_NUM							1
#define VBOTX_PHY_ENTCNT_3_NRESET_BIT						5
#define VBOTX_PHY_MACRO_CTRL_ENTCNT_3_NPDOWN_ON				0x10
#define VBOTX_PHY_ENTCNT_3_MASK								0x000000FF
#define VBOTX_SLEEP_TIME_1MSEC								1
//------------------------------------------------------------------------------
// vbotx_phy_lane_enable_ctrl_setting()
#define VBOTX_PHY_LANE_ENABLE_CTRL_LEN						3
#define VBOTX_PHY_ENTCNT_0_LANE0_7_DISABLE					0x00
#define VBOTX_PHY_ENTCNT_1_LANE8_15_ENABLE					0xFF
#define VBOTX_PHY_ENTCNT_2_LANE16_23_DISABLE				0x00
//------------------------------------------------------------------------------
// vbotx_path_setting()
#define VBOTX_PATH_NUM										1
#define VBOTX_PATH_SWMODE4_01_LVBON_ENABLE					0x20
#define VBOTX_PATH_SWMODE4_01_LVDSEN_VBO					0x00
#define VBOTX_PATH_SWMODE4_01_MASK							0x60
//------------------------------------------------------------------------------
// vbotx_clock_setting()
#define VBOTX_CLOCK_NUM										3
#define VBOTX_CLOCK_RXCORE0_05_3_2							0x04
#define VBOTX_CLOCK_RXCORE0_05_5_4							0x20
#define VBOTX_CLOCK_RXCORE0_05_7_6							0x00
#define VBOTX_CLOCK_RXCORE0_05_MASK							0x000000FC
#define VBOTX_CKSELEN_TX0_SEN_AUTO							0x00
#define VBOTX_CKSELEN_TX0_SEN_MANUAL						0x04
#define VBOTX_CKSELEN_TX0_SEN_MASK							0x00000004
#define VBOTX_CLOCK_TXCORE0_05_5_4							0x10
#define VBOTX_CLOCK_TXCORE0_05_7_6_150MHZ_OR_LESS			0x00
#define VBOTX_CLOCK_TXCORE0_05_7_6_150MHZ_300MHZ			0x40
#define VBOTX_CLOCK_TXCORE0_05_7_6_OVER_300MHZ				0x80
#define VBOTX_CLOCK_TXCORE0_05_7_4_MASK						0x000000F0
//------------------------------------------------------------------------------
// vbotx_hot_plug_detect()
#define VBOTX_HPD_NUM										1
#define VBOTX_HPD_VBOCTL16_RDVBO_HTPDN_HPD					0x00
#define VBOTX_HPD_VBOCTL16_RDVBO_HTPDN_MASK					0x00000001
//------------------------------------------------------------------------------
// vbotx_glue_setting()
#define VBOTX_GLUE_LEN										4
#define VBOTX_GLUE_NUM										2
#define VBOTX_GLUE_GLUECTL0_CKMODE_150MHZ_OR_LESS			0x00
#define VBOTX_GLUE_GLUECTL0_CKMODE_150MHZ_300MHZ			0x01
#define VBOTX_GLUE_GLUECTL0_CKMODE_OVER_300MHZ				0x02
#define VBOTX_GLUE_GLUECTL1_LNMODE_150MHZ_OR_LESS			0x00
#define VBOTX_GLUE_GLUECTL1_LNMODE_150MHZ_300MHZ			0x01
#define VBOTX_GLUE_GLUECTL1_LNMODE_OVER_300MHZ				0x02
#define VBOTX_GLUE_GLUECTL1_1_SDMODE_DE_HALF				0x00
#define VBOTX_GLUE_GLUECTL1_1_SDMODE_DE_SAME				0x04
#define VBOTX_GLUE_GLUECTL1_1_SDMODE_REG_SETUP				0x08
#define VBOTX_GLUE_GLUECTL1_1_LINE_DLY_0					0x00
#define VBOTX_GLUE_GLUECTL1_1_LINE_DLY_1					0x01
#define VBOTX_GLUE_GLUECTL1_1_SDMODE_MASK					0x0000000C
#define VBOTX_GLUE_GLUECTL1_1_LINE_DLY_MASK					0x00000003
#define VBOTX_GLUE_GLUECTL8_SY_DLY_DEFAULT					0x00
#define VBOTX_GLUE_GLUECTL8_SY_DLY_08						0x08
#define VBOTX_GLUE_GLUECTL8_SY_DLY_MASK						0x000000FF
#define VBOTX_GLUE_GLUECTLX_BKMODE_DIV_LINE_INTO_2BANK		0x04
#define VBOTX_GLUE_GLUECTLX_BKMODE_NOT_USE_BANK_SWITCH		0x00
//------------------------------------------------------------------------------
// vbotx_lvds_link_double_phase_output_setting()
#define VBOTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN				4
#define VBOTX_LVDS_LINK_LVDSCONFCTL1_ODEVPASS_X_DOUBLE		0x00
//------------------------------------------------------------------------------
// vbotx_lvds_link_vbo_output_setting()
#define VBOTX_LVDS_LINK_VBO_OUTPUT_LEN						5
#define VBOTX_LVDS_LINK_VBOOUTCTL_VBOTXENB_2LANE			0x03
#define VBOTX_LVDS_LINK_VBOOUTCTL_VBOTXENB_4LANE			0x0F
#define VBOTX_LVDS_LINK_VBOOUTCTL_VBOTXENB_8LANE			0xFF
#define VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOASMODE_X_00		0x00
#define VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOBITMODE_X_08BIT	0x00
#define VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOBITMODE_X_10BIT	0x10
#define VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOPIXSWAP_X_OFF		0x00
//------------------------------------------------------------------------------
// vbotx_lvds_link_lvds_output_disable_setting()
#define VBOTX_LVDS_LINK_LVDS_OUTPUT_DISABLE_NUM				1
#define VBOTX_LVDS_LINK_LVDSOUTCTL1_ALL_DISABLE				0x00
#define VBOTX_LVDS_LINK_LVDSOUTCTL1_MASK					0x000000FF
//------------------------------------------------------------------------------
// vbotx_lvds_link_lvds_output_lane_setting()
#define VBOTX_LVDS_LINK_LVDS_OUTPUT_LANE_LEN				2
#define VBOTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T4LSEL_0X0			0x00
#define VBOTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T3LSEL_0X0			0x00
#define VBOTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T2LSEL_0X0			0x00
#define VBOTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T1LSEL_0X0			0x00
#define VBOTX_LVDS_LINK_LVDSOUTCTL4_LVDSMODE_SEL_DEFAULT	0x00
//------------------------------------------------------------------------------
// vbotx_lvds_link_lvds_pinsel_setting()
#define VBOTX_LVDS_LINK_LVDS_PINSEL_NUM						4
#define VBOTX_LVDS_LINK_LVDSPINASSIGN1_LVASMODE_1_VESA8		0x00
#define VBOTX_LVDS_LINK_LVDSPINASSIGN2_LVASMODE_2_VESA8		0x00
#define VBOTX_LVDS_LINK_LVDSPINASSIGN3_LVASMODE_3_VESA8		0x00
#define VBOTX_LVDS_LINK_LVDSPINASSIGN4_LVASMODE_4_VESA8		0x00
#define VBOTX_LVDS_LINK_LVDSPINASSIGNX_LVASMODE_X_MASK		0x00000007
//------------------------------------------------------------------------------
// vbotx_set_glue_polarity()
#define VBOTX_SET_GLUE_POLARITY_NUM							1
#define VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_P					0x00
#define VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_N					0x80
#define VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_P					0x00
#define VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_N					0x40
#define VBOTX_LVDVB_GLUE_GLUECTL3_XS_POL_MASK				0x000000C0
//------------------------------------------------------------------------------
// vbotx_set_link_polarity()
#define VBOTX_SET_LINK_POLARITY_NUM							1
#define VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_LOW					0x00
#define VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_HIGH					0x01
#define VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_LOW					0x00
#define VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_HIGH					0x02
#define VBOTX_VBO_LINK_VBOCTL2_VBOVPOL_LOW					0x00
#define VBOTX_VBO_LINK_VBOCTL2_VBOVPOL_HIGH					0x04
#define VBOTX_VBO_LINK_VBOCTL2_VBOHPOL_LOW					0x00
#define VBOTX_VBO_LINK_VBOCTL2_VBOHPOL_HIGH					0x08
#define VBOTX_VBO_LINK_VBOCTL2_VIOXPOL_MASK					0x00000003
#define VBOTX_VBO_LINK_VBOCTL2_VBOXPOL_MASK					0x0000000C
#define VBOTX_VBO_LINK_VBOCTL2_MASK							0x000000FF
//------------------------------------------------------------------------------
#define VBOTX_VBO_LINK_LANE_ASSIGN_LEN						4
#define VBOTX_VBOCTL23_LANE0SEL_LANE0						0x00
#define VBOTX_VBOCTL23_LANE1SEL_LANE1						0x10
#define VBOTX_VBOCTL23_LANE1SEL_LANE2						0x20
#define VBOTX_VBOCTL24_LANE2SEL_LANE4						0x04
#define VBOTX_VBOCTL24_LANE3SEL_LANE6						0x60
#define VBOTX_VBOCTL24_LANE2SEL_LANE1						0x01
#define VBOTX_VBOCTL24_LANE3SEL_LANE3						0x30
#define VBOTX_VBOCTL25_LANE4SEL_LANE1						0x01
#define VBOTX_VBOCTL25_LANE5SEL_LANE3						0x30
#define VBOTX_VBOCTL26_LANE6SEL_LANE5						0x05
#define VBOTX_VBOCTL26_LANE7SEL_LANE7						0x70
//------------------------------------------------------------------------------
// vbotx_vbo_link_link_data_path_setting()
#define VBOTX_VBO_LINK_LINK_DATA_PATH_NUM					1
#define VBOTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_VBO				0x01
#define VBOTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_MASK				0x000000FF
//------------------------------------------------------------------------------
//vbotx_vbo_link_output_Inverting_Data_setting()
#define VBOTX_VBO_LINK_OUTPUT_INVERTING_DATA_PATH_NUM		1
#define VBOTX_VBO_LINK_VBOCTL27_OUTPUT_INVERTING_VBO		0xff
#define VBOTX_VBO_LINK_VBOCTL22_OUTPUT_INVERTING_MASK		0x000000FF
//------------------------------------------------------------------------------
// vbotx_pll_lock_inform()
#define VBOTX_PLL_LOCK_INFORM_NUM							1
#define VBOTX_PLL_LOCK_VBOCTL0_PLLLOCKED_LOCK				0x01
#define VBOTX_PLL_LOCK_VBOCTL0_PLLLOCKED_MASK				0x000000FF
#define VBOTX_SLEEP_TIME_4MSEC								4
//------------------------------------------------------------------------------
// vbotx_pll_lock_detect()
#define VBOTX_PLD_NUM										1
#define VBOTX_PLD_VBOCTL16_RDVBO_LOCKN_PLD					0x00
#define VBOTX_PLD_VBOCTL16_RDVBO_LOCKN_MASK					0x00000002
//------------------------------------------------------------------------------
// vbotx_link_up_complete()
#define VBOTX_LUC_NUM										1
#define VBOTX_LUC_VBOCTL1_RDVBO_READY_LUC					0x01
#define VBOTX_LUC_VBOCTL1_RDVBO_READY_MASK					0x00000001
#define VBOTX_LUC_RETRY_MAX									300
#define VBOTX_LUC_RETRY_INTERVAL							10
//------------------------------------------------------------------------------
#define VBOTX_CONFIG_RETRY_HOT_PLUG_TIMES_DEFAULT			300
#define VBOTX_CONFIG_RETRY_HOT_PLUG_INTERVAL_DEFAULT		10
#define VBOTX_CONFIG_RETRY_PLL_LOCK_TIMES_DEFAULT			300
#define VBOTX_CONFIG_RETRY_PLL_LOCK_INTERVAL_DEFAULT		10
//------------------------------------------------------------------------------
// vbotx_phy_driver_output_EME_TMCOR_switch()
#define VBOTX_PHY_DRIVER_OUTPUT_EME_LEN						1
#define VBOTX_PHY_DRIVER_OUTPUT_EME							0X01
#define VBOTX_PHY_DRVCNT_2_MASK								0xF0
//------------------------------------------------------------------------------
// vbotx_phy_driver_output_emphasis_strength_control()
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_STRENGTH_LEN		4
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_STRENGTH_0			0x0
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_STRENGTH_0P5		0x1
#define VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength_1			0x2
#define VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength_1P5		0x3
#define VIDEOIF_VBO_PHY_DRVCNT_0_MASK						0x04
#define VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength_2P5		0x5
#define VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength			(VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength_2P5 << 4 | VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength_2P5)

//------------------------------------------------------------------------------
// vbotx_phy_driver_output_emphasis_period()
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_LEN			1
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_380PS		0X0
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_340PS		0X1
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_300PS		0X2
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_265PS		0X3
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_226PS		0X4
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_187PS		0X5
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_151PS		0X6
#define VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_114PS		0X7

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
typedef enum _vbotx_page {
	VBOTX_PAGE_LVDS_LINK = 0,
	VBOTX_PAGE_VBO_LINK,
	VBOTX_PAGE_LVDSVBO_PHY,
	VBOTX_PAGE_GLUE
} vbotx_page;
//------------------------------------------------------------------------------
typedef enum _vbotx_dc_test_mode {
	VBOTX_DC_TEST_MODE_OFF = 0,
	VBOTX_DC_TEST_MODE_ON
} vbotx_dc_test_mode;
//------------------------------------------------------------------------------
typedef enum _vbotx_vco_low_speed_oscillation {
	VBOTX_VCO_LOW_SPEED_OSCILLATION_OFF = 0,
	VBOTX_VCO_LOW_SPEED_OSCILLATION_ON
} vbotx_vco_low_speed_oscillation;
//------------------------------------------------------------------------------
typedef enum _vbotx_macro_reset {
	VBOTX_MACRO_RESET = 0,
	VBOTX_MACRO_RESET_RELEASE
} vbotx_macro_reset;
//------------------------------------------------------------------------------
typedef enum _vbotx_ctrl_retry {
	VBOTX_CTRL_RETRY_GET,
	VBOTX_CTRL_RETRY_SET,
	VBOTX_CTRL_RETRY_MAX
} vbotx_ctrl_retry;
//------------------------------------------------------------------------------
typedef enum _vbotx_ctrl_req {
	VBOTX_CTRL_REQ_GET,
	VBOTX_CTRL_REQ_SET,
	VBOTX_CTRL_REQ_MAX
} vbotx_ctrl_req;
//------------------------------------------------------------------------------
static uint32_t vbotx_select_page(system_port port, vbotx_page page);
//------------------------------------------------------------------------------
static uint32_t vbotx_init_setting(system_port port);
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_fraction_freq_dividing_setting(system_port port);
static uint32_t vbotx_pll_delta_sigma_mode_setting(system_port port);
static uint32_t vbotx_pll_analog_ctrl_setting(system_port port);
static uint32_t vbotx_pll_digital_ctrl_setting(system_port port);
static uint32_t vbotx_pll_reset_release_of_delta_sigma_circuit(system_port port);
static uint32_t vbotx_pll_setting(system_port port);
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_clock_provision_to_lane_setting(system_port port);
static uint32_t vbotx_phy_driver_output_amplitude_setting(system_port port);
static uint32_t vbotx_phy_driver_output_common_mode_voltage_alignment_setting(system_port port);
static uint32_t vbotx_phy_dc_test_mode_setting(system_port port, vbotx_dc_test_mode mode);
static uint32_t vbotx_phy_vco_low_speed_oscillation_setting(system_port port, vbotx_vco_low_speed_oscillation vco);
static uint32_t vbotx_phy_macro_ctrl_setting(system_port port, vbotx_macro_reset reset);
static uint32_t vbotx_phy_lane_enable_ctrl_setting(system_port port);
static uint32_t vbotx_phy_setting(system_port port);
//------------------------------------------------------------------------------
static uint32_t vbotx_path_setting(system_port port);
static uint32_t vbotx_clock_setting(system_port port, vbotx_video *p_video);
static uint32_t vbotx_hot_plug_detect(system_port port);
static uint32_t vbotx_glue_setting(system_port port, vbotx_video *p_video);
static uint32_t vbotx_hvpol_setting(system_port port, vbotx_video *p_video);
static uint32_t vbotx_lvds_link_double_phase_output_setting(system_port port);
static uint32_t vbotx_lvds_link_vbo_output_setting(system_port port, vbotx_video *p_video);
static uint32_t vbotx_lvds_link_lvds_output_disable_setting(system_port port);
static uint32_t vbotx_lvds_link_lvds_output_lane_setting(system_port port);
static uint32_t vbotx_lvds_link_lvds_pinsel_setting(system_port port);
static uint32_t vbotx_lvds_link_setting(system_port port);
static uint32_t vbotx_set_glue_polarity(system_port port, uint8_t pol);
static uint32_t vbotx_set_link_polarity(system_port port, uint8_t pol, uint8_t mask);
static uint32_t vbotx_vbo_link_hvsync_polarity_setting(system_port port, system_inversion hvsync_polarity);
static uint32_t vbotx_vbo_link_lane_assignment_setting(system_port port, vbotx_video *p_video);
static uint32_t vbotx_vbo_link_output_Inverting_Data_setting(system_port port);
static uint32_t vbotx_vbo_link_link_data_path_setting(system_port port);
static uint32_t vbotx_vbo_link_setting(system_port port, system_inversion hvsync_polarity);
static uint32_t vbotx_pll_lock_inform(system_port port);
static uint32_t vbotx_pll_lock_detect(system_port port);
static uint32_t vbotx_link_up_complete(system_port port);
static uint32_t vbotx_pll_lock_setting(system_port port);
static uint32_t vbotx_tx_link_setting(vbotx_video *p_video);
static uint32_t vbotx_ctrl_retry_hot_plug(system_config_retry *p_retry, vbotx_ctrl_retry ctrl);
static uint32_t vbotx_ctrl_retry_pll_lock(system_config_retry *p_retry, vbotx_ctrl_retry ctrl);
//------------------------------------------------------------------------------
static uint32_t vbotx_ctrl_req_abort(system_request *p_req, vbotx_ctrl_req ctrl);
static system_request vbotx_ctrl_get_abort_req(void);
static void vbotx_ctrl_set_abort_req(system_request req);

uint8_t vbotxdata[3] = {VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_300PS,
												VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_VBO_500,
												VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength};

//------------------------------------------------------------------------------
// abort request check
#define CHECK_ABORT() {												\
	if (vbotx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		vbotx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_VBOTX_ABORT;										\
	}																\
}
//------------------------------------------------------------------------------

extern int	panelDiv;



static uint32_t vbotx_select_page(system_port port, vbotx_page page)
{
	system_reg_access access;
	system_command cmd[VBOTX_PAGE_SELECT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBOTX_PAGE_SELECT_NUM);
	// 0x34FC
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSLINKPAGESEL_ADDR, page, VBOTX_PAGE_SELECT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}

uint32_t vbotx_en(system_port port, int onoff)
{
		system_reg_access access;
		system_command cmd[1];
		uint32_t ret;
		int onoff_vbo = 0;

		ret = vbotx_select_page(port, VBOTX_PAGE_LVDSVBO_PHY);
		CHECK_ERROR(ret);

		SET_ACCESS(access, &port, cmd, 1);
		if (onoff == 1) {
				onoff_vbo = 0xff;
		} else {
				onoff_vbo = 0x0;
		}

		SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_ENTCNT_1_ADDR,
												onoff_vbo,
												0xff);
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_init_setting(system_port port)
{
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_fraction_freq_dividing_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_PLL_SSC_FFD_SETTING_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_SSCCNT1_0_ADDR,
						VBOTX_PLL_SSC_FFD_SETTING_LEN, data);
	// 297MHz (4byte mode, 2.97Gbps/lane)
	// MIN
	SET_BLOCK_DATA(data, 0, VBOTX_PLL_SSCCNT1_0_MIN_0X1B);		// 0x34E0
	// KIN 0
	SET_BLOCK_DATA(data, 1, VBOTX_PLL_SSCCNT1_1_KIN_0_0X00);	// 0x34E1
	// KIN 1
	SET_BLOCK_DATA(data, 2, VBOTX_PLL_SSCCNT1_2_KIN_1_0X00);	// 0x34E2
	// KIN 2
	SET_BLOCK_DATA(data, 3, VBOTX_PLL_SSCCNT1_3_KIN_2_0X08);	// 0x34E3
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_delta_sigma_mode_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PLL_SSC_DSIGMODE_SETTING_NUM];
	uint32_t ret;

	// page: 2

	// 2nd order delta-sigma
	SET_ACCESS(access, &port, cmd, VBOTX_PLL_SSC_DSIGMODE_SETTING_NUM);
	// 0x34E7
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_SSCCNT2_3_ADDR,
						VBOTX_PLL_SSCCNT2_3_DSIGMODE_2ND,
						VBOTX_PLL_SSCCNT2_3_DSIGMODE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_analog_ctrl_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_PLL_ANALOG_CTRL_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_PLLANACNT_0_ADDR,
						VBOTX_PLL_ANALOG_CTRL_LEN, data);
	// REG1: 2.55GHz/V
	// REG2: Irefx 14/20
	SET_BLOCK_DATA(data, 0, VBOTX_PLL_PLLANACNT_0_REG1_255 |
								VBOTX_PLL_PLLANACNT_0_REG2_IREFX14);			// 0x34C8
	// REG2: Irefx 14/20
	// REG3: filter resistance: 3
	SET_BLOCK_DATA(data, 1, VBOTX_PLL_PLLANACNT_1_REG2_IREFX14 |
								VBOTX_PLL_PLLANACNT_1_REG3_FR3);				// 0x34C9
	// REG4: filter resistance: 3
	// VCOLMT: 0b100
	SET_BLOCK_DATA(data, 2, VBOTX_PLL_PLLANACNT_2_REG4_FR3 |
								VBOTX_PLL_PLLANACNT_2_VCOLMT_0B100);			// 0x34CA
	// DIVL: 0: normal state
	// CP2EN: 0: disable halving filter capacitance
	// CP2EN2: 0: 1 charge pump use
	SET_BLOCK_DATA(data, 3, VBOTX_PLL_PLLANACNT_3_DIVL_NORMAL						   |
								VBOTX_PLL_PLLANACNT_3_CP2EN_DISABLE_HALVING |
								VBOTX_PLL_PLLANACNT_3_CP2EN2_1CHARGE_PUMP);		// 0x34CB
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_digital_ctrl_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_PLL_DIGITAL_CTRL_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_PLLDIGCNT_0_ADDR,
						VBOTX_PLL_DIGITAL_CTRL_LEN, data);
	// FIDIV: division ratio: 1
	// PDIV: division ratio: 4
	SET_BLOCK_DATA(data, 0, VBOTX_PLL_PLLDIGCNT_0_FIDIV_DR1 |
								VBOTX_PLL_PLLDIGCNT_0_PDIV_DR4);	// 0x34CC
	// QDIV: division ratio: 1
	SET_BLOCK_DATA(data, 1, VBOTX_PLL_PLLDIGCNT_1_QDIV_DR1);		// 0x34CD
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_reset_release_of_delta_sigma_circuit(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PLL_SSC_RESET_CTRL_DELTA_SIGMA_NUM];
	uint32_t ret;

	// page: 2

	// reset release of delta sigma circuit
	SET_ACCESS(access, &port, cmd, VBOTX_PLL_SSC_RESET_CTRL_DELTA_SIGMA_NUM);
	// 0x34E7
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_SSCCNT2_3_ADDR,
						VBOTX_PLL_SSCCNT2_3_NRSTDS_RESET_RELEASE,
						VBOTX_PLL_SSCCNT2_3_NRSTDS_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SLEEP(VBOTX_SLEEP_TIME_20MSEC);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_setting(system_port port)
{
	uint32_t ret;

	// page: 2

	ret = vbotx_select_page(port, VBOTX_PAGE_LVDSVBO_PHY);
	CHECK_ERROR(ret);
	// fraction frequency dividing of PLL setting
	ret = vbotx_pll_fraction_freq_dividing_setting(port);
	CHECK_ERROR(ret);
	// delta-sigma mode setting
	ret = vbotx_pll_delta_sigma_mode_setting(port);
	CHECK_ERROR(ret);
	// PLL analog control setting
	ret = vbotx_pll_analog_ctrl_setting(port);
	CHECK_ERROR(ret);
	// PLL digital control setting
	ret = vbotx_pll_digital_ctrl_setting(port);
	CHECK_ERROR(ret);
	// reset release of delta-sigma circuit
	ret = vbotx_pll_reset_release_of_delta_sigma_circuit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_clock_provision_to_lane_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_CLOCK_PROVISION_NUM];
	uint32_t ret;

	// page: 2

	// clock provision: lane 8-15
	SET_ACCESS(access, &port, cmd, VBOTX_PHY_CLOCK_PROVISION_NUM);
	// 0x34D0
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_PHYRSV_ADDR,
						VBOTX_PHY_PHYRSV_RSV_LANE8_15,
						VBOTX_PHY_PHYRSV_RSV_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_driver_output_amplitude_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN];
	uint32_t ret;
	uint32_t i;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_DRVAMP1_0_ADDR,
						VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN, data);
	// driver output amplitude: 300 mVp-p (for VBO)
	for (i=0; i<VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN; i++) {
		SET_BLOCK_DATA(data, i, VBOTX_PHY_DRIVER_OUTPUT_AMPLITUDE_VBO_300);		// 0x3484-0x348F
	}
		if(vbotxdata[0] != 0xFF)
		{
				//0X74:0X88(Page:2)
				SET_BLOCK_DATA(data, 4, vbotxdata[1]);
				//0X74:0X89(Page:2)
				SET_BLOCK_DATA(data, 5, vbotxdata[1]);
				//0X74:0X8a(Page:2)
				SET_BLOCK_DATA(data, 6, vbotxdata[1]);
				//0X74:0X8b(Page:2)
				SET_BLOCK_DATA(data, 7, vbotxdata[1]);
		}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_driver_output_common_mode_voltage_alignment_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM];
	uint32_t ret;

	// page: 2

	SET_ACCESS(access, &port, cmd, VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM);
	// NVCOM080: 1
	// NVCOM120: 1
	// NVCOM125: 0
	// NPORTEN:  0
	// LVDSEN:   0
	// 0x34C1
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_1_ADDR,
						VBOTX_PHY_DRVCNT_1_NVCOM080_1		 |
							VBOTX_PHY_DRVCNT_1_NVCOM120_1 |
							VBOTX_PHY_DRVCNT_1_NVCOM125_0 |
							VBOTX_PHY_DRVCNT_1_NPORTEN_0  |
							VBOTX_PHY_DRVCNT_1_LVDSEN_0,
						VBOTX_PHY_DRVCNT_1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_dc_test_mode_setting(system_port port, vbotx_dc_test_mode mode)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_DC_TEST_MODE_NUM];
	uint32_t ret;

	// page: 2

	SET_ACCESS(access, &port, cmd, VBOTX_PHY_DC_TEST_MODE_NUM);
	// 0x34C3
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_3_ADDR,
						(uint8_t)mode << VBOTX_PHY_DRVCNT_3_DRVMEN_BIT,
						VBOTX_PHY_DRVCNT_3_DRVMEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_vco_low_speed_oscillation_setting(system_port port, vbotx_vco_low_speed_oscillation vco)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_VCO_LOW_SPEED_OSCILLATION_NUM];
	uint32_t ret;

	// page: 2
	
	SET_ACCESS(access, &port, cmd, VBOTX_PHY_VCO_LOW_SPEED_OSCILLATION_NUM);
	// 0x34CD
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_PLLDIGCNT_1_ADDR,
						(uint8_t)vco << VBOTX_PHY_PLLDIGCNT_1_OFFSETON_BIT,
						VBOTX_PHY_PLLDIGCNT_1_OFFSETON_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_macro_ctrl_setting(system_port port, vbotx_macro_reset reset)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_MACRO_CTRL_NUM];
	uint32_t ret;

	// page: 2

	// reset release
	// power on
	//   already done by system_ctrl_init
	SET_ACCESS(access, &port, cmd, VBOTX_PHY_MACRO_CTRL_NUM);
	// 0x3483
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_ENTCNT_3_ADDR,
						(uint8_t)reset << VBOTX_PHY_ENTCNT_3_NRESET_BIT |
							VBOTX_PHY_MACRO_CTRL_ENTCNT_3_NPDOWN_ON,
						VBOTX_PHY_ENTCNT_3_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SLEEP(VBOTX_SLEEP_TIME_1MSEC);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_lane_enable_ctrl_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_PHY_LANE_ENABLE_CTRL_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_ENTCNT_0_ADDR,
						VBOTX_PHY_LANE_ENABLE_CTRL_LEN, data);
	// lane 0-7: disable
	SET_BLOCK_DATA(data, 0, VBOTX_PHY_ENTCNT_0_LANE0_7_DISABLE);		// 0x3480
	// lane 8-15: enable
	SET_BLOCK_DATA(data, 1, VBOTX_PHY_ENTCNT_1_LANE8_15_ENABLE);		// 0x3481
	// lane 16-23: disable
	SET_BLOCK_DATA(data, 2, VBOTX_PHY_ENTCNT_2_LANE16_23_DISABLE);		// 0x3482
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_driver_output_EME_TMCOR_switch2(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM];
	uint32_t ret;
	// page: 2
	SET_ACCESS(access, &port, cmd, VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM);
	// EME:				0
	// 0x34C2
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_2_ADDR,
													VBOTX_PHY_DRIVER_OUTPUT_EME,
											0xFF);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}

static uint32_t vbotx_phy_driver_output_EME_TMCOR_switch(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM];
	uint32_t ret;
	// page: 2
	SET_ACCESS(access, &port, cmd, VBOTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM);
	// EME:				0
	// 0x34C2
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_2_ADDR,
													VBOTX_PHY_DRIVER_OUTPUT_EME,
											VBOTX_PHY_DRVCNT_2_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------

static uint32_t vbotx_phy_driver_output_emphasis_strength_control(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_STRENGTH_LEN];
	uint32_t ret;
	uint32_t i;

	// page: 2
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_EMLVL2_0_ADDR,
											VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_STRENGTH_LEN, data);
	// driver output Emphasis Value(0 0.5 1 1.5)
	for (i=0; i<VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_STRENGTH_LEN; i++) {
			SET_BLOCK_DATA(data, i, VBOTX_PHY_DRIVER_OUTPUT_emphasis_strength_1);		// 0x3494-0x3497
	}

	if(vbotxdata[0] != 0xFF)
	{
			//0X74:0X94(Page:2)
			SET_BLOCK_DATA(data, 0, vbotxdata[2]);
			//0X74:0X95(Page:2)
			SET_BLOCK_DATA(data, 1, vbotxdata[2]);
			//0X74:0X96(Page:2)
			SET_BLOCK_DATA(data, 2, vbotxdata[2]);
			//0X74:0X97(Page:2)
			SET_BLOCK_DATA(data, 3, vbotxdata[2]);
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_phy_driver_output_emphasis_period(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_LEN];
	uint32_t ret;

	// page: 2
	SET_ACCESS(access, &port, cmd, VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_LEN);
	// EMT:
	// 0x34C0
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_0_ADDR,
													VBOTX_PHY_DRIVER_OUTPUT_EMPHASIS_PERIOD_300PS,
											VIDEOIF_VBO_PHY_DRVCNT_0_MASK);

	if(vbotxdata[0] != 0xFF)
	{
			SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_0_ADDR,
													vbotxdata[0],
											0xFF);
	}
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}

//------------------------------------------------------------------------------
static uint32_t vbotx_phy_setting(system_port port)
{
	uint32_t ret;

	// page: 2

	ret = vbotx_select_page(port, VBOTX_PAGE_LVDSVBO_PHY);
	CHECK_ERROR(ret);
	// clock provision: lane 8-15
	ret = vbotx_phy_clock_provision_to_lane_setting(port);
	CHECK_ERROR(ret);


	// driver output amplitude setting in every lane
	/*
	1、 swing vbotx phy driver output amplitude setting
	0X74:0X88(Page:2)
	0X74:0X89(Page:2)
	0X74:0X8a(Page:2)
	0X74:0X8b(Page:2)
	*/
	ret = vbotx_phy_driver_output_amplitude_setting(port);
	CHECK_ERROR(ret);
	// driver output common mode voltage alignment
	ret = vbotx_phy_driver_output_common_mode_voltage_alignment_setting(port);
	CHECK_ERROR(ret);
	// driver output change Emphasis value
	/*
	2、
	0X74:0XC2(Page:2)[0]=1EME
	0X74:0X94(Page:2)
	0X74:0X95(Page:2)
	0X74:0X96(Page:2)
	0X74:0X97(Page:2)
	*/
	ret = vbotx_phy_driver_output_EME_TMCOR_switch2(port);
	ret = vbotx_phy_driver_output_emphasis_strength_control(port);
	CHECK_ERROR(ret);
	/*
	3、time(EMT)，
	0X74:0XC2(Page:2)[0]=1 EME
	0X74:0XC0(Page:2)[6:4]EMT
	*/
	// driver output Emphasis period
	ret = vbotx_phy_driver_output_EME_TMCOR_switch2(port);
	ret = vbotx_phy_driver_output_emphasis_period(port);
	CHECK_ERROR(ret);
	/* 0X74:0XC2(Page:2)[0]=1 EME */
	// driver output open EME
	ret = vbotx_phy_driver_output_EME_TMCOR_switch(port);
	CHECK_ERROR(ret);

	// DC test mode: ON
	ret = vbotx_phy_dc_test_mode_setting(port, VBOTX_DC_TEST_MODE_ON);
	CHECK_ERROR(ret);
	// VCO low speed oscillation: ON
	ret = vbotx_phy_vco_low_speed_oscillation_setting(port, VBOTX_VCO_LOW_SPEED_OSCILLATION_ON);
	CHECK_ERROR(ret);
	// VBO macro reset control: reset release (& power on)
	ret = vbotx_phy_macro_ctrl_setting(port, VBOTX_MACRO_RESET_RELEASE);
	CHECK_ERROR(ret);
	// lane enable control: enable 8lane (8-15)
	ret = vbotx_phy_lane_enable_ctrl_setting(port);
	CHECK_ERROR(ret);
	// DC test mode: OFF
	ret = vbotx_phy_dc_test_mode_setting(port, VBOTX_DC_TEST_MODE_OFF);
	CHECK_ERROR(ret);
	// VCO low speed oscillation: OFF
	ret = vbotx_phy_vco_low_speed_oscillation_setting(port, VBOTX_VCO_LOW_SPEED_OSCILLATION_OFF);
	CHECK_ERROR(ret);
	// wait for 4ms
	SLEEP(VBOTX_SLEEP_TIME_4MSEC);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_path_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PATH_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, VBOTX_PATH_NUM);
	// LVBON: 0b1 (enable)
	// LVDSEN: 0b0 (VBO)
	// 0x10B1
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR,
						VBOTX_PATH_SWMODE4_01_LVBON_ENABLE |
							VBOTX_PATH_SWMODE4_01_LVDSEN_VBO,
						VBOTX_PATH_SWMODE4_01_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_clock_setting(system_port port, vbotx_video *p_video)
{
	system_reg_access access;
	system_command cmd[VBOTX_CLOCK_NUM];
	system_ctrl_vbo_lane cnt;
	uint8_t tx0_sen;
	uint8_t txcore0_05_7_6;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	ret = system_ctrl_get_vbo_lane_count(p_video->tx_video_format, &cnt);
	CHECK_ERROR(ret);
	switch (cnt) {
		// NOTE: VBO-Tx cannot transmit by using 1 lane
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			tx0_sen = VBOTX_CKSELEN_TX0_SEN_MANUAL;
			txcore0_05_7_6 = VBOTX_CLOCK_TXCORE0_05_7_6_150MHZ_OR_LESS;
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			tx0_sen = VBOTX_CKSELEN_TX0_SEN_MANUAL;
			txcore0_05_7_6 = VBOTX_CLOCK_TXCORE0_05_7_6_150MHZ_300MHZ;
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			tx0_sen = VBOTX_CKSELEN_TX0_SEN_AUTO;
			txcore0_05_7_6 = VBOTX_CLOCK_TXCORE0_05_7_6_OVER_300MHZ;
			break;
		default:
			return ERR_VBOTX_TRANSMIT_FORMAT;
	}

	SET_ACCESS(access, &port, cmd, VBOTX_CLOCK_NUM);
	// 0x1E6A
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXCORE0_05_ADDR,
						VBOTX_CLOCK_RXCORE0_05_3_2
						| VBOTX_CLOCK_RXCORE0_05_5_4
						| VBOTX_CLOCK_RXCORE0_05_7_6,
						VBOTX_CLOCK_RXCORE0_05_MASK);
	// 0x1E7F
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_CKSELEN_ADDR,
						tx0_sen,
						VBOTX_CKSELEN_TX0_SEN_MASK);
	// 0x1E70
	SET_COMMAND(cmd, 2, VIDEOIF_CKGEN_TXCORE0_05_ADDR,
						VBOTX_CLOCK_TXCORE0_05_5_4 | txcore0_05_7_6,
						VBOTX_CLOCK_TXCORE0_05_7_4_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_hot_plug_detect(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_HPD_NUM];
	uint32_t i;
	system_config_retry retry_hot_plug;
	uint32_t ret;

	// get retry info
	ret = vbotx_ctrl_retry_hot_plug(&retry_hot_plug, VBOTX_CTRL_RETRY_GET);
	CHECK_ERROR(ret);

	// page: 1

	ret = vbotx_select_page(port, VBOTX_PAGE_VBO_LINK);
	CHECK_ERROR(ret);
	// read hot plug status
	SET_ACCESS(access, &port, cmd, VBOTX_HPD_NUM);
	// 0x3490
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL16_ADDR, 0, VBOTX_HPD_VBOCTL16_RDVBO_HTPDN_MASK);
	// retry hot plug detection
	for (i=0; i<retry_hot_plug.times; i++) {
		CHECK_ABORT();
		ret = lf9129_device_read(&access);
		CHECK_ERROR(ret);
		if (cmd[0].data == VBOTX_HPD_VBOCTL16_RDVBO_HTPDN_HPD) {
			break;
		}
		SLEEP(retry_hot_plug.interval);
	}
	if (i == retry_hot_plug.times) {
		return ERR_VBOTX_HOT_PLUG_DETECT_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_glue_setting(system_port port, vbotx_video *p_video)
{
	system_block_access baccess;
	uint8_t data[VBOTX_GLUE_LEN];
	system_reg_access access;
	system_command cmd[VBOTX_GLUE_NUM];
	system_ctrl_vbo_lane cnt;
	uint8_t ckmode, lnmode;
	uint8_t hstart70, hstart98;
	uint8_t sdmode, line_dly, bkmode, sy_dly;
	uint32_t pf, lane;
	uint32_t ret;

	ret = system_ctrl_get_vbo_lane_count(p_video->tx_video_format, &cnt);
	CHECK_ERROR(ret);
	switch (cnt) {
		// NOTE: VBO-Tx cannot transmit by using 1 lane
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			ckmode = VBOTX_GLUE_GLUECTL0_CKMODE_150MHZ_OR_LESS;
			lnmode = VBOTX_GLUE_GLUECTL1_LNMODE_150MHZ_OR_LESS;
			lane = 2;
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			ckmode = VBOTX_GLUE_GLUECTL0_CKMODE_150MHZ_300MHZ;
			lnmode = VBOTX_GLUE_GLUECTL1_LNMODE_150MHZ_300MHZ;
			lane = 4;
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			ckmode = VBOTX_GLUE_GLUECTL0_CKMODE_OVER_300MHZ;
			lnmode = VBOTX_GLUE_GLUECTL1_LNMODE_OVER_300MHZ;
			lane = 8;
			break;
		default:
			return ERR_VBOTX_TRANSMIT_FORMAT;
	}
	ret = system_ctrl_get_tx_glue_hstart(port, p_video->tx_video_format,
										SYSTEM_CTRL_VBO_FREQ_297000KHZ_P2L, &hstart70, &hstart98);
	CHECK_ERROR(ret);
	ret = system_ctrl_get_pixel_freq(p_video->tx_video_format, &pf);
	CHECK_ERROR(ret);
	if (pf == (SYSTEM_CTRL_VBO_FREQ_297000KHZ_P2L * lane / 2)) {
		sdmode = VBOTX_GLUE_GLUECTL1_1_SDMODE_DE_HALF;
		line_dly = VBOTX_GLUE_GLUECTL1_1_LINE_DLY_0;
		sy_dly = VBOTX_GLUE_GLUECTL8_SY_DLY_DEFAULT;
	} else {
		// only when 720x576P, set 0x08 to sy_dly
		if (p_video->tx_video_format == SYSTEM_VFORM_720x576P_50_4v3
			|| p_video->tx_video_format == SYSTEM_VFORM_720x576P_50_16v9) {
			sdmode = VBOTX_GLUE_GLUECTL1_1_SDMODE_REG_SETUP;
			line_dly = VBOTX_GLUE_GLUECTL1_1_LINE_DLY_1;
			sy_dly = VBOTX_GLUE_GLUECTL8_SY_DLY_08;
		} else {
			sdmode = VBOTX_GLUE_GLUECTL1_1_SDMODE_DE_SAME;
			line_dly = VBOTX_GLUE_GLUECTL1_1_LINE_DLY_1;
			sy_dly = VBOTX_GLUE_GLUECTL8_SY_DLY_DEFAULT;
		}
	}
	if (sdmode && line_dly) {
		bkmode = VBOTX_GLUE_GLUECTLX_BKMODE_DIV_LINE_INTO_2BANK;
	} else {
		bkmode = VBOTX_GLUE_GLUECTLX_BKMODE_NOT_USE_BANK_SWITCH;
	}

	// page: 3

	ret = vbotx_select_page(port, VBOTX_PAGE_GLUE);
	CHECK_ERROR(ret);
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_GLUE_GLUECTL0_ADDR,
						VBOTX_GLUE_LEN, data);
	// SRAM write main control: clock mode, bank mode
	SET_BLOCK_DATA(data, 0, ckmode | bkmode);					// 0x3480
	// SRAM read main control: number of lane, bank mode
	SET_BLOCK_DATA(data, 1, lnmode | bkmode);					// 0x3481
	// Hstart [7:0]:
	SET_BLOCK_DATA(data, 2, hstart70);							// 0x3482
	// Hstart [9:8]:
	SET_BLOCK_DATA(data, 3, hstart98);							// 0x3483
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_ACCESS(access, &port, cmd, VBOTX_GLUE_NUM);
	// 0x3488
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_GLUE_GLUECTL8_ADDR,
						sy_dly,
						VBOTX_GLUE_GLUECTL8_SY_DLY_MASK);
	// 0x348E
	SET_COMMAND(cmd, 1, VIDEOIF_LVDVB_GLUE_GLUECTL1_1_ADDR,
						sdmode
						| line_dly,
						VBOTX_GLUE_GLUECTL1_1_SDMODE_MASK
						| VBOTX_GLUE_GLUECTL1_1_LINE_DLY_MASK);
	ret = lf9129_device_write(&access);

	if (panelDiv == VBO_DOUBLE_DIVIDE_SECTION) { // 采用双分屏屏参，且带9129，就需要切换为单分屏屏参
		MES("Mn869129 switch double vbo div\n");
		SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_GLUE_GLUECTL1_1_ADDR, 1, data);
		SET_BLOCK_DATA(data, 0, 0x14);
		ret = device_block_write(&baccess);
		CHECK_ERROR(ret);
	} else if(panelDiv == VBO_SINGLE_DIVIDE_SECTION) {
		MES("Mn869129 switch single vbo div\n");
	} else {
		MES("Mn869129 panel div error, panel div == %d\n", panelDiv);
	}


	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_hvpol_setting(system_port port, vbotx_video *p_video)
{
	system_ctrl_polarity hpol, vpol;
	uint8_t hs_pol, vs_pol;
	uint8_t viohpol, viovpol;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_HDMI) {
		ret = system_ctrl_get_hdmirx_sync_polarity(port, &hpol, &vpol);
		CHECK_ERROR(ret);
		// hs_pol: 0:Positive, 1:Negative
		hs_pol = (hpol == SYSTEM_CTRL_POL_P)
					? VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_P
					: VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_N;
		// vs_pol: 0:Positive, 1:Negative
		vs_pol = (vpol == SYSTEM_CTRL_POL_P)
					? VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_P
					: VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_N;
		// viohpol: 1:Positive, 0:Negative
		viohpol = (hpol == SYSTEM_CTRL_POL_P)
					? VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_HIGH
					: VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_LOW;
		// viovpol: 1:Positive, 0:Negative
		viovpol = (vpol == SYSTEM_CTRL_POL_P)
					? VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_HIGH
					: VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_LOW;
	} else if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_VBO) {
		// hs_pol: 0:Positive, 1:Negative
		hs_pol = VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_P;
		// vs_pol: 0:Positive, 1:Negative
		vs_pol = VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_P;
		// viohpol: 1:Positive, 0:Negative
		viohpol = VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_HIGH;
		// viovpol: 1:Positive, 0:Negative
		viovpol = VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_HIGH;
	} else {
		ret = system_ctrl_get_sync_polarity(p_video->rx_video_format, &hpol, &vpol);
		CHECK_ERROR(ret);
		// hs_pol: 0:Positive, 1:Negative
		hs_pol = (hpol == SYSTEM_CTRL_POL_P)
					? VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_P
					: VBOTX_LVDVB_GLUE_GLUECTL3_HS_POL_N;
		// vs_pol: 0:Positive, 1:Negative
		vs_pol = (vpol == SYSTEM_CTRL_POL_P)
					? VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_P
					: VBOTX_LVDVB_GLUE_GLUECTL3_VS_POL_N;
		// viohpol: 1:Positive, 0:Negative
		viohpol = (hpol == SYSTEM_CTRL_POL_P)
					? VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_HIGH
					: VBOTX_VBO_LINK_VBOCTL2_VIOHPOL_LOW;
		// viovpol: 1:Positive, 0:Negative
		viovpol = (vpol == SYSTEM_CTRL_POL_P)
					? VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_HIGH
					: VBOTX_VBO_LINK_VBOCTL2_VIOVPOL_LOW;
	}

	// page: 3

	ret = vbotx_select_page(port, VBOTX_PAGE_GLUE);
	CHECK_ERROR(ret);
	ret = vbotx_set_glue_polarity(port, (hs_pol | vs_pol));
	CHECK_ERROR(ret);

	// page: 1

	ret = vbotx_select_page(port, VBOTX_PAGE_VBO_LINK);
	CHECK_ERROR(ret);
	ret = vbotx_set_link_polarity(port, (viohpol | viovpol), VBOTX_VBO_LINK_VBOCTL2_VIOXPOL_MASK);
	CHECK_ERROR(ret);

	// 0x3482 [3:2]: vbohpol/vbovpol setting is performed at vbotx_vbo_link_hvsync_polarity_setting()

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_lvds_link_double_phase_output_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN];
	uint32_t ret;
	uint32_t i;

	// page: 0

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDS_LINK_LVDSCONFCTL1_ADDR,
						VBOTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN, data);
	// enable (double phase output)
	// 0x3480-0x3483
	for (i=0; i<VBOTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN; i++) {
		SET_BLOCK_DATA(data, i, VBOTX_LVDS_LINK_LVDSCONFCTL1_ODEVPASS_X_DOUBLE);
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_lvds_link_vbo_output_setting(system_port port, vbotx_video *p_video)
{
	system_block_access baccess;
	uint8_t data[VBOTX_LVDS_LINK_VBO_OUTPUT_LEN];
	system_ctrl_vbo_lane cnt;
	uint8_t vbotxenb, vbobitmode;
	uint32_t i;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	ret = system_ctrl_get_vbo_lane_count(p_video->tx_video_format, &cnt);
	CHECK_ERROR(ret);
	switch (cnt) {
		// NOTE: VBO-Tx cannot transmit by using 1 lane
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			vbotxenb = VBOTX_LVDS_LINK_VBOOUTCTL_VBOTXENB_2LANE;
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			vbotxenb = VBOTX_LVDS_LINK_VBOOUTCTL_VBOTXENB_4LANE;
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			vbotxenb = VBOTX_LVDS_LINK_VBOOUTCTL_VBOTXENB_8LANE;
			break;
		default:
			return ERR_VBOTX_TRANSMIT_FORMAT;
	}
	// deep color
	switch (p_video->tx_deep_color) {
		case SYSTEM_DEEP_COLOR_8BIT:								//  8bit
			vbobitmode = VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOBITMODE_X_08BIT;
			break;
		case SYSTEM_DEEP_COLOR_10BIT:								// 10bit
			vbobitmode = VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOBITMODE_X_10BIT;
			break;
		case SYSTEM_DEEP_COLOR_12BIT:
		default:
			return ERR_VBOTX_TRANSMIT_FORMAT;
	}

	// page: 0

	ret = vbotx_select_page(port, VBOTX_PAGE_LVDS_LINK);
	CHECK_ERROR(ret);
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDS_LINK_VBOOUTCTL_ADDR,
						VBOTX_LVDS_LINK_VBO_OUTPUT_LEN, data);
	// VBO output enable setting
	// 0x34A0
	SET_BLOCK_DATA(data, 0, vbotxenb);
	// VBO pinsel setting
	//   pin assignment mode: 0b000
	//   bit number:          0b10 (10bit) | 0b00 (8bit)
	//   msb-lsb order swap:  0b0
	// 0x34A1-0x34A4
	for (i=1; i<VBOTX_LVDS_LINK_VBO_OUTPUT_LEN; i++) {
		SET_BLOCK_DATA(data, i, VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOASMODE_X_00 |
									vbobitmode |
									VBOTX_LVDS_LINK_VBOPINASSIGNX_VBOPIXSWAP_X_OFF);
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_lvds_link_lvds_output_disable_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_LVDS_LINK_LVDS_OUTPUT_DISABLE_NUM];
	uint32_t ret;

	// page: 0

	SET_ACCESS(access, &port, cmd, VBOTX_LVDS_LINK_LVDS_OUTPUT_DISABLE_NUM);
	// data disable
	// 0x3484
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSOUTCTL1_ADDR,
						VBOTX_LVDS_LINK_LVDSOUTCTL1_ALL_DISABLE,
						VBOTX_LVDS_LINK_LVDSOUTCTL1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_lvds_link_lvds_output_lane_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[VBOTX_LVDS_LINK_LVDS_OUTPUT_LANE_LEN];
	uint32_t ret;

	// page: 0

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDS_LINK_LVDSOUTCTL3_ADDR,
						VBOTX_LVDS_LINK_LVDS_OUTPUT_LANE_LEN, data);
	// 0x3486
	SET_BLOCK_DATA(data, 0, VBOTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T4LSEL_0X0       |
								VBOTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T3LSEL_0X0);
	// 0x3487
	SET_BLOCK_DATA(data, 1, VBOTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T2LSEL_0X0       |
								VBOTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T1LSEL_0X0   |
								VBOTX_LVDS_LINK_LVDSOUTCTL4_LVDSMODE_SEL_DEFAULT);
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_lvds_link_lvds_pinsel_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_LVDS_LINK_LVDS_PINSEL_NUM];
	uint32_t ret;

	// page: 0

	SET_ACCESS(access, &port, cmd, VBOTX_LVDS_LINK_LVDS_PINSEL_NUM);
	// 1st phase
	// 0x3488
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSPINASSIGN1_ADDR,
						VBOTX_LVDS_LINK_LVDSPINASSIGN1_LVASMODE_1_VESA8,
						VBOTX_LVDS_LINK_LVDSPINASSIGNX_LVASMODE_X_MASK);
	// 2nd phase
	// 0x348E
	SET_COMMAND(cmd, 1, VIDEOIF_LVDS_LINK_LVDSPINASSIGN2_ADDR,
						VBOTX_LVDS_LINK_LVDSPINASSIGN2_LVASMODE_2_VESA8,
						VBOTX_LVDS_LINK_LVDSPINASSIGNX_LVASMODE_X_MASK);
	// 3rd phase
	// 0x3494
	SET_COMMAND(cmd, 2, VIDEOIF_LVDS_LINK_LVDSPINASSIGN3_ADDR,
						VBOTX_LVDS_LINK_LVDSPINASSIGN3_LVASMODE_3_VESA8,
						VBOTX_LVDS_LINK_LVDSPINASSIGNX_LVASMODE_X_MASK);
	// 4th phase
	// 0x349A
	SET_COMMAND(cmd, 3, VIDEOIF_LVDS_LINK_LVDSPINASSIGN4_ADDR,
						VBOTX_LVDS_LINK_LVDSPINASSIGN4_LVASMODE_4_VESA8,
						VBOTX_LVDS_LINK_LVDSPINASSIGNX_LVASMODE_X_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_lvds_link_setting(system_port port)
{
	uint32_t ret;

	// page: 0

	ret = vbotx_select_page(port, VBOTX_PAGE_LVDS_LINK);
	CHECK_ERROR(ret);
	// double phase output setting
	ret = vbotx_lvds_link_double_phase_output_setting(port);
	CHECK_ERROR(ret);
	// LVDS output disable setting
	ret = vbotx_lvds_link_lvds_output_disable_setting(port);
	CHECK_ERROR(ret);
	// LVDS output lane setting
	ret = vbotx_lvds_link_lvds_output_lane_setting(port);
	CHECK_ERROR(ret);
	// LVDS pinsel setting
	ret = vbotx_lvds_link_lvds_pinsel_setting(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_set_glue_polarity(system_port port, uint8_t pol)
{
	system_reg_access access;
	system_command cmd[VBOTX_SET_GLUE_POLARITY_NUM];
	uint32_t ret;

	// page: 3

	SET_ACCESS(access, &port, cmd, VBOTX_SET_GLUE_POLARITY_NUM);
	// vs_pol,hs_pol: 0:Positive, 1:Negative
	// 0x3483
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_GLUE_GLUECTL3_ADDR,
						pol,
						VBOTX_LVDVB_GLUE_GLUECTL3_XS_POL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_set_link_polarity(system_port port, uint8_t pol, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[VBOTX_SET_LINK_POLARITY_NUM];
	uint32_t ret;

	// page: 1

	SET_ACCESS(access, &port, cmd, VBOTX_SET_LINK_POLARITY_NUM);
	// vbohpol,vbovpol: 1:Positive, 0:Negative
	// viohpol,viohpol: 1:Positive, 0:Negative
	// 0x3482
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL2_ADDR,
						pol,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_vbo_link_hvsync_polarity_setting(system_port port, system_inversion hvsync_polarity)
{
	uint8_t vbohpol, vbovpol;
	uint32_t ret;

	// page: 1

	// vbohpol,vbovpol: 1:Positive, 0:Negative
	if (hvsync_polarity == SYSTEM_INVERSION_ON) {
		// Negative sync
		vbohpol = VBOTX_VBO_LINK_VBOCTL2_VBOHPOL_LOW;
		vbovpol = VBOTX_VBO_LINK_VBOCTL2_VBOVPOL_LOW;
	} else {
		// Positive sync
		vbohpol = VBOTX_VBO_LINK_VBOCTL2_VBOHPOL_HIGH;
		vbovpol = VBOTX_VBO_LINK_VBOCTL2_VBOVPOL_HIGH;
	}
	ret = vbotx_set_link_polarity(port, (vbohpol | vbovpol), VBOTX_VBO_LINK_VBOCTL2_MASK);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_vbo_link_lane_assignment_setting(system_port port, vbotx_video *p_video)
{
	system_block_access baccess;
	uint8_t data[VBOTX_VBO_LINK_LANE_ASSIGN_LEN];
	system_ctrl_vbo_lane cnt;
	uint8_t vboctl23, vboctl24, vboctl25, vboctl26;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	ret = system_ctrl_get_vbo_lane_count(p_video->tx_video_format, &cnt);
	CHECK_ERROR(ret);
	switch (cnt) {
		// NOTE: VBO-Tx cannot transmit by using 1 lane
		case SYSTEM_CTRL_VBO_LANE_1:
		case SYSTEM_CTRL_VBO_LANE_2:
			vboctl23 = VBOTX_VBOCTL23_LANE0SEL_LANE0 | VBOTX_VBOCTL23_LANE1SEL_LANE1;
			vboctl24 = 0;		// not use lane2, 3
			vboctl25 = 0;		// not use lane4, 5
			vboctl26 = 0;		// not use lane6, 7
			break;
		case SYSTEM_CTRL_VBO_LANE_4:
			vboctl23 = VBOTX_VBOCTL23_LANE0SEL_LANE0 | VBOTX_VBOCTL23_LANE1SEL_LANE2;
			vboctl24 = VBOTX_VBOCTL24_LANE2SEL_LANE1 | VBOTX_VBOCTL24_LANE3SEL_LANE3;
			vboctl25 = 0;		// not use lane4, 5
			vboctl26 = 0;		// not use lane6, 7
			break;
		case SYSTEM_CTRL_VBO_LANE_8:
			// vboctl23 = VBOTX_VBOCTL23_LANE0SEL_LANE0 | VBOTX_VBOCTL23_LANE1SEL_LANE2;
			// vboctl24 = VBOTX_VBOCTL24_LANE2SEL_LANE4 | VBOTX_VBOCTL24_LANE3SEL_LANE6;
			// vboctl25 = VBOTX_VBOCTL25_LANE4SEL_LANE1 | VBOTX_VBOCTL25_LANE5SEL_LANE3;
			// vboctl26 = VBOTX_VBOCTL26_LANE6SEL_LANE5 | VBOTX_VBOCTL26_LANE7SEL_LANE7;

			if(panelDiv == VBO_DOUBLE_DIVIDE_SECTION) {
				vboctl23 = 0x07 | 0x50;
				vboctl24 = 0x40 | 0x06;		// not use lane2, 3
				vboctl25 = 0x10 | 0x03;		// not use lane4, 5
				vboctl26 = 0x00 | 0x02;		// not use lane6, 7
				MES("Mn869129 switch double vbo div line\n");
			} else if(panelDiv == VBO_SINGLE_DIVIDE_SECTION){
				vboctl23 = 0x07 | 0x50;
				vboctl24 = 0x10 | 0x03;		 // not use lane2, 3
				vboctl25 = 0x40 | 0x06;		 // not use lane4, 5
				vboctl26 = 0x00 | 0x02;		 // not use lane6, 7
				MES("Mn869129 switch single vbo div line\n");
			} else {
				MES("Mn869129 panel div error, panel div == %d\n", panelDiv);
			}
			break;
		default:
			return ERR_VBOTX_TRANSMIT_FORMAT;
	}

	// page: 1

	ret = vbotx_select_page(port, VBOTX_PAGE_VBO_LINK);
	CHECK_ERROR(ret);
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VBO_LINK_VBOCTL23_ADDR,
						VBOTX_VBO_LINK_LANE_ASSIGN_LEN, data);
	SET_BLOCK_DATA(data, 0, vboctl23);							// 0x3497
	SET_BLOCK_DATA(data, 1, vboctl24);							// 0x3498
	SET_BLOCK_DATA(data, 2, vboctl25);							// 0x3499
	SET_BLOCK_DATA(data, 3, vboctl26);							// 0x349A
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_vbo_link_output_Inverting_Data_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_VBO_LINK_OUTPUT_INVERTING_DATA_PATH_NUM];
	uint32_t ret;

	// page: 1

	SET_ACCESS(access, &port, cmd, VBOTX_VBO_LINK_OUTPUT_INVERTING_DATA_PATH_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL27_ADDR,
						VBOTX_VBO_LINK_VBOCTL27_OUTPUT_INVERTING_VBO,
						VBOTX_VBO_LINK_VBOCTL22_OUTPUT_INVERTING_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_vbo_link_link_data_path_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_VBO_LINK_LINK_DATA_PATH_NUM];
	uint32_t ret;

	// page: 1

	SET_ACCESS(access, &port, cmd, VBOTX_VBO_LINK_LINK_DATA_PATH_NUM);
	// 0x3496
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL22_ADDR,
						VBOTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_VBO,
						VBOTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_vbo_link_setting(system_port port, system_inversion hvsync_polarity)
{
	uint32_t ret;

	// page: 1

	ret = vbotx_select_page(port, VBOTX_PAGE_VBO_LINK);
	CHECK_ERROR(ret);
	// Hsync/Vsync polarity setting
	ret = vbotx_vbo_link_hvsync_polarity_setting(port, hvsync_polarity);
	CHECK_ERROR(ret);

	//output Inverting
	vbotx_vbo_link_output_Inverting_Data_setting(port);

	// link data path setting
	ret = vbotx_vbo_link_link_data_path_setting(port);
	CHECK_ERROR(ret);
	// power down setting
	//   already done by system_ctrl_init

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_lock_inform(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PLL_LOCK_INFORM_NUM];
	uint32_t ret;

	CHECK_ABORT();
	// wait for 4ms before performing the following
	SLEEP(VBOTX_SLEEP_TIME_4MSEC);

	// page: 1

	SET_ACCESS(access, &port, cmd, VBOTX_PLL_LOCK_INFORM_NUM);
	// 0x3480
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL0_ADDR,
						VBOTX_PLL_LOCK_VBOCTL0_PLLLOCKED_LOCK,
						VBOTX_PLL_LOCK_VBOCTL0_PLLLOCKED_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_lock_detect(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_PLD_NUM];
	uint32_t i;
	system_config_retry retry_pll_lock;
	uint32_t ret;

	// get retry info
	ret = vbotx_ctrl_retry_pll_lock(&retry_pll_lock, VBOTX_CTRL_RETRY_GET);
	CHECK_ERROR(ret);

	// page: 1

	SET_ACCESS(access, &port, cmd, VBOTX_PLD_NUM);
	// read VBO-Rx PLL lock status
	// 0x3490
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL16_ADDR, 0, VBOTX_PLD_VBOCTL16_RDVBO_LOCKN_MASK);
	// retry pll lock detection
	for (i=0; i<retry_pll_lock.times; i++) {
		CHECK_ABORT();
		ret = lf9129_device_read(&access);
		CHECK_ERROR(ret);
		if (cmd[0].data == VBOTX_PLD_VBOCTL16_RDVBO_LOCKN_PLD) {
			break;
		}
		SLEEP(retry_pll_lock.interval);
	}
	if (i == retry_pll_lock.times) {
		return ERR_VBOTX_PLL_LOCK_DETECT_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_link_up_complete(system_port port)
{
	system_reg_access access;
	system_command cmd[VBOTX_LUC_NUM];
	uint32_t ret;
	uint32_t i;

	// page: 1

	SET_ACCESS(access, &port, cmd, VBOTX_LUC_NUM);
	// read VBO-Tx link-up status
	// 0x3481
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL1_ADDR, 0, VBOTX_LUC_VBOCTL1_RDVBO_READY_MASK);
	for (i=0; i<VBOTX_LUC_RETRY_MAX; i++) {
		CHECK_ABORT();
		ret = lf9129_device_read(&access);
		CHECK_ERROR(ret);
		if (cmd[0].data == VBOTX_LUC_VBOCTL1_RDVBO_READY_LUC) {
			break;
		}
		SLEEP(VBOTX_LUC_RETRY_INTERVAL);
	}
	if (i == VBOTX_LUC_RETRY_MAX) {
		return ERR_VBOTX_LINK_UP_COMPLETE_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_pll_lock_setting(system_port port)
{
	uint32_t ret;

	// page: 1

	ret = vbotx_select_page(port, VBOTX_PAGE_VBO_LINK);
	CHECK_ERROR(ret);
	// PLL lock inform
	ret = vbotx_pll_lock_inform(port);
	CHECK_ERROR(ret);
	// check PLL lock detect
	ret = vbotx_pll_lock_detect(port);
	CHECK_ERROR(ret);
	// check link-up complete
	ret = vbotx_link_up_complete(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_tx_link_setting(vbotx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	CHECK_ABORT();
	// set video format (Tx link)
	ret = system_ctrl_tx_set_video_format(p_video);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// set video color format
	ret = system_ctrl_set_color_format(p_video);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// update register
	ret = system_ctrl_tx_update_reg(p_video->tx_port, SYSTEM_CTRL_TX_UPDATE_REG_ALL);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_ctrl_retry_hot_plug(system_config_retry *p_retry, vbotx_ctrl_retry ctrl)
{
	static system_config_retry vbotx_config_retry_hot_plug = {
		SYSTEM_UPDATE_DISABLE,
		VBOTX_CONFIG_RETRY_HOT_PLUG_TIMES_DEFAULT,
		VBOTX_CONFIG_RETRY_HOT_PLUG_INTERVAL_DEFAULT
	};

	if (p_retry == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == VBOTX_CTRL_RETRY_GET) {
		p_retry->times = vbotx_config_retry_hot_plug.times;
		p_retry->interval = vbotx_config_retry_hot_plug.interval;
	} else if (ctrl == VBOTX_CTRL_RETRY_SET) {
		if (p_retry->update == SYSTEM_UPDATE_ENABLE) {
			vbotx_config_retry_hot_plug.update = SYSTEM_UPDATE_ENABLE;
			vbotx_config_retry_hot_plug.times = p_retry->times;
			vbotx_config_retry_hot_plug.interval = p_retry->interval;
		}
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_ctrl_retry_pll_lock(system_config_retry *p_retry, vbotx_ctrl_retry ctrl)
{
	static system_config_retry vbotx_config_retry_pll_lock = {
		SYSTEM_UPDATE_DISABLE,
		VBOTX_CONFIG_RETRY_PLL_LOCK_TIMES_DEFAULT,
		VBOTX_CONFIG_RETRY_PLL_LOCK_INTERVAL_DEFAULT
	};

	if (p_retry == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == VBOTX_CTRL_RETRY_GET) {
		p_retry->times = vbotx_config_retry_pll_lock.times;
		p_retry->interval = vbotx_config_retry_pll_lock.interval;
	} else if (ctrl == VBOTX_CTRL_RETRY_SET) {
		if (p_retry->update == SYSTEM_UPDATE_ENABLE) {
			vbotx_config_retry_pll_lock.update = SYSTEM_UPDATE_ENABLE;
			vbotx_config_retry_pll_lock.times = p_retry->times;
			vbotx_config_retry_pll_lock.interval = p_retry->interval;
		}
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t vbotx_ctrl_req_abort(system_request *p_req, vbotx_ctrl_req ctrl)
{
	static system_request vbotx_abort_req = SYSTEM_REQUEST_NONE;

	if (p_req == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == VBOTX_CTRL_REQ_GET) {
		*p_req =vbotx_abort_req ;
	} else if (ctrl == VBOTX_CTRL_REQ_SET) {
		vbotx_abort_req = *p_req;
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static system_request vbotx_ctrl_get_abort_req(void)
{
	system_request req;

	if (vbotx_ctrl_req_abort(&req, VBOTX_CTRL_REQ_GET)) {
		return SYSTEM_REQUEST_NONE;
	}

	return req;
}
//------------------------------------------------------------------------------
static void vbotx_ctrl_set_abort_req(system_request req)
{
	if (vbotx_ctrl_req_abort(&req, VBOTX_CTRL_REQ_SET)) {
		return;
	}
}
//------------------------------------------------------------------------------
uint32_t vbotx_set_video_format_wo_tx_link(vbotx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_vbotx_init(p_video->tx_port);
	CHECK_ERROR(ret);
	// clock setting
	ret = vbotx_clock_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// Tx link setting
	//
	//
	// hot plug detect
	ret = vbotx_hot_plug_detect(p_video->tx_port);
	CHECK_ERROR(ret);
	// glue setting
	ret = vbotx_glue_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// hpol, vpol setting
	ret = vbotx_hvpol_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// VBO output setting
	ret = vbotx_lvds_link_vbo_output_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// lane assignment setting
	ret = vbotx_vbo_link_lane_assignment_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// PLL lock
	ret = vbotx_pll_lock_setting(p_video->tx_port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vbotx_configure(vbotx_config *p_conf)
{
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// set retry info for hot plug detection
	ret = vbotx_ctrl_retry_hot_plug(&(p_conf->hot_plug_detect), VBOTX_CTRL_RETRY_SET);
	CHECK_ERROR(ret);
	// set retry info for pll lock detection
	ret = vbotx_ctrl_retry_pll_lock(&(p_conf->pll_lock_detect), VBOTX_CTRL_RETRY_SET);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vbotx_init(system_config *p_conf)
{
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}
	// system control initialization
	ret = system_ctrl_init(p_conf->port);
	CHECK_ERROR(ret);
	// initial setting
	ret = vbotx_init_setting(p_conf->port);
	CHECK_ERROR(ret);
	// PLL setting
	ret = vbotx_pll_setting(p_conf->port);
	CHECK_ERROR(ret);
	// PHY setting
	ret = vbotx_phy_setting(p_conf->port);
	CHECK_ERROR(ret);
	// path setting
	ret = vbotx_path_setting(p_conf->port);
	CHECK_ERROR(ret);
	// LVDS link setting
	ret = vbotx_lvds_link_setting(p_conf->port);
	CHECK_ERROR(ret);
	// VBO link setting
	ret = vbotx_vbo_link_setting(p_conf->port, p_conf->hvsync_polarity);
	CHECK_ERROR(ret);
	// init abort request
	vbotx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vbotx_exit(system_port port)
{
	uint32_t ret;

	// System control exit
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vbotx_set_video_format(vbotx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_vbotx_init(p_video->tx_port);
	CHECK_ERROR(ret);
	// clock setting
	ret = vbotx_clock_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// Tx link setting
	ret = vbotx_tx_link_setting(p_video);
	CHECK_ERROR(ret);
	// hot plug detect
	ret = vbotx_hot_plug_detect(p_video->tx_port);
	CHECK_ERROR(ret);
	// glue setting
	ret = vbotx_glue_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// hpol, vpol setting
	ret = vbotx_hvpol_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// VBO output setting
	ret = vbotx_lvds_link_vbo_output_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// lane assignment setting
	ret = vbotx_vbo_link_lane_assignment_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// PLL lock
	ret = vbotx_pll_lock_setting(p_video->tx_port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vbotx_video_mute(vbotx_mute *p_mute)
{
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_vbotx_init(p_mute->port);
	CHECK_ERROR(ret);

	if (p_mute->mute == SYSTEM_TX_MUTE_ON || p_mute->mute == SYSTEM_TX_MUTE_OFF) {
		ret = system_ctrl_tx_video_mute((system_mute *)p_mute);
		CHECK_ERROR(ret);
	} else if (p_mute->mute == SYSTEM_TX_MUTE_ON_BLACK ||
				p_mute->mute == SYSTEM_TX_MUTE_OFF_BLACK) {
		ret = system_ctrl_tx_video_black((system_mute *)p_mute);
		CHECK_ERROR(ret);
	} else {
		return ERR_NG;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t vbotx_abort(void)
{
	// abort request
	vbotx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

	return ERR_OK;
}

void vbotx_Parm(uint8_t* pVb1data)
{
		if(pVb1data == NULL)
		{
				vbotxdata[0] = 0;
				return;
		}
		memcpy(vbotxdata,pVb1data,sizeof(vbotxdata));
}
//------------------------------------------------------------------------------
