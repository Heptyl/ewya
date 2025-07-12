#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMA311D2_Y"

#include "XbhLog.h"
#include "XbhXMA311D2_Y.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhDp2HdmiManager.h"
#include "XbhMcuManager.h"
#include "XbhAVOutManager.h"
#include "XbhI2cSimulator.h"



XbhXMA311D2_Y            *XbhXMA311D2_Y::mInstance = XBH_NULL;
XbhMutex                   XbhXMA311D2_Y::mLock;

#define RTK8367RB_VB_WOL_DEV            "/sys/devices/virtual/rtl8367/phy/wol"
#define EDID_VGA                        "/vendor/etc/projects/projectid_0/edid/vga.bin"
#define XBH_EEPROM_ADDR         0xA0   //eeprom的设备地址
XbhXMA311D2_Y::XbhXMA311D2_Y()
{
    XLOGD(" begin ");
    VgaEnable = XBH_FALSE;
    frontHpdState = XBH_TRUE;
    frontHdmiState = XBH_FALSE;
    frontUsbcState = XBH_FALSE;
    XLOGD(" end ");
}

XbhXMA311D2_Y::~XbhXMA311D2_Y()
{
}

XBH_S32 XbhXMA311D2_Y::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_Y::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_Y::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_Y::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_Y::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_DP_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_Y::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_VGA_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_VGA_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_Y::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32OpspowerTimer = 500;
    while (XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        XLOGD("setOpsPowerCtrl~ wait middleware inited \n");
        usleep(20*1000); /* sleep 20ms */
    }
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_STATUS_ON ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (bEnable)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
    }
    else
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, !XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_OFF:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SYSTEM_LED_GPIO,XBH_BOARD_GPIO_LOW);
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SYSTEM_LED_GPIO,XBH_BOARD_GPIO_HIGH);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->setHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->setMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_STEREO:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->getHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->getMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        default:
            break;
    }
    XLOGD("getMute channel = %d mute = %d ",enAudioChannel, *bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setWolEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    XLOGD("setWolEnable  =  %d ", bEnable);
    fd = open(RTK8367RB_VB_WOL_DEV, O_WRONLY);
    if(fd < 0)
    {
        XLOGE("setWolEnable : open %s failed !!!!",RTK8367RB_VB_WOL_DEV);
        return XBH_FAILURE;
    }

    if(bEnable)
    {
        XBH_CHAR macaddress[17] = {0};
        s32Ret = getMacAddress(0, macaddress);

        if(s32Ret == XBH_SUCCESS)
        {
            XLOGE("setWolEnable: get Eth MAC %s",macaddress);
            s32Ret = write(fd, macaddress, sizeof(macaddress));
        }
        else
        {
            XLOGE("setWolEnable: get Eth MAC failed !!!");
        }
    }
    else
    {
        s32Ret = write(fd, "00:00:00:00:00:00", 17);  //disable WOL in phy driver
    }

    if(s32Ret > 0)
    {
        s32Ret = XBH_SUCCESS;
    }
    else
    {
         XLOGE("setWolEnable: write file failed !!!");
    }

    close(fd);

    s32Ret = this->XbhAmlogic_311d2::setWolMode(bEnable);

    XLOGD("setWolEnable: bEnable = %d, s32Ret = %d",bEnable,s32Ret);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[1] = {0};
    XLOGD("setWoTStatus enWakeUpType=%d bEnable=%d ",enWakeUpType, bEnable);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            setWolEnable(bEnable);
        break;
        case XBH_WAKEUP_SOURCE:
            this->XbhAmlogic_311d2::setWosEnable(bEnable);
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setWoTStatus enWakeUpType=%d  ",enWakeUpType);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            *bEnable = property_get_bool("persist.vendor.xbh.wol.enable", false);
        break;
        case XBH_WAKEUP_SOURCE:
            *bEnable = this->XbhAmlogic_311d2::getWosEnable();
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR sWolStatus[PROPERTY_VALUE_MAX];
    XLOGD("---- onStop ----");

    property_get("persist.vendor.xbh.wol.enable", sWolStatus, "false");
    if(strncmp(sWolStatus, "true",4) == 0) {
        setWolEnable(XBH_TRUE);
    } else {
        setWolEnable(XBH_FALSE);
    }

    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_DP1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_LT6711A:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_LT6711A, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_LT6711A:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_LT6711A, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}


XBH_S32 XbhXMA311D2_Y::upgradeExtendIcByData(XBH_U8 *data, XBH_U32 dataLen, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmwareByData(XBH_SOURCE_F_USBC1, data, dataLen, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMA311D2_Y::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1,s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_LT6711A:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_LT6711A, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("getUpgradeExtIcState :%d\n", * s32Value);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    if(u32UsbType == 0)
    {
        //enable front usb port and sider usb port----- type = 0
        XLOGD("XbhSetUsbLockMode enable front usb port and sider usb port , type is %d\n", u32UsbType);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_FUSB_PWREN_PIN, XBH_BOARD_GPIO_HIGH);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_AUSB20_PWREN_PIN, XBH_BOARD_GPIO_HIGH);
    } 
    else if(u32UsbType == 1)
    {
        //just disable front usb port----- type = 1
        XLOGD("XbhSetUsbLockMode just disable front usb port , type is %d\n", u32UsbType);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_FUSB_PWREN_PIN, XBH_BOARD_GPIO_LOW);
        
    }
    else if(u32UsbType == 2)
    {
        //just disable sider usb port----- type = 2
        XLOGD("XbhSetUsbLockMode disable front usb port and sider usb port , type is %d\n", u32UsbType);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_AUSB20_PWREN_PIN, XBH_BOARD_GPIO_LOW);
    }
    else if(u32UsbType == 3)
    {
        //disable front usb port and sider usb port ----- type = 3
        XLOGD("XbhSetUsbLockMode enable front usb port , type is %d\n", u32UsbType);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_FUSB_PWREN_PIN, XBH_BOARD_GPIO_LOW);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_AUSB20_PWREN_PIN, XBH_BOARD_GPIO_LOW);
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::dumpEdid(XBH_SOURCE_E idx)
{
    XLOGD("dump edid\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(idx)
    {
        case XBH_SOURCE_E::XBH_SOURCE_VGA1:
            s32Ret = XbhVgaEdidManager::getInstance()->dumpEdid();
        break;
        default:
        break;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::initProjectIdConfig()
{
    //每次开机都需要更新edid的数据
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    XLOGD("initProjectIdConfig projectid=%d \n", projectid);
    XBH_HDMIRX_EDID_TYPE_E type;
     if(projectid == 0)
     {
         //当VGA EDID没有更新时需要更新
         if(property_get_int32("persist.vendor.xbh.vgaedid.update", 0) == 0)
         {
            
             property_set("persist.vendor.xbh.vgaedid.update", "1");
             XbhVgaEdidManager::getInstance()->setVgaEdid(EDID_VGA, XBH_SOURCE_VGA1);
         }
     }
      else//其它projectid 从bin文件中获取
     {
        readAndUpdateEdidBinFile(projectid);
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_Y::setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length) 
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL, XBH_EEPROM_ADDR);
    if (pSimulator == nullptr) {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Write_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL, XBH_EEPROM_ADDR);
    if (pSimulator == nullptr) {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Read_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = 0;
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            type = 1;
            break;
        case XBH_HDMIRX_EDID_TYPE_20:
            type = 2;
            break;
        case XBH_HDMIRX_EDID_TYPE_21:
            type = 3;
            break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            type = 4;
            break;
        default:
            break;
    }
    s32Ret = XbhAmlogic_311d2::setHdmirxEdidType(type);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_Y::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = 2;
    s32Ret = XbhAmlogic_311d2::getHdmirxEdidType(&type);
    if(type == 1)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_14;
    }
    else if(type == 2)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_20;
    }
    else if(type == 3)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_21;
    }
    else if(type == 4)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_AUTO;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_Y::readAndUpdateEdidBinFile(int projectid)
{
     if(projectid > 0)//分别读取4个bin文件，对应不同端口的edid
    {
        char edid20BinFilePath[128] = {0};
        XBH_HDMIRX_EDID_TYPE_E type;
        //当VGA EDID没有更新时需要更新
        if(property_get_int32("persist.vendor.xbh.vgaedid.update", 0) == 0)
        {
            property_set("persist.vendor.xbh.vgaedid.update", "1");
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/projects/projectid_%d/edid/vga.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_MS9282, edid20BinFilePath, XBH_HDMI_RXA);
        }
    }
     return XBH_SUCCESS;
}

XbhXMA311D2_Y *XbhXMA311D2_Y::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMA311D2_Y();
        }
    }
    return mInstance;
}