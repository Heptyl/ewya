/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HDCP_H__
#define __HDCP_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
#define HDMITX_HDCP_DOWNSTREAM_MAX		31
#define HDMITX_HDCP_KSV_BYTE_SIZE		5
#define HDMITX_HDCP_BSTATUS_BYTE_SIZE	2
//------------------------------------------------------------------------------
typedef enum _hdmitx_hdcp_version {
	HDMITX_HDCP_VERSION_NON_COMPLIANT,
	HDMITX_HDCP_VERSION_1_4,
	HDMITX_HDCP_VERSION_2_2,
	HDMITX_HDCP_VERSION_MAX
} hdmitx_hdcp_version;
//------------------------------------------------------------------------------
typedef enum _hdmitx_hdcp_repeater {
	HDMITX_HDCP_NOT_REPEATER,
	HDMITX_HDCP_REPEATER,
	HDMITX_HDCP_REPEATER_MAX
} hdmitx_hdcp_repeater;
//------------------------------------------------------------------------------
typedef enum _hdmitx_hdcp_stream_type {
	HDMITX_HDCP_STREAM_TYPE_0,
	HDMITX_HDCP_STREAM_TYPE_1,
	HDMITX_HDCP_STREAM_TYPE_AUTO,
	HDMITX_HDCP_STREAM_TYPE_MAX
} hdmitx_hdcp_stream_type;
//------------------------------------------------------------------------------
typedef enum _hdmirx_hdcp_version {
	HDMIRX_HDCP_VERSION_NON_COMPLIANT,
	HDMIRX_HDCP_VERSION_1_4,
	HDMIRX_HDCP_VERSION_2_2,
	HDMIRX_HDCP_VERSION_AUTO,
	HDMIRX_HDCP_VERSION_MAX
} hdmirx_hdcp_version;
//------------------------------------------------------------------------------
typedef enum _hdmirx_hdcp_repeater {
	HDMIRX_HDCP_NOT_REPEATER,
	HDMIRX_HDCP_REPEATER,
	HDMIRX_HDCP_REPEATER_MAX
} hdmirx_hdcp_repeater;
//------------------------------------------------------------------------------
typedef enum _hdmirx_hdcp_eess {					// Enhanced Encryption Status Signaling (EESS)
	HDMIRX_HDCP_EESS_NONE,							// none
	HDMIRX_HDCP_EESS_ENC_DIS,						// ENC_DIS (encryption is disabled)
	HDMIRX_HDCP_EESS_ENC_EN,						// ENC_EN (encryption is enabled)
	HDMIRX_HDCP_EESS_MAX
} hdmirx_hdcp_eess;
//------------------------------------------------------------------------------
typedef enum _hdmirx_hdcp_message {					// received:
	HDMIRX_HDCP_MESSAGE_NONE,						// none
	HDMIRX_HDCP_MESSAGE_AKSV,						// Aksv							HDCP1.4
	HDMIRX_HDCP_MESSAGE_AKE_INIT,					// AKE_Init						HDCP2.2
	HDMIRX_HDCP_MESSAGE_AKE_NO_STORED_KM,			// AKE_No_Stored_km				HDCP2.2
	HDMIRX_HDCP_MESSAGE_AKE_STORED_KM,				// AKE_Stored_km				HDCP2.2
	HDMIRX_HDCP_MESSAGE_LC_INIT,					// LC_Init						HDCP2.2
	HDMIRX_HDCP_MESSAGE_SKE_SEND_EKS,				// SKE_Send_Eks					HDCP2.2
	HDMIRX_HDCP_MESSAGE_REPEATERAUTH_SEND_ACK,		// RepeaterAuth_Send_Ack		HDCP2.2
	HDMIRX_HDCP_MESSAGE_REPEATERAUTH_STREAM_MANAGE,	// RepeaterAuth_Stream_Manage	HDCP2.2
	HDMIRX_HDCP_MESSAGE_MAX
} hdmirx_hdcp_message;
//------------------------------------------------------------------------------
typedef enum _hdmirx_hdcp_error {
	HDMIRX_HDCP_ERROR_NONE,
	HDMIRX_HDCP_ERROR_PACKET_HEADER_BCH,
	HDMIRX_HDCP_ERROR_PACKET_BODY_BCH,
	HDMIRX_HDCP_ERROR_MAX
} hdmirx_hdcp_error;
//------------------------------------------------------------------------------
typedef struct _hdmitx_hdcp_config {
	system_config_retry		init_auth;
	system_config_retry		read_rxstatus;
	system_config_retry		stream_manage;
} hdmitx_hdcp_config;
//------------------------------------------------------------------------------
typedef struct _hdmitx_hdcp_auth {
	system_port				port;
	hdmitx_hdcp_version		hdcp_version;
	hdmitx_hdcp_repeater	is_repeater;
	uint8_t					bksv[HDMITX_HDCP_KSV_BYTE_SIZE];	// bksv or receiver ID
} hdmitx_hdcp_auth;
//------------------------------------------------------------------------------
typedef struct _hdmitx_hdcp_repeater_auth {
	system_port				port;
	hdmitx_hdcp_version		hdcp_version;
	hdmitx_hdcp_stream_type	stream_type;
	uint8_t					downstream_count;
	uint8_t					downstream_depth;
	uint8_t					ksv_list[HDMITX_HDCP_DOWNSTREAM_MAX][HDMITX_HDCP_KSV_BYTE_SIZE];
	uint8_t					bstatus[HDMITX_HDCP_BSTATUS_BYTE_SIZE];	// bstatus or rxinfo
} hdmitx_hdcp_repeater_auth;
//------------------------------------------------------------------------------
typedef struct _hdmirx_hdcp_auth {
	system_port				port;
	hdmirx_hdcp_version		port0;
	hdmirx_hdcp_version		port1;
	hdmirx_hdcp_version		port2;
	hdmirx_hdcp_version		port3;
	hdmirx_hdcp_repeater	repeater;
} hdmirx_hdcp_auth;
//------------------------------------------------------------------------------
typedef struct _hdmirx_hdcp_auth_status {
	system_port				port;
	hdmirx_hdcp_repeater	repeater;
	hdmirx_hdcp_eess		eess;
	hdmirx_hdcp_message		message;
	hdmirx_hdcp_error		error;
} hdmirx_hdcp_auth_status;
//------------------------------------------------------------------------------
typedef struct _hdmirx_hdcp_auth_version {
	system_port				port;
	hdmirx_hdcp_version		hdcp_version;
} hdmirx_hdcp_auth_version;
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_configure(hdmitx_hdcp_config *p_conf);
uint32_t hdmitx_hdcp_authentication(hdmitx_hdcp_auth *p_auth);
uint32_t hdmitx_hdcp_repeater_authentication(hdmitx_hdcp_repeater_auth *p_repeater_auth);
uint32_t hdmitx_hdcp_get_authentication_status(hdmitx_hdcp_auth *p_auth);
uint32_t hdmitx_hdcp_authentication_reset(hdmitx_hdcp_auth *p_auth);
uint32_t hdmitx_hdcp_abort(void);
uint32_t hdmirx_hdcp_authentication(hdmirx_hdcp_auth *p_auth);
uint32_t hdmirx_hdcp_get_authentication_status(hdmirx_hdcp_auth_status *p_status);
uint32_t hdmirx_hdcp_get_authentication_version(hdmirx_hdcp_auth_version *p_ver);
uint32_t hdmirx_hdcp_authentication_reset(hdmirx_hdcp_auth *p_auth);
uint32_t hdmirx_hdcp_abort(void);
//------------------------------------------------------------------------------
#pragma pack()

#endif  // __HDCP_H__
