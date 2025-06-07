#ifndef _CHIP_IT66353_BASE_H_
#define _CHIP_IT66353_BASE_H_

#include "XbhType.h"

//-------------------- platform.h start ---------------------------
typedef unsigned long __tick;

//-------------------- chip_type.h start ---------------------------

#define BIT(X)  (1 << (X))

#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080

#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000

#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000

#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

enum
{
    RX_SWITCH_PORT,
    RX_HPD_HIGH,
    RX_HPD_LOW,
    CLK_STABLE,
    CLK_UNSTABLE,
    AEQ_TOGGLE_HPD,
    TXOE0,
    TXOE1,
};

typedef enum
{
    RX_TOGGLE_HPD,      // 0
    RX_PORT_CHANGE,     // 1
    TX_OUTPUT,          // 2
    TX_OUTPUT_PREPARE,  // 3
    RX_CHECK_EQ,        // 4
    SETUP_AFE,          // 5
    RX_WAIT_CLOCK,      // 6
    RX_HPD,             // 7
    TX_GOT_HPD,         // 8
    TX_WAIT_HPD,        // 9
    TX_UNPLUG,          // 10
    RX_UNPLUG,          // 11
    IDLE,
}_SYS_FSM_STATE;

static char *s__SYS_FSM_STATE[]=

{
    "RX_TOGGLE_HPD",
    "RX_PORT_CHANGE",
    "TX_OUTPUT",
    "TX_OUTPUT_PREPARE",
    "RX_CHECK_EQ",
    "SETUP_AFE",
    "RX_WAIT_CLOCK",
    "RX_HPD",
    "TX_GOT_HPD",
    "TX_WAIT_HPD",
    "TX_UNPLUG",
    "RX_UNPLUG",
    "IDLE",
};

enum
{
    HDMI_MODE_AUTO,
    HDMI_MODE_14,
    HDMI_MODE_20,
};

enum
{
    EQ_MODE_H14,
    EQ_MODE_H20,
};


typedef enum
{
    DEV_DEVICE_LOOP,
    DEV_DEVICE_INIT,
    DEV_WAIT_DEVICE_READY,
    DEV_FW_VAR_INIT,
    DEV_WAIT_RESET,
}_DEV_FSM_STATE;

typedef enum
{
    AEQ_OFF,
    AEQ_START,
    AEQ_CHECK_SAREQ_RESULT,
    AEQ_APPLY_SAREQ,
    AEQ_DONE,
    AEQ_FAIL,
    AEQ_MAX,
}_AEQ_FSM_STATE;

typedef enum
{
    EQRES_UNKNOWN,
    EQRES_BUSY,
    EQRES_SAREQ_DONE,
    EQRES_SAREQ_FAIL,
    EQRES_SAREQ_TIMEOUT,
    EQRES_H14EQ_DONE,
    EQRES_H14EQ_FAIL,
    EQRES_H14EQ_TIMEOUT,
    EQRES_DONE,
}_EQ_RESULT_TYPE;

typedef enum
{
    SysAEQ_OFF,
    SysAEQ_RUN,
    SysAEQ_DONE,
}_SYS_AEQ_TYPE;

enum
{
    EDID_SRC_EXT_SINK,
    EDID_SRC_INTERNAL,
};

enum
{
    TERM_LOW,
    TERM_HIGH,
    TERM_FOLLOW_TX,
    TERM_FOLLOW_HPD,
};


typedef struct
{
    XBH_U8 EnRxDDCBypass;
    XBH_U8 EnableAutoEQ;
}IT6635_DEVICE_OPTION;


typedef struct
{
    // TxSwap
    XBH_U8 EnTxPNSwap;
    XBH_U8 EnTxChSwap;
    XBH_U8 EnTxVCLKInv;
    XBH_U8 EnTxOutD1t;

    XBH_U8 EnRxDDCBypass;
    XBH_U8 EnRxPWR5VBypass;
    XBH_U8 EnRxHPDBypass;

    XBH_U8 EnCEC;

    XBH_U8 EnableAutoEQ;
    XBH_U8 ParseEDIDFromSink;
    XBH_U8 NonActivePortReplyHPD;
    XBH_U8 DisableEdidRam;
    XBH_U8 TryFixedEQFirst;
    XBH_U8 TurnOffTx5VWhenSwitchPort;
    XBH_U8 FixIncorrectHdmiEnc;

}IT6635_DEVICE_OPTION_INT;

typedef struct
{
    XBH_U8 tag1;
    XBH_U8 EnRxDDCBypass;
    XBH_U8 EnRxPWR5VBypass;
    XBH_U8 EnRxHPDBypass;
    XBH_U8 TryFixedEQFirst;
    XBH_U8 EnableAutoEQ;
    XBH_U8 NonActivePortReplyHPD;
    XBH_U8 DisableEdidRam;
    XBH_U8 DefaultEQ[3];
    XBH_U8 FixIncorrectHdmiEnc;
    XBH_U8 HPDOutputInverse;
    XBH_U8 HPDTogglePeriod;
    XBH_U8 TxOEAlignment;
    XBH_U8 str_size;

}IT6635_RX_OPTIONS;

typedef struct
{
    XBH_U8 tag1;
    // TxSwap
    XBH_U8 EnTxPNSwap;
    XBH_U8 EnTxChSwap;
    XBH_U8 EnTxVCLKInv;
    XBH_U8 EnTxOutD1t;
    XBH_U8 CopyEDIDFromSink;
    XBH_U8 ParsePhysicalAddr;
    XBH_U8 TurnOffTx5VWhenSwitchPort;
    XBH_U8 str_size;

}IT6635_TX_OPTIONS;

typedef struct
{
    XBH_U8 tag1;
    XBH_U32 DevNo;
    XBH_U8 SwAddr;
    XBH_U8 RxAddr;
    XBH_U8 CecAddr;
    XBH_U8 EdidAddr;
    XBH_U8 ForceRxOn;
    XBH_U8 RxAutoPowerDown;
    XBH_U8 DoTxPowerDown;
    XBH_U8 TxPowerDownWhileWaitingClock;
    XBH_U8 str_size;

}IT6635_DEV_OPTION;

typedef struct
{
    IT6635_RX_OPTIONS *active_rx_opt;
    IT6635_RX_OPTIONS *rx_opt[4];
    IT6635_TX_OPTIONS *tx_opt;
    IT6635_DEV_OPTION *dev_opt;

}IT6635_DEV_OPTION_INTERNAL;

typedef struct
{
    struct
    {
        XBH_U8 Rev;
        XBH_U32 RCLK;
        XBH_U8 RxHPDFlag[4];

        XBH_U8 VSDBOffset;  //0xFF;

        XBH_U8 PhyAdr[4];
        XBH_U8 EdidChkSum[2];

        _SYS_FSM_STATE state_sys_fsm;
        XBH_U8 state_dev_init;
        XBH_U8 count_wait_clock_none_edid;
        XBH_U8 state_dev;
        XBH_U8 fsm_return;
        XBH_U8 Rx_active_port;
        XBH_U8 Rx_new_port;
        XBH_U8 Tx_current_5v;
        XBH_U32 vclk;
        XBH_U32 vclk_prev;

        XBH_U16 RxCEDErr[3];
        XBH_U8 RxCEDErrValid;
        XBH_U16 RxCEDErrRec[3][3];

        XBH_U8 count_unlock;
        XBH_U8 count_symlock;
        XBH_U8 count_symlock_lost;
        XBH_U8 count_symlock_fail;
        XBH_U8 count_symlock_unstable;
        XBH_U8 count_fsm_err;
        XBH_U8 count_eq_check;
        XBH_U8 count_try_force_hdmi_mode;
        XBH_U8 count_auto_eq_fail;
        XBH_U8 count_wait_clock;
        XBH_U8 clock_ratio;
        XBH_U8 h2_scramble;
        XBH_U8 edid_ready;
        XBH_U8 prev_hpd_state;

        XBH_U8 try_fixed_EQ;
        XBH_U8 current_hdmi_mode;
        XBH_U8 current_txoe;
        XBH_U8 check_for_hpd_toggle;
        XBH_U8 sdi_stable_count;
        XBH_U8 check_for_sdi;
        XBH_U8 force_hpd_state;
        //XBH_U8 txoe_alignment;
        XBH_U8 hpd_toggle_timeout;
        XBH_U8 spmon;

        __tick tick_set_afe;
        __tick tick_hdcp;
        //XBH_U8 en_count_hdcp;
        XBH_U8 *default_edid[4];

        // tx
        XBH_U8 hpd_wait_count;
        XBH_U8 is_hdmi20_sink;
        XBH_U8 rx_deskew_err;
    }vars;

    struct
    {
        _SYS_AEQ_TYPE sys_aEQ;
        XBH_U8 AutoEQ_state;
        XBH_U8 AutoEQ_WaitTime;
        XBH_U8 AutoEQ_Result;
        XBH_U8 DFE_Valid;
        XBH_U8 RS_Valid;
        XBH_U16 RS_ValidMap[3];
        XBH_U8 EqHDMIMode;
        XBH_U8 ManuEQ_state;
        XBH_U8 DFE[14][3][3]; // [RS_value][channel012][NumABC]  -> 0x34B...0x353
        XBH_U8 CalcRS[3];

        XBH_U8 EQ_flag_14;
        XBH_U8 EQ_flag_20;
        XBH_U8 txoe_ready14;
        XBH_U8 txoe_ready20;
        XBH_U8 stored_RS_14[3];
        XBH_U8 stored_RS_20[3];
        XBH_U8 current_eq_mode;

        //XBH_U8 FixedRsIndex[4];

        XBH_U8 meq_cur_idx;

        XBH_U8 meq_adj_idx[3];
        XBH_U32 ced_err_avg[3];
        XBH_U32 ced_err_avg_prev[3];
        XBH_U8  ced_acc_count;
        XBH_U8  manu_eq_fine_tune_count[3];
        XBH_U8  manu_eq_fine_tune_best_rs[3];

    }EQ;

    //XBH_U8 edid_buf[128];

    IT6635_DEV_OPTION_INTERNAL opts;

}IT6635_DEVICE_DATA;


#endif //_CHIP_IT66353_BASE_H_
