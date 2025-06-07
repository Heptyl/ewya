/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HPD_CONTROL_H__
#define __HPD_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	HPD_CTRL_NEGATE = 0,
	HPD_CTRL_ASSERT = 1
}HPD_CTRL;

#ifdef _WIN32
	#define hpd_ctrl_rx0(c)
	#define hpd_ctrl_rx1(c)
	#define hpd_ctrl_tx0(c)
#elif defined(__linux)
	#define hpd_ctrl_rx0(c)
	#define hpd_ctrl_rx1(c)
	#define hpd_ctrl_tx0(c)
#elif defined(__XC8)
	#define hpd_ctrl_rx0(c)
	#define hpd_ctrl_rx1(c)
	#define hpd_ctrl_tx0(c)
#elif (1)
	void pic24_hpd_ctrl_rx0(HPD_CTRL c);
	void pic24_hpd_ctrl_rx1(HPD_CTRL c);
	void pic24_hpd_ctrl_tx0(HPD_CTRL c);
	#define hpd_ctrl_rx0(c)		pic24_hpd_ctrl_rx0(c)
	#define hpd_ctrl_rx1(c)		pic24_hpd_ctrl_rx1(c)
	#define hpd_ctrl_tx0(c)		pic24_hpd_ctrl_tx0(c)
#else
	#error "unsupported compiler"
#endif

#ifdef __cplusplus
}
#endif

#endif // __HPD_CONTROL_H__
