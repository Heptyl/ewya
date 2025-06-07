#ifndef _IT6265_H_
#define _IT6265_H_

#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "It6265Base.h"
#include "XbhCommon.h"
#include "XbhService.h"

class It6265 : public XbhMWThread
{
public:
    #define  HDMI_OUT_TIMING_TO_SIGNAL
    #define  HDMI_OUT_ON        0
    #define  HDMI_OUT_OFF       1
    #define  HDMI_OUT_HDCP_ON   1
    #define  HDMI_OUT_HDCP_OFF  0
    #define  HDMI_OUT_480P      0
    #define  HDMI_OUT_720P      1
    #define  HDMI_OUT_1080P     2
    #define  HDMI_OUT_RESET     3

    #define  STEP_READY         0
    #define  STEP_INIT          1
    #define  STEP_TIMING        2
    #define  STEP_RUNING        3

    It6265(XBH_U32 i2cBus, XBH_U32 ResetGPIO, XBH_U32 ResetLevel);
    ~It6265();
    It6265() = delete;

    void start();
    XBH_VOID initHdmiOut();
    XBH_S32  setIte6265Timing480p();
    XBH_S32  setIte6265Timing720p();
    XBH_S32  setIte6265Timing1080p();
    XBH_S32  resetIte6265Timing();

    XBH_VOID setHdmiOutEnable(XBH_U32 onOff);
    XBH_U32  getHdmiOutEnable();
    XBH_VOID setHdmiOutSoundEnable(XBH_U32 onOff);
    XBH_U32  getHdmiOutSoundEnable();
    XBH_VOID setHdmiOutHdcpEnable(XBH_U32 onOff);
    XBH_U32  getHdmiOutHdcpEnable();
    XBH_VOID setHdmiOutResolution(XBH_U32 resolution);
    XBH_U32  getHdmiOutResolution();

    int it6265_set_video_format_by_vic( void *Dev, unsigned short vic, unsigned char byte_mode, unsigned char lane_count );
    int it6265_hdcp_enable( int enable_hdcp );
    void it6265_enable_hdcp1( void );
    void it6265_enable_hdcp2( void );
    XBH_BOOL checkIte6265Timing ( void );

    XBH_U32 m_step = 0;
    XBH_BOOL m_bEnable = XBH_FALSE;
    XBH_BOOL m_initState = XBH_FALSE;
    XBH_U32 hdcpEnable = 0;
private:
//-------------------- it6265.c start ---------------------------
    u8 hdmitxrd(u8 offset);
    void hdmitxwr(u8 offset, u8 rddata);
    void hdmitxset(u8 offset, u8 mask, u8 rddata);
    void hdmitxbrd(u8 offset, u8 length, u8 *rddata);
    void vborxwr(u8 offset, u8 rddata);
    u8 vborxrd(u8 offset);
    void vborxset(u8 offset, u8 mask, u8 rddata);
    void vborxbrd(u8 offset, u8 byteno, u8 *rddata);
    void cecwr(u8 offset, u8 rddata);
    u8 cecrd(u8 offset);
    void cecset( u8 offset, u8 mask, u8 rddata );
    void chgrxbank(u8 bankno);
    void chgtxbank(u8 bankno);
    void it6265_dump_register( u8 addr, char *reg_desc);
    void it6265_dump_register_all(void);
    void it6265_print_buffer( void *buffer, int length );
    const VTiming *it6265_find_timing_by_vic(u16 vic);
    bool it6265_get_timing_by_vic(int vic);
    bool _value_in_range( u32 value, u32 comp, int tor_percent );
    int it6265_get_vic_by_timing(VTiming *CurVTiming);
    bool it6265_is_video_in_stable( void );
    bool it6265_is_rx_sense( void );
    bool it6265_is_hpd_high( void );
    bool it6265_disable_hdcp2(void);
    void it6265_enable_timestamp( u8 active );
    void it6265_set_vx1_htpdn(u8 active);
    void it6265_set_avmute(u8 mute);
    void it6265_ddc_abort( void );
    void it6265_ddc_fifo_clear( void );
    void it6265_ddc_status_check(void);
    void it6265_scdc_status_check( void );
    bool it6265_ddcwait(void);
    bool it6265_scdc_write(u8 offset, u8 scdc_data);
    bool it6265_scdc_read(u8 offset, u8 bytenum);
    bool it6265_hdcp_read(u8 offset, u16 bytenum);
    u8 it6265_vborx_lane_detect(void);
    void it6265_check_vborx_fifo(void);
    void it6265_gate_rclk(u8 gating);
    void it6265_power_on( void );
    void it6265_power_down( void );
    bool it6265_is_valid_rclk( u32 rclk );
    void it6265_set_calib_data( u32 rclk );
    void it6265_load_rclk( void );
    void it6265_cal_rclk( void );
    void it6265_cal_vclk( void );
    void it6265_set_asclk(void);
    void it6265_assign_txclk( u32 PixelClock );
    void it6265_cal_txclk( void );
    void it6265_ddc_reset(void);
    void it6265_set_pattern_gen(u16 vic);
    void it6265_set_syncde_gen(u16 vic);
    void it6265_setup_vborx( void );
    void it6265_setup_hdmitx_afe( void );
    void it6265_set_infoframe( void );
    void it6265_set_csc_matrix( const u8 *csc_matrix );
    void it6265_set_video_format( void );
    void it6265_config_hdmi_pg(void);
    void it6265_set_audio_format( void );
    void it6265_scdc_cancel_rr(void);
    bool it6265_scdc_set_rr(u8 rr_enable);
    bool it6265_read_edid(u8 block, u8 offset, int length, u8 *edid_buffer);
    bool it6265_read_one_block_edid(u8 block, u8 *edid_buffer);
    void it6265_reset_vbotx( void );
    bool it6265_edid_parse( void );
    void it6265_hdcp_show_ri( void );
    void it6265_hdcp_show_pj( void );
    bool it6265_hdcp_is_revocation_key(u8 *BKSV);
    bool it6265_hdcp_is_revocation_rxid(u8 *RxID);
    void it6265_hdcp1_show_status( void );
    void it6265_hdcp2_show_status( void );
    void it6265_hdcp_auth_fail( void );
    void it6265_set_audio_option( void );
    void it6265_setup_hdmi2_scdc( void );
    bool it6265_check_audio_config( void );
    void it6265_audio_state(AudState_Type NewState);
    void it6265_show_vbo_input_info( void );
    void it6265_show_vbo_output_info( void );
    void it6265_show_audio_cts(void);
    void it6265_show_txvid_info( void );
    void it6265_video_state(VidState_Type NewState);
    void it6265_hdcp_state(HdcpState_Type CPState);
    void it6265_reset( u8 rst_all );
    void it6265_sys_state(SysState_Type NewState);
    void it6265_ddc_error_handler( void );

    u8 softi2crd(int offset, int length);
    void softi2cwr(u8 data);
    void softhdcprd(u8 offset, u8 length, u8 *databuf);
    bool it6265_hdcp_readksv(u8 offset,u16 bytenum, u8* KSVLIST);
    void SHA1Transform(u32 state[5], u8 buffer[64]);
    void SHA1Init(SHA1_CTX *context);
    void SHA1Update(SHA1_CTX *context, u8 *data, u32 len);


    void it6265_sha_transform( u32 *h, u32 *w );
    void it6265_sha_simple( u8 *p, int msgcnt , u8 *output);
    bool it6265_hdcp_repeater_check( void );
    void it6265_timer_task( void );
    void it6265_hdcp_task_retry_auth( void );
    void it6265_hdcp_task_check_ksv_list( void );
    void it6265_hdcp_task_polling_bcaps( void );
    void it6265_avi_task_wait_for_audio_stable( void );
    void it6265_avi_task_sys_unplug( void );
    void it6265_avi_task_show_audio_info( void );
    void it6265_avi_task_check_video_in_stable( void );
    void it6265_irq_vborx_reg10(u8 RxReg10);
    void it6265_irq_vborx_reg11(u8 RxReg11);
    void it6265_irq_vborx_reg12(u8 RxReg12);
    void it6265_irq_vborx_reg13(u8 RxReg13);
    void it6265_irq_hdmitx_reg12(u8 TxReg12);
    void it6265_irq_hdmitx_reg13(u8 TxReg13);
    void it6265_irq_hdmitx_reg16(u8 TxReg16);
    void it6265_scdc_update_flag_status_handler( u8 rddata );
    void it6265_scdc_update_flag_handler( void );
    void it6265_scdc_rr_handler( void );
    void it6265_irq_hdmitx_reg17(u8 TxReg17);
    void it6265_irq(void);
    bool it6265_set_output_format( it6265_video_fmt format );
    void it6265_init_vars( void );
    bool it6265_is_device_ready(void);
    void it6265_chip_init(void);
    bool it6265_hdcp2_get_rx_status( unsigned short *rx_status);
    void it6265_hdcp2_continue_stream_manage(void);
    void it6265_hdcp2_continue_SEK_send_Eks(void);
    void it6265_enable_480p_pattern_gen(void);
//--------------------  it6265.c end   ---------------------------

//--------------------  it6265_cec.c start   ---------------------------
    void cbInit(circular_buffer *cb, int size, void *data_queue);
    void cbPrint(circular_buffer *cb);
    int cbIsFull(circular_buffer *cb);
    int cbIsEmpty(circular_buffer *cb);
    int cbIncr(circular_buffer *cb, int p);
    void* cbAdd(circular_buffer *cb);
    void *cbRemove(circular_buffer *cb);
    void cecbrd( u8 offset, u8 byteno, u8 *rddata );
    void cecbwr( u8 offset, u8 byteno, u8 *rddata );
    char Cec_SwitchLA(void);
    void Cec_RxFifoReset(void);
    int Cec_RxCmdGet(void);
    void Cec_TxFire(void);
    void Cec_send(CEC_FRAME *cec);
    void Cec_TxPolling(u8 u8LogicalAdr);
    void Cec_TxFeatureAbort(u8 Follower, u8 CecRxCmd, CecAbortReson eAbortReson);
    void Cec_TxReportPA(void);
    void Cec_rx_cmd_handler(void);
    void Cec_tx_cmd_handler(void);
    void Cec_Irq(void);
    void it6265_cec_init(u8 phyAddrL, u8 phyAddrH, u32 ulClkCnt);
    void it6265_cec_tx_polling(void);
    void it6265_cec_maganer(void);
    char it6265_cec_send( u8 opcode, u8 *operand, u8 operand_len, u8 follower);
//--------------------  it6265_cec.c end   ---------------------------

//--------------------  it6265_edid.c start   ---------------------------
    void parseHDMI1VSDB(u8 VSDB[]);
    void parseHDMI2VSDB(u8 VSDB[]);
    void parseCEAExtensionDescriptor(u8 Descriptor[]);
    void parseVideoCapabilityBlock(u8 Descriptor[]);
    void parseVendorSpecificVideoDataBlock(u8 Descriptor[]);
    void parseVesaDisplayDeviceDataBlock(u8 Descriptor[]);
    void parseVesaTimingDataBlock(u8 Descriptor[]);
    void parseColorDataBlock(u8 Descriptor[]);
    void parseVideoFormatPreferenceDataBlock(u8 Descriptor[]);
    void parseYCC420VideoDataBlock(u8 Descriptor[]);
    void parseYCC420MapDataBlock(u8 Descriptor[]);
    int checksumEDID(u8 EDID[]);
    void parseDTD(u8 *dtd);
    void parseDisplayLimitDescriptor(u8 *descriptor);
    void parseMonitorDescriptor(u8 *descriptor);
    void ParseColorCharateristic(unsigned char *pCh);
    void parseVSDB(u8 VSDB[]);
    bool it6265_parse_edid_block0( u8 *EDID );
    bool it6265_parse_CEA_Block( u8 *EDID );
//--------------------  it6265_edid.c end     ---------------------------

//-------------------- it6265_io.c start ---------------------------
    void idle(u16 ms);
    XBH_U8 it6265_i2c_write(u8 addr, u8 offset, u8 length, u8 *buffer);
    XBH_U8 it6265_i2c_read(u8 addr, u8 offset, u8 length, u8 *buffer);
//-------------------- it6265_io.c end   ---------------------------

private:
    void run(const void* arg);

private:
    XBH_U32 Device_I2CBus;
    XBH_U32 Reset_GPIO;
    XBH_U32 Reset_Level;
};

#endif