#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipIt66353_InputSource"

#include "XbhLog.h"

#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include "XbhService.h"
#include "ChipIt66353.h"
#include <hardware/board.h>
//-------------------- chip_type.h end ---------------------------


#define CONST const


//
//  assign the print function
//
#define pr_err    printf
#define pr_info   printf
#define pr_info2  printf


//---------- for CEC

#define iTE_FALSE 0
#define iTE_TRUE  1


typedef XBH_U8 iTE_u1 ;

// output TXOE state on JP47 (GPC5)
// by nVidia's clock detect request
#define REPORT_TXOE_0(x) { GPDRC &= ~0x20;} //GPC5=0;
#define REPORT_TXOE_1(x) { GPDRC |= 0x20; } //GPC5=1;

typedef char iTE_s8, *iTE_ps8;
typedef XBH_U8 iTE_u8, *iTE_pu8;
typedef short iTE_s16, *iTE_ps16;
typedef unsigned short iTE_u16, *iTE_pu16;
//-------------------- platform.h end ---------------------------


//-------------------- config.h start ---------------------------
#define _SHOW_PRAGMA_MSG
#ifndef _SHOW_PRAGMA_MSG
#define message(ignore)
#endif


//**********************
// Internal compile options:

#define USING_WDOG 0

// 66353 and 66354 don't need this:
// set to 0
#define CHECK_INT_BEFORE_TXOE 0

//
// EN_AUTO_RS: ( compile option )
// 1: Enable Auto EQ code
// 0: Disable Auto EQ code
#define EN_H14_SKEW 0

//
// EN_AUTO_RS: ( compile option )
// 1: Enable Auto EQ code
// 0: Disable Auto EQ code
#define EN_AUTO_RS 1

//
// EN_CEC:
// 1: Enable CEC function
// 0: Disable CEC function
#define EN_CEC 0

//
// FIX_EDID_FOR_ATC_4BLOCK_CTS:
// 1: For ATC 4 blocks EDID test
//
#define FIX_EDID_FOR_ATC_4BLOCK_CTS 1
//-------------------- config.h end ---------------------------

//-------------------- debug.h start ---------------------------

#define PR_INFO(x) XLOGI x
#define PR_INFO2(x) XLOGD x
#define PR_ERR(x) XLOGE x
#define PR_AEQ(x) {XLOGD x ;}


#define DBG_TXOE_1(x) {  }
#define DBG_TXOE_0(x) {  }

#define DBG_CLKSTABLE_0(x) {  }
#define DBG_CLKSTABLE_1(x) {  }

#define DBG_SYMLOCK_0(x) {  }
#define DBG_SYMLOCK_1(x) {  }

#define _rx_edid_address_enable(port) { h2swset(0x55 + port, 0x24, 0x20); }
#define _rx_edid_address_disable(port) { h2swset(0x55 + port, 0x24, 0x04); }
#define _rx_edid_ram_enable(port) { if ( gdev.opts.rx_opt[port]->EnRxDDCBypass == 0 ) { h2swset(0x55 + port, 0x01, 0x00); } }
#define _rx_edid_ram_disable(port) { h2swset(0x55 + port, 0x01, 0x01); }
#define _rx_edid_set_chksum(port, sum) { h2swwr(0xe1 + port * 2, sum);  }
#define _rx_edid_set_cec_phyaddr(port, phyAB, phyCD) { h2swwr(0xd9 + port*2, phyAB); h2swwr(0xda + port*2, phyCD);  }

#define DBG_TM(n) //{ __debug_set_io(n); }
// int set_port( int portnum, int wrmask, int wrdata );
// void __debug_set_io(XBH_U8 n);
//-------------------- debug.h end ---------------------------


//-------------------- it6635.h start ---------------------------
#define SW_HPD_LOW 0
#define SW_HPD_AUTO 1

//-------------------- it6635.h end ---------------------------

static int mResetPort = -1;
static int mDDCErrorCount = 0;
static int mChipWaitClockErrorAndReset = -1;

//-------------------- it6635_drv.h start ---------------------------
//#define RX_PORT_COUNT 4

#define RX_PORT_0              0
#define RX_PORT_1              1
#define RX_PORT_2              2
#define RX_PORT_3              3

#define TRUE            1
#define FALSE           0

#define DDCWAITTIME     5
#define DDCWAITNUM      10

#define RX_PORT_COUNT 4

// for _rx_term_power_down
#define CH0_OFF (0x10)
#define CH1_OFF (0x20)
#define CH2_OFF (0x40)
#define CLK_OFF (0x80)
#define ALLCH_OFF (0xF0)
#define ALLCH_ON (0x00)

//===================================================
// config:
//===================================================

// RCLKFreqSel => 0: 20MHz, 1: 10MHz, 2: 5MHz, 3: 2.5MHz
#define RCLKFreqSel 0



#define EDID_PORT_0 0x01
#define EDID_PORT_1 0x02
#define EDID_PORT_2 0x04
#define EDID_PORT_3 0x08
#define EDID_PORT_ALL (EDID_PORT_0|EDID_PORT_1|EDID_PORT_2|EDID_PORT_3)

//
// for it6635_get_port_info0()
//
#define PI_5V ( BIT0 )
#define PI_HDMI_MODE ( BIT1 )
#define PI_CLK_DET ( BIT2 )
#define PI_CLK_VALID ( BIT3 )
#define PI_CLK_STABLE ( BIT4 )
#define PI_PLL_LOCK ( BIT5 )
//#define PI_XX ( BIT6 )
#define PI_SYM_LOCK ( BIT7 )

//
// for it6635_get_port_info1()
//
#define PI_PLL_HS1G (BIT0)

//-------------------- it6635_drv.h end ---------------------------


//-------------------- it6635_opts.cpp start ---------------------------

//
// TX_PN_SWAP
// 1: Enable TX side TMDS P/N swap
// 0: Disable TX side TMDS P/N swap
#define TX_PN_SWAP 0

//
// DEFAULT_RS_IDX
// :The default EQ when power on.
//
#define DEFAULT_RS_IDX 4


// constant definition
#define HPD_TOGGLE_TIMEOUT_400MS (27)
#define HPD_TOGGLE_TIMEOUT_500MS (50) // (50) 改 (70)
#define HPD_TOGGLE_TIMEOUT_1SEC (100)
#define HPD_TOGGLE_TIMEOUT_2SEC (20|(BIT7))
#define HPD_TOGGLE_TIMEOUT_3SEC (30|(BIT7))

static XBH_U8 s_default_edid_port0[]=
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
    0x26, 0x85, 0x35, 0x66, 0x01, 0x01, 0x01, 0x01,
    0x00, 0x19, 0x01, 0x03, 0x80, 0x80, 0x48, 0x78,
    0x0A, 0xDA, 0xFF, 0xA3, 0x58, 0x4A, 0xA2, 0x29,
    0x17, 0x49, 0x4B, 0x20, 0x08, 0x00, 0x31, 0x40,
    0x61, 0x40, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x08, 0xE8,
    0x00, 0x30, 0xF2, 0x70, 0x5A, 0x80, 0xB0, 0x58,
    0x8A, 0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E,
    0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40,
    0x58, 0x2C, 0x45, 0x00, 0xBA, 0x88, 0x21, 0x00,
    0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x50,
    0x61, 0x6E, 0x61, 0x73, 0x6F, 0x6E, 0x69, 0x63,
    0x2D, 0x54, 0x56, 0x0A, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x17, 0x3D, 0x0F, 0x88, 0x3C, 0x00, 0x0A,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xF0,

    0x02, 0x03, 0x43, 0xF0, 0x57, 0x10, 0x1F, 0x05,
    0x14, 0x20, 0x21, 0x22, 0x04, 0x13, 0x03, 0x12,
    0x07, 0x16, 0x5D, 0x5E, 0x5F, 0x62, 0x63, 0x64,
    0x61, 0x60, 0x66, 0x65, 0x23, 0x09, 0x07, 0x01,
    0x6E, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x38, 0x3C,
    0x2F, 0x00, 0x80, 0x01, 0x02, 0x03, 0x04, 0x67,
    0xD8, 0x5D, 0xC4, 0x01, 0x78, 0x80, 0x03, 0xE2,
    0x00, 0x4B, 0xE3, 0x05, 0x1F, 0x01, 0xE4, 0x0F,
    0x00, 0x00, 0x78, 0x56, 0x5E, 0x00, 0xA0, 0xA0,
    0xA0, 0x29, 0x50, 0x30, 0x20, 0x35, 0x00, 0xBA,
    0x88, 0x21, 0x00, 0x00, 0x1A, 0x66, 0x21, 0x56,
    0xAA, 0x51, 0x00, 0x1E, 0x30, 0x46, 0x8F, 0x33,
    0x00, 0xBA, 0x88, 0x21, 0x00, 0x00, 0x1E, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
};

void ChipIt66353::it6635_options_init()
{
    gdev.opts.rx_opt[0] = &s_RxOpts;
    gdev.opts.rx_opt[1] = &s_RxOpts;
    gdev.opts.rx_opt[2] = &s_RxOpts;
    gdev.opts.rx_opt[3] = &s_RxOpts;
    gdev.opts.active_rx_opt = gdev.opts.rx_opt[mDefaultPort];
    gdev.opts.tx_opt = &s_TxOpts;
    gdev.opts.dev_opt = &s_DevOpts;
    gdev.vars.Rx_active_port = mDefaultPort;
    gdev.vars.Rx_new_port = mDefaultPort;
    if(mResetPort != -1)
    {
        gdev.vars.Rx_active_port = mResetPort;
        gdev.vars.Rx_new_port = mResetPort;
        mResetPort = -1;
    }
    mChipWaitClockErrorAndReset = 2;
    it6635_vars_init();
}

//-------------------- it6635_opts.cpp end ---------------------------

#ifdef XBH_ITE66353_HDMI_PORT_0_EDID_ADDR
#define HDMI_PORT_0_EDID_ADDR       XBH_ITE66353_HDMI_PORT_0_EDID_ADDR
#else
#define HDMI_PORT_0_EDID_ADDR       1
#endif
#ifdef XBH_ITE66353_HDMI_PORT_1_EDID_ADDR
#define HDMI_PORT_1_EDID_ADDR       XBH_ITE66353_HDMI_PORT_1_EDID_ADDR
#else
#define HDMI_PORT_1_EDID_ADDR       4
#endif
#ifdef XBH_ITE66353_HDMI_PORT_2_EDID_ADDR
#define HDMI_PORT_2_EDID_ADDR       XBH_ITE66353_HDMI_PORT_2_EDID_ADDR
#else
#define HDMI_PORT_2_EDID_ADDR       5
#endif
#ifdef XBH_ITE66353_HDMI_PORT_3_EDID_ADDR
#define HDMI_PORT_3_EDID_ADDR       XBH_ITE66353_HDMI_PORT_3_EDID_ADDR
#else
#define HDMI_PORT_3_EDID_ADDR       15
#endif


//-------------------- it6635_EQ.cpp start ---------------------------
#define Channel_B 0
#define Channel_G 1
#define Channel_R 2

//............................0     1     2     3     4     5     6     7     8     9     10    11    12    13
const static XBH_U8 RS_Value[] =  { 0x7F, 0x7E, 0x3F, 0x3E, 0x1F, 0x1E, 0x0F, 0x0E, 0x07, 0x06, 0x03, 0x02, 0x01, 0x00 };
static XBH_U8 rs_try_idx[] = {7, 6, 8, 5, 9, 4, 10, 3, 11, 2, 12, 1, 13, 0};
static XBH_U8 err_acc=0;
static XBH_U8 idx[3]={0};
static XBH_U8 ch_rs_idx[3];
static XBH_U8 read_from_scdc = 0;
static XBH_U8 aeq_retry = 0;
static XBH_U8 prep_count = 0;
static XBH_U8 prep_fail_count = 0;
static XBH_U8 readBuff[128] = {0};
static XBH_U8 mInitStatus = 0;

XBH_U8 ChipIt66353::_rx_is_h14_tmds_over_1G(void)
{
    return it6635_get_port_info1( gdev.vars.Rx_active_port, PI_PLL_HS1G );
}

void ChipIt66353::_rx_DFE_enable( XBH_U8 enable )
{
    chgrxbank(3);
    if ( enable )
    {
        h2rxset(0x22, 0x40, 0x40); // Enable DFE
    }
    else
    {
        h2rxset(0x22, 0x40, 0x00); // Disable DFE
    }
    chgrxbank(0);
}

void ChipIt66353::_rx_set_rs_3ch( XBH_U8 *rs_value )
{
    XBH_U8 rs[3];

    if ( rs_value )
    {
        rs[0] = rs_value[0]|0x80;
        rs[1] = rs_value[1]|0x80;
        rs[2] = rs_value[2]|0x80;
        chgrxbank(3);
        h2rxbwr(0x27, 3, rs);
        rs[0]=0xCC;
        rs[1]=0xCC;
        rs[2]=0xCC;
        h2rxbrd(0x27, 3, rs);
        chgrxbank(0);

        PR_INFO(("***RS set to %02x %02x %02x\r\n", rs[0], rs[1], rs[2]));
    }
}

void ChipIt66353::_rx_set_rs( XBH_U8 ch, XBH_U8 rs_value )
{
    rs_value|=0x80;
    chgrxbank(3);
    h2rxwr(0x27+ch, rs_value);
    chgrxbank(0);

    PR_INFO(("*RS set to %d:%02x\r\n", ch, rs_value));
}

XBH_U8 ChipIt66353::_rx_get_rs( XBH_U8 ch )
{
    XBH_U8 rs_value;
    chgrxbank(3);
    rs_value = h2rxrd(0x27+ch);
    chgrxbank(0);

    return rs_value;
}

void ChipIt66353::_rx_clear_ced_err(void)
{
    h2rxset(0x3B, 0x08, 0x08);  // read CED Error from SAREQ CEDError Counter

    chgrxbank(3);
    h2rxset(0x55, 0x80, 0x00);
    h2rxwr(0xe9, 0x80);
    chgrxbank(0);
}

void ChipIt66353::_rx_ced_err_recheck( void )
{
    XBH_U8 i;
    XBH_U8 tmp;

    h2rxwr(0xB9, 0xFF);
    h2rxwr(0xBE, 0xFF);
    delay1ms(10);

    for( i=0 ; i<3 ; i++ )
    {
        if ( (gdev.vars.RxCEDErrValid&(1<<i)) && (gdev.vars.RxCEDErr[i] > 0x100) )
        {
            if ( (h2rxrd(0x19)&0x80 ) ) // SCDT == 1
            {
                tmp = h2rxrd(0xB9);
                if ( 0 == (tmp & (0x03<<i)) )
                {
                    gdev.vars.RxCEDErr[i] = 0;
                }
            }
        }
    }
}

void ChipIt66353::_rx_update_ced_err_from_hw(void)
{
    XBH_U8 symlock;

    if ( _rx_is_clock_stable() )
    {
        symlock = h2rxrd(0x14);

        if ( 0 == (symlock & 0x38) )
        {
            gdev.vars.RxCEDErrValid=0;
        }
        else
        {
            h2rxset(0x3B, 0x08, 0x08);  // read CED Error from SAREQ CEDError Counter
            chgrxbank(3);
            h2rxset(0x55, 0x80, 0x00);

            if ( symlock & 0x08 )
            {
                h2rxwr(0xe9, 0x00);
                gdev.vars.RxCEDErr[0] = h2rxrd(0xEB);

                if ( gdev.vars.RxCEDErr[0] & 0x80 ) // ced valid
                {
                    gdev.vars.RxCEDErr[0] = gdev.vars.RxCEDErr[0] & 0x7F;
                    gdev.vars.RxCEDErr[0] = (gdev.vars.RxCEDErr[0]<<8) + h2rxrd(0xEA);
                    gdev.vars.RxCEDErrValid=1;
                }
                else
                {
                    gdev.vars.RxCEDErrValid=0;
                }
            }
            else
            {
                gdev.vars.RxCEDErrValid=0;
            }

            if ( symlock & 0x10 )
            {
                h2rxwr(0xe9, 0x20);
                gdev.vars.RxCEDErr[1] = h2rxrd(0xEB);

                if ( gdev.vars.RxCEDErr[1] & 0x80 )
                {
                    gdev.vars.RxCEDErr[1] = gdev.vars.RxCEDErr[1] & 0x7F;
                    gdev.vars.RxCEDErr[1] = (gdev.vars.RxCEDErr[1]<<8) + h2rxrd(0xEA);
                    gdev.vars.RxCEDErrValid |= 0x02;
                }
                else
                {
                    gdev.vars.RxCEDErrValid &= ~0x02;
                }
            }
            else
            {
                gdev.vars.RxCEDErrValid &= ~0x02;
            }

            if ( symlock & 0x20 )
            {
                h2rxwr(0xe9, 0x40);
                gdev.vars.RxCEDErr[2] = h2rxrd(0xEB);

                if ( gdev.vars.RxCEDErr[2] & 0x80 )
                {
                    gdev.vars.RxCEDErr[2] = gdev.vars.RxCEDErr[2] & 0x7F;
                    gdev.vars.RxCEDErr[2] = (gdev.vars.RxCEDErr[2]<<8) + h2rxrd(0xEA);
                    gdev.vars.RxCEDErrValid |= 0x04;
                }
                else
                {
                    gdev.vars.RxCEDErrValid &= ~0x04;
                }
            }
            else
            {
                gdev.vars.RxCEDErrValid &= ~0x04;
            }

            h2rxwr(0xe9, 0x80);

            chgrxbank(0);

            _rx_ced_err_recheck();

        }
    }
    else
    {
        gdev.vars.RxCEDErrValid=0;
    }
}

void ChipIt66353::_rx_record_ced_err(void)
{
    XBH_U8 i;

    if ( gdev.EQ.ced_acc_count == 0 )
    {
        for (i=0 ; i<3 ; i++)
        {
            gdev.EQ.ced_err_avg[i] = gdev.vars.RxCEDErr[i];
            //gdev.EQ.ced_err_avg_prev[i] = 0x8888;
        }
    }
    else
    {
        for (i=0 ; i<3 ; i++)
        {
            gdev.EQ.ced_err_avg[i] = gdev.EQ.ced_err_avg[i] + gdev.vars.RxCEDErr[i];
            gdev.EQ.ced_err_avg[i]>>=1;
        }
    }
    gdev.EQ.ced_acc_count++;
}

XBH_U8 ChipIt66353::_rx_monitor_ced_err(void)
{

    if ( 0 == _rx_is_all_ch_symlock() )
    {
        return 0;
    }

    _rx_record_ced_err();


    if ( gdev.vars.RxCEDErrValid )
    {
        if ( (gdev.vars.RxCEDErr[0] || gdev.vars.RxCEDErr[1] || gdev.vars.RxCEDErr[2]) )
        {
            PR_INFO(("CED=0x%04X, 0x%04X, 0x%04X\r\n", gdev.vars.RxCEDErr[0], gdev.vars.RxCEDErr[1], gdev.vars.RxCEDErr[2]));

            err_acc++;

            if (err_acc > 10)
            {
                err_acc = 0;
                if ( (gdev.vars.RxCEDErr[0]>0x08 || gdev.vars.RxCEDErr[1]>0x08 || gdev.vars.RxCEDErr[2]>0x08) )
                {
                    // return 1, means too much CED
                    return 1;
                }
            }

        }
        else
        {
            err_acc=0;
        }
    }
    else
    {
        // no ced error value for reference
        // do nothing about CED error
    }

    return 0;
}

XBH_U8 ChipIt66353::_rx_is_hdmi20(void)
{
    #if 1
    return gdev.vars.clock_ratio;
    #else
    if( h2swrd(0x61+ gdev.vars.Rx_active_port *3)&0x40 )
    {
        return 1;
    }
    return 0;
    #endif
}

#if EN_H14_SKEW
void _rx_skew_adj(XBH_U8 ch)
{
    chgrxbank(3);
    h2rxset(0x2D,0x1<<ch, 0x01<<ch);
    switch ( idx[ch] )
    {
        case 0:
            h2rxset(0x2C,0x3<<ch, 0x02<<ch);
            idx[ch] = 1;
            break;
        case 1:
            h2rxset(0x2C,0x3<<ch, 0x03<<ch);
            idx[ch] = 2;
            break;
        case 2:
            h2rxset(0x2C,0x3<<ch, 0x00<<ch);
            idx[ch] = 0;
            break;
        default:
            idx[ch] = 0;
            break;
    }
    chgrxbank(0);

}
#endif

void ChipIt66353::_eq_save_h20(void)
{
    chgrxbank(3);
    gdev.EQ.stored_RS_20[0] = h2rxrd(0x27);
    gdev.EQ.stored_RS_20[1] = h2rxrd(0x28);
    gdev.EQ.stored_RS_20[2] = h2rxrd(0x29);
    chgrxbank(0);
}

void ChipIt66353::_eq_load_h20(void)
{
    PR_INFO(("_eq_load_h20\r\n"));
    _rx_set_rs_3ch( gdev.EQ.stored_RS_20 );
    _aeq_set_DFE2( gdev.EQ.stored_RS_20[0], gdev.EQ.stored_RS_20[1], gdev.EQ.stored_RS_20[2] );
    _rx_DFE_enable(1);
}

void ChipIt66353::_eq_save_h14(void)
{
    chgrxbank(3);
    gdev.EQ.stored_RS_14[0] = h2rxrd(0x27);
    gdev.EQ.stored_RS_14[1] = h2rxrd(0x28);
    gdev.EQ.stored_RS_14[2] = h2rxrd(0x29);
    chgrxbank(0);
}

void ChipIt66353::_eq_load_h14(void)
{
    PR_INFO(("_eq_load_h14\r\n"));
    _rx_set_rs_3ch( gdev.EQ.stored_RS_14 );
}

void ChipIt66353::_eq_load_previous(void)
{
    if ( _rx_is_hdmi20() )
    {
        if (gdev.EQ.current_eq_mode == 0) {
            gdev.EQ.current_eq_mode = 1;
            _eq_load_h20();
        }
    }
    else
    {
        if (gdev.EQ.current_eq_mode == 1) {
            gdev.EQ.current_eq_mode = 0;
            _eq_load_h14();
        }
    }
}

void ChipIt66353::_eq_load_default(void)
{
    _rx_set_rs_3ch( gdev.opts.active_rx_opt->DefaultEQ );

    if ( _rx_is_hdmi20() )
    {
        _rx_DFE_enable(0);
        PR_INFO(("_eq_load_def_20\r\n"));
    }
    else
    {
        PR_INFO(("_eq_load_def_14\r\n"));
    }
}

XBH_U8 ChipIt66353::_eq_find_rs_index(XBH_U8 EQ_Value)
{
    XBH_U8 i = 0;
    EQ_Value &= ~0x80;
    for(i = 0; i < sizeof(RS_Value); i++)
    {
        if(RS_Value[i] == EQ_Value) return i;
    }

    PR_ERR(("EQ %02X not found !!\r\n", EQ_Value));
    return 4;
}

void ChipIt66353::_eq_reset_txoe_ready(void)
{
    gdev.EQ.txoe_ready20 = 0;
    gdev.EQ.txoe_ready14 = 0;
}

void ChipIt66353::_eq_set_txoe_ready(XBH_U8 ready)
{
    if ( _rx_is_hdmi20() )
    {
        gdev.EQ.txoe_ready20 = ready;
    }
    else
    {
        gdev.EQ.txoe_ready14 = ready;
    }

    PR_INFO(("set ready 14=%d 20=%d\r\n", gdev.EQ.txoe_ready14, gdev.EQ.txoe_ready20));
}

XBH_U8 ChipIt66353::_eq_get_txoe_ready(void)
{
    PR_INFO(("get ready 14=%d 20=%d\r\n", gdev.EQ.txoe_ready14, gdev.EQ.txoe_ready20));
    if ( _rx_is_hdmi20() )
    {
        return gdev.EQ.txoe_ready20;
    }
    else
    {
        return gdev.EQ.txoe_ready14;
    }
}

void ChipIt66353::_eq_reset_state(void)
{
    gdev.EQ.EQ_flag_20 = SysAEQ_RUN;
    gdev.EQ.EQ_flag_14 = SysAEQ_RUN;
}

void ChipIt66353::_eq_set_state(XBH_U8 state)
{
    if ( _rx_is_hdmi20() )
    {
        gdev.EQ.EQ_flag_20 = state;
    }
    else
    {
        gdev.EQ.EQ_flag_14 = state;
    }

    PR_INFO(("set eq flag 14=%d 20=%d\r\n", gdev.EQ.EQ_flag_14, gdev.EQ.EQ_flag_20));
}

XBH_U8 ChipIt66353::_eq_get_state(void)
{
    PR_INFO(("get eq flag 14=%d 20=%d\r\n", gdev.EQ.EQ_flag_14, gdev.EQ.EQ_flag_20));

    if ( _rx_is_hdmi20() )
    {
        return gdev.EQ.EQ_flag_20;
    }
    else
    {
        return gdev.EQ.EQ_flag_14;
    }
}

XBH_U8 ChipIt66353::_aeq_ready_to_start(void)
{
    return _rx_is_clock_stable();
}

void ChipIt66353::_aeq_diable_eq_trigger(void)
{
    chgrxbank(3);
    h2rxset(0x22, 0x04, 0x00);
    chgrxbank(0);
}

void ChipIt66353::_aeq_reset(void)
{
    _aeq_diable_eq_trigger();

    h2rxwr(0x07, 0xff);
    h2rxwr(0x23, 0xB0);
    delay1ms(10);
    h2rxwr(0x23, 0xA0);
}

void ChipIt66353::_aeq_get_DFE2(XBH_U8 ch, XBH_U8 rs_idx)
{
    XBH_U8 i;
    XBH_U8 amp_a, amp_b, amp_c, amp_d;
    XBH_U8 tap1_sign, tap1_value;
    XBH_U8 tap2_sign, tap2_value;
    XBH_U8 tap3_sign, tap3_value;
    XBH_U8 dfe_a, dfe_b, dfe_c;
    XBH_U8 reg37o, reg37;
    XBH_U8 rec[4];

    chgrxbank(3);

    i = rs_idx;

    h2rxset(0x36, 0x0F, i);
    reg37o=h2rxrd(0x37)&(~0xC0);
    PR_AEQ(("RS=%02x DEF:\r\n", 0x80|RS_Value[i]));

    for( ch=0 ; ch<3 ; ch++ )
    {
        //hdmirxset(0x37 ,0xC0 ,ch<<6);
        reg37 = reg37o|(ch<<6);
        h2rxwr(0x37, reg37);

        #if 0
        amp_a = hdmirxrd(0x5d);
        amp_b = hdmirxrd(0x5e);
        amp_c = hdmirxrd(0x5f);
        amp_d = hdmirxrd(0x60);
        #else
        h2rxbrd(0x5d, 4, rec);
        amp_a = rec[0];
        amp_b = rec[1];
        amp_c = rec[2];
        amp_d = rec[3];
        #endif

        if (amp_a > amp_b)
        {
            tap1_sign = 1;
            tap1_value = (amp_a - amp_b) >> 1;
        }
        else
        {
            tap1_sign = 0;
            tap1_value = (amp_b - amp_a) >> 1;
        }

        if (amp_a > amp_c)
        {
            tap2_sign = 1;
            tap2_value = (amp_a - amp_c) >> 1;
        }
        else
        {
            tap2_sign = 0;
            tap2_value = (amp_c - amp_a) >> 1;
        }

        if (amp_a > amp_d)
        {
            tap3_sign = 1;
            tap3_value = (amp_a - amp_d) >> 1;
        }
        else
        {
            tap3_sign = 0;
            tap3_value = (amp_d - amp_a) >> 1;
        }

        if(tap1_value > 0x1F) tap1_value = 0x1F;
        if(tap2_value > 0x0F) tap2_value = 0x0F;
        if(tap3_value > 0x07) tap3_value = 0x07;

        dfe_a = BIT6 + (tap1_sign << 5) + tap1_value;
        dfe_b = BIT5 + (tap2_sign << 4) + tap2_value;
        dfe_c = BIT4 + (tap3_sign << 3) + tap3_value;

        gdev.EQ.DFE[i][ch][0] = dfe_a;
        gdev.EQ.DFE[i][ch][1] = dfe_b;
        gdev.EQ.DFE[i][ch][2] = dfe_c;

        PR_AEQ((" ch%d=%02X %02X %02X |",ch, dfe_a, dfe_b, dfe_c));
    }
    PR_AEQ(("\r\n"));

    chgrxbank(0);

}

void ChipIt66353::_aeq_set_DFE2(XBH_U8 EQ0, XBH_U8 EQ1, XBH_U8 EQ2)
{
    XBH_U8 dfe[9];
    XBH_U8 i;

    i = _eq_find_rs_index(EQ0);
    _aeq_get_DFE2(0, i);
    dfe[0] = gdev.EQ.DFE[i][Channel_B][0];
    dfe[1] = gdev.EQ.DFE[i][Channel_B][1];
    dfe[2] = gdev.EQ.DFE[i][Channel_B][2];

    i = _eq_find_rs_index(EQ1);
    _aeq_get_DFE2(1, i);
    dfe[3] = gdev.EQ.DFE[i][Channel_G][0];
    dfe[4] = gdev.EQ.DFE[i][Channel_G][1];
    dfe[5] = gdev.EQ.DFE[i][Channel_G][2];

    i = _eq_find_rs_index(EQ2);
    _aeq_get_DFE2(2, i);
    dfe[6] = gdev.EQ.DFE[i][Channel_R][0];
    dfe[7] = gdev.EQ.DFE[i][Channel_R][1];
    dfe[8] = gdev.EQ.DFE[i][Channel_R][2];

    chgrxbank(3);
    h2rxbwr(0x4B, 9, dfe);
    //h2rxbrd(0x4B, 9, dfe); // for recheck only
    for (i=0; i<3 ;i++)
    {
        PR_AEQ(("DFE:CH%d=%02X %02X %02X\r\n", i, dfe[0+i*3], dfe[1+i*3], dfe[2+i*3]));
    }
    chgrxbank(0);
}

void ChipIt66353::_aeq_get_calc_rs(void)
{
    chgrxbank(3);
    h2rxbrd( 0xD5, 3, gdev.EQ.CalcRS );
    chgrxbank(0);
    PR_AEQ(("AEQ RS=%02X %02X %02X\r\n", gdev.EQ.CalcRS[0], gdev.EQ.CalcRS[1], gdev.EQ.CalcRS[2] ));
}

void ChipIt66353::_aeq_trigger_EQ(void)
{
    _aeq_reset();

    chgrxbank(3);

    // if 1G~3G, need to force set CSEL to 110
    if( _rx_is_h14_tmds_over_1G() )
    {
        PR_AEQ(("over 1G\r\n"));
        h2rxwr(0x20, 0x36);
        h2rxwr(0x21, 0x0E);
    }
    else
    {
        PR_AEQ(("under 1G\r\n"));
        h2rxwr(0x20, 0x1B);
        h2rxwr(0x21, 0x03);
    }

    h2rxwr(0x26, 0x00);
    h2rxwr(0x22, 0x38); // [5:3] AUTOAMP , AUTOEQ , EQPWDB
    h2rxset(0x22, 0x04, 0x04); // Trigger EQ
    delay1ms(1);
    h2rxset(0x22, 0x04, 0x00); // Trigger EQ
    chgrxbank(0);

    gdev.EQ.AutoEQ_WaitTime = 0;
}

void ChipIt66353::_aeq_trigger_SAREQ(void)
{
    // MUST disable EQTrg before EQRst
    XBH_U8 def_opt=1;
    chgrxbank(3);
    //  init CSC value
    h2rxwr(0x20, 0x1B);
    h2rxwr(0x21, 0x03);
    h2rxset(0x20, 0x80, 0x00);  // disable CLKStb AutoEQTrg
    h2rxwr(0x22, 0x00); // disable [4] ENDFE, set [2] EQTrg low
    chgrxbank(0);

    _aeq_reset();

    h2rxwr(0x3B, 0x03); // Reg_CEDOPT[5:0]

    chgrxbank(3);

    h2rxwr(0x26, 0x00);
    //h2rxwr(0x27, 0x1F);
    //h2rxwr(0x28, 0x1F);
    //h2rxwr(0x29, 0x1F);
    h2rxset(0x27, 0x80, 0x00);
    h2rxset(0x28, 0x80, 0x00);
    h2rxset(0x29, 0x80, 0x00);

    h2rxset(0x2C, 0xC0, 0xC0);
    //h2rxset(0x2D, 0xF0, 0x20);
    h2rxset(0x2D, 0xF0, 0x20+(def_opt<<4)); // rsdfeopt=1
    h2rxset(0x2D, 0x07, 0x00);
    h2rxwr(0x30, 0x8C);

    h2rxwr(0x31, 0xB0); // AMPTime[7:0]

    h2rxwr(0x32, 0x43);
    h2rxwr(0x33, 0x47);
    h2rxwr(0x34, 0x4B);
    h2rxwr(0x35, 0x53);

    h2rxset(0x36, 0xc0, 0x00); // [7:6] AMPTime[9:8]

    h2rxwr(0x37, 0x0B);        // [7:6] RecChannel, [4]: RSOnestage,
                               // [3] IgnoreOPT, [1:0] MacthNoSel
                               //
    h2rxwr(0x38, 0xF2);        // [7:4] MonTime
    h2rxwr(0x39, 0x04);        // [5] POLBOPT, [4] ADDPClrOPT, [3:0] CED Valid Threshold 0x0D
    h2rxset(0x4A, 0x80, 0x00);
    h2rxset(0x4B, 0x80, 0x00);
    h2rxset(0x54, 0x80, 0x80); // Reg_EN_PREEQ
    h2rxset(0x54, 0x38, 0x38);

    h2rxwr(0x55, 0x40);  // RSM Threshold

    //h2rxset(0x22, 0x04, 0x04);  // Trigger EQ
    h2rxset(0x22, 0x44, 0x04+(def_opt<<6));  // Trigger EQ
    chgrxbank(0);

    gdev.EQ.AutoEQ_WaitTime = 0;
}

void ChipIt66353::_aeq_check_amp_and_rs_status(void)
{
    XBH_U8 i;
    XBH_U8 reg63, reg6D;
    XBH_U16 reg64, reg6E;
    XBH_U16 amp_timeout;

    gdev.EQ.DFE_Valid = 1;
    gdev.EQ.RS_Valid = 1;

    chgrxbank(3);
    for(i=0;i<3;i++)
    {
        h2rxset(0x37, 0xC0, i<<6);
        reg63=h2rxrd(0x63);
        reg64=h2rxrd(0x64)&0x3F;
        reg6D=h2rxrd(0x6D);
        reg6E=h2rxrd(0x6E);

        gdev.EQ.RS_ValidMap[i] =(reg6E<<8) + reg6D;
        amp_timeout = (reg64<<8) + reg63;

        PR_AEQ(("CH %d, AMP_TimeOut = 0x%04X \r\n", i, amp_timeout ));
        PR_AEQ(("CH %d, RS_ValidMap = 0x%04X \r\n",  i, gdev.EQ.RS_ValidMap[i] ));

        if( amp_timeout == 0x3FFF )
        {
            gdev.EQ.DFE_Valid = 0;
        }

        reg63=h2rxrd(0x61);
        reg64=h2rxrd(0x62)&0x3F;
        amp_timeout = (reg64<<8) + reg63;

        PR_AEQ(("CH %d, AMP_Done = 0x%04X \r\n", i, amp_timeout ));

        if ( gdev.EQ.RS_ValidMap[i] == 0x0000 )
        {
            gdev.EQ.RS_Valid = 0;
        }
    }
    chgrxbank(0);
}


XBH_U8 ChipIt66353::_aeq_check_sareq_result(void)
{
    XBH_U8 tmp;
    tmp = h2rxrd(0x07);

    if ( tmp & (BIT5|BIT4) ) // EQ fail | EQ Done
    {
        _aeq_diable_eq_trigger();
        _aeq_check_amp_and_rs_status();
        if ( tmp & BIT4 ) {
            gdev.EQ.AutoEQ_Result = EQRES_SAREQ_DONE;
        }
        else{
            gdev.EQ.AutoEQ_Result = EQRES_SAREQ_FAIL;
        }

        return 1;
    }
    /*
    else if ( tmp & BIT5 ) // EQ fail
    {
        _aeq_diable_eq_trigger();
        _aeq_check_amp_and_rs_status();
        gdev.EQ.AutoEQ_Result = EQRES_SAREQ_FAIL;

    }
    */
    else
    {
        gdev.EQ.AutoEQ_WaitTime++;
        delay1ms(20);
        if ( gdev.EQ.AutoEQ_WaitTime > 30 )
        {
            gdev.EQ.AutoEQ_Result = EQRES_SAREQ_TIMEOUT;
            return 1;
        }
    }

    return 0;
}

XBH_U8 ChipIt66353::_aeq_check_h14eq_result(void)
{
    XBH_U8 tmp;
    tmp = h2rxrd(0x07);

    if ( tmp & BIT6 ) // EQ Done
    {
        _aeq_diable_eq_trigger();
        PR_AEQ(("AEQ 14, DONE\r\n"));
        return EQRES_H14EQ_DONE;
    }
    else if ( tmp & BIT7 ) // EQ fail
    {
        _aeq_diable_eq_trigger();
        PR_ERR(("AEQ 14, FAIL\r\n"));
        return EQRES_H14EQ_FAIL;
    }
    else
    {
        delay1ms(25);
        gdev.EQ.AutoEQ_WaitTime++;
        if ( gdev.EQ.AutoEQ_WaitTime > 100 )
        {
            gdev.EQ.AutoEQ_Result = EQRES_SAREQ_TIMEOUT;
            PR_ERR(("AEQ 14, TIMEOUT\r\n"));
            return EQRES_H14EQ_TIMEOUT;
        }
        else
        {
            //return EQRES_BUSY;
        }

    }
    return EQRES_BUSY;
}

XBH_U8 ChipIt66353::_meq_14(void)
{
    XBH_U8 rs_idx;
    XBH_U8 ch;
    XBH_U8 ch_all_done;
    XBH_U8 ch_done[3];
    XBH_U8 res = EQRES_H14EQ_FAIL;

    ch_done[0] = 0;
    ch_done[1] = 0;
    ch_done[2] = 0;
    ch_all_done = 0;
    ch_rs_idx[0]=0;
    ch_rs_idx[1]=0;
    ch_rs_idx[2]=0;

    _rx_clear_ced_err();
    delay1ms(50);


    for ( rs_idx = 0; rs_idx<14 ; rs_idx++ )
    {
        if ( 0 == _aeq_ready_to_start() )
        {
            PR_ERR(("Cannot perform EQ adjust\r\n"));
            break;
        }


        _rx_update_ced_err_from_hw();

        for (ch = 0; ch < 3; ch++)
        {
            if ( ch_done[ch] )
            {
                //break;
            }

            if ( gdev.vars.RxCEDErrValid & (1<<ch) )
            {
                if ( gdev.vars.RxCEDErr[ch] < 0x01 )
                {
                    ch_done[ch]=1;
                    ch_all_done++;
                    PR_AEQ(("RS good ch%d = %02X\r\n", ch, RS_Value[ ch_rs_idx[ch] ] ));
                }
                else
                {
                    ch_done[ch]=0;
                    PR_AEQ(("RS NG ch%d = %02X, err=%04X\r\n", ch, RS_Value[ ch_rs_idx[ch] ], gdev.vars.RxCEDErr[ch] ));
                }
            }
            else
            {
                ch_done[ch]=0;
            }
        }

        if ( ch_done[0] && ch_done[1] && ch_done[2] ) {
            PR_AEQ(("ch_all_done\r\n"));
            res = EQRES_H14EQ_DONE;
            break;
        }

        _rx_clear_ced_err();
        for (ch = 0; ch < 3; ch++)
        {
            if ( 0 == ch_done[ch] )
            {
                _rx_set_rs(ch, RS_Value[ rs_try_idx[ ch_rs_idx[ch] ] ]);
                ch_rs_idx[ch]++;
                if ( ch_rs_idx[ch]>=14 )
                {
                    ch_rs_idx[ch] = 0;
                }
            }
        }

        delay1ms(100);
    }

    return res;
}


bool ChipIt66353::it6635_auto_eq_adjust(void)
{
    XBH_U8 result;
    XBH_U8 ret=XBH_FALSE;

    if ( _aeq_ready_to_start() )
    {
        h2swset(0xD4, 0x03, 0x01);      // Set TXOE off

        if ( gdev.vars.count_auto_eq_fail > 13 )
        {
            gdev.vars.count_auto_eq_fail = 0;
            _rx_caof_init(gdev.vars.Rx_active_port);
        }

        if ( _rx_is_hdmi20() )
        {
            gdev.EQ.current_eq_mode = EQ_MODE_H20;
            gdev.EQ.RS_Valid = 0;
            gdev.EQ.DFE_Valid = 0;
            _aeq_trigger_SAREQ();

            while ( 0 == _aeq_check_sareq_result() )
            {
                // auto EQ will wait about 400ms here
            }

            if ( gdev.EQ.DFE_Valid && gdev.EQ.RS_Valid )
            {
                _aeq_get_calc_rs();
                _rx_set_rs_3ch( gdev.EQ.CalcRS );
                _aeq_set_DFE2( gdev.EQ.CalcRS[0], gdev.EQ.CalcRS[1], gdev.EQ.CalcRS[2] );
                _rx_DFE_enable(1);

                _eq_save_h20();

                ret = XBH_TRUE;
            }
            else
            {
                chgrxbank(3);
                h2rxset(0x27, 0x80, 0x80);
                h2rxset(0x28, 0x80, 0x80);
                h2rxset(0x29, 0x80, 0x80);
                chgrxbank(0);
            }
        }
        else
        {
            gdev.EQ.current_eq_mode = EQ_MODE_H14;

            #if 0
            _aeq_trigger_EQ();

            result = _aeq_check_h14eq_result();
            while ( EQRES_BUSY == result )
            {
                result = _aeq_check_h14eq_result();
            }

            if ( result == EQRES_H14EQ_DONE )
            {
                _aeq_get_calc_rs();
                _rx_set_rs_3ch( gdev.EQ.CalcRS );
            }
            else
            {
                result = _meq_14();
            }
            #else
            result = _meq_14();
            #endif

            if ( result == EQRES_H14EQ_DONE )
            {
                _eq_save_h14();
                ret = XBH_TRUE;
            }

        }

        h2swset(0xD4, 0x03, 0x00);      // Set DRV_RST='1'

        if ( ret )
        {
            gdev.vars.count_auto_eq_fail=0;
        }
        else
        {
            gdev.vars.count_auto_eq_fail++;
            PR_ERR(("EQ result NG %d\r\n", gdev.vars.count_auto_eq_fail));
        }
    }
    else
    {
        PR_ERR(("EQ adjust not available\r\n"));
    }

    return ret;
}

//-------------------- it6635_EQ.cpp end ---------------------------


//-------------------- it6635.cpp start ---------------------------
//
// ********* compile options ***********
//
// CHECK_DEV_PRESENT:
// 1: FW will restart when device ID check failed.
//
#define CHECK_DEV_PRESENT 0


// ********* compile options end *******

//for CEC
#if EN_CEC
#include "it6635_cec.h"
#include "..\Customer\IT6635_CecSys.h"
#endif
//for CEC

////////////////////////////////////////////////////////////////////////////////
#define g_enable_io_log 0
#define PR_IO(x)  { if (g_enable_io_log)  pr_info x ; }
//#define PR_IO(x)

void ChipIt66353::_pr_buf(void *buffer, int length)
{
    int i;
    XBH_U8 *buf = (XBH_U8 *)buffer;
    int pr_len = 16;

    while ( length )
    {
        if ( length < pr_len )
        {
            pr_len = length;
        }

        for ( i = 0; i < pr_len; i++ )
        {
            PR_INFO(("%02x ",*buf));
            buf++;
        }
        PR_INFO(("\r\n"));
        length -= pr_len;
    }
}

XBH_U8 ChipIt66353::h2swwr(XBH_U8 offset, XBH_U8 wdata)
{
    if ( g_is_set_io == 0 ) {
        //PR_IO(("    ---> h2swwr(0x%02x, 0x%02x);\r\n", offset, wdata));
        //PR51_IO(("    ---> h2swwr(0x%02Bx, 0x%02Bx);\n", offset, wdata));
        PR_IO(("    w %02x %02x %02x\r\n", offset, wdata, gdev.opts.dev_opt->SwAddr));
    }
    else{
        g_is_set_io = 0;
    }

    if ( (gdev.opts.active_rx_opt->EnRxDDCBypass) && (offset == 0x3C) )
    {
        PR_ERR(("****h2swwr 0x3C 0x%02x\n", wdata));
    }

    return it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->SwAddr, offset, 1, &wdata);
}

XBH_U8 ChipIt66353::h2swrd(XBH_U8 offset)
{
    XBH_U8 rddata;

    it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->SwAddr, offset, 1, &rddata);

    return rddata;
}

XBH_U8 ChipIt66353::h2swset(XBH_U8 offset, XBH_U8 mask, XBH_U8 wdata)
{
    XBH_U8 temp;

    g_is_set_io = 1;
    //PR_IO(("    ---> h2swset(0x%02x, 0x%02x, 0x%02x);\r\n", offset, mask, wdata));
    //PR51_IO(("    ---> h2swset(0x%02Bx, 0x%02Bx, 0x%02Bx);\n", offset, mask, wdata));
    PR_IO(("    s %02x %02x %02x %02x\n", offset, mask, wdata, gdev.opts.dev_opt->SwAddr));

    temp = h2swrd(offset);
    temp = (temp & ((~mask) & 0xFF)) + (mask & wdata);

    return h2swwr(offset, temp);
}

void ChipIt66353::h2swbrd(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata)
{
    if ( length > 0 )
    {
        it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->SwAddr, offset, length, rddata);
    }
}

void ChipIt66353::h2swbwr(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata)
{
    PR_IO(("    ---> h2swbwr(0x%02x, 0x%02x);\r\n", offset,*rddata));
    if ( length > 0 )
    {
        it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->SwAddr, offset, length, rddata);
    }
}

////////////////////////////////////////////////////////////////////////////////
XBH_U8 ChipIt66353::h2rxedidwr(XBH_U8 offset, XBH_U8 *wrdata, XBH_U8 length)
{
    //PR_IO(("    ---> h2rxedidwr(0x%02x, 0x%02x);\r\n", offset, length));
    //PR51_IO(("    ---> h2rxedidwr(0x%02Bx, 0x%02Bx);\n", offset, length));
    return it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->EdidAddr, offset, length, wrdata);
}

XBH_U8 ChipIt66353::h2rxedidrd(XBH_U8 offset, XBH_U8 *wrdata, XBH_U8 length)
{
    return it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->EdidAddr, offset, length, wrdata);
}

XBH_U8 ChipIt66353::h2rxwr(XBH_U8 offset, XBH_U8 rdata)
{
    if ( g_is_set_io == 0 ) {
        //PR_IO(("    ---> h2swwr(0x%02x, 0x%02x);\r\n", offset, rdata));
        //PR51_IO(("    ---> h2swwr(0x%02Bx, 0x%02Bx);\n", offset, rdata));
        PR_IO(("w %02x %02x %02x\r\n", offset, rdata, gdev.opts.dev_opt->RxAddr));
    }
    else{
        g_is_set_io = 0;
    }

    return it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->RxAddr, offset, 1, &rdata);
}

XBH_U8 ChipIt66353::h2rxrd(XBH_U8 offset)
{
    XBH_U8 rddata;

    it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->RxAddr, offset, 1, &rddata);

    return rddata;
}

XBH_U8 ChipIt66353::h2rxset(XBH_U8 offset, XBH_U8 mask, XBH_U8 wdata)
{
    XBH_U8 temp;

    g_is_set_io = 1;
    //PR_IO(("    ---> h2swset(0x%02x, 0x%02x, 0x%02x);\r\n", offset, mask, wdata));
    //PR51_IO(("    ---> h2swset(0x%02Bx, 0x%02Bx, 0x%02Bx);\n", offset, mask, wdata));
    PR_IO(("    s %02x %02x %02x %02x\r\n", offset, mask, wdata, gdev.opts.dev_opt->RxAddr));

    temp = h2rxrd(offset);
    temp = (temp & ((~mask) & 0xFF)) + (mask & wdata);

    return h2rxwr(offset, temp);
}

void ChipIt66353::h2rxbrd(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata)
{
    if ( length > 0 )
    {
        it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->RxAddr, offset, length, rddata);
    }
}

void ChipIt66353::h2rxbwr(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata)
{
    PR_IO(("    ---> h2rxbwr(0x%02x, 0x%02x);\r\n", offset,*rddata));
    if ( length > 0 )
    {
        it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->RxAddr, offset, length, rddata);
    }
}

#if EN_CEC

XBH_U8 cecwr(XBH_U8 offset, XBH_U8 wdata)
{
    PR_IO(("    ---> cecwr(0x%02x, 0x%02x);\r\n", offset, wdata));
    return it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->CecAddr, offset, 1, &wdata);
}

XBH_U8 cecrd(XBH_U8 offset)
{
    XBH_U8 rddata;

    it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->CecAddr, offset, 1, &rddata);

    return rddata;
}

XBH_U8 cecset(XBH_U8 offset, XBH_U8 mask, XBH_U8 rdata)
{
    XBH_U8 temp;

    temp = cecrd(offset);
    temp = (temp & ((~mask) & 0xFF)) + (mask & rdata);

    return cecwr(offset, temp);
}

void cecbrd(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata)
{
    if ( length > 0 )
    {
        it6635_i2c_read(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->CecAddr, offset, length, rddata);
    }
}

void cecbwr(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata)
{
    PR_IO(("    ---> cecbwr(0x%02x, 0x%02x);\r\n", offset,*rddata));
    PR51_IO(("    ---> cecbwr(0x%02Bx, 0x%02Bx);\n", offset,*rddata));
    if ( length > 0 )
    {
        it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->CecAddr, offset, length, rddata);
    }
}

#endif

void ChipIt66353::chgrxbank(XBH_U8 bankno)
{
    h2rxset(0x0f, 0x07, bankno & 0x07);
}


//void chgswbank(XBH_U8 bankno)
//{
//    h2swset(0x0f, 0x03, bankno & 0x03);
//}

bool ChipIt66353::_tx_is_sink_hpd_high(void)
{
    if ( h2swrd(0x11) & 0x20 )
    {
        return TRUE;
    }
    return FALSE;
}

bool ChipIt66353::_tx_ddcwait(void)
{
    XBH_U8 ddcwaitcnt, ddc_status;

    ddcwaitcnt = 0;
    do
    {
        ddcwaitcnt++;
        delay1ms(DDCWAITTIME);
    }
    while ( (h2swrd(0x1B) & 0x80) == 0x00 && ddcwaitcnt < DDCWAITNUM );

    if ( ddcwaitcnt == DDCWAITNUM )
    {
        ddc_status = h2swrd(0x1B) & 0xFE;
        PR_ERR(("** TX DDC Bus Sta=%02x\r\n", ddc_status));
        PR_ERR(("** TX DDC Bus Wait TimeOut => "));

        if ( h2swrd(0x27) & 0x80 )
        {
            PR_ERR(("** DDC Bus Hang\r\n"));
            //Do not handle the DDC Bus Hang here
            //h2txwr(port, 0x2E, 0x0F);  // Abort DDC Command
            //h2txwr(port, 0x16, 0x08);  // Clear Interrupt
        }
        else if ( ddc_status & 0x20 )
        {
            PR_ERR(("** DDC NoACK\r\n"));
        }
        else if ( ddc_status & 0x10 )
        {
            PR_ERR(("** DDC WaitBus\r\n"));
        }
        else if ( ddc_status & 0x08 )
        {
            PR_ERR(("** DDC ArbiLose\r\n"));
        }
        else
        {
            PR_ERR(("** UnKnown Issue\r\n"));
        }
        mDDCErrorCount ++;
        if(mDDCErrorCount > 2)
        {
            mDDCErrorCount = 0;
            it66353_dev_reset();
        }
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

XBH_U8 ChipIt66353::_tx_scdc_write(XBH_U8 offset, XBH_U8 wdata)
{
    int ddcwaitsts;
    XBH_U8 reg3C;

    if ( gdev.opts.active_rx_opt->EnRxDDCBypass )
    {
        PR_ERR(("** EnRxDDCBypass:Abort SCDC write\r\n"));
        return FALSE;
    }

    if ( (h2swrd(0x11) & 0x20) == 0x00 )
    {
        PR_ERR(("** HPD-Low:Abort SCDC write\r\n"));
        return FALSE;
    }

    reg3C = h2swrd(0x3C);
    h2swset(0x3C, 0x01, 0x01);          // Enable PC DDC Mode
    h2swwr(0x3D, 0x09);                 // DDC FIFO Clear
    h2swwr(0x3E, 0xA8);                 // EDID Address
    h2swwr(0x3F, offset);               // EDID Offset
    h2swwr(0x40, 0x01);                 // ByteNum[7:0]
    h2swwr(0x42, wdata);                 // WrData
    h2swwr(0x3D, 0x01);                 // Sequential Burst Write
    ddcwaitsts = _tx_ddcwait();
    h2swwr(0x3C, reg3C);          // Disable PC DDC Mode

    if ( ddcwaitsts == 0 )
    {
        PR_INFO(("SCDC wr %02x %02x, ddcwaitsts = %d\r\n", offset, wdata, ddcwaitsts));
    }

    return ddcwaitsts;
}

XBH_U8 ChipIt66353::_tx_scdc_read(XBH_U8 offset, XBH_U8 *data_buf)
{
    int ddcwaitsts;
    XBH_U8 reg3C;

    if ( gdev.opts.active_rx_opt->EnRxDDCBypass )
    {
        PR_ERR(("EnRxDDCBypass:Abort SCDC read\r\n"));
        return FALSE;
    }

    if ( (h2swrd(0x11) & 0x20) == 0x00 )
    {
        PR_ERR(("HPD-Low:Abort SCDC read\r\n"));
        return FALSE;
    }

    reg3C = h2swrd(0x3C);
    h2swset(0x3C, 0x01, 0x01);          // Enable PC DDC Mode
    h2swwr(0x3D, 0x09);                 // DDC FIFO Clear
    h2swwr(0x3E, 0xA8);                 // EDID Address
    h2swwr(0x3F, offset);               // EDID Offset
    h2swwr(0x40, 0x01);                 // ByteNum[7:0]
                                        //h2swwr(0x42, data);                 // WrData
    h2swwr(0x3D, 0x00);                 // Sequential Burst Write
    ddcwaitsts = _tx_ddcwait();
    h2swwr(0x3C, reg3C);          // Disable PC DDC Mode

    if ( ddcwaitsts == 0 )
    {
        PR_ERR(("SCDC rd %02x ddcwaitsts = %d\r\n", offset, ddcwaitsts));
    }
    else
    {
        *data_buf = h2swrd(0x42);
    }

    return ddcwaitsts;
}

XBH_U8 ChipIt66353::_tx_hdcp_write(XBH_U8 offset, XBH_U8 data)
{
    int ddcwaitsts;

    if ( gdev.opts.active_rx_opt->EnRxDDCBypass )
    {
        PR_ERR(("EnRxDDCBypass:Abort HDCP write\r\n"));
        return FALSE;
    }

    if ( (h2swrd(0x11) & 0x20) == 0x00 )
    {
        PR_ERR(("HPD-Low:Abort HDCP write\r\n"));
        return FALSE;
    }

    h2swset(0x3C, 0x01, 0x01);          // Enable PC DDC Mode
    h2swwr(0x3D, 0x09);                 // DDC FIFO Clear
    h2swwr(0x3E, 0x74);                 // EDID Address
    h2swwr(0x3F, offset);               // EDID Offset
    h2swwr(0x40, 0x01);                 // ByteNum[7:0]
    h2swwr(0x42, data);                 // WrData
    h2swwr(0x3D, 0x01);                 // Sequential Burst Write
    ddcwaitsts = _tx_ddcwait();
    h2swset(0x3C, 0x01, 0x00);          // Disable PC DDC Mode

    if ( ddcwaitsts == 0 )
    {
        PR_INFO(("SCDC wr %02x %02x, ddcwaitsts = %d\r\n", offset, data, ddcwaitsts));
    }

    return ddcwaitsts;
}

XBH_U8 ChipIt66353::_tx_hdcp_read(XBH_U8 offset, XBH_U8 *data_buf, XBH_U8 len)
{
    int ddcwaitsts;

    if ( gdev.opts.active_rx_opt->EnRxDDCBypass )
    {
        PR_ERR(("EnRxDDCBypass:Abort HDCP read\r\n"));
        return FALSE;
    }


    if ( (h2swrd(0x11) & 0x20) == 0x00 )
    {
        PR_ERR(("HPD-Low:Abort HDCP read\r\n"));
        return FALSE;
    }

    h2swset(0x3C, 0x01, 0x01);          // Enable PC DDC Mode
    h2swwr(0x3D, 0x09);                 // DDC FIFO Clear
    h2swwr(0x3E, 0x74);                 // EDID Address
    h2swwr(0x3F, offset);               // EDID Offset
    h2swwr(0x40, len);                 // ByteNum[7:0]
                                        //h2swwr(0x42, data);                 // WrData
    h2swwr(0x3D, 0x00);                 // Sequential Burst Write
    ddcwaitsts = _tx_ddcwait();
    h2swset(0x3C, 0x01, 0x00);          // Disable PC DDC Mode

    if ( ddcwaitsts == 0 )
    {
        PR_ERR(("SCDC rd %02x ddcwaitsts = %d\r\n", offset, ddcwaitsts));
    }
    else
    {
        XBH_U8 i;

        PR_INFO(("HDCP read - %02X : ", offset));
        for( i=0 ; i<len ; i++ )
        {
            data_buf[i] = h2swrd(0x42);
            PR_INFO(("%02X ", data_buf[i]));
        }

        PR_INFO(("\r\n"));
    }

    return ddcwaitsts;
}

XBH_U8 ChipIt66353::_tx_scdc_read_ced(XBH_U8 *data_buf)
{
    int ddcwaitsts;
    XBH_U8 i;

    if ( (h2swrd(0x11) & 0x20) == 0x00 )
    {
        PR_ERR(("HPD-Low:Abort SCDC read\r\n"));
        return FALSE;
    }

    h2swset(0x3C, 0x01, 0x01);          // Enable PC DDC Mode
    h2swwr(0x3D, 0x09);                 // DDC FIFO Clear
    h2swwr(0x3E, 0xA8);                 // EDID Address
    h2swwr(0x3F, 0x50);                 // EDID Offset
    h2swwr(0x40, 0x06);                 // ByteNum[7:0]
                                        //h2swwr(0x42, data);                 // WrData
    h2swwr(0x3D, 0x00);                 // Sequential Burst Write
    ddcwaitsts = _tx_ddcwait();
    h2swset(0x3C, 0x01, 0x00);          // Disable PC DDC Mode

    if ( ddcwaitsts == 0 )
    {
        PR_ERR(("SCDC rd ced ddcwaitsts = %d\r\n", ddcwaitsts));
    }
    else
    {
        for ( i=0 ; i<6 ; i++)
        {
            data_buf[i] = h2swrd(0x42);
        }
    }

    return ddcwaitsts;
}

void ChipIt66353::_tx_power_down(void)
{
    if ( gdev.opts.dev_opt->DoTxPowerDown )
    {
        h2swset(0xD3, 0x80, 0x00);
        h2swset(0xD1, 0x60, 0x60);
    }
}

void ChipIt66353::_tx_power_on(void)
{
    h2swset(0xD3, 0x80, 0x80);   // Reg_XP_ALPWDB=1
    h2swset(0xD1, 0x60, 0x00);   // Reg_XP_PWDI=0, Reg_XP_PWDPLL=0
}

void ChipIt66353::_tx_show_sink_ced(void)
{
    XBH_U8 ced_valid;
    XBH_U8 i;
    XBH_U8 pr_ced = 0;
    XBH_U8 ced_value[6];

    ced_valid = h2swrd(0xB0);
    if ( ced_valid )
    {
        PR_INFO(("Begin READ CED:\r\n"));
        pr_ced = 1;
        for (i=0 ; i<6 ; i++)
        {
            if ( ced_valid & (0x01<<i) ) // 0x5? error status is valid
            {
                h2swset(0xAC, 0xE0, (i<<5)); // offset select
                ced_value[i] = h2swrd(0xB1);
            }
        }
        h2swwr(0xAD, 0xFF); // clear CED valid on 0xB0
    }
    else
    {
        #if 0
        if ( read_from_scdc > 10 )
        {
            #if 0
            for ( i = 0; i < 6; i++ )
            {
                _tx_scdc_read(0x50+i, &ced_value[i]);

            }
            #else
            _tx_scdc_read_ced(&ced_value[0]);
            #endif

            PR_INFO(("SCDC: "));
            pr_ced = 1;
            read_from_scdc = 0;
        }
        else
        {
            read_from_scdc++;
        }
        #else
        read_from_scdc = read_from_scdc; // suppress warning
        #endif
    }

    if ( pr_ced )
    {
        for (i=0 ; i<3 ; i++)
        {
            PR_INFO(("ced_valid = %02X, ch%d V=%d err=%04X\r\n", ced_valid, i, (ced_value[2*i+1]>>7)&0x01, ((ced_value[2*i+1]&0xEF)<<8)+ced_value[2*i] ));
        }
        PR_INFO(("\r\n"));
    }
}

void ChipIt66353::_tx_ovwr_hdmi_clk(XBH_U8 ratio)
{
    switch ( ratio )
    {
        case HDMI_MODE_AUTO:
            h2swset(0xB2, 0x03, 0x00);
            break;
        case HDMI_MODE_14:
            h2swset(0xB2, 0x03, 0x01);
            break;
        case HDMI_MODE_20:
            h2swset(0xB2, 0x03, 0x03);
            break;
    }
}

void ChipIt66353::_tx_ovwr_h20_scrb(XBH_U8 scrb)
{
    switch ( scrb )
    {
        case HDMI_MODE_AUTO:
            h2swset(0xB2, 0x0C, 0x00);
            break;
        case HDMI_MODE_14:
            h2swset(0xB2, 0x0C, 0x08);
            break;
        case HDMI_MODE_20:
            h2swset(0xB2, 0x0C, 0x0c);
            break;
    }
}


XBH_U8 ChipIt66353::_rx_is_ch_symlock(XBH_U8 ch)
{
    if ( (h2rxrd(0x14) & (0x08 << ch)) )
    {
        return 1;
    }
    return 0;
}


void ChipIt66353::_rx_ovwr_hdmi_clk(XBH_U8 port, XBH_U8 ratio)
{
    switch ( ratio )
    {
        case HDMI_MODE_AUTO:
            h2swset(0x51 + port, 0x28, 0x00);
            break;
        case HDMI_MODE_14:
            h2swset(0x51 + port, 0x28, 0x20);
            break;
        case HDMI_MODE_20:
            h2swset(0x51 + port, 0x28, 0x28);
            break;
    }
}

void ChipIt66353::_rx_ovwr_h20_scrb(XBH_U8 port, XBH_U8 scrb)
{
    switch ( scrb )
    {
        case HDMI_MODE_AUTO:
            h2swset(0x51 + port, 0x30, 0x00);
            break;
        case HDMI_MODE_14:
            h2swset(0x51 + port, 0x30, 0x20);
            break;
        case HDMI_MODE_20:
            h2swset(0x51 + port, 0x30, 0x30);
            break;
    }
}


void ChipIt66353::_sw_config_timer0(XBH_U8 count)
{
    // init timer = count[6:0] * 10 ms
    h2swwr(0x1C, count);
}

void ChipIt66353::_sw_enable_timer0(void)
{
    h2swset(0x38, 0x02, 0x02);
}

void ChipIt66353::_sw_clear_timer0_interrupt(void)
{
    h2swset(0x28, 0x02, 0x02);
}

void ChipIt66353::_sw_enable_txoe_timer_check(void)
{
    _sw_disable_timer0();
    _sw_config_timer0(45); // 450ms time out
    _sw_clear_timer0_interrupt();
    _sw_enable_timer0();
}

void ChipIt66353::_sw_disable_txoe_timer_check(void)
{
    _sw_disable_timer0();
}


void ChipIt66353::_sw_show_hdcp_status(void)
{
    XBH_U8 hdcp_sts;
    if ( gdev.vars.Rev >= 0xC0 )
    {
        hdcp_sts = h2swrd(0xB3);
        if ( hdcp_sts & BIT5 )
        {
            PR_INFO(("HDCP 2 done\r\n"));
            _sw_clear_hdcp_status();
        }
        if ( hdcp_sts & BIT6 )
        {
            PR_INFO(("HDCP 1 done\r\n"));
            _sw_clear_hdcp_status();
        }
    }
}


XBH_U8 ChipIt66353::it6635_get_port_info1(XBH_U8 port, XBH_U8 info)
{
    XBH_U8 tmp;
    tmp = h2swrd(0x61 + port * 3);

    if ( (tmp & info) == info )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void ChipIt66353::_tx_ovwr_hdmi_mode(XBH_U8 mode)
{
    switch ( mode )
    {
        case HDMI_MODE_AUTO:
            h2swset(0xB2, 0x0F, 0x00);
            break;
        case HDMI_MODE_14:
            h2swset(0xB2, 0x0F, 0x05);
            break;
        case HDMI_MODE_20:
            h2swset(0xB2, 0x0F, 0x0F);
            break;
    }
}

void ChipIt66353::_tx_setup_afe(XBH_U32 vclk)
{
    XBH_U8 H2ON_PLL, DRV_TERMON, DRV_RTERM, DRV_ISW, DRV_ISWC, DRV_TPRE, DRV_NOPE, H2ClkRatio;
    XBH_U8 DRV_PISW, DRV_PISWC, DRV_HS;
    PR_INFO(("_tx_setup_afe %lu\r\n", vclk));

    //h2rxset(0x23, 0x04, 0x04);

    if ( vclk > 100000UL ) // IP_VCLK05 > 50MHz
    {
        h2swset(0xD1, 0x07, 0x04);
    }
    else
    {
        h2swset(0xD1, 0x07, 0x03);
    }

    if ( vclk > 162000UL ) // IP_VCLK05 > 81MHz
    {
        //h2swset(0xD4, 0x04, 0x04);
        DRV_HS=1;
    }
    else
    {
        DRV_HS=0;
    }

    h2swset(0xd8, 0xf0, 0x00);
    if ( vclk > 300000UL )  // single-end swing = 520mV
    {
        DRV_TERMON = 1;
        DRV_RTERM = 0x5;
        DRV_ISW = 0x0E;
        DRV_ISWC = 0x0B;
        DRV_TPRE = 0x0;
        DRV_NOPE = 0;
        H2ON_PLL = 1;
        DRV_PISW = 1;
        DRV_PISWC = 1;
        h2swset(0xd8, 0xf0, 0x30);
    }
    else if ( vclk > 100000UL )  // single-end swing = 450mV
    {
        DRV_TERMON = 1;
        DRV_RTERM = 0x1;
        DRV_ISW = 0x9;
        DRV_ISWC = 0x9;
        DRV_TPRE = 0;
        DRV_NOPE = 1;
        H2ON_PLL = 0;
        DRV_PISW = 1;
        DRV_PISWC = 1;
    }
    else                    // single-end swing = 500mV
    {
        DRV_TERMON = 0;
        DRV_RTERM = 0x0;
        DRV_ISW = 0x3;
        DRV_ISWC = 0x3;
        DRV_TPRE = 0;
        DRV_NOPE = 1;
        H2ON_PLL = 0;
        DRV_PISW = 1;
        DRV_PISWC = 1;
    }

    h2swset(0xD0, 0x08, (H2ON_PLL << 3));
    h2swset(0xD3, 0x1F, DRV_ISW);
    h2swset(0xD4, 0xF4, (DRV_PISWC<<6)+(DRV_PISW<<4)+(DRV_HS<<2));
    h2swset(0xD5, 0xBF, (DRV_NOPE << 7) + (DRV_TERMON << 5) + DRV_RTERM);
    h2swset(0xD7, 0x1F, DRV_ISWC);
    h2swset(0xD6, 0x0F, DRV_TPRE);


    H2ClkRatio = (h2swrd(0xb2) & 0x02) >> 1;    // RegH2ClkRatio from TX

    PR_INFO(("TX Output H2ClkRatio=%d ...\r\n", H2ClkRatio));

    //delay1ms(10);
    //h2rxset(0x23, 0x04, 0x00);
}

XBH_U8 ChipIt66353::_rx_calc_edid_sum(XBH_U8 *edid )
{
    XBH_U8 i;
    XBH_U16 sum = 0x100;

    for (i=0 ; i<127 ; i++)
    {
        sum = sum - edid[i];
    }
    return (sum&0xFF);
}

void ChipIt66353::_rx_caof_init(XBH_U8 port)
{
    XBH_U8 reg08;
    XBH_U8 failcnt;

    h2swset(0x05, 0x01, 0x01);
    h2swset(0x59 + port, 0x20, 0x20);   // new at IT6635B0
    h2swset(0x05 + port, 0x01, 0x01);   // IPLL RST, it6635
    _rx_auto_power_down_enable(port,0);
    _rx_term_power_down(port, 0x00);    // disable PWD CHx termination

    delay1ms(1);
    chgrxbank(3);
    h2rxset(0x3A, 0x80, 0x00);   // Reg_CAOFTrg low
    h2rxset(0xA0, 0x80, 0x80);
    h2rxset(0xA1, 0x80, 0x80);
    h2rxset(0xA2, 0x80, 0x80);
    chgrxbank(0);

    h2rxset(0x2A, 0x41, 0x41);   // CAOF RST and CAOFCLK inversion
    delay1ms(1);
    h2rxset(0x2A, 0x40, 0x00);   // deassert CAOF RST
    h2rxwr(0x25, 0x00);          // Disable AFE PWD
    h2rxset(0x3C, 0x10, 0x00);   // disable PLLBufRst

    chgrxbank(3);
    h2rxset(0x3B, 0xC0, 0x00);   // Reg_ENSOF, Reg_ENCAOF
    h2rxset(0x48, 0x80, 0x80);   // for read back sof value registers
    delay1ms(10);
    h2rxset(0x3A, 0x80, 0x80);   // Reg_CAOFTrg high

    // wait for INT Done
    chgrxbank(0);
    reg08 = 0;
    failcnt = 0;

    while ( reg08 == 0x00 )
    {
        reg08 = h2rxrd(0x08) & 0x10;

        if ( reg08 == 0 )
        {
            failcnt++;
            if ( failcnt >= 10 )
            {
                PR_ERR(("ERROR: CAOF fail !!!\r\n"));

                chgrxbank(3);
                h2rxset(0x3A, 0x80, 0x00); // disable CAOF_Trig
                chgrxbank(0);
                h2rxset(0x2A, 0x40, 0x40); // reset CAOF when caof fail
                h2rxset(0x2A, 0x40, 0x00);
                break;
            }
        }

        delay1ms(2);
    }


    chgrxbank(3);
    h2rxset(0x48, 0x80, 0x80);

    PR_INFO(("CAOF_Int=%02x, Status=%02x\r\n\r\n",(h2rxrd(0x59) & 0xC0),((h2rxrd(0x5A) << 4) + (h2rxrd(0x59) & 0x0F))));
    chgrxbank(0);

    h2swset(0x59+port, 0x20, 0x00);
    h2swset(0x05+port, 0x01, 0x00);
    h2swset(0x05, 0x01, 0x00);

    h2rxset(0x08, 0x30, 0x30);
    h2rxset(0x3C, 0x10, 0x10);

    chgrxbank(3);
    h2rxset(0x3A, 0x80, 0x00); // Reg_CAOFTrg low
    h2rxset(0xA0, 0x80, 0x00);
    h2rxset(0xA1, 0x80, 0x00);
    h2rxset(0xA2, 0x80, 0x00);
    chgrxbank(0);

    _rx_auto_power_down_enable(port, gdev.opts.dev_opt->RxAutoPowerDown);
}

void ChipIt66353::_rx_show_ced_info(void)
{
    XBH_U8 symlock = (h2rxrd(0x14) & 0x38) >> 3;
    XBH_U8 ch;

    if ( 0x38 != symlock )
    {
        PR_INFO(("symlock = %02x\r\n", symlock));
    }
    else
    {
        for (ch=0 ; ch<3 ; ch++)
        {
            if ( gdev.vars.RxCEDErrValid & (0x01<<ch) )
            {
                PR_INFO(("ch_%d CED=0x%04x\r\n", ch, gdev.vars.RxCEDErr[ch]));
            }
            else
            {
                PR_INFO(("ch_%d CED=invalid\r\n", ch));
            }
        }
    }
}


void ChipIt66353::_rx_setup_afe(XBH_U32 vclk)
{
    chgrxbank(3);

    if ( vclk >= (1024UL * 102UL) )
    {
        h2rxset(0xA7, 0x40, 0x40);
    }
    else
    {
        h2rxset(0xA7, 0x40, 0x00);
    }

    chgrxbank(0);
}

XBH_U8 ChipIt66353::_rx_is_any_ch_symlock(void)
{
    if ( (h2rxrd(0x14) & 0x38) )
    {
        return 1;
    }
    return 0;
}

XBH_U8 ChipIt66353::_rx_is_all_ch_symlock(void)
{
    if ( (h2rxrd(0x14) & 0x38) == 0x38 )
    {
        DBG_SYMLOCK_1();
        //it6635_txoe(1);
        return 1;
    }
    DBG_SYMLOCK_0();
    return 0;
}

bool ChipIt66353::_rx_is_5v_active(void)
{

/*
     XBH_BOOL hasPlugin = XBH_FALSE;

     switch (platform_port) {
         case 0:
             XbhService::getModuleInterface()->getFHdmi1Det(&hasPlugin);
             break;
         case 1:
             XbhService::getModuleInterface()->getHdmi1Det(&hasPlugin);
             break;
         case 2:
             XbhService::getModuleInterface()->getHdmi2Det(&hasPlugin);
             break;
         default:
             XLOGE("[%s:%d] Invalid port number:%d\r\n", __func__, __LINE__, platform_port);
             hasPlugin = XBH_FALSE;
    }
     XLOGD("[%s:%d] _rx_is_5v_active  platform_port  %d =  %d ", __func__, __LINE__,platform_port,hasPlugin);

     return hasPlugin;*/

     return (h2rxrd(0x13) & 0x01);
}

XBH_U8 ChipIt66353::_rx_is_clock_stable(void)
{
    if ( it6635_get_port_info0(gdev.vars.Rx_active_port, (PI_CLK_STABLE | PI_CLK_VALID | PI_5V) ) )
    {
        DBG_CLKSTABLE_1();
        return 1;
    }
    else
    {
        DBG_CLKSTABLE_0();
        return 0;
    }
}

#if EN_AUTO_RS
XBH_U8 ChipIt66353::_rx_need_hpd_toggle(void)
{
    XBH_U8 hdcp_sts;
    if ( gdev.vars.Rev >= 0xC0 )
    {
        hdcp_sts = h2swrd(0xB3);
        if ( hdcp_sts & BIT5 )
        {
            PR_INFO(("HDCP 2 done\r\n"));
            return 1;
        }
        if ( hdcp_sts & BIT6 )
        {
            PR_INFO(("HDCP 1 done\r\n"));
            return 1;
        }
        if ( hdcp_sts & BIT7 )
        {
            PR_INFO(("HDCP acc\r\n"));
            //return 0;
        }
    }
    else
    {
        if ( _sw_get_timer0_interrupt() )
        {
            PR_INFO(("TXOE timeout 2\r\n"));
            return 1;
        }
    }

    return 0;

    #if 0
    // todo: need more information
    return 1;
    #endif
}
#endif

void ChipIt66353::_rx_int_enable(void)
{
    // Set RX Interrupt Enable
    h2rxwr(0x53, 0xFF);           // Enable RxIntEn[7:0]
    h2rxwr(0x54, 0xFF);           // Enable RxIntEn[15:8]
    h2rxwr(0x55, 0xFF);           // Enable RxIntEn[23:16]
    h2rxwr(0x56, 0xFF);           // Enable RxIntEn[31:24]
    h2rxwr(0x57, 0xFF);           // Enable RxIntEn[39:32]
    h2rxwr(0x5D, 0xF7);           // Enable BKIntEn[7:0], but timer int
    h2rxwr(0x5E, 0xFF);           // Enable BKIntEn[15:8]
    h2rxwr(0x5F, 0xFF);           // Enable BKIntEn[23:16]
    h2rxset(0x60, 0x20, 0x20);    // RegEnIntOut=1
}

void ChipIt66353::_rx_wdog_rst(XBH_U8 port)
{
    #if 0
    XBH_U8 mask;
    mask = (0x10 << port) | (1 << port);
    h2swset(0x16, mask, mask);
    delay1ms(1);
    h2swset(0x16, mask, 0x00);
    #else
    port=port; // suppress compiler warning
    h2swset(0x2b, 0x01, 0x00);
    delay1ms(2);
    //h2swwr(0x20 + port * 2, 0x7C); // clear clock related interrupt
    h2swset(0x2b, 0x01, 0x01);
    //h2swwr(0x20 + port * 2, 0x04);
    #endif
}

void ChipIt66353::_rx_ovwr_hdmi_mode(XBH_U8 port, XBH_U8 mode)
{
    switch ( mode )
    {
        case HDMI_MODE_AUTO:
            h2swset(0x51 + port, 0x38, 0x00);
            h2swset(0x98 + port, 0xC0, 0x00);
            break;
        case HDMI_MODE_14:
            h2swset(0x51 + port, 0x38, 0x20);
            h2swset(0x98 + port, 0xC0, 0x00);
            break;
        case HDMI_MODE_20:
            h2swset(0x51 + port, 0x38, 0x38);
            h2swset(0x98 + port, 0xC0, 0xC0);
            break;
    }
}


void ChipIt66353::_rx_set_hpd(XBH_U8 port, XBH_U8 hpd_value, XBH_U8 term_value)
{
    //xbh patch begin
    if(term_value == TERM_FOLLOW_TX)
    {
        term_value = TERM_FOLLOW_HPD;
    }
    //xbh patch end
    if ( port < RX_PORT_COUNT )
    {
        switch ( term_value )
        {
            case TERM_LOW:
                term_value = 0xFF;
                break;
            case TERM_HIGH:
                term_value = 0x00;
                break;
            case TERM_FOLLOW_TX:
                if ( h2swrd(0x11) & 0x40 )
                {
                    term_value = 0x00;
                }
                else
                {
                    term_value = 0xFF;
                }
                break;
            case TERM_FOLLOW_HPD:
            default:
                if ( hpd_value )
                {
                    term_value = 0x00;
                }
                else
                {
                    term_value = 0xFF;
                }
                break;
        }

        //if (gdev.vars.RxHPDFlag[port] != value)
        {
            gdev.vars.RxHPDFlag[port] = hpd_value;
            if ( hpd_value )
            {
                if ( gdev.vars.Rx_active_port == port )
                {
                    DBG_TM(RX_HPD_HIGH);

                    if ( gdev.opts.rx_opt[port]->EnRxDDCBypass == 0 )
                    {
                        h2swset(0x3C,0x01,0x01);
                        delay1ms(1);
                        h2swset(0x3C,0x01,0x00);
                    }
                }

                if ( gdev.opts.rx_opt[port]->DisableEdidRam == 0 )
                {
                    _rx_edid_ram_enable(port);
                }

                if ( gdev.opts.rx_opt[port]->HPDOutputInverse )
                {
                    h2swset(0x4C + port, 0xC0, 0x40);   // RXHPD=0
                }
                else
                {
                    h2swset(0x4C + port, 0xC0, 0xC0);   // RXHPD=1
                }


                #if 0
                if ( gdev.vars.Rx_active_port == port )
                {
                    _rx_term_power_down(port,0x7e);     // term power down = 0
                }
                else
                {
                    #if NON_ACTIVE_PORT_DETECT_CLOCK
                    _rx_term_power_down(port,0x7e);     // term power down = 0
                    #else
                    _rx_term_power_down(port,0xFF);     // term power down = 0
                    #endif
                }
                #else
                _rx_term_power_down(port, term_value);
                #endif
            }
            else
            {
                if ( gdev.vars.Rx_active_port == port )
                {
                    DBG_TM(RX_HPD_LOW);
                }

                _rx_edid_ram_disable(port);

                _rx_term_power_down(port, term_value);

                if ( gdev.opts.rx_opt[port]->HPDOutputInverse )
                {
                    h2swset(0x4C + port, 0xC0, 0xC0);   // RXHPD=1
                }
                else
                {
                    h2swset(0x4C + port, 0xC0, 0x40);   // RXHPD=0
                }

                if ( port == gdev.vars.Rx_active_port )
                {
                    h2swset(0xB2, 0x0A, 0x0A); // clear H2Mode
                }
            }
            PR_INFO(("Set RxP%d HPD = %d %02x\r\n",(int)port,(int)hpd_value, (int)term_value));
        }
    }
    else
    {
        PR_ERR(("Invaild port %d\r\n", port));
    }
}

void ChipIt66353::_rx_set_hpd_all(XBH_U8 value)
{
    XBH_U8 i;
    for ( i = 0; i < RX_PORT_COUNT; i++ )
    {
        _rx_set_hpd(i, value, TERM_FOLLOW_HPD);
    }
}

void ChipIt66353::_rx_set_hpd_with_5v_all(XBH_U8 non_active_port_only)
{
    XBH_U8 i;
    for ( i = 0; i < RX_PORT_COUNT; i++ )
    {
        if ( non_active_port_only )
        {
            if ( gdev.vars.Rx_active_port == i )
            {
                continue;
            }
        }

        if ( gdev.opts.rx_opt[i]->NonActivePortReplyHPD )
        {
            if ( it6635_get_port_info0(i, PI_5V) )
            {
                _rx_set_hpd(i, 1, TERM_FOLLOW_HPD);
            }
            else
            {
                _rx_set_hpd(i, 0, TERM_FOLLOW_HPD);
            }
        }
    }
}

/*XBH_U8 ChipIt66353::_rx_get_all_port_5v(void)
{
    XBH_U8 i;
    XBH_U8 ret = 0;
    for ( i = 0; i < RX_PORT_COUNT; i++ )
    {
        if ( it6635_get_port_info0(i, PI_5V) )
        {
            ret |= (1 << i);
        }
    }

    return ret;
}*/

void ChipIt66353::_rx_handle_output_err(void)
{
#if EN_AUTO_RS
    if ( gdev.opts.active_rx_opt->EnableAutoEQ )
    {
        if ( gdev.vars.try_fixed_EQ )
        {
            PR_INFO(("*** fixed EQ fail\r\n"));
            gdev.vars.try_fixed_EQ = 0;
            _eq_reset_txoe_ready();
            _eq_reset_state();
            it6635_fsm_chg(RX_CHECK_EQ);
        }
        else
        {
            it6635_auto_eq_adjust();
        }
    }
#endif
}

void ChipIt66353::_rx_auto_power_down_enable(XBH_U8 port, XBH_U8 enable)
{
    if ( enable )
    {
        //
        // h2swset(0x90 + port, 0x3D, 0x3D); <-- will auto power down D0~D2 3.3V
        // h2swset(0x90 + port, 0x3D, 0x1D); <-- will not power down D0~D2 3.3V
        h2swset(0x90 + port, 0x3D, 0x1D);
    }
    else
    {
        h2swset(0x90 + port, 0x3D, 0x00);
    }
}

void ChipIt66353::_rx_auto_power_down_enable_all(XBH_U8 enable)
{
    XBH_U8 i;
    for ( i = 0; i < RX_PORT_COUNT; i++ )
    {
        _rx_auto_power_down_enable(i, enable);
    }
}

void ChipIt66353::_rx_term_power_down(XBH_U8 port, XBH_U8 channel)
{
    // to detect clock,
    // 0x88[7][0] must be '0','0';
    h2swset(0x88 + port, 0xFF, channel);
}


void ChipIt66353::_sw_int_enable(XBH_U8 port, XBH_U8 enable)
{
    if ( enable )
    {
        h2swwr(0x30 + port * 2, 0xff);  // Enable Switch RX Port Interrupt
        h2swset(0x31 + port * 2, 0x01, 0x01);
    }
    else
    {
        h2swwr(0x30 + port * 2, 0x00);  // Disable Switch RX Port Interrupt
        h2swset(0x31 + port * 2, 0x01, 0x00);
        h2swwr(0x20 + port * 2, 0xff);
        h2swwr(0x21 + port * 2, 0xff);
    }

}

void ChipIt66353::_sw_int_enable_all(XBH_U8 enable)
{
    XBH_U8 i;
    for ( i = 0; i < RX_PORT_COUNT; i++ )
    {
        _sw_int_enable(i, enable);
    }
}

void ChipIt66353::_sw_disable_timer0(void)
{
    // disable timer will also clear timer interrupt flag
    h2swset(0x38, 0x02, 0x00);
}

#if EN_AUTO_RS
XBH_U8 ChipIt66353::_sw_get_timer0_interrupt(void)
{
    return ( (h2swrd(0x28)&0x02) );
}
#endif


void ChipIt66353::_sw_config_timer1(XBH_U8 count)
{
    // init timer = count[6:0] * 10 ms
    // init timer = BIT7|count[6:0] * 100 ms
    h2swwr(0x1D, count);
}

void ChipIt66353::_sw_enable_timer1(void)
{
    h2swset(0x38, 0x04, 0x04);
}

void ChipIt66353::_sw_disable_timer1(void)
{
    h2swset(0x38, 0x04, 0x00);
}

XBH_U8 ChipIt66353::_sw_get_timer1_interrupt(void)
{
    return ( (h2swrd(0x28)&0x04) );
}

void ChipIt66353::_sw_clear_timer1_interrupt(void)
{
    h2swset(0x28, 0x04, 0x04);
}

void ChipIt66353::_sw_enable_hpd_toggle_timer(XBH_U8 timeout)
{
    // init timer = count[6:0] * 10 ms
    // init timer = BIT7|count[6:0] * 100 ms
    _sw_config_timer1(timeout); // HPT toggle time out

    _sw_clear_timer1_interrupt();
    _sw_enable_timer1();
}

void ChipIt66353::_sw_disable_hpd_toggle_timer(void)
{
    _sw_disable_timer1();
}

XBH_U8 ChipIt66353::_sw_check_hpd_toggle_timer(void)
{
    return _sw_get_timer1_interrupt();
}

void ChipIt66353::_sw_reset_scdc_monitor(void)
{
    h2swwr(0xAD, 0xFF);
}

void ChipIt66353::_sw_monitor_and_fix_scdc_write(void)
{
    XBH_U8 reg;
    reg = h2swrd(0xAD);
    if ( reg & 0x10 ) // P0SCDCWrReg20hVld
    {
        PR_INFO(("## src SCDC wr %02x\r\n", reg));

        if ( gdev.vars.current_hdmi_mode == HDMI_MODE_20 )
        {
            if ( (reg&0x03) != 0x03 )
            {
                _tx_scdc_write(0x20, 0x03);
            }
        }
        else if ( gdev.vars.current_hdmi_mode == HDMI_MODE_14 )
        {
            if ( (reg&0x03) != 0x00 )
            {
                _tx_scdc_write(0x20, 0x00);
            }
        }
        _sw_reset_scdc_monitor();
    }
}

void ChipIt66353::_sw_clear_hdcp_status(void)
{
    h2swwr(0xB0, 0xC0);
}

void ChipIt66353::_sw_sdi_check(void)
{
    XBH_U8 port;
    XBH_U8 reg6C, reg70;
    port = gdev.vars.Rx_active_port;

    if ( gdev.vars.sdi_stable_count < 8 )
    {
        if (  it6635_get_port_info0(port, PI_CLK_STABLE) )
        {
            gdev.vars.sdi_stable_count++;
        }
        else
        {
            gdev.vars.sdi_stable_count = 0;
        }
    }
    else
    {
        // perform check
        gdev.vars.sdi_stable_count = 0;
        reg6C = h2swrd(0x6c+port);
        reg70 = h2swrd(0x70+port);

        if ( reg70 & BIT3 )
        {
            reg6C = reg6C/8;
        }
        else if ( reg70 & BIT2 )
        {
            reg6C = reg6C/4;
        }
        else if ( reg70 & BIT1 )
        {
            reg6C = reg6C/2;
        }
        else
        {
            //reg6C = reg6C/1;
        }

        if ( reg6C < 22 )
        {
            reg70 = h2swrd(0x61+port*3);
            if ( 0 == (reg70 & BIT1) )
            {
                //need re-calculate RDetIPLL_HS1P48G
                reg70 = 1<<port;
                h2swset(0x2A, reg70, reg70);
                PR_INFO(("check_for_sdi recheck ...\r\n"));
            }
            else
            {
                gdev.vars.check_for_sdi = 0;
                PR_INFO(("check_for_sdi disabled ...%02x\r\n", h2rxrd(0x13)));
            }
        }
        else
        {
            gdev.vars.check_for_sdi = 0;
        }
    }
}

void ChipIt66353::_sw_hdcp_access_enable(XBH_U8 enable)
{
    if ( gdev.vars.spmon == 2 )
    {
        PR_INFO(("  >> skip HDCP acc %d\r\n", enable));
        return;
    }

    PR_INFO(("  >> HDCP acc %d\r\n", enable));

    if ( enable )
    {
        h2swwr(0xAB, 0x60);
        //h2swset(0x3C, 0x01, 0x00);
    }
    else
    {
        h2swwr(0xAB, 0x74);
        //h2swset(0x3C, 0x01, 0x01);
    }
}

void ChipIt66353::_tx_init(void)
{
    if ( gdev.opts.dev_opt->ForceRxOn )
    {
        // for ATC electrical test
        h2swwr(0xFF, 0xC3);
        h2swwr(0xFF, 0xA5);
        h2swset(0xF4, 0x80, gdev.opts.dev_opt->ForceRxOn << 7);
        h2swwr(0xFF, 0xFF);
    }

    h2swset(0x50, 0x0B, 0x08);

    h2swset(0x3A, 0xC0, (1 << 7) + (0 << 6));
    h2swset(0x3B, 0x03, 0); // DDC 75K
    h2swset(0x43, 0xFC, (0 << 7) + (0 << 5) + (0 << 4) + (2 << 2));
    h2swset(0xA9, 0xC0, (gdev.opts.tx_opt->EnTxChSwap << 7) + (gdev.opts.tx_opt->EnTxPNSwap << 6));

    // Enable HPD and RxSen Interrupt
    h2swwr(0x27, 0xff);
    h2swset(0x37, 0x78, 0x78);

    _tx_power_down();

    h2swset(0xBD, 0x01, gdev.opts.tx_opt->EnTxVCLKInv);
    h2swset(0xA9, 0x20, gdev.opts.tx_opt->EnTxOutD1t << 5);

    h2swset(0x50, 0x03, gdev.vars.Rx_active_port);
    it6635_enable_tx_port(1);
}

void ChipIt66353::_tx_reset(void)
{
    PR_INFO(("TX Reset\r\n"));

    h2swset(0x09, 0x01, 0x01);    // RegSoftTxVRst=1
    h2swset(0x09, 0x01, 0x00);    // RegSoftTxVRst=0

    // Enable TX DDC Master Reset
    h2swset(0x3B, 0x10, 0x10);    // DDC Master Reset
    h2swset(0x3B, 0x10, 0x00);

    _tx_init();
}

void ChipIt66353::_rx_init(void)
{
    // Add RX initial option setting here
    h2rxset(0x34, 0x01, 0x01);  // Reg_AutoRCLK=1 (default)
    h2rxset(0x21, 0x40, 0x40);  // Reg_AutoEDIDRst=1
    h2rxwr(0x3A, 0xCB);         // to reduce RxDeskew Err and Chx LagErr
    h2rxset(0x3B, 0x20, 0x20);  // CED_Opt
    h2swset(0x44, 0x08, 0x00);
    h2rxset(0x29, 0x40, 0x00);
    h2rxset(0x3C, 0x01, 0x00);

    //h2rxset(0x3d, 0x02, 0x02); // Reg_deskewdown = 1
}

void ChipIt66353::_rx_reset(void)
{
    PR_INFO(("RX Reset\r\n"));

    h2rxset(0x29, 0x40, 0x00);
    h2swset(0x44, 0x08, 0x08);
    h2rxwr(0x23, 0x01);   // SWRst=1
    //h2rxwr(0x22, 0x08);   // RegRst=1

    h2rxwr(0x23, 0xAF);
    delay1ms(1);
    h2rxwr(0x23, 0xA0);

    _rx_init();
}

void ChipIt66353::_sw_init(void)
{
    XBH_U8 port;
    // H2SW Initial Setting
    h2swset(0x44, 0x03, RCLKFreqSel);
    delay1ms(1);

    it6635_init_rclk();

    // Enable Slave Address
    h2swwr(0xEF, (gdev.opts.dev_opt->RxAddr<<1) | 0x01);

    #if EN_CEC
    if ( gdev.opts.EnCEC )
    {
        // if CEC is enabled, we should have a accurate RCLK.
        XBH_U16 cec_timer_unit;

        h2swwr(0xEE, ((gdev.opts.dev_opt->CecAddr<<1) | 0x01));
        cecset(0x08, 0x01, 0x01);

        cec_timer_unit = gdev.vars.RCLK / (16*10);
        Cec_Init( 0xff&cec_timer_unit );
    }
    else
    #endif
    {
        XBH_U8 tmp;
        h2swwr(0xEE, ((gdev.opts.dev_opt->CecAddr<<1) | 0x01));
        //cecset(0x0d, 0x10, 0x00); // Disable CEC_IOPU
        tmp = 0x40;
        it6635_i2c_write(gdev.opts.dev_opt->DevNo, gdev.opts.dev_opt->CecAddr, 0x10, 1, &tmp); // Disable CEC_IOPU
        h2swwr(0xEE, ((gdev.opts.dev_opt->CecAddr<<1) & 0xFE));
    }

    h2swset(0x44, 0x40, 0x00);  // EnRxPort2Pwd=0
    delay1ms(10);

    _rx_caof_init(gdev.vars.Rx_active_port);

    // Setup INT Pin: Active Low & Open-Drain
    h2swset(0x11, 0x07, 0x03);

    // Enable SW Interrupt
    h2swset(0x37, 0xE0, 0xE0);
    h2swset(0x38, 0xF9, 0xF9);

    // enable non main port to power down
    h2swset(0x15, 0x08, 0 << 3);

    h2swset(0x2B, 0x02, 0x00);
    h2swset(0x2C, 0xC0, 0xC0);

    h2swset(0x50, 0xf0, 0x00);

    h2swset(0xC4, 0x08, 0x08);
    h2swset(0xC5, 0x08, 0x08);
    h2swset(0xC6, 0x08, 0x08);

    // P0~P3 auto power downs
#if 0
    _rx_auto_power_down_enable_all(1);
#else
    _rx_auto_power_down_enable_all(gdev.opts.dev_opt->RxAutoPowerDown);

    _rx_term_power_down(RX_PORT_0, 0);
    _rx_term_power_down(RX_PORT_1, 0);
    _rx_term_power_down(RX_PORT_2, 0);
    _rx_term_power_down(RX_PORT_3, 0);

#endif

    h2swset(0xF5, 0xE0, (gdev.opts.active_rx_opt->EnRxDDCBypass << 7) + (gdev.opts.active_rx_opt->EnRxPWR5VBypass << 6) + (gdev.opts.active_rx_opt->EnRxHPDBypass << 5));
    if ( gdev.opts.active_rx_opt->EnRxDDCBypass == 1 )
    {
        h2swset(0x3C, 0x01, 0x01); // disable DDCRegen by set RegTxMastersel=1
        h2swset(0xb3, 0x20, 0x20);
        _rx_edid_ram_disable(RX_PORT_0);
        _rx_edid_ram_disable(RX_PORT_1);
        _rx_edid_ram_disable(RX_PORT_2);
        _rx_edid_ram_disable(RX_PORT_3);
    }
    else
    {
        // config EDID RAM
        for (port=0 ; port<RX_PORT_COUNT ; port++)
        {
            if ( gdev.opts.rx_opt[port]->DisableEdidRam )
            {
                _rx_edid_ram_disable(port);
                _rx_edid_address_disable(port);
            }
            else
            {
                _rx_edid_ram_enable(port);
                _rx_edid_address_enable(port);
            }
        }
    }

    if ( gdev.opts.active_rx_opt->EnRxHPDBypass )
    {
        h2swset(0x4c, 0x40, 0x00);
        h2swset(0x4d, 0x40, 0x00);
        h2swset(0x4e, 0x40, 0x00);
    }

    // disable EDID read/write to clear P0AutoH2Mode and AutoScrbEn
    h2swset(0xB2, 0x60, 0x00);
    //h2swset(0xB2, 0x40, 0x00);

    // enable TX port latch ERROR count
    h2swset(0xAC, 0x11, 0x11);

    // ddc monitor
    h2swwr(0xB0, 0x80);

}

void ChipIt66353::_sw_reset(void)
{
    PR_INFO(("Switch Reset\r\n"));

    h2swwr(0xEF, (gdev.opts.dev_opt->RxAddr<<1) | 0x01);
    h2swset(0x0A, 0x01, 0x01);  // SoftRstAll=1
    if(h2swrd(0xEF) == ((gdev.opts.dev_opt->RxAddr<<1) | 0x01))
    {
        h2swset(0x44, 0xA0, 0x80); // ForceWrUpd = 1 and SWGateRCLK = 0
    }
    //h2swset(0x0A, 0x02, 0x02);  // SoftSWRRst=1

    _sw_init();
}

// To have accurate RCLK,
// we should use "it6635_cal_rclk" instead of "it6635_init_rclk"
#if EN_CEC
void it6635_cal_rclk(void)
{
    XBH_U8 i;
    XBH_U8 timer_int, timer_flt, wclk_high_ext;
    XBH_U32 wclk_valid_num, wclk_high_num, wclk_high_num_b, wclk_high_num_c;
    XBH_U32 sum, rclk_tmp, rclk, rddata;

    sum = 0;
    for ( i = 0; i < 5; i++ )
    {
        h2swset(0x11, 0x80, 0x80);
        delay1ms(99);
        h2swset(0x11, 0x80, 0x00);

        rddata = h2swrd(0x12);
        rddata += (h2swrd(0x13) << 8);
        rddata += (h2swrd(0x14) << 16);

        sum += rddata;
    }
    sum /= 5;
    rclk = sum / 100;

    PR_INFO(("RCLK=%d kHz\r\n\r\n", rclk));

    timer_int = rclk / 1000;
    timer_flt = (rclk - timer_int * 1000) * 256 / 1000;

    h2swwr(0x1E, timer_int);
    h2swwr(0x1F, timer_flt);

    rclk_tmp = (rclk)*(1 << RCLKFreqSel);

    wclk_valid_num = (8UL * rclk_tmp + 625) / 1250UL;
    wclk_high_num = (8 * rclk_tmp + 3125) / 6250UL;
    h2swset(0x2C, 0x3F, (XBH_U8)wclk_high_num & 0xFF);
    h2swwr(0x2D, (XBH_U8)wclk_valid_num & 0xFF);

    wclk_high_num_b = 32UL * rclk_tmp / (37125UL);
    wclk_high_num = 32UL * rclk_tmp - (wclk_high_num_b * 37125UL);
    wclk_high_ext = wclk_high_num * 2 / 37125UL;
    h2swwr(0x2E, (wclk_high_ext << 6) + ((XBH_U8)wclk_high_num_b));

    wclk_high_num_c = 4UL * rclk_tmp / 10625UL;
    wclk_high_num = 4UL * rclk_tmp - (wclk_high_num_c * 10625UL);
    wclk_high_ext = wclk_high_num * 4 / 10625UL;
    h2swwr(0x2F, (wclk_high_ext << 6) + ((XBH_U8)wclk_high_num_c));

    gdev.vars.RCLK = rclk;
}
#endif


void ChipIt66353::it6635_init_rclk(void)
{
#if EN_CEC
    if ( gdev.opts.EnCEC )
    {
        it6635_cal_rclk();
    }
    else
#endif
    {
        #if 0
            // RCLK=20000 kHz
            h2swwr(0x1e, 0x14);
            h2swwr(0x1f, 0x00);
            h2swset(0x2c, 0x3f, 0x1a);
            h2swwr(0x2d, 0x80);
            h2swwr(0x2e, 0x11);
            h2swwr(0x2f, 0x87);
            gdev.vars.RCLK = 20000;
        #else
            #if 0
            // RCLK=19569 kHz
            h2swwr(0x1e, 0x13);
            h2swwr(0x1f, 0x91);
            h2swset(0x2c, 0x3f, 0x19);
            h2swwr(0x2d, 0x7d);
            h2swwr(0x2e, 0x50);
            h2swwr(0x2f, 0x47);
            gdev.vars.RCLK = 19569;
            #endif

            // RCLK=18562 kHz
            h2swwr(0x1e, 0x12);
            h2swwr(0x1f, 0x90);
            h2swset(0x2c, 0x3f, 0x18);
            h2swwr(0x2d, 0x77);
            h2swwr(0x2e, 0x10);
            h2swwr(0x2f, 0xc6);
            gdev.vars.RCLK = 18562;

        #endif
    }
}



XBH_U8 ChipIt66353::it6635_get_port_info0(XBH_U8 port, XBH_U8 info)
{
    XBH_U8 tmp;
    tmp = h2swrd(0x60 + port * 3);

    if ( (tmp & info) == info )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void ChipIt66353::it6635_enable_tx_port(XBH_U8 enable)
{
    h2swset(0x50, 0x08, (enable << 3));
}

void ChipIt66353::it6635_txoe(XBH_U8 enable)
{
    if ( gdev.vars.current_txoe == enable )
    {
        return;
    }

    PR_INFO(("TXOE=%d align=%d\r\n", enable, gdev.opts.active_rx_opt->TxOEAlignment));

    if ( enable )
    {
        if ( gdev.vars.current_hdmi_mode == HDMI_MODE_20 )
        {
            _tx_ovwr_hdmi_mode(HDMI_MODE_20);
            _tx_scdc_write(0x20, 0x03);
        }
        else if ( gdev.vars.current_hdmi_mode == HDMI_MODE_14 )
        {
            _tx_ovwr_hdmi_mode(HDMI_MODE_14);
            _tx_scdc_write(0x20, 0x00); // todo: ? check if safe to send this?
        }

        h2swset(0xD4, 0x03, 0x01);      // Set DRV_RST='0'
        h2swset(0xD4, 0x03, 0x00);      // Set DRV_RST='0'

        //REPORT_TXOE_1();
    }
    else
    {
        //REPORT_TXOE_0();

        h2swset(0xD4, 0x07, 0x03);      // Set DRV_RST='1'
    }

    gdev.vars.current_txoe = enable;
}

void ChipIt66353::it6635_auto_txoe(XBH_U8 enable)
{
    PR_INFO(("A_TXOE=%d align=%d\r\n", enable, gdev.opts.active_rx_opt->TxOEAlignment));

    if ( enable )
    {
        h2swset(0xEB, 0x07, 0x02);  // output when data ready
        //h2swset(0xEB, 0x07, 0x07);  // output when clock ready

        h2swset(0xEA, 0xA2, 0x00); //[7]Reg_GateTxOut, [5]Disoutdeskew, [1]Reg_EnTxDly
        h2swset(0xEB, 0x10, 0x00); //[4]RegEnTxDODeskew_doneDly
    }
    else
    {
        h2swset(0xEB, 0x03, 0x01);
    }
}

void ChipIt66353::it6635_set_tx_5v(XBH_U8 output_value)
{
    if ( gdev.vars.Tx_current_5v != output_value )
    {
        gdev.vars.Tx_current_5v = output_value;
        PR_INFO(("TX 5V output=%d\r\n", output_value));
    }

    if ( output_value )
    {
        h2swset(0xF4, 0x0C, 0x0C);    // TXPWR5V=1
    }
    else
    {
        h2swset(0xF4, 0x0C, 0x08);    // TXPWR5V=0
    }
}


XBH_U32 ChipIt66353::it6635_get_rx_vclk(XBH_U8 port)
{
    XBH_U32 tmds_clk;
    #if USING_WDOG
    XBH_U16 tmds_clk_speed;
    XBH_U8  wdog_clk_div;
    XBH_U8  sw_reg20;

    if ( port >= RX_PORT_COUNT )
    {
        PR_ERR(("it6635_get_rx_vclk p=%u\r\n", port));
        return 0;
    }

    _rx_wdog_rst(gdev.vars.Rx_active_port);

__RETRY_VCLK:

    wdog_clk_div = h2swrd(0x70 + port) & 0x07;

    if ( wdog_clk_div & 0x04 )
        wdog_clk_div = 8;
    else if ( wdog_clk_div & 0x02 )
        wdog_clk_div = 4;
    else if ( wdog_clk_div & 0x01 )
        wdog_clk_div = 2;
    else
        wdog_clk_div = 1;

    tmds_clk_speed = h2swrd(0x6C + port);

    sw_reg20 = h2swrd(0x20 + port * 2);
    if ( sw_reg20 & 0x7C )
    {
        PR_ERR(("it6635_get_rx_vclk sw_reg20=%02x\r\n", sw_reg20));
        tmds_clk_speed = ((tmds_clk_speed * 2) >> (RCLKFreqSel));
        tmds_clk = gdev.vars.RCLK * 256 * wdog_clk_div / tmds_clk_speed;

        PR_ERR(("RXP%d WatchDog detect TMDSCLK = %lu kHz (div=%d, 6C=%02x)\r\n", port, tmds_clk, wdog_clk_div, tmds_clk_speed ));

        tmds_clk_speed = 0;

        h2swwr(0x20 + port * 2, sw_reg20);

        goto __RETRY_VCLK;
    }

    if ( tmds_clk_speed )
    {
        tmds_clk_speed = ((tmds_clk_speed * 2) >> (RCLKFreqSel));
        tmds_clk = gdev.vars.RCLK * 256 * wdog_clk_div / tmds_clk_speed;

        PR_INFO(("RXP%d WatchDog detect TMDSCLK = %lu kHz (div=%d, 6C=%02x)\r\n", port, tmds_clk, wdog_clk_div, tmds_clk_speed ));
    }
    else
    {
        PR_ERR(("TMDSCLKSpeed == 0 p=%u\r\n", port));
        tmds_clk = 0;
    }
    #else

    XBH_U8 clk;

    if ( port >= RX_PORT_COUNT )
    {
        PR_ERR(("it6635_get_rx_vclk p=%u\r\n", port));
        return 0;
    }

    clk = h2swrd(0x61 + port*3);

    // the assigned tmds_clk value should refer to _tx_setup_afe()
    if ( clk & 0x04 )
    {
        PR_INFO(("RXP%d clock > 340M\r\n", port ));
        tmds_clk=340000UL;
    }
    else if ( clk & 0x02 )
    {
        PR_INFO(("RXP%d clock > 148M\r\n", port ));
        tmds_clk=163000UL;
    }
    else if ( clk & 0x01 )
    {
        PR_INFO(("RXP%d clock > 100M\r\n", port ));
        tmds_clk=148500UL;
    }
    else
    {
        PR_INFO(("RXP%d clock < 100M\r\n", port ));
        tmds_clk=74000UL;
    }
    #endif

    return tmds_clk;
}

void ChipIt66353::it6635_detect_port(XBH_U8 port)
{
    XBH_U8 sw_reg20;
    XBH_U8 sw_reg21;
    XBH_U8 rddata;
    XBH_U8 sts_off0;

    sw_reg20 = h2swrd(0x20 + port * 2);
    sw_reg21 = h2swrd(0x21 + port * 2) & 0x01;

    if ( sw_reg20 )
    {
        sts_off0 = 0x60 + port * 3;
        rddata = h2swrd(sts_off0);

        if ( sw_reg20 & 0x01 )
        {
            PR_INFO(("--RXP-%d 5V Chg => 5V = %d\r\n", port,(rddata & 0x01)));
            if ( gdev.vars.Rx_active_port != port )
            {
                if ( (rddata & 0x01) )
                {
                    // 5V presents
                    if ( gdev.opts.rx_opt[port]->NonActivePortReplyHPD )
                    {
                        _rx_set_hpd(port, 1,TERM_FOLLOW_HPD);
                        sw_reg20 &= 0x01;
                    }
                    else
                    {
                        _rx_set_hpd(port, 0,TERM_FOLLOW_HPD);
                    }
                }
                else
                {
                    _rx_set_hpd(port, 0,TERM_FOLLOW_HPD);
                }
            }
        }

        if ( sw_reg20 & 0x02 )
        {
            PR_INFO(("--RXP-%d RX Clock Valid Chg => RxCLK_Valid = %d\r\n", port,(rddata & 0x08) >> 3));
        }

        if ( sw_reg20 & 0x04 )
        {
            PR_INFO(("--RXP-%d RX Clock Stable Chg => RxCLK_Stb = %d\r\n\r\n", port,(rddata & 0x10) >> 4));
        }

        if ( sw_reg20 & 0x08 )
        {
            PR_INFO(("--RXP-%d RX Clock Frequency Change ...\r\n", port));
        }

        sts_off0 = 0x61 + port * 3;
        rddata = h2swrd(sts_off0);

        if ( sw_reg20 & 0x10 )
        {
            PR_INFO(("--RXP-%d RX Clock Ratio Chg => Clk_Ratio = %d \r\n", port,(rddata & 0x40) >> 6));
        }

        if ( sw_reg20 & 0x20 )
        {
            PR_INFO(("--RXP%d RX Scrambling Enable Chg => Scr_En = %d \r\n",port,(rddata & 0x80) >> 7));
        }

        sts_off0 = 0x62 + port * 3;
        rddata = h2swrd(sts_off0);

        if ( sw_reg20 & 0x40 )
        {
            PR_INFO(("--RXP%d RX Scrambling Status Chg => ScrbSts = %d \r\n",port,(rddata & 0x02) >> 1));
        }

        if ( sw_reg20 & 0x80 )
        {
            PR_INFO(("--RXP%d RX HDMI2 Detected Interrupt => HDMI2DetSts = %d \r\n",port,(rddata & 0x3C) >> 2));
        }

        h2swwr(0x20 + port * 2, sw_reg20);
    }

    if ( sw_reg21 )
    {

        h2swwr(0x21 + port * 2, sw_reg21);

#if 1
        if ( sw_reg21 & 0x01 )
        {
            PR_INFO(("--RXP%d EDID Bus Hang\r\n", port));
        }
#endif
    }
}

void ChipIt66353::it6635_detect_ports(void)
{
    XBH_U8 i;

    for ( i = 0; i < 4; i++ )
    {
        if ( gdev.vars.Rx_active_port != i )
        {
            it6635_detect_port(i);
        }
    }
}


void ChipIt66353::it6635_rx_irq(void)
{
    XBH_U8 rddata, hdmi_int;
    XBH_U8 rx_reg05, rx_reg06, rx_reg10;

    rddata = h2rxrd(0x96);
    hdmi_int = (rddata & 0x40) >> 6;

    if ( hdmi_int )
    {
        rx_reg05 = h2rxrd(0x05);
        rx_reg06 = h2rxrd(0x06);
        rx_reg10 = h2rxrd(0x10);
        h2rxwr(0x05, rx_reg05);
        h2rxwr(0x06, rx_reg06);

        if ( rx_reg05 & 0x01 )
        {
            PR_INFO(("..RX5V change\r\n"));

            _eq_reset_txoe_ready();
            _eq_reset_state();
            it6635_auto_detect_hdmi_encoding();

            if ( gdev.opts.active_rx_opt->TryFixedEQFirst )
            {
                gdev.vars.try_fixed_EQ = 1;
            }

            if ( 0 == _rx_is_5v_active() )
            {
                XLOGE("[%s:%d] !!! Lost Port.[%d] Signal !!!", __func__, __LINE__, gdev.vars.Rx_active_port);
                it6635_fsm_chg_delayed(RX_UNPLUG);
            }
        }

        if ( rx_reg05 & 0x10 )
        {
            PR_INFO(("..RX HDMIMode chg => HDMIMode = %d\r\n",(h2rxrd(0x13) & 0x02) >> 1));
        }

        if ( rx_reg05 & 0x40 )
        {
            PR_INFO(("..RX DeSkew Err\r\n"));
            gdev.vars.rx_deskew_err++;
            if ( gdev.vars.rx_deskew_err > 50 )
            {
                gdev.vars.rx_deskew_err = 0;
                it6635_toggle_hpd(1000);
            }
        }

        if ( rx_reg05 & 0x80 )
        {
            PR_INFO(("..RXP H2V FIFO Skew Fail\r\n"));
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( rx_reg06 & 0x01 )
        {
            XBH_U8 symlock = ((h2rxrd(0x13) & 0x80) >> 7);
            PR_INFO(("..RX CHx SymLock Chg => RxSymLock = %d\r\n", symlock));
            if ( symlock )
            {
                //gdev.vars.count_fsm_err = 0;
            }
            else
            {
                m_bTxOutput = XBH_FALSE;
            }
        }

        if ( rx_reg06 & 0x02 )
        {
            PR_INFO(("..RX CH0 SymFIFORst\r\n"));
        }

        if ( rx_reg06 & 0x04 )
        {
            PR_INFO(("..RX CH1 SymFIFORst\r\n"));
        }

        if ( rx_reg06 & 0x08 )
        {
            PR_INFO(("..RX CH2 SymFIFORst\r\n"));
        }

        if ( rx_reg06 & 0x10 )
        {
            PR_INFO(("..RX CH0 SymLockRst\r\n"));
        }

        if ( rx_reg06 & 0x20 )
        {
            PR_INFO(("..RX CH1 SymLockRst\r\n"));
        }

        if ( rx_reg06 & 0x40 )
        {
            PR_INFO(("..RX CH2 SymLockRst\r\n"));
        }

        if ( rx_reg06 & 0x80 )
        {
            PR_INFO(("..RX FSM Fail count_fsm_err = %d \r\n",gdev.vars.count_fsm_err));
            gdev.vars.count_fsm_err++;
            if ( gdev.vars.count_fsm_err > 20 )
            {
                if ( gdev.opts.active_rx_opt->FixIncorrectHdmiEnc )
                {
                    it6635_fix_incorrect_hdmi_encoding();
                }
                _eq_reset_txoe_ready();
                _eq_reset_state();
                it6635_fsm_chg(RX_WAIT_CLOCK);
                gdev.vars.count_fsm_err = 0;
            }
        }
        else
        {
            if ( gdev.vars.count_fsm_err > 0 )
            {
                gdev.vars.count_fsm_err--;
            }
        }

        #if EN_H14_SKEW
        {
            XBH_U8 rx_reg07;
            rx_reg07 = h2rxrd(0x07);
            h2rxwr(0x07, rx_reg07);

            if ( rx_reg07 & 0x01 )
            {
                PR_INFO(("..RX CH0 Lag Err\r\n"));
                _rx_skew_adj(0);
            }
            if ( rx_reg07 & 0x02 )
            {
                PR_INFO(("..RX CH1 Lag Err\r\n"));
                _rx_skew_adj(1);
            }
            if ( rx_reg07 & 0x04 )
            {
                PR_INFO(("..RX CH2 Lag Err\r\n"));
                _rx_skew_adj(2);
            }
        }
        #endif

        if ( rx_reg10 & 0x08 )
        {
            h2rxwr(0x10, 0x08);
            PR_INFO(("..RX FW Timer Interrupt ...\r\n"));
        }
    }
}

static int errCount = 0;
void ChipIt66353::it6635_sw_irq(XBH_U8 port)
{
    XBH_U8 sw_reg20;
    XBH_U8 sw_reg21;
    XBH_U8 rddata;
    XBH_U8 sts_off0;


    sw_reg20 = h2swrd(0x20 + port * 2);
    sw_reg21 = h2swrd(0x21 + port * 2) & 0x01;

    if ( sw_reg20 || sw_reg21 )
    {
        h2swwr(0x20 + port * 2, sw_reg20);
        h2swwr(0x21 + port * 2, sw_reg21);

        sts_off0 = 0x60 + port * 3;

        if ( sw_reg20 & 0x01 )
        {
            // not here
            rddata = h2swrd(sts_off0);
            PR_INFO(("..RX-P%d PWR5V Chg => PWR5V = %d\r\n", port,(rddata & 0x01)));
            //_rx_wdog_rst(port);
            if ( 0 == (rddata & 0x01) )
            {
                it6635_fsm_chg_delayed(RX_UNPLUG);
            }
        }

        if ( sw_reg20 & 0x02 )
        {
            rddata = h2swrd(sts_off0);
            PR_INFO(("..RXP%d RX Clock Valid Chg => RxCLK_Valid = %d\r\n", port,(rddata & 0x08) >> 3));
            if ( port == gdev.vars.Rx_active_port )
            {
                if ( 0 == (rddata & 0x08) ) // clock not valid
                {
                    DBG_TM(CLK_UNSTABLE);
                    if ( gdev.vars.RxHPDFlag[gdev.vars.Rx_active_port] > 0 )
                    {
                        XLOGE(" !!! Lost Singal !!!");
                        it6635_fsm_chg_delayed(RX_WAIT_CLOCK);
                    }
                }
                else
                {
                    DBG_TM(CLK_STABLE);
                }
            }
        }

        if ( sw_reg20 & 0x04 )
        {
            delay1ms(10);
            rddata = h2swrd(sts_off0);
            PR_INFO(("it6635_sw_irq ..RXP%d RX Clock Stable Chg => RxCLK_Stb = %d\r\n\r\n", port,(rddata & 0x10) >> 4));
            if(port == gdev.vars.Rx_active_port) //avoid handle inactive port
            {
                if ( 0 == (rddata & 0x10) )
                {
                    DBG_CLKSTABLE_0();
                    DBG_SYMLOCK_0();
                    if ( gdev.vars.RxHPDFlag[port] )
                    {
                        it6635_fsm_chg_delayed(RX_WAIT_CLOCK);
                    }
                }
                else
                {
                    gdev.vars.vclk = it6635_get_rx_vclk(gdev.vars.Rx_active_port);
                    PR_ERR(("it6635_sw_irq ..RXP%d gdev.vars.vclk = %d  gdev.vars.vclk_prev = %d errCount = %d \r\n", port, gdev.vars.vclk, gdev.vars.vclk_prev, errCount));
                    if ( (gdev.vars.vclk != gdev.vars.vclk_prev) )
                    {
                        errCount ++;
                        gdev.vars.vclk_prev = gdev.vars.vclk;
                        if ( gdev.vars.RxHPDFlag[port] )
                        {
                            it6635_fsm_chg_delayed(RX_WAIT_CLOCK);
                        }
                    }
                    else
                    {
                        errCount = 0;
                    }
                }
            } //avoid handle inactive port
        }


        if ( sw_reg20 & 0x08 )
        {
            PR_INFO(("..RXP%d RX Clock Frequency Chg ...\r\n", port));
        }

        if ( sw_reg20 & 0x10 )
        {
            XBH_U8 new_ratio = (h2swrd(0x61 + port * 3) & 0x40) >> 6;

            PR_INFO(("..RXP%d RX Clock Ratio Chg => Clk_Ratio = %d \r\n", port, new_ratio));

            if ( gdev.vars.Rx_active_port == port )
            {
                if ( new_ratio > 0 )
                {
                    it6635_auto_txoe( gdev.opts.active_rx_opt->TxOEAlignment );
                }
                else
                {
                    it6635_auto_txoe(0);
                }
                it6635_txoe(1);

                if ( new_ratio != gdev.vars.clock_ratio )
                {
                    //it6635_auto_detect_hdmi_encoding();
                    //it6635_fsm_chg_delayed(RX_WAIT_CLOCK);
                }
            }
        }

        if ( sw_reg20 & 0x20 )
        {
            PR_INFO(("..RXP%d RX Scrambling Enable Chg => Scr_En = %d \r\n",port,(h2swrd(0x61 + port * 3) & 0x80) >> 7));
        }

        if ( sw_reg20 & 0x40 )
        {
            XBH_U8 new_scramble = (h2swrd(0x62 + port * 3) & 0x02) >> 1;

            PR_INFO(("..RXP%d RX Scrambling Status Chg => ScrbSts = %d \r\n",port, new_scramble));
            if ( gdev.vars.Rx_active_port == port )
            {
                if ( new_scramble != gdev.vars.h2_scramble )
                {
                    //it6635_fsm_chg_delayed(RX_WAIT_CLOCK);
                }
            }
        }

        if ( sw_reg20 & 0x80 )
        {
            PR_INFO(("..RXP%d RX HDMI2 Detected Interrupt => HDMI2DetSts = %d \r\n",port,(h2swrd(0x62 + port * 3) & 0x3C) >> 2));
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if ( sw_reg21 & 0x01 )
        {
            PR_INFO(("..RXP%d EDID Bus Hang\r\n", port));
        }
    }
}


void ChipIt66353::it6635_tx_irq(void)
{
    XBH_U8 sw_reg27;
    XBH_U8 sw_reg28;
    XBH_U8 rddata;
    XBH_U8 reg3C;

    sw_reg27 = h2swrd(0x27);
    sw_reg28 = h2swrd(0x28) & ~(0x02|0x04);
    h2swwr(0x27, sw_reg27);
    h2swwr(0x28, sw_reg28);

    if ( sw_reg27 & 0x08 )
    {
        //PR_INFO((" => HDCP 0x74 is detected\r\n"));
    }

    if ( sw_reg27 & 0x10 )
    {
        PR_INFO(("  => HDCP 0x74 NOACK\r\n"));
    }


    if ( sw_reg27 & 0x20 )
    {
        rddata = h2swrd(0x11);

        if ( (rddata & 0x20) )
        {
            PR_INFO(("  => HPD High\r\n"));
        }
        else
        {
            PR_INFO(("  => HPD Low\r\n"));

            if ( gdev.vars.state_sys_fsm != RX_TOGGLE_HPD &&
                 gdev.vars.state_sys_fsm != RX_UNPLUG )
            {
                it6635_fsm_chg_delayed(TX_UNPLUG);
            }
        }
    }

    if ( sw_reg27 & 0x40 )
    {
        PR_INFO(("  TX RxSen chg\r\n"));
        if ( h2swrd(0x11) & 0x40 )
        {
            // rxsen = 1
        }
        else
        {
            // rxsen = 0
            //_rx_int_enable_all(0);
            //_rx_set_hpd_all(0);
            //it6635_fsm_chg( TX_WAIT_HPD );
        }
    }

    if ( sw_reg27 & 0x80 )
    {
        PR_INFO(("  TX DDC Bus Hang\r\n"));

        if ( gdev.opts.active_rx_opt->EnRxDDCBypass == XBH_FALSE )
        {
            reg3C = h2swrd(0x3C);
            h2swset(0x3C, 0x01, 0x01);
            h2swwr(0x3D, 0x0A);     // Generate SCL Clock
            h2swwr(0x3C, reg3C);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if ( sw_reg28 & 0x02 )
    {
        //PR_INFO(("SW User Timer 0 Interrupt ...\r\n"));
    }

    if ( sw_reg28 & 0x04 )
    {
        //PR_INFO(("SW User Timer 1 Interrupt ...\r\n"));
    }

    if ( sw_reg28 & 0x08 )
    {
        PR_INFO(("  TX DDC Command Fail\r\n"));
        if ( gdev.opts.active_rx_opt->EnRxDDCBypass == XBH_FALSE )
        {
            reg3C = h2swrd(0x3C);
            h2swset(0x3C, 0x01, 0x01);
            h2swwr(0x3D, 0x0F);
            h2swwr(0x3C, reg3C);
        }

    }

    if ( sw_reg28 & 0x80 )
    {
        PR_INFO(("  TX DDC FIFO Error\r\n"));
        if ( gdev.opts.active_rx_opt->EnRxDDCBypass == XBH_FALSE )
        {
            reg3C = h2swrd(0x3C);
            h2swset(0x3C, 0x01, 0x01);
            h2swwr(0x3D, 0x09);
            h2swwr(0x3C, reg3C);
        }
    }
}

void ChipIt66353::it6635_wait_for_ddc_idle(void)
{
    XBH_U8 ddc_sts;
    XBH_U8 idle_cnt = 0;
    XBH_U8 busy_cnt = 0;
    XBH_U8 chk_dly = 3;

    while(1)
    {
        ddc_sts = h2swrd(0xB3);
        if ( (ddc_sts & 0x10) )
        {
            busy_cnt=0;
            idle_cnt++;
            chk_dly++;
            if (idle_cnt>=5)
            {
                break;
            }
        }
        else
        {
            busy_cnt++;
            idle_cnt = 0;
            chk_dly = 3;

            delay1ms(100);
            if ( busy_cnt > 10 )
            {
                PR_ERR(("**Wait DDC idle timeout\n"));
                break;
            }
        }

        delay1ms(chk_dly);
    }
}

void ChipIt66353::it6635_fsm_chg( XBH_U8 new_state )
{
    XLOGD("[%s:%d] state_fsm %s -> %s ", __func__, __LINE__, s__SYS_FSM_STATE[gdev.vars.state_sys_fsm], s__SYS_FSM_STATE[new_state]);

    if ( RX_PORT_CHANGE != new_state )
    {
        if ( gdev.vars.state_sys_fsm == new_state )
        {
            PR_INFO2(("fsm_chg ret\r\n"));
            return;
        }
    }


    if ( new_state == RX_WAIT_CLOCK )
    {
        if ( gdev.vars.RxHPDFlag[gdev.vars.Rx_active_port] == 0 )
        {
            // don't change before HPD High
            PR_INFO2(("skip fsm chg 2\r\n"));
            return;
        }
    }


    gdev.vars.state_sys_fsm = (_SYS_FSM_STATE)new_state;
    gdev.vars.fsm_return = 0;

    switch ( gdev.vars.state_sys_fsm )
    {
        case RX_TOGGLE_HPD:
            h2swset(0x3c, 0x01, 1);//disable ddc hw channel
            _sw_enable_hpd_toggle_timer(gdev.vars.hpd_toggle_timeout);
            break;

        case RX_PORT_CHANGE:

            DBG_TM(RX_SWITCH_PORT);
            PR_INFO(("Active port change from P%d to P%d\r\n", gdev.vars.Rx_active_port, gdev.vars.Rx_new_port));
            break;

        case TX_OUTPUT:
            gdev.vars.count_symlock_lost = 0;
            gdev.vars.count_symlock_unstable = 0;
            gdev.vars.count_try_force_hdmi_mode = 0;
            _sw_disable_hpd_toggle_timer();

            if ( (gdev.opts.active_rx_opt->FixIncorrectHdmiEnc) && (gdev.opts.active_rx_opt->EnRxDDCBypass == FALSE) )
            {
                if ( gdev.vars.current_hdmi_mode == HDMI_MODE_20 )
                {
                    _tx_scdc_write(0x20, 0x03);
                }
                else if ( gdev.vars.current_hdmi_mode == HDMI_MODE_14 )
                {
                    // todo: to check sink support SCDC ?
                    _tx_scdc_write(0x20, 0x00);
                }
                it6635_auto_detect_hdmi_encoding();
            }

            _sw_hdcp_access_enable(1);
            it6635_txoe(1);

            if ( gdev.vars.spmon == 1 )
            {
                if ( (gdev.opts.active_rx_opt->DisableEdidRam & (1<<gdev.vars.Rx_active_port)) == 0 )
                {
                    _rx_edid_ram_enable(gdev.vars.Rx_active_port);
                }
            }
            break;

        case TX_OUTPUT_PREPARE:
            //gdev.vars.count_try_force_hdmi_mode = 0;
//            it6635_auto_txoe(1);
            h2rxwr(0x05, 0xFF);
            h2rxwr(0x06, 0xFF);
            h2rxwr(0x07, 0xFF);
            break;

        #if EN_AUTO_RS
        case RX_CHECK_EQ:
            gdev.vars.count_symlock_fail=0;
            //_sw_hdcp_access_enable(0);
            break;
        #endif

        case SETUP_AFE:
            //gdev.vars.en_count_hdcp = 1;
            gdev.vars.tick_set_afe = get_tick_count();
            _rx_term_power_down(gdev.vars.Rx_active_port,0x00);
            gdev.vars.vclk = it6635_get_rx_vclk(gdev.vars.Rx_active_port);

            if ( gdev.vars.vclk )
            {
                gdev.vars.clock_ratio = ((h2swrd(0x61 + gdev.vars.Rx_active_port * 3) >> 6) & 1);
                PR_INFO2(("Clk Ratio = %d\r\n", gdev.vars.clock_ratio));

                if ( gdev.vars.clock_ratio > 0 )
                {
                    if ( gdev.vars.vclk < 300000UL )
                    {
                        gdev.vars.vclk = 300001UL;
                    }
                    h2rxset(0x3B, 0x10, 0x00); // CED opt for HDBaseT disabled
                }
                else
                {
                    if ( gdev.vars.vclk >= 300000UL )
                    {
                        gdev.vars.vclk = 297000UL;
                    }
                    h2rxset(0x3B, 0x10, 0x10); // CED opt for HDBaseT enabled
                }

                #if 0 // for 8-7 480p
                if ( gdev.vars.vclk < 35000UL ) {
                    PR_INFO2(("## ATC 480P\r\n"));
                    //h2rxset(0x3c, 0x01, 0x00);
                    h2swset(0x2b, 0x02, 0x00);
                }
                else{
                    //h2rxset(0x3c, 0x01, 0x01);
                    h2swset(0x2b, 0x02, 0x02);
                }
                #endif

                _tx_power_on();
                _rx_setup_afe(gdev.vars.vclk);
                _tx_setup_afe(gdev.vars.vclk);


                if ( gdev.vars.clock_ratio == 0 )
                {
                    it6635_auto_txoe(0);
                    PR_INFO2(("Clk Ratio==0, align=0\n"));
                }
                else
                {
                    it6635_auto_txoe(gdev.opts.active_rx_opt->TxOEAlignment);
                    PR_INFO2(("Clk Ratio==1, align=%d\n", gdev.opts.active_rx_opt->TxOEAlignment));
                }

                it6635_txoe(1);


            }
            break;

        case RX_WAIT_CLOCK:
            it6635_txoe(0);
            if ( gdev.opts.dev_opt->TxPowerDownWhileWaitingClock )
            {
                _tx_power_down();
            }

            _sw_disable_timer0();
            _sw_clear_hdcp_status();
            //_rx_wdog_rst(gdev.vars.Rx_active_port);

            _rx_pll_reset();

            #if EN_AUTO_RS
            gdev.vars.RxCEDErrRec[1][0]=0xffff;
            gdev.vars.RxCEDErrRec[1][1]=0xffff;
            gdev.vars.RxCEDErrRec[1][2]=0xffff;
            gdev.EQ.manu_eq_fine_tune_count[0]=0;
            gdev.EQ.manu_eq_fine_tune_count[1]=0;
            gdev.EQ.manu_eq_fine_tune_count[2]=0;
            gdev.EQ.ced_err_avg_prev[0] = 0x8888;
            gdev.EQ.ced_err_avg_prev[1] = 0x8888;
            gdev.EQ.ced_err_avg_prev[2] = 0x8888;
            gdev.EQ.ced_acc_count = 0;
            #endif
            gdev.vars.count_symlock = 0;
            gdev.vars.count_unlock = 0;
            gdev.vars.check_for_hpd_toggle = 0;
            gdev.vars.sdi_stable_count = 0;
            gdev.vars.check_for_sdi = 1;
            gdev.vars.rx_deskew_err = 0;
            break;

        case RX_HPD:
            _rx_int_enable();

            #if 1
            //_rx_ovwr_hdmi_clk(gdev.vars.Rx_active_port, HDMI_MODE_14);
            //_rx_ovwr_h20_scrb(gdev.vars.Rx_active_port, 0);
            #else
            _rx_ovwr_hdmi_clk(gdev.vars.Rx_active_port, RX_CLK_H20);
            _rx_ovwr_h20_scrb(gdev.vars.Rx_active_port, 1);
            #endif


            //gdev.vars.Rx_prev_port = gdev.vars.Rx_active_port;
            //_rx_int_enable_all(0);
            //_rx_set_hpd_all(0);
            _sw_hdcp_access_enable(1);
            _sw_int_enable(gdev.vars.Rx_active_port, 1);
            //_rx_set_hpd(gdev.vars.Rx_active_port, 1);
            _rx_wdog_rst(gdev.vars.Rx_active_port);
            if ( gdev.vars.spmon == 1 )
            {
                if ( (gdev.opts.active_rx_opt->DisableEdidRam & (1<<gdev.vars.Rx_active_port)) == 0 )
                {
                    _rx_edid_ram_disable(gdev.vars.Rx_active_port);
                }
            }

            _rx_set_hpd(gdev.vars.Rx_active_port, 1, TERM_FOLLOW_HPD);

            if ( gdev.vars.is_hdmi20_sink == 0 )
            {
                it6635_auto_txoe(0);
            }
            else
            {
                it6635_auto_txoe( gdev.opts.active_rx_opt->TxOEAlignment );
            }
            it6635_txoe(1);
            h2swset(0x3c, 0x01, 0);//enable ddc hw channel
            break;

        case TX_GOT_HPD:
            it6635_txoe(0);

            //_tx_power_on();
            if ( gdev.opts.active_rx_opt->EnRxDDCBypass == false )
            {
                it6635_setup_edid_ram(0);
                gdev.vars.edid_ready = 1;
            }

            #if EN_CEC
            if(gdev.opts.EnCEC)
            {
                XBH_U8 u8phyAB = (gdev.vars.PhyAdr[0] << 4) | (gdev.vars.PhyAdr[1] & 0xF);
                XBH_U8 u8phyCD = (gdev.vars.PhyAdr[2] << 4) | (gdev.vars.PhyAdr[3] & 0xF);

                CecSys_Init(u8phyAB, u8phyCD, gdev.vars.Rx_active_port);
            }
            #endif

            if ( gdev.opts.active_rx_opt->NonActivePortReplyHPD )
            {
                _rx_set_hpd_with_5v_all(XBH_TRUE);
            }

            break;

        case TX_WAIT_HPD:
            it6635_txoe(0);
            it6635_auto_txoe(0);
            if ( _rx_is_5v_active() )
            {
                it6635_set_tx_5v(1);
            }
            else
            {
                it6635_set_tx_5v(0);
            }

            //it6635_set_RS(10);
            break;

        case TX_UNPLUG:
            h2swset(0x3c, 0x01, 1);//disable ddc hw channel
            it6635_txoe(0);
            it6635_auto_txoe(0);
            gdev.vars.edid_ready = 0;
            //_rx_int_enable(gdev.vars.Rx_active_port, 0);
            _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_TX);
            //_rx_set_hpd_all(0);
            h2swset(0xB2, 0x0A, 0x0A);  // W1C AutoH2Mode and AutoScrbEn
            _tx_power_down();
            break;

        case RX_UNPLUG:
            h2swset(0x3c, 0x01, 1);//disable ddc hw channel
            it6635_txoe(0);
            it6635_auto_txoe(0);
            //_rx_int_enable(gdev.vars.Rx_active_port, 0);
            if ( gdev.vars.force_hpd_state == SW_HPD_LOW )
            {
                _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_HPD);
            }
            else
            {
                _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_TX);
            }

            _rx_reset();
            _rx_caof_init(gdev.vars.Rx_active_port);
            _rx_pll_reset();

            it6635_set_tx_5v(_rx_is_5v_active());
            //gdev.EQ.sys_aEQ = SysAEQ_RUN;
            h2swset(0xB2, 0x0A, 0x0A);  // W1C AutoH2Mode and AutoScrbEn
            _tx_power_down();
            break;
        default:
            break;
    }
}

void ChipIt66353::it6635_fsm_chg_delayed( XBH_U8 new_state )
{
    XLOGD("[%s:%d] state_fsm %s -> %s ", __func__, __LINE__, s__SYS_FSM_STATE[gdev.vars.state_sys_fsm], s__SYS_FSM_STATE[new_state]);
    it6635_fsm_chg(new_state);
    gdev.vars.fsm_return = 1;
}

void ChipIt66353::_rx_pll_reset(void)
{
    h2swset( 0x06+gdev.vars.Rx_active_port, 0x01, 0x01 );
    delay1ms(2);
    h2swset( 0x06+gdev.vars.Rx_active_port, 0x01, 0x00 );
}

void ChipIt66353::it6635_auto_detect_hdmi_encoding(void)
{
    _rx_ovwr_hdmi_mode(gdev.vars.Rx_active_port, HDMI_MODE_AUTO);
    _tx_ovwr_hdmi_mode(HDMI_MODE_AUTO);
    gdev.vars.current_hdmi_mode = HDMI_MODE_AUTO;
    PR_INFO(("HDMI_MODE=AUTO \r\n"));
}

void ChipIt66353::it6635_force_hdmi20(void)
{
    _rx_ovwr_hdmi_mode(gdev.vars.Rx_active_port, HDMI_MODE_20);
    _tx_ovwr_hdmi_mode(HDMI_MODE_20);
    gdev.vars.current_hdmi_mode = HDMI_MODE_20;
    PR_INFO(("HDMI_MODE=F20\r\n"));
}

void ChipIt66353::it6635_force_hdmi14(void)
{
    _rx_ovwr_hdmi_mode(gdev.vars.Rx_active_port, HDMI_MODE_14);
    _tx_ovwr_hdmi_mode(HDMI_MODE_14);
    gdev.vars.current_hdmi_mode = HDMI_MODE_14;
    PR_INFO(("HDMI_MODE=F14\r\n"));
}


void ChipIt66353::it6635_fix_incorrect_hdmi_encoding(void)
{
    switch ( gdev.vars.current_hdmi_mode )
    {
        case HDMI_MODE_AUTO:
            // try HDMI 2.0
            it6635_force_hdmi20();
            break;
        case HDMI_MODE_20:
            // try HDMI 1.4
            it6635_force_hdmi14();
            break;
        case HDMI_MODE_14:
            // try HDMI 2.0
            it6635_auto_detect_hdmi_encoding();
            break;
        default:
            // try HDMI 2.0
            it6635_auto_detect_hdmi_encoding();
            break;
    }

    _rx_pll_reset();
}

#if EN_AUTO_RS
void ChipIt66353::it6635_fsm_EQ_check(void)
{
    XBH_U8 eq_state;

    if ( _rx_is_clock_stable() )
    {
        _rx_show_ced_info();

        if ( _eq_get_txoe_ready() == 1 )
        {
            _eq_load_previous();
            PR_INFO(("EQ restore2 !\r\n"));
            //it6635_fsm_chg(TX_OUTPUT);
            it6635_fsm_chg(TX_OUTPUT_PREPARE);
        }
        else
        {
            eq_state = _eq_get_state();
            PR_INFO(("[%d] eq_state=%d\r\n", __LINE__,(int)eq_state));

            if ( eq_state == SysAEQ_RUN )
            {
                _eq_set_txoe_ready(0);
                if ( it6635_auto_eq_adjust() )
                {
                    gdev.vars.check_for_hpd_toggle = 1;
                    _eq_set_state(SysAEQ_DONE);
                    it6635_fsm_chg(TX_OUTPUT_PREPARE);
                    PR_INFO(("EQ done !\r\n"));
                }
                else
                {
                    aeq_retry++;
                    if ( aeq_retry > 5 )
                    {
                        aeq_retry = 0;
                        gdev.vars.check_for_hpd_toggle = 1;
                        _eq_set_state(SysAEQ_DONE);
                        it6635_fsm_chg(TX_OUTPUT_PREPARE);
                        PR_INFO(("EQ give up !\r\n"));
                    }
                }
            }
            else
            {
                if ( eq_state == SysAEQ_DONE )
                {
                    _eq_load_previous();
                    it6635_fsm_chg(TX_OUTPUT_PREPARE);
                    PR_INFO(("EQ restore !\r\n"));
                }
                else if ( eq_state == SysAEQ_OFF )
                {
                    _eq_load_default();
                    it6635_fsm_chg(TX_OUTPUT_PREPARE);
                    PR_INFO(("EQ default !\r\n"));
                }
                else
                {
                    PR_ERR(("??eq_state=%d\r\n",eq_state));
                }
            }
        }
    }
    else
    {
        it6635_fsm_chg(RX_WAIT_CLOCK);
    }
}
#endif

void ChipIt66353::it6635_fsm(void)
{
//LOOP_FSM:
    fms_state = gdev.vars.state_sys_fsm;
    switch ( gdev.vars.state_sys_fsm )
    {
        case RX_TOGGLE_HPD:
            m_bTxOutput = XBH_FALSE;
            if ( (gdev.opts.active_rx_opt->NonActivePortReplyHPD == 0) && (gdev.opts.tx_opt->TurnOffTx5VWhenSwitchPort == 0) )
            {
                _sw_disable_hpd_toggle_timer();
                it6635_fsm_chg(RX_UNPLUG);
            }
            else
            {
                if ( _sw_check_hpd_toggle_timer() )
                {
                    _sw_disable_hpd_toggle_timer();
                    it6635_fsm_chg(RX_UNPLUG);
                }
                else
                {
                    // keep waiting hpd toggle
                }
            }
            break;

        case RX_PORT_CHANGE:
            //xbh patch begin
            m_bTxOutput = XBH_FALSE;
            mChipWaitClockErrorAndReset = 2;
            //xbh patch end
            it6635_txoe(0);

            if ( gdev.vars.clock_ratio > 0 && gdev.opts.active_rx_opt->EnRxDDCBypass == false )
            {
                _tx_scdc_write(0x20,0x00);
            }

            if ( gdev.opts.tx_opt->TurnOffTx5VWhenSwitchPort )
            {
                it6635_set_tx_5v(0);
            }

            //_rx_int_enable(gdev.vars.Rx_active_port, 1);
            //_rx_set_hpd(gdev.vars.Rx_active_port, 0);
            //_rx_wdog_rst(gdev.vars.Rx_prev_port);

            // make HPD low to stop DDC traffic
            _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_HPD);
            // wait 200ms for DDC traffic stopped
            delay1ms(200);

            // set gdev.vars.force_hpd_state to SW_HPD_AUTO
            // this to reset the force hpd low in previous active port
            // remove this line if you want to keep HPD low after port changing
            gdev.vars.force_hpd_state = SW_HPD_AUTO;

            gdev.vars.Rx_active_port = gdev.vars.Rx_new_port;

            it6635_wait_for_ddc_idle();

            h2swset(0x50, 0x03, gdev.vars.Rx_active_port);

            it6635_set_RS(gdev.opts.active_rx_opt->DefaultEQ[0],
                          gdev.opts.active_rx_opt->DefaultEQ[1],
                          gdev.opts.active_rx_opt->DefaultEQ[2]);

            gdev.EQ.sys_aEQ = SysAEQ_RUN;
            it6635_auto_detect_hdmi_encoding();
            _eq_reset_state();
            _eq_reset_txoe_ready();

            _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_HPD);
            _rx_reset();
            _rx_caof_init(gdev.vars.Rx_active_port);
            _rx_pll_reset();
            gdev.vars.hpd_toggle_timeout = gdev.opts.active_rx_opt->HPDTogglePeriod;
            h2swset(0x3c, 0x01, 1);//disable ddc hw channel
            it6635_fsm_chg(RX_TOGGLE_HPD);
            break;

        case TX_OUTPUT:

            //if ( gdev.opts.WaitSymlockBeforeTXOE )
            {
                if ( 0 == _rx_is_all_ch_symlock() )
                {
                    if ( 0 == _rx_is_any_ch_symlock() )
                    {
                        gdev.vars.count_symlock_lost++;
                        PR_ERR(("RX Symlock lost %d\r\n", gdev.vars.count_symlock_lost));
                        if ( gdev.vars.count_symlock_lost == 10 )
                        {
                            it66353_dev_reset();
                            //_rx_pll_reset();
                            //it6635_toggle_hpd(1000);
                            //it6635_set_tx_5v(0);
                            //gdev.vars.count_symlock_lost = 0;
                            //_rx_handle_output_err();
                        }
                    }
                    else
                    {
                        gdev.vars.count_symlock_unstable++;
                        PR_ERR(("RX Symlock unstable %d\r\n", gdev.vars.count_symlock_unstable));
                        if ( gdev.vars.count_symlock_unstable > 8 )
                        {
                            it66353_dev_reset();
                            //_rx_pll_reset();
                            //it6635_fsm_chg(RX_WAIT_CLOCK);
                            //it6635_toggle_hpd(1000);
                        }
                    }
                }
                else
                {
                    gdev.vars.count_symlock_lost = 0;
                    gdev.vars.count_symlock_unstable = 0;
                }
            }

            if ( _rx_monitor_ced_err() )
            {
                _rx_handle_output_err();
            }

            //_sw_show_hdcp_status();

            if ( gdev.opts.active_rx_opt->FixIncorrectHdmiEnc )
            {
                //check if source send incorrect SCDC clock ratio after 6635 sent.
                if ( gdev.vars.current_hdmi_mode != HDMI_MODE_AUTO && gdev.opts.active_rx_opt->EnRxDDCBypass == XBH_FALSE )
                {
                    _sw_monitor_and_fix_scdc_write();
                }
            }

            if ( gdev.vars.check_for_sdi )
            {
                _sw_sdi_check();
            }
            if(gdev.vars.count_symlock_lost == 0 && gdev.vars.count_symlock_unstable == 0)
            {
                m_bTxOutput = XBH_TRUE;
            }
            else
            {
                m_bTxOutput = XBH_FALSE;
            }

            //_tx_show_sink_ced();

            //_pr_port_info(gdev.vars.Rx_active_port);

            break;

        case TX_OUTPUT_PREPARE:
            m_bTxOutput = XBH_FALSE;
            if ( gdev.opts.active_rx_opt->EnableAutoEQ == 0)
            {
                it6635_fsm_chg(TX_OUTPUT);
                break;
            }
        #if EN_AUTO_RS
            // check symbol lock before tx output
            if ( 0 == _rx_is_any_ch_symlock() )
            {
                PR_ERR(("11111 RxChk-SymUnlock fail count = %d \r\n",gdev.vars.count_symlock_fail));
                gdev.vars.count_symlock_fail++;
                if ( gdev.vars.count_symlock_fail > 3 )
                {
                    gdev.vars.count_symlock_fail = 0;

                    // can not get any channel symbol lock,
                    // the HDMI encoding may be incorrect
                    if ( gdev.opts.active_rx_opt->FixIncorrectHdmiEnc )
                    {
                        PR_ERR(("RxChk-SymUnlock force hdmi mode = %d \r\n",gdev.vars.count_try_force_hdmi_mode));
                        if ( gdev.vars.count_try_force_hdmi_mode < 6 )
                        {
                            gdev.vars.count_try_force_hdmi_mode++;
                            it6635_fix_incorrect_hdmi_encoding();
                        }
                        else
                        {
                            gdev.vars.count_try_force_hdmi_mode = 0;
                            //it6635_dump_register_all();
                            //it6635_toggle_hpd(1000);
                            it66353_dev_reset();
                        }
                    }
                    else
                    {
                        _eq_reset_state();
                        _eq_reset_txoe_ready();
                        it6635_fsm_chg(RX_CHECK_EQ);
                    }
                }
            }
            else
            {
                _eq_set_txoe_ready(1);

                if ( (gdev.vars.check_for_hpd_toggle == 1) && (gdev.vars.current_txoe == 0) && (_rx_need_hpd_toggle()) )
                {
                    DBG_TM(AEQ_TOGGLE_HPD);
                    it6635_set_tx_5v(0);
                    _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_HPD);
                    it6635_fsm_chg(RX_TOGGLE_HPD);
                }
                else
                {
                    #if CHECK_INT_BEFORE_TXOE
                    XBH_U8 reg05 = h2rxrd(0x05);
                    XBH_U8 reg06 = h2rxrd(0x06);
                    XBH_U8 reg07 = h2rxrd(0x07);

                    if ( reg05==0 && reg06 ==0 && reg07 ==0)
                    {
                        prep_count++;
                    }
                    else
                    {
                        PR_ERR(("RX reg: 05=%02x, 06=%02x 07=%02x\r\n", reg05, reg06, reg07));
                        h2rxwr(0x05, reg05);
                        h2rxwr(0x06, reg06);
                        h2rxwr(0x07, reg07);
                        prep_count = 0;
                        prep_fail_count++;
                    }

                    if ( prep_count == 1 )
                    {
                        _sw_hdcp_access_enable(0);
                    }

                    if ( prep_count >= 4 )
                    {
                        prep_count = 0;
                        it6635_fsm_chg(TX_OUTPUT);
                    }
                    else
                    {
                        if ( prep_fail_count > 20)
                        {
                            prep_fail_count = 0;
                            it6635_fsm_chg(RX_WAIT_CLOCK);
                        }
                    }
                    #else
                    it6635_fsm_chg(TX_OUTPUT);
                    #endif
                }
            }
        #endif
            break;

        case RX_CHECK_EQ:
            m_bTxOutput = XBH_FALSE;
        #if EN_AUTO_RS
            it6635_fsm_EQ_check();
        #endif
            break;

        case SETUP_AFE:
            m_bTxOutput = XBH_FALSE;
            prep_count = 0;
            prep_fail_count = 0;
            if ( gdev.vars.vclk == 0 )
            {
                it6635_fsm_chg(RX_WAIT_CLOCK);
            }
            else
            {
            #if EN_AUTO_RS
                if ( gdev.vars.try_fixed_EQ )
                {
                    _eq_set_txoe_ready(1);
                    //it6635_fsm_chg(TX_OUTPUT);
                    it6635_fsm_chg(TX_OUTPUT_PREPARE);
                }
                else
                {
                    if ( gdev.opts.active_rx_opt->EnableAutoEQ )
                    {
                        it6635_fsm_chg(RX_CHECK_EQ);
                    }
                    else
                    {
                        _eq_set_txoe_ready(1);
                        //it6635_fsm_chg(TX_OUTPUT);
                        it6635_fsm_chg(TX_OUTPUT_PREPARE);
                    }
                }
            #else
                _eq_set_txoe_ready(1);
                it6635_fsm_chg(TX_OUTPUT);
                //it6635_fsm_chg(TX_OUTPUT_PREPARE);
            #endif
            }

            break;

        case RX_WAIT_CLOCK:
            m_bTxOutput = XBH_FALSE;
            if ( _rx_is_clock_stable() )
            {
                _rx_clear_ced_err();
                //_sw_enable_txoe_timer_check();
                it6635_fsm_chg(SETUP_AFE);
                gdev.vars.count_wait_clock_none_edid = 0;
                mChipWaitClockErrorAndReset = 2;
            }
            else
            {
                if ( gdev.vars.RxHPDFlag[gdev.vars.Rx_active_port] == 0 )
                {
                    it6635_fsm_chg(RX_UNPLUG);
                }

                if ( gdev.vars.current_hdmi_mode != HDMI_MODE_AUTO )
                {
                    gdev.vars.count_wait_clock++;
                    if ( gdev.vars.count_wait_clock > 100 )
                    {
                        gdev.vars.count_wait_clock = 0;
                        it6635_auto_detect_hdmi_encoding();
                        it6635_fsm_chg(RX_UNPLUG);
                    }
                }
				#if 0
                if(gdev.vars.Rx_active_port == 0 && XbhSourceLogic::getInstance()->getLastSource() == XBH_SOURCE_OPS1)
                {
                    if(mChipWaitClockErrorAndReset >= 0)
                    {
                        gdev.vars.count_wait_clock_none_edid++;
                        PR_ERR(("RX_WAIT_CLOCK  count_wait_clock_none_edid = %d \n", gdev.vars.count_wait_clock_none_edid));
                        if ( gdev.vars.count_wait_clock_none_edid > 200 )
                        {
                            gdev.vars.count_wait_clock_none_edid = 0;
                            //it6635_toggle_hpd(1000);
                            it66353_dev_reset();
                            mChipWaitClockErrorAndReset --;
                            PR_ERR(("RX_WAIT_CLOCK  it6635_toggle_hpd ################ \n"));
                        }
                    }
                }
				#endif
            }
            break;

        case RX_HPD:
            //xbh patch begin
            m_bTxOutput = XBH_FALSE;
            gdev.vars.count_wait_clock_none_edid = 0;
            //xbh patch end
            it6635_fsm_chg(RX_WAIT_CLOCK);
            break;

        case TX_GOT_HPD:
            m_bTxOutput = XBH_FALSE;
            it6635_fsm_chg(RX_HPD);
            //scdcwr(0x30, 0x01);
            break;

        case TX_WAIT_HPD:
            m_bTxOutput = XBH_FALSE;
            if ( 0 == _rx_is_5v_active() )
            {
                it6635_fsm_chg_delayed(RX_UNPLUG);
            }
            if ( _tx_is_sink_hpd_high() )
            {
                it6635_fsm_chg(TX_GOT_HPD);
            }
            break;

        case TX_UNPLUG:
            m_bTxOutput = XBH_FALSE;
            if ( _rx_is_5v_active() )
            {
                it6635_fsm_chg_delayed(TX_WAIT_HPD);
            }
            else
            {
                it6635_fsm_chg_delayed(RX_UNPLUG);
            }
            break;

        case RX_UNPLUG:
            m_bTxOutput = XBH_FALSE;
            if ( _rx_is_5v_active() )
            {
                if ( gdev.vars.force_hpd_state == SW_HPD_LOW )
                {
                    break;
                }

                if ( gdev.vars.state_sys_fsm != RX_TOGGLE_HPD )
                {
                    it6635_fsm_chg_delayed(TX_WAIT_HPD);
                }
            }
            else
            {
                /*if ( _rx_get_all_port_5v() )
                {
                    //it6635_fsm_chg2(TX_WAIT_HPD);
                }*/
            }
            break;

        case IDLE:
            break;
    }

    if ( gdev.vars.fsm_return == 0 )
    {
        gdev.vars.fsm_return = 1;
        //goto LOOP_FSM;
    }
}


void ChipIt66353::it6635_irq(void)
{
    XBH_U8 sys_int_sts;

    it6635_detect_ports();

    if ( gdev.vars.state_sys_fsm == RX_TOGGLE_HPD )
    {
        return;
    }

    {
        //static XBH_U8 prevBD =1;
        XBH_U8 currBD = h2swrd(0xBD);
        //if ( currBD != prevBD ) {
        if ( currBD & 0xe0 ) {
            gdev.vars.tick_hdcp = get_tick_count();
            PR_INFO(("---HDCP BD=%02x (%lu)\r\n", currBD, (XBH_U32)(gdev.vars.tick_hdcp-gdev.vars.tick_set_afe)));
            //prevBD = currBD;
            _sw_clear_hdcp_status();


        }
    }

    sys_int_sts = h2swrd(0x0C);

    if ( sys_int_sts == 0x00 )
    {
        return;
    }

    if ( sys_int_sts & 0x01 )
    {
        it6635_rx_irq();
    }

    if ( sys_int_sts & 0x10 )
    {
        it6635_sw_irq(gdev.vars.Rx_active_port);
        it6635_tx_irq();
    }

    #if EN_CEC
    if ( gdev.opts.EnCEC && (sys_int_sts & 0x80)  )
    {
        Cec_Irq();
    }
    if(gdev.opts.EnCEC)
    {
        CecSys_TxHandler();
        CecSys_RxHandler();
    }
    #endif
}

bool ChipIt66353::it6635_device_init(void)
{
    XBH_U8 i;
    bool init_done = 0;

    switch ( gdev.vars.state_dev_init )
    {
        case 0:
            DBG_CLKSTABLE_0();
            DBG_SYMLOCK_0();

            it6635_set_tx_5v(0);

            _sw_reset();
            _rx_set_hpd_all(0);
            _rx_reset();
            _tx_reset();

            it6635_txoe(0);

            _sw_disable_timer0();
            _sw_disable_timer1();
            //_sw_config_timer1(50);
            //_sw_enable_timer1();

            // config default RS
            it6635_set_RS(gdev.opts.active_rx_opt->DefaultEQ[0],
                          gdev.opts.active_rx_opt->DefaultEQ[1],
                          gdev.opts.active_rx_opt->DefaultEQ[2]);


            if ( gdev.opts.tx_opt->CopyEDIDFromSink )
            {
                it6635_set_tx_5v(1);
                gdev.vars.state_dev_init = 1;
                gdev.vars.hpd_wait_count = 0;
            }
            else
            {
                gdev.vars.state_dev_init = 2;
            }
            break;

        case 1:
            if ( _tx_is_sink_hpd_high() )
            {
                if ( gdev.opts.active_rx_opt->EnRxDDCBypass == XBH_FALSE )
                {
                    if ( it6635_setup_edid_ram(0) )
                    {
                        gdev.vars.edid_ready = 1;
                    }
                    gdev.vars.state_dev_init = 3;
                }
                else
                {
                    gdev.vars.edid_ready = 1;
                    gdev.vars.state_dev_init = 3;
                }
            }
            else
            {
                gdev.vars.hpd_wait_count++;
                if ( gdev.vars.hpd_wait_count > 200 )
                {
                    //gdev.vars.state_dev_init = 2;
                    gdev.vars.hpd_wait_count = 0;
                    PR_INFO(("waiting HPD...\r\n"));
                }
                //it6635_set_tx_5v(_rx_is_5v_active());
            }
            break;

        case 2:
            // load FW default EDID
            PR_INFO(("Using internal EDID...\r\n"));
            if ( gdev.opts.active_rx_opt->EnRxDDCBypass == XBH_FALSE )
            {
                gdev.vars.default_edid[0] = s_default_edid_port0;
                gdev.vars.default_edid[1] = s_default_edid_port0;
                gdev.vars.default_edid[2] = s_default_edid_port0;
                gdev.vars.default_edid[3] = s_default_edid_port0;

                // note:
                // The EDID can be different from each port.
                // please set default_edid[?] pointer to a desired EDID array
                // if you need different EDID.
                //
                // for example:
                // gdev.vars.default_edid[1] = s_default_edid_port1;
                // gdev.vars.default_edid[2] = s_default_edid_port2;
                // gdev.vars.default_edid[3] = s_default_edid_port3;

                for ( i = 0; i < RX_PORT_COUNT; i++ )
                {
                    it6635_setup_edid_ram(i);
                }
            }

            gdev.vars.edid_ready = 1;
            gdev.vars.state_dev_init = 3;

            break;

        case 3:
            _sw_int_enable_all(1);
            _rx_set_hpd_with_5v_all(1);

            PR_INFO(("Active port = P%d ", gdev.vars.Rx_active_port));

            it6635_set_tx_5v(_rx_is_5v_active());

            init_done = 1;
            mInitStatus = 1;
            break;

        default:
            gdev.vars.state_dev_init = 0;
            break;
    }

    return init_done;
}

XBH_U8 ChipIt66353::it66353_get_init_status(void)
{
    if (mInitStatus == 1)
    {
        return 1;
    }
    return 0;
}

void ChipIt66353::it6635_vars_init(void)
{
    // FW Variables init:
    gdev.vars.state_dev_init = 0;
    gdev.vars.VSDBOffset = 0xFF;
    gdev.vars.PhyAdr[0] = 0;
    gdev.vars.PhyAdr[1] = 0;
    gdev.vars.PhyAdr[2] = 0;
    gdev.vars.PhyAdr[3] = 0;

    gdev.vars.RxHPDFlag[0] = -1;
    gdev.vars.RxHPDFlag[1] = -1;
    gdev.vars.RxHPDFlag[2] = -1;
    gdev.vars.RxHPDFlag[3] = -1;
    gdev.vars.Tx_current_5v = -1;

    gdev.vars.count_eq_check = 0;
    gdev.vars.count_fsm_err = 0;
    gdev.vars.count_unlock = 0;
    gdev.vars.state_sys_fsm = RX_UNPLUG;
    gdev.EQ.AutoEQ_state = AEQ_OFF;
    gdev.EQ.sys_aEQ = SysAEQ_RUN;
    gdev.vars.edid_ready = 0;
    gdev.vars.current_txoe = 0xFF;
    gdev.vars.check_for_hpd_toggle = 0;
    gdev.vars.sdi_stable_count = 0;
    gdev.vars.check_for_sdi = 1;
    gdev.vars.force_hpd_state = SW_HPD_AUTO; // 1 : auto, don't modify here
    gdev.vars.vclk_prev = 0;

    if ( gdev.opts.active_rx_opt->TryFixedEQFirst )
    {
        gdev.vars.try_fixed_EQ = 1;
    }

    gdev.vars.current_hdmi_mode = HDMI_MODE_AUTO;
    gdev.vars.rx_deskew_err = 0;
    _eq_reset_state();
    _eq_reset_txoe_ready();

    it6635_dump_opts();
}

#if CHECK_DEV_PRESENT
bool it6635_is_device_lost(void)
{
    XBH_U8 vendor_id[2] = { 0 };

    vendor_id[0] = h2swrd(0x00);
    vendor_id[1] = h2swrd(0x01);
    if ( vendor_id[0] == 0x54 && vendor_id[1] == 0x49 )
    {
        return FALSE;
    }
    return TRUE;
}
#endif

bool ChipIt66353::it6635_is_device_ready(void)
{
    XBH_U8 vendor_id[2] = { 0 };

    vendor_id[0] = h2swrd(0x00);
    vendor_id[1] = h2swrd(0x01);
    if ( vendor_id[0] == 0x54 && vendor_id[1] == 0x49 )
    {
        vendor_id[0] = 0;
        vendor_id[1] = 0;

        vendor_id[1] = h2swrd(0x03);
        if ( vendor_id[1] == 0x66 )
        {
            vendor_id[0] = h2swrd(0x02);
            if ( vendor_id[0] == 0x35 )
            {
                gdev.vars.Rev = h2swrd(0x04);
                PR_INFO(("Find 6635 %02x !! \r\n",(int)gdev.vars.Rev));
                return TRUE;
            }
        }
        else if ( vendor_id[1] == 0x35 )
        {
            vendor_id[0] = h2swrd(0x04);
            if ( vendor_id[0] == 0x66 )
            {
                gdev.vars.Rev = h2swrd(0x05);
                PR_INFO(("Find 6635x %02x !! \r\n",(int)gdev.vars.Rev));
                return TRUE;
            }
        }
    }
    PR_INFO(("Find 6635 fail !!\r\n"));

    return FALSE;
}



bool ChipIt66353::it6635_read_edid(XBH_U8 block, XBH_U8 offset, int length, XBH_U8 *edid_buffer)
{
    bool result = XBH_FALSE;
    int off = block * 128 + offset;
    XBH_U8 reg3C;
    int retry = 5;//0  sometimes soc hdmi init later than 66353


    offset = off % 256;
    reg3C = h2swrd(0x3C);

__RETRY:

    h2swset(0x3C, 0x01, 0x01);    // Enable PC DDC Mode
    h2swset(0x38, 0x08, 0x08);    // Enable DDC Command Fail Interrupt
    h2swset(0x37, 0x80, 0x80);    // Enable DDC Bus Hang Interrupt

    h2swwr(0x3D, 0x09);                  // DDC FIFO Clear
    h2swwr(0x3E, 0xA0);                  // EDID Address
    h2swwr(0x3F, offset);                // EDID Offset
    h2swwr(0x40, length);                // Read ByteNum[7:0]
    h2swwr(0x41, block/2);           // EDID Segment

    if ( _tx_is_sink_hpd_high() )
    {
        h2swwr(0x3D, 0x03);              // EDID Read Fire

        if ( _tx_ddcwait() )
        {
            h2swbrd(0x42, length, edid_buffer);
            result = XBH_TRUE;
        }
        else
        {
            PR_ERR(("ERROR: DDC EDID Read Fail !!!\r\n"));
            if ( retry > 0 )
            {
                retry--;
                delay1ms(100);
                goto __RETRY;
            }
        }
    }
    else
    {
        PR_ERR(("Abort EDID read becasue of detecting unplug !!!\r\n"));
    }

    h2swwr(0x3C, reg3C);   // restore PC DDC Mode

    return result;
}

/**
 * it6635_read_one_block_edid - will read 128 byte EDID data
 *
 * @block: EDID block number. ( 0,1,2 or 3 )
 *
 * @edid_buffer: 128 byte EDID data buffer to store data.
 *
 * Read 128 byte EDID data from assigned block.
 */
bool ChipIt66353::it6635_read_one_block_edid(XBH_U8 block, XBH_U8 *edid_buffer)
{
    XBH_U8 offset;
    XBH_U8 i;
    XBH_U8 read_len = 16;
    offset = 0;
    for ( i = 0; i < 128 / read_len; i++ )
    {
        if ( it6635_read_edid(block, offset, read_len, edid_buffer) )
        {
            edid_buffer += read_len;
            offset += read_len;
            continue;
        }
        else
        {
            PR_ERR(("ERROR: read edid block 0, offset %d, length %d fail.\r\r\n",(int)offset,(int)read_len));
            return XBH_FALSE;
        }
    }

    return XBH_TRUE;
}

void ChipIt66353::it6635_parse_edid_for_vsdb(XBH_U8 *edid)
{
    int off;
    XBH_U8 tag;
    XBH_U8 len;

    // to find HDMI2.0 VSDB in EDID

    if ( edid[0] == 0x02 ) // CTA ext tag
    {
        off = 4;
        while ( off < 0x100 )
        {
            tag = (edid[off] >> 5) & 0x7;
            len = (edid[off] & 0x1F) + 1;

            if ( tag == 0x03 ) // VSDB
            {
                if ( (edid[off+1] == 0xD8) && (edid[off+2] == 0x5D) && (edid[off+3] == 0xC4) )
                {
                    gdev.vars.is_hdmi20_sink = 1;
                    break;
                }
            }

            if ( len == 0 )
                break;
            off += len;
        }
    }

    PR_INFO(("HDMI2 sink=%d\n", gdev.vars.is_hdmi20_sink));
}

/**
 * it6635_parse_edid_for_phyaddr - parse necessary data for RX
 * EDID
 *
 * @edid: 128 byte EDID data buffer that contains HDMI CEA ext
 *
 * Before set RX EDID, must call it6635_parse_edid_cea_ext to
 * initialize some variables.
 */
void ChipIt66353::it6635_parse_edid_for_phyaddr(XBH_U8 *edid)
{
    int off;
    XBH_U8 tag;
    XBH_U8 len;

    // to find VSDB in EDID

    if ( edid[0] == 0x02 ) // CTA ext tag
    {
        off = 4;
        while ( off < 0x100 )
        {
            tag = (edid[off] >> 5) & 0x7;
            len = (edid[off] & 0x1F) + 1;

            if ( tag == 0x03 ) // VSDB
            {
                if ( (edid[off + 1] == 0x03) && (edid[off + 2] == 0x0C) && (edid[off + 3] == 0x00) )
                //(edid[off+1] == 0xD8) && (edid[off+2] == 0x5D) && (edid[off+3] == 0xC4) )
                {
                    gdev.vars.VSDBOffset = ((XBH_U8)off) + 0x80 + 4;
                    gdev.vars.PhyAdr[0] = (edid[off + 4] >> 4) & 0xF;
                    gdev.vars.PhyAdr[1] = edid[off + 4] & 0xF;
                    gdev.vars.PhyAdr[2] = (edid[off + 5] >> 4) & 0xF;
                    gdev.vars.PhyAdr[3] = edid[off + 5] & 0xF;
                    gdev.vars.EdidChkSum[1] = (0x100 - edid[0x7F] - edid[off + 4] - edid[off + 5]) & 0xFF;
                    break;
                }
            }

            if ( len == 0 )
                break;
            off += len;
        }
    }
}

void ChipIt66353::it6635_setup_edid_ram_step2(XBH_U8 block)
{
    int i;
    XBH_U8 wcount = 16;
    XBH_U8 phyAB, phyCD;
    XBH_U8 mask;
    XBH_U16 sum;

    PR_INFO(("Set RX EDID step2...\r\n"));

    if ( block == 0 )
    {
        for ( i = 0; i < 4; i++ )
        {
            _rx_edid_set_chksum(i, gdev.vars.EdidChkSum[0]);
        }
    }

    if ( block == 1 )
    {
        h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1) | 0x01);
        h2rxset(0x4C, 0x0F, 0x0F);

        h2swwr(0xe9, gdev.vars.VSDBOffset);    // VSDB_Offset
        PR_INFO(("VSDB=%02x\r\n", gdev.vars.VSDBOffset));

        // fill 0xF, if there is no zero address
        if ( gdev.vars.PhyAdr[0] && gdev.vars.PhyAdr[1] && gdev.vars.PhyAdr[2] && gdev.vars.PhyAdr[3] )
        {
            gdev.vars.PhyAdr[0] = 0xF;
            gdev.vars.PhyAdr[1] = 0xF;
            gdev.vars.PhyAdr[2] = 0xF;
            gdev.vars.PhyAdr[3] = 0xF;
        }

        phyAB = (gdev.vars.PhyAdr[0] << 4) | (gdev.vars.PhyAdr[1] & 0xF);
        phyCD = (gdev.vars.PhyAdr[2] << 4) | (gdev.vars.PhyAdr[3] & 0xF);

        for ( i = 0; i < 4; i++ )
        {
            h2swwr(0xd9 + i * 2, phyAB);    // Port0 VSDB_AB
            h2swwr(0xda + i * 2, phyCD);    // Port0 VSDB_CD
        }

#if 1
        mask = 0xf0;//set first addr bit.
        //port0;
        phyCD = 0x00;
        phyAB = HDMI_PORT_0_EDID_ADDR;
        h2swset(0xd9 + 0 * 2, mask, phyAB << 4);
        h2swset(0xda + 0 * 2, mask, phyCD);
        //port1;
        phyCD = 0x00;
        phyAB = HDMI_PORT_1_EDID_ADDR;
        h2swset(0xd9 + 1 * 2, mask, phyAB << 4);
        h2swset(0xda + 1 * 2, mask, phyCD);
        //port2;
        phyCD = 0x00;
        phyAB = HDMI_PORT_2_EDID_ADDR;
        h2swset(0xd9 + 2 * 2, mask, phyAB << 4);
        h2swset(0xda + 2 * 2, mask, phyCD);
        //port3;
        phyCD = 0x00;
        phyAB = HDMI_PORT_3_EDID_ADDR;
        h2swset(0xd9 + 3 * 2, mask, phyAB << 4);
        h2swset(0xda + 3 * 2, mask, phyCD);
#else
        for ( i = 0; i < 4; i++ )
        {
            if ( gdev.vars.PhyAdr[i] == 0 )
            {
                mask = 0xF0 >> (4 * (i & 0x01));
                for ( wcount = 0; wcount < 4; wcount++ )
                {
                    phyAB = wcount + 1;
                    if ( mask == 0xF0 )
                    {
                        phyAB = phyAB << 4;
                    }
                    h2swset(0xd9 + wcount * 2 + i / 2, mask, phyAB);
                }
                break;
            }
        }
#endif

        for ( i = 0; i < 4; i++ )
        {
            phyAB = h2swrd(0xd9 + i * 2);    // Port(i) VSDB_AB
            phyCD = h2swrd(0xda + i * 2);    // Port(i) VSDB_CD

            // Port(i) Block1_ChkSum
            sum = (0x100 - gdev.vars.EdidChkSum[1] - phyAB - phyCD) & 0xFF;
            h2swwr(0xe2 + i * 2, (XBH_U8)sum);

            //if ( gdev.vars.Rev >= 0xC )
            {
                #if 0
                switch(i)
                {
                    case 0: mask = 1<<1; break;
                    case 1: mask = 1<<2; break;
                    case 2: mask = 1<<0; break;
                    case 3: mask = 1<<3; break;
                    default:mask = 1<<3; break;
                }
                #endif

                mask = 1<<i;

                h2rxset(0x4C, 0x0F, mask);

                h2rxedidwr(gdev.vars.VSDBOffset, &phyAB, 1);
                h2rxedidwr(gdev.vars.VSDBOffset+1, &phyCD, 1);
                phyAB = (XBH_U8)sum;
                h2rxedidwr(128+127, &phyAB, 1);
            }

        }

        h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1));
    }
}

void ChipIt66353::it6635_ddc_abort(void)
{
    XBH_U8 reg3C = h2swrd(0x3C);
    XBH_U8 i,j, uc ;

    h2swset(0x3C, 0x01, 0x01);
    for( i = 0 ;i < 2 ; i++ )
    {
        h2swwr(0x3D, 0x0F);
        for(j = 0 ; j < 50 ; j++ )
        {
            uc = h2swrd(0x1B);
            if( uc &0x80)
            {
                // DDC_FW_Stus_DONE
                break ;
            }
            if( uc & 0x38)
            {
                // DDC has something error
                PR_ERR(("ERROR: DDC 0x1B=%02X\r\n",uc));
                break ;
            }
            delay1ms(1) ;
        }
    }
    h2swwr(0x3D, 0x0A);
    h2swwr(0x3D, 0x09);
    h2swwr(0x3C, reg3C);
}

XBH_BOOL ChipIt66353::it6635_update_edid(XBH_U8 block, XBH_U8 *edid_buf, XBH_U8 flag)
{
    XBH_U8 i;
    XBH_BOOL ret;
    XBH_U8 retry=0;
__RETRY_EDID_READ:
    if (gdev.opts.tx_opt->CopyEDIDFromSink)
    {
        ret = it6635_read_one_block_edid(block, edid_buf);
        if ( XBH_FALSE == ret )
        {
            PR_ERR(("ERROR: read edid block 0\r\n"));
            if ( retry <3 )
            {
                retry++;
                it6635_ddc_abort();
                goto __RETRY_EDID_READ;
            }
            else
            {
                it66353_dev_reset();
            }
        }
    }
    else
    {
        for (i=0 ; i < 128 ; i++)
        {
            edid_buf[i] = s_default_edid_port0[i+block*128];// def_edid[i+block*128];
        }
        ret = XBH_TRUE;
    }
    return ret;
}

bool ChipIt66353::it6635_setup_edid_ram(XBH_U8 flag)
{
    XBH_U8 edid_tmp[128];
    XBH_U8 extblock;
    XBH_U8 i;

    gdev.vars.spmon = 0;
    gdev.vars.is_hdmi20_sink = 0;

    if ( XBH_FALSE == it6635_update_edid(0, edid_tmp, flag) )
    {
        goto __err_exit;
    }

    if ( (edid_tmp[0x08] == 0x5A) &&
         (edid_tmp[0x09] == 0x63) &&
         (edid_tmp[0x0a] == 0x32) &&
         (edid_tmp[0x0b] == 0x0e) )
    {
        gdev.vars.spmon = 1;
    }

    if ( (edid_tmp[0x71] == 0x4C) &&
         (edid_tmp[0x72] == 0x47) &&
         (edid_tmp[0x74] == 0x54) &&
         (edid_tmp[0x75] == 0x56) &&
         (edid_tmp[0x7F] == 0x63) )
    {
        gdev.vars.spmon = 2;
    }

    if ( (edid_tmp[0x60] == 0x48) &&
         (edid_tmp[0x61] == 0x4C) &&
         (edid_tmp[0x63] == 0x32) &&
         (edid_tmp[0x64] == 0x37) &&
         (edid_tmp[0x65] == 0x36) &&
         (edid_tmp[0x66] == 0x45) &&
         (edid_tmp[0x67] == 0x38) &&
         (edid_tmp[0x68] == 0x56) )
    {
        gdev.vars.spmon = 3;
    }

    // read Ext block no
    extblock = edid_tmp[0x7E];
    gdev.vars.EdidChkSum[0] = edid_tmp[0x7F];

    #if FIX_EDID_FOR_ATC_4BLOCK_CTS
    if ( extblock > 1 )
    {
        edid_tmp[0x7E] = 1;
        gdev.vars.EdidChkSum[0] = _rx_calc_edid_sum(edid_tmp);
    }
    #endif

    // _pr_buf(edid_tmp, 128);

    if ( gdev.opts.tx_opt->CopyEDIDFromSink )
    {
        // update EDID block 0 for all port
        it6635_set_internal_EDID(0, edid_tmp, EDID_PORT_ALL);
    }
    else
    {
        // update EDID block 0 for assigned port
        it6635_set_internal_EDID(0, edid_tmp, (1<<flag));
    }
    it6635_setup_edid_ram_step2(0);

    if ( extblock > 3 )
    {
        PR_ERR(("Warning: Extblock = %d\r\n", extblock));
        extblock = 3;
    }

    for ( i = 1; i <= extblock; i++ )
    {
        if ( XBH_FALSE == it6635_update_edid(i, edid_tmp, flag) )
        {
            goto __err_exit;
        }

        gdev.vars.VSDBOffset = 0;
        it6635_parse_edid_for_vsdb(edid_tmp);

        if ( i==1 ) // assume our sink has only 2 block EDID
        {
            if ( gdev.vars.spmon == 2 )
            {
                if ( edid_tmp[0x7F] != 0x6A)
                {
                    gdev.vars.spmon = 0;
                }
            }

            // _pr_buf(edid_tmp, 128);

            if ( gdev.opts.tx_opt->CopyEDIDFromSink )
            {
                // update EDID block 0 for all port
                it6635_set_internal_EDID(1, edid_tmp, EDID_PORT_ALL);
            }
            else
            {
                // update EDID block 0 for assigned port
                it6635_set_internal_EDID(1, edid_tmp, (1<<flag));
            }

            if ( gdev.opts.tx_opt->ParsePhysicalAddr )
            {
                it6635_parse_edid_for_phyaddr(edid_tmp);

                if ( gdev.vars.VSDBOffset )
                {
                    it6635_setup_edid_ram_step2(1);

                    // break;
                    // break parsing here will make the 4 block EDID CTS fail.
                }
            }
        }
    } // for i

    return XBH_TRUE;

__err_exit:
    return XBH_FALSE;
}

void ChipIt66353::it66353_dev_reset(void){
    PR_ERR(("it66353_dev_reset "));
    mResetPort = gdev.vars.Rx_new_port;
    dev_state = DEV_WAIT_RESET;
}

void ChipIt66353::it6635_dev_loop(void)
{
    XbhMutex::Autolock _l(mLock);
    #if CHECK_DEV_PRESENT
    if ( dev_state < DEV_WAIT_DEVICE_READY ) {
        if ( it6635_is_device_lost() )
        {
            dev_state = DEV_FW_VAR_INIT;
        }
    }
    #endif

    switch ( dev_state )
    {
        case DEV_DEVICE_LOOP:
            it6635_fsm();
            it6635_irq();
            _rx_update_ced_err_from_hw();
            break;

        case DEV_DEVICE_INIT:
            if ( it6635_device_init() )
            {
                dev_state = DEV_DEVICE_LOOP;
            }
            break;

        case DEV_WAIT_DEVICE_READY:
            if ( it6635_is_device_ready() )
            {
                dev_state = DEV_DEVICE_INIT;
            }
            break;

        case DEV_FW_VAR_INIT:
            it6635_options_init();
            dev_state = DEV_WAIT_DEVICE_READY;
            break;

        case DEV_WAIT_RESET:
            if(mRGpio >= 0)
            {
                XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
                usleep(50*1000);
                XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
            }
            dev_state = DEV_FW_VAR_INIT;
            break;

        default:
            break;
    }

}

///////

//
// APIs:
//

void ChipIt66353::it6635_dump_opts(void)
{
    PR_INFO((".rx_opt->tag1=%d\r\n", gdev.opts.active_rx_opt->tag1));
    PR_INFO((".rx_opt->EnRxDDCBypass=%d\r\n", gdev.opts.active_rx_opt->EnRxDDCBypass));
    PR_INFO((".rx_opt->EnRxPWR5VBypass=%d\r\n", gdev.opts.active_rx_opt->EnRxPWR5VBypass));
    PR_INFO((".rx_opt->EnRxHPDBypass=%d\r\n", gdev.opts.active_rx_opt->EnRxHPDBypass));
    PR_INFO((".rx_opt->TryFixedEQFirst=%d\r\n", gdev.opts.active_rx_opt->TryFixedEQFirst));
    PR_INFO((".rx_opt->EnableAutoEQ=%d\r\n", gdev.opts.active_rx_opt->EnableAutoEQ));
    PR_INFO((".rx_opt->NonActivePortReplyHPD=%d\r\n", gdev.opts.active_rx_opt->NonActivePortReplyHPD));
    PR_INFO((".rx_opt->DisableEdidRam=%d\r\n", gdev.opts.active_rx_opt->DisableEdidRam));
    PR_INFO((".rx_opt->DefaultEQ=%x %x %x\r\n", gdev.opts.active_rx_opt->DefaultEQ[0], gdev.opts.active_rx_opt->DefaultEQ[1], gdev.opts.active_rx_opt->DefaultEQ[2]));
    PR_INFO((".rx_opt->FixIncorrectHdmiEnc=%d\r\n", gdev.opts.active_rx_opt->FixIncorrectHdmiEnc));
    PR_INFO((".rx_opt->HPDOutputInverse=%d\r\n", gdev.opts.active_rx_opt->HPDOutputInverse));
    PR_INFO((".rx_opt->HPDTogglePeriod=%d\r\n", gdev.opts.active_rx_opt->HPDTogglePeriod));
    PR_INFO((".rx_opt->TxOEAlignment=%d\r\n", gdev.opts.active_rx_opt->TxOEAlignment));
    PR_INFO((".rx_opt->str_size=%d\r\n", gdev.opts.active_rx_opt->str_size));

    PR_INFO((".tx_opt->tag1=%d\r\n", gdev.opts.tx_opt->tag1));
    PR_INFO((".tx_opt->EnTxPNSwap=%d\r\n", gdev.opts.tx_opt->EnTxPNSwap));
    PR_INFO((".tx_opt->EnTxChSwap=%d\r\n", gdev.opts.tx_opt->EnTxChSwap));
    PR_INFO((".tx_opt->EnTxVCLKInv=%d\r\n", gdev.opts.tx_opt->EnTxVCLKInv));
    PR_INFO((".tx_opt->EnTxOutD1t=%d\r\n", gdev.opts.tx_opt->EnTxOutD1t));
    PR_INFO((".tx_opt->CopyEDIDFromSink=%d\r\n", gdev.opts.tx_opt->CopyEDIDFromSink));
    PR_INFO((".tx_opt->ParsePhysicalAddr=%d\r\n", gdev.opts.tx_opt->ParsePhysicalAddr));
    PR_INFO((".tx_opt->TurnOffTx5VWhenSwitchPort=%d\r\n", gdev.opts.tx_opt->TurnOffTx5VWhenSwitchPort));
    PR_INFO((".tx_opt->str_size=%d\r\n", gdev.opts.tx_opt->str_size));

    PR_INFO((".dev_opt->tag1=%d\r\n", gdev.opts.dev_opt->tag1));
    PR_INFO((".dev_opt->SwAddr=%d\r\n", gdev.opts.dev_opt->SwAddr));
    PR_INFO((".dev_opt->RxAddr=%d\r\n", gdev.opts.dev_opt->RxAddr));
    PR_INFO((".dev_opt->CecAddr=%d\r\n", gdev.opts.dev_opt->CecAddr));
    PR_INFO((".dev_opt->EdidAddr=%d\r\n", gdev.opts.dev_opt->EdidAddr));
    PR_INFO((".dev_opt->ForceRxOn=%d\r\n", gdev.opts.dev_opt->ForceRxOn));
    PR_INFO((".dev_opt->RxAutoPowerDown=%d\r\n", gdev.opts.dev_opt->RxAutoPowerDown));
    PR_INFO((".dev_opt->DoTxPowerDown=%d\r\n", gdev.opts.dev_opt->DoTxPowerDown));
    PR_INFO((".dev_opt->TxPowerDownWhileWaitingClock=%d\r\n", gdev.opts.dev_opt->TxPowerDownWhileWaitingClock));
    PR_INFO((".dev_opt->str_size=%d\r\n", gdev.opts.dev_opt->str_size));
}

#define BUF_LEN 16
void ChipIt66353::it6635_dump_register( XBH_U8 addr, char *reg_desc)
{
    XBH_U8 regbuf[BUF_LEN];
    int i,j;

    // print reg description
    PR_INFO(("%c", reg_desc));

    // print table
    PR_INFO(("   | "));
    for (j=0;j<BUF_LEN;j++)
    {
        if ( j==(BUF_LEN/2) )
        {
            PR_INFO(("- "));
        }
        PR_INFO(("%02X ", j));
    }
    PR_INFO(("\n"));

    // print split line
    for (j=0;j<BUF_LEN+2;j++)
    {
        PR_INFO(("---"));
    }
    PR_INFO(("\n"));

    // print register values
    for (i=0;i<256;i+=BUF_LEN)
    {
        PR_INFO(("%02X | ", i));
        it6635_i2c_read(gdev.opts.dev_opt->DevNo, addr, i, BUF_LEN, regbuf);
        for (j=0;j<BUF_LEN;j++)
        {
            if ( j==(BUF_LEN/2) )
            {
                PR_INFO(("- "));
            }
            PR_INFO(("%02x ", regbuf[j]));
        }
        PR_INFO(("\n"));
    }
    PR_INFO(("\n"));
}

void ChipIt66353::it6635_dump_register_all(void)
{
    it6635_dump_register(gdev.opts.dev_opt->SwAddr, "\n*** Switch Register:\n");

    it6635_dump_register(gdev.opts.dev_opt->RxAddr, "\n*** RX Register(0):\n");
    chgrxbank(3);
    it6635_dump_register(gdev.opts.dev_opt->RxAddr, "\n*** RX Register(3):\n");
    chgrxbank(5);
    it6635_dump_register(gdev.opts.dev_opt->RxAddr, "\n*** RX Register(5):\n");
    chgrxbank(0);

    // dump EDID RAM
    h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1) | 0x01); // enable EDID RAM i2c address
    h2rxset(0x4C, 0x30, 0x00);
    it6635_dump_register(gdev.opts.dev_opt->EdidAddr, "\n*** EDID Port 0:\n");
    h2rxset(0x4C, 0x30, 0x10);
    it6635_dump_register(gdev.opts.dev_opt->EdidAddr, "\n*** EDID Port 1:\n");
    h2rxset(0x4C, 0x30, 0x20);
    it6635_dump_register(gdev.opts.dev_opt->EdidAddr, "\n*** EDID Port 2:\n");
    h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1)); // disable EDID RAM i2c address

    #if EN_CEC
    it6635_dump_register(gdev.opts.dev_opt->CecAddr, "\n*** CEC Register:\n");
    #endif
}

XBH_BOOL ChipIt66353::it6635_write_edid(XBH_U8 block, XBH_U8 offset, int length, XBH_U8 *data_buffer)
{
    XBH_BOOL result = XBH_FALSE;
    int off = block * 128 + offset;
    XBH_U8 reg3C;
    XBH_U8 segment = off / 256;

    offset = off % 256;
    reg3C = h2swrd(0x3C);

    h2swset(0xF5, 0x80, (1 << 7));
    h2swset(0x3C, 0x01, 0x01); // disable DDCRegen by set RegTxMastersel=1

    h2swset(0x3C, 0x01, 0x01);    // Enable PC DDC Mode
    h2swset(0x38, 0x08, 0x08);    // Enable DDC Command Fail Interrupt
    h2swset(0x37, 0x80, 0x80);    // Enable DDC Bus Hang Interrupt

    h2swwr(0x3D, 0x09);                  // DDC FIFO Clear
    h2swwr(0x3E, 0xA0);                  // EDID Address
    h2swwr(0x3F, offset);                // EDID Offset
    h2swwr(0x40, length);                // Read ByteNum[7:0]
    h2swwr(0x41, segment / 2);           // EDID Segment

    while ( length )
    {
        h2swwr(0x42, *data_buffer);
        length--;
        data_buffer++;
    }

    h2swwr(0x3D, 0x07);              // EDID Write Fire

    if ( _tx_ddcwait() )
    {
        result = XBH_TRUE;
    }
    else
    {
        PR_ERR(("ERROR: DDC EDID Write Fail !!!\r\n"));
    }

    h2swwr(0x3C, reg3C);   // restore PC DDC Mode

    h2swset(0xF5, 0x80, (gdev.opts.active_rx_opt->EnRxDDCBypass << 7));
    if ( gdev.opts.active_rx_opt->EnRxDDCBypass == 0 )
    {
        h2swset(0x3C, 0x01, 0x00); // enable DDCRegen by set RegTxMastersel=1
    }

    return result;
}

bool ChipIt66353::it6635_write_one_block_edid(XBH_U8 block, XBH_U8 *edid_buffer)
{
    XBH_U8 offset;
    XBH_U8 i;
    XBH_U8 op_len = 16;
    offset = 0;
    for ( i = 0; i < 128 / op_len; i++ )
    {
        if ( it6635_write_edid(block, offset, op_len, edid_buffer) )
        {
            edid_buffer += op_len;
            offset += op_len;
            continue;
        }
        else
        {
            PR_ERR(("ERROR: write edid block 0, offset %d, length %d fail.\r\r\n",(int)offset,(int)op_len));
            return XBH_FALSE;
        }
    }

    return XBH_TRUE;
}

XBH_U8 ChipIt66353::it6635_is_5v_present(XBH_U8 port)
{
    if ( port < RX_PORT_COUNT )
    {
        if ( it6635_get_port_info0( port, PI_5V ) )
        {
            return 1;
        }
    }
    else
    {
        PR_ERR(("Invalid port number:%d\r\n", port));
    }

    return 0;
}

XBH_U8 ChipIt66353::it6635_is_clock_detected(XBH_U8 port)
{
    if ( port < RX_PORT_COUNT )
    {
        if ( it6635_get_port_info0( port, (PI_CLK_STABLE | PI_CLK_VALID | PI_5V) ) )
        {
            return 1;
        }
    }
    else
    {
        PR_ERR(("Invalid port number:%d\r\n", port));
    }

    return 0;
}

void ChipIt66353::it6635_set_option(IT6635_DEVICE_OPTION *Opts)
{
    if ( Opts )
    {
        //gdev.opts.EnableAutoEQ = Opts->EnableAutoEQ;
        //gdev.opts.rx_opt->EnRxDDCBypass = Opts->EnRxDDCBypass;
    }
}

void ChipIt66353::it6635_get_option(IT6635_DEVICE_OPTION *Opts)
{
    if ( Opts )
    {
        //Opts->EnableAutoEQ = gdev.opts.EnableAutoEQ;
        //Opts->EnRxDDCBypass = gdev.opts.rx_opt->EnRxDDCBypass;
    }
}

void ChipIt66353::it6635_dev_restart(void)
{
    //gdev.vars.Rx_prev_port = -1;
    mResetPort = gdev.vars.Rx_active_port;
    gdev.vars.state_sys_fsm = RX_UNPLUG;

    dev_state = DEV_WAIT_DEVICE_READY;
}

XBH_U8 ChipIt66353::it6635_get_RS(void)
{
    //return gdev.EQ.FixedRsIndex[gdev.vars.Rx_active_port];
    return 0;
}

void ChipIt66353::it6635_set_RS(XBH_U8 rs_idx0, XBH_U8 rs_idx1, XBH_U8 rs_idx2)
{
    XBH_U8 rs[3];
    if ( (rs_idx0 < 14) && ( rs_idx1 < 14 ) && ( rs_idx2 < 14 ) )
    {
        //gdev.EQ.FixedRsIndex[gdev.vars.Rx_active_port] = rs_index;

        rs[0] = RS_Value[rs_idx0] | 0x80;
        rs[1] = RS_Value[rs_idx1] | 0x80;
        rs[2] = RS_Value[rs_idx2] | 0x80;
        _rx_set_rs_3ch(rs);

        chgrxbank(3);
        h2rxbrd(0x27, 3, rs);
        //h2rxset(0x22, 0x40, 0x00);
        chgrxbank(0);
        PR_INFO(("==> RS set to %02x %02x %02x\r\n", rs[0], rs[1], rs[2]));

    }
}

void ChipIt66353::it6635_set_ch_RS(XBH_U8 ch, XBH_U8 rs_index)
{
    XBH_U8 rs;
    if ( rs_index < 14 )
    {
        rs = RS_Value[rs_index] | 0x80;
        _rx_set_rs(ch, rs);
    }
}

void ChipIt66353::it6635_set_rx_hpd(XBH_U8 hpd_value)
{
    _rx_set_hpd(gdev.vars.Rx_active_port, hpd_value, TERM_FOLLOW_HPD);
}

/**
 * it6635_set_internal_EDID - write data to EDID RAM
 *
 * @edid: 128 byte EDID data buffer.
 *
 * @block: EDID block number (0,1 or 2)
 *
 * target_port is a bitmap from 0x1 to 0xF
 *
 * ex: set port 1 EDID: target_port = EDID_PORT_1
 *     set port 1,3 EDID: target_port = EDID_PORT_1|EDID_PORT_3
 */
void ChipIt66353::it6635_set_internal_EDID( XBH_U8 block, XBH_U8 *edid, XBH_U8 target_port )
{
    int i;
    XBH_U8 wcount = 16;

    if ( block > 1 )
    {
        PR_ERR(("Invalid block %d\r\n", block));
        return;
    }

    h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1) | 0x01); // enable EDID RAM i2c address

    h2rxset(0x4C, 0x0F, target_port);  // for block 1, select port to be written

    for ( i = 0; i < 128; i += wcount )
    {
        h2rxedidwr(i + 128 * block, edid, wcount);
        edid += wcount;
    }

    h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1));  // disable EDID RAM i2c address
}

/**
 * it6635_get_internal_EDID - read data from EDID RAM
 *
 * @edid: 128 byte EDID data buffer.
 *
 * @block: EDID block number (0,1,2 or 3)
 *
 */
void ChipIt66353::it6635_get_internal_EDID( XBH_U8 block, XBH_U8 *edid, XBH_U8 target_port )
{
    int i;
    XBH_U8 wcount = 16;

    if ( block > 1 )
    {
        PR_ERR(("Invalid block %d\r\n", block));
        return;
    }
    if ( target_port > 2 )
    {
        PR_ERR(("Invalid port %d\r\n", target_port));
        return;
    }

    h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1) | 0x01); // enable EDID RAM i2c address
    h2rxset(0x4C, 0x30, target_port<<4);

    for ( i = 0; i < 128; i += wcount )
    {
        h2rxedidrd(i + 128 * block, edid, wcount);
        edid += wcount;
    }

    h2rxwr(0x4B, (gdev.opts.dev_opt->EdidAddr<<1)); // disable EDID RAM i2c address
}


/**
 * it6635_change_default_RS - set the default RS index for each
 * port
 *
 * @port: port number can be P0~P3.
 *
 * @new_rs_idx: RS index from 0 to 13
 *
 * @update_hw: 0: only update the vaule in RAM
 *             1: update the value to RAM and Hardware register
 *                ( for active port only )
 *
 */
void ChipIt66353::it6635_change_default_RS( XBH_U8 port, XBH_U8 new_rs_idx0, XBH_U8 new_rs_idx1, XBH_U8 new_rs_idx2, XBH_U8 update_hw )
{
    if ( port <= RX_PORT_3 )
    {
        gdev.opts.rx_opt[port]->DefaultEQ[0] = new_rs_idx0;
        gdev.opts.rx_opt[port]->DefaultEQ[1] = new_rs_idx1;
        gdev.opts.rx_opt[port]->DefaultEQ[2] = new_rs_idx2;
        if ( update_hw && (port == gdev.vars.Rx_active_port) )
        {
            it6635_set_RS( new_rs_idx0, new_rs_idx1, new_rs_idx2 );
        }
    }
    else
    {
        PR_ERR(("Invalid port number:%d\r\n", port));
    }
}


/**
 * it6635_force_rx_hpd :
 * to force active port HPD low or auto control by driver
 *
 * @hpd_state: 0: Force HPD of active port to low
 *             1: HPD of active port is controlled by it6635
 *                driver
 *
 * gdev.vars.force_hpd_state will reset to SW_HPD_AUTO when
 * active port changed by it6635_fsm_chg(RX_PORT_CHANGE)
 *
 */
void ChipIt66353::it6635_force_rx_hpd( XBH_U8 hpd_state )
{
    gdev.vars.force_hpd_state = hpd_state;

    if ( hpd_state ) // hpd 0 --> hpd auto
    {
        // nothing to do here:
        // hpd will be controlled by it6635_fsm()
    }
    else // hpd auto --> hpd 0
    {
        _rx_set_hpd(gdev.vars.Rx_active_port, hpd_state, TERM_FOLLOW_HPD);
        it6635_fsm_chg_delayed(RX_UNPLUG);
    }
}


/**
 * it6635_toggle_hpd : to make HPD toggle for active port with a
 * given duration.
 *
 * @ms_duration: duration of HPD low in millisecond.
 * range from 10ms to 12700ms
 *
 */
bool ChipIt66353::it6635_toggle_hpd(XBH_U16 ms_duration)
{
    XBH_U8 timeout;
    bool ret=XBH_TRUE;

    if ( ms_duration <= (0x7F*10) )
    {
        timeout = ms_duration/10;
    }
    else if ( ms_duration <= (0x7F*100) )
    {
        timeout = ms_duration/100;
        timeout |= (BIT7);
    }
    else
    {
        ret = XBH_FALSE;
    }

    _rx_set_hpd(gdev.vars.Rx_active_port, 0, TERM_FOLLOW_HPD);
    _tx_scdc_write(0x20,0x00);
    gdev.vars.hpd_toggle_timeout = timeout;
    it6635_fsm_chg(RX_TOGGLE_HPD);

    return ret;
}

//-------------------- it6635.cpp end ---------------------------


//-------------------- ChipIt66353.cpp start ---------------------------
XBH_U8 ChipIt66353::it6635_i2c_write(XBH_U32 u32I2cNum, XBH_U8 addr, XBH_U8 offset, XBH_U8 length, XBH_U8 *buffer)
{
    XBH_U32 ret = XbhService::getModuleInterface()->setI2cData(u32I2cNum, addr, offset, 0x01, length, buffer);
    if(ret == XBH_SUCCESS)
    {
        return TRUE;
    }
    it66353_dev_reset();
    PR_ERR(("ERROR:it6635_i2c_write:u32I2cNum:%d addr=0x%x,offset=0x%x,length=0x%x,buffer[0]=0x%x\n", u32I2cNum, addr, offset, length,buffer[0]));
    return FALSE;
}

XBH_U8 ChipIt66353::it6635_i2c_read(XBH_U32 u32I2cNum, XBH_U8 addr, XBH_U8 offset, XBH_U8 length, XBH_U8 *buffer)
{
    if (buffer == NULL){
        return FALSE;
    }
    XBH_U32 ret = XbhService::getModuleInterface()->getI2cData(u32I2cNum, addr, offset, 0x01, length, buffer);
    if(ret == XBH_SUCCESS)
    {
        return TRUE;
    }
    it66353_dev_reset();
    PR_ERR(("ERROR:it6635_i2c_read:u32I2cNum:%d  addr=0x%x,offset=0x%x,length=0x%x,buffer[0]=0x%x\n", u32I2cNum, addr, offset, length, buffer[0]));
    return FALSE;
}

void ChipIt66353::delay1ms(unsigned short ms)
{
    usleep(ms * 1000);
}

unsigned long ChipIt66353::get_tick_count(void)
{
    return 0;
}

ChipIt66353::ChipIt66353(XBH_U32 DevNo, XBH_U32 SwAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_U8 defaultPort, HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level)
{
    // Rx操作参数
    s_RxOpts = {
        0xC3, //XBH_U8 tag1;
        0,//XBH_U8 EnRxDDCBypass;
        0,//XBH_U8 EnRxPWR5VBypass;
        0,//XBH_U8 EnRxHPDBypass;
        1, //XBH_U8 TryFixedEQFirst;
        1,//XBH_U8 EnableAutoEQ;
        1,//XBH_U8 NonActivePortReplyHPD;
        0,//XBH_U8 DisableEdidRam;
        {DEFAULT_RS_IDX, DEFAULT_RS_IDX, DEFAULT_RS_IDX}, //XBH_U8 DefaultEQ[3];
        1,//XBH_U8 FixIncorrectHdmiEnc;
        0,//XBH_U8 HPDOutputInverse;
        HPD_TOGGLE_TIMEOUT_1SEC,//XBH_U8 HPDTogglePeriod;
        1,//XBH_U8 TxOEAlignment;
        (XBH_U8)sizeof(IT6635_RX_OPTIONS),//XBH_U8 str_size;
    };

    // Tx操作参数
    s_TxOpts = {
        0x3C,//XBH_U8 tag1;
        TX_PN_SWAP,//XBH_U8 EnTxPNSwap;
        TX_PN_SWAP,//XBH_U8 EnTxChSwap;
        0,//XBH_U8 EnTxVCLKInv;
        0,//XBH_U8 EnTxOutD1t;
        1,//XBH_U8 CopyEDIDFromSink;
        1, //XBH_U8 ParsePhysicalAddr;
        1,//XBH_U8 TurnOffTx5VWhenSwitchPort;
        (XBH_U8)sizeof(IT6635_TX_OPTIONS),//XBH_U8 str_size;
    };

    // 设备操作参数
    s_DevOpts = {
        .DevNo =  DevNo,
        .SwAddr = (XBH_U8)SwAddr,
        .RxAddr = 0xB2,
        .CecAddr = 0xC0,
        .EdidAddr = 0xA8,
        .ForceRxOn = 0,
        .RxAutoPowerDown =1 ,
        .DoTxPowerDown = 1,
        .TxPowerDownWhileWaitingClock = 0,
        .str_size = sizeof(IT6635_DEV_OPTION),
    };
    mRGpio = rGpio;
    mRLevel = rLevel;
    m_bEnable = XBH_FALSE;
    isWorking = XBH_FALSE;
    initState = XBH_TRUE;
    mDefaultPort = defaultPort;
    memcpy(&m_u8SwitchPort, &u8SwitchPort, sizeof(HDMI_SWITCH_PORT));

    it6635_options_init();
#if 0
    if (SubDev){
        PR_INFO(("ChipIt66353 for sub dev"));
        gdev.opts.rx_opt[1]->EnableAutoEQ = 0;
        gdev.opts.rx_opt[1]->NonActivePortReplyHPD = 1;
        gdev.opts.rx_opt[1]->HPDTogglePeriod = HPD_TOGGLE_TIMEOUT_400MS;
    }
#endif
    start();
}

ChipIt66353::~ChipIt66353()
{

}

void ChipIt66353::start()
{
    XbhMWThread::run(REPEAT);
}

void ChipIt66353::run(const void* arg)
{
    if(initState){
        XLOGD("[ChipIt66353 %d] reset chip !!! \n", s_DevOpts.DevNo);
        mInitStatus = 0;
        initState = XBH_FALSE;
    }
    if(isWorking == XBH_FALSE){
        if(it6635_isWorking()){
            isWorking = XBH_TRUE;
            XLOGD("[ChipIt66353 %d] find chip !!! \n", s_DevOpts.DevNo);
            return;
        }
        checkCnt++;
        if(checkCnt >= 10){
            exit();
            XLOGD("[ChipIt66353 %d] no find chip !!! \n", s_DevOpts.DevNo);
            return;
        }
        usleep(1000*1000);
    }else if(m_bEnable == XBH_FALSE){
        if(it6635_is_device_ready()){
            m_bEnable = XBH_TRUE;
        }else{
            usleep(1000*1000);
        }
    }else{
        it6635_dev_loop();
        usleep(25 * 1000);
    }
}

XBH_BOOL ChipIt66353::it6635_isWorking()
{
    XBH_U8 buffer = 0;
    return XbhService::getModuleInterface()->getI2cData(s_DevOpts.DevNo, s_DevOpts.SwAddr, 0, 0x01, 1, &buffer) == 0 ? TRUE : FALSE;
}

XBH_BOOL ChipIt66353::checkChip(){
    XLOGD("[Chip %d] check chip %d !!! \n", s_DevOpts.DevNo, isWorking);
    return isWorking;
}
//-------------------- ChipIt66353.cpp end ---------------------------
/**
 * 复位当前转换芯片
 * hw: 采用硬复位还是软复位
 */
XBH_S32 ChipIt66353::resetChip(XBH_BOOL hw)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    it66353_dev_reset();
    return s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 ChipIt66353::setRxHpd(XBH_U8 port, XBH_BOOL high)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    it6635_set_rx_hpd(high);
    return s32Ret;
}

/**
 * 切换switch的port
 * port: 指定的port
 */
XBH_S32 ChipIt66353::setActivePort(XBH_U8 port)
{
    XbhMutex::Autolock _l(mLock);
    if ( port < RX_PORT_COUNT )
    {
        if ( gdev.vars.Rx_active_port != port )
        {
            gdev.vars.Rx_new_port = port;
            it6635_fsm_chg_delayed(RX_PORT_CHANGE);
            return XBH_TRUE;
        }
    }
    else
    {
        PR_ERR(("Invalid port number:%d\r\n", port));
    }

    return XBH_FALSE;
}

/**
 * 获取switch当前切换到的port
 * port: 指定的port
 */
XBH_S32 ChipIt66353::getActivePort(XBH_U8* port)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *port = gdev.vars.Rx_active_port;
    return s32Ret;
}

/**
 * 获取switch上面连接的port的信号状态 
 * （部分场景下使用，比如有些dp转换过来的信号无法导致det pin为高电平就需要switch来判断）
 * port: 指定的port
 * status：信号状态
 */
XBH_S32 ChipIt66353::getRxPortStatus(XBH_U8 port, XBH_U8* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
	// Some graphics cards do not pull the aux pin high after the DP connector is plugged in.
    // therefore, the DP port signal is detected to determine whether DP is inserted.
    //DPSignalState = it6635_get_port_info0(2, (PI_CLK_DET | PI_CLK_VALID | PI_CLK_STABLE));
    return s32Ret;
}

