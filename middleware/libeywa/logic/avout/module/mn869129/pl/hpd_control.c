/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#include "hpd_control.h"

#ifdef __XC16
//#include "pic24_port_control.h"
#endif

// -----------------------------------------------------------------------------
#ifdef __XC16
void pic24_hpd_ctrl_rx0(HPD_CTRL c)
{
	//PIC24_PORT_CTRL pic24_c;
	//pic24_c = (c ==HPD_CTRL_NEGATE) ? PIC24_PORT_CTRL_NEGATE : PIC24_PORT_CTRL_ASSERT;
	//pic24_output_rd6(pic24_c);
}
#endif
// -----------------------------------------------------------------------------
#ifdef __XC16
void pic24_hpd_ctrl_rx1(HPD_CTRL c)
{
	//PIC24_PORT_CTRL pic24_c;
	//pic24_c = (c ==HPD_CTRL_NEGATE) ? PIC24_PORT_CTRL_NEGATE : PIC24_PORT_CTRL_ASSERT;
	//pic24_output_rd5(pic24_c);
}
#endif
// -----------------------------------------------------------------------------
#ifdef __XC16
void pic24_hpd_ctrl_tx0(HPD_CTRL c)
{
	//PIC24_PORT_CTRL pic24_c;
	//pic24_c = (c ==HPD_CTRL_NEGATE) ? PIC24_PORT_CTRL_NEGATE : PIC24_PORT_CTRL_ASSERT;
	//pic24_output_re4(pic24_c);
}
#endif
// -----------------------------------------------------------------------------
