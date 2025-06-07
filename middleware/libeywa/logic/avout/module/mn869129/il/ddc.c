/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "ddc.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
// AKE command
#define DDCTX_WRITE_DDC_CMD									0x80
#define DDCTX_READ_DDC_CMD									0x81
#define DDCTX_READ_EDID_CMD									0x82
#define DDCTX_CHECK_DDC_CMD									0x87
#define DDCTX_RESET_DDC_CMD									0x88
//------------------------------------------------------------------------------
// maximum retry count & retry interval for akecmd
#define DDCTX_AKECMD_ACCESS_RETRY_MAX						10
#define DDCTX_AKECMD_ACCESS_RETRY_INTERVAL					100
#define DDCTX_AKESTA_ACCESS_RETRY_MAX						300
#define DDCTX_AKESTA_ACCESS_RETRY_INTERVAL					10
//------------------------------------------------------------------------------
// ddctx_akecmd()
#define DDCTX_AKECMD_NUM									1
#define DDCTX_AKECMD_MASK									0x000000FF
//------------------------------------------------------------------------------
// ddctx_akeerr()
#define DDCTX_AKESTA_REG_MASK								0xE0
#define DDCTX_AKESTA_NORMAL_END								0xA0
#define DDCTX_AKESTA_ERROR_END								0xC0
//------------------------------------------------------------------------------
// ddctx_read_akesta()
#define DDCTX_AKESTA_NUM									1
#define DDCTX_AKESTA_MASK									0x000000FF
//------------------------------------------------------------------------------
// ddctx_read_akeerr()
#define DDCTX_AKEERR_NUM									1
#define DDCTX_AKEERR_MASK									0x000000FF
//------------------------------------------------------------------------------
// ddctx_reset_ddc()
#define DDCTX_RESET_DDC_NUM									1
#define DDCTX_RESET_DDC_MASK								0x000000FF
//------------------------------------------------------------------------------
// ddctx_check_ddc()
#define DDCTX_CHECK_DDC_NUM									1
#define DDCTX_CHECK_DDC_MASK								0x000000FF
//------------------------------------------------------------------------------
// ddctx_set_ddc_property
#define DDCTX_SET_DDC_PROPERTY_LEN							3
//------------------------------------------------------------------------------
// ddctx_write()
// ddctx_read()
#define DDCTX_DDC_TOP_IN_DDCBUF0_OFFSET						0x00
//------------------------------------------------------------------------------
// hdmitx_read_edid()
#define DDCTX_READ_EDID_NUM									1
#define DDCTX_EDID_AKEMODE_EDIDRS_1							0x84
#define DDCTX_EDID_AKEMODE_MASK								0x000000FF
#define DDCTX_EDID_OFFSET_ADDR_MASK							0xF0
//------------------------------------------------------------------------------
// SCDC slave address
#define DDCTX_SCDC_SLAVE_ADDR								0xA8
//------------------------------------------------------------------------------
// ddctx_select_page_of_0x7c()
#define DDCTX_PAGE_SELECT_0X7C_NUM							1
#define DDCTX_PAGE_SELECT_0X7C_MASK							0x0000000F
#define DDCTX_PAGE_NO_0										0x00
#define DDCTX_PAGE_NO_3										0x03
//------------------------------------------------------------------------------
// ddcrx_set_edid_ope_mode()
#define DDCRX_SET_EDID_OPE_MODE_LEN							4
#define DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_EXTERNAL	0x04
#define DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_INTERNAL	0x05
#define DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_MASK		0x00000007
//------------------------------------------------------------------------------
// ddcrx_select_page_of_0x18()
#define DDCRX_PAGE_SELECT_0X18_NUM							1
#define DDCRX_PAGE_SELECT_0X18_EDID							0x00
#define DDCRX_PAGE_SELECT_0X18_SCDCS						0x10
#define DDCRX_PAGE_SELECT_0X18_SYSCFG14_MASK				0x000000FF
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
// AKE command retry control
typedef struct _akecmd_ctrl {
	uint32_t cmd_retry_max;
	uint32_t cmd_retry_interval;
	uint32_t ack_retry_max;
	uint32_t ack_retry_interval;
} akecmd_ctrl;
//------------------------------------------------------------------------------
static akecmd_ctrl akectrl_accessddc = {
									DDCTX_AKECMD_ACCESS_RETRY_MAX,
									DDCTX_AKECMD_ACCESS_RETRY_INTERVAL,
									DDCTX_AKESTA_ACCESS_RETRY_MAX,
									DDCTX_AKESTA_ACCESS_RETRY_INTERVAL
								};
//------------------------------------------------------------------------------
static uint32_t ddctx_akecmd(system_port port, uint8_t akecmd, akecmd_ctrl *p_akectrl);
static uint32_t ddctx_akeerr(system_port port, akecmd_ctrl *p_akectrl);
static uint32_t ddctx_read_akesta(system_port port, uint8_t *p_sta);
static uint32_t ddctx_read_akeerr(system_port port, uint8_t *p_err);
static uint32_t ddctx_reset_ddc(system_port port);
static uint32_t ddctx_check_ddc(system_port port);
static uint32_t ddctx_set_ddc_property(system_port port, uint8_t saddr, uint8_t oaddr, uint16_t len);
//------------------------------------------------------------------------------
static uint32_t ddctx_write(hdmitx_ddc *p_ddc, uint8_t akecmd);
static uint32_t ddctx_read(hdmitx_ddc *p_ddc, uint8_t akecmd);
//------------------------------------------------------------------------------
static uint32_t ddctx_select_page_of_0x7c(system_port port, uint8_t page);
static uint32_t ddctx_write_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data);
static uint32_t ddctx_read_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data);
static uint32_t ddcrx_set_edid_ope_mode(hdmirx_edid *p_edid);
static uint32_t ddcrx_select_page_of_0x18(system_port port, uint8_t page);
static uint32_t ddcrx_write_edid(system_port port, uint8_t port_num, uint8_t *p_edid);
static uint32_t ddcrx_set_edid(hdmirx_edid *p_edid);
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
extern system_request hdmitx_ctrl_get_abort_req(void);
extern void hdmitx_ctrl_set_abort_req(system_request req);
#endif
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
extern system_request hdmirx_ctrl_get_abort_req(void);
extern void hdmirx_ctrl_set_abort_req(system_request req);
#endif
//------------------------------------------------------------------------------
// abort request check for HDMI Tx
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
#define CHECK_TX_ABORT() {											\
	if (hdmitx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		hdmitx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_HDMITX_ABORT;									\
	}																\
}
#else
#define CHECK_TX_ABORT()
#endif
//------------------------------------------------------------------------------
// abort request check for HDMI Rx
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
	defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
#define CHECK_RX_ABORT() {											\
	if (hdmirx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {		\
		hdmirx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);				\
		return ERR_HDMIRX_ABORT;									\
	}																\
}
#else
#define CHECK_RX_ABORT()
#endif
//------------------------------------------------------------------------------
static uint32_t ddctx_akecmd(system_port port, uint8_t akecmd, akecmd_ctrl *p_akectrl)
{
	system_reg_access access;
	system_command cmd[DDCTX_AKECMD_NUM];
	uint32_t ret;
	uint32_t i;

	if (p_akectrl == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, DDCTX_AKECMD_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKECMD_ADDR, akecmd, DDCTX_AKECMD_MASK);

	for (i=0; i<p_akectrl->cmd_retry_max; i++) {
		CHECK_TX_ABORT();
		ret = lf9129_device_write(&access);
		CHECK_ERROR(ret);
		// check error
		ret = ddctx_akeerr(port, p_akectrl);
		if (ret == ERR_OK) {
			break;
		} else if (ret == ERR_DDCTX_DDC_NO_ACK_REPLY) {
			ret = ddctx_reset_ddc(port);
			CHECK_ERROR(ret);
		} else if (ret == ERR_DDCTX_DDC_FIXED_LOW_LEVEL) {
			ret = ddctx_check_ddc(port);
			CHECK_ERROR(ret);
		} else {
			return ret;
		}
		SLEEP_Delay_ms(p_akectrl->cmd_retry_interval);
	}
	if (i == p_akectrl->cmd_retry_max) {
		return ERR_DDCTX_DDC_ACCESS_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_akeerr(system_port port, akecmd_ctrl *p_akectrl)
{
	uint8_t akesta, akeerr;
	uint32_t ret;
	uint32_t i;

	if (p_akectrl == NULL) {
		return ERR_NULL_POINTER;
	}

	akesta = 0;
	akeerr = 0;
	for (i=0; i<p_akectrl->ack_retry_max; i++) {
		CHECK_TX_ABORT();
		ret = ddctx_read_akesta(port, &akesta);
		CHECK_ERROR(ret);
		if ((akesta & DDCTX_AKESTA_REG_MASK) == DDCTX_AKESTA_NORMAL_END) {
			ret = ERR_OK;
			break;
		} else if ((akesta & DDCTX_AKESTA_REG_MASK) == DDCTX_AKESTA_ERROR_END) {
			ret = ddctx_read_akeerr(port, &akeerr);
			CHECK_ERROR(ret);
			ret = ERR_DDCTX_AKECMD_NORMAL_END | akeerr;
			break;
		} else {
			// retry
		}
		SLEEP_Delay_ms(p_akectrl->ack_retry_interval);
	}
	if (i == p_akectrl->ack_retry_max) {
		ret = ERR_DDCTX_AKESTA_READ_RETRY;
	}

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_read_akesta(system_port port, uint8_t *p_sta)
{
	system_reg_access access;
	system_command cmd[DDCTX_AKESTA_NUM];
	uint32_t ret;

	if (p_sta == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, DDCTX_AKEERR_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKESTA_ADDR, 0, DDCTX_AKESTA_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	*p_sta = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_read_akeerr(system_port port, uint8_t *p_err)
{
	system_reg_access access;
	system_command cmd[DDCTX_AKEERR_NUM];
	uint32_t ret;

	if (p_err == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_ACCESS(access, &port, cmd, DDCTX_AKEERR_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKEERR_ADDR, 0, DDCTX_AKEERR_MASK);
	ret = lf9129_device_read(&access);
	CHECK_ERROR(ret);
	*p_err = cmd[0].data;

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_reset_ddc(system_port port)
{
	system_reg_access access;
	system_command cmd[DDCTX_RESET_DDC_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, DDCTX_RESET_DDC_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKECMD_ADDR, DDCTX_RESET_DDC_CMD, DDCTX_RESET_DDC_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_check_ddc(system_port port)
{
	system_reg_access access;
	system_command cmd[DDCTX_CHECK_DDC_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, DDCTX_CHECK_DDC_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKECMD_ADDR, DDCTX_CHECK_DDC_CMD, DDCTX_CHECK_DDC_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_set_ddc_property(system_port port, uint8_t saddr, uint8_t oaddr, uint16_t len)
{
	system_block_access baccess;
	uint8_t data[DDCTX_SET_DDC_PROPERTY_LEN];
	uint32_t ret;

	if (len == 0 || len > HDMITX_DDC_MAX_BUF_SIZE) {
		return ERR_INVALID_SIZE;
	}

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_DDCSLA_ADDR,
										DDCTX_SET_DDC_PROPERTY_LEN,
										data);
	// slave address
	SET_BLOCK_DATA(data, 0, saddr);							// 0x3A80
	// offset address
	SET_BLOCK_DATA(data, 1, oaddr);							// 0x3A81
	// length (1byte: 0, 2byte: 1, ... 256byte: 255(0xFF))
	SET_BLOCK_DATA(data, 2, (len - 1));						// 0x3A82

	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_write(hdmitx_ddc *p_ddc, uint8_t akecmd)
{
	uint32_t ret;

	if (p_ddc == NULL) {
		return ERR_NULL_POINTER;
	}
	if (p_ddc->length == 0 || p_ddc->length > HDMITX_DDC_MAX_BUF_SIZE) {
		return ERR_INVALID_SIZE;
	}

	// Select DDC buffer page
	ret = ddctx_select_page_of_0x7c(p_ddc->port, DDCTX_PAGE_NO_0);
	CHECK_ERROR(ret);
	// Set data to DDC buffer
	ret = ddctx_write_ddc_buffer(p_ddc->port, DDCTX_DDC_TOP_IN_DDCBUF0_OFFSET,
								p_ddc->length, p_ddc->data);
	CHECK_ERROR(ret);
	// Set DDC slave/offset address and length to write data
	ret = ddctx_set_ddc_property(p_ddc->port, p_ddc->saddr, p_ddc->oaddr, p_ddc->length);
	CHECK_ERROR(ret);
	// Write data to DDC
	ret = ddctx_akecmd(p_ddc->port, akecmd, &akectrl_accessddc);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_read(hdmitx_ddc *p_ddc, uint8_t akecmd)
{
	uint32_t ret;

	if (p_ddc == NULL) {
		return ERR_NULL_POINTER;
	}
	if (p_ddc->length == 0 || p_ddc->length > HDMITX_DDC_MAX_BUF_SIZE) {
		return ERR_INVALID_SIZE;
	}

	// Set DDC slave/offset address and length to read data
	ret = ddctx_set_ddc_property(p_ddc->port, p_ddc->saddr, p_ddc->oaddr, p_ddc->length);
	CHECK_ERROR(ret);
	// Read data from DDC
	ret = ddctx_akecmd(p_ddc->port, akecmd, &akectrl_accessddc);
	CHECK_ERROR(ret);
	// Select DDC buffer page
	ret = ddctx_select_page_of_0x7c(p_ddc->port, DDCTX_PAGE_NO_0);
	CHECK_ERROR(ret);
	// Get data from DDC buffer
	ret = ddctx_read_ddc_buffer(p_ddc->port, DDCTX_DDC_TOP_IN_DDCBUF0_OFFSET,
								p_ddc->length, p_ddc->data);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_select_page_of_0x7c(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[DDCTX_PAGE_SELECT_0X7C_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, DDCTX_PAGE_SELECT_0X7C_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_THDCPG_ADDR, page, DDCTX_PAGE_SELECT_0X7C_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_write_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_data == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_DDCBUF_DDCBUF_ADDR + offset,
						(uint32_t)length, p_data);
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddctx_read_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_data == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_DDCBUF_DDCBUF_ADDR + offset,
						(uint32_t)length, p_data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddcrx_set_edid_ope_mode(hdmirx_edid *p_edid)
{
	system_block_access baccess;
	uint8_t data[DDCRX_SET_EDID_OPE_MODE_LEN] = {0};
	uint32_t ret;

	if (p_edid == NULL) {
		return ERR_NULL_POINTER;
	}

	SET_BLOCK_ACCESS(baccess, &(p_edid->port), VIDEOIF_SYSTEM_SYSCFG10_ADDR,
						DDCRX_SET_EDID_OPE_MODE_LEN, data);
	// read current setting
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);

	// set Rx port 0 (0x1018)
	data[0] = (data[0] & ~DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_MASK)
				| ((p_edid->p_edid0 == NULL)
					? DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_EXTERNAL
					: DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_INTERNAL);
	// set Rx port 1 (0x1019)
	data[1] = (data[1] & ~DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_MASK)
				| ((p_edid->p_edid1 == NULL)
					? DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_EXTERNAL
					: DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_INTERNAL);
	// set Rx port 2 (0x101A)
	data[2] = (data[2] & ~DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_MASK)
				| ((p_edid->p_edid2 == NULL)
					? DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_EXTERNAL
					: DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_INTERNAL);
	// set Rx port 3 (0x101B)
	data[3] = (data[3] & ~DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_MASK)
				| ((p_edid->p_edid3 == NULL)
					? DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_EXTERNAL
					: DDCRX_SET_EDID_OPE_MODE_SYSCFG1X_SYSOPMX_INTERNAL);

	// set EDID operating mode
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddcrx_select_page_of_0x18(system_port port, uint8_t page)
{
	system_reg_access access;
	system_command cmd[DDCRX_PAGE_SELECT_0X18_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, DDCRX_PAGE_SELECT_0X18_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_SYSCFG14_ADDR,
						page + port.port_num,
						DDCRX_PAGE_SELECT_0X18_SYSCFG14_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddcrx_write_edid(system_port port, uint8_t port_num, uint8_t *p_edid)
{
	system_block_access baccess;
	uint32_t ret;

	if (p_edid == NULL) {
		return ERR_NULL_POINTER;
	}

	port.port_num = port_num;
	ret = ddcrx_select_page_of_0x18(port, DDCRX_PAGE_SELECT_0X18_EDID);
	CHECK_ERROR(ret);

	// page: 10, 11, 12, 13 (according to port.port_num)

	// write EDID
	SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_EDIDRX_EDIDRX0_ADDR,
						HDMIRX_EDID_MAX_BUF_SIZE, p_edid);
	ret = device_block_write(&baccess);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t ddcrx_set_edid(hdmirx_edid *p_edid)
{
	uint32_t ret = ERR_OK;

	if (p_edid == NULL) {
		return ERR_NULL_POINTER;
	}

	CHECK_RX_ABORT();
	// write internal EDID of Rx port 0
	if (p_edid->p_edid0 != NULL) {
		ret = ddcrx_write_edid(p_edid->port, SYSTEM_PORT_NUMBER_0, p_edid->p_edid0);
		CHECK_ERROR(ret);
	}
	CHECK_RX_ABORT();
	// write internal EDID of Rx port 1
	if (p_edid->p_edid1 != NULL) {
		ret = ddcrx_write_edid(p_edid->port, SYSTEM_PORT_NUMBER_1, p_edid->p_edid1);
		CHECK_ERROR(ret);
	}
	CHECK_RX_ABORT();
	// write internal EDID of Rx port 2
	if (p_edid->p_edid2 != NULL) {
		ret = ddcrx_write_edid(p_edid->port, SYSTEM_PORT_NUMBER_2, p_edid->p_edid2);
		CHECK_ERROR(ret);
	}
	CHECK_RX_ABORT();
	// write internal EDID of Rx port 3
	if (p_edid->p_edid3 != NULL) {
		ret = ddcrx_write_edid(p_edid->port, SYSTEM_PORT_NUMBER_3, p_edid->p_edid3);
		CHECK_ERROR(ret);
	}

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_write_ddc(hdmitx_ddc *p_ddc)
{
	uint32_t ret;

	if (p_ddc == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_ddc->port);
	CHECK_ERROR(ret);

	ret = ddctx_write(p_ddc, DDCTX_WRITE_DDC_CMD);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_read_ddc(hdmitx_ddc *p_ddc)
{
	uint32_t ret;

	if (p_ddc == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_ddc->port);
	CHECK_ERROR(ret);

	ret = ddctx_read(p_ddc, DDCTX_READ_DDC_CMD);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_write_scdc(hdmitx_scdc *p_scdc)
{
	uint32_t ret;

	if (p_scdc == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_scdc->port);
	CHECK_ERROR(ret);

	p_scdc->saddr = DDCTX_SCDC_SLAVE_ADDR;
	ret = ddctx_write((hdmitx_ddc *)p_scdc, DDCTX_WRITE_DDC_CMD);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_read_scdc(hdmitx_scdc *p_scdc)
{
	uint32_t ret;

	if (p_scdc == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_scdc->port);
	CHECK_ERROR(ret);

	p_scdc->saddr = DDCTX_SCDC_SLAVE_ADDR;
	ret = ddctx_read((hdmitx_ddc *)p_scdc, DDCTX_READ_DDC_CMD);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_read_edid(hdmitx_edid *p_edid)
{
	system_reg_access access;
	system_command cmd[DDCTX_READ_EDID_NUM];
	uint32_t ret;

	if (p_edid == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_edid->port);
	CHECK_ERROR(ret);

	// EDID Read Mode (offset mode)
	SET_ACCESS(access, &(p_edid->port), cmd, DDCTX_READ_EDID_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKEMODE_ADDR,
						DDCTX_EDID_AKEMODE_EDIDRS_1,
						DDCTX_EDID_AKEMODE_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	p_edid->oaddr = p_edid->oaddr & DDCTX_EDID_OFFSET_ADDR_MASK;
	ret = ddctx_read((hdmitx_ddc *)p_edid, DDCTX_READ_EDID_CMD);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_write_edid(hdmirx_edid *p_edid)
{
	uint32_t ret;

	if (p_edid == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmirx_init()
	ret = system_ctrl_check_hdmirx_init(p_edid->port);
	CHECK_ERROR(ret);
	// set EDID operating mode
	ret = ddcrx_set_edid_ope_mode(p_edid);
	CHECK_ERROR(ret);
	// set EDID
	ret = ddcrx_set_edid(p_edid);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
