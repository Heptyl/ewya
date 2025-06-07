/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#include "led_control.h"

#if 0
#include "pic24_port_control.h"
#endif

// -----------------------------------------------------------------------------
#if 1
void pic24_led_ctrl_ch0(LED_CTRL c)
{
	//PIC24_PORT_CTRL pic24_c;
	//pic24_c = (c == LED_CTRL_OFF) ? PIC24_PORT_CTRL_NEGATE : PIC24_PORT_CTRL_ASSERT;
	//pic24_output_rb6(pic24_c);
}
#endif
// -----------------------------------------------------------------------------
#if 1
void pic24_led_ctrl_ch1(LED_CTRL c)
{
	//PIC24_PORT_CTRL pic24_c;
	//pic24_c = (c == LED_CTRL_OFF) ? PIC24_PORT_CTRL_NEGATE : PIC24_PORT_CTRL_ASSERT;
	//pic24_output_rb7(pic24_c);
}
#endif
// -----------------------------------------------------------------------------
