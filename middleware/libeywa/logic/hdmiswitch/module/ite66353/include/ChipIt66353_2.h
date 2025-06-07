#ifndef _CHIP_IT66353_2_H_
#define _CHIP_IT66353_2_H_

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMWThread.h"
#include "ChipIt66353Base.h"
#include "XbhHdmiSwitchInterface.h"

class ChipIt66353_2 : public XbhMWThread, public XbhHdmiSwitchInterface
{
public:
    /**
     * DevNo        使用的IIC序号
     * SwAddr       芯片IIC地址
     * pGpio        芯片供电引脚
     * pLevel       供电工作时的电平状态
     * rGpio        芯片复位引脚
     * rLevel       复位工作时的电平状态
     * defaultPort  默认端口
     * u8SwitchPort 对应的用户端口
     * SubDev       是否作为二级设备
    */
    ChipIt66353_2(XBH_U32 DevNo, XBH_U32 SwAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_U8 defaultPort, HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level);
    ~ChipIt66353_2();

    XBH_BOOL m_bEnable = XBH_FALSE;
    XBH_BOOL m_bTxOutput = XBH_FALSE;
    XBH_U8 fms_state = -1;
    void it6635_options_init();
    bool it6635_is_device_ready(void);
    void it6635_dev_loop();
    XBH_BOOL checkChip();
    XBH_BOOL it6635_isWorking();
    unsigned long get_tick_count(void);
    void start();
    void it66353_dev_reset(void);
    XBH_U8 it66353_get_init_status(void);

    XBH_S32 get_current_fsm_status(){
        return gdev.vars.state_sys_fsm;
    }

    XBH_S32 resetChip(XBH_BOOL hw);
    XBH_S32 setRxHpd(XBH_U8 port, XBH_BOOL high);
    XBH_S32 setActivePort(XBH_U8 port);
    XBH_S32 getActivePort(XBH_U8* port);
    XBH_S32 getRxPortStatus(XBH_U8 port, XBH_U8* status);

private:
//-------------------- it6635.h start ---------------------------
    char *it6635_get_lib_version(void);
    XBH_U8 it6635_i2c_write(XBH_U32 u32I2cNum, XBH_U8 addr, XBH_U8 offset, XBH_U8 length, XBH_U8 *buffer);
    XBH_U8 it6635_i2c_read(XBH_U32 u32I2cNum, XBH_U8 addr, XBH_U8 offset, XBH_U8 length, XBH_U8 *buffer);


    void it6635_setup_edid_ram_phyaddr(XBH_U8 *edid, XBH_U8 block);
    void it6635_set_internal_EDID( XBH_U8 block, XBH_U8 *edid, XBH_U8 target_port );
    void it6635_get_internal_EDID( XBH_U8 block, XBH_U8 *edid, XBH_U8 target_port );
    void it6635_parse_edid_for_phyaddr(XBH_U8 *edid);
    bool it6635_read_one_block_edid(XBH_U8 block, XBH_U8 *edid_buffer);

    void it6635_force_rx_hpd( XBH_U8 hpd_state );

    void it6635_set_option( IT6635_DEVICE_OPTION *Opts );
    void it6635_get_option( IT6635_DEVICE_OPTION *Opts );

    XBH_U8 it6635_get_RS(void);
    void it6635_set_RS(XBH_U8 rs_idx0, XBH_U8 rs_idx1, XBH_U8 rs_idx2 );
    void it6635_set_ch_RS(XBH_U8 ch, XBH_U8 rs_index);

    void it6635_dump_register_all(void);
    void it6635_dump_opts(void);

    void it6635_change_default_RS( XBH_U8 port, XBH_U8 new_rs_idx0, XBH_U8 new_rs_idx1, XBH_U8 new_rs_idx2, XBH_U8 update_hw );

    void it6635_set_rx_hpd(XBH_U8 hpd_value);
    void it6635_set_tx_5v(XBH_U8 output_value);
    bool it6635_toggle_hpd(XBH_U16 ms_duration);

    bool it6635_auto_eq_adjust(void);

    void it6635_dev_restart(void);
    void it6635_vars_init(void);
//-------------------- it6635.h end ---------------------------


//-------------------- it6635_drv.h start ---------------------------
    XBH_U8 h2swwr( XBH_U8 offset, XBH_U8 wdata );
    XBH_U8 h2swrd( XBH_U8 offset );
    XBH_U8 h2swset( XBH_U8 offset, XBH_U8 mask, XBH_U8 wdata );
    void h2swbrd( XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata );
    void h2swbwr( XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata );

    XBH_U8 h2rxwr(XBH_U8 offset, XBH_U8 wdata );
    XBH_U8 h2rxrd( XBH_U8 offset );
    XBH_U8 h2rxset( XBH_U8 offset, XBH_U8 mask, XBH_U8 dwata );
    void h2rxbrd( XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata );
    void h2rxbwr( XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata );

    XBH_U8 cecwr( XBH_U8 offset, XBH_U8 wdata );
    XBH_U8 cecrd( XBH_U8 offset );
    XBH_U8 cecset( XBH_U8 offset, XBH_U8 mask, XBH_U8 wdata );
    void cecbrd(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata);
    void cecbwr(XBH_U8 offset, XBH_U8 length, XBH_U8 *rddata);

    XBH_U8 h2rxedidwr( XBH_U8 offset, XBH_U8 *wrdata, XBH_U8 length );
    XBH_U8 h2rxedidrd(XBH_U8 offset, XBH_U8 *wrdata, XBH_U8 length);

    void chgrxbank( XBH_U8 bankno );
    void chgswbank( XBH_U8 bankno );

    bool _tx_is_sink_hpd_high(void);
    bool _tx_ddcwait(void);
    XBH_U8 _tx_scdc_write(XBH_U8 offset, XBH_U8 wdata);
    XBH_U8 _tx_scdc_read(XBH_U8 offset, XBH_U8 *data_buf);
    XBH_U8 _tx_hdcp_write(XBH_U8 offset, XBH_U8 data);
    XBH_U8 _tx_hdcp_read(XBH_U8 offset, XBH_U8 *data_buf, XBH_U8 len);
    XBH_U8 _tx_scdc_read_ced(XBH_U8 *data_buf);
    void _tx_power_down(void);
    void _tx_power_on(void);
    void _tx_show_sink_ced(void);
    void _tx_ovwr_hdmi_clk(XBH_U8 ratio);
    void _tx_ovwr_h20_scrb(XBH_U8 scrb);
    void _rx_ovwr_hdmi_clk(XBH_U8 port, XBH_U8 ratio);
    void _rx_ovwr_h20_scrb(XBH_U8 port, XBH_U8 scrb);
    void _sw_config_timer0(XBH_U8 count);
    void _sw_enable_timer0(void);
    void _sw_clear_timer0_interrupt(void);
    void _sw_enable_txoe_timer_check(void);
    void _sw_disable_txoe_timer_check(void);
    void _sw_show_hdcp_status(void);
    void _tx_ovwr_hdmi_mode(XBH_U8 mode);
    void _tx_setup_afe(XBH_U32 vclk);


    void _rx_update_ced_err_from_hw(void);
    void _rx_get_ced_err(void);
    void _rx_clear_ced_err(void);
    XBH_U8 _rx_monitor_ced_err(void);
    void _rx_DFE_enable( XBH_U8 enable );
    void _rx_set_rs_3ch( XBH_U8 *rs_value );
    void _rx_set_rs( XBH_U8 ch, XBH_U8 rs_value );

    XBH_U8 _rx_is_all_ch_symlock(void);
    XBH_U8 _rx_is_ch_symlock(XBH_U8 ch);
    XBH_U8 _rx_is_clock_stable(void);

    void _rx_auto_power_down_enable(XBH_U8 port, XBH_U8 enable);
    void _rx_term_power_down(XBH_U8 port, XBH_U8 channel);
    void _rx_handle_output_err(void);

    void _sw_disable_timer0(void);
    XBH_U8 _sw_get_timer0_interrupt(void);

    XBH_U8 _sw_is_hdcp_enabled(void);
    void _sw_clear_hdcp_status(void);
    XBH_U8 _sw_is_hdcp_auth_done(void);

    //--------------------------------
    void it6635_txoe(XBH_U8 enable);
    void it6635_auto_detect_hdmi_encoding(void);
    void it6635_fix_incorrect_hdmi_encoding(void);

    XBH_U8 it6635_get_port_info1( XBH_U8 port, XBH_U8 info );
    XBH_U8 it6635_get_port_info0(XBH_U8 port, XBH_U8 info);

    void it6635_init_rclk(void);
    void it6635_enable_tx_port(XBH_U8 enable);

    // --------------
    void it6635_sys_state(XBH_U8 new_state);
    void _rx_reset( void );
    void _rx_caof_init(XBH_U8 port);
	void it6635_wait_for_ddc_idle(void);
	void _rx_pll_reset(void);
	void it6635_ddc_abort(void);
//-------------------- it6635.h end ---------------------------



private:

//-------------------- it6635_drv.h start ---------------------------
    void _eq_save_h20(void);
    void _eq_load_h20(void);
    void _eq_save_h14(void);
    void _eq_load_h14(void);
    void _eq_load_previous(void);
    void _eq_load_default(void);


    void _eq_reset_state(void);
    void _eq_set_state(XBH_U8 state);
    XBH_U8 _eq_get_state(void);
    void _eq_reset_txoe_ready(void);
    void _eq_set_txoe_ready(XBH_U8 ready);
    XBH_U8 _eq_get_txoe_ready(void);

    void _aeq_set_DFE2(XBH_U8 EQ0, XBH_U8 EQ1, XBH_U8 EQ2);

    XBH_U8 _rx_is_hdmi20(void);

    void _aeq_diable_eq_trigger();
    XBH_U8 _aeq_check_sareq_result(void);
    void _aeq_chg( XBH_U8 new_state );
    XBH_U8 _aeq_fsm(void);


    void it6635_fsm_chg( XBH_U8 new_state );
    void it6635_fsm_chg_delayed( XBH_U8 new_state );

    void __it6635_fsm_chg(XBH_U8 new_state, int caller);
    void __it6635_fsm_chg2(XBH_U8 new_state, int caller);
    bool it6635_device_init(void);
    bool it6635_device_init2(void);

    bool it6635_read_edid(XBH_U8 block, XBH_U8 offset, int length, XBH_U8 *edid_buffer);
    bool it6635_write_one_block_edid(XBH_U8 block, XBH_U8 *edid_buffer);
    bool it6635_setup_edid_ram(XBH_U8 flag);

    void it6635_force_hdmi20(void);

    void _rx_skew_adj(XBH_U8 ch);
//-------------------- it6635_drv.h end ---------------------------


//-------------------- it6635_EQ.cpp start ---------------------------
    XBH_U8 _rx_is_h14_tmds_over_1G(void);
    XBH_U8 _rx_get_rs( XBH_U8 ch );
    XBH_U8 _meq_14(void);
    XBH_U8 _aeq_check_h14eq_result(void);
    void _aeq_check_amp_and_rs_status(void);
    void _aeq_trigger_SAREQ(void);
    void _aeq_trigger_EQ(void);
    void _aeq_get_calc_rs(void);
    void _aeq_get_DFE2(XBH_U8 ch, XBH_U8 rs_idx);
    void _aeq_reset(void);
    XBH_U8 _aeq_ready_to_start(void);
    XBH_U8 _eq_find_rs_index(XBH_U8 EQ_Value);
    void _rx_record_ced_err(void);
    void _rx_ced_err_recheck( void );
//-------------------- it6635_EQ.cpp end ---------------------------

    void delay1ms(unsigned short ms);
    void _pr_buf(void *buffer, int length);

    XBH_U8 _rx_calc_edid_sum(XBH_U8 *edid );
    XBH_U8 _rx_is_any_ch_symlock(void);
    XBH_U8 _rx_need_hpd_toggle(void);
    //XBH_U8 _rx_get_all_port_5v(void);
    void _rx_show_ced_info(void);
    void _rx_setup_afe(XBH_U32 vclk);
    bool _rx_is_5v_active(void);
    void _rx_int_enable(void);
    void _rx_wdog_rst(XBH_U8 port);
    void _rx_ovwr_hdmi_mode(XBH_U8 port, XBH_U8 mode);
    void _rx_set_hpd(XBH_U8 port, XBH_U8 hpd_value, XBH_U8 term_value);
    void _rx_set_hpd_all(XBH_U8 value);
    void _rx_set_hpd_with_5v_all(XBH_U8 non_active_port_only);
    void _rx_auto_power_down_enable_all(XBH_U8 enable);
    void _sw_int_enable(XBH_U8 port, XBH_U8 enable);
    void _sw_int_enable_all(XBH_U8 enable);
    void _sw_config_timer1(XBH_U8 count);
    void _sw_enable_timer1(void);

    XBH_U8 _sw_get_timer1_interrupt(void);
    XBH_U8 _sw_check_hpd_toggle_timer(void);
    void _sw_disable_timer1(void);
    void _sw_clear_timer1_interrupt(void);
    void _sw_enable_hpd_toggle_timer(XBH_U8 timeout);
    void _sw_disable_hpd_toggle_timer(void);
    void _sw_reset_scdc_monitor(void);
    void _sw_monitor_and_fix_scdc_write(void);
    void _sw_sdi_check(void);
    void _sw_hdcp_access_enable(XBH_U8 enable);
    void _tx_init(void);
    void _tx_reset(void);
    void _rx_init(void);
    void _sw_init(void);
    XBH_U32 it6635_get_rx_vclk(XBH_U8 port);
    void _sw_reset(void);
    void it6635_auto_txoe(XBH_U8 enable);
    void it6635_detect_port(XBH_U8 port);
    void it6635_detect_ports(void);
    void it6635_rx_irq(void);
    void it6635_sw_irq(XBH_U8 port);
    void it6635_tx_irq(void);
    void it6635_force_hdmi14(void);
    void it6635_fsm_EQ_check(void);
    void it6635_fsm(void);
    void it6635_irq(void);
    void it6635_parse_edid_for_vsdb(XBH_U8 *edid);
    void it6635_setup_edid_ram_step2(XBH_U8 block);
    XBH_BOOL it6635_update_edid(XBH_U8 block, XBH_U8 *edid_buf, XBH_U8 flag);
    void it6635_dump_register( XBH_U8 addr, char *reg_desc);
    XBH_BOOL it6635_write_edid(XBH_U8 block, XBH_U8 offset, int length, XBH_U8 *data_buffer);
    XBH_U8 it6635_is_5v_present(XBH_U8 port);
    XBH_U8 it6635_is_clock_detected(XBH_U8 port);

private: 
    IT6635_DEVICE_DATA gdev;
    IT6635_RX_OPTIONS s_RxOpts;
    IT6635_TX_OPTIONS s_TxOpts;
    IT6635_DEV_OPTION s_DevOpts;
    XbhMutex  mLock;
    XBH_U8 g_is_set_io = 0;
    XBH_U8 dev_state = DEV_FW_VAR_INIT;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_BOOL isWorking;
    XBH_U32 checkCnt;
    XBH_BOOL initState;
    XBH_U32 errorState;
    XBH_U8 mDefaultPort;
    HDMI_SWITCH_PORT m_u8SwitchPort;

private:
    void run(const void* arg);
};


#endif //_CHIP_IT66353_2_H_

