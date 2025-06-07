/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "lvdstx.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// lvdstx_select_page()
#define LVDSTX_PAGE_SELECT_NUM								1
#define LVDSTX_PAGE_SELECT_MASK								0x000000FF
//------------------------------------------------------------------------------
// lvdstx_pll_fraction_freq_dividing_setting()
#define LVDSTX_PLL_SSC_FFD_SETTING_LEN						4
#define LVDSTX_PLL_SSCCNT1_0_MIN_0X26						0x26
#define LVDSTX_PLL_SSCCNT1_1_KIN_0_0X00						0x00
#define LVDSTX_PLL_SSCCNT1_2_KIN_1_0X00						0x00
#define LVDSTX_PLL_SSCCNT1_3_KIN_2_0X08						0x08
//------------------------------------------------------------------------------
// lvdstx_pll_analog_ctrl_setting()
#define LVDSTX_PLL_ANALOG_CTRL_LEN							4
#define LVDSTX_PLL_PLLANACNT_0_REG1_181						0x08
#define LVDSTX_PLL_PLLANACNT_0_REG2_IREFX13					0xD0
#define LVDSTX_PLL_PLLANACNT_1_REG2_IREFX13					0x00
#define LVDSTX_PLL_PLLANACNT_1_REG3_FR3						0x30
#define LVDSTX_PLL_PLLANACNT_2_REG4_FR3						0x03
#define LVDSTX_PLL_PLLANACNT_2_VCOLMT_0B100					0x40
#define LVDSTX_PLL_PLLANACNT_3_DIVL_NORMAL					0x00
#define LVDSTX_PLL_PLLANACNT_3_CP2EN_DISABLE_HALVING		0x00
#define LVDSTX_PLL_PLLANACNT_3_CP2EN2_1CHARGE_PUMP			0x00
//------------------------------------------------------------------------------
// lvdstx_pll_digital_ctrl_setting()
#define LVDSTX_PLL_DIGITAL_CTRL_LEN							2
#define LVDSTX_PLL_PLLDIGCNT_0_FIDIV_DR1					0x00
#define LVDSTX_PLL_PLLDIGCNT_0_PDIV_DR2						0x20
#define LVDSTX_PLL_PLLDIGCNT_1_QDIV_DR1						0x00
//------------------------------------------------------------------------------
// lvdstx_pll_reset_release_of_delta_sigma_circuit()
#define LVDSTX_PLL_SSC_RESET_CTRL_DELTA_SIGMA_NUM			1
#define LVDSTX_PLL_SSCCNT2_3_NRSTDS_RESET_RELEASE			0x40
#define LVDSTX_PLL_SSCCNT2_3_NRSTDS_MASK					0x00000040
#define LVDSTX_SLEEP_TIME_20MSEC							20
//------------------------------------------------------------------------------
// lvdstx_phy_clock_provision_to_lane_setting()
#define LVDSTX_PHY_CLOCK_PROVISION_NUM						1
#define LVDSTX_PHY_PHYRSV_RSV_LANE0_23						0x00
#define LVDSTX_PHY_PHYRSV_RSV_MASK							0x0000000C
//------------------------------------------------------------------------------
// lvdstx_phy_driver_output_amplitude_setting()
#define LVDSTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN				12
#define LVDSTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LVDS_200			0x00
//------------------------------------------------------------------------------
// lvdstx_phy_driver_output_common_mode_voltage_alignment_setting()
#define LVDSTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM	1
#define LVDSTX_PHY_DRVCNT_1_NVCOM080_1						0x40
#define LVDSTX_PHY_DRVCNT_1_NVCOM120_1						0x20
#define LVDSTX_PHY_DRVCNT_1_NVCOM125_0						0x00
#define LVDSTX_PHY_DRVCNT_1_NPORTEN_1						0x08
#define LVDSTX_PHY_DRVCNT_1_LVDSEN_1						0x04
#define LVDSTX_PHY_DRVCNT_1_MASK							0x0000007C
//------------------------------------------------------------------------------
// lvdstx_phy_dc_test_mode_setting()
#define LVDSTX_PHY_DC_TEST_MODE_NUM							1
#define LVDSTX_PHY_DRVCNT_3_DRVMEN_BIT						4
#define LVDSTX_PHY_DRVCNT_3_DRVMEN_MASK						0x00000010
//------------------------------------------------------------------------------
// lvdstx_phy_vco_low_speed_oscillation_setting()
#define LVDSTX_PHY_VCO_LOW_SPEED_OSCILLATION_NUM			1
#define LVDSTX_PHY_PLLDIGCNT_1_OFFSETON_BIT					4
#define LVDSTX_PHY_PLLDIGCNT_1_OFFSETON_MASK				0x00000010
//------------------------------------------------------------------------------
// lvdstx_phy_macro_ctrl_setting()
#define LVDSTX_PHY_MACRO_CTRL_NUM							1
#define LVDSTX_PHY_ENTCNT_3_NRESET_BIT						5
#define LVDSTX_PHY_ENTCNT_3_NPDOWN_BIT						4
#define LVDSTX_PHY_ENTCNT_3_MASK							0x000000FF
#define LVDSTX_SLEEP_TIME_1MSEC								1
//------------------------------------------------------------------------------
// lvdstx_phy_lane_enable_ctrl_setting()
#define LVDSTX_PHY_LANE_ENABLE_CTRL_LEN						3
#define LVDSTX_PHY_ENTCNT_0_LANE0_7_ENABLE					0xFF
#define LVDSTX_PHY_ENTCNT_1_LANE8_15_ENABLE					0xFF
#define LVDSTX_PHY_ENTCNT_2_LANE16_23_ENABLE				0xFF
//------------------------------------------------------------------------------
// lvdstx_path_setting()
#define LVDSTX_PATH_NUM										1
#define LVDSTX_PATH_SWMODE4_01_LVBON_ENABLE					0x20
#define LVDSTX_PATH_SWMODE4_01_LVDSEN_LVDS					0x40
#define LVDSTX_PATH_SWMODE4_01_MASK							0x60
//------------------------------------------------------------------------------
// lvdstx_clock_setting()
#define LVDSTX_CLOCK_NUM									3
#define LVDSTX_CLOCK_RXCORE0_05_3_2							0x00
#define LVDSTX_CLOCK_RXCORE0_05_5_4							0x00
#define LVDSTX_CLOCK_RXCORE0_05_7_6							0x00
#define LVDSTX_CLOCK_RXCORE0_05_MASK						0x000000FC
#define LVDSTX_CKSELEN_TX0_SEN_AUTO							0x00
#define LVDSTX_CKSELEN_TX0_SEN_MANUAL						0x04
#define LVDSTX_CKSELEN_TX0_SEN_MASK							0x00000004
#define LVDSTX_CLOCK_TXCORE0_05_5_4							0x00
#define LVDSTX_CLOCK_TXCORE0_05_7_6_150MHZ_OR_LESS			0x00
#define LVDSTX_CLOCK_TXCORE0_05_7_6_150MHZ_300MHZ			0x40
#define LVDSTX_CLOCK_TXCORE0_05_7_6_OVER_300MHZ				0x80
#define LVDSTX_CLOCK_TXCORE0_05_7_4_MASK					0x000000F0
//------------------------------------------------------------------------------
// lvdstx_glue_setting()
#define LVDSTX_GLUE_LEN										4
#define LVDSTX_GLUE_NUM										2
#define LVDSTX_GLUE_GLUECTL0_CKMODE_150MHZ_OR_LESS			0x00
#define LVDSTX_GLUE_GLUECTL0_CKMODE_150MHZ_300MHZ			0x01
#define LVDSTX_GLUE_GLUECTL0_CKMODE_OVER_300MHZ				0x02
#define LVDSTX_GLUE_GLUECTL1_LNMODE_150MHZ_OR_LESS			0x00
#define LVDSTX_GLUE_GLUECTL1_LNMODE_150MHZ_300MHZ			0x01
#define LVDSTX_GLUE_GLUECTL1_LNMODE_OVER_300MHZ				0x02
#define LVDSTX_GLUE_GLUECTL1_1_SDMODE_DE_HALF				0x00
#define LVDSTX_GLUE_GLUECTL1_1_SDMODE_DE_SAME				0x04
#define LVDSTX_GLUE_GLUECTL1_1_SDMODE_REG_SETUP				0x08
#define LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_0					0x00
#define LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_1					0x01
#define LVDSTX_GLUE_GLUECTL1_1_SDMODE_MASK					0x0000000C
#define LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_MASK				0x00000003
#define LVDSTX_GLUE_GLUECTL8_SY_DLY_DEFAULT					0x00
#define LVDSTX_GLUE_GLUECTL8_SY_DLY_08						0x08
#define LVDSTX_GLUE_GLUECTL8_SY_DLY_MASK					0x000000FF
#define LVDSTX_GLUE_GLUECTLX_BKMODE_DIV_LINE_INTO_2BANK		0x04
#define LVDSTX_GLUE_GLUECTLX_BKMODE_NOT_USE_BANK_SWITCH		0x00
//------------------------------------------------------------------------------
// lvdstx_set_glue_polarity()
#define LVDSTX_SET_GLUE_POLARITY_NUM						1
#define LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_P					0x00
#define LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_N					0x80
#define LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_P					0x00
#define LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_N					0x40
#define LVDSTX_LVDVB_GLUE_GLUECTL3_XS_POL_MASK				0x000000C0
//------------------------------------------------------------------------------
// lvdstx_lvds_link_double_phase_output_setting()
#define LVDSTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN			4
#define LVDSTX_LVDS_LINK_LVDSCONFCTL1_ODEVPASS_X_SINGLE		0x08
//------------------------------------------------------------------------------
// lvdstx_lvds_link_vbo_output_setting()
#define LVDSTX_LVDS_LINK_VBO_OUTPUT_LEN						5
#define LVDSTX_LVDS_LINK_VBOOUTCTL_ALL_DISABLE				0x00
#define LVDSTX_LVDS_LINK_VBOPINASSIGNX_VBOASMODE_X_00		0x00
#define LVDSTX_LVDS_LINK_VBOPINASSIGNX_VBOBITMODE_X_8BIT	0x00
#define LVDSTX_LVDS_LINK_VBOPINASSIGNX_VBOPIXSWAP_X_OFF		0x00
//------------------------------------------------------------------------------
// lvdstx_lvds_link_lvds_output_enable_setting()
#define LVDSTX_LVDS_LINK_LVDS_OUTPUT_ENABLE_NUM				1
#define LVDSTX_LVDS_LINK_LVDSOUTCTL1_ALL_ENABLE				0x0F
#define LVDSTX_LVDS_LINK_LVDSOUTCTL1_MASK					0x000000FF
//------------------------------------------------------------------------------
// lvdstx_lvds_link_lvds_output_lane_setting()
#define LVDSTX_LVDS_LINK_LVDS_OUTPUT_LANE_LEN				2
#define LVDSTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T4LSEL_0X6		0xC0
#define LVDSTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T3LSEL_0X4		0x10
#define LVDSTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T2LSEL_0X2		0x40
#define LVDSTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T1LSEL_0X0		0x00
#define LVDSTX_LVDS_LINK_LVDSOUTCTL4_LVDSMODE_SEL_LVDS		0x01
//------------------------------------------------------------------------------
// lvdstx_lvds_link_lvds_pinsel_setting()
#define LVDSTX_LVDS_LINK_LVDS_PINSEL_NUM					4
#define LVDSTX_LVDSPINASSIGNX_LVASMODE_X_VESA8				0x00
#define LVDSTX_LVDSPINASSIGNX_LVASMODE_X_VESA10				0x01
#define LVDSTX_LVDSPINASSIGNX_LVASMODE_X_JEIDA8				0x02
#define LVDSTX_LVDSPINASSIGNX_LVASMODE_X_JEIDA10			0x03
#define LVDSTX_LVDSPINASSIGNX_LVASMODE_X_VESA_JEIDA			0x04
#define LVDSTX_LVDSPINASSIGNX_LVASMODE_X_MASK				0x00000007
//------------------------------------------------------------------------------
// lvdstx_vbo_link_link_data_path_setting()
#define LVDSTX_VBO_LINK_LINK_DATA_PATH_NUM					1
#define LVDSTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_LVDS			0x00
#define LVDSTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_MASK			0x000000FF
//------------------------------------------------------------------------------
// lvdstx_set_skew_param()
#define LVDSTX_SET_SKEW_PARAM_LEN							(24+7)
//------------------------------------------------------------------------------
// lvdstx_get_skew_param()
#define LVDSTX_GET_SKEW_PARAM_LEN							LVDSTX_SET_SKEW_PARAM_LEN
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
typedef enum _lvdstx_page {
	LVDSTX_PAGE_LVDS_LINK = 0,
	LVDSTX_PAGE_VBO_LINK,
	LVDSTX_PAGE_LVDSVBO_PHY,
	LVDSTX_PAGE_GLUE
} lvdstx_page;
//------------------------------------------------------------------------------
typedef enum _lvdstx_dc_test_mode {
	LVDSTX_DC_TEST_MODE_OFF = 0,
	LVDSTX_DC_TEST_MODE_ON
} lvdstx_dc_test_mode;
//------------------------------------------------------------------------------
typedef enum _lvdstx_vco_low_speed_oscillation {
	LVDSTX_VCO_LOW_SPEED_OSCILLATION_OFF = 0,
	LVDSTX_VCO_LOW_SPEED_OSCILLATION_ON
} lvdstx_vco_low_speed_oscillation;
//------------------------------------------------------------------------------
typedef enum _lvdstx_macro_reset {
	LVDSTX_MACRO_RESET = 0,
	LVDSTX_MACRO_RESET_RELEASE
} lvdstx_macro_reset;
//------------------------------------------------------------------------------
typedef enum _lvdstx_macro_power {
	LVDSTX_MACRO_POWER_DOWN = 0,
	LVDSTX_MACRO_POWER_ON
} lvdstx_macro_power;
//------------------------------------------------------------------------------
typedef enum _lvdstx_ctrl_req {
	LVDSTX_CTRL_REQ_GET,
	LVDSTX_CTRL_REQ_SET,
	LVDSTX_CTRL_REQ_MAX
} lvdstx_ctrl_req;
//------------------------------------------------------------------------------
static uint32_t lvdstx_select_page(system_port port, lvdstx_page page);
//------------------------------------------------------------------------------
static uint32_t lvdstx_init_setting(system_port port);
//------------------------------------------------------------------------------
static uint32_t lvdstx_pll_fraction_freq_dividing_setting(system_port port);
static uint32_t lvdstx_pll_analog_ctrl_setting(system_port port);
static uint32_t lvdstx_pll_digital_ctrl_setting(system_port port);
static uint32_t lvdstx_pll_reset_release_of_delta_sigma_circuit(system_port port);
static uint32_t lvdstx_pll_setting(system_port port);
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_clock_provision_to_lane_setting(system_port port);
static uint32_t lvdstx_phy_driver_output_amplitude_setting(system_port port);
static uint32_t lvdstx_phy_driver_output_common_mode_voltage_alignment_setting(system_port port);
static uint32_t lvdstx_phy_dc_test_mode_setting(system_port port, lvdstx_dc_test_mode mode);
static uint32_t lvdstx_phy_vco_low_speed_oscillation_setting(system_port port, lvdstx_vco_low_speed_oscillation vco);
static uint32_t lvdstx_phy_macro_ctrl_setting(system_port port, lvdstx_macro_reset reset, lvdstx_macro_power power);
static uint32_t lvdstx_phy_lane_enable_ctrl_setting(system_port port);
static uint32_t lvdstx_phy_setting(system_port port);
//------------------------------------------------------------------------------
static uint32_t lvdstx_path_setting(system_port port);
static uint32_t lvdstx_clock_setting(system_port port, lvdstx_video *p_video);
static uint32_t lvdstx_glue_setting(system_port port, lvdstx_video *p_video);
static uint32_t lvdstx_set_glue_polarity(system_port port, uint8_t pol);
static uint32_t lvdstx_hvpol_setting(system_port port, lvdstx_video *p_video);
static uint32_t lvdstx_lvds_link_double_phase_output_setting(system_port port);
static uint32_t lvdstx_lvds_link_vbo_output_setting(system_port port);
static uint32_t lvdstx_lvds_link_lvds_output_enable_setting(system_port port);
static uint32_t lvdstx_lvds_link_lvds_output_lane_setting(system_port port);
static uint32_t lvdstx_lvds_link_lvds_pinsel_setting(lvdstx_config *p_conf);
static uint32_t lvdstx_vbo_link_link_data_path_setting(system_port port);
static uint32_t lvdstx_lvds_link_setting(lvdstx_config *p_conf);
static uint32_t lvdstx_tx_link_setting(lvdstx_video *p_video);
//------------------------------------------------------------------------------
static uint32_t lvdstx_ctrl_req_abort(system_request *p_req, lvdstx_ctrl_req ctrl);
static system_request lvdstx_ctrl_get_abort_req(void);
static void lvdstx_ctrl_set_abort_req(system_request req);
//------------------------------------------------------------------------------
// abort request check
#define CHECK_ABORT() {												\
	if (lvdstx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		lvdstx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_LVDSTX_ABORT;									\
	}																\
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_select_page(system_port port, lvdstx_page page)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PAGE_SELECT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, LVDSTX_PAGE_SELECT_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSLINKPAGESEL_ADDR, page, LVDSTX_PAGE_SELECT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);
	
	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_init_setting(system_port port)
{
	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_pll_fraction_freq_dividing_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_PLL_SSC_FFD_SETTING_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_SSCCNT1_0_ADDR,
						LVDSTX_PLL_SSC_FFD_SETTING_LEN, data);
	// 148.5MHz (HS-LVDS, 1.04Gbps/lane)
	// MIN
	SET_BLOCK_DATA(data, 0, LVDSTX_PLL_SSCCNT1_0_MIN_0X26);		// 0x34E0
	// KIN 0
	SET_BLOCK_DATA(data, 1, LVDSTX_PLL_SSCCNT1_1_KIN_0_0X00);	// 0x34E1
	// KIN 1
	SET_BLOCK_DATA(data, 2, LVDSTX_PLL_SSCCNT1_2_KIN_1_0X00);	// 0x34E2
	// KIN 2
	SET_BLOCK_DATA(data, 3, LVDSTX_PLL_SSCCNT1_3_KIN_2_0X08);	// 0x34E3
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_pll_analog_ctrl_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_PLL_ANALOG_CTRL_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_PLLANACNT_0_ADDR,
						LVDSTX_PLL_ANALOG_CTRL_LEN, data);
	// REG1: 1.81GHz/V
	// REG2: Irefx 13/20
	SET_BLOCK_DATA(data, 0, LVDSTX_PLL_PLLANACNT_0_REG1_181 |
								LVDSTX_PLL_PLLANACNT_0_REG2_IREFX13);			// 0x34C8
	// REG2: Irefx 13/20
	// REG3: filter resistance: 3
	SET_BLOCK_DATA(data, 1, LVDSTX_PLL_PLLANACNT_1_REG2_IREFX13 |
								LVDSTX_PLL_PLLANACNT_1_REG3_FR3);				// 0x34C9
	// REG4: filter resistance: 3
	// VCOLMT: 0b100
	SET_BLOCK_DATA(data, 2, LVDSTX_PLL_PLLANACNT_2_REG4_FR3 |
								LVDSTX_PLL_PLLANACNT_2_VCOLMT_0B100);			// 0x34CA
	// DIVL: 0: normal state
	// CP2EN: 0: disable halving filter capacitance
	// CP2EN2: 0: 1 charge pump use
	SET_BLOCK_DATA(data, 3, LVDSTX_PLL_PLLANACNT_3_DIVL_NORMAL               |
								LVDSTX_PLL_PLLANACNT_3_CP2EN_DISABLE_HALVING |
								LVDSTX_PLL_PLLANACNT_3_CP2EN2_1CHARGE_PUMP);	// 0x34CB
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_pll_digital_ctrl_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_PLL_DIGITAL_CTRL_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_PLLDIGCNT_0_ADDR,
						LVDSTX_PLL_DIGITAL_CTRL_LEN, data);
	// FIDIV: division ratio: 1
	// PDIV: division ratio: 2
	SET_BLOCK_DATA(data, 0, LVDSTX_PLL_PLLDIGCNT_0_FIDIV_DR1 |
								LVDSTX_PLL_PLLDIGCNT_0_PDIV_DR2);	// 0x34CC
	// QDIV: division ratio: 1
	SET_BLOCK_DATA(data, 1, LVDSTX_PLL_PLLDIGCNT_1_QDIV_DR1);		// 0x34CD
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_pll_reset_release_of_delta_sigma_circuit(system_port port)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PLL_SSC_RESET_CTRL_DELTA_SIGMA_NUM];
	uint32_t ret;

	// NOTE: when 148.5MHz (HS-LVDS, 1.04Gbps/lane), the following setting is necessary.
	//       when 135MHz (HS-LVDS, 0.945Gbps/lane), the following setting is NOT necessary.

	// page: 2

	// reset release of delta sigma circuit
	SET_ACCESS(access, &port, cmd, LVDSTX_PLL_SSC_RESET_CTRL_DELTA_SIGMA_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_SSCCNT2_3_ADDR,
						LVDSTX_PLL_SSCCNT2_3_NRSTDS_RESET_RELEASE,
						LVDSTX_PLL_SSCCNT2_3_NRSTDS_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SLEEP(LVDSTX_SLEEP_TIME_20MSEC);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_pll_setting(system_port port)
{
	uint32_t ret;

	// page: 2

	ret = lvdstx_select_page(port, LVDSTX_PAGE_LVDSVBO_PHY);
	CHECK_ERROR(ret);
	// fraction frequency dividing of PLL setting
	ret = lvdstx_pll_fraction_freq_dividing_setting(port);
	CHECK_ERROR(ret);
	// PLL analog control setting
	ret = lvdstx_pll_analog_ctrl_setting(port);
	CHECK_ERROR(ret);
	// PLL digital control setting
	ret = lvdstx_pll_digital_ctrl_setting(port);
	CHECK_ERROR(ret);
	// reset release of delta-sigma circuit
	ret = lvdstx_pll_reset_release_of_delta_sigma_circuit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_clock_provision_to_lane_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PHY_CLOCK_PROVISION_NUM];
	uint32_t ret;

	// page: 2

	// clock provision: lane 0-23
	SET_ACCESS(access, &port, cmd, LVDSTX_PHY_CLOCK_PROVISION_NUM);
	// 0x34D0
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_PHYRSV_ADDR,
						LVDSTX_PHY_PHYRSV_RSV_LANE0_23,
						LVDSTX_PHY_PHYRSV_RSV_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_driver_output_amplitude_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN];
	uint32_t ret;
	uint32_t i;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_DRVAMP1_0_ADDR,
						LVDSTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN, data);
	// driver output amplitude: 200 mVp-p (for LVDS)
	for (i=0; i<LVDSTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LEN; i++) {
		SET_BLOCK_DATA(data, i, LVDSTX_PHY_DRIVER_OUTPUT_AMPLITUDE_LVDS_200);	// 0x3484-0x348F
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_driver_output_common_mode_voltage_alignment_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM];
	uint32_t ret;

	// page: 2

	SET_ACCESS(access, &port, cmd, LVDSTX_PHY_DRIVER_OUTPUT_COMMON_MODE_VOLTAGE_NUM);
	// NVCOM080: 1
	// NVCOM120: 1
	// NVCOM125: 0
	// NPORTEN:  1
	// LVDSEN:   1
	// 0x34C1
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_1_ADDR,
						LVDSTX_PHY_DRVCNT_1_NVCOM080_1     |
							LVDSTX_PHY_DRVCNT_1_NVCOM120_1 |
							LVDSTX_PHY_DRVCNT_1_NVCOM125_0 |
							LVDSTX_PHY_DRVCNT_1_NPORTEN_1  |
							LVDSTX_PHY_DRVCNT_1_LVDSEN_1,
						LVDSTX_PHY_DRVCNT_1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_dc_test_mode_setting(system_port port, lvdstx_dc_test_mode mode)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PHY_DC_TEST_MODE_NUM];
	uint32_t ret;

	// page: 2

	SET_ACCESS(access, &port, cmd, LVDSTX_PHY_DC_TEST_MODE_NUM);
	// 0x34C3
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_DRVCNT_3_ADDR,
						(uint8_t)mode << LVDSTX_PHY_DRVCNT_3_DRVMEN_BIT,
						LVDSTX_PHY_DRVCNT_3_DRVMEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_vco_low_speed_oscillation_setting(system_port port, lvdstx_vco_low_speed_oscillation vco)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PHY_VCO_LOW_SPEED_OSCILLATION_NUM];
	uint32_t ret;

	// page: 2

	SET_ACCESS(access, &port, cmd, LVDSTX_PHY_VCO_LOW_SPEED_OSCILLATION_NUM);
	// 0x34CD
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_PLLDIGCNT_1_ADDR,
						(uint8_t)vco << LVDSTX_PHY_PLLDIGCNT_1_OFFSETON_BIT,
						LVDSTX_PHY_PLLDIGCNT_1_OFFSETON_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_macro_ctrl_setting(system_port port, lvdstx_macro_reset reset, lvdstx_macro_power power)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PHY_MACRO_CTRL_NUM];
	uint32_t ret;

	// page: 2

	SET_ACCESS(access, &port, cmd, LVDSTX_PHY_MACRO_CTRL_NUM);
	// 0x3483
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_PHY_ENTCNT_3_ADDR,
						(uint8_t)reset << LVDSTX_PHY_ENTCNT_3_NRESET_BIT |
							(uint8_t)power << LVDSTX_PHY_ENTCNT_3_NPDOWN_BIT,
						LVDSTX_PHY_ENTCNT_3_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	SLEEP(LVDSTX_SLEEP_TIME_1MSEC);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_lane_enable_ctrl_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_PHY_LANE_ENABLE_CTRL_LEN];
	uint32_t ret;

	// page: 2

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_PHY_ENTCNT_0_ADDR,
						LVDSTX_PHY_LANE_ENABLE_CTRL_LEN, data);
	// lane 0-7: enable
	SET_BLOCK_DATA(data, 0, LVDSTX_PHY_ENTCNT_0_LANE0_7_ENABLE);		// 0x3480
	// lane 8-15: enable
	SET_BLOCK_DATA(data, 1, LVDSTX_PHY_ENTCNT_1_LANE8_15_ENABLE);		// 0x3481
	// lane 16-23: enable
	SET_BLOCK_DATA(data, 2, LVDSTX_PHY_ENTCNT_2_LANE16_23_ENABLE);		// 0x3482
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_phy_setting(system_port port)
{
	uint32_t ret;

	// page: 2

	ret = lvdstx_select_page(port, LVDSTX_PAGE_LVDSVBO_PHY);
	CHECK_ERROR(ret);
	// clock provision: lane 0-23
	ret = lvdstx_phy_clock_provision_to_lane_setting(port);
	CHECK_ERROR(ret);
	// driver output amplitude setting in every lane
	ret = lvdstx_phy_driver_output_amplitude_setting(port);
	CHECK_ERROR(ret);
	// driver output common mode voltage alignment
	ret = lvdstx_phy_driver_output_common_mode_voltage_alignment_setting(port);
	CHECK_ERROR(ret);
	// DC test mode: ON
	ret = lvdstx_phy_dc_test_mode_setting(port, LVDSTX_DC_TEST_MODE_ON);
	CHECK_ERROR(ret);
	// VCO low speed oscillation: ON
	ret = lvdstx_phy_vco_low_speed_oscillation_setting(port, LVDSTX_VCO_LOW_SPEED_OSCILLATION_ON);
	CHECK_ERROR(ret);
	// LVDS macro reset control: reset release
	// LVDS macro power control: power on
	ret = lvdstx_phy_macro_ctrl_setting(port, LVDSTX_MACRO_RESET_RELEASE, LVDSTX_MACRO_POWER_ON);
	CHECK_ERROR(ret);
	// lane enable control: enable 24lane (0-23)
	ret = lvdstx_phy_lane_enable_ctrl_setting(port);
	CHECK_ERROR(ret);
	// DC test mode: OFF
	ret = lvdstx_phy_dc_test_mode_setting(port, LVDSTX_DC_TEST_MODE_OFF);
	CHECK_ERROR(ret);
	// VCO low speed oscillation: OFF
	ret = lvdstx_phy_vco_low_speed_oscillation_setting(port, LVDSTX_VCO_LOW_SPEED_OSCILLATION_OFF);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_path_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[LVDSTX_PATH_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, LVDSTX_PATH_NUM);
	// LVBON: 0b1 (enable)
	// LVDSEN: 0b1 (LVDS)
	// 0x10B1
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE4_01_ADDR,
						LVDSTX_PATH_SWMODE4_01_LVBON_ENABLE |
							LVDSTX_PATH_SWMODE4_01_LVDSEN_LVDS,
						LVDSTX_PATH_SWMODE4_01_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_clock_setting(system_port port, lvdstx_video *p_video)
{
	system_reg_access access;
	system_command cmd[LVDSTX_CLOCK_NUM];
	system_ctrl_lvds_link cnt;
	uint8_t tx0_sen;
	uint8_t txcore0_05_7_6;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	ret = system_ctrl_get_lvds_link_count(p_video->tx_video_format, &cnt);
	CHECK_ERROR(ret);
	switch (cnt) {
		case SYSTEM_CTRL_LVDS_LINK_1:
			tx0_sen = LVDSTX_CKSELEN_TX0_SEN_MANUAL;
			txcore0_05_7_6 = LVDSTX_CLOCK_TXCORE0_05_7_6_150MHZ_OR_LESS;
			break;
		case SYSTEM_CTRL_LVDS_LINK_2:
			tx0_sen = LVDSTX_CKSELEN_TX0_SEN_MANUAL;
			txcore0_05_7_6 = LVDSTX_CLOCK_TXCORE0_05_7_6_150MHZ_300MHZ;
			break;
		case SYSTEM_CTRL_LVDS_LINK_4:
			tx0_sen = LVDSTX_CKSELEN_TX0_SEN_AUTO;
			txcore0_05_7_6 = LVDSTX_CLOCK_TXCORE0_05_7_6_OVER_300MHZ;
			break;
		default:
			return ERR_LVDSTX_TRANSMIT_FORMAT;
	}

	SET_ACCESS(access, &port, cmd, LVDSTX_CLOCK_NUM);
	// 0x1E6A
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXCORE0_05_ADDR,
						LVDSTX_CLOCK_RXCORE0_05_3_2
						| LVDSTX_CLOCK_RXCORE0_05_5_4
						| LVDSTX_CLOCK_RXCORE0_05_7_6,
						LVDSTX_CLOCK_RXCORE0_05_MASK);
	// 0x1E7F
	SET_COMMAND(cmd, 1, VIDEOIF_CKGEN_CKSELEN_ADDR,
						tx0_sen,
						LVDSTX_CKSELEN_TX0_SEN_MASK);
	// 0x1E70
	SET_COMMAND(cmd, 2, VIDEOIF_CKGEN_TXCORE0_05_ADDR,
						LVDSTX_CLOCK_TXCORE0_05_5_4 | txcore0_05_7_6,
						LVDSTX_CLOCK_TXCORE0_05_7_4_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_glue_setting(system_port port, lvdstx_video *p_video)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_GLUE_LEN];
	system_reg_access access;
	system_command cmd[LVDSTX_GLUE_NUM];
	system_ctrl_lvds_link cnt;
	uint8_t ckmode, lnmode;
	uint8_t hstart70, hstart98;
	uint8_t sdmode, line_dly, bkmode, sy_dly;
	uint32_t pf, link;
	uint32_t ret;

	ret = system_ctrl_get_lvds_link_count(p_video->tx_video_format, &cnt);
	CHECK_ERROR(ret);
	switch (cnt) {
		case SYSTEM_CTRL_LVDS_LINK_1:
			ckmode = LVDSTX_GLUE_GLUECTL0_CKMODE_150MHZ_OR_LESS;
			lnmode = LVDSTX_GLUE_GLUECTL1_LNMODE_150MHZ_OR_LESS;
			link = 1;
			break;
		case SYSTEM_CTRL_LVDS_LINK_2:
			ckmode = LVDSTX_GLUE_GLUECTL0_CKMODE_150MHZ_300MHZ;
			lnmode = LVDSTX_GLUE_GLUECTL1_LNMODE_150MHZ_300MHZ;
			link = 2;
			break;
		case SYSTEM_CTRL_LVDS_LINK_4:
			ckmode = LVDSTX_GLUE_GLUECTL0_CKMODE_OVER_300MHZ;
			lnmode = LVDSTX_GLUE_GLUECTL1_LNMODE_OVER_300MHZ;
			link = 4;
			break;
		default:
			return ERR_LVDSTX_TRANSMIT_FORMAT;
	}
	ret = system_ctrl_get_tx_glue_hstart(port, p_video->tx_video_format,
										SYSTEM_CTRL_LVDS_FREQ_148500KHZ, &hstart70, &hstart98);
	CHECK_ERROR(ret);
	ret = system_ctrl_get_pixel_freq(p_video->tx_video_format, &pf);
	CHECK_ERROR(ret);
	if (pf == (SYSTEM_CTRL_LVDS_FREQ_148500KHZ * link)) {
		sdmode = LVDSTX_GLUE_GLUECTL1_1_SDMODE_DE_HALF;
		line_dly = LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_0;
		sy_dly = LVDSTX_GLUE_GLUECTL8_SY_DLY_DEFAULT;
	} else {
		// only when 720x576P, set 0x08 to sy_dly
		if (p_video->tx_video_format == SYSTEM_VFORM_720x576P_50_4v3
			|| p_video->tx_video_format == SYSTEM_VFORM_720x576P_50_16v9) {
			sdmode = LVDSTX_GLUE_GLUECTL1_1_SDMODE_REG_SETUP;
			line_dly = LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_1;
			sy_dly = LVDSTX_GLUE_GLUECTL8_SY_DLY_08;
		} else {
			sdmode = LVDSTX_GLUE_GLUECTL1_1_SDMODE_DE_SAME;
			line_dly = LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_1;
			sy_dly = LVDSTX_GLUE_GLUECTL8_SY_DLY_DEFAULT;
		}
	}
	if (sdmode && line_dly) {
		bkmode = LVDSTX_GLUE_GLUECTLX_BKMODE_DIV_LINE_INTO_2BANK;
	} else {
		bkmode = LVDSTX_GLUE_GLUECTLX_BKMODE_NOT_USE_BANK_SWITCH;
	}

	// page: 3

	ret = lvdstx_select_page(port, LVDSTX_PAGE_GLUE);
	CHECK_ERROR(ret);
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDVB_GLUE_GLUECTL0_ADDR,
						LVDSTX_GLUE_LEN, data);
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

	SET_ACCESS(access, &port, cmd, LVDSTX_GLUE_NUM);
	// 0x3488
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_GLUE_GLUECTL8_ADDR,
						sy_dly,
						LVDSTX_GLUE_GLUECTL8_SY_DLY_MASK);
	// 0x348E
	SET_COMMAND(cmd, 1, VIDEOIF_LVDVB_GLUE_GLUECTL1_1_ADDR,
						sdmode
						| line_dly,
						LVDSTX_GLUE_GLUECTL1_1_SDMODE_MASK
						| LVDSTX_GLUE_GLUECTL1_1_LINE_DLY_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_set_glue_polarity(system_port port, uint8_t pol)
{
	system_reg_access access;
	system_command cmd[LVDSTX_SET_GLUE_POLARITY_NUM];
	uint32_t ret;

	// page: 3

	SET_ACCESS(access, &port, cmd, LVDSTX_SET_GLUE_POLARITY_NUM);
	// vs_pol,hs_pol: 0:Positive, 1:Negative
	// 0x3483
	SET_COMMAND(cmd, 0, VIDEOIF_LVDVB_GLUE_GLUECTL3_ADDR,
						pol,
						LVDSTX_LVDVB_GLUE_GLUECTL3_XS_POL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_hvpol_setting(system_port port, lvdstx_video *p_video)
{
	system_ctrl_polarity hpol, vpol;
	uint8_t hs_pol, vs_pol;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_HDMI) {
		ret = system_ctrl_get_hdmirx_sync_polarity(port, &hpol, &vpol);
		CHECK_ERROR(ret);
		// hs_pol: 0:Positive, 1:Negative
		hs_pol = (hpol == SYSTEM_CTRL_POL_P)
					? LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_P
					: LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_N;
		// vs_pol: 0:Positive, 1:Negative
		vs_pol = (vpol == SYSTEM_CTRL_POL_P)
					? LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_P
					: LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_N;
	} else if (p_video->rx_port.port_type == SYSTEM_PORT_TYPE_VBO) {
		// hs_pol: 0:Positive, 1:Negative
		hs_pol = LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_P;
		// vs_pol: 0:Positive, 1:Negative
		vs_pol = LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_P;
	} else {
		ret = system_ctrl_get_sync_polarity(p_video->rx_video_format, &hpol, &vpol);
		CHECK_ERROR(ret);
		// hs_pol: 0:Positive, 1:Negative
		hs_pol = (hpol == SYSTEM_CTRL_POL_P)
					? LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_P
					: LVDSTX_LVDVB_GLUE_GLUECTL3_HS_POL_N;
		// vs_pol: 0:Positive, 1:Negative
		vs_pol = (vpol == SYSTEM_CTRL_POL_P)
					? LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_P
					: LVDSTX_LVDVB_GLUE_GLUECTL3_VS_POL_N;
	}

	// page: 3

	ret = lvdstx_select_page(port, LVDSTX_PAGE_GLUE);
	CHECK_ERROR(ret);
	ret = lvdstx_set_glue_polarity(port, (hs_pol | vs_pol));
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_lvds_link_double_phase_output_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN];
	uint32_t ret;
	uint32_t i;

	// page: 0

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDS_LINK_LVDSCONFCTL1_ADDR,
						LVDSTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN, data);
	// disable (single phase outpu)
	// 0x3480-0x3483
	for (i=0; i<LVDSTX_LVDS_LINK_DOUBLE_PHASE_OUTPUT_LEN; i++) {
		SET_BLOCK_DATA(data, i, LVDSTX_LVDS_LINK_LVDSCONFCTL1_ODEVPASS_X_SINGLE);
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_lvds_link_vbo_output_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_LVDS_LINK_VBO_OUTPUT_LEN];
	uint32_t ret;
	uint32_t i;

	// page: 0

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDS_LINK_VBOOUTCTL_ADDR,
						LVDSTX_LVDS_LINK_VBO_OUTPUT_LEN, data);
	// VBO output disable setting
	// 0x34A0
	SET_BLOCK_DATA(data, 0, LVDSTX_LVDS_LINK_VBOOUTCTL_ALL_DISABLE);
	// VBO pinsel setting
	//   pin assignment mode: 0b000
	//   bit number:          0b00
	//   msb-lsb order swap:  0b0
	// 0x34A1-0x34A4
	for (i=1; i<LVDSTX_LVDS_LINK_VBO_OUTPUT_LEN; i++) {
		SET_BLOCK_DATA(data, i, LVDSTX_LVDS_LINK_VBOPINASSIGNX_VBOASMODE_X_00        |
									LVDSTX_LVDS_LINK_VBOPINASSIGNX_VBOBITMODE_X_8BIT |
									LVDSTX_LVDS_LINK_VBOPINASSIGNX_VBOPIXSWAP_X_OFF);
	}
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_lvds_link_lvds_output_enable_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[LVDSTX_LVDS_LINK_LVDS_OUTPUT_ENABLE_NUM];
	uint32_t ret;

	// page: 0

	SET_ACCESS(access, &port, cmd, LVDSTX_LVDS_LINK_LVDS_OUTPUT_ENABLE_NUM);
	// data enable
	// 0x3484
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSOUTCTL1_ADDR,
						LVDSTX_LVDS_LINK_LVDSOUTCTL1_ALL_ENABLE,
						LVDSTX_LVDS_LINK_LVDSOUTCTL1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_lvds_link_lvds_output_lane_setting(system_port port)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_LVDS_LINK_LVDS_OUTPUT_LANE_LEN];
	uint32_t ret;

	// page: 0

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_LVDS_LINK_LVDSOUTCTL3_ADDR,
						LVDSTX_LVDS_LINK_LVDS_OUTPUT_LANE_LEN, data);
	// 0x3486
	SET_BLOCK_DATA(data, 0, LVDSTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T4LSEL_0X6     |
								LVDSTX_LVDS_LINK_LVDSOUTCTL3_LVDS_T3LSEL_0X4);
	// 0x3487
	SET_BLOCK_DATA(data, 1, LVDSTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T2LSEL_0X2     |
								LVDSTX_LVDS_LINK_LVDSOUTCTL4_LVDS_T1LSEL_0X0 |
								LVDSTX_LVDS_LINK_LVDSOUTCTL4_LVDSMODE_SEL_LVDS);
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_lvds_link_lvds_pinsel_setting(lvdstx_config *p_conf)
{
	system_reg_access access;
	system_command cmd[LVDSTX_LVDS_LINK_LVDS_PINSEL_NUM];
	uint8_t format;
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// pin assignment mode
	switch (p_conf->format) {
		case LVDSTX_DATA_FORMAT_VESA_8BIT:							// VESA 8bit
			format = LVDSTX_LVDSPINASSIGNX_LVASMODE_X_VESA8;
			break;
		case LVDSTX_DATA_FORMAT_VESA_10BIT:							// VESA 10bit
			format = LVDSTX_LVDSPINASSIGNX_LVASMODE_X_VESA10;
			break;
		case LVDSTX_DATA_FORMAT_JEIDA_8BIT:							// JEIDA 8bit
			format = LVDSTX_LVDSPINASSIGNX_LVASMODE_X_JEIDA8;
			break;
		case LVDSTX_DATA_FORMAT_JEIDA_10BIT:						// JEIDA 10bit
			format = LVDSTX_LVDSPINASSIGNX_LVASMODE_X_JEIDA10;
			break;
		case LVDSTX_DATA_FORMAT_VESA_JEIDA_COMPOSITE:				// VESA/JEIDA composite
			format = LVDSTX_LVDSPINASSIGNX_LVASMODE_X_VESA_JEIDA;
			break;
		default:
			return ERR_NG;
	}

	// page: 0

	SET_ACCESS(access, &(p_conf->port), cmd, LVDSTX_LVDS_LINK_LVDS_PINSEL_NUM);
	// 1st phase
	// 0x3488
	SET_COMMAND(cmd, 0, VIDEOIF_LVDS_LINK_LVDSPINASSIGN1_ADDR,
						format,
						LVDSTX_LVDSPINASSIGNX_LVASMODE_X_MASK);
	// 2nd phase
	// 0x348E
	SET_COMMAND(cmd, 1, VIDEOIF_LVDS_LINK_LVDSPINASSIGN2_ADDR,
						format,
						LVDSTX_LVDSPINASSIGNX_LVASMODE_X_MASK);
	// 3rd phase
	// 0x3494
	SET_COMMAND(cmd, 2, VIDEOIF_LVDS_LINK_LVDSPINASSIGN3_ADDR,
						format,
						LVDSTX_LVDSPINASSIGNX_LVASMODE_X_MASK);
	// 4th phase
	// 0x349A
	SET_COMMAND(cmd, 3, VIDEOIF_LVDS_LINK_LVDSPINASSIGN4_ADDR,
						format,
						LVDSTX_LVDSPINASSIGNX_LVASMODE_X_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_vbo_link_link_data_path_setting(system_port port)
{
	system_reg_access access;
	system_command cmd[LVDSTX_VBO_LINK_LINK_DATA_PATH_NUM];
	uint32_t ret;

	// page: 1

	SET_ACCESS(access, &port, cmd, LVDSTX_VBO_LINK_LINK_DATA_PATH_NUM);
	// 0x3496
	SET_COMMAND(cmd, 0, VIDEOIF_VBO_LINK_VBOCTL22_ADDR,
						LVDSTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_LVDS,
						LVDSTX_VBO_LINK_VBOCTL22_LVDSVBOSEL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_lvds_link_setting(lvdstx_config *p_conf)
{
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// page: 0

	ret = lvdstx_select_page(p_conf->port, LVDSTX_PAGE_LVDS_LINK);
	CHECK_ERROR(ret);
	// double phase output setting
	ret = lvdstx_lvds_link_double_phase_output_setting(p_conf->port);
	CHECK_ERROR(ret);
	// VBO output setting
	ret = lvdstx_lvds_link_vbo_output_setting(p_conf->port);
	CHECK_ERROR(ret);
	// LVDS output enable setting
	ret = lvdstx_lvds_link_lvds_output_enable_setting(p_conf->port);
	CHECK_ERROR(ret);
	// LVDS output lane setting
	ret = lvdstx_lvds_link_lvds_output_lane_setting(p_conf->port);
	CHECK_ERROR(ret);
	// LVDS pinsel setting
	ret = lvdstx_lvds_link_lvds_pinsel_setting(p_conf);
	CHECK_ERROR(ret);

	// page: 1

	ret = lvdstx_select_page(p_conf->port, LVDSTX_PAGE_VBO_LINK);
	CHECK_ERROR(ret);
	// LVDS link data path setting
	ret = lvdstx_vbo_link_link_data_path_setting(p_conf->port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t lvdstx_tx_link_setting(lvdstx_video *p_video)
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
static uint32_t lvdstx_ctrl_req_abort(system_request *p_req, lvdstx_ctrl_req ctrl)
{
	static system_request lvdstx_abort_req = SYSTEM_REQUEST_NONE;

	if (p_req == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == LVDSTX_CTRL_REQ_GET) {
		*p_req =lvdstx_abort_req ;
	} else if (ctrl == LVDSTX_CTRL_REQ_SET) {
		lvdstx_abort_req = *p_req;
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static system_request lvdstx_ctrl_get_abort_req(void)
{
	system_request req;

	if (lvdstx_ctrl_req_abort(&req, LVDSTX_CTRL_REQ_GET)) {
		return SYSTEM_REQUEST_NONE;
	}

	return req;
}
//------------------------------------------------------------------------------
static void lvdstx_ctrl_set_abort_req(system_request req)
{
	if (lvdstx_ctrl_req_abort(&req, LVDSTX_CTRL_REQ_SET)) {
		return;
	}
}
//------------------------------------------------------------------------------
uint32_t lvdstx_set_video_format_wo_tx_link(lvdstx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_lvdstx_init(p_video->tx_port);
	CHECK_ERROR(ret);
	// clock setting
	ret = lvdstx_clock_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// glue setting
	ret = lvdstx_glue_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// hpol, vpol, setting
	ret = lvdstx_hvpol_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// Tx link setting
	//
	//

	return ret;
}
//------------------------------------------------------------------------------
uint32_t lvdstx_init(lvdstx_config *p_conf)
{
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// system control initialization
	ret = system_ctrl_init(p_conf->port);
	CHECK_ERROR(ret);
	// initial setting
	ret = lvdstx_init_setting(p_conf->port);
	CHECK_ERROR(ret);
	// PLL setting
	ret = lvdstx_pll_setting(p_conf->port);
	CHECK_ERROR(ret);
	// PHY setting
	ret = lvdstx_phy_setting(p_conf->port);
	CHECK_ERROR(ret);
	// path setting
	ret = lvdstx_path_setting(p_conf->port);
	CHECK_ERROR(ret);
	// LVDS link setting
	ret = lvdstx_lvds_link_setting(p_conf);
	CHECK_ERROR(ret);
	// init abort request
	lvdstx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t lvdstx_exit(system_port port)
{
	uint32_t ret;

	// System control exit
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t lvdstx_set_video_format(lvdstx_video *p_video)
{
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_lvdstx_init(p_video->tx_port);
	CHECK_ERROR(ret);
	// clock setting
	ret = lvdstx_clock_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// glue setting
	ret = lvdstx_glue_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// hpol, vpol, setting
	ret = lvdstx_hvpol_setting(p_video->tx_port, p_video);
	CHECK_ERROR(ret);
	// Tx link setting
	ret = lvdstx_tx_link_setting(p_video);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t lvdstx_video_mute(lvdstx_mute *p_mute)
{
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_lvdstx_init(p_mute->port);
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
uint32_t lvdstx_set_skew_param(lvdstx_skew_param *p_skew)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_SET_SKEW_PARAM_LEN];
	uint32_t ret;

	if (p_skew == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_lvdstx_init(p_skew->port);
	CHECK_ERROR(ret);

	// page: 2

	ret = lvdstx_select_page(p_skew->port, LVDSTX_PAGE_LVDSVBO_PHY);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &(p_skew->port), VIDEOIF_LVDVB_PHY_SKEWADJ1A_A_ADDR,
						LVDSTX_SET_SKEW_PARAM_LEN, data);
	SET_BLOCK_DATA(data,  0, p_skew->ch0.lane_a);		// 0x34A0
	SET_BLOCK_DATA(data,  1, p_skew->ch0.lane_b);		// 0x34A1
	SET_BLOCK_DATA(data,  2, p_skew->ch0.lane_c);		// 0x34A2
	SET_BLOCK_DATA(data,  3, 0);						// 0x34A3
	SET_BLOCK_DATA(data,  4, p_skew->ch0.clk);			// 0x34A4
	SET_BLOCK_DATA(data,  5, p_skew->ch0.lane_d);		// 0x34A5
	SET_BLOCK_DATA(data,  6, p_skew->ch0.lane_e);		// 0x34A6
	SET_BLOCK_DATA(data,  7, 0);						// 0x34A7
	SET_BLOCK_DATA(data,  8, p_skew->ch1.lane_a);		// 0x34A8
	SET_BLOCK_DATA(data,  9, p_skew->ch1.lane_b);		// 0x34A9
	SET_BLOCK_DATA(data, 10, p_skew->ch1.lane_c);		// 0x34AA
	SET_BLOCK_DATA(data, 11, 0);						// 0x34AB
	SET_BLOCK_DATA(data, 12, p_skew->ch1.clk);			// 0x34AC
	SET_BLOCK_DATA(data, 13, p_skew->ch1.lane_d);		// 0x34AD
	SET_BLOCK_DATA(data, 14, p_skew->ch1.lane_e);		// 0x34AE
	SET_BLOCK_DATA(data, 15, 0);						// 0x34AF
	SET_BLOCK_DATA(data, 16, p_skew->ch2.lane_a);		// 0x34B0
	SET_BLOCK_DATA(data, 17, p_skew->ch2.lane_b);		// 0x34B1
	SET_BLOCK_DATA(data, 18, p_skew->ch2.lane_c);		// 0x34B2
	SET_BLOCK_DATA(data, 19, 0);						// 0x34B3
	SET_BLOCK_DATA(data, 20, p_skew->ch2.clk);			// 0x34B4
	SET_BLOCK_DATA(data, 21, p_skew->ch2.lane_d);		// 0x34B5
	SET_BLOCK_DATA(data, 22, p_skew->ch2.lane_e);		// 0x34B6
	SET_BLOCK_DATA(data, 23, 0);						// 0x34B7
	SET_BLOCK_DATA(data, 24, p_skew->ch3.lane_a);		// 0x34B8
	SET_BLOCK_DATA(data, 25, p_skew->ch3.lane_b);		// 0x34B9
	SET_BLOCK_DATA(data, 26, p_skew->ch3.lane_c);		// 0x34BA
	SET_BLOCK_DATA(data, 27, 0);						// 0x34BB
	SET_BLOCK_DATA(data, 28, p_skew->ch3.clk);			// 0x34BC
	SET_BLOCK_DATA(data, 29, p_skew->ch3.lane_d);		// 0x34BD
	SET_BLOCK_DATA(data, 30, p_skew->ch3.lane_e);		// 0x34BE
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t lvdstx_get_skew_param(lvdstx_skew_param *p_skew)
{
	system_block_access baccess;
	uint8_t data[LVDSTX_GET_SKEW_PARAM_LEN] = {0};
	uint32_t ret;

	if (p_skew == NULL) {
		return ERR_NULL_POINTER;
	}
	// check init
	ret = system_ctrl_check_lvdstx_init(p_skew->port);
	CHECK_ERROR(ret);

	// page: 2

	ret = lvdstx_select_page(p_skew->port, LVDSTX_PAGE_LVDSVBO_PHY);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &(p_skew->port), VIDEOIF_LVDVB_PHY_SKEWADJ1A_A_ADDR,
						LVDSTX_GET_SKEW_PARAM_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	p_skew->ch0.lane_a = data[ 0];			// 0x34A0
	p_skew->ch0.lane_b = data[ 1];			// 0x34A1
	p_skew->ch0.lane_c = data[ 2];			// 0x34A2
											// 0x34A3
	p_skew->ch0.clk    = data[ 4];			// 0x34A4
	p_skew->ch0.lane_d = data[ 5];			// 0x34A5
	p_skew->ch0.lane_e = data[ 6];			// 0x34A6
											// 0x34A7
	p_skew->ch1.lane_a = data[ 8];			// 0x34A8
	p_skew->ch1.lane_b = data[ 9];			// 0x34A9
	p_skew->ch1.lane_c = data[10];			// 0x34AA
											// 0x34AB
	p_skew->ch1.clk    = data[12];			// 0x34AC
	p_skew->ch1.lane_d = data[13];			// 0x34AD
	p_skew->ch1.lane_e = data[14];			// 0x34AE
											// 0x34AF
	p_skew->ch2.lane_a = data[16];			// 0x34B0
	p_skew->ch2.lane_b = data[17];			// 0x34B1
	p_skew->ch2.lane_c = data[18];			// 0x34B2
											// 0x34B3
	p_skew->ch2.clk    = data[20];			// 0x34B4
	p_skew->ch2.lane_d = data[21];			// 0x34B5
	p_skew->ch2.lane_e = data[22];			// 0x34B6
											// 0x34B7
	p_skew->ch3.lane_a = data[24];			// 0x34B8
	p_skew->ch3.lane_b = data[25];			// 0x34B9
	p_skew->ch3.lane_c = data[26];			// 0x34BA
											// 0x34BB
	p_skew->ch3.clk    = data[28];			// 0x34BC
	p_skew->ch3.lane_d = data[29];			// 0x34BD
	p_skew->ch3.lane_e = data[30];			// 0x34BE

	return ret;
}
//------------------------------------------------------------------------------
uint32_t lvdstx_abort(void)
{
	// abort request
	lvdstx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

	return ERR_OK;
}
//------------------------------------------------------------------------------
