#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMR3576_D"

#include "XbhLog.h"
#include "XbhXMR3576_D.h"
#include "lcdparamservice.h"

XbhXMR3576_D            *XbhXMR3576_D::mInstance = XBH_NULL;
XbhMutex                 XbhXMR3576_D::mLock;
#define LED_STANDBY_STATUS        "/sys/class/leds/work/brightness"

struct gpio_standby_info {
    int pin;
    int init_level;
};

static struct gpio_standby_info gpio_standby_table[] = {
#ifdef XBH_BOARD_GPIO_BOOT_STANDBY_TABLE
    XBH_BOARD_GPIO_BOOT_STANDBY_TABLE
#endif
};

static int xbh_led_standby_status(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char ledBrightnessPath[256] = LED_STANDBY_STATUS;

    if (access(ledBrightnessPath, F_OK) == 0) {
        fd = open(ledBrightnessPath, O_RDWR);
        if (fd  < 0)
        {
            XLOGE("%s open led dev error \n", __func__);
            close(fd);
            return s32Ret;
        }
        s32Ret = write(fd, "0", 1);
        if (s32Ret < 0) {
            ALOGE("Error writing to %s\n", ledBrightnessPath);
            close(fd);
            return s32Ret;
        }
        close(fd);
    }
   return s32Ret;

}

static void xbh_set_gpio_init(void)
{


}

XbhXMR3576_D::XbhXMR3576_D()
{
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMR3576_D::~XbhXMR3576_D()
{
}

XBH_S32 XbhXMR3576_D::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_D::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_2_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_D::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_D::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_USBC_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_D::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_D::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_D::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_D::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PS_ON
    XBH_U32 u32OpspowerTimer = 500;
    XBH_U32 u32Ops12VTimer = 200;
    XBH_BOOL bIsDet = XBH_FALSE;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_POWER_STATUS
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_POWER_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_POWER_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_1_DET
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}


XBH_S32 XbhXMR3576_D::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PWREN
    if (enSource == XBH_SOURCE_OPS1)
    {
        if (bEnable)
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
        else
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, !XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PWREN
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            s32Ret |= XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->setHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->setMuteMicIn(bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMR3576_D::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_DP1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;

        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        default:
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMR3576_D::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
#if 1
//拉低待机时的io口
    XBH_S32 i = 0;
    XBH_S32 count = sizeof(gpio_standby_table) / sizeof(struct gpio_standby_info);
    for (i = 0; i < count; i++) {
        setGpioOutputValue( gpio_standby_table[i].pin, gpio_standby_table[i].init_level);
    }
#endif
    xbh_led_standby_status(); 
#if 0

    request_gpio_wakeup(XBH_WAKE_UP_POWER_KEY, XBH_WAKE_UP_POWER_KEY_TYPE);

    if (s8Buff[XBH_WAKEUP_SOURCE])
    {
        XLOGD("Source wake is open");
        request_gpio_wakeup(XBH_WAKE_UP_HDMI1_DET, XBH_WAKE_UP_HDMI1_DET_TYPE);
        request_gpio_wakeup(XBH_WAKE_UP_HDMI2_DET, XBH_WAKE_UP_HDMI2_DET_TYPE);
        request_gpio_wakeup(XBH_WAKE_UP_DP_DET, XBH_WAKE_UP_DP_DET_TYPE);
        request_gpio_wakeup(XBH_WAKE_UP_F_HDMI_DET, XBH_WAKE_UP_F_HDMI_DET_TYPE);
        request_gpio_wakeup(XBH_WAKE_UP_TYPE_C_DET, XBH_WAKE_UP_TYPE_C_DET_TYPE);
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_PD_POWER, XBH_BOARD_GPIO_PD_POWER_OFF);
    }

    if (s8Buff[XBH_WAKEUP_ETH])
    {
        XLOGD("eth wake is open");
        request_gpio_wakeup(XBH_WAKE_UP_ETH, XBH_WAKE_UP_ETH_TYPE);
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_ETH_PWR, XBH_BOARD_GPIO_ETH_PWR_OFF);
    }

    if (s8Buff[XBH_WAKEUP_PIR])
    {
        XLOGD("pir wake is open");
        request_gpio_wakeup(XBH_WAKE_UP_PIR1, XBH_WAKE_UP_PIR1_TYPE);
        request_gpio_wakeup(XBH_WAKE_UP_PIR2, XBH_WAKE_UP_PIR2_TYPE);
    }

    if (s8Buff[XBH_WAKEUP_NFC])
    {
        XLOGD("nfc wake is open");
        request_gpio_wakeup(XBH_WAKE_UP_MCU_IRQ, XBH_WAKE_UP_MCU_IRQ_TYPE);
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_MCU_PWR, !XBH_BOARD_GPIO_MCU_PWR_ON);
    }
#endif
    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhXMR3576_D::loadEdlaCust(XBH_BOOL *enable)
{
    int swap_port = property_get_int32("persist.vendor.xbh.swap_port", 2);
    int versa_mode = property_get_int32("persist.vendor.xbh.versa_mode", 2);
    int swap_and_versa = 1;
    if((swap_port + versa_mode) == 4){
        swap_and_versa = 2;
    }
    
    setPanelMapSwap((swap_and_versa*100 + versa_mode*10 + swap_port));
    lcdparamservice:: rk_update_lcd_parameters_from_sdcard();
    *enable = XBH_TRUE;
    
    return 0;
}


XBH_S32 XbhXMR3576_D::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XbhXMR3576_D *XbhXMR3576_D::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMR3576_D();
        }
    }
    return mInstance;
}
