/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "miscvborx.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// vborx_switch_hsmode()
#define VBORX_SWITCH_HSMODE_NUM								1
#define VBORX_GLUECTL0_1_HSMODE_EXTERNAL					0x00
#define VBORX_GLUECTL0_1_HSMODE_INTERNAL					0x40
#define VBORX_GLUECTL0_1_HSMODE_MASK						0x00000040
//------------------------------------------------------------------------------
// vborx_set_htotal_hsync()
#define VBORX_SET_HTOTAL_HSYNC_LEN							4
#define VBORX_FF_MASK										0xFF
#define VBORX_03_MASK										0x03
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
static uint32_t vborx_switch_hsmode(vborx_hsync *p_hsync);
static uint32_t vborx_set_htotal_hsync(vborx_hsync *p_hsync);
//------------------------------------------------------------------------------
static uint32_t vborx_switch_hsmode(vborx_hsync *p_hsync)
{
	system_reg_access access;
	system_command cmd[VBORX_SWITCH_HSMODE_NUM];
	uint32_t ret;

	if (p_hsync == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &(p_hsync->port), cmd, VBORX_SWITCH_HSMODE_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_VBORX_GLUE_GLUECTL0_1_ADDR,
						((p_hsync->hsmode == VBORX_HSMODE_EXTERNAL_INPUT)
							? VBORX_GLUECTL0_1_HSMODE_EXTERNAL
							: VBORX_GLUECTL0_1_HSMODE_INTERNAL),
						VBORX_GLUECTL0_1_HSMODE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_set_htotal_hsync(vborx_hsync *p_hsync)
{
	system_block_access baccess;
	uint8_t data[VBORX_SET_HTOTAL_HSYNC_LEN];
	uint32_t ret;

	if (p_hsync == NULL) {
		return ERR_NULL_POINTER;
	}
	// when hsmode=0, not necessary to set H total length and H sync width.
	if (p_hsync->hsmode == VBORX_HSMODE_EXTERNAL_INPUT) {
		return ERR_OK;
	}

	SET_BLOCK_ACCESS(baccess, &(p_hsync->port), VIDEOIF_VBORX_GLUE_GLUECTL1_0_ADDR,
						VBORX_SET_HTOTAL_HSYNC_LEN, data);
	SET_BLOCK_DATA(data, 0, ((p_hsync->htotal_length >> 0) & VBORX_FF_MASK));	// 0x2E84
	SET_BLOCK_DATA(data, 1, ((p_hsync->htotal_length >> 8) & VBORX_FF_MASK));	// 0x2E85
	SET_BLOCK_DATA(data, 2, ((p_hsync->hsync_width >> 0) & VBORX_FF_MASK));		// 0x2E86
	SET_BLOCK_DATA(data, 3, ((p_hsync->hsync_width >> 8) & VBORX_03_MASK));		// 0x2E87
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
// vborx_set_hsync
//   sets Hsync source from external input or internal generation
//
//   Prototype:
//     uint32_t vborx_set_hsync(vborx_hsync *p_hsync);
//
//   Parameters:
//     p_hsync    Pointer to structure of vborx_hsync to hold Hsync preference
//
//   Returns:
//     ERR_OK if successful, otherwise a system error code depending on the function.
//
//   Description:
//     This function sets Hsync source from external input or internal generation
//     Hsync source is set by external input usually (default). When internal
//     generation is preferred, set p_hsync->hsmode by
//     VBORX_HSMODE_INTERNAL_GENERATION, p_hsync->htotal_length and
//     p_hsync->hsync_width by preference values.
//     This function must be called after vborx_init function is called.
//   Examples:
//     vborx_hsync hsync;
//     hsync.hsmode = VBORX_HSMODE_INTERNAL_GENERATION;
//     hsync.htotal_length = 4400;
//     hsync.hsync_width = 88;
//     vborx_set_hsync(&hsync);
//------------------------------------------------------------------------------
uint32_t vborx_set_hsync(vborx_hsync *p_hsync)
{
	uint32_t ret;

	if (p_hsync == NULL) {
		return ERR_NULL_POINTER;
	}

	// check vborx_init()
	ret = system_ctrl_check_vborx_init(p_hsync->port);
	CHECK_ERROR(ret);
	// set htotal and hsync
	ret = vborx_set_htotal_hsync(p_hsync);
	CHECK_ERROR(ret);
	// switch hsmode
	ret = vborx_switch_hsmode(p_hsync);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
