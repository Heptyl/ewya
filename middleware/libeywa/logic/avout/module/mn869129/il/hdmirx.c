/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "hdmirx.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "hv_version.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// This is used to invert TMDS data channel polarity.
//#define CONFIG_RX0_TMDS_DATA_POLARITY_INVERSION
//#define CONFIG_RX1_TMDS_DATA_POLARITY_INVERSION
//#define CONFIG_RX2_TMDS_DATA_POLARITY_INVERSION
//#define CONFIG_RX3_TMDS_DATA_POLARITY_INVERSION
// This is used to swap TMDS data channel CH.0 for TX CH.2.
//#define CONFIG_RX0_TMDS_DATA_CHANNEL_SWAP
//#define CONFIG_RX1_TMDS_DATA_CHANNEL_SWAP
//#define CONFIG_RX2_TMDS_DATA_CHANNEL_SWAP
//#define CONFIG_RX3_TMDS_DATA_CHANNEL_SWAP
//------------------------------------------------------------------------------
// hdmirx_set_switch_mode123()
#define HDMIRX_SET_SWITCH_MODE123_LEN								3
#define HDMIRX_SET_SWITCH_MODE123_SWMODE1_REPEAT					0x98
#define HDMIRX_SET_SWITCH_MODE123_SWMODE2_DEF						0x00
#define HDMIRX_SET_SWITCH_MODE123_SWMODE3_RX0JRON_ON				0x01
//------------------------------------------------------------------------------
// hdmirx_set_irq_main_status_mask()
#define HDMIRX_SET_IRQ_MAIN_STATUS_MASK_LEN							2
#define HDMIRX_MINTSTAMSK_00_CCIRQST_BIT							0x40
#define HDMIRX_MINTSTAMSK_01_DEF									0x00
//------------------------------------------------------------------------------
// hdmirx_select_hdmirx()
#define HDMIRX_SELECT_HDMIRX_NUM									1
#define HDMIRX_MISC_VBOREN_HDMI_RX									0x00
#define HDMIRX_MISC_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_sysconf10()
#define HDMIRX_SET_SYSCONF10_LEN									4
//------------------------------------------------------------------------------
// hdmirx_set_sysconf10_of_portno()
#define HDMIRX_SET_SYSCONF10_OF_PORTNO_NUM							1
//------------------------------------------------------------------------------
// hdmirx_get_irq0()
#define HDMIRX_GET_IRQ0_LEN											4
//------------------------------------------------------------------------------
// hdmirx_get_irq0_of_portno()
#define HDMIRX_GET_IRQ0_OF_PORTNO_NUM								1
//------------------------------------------------------------------------------
// hdmirx_set_irq0_of_portno()
#define HDMIRX_SET_IRQ0_OF_PORTNO_NUM								1
#define HDMIRX_INTREGX_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_irq4_of_portno()
#define HDMIRX_SET_IRQ4_OF_PORTNO_NUM								1
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx1_0_of_portno()
#define HDMIRX_SET_TMDSRX1_0_OF_PORTNO_NUM							1
#define HDMIRX_TMDSRXPX1_0_PORTENX_PORT_ENA							0x10
#define HDMIRX_TMDSRXPX1_0_PORTENX_PORT_DIS							0x00
#define HDMIRX_TMDSRXPX1_0_PORTENX_MASK								0x00000010
#define HDMIRX_TMDSRXPX1_0_TERMCKEN0_ENA							0x20
#define HDMIRX_TMDSRXPX1_0_TERMCKEN0_DIS							0x00
#define HDMIRX_TMDSRXPX1_0_TERMCKEN0_MASK							0x00000020
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx1_0_2_of_portno()
#define HDMIRX_SET_TMDSRX1_0_2_OF_PORTNO_LEN						3
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx2_0_of_portno()
#define HDMIRX_SET_TMDSRX2_0_OF_PORT_NUM							1
#define HDMIRX_TMDSRXPX2_0_EQSETX_3									3
#define HDMIRX_TMDSRXPX2_0_EQSETX_MASK								0x0000000F
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx4_0_of_portno()
#define HDMIRX_SET_TMDSRX4_0_OF_PORTNO_NUM							1
#define HDMIRX_TMDSRXPX4_0_AEC_STVX_INIT_SETUP						0x80
#define HDMIRX_TMDSRXPX4_0_AEC_STVX_MASK							0x000000C0
#define HDMIRX_TMDSRXPX4_0_AEC_VALX_SENS_SETUP						0x30
#define HDMIRX_TMDSRXPX4_0_AEC_VALX_MASK							0x00000030
#define HDMIRX_TMDSRXPX4_0_AEC_EN_ENA								0x08
#define HDMIRX_TMDSRXPX4_0_AEC_EN_DIS								0x00
#define HDMIRX_TMDSRXPX4_0_AEC_EN_MASK								0x00000008
#define HDMIRX_TMDSRXPX4_0_BIT_EXTENDX_SETUP						0x04
#define HDMIRX_TMDSRXPX4_0_BIT_EXTENDX_NOT_SETUP					0x00
#define HDMIRX_TMDSRXPX4_0_BIT_EXTENDX_MASK							0x00000004
#define HDMIRX_TMDSRXPX4_0_AEC_CLRX_CLEAR							0x02
#define HDMIRX_TMDSRXPX4_0_AEC_CLRX_NOT_CLEAR						0x00
#define HDMIRX_TMDSRXPX4_0_AEC_CLRX_MASK							0x00000002
#define HDMIRX_TMDSRXPX4_0_AEC_LOCKX_FIX							0x01
#define HDMIRX_TMDSRXPX4_0_AEC_LOCKX_NOT_FIX						0x00
#define HDMIRX_TMDSRXPX4_0_AEC_LOCKX_MASK							0x00000001
#define HDMIRX_TMDSRXPX4_0_MASK										0x000000FF
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_INIT					(HDMIRX_TMDSRXPX4_0_AEC_LOCKX_NOT_FIX		\
																	| HDMIRX_TMDSRXPX4_0_AEC_CLRX_CLEAR			\
																	| HDMIRX_TMDSRXPX4_0_BIT_EXTENDX_SETUP		\
																	| HDMIRX_TMDSRXPX4_0_AEC_EN_DIS				\
																	| HDMIRX_TMDSRXPX4_0_AEC_VALX_SENS_SETUP	\
																	| HDMIRX_TMDSRXPX4_0_AEC_STVX_INIT_SETUP)
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_INIT_MASK				HDMIRX_TMDSRXPX4_0_MASK
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_START					(HDMIRX_TMDSRXPX4_0_AEC_CLRX_NOT_CLEAR		\
																	| HDMIRX_TMDSRXPX4_0_AEC_EN_ENA)
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_START_MASK				(HDMIRX_TMDSRXPX4_0_AEC_CLRX_MASK			\
																	| HDMIRX_TMDSRXPX4_0_AEC_EN_MASK)
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_STOP					HDMIRX_TMDSRXPX4_0_AEC_LOCKX_FIX
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_STOP_MASK				HDMIRX_TMDSRXPX4_0_AEC_LOCKX_MASK
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_EXIT					(HDMIRX_TMDSRXPX4_0_AEC_LOCKX_NOT_FIX		\
																	| HDMIRX_TMDSRXPX4_0_AEC_EN_DIS)
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_EXIT_MASK				(HDMIRX_TMDSRXPX4_0_AEC_LOCKX_MASK			\
																	| HDMIRX_TMDSRXPX4_0_AEC_EN_MASK)
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx5_0_of_portno()
#define HDMIRX_SET_TMDSRX5_0_OF_PORTNO_NUM							1
#define HDMIRX_TMDSRXPX5_0_ATVLPGX_0								0x00
#define HDMIRX_TMDSRXPX5_0_ATVLPGX_1								0x80
#define HDMIRX_TMDSRXPX5_0_RESERVED									0x0E
#define HDMIRX_TMDSRXPX5_0_ATEQSTX_MANUAL							0x00
#define HDMIRX_TMDSRXPX5_0_ATEQSTX_AUTO								0x01
#define HDMIRX_TMDSRXPX5_0_MASK										0x000000FF
#define HDMIRX_SET_TMDS_RX_PARAM_0									(HDMIRX_TMDSRXPX5_0_ATVLPGX_0		\
																	| HDMIRX_TMDSRXPX5_0_RESERVED		\
																	| HDMIRX_TMDSRXPX5_0_ATEQSTX_AUTO)
#define HDMIRX_SET_TMDS_RX_PARAM_1									(HDMIRX_TMDSRXPX5_0_ATVLPGX_1		\
																	| HDMIRX_TMDSRXPX5_0_RESERVED		\
																	| HDMIRX_TMDSRXPX5_0_ATEQSTX_AUTO)
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX5_0_AUTO					(HDMIRX_TMDSRXPX5_0_ATVLPGX_0		\
																	| HDMIRX_TMDSRXPX5_0_RESERVED		\
																	| HDMIRX_TMDSRXPX5_0_ATEQSTX_AUTO)
#define HDMIRX_TMDS_CALIBRATION_TMDSRXPX5_0_MANUAL					(HDMIRX_TMDSRXPX5_0_ATVLPGX_0		\
																	| HDMIRX_TMDSRXPX5_0_RESERVED		\
																	| HDMIRX_TMDSRXPX5_0_ATEQSTX_MANUAL)
//------------------------------------------------------------------------------
// hdmirx_get_tmdsrx5_1_3_of_portno()
#define HDMIRX_GET_TMDSRX5_1_3_OF_PORTNO_LEN						3
#define HDMIRX_TMDSRXPX5_EQSETOUTX_3								3
#define HDMIRX_TMDSRXPX5_EQSETOUTX_MASK								0x0000000F
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx6_of_portno()
#define HDMIRX_TMDSRXPX6_PARAM_KIND_NUM								2
#define HDMIRX_TMDSRXPX6_PARAM_KIND_0								0
#define HDMIRX_TMDSRXPX6_PARAM_KIND_1								1
#define HDMIRX_TMDSRXPX6_PARAM_NUM									16
#define HDMIRX_SET_TMDSRX6_OF_PORTNO_LEN							HDMIRX_TMDSRXPX6_PARAM_NUM
//------------------------------------------------------------------------------
// hdmirx_get_auth_phase_of_portno()
#define HDMIRX_GET_AUTH_PHASE_OF_PORTNO_NUM							3
#define HDMIRX_FPSWMON_01_AUPHS0_BIT								0x01
#define HDMIRX_CKDTPRM2_P0TCKSEL_BIT								0x10
#define HDMIRX_FPSWCTRL_00_PSSYEN_MASK								0x0000000F
//------------------------------------------------------------------------------
// hdmirx_set_switch_mode1()
#define HDMIRX_SET_SWITCH_MODE1_NUM									1
#define HDMIRX_SWMODE1_PORT1MODE_NOR								0x80
#define HDMIRX_SWMODE1_PORT1OUT_DEF									0x10
#define HDMIRX_SWMODE1_PORT0MODE_REPEAT_MODE						0x08
#define HDMIRX_SWMODE1_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_tmds_clock_freq_of_portno()
#define HDMIRX_GET_TMDS_CLOCK_FREQ_OF_PORTNO_LEN					2
#define HDMIRX_PXTCLKFQ_MASK										0x0000000F
#define HDMIRX_PXTCLKFQ_P1TCLKFQ_BIT								4
#define HDMIRX_PXTCLKFQ_P3TCLKFQ_BIT								HDMIRX_PXTCLKFQ_P1TCLKFQ_BIT
#define HDMIRX_PXTCLKFQ_NOT_DETECTED								0x00	// not detected
#define HDMIRX_PXTCLKFQ_LOWER_FDTTHA								0x01	//   0   -  20   MHz
#define HDMIRX_PXTCLKFQ_FDTTHA_FDTTHB								0x02	//  20   -  45   MHz
#define HDMIRX_PXTCLKFQ_FDTTHB_FDTTHC								0x03	//  45   -  85.5 MHz
#define HDMIRX_PXTCLKFQ_FDTTHC_FDTTHD								0x04	//  85.5 - 123   MHz
#define HDMIRX_PXTCLKFQ_FDTTHD_FDTTHE								0x05	// 123   - 175   MHz
#define HDMIRX_PXTCLKFQ_FDTTHE_FDTTHF								0x06	// 175   - 250   MHz
#define HDMIRX_PXTCLKFQ_FDTTHF_FDTTHG								0x07	// 250   - 342   MHz
#define HDMIRX_PXTCLKFQ_FDTTHG_FDTTHH								0x08	// 342   - 492   MHz
//------------------------------------------------------------------------------
// hdmirx_init_ddc_ctrl()
#define HDMIRX_SYSCFG1X_NPRIMEX_0X74								0x00
#define HDMIRX_SYSCFG1X_DDCFLTX_NOR									0x00
#define HDMIRX_SYSCFG1X_DDCNCSX_ACK									0x00
#define HDMIRX_SYSCFG1X_DDCNCSX_NACK								0x10
#define HDMIRX_SYSCFG1X_DDCNCSX_MASK								0x00000010
#define HDMIRX_SYSCFG1X_HDDCNCSX_NOR								0x00
#define HDMIRX_SYSCFG1X_SYSOPMX_EXTERNAL							0x04
#define HDMIRX_SYSCFG1X_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_scdcs_mode()
#define HDMIRX_SET_SCDCS_MODE_NUM									4
#define HDMIRX_MONREGX_0_SCDCNCSX_ACK								0x00
#define HDMIRX_MONREGX_0_SCDCNCSX_NACK								0x80
#define HDMIRX_MONREGX_0_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_scramble_mon_of_portno()
#define HDMIRX_GET_SCRAMBLE_MON_OF_PORTNO_NUM						1
#define HDMIRX_MONREGX_SCRMONX_MASK									0x00000002
//------------------------------------------------------------------------------
// hdmirx_set_tmds_maxtcc()
#define HDMIRX_SET_TMDS_MAXTCC_LEN									3
//------------------------------------------------------------------------------
// hdmirx_set_tmds_fchgth()
#define HDMIRX_SET_TMDS_FCHGTH_LEN									3
#define HDMIRX_CKDTPRM6_0_DEF										0x30
#define HDMIRX_CKDTPRM6_1_DEF										0x00
#define HDMIRX_CKDTPRM6_2_DEF										0x00
//------------------------------------------------------------------------------
// hdmirx_set_tmds_fdttha_h()
#define HDMIRX_SET_TMDS_FDTTHA_H_LEN								31
//------------------------------------------------------------------------------
// hdmirx_set_tmds_maxhcc()
// hdmirx_set_link_freq_detect_param()
#define HDMIRX_SET_TMDS_MAXHCC_LEN									2
//------------------------------------------------------------------------------
// hdmirx_set_tmds_tckthrb()
#define HDMIRX_SET_TMDS_TCKTHRB_LEN									3
#define HDMIRX_CKDTPRM10_0_DEF										0x20
#define HDMIRX_CKDTPRM10_1_DEF										0x4E
#define HDMIRX_CKDTPRM10_2_DEF										0x00
//------------------------------------------------------------------------------
// hdmirx_set_auto_reset_mode()
#define HDMIRX_SET_AUTO_RESET_MODE_NUM								4
#define HDMIRX_SYSCFGX_PXRSEQEN_AUTO_EXEC							0x08
#define HDMIRX_SYSCFGX_PXLKDTEN_GEN_IRQ								0x04
#define HDMIRX_SYSCFGX_PXSRSTEB_AUTO_CLR							0x00
#define HDMIRX_SYSCFGX_PXSRSTEA_AUTO_CLR							0x00
#define HDMIRX_SET_AUTO_RESET_MODE_MASK								0x0000000F
//------------------------------------------------------------------------------
// hdmirx_set_tmds_reset_lock_time()
#define HDMIRX_SET_TMDS_RESET_LOCK_TIME_LEN							15
//------------------------------------------------------------------------------
// hdmirx_set_fast_port_switch()
#define HDMIRX_SET_FAST_PORT_SWITCH_LEN								4
//------------------------------------------------------------------------------
// hdmirx_set_auto_scramble_detect_mode()
#define HDMIRX_SET_AUTO_SCRAMBLE_DETECT_MODE_LEN					4
#define HDMIRX_SYSCFGX_SYFILMX_DEF									0x00
#define HDMIRX_SYSCFGX_ASCRENX_AUTO_DETECT							0x40
#define HDMIRX_SYSCFGX_SYFILTX_DEF									0x00
#define HDMIRX_SYSCFGX_SKWDISX_DEF									0x00
#define HDMIRX_SYSCFGX_RSYAENX_DEF									0x00
#define HDMIRX_SYSCFGX_RSYDAUX_DEF									0x00
#define HDMIRX_SYSCFGX_RSYDISX_DEF									0x00
//------------------------------------------------------------------------------
// hdmirx_set_scramble_seed()
#define HDMIRX_SET_SCRAMBLE_SEED_LEN								6
#define HDMIRX_SCRINIT0_SCRINIA_INIT								0xFF
#define HDMIRX_SCRINIT1_SCRINIA_INIT								0xFF
#define HDMIRX_SCRINIT2_SCRINIB_INIT								0xFF
#define HDMIRX_SCRINIT3_SCRINIB_INIT								0x7F
#define HDMIRX_SCRINIT4_SCRINIC_INIT								0xFF
#define HDMIRX_SCRINIT5_SCRINIC_INIT								0xBF
//------------------------------------------------------------------------------
// hdmirx_set_scramble_mode()
#define HDMIRX_SET_SCRAMBLE_MODE_NUM								4
#define HDMIRX_DBGREGX_SCRMODX_ON									0x03
#define HDMIRX_DBGREGX_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_lump_mode()
#define HDMIRX_SET_LUMP_MODE_NUM									1
#define HDMIRX_SYSCFG15_TMRLUMP_ENABLE								0x01
#define HDMIRX_SYSCFG15_TMRLUMP_DISABLE								0x00
#define HDMIRX_SYSCFG15_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_tmds_auto_speed_mode()
#define HDMIRX_SET_TMDS_AUTO_SPEED_MODE_NUM							1
#define HDMIRX_CKDTPRM4_02_AT4DIVDS_AUTO							0x00
#define HDMIRX_CKDTPRM4_02_AT4DIVDS_MANUAL							0x80
#define HDMIRX_CKDTPRM4_02_AT4DIVDS_MASK							0x00000080
//------------------------------------------------------------------------------
// hdmirx_set_clock_freq_detect_param2()
#define HDMIRX_SET_CLOCK_FREQ_DETECT_PARAM2_NUM						1
#define HDMIRX_CKDTPRM2_PXTCKSEL_CRU								0x00
#define HDMIRX_CKDTPRM2_P0TCKSEL_CRU								0x00
#define HDMIRX_CKDTPRM2_PXTCKSEL_TMDS								0xF0
#define HDMIRX_CKDTPRM2_P0TCKSEL_TMDS								0x10
#define HDMIRX_CKDTPRM2_PXTCKSEL_MASK								0x000000F0
#define HDMIRX_CKDTPRM2_P0TCKSEL_MASK								0x00000010
#define HDMIRX_CKDTPRM2_PXTCLK4DIV_NOR								0x00
#define HDMIRX_CKDTPRM2_P0TCLK4DIV_NOR								0x00
#define HDMIRX_CKDTPRM2_PXTCLK4DIV_4DIV								0x0F
#define HDMIRX_CKDTPRM2_P0TCLK4DIV_4DIV								0x01
#define HDMIRX_CKDTPRM2_PXTCLK4DIV_MASK								0x0000000F
#define HDMIRX_CKDTPRM2_P0TCLK4DIV_MASK								0x00000001
//------------------------------------------------------------------------------
// hdmirx_set_fast_port_switch_mode()
#define HDMIRX_SET_FAST_PORT_SW_MODE_NUM							1
#define HDMIRX_FPSWCTRL_00_PSYOENX_NOT_OUTPUT						0x00
#define HDMIRX_FPSWCTRL_00_PSYOENX_OUTPUT							0xF0
#define HDMIRX_FPSWCTRL_00_PSSYEN_FAST_DIS							0x00
#define HDMIRX_FPSWCTRL_00_PSSYEN_FAST_ENA							0x0F
#define HDMIRX_FPSWCTRL_00_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_intr_mask_port()
#define HDMIRX_SET_INTR_MASK_PORT_LEN								9
#define HDMIRX_INTMSK_0_3_0F										0x0F
#define HDMIRX_INTMSK_4_7_E1										0xE1
#define HDMIRX_INTMSK_8_F0											0xF0
//------------------------------------------------------------------------------
// hdmirx_set_intr_status_mask_port()
#define HDMIRX_SET_INTR_STATUS_MASK_PORT_LEN						2
#define HDMIRX_INTSTAMSK_0_FF										0xFF
#define HDMIRX_INTSTAMSK_0_01										0x01
//------------------------------------------------------------------------------
// hdmirx_select_page_of_0x60c4()
#define HDMIRX_SELECT_PAGE_OF_0X60C4_NUM							1
#define HDMIRX_RCKFQDET3_00_PAGE_NO_0								0x00
#define HDMIRX_RCKFQDET3_00_PAGE_NO_1								0x01
#define HDMIRX_RCKFQDET3_00_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_reset_seq_time1()
#define HDMIRX_SET_RESET_SEQ_TIME1_LEN								2
//------------------------------------------------------------------------------
// hdmirx_set_reset_seq_time2()
#define HDMIRX_SET_RESET_SEQ_TIME2_LEN								3
//------------------------------------------------------------------------------
// hdmirx_set_clock_freq_detect_param()
#define HDMIRX_SET_CLOCK_FREQ_DETECT_PARAM_LEN						2
//------------------------------------------------------------------------------
// hdmirx_set_sptvfm()
#define HDMIRX_SET_SPTVFM_LEN										8
//------------------------------------------------------------------------------
// hdmirx_set_sptpfm()
#define HDMIRX_SET_SPTPFM_LEN										8
//------------------------------------------------------------------------------
// hdmirx_set_spthfm()
#define HDMIRX_SET_SPTHFM_LEN										3
//------------------------------------------------------------------------------
// hdmirx_set_audio_clock_regen_mode()
#define HDMIRX_SET_AUDIO_CLOCK_REGEN_MODE_NUM						1
#define HDMIRX_RXACTL4_CTSDIVMOD_ENA								0x10
#define HDMIRX_RXACTL4_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_support_audio_format()
#define HDMIRX_SET_SUPPORT_AUDIO_FORMAT_NUM							1
#define HDMIRX_RXACTL9_ASREN_ENA									0x01
#define HDMIRX_RXACTL9_HBRREN_ENA									0x04
#define HDMIRX_RXACTL9_MSTREN_DIS									0x00
#define HDMIRX_RXACTL9_BSSTOP_SPDIF_SDO								0x00
#define HDMIRX_RXACTL9_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_output_i2s_mode()
#define HDMIRX_SET_OUTPUT_I2S_MODE_LEN								2
#define HDMIRX_RXACTL5_DEF											0x34
#define HDMIRX_RXACTL6_DEF											0x00
//------------------------------------------------------------------------------
// hdmirx_set_output_master_clock_freq()
#define HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ1_LEN					4
#define HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ2_LEN					3
#define HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ_LEN						HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ1_LEN
#define HDMIRX_RXMCLK1_SPDMEN_NOT_MASK								0x00
#define HDMIRX_RXMCLK1_FS44MCLK_BIT									4
#define HDMIRX_RXMCLK1_FS88MCLK_BIT									4
#define HDMIRX_RXMCLK1_FS176MCLK_BIT								4
#define HDMIRX_RXMCLK2_FS128MCLK_BIT								4
#define HDMIRX_RXMCLK2_FS384MCLK_BIT								4
#define HDMIRX_RXMCLK2_GAINCTL_UNLOCK								0x00
#define HDMIRX_RXMCLK2_RSTMODE_LARGE								0x00
//------------------------------------------------------------------------------
// hdmirx_set_audio_ch_assign()
#define HDMIRX_SET_AUDIO_CH_ASSIGN_NUM								1
#define HDMIRX_RXADCH1_02_CHASGN_FR_FL								0x00
#define HDMIRX_RXADCH1_02_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_audio_mute_ctrl()
#define HDMIRX_SET_AUDIO_MUTE_CTRL_LEN								3
#define HDMIRX_RXMUTECTL_0_LKDMEN_MUTE								0x01
#define HDMIRX_RXMUTECTL_0_HPLULMEN_MUTE							0x02
#define HDMIRX_RXMUTECTL_0_DVIMMEN_MUTE								0x10
#define HDMIRX_RXMUTECTL_0_FLATLMEN_MUTE							0x80
#define HDMIRX_RXMUTECTL_1_AVMMEN_MUTE								0x01
#define HDMIRX_RXMUTECTL_1_ASPNDMEN_MUTE							0x10
#define HDMIRX_RXMUTECTL_2_DEF										0x00
//------------------------------------------------------------------------------
// hdmirx_set_mode_ctrl1()
#define HDMIRX_SET_MODE_CTRL1_NUM									1
#define HDMIRX_MODCNT1_ADMUTE_UNMUTE								0x00
#define HDMIRX_MODCNT1_ADMUTE_MUTE									0x20
#define HDMIRX_MODCNT1_ADMUTE_MASK									0x00000020
#define HDMIRX_MODCNT1_VDMUTE_UNMUTE								0x00
#define HDMIRX_MODCNT1_VDMUTE_MUTE									0x10
#define HDMIRX_MODCNT1_VDMUTE_MASK									0x00000010
#define HDMIRX_MODCNT1_VFMCLR_DEF									0x00
#define HDMIRX_MODCNT1_VFMCLR_RE_DETECT								0x04
#define HDMIRX_MODCNT1_VFMCLR_MASK									0x00000004
//------------------------------------------------------------------------------
// hdmirx_set_mode_ctrl2()
#define HDMIRX_SET_MODE_CTRL2_NUM									1
#define HDMIRX_MODCNT2_INTR2ADM_MUTE								0x10
#define HDMIRX_MODCNT2_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_select_page_of_0x68c0()
#define HDMIRX_SELECT_PAGE_OF_0X68C0_NUM							1
#define HDMIRX_RXVSICTL_00_HVSIPAGE_H_VSIF							0x00
#define HDMIRX_RXVSICTL_00_HVSIPAGE_HF_VSIF							0x10
#define HDMIRX_RXVSICTL_00_HVSIPAGE_MASK							0x00000010
//------------------------------------------------------------------------------
// hdmirx_set_packet_receive_mode()
#define HDMIRX_SET_PACKET_RECEIVE_MODE_NUM							1
#define HDMIRX_EPKTCTL_00_PKTMODSEL_FIRST_PACKET					0x04
#define HDMIRX_EPKTCTL_00_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_packet_hold()
#define HDMIRX_SET_PACKET_HOLD_NUM									1
#define HDMIRX_PKTHLDCTL_00_PKTHOLD_NOT_HOLD						0x00
#define HDMIRX_PKTHLDCTL_00_PKTHOLD_FRE								0x07
#define HDMIRX_PKTHLDCTL_00_PKTHOLD_SPD								0x14
#define HDMIRX_PKTHLDCTL_00_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_hdmi_forum_vsi_id()
#define HDMIRX_SET_HDMI_FORUM_VSI_ID_LEN							3
#define HDMIRX_RXHFVSIMON_04_ID1									0xD8
#define HDMIRX_RXHFVSIMON_05_ID2									0x5D
#define HDMIRX_RXHFVSIMON_06_ID3									0xC4
//------------------------------------------------------------------------------
// hdmirx_set_intr_mask_rx()
#define HDMIRX_SET_INTR_MASK_RX_LEN									6
#define HDMIRX_RINTMSK_0_5D											0x5D
#define HDMIRX_RINTMSK_1_4C											0x4C
#define HDMIRX_RINTMSK_2_8D											0x8D
#define HDMIRX_RINTMSK_3_1D											0x1D
#define HDMIRX_RINTMSK_4_NONE										0x00
#define HDMIRX_RINTMSK_5_NONE										0x00
//------------------------------------------------------------------------------
// hdmirx_set_intr_mask_rx_1()
#define HDMIRX_SET_INTR_MASK_RX_1_NUM								1
#define HDMIRX_RINTMSK_00_VFSUCS_OFF								0x00
#define HDMIRX_RINTMSK_00_VFSUCS_ON									0x04
#define HDMIRX_RINTMSK_00_VFSUCS_MASK								0x00000004
#define HDMIRX_RINTMSK_00_VFFAIL_OFF								0x00
#define HDMIRX_RINTMSK_00_VFFAIL_ON									0x08
#define HDMIRX_RINTMSK_00_VFFAIL_MASK								0x00000008
#define HDMIRX_RINTMSK_00_UNKNOWN_OFF								0x00
#define HDMIRX_RINTMSK_00_UNKNOWN_ON								0x10
#define HDMIRX_RINTMSK_00_UNKNOWN_MASK								0x00000010
//------------------------------------------------------------------------------
// hdmirx_set_intr_audio_packet_mask_rx()
#define HDMIRX_SET_INTR_AUDIO_PACKET_MASK_RX_LEN					2
#define HDMIRX_APKSTMSK_0_09										0x09
#define HDMIRX_APKSTMSK_1_NONE										0x00
//------------------------------------------------------------------------------
// hdmirx_set_intr_packet_mask_rx()
#define HDMIRX_SET_INTR_PACKET_MASK_RX_LEN							3
#define HDMIRX_PKTSTMSK_0_3D										0x3D
#define HDMIRX_PKTSTMSK_1_0F										0x0F
#define HDMIRX_PKTSTMSK_2_NONE										0x00
//------------------------------------------------------------------------------
// hdmirx_set_ext_intr_mask_rx()
#define HDMIRX_SET_EXT_INTR_MASK_RX_LEN								6
#define HDMIRX_REINTMSK_0_5D										0x5D
#define HDMIRX_REINTMSK_1_4C										0x4C
#define HDMIRX_REINTMSK_2_8D										0x8D
#define HDMIRX_REINTMSK_3_NONE										0x00
#define HDMIRX_REINTMSK_4_NONE										0x00
#define HDMIRX_REINTMSK_5_NONE										0x00
//------------------------------------------------------------------------------
// hdmirx_set_intr_status_mask_rx()
#define HDMIRX_SET_INTR_STATUS_MASK_RX_LEN							4
#define HDMIRX_RISTMSK0_IRQMSA_ALL									0xFF
#define HDMIRX_RISTMSK1_IRQMSB_ALL									0x01
#define HDMIRX_REISTMSK0_EIRMSA_ALL									0xFF
#define HDMIRX_REISTMSK1_EIRMSB_ALL									0x01
//------------------------------------------------------------------------------
// hdmirx_get_selected_rx_port()
#define HDMIRX_GET_SELECTED_RX_PORT_NUM								1
#define HDMIRX_GET_SELECTED_RX_PORT_SWMODE1_PORT0OUT_MASK			0x00000003
//------------------------------------------------------------------------------
// hdmirx_set_tmds_data_swp_pol_portno()
#define HDMIRX_SET_TMDS_DATA_SWP_POL_PORTNO_NUM						2
#define HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_DIS						0x00
#define HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_ENA						0x80
#define HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_MASK							0x00000080
#define HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_NOR					0x00
#define HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_INV					0x40
#define HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_MASK					0x00000040
//------------------------------------------------------------------------------
// hdmirx_get_irq_5v_power()
#define HDMIRX_GET_IRQ_5V_POWER_NUM									1
#define HDMIRX_GET_IRQ_5V_POWER_INTREG8_PXV5STCHG_MASK				0x000000F0
//------------------------------------------------------------------------------
// hdmirx_clr_irq_5v_power()
#define HDMIRX_CLR_IRQ_5V_POWER_NUM									1
#define HDMIRX_CLR_IRQ_5V_POWER_INTREG8_MASK						0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_mon_5v_power()
#define HDMIRX_GET_MON_5V_POWER_NUM									1
#define HDMIRX_GET_MON_5V_POWER_MONREG5_03_PXV5MN_OFF				0x00
#define HDMIRX_GET_MON_5V_POWER_MONREG5_03_PXV5MN_MASK				0x000000F0
//------------------------------------------------------------------------------
// hdmirx_get_irq_tmds_clock()
#define HDMIRX_IRQ_INTREGX_FRQCHGX_MASK								0x00000008
//------------------------------------------------------------------------------
// hdmirx_get_rx_irq1()
#define HDMIRX_GET_RX_IRQ1_NUM										1
#define HDMIRX_RINTREG1_VFSUCS_MASK									0x00000004
#define HDMIRX_RINTREG1_VFFAIL_MASK									0x00000008
#define HDMIRX_RINTREG1_UNKNOWN_MASK								0x00000010
#define HDMIRX_RINTREG1_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_rx_irq1()
#define HDMIRX_SET_RX_IRQ1_NUM										1
//------------------------------------------------------------------------------
// hdmirx_get_rx_irq3()
#define HDMIRX_GET_RX_IRQ3_NUM										1
#define HDMIRX_RINTREG3_AMSTCHG_MASK								0x00000080
//------------------------------------------------------------------------------
// hdmirx_set_rx_irq3()
#define HDMIRX_SET_RX_IRQ3_NUM										1
#define HDMIRX_RINTREG3_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_rx_irq6()
#define HDMIRX_GET_RX_IRQ6_NUM										1
#define HDMIRX_RINTREG6_00_AVMCHG_MASK								0x00000002
//------------------------------------------------------------------------------
// hdmirx_set_rx_irq6()
#define HDMIRX_SET_RX_IRQ6_NUM										1
#define HDMIRX_RINTREG6_00_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_rx_mon()
#define HDMIRX_GET_RX_MON_NUM										1
#define HDMIRX_MONREG_00_ASTMON_MASK								0x80
#define HDMIRX_MONREG_00_AVMMON_MASK								0x40
#define HDMIRX_MONREG_00_PCKMON_MASK								0x30
#define HDMIRX_MONREG_00_PCKMON_BIT									4
#define HDMIRX_PIXEL_CLOCK_24BIT									0x00
#define HDMIRX_PIXEL_CLOCK_30BIT									0x10
#define HDMIRX_PIXEL_CLOCK_36BIT									0x20
#define HDMIRX_PIXEL_CLOCK_48BIT									0x30
//------------------------------------------------------------------------------
// hdmirx_set_scdcs_source_ver()
#define HDMIRX_SET_SCDCS_SOURCE_VER_NUM								1
#define HDMIRX_SRCVERX_SOURCE_VERSION_0								0
#define HDMIRX_SRCVERX_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_scdcs_tmds_config()
#define HDMIRX_GET_SCDCS_TMDS_CONFIG_NUM							1
#define HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_MASK					0x02
#define HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_MASK						0x01
//------------------------------------------------------------------------------
// hdmirx_set_scdcs_tmds_config()
#define HDMIRX_SET_SCDCS_TMDS_CONFIG_NUM							1
#define HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_1_10					0x00
#define HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_DIS						0x00
#define HDMIRX_TMDSCFGX_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_scdcs_scrambler_status()
#define HDMIRX_SET_SCDCS_SCRAMBLER_STATUS_NUM						1
#define HDMIRX_SCRSTATX_SCRAMBLING_STATUS_NOT_DETECT				0x00
#define HDMIRX_SCRSTATX_SCRAMBLING_STATUS_DETECT					0x01
#define HDMIRX_SCRSTATX_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_tmds_alignment_lock_detect()
#define HDMIRX_TMDSRXPX1_1_TOFFX_SCRAMBLE_OFF						0x00
#define HDMIRX_TMDSRXPX1_1_TOFFX_SCRAMBLE_ON						0x04
#define HDMIRX_TMDSRXPX1_1_TOFFX_MASK								0x00000004
#define HDMIRX_TMDSRXPX1_2_VALX_2STAGES								0x02
#define HDMIRX_TMDSRXPX1_2_VALX_4STAGES								0x04
#define HDMIRX_TMDSRXPX1_2_VALX_MASK								0x00000007
#define HDMIRX_TMDSRXPX1_2_QMODE_T_6_STEP							0x00
#define HDMIRX_TMDSRXPX1_2_QMODE_T_12_STEP							0x80
#define HDMIRX_TMDSRXPX1_2_QMODE_MASK								0x00000080
//------------------------------------------------------------------------------
// hdmirx_set_equalizer_max_limit_of_portno()
#define HDMIRX_SET_EQUALIZER_MAX_LIMIT_OF_PORTNO_NUM				2
#define HDMIRX_EQMANSET20_X_EQSETMAXLIMITX_0						0x00
#define HDMIRX_EQMANSET20_X_EQSETMAXLIMITX_9						0x09
#define HDMIRX_EQMANSET20_X_EQSETMAXLIMITX_MASK						0x0000000F
#define HDMIRX_EQMANSET31_X_EQMAXLIMITENX_ENA						0x04
#define HDMIRX_EQMANSET31_X_EQMAXLIMITENX_DIS						0x00
#define HDMIRX_EQMANSET31_X_EQMAXLIMITENX_MASK						0x00000004
//------------------------------------------------------------------------------
// hdmirx_set_rxpll0en()
#define HDMIRX_SET_RXPLL0EN_NUM										1
#define HDMIRX_RXPLL0_01_RXPLL0EN_AUTO								0x00
#define HDMIRX_RXPLL0_01_RXPLL0EN_MANUAL							0x80
#define HDMIRX_RXPLL0_01_RXPLL0EN_MASK								0x00000080
//------------------------------------------------------------------------------
// hdmirx_set_rxpll00en()
#define HDMIRX_SET_RXPLL00EN_NUM									1
#define HDMIRX_RXPLL00_03_RXPLL00EN_AUTO							0x00
#define HDMIRX_RXPLL00_03_RXPLL00EN_MANUAL							0x08
#define HDMIRX_RXPLL00_03_RXPLL00EN_MASK							0x00000008
//------------------------------------------------------------------------------
// hdmirx_set_rx0_sen()
#define HDMIRX_SET_RX0_SEN_NUM										1
#define HDMIRX_CKSELEN_RX0_SEN_AUTO									0x00
#define HDMIRX_CKSELEN_RX0_SEN_MANUAL								0x02
#define HDMIRX_CKSELEN_RX0_SEN_MASK									0x00000002
//------------------------------------------------------------------------------
// hdmirx_get_gcp_color_depth()
#define HDMIRX_GET_GCP_COLOR_DEPTH_NUM								1
#define HDMIRX_RGCPPKT_04_CD_MASK									0x0F
#define HDMIRX_GCP_COLOR_DEPTH_NONE									0x00
#define HDMIRX_GCP_COLOR_DEPTH_RESERVED0							0x01
#define HDMIRX_GCP_COLOR_DEPTH_RESERVED1							0x02
#define HDMIRX_GCP_COLOR_DEPTH_RESERVED2							0x03
#define HDMIRX_GCP_COLOR_DEPTH_24BIT								0x04
#define HDMIRX_GCP_COLOR_DEPTH_30BIT								0x05
#define HDMIRX_GCP_COLOR_DEPTH_36BIT								0x06
#define HDMIRX_GCP_COLOR_DEPTH_48BIT								0x07
//------------------------------------------------------------------------------
// hdmirx_get_tmds_clock_cnt_of_portno()
#define HDMIRX_GET_TMDS_CLOCK_CNT_OF_PORTNO_LEN							3
//------------------------------------------------------------------------------
// hdmirx_get_tmds_data_rate_of_portno()
#define HDMIRX_GET_TMDS_DATA_RATE_OF_PORTNO_NUM						1
#define HDMIRX_CKDET2X_3_PXTMDSFQ_MASK								0x00000007
#define HDMIRX_CKDET2X_3_PXTMDSFQ_LOWER_FDTTHB						0x00	//    0 -  450 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHB_FDTTHC						0x01	//  450 -  855 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHC_FDTTHD						0x02	//  855 - 1230 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHD_FDTTHE						0x03	// 1230 - 1750 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHE_FDTTHF						0x04	// 1750 - 2500 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHF_FDTTHG						0x05	// 2500 - 3420 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHG_FDTTHH						0x06	// 3420 - 4920 Mbps
#define HDMIRX_CKDET2X_3_PXTMDSFQ_UPPER_FDTTHH						0x07	// 4920 -      Mbps
//------------------------------------------------------------------------------
// hdmirx_get_calc_pixel_clock_freq()
#define HDMIRX_PIXEL_CLOCK_MULTI_1									1
#define HDMIRX_PIXEL_CLOCK_MULTI_4									4
#define HDMIRX_PIXEL_CLOCK_DIV_1000									1000
#define HDMIRX_PIXEL_CLOCK_DIV_1250									1250
#define HDMIRX_PIXEL_CLOCK_DIV_1500									1500
#define HDMIRX_PIXEL_CLOCK_DIV_2000									2000
#define HDMIRX_PCKMON_UNSPECIFIED									0xFF
//------------------------------------------------------------------------------
// hdmirx_get_calc_pixel_clock_range()
#define HDMIRX_PIXEL_CLOCK_0KHZ										0
#define HDMIRX_PIXEL_CLOCK_44000KHZ									44000
#define HDMIRX_PIXEL_CLOCK_65700KHZ									65700
#define HDMIRX_PIXEL_CLOCK_92000KHZ									92000
#define HDMIRX_PIXEL_CLOCK_190000KHZ								190000
#define HDMIRX_PIXEL_CLOCK_350000KHZ								350000
#define HDMIRX_PIXEL_CLOCK_620000KHZ								620000
//------------------------------------------------------------------------------
// hdmirx_get_pixel_clock_freq()
#define HDMIRX_GET_PIXEL_CLOCK_FREQ_NUM								1
#define HDMIRX_GET_PIXEL_CLOCK_FREQ_LEN								3
#define HDMIRX_CSCMOD3_ICFMT_MASK									0x000000F0
#define HDMIRX_CSCMOD3_ICFMT_BIN									4
#define HDMIRX_RCFDPRM12_00_PCKC_7_0_IDX							0
#define HDMIRX_RCFDPRM12_00_PCKC_7_0_BIT							0
#define HDMIRX_RCFDPRM12_00_PCKC_15_8_IDX							1
#define HDMIRX_RCFDPRM12_00_PCKC_15_8_BIT							8
#define HDMIRX_RCFDPRM12_00_PCKC_19_16_IDX							2
#define HDMIRX_RCFDPRM12_00_PCKC_19_16_BIT							16
//------------------------------------------------------------------------------
// hdmirx_get_pixel_clock_range()
#define HDMIRX_RCKFQDET1_PCLKFQ_NOT_FOUND							0x00	// not found
#define HDMIRX_RCKFQDET1_PCLKFQ_LOWER_PDTTHA						0x10	//   0   -  44   MHz
#define HDMIRX_RCKFQDET1_PCLKFQ_PDTTHA_PDTTHB						0x20	//  44   -  65.7 MHz
#define HDMIRX_RCKFQDET1_PCLKFQ_PDTTHB_PDTTHC						0x30	//  65.7 -  92   MHz
#define HDMIRX_RCKFQDET1_PCLKFQ_PDTTHC_PDTTHD						0x40	//  92   - 190   MHz
#define HDMIRX_RCKFQDET1_PCLKFQ_PDTTHD_PDTTHE						0x50	// 190   - 350   MHz
#define HDMIRX_RCKFQDET1_PCLKFQ_PDTTHE_PDTTHF						0x60	// 350   - 620   MHz
#define HDMIRX_RCKFQDET1_PCLKFQ_UPPER_PDTTHF						0x70	// 620   -       MHz
//------------------------------------------------------------------------------
// hdmirx_get_detected_vformat()
#define HDMIRX_GET_DETECTED_VFORMAT_LEN								2
#define HDMIRX_DETVFM_INDEX											0
#define HDMIRX_PCFDMT_INDEX											1
//------------------------------------------------------------------------------
// hdmirx_set_current_param_latch()
#define HDMIRX_SET_CURRENT_PARAM_LATCH								1
#define HDMIRX_CRRSYNC_01_CRRLATCH_LATCH							0x01
#define HDMIRX_CRRSYNC_01_CRRLATCH_CLR								0x00
#define HDMIRX_CRRSYNC_01_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_hori_vert_period()
#define HDMIRX_GET_HORI_VERT_PERIOD_LEN								4
#define HDMIRX_HVPERIOD_DETHPE_7_0_INDEX							0
#define HDMIRX_HVPERIOD_DETHPE_12_8_INDEX							1
#define HDMIRX_HVPERIOD_DETVPE_7_0_INDEX							2
#define HDMIRX_HVPERIOD_DETVPE_11_8_INDEX							3
//------------------------------------------------------------------------------
// hdmirx_get_current_hori_vert_period()
#define HDMIRX_GET_CURRENT_HORI_VERT_PERIOD_LEN						15
#define HDMIRX_CRRHVPERIOD_CRRHDE_1920								1920
#define HDMIRX_CRRHVPERIOD_CRRHDE_2048								2048
//------------------------------------------------------------------------------
// hdmirx_get_pc_format()
#define HDMIRX_VFMDET_00_DETVFM_UNKNOWN								0x00
#define HDMIRX_VFMDET_00_DETVFM_VGA									0x81
#define HDMIRX_VFMDET_00_DETVFM_WVGA								0x82
#define HDMIRX_VFMDET_00_DETVFM_SVGA								0x83
#define HDMIRX_VFMDET_00_DETVFM_WSVGA								0x84
#define HDMIRX_VFMDET_00_DETVFM_XGA									0x85
#define HDMIRX_VFMDET_00_DETVFM_WXGA								0x86
#define HDMIRX_VFMDET_00_DETVFM_WSXGA								0x87
#define HDMIRX_VFMDET_00_DETVFM_WXGA_P								0x88
#define HDMIRX_VFMDET_00_DETVFM_QVGA								0x89
#define HDMIRX_VFMDET_00_DETVFM_SXGA								0x8A
#define HDMIRX_VFMDET_00_DETVFM_SXGA_P								0x8B
#define HDMIRX_VFMDET_00_DETVFM_WSXGA_P								0x8C
#define HDMIRX_VFMDET_00_DETVFM_UXGA								0x8D
#define HDMIRX_VFMDET_00_DETVFM_WUXGA								0x8E
#define HDMIRX_VFMDET_00_DETVFM_QXGA								0x8F
#define HDMIRX_VFMDET_00_DETVFM_QSXGA								0x90
#define HDMIRX_VFMDET_00_DETVFM_QSXGA_P								0x91
#define HDMIRX_VFMDET_00_DETVFM_QUXGA								0x92
#define HDMIRX_VFMDET_00_DETVFM_640X400								0x93
#define HDMIRX_VFMDET_00_DETVFM_720X400								0x94
#define HDMIRX_VFMDET_00_DETVFM_848X480								0x95
#define HDMIRX_VFMDET_00_DETVFM_1360X768							0x96
#define HDMIRX_VFMDET_00_DETVFM_WQHD								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_UNKNOWN								0x00
#define HDMIRX_VFMDET_01_PCFDMT_VGA									0x04
#define HDMIRX_VFMDET_01_PCFDMT_WVGA								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_SVGA								0x09
#define HDMIRX_VFMDET_01_PCFDMT_WSVGA								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_XGA									0x10
#define HDMIRX_VFMDET_01_PCFDMT_WXGA								0x17
#define HDMIRX_VFMDET_01_PCFDMT_WSXGA								0x1C
#define HDMIRX_VFMDET_01_PCFDMT_WXGA_P								0x2F
#define HDMIRX_VFMDET_01_PCFDMT_QVGA								0x20
#define HDMIRX_VFMDET_01_PCFDMT_SXGA								0x23
#define HDMIRX_VFMDET_01_PCFDMT_SXGA_P								0x2A
#define HDMIRX_VFMDET_01_PCFDMT_WSXGA_P								0x3A
#define HDMIRX_VFMDET_01_PCFDMT_UXGA								0x33
#define HDMIRX_VFMDET_01_PCFDMT_WUXGA								0x45
#define HDMIRX_VFMDET_01_PCFDMT_QXGA								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_QSXGA								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_QSXGA_P								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_QUXGA								0xFF
#define HDMIRX_VFMDET_01_PCFDMT_640X400								0x02
#define HDMIRX_VFMDET_01_PCFDMT_720X400								0x03
#define HDMIRX_VFMDET_01_PCFDMT_848X480								0x0E
#define HDMIRX_VFMDET_01_PCFDMT_1360X768							0x27
#define HDMIRX_VFMDET_01_PCFDMT_WQHD								0xFF
#define HDMIRX_HVPERIOD_00_IDX										0
#define HDMIRX_HVPERIOD_01_IDX										1
#define HDMIRX_HVPERIOD_02_IDX										2
#define HDMIRX_HVPERIOD_03_IDX										3
#define HDMIRX_HVPERIOD_01_IDX_BIT									8
#define HDMIRX_HVPERIOD_03_IDX_BIT									8
#define HDMIRX_HVPERIOD_00_MASK										0xFF
#define HDMIRX_HVPERIOD_01_MASK										0x1F
#define HDMIRX_HVPERIOD_02_MASK										0xFF
#define HDMIRX_HVPERIOD_03_MASK										0x0F
#define HDMIRX_HPE_VGA_640x480_72									832
#define HDMIRX_HPE_VGA_640x480_75									840
#define LOWER_LIMIT01(n)											((uint32_t)(n)*1000/1001)
#define UPPER_LIMIT01(n)											(((uint32_t)(n)*1001/1000) \
																		+ (((uint32_t)(n)%1000) ? 1 : 0))
#define HDMIRX_PIXEL_CLOCK_L31500KHZ								31400
#define HDMIRX_PIXEL_CLOCK_H31500KHZ								31600
#define HDMIRX_PIXEL_CLOCK_L36000KHZ								35900
#define HDMIRX_PIXEL_CLOCK_H36000KHZ								36100
#define HDMIRX_PIXEL_CLOCK_L40000KHZ								39900
#define HDMIRX_PIXEL_CLOCK_H40000KHZ								40100
#define HDMIRX_PIXEL_CLOCK_L50000KHZ								49900
#define HDMIRX_PIXEL_CLOCK_H50000KHZ								50100
#define HDMIRX_PIXEL_CLOCK_L49500KHZ								49400
#define HDMIRX_PIXEL_CLOCK_H49500KHZ								49600
#define HDMIRX_PIXEL_CLOCK_L56250KHZ								56150
#define HDMIRX_PIXEL_CLOCK_H56250KHZ								56350
#define HDMIRX_PIXEL_CLOCK_L73250KHZ								73150
#define HDMIRX_PIXEL_CLOCK_H73250KHZ								73350
#define HDMIRX_PIXEL_CLOCK_L65000KHZ								64900
#define HDMIRX_PIXEL_CLOCK_H65000KHZ								65100
#define HDMIRX_PIXEL_CLOCK_L75000KHZ								74900
#define HDMIRX_PIXEL_CLOCK_H75000KHZ								75100
#define HDMIRX_PIXEL_CLOCK_L78750KHZ								78650
#define HDMIRX_PIXEL_CLOCK_H78750KHZ								78850
#define HDMIRX_PIXEL_CLOCK_L94500KHZ								94400
#define HDMIRX_PIXEL_CLOCK_H94500KHZ								94600
#define HDMIRX_PIXEL_CLOCK_L115500KHZ								115400
#define HDMIRX_PIXEL_CLOCK_H115500KHZ								115600
#define HDMIRX_PIXEL_CLOCK_L68250KHZ								68150
#define HDMIRX_PIXEL_CLOCK_H68250KHZ								68350
#define HDMIRX_PIXEL_CLOCK_L79500KHZ								79400
#define HDMIRX_PIXEL_CLOCK_H79500KHZ								79600
#define HDMIRX_PIXEL_CLOCK_L102250KHZ								102150
#define HDMIRX_PIXEL_CLOCK_H102250KHZ								102350
#define HDMIRX_PIXEL_CLOCK_L117500KHZ								117400
#define HDMIRX_PIXEL_CLOCK_H117500KHZ								117600
#define HDMIRX_PIXEL_CLOCK_L140250KHZ								140150
#define HDMIRX_PIXEL_CLOCK_H140250KHZ								140350
#define HDMIRX_PIXEL_CLOCK_L71000KHZ								70900
#define HDMIRX_PIXEL_CLOCK_H71000KHZ								71100
#define HDMIRX_PIXEL_CLOCK_L83500KHZ								83400
#define HDMIRX_PIXEL_CLOCK_H83500KHZ								83600
#define HDMIRX_PIXEL_CLOCK_L106500KHZ								106400
#define HDMIRX_PIXEL_CLOCK_H106500KHZ								106600
#define HDMIRX_PIXEL_CLOCK_L122500KHZ								122400
#define HDMIRX_PIXEL_CLOCK_H122500KHZ								122600
#define HDMIRX_PIXEL_CLOCK_L146250KHZ								146150
#define HDMIRX_PIXEL_CLOCK_H146250KHZ								146350
#define HDMIRX_PIXEL_CLOCK_L88750KHZ								88650
#define HDMIRX_PIXEL_CLOCK_H88750KHZ								88850
#define HDMIRX_PIXEL_CLOCK_L136750KHZ								136650
#define HDMIRX_PIXEL_CLOCK_H136750KHZ								136850
#define HDMIRX_PIXEL_CLOCK_L157000KHZ								156900
#define HDMIRX_PIXEL_CLOCK_H157000KHZ								157100
#define HDMIRX_PIXEL_CLOCK_L182750KHZ								182650
#define HDMIRX_PIXEL_CLOCK_H182750KHZ								182850
#define HDMIRX_PIXEL_CLOCK_L108000KHZ								107900
#define HDMIRX_PIXEL_CLOCK_H108000KHZ								108100
#define HDMIRX_PIXEL_CLOCK_L148500KHZ								148400
#define HDMIRX_PIXEL_CLOCK_H148500KHZ								148600
#define HDMIRX_PIXEL_CLOCK_L175500KHZ								175400
#define HDMIRX_PIXEL_CLOCK_H175500KHZ								175600
#define HDMIRX_PIXEL_CLOCK_L135000KHZ								134900
#define HDMIRX_PIXEL_CLOCK_H135000KHZ								135100
#define HDMIRX_PIXEL_CLOCK_L157500KHZ								157400
#define HDMIRX_PIXEL_CLOCK_H157500KHZ								157600
#define HDMIRX_PIXEL_CLOCK_L187250KHZ								187150
#define HDMIRX_PIXEL_CLOCK_H187250KHZ								187350
#define HDMIRX_PIXEL_CLOCK_L101000KHZ								100900
#define HDMIRX_PIXEL_CLOCK_H101000KHZ								101100
#define HDMIRX_PIXEL_CLOCK_L121750KHZ								121650
#define HDMIRX_PIXEL_CLOCK_H121750KHZ								121850
#define HDMIRX_PIXEL_CLOCK_L156000KHZ								155900
#define HDMIRX_PIXEL_CLOCK_H156000KHZ								156100
#define HDMIRX_PIXEL_CLOCK_L179500KHZ								179400
#define HDMIRX_PIXEL_CLOCK_H179500KHZ								179600
#define HDMIRX_PIXEL_CLOCK_L208000KHZ								207900
#define HDMIRX_PIXEL_CLOCK_H208000KHZ								208100
#define HDMIRX_PIXEL_CLOCK_L119000KHZ								118900
#define HDMIRX_PIXEL_CLOCK_H119000KHZ								119100
#define HDMIRX_PIXEL_CLOCK_L187000KHZ								186900
#define HDMIRX_PIXEL_CLOCK_H187000KHZ								187100
#define HDMIRX_PIXEL_CLOCK_L214750KHZ								214650
#define HDMIRX_PIXEL_CLOCK_H214750KHZ								214850
#define HDMIRX_PIXEL_CLOCK_L245500KHZ								245400
#define HDMIRX_PIXEL_CLOCK_H245500KHZ								245600
#define HDMIRX_PIXEL_CLOCK_L162000KHZ								161900
#define HDMIRX_PIXEL_CLOCK_H162000KHZ								162100
#define HDMIRX_PIXEL_CLOCK_L189000KHZ								188900
#define HDMIRX_PIXEL_CLOCK_H189000KHZ								189100
#define HDMIRX_PIXEL_CLOCK_L202500KHZ								202400
#define HDMIRX_PIXEL_CLOCK_H202500KHZ								202600
#define HDMIRX_PIXEL_CLOCK_L229500KHZ								229400
#define HDMIRX_PIXEL_CLOCK_H229500KHZ								229600
#define HDMIRX_PIXEL_CLOCK_L268250KHZ								268150
#define HDMIRX_PIXEL_CLOCK_H268250KHZ								268350
#define HDMIRX_PIXEL_CLOCK_L154000KHZ								153900
#define HDMIRX_PIXEL_CLOCK_H154000KHZ								154100
#define HDMIRX_PIXEL_CLOCK_L193250KHZ								193150
#define HDMIRX_PIXEL_CLOCK_H193250KHZ								193350
#define HDMIRX_PIXEL_CLOCK_L245250KHZ								245150
#define HDMIRX_PIXEL_CLOCK_H245250KHZ								245350
#define HDMIRX_PIXEL_CLOCK_L281250KHZ								281150
#define HDMIRX_PIXEL_CLOCK_H281250KHZ								281350
#define HDMIRX_PIXEL_CLOCK_L265000KHZ								264900
#define HDMIRX_PIXEL_CLOCK_H265000KHZ								265100
#define HDMIRX_PIXEL_CLOCK_L35500KHZ								35400
#define HDMIRX_PIXEL_CLOCK_H35500KHZ								35600
#define HDMIRX_PIXEL_CLOCK_L33750KHZ								33650
#define HDMIRX_PIXEL_CLOCK_H33750KHZ								33850
#define HDMIRX_PIXEL_CLOCK_L72000KHZ								71900
#define HDMIRX_PIXEL_CLOCK_H72000KHZ								72100
#define HDMIRX_PIXEL_CLOCK_L85500KHZ								85400
#define HDMIRX_PIXEL_CLOCK_H85500KHZ								85600
#define HDMIRX_HDE_1360												1360
#define HDMIRX_HDE_1366												1366
#define HDMIRX_VDE_768												768
#define HDMIRX_HPE_1792												1792
#define HDMIRX_HSW_112												112
#define HDMIRX_HSW_143												143
#define HDMIRX_PIXEL_CLOCK_L148250KHZ								148150
#define HDMIRX_PIXEL_CLOCK_H148250KHZ								148350
//------------------------------------------------------------------------------
// hdmirx_get_unknown_format()
#define HDMIRX_CRRHVPERIOD_CRRHPE_7_0_IDX							0
#define HDMIRX_CRRHVPERIOD_CRRHPE_12_8_IDX							1
#define HDMIRX_CRRHVPERIOD_CRRHPE_12_8_IDX_BIT						8
#define HDMIRX_CRRHVPERIOD_CRRHPE_7_0_MASK							0xFF
#define HDMIRX_CRRHVPERIOD_CRRHPE_12_8_MASK							0x1F
#define HDMIRX_CRRHVPERIOD_CRRHSW_7_0_IDX							4
#define HDMIRX_CRRHVPERIOD_CRRHSW_8_8_IDX							5
#define HDMIRX_CRRHVPERIOD_CRRHSW_8_8_IDX_BIT						8
#define HDMIRX_CRRHVPERIOD_CRRHSW_7_0_MASK							0xFF
#define HDMIRX_CRRHVPERIOD_CRRHSW_8_8_MASK							0x01
#define HDMIRX_CRRHVPERIOD_CRRHDE_7_0_IDX							8
#define HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX							9
#define HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX_BIT						8
#define HDMIRX_CRRHVPERIOD_CRRHDE_7_0_MASK							0xFF
#define HDMIRX_CRRHVPERIOD_CRRHDE_12_8_MASK							0x1F
#define HDMIRX_CRRHVPERIOD_CRRVDE_7_0_IDX							10
#define HDMIRX_CRRHVPERIOD_CRRVDE_11_8_IDX							11
#define HDMIRX_CRRHVPERIOD_CRRVDE_11_8_IDX_BIT						8
#define HDMIRX_CRRHVPERIOD_CRRVDE_7_0_MASK							0xFF
#define HDMIRX_CRRHVPERIOD_CRRVDE_11_8_MASK							0x0F
#define HDMIRX_PIXEL_CLOCK_L556800KHZ								554016		// 556800 *  99.5%
#define HDMIRX_PIXEL_CLOCK_H556800KHZ								559584		// 556800 * 100.5%
#define HDMIRX_PIXEL_CLOCK_L346500KHZ								344767		// 346500 *  99.5%
#define HDMIRX_PIXEL_CLOCK_H346500KHZ								348233		// 346500 * 100.5%
#define HDMIRX_PIXEL_CLOCK_L297000KHZ								296900
#define HDMIRX_PIXEL_CLOCK_H297000KHZ								297100
#define HDMIRX_PIXEL_CLOCK_L288000KHZ								287900
#define HDMIRX_PIXEL_CLOCK_H288000KHZ								288100
#define HDMIRX_PIXEL_CLOCK_L285500KHZ								284072		// 285500 *  99.5%
#define HDMIRX_PIXEL_CLOCK_H285500KHZ								286928		// 285500 * 100.5%
#define HDMIRX_PIXEL_CLOCK_L268500KHZ								268400
#define HDMIRX_PIXEL_CLOCK_H268500KHZ								268600
#define HDMIRX_PIXEL_CLOCK_L261000KHZ								260900
#define HDMIRX_PIXEL_CLOCK_H261000KHZ								261100
#define HDMIRX_PIXEL_CLOCK_L241500KHZ								241400
#define HDMIRX_PIXEL_CLOCK_H241500KHZ								241600
#define HDMIRX_PIXEL_CLOCK_L234000KHZ								233900
#define HDMIRX_PIXEL_CLOCK_H234000KHZ								234100
#define HDMIRX_PIXEL_CLOCK_L218250KHZ								218150
#define HDMIRX_PIXEL_CLOCK_H218250KHZ								218350
#define HDMIRX_PIXEL_CLOCK_L204750KHZ								204650
#define HDMIRX_PIXEL_CLOCK_H204750KHZ								204850
#define HDMIRX_HDE_2560												2560
#define HDMIRX_HDE_1920												1920
#define HDMIRX_HDE_1856												1856
#define HDMIRX_HDE_1792												1792
#define HDMIRX_HDE_1152												1152
#define HDMIRX_VDE_2160												2160
#define HDMIRX_VDE_1600												1600
#define HDMIRX_VDE_1440												1440
#define HDMIRX_VDE_1392												1392
#define HDMIRX_VDE_1344												1344
#define HDMIRX_VDE_1080												1080
#define HDMIRX_VDE_864												864
#define HDMIRX_HPE_3300												3300
#define HDMIRX_HPE_2720												2720
#define HDMIRX_HPE_2640												2640
#define HDMIRX_HPE_2600												2600
#define HDMIRX_HPE_2560												2560
#define HDMIRX_HPE_2528												2528
#define HDMIRX_HPE_2456												2456
#define HDMIRX_HPE_2448												2448
#define HDMIRX_HPE_2200												2200
#define HDMIRX_HPE_2080												2080
#define HDMIRX_HPE_2000												2000
#define HDMIRX_HPE_1600												1600
#define HDMIRX_HSW_224												224
#define HDMIRX_HSW_216												216
#define HDMIRX_HSW_208												208
#define HDMIRX_HSW_200												200
#define HDMIRX_HSW_128												128
#define HDMIRX_HSW_80												80
#define HDMIRX_HSW_44												44
#define HDMIRX_HSW_32												32
//------------------------------------------------------------------------------
// hdmirx_get_vformat1()
#define HDMIRX_VFMDET_00_DETVFM_3840X2160P_24HZ						0xCF
#define HDMIRX_VFMDET_00_DETVFM_3840X2160P_25_50HZ					0xCE
#define HDMIRX_VFMDET_00_DETVFM_3840X2160P_30_60HZ					0xCD
#define HDMIRX_VFMDET_00_DETVFM_4096X2160P_24HZ						0xCC
#define HDMIRX_VFMDET_00_DETVFM_4096X2160P_25_50HZ					0xD1
#define HDMIRX_VFMDET_00_DETVFM_4096X2160P_30_60HZ					0xD0
#define HDMIRX_VFMDET_00_DETVFM_FHD_3D_UHD_420_60HZ					0xC5
#define HDMIRX_VFMDET_00_DETVFM_FHD_3D_UHD_420_50HZ					0xC6
//------------------------------------------------------------------------------
// hdmirx_video_format_detect()
#define HDMIRX_VIDEO_FORMAT_DETECT_RETRY_MAX						3
//------------------------------------------------------------------------------
// hdmirx_select_page_of_0x14()
#define HDMIRX_SELECT_PAGE_OF_0X14_NUM								1
#define HDMIRX_AVMMASK_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_hdmi_mode()
#define HDMIRX_GET_HDMI_MODE_NUM									1
#define HDMIRX_BSTARP0_01_HDMI_MODE_MASK							0x00000010
//------------------------------------------------------------------------------
// hdmirx_get_packet_status0()
#define HDMIRX_GET_PACKET_STATUS0_NUM								1
#define HDMIRX_PKTDETS0_FREDETS_MASK								0x00000080
//------------------------------------------------------------------------------
// hdmirx_get_packet_status1()
#define HDMIRX_GET_PACKET_STATUS1_NUM								1
#define HDMIRX_PKTDETS1_AVIDETS_MASK								0x00000001
#define HDMIRX_PKTDETS1_SPDDETS_MASK								0x00000002
//------------------------------------------------------------------------------
// hdmirx_get_packet_status2()
#define HDMIRX_GET_PACKET_STATUS2_NUM								1
#define HDMIRX_PKTDETS2_HVSDETS_MASK								0x00000010
#define HDMIRX_PKTDETS2_FVSDETS_MASK								0x00000020
//------------------------------------------------------------------------------
// hdmirx_get_packet_param_change0()
#define HDMIRX_GET_PACKET_PARAM_CHANGE0_NUM							1
#define HDMIRX_PPMCHG0_FRECHG_MASK									0x00000080
//------------------------------------------------------------------------------
// hdmirx_get_packet_param_change1()
#define HDMIRX_GET_PACKET_PARAM_CHANGE1_NUM							1
#define HDMIRX_PPMCHG1_AVICHG_MASK									0x00000001
#define HDMIRX_PPMCHG1_SPDCHG_MASK									0x00000002
//------------------------------------------------------------------------------
// hdmirx_clr_packet_param_change0()
#define HDMIRX_CLR_PACKET_PARAM_CHANGE0_NUM							1
#define HDMIRX_PPMCHG0_ALL_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_clr_packet_param_change1()
#define HDMIRX_CLR_PACKET_PARAM_CHANGE1_NUM							1
#define HDMIRX_PPMCHG1_ALL_MASK										0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_free_packet_header()
#define HDMIRX_SET_FREE_PACKET_HEADER_NUM							1
#define HDMIRX_RFREPKT_FRE00H_HDR_TYPE								0x87
#define HDMIRX_RFREPKT_FRE00H_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmirx_get_avi_infoframe()
#define HDMIRX_GET_AVI_INFOFRAME_LEN								9
#define HDMIRX_AVI_IF_VERSION_INDEX									1
#define HDMIRX_AVI_IF_VERSION_2										0x02
#define HDMIRX_AVI_IF_VERSION_3										0x03
#define HDMIRX_AVI_IF_Y_INDEX										4
#define HDMIRX_AVI_IF_Y_MASK										0xE0
#define HDMIRX_AVI_IF_Y_RGB											0x00
#define HDMIRX_AVI_IF_Y_YCBCR422									0x20
#define HDMIRX_AVI_IF_Y_YCBCR444									0x40
#define HDMIRX_AVI_IF_Y_YCBCR420									0x60
#define HDMIRX_AVI_IF_C_INDEX										5
#define HDMIRX_AVI_IF_C_MASK										0xC0
#define HDMIRX_AVI_IF_C_NO_DATA										0x00
#define HDMIRX_AVI_IF_C_BT601										0x40
#define HDMIRX_AVI_IF_C_BT709										0x80
#define HDMIRX_AVI_IF_C_EXTENDED									0xC0
#define HDMIRX_AVI_IF_EC_INDEX										6
#define HDMIRX_AVI_IF_EC_MASK										0x70
#define HDMIRX_AVI_IF_EC_XVYCC601									0x00
#define HDMIRX_AVI_IF_EC_XVYCC709									0x10
#define HDMIRX_AVI_IF_EC_ADOBERGB									0x40
#define HDMIRX_AVI_IF_EC_BT2020_RGB_YCBCR							0x60
#define HDMIRX_AVI_IF_Q_INDEX										6
#define HDMIRX_AVI_IF_Q_MASK										0x0C
#define HDMIRX_AVI_IF_Q_DEFAULT										0x00
#define HDMIRX_AVI_IF_Q_LIMITED_RANGE								0x04
#define HDMIRX_AVI_IF_Q_FULL_RANGE									0x08
#define HDMIRX_AVI_IF_VIC_INDEX										7
//------------------------------------------------------------------------------
// hdmirx_get_hdmi_vs_infoframe()
#define HDMIRX_GET_HDMI_VS_INFOFRAME_LEN							9
#define HDMIRX_HVS_IF_HDMI_VIDEO_FORMAT_INDEX						7
#define HDMIRX_HVS_IF_HDMI_VIDEO_FORMAT_MASK						0xE0
#define HDMIRX_HVS_IF_HDMI_VIDEO_FORMAT_EXT_FORMAT					0x20
#define HDMIRX_HVS_IF_HDMI_VIC_INDEX								8
#define HDMIRX_HVS_IF_HDMI_VIC_4K_24HZ								0x03
#define HDMIRX_HVS_IF_HDMI_VIC_4K_SMPTE_24HZ						0x04
#define HDMIRX_HVS_IF_HDMI_VIC_4K_25HZ								0x02
#define HDMIRX_HVS_IF_HDMI_VIC_4K_30HZ								0x01
//------------------------------------------------------------------------------
// hdmirx_get_pixel_repetition()
#define HDMIRX_VOUTCNT_NREPST_REPETITION_MANUAL_0					0x00
#define HDMIRX_VOUTCNT_NREPST_REPETITION_MANUAL_2					0x40
#define HDMIRX_VOUTCNT_NREPST_REPETITION_MANUAL_4					0x80
#define HDMIRX_VOUTCNT_NREPST_REPETITION_AUTO						0xC0
//------------------------------------------------------------------------------
// hdmirx_set_pixel_repetition()
#define HDMIRX_SET_PIXEL_REPETITION_LEN								2
//------------------------------------------------------------------------------
// hdmirx_set_rxpll()
#define HDMIRX_SET_RXPLL_LEN										6
#define HDMIRX_RXPLL0_0_DEF											0x82
#define HDMIRX_RXPLL0_0_MODE1										0x83
#define HDMIRX_RXPLL0_0_MODE3										HDMIRX_RXPLL0_0_MODE1
#define HDMIRX_RXPLL0_1_DEF											0x18
#define HDMIRX_RXPLL0_1_MODE1										0xA0
#define HDMIRX_RXPLL0_1_MODE3										0x90
#define HDMIRX_RXPLL00_0_DEF										0xE8
#define HDMIRX_RXPLL00_0_MODE1										0xE9
#define HDMIRX_RXPLL00_0_MODE3										0xD1
#define HDMIRX_RXPLL00_1_DEF										0x43
#define HDMIRX_RXPLL00_1_MODE1										HDMIRX_RXPLL00_1_DEF
#define HDMIRX_RXPLL00_1_MODE3										0x4F
#define HDMIRX_RXPLL00_2_DEF										0xF4
#define HDMIRX_RXPLL00_2_MODE1										HDMIRX_RXPLL00_2_DEF
#define HDMIRX_RXPLL00_2_MODE3										HDMIRX_RXPLL00_2_DEF
#define HDMIRX_RXPLL00_3_DEF										0x01
#define HDMIRX_RXPLL00_3_MODE1										0x09
#define HDMIRX_RXPLL00_3_MODE3										HDMIRX_RXPLL00_3_MODE1
//------------------------------------------------------------------------------
// hdmirx_set_rxcore()
#define HDMIRX_SET_RXCORE_LEN										4
#define HDMIRX_RXCORE0_0_DEF										0x14
#define HDMIRX_RXCORE0_0_MODE1										0x18
#define HDMIRX_RXCORE0_0_MODE2										HDMIRX_RXCORE0_0_MODE1
#define HDMIRX_RXCORE0_0_MODE3										HDMIRX_RXCORE0_0_MODE1
#define HDMIRX_RXCORE0_1_DEF										0xE0
#define HDMIRX_RXCORE0_1_MODE1										0x07
#define HDMIRX_RXCORE0_1_MODE2										0x08
#define HDMIRX_RXCORE0_1_MODE3										0x06
#define HDMIRX_RXCORE0_2_DEF										0x00
#define HDMIRX_RXCORE0_2_MODE1										0x8A
#define HDMIRX_RXCORE0_2_MODE2										HDMIRX_RXCORE0_2_MODE1
#define HDMIRX_RXCORE0_2_MODE3										HDMIRX_RXCORE0_2_MODE1
#define HDMIRX_RXCORE0_3_DEF										0x20
#define HDMIRX_RXCORE0_3_MODE1										HDMIRX_RXCORE0_3_DEF
#define HDMIRX_RXCORE0_3_MODE2										HDMIRX_RXCORE0_3_DEF
#define HDMIRX_RXCORE0_3_MODE3										HDMIRX_RXCORE0_3_DEF
//------------------------------------------------------------------------------
// hdmirx_set_dfmt()
#define HDMIRX_SET_DFMT_NUM											1
#define HDMIRX_SWMODE4_02_MASK										0x000000FF
#define HDMIRX_SWMODE4_02_DFMT_DEF									0x00
#define HDMIRX_SWMODE4_02_DFMT_3840X2160P_30HZ						0x08
#define HDMIRX_SWMODE4_02_DFMT_3840X2160P_25HZ						0x10
#define HDMIRX_SWMODE4_02_DFMT_3840X2160P_24HZ						0x18
#define HDMIRX_SWMODE4_02_DFMT_4096X2160P_24HZ						0x20
#define HDMIRX_SWMODE4_02_DFMT_4096X2160P_25HZ						0x28
#define HDMIRX_SWMODE4_02_DFMT_4096X2160P_30HZ						0x30
#define HDMIRX_SWMODE4_02_DFMT_3840X2160P_60HZ						0x38
#define HDMIRX_SWMODE4_02_DFMT_3840X2160P_50HZ						0x40
#define HDMIRX_SWMODE4_02_DFMT_4096X2160P_60HZ						0x48
#define HDMIRX_SWMODE4_02_DFMT_4096X2160P_50HZ						0x50
//------------------------------------------------------------------------------
// hdmirx_get_audio_status_mon()
#define HDMIRX_GET_AUDIO_STATUS_MON_LEN								2
#define HDMIRX_RXAMON2_RFMTSTATUS_AS								0x00
#define HDMIRX_RXAMON2_RFMTSTATUS_OBA								0x01
#define HDMIRX_RXAMON2_RFMTSTATUS_HBR								0x02
#define HDMIRX_RXAMON2_RFMTSTATUS_DST								0x03
#define HDMIRX_RXAMON2_RFMTSTATUS_MS_AS								0x08
#define HDMIRX_RXAMON2_RFMTSTATUS_MS_OBA							0x09
#define HDMIRX_RXAMON2_RFMTSTATUS_MASK								0x0000000F
//------------------------------------------------------------------------------
// hdmirx_set_sampling_freq()
#define HDMIRX_SET_SAMPLING_FREQ_NUM								1
#define HDMIRX_RXCRCTL_FSALT_HW										0x00
#define HDMIRX_RXCRCTL_FSALT_SW										0x02
#define HDMIRX_RXCRCTL_FSALT_MASK									0x00000002
//------------------------------------------------------------------------------
// hdmirx_set_sw_sampling_freq()
#define HDMIRX_SET_SW_SAMPLING_FREQ_NUM								1
#define HDMIRX_RXFSCTL_MASK											0x000000FF
#define HDMIRX_RXCBMON_03_FS_EXT_RX_BIT								6
#define HDMIRX_RXFSCTL_SWSEXT_BIT									4
//------------------------------------------------------------------------------
// hdmirx_get_audio_infoframe()
#define HDMIRX_GET_AUDIO_INFOFRAME_LEN								14
//------------------------------------------------------------------------------
// hdmirx_get_audio_cbit()
#define HDMIRX_GET_AUDIO_CBIT_LEN									8
//------------------------------------------------------------------------------
// hdmirx_get_aformat()
#define INFOFRAME_VERSION_AUDIO										0x01
#define HDMIRX_RXCBMON_03_FS_RX_MASK								0x0F
#define HDMIRX_RXCBMON_03_FS_EXT_RX_MASK							0xC0
#define HDMIRX_RXCBMON_04_WORD_LEN_RX_MASK							0x0F
#define HDMIRX_RXAUDIMON_04_CC_MASK									0x07
#define HDMIRX_RXAUDIMON_05_SF_MASK									0x1C
#define HDMIRX_RXAUDIMON_05_SS_MASK									0x03
#define HDMIRX_RXAUDIMON_07_CA_MASK									0xFF
#define HDMIRX_RXAUDIMON_08_LSV_MASK								0x78
#define HDMIRX_RXAUDIMON_08_LSV0_BIT								3
//------------------------------------------------------------------------------
// hdmirx_get_spd_infoframe_data()
#define HDMIRX_GET_SPD_INFOFRAME_DATA_LEN							29
#define HDMIRX_RXSPDIMON_VN_IDX										4
#define HDMIRX_RXSPDIMON_PD_IDX										(HDMIRX_RXSPDIMON_VN_IDX \
																	+ SYSTEM_SPD_INFOFRAME_VN_BYTE_SIZE)
#define HDMIRX_RXSPDIMON_SI_IDX										(HDMIRX_RXSPDIMON_PD_IDX \
																	+ SYSTEM_SPD_INFOFRAME_PD_BYTE_SIZE)
//------------------------------------------------------------------------------
// hdmirx_get_hdr_infoframe_data()
#define HDMIRX_GET_HDR_INFOFRAME_DATA_LEN							30
#define HDMIRX_RFREPKT_EOTF_IDX										4
#define HDMIRX_RFREPKT_EOTF_MASK									0x00000007
#define HDMIRX_RFREPKT_SMD_ID_IDX									5
#define HDMIRX_RFREPKT_SMD_ID_MASK									0x00000007
#define HDMIRX_RFREPKT_SMD_IDX										6
//------------------------------------------------------------------------------
// hdmirx_select_page_of_0x18()
#define HDMIRX_SELECT_PAGE_OF_0X18_NUM								1
#define HDMIRX_SELECT_PAGE_OF_0X18_EDID								0x00
#define HDMIRX_SELECT_PAGE_OF_0X18_SCDCS							0x10
#define HDMIRX_SELECT_PAGE_OF_0X18_SYSCFG14_MASK					0x000000FF
//------------------------------------------------------------------------------
// hdmirx_reset_all_irq()
#define HDMIRX_RESET_ALL_IRQ_CLR									0xFF
#define HDMIRX_RESET_ALL_IRQ_MASK									0x000000FF
//------------------------------------------------------------------------------
// hdmirx_check_port_status()
#define HDMIRX_CHECK_PORT_STATUS_POWER_ON							1
#define HDMIRX_CHECK_PORT_STATUS_POWER_DOWN							0
#define HDMIRX_INTREG8_P0V5STCHG_BIT								0x10
//------------------------------------------------------------------------------
// hdmirx_set_rx_soft_reset1()
#define HDMIRX_SET_RX_SOFT_RESET1_NUM								1
#define HDMIRX_RSRST1_VIFRST										0x08
#define HDMIRX_RSRST1_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_set_rx_soft_reset3()
#define HDMIRX_SET_RX_SOFT_RESET3_NUM								1
#define HDMIRX_RSRST3_PFQRST										0x04
#define HDMIRX_RSRST3_DCMRST										0x08
#define HDMIRX_RSRST3_MASK											0x000000FF
//------------------------------------------------------------------------------
// hdmirx_re_calibrate_tmds()
#define HDMIRX_RE_CALIBRATE_TMDS_RETRY_MAX							10
//------------------------------------------------------------------------------
// hdmirx_re_detect_deep_color()
#define HDMIRX_RE_DETECT_DEEP_COLOR_RETRY_MAX						10
//------------------------------------------------------------------------------
#define HDMIRX_SADDR_10_PORT_OFFSET									0x00000001
#define HDMIRX_SADDR_12_PORT_OFFSET									0x00000020
#define HDMIRX_SADDR_12D_PORT_OFFSET								0x00000004
#define HDMIRX_SADDR_1C_PORT_OFFSET									0x00000004
#define HDMIRX_SADDR_1E_PORT_OFFSET									0x00000010
//------------------------------------------------------------------------------
// for TMDS setting
#define HDMIRX_SYSCLK					27				// 27MHz
#define HDMIRX_MAXTCC					0x00010000
#define HDMIRX_CALC_FDTTHA()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK /  20)
#define HDMIRX_CALC_FDTTHB()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK /  45)
#define HDMIRX_CALC_FDTTHC()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK /  85.5)
#define HDMIRX_CALC_FDTTHD()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK / 123)
#define HDMIRX_CALC_FDTTHE()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK / 175)
#define HDMIRX_CALC_FDTTHF()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK / 250)
#define HDMIRX_CALC_FDTTHG()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK / 342)
#define HDMIRX_CALC_FDTTHH()			((uint32_t)HDMIRX_MAXTCC * HDMIRX_SYSCLK / 492)
#define HDMIRX_CALC_TMRTM(u)			((uint32_t)(u) * HDMIRX_SYSCLK)
#define HDMIRX_CALC_PSELTM(u)			((uint32_t)(u) * HDMIRX_SYSCLK / 256)
#define HDMIRX_CALC_PAVRTM(u)			((uint32_t)(u) * HDMIRX_SYSCLK / 256)
//------------------------------------------------------------------------------
// for Link stting
#define HDMIRX_ROUNDUP(a, b)			(uint32_t)(!(b) ? 0 : (((a) + (b) - 1) / (b)))
#define HDMIRX_CALC_MAXHCC(u)			((uint32_t)(u) * HDMIRX_SYSCLK)
#define HDMIRX_CALC_DPCRTM(u)			((uint32_t)(u) * HDMIRX_SYSCLK / 256)
#define HDMIRX_CALC_VAIRTM(u)			((uint32_t)(u) * HDMIRX_SYSCLK)
#define HDMIRX_CALC_HPRRT0(n)			((HDMIRX_ROUNDUP(((uint32_t)(n) * HDMIRX_SYSCLK / 1000), 16) - 1))
#define HDMIRX_CALC_HPRRT1(u)			((HDMIRX_ROUNDUP(((uint32_t)(u) * HDMIRX_SYSCLK), 32)))
//------------------------------------------------------------------------------
#define HDMIRX_SPTVFM_ALL0											(uint32_t)0xFFFFFFFF
#define HDMIRX_SPTVFM_ALL1											(uint32_t)0x000003FF
#define HDMIRX_SPTPFM_ALL0											(uint32_t)0x003FFFFF
#define HDMIRX_SPTPFM_ALL1											(uint32_t)0x00000000
#define HDMIRX_SPTHFM_ALL0											(uint32_t)0x0001F87F
//------------------------------------------------------------------------------
#define HDMIRX_MASK_03												0x00000003
#define HDMIRX_MASK_FF												0x000000FF
//------------------------------------------------------------------------------
// SLEEP() time
#define HDMIRX_SLEEP_TIME_1MSEC										1
#define HDMIRX_SLEEP_TIME_5MSEC										5
#define HDMIRX_SLEEP_TIME_20MSEC									20
#define HDMIRX_SLEEP_TIME_80MSEC									80
#define HDMIRX_SLEEP_TIME_100MSEC									100
#define HDMIRX_SLEEP_TIME_200MSEC									200
//------------------------------------------------------------------------------
// hdmirx_set_tmdsrx6_of_portno()
static const uint8_t hdmirx_tmdsrx_param[HDMIRX_TMDSRXPX6_PARAM_KIND_NUM][HDMIRX_TMDSRXPX6_PARAM_NUM] =
{
	// when 0x120C == 0x0F
	{0xE8, 0xE8, 0xE8, 0xA8, 0xA8, 0xA5, 0x18, 0x18, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x03, 0x03},
	// when 0x120C == 0x8F
	{0x02, 0x02, 0x02, 0x14, 0x17, 0x2A, 0x17, 0x1A, 0x14, 0x14, 0x18, 0x12, 0x14, 0x14, 0x1C, 0x1C}
};
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
typedef enum _hdmirx_mode {
	HDMIRX_MODE_BYPASS,
	HDMIRX_MODE_SWITCH,
	HDMIRX_MODE_REPEAT,
	HDMIRX_MODE_reserve
} hdmirx_mode;
//------------------------------------------------------------------------------
typedef enum _hdmirx_state {
	HDMIRX_STATE_NO_SIGNAL,
	HDMIRX_STATE_PLUGED,
	HDMIRX_STATE_STABLE,
	HDMIRX_STATE_UNPLUGED,
	HDMIRX_STATE_VIDEO_DETECT,
	HDMIRX_STATE_WAIT_ALIGNMENT_LOCK,
	HDMIRX_STATE_SET_TMDS_CLOCK,
	HDMIRX_STATE_HDCPTX,
	HDMIRX_STATE_SET_TMDS_CLOCK_BYPASS
} hdmirx_state;
//------------------------------------------------------------------------------
// HDMI RX Audio Master Clock
typedef enum hdmirx_audio_mclk {
	HDMIRX_AUDIO_MCLK_128FS,
	HDMIRX_AUDIO_MCLK_192FS,
	HDMIRX_AUDIO_MCLK_256FS,
	HDMIRX_AUDIO_MCLK_384FS,
	HDMIRX_AUDIO_MCLK_512FS,
	HDMIRX_AUDIO_MCLK_768FS,
	HDMIRX_AUDIO_MCLK_1024FS,
	HDMIRX_AUDIO_MCLK_RESERVED
} hdmirx_audio_mclk;
//------------------------------------------------------------------------------
typedef enum _hdmirx_fast_port {
	HDMIRX_FAST_PORT_DISABLE,
	HDMIRX_FAST_PORT_ENABLE
} hdmirx_fast_port;
//------------------------------------------------------------------------------
// TMDS speed mode
typedef enum _hdmirx_tmds_speed_mode {
	HDMIRX_TMDS_SPEED_MODE_LEGACY,
	HDMIRX_TMDS_SPEED_MODE_HIGH
} hdmirx_tmds_speed_mode;
//------------------------------------------------------------------------------
// SD, HD, UHD
typedef enum _hdmirx_video_definition {
	HDMIRX_VIDEO_DEFINITION_SD,
	HDMIRX_VIDEO_DEFINITION_HD,
	HDMIRX_VIDEO_DEFINITION_UHD,
} hdmirx_video_definition;
//------------------------------------------------------------------------------
typedef enum _hdmirx_ctrl_req {
	HDMIRX_CTRL_REQ_GET,
	HDMIRX_CTRL_REQ_SET,
	HDMIRX_CTRL_REQ_MAX
} hdmirx_ctrl_req;
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_switch_mode123(system_port port);
static uint32_t hdmirx_set_irq_main_status_mask(system_port port);
static uint32_t hdmirx_select_hdmirx(system_port port);
static uint32_t hdmirx_select_hdmi_mode(system_port port);
static uint32_t hdmirx_set_sysconf10(system_port port, uint8_t *p_conf);
static uint32_t hdmirx_set_sysconf10_of_portno(system_port port, uint8_t no, uint8_t conf, uint8_t mask);
static uint32_t hdmirx_get_irq0(system_port port, uint8_t *p_irq);
static uint32_t hdmirx_get_irq0_of_portno(system_port port, uint8_t no, uint8_t *p_irq, uint8_t mask);
static uint32_t hdmirx_set_irq0_of_portno(system_port port, uint8_t no, uint8_t irq, uint8_t mask);
static uint32_t hdmirx_set_irq4_of_portno(system_port port, uint8_t no, uint8_t irq, uint8_t mask);
static uint32_t hdmirx_set_tmdsrx1_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask);
static uint32_t hdmirx_set_tmdsrx1_0_2_of_portno(system_port port, uint8_t no, uint8_t tmds0, uint8_t mask0,
												uint8_t tmds1, uint8_t mask1, uint8_t tmds2, uint8_t mask2);
static uint32_t hdmirx_set_tmdsrx2_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask);
static uint32_t hdmirx_set_tmdsrx4_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask);
static uint32_t hdmirx_set_tmdsrx5_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask);
static uint32_t hdmirx_get_tmdsrx5_1_3_of_portno(system_port port, uint8_t no, uint8_t *p_tmds);
static uint32_t hdmirx_set_tmdsrx6_of_portno(system_port port, uint8_t no, const uint8_t *p_tmds);
static uint32_t hdmirx_get_auth_phase_of_portno(system_port port, uint8_t no, uint8_t *p_auphs,
												uint8_t *p_tcksel, uint8_t *p_pssyen);
static uint32_t hdmirx_set_switch_mode1(system_port port);
static uint32_t hdmirx_set_unselected_ddc_tmds_of_portno(system_port port, uint8_t no);
static uint32_t hdmirx_get_tmds_clock_freq_of_portno(system_port port, uint8_t no, uint8_t *p_freq);
static uint32_t hdmirx_init_ddc_ctrl(system_port port);
static uint32_t hdmirx_set_scdcs_mode(system_port port, uint8_t scdsc);
static uint32_t hdmirx_get_scramble_mon_of_portno(system_port port, uint8_t no, uint8_t *p_scrmon);
static uint32_t hdmirx_set_tmds_maxtcc(system_port port);
static uint32_t hdmirx_set_tmds_fchgth(system_port port);
static uint32_t hdmirx_set_tmds_fdttha_h(system_port port);
static uint32_t hdmirx_set_tmds_maxhcc(system_port port);
static uint32_t hdmirx_set_tmds_tckthrb(system_port port);
static uint32_t hdmirx_tmds_freq_detect_param(system_port port);
static uint32_t hdmirx_set_auto_reset_mode(system_port port);
static uint32_t hdmirx_set_tmds_reset_lock_time(system_port port);
static uint32_t hdmirx_set_fast_port_switch(system_port port);
static uint32_t hdmirx_tmds_reset_seq_param(system_port port);
static uint32_t hdmirx_set_auto_scramble_detect_mode(system_port port);
static uint32_t hdmirx_set_scramble_seed(system_port port);
static uint32_t hdmirx_set_scramble_mode(system_port port);
static uint32_t hdmirx_tmds_scramble_detect_mode(system_port port);
static uint32_t hdmirx_set_tmds_param(system_port port);
static uint32_t hdmirx_set_lump_mode(system_port port, uint8_t lump);
static uint32_t hdmirx_set_tmds_auto_speed_mode(system_port port, uint8_t mode);
static uint32_t hdmirx_set_clock_freq_detect_param2(system_port port, uint8_t mode, uint8_t mask);
static uint32_t hdmirx_set_tmds_rx_param(system_port port, uint8_t mode);
static uint32_t hdmirx_set_fast_port_switch_mode(system_port port, hdmirx_fast_port fast);
static uint32_t hdmirx_set_intr_mask_port(system_port port);
static uint32_t hdmirx_set_intr_status_mask_port(system_port port);
static uint32_t hdmirx_tmds_receiver_init(system_port port);
static uint32_t hdmirx_select_page_of_0x60c4(system_port port, uint8_t page);
static uint32_t hdmirx_set_link_freq_detect_param(system_port port);
static uint32_t hdmirx_set_reset_seq_time1(system_port port);
static uint32_t hdmirx_set_reset_seq_time2(system_port port);
static uint32_t hdmirx_set_clock_freq_detect_param(system_port port);
static uint32_t hdmirx_set_link_reset_seq_param(system_port port);
static uint32_t hdmirx_set_sptvfm(system_port port);
static uint32_t hdmirx_set_sptpfm(system_port port);
static uint32_t hdmirx_set_spthfm(system_port port);
static uint32_t hdmirx_set_video_if_param(system_port port);
static uint32_t hdmirx_set_audio_clock_regen_mode(system_port port);
static uint32_t hdmirx_set_support_audio_format(system_port port);
static uint32_t hdmirx_set_output_i2s_mode(system_port port);
static uint32_t hdmirx_set_output_master_clock_freq(system_port port);
static uint32_t hdmirx_set_audio_ch_assign(system_port port, uint8_t chasgn);
static uint32_t hdmirx_set_audio_mute_ctrl(system_port port);
static uint32_t hdmirx_set_mode_ctrl1(system_port port, uint8_t mode, uint8_t mask);
static uint32_t hdmirx_set_mode_ctrl2(system_port port, uint8_t mode, uint8_t mask);
static uint32_t hdmirx_set_audio_if_param(system_port port);
static uint32_t hdmirx_select_page_of_0x68c0(system_port port, uint8_t page);
static uint32_t hdmirx_set_packet_receive_mode(system_port port);
static uint32_t hdmirx_set_packet_hold(system_port port, uint8_t ctrl);
static uint32_t hdmirx_set_hdmi_forum_vsi_id(system_port port);
static uint32_t hdmirx_set_packet_infoframe_param(system_port port);
static uint32_t hdmirx_set_intr_mask_rx(system_port port);
static uint32_t hdmirx_set_intr_mask_rx_1(system_port port, uint8_t intr, uint8_t mask);
static uint32_t hdmirx_set_intr_audio_packet_mask_rx(system_port port);
static uint32_t hdmirx_set_intr_packet_mask_rx(system_port port);
static uint32_t hdmirx_set_ext_intr_mask_rx(system_port port);
static uint32_t hdmirx_set_intr_status_mask_rx(system_port port);
static uint32_t hdmirx_receiver_link_init(system_port port);
static uint32_t hdmirx_get_selected_rx_port(system_port port, uint8_t *p_port);
static uint32_t hdmirx_receiver_port_select(system_port port);
static uint32_t hdmirx_set_tmds_data_swp_pol_portno(system_port port, uint8_t no, uint8_t swp, uint8_t inv);
static uint32_t hdmirx_set_tmds_data_swp(system_port port);
static uint32_t hdmirx_get_irq_5v_power(system_port port, uint8_t *p_irq);
static uint32_t hdmirx_clr_irq_5v_power(system_port port, uint8_t irq);
static uint32_t hdmirx_get_mon_5v_power(system_port port, uint8_t *p_mon);
static uint32_t hdmirx_get_irq_tmds_clock(system_port port, uint8_t *p_irq);
static uint32_t hdmirx_get_rx_irq1(system_port port, uint8_t *p_irq, uint8_t mask);
static uint32_t hdmirx_set_rx_irq1(system_port port, uint8_t irq, uint8_t mask);
static uint32_t hdmirx_get_rx_irq3(system_port port, uint8_t *p_irq, uint8_t mask);
static uint32_t hdmirx_set_rx_irq3(system_port port, uint8_t irq, uint8_t mask);
static uint32_t hdmirx_get_rx_irq6(system_port port, uint8_t *p_irq, uint8_t mask);
static uint32_t hdmirx_set_rx_irq6(system_port port, uint8_t irq, uint8_t mask);
static uint32_t hdmirx_get_rx_mon(system_port port, uint8_t *p_mon, uint8_t mask);
static uint32_t hdmirx_get_rx_video_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute);
static uint32_t hdmirx_get_rx_audio_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute);
static uint32_t hdmirx_get_rx_avi_infoframe_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute);
static uint32_t hdmirx_get_rx_spd_infoframe_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute);
static uint32_t hdmirx_get_rx_hdr_infoframe_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute);
static uint32_t hdmirx_set_scdcs_source_ver(system_port port, uint8_t scdcs);
static uint32_t hdmirx_get_scdcs_tmds_config(system_port port, uint8_t *p_scdcs, uint8_t mask);
static uint32_t hdmirx_set_scdcs_tmds_config(system_port port, uint8_t scdcs);
static uint32_t hdmirx_set_scdcs_scrambler_status(system_port port, uint8_t scdcs);
static uint32_t hdmirx_set_scdcs_scrambling_status(system_port port, uint8_t no);
static uint32_t hdmirx_access_tmds_speed_mode(system_port port, hdmirx_tmds_speed_mode *p_get,
																hdmirx_tmds_speed_mode *p_set);
static uint32_t hdmirx_tmds_alignment_lock_detect(system_port port);
static uint32_t hdmirx_set_equalizer_max_limit_of_portno(system_port port, uint8_t no, uint8_t max);
static uint32_t hdmirx_tmds_calibration(system_port port);
static uint32_t hdmirx_set_rxpll0en(system_port port, uint8_t en);
static uint32_t hdmirx_set_rxpll00en(system_port port, uint8_t en);
static uint32_t hdmirx_set_rx0_sen(system_port port, uint8_t en);
static uint32_t hdmirx_set_pixel_clock(system_port port);
static uint32_t hdmirx_change_receiver_clock_setting(system_port port);
static uint32_t hdmirx_get_gcp_color_depth(system_port port, uint8_t *p_cd);
static uint32_t hdmirx_get_tmds_clock_cnt_of_portno(system_port port, uint8_t no, uint8_t *p_cnt);
static uint32_t hdmirx_get_tmds_data_rate_of_portno(system_port port, uint8_t no, uint8_t *p_rate);
static uint32_t hdmirx_get_calc_pixel_clock_freq(system_port port, uint8_t pckmon, uint32_t *p_pcf);
static uint32_t hdmirx_get_calc_pixel_clock_range(system_port port, uint8_t pckmon, uint8_t *p_pcr);
static uint32_t hdmirx_get_pixel_clock_freq(system_port port, uint32_t *p_pcf);
static uint32_t hdmirx_get_pixel_clock_range(system_port port, uint8_t *p_pcr);
static uint32_t hdmirx_get_detected_vformat(system_port port, uint8_t *p_vform);
static uint32_t hdmirx_set_current_param_latch(system_port port);
static uint32_t hdmirx_get_hori_vert_period(system_port port, uint8_t *p_period);
static uint32_t hdmirx_get_current_hori_vert_period(system_port port, uint8_t *p_period);
static uint32_t hdmirx_get_pc_format(hdmirx_video *p_video, uint8_t *p_vform,
										uint8_t *p_hv, uint8_t *p_curr_hv, system_video_format *p_vf);
static uint32_t hdmirx_get_unknown_format(hdmirx_video *p_video, uint8_t *p_vform,
										uint8_t *p_curr_hv, system_video_format *p_vf);
static uint32_t hdmirx_get_vformat0(hdmirx_video *p_video, uint8_t pckmon);
static uint32_t hdmirx_get_vformat1(hdmirx_video *p_video, uint8_t pckmon, uint8_t *p_vform,
									uint8_t *p_hv, uint8_t *p_curr_hv);
static uint32_t hdmirx_get_sd_hd_uhd(system_video_format vf, hdmirx_video_definition *p_vd);
static uint32_t hdmirx_set_avi_video_format(system_video *p_video, uint8_t vic,
										uint8_t hdmi_vs_detect, uint8_t *p_hdmi_vs_if);
static uint32_t hdmirx_set_avi_color_format(system_video *p_video, uint8_t y, uint8_t c,
										uint8_t ec, uint8_t q);
static uint32_t hdmirx_get_vformat2(system_video *p_video, uint8_t avi_detect, uint8_t *p_avi_if,
									uint8_t hdmi_vs_detect, uint8_t *p_hdmi_vs_if);
static uint32_t hdmirx_video_format_detect(hdmirx_video *p_video, uint8_t *p_irq);
static uint32_t hdmirx_select_page_of_0x14(system_port port, uint8_t page);
static uint32_t hdmirx_get_hdmi_mode(system_port port, uint8_t *p_mode);
static uint32_t hdmirx_get_packet_status0(system_port port, uint8_t *p_stat, uint8_t mask);
static uint32_t hdmirx_get_packet_status1(system_port port, uint8_t *p_stat, uint8_t mask);
static uint32_t hdmirx_get_packet_status2(system_port port, uint8_t *p_stat, uint8_t mask);
static uint32_t hdmirx_get_packet_param_change0(system_port port, uint8_t *p_irq, uint8_t mask);
static uint32_t hdmirx_get_packet_param_change1(system_port port, uint8_t *p_irq, uint8_t mask);
static uint32_t hdmirx_clr_packet_param_change0(system_port port, uint8_t irq);
static uint32_t hdmirx_clr_packet_param_change1(system_port port, uint8_t irq);
static uint32_t hdmirx_set_free_packet_header(system_port port, uint8_t h0);
static uint32_t hdmirx_get_avi_infoframe(system_port port, uint8_t *p_detect, uint8_t *p_if);
static uint32_t hdmirx_get_hdmi_vs_infoframe(system_port port, uint8_t *p_detect, uint8_t *p_if);
static uint32_t hdmirx_get_pixel_repetition(system_video_format vform, uint8_t *p_rep);
static uint32_t hdmirx_set_pixel_repetition(system_port port, uint8_t rep);
static uint32_t hdmirx_set_rxpll(system_port port, uint8_t tmds_freq, system_color_format cf, system_deep_color dc);
static uint32_t hdmirx_set_rxcore(system_port port, uint8_t tmds_freq, system_color_format cf, system_deep_color dc);
static uint32_t hdmirx_set_clock_param(system_video *p_video);
static uint32_t hdmirx_set_dfmt(system_port port, system_video_format vf);
static uint32_t hdmirx_output_video_data(system_video *p_video, uint8_t irq);
static uint32_t hdmirx_get_audio_status_mon(system_port port, uint8_t *p_astat);
static uint32_t hdmirx_is_oba(uint8_t astat);
static uint32_t hdmirx_set_sampling_freq(system_port port, uint8_t *p_astat);
static uint32_t hdmirx_set_sw_sampling_freq(system_port port, system_audio_sf sf);
static uint32_t hdmirx_get_audio_infoframe(system_port port, uint8_t *p_aif);
static uint32_t hdmirx_get_audio_cbit(system_port port, uint8_t *p_cbit);
static uint32_t hdmirx_get_aformat(hdmirx_audio *p_audio, uint8_t *p_astat);
static uint32_t hdmirx_output_audio_data(system_audio *p_audio);
static uint32_t hdmirx_get_spd_infoframe_data(hdmirx_spd_infoframe *p_spd);
static uint32_t hdmirx_get_hdr_infoframe_data(hdmirx_hdr_infoframe *p_hdr);
static uint32_t hdmirx_select_page_of_0x18(system_port port, uint8_t page);
static uint32_t hdmirx_init_scdcs(system_port port);
static uint32_t hdmirx_reset_all_irq(system_port port);
static uint32_t hdmirx_check_port_status(hdmirx_port_status *p_status, uint8_t irq_5v,
											uint8_t mon_5v, uint8_t *p_irq_tmds);
static uint32_t hdmirx_set_rx_soft_reset1(system_port port, uint8_t rst);
static uint32_t hdmirx_set_rx_soft_reset3(system_port port, uint8_t rst);
static uint32_t hdmirx_clear_unnecessary_irq(system_port port, uint8_t irq);
static uint32_t hdmirx_check_tmds_speed_mode(system_port port, hdmirx_tmds_speed_mode *p_sm);
static uint32_t hdmirx_re_calibrate_tmds(system_port port);
static uint32_t hdmirx_re_detect_deep_color(system_port port);
extern uint32_t hdmirx_hdcp_init(system_port port);
static uint32_t hdmirx_ctrl_req_abort(system_request *p_req, hdmirx_ctrl_req ctrl);
system_request hdmirx_ctrl_get_abort_req(void);
void hdmirx_ctrl_set_abort_req(system_request req);
//------------------------------------------------------------------------------
// abort request check
#define CHECK_ABORT() {												\
	if (hdmirx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		hdmirx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_HDMIRX_ABORT;									\
	}																\
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_switch_mode123(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_SWITCH_MODE123_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM2_SWMODE1_ADDR,
						HDMIRX_SET_SWITCH_MODE123_LEN, data);

	// operation mode of Tx port0: Repeat mode
	// rx port# to output t tx port0 (default setting): port.port_num
	SET_BLOCK_DATA(data, 0, HDMIRX_SET_SWITCH_MODE123_SWMODE1_REPEAT
								| port.port_num);							// 0x10A0
	// default
	SET_BLOCK_DATA(data, 1, HDMIRX_SET_SWITCH_MODE123_SWMODE2_DEF);			// 0x10A1
	// jitter remover PLL setting: ON
	SET_BLOCK_DATA(data, 2, HDMIRX_SET_SWITCH_MODE123_SWMODE3_RX0JRON_ON);	// 0x10A2

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_irq_main_status_mask(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_IRQ_MAIN_STATUS_MASK_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_MINTSTAMSK_00_ADDR,
						HDMIRX_SET_IRQ_MAIN_STATUS_MASK_LEN, data);

	// enable CCIRQST irq
	SET_BLOCK_DATA(data, 0, HDMIRX_MINTSTAMSK_00_CCIRQST_BIT);				// 0x1068
	// default
	SET_BLOCK_DATA(data, 1, HDMIRX_MINTSTAMSK_01_DEF);						// 0x1069

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_select_hdmirx(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SELECT_HDMIRX_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SELECT_HDMIRX_NUM);
	// select HDMI-Rx
	// 0x227B
	SET_COMMAND(cmd, 0, VIDEOIF_CC_MISC_ADDR,
						HDMIRX_MISC_VBOREN_HDMI_RX,
						HDMIRX_MISC_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_select_hdmi_mode(system_port port)
{
	uint32_t ret;

	// set Repeat mode, default selected port and jitter remover on
	ret = hdmirx_set_switch_mode123(port);
	CHECK_ERROR(ret);
	// set irq of main status mask
	ret = hdmirx_set_irq_main_status_mask(port);
	CHECK_ERROR(ret);
	// select HDMI-Rx
	ret = hdmirx_select_hdmirx(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_sysconf10(system_port port, uint8_t *p_conf)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_conf == NULL) {
		return ERR_NULL_POINTER;
	}

	// set System Configuration Register #10,11,12,13
	// 0x1018, 0x1019, 0x101A, 0x101B
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_SYSCFG10_ADDR,
						HDMIRX_SET_SYSCONF10_LEN, p_conf);
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_sysconf10_of_portno(system_port port, uint8_t no, uint8_t conf, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SYSCONF10_OF_PORTNO_NUM];
	uint32_t ret;

	// set System Configuration Register #10,11,12,13
	// 0x1018 or 0x1019 or 0x101A or 0x101B
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SYSCONF10_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG10_ADDR + (no * HDMIRX_SADDR_10_PORT_OFFSET),
						conf,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_irq0(system_port port, uint8_t *p_irq)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Interrupt Register #0, 1, 2, 3
	// 0x10F4, 0x10F5, 0x10F6, 0x10F7
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_IRQ_INTREG0_ADDR,
						HDMIRX_GET_IRQ0_LEN, p_irq);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_irq0_of_portno(system_port port, uint8_t no, uint8_t *p_irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_IRQ0_OF_PORTNO_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Interrupt Register #0, 1, 2, 3
	// 0x10F4 or 0x10F5 or 0x10F6 or 0x10F7
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_IRQ0_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG0_ADDR + (no * HDMIRX_SADDR_10_PORT_OFFSET),
						0,
						mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_irq0_of_portno(system_port port, uint8_t no, uint8_t irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_IRQ0_OF_PORTNO_NUM];
	uint32_t ret;

	if (irq == 0) {
		return ERR_OK;
	}

	// set Interrupt Register #0, 1, 2, 3
	// 0x10F4 or 0x10F5 or 0x10F6 or 0x10F7
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_IRQ0_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG0_ADDR + (no * HDMIRX_SADDR_10_PORT_OFFSET),
						irq,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_irq4_of_portno(system_port port, uint8_t no, uint8_t irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_IRQ4_OF_PORTNO_NUM];
	uint32_t ret;

	// set Interrupt Register #4, 5, 6, 7
	// 0x10F8 or 0x10F9 or 0x10FA or 0x10FB
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_IRQ4_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG4_ADDR + (no * HDMIRX_SADDR_10_PORT_OFFSET),
						irq,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmdsrx1_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_TMDSRX1_0_OF_PORTNO_NUM];
	uint32_t ret;

	// set TMDS RX Port0, 1, 2, 3 Register #1_0
	// 0x1200 or 0x1220 or 0x1240 or 0x1260
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_TMDSRX1_0_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSRXP0_1_00_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						tmds,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmdsrx1_0_2_of_portno(system_port port, uint8_t no, uint8_t tmds0, uint8_t mask0,
												uint8_t tmds1, uint8_t mask1, uint8_t tmds2, uint8_t mask2)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDSRX1_0_2_OF_PORTNO_LEN];
	uint32_t ret;

	// set TMDS RX Port0, 1, 2, 3 Register #1_0,1,2
	// 0x1200-0x1202 or 0x1220-0x1222 or 0x1240-0x1242 or 0x1260-0x1262
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_TMDS_TMDSRXP0_1_00_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						HDMIRX_SET_TMDSRX1_0_2_OF_PORTNO_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	data[0] = (data[0] & ~mask0) | (tmds0 & mask0);
	data[1] = (data[1] & ~mask1) | (tmds1 & mask1);
	data[2] = (data[2] & ~mask2) | (tmds2 & mask2);

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmdsrx2_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_TMDSRX2_0_OF_PORT_NUM];
	uint32_t ret;

	// set TMDS RX Port0, 1, 2, 3 Register #2_0
	// 0x1204 or 0x1224 or 0x1244 or 0x1264
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_TMDSRX2_0_OF_PORT_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSRXP0_2_00_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						tmds,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmdsrx4_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_TMDSRX4_0_OF_PORTNO_NUM];
	uint32_t ret;

	// set TMDS RX Port0, 1, 2, 3 Register #4_0
	// 0x1209 or 0x1229 or 0x1249 or 0x1269
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_TMDSRX4_0_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSRXP0_4_00_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						tmds,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmdsrx5_0_of_portno(system_port port, uint8_t no, uint8_t tmds, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_TMDSRX5_0_OF_PORTNO_NUM];
	uint32_t ret;

	// set TMDS Rx Port0, 1, 2, 3 Register #5_0
	// 0x120C or 0x122C or 0x124C or 0x126C
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_TMDSRX5_0_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_TMDSRXP0_5_00_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						tmds,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_tmdsrx5_1_3_of_portno(system_port port, uint8_t no, uint8_t *p_tmds)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_tmds == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS Rx Port0, 1, 2, 3 Register #5_1,2,3
	// 0x120D-0x120F or 0x122D-0x122F or 0x124D-0x124F or 0x126D-0x126F
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_TMDS_TMDSRXP0_5_01_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						HDMIRX_GET_TMDSRX5_1_3_OF_PORTNO_LEN, p_tmds);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmdsrx6_of_portno(system_port port, uint8_t no, const uint8_t *p_tmds)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_tmds == NULL) {
		return ERR_NULL_POINTER;
	}

	// set TMDS Rx Port0, 1, 2, 3 Register #6 (16bytes)
	// 0x1210-121F, 0x1230-0x123F, 0x1250-0x125F, 0x1270-0x127F
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_TMDS_TMDSRXP0_6_00_ADDR + (no * HDMIRX_SADDR_12_PORT_OFFSET),
						HDMIRX_SET_TMDSRX6_OF_PORTNO_LEN, (uint8_t *)p_tmds);
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_auth_phase_of_portno(system_port port, uint8_t no, uint8_t *p_auphs,
												uint8_t *p_tcksel, uint8_t *p_pssyen)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_AUTH_PHASE_OF_PORTNO_NUM];
	uint32_t ret;

	if (p_auphs == NULL || p_tcksel == NULL || p_pssyen == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_AUTH_PHASE_OF_PORTNO_NUM);
	// 0x1606: AUPHSx 0, 1, 2, 3 (bit0, 1, 2, 3)
	SET_COMMAND(cmd, 0, VIDEOIF_AKERX_FPSWMON_01_ADDR,
						0,
						(HDMIRX_FPSWMON_01_AUPHS0_BIT << no));
	// 0x1295: PxTCKSEL 0, 1, 2, 3 (bit4, 5, 6, 7)
	SET_COMMAND(cmd, 1, VIDEOIF_CKDET_CKDTPRM2_ADDR,
						0,
						(HDMIRX_CKDTPRM2_P0TCKSEL_BIT << no));
	// 0x1600: PSSYEN[3:0] (bit3-0)
	SET_COMMAND(cmd, 2, VIDEOIF_AKERX_FPSWCTRL_00_ADDR,
						0,
						HDMIRX_FPSWCTRL_00_PSSYEN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_auphs = cmd[0].data;
	*p_tcksel = cmd[1].data;
	*p_pssyen = cmd[2].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_switch_mode1(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SWITCH_MODE1_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SWITCH_MODE1_NUM);
	// 0x10A0
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE1_ADDR,
						HDMIRX_SWMODE1_PORT1MODE_NOR				// port1 mode: repeat mode
							| HDMIRX_SWMODE1_PORT1OUT_DEF			// output to tx port1: rx port1
							| HDMIRX_SWMODE1_PORT0MODE_REPEAT_MODE	// port0 mode: repeat mode
							| port.port_num,						// output to tx port0: rx port#
						HDMIRX_SWMODE1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_unselected_ddc_tmds_of_portno(system_port port, uint8_t no)
{
	uint32_t ret;

	// Set DDC NACK Mode of Non-select Port
	ret = hdmirx_set_sysconf10_of_portno(port, no,
										HDMIRX_SYSCFG1X_DDCNCSX_NACK,
										HDMIRX_SYSCFG1X_DDCNCSX_MASK);
	CHECK_ERROR(ret);
	// Set Terminate Resistor of Non-select Port OFF
	ret = hdmirx_set_tmdsrx1_0_of_portno(port, no,
										HDMIRX_TMDSRXPX1_0_TERMCKEN0_DIS,
										HDMIRX_TMDSRXPX1_0_TERMCKEN0_MASK);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_tmds_clock_freq_of_portno(system_port port, uint8_t no, uint8_t *p_freq)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_GET_TMDS_CLOCK_FREQ_OF_PORTNO_LEN];
	uint32_t ret;

	if (p_freq == NULL) {
		return ERR_NULL_POINTER;
	}
	// get TMDS clock frequency (Rx Port0,1,2,3)
	// P*TCLKFQ[3:0]: 0000:  not detected
	//                0001:    0   -  20   MHz
	//                0010:   20   -  45   MHz
	//                0011:   45   -  85.5 MHz
	//                0100:   85.5 - 123   MHz
	//                0101:  123   - 175   MHz
	//                0110:  175   - 250   MHz
	//                0111:  250   - 342   MHz
	//                1000:  342   - 492   MHz
	//                other: 492   -       MHz
	// 0x1290 bit7-4: port1, bit3-0: port0
	// 0x1291 bit7-4: port3, bit3-0: port2
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDET11_00_ADDR,
						HDMIRX_GET_TMDS_CLOCK_FREQ_OF_PORTNO_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	switch(no) {
		case SYSTEM_PORT_NUMBER_0:
			*p_freq = data[0] & HDMIRX_PXTCLKFQ_MASK;
			break;
		case SYSTEM_PORT_NUMBER_1:
			*p_freq = (data[0] >> HDMIRX_PXTCLKFQ_P1TCLKFQ_BIT)
						& HDMIRX_PXTCLKFQ_MASK;
			break;
		case SYSTEM_PORT_NUMBER_2:
			*p_freq = data[1] & HDMIRX_PXTCLKFQ_MASK;
			break;
		case SYSTEM_PORT_NUMBER_3:
			*p_freq = (data[1] >> HDMIRX_PXTCLKFQ_P3TCLKFQ_BIT)
						& HDMIRX_PXTCLKFQ_MASK;
			break;
		default:
			return ERR_NG;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_init_ddc_ctrl(system_port port)
{
	uint8_t ddc[HDMIRX_SET_SYSCONF10_LEN];
	uint32_t ret;

	ddc[0] = HDMIRX_SYSCFG1X_NPRIMEX_0X74		// Negative Primary Enable: 0x74 (primary)
			| HDMIRX_SYSCFG1X_DDCFLTX_NOR		// DDC Interface Sampling Filter Control: normal ope
			| HDMIRX_SYSCFG1X_DDCNCSX_ACK		// DDC I/F Acknowledge Control: normal ope
			| HDMIRX_SYSCFG1X_HDDCNCSX_NOR		// HDCP port of DDC I/F Acknowledge Control: normal ope
			| HDMIRX_SYSCFG1X_SYSOPMX_EXTERNAL;	// System Operation Mode: use external EDID
	ddc[1] = ddc[0];
	ddc[2] = ddc[0];
	ddc[3] = ddc[0];

	ret = hdmirx_set_sysconf10(port, ddc);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scdcs_mode(system_port port, uint8_t scdsc)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SCDCS_MODE_NUM];
	uint32_t ret;

	// set monitor register #1, 2, 3, 4 (0x1CEC, 0x1CF0, 0x1CF4, 0x1CF8)
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SCDCS_MODE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_MONITOR_MONREG1_00_ADDR, scdsc, HDMIRX_MONREGX_0_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_MONITOR_MONREG2_00_ADDR, scdsc, HDMIRX_MONREGX_0_MASK);
	SET_COMMAND(cmd, 2, VIDEOIF_MONITOR_MONREG3_00_ADDR, scdsc, HDMIRX_MONREGX_0_MASK);
	SET_COMMAND(cmd, 3, VIDEOIF_MONITOR_MONREG4_00_ADDR, scdsc, HDMIRX_MONREGX_0_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_scramble_mon_of_portno(system_port port, uint8_t no, uint8_t *p_scrmon)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_SCRAMBLE_MON_OF_PORTNO_NUM];
	uint32_t ret;

	if (p_scrmon == NULL) {
		return ERR_NULL_POINTER;
	}

	// get scramble monitor #1, 2, 3, 4
	// 0x1CEF or 0x1CF3 or 0x1CF7 or 0x1CFB
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_SCRAMBLE_MON_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_MONITOR_MONREG1_03_ADDR + (no * HDMIRX_SADDR_1C_PORT_OFFSET),
						0,
						HDMIRX_MONREGX_SCRMONX_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_scrmon = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_maxtcc(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_MAXTCC_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDTPRM4_00_ADDR,
						HDMIRX_SET_TMDS_MAXTCC_LEN, data);
	SET_BLOCK_DATA(data, 0, ((HDMIRX_MAXTCC >> 0) & HDMIRX_MASK_FF));	// 0x1298
	SET_BLOCK_DATA(data, 1, ((HDMIRX_MAXTCC >> 8) & HDMIRX_MASK_FF));	// 0x1299
	SET_BLOCK_DATA(data, 2, ((HDMIRX_MAXTCC >> 16) & HDMIRX_MASK_FF));	// 0x129A

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_fchgth(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_FCHGTH_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDTPRM6_00_ADDR,
						HDMIRX_SET_TMDS_FCHGTH_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_CKDTPRM6_0_DEF);						// 0x129C
	SET_BLOCK_DATA(data, 1, HDMIRX_CKDTPRM6_1_DEF);						// 0x129D
	SET_BLOCK_DATA(data, 2, HDMIRX_CKDTPRM6_2_DEF);						// 0x129E

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_fdttha_h(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_FDTTHA_H_LEN];
	uint32_t val;
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDTPRM7_00_ADDR,
						HDMIRX_SET_TMDS_FDTTHA_H_LEN, data);
	// FDTTHA: 0x00010000 * 27MHz / 20MHz
	val = HDMIRX_CALC_FDTTHA();
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x12A0
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x12A1
	SET_BLOCK_DATA(data, 2, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12A2
	SET_BLOCK_DATA(data, 3, 0);											// 0x12A3
	// FDTTHB: 0x00010000 * 27MHz / 45MHz
	val = HDMIRX_CALC_FDTTHB();
	SET_BLOCK_DATA(data, 4, ((val >> 0) & HDMIRX_MASK_FF));				// 0x12A4
	SET_BLOCK_DATA(data, 5, ((val >> 8) & HDMIRX_MASK_FF));				// 0x12A5
	SET_BLOCK_DATA(data, 6, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12A6
	SET_BLOCK_DATA(data, 7, 0);											// 0x12A7
	// FDTTHC: 0x00010000 * 27MHz / 89MHz
	val = HDMIRX_CALC_FDTTHC();
	SET_BLOCK_DATA(data, 8, ((val >> 0) & HDMIRX_MASK_FF));				// 0x12A8
	SET_BLOCK_DATA(data, 9, ((val >> 8) & HDMIRX_MASK_FF));				// 0x12A9
	SET_BLOCK_DATA(data, 10, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12AA
	SET_BLOCK_DATA(data, 11, 0);										// 0x12AB
	// FDTTHD: 0x00010000 * 27MHz / 123MHz
	val = HDMIRX_CALC_FDTTHD();
	SET_BLOCK_DATA(data, 12, ((val >> 0) & HDMIRX_MASK_FF));			// 0x12AC
	SET_BLOCK_DATA(data, 13, ((val >> 8) & HDMIRX_MASK_FF));			// 0x12AD
	SET_BLOCK_DATA(data, 14, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12AE
	SET_BLOCK_DATA(data, 15, 0);										// 0x12AF
	// FDTTHE: 0x00010000 * 27MHz / 175MHz
	val = HDMIRX_CALC_FDTTHE();
	SET_BLOCK_DATA(data, 16, ((val >> 0) & HDMIRX_MASK_FF));			// 0x12B0
	SET_BLOCK_DATA(data, 17, ((val >> 8) & HDMIRX_MASK_FF));			// 0x12B1
	SET_BLOCK_DATA(data, 18, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12B2
	SET_BLOCK_DATA(data, 19, 0);										// 0x12B3
	// FDTTHF: 0x00010000 * 27MHz / 250MHz
	val = HDMIRX_CALC_FDTTHF();
	SET_BLOCK_DATA(data, 20, ((val >> 0) & HDMIRX_MASK_FF));			// 0x12B4
	SET_BLOCK_DATA(data, 21, ((val >> 8) & HDMIRX_MASK_FF));			// 0x12B5
	SET_BLOCK_DATA(data, 22, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12B6
	SET_BLOCK_DATA(data, 23, 0);										// 0x12B7
	// FDTTHG: 0x00010000 * 27MHz / 342MHz
	val = HDMIRX_CALC_FDTTHG();
	SET_BLOCK_DATA(data, 24, ((val >> 0) & HDMIRX_MASK_FF));			// 0x12B8
	SET_BLOCK_DATA(data, 25, ((val >> 8) & HDMIRX_MASK_FF));			// 0x12B9
	SET_BLOCK_DATA(data, 26, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12BA
	SET_BLOCK_DATA(data, 27, 0);										// 0x12BB
	// FDTTHH: 0x00010000 * 27MHz / 492MHz
	val = HDMIRX_CALC_FDTTHH();
	SET_BLOCK_DATA(data, 28, ((val >> 0) & HDMIRX_MASK_FF));			// 0x12BC
	SET_BLOCK_DATA(data, 29, ((val >> 8) & HDMIRX_MASK_FF));			// 0x12BD
	SET_BLOCK_DATA(data, 30, ((val >> 16) & HDMIRX_MASK_FF));			// 0x12BE

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_maxhcc(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_MAXHCC_LEN];
	uint32_t val;
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDTPRM9_00_ADDR,
						HDMIRX_SET_TMDS_MAXHCC_LEN, data);
	// when 27MHz: (27*10^6)/(1/1*10^-3) = 27000 = 0x6978
	val = HDMIRX_CALC_MAXHCC((uint32_t)1 * (uint32_t)1000);				// detect cycle : 1ms
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x12C4
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x12C5

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_tckthrb(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_TCKTHRB_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDTPRM10_00_ADDR,
						HDMIRX_SET_TMDS_TCKTHRB_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_CKDTPRM10_0_DEF);					// 0x12CC
	SET_BLOCK_DATA(data, 1, HDMIRX_CKDTPRM10_1_DEF);					// 0x12CD
	SET_BLOCK_DATA(data, 2, HDMIRX_CKDTPRM10_2_DEF);					// 0x12CE

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_tmds_freq_detect_param(system_port port)
{
	uint32_t ret;

	// Set MAX TMDS Clock Counter
	ret = hdmirx_set_tmds_maxtcc(port);
	CHECK_ERROR(ret);
	// Set TMDS Clock Frequency Change
	ret = hdmirx_set_tmds_fchgth(port);
	CHECK_ERROR(ret);
	// Set TMDS Clock Frequency Detect
	ret = hdmirx_set_tmds_fdttha_h(port);
	CHECK_ERROR(ret);
	// Set Max Host Clock Counter
	ret = hdmirx_set_tmds_maxhcc(port);
	CHECK_ERROR(ret);
	// Set Bottom Threshold to detect TMDS Clock
	ret = hdmirx_set_tmds_tckthrb(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_auto_reset_mode(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_AUTO_RESET_MODE_NUM];
	uint8_t val;
	uint32_t ret;

	val = HDMIRX_SYSCFGX_PXRSEQEN_AUTO_EXEC
			| HDMIRX_SYSCFGX_PXLKDTEN_GEN_IRQ
			| HDMIRX_SYSCFGX_PXSRSTEB_AUTO_CLR
			| HDMIRX_SYSCFGX_PXSRSTEA_AUTO_CLR;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_AUTO_RESET_MODE_NUM);
	// 0x1010
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG2_ADDR, val, HDMIRX_SET_AUTO_RESET_MODE_MASK);
	// 0x1011
	SET_COMMAND(cmd, 1, VIDEOIF_SYSTEM_SYSCFG3_ADDR, val, HDMIRX_SET_AUTO_RESET_MODE_MASK);
	// 0x1012
	SET_COMMAND(cmd, 2, VIDEOIF_SYSTEM_SYSCFG4_ADDR, val, HDMIRX_SET_AUTO_RESET_MODE_MASK);
	// 0x1013
	SET_COMMAND(cmd, 3, VIDEOIF_SYSTEM_SYSCFG5_ADDR, val, HDMIRX_SET_AUTO_RESET_MODE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_reset_lock_time(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_RESET_LOCK_TIME_LEN];
	uint32_t val;
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_RSEQTM1_00_ADDR,
						HDMIRX_SET_TMDS_RESET_LOCK_TIME_LEN, data);
	// TMRTMA: 100us * 27MHz
	val = HDMIRX_CALC_TMRTM(100);
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x1024
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x1025
	SET_BLOCK_DATA(data, 2, ((val >> 16) & HDMIRX_MASK_FF));			// 0x1026
	SET_BLOCK_DATA(data, 3, 0);											// 0x1027
	// TMRTMB: 100us * 27MHz
	val = HDMIRX_CALC_TMRTM(100);
	SET_BLOCK_DATA(data, 4, ((val >> 0) & HDMIRX_MASK_FF));				// 0x1028
	SET_BLOCK_DATA(data, 5, ((val >> 8) & HDMIRX_MASK_FF));				// 0x1029
	SET_BLOCK_DATA(data, 6, ((val >> 16) & HDMIRX_MASK_FF));			// 0x102A
	SET_BLOCK_DATA(data, 7, 0);											// 0x102B
	// TMRTMC: 10000us * 27MHz
	val = HDMIRX_CALC_TMRTM(10000);
	SET_BLOCK_DATA(data, 8, ((val >> 0) & HDMIRX_MASK_FF));				// 0x102C
	SET_BLOCK_DATA(data, 9, ((val >> 8) & HDMIRX_MASK_FF));				// 0x102D
	SET_BLOCK_DATA(data, 10, ((val >> 16) & HDMIRX_MASK_FF));			// 0x102E
	SET_BLOCK_DATA(data, 11, 0);										// 0x102F
	// LKDTTM: 10us * 27MHz
	val = HDMIRX_CALC_TMRTM(10);
	SET_BLOCK_DATA(data, 12, ((val >> 0) & HDMIRX_MASK_FF));			// 0x1030
	SET_BLOCK_DATA(data, 13, ((val >> 8) & HDMIRX_MASK_FF));			// 0x1031
	SET_BLOCK_DATA(data, 14, ((val >> 16) & HDMIRX_MASK_FF));			// 0x1032

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_fast_port_switch(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_FAST_PORT_SWITCH_LEN];
	uint32_t val;
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AKERX_FPSWRTM_00_ADDR,
						HDMIRX_SET_FAST_PORT_SWITCH_LEN, data);
	// PSELTM: 200000us * 27MHz / 256
	val = HDMIRX_CALC_PSELTM(200000);
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x1608
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x1609
	// PAVRTM: 250000us * 27MHz / 256
	val = HDMIRX_CALC_PAVRTM(250000);
	SET_BLOCK_DATA(data, 2, ((val >> 0) & HDMIRX_MASK_FF));				// 0x160A
	SET_BLOCK_DATA(data, 3, ((val >> 8) & HDMIRX_MASK_FF));				// 0x160B

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_tmds_reset_seq_param(system_port port)
{
	uint32_t ret;

	// Set Auto Reset Mode
	ret = hdmirx_set_auto_reset_mode(port);
	CHECK_ERROR(ret);
	// Set TMDS Reset Time
	// Set Lock Detect Time
	ret = hdmirx_set_tmds_reset_lock_time(port);
	CHECK_ERROR(ret);
	// Set Reset Time for Fast Port Switch
	ret = hdmirx_set_fast_port_switch(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_auto_scramble_detect_mode(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_AUTO_SCRAMBLE_DETECT_MODE_LEN];
	uint8_t val;
	uint32_t ret;

	val = HDMIRX_SYSCFGX_SYFILMX_DEF
			| HDMIRX_SYSCFGX_ASCRENX_AUTO_DETECT
			| HDMIRX_SYSCFGX_SYFILTX_DEF
			| HDMIRX_SYSCFGX_SKWDISX_DEF
			| HDMIRX_SYSCFGX_RSYAENX_DEF
			| HDMIRX_SYSCFGX_RSYDAUX_DEF
			| HDMIRX_SYSCFGX_RSYDISX_DEF;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_SYSCFG6_ADDR,
						HDMIRX_SET_AUTO_SCRAMBLE_DETECT_MODE_LEN, data);
	SET_BLOCK_DATA(data, 0, val);										// 0x1014
	SET_BLOCK_DATA(data, 1, val);										// 0x1015
	SET_BLOCK_DATA(data, 2, val);										// 0x1016
	SET_BLOCK_DATA(data, 3, val);										// 0x1017

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scramble_seed(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_SCRAMBLE_SEED_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_MONITOR_SCRINIT_00_ADDR,
						HDMIRX_SET_SCRAMBLE_SEED_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_SCRINIT0_SCRINIA_INIT);				// 0x1C90
	SET_BLOCK_DATA(data, 1, HDMIRX_SCRINIT1_SCRINIA_INIT);				// 0x1C91
	SET_BLOCK_DATA(data, 2, HDMIRX_SCRINIT2_SCRINIB_INIT);				// 0x1C92
	SET_BLOCK_DATA(data, 3, HDMIRX_SCRINIT3_SCRINIB_INIT);				// 0x1C93
	SET_BLOCK_DATA(data, 4, HDMIRX_SCRINIT4_SCRINIC_INIT);				// 0x1C94
	SET_BLOCK_DATA(data, 5, HDMIRX_SCRINIT5_SCRINIC_INIT);				// 0x1C95

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scramble_mode(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SCRAMBLE_MODE_NUM];
	uint32_t ret;

	// set debug register #0, 1, 2, 3 (0x1CD1, 0x1CD5, 0x1CD9, 0x1CDD)
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SCRAMBLE_MODE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_MONITOR_DBGREG0_01_ADDR,
						HDMIRX_DBGREGX_SCRMODX_ON,
						HDMIRX_DBGREGX_MASK);
	SET_COMMAND(cmd, 1, VIDEOIF_MONITOR_DBGREG1_01_ADDR,
						HDMIRX_DBGREGX_SCRMODX_ON,
						HDMIRX_DBGREGX_MASK);
	SET_COMMAND(cmd, 2, VIDEOIF_MONITOR_DBGREG2_01_ADDR,
						HDMIRX_DBGREGX_SCRMODX_ON,
						HDMIRX_DBGREGX_MASK);
	SET_COMMAND(cmd, 3, VIDEOIF_MONITOR_DBGREG3_01_ADDR,
						HDMIRX_DBGREGX_SCRMODX_ON,
						HDMIRX_DBGREGX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_tmds_scramble_detect_mode(system_port port)
{
	uint32_t ret;

	// Set Auto Scramble Detect Mode
	ret = hdmirx_set_auto_scramble_detect_mode(port);
	CHECK_ERROR(ret);
	// set scramble seed
	ret = hdmirx_set_scramble_seed(port);
	CHECK_ERROR(ret);
	// Set Scramble Mode
	ret = hdmirx_set_scramble_mode(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_lump_mode(system_port port, uint8_t lump)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_LUMP_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_LUMP_MODE_NUM);
	// set TMDS Rx lump mode:
	//   when 1, copy 0x1200-0x121F (port0) to 0x1220-0x123F (port1)
	//                                         0x1240-0x125F (port2)
	//                                         0x1260-0x127F (port3)
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG15_ADDR,
						lump,
						HDMIRX_SYSCFG15_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_auto_speed_mode(system_port port, uint8_t mode)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_TMDS_AUTO_SPEED_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_TMDS_AUTO_SPEED_MODE_NUM);
	// set 1/4 mode of TMDS clock selection
	// 0x129A
	SET_COMMAND(cmd, 0, VIDEOIF_CKDET_CKDTPRM4_02_ADDR,
						mode,
						HDMIRX_CKDTPRM4_02_AT4DIVDS_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_clock_freq_detect_param2(system_port port, uint8_t mode, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_CLOCK_FREQ_DETECT_PARAM2_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_CLOCK_FREQ_DETECT_PARAM2_NUM);
	// 0x1295
	SET_COMMAND(cmd, 0, VIDEOIF_CKDET_CKDTPRM2_ADDR,
						mode,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_rx_param(system_port port, uint8_t mode)
{
	uint32_t ret;

	// set 0x12:0x0C = 0x0F
	ret = hdmirx_set_tmdsrx5_0_of_portno(port, SYSTEM_PORT_NUMBER_0,
										HDMIRX_SET_TMDS_RX_PARAM_0,
										HDMIRX_TMDSRXPX5_0_MASK);
	CHECK_ERROR(ret);
	// set 0x12:0x10-0x1F, when 0x12:0x0C = 0x0F
	ret = hdmirx_set_tmdsrx6_of_portno(port, SYSTEM_PORT_NUMBER_0,
										hdmirx_tmdsrx_param[HDMIRX_TMDSRXPX6_PARAM_KIND_0]);
	CHECK_ERROR(ret);
	// set 0x12:0x0C = 0x8F
	ret = hdmirx_set_tmdsrx5_0_of_portno(port, SYSTEM_PORT_NUMBER_0,
										HDMIRX_SET_TMDS_RX_PARAM_1,
										HDMIRX_TMDSRXPX5_0_MASK);
	CHECK_ERROR(ret);
	// set 0x12:0x10-0x1F, when 0x12:0x0C = 0x8F
	ret = hdmirx_set_tmdsrx6_of_portno(port, SYSTEM_PORT_NUMBER_0,
										hdmirx_tmdsrx_param[HDMIRX_TMDSRXPX6_PARAM_KIND_1]);
	CHECK_ERROR(ret);
	// set 0x12:0x95 bit7-4 PXTCKSEL = 1 or 0
	// set 0x12:0x00 bit5 TERMCKENX = 1 or 0
	if (mode == HDMIRX_CKDTPRM2_PXTCKSEL_TMDS) {
		// unnecessary: set 0x12:0x95 bit7-4 PXTCKSEL = 1
		// set 0x12:0x00 bit5 TERMCKENX = 1
		ret = hdmirx_set_tmdsrx1_0_of_portno(port, SYSTEM_PORT_NUMBER_0,
											HDMIRX_TMDSRXPX1_0_TERMCKEN0_ENA,
											HDMIRX_TMDSRXPX1_0_TERMCKEN0_MASK);
		CHECK_ERROR(ret);
	} else if (mode == HDMIRX_CKDTPRM2_PXTCKSEL_CRU) {
		// unnecessary: set 0x12:0x95 bit7-4 PXTCKSEL = 0
		// set 0x12:0x00 bit5 TERMCKENX = 0
		ret = hdmirx_set_tmdsrx1_0_of_portno(port, SYSTEM_PORT_NUMBER_0,
											HDMIRX_TMDSRXPX1_0_TERMCKEN0_DIS,
											HDMIRX_TMDSRXPX1_0_TERMCKEN0_MASK);
		CHECK_ERROR(ret);
	} else {
		return ERR_NG;
	}
	// set 0x12:0x0C = 0x0F
	ret = hdmirx_set_tmdsrx5_0_of_portno(port, SYSTEM_PORT_NUMBER_0,
										HDMIRX_SET_TMDS_RX_PARAM_0,
										HDMIRX_TMDSRXPX5_0_MASK);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_param(system_port port)
{
	uint32_t ret;

	// Set TMDS-Rx Lump Setting Enable
	ret = hdmirx_set_lump_mode(port, HDMIRX_SYSCFG15_TMRLUMP_ENABLE);
	CHECK_ERROR(ret);
	// Set TMDS Speed Mode auto-set disable
	ret = hdmirx_set_tmds_auto_speed_mode(port, HDMIRX_CKDTPRM4_02_AT4DIVDS_MANUAL);
	CHECK_ERROR(ret);
	// Set TMDS Legacy Speed Mode: only 4 division mode of TMDS clock
	ret = hdmirx_set_clock_freq_detect_param2(port, HDMIRX_CKDTPRM2_PXTCLK4DIV_NOR,
													HDMIRX_CKDTPRM2_PXTCLK4DIV_MASK
													| HDMIRX_CKDTPRM2_PXTCKSEL_MASK);
	CHECK_ERROR(ret);
	// Set TMDS Receiver Parameter 1
	ret = hdmirx_set_tmds_rx_param(port, HDMIRX_CKDTPRM2_PXTCKSEL_TMDS);
	CHECK_ERROR(ret);
	// Set TMDS High Speed Mode: only 4 division mode of TMDS clock
	ret = hdmirx_set_clock_freq_detect_param2(port, HDMIRX_CKDTPRM2_PXTCLK4DIV_4DIV,
													HDMIRX_CKDTPRM2_PXTCLK4DIV_MASK
													| HDMIRX_CKDTPRM2_PXTCKSEL_MASK);
	CHECK_ERROR(ret);
	// Set TMDS Receiver Parameter 2
	ret = hdmirx_set_tmds_rx_param(port, HDMIRX_CKDTPRM2_PXTCKSEL_CRU);
	CHECK_ERROR(ret);
	// Set TMDS Speed Mode auto-set enable
	ret = hdmirx_set_tmds_auto_speed_mode(port, HDMIRX_CKDTPRM4_02_AT4DIVDS_AUTO);
	CHECK_ERROR(ret);
	// Set TMDS-Rx Lump Setting Disable
	ret = hdmirx_set_lump_mode(port, HDMIRX_SYSCFG15_TMRLUMP_DISABLE);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_fast_port_switch_mode(system_port port, hdmirx_fast_port fast)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_FAST_PORT_SW_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_FAST_PORT_SW_MODE_NUM);
	// 0x1600
	SET_COMMAND(cmd, 0, VIDEOIF_AKERX_FPSWCTRL_00_ADDR,
						(fast == HDMIRX_FAST_PORT_ENABLE)
							? (HDMIRX_FPSWCTRL_00_PSYOENX_OUTPUT
								| HDMIRX_FPSWCTRL_00_PSSYEN_FAST_ENA)
							: (HDMIRX_FPSWCTRL_00_PSYOENX_NOT_OUTPUT
								| HDMIRX_FPSWCTRL_00_PSSYEN_FAST_DIS),
						HDMIRX_FPSWCTRL_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_mask_port(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_INTR_MASK_PORT_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_INTMSK_00_ADDR,
						HDMIRX_SET_INTR_MASK_PORT_LEN, data);

	// FRQCHG, LFQDET, HDMCHG, LOCKDT (Port 0,1,2,3)
	SET_BLOCK_DATA(data, 0, HDMIRX_INTMSK_0_3_0F);				// 0x1070
	SET_BLOCK_DATA(data, 1, HDMIRX_INTMSK_0_3_0F);				// 0x1071
	SET_BLOCK_DATA(data, 2, HDMIRX_INTMSK_0_3_0F);				// 0x1072
	SET_BLOCK_DATA(data, 3, HDMIRX_INTMSK_0_3_0F);				// 0x1073
	// AKEIRQ, AKREDY (Port 0,1,2,3)
	SET_BLOCK_DATA(data, 4, HDMIRX_INTMSK_4_7_E1);				// 0x1074
	SET_BLOCK_DATA(data, 5, HDMIRX_INTMSK_4_7_E1);				// 0x1075
	SET_BLOCK_DATA(data, 6, HDMIRX_INTMSK_4_7_E1);				// 0x1076
	SET_BLOCK_DATA(data, 7, HDMIRX_INTMSK_4_7_E1);				// 0x1077
	// P3V5STCHG, P2V5STCHG, P1V5STCHG, P0V5STCHG
	SET_BLOCK_DATA(data, 8, HDMIRX_INTMSK_8_F0);				// 0x1078

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_status_mask_port(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_INTR_STATUS_MASK_PORT_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_INTSTAMSK_00_ADDR,
						HDMIRX_SET_INTR_STATUS_MASK_PORT_LEN, data);

	SET_BLOCK_DATA(data, 0, HDMIRX_INTSTAMSK_0_FF);				// 0x106C
	SET_BLOCK_DATA(data, 1, HDMIRX_INTSTAMSK_0_01);				// 0x106D

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_tmds_receiver_init(system_port port)
{
	uint32_t ret;

	// TMDS Receiver Power ON
	//   already set by system_ctrl_init()
	// Set EDID Operating Mode and EDID
	//   see hdmirx_write_edid()
	// Start DDC Slave Operation
	ret = hdmirx_init_ddc_ctrl(port);
	CHECK_ERROR(ret);
	// Set SCDCS Mode Disable, when using external SCDCS registers
	ret = hdmirx_set_scdcs_mode(port, HDMIRX_MONREGX_0_SCDCNCSX_ACK);
	CHECK_ERROR(ret);
	// Initialize HDCP
	//   see hdmirx_hdcp_authentication()
	// Set TMDS Frequency Detect Parameter
	ret = hdmirx_tmds_freq_detect_param(port);
	CHECK_ERROR(ret);
	// Set TMDS Reset Sequence Parameter
	ret = hdmirx_tmds_reset_seq_param(port);
	CHECK_ERROR(ret);
	// Set TMDS Scramble Detect Mode
	ret = hdmirx_tmds_scramble_detect_mode(port);
	CHECK_ERROR(ret);
	// Set TMDS Parameter
	ret = hdmirx_set_tmds_param(port);
	CHECK_ERROR(ret);
	// Set Fast Port Switch Mode: disable
	ret = hdmirx_set_fast_port_switch_mode(port, HDMIRX_FAST_PORT_DISABLE);
	CHECK_ERROR(ret);
	// Set Interrupt Mask
	ret = hdmirx_set_intr_mask_port(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_intr_status_mask_port(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_select_page_of_0x60c4(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SELECT_PAGE_OF_0X60C4_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SELECT_PAGE_OF_0X60C4_NUM);
	// 0x20C2
	SET_COMMAND(cmd, 0, VIDEOIF_CKDET_RCKFQDET3_00_ADDR,
						page,
						HDMIRX_RCKFQDET3_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_link_freq_detect_param(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_TMDS_MAXHCC_LEN];
	uint32_t val;
	uint32_t ret;

	// page(0x60c4): 1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_RCFDPRM11_00_ADDR,
						HDMIRX_SET_TMDS_MAXHCC_LEN, data);
	// when 27MHz: (27*10^6)/(1/1*10^-3) = 27000 = 0x6978
	val = HDMIRX_CALC_MAXHCC((uint32_t)1 * (uint32_t)1000);				// detect cycle : 1ms
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x20C8
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x20C9

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_reset_seq_time1(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_RESET_SEQ_TIME1_LEN];
	uint32_t val;
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_RRSEQTM1_00_ADDR,
						HDMIRX_SET_RESET_SEQ_TIME1_LEN, data);
	// DPCRTM: Reset Timming when Deep Color: 100ms
	// when 27MHz: 100 * 27 / 256 = 10546 = 0x2933
	val = HDMIRX_CALC_DPCRTM((uint32_t)100 * (uint32_t)1000);
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x2022
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x2023

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_reset_seq_time2(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_RESET_SEQ_TIME2_LEN];
	uint32_t val;
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_RRSEQTM2_00_ADDR,
						HDMIRX_SET_RESET_SEQ_TIME2_LEN, data);
	// VAIRTM: Video/Audio I/F Reset Time: 300ms
	// when 27MHz: 300 * 27 = 8100000 = 0x7B98A0
	val = HDMIRX_CALC_VAIRTM((uint32_t)300 * (uint32_t)1000);
	SET_BLOCK_DATA(data, 0, ((val >> 0) & HDMIRX_MASK_FF));				// 0x203C
	SET_BLOCK_DATA(data, 1, ((val >> 8) & HDMIRX_MASK_FF));				// 0x203D
	SET_BLOCK_DATA(data, 2, ((val >> 16) & HDMIRX_MASK_FF));			// 0x203E

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_clock_freq_detect_param(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_CLOCK_FREQ_DETECT_PARAM_LEN];
	uint32_t val;
	uint32_t ret;

	// page(0x60c4): 1

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_RCFDPRM13_00_ADDR,
						HDMIRX_SET_CLOCK_FREQ_DETECT_PARAM_LEN, data);
	// HPRRT1: 20us
	// when 27MHz: 20 / (32 / 27) = 17 = 0x11
	val = HDMIRX_CALC_HPRRT1(20);
	SET_BLOCK_DATA(data, 0, (val & HDMIRX_MASK_FF));					// 0x20EC
	// HPRRT0: 0.5us = 500ns
	// when 27MHz: 0.5 / (16 / 17) -1 = 0 = 0x00
	val = HDMIRX_CALC_HPRRT0(500);
	SET_BLOCK_DATA(data, 1, (val & HDMIRX_MASK_03));					// 0x20ED

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_link_reset_seq_param(system_port port)
{
	uint32_t ret;

	// Set Receiver Link Reset Time
	ret = hdmirx_set_reset_seq_time1(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_reset_seq_time2(port);
	CHECK_ERROR(ret);
	// page(0x60c4): 1
	// Set Audio PLL Reset Time
	ret = hdmirx_set_clock_freq_detect_param(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_sptvfm(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_SPTVFM_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_SPTVFM_00_ADDR,
						HDMIRX_SET_SPTVFM_LEN, data);
	SET_BLOCK_DATA(data, 0, ((HDMIRX_SPTVFM_ALL0 >> 0) & HDMIRX_MASK_FF));		// 0x2220
	SET_BLOCK_DATA(data, 1, ((HDMIRX_SPTVFM_ALL0 >> 8) & HDMIRX_MASK_FF));		// 0x2221
	SET_BLOCK_DATA(data, 2, ((HDMIRX_SPTVFM_ALL0 >> 16) & HDMIRX_MASK_FF));		// 0x2222
	SET_BLOCK_DATA(data, 3, ((HDMIRX_SPTVFM_ALL0 >> 24) & HDMIRX_MASK_FF));		// 0x2223
	SET_BLOCK_DATA(data, 4, ((HDMIRX_SPTVFM_ALL1 >> 0) & HDMIRX_MASK_FF));		// 0x2224
	SET_BLOCK_DATA(data, 5, ((HDMIRX_SPTVFM_ALL1 >> 8) & HDMIRX_MASK_FF));		// 0x2225
	SET_BLOCK_DATA(data, 6, ((HDMIRX_SPTVFM_ALL1 >> 16) & HDMIRX_MASK_FF));		// 0x2226
	SET_BLOCK_DATA(data, 7, ((HDMIRX_SPTVFM_ALL1 >> 24) & HDMIRX_MASK_FF));		// 0x2227

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_sptpfm(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_SPTPFM_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_SPTPFM_00_ADDR,
						HDMIRX_SET_SPTPFM_LEN, data);
	SET_BLOCK_DATA(data, 0, ((HDMIRX_SPTPFM_ALL0 >> 0) & HDMIRX_MASK_FF));		// 0x2228
	SET_BLOCK_DATA(data, 1, ((HDMIRX_SPTPFM_ALL0 >> 8) & HDMIRX_MASK_FF));		// 0x2229
	SET_BLOCK_DATA(data, 2, ((HDMIRX_SPTPFM_ALL0 >> 16) & HDMIRX_MASK_FF));		// 0x222A
	SET_BLOCK_DATA(data, 3, ((HDMIRX_SPTPFM_ALL0 >> 24) & HDMIRX_MASK_FF));		// 0x222B
	SET_BLOCK_DATA(data, 4, ((HDMIRX_SPTPFM_ALL1 >> 0) & HDMIRX_MASK_FF));		// 0x222C
	SET_BLOCK_DATA(data, 5, ((HDMIRX_SPTPFM_ALL1 >> 8) & HDMIRX_MASK_FF));		// 0x222D
	SET_BLOCK_DATA(data, 6, ((HDMIRX_SPTPFM_ALL1 >> 16) & HDMIRX_MASK_FF));		// 0x222E
	SET_BLOCK_DATA(data, 7, ((HDMIRX_SPTPFM_ALL1 >> 24) & HDMIRX_MASK_FF));		// 0x222F

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_spthfm(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_SPTHFM_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_SPTHFM_00_ADDR,
						HDMIRX_SET_SPTHFM_LEN, data);
	SET_BLOCK_DATA(data, 0, ((HDMIRX_SPTHFM_ALL0 >> 0) & HDMIRX_MASK_FF));		// 0x2235
	SET_BLOCK_DATA(data, 1, ((HDMIRX_SPTHFM_ALL0 >> 8) & HDMIRX_MASK_FF));		// 0x2236
	SET_BLOCK_DATA(data, 2, ((HDMIRX_SPTHFM_ALL0 >> 16) & HDMIRX_MASK_FF));		// 0x2237

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_video_if_param(system_port port)
{
	uint32_t ret;

	// Set Support Video Format
	ret = hdmirx_set_sptvfm(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_sptpfm(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_spthfm(port);
	CHECK_ERROR(ret);
	// Set Output Color Format
	//   set by hdmirx_get_video_format()

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_audio_clock_regen_mode(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_AUDIO_CLOCK_REGEN_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_AUDIO_CLOCK_REGEN_MODE_NUM);
	// 0x22A3
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_RXACTL4_ADDR,
						HDMIRX_RXACTL4_CTSDIVMOD_ENA,
						HDMIRX_RXACTL4_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_support_audio_format(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SUPPORT_AUDIO_FORMAT_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SUPPORT_AUDIO_FORMAT_NUM);
	// ASREN: enable
	// HBRREN: enable
	// MSTREN: disable
	// BSSTOP: SPDIF and SDO output
	// 0x22A8
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_RXACTL9_ADDR,
						HDMIRX_RXACTL9_ASREN_ENA
							| HDMIRX_RXACTL9_HBRREN_ENA
							| HDMIRX_RXACTL9_MSTREN_DIS
						 	| HDMIRX_RXACTL9_BSSTOP_SPDIF_SDO,
						HDMIRX_RXACTL9_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_output_i2s_mode(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_OUTPUT_I2S_MODE_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_RXACTL5_ADDR,
						HDMIRX_SET_OUTPUT_I2S_MODE_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_RXACTL5_DEF);						// 0x22A4
	SET_BLOCK_DATA(data, 1, HDMIRX_RXACTL6_DEF);						// 0x22A5

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_output_master_clock_freq(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_RXMCLK1_00_ADDR,
						HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ1_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_RXMCLK1_SPDMEN_NOT_MASK
							| HDMIRX_AUDIO_MCLK_128FS);										// 0x22AC
	SET_BLOCK_DATA(data, 1, HDMIRX_AUDIO_MCLK_128FS
							| (HDMIRX_AUDIO_MCLK_128FS << HDMIRX_RXMCLK1_FS44MCLK_BIT));	// 0x22AD
	SET_BLOCK_DATA(data, 2, HDMIRX_AUDIO_MCLK_128FS
							| (HDMIRX_AUDIO_MCLK_128FS << HDMIRX_RXMCLK1_FS88MCLK_BIT));	// 0x22AE
	SET_BLOCK_DATA(data, 3, HDMIRX_AUDIO_MCLK_128FS
							| (HDMIRX_AUDIO_MCLK_128FS << HDMIRX_RXMCLK1_FS176MCLK_BIT));	// 0x22AF
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_RXMCLK2_00_ADDR,
						HDMIRX_SET_OUTPUT_MASTER_CLOCK_FREQ2_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_AUDIO_MCLK_128FS
							| (HDMIRX_AUDIO_MCLK_128FS << HDMIRX_RXMCLK2_FS128MCLK_BIT));	// 0x22B8
	SET_BLOCK_DATA(data, 1, HDMIRX_AUDIO_MCLK_128FS
							| (HDMIRX_AUDIO_MCLK_128FS << HDMIRX_RXMCLK2_FS384MCLK_BIT));	// 0x22B9
	SET_BLOCK_DATA(data, 2, HDMIRX_AUDIO_MCLK_128FS
							| HDMIRX_RXMCLK2_GAINCTL_UNLOCK
							| HDMIRX_RXMCLK2_RSTMODE_LARGE);								// 0x22BA
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_audio_ch_assign(system_port port, uint8_t chasgn)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_AUDIO_CH_ASSIGN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_AUDIO_CH_ASSIGN_NUM);
	// 0x22B2: CHASGN
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_RXADCH1_02_ADDR,
						chasgn,
						HDMIRX_RXADCH1_02_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_audio_mute_ctrl(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_AUDIO_MUTE_CTRL_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_RXMUTECTL_00_ADDR,
						HDMIRX_SET_AUDIO_MUTE_CTRL_LEN, data);
	// LKDMEN
	// HPLULMEN
	// DVIMMEN
	// FLATLMEN
	SET_BLOCK_DATA(data, 0, HDMIRX_RXMUTECTL_0_LKDMEN_MUTE
							| HDMIRX_RXMUTECTL_0_HPLULMEN_MUTE
							| HDMIRX_RXMUTECTL_0_DVIMMEN_MUTE
							| HDMIRX_RXMUTECTL_0_FLATLMEN_MUTE);		// 0x22FC
	// AVMMEN
	// ASPNDMEN
	SET_BLOCK_DATA(data, 1, HDMIRX_RXMUTECTL_1_AVMMEN_MUTE
							| HDMIRX_RXMUTECTL_1_ASPNDMEN_MUTE);		// 0x22FD
	SET_BLOCK_DATA(data, 2, HDMIRX_RXMUTECTL_2_DEF);					// 0x22FE
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_mode_ctrl1(system_port port, uint8_t mode, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_MODE_CTRL1_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_MODE_CTRL1_NUM);
	// 0x2230
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_MODCNT1_ADDR,
						mode,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_mode_ctrl2(system_port port, uint8_t mode, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_MODE_CTRL2_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_MODE_CTRL2_NUM);
	// 0x2231
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_MODCNT2_ADDR,
						mode,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_audio_if_param(system_port port)
{
	uint32_t ret;

	// Set Audio Clock Regeneration Mode
	ret = hdmirx_set_audio_clock_regen_mode(port);
	CHECK_ERROR(ret);
	// Set Support Audio Format
	ret = hdmirx_set_support_audio_format(port);
	CHECK_ERROR(ret);
	// Set Output I2S Mode
	ret = hdmirx_set_output_i2s_mode(port);
	CHECK_ERROR(ret);
	// Set Output Master Clock Frequency
	ret = hdmirx_set_output_master_clock_freq(port);
	CHECK_ERROR(ret);
	// Set Audio Channel Assignment (default)
	ret = hdmirx_set_audio_ch_assign(port, HDMIRX_RXADCH1_02_CHASGN_FR_FL);
	CHECK_ERROR(ret);
	// Set Audio Mute Control
	ret = hdmirx_set_audio_mute_ctrl(port);
	CHECK_ERROR(ret);
	// Set Audio Mute Enable
	ret = hdmirx_set_mode_ctrl2(port, HDMIRX_MODCNT2_INTR2ADM_MUTE, HDMIRX_MODCNT2_MASK);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_select_page_of_0x68c0(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SELECT_PAGE_OF_0X68C0_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SELECT_PAGE_OF_0X68C0_NUM);
	// 0x2814
	SET_COMMAND(cmd, 0, VIDEOIF_INF_RXVSICTL_00_ADDR,
						page,
						HDMIRX_RXVSICTL_00_HVSIPAGE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_packet_receive_mode(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_PACKET_RECEIVE_MODE_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_PACKET_RECEIVE_MODE_NUM);
	// 0x26D0
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_EPKTCTL_00_ADDR,
						HDMIRX_EPKTCTL_00_PKTMODSEL_FIRST_PACKET,
						HDMIRX_EPKTCTL_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_packet_hold(system_port port, uint8_t ctrl)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_PACKET_HOLD_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_PACKET_HOLD_NUM);
	// 0x26FB
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PKTHLDCTL_00_ADDR,
						ctrl,
						HDMIRX_PKTHLDCTL_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_hdmi_forum_vsi_id(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_HDMI_FORUM_VSI_ID_LEN];
	uint32_t ret;

	// page(0x68c0): 1: HDMI Forum Vendor Specific InfoFrame
	ret = hdmirx_select_page_of_0x68c0(port, HDMIRX_RXVSICTL_00_HVSIPAGE_HF_VSIF);
	CHECK_ERROR(ret);

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_INF_RXHFVSIMON_04_ADDR,
						HDMIRX_SET_HDMI_FORUM_VSI_ID_LEN, data);
	SET_BLOCK_DATA(data, 0, HDMIRX_RXHFVSIMON_04_ID1);					// 0x28C4
	SET_BLOCK_DATA(data, 1, HDMIRX_RXHFVSIMON_05_ID2);					// 0x28C5
	SET_BLOCK_DATA(data, 2, HDMIRX_RXHFVSIMON_06_ID3);					// 0x28C6

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	// page(0x68c0): 0: HDMI Vendor Specific InfoFrame
	ret = hdmirx_select_page_of_0x68c0(port, HDMIRX_RXVSICTL_00_HVSIPAGE_H_VSIF);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_packet_infoframe_param(system_port port)
{
	uint32_t ret;

	// Set Packet Receive Mode
	ret = hdmirx_set_packet_receive_mode(port);
	CHECK_ERROR(ret);
	// Set Packet Hold Disable
	ret = hdmirx_set_packet_hold(port, HDMIRX_PKTHLDCTL_00_PKTHOLD_NOT_HOLD);
	CHECK_ERROR(ret);
	// Set HDMI Forum Vendor Specific InfoFrame Identifier
	ret = hdmirx_set_hdmi_forum_vsi_id(port);
	CHECK_ERROR(ret);
	// not needed: Set Vendor Specific InfoFrame 0 Identifier
	// not needed: Set Vendor Specific InfoFrame 1 Identifier
	// not needed: Set Vendor Specific InfoFrame 2 Identifier
	// not needed: Set Vendor Specific InfoFrame 3 Identifier

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_mask_rx(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_INTR_MASK_RX_LEN];
	uint32_t ret;

	// Rx Interrupt Mask
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_IRQ_RINTMSK_00_ADDR,
						HDMIRX_SET_INTR_MASK_RX_LEN, data);
	// 0x40: FRQCHG:  TMDS Clock Frequency Changed
	// 0x10: UNKNOWN: Unknown Video Format Detect
	// 0x08: VFFAIL:  Video Format Detect Fail
	// 0x04: VFSUCS:  Video Format Detect Success
	// 0x01: LOCKDT:  TMDS PLL Lock Detect
	SET_BLOCK_DATA(data, 0, HDMIRX_RINTMSK_0_5D);						// 0x2061
	// 0x40: ACHCHG: Audio Channel Number Change Detect
	// 0x08: HPLOCK: Audio Hybrid PLL Lock Indicator
	// 0x04: HPULCK: Audio Hybrid PLL Unlock Indicator
	// NOTE: bit1-0 must not be set
	SET_BLOCK_DATA(data, 1, HDMIRX_RINTMSK_1_4C);						// 0x2062
	// 0x80: AMSTCHG: Audio Mute Status Changed
	// 0x08: CBTCHG:  Cbit Changed
	// 0x04: FSCHG:   Cbit FS Changed
	// 0x01: AFMTCHG: Audio Received Format Changed
	SET_BLOCK_DATA(data, 2, HDMIRX_RINTMSK_2_8D);						// 0x2063
	// 0x10: PKTSTCHG2:  Packet Status Change2
	// 0x08: PKTSTCHG1:  Packet Status Change1
	// 0x04: PKTSTCHG0:  Packet Status Change0
	// 0x01: APKTSTCHG0: Audio Packet Status Change0
	SET_BLOCK_DATA(data, 3, HDMIRX_RINTMSK_3_1D);						// 0x2064
	// 0x00
	SET_BLOCK_DATA(data, 4, HDMIRX_RINTMSK_4_NONE);						// 0x2065
	// 0x00
	SET_BLOCK_DATA(data, 5, HDMIRX_RINTMSK_5_NONE);						// 0x2066

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_mask_rx_1(system_port port, uint8_t intr, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_INTR_MASK_RX_1_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_INTR_MASK_RX_1_NUM);
	// set Interrupt Mask for #1 (0x2041)
	// 0x2061
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTMSK_00_ADDR,
						intr,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_audio_packet_mask_rx(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_INTR_AUDIO_PACKET_MASK_RX_LEN];
	uint32_t ret;

	// Rx Audio Packet Interrupt Mask
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_PKT_APKSTMSK_00_ADDR,
						HDMIRX_SET_INTR_AUDIO_PACKET_MASK_RX_LEN, data);
	// 0x08: HBRSTCHG: High Bitrate Audio Stream Packet Status Change.
	// 0x01: ASPSTCHG: ASP Packet Status Change
	SET_BLOCK_DATA(data, 0, HDMIRX_APKSTMSK_0_09);						// 0x26EC
	// 0x00
	SET_BLOCK_DATA(data, 1, HDMIRX_APKSTMSK_1_NONE);					// 0x26ED

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_packet_mask_rx(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_INTR_PACKET_MASK_RX_LEN];
	uint32_t ret;

	// Rx Packet Interrupt Mask
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_PKT_PKTSTMSK_00_ADDR,
						HDMIRX_SET_INTR_PACKET_MASK_RX_LEN, data);
	// 0x20: GMTSTCHG: Gamut Metadata Packet Status Change.
	// 0x10: ISBSTCHG: ISRC2 Packet Status Change
	// 0x08: ISASTCHG: ISRC1 Packet Status Change
	// 0x04: ACRSTCHG: Audio Clock Regeneration Packet Status Change
	// 0x01: GCPSTCHG: General Control Packet Status Change
	SET_BLOCK_DATA(data, 0, HDMIRX_PKTSTMSK_0_3D);						// 0x26F0
	// 0x08: MPEGSTCHG: MPEG Source InfoFrame Packet Status Change
	// 0x04: AUDSTCHG:  Audio InfoFrame Packet Status Change
	// 0x02: SPDSTCHG:  Source Product Descriptor InfoFrame Packet Status Change
	// 0x01: AVISTCHG:  AVI InfoFrame Packet Status Change
	SET_BLOCK_DATA(data, 1, HDMIRX_PKTSTMSK_1_0F);						// 0x26F1
	// 0x00
	SET_BLOCK_DATA(data, 2, HDMIRX_PKTSTMSK_2_NONE);					// 0x26F2

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_ext_intr_mask_rx(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_EXT_INTR_MASK_RX_LEN];
	uint32_t ret;

	// Rx Extended Interrupt Mask
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_IRQ_REINTMSK_00_ADDR,
						HDMIRX_SET_EXT_INTR_MASK_RX_LEN, data);
	// 0x40: FRQCHG:  TMDS Clock Frequency Changed
	// 0x10: UNKNOWN: Unknown Video Format Detect
	// 0x08: VFFAIL:  Video Format Detect Fail
	// 0x04: VFSUCS:  Video Format Detect Success
	// 0x01: LOCKDT:  TMDS PLL Lock Detect
	SET_BLOCK_DATA(data, 0, HDMIRX_REINTMSK_0_5D);						// 0x2071
	// 0x40: ACHCHG: Audio Channel Number Change Detect
	// 0x08: HPLOCK: Audio Hybrid PLL Lock Indicator
	// 0x04: HPULCK: Audio Hybrid PLL Unlock Indicator
	// NOTE: bit1-0 must not be set
	SET_BLOCK_DATA(data, 1, HDMIRX_REINTMSK_1_4C);						// 0x2072
	// 0x80: AMSTCHG: Audio Mute Status Changed
	// 0x08: CBTCHG:  Cbit Changed
	// 0x04: FSCHG:   Cbit FS Changed
	// 0x01: AFMTCHG: Audio Received Format Changed
	SET_BLOCK_DATA(data, 2, HDMIRX_REINTMSK_2_8D);						// 0x2073
	// 0x00
	SET_BLOCK_DATA(data, 3, HDMIRX_REINTMSK_3_NONE);					// 0x2074
	// 0x00
	SET_BLOCK_DATA(data, 4, HDMIRX_REINTMSK_4_NONE);					// 0x2075
	// 0x00
	SET_BLOCK_DATA(data, 5, HDMIRX_REINTMSK_5_NONE);					// 0x2076

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_intr_status_mask_rx(system_port port)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_INTR_STATUS_MASK_RX_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_IRQ_RISTMSK0_ADDR,
						HDMIRX_SET_INTR_STATUS_MASK_RX_LEN, data);
	// Rx Interrupt Status Mask
	SET_BLOCK_DATA(data, 0, HDMIRX_RISTMSK0_IRQMSA_ALL);						// 0x2058
	SET_BLOCK_DATA(data, 1, HDMIRX_RISTMSK1_IRQMSB_ALL);						// 0x2059
	// Rx Extended Interrupt Status Mask
	SET_BLOCK_DATA(data, 2, HDMIRX_REISTMSK0_EIRMSA_ALL);						// 0x205A
	SET_BLOCK_DATA(data, 3, HDMIRX_REISTMSK1_EIRMSB_ALL);						// 0x205B

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_receiver_link_init(system_port port)
{
	uint32_t ret;

	// page(0x60c4): 1
	ret = hdmirx_select_page_of_0x60c4(port, HDMIRX_RCKFQDET3_00_PAGE_NO_1);
	CHECK_ERROR(ret);
	// page(0x60c4): 1
	// Set Link Frequency Detect Parameter
	ret = hdmirx_set_link_freq_detect_param(port);
	CHECK_ERROR(ret);
	// page(0x60c4): 1
	// Set Link Reset Sequence Parameter
	ret = hdmirx_set_link_reset_seq_param(port);
	CHECK_ERROR(ret);
	// Set Video I/F Parameter
	ret = hdmirx_set_video_if_param(port);
	CHECK_ERROR(ret);
	// Set Audio I/F Parameter
	ret = hdmirx_set_audio_if_param(port);
	CHECK_ERROR(ret);
	// Set Packet and InfoFrame Parameter
	ret = hdmirx_set_packet_infoframe_param(port);
	CHECK_ERROR(ret);
	// Set Host Interrupt Mask
	// Set Audio Mute Interrupt Mask
	ret = hdmirx_set_intr_mask_rx(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_intr_audio_packet_mask_rx(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_intr_packet_mask_rx(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_ext_intr_mask_rx(port);
	CHECK_ERROR(ret);
	ret = hdmirx_set_intr_status_mask_rx(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_selected_rx_port(system_port port, uint8_t *p_port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_SELECTED_RX_PORT_NUM];
	uint32_t ret;

	if (p_port == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_SELECTED_RX_PORT_NUM);
	// 0x10A0
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE1_ADDR,
						0,
						HDMIRX_GET_SELECTED_RX_PORT_SWMODE1_PORT0OUT_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_port = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_receiver_port_select(system_port port)
{
	uint8_t selected_portno;
	uint8_t auphs, tcksel, pssyen;
	uint32_t ret;

	// get current selected port#
	ret = hdmirx_get_selected_rx_port(port, &selected_portno);
	CHECK_ERROR(ret);

	// setting of non-select port

	// Set Non-select Port Enable OFF
	ret = hdmirx_set_tmdsrx1_0_of_portno(port, selected_portno,
										HDMIRX_TMDSRXPX1_0_PORTENX_PORT_DIS,
										HDMIRX_TMDSRXPX1_0_PORTENX_MASK);
	CHECK_ERROR(ret);
	// Check Authentication Phase
	ret = hdmirx_get_auth_phase_of_portno(port, selected_portno, &auphs, &tcksel, &pssyen);
	CHECK_ERROR(ret);
	// AUPHSA == 0 or Upper 3.4Gbsp or not Fast Switch Mode
	if (auphs == 0 || tcksel == 0 || pssyen == 0) {
		ret = hdmirx_set_unselected_ddc_tmds_of_portno(port, selected_portno);
		CHECK_ERROR(ret);
	}

	// setting of select port

	// Set DDC ACK Mode of Select Port
	ret = hdmirx_set_sysconf10_of_portno(port, port.port_num,
										HDMIRX_SYSCFG1X_DDCNCSX_ACK,
										HDMIRX_SYSCFG1X_DDCNCSX_MASK);
	CHECK_ERROR(ret);
	// Set Select Port Enable ON
	// Set Terminate Resistor of Select Port ON: only when using Fast Switch Mode
	ret = hdmirx_set_tmdsrx1_0_of_portno(port, port.port_num,
										HDMIRX_TMDSRXPX1_0_PORTENX_PORT_ENA
											| ((pssyen != 0)
												? HDMIRX_TMDSRXPX1_0_TERMCKEN0_ENA
												: HDMIRX_TMDSRXPX1_0_TERMCKEN0_DIS),
										HDMIRX_TMDSRXPX1_0_PORTENX_MASK
											| HDMIRX_TMDSRXPX1_0_TERMCKEN0_MASK);
	CHECK_ERROR(ret);
	// Set Receiver Port Select
	ret = hdmirx_set_switch_mode1(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_data_swp_pol_portno(system_port port, uint8_t no, uint8_t swp, uint8_t inv)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_TMDS_DATA_SWP_POL_PORTNO_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_TMDS_DATA_SWP_POL_PORTNO_NUM);
	// 0x1010, 0x1011, 0x1012, 0x1013
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG2_ADDR + (no * HDMIRX_SADDR_10_PORT_OFFSET),
						swp,
						HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_MASK);

	// 0x1EC3, 0x1ED3, 0x1EE3, 0x1EF3
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_EQMANSET03_0_ADDR + (no * HDMIRX_SADDR_1E_PORT_OFFSET),
						inv,
						HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_tmds_data_swp(system_port port)
{
	uint8_t swp[SYSTEM_PORT_MAX_NUMBER];
	uint8_t inv[SYSTEM_PORT_MAX_NUMBER];
	uint8_t i;
	uint32_t ret;

	// TMDS data channel swap (depending on PCB)
#ifdef CONFIG_RX0_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_0] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_ENA;
#else // CONFIG_RX0_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_0] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_DIS;
#endif // CONFIG_RX0_TMDS_DATA_CHANNEL_SWAP
#ifdef CONFIG_RX1_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_1] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_ENA;
#else // CONFIG_RX1_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_1] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_DIS;
#endif // CONFIG_RX1_TMDS_DATA_CHANNEL_SWAP
#ifdef CONFIG_RX2_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_2] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_ENA;
#else // CONFIG_RX2_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_2] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_DIS;
#endif // CONFIG_RX2_TMDS_DATA_CHANNEL_SWAP
#ifdef CONFIG_HV2_REF_BOARD
	swp[SYSTEM_PORT_NUMBER_3] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_ENA;
#else // CONFIG_HV2_REF_BOARD
#ifdef CONFIG_RX3_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_3] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_ENA;
#else // CONFIG_RX3_TMDS_DATA_CHANNEL_SWAP
	swp[SYSTEM_PORT_NUMBER_3] = HDMIRX_SYSTEM_SYSCFGX_PXTMDSSW_SWP_DIS;
#endif // CONFIG_RX3_TMDS_DATA_CHANNEL_SWAP
#endif // CONFIG_HV2_REF_BOARD

	// TMDS data polarity inversion (depending on PCB)
#ifdef CONFIG_RX0_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_0] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_INV;
#else // CONFIG_RX0_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_0] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_NOR;
#endif // CONFIG_RX0_TMDS_DATA_POLARITY_INVERSION
#ifdef CONFIG_RX1_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_1] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_INV;
#else // CONFIG_RX1_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_1] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_NOR;
#endif // CONFIG_RX1_TMDS_DATA_POLARITY_INVERSION
#ifdef CONFIG_RX2_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_2] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_INV;
#else // CONFIG_RX2_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_2] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_NOR;
#endif // CONFIG_RX2_TMDS_DATA_POLARITY_INVERSION
#ifdef CONFIG_HV2_REF_BOARD
	inv[SYSTEM_PORT_NUMBER_3] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_INV;
#else // CONFIG_HV2_REF_BOARD
#ifdef CONFIG_RX3_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_3] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_INV;
#else // CONFIG_RX3_TMDS_DATA_POLARITY_INVERSION
	inv[SYSTEM_PORT_NUMBER_3] = HDMIRX_TMDS_EQMANSET03_X_POL_INVERTX_NOR;
#endif // CONFIG_RX3_TMDS_DATA_POLARITY_INVERSION
#endif // CONFIG_HV2_REF_BOARD

	for (i=0; i<SYSTEM_PORT_MAX_NUMBER; i++) {
		ret = hdmirx_set_tmds_data_swp_pol_portno(port, i, swp[i], inv[i]);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_irq_5v_power(system_port port, uint8_t *p_irq)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_IRQ_5V_POWER_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	// get 5v power status change
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_IRQ_5V_POWER_NUM);
	// 0x10FC
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG8_ADDR,
						0,
						HDMIRX_GET_IRQ_5V_POWER_INTREG8_PXV5STCHG_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_clr_irq_5v_power(system_port port, uint8_t irq)
{
	system_reg_access access;
	system_command cmd[HDMIRX_CLR_IRQ_5V_POWER_NUM];
	uint32_t ret;

	if (!irq) {
		return ERR_OK;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_CLR_IRQ_5V_POWER_NUM);
	// 0x10FC
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG8_ADDR,
						irq,
						HDMIRX_CLR_IRQ_5V_POWER_INTREG8_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_mon_5v_power(system_port port, uint8_t *p_mon)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_MON_5V_POWER_NUM];
	uint32_t ret;

	if (p_mon == NULL) {
		return ERR_NULL_POINTER;
	}

	// get 5v power monitor
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_MON_5V_POWER_NUM);
	// 0x1CFF
	SET_COMMAND(cmd, 0, VIDEOIF_MONITOR_MONREG5_03_ADDR,
						0,
						HDMIRX_GET_MON_5V_POWER_MONREG5_03_PXV5MN_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_mon = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_irq_tmds_clock(system_port port, uint8_t *p_irq)
{
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS clock frequency changed
	ret = hdmirx_get_irq0(port, p_irq);
	CHECK_ERROR(ret);

	p_irq[0] = p_irq[0] & HDMIRX_IRQ_INTREGX_FRQCHGX_MASK;
	p_irq[1] = p_irq[1] & HDMIRX_IRQ_INTREGX_FRQCHGX_MASK;
	p_irq[2] = p_irq[2] & HDMIRX_IRQ_INTREGX_FRQCHGX_MASK;
	p_irq[3] = p_irq[3] & HDMIRX_IRQ_INTREGX_FRQCHGX_MASK;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_irq1(system_port port, uint8_t *p_irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_RX_IRQ1_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}
	// get Interrupt Register #1
	// 0x2041
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_RX_IRQ1_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG1_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rx_irq1(system_port port, uint8_t irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RX_IRQ1_NUM];
	uint32_t ret;

	if (irq == 0) {
		return ERR_OK;
	}

	// set Interrupt Register #1
	// 0x2041
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RX_IRQ1_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG1_ADDR, irq, mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_irq3(system_port port, uint8_t *p_irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_RX_IRQ3_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}
	// get Interrupt Register #3
	// 0x2043
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_RX_IRQ3_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG3_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rx_irq3(system_port port, uint8_t irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RX_IRQ3_NUM];
	uint32_t ret;

	if (irq == 0) {
		return ERR_OK;
	}

	// set Interrupt Register #3
	// 0x2043
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RX_IRQ3_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG3_ADDR, irq, mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_irq6(system_port port, uint8_t *p_irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_RX_IRQ6_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}
	// get Interrupt Register #6
	// 0x2046
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_RX_IRQ6_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG6_00_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rx_irq6(system_port port, uint8_t irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RX_IRQ6_NUM];
	uint32_t ret;

	if (irq == 0) {
		return ERR_OK;
	}

	// get Interrupt Register #6
	// 0x2046
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RX_IRQ6_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG6_00_ADDR, irq, mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_mon(system_port port, uint8_t *p_mon, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_RX_MON_NUM];
	uint32_t ret;

	if (p_mon == NULL) {
		return ERR_NULL_POINTER;
	}
	// get Monitor Register
	// 0x20FB
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_RX_MON_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CKDET_MONREG_00_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_mon = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_video_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute)
{
	uint8_t irq, mon;
	uint32_t ret;

	if (p_irq == NULL || p_fmt == NULL || p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// detect video format
	ret = hdmirx_get_rx_irq1(port, p_irq, (HDMIRX_RINTREG1_UNKNOWN_MASK
											| HDMIRX_RINTREG1_VFSUCS_MASK));
	CHECK_ERROR(ret);
	if (*p_irq) {
		// video format has detected
		*p_fmt = HDMIRX_AV_STATUS_FORMAT_CHANGED;
	} else {
		//  video format has not detected
		*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
	}
	// get AVMCHG (AVMUTE changed)
	ret = hdmirx_get_rx_irq6(port, &irq, HDMIRX_RINTREG6_00_AVMCHG_MASK);
	CHECK_ERROR(ret);
	// get AVMMON (AVMUTE monitor)
	ret = hdmirx_get_rx_mon(port, &mon, HDMIRX_MONREG_00_AVMMON_MASK);
	CHECK_ERROR(ret);
	if (irq) {
		if (mon) {									// Set_AVMUTE
			*p_mute = HDMIRX_AV_STATUS_MUTE_ON;
		} else {									// Clear_AVMUTE
			*p_mute = HDMIRX_AV_STATUS_MUTE_OFF;
		}
		// clear AVMCHG
		ret = hdmirx_set_rx_irq6(port, irq, HDMIRX_RINTREG6_00_MASK);
		CHECK_ERROR(ret);
	} else {
		if (mon) {									// Set_AVMUTE
			*p_mute = HDMIRX_AV_STATUS_MUTE_ON;
		} else {									// Clear_AVMUTE
			*p_mute = HDMIRX_AV_STATUS_MUTE_OFF;
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_audio_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute)
{
	uint8_t irq;
	uint8_t mon;
	uint32_t ret;

	if (p_irq == NULL || p_fmt == NULL || p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// get AMSTCHG (audio mute status changed)
	ret = hdmirx_get_rx_irq3(port, &irq, HDMIRX_RINTREG3_AMSTCHG_MASK);
	CHECK_ERROR(ret);
	*p_irq = irq;
	// get ASTMON (audio mute status monitor)
	ret = hdmirx_get_rx_mon(port, &mon, HDMIRX_MONREG_00_ASTMON_MASK);
	CHECK_ERROR(ret);
	if (irq) {
		if (mon) {											// audio is muted
			// audio format has not changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
		} else {											// audio is unmuted
			// audio format might have changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_CHANGED;
		}
	} else {
		// audio format has not changed
		*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
	}
	if (mon) {												// audio is muted
		*p_mute = HDMIRX_AV_STATUS_MUTE_ON;
	} else {												// audio is unmuted
		*p_mute = HDMIRX_AV_STATUS_MUTE_OFF;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_avi_infoframe_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute)
{
	uint8_t stat, irq;
	uint32_t ret;

	if (p_irq == NULL || p_fmt == NULL || p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// get AVI InfoFrame Packet Detect Status
	ret = hdmirx_get_packet_status1(port, &stat, HDMIRX_PKTDETS1_AVIDETS_MASK);
	CHECK_ERROR(ret);
	// get AVI InfoFrame Parameter Changed
	ret = hdmirx_get_packet_param_change1(port, &irq, HDMIRX_PPMCHG1_AVICHG_MASK);
	CHECK_ERROR(ret);
	*p_irq = irq;
	if (stat) {													// AVI is enabled
		*p_mute = HDMIRX_AV_STATUS_MUTE_OFF;
		if (irq) {												// AVI param has changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_CHANGED;
		} else {												// AVI param has not changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
		}
	} else {													// AVI is not enabled
		*p_mute = HDMIRX_AV_STATUS_MUTE_ON;
		*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_spd_infoframe_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute)
{
	uint8_t stat, irq;
	uint32_t ret;

	if (p_irq == NULL || p_fmt == NULL || p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// get SPD InfoFrame Packet Detect Status
	ret = hdmirx_get_packet_status1(port, &stat, HDMIRX_PKTDETS1_SPDDETS_MASK);
	CHECK_ERROR(ret);
	// get SPD InfoFrame Parameter Changed
	ret = hdmirx_get_packet_param_change1(port, &irq, HDMIRX_PPMCHG1_SPDCHG_MASK);
	CHECK_ERROR(ret);
	*p_irq = irq;
	if (stat) {													// SPD is enabled
		*p_mute = HDMIRX_AV_STATUS_MUTE_OFF;
		if (irq) {												// SPD param has changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_CHANGED;
		} else {												// SPD param has not changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
		}
	} else {													// SPD is not enabled
		*p_mute = HDMIRX_AV_STATUS_MUTE_ON;
		*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_rx_hdr_infoframe_status(system_port port, uint8_t *p_irq,
						hdmirx_av_status_format *p_fmt, hdmirx_av_status_mute *p_mute)
{
	uint8_t stat, irq;
	uint32_t ret;

	if (p_irq == NULL || p_fmt == NULL || p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	// set HDR InfoFrame Header to Free Packet Register
	ret = hdmirx_set_free_packet_header(port, HDMIRX_RFREPKT_FRE00H_HDR_TYPE);
	CHECK_ERROR(ret);
	// get HDR InfoFrame Packet Detect Status
	ret = hdmirx_get_packet_status0(port, &stat, HDMIRX_PKTDETS0_FREDETS_MASK);
	CHECK_ERROR(ret);
	// get HDR InfoFrame Parameter Changed
	ret = hdmirx_get_packet_param_change0(port, &irq, HDMIRX_PPMCHG0_FRECHG_MASK);
	CHECK_ERROR(ret);
	*p_irq = irq;
	if (stat) {													// HDR is enabled
		*p_mute = HDMIRX_AV_STATUS_MUTE_OFF;
		if (irq) {												// HDR param has changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_CHANGED;
		} else {												// HDR param has not changed
			*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
		}
	} else {													// HDR is not enabled
		*p_mute = HDMIRX_AV_STATUS_MUTE_ON;
		*p_fmt = HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scdcs_source_ver(system_port port, uint8_t scdcs)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SCDCS_SOURCE_VER_NUM];
	uint32_t ret;

	// page: 10, 11, 12, 13 (according to port.port_num)

	// set Source Version
	// 0x1802 (page: 10, 11, 12, 13)
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SCDCS_SOURCE_VER_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SCDCS_SRCVER0_ADDR, scdcs, HDMIRX_SRCVERX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_scdcs_tmds_config(system_port port, uint8_t *p_scdcs, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_SCDCS_TMDS_CONFIG_NUM];
	uint32_t ret;

	if (p_scdcs == NULL) {
		return ERR_NULL_POINTER;
	}

	// page: 10, 11, 12, 13 (according to port.port_num)

	// get TMDS Configuration of SCDCS
	// 0x1820 (page: 10, 11, 12, 13)
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_SCDCS_TMDS_CONFIG_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SCDCS_TMDSCFG0_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_scdcs = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scdcs_tmds_config(system_port port, uint8_t scdcs)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SCDCS_TMDS_CONFIG_NUM];
	uint32_t ret;

	// page: 10, 11, 12, 13 (according to port.port_num)

	// set TMDS Configuration of SCDCS
	// 0x1820 (page: 10, 11, 12, 13)
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SCDCS_TMDS_CONFIG_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SCDCS_TMDSCFG0_ADDR, scdcs, HDMIRX_TMDSCFGX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scdcs_scrambler_status(system_port port, uint8_t scdcs)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SCDCS_SCRAMBLER_STATUS_NUM];
	uint32_t ret;

	// page: 10, 11, 12, 13 (according to port.port_num)

	// set Scrambler Status of SCDCS
	// 0x1821 (page: 10, 11, 12, 13)
	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SCDCS_SCRAMBLER_STATUS_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SCDCS_SCRSTAT0_ADDR, scdcs, HDMIRX_SCRSTATX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_scdcs_scrambling_status(system_port port, uint8_t no)
{
	uint8_t scrmon;
	uint8_t scr_enable;
	uint32_t ret;

	// get Scramble Monitor
	ret = hdmirx_get_scramble_mon_of_portno(port, no, &scrmon);
	CHECK_ERROR(ret);
	// get Scrambling_Enable
	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_select_page_of_0x18(port, HDMIRX_SELECT_PAGE_OF_0X18_SCDCS);
	CHECK_ERROR(ret);
	ret = hdmirx_get_scdcs_tmds_config(port, &scr_enable, HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_MASK);
	CHECK_ERROR(ret);
	// set Scrambling_Status
	if (scrmon && scr_enable) {
		// detected
		ret = hdmirx_set_scdcs_scrambler_status(port, HDMIRX_SCRSTATX_SCRAMBLING_STATUS_DETECT);
		CHECK_ERROR(ret);
	} else {
		// not detected
		ret = hdmirx_set_scdcs_scrambler_status(port, HDMIRX_SCRSTATX_SCRAMBLING_STATUS_NOT_DETECT);
		CHECK_ERROR(ret);
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_access_tmds_speed_mode(system_port port, hdmirx_tmds_speed_mode *p_get,
																hdmirx_tmds_speed_mode *p_set)
{
	static hdmirx_tmds_speed_mode tmds_sm[SYSTEM_CHIP_MAX_NUMBER][SYSTEM_PORT_MAX_NUMBER]
											= {{ HDMIRX_TMDS_SPEED_MODE_LEGACY }};

	if (p_get == NULL && p_set == NULL) {
		return ERR_NULL_POINTER;
	}
	if (p_get) {
		*p_get = tmds_sm[port.chip_num][port.port_num];
	} else if (p_set) {
		tmds_sm[port.chip_num][port.port_num] = *p_set;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_tmds_alignment_lock_detect(system_port port)
{
	uint8_t tmds_config, tmds_rate;
	uint8_t ckdtprm2, ckdtprm2_mask;
	uint8_t tmdsrxpx_1_0, tmdsrxpx_1_1, tmdsrxpx_1_2;
	hdmirx_tmds_speed_mode tmds_sm;
	uint32_t ret;

	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_select_page_of_0x18(port, HDMIRX_SELECT_PAGE_OF_0X18_SCDCS);
	CHECK_ERROR(ret);
	// get TMDS config (according to port.port_num)
	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_get_scdcs_tmds_config(port, &tmds_config,
										HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_MASK
										| HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_MASK);
	CHECK_ERROR(ret);
	// get TMDS Data Rate (Rx Port0,1,2,3)
	ret = hdmirx_get_tmds_data_rate_of_portno(port, port.port_num, &tmds_rate);
	CHECK_ERROR(ret);

	if ((tmds_config & HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_MASK)
		|| (tmds_rate > HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHF_FDTTHG)) {
		// Set TMDS High Speed Mode
		ckdtprm2 = (HDMIRX_CKDTPRM2_P0TCKSEL_CRU << port.port_num)
					| (HDMIRX_CKDTPRM2_P0TCLK4DIV_4DIV << port.port_num);		// 0x1295
		tmdsrxpx_1_0 = HDMIRX_TMDSRXPX1_0_TERMCKEN0_DIS;						// 0x1200
		tmdsrxpx_1_2 = HDMIRX_TMDSRXPX1_2_VALX_4STAGES
						| HDMIRX_TMDSRXPX1_2_QMODE_T_12_STEP;					// 0x1202
		tmds_sm = HDMIRX_TMDS_SPEED_MODE_HIGH;
	} else {
		// Set TMDS Legacy Speed Mode
		ckdtprm2 = (HDMIRX_CKDTPRM2_P0TCKSEL_TMDS << port.port_num)
					| (HDMIRX_CKDTPRM2_P0TCLK4DIV_NOR << port.port_num);		// 0x1295
		tmdsrxpx_1_0 = HDMIRX_TMDSRXPX1_0_TERMCKEN0_ENA;						// 0x1200
		tmdsrxpx_1_2 = HDMIRX_TMDSRXPX1_2_VALX_2STAGES
						| HDMIRX_TMDSRXPX1_2_QMODE_T_6_STEP;					// 0x1202
		tmds_sm = HDMIRX_TMDS_SPEED_MODE_LEGACY;
	}
	ret = hdmirx_access_tmds_speed_mode(port, NULL, &tmds_sm);
	CHECK_ERROR(ret);
	ckdtprm2_mask = (HDMIRX_CKDTPRM2_P0TCKSEL_MASK << port.port_num)
					| (HDMIRX_CKDTPRM2_P0TCLK4DIV_MASK << port.port_num);

	if (tmds_config & HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_MASK) {
		// Enable scrambling in sink
		tmdsrxpx_1_1 = HDMIRX_TMDSRXPX1_1_TOFFX_SCRAMBLE_ON;					// 0x1201
	} else {
		// Disable scrambling in sink
		tmdsrxpx_1_1 = HDMIRX_TMDSRXPX1_1_TOFFX_SCRAMBLE_OFF;					// 0x1201
	}
	// 0x1295
	ret = hdmirx_set_clock_freq_detect_param2(port, ckdtprm2, ckdtprm2_mask);
	CHECK_ERROR(ret);
	// 0x1200, 0x1201, 0x1202
	ret = hdmirx_set_tmdsrx1_0_2_of_portno(port, port.port_num,
											tmdsrxpx_1_0,
											HDMIRX_TMDSRXPX1_0_TERMCKEN0_MASK,
											tmdsrxpx_1_1,
											HDMIRX_TMDSRXPX1_1_TOFFX_MASK,
											tmdsrxpx_1_2,
											(HDMIRX_TMDSRXPX1_2_VALX_MASK
												| HDMIRX_TMDSRXPX1_2_QMODE_MASK));
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_equalizer_max_limit_of_portno(system_port port, uint8_t no, uint8_t max)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_EQUALIZER_MAX_LIMIT_OF_PORTNO_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_EQUALIZER_MAX_LIMIT_OF_PORTNO_NUM);
	// EQSETMAXLIMIT*
	// 0x1EC8, 0x1ED8, 0x1EE8, 0x1EF8
	SET_COMMAND(cmd, 0, VIDEOIF_TMDS_EQMANSET20_0_ADDR + (no * HDMIRX_SADDR_1E_PORT_OFFSET),
						max,
						HDMIRX_EQMANSET20_X_EQSETMAXLIMITX_MASK);
	// EQMAXLIMITEN*: if max is 0, disable
	// 0x1ECD, 0x1EDD, 0x1EED, 0x1EFD
	SET_COMMAND(cmd, 1, VIDEOIF_TMDS_EQMANSET31_0_ADDR + (no * HDMIRX_SADDR_1E_PORT_OFFSET),
						(max)
							? HDMIRX_EQMANSET31_X_EQMAXLIMITENX_ENA
							: HDMIRX_EQMANSET31_X_EQMAXLIMITENX_DIS,
						HDMIRX_EQMANSET31_X_EQMAXLIMITENX_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_tmds_calibration(system_port port)
{
	uint8_t tmds_freq;
	uint8_t eqsetout[HDMIRX_GET_TMDSRX5_1_3_OF_PORTNO_LEN];
	uint8_t eqset;
	uint32_t ret;

	CHECK_ABORT();
	// wait for 100ms
	SLEEP(HDMIRX_SLEEP_TIME_100MSEC);
	// Initialize Calibration
	ret = hdmirx_set_tmdsrx4_0_of_portno(port, port.port_num,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_INIT,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_INIT_MASK);
	CHECK_ERROR(ret);
	// get TMDS clock frequency (Rx Port0,1,2,3)
	ret = hdmirx_get_tmds_clock_freq_of_portno(port, port.port_num, &tmds_freq);
	CHECK_ERROR(ret);
	// P*TCLKFQ == 0x2 ?
	if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHA_FDTTHB) {
		// Equalizer Value Limit Enable
		ret = hdmirx_set_equalizer_max_limit_of_portno(port, port.port_num,
														HDMIRX_EQMANSET20_X_EQSETMAXLIMITX_9);
		CHECK_ERROR(ret);
	}
	// Start Calibration
	ret = hdmirx_set_tmdsrx5_0_of_portno(port, port.port_num,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX5_0_AUTO,
											HDMIRX_TMDSRXPX5_0_MASK);
	CHECK_ERROR(ret);
	ret = hdmirx_set_tmdsrx4_0_of_portno(port, port.port_num,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_START,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_START_MASK);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// wait for 100ms
	SLEEP(HDMIRX_SLEEP_TIME_100MSEC);
	// Stop Calibration
	ret = hdmirx_set_tmdsrx4_0_of_portno(port, port.port_num,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_STOP,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_STOP_MASK);
	CHECK_ERROR(ret);
	// Check Calibration Result
	ret = hdmirx_get_tmdsrx5_1_3_of_portno(port, port.port_num, eqsetout);
	CHECK_ERROR(ret);
	eqsetout[0] = eqsetout[0] & HDMIRX_TMDSRXPX5_EQSETOUTX_MASK;
	eqsetout[1] = eqsetout[1] & HDMIRX_TMDSRXPX5_EQSETOUTX_MASK;
	eqsetout[2] = eqsetout[2] & HDMIRX_TMDSRXPX5_EQSETOUTX_MASK;
	// EQSETOUT0,1,2 <= 3 ?
	if ((eqsetout[0] <= HDMIRX_TMDSRXPX5_EQSETOUTX_3)
		&& (eqsetout[1] <= HDMIRX_TMDSRXPX5_EQSETOUTX_3)
		&& (eqsetout[2] <= HDMIRX_TMDSRXPX5_EQSETOUTX_3)) {
		eqset = HDMIRX_TMDSRXPX2_0_EQSETX_3;
	} else {
		// Calcurate Equalizer Value
		eqset = (eqsetout[0] + eqsetout[1] + eqsetout[2]) / 3 + 1;
		if (eqset > HDMIRX_TMDSRXPX2_0_EQSETX_MASK) {
			eqset = HDMIRX_TMDSRXPX2_0_EQSETX_MASK;
		}
	}
	// Set TMDS Equalizer
	ret = hdmirx_set_tmdsrx5_0_of_portno(port, port.port_num,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX5_0_MANUAL,
											HDMIRX_TMDSRXPX5_0_MASK);
	CHECK_ERROR(ret);
	ret = hdmirx_set_tmdsrx2_0_of_portno(port, port.port_num,
											eqset,
											HDMIRX_TMDSRXPX2_0_EQSETX_MASK);
	CHECK_ERROR(ret);
	ret = hdmirx_set_tmdsrx4_0_of_portno(port, port.port_num,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_EXIT,
											HDMIRX_TMDS_CALIBRATION_TMDSRXPX4_0_EXIT_MASK);
	CHECK_ERROR(ret);
	// Equalizer Value Limit Disable
	ret = hdmirx_set_equalizer_max_limit_of_portno(port, port.port_num,
													HDMIRX_EQMANSET20_X_EQSETMAXLIMITX_0);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rxpll0en(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RXPLL0EN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RXPLL0EN_NUM);
	// 0x1E41: RXPLL0EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL0_01_ADDR,
						en,
						HDMIRX_RXPLL0_01_RXPLL0EN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rxpll00en(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RXPLL00EN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RXPLL00EN_NUM);
	// 0x1E45: RXPLL00EN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_RXPLL00_03_ADDR,
						en,
						HDMIRX_RXPLL00_03_RXPLL00EN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rx0_sen(system_port port, uint8_t en)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RX0_SEN_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RX0_SEN_NUM);
	// 0x1E7F: RX0_SEN: auto/manual
	SET_COMMAND(cmd, 0, VIDEOIF_CKGEN_CKSELEN_ADDR,
						en,
						HDMIRX_CKSELEN_RX0_SEN_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_pixel_clock(system_port port)
{
	uint32_t ret;

	ret = hdmirx_set_rxpll0en(port, HDMIRX_RXPLL0_01_RXPLL0EN_AUTO);
	CHECK_ERROR(ret);
	ret = hdmirx_set_rxpll00en(port, HDMIRX_RXPLL00_03_RXPLL00EN_AUTO);
	CHECK_ERROR(ret);
	ret = hdmirx_set_rx0_sen(port, HDMIRX_CKSELEN_RX0_SEN_AUTO);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_change_receiver_clock_setting(system_port port)
{
	uint32_t ret;

	// Set Pixel Clock Setting
	ret = hdmirx_set_pixel_clock(port);
	CHECK_ERROR(ret);
	// Reset Video FIFO
	ret = hdmirx_set_rx_soft_reset1(port, HDMIRX_RSRST1_VIFRST);
	CHECK_ERROR(ret);
	CHECK_ABORT();
	// wait for 200ms
	SLEEP(HDMIRX_SLEEP_TIME_200MSEC);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_gcp_color_depth(system_port port, uint8_t *p_cd)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_GCP_COLOR_DEPTH_NUM];
	uint32_t ret;

	if (p_cd == NULL) {
		return ERR_NULL_POINTER;
	}
	// get Color Depth of GCP (General Control Packet)
	//       +7  +6  +5  +4  +3  +2  +1  +0
	//   HB0
	//   HB1
	//   HB2
	//   SB0
	//   SB1                 CD3 CD2 CD1 CD0
	//
	//                       0   0   0   0   : Color Depth not indicated
	//                       0   0   0   1   : Reserved
	//                       0   0   1   0   : Reserved
	//                       0   0   1   1   : Reserved
	//                       0   1   0   0   : 24 bits per pixel
	//                       0   1   0   1   : 30 bits per pixel
	//                       0   1   1   0   : 36 bits per pixel
	//                       0   1   1   1   : 48 bits per pixel
	//
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_GCP_COLOR_DEPTH_NUM);
	// 0x2604
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_RGCPPKT_04_ADDR, 0, HDMIRX_RGCPPKT_04_CD_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_cd = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_tmds_clock_cnt_of_portno(system_port port, uint8_t no, uint8_t *p_cnt)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_cnt == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS Clock Counter Value (Rx Port0,1,2,3)
	// 0x12D0-0x12D2 or 0x12D4-0x12D6 or 0x12D8-0x12DA or 0x12DC-0x12DE
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_CKDET21_00_ADDR + (no * HDMIRX_SADDR_12D_PORT_OFFSET),
						HDMIRX_GET_TMDS_CLOCK_CNT_OF_PORTNO_LEN, p_cnt);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_tmds_data_rate_of_portno(system_port port, uint8_t no, uint8_t *p_rate)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_TMDS_DATA_RATE_OF_PORTNO_NUM];
	uint32_t ret;

	if (p_rate == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS Data Rate (Rx Port0,1,2,3)
	// P0TMDSFQ[2:0]: 000:    0 -  450 Mbps
	//                001:  450 -  855 Mbps
	//                010:  855 - 1230 Mbps
	//                011: 1230 - 1750 Mbps
	//                100: 1750 - 2500 Mbps
	//                101: 2500 - 3420 Mbps
	//                110: 3420 - 4920 Mbps
	//                111: 4920 -      Mbps
	// 0x12D3 or 0x12D7 or 0x12DB or 0x12DF
	SET_ACCESS(access, &port, cmd, HDMIRX_GET_TMDS_DATA_RATE_OF_PORTNO_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_CKDET_CKDET21_03_ADDR + (no * HDMIRX_SADDR_12D_PORT_OFFSET),
						0,
						HDMIRX_CKDET2X_3_PXTMDSFQ_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_rate = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_calc_pixel_clock_freq(system_port port, uint8_t pckmon, uint32_t *p_pcf)
{
	uint8_t pxtcc[HDMIRX_GET_TMDS_CLOCK_CNT_OF_PORTNO_LEN];
	uint8_t tmds_bit_clock_ratio;
	uint32_t tmds_clock_cnter;
	uint32_t ret;

	if (p_pcf == NULL) {
		return ERR_NULL_POINTER;
	}

	// get TMDS Clock Counter Value (Rx Port0,1,2,3)
	ret = hdmirx_get_tmds_clock_cnt_of_portno(port, port.port_num, pxtcc);
	CHECK_ERROR(ret);
	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_select_page_of_0x18(port, HDMIRX_SELECT_PAGE_OF_0X18_SCDCS);
	CHECK_ERROR(ret);
	// get TMDS_Bit_Clock_Ratio of TMDS config (according to port.port_num)
	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_get_scdcs_tmds_config(port, &tmds_bit_clock_ratio,
										HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_MASK);
	CHECK_ERROR(ret);
	if (pckmon == HDMIRX_PCKMON_UNSPECIFIED) {
		// get Pixel Clock Monitor
		ret = hdmirx_get_rx_mon(port, &pckmon, HDMIRX_MONREG_00_PCKMON_MASK);
		CHECK_ERROR(ret);
	}

	tmds_clock_cnter = ((uint32_t)pxtcc[2] << 16) | ((uint32_t)pxtcc[1] << 8) | ((uint32_t)pxtcc[0]);
	// calc pixel clock [kHz]
	*p_pcf = (tmds_clock_cnter * 1000)
				* (tmds_bit_clock_ratio
					? HDMIRX_PIXEL_CLOCK_MULTI_4
					: HDMIRX_PIXEL_CLOCK_MULTI_1)
				/ ((pckmon == HDMIRX_PIXEL_CLOCK_24BIT)
					? HDMIRX_PIXEL_CLOCK_DIV_1000
					: (pckmon == HDMIRX_PIXEL_CLOCK_30BIT)
					? HDMIRX_PIXEL_CLOCK_DIV_1250
					: (pckmon == HDMIRX_PIXEL_CLOCK_36BIT)
					? HDMIRX_PIXEL_CLOCK_DIV_1500
					: HDMIRX_PIXEL_CLOCK_DIV_2000);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_calc_pixel_clock_range(system_port port, uint8_t pckmon, uint8_t *p_pcr)
{
	uint32_t pcf;
	uint32_t ret;

	if (p_pcr == NULL) {
		return ERR_NULL_POINTER;
	}

	// calculate pixel clock
	ret = hdmirx_get_calc_pixel_clock_freq(port, pckmon, &pcf);
	CHECK_ERROR(ret);
	// convert to range
	*p_pcr = (pcf < HDMIRX_PIXEL_CLOCK_44000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_LOWER_PDTTHA				// 0MHz - 44MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_65700KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHA_PDTTHB				// 44MHz - 65.7MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_92000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHB_PDTTHC				// 65.7MHz - 92MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_190000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHC_PDTTHD				// 92MHz - 190MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_350000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHD_PDTTHE				// 190MHz - 350MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_620000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHE_PDTTHF				// 350MHz - 620MHz
				: HDMIRX_RCKFQDET1_PCLKFQ_UPPER_PDTTHF;				// 620MHz -

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_pixel_clock_freq(system_port port, uint32_t *p_pcf)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_PIXEL_CLOCK_FREQ_NUM];
	system_block_access baccess;
	uint8_t data[HDMIRX_GET_PIXEL_CLOCK_FREQ_LEN];
	uint8_t icfmt;
	uint32_t pckc;
	uint32_t ret;

	if (p_pcf == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_PIXEL_CLOCK_FREQ_NUM);
	// 0x2272: ICFMT
	SET_COMMAND(cmd, 0, VIDEOIF_CC_CSCMOD3_ADDR, 0, HDMIRX_CSCMOD3_ICFMT_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	icfmt = cmd[0].data >> HDMIRX_CSCMOD3_ICFMT_BIN;

	// page(0x60c4): 1
	ret = hdmirx_select_page_of_0x60c4(port, HDMIRX_RCKFQDET3_00_PAGE_NO_1);
	CHECK_ERROR(ret);
	// page(0x60c4): 1
	// 0x20E8 - 0x20EA: PCKC: Pixel CLock Counter Value
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKDET_RCFDPRM12_00_ADDR,
						HDMIRX_GET_PIXEL_CLOCK_FREQ_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	pckc = (uint32_t)data[HDMIRX_RCFDPRM12_00_PCKC_7_0_IDX] << HDMIRX_RCFDPRM12_00_PCKC_7_0_BIT
			| (uint32_t)data[HDMIRX_RCFDPRM12_00_PCKC_15_8_IDX] << HDMIRX_RCFDPRM12_00_PCKC_15_8_BIT
			| (uint32_t)data[HDMIRX_RCFDPRM12_00_PCKC_19_16_IDX] << HDMIRX_RCFDPRM12_00_PCKC_19_16_BIT;

	// if ICFMT is set by YCbCr4:2:0 BT.601/709,
	// pckc is divided by 2 regardless of a present color format.
	// the pixel clock freq excluding the above are the same as 0x20C0 bit6-4 (PCLKFQ[2:0]).
	if (is_color_format_yuv420(icfmt)) {
		pckc = pckc / 2;
	}
	*p_pcf = pckc;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_pixel_clock_range(system_port port, uint8_t *p_pcr)
{
	uint32_t pcf;
	uint32_t ret;

	if (p_pcr == NULL) {
		return ERR_NULL_POINTER;
	}

	// get pixel clock frequency
	ret = hdmirx_get_pixel_clock_freq(port, &pcf);
	CHECK_ERROR(ret);
	// convert to range
	*p_pcr = (pcf < HDMIRX_PIXEL_CLOCK_44000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_LOWER_PDTTHA				// 0MHz - 44MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_65700KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHA_PDTTHB				// 44MHz - 65.7MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_92000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHB_PDTTHC				// 65.7MHz - 92MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_190000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHC_PDTTHD				// 92MHz - 190MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_350000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHD_PDTTHE				// 190MHz - 350MHz
				: (pcf < HDMIRX_PIXEL_CLOCK_620000KHZ)
				? HDMIRX_RCKFQDET1_PCLKFQ_PDTTHE_PDTTHF				// 350MHz - 620MHz
				: HDMIRX_RCKFQDET1_PCLKFQ_UPPER_PDTTHF;				// 620MHz -

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_detected_vformat(system_port port, uint8_t *p_vform)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_vform == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Video Format Detect
	// 0x2200: DETVFM: Detected Video Format
	// 0x2201: PCFDMT: PC Format DMT ID Number
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_VFMDET_00_ADDR,
						HDMIRX_GET_DETECTED_VFORMAT_LEN, p_vform);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_current_param_latch(system_port port)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_CURRENT_PARAM_LATCH];
	uint32_t ret;

	CHECK_ABORT();

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_CURRENT_PARAM_LATCH);
	// select HDMI-Rx
	// CRRLATCH:
	// 0x2209
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_CRRSYNC_01_ADDR,
						HDMIRX_CRRSYNC_01_CRRLATCH_LATCH,
						HDMIRX_CRRSYNC_01_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);
	SET_COMMAND(cmd, 0, VIDEOIF_VIDEO_CRRSYNC_01_ADDR,
						HDMIRX_CRRSYNC_01_CRRLATCH_CLR,
						HDMIRX_CRRSYNC_01_MASK);
	ret = device_read_wait(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_hori_vert_period(system_port port, uint8_t *p_period)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_period == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Horizontal/Vertical Period
	// 0x2204-0x2207
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_HVPERIOD_00_ADDR,
						HDMIRX_GET_HORI_VERT_PERIOD_LEN, p_period);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_current_hori_vert_period(system_port port, uint8_t *p_period)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_period == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Current Horizontal/Vertical Period
	// 0x2210-0x221E
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_CRRHVPERIOD_00_ADDR,
						HDMIRX_GET_CURRENT_HORI_VERT_PERIOD_LEN, p_period);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_pc_format(hdmirx_video *p_video, uint8_t *p_vform,
										uint8_t *p_hv, uint8_t *p_curr_hv, system_video_format *p_vf)
{
	uint8_t detvfm, pcfdmt;
	uint32_t pcf;
	uint32_t hpe, hsw, hde, vde;
	uint32_t ret = ERR_OK;

	if (p_vform == NULL || p_hv == NULL || p_curr_hv == NULL || p_vf == NULL) {
		return ERR_NULL_POINTER;
	}

	detvfm = p_vform[HDMIRX_DETVFM_INDEX];
	pcfdmt = p_vform[HDMIRX_PCFDMT_INDEX];

	if (pcfdmt) {
		// calculate pixel clock
		ret = hdmirx_get_calc_pixel_clock_freq(p_video->rx_port, HDMIRX_PCKMON_UNSPECIFIED, &pcf);
		CHECK_ERROR(ret);
		if (pcf == HDMIRX_PIXEL_CLOCK_0KHZ || pcf > HDMIRX_PIXEL_CLOCK_620000KHZ) {
			// TMDS clock counter is zero
			// TMDS clock counter is over 620MHz
			return ERR_OK;
		}
		if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_VGA && detvfm == HDMIRX_VFMDET_00_DETVFM_VGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L31500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H31500KHZ) {				// 31500kHz
				// Horizontal Period
				hpe = ((p_hv[HDMIRX_HVPERIOD_01_IDX] & HDMIRX_HVPERIOD_01_MASK) << HDMIRX_HVPERIOD_01_IDX_BIT)
						| (p_hv[HDMIRX_HVPERIOD_00_IDX] & HDMIRX_HVPERIOD_00_MASK);
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_VGA_640x480_72))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_VGA_640x480_72))) {
					*p_vf = SYSTEM_VFORM_PC_VGA_640x480_72;
				} else if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_VGA_640x480_75))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_VGA_640x480_75))) {
					*p_vf = SYSTEM_VFORM_PC_VGA_640x480_75;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L36000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H36000KHZ) {		// 36000kHz
				*p_vf = SYSTEM_VFORM_PC_VGA_640x480_85;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_SVGA && detvfm == HDMIRX_VFMDET_00_DETVFM_SVGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L36000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H36000KHZ) {				// 36000kHz
				*p_vf = SYSTEM_VFORM_PC_SVGA_800x600_56;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L40000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H40000KHZ) {		// 40000kHz
				*p_vf = SYSTEM_VFORM_PC_SVGA_800x600_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L50000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H50000KHZ) {		// 50000kHz
				*p_vf = SYSTEM_VFORM_PC_SVGA_800x600_72;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L49500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H49500KHZ) {		// 49500kHz
				*p_vf = SYSTEM_VFORM_PC_SVGA_800x600_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L56250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H56250KHZ) {		// 56250kHz
				*p_vf = SYSTEM_VFORM_PC_SVGA_800x600_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L73250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H73250KHZ) {		// 73250kHz
				*p_vf = SYSTEM_VFORM_PC_SVGA_800x600_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_XGA && detvfm == HDMIRX_VFMDET_00_DETVFM_XGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L65000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H65000KHZ) {				// 65000kHz
				*p_vf = SYSTEM_VFORM_PC_XGA_1024x768_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L75000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H75000KHZ) {		// 75000kHz
				*p_vf = SYSTEM_VFORM_PC_XGA_1024x768_70;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L78750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H78750KHZ) {		// 78750kHz
				*p_vf = SYSTEM_VFORM_PC_XGA_1024x768_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L94500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H94500KHZ) {		// 94500kHz
				*p_vf = SYSTEM_VFORM_PC_XGA_1024x768_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L115500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H115500KHZ) {	// 115500kHz
				*p_vf = SYSTEM_VFORM_PC_XGA_1024x768_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_WXGA && detvfm == HDMIRX_VFMDET_00_DETVFM_WXGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L68250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H68250KHZ) {				// 68250kHz
				*p_vf = SYSTEM_VFORM_PC_WXGA_1280x768_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L79500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H79500KHZ) {		// 79500kHz
				*p_vf = SYSTEM_VFORM_PC_WXGA_1280x768_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L102250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H102250KHZ) {	// 102250kHz
				*p_vf = SYSTEM_VFORM_PC_WXGA_1280x768_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L117500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H117500KHZ) {	// 117500kHz
				*p_vf = SYSTEM_VFORM_PC_WXGA_1280x768_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L140250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H140250KHZ) {	// 140250kHz
				*p_vf = SYSTEM_VFORM_PC_WXGA_1280x768_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_WSXGA && detvfm == HDMIRX_VFMDET_00_DETVFM_WSXGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L71000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H71000KHZ) {				// 71000kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGA_1280x800_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L83500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H83500KHZ) {		// 83500kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGA_1280x800_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L106500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H106500KHZ) {	// 106500kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGA_1280x800_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L122500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H122500KHZ) {	// 122500kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGA_1280x800_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L146250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H146250KHZ) {	// 146250kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGA_1280x800_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_WXGA_P && detvfm == HDMIRX_VFMDET_00_DETVFM_WXGA_P) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L88750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H88750KHZ) {				// 88750kHz
				*p_vf = SYSTEM_VFORM_PC_WXGAp_1440x900_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L106500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H106500KHZ) {	// 106500kHz
				*p_vf = SYSTEM_VFORM_PC_WXGAp_1440x900_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L136750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H136750KHZ) {	// 136750kHz
				*p_vf = SYSTEM_VFORM_PC_WXGAp_1440x900_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L157000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H157000KHZ) {	// 157000kHz
				*p_vf = SYSTEM_VFORM_PC_WXGAp_1440x900_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L182750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H182750KHZ) {	// 182750kHz
				*p_vf = SYSTEM_VFORM_PC_WXGAp_1440x900_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_QVGA && detvfm == HDMIRX_VFMDET_00_DETVFM_QVGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L108000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H108000KHZ) {			// 108000kHz
				*p_vf = SYSTEM_VFORM_PC_QVGA_1280x960_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L148500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H148500KHZ) {	// 148500kHz
				*p_vf = SYSTEM_VFORM_PC_QVGA_1280x960_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L175500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H175500KHZ) {	// 175500kHz
				*p_vf = SYSTEM_VFORM_PC_QVGA_1280x960_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_SXGA && detvfm == HDMIRX_VFMDET_00_DETVFM_SXGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L108000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H108000KHZ) {			// 108000kHz
				*p_vf = SYSTEM_VFORM_PC_SXGA_1280x1024_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L135000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H135000KHZ) {	// 135000kHz
				*p_vf = SYSTEM_VFORM_PC_SXGA_1280x1024_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L157500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H157500KHZ) {	// 157500kHz
				*p_vf = SYSTEM_VFORM_PC_SXGA_1280x1024_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L187250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H187250KHZ) {	// 187250kHz
				*p_vf = SYSTEM_VFORM_PC_SXGA_1280x1024_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_SXGA_P && detvfm == HDMIRX_VFMDET_00_DETVFM_SXGA_P) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L101000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H101000KHZ) {			// 101000kHz
				*p_vf = SYSTEM_VFORM_PC_SXGAp_1400x1050_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L121750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H121750KHZ) {	// 121750kHz
				*p_vf = SYSTEM_VFORM_PC_SXGAp_1400x1050_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L156000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H156000KHZ) {	// 156000kHz
				*p_vf = SYSTEM_VFORM_PC_SXGAp_1400x1050_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L179500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H179500KHZ) {	// 179500kHz
				*p_vf = SYSTEM_VFORM_PC_SXGAp_1400x1050_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L208000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H208000KHZ) {	// 208000kHz
				*p_vf = SYSTEM_VFORM_PC_SXGAp_1400x1050_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_WSXGA_P && detvfm == HDMIRX_VFMDET_00_DETVFM_WSXGA_P) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L119000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H119000KHZ) {			// 119000kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGAp_1680x1050_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L146250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H146250KHZ) {	// 146250kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGAp_1680x1050_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L187000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H187000KHZ) {	// 187000kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGAp_1680x1050_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L214750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H214750KHZ) {	// 214750kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGAp_1680x1050_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L245500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H245500KHZ) {	// 245500kHz
				*p_vf = SYSTEM_VFORM_PC_WSXGAp_1680x1050_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_UXGA && detvfm == HDMIRX_VFMDET_00_DETVFM_UXGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L162000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H162000KHZ) {			// 162000kHz
				*p_vf = SYSTEM_VFORM_PC_UXGA_1600x1200_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L175500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H175500KHZ) {	// 175500kHz
				*p_vf = SYSTEM_VFORM_PC_UXGA_1600x1200_65;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L189000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H189000KHZ) {	// 189000kHz
				*p_vf = SYSTEM_VFORM_PC_UXGA_1600x1200_70;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L202500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H202500KHZ) {	// 202500kHz
				*p_vf = SYSTEM_VFORM_PC_UXGA_1600x1200_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L229500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H229500KHZ) {	// 229500kHz
				*p_vf = SYSTEM_VFORM_PC_UXGA_1600x1200_85;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L268250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H268250KHZ) {	// 268250kHz
				*p_vf = SYSTEM_VFORM_PC_UXGA_1600x1200_120_RB;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_WUXGA && detvfm == HDMIRX_VFMDET_00_DETVFM_WUXGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L154000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H154000KHZ) {			// 154000kHz
				*p_vf = SYSTEM_VFORM_PC_WUXGA_1920x1200_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L193250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H193250KHZ) {	// 193250kHz
				*p_vf = SYSTEM_VFORM_PC_WUXGA_1920x1200_60;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L245250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H245250KHZ) {	// 245250kHz
				*p_vf = SYSTEM_VFORM_PC_WUXGA_1920x1200_75;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L281250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H281250KHZ) {	// 281250kHz
				*p_vf = SYSTEM_VFORM_PC_WUXGA_1920x1200_85;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_QXGA && detvfm == HDMIRX_VFMDET_00_DETVFM_QXGA) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L265000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H265000KHZ) {			// 265000kHz
				*p_vf = SYSTEM_VFORM_PC_QXGA_2048x1536_60;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_640X400 && detvfm == HDMIRX_VFMDET_00_DETVFM_640X400) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L31500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H31500KHZ) {				// 31500kHz
				*p_vf = SYSTEM_VFORM_PC_640x400_85;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_720X400 && detvfm == HDMIRX_VFMDET_00_DETVFM_720X400) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L35500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H35500KHZ) {				// 35500kHz
				*p_vf = SYSTEM_VFORM_PC_720x400_85;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_848X480 && detvfm == HDMIRX_VFMDET_00_DETVFM_848X480) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L33750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H33750KHZ) {				// 33750kHz
				*p_vf = SYSTEM_VFORM_PC_848x480_60;
			}
		} else if (pcfdmt == HDMIRX_VFMDET_01_PCFDMT_1360X768 && detvfm == HDMIRX_VFMDET_00_DETVFM_1360X768) {
			if (pcf > HDMIRX_PIXEL_CLOCK_L72000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H72000KHZ) {				// 72000kHz
				*p_vf = SYSTEM_VFORM_PC_FWXGA_1366x768_60_RB;
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L85500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H85500KHZ) {		// 85500kHz
				// Horizontal Data Enable Period
				hde = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHDE_12_8_MASK)
						<< HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX_BIT)
						| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHDE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHDE_7_0_MASK);
				// Vertical Data Enable Period
				vde = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRVDE_11_8_IDX] & HDMIRX_CRRHVPERIOD_CRRVDE_11_8_MASK)
						<< HDMIRX_CRRHVPERIOD_CRRVDE_11_8_IDX_BIT)
						| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRVDE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRVDE_7_0_MASK);
				// Horizontal Period
				hpe = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHPE_12_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHPE_12_8_MASK)
						<< HDMIRX_CRRHVPERIOD_CRRHPE_12_8_IDX_BIT)
						| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHPE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHPE_7_0_MASK);
				// Hsync Width
				hsw = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHSW_8_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHSW_8_8_MASK)
						<< HDMIRX_CRRHVPERIOD_CRRHSW_8_8_IDX_BIT)
						| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHSW_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHSW_7_0_MASK);
				if (hde == HDMIRX_HDE_1360 && vde == HDMIRX_VDE_768) {
					if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_1792))
						&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_1792))
						&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_112))
						&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_112))) {
						*p_vf = SYSTEM_VFORM_PC_1360x768_60;
					}
				} else if (hde == HDMIRX_HDE_1366 && vde == HDMIRX_VDE_768) {
					if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_1792))
						&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_1792))
						&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_143))
						&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_143))) {
						*p_vf = SYSTEM_VFORM_PC_FWXGA_1366x768_60;
					}
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L148250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H148250KHZ) {	// 148250kHz
				*p_vf = SYSTEM_VFORM_PC_1360x768_120_RB;
			}
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_unknown_format(hdmirx_video *p_video, uint8_t *p_vform,
										uint8_t *p_curr_hv, system_video_format *p_vf)
{
	uint8_t detvfm, pcfdmt;
	uint32_t pcf;
	uint32_t hpe, hsw, hde, vde;
	uint32_t ret = ERR_OK;

	if (p_vform == NULL || p_curr_hv == NULL || p_vf == NULL) {
		return ERR_NULL_POINTER;
	}

	detvfm = p_vform[HDMIRX_DETVFM_INDEX];
	pcfdmt = p_vform[HDMIRX_PCFDMT_INDEX];

	if (detvfm == HDMIRX_VFMDET_00_DETVFM_UNKNOWN && pcfdmt == HDMIRX_VFMDET_00_DETVFM_UNKNOWN) {
		// calculate pixel clock
		ret = hdmirx_get_calc_pixel_clock_freq(p_video->rx_port, HDMIRX_PCKMON_UNSPECIFIED, &pcf);
		CHECK_ERROR(ret);
		if (pcf == HDMIRX_PIXEL_CLOCK_0KHZ || pcf > HDMIRX_PIXEL_CLOCK_620000KHZ) {
			// TMDS clock counter is zero
			// TMDS clock counter is over 620MHz
			return ERR_OK;
		}
		// Horizontal Data Enable Period
		hde = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHDE_12_8_MASK)
				<< HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX_BIT)
				| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHDE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHDE_7_0_MASK);
		// Vertical Data Enable Period
		vde = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRVDE_11_8_IDX] & HDMIRX_CRRHVPERIOD_CRRVDE_11_8_MASK)
				<< HDMIRX_CRRHVPERIOD_CRRVDE_11_8_IDX_BIT)
				| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRVDE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRVDE_7_0_MASK);
		// Horizontal Period
		hpe = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHPE_12_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHPE_12_8_MASK)
				<< HDMIRX_CRRHVPERIOD_CRRHPE_12_8_IDX_BIT)
				| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHPE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHPE_7_0_MASK);
		// Hsync Width
		hsw = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHSW_8_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHSW_8_8_MASK)
				<< HDMIRX_CRRHVPERIOD_CRRHSW_8_8_IDX_BIT)
				| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHSW_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHSW_7_0_MASK);
		if (hde == HDMIRX_HDE_1152 && vde == HDMIRX_VDE_864) {
			// 1152x864
			if (pcf > HDMIRX_PIXEL_CLOCK_L108000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H108000KHZ) {			// 108000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_1600))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_1600))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_128))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_128))) {
					*p_vf = SYSTEM_VFORM_PC_XGAp_1152x864_75;
				}
			}
		} else if (hde == HDMIRX_HDE_1792 && vde == HDMIRX_VDE_1344) {
			// 1792x1344
			if (pcf > HDMIRX_PIXEL_CLOCK_L204750KHZ && pcf < HDMIRX_PIXEL_CLOCK_H204750KHZ) {			// 204750kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2448))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2448))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_200))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_200))) {
					*p_vf = SYSTEM_VFORM_PC_1792x1344_60;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L261000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H261000KHZ) {	// 261000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2456))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2456))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_216))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_216))) {
					*p_vf = SYSTEM_VFORM_PC_1792x1344_75;
				}
			}
		} else if (hde == HDMIRX_HDE_1856 && vde == HDMIRX_VDE_1392) {
			// 1856x1392
			if (pcf > HDMIRX_PIXEL_CLOCK_L218250KHZ && pcf < HDMIRX_PIXEL_CLOCK_H218250KHZ) {			// 218250kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2528))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2528))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_224))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_224))) {
					*p_vf = SYSTEM_VFORM_PC_1856x1392_60;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L288000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H288000KHZ) {	// 288000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2560))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2560))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_224))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_224))) {
					*p_vf = SYSTEM_VFORM_PC_1856x1392_75;
				}
			}
		} else if (hde == HDMIRX_HDE_1920 && vde == HDMIRX_VDE_1440) {
			// 1920x1440
			if (pcf > HDMIRX_PIXEL_CLOCK_L234000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H234000KHZ) {			// 234000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2600))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2600))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_208))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_208))) {
					*p_vf = SYSTEM_VFORM_PC_1920x1440_60;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L297000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H297000KHZ) {	// 297000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2640))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2640))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_224))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_224))) {
					*p_vf = SYSTEM_VFORM_PC_1920x1440_75;
				}
			}
		} else if (hde == HDMIRX_HDE_2560 && vde == HDMIRX_VDE_1600) {
			// 2650x1600
			if (pcf > HDMIRX_PIXEL_CLOCK_L268500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H268500KHZ) {			// 268500kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2720))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2720))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_32))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_32))) {
					*p_vf = SYSTEM_VFORM_PC_WQXGA_2560x1600_60_RB;
				}
			}
		} else if (hde == HDMIRX_HDE_2560 && vde == HDMIRX_VDE_1440) {
			// 2560x1440
			if (pcf > HDMIRX_PIXEL_CLOCK_L241500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H241500KHZ) {			// 241500kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2720))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2720))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_32))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_32))) {
					*p_vf = SYSTEM_VFORM_PC_WQHD_2560x1440_60_RB;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L297000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H297000KHZ) {	// 297000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_3300))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_3300))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_80))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_80))) {
					*p_vf = SYSTEM_VFORM_PC_WQHD_2560x1440_60;
				}
			}
		} else if (hde == HDMIRX_HDE_1920 && vde == HDMIRX_VDE_2160) {
			// 1920x2160
			if (pcf > HDMIRX_PIXEL_CLOCK_L297000KHZ && pcf < HDMIRX_PIXEL_CLOCK_H297000KHZ) {			// 297000kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2200))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2200))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_44))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_44))) {
					*p_vf = SYSTEM_VFORM_PC_1920x2160_60;
				}
			}
		} else if (hde == HDMIRX_HDE_1920 && vde == HDMIRX_VDE_1080) {
			// 1920x1080
			if (pcf > HDMIRX_PIXEL_CLOCK_L285500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H285500KHZ) {			// 285500kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2080))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2080))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_32))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_32))) {
					*p_vf = SYSTEM_VFORM_PC_FHD_1920x1080_120_1;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L346500KHZ && pcf < HDMIRX_PIXEL_CLOCK_H346500KHZ) {	// 346500kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2080))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2080))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_32))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_32))) {
					*p_vf = SYSTEM_VFORM_PC_FHD_1920x1080_144_1;
				}
			} else if (pcf > HDMIRX_PIXEL_CLOCK_L556800KHZ && pcf < HDMIRX_PIXEL_CLOCK_H556800KHZ) {	// 556800kHz
				if ((hpe >= LOWER_LIMIT01(HDMIRX_HPE_2000))
					&& (hpe <= UPPER_LIMIT01(HDMIRX_HPE_2000))
					&& (hsw >= LOWER_LIMIT01(HDMIRX_HSW_32))
					&& (hsw <= UPPER_LIMIT01(HDMIRX_HSW_32))) {
					*p_vf = SYSTEM_VFORM_PC_FHD_1920x1080_240_1;
				}
			}
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_vformat0(hdmirx_video *p_video, uint8_t pckmon)
{
	system_deep_color dc = SYSTEM_DEEP_COLOR_8BIT;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	// set deep color
	switch (pckmon) {
		case HDMIRX_PIXEL_CLOCK_24BIT:	dc = SYSTEM_DEEP_COLOR_8BIT;	break;	// 8 bit
		case HDMIRX_PIXEL_CLOCK_30BIT:	dc = SYSTEM_DEEP_COLOR_10BIT;	break;	// 10 bit
		case HDMIRX_PIXEL_CLOCK_36BIT:	dc = SYSTEM_DEEP_COLOR_12BIT;	break;	// 12 bit
		case HDMIRX_PIXEL_CLOCK_48BIT:	dc = SYSTEM_DEEP_COLOR_MAX;		break;	// 16 bit (none)
		default:						return ERR_HDMIRX_DEEP_COLOR;
	}
	p_video->rx_deep_color = dc;

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_vformat1(hdmirx_video *p_video, uint8_t pckmon, uint8_t *p_vform,
									uint8_t *p_hv, uint8_t *p_curr_hv)
{
	system_video_format vf = SYSTEM_VFORM_UNUSED;
	system_color_format cf = SYSTEM_COLOR_RGB_FULL_RANGE1;
	uint8_t detvfm, pcfdmt;
	uint32_t crrhde;
	uint32_t ret;

	if (p_video == NULL || p_vform == NULL || p_hv == NULL || p_curr_hv == NULL) {
		return ERR_NULL_POINTER;
	}

	detvfm = p_vform[HDMIRX_DETVFM_INDEX];
	pcfdmt = p_vform[HDMIRX_PCFDMT_INDEX];
	// Horizontal Data Enable Period
	crrhde = ((p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX] & HDMIRX_CRRHVPERIOD_CRRHDE_12_8_MASK)
				<< HDMIRX_CRRHVPERIOD_CRRHDE_12_8_IDX_BIT)
				| (p_curr_hv[HDMIRX_CRRHVPERIOD_CRRHDE_7_0_IDX] & HDMIRX_CRRHVPERIOD_CRRHDE_7_0_MASK);

	// set video format (excepting PC Format and 3D Format)
	if ((detvfm >= SYSTEM_VFORM_640x480P_5994or60_4v3)
		&& detvfm <= (SYSTEM_VFORM_1920x1080P_100_16v9)) {
		vf = (system_video_format)detvfm;										// VIC: 1-64
	} else {
		if (detvfm == HDMIRX_VFMDET_00_DETVFM_3840X2160P_24HZ) {				// VIC: 93
			vf = SYSTEM_VFORM_3840x2160P_2398or24_16v9;
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_3840X2160P_25_50HZ) {		// VIC: 94 or 96
			vf = SYSTEM_VFORM_3840x2160P_50_16v9;								// tentative setting
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_3840X2160P_30_60HZ) {		// VIC: 95 or 97
			vf = SYSTEM_VFORM_3840x2160P_5994or60_16v9;							// tentative setting
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_4096X2160P_24HZ) {			// VIC: 98
			vf= SYSTEM_VFORM_4096x2160P_2398or24_256v135;
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_4096X2160P_25_50HZ) {		// VIC: 99 or 101
			vf = SYSTEM_VFORM_4096x2160P_50_256v135;							// tentative setting
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_4096X2160P_30_60HZ) {		// VIC: 100 or 102
			vf = SYSTEM_VFORM_4096x2160P_5994or60_256v135;						// tentative setting
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_FHD_3D_UHD_420_60HZ) {		// FHD 3D/UHD 420 60Hz
			// when UHD 60Hz, it is YCbCr420 (BT.601 or BT.709)
			if (crrhde == HDMIRX_CRRHVPERIOD_CRRHDE_1920) {						// VIC: 97
				vf = SYSTEM_VFORM_3840x2160P_5994or60_16v9;
				p_video->rx_color_format = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;	// tentative setting
			} else if (crrhde == HDMIRX_CRRHVPERIOD_CRRHDE_2048) {				// VIC: 102
				vf = SYSTEM_VFORM_4096x2160P_5994or60_256v135;
				p_video->rx_color_format = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;	// tentative setting
			}
		} else if (detvfm == HDMIRX_VFMDET_00_DETVFM_FHD_3D_UHD_420_50HZ) {		// FHD 3D/UHD 420 50Hz
			// when UHD 50Hz, it is YCbCr420 (BT.601 or BT.709)
			if (crrhde == HDMIRX_CRRHVPERIOD_CRRHDE_1920) {						// VIC: 96
				vf = SYSTEM_VFORM_3840x2160P_50_16v9;
				p_video->rx_color_format = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;	// tentative setting
			} else if (crrhde == HDMIRX_CRRHVPERIOD_CRRHDE_2048) {				// VIC: 101
				vf = SYSTEM_VFORM_4096x2160P_50_256v135;
				p_video->rx_color_format = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;	// tentative setting
			}
		} else if (pcfdmt) {													// PC format
			ret = hdmirx_get_pc_format(p_video, p_vform, p_hv, p_curr_hv, &vf);
			CHECK_ERROR(ret);
		} else {																// unknown format
			ret = hdmirx_get_unknown_format(p_video, p_vform, p_curr_hv, &vf);
			CHECK_ERROR(ret);
		}
	}

	p_video->rx_video_format = vf;
	p_video->rx_color_format = cf;
	// set deep color
	ret =hdmirx_get_vformat0(p_video, pckmon);
	CHECK_ERROR(ret);

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_sd_hd_uhd(system_video_format vf, hdmirx_video_definition *p_vd)
{
	if (p_vd == NULL) {
		return ERR_NULL_POINTER;
	}

	switch (vf) {
		case SYSTEM_VFORM_640x480P_5994or60_4v3:
		case SYSTEM_VFORM_720x480P_5994or60_4v3:
		case SYSTEM_VFORM_720x480P_5994or60_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1280x720P_5994or60_16v9:
		case SYSTEM_VFORM_1920x1080I_5994or60_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_720x480I_5994or60_4v3:
		case SYSTEM_VFORM_720x480I_5994or60_16v9:
		case SYSTEM_VFORM_720x240P_5994or60_4v3:
		case SYSTEM_VFORM_720x240P_5994or60_16v9:
		case SYSTEM_VFORM_2880x480I_5994or60_4v3:
		case SYSTEM_VFORM_2880x480I_5994or60_16v9:
		case SYSTEM_VFORM_2880x240P_5994or60_4v3:
		case SYSTEM_VFORM_2880x240P_5994or60_16v9:
		case SYSTEM_VFORM_1440x480P_5994or60_4v3:
		case SYSTEM_VFORM_1440x480P_5994or60_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1920x1080P_5994or60_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_720x576P_50_4v3:
		case SYSTEM_VFORM_720x576P_50_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1280x720P_50_16v9:
		case SYSTEM_VFORM_1920x1080I_50_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_720x576I_50_4v3:
		case SYSTEM_VFORM_720x576I_50_16v9:
		case SYSTEM_VFORM_720x288P_50_4v3:
		case SYSTEM_VFORM_720x288P_50_16v9:
		case SYSTEM_VFORM_2880x576I_50_4v3:
		case SYSTEM_VFORM_2880x576I_50_16v9:
		case SYSTEM_VFORM_2880x288P_50_4v3:
		case SYSTEM_VFORM_2880x288P_50_16v9:
		case SYSTEM_VFORM_1440x576P_50_4v3:
		case SYSTEM_VFORM_1440x576P_50_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1920x1080P_50_16v9:
		case SYSTEM_VFORM_1920x1080P_2398or24_16v9:
		case SYSTEM_VFORM_1920x1080P_25_16v9:
		case SYSTEM_VFORM_1920x1080P_2997or30_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_2880x480P_5994or60_4v3:
		case SYSTEM_VFORM_2880x480P_5994or60_16v9:
		case SYSTEM_VFORM_2880x576P_50_4v3:
		case SYSTEM_VFORM_2880x576P_50_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1920x1080I_T1250_50_16v9:
		case SYSTEM_VFORM_1920x1080I_100_16v9:
		case SYSTEM_VFORM_1280x720P_100_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_720x576P_100_4v3:
		case SYSTEM_VFORM_720x576P_100_16v9:
		case SYSTEM_VFORM_720x576I_100_4v3:
		case SYSTEM_VFORM_720x576I_100_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1920x1080I_11988or120_16v9:
		case SYSTEM_VFORM_1280x720P_11988or120_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_720x480P_11988or120_4v3:
		case SYSTEM_VFORM_720x480P_11988or120_16v9:
		case SYSTEM_VFORM_720x480I_11988or120_4v3:
		case SYSTEM_VFORM_720x480I_11988or120_16v9:
		case SYSTEM_VFORM_720x576P_200_4v3:
		case SYSTEM_VFORM_720x576P_200_16v9:
		case SYSTEM_VFORM_720x576I_200_4v3:
		case SYSTEM_VFORM_720x576I_200_16v9:
		case SYSTEM_VFORM_720x480P_23976or240_4v3:
		case SYSTEM_VFORM_720x480P_23976or240_16v9:
		case SYSTEM_VFORM_720x480I_23976or240_4v3:
		case SYSTEM_VFORM_720x480I_23976or240_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_SD;
			break;
		case SYSTEM_VFORM_1280x720P_2398or24_16v9:
		case SYSTEM_VFORM_1280x720P_25_16v9:
		case SYSTEM_VFORM_1280x720P_2997or30_16v9:
		case SYSTEM_VFORM_1920x1080P_11988or120_16v9:
		case SYSTEM_VFORM_1920x1080P_100_16v9:
			*p_vd = HDMIRX_VIDEO_DEFINITION_HD;
			break;
		case SYSTEM_VFORM_3840x2160P_2398or24_16v9:
		case SYSTEM_VFORM_3840x2160P_25_16v9:
		case SYSTEM_VFORM_3840x2160P_2997or30_16v9:
		case SYSTEM_VFORM_3840x2160P_50_16v9:
		case SYSTEM_VFORM_3840x2160P_5994or60_16v9:
		case SYSTEM_VFORM_4096x2160P_2398or24_256v135:
		case SYSTEM_VFORM_4096x2160P_25_256v135:
		case SYSTEM_VFORM_4096x2160P_2997or30_256v135:
		case SYSTEM_VFORM_4096x2160P_50_256v135:
		case SYSTEM_VFORM_4096x2160P_5994or60_256v135:
			*p_vd = HDMIRX_VIDEO_DEFINITION_UHD;
			break;
		case SYSTEM_VFORM_1366x768P_5979or60_16v9:
		default:
			return ERR_HDMIRX_VIDEO_FORMAT;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_avi_video_format(system_video *p_video, uint8_t vic,
										uint8_t hdmi_vs_detect, uint8_t *p_hdmi_vs_if)
{
	system_video_format vf = SYSTEM_VFORM_UNUSED;
	uint8_t hvf, hdmi_vic;

	if (p_video == NULL || p_hdmi_vs_if == NULL) {
		return ERR_NULL_POINTER;
	}

	if (vic != SYSTEM_VFORM_UNUSED) {
		vf = (system_video_format)vic;
	} else {
		// 4K2K 24/25/30Hz, 4K2K(SMPTE) 24Hz
		if (hdmi_vs_detect) {
			// HDMI Vendor Specific InfoFrame parameters
			hvf = p_hdmi_vs_if[HDMIRX_HVS_IF_HDMI_VIDEO_FORMAT_INDEX]
					& HDMIRX_HVS_IF_HDMI_VIDEO_FORMAT_MASK;
			hdmi_vic = p_hdmi_vs_if[HDMIRX_HVS_IF_HDMI_VIC_INDEX];
			if (hvf == HDMIRX_HVS_IF_HDMI_VIDEO_FORMAT_EXT_FORMAT) {
				switch (hdmi_vic) {
					case HDMIRX_HVS_IF_HDMI_VIC_4K_24HZ:
						vf = SYSTEM_VFORM_3840x2160P_2398or24_16v9;
						break;
					case HDMIRX_HVS_IF_HDMI_VIC_4K_SMPTE_24HZ:
						vf = SYSTEM_VFORM_4096x2160P_2398or24_256v135;
						break;
					case HDMIRX_HVS_IF_HDMI_VIC_4K_25HZ:
						vf = SYSTEM_VFORM_3840x2160P_25_16v9;
						break;
					case HDMIRX_HVS_IF_HDMI_VIC_4K_30HZ:
						vf = SYSTEM_VFORM_3840x2160P_2997or30_16v9;
						break;
					default:
						return ERR_HDMIRX_VIDEO_FORMAT;
				}
			} else {
				return ERR_OK;			// p_video->rx_video_format is not set
			}
		} else {
			return ERR_OK;				// p_video->rx_video_format is not set
		}
	}

	p_video->rx_video_format = vf;

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_avi_color_format(system_video *p_video, uint8_t y, uint8_t c,
										uint8_t ec, uint8_t q)
{
	system_color_format cf = SYSTEM_COLOR_RGB_FULL_RANGE1;
	hdmirx_video_definition vd;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	switch (y) {
		case HDMIRX_AVI_IF_Y_RGB:										// RGB
			switch (c) {
				case HDMIRX_AVI_IF_C_EXTENDED:
					switch (ec) {
						case HDMIRX_AVI_IF_EC_ADOBERGB:
							cf = SYSTEM_COLOR_RGB_FULL_RANGE2;			break;	// RGB Full Range (Mode2)
						case HDMIRX_AVI_IF_EC_BT2020_RGB_YCBCR:
							cf = SYSTEM_COLOR_RGB_ITU_R_BT2020;			break;	// RGB BT.2020
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
				default:
					switch (q) {
						case HDMIRX_AVI_IF_Q_DEFAULT:
							if ((p_video->rx_video_format >= SYSTEM_VFORM_720x480P_5994or60_4v3)
								&& (p_video->rx_video_format <= SYSTEM_VFORM_4096x2160P_5994or60_256v135)) {
								cf = SYSTEM_COLOR_RGB_LIMITED_RANGE;			// RGB Limited Range
							} else {
								cf = SYSTEM_COLOR_RGB_FULL_RANGE1;				// RGB Full Range (Mode1)
							}
																		break;
						case HDMIRX_AVI_IF_Q_LIMITED_RANGE:
							cf = SYSTEM_COLOR_RGB_LIMITED_RANGE;		break;	// RGB Limited Range
						case HDMIRX_AVI_IF_Q_FULL_RANGE:
							cf = SYSTEM_COLOR_RGB_FULL_RANGE1;			break;	// RGB Full Range (Mode1)
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
			}
			break;
		case HDMIRX_AVI_IF_Y_YCBCR422:									// YCbCr4:2:2
			switch (c) {
				case HDMIRX_AVI_IF_C_BT601:
					cf = SYSTEM_COLOR_YCBCR422_ITU_R_BT601;			break;	// YCbCr4:2:2 BT.601
				case HDMIRX_AVI_IF_C_BT709:
					cf = SYSTEM_COLOR_YCBCR422_ITU_R_BT709;			break;	// YCbCr4:2:2 BT.709
				case HDMIRX_AVI_IF_C_EXTENDED:
					switch (ec) {
						case HDMIRX_AVI_IF_EC_XVYCC601:
							cf = SYSTEM_COLOR_XVYCC422_ITU_R_BT601;		break;	// xvYCC4:2:2 BT.601
						case HDMIRX_AVI_IF_EC_XVYCC709:
							cf = SYSTEM_COLOR_XVYCC422_ITU_R_BT709;		break;	// xvYCC4:2:2 BT.709
						case HDMIRX_AVI_IF_EC_BT2020_RGB_YCBCR:
							cf = SYSTEM_COLOR_YCBCR422_ITU_R_BT2020;	break;	// YCbCr4:2:2 BT.2020
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
				case HDMIRX_AVI_IF_C_NO_DATA:
				default:
					ret = hdmirx_get_sd_hd_uhd(p_video->rx_video_format, &vd);
					CHECK_ERROR(ret);
					switch (vd) {
						case HDMIRX_VIDEO_DEFINITION_SD:
							cf = SYSTEM_COLOR_YCBCR422_ITU_R_BT601;		break;	// YCbCr4:2:2 BT.601
						case HDMIRX_VIDEO_DEFINITION_HD:
						case HDMIRX_VIDEO_DEFINITION_UHD:
							cf = SYSTEM_COLOR_YCBCR422_ITU_R_BT709;		break;	// YCbCr4:2:2 BT.709
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
			}
			break;
		case HDMIRX_AVI_IF_Y_YCBCR444:									// YCbCr4:4:4
			switch (c) {
				case HDMIRX_AVI_IF_C_BT601:
					cf = SYSTEM_COLOR_YCBCR444_ITU_R_BT601;			break;	// YCbCr4:4:4 BT.601
				case HDMIRX_AVI_IF_C_BT709:
					cf = SYSTEM_COLOR_YCBCR444_ITU_R_BT709;			break;	// YCbCr4:4:4 BT.709
				case HDMIRX_AVI_IF_C_EXTENDED:
					switch (ec) {
						case HDMIRX_AVI_IF_EC_XVYCC601:
							cf = SYSTEM_COLOR_XVYCC444_ITU_R_BT601;		break;	// xvYCC4:4:4 BT.601
						case HDMIRX_AVI_IF_EC_XVYCC709:
							cf = SYSTEM_COLOR_XVYCC444_ITU_R_BT709;		break;	// xvYCC4:4:4 BT.709
						case HDMIRX_AVI_IF_EC_BT2020_RGB_YCBCR:
							cf = SYSTEM_COLOR_YCBCR444_ITU_R_BT2020;	break;	// YCbCr4:4:4 BT.2020
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
				case HDMIRX_AVI_IF_C_NO_DATA:
				default:
					ret = hdmirx_get_sd_hd_uhd(p_video->rx_video_format, &vd);
					CHECK_ERROR(ret);
					switch (vd) {
						case HDMIRX_VIDEO_DEFINITION_SD:
							cf = SYSTEM_COLOR_YCBCR444_ITU_R_BT601;		break;	// YCbCr4:4:4 BT.601
						case HDMIRX_VIDEO_DEFINITION_HD:
						case HDMIRX_VIDEO_DEFINITION_UHD:
							cf = SYSTEM_COLOR_YCBCR444_ITU_R_BT709;		break;	// YCbCr4:4:4 BT.709
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
			}
			break;
		case HDMIRX_AVI_IF_Y_YCBCR420:									// YCbCr4:2:0
			switch (c) {
				case HDMIRX_AVI_IF_C_BT601:
					cf = SYSTEM_COLOR_YCBCR420_ITU_R_BT601;			break;	// YCbCr4:2:0 BT.601
				case HDMIRX_AVI_IF_C_BT709:
					cf = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;			break;	// YCbCr4:2:0 BT.709
				case HDMIRX_AVI_IF_C_EXTENDED:
					switch (ec) {
						case HDMIRX_AVI_IF_EC_BT2020_RGB_YCBCR:
							cf = SYSTEM_COLOR_YCBCR420_ITU_R_BT2020;	break;	// YCbCr4:4:4 BT.2020
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
				case HDMIRX_AVI_IF_C_NO_DATA:
				default:
					ret = hdmirx_get_sd_hd_uhd(p_video->rx_video_format, &vd);
					CHECK_ERROR(ret);
					switch (vd) {
						case HDMIRX_VIDEO_DEFINITION_SD:
							cf = SYSTEM_COLOR_YCBCR420_ITU_R_BT601;		break;	// YCbCr4:2:0 BT.601
						case HDMIRX_VIDEO_DEFINITION_HD:
						case HDMIRX_VIDEO_DEFINITION_UHD:
							cf = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;		break;	// YCbCr4:2:0 BT.709
						default:
							return ERR_HDMIRX_COLOR_FORMAT;
					}
					break;
			}
			break;
		default:
			return ERR_HDMIRX_COLOR_FORMAT;
	}

	p_video->rx_color_format = cf;

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_vformat2(system_video *p_video, uint8_t avi_detect, uint8_t *p_avi_if,
									uint8_t hdmi_vs_detect, uint8_t *p_hdmi_vs_if)
{
	uint8_t ver, y, c, ec, q, vic;
	uint32_t ret;

	if (p_video == NULL || p_avi_if == NULL || p_hdmi_vs_if == NULL) {
		return ERR_NULL_POINTER;
	}
	if (!avi_detect) {
		return ERR_OK;		// video format by hdmirx_get_vformat1() is used
	}
	// AVI InfoFrame parameters
	ver = p_avi_if[HDMIRX_AVI_IF_VERSION_INDEX];
	y = p_avi_if[HDMIRX_AVI_IF_Y_INDEX] & HDMIRX_AVI_IF_Y_MASK;
	c = p_avi_if[HDMIRX_AVI_IF_C_INDEX] & HDMIRX_AVI_IF_C_MASK;
	ec = p_avi_if[HDMIRX_AVI_IF_EC_INDEX] & HDMIRX_AVI_IF_EC_MASK;
	q = p_avi_if[HDMIRX_AVI_IF_Q_INDEX] & HDMIRX_AVI_IF_Q_MASK;
	vic = p_avi_if[HDMIRX_AVI_IF_VIC_INDEX];

	if (!(ver == HDMIRX_AVI_IF_VERSION_2 || ver == HDMIRX_AVI_IF_VERSION_3)) {
		return ERR_OK;		// video format by hdmirx_get_vformat1() is used
	}
	// set video format
	ret = hdmirx_set_avi_video_format(p_video, vic, hdmi_vs_detect, p_hdmi_vs_if);
	CHECK_ERROR(ret);
	// set color format
	ret = hdmirx_set_avi_color_format(p_video, y, c, ec, q);
	CHECK_ERROR(ret);

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_video_format_detect(hdmirx_video *p_video, uint8_t *p_irq)
{
	system_port port;
	uint32_t i;
	uint8_t pckmon, calc_pfq, pfq;
	uint8_t vform[HDMIRX_GET_DETECTED_VFORMAT_LEN] = { 0 };
	uint8_t hv[HDMIRX_GET_HORI_VERT_PERIOD_LEN] = { 0 };
	uint8_t curr_hv[HDMIRX_GET_CURRENT_HORI_VERT_PERIOD_LEN] = { 0 };
	uint32_t ret;

	if (p_video == NULL || p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;

	for (i=0; i<HDMIRX_VIDEO_FORMAT_DETECT_RETRY_MAX; i++) {
		CHECK_ABORT();
		// get Pixel Clock Monitor
		ret = hdmirx_get_rx_mon(port, &pckmon, HDMIRX_MONREG_00_PCKMON_MASK);
		CHECK_ERROR(ret);
		// set deep color
		ret =hdmirx_get_vformat0(p_video, pckmon);
		CHECK_ERROR(ret);
		// Check Clock Parameter
		// Calculate Pixel Clock and Parameter calc_PFQ
		ret = hdmirx_get_calc_pixel_clock_range(port, pckmon, &calc_pfq);
		CHECK_ERROR(ret);
		// get pixel clock frequency
		ret = hdmirx_get_pixel_clock_range(port, &pfq);
		CHECK_ERROR(ret);
		if (pfq == calc_pfq) {
			// Interrupt UNKNOWN ?
			if (*p_irq & HDMIRX_RINTREG1_UNKNOWN_MASK) {
				;
			} else {
				// Check Video Format ID Number
				ret = hdmirx_get_detected_vformat(port, vform);
				CHECK_ERROR(ret);
			}
			// Check Video Format Parameter
			ret = hdmirx_set_current_param_latch(port);
			CHECK_ERROR(ret);
			ret = hdmirx_get_hori_vert_period(port, hv);
			CHECK_ERROR(ret);
			ret = hdmirx_get_current_hori_vert_period(port, curr_hv);
			CHECK_ERROR(ret);
			// set HDMI Rx video format (1st)
			ret = hdmirx_get_vformat1(p_video, pckmon, vform, hv, curr_hv);
			CHECK_ERROR(ret);
			break;
		} else {
			// irq cleanup
			ret = hdmirx_set_rx_irq1(port, *p_irq, HDMIRX_RINTREG1_MASK);
			CHECK_ERROR(ret);
			// clear interrupt mask (VFSUCS and UNKNOWN)
			ret = hdmirx_set_intr_mask_rx_1(port, HDMIRX_RINTMSK_00_VFSUCS_OFF
													| HDMIRX_RINTMSK_00_UNKNOWN_OFF,
												HDMIRX_RINTMSK_00_VFSUCS_MASK
													| HDMIRX_RINTMSK_00_UNKNOWN_MASK);
			CHECK_ERROR(ret);
			// TMDS Re-Calibration
			ret = hdmirx_re_calibrate_tmds(port);
			CHECK_ERROR(ret);
			// Deep Color Mode Re-Detection (3.6)
			ret = hdmirx_re_detect_deep_color(port);
			CHECK_ERROR(ret);
			// Reset Pixel Clock Frequency Detect
			ret = hdmirx_set_rx_soft_reset3(port, HDMIRX_RSRST3_PFQRST);
			CHECK_ERROR(ret);
			// Clear Detect Video Format
			ret = hdmirx_set_mode_ctrl1(port, HDMIRX_MODCNT1_VFMCLR_RE_DETECT, HDMIRX_MODCNT1_VFMCLR_MASK);
			CHECK_ERROR(ret);
			// set interrupt mask (VFSUCS and UNKNOWN)
			ret = hdmirx_set_intr_mask_rx_1(port, HDMIRX_RINTMSK_00_VFSUCS_ON
													| HDMIRX_RINTMSK_00_UNKNOWN_ON,
												HDMIRX_RINTMSK_00_VFSUCS_MASK
													| HDMIRX_RINTMSK_00_UNKNOWN_MASK);
			CHECK_ERROR(ret);
			CHECK_ABORT();
			// wait for 1ms
			SLEEP(HDMIRX_SLEEP_TIME_1MSEC);
			// Video Format Re-Detectin Start
			ret = hdmirx_set_mode_ctrl1(port, HDMIRX_MODCNT1_VFMCLR_DEF, HDMIRX_MODCNT1_VFMCLR_MASK);
			CHECK_ERROR(ret);
			CHECK_ABORT();
			// wait for 20ms
			SLEEP(HDMIRX_SLEEP_TIME_20MSEC);
			// detect video format
			ret = hdmirx_get_rx_irq1(port, p_irq, (HDMIRX_RINTREG1_UNKNOWN_MASK
													| HDMIRX_RINTREG1_VFSUCS_MASK));
			CHECK_ERROR(ret);
		}
	}
	if (i == HDMIRX_VIDEO_FORMAT_DETECT_RETRY_MAX) {
		return ERR_HDMIRX_VIDEO_FORMAT_DETECT_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_select_page_of_0x14(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SELECT_PAGE_OF_0X14_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SELECT_PAGE_OF_0X14_NUM);
	// 0x1604
	SET_COMMAND(cmd, 0, VIDEOIF_AKERX_AVMMASK_ADDR, page, HDMIRX_AVMMASK_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_hdmi_mode(system_port port, uint8_t *p_mode)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_HDMI_MODE_NUM];
	uint32_t ret;

	if (p_mode == NULL) {
		return ERR_NULL_POINTER;
	}

	// Select HDCP page: HDCP Port#
	ret = hdmirx_select_page_of_0x14(port, port.port_num);
	CHECK_ERROR(ret);

	// page 0x14: 0,1,2,3

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_HDMI_MODE_NUM);
	// 0x1442
	SET_COMMAND(cmd, 0, VIDEOIF_HDCPRX_BSTARP0_01_ADDR, 0, HDMIRX_BSTARP0_01_HDMI_MODE_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_mode = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_packet_status0(system_port port, uint8_t *p_stat, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_PACKET_STATUS0_NUM];
	uint32_t ret;

	if (p_stat == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_PACKET_STATUS0_NUM);
	// 0x26E1
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PKTDETS0_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_stat = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_packet_status1(system_port port, uint8_t *p_stat, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_PACKET_STATUS1_NUM];
	uint32_t ret;

	if (p_stat == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_PACKET_STATUS1_NUM);
	// 0x26E5
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PKTDETS1_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_stat = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_packet_status2(system_port port, uint8_t *p_stat, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_PACKET_STATUS2_NUM];
	uint32_t ret;

	if (p_stat == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_PACKET_STATUS2_NUM);
	// 0x26E9
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PKTDETS2_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_stat = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_packet_param_change0(system_port port, uint8_t *p_irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_PACKET_PARAM_CHANGE0_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_PACKET_PARAM_CHANGE0_NUM);
	// 0x26E2
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PPMCHG0_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_packet_param_change1(system_port port, uint8_t *p_irq, uint8_t mask)
{
	system_reg_access access;
	system_command cmd[HDMIRX_GET_PACKET_PARAM_CHANGE1_NUM];
	uint32_t ret;

	if (p_irq == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_GET_PACKET_PARAM_CHANGE1_NUM);
	// 0x26E6
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PPMCHG1_ADDR, 0, mask);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);

	*p_irq = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_clr_packet_param_change0(system_port port, uint8_t irq)
{
	system_reg_access access;
	system_command cmd[HDMIRX_CLR_PACKET_PARAM_CHANGE0_NUM];
	uint32_t ret;

	if (irq == 0) {
		return ERR_OK;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_CLR_PACKET_PARAM_CHANGE0_NUM);
	// 0x26E2
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PPMCHG0_ADDR, irq, HDMIRX_PPMCHG0_ALL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_clr_packet_param_change1(system_port port, uint8_t irq)
{
	system_reg_access access;
	system_command cmd[HDMIRX_CLR_PACKET_PARAM_CHANGE1_NUM];
	uint32_t ret;

	if (irq == 0) {
		return ERR_OK;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_CLR_PACKET_PARAM_CHANGE1_NUM);
	// 0x26E6
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_PPMCHG1_ADDR, irq, HDMIRX_PPMCHG1_ALL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_free_packet_header(system_port port, uint8_t h0)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_FREE_PACKET_HEADER_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_FREE_PACKET_HEADER_NUM);
	// 0x26A0
	SET_COMMAND(cmd, 0, VIDEOIF_PKT_RFREPKT_00_ADDR, h0, HDMIRX_RFREPKT_FRE00H_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_avi_infoframe(system_port port, uint8_t *p_detect, uint8_t *p_if)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_detect == NULL || p_if == NULL) {
		return ERR_NULL_POINTER;
	}

	// get AVI InfoFrame Packet Detect Status
	ret = hdmirx_get_packet_status1(port, p_detect, HDMIRX_PKTDETS1_AVIDETS_MASK);
	CHECK_ERROR(ret);
	if (!(*p_detect)) {
		return ERR_OK;		// not detected
	}
	// get AVI InfoFrame
	// 0x2820-0x2828
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_INF_RXAVIMON_00_ADDR,
						HDMIRX_GET_AVI_INFOFRAME_LEN, p_if);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_hdmi_vs_infoframe(system_port port, uint8_t *p_detect, uint8_t *p_if)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_detect == NULL || p_if == NULL) {
		return ERR_NULL_POINTER;
	}

	// get HDMI Vendor Specific InfoFrame Packet Detect Status
	ret = hdmirx_get_packet_status2(port, p_detect, HDMIRX_PKTDETS2_HVSDETS_MASK);
	CHECK_ERROR(ret);
	if (!(*p_detect)) {
		return ERR_OK;		// not detected
	}
	// page(0x68c0): 0: HDMI Vendor Specific InfoFrame
	ret = hdmirx_select_page_of_0x68c0(port, HDMIRX_RXVSICTL_00_HVSIPAGE_H_VSIF);
	CHECK_ERROR(ret);
	// get HDMI Vendor Specific InfoFrame
	// 0x28C0-0x28C8
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_INF_RXHVSIMON_00_ADDR,
						HDMIRX_GET_HDMI_VS_INFOFRAME_LEN, p_if);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_pixel_repetition(system_video_format vform, uint8_t *p_rep)
{
	if (p_rep == NULL) {
		return ERR_NULL_POINTER;
	}

	switch (vform) {
		case SYSTEM_VFORM_720x480I_5994or60_4v3:				// 6
		case SYSTEM_VFORM_720x480I_5994or60_16v9:				// 7
		case SYSTEM_VFORM_720x240P_5994or60_4v3:				// 8
		case SYSTEM_VFORM_720x240P_5994or60_16v9:				// 9
		case SYSTEM_VFORM_1440x480P_5994or60_4v3:				// 14
		case SYSTEM_VFORM_1440x480P_5994or60_16v9:				// 15
		case SYSTEM_VFORM_720x576I_50_4v3:						// 21
		case SYSTEM_VFORM_720x576I_50_16v9:						// 22
		case SYSTEM_VFORM_720x288P_50_4v3:						// 23
		case SYSTEM_VFORM_720x288P_50_16v9:						// 24
		case SYSTEM_VFORM_1440x576P_50_4v3:						// 29
		case SYSTEM_VFORM_1440x576P_50_16v9:					// 30
		case SYSTEM_VFORM_720x576I_100_4v3:						// 44
		case SYSTEM_VFORM_720x576I_100_16v9:					// 45
		case SYSTEM_VFORM_720x480I_11988or120_4v3:				// 50
		case SYSTEM_VFORM_720x480I_11988or120_16v9:				// 51
		case SYSTEM_VFORM_720x576I_200_4v3:						// 54
		case SYSTEM_VFORM_720x576I_200_16v9:					// 55
		case SYSTEM_VFORM_720x480I_23976or240_4v3:				// 58
		case SYSTEM_VFORM_720x480I_23976or240_16v9:				// 59
			// pixel repetition: 2 (NREPST, DTRATE: 0x1)
			*p_rep = HDMIRX_VOUTCNT_NREPST_REPETITION_MANUAL_2;
			break;
		case SYSTEM_VFORM_2880x480I_5994or60_4v3:				// 10
		case SYSTEM_VFORM_2880x480I_5994or60_16v9:				// 11
		case SYSTEM_VFORM_2880x240P_5994or60_4v3:				// 12
		case SYSTEM_VFORM_2880x240P_5994or60_16v9:				// 13
		case SYSTEM_VFORM_2880x576I_50_4v3:						// 25
		case SYSTEM_VFORM_2880x576I_50_16v9:					// 26
		case SYSTEM_VFORM_2880x288P_50_4v3:						// 27
		case SYSTEM_VFORM_2880x288P_50_16v9:					// 28
		case SYSTEM_VFORM_2880x480P_5994or60_4v3:				// 35
		case SYSTEM_VFORM_2880x480P_5994or60_16v9:				// 36
		case SYSTEM_VFORM_2880x576P_50_4v3:						// 37
		case SYSTEM_VFORM_2880x576P_50_16v9:					// 38
			// pixel repetition: 4 (NREPST, DTRATE: 0x2)
			*p_rep = HDMIRX_VOUTCNT_NREPST_REPETITION_MANUAL_4;
			break;
		default:
			// pixel repetition: 0 (NREPST, DTRATE: 0x3)
			*p_rep = HDMIRX_VOUTCNT_NREPST_REPETITION_AUTO;
			break;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_pixel_repetition(system_port port, uint8_t rep)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_PIXEL_REPETITION_LEN];
	uint32_t ret;

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_VIDEO_VOUTCNT_ADDR,
						HDMIRX_SET_PIXEL_REPETITION_LEN, data);

	// NREPST: Receive Repetition Setting
	SET_BLOCK_DATA(data, 0, rep);									// 0x2238
	// DTRETE: Repetition Output Setting
	SET_BLOCK_DATA(data, 1, rep);									// 0x2239

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rxpll(system_port port, uint8_t tmds_freq, system_color_format cf, system_deep_color dc)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_RXPLL_LEN];
	uint8_t rxpll0en, rxpll00en;
	uint32_t ret;

	// NOTE: when no color space conversion from 420 10/12bit,
	//       RXPLL0EN and RXPLL00EN must be set auto mode.
	//       it is performed by sysctrl_set_clock_mode_hdmirx_420_10_12bit()
	if (is_color_format_yuv420(cf)) {
		if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHC_FDTTHD) {			// 4: 85.5MHz to 123MHz
			if ((dc == SYSTEM_DEEP_COLOR_10BIT)						// 1: 10bit
				|| (dc == SYSTEM_DEEP_COLOR_12BIT)) {				// 2: 12bit
				rxpll0en = HDMIRX_RXPLL0_01_RXPLL0EN_MANUAL;
				rxpll00en = HDMIRX_RXPLL00_03_RXPLL00EN_MANUAL;
			} else {
				rxpll0en = HDMIRX_RXPLL0_01_RXPLL0EN_AUTO;
				rxpll00en = HDMIRX_RXPLL00_03_RXPLL00EN_AUTO;
			}
		} else if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHD_FDTTHE) {	// 5: 123MHz to 175MHz
			if (dc == SYSTEM_DEEP_COLOR_MAX) {						// 3: 16bit (none)
				rxpll0en = HDMIRX_RXPLL0_01_RXPLL0EN_MANUAL;
				rxpll00en = HDMIRX_RXPLL00_03_RXPLL00EN_MANUAL;
			} else {
				rxpll0en = HDMIRX_RXPLL0_01_RXPLL0EN_AUTO;
				rxpll00en = HDMIRX_RXPLL00_03_RXPLL00EN_AUTO;
			}
		} else {
			rxpll0en = HDMIRX_RXPLL0_01_RXPLL0EN_AUTO;
			rxpll00en = HDMIRX_RXPLL00_03_RXPLL00EN_AUTO;
		}
	} else {
		rxpll0en = HDMIRX_RXPLL0_01_RXPLL0EN_AUTO;
		rxpll00en = HDMIRX_RXPLL00_03_RXPLL00EN_AUTO;
	}
	// 0x1E41: RXPLL0EN: manual or auto
	ret = hdmirx_set_rxpll0en(port, rxpll0en);
	CHECK_ERROR(ret);
	// 0x1E45: RXPLL00EN: manual or auto
	ret = hdmirx_set_rxpll00en(port, rxpll00en);
	CHECK_ERROR(ret);
	// set specified values when manual
	if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHC_FDTTHD) {				// 4: 85.5MHz to 123MHz
		if ((dc == SYSTEM_DEEP_COLOR_10BIT)							// 1: 10bit
			|| (dc == SYSTEM_DEEP_COLOR_12BIT)) {					// 2: 12bit
			SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXPLL0_00_ADDR,
								HDMIRX_SET_RXPLL_LEN, data);
			SET_BLOCK_DATA(data, 0, HDMIRX_RXPLL0_0_MODE1);			// 0x1E40
			SET_BLOCK_DATA(data, 1, HDMIRX_RXPLL0_1_MODE1);			// 0x1E41
			SET_BLOCK_DATA(data, 2, HDMIRX_RXPLL00_0_MODE1);		// 0x1E42
			SET_BLOCK_DATA(data, 3, HDMIRX_RXPLL00_1_MODE1);		// 0x1E43
			SET_BLOCK_DATA(data, 4, HDMIRX_RXPLL00_2_MODE1);		// 0x1E44
			SET_BLOCK_DATA(data, 5, HDMIRX_RXPLL00_3_MODE1);		// 0x1E45: manual
			ret = device_block_write(&baccess);
			CHECK_ERROR(ret);
		}
	} else if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHD_FDTTHE) {		// 5: 123MHz to 175MHz
		if (dc == SYSTEM_DEEP_COLOR_MAX) {							// 3: 16bit (none)
			SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXPLL0_00_ADDR,
								HDMIRX_SET_RXPLL_LEN, data);
			SET_BLOCK_DATA(data, 0, HDMIRX_RXPLL0_0_MODE3);			// 0x1E40
			SET_BLOCK_DATA(data, 1, HDMIRX_RXPLL0_1_MODE3);			// 0x1E41
			SET_BLOCK_DATA(data, 2, HDMIRX_RXPLL00_0_MODE3);		// 0x1E42
			SET_BLOCK_DATA(data, 3, HDMIRX_RXPLL00_1_MODE3);		// 0x1E43
			SET_BLOCK_DATA(data, 4, HDMIRX_RXPLL00_2_MODE3);		// 0x1E44
			SET_BLOCK_DATA(data, 5, HDMIRX_RXPLL00_3_MODE3);		// 0x1E45: manual
			ret = device_block_write(&baccess);
			CHECK_ERROR(ret);
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rxcore(system_port port, uint8_t tmds_freq, system_color_format cf, system_deep_color dc)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_SET_RXCORE_LEN];
	uint8_t rx0_sen;
	uint32_t ret;

	// NOTE: when no color space conversion from 420 10/12bit,
	//       RX0_SEN must be set auto mode.
	//       it is performed by sysctrl_set_clock_mode_hdmirx_420_10_12bit()
	if (is_color_format_yuv420(cf)) {
		if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHC_FDTTHD) {			// 4: 85.5MHz to 123MHz
			if (dc == SYSTEM_DEEP_COLOR_10BIT) {					// 1: 10bit
				rx0_sen = HDMIRX_CKSELEN_RX0_SEN_MANUAL;
			} else if (dc == SYSTEM_DEEP_COLOR_12BIT) {				// 2: 12bit
				rx0_sen = HDMIRX_CKSELEN_RX0_SEN_MANUAL;
			} else {
				rx0_sen = HDMIRX_CKSELEN_RX0_SEN_AUTO;
			}
		} else if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHD_FDTTHE) {	// 5: 123MHz to 175MHz
			if (dc == SYSTEM_DEEP_COLOR_MAX) {						// 3: 16bit (none)
				rx0_sen = HDMIRX_CKSELEN_RX0_SEN_MANUAL;
			} else {
				rx0_sen = HDMIRX_CKSELEN_RX0_SEN_AUTO;
			}
		} else {
			rx0_sen = HDMIRX_CKSELEN_RX0_SEN_AUTO;
		}
	} else {
		rx0_sen = HDMIRX_CKSELEN_RX0_SEN_AUTO;
	}
	// 0x1E7F: RX0_SEN: manual or auto
	ret = hdmirx_set_rx0_sen(port, rx0_sen);
	CHECK_ERROR(ret);
	// set specified values
	if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHC_FDTTHD) {				// 4: 85.5MHz to 123MHz
		if (dc == SYSTEM_DEEP_COLOR_10BIT) {						// 1: 10bit
			SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_00_ADDR,
								HDMIRX_SET_RXCORE_LEN, data);
			SET_BLOCK_DATA(data, 0, HDMIRX_RXCORE0_0_MODE1
										+ port.port_num);			// 0x1E65
			SET_BLOCK_DATA(data, 1, HDMIRX_RXCORE0_1_MODE1);		// 0x1E66
			SET_BLOCK_DATA(data, 2, HDMIRX_RXCORE0_2_MODE1);		// 0x1E67
			SET_BLOCK_DATA(data, 3, HDMIRX_RXCORE0_3_MODE1);		// 0x1E68
			ret = device_block_write(&baccess);
			CHECK_ERROR(ret);
		} else if (dc == SYSTEM_DEEP_COLOR_12BIT) {					// 2: 12bit
			SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_00_ADDR,
								HDMIRX_SET_RXCORE_LEN, data);
			SET_BLOCK_DATA(data, 0, HDMIRX_RXCORE0_0_MODE2
										+ port.port_num);			// 0x1E65
			SET_BLOCK_DATA(data, 1, HDMIRX_RXCORE0_1_MODE2);		// 0x1E66
			SET_BLOCK_DATA(data, 2, HDMIRX_RXCORE0_2_MODE2);		// 0x1E67
			SET_BLOCK_DATA(data, 3, HDMIRX_RXCORE0_3_MODE2);		// 0x1E68
			ret = device_block_write(&baccess);
			CHECK_ERROR(ret);
		}
	} else if (tmds_freq == HDMIRX_PXTCLKFQ_FDTTHD_FDTTHE) {		// 5: 123MHz to 175MHz
		if (dc == SYSTEM_DEEP_COLOR_MAX) {							// 3: 16bit (none)
			SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_CKGEN_RXCORE0_00_ADDR,
								HDMIRX_SET_RXCORE_LEN, data);
			SET_BLOCK_DATA(data, 0, HDMIRX_RXCORE0_0_MODE3
										+ port.port_num);			// 0x1E65
			SET_BLOCK_DATA(data, 1, HDMIRX_RXCORE0_1_MODE3);		// 0x1E66
			SET_BLOCK_DATA(data, 2, HDMIRX_RXCORE0_2_MODE3);		// 0x1E67
			SET_BLOCK_DATA(data, 3, HDMIRX_RXCORE0_3_MODE3);		// 0x1E68
			ret = device_block_write(&baccess);
			CHECK_ERROR(ret);
		}
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_clock_param(system_video *p_video)
{
	system_port port;
	uint8_t tmds_freq;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;

	// get TMDS clock frequency (Rx Port0,1,2,3)
	ret = hdmirx_get_tmds_clock_freq_of_portno(port, port.port_num, &tmds_freq);
	CHECK_ERROR(ret);
	// set RXPLL
	ret = hdmirx_set_rxpll(port, tmds_freq, p_video->rx_color_format, p_video->rx_deep_color);
	CHECK_ERROR(ret);
	// set RXCORE
	ret = hdmirx_set_rxcore(port, tmds_freq, p_video->rx_color_format, p_video->rx_deep_color);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_dfmt(system_port port, system_video_format vf)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_DFMT_NUM];
	uint8_t dfmt;
	uint32_t ret;

	switch (vf) {
		case SYSTEM_VFORM_3840x2160P_2398or24_16v9:
			dfmt = HDMIRX_SWMODE4_02_DFMT_3840X2160P_24HZ;		break;
		case SYSTEM_VFORM_3840x2160P_25_16v9:
			dfmt = HDMIRX_SWMODE4_02_DFMT_3840X2160P_25HZ;		break;
		case SYSTEM_VFORM_3840x2160P_2997or30_16v9:
			dfmt = HDMIRX_SWMODE4_02_DFMT_3840X2160P_30HZ;		break;
		case SYSTEM_VFORM_3840x2160P_50_16v9:
			dfmt = HDMIRX_SWMODE4_02_DFMT_3840X2160P_50HZ;		break;
		case SYSTEM_VFORM_3840x2160P_5994or60_16v9:
			dfmt = HDMIRX_SWMODE4_02_DFMT_3840X2160P_60HZ;		break;
		case SYSTEM_VFORM_4096x2160P_2398or24_256v135:
			dfmt = HDMIRX_SWMODE4_02_DFMT_4096X2160P_24HZ;		break;
		case SYSTEM_VFORM_4096x2160P_25_256v135:
			dfmt = HDMIRX_SWMODE4_02_DFMT_4096X2160P_25HZ;		break;
		case SYSTEM_VFORM_4096x2160P_2997or30_256v135:
			dfmt = HDMIRX_SWMODE4_02_DFMT_4096X2160P_30HZ;		break;
		case SYSTEM_VFORM_4096x2160P_50_256v135:
			dfmt = HDMIRX_SWMODE4_02_DFMT_4096X2160P_50HZ;		break;
		case SYSTEM_VFORM_4096x2160P_5994or60_256v135:
			dfmt = HDMIRX_SWMODE4_02_DFMT_4096X2160P_60HZ;		break;
		default:
			dfmt = HDMIRX_SWMODE4_02_DFMT_DEF;					break;
	}

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_DFMT_NUM);
	// 0x10B2: DFMT
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE4_02_ADDR,
						dfmt,
						HDMIRX_SWMODE4_02_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_output_video_data(system_video *p_video, uint8_t irq)
{
	system_port port;
	uint8_t hdmi_mode;
	uint8_t avi_detect, hdmi_vs_detect;
	uint8_t avi_if[HDMIRX_GET_AVI_INFOFRAME_LEN] = { 0 };
	uint8_t hdmi_vs_if[HDMIRX_GET_HDMI_VS_INFOFRAME_LEN] = { 0 };
	uint8_t rep;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;

	// Check HDMI Mode from Bstatus/RxInfo of HDCP
	ret = hdmirx_get_hdmi_mode(port, &hdmi_mode);
	CHECK_ERROR(ret);
	// get AVI InfoFrame
	ret = hdmirx_get_avi_infoframe(port, &avi_detect, avi_if);
	CHECK_ERROR(ret);
	// get HDMI Vendor Specific InfoFrame
	ret = hdmirx_get_hdmi_vs_infoframe(port, &hdmi_vs_detect, hdmi_vs_if);
	CHECK_ERROR(ret);
	// get HDMI Forum Vendor Specific InfoFrame
	// * not used currently
	// Check Video Format Parameter
	// * not used currently
	// HDMI Mode ?
	if (hdmi_mode) {
		rep = HDMIRX_VOUTCNT_NREPST_REPETITION_AUTO;
	} else {
		// Check Received Video Format
		// * already set in p_video->rx_video_format
		// get video pixel repetition
		ret = hdmirx_get_pixel_repetition(p_video->rx_video_format, &rep);
		CHECK_ERROR(ret);

	}
	// Set Received Video Pixel Repetition and Output Repetition
	ret = hdmirx_set_pixel_repetition(port, rep);
	CHECK_ERROR(ret);
	// set HDMI Rx video format (2nd)
	ret = hdmirx_get_vformat2(p_video, avi_detect, avi_if, hdmi_vs_detect, hdmi_vs_if);
	CHECK_ERROR(ret);
	// Set Clock Parameter
	ret = hdmirx_set_clock_param(p_video);
	CHECK_ERROR(ret);
	// Set Received Video Format when YCbCr420
	if (is_color_format_yuv420(p_video->rx_color_format)) {
		ret = hdmirx_set_dfmt(port, p_video->rx_video_format);
		CHECK_ERROR(ret);
	}
	CHECK_ABORT();
	// Input Color Format Setting
	// Set Output Color Format
	ret = system_ctrl_set_color_format(p_video);
	CHECK_ERROR(ret);
	// delete UNKNOWN or VFSUCS interrupt
	// generated while system_ctrl_set_color_format() is being executed
	ret = hdmirx_clear_unnecessary_irq(port, irq);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_audio_status_mon(system_port port, uint8_t *p_astat)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_astat == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Audio Status Monitor Register #1,2
	// 0x22C0-0x22C1
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_RXAMON1_ADDR,
						HDMIRX_GET_AUDIO_STATUS_MON_LEN, p_astat);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_is_oba(uint8_t astat)
{
	uint32_t ret;

	switch (astat & HDMIRX_RXAMON2_RFMTSTATUS_MASK) {
		case HDMIRX_RXAMON2_RFMTSTATUS_AS:		ret = 0;	break;
		case HDMIRX_RXAMON2_RFMTSTATUS_OBA:		ret = 1;	break;
		case HDMIRX_RXAMON2_RFMTSTATUS_HBR:		ret = 0;	break;
		case HDMIRX_RXAMON2_RFMTSTATUS_DST:		ret = 0;	break;
		case HDMIRX_RXAMON2_RFMTSTATUS_MS_AS:	ret = 0;	break;
		case HDMIRX_RXAMON2_RFMTSTATUS_MS_OBA:	ret = 1;	break;
		default:								ret = 0;	break;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_sampling_freq(system_port port, uint8_t *p_astat)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SAMPLING_FREQ_NUM];
	uint8_t fsalt;
	uint32_t ret;

	if (p_astat == NULL) {
		return ERR_NULL_POINTER;
	}

	// check whether received audio format is One Bit Audio
	ret = hdmirx_is_oba(p_astat[1]);
	fsalt = (ret) ? HDMIRX_RXCRCTL_FSALT_SW : HDMIRX_RXCRCTL_FSALT_HW;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SAMPLING_FREQ_NUM);
	// 0x22E0
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_RXCRCTL_ADDR,
						fsalt,
						HDMIRX_RXCRCTL_FSALT_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_sw_sampling_freq(system_port port, system_audio_sf sf)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_SW_SAMPLING_FREQ_NUM];
	uint8_t cbit, swsf;
	uint32_t ret;

	// convert system_audio_sf into sampling freq
	ret = system_ctrl_audio_sf2cbit(sf, &cbit);
	CHECK_ERROR(ret);
	// convert sampling freq of cbit into SWFS and SWSEXT
	swsf = (cbit & HDMIRX_RXCBMON_03_FS_RX_MASK)
			| ((cbit & HDMIRX_RXCBMON_03_FS_EXT_RX_MASK)
				>> (HDMIRX_RXCBMON_03_FS_EXT_RX_BIT - HDMIRX_RXFSCTL_SWSEXT_BIT));

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_SW_SAMPLING_FREQ_NUM);
	// 0x22E1
	SET_COMMAND(cmd, 0, VIDEOIF_AUDIO_RXFSCTL_ADDR,
						swsf,
						HDMIRX_RXFSCTL_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_audio_infoframe(system_port port, uint8_t *p_aif)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_aif == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Audio InfoFrame Register
	// 0x2860-0x286D
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_INF_RXAUDIMON_00_ADDR,
						HDMIRX_GET_AUDIO_INFOFRAME_LEN, p_aif);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_audio_cbit(system_port port, uint8_t *p_cbit)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_cbit == NULL) {
		return ERR_NULL_POINTER;
	}

	// get Received Channel Status Bit Monitor
	// 0x22D0-0x22D7
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_RXCBMON_00_ADDR,
						HDMIRX_GET_AUDIO_CBIT_LEN, p_cbit);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_aformat(hdmirx_audio *p_audio, uint8_t *p_astat)
{
	system_port port;
	uint8_t aif[HDMIRX_GET_AUDIO_INFOFRAME_LEN];
	uint8_t cbit[HDMIRX_GET_AUDIO_CBIT_LEN];
	system_audio_sf if_sf, cb_sf;
	system_audio_ss if_ss, cb_ss;
	uint32_t ret;

	if (p_audio == NULL || p_astat == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_audio->port;

	// get received audio infoframe
	ret = hdmirx_get_audio_infoframe(port, aif);
	CHECK_ERROR(ret);
	// get received audio channel status bit
	ret = hdmirx_get_audio_cbit(port, cbit);
	CHECK_ERROR(ret);
	// check audio infoframe version
	if (aif[1] != INFOFRAME_VERSION_AUDIO) {
		return ERR_HDMIRX_AUDIO_INFOFRAME_VERSION;
	}
	// convert audio infoframe sampling freq into system_audio_sf
	ret = system_ctrl_audio_if2sf((aif[5] & HDMIRX_RXAUDIMON_05_SF_MASK), &if_sf);
	CHECK_ERROR(ret);
	// convert audio infoframe sample size into system_audio_ss
	ret = system_ctrl_audio_if2ss((aif[5] & HDMIRX_RXAUDIMON_05_SS_MASK), &if_ss);
	CHECK_ERROR(ret);
	// convert cbit sampling freq into system_audio_sf
	ret = system_ctrl_audio_cbit2sf((cbit[3] & (HDMIRX_RXCBMON_03_FS_RX_MASK
												| HDMIRX_RXCBMON_03_FS_EXT_RX_MASK)),
									&cb_sf);
	CHECK_ERROR(ret);
	// convert cbit sample size into system_audio_ss
	ret = system_ctrl_audio_cbit2ss((cbit[4] & HDMIRX_RXCBMON_04_WORD_LEN_RX_MASK),
									&cb_ss);
	CHECK_ERROR(ret);

	p_audio->if_channel_count		= aif[4] & HDMIRX_RXAUDIMON_04_CC_MASK;
	p_audio->if_sampling_freq		= if_sf;
	p_audio->if_sample_size			= if_ss;
	p_audio->if_channel_number		= aif[7] & HDMIRX_RXAUDIMON_07_CA_MASK;
	p_audio->if_level_shift_value	= (aif[8] & HDMIRX_RXAUDIMON_08_LSV_MASK)
										>> HDMIRX_RXAUDIMON_08_LSV0_BIT;
	p_audio->cb_sampling_freq		= cb_sf;
	p_audio->cb_sample_size			= cb_ss;
	p_audio->sampling_freq_ratio	= SYSTEM_AUDIO_SF_RATIO_1_1;
	p_audio->i2s.endian_mode		= SYSTEM_AUDIO_EM_MSB;
	p_audio->i2s.polarity			= SYSTEM_AUDIO_PO_LOW;
	p_audio->i2s.i2s_format			= SYSTEM_AUDIO_IF_DISABLE;
	p_audio->i2s.start_pos			= SYSTEM_AUDIO_SP_LEFT;

	// check whether received audio format is One Bit Audio
	ret = hdmirx_is_oba(p_astat[1]);
	if (ret) {
		// set SW sampling frequency according to hdmirx_set_sampling_freq()
		ret = hdmirx_set_sw_sampling_freq(port, cb_sf);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_output_audio_data(system_audio *p_audio)
{
	system_port port;
	uint8_t astat[HDMIRX_GET_AUDIO_STATUS_MON_LEN];
	uint32_t ret;

	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_audio->port;

	// Check Received Audio Format
	ret = hdmirx_get_audio_status_mon(port, astat);
	CHECK_ERROR(ret);
	// Set Output Sampling Frequency
	ret = hdmirx_set_sampling_freq(port, astat);
	CHECK_ERROR(ret);
	// Release Audio Mute
	ret = hdmirx_set_mode_ctrl1(port, HDMIRX_MODCNT1_ADMUTE_UNMUTE, HDMIRX_MODCNT1_ADMUTE_MASK);
	CHECK_ERROR(ret);
	// get audio format from Audio InfoFrame and Audio Channel Status Bit
	ret = hdmirx_get_aformat(p_audio, astat);
	CHECK_ERROR(ret);
	// Set Audio Channel Assignment
	ret = hdmirx_set_audio_ch_assign(port, p_audio->if_channel_number);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_spd_infoframe_data(hdmirx_spd_infoframe *p_spd)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_GET_SPD_INFOFRAME_DATA_LEN];
	uint8_t i, j;
	uint32_t ret;

	if (p_spd == NULL) {
		return ERR_NULL_POINTER;
	}

	// hold SPD packet data
	ret = hdmirx_set_packet_hold(p_spd->port, HDMIRX_PKTHLDCTL_00_PKTHOLD_SPD);
	CHECK_ERROR(ret);

	// 0x2840-0x285C
	SET_BLOCK_ACCESS(baccess, &p_spd->port, VIDEOIF_INF_RXSPDIMON_00_ADDR,
						HDMIRX_GET_SPD_INFOFRAME_DATA_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	// not hold packet data
	ret = hdmirx_set_packet_hold(p_spd->port, HDMIRX_PKTHLDCTL_00_PKTHOLD_NOT_HOLD);
	CHECK_ERROR(ret);

	// set Vendor Name Character
	for (i=0, j=HDMIRX_RXSPDIMON_VN_IDX; i<SYSTEM_SPD_INFOFRAME_VN_BYTE_SIZE; i++, j++) {
		p_spd->vendor_name[i] = data[j];
	}
	// set Product Description Character
	for (i=0, j=HDMIRX_RXSPDIMON_PD_IDX; i<SYSTEM_SPD_INFOFRAME_PD_BYTE_SIZE; i++, j++) {
		p_spd->product_description[i] = data[j];
	}
	// set Source Information
	p_spd->source_infomation = data[HDMIRX_RXSPDIMON_SI_IDX];

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_get_hdr_infoframe_data(hdmirx_hdr_infoframe *p_hdr)
{
	system_block_access baccess;
	uint8_t data[HDMIRX_GET_HDR_INFOFRAME_DATA_LEN];
	uint8_t i, j;
	uint32_t ret;

	if (p_hdr == NULL) {
		return ERR_NULL_POINTER;
	}

	// hold HDR(Free) packet data
	ret = hdmirx_set_packet_hold(p_hdr->port, HDMIRX_PKTHLDCTL_00_PKTHOLD_FRE);
	CHECK_ERROR(ret);

	// 0x26A0-0x26BD
	SET_BLOCK_ACCESS(baccess, &p_hdr->port, VIDEOIF_PKT_RFREPKT_00_ADDR,
						HDMIRX_GET_HDR_INFOFRAME_DATA_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	// not hold packet data
	ret = hdmirx_set_packet_hold(p_hdr->port, HDMIRX_PKTHLDCTL_00_PKTHOLD_NOT_HOLD);
	CHECK_ERROR(ret);

	// set EOTF
	p_hdr->eotf = data[HDMIRX_RFREPKT_EOTF_IDX] & HDMIRX_RFREPKT_EOTF_MASK;
	// set Static_Metadata_Descriptor_ID
	p_hdr->static_metadata_descriptor_id = data[HDMIRX_RFREPKT_SMD_ID_IDX] & HDMIRX_RFREPKT_SMD_ID_MASK;
	// set Static_Metadata_Descriptor
	for (i=0, j=HDMIRX_RFREPKT_SMD_IDX; i<SYSTEM_HDR_INFOFRAME_SMD_BYTE_SIZE; i++, j++) {
		p_hdr->static_metadata_descriptor[i] = data[j];
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_select_page_of_0x18(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SELECT_PAGE_OF_0X18_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SELECT_PAGE_OF_0X18_NUM);
	// 0x101C
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG14_ADDR,
						page + port.port_num,
						HDMIRX_SELECT_PAGE_OF_0X18_SYSCFG14_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_init_scdcs(system_port port)
{
	uint32_t ret;

	ret = hdmirx_select_page_of_0x18(port, HDMIRX_SELECT_PAGE_OF_0X18_SCDCS);
	CHECK_ERROR(ret);

	// page: 10, 11, 12, 13 (according to port.port_num)

	// source version: 0
	ret = hdmirx_set_scdcs_source_ver(port, HDMIRX_SRCVERX_SOURCE_VERSION_0);
	CHECK_ERROR(ret);
	// TMDS_Bit_Clock_Ratio: (TMDS bit period)/(TMDS clock period) ratio is 1/10
	// Scrambling_Enable: disable
	ret = hdmirx_set_scdcs_tmds_config(port, HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_1_10
												| HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_DIS);
	CHECK_ERROR(ret);
	// Scrambling_Status: not detected
	ret = hdmirx_set_scdcs_scrambler_status(port, HDMIRX_SCRSTATX_SCRAMBLING_STATUS_NOT_DETECT);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_reset_all_irq(system_port port)
{
	uint32_t ret;

	ret = hdmirx_set_irq0_of_portno(port, port.port_num,
									HDMIRX_RESET_ALL_IRQ_CLR, HDMIRX_RESET_ALL_IRQ_MASK);
	CHECK_ERROR(ret);
	ret = hdmirx_set_irq4_of_portno(port, port.port_num,
									HDMIRX_RESET_ALL_IRQ_CLR, HDMIRX_RESET_ALL_IRQ_MASK);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_check_port_status(hdmirx_port_status *p_status, uint8_t irq_5v,
											uint8_t mon_5v, uint8_t *p_irq_tmds)
{
	uint32_t i;
	system_port port;
	uint8_t mask, selected_portno;
	uint8_t power[SYSTEM_PORT_MAX_NUMBER];
	uint32_t ret;

	if (p_status == NULL || p_irq_tmds == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_status->port;
	// get selected rx port
	ret = hdmirx_get_selected_rx_port(port, &selected_portno);
	CHECK_ERROR(ret);

	for (i=0; i<SYSTEM_PORT_MAX_NUMBER; i++) {
		CHECK_ABORT();
		mask = (HDMIRX_INTREG8_P0V5STCHG_BIT << i);
		// set power status
		// is 5V power down?
		if ((mon_5v & mask) == 0) {			// power down
			power[i] = HDMIRX_CHECK_PORT_STATUS_POWER_DOWN;
		} else {							// power on
			power[i] = HDMIRX_CHECK_PORT_STATUS_POWER_ON;
		}
		// is 5V power status changed ?
		if ((irq_5v & mask) != 0) {			// 5V power status is changed
			// is 5V power down?
			if ((mon_5v & mask) == 0) {			// power down
				ret = hdmirx_init_scdcs(port);
				CHECK_ERROR(ret);
				ret = hdmirx_reset_all_irq(port);
				CHECK_ERROR(ret);
			} else {							// power on
				// is TMDS clock freq changed?
				if (p_irq_tmds[i] != 0) {		// TMDS clock freq is changed
					// is port#(i) selected port#?
					if (i != selected_portno) {		// not selected
						// Variation Profile of reception data in non-preferred port (3.3)
						ret = hdmirx_set_unselected_ddc_tmds_of_portno(port, i);
						CHECK_ERROR(ret);
						// clear irq of TMDS clock freq changed for unselected port
						// as for selected port irq, it is not cleared
						// because it is referred in hdmirx_get_av_status()
						ret = hdmirx_set_irq0_of_portno(port, i, p_irq_tmds[i], HDMIRX_INTREGX_MASK);
						CHECK_ERROR(ret);
					} else {						// selected
						;
					}
				} else {						// TMDS clock freq is not changed
					;
				}
			}
		} else {							// 5V power status is not changed
			;
		}
	}

	// clear irq of 5V power
	ret = hdmirx_clr_irq_5v_power(port, irq_5v);
	CHECK_ERROR(ret);
	// set connected status
	p_status->port0 = power[0];
	p_status->port1 = power[1];
	p_status->port2 = power[2];
	p_status->port3 = power[3];

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rx_soft_reset1(system_port port, uint8_t rst)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RX_SOFT_RESET1_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RX_SOFT_RESET1_NUM);
	// 0x2004
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_RSRST1_ADDR,
						rst,
						HDMIRX_RSRST1_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_set_rx_soft_reset3(system_port port, uint8_t rst)
{
	system_reg_access access;
	system_command cmd[HDMIRX_SET_RX_SOFT_RESET3_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMIRX_SET_RX_SOFT_RESET3_NUM);
	// 0x2006
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_RSRST3_00_ADDR,
						rst,
						HDMIRX_RSRST3_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_clear_unnecessary_irq(system_port port, uint8_t irq)
{
	uint8_t irq_re;
	uint32_t ret;

	// delete UNKNOWN or VFSUCS interrupt
	// generated while hdmirx_re_calibrate_tmds() or system_ctrl_set_color_format()
	// is being executed
	// re-detect video format
	ret = hdmirx_get_rx_irq1(port, &irq_re, (HDMIRX_RINTREG1_UNKNOWN_MASK
											| HDMIRX_RINTREG1_VFSUCS_MASK));
	CHECK_ERROR(ret);
	// irq cleanup
	ret = hdmirx_set_rx_irq1(port, (~irq & irq_re), HDMIRX_RINTREG1_MASK);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_check_tmds_speed_mode(system_port port, hdmirx_tmds_speed_mode *p_sm)
{
	uint8_t tmds_config, tmds_rate;
	uint32_t ret;

	if (p_sm == NULL) {
		return ERR_NULL_POINTER;
	}

	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_select_page_of_0x18(port, HDMIRX_SELECT_PAGE_OF_0X18_SCDCS);
	CHECK_ERROR(ret);
	// get TMDS config (according to port.port_num)
	// page: 10, 11, 12, 13 (according to port.port_num)
	ret = hdmirx_get_scdcs_tmds_config(port, &tmds_config,
										HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_MASK
										| HDMIRX_TMDSCFGX_SCRAMBLING_ENABLE_MASK);
	CHECK_ERROR(ret);
	// get TMDS Data Rate (Rx Port0,1,2,3)
	ret = hdmirx_get_tmds_data_rate_of_portno(port, port.port_num, &tmds_rate);
	CHECK_ERROR(ret);
	if ((tmds_config & HDMIRX_TMDSCFGX_TMDS_BIT_CLOCK_RATIO_MASK)
		|| (tmds_rate > HDMIRX_CKDET2X_3_PXTMDSFQ_FDTTHF_FDTTHG)) {
		// Set TMDS High Speed Mode
		*p_sm = HDMIRX_TMDS_SPEED_MODE_HIGH;
	} else {
		// Set TMDS Legacy Speed Mode
		*p_sm = HDMIRX_TMDS_SPEED_MODE_LEGACY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_re_calibrate_tmds(system_port port)
{
	uint32_t i;
	hdmirx_tmds_speed_mode tmds_sm, pre_tmds_sm;
	uint8_t frqchg, vffail;
	uint32_t pcf;
	uint32_t ret = ERR_OK;

	// when TMDS clock freq is changed between 3.4Gbps or less and more,
	// TMDS speed mode must be changed.

	for (i=0; i<HDMIRX_RE_CALIBRATE_TMDS_RETRY_MAX; i++) {
		CHECK_ABORT();
		// check TMDS speed mode
		ret = hdmirx_check_tmds_speed_mode(port, &tmds_sm);
		CHECK_ERROR(ret);
		// get pre TMDS speed mode
		ret = hdmirx_access_tmds_speed_mode(port, &pre_tmds_sm, NULL);
		CHECK_ERROR(ret);
		// get TMDS clock freq changed
		ret = hdmirx_get_irq0_of_portno(port, port.port_num, &frqchg, HDMIRX_IRQ_INTREGX_FRQCHGX_MASK);
		CHECK_ERROR(ret);
		// get irq of VFFAIL
		ret = hdmirx_get_rx_irq1(port, &vffail, HDMIRX_RINTREG1_VFFAIL_MASK);
		CHECK_ERROR(ret);

		if ((tmds_sm == pre_tmds_sm) && frqchg == 0 && vffail == 0) {
			// TMDS speed mode is not changed
			// TMDS clock freq is not changed
			// VFAIL is not detected
			break;
		}
		if (frqchg != 0) {
			// calculate pixel clock
			ret = hdmirx_get_calc_pixel_clock_freq(port, HDMIRX_PCKMON_UNSPECIFIED, &pcf);
			CHECK_ERROR(ret);
			if (pcf == HDMIRX_PIXEL_CLOCK_0KHZ || pcf > HDMIRX_PIXEL_CLOCK_620000KHZ) {
				// TMDS clock counter is zero
				// TMDS clock counter is over 620MHz
				break;
			}
		}
		if (vffail != 0) {
			ret = hdmirx_set_rx_soft_reset3(port, HDMIRX_RSRST3_DCMRST
													| HDMIRX_RSRST3_PFQRST);
			CHECK_ERROR(ret);
			ret = hdmirx_set_rx_soft_reset1(port, HDMIRX_RSRST1_VIFRST);
			CHECK_ERROR(ret);
			CHECK_ABORT();
			// wait for 200ms
			SLEEP(HDMIRX_SLEEP_TIME_200MSEC);
		}
		// TMDS Alignment Lock Detect (3.1.1) (2.1.2.6)
		ret = hdmirx_tmds_alignment_lock_detect(port);
		CHECK_ERROR(ret);
		// TMDS Calibration (3.1.2)
		ret = hdmirx_tmds_calibration(port);
		CHECK_ERROR(ret);
		if (vffail != 0) {
			ret = hdmirx_set_rx_soft_reset3(port, HDMIRX_RSRST3_DCMRST
													| HDMIRX_RSRST3_PFQRST);
			CHECK_ERROR(ret);
			ret = hdmirx_set_rx_soft_reset1(port, HDMIRX_RSRST1_VIFRST);
			CHECK_ERROR(ret);
			CHECK_ABORT();
			// wait for 200ms
			SLEEP(HDMIRX_SLEEP_TIME_200MSEC);
		}

		// clear irq of VFFAIL
		ret = hdmirx_set_rx_irq1(port, vffail, HDMIRX_RINTREG1_MASK);
		CHECK_ERROR(ret);
		// clear irq of TMDS clock freq changed
		ret = hdmirx_set_irq0_of_portno(port, port.port_num, frqchg, HDMIRX_INTREGX_MASK);
		CHECK_ERROR(ret);
		CHECK_ABORT();
		// wait for 20ms
		SLEEP(HDMIRX_SLEEP_TIME_20MSEC);
	}
	if (i == HDMIRX_RE_CALIBRATE_TMDS_RETRY_MAX) {
		return ERR_HDMIRX_TMDS_CALIBRATION_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_re_detect_deep_color(system_port port)
{
	uint32_t i;
	uint8_t cd, pckmon;
	uint32_t ret = ERR_OK;

	for (i=0; i<HDMIRX_RE_DETECT_DEEP_COLOR_RETRY_MAX; i++) {
		CHECK_ABORT();
		// Check GCP Packet (Color Depth)
		ret = hdmirx_get_gcp_color_depth(port, &cd);
		CHECK_ERROR(ret);
		// Check Pixel Clock Monitor
		ret = hdmirx_get_rx_mon(port, &pckmon, HDMIRX_MONREG_00_PCKMON_MASK);
		CHECK_ERROR(ret);
		// Reset Deep Color Mode
		if (((cd == HDMIRX_GCP_COLOR_DEPTH_NONE) && (pckmon == HDMIRX_PIXEL_CLOCK_24BIT))
			|| ((cd == HDMIRX_GCP_COLOR_DEPTH_24BIT) && (pckmon == HDMIRX_PIXEL_CLOCK_24BIT))
			|| ((cd == HDMIRX_GCP_COLOR_DEPTH_30BIT) && (pckmon == HDMIRX_PIXEL_CLOCK_30BIT))
			|| ((cd == HDMIRX_GCP_COLOR_DEPTH_36BIT) && (pckmon == HDMIRX_PIXEL_CLOCK_36BIT))) {
			// video format detection does not fail
			// and color depth of GCP and pckmon are same
			break;
		}
		// video format detection fails
		// or color depth of GCP and pckmon are not same
		ret = hdmirx_set_rx_soft_reset3(port, HDMIRX_RSRST3_DCMRST);
		CHECK_ERROR(ret);
		ret = hdmirx_set_rx_soft_reset1(port, HDMIRX_RSRST1_VIFRST);
		CHECK_ERROR(ret);
		CHECK_ABORT();
		// wait for 200ms
		SLEEP(HDMIRX_SLEEP_TIME_200MSEC);
	}
	if (i == HDMIRX_RE_DETECT_DEEP_COLOR_RETRY_MAX) {
		return ERR_HDMIRX_DEEP_COLOR_DETECT_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_ctrl_req_abort(system_request *p_req, hdmirx_ctrl_req ctrl)
{
	static system_request hdmirx_abort_req = SYSTEM_REQUEST_NONE;

	if (p_req == NULL) {
		return ERR_NULL_POINTER;
	}

	if (ctrl == HDMIRX_CTRL_REQ_GET) {
		*p_req =hdmirx_abort_req ;
	} else if (ctrl == HDMIRX_CTRL_REQ_SET) {
		hdmirx_abort_req = *p_req;
	} else {
		return ERR_NG;
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
system_request hdmirx_ctrl_get_abort_req(void)
{
	system_request req;

	if (hdmirx_ctrl_req_abort(&req, HDMIRX_CTRL_REQ_GET)) {
		return SYSTEM_REQUEST_NONE;
	}

	return req;
}
//------------------------------------------------------------------------------
void hdmirx_ctrl_set_abort_req(system_request req)
{
	if (hdmirx_ctrl_req_abort(&req, HDMIRX_CTRL_REQ_SET)) {
		return;
	}
}
//------------------------------------------------------------------------------
uint32_t hdmirx_init(system_port port)
{
	uint32_t ret;

	// set system configuration (2.1.1)
	//   HDMI Rx power on
	ret = system_ctrl_init(port);
	CHECK_ERROR(ret);
	//   audio initialization
	ret = system_ctrl_audio_init(port, SYSTEM_CTRL_AUDIO_HDMI_RX);
	CHECK_ERROR(ret);
	//   HDMI mode selection
	ret = hdmirx_select_hdmi_mode(port);
	CHECK_ERROR(ret);
	// TMDS data channel swap & polarity inversion setting
	ret = hdmirx_set_tmds_data_swp(port);
	CHECK_ERROR(ret);
	// TMDS Receiver Initialization (2.1.2)
	ret = hdmirx_tmds_receiver_init(port);
	CHECK_ERROR(ret);
	// Receiver Link Initialization (2.1.3)
	ret = hdmirx_receiver_link_init(port);
	CHECK_ERROR(ret);
	// Receiver Port Select (2.1.4)
	ret = hdmirx_receiver_port_select(port);
	CHECK_ERROR(ret);
	// init abort request
	hdmirx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);
	// init HDCP Rx
	ret = hdmirx_hdcp_init(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_exit(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_exit(port, SYSTEM_CTRL_AUDIO_HDMI_RX);
	CHECK_ERROR(ret);
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_port_status(hdmirx_port_status *p_status)
{
	uint8_t irq_5v, mon_5v, irq_tmds[SYSTEM_PORT_MAX_NUMBER];
	system_port port;
	uint32_t ret;

	if (p_status == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_status->port;

	// hdmirx_init() check is omitted.

	// port# connection is confirmed
	// whether 5V power is input or HPD pin is active.

	// get 5v power status change
	ret = hdmirx_get_irq_5v_power(port, &irq_5v);
	CHECK_ERROR(ret);
	// get 5v power monitor
	ret = hdmirx_get_mon_5v_power(port, &mon_5v);
	CHECK_ERROR(ret);
	// get TMDS clock frequency changed
	ret = hdmirx_get_irq_tmds_clock(port, irq_tmds);
	CHECK_ERROR(ret);
	// check port status
	ret = hdmirx_check_port_status(p_status, irq_5v, mon_5v, irq_tmds);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_av_status(hdmirx_av_status *p_status)
{
	system_port port;
	uint8_t selected_portno;
	uint8_t irq;
	hdmirx_av_status_format fmt;
	hdmirx_av_status_mute mute;
	uint32_t ret;

	if (p_status == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_status->port;

	// hdmirx_init() check is omitted.

	// check current selected port#
	ret = hdmirx_get_selected_rx_port(port, &selected_portno);
	CHECK_ERROR(ret);
	if (port.port_num != selected_portno) {
		return ERR_HDMIRX_INVALID_PORT;
	}
	// TMDS Re-Calibration
	ret = hdmirx_re_calibrate_tmds(port);
	CHECK_ERROR(ret);
	// Deep Color Mode Re-Detection (3.6)
	ret = hdmirx_re_detect_deep_color(port);
	CHECK_ERROR(ret);
	// when Scramble Monitor is enabled and Source writes a 1 to Scrambling_Enable,
	// set Scrambling_Status to a 1
	ret = hdmirx_set_scdcs_scrambling_status(port, selected_portno);
	CHECK_ERROR(ret);
	// whether video format has changed or not
	// whether video has muted or unmuted only by AVMUTE of GCP
	ret = hdmirx_get_rx_video_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	p_status->video_format = fmt;
	p_status->video_mute = mute;
	// whether AVI InfoFrame has changed or not
	// if changed, video format has changed
	ret = hdmirx_get_rx_avi_infoframe_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	p_status->video_format = p_status->video_format | fmt;
	// whether audio format might have changed or not
	// whether audio has muted or unmuted
	ret = hdmirx_get_rx_audio_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	p_status->audio_format = fmt;
	p_status->audio_mute = mute;
	// whether SPD InfoFrame has changed or not
	// whether SPD InfoFrame has muted or unmuted
	ret = hdmirx_get_rx_spd_infoframe_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	p_status->spd_format = fmt;
	p_status->spd_mute = mute;
	// whether HDR InfoFrame has changed or not
	// whether HDR InfoFrame has muted or unmuted
	ret = hdmirx_get_rx_hdr_infoframe_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	p_status->hdr_format = fmt;
	p_status->hdr_mute = mute;

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_select_port(system_port port)
{
	uint32_t ret;

	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(port);
	CHECK_ERROR(ret);
	// Receiver Port Select (2.1.4)
	ret = hdmirx_receiver_port_select(port);
	CHECK_ERROR(ret);
	// TMDS Alignment Lock Detect (3.1.1) (2.1.2.6)
	ret = hdmirx_tmds_alignment_lock_detect(port);
	CHECK_ERROR(ret);
	// TMDS Calibration (3.1.2)
	ret = hdmirx_tmds_calibration(port);
	CHECK_ERROR(ret);
	// Change Receiver Clock Setting (3.1.3)
	ret = hdmirx_change_receiver_clock_setting(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_video_format(hdmirx_video *p_video)
{
	system_port port;
	uint8_t irq, irq2;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_video->rx_port;

	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(port);
	CHECK_ERROR(ret);
	// detect video format
	ret = hdmirx_get_rx_irq1(port, &irq, (HDMIRX_RINTREG1_UNKNOWN_MASK
											| HDMIRX_RINTREG1_VFSUCS_MASK));
	CHECK_ERROR(ret);
	// get AVI InfoFrame Parameter Changed
	ret = hdmirx_get_packet_param_change1(port, &irq2, HDMIRX_PPMCHG1_AVICHG_MASK);
	CHECK_ERROR(ret);

	if (irq || irq2) {
		// Video Format Detect (3.1.4)
		ret = hdmirx_video_format_detect(p_video, &irq);
		// if retry error occurs, get video format from avi infoframe
		if (ret != ERR_OK) {
			if (ret != ERR_HDMIRX_VIDEO_FORMAT_DETECT_RETRY) {
				return ret;
			}
		}
		// Output Video Data (3.1.5)
		ret = hdmirx_output_video_data(p_video, irq);
		CHECK_ERROR(ret);
		// TMDS Re-Calibration
		ret = hdmirx_re_calibrate_tmds(port);
		CHECK_ERROR(ret);
		// delete UNKNOWN or VFSUCS interrupt
		// generated while hdmirx_re_calibrate_tmds() is being executed
		ret = hdmirx_clear_unnecessary_irq(port, irq);
		CHECK_ERROR(ret);
		// irq cleanup
		ret = hdmirx_set_rx_irq1(port, irq, HDMIRX_RINTREG1_MASK);
		CHECK_ERROR(ret);
		// clear irq of AVI InfoFrame Parameter Changed
		ret = hdmirx_clr_packet_param_change1(port, irq2);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_audio_format(hdmirx_audio *p_audio)
{
	system_port port;
	uint8_t irq;
	hdmirx_av_status_format fmt;
	hdmirx_av_status_mute mute;
	uint32_t ret;

	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_audio->port;

	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(port);
	CHECK_ERROR(ret);
	// whether audio format might have changed or not
	// whether audio has muted or unmuted
	ret = hdmirx_get_rx_audio_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	// audio format can be detected
	if (mute == HDMIRX_AV_STATUS_MUTE_OFF) {
		// Output Audio Data (3.1.6)
		ret = hdmirx_output_audio_data(p_audio);
		CHECK_ERROR(ret);
		// irq cleanup
		ret = hdmirx_set_rx_irq3(port, irq, HDMIRX_RINTREG3_MASK);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_spd_infoframe(hdmirx_spd_infoframe *p_spd)
{
	system_port port;
	uint8_t irq;
	hdmirx_av_status_format fmt;
	hdmirx_av_status_mute mute;
	uint32_t ret;

	if (p_spd == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_spd->port;

	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(port);
	CHECK_ERROR(ret);
	// whether SPD InfoFrame has changed or not
	// whether SPD InfoFrame has muted or unmuted
	ret = hdmirx_get_rx_spd_infoframe_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	if (mute == HDMIRX_AV_STATUS_MUTE_OFF) {
		// get SPD InfoFrame
		ret = hdmirx_get_spd_infoframe_data(p_spd);
		CHECK_ERROR(ret);
		// clear irq of SPD InfoFrame Parameter Changed
		ret = hdmirx_clr_packet_param_change1(port, irq);
		CHECK_ERROR(ret);
	} else {
		return ERR_NG;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_hdr_infoframe(hdmirx_hdr_infoframe *p_hdr)
{
	system_port port;
	uint8_t irq;
	hdmirx_av_status_format fmt;
	hdmirx_av_status_mute mute;
	uint32_t ret;

	if (p_hdr == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_hdr->port;

	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(port);
	CHECK_ERROR(ret);
	// whether HDR InfoFrame has changed or not
	// whether HDR InfoFrame has muted or unmuted
	ret = hdmirx_get_rx_hdr_infoframe_status(port, &irq, &fmt, &mute);
	CHECK_ERROR(ret);
	if (mute == HDMIRX_AV_STATUS_MUTE_OFF) {
		// get HDR InfoFrame
		ret = hdmirx_get_hdr_infoframe_data(p_hdr);
		CHECK_ERROR(ret);
		// clear irq of HDR InfoFrame Parameter Changed
		ret = hdmirx_clr_packet_param_change0(port, irq);
		CHECK_ERROR(ret);
	} else {
		return ERR_NG;
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_get_pixel_clock_frequency(hdmirx_pixel_clock_freq *p_pcf)
{
	system_port port;
	uint32_t pcf;
	uint32_t ret;

	if (p_pcf == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_pcf->port;
	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(port);
	CHECK_ERROR(ret);
	// get pixel clock
	ret = hdmirx_get_calc_pixel_clock_freq(port, HDMIRX_PCKMON_UNSPECIFIED, &pcf);
	CHECK_ERROR(ret);
	p_pcf->pixel_clock_freq = pcf;

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_abort(void)
{
	// abort request
	hdmirx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

	return ERR_OK;
}
//------------------------------------------------------------------------------
