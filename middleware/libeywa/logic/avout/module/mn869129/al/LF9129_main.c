/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#include <stdlib.h>
#include <stdint.h>
#include "hv.h"
#include "misc.h"
#include "reg_access.h"        // I2C Access Functions Header

#include "hv_version.h"
#include "hv_print.h"
#include "opts.h"
#include "i2c_fa.h"
#include "input.h"
#include "hv_error.h"

extern XBH_S32                      xbh_vbo2hdmi_reset_key;
uint8_t xbh_9129_is_repeater = 0;

int set_hdmiout_res_value = 2;
int is_hdcp_enable = 1;
static int hpd_active_state = 0;
static int current_hdmitx_hdcp_mute = -1;
int current_hdcp_version = 1;
int edid_read = 0;
static uint32_t auto_hdmitx_hdcp_thread_state = 0;
extern XBH_BOOL         mAVMute;
extern XBH_U8           mMaxVideoFormat;
extern XBH_U8           mCurrVideoFormat;
extern XBH_U8           mCurrent_hdmitx_video_format;
pthread_t hdcptx_thread_run = 1;
uint32_t ext_block_number = 0; // 除block0和block1之外，还有多少个block


typedef union {
    hdmitx_video    htx;
    system_video    sys;
} system_videos;

//------------------------------------------------------------------------------
// Prototypes and Definitions
//------------------------------------------------------------------------------
static uint32_t auto_process_hdmitx(system_port p_txp);
static uint32_t set_hpd_thread_hdmitx(system_port p_txp);
static uint32_t init_hdmitx_settings();
static uint32_t modeChange;
static uint32_t hdmiStatus;
static uint32_t initOK;

void i2c_info(void);
static uint32_t cmd_reg(cmds_main *cmd);
static uint32_t cmd_preset(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_preset_video(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_audio(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                    system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_preset_edid(cmds_main *cmd);
static uint32_t cmd_preset_chip(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_preset_video_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_video_vbo(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_video_lvds(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_audio_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                        system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_preset_video_hdmitx(cmds_main *cmd, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_video_hdmirx(cmds_main *cmd, system_port *p_rxp, system_videos*p_vform);
static uint32_t cmd_preset_video_vbotx(cmds_main *cmd, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_video_vborx(cmds_main *cmd, system_port *p_rxp, system_videos *p_vform);
static uint32_t cmd_preset_video_lvdstx(cmds_main *cmd, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_preset_audio_hdmitx(cmds_main *cmd, system_port *p_txp, system_audio *p_aform);
//------------------------------------------------------------------------------
static uint32_t cmd_config(cmds_main *cmd);
static uint32_t cmd_config_hdmi(cmds_main *cmd);
static uint32_t cmd_config_vbo(cmds_main *cmd);
static uint32_t cmd_config_hdmitx(cmds_main *cmd);
static uint32_t cmd_config_hdmirx(cmds_main *cmd);
static uint32_t cmd_config_vbotx(cmds_main *cmd);
static uint32_t cmd_config_vborx(cmds_main *cmd);
//------------------------------------------------------------------------------
static uint32_t cmd_init(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_init_hdmi(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_init_vbo(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_init_lvds(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_init_spdif(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_init_i2s(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_init_hdmitx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_hdmirx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_vbotx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_vborx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_lvdstx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_spdiftx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_spdifrx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_i2stx(cmds_main *cmd, system_port port);
static uint32_t cmd_init_i2srx(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_read(cmds_main *cmd, system_port txp);
static uint32_t cmd_read_edid(cmds_main *cmd, system_port port);
static uint32_t cmd_read_scdc(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_write(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_write_edid(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_write_edid_sram(cmds_main *cmd, system_port port);
static uint32_t cmd_write_edid_via_hdmi_tx(cmds_main *cmd, system_port port);
static uint32_t cmd_write_scdc(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_set(cmds_main *cmd, system_videos *p_vform, system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_set_video(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_hdmi(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_hdmitx(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_vbo(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_vbotx(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_vborx(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_lvds(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_video_lvdstx(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_set_audio(cmds_main *cmd, system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_set_audio_hdmi(cmds_main *cmd, system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_set_audio_hdmitx(cmds_main *cmd, system_audio *p_aform);
static uint32_t cmd_set_hdcp(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_set_hdcptx(cmds_main *cmd, system_port port);
static uint32_t cmd_set_hdcprx(cmds_main *cmd, system_port port);
static uint32_t cmd_set_spd_infoframe(cmds_main *cmd, system_port port);
static uint32_t cmd_set_hdr_infoframe(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_get(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_get_video(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_get_video_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform);
static uint32_t cmd_get_video_hdmirx(cmds_main *cmd, system_port *p_rxp, system_videos *p_vform);
static uint32_t cmd_get_audio(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_get_audio_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                    system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_get_audio_hdmirx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                        system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_get_audio_spdif(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                    system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_get_audio_spdifrx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                        system_audio *p_rx_aform, system_audio *p_tx_aform);
//------------------------------------------------------------------------------
static uint32_t cmd_get_status(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_status_hpd(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_status_hdcp(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_status_hdcptx(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_status_hdcprx(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_status_port(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_status_av(cmds_main *cmd, system_videos *p_vform);
//------------------------------------------------------------------------------
static uint32_t cmd_get_format(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_format_hdmi(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_format_hdmi_rx(cmds_main *cmd, system_videos *p_vform);
static uint32_t hdmirx_detect_video_format(system_port port);
static uint32_t cmd_get_format_vbo(cmds_main *cmd, system_videos *p_vform);
static uint32_t cmd_get_format_vbo_rx(cmds_main *cmd, system_videos *p_vform);
static uint32_t vborx_start_detect_video_format(system_port port);
static uint32_t vborx_detect_video_format(system_port port);
static uint32_t vborx_stop_detect_video_format(system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_reset(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_reset_hdcp(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_reset_hdcptx(cmds_main *cmd, system_port port);
static uint32_t cmd_reset_hdcprx(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_select(cmds_main *cmd, system_port *p_rxp);
static uint32_t cmd_select_port_hdmirx(cmds_main *cmd, system_port *p_port);
//------------------------------------------------------------------------------
static uint32_t cmd_mute(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_video(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_video_hdmi(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_video_hdmitx(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_hdmitx_system(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_hdmitx_avmute(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_vbo(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_video_vbotx(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_vbotx_black(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_vbotx_system(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_lvds(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_video_lvdstx(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_lvdstx_system(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_video_lvdstx_black(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_audio(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_audio_hdmi(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_audio_spdif(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_audio_i2s(cmds_main *cmd, system_port txp);
static uint32_t cmd_mute_audio_hdmitx(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_audio_spdiftx(cmds_main *cmd, system_port port);
static uint32_t cmd_mute_audio_i2stx(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_unmute(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_video(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_video_hdmi(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_video_hdmitx(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_hdmitx_system(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_hdmitx_avmute(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_vbo(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_video_vbotx(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_vbotx_system(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_vbotx_black(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_lvds(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_video_lvdstx(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_lvdstx_system(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_video_lvdstx_black(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_audio(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_audio_hdmi(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_audio_spdif(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_audio_i2s(cmds_main *cmd, system_port txp);
static uint32_t cmd_unmute_audio_hdmitx(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_audio_spdiftx(cmds_main *cmd, system_port port);
static uint32_t cmd_unmute_audio_i2stx(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static uint32_t cmd_abort(cmds_main *cmd);
static uint32_t cmd_abort_hdmi(cmds_main *cmd);
static uint32_t cmd_abort_hdmitx(cmds_main *cmd);
static uint32_t cmd_abort_hdmirx(cmds_main *cmd);
static uint32_t cmd_abort_hdcp(cmds_main *cmd);
static uint32_t cmd_abort_hdcptx(cmds_main *cmd);
static uint32_t cmd_abort_hdcprx(cmds_main *cmd);
static uint32_t cmd_abort_vbo(cmds_main *cmd);
static uint32_t cmd_abort_vbotx(cmds_main *cmd);
static uint32_t cmd_abort_vborx(cmds_main *cmd);
static uint32_t cmd_abort_lvds(cmds_main *cmd);
static uint32_t cmd_abort_lvdstx(cmds_main *cmd);
//------------------------------------------------------------------------------
static uint32_t cmd_sleep_main(cmds_main *cmd);
static uint32_t gettimestamp(long *sec, long *nsec);
static uint32_t getmonotonictime(long *sec, long *nsec, int is_clear);
static uint32_t cmd_time(cmds_main *cmd);
//------------------------------------------------------------------------------
static uint32_t cmd_auto(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_auto_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_auto_hdmitx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_auto_hdmitx_sg(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_auto_hdmitx_hdcp(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_auto_hdmirx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
//------------------------------------------------------------------------------
static uint32_t cmd_misc(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform);
static uint32_t cmd_misc_vborx_set_hsync(cmds_main *cmd, int cnt, system_port port);
static uint32_t cmd_misc_hdmitx_set_sg_format(cmds_main *cmd, int cnt, system_videos *p_vform);
//------------------------------------------------------------------------------
static uint32_t cmd_exit(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_exit_hdmi(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_exit_vbo(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_exit_lvds(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_exit_spdif(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_exit_i2s(cmds_main *cmd, system_port rxp, system_port txp);
static uint32_t cmd_exit_hdmitx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_hdmirx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_vbotx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_vborx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_lvdstx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_spdiftx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_spdifrx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_i2stx(cmds_main *cmd, system_port port);
static uint32_t cmd_exit_i2srx(cmds_main *cmd, system_port port);
//------------------------------------------------------------------------------
static void version(void);
static void dump_bin(uint8_t *data, uint16_t size);
static uint32_t get_video_format(opt_resolution res, opt_fps fps, system_video_format *p_vform);
static uint32_t get_video_format_str(system_video_format vform, char **p_str);
static uint32_t get_color_format(opt_color_format cf, system_color_format *p_cform);
static uint32_t get_color_format_str(system_color_format cform, char **p_str);
static uint32_t get_deep_color(opt_deep_color dc, system_deep_color *p_dcolor);
static uint32_t get_deep_color_str(system_deep_color dcolor, char **p_str);
static uint32_t print_video_format(system_video_format vform, system_color_format cform, system_deep_color dcolor);
static uint32_t get_audio_cc(opt_cc cc, system_audio_cc *p_cc);
static uint32_t get_audio_sf(opt_freq freq, system_audio_sf *p_sf);
static uint32_t get_audio_ss(opt_ss ss, system_audio_ss *p_ss);
static uint32_t get_audio_sf_ratio(opt_freq_ratio freq_ratio, system_audio_sf_ratio *p_sfr);
static uint32_t get_audio_em(opt_em em, system_audio_em *p_em);
static uint32_t get_audio_po(opt_po po, system_audio_po *p_po);
static uint32_t get_audio_if(opt_if i2sf, system_audio_if *p_if);
static uint32_t get_audio_sp(opt_sp sp, system_audio_sp *p_sp);
static uint32_t get_audio_cc_str(system_audio_cc cc, char **p_str);
static uint32_t get_audio_sf_str(system_audio_sf sf, char **p_str);
static uint32_t get_audio_ss_str(system_audio_ss ss, char **p_str);
static uint32_t get_audio_sf_ratio_str(system_audio_sf_ratio sfr, char **p_str);
static uint32_t get_audio_em_str(system_audio_em em, char **p_str);
static uint32_t get_audio_po_str(system_audio_po po, char **p_str);
static uint32_t get_audio_if_str(system_audio_if i2sf, char **p_str);
static uint32_t get_audio_sp_str(system_audio_sp sp, char **p_str);
static uint32_t print_audio_format(system_audio *p_aform);
static uint32_t get_init_vbo(opt_init_opt init_opt, system_inversion *p_inv);
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
static uint32_t get_init_lvds(opt_init_opt init_opt, lvdstx_data_format *p_df);
#endif
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H27BJ2)
static uint32_t read_edid_txt(const char *file, uint8_t *data, uint32_t *size);
#endif
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
static uint32_t print_scdc(hdmitx_scdc *scdc);
#endif
//------------------------------------------------------------------------------
// Prototypes and Definitions for I2C free access
//------------------------------------------------------------------------------
static uint32_t console_i2c(void);
static uint32_t cmd_i2cr(cmds_i2c *cmd);
static uint32_t cmd_i2cw(cmds_i2c *cmd);
static uint32_t cmd_i2cp(cmds_i2c *cmd);
static uint32_t cmd_sleep(cmds_i2c *cmd);
static uint32_t cmd_wait(cmds_i2c *cmd);

uint32_t do_reset_auther_hdcp(void);

//------------------------------------------------------------------------------
// EDID Buffer
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H27BJ2)
static uint8_t edid_table[DDC_MAX_BUF_SIZE] = {
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t edid_extend_table[7][DDC_MAX_BUF_SIZE/2];
#endif
//------------------------------------------------------------------------------
// SPD InfoFrame
//------------------------------------------------------------------------------
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || \
    defined(CONFIG_HV_SC1H27BJ2)
static system_spd_infoframe spd_infoframe;
#endif
//------------------------------------------------------------------------------
// HDR InfoFrame
//------------------------------------------------------------------------------
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || \
    defined(CONFIG_HV_SC1H27BJ2)
static system_hdr_infoframe hdr_infoframe;
#endif
//------------------------------------------------------------------------------
// Auto HDCP control
//------------------------------------------------------------------------------
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
#define DISABLE_UNMUTE                    0
#define ENABLE_UNMUTE                    1
static uint8_t hdcptx_auth_auto_unmute = ENABLE_UNMUTE;
#endif
//------------------------------------------------------------------------------
// Definitions for Hot Plug Detect Status
//------------------------------------------------------------------------------
#define HDMITX_EVENT_HPD_MASK            0x01
#define HDMITX_EVENT_HPD_ACTIVE            0x01
#define HDMITX_EVENT_HPD_INACTIVE        0x00
#define HDMITX_EVENT_SENSE_MASK            0x02
#define HDMITX_EVENT_SENSE_ACTIVE        0x02
#define HDMITX_EVENT_SENSE_INACTIVE        0x00
//------------------------------------------------------------------------------
// Definitions for Hot Plug Detect Interrupt
//------------------------------------------------------------------------------
#define HDMITX_INTR_HPD_NEGATIVE        0x01
#define HDMITX_INTR_HPD_POSITIVE        0x02
#define HDMITX_INTR_SENSE_NEGATIVE        0x04
#define HDMITX_INTR_SENSE_POSITIVE        0x08
//------------------------------------------------------------------------------
// String
//------------------------------------------------------------------------------
#define API_ERROR_OCCURRED                "%s error has occurred (%s)."
const char COMMAND_ERROR_OCCURRED[] =    "command error has occurred.";
const char UNSUPPORTED_COMMAND[] =        "unsupported command";
const char LAST_ERROR[] =                "  ... last error:";
//------------------------------------------------------------------------------
// Definitions for Terminal Console
//------------------------------------------------------------------------------
#if defined(_WIN32) || defined(__linux) || (1)
    #define INPUT(buf, len)        input(buf, len)
#else
    #error "unsupported compiler"
#endif
#define PROMPT()            fprintf(stdout, "> ")
//------------------------------------------------------------------------------
// Variables for HDCP Authentication
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
static hdmitx_hdcp_auth hdcptx_auth = {
    {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
    HDMITX_HDCP_VERSION_NON_COMPLIANT,
    HDMITX_HDCP_NOT_REPEATER,
    {0}
};
#endif
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
static hdmirx_hdcp_auth hdcprx_auth = {
    {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
    HDMIRX_HDCP_VERSION_AUTO,
    HDMIRX_HDCP_VERSION_AUTO,
    HDMIRX_HDCP_VERSION_AUTO,
    HDMIRX_HDCP_VERSION_AUTO,
    HDMIRX_HDCP_NOT_REPEATER
};
#endif
//------------------------------------------------------------------------------
// Definitions for Thread
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Definitions for Thread
//------------------------------------------------------------------------------
#if defined(__linux)
#include <pthread.h>
typedef void*                        THREAD_RET_TYPE;
typedef pthread_t                    THREAD_ID_TYPE;
#define THREAD_ATTR
#define THREAD_RETURN(a)            return (a)
#define pthread_cancel(a)              (a)
static pthread_mutex_t thread_mtx;
#define THREAD_MTX                    thread_mtx
#define MUTEX_INIT(a, b)            pthread_mutex_init(&(a), (b))
#define MUTEX_LOCK(a)                pthread_mutex_lock(&(a))
#define MUTEX_UNLOCK(a)                pthread_mutex_unlock(&(a))
#define MUTEX_DESTROY(a)            pthread_mutex_destroy(&(a))
#define THREAD_CREATE(a, b, c, d)    pthread_create(&(a), (b), (c), (d))
#define THREAD_EXIT(a)                pthread_exit(a)
#define THREAD_CANCEL(a)            pthread_cancel(a)
#define THREAD_JOIN(a, b)            pthread_join((a), (b));
#define TESTCANCEL()                pthread_testcancel()
#define SETCANCELSTATE(a, b)        pthread_setcancelstate((a), (b))
//------------------------------------------------------------------------------
#elif defined(_WIN32) // __linux _WIN32
#include <windows.h>
#include <process.h>
typedef unsigned                    THREAD_RET_TYPE;
typedef HANDLE                        THREAD_ID_TYPE;

#if defined(__x86_64)
#define STATIC_CAST_UNSIGNED        (unsigned long long)
#else // __x86_64
#define STATIC_CAST_UNSIGNED        (unsigned)
#endif // __x86_64

#define THREAD_ATTR                    __stdcall
#define THREAD_RETURN(a)            return STATIC_CAST_UNSIGNED(a)
static HANDLE thread_mtx = 0;
#define THREAD_MTX                    thread_mtx
#define MUTEX_INIT(a, b)            a = CreateMutex((b), FALSE, "hvdiag_mutex")
#define MUTEX_LOCK(a)                WaitForSingleObject((a), INFINITE)
#define MUTEX_UNLOCK(a)                ReleaseMutex(a)
#define MUTEX_DESTROY(a)            CloseHandle(a);
#define THREAD_CREATE(a, b, c, d)                                    \
    do {                                                            \
        a = (HANDLE)_beginthreadex(NULL, 0, &(c), (d), 0, NULL);    \
        LF9129_CMD_Delay(100);                                                    \
    } while (0)
#define THREAD_EXIT(a)                _endthreadex(STATIC_CAST_UNSIGNED(a))
#define THREAD_CANCEL(a)            TerminateThread(a, FALSE)
#define THREAD_JOIN(a, b)            WaitForSingleObject((a), INFINITE);
#define TESTCANCEL()
#define SETCANCELSTATE(a, b)
//------------------------------------------------------------------------------
#else // __linux _WIN32
typedef void                        THREAD_RET_TYPE;
#define THREAD_ATTR
#define THREAD_RETURN(a)
#define THREAD_MTX
#define MUTEX_INIT(a, b)
#define MUTEX_LOCK(a)
#define MUTEX_UNLOCK(a)
#define MUTEX_DESTROY(a)
#define THREAD_CREATE(a, b, c, d)
#define THREAD_EXIT(a)
#define TESTCANCEL()
#define SETCANCELSTATE(a, b)
#endif // __linux _WIN32
//------------------------------------------------------------------------------
#define MUTEX_LOCK2()                                    \
    do {                                                \
        MUTEX_LOCK(THREAD_MTX);                            \
    } while (0)
#define MUTEX_UNLOCK2()                                    \
    do {                                                \
        MUTEX_UNLOCK(THREAD_MTX);                        \
    } while (0)

    system_port rxp = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port txp = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port txp_vbo = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port txp_lvds = {
            SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_LVDS, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };

    system_port txp_spdif = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_videos vform_vbo = {
        {
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_10BIT,
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_10BIT,
            HDMITX_DVI_MODE_DISABLE
        }
    };
    
    system_videos vform_lvds = {
        {
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_LVDS, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            SYSTEM_VFORM_1920x1080P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            HDMITX_DVI_MODE_DISABLE
        }
    };
    
    system_videos vform = {
        {
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_10BIT,
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            HDMITX_DVI_MODE_DISABLE
        }
    };

    system_port au_rxp = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port au_txp = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };

    system_audio rx_aform = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
        SYSTEM_AUDIO_CC_RSH, SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, 0x00, 0x0,
        SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, SYSTEM_AUDIO_SF_RATIO_1_1,
        {SYSTEM_AUDIO_EM_MSB, SYSTEM_AUDIO_PO_LOW, SYSTEM_AUDIO_IF_DISABLE, SYSTEM_AUDIO_SP_RIGHT}
    };
    
    system_audio tx_aform = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        SYSTEM_AUDIO_CC_RSH, SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, 0x00, 0x0,
        SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, SYSTEM_AUDIO_SF_RATIO_1_1,
        {SYSTEM_AUDIO_EM_MSB, SYSTEM_AUDIO_PO_LOW, SYSTEM_AUDIO_IF_DISABLE, SYSTEM_AUDIO_SP_RIGHT}
    };


//------------------------------------------------------------------------------
// Thread
//------------------------------------------------------------------------------
#if defined(__linux) || defined(_WIN32)
#if 1 || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    //--------------------------------------------------------------------------
    // Sample of Hot Plug Detect Status of HDMI Tx
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE hpd_thread_id = 0;
    static uint8_t hpd_thread_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
    uint8_t hpd_thread_run = 0;

    THREAD_RET_TYPE THREAD_ATTR hpd_thread(void *p_arg)
    {
        system_port port;
        uint8_t pre_stat;
        uint8_t intr;
        uint32_t ret;

        port = *(system_port *)p_arg;
        pre_stat = HDMITX_EVENT_HPD_ACTIVE | HDMITX_EVENT_SENSE_ACTIVE;
        hpd_thread_run = 1;
        MES("LF9129 hpd_thread start run");
        while (hpd_thread_run) {
            MUTEX_LOCK2();
            ret = hdmitx_get_hpd_intr(port, &intr);
            MUTEX_UNLOCK2();
            if (ret != ERR_OK) {
                ERR(API_ERROR_OCCURRED, "hdmitx_get_hpd_intr", hv_strerr(ret));
            }
            if (intr & (HDMITX_INTR_HPD_NEGATIVE | HDMITX_INTR_SENSE_NEGATIVE)) {
                // detect HPD/RxSense Negative Edge
                intr = intr & (HDMITX_INTR_HPD_NEGATIVE | HDMITX_INTR_SENSE_NEGATIVE);
                MUTEX_LOCK2();
                ret = hdmitx_set_hpd_intr(port, intr);  // clear
                MUTEX_UNLOCK2();
                if (ret != ERR_OK) {
                    ERR(API_ERROR_OCCURRED, "hdmitx_set_hpd_intr", hv_strerr(ret));
                }
                if (intr & HDMITX_INTR_HPD_NEGATIVE) {
                    MUTEX_LOCK(THREAD_MTX);
                    hpd_thread_stat = (hpd_thread_stat & ~HDMITX_EVENT_HPD_MASK)
                                        | HDMITX_EVENT_HPD_INACTIVE;
                    MES("MYH %s %d, hpd_thread_stat = %d", __func__, __LINE__, hpd_thread_stat);
                    MUTEX_UNLOCK(THREAD_MTX);
                }
                if (intr & HDMITX_INTR_SENSE_NEGATIVE) {
                    MUTEX_LOCK(THREAD_MTX);
                    hpd_thread_stat = (hpd_thread_stat & ~HDMITX_EVENT_SENSE_MASK)
                                        | HDMITX_EVENT_SENSE_INACTIVE;
                    MES("MYH %s %d, hpd_thread_stat = %d", __func__, __LINE__, hpd_thread_stat);
                    MUTEX_UNLOCK(THREAD_MTX);
                }
            } else if (intr & (HDMITX_INTR_HPD_POSITIVE | HDMITX_INTR_SENSE_POSITIVE)) {
                // detect HPD/RxSense Positive Edge
                intr = intr & (HDMITX_INTR_HPD_POSITIVE | HDMITX_INTR_SENSE_POSITIVE);
                MUTEX_LOCK2();
                ret = hdmitx_set_hpd_intr(port, intr);
                MUTEX_UNLOCK2();
                if (ret != ERR_OK) {
                    ERR(API_ERROR_OCCURRED, "hdmitx_set_hpd_intr", hv_strerr(ret));
                }
                if (intr & HDMITX_INTR_HPD_POSITIVE) {
                    MUTEX_LOCK(THREAD_MTX);
                    hpd_thread_stat = (hpd_thread_stat & ~HDMITX_EVENT_HPD_MASK)
                                        | HDMITX_EVENT_HPD_ACTIVE;
                    MES("MYH %s %d, hpd_thread_stat = %d", __func__, __LINE__, hpd_thread_stat);
                    MUTEX_UNLOCK(THREAD_MTX);
                }
                if (intr & HDMITX_INTR_SENSE_NEGATIVE) {
                    MUTEX_LOCK(THREAD_MTX);
                    hpd_thread_stat = (hpd_thread_stat & ~HDMITX_EVENT_SENSE_MASK)
                                        | HDMITX_EVENT_SENSE_ACTIVE;
                    MES("MYH %s %d, hpd_thread_stat = %d", __func__, __LINE__, hpd_thread_stat);
                    MUTEX_UNLOCK(THREAD_MTX);
                }
            } else {
                // get HPD/RxSense Status
                MUTEX_LOCK2();
                ret = hdmitx_get_hpd_status(port, &hpd_thread_stat);
                MES("MYH %s %d, hpd_thread_stat = %d", __func__, __LINE__, hpd_thread_stat);
                MUTEX_UNLOCK2();
                if (ret != ERR_OK) {
                    ERR(API_ERROR_OCCURRED, "hdmitx_get_hpd_status", hv_strerr(ret));
                }
            }
            if (pre_stat != hpd_thread_stat) {
                switch (hpd_thread_stat & HDMITX_EVENT_HPD_MASK) {
                    case HDMITX_EVENT_HPD_ACTIVE:
                        MES("\nMYH %s %d, HDMI Tx HPD active/", __func__, __LINE__);
                        hpd_active_state = 1;
                        break;
                    case HDMITX_EVENT_HPD_INACTIVE:
                        MES("\nMYH %s %d, HDMI Tx HPD inactive/", __func__, __LINE__);
                        break;
                }
                switch (hpd_thread_stat & HDMITX_EVENT_SENSE_MASK) {
                    case HDMITX_EVENT_SENSE_ACTIVE:
                        MES("MYH %s %d, Sense active", __func__, __LINE__);
                        break;
                    case HDMITX_EVENT_SENSE_INACTIVE:
                        MES("MYH %s %d, Sense inactive", __func__, __LINE__);
                        break;
                }
                // if(hpd_thread_stat == (HDMITX_EVENT_HPD_ACTIVE|HDMITX_EVENT_SENSE_ACTIVE)){
                    MES("LF9129 hpd_thread HPD IS OK!!");
                //     if(hdmiStatus == 0&&(initOK == 1)){
                //         MUTEX_LOCK(THREAD_MTX);
                //         modeChange = 1;
                //         MUTEX_UNLOCK(THREAD_MTX);
                //     }
                //     hdmiStatus = 1;
                // }else{
                //     hdmiStatus = 0;
                // }
            }
            MES("MYH %s %d, hpd_thread_stat = %d", __func__, __LINE__, hpd_thread_stat);
            pre_stat = hpd_thread_stat;
            LF9129_CMD_Delay(100);
        }
        MES("LF9129 hpd_thread is THREAD_EXIT!!");
        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel hpd_thread()
    //--------------------------------------------------------------------------
    void hpd_thread_cancel(void)
    {
        if (hpd_thread_id != 0) {
            hpd_thread_run = 0;
            hpd_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define HPD_THREAD                          hpd_thread
    #define HPD_THREAD_CANCEL()                 hpd_thread_cancel()
    #define HPD_THREAD_ID                       hpd_thread_id
    #define HPD_THREAD_STAT                     hpd_thread_stat
    //--------------------------------------------------------------------------
    // Sample of HDCP Authentication Status of HDMI Tx
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE hdcptx_thread_id = 0;
    static uint32_t hdcptx_thread_ret = ERR_OK;
    THREAD_RET_TYPE THREAD_ATTR hdcptx_thread(void *p_arg)
    {
        MES("HDCP is open");
        hdmitx_hdcp_auth *p_auth;
        uint32_t stime;

        p_auth = (hdmitx_hdcp_auth *)p_arg;

        if (p_auth->hdcp_version == HDMITX_HDCP_VERSION_1_4) {
            // Reading Ri synchronously every 128th frame is also acceptable
            // in lieu of asynchronous polling every 2 seconds.
            stime = 1000;
        } else if (p_auth->hdcp_version == HDMITX_HDCP_VERSION_2_2) {
            // Polling of the RxStatus register by the HDCP Transmitter is done
            // at least once every second while the Transmitter is in the authenticated state.
            stime = 1000;
        } else {
            stime = 1000;
        }

        hdcptx_thread_run = 1;

        while (hdcptx_thread_run) {
            MES("%s %d, hdmitx_hdcp_get_authentication_status st ", __func__, __LINE__);
            if(auto_hdmitx_hdcp_thread_state < 3)
            {
                break;
            }
            MUTEX_LOCK2();
            hdcptx_thread_ret = hdmitx_hdcp_get_authentication_status(p_auth);
            MUTEX_UNLOCK2();
            MES("%s %d, hdmitx_hdcp_get_authentication_status edn", __func__, __LINE__);
            if (hdcptx_thread_run == 0) {
                break;
            }
            MUTEX_LOCK(THREAD_MTX);
            if (hdcptx_thread_ret != ERR_OK) {
                MES("MN869129 %s %d, HDCP Tx: %s", __func__, __LINE__, hv_strerr(hdcptx_thread_ret));
            }

            MUTEX_UNLOCK(THREAD_MTX);
            if(auto_hdmitx_hdcp_thread_state < 3)
            {
                break;
            }
            SLEEP(stime / 2);
            if(auto_hdmitx_hdcp_thread_state < 3)
            {
                break;
            }
            if (hdcptx_thread_id == 0)
            {
                break;
            }
            SLEEP(stime / 2);
        }
        MES("HDCP is EXIT");
        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel hdcptx_thread()
    //--------------------------------------------------------------------------
    void hdcptx_thread_cancel(void)
    {
        MES("hdcptx_thread_id = %d", hdcptx_thread_id);
        if (hdcptx_thread_id != 0) {
            MES("MYH %s %d", __func__, __LINE__);
            THREAD_CANCEL(hdcptx_thread_id);
            MES("MYH %s %d, cancel hdcptx_thread, join hdcptx_thread exit", __func__, __LINE__);
            hdcptx_thread_run = 0;
            THREAD_JOIN(hdcptx_thread_id, NULL);
            MES("MYH %s %d, cancel hdcptx_thread, join hdcptx_thread exit", __func__, __LINE__);
            hdcptx_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define HDCPTX_THREAD                    hdcptx_thread
    #define HDCPTX_THREAD_CANCEL()            hdcptx_thread_cancel()
    #define HDCPTX_THREAD_ID                hdcptx_thread_id
    #define HDCPTX_THREAD_RET                hdcptx_thread_ret

    //--------------------------------------------------------------------------
    // Sample of HDMI Tx auto processing
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE auto_hdmitx_thread_id = 0;
    static uint32_t auto_hdmitx_thread_run = 0;
    static hdmitx_hdcp_auth auto_hdmitx_thread_auth = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        HDMITX_HDCP_VERSION_NON_COMPLIANT,
        HDMITX_HDCP_NOT_REPEATER,
        {0}
    };
    uint32_t cur_hdcptx_thread_ret;

    THREAD_RET_TYPE THREAD_ATTR auto_hdmitx_thread(void *p_arg)
    {
        system_port port;
        uint8_t cur_stat;
        uint8_t pre_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
        hdmitx_mute mute;
        uint32_t i;
        uint32_t ret;

        port = *(system_port*)p_arg;
        auto_hdmitx_thread_run = 1;
        MES("LF9129 auto_hdmitx_thread start run");
        while (auto_hdmitx_thread_run) {
            MUTEX_LOCK(THREAD_MTX);
            cur_stat = HPD_THREAD_STAT;
            cur_hdcptx_thread_ret = HDCPTX_THREAD_RET;
            MUTEX_UNLOCK(THREAD_MTX);
            // when HPD and/or Rx Sense are changed between active and inactive status
            // or when error of hdmitx_hdcp_get_authentication_status() has occurred
            if ((pre_stat != cur_stat) || (cur_hdcptx_thread_ret != ERR_OK)) {
                // when HPD and Rx Sense are active status
                // and when error of hdmitx_hdcp_get_authentication_status() is not
                // ERR_HDMITX_HPD_INACTIVE and ERR_HDMITX_RXSENSE_INACTIVE
                if (((cur_stat & HDMITX_EVENT_HPD_MASK) == HDMITX_EVENT_HPD_ACTIVE)
                    && ((cur_stat & HDMITX_EVENT_SENSE_MASK) == HDMITX_EVENT_SENSE_ACTIVE)
                    && (cur_hdcptx_thread_ret != ERR_HDMITX_HPD_INACTIVE)
                    && (cur_hdcptx_thread_ret != ERR_HDMITX_RXSENSE_INACTIVE)) {
                    // when READY bit is detected, retry hdmitx_hdcp_repeater_authentication()
                    if (cur_hdcptx_thread_ret == ERR_HDCPTX22_RXSTATUS_READY) {
                        MES("when READY bit is detected, retry hdmitx_hdcp_repeater_authentication()");
                        goto _LOOP_END;
                    } else if (cur_hdcptx_thread_ret != ERR_OK) {
                        // hdmitx_video_mute(ON)
                        MES("LF9129 mute HDMI Tx video HPD ACTIVE");
                        mute.port = port;
                        mute.mute = SYSTEM_TX_MUTE_ON;
                        MUTEX_LOCK2();
                        ret = hdmitx_video_mute(&mute);
                        MUTEX_UNLOCK2();
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                            goto _LOOP_END;
                        }
                        // hdmitx_hdcp_authentication_reset()
                        MES("LF9129 reset HDCP Tx, %d", __LINE__);
                        HDCPTX_THREAD_CANCEL();
                        MUTEX_LOCK2();
                        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_thread_auth);
                        MUTEX_UNLOCK2();
                        // init hdcptx_thread_ret
                        MUTEX_LOCK(THREAD_MTX);
                        HDCPTX_THREAD_RET = ERR_OK;
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        }
                    }
                    // HPD and Rx Sense are active
                    // hdmitx_hdcp_authentication()
                    MES("LF9129 set HDCP Tx");
                    MUTEX_LOCK(THREAD_MTX);
                    auto_hdmitx_thread_auth.port = port;
                    MUTEX_UNLOCK(THREAD_MTX);
                    HDCPTX_THREAD_CANCEL();
                    MUTEX_LOCK2();
                    ret = hdmitx_hdcp_authentication(&auto_hdmitx_thread_auth);
                    MES("LF9129 hdmitx_hdcp_authentication end, %d", __LINE__);
                    MUTEX_UNLOCK2();
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication", hv_strerr(ret));
                #ifdef HV_ERRNO
                        if (HV_ERRNO != ERR_OK) {
                            ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
                        }
                #endif // HV_ERRNO
                        // hdmitx_hdcp_authentication_reset()
                        MES("LF9129 reset HDCP Tx, %d", __LINE__);
                        HDCPTX_THREAD_CANCEL();
                        MUTEX_LOCK2();
                        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_thread_auth);
                        MUTEX_UNLOCK2();
                        // init hdcptx_thread_ret
                        MUTEX_LOCK(THREAD_MTX);
                        HDCPTX_THREAD_RET = ERR_OK;
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        }
                        goto _LOOP_END;
                    }
                    MES("LF9129 HDCP %s", ((auto_hdmitx_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_1_4) ? "1.4": (auto_hdmitx_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_2_2) ? "2.2": "non-compliant"));
                    MES("LF9129   Sink device: %s", ((auto_hdmitx_thread_auth.is_repeater == HDMITX_HDCP_REPEATER) ? "Repeater": "NOT Repeater"));
                    MES("LF9129   bksv: %02X %02X %02X %02X %02X", auto_hdmitx_thread_auth.bksv[0],auto_hdmitx_thread_auth.bksv[1],auto_hdmitx_thread_auth.bksv[2],auto_hdmitx_thread_auth.bksv[3],auto_hdmitx_thread_auth.bksv[4]);
                    // TODO: You must check for bksv in the current revocation list.
                    // call hdmitx_hdcp_get_authentication_status() periodically
                    THREAD_CREATE(HDCPTX_THREAD_ID, NULL, HDCPTX_THREAD, (void *)&auto_hdmitx_thread_auth);
                    // hdmitx_video_mute(OFF)
                    MES("LF9129 unmute HDMI Tx video");
                    mute.port = port;
                    mute.mute = SYSTEM_TX_MUTE_OFF;
                    MUTEX_LOCK2();
                    ret = hdmitx_video_mute(&mute);
                    MUTEX_UNLOCK2();
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                        goto _LOOP_END;
                    }
                    MES("LF9129 mute HDMI Tx video HPD ACTIVE DONE");
                } else {
                    // HPD and/or Rx Sense are inactive
                    // hdmitx_video_mute(ON)
                    MES("LF9129 mute HDMI Tx video HPD INACTIVE");
                    mute.port = port;
                    mute.mute = SYSTEM_TX_MUTE_ON;
                    MUTEX_LOCK2();
                    ret = hdmitx_video_mute(&mute);
                    MUTEX_UNLOCK2();
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                        goto _LOOP_END;
                    }
                    // hdmitx_hdcp_authentication_reset()
                    MES("LF9129 reset HDCP Tx, %d", __LINE__);
                    HDCPTX_THREAD_CANCEL();
                    MUTEX_LOCK2();
                    ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_thread_auth);
                    MUTEX_UNLOCK2();
                    // init hdcptx_thread_ret
                    MUTEX_LOCK(THREAD_MTX);
                    HDCPTX_THREAD_RET = ERR_OK;
                    MUTEX_UNLOCK(THREAD_MTX);
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        goto _LOOP_END;
                    }

                    MES("LF9129 mute HDMI Tx video HPD INACTIVE DONE");
                }
                pre_stat = cur_stat;
            }
_LOOP_END:
            LF9129_CMD_Delay(100);
        }
_END:
        MES("LF9129 auto_hdmitx_thread exit!!!");
        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel auto_hdmitx_sg_thread()
    //--------------------------------------------------------------------------
    void auto_hdmitx_thread_cancel(void)
    {
        if (auto_hdmitx_thread_id != 0) {
            auto_hdmitx_thread_run = 0;
            auto_hdmitx_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define AUTO_HDMITX_THREAD                auto_hdmitx_thread
    #define AUTO_HDMITX_THREAD_CANCEL()        auto_hdmitx_thread_cancel()
    #define AUTO_HDMITX_THREAD_ID            auto_hdmitx_thread_id
    #define AUTO_HDMITX_THREAD_AUTH        auto_hdmitx_thread_auth
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    // Sample of HDMI Tx auto SG processing
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE auto_hdmitx_sg_thread_id = 0;
    static hdmitx_hdcp_auth auto_hdmitx_sg_thread_auth = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        HDMITX_HDCP_VERSION_NON_COMPLIANT,
        HDMITX_HDCP_NOT_REPEATER,
        {0}
    };
    static hdmitx_hdcp_repeater_auth auto_hdmitx_sg_thread_rep_auth = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        HDMITX_HDCP_VERSION_NON_COMPLIANT,
        HDMITX_HDCP_STREAM_TYPE_0,
        0,
        0,
        {
            {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
            {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
            {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
            {0}
        },
        {0}
    };
    THREAD_RET_TYPE THREAD_ATTR auto_hdmitx_sg_thread(void *p_arg)
    {
        hdmitx_sg_video *p_vform;
        system_port port;
        uint8_t cur_stat;
        uint8_t pre_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
        uint32_t cur_hdcptx_thread_ret;
        hdmitx_mute mute;
        uint32_t i;
        uint32_t ret;

        p_vform = (hdmitx_sg_video *)p_arg;
        port = p_vform->port;

        // hdmitx_set_sg_format()
        MES("set video format as signal generator for HDMI Tx:");
        print_video_format(p_vform->video_format, p_vform->color_format, p_vform->deep_color);
        MES("  color bar by RGB (Vsync synchronous)");
        MUTEX_LOCK2();
        ret = hdmitx_set_sg_format(p_vform);
        MUTEX_UNLOCK2();
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_set_sg_format", hv_strerr(ret));
            goto _END;
        }
        while (1) {
            MUTEX_LOCK(THREAD_MTX);
            cur_stat = HPD_THREAD_STAT;
            cur_hdcptx_thread_ret = HDCPTX_THREAD_RET;
            MUTEX_UNLOCK(THREAD_MTX);
            // when HPD and/or Rx Sense are changed between active and inactive status
            // or when error of hdmitx_hdcp_get_authentication_status() has occurred
            if ((pre_stat != cur_stat) || (cur_hdcptx_thread_ret != ERR_OK)) {
                // when HPD and Rx Sense are active status
                // and when error of hdmitx_hdcp_get_authentication_status() is not
                // ERR_HDMITX_HPD_INACTIVE and ERR_HDMITX_RXSENSE_INACTIVE
                if (((cur_stat & HDMITX_EVENT_HPD_MASK) == HDMITX_EVENT_HPD_ACTIVE)
                    && ((cur_stat & HDMITX_EVENT_SENSE_MASK) == HDMITX_EVENT_SENSE_ACTIVE)
                    && (cur_hdcptx_thread_ret != ERR_HDMITX_HPD_INACTIVE)
                    && (cur_hdcptx_thread_ret != ERR_HDMITX_RXSENSE_INACTIVE)) {
                    // when READY bit is detected, retry hdmitx_hdcp_repeater_authentication()
                    if (cur_hdcptx_thread_ret == ERR_HDCPTX22_RXSTATUS_READY) {
                        goto _REPEATER_AUTH_START;
                    } else if (cur_hdcptx_thread_ret != ERR_OK) {
                        // hdmitx_video_mute(ON)
                        MES("mute HDMI Tx video");
                        mute.port = port;
                        mute.mute = SYSTEM_TX_MUTE_ON;
                        MUTEX_LOCK2();
                        ret = hdmitx_video_mute(&mute);
                        MUTEX_UNLOCK2();
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                            goto _LOOP_END;
                        }
                        // hdmitx_hdcp_authentication_reset()
                        MES("reset HDCP Tx, %d", __LINE__);
                        HDCPTX_THREAD_CANCEL();
                        MUTEX_LOCK2();
                        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_sg_thread_auth);
                        MUTEX_UNLOCK2();
                        // init hdcptx_thread_ret
                        MUTEX_LOCK(THREAD_MTX);
                        HDCPTX_THREAD_RET = ERR_OK;
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        }
                    }
                    // HPD and Rx Sense are active
                    // hdmitx_hdcp_authentication()
                    MES("set HDCP Tx, %d", __LINE__);
                    MUTEX_LOCK(THREAD_MTX);
                    auto_hdmitx_sg_thread_auth.port = port;
                    MUTEX_UNLOCK(THREAD_MTX);
                    HDCPTX_THREAD_CANCEL();
                    MUTEX_LOCK2();
                    ret = hdmitx_hdcp_authentication(&auto_hdmitx_sg_thread_auth);
                    MUTEX_UNLOCK2();
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication", hv_strerr(ret));
#ifdef HV_ERRNO
                        if (HV_ERRNO != ERR_OK) {
                            ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
                        }
#endif // HV_ERRNO
                        // hdmitx_hdcp_authentication_reset()
                        MES("reset HDCP Tx, %d", __LINE__);
                        HDCPTX_THREAD_CANCEL();
                        MUTEX_LOCK2();
                        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_sg_thread_auth);
                        MUTEX_UNLOCK2();
                        // init hdcptx_thread_ret
                        MUTEX_LOCK(THREAD_MTX);
                        HDCPTX_THREAD_RET = ERR_OK;
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        }
                        goto _LOOP_END;
                    }
                    MES("HDCP %s", ((auto_hdmitx_sg_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_1_4) ? "1.4": (auto_hdmitx_sg_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_2_2) ? "2.2": "non-compliant"));
                    MES("  Sink device: %s", ((auto_hdmitx_sg_thread_auth.is_repeater == HDMITX_HDCP_REPEATER) ? "Repeater": "NOT Repeater"));
                    MES("  bksv: %02X %02X %02X %02X %02X", auto_hdmitx_sg_thread_auth.bksv[0],auto_hdmitx_sg_thread_auth.bksv[1],auto_hdmitx_sg_thread_auth.bksv[2],auto_hdmitx_sg_thread_auth.bksv[3],auto_hdmitx_sg_thread_auth.bksv[4]);
                    // TODO: You must check for bksv in the current revocation list.
                    if (auto_hdmitx_sg_thread_auth.is_repeater == HDMITX_HDCP_REPEATER) {
_REPEATER_AUTH_START:
                        // hdmitx_hdcp_repeater_authentication()
                        MUTEX_LOCK(THREAD_MTX);
                        auto_hdmitx_sg_thread_rep_auth.port = port;
                        auto_hdmitx_sg_thread_rep_auth.hdcp_version = auto_hdmitx_sg_thread_auth.hdcp_version;
                        auto_hdmitx_sg_thread_rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_0;
                        //auto_hdmitx_sg_thread_rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_1;
                        //auto_hdmitx_sg_thread_rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_AUTO;
                        MUTEX_UNLOCK(THREAD_MTX);
                        MUTEX_LOCK2();
                        ret = hdmitx_hdcp_repeater_authentication(&auto_hdmitx_sg_thread_rep_auth);
                        MUTEX_UNLOCK2();
                        // when READY bit is detected, retry hdmitx_hdcp_repeater_authentication()
                        if (ret == ERR_HDCPTX22_RXSTATUS_READY) {
                            goto _REPEATER_AUTH_START;
                        }
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_repeater_authentication", hv_strerr(ret));
#ifdef HV_ERRNO
                            if (HV_ERRNO != ERR_OK) {
                                ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
                            }
#endif // HV_ERRNO
                            // hdmitx_hdcp_authentication_reset()
                            MES("reset HDCP Tx, %d", __LINE__);
                            HDCPTX_THREAD_CANCEL();
                            MUTEX_LOCK2();
                            ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_sg_thread_auth);
                            MUTEX_UNLOCK2();
                            // init hdcptx_thread_ret
                            MUTEX_LOCK(THREAD_MTX);
                            HDCPTX_THREAD_RET = ERR_OK;
                            MUTEX_UNLOCK(THREAD_MTX);
                            if (ret != ERR_OK) {
                                ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                            }
                            goto _LOOP_END;
                        }
                        MES("  Downstream count: %d", (int)auto_hdmitx_sg_thread_rep_auth.downstream_count);
                        MES("  Downstream depth: %d", (int)auto_hdmitx_sg_thread_rep_auth.downstream_depth);
                        for (i=0; i<auto_hdmitx_sg_thread_rep_auth.downstream_count; i++) {
                            MES("    ksv[%d]: %02X %02X %02X %02X %02X", (int)i,auto_hdmitx_sg_thread_rep_auth.ksv_list[i][0],auto_hdmitx_sg_thread_rep_auth.ksv_list[i][1],auto_hdmitx_sg_thread_rep_auth.ksv_list[i][2],auto_hdmitx_sg_thread_rep_auth.ksv_list[i][3],auto_hdmitx_sg_thread_rep_auth.ksv_list[i][4]);
                        }
                        MES("  Bstatus: %02X %02X", auto_hdmitx_sg_thread_rep_auth.bstatus[0],auto_hdmitx_sg_thread_rep_auth.bstatus[1]);
                        // TODO: You must check for ksv_list in the current revocation list.
                    }
                    // call hdmitx_hdcp_get_authentication_status() periodically
                    THREAD_CREATE(HDCPTX_THREAD_ID, NULL, HDCPTX_THREAD, (void *)&auto_hdmitx_sg_thread_auth);
                    // hdmitx_video_mute(OFF)
                    MES("%s, unmute HDMI Tx video", __func__);
                    mute.port = port;
                    mute.mute = SYSTEM_TX_MUTE_OFF;
                    MUTEX_LOCK2();
                    ret = hdmitx_video_mute(&mute);
                    MUTEX_UNLOCK2();
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                        goto _LOOP_END;
                    }
                } else {
                    // HPD and/or Rx Sense are inactive
                    // hdmitx_video_mute(ON)
                    MES("mute HDMI Tx video");
                    mute.port = port;
                    mute.mute = SYSTEM_TX_MUTE_ON;
                    MUTEX_LOCK2();
                    ret = hdmitx_video_mute(&mute);
                    MUTEX_UNLOCK2();
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                        goto _LOOP_END;
                    }
                    // hdmitx_hdcp_authentication_reset()
                    MES("reset HDCP Tx, %d", __LINE__);
                    HDCPTX_THREAD_CANCEL();
                    MUTEX_LOCK2();
                    ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_sg_thread_auth);
                    MUTEX_UNLOCK2();
                    // init hdcptx_thread_ret
                    MUTEX_LOCK(THREAD_MTX);
                    HDCPTX_THREAD_RET = ERR_OK;
                    MUTEX_UNLOCK(THREAD_MTX);
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        goto _LOOP_END;
                    }
                }
                pre_stat = cur_stat;
            }
_LOOP_END:
            LF9129_CMD_Delay(100);
        }
_END:
        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel auto_hdmitx_sg_thread()
    //--------------------------------------------------------------------------
    void auto_hdmitx_sg_thread_cancel(void)
    {
        if (auto_hdmitx_sg_thread_id != 0) {
            THREAD_CANCEL(auto_hdmitx_sg_thread_id);
            THREAD_JOIN(auto_hdmitx_sg_thread_id, NULL);
            auto_hdmitx_sg_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define AUTO_HDMITX_SG_THREAD            auto_hdmitx_sg_thread
    #define AUTO_HDMITX_SG_THREAD_CANCEL()    auto_hdmitx_sg_thread_cancel()
    #define AUTO_HDMITX_SG_THREAD_ID        auto_hdmitx_sg_thread_id
    #define AUTO_HDMITX_SG_THREAD_AUTH        auto_hdmitx_sg_thread_auth
    #define AUTO_HDMITX_SG_THREAD_REP_AUTH    auto_hdmitx_sg_thread_rep_auth
    //--------------------------------------------------------------------------
    // Sample of HDMI Tx auto HDCP processing
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE auto_hdmitx_hdcp_thread_id = 0;
    static uint32_t auto_hdmitx_hdcp_thread_run = 0;
    static hdmitx_hdcp_auth auto_hdmitx_hdcp_thread_auth = {
        {SYSTEM_PORT_NUMBER_0, SYSTEM_BUS_NUMBER_0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        HDMITX_HDCP_VERSION_NON_COMPLIANT,
        HDMITX_HDCP_NOT_REPEATER,
        {0}
    };
    static hdmitx_hdcp_repeater_auth auto_hdmitx_hdcp_thread_rep_auth = {
        {SYSTEM_PORT_NUMBER_0, SYSTEM_BUS_NUMBER_0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        HDMITX_HDCP_VERSION_NON_COMPLIANT,
        HDMITX_HDCP_STREAM_TYPE_0,
        0,
        0,
        {
            {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
            {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
            {0},{0},{0},{0},{0},{0},{0},{0},{0},{0},
            {0}
        },
        {0}
    };
    
    THREAD_RET_TYPE THREAD_ATTR auto_hdmitx_hdcp_thread(void *p_arg)
    {
        hdmitx_video *p_vform;
        system_port port;
        uint8_t cur_stat;
        uint8_t pre_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
        uint32_t cur_hdcptx_thread_ret;
        auto_hdmitx_hdcp_thread_run = 1;
        hdmitx_mute mute;
        uint32_t i;
        uint32_t ret;
        uint8_t hdcp_detect_thread = 1;
        MES("auto_hdmitx_hdcp_thread is open");

        p_vform = (hdmitx_video *)p_arg;
        port = p_vform->tx_port;

        MES("%s, into  %d", __func__, __LINE__);
        while (auto_hdmitx_hdcp_thread_run) {
            MUTEX_LOCK(THREAD_MTX);
            cur_stat = auto_hdmitx_hdcp_thread_state;
            cur_hdcptx_thread_ret = HDCPTX_THREAD_RET;
            MUTEX_UNLOCK(THREAD_MTX);
            // when HPD and/or Rx Sense are changed between active and inactive status
            // or when error of hdmitx_hdcp_get_authentication_status() has occurred

            if ((pre_stat != cur_stat) || (cur_hdcptx_thread_ret != ERR_OK)) {
                                                                    MES("MYH %s, if ((pre_stat != cur_stat) ||  %d", __func__, __LINE__);
                // when HPD and Rx Sense are active status
                // and when error of hdmitx_hdcp_get_authentication_status() is not
                // ERR_HDMITX_HPD_INACTIVE and ERR_HDMITX_RXSENSE_INACTIVE
                MES("MYH (cur_stat & HDMITX_EVENT_HPD_MASK) == HDMITX_EVENT_HPD_ACTIVE) = %d", (cur_stat & HDMITX_EVENT_HPD_MASK) == HDMITX_EVENT_HPD_ACTIVE);
                MES("MYH ((cur_stat & HDMITX_EVENT_SENSE_MASK) == HDMITX_EVENT_SENSE_ACTIVE) = %d", ((cur_stat & HDMITX_EVENT_SENSE_MASK) == HDMITX_EVENT_SENSE_ACTIVE));
                MES("MYH (cur_hdcptx_thread_ret != ERR_HDMITX_HPD_INACTIVE) = %d", (cur_hdcptx_thread_ret != ERR_HDMITX_HPD_INACTIVE));
                MES("MYH (cur_hdcptx_thread_ret != ERR_HDMITX_RXSENSE_INACTIVE = %d", (cur_hdcptx_thread_ret != ERR_HDMITX_RXSENSE_INACTIVE));

                if (((cur_stat & HDMITX_EVENT_HPD_MASK) == HDMITX_EVENT_HPD_ACTIVE)
                    && ((cur_stat & HDMITX_EVENT_SENSE_MASK) == HDMITX_EVENT_SENSE_ACTIVE)
                    && (cur_hdcptx_thread_ret != ERR_HDMITX_HPD_INACTIVE)
                    && (cur_hdcptx_thread_ret != ERR_HDMITX_RXSENSE_INACTIVE)) {
                                                                    MES("MYH %s, if (((cur_stat & HDMITX_EVENT_HPD_MASK) ==  %d", __func__, __LINE__);
                    // when READY bit is detected, retry hdmitx_hdcp_repeater_authentication()
                    if (cur_hdcptx_thread_ret == ERR_HDCPTX22_RXSTATUS_READY) {
                                                                    MES("MYH %s, if (cur_hdcptx_thread_ret ==  %d", __func__, __LINE__);
                        goto _REPEATER_AUTH_START;
                    } else if (cur_hdcptx_thread_ret != ERR_OK) {
                                                                    MES("MYH %s, } else if (cur_hdcptx_thread_ret !=  %d", __func__, __LINE__);
                                                                    MES("MN869129 %s %d, HDCP Tx: %s", __func__, __LINE__, hv_strerr(hdcptx_thread_ret));
                        // hdmitx_video_mute(ON)
                        MES("mute HDMI Tx video");
_REPEATER_AUTH_RESET:
                        // mute.port = port;
                        // mute.mute = SYSTEM_TX_MUTE_ON;
                        // MUTEX_LOCK2();
                                                                    MES("MYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
                        // ret = hdmitx_video_mute(&mute);
                        // MUTEX_UNLOCK2();
                                                                    MES("MYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
                        // if (ret != ERR_OK) {
                        //     ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                        //     goto _LOOP_END;
                        // }
                        // hdmitx_audio_mute(ON)
                        MES("mute HDMI Tx audio");
                        // mute.port = port;
                        // mute.mute = SYSTEM_TX_MUTE_ON;
                        // MUTEX_LOCK2();
                                                                    MES("MYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
                        // ret = hdmitx_audio_mute(&mute);
                                                                    MES("MYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
                        // MUTEX_UNLOCK2();
                        // if (ret != ERR_OK) {
                        //     ERR(API_ERROR_OCCURRED, "hdmitx_audio_mute", hv_strerr(ret));
                        //     goto _LOOP_END;
                        // }
                        // hdmitx_hdcp_authentication_reset()
                        MES("reset HDCP Tx");
                        // HDCPTX_THREAD_CANCEL();
                        MUTEX_LOCK2();
                                                                    MES("MYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
                        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
                        MUTEX_UNLOCK2();
                                                                    MES("MYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
                        // init hdcptx_thread_ret
                        MUTEX_LOCK(THREAD_MTX);
                                                                    MES("MYH %s, MUTEX_LOCK();  %d", __func__, __LINE__);
                        HDCPTX_THREAD_RET = ERR_OK;
                        MUTEX_UNLOCK(THREAD_MTX);
                                                                    MES("MYH %s, MUTEX_UNLOCK();  %d", __func__, __LINE__);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        }
                    }
                    // HPD and Rx Sense are active
                    // hdmitx_hdcp_authentication()
                    MES("set HDCP Tx");
                    MUTEX_LOCK(THREAD_MTX);
                                                                    MES("MYH %s, MUTEX_LOCK();  %d", __func__, __LINE__);
                    auto_hdmitx_hdcp_thread_auth.port = port;
                    MUTEX_UNLOCK(THREAD_MTX);
                                                                    MES("MYH %s, MUTEX_UNLOCK();  %d", __func__, __LINE__);
                    // HDCPTX_THREAD_CANCEL();
                    MUTEX_LOCK2();
                                                                    MES("MYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
                    if ( (current_hdcp_version == 1) && (edid_read) ) {
                        MES("delay 1.5s========================================================\n");
                        SLEEP_Delay_ms(2000);
                        edid_read = 0;
                    }
                    ret = hdmitx_hdcp_authentication(&auto_hdmitx_hdcp_thread_auth);
                    MUTEX_UNLOCK2();
                                                                    MES("MYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
                    if (ret != ERR_OK && is_hdcp_enable) {
                                                                    MES("MYH %s, if (ret != ERR_OK)  %d", __func__, __LINE__);
                        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication", hv_strerr(ret));
#ifdef HV_ERRNO
                        if (HV_ERRNO != ERR_OK) {
                            ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
                        }
#endif // HV_ERRNO
                        #if 0
                        MES("MN869129 %s %d, ret: %s", __func__, __LINE__, hv_strerr(ret));
                        MES("reset HDCP Tx");
                        HDCPTX_THREAD_CANCEL();
                        MUTEX_LOCK2();
                        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
                        MUTEX_UNLOCK2();
                        // init hdcptx_thread_ret
                        MUTEX_LOCK(THREAD_MTX);
                        HDCPTX_THREAD_RET = ERR_OK;
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        }
                        #endif
                        goto _LOOP_END;
                    }
                    MES("HDCP %s", ((auto_hdmitx_hdcp_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_1_4) ? "1.4": (auto_hdmitx_hdcp_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_2_2) ? "2.2": "non-compliant"));
                    MES("  Sink device: %s", ((auto_hdmitx_hdcp_thread_auth.is_repeater == HDMITX_HDCP_REPEATER) ? "Repeater": "NOT Repeater"));
                    MES("  bksv: %02X %02X %02X %02X %02X", auto_hdmitx_hdcp_thread_auth.bksv[0],auto_hdmitx_hdcp_thread_auth.bksv[1],auto_hdmitx_hdcp_thread_auth.bksv[2],auto_hdmitx_hdcp_thread_auth.bksv[3],auto_hdmitx_hdcp_thread_auth.bksv[4]);
                    // TODO: You must check for bksv in the current revocation list.
                    if ( (auto_hdmitx_hdcp_thread_auth.is_repeater == HDMITX_HDCP_REPEATER) || xbh_9129_is_repeater ) {
_REPEATER_AUTH_START:
                        // hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);

                        // hdmitx_hdcp_repeater_authentication()
                        MUTEX_LOCK(THREAD_MTX);
                        auto_hdmitx_hdcp_thread_rep_auth.port = port;
                        auto_hdmitx_hdcp_thread_rep_auth.hdcp_version = auto_hdmitx_hdcp_thread_auth.hdcp_version;
                        auto_hdmitx_hdcp_thread_rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_0;
                        //auto_hdmitx_hdcp_thread_rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_1;
                        //auto_hdmitx_hdcp_thread_rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_AUTO;
                        MUTEX_UNLOCK(THREAD_MTX);
                        MUTEX_LOCK2();
                        xbh_9129_is_repeater = 0;
                        ret = hdmitx_hdcp_repeater_authentication(&auto_hdmitx_hdcp_thread_rep_auth);
                        MUTEX_UNLOCK2();
                        // when READY bit is detected, retry hdmitx_hdcp_repeater_authentication()
                        if (ret == ERR_HDCPTX22_RXSTATUS_READY) {
                            goto _REPEATER_AUTH_START;
                        } else if ( (ret == ERR_HDCPTX22_RXSTATUS_READ_RETRY) || (ret == ERR_HDCPTX22_SEQ_NUM_V_INVALID) || (ret == ERR_HDCPTX22_SEQ_NUM_V_ROLL_OVER) ) {
                            xbh_9129_is_repeater = 1;
                            goto _REPEATER_AUTH_RESET;
                        }

                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_repeater_authentication", hv_strerr(ret));
#ifdef HV_ERRNO
                            if (HV_ERRNO != ERR_OK) {
                                ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
                            }
#endif // HV_ERRNO
                            MES("Mn869129 this is repeater\n");
                            #if 0
                            // hdmitx_hdcp_authentication_reset()
                            MES("reset HDCP Tx");
                            HDCPTX_THREAD_CANCEL();
                            MUTEX_LOCK2();
                            ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
                            MUTEX_UNLOCK2();
                            // init hdcptx_thread_ret
                            MUTEX_LOCK(THREAD_MTX);
                            HDCPTX_THREAD_RET = ERR_OK;
                            MUTEX_UNLOCK(THREAD_MTX);
                            if (ret != ERR_OK) {
                                ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                            }
                            #endif
                            goto _LOOP_END;
                        }
                        MES("  Downstream count: %d", (int)auto_hdmitx_hdcp_thread_rep_auth.downstream_count);
                        MES("  Downstream depth: %d", (int)auto_hdmitx_hdcp_thread_rep_auth.downstream_depth);
                        for (i=0; i<auto_hdmitx_hdcp_thread_rep_auth.downstream_count; i++) {
                            MES("    ksv[%d]: %02X %02X %02X %02X %02X", (int)i, auto_hdmitx_hdcp_thread_rep_auth.ksv_list[i][0],auto_hdmitx_hdcp_thread_rep_auth.ksv_list[i][1],auto_hdmitx_hdcp_thread_rep_auth.ksv_list[i][2],auto_hdmitx_hdcp_thread_rep_auth.ksv_list[i][3],auto_hdmitx_hdcp_thread_rep_auth.ksv_list[i][4]);
                        }
                        MES("  Bstatus: %02X %02X", auto_hdmitx_hdcp_thread_rep_auth.bstatus[0],auto_hdmitx_hdcp_thread_rep_auth.bstatus[1]);
                        // TODO: You must check for ksv_list in the current revocation list.
                    }
                    // call hdmitx_hdcp_get_authentication_status() periodically
                                                                                        MES("MYH %s, create hdcptx_thread()  %d", __func__, __LINE__);
                    // if (hdcptx_auth_auto_unmute == ENABLE_UNMUTE) {
                                                                                        MES("MYH %s, if (hdcptx_auth_auto_unmute ==  %d", __func__, __LINE__);
                    //     // hdmitx_video_mute(OFF)
                        MES("unmute HDMI Tx video");
                    //     mute.port = port;
                    //     mute.mute = SYSTEM_TX_MUTE_OFF;
                    //     MUTEX_LOCK2();
                    //     ret = hdmitx_video_mute(&mute);
                    //     MUTEX_UNLOCK2();
                    //     if (ret != ERR_OK) {
                    //         ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                    //         goto _LOOP_END;
                    //     }
                    // }
                    // if (hdcptx_auth_auto_unmute == ENABLE_UNMUTE) {
                                                                                        MES("MYH %s, if (hdcptx_auth_auto_unmute ==  %d", __func__, __LINE__);
                    //     // hdmitx_audio_mute(OFF)
                        MES("unmute HDMI Tx audio");
                    //     mute.port = port;
                    //     mute.mute = SYSTEM_TX_MUTE_OFF;
                    //     MUTEX_LOCK2();
                    //     ret = hdmitx_audio_mute(&mute);
                    //     MUTEX_UNLOCK2();
                    //     if (ret != ERR_OK) {
                    //         ERR(API_ERROR_OCCURRED, "hdmitx_audio_mute", hv_strerr(ret));
                    //         goto _LOOP_END;
                    //     }
                    // }
                } else {
                                                                                        MES("MYH %s, else %d", __func__, __LINE__);
                    // HPD and/or Rx Sense are inactive
                    // hdmitx_video_mute(ON)
                    MES("mute HDMI Tx video");
                    // mute.port = port;
                    // mute.mute = SYSTEM_TX_MUTE_ON;
                    // MUTEX_LOCK2();
                    // ret = hdmitx_video_mute(&mute);
                    // MUTEX_UNLOCK2();
                    // if (ret != ERR_OK) {
                    //     ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
                    //     goto _LOOP_END;
                    // }
                    // // hdmitx_audio_mute(ON)
                    MES("mute HDMI Tx audio");
                    // mute.port = port;
                    // mute.mute = SYSTEM_TX_MUTE_ON;
                    // MUTEX_LOCK2();
                    // ret = hdmitx_audio_mute(&mute);
                    // MUTEX_UNLOCK2();
                    // if (ret != ERR_OK) {
                    //     ERR(API_ERROR_OCCURRED, "hdmitx_audio_mute", hv_strerr(ret));
                    //     goto _LOOP_END;
                    // }
                    // hdmitx_hdcp_authentication_reset()
                    MES("reset HDCP Tx");
                    // HDCPTX_THREAD_CANCEL();
                    MUTEX_LOCK2();
                                                                                        MES("MYH %s, else %d", __func__, __LINE__);
                    ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
                    MUTEX_UNLOCK2();
                    // init hdcptx_thread_ret
                    MUTEX_LOCK(THREAD_MTX);
                    HDCPTX_THREAD_RET = ERR_OK;
                    MUTEX_UNLOCK(THREAD_MTX);
                    if (ret != ERR_OK) {
                        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                        goto _LOOP_END;
                    }
                }
                pre_stat = cur_stat;
            }
_LOOP_END:
            if(hdcp_detect_thread) {
                THREAD_CREATE(HDCPTX_THREAD_ID, NULL, HDCPTX_THREAD, (void *)&auto_hdmitx_hdcp_thread_auth);
                hdcp_detect_thread = 0;
            }

            SLEEP(100);
        }
        MES("auto_hdmitx_hdcp_thread is exit");
        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel auto_hdmitx_hdcp_thread()
    //--------------------------------------------------------------------------
    void auto_hdmitx_hdcp_thread_cancel(void)
    {
        MES("auto_hdmitx_hdcp_thread_id = %d", auto_hdmitx_hdcp_thread_id);
        if (auto_hdmitx_hdcp_thread_id != 0) {
            MES("MYH %s %d, into hdcptx_thread_cancel()", __func__, __LINE__);
            HDCPTX_THREAD_CANCEL();

            MES("MYH %s %d, hdcptx_thread is quit", __func__, __LINE__);
            THREAD_CANCEL(auto_hdmitx_hdcp_thread_id);
            MES("MYH %s %d, cancel hdcptx_thread, join hdcptx_thread exit", __func__, __LINE__);
            auto_hdmitx_hdcp_thread_run = 0;
            THREAD_JOIN(auto_hdmitx_hdcp_thread_id, NULL);
            MES("MYH %s %d, hdcptx_thread exit", __func__, __LINE__);
            auto_hdmitx_hdcp_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define AUTO_HDMITX_HDCP_THREAD                 auto_hdmitx_hdcp_thread
    #define AUTO_HDMITX_HDCP_THREAD_CANCEL()        auto_hdmitx_hdcp_thread_cancel()
    #define AUTO_HDMITX_HDCP_THREAD_ID              auto_hdmitx_hdcp_thread_id
    #define AUTO_HDMITX_HDCP_THREAD_AUTH            auto_hdmitx_hdcp_thread_auth
    #define AUTO_HDMITX_HDCP_THREAD_REP_AUTH        auto_hdmitx_hdcp_thread_rep_auth
    //--------------------------------------------------------------------------
#endif // Product Number having HDMI Tx

int LF9129_HDCP_Mute(int bEnable)
{
    is_hdcp_enable = (bEnable == SYSTEM_TX_MUTE_OFF ? 1 : 0);
    uint32_t ret;
    cmds_main cmd;

    system_port port;
    hdmitx_video *p_vform;
    p_vform = (hdmitx_video *)&vform.htx;
    port = p_vform->tx_port;

    // hdmitx_video_mute(ON)
    // cmd_mute_video_hdmitx_avmute(&cmd, txp);
    // cmd_mute_audio_hdmitx(&cmd, txp);

    SLEEP(200);

    // hdmitx_hdcp_authentication_reset()
    MES("reset HDCP Tx");
    HDCPTX_THREAD_CANCEL();
    MUTEX_LOCK2();
                                                MES("HDCP SWITCH MYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
    ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
    MUTEX_UNLOCK2();
                                                MES("HDCP SWITCH MYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
    // init hdcptx_thread_ret
    MUTEX_LOCK(THREAD_MTX);
                                                MES("HDCP SWITCH MYH %s, MUTEX_LOCK();  %d", __func__, __LINE__);
    HDCPTX_THREAD_RET = ERR_OK;
    MUTEX_UNLOCK(THREAD_MTX);
                                                MES("HDCP SWITCH MYH %s, MUTEX_UNLOCK();  %d", __func__, __LINE__);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
    }

    if(bEnable == SYSTEM_TX_MUTE_OFF)
    {
        // hdmitx_hdcp_authentication()
        MES("set HDCP Tx");
        MUTEX_LOCK(THREAD_MTX);
                                                        MES("HDCP SWITCH MYH %s, MUTEX_LOCK();  %d", __func__, __LINE__);
        auto_hdmitx_hdcp_thread_auth.port = port;
        MUTEX_UNLOCK(THREAD_MTX);
                                                        MES("HDCP SWITCH MYH %s, MUTEX_UNLOCK();  %d", __func__, __LINE__);
        HDCPTX_THREAD_CANCEL();
        MUTEX_LOCK2();
                                                        MES("HDCP SWITCH MYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);

        for(int i = 0; i < HDCPTX22_RECEIVER_ID_BYTE_SIZE; ++i)
        {
            auto_hdmitx_hdcp_thread_auth.bksv[i] = 0;
        }

        ret = hdmitx_hdcp_authentication(&auto_hdmitx_hdcp_thread_auth);
        MUTEX_UNLOCK2();
        MES("HDCP SWITCH HDCP %s", ((auto_hdmitx_hdcp_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_1_4) ? "1.4": (auto_hdmitx_hdcp_thread_auth.hdcp_version == HDMITX_HDCP_VERSION_2_2) ? "2.2": "non-compliant"));
        MES("HDCP SWITCH   Sink device: %s", ((auto_hdmitx_hdcp_thread_auth.is_repeater == HDMITX_HDCP_REPEATER) ? "Repeater": "NOT Repeater"));
        MES("HDCP SWITCH   bksv: %02X %02X %02X %02X %02X", auto_hdmitx_hdcp_thread_auth.bksv[0],auto_hdmitx_hdcp_thread_auth.bksv[1],auto_hdmitx_hdcp_thread_auth.bksv[2],auto_hdmitx_hdcp_thread_auth.bksv[3],auto_hdmitx_hdcp_thread_auth.bksv[4]);
    } else {
        is_hdcp_enable = 0;
    }


    // hdmitx_video_mute(OFF)
    // cmd_unmute_video_hdmitx_avmute(&cmd, txp);
    // cmd_unmute_audio_hdmitx(&cmd, txp);

    return ret;
}

int LF9129_Get_HDCP_Mute(char* info)
{
    *info = is_hdcp_enable;
    return 0;
}

#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    //--------------------------------------------------------------------------
    // Sample of Port Status of HDMI Rx
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE port_thread_id = 0;
    static hdmirx_port_status port_thread_status = {{0}, HDMIRX_PORT_STATUS_CONNECTION_INACTIVE,
                                                        HDMIRX_PORT_STATUS_CONNECTION_INACTIVE,
                                                        HDMIRX_PORT_STATUS_CONNECTION_INACTIVE,
                                                        HDMIRX_PORT_STATUS_CONNECTION_INACTIVE};
    THREAD_RET_TYPE THREAD_ATTR port_thread(void *p_arg)
    {
        hdmirx_port_status pre_stat;
        uint32_t ret;

        port_thread_status.port = *(system_port *)p_arg;
        pre_stat.port0 = port_thread_status.port0 = 0;
        pre_stat.port1 = port_thread_status.port1 = 0;
        pre_stat.port2 = port_thread_status.port2 = 0;
        pre_stat.port3 = port_thread_status.port3 = 0;

        while (1) {
            MUTEX_LOCK2();
            ret = hdmirx_get_port_status(&port_thread_status);
            MUTEX_UNLOCK2();
            if (ret != ERR_OK) {
                ERR(API_ERROR_OCCURRED, "hdmirx_get_port_status", hv_strerr(ret));
                break;
            }
            if (pre_stat.port0 != port_thread_status.port0) {
                MES("\nHDMI Rx Port 0: %sactive", (port_thread_status.port0) ? "" : "in");
            }
            if (pre_stat.port1 != port_thread_status.port1) {
                MES("\nHDMI Rx Port 1: %sactive", (port_thread_status.port1) ? "" : "in");
            }
            if (pre_stat.port2 != port_thread_status.port2) {
                MES("\nHDMI Rx Port 2: %sactive", (port_thread_status.port2) ? "" : "in");
            }
            if (pre_stat.port3 != port_thread_status.port3) {
                MES("\nHDMI Rx Port 3: %sactive", (port_thread_status.port3) ? "" : "in");
            }
            pre_stat.port0 = port_thread_status.port0;
            pre_stat.port1 = port_thread_status.port1;
            pre_stat.port2 = port_thread_status.port2;
            pre_stat.port3 = port_thread_status.port3;
            LF9129_CMD_Delay(1000);
        }

        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel port_thread()
    //--------------------------------------------------------------------------
    void port_thread_cancel(void)
    {
        if (port_thread_id != 0) {
            THREAD_CANCEL(port_thread_id);
            THREAD_JOIN(port_thread_id, NULL);
            port_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define PORT_THREAD                    port_thread
    #define PORT_THREAD_CANCEL()        port_thread_cancel()
    #define PORT_THREAD_ID                port_thread_id
    #define PORT_THREAD_STATUS            port_thread_status
    //--------------------------------------------------------------------------
    // Sample of Audio/Video Status of HDMI Rx
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE av_thread_id = 0;
    static hdmirx_av_status av_thread_status = {{0}, HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED, HDMIRX_AV_STATUS_MUTE_ON,
                                                HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED, HDMIRX_AV_STATUS_MUTE_ON};
    THREAD_RET_TYPE THREAD_ATTR av_thread(void *p_arg)
    {
        hdmirx_av_status pre_stat = {{0}, HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED, HDMIRX_AV_STATUS_MUTE_ON,
                                        HDMIRX_AV_STATUS_FORMAT_NOT_CHANGED, HDMIRX_AV_STATUS_MUTE_ON};
        uint32_t ret;

        av_thread_status.port = *(system_port *)p_arg;

        while (1) {
            MUTEX_LOCK2();
            ret = hdmirx_get_av_status(&av_thread_status);
            MUTEX_UNLOCK2();
            if (ret != ERR_OK) {
                ERR(API_ERROR_OCCURRED, "hdmirx_get_av_status", hv_strerr(ret));
                break;
            }
            if (av_thread_status.video_format != pre_stat.video_format) {
                if (av_thread_status.video_format == HDMIRX_AV_STATUS_FORMAT_CHANGED) {
                    MES("\nHDMI Rx Port %d: Video Format change", av_thread_status.port.port_num);
                } else {
                    ;
                }
            }
            if (av_thread_status.video_mute != pre_stat.video_mute) {
                if (av_thread_status.video_mute == HDMIRX_AV_STATUS_MUTE_OFF) {
                    MES("\nHDMI Rx Port %d: Video unmuted", av_thread_status.port.port_num);
                } else {
                    MES("\nHDMI Rx Port %d: Video muted", av_thread_status.port.port_num);
                }
            }
            if (av_thread_status.audio_format != pre_stat.audio_format) {
                if (av_thread_status.audio_format == HDMIRX_AV_STATUS_FORMAT_CHANGED) {
                    MES("\nHDMI Rx Port %d: Audio Format might have changed", av_thread_status.port.port_num);
                } else {
                    ;
                }
            }
            if (av_thread_status.audio_mute != pre_stat.audio_mute) {
                if (av_thread_status.audio_mute == HDMIRX_AV_STATUS_MUTE_OFF) {
                    MES("\nHDMI Rx Port %d: Audio unmuted", av_thread_status.port.port_num);
                } else {
                    MES("\nHDMI Rx Port %d: Audio muted", av_thread_status.port.port_num);
                }
            }
            pre_stat.video_format = av_thread_status.video_format;
            pre_stat.video_mute = av_thread_status.video_mute;
            pre_stat.audio_format = av_thread_status.audio_format;
            pre_stat.audio_mute = av_thread_status.audio_mute;
            LF9129_CMD_Delay(100);
        }

        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel av_thread()
    //--------------------------------------------------------------------------
    void av_thread_cancel(void)
    {
        if (av_thread_id != 0) {
            THREAD_CANCEL(av_thread_id);
            THREAD_JOIN(av_thread_id, NULL);
            av_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define AV_THREAD                    av_thread
    #define AV_THREAD_CANCEL()            av_thread_cancel()
    #define AV_THREAD_ID                av_thread_id
    #define AV_THREAD_STATUS            av_thread_status
    //--------------------------------------------------------------------------
    // Sample of HDCP Authentication Status of HDMI Rx
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE hdcprx_thread_id = 0;
    static hdmirx_hdcp_auth_status hdcprx_thread_status = {{0}, HDMIRX_HDCP_NOT_REPEATER,
                                                                HDMIRX_HDCP_EESS_NONE,
                                                                HDMIRX_HDCP_MESSAGE_NONE,
                                                                HDMIRX_HDCP_ERROR_NONE};
    THREAD_RET_TYPE THREAD_ATTR hdcprx_thread(void *p_arg)
    {
        hdmirx_hdcp_auth *p_auth;
        hdmirx_hdcp_eess eess = HDMIRX_HDCP_EESS_NONE;
        hdmirx_hdcp_message msg = HDMIRX_HDCP_MESSAGE_NONE;
        uint32_t stime = 1;
        uint32_t ret;

        p_auth = (hdmirx_hdcp_auth *)p_arg;
        hdcprx_thread_status.port = p_auth->port;
        hdcprx_thread_status.repeater = p_auth->repeater;

        while (1) {
            MUTEX_LOCK2();
            ret = hdmirx_hdcp_get_authentication_status(&hdcprx_thread_status);
            MUTEX_UNLOCK2();
            if (ret != ERR_OK) {
                if (ret == ERR_HDCPRXXX_PACKET_HEADER_BCH_ERROR) {
                    WAR("HDCP Rx: packet header BCH error");
                } else if (ret == ERR_HDCPRXXX_PACKET_BODY_BCH_ERROR) {
                    WAR("HDCP Rx: packet body BCH error");
                } else {
                    ERR("HDCP Rx: %s", hv_strerr(ret));
                }
            }
            if (hdcprx_thread_status.eess != eess) {
                if (hdcprx_thread_status.eess == HDMIRX_HDCP_EESS_ENC_DIS) {
                    MES("EESS: ENC_DIS");
                    stime = 1;
                } else if (hdcprx_thread_status.eess == HDMIRX_HDCP_EESS_ENC_EN) {
                    MES("EESS: ENC_EN");
                    stime = 1000;
                }
                eess = hdcprx_thread_status.eess;
            }
            if (hdcprx_thread_status.repeater == HDMIRX_HDCP_REPEATER) {
                if (hdcprx_thread_status.message != msg) {
                    switch (hdcprx_thread_status.message) {
                        case HDMIRX_HDCP_MESSAGE_AKSV:
                            MES("Received Message: Aksv");
                            break;
                        case HDMIRX_HDCP_MESSAGE_AKE_INIT:
                            MES("Received Message: AKE_Init");
                            break;
                        case HDMIRX_HDCP_MESSAGE_AKE_NO_STORED_KM:
                            MES("Received Message: AKE_No_Stored_km");
                            break;
                        case HDMIRX_HDCP_MESSAGE_AKE_STORED_KM:
                            MES("Received Message: AKE_Stored_km");
                            break;
                        case HDMIRX_HDCP_MESSAGE_LC_INIT:
                            MES("Received Message: LC_Init");
                            break;
                        case HDMIRX_HDCP_MESSAGE_SKE_SEND_EKS:
                            MES("Received Message: SKE_Send_Eks");
                            break;
                        case HDMIRX_HDCP_MESSAGE_REPEATERAUTH_SEND_ACK:
                            MES("Received Message: RepeaterAuth_Send_Ack");
                            break;
                        case HDMIRX_HDCP_MESSAGE_REPEATERAUTH_STREAM_MANAGE:
                            MES("Received Message: RepeaterAuth_Stream_Manage");
                            break;
                        default:
                            break;
                    }
                    msg = hdcprx_thread_status.message;
                }
            }
            LF9129_CMD_Delay(stime);
        }

        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel hdcprx_thread()
    //--------------------------------------------------------------------------
    void hdcprx_thread_cancel(void)
    {
        if (hdcprx_thread_id != 0) {
            THREAD_CANCEL(hdcprx_thread_id);
            THREAD_JOIN(hdcprx_thread_id, NULL);
            hdcprx_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define HDCPRX_THREAD                hdcprx_thread
    #define HDCPRX_THREAD_CANCEL()        hdcprx_thread_cancel()
    #define HDCPRX_THREAD_ID            hdcprx_thread_id
    #define HDCPRX_THREAD_STATUS        hdcprx_thread_status
    //--------------------------------------------------------------------------
    // Sample of HDMI Rx auto processing
    //--------------------------------------------------------------------------
    static THREAD_ID_TYPE auto_hdmirx_thread_id = 0;
    THREAD_RET_TYPE THREAD_ATTR auto_hdmirx_thread(void *p_arg)
    {
        system_videos *p_vform;
        system_videos pre_vform = {
            {
                {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
                {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
                SYSTEM_VFORM_UNUSED, SYSTEM_COLOR_MAX, SYSTEM_DEEP_COLOR_MAX,
                SYSTEM_VFORM_UNUSED, SYSTEM_COLOR_MAX, SYSTEM_DEEP_COLOR_MAX
            }
        };
        system_port port;
        system_audio aform;
        system_audio pre_aform = {
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
            SYSTEM_AUDIO_CC_MAX, SYSTEM_AUDIO_SF_MAX, SYSTEM_AUDIO_SS_MAX, 0xFF, 0xF,
            SYSTEM_AUDIO_SF_MAX, SYSTEM_AUDIO_SS_MAX, SYSTEM_AUDIO_SF_RATIO_MAX,
            {SYSTEM_AUDIO_EM_MAX, SYSTEM_AUDIO_PO_MAX, SYSTEM_AUDIO_IF_MAX, SYSTEM_AUDIO_SP_MAX}
        };
        hdmirx_port_status_connection pstat;
        hdmirx_hdcp_eess eess;
        hdmirx_hdcp_error err;
        hdmirx_av_status_format asform, vsform;
        hdmirx_av_status_mute asmute, vsmute;
        uint32_t ret;

        p_vform = (system_videos *)p_arg;
        port = p_vform->sys.rx_port;
        aform.port = port;

        while (1) {
            MUTEX_LOCK(THREAD_MTX);
            pstat = (port.port_num == 0) ? PORT_THREAD_STATUS.port0
                        : (port.port_num == 1) ? PORT_THREAD_STATUS.port1
                        : (port.port_num == 2) ? PORT_THREAD_STATUS.port2
                        : PORT_THREAD_STATUS.port3;
            MUTEX_UNLOCK(THREAD_MTX);
            if (pstat == HDMIRX_PORT_STATUS_CONNECTION_ACTIVE) {
                MUTEX_LOCK(THREAD_MTX);
                eess = HDCPRX_THREAD_STATUS.eess;
                err = HDCPRX_THREAD_STATUS.error;
                MUTEX_UNLOCK(THREAD_MTX);
                if ((err == HDMIRX_HDCP_ERROR_NONE)
                    && ((eess == HDMIRX_HDCP_EESS_NONE) || (eess == HDMIRX_HDCP_EESS_ENC_EN))) {
                    MUTEX_LOCK(THREAD_MTX);
                    vsform = AV_THREAD_STATUS.video_format;
                    vsmute = AV_THREAD_STATUS.video_mute;
                    MUTEX_UNLOCK(THREAD_MTX);
                    if (vsmute == HDMIRX_AV_STATUS_MUTE_OFF
                        && vsform == HDMIRX_AV_STATUS_FORMAT_CHANGED) {
                        MUTEX_LOCK2();
                        ret = hdmirx_get_video_format((hdmirx_video *)p_vform);
                        MUTEX_UNLOCK2();
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmirx_get_video_format", hv_strerr(ret));
                        }
                        if (memcmp((void *)&pre_vform, (void *)p_vform, sizeof(pre_vform))) {
                            print_video_format(p_vform->sys.rx_video_format,
                                                p_vform->sys.rx_color_format,
                                                p_vform->sys.rx_deep_color);
                        }
                        memcpy((void *)&pre_vform, (void *)p_vform, sizeof(pre_vform));
                    }
                    MUTEX_LOCK(THREAD_MTX);
                    asform = AV_THREAD_STATUS.audio_format;
                    asmute = AV_THREAD_STATUS.audio_mute;
                    MUTEX_UNLOCK(THREAD_MTX);
                    if (asmute == HDMIRX_AV_STATUS_MUTE_OFF
                        && asform == HDMIRX_AV_STATUS_FORMAT_CHANGED) {
                        MUTEX_LOCK2();
                        ret = hdmirx_get_audio_format((hdmirx_audio *)&aform);
                        MUTEX_UNLOCK2();
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmirx_get_audio_format", hv_strerr(ret));
                        }
                        if (memcmp((void *)&pre_aform, (void *)&aform, sizeof(aform))) {
                            print_audio_format(&aform);
                        }
                        memcpy((void *)&pre_aform, (void *)&aform, sizeof(aform));
                    }
                }
            }
            LF9129_CMD_Delay(100);
        }

        THREAD_EXIT(NULL);
        THREAD_RETURN(NULL);
    }
    //--------------------------------------------------------------------------
    // Cancel auto_hdmirx_thread()
    //--------------------------------------------------------------------------
    void auto_hdmirx_thread_cancel(void)
    {
        if (auto_hdmirx_thread_id != 0) {
            THREAD_CANCEL(auto_hdmirx_thread_id);
            THREAD_JOIN(auto_hdmirx_thread_id, NULL);
            auto_hdmirx_thread_id = 0;
        }
    }
    //--------------------------------------------------------------------------
    #define AUTO_HDMIRX_THREAD            auto_hdmirx_thread
    #define AUTO_HDMIRX_THREAD_CANCEL()    auto_hdmirx_thread_cancel()
    #define AUTO_HDMIRX_THREAD_ID        auto_hdmirx_thread_id
    //--------------------------------------------------------------------------
#endif // Product Number having HDMI Rx

#if defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    #define HPD_THREAD
    #define HPD_THREAD_CANCEL()
    #define HPD_THREAD_ID
    #define HPD_THREAD_STAT
    #define HDCPTX_THREAD
    #define HDCPTX_THREAD_CANCEL()
    #define HDCPTX_THREAD_ID
    #define HDCPTX_THREAD_RET
    #define AUTO_HDMITX_SG_THREAD
    #define AUTO_HDMITX_SG_THREAD_CANCEL()
    #define AUTO_HDMITX_SG_THREAD_ID
    #define AUTO_HDMITX_SG_THREAD_AUTH
    #define AUTO_HDMITX_SG_THREAD_REP_AUTH
#endif // Product Number not having HDMI Tx

#if defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    #define PORT_THREAD
    #define PORT_THREAD_CANCEL()
    #define PORT_THREAD_ID
    #define PORT_THREAD_STATUS
    #define AV_THREAD
    #define AV_THREAD_CANCEL()
    #define AV_THREAD_ID
    #define AV_THREAD_STATUS
    #define HDCPRX_THREAD
    #define HDCPRX_THREAD_CANCEL()
    #define HDCPRX_THREAD_ID
    #define HDCPRX_THREAD_STATUS
    #define AUTO_HDMIRX_THREAD
    #define AUTO_HDMIRX_THREAD_CANCEL()
    #define AUTO_HDMIRX_THREAD_ID
#endif // Product Number not having HDMI Rx
#else // __linux _WIN32
    #define HPD_THREAD
    #define HPD_THREAD_CANCEL()
    #define HPD_THREAD_ID
    #define HPD_THREAD_STAT
    #define HDCPTX_THREAD
    #define HDCPTX_THREAD_CANCEL()
    #define HDCPTX_THREAD_ID
    #define HDCPTX_THREAD_RET
    #define AUTO_HDMITX_SG_THREAD
    #define AUTO_HDMITX_SG_THREAD_CANCEL()
    #define AUTO_HDMITX_SG_THREAD_ID
    #define AUTO_HDMITX_SG_THREAD_AUTH
    #define AUTO_HDMITX_SG_THREAD_REP_AUTH
    #define PORT_THREAD
    #define PORT_THREAD_CANCEL()
    #define PORT_THREAD_ID
    #define PORT_THREAD_STATUS
    #define AV_THREAD
    #define AV_THREAD_CANCEL()
    #define AV_THREAD_ID
    #define AV_THREAD_STATUS
    #define HDCPRX_THREAD
    #define HDCPRX_THREAD_CANCEL()
    #define HDCPRX_THREAD_ID
    #define HDCPRX_THREAD_STATUS
    #define AUTO_HDMIRX_THREAD
    #define AUTO_HDMIRX_THREAD_CANCEL()
    #define AUTO_HDMIRX_THREAD_ID
#endif // __linux _WIN32

#define LVDS_OUTPUT 0
//------------------------------------------------------------------------------
// Init HDMI Tx
//------------------------------------------------------------------------------
static uint32_t set_hpd_thread_hdmitx(system_port port)
{
    uint32_t ret;
    MES("LF9129 set_hpd_thread_hdmitx");
    AUTO_HDMITX_SG_THREAD_CANCEL();
    HPD_THREAD_CANCEL();
    HDCPTX_THREAD_CANCEL();
    // call hdmitx_get_hpd_status() periodically
    THREAD_CREATE(HPD_THREAD_ID, NULL, HPD_THREAD, (void *)&port);
    return ret;
}

static uint32_t init_hdmitx_settings()
{
    int ret;
    initOK = 0;
    //AUTO_HDMITX_THREAD_CANCEL();
    //HPD_THREAD_CANCEL();
    //HDCPTX_THREAD_CANCEL();

    system_port rxp2 = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port txp2 = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port txp_vbo2 = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_videos vform_vbo2 = {
        {
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            HDMITX_DVI_MODE_DISABLE
        }
    };
    
    system_videos vform2 = {
        {
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_VBO, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
            {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
            SYSTEM_VFORM_3840x2160P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            SYSTEM_VFORM_1920x1080P_5994or60_16v9, SYSTEM_COLOR_RGB_FULL_RANGE1, SYSTEM_DEEP_COLOR_8BIT,
            HDMITX_DVI_MODE_DISABLE
        }
    };

    system_port au_rxp2 = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0
    };
    
    system_port au_txp2 = {
        SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0
    };

    system_audio rx_aform2 = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_RX, SYSTEM_CHIP_NUMBER_0},
        SYSTEM_AUDIO_CC_RSH, SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, 0x00, 0x0,
        SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, SYSTEM_AUDIO_SF_RATIO_1_1,
        {SYSTEM_AUDIO_EM_MSB, SYSTEM_AUDIO_PO_LOW, SYSTEM_AUDIO_IF_DISABLE, SYSTEM_AUDIO_SP_RIGHT}
    };

    system_audio tx_aform2 = {
        {SYSTEM_PORT_NUMBER_0, 0, SYSTEM_PORT_TYPE_HDMI, SYSTEM_RXTX_TYPE_TX, SYSTEM_CHIP_NUMBER_0},
        SYSTEM_AUDIO_CC_RSH, SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, 0x00, 0x0,
        SYSTEM_AUDIO_SF_RSH, SYSTEM_AUDIO_SS_RSH, SYSTEM_AUDIO_SF_RATIO_1_1,
        {SYSTEM_AUDIO_EM_MSB, SYSTEM_AUDIO_PO_LOW, SYSTEM_AUDIO_IF_DISABLE, SYSTEM_AUDIO_SP_RIGHT}
    };

    MES("LF9129 set_hdmiout_res_value = %d",set_hdmiout_res_value);
    if (set_hdmiout_res_value == 0)
    {
        vform2.htx.tx_video_format = SYSTEM_VFORM_720x480P_5994or60_4v3;
        vform2.htx.tx_color_format = SYSTEM_COLOR_RGB_LIMITED_RANGE;
    }
    else if(set_hdmiout_res_value == 1)
    {
        vform2.htx.tx_video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
        vform2.htx.tx_color_format = SYSTEM_COLOR_RGB_LIMITED_RANGE;
    }
    else if(set_hdmiout_res_value == 2)
    {
        vform2.htx.tx_video_format = SYSTEM_VFORM_3840x2160P_5994or60_16v9;
        vform2.htx.tx_color_format = SYSTEM_COLOR_RGB_FULL_RANGE1;
    }
    cmds_main cmd;
    cmd_init_vborx(&cmd, rxp2);
    cmd_init_vbotx(&cmd, txp_vbo2);
    cmd_init_hdmitx(&cmd, txp2);
    cmd_init_spdifrx(&cmd, au_rxp2);

    cmd_set_video_vborx(&cmd, &vform2);
    cmd_set_video_vbotx(&cmd, &vform_vbo2);
    cmd_unmute_video_vbotx_system(&cmd, txp_vbo2);

    cmd_get_audio_spdifrx(&cmd, &au_rxp2, &au_txp2, &rx_aform2, &tx_aform2);
    cmd_set_video_hdmitx(&cmd,    &vform2);
    cmd_set_audio_hdmitx(&cmd,    &tx_aform2);

    auto_process_hdmitx(txp2);
    initOK = 1;

    return ret;
}

int LF9129_detect(void)
{
    version();
    uint8_t u8hpd_thread_stat;
    return hdmitx_get_hpd_status(txp, &u8hpd_thread_stat);
    //return do_reset_auther_hdcp();
}
hdmitx_sg_video g_tx_vform;

#if 0
int LF9129_main(void)
{
     cmds_main cmd;
     int ret = 0;
     int i = 0, k=0;
     uint8_t uTotal = 0;
     uint8_t uSupport = 0;
     uint8_t uSupport480p = 0;
     uint8_t uSupport1080p = 0;
     uint8_t uSupport4k2k = 0;
     uint8_t hpd_thread_stat_first = 0;

    TERMINAL_MODE_DECLARATION;

    // show version
    //version();

    // init mutex
    MUTEX_INIT(THREAD_MTX, NULL);
    #if 0
    if (getenv ("hdmi_out_resolution")!= NULL)
    {
        set_hdmiout_res_value = (int)simple_strtol(getenv ("hdmi_out_resolution"), NULL, 10); 
    }
    else
    {
        set_hdmiout_res_value = 1;
    }

    if (getenv ("hdmi_out_hdcp_disable")!= NULL)
    {
        is_hdcp_enable = (int)simple_strtol(getenv ("hdmi_out_hdcp_disable"), NULL, 10); 
    }
    else
    {
        is_hdcp_enable = 0;
    }
    #endif

    do_reset_auther_hdcp();

    cmd_init_vborx(&cmd, rxp);
    cmd_init_vbotx(&cmd, txp_vbo);
    cmd_init_hdmitx(&cmd, txp);
    cmd_init_spdifrx(&cmd, au_rxp);
    #if 1
    cmd_read_edid(&cmd, txp);
    MES("edid_table[0x7E] = %0d \n",edid_table[0x7E]);
    if (edid_table[0x7E] == 0)
    {
        vform.htx.tx_dvi_mode = HDMITX_DVI_MODE_DISABLE;
    }
    else if(edid_table[0x7E]  >= 1)
    {
          for(i = 0; i < 126; i ++)
          {
            if (edid_extend_table[i] == 0x03 && edid_extend_table[i+1] == 0x0c && edid_extend_table[i+2] == 0x00)
            {
                vform.htx.tx_dvi_mode = HDMITX_DVI_MODE_DISABLE;
                break;
            }
        }
        MES("edid_extend_table[i] = %0d \n",edid_extend_table[i]);
        MES("edid_extend_table[i+1] = %0d \n",edid_extend_table[i+1]);
        MES("edid_extend_table[i+2] = %0d \n",edid_extend_table[i+2]);
        MES("i = %0d \n",i);
        if (i >= 126)
        {
            vform.htx.tx_dvi_mode = HDMITX_DVI_MODE_ENABLE;
        }
    }

    uTotal = edid_extend_table[132-128]-0x40;
    for(k=0;k<uTotal;k++)
    {
        uSupport = edid_extend_table[133 - 128 +k];
        if(uSupport == 0x03)
        {
            printf("LF9129 ++++ support 480P\n");
            uSupport480p = 1;
        }
        else if(uSupport == 0x90 || uSupport == 0x10)
        {
            printf("LF9129 ++++ support 1080P\n");
            uSupport1080p = 1;
        }
        else if(uSupport == 0x61)
        {
            printf("LF9129 ++++ support 2160P\n");
            uSupport4k2k = 1;
        }
        else if(uSupport == 0x66){
            printf("LF9129 ++++ support 4096x2160\n");
            uSupport4k2k = 1;
        }
    }

    MES("LF9129 set_hdmiout_res_value = %d \n", set_hdmiout_res_value);

    if (set_hdmiout_res_value == 0)
    {
        vform.htx.tx_video_format = SYSTEM_VFORM_720x480P_5994or60_4v3;
    }
    else if(set_hdmiout_res_value == 1)
    {
        vform.htx.tx_video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
    }
    else if(set_hdmiout_res_value == 2)
    {
        vform.htx.tx_video_format = SYSTEM_VFORM_3840x2160P_5994or60_16v9;
    }
    #endif
    cmd_set_video_vborx(&cmd, &vform);
    cmd_set_video_vbotx(&cmd, &vform_vbo);
    cmd_unmute_video_vbotx_system(&cmd, txp_vbo);

    cmd_get_audio_spdifrx(&cmd, &au_rxp, &au_txp, &rx_aform, &tx_aform);

    cmd_set_video_hdmitx(&cmd,    &vform);
    cmd_set_audio_hdmitx(&cmd,    &tx_aform);
    if(is_hdcp_enable == 1)
    {
        // cmd_set_hdcptx(&cmd,  txp);
        cmd_auto_hdmitx_hdcp(&cmd, 0, 0, &vform, 0, 0);
    }
    cmd_unmute_video_hdmitx_system(&cmd, txp);
    cmd_unmute_audio_hdmitx(&cmd, txp);
    


//_END:
    AUTO_HDMITX_SG_THREAD_CANCEL();
    HPD_THREAD_CANCEL();
    HDCPTX_THREAD_CANCEL();
    AUTO_HDMIRX_THREAD_CANCEL();
    PORT_THREAD_CANCEL();
    AV_THREAD_CANCEL();
    HDCPRX_THREAD_CANCEL();
    MUTEX_DESTROY(THREAD_MTX);
    return 0;
}
#endif

uint32_t do_reset_auther_hdcp(void)
{
    uint32_t ret = 0;
    hdmitx_hdcp_auth hdcp14;
    hdcp14.port = txp;
    hdcp14.hdcp_version = HDMITX_HDCP_VERSION_NON_COMPLIANT;
    if(hdmitx_hdcp_authentication_reset(&hdcp14) == ERR_OK)
    {
        MES("pangm_hdmiout,hdmitx_hdcp_authentication_reset ok!!!!");
        printf("pangm_hdmiout,hdmitx_hdcp_authentication_reset ok!!!!\n");
        ret = 0;
    }
    else
    {
        MES("pangm_hdmiout,hdmitx_hdcp_authentication_reset FAIL!!!!");
        printf("pangm_hdmiout,hdmitx_hdcp_authentication_reset error!!!!\n");
        ret = 1;
    }
    return ret;
}
int LF9129_Init(void)
{
    uint32_t ret = 0;
    cmds_main cmd;
    cmds_main cmd_config1;
    cmds_main cmd_config2;
    cmds_main cmd_config3;
    cmds_main cmd_vborx;
    MES("into LF9129_Init");

    // ret |= do_reset_auther_hdcp();
    cmd_config1.opts.target2 = OPT_TARGET2_CDR_LOCK;
    cmd_config1.opts.times = 600;
    cmd_config1.opts.msec = 100;
    ret |= cmd_config_vborx(&cmd_config1);
    cmd_config2.opts.target2 = OPT_TARGET2_HOT_PLUG;
    cmd_config2.opts.times = 600;
    cmd_config2.opts.msec = 100;
    ret |= cmd_config_vbotx(&cmd_config2);
    cmd_config3.opts.target2 = OPT_TARGET2_PLL_LOCK;
    cmd_config3.opts.times = 600;
    cmd_config3.opts.msec = 100;
    ret |= cmd_config_vbotx(&cmd_config3);

    cmd_vborx.opts.init_opt = OPT_INIT_OPT_INVERSION_ON;
    ret |= cmd_init_vborx(&cmd_vborx, rxp);
    ret |= cmd_set_video_vborx(&cmd_vborx, &vform);

    ret |= cmd_init_vborx(&cmd_vborx, rxp);
    ret |= cmd_set_video_vborx(&cmd_vborx, &vform);
    //cmd_get_format_vbo_rx(&cmd_vborx, &vform);

    ret |= cmd_init_vbotx(&cmd, txp_vbo);
    ret |= cmd_set_video_vbotx(&cmd, &vform_vbo);

    ret |= cmd_unmute_video_vbotx_system(&cmd, txp_vbo);

    MES("into reset hdcp st\n");
    ret |= do_reset_auther_hdcp();
    MES("into reset hdcp end\n");

    ret |= cmd_init_hdmitx(&cmd, txp);
    ret |= cmd_init_spdifrx(&cmd, au_rxp);
    ret |= cmd_get_audio_spdifrx(&cmd, &au_rxp, &au_txp, &rx_aform, &tx_aform);
    //ret |= cmd_set_video_hdmitx(&cmd,  &vform);
    //ret |= cmd_set_audio_hdmitx(&cmd,  &tx_aform);

    //ret |= cmd_unmute_video_hdmitx_system(&cmd, txp);
    //ret |= cmd_unmute_audio_hdmitx(&cmd, txp);

    MES("into LF9129_Init end");
    return ret;
}

int LF9129_initSPDIF(void)
{
    uint32_t ret = 0;
    cmds_main cmd;
    MES("into LF9129_initSPDIF");

    ret |= cmd_init_hdmitx(&cmd, txp);
    ret |= cmd_init_spdifrx(&cmd, au_rxp);
    ret |= cmd_get_audio_spdifrx(&cmd, &au_rxp, &au_txp, &rx_aform, &tx_aform);
    //ret |= cmd_set_video_hdmitx(&cmd,  &vform);
    //ret |= cmd_set_audio_hdmitx(&cmd,  &tx_aform);

    ret |= cmd_unmute_video_vbotx_system(&cmd, txp_vbo);
    //ret |= cmd_unmute_video_hdmitx_system(&cmd, txp);
    //ret |= cmd_unmute_audio_hdmitx(&cmd, txp);

    return ret;
}

int LF9129_Set_VideoFormat(int format)
{
    uint32_t ret = 0;
    cmds_main cmd;
    system_videos hdmivform ;
    hdmivform = vform;
    hdmivform.htx.tx_video_format = format;

    if(format == SYSTEM_VFORM_3840x2160P_5994or60_16v9)
    {
        mCurrent_hdmitx_video_format = 2;
    }
    else if(format == SYSTEM_VFORM_1920x1080P_5994or60_16v9)
    {
        mCurrent_hdmitx_video_format = 1;
    }
    else if(format == SYSTEM_VFORM_720x480P_5994or60_4v3)
    {
        mCurrent_hdmitx_video_format = 0;
    }

    MES("getdebug mCurrent_hdmitx_video_format = %d\n", mCurrent_hdmitx_video_format);

    MES("MYH %s, %d, mute HDMI Tx video & audio", __func__, __LINE__);
    // cmd_mute_video_hdmitx_system(&cmd, txp);
    // cmd_mute_audio_hdmitx(&cmd, txp);


    // hdmitx_hdcp_authentication_reset()
    if (is_hdcp_enable == 1)
    {
        MES("reset HDCP Tx");
        HDCPTX_THREAD_CANCEL();
        MUTEX_LOCK2();
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
        ret |= hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
        MUTEX_UNLOCK2();
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
        // init hdcptx_thread_ret
        MUTEX_LOCK(THREAD_MTX);
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_LOCK();  %d", __func__, __LINE__);
        HDCPTX_THREAD_RET = ERR_OK;
        MUTEX_UNLOCK(THREAD_MTX);
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_UNLOCK();  %d", __func__, __LINE__);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
        }
        AUTO_HDMITX_HDCP_THREAD_CANCEL();
    }

    ret |= cmd_set_video_hdmitx(&cmd,    &hdmivform);

    MES("MYH 1 into cmd_set_audio_hdmitx\n");
    ret |= cmd_set_audio_hdmitx(&cmd,    &tx_aform);

    MES("MYH 1 into cmd_reset_hdcptx\n");
    if(is_hdcp_enable == 1)
    {
        ret |= cmd_reset_hdcptx(&cmd,  txp);
    }

    if(is_hdcp_enable == 1)
    {
        MES("cmd_set_hdcptx\n");
        // ret |= cmd_set_hdcptx(&cmd,  txp);
        ret |= cmd_auto_hdmitx_hdcp(&cmd, 0, 0, &vform, 0, 0);
    }

    ERR(API_ERROR_OCCURRED, "LF9129_Set_VideoFormat", hv_strerr(ret));
    return ret;
}

int LF9129_Get_VideoFormat(char* info)
{
    MES("getdebug main mCurrent_hdmitx_video_format is %d\n", mCurrent_hdmitx_video_format);
    if(mCurrent_hdmitx_video_format != -1)
    {
        *info = mCurrent_hdmitx_video_format;
        MES("getdebug main current video format is %d\n", *info);
    }
    else
    {
        return -1;
    }
    return 0;
}

void LF9129_Auto_VideoFormat(void)
{
    cmds_main cmd;
    uint32_t ret = 0;
    int i = 0, j = 0, k=0;
    uint8_t uTotal = 0;
    uint8_t uSupport = 0;
    uint8_t uSupport480p = 0;
    uint8_t uSupport1080p = 0;
    uint8_t uSupport4k2k = 0;
    uint8_t hdmiFlag = 0;
    uint8_t single_block = 1;
    edid_read = 1;

    MES("Mn869129 edid_extend_table = 0 st\n");

    for(int i = 0; i < 7; ++i)
    {
        for(int j = 0; j < (DDC_MAX_BUF_SIZE/2); ++j)
        {
            edid_extend_table[i][j] = 0;
        }
    }
    MES("Mn869129 edid_extend_table = 0 end\n");

    cmd_read_edid(&cmd, txp);
    if(edid_table[0x7E] == 0)
    {
        vform.htx.tx_dvi_mode = HDMITX_DVI_MODE_ENABLE;
    }
    else if(edid_table[0x7E]  >= 1)
    {
        single_block = 0;
        for(i = 0; i <= ext_block_number; ++i)
        { // 要包含block1，所以 <= ext_block_number
            for(j = 0; j < 126; ++j)
            {
                if(edid_extend_table[i][j] == 0x03 && edid_extend_table[i][j + 1] == 0x0c && edid_extend_table[i][j + 2] == 0x00)
                {
                    vform.htx.tx_dvi_mode = HDMITX_DVI_MODE_DISABLE;
                    hdmiFlag = 1;
                    break;
                }
            }
            if(hdmiFlag == 1)
            {
                break;
            }
        }
        if(i > ext_block_number ) // 证明没有HDMI规范
        {
            vform.htx.tx_dvi_mode = HDMITX_DVI_MODE_ENABLE;
        }
    }

    if(vform.htx.tx_dvi_mode == HDMITX_DVI_MODE_DISABLE) {
        MES("this is hdmitx dvi mode disable\n");
    } else if(vform.htx.tx_dvi_mode == HDMITX_DVI_MODE_ENABLE) {
        MES("this is hdmitx dvi mode enable\n");
    }

    MES("this is block0\n");
    for(i = 0; i < 8; ++i) {
        MES("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", edid_table[i*16 + 0], edid_table[i*16 + 1], edid_table[i*16 +2], edid_table[i*16 + 3], edid_table[i*16 + 4], edid_table[i*16 + 5], edid_table[i*16 + 6], edid_table[i*16 + 7], edid_table[i*16 + 8], edid_table[i*16 + 9], edid_table[i*16 + 10], edid_table[i*16 + 11], edid_table[i*16 + 12], edid_table[i*16 + 13], edid_table[i*16 + 14], edid_table[i*16 + 15]);
    }

    if (single_block == 0)
    {
        MES("this is ext block\n");
        for(i = 0; i <= ext_block_number; ++i) {
            for(j = 0; j < 8; ++j) {
                MES("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", edid_extend_table[i][j*16 + 0], edid_extend_table[i][j*16 + 1], edid_extend_table[i][j*16 +2], edid_extend_table[i][j*16 + 3], edid_extend_table[i][j*16 + 4], edid_extend_table[i][j*16 + 5], edid_extend_table[i][j*16 + 6], edid_extend_table[i][j*16 + 7], edid_extend_table[i][j*16 + 8], edid_extend_table[i][j*16 + 9], edid_extend_table[i][j*16 + 10], edid_extend_table[i][j*16 + 11], edid_extend_table[i][j*16 + 12], edid_extend_table[i][j*16 + 13], edid_extend_table[i][j*16 + 14], edid_extend_table[i][j*16 + 15]);
            }
        }

        for(i = 0; i <= ext_block_number; ++i)
        {
            int HF_EEODB_OFFSET = 0;
            if( (i == 0) && ( (edid_extend_table[0][4] >> 5) == 7) )
            {
                MES("            HF_EEODB_OFFSET = 3;");
                HF_EEODB_OFFSET = 3;
            }

            MES("edid_extend_table[i][132-128 + HF_EEODB_OFFSET] = 0x%x\n", edid_extend_table[i][132-128 + HF_EEODB_OFFSET]);
            MES("edid_extend_table[i][132-128 + HF_EEODB_OFFSET] >> 5 = 0x%x\n", edid_extend_table[i][132-128 + HF_EEODB_OFFSET] >> 5);
            if( (edid_extend_table[i][132-128 + HF_EEODB_OFFSET] >> 5) == 2 )
            {
                uTotal = edid_extend_table[i][132-128 + HF_EEODB_OFFSET] & 0x1F;
                MES("uTotal = %d\n", uTotal);
                for(k=0;k<uTotal;k++)
                {
                    uSupport = edid_extend_table[i][133 - 128 +k + HF_EEODB_OFFSET];
                    MES("uSupport = 0x%x\n", uSupport);
                    if(uSupport == 0x03 || uSupport == 0x02)
                    {
                        MES("LF9129 ++++ support 480P\n");
                        uSupport480p = 1;
                    }
                    else if(uSupport == 0x90 || uSupport == 0x10)
                    {
                        MES("LF9129 ++++ support 1080P\n");
                        uSupport1080p = 1;
                    }
                    else if(uSupport == 0x61)
                    {
                        MES("LF9129 ++++ support 2160P\n");
                        uSupport4k2k = 1;
                    }
                    else if(uSupport == 0x66)
                    {
                        MES("LF9129 ++++ support 4096x2160\n");
                        uSupport4k2k = 1;
                    }
                }
            }
        }
    }
    else
    {
        MES("EDID block number is 1, video mode is DVI, video format is 720*480\n");
    }

    if (uSupport4k2k == 1)
    {
        set_hdmiout_res_value = 2;
    }
    else if (uSupport1080p == 1)
    {
        set_hdmiout_res_value = 1;
    }
    else if (uSupport480p == 1)
    {
        set_hdmiout_res_value = 0;
    }
    else
    {
        set_hdmiout_res_value = 0;
    }

    MES("LF9129 set_hdmiout_res_value = %d \n", set_hdmiout_res_value);
    mMaxVideoFormat = set_hdmiout_res_value;

    if (set_hdmiout_res_value == 0)
    {
        cmd_mute_video_hdmitx_system(&cmd, txp);

        vform.htx.tx_video_format = SYSTEM_VFORM_720x480P_5994or60_4v3;
        mCurrent_hdmitx_video_format = 0;
    }
    else if(set_hdmiout_res_value == 1)
    {
        vform.htx.tx_video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
        mCurrent_hdmitx_video_format = 1;
    }
    else if(set_hdmiout_res_value == 2)
    {
        vform.htx.tx_video_format = SYSTEM_VFORM_3840x2160P_5994or60_16v9;
        mCurrent_hdmitx_video_format = 2;
    }


    cmd_set_video_hdmitx(&cmd,    &vform);

    cmd_set_audio_hdmitx(&cmd,    &tx_aform);

    // MES("mAVMute = %d", mAVMute);
    if(mAVMute == 1)
    {
        // MES("this is mute video");
        cmd_mute_video_hdmitx_avmute(&cmd, txp);
        cmd_mute_audio_hdmitx(&cmd, txp);
    }
    else
    {
        // MES("this is unmute video");
        cmd_unmute_video_hdmitx_avmute(&cmd, txp);
        cmd_unmute_audio_hdmitx(&cmd, txp);
    }

    if(is_hdcp_enable == 1)
    {
        // hdmitx_video_mute(ON)
        MES("MYH %s, %d, mute HDMI Tx video & audio", __func__, __LINE__);
        // cmd_mute_video_hdmitx_avmute(&cmd, txp);
        // cmd_mute_audio_hdmitx(&cmd, txp);

        // hdmitx_hdcp_authentication_reset()
        MES("reset HDCP Tx");
        HDCPTX_THREAD_CANCEL();
        MUTEX_LOCK2();
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_LOCK2();  %d", __func__, __LINE__);
        ret = hdmitx_hdcp_authentication_reset(&auto_hdmitx_hdcp_thread_auth);
        MUTEX_UNLOCK2();
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_UNLOCK2();  %d", __func__, __LINE__);
        // init hdcptx_thread_ret
        MUTEX_LOCK(THREAD_MTX);
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_LOCK();  %d", __func__, __LINE__);
        HDCPTX_THREAD_RET = ERR_OK;
        MUTEX_UNLOCK(THREAD_MTX);
                                                    MES("LF9129_Auto_VideoFormatMYH %s, MUTEX_UNLOCK();  %d", __func__, __LINE__);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
        }

        // cmd_set_hdcptx(&cmd,  txp);
        cmd_auto_hdmitx_hdcp(&cmd, 0, 0, &vform, 0, 0);
    }
    // else
    // {
    //     ret |= cmd_unmute_video_hdmitx_system(&cmd, txp);
        MES("unmute video");
    // }

    // cmd_unmute_video_hdmitx_avmute(&cmd, txp);
    // cmd_unmute_audio_hdmitx(&cmd, txp);
}

uint32_t reset_hdmi_flag_myh = 0;
int LF9129_reset_hdmi(void)
{
    xbh_vbo2hdmi_reset_key = 0;
    hdcptx_thread_run = 0;
    auto_hdmitx_hdcp_thread_run = 0;
    MES("cancle st");
    // SLEEP(1000);
    MES("cancle OK");

    // reset_hdmi_flag_myh = 1;
    return 1;
}

int LF9129_open_hdmi(void)
{
    // usleep(10*1000);
    hdcptx_thread_run = 1;
    auto_hdmitx_hdcp_thread_run = 1;
    xbh_vbo2hdmi_reset_key = 1;

    return 1;
}

int LF9129_CheckState_Loop(void)
{
    uint32_t ret = 0;

    // static uint8_t pre_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
    // static uint8_t cus_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;

    // cus_stat = HPD_THREAD_STAT;
    // if((pre_stat != cus_stat) && (cus_stat == (HDMITX_EVENT_HPD_ACTIVE|HDMITX_EVENT_SENSE_ACTIVE)))
    // {
    //         LF9129_Auto_VideoFormat();
    // }


    // pre_stat = cus_stat;
    // SLEEP(100);

    static uint8_t pre_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
    static uint8_t cus_stat = HDMITX_EVENT_HPD_INACTIVE | HDMITX_EVENT_SENSE_INACTIVE;
    static uint8_t intr;
    system_port port;
    //cmds_main cmd;

    port = vform.htx.tx_port;
    MUTEX_LOCK2();
    ret = hdmitx_get_hpd_intr(port, &intr);
    MUTEX_UNLOCK2();
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_get_hpd_intr", hv_strerr(ret));
    }
    hdmitx_set_update_mode(HDMITX_UPDATE_MODE_DISABLE,0);
    if(xbh_vbo2hdmi_reset_key == 1) {
        if (intr & (HDMITX_INTR_HPD_NEGATIVE | HDMITX_INTR_SENSE_NEGATIVE)) {
            // detect HPD/RxSense Negative Edge
            intr = intr & (HDMITX_INTR_HPD_NEGATIVE | HDMITX_INTR_SENSE_NEGATIVE);
            MUTEX_LOCK2();
            ret = hdmitx_set_hpd_intr(port, intr);
            MUTEX_UNLOCK2();
            MES("if (intr & (HDMITX_INTR_HPD_NEGATIVE | HDMITX_INTR_SENSE_NEGATIVE)), intr = %d", intr);
            if (ret != ERR_OK) {
                ERR(API_ERROR_OCCURRED, "hdmitx_set_hpd_intr", hv_strerr(ret));
            }

            if (intr & HDMITX_INTR_HPD_NEGATIVE) {
                MUTEX_LOCK(THREAD_MTX);
                cus_stat = (cus_stat & ~HDMITX_EVENT_HPD_MASK)| HDMITX_EVENT_HPD_INACTIVE;
                MUTEX_UNLOCK(THREAD_MTX);
            }
            if (intr & HDMITX_INTR_SENSE_NEGATIVE) {
                MUTEX_LOCK(THREAD_MTX);
                cus_stat = (cus_stat & ~HDMITX_EVENT_SENSE_MASK)| HDMITX_EVENT_SENSE_INACTIVE;
                MUTEX_UNLOCK(THREAD_MTX);
            }
        }
        else if (intr & (HDMITX_INTR_HPD_POSITIVE | HDMITX_INTR_SENSE_POSITIVE)) {
            MES("else if (intr & (HDMITX_INTR_HPD_POSITIVE | HDMITX_INTR_SENSE_POSITIVE)), intr = %d", intr);
            // detect HPD/RxSense Positive Edge
            intr = intr & (HDMITX_INTR_HPD_POSITIVE | HDMITX_INTR_SENSE_POSITIVE);
            MUTEX_LOCK2();
            ret = hdmitx_set_hpd_intr(port, intr);
            MUTEX_UNLOCK2();
            if (ret != ERR_OK) {
                ERR(API_ERROR_OCCURRED, "hdmitx_set_hpd_intr", hv_strerr(ret));
            }

            if (intr & HDMITX_INTR_HPD_POSITIVE) {
                MUTEX_LOCK(THREAD_MTX);
                cus_stat = (cus_stat & ~HDMITX_EVENT_HPD_MASK)| HDMITX_EVENT_HPD_ACTIVE;
                MUTEX_UNLOCK(THREAD_MTX);
            }
            if (intr & HDMITX_INTR_SENSE_NEGATIVE) {
                MUTEX_LOCK(THREAD_MTX);
                cus_stat = (cus_stat & ~HDMITX_EVENT_SENSE_MASK)| HDMITX_EVENT_SENSE_ACTIVE;
                MUTEX_UNLOCK(THREAD_MTX);
            }
        }
        else
        {
            MES("else, intr = %d", intr);
            // get HPD/RxSense Status
            MUTEX_LOCK2();
            ret = hdmitx_get_hpd_status(port, &cus_stat);
            MUTEX_UNLOCK2();
            if (ret != ERR_OK) {
                ERR(API_ERROR_OCCURRED, "hdmitx_get_hpd_status", hv_strerr(ret));
            }
        }

        // if(reset_hdmi_flag_myh == 1)
        // {
        //     pre_stat = 0;
        // }

        if (pre_stat != cus_stat) {
            MES("MYH %s %d, cus_stat = 0x%x", __func__, __LINE__, cus_stat);
            switch (cus_stat & HDMITX_EVENT_HPD_MASK) {
                case HDMITX_EVENT_HPD_ACTIVE:
                    MES("\nMYH %s %d, HDMI Tx HPD active/", __func__, __LINE__);
                    break;
                case HDMITX_EVENT_HPD_INACTIVE:
                    MES("\nMYH %s %d, HDMI Tx HPD inactive/", __func__, __LINE__);
                    break;
            }
            switch (cus_stat & HDMITX_EVENT_SENSE_MASK) {
                case HDMITX_EVENT_SENSE_ACTIVE:
                    MES("MYH %s %d, Sense active", __func__, __LINE__);
                    break;
                case HDMITX_EVENT_SENSE_INACTIVE:
                    MES("MYH %s %d, Sense inactive", __func__, __LINE__);
                    break;
            }
            MUTEX_LOCK(THREAD_MTX);
            auto_hdmitx_hdcp_thread_state = cus_stat;
            MUTEX_UNLOCK(THREAD_MTX);
            if(cus_stat == (HDMITX_EVENT_HPD_ACTIVE|HDMITX_EVENT_SENSE_ACTIVE)){
                MES("LF9129 hpd_thread HPD IS OK!!");
                AUTO_HDMITX_HDCP_THREAD_CANCEL();

                LF9129_Auto_VideoFormat();
            }
        }
        pre_stat = cus_stat;
        // reset_hdmi_flag_myh = 0;
        // xbh_vbo2hdmi_reset_key = 0;
    } else {
        pre_stat = 0;
        // SLEEP(20);
    }

    SLEEP(100);

    return 0;
}

//------------------------------------------------------------------------------
void i2c_info(void)
{
    uint32_t chnum, ids[MAX_I2C_CHANNEL], cur_idx;
    uint32_t i;

    /*if (!reg_info(&chnum, ids, &cur_idx)) {
        MES("  I2C: num=%"PRIu32", ", chnum);
        for (i=0; i<chnum; i++) {
            MES("id[%"PRIu32"]=%"PRIu32"%s ", i, ids[i], (cur_idx == i) ? "*" : "");
        }
        MES("\n");
    }*/
}
//------------------------------------------------------------------------------
static uint32_t cmd_reg(cmds_main *cmd)
{
    uint32_t idx;
    uint32_t ret = 0;

    idx = (uint32_t)(cmd->opts.data);
    // select I2C channel
    //ret = reg_select(idx);
    if (ret) {
        ERR("%s (%s)", COMMAND_ERROR_OCCURRED, hv_strerr(ret));
        return ERR_NG;
    }
    // show I2C info
    i2c_info();

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_VIDEO:        ret = cmd_preset_video(cmd, p_rxp, p_txp, p_vform);        break;
        case OPT_TARGET_AUDIO:        ret = cmd_preset_audio(cmd, p_rxp, p_txp,
                                                                p_rx_aform, p_tx_aform);    break;
        case OPT_TARGET_EDID:        ret = cmd_preset_edid(cmd);                                break;
        case OPT_TARGET_CHIP:        ret = cmd_preset_chip(cmd, p_rxp, p_txp, p_vform,
                                                                p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_preset_video_hdmi(cmd, p_rxp, p_txp, p_vform);    break;
        case OPT_PORT_VBO:        ret = cmd_preset_video_vbo(cmd, p_rxp, p_txp, p_vform);        break;
        case OPT_PORT_LVDS:        ret = cmd_preset_video_lvds(cmd, p_rxp, p_txp, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_audio(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                    system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_preset_audio_hdmi(cmd, p_rxp, p_txp,
                                                            p_rx_aform, p_tx_aform);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_edid(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    char  buf[MAX_LINE], filename[MAX_LINE];
    char *tok, *str, *delim = " \r\n\0";
    int idx;
    uint32_t size;
    uint32_t ret;

    MES("preset EDID table");

    strcpy(buf, cmd->str);
    del_line_comment(buf);
    for (str = buf, idx = 0; idx < MAX_TOK_NUM; str = NULL, idx++) {
        tok = strtok(str, delim);
        if (tok == NULL) break;
        switch(idx) {
            case 0:                                        break;    // preset
            case 1:                                        break;    // edid
            case 2: strncpy(filename, tok, MAX_LINE);    break;    // <filename>
            default:                                    break;
        }
    }

    //ret = read_edid_txt(filename, edid_table, &size);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    // dump setting data
    dump_bin(edid_table, size);

    return ERR_OK;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_chip(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    MES("preset Chip No: %d", cmd->opts.chip_num);

    if (cmd->opts.chip_num > SYSTEM_CHIP_NUMBER_3) {
        ERR("out of bounds setting");
    }

    p_rxp->chip_num = cmd->opts.chip_num;
    p_txp->chip_num = cmd->opts.chip_num;
    p_vform->sys.rx_port.chip_num = cmd->opts.chip_num;
    p_vform->sys.tx_port.chip_num = cmd->opts.chip_num;
    p_rx_aform->port.chip_num = cmd->opts.chip_num;
    p_tx_aform->port.chip_num = cmd->opts.chip_num;

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_preset_video_hdmitx(cmd, p_txp, p_vform);            break;
        case OPT_TXRX_RX:        ret = cmd_preset_video_hdmirx(cmd, p_rxp, p_vform);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_vbo(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_preset_video_vbotx(cmd, p_txp, p_vform);            break;
        case OPT_TXRX_RX:        ret = cmd_preset_video_vborx(cmd, p_rxp, p_vform);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_lvds(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_preset_video_lvdstx(cmd, p_txp, p_vform);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_preset_audio_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                        system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_preset_audio_hdmitx(cmd, p_txp, p_tx_aform);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Preset HDMI Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_hdmitx(cmds_main *cmd, system_port *p_txp, system_videos *p_vform)
{
    system_video_format vform = SYSTEM_VFORM_MAX;
    system_color_format cform = SYSTEM_COLOR_MAX;
    system_deep_color dcolor = SYSTEM_DEEP_COLOR_MAX;
    uint32_t ret;

    MES("preset HDMI Tx video");

    p_txp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_vform->sys.tx_port = *p_txp;

    // video format
    ret = get_video_format(cmd->opts.resolution, cmd->opts.fps, &vform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->htx.tx_video_format = vform;
    // color format
    ret = get_color_format(cmd->opts.color_format, &cform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->htx.tx_color_format = cform;
    // deep color
    ret = get_deep_color(cmd->opts.deep_color, &dcolor);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->htx.tx_deep_color = dcolor;
    // DVI mode
    p_vform->htx.tx_dvi_mode = (cmd->opts.dvi_mode == OPT_DVI_MODE_ENABLE)
                        ? HDMITX_DVI_MODE_ENABLE
                        : HDMITX_DVI_MODE_DISABLE;

    print_video_format(vform, cform, dcolor);
    if (p_vform->htx.tx_dvi_mode == HDMITX_DVI_MODE_ENABLE) {
        MES("  DVI mode");
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
// Preset HDMI Rx Video
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_hdmirx(cmds_main *cmd, system_port *p_rxp, system_videos *p_vform)
{
    system_video_format vform = SYSTEM_VFORM_MAX;
    system_color_format cform = SYSTEM_COLOR_MAX;
    system_deep_color dcolor = SYSTEM_DEEP_COLOR_MAX;
    uint32_t ret;

    MES("preset HDMI Rx video");

    p_rxp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_vform->sys.rx_port = *p_rxp;

    // video format
    ret = get_video_format(cmd->opts.resolution, cmd->opts.fps, &vform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.rx_video_format = vform;
    // color format
    ret = get_color_format(cmd->opts.color_format, &cform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.rx_color_format = cform;
    // deep color
    ret = get_deep_color(cmd->opts.deep_color, &dcolor);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.rx_deep_color = dcolor;

    print_video_format(vform, cform, dcolor);

    return ERR_OK;
}
//------------------------------------------------------------------------------
// Preset VBO Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_vbotx(cmds_main *cmd, system_port *p_txp, system_videos *p_vform)
{
    system_video_format vform = SYSTEM_VFORM_MAX;
    system_color_format cform = SYSTEM_COLOR_MAX;
    system_deep_color dcolor = SYSTEM_DEEP_COLOR_MAX;
    uint32_t ret;

    MES("preset VBO Tx video");

    p_txp->port_type = SYSTEM_PORT_TYPE_VBO;
    p_vform->sys.tx_port = *p_txp;

    // video format
    ret = get_video_format(cmd->opts.resolution, cmd->opts.fps, &vform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.tx_video_format = vform;
    // color format
    ret = get_color_format(cmd->opts.color_format, &cform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.tx_color_format = cform;
    // deep color
    ret = get_deep_color(cmd->opts.deep_color, &dcolor);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.tx_deep_color = dcolor;

    print_video_format(vform, cform, dcolor);

    return ERR_OK;
}
//------------------------------------------------------------------------------
// Preset VBO Rx Video
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_vborx(cmds_main *cmd, system_port *p_rxp, system_videos *p_vform)
{
    system_video_format vform = SYSTEM_VFORM_MAX;
    system_color_format cform = SYSTEM_COLOR_MAX;
    system_deep_color dcolor = SYSTEM_DEEP_COLOR_MAX;
    uint32_t ret;

    MES("preset VBO Rx video");

    p_rxp->port_type = SYSTEM_PORT_TYPE_VBO;
    p_vform->sys.rx_port = *p_rxp;

    // video format
    ret = get_video_format(cmd->opts.resolution, cmd->opts.fps, &vform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.rx_video_format = vform;
    // color format
    ret = get_color_format(cmd->opts.color_format, &cform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.rx_color_format = cform;
    // deep color
    ret = get_deep_color(cmd->opts.deep_color, &dcolor);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.rx_deep_color = dcolor;

    print_video_format(vform, cform, dcolor);

    return ERR_OK;
}
//------------------------------------------------------------------------------
// Preset LVDS Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_preset_video_lvdstx(cmds_main *cmd, system_port *p_txp, system_videos *p_vform)
{
    system_video_format vform = SYSTEM_VFORM_MAX;
    system_color_format cform = SYSTEM_COLOR_MAX;
    system_deep_color dcolor = SYSTEM_DEEP_COLOR_MAX;
    uint32_t ret;

    MES("preset LVDS Tx video");

    p_txp->port_type = SYSTEM_PORT_TYPE_LVDS;
    p_vform->sys.tx_port = *p_txp;

    // video format
    ret = get_video_format(cmd->opts.resolution, cmd->opts.fps, &vform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.tx_video_format = vform;
    // color format
    ret = get_color_format(cmd->opts.color_format, &cform);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.tx_color_format = cform;
    // deep color
    ret = get_deep_color(cmd->opts.deep_color, &dcolor);
    if (ret) ERR("%s", COMMAND_ERROR_OCCURRED);
    p_vform->sys.tx_deep_color = dcolor;

    print_video_format(vform, cform, dcolor);

    return ERR_OK;
}
//------------------------------------------------------------------------------
// Preset HDMI Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_preset_audio_hdmitx(cmds_main *cmd, system_port *p_txp, system_audio *p_aform)
{
    system_audio_cc if_cc            = SYSTEM_AUDIO_CC_2CH;
    system_audio_sf if_sf            = SYSTEM_AUDIO_SF_RSH;
    system_audio_ss if_ss            = SYSTEM_AUDIO_SS_RSH;
    uint8_t if_ca                    = 0x00;
    uint8_t if_lsv                    = 0x00;
    system_audio_sf cb_sf            = SYSTEM_AUDIO_SF_32KHZ;
    system_audio_ss cb_ss            = SYSTEM_AUDIO_SS_16BIT;
    system_audio_sf_ratio sf_ratio    = SYSTEM_AUDIO_SF_RATIO_1_1;
    system_audio_em i2s_em            = SYSTEM_AUDIO_EM_MSB;
    system_audio_po i2s_po            = SYSTEM_AUDIO_PO_LOW;
    system_audio_if i2s_if            = SYSTEM_AUDIO_IF_DISABLE;
    system_audio_sp i2s_sp            = SYSTEM_AUDIO_SP_RIGHT;

    p_txp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_aform->port = *p_txp;

    MES("preset HDMI Tx audio");

    if (get_audio_cc(cmd->opts.if_cc, &if_cc)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_sf(cmd->opts.if_sf, &if_sf)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_ss(cmd->opts.if_ss, &if_ss)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if_ca = cmd->opts.if_ca;
    if_lsv = cmd->opts.if_lsv;
    if (get_audio_sf(cmd->opts.cb_sf, &cb_sf)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_ss(cmd->opts.cb_ss, &cb_ss)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_sf_ratio(cmd->opts.sf_ratio, &sf_ratio)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_em(cmd->opts.i2s_em, &i2s_em)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_po(cmd->opts.i2s_po, &i2s_po)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_if(cmd->opts.i2s_if, &i2s_if)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }
    if (get_audio_sp(cmd->opts.i2s_sp, &i2s_sp)) {
        ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    p_aform->if_channel_count        = if_cc;
    p_aform->if_sampling_freq        = if_sf;
    p_aform->if_sample_size            = if_ss;
    p_aform->if_channel_number        = if_ca;
    p_aform->if_level_shift_value    = if_lsv;
    p_aform->cb_sampling_freq        = cb_sf;
    p_aform->cb_sample_size            = cb_ss;
    p_aform->sampling_freq_ratio    = sf_ratio;
    p_aform->i2s.endian_mode        = i2s_em;
    p_aform->i2s.polarity            = i2s_po;
    p_aform->i2s.i2s_format            = i2s_if;
    p_aform->i2s.start_pos            = i2s_sp;

    print_audio_format(p_aform);

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t cmd_config(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_config_hdmi(cmd);                break;
        case OPT_PORT_VBO:        ret = cmd_config_vbo(cmd);                break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }
    
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_config_hdmi(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_config_hdmitx(cmd);            break;
        case OPT_TXRX_RX:        ret = cmd_config_hdmirx(cmd);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_config_vbo(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_config_vbotx(cmd);            break;
        case OPT_TXRX_RX:        ret = cmd_config_vborx(cmd);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_config_hdmitx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_hdcp_config hdcp_conf;
    uint32_t ret = ERR_NG;

    memset(&hdcp_conf, 0, sizeof(hdmitx_hdcp_config));

    if (cmd->opts.target2 == OPT_TARGET2_INIT_AUTH) {
        MES("  config HDMI Tx HDCP Initial Authentication Retry");
        MES("  Times:    %lu", cmd->opts.times);
        MES("  Interval: %lu (ms)", cmd->opts.msec);
        hdcp_conf.init_auth.update = SYSTEM_UPDATE_ENABLE;
        hdcp_conf.init_auth.times = cmd->opts.times;
        hdcp_conf.init_auth.interval = cmd->opts.msec;
        ret = hdmitx_hdcp_configure(&hdcp_conf);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_configure", hv_strerr(ret));
            return ret;
        }
    } else {
        ERR("%s", UNSUPPORTED_COMMAND);
        return ERR_NG;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_config_hdmirx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_config_vbotx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    vbotx_config vbotx_conf;
    uint32_t ret = ERR_NG;

    memset(&vbotx_conf, 0, sizeof(vbotx_config));

    if (cmd->opts.target2 == OPT_TARGET2_HOT_PLUG) {
        MES("config VBO Tx Hot Plug Detection Retry");
        MES("  Times:    %lu", cmd->opts.times);
        MES("  Interval: %lu (ms)", cmd->opts.msec);
        vbotx_conf.hot_plug_detect.update = SYSTEM_UPDATE_ENABLE;
        vbotx_conf.hot_plug_detect.times = cmd->opts.times;
        vbotx_conf.hot_plug_detect.interval = cmd->opts.msec;
        ret = vbotx_configure(&vbotx_conf);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "vbotx_configure", hv_strerr(ret));
            return ret;
        }
    } else if (cmd->opts.target2 == OPT_TARGET2_PLL_LOCK) {
        MES("config VBO Tx PLL Lock Detection Retry");
        MES("  Times:    %lu", cmd->opts.times);
        MES("  Interval: %lu (ms)", cmd->opts.msec);
        vbotx_conf.pll_lock_detect.update = SYSTEM_UPDATE_ENABLE;
        vbotx_conf.pll_lock_detect.times = cmd->opts.times;
        vbotx_conf.pll_lock_detect.interval = cmd->opts.msec;
        ret = vbotx_configure(&vbotx_conf);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "vbotx_configure", hv_strerr(ret));
            return ret;
        }
    } else {
        ERR("%s", UNSUPPORTED_COMMAND);
        return ERR_NG;
    }
    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_config_vborx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    vborx_config vborx_conf;
    uint32_t ret = ERR_NG;

    memset(&vborx_conf, 0, sizeof(vborx_config));

   if (cmd->opts.target2 == OPT_TARGET2_CDR_LOCK) {
        MES("config VBO Rx CDR Lock Detection Retry");
        MES("  Times:    %lu", cmd->opts.times);
        MES("  Interval: %lu (ms)", cmd->opts.msec);
        vborx_conf.cdr_lock_detect.update = SYSTEM_UPDATE_ENABLE;
        vborx_conf.cdr_lock_detect.times = cmd->opts.times;
        vborx_conf.cdr_lock_detect.interval = cmd->opts.msec;
        ret = vborx_configure(&vborx_conf);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "vborx_configure", hv_strerr(ret));
            return ret;
        }
    } else {
        ERR("%s", UNSUPPORTED_COMMAND);
        return ERR_NG;
    }
    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_init(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_init_hdmi(cmd, rxp, txp);        break;
        case OPT_PORT_VBO:        ret = cmd_init_vbo(cmd, rxp, txp);        break;
        case OPT_PORT_LVDS:        ret = cmd_init_lvds(cmd, rxp, txp);        break;
        case OPT_PORT_SPDIF:    ret = cmd_init_spdif(cmd, rxp, txp);    break;
        case OPT_PORT_I2S:        ret = cmd_init_i2s(cmd, rxp, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }
    
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_init_hdmi(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_init_hdmitx(cmd, txp);        break;
        case OPT_TXRX_RX:        ret = cmd_init_hdmirx(cmd, rxp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_init_vbo(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_init_vbotx(cmd, txp);            break;
        case OPT_TXRX_RX:        ret = cmd_init_vborx(cmd, rxp);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_init_lvds(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_init_lvdstx(cmd, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_init_spdif(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_init_spdiftx(cmd, txp);        break;
        case OPT_TXRX_RX:        ret = cmd_init_spdifrx(cmd, rxp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_init_i2s(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_init_i2stx(cmd, txp);            break;
        case OPT_TXRX_RX:        ret = cmd_init_i2srx(cmd, rxp);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Init HDMI Tx
//------------------------------------------------------------------------------
static uint32_t cmd_init_hdmitx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("LF9129 init HDMI Tx");
    MES("LF9129 chip_num = %0x, port_num = %0x, port_type = %0x,  rxtx_type = %0x \n", port.chip_num, port.port_num, port.port_type, port.rxtx_type);
    // AUTO_HDMITX_THREAD_CANCEL();
    // HPD_THREAD_CANCEL();
    // HDCPTX_THREAD_CANCEL();

    // MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_init(port);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_init", hv_strerr(ret));
        return ret;
    }
    
    // call hdmitx_get_hpd_status() periodically
    THREAD_CREATE(HPD_THREAD_ID, NULL, HPD_THREAD, (void *)&port);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Init HDMI Rx
//------------------------------------------------------------------------------
static uint32_t cmd_init_hdmirx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("init HDMI Rx");

    AUTO_HDMIRX_THREAD_CANCEL();
    PORT_THREAD_CANCEL();
    AV_THREAD_CANCEL();
    HDCPRX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_init(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_init", hv_strerr(ret));
        return ret;
    }

    // call hdmirx_get_port_status() periodically
    THREAD_CREATE(PORT_THREAD_ID, NULL, PORT_THREAD, (void *)&port);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Init VBO Tx
//------------------------------------------------------------------------------
static uint32_t cmd_init_vbotx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    system_config conf;
    uint32_t ret;

    conf.port = port;
    get_init_vbo(cmd->opts.init_opt, &(conf.hvsync_polarity));    // default: inversion OFF

    MES("init VBO Tx (%s)", (conf.hvsync_polarity == SYSTEM_INVERSION_ON)? "inversion ON": "inversion OFF");
    // MUTEX_LOCK(THREAD_MTX);
    MES("Func: %s, Line: %d vbotx_init ST", __func__, __LINE__);
    ret = vbotx_init(&conf);
    MES("Func: %s, Line: %d vbotx_init EN", __func__, __LINE__);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Init VBO Rx
//------------------------------------------------------------------------------
static uint32_t cmd_init_vborx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    system_config conf;
    uint32_t ret;
    conf.port = port;
    get_init_vbo(cmd->opts.init_opt, &(conf.hvsync_polarity));    // default: inversion OFF

    MES("init VBO Rx (%s)", (conf.hvsync_polarity == SYSTEM_INVERSION_ON)? "inversion ON": "inversion OFF");
    MES("LF9129 chip_num = %0x, port_num = %0x, port_type = %0x,  rxtx_type = %0x, hvsync_polarity = %0x \n", port.chip_num, port.port_num, port.port_type,  port.rxtx_type, conf.hvsync_polarity);

    // MUTEX_LOCK(THREAD_MTX);
    ret = vborx_init(&conf);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vborx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif

}
//------------------------------------------------------------------------------
// Init LVDS Tx
//------------------------------------------------------------------------------
static uint32_t cmd_init_lvdstx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    lvdstx_config conf;
    uint32_t ret;

    conf.port = port;
    get_init_lvds(cmd->opts.init_opt, &(conf.format));    // default: VESA 10bit

    MES("init LVDS Tx (%s)", (conf.format == LVDSTX_DATA_FORMAT_VESA_8BIT)? "VESA 8bit": (conf.format == LVDSTX_DATA_FORMAT_VESA_10BIT)? "VESA 10bit": (conf.format == LVDSTX_DATA_FORMAT_JEIDA_8BIT)? "JEIDA 8bit": (conf.format == LVDSTX_DATA_FORMAT_JEIDA_10BIT)? "JEIDA 10bit": (conf.format == LVDSTX_DATA_FORMAT_VESA_JEIDA_COMPOSITE)? "VESA/JEIDA composite": "ERROR");

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_init(&conf);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Init SPDIF Tx
//------------------------------------------------------------------------------
static uint32_t cmd_init_spdiftx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("init SPDIF Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = spdiftx_init(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdiftx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Init SPDIF Rx
//------------------------------------------------------------------------------
static uint32_t cmd_init_spdifrx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("LF9129 init SPDIF Rx");
    MES("LF9129 chip_num = %0x, port_num = %0x, port_type = %0x,  rxtx_type = %0x", port.chip_num, port.port_num, port.port_type, port.rxtx_type);
    // MUTEX_LOCK(THREAD_MTX);
    ret = spdifrx_init(port);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdifrx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Init I2S Tx
//------------------------------------------------------------------------------
static uint32_t cmd_init_i2stx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("init I2S Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = i2stx_init(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "i2stx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Init I2S Rx
//------------------------------------------------------------------------------
static uint32_t cmd_init_i2srx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("init I2S Rx");

    MUTEX_LOCK(THREAD_MTX);
    ret = i2srx_init(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "i2srx_init", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_read(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_EDID:    ret = cmd_read_edid(cmd, txp);            break;
        case OPT_TARGET_SCDC:    ret = cmd_read_scdc(cmd, txp);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Read HDMI Tx EDID
//------------------------------------------------------------------------------
static uint32_t cmd_read_edid(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_edid tx_edid;
    uint32_t ext_flag, i, j;
    uint32_t ret;
    ext_block_number = 0;

    MES("read HDMI Tx EDID");

    tx_edid.port = port;

    // read Base EDID block at first
    tx_edid.saddr = 0x00;
    tx_edid.oaddr = 0x00;
    tx_edid.length = 128;
    memset(tx_edid.data, 0x00, HDMITX_DDC_MAX_BUF_SIZE);

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_read_edid(&tx_edid);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_read_edid", hv_strerr(ret));
        return ret;
    }

    // data copy read EDID to edid_table (0-127)
    for (i=0; i<128; i++) {
        edid_table[i] = tx_edid.data[i];
    }

    // get extension flag of Base EDID
    MES("this is sink block0\n");
    for(i = 0; i < 8; ++i) {
        MES("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x", tx_edid.data[i*16 + 0], tx_edid.data[i*16 + 1], tx_edid.data[i*16 +2], tx_edid.data[i*16 + 3], tx_edid.data[i*16 + 4], tx_edid.data[i*16 + 5], tx_edid.data[i*16 + 6], tx_edid.data[i*16 + 7], tx_edid.data[i*16 + 8], tx_edid.data[i*16 + 9], tx_edid.data[i*16 + 10], tx_edid.data[i*16 + 11], tx_edid.data[i*16 + 12], tx_edid.data[i*16 + 13], tx_edid.data[i*16 + 14], tx_edid.data[i*16 + 15]);
    }
    ext_flag = tx_edid.data[0x7E];
    MES("ext_flag = %d\n", ext_flag);

    if (ext_flag == 0) { // 证明只有一个block，是DVI或者VGA信号
        return ret;
    }

    // read extension EDID block
    tx_edid.saddr = 1 / 2;
    tx_edid.oaddr = (1 % 2) * 0x80;
    tx_edid.length = 128;
    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_read_edid(&tx_edid);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_read_edid", hv_strerr(ret));
        return ret;
    }
    for(j = 0;j < 128; j++) {
        edid_extend_table[0][j] = tx_edid.data[j];
    }

    if (ext_flag > 1) // 证明除了block0和block1 还有额外的拓展块
    {
        MES("ext_block_number = %d\n", ext_block_number);
        ext_block_number = ext_flag - 1; // 减去已经读取的block1
    }
    if ( (edid_extend_table[0][4] == 0xE2) && (edid_extend_table[0][5] == 0x78) ) {// CTA Data Block: Extended Tag 120, bytes 2: HF-EEODB
        MES("edid_extend_table[0][6] - 1 = %d\n", edid_extend_table[0][6] - 1);
        ext_block_number += edid_extend_table[0][6] - 1; // 除Block0主块, Block1, 还有ext_block_number个扩展块
    }

    if (ext_block_number == 0)
    {
        return ret; // 只有block0和block1，无其他拓展块，故退出。
    }

    for (int ext_num = 1; ext_num <= ext_block_number; ++ext_num) {
        tx_edid.saddr = (ext_num + 1) / 2;
        tx_edid.oaddr = ( (ext_num + 1) % 2) * 0x80;
        tx_edid.length = 128;

        MUTEX_LOCK(THREAD_MTX);
        ret = hdmitx_read_edid(&tx_edid);
        MUTEX_UNLOCK(THREAD_MTX);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_read_edid", hv_strerr(ret));
            return ret;
        }
        
        for(j = 0;j < 128; j++) {
            edid_extend_table[ext_num][j] = tx_edid.data[j];
        }
        ++i;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}

//------------------------------------------------------------------------------
// Read HDMI Tx SCDC
//------------------------------------------------------------------------------
static uint32_t cmd_read_scdc(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_scdc scdc;
    uint32_t ret;

    MES("read HDMI Tx SCDC");

    scdc.port = port;
    scdc.oaddr = cmd->opts.oaddr;
    scdc.length = 1;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_read_scdc(&scdc);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_read_scdc", hv_strerr(ret));
        return ret;
    }
    print_scdc(&scdc);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_write(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_EDID:    ret = cmd_write_edid(cmd, rxp, txp);    break;
        case OPT_TARGET_SCDC:    ret = cmd_write_scdc(cmd, txp);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_write_edid(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.target2) {
        case OPT_TARGET2_SRAM:    ret = cmd_write_edid_sram(cmd, rxp);        break;
        case OPT_TARGET2_VIA:
            switch (cmd->opts.port) {
                case OPT_PORT_HDMI:
                    switch (cmd->opts.txrx) {
                        case OPT_TXRX_TX:
                                ret = cmd_write_edid_via_hdmi_tx(cmd, txp);            break;
                        default:        ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
                    }
                                                                                break;
                default:            ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
            }
                                                                            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Write HDMI Rx EDID for SRAM
//------------------------------------------------------------------------------
static uint32_t cmd_write_edid_sram(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    hdmirx_edid edid;
    uint32_t ret;

    MES("write HDMI Rx EDID for SRAM");

    edid.port = port;
    edid.p_edid0 = NULL;        // external memory (EEPROM)
    edid.p_edid1 = edid_table;    // internal memory (SRAM)
    edid.p_edid2 = NULL;        // external memory (EEPROM)
    edid.p_edid3 = edid_table;    // internal memory (SRAM)

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_write_edid(&edid);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_write_edid", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Write EDID via HDMI Tx
//------------------------------------------------------------------------------
#define PAGE_SIZE_OF_EEPROM_24LC024I                        16
static uint32_t cmd_write_edid_via_hdmi_tx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_ddc ddc;
    uint32_t i;
    uint32_t ret;

    MES("write EDID via HDMI Tx");

    ddc.port = port;
    ddc.saddr = 0xA0;
    ddc.oaddr = 0x00;
    ddc.length = PAGE_SIZE_OF_EEPROM_24LC024I;
    memcpy(ddc.data, &edid_table[ddc.oaddr], PAGE_SIZE_OF_EEPROM_24LC024I);

    // EDID data is written by dividing each page write buffer of EEPROM
    for (i=0; i<(DDC_MAX_BUF_SIZE / PAGE_SIZE_OF_EEPROM_24LC024I); i++) {
        ddc.oaddr = i * PAGE_SIZE_OF_EEPROM_24LC024I;
        memcpy(ddc.data, &edid_table[ddc.oaddr], PAGE_SIZE_OF_EEPROM_24LC024I);
        MUTEX_LOCK(THREAD_MTX);
        ret = hdmitx_write_ddc(&ddc);
        MUTEX_UNLOCK(THREAD_MTX);
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_write_ddc", hv_strerr(ret));
            ERR("  %02X:%02X.", ddc.saddr, ddc.oaddr);
            return ret;
        }
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Write HDMI Tx SCDC
//------------------------------------------------------------------------------
static uint32_t cmd_write_scdc(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_scdc scdc;
    uint32_t ret;

    MES("write HDMI Tx SCDC");

    scdc.port = port;
    scdc.oaddr = cmd->opts.oaddr;
    scdc.length = 1;
    scdc.data[0] = cmd->opts.data;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_write_scdc(&scdc);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_write_scdc", hv_strerr(ret));
        return ret;
    }
    print_scdc(&scdc);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_set(cmds_main *cmd, system_videos *p_vform, system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_VIDEO:    ret = cmd_set_video(cmd, p_vform);                                        break;
        case OPT_TARGET_AUDIO:    ret = cmd_set_audio(cmd, p_rx_aform, p_tx_aform);                        break;
        case OPT_TARGET_HDCP:    ret = cmd_set_hdcp(cmd, p_vform->sys.rx_port, p_vform->sys.tx_port);    break;
        case OPT_TARGET_SPD_IF:    ret = cmd_set_spd_infoframe(cmd, p_vform->sys.tx_port);                    break;
        case OPT_TARGET_HDR_IF:    ret = cmd_set_hdr_infoframe(cmd, p_vform->sys.tx_port);                    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                                        return ERR_NG;
    }
    
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_video(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_set_video_hdmi(cmd, p_vform);        break;
        case OPT_PORT_VBO:        ret = cmd_set_video_vbo(cmd, p_vform);        break;
        case OPT_PORT_LVDS:        ret = cmd_set_video_lvds(cmd, p_vform);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_hdmi(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_set_video_hdmitx(cmd, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Set HDMI Tx Video Format
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_hdmitx(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("LF9129 set HDMI Tx video");
    MES("LF9129 chip_num = %0x, port_num = %0x, port_type = %0x,  rxtx_type = %0x \n", p_vform->htx.tx_port.chip_num, p_vform->htx.tx_port.port_num, p_vform->htx.tx_port.port_type,   p_vform->htx.tx_port.rxtx_type);
    MES("LF9129 rx_video_format = %0x, rx_color_format = %0x, rx_deep_color = %0x \n", p_vform->htx.rx_video_format, p_vform->htx.rx_color_format, p_vform->htx.rx_deep_color);
    print_video_format(p_vform->htx.tx_video_format, p_vform->htx.tx_color_format, p_vform->htx.tx_deep_color);
    if (p_vform->htx.tx_dvi_mode == HDMITX_DVI_MODE_ENABLE) {
        MES("LF9129 DVI mode");
    }

    HDCPTX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    MES("MN869129 %s %d, hdmitx_set_video_format st", __func__, __LINE__);
    ret = hdmitx_set_video_format(&(p_vform->htx));
    MES("MN869129 %s %d, hdmitx_set_video_format end", __func__, __LINE__);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_set_video_format", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_vbo(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_set_video_vbotx(cmd, p_vform);    break;
        case OPT_TXRX_RX:        ret = cmd_set_video_vborx(cmd, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Set VBO Tx Video Format
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_vbotx(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    uint32_t ret;

    MES("set VBO Tx video");
    print_video_format(p_vform->sys.tx_video_format, p_vform->sys.tx_color_format, p_vform->sys.tx_deep_color);

    // MUTEX_LOCK(THREAD_MTX);
    ret = vbotx_set_video_format((vbotx_video *)p_vform);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_set_video_format", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Set VBO Rx Video Format
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_vborx(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    uint32_t ret;

    MES("LF9129 set VBO Rx video");
    MES("LF9129 chip_num = %0x, port_num = %0x, port_type = %0x,  rxtx_type = %0x \n", p_vform->sys.tx_port.chip_num, p_vform->sys.tx_port.port_num, p_vform->sys.tx_port.port_type,  p_vform->sys.tx_port.rxtx_type);
    print_video_format(p_vform->sys.rx_video_format, p_vform->sys.rx_color_format, p_vform->sys.rx_deep_color);
    MES("LF9129 tx_video_format = %0x, tx_color_format = %0x, tx_deep_color = %0x \n", p_vform->sys.tx_video_format, p_vform->sys.tx_color_format, p_vform->sys.tx_deep_color);
    // MUTEX_LOCK(THREAD_MTX);
    ret = vborx_set_video_format((vborx_video *)p_vform);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        MES("vborx_set_video_format is not OK, quit\n");
        ERR(API_ERROR_OCCURRED, "vborx_set_video_format", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_lvds(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_set_video_lvdstx(cmd, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Set LVDS Tx Video Format
//------------------------------------------------------------------------------
static uint32_t cmd_set_video_lvdstx(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    uint32_t ret;

    MES("set LVDS Tx video");
    print_video_format(p_vform->sys.tx_video_format, p_vform->sys.tx_color_format, p_vform->sys.tx_deep_color);

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_set_video_format((lvdstx_video *)p_vform);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_set_video_format", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_audio(cmds_main *cmd, system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_set_audio_hdmi(cmd, p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_audio_hdmi(cmds_main *cmd, system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_set_audio_hdmitx(cmd, p_tx_aform);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Set HDMI Tx Audio Format
//------------------------------------------------------------------------------
static uint32_t cmd_set_audio_hdmitx(cmds_main *cmd, system_audio *p_aform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("set HDMI Tx audio");
    print_audio_format(p_aform);

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_set_audio_format((hdmitx_audio *)p_aform);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_set_audio_format", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_set_hdcp(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_set_hdcptx(cmd, txp);        break;
        case OPT_TXRX_RX:        ret = cmd_set_hdcprx(cmd, rxp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Set HDIM Tx HDCP Authentication
//------------------------------------------------------------------------------
static uint32_t cmd_set_hdcptx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_hdcp_repeater_auth rep_auth;
    uint32_t ret;
    uint32_t i;

    MES("LF9129 set HDCP Tx");

    hdcptx_auth.port = port;

    HDCPTX_THREAD_CANCEL();

    // call hdmitx_hdcp_authentication()
    MUTEX_LOCK2();
    ret = hdmitx_hdcp_authentication(&hdcptx_auth);
    MUTEX_UNLOCK2();
    if (ret != ERR_OK) {
        MES("LF9129 is error");
        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication", hv_strerr(ret));
#ifdef HV_ERRNO
        if (HV_ERRNO != ERR_OK) {
            ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
        }
#endif // HV_ERRNO
        return ret;
    }
    MES("LF9129 HDCP %s", ((hdcptx_auth.hdcp_version == HDMITX_HDCP_VERSION_1_4) ? "1.4": (hdcptx_auth.hdcp_version == HDMITX_HDCP_VERSION_2_2) ? "2.2": "non-compliant"));
    MES("LF9129   Sink device: %s", ((hdcptx_auth.is_repeater == HDMITX_HDCP_REPEATER) ? "Repeater" : "NOT Repeater"));
    MES("LF9129   bksv: %02X %02X %02X %02X %02X", hdcptx_auth.bksv[0],hdcptx_auth.bksv[1],hdcptx_auth.bksv[2],hdcptx_auth.bksv[3],hdcptx_auth.bksv[4]);

    // TODO: You must check for bksv in the current revocation list.
    //       If bksv is in the revocation list,
    //       reset the authentication using hdmitx_hdcp_authentication_reset(),
    //       and call hdmitx_exit() to stop the video signal transmission.
    /* example:
    {
        for (i=0; i<REVOCATION_LIST_NUM; i++) {
            if (hdcptx_auth.bksv[0] == REVOKED_KSV[i][0]
                && hdcptx_auth.bksv[1] == REVOKED_KSV[i][1]
                && hdcptx_auth.bksv[2] == REVOKED_KSV[i][2]
                && hdcptx_auth.bksv[3] == REVOKED_KSV[i][3]
                && hdcptx_auth.bksv[4] == REVOKED_KSV[i][4]) {
                MES("bksv is in the revocation list!!!");
                MUTEX_LOCK(THREAD_MTX);
                ret = hdmitx_hdcp_authentication_reset(&hdcptx_auth);
                MUTEX_UNLOCK(THREAD_MTX);
                if (ret != ERR_OK) {
                    ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                    return ret;
                }
                MUTEX_LOCK(THREAD_MTX);
                ret = hdmitx_exit(port);
                MUTEX_UNLOCK(THREAD_MTX);
                if (ret != ERR_OK) {
                    ERR(API_ERROR_OCCURRED, "hdmitx_exit", hv_strerr(ret));
                    return ret;
                }
                return ERR_NG;
            }
        }
    }
    */

    // When sink device is Repeater, call hdmitx_hdcp_repeater_authentication()
    if (hdcptx_auth.is_repeater == HDMITX_HDCP_REPEATER) {
        rep_auth.port = port;
        rep_auth.hdcp_version = hdcptx_auth.hdcp_version;
        rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_0;
        //rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_1;
        //rep_auth.stream_type = HDMITX_HDCP_STREAM_TYPE_AUTO;

        MUTEX_LOCK2();
        ret = hdmitx_hdcp_repeater_authentication(&rep_auth);
        MUTEX_UNLOCK2();
        if (ret != ERR_OK) {
            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_repeater_authentication", hv_strerr(ret));
#ifdef HV_ERRNO
            if (HV_ERRNO != ERR_OK) {
                ERR("%s %s", LAST_ERROR, hv_strerr(HV_ERRNO));
            }
#endif // HV_ERRNO
            return ret;
        }
        MES("LF9129   Downstream count: %d", (int)rep_auth.downstream_count);
        MES("LF9129   Downstream depth: %d", (int)rep_auth.downstream_depth);
        for (i=0; i<rep_auth.downstream_count; i++) {
            MES("LF9129     ksv[%d]: %02X %02X %02X %02X %02X", (int)i,rep_auth.ksv_list[i][0],rep_auth.ksv_list[i][1],rep_auth.ksv_list[i][2],rep_auth.ksv_list[i][3],rep_auth.ksv_list[i][4]);
        }
        MES("LF9129   Bstatus: %02X %02X", rep_auth.bstatus[0], rep_auth.bstatus[1]);

        // TODO: You must check for ksv_list in the current revocation list.
        //       If one or more of ksv_list are in the revocation list,
        //       reset the authentication using hdmitx_hdcp_authentication_reset(),
        //       and call hdmitx_exit() to stop the video signal transmission.
        /* example
        {
            for (i=0; i<REVOCATION_LIST_NUM; i++) {
                for (j=0; j<rep_auth.downstream_sum; j++) {
                    if (rep_auth.ksv_list[j][0] == REVOKED_KSV[i][0]
                        && rep_auth.ksv_list[j][1] == REVOKED_KSV[i][1]
                        && rep_auth.ksv_list[j][2] == REVOKED_KSV[i][2]
                        && rep_auth.ksv_list[j][3] == REVOKED_KSV[i][3]
                        && rep_auth.ksv_list[j][4] == REVOKED_KSV[i][4]) {
                        MES("ksv_list is in the revocation list!!!");
                        MUTEX_LOCK(THREAD_MTX);
                        ret = hdmitx_hdcp_authentication_reset(&hdcptx_auth);
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
                            return ret;
                        }
                        MUTEX_LOCK(THREAD_MTX);
                        ret = hdmitx_exit(port);
                        MUTEX_UNLOCK(THREAD_MTX);
                        if (ret != ERR_OK) {
                            ERR(API_ERROR_OCCURRED, "hdmitx_exit", hv_strerr(ret));
                            return ret;
                        }
                        return ERR_NG;
                    }
                }
            }
        }
        */
    }

    // call hdmitx_hdcp_get_authentication_status() periodically
    THREAD_CREATE(HDCPTX_THREAD_ID, NULL, HDCPTX_THREAD, (void *)&hdcptx_auth);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Set HDIM Rx HDCP Authentication
//------------------------------------------------------------------------------
static uint32_t cmd_set_hdcprx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    hdmirx_hdcp_version hdcp_ver;
    uint32_t ret;

    MES("set HDCP Rx");

    switch (cmd->opts.hdcp_ver) {
        case OPT_HDCP_VER_14:
            hdcp_ver = HDMIRX_HDCP_VERSION_1_4;
            break;
        case OPT_HDCP_VER_22:
            hdcp_ver = HDMIRX_HDCP_VERSION_2_2;
            break;
        case OPT_HDCP_VER_AUTO:
        default:
            hdcp_ver = HDMIRX_HDCP_VERSION_AUTO;
            break;
    }

    hdcprx_auth.port = port;
    hdcprx_auth.port0 = hdcp_ver;
    hdcprx_auth.port1 = hdcp_ver;
    hdcprx_auth.port2 = hdcp_ver;
    hdcprx_auth.port3 = hdcp_ver;
    MES(": %s", (hdcp_ver == HDMIRX_HDCP_VERSION_1_4)? "1.4": (hdcp_ver == HDMIRX_HDCP_VERSION_2_2)? "2.2": (hdcp_ver == HDMIRX_HDCP_VERSION_AUTO)? "AUTO": "???");
    if (cmd->opts.repeater == OPT_REPEATER_ON) {
        hdcprx_auth.repeater = HDMIRX_HDCP_REPEATER;
        MES(": REPEATER");
    } else {
        hdcprx_auth.repeater = HDMIRX_HDCP_NOT_REPEATER;
        MES("");
    }

    HDCPRX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_hdcp_authentication(&hdcprx_auth);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_hdcp_authentication", hv_strerr(ret));
        return ret;
    }

    // call hdmirx_hdcp_get_authentication_status() periodically
    THREAD_CREATE(HDCPRX_THREAD_ID, NULL, HDCPRX_THREAD, (void *)&hdcprx_auth);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Set SPD InfoFrame
//------------------------------------------------------------------------------
static uint32_t cmd_set_spd_infoframe(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_spd_infoframe    spd;
    uint32_t ret;

    MES("set SPD infoframe");

    spd.port = port;
    spd.vendor_name[0] = 0x00;
    spd.vendor_name[1] = 0x01;
    spd.vendor_name[2] = 0x02;
    spd.vendor_name[3] = 0x03;
    spd.vendor_name[4] = 0x04;
    spd.vendor_name[5] = 0x05;
    spd.vendor_name[6] = 0x06;
    spd.vendor_name[7] = 0x07;
    spd.product_description[0] = 0x10;
    spd.product_description[1] = 0x11;
    spd.product_description[2] = 0x12;
    spd.product_description[3] = 0x13;
    spd.product_description[4] = 0x14;
    spd.product_description[5] = 0x15;
    spd.product_description[6] = 0x16;
    spd.product_description[7] = 0x17;
    spd.product_description[8] = 0x18;
    spd.product_description[9] = 0x19;
    spd.product_description[10] = 0x1A;
    spd.product_description[11] = 0x1B;
    spd.product_description[12] = 0x1C;
    spd.product_description[13] = 0x1D;
    spd.product_description[14] = 0x1E;
    spd.product_description[15] = 0x1F;
    spd.source_infomation = 0x20;
    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_set_spd_infoframe(&spd);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_set_spd_infoframe", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Set HDR InfoFrame
//------------------------------------------------------------------------------
static uint32_t cmd_set_hdr_infoframe(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_hdr_infoframe    hdr;
    uint32_t ret;

    MES("set HDR infoframe");

    hdr.port = port;
    hdr.eotf = 0x02;
    hdr.static_metadata_descriptor_id = 0x00;
    hdr.static_metadata_descriptor[0] = 0xC2;
    hdr.static_metadata_descriptor[1] = 0x33;
    hdr.static_metadata_descriptor[2] = 0xC4;
    hdr.static_metadata_descriptor[3] = 0x86;
    hdr.static_metadata_descriptor[4] = 0x4C;
    hdr.static_metadata_descriptor[5] = 0x1D;
    hdr.static_metadata_descriptor[6] = 0xB8;
    hdr.static_metadata_descriptor[7] = 0x0B;
    hdr.static_metadata_descriptor[8] = 0xD0;
    hdr.static_metadata_descriptor[9] = 0x84;
    hdr.static_metadata_descriptor[10] = 0x80;
    hdr.static_metadata_descriptor[11] = 0x3E;
    hdr.static_metadata_descriptor[12] = 0x13;
    hdr.static_metadata_descriptor[13] = 0x3D;
    hdr.static_metadata_descriptor[14] = 0x42;
    hdr.static_metadata_descriptor[15] = 0x40;
    hdr.static_metadata_descriptor[16] = 0x4C;
    hdr.static_metadata_descriptor[17] = 0x04;
    hdr.static_metadata_descriptor[18] = 0x00;
    hdr.static_metadata_descriptor[19] = 0x00;
    hdr.static_metadata_descriptor[20] = 0x00;
    hdr.static_metadata_descriptor[21] = 0x00;
    hdr.static_metadata_descriptor[22] = 0x00;
    hdr.static_metadata_descriptor[23] = 0x00;
    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_set_hdr_infoframe(&hdr);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_set_hdr_infoframe", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_get(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_VIDEO:    ret = cmd_get_video(cmd, p_rxp, p_txp, p_vform);    break;
        case OPT_TARGET_AUDIO:    ret = cmd_get_audio(cmd, p_rxp, p_txp,
                                                    p_rx_aform, p_tx_aform);        break;
        case OPT_TARGET_STATUS: ret = cmd_get_status(cmd, p_vform);                    break;
        case OPT_TARGET_FORMAT: ret = cmd_get_format(cmd, p_vform);                    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }
    
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_video(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_get_video_hdmi(cmd, p_rxp, p_txp,
                                                            p_vform);                break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_video_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_RX:        ret = cmd_get_video_hdmirx(cmd, p_rxp, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Get HDMI Rx Video Format
//------------------------------------------------------------------------------
static uint32_t cmd_get_video_hdmirx(cmds_main *cmd, system_port *p_rxp, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("get HDMI Rx video");

    p_rxp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_vform->sys.rx_port = *p_rxp;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_get_video_format((hdmirx_video *)p_vform);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_get_video_format", hv_strerr(ret));
        return ret;
    }
    MES("  port_num: %d", p_vform->sys.rx_port.port_num);
    print_video_format(p_vform->sys.rx_video_format, p_vform->sys.rx_color_format, p_vform->sys.rx_deep_color);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_audio(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_get_audio_hdmi(cmd, p_rxp, p_txp,
                                                        p_rx_aform, p_tx_aform);    break;
        case OPT_PORT_SPDIF:    ret = cmd_get_audio_spdif(cmd, p_rxp, p_txp,
                                                        p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_audio_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                    system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_RX:        ret = cmd_get_audio_hdmirx(cmd, p_rxp, p_txp,
                                                        p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Get HDMI Rx Audio Format
//------------------------------------------------------------------------------
static uint32_t cmd_get_audio_hdmirx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                        system_audio *p_rx_aform, system_audio *p_tx_aform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("get HDMI Rx audio");

    p_rxp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_rx_aform->port = *p_rxp;
    p_txp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_tx_aform->port = *p_txp;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_get_audio_format((hdmirx_audio *)p_rx_aform);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_get_audio_format", hv_strerr(ret));
        return ret;
    }
    MES("  port_num: %d", p_rx_aform->port.port_num);
    print_audio_format(p_rx_aform);

    // copy Rx data to Tx
    p_tx_aform->if_channel_count        = p_rx_aform->if_channel_count;
    p_tx_aform->if_sampling_freq        = p_rx_aform->if_sampling_freq;
    p_tx_aform->if_sample_size            = p_rx_aform->if_sample_size;
    p_tx_aform->if_channel_number        = p_rx_aform->if_channel_number;
    p_tx_aform->if_level_shift_value    = p_rx_aform->if_level_shift_value;
    p_tx_aform->cb_sampling_freq        = p_rx_aform->cb_sampling_freq;
    p_tx_aform->cb_sample_size            = p_rx_aform->cb_sample_size;
    p_tx_aform->sampling_freq_ratio        = p_rx_aform->sampling_freq_ratio;
    p_tx_aform->i2s.endian_mode            = p_rx_aform->i2s.endian_mode;
    p_tx_aform->i2s.polarity            = p_rx_aform->i2s.polarity;
    p_tx_aform->i2s.i2s_format            = p_rx_aform->i2s.i2s_format;
    p_tx_aform->i2s.start_pos            = p_rx_aform->i2s.start_pos;

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_audio_spdif(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                    system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_RX:        ret = cmd_get_audio_spdifrx(cmd, p_rxp, p_txp,
                                                        p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Get SPDIF Rx Audio Format
//------------------------------------------------------------------------------
static uint32_t cmd_get_audio_spdifrx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp,
                                        system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    MES("get SPDIF Rx audio");

    p_rxp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_rx_aform->port = *p_rxp;
    p_txp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_tx_aform->port = *p_txp;

    // MUTEX_LOCK(THREAD_MTX);
    ret = spdifrx_get_audio_format((spdifrx_audio *)p_rx_aform);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdifrx_get_audio_format", hv_strerr(ret));
        return ret;
    }
    print_audio_format(p_rx_aform);

    // copy Rx data to Tx
    p_tx_aform->if_channel_count        = p_rx_aform->if_channel_count;
    p_tx_aform->if_sampling_freq        = p_rx_aform->if_sampling_freq;
    p_tx_aform->if_sample_size            = p_rx_aform->if_sample_size;
    p_tx_aform->if_channel_number        = p_rx_aform->if_channel_number;
    p_tx_aform->if_level_shift_value    = p_rx_aform->if_level_shift_value;
    p_tx_aform->cb_sampling_freq        = p_rx_aform->cb_sampling_freq;
    p_tx_aform->cb_sample_size            = p_rx_aform->cb_sample_size;
    p_tx_aform->sampling_freq_ratio        = p_rx_aform->sampling_freq_ratio;
    p_tx_aform->i2s.endian_mode            = p_rx_aform->i2s.endian_mode;
    p_tx_aform->i2s.polarity            = p_rx_aform->i2s.polarity;
    p_tx_aform->i2s.i2s_format            = p_rx_aform->i2s.i2s_format;
    p_tx_aform->i2s.start_pos            = p_rx_aform->i2s.start_pos;

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_status(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.stat) {
        case OPT_STAT_HPD:        ret = cmd_get_status_hpd(cmd, p_vform);        break;
        case OPT_STAT_HDCP:        ret = cmd_get_status_hdcp(cmd, p_vform);    break;
        case OPT_STAT_PORT:        ret = cmd_get_status_port(cmd, p_vform);    break;
        case OPT_STAT_AV:        ret = cmd_get_status_av(cmd, p_vform);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Get Hot Plug Detect Status of HDMI Tx
//------------------------------------------------------------------------------
static uint32_t cmd_get_status_hpd(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint8_t stat;
    uint32_t ret;

    MES("get HPD status of HDMI Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_get_hpd_status(p_vform->sys.tx_port, &stat);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_get_hpd_status", hv_strerr(ret));
        return ret;
    }
    switch (stat & HDMITX_EVENT_HPD_MASK) {
        case HDMITX_EVENT_HPD_ACTIVE:
            MES("\nMYH %s %d, HDMI Tx HPD active/", __func__, __LINE__);
            break;
        case HDMITX_EVENT_HPD_INACTIVE:
            MES("\nMYH %s %d, HDMI Tx HPD inactive/", __func__, __LINE__);
            break;
    }
    switch (stat & HDMITX_EVENT_SENSE_MASK) {
        case HDMITX_EVENT_SENSE_ACTIVE:
            MES("MYH %s %d, Sense active", __func__, __LINE__);
            break;
        case HDMITX_EVENT_SENSE_INACTIVE:
            MES("MYH %s %d, Sense inactive", __func__, __LINE__);
            break;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_status_hdcp(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_get_status_hdcptx(cmd, p_vform);    break;
        case OPT_TXRX_RX:        ret = cmd_get_status_hdcprx(cmd, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Get HDCP Status of HDMI Tx
//------------------------------------------------------------------------------
static uint32_t cmd_get_status_hdcptx(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("get HDCP status of HDMI Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_hdcp_get_authentication_status(&hdcptx_auth);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_get_authentication_status", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Get HDCP Status of HDMI Rx
//------------------------------------------------------------------------------
static uint32_t cmd_get_status_hdcprx(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    hdmirx_hdcp_auth_status stat;
    uint32_t ret;

    MES("get HDCP status of HDMI Rx");

    stat.port = hdcprx_auth.port;
    stat.repeater = hdcprx_auth.repeater;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_hdcp_get_authentication_status(&stat);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        if (ret == ERR_HDCPRXXX_PACKET_HEADER_BCH_ERROR) {
            WAR("HDCP Rx: packet header BCH error");
        } else if (ret == ERR_HDCPRXXX_PACKET_BODY_BCH_ERROR) {
            WAR("HDCP Rx: packet body BCH error");
        } else {
            ERR(API_ERROR_OCCURRED, "hdmirx_hdcp_get_authentication_status", hv_strerr(ret));
            ERR("HDCP Rx: %s", hv_strerr(ret));
        }
    }
    switch (stat.eess) {
        case HDMIRX_HDCP_EESS_NONE:
            MES("EESS: None");
            break;
        case HDMIRX_HDCP_EESS_ENC_DIS:
            MES("EESS: ENC_DIS");
            break;
        case HDMIRX_HDCP_EESS_ENC_EN:
            MES("EESS: ENC_EN");
            break;
        default:
            break;
    }
    if (stat.repeater == HDMIRX_HDCP_REPEATER) {
        switch (stat.message) {
            case HDMIRX_HDCP_MESSAGE_AKSV:
                MES("Received Message: Aksv");
                break;
            case HDMIRX_HDCP_MESSAGE_AKE_INIT:
                MES("Received Message: AKE_Init");
                break;
            case HDMIRX_HDCP_MESSAGE_AKE_NO_STORED_KM:
                MES("Received Message: AKE_No_Stored_km");
                break;
            case HDMIRX_HDCP_MESSAGE_AKE_STORED_KM:
                MES("Received Message: AKE_Stored_km");
                break;
            case HDMIRX_HDCP_MESSAGE_LC_INIT:
                MES("Received Message: LC_Init");
                break;
            case HDMIRX_HDCP_MESSAGE_SKE_SEND_EKS:
                MES("Received Message: SKE_Send_Eks");
                break;
            case HDMIRX_HDCP_MESSAGE_REPEATERAUTH_SEND_ACK:
                MES("Received Message: RepeaterAuth_Send_Ack");
                break;
            case HDMIRX_HDCP_MESSAGE_REPEATERAUTH_STREAM_MANAGE:
                MES("Received Message: RepeaterAuth_Stream_Manage");
                break;
            default:
                break;
        }
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Get Port Status of HDMI Rx
//------------------------------------------------------------------------------
static uint32_t cmd_get_status_port(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    hdmirx_port_status stat;
    uint32_t ret;

    MES("get Port status of HDMI Rx");

    stat.port = p_vform->sys.rx_port;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_get_port_status(&stat);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_get_port_status", hv_strerr(ret));
        return ret;
    }
    MES("HDMI Rx Port 0: %sactive", (stat.port0) ? "" : "in");
    MES("HDMI Rx Port 1: %sactive", (stat.port1) ? "" : "in");
    MES("HDMI Rx Port 2: %sactive", (stat.port2) ? "" : "in");
    MES("HDMI Rx Port 3: %sactive", (stat.port3) ? "" : "in");

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Get Audio/Video Status of HDMI Rx
//------------------------------------------------------------------------------
static uint32_t cmd_get_status_av(cmds_main *cmd, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    hdmirx_av_status stat;
    uint32_t ret;

    MES("get AV status of HDMI Rx");

    stat.port = p_vform->sys.rx_port;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_get_av_status(&stat);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_get_av_status", hv_strerr(ret));
        return ret;
    }
    MES("HDMI Rx Port %d: Video Format %schange: Video %smuted", stat.port.port_num,((stat.video_format) ? "" : "not "),((stat.video_mute) ? "un" : ""));
    MES("HDMI Rx Port %d: Audio Format %s: Audio %smuted", stat.port.port_num,((stat.audio_format)? "might have changed": "not change"),((stat.audio_mute) ? "un" : ""));

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_format(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_get_format_hdmi(cmd, p_vform);    break;
        case OPT_PORT_VBO:        ret = cmd_get_format_vbo(cmd, p_vform);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_format_hdmi(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_RX:        ret = cmd_get_format_hdmi_rx(cmd, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_format_hdmi_rx(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    MES("get received video format of HDMI Rx");

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_detect_video_format(p_vform->sys.rx_port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR("%s", COMMAND_ERROR_OCCURRED);
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t hdmirx_detect_video_format(system_port port)
{
    system_reg_access access;
    system_command cmd[1];
    system_block_access baccess;
    uint8_t data[15] = {0};
    uint32_t i;
    uint32_t ret;

    access.p_port = &port;
    access.p_com = cmd;
    access.count = 1;

    // i2cw 22 09 01
    cmd[0].addr = 0x00002209;        // VIDEOIF_VIDEO_CRRSYNC_01_ADDR
    cmd[0].data = 0x01;
    cmd[0].mask = 0xFF;

    ret = lf9129_device_write(&access);
    if (ret != ERR_OK) {
        return ret;
    }
    for (i=0; i<10; i++) {
        ret = lf9129_device_read(&access);
        if (ret != ERR_OK) {
            return ret;
        }
        if ((cmd[0].data & 0x01) == 0x00) {
            break;
        }
        LF9129_CMD_Delay(100);
    }
    if (i == 10) {
        return ERR_NG;
    }

    // i2cr 22 10 15
    baccess.p_port = &port;
    baccess.addr = 0x00002210;        // VIDEOIF_VIDEO_CRRHVPERIOD_00_ADDR
    baccess.length = 15;
    baccess.p_data = data;

    ret = device_block_read(&baccess);
    if (ret != ERR_OK) {
        return ret;
    }

    MES("HPE:0x%02X%02X ", (data[ 1] & 0x1F), data[ 0]);
    MES("VPE:0x%02X%02X ", (data[ 3] & 0x1F), data[ 2]);
    MES("HSW:0x%02X%02X ", (data[ 5] & 0x01), data[ 4]);
    MES("VSW:0x%02X%02X ", (data[ 7] & 0xFF), data[ 6]);
    MES("HDE:0x%02X%02X ", (data[ 9] & 0x1F), data[ 8]);
    MES("VDE:0x%02X%02X ", (data[11] & 0x0F), data[10]);
    MES("HSD:0x%02X%02X ", (data[13] & 0x07), data[12]);
    MES("VSD:0x%04X", data[14]);
    MES("");

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_format_vbo(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_RX:        ret = cmd_get_format_vbo_rx(cmd, p_vform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_get_format_vbo_rx(cmds_main *cmd, system_videos *p_vform)
{
    uint32_t ret, ret2, ret3;

    MES("get received video format of VBO Rx");

    MUTEX_LOCK(THREAD_MTX);
    ret = vborx_start_detect_video_format(p_vform->sys.rx_port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR("%s", COMMAND_ERROR_OCCURRED);
        return ret;
    }

    LF9129_CMD_Delay(1000);

    MUTEX_LOCK(THREAD_MTX);
    ret2 = vborx_detect_video_format(p_vform->sys.rx_port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret2 != ERR_OK) {
        ERR("%s", COMMAND_ERROR_OCCURRED);
        return ret;
    }
    MUTEX_LOCK(THREAD_MTX);
    ret3 = vborx_stop_detect_video_format(p_vform->sys.rx_port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret3 != ERR_OK) {
        ERR("%s", COMMAND_ERROR_OCCURRED);
        return ret;
    }

    return (ret|ret2);
}
//------------------------------------------------------------------------------
static uint32_t vborx_start_detect_video_format(system_port port)
{
    system_reg_access access;
    system_command cmd[1];
    uint32_t i;
    uint32_t ret;

    access.p_port = &port;
    access.p_com = cmd;
    access.count = 1;

    // i2cw 2E 9D C0
    cmd[0].addr = 0x00002E9D;        // VIDEOIF_VBORX_GLUE_AVECTL_1_ADDR
    cmd[0].data = 0xC0;
    cmd[0].mask = 0xFF;

    ret = lf9129_device_write(&access);
    if (ret != ERR_OK) {
        return ret;
    }

    // i2cw 2E 9C 88
    cmd[0].addr = 0x00002E9C;        // VIDEOIF_VBORX_GLUE_AVECTL_0_ADDR
    cmd[0].data = 0x88;
    cmd[0].mask = 0xFF;

    ret = lf9129_device_write(&access);
    if (ret != ERR_OK) {
        return ret;
    }
    for (i=0; i<10; i++) {
        ret = lf9129_device_read(&access);
        if (ret != ERR_OK) {
            return ret;
        }
        if ((cmd[0].data & 0x88) == 0x00) {
            break;
        }
        LF9129_CMD_Delay(100);
    }
    if (i == 10) {
        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_detect_video_format(system_port port)
{
    system_block_access baccess;
    uint8_t data[8] = {0};
    int32_t hs, ht, vs, vt;
    uint32_t ret;

    // i2cr 2E A0 8
    baccess.p_port = &port;
    baccess.addr = 0x00002EA0;        // VIDEOIF_VBORX_GLUE_H_AVE_0_ADDR
    baccess.length = 8;
    baccess.p_data = data;

    ret = device_block_read(&baccess);
    if (ret != ERR_OK) {
        return ret;
    }

    hs = (data[1] << 8) | data[0];
    ht = (data[3] << 8) | data[2];
    vs = (data[5] << 8) | data[4];
    vt = (data[7] << 8) | data[6];

    MES("H_START:0x%02X%02X(%lu) ", data[1], data[0], hs);
    MES("H_TOTAL:0x%02X%02X(%lu) ", data[3], data[2], ht);
    MES("V_START:0x%02X%02X(%lu) ", data[5], data[4], vs);
    MES("V_TOTAL:0x%02X%02X(%lu) ", data[7], data[6], vt);
    MES("");

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t vborx_stop_detect_video_format(system_port port)
{
    system_block_access baccess;
    uint8_t data[2] = {0};
    uint32_t ret;

    // i2cw 2E 9C 00 00
    baccess.p_port = &port;
    baccess.addr = 0x00002E9C;        // VIDEOIF_VBORX_GLUE_AVECTL_0_ADDR
    baccess.length = 2;
    baccess.p_data = data;

    ret = device_block_write(&baccess);
    if (ret != ERR_OK) {
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_reset(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_HDCP:    ret = cmd_reset_hdcp(cmd, rxp, txp);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_reset_hdcp(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_reset_hdcptx(cmd, txp);        break;
        case OPT_TXRX_RX:        ret = cmd_reset_hdcprx(cmd, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Reset HDIM Tx HDCP Authentication
//------------------------------------------------------------------------------
static uint32_t cmd_reset_hdcptx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("reset HDCP Tx, %d", __LINE__);

    HDCPTX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_hdcp_authentication_reset(&hdcptx_auth);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_hdcp_authentication_reset", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Reset HDIM Rx HDCP Authentication
//------------------------------------------------------------------------------
static uint32_t cmd_reset_hdcprx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("reset HDCP Rx");

    HDCPRX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_hdcp_authentication_reset(&hdcprx_auth);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_hdcp_authentication_reset", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_select(cmds_main *cmd, system_port *p_rxp)
{
    uint32_t ret;

    ret = cmd_select_port_hdmirx(cmd, p_rxp);

    return ret;
}
//------------------------------------------------------------------------------
// Select HDMI Rx port
//------------------------------------------------------------------------------
static uint32_t cmd_select_port_hdmirx(cmds_main *cmd, system_port *p_port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("HDMI Rx select port");

    AV_THREAD_CANCEL();

    p_port->port_num = cmd->opts.port_num;

    if (hdcprx_auth.repeater == HDMIRX_HDCP_REPEATER) {
        HDCPRX_THREAD_CANCEL();
        hdcprx_auth.port = *p_port;
    }

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_select_port(*p_port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_select_port", hv_strerr(ret));
        return ret;
    }
    MES("  port_num: %d", cmd->opts.port_num);

    // call hdmirx_get_av_status() periodically
    THREAD_CREATE(AV_THREAD_ID, NULL, AV_THREAD, (void *)p_port);

    if (hdcprx_auth.repeater == HDMIRX_HDCP_REPEATER) {
        // call hdmirx_hdcp_get_authentication_status() periodically
        THREAD_CREATE(HDCPRX_THREAD_ID, NULL, HDCPRX_THREAD, (void *)&hdcprx_auth);
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_VIDEO:    ret = cmd_mute_video(cmd, txp);                        break;
        case OPT_TARGET_AUDIO:    ret = cmd_mute_audio(cmd, txp);                        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_mute_video_hdmi(cmd, txp);                break;
        case OPT_PORT_VBO:        ret = cmd_mute_video_vbo(cmd, txp);                    break;
        case OPT_PORT_LVDS:        ret = cmd_mute_video_lvds(cmd, txp);                break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_hdmi(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_mute_video_hdmitx(cmd, txp);                break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_hdmitx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    switch (cmd->opts.mute_opt) {
        case OPT_MUTE_OPT_NONE:        ret = cmd_mute_video_hdmitx_system(cmd, port);    break;
        case OPT_MUTE_OPT_AVMUTE:    ret = cmd_mute_video_hdmitx_avmute(cmd, port);    break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Mute HDIM Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_hdmitx_system(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_mute tx_mute;
    uint32_t ret;

    MES("mute HDMI Tx video");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// AV Mute HDIM Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_hdmitx_avmute(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_mute tx_mute;
    uint32_t ret;

    MES("mute HDMI Tx video (AVMUTE)");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON_AVMUTE;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_vbo(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:            ret = cmd_mute_video_vbotx(cmd, txp);            break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_vbotx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    switch (cmd->opts.mute_opt) {
        case OPT_MUTE_OPT_NONE:        ret = cmd_mute_video_vbotx_system(cmd, port);    break;
        case OPT_MUTE_OPT_BLACK:    ret = cmd_mute_video_vbotx_black(cmd, port);    break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Mute VBO Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_vbotx_system(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    vbotx_mute tx_mute;
    uint32_t ret;

    MES("mute VBO Tx video");

    tx_mute.port  = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON;

    MUTEX_LOCK(THREAD_MTX);
    ret = vbotx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Black Out VBO Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_vbotx_black(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    vbotx_mute tx_mute;
    uint32_t ret;

    MES("mute VBO Tx video (BLACK)");

    tx_mute.port  = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON_BLACK;

    MUTEX_LOCK(THREAD_MTX);
    ret = vbotx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_lvds(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:            ret = cmd_mute_video_lvdstx(cmd, txp);            break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_lvdstx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    switch (cmd->opts.mute_opt) {
        case OPT_MUTE_OPT_NONE:        ret = cmd_mute_video_lvdstx_system(cmd, port);    break;
        case OPT_MUTE_OPT_BLACK:    ret = cmd_mute_video_lvdstx_black(cmd, port);    break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Mute LVDS Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_lvdstx_system(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    lvdstx_mute tx_mute;
    uint32_t ret;

    MES("mute LVDS Tx video");

    tx_mute.port  = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON;

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Black Out LVDS Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_mute_video_lvdstx_black(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    lvdstx_mute tx_mute;
    uint32_t ret;

    MES("mute LVDS Tx video (BLACK)");

    tx_mute.port  = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON_BLACK;

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_mute_audio_hdmi(cmd, txp);    break;
        case OPT_PORT_SPDIF:    ret = cmd_mute_audio_spdif(cmd, txp);    break;
        case OPT_PORT_I2S:        ret = cmd_mute_audio_i2s(cmd, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio_hdmi(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_mute_audio_hdmitx(cmd, txp);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio_spdif(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_mute_audio_spdiftx(cmd, txp);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio_i2s(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_mute_audio_i2stx(cmd, txp);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Mute HDIM Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio_hdmitx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_mute tx_mute;
    uint32_t ret;

    MES("mute HDMI Tx audio");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_audio_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_audio_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Mute SPDIF Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio_spdiftx(cmds_main *cmd, system_port port)
{
    spdiftx_mute tx_mute;
    uint32_t ret;

    MES("mute SPDIF Tx audio");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON;

    MUTEX_LOCK(THREAD_MTX);
    ret = spdiftx_audio_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdiftx_audio_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Mute I2S Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_mute_audio_i2stx(cmds_main *cmd, system_port port)
{
    i2stx_mute tx_mute;
    uint32_t ret;

    MES("mute I2S Tx audio");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_ON;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2stx_audio_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "i2stx_audio_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.target) {
        case OPT_TARGET_VIDEO:        ret = cmd_unmute_video(cmd, txp);                    break;
        case OPT_TARGET_AUDIO:        ret = cmd_unmute_audio(cmd, txp);                    break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:            ret = cmd_unmute_video_hdmi(cmd, txp);                break;
        case OPT_PORT_VBO:            ret = cmd_unmute_video_vbo(cmd, txp);                break;
        case OPT_PORT_LVDS:            ret = cmd_unmute_video_lvds(cmd, txp);                break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_hdmi(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:            ret = cmd_unmute_video_hdmitx(cmd, txp);            break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_hdmitx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    switch (cmd->opts.mute_opt) {
        case OPT_MUTE_OPT_NONE:        ret = cmd_unmute_video_hdmitx_system(cmd, port);    break;
        case OPT_MUTE_OPT_AVMUTE:    ret = cmd_unmute_video_hdmitx_avmute(cmd, port);    break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Unmute HDIM Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_hdmitx_system(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_mute tx_mute;
    uint32_t ret;

    MES("%s, unmute HDMI Tx video", __func__);

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// AV Unmute HDIM Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_hdmitx_avmute(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_mute tx_mute;
    uint32_t ret;

    MES("unmute HDMI Tx video (AVMUTE)");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF_AVMUTE;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_vbo(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:            ret = cmd_unmute_video_vbotx(cmd, txp);            break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_vbotx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    switch (cmd->opts.mute_opt) {
        case OPT_MUTE_OPT_NONE:        ret = cmd_unmute_video_vbotx_system(cmd, port);    break;
        case OPT_MUTE_OPT_BLACK:    ret = cmd_unmute_video_vbotx_black(cmd, port);    break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Unmute VBO Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_vbotx_system(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    vbotx_mute tx_mute;
    uint32_t ret;

    MES("unmute VBO Tx video");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF;

    // MUTEX_LOCK(THREAD_MTX);
    ret = vbotx_video_mute(&tx_mute);
    // MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_video_mute", hv_strerr(ret));
        return ret;
    }
    MES("unmute VBO Tx video end, video OK");

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Unmute normally VBO Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_vbotx_black(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    vbotx_mute tx_mute;
    uint32_t ret;

    MES("unmute VBO Tx video (BLACK)");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF_BLACK;

    MUTEX_LOCK(THREAD_MTX);
    ret = vbotx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_lvds(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:            ret = cmd_unmute_video_lvdstx(cmd, txp);            break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_lvdstx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    switch (cmd->opts.mute_opt) {
        case OPT_MUTE_OPT_NONE:        ret = cmd_unmute_video_lvdstx_system(cmd, port);    break;
        case OPT_MUTE_OPT_BLACK:    ret = cmd_unmute_video_lvdstx_black(cmd, port);        break;
        default:                    ERR("%s", COMMAND_ERROR_OCCURRED);                    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Unmute LVDS Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_lvdstx_system(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    lvdstx_mute tx_mute;
    uint32_t ret;

    MES("unmute LVDS Tx video");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF;

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Unmute normally LVDS Tx Video
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_video_lvdstx_black(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    lvdstx_mute tx_mute;
    uint32_t ret;

    MES("unmute LVDS Tx video (BLACK)");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF_BLACK;

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_video_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_video_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_unmute_audio_hdmi(cmd, txp);        break;
        case OPT_PORT_SPDIF:    ret = cmd_unmute_audio_spdif(cmd, txp);        break;
        case OPT_PORT_I2S:        ret = cmd_unmute_audio_i2s(cmd, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio_hdmi(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_unmute_audio_hdmitx(cmd, txp);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio_spdif(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_unmute_audio_spdiftx(cmd, txp);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio_i2s(cmds_main *cmd, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_unmute_audio_i2stx(cmd, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);            return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Unmute HDIM Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio_hdmitx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_mute tx_mute;
    uint32_t ret;

    MES("unmute HDMI Tx audio");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF;

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_audio_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_audio_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Unmute SPDIF Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio_spdiftx(cmds_main *cmd, system_port port)
{
    spdiftx_mute tx_mute;
    uint32_t ret;

    MES("unmute SPDIF Tx audio");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF;

    MUTEX_LOCK(THREAD_MTX);
    ret = spdiftx_audio_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdiftx_audio_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Unmute I2S Tx Audio
//------------------------------------------------------------------------------
static uint32_t cmd_unmute_audio_i2stx(cmds_main *cmd, system_port port)
{
    i2stx_mute tx_mute;
    uint32_t ret;

    MES("unmute I2S Tx audio");

    tx_mute.port = port;
    tx_mute.mute = SYSTEM_TX_MUTE_OFF;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2stx_audio_mute(&tx_mute);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "i2stx_audio_mute", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_abort_hdmi(cmd);            break;
        case OPT_PORT_HDCP:        ret = cmd_abort_hdcp(cmd);            break;
        case OPT_PORT_VBO:        ret = cmd_abort_vbo(cmd);            break;
        case OPT_PORT_LVDS:        ret = cmd_abort_lvds(cmd);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_hdmi(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_abort_hdmitx(cmd);        break;
        case OPT_TXRX_RX:        ret = cmd_abort_hdmirx(cmd);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_hdmitx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("abort HDMI Tx");
    ret = hdmitx_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_hdmirx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("abort HDMI Rx");
    ret = hdmirx_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_hdcp(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_abort_hdcptx(cmd);        break;
        case OPT_TXRX_RX:        ret = cmd_abort_hdcprx(cmd);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_hdcptx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("abort HDCP Tx");
    ret = hdmitx_hdcp_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_hdcprx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("abort HDCP Rx");
    ret = hdmirx_hdcp_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_vbo(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_abort_vbotx(cmd);            break;
        case OPT_TXRX_RX:        ret = cmd_abort_vborx(cmd);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_vbotx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    uint32_t ret;

    MES("abort VBO Tx");
    ret = vbotx_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_vborx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    uint32_t ret;

    MES("abort VBO Rx");
    ret = vborx_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_lvds(cmds_main *cmd)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_abort_lvdstx(cmd);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);    return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_abort_lvdstx(cmds_main *cmd)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    uint32_t ret;

    MES("abort LVDS Rx");
    ret = lvdstx_abort();

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_sleep_main(cmds_main *cmd)
{
    uint32_t ms;

    ms = cmd->opts.msec;
    MES("sleep %u", (unsigned int)ms);
    LF9129_CMD_Delay(ms);
    return ERR_OK;
}
//------------------------------------------------------------------------------
#if defined(__linux)
static uint32_t gettimestamp(long *sec, long *nsec)
{
    static struct timespec base = {0};
    struct timespec t;
    long s, ns;

    if (sec == NULL || nsec == NULL) {
        return ERR_NULL_POINTER;
    }
    if (!(base.tv_nsec)) {
        if (clock_gettime(CLOCK_MONOTONIC, &base))    return ERR_NG;
    }
    if (clock_gettime(CLOCK_MONOTONIC, &t))    return ERR_NG;
    s = t.tv_sec - base.tv_sec;
    ns = t.tv_nsec - base.tv_nsec;
    if (ns < 0) {
        s--;
        ns += 1000000000;
    }
    *sec = s;
    *nsec = ns;

    return ERR_OK;
}
//------------------------------------------------------------------------------
#elif defined(_WIN32)
static uint32_t gettimestamp(long *sec, long *nsec)
{
    static LARGE_INTEGER freq = {.QuadPart=0};
    static LARGE_INTEGER base = {.QuadPart=0};
    LARGE_INTEGER t;
    double n;

    if (sec == NULL || nsec == NULL) {
        return ERR_NULL_POINTER;
    }
    if (!(freq.QuadPart)) {
        if (!QueryPerformanceFrequency(&freq))    return ERR_NG;
        if (!QueryPerformanceCounter(&base))    return ERR_NG;
    }
    if (!QueryPerformanceCounter(&t))    return ERR_NG;

    t.QuadPart -= base.QuadPart;
    n = (double)(t.QuadPart) / (double)(freq.QuadPart);
    t.QuadPart = n * 1000000000;
    *sec = t.QuadPart / 1000000000;
    *nsec = (t.QuadPart % 1000000000);

    return ERR_OK;
}
//------------------------------------------------------------------------------
#else // __linux _WIN32
static uint32_t gettimestamp(long *sec, long *nsec)
{
    return ERR_NG;
}
//------------------------------------------------------------------------------
#endif // __linux _WIN32
static uint32_t getmonotonictime(long *sec, long *nsec, int is_clear)
{
    static long base_sec = 0;
    static long base_nsec = 0;
    long s, n;

    if (!base_nsec) {
        if (gettimestamp(&base_sec, &base_nsec)) {
            return ERR_NG;
        }
        s = base_sec;
        n = base_nsec;
    } else {
        if (is_clear) {
            if (gettimestamp(&base_sec, &base_nsec)) {
                return ERR_NG;
            }
            s = base_sec;
            n = base_nsec;
        } else {
            if (gettimestamp(&s, &n)) {
                return ERR_NG;
            }
        }
    }

    s = s - base_sec;
    n = n - base_nsec;
    if (n < 0) {
        s--;
        n += 1000000000;
    }
    *sec = s;
    *nsec = n;

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t cmd_time(cmds_main *cmd)
{
    long sec, nsec;
    uint32_t ret;

    ret = getmonotonictime(&sec, &nsec, ((cmd->opts.target == OPT_TARGET_CLEAR) ? 1 : 0));
    if (ret != ERR_OK) {
        ERR("time function error has occurred.");
        return ERR_NG;
    }
    MES("time: %ld.%09lds", sec, nsec);

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t cmd_auto(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_auto_hdmi(cmd, p_rxp, p_txp, p_vform,
                                                    p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }
    
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_auto_hdmi(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_auto_hdmitx(cmd, p_rxp, p_txp, p_vform,
                                                    p_rx_aform, p_tx_aform);    break;
        case OPT_TXRX_RX:        ret = cmd_auto_hdmirx(cmd, p_rxp, p_txp, p_vform,
                                                    p_rx_aform, p_tx_aform);    break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Auto HDMI Tx
//------------------------------------------------------------------------------
static uint32_t cmd_auto_hdmitx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t ret;

    switch (cmd->opts.target2) {
        case OPT_TARGET2_SG:    ret = cmd_auto_hdmitx_sg(cmd, p_rxp, p_txp, p_vform,
                                                    p_rx_aform, p_tx_aform);    break;
        case OPT_TARGET2_HDCP:    ret = cmd_auto_hdmitx_hdcp(cmd, p_rxp, p_txp, p_vform,
                                                    p_rx_aform, p_tx_aform);
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);                return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_auto_hdmitx_sg(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_sg_video vform;
    uint32_t ret;

    MES("auto HDMI Tx SG");

    AUTO_HDMITX_SG_THREAD_CANCEL();

    // settings for system_port and hdmitx_video
    p_txp->port_num = SYSTEM_PORT_NUMBER_0;
    p_txp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_txp->rxtx_type = SYSTEM_RXTX_TYPE_TX;
    p_txp->chip_num = SYSTEM_CHIP_NUMBER_0;
    p_vform->htx.tx_port = *p_txp;
    switch (cmd->opts.resolution) {
        case OPT_RESOLUTION_FHD:
            p_vform->htx.tx_video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
            break;
        case OPT_RESOLUTION_720:
            p_vform->htx.tx_video_format = SYSTEM_VFORM_1280x720P_5994or60_16v9;
            break;
        case OPT_RESOLUTION_480:
            p_vform->htx.tx_video_format = SYSTEM_VFORM_720x480P_5994or60_4v3;
            break;
        default:    
            ERR("%s", UNSUPPORTED_COMMAND);
            return ERR_NG;
    }
    p_vform->htx.tx_color_format = SYSTEM_COLOR_RGB_FULL_RANGE1;
    p_vform->htx.tx_deep_color = SYSTEM_DEEP_COLOR_8BIT;
    p_vform->htx.tx_dvi_mode = HDMITX_DVI_MODE_DISABLE;
    // settings for sg format
    vform.port = p_vform->htx.tx_port;
    vform.video_format = p_vform->htx.tx_video_format;
    vform.color_format = p_vform->htx.tx_color_format;
    vform.deep_color = p_vform->htx.tx_deep_color;
    vform.dvi_mode = p_vform->htx.tx_dvi_mode;
    vform.sg_format = SYSTEM_SG_FORMAT_COLOR_BAR_RGB;
    p_vform->htx.tx_port = vform.port;    // measure of make warning

    // auto
    THREAD_CREATE(AUTO_HDMITX_SG_THREAD_ID, NULL, AUTO_HDMITX_SG_THREAD, (void *)&vform);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}

//------------------------------------------------------------------------------
static uint32_t cmd_auto_hdmitx_hdcp(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
#if defined(CONFIG_HV_SC1H21BJ2) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || \
    defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    MES("auto HDMI Tx HDCP");


    AUTO_HDMITX_HDCP_THREAD_CANCEL();

    // whether automatic unmute is disabled or not

    hdcptx_auth_auto_unmute = (cmd->opts.data) ? DISABLE_UNMUTE : ENABLE_UNMUTE;
    if (hdcptx_auth_auto_unmute == DISABLE_UNMUTE) {
        MES("  auto unmute: disabled");
    }

    // auto
    MES("open auto hdmitx hdcp thread");
    THREAD_CREATE(AUTO_HDMITX_HDCP_THREAD_ID, NULL, AUTO_HDMITX_HDCP_THREAD, (void *)&(p_vform->htx));

    return ERR_OK;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}

//------------------------------------------------------------------------------
// Auto HDMI Rx
//------------------------------------------------------------------------------
static uint32_t cmd_auto_hdmirx(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("auto HDMI Rx");

    AUTO_HDMIRX_THREAD_CANCEL();
    PORT_THREAD_CANCEL();
    AV_THREAD_CANCEL();
    HDCPRX_THREAD_CANCEL();

    // preparation
    if (p_vform->sys.tx_video_format == SYSTEM_VFORM_UNUSED
        || p_vform->sys.tx_video_format == SYSTEM_VFORM_MAX
        || p_vform->sys.tx_color_format == SYSTEM_COLOR_MAX
        || p_vform->sys.tx_deep_color == SYSTEM_DEEP_COLOR_MAX) {
        // temporary setting
        p_vform->sys.tx_video_format = SYSTEM_VFORM_1920x1080P_5994or60_16v9;
        p_vform->sys.tx_color_format = SYSTEM_COLOR_RGB_FULL_RANGE1;
        p_vform->sys.tx_deep_color = SYSTEM_DEEP_COLOR_8BIT;
    }

    // init
    p_rxp->port_num = cmd->opts.port_num;
    p_rxp->port_type = SYSTEM_PORT_TYPE_HDMI;
    p_rxp->rxtx_type = SYSTEM_RXTX_TYPE_RX;
    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_init(*p_rxp);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_init", hv_strerr(ret));
        return ret;
    }
    // call hdmirx_get_port_status() periodically
    THREAD_CREATE(PORT_THREAD_ID, NULL, PORT_THREAD, (void *)p_rxp);

    // hdcp
    hdcprx_auth.port = *p_rxp;
    hdcprx_auth.port0 = HDMIRX_HDCP_VERSION_AUTO;
    hdcprx_auth.port1 = HDMIRX_HDCP_VERSION_AUTO;
    hdcprx_auth.port2 = HDMIRX_HDCP_VERSION_AUTO;
    hdcprx_auth.port3 = HDMIRX_HDCP_VERSION_AUTO;
    hdcprx_auth.repeater = HDMIRX_HDCP_NOT_REPEATER;
    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_hdcp_authentication(&hdcprx_auth);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_hdcp_authentication", hv_strerr(ret));
        return ret;
    }
    // call hdmirx_hdcp_get_authentication_status() periodically
    THREAD_CREATE(HDCPRX_THREAD_ID, NULL, HDCPRX_THREAD, (void *)&hdcprx_auth);

    // select
    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_select_port(*p_rxp);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_select_port", hv_strerr(ret));
        return ret;
    }
    MES("  port_num: %d", cmd->opts.port_num);
    // call hdmirx_get_av_status() periodically
    THREAD_CREATE(AV_THREAD_ID, NULL, AV_THREAD, (void *)p_rxp);

    // auto
    p_vform->sys.rx_port = *p_rxp;
    THREAD_CREATE(AUTO_HDMIRX_THREAD_ID, NULL, AUTO_HDMIRX_THREAD, (void *)p_vform);

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_misc(cmds_main *cmd, system_port *p_rxp, system_port *p_txp, system_videos *p_vform,
                            system_audio *p_rx_aform, system_audio *p_tx_aform)
{
    uint32_t i;
    uint32_t ret = ERR_NG;

    for (i=0; cmd->tok[i][0] != '\0'; i++) {
        MES("%s ", cmd->tok[i]);
    }
    MES("");

    switch (cmd->tok[1][0]) {
        case '0':
            // command: misc 0 <hsmode: 0|1> [H total length] [H sync width]
            ret = cmd_misc_vborx_set_hsync(cmd, i, *p_rxp);
            break;
        case '1':
            // preset video hdmi tx <resolution> <fps> <color format> <deep color>
            // init hdmi tx
            // command: misc 1 <sg format: 1|2|3|4|5|6|7>    ... see system_sg_format
            // unmute video hdmi tx
            ret = cmd_misc_hdmitx_set_sg_format(cmd, i, p_vform);
            break;
        default:
            ERR("the specified misc index is not found.");
            break;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_misc_vborx_set_hsync(cmds_main *cmd, int cnt, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    vborx_hsync hsync;
    uint32_t ret;

    hsync.port = port;
    hsync.hsmode = VBORX_HSMODE_EXTERNAL_INPUT;
    hsync.htotal_length = 0;
    hsync.hsync_width = 0;

    if (cnt > 2)
    hsync.hsmode = (strtol(cmd->tok[2], NULL, 10) == 0)                // (DEC) hsmode: 1 or 0
                    ? VBORX_HSMODE_EXTERNAL_INPUT
                    : VBORX_HSMODE_INTERNAL_GENERATION;
    if (cnt > 3)
    hsync.htotal_length = (uint16_t)strtol(cmd->tok[3], NULL, 10);    // (DEC) H total length
    if (cnt > 4)
    hsync.hsync_width = (uint16_t)strtol(cmd->tok[4], NULL, 10);    // (DEC) H sync width

    MES("set VBO Rx hsync: hsmode=%d, Htotal=%d, Hsyncw=%d",hsync.hsmode,hsync.htotal_length,hsync.hsync_width);

    MUTEX_LOCK(THREAD_MTX);
    ret = vborx_set_hsync(&hsync);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vborx_set_hsync", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_misc_hdmitx_set_sg_format(cmds_main *cmd, int cnt, system_videos *p_vform)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    hdmitx_sg_video vform;
    uint32_t ret;

    vform.port = p_vform->sys.tx_port;
    vform.video_format = p_vform->sys.tx_video_format;
    vform.color_format = p_vform->sys.tx_color_format;
    vform.deep_color = p_vform->sys.tx_deep_color;
    vform.dvi_mode = p_vform->htx.tx_dvi_mode;

    if (cnt > 2) {
        switch (cmd->tok[2][0]) {
            case '1':    vform.sg_format = SYSTEM_SG_FORMAT_ASYNC_BLANK;        break;
            case '2':    vform.sg_format = SYSTEM_SG_FORMAT_BLANK;            break;
            case '3':    vform.sg_format = SYSTEM_SG_FORMAT_RAMP_A;            break;
            case '4':    vform.sg_format = SYSTEM_SG_FORMAT_RAMP_B;            break;
            case '5':    vform.sg_format = SYSTEM_SG_FORMAT_TOGGLE;            break;
            case '6':    vform.sg_format = SYSTEM_SG_FORMAT_COLOR_BAR_RGB;    break;
            case '7':    vform.sg_format = SYSTEM_SG_FORMAT_COLOR_BAR_444;    break;
            default:    ERR("the specified sg format index is not found.");    return ERR_NG;
        }
    }

    MES("set video format as signal generator for HDMI Tx:");
    print_video_format(vform.video_format, vform.color_format, vform.deep_color);
    MES("  %s", vform.sg_format == SYSTEM_SG_FORMAT_ASYNC_BLANK? "blank (asynchronous)": vform.sg_format == SYSTEM_SG_FORMAT_BLANK? "blank (Vsync synchronous)": vform.sg_format == SYSTEM_SG_FORMAT_RAMP_A? "ramp A (Vsync synchronous)": vform.sg_format == SYSTEM_SG_FORMAT_RAMP_B? "ramp B (Vsync synchronous)": vform.sg_format == SYSTEM_SG_FORMAT_TOGGLE? "toggle (Vsync synchronous)": vform.sg_format == SYSTEM_SG_FORMAT_COLOR_BAR_RGB? "color bar by RGB (Vsync synchronous)": vform.sg_format == SYSTEM_SG_FORMAT_COLOR_BAR_444? "color bar by YCbCr444 (Vsync synchronous)": "the specified sg format index is not found.");
    if (vform.dvi_mode == HDMITX_DVI_MODE_ENABLE) {
        MES("  DVI mode");
    }

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_set_sg_format(&vform);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_set_sg_format", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
static uint32_t cmd_exit(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.port) {
        case OPT_PORT_HDMI:        ret = cmd_exit_hdmi(cmd, rxp, txp);        break;
        case OPT_PORT_VBO:        ret = cmd_exit_vbo(cmd, rxp, txp);        break;
        case OPT_PORT_LVDS:        ret = cmd_exit_lvds(cmd, rxp, txp);        break;
        case OPT_PORT_SPDIF:    ret = cmd_exit_spdif(cmd, rxp, txp);    break;
        case OPT_PORT_I2S:        ret = cmd_exit_i2s(cmd, rxp, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }
    
    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_exit_hdmi(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_exit_hdmitx(cmd, txp);        break;
        case OPT_TXRX_RX:        ret = cmd_exit_hdmirx(cmd, rxp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_exit_vbo(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_exit_vbotx(cmd, txp);            break;
        case OPT_TXRX_RX:        ret = cmd_exit_vborx(cmd, rxp);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_exit_lvds(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_exit_lvdstx(cmd, txp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_exit_spdif(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_exit_spdiftx(cmd, txp);        break;
        case OPT_TXRX_RX:        ret = cmd_exit_spdifrx(cmd, rxp);        break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_exit_i2s(cmds_main *cmd, system_port rxp, system_port txp)
{
    uint32_t ret;

    switch (cmd->opts.txrx) {
        case OPT_TXRX_TX:        ret = cmd_exit_i2stx(cmd, txp);            break;
        case OPT_TXRX_RX:        ret = cmd_exit_i2srx(cmd, rxp);            break;
        default:                ERR("%s", COMMAND_ERROR_OCCURRED);        return ERR_NG;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Exit HDMI Tx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_hdmitx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
    uint32_t ret;

    MES("exit HDMI Tx");

    AUTO_HDMITX_SG_THREAD_CANCEL();
    HPD_THREAD_CANCEL();
    HDCPTX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmitx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmitx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Exit HDMI Rx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_hdmirx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2)
    uint32_t ret;

    MES("exit HDMI Rx");

    AUTO_HDMIRX_THREAD_CANCEL();
    PORT_THREAD_CANCEL();
    AV_THREAD_CANCEL();
    HDCPRX_THREAD_CANCEL();

    MUTEX_LOCK(THREAD_MTX);
    ret = hdmirx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "hdmirx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Exit VBO Tx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_vbotx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H28BJ2)
    uint32_t ret;

    MES("exit VBO Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = vbotx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vbotx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Exit VBO Rx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_vborx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H27BJ2) || defined(CONFIG_HV_SC1H28BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    uint32_t ret;

    MES("exit VBO Rx");

    MUTEX_LOCK(THREAD_MTX);
    ret = vborx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "vborx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Exit LVDS Tx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_lvdstx(cmds_main *cmd, system_port port)
{
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
    uint32_t ret;

    MES("exit LVDS Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = lvdstx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "lvdstx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
#else
    ERR("%s", UNSUPPORTED_COMMAND);
    return ERR_NG;
#endif
}
//------------------------------------------------------------------------------
// Exit SPDIF Tx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_spdiftx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("exit SPDIF Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = spdiftx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdiftx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Exit SPDIF Rx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_spdifrx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("exit SPDIF Rx");

    MUTEX_LOCK(THREAD_MTX);
    ret = spdifrx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "spdifrx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Exit I2S Tx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_i2stx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("exit I2S Tx");

    MUTEX_LOCK(THREAD_MTX);
    ret = i2stx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "i2stx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
// Exit I2S Rx
//------------------------------------------------------------------------------
static uint32_t cmd_exit_i2srx(cmds_main *cmd, system_port port)
{
    uint32_t ret;

    MES("exit I2S Rx");

    MUTEX_LOCK(THREAD_MTX);
    ret = i2srx_exit(port);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret != ERR_OK) {
        ERR(API_ERROR_OCCURRED, "i2srx_exit", hv_strerr(ret));
        return ret;
    }

    return ret;
}
//------------------------------------------------------------------------------
static void version(void)
{
    MES("%s sample software version %s (build date: %s)", HV_PRODUCT_NO, HV_VERSION, HV_BUILD_DATE);
    MES("%s", HV_COPYRIGHT);
    MES("");
}
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H27BJ2)
static void dump_bin(uint8_t *data, uint16_t size)
{
    uint16_t i;
    uint8_t remainder;

    MES("     +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F");
    for (i=0; i<size; i++) {
        remainder = i % 0x10;
        if (remainder == 0x00) {
            MES("[%02X] ", (i & 0xF0));
        } else if (remainder == 0x08) {
            MES(" ");
        }
        MES("%02X ", data[i]);
        if (remainder == 0x0F) {
            MES("\n");
        }
    }
}
#endif
//------------------------------------------------------------------------------
static uint32_t get_video_format(opt_resolution res, opt_fps fps, system_video_format *p_vform)
{
    switch (res) {
        case OPT_RESOLUTION_UHD:
            switch (fps) {
                case OPT_FPS_24P:    *p_vform = SYSTEM_VFORM_3840x2160P_2398or24_16v9;        break;
                case OPT_FPS_25P:    *p_vform = SYSTEM_VFORM_3840x2160P_25_16v9;                break;
                case OPT_FPS_30P:    *p_vform = SYSTEM_VFORM_3840x2160P_2997or30_16v9;        break;
                case OPT_FPS_50P:    *p_vform = SYSTEM_VFORM_3840x2160P_50_16v9;                break;
                case OPT_FPS_60P:    *p_vform = SYSTEM_VFORM_3840x2160P_5994or60_16v9;        break;
                default:                                                                    return ERR_NG;
            }
                                                                                            break;
        case OPT_RESOLUTION_FHD:
            switch (fps) {
                case OPT_FPS_24P:    *p_vform = SYSTEM_VFORM_1920x1080P_2398or24_16v9;        break;
                case OPT_FPS_25P:    *p_vform = SYSTEM_VFORM_1920x1080P_25_16v9;                break;
                case OPT_FPS_30P:    *p_vform = SYSTEM_VFORM_1920x1080P_2997or30_16v9;        break;
                case OPT_FPS_50P:    *p_vform = SYSTEM_VFORM_1920x1080P_50_16v9;                break;
                case OPT_FPS_60P:    *p_vform = SYSTEM_VFORM_1920x1080P_5994or60_16v9;        break;
                default:                                                                    return ERR_NG;
            }
                                                                                            break;
        case OPT_RESOLUTION_DCI4K:
            switch (fps) {
                case OPT_FPS_24P:    *p_vform = SYSTEM_VFORM_4096x2160P_2398or24_256v135;    break;
                case OPT_FPS_25P:    *p_vform = SYSTEM_VFORM_4096x2160P_25_256v135;            break;
                case OPT_FPS_30P:    *p_vform = SYSTEM_VFORM_4096x2160P_2997or30_256v135;    break;
                case OPT_FPS_50P:    *p_vform = SYSTEM_VFORM_4096x2160P_50_256v135;            break;
                case OPT_FPS_60P:    *p_vform = SYSTEM_VFORM_4096x2160P_5994or60_256v135;    break;
                default:                                                                    return ERR_NG;
            }
                                                                                            break;
        case OPT_RESOLUTION_FWXGA:
            switch (fps) {
                case OPT_FPS_60P:    *p_vform = SYSTEM_VFORM_1366x768P_5979or60_16v9;        break;
                default:                                                                    return ERR_NG;
            }
                                                                                            break;
        case OPT_RESOLUTION_720:
            switch (fps) {
                case OPT_FPS_50P:    *p_vform = SYSTEM_VFORM_1280x720P_50_16v9;                break;
                case OPT_FPS_60P:    *p_vform = SYSTEM_VFORM_1280x720P_5994or60_16v9;        break;
                default:                                                                    return ERR_NG;
            }
                                                                                            break;
        case OPT_RESOLUTION_480:
            switch (fps) {
                case OPT_FPS_60P:    *p_vform = SYSTEM_VFORM_720x480P_5994or60_4v3;            break;
                default:                                                                    return ERR_NG;
            }
                                                                                            break;
        default:                                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_video_format_str(system_video_format vform, char **p_str)
{
    static char ret[16];

    switch (vform) {
        case SYSTEM_VFORM_3840x2160P_2398or24_16v9:        *p_str = "3840x2160p 23.98/24Hz";    break;
        case SYSTEM_VFORM_3840x2160P_25_16v9:            *p_str = "3840x2160p 25Hz";            break;
        case SYSTEM_VFORM_3840x2160P_2997or30_16v9:        *p_str = "3840x2160p 29.97/30Hz";    break;
        case SYSTEM_VFORM_3840x2160P_50_16v9:            *p_str = "3840x2160p 50Hz";            break;
        case SYSTEM_VFORM_3840x2160P_5994or60_16v9:        *p_str = "3840x2160p 59.94/60Hz";    break;
        case SYSTEM_VFORM_4096x2160P_2398or24_256v135:    *p_str = "4096x2160p 23.98/24Hz";    break;
        case SYSTEM_VFORM_4096x2160P_25_256v135:        *p_str = "4096x2160p 25Hz";            break;
        case SYSTEM_VFORM_4096x2160P_2997or30_256v135:    *p_str = "4096x2160p 29.97/30Hz";    break;
        case SYSTEM_VFORM_4096x2160P_50_256v135:        *p_str = "4096x2160p 50Hz";            break;
        case SYSTEM_VFORM_4096x2160P_5994or60_256v135:    *p_str = "4096x2160p 59.94/60Hz";    break;
        case SYSTEM_VFORM_1920x1080P_2398or24_16v9:        *p_str = "1920x1080p 23.98/24Hz";    break;
        case SYSTEM_VFORM_1920x1080P_25_16v9:            *p_str = "1920x1080p 25Hz";            break;
        case SYSTEM_VFORM_1920x1080P_2997or30_16v9:        *p_str = "1920x1080p 29.97/30Hz";    break;
        case SYSTEM_VFORM_1920x1080P_50_16v9:            *p_str = "1920x1080p 50Hz";            break;
        case SYSTEM_VFORM_1920x1080P_5994or60_16v9:        *p_str = "1920x1080p 59.94/60Hz";    break;
        case SYSTEM_VFORM_1366x768P_5979or60_16v9:        *p_str = "1366x768p 59.79/60Hz";    break;
        case SYSTEM_VFORM_1280x720P_50_16v9:            *p_str = "1280x720p 50Hz";            break;
        case SYSTEM_VFORM_1280x720P_5994or60_16v9:        *p_str = "1280x720p 59.94/60Hz";    break;
        case SYSTEM_VFORM_720x480P_5994or60_4v3:        *p_str = "720x480p 59.94/60Hz";        break;
        default:                                        sprintf(ret, "unknown(%02X)", vform);
                                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_color_format(opt_color_format cf, system_color_format *p_cform)
{
    switch (cf) {
        case OPT_COLOR_FORMAT_YCBCR422_BT601:    *p_cform = SYSTEM_COLOR_YCBCR422_ITU_R_BT601;    break;
        case OPT_COLOR_FORMAT_XVYCC422_BT601:    *p_cform = SYSTEM_COLOR_XVYCC422_ITU_R_BT601;    break;
        case OPT_COLOR_FORMAT_YCBCR444_BT601:    *p_cform = SYSTEM_COLOR_YCBCR444_ITU_R_BT601;    break;
        case OPT_COLOR_FORMAT_XVYCC444_BT601:    *p_cform = SYSTEM_COLOR_XVYCC444_ITU_R_BT601;    break;
        case OPT_COLOR_FORMAT_YCBCR422_BT709:    *p_cform = SYSTEM_COLOR_YCBCR422_ITU_R_BT709;    break;
        case OPT_COLOR_FORMAT_XVYCC422_BT709:    *p_cform = SYSTEM_COLOR_XVYCC422_ITU_R_BT709;    break;
        case OPT_COLOR_FORMAT_YCBCR444_BT709:    *p_cform = SYSTEM_COLOR_YCBCR444_ITU_R_BT709;    break;
        case OPT_COLOR_FORMAT_XVYCC444_BT709:    *p_cform = SYSTEM_COLOR_XVYCC444_ITU_R_BT709;    break;
        case OPT_COLOR_FORMAT_RGB_FULL1:        *p_cform = SYSTEM_COLOR_RGB_FULL_RANGE1;        break;
        case OPT_COLOR_FORMAT_RGB_LIMITED:        *p_cform = SYSTEM_COLOR_RGB_LIMITED_RANGE;        break;
        case OPT_COLOR_FORMAT_RGB_FULL2:        *p_cform = SYSTEM_COLOR_RGB_FULL_RANGE2;        break;
        case OPT_COLOR_FORMAT_YCBCR420_BT601:    *p_cform = SYSTEM_COLOR_YCBCR420_ITU_R_BT601;    break;
        case OPT_COLOR_FORMAT_YCBCR420_BT709:    *p_cform = SYSTEM_COLOR_YCBCR420_ITU_R_BT709;    break;
        case OPT_COLOR_FORMAT_YCBCR420_BT2020:    *p_cform = SYSTEM_COLOR_YCBCR420_ITU_R_BT2020;    break;
        case OPT_COLOR_FORMAT_YCBCR422_BT2020:    *p_cform = SYSTEM_COLOR_YCBCR422_ITU_R_BT2020;    break;
        case OPT_COLOR_FORMAT_YCBCR444_BT2020:    *p_cform = SYSTEM_COLOR_YCBCR444_ITU_R_BT2020;    break;
        case OPT_COLOR_FORMAT_RGB_BT2020:        *p_cform = SYSTEM_COLOR_RGB_ITU_R_BT2020;        break;
        default:                                                                                return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_color_format_str(system_color_format cform, char **p_str)
{
    static char ret[16];

    switch (cform) {
        case SYSTEM_COLOR_YCBCR420_ITU_R_BT601:        *p_str = "YCbCr4:2:0 BT.601";    break;
        case SYSTEM_COLOR_YCBCR422_ITU_R_BT601:        *p_str = "YCbCr4:2:2 BT.601";    break;
        case SYSTEM_COLOR_YCBCR444_ITU_R_BT601:        *p_str = "YCbCr4:4:4 BT.601";    break;
        case SYSTEM_COLOR_YCBCR420_ITU_R_BT709:        *p_str = "YCbCr4:2:0 BT.709";    break;
        case SYSTEM_COLOR_YCBCR422_ITU_R_BT709:        *p_str = "YCbCr4:2:2 BT.709";    break;
        case SYSTEM_COLOR_YCBCR444_ITU_R_BT709:        *p_str = "YCbCr4:4:4 BT.709";    break;
        case SYSTEM_COLOR_XVYCC422_ITU_R_BT601:        *p_str = "xvYCC4:2:2 BT.601";    break;
        case SYSTEM_COLOR_XVYCC444_ITU_R_BT601:        *p_str = "xvYCC4:4:4 BT.601";    break;
        case SYSTEM_COLOR_XVYCC422_ITU_R_BT709:        *p_str = "xvYCC4:2:2 BT.709";    break;
        case SYSTEM_COLOR_XVYCC444_ITU_R_BT709:        *p_str = "xvYCC4:4:4 BT.709";    break;
        case SYSTEM_COLOR_RGB_FULL_RANGE1:            *p_str = "RGB Full Range1";        break;
        case SYSTEM_COLOR_RGB_FULL_RANGE2:            *p_str = "RGB Full Range2";        break;
        case SYSTEM_COLOR_RGB_LIMITED_RANGE:        *p_str = "RGB Limited Range";    break;
        case SYSTEM_COLOR_YCBCR420_ITU_R_BT2020:    *p_str = "YCbCr4:2:0 BT.2020";    break;
        case SYSTEM_COLOR_YCBCR422_ITU_R_BT2020:    *p_str = "YCbCr4:2:2 BT.2020";    break;
        case SYSTEM_COLOR_YCBCR444_ITU_R_BT2020:    *p_str = "YCbCr4:4:4 BT.2020";    break;
        case SYSTEM_COLOR_RGB_ITU_R_BT2020:            *p_str = "RGB BT.2020";            break;
        default:                                    sprintf(ret, "unknown(%02X)", cform);
                                                    *p_str = ret;            break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_deep_color(opt_deep_color dc, system_deep_color *p_dcolor)
{
    switch (dc) {
        case OPT_DEEP_COLOR_8:        *p_dcolor = SYSTEM_DEEP_COLOR_8BIT;        break;
        case OPT_DEEP_COLOR_10:        *p_dcolor = SYSTEM_DEEP_COLOR_10BIT;    break;
        case OPT_DEEP_COLOR_12:        *p_dcolor = SYSTEM_DEEP_COLOR_12BIT;    break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_deep_color_str(system_deep_color dcolor, char **p_str)
{
    static char ret[16];

    switch (dcolor) {
        case SYSTEM_DEEP_COLOR_8BIT:    *p_str = " 8bit";                    break;
        case SYSTEM_DEEP_COLOR_10BIT:    *p_str = "10bit";                    break;
        case SYSTEM_DEEP_COLOR_12BIT:    *p_str = "12bit";                    break;
        default:                        sprintf(ret, "unknown(%02X)", dcolor);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t print_video_format(system_video_format vform, system_color_format cform, system_deep_color dcolor)
{
    char *vform_str = NULL, *cform_str = NULL, *dcolor_str = NULL;

    get_video_format_str(vform, &vform_str);
    get_color_format_str(cform, &cform_str);
    get_deep_color_str(dcolor, &dcolor_str);
    MES("LF9129  %s %s %s", vform_str, cform_str, dcolor_str);

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_cc(opt_cc cc, system_audio_cc *p_cc)
{
    switch (cc) {
        case OPT_CC_RSH:        *p_cc = SYSTEM_AUDIO_CC_RSH;                break;
        case OPT_CC_2CH:        *p_cc = SYSTEM_AUDIO_CC_2CH;                break;
        case OPT_CC_3CH:        *p_cc = SYSTEM_AUDIO_CC_3CH;                break;
        case OPT_CC_4CH:        *p_cc = SYSTEM_AUDIO_CC_4CH;                break;
        case OPT_CC_5CH:        *p_cc = SYSTEM_AUDIO_CC_5CH;                break;
        case OPT_CC_6CH:        *p_cc = SYSTEM_AUDIO_CC_6CH;                break;
        case OPT_CC_7CH:        *p_cc = SYSTEM_AUDIO_CC_7CH;                break;
        case OPT_CC_8CH:        *p_cc = SYSTEM_AUDIO_CC_8CH;                break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_sf(opt_freq freq, system_audio_sf *p_sf)
{
    switch (freq) {
        case OPT_FREQ_RSH:        *p_sf = SYSTEM_AUDIO_SF_RSH;                break;
        case OPT_FREQ_32:        *p_sf = SYSTEM_AUDIO_SF_32KHZ;                break;
        case OPT_FREQ_44_1:        *p_sf = SYSTEM_AUDIO_SF_44KHZ;                break;
        case OPT_FREQ_48:        *p_sf = SYSTEM_AUDIO_SF_48KHZ;                break;
        case OPT_FREQ_88_2:        *p_sf = SYSTEM_AUDIO_SF_88KHZ;                break;
        case OPT_FREQ_96:        *p_sf = SYSTEM_AUDIO_SF_96KHZ;                break;
        case OPT_FREQ_176_4:    *p_sf = SYSTEM_AUDIO_SF_176KHZ;                break;
        case OPT_FREQ_192:        *p_sf = SYSTEM_AUDIO_SF_192KHZ;                break;
        case OPT_FREQ_64:        *p_sf = SYSTEM_AUDIO_SF_64KHZ;                break;
        case OPT_FREQ_128:        *p_sf = SYSTEM_AUDIO_SF_128KHZ;                break;
        case OPT_FREQ_256:        *p_sf = SYSTEM_AUDIO_SF_256KHZ;                break;
        case OPT_FREQ_352_8:    *p_sf = SYSTEM_AUDIO_SF_352KHZ;                break;
        case OPT_FREQ_384:        *p_sf = SYSTEM_AUDIO_SF_384KHZ;                break;
        case OPT_FREQ_512:        *p_sf = SYSTEM_AUDIO_SF_512KHZ;                break;
        case OPT_FREQ_705_6:    *p_sf = SYSTEM_AUDIO_SF_705KHZ;                break;
        case OPT_FREQ_768:        *p_sf = SYSTEM_AUDIO_SF_768KHZ;                break;
        case OPT_FREQ_1024:        *p_sf = SYSTEM_AUDIO_SF_1024KHZ;            break;
        case OPT_FREQ_1411_2:    *p_sf = SYSTEM_AUDIO_SF_1411KHZ;            break;
        case OPT_FREQ_1536:        *p_sf = SYSTEM_AUDIO_SF_1536KHZ;            break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_ss(opt_ss ss, system_audio_ss *p_ss)
{
    switch (ss) {
        case OPT_SS_RSH:        *p_ss = SYSTEM_AUDIO_SS_RSH;                break;
        case OPT_SS_16BIT:        *p_ss = SYSTEM_AUDIO_SS_16BIT;                break;
        case OPT_SS_20BIT:        *p_ss = SYSTEM_AUDIO_SS_20BIT;                break;
        case OPT_SS_24BIT:        *p_ss = SYSTEM_AUDIO_SS_24BIT;                break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_sf_ratio(opt_freq_ratio freq_ratio, system_audio_sf_ratio *p_sfr)
{
    switch (freq_ratio) {
        case OPT_FREQ_RATIO_1_4:    *p_sfr = SYSTEM_AUDIO_SF_RATIO_1_4;        break;
        case OPT_FREQ_RATIO_1_2:    *p_sfr = SYSTEM_AUDIO_SF_RATIO_1_2;        break;
        case OPT_FREQ_RATIO_1_1:    *p_sfr = SYSTEM_AUDIO_SF_RATIO_1_1;        break;
        case OPT_FREQ_RATIO_2_1:    *p_sfr = SYSTEM_AUDIO_SF_RATIO_2_1;        break;
        case OPT_FREQ_RATIO_4_1:    *p_sfr = SYSTEM_AUDIO_SF_RATIO_4_1;        break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_em(opt_em em, system_audio_em *p_em)
{
    switch (em) {
        case OPT_EM_MSB:        *p_em = SYSTEM_AUDIO_EM_MSB;                break;
        case OPT_EM_LSB:        *p_em = SYSTEM_AUDIO_EM_LSB;                break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_po(opt_po po, system_audio_po *p_po)
{
    switch (po) {
        case OPT_PO_LOW:        *p_po = SYSTEM_AUDIO_PO_LOW;                break;
        case OPT_PO_HIGH:        *p_po = SYSTEM_AUDIO_PO_HIGH;                break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_if(opt_if i2sf, system_audio_if *p_if)
{
    switch (i2sf) {
        case OPT_IF_DISABLE:    *p_if = SYSTEM_AUDIO_IF_DISABLE;            break;
        case OPT_IF_ENABLE:        *p_if = SYSTEM_AUDIO_IF_ENABLE;                break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_sp(opt_sp sp, system_audio_sp *p_sp)
{
    switch (sp) {
        case OPT_SP_LEFT:        *p_sp = SYSTEM_AUDIO_SP_LEFT;                break;
        case OPT_SP_RIGHT:        *p_sp = SYSTEM_AUDIO_SP_RIGHT;                break;
        default:                                                            return ERR_NG;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_cc_str(system_audio_cc cc, char **p_str)
{
    static char ret[16];

    switch (cc) {
        case SYSTEM_AUDIO_CC_RSH:        *p_str = "Refer to Stream Header";    break;
        case SYSTEM_AUDIO_CC_2CH:        *p_str = "2CH";                        break;
        case SYSTEM_AUDIO_CC_3CH:        *p_str = "3CH";                        break;
        case SYSTEM_AUDIO_CC_4CH:        *p_str = "4CH";                        break;
        case SYSTEM_AUDIO_CC_5CH:        *p_str = "5CH";                        break;
        case SYSTEM_AUDIO_CC_6CH:        *p_str = "6CH";                        break;
        case SYSTEM_AUDIO_CC_7CH:        *p_str = "7CH";                        break;
        case SYSTEM_AUDIO_CC_8CH:        *p_str = "8CH";                        break;
        default:                        sprintf(ret, "unknown(%02X)", cc);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_sf_str(system_audio_sf sf, char **p_str)
{
    static char ret[16];

    switch (sf) {
        case SYSTEM_AUDIO_SF_RSH:        *p_str = "Refer to Stream Header";    break;
        case SYSTEM_AUDIO_SF_32KHZ:        *p_str = "32 kHz";                    break;
        case SYSTEM_AUDIO_SF_44KHZ:        *p_str = "44.1 kHz";                break;
        case SYSTEM_AUDIO_SF_48KHZ:        *p_str = "48 kHz";                    break;
        case SYSTEM_AUDIO_SF_88KHZ:        *p_str = "88.2 kHz";                break;
        case SYSTEM_AUDIO_SF_96KHZ:        *p_str = "96 kHz";                    break;
        case SYSTEM_AUDIO_SF_176KHZ:    *p_str = "176.4 kHz";                break;
        case SYSTEM_AUDIO_SF_192KHZ:    *p_str = "192 kHz";                    break;
        case SYSTEM_AUDIO_SF_64KHZ:        *p_str = "64 kHz";                    break;
        case SYSTEM_AUDIO_SF_128KHZ:    *p_str = "128 kHz";                    break;
        case SYSTEM_AUDIO_SF_256KHZ:    *p_str = "256 kHz";                    break;
        case SYSTEM_AUDIO_SF_352KHZ:    *p_str = "352.8 kHz";                break;
        case SYSTEM_AUDIO_SF_384KHZ:    *p_str = "384 kHz";                    break;
        case SYSTEM_AUDIO_SF_512KHZ:    *p_str = "512 kHz";                    break;
        case SYSTEM_AUDIO_SF_705KHZ:    *p_str = "705.6 kHz";                break;
        case SYSTEM_AUDIO_SF_768KHZ:    *p_str = "768kHz";                    break;
        case SYSTEM_AUDIO_SF_1024KHZ:    *p_str = "1024 kHz";                break;
        case SYSTEM_AUDIO_SF_1411KHZ:    *p_str = "1411.2 kHz";                break;
        case SYSTEM_AUDIO_SF_1536KHZ:    *p_str = "1536 kHz";                break;
        default:                        sprintf(ret, "unknown(%02X)", sf);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_ss_str(system_audio_ss ss, char **p_str)
{
    static char ret[16];

    switch (ss) {
        case SYSTEM_AUDIO_SS_RSH:        *p_str = "Refer to Stream Header";        break;
        case SYSTEM_AUDIO_SS_16BIT:        *p_str = "16 bit";                        break;
        case SYSTEM_AUDIO_SS_20BIT:
        case SYSTEM_AUDIO_SS_20BIT_0:
        case SYSTEM_AUDIO_SS_20BIT_1:
                                        *p_str = "20 bit";                        break;
        case SYSTEM_AUDIO_SS_24BIT:        *p_str = "24 bit";                        break;
        case SYSTEM_AUDIO_MAX_20BIT:    *p_str = "MAX 20 bit (NON-STANDARD)";    break;
        case SYSTEM_AUDIO_MAX_24BIT:    *p_str = "MAX 24 bit (NON-STANDARD)";    break;
        default:                        sprintf(ret, "unknown(%02X)", ss);
                                        *p_str = ret;                            break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_sf_ratio_str(system_audio_sf_ratio sfr, char **p_str)
{
    static char ret[16];

    switch (sfr) {
        case SYSTEM_AUDIO_SF_RATIO_1_4:    *p_str = "1/4 down sampling";        break;
        case SYSTEM_AUDIO_SF_RATIO_1_2:    *p_str = "1/2 down sampling";        break;
        case SYSTEM_AUDIO_SF_RATIO_1_1:    *p_str = "x1 normal sampling";        break;
        case SYSTEM_AUDIO_SF_RATIO_2_1:    *p_str = "x2 over sampling";        break;
        case SYSTEM_AUDIO_SF_RATIO_4_1:    *p_str = "x4 over sampling";        break;
        default:                        sprintf(ret, "unknown(%02X)", sfr);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_em_str(system_audio_em em, char **p_str)
{
    static char ret[16];

    switch (em) {
        case SYSTEM_AUDIO_EM_MSB:        *p_str = "MSB first";                break;
        case SYSTEM_AUDIO_EM_LSB:        *p_str = "LSB first";                break;
        default:                        sprintf(ret, "unknown(%02X)", em);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_po_str(system_audio_po po, char **p_str)
{
    static char ret[16];

    switch (po) {
        case SYSTEM_AUDIO_PO_LOW:        *p_str = "L-ch=Low";                break;
        case SYSTEM_AUDIO_PO_HIGH:        *p_str = "L-ch=High";                break;
        default:                        sprintf(ret, "unknown(%02X)", po);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_if_str(system_audio_if i2sf, char **p_str)
{
    static char ret[16];

    switch (i2sf) {
        case SYSTEM_AUDIO_IF_DISABLE:    *p_str = "disable";                    break;
        case SYSTEM_AUDIO_IF_ENABLE:    *p_str = "enable";                    break;
        default:                        sprintf(ret, "unknown(%02X)", i2sf);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_audio_sp_str(system_audio_sp sp, char **p_str)
{
    static char ret[16];

    switch (sp) {
        case SYSTEM_AUDIO_SP_RIGHT:        *p_str = "Right justified";            break;
        case SYSTEM_AUDIO_SP_LEFT:        *p_str = "Left justified";            break;
        default:                        sprintf(ret, "unknown(%02X)", sp);
                                        *p_str = ret;                        break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t print_audio_format(system_audio *p_aform)
{
    char *if_cc, *if_sf, *if_ss;
    char *cb_sf, *cb_ss;
    char *sf_ratio;
    char *i2s_em, *i2s_po, *i2s_if, *i2s_sp;

    get_audio_cc_str(p_aform->if_channel_count, &if_cc);
    get_audio_sf_str(p_aform->if_sampling_freq, &if_sf);
    get_audio_ss_str(p_aform->if_sample_size, &if_ss);
    // if_channel_number
    // if_level_shift_value
    get_audio_sf_str(p_aform->cb_sampling_freq, &cb_sf);
    get_audio_ss_str(p_aform->cb_sample_size, &cb_ss);
    get_audio_sf_ratio_str(p_aform->sampling_freq_ratio, &sf_ratio);
    get_audio_em_str(p_aform->i2s.endian_mode, &i2s_em);
    get_audio_po_str(p_aform->i2s.polarity, &i2s_po);
    get_audio_if_str(p_aform->i2s.i2s_format, &i2s_if);
    get_audio_sp_str(p_aform->i2s.start_pos, &i2s_sp);

    MES("  if_channel_count:     %s", if_cc);
    MES("  if_sampling_freq:     %s", if_sf);
    MES("  if_sample_size:       %s", if_ss);
    MES("  if_channel_number:    0x%02X", p_aform->if_channel_number);
    MES("  if_level_shift_value: 0x%1X", p_aform->if_level_shift_value);
    MES("  cb_sampling_freq:     %s", cb_sf);
    MES("  cb_sample_size:       %s", cb_ss);
    MES("  sampling_freq_ratio:  %s", sf_ratio);
    MES("  i2s.endian_mode:      %s", i2s_em);
    MES("  i2s.polarity:         %s", i2s_po);
    MES("  i2s.i2s_format:       %s", i2s_if);
    MES("  i2s.start_pos:        %s", i2s_sp);

    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t get_init_vbo(opt_init_opt init_opt, system_inversion *p_inv)
{
    switch (init_opt) {
        case OPT_INIT_OPT_INVERSION_ON:        *p_inv = SYSTEM_INVERSION_ON;    break;
        case OPT_INIT_OPT_INVERSION_OFF:
        default:
                                            *p_inv = SYSTEM_INVERSION_OFF;    break;
    }

    return ERR_OK;
}
//------------------------------------------------------------------------------
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H29BJ2)
static uint32_t get_init_lvds(opt_init_opt init_opt, lvdstx_data_format *p_df)
{
    switch (init_opt) {
        case OPT_INIT_OPT_VESA08:        *p_df = LVDSTX_DATA_FORMAT_VESA_8BIT;                break;
        case OPT_INIT_OPT_VESA10:        *p_df = LVDSTX_DATA_FORMAT_VESA_10BIT;                break;
        case OPT_INIT_OPT_JEIDA08:        *p_df = LVDSTX_DATA_FORMAT_JEIDA_8BIT;                break;
        case OPT_INIT_OPT_JEIDA10:        *p_df = LVDSTX_DATA_FORMAT_JEIDA_10BIT;                break;
        case OPT_INIT_OPT_COMPOSITE:    *p_df = LVDSTX_DATA_FORMAT_VESA_JEIDA_COMPOSITE;    break;
        default:                        *p_df = LVDSTX_DATA_FORMAT_VESA_8BIT;                break;
    }

    return ERR_OK;
}
#endif
#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) ||  defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H25BJ2) || defined(CONFIG_HV_SC1H26BJ2) || defined(CONFIG_HV_SC1H27BJ2)
//------------------------------------------------------------------------------
/*
static uint32_t read_edid_txt(const char *file, uint8_t *data, uint32_t *size)
{
    uint32_t ret=ERR_OK;
    FILE *fp;
    int32_t f_size;
    char c, pre_c, hex[2+1]={0};
    int32_t ti, di;

    fp = fopen(file, "rt");
    if (fp == NULL) {
        return ERR_NG;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        ret = ERR_NG;    goto _end;
    }
    f_size = ftell(fp);
    if (f_size == -1) {
        ret = ERR_NG;    goto _end;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        ret = ERR_NG;    goto _end;
    }
    // read
    ti=0;
    di=0;
    pre_c='\0';
    while ((c = fgetc(fp)) != EOF) {
        // if "0x" exists, reset
        if (pre_c == '0' && (c == 'x' || c == 'X')) {
            ti=0;
            continue;
        }
        // to lower case
        if (c >= 'A' && c <= 'F') {
            c += 0x20;
        }
        // to binary
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')) {
            hex[ti] = c;
            ti++;
            if(ti == 2){
                ti=0;
                data[di] = (uint8_t)strtoul(hex, NULL, 16);
                di++;
            }
        }
        pre_c = c;
    }
    *size = di;

_end:
    fclose(fp);

    return ret;
}*/
#endif

#if (1) || defined(CONFIG_HV_SC1H22BJ2) || defined(CONFIG_HV_SC1H23BJ2) || defined(CONFIG_HV_SC1H24BJ2) || defined(CONFIG_HV_SC1H27BJ2)
//------------------------------------------------------------------------------
static uint32_t print_scdc(hdmitx_scdc *scdc)
{
    char *name;
    char bin[8+1];

    switch (scdc->oaddr) {
        case 0x01:    name = "[r ] Sink Version";                            break;
        case 0x02:    name = "[rw] Source Version";                        break;
        case 0x10:    name = "[rw] Update_0";                                break;
        case 0x11:    name = "[rw] Update_1";                                break;
        case 0x20:    name = "[rw] TMDS_Config";                            break;
        case 0x21:    name = "[r ] Scrambler_Status";                        break;
        case 0x30:    name = "[rw] Config_0";                                break;
        case 0x40:    name = "[r ] Status_Flags_0";                        break;
        case 0x41:    name = "[r ] Status_Flags_1";                        break;
        case 0x50:    name = "[r ] Err_Det_0_L";                            break;
        case 0x51:    name = "[r ] Err_Det_0_H";                            break;
        case 0x52:    name = "[r ] Err_Det_1_L";                            break;
        case 0x53:    name = "[r ] Err_Det_1_H";                            break;
        case 0x54:    name = "[r ] Err_Det_2_L";                            break;
        case 0x55:    name = "[r ] Err_Det_2_H";                            break;
        case 0x56:    name = "[r ] Err_Det_Checksum";                        break;
        case 0xC0:    name = "[rw] Test_Config_0";                        break;
        case 0xD0:    name = "[r ] Manufacturer IEEE OUI, Third Octet";    break;
        case 0xD1:    name = "[r ] Manufacturer IEEE OUI, Second Octet";    break;
        case 0xD2:    name = "[r ] Manufacturer IEEE OUI, First Octet";    break;
        default:
            if (scdc->oaddr >= 0x12 && scdc->oaddr <= 0x1F)
                name = "Reserved for Update Related Uses";
            else if (scdc->oaddr >= 0x31 && scdc->oaddr <= 0x3F)
                name = "Reserved for Configuration";
            else if (scdc->oaddr >= 0x42 && scdc->oaddr <= 0x4F)
                name = "Reserved for Status Related Uses";
            else if (scdc->oaddr >= 0xC1 && scdc->oaddr <= 0xCF)
                name = "Reserved for test features";
            else if (scdc->oaddr >= 0xD3 && scdc->oaddr <= 0xDD)
                name = "Device ID";
            else if (scdc->oaddr >= 0xDE)
                name = "ManufacturerSpecific";
            else
                name = "Reserved";
            break;
    }
    MES("  %s: ", name);
    MES("[%02X:%02X] 0x%02X : BIN:%s", scdc->saddr,scdc->oaddr,scdc->data[0],dec2bin(scdc->data[0], bin));

    return ERR_OK;
}
#endif
//------------------------------------------------------------------------------
// I2C free access
//------------------------------------------------------------------------------
static uint32_t console_i2c(void)
{
    cmds_i2c cmd;

    MES("[I2C free access]");

    while(1) {
        /*PROMPT();
        INPUT(cmd.str, MAX_LINE);
        opts_i2c_parsing(&cmd);
        switch (cmd.id) {
            case CMD_I2C_ID_NONE:        if (strlen(cmd.str)) // MES("%s", cmd.str);    break;
            case CMD_I2C_ID_UNKNOWN:    opts_i2c_usage(NULL);                        break;
            case CMD_I2C_ID_I2CR:        cmd_i2cr(&cmd);                                break;
            case CMD_I2C_ID_I2CW:        cmd_i2cw(&cmd);                                break;
            case CMD_I2C_ID_I2CP:        cmd_i2cp(&cmd);                                break;
            case CMD_I2C_ID_SLEEP:        cmd_sleep(&cmd);                            break;
            case CMD_I2C_ID_WAIT:        cmd_wait(&cmd);                                break;
            case CMD_I2C_ID_HELP:        opts_i2c_usage(&cmd);                        break;
            // case CMD_I2C_ID_QUIT:        MES("quit");                                goto _END;
            default:                                                                break;
        }*/
    }

_END:
    return ERR_OK;
}
//------------------------------------------------------------------------------
static uint32_t cmd_i2cr(cmds_i2c *cmd)
{
    uint32_t ret;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2cr(cmd);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret) {
        ERR(API_ERROR_OCCURRED, "i2cr", hv_strerr(ret));
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_i2cw(cmds_i2c *cmd)
{
    uint32_t ret;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2cw(cmd);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret) {
        ERR(API_ERROR_OCCURRED, "i2cw", hv_strerr(ret));
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_i2cp(cmds_i2c *cmd)
{
    uint32_t ret;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2cp(cmd);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret) {
        ERR(API_ERROR_OCCURRED, "i2cp", hv_strerr(ret));
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_sleep(cmds_i2c *cmd)
{
    uint32_t ret;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2c_sleep(cmd);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret) {
        ERR(API_ERROR_OCCURRED, "sleep", hv_strerr(ret));
    }

    return ret;
}
//------------------------------------------------------------------------------
static uint32_t cmd_wait(cmds_i2c *cmd)
{
    uint32_t ret;

    MUTEX_LOCK(THREAD_MTX);
    ret = i2c_wait(cmd);
    MUTEX_UNLOCK(THREAD_MTX);
    if (ret) {
        ERR(API_ERROR_OCCURRED, "wait", hv_strerr(ret));
    }

    return ret;
}
//------------------------------------------------------------------------------
