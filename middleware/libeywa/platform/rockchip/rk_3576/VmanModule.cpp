#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@VmanModule"

#include "VmanModule.h"
#include <hardware/hardware.h>
#include "XbhLog.h"
#include <hardware/board.h>

VmanModule* VmanModule::INSTANCE = nullptr;
XbhMutex VmanModule::mLock;



#ifndef ENABLE_DISPLAY_CONNECT_STATE_CHEACK
#define ENABLE_DISPLAY_CONNECT_STATE_CHEACK 0
#endif

#ifndef DISPLAY_CONNECT_SUCCESS
#define DISPLAY_CONNECT_SUCCESS 1
#endif

#ifndef DISPLAY_CONNECT_FAIL
#define DISPLAY_CONNECT_FAIL 2
#endif

VmanModule::VmanModule()
{
    initDisplayHal();
    initPowerHal();
    initPqHal();
    initSysHal();
}

VmanModule::~VmanModule()
{
    delete display_hal;
    delete power_hal;
    delete pq_hal;
    delete sys_hal;
}

VmanModule* VmanModule::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!INSTANCE)
    {
        INSTANCE = new VmanModule();
    }
    
    return INSTANCE;
}

void VmanModule::initDisplayHal()
{
    const hw_module_t* hwModule = NULL;

    int ret = hw_get_module(DISPLAY_HAL_HARDWARE_MODULE_ID, &hwModule);

    if (ret == 0) {
        display_hal = (display_hal_module *) hwModule;
        XLOGI("Display hal load success.");
    } else {
        XLOGI("Display hal load failed.");
    }
}

void VmanModule::initPowerHal()
{
    const hw_module_t* hwModule = NULL;

    int ret = hw_get_module(POWER_HAL_HARDWARE_MODULE_ID, &hwModule);

    if (ret == 0) {
        power_hal = (power_hal_module *) hwModule;
        XLOGI("Power hal load success.");
    } else {
        XLOGI("Power hal load failed.");
    }

}

void VmanModule::initPqHal()
{
    const hw_module_t* hwModule = NULL;

    int ret = hw_get_module(PQ_HAL_HARDWARE_MODULE_ID, &hwModule);
    
    if (ret == 0) {
        pq_hal = (pq_hal_module *) hwModule;
        pq_hal->init(pq_hal);
        XLOGI("Pq hal load success.");
    } else {
        XLOGI("Pq hal load failed.");
    }
}

void VmanModule::initSysHal()
{
    const hw_module_t* hwModule = NULL;

    int ret = hw_get_module(SYS_HAL_HARDWARE_MODULE_ID, &hwModule);

    if (ret == 0) {
        sys_hal = (sys_hal_module *) hwModule;
        sys_hal->sysmodule_init(HDMIIN_TYPE_HDMIRX);
        XLOGI("Sys hal load success.");
    } else {
        XLOGI("Sys hal load failed.");
    }
}

int VmanModule::check_display_connector_state()
{
#if ENABLE_DISPLAY_CONNECT_STATE_CHEACK
    int status = get_display_connector_state();
    return status;
#endif
    return DISPLAY_CONNECT_SUCCESS; 
}

int VmanModule::get_display_enable()
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->get_display_enable(display_hal);
}

int VmanModule::set_display_enable(int value)
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->set_display_enable(display_hal, value);
}

int VmanModule::get_display_connector_state()
{
#if ENABLE_DISPLAY_CONNECT_STATE_CHEACK
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->get_display_connector_state(display_hal);
#else
    return -1;
#endif
}

int VmanModule::get_display_hdmi_enable()
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->get_display_hdmi_enable(display_hal);
}

int VmanModule::set_display_hdmi_enable(int value)
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->set_display_hdmi_enable(display_hal, value);
}

char** VmanModule::get_display_support_resolution_list()
{
    std::vector<std::string> vec;
    int ret = 0;
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return nullptr;
    }
    ret = display_hal->get_display_support_resolution_list(display_hal, vec);
    if (ret < 0)
    {
        XLOGW("get_display_support_resolution_list fail");
        return nullptr;
    }

    char **resolutionList = new char*[vec.size()];

    for (size_t i = 0; i < vec.size(); ++i) {
        resolutionList[i] = new char[vec[i].size() + 1];
        std::strcpy(resolutionList[i], vec[i].c_str());
    }

    return resolutionList;
}

const char* VmanModule::get_display_resolution()
{
    std::string ret;
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return nullptr;
    }

    ret = display_hal->get_display_resolution(display_hal);

    if (ret == "")
    {
        XLOGW("get_display_resolution fail");
        return nullptr;
    }

    return ret.c_str();
}

int VmanModule::set_display_resolution(const char* resolution)
{
    std::string value(resolution);
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->set_display_resolution(display_hal, value);
}

int VmanModule::get_display_hdcp_status()
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->get_display_hdcp_status(display_hal);
}

int VmanModule::set_display_hdcp_status(int dpy, int value)
{
    int ret;
    int old_dpy = display_hal->dpy;
    
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    display_hal->dpy = dpy;

    ret = display_hal->set_display_hdcp_status(display_hal, value);

    display_hal->dpy = old_dpy;

    return ret;
}

int VmanModule::get_extend_display_en_dvi_status()
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->get_extend_display_en_dvi_status(display_hal);
}

int VmanModule::set_extend_display_en_dvi_status(int value)
{
    if (display_hal == nullptr)
    {
        XLOGW("display_hal not init");
        return -1;
    }

    return display_hal->set_extend_display_en_dvi_status(display_hal, value);
}
// display end
// power start
int VmanModule::get_power_mode()
{
    if (power_hal == nullptr)
    {
        XLOGW("power_hal not init");
        return -1;
    }

    return power_hal->get_power_mode(power_hal);
}

/**
 * value :
 * 0 - Standby mode: Standby after power-on
 * 1 - power-on mode: System starts after power-on
 * 2 - last mode
 */
int VmanModule::set_power_mode(int value)
{
    if (power_hal == nullptr)
    {
        XLOGW("power_hal not init");
        return -1;
    }

    return power_hal->set_power_mode(power_hal, ui_power_mode_t(value));
}

// range[0-100]
int VmanModule::get_brightness()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_brightness(pq_hal);
}

// range[0-100]
int VmanModule::set_brightness(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_brightness(pq_hal, value);
}

// range[0-100]
int VmanModule::get_contrast()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_contrast(pq_hal);
}

// range[0-100]
int VmanModule::set_contrast(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_contrast(pq_hal, value);
}

// range[0-100]
int VmanModule::get_saturation()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_saturation(pq_hal);
}

// range[0-100]
int VmanModule::set_saturation(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_saturation(pq_hal, value);
}

// range[0-100]
int VmanModule::set_sharpness(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_sharpness(pq_hal, value);
}

// range[0-100]
int VmanModule::get_sharpness()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_sharpness(pq_hal);
}

// range[0-100]
int VmanModule::get_hue()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_hue(pq_hal);
}

// range[0-100]
int VmanModule::set_hue(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_hue(pq_hal, value);
}

int VmanModule::set_preset_color_temp_mode(int path, int index, uint32_t rgain, uint32_t ggain, uint32_t bgain)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_preset_color_temp_mode(pq_hal, path, index, rgain, ggain, bgain);
}

int VmanModule::get_preset_color_temp_mode(int path,int index, uint32_t* rgain, uint32_t* ggain, uint32_t* bgain)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_preset_color_temp_mode(pq_hal, path, index, rgain, ggain, bgain);
}

int VmanModule::set_color_temp_mode(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_color_temp_mode(pq_hal, ui_color_temp_t(value));
}

int VmanModule::get_color_temp_mode()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_color_temp_mode(pq_hal);
}

int VmanModule::reset_color_temp()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->reset_color_temp(pq_hal);
}

int VmanModule::get_wb_r_gain()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_wb_r_gain(pq_hal);
}

int VmanModule::set_wb_r_gain(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_wb_r_gain(pq_hal, value);
}

int VmanModule::get_wb_g_gain()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_wb_g_gain(pq_hal);
}

int VmanModule::set_wb_g_gain(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_wb_g_gain(pq_hal, value);
}

int VmanModule::get_wb_b_gain()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_wb_b_gain(pq_hal);
}

int VmanModule::set_wb_b_gain(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_wb_b_gain(pq_hal, value);
}

int VmanModule::get_wb_r_offset()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_wb_r_offset(pq_hal);
}

int VmanModule::set_wb_r_offset(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_wb_r_offset(pq_hal, value);
}

int VmanModule::get_wb_g_offset()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_wb_g_offset(pq_hal);
}

int VmanModule::set_wb_g_offset(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_wb_g_offset(pq_hal, value);
}

int VmanModule::get_wb_b_offset()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_wb_b_offset(pq_hal);
}

int VmanModule::set_wb_b_offset(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_wb_b_offset(pq_hal, value);
}

int VmanModule::set_csc_enable(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_csc_enable(pq_hal, value);
}

int VmanModule::get_csc_enable()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_csc_enable(pq_hal);
}

int VmanModule::set_dci_enable(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_dci_enable(pq_hal, value);
}

int VmanModule::get_dci_enable()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_dci_enable(pq_hal);
}

int VmanModule::set_acm_enable(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_acm_enable(pq_hal, value);
}

int VmanModule::get_acm_enable()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_acm_enable(pq_hal);
}

int VmanModule::set_sharp_enable(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_sharp_enable(pq_hal, value);
}

int VmanModule::get_sharp_enable()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_sharp_enable(pq_hal);
}

int VmanModule::set_pq_enable(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_pq_enable(pq_hal, value);
}

int VmanModule::get_pq_enable()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_pq_enable(pq_hal);
}

int VmanModule::set_hdr_enable(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_hdr_enable(pq_hal, value);
}

int VmanModule::get_hdr_enable()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_hdr_enable(pq_hal);
}

int VmanModule::set_picture_mode(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_picture_mode(pq_hal, ui_picture_mode_t(value));
}

int VmanModule::get_picture_mode() {
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_picture_mode(pq_hal);
}

int VmanModule::set_gamma_mode(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_gamma_mode(pq_hal, value);
}

int VmanModule::get_gamma_mode()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_gamma_mode(pq_hal);
}

int VmanModule::set_3dlut_mode(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_3dlut_mode(pq_hal, value);
}

int VmanModule::get_3dlut_mode()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_3dlut_mode(pq_hal);
}

int VmanModule::set_3dlut_data_buff(int index, char *data)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_3dlut_data_buff(pq_hal, index, data);
}

int VmanModule::get_3dlut_data_buff(int index, char *data)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_3dlut_data_buff(pq_hal, index, data);
}

int VmanModule::reset_3dlut_data()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->reset_3dlut_data(pq_hal);
}

int VmanModule::get_color_range()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_color_range(pq_hal);
}

int VmanModule::set_color_range(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_color_range(pq_hal, ui_color_range_t(value));
}

int VmanModule::get_backlight()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_backlight(pq_hal);
}

int VmanModule::set_backlight(int value)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_backlight(pq_hal, value);
}

int VmanModule::set_acm_mode(int mode)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_acm_mode(pq_hal, mode);
}

int VmanModule::get_acm_mode()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_acm_mode(pq_hal);
}

int VmanModule::set_dci_mode(int mode)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_dci_mode(pq_hal, mode);
}

int VmanModule::get_dci_mode()
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_dci_mode(pq_hal);
}

int VmanModule::set_preset_picture_mode(int  path, int index, int brightness, int contrast, int saturation, int hue)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_preset_picture_mode(pq_hal, path, index, brightness, contrast, saturation, hue);
}

int VmanModule::get_preset_picture_mode(int  path, int index, int *brightness, int *contrast, int *saturation, int *hue)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_preset_picture_mode(pq_hal, path, index, brightness, contrast, saturation, hue);
}

int VmanModule::set_preset_gamma_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_preset_gamma_mode(pq_hal, path, index, size, r, g, b);
}

int VmanModule::get_preset_gamma_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_preset_gamma_mode(pq_hal, path, index, size, r, g, b);
}

int VmanModule::set_preset_3dlut_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->set_preset_3dlut_mode(pq_hal, path, index, size, r, g, b);
}

int VmanModule::get_preset_3dlut_mode(int path, int index, uint32_t size, uint16_t* r, uint16_t* g, uint16_t* b)
{
    int state = check_display_connector_state();
    if (pq_hal == nullptr || state == DISPLAY_CONNECT_FAIL )
    {
        XLOGW("pq_hal not init");
        return -1;
    }

    return pq_hal->get_preset_3dlut_mode(pq_hal, path, index, size, r, g, b);
}

// for pq end
// for sys start
int VmanModule::get_input_stream_config(xbh_ui_stream_config_t* data)
{
    ui_stream_config_t* value = reinterpret_cast<ui_stream_config_t*>(data);;
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_input_stream_config(sys_hal, value);
}

int VmanModule::get_cur_signal_status()
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_cur_signal_status(sys_hal);
}

int VmanModule::get_cur_Source_Interlaced()
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_cur_Source_Interlaced(sys_hal);
}

int VmanModule::get_cur_source_dvi_mode()
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_cur_source_dvi_mode(sys_hal);
}

int VmanModule::get_cur_source_hdcp_encrypted()
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_cur_source_hdcp_encrypted(sys_hal);
}

int VmanModule::get_overscan(xbh_ui_overscan_data_t *data)
{
    ui_overscan_data_t value;
    int ret = 0;
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    ret = sys_hal->get_overscan(sys_hal, &value);

    if (ret < 0) {
        XLOGW("get_overscan fail");
        return -1;
    }

    data = reinterpret_cast<xbh_ui_overscan_data_t*>(&value);

    return ret;
}

int VmanModule::set_overscan(xbh_ui_overscan_data_t data)
{
    ui_overscan_data_t *value = reinterpret_cast<ui_overscan_data_t*>(&data);
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->set_overscan(sys_hal, *value);
}

int VmanModule::get_average_brightness()
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_average_brightness(sys_hal);
}

int VmanModule::get_hdmi_edid_mode()
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_hdmi_edid_mode(sys_hal);
}

/**
 *  0 - EDID_MODE_2K60HZ_YUV444,
	1 - EDID_MODE_4K30HZ_YUV444,
	2 - EDID_MODE_4K60HZ_YUV444,
	3 - EDID_MODE_4K60HZ_YUV420
 */
int VmanModule::set_hdmi_edid_mode(int value)
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->set_hdmi_edid_mode(sys_hal, ui_edid_mode_t(value));
}

int VmanModule::set_screen_pixel_shift(int x, int y)
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->set_screen_pixel_shift(sys_hal, x, y);
}

int VmanModule::get_screen_pixel_shift(int *x, int *y)
{
    if (sys_hal == nullptr)
    {
        XLOGW("sys_hal not init");
        return -1;
    }

    return sys_hal->get_screen_pixel_shift(sys_hal, x, y);
}
// sys end
