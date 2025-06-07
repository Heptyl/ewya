/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "spdif.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// spdifrx_get_audio_format()
#define SPDIFRX_AUDIO_IF_CHANNEL_NUMBER_FR_FL						0x00
#define SPDIFRX_AUDIO_IF_LEVEL_SHIFT_VALUE_0DB						0x00
//------------------------------------------------------------------------------
// spdifrx_get_audio_cbit()
#define SPDIFRX_GET_AUDIO_CBIT_LEN									7
#define SPDIFRX_IECMON_03_FS_RX_MASK								0x0000000F
#define SPDIFRX_IECMON_03_FS_EXT_RX_MASK							0x000000C0
#define SPDIFRX_IECMON_04_WORD_LEN_RX_MASK							0x0000000F
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL														0
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
static uint32_t spdifrx_get_audio_cbit(system_port port, uint8_t *p_cbit);
//------------------------------------------------------------------------------
static uint32_t spdifrx_get_audio_cbit(system_port port, uint8_t *p_cbit)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_cbit == NULL) {
		return ERR_NULL_POINTER;
	}

	// get IEC60958 received channel status bit
	// 0x30A0-0x30A6
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_AUDIO_IECMON_00_ADDR,
						SPDIFRX_GET_AUDIO_CBIT_LEN, p_cbit);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t spdifrx_init(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_init(port, SYSTEM_CTRL_AUDIO_SPDIF_RX);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t spdifrx_exit(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_exit(port, SYSTEM_CTRL_AUDIO_SPDIF_RX);
	CHECK_ERROR(ret);
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t spdifrx_get_audio_format(spdifrx_audio *p_audio)
{
	system_port port;
	uint8_t cbit[SPDIFRX_GET_AUDIO_CBIT_LEN];
	system_audio_sf cb_sf;
	system_audio_ss cb_ss;
	uint32_t ret;

	if (p_audio == NULL) {
		return ERR_NULL_POINTER;
	}

	port = p_audio->port;

	// get received audio channel status bit
	ret = spdifrx_get_audio_cbit(port, cbit);
	CHECK_ERROR(ret);
	// convert cbit sampling freq into system_audio_sf
	ret = system_ctrl_audio_cbit2sf((cbit[3] & (SPDIFRX_IECMON_03_FS_RX_MASK
												| SPDIFRX_IECMON_03_FS_EXT_RX_MASK)),
									&cb_sf);
	CHECK_ERROR(ret);
	// convert cbit sample size into system_audio_ss
	ret = system_ctrl_audio_cbit2ss(cbit[4] & SPDIFRX_IECMON_04_WORD_LEN_RX_MASK,
									&cb_ss);
	CHECK_ERROR(ret);

	// SPDIF-RX default setting
	p_audio->if_channel_count		= SYSTEM_AUDIO_CC_2CH;
	p_audio->if_sampling_freq		= SYSTEM_AUDIO_SF_RSH;
	p_audio->if_sample_size			= SYSTEM_AUDIO_SS_RSH;
	p_audio->if_channel_number		= SPDIFRX_AUDIO_IF_CHANNEL_NUMBER_FR_FL;
	p_audio->if_level_shift_value	= SPDIFRX_AUDIO_IF_LEVEL_SHIFT_VALUE_0DB;
	p_audio->cb_sampling_freq		= cb_sf;
	p_audio->cb_sample_size			= cb_ss;
	p_audio->sampling_freq_ratio	= SYSTEM_AUDIO_SF_RATIO_1_1;
	p_audio->i2s.endian_mode		= SYSTEM_AUDIO_EM_MSB;
	p_audio->i2s.polarity			= SYSTEM_AUDIO_PO_LOW;
	p_audio->i2s.i2s_format			= SYSTEM_AUDIO_IF_DISABLE;
	p_audio->i2s.start_pos			= SYSTEM_AUDIO_SP_LEFT;

	return ret;
}
//------------------------------------------------------------------------------
uint32_t spdiftx_init(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_init(port, SYSTEM_CTRL_AUDIO_SPDIF_TX);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t spdiftx_exit(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_exit(port, SYSTEM_CTRL_AUDIO_SPDIF_TX);
	CHECK_ERROR(ret);
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t spdiftx_audio_mute(spdiftx_mute *p_mute)
{
	uint32_t ret;

	if (p_mute == NULL) {
		return ERR_NULL_POINTER;
	}

	ret = system_ctrl_external_audio_mute(p_mute);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
