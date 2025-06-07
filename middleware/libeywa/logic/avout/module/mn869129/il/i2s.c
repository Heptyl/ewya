/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "i2s.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL											0
#endif // NULL
//------------------------------------------------------------------------------
// error check & if (c) is a error, return (c)
#define CHECK_ERROR(c) {						\
		if ((c) != ERR_OK) {					\
			return (c);							\
		}										\
}
// register access setting
#define SET_ACCESS(a, p, c, n) {				\
		(a).p_port = (p);						\
		(a).p_com = (c);						\
		(a).count = (n);						\
}
// command setting
#define SET_COMMAND(c, i, a, d, m) {			\
		(c)[(i)].addr = (a);					\
		(c)[(i)].data = (d);					\
		(c)[(i)].mask = (m);					\
}
//------------------------------------------------------------------------------
uint32_t i2srx_init(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_init(port, SYSTEM_CTRL_AUDIO_I2S_RX);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t i2srx_exit(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_exit(port, SYSTEM_CTRL_AUDIO_I2S_RX);
	CHECK_ERROR(ret);
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t i2stx_init(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_init(port, SYSTEM_CTRL_AUDIO_I2S_TX);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t i2stx_exit(system_port port)
{
	uint32_t ret;

	ret = system_ctrl_audio_exit(port, SYSTEM_CTRL_AUDIO_I2S_TX);
	CHECK_ERROR(ret);
	ret = system_ctrl_exit(port);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t i2stx_audio_mute(i2stx_mute *p_mute)
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

