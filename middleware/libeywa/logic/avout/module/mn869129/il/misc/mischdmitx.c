/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "mischdmitx.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "reg_access.h"
#include "videoif_regs.h"
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL												0
#endif // NULL
//------------------------------------------------------------------------------
#define HDMITX_AVI_INFOFRAME_LEN							5
#define HDMITX_SET_AVI_INFOFRAME_DIS						0xFF
#define HDMITX_SET_INFENA_NUM								1
#define HDMITX_INFENA_NONE									0x00
#define HDMITX_INFENA_AVIEN									0x40
#define HDMITX_UPDATE_INFOFRAME_NUM							1
#define HDMITX_UPDATE_INFOFRAME_PKTUPD_00_ALL_CLEAR			0x00
#define HDMITX_UPDATE_INFOFRAME_PKTUPD_00_MASK				0x000000FF
#define HDMITX_PKTUPD_AVIUPD								0x40
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
extern uint32_t hdmitx_set_video_format_and_sg(hdmitx_video *p_video, system_sg *p_sg);
//------------------------------------------------------------------------------
// hdmitx_set_sg_format
//   sets the supported video format as signal generator for HDMI Tx
//
//   Prototype:
//     uint32_t hdmitx_set_sg_format(hdmitx_sg_video *p_video)
//
//   Parameters:
//     p_video    Pointer to structure of hdmitx_sg_video to hold video format property
//
//   Returns:
//     ERR_OK if successful, otherwise a system error code depending on the function.
//
//   Description:
//     This function sets the video format supported as a signal generator for HDMI Tx.
//     This function must be called after hdmitx_init function is called.
//     The supported video format as signal generator is as follows.
//       system_video_format:
//         SYSTEM_VFORM_720x480P_5994or60_4v3
//         SYSTEM_VFORM_720x480P_5994or60_16v9
//         SYSTEM_VFORM_1280x720P_50_16v9
//         SYSTEM_VFORM_1280x720P_5994or60_16v9
//         SYSTEM_VFORM_1920x1080P_2398or24_16v9
//         SYSTEM_VFORM_1920x1080P_25_16v9
//         SYSTEM_VFORM_1920x1080P_2997or30_16v9
//         SYSTEM_VFORM_1920x1080P_50_16v9
//         SYSTEM_VFORM_1920x1080P_5994or60_16v9
//         SYSTEM_VFORM_3840x2160P_2398or24_16v9
//         SYSTEM_VFORM_3840x2160P_25_16v9
//         SYSTEM_VFORM_3840x2160P_2997or30_16v9
//         SYSTEM_VFORM_3840x2160P_50_16v9
//         SYSTEM_VFORM_3840x2160P_5994or60_16v9
//         SYSTEM_VFORM_4096x2160P_2398or24_256v135
//         SYSTEM_VFORM_4096x2160P_25_256v135
//         SYSTEM_VFORM_4096x2160P_2997or30_256v135
//         SYSTEM_VFORM_4096x2160P_50_256v135
//         SYSTEM_VFORM_4096x2160P_5994or60_256v135
//       system_color_format:
//         SYSTEM_COLOR_YCBCR444_ITU_R_BT709
//         SYSTEM_COLOR_RGB_FULL_RANGE1
//       system_deep_color:
//         SYSTEM_DEEP_COLOR_8BIT
//
//   Examples:
//     system_port port = {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI,
//                         SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0};
//     hdmitx_sg_video vform;
//     vform.port = port;
//     vform.video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
//     vform.color_format = SYSTEM_COLOR_RGB_FULL_RANGE1;
//     vform.deep_color = SYSTEM_DEEP_COLOR_8BIT;
//     vform.sg_format = SYSTEM_SG_FORMAT_COLOR_BAR_RGB;
//     vform.dvi_mode = HDMITX_DVI_MODE_DISABLE;
//     hdmitx_set_sg_format(&vform);
//------------------------------------------------------------------------------
uint32_t hdmitx_set_sg_format(hdmitx_sg_video *p_video)
{
	hdmitx_video vf;
	system_sg sg;
	uint32_t ret;

	if (p_video == NULL) {
		return ERR_NULL_POINTER;
	}

	vf.rx_port = vf.tx_port = p_video->port;
	vf.rx_video_format = vf.tx_video_format = p_video->video_format;
	vf.rx_color_format = vf.tx_color_format = p_video->color_format;
	vf.rx_deep_color = vf.tx_deep_color = p_video->deep_color;
	vf.tx_dvi_mode = p_video->dvi_mode;
	sg.onoff = SYSTEM_ONOFF_ON;
	sg.sg_format = p_video->sg_format;

	ret = hdmitx_set_video_format_and_sg(&vf, &sg);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_set_infena_misc(system_port port, uint8_t infena, uint32_t mask)
{
	system_reg_access access;
	system_command cmd[HDMITX_SET_INFENA_NUM];
	uint32_t ret;

	SET_ACCESS(access, &port, cmd, HDMITX_SET_INFENA_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_INFENA_ADDR,
						infena,
						mask);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmitx_update_infoframe_misc(system_port port, uint8_t upd)
{
	system_reg_access access;
	system_command cmd[HDMITX_UPDATE_INFOFRAME_NUM];
	uint32_t ret;

	// infoframe update
	SET_ACCESS(access, &port, cmd, HDMITX_UPDATE_INFOFRAME_NUM);
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTUPD_00_ADDR,
						upd,
						HDMITX_UPDATE_INFOFRAME_PKTUPD_00_MASK);
	ret = lf9129_device_write(&access);
	CHECK_ERROR(ret);
	// check autocleared
	SET_COMMAND(cmd, 0, VIDEOIF_INF_PKTUPD_00_ADDR,
						HDMITX_UPDATE_INFOFRAME_PKTUPD_00_ALL_CLEAR,
						HDMITX_UPDATE_INFOFRAME_PKTUPD_00_MASK);
	ret = device_read_wait(&access);
	CHECK_ERROR(ret);

	return ret;
}
//------------------------------------------------------------------------------
// hdmitx_set_avi_infoframe_misc
//   sets AVI InfoFrame
//
//   Prototype:
//     uint32_t hdmitx_set_avi_infoframe_misc(hdmitx_avi_infoframe *p_avi);
//
//   Parameters:
//     p_avi    Pointer to structure of hdmitx_avi_infoframe to hold the AVI InfoFrame
//
//   Returns:
//     ERR_OK if successful, otherwise a system error code depending on the function.
//
//   Description:
//     This function sets the AVI InfoFrame according to the parameters that are set
//     to the members of p_avi. When p_avi->vic is set to 0xFF, the AVI InfoFrame can
//     be disabled.
//     This function must be called after hdmitx_set_video_format function is called.
//     NOTE: Originally, AVI InfoFrame is set by calling hdmitx_set_video_format
//           function, but this function is used when additional setting is required.
//------------------------------------------------------------------------------
uint32_t hdmitx_set_avi_infoframe_misc(hdmitx_avi_infoframe *p_avi)
{
	system_block_access baccess;
	uint8_t data[HDMITX_AVI_INFOFRAME_LEN];
	uint32_t ret;

	if (p_avi == NULL) {
		return ERR_NULL_POINTER;
	}
	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_avi->port);
	CHECK_ERROR(ret);

	if (p_avi->vic == HDMITX_SET_AVI_INFOFRAME_DIS) {
		// disable AVI infoframe packet
		ret = hdmitx_set_infena_misc(p_avi->port, HDMITX_INFENA_NONE, HDMITX_INFENA_AVIEN);
		CHECK_ERROR(ret);
	} else {
		// 0x3224 - 0x3228
		SET_BLOCK_ACCESS(baccess, &(p_avi->port),
							VIDEOIF_INF_AVIINF_04_ADDR, HDMITX_AVI_INFOFRAME_LEN, data);
		SET_BLOCK_DATA(data, 0, p_avi->databyte1);		// 0x3224
		SET_BLOCK_DATA(data, 1, p_avi->databyte2);		// 0x3225
		SET_BLOCK_DATA(data, 2, p_avi->databyte3);		// 0x3226
		SET_BLOCK_DATA(data, 3, p_avi->databyte4);		// 0x3227
		SET_BLOCK_DATA(data, 4, p_avi->databyte5);		// 0x3228
		ret = device_block_write(&baccess);
		CHECK_ERROR(ret);
		// update AVI infoframe packet (autocleared)
		ret = hdmitx_update_infoframe_misc(p_avi->port, HDMITX_PKTUPD_AVIUPD);
		CHECK_ERROR(ret);
		// enable AVI infoframe packet
		ret = hdmitx_set_infena_misc(p_avi->port, HDMITX_INFENA_AVIEN, HDMITX_INFENA_AVIEN);
		CHECK_ERROR(ret);
	}

	return ERR_OK;
}
//------------------------------------------------------------------------------
// hdmitx_get_avi_infoframe
//   gets AVI InfoFrame
//
//   Prototype:
//     uint32_t hdmitx_get_avi_infoframe(hdmitx_avi_infoframe *p_avi);
//
//   Parameters:
//     p_avi    Pointer to structure of hdmitx_avi_infoframe to hold the AVI InfoFrame
//
//   Returns:
//     ERR_OK if successful, otherwise a system error code depending on the function.
//
//   Description:
//     This function gets the AVI InfoFrame that has already been set.
//     This function must be called after hdmitx_set_video_format function is called.
//------------------------------------------------------------------------------
uint32_t hdmitx_get_avi_infoframe(hdmitx_avi_infoframe *p_avi)
{
	system_block_access baccess;
	uint8_t data[HDMITX_AVI_INFOFRAME_LEN];
	uint32_t ret;

	if (p_avi == NULL) {
		return ERR_NULL_POINTER;
	}

	// check hdmitx_init()
	ret = system_ctrl_check_hdmitx_init(p_avi->port);
	CHECK_ERROR(ret);
	// 0x3224 - 0x3228
	SET_BLOCK_ACCESS(baccess, &(p_avi->port),
						VIDEOIF_INF_AVIINF_04_ADDR, HDMITX_AVI_INFOFRAME_LEN, data);
	ret = device_block_read(&baccess);
	CHECK_ERROR(ret);
	p_avi->databyte1 = data[0];
	p_avi->databyte2 = data[1];
	p_avi->databyte3 = data[2];
	p_avi->databyte4 = data[3];
	p_avi->databyte5 = data[4];

	return ERR_OK;
}
//------------------------------------------------------------------------------
