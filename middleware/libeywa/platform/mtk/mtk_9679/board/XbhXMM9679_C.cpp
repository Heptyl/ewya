#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_C"

#include "XbhLog.h"
#include "XbhXMM9679_C.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"

#define WAKE_UP_REASON   "/sys/mtk_pm/wakeup_reason/name"

XbhXMM9679_C            *XbhXMM9679_C::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_C::mLock;

XbhXMM9679_C::XbhXMM9679_C()
{
    XLOGD(" begin ");
    VgaEnable = XBH_FALSE;
    frontHpdState = XBH_TRUE;
    frontHdmiState = XBH_FALSE;
    frontUsbcState = XBH_FALSE;
    XLOGD(" end ");
}

XbhXMM9679_C::~XbhXMM9679_C()
{
}

XBH_S32 XbhXMM9679_C::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = getHdmiDirectDet(3, enable);
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8State;
    s32Ret = XbhVga2HdmiManager::getInstance()->getCurVgaStatus(XBH_SOURCE_VGA1, &u8State);
    if (u8State == 0x85) //STATE_NO_INPUT_SIGNAL
    {
        VgaEnable = XBH_FALSE;
    }
    else if (u8State == 0x0) //STATE_SUCCESS
    {
        VgaEnable = XBH_TRUE;
    }
    *enable = VgaEnable;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    
    if(XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        return s32Ret;
    }

    if (*enable == XBH_TRUE)
    {
        frontHdmiState = XBH_TRUE;
    }
    else
    {
        frontHdmiState = XBH_FALSE;
    }

    if (!frontHpdState && (frontHdmiState || frontUsbcState))
    {
        setHdmiHPD(0, XBH_LEVEL_HIGH);
        frontHpdState = XBH_TRUE;
    }

    if (frontHpdState && !frontHdmiState && !frontUsbcState)
    {
        setHdmiHPD(0, XBH_LEVEL_LOW);
        frontHpdState = XBH_FALSE;
    }

    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;

    if(XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        return s32Ret;
    }
    
    if (*enable == XBH_TRUE)
    {
        frontUsbcState = XBH_TRUE;
    }
    else
    {
        frontUsbcState = XBH_FALSE;
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;

    return s32Ret;
}

XBH_S32 XbhXMM9679_C::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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
        s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_POWER_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_POWER_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}


XBH_S32 XbhXMM9679_C::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_C::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getOpsOvercurrentStatus(XBH_U32 srcIdx, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_OVERCURRENT, &u32GpioValue);
        *enable = u32GpioValue == XBH_BOARD_GPIO_OPS_1_OVERCURRENT_LEVEL ? XBH_FALSE : XBH_TRUE;
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(HALL_INT){
        *bEnable = XBH_TRUE;
    }else{
        *bEnable = XBH_FALSE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 1 || u32GpioHallInt2 == 1){
        *s32Value = 1;
    }else{
        *s32Value = 0;
    }
    XLOGD("u32GpioHallInt1 = %d,u32GpioHallInt2 = %d",u32GpioHallInt1,u32GpioHallInt2);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getLightSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_OFF:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_LED_CTL,XBH_BOARD_GPIO_LOW);
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_LED_CTL,XBH_BOARD_GPIO_HIGH);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMM9679_C::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("---- onStop ----");
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_LED_RED, XBH_BOARD_GPIO_LED_1_EN_ON);
    s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_LED_WHITE, XBH_BOARD_GPIO_LED_2_EN_OFF);
    XBH_BOOL bWakeupSource = XBH_FALSE;
    s32Ret = getWoTStatus(XBH_WAKEUP_SOURCE,&bWakeupSource);
    if((s32Ret == XBH_FAILURE) || (bWakeupSource == XBH_FALSE)){
        XLOGD("----onStop close front typeC  power ----");
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_F_USBC_1_PWR, XBH_BOARD_GPIO_F_USBC_1_PWR_OFF);
    }
    XBH_BOOL bWakeupEth = XBH_FALSE;
    s32Ret = getWoTStatus(XBH_WAKEUP_ETH,&bWakeupEth);
    if((s32Ret == XBH_FAILURE) || (bWakeupEth == XBH_FALSE)){
        XLOGD("----onStop close eth power ----");
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_RTL8305_PWR, !XBH_BOARD_GPIO_RTL8305_PWR_LEVEL);
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_C::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
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

XBH_S32 XbhXMM9679_C::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
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

XBH_S32 XbhXMM9679_C::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
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

XBH_S32 XbhXMM9679_C::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS0_D2, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    // sider usb port
    // s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D3, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D4, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMM9679_C::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_CHAR prop[PROPERTY_VALUE_MAX] = {0};
    ret = XbhMtk_9679::getWakeUpReason(stWake);

    property_get("persist.vendor.xbh.wakeup_reason_is_hide", prop, "true");
    if(strcmp(prop, "false") == 0 )
    {
        XLOGE("Hide the wake-up reason externally.\n");
        ret = XBH_FAILURE;
    }
    return ret;
}

XBH_S32 XbhXMM9679_C::dumpEdid(XBH_SOURCE_E idx)
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

XbhXMM9679_C *XbhXMM9679_C::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_C();
        }
    }
    return mInstance;
}
