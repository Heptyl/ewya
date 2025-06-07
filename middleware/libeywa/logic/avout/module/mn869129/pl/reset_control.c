/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#include "reset_control.h"
#include "hv_sleep.h"

#ifdef __XC16
//#include "pic24_port_control.h"
#endif

// -----------------------------------------------------------------------------
#ifdef __XC16
void pic24_reset_ctrl(void)
{
	//pic24_output_re5(PIC24_PORT_CTRL_NEGATE);
	//SLEEP(10);
	//pic24_output_re5(PIC24_PORT_CTRL_ASSERT);
	//SLEEP(1);
	//pic24_output_re5(PIC24_PORT_CTRL_NEGATE);
	//SLEEP(10);
}
#endif
// -----------------------------------------------------------------------------
