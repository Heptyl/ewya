/*
 * Copyright (c) CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description:uapi_uart define
 * Author: AuthorNameMagicTag
 * Create: 2018-12-22
 * Update: 2019-11-20
 */

#ifndef __UAPI_UART_H__
#define __UAPI_UART_H__

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* *
\brief  get working uart number.
\retval uart number success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_get_number(void);

/* *
\brief switch to specific uart.
\param[in] tty_ama spec uart.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_switch(td_s32 tty_ama);

/* *
\brief initialize uart.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
\attention
\brief uart1 operation will not be effective until initialization except UAPI_UART_Switch and UAPI_UART_GetUart
\brief default received buff size of uart is 1000B, you can change it with -DUAPI_UART_BUFLEN=XXX in Makefile
*/
td_s32 uapi_uart_init(void);

/* *
\brief get uart attribute.
\param[out] data_bits   data bits.
\param[out] stop_bits   stop bits.
\param[out] parity   parity.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_get_attr(td_s32 *data_bits, td_s32 *stop_bits, td_char *parity);

/* *
\brief set uart attribute.
\param[in] data_bits   data bits.
\param[in] stop_bits   stop bits.
\param[in] parity   parity.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_set_attr(td_s32 data_bits, td_s32 stop_bits, td_char parity);

/* *
\brief get uart baud speed.
\param[out] speed   baud speed.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_get_speed(td_s32 *speed);


/* *
\brief set uart baud speed.
\param[in] speed   baud speed.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_set_speed(td_s32 speed);

/* *
\brief set interval of reading data.
\param[in] timeout   interval(ms).
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_set_timeout(td_s32 timeout);

/* *
\brief read data from uart.
\param[out] buff   data buffer.
\param[in] len   buffer length.
\retval number of read data success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_read(td_char *buff, td_u32 len);

/* *
\brief wirte data to uart.
\param[in] buff   data buffer.
\param[in] len   buffer length.
\retval number of written data success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_write(td_char *buff, td_u32 len);

/* *
\brief de-initialize uart.
\retval TD_SUCCESS success.
\retval please refer to the err code definitino of mpi.
*/
td_s32 uapi_uart_deinit(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_UART_H__ */