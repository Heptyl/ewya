#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMA311D2_B"

#include "XbhLog.h"
#include "XbhXMA311D2_B.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"
#include "XbhMcuManager.h"

XbhXMA311D2_B            *XbhXMA311D2_B::mInstance = XBH_NULL;
XbhMutex                   XbhXMA311D2_B::mLock;

#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"
//HALL sensor MCU I2C register
#define CMD_I2C_GET_ADC         0xA6

XbhXMA311D2_B::XbhXMA311D2_B()
{
  XLOGD(" NULL ");
}

XbhXMA311D2_B::~XbhXMA311D2_B()
{
}

XBH_S32 XbhXMA311D2_B::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
// XBH_S32 XbhXMA311D2_B::getDP1Det(XBH_BOOL *enable)
// {
//     XBH_S32 s32Ret = XBH_FAILURE;
//     XBH_U32 u32GpioValue;
//     s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_DP_IN_DET_PIN, &u32GpioValue);
//     *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
//     return s32Ret;
// }
XBH_S32 XbhXMA311D2_B::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_VGA_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_VGA_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_B::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_B::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_STATUS_ON ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_B::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}

// XBH_S32 XbhXMA311D2_B::getHallSensorExist(XBH_BOOL* bEnable)
// {
//     XBH_S32 s32Ret = XBH_SUCCESS;
//     XBH_U32 u32GpioHallInt1 = 0;
//     XBH_U32 u32GpioHallInt2 = 0;
//     s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
//     s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
//     if(u32GpioHallInt1 == 0 || u32GpioHallInt2 == 0){
//         *bEnable = 1;
//     }else{
//         *bEnable = 0;
//     }
//     return  s32Ret;
// }

// XBH_S32 XbhXMA311D2_B::getHallSensorValue(XBH_S32* s32Value)
// {
//     XBH_S32 s32Ret = XBH_FAILURE;
//     XBH_U32 u32GpioHallInt1 = 0;
//     XBH_U32 u32GpioHallInt2 = 0;
//     s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
//     s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
//     if(u32GpioHallInt1 == 0 && u32GpioHallInt2 == 0){
//         *s32Value = 2;
//     }else if(u32GpioHallInt1 != 0 && u32GpioHallInt2 != 0){
//         *s32Value = 0;
//     }else{
//         *s32Value = 1;
//     }
//     return  s32Ret;
// }

XBH_S32 XbhXMA311D2_B::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;
    XBH_MCU_I2CBUFFDEF_S stOutData;

    stOutData.cmd = CMD_I2C_GET_ADC;
    stOutData.len = 3;
    s32Ret = this->XbhAmlogic_311d2::getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADR, stOutData.cmd, 0x01, stOutData.len, stOutData.data);

    adcValue = stOutData.data[0] << 8 | stOutData.data[1] ;

    if(adcValue > 1150 && adcValue < 1600)
    {
        *s32Value = 1;   //当只有一支笔,不管是A笔还是B笔,范围都在1150 < adcValue < 1600
    }
    else if(adcValue > 1600)
    {
        *s32Value = 2;   //当两只笔都放上,adcValue > 1600
    }
    else
    {
        *s32Value = 0;   //当没有笔放上,adcValue < 1150
    }
    XLOGD("getHallSensorValue adcValue : %d s32Value:%d",adcValue,*s32Value);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;   //默认为开
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt = 0;

    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR1,&u32GpioPirInt);

    *s32Value = u32GpioPirInt == XBH_BOARD_GPIO_PIR_DETECT_LEVEL ? 1 : 0;
    XLOGD("getPirSensorValue Value: %d %d ret:%d",*s32Value,u32GpioPirInt,s32Ret);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::setLedPwrStatus(XBH_LED_LIGHT_E enState)
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

XBH_S32 XbhXMA311D2_B::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_B::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::setWolEnable(XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_B::setOnStop()
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

XBH_S32 XbhXMA311D2_B::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
        s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC1, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_B::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_B::dumpEdid(XBH_SOURCE_E idx)
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

XbhXMA311D2_B *XbhXMA311D2_B::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMA311D2_B();
        }
    }
    return mInstance;
}