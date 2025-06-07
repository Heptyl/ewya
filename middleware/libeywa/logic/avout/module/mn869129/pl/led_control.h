/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __LED_CONTROL_H__
#define __LED_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	LED_CTRL_OFF = 0,
	LED_CTRL_ON = 1
}LED_CTRL;

#ifdef _WIN32
	#define led_ctrl_ch0(c)
	#define led_ctrl_ch1(c)
#elif defined(__linux)
	#define led_ctrl_ch0(c)
	#define led_ctrl_ch1(c)
#elif defined(__XC8)
	#define led_ctrl_ch0(c)
	#define led_ctrl_ch1(c)
#elif (1)
	void pic24_led_ctrl_ch0(LED_CTRL c);
	void pic24_led_ctrl_ch1(LED_CTRL c);
	#define led_ctrl_ch0(c)		pic24_led_ctrl_ch0(c)
	#define led_ctrl_ch1(c)		pic24_led_ctrl_ch1(c)
#else
	#error "unsupported compiler"
#endif

#ifdef __cplusplus
}
#endif

#endif // __LED_CONTROL_H__
