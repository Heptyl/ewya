/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#ifndef __RESET_CONTROL_H__
#define __RESET_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
	#define reset_ctrl()
#elif defined(__linux)
	#define reset_ctrl()
#elif defined(__XC8)
	#define reset_ctrl()
#elif (1)
	void pic24_reset_ctrl(void);
	#define reset_ctrl()		pic24_reset_ctrl()
#else
	#error "unsupported compiler"
#endif

#ifdef __cplusplus
}
#endif

#endif // __RESET_CONTROL_H__
