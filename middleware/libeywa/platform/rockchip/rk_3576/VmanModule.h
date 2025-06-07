

#pragma once

#include <DisplayModule.h>
#include <PowerModule.h>
#include <PqModule.h>
#include <SysModule.h>
#include "XbhType.h"
#include "XbhMutex.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

typedef struct XbhDataOverScan {
    int left;
    int top;
    int right;
    int bottom;
} xbh_ui_overscan_data_t;

typedef struct XbhStreamConfig {
    int width;
    int height;
    int interlace;
    int initFps;
    int Hfreq;
    int Vfreq;
} xbh_ui_stream_config_t;

class VmanModule
{
private:
    static VmanModule *INSTANCE;
    static XbhMutex mLock;
    struct display_hal_module *display_hal;
    struct power_hal_module *power_hal;
    struct pq_hal_module *pq_hal;
    struct sys_hal_module *sys_hal;
    void initDisplayHal();
    void initPowerHal();
    void initPqHal();
    void initSysHal();
    int check_display_connector_state();
    VmanModule();
    ~VmanModule();
public:
    static VmanModule* getInstance();
    // for display start
    int get_display_enable();
    int set_display_enable(int value);
    int get_display_hdmi_enable();
    int set_display_hdmi_enable(int value);
    char** get_display_support_resolution_list();
    const char* get_display_resolution();
    int set_display_resolution(const char* resolution);
    int get_display_hdcp_status();
    int set_display_hdcp_status(int dpy, int value);
    int get_extend_display_en_dvi_status();
    int set_extend_display_en_dvi_status(int value);
    // for display end
    // for power start
    int get_power_mode();
    int set_power_mode(int value);
    // for power end
    // for pq start
    int get_brightness();
    int set_brightness(int value);
    int get_contrast();
    int set_contrast(int value);
    int get_saturation();
    int set_saturation(int value);
    int set_sharpness(int value);
    int get_sharpness();
    int get_hue();
    int set_hue(int value);
    int set_preset_color_temp_mode(int path, int index, uint32_t rgain, uint32_t ggain, uint32_t bgain);
    int get_preset_color_temp_mode(int path,int index, uint32_t* rgain, uint32_t* ggain, uint32_t* bgain);
    int set_color_temp_mode(int value);
    int get_color_temp_mode();
    int reset_color_temp();
    int get_wb_r_gain();
    int set_wb_r_gain(int value);
    int get_wb_g_gain();
    int set_wb_g_gain(int value);
    int get_wb_b_gain();
    int set_wb_b_gain(int value);
    int get_wb_r_offset();
    int set_wb_r_offset(int value);
    int get_wb_g_offset();
    int set_wb_g_offset(int value);
    int get_wb_b_offset();
    int set_wb_b_offset(int value);
    int set_csc_enable(int value);
    int get_csc_enable();
    int set_dci_enable(int value);
    int get_dci_enable();
    int set_acm_enable(int value);
    int get_acm_enable();
    int set_sharp_enable(int value);
    int get_sharp_enable();
    int set_pq_enable(int value);
    int get_pq_enable();
    int set_hdr_enable(int value);
    int get_hdr_enable();
    int set_picture_mode(int value);
    int get_picture_mode();
    int set_gamma_mode(int value);
    int get_gamma_mode();
    int set_3dlut_mode(int value);
    int get_3dlut_mode();
    int set_3dlut_data_buff(int index, char *data);
    int get_3dlut_data_buff(int index, char *data);
    int reset_3dlut_data();
    int get_color_range();
    int set_color_range(int value);
    int get_backlight();
    int set_backlight(int value);
    int set_acm_mode(int mode);
    int get_acm_mode();
    int set_dci_mode(int mode);
    int get_dci_mode();
    int set_preset_picture_mode(int  path, int index, int brightness, int contrast, int saturation, int hue);
    int get_preset_picture_mode(int  path, int index, int *brightness, int *contrast, int *saturation, int *hue);
    int set_preset_gamma_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b);
    int get_preset_gamma_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b);
    int set_preset_3dlut_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b);
    int get_preset_3dlut_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b);
    // for pq end
    // for sys start
    int get_input_stream_config(xbh_ui_stream_config_t* data);
    int get_cur_signal_status();
    int get_cur_Source_Interlaced();
    int get_cur_source_dvi_mode();
    int get_cur_source_hdcp_encrypted();
    int get_overscan(xbh_ui_overscan_data_t *data);
    int set_overscan(xbh_ui_overscan_data_t data);
    int get_average_brightness();
    int set_hdmi_edid_mode(int value);
    int get_hdmi_edid_mode();
    int set_screen_pixel_shift(int x, int y);
    int get_screen_pixel_shift(int *x, int *y);
    int get_display_connector_state();
    // for sys end
};
