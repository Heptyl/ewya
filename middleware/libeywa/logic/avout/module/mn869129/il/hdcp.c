/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include "hdcp.h"
#include "ddc.h"
#include "system_ctrl.h"
#include "hv_error.h"
#include "hv_sleep.h"
#include "hv_version.h"
#include "reg_access.h"
#include "videoif_regs.h"
#include "hv_print.h"
//------------------------------------------------------------------------------
// This is used to prevent READY bit timeout for 5 seconds in the second part of
// HDCP1.4 authentication. When this is enabled, white/snow noise can be detected
// by verifying Ri before the second part of HDCP1.4 authentication.
// However, do not enable this in Repeater equipment.
// #define CONFIG_HDCP14_VERIFY_RI_BEFORE_SECOND

extern int current_hdcp_version;
extern uint8_t xbh_9129_is_repeater;


//------------------------------------------------------------------------------
// Primary Link HDCP Port
#define HDCPTXXX_SLAVE_ADDR                                        0x74
#define HDCPTX14_OFFSET_ADDR_BKSV_5_R                              0x00
#define HDCPTX14_OFFSET_ADDR_RID_2_R                               0x08
#define HDCPTX14_OFFSET_ADDR_PJD_1_R                               0x0A
#define HDCPTX14_OFFSET_ADDR_AKSV_5_W                              0x10
#define HDCPTX14_OFFSET_ADDR_AINFO_1_W                             0x15
#define HDCPTX14_OFFSET_ADDR_AN_8_W                                0x18
#define HDCPTX14_OFFSET_ADDR_VDH0_4_R                              0x20
#define HDCPTX14_OFFSET_ADDR_VDH1_4_R                              0x24
#define HDCPTX14_OFFSET_ADDR_VDH2_4_R                              0x28
#define HDCPTX14_OFFSET_ADDR_VDH3_4_R                              0x2C
#define HDCPTX14_OFFSET_ADDR_VDH4_4_R                              0x30
#define HDCPTX14_OFFSET_ADDR_BCAPS_1_R                             0x40
#define HDCPTX14_OFFSET_ADDR_BSTATUS_2_R                           0x41
#define HDCPTX14_OFFSET_ADDR_KSVFIFO_1_R                           0x43
#define HDCPTXXX_OFFSET_ADDR_DBG_64_RW                             0xC0
#define HDCPTX22_OFFSET_ADDR_HDCP2VERSION_1_R                      0x50
#define HDCPTX22_OFFSET_ADDR_WRITE_MESSAGE_1_W                     0x60
#define HDCPTX22_OFFSET_ADDR_RXSTATUS_2_R                          0x70
#define HDCPTX22_OFFSET_ADDR_READ_MESSAGE_1_R                      0x80
// AKE command (Tx)
#define HDCPTX14_WRITE_DDC_CMD                                     0x80
#define HDCPTX14_READ_DDC_CMD                                      0x81
#define HDCPTX14_AN_GEN_START_CMD                                  0x83
#define HDCPTX14_AN_GEN_STOP_CMD                                   0x84
#define HDCPTX14_R0_CALC_CMD                                       0x85
#define HDCPTX14_R0_R0D_COMPARE_CMD                                0x86
#define HDCPTX14_CHECK_DDC_CMD                                     0x87
#define HDCPTX14_RESET_DDC_CMD                                     0x88
#define HDCPTX14_RI_RID_COMAPRE_CMD                                0x8E
#define HDCPTX14_KSVLIST_READ_CMD                                  0xC4
#define HDCPTX14_CALC_V_CMD                                        0xC8
#define HDCPTX14_VERIFY_V_VD_CMD                                   0xD0
#define HDCPTX22_READ_DDC_CMD                                      0x40
#define HDCPTX22_CHECK_DDC_CMD                                     0x49
#define HDCPTX22_RESET_DDC_CMD                                     0x4A
#define HDCPTX22_GENERATE_RANDOM_SEED_CMD                          0x4B
#define HDCPTX22_GENERATE_RANDOM_SEED_SUBCMD                       0x01
#define HDCPTX22_REPEATERAUTH_SEND_RECEIVERID_LIST_CMD             0x57
#define HDCPTX22_REPEATERAUTH_SEND_RECEIVERID_LIST_SUBCMD          0x1F
#define HDCPTX22_REPEATERAUTH_SEND_ACK_CMD                         0x57
#define HDCPTX22_REPEATERAUTH_SEND_ACK_SUBCMD                      0x60
#define HDCPTX22_AKE_INIT_CMD                                      0x60
#define HDCPTX22_VERIFY_SIGNATURE_CMD                              0x61
#define HDCPTX22_AKE_NO_STORED_KM_CMD                              0x62
#define HDCPTX22_AKE_PAIRING_INFO_CMD                              0x63
#define HDCPTX22_AKE_STORED_KM_CMD                                 0x64
#define HDCPTX22_LC_INIT_CMD                                       0x65
#define HDCPTX22_SKE_SEND_EKS_CMD                                  0x66
#define HDCPTX22_REPEATERAUTH_STREAM_MANAGE_CMD                    0x68
#define HDCPTX22_CHECK_REAUTH_REQ_CMD                              0x69
// maximum retry count & retry interval for akecmd
#define HDCPTX14_AKECMD_ACCESS_RETRY_MAX                           1
#define HDCPTX14_AKECMD_ACCESS_RETRY_INTERVAL                      100
#define HDCPTX14_AKESTA_ACCESS_RETRY_MAX                           300
#define HDCPTX14_AKESTA_ACCESS_RETRY_INTERVAL                      10
#define HDCPTX14_AKECMD_PROCESS_RETRY_MAX                          10
#define HDCPTX14_AKECMD_PROCESS_RETRY_INTERVAL                     10
#define HDCPTX14_AKESTA_PROCESS_RETRY_MAX                          300
#define HDCPTX14_AKESTA_PROCESS_RETRY_INTERVAL                     1
#define HDCPTX14_AKECMD_2ND_AUTH_RETRY_MAX                         2
#define HDCPTX14_AKECMD_2ND_AUTH_RETRY_INTERVAL                    200
#define HDCPTX14_AKESTA_2ND_AUTH_RETRY_MAX                         50
#define HDCPTX14_AKESTA_2ND_AUTH_RETRY_INTERVAL                    100
#define HDCPTX22_AKECMD_LC_INIT_RETRY_MAX                          1
#define HDCPTX22_AKECMD_LC_INIT_RETRY_INTERVAL                     1
#define HDCPTX22_AKESTA_LC_INIT_RETRY_MAX                          300
#define HDCPTX22_AKESTA_LC_INIT_RETRY_INTERVAL                     10
#define HDCPTX22_AKECMD_AKE_INIT_ACCESS_RETRY_MAX                  1
#define HDCPTX22_AKECMD_AKE_INIT_ACCESS_RETRY_INTERVAL             200
#define HDCPTX22_AKESTA_AKE_INIT_ACCESS_RETRY_MAX                  300
#define HDCPTX22_AKESTA_AKE_INIT_ACCESS_RETRY_INTERVAL             10
#define HDCPTX22_AKECMD_STREAM_MANAGE_RETRY_MAX                    1
#define HDCPTX22_AKECMD_STREAM_MANAGE_RETRY_INTERVAL               10
#define HDCPTX22_AKESTA_STREAM_MANAGE_RETRY_MAX                    300
#define HDCPTX22_AKESTA_STREAM_MANAGE_RETRY_INTERVAL               10
// Bstatus, RxInfo
#define HDCPTX14_BSTATUS_DEVICE_COUNT_MASK                         0x7F
#define HDCPTX14_BSTATUS_DEPTH_MASK                                0x07
#define HDCPTX14_BSTATUS_DEVICE_COUNT_MAX                          16        // not HDMITX_HDCP_DOWNSTREAM_MAX
#define HDCPTX14_BSTATUS_MAX_DEVS_EXCEEDED_0                       0x00
#define HDCPTX14_BSTATUS_MAX_DEVS_EXCEEDED_1                       0x80
#define HDCPTX14_BSTATUS_DEPTH_MAX                                 7
#define HDCPTX14_BSTATUS_MAX_CASCADE_EXCEEDED_0                    0x00
#define HDCPTX14_BSTATUS_MAX_CASCADE_EXCEEDED_1                    0x08
#define HDCPTX14_BSTATUS_HDMI_MODE_DVI                             0x00
#define HDCPTX14_BSTATUS_HDMI_MODE_HDMI                            0x10
#define HDCPTX14_BSTATUS_HDMI_MODE_MASK                            0x10
#define HDCPTX22_RXINFO_DEVICE_COUNT_BIT                           0x04
#define HDCPTX22_RXINFO_DEVICE_COUNT_7_4_MASK                      0x0F
#define HDCPTX22_RXINFO_DEVICE_COUNT_8_8_MASK                      0x10
#define HDCPTX22_RXINFO_DEPTH_BIT                                  0x01
#define HDCPTX22_RXINFO_DEPTH_11_9_MASK                            0x07
#define HDCPTX22_RXINFO_DEVICE_COUNT_MAX                           HDMITX_HDCP_DOWNSTREAM_MAX
#define HDCPTX22_RXINFO_DEPTH_MAX                                  4
#define HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_0                  0x00
#define HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_1                  0x01
#define HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_MASK               0x01
#define HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_0              0x00
#define HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_1              0x02
#define HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_MASK           0x02
#define HDCPTX22_RXINFO_MAX_CASCADE_EXCEEDED_0                     0x00
#define HDCPTX22_RXINFO_MAX_CASCADE_EXCEEDED_1                     0x04
#define HDCPTX22_RXINFO_MAX_DEVS_EXCEEDED_0                        0x00
#define HDCPTX22_RXINFO_MAX_DEVS_EXCEEDED_1                        0x08
// byte size
#define HDCPTX14_AN_BYTE_SIZE                                      8
#define HDCPTX14_AINFO_BYTE_SIZE                                   1
#define HDCPTX14_KSV_BYTE_SIZE                                     HDMITX_HDCP_KSV_BYTE_SIZE
#define HDCPTX14_BCAPS_BYTE_SIZE                                   1
#define HDCPTX14_BSTATUS_BYTE_SIZE                                 HDMITX_HDCP_BSTATUS_BYTE_SIZE
#define HDCPTX22_RANDOM_SEED_BYTE_SIZE                             16
#define HDCPTX22_HDCP2VERSION_BYTE_SIZE                            1
#define HDCPTX22_RECEIVER_ID_BYTE_SIZE                             HDMITX_HDCP_KSV_BYTE_SIZE
#define HDCPTX22_RXCAPS_BYTE_SIZE                                  3
#define HDCPTX22_RXSTATUS_BYTE_SIZE                                2
#define HDCPTX22_RXINFO_BYTE_SIZE                                  HDMITX_HDCP_BSTATUS_BYTE_SIZE
#define HDCPTX22_SEQ_NUM_M_BYTE_SIZE                               3
#define HDCPRX22_MSG_ID_BYTE_SIZE                                  1
#define HDCPTX22_SEQ_NUM_V_BYTE_SIZE                               3
#define HDCPTX22_VD_BYTE_SIZE                                      16
#define HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE                      4
#define HDCPRX22_VD_CALC_DATA_MAX_BYTE_SIZE                        (HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE \
                                                                    + (HDCPTX22_RXINFO_DEVICE_COUNT_MAX \
                                                                    * HDCPTX22_RECEIVER_ID_BYTE_SIZE) \
                                                                    + HDCPTX22_RXINFO_BYTE_SIZE \
                                                                    + HDCPTX22_SEQ_NUM_V_BYTE_SIZE)
#define HDCPRX22_RXIDLIST_MSG_BYTE_SIZE                            (HDCPRX22_MSG_ID_BYTE_SIZE \
                                                                    + HDCPTX22_RXINFO_BYTE_SIZE \
                                                                    + HDCPTX22_SEQ_NUM_V_BYTE_SIZE \
                                                                    + HDCPTX22_VD_BYTE_SIZE \
                                                                    + (HDCPTX22_RXINFO_DEVICE_COUNT_MAX \
                                                                    * HDCPTX22_RECEIVER_ID_BYTE_SIZE))
#define HDCPTX22_K_BYTE_SIZE                                       2
// bit assign of bcaps for HDCP1.4
#define HDCPTX14_BCAPS_REPEATER_BIT                                0x40
// bit assign of HDCP2Version for HDCP2.2
#define HDCPTX22_HDCP2VERSION_SUPPORTED_HDCP22_BIT                 0x04
// bit assign of RxCaps for HDCP2.2
#define HDCPTX22_RXCAPS_REPEATER_BIT                               0x01
// bit assign of RxStatus[2] for HDCP2.2
#define HDCPTX22_RXSTATUS_INDEX__7__0                              0
#define HDCPTX22_RXSTATUS_INDEX_15__8                              1
#define HDCPTX22_RXSTATUS_15__8_REAUTH_REQ_BIT                     0x08
#define HDCPTX22_RXSTATUS_15__8_READY_BIT                          0x04
// hdcptx14_akecmd()
#define HDCPTX14_AKECMD_NUM                                        1
#define HDCPTX14_AKECMD_MASK                                       0x000000FF
// hdcptx14_akeerr()
#define HDCPTX14_AKESTA_REG_MASK                                   0xE0
#define HDCPTX14_AKESTA_NORMAL_END                                 0xA0
#define HDCPTX14_AKESTA_ERROR_END                                  0xC0
// hdcptx14_read_akesta()
#define HDCPTX14_AKESTA_NUM                                        1
#define HDCPTX14_AKESTA_MASK                                       0x000000FF
// hdcptx14_read_akeerr()
#define HDCPTX14_AKEERR_NUM                                        1
#define HDCPTX14_AKEERR_MASK                                       0x000000FF
// hdcptx14_reset_ddc()
#define HDCPTX14_RESET_DDC_NUM                                     1
#define HDCPTX14_RESET_DDC_MASK                                    0x000000FF
// hdcptx14_check_ddc()
#define HDCPTX14_CHECK_DDC_NUM                                     1
#define HDCPTX14_CHECK_DDC_MASK                                    0x000000FF
//hdcptx14_set_ddc_property
#define HDCPTX14_SET_DDC_PROPERTY_LEN                              3
// hdcptx14_get_an_from_register()
#define HDCPTX14_AN_GET_REGISTER_LEN                               HDCPTX14_AN_BYTE_SIZE
// hdcptx14_get_ainfo()
#define HDCPTX14_AINFO_GET_REGISTER_NUM                            1
#define HDCPTX14_AINFO_GET_REGISTER_MASK                           0x000000FF
// hdcptx14_get_aksv_from_register()
#define HDCPTX14_AKSV_GET_REGISTER_LEN                             HDCPTX14_KSV_BYTE_SIZE
// hdcptx14_set_bksv_to_register()
#define HDCPTX14_BKSV_SET_REGISTER_LEN                             HDCPTX14_KSV_BYTE_SIZE
// hdcptx14_set_bcaps_to_register()
#define HDCPTX14_BCAPS_SET_REGISTER_NUM                            1
#define HDCPTX14_BCAPS_SET_REGISTER_MASK                           0x000000FF
// hdcptx14_get_bcaps_from_register()
#define HDCPTX14_GET_BCAPS_FROM_REGISTER_NUM                       1
#define HDCPTX14_GET_BCAPS_FROM_REGISTER_MASK                      0x000000FF
// hdcptx14_set_bstatus_to_register()
#define HDCPTX14_BSTATUS_SET_REGISTER_LEN                          HDCPTX14_BSTATUS_BYTE_SIZE
// hdcptx14_get_bstatus_from_register()
#define HDCPTX14_BSTATUS_GET_REGISTER_LEN                          HDCPTX14_BSTATUS_BYTE_SIZE
// hdcptx14_set_cmpfrm_cmplin()
#define HDCPTX14_SET_CMPFRM_CMPLIN_NUM                             1
#define HDCPTX14_SET_CMPFRM_CMPLIN_MASK                            0x000000FF
#define HDCPTX14_SET_CMPFRM_CMPLIN_128F_3S4L                       0x9F
//------------------------------------------------------------------------------
// hdcptx22_akecmd()
#define HDCPTX22_AKECMD_NUM                                        1
#define HDCPTX22_AKECMD_MASK                                       0x000000FF
// hdcptx22_akeerr()
#define HDCPTX22_AKESTA_REG_MASK                                   0xE0
#define HDCPTX22_AKESTA_NORMAL_END                                 0xA0
#define HDCPTX22_AKESTA_ERROR_END                                  0xC0
// hdcptx22_read_akesta()
#define HDCPTX22_AKESTA_NUM                                        1
#define HDCPTX22_AKESTA_MASK                                       0x000000FF
// hdcptx22_read_akeerr()
#define HDCPTX22_AKEERR_NUM                                        1
#define HDCPTX22_AKEERR_MASK                                       0x000000FF
// hdcptx22_reset_ddc()
#define HDCPTX22_RESET_DDC_NUM                                     1
#define HDCPTX22_RESET_DDC_MASK                                    0x000000FF
// hdcptx22_check_ddc()
#define HDCPTX22_CHECK_DDC_NUM                                     1
#define HDCPTX22_CHECK_DDC_MASK                                    0x000000FF
//hdcptx22_set_ddc_property
#define HDCPTX22_SET_DDC_PROPERTY_LEN                              3
// hdcptx22_set_ddc_subcmd()
#define HDCPTX22_SET_DDC_SUBCMD_NUM                                1
#define HDCPTX22_SET_DDC_SUBCMD_MASK                               0x000000FF
// hdcptx22_generate_random_seed()
#define HDCPTX22_GENERATE_RANDOM_SEED_LOOP                         32
// hdcptx22_set_random_seed_to_register()
#define HDCPTX22_RANDOM_SEED_SET_REGISTER_LEN                      HDCPTX22_RANDOM_SEED_BYTE_SIZE
// hdcptx22_get_random_seed_from_register()
#define HDCPTX22_RANDOM_SEED_GET_REGISTER_LEN                      HDCPTX22_RANDOM_SEED_BYTE_SIZE
// hdcptx22_ake2rs()
#define HDCPTX22_AKE2RS_NUM                                        1
#define HDCPTX22_AKE2RS_MASK                                       0x000000FF
#define HDCPTX22_AKE2RS_SUSPEND                                    0x02
#define HDCPTX22_AKE2RS_RESET                                      0x01
// hdcptx22_set_txcaps_to_register()
#define HDCPTX22_SET_TXCAPS_REGISTER_LEN                           3
#define HDCPTX22_TXCAPS_IN_DDCBUF3_OFFSET01                        0x50
#define HDCPTX22_TXCAPS_DATA01                                     0x02
#define HDCPTX22_TXCAPS_DATA02                                     0x00
#define HDCPTX22_TXCAPS_DATA03                                     0x00
// hdcptx22_get_tx_receiver_id()
#define HDCPTX22_GET_TX_RECEIVER_ID_LEN                            HDCPTX22_RECEIVER_ID_BYTE_SIZE
#define HDCPTX22_TX_RECEIVER_ID_IN_DDCBUF3_OFFSET                  0x58
// hdcptx22_get_stored_receiver_id()
#define HDCPTX22_GET_STORED_RECEIVER_ID_LEN                        HDCPTX22_RECEIVER_ID_BYTE_SIZE
#define HDCPTX22_STORED_RECEIVER_ID_IN_DDCBUF3_OFFSET              0x80
// hdcptx22_check_master_key_km()
#define HDCPTX22_CHECK_MASTER_KEY_KM_STORED                        1
#define HDCPTX22_CHECK_MASTER_KEY_KM_NOT_STORED                    0
// hdcptx22_lc_init()
#define HDCPTX22_LC_INIT_RETRY_MAX                                 10
// hdcptx22_read_rxcaps()
#define HDCPTX22_READ_RXCAPS_LEN                                   HDCPTX22_RXCAPS_BYTE_SIZE
#define HDCPTX22_RXCAPS_IN_DDCBUF3_OFFSET                          0x54
#define HDCPTX22_RXCAPS_INDEX_23_16                                0
#define HDCPTX22_RXCAPS_INDEX_15__8                                1
#define HDCPTX22_RXCAPS_INDEX__7__0                                2
// hdcptx22_get_rxinfo()
#define HDCPTX22_RXINFO_IN_DDCBUF0_OFFSET                          0x01
// hdcptx22_get_seq_num_v_from_ddc_buffer()
#define HDCPTX22_SEQ_NUM_V_IN_DDCBUF0_OFFSET                       0x03
#define HDCPTX22_SEQ_NUM_V_00                                      0x00
#define HDCPTX22_SEQ_NUM_V_FF                                      0xFF
// hdcptx22_get_ksvlist_from_ddc_buffer()
#define HDCPTX22_RECEIVER_ID_LIST_IN_DDCBUF0_OFFSET                0x16
// hdcptx22_repeaterauth_stream_manage()
#define HDCPTX22_MSG_ID_IN_DDCBUF0_OFFSET                          0x00
#define HDCPTX22_STREAMID_TYPE_00_IN_DDCBUF0_OFFSET                0x06
#define HDCPTX22_STREAM_MANAGE_HEADER_BYTE_SIZE                    6
#define HDCPTX22_STREAMID_TYPE_BYTE_SIZE                           2
#define HDCPTX22_MES_ID_OFFSET                                     0x00
#define HDCPTX22_SEQ_NUM_M_OFFSET                                  0x01
#define HDCPTX22_K_OFFSET                                          0x04
#define HDCPTX22_STREAM_ID_OFFSET                                  0x00
#define HDCPTX22_TYPE_OFFSET                                       0x01
#define HDCPTX22_MES_ID_PARAM                                      0x10
#define HDCPTX22_K_PARAM_1                                         0x0001
#define HDCPTX22_STREAM_ID_PARAM                                   0x00
#define HDCPTX22_TYPE_PARAM_TYPE_0                                 0x00
#define HDCPTX22_TYPE_PARAM_TYPE_1                                 0x01
#define HDCPTX22_HDCP2_0_REPEATER_DOWNSTREAM_BIT                   0x02
#define HDCPTX22_HDCP1_DEVICE_DOWNSTREAM_BIT                       0x01
// hdcptx22_check_rxstatus()
#define HDCPTX22_REAITV_IN_DDCBUF3_OFFSET                          0x9E
#define HDCPTX22_REARTY_IN_DDCBUF3_OFFSET                          0x9F
#define HDCPTX22_REAITV_20MS                                       0x01
#define HDCPTX22_REARTY_01TIME                                     0x01
//------------------------------------------------------------------------------
// hdcptxxx_decrypt_dev_key()
#define HDCPTXXX_DECRYPT_DEV_KEY_NUM                               1
#define HDCPTXXX_DECRYPT_DEV_KEY_MASK                              0x000000FF
#define HDCPTXXX_DECRYPT_DEV_KEY_CMD                               0x03
// hdcptxxx_select_page_of_0x7c()
#define HDCPTXXX_PAGE_SELECT_0X7C_NUM                              1
#define HDCPTXXX_PAGE_SELECT_0X7C_MASK                             0x000000FF
#define HDCPTXXX_PAGE_NO_0                                         0x00
#define HDCPTXXX_PAGE_NO_3                                         0x03
// hdcptxxx_write_ddc_buffer()
// hdcptxxx_read_ddc_buffer()
#define HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET                         0x00
// hdcptxxx_akesrst()
#define HDCPTXXX_AKESRST_NUM                                       1
#define HDCPTXXX_AKESRST_MASK                                      0x000000FF
#define HDCPTXXX_AKESRST_SUSPEND                                   0xF9
#define HDCPTXXX_AKESRST_RESET                                     0xFF
// hdcptxxx_output_eess()
#define HDCPTXXX_OUTPUT_EESS_NUM                                   1
#define HDCPTXXX_OUTPUT_EESS_MASK                                  0x000000FF
#define HDCPTXXX_OUTPUT_EESS_HDCP14_ENC_DIS                        0x05
#define HDCPTXXX_OUTPUT_EESS_HDCP14_ENC_EN                         0x03
#define HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_DIS                        0x84
#define HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_EN                         0x82
#define HDCPTXXX_OUTPUT_EESS_HDCPXX_DEFAULT                        0x00
// hdcptxxx_encryption_reset()
#define HDCPTXXX_ENCRYPTION_RESET_NUM                              1
#define HDCPTXXX_ENCRYPTION_RESET_MASK                             0x00000010
#define HDCPTXXX_ENCRYPTION_RESET_DATA                             0x10
//------------------------------------------------------------------------------
// hdcptxxx_ctrl_retry_init_auth()
#define HDCPTXXX_CONFIG_RETRY_INIT_AUTH_TIMES_DEFAULT              30
#define HDCPTXXX_CONFIG_RETRY_INIT_AUTH_INTERVAL_DEFAULT           100
// hdcptxxx_ctrl_retry_read_rxstatus()
#define HDCPTXXX_CONFIG_RETRY_READ_RXSTATUS_TIMES_DEFAULT          75
#define HDCPTXXX_CONFIG_RETRY_READ_RXSTATUS_INTERVAL_DEFAULT       10
// hdcptxxx_ctrl_retry_stream_manage()
#define HDCPTXXX_CONFIG_RETRY_STREAM_MANAGE_TIMES_DEFAULT          100
#define HDCPTXXX_CONFIG_RETRY_STREAM_MANAGE_INTERVAL_DEFAULT       1
// hdcptxxx_ctrl_cnt_seq_num_m()
#define HDCPTXXX_CTRL_SEQ_NUM_M_MASK                               0x00FFFFFF
//------------------------------------------------------------------------------
// AKE command (Rx)
#define HDCPRX14_CHECK_CHECKSUM_CMD                                0x01
#define HDCPRX14_DECRYPT_ENCRYPTED_DEVICE_KEY1_CMD                 0x02
#define HDCPRX14_CHECK_BKSV_CMD                                    0x04
#define HDCPRX14_INIT_HDCP_CMD                                     0x08
#define HDCPRX14_LOAD_ENCRYPTED_DEVICE_KEY_CMD                     0x80
#define HDCPRX14_LOAD_ENCRYPTED_DEVICE_KEY_EXEC_1248_CMD           0x8F
#define HDCPRX14_START_AKE1_CMD                                    0x10
#define HDCPRX14_START_AKE2_CMD                                    0x18
#define HDCPRX14_CALC_SHA_1_CMD                                    0xC0
#define HDCPRX14_DECRYPT_ENCRYPTED_DEVICE_KEY2_CMD                 0xC3
#define HDCPRX14_CALC_KMD_START_HDCP_CALC_R0D_CMD                  0xC7
#define HDCPRX22_DECRYPT_DEVICE_KEY_CMD                            0x70
#define HDCPRX22_STORE_RECEIVED_RTX_TXCAPS_CMD                     0x71
#define HDCPRX22_DECRYPT_EKPUB_KM_CMD                              0x72
#define HDCPRX22_DECRYPT_EKH_KM_CMD                                0x73
#define HDCPRX22_STORE_ACQUIRED_RN_CMD                             0x74
#define HDCPRX22_DECRYPT_EDKEY_KS_CMD                              0x75
#define HDCPRX22_CALC_VD_CMD                                       0x76
#define HDCPRX22_CALC_MD_CMD                                       0x77
#define HDCPRX22_COMPARE_V_VD_CMD                                  0x7D
#define HDCPRX22_COMPARE_V_VD_SUBCMD                               0x10
#define HDCPRX22_PICK_CMD_UP_CMD                                   0x7F
// maximum retry count & retry interval for akecmd
#define HDCPRXXX_AKESTA_PROCESS_RETRY_MAX                          300
#define HDCPRXXX_AKESTA_PROCESS_RETRY_INTERVAL                     1
#define HDCPRXXX_AKESTA_AKE1ST_RETRY_MAX                           300
#define HDCPRXXX_AKESTA_AKE1ST_RETRY_INTERVAL                      10
// AKE rx port offset from 0x16:0x13
#define HDCPRXXX_PORT_OFFSET                                       0x30
// hdcprx14_akecmd_nocheck()
#define HDCPRX14_AKECMD_NC_NUM                                     1
#define HDCPRX14_AKECMD_NC_MASK                                    0x000000FF
// hdcprx14_akeerr()
#define HDCPRX14_AKESTA_REG_MASK                                   0xE0
#define HDCPRX14_AKESTA_NORMAL_END                                 0xA0
#define HDCPRX14_AKESTA_ERROR_END                                  0xC0
//  hdcprx14_read_akesta()
#define HDCPRX14_AKESTA_NUM                                        1
#define HDCPRX14_AKESTA_MASK                                       0x000000FF
// hdcprx14_read_akeerr()
#define HDCPRX14_AKEERR_NUM                                        1
#define HDCPRX14_AKEERR_MASK                                       0x000000FF
// hdcprx22_akecmd()
#define HDCPRX22_AKECMD_NC_NUM                                     1
#define HDCPRX22_AKECMD_NC_MASK                                    0x000000FF
// hdcprx22_akeerr()
#define HDCPRX22_AKESTA_REG_MASK                                   0xE0
#define HDCPRX22_AKESTA_NORMAL_END                                 0xA0
#define HDCPRX22_AKESTA_ERROR_END                                  0xC0
// hdcprx22_read_akesta()
#define HDCPRX22_AKESTA_NUM                                        1
#define HDCPRX22_AKESTA_MASK                                       0x000000FF
// hdcprx22_read_akeerr()
#define HDCPRX22_AKEERR_NUM                                        1
#define HDCPRX22_AKEERR_MASK                                       0x000000FF
// hdcprx22_set_ddc_subcmd()
#define HDCPRX22_SET_DDC_SUBCMD_NUM                                1
#define HDCPRX22_SET_DDC_SUBCMD_MASK                               0x000000FF
// hdcprxxx_select_page_of_0x14()
#define HDCPRXXX_PAGE_SELECT_0X14_NUM                              1
#define HDCPRXXX_PAGE_SELECT_0X14_MASK                             0x000000FF
#define HDCPRXXX_PAGE_HDCP_PORT0                                   0x00
#define HDCPRXXX_PAGE_HDCP_PORT1                                   0x01
#define HDCPRXXX_PAGE_HDCP_PORT2                                   0x02
#define HDCPRXXX_PAGE_HDCP_PORT3                                   0x03
#define HDCPRXXX_PAGE_HDCP22_CERTRX0                               0x06
#define HDCPRXXX_PAGE_HDCP22_CERTRX1                               0x07
#define HDCPRXXX_PAGE_HDCP22_CERTRX2                               0x08
#define HDCPRXXX_PAGE_HDCP22_READ_MSG0                             0x09
#define HDCPRXXX_PAGE_HDCP22_READ_MSG1                             0x0A
#define HDCPRXXX_PAGE_HDCP22_READ_MSG2_WRITE_MSG                   0x0B
#define HDCPRXXX_PAGE_HDCP22_AUTH_REG                              0x0C
#define HDCPRXXX_PAGE_HDCP22_REPEATER_INFO                         0x89
// hdcprx22_set_hdcp2version_bit()
#define HDCPRX22_SET_HDCP2VERSION_BIT_NUM                          1
#define HDCPRX22_SET_HDCP2VERSION_BIT_MASK                         0x000000FF
#define HDCPRX22_SET_HDCP2VERSION_BIT_ENABLE                       0x04
#define HDCPRX22_SET_HDCP2VERSION_BIT_DISABLE                      0x00
// hdcprx14_set_bcaps()
#define HDCPRX14_SET_BCAPS_NUM                                     1
#define HDCPRX14_SET_BCAPS_REPEATER_ENA                            0x40
#define HDCPRX14_SET_BCAPS_REPEATER_DIS                            0x00
#define HDCPRX14_SET_BCAPS_READY_ENA                               0x20
#define HDCPRX14_SET_BCAPS_READY_DIS                               0x00
// hdcprx22_set_rxcaps()
#define HDCPRX22_SET_RXCAPS_LEN                                    3
#define HDCPRX22_SET_RXCAPS_23_16_DEF                              0x02
#define HDCPRX22_SET_RXCAPS_15_08_DEF                              0x00
#define HDCPRX22_SET_RXCAPS_07_00_DEF                              0x00
#define HDCPRX22_SET_RXCAPS_REPEATER_ENA                           0x01
#define HDCPRX22_SET_RXCAPS_REPEATER_DIS                           0x00
// hdcprxxx_set_hdcp_func_of_portno()
#define HDCPRXXX_SET_HDCP_FUNC_NUM                                 1
#define HDCPRXXX_SET_HDCP_FUNC_MASK                                0x000000FF
#define HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE                       0x01
#define HDCPRXXX_SET_HDCP_FUNC_HDCP14_DISABLE                      0x00
#define HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE                       0x02
#define HDCPRXXX_SET_HDCP_FUNC_HDCP22_DISABLE                      0x00
// hdcprx14_akcrst()
#define HDCPRX14_AKCRST_NUM                                        1
#define HDCPRX14_AKCRST_AKE_CANCEL                                 0x02
#define HDCPRX14_AKCRST_MASK                                       0x000000FF
#define HDCPRX14_AKCRST_PORT_OFFSET                                0x02
// hdcprx22_ake2rs()
#define HDCPRX22_AKE2RS_NUM                                        HDCPTX22_AKE2RS_NUM
#define HDCPRX22_AKE2RS_MASK                                       HDCPTX22_AKE2RS_MASK
#define HDCPRX22_AKE2RS_SUSPEND                                    HDCPTX22_AKE2RS_SUSPEND
#define HDCPRX22_AKE2RS_RESET                                      HDCPTX22_AKE2RS_RESET
// hdcprx22_set_rxstatus_msb()
#define HDCPRX22_SET_RXSTATUS_MSB_NUM                              1
#define HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_1                     0x08
#define HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_0                     0x00
#define HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_MASK                  0x08
#define HDCPRX22_SET_RXSTATUS_MSB_READY_1                          0x04
#define HDCPRX22_SET_RXSTATUS_MSB_READY_0                          0x00
#define HDCPRX22_SET_RXSTATUS_MSB_READY_MASK                       0x04
#define HDCPRX22_SET_RXSTATUS_MSB_MSG_SIZE_MASK                    0x03
#define HDCPRX22_SET_RXSTATUS_LSB_MSG_SIZE_MASK                    0xFF
// hdcprxxx_check_bch_error()
#define HDCPRXXX_CHECK_BCH_ERROR_NUM                               1
#define HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_HDBERR_BIT            0x04
#define HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_PBBERR_BIT            0x02
#define HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_MASK                  0x000000FF
// hdcprxxx_get_selected_rx_port()
#define HDCPRXXX_GET_SELECTED_RX_PORT_NUM                          1
#define HDMIRXXX_GET_SELECTED_RX_PORT_SWMODE1_PORT0OUT_MASK        0x00000003
// hdcprxxx_check_eess()
#define HDCPRXXX_CHECK_EESS_NUM                                    1
#define HDCPRXXX_CHECK_EESS_HDCPMONX_EESSENX_MASK                  0x02
#define HDCPRXXX_CHECK_EESS_HDCPMONX_EESSDSX_MASK                  0x04
#define HDCPRXXX_CHECK_EESS_HDCPMONX_MASK                          0x000000FF
// hdmirxxx_get_hdcp_decryption_status()
#define HDCPRXXX_GET_HDCP_DECRYPTION_STATUS_NUM                    1
#define HDCPRXXX_H22STX_PORT_OFFSET                                0x01
#define HDCPRXXX_H22STXX_MASK                                      0x000000FF
#define HDCPRXXX_H22STX_BIT                                        4
#define HDCPRXXX_H22STX_MASK                                       0x07
#define HDCPRXXX_H22STX_HDCP22_L                                   2
#define HDCPRXXX_H22STX_HDCP22_H                                   5
#define HDCPRXXX_H22STX_HDCP14                                     7
// hdcprx14_get_irq()
#define HDCPRX14_GET_IRQ_NUM                                       1
#define HDCPRX14_GET_IRQ_INTREGX_AKREDYX_OFFSET                    0x01
#define HDCPRX14_GET_IRQ_INTREGX_AKREDYX_MASK                      0x00000001
#define HDCPRX14_GET_IRQ_INTREGX_MASK                              0x000000FF
// hdcprx14_clear_akredy()
#define HDCPRX14_CLEAR_AKREDY_NUM                                  1
#define HDCPRX14_CLEAR_AKREDY_INTREGX_AKREDYX_OFFSET               0x01
#define HDCPRX14_CLEAR_AKREDY_INTREGX_AKREDYX_CLEAR                0x01
#define HDCPRX14_CLEAR_AKREDY_INTREGX_MASK                         0x000000FF
// hdcprx14_check_irq()
#define HDCPRX14_CHECK_IRQ_NUM                                     1
#define HDCPRX14_CHECK_IRQ_INTREGX_AKREDYX_OFFSET                  0x01
#define HDCPRX14_CHECK_IRQ_INTREGX_AKREDYX_MASK                    0x00000001
#define HDCPRX14_CHECK_IRQ_INTREGX_MASK                            0x000000FF
// hdcprx22_clear_h22msg0()
#define HDCPRX22_CLEAR_H22MSG0_NUM                                 1
#define HDCPRX22_CLEAR_H22MSG0_INTREG4_H22MSG0                     0x02
#define HDCPRX22_CLEAR_H22MSG0_INTREG4_H22MSG0_MASK                0x00000002
#define HDCPRX22_CLEAR_H22MSG0_INTREG4_MASK                        0x000000FF
// hdcprx22_get_irq()
#define HDCPRX22_GET_IRQ_LEN                                       4
// hdcprx22_check_irq()
#define HDCPRX22_IRQ_NONE                                          0x00
#define HDCPRX22_IRQ0_AKE_INIT                                     0x01
#define HDCPRX22_IRQ0_AKE_NO_STORED_KM                             0x04
#define HDCPRX22_IRQ0_AKE_STORED_KM                                0x08
#define HDCPRX22_IRQ0_LC_INIT                                      0x40
#define HDCPRX22_IRQ1_SKE_SEND_EKS                                 0x01
#define HDCPRX22_IRQ1_REPEATERAUTH_SEND_ACK                        0x04
#define HDCPRX22_IRQ1_REPEATERAUTH_STREAM_MANAGE                   0x08
// hdcprx22_write_rxidlist_msg()
#define HDCPRX22_MSG_ID_RXIDLIST                                   0x0C
#define HDCPRX22_WRITE_RXIDLIST_MSG_VD_OFFSET                      (HDCPRX22_MSG_ID_BYTE_SIZE \
                                                                    + HDCPTX22_RXINFO_BYTE_SIZE \
                                                                    + HDCPTX22_SEQ_NUM_V_BYTE_SIZE)
#define HDCPRX22_WRITE_RXIDLIST_MSG_RXID_OFFSET                    (HDCPRX22_WRITE_RXIDLIST_MSG_VD_OFFSET \
                                                                    + HDCPTX22_VD_BYTE_SIZE)
// hdcprx22_set_rxstatus()
#define HDCPRX22_SET_RXSTATUS_LEN                                  2
// hdcprx14_set_irq_mask()
#define HDCPRX14_SET_IRQ_MASK_LEN                                  4
// hdcprx14_set_irq_status_mask()
#define HDCPRX14_SET_IRQ_STATUS_MASK_LEN                           2
// hdcprx14_set_irq_main_status_mask()
#define HDCPRX14_SET_IRQ_MAIN_STATUS_MASK_LEN                      2
// hdcprxxx_set_irq()
#define HDCPRXXX_SET_IRQ_NONE                                      0x00
#define HDCPRXXX_SET_IRQ_AKSV                                      0x01
#define HDCPRXXX_SET_IRQ_H22MSG0                                   0x02
#define HDCPRXXX_SET_IRQ_REG4_ENA                                  0x10
#define HDCPRXXX_SET_IRQ_REG4_DIS                                  0x00
#define HDCPRXXX_SET_IRQ_REG5_ENA                                  0x20
#define HDCPRXXX_SET_IRQ_REG5_DIS                                  0x00
#define HDCPRXXX_SET_IRQ_REG6_ENA                                  0x40
#define HDCPRXXX_SET_IRQ_REG6_DIS                                  0x00
#define HDCPRXXX_SET_IRQ_REG7_ENA                                  0x80
#define HDCPRXXX_SET_IRQ_REG7_DIS                                  0x00
#define HDCPRXXX_SET_IRQ_CCIRQST_ENA                               0x40
#define HDCPRXXX_SET_IRQ_CCIRQST_DIS                               0x00
// hdcprx22_set_irq_mask()
#define HDCPRX22_SET_IRQ_MASK_LEN                                  4
// hdcprxxx_select_page_of_ksvlist()
#define HDCPRXXX_SELECT_PAGE_OF_KSVLIST_NUM                        1
#define HDCPRXXX_SELECT_PAGE_OF_KSVLIST_KSVPAGEX_MASK              0x000000FF
#define HDCPRXXX_SELECT_PAGE_OF_KSVLIST_PORT_NUM                   4
// hdcprx14_set_bstatus_of_portno()
#define HDCPRX14_SET_BSTATUS_OF_PORTNO_LEN                         2
// hdcprx22_vd_data_ctrl()
#define HDCPRX22_VD_DATA_CTRL_PUSH                                 1
#define HDCPRX22_VD_DATA_CTRL_POP                                  2
// hdcprx22_set_rxidlist()
#define HDCPRX22_SET_RXIDLIST_MAX_LEN                              (HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE \
                                                                    + (HDCPTX22_RXINFO_DEVICE_COUNT_MAX \
                                                                    * HDCPTX22_RECEIVER_ID_BYTE_SIZE))
#define HDCPRX22_SET_RXIDLIST_0_PADDING                            0x00
// hdcprx22_set_rxinfo()
#define HDCPRX22_SET_RXINFO_LEN                                    (HDCPTX22_RXINFO_BYTE_SIZE \
                                                                    + HDCPTX22_SEQ_NUM_V_BYTE_SIZE)
#define HDCPTX22_SEQ_NUM_V_PARAM_23_16                             0x00
#define HDCPTX22_SEQ_NUM_V_PARAM_15__8                             0x00
#define HDCPTX22_SEQ_NUM_V_PARAM__7__0                             0x00
// hdcptxxx_check_hpd
#define HDCPTXXX_INTREG10_RXSENP0                                  0x08
#define HDCPTXXX_INTREG10_RXSENN0                                  0x04
#define HDCPTXXX_INTREG10_HPDDETP0                                 0x02
#define HDCPTXXX_INTREG10_HPDDETN0                                 0x01
//------------------------------------------------------------------------------
// SLEEP() time
#define HDCP_SLEEP_TIME_1MSEC                                      1
#define HDCP_SLEEP_TIME_10MSEC                                     10
#define HDCP_SLEEP_TIME_50MSEC                                     50
#define HDCP_SLEEP_TIME_80MSEC                                     80
#define HDCP_SLEEP_TIME_100MSEC                                    100
#define HDCP_SLEEP_TIME_150MSEC                                    150
#define HDCP_SLEEP_TIME_180MSEC                                    180
#define HDCP_SLEEP_TIME_200MSEC                                    200
//------------------------------------------------------------------------------
#ifndef NULL
#define NULL                                                       0
#endif // NULL
//------------------------------------------------------------------------------
// error check & if (c) is a error, return (c)
#define CHECK_ERROR(c) {                        \
    if ((c) != ERR_OK) {                        \
        return (c);                             \
    }                                           \
}
// register access setting
#define SET_ACCESS(a, p, c, n) {                \
    (a).p_port = (p);                           \
    (a).p_com = (c);                            \
    (a).count = (n);                            \
}
// command setting
#define SET_COMMAND(c, i, a, d, m) {            \
    (c)[(i)].addr = (a);                        \
    (c)[(i)].data = (d);                        \
    (c)[(i)].mask = (m);                        \
}
// resigter block access setting
#define SET_BLOCK_ACCESS(b, p, a, l, d) {       \
    (b).p_port = (p);                           \
    (b).addr = (a);                             \
    (b).length = (l);                           \
    (b).p_data = (d);                           \
}
// block data setting
#define SET_BLOCK_DATA(b, i, d) {               \
    (b)[(i)] = (d);                             \
}
// last error setting
#ifdef HV_ERRNO
#define SET_HV_ERRNO(e)        { HV_ERRNO = (e); }
#else // HV_ERRNO
#define SET_HV_ERRNO(e)
#endif // HV_ERRNO
//------------------------------------------------------------------------------
typedef enum _hdcptx_ctrl_retry {
    HDCPTX_CTRL_RETRY_GET,
    HDCPTX_CTRL_RETRY_SET,
    HDCPTX_CTRL_RETRY_MAX
} hdcptx_ctrl_retry;
//------------------------------------------------------------------------------
typedef enum _hdcptx_ctrl_cnt {
    HDCPTX_CTRL_CNT_GET,
    HDCPTX_CTRL_CNT_SET,
    HDCPTX_CTRL_CNT_INC,
    HDCPTX_CTRL_CNT_DEC,
    HDCPTX_CTRL_CNT_MAX
} hdcptx_ctrl_cnt;
//------------------------------------------------------------------------------
typedef enum _hdcpxx_ctrl_req {
    HDCPXX_CTRL_REQ_GET,
    HDCPXX_CTRL_REQ_SET,
    HDCPXX_CTRL_REQ_MAX
} hdcpxx_ctrl_req;
//------------------------------------------------------------------------------
// AKE command retry control (Tx)
#if 0
typedef struct _akecmd_ctrl {
    uint32_t cmd_retry_max;
    uint32_t cmd_retry_interval;
    uint32_t ack_retry_max;
    uint32_t ack_retry_interval;
} akecmd_ctrl;
#endif
//------------------------------------------------------------------------------
static akecmd_ctrl akectrl_access = {
                                    HDCPTX14_AKECMD_ACCESS_RETRY_MAX,
                                    HDCPTX14_AKECMD_ACCESS_RETRY_INTERVAL,
                                    HDCPTX14_AKESTA_ACCESS_RETRY_MAX,
                                    HDCPTX14_AKESTA_ACCESS_RETRY_INTERVAL
                                };
static akecmd_ctrl akectrl_process = {
                                    HDCPTX14_AKECMD_PROCESS_RETRY_MAX,
                                    HDCPTX14_AKECMD_PROCESS_RETRY_INTERVAL,
                                    HDCPTX14_AKESTA_PROCESS_RETRY_MAX,
                                    HDCPTX14_AKESTA_PROCESS_RETRY_INTERVAL
                                };
static akecmd_ctrl akectrl_2nd_auth = {
                                    HDCPTX14_AKECMD_2ND_AUTH_RETRY_MAX,
                                    HDCPTX14_AKECMD_2ND_AUTH_RETRY_INTERVAL,
                                    HDCPTX14_AKESTA_2ND_AUTH_RETRY_MAX,
                                    HDCPTX14_AKESTA_2ND_AUTH_RETRY_INTERVAL
                                };
static akecmd_ctrl akectrl_lc_init = {
                                    HDCPTX22_AKECMD_LC_INIT_RETRY_MAX,
                                    HDCPTX22_AKECMD_LC_INIT_RETRY_INTERVAL,
                                    HDCPTX22_AKESTA_LC_INIT_RETRY_MAX,
                                    HDCPTX22_AKESTA_LC_INIT_RETRY_INTERVAL
                                };
static akecmd_ctrl akectrl_ake_init = {
                                    HDCPTX22_AKECMD_AKE_INIT_ACCESS_RETRY_MAX,
                                    HDCPTX22_AKECMD_AKE_INIT_ACCESS_RETRY_INTERVAL,
                                    HDCPTX22_AKESTA_AKE_INIT_ACCESS_RETRY_MAX,
                                    HDCPTX22_AKESTA_AKE_INIT_ACCESS_RETRY_INTERVAL
                                };
static akecmd_ctrl akectrl_sm = {
                                    HDCPTX22_AKECMD_STREAM_MANAGE_RETRY_MAX,
                                    HDCPTX22_AKECMD_STREAM_MANAGE_RETRY_INTERVAL,
                                    HDCPTX22_AKESTA_STREAM_MANAGE_RETRY_MAX,
                                    HDCPTX22_AKESTA_STREAM_MANAGE_RETRY_INTERVAL
                                };
//------------------------------------------------------------------------------
// AKE command retry control (Rx)
typedef struct _akecmdrx_ctrl {
    uint32_t ack_retry_max;
    uint32_t ack_retry_interval;
} akecmdrx_ctrl;
//------------------------------------------------------------------------------
static akecmdrx_ctrl akerxctrl_process = {
                                    HDCPRXXX_AKESTA_PROCESS_RETRY_MAX,
                                    HDCPRXXX_AKESTA_PROCESS_RETRY_INTERVAL
                                };
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static akecmdrx_ctrl akerxctrl_ake1st = {
                                    HDCPRXXX_AKESTA_AKE1ST_RETRY_MAX,
                                    HDCPRXXX_AKESTA_AKE1ST_RETRY_INTERVAL
                                };
#endif // Product Number having HDMI Rx & HDMI Tx
//------------------------------------------------------------------------------
// static function prototype declaration
static uint32_t hdcptx14_akecmd(system_port port, uint8_t akecmd, akecmd_ctrl *p_akectrl);
static uint32_t hdcptx14_akeerr(system_port port, akecmd_ctrl *p_akectrl);
static uint32_t hdcptx14_read_akesta(system_port port, uint8_t *p_sta);
static uint32_t hdcptx14_read_akeerr(system_port port, uint8_t *p_err);
static uint32_t hdcptx14_reset_ddc(system_port port);
static uint32_t hdcptx14_check_ddc(system_port port);
static uint32_t hdcptx14_set_ddc_property(system_port port, uint8_t saddr, uint8_t oaddr, uint16_t len);
//------------------------------------------------------------------------------
static uint32_t hdcptx14_generate_an(system_port port);
static uint32_t hdcptx14_get_an_from_register(system_port port, uint8_t *p_an);
static uint32_t hdcptx14_get_ainfo_from_register(system_port port, uint8_t *p_ainfo);
static uint32_t hdcptx14_get_aksv_from_register(system_port port, uint8_t *p_aksv);
static uint32_t hdcptx14_set_bksv_to_register(system_port port, uint8_t *p_bksv);
static uint32_t hdcptx14_set_bcaps_to_register(system_port port, uint8_t bcaps);
static uint32_t hdcptx14_get_bcaps_from_register(system_port port, uint8_t *p_bcaps);
static uint32_t hdcptx14_set_bstatus_to_register(system_port port, uint8_t *p_bstatus);
static uint32_t hdcptx14_get_bstatus_from_register(system_port port, uint8_t *p_bstatus);
static uint32_t hdcptx14_write_an(system_port port);
static uint32_t hdcptx14_write_ainfo(system_port port);
static uint32_t hdcptx14_write_aksv(system_port port);
static uint32_t hdcptx14_read_bksv(system_port port, uint8_t *p_bksv);
static uint32_t hdcptx14_read_bcaps(system_port port, uint8_t *p_bcaps);
static uint32_t hdcptx14_read_bstatus(system_port port, uint8_t *p_bstatus);
static uint32_t hdcptx14_calculate_r0(system_port port);
static uint32_t hdcptx14_compare_r0_with_r0d(system_port port);
static uint32_t hdcptx14_compare_ri_with_rid(system_port port);
static uint32_t hdcptx14_set_cmpfrm_cmplin(system_port port, uint8_t cmp);
static uint32_t hdcptx14_get_ksvlist_from_ddc_buffer(system_port port, uint8_t cnt, uint8_t *p_ksv_list);
//------------------------------------------------------------------------------
static uint32_t hdcptx14_authentication(hdmitx_hdcp_auth *p_auth);
static uint32_t hdcptx14_repeater_authentication(hdmitx_hdcp_repeater_auth *p_repeater_auth);
static uint32_t hdcptx14_get_authentication_status(hdmitx_hdcp_auth *p_auth);
//------------------------------------------------------------------------------
static uint32_t hdcptx22_akecmd(system_port port, uint8_t akecmd, akecmd_ctrl *p_akectrl);
static uint32_t hdcptx22_akeerr(system_port port, akecmd_ctrl *p_akectrl);
static uint32_t hdcptx22_read_akesta(system_port port, uint8_t *p_sta);
static uint32_t hdcptx22_read_akeerr(system_port port, uint8_t *p_err);
static uint32_t hdcptx22_reset_ddc(system_port port);
static uint32_t hdcptx22_check_ddc(system_port port);
static uint32_t hdcptx22_set_ddc_property(system_port port, uint8_t saddr, uint8_t oaddr, uint16_t len);
static uint32_t hdcptx22_set_ddc_subcmd(system_port port, uint8_t subcmd);
//------------------------------------------------------------------------------
static uint32_t hdcptx22_generate_random_seed(system_port port);
static uint32_t hdcptx22_set_random_seed_to_register(system_port port, uint8_t *p_rand);
static uint32_t hdcptx22_get_random_seed_from_register(system_port port, uint8_t *p_rand);
static uint32_t hdcptx22_ake2rs(system_port port, uint8_t rst);
static uint32_t hdcptx22_read_hdcp2version(system_port port, uint8_t *p_ver);
static uint32_t hdcptx22_set_txcaps_to_register(system_port port);
static uint32_t hdcptx22_ake_init(system_port port);
static uint32_t hdcptx22_verify_signature(system_port port);
static uint32_t hdcptx22_get_tx_receiver_id(system_port port, uint8_t *p_tx_rxid);
static uint32_t hdcptx22_init_stored_receiver_id(system_port port);
static uint32_t hdcptx22_get_stored_receiver_id(system_port port, uint8_t *p_stored_rxid);
static uint32_t hdcptx22_check_master_key_km(system_port port,
                    uint8_t *p_tx_rxid, uint8_t *p_stored_rxid, uint8_t *p_chk);
static uint32_t hdcptx22_ake_no_stored_km(system_port port);
static uint32_t hdcptx22_ake_pairing_info(system_port port);
static uint32_t hdcptx22_ake_stored_km(system_port port);
static uint32_t hdcptx22_lc_init(system_port port);
static uint32_t hdcptx22_ske_send_eks(system_port port);
static uint32_t hdcptx22_read_rxcaps(system_port port, uint8_t *p_rxcaps);
static uint32_t hdcptx22_read_rxstatus(system_port port, uint8_t *p_rxstatus);
static uint32_t hdcptx22_repeaterauth_send_receiverid_list(system_port port);
static uint32_t hdcptx22_get_rxinfo(system_port port, uint8_t *p_rxinfo);
static uint32_t hdcptx22_get_seq_num_v_from_ddc_buffer(system_port port, uint8_t *p_num);
static uint32_t hdcptx22_get_ksvlist_from_ddc_buffer(system_port port, uint8_t cnt, uint8_t *p_ksv_list);
static uint32_t hdcptx22_repeaterauth_send_ack(system_port port);
static uint32_t hdcptx22_repeaterauth_stream_manage(system_port port, uint32_t num,
                                                    hdmitx_hdcp_stream_type stype, uint8_t *p_rxinfo);
static uint32_t hdcptx22_check_rxstatus(system_port port);
//------------------------------------------------------------------------------
static uint32_t hdcptx22_authentication(hdmitx_hdcp_auth *p_auth);
static uint32_t hdcptx22_repeater_authentication(hdmitx_hdcp_repeater_auth *p_repeater_auth);
static uint32_t hdcptx22_get_authentication_status(hdmitx_hdcp_auth *p_auth);
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_decrypt_dev_key(system_port port);
static uint32_t hdcptxxx_select_page_of_0x7c(system_port port, uint8_t page);
static uint32_t hdcptxxx_write_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data);
static uint32_t hdcptxxx_read_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data);
static uint32_t hdcptxxx_akesrst(system_port port, uint8_t rst);
static uint32_t hdcptxxx_output_eess(system_port port, uint8_t eess);
static uint32_t hdcptxxx_encryption_reset(system_port port);
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_retry_init_auth(system_config_retry *p_retry, hdcptx_ctrl_retry ctrl);
static uint32_t hdcptxxx_ctrl_retry_read_rxstatus(system_config_retry *p_retry, hdcptx_ctrl_retry ctrl);
static uint32_t hdcptxxx_ctrl_cnt_seq_num_v(uint32_t *p_num, hdcptx_ctrl_cnt ctrl);
static uint32_t hdcptxxx_ctrl_cnt_stream_manage(uint32_t *p_csm, hdcptx_ctrl_cnt ctrl);
static uint32_t hdcptxxx_ctrl_retry_stream_manage(system_config_retry *p_retry, hdcptx_ctrl_retry ctrl);
static uint32_t hdcptxxx_ctrl_cnt_seq_num_m(uint32_t *p_num, hdcptx_ctrl_cnt ctrl);
static uint32_t hdcptxxx_ctrl_req_abort(system_request *p_req, hdcpxx_ctrl_req ctrl);
static system_request hdcptxxx_ctrl_get_abort_req(void);
static void hdcptxxx_ctrl_set_abort_req(system_request req);
static uint32_t hdcptxxx_ctrl_req_hpd_nocheck(system_request *p_req, hdcpxx_ctrl_req ctrl);
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_check_hpd(system_port port);
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
extern uint32_t hdmitx_set_hpd_intr(system_port port, uint8_t intr);
extern uint32_t hdmitx_get_hpd_intr(system_port port, uint8_t *p_intr);
#endif // Product Number having HDMI Tx
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_init(system_port port);
//------------------------------------------------------------------------------
static uint32_t hdcprx14_akecmd_nocheck(system_port port, uint8_t no, uint8_t akecmd);
static uint32_t hdcprx14_akecmd(system_port port, uint8_t no, uint8_t akecmd, akecmdrx_ctrl *p_actrl);
static uint32_t hdcprx14_akeerr(system_port port, uint8_t no, akecmdrx_ctrl *p_actrl);
static uint32_t hdcprx14_read_akesta(system_port port, uint8_t no, uint8_t *p_sta);
static uint32_t hdcprx14_read_akeerr(system_port port, uint8_t no, uint8_t *p_err);
static uint32_t hdcprx22_akecmd_nocheck(system_port port, uint8_t akecmd);
static uint32_t hdcprx22_akecmd(system_port port, uint8_t akecmd, akecmdrx_ctrl *p_actrl);
static uint32_t hdcprx22_akeerr(system_port port, akecmdrx_ctrl *p_actrl);
static uint32_t hdcprx22_read_akesta(system_port port, uint8_t *p_sta);
static uint32_t hdcprx22_read_akeerr(system_port port, uint8_t *p_err);
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static uint32_t hdcprx22_set_ddc_subcmd(system_port port, uint8_t subcmd);
#endif // Product Number having HDMI Rx & HDMI Tx
static uint32_t hdcprxxx_select_page_of_0x14(system_port port, uint8_t page);
static uint32_t hdcprx22_set_hdcp2version_bit(system_port port, uint8_t bit);
static uint32_t hdcprx22_set_hdcp2ver_of_portno(system_port port, uint8_t no, hdmirx_hdcp_version ver);
static uint32_t hdcprx22_set_hdcp2ver(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx14_set_bcaps(system_port port, uint8_t bcaps, uint8_t mask);
static uint32_t hdcprx14_set_bcaps_of_portno(system_port port, uint8_t no, uint8_t bcaps, uint8_t mask);
static uint32_t hdcprx14_set_bcaps_repeater(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx14_set_bcaps_reset(system_port port, uint8_t no);
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static uint32_t hdcprx14_set_bcaps_ready(system_port port, uint8_t no);
#endif // Product Number having HDMI Rx & HDMI Tx
static uint32_t hdcprx22_set_random_seed(system_port port);
static uint32_t hdcprx22_set_rxcaps(system_port port, uint8_t rxcaps0, uint8_t rxcaps1, uint8_t rxcaps2);
static uint32_t hdcprx22_set_rxcaps_repeater(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx22_decrypt_device_key(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx14_decrypt_device_key(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprxxx_set_hdcp_func_of_portno(system_port port, uint8_t no, uint8_t func);
static uint32_t hdcprxxx_set_hdcp_func(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx14_start_hdcp_auth(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx22_start_hdcp_auth(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx14_akcrst(system_port port, uint8_t no);
static uint32_t hdcprx22_ake2rs(system_port port, uint8_t rst);
static uint32_t hdcprx22_set_rxstatus_msb(system_port port, uint8_t no, uint8_t rxstatus_15_8, uint8_t mask);
static uint32_t hdcprxxx_auth_reset_port(system_port port, uint8_t no, hdmirx_hdcp_version ver);
static uint32_t hdcprxxx_check_bch_error(system_port port);
static uint32_t hdcprxxx_get_selected_rx_port(system_port port, uint8_t *p_port);
static uint32_t hdcprxxx_check_eess(system_port port, uint8_t no, hdmirx_hdcp_eess *p_eess);
static uint32_t hdmirxxx_get_hdcp_decryption_status(system_port port, uint8_t no, uint8_t *p_stat);
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static uint32_t hdcprx14_get_irq(system_port port, uint8_t no, uint8_t *p_msg);
static uint32_t hdcprx14_check_irq(system_port port, uint8_t no, hdmirx_hdcp_message *p_msg);
static uint32_t hdcprx14_clear_akredy(system_port port, uint8_t no);
static uint32_t hdcprx14_1st_auth(system_port port, uint8_t no);
static uint32_t hdcprx14_calc_vddhn(system_port port, uint8_t no);
static uint32_t hdcprx22_clear_h22msg0(system_port port);
static uint32_t hdcprx22_get_irq(system_port port, uint8_t *p_msg0, uint8_t *p_msg1);
static uint32_t hdcprx22_check_irq(system_port port, hdmirx_hdcp_message *p_msg);
static uint32_t hdcprx22_ack_ake_init(system_port port);
static uint32_t hdcprx22_ack_ake_no_stored_km(system_port port);
static uint32_t hdcprx22_ack_ake_stored_km(system_port port);
static uint32_t hdcprx22_ack_lc_init(system_port port);
static uint32_t hdcprx22_ack_ske_send_eks(system_port port);
static uint32_t hdcprx22_ack_repeaterauth_send_ack(system_port port);
static uint32_t hdcprx22_ack_repeaterauth_stream_manage(system_port port);
static uint32_t hdcprx22_calc_vd(system_port port);
static uint32_t hdcprx22_read_vd_calc_data(system_port port, uint8_t *p_vd_data);
static uint32_t hdcprx22_read_vd(system_port port, uint8_t *p_vd);
static uint32_t hdcprx22_write_rxidlist_msg(system_port port, uint8_t *p_vd_data,
                                                uint8_t *p_vd, uint8_t *p_msg_size);
static uint32_t hdcprx22_set_rxidlist_msg(system_port port, uint8_t *p_msg_size);
static uint32_t hdcprx22_set_rxstatus(system_port port, uint8_t no,uint8_t rxstatus_15_8,
                                        uint8_t mask_15_8, uint8_t rxstatus_7_0, uint8_t mask_7_0);
static uint32_t hdcprx22_set_rxstatus_ready_msgsize(system_port port, uint8_t no, uint8_t msg_size);
static uint32_t hdcprxxx_check_auth_status(hdmirx_hdcp_auth_status *p_status, uint8_t no);
static uint32_t hdcprx14_set_irq_mask(system_port port, uint8_t msk0, uint8_t msk1,
                                        uint8_t msk2, uint8_t msk3);
static uint32_t hdcprx14_set_irq_status_mask(system_port port, uint8_t smsk0, uint8_t smsk1);
static uint32_t hdcprx14_set_irq_main_status_mask(system_port port, uint8_t mmsk0, uint8_t mmsk1);
static uint32_t hdcprxxx_set_irq(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprx22_set_irq_mask(system_port port, uint8_t msk0, uint8_t msk1,
                                        uint8_t msk2, uint8_t msk3);
static uint32_t hdcprx22_set_irq(hdmirx_hdcp_auth *p_auth);
static uint32_t hdcprxxx_select_page_of_ksvlist(system_port port, uint8_t no, uint8_t page);
static uint32_t hdcprx14_set_ksv_of_portno(system_port port, uint8_t no, uint8_t idx, uint8_t *p_ksv);
static uint32_t hdcprx14_set_ksvlist(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                        hdmitx_hdcp_repeater_auth *p_tx_rep);
static uint32_t hdcprx14_set_bstatus_of_portno(system_port port, uint8_t no, uint8_t bs0, uint8_t bs1);
static uint32_t hdcprx14_set_bstatus(hdmirx_hdcp_auth *p_rx, uint8_t count, uint8_t depth);
static uint32_t hdcprx14_set_repeater_info(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                            hdmitx_hdcp_repeater_auth *p_tx_rep);
static uint32_t hdcprx22_vd_data_ctrl(system_port port, uint8_t ctrl);
static uint32_t hdcprx22_set_rxidlist(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                        hdmitx_hdcp_repeater_auth *p_tx_rep);
static uint32_t hdcprx22_set_rxinfo(hdmirx_hdcp_auth *p_rx, uint8_t count, uint8_t depth,
                                    uint8_t hdcp1dev, uint8_t hdcp2rep);
static uint32_t hdcprx22_set_repeater_info(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                            hdmitx_hdcp_repeater_auth *p_tx_rep);
static uint32_t hdcprxxx_set_repeater_info(hdmirx_hdcp_auth *p_auth);
extern uint32_t hdmitx_set_sg_on(system_port port);
extern uint32_t hdmitx_set_sg_off(system_port port);
#endif // Product Number having HDMI Rx & HDMI Tx
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_init(system_port port);
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_ctrl_req_abort(system_request *p_req, hdcpxx_ctrl_req ctrl);
static system_request hdcprxxx_ctrl_get_abort_req(void);
static void hdcprxxx_ctrl_set_abort_req(system_request req);
//------------------------------------------------------------------------------
// error check for HDCP2.2 & if (c) is a error, perform hdmitx_hdcp_init() and return (c)
#define CHECK_HDCP22_ERROR(c, p) {                \
    if ((c) != ERR_OK) {                        \
        hdmitx_hdcp_init(p);                    \
        return (c);                                \
    }                                            \
}
// error check of HDP & RxSense inactive
#define CHECK_INACTIVE_ERROR(c) {                \
    if ((c) == ERR_HDMITX_HPD_INACTIVE) {        \
        return (c);                                \
    }                                            \
    if ((c) == ERR_HDMITX_RXSENSE_INACTIVE) {    \
        return (c);                                \
    }                                            \
}
// error check of HDP & RxSense inactive for HDCP2.2
#define CHECK_HDCP22_INACTIVE_ERROR(c, p) {        \
    if ((c) == ERR_HDMITX_HPD_INACTIVE) {        \
        hdmitx_hdcp_init(p);                    \
        return (c);                                \
    }                                            \
    if ((c) == ERR_HDMITX_RXSENSE_INACTIVE) {    \
        hdmitx_hdcp_init(p);                    \
        return (c);                                \
    }                                            \
}
//------------------------------------------------------------------------------
// abort request check of HDCP Tx
#define CHECK_TX_ABORT() {                                            \
    if (hdcptxxx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {    \
        hdcptxxx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);            \
        return ERR_HDCPTXXX_ABORT;                                    \
    }                                                                \
}
// abort request check of HDCP Rx
#define CHECK_RX_ABORT() {                                            \
    if (hdcprxxx_ctrl_get_abort_req() == SYSTEM_REQUEST_ABORT) {    \
        hdcprxxx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);            \
        return ERR_HDCPRXXX_ABORT;                                    \
    }                                                                \
}
// error check of Tx abort
#define CHECK_TX_ABORT_ERROR(c) {                                    \
    if ((c) == ERR_HDCPTXXX_ABORT) {                                \
        return (c);                                                    \
    }                                                                \
}
// error check of Tx abort for HDCP2.2
#define CHECK_HDCP22_TX_ABORT_ERROR(c, p) {                            \
    if ((c) == ERR_HDCPTXXX_ABORT) {                                \
        hdmitx_hdcp_init(p);                                        \
        return (c);                                                    \
    }                                                                \
}

extern XBH_S32                      xbh_vbo2hdmi_reset_key;
//------------------------------------------------------------------------------
static uint32_t hdcptx14_akecmd(system_port port, uint8_t akecmd, akecmd_ctrl *p_akectrl)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_AKECMD_NUM];
    uint32_t ret, ret2;
    uint32_t i;

    if (p_akectrl == NULL) {
        return ERR_NULL_POINTER;
    }

    // check HPD of HDMI Tx
    ret = hdcptxxx_check_hpd(port);
    MES("LF9129 %s is check %d ST", __func__, __LINE__);
    CHECK_ERROR(ret);
    MES("LF9129 %s is check %d OK", __func__, __LINE__);

    SET_ACCESS(access, &port, cmd, HDCPTX14_AKECMD_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKECMD_ADDR, akecmd, HDCPTX14_AKECMD_MASK);

    for (i=0; i<p_akectrl->cmd_retry_max; i++) {
        // if(xbh_vbo2hdmi_reset_key == 1)
        //     break;
        CHECK_TX_ABORT();
        ret = lf9129_device_write(&access);
        MES("LF9129 %s is check %d ST", __func__, __LINE__);
        CHECK_ERROR(ret);
        MES("LF9129 %s is check %d OK", __func__, __LINE__);
        // check error
        ret = hdcptx14_akeerr(port, p_akectrl);
        if (ret == ERR_OK) {
            break;
        } else {
            // check HPD of HDMI Tx
            ret2 = hdcptxxx_check_hpd(port);
            CHECK_ERROR(ret2);
            if (ret == ERR_HDCPTX14_DDC_NO_ACK_REPLY) {
                // DDC Reset
                ret = hdcptx14_reset_ddc(port);
                MES("LF9129 %s is check %d error", __func__, __LINE__);
                CHECK_ERROR(ret);
                MES("LF9129 %s is check %d error", __func__, __LINE__);
            } else if (ret == ERR_HDCPTX14_DDC_FIXED_LOW_LEVEL) {
                // DDC Check
                ret = hdcptx14_check_ddc(port);
                MES("LF9129 %s is check %d error", __func__, __LINE__);
                CHECK_ERROR(ret);
                MES("LF9129 %s is check %d error", __func__, __LINE__);
            } else {
                MES("LF9129 %s is check %d error", __func__, __LINE__);
                return ret;
            }
        }
        SLEEP_Delay_ms(p_akectrl->cmd_retry_interval);
    }
    if (i == p_akectrl->cmd_retry_max) {
        return ERR_HDCPTX14_DDC_ACCESS_RETRY;
    }

    return ret;
}


//------------------------------------------------------------------------------
static uint32_t hdcptx14_akeerr(system_port port, akecmd_ctrl *p_akectrl)
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
        // if(xbh_vbo2hdmi_reset_key == 1)
        // {
        //     break;
        // }
        CHECK_TX_ABORT();
        MES("LF9129 %s is check %d hdcptx14_read_akesta", __func__, __LINE__);
        ret = hdcptx14_read_akesta(port, &akesta);
        CHECK_ERROR(ret);
        if ((akesta & HDCPTX14_AKESTA_REG_MASK) == HDCPTX14_AKESTA_NORMAL_END) {
            MES("LF9129 %s is check %d HDCPTX14_AKESTA_NORMAL_END", __func__, __LINE__);
            ret = ERR_OK;
            break;
        } else if ((akesta & HDCPTX14_AKESTA_REG_MASK) == HDCPTX14_AKESTA_ERROR_END) {
            MES("LF9129 %s is check %d HDCPTX14_AKESTA_ERROR_END", __func__, __LINE__);
            ret = hdcptx14_read_akeerr(port, &akeerr);
            CHECK_ERROR(ret);
            MES("LF9129 %s is check %d return ", __func__, __LINE__);
            ret = ERR_HDCPTX14_AKECMD_NORMAL_END | akeerr;
            return ret;
        } else {
            // retry
        }
        SLEEP_Delay_ms(p_akectrl->ack_retry_interval);
    }
    if (i == p_akectrl->ack_retry_max) {
        MES("LF9129 %s is check %d i == p_akectrl->ack_retry_max", __func__, __LINE__);
        ret = ERR_HDCPTX14_AKESTA_READ_RETRY;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_read_akesta(system_port port, uint8_t *p_sta)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_AKESTA_NUM];
    uint32_t ret;

    if (p_sta == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPTX14_AKESTA_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKESTA_ADDR, 0, HDCPTX14_AKESTA_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_sta = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_read_akeerr(system_port port, uint8_t *p_err)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_AKEERR_NUM];
    uint32_t ret;

    if (p_err == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPTX14_AKEERR_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKEERR_ADDR, 0, HDCPTX14_AKEERR_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_err = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_reset_ddc(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_RESET_DDC_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX14_RESET_DDC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKECMD_ADDR, HDCPTX14_RESET_DDC_CMD, HDCPTX14_RESET_DDC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_check_ddc(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_CHECK_DDC_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX14_CHECK_DDC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKECMD_ADDR, HDCPTX14_CHECK_DDC_CMD, HDCPTX14_CHECK_DDC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_set_ddc_property(system_port port, uint8_t saddr, uint8_t oaddr, uint16_t len)
{
    system_block_access baccess;
    uint8_t data[HDCPTX14_SET_DDC_PROPERTY_LEN];
    uint32_t ret;

    if (len == 0 || len > HDMITX_DDC_MAX_BUF_SIZE) {
        return ERR_INVALID_SIZE;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_DDCSLA_ADDR,
                        HDCPTX14_SET_DDC_PROPERTY_LEN, data);
    // slave address
    SET_BLOCK_DATA(data, 0, saddr);                            // 0x3A80
    // offset address
    SET_BLOCK_DATA(data, 1, oaddr);                            // 0x3A81
    // length (1byte: 0, 2byte: 1, ... 256byte: 255(0xFF))
    SET_BLOCK_DATA(data, 2, (len - 1));                        // 0x3A82

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_generate_an(system_port port)
{
    uint32_t ret;

    // Stop An calculation, because random number is being calculated after reset.
    ret = hdcptx14_akecmd(port, HDCPTX14_AN_GEN_STOP_CMD, &akectrl_process);
    CHECK_ERROR(ret);
    // Start An calculation
    ret = hdcptx14_akecmd(port, HDCPTX14_AN_GEN_START_CMD, &akectrl_process);
    CHECK_ERROR(ret);
    CHECK_TX_ABORT();
    // Wait 10ms
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_10MSEC);
    // Stop An calculation
    ret = hdcptx14_akecmd(port, HDCPTX14_AN_GEN_STOP_CMD, &akectrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_an_from_register(system_port port, uint8_t *p_an)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_an == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_ANREG_00_ADDR,
                                        HDCPTX14_AN_GET_REGISTER_LEN,
                                        p_an);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_ainfo_from_register(system_port port, uint8_t *p_ainfo)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_AINFO_GET_REGISTER_NUM];
    uint32_t ret;

    if (p_ainfo == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPTX14_AINFO_GET_REGISTER_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AINFREG_00_ADDR, 0, HDCPTX14_AINFO_GET_REGISTER_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_ainfo = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_aksv_from_register(system_port port, uint8_t *p_aksv)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_aksv == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_AKSVREG_00_ADDR,
                                        HDCPTX14_AKSV_GET_REGISTER_LEN,
                                        p_aksv);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_set_bksv_to_register(system_port port, uint8_t *p_bksv)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_bksv == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_BKSVREG_00_ADDR,
                                        HDCPTX14_BKSV_SET_REGISTER_LEN,
                                        p_bksv);
    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_set_bcaps_to_register(system_port port, uint8_t bcaps)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_BCAPS_SET_REGISTER_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX14_BCAPS_SET_REGISTER_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_BCAPSREG_ADDR, bcaps, HDCPTX14_BCAPS_SET_REGISTER_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_bcaps_from_register(system_port port, uint8_t *p_bcaps)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_GET_BCAPS_FROM_REGISTER_NUM];
    uint32_t ret;

    if (p_bcaps == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPTX14_GET_BCAPS_FROM_REGISTER_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_BCAPSREG_ADDR, 0, HDCPTX14_GET_BCAPS_FROM_REGISTER_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_bcaps = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_set_bstatus_to_register(system_port port, uint8_t *p_bstatus)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_bstatus == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_BSTAREG_00_ADDR,
                                        HDCPTX14_BSTATUS_SET_REGISTER_LEN,
                                        p_bstatus);
    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_bstatus_from_register(system_port port, uint8_t *p_bstatus)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_bstatus == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_BSTAREG_00_ADDR,
                                        HDCPTX14_BSTATUS_GET_REGISTER_LEN,
                                        p_bstatus);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_write_an(system_port port)
{
    uint8_t an[HDCPTX14_AN_BYTE_SIZE];
    uint32_t ret;

    // page(0x7c): 0

    // Get An from register
    ret = hdcptx14_get_an_from_register(port, an);
    CHECK_ERROR(ret);
    // page(0x7c): 0
    // Set An to DDC buffer
    ret = hdcptxxx_write_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            HDCPTX14_AN_BYTE_SIZE,
                                            an);
    CHECK_ERROR(ret);
    // Set DDC slave/offset address and length to write An
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX14_OFFSET_ADDR_AN_8_W,
                                        HDCPTX14_AN_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Write An to DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_WRITE_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_write_ainfo(system_port port)
{
    uint8_t ainfo;
    uint32_t ret;

    // page(0x7c): 0

    // Get Ainfo from register
    ret = hdcptx14_get_ainfo_from_register(port, &ainfo);
    CHECK_ERROR(ret);
    // page(0x7c): 0
    // Set Ainfo to DDC buffer
    ret = hdcptxxx_write_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            1,
                                            &ainfo);
    CHECK_ERROR(ret);
    // Set DDC slave/offset address and length to write Ainfo
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX14_OFFSET_ADDR_AINFO_1_W,
                                        HDCPTX14_AINFO_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Write Ainfo to DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_WRITE_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_write_aksv(system_port port)
{
    uint8_t aksv[HDCPTX14_KSV_BYTE_SIZE];
    uint32_t ret;

    // page(0x7c): 0

    // Get Aksv from register
    ret = hdcptx14_get_aksv_from_register(port, aksv);
    CHECK_ERROR(ret);
    // page(0x7c): 0
    // Set An to DDC buffer
    ret = hdcptxxx_write_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            HDCPTX14_KSV_BYTE_SIZE,
                                            aksv);
    CHECK_ERROR(ret);
    // Set DDC slave/offset address and length to write Aksv
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX14_OFFSET_ADDR_AKSV_5_W,
                                        HDCPTX14_KSV_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Write Aksv to DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_WRITE_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_read_bksv(system_port port, uint8_t *p_bksv)
{
    uint32_t ret;

    if (p_bksv == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // Set DDC slave/offset address and length to read Bksv
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX14_OFFSET_ADDR_BKSV_5_R,
                                        HDCPTX14_KSV_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Read Bksv from DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_READ_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);
    // page(0x7c): 0
    // Get Bksv from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            HDCPTX14_KSV_BYTE_SIZE,
                                            p_bksv);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_read_bcaps(system_port port, uint8_t *p_bcaps)
{
    uint8_t bcaps;
    uint32_t ret;

    if (p_bcaps == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // Set DDC slave/offset address and length to read Bcaps
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX14_OFFSET_ADDR_BCAPS_1_R,
                                        HDCPTX14_BCAPS_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Read Bcaps from DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_READ_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);
    // page(0x7c): 0
    // Get Bcaps from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            1,
                                            &bcaps);
    CHECK_ERROR(ret);
    // set output data
    *p_bcaps = bcaps;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_read_bstatus(system_port port, uint8_t *p_bstatus)
{
    uint32_t ret;

    if (p_bstatus == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // Set DDC slave/offset address and length to read Bstatus
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX14_OFFSET_ADDR_BSTATUS_2_R,
                                        HDCPTX14_BSTATUS_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Read Bcaps from DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_READ_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);
    // page(0x7c): 0
    // Get Bstatus from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            HDCPTX14_BSTATUS_BYTE_SIZE,
                                            p_bstatus);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_calculate_r0(system_port port)
{
    uint32_t ret;

    // Calculate R0
    // An generation, Bksv read and Bstatus read are necessary prior to this calculation.
    ret = hdcptx14_akecmd(port, HDCPTX14_R0_CALC_CMD, &akectrl_process);
    MES("LF9129 %s is check %d ST", __func__, __LINE__);
    CHECK_ERROR(ret);
    MES("LF9129 %s is check %d OK", __func__, __LINE__);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_compare_r0_with_r0d(system_port port)
{
    uint32_t ret;

    // Compare R0 with R0'
    ret = hdcptx14_akecmd(port, HDCPTX14_R0_R0D_COMPARE_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_compare_ri_with_rid(system_port port)
{
    uint32_t ret;

    // Compare Ri with Ri'
    // CMPFRM/CMPLIN setting are necessary prior to this comparison.
    ret = hdcptx14_akecmd(port, HDCPTX14_RI_RID_COMAPRE_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_set_cmpfrm_cmplin(system_port port, uint8_t cmp)
{
    system_reg_access access;
    system_command cmd[HDCPTX14_SET_CMPFRM_CMPLIN_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX14_SET_CMPFRM_CMPLIN_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_CMPFRLI_ADDR, cmp, HDCPTX14_SET_CMPFRM_CMPLIN_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_ksvlist_from_ddc_buffer(system_port port, uint8_t cnt, uint8_t *p_ksv_list)
{
    uint32_t ddc_data_size;
    uint32_t ret;

    // page(0x7c): 0

    if (p_ksv_list == NULL) {
        return ERR_NULL_POINTER;
    }
    if (cnt > HDCPTX14_BSTATUS_DEVICE_COUNT_MAX) {
        return ERR_HDCPTX14_MAX_DEVS_EXCEEDED;
    }

    ret = ERR_NG;
    ddc_data_size = cnt * HDCPTX14_KSV_BYTE_SIZE;
    // Get KSV list from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            ddc_data_size,
                                            p_ksv_list);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_authentication(hdmitx_hdcp_auth *p_auth)
{
    system_port port;
    uint8_t bksv[HDCPTX14_KSV_BYTE_SIZE];
    uint8_t bcaps;
    uint8_t bstatus[HDCPTX14_BSTATUS_BYTE_SIZE];
    uint32_t i;
    system_config_retry retry_init_auth;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    port = p_auth->port;

    // 1st Authentication

    // ENC_DIS
    MES("Func: %s, Line: %d ENC_DIS", __func__, __LINE__);
    ret = hdcptxxx_output_eess(port, HDCPTXXX_OUTPUT_EESS_HDCP14_ENC_DIS);
    CHECK_ERROR(ret);

    // Reset authentication process
    MES("Func: %s, Line: %d Reset authentication process", __func__, __LINE__);
    ret = hdcptxxx_akesrst(port, HDCPTXXX_AKESRST_RESET);
    CHECK_ERROR(ret);

    // Reset HDCP2.2 authentication process
    MES("Func: %s, Line: %d Reset HDCP2.2 authentication process", __func__, __LINE__);
    ret = hdcptx22_ake2rs(port, HDCPTX22_AKE2RS_RESET);
    CHECK_ERROR(ret);
    CHECK_TX_ABORT();
    // Wait 100ms (tentative)

    SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);

    // Generate An
    MES("Func: %s, Line: %d Generate An", __func__, __LINE__);
    ret = hdcptx14_generate_an(port);
    CHECK_ERROR(ret);

    // page(0x7c): 0 (Select DDC buffer page)
    MES("Func: %s, Line: %d page(0x7c): 0 (Select DDC buffer page)", __func__, __LINE__);
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_0);
    CHECK_ERROR(ret);

    // get retry info
    MES("Func: %s, Line: %d get retry info", __func__, __LINE__);
    ret = hdcptxxx_ctrl_retry_init_auth(&retry_init_auth, HDCPTX_CTRL_RETRY_GET);
    CHECK_ERROR(ret);

    // retry initial authentication
    MES("Func: %s, Line: %d retry initial authentication", __func__, __LINE__);
    for (i=0; i<retry_init_auth.times; i++) {
        // Write An
        // page(0x7c): 0
        MES("Func: %s, Line: %d Write An", __func__, __LINE__);
        ret = hdcptx14_write_an(port);
        CHECK_ERROR(ret);

        // Write Ainfo
        // page(0x7c): 0
        MES("Func: %s, Line: %d Write Ainfo", __func__, __LINE__);
        ret = hdcptx14_write_ainfo(port);
        CHECK_ERROR(ret);

        // Write Aksv
        // page(0x7c): 0
        MES("Func: %s, Line: %d Write Aksv", __func__, __LINE__);
        ret = hdcptx14_write_aksv(port);
        CHECK_ERROR(ret);

        // Read Bksv
        // page(0x7c): 0
        MES("Func: %s, Line: %d Read Bksv", __func__, __LINE__);
        ret = hdcptx14_read_bksv(port, bksv);
        SET_HV_ERRNO(ret);
        if (ret == ERR_OK) break;
        CHECK_INACTIVE_ERROR(ret);
        CHECK_TX_ABORT_ERROR(ret);
        MES("Func: %s, Line: %d SLEEP_Delay_ms", __func__, __LINE__);
        SLEEP_Delay_ms(retry_init_auth.interval);
    }
    if (i == retry_init_auth.times) {
        MES("Func: %s, Line: %d i == retry_init_auth.times", __func__, __LINE__);
        return ERR_HDCPTX14_INIT_AUTH_RETRY;
    }
    // Set Bksv to Bksv register
    MES("Func: %s, Line: %d Set Bksv to Bksv register", __func__, __LINE__);
    ret = hdcptx14_set_bksv_to_register(port, bksv);
    CHECK_ERROR(ret);

    // Read Bcaps
    // page(0x7c): 0
    MES("Func: %s, Line: %d Read Bcaps", __func__, __LINE__);
    ret = hdcptx14_read_bcaps(port, &bcaps);
    CHECK_ERROR(ret);

    // Set Bcaps to Bcaps register
    MES("Func: %s, Line: %d Set Bcaps to Bcaps register", __func__, __LINE__);
    ret = hdcptx14_set_bcaps_to_register(port, bcaps);
    CHECK_ERROR(ret);

    // Read Bstatus
    // page(0x7c): 0
    MES("Func: %s, Line: %d Read Bstatus", __func__, __LINE__);
    ret = hdcptx14_read_bstatus(port, bstatus);
    CHECK_ERROR(ret);

    // Set Bstatus to Bstatus register
    MES("Func: %s, Line: %d Set Bstatus to Bstatus register", __func__, __LINE__);
    ret = hdcptx14_set_bstatus_to_register(port, bstatus);
    CHECK_ERROR(ret);

    // Calculate R0
    MES("Func: %s, Line: %d Calculate R0", __func__, __LINE__);
    ret = hdcptx14_calculate_r0(port);
    CHECK_ERROR(ret);

    CHECK_TX_ABORT();
    // Wait 100ms (standard)
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);
    // Compare R0 with R0'
    MES("Func: %s, Line: %d Compare R0 with R0'", __func__, __LINE__);
    ret = hdcptx14_compare_r0_with_r0d(port);
    CHECK_ERROR(ret);
    CHECK_TX_ABORT();

    // Wait 200ms (tentative)
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_200MSEC);

    // check HPD of HDMI Tx
    MES("Func: %s, Line: %d check HPD of HDMI Tx", __func__, __LINE__);
    ret = hdcptxxx_check_hpd(port);
    CHECK_ERROR(ret);

    // ENC_EN
    MES("Func: %s, Line: %d ENC_EN", __func__, __LINE__);
    ret = hdcptxxx_output_eess(port, HDCPTXXX_OUTPUT_EESS_HDCP14_ENC_EN);
    CHECK_ERROR(ret);

    // 3rd Authentication

    // Set CMPFRM/CMPLIN
    //   CMPFRM: every 128 frames
    //   CMPLIN: 3/4 of the total line number
    MES("Func: %s, Line: %d Set CMPFRM/CMPLIN", __func__, __LINE__);
    ret = hdcptx14_set_cmpfrm_cmplin(port, HDCPTX14_SET_CMPFRM_CMPLIN_128F_3S4L);
    CHECK_ERROR(ret);

#ifdef CONFIG_HDCP14_VERIFY_RI_BEFORE_SECOND
    // Compare Ri with Ri'
    MES("Func: %s, Line: %d Compare Ri with Ri'", __func__, __LINE__);
    ret = hdcptx14_compare_ri_with_rid(port);
    CHECK_ERROR(ret);
    // Suspend comparing Ri with Ri'
    MES("Func: %s, Line: %d Suspend comparing Ri with Ri'", __func__, __LINE__);
    ret = hdcptxxx_akesrst(port, HDCPTXXX_AKESRST_SUSPEND);
    CHECK_ERROR(ret);
#endif // CONFIG_HDCP14_VERIFY_RI_BEFORE_SECOND

    // set is_repeater member
    MES("Func: %s, Line: %d set is_repeater member", __func__, __LINE__);
    if ((bcaps & HDCPTX14_BCAPS_REPEATER_BIT) == HDCPTX14_BCAPS_REPEATER_BIT) {
        MES("Func: %s, Line: %d (bcaps & HDCPTX14_BCAPS_REPEATER_BIT) == HDCPTX14_BCAPS_REPEATER_BIT", __func__, __LINE__);
        p_auth->is_repeater = HDMITX_HDCP_REPEATER;
    } else {
        MES("Func: %s, Line: %d else else else", __func__, __LINE__);
        p_auth->is_repeater = HDMITX_HDCP_NOT_REPEATER;
    }
    // set bksv member
    MES("Func: %s, Line: %d set bksv member", __func__, __LINE__);
    for (i=0; i<HDCPTX14_KSV_BYTE_SIZE; i++) {
        p_auth->bksv[i] = bksv[i];
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_repeater_authentication(hdmitx_hdcp_repeater_auth *p_repeater_auth)
{
    system_port port;
    uint8_t bcaps;
    uint8_t bstatus[HDCPTX14_BSTATUS_BYTE_SIZE];
    uint8_t down_count, down_depth;
    uint8_t max_devs_exceeded, max_cascade_exceeded;
    uint8_t ksv_list[HDCPTX14_BSTATUS_DEVICE_COUNT_MAX * HDMITX_HDCP_KSV_BYTE_SIZE];
    uint32_t ret;
    uint8_t i, j;

    if (p_repeater_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    port = p_repeater_auth->port;

    // 2nd Authentication

    // Get Bcaps from Bcaps register
    ret = hdcptx14_get_bcaps_from_register(port, &bcaps);
    CHECK_ERROR(ret);
    // check REPEATER bit
    if ((bcaps & HDCPTX14_BCAPS_REPEATER_BIT) != HDCPTX14_BCAPS_REPEATER_BIT) {
        return ERR_NG;
    }
    // Read KSV list from KSV FIFO after checking the READY bit of Bcaps and Bstatus
    // The READY bit check is performed for a maximum of 5 seconds every 100ms
    ret = hdcptx14_akecmd(port, HDCPTX14_KSVLIST_READ_CMD, &akectrl_2nd_auth);
    CHECK_ERROR(ret);
    // Get Bstatus from Bstatus register
    ret = hdcptx14_get_bstatus_from_register(port, bstatus);
    CHECK_ERROR(ret);
    // set downstream_count, downstream_depth
    down_count = (uint8_t)(bstatus[0] & HDCPTX14_BSTATUS_DEVICE_COUNT_MASK);
    down_depth = (uint8_t)(bstatus[1] & HDCPTX14_BSTATUS_DEPTH_MASK);
    max_devs_exceeded = (uint8_t)(bstatus[0] & HDCPTX14_BSTATUS_MAX_DEVS_EXCEEDED_1);
    max_cascade_exceeded = (uint8_t)(bstatus[1] & HDCPTX14_BSTATUS_MAX_CASCADE_EXCEEDED_1);
    p_repeater_auth->downstream_count = down_count;
    p_repeater_auth->downstream_depth = down_depth;
    p_repeater_auth->bstatus[0] = bstatus[0];
    p_repeater_auth->bstatus[1] = bstatus[1];
    // check MAX_DEVS_EXCEEDED
    if((down_count > HDCPTX14_BSTATUS_DEVICE_COUNT_MAX)
        || (max_devs_exceeded == HDCPTX14_BSTATUS_MAX_DEVS_EXCEEDED_1)) {
        return ERR_HDCPTX14_MAX_DEVS_EXCEEDED;
    }
    // check MAX_CASCADE_EXCEEDED
    if ((down_depth > HDCPTX14_BSTATUS_DEPTH_MAX)
        || (max_cascade_exceeded == HDCPTX14_BSTATUS_MAX_CASCADE_EXCEEDED_1)) {
        return ERR_HDCPTX14_MAX_CASCADE_EXCEEDED;
    }
    // page(0x7c): 0 (Select DDC buffer page)
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_0);
    CHECK_ERROR(ret);
    // get KSV list
    // page(0x7c): 0
    ret = hdcptx14_get_ksvlist_from_ddc_buffer(port, down_count, ksv_list);
    CHECK_ERROR(ret);
    // set ksv_list
    for (i=0; i<down_count; i++) {
        for (j=0; j<HDMITX_HDCP_KSV_BYTE_SIZE; j++) {
            p_repeater_auth->ksv_list[i][j] = ksv_list[(uint8_t)(i * HDMITX_HDCP_KSV_BYTE_SIZE + j)];
        }
    }
    // Calculate V
    ret = hdcptx14_akecmd(port, HDCPTX14_CALC_V_CMD, &akectrl_process);
    CHECK_ERROR(ret);
    // Read V' from Sink and compare V and V'
    ret = hdcptx14_akecmd(port, HDCPTX14_VERIFY_V_VD_CMD, &akectrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx14_get_authentication_status(hdmitx_hdcp_auth *p_auth)
{
    system_port port;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    port = p_auth->port;

    // 3rd Authentication

    // Resume comparing Ri with Ri'
    // This must be called within 128 frame cycle.
    ret = hdcptx14_compare_ri_with_rid(port);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_akecmd(system_port port, uint8_t akecmd, akecmd_ctrl *p_akectrl)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_AKECMD_NUM];
    uint32_t ret, ret2;
    uint32_t i;

    if (p_akectrl == NULL) {
        return ERR_NULL_POINTER;
    }

    // check HPD of HDMI Tx
    ret = hdcptxxx_check_hpd(port);
    CHECK_ERROR(ret);

    SET_ACCESS(access, &port, cmd, HDCPTX22_AKECMD_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_H22CMD_ADDR, akecmd, HDCPTX22_AKECMD_MASK);

    for (i=0; i<p_akectrl->cmd_retry_max; i++) {
        CHECK_TX_ABORT();
        ret = lf9129_device_write(&access);
        CHECK_ERROR(ret);
        // check error
        ret = hdcptx22_akeerr(port, p_akectrl);
        if (ret == ERR_OK) {
            break;
        } else {
            // check HPD of HDMI Tx
            ret2 = hdcptxxx_check_hpd(port);
            CHECK_ERROR(ret2);
            if (ret == ERR_HDCPTX22_DDC_NO_ACK_REPLY) {
                // DDC Reset
                ret = hdcptx22_reset_ddc(port);
                CHECK_ERROR(ret);
            } else if (ret == ERR_HDCPTX22_DDC_FIXED_LOW_LEVEL) {
                // DDC Check
                ret = hdcptx22_check_ddc(port);
                CHECK_ERROR(ret);
            } else {
                return ret;
            }
        }
        SLEEP_Delay_ms(p_akectrl->cmd_retry_interval);
    }
    if (i == p_akectrl->cmd_retry_max) {
        return ERR_HDCPTX22_DDC_ACCESS_RETRY;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_akeerr(system_port port, akecmd_ctrl *p_akectrl)
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
        ret = hdcptx22_read_akesta(port, &akesta);
        CHECK_ERROR(ret);
        if ((akesta & HDCPTX22_AKESTA_REG_MASK) == HDCPTX22_AKESTA_NORMAL_END) {
            ret = ERR_OK;
            break;
        } else if ((akesta & HDCPTX22_AKESTA_REG_MASK) == HDCPTX22_AKESTA_ERROR_END) {
            ret = hdcptx22_read_akeerr(port, &akeerr);
            CHECK_ERROR(ret);
            ret = ERR_HDCPTX22_AKECMD_NORMAL_END | akeerr;
            return ret;
        } else {
            // retry
        }
        SLEEP_Delay_ms(p_akectrl->ack_retry_interval);
    }
    if (i == p_akectrl->ack_retry_max) {
        ret = ERR_HDCPTX22_AKESTA_READ_RETRY;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_read_akesta(system_port port, uint8_t *p_sta)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_AKESTA_NUM];
    uint32_t ret;

    if (p_sta == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPTX22_AKESTA_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKE2ST_ADDR, 0, HDCPTX22_AKESTA_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_sta = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_read_akeerr(system_port port, uint8_t *p_err)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_AKEERR_NUM];
    uint32_t ret;

    if (p_err == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPTX22_AKEERR_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKE2ER_ADDR, 0, HDCPTX22_AKEERR_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_err = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_reset_ddc(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_RESET_DDC_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX22_RESET_DDC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_H22CMD_ADDR, HDCPTX22_RESET_DDC_CMD, HDCPTX22_RESET_DDC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_check_ddc(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_CHECK_DDC_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX22_CHECK_DDC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_H22CMD_ADDR, HDCPTX22_CHECK_DDC_CMD, HDCPTX22_CHECK_DDC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_set_ddc_property(system_port port, uint8_t saddr, uint8_t oaddr, uint16_t len)
{
    system_block_access baccess;
    uint8_t data[HDCPTX22_SET_DDC_PROPERTY_LEN];
    uint32_t ret;

    if (len == 0 || len > HDMITX_DDC_MAX_BUF_SIZE) {
        return ERR_INVALID_SIZE;
    }

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCP_H22CMA_ADDR,
                        HDCPTX22_SET_DDC_PROPERTY_LEN, data);
    // slave address
    SET_BLOCK_DATA(data, 0, saddr);                            // 0x3A94
    // offset address
    SET_BLOCK_DATA(data, 1, oaddr);                            // 0x3A95
    // length (1byte: 0, 2byte: 1, ... 256byte: 255(0xFF))
    SET_BLOCK_DATA(data, 2, (len - 1));                        // 0x3A96

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_set_ddc_subcmd(system_port port, uint8_t subcmd)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_SET_DDC_SUBCMD_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX22_SET_DDC_SUBCMD_NUM);
    // subcmd
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_H22CMC_ADDR, subcmd, HDCPTX22_SET_DDC_SUBCMD_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_set_random_seed(system_port port)
{
    uint8_t seed[HDCPTX22_RANDOM_SEED_BYTE_SIZE];
    uint32_t ret;

    // Select DDC buffer page
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_3);
    CHECK_ERROR(ret);
    // Read random seed
    ret = hdcptx22_get_random_seed_from_register(port, seed);
    CHECK_ERROR(ret);
    // check already generated
    if (seed[0]==0 && seed[4]==0 && seed[8]==0 && seed[12]==0) {
        // Generate random seed
        ret =  hdcptx22_generate_random_seed(port);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_generate_random_seed(system_port port)
{
    uint8_t an[HDCPTX14_AN_BYTE_SIZE];
    uint8_t seed[HDCPTX22_RANDOM_SEED_BYTE_SIZE];
    uint32_t i, j, idx, cnt;
    uint32_t ret;

    // get An(8byte) x 2
    ret = ERR_INVALID_SIZE;
    cnt = HDCPTX22_RANDOM_SEED_BYTE_SIZE / HDCPTX14_AN_BYTE_SIZE;
    idx = 0;
    for (i=0; i<cnt; i++) {
        // Generate An
        ret = hdcptx14_generate_an(port);
        CHECK_ERROR(ret);
        // Get An from register
        ret = hdcptx14_get_an_from_register(port, an);
        CHECK_ERROR(ret);
        for (j=0 ; j<HDCPTX14_AN_BYTE_SIZE; j++) {
            seed[idx] = an[j];
            idx++;
        }
    }
    // page(0x7c): 3 (Select DDC buffer page)
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_3);
    CHECK_ERROR(ret);
    // set seed to register
    ret = hdcptx22_set_random_seed_to_register(port, seed);
    CHECK_ERROR(ret);
    // Set DDC sub-command to generate random value
    ret = hdcptx22_set_ddc_subcmd(port, HDCPTX22_GENERATE_RANDOM_SEED_SUBCMD);
    CHECK_ERROR(ret);
    // Looping execution for random seed at least 32 times
    for (i=0; i<HDCPTX22_GENERATE_RANDOM_SEED_LOOP; i++) {
        // Generate random seed
        ret = hdcptx22_akecmd(port, HDCPTX22_GENERATE_RANDOM_SEED_CMD, &akectrl_process);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_set_random_seed_to_register(system_port port, uint8_t *p_rand)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_rand == NULL) {
        return ERR_NULL_POINTER;
    }

    // page: 3

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_RANDOM_RANDOM_ADDR,
                                        HDCPTX22_RANDOM_SEED_SET_REGISTER_LEN,
                                        p_rand);
    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_random_seed_from_register(system_port port, uint8_t *p_rand)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_rand == NULL) {
        return ERR_NULL_POINTER;
    }

    // page: 3

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_RANDOM_RANDOM_ADDR,
                                        HDCPTX22_RANDOM_SEED_GET_REGISTER_LEN,
                                        p_rand);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_ake2rs(system_port port, uint8_t rst)
{
    system_reg_access access;
    system_command cmd[HDCPTX22_AKE2RS_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTX22_AKE2RS_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKE2RS_ADDR, rst, HDCPTX22_AKE2RS_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_read_hdcp2version(system_port port, uint8_t *p_ver)
{
    uint8_t hdcp2version;
    uint32_t ret;

    if (p_ver == NULL) {
        return ERR_NULL_POINTER;
    }

    // Set DDC slave/offset address and length to read HDCP2Version
    ret = hdcptx14_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX22_OFFSET_ADDR_HDCP2VERSION_1_R,
                                        HDCPTX22_HDCP2VERSION_BYTE_SIZE);
    CHECK_ERROR(ret);
    // Read HDCP2Version from DDC
    ret = hdcptx14_akecmd(port, HDCPTX14_READ_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);
    // page(0x7c): 0 (Select DDC buffer page)
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_0);
    CHECK_ERROR(ret);
    // Get HDCP2Version from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            1,
                                            &hdcp2version);
    CHECK_ERROR(ret);
    // set output data
    *p_ver = hdcp2version;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_set_txcaps_to_register(system_port port)
{
    uint8_t data[HDCPTX22_SET_TXCAPS_REGISTER_LEN];
    uint32_t ret;

    // page(0x7c): 3

    // Set TxCaps (0x020000)
    // slave address
    SET_BLOCK_DATA(data, 0, HDCPTX22_TXCAPS_DATA01);            // 0x3C50
    // offset address
    SET_BLOCK_DATA(data, 1, HDCPTX22_TXCAPS_DATA02);            // 0x3C51
    // length (1byte: 0, 2byte: 1, ... 256byte: 255(0xFF))
    SET_BLOCK_DATA(data, 2, HDCPTX22_TXCAPS_DATA03);            // 0x3C52

    // page(0x7c): 3
    ret = hdcptxxx_write_ddc_buffer(port, HDCPTX22_TXCAPS_IN_DDCBUF3_OFFSET01,
                                            HDCPTX22_SET_TXCAPS_REGISTER_LEN,
                                            data);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_ake_init(system_port port)
{
    uint32_t snv = 0;
    uint32_t csm = 0;
    uint32_t snm = 0;
    uint32_t ret;

    ret = hdcptx22_akecmd(port, HDCPTX22_AKE_INIT_CMD, &akectrl_ake_init);
    CHECK_ERROR(ret);
    ret = hdcptxxx_ctrl_cnt_seq_num_v(&snv, HDCPTX_CTRL_CNT_SET);
    CHECK_ERROR(ret);
    ret = hdcptxxx_ctrl_cnt_stream_manage(&csm, HDCPTX_CTRL_CNT_SET);
    CHECK_ERROR(ret);
    ret = hdcptxxx_ctrl_cnt_seq_num_m(&snm, HDCPTX_CTRL_CNT_SET);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_verify_signature(system_port port)
{
    uint32_t ret;

    ret = hdcptx22_akecmd(port, HDCPTX22_VERIFY_SIGNATURE_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_tx_receiver_id(system_port port, uint8_t *p_tx_rxid)
{
    uint32_t ret;

    if (p_tx_rxid == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 3

    // Read Tx_Receiver_ID
    // page(0x7c): 3
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTX22_TX_RECEIVER_ID_IN_DDCBUF3_OFFSET,
                                            HDCPTX22_GET_TX_RECEIVER_ID_LEN,
                                            p_tx_rxid);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_init_stored_receiver_id(system_port port)
{
    uint32_t ret;
    uint8_t init_stored_rxid[HDCPTX22_RECEIVER_ID_BYTE_SIZE] = { 0 };

    // page(0x7c): 3 (Select DDC buffer page)
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_3);
    CHECK_ERROR(ret);
    // Init Stored_Receiver_ID by padding 0
    // page(0x7c): 3
    ret = hdcptxxx_write_ddc_buffer(port, HDCPTX22_STORED_RECEIVER_ID_IN_DDCBUF3_OFFSET,
                                            HDCPTX22_GET_STORED_RECEIVER_ID_LEN,
                                            init_stored_rxid);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_stored_receiver_id(system_port port, uint8_t *p_stored_rxid)
{
    uint32_t ret;

    if (p_stored_rxid == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 3

    // Read Stored_Receiver_ID
    // page(0x7c): 3
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTX22_STORED_RECEIVER_ID_IN_DDCBUF3_OFFSET,
                                            HDCPTX22_GET_STORED_RECEIVER_ID_LEN,
                                            p_stored_rxid);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_check_master_key_km(system_port port,
                    uint8_t *p_tx_rxid, uint8_t *p_stored_rxid, uint8_t *p_chk)
{
    uint32_t i;

    if (p_tx_rxid == NULL || p_stored_rxid == NULL || p_chk == NULL) {
        return ERR_NULL_POINTER;
    }

    // Verify Receiver_ID
    *p_chk = HDCPTX22_CHECK_MASTER_KEY_KM_STORED;
    for (i=0; i<HDCPTX22_RECEIVER_ID_BYTE_SIZE; i++) {
        if(p_tx_rxid[i] != p_stored_rxid[i]) {
            *p_chk = HDCPTX22_CHECK_MASTER_KEY_KM_NOT_STORED;
            break;
        }
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_ake_no_stored_km(system_port port)
{
    uint32_t ret;

    ret = hdcptx22_akecmd(port, HDCPTX22_AKE_NO_STORED_KM_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_ake_pairing_info(system_port port)
{
    uint32_t ret;

    ret = hdcptx22_akecmd(port, HDCPTX22_AKE_PAIRING_INFO_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_ake_stored_km(system_port port)
{
    uint32_t ret;

    ret = hdcptx22_akecmd(port, HDCPTX22_AKE_STORED_KM_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_lc_init(system_port port)
{
    uint32_t i;
    uint32_t ret;

    // HDCP2.2 specification allows retrying of 1024 times or less.
    for (i=0; i<HDCPTX22_LC_INIT_RETRY_MAX; i++) {
        ret = hdcptx22_akecmd(port, HDCPTX22_LC_INIT_CMD, &akectrl_lc_init);
        SET_HV_ERRNO(ret);
        if (ret == ERR_OK) break;
        CHECK_INACTIVE_ERROR(ret);
        CHECK_TX_ABORT_ERROR(ret);
        SLEEP_Delay_ms(HDCP_SLEEP_TIME_1MSEC);
    }
    if (i == HDCPTX22_LC_INIT_RETRY_MAX) {
        return ERR_HDCPTX22_LC_INIT_RETRY;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_ske_send_eks(system_port port)
{
    uint32_t ret;

    ret = hdcptx22_akecmd(port, HDCPTX22_SKE_SEND_EKS_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_read_rxcaps(system_port port, uint8_t *p_rxcaps)
{
    uint32_t ret;

    if (p_rxcaps == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 3

    // Read RxCaps
    // page(0x7c): 3
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTX22_RXCAPS_IN_DDCBUF3_OFFSET,
                                            HDCPTX22_READ_RXCAPS_LEN,
                                            p_rxcaps);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_read_rxstatus(system_port port, uint8_t *p_rxstatus)
{
    uint32_t ret;

    if (p_rxstatus == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // Set DDC slave/offset address and length to read RxStatus
    ret = hdcptx22_set_ddc_property(port, HDCPTXXX_SLAVE_ADDR,
                                        HDCPTX22_OFFSET_ADDR_RXSTATUS_2_R,
                                        HDCPTX22_RXSTATUS_BYTE_SIZE);
    CHECK_ERROR(ret);
    // read RxStatus command
    ret = hdcptx22_akecmd(port, HDCPTX22_READ_DDC_CMD, &akectrl_access);
    CHECK_ERROR(ret);
    // Get RxStatus from DDC buffer
    // page(0x7c): 0
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTXXX_DDC_TOP_IN_DDCBUF0_OFFSET,
                                            HDCPTX22_RXSTATUS_BYTE_SIZE,
                                            p_rxstatus);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_repeaterauth_send_receiverid_list(system_port port)
{
    uint32_t ret;

    // Set DDC sub-command to read RepeaterAuth_Send_ReceiverID_List
    ret = hdcptx22_set_ddc_subcmd(port, HDCPTX22_REPEATERAUTH_SEND_RECEIVERID_LIST_SUBCMD);
    CHECK_ERROR(ret);
    // RepeaterAuth_Send_ReceiverID_List command
    ret = hdcptx22_akecmd(port, HDCPTX22_REPEATERAUTH_SEND_RECEIVERID_LIST_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_rxinfo(system_port port, uint8_t *p_rxinfo)
{
    uint32_t ret;

    if (p_rxinfo == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // Get RxInfo from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTX22_RXINFO_IN_DDCBUF0_OFFSET,
                                            HDCPTX22_RXINFO_BYTE_SIZE,
                                            p_rxinfo);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_seq_num_v_from_ddc_buffer(system_port port, uint8_t *p_num)
{
    uint32_t ret;

    if (p_num == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // Get seq_num_V from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTX22_SEQ_NUM_V_IN_DDCBUF0_OFFSET,
                                            HDCPTX22_SEQ_NUM_V_BYTE_SIZE,
                                            p_num);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_ksvlist_from_ddc_buffer(system_port port, uint8_t cnt, uint8_t *p_ksv_list)
{
    uint32_t ddc_data_size;
    uint32_t ret;

    if (p_ksv_list == NULL) {
        return ERR_NULL_POINTER;
    }
    if (cnt > HDCPTX22_RXINFO_DEVICE_COUNT_MAX) {
        return ERR_HDCPTX22_MAX_DEVS_EXCEEDED;
    }

    // page(0x7c): 0

    ret = ERR_NG;
    ddc_data_size = cnt * HDCPTX22_RECEIVER_ID_BYTE_SIZE;
    // Get Receiver ID List from DDC buffer
    ret = hdcptxxx_read_ddc_buffer(port, HDCPTX22_RECEIVER_ID_LIST_IN_DDCBUF0_OFFSET,
                                            ddc_data_size,
                                            p_ksv_list);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_repeaterauth_send_ack(system_port port)
{
    uint32_t ret;

    // Set DDC sub-command to read RepeaterAuth_Send_Ack
    ret = hdcptx22_set_ddc_subcmd(port, HDCPTX22_REPEATERAUTH_SEND_ACK_SUBCMD);
    CHECK_ERROR(ret);
    // RepeaterAuth_Send_Ack command
    ret = hdcptx22_akecmd(port, HDCPTX22_REPEATERAUTH_SEND_ACK_CMD, &akectrl_access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_repeaterauth_stream_manage(system_port port, uint32_t num,
                                                    hdmitx_hdcp_stream_type stype, uint8_t *p_rxinfo)
{
    uint8_t header[HDCPTX22_STREAM_MANAGE_HEADER_BYTE_SIZE];
    uint8_t streamid_type[HDCPTX22_STREAMID_TYPE_BYTE_SIZE];
    uint16_t k, i;
    uint8_t type;
    uint32_t ret;

    if (p_rxinfo == NULL) {
        return ERR_NULL_POINTER;
    }

    // page(0x7c): 0

    // set msg_id
    header[HDCPTX22_MES_ID_OFFSET] = HDCPTX22_MES_ID_PARAM;
    // set seq_num_M
    header[HDCPTX22_SEQ_NUM_M_OFFSET + 0] = (uint8_t)((num & 0x00FF0000) >> 16);
    header[HDCPTX22_SEQ_NUM_M_OFFSET + 1] = (uint8_t)((num & 0x0000FF00) >> 8);
    header[HDCPTX22_SEQ_NUM_M_OFFSET + 2] = (uint8_t)((num & 0x000000FF) >> 0);
    // set k
    k = HDCPTX22_K_PARAM_1;
    header[HDCPTX22_K_OFFSET + 0] = (uint8_t)((k & 0xFF00) >> 8);
    header[HDCPTX22_K_OFFSET + 1] = (uint8_t)((k & 0x00FF) >> 0);
    ret = hdcptxxx_write_ddc_buffer(port, HDCPTX22_MSG_ID_IN_DDCBUF0_OFFSET,
                                            HDCPTX22_STREAM_MANAGE_HEADER_BYTE_SIZE,
                                            header);
    CHECK_ERROR(ret);

    // select type
    if (stype == HDMITX_HDCP_STREAM_TYPE_0) {
        type = HDCPTX22_TYPE_PARAM_TYPE_0;
    } else if (stype == HDMITX_HDCP_STREAM_TYPE_1) {
        type = HDCPTX22_TYPE_PARAM_TYPE_1;
    } else if (stype == HDMITX_HDCP_STREAM_TYPE_AUTO) {
        if ((p_rxinfo[1] & HDCPTX22_HDCP2_0_REPEATER_DOWNSTREAM_BIT)
            || (p_rxinfo[1] & HDCPTX22_HDCP1_DEVICE_DOWNSTREAM_BIT)) {
            // Type 1 Content Stream: Must not be transmitted by the HDCP Repeater
            // to HDCP 1.x-compliant Devices and HDCP 2.0-compliant Repeaters.
            type = HDCPTX22_TYPE_PARAM_TYPE_1;
        } else {
            // Type 0 Content Stream: May be transmitted by the HDCP Repeater
            // to all HDCP Devices.
            type = HDCPTX22_TYPE_PARAM_TYPE_0;
        }
    } else {
        return ERR_NG;
    }

    for (i=0; i<k; i++) {
        // set STREAM_ID
        streamid_type[HDCPTX22_STREAM_ID_OFFSET] = HDCPTX22_STREAM_ID_PARAM;
        // set Type
        streamid_type[HDCPTX22_TYPE_OFFSET] = type;
        ret = hdcptxxx_write_ddc_buffer(port, (HDCPTX22_STREAMID_TYPE_00_IN_DDCBUF0_OFFSET
                                                + (HDCPTX22_STREAMID_TYPE_BYTE_SIZE * i)),
                                                HDCPTX22_STREAMID_TYPE_BYTE_SIZE,
                                                streamid_type);
        CHECK_ERROR(ret);
    }

    // RepeaterAuth_Stream_Manage command
    ret = hdcptx22_akecmd(port, HDCPTX22_REPEATERAUTH_STREAM_MANAGE_CMD, &akectrl_sm);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_check_rxstatus(system_port port)
{
    uint8_t rxstatus[HDCPTX22_RXSTATUS_BYTE_SIZE];
    uint32_t ret;
    
    // page(0x7c): 0 (Select DDC buffer page)
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_0);
    CHECK_ERROR(ret);
    // check READY bit and REAUTH_REQ of RxStatus
    // Read RxStatus
    // page(0x7c): 0
    ret = hdcptx22_read_rxstatus(port, rxstatus);
    CHECK_ERROR(ret);
    if((rxstatus[HDCPTX22_RXSTATUS_INDEX_15__8] & HDCPTX22_RXSTATUS_15__8_READY_BIT)
            == HDCPTX22_RXSTATUS_15__8_READY_BIT){
        // READY bit = 1
        return ERR_HDCPTX22_RXSTATUS_READY;
    } else if ((rxstatus[HDCPTX22_RXSTATUS_INDEX_15__8] & HDCPTX22_RXSTATUS_15__8_REAUTH_REQ_BIT)
                    == HDCPTX22_RXSTATUS_15__8_REAUTH_REQ_BIT) {
        // Re-Authentication request
        return ERR_HDCPTX22_REAUTH_REQ;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_authentication(hdmitx_hdcp_auth *p_auth)
{
    system_port port;
    uint8_t tx_rxid[HDCPTX22_RECEIVER_ID_BYTE_SIZE];
    uint8_t stored_rxid[HDCPTX22_RECEIVER_ID_BYTE_SIZE];
    uint8_t chk;
    uint8_t rxcaps[HDCPTX22_RXCAPS_BYTE_SIZE];
    uint32_t i;
    system_config_retry retry_init_auth;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    port = p_auth->port;

    // 1st Authentication

    // page(0x7c): 3 (Select DDC buffer page)
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_3);
    CHECK_HDCP22_ERROR(ret, port);
    // Set TxCaps to register
    // page(0x7c): 3
    ret = hdcptx22_set_txcaps_to_register(port);
    CHECK_HDCP22_ERROR(ret, port);
    // ENC_DIS
    ret = hdcptxxx_output_eess(port, HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_DIS);
    CHECK_HDCP22_ERROR(ret, port);
    // Reset authentication process
    ret = hdcptxxx_akesrst(port, HDCPTXXX_AKESRST_RESET);
    CHECK_HDCP22_ERROR(ret, port);
    // Reset HDCP2.2 authentication process
    ret = hdcptx22_ake2rs(port, HDCPTX22_AKE2RS_RESET);
    CHECK_HDCP22_ERROR(ret, port);
    CHECK_TX_ABORT();
    // Wait 200ms (tentative)
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_200MSEC);
    // get retry info
    ret = hdcptxxx_ctrl_retry_init_auth(&retry_init_auth, HDCPTX_CTRL_RETRY_GET);
    CHECK_HDCP22_ERROR(ret, port);
    // retry initial authentication
    for (i=0; i<retry_init_auth.times; i++) {
        // AKE_Init
        ret = hdcptx22_ake_init(port);
        SET_HV_ERRNO(ret);
        if (ret == ERR_OK) break;
        CHECK_HDCP22_INACTIVE_ERROR(ret, port);
        CHECK_HDCP22_TX_ABORT_ERROR(ret, port);
        SLEEP_Delay_ms(retry_init_auth.interval);
    }
    if (i == retry_init_auth.times) {
        ret = hdmitx_hdcp_init(port);
        CHECK_HDCP22_ERROR(ret, port);
        return ERR_HDCPTX22_INIT_AUTH_RETRY;
    }
    // Verify signature
    ret = hdcptx22_verify_signature(port);
    CHECK_HDCP22_ERROR(ret, port);
    // get Tx_Receiver_ID
    // page(0x7c): 3
    ret = hdcptx22_get_tx_receiver_id(port, tx_rxid);
    CHECK_HDCP22_ERROR(ret, port);
    // get Stored_Receiver_ID
    // page(0x7c): 3
    ret = hdcptx22_get_stored_receiver_id(port, stored_rxid);
    CHECK_HDCP22_ERROR(ret, port);
    // Check master key km
    ret = hdcptx22_check_master_key_km(port, tx_rxid, stored_rxid, &chk);
    CHECK_HDCP22_ERROR(ret, port);
    MES("LF9129 hdcptx22_authentication chk = %d",chk);
    if (chk == HDCPTX22_CHECK_MASTER_KEY_KM_STORED) {
        // AKE_Stored_km
        ret = hdcptx22_ake_stored_km(port);
        CHECK_HDCP22_ERROR(ret, port);
    } else {
        // AKE_No_Stored_km
        ret = hdcptx22_ake_no_stored_km(port);
        CHECK_HDCP22_ERROR(ret, port);
        // AKE_Pairing_Info
        ret = hdcptx22_ake_pairing_info(port);
        CHECK_HDCP22_ERROR(ret, port);
    }
    // LC_Init
    ret = hdcptx22_lc_init(port);
    CHECK_ERROR(ret);
    CHECK_TX_ABORT();
    // Wait 200ms (tentative)
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_200MSEC);
    // SKE_Send_Eks
    ret = hdcptx22_ske_send_eks(port);
    CHECK_ERROR(ret);
    CHECK_TX_ABORT();
    // Wait 200ms (standard)
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_200MSEC);
    // check HPD of HDMI Tx
    ret = hdcptxxx_check_hpd(port);
    CHECK_ERROR(ret);
    // HDCP Encryption Reset
    ret = hdcptxxx_encryption_reset(port);
    CHECK_ERROR(ret);
    // ENC_EN
    ret = hdcptxxx_output_eess(port, HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_EN);
    CHECK_ERROR(ret);
    // Read RxCaps
    // page(0x7c): 3
    ret = hdcptx22_read_rxcaps(port, rxcaps);
    CHECK_ERROR(ret);

    // set is_repeater member
    MES("(rxcaps[HDCPTX22_RXCAPS_INDEX__7__0] = 0x%x, HDCPTX22_RXCAPS_REPEATER_BIT = 0x%x", rxcaps[HDCPTX22_RXCAPS_INDEX__7__0], HDCPTX22_RXCAPS_REPEATER_BIT);
    MES("(rxcaps[HDCPTX22_RXCAPS_INDEX__7__0] & HDCPTX22_RXCAPS_REPEATER_BIT = 0x%x", (rxcaps[HDCPTX22_RXCAPS_INDEX__7__0] & HDCPTX22_RXCAPS_REPEATER_BIT) );
    MES("HDCPTX22_RXCAPS_REPEATER_BIT = 0x%x", HDCPTX22_RXCAPS_REPEATER_BIT);
    if ((rxcaps[HDCPTX22_RXCAPS_INDEX__7__0] & HDCPTX22_RXCAPS_REPEATER_BIT)
            == HDCPTX22_RXCAPS_REPEATER_BIT) {
        p_auth->is_repeater = HDMITX_HDCP_REPEATER;
    } else {
        p_auth->is_repeater = HDMITX_HDCP_NOT_REPEATER;
    }
    // set receiver ID
    for (i=0; i<HDCPTX22_RECEIVER_ID_BYTE_SIZE; i++) {
        p_auth->bksv[i] = tx_rxid[i];
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_repeater_authentication(hdmitx_hdcp_repeater_auth *p_repeater_auth)
{
    system_port port;
    uint8_t rxcaps[HDCPTX22_RXCAPS_BYTE_SIZE];
    system_config_retry retry_read_rxstatus;
    uint8_t rxstatus[HDCPTX22_RXSTATUS_BYTE_SIZE];
    uint8_t rxinfo[HDCPTX22_RXINFO_BYTE_SIZE];
    uint8_t down_count, down_depth;
    uint8_t seq_num_v[HDCPTX22_SEQ_NUM_V_BYTE_SIZE];
    static uint8_t pre_seq_num_v[HDCPTX22_SEQ_NUM_V_BYTE_SIZE] = { HDCPTX22_SEQ_NUM_V_00 };
    uint32_t snv;
    uint32_t csm;
    uint8_t ksv_list[HDCPTX22_RXINFO_DEVICE_COUNT_MAX * HDMITX_HDCP_KSV_BYTE_SIZE];
    system_config_retry retry_stream_manage;
    uint32_t snm;
    uint32_t i, j;
    uint32_t ret;

    if (p_repeater_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    port = p_repeater_auth->port;

    // 2nd Authentication

    // page(0x7c): 3 (Select DDC buffer page)
    MES("%s, %d,  (Select DDC buffer page)", __func__, __LINE__);
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_3);
    CHECK_ERROR(ret);
    // Read RxCaps
    // page(0x7c): 3
    MES("%s, %d,  Read RxCaps", __func__, __LINE__);

    if(!xbh_9129_is_repeater) {
        ret = hdcptx22_read_rxcaps(port, rxcaps);
        CHECK_ERROR(ret);

        // check REPEATER bit
        MES("%s, %d, check REPEATER bit ", __func__, __LINE__);

        if ((rxcaps[HDCPTX22_RXCAPS_INDEX__7__0] & HDCPTX22_RXCAPS_REPEATER_BIT)
                != HDCPTX22_RXCAPS_REPEATER_BIT) {
            return ERR_NG;
        }
    }

    // get retry info
    MES("%s, %d, get retry info ", __func__, __LINE__);

    ret = hdcptxxx_ctrl_retry_read_rxstatus(&retry_read_rxstatus, HDCPTX_CTRL_RETRY_GET);
    CHECK_ERROR(ret);
    // page(0x7c): 0 (Select DDC buffer page)
    MES("%s, %d,   (Select DDC buffer page)", __func__, __LINE__);

    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_0);
    CHECK_ERROR(ret);
    // check READY bit and REAUTH_REQ of RxStatus
    MES("%s, %d,  check READY bit and REAUTH_REQ of RxStatus", __func__, __LINE__);

    for (i=0; i<retry_read_rxstatus.times; i++) {
        // Read RxStatus
        // page(0x7c): 0
        MES("%s, %d,  Read RxStatus", __func__, __LINE__);

        ret = hdcptx22_read_rxstatus(port, rxstatus);
        CHECK_ERROR(ret);
        MES("%s, %d,  rxstatus[HDCPTX22_RXSTATUS_INDEX_15__8] = 0x%x", __func__, __LINE__, rxstatus[HDCPTX22_RXSTATUS_INDEX_15__8]);
        if((rxstatus[HDCPTX22_RXSTATUS_INDEX_15__8] & HDCPTX22_RXSTATUS_15__8_READY_BIT)
                == HDCPTX22_RXSTATUS_15__8_READY_BIT){
            MES("%s, %d, READY bit = 1 ", __func__, __LINE__);

            // READY bit = 1
            break;
        } else if ((rxstatus[HDCPTX22_RXSTATUS_INDEX_15__8] & HDCPTX22_RXSTATUS_15__8_REAUTH_REQ_BIT)
                        == HDCPTX22_RXSTATUS_15__8_REAUTH_REQ_BIT) {
            MES("%s, %d,  Re-Authentication request", __func__, __LINE__);

            // Re-Authentication request
            return ERR_HDCPTX22_REAUTH_REQ;
        }
        SLEEP_Delay_ms(retry_read_rxstatus.interval);
    }
    if (i == retry_read_rxstatus.times) {
        MES("%s, %d,  i == retry_read_rxstatus.times", __func__, __LINE__);

        return ERR_HDCPTX22_RXSTATUS_READ_RETRY;
    }
    // RepeaterAuth_Send_ReceiverID_List
    MES("%s, %d,  RepeaterAuth_Send_ReceiverID_List", __func__, __LINE__);

    ret = hdcptx22_repeaterauth_send_receiverid_list(port);
    CHECK_ERROR(ret);
    // get RxInfo
    // page(0x7c): 0
    MES("%s, %d,  get RxInfo", __func__, __LINE__);

    ret = hdcptx22_get_rxinfo(port, rxinfo);
    CHECK_ERROR(ret);
    // RxInfo[15: 8]: rxinfo[0]
    // RxInfo[ 7: 0]: rxinfo[1]
    // DEVICE_COUNT: RxInfo[ 8: 4]
    down_count = ((rxinfo[1] >> HDCPTX22_RXINFO_DEVICE_COUNT_BIT) & HDCPTX22_RXINFO_DEVICE_COUNT_7_4_MASK) |
                 ((rxinfo[0] << HDCPTX22_RXINFO_DEVICE_COUNT_BIT) & HDCPTX22_RXINFO_DEVICE_COUNT_8_8_MASK);
    down_depth = (rxinfo[0] >> HDCPTX22_RXINFO_DEPTH_BIT) & HDCPTX22_RXINFO_DEPTH_11_9_MASK;
    // get seq_num_V
    // page(0x7c): 0
    MES("%s, %d, get seq_num_V ", __func__, __LINE__);

    ret = hdcptx22_get_seq_num_v_from_ddc_buffer(port, seq_num_v);
    CHECK_ERROR(ret);

    MES("%s, %d, pre_seq_num_v[0] = 0x%x ", __func__, __LINE__, pre_seq_num_v[0]);
    MES("%s, %d, pre_seq_num_v[1] = 0x%x ", __func__, __LINE__, pre_seq_num_v[1]);
    MES("%s, %d, pre_seq_num_v[2] = 0x%x ", __func__, __LINE__, pre_seq_num_v[2]);
    MES("%s, %d, seq_num_v[0] = 0x%x ", __func__, __LINE__, seq_num_v[0]);
    MES("%s, %d, seq_num_v[1] = 0x%x ", __func__, __LINE__, seq_num_v[1]);
    MES("%s, %d, seq_num_v[2] = 0x%x ", __func__, __LINE__, seq_num_v[2]);


    // check seq_num_V rolls over (roll-over)
    if((pre_seq_num_v[0] == HDCPTX22_SEQ_NUM_V_FF)
        && (pre_seq_num_v[1] == HDCPTX22_SEQ_NUM_V_FF)
        && (pre_seq_num_v[2] == HDCPTX22_SEQ_NUM_V_FF)
        && (seq_num_v[0] == HDCPTX22_SEQ_NUM_V_00)
        && (seq_num_v[1] == HDCPTX22_SEQ_NUM_V_00)
        && (seq_num_v[2] == HDCPTX22_SEQ_NUM_V_00)) {
        // for restart AKE_Init
        pre_seq_num_v[0] = seq_num_v[0];
        pre_seq_num_v[1] = seq_num_v[1];
        pre_seq_num_v[2] = seq_num_v[2];
        MES("%s, %d,  for restart AKE_Init ERR_HDCPTX22_SEQ_NUM_V_ROLL_OVER, nums: 0x%x", __func__, __LINE__, ERR_HDCPTX22_SEQ_NUM_V_ROLL_OVER);

        return ERR_HDCPTX22_SEQ_NUM_V_ROLL_OVER;
    }
    pre_seq_num_v[0] = seq_num_v[0];
    pre_seq_num_v[1] = seq_num_v[1];
    pre_seq_num_v[2] = seq_num_v[2];
    // check seq_num_V == 0 after AKE_Init
    MES("%s, %d, check seq_num_V == 0 after AKE_Init ", __func__, __LINE__);

    ret = hdcptxxx_ctrl_cnt_seq_num_v(&snv, HDCPTX_CTRL_CNT_GET);
    CHECK_ERROR(ret);
    if ((snv == 0)
        && ((seq_num_v[0] != HDCPTX22_SEQ_NUM_V_00)
            || (seq_num_v[1] != HDCPTX22_SEQ_NUM_V_00)
            || (seq_num_v[2] != HDCPTX22_SEQ_NUM_V_00))) {
        MES("%s, %d,  return ERR_HDCPTX22_SEQ_NUM_V_INVALID;", __func__, __LINE__);

        return ERR_HDCPTX22_SEQ_NUM_V_INVALID;
    }
    snv=1;
    MES("%s, %d, hdcptxxx_ctrl_cnt_seq_num_v ", __func__, __LINE__);

    ret = hdcptxxx_ctrl_cnt_seq_num_v(&snv, HDCPTX_CTRL_CNT_SET);
    CHECK_ERROR(ret);
    // get Receiver ID List
    // page(0x7c): 0
    MES("%s, %d,  get Receiver ID List", __func__, __LINE__);

    ret = hdcptx22_get_ksvlist_from_ddc_buffer(port, down_count, ksv_list);
    CHECK_ERROR(ret);
    // RepeaterAuth_Send_Ack
    //   It is a tentative reply.
    //   If Receiver ID List has the problems, it is necessary to reset the authentication.
    MES("%s, %d,  RepeaterAuth_Send_Ack", __func__, __LINE__);

    ret = hdcptx22_repeaterauth_send_ack(port);
    CHECK_ERROR(ret);
    // check Content Stream Management not yet transmitted
    MES("%s, %d,  check Content Stream Management not yet transmitted", __func__, __LINE__);

    ret = hdcptxxx_ctrl_cnt_stream_manage(&csm, HDCPTX_CTRL_CNT_GET);
    CHECK_ERROR(ret);
    if (csm == 0) {
        // get retry info
        MES("%s, %d,  get retry info", __func__, __LINE__);

        ret = hdcptxxx_ctrl_retry_stream_manage(&retry_stream_manage, HDCPTX_CTRL_RETRY_GET);
        CHECK_ERROR(ret);
        // retry stream management
        MES("%s, %d, retry stream management ", __func__, __LINE__);

        for (i=0; i<retry_stream_manage.times; i++) {
            // get seq_num_M
            MES("%s, %d, get seq_num_M ", __func__, __LINE__);

            ret = hdcptxxx_ctrl_cnt_seq_num_m(&snm, HDCPTX_CTRL_CNT_GET);
            CHECK_ERROR(ret);
            // RepeaterAuth_Stream_Manage
            // page(0x7c): 0
            MES("%s, %d,  RepeaterAuth_Stream_Manage", __func__, __LINE__);

            ret = hdcptx22_repeaterauth_stream_manage(port, snm, p_repeater_auth->stream_type,
                                                        rxinfo);
            SET_HV_ERRNO(ret);
            if (ret == ERR_OK) {
                // ENC_EN
                MES("%s, %d,  ENC_EN", __func__, __LINE__);

                ret = hdcptxxx_output_eess(port, HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_EN);
                CHECK_ERROR(ret);
                break;
            }
            CHECK_INACTIVE_ERROR(ret);
            CHECK_TX_ABORT_ERROR(ret);
            SLEEP_Delay_ms(retry_stream_manage.interval);
            // ENC_DIS
            MES("%s, %d, ENC_DIS ", __func__, __LINE__);

            ret = hdcptxxx_output_eess(port, HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_DIS);
            CHECK_ERROR(ret);
            // increment seq_num_M
            MES("%s, %d,  increment seq_num_M", __func__, __LINE__);

            ret = hdcptxxx_ctrl_cnt_seq_num_m(&snm, HDCPTX_CTRL_CNT_INC);
            CHECK_ERROR(ret);
        }
        if (i == retry_stream_manage.times) {
            MES("%s, %d,  i == retry_stream_manage.times", __func__, __LINE__);

            return ERR_HDCPTX22_STREAM_MANAGE_RETRY;
        }
        csm=1;
        MES("%s, %d, hdcptxxx_ctrl_cnt_stream_manage ", __func__, __LINE__);

        ret = hdcptxxx_ctrl_cnt_stream_manage(&csm, HDCPTX_CTRL_CNT_SET);
        CHECK_ERROR(ret);
    }
    // set downstream_count, downstream_depth, ksv_list
    if(down_count > HDCPTX22_RXINFO_DEVICE_COUNT_MAX) {
        MES("%s, %d,  set downstream_count, downstream_depth, ksv_list", __func__, __LINE__);

        return ERR_HDCPTX22_MAX_DEVS_EXCEEDED;
    }
    p_repeater_auth->downstream_count = down_count;
    p_repeater_auth->downstream_depth = down_depth;
    p_repeater_auth->bstatus[0] = rxinfo[0];
    p_repeater_auth->bstatus[1] = rxinfo[1];
    for (i=0; i<down_count; i++) {
        for (j=0; j<HDMITX_HDCP_KSV_BYTE_SIZE; j++) {
            p_repeater_auth->ksv_list[i][j] = ksv_list[i * HDMITX_HDCP_KSV_BYTE_SIZE + j];
        }
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptx22_get_authentication_status(hdmitx_hdcp_auth *p_auth)
{
    system_port port;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    port = p_auth->port;

    // Check REAUTH_REQ and READY
    ret = hdcptx22_check_rxstatus(port);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_decrypt_dev_key(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPTXXX_DECRYPT_DEV_KEY_NUM];
    uint8_t aksv[HDCPTX14_KSV_BYTE_SIZE];
    uint32_t ret;

    // The number of times to issue the decryption command is only once
    // after the hardware reset. Therefore, after checking whether the device key
    // is decrypted, issue the decryption command.

    // check decripte
    ret = hdcptx14_get_aksv_from_register(port, aksv);
    CHECK_ERROR(ret);
    if (!((aksv[0]==0) && (aksv[1]==0) && (aksv[2]==0) && (aksv[3]==0) && (aksv[4]==0))) {
        return ERR_OK;
    }

    SET_ACCESS(access, &port, cmd, HDCPTXXX_DECRYPT_DEV_KEY_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_EDKEYR_00_ADDR,
                        HDCPTXXX_DECRYPT_DEV_KEY_CMD,
                        HDCPTXXX_DECRYPT_DEV_KEY_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);
    ret = hdcptx14_akeerr(port, &akectrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_select_page_of_0x7c(system_port port, uint8_t page)
{
    system_reg_access access;
    system_command cmd[HDCPTXXX_PAGE_SELECT_0X7C_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTXXX_PAGE_SELECT_0X7C_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_THDCPG_ADDR, page, HDCPTXXX_PAGE_SELECT_0X7C_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_write_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_data == NULL) {
        return ERR_NULL_POINTER;
    }

    // page: 0,1,2,3

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_DDCBUF_DDCBUF_ADDR + offset,
                                        (uint32_t)length,
                                        p_data);
    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_read_ddc_buffer(system_port port, uint8_t offset, uint16_t length, uint8_t *p_data)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_data == NULL) {
        return ERR_NULL_POINTER;
    }

    // page: 0,1,2,3

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_DDCBUF_DDCBUF_ADDR + offset,
                                        (uint32_t)length,
                                        p_data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_akesrst(system_port port, uint8_t rst)
{
    system_reg_access access;
    system_command cmd[HDCPTXXX_AKESRST_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTXXX_AKESRST_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKESRST_ADDR, rst, HDCPTXXX_AKESRST_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_output_eess(system_port port, uint8_t eess)
{
    system_reg_access access;
    system_command cmd[HDCPTXXX_OUTPUT_EESS_NUM];
    uint32_t ret;

    MES("%s", __func__);
    SET_ACCESS(access, &port, cmd, HDCPTXXX_OUTPUT_EESS_NUM);
    MES("%d ", __LINE__);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_HDCPEN_00_ADDR, eess, HDCPTXXX_OUTPUT_EESS_MASK);
    MES("%d ", __LINE__);

    ret = lf9129_device_write(&access);
    MES("%d ", __LINE__);

    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_encryption_reset(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPTXXX_ENCRYPTION_RESET_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPTXXX_ENCRYPTION_RESET_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_TSRST_ADDR,
                        HDCPTXXX_ENCRYPTION_RESET_DATA,
                        HDCPTXXX_ENCRYPTION_RESET_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_retry_init_auth(system_config_retry *p_retry, hdcptx_ctrl_retry ctrl)
{
    static system_config_retry hdmitx_hdcp_config_retry_init_auth = {
        SYSTEM_UPDATE_DISABLE,
        HDCPTXXX_CONFIG_RETRY_INIT_AUTH_TIMES_DEFAULT,
        HDCPTXXX_CONFIG_RETRY_INIT_AUTH_INTERVAL_DEFAULT
    };

    if (p_retry == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPTX_CTRL_RETRY_GET) {
        p_retry->times = hdmitx_hdcp_config_retry_init_auth.times;
        p_retry->interval = hdmitx_hdcp_config_retry_init_auth.interval;
    } else if (ctrl == HDCPTX_CTRL_RETRY_SET) {
        if (p_retry->update == SYSTEM_UPDATE_ENABLE) {
            hdmitx_hdcp_config_retry_init_auth.update = SYSTEM_UPDATE_ENABLE;
            hdmitx_hdcp_config_retry_init_auth.times = p_retry->times;
            hdmitx_hdcp_config_retry_init_auth.interval = p_retry->interval;
        }
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_retry_read_rxstatus(system_config_retry *p_retry, hdcptx_ctrl_retry ctrl)
{
    static system_config_retry hdmitx_hdcp_config_retry_read_rxstatus = {
        SYSTEM_UPDATE_DISABLE,
        HDCPTXXX_CONFIG_RETRY_READ_RXSTATUS_TIMES_DEFAULT,
        HDCPTXXX_CONFIG_RETRY_READ_RXSTATUS_INTERVAL_DEFAULT
    };

    if (p_retry == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPTX_CTRL_RETRY_GET) {
        p_retry->times = hdmitx_hdcp_config_retry_read_rxstatus.times;
        p_retry->interval = hdmitx_hdcp_config_retry_read_rxstatus.interval;
    } else if (ctrl == HDCPTX_CTRL_RETRY_SET) {
        if (p_retry->update == SYSTEM_UPDATE_ENABLE) {
            hdmitx_hdcp_config_retry_read_rxstatus.update = SYSTEM_UPDATE_ENABLE;
            hdmitx_hdcp_config_retry_read_rxstatus.times = p_retry->times;
            hdmitx_hdcp_config_retry_read_rxstatus.interval = p_retry->interval;
        }
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_cnt_seq_num_v(uint32_t *p_snv, hdcptx_ctrl_cnt ctrl)
{
    static uint32_t hdmitx_hdcp_config_seq_num_v = 0;

    if (p_snv == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPTX_CTRL_CNT_GET) {
        *p_snv = hdmitx_hdcp_config_seq_num_v;
    } else if (ctrl == HDCPTX_CTRL_CNT_SET) {
        hdmitx_hdcp_config_seq_num_v = *p_snv;
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_cnt_stream_manage(uint32_t *p_csm, hdcptx_ctrl_cnt ctrl)
{
    static uint32_t hdmitx_hdcp_config_content_stream_management = 0;

    if (p_csm == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPTX_CTRL_CNT_GET) {
        *p_csm = hdmitx_hdcp_config_content_stream_management;
    } else if (ctrl == HDCPTX_CTRL_CNT_SET) {
        hdmitx_hdcp_config_content_stream_management = *p_csm;
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_retry_stream_manage(system_config_retry *p_retry, hdcptx_ctrl_retry ctrl)
{
    static system_config_retry hdmitx_hdcp_config_retry_stream_manage = {
        SYSTEM_UPDATE_DISABLE,
        HDCPTXXX_CONFIG_RETRY_STREAM_MANAGE_TIMES_DEFAULT,
        HDCPTXXX_CONFIG_RETRY_STREAM_MANAGE_INTERVAL_DEFAULT
    };

    if (p_retry == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPTX_CTRL_RETRY_GET) {
        p_retry->times = hdmitx_hdcp_config_retry_stream_manage.times;
        p_retry->interval = hdmitx_hdcp_config_retry_stream_manage.interval;
    } else if (ctrl == HDCPTX_CTRL_RETRY_SET) {
        if (p_retry->update == SYSTEM_UPDATE_ENABLE) {
            hdmitx_hdcp_config_retry_stream_manage.update = SYSTEM_UPDATE_ENABLE;
            hdmitx_hdcp_config_retry_stream_manage.times = p_retry->times;
            hdmitx_hdcp_config_retry_stream_manage.interval = p_retry->interval;
        }
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_cnt_seq_num_m(uint32_t *p_snm, hdcptx_ctrl_cnt ctrl)
{   
    static uint32_t hdmitx_hdcp_config_seq_num_m = 0;

    if (p_snm == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPTX_CTRL_CNT_GET) {
        *p_snm = hdmitx_hdcp_config_seq_num_m;
    } else if (ctrl == HDCPTX_CTRL_CNT_SET) {
        hdmitx_hdcp_config_seq_num_m = *p_snm;
    } else if (ctrl == HDCPTX_CTRL_CNT_INC) {
        hdmitx_hdcp_config_seq_num_m++;
        // roll-over
        if(hdmitx_hdcp_config_seq_num_m > HDCPTXXX_CTRL_SEQ_NUM_M_MASK) {
            hdmitx_hdcp_config_seq_num_m = 0;
        }
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_req_abort(system_request *p_req, hdcpxx_ctrl_req ctrl)
{
    static system_request hdmitx_hdcp_abort_req = SYSTEM_REQUEST_NONE;

    if (p_req == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPXX_CTRL_REQ_GET) {
        *p_req =hdmitx_hdcp_abort_req ;
    } else if (ctrl == HDCPXX_CTRL_REQ_SET) {
        hdmitx_hdcp_abort_req = *p_req;
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static system_request hdcptxxx_ctrl_get_abort_req(void)
{
    system_request req;

    if (hdcptxxx_ctrl_req_abort(&req, HDCPXX_CTRL_REQ_GET)) {
        return SYSTEM_REQUEST_NONE;
    }

    return req;
}
//------------------------------------------------------------------------------
static void hdcptxxx_ctrl_set_abort_req(system_request req)
{
    if (hdcptxxx_ctrl_req_abort(&req, HDCPXX_CTRL_REQ_SET)) {
        return;
    }
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_ctrl_req_hpd_nocheck(system_request *p_req, hdcpxx_ctrl_req ctrl)
{
    static system_request hdmitx_hdcp_hpd_nocheck_req = SYSTEM_REQUEST_NONE;

    if (p_req == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPXX_CTRL_REQ_GET) {
        *p_req = hdmitx_hdcp_hpd_nocheck_req;
    } else if (ctrl == HDCPXX_CTRL_REQ_SET) {
        hdmitx_hdcp_hpd_nocheck_req = *p_req;
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcptxxx_check_hpd(system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    system_request req;
    uint8_t intr;
    uint32_t ret;

    ret = hdcptxxx_ctrl_req_hpd_nocheck(&req, HDCPXX_CTRL_REQ_GET);
    CHECK_ERROR(ret);
    if (req == SYSTEM_REQUEST_NOCHECK) {
        return ERR_OK;
    }
    ret = hdmitx_get_hpd_intr(port, &intr);
    CHECK_ERROR(ret);
    intr = intr & (HDCPTXXX_INTREG10_HPDDETN0 | HDCPTXXX_INTREG10_RXSENN0);
    if (intr) {
        ret = hdmitx_set_hpd_intr(port, intr);
        CHECK_ERROR(ret);
    }
    if (intr & HDCPTXXX_INTREG10_HPDDETN0) {
        return ERR_HDMITX_HPD_INACTIVE;
    }
    if (intr & HDCPTXXX_INTREG10_RXSENN0) {
        return ERR_HDMITX_RXSENSE_INACTIVE;
    }

    return ret;
#else  //
    return ERR_OK;
#endif // Product Number having HDMI Tx
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_init(system_port port)
{
    uint32_t ret;

    // init Stored_Receiver_ID
    ret = hdcptx22_init_stored_receiver_id(port);
    CHECK_ERROR(ret);
    // init Tx abort request
    hdcptxxx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_configure(hdmitx_hdcp_config *p_conf)
{
    uint32_t ret;

    if (p_conf == NULL) {
        return ERR_NULL_POINTER;
    }

    // set retry info for initial authentication
    ret = hdcptxxx_ctrl_retry_init_auth(&(p_conf->init_auth), HDCPTX_CTRL_RETRY_SET);
    CHECK_ERROR(ret);
    // set retry info for reading RxStatus
    ret = hdcptxxx_ctrl_retry_read_rxstatus(&(p_conf->read_rxstatus), HDCPTX_CTRL_RETRY_SET);
    CHECK_ERROR(ret);
    // set retry info for stream management
    ret = hdcptxxx_ctrl_retry_stream_manage(&(p_conf->stream_manage), HDCPTX_CTRL_RETRY_SET);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_authentication(hdmitx_hdcp_auth *p_auth)
{
    uint8_t ver;
    uint32_t ret;
    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // check hdmitx_init()
    MES("Func: %s, Line: %d check hdmitx_init()", __func__, __LINE__);
    ret = system_ctrl_check_hdmitx_init(p_auth->port);
    CHECK_ERROR(ret);
    p_auth->hdcp_version = HDMITX_HDCP_VERSION_NON_COMPLIANT;

    // check HPD of HDMI Tx
    MES("Func: %s, Line: %d check HPD of HDMI Tx", __func__, __LINE__);
    ret = hdcptxxx_check_hpd(p_auth->port);

    CHECK_ERROR(ret);

    // Set random seed
    MES("Func: %s, Line: %d Set random seed", __func__, __LINE__);
    ret = hdcptx22_set_random_seed(p_auth->port);
    CHECK_ERROR(ret);

    // Decrypt device key
    MES("Func: %s, Line: %d Decrypt device key", __func__, __LINE__);
    ret = hdcptxxx_decrypt_dev_key(p_auth->port);
    CHECK_ERROR(ret);

    // Reset authentication process
    MES("Func: %s, Line: %d Reset authentication process", __func__, __LINE__);
    ret = hdcptxxx_akesrst(p_auth->port, HDCPTXXX_AKESRST_RESET);
    CHECK_ERROR(ret);

    // Reset HDCP2.2 authentication process
    MES("Func: %s, Line: %d Reset HDCP2.2 authentication process", __func__, __LINE__);
    ret = hdcptx22_ake2rs(p_auth->port, HDCPTX22_AKE2RS_RESET);
    CHECK_ERROR(ret);
    CHECK_TX_ABORT();

    // Wait 100ms (tentative)
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);

    // Read HDCP2Version
    MES("Func: %s, Line: %d Read HDCP2Version", __func__, __LINE__);
    ret = hdcptx22_read_hdcp2version(p_auth->port, &ver);

    CHECK_ERROR(ret);

    // check supported HDCP22 bit and HDCP 1.4 or 2.2 Authentication
    if ((ver & HDCPTX22_HDCP2VERSION_SUPPORTED_HDCP22_BIT)
            == HDCPTX22_HDCP2VERSION_SUPPORTED_HDCP22_BIT) {
        // HDCP 2.2 Authentication
        MES("Func: %s, Line: %d HDCP 2.2 Authentication st", __func__, __LINE__);
        current_hdcp_version = 2;
        ret = hdcptx22_authentication(p_auth);
        MES("Func: %s, Line: %d HDCP 2.2 Authentication ed", __func__, __LINE__);
        CHECK_ERROR(ret);

        p_auth->hdcp_version = HDMITX_HDCP_VERSION_2_2;
    } else {
        // HDCP 1.4 Authentication
        MES("Func: %s, Line: %d HDCP 1.4 Authentication st", __func__, __LINE__);
        current_hdcp_version = 1;
        ret = hdcptx14_authentication(p_auth);
        MES("Func: %s, Line: %d HDCP 1.4 Authentication ed", __func__, __LINE__);
        CHECK_ERROR(ret);

        p_auth->hdcp_version = HDMITX_HDCP_VERSION_1_4;
    }

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_repeater_authentication(hdmitx_hdcp_repeater_auth *p_repeater_auth)
{
    uint32_t ret;

    if (p_repeater_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // check HPD of HDMI Tx
    MES("%s, %d, check HPD of HDMI Tx", __func__, __LINE__);

    ret = hdcptxxx_check_hpd(p_repeater_auth->port);

    CHECK_ERROR(ret);

    if (p_repeater_auth->hdcp_version == HDMITX_HDCP_VERSION_2_2) {
        MES("%s, %d, p_repeater_auth->hdcp_version == HDMITX_HDCP_VERSION_2_2", __func__, __LINE__);
        // HDCP 2.2 Authentication
        ret = hdcptx22_repeater_authentication(p_repeater_auth);
        MES("%s, %d, hdcptx22_repeater_authentication end end end ret = 0x%x", __func__, __LINE__, ret);
        CHECK_ERROR(ret);
        CHECK_TX_ABORT();
        // Wait 100ms (tentative)
        SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);
        // check RxStatus again
        MES("%s, %d, check RxStatus again", __func__, __LINE__);
        ret = hdcptx22_check_rxstatus(p_repeater_auth->port);
        MES("%s, %d, check RxStatus again end", __func__, __LINE__);
        CHECK_ERROR(ret);
    } else if (p_repeater_auth->hdcp_version == HDMITX_HDCP_VERSION_1_4) {
        // HDCP 1.4 Authentication
        MES("%s, %d, p_repeater_auth->hdcp_version == HDMITX_HDCP_VERSION_1_4", __func__, __LINE__);
        ret = hdcptx14_repeater_authentication(p_repeater_auth);
        MES("%s, %d, hdcptx14_repeater_authentication end", __func__, __LINE__);

        CHECK_ERROR(ret);
    } else {
        ret = ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_get_authentication_status(hdmitx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }
    CHECK_TX_ABORT();
    MES("hdmitx_hdcp_get_authentication_status()");
    if (p_auth->hdcp_version == HDMITX_HDCP_VERSION_2_2) {
        // HDCP 2.2 Authentication
        MES("%s, HDMITX_HDCP_VERSION_2_2", __func__);
        ret = hdcptx22_get_authentication_status(p_auth);
        CHECK_ERROR(ret);
    } else if (p_auth->hdcp_version == HDMITX_HDCP_VERSION_1_4) {
        // HDCP 1.4 Authentication
        MES("%s, HDMITX_HDCP_VERSION_1_4", __func__);
        ret = hdcptx14_get_authentication_status(p_auth);
        CHECK_ERROR(ret);
    } else {
        ret = ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_authentication_reset(hdmitx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // ENC_DIS
    if (p_auth->hdcp_version == HDMITX_HDCP_VERSION_2_2) {
        // HDCP 2.2 Authentication
        MES("%s, %d, HDMITX_HDCP_VERSION_2_2", __func__, __LINE__);
        ret = hdcptxxx_output_eess(p_auth->port, HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_DIS);
        CHECK_ERROR(ret);
    } else if (p_auth->hdcp_version == HDMITX_HDCP_VERSION_1_4) {
        // HDCP 1.4 Authentication
        MES("%s, %d, HDMITX_HDCP_VERSION_1_4", __func__, __LINE__);
        ret = hdcptxxx_output_eess(p_auth->port, HDCPTXXX_OUTPUT_EESS_HDCP14_ENC_DIS);
        CHECK_ERROR(ret);
    } else {
        // HDCP 1.4 Authentication
        MES("%s, %d, else", __func__, __LINE__);
        ret = hdcptxxx_output_eess(p_auth->port, HDCPTXXX_OUTPUT_EESS_HDCP14_ENC_DIS);
        CHECK_ERROR(ret);
        CHECK_TX_ABORT();
        // Wait 100ms (tentative)
        SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);
        // HDCP 2.2 Authentication
        ret = hdcptxxx_output_eess(p_auth->port, HDCPTXXX_OUTPUT_EESS_HDCP22_ENC_DIS);
        CHECK_ERROR(ret);
    }
    // Reset authentication process
    MES("%s, %d, Reset authentication process", __func__, __LINE__);
    ret = hdcptxxx_akesrst(p_auth->port, HDCPTXXX_AKESRST_RESET);
    CHECK_ERROR(ret);
    // Reset HDCP2.2 authentication process
    MES("%s, %d, HDCP2.2 authentication process", __func__, __LINE__);
    ret = hdcptx22_ake2rs(p_auth->port, HDCPTX22_AKE2RS_RESET);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmitx_hdcp_abort(void)
{
    // abort request
    hdcptxxx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_akecmd_nocheck(system_port port, uint8_t no, uint8_t akecmd)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_AKECMD_NC_NUM];
    uint32_t ret;

    CHECK_RX_ABORT();
    SET_ACCESS(access, &port, cmd, HDCPRX14_AKECMD_NC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_AKECMD0_ADDR + (no * HDCPRXXX_PORT_OFFSET),
                        akecmd,
                        HDCPRX14_AKECMD_NC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_akecmd(system_port port, uint8_t no, uint8_t akecmd, akecmdrx_ctrl *p_actrl)
{
    uint32_t ret;

    if (p_actrl == NULL) {
        return ERR_NULL_POINTER;
    }

    ret = hdcprx14_akecmd_nocheck(port, no, akecmd);
    CHECK_ERROR(ret);
    // check error
    ret = hdcprx14_akeerr(port, no, p_actrl);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_akeerr(system_port port, uint8_t no, akecmdrx_ctrl *p_actrl)
{
    uint8_t akesta, akeerr;
    uint32_t ret;
    uint32_t i;

    if (p_actrl == NULL) {
        return ERR_NULL_POINTER;
    }

    akesta = 0;
    akeerr = 0;
    for (i=0; i<p_actrl->ack_retry_max; i++) {
        CHECK_RX_ABORT();
        ret = hdcprx14_read_akesta(port, no, &akesta);
        CHECK_ERROR(ret);
        if ((akesta & HDCPRX14_AKESTA_REG_MASK) == HDCPRX14_AKESTA_NORMAL_END) {
            ret = ERR_OK;
            break;
        } else if ((akesta & HDCPRX14_AKESTA_REG_MASK) == HDCPRX14_AKESTA_ERROR_END) {
            ret = hdcprx14_read_akeerr(port, no, &akeerr);
            CHECK_ERROR(ret);
            ret = ERR_HDCPRX14_AKECMD_NORMAL_END | akeerr;
            return ret;
        } else {
            // retry
        }
        SLEEP_Delay_ms(p_actrl->ack_retry_interval);
    }
    if (i == p_actrl->ack_retry_max) {
        ret = ERR_HDCPRX14_AKESTA_READ_RETRY;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_read_akesta(system_port port, uint8_t no, uint8_t *p_sta)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_AKESTA_NUM];
    uint32_t ret;

    if (p_sta == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPRX14_AKESTA_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_AKESTA0_ADDR + (no * HDCPRXXX_PORT_OFFSET),
                        0,
                        HDCPRX14_AKESTA_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_sta = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_read_akeerr(system_port port, uint8_t no, uint8_t *p_err)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_AKEERR_NUM];
    uint32_t ret;

    if (p_err == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPRX14_AKEERR_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_AKEERR0_00_ADDR + (no * HDCPRXXX_PORT_OFFSET),
                        0,
                        HDCPRX14_AKEERR_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_err = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_akecmd_nocheck(system_port port, uint8_t akecmd)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_AKECMD_NC_NUM];
    uint32_t ret;

    CHECK_RX_ABORT();
    SET_ACCESS(access, &port, cmd, HDCPRX22_AKECMD_NC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_H22CMD_ADDR, akecmd, HDCPRX22_AKECMD_NC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_akecmd(system_port port, uint8_t akecmd, akecmdrx_ctrl *p_actrl)
{
    uint32_t ret;

    if (p_actrl == NULL) {
        return ERR_NULL_POINTER;
    }

    ret = hdcprx22_akecmd_nocheck(port, akecmd);
    CHECK_ERROR(ret);
    // check error
    ret = hdcprx22_akeerr(port, p_actrl);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_akeerr(system_port port, akecmdrx_ctrl *p_actrl)
{
    uint8_t akesta, akeerr;
    uint32_t ret;
    uint32_t i;

    if (p_actrl == NULL) {
        return ERR_NULL_POINTER;
    }

    akesta = 0;
    akeerr = 0;
    for (i=0; i<p_actrl->ack_retry_max; i++) {
        CHECK_RX_ABORT();
        ret = hdcprx22_read_akesta(port, &akesta);
        CHECK_ERROR(ret);
        if ((akesta & HDCPRX22_AKESTA_REG_MASK) == HDCPRX22_AKESTA_NORMAL_END) {
            ret = ERR_OK;
            break;
        } else if ((akesta & HDCPRX22_AKESTA_REG_MASK) == HDCPRX22_AKESTA_ERROR_END) {
            ret = hdcprx22_read_akeerr(port, &akeerr);
            CHECK_ERROR(ret);
            ret = ERR_HDCPRX22_AKECMD_NORMAL_END | akeerr;
            return ret;
        } else {
            // retry
        }
        SLEEP_Delay_ms(p_actrl->ack_retry_interval);
    }
    if (i == p_actrl->ack_retry_max) {
        ret = ERR_HDCPRX22_AKESTA_READ_RETRY;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_read_akesta(system_port port, uint8_t *p_sta)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_AKESTA_NUM];
    uint32_t ret;

    if (p_sta == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPRX22_AKESTA_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKE2ST_ADDR, 0, HDCPRX22_AKESTA_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_sta = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_read_akeerr(system_port port, uint8_t *p_err)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_AKEERR_NUM];
    uint32_t ret;

    if (p_err == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPRX22_AKEERR_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKE2ER_ADDR, 0, HDCPRX22_AKEERR_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_err = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static uint32_t hdcprx22_set_ddc_subcmd(system_port port, uint8_t subcmd)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_SET_DDC_SUBCMD_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRX22_SET_DDC_SUBCMD_NUM);
    // subcmd
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_H22CMC_ADDR, subcmd, HDCPRX22_SET_DDC_SUBCMD_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
#endif // Product Number having HDMI Rx & HDMI Tx
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_select_page_of_0x14(system_port port, uint8_t page)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_PAGE_SELECT_0X14_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRXXX_PAGE_SELECT_0X14_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_AVMMASK_ADDR, page, HDCPRXXX_PAGE_SELECT_0X14_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_hdcp2version_bit(system_port port, uint8_t bit)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_SET_HDCP2VERSION_BIT_NUM];
    uint32_t ret;

    // page 0x14: 0,1,2,3

    SET_ACCESS(access, &port, cmd, HDCPRX22_SET_HDCP2VERSION_BIT_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCPRX_HD2VERP0_ADDR, bit, HDCPRX22_SET_HDCP2VERSION_BIT_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_hdcp2ver_of_portno(system_port port, uint8_t no, hdmirx_hdcp_version ver)
{
    uint8_t bit;
    uint32_t ret;

    // Select HDCP page: HDCP Port#
    ret = hdcprxxx_select_page_of_0x14(port, no);
    CHECK_ERROR(ret);

    // page 0x14: 0,1,2,3

    // Set HDCP2Version
    bit = (ver == HDMIRX_HDCP_VERSION_2_2 || ver == HDMIRX_HDCP_VERSION_AUTO)
            ? HDCPRX22_SET_HDCP2VERSION_BIT_ENABLE
            : HDCPRX22_SET_HDCP2VERSION_BIT_DISABLE;
    ret = hdcprx22_set_hdcp2version_bit(port, bit);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_hdcp2ver(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // HDCP Port0
    ret = hdcprx22_set_hdcp2ver_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_0, p_auth->port0);
    CHECK_ERROR(ret);
    // HDCP Port1
    ret = hdcprx22_set_hdcp2ver_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_1, p_auth->port1);
    CHECK_ERROR(ret);
    // HDCP Port2
    ret = hdcprx22_set_hdcp2ver_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_2, p_auth->port2);
    CHECK_ERROR(ret);
    // HDCP Port3
    ret = hdcprx22_set_hdcp2ver_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_3, p_auth->port3);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_bcaps(system_port port, uint8_t bcaps, uint8_t mask)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_SET_BCAPS_NUM];
    uint32_t ret;

    // page 0x14: 0,1,2,3

    SET_ACCESS(access, &port, cmd, HDCPRX14_SET_BCAPS_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCPRX_BCAPRP0_ADDR, bcaps, mask);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_bcaps_of_portno(system_port port, uint8_t no, uint8_t bcaps, uint8_t mask)
{
    uint32_t ret;

    // Select HDCP page: HDCP Port#
    ret = hdcprxxx_select_page_of_0x14(port, no);
    CHECK_ERROR(ret);

    // page 0x14: 0,1,2,3

    ret = hdcprx14_set_bcaps(port, bcaps, mask);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_bcaps_repeater(hdmirx_hdcp_auth *p_auth)
{
    uint8_t bcaps, mask;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // set Bcaps.REPEATER
    bcaps = (p_auth->repeater == HDMIRX_HDCP_REPEATER)
                ? HDCPRX14_SET_BCAPS_REPEATER_ENA
                : HDCPRX14_SET_BCAPS_REPEATER_DIS;
    mask = HDCPRX14_SET_BCAPS_REPEATER_ENA;

    // HDCP Port0
    ret = hdcprx14_set_bcaps_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_0, bcaps, mask);
    CHECK_ERROR(ret);
    // HDCP Port1
    ret = hdcprx14_set_bcaps_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_1, bcaps, mask);
    CHECK_ERROR(ret);
    // HDCP Port2
    ret = hdcprx14_set_bcaps_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_2, bcaps, mask);
    CHECK_ERROR(ret);
    // HDCP Port3
    ret = hdcprx14_set_bcaps_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_3, bcaps, mask);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_bcaps_reset(system_port port, uint8_t no)
{
    uint8_t bcaps, mask;
    uint32_t ret;

    // clear Bcaps.REPEATER, Bcaps.READY
    bcaps = HDCPRX14_SET_BCAPS_REPEATER_DIS | HDCPRX14_SET_BCAPS_READY_DIS;
    mask = HDCPRX14_SET_BCAPS_REPEATER_ENA | HDCPRX14_SET_BCAPS_READY_ENA;

    ret = hdcprx14_set_bcaps_of_portno(port, no, bcaps, mask);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static uint32_t hdcprx14_set_bcaps_ready(system_port port, uint8_t no)
{
    uint8_t bcaps, mask;
    uint32_t ret;

    // set Bcaps.READY
    bcaps = HDCPRX14_SET_BCAPS_READY_ENA;
    mask = HDCPRX14_SET_BCAPS_READY_ENA;

    ret = hdcprx14_set_bcaps_of_portno(port, no, bcaps, mask);
    CHECK_ERROR(ret);

    return ret;
}
#endif // Product Number having HDMI Rx & HDMI Tx
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_random_seed(system_port port)
{
    uint8_t seed[HDCPTX22_RANDOM_SEED_BYTE_SIZE];
    system_request req;
    uint32_t ret, ret2;

    // Select DDC buffer page
    ret = hdcptxxx_select_page_of_0x7c(port, HDCPTXXX_PAGE_NO_3);
    CHECK_ERROR(ret);
    // Read random seed
    ret = hdcptx22_get_random_seed_from_register(port, seed);
    CHECK_ERROR(ret);
    // check already generated
    if (seed[0]==0 && seed[4]==0 && seed[8]==0 && seed[12]==0) {
        req = SYSTEM_REQUEST_NOCHECK;
        ret2 = hdcptxxx_ctrl_req_hpd_nocheck(&req, HDCPXX_CTRL_REQ_SET);
        CHECK_ERROR(ret2);
        // Generate random seed
        ret =  hdcptx22_generate_random_seed(port);
        req = SYSTEM_REQUEST_NONE;
        ret2 = hdcptxxx_ctrl_req_hpd_nocheck(&req, HDCPXX_CTRL_REQ_SET);
        CHECK_ERROR(ret2);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxcaps(system_port port, uint8_t rxcaps0, uint8_t rxcaps1, uint8_t rxcaps2)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_SET_RXCAPS_LEN];
    uint32_t ret;

    // page 0x14: 12

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_RXCAPS_00_ADDR,
                        HDCPRX22_SET_RXCAPS_LEN, data);
    // RxCaps
    SET_BLOCK_DATA(data, 0, rxcaps2);                // 0x1454
    SET_BLOCK_DATA(data, 1, rxcaps1);                // 0x1455
    SET_BLOCK_DATA(data, 2, rxcaps0);                // 0x1456

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxcaps_repeater(hdmirx_hdcp_auth *p_auth)
{
    uint8_t rxcaps0;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    rxcaps0 = (p_auth->repeater == HDMIRX_HDCP_REPEATER)
                ? HDCPRX22_SET_RXCAPS_REPEATER_ENA
                : HDCPRX22_SET_RXCAPS_REPEATER_DIS;

    // Select HDCP page: HDCP2.2 authentication register
    ret = hdcprxxx_select_page_of_0x14(p_auth->port, HDCPRXXX_PAGE_HDCP22_AUTH_REG);
    CHECK_ERROR(ret);
    // Set RxCaps.REPEATER
    ret = hdcprx22_set_rxcaps(p_auth->port, rxcaps0,
                                    HDCPRX22_SET_RXCAPS_15_08_DEF,
                                    HDCPRX22_SET_RXCAPS_23_16_DEF);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_decrypt_device_key(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    if (p_auth->port0 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port1 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port2 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port3 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx22_akecmd(p_auth->port, HDCPRX22_DECRYPT_DEVICE_KEY_CMD, &akerxctrl_process);
        CHECK_ERROR(ret);
    } else {
        ret = ERR_OK;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_decrypt_device_key(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    ret = ERR_OK;

    // HDCP Port0
    if (p_auth->port0 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd(p_auth->port, SYSTEM_PORT_NUMBER_0,
                                HDCPRX14_LOAD_ENCRYPTED_DEVICE_KEY_EXEC_1248_CMD,
                                &akerxctrl_process);
        CHECK_ERROR(ret);
    }
    // HDCP Port1
    if (p_auth->port1 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd(p_auth->port, SYSTEM_PORT_NUMBER_1,
                                HDCPRX14_LOAD_ENCRYPTED_DEVICE_KEY_EXEC_1248_CMD,
                                &akerxctrl_process);
        CHECK_ERROR(ret);
    }
    // HDCP Port2
    if (p_auth->port2 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd(p_auth->port, SYSTEM_PORT_NUMBER_2,
                                HDCPRX14_LOAD_ENCRYPTED_DEVICE_KEY_EXEC_1248_CMD,
                                &akerxctrl_process);
        CHECK_ERROR(ret);
    }
    // HDCP Port3
    if (p_auth->port3 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd(p_auth->port, SYSTEM_PORT_NUMBER_3,
                                HDCPRX14_LOAD_ENCRYPTED_DEVICE_KEY_EXEC_1248_CMD,
                                &akerxctrl_process);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_set_hdcp_func_of_portno(system_port port, uint8_t no, uint8_t func)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_SET_HDCP_FUNC_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRXXX_SET_HDCP_FUNC_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_HDCPCTL10_ADDR + (no * HDCPRXXX_PORT_OFFSET),
                        func,
                        HDCPRXXX_SET_HDCP_FUNC_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_set_hdcp_func(hdmirx_hdcp_auth *p_auth)
{
    uint8_t func;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // HDCP Port0
    func = (p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO)
            ? (HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE)
            : (p_auth->port0 == HDMIRX_HDCP_VERSION_1_4)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE
            : (p_auth->port0 == HDMIRX_HDCP_VERSION_2_2)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE
            : ( HDCPRXXX_SET_HDCP_FUNC_HDCP14_DISABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_DISABLE);
    ret = hdcprxxx_set_hdcp_func_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_0, func);
    CHECK_ERROR(ret);
    // HDCP Port1
    func = (p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO)
            ? (HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE)
            : (p_auth->port1 == HDMIRX_HDCP_VERSION_1_4)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE
            : (p_auth->port1 == HDMIRX_HDCP_VERSION_2_2)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE
            : ( HDCPRXXX_SET_HDCP_FUNC_HDCP14_DISABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_DISABLE);
    ret = hdcprxxx_set_hdcp_func_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_1, func);
    CHECK_ERROR(ret);
    // HDCP Port2
    func = (p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO)
            ? (HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE)
            : (p_auth->port2 == HDMIRX_HDCP_VERSION_1_4)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE
            : (p_auth->port2 == HDMIRX_HDCP_VERSION_2_2)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE
            : ( HDCPRXXX_SET_HDCP_FUNC_HDCP14_DISABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_DISABLE);
    ret = hdcprxxx_set_hdcp_func_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_2, func);
    CHECK_ERROR(ret);
    // HDCP Port3
    func = (p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO)
            ? (HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE)
            : (p_auth->port3 == HDMIRX_HDCP_VERSION_1_4)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP14_ENABLE
            : (p_auth->port3 == HDMIRX_HDCP_VERSION_2_2)
            ? HDCPRXXX_SET_HDCP_FUNC_HDCP22_ENABLE
            : ( HDCPRXXX_SET_HDCP_FUNC_HDCP14_DISABLE | HDCPRXXX_SET_HDCP_FUNC_HDCP22_DISABLE);
    ret = hdcprxxx_set_hdcp_func_of_portno(p_auth->port, SYSTEM_PORT_NUMBER_3, func);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_start_hdcp_auth(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // when repeater, the following must not be done.
    if (p_auth->repeater == HDMIRX_HDCP_REPEATER) {
        return ERR_OK;
    }

    ret = ERR_OK;

    // HDCP Port0
    if (p_auth->port0 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd_nocheck(p_auth->port, SYSTEM_PORT_NUMBER_0, HDCPRX14_START_AKE2_CMD);
        CHECK_ERROR(ret);
    }
    // HDCP Port1
    if (p_auth->port1 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd_nocheck(p_auth->port, SYSTEM_PORT_NUMBER_1, HDCPRX14_START_AKE2_CMD);
        CHECK_ERROR(ret);
    }
    // HDCP Port2
    if (p_auth->port2 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd_nocheck(p_auth->port, SYSTEM_PORT_NUMBER_2, HDCPRX14_START_AKE2_CMD);
        CHECK_ERROR(ret);
    }
    // HDCP Port3
    if (p_auth->port3 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_akecmd_nocheck(p_auth->port, SYSTEM_PORT_NUMBER_3, HDCPRX14_START_AKE2_CMD);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_start_hdcp_auth(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // when repeater, the following must not be done.
    if (p_auth->repeater == HDMIRX_HDCP_REPEATER) {
        return ERR_OK;
    }

    if (p_auth->port0 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port1 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port2 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port3 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx22_akecmd_nocheck(p_auth->port, HDCPRX22_PICK_CMD_UP_CMD);
        CHECK_ERROR(ret);
    } else {
        ret = ERR_OK;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_akcrst(system_port port, uint8_t no)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_AKCRST_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRX14_AKCRST_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM_RXSRST2_ADDR + (no * HDCPRX14_AKCRST_PORT_OFFSET),
                        HDCPRX14_AKCRST_AKE_CANCEL,
                        HDCPRX14_AKCRST_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ake2rs(system_port port, uint8_t rst)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_AKE2RS_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRX22_AKE2RS_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCP_AKE2RS_ADDR, rst, HDCPRX22_AKE2RS_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxstatus_msb(system_port port, uint8_t no, uint8_t rxstatus_15_8, uint8_t mask)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_SET_RXSTATUS_MSB_NUM];
    uint32_t ret;

    // Select HDCP page: HDCP Port#
    ret = hdcprxxx_select_page_of_0x14(port, no);
    CHECK_ERROR(ret);

    // page 0x14: 0,1,2,3

    SET_ACCESS(access, &port, cmd, HDCPRX22_SET_RXSTATUS_MSB_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCPRX_RXSTATP0_01_ADDR,
                        rxstatus_15_8,
                        mask);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_auth_reset_port(system_port port, uint8_t no, hdmirx_hdcp_version ver)
{
    uint32_t ret;

    if (ver == HDMIRX_HDCP_VERSION_1_4) {
        // Reset HDCP1.4 authentication process by resetting AKE command: Port#
        ret = hdcprx14_akcrst(port, no);
        CHECK_ERROR(ret);
        // Reset HDCP1.4 Bcaps
        ret = hdcprx14_set_bcaps_reset(port, no);
        CHECK_ERROR(ret);
    } else if (ver == HDMIRX_HDCP_VERSION_2_2) {
        // Reset HDCP2.2 authentication process
        // NOTE: HDCP2.2 authentication of all ports is reset.
        ret = hdcprx22_ake2rs(port, HDCPRX22_AKE2RS_RESET);
        CHECK_ERROR(ret);
        // Set REAUTH_REQ=1, READY=0 at HDCP2.2
        ret = hdcprx22_set_rxstatus_msb(port, no, HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_1
                                                    | HDCPRX22_SET_RXSTATUS_MSB_READY_0,
                                                HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_MASK
                                                    | HDCPRX22_SET_RXSTATUS_MSB_READY_MASK);
        CHECK_ERROR(ret);
    } else if (ver == HDMIRX_HDCP_VERSION_AUTO) {
        // Reset HDCP1.4 authentication process by resetting AKE command: Port#
        ret = hdcprx14_akcrst(port, no);
        CHECK_ERROR(ret);
        // Reset HDCP1.4 Bcaps
        ret = hdcprx14_set_bcaps_reset(port, no);
        CHECK_ERROR(ret);
        // Reset HDCP2.2 authentication process
        // NOTE: HDCP2.2 authentication of all ports is reset.
        ret = hdcprx22_ake2rs(port, HDCPRX22_AKE2RS_RESET);
        CHECK_ERROR(ret);
        // Set REAUTH_REQ=1, READY=0 at HDCP2.2
        ret = hdcprx22_set_rxstatus_msb(port, no, HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_1
                                                    | HDCPRX22_SET_RXSTATUS_MSB_READY_0,
                                                HDCPRX22_SET_RXSTATUS_MSB_REAUTH_REQ_MASK
                                                    | HDCPRX22_SET_RXSTATUS_MSB_READY_MASK);
        CHECK_ERROR(ret);
    } else {
        return ERR_INVALID_INDEX;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_check_bch_error(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_CHECK_BCH_ERROR_NUM];
    uint8_t rst;
    uint32_t ret, ret2;

    SET_ACCESS(access, &port, cmd, HDCPRXXX_CHECK_BCH_ERROR_NUM);
    // HDBERR: packet header BCH error
    // PBBERR: packet body BCH error
    SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG8_00_ADDR,
                        0,
                        HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_HDBERR_BIT |
                            HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_PBBERR_BIT);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);

    rst = 0;
    ret2 = ERR_OK;
    // HDBERR: packet header BCH error has occurred
    if (cmd[0].data & HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_HDBERR_BIT) {
        rst |= HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_HDBERR_BIT;
        ret2 = ERR_HDCPRXXX_PACKET_HEADER_BCH_ERROR;
    }
    // PBBERR: packet body BCH error has occurred
    if (cmd[0].data & HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_PBBERR_BIT) {
        rst |= HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_PBBERR_BIT;
        ret2 = ERR_HDCPRXXX_PACKET_BODY_BCH_ERROR;
    }
    // bit clear
    if (rst) {
        SET_COMMAND(cmd, 0, VIDEOIF_IRQ_RINTREG8_00_ADDR,
                            rst,
                            HDCPRXXX_CHECK_BCH_ERROR_RINTREG8_00_MASK);
        ret = lf9129_device_write(&access);
        CHECK_ERROR(ret);
    }

    return ret2;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_get_selected_rx_port(system_port port, uint8_t *p_port)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_GET_SELECTED_RX_PORT_NUM];
    uint32_t ret;

    if (p_port == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPRXXX_GET_SELECTED_RX_PORT_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_SYSTEM2_SWMODE1_ADDR,
                        0,
                        HDMIRXXX_GET_SELECTED_RX_PORT_SWMODE1_PORT0OUT_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);

    *p_port = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_check_eess(system_port port, uint8_t no, hdmirx_hdcp_eess *p_eess)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_CHECK_EESS_NUM];
    uint32_t ret;

    if (p_eess == NULL) {
        return ERR_NULL_POINTER;
    }

    SET_ACCESS(access, &port, cmd, HDCPRXXX_CHECK_EESS_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_HDCPMON0_ADDR + (no * HDCPRXXX_PORT_OFFSET),
                        0,
                        HDCPRXXX_CHECK_EESS_HDCPMONX_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);

    *p_eess = (cmd[0].data & HDCPRXXX_CHECK_EESS_HDCPMONX_EESSENX_MASK)
                ? HDMIRX_HDCP_EESS_ENC_EN
                : (cmd[0].data & HDCPRXXX_CHECK_EESS_HDCPMONX_EESSDSX_MASK)
                ? HDMIRX_HDCP_EESS_ENC_DIS
                : HDMIRX_HDCP_EESS_NONE;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirxxx_get_hdcp_decryption_status(system_port port, uint8_t no, uint8_t *p_stat)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_GET_HDCP_DECRYPTION_STATUS_NUM];
    uint32_t ret;

    if (p_stat == NULL) {
        return ERR_NULL_POINTER;
    }

    // page 0x14: 12

    SET_ACCESS(access, &port, cmd, HDCPRXXX_GET_HDCP_DECRYPTION_STATUS_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_HDCPRX_H22ST01_ADDR + ((no/2) * HDCPRXXX_H22STX_PORT_OFFSET),
                        0,
                        HDCPRXXX_H22STXX_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);
    *p_stat = (cmd[0].data >> ((no%2) * HDCPRXXX_H22STX_BIT)) & HDCPRXXX_H22STX_MASK;

    return ret;
}
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
static uint32_t hdcprx14_get_irq(system_port port, uint8_t no, uint8_t *p_msg)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_GET_IRQ_NUM];
    uint32_t ret;

    if (p_msg == NULL) {
        return ERR_NULL_POINTER;
    }

    // get Aksv value updated
    // 0x10F8 or 0x10F9 or 0x10FA or 0x10FB
    SET_ACCESS(access, &port, cmd, HDCPRX14_GET_IRQ_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG4_ADDR
                        + (no * HDCPRX14_GET_IRQ_INTREGX_AKREDYX_OFFSET),
                        0,
                        HDCPRX14_GET_IRQ_INTREGX_AKREDYX_MASK);
    ret = lf9129_device_read(&access);
    CHECK_ERROR(ret);

    *p_msg = cmd[0].data;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_check_irq(system_port port, uint8_t no, hdmirx_hdcp_message *p_msg)
{
    uint8_t msg;
    uint32_t ret;

    if (p_msg == NULL) {
        return ERR_NULL_POINTER;
    }

    // get Aksv value updated
    ret = hdcprx14_get_irq(port, no, &msg);
    CHECK_ERROR(ret);

    *p_msg = (msg) ? HDMIRX_HDCP_MESSAGE_AKSV : HDMIRX_HDCP_MESSAGE_NONE;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_clear_akredy(system_port port, uint8_t no)
{
    system_reg_access access;
    system_command cmd[HDCPRX14_CLEAR_AKREDY_NUM];
    uint32_t ret;

    // clear AKREDYx
    // 0x10F8 or 0x10F9 or 0x10FA or 0x10FB
    SET_ACCESS(access, &port, cmd, HDCPRX14_CLEAR_AKREDY_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG4_ADDR
                        + (no * HDCPRX14_GET_IRQ_INTREGX_AKREDYX_OFFSET),
                        HDCPRX14_CLEAR_AKREDY_INTREGX_AKREDYX_CLEAR,
                        HDCPRX14_CLEAR_AKREDY_INTREGX_MASK);    // no mask because write 1, cleared
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_1st_auth(system_port port, uint8_t no)
{
    uint32_t ret;

    ret = hdcprx14_akecmd(port, no, HDCPRX14_START_AKE1_CMD, &akerxctrl_ake1st);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_calc_vddhn(system_port port, uint8_t no)
{
    uint32_t ret;

    ret = hdcprx14_akecmd(port, no, HDCPRX14_CALC_SHA_1_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_clear_h22msg0(system_port port)
{
    system_reg_access access;
    system_command cmd[HDCPRX22_CLEAR_H22MSG0_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRX22_CLEAR_H22MSG0_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_IRQ_INTREG4_ADDR,
                        HDCPRX22_CLEAR_H22MSG0_INTREG4_H22MSG0,
                        HDCPRX22_CLEAR_H22MSG0_INTREG4_MASK);    // no mask because write 1, cleared
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_get_irq(system_port port, uint8_t *p_msg0, uint8_t *p_msg1)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_GET_IRQ_LEN] = { 0 };
    uint32_t ret;

    if (p_msg0 == NULL || p_msg1 == NULL) {
        return ERR_NULL_POINTER;
    }

    // H22MSG0 == 1 ?
    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_IRQ_INTREG4_ADDR,
                        HDCPRX22_CLEAR_H22MSG0_NUM, data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);
    if (!(data[0] & HDCPRX22_CLEAR_H22MSG0_INTREG4_H22MSG0_MASK)) {
        *p_msg0 = 0;
        *p_msg1 = 0;
        return ERR_OK;
    }

    // page 0x14: 12

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_MSGIRQ_00_ADDR,
                        HDCPRX22_GET_IRQ_LEN, data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    *p_msg0 = data[0];
    *p_msg1 = data[1];

    // clear bit if set
    if (data[0] || data[1] || data[2] || data[3]) {
        ret = device_block_write(&baccess);
        CHECK_ERROR(ret);
        // clear H22MSG0 after clearing 0x14A4 - 0x14A7
        ret = hdcprx22_clear_h22msg0(port);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_check_irq(system_port port, hdmirx_hdcp_message *p_msg)
{
    uint8_t msg0, msg1;
    uint32_t ret;

    // Select HDCP page: HDCP2.2 authentication register
    ret = hdcprxxx_select_page_of_0x14(port, HDCPRXXX_PAGE_HDCP22_AUTH_REG);
    CHECK_ERROR(ret);
    // page 0x14: 12
    // Get IRQ message
    ret = hdcprx22_get_irq(port, &msg0, &msg1);
    CHECK_ERROR(ret);

    if (msg0 & HDCPRX22_IRQ0_AKE_INIT) {
        *p_msg = HDMIRX_HDCP_MESSAGE_AKE_INIT;
    } else if (msg0 & HDCPRX22_IRQ0_AKE_NO_STORED_KM) {
        *p_msg = HDMIRX_HDCP_MESSAGE_AKE_NO_STORED_KM;
    } else if (msg0 & HDCPRX22_IRQ0_AKE_STORED_KM) {
        *p_msg = HDMIRX_HDCP_MESSAGE_AKE_STORED_KM;
    } else if (msg0 & HDCPRX22_IRQ0_LC_INIT) {
        *p_msg = HDMIRX_HDCP_MESSAGE_LC_INIT;
    } else if (msg1 & HDCPRX22_IRQ1_SKE_SEND_EKS) {
        *p_msg = HDMIRX_HDCP_MESSAGE_SKE_SEND_EKS;
    } else if (msg1 & HDCPRX22_IRQ1_REPEATERAUTH_SEND_ACK) {
        *p_msg = HDMIRX_HDCP_MESSAGE_REPEATERAUTH_SEND_ACK;
    } else if (msg1 & HDCPRX22_IRQ1_REPEATERAUTH_STREAM_MANAGE) {
        *p_msg = HDMIRX_HDCP_MESSAGE_REPEATERAUTH_STREAM_MANAGE;
    } else {
        *p_msg = HDMIRX_HDCP_MESSAGE_NONE;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_ake_init(system_port port)
{
    uint32_t ret;

    CHECK_RX_ABORT();
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_10MSEC);

    ret = hdcprx22_akecmd(port, HDCPRX22_STORE_RECEIVED_RTX_TXCAPS_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_ake_no_stored_km(system_port port)
{
    uint32_t ret;

    ret = hdcprx22_akecmd(port, HDCPRX22_DECRYPT_EKPUB_KM_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_ake_stored_km(system_port port)
{
    uint32_t ret;

    CHECK_RX_ABORT();
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);

    ret = hdcprx22_akecmd(port, HDCPRX22_DECRYPT_EKH_KM_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_lc_init(system_port port)
{
    uint32_t ret;

    ret = hdcprx22_akecmd(port, HDCPRX22_STORE_ACQUIRED_RN_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_ske_send_eks(system_port port)
{
    uint32_t ret;

    CHECK_RX_ABORT();
    SLEEP_Delay_ms(HDCP_SLEEP_TIME_100MSEC);

    ret = hdcprx22_akecmd(port, HDCPRX22_DECRYPT_EDKEY_KS_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_repeaterauth_send_ack(system_port port)
{
    uint32_t ret;

    // Set DDC sub-command to compare V and V'
    ret = hdcprx22_set_ddc_subcmd(port, HDCPRX22_COMPARE_V_VD_SUBCMD);
    CHECK_ERROR(ret);

    ret = hdcprx22_akecmd(port, HDCPRX22_COMPARE_V_VD_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_ack_repeaterauth_stream_manage(system_port port)
{
    uint32_t ret;

    ret = hdcprx22_akecmd(port, HDCPRX22_CALC_MD_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_calc_vd(system_port port)
{
    uint32_t ret;

    // receiver ID list, RxInfo, seq_num_V  have already been set

    // Select HDCP page: HDCP2.2 repeater infomation
    ret = hdcprxxx_select_page_of_0x14(port, HDCPRXXX_PAGE_HDCP22_REPEATER_INFO);
    CHECK_ERROR(ret);

    ret = hdcprx22_akecmd(port, HDCPRX22_CALC_VD_CMD, &akerxctrl_process);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_read_vd_calc_data(system_port port, uint8_t *p_vd_data)
{
    system_block_access baccess;
    uint8_t len;
    uint32_t ret;

    if (p_vd_data == NULL) {
        return ERR_NULL_POINTER;
    }

    // Select HDCP page: HDCP2.2 repeater infomation
    ret = hdcprxxx_select_page_of_0x14(port, HDCPRXXX_PAGE_HDCP22_REPEATER_INFO);
    CHECK_ERROR(ret);

    // page 0x14: 25

    // get length
    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_BKSVRP0_00_ADDR,
                        HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE, p_vd_data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);
    len = p_vd_data[0] + HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE;
    // get V' calc data
    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_BKSVRP0_00_ADDR,
                        len, p_vd_data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_read_vd(system_port port, uint8_t *p_vd)
{
    system_block_access baccess;
    uint32_t ret;

    if (p_vd == NULL) {
        return ERR_NULL_POINTER;
    }

    // Select HDCP page: HDCP2.2 authentication register
    ret = hdcprxxx_select_page_of_0x14(port, HDCPRXXX_PAGE_HDCP22_AUTH_REG);
    CHECK_ERROR(ret);

    // page 0x14: 12

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_VHRP0_04_ADDR,
                        HDCPTX22_VD_BYTE_SIZE, p_vd);

    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_write_rxidlist_msg(system_port port, uint8_t *p_vd_data,
                                                uint8_t *p_vd, uint8_t *p_msg_size)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_RXIDLIST_MSG_BYTE_SIZE] = { 0 };
    uint8_t len, rxid_len, i, j;
    uint32_t ret;

    if (p_vd_data == NULL || p_vd == NULL || p_msg_size == NULL) {
        return ERR_NULL_POINTER;
    }

    // Select HDCP page: HDCP2.2 read message 0
    ret = hdcprxxx_select_page_of_0x14(port, HDCPRXXX_PAGE_HDCP22_READ_MSG0);
    CHECK_ERROR(ret);

    // page 0x14: 9

    // HDCPRX22_MSG_ID_BYTE_SIZE
    len = HDCPRX22_MSG_ID_BYTE_SIZE;
    // + (RxID * HDCPTX22_RECEIVER_ID_BYTE_SIZE) + HDCPTX22_RXINFO_BYTE_SIZE + HDCPTX22_SEQ_NUM_V_BYTE_SIZE
    len += p_vd_data[0];
    // + HDCPTX22_VD_BYTE_SIZE
    len += HDCPTX22_VD_BYTE_SIZE;
    // set message size
    *p_msg_size = len;

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_RMSGID_ADDR,
                        len, data);
    // msg_id = 12
    SET_BLOCK_DATA(data, 0, HDCPRX22_MSG_ID_RXIDLIST);
    // RxInfo
    rxid_len = p_vd_data[0] - HDCPTX22_RXINFO_BYTE_SIZE - HDCPTX22_SEQ_NUM_V_BYTE_SIZE;
    j = HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE + rxid_len;
    SET_BLOCK_DATA(data, 1, p_vd_data[j]);
    SET_BLOCK_DATA(data, 2, p_vd_data[j + 1]);
    // seq_num_V
    j = HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE + rxid_len + HDCPTX22_RXINFO_BYTE_SIZE;
    SET_BLOCK_DATA(data, 3, p_vd_data[j]);
    SET_BLOCK_DATA(data, 4, p_vd_data[j + 1]);
    SET_BLOCK_DATA(data, 5, p_vd_data[j + 2]);
    // V'
    for (i=HDCPRX22_WRITE_RXIDLIST_MSG_VD_OFFSET, j=0; j<HDCPTX22_VD_BYTE_SIZE; i++, j++) {
        SET_BLOCK_DATA(data, i, p_vd[j]);
    }
    // receiver ID list
    for (i=HDCPRX22_WRITE_RXIDLIST_MSG_RXID_OFFSET, j=HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE;
            j<(HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE + rxid_len); i++, j++) {
        SET_BLOCK_DATA(data, i, p_vd_data[j]);
    }

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxidlist_msg(system_port port, uint8_t *p_msg_size)
{
    uint8_t vd_data[HDCPRX22_VD_CALC_DATA_MAX_BYTE_SIZE];
    uint8_t vd[HDCPTX22_VD_BYTE_SIZE];
    uint32_t ret;

    if (p_msg_size == NULL) {
        return ERR_NULL_POINTER;
    }

    *p_msg_size = 0;

    // read out V' calc data
    ret = hdcprx22_read_vd_calc_data(port, vd_data);
    CHECK_ERROR(ret);
    // read out V'
    ret = hdcprx22_read_vd(port, vd);
    CHECK_ERROR(ret);
    // set RepeaterAuth_Send_ReceiverID_List message
    ret = hdcprx22_write_rxidlist_msg(port, vd_data, vd, p_msg_size);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxstatus(system_port port, uint8_t no,uint8_t rxstatus_15_8,
                                        uint8_t mask_15_8, uint8_t rxstatus_7_0, uint8_t mask_7_0)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_SET_RXSTATUS_LEN] = { 0 };
    uint8_t md1, md2;
    uint32_t ret;

    // Select HDCP page: HDCP Port#
    ret = hdcprxxx_select_page_of_0x14(port, no);
    CHECK_ERROR(ret);

    // page 0x14: 0,1,2,3

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_RXSTATP0_00_ADDR,
                        HDCPRX22_SET_RXSTATUS_LEN, data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    // RXSTATPx[7:0]
    md1 = data[0] & ~mask_7_0;
    md2 = rxstatus_7_0 & mask_7_0;
    SET_BLOCK_DATA(data, 0, (md1 | md2));                        // 0x1470
    // RXSTATPx[15:8]
    md1 = data[1] & ~mask_15_8;
    md2 = rxstatus_15_8 & mask_15_8;
    SET_BLOCK_DATA(data, 1, (md1 | md2));                        // 0x1471

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxstatus_ready_msgsize(system_port port, uint8_t no, uint8_t msg_size)
{
    uint8_t rxstatus_15_8, mask_15_8, rxstatus_7_0, mask_7_0;;
    uint32_t ret;

    // RxStatus.READY
    rxstatus_15_8 = HDCPRX22_SET_RXSTATUS_MSB_READY_1;
    mask_15_8 = HDCPRX22_SET_RXSTATUS_MSB_READY_MASK;
    // RxStatus.Message_Size (NOTE: HV read message buffer size max is (256 - 4))
    rxstatus_7_0 = msg_size;
    mask_7_0 = HDCPRX22_SET_RXSTATUS_LSB_MSG_SIZE_MASK;

    ret = hdcprx22_set_rxstatus(port, no, rxstatus_15_8, mask_15_8, rxstatus_7_0, mask_7_0);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_check_auth_status(hdmirx_hdcp_auth_status *p_status, uint8_t no)
{
    hdmirx_hdcp_message msg;
    uint8_t msg_size;
    uint32_t ret;

    if (p_status == NULL) {
        return ERR_NULL_POINTER;
    }

    // when repeater, the following must be done.
    if (p_status->repeater != HDMIRX_HDCP_REPEATER) {
        return ERR_OK;
    }

    // Check HDCP 1.4 interrupt request message, when repeater
    // received Aksv from the selected port ?
    ret = hdcprx14_check_irq(p_status->port, no, &msg);
    CHECK_ERROR(ret);
    if (msg == HDMIRX_HDCP_MESSAGE_AKSV) {
        // perform 1st authentication
        ret = hdcprx14_1st_auth(p_status->port, no);
        CHECK_ERROR(ret);
        // clear AKREDYx
        ret = hdcprx14_clear_akredy(p_status->port, no);
        CHECK_ERROR(ret);
        // ksvlist has already been set
        // calculate V'.Hn
        ret = hdcprx14_calc_vddhn(p_status->port, no);
        CHECK_ERROR(ret);
        // Bstatus has already been set
        // set Bcaps.READY
        ret = hdcprx14_set_bcaps_ready(p_status->port, no);
        CHECK_ERROR(ret);

        p_status->message = msg;
    }
    // Check HDCP 2.2 interrupt request message, when repeater
    // received HDCP2.2 message ?
    ret = hdcprx22_check_irq(p_status->port, &msg);
    CHECK_ERROR(ret);
    if (msg != HDMIRX_HDCP_MESSAGE_NONE) {
        switch (msg) {
            case HDMIRX_HDCP_MESSAGE_AKE_INIT:
                ret = hdcprx22_ack_ake_init(p_status->port);
                CHECK_ERROR(ret);
                break;
            case HDMIRX_HDCP_MESSAGE_AKE_NO_STORED_KM:
                // pop V' calc data, because delete it below.
                ret = hdcprx22_vd_data_ctrl(p_status->port, HDCPRX22_VD_DATA_CTRL_POP);
                CHECK_ERROR(ret);
                ret = hdcprx22_ack_ake_no_stored_km(p_status->port);
                CHECK_ERROR(ret);
                // push V' calc data
                ret = hdcprx22_vd_data_ctrl(p_status->port, HDCPRX22_VD_DATA_CTRL_PUSH);
                CHECK_ERROR(ret);
                break;
            case HDMIRX_HDCP_MESSAGE_AKE_STORED_KM:
                ret = hdcprx22_ack_ake_stored_km(p_status->port);
                CHECK_ERROR(ret);
                break;
            case HDMIRX_HDCP_MESSAGE_LC_INIT:
                ret = hdcprx22_ack_lc_init(p_status->port);
                CHECK_ERROR(ret);
                break;
            case HDMIRX_HDCP_MESSAGE_SKE_SEND_EKS:
                ret = hdcprx22_ack_ske_send_eks(p_status->port);
                CHECK_ERROR(ret);
                // calculate V'
                ret = hdcprx22_calc_vd(p_status->port);
                CHECK_ERROR(ret);
                // set RepeaterAuth_Send_ReceiverID_List message
                ret = hdcprx22_set_rxidlist_msg(p_status->port, &msg_size);
                CHECK_ERROR(ret);
                // set RxStatus.READY, RxStatus.Message_Size
                ret = hdcprx22_set_rxstatus_ready_msgsize(p_status->port, no, msg_size);
                CHECK_ERROR(ret);
                break;
            case HDMIRX_HDCP_MESSAGE_REPEATERAUTH_SEND_ACK:
                ret = hdcprx22_ack_repeaterauth_send_ack(p_status->port);
                CHECK_ERROR(ret);
                break;
            case HDMIRX_HDCP_MESSAGE_REPEATERAUTH_STREAM_MANAGE:
                ret = hdcprx22_ack_repeaterauth_stream_manage(p_status->port);
                CHECK_ERROR(ret);
                break;
            default:
                return ERR_NG;
        }
        p_status->message = msg;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_irq_mask(system_port port, uint8_t msk0, uint8_t msk1,
                                        uint8_t msk2, uint8_t msk3)
{
    system_block_access baccess;
    uint8_t data[HDCPRX14_SET_IRQ_MASK_LEN] = { 0 };
    uint32_t ret;

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_INTMSK_04_ADDR,
                        HDCPRX14_SET_IRQ_MASK_LEN, data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    SET_BLOCK_DATA(data, 0, msk0 | data[0]);                        // 0x1074
    SET_BLOCK_DATA(data, 1, msk1 | data[1]);                        // 0x1075
    SET_BLOCK_DATA(data, 2, msk2 | data[2]);                        // 0x1076
    SET_BLOCK_DATA(data, 3, msk3 | data[3]);                        // 0x1077

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_irq_status_mask(system_port port, uint8_t smsk0, uint8_t smsk1)
{
    system_block_access baccess;
    uint8_t data[HDCPRX14_SET_IRQ_STATUS_MASK_LEN] = { 0 };
    uint32_t ret;

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_INTSTAMSK_00_ADDR,
                        HDCPRX14_SET_IRQ_STATUS_MASK_LEN, data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    SET_BLOCK_DATA(data, 0, smsk0 | data[0]);                        // 0x106C
    SET_BLOCK_DATA(data, 1, smsk1 | data[1]);                        // 0x106D

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_irq_main_status_mask(system_port port, uint8_t mmsk0, uint8_t mmsk1)
{
    system_block_access baccess;
    uint8_t data[HDCPRX14_SET_IRQ_MAIN_STATUS_MASK_LEN] = { 0 };
    uint32_t ret;

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_SYSTEM_MINTSTAMSK_00_ADDR,
                        HDCPRX14_SET_IRQ_MAIN_STATUS_MASK_LEN, data);
    ret = device_block_read(&baccess);
    CHECK_ERROR(ret);

    SET_BLOCK_DATA(data, 0, mmsk0 | data[0]);                        // 0x1068
    SET_BLOCK_DATA(data, 1, mmsk1 | data[1]);                        // 0x1069

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_set_irq(hdmirx_hdcp_auth *p_auth)
{
    uint8_t msk0, msk1, msk2, msk3;
    uint8_t smsk0, smsk1;
    uint8_t mmsk0, mmsk1;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // when repeater and HDCP1.4/2.2, set the following mask
    if (p_auth->repeater == HDMIRX_HDCP_REPEATER) {
        smsk0 = HDCPRXXX_SET_IRQ_NONE;
        smsk1 = HDCPRXXX_SET_IRQ_NONE;
        mmsk0 = HDCPRXXX_SET_IRQ_NONE;
        mmsk1 = HDCPRXXX_SET_IRQ_NONE;
        // HDCP Port0
        if (p_auth->port0 == HDMIRX_HDCP_VERSION_1_4) {
            msk0 = HDCPRXXX_SET_IRQ_AKSV;
            smsk0 |= HDCPRXXX_SET_IRQ_REG4_ENA;
            mmsk0 = HDCPRXXX_SET_IRQ_CCIRQST_ENA;
        } else if (p_auth->port0 == HDMIRX_HDCP_VERSION_2_2) {
            msk0 = HDCPRXXX_SET_IRQ_H22MSG0;                            // HDCP2.2 Port0,1,2,3
            smsk0 |= HDCPRXXX_SET_IRQ_REG4_ENA;
            mmsk0 = HDCPRXXX_SET_IRQ_CCIRQST_ENA;
        } else if (p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO) {
            msk0 = HDCPRXXX_SET_IRQ_AKSV | HDCPRXXX_SET_IRQ_H22MSG0;    // HDCP2.2 Port0,1,2,3
            smsk0 |= HDCPRXXX_SET_IRQ_REG4_ENA;
            mmsk0 = HDCPRXXX_SET_IRQ_CCIRQST_ENA;
        } else {
            msk0 = HDCPRXXX_SET_IRQ_NONE;
            smsk0 |= HDCPRXXX_SET_IRQ_REG4_DIS;
        }
        // HDCP Port1
        if (p_auth->port1 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO) {
            msk1 = HDCPRXXX_SET_IRQ_AKSV;
            smsk0 |= HDCPRXXX_SET_IRQ_REG5_ENA;
            mmsk0 = HDCPRXXX_SET_IRQ_CCIRQST_ENA;
        } else {
            msk1 = HDCPRXXX_SET_IRQ_NONE;
            smsk0 |= HDCPRXXX_SET_IRQ_REG5_DIS;
        }
        // HDCP Port2
        if (p_auth->port2 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO) {
            msk2 = HDCPRXXX_SET_IRQ_AKSV;
            smsk0 |= HDCPRXXX_SET_IRQ_REG6_ENA;
            mmsk0 = HDCPRXXX_SET_IRQ_CCIRQST_ENA;
        } else {
            msk2 = HDCPRXXX_SET_IRQ_NONE;
            smsk0 |= HDCPRXXX_SET_IRQ_REG6_DIS;
        }
        // HDCP Port3
        if (p_auth->port3 == HDMIRX_HDCP_VERSION_1_4 || p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO) {
            msk3 = HDCPRXXX_SET_IRQ_AKSV;
            smsk0 |= HDCPRXXX_SET_IRQ_REG7_ENA;
            mmsk0 = HDCPRXXX_SET_IRQ_CCIRQST_ENA;
        } else {
            msk3 = HDCPRXXX_SET_IRQ_NONE;
            smsk0 |= HDCPRXXX_SET_IRQ_REG7_DIS;
        }
    } else {
        msk0 = HDCPRXXX_SET_IRQ_NONE;
        msk1 = HDCPRXXX_SET_IRQ_NONE;
        msk2 = HDCPRXXX_SET_IRQ_NONE;
        msk3 = HDCPRXXX_SET_IRQ_NONE;
        smsk0 = HDCPRXXX_SET_IRQ_NONE;
        smsk1 = HDCPRXXX_SET_IRQ_NONE;
        mmsk0 = HDCPRXXX_SET_IRQ_NONE;
        mmsk1 = HDCPRXXX_SET_IRQ_NONE;
    }
    // Set IRQ mask
    ret = hdcprx14_set_irq_mask(p_auth->port, msk0, msk1, msk2, msk3);
    CHECK_ERROR(ret);
    // Set IRQ status mask
    ret = hdcprx14_set_irq_status_mask(p_auth->port, smsk0, smsk1);
    CHECK_ERROR(ret);
    // Set IRQ main status mask
    ret = hdcprx14_set_irq_main_status_mask(p_auth->port, mmsk0, mmsk1);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_irq_mask(system_port port, uint8_t msk0, uint8_t msk1,
                                        uint8_t msk2, uint8_t msk3)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_SET_IRQ_MASK_LEN];
    uint32_t ret;

    // page 0x14: 12

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_MSGMSK_00_ADDR,
                        HDCPRX22_SET_IRQ_MASK_LEN, data);
    // it is not necessary to read and mask write
    SET_BLOCK_DATA(data, 0, msk0);                        // 0x14A8
    SET_BLOCK_DATA(data, 1, msk1);                        // 0x14A9
    SET_BLOCK_DATA(data, 2, msk2);                        // 0x14AA
    SET_BLOCK_DATA(data, 3, msk3);                        // 0x14AB

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_irq(hdmirx_hdcp_auth *p_auth)
{
    uint8_t msk0, msk1, msk2, msk3;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // when repeater and HDCP2.2, set the following mask
    if (p_auth->repeater == HDMIRX_HDCP_REPEATER
        &&
        (p_auth->port0 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port1 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port2 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port3 == HDMIRX_HDCP_VERSION_2_2
        || p_auth->port0 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port1 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port2 == HDMIRX_HDCP_VERSION_AUTO
        || p_auth->port3 == HDMIRX_HDCP_VERSION_AUTO)
        ) {
        // 0x14:0xA8 bit 0: AKE_Init
        // 0x14:0xA8 bit 2: AKE_No_Stored_km
        // 0x14:0xA8 bit 3: AKE_Stored_km
        // 0x14:0xA8 bit 6: LC_Init
        msk0 = HDCPRX22_IRQ0_AKE_INIT
                | HDCPRX22_IRQ0_AKE_NO_STORED_KM
                | HDCPRX22_IRQ0_AKE_STORED_KM
                | HDCPRX22_IRQ0_LC_INIT;
        // 0x14:0xA9 bit 0: SKE_Send_Eks
        // 0x14:0xA9 bit 2: RepeaterAuth_Send_Ack
        // 0x14:0xA9 bit 3: RepeaterAuth_Stream_Manage
        msk1 = HDCPRX22_IRQ1_SKE_SEND_EKS
                | HDCPRX22_IRQ1_REPEATERAUTH_SEND_ACK
                | HDCPRX22_IRQ1_REPEATERAUTH_STREAM_MANAGE;
        // 0x14:0xAA: none
        msk2 = HDCPRX22_IRQ_NONE;
        // 0x14:0xAB: none
        msk3 = HDCPRX22_IRQ_NONE;
    } else {
        // 0x14:0xA8: none
        msk0 = HDCPRX22_IRQ_NONE;
        // 0x14:0xA9: none
        msk1 = HDCPRX22_IRQ_NONE;
        // 0x14:0xAA: none
        msk2 = HDCPRX22_IRQ_NONE;
        // 0x14:0xAB: none
        msk3 = HDCPRX22_IRQ_NONE;
    }
    // Select HDCP page: HDCP2.2 authentication register
    ret = hdcprxxx_select_page_of_0x14(p_auth->port, HDCPRXXX_PAGE_HDCP22_AUTH_REG);
    CHECK_ERROR(ret);
    // page 0x14: 12
    // Set IRQ mask
    ret = hdcprx22_set_irq_mask(p_auth->port, msk0, msk1, msk2, msk3);
    CHECK_ERROR(ret);
    // AKEIRQ mask setting is done by hdmirx_init()

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_select_page_of_ksvlist(system_port port, uint8_t no, uint8_t page)
{
    system_reg_access access;
    system_command cmd[HDCPRXXX_SELECT_PAGE_OF_KSVLIST_NUM];
    uint32_t ret;

    SET_ACCESS(access, &port, cmd, HDCPRXXX_SELECT_PAGE_OF_KSVLIST_NUM);
    SET_COMMAND(cmd, 0, VIDEOIF_AKERX_KSVPAGE0_ADDR + (no * HDCPRXXX_PORT_OFFSET),
                        page,
                        HDCPRXXX_SELECT_PAGE_OF_KSVLIST_KSVPAGEX_MASK);
    ret = lf9129_device_write(&access);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_ksv_of_portno(system_port port, uint8_t no, uint8_t idx, uint8_t *p_ksv)
{
    system_block_access baccess;
    uint8_t page, pos;
    uint32_t ret;

    if (idx >= HDCPTX14_BSTATUS_DEVICE_COUNT_MAX) {
        // if idx >= 16, MAX_DEVS_EXCEEDED=1 of Bstatus must be set.
        return ERR_OK;
    }

    // select page of ksvlist
    page = idx / HDCPRXXX_SELECT_PAGE_OF_KSVLIST_PORT_NUM;
    ret = hdcprxxx_select_page_of_ksvlist(port, no, page);
    CHECK_ERROR(ret);
    // set ksv
    pos = idx % HDCPRXXX_SELECT_PAGE_OF_KSVLIST_PORT_NUM;
    SET_BLOCK_ACCESS(baccess, &port,
                        VIDEOIF_AKERX_KSVLIST0_00_ADDR                // 0x1628 (0x1658, 0x1688, 0x16B8)
                            + (no * HDCPRXXX_PORT_OFFSET)            // + 0x00, 0x30, 0x60, 0x90
                            + (pos * HDMITX_HDCP_KSV_BYTE_SIZE),    // + 0x00, 0x05, 0x0A, 0x0F
                        HDMITX_HDCP_KSV_BYTE_SIZE,
                        p_ksv);
    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_ksvlist(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                        hdmitx_hdcp_repeater_auth *p_tx_rep)
{
    uint8_t idx, cnt, i;
    uint32_t ret;

    if (p_rx == NULL || p_tx == NULL || p_tx_rep == NULL) {
        return ERR_NULL_POINTER;
    }

    // set downstream ksv
    idx = 0;
    // HDCP Port0
    if (p_rx->port0 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port0 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_0, idx, p_tx->bksv);
        CHECK_ERROR(ret);
    }
    // HDCP Port1
    if (p_rx->port1 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port1 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_1, idx, p_tx->bksv);
        CHECK_ERROR(ret);
    }
    // HDCP Port2
    if (p_rx->port2 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port2 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_2, idx, p_tx->bksv);
        CHECK_ERROR(ret);
    }
    // HDCP Port3
    if (p_rx->port3 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port3 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_3, idx, p_tx->bksv);
        CHECK_ERROR(ret);
    }
    // set ksvlist, if downstream is repeater
    if (p_tx->is_repeater == HDMITX_HDCP_REPEATER) {
        cnt = p_tx_rep->downstream_count;
        for (i=0; i<cnt; i++) {
            idx++;
            // HDCP Port0
            if (p_rx->port0 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port0 == HDMIRX_HDCP_VERSION_AUTO) {
                ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_0, idx, p_tx_rep->ksv_list[i]);
                CHECK_ERROR(ret);
            }
            // HDCP Port1
            if (p_rx->port1 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port1 == HDMIRX_HDCP_VERSION_AUTO) {
                ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_1, idx, p_tx_rep->ksv_list[i]);
                CHECK_ERROR(ret);
            }
            // HDCP Port2
            if (p_rx->port2 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port2 == HDMIRX_HDCP_VERSION_AUTO) {
                ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_2, idx, p_tx_rep->ksv_list[i]);
                CHECK_ERROR(ret);
            }
            // HDCP Port3
            if (p_rx->port3 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port3 == HDMIRX_HDCP_VERSION_AUTO) {
                ret = hdcprx14_set_ksv_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_3, idx, p_tx_rep->ksv_list[i]);
                CHECK_ERROR(ret);
            }
        }
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_bstatus_of_portno(system_port port, uint8_t no, uint8_t bs0, uint8_t bs1)
{
    system_block_access baccess;
    uint8_t data[HDCPRX14_SET_BSTATUS_OF_PORTNO_LEN];
    uint32_t ret;

    // Select HDCP page: HDCP Port#
    ret = hdcprxxx_select_page_of_0x14(port, no);
    CHECK_ERROR(ret);

    // page 0x14: 0,1,2,3

    SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_BSTARP0_00_ADDR,
                        HDCPRX14_SET_BSTATUS_OF_PORTNO_LEN, data);
    // it is not necessary to read and mask write
    SET_BLOCK_DATA(data, 0, bs0);                        // 0x1441
    SET_BLOCK_DATA(data, 1, bs1);                        // 0x1442

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_bstatus(hdmirx_hdcp_auth *p_rx, uint8_t count, uint8_t depth)
{
    uint8_t bstatus_7_0 = 0, bstatus_15_8 = 0;
    uint32_t ret;

    if (p_rx == NULL) {
        return ERR_NULL_POINTER;
    }

    // 6:0   DEVICE_COUNT
    bstatus_7_0 |= count & HDCPTX14_BSTATUS_DEVICE_COUNT_MASK;
    // 7     MAX_DEVS_EXCEEDED
    bstatus_7_0 |= (count > HDCPTX14_BSTATUS_DEVICE_COUNT_MAX)
                    ? HDCPTX14_BSTATUS_MAX_DEVS_EXCEEDED_1
                    : HDCPTX14_BSTATUS_MAX_DEVS_EXCEEDED_0;
    // 10:8  DEPTH
    bstatus_15_8 |= depth & HDCPTX14_BSTATUS_DEPTH_MASK;
    // 11    MAX_CASCADE_EXCEEDED
    bstatus_15_8 |= (depth > HDCPTX14_BSTATUS_DEPTH_MAX)
                    ? HDCPTX14_BSTATUS_MAX_CASCADE_EXCEEDED_1
                    : HDCPTX14_BSTATUS_MAX_CASCADE_EXCEEDED_0;
    // 12    HDMI_MODE
    bstatus_15_8 |= HDCPTX14_BSTATUS_HDMI_MODE_HDMI & HDCPTX14_BSTATUS_HDMI_MODE_MASK;
    // 13    HDMI_RESERVED_2
    // 15:14 Rsvd (0)

    // HDCP Port0
    if (p_rx->port0 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port0 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_bstatus_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_0, bstatus_7_0, bstatus_15_8);
        CHECK_ERROR(ret);
    }
    // HDCP Port1
    if (p_rx->port1 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port1 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_bstatus_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_1, bstatus_7_0, bstatus_15_8);
        CHECK_ERROR(ret);
    }
    // HDCP Port2
    if (p_rx->port2 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port2 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_bstatus_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_2, bstatus_7_0, bstatus_15_8);
        CHECK_ERROR(ret);
    }
    // HDCP Port3
    if (p_rx->port3 == HDMIRX_HDCP_VERSION_1_4 || p_rx->port3 == HDMIRX_HDCP_VERSION_AUTO) {
        ret = hdcprx14_set_bstatus_of_portno(p_rx->port, SYSTEM_PORT_NUMBER_3, bstatus_7_0, bstatus_15_8);
        CHECK_ERROR(ret);
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx14_set_repeater_info(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                            hdmitx_hdcp_repeater_auth *p_tx_rep)
{
    uint8_t down_count, down_depth;
    uint32_t ret;

    if (p_rx == NULL || p_tx == NULL || p_tx_rep == NULL) {
        return ERR_NULL_POINTER;
    }

    // calc downstream count & depth
    down_count = p_tx_rep->downstream_count + 1;    // kvslist count + just downstream one
    down_depth = p_tx_rep->downstream_depth + 1;    // downstream depth + oneself
    // set downstream ksvlist
    ret = hdcprx14_set_ksvlist(p_rx, p_tx, p_tx_rep);
    CHECK_ERROR(ret);
    // set Bstatus, when both repeater and non-repeater
    ret = hdcprx14_set_bstatus(p_rx, down_count, down_depth);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_vd_data_ctrl(system_port port, uint8_t ctrl)
{
    static uint8_t data[HDCPRX22_SET_RXIDLIST_MAX_LEN] = { 0 };

    system_block_access baccess;
    uint8_t len;
    uint32_t ret;

    // Select HDCP page: HDCP2.2 repeater infomation
    ret = hdcprxxx_select_page_of_0x14(port, HDCPRXXX_PAGE_HDCP22_REPEATER_INFO);
    CHECK_ERROR(ret);

    // page 0x14: 25

    if (ctrl == HDCPRX22_VD_DATA_CTRL_PUSH) {
        // get length
        len = data[0] + HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE;
        // set V' calc data
        SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_BKSVRP0_00_ADDR,
                            len, data);
        ret = device_block_write(&baccess);
        CHECK_ERROR(ret);
    } else if (ctrl == HDCPRX22_VD_DATA_CTRL_POP) {
        // get length
        SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_BKSVRP0_00_ADDR,
                            HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE, data);
        ret = device_block_read(&baccess);
        CHECK_ERROR(ret);
        len = data[0] + HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE;
        // get V' calc data
        SET_BLOCK_ACCESS(baccess, &port, VIDEOIF_HDCPRX_BKSVRP0_00_ADDR,
                            len, data);
        ret = device_block_read(&baccess);
        CHECK_ERROR(ret);
    } else {
        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxidlist(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                        hdmitx_hdcp_repeater_auth *p_tx_rep)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_SET_RXIDLIST_MAX_LEN] = { 0 };
    uint8_t down_count, length, len, i, j, k;
    uint32_t ret;

    if (p_rx == NULL || p_tx == NULL || p_tx_rep == NULL) {
        return ERR_NULL_POINTER;
    }

    // page 0x14: 25

    // calc downstream count & length
    down_count = p_tx_rep->downstream_count + 1;    // kvslist count + just downstream one
    length = (down_count * HDCPTX22_RECEIVER_ID_BYTE_SIZE)
                + HDCPTX22_RXINFO_BYTE_SIZE
                + HDCPTX22_SEQ_NUM_V_BYTE_SIZE;
    len = HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE + (down_count * HDCPTX22_RECEIVER_ID_BYTE_SIZE);

    SET_BLOCK_ACCESS(baccess, &(p_rx->port), VIDEOIF_HDCPRX_BKSVRP0_00_ADDR,
                        len, data);
    // length
    SET_BLOCK_DATA(data, 0, length);
    SET_BLOCK_DATA(data, 1, HDCPRX22_SET_RXIDLIST_0_PADDING);
    SET_BLOCK_DATA(data, 2, HDCPRX22_SET_RXIDLIST_0_PADDING);
    SET_BLOCK_DATA(data, 3, HDCPRX22_SET_RXIDLIST_0_PADDING);
    // receiver ID list
    // 1st only
    i = HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE;
    for (j=0; j<HDCPTX22_RECEIVER_ID_BYTE_SIZE; i++, j++) {
        SET_BLOCK_DATA(data, i, p_tx->bksv[j]);
    }
    // 2nd and subsequent receiver ID
    for (k=0; k<p_tx_rep->downstream_count; k++) {
        i = HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE
            + HDCPTX22_RECEIVER_ID_BYTE_SIZE
            + (k * HDCPTX22_RECEIVER_ID_BYTE_SIZE);
        for (j=0; j<HDCPTX22_RECEIVER_ID_BYTE_SIZE; i++, j++) {
            SET_BLOCK_DATA(data, i, p_tx_rep->ksv_list[k][j]);
        }
    }

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_rxinfo(hdmirx_hdcp_auth *p_rx, uint8_t count, uint8_t depth,
                                    uint8_t hdcp1dev, uint8_t hdcp2rep)
{
    system_block_access baccess;
    uint8_t data[HDCPRX22_SET_RXINFO_LEN] = { 0 };
    uint8_t rxinfo_7_0 = 0, rxinfo_15_8 = 0;
    uint8_t offset;
    uint32_t ret;

    if (p_rx == NULL) {
        return ERR_NULL_POINTER;
    }

    // 0     HDCP1_DEVICE_DOWNSTREAM
    rxinfo_7_0 |= hdcp1dev;
    // 1     HDCP2_0_REPEATER_DOWNSTREAM
    rxinfo_7_0 |= hdcp2rep;
    // 2     MAX_CASCADE_EXCEEDED
    rxinfo_15_8 |= (depth > HDCPTX22_RXINFO_DEPTH_MAX)
                    ? HDCPTX22_RXINFO_MAX_CASCADE_EXCEEDED_1
                    : HDCPTX22_RXINFO_MAX_CASCADE_EXCEEDED_0;
    // 3     MAX_DEVS_EXCEEDED
    rxinfo_7_0 |= (count > HDCPTX22_RXINFO_DEVICE_COUNT_MAX)
                    ? HDCPTX22_RXINFO_MAX_DEVS_EXCEEDED_1
                    : HDCPTX22_RXINFO_MAX_DEVS_EXCEEDED_0;
    // 8:4   DEVICE_COUNT
    rxinfo_7_0 |= (count & HDCPTX22_RXINFO_DEVICE_COUNT_7_4_MASK) << HDCPTX22_RXINFO_DEVICE_COUNT_BIT;
    rxinfo_15_8 |= (count & HDCPTX22_RXINFO_DEVICE_COUNT_8_8_MASK) >> HDCPTX22_RXINFO_DEVICE_COUNT_BIT;
    // 11:9  DEPTH
    rxinfo_15_8 |= (depth & HDCPTX22_RXINFO_DEPTH_11_9_MASK) << HDCPTX22_RXINFO_DEPTH_BIT;
    // 15:12 Rsvd (0)

    // page 0x14: 25

    offset = HDCPRX22_V_HASH_CALC_HEADER_BYTE_SIZE + (count * HDCPTX22_RECEIVER_ID_BYTE_SIZE);
    SET_BLOCK_ACCESS(baccess, &(p_rx->port), VIDEOIF_HDCPRX_BKSVRP0_00_ADDR + offset,
                        HDCPRX22_SET_RXINFO_LEN, data);
    SET_BLOCK_DATA(data, 0, rxinfo_15_8);
    SET_BLOCK_DATA(data, 1, rxinfo_7_0);
    SET_BLOCK_DATA(data, 2, HDCPTX22_SEQ_NUM_V_PARAM_23_16);
    SET_BLOCK_DATA(data, 3, HDCPTX22_SEQ_NUM_V_PARAM_15__8);
    SET_BLOCK_DATA(data, 4, HDCPTX22_SEQ_NUM_V_PARAM__7__0);

    ret = device_block_write(&baccess);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprx22_set_repeater_info(hdmirx_hdcp_auth *p_rx, hdmitx_hdcp_auth *p_tx,
                                            hdmitx_hdcp_repeater_auth *p_tx_rep)
{
    uint8_t down_count, down_depth, hdcp1dev, hdcp2rep;
    uint32_t ret;

    if (p_rx == NULL || p_tx == NULL || p_tx_rep == NULL) {
        return ERR_NULL_POINTER;
    }

    if (!(p_rx->port0 == HDMIRX_HDCP_VERSION_2_2
        || p_rx->port1 == HDMIRX_HDCP_VERSION_2_2
        || p_rx->port2 == HDMIRX_HDCP_VERSION_2_2
        || p_rx->port3 == HDMIRX_HDCP_VERSION_2_2
        || p_rx->port0 == HDMIRX_HDCP_VERSION_AUTO
        || p_rx->port1 == HDMIRX_HDCP_VERSION_AUTO
        || p_rx->port2 == HDMIRX_HDCP_VERSION_AUTO
        || p_rx->port3 == HDMIRX_HDCP_VERSION_AUTO)) {
        return ERR_OK;
    }

    // calc downstream count & depth
    down_count = p_tx_rep->downstream_count + 1;    // kvslist count + just downstream one
    down_depth = p_tx_rep->downstream_depth + 1;    // downstream depth + oneself
    // set HDCP1_DEVICE_DOWNSTREAM
    if (p_tx->hdcp_version == HDMITX_HDCP_VERSION_1_4) {
        hdcp1dev = HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_1;
    } else if (p_tx->hdcp_version == HDMITX_HDCP_VERSION_2_2) {
        hdcp1dev = (p_tx_rep->bstatus[1] & HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_MASK)
                    ? HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_1
                    : HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_0;
    } else {
        hdcp1dev = HDCPTX22_RXINFO_HDCP1_DEVICE_DOWNSTREAM_0;
    }
    // set HDCP2_0_REPEATER_DOWNSTREAM
    if (p_tx->hdcp_version == HDMITX_HDCP_VERSION_2_2) {
        hdcp2rep = (p_tx_rep->bstatus[1] & HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_MASK)
                    ? HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_1
                    : HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_0;
    } else {
        hdcp2rep = HDCPTX22_RXINFO_HDCP2_0_REPEATER_DOWNSTREAM_0;
    }
    // Select HDCP page: HDCP2.2 repeater infomation
    ret = hdcprxxx_select_page_of_0x14(p_rx->port, HDCPRXXX_PAGE_HDCP22_REPEATER_INFO);
    CHECK_ERROR(ret);
    // page 0x14: 25
    // set downstream receiver ID list
    ret = hdcprx22_set_rxidlist(p_rx, p_tx, p_tx_rep);
    CHECK_ERROR(ret);
    // page 0x14: 25
    // set RxInfo, seq_num
    ret = hdcprx22_set_rxinfo(p_rx, down_count, down_depth, hdcp1dev, hdcp2rep);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_set_repeater_info(hdmirx_hdcp_auth *p_auth)
{
    hdmitx_hdcp_auth tx_auth;
    hdmitx_hdcp_repeater_auth tx_rep_auth;
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // when repeater, the following must be done.
    if (p_auth->repeater != HDMIRX_HDCP_REPEATER) {
        return ERR_OK;
    }

    // SG on
    ret = hdmitx_set_sg_on(p_auth->port);
    CHECK_ERROR(ret);
    // get HDCP downstream info
    tx_auth.port = p_auth->port;
    ret = hdmitx_hdcp_authentication(&tx_auth);
    CHECK_ERROR(ret);
    // get HDCP repeater info
    tx_rep_auth.port = tx_auth.port;
    tx_rep_auth.hdcp_version = tx_auth.hdcp_version;
    if (tx_auth.is_repeater == HDMITX_HDCP_REPEATER) {
        ret = hdmitx_hdcp_repeater_authentication(&tx_rep_auth);
        CHECK_ERROR(ret);
    } else {
        tx_rep_auth.downstream_count = 0;
        tx_rep_auth.downstream_depth = 0;
        tx_rep_auth.bstatus[0] = 0;
        tx_rep_auth.bstatus[1] = 0;
    }
    // reset HDCP Tx
    ret = hdmitx_hdcp_authentication_reset(&tx_auth);
    CHECK_ERROR(ret);
    // SG off
    ret = hdmitx_set_sg_off(p_auth->port);
    CHECK_ERROR(ret);
    // Set HDCP 1.4 repeater info
    ret = hdcprx14_set_repeater_info(p_auth, &tx_auth, &tx_rep_auth);
    CHECK_ERROR(ret);
    // Set HDCP 2.2 repeater info
    ret = hdcprx22_set_repeater_info(p_auth, &tx_auth, &tx_rep_auth);
    CHECK_ERROR(ret);

    return ret;
}
#endif // Product Number having HDMI Rx & HDMI Tx
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_init(system_port port)
{
    // init Rx abort request
    hdcprxxx_ctrl_set_abort_req(SYSTEM_REQUEST_NONE);

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t hdcprxxx_ctrl_req_abort(system_request *p_req, hdcpxx_ctrl_req ctrl)
{
    static system_request hdmirx_hdcp_abort_req = SYSTEM_REQUEST_NONE;

    if (p_req == NULL) {
        return ERR_NULL_POINTER;
    }

    if (ctrl == HDCPXX_CTRL_REQ_GET) {
        *p_req = hdmirx_hdcp_abort_req;
    } else if (ctrl == HDCPXX_CTRL_REQ_SET) {
        hdmirx_hdcp_abort_req = *p_req;
    } else {
        return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static system_request hdcprxxx_ctrl_get_abort_req(void)
{
    system_request req;

    if (hdcprxxx_ctrl_req_abort(&req, HDCPXX_CTRL_REQ_GET)) {
        return SYSTEM_REQUEST_NONE;
    }

    return req;
}
//------------------------------------------------------------------------------
static void hdcprxxx_ctrl_set_abort_req(system_request req)
{
    if (hdcprxxx_ctrl_req_abort(&req, HDCPXX_CTRL_REQ_SET)) {
        return;
    }
}
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_authentication(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // check hdmirx_init()
    ret = system_ctrl_check_hdmirx_init(p_auth->port);
    CHECK_ERROR(ret);
    // Reset authentication process
    ret = hdmirx_hdcp_authentication_reset(p_auth);
    CHECK_ERROR(ret);
    // Set HDCP2Version
    ret = hdcprx22_set_hdcp2ver(p_auth);
    CHECK_ERROR(ret);
    // Set random seed
    ret = hdcprx22_set_random_seed(p_auth->port);
    CHECK_ERROR(ret);
    // Set Bcaps (REPEATER bit), when repeater
    ret = hdcprx14_set_bcaps_repeater(p_auth);
    CHECK_ERROR(ret);
    // Set RxCaps (REPEATER bit), when repeater
    ret = hdcprx22_set_rxcaps_repeater(p_auth);
    CHECK_ERROR(ret);
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
    // Set HDCP 1.4/2.2 interrupt request mask, when repeater
    ret = hdcprxxx_set_irq(p_auth);
    CHECK_ERROR(ret);
    // Set HDCP 2.2 interrupt request mask, when repeater
    ret = hdcprx22_set_irq(p_auth);
    CHECK_ERROR(ret);
    // Set HDCP 1.4/2.2 repeater info, when repeater
    ret = hdcprxxx_set_repeater_info(p_auth);
    CHECK_ERROR(ret);
#endif // Product Number having HDMI Rx & HDMI Tx
    // Decrypt HDCP 2.2 device key
    ret = hdcprx22_decrypt_device_key(p_auth);
    CHECK_ERROR(ret);
    // Decrypt HDCP 1.4 device key
    ret = hdcprx14_decrypt_device_key(p_auth);
    CHECK_ERROR(ret);
    // HDCP 1.4/2.2 On
    ret = hdcprxxx_set_hdcp_func(p_auth);
    CHECK_ERROR(ret);
    // Check old BCH error and clear. even if old BCH error, don't care.
    hdcprxxx_check_bch_error(p_auth->port);
    // Start HDCP 1.4 AKE, when not repeater
    ret = hdcprx14_start_hdcp_auth(p_auth);
    CHECK_ERROR(ret);
    // Start HDCP 2.2. AKE, when not repeater
    ret = hdcprx22_start_hdcp_auth(p_auth);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_get_authentication_status(hdmirx_hdcp_auth_status *p_status)
{
    uint8_t selected_port;
    hdmirx_hdcp_eess eess;
    uint32_t ret;

    if (p_status == NULL) {
        return ERR_NULL_POINTER;
    }
    CHECK_RX_ABORT();

    // Check BCH error
    ret = hdcprxxx_check_bch_error(p_status->port);
    switch (ret) {
        case ERR_OK:
            p_status->error = HDMIRX_HDCP_ERROR_NONE;
            break;
        case ERR_HDCPRXXX_PACKET_HEADER_BCH_ERROR:
            p_status->error = HDMIRX_HDCP_ERROR_PACKET_HEADER_BCH;
            break;
        case ERR_HDCPRXXX_PACKET_BODY_BCH_ERROR:
            p_status->error = HDMIRX_HDCP_ERROR_PACKET_BODY_BCH;
            break;
        default:
            break;
    }
    CHECK_ERROR(ret);
    // Get selected rx port
    ret = hdcprxxx_get_selected_rx_port(p_status->port, &selected_port);
    CHECK_ERROR(ret);
    // Check EESS
    ret = hdcprxxx_check_eess(p_status->port, selected_port, &eess);
    CHECK_ERROR(ret);
    p_status->eess = eess;
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2)
    // Check Authentication status, when repeater
    ret = hdcprxxx_check_auth_status(p_status, selected_port);
    CHECK_ERROR(ret);
#endif // Product Number having HDMI Rx & HDMI Tx

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_get_authentication_version(hdmirx_hdcp_auth_version *p_ver)
{
    uint8_t selected_port;
    hdmirx_hdcp_eess eess;
    uint8_t hdcp_stat;
    uint32_t ret;

    if (p_ver == NULL) {
        return ERR_NULL_POINTER;
    }
    // Get selected rx port
    ret = hdcprxxx_get_selected_rx_port(p_ver->port, &selected_port);
    CHECK_ERROR(ret);
    // Check EESS
    ret = hdcprxxx_check_eess(p_ver->port, selected_port, &eess);
    CHECK_ERROR(ret);
    // Select HDCP page: HDCP2.2 authentication register
    ret = hdcprxxx_select_page_of_0x14(p_ver->port, HDCPRXXX_PAGE_HDCP22_AUTH_REG);
    CHECK_ERROR(ret);

    // page 0x14: 12

    // Get HDCP Decryption Status
    ret = hdmirxxx_get_hdcp_decryption_status(p_ver->port, selected_port, &hdcp_stat);
    CHECK_ERROR(ret);
    // Set HDCP version
    if (eess == HDMIRX_HDCP_EESS_ENC_EN) {
        if (hdcp_stat == HDCPRXXX_H22STX_HDCP14) {
            p_ver->hdcp_version = HDMIRX_HDCP_VERSION_1_4;
        } else if ((hdcp_stat >= HDCPRXXX_H22STX_HDCP22_L) && (hdcp_stat <= HDCPRXXX_H22STX_HDCP22_H)) {
            p_ver->hdcp_version = HDMIRX_HDCP_VERSION_2_2;
        } else {
            p_ver->hdcp_version = HDMIRX_HDCP_VERSION_NON_COMPLIANT;
        }
    } else {
        p_ver->hdcp_version = HDMIRX_HDCP_VERSION_NON_COMPLIANT;
    }

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_authentication_reset(hdmirx_hdcp_auth *p_auth)
{
    uint32_t ret;

    if (p_auth == NULL) {
        return ERR_NULL_POINTER;
    }

    // Reset Port0
    ret = hdcprxxx_auth_reset_port(p_auth->port, SYSTEM_PORT_NUMBER_0, p_auth->port0);
    CHECK_ERROR(ret);
    // Reset Port1
    ret = hdcprxxx_auth_reset_port(p_auth->port, SYSTEM_PORT_NUMBER_1, p_auth->port1);
    CHECK_ERROR(ret);
    // Reset Port2
    ret = hdcprxxx_auth_reset_port(p_auth->port, SYSTEM_PORT_NUMBER_2, p_auth->port2);
    CHECK_ERROR(ret);
    // Reset Port3
    ret = hdcprxxx_auth_reset_port(p_auth->port, SYSTEM_PORT_NUMBER_3, p_auth->port3);
    CHECK_ERROR(ret);

    return ret;
}
//------------------------------------------------------------------------------
uint32_t hdmirx_hdcp_abort(void)
{
    // abort request
    hdcprxxx_ctrl_set_abort_req(SYSTEM_REQUEST_ABORT);

    return ERR_OK;
}
//------------------------------------------------------------------------------
