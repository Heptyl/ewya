#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMA311D2_B2"

#include "XbhLog.h"
#include "XbhXMA311D2_B2.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"


XbhXMA311D2_B2            *XbhXMA311D2_B2::mInstance = XBH_NULL;
XbhMutex                   XbhXMA311D2_B2::mLock;

#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"

XbhXMA311D2_B2::XbhXMA311D2_B2()
{
    XLOGD(" begin ");
    VgaEnable = XBH_FALSE;
    frontHpdState = XBH_TRUE;
    frontHdmiState = XBH_FALSE;
    frontUsbcState = XBH_FALSE;
    XLOGD(" end ");
}

XbhXMA311D2_B2::~XbhXMA311D2_B2()
{
}

XBH_S32 XbhXMA311D2_B2::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI1] = *enable;
    XLOGD("getHdmi1Det~\n");
    //s32Ret = switchSourceHpd(2, XBH_SOURCE_HDMI1, XBH_SOURCE_DP1);
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI2] = *enable;
    XLOGD("getHdmi2Det~\n");
    //s32Ret = switchSourceHpd(1, XBH_SOURCE_HDMI2, XBH_SOURCE_USBC1);
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_HDMI1] = *enable;
    //s32Ret = switchSourceHpd(3, XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_USBC1);
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_USBC1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_USBC1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    XBH_U32 u32AdcChannels;
    XBH_U32 u32AdcValue;

    u32AdcChannels = XBH_BOARD_DP_RTC_ADC_CHAN;
    u32AdcValue = 0;
    s32Ret = getADCChannelValue(u32AdcChannels,&u32AdcValue);
    XLOGD("getSourceDet----DP u32AdcValue : %d\n",u32AdcValue);
    if(u32AdcValue >= 500 && u32AdcValue <= 600 )  //   1v ~ 1.3V : DP in
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_VGA_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_VGA_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_VGA1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B2::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_OPS1] = *enable;
    return s32Ret;
}


XBH_S32 XbhXMA311D2_B2::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_B2::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_STATUS_ON ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}


XBH_S32 XbhXMA311D2_B2::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_B2::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getOpsOvercurrentStatus(XBH_U32 srcIdx, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_OVERCURRENT, &u32GpioValue);
        //*enable = u32GpioValue == XBH_BOARD_GPIO_OPS_1_OVERCURRENT_LEVEL ? XBH_FALSE : XBH_TRUE;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    /*if(HALL_INT){
        *bEnable = XBH_TRUE;
    }else{
        *bEnable = XBH_FALSE;
    }*/
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    //s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 1 || u32GpioHallInt2 == 1){
        *s32Value = 1;
    }else{
        *s32Value = 0;
    }
    XLOGD("u32GpioHallInt1 = %d,u32GpioHallInt2 = %d",u32GpioHallInt1,u32GpioHallInt2);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::setLedPwrStatus(XBH_LED_LIGHT_E enState)
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

XBH_S32 XbhXMA311D2_B2::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_B2::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::setWolEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    XLOGD("setWolEnable   =  %d ", bEnable);
    fd = open(RTK8367_WOL_DEV, O_WRONLY);
    if(fd < 0)
    {
        XLOGE("setWolEnable : open %s failed !!!!",RTK8367_WOL_DEV);
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


XBH_S32 XbhXMA311D2_B2::setOnStop()
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

XBH_S32 XbhXMA311D2_B2::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B2::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    // s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS0_D2, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    // sider usb port
    // s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D3, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    // s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D4, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B2::dumpEdid(XBH_SOURCE_E idx)
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

XbhXMA311D2_B2 *XbhXMA311D2_B2::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMA311D2_B2();
        }
    }
    return mInstance;
}