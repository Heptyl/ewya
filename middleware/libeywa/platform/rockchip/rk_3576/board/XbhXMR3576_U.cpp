#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMR3576_U"

#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhXMR3576_U.h"
#include "XbhSysOpt.h"
#include "XbhGSensorManager.h"

#include "XbhRk_3576.h"
#include "XbhPartitionData.h"

XbhXMR3576_U            *XbhXMR3576_U::mInstance = XBH_NULL;
XbhMutex                 XbhXMR3576_U::mLock;

#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"
#define HDMI_RX_EDID            "/sys/class/hdmirx/rk628/edid_index"

XbhXMR3576_U::XbhXMR3576_U()
{
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMR3576_U::~XbhXMR3576_U()
{
}

XBH_S32 XbhXMR3576_U::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_2_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_USBC_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_U::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_U::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_U::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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


XBH_S32 XbhXMR3576_U::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_U::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_U::getAirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAirSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *fHumidity = XbhAirSensorManager::getInstance()->getHumidityValue();
    *fTemperature = XbhAirSensorManager::getInstance()->getTemperatureValue();
    *fTvoc = XbhAirSensorManager::getInstance()->getVocValue();
    *fPm2p5 = XbhAirSensorManager::getInstance()->getPm25Value();
    *fCo2 = XbhAirSensorManager::getInstance()->getCo2Value();
    *fPm1p0 = 0;
    *fPm10 = 0;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getMute channel = %d",enAudioChannel);
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
    return  s32Ret;
}
XBH_S32 XbhXMR3576_U::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string value;
    XLOGD("get Hdmi Tx Info enInfo = %d",enInfo);
    switch(enInfo)
    {
        case XBH_HDMI_TX_VIDEO_MUTE:
            XbhSysOpt::getInstance()->readSysfs("/sys/class/drm/card0-DP-1/status", value);
            if(strncmp(value.c_str(),"connected",strlen("connected")) == 0)
            {
                strcpy(info,"0");
            }
             else if(strncmp(value.c_str(),"disconnected",strlen("disconnected")) == 0)
            {
                strcpy(info,"1");
            }
            s32Ret = XBH_SUCCESS;
            break;
        default:
            break;
    }
    return s32Ret;
}
XBH_S32 XbhXMR3576_U::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("set Hdmi Tx Info enInfo = %d data = %d ",enInfo, u32Data);
    switch(enInfo)
    {
        case XBH_HDMI_TX_VIDEO_MUTE:    //HDMI TX VIDEO 开关(可选支持)
            if(u32Data == 0)
            {
                XbhSysOpt::getInstance()->writeSysfs("/sys/class/drm/card0-DP-1/status","detect");
            }
            else if(u32Data == 1)
            {
                XbhSysOpt::getInstance()->writeSysfs("/sys/class/drm/card0-DP-1/status","off");
            }
            s32Ret = XBH_SUCCESS;
            break;
        default:
            break;
    }
    return s32Ret;
}

XBH_S32 XbhXMR3576_U::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XLOGD("set Hdmi Tx Info enType = %d",enType);
    XBH_S32 type  = 2;
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"0");
            type = 1;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_20:
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"1");
            type = 2;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_21:
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"2");
            type = 3;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"3");
            type = 4;
            s32Ret = XBH_SUCCESS;
        break;
        default:
        break;
    }
    s32Ret = XbhRk_3576::setHdmirxEdidType(type);
    XLOGD("setHdmirxEdidType %d \n", type);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string value;
    XbhSysOpt::getInstance()->readSysfs(HDMI_RX_EDID, value);
    if(strncmp(value.c_str(),"XBH_HDMIRX_EDID_TYPE_14",strlen("XBH_HDMIRX_EDID_TYPE_14")) == 0)
    {
         *enType = XBH_HDMIRX_EDID_TYPE_14;
         s32Ret = XBH_SUCCESS;
    }
    else if(strncmp(value.c_str(),"XBH_HDMIRX_EDID_TYPE_20",strlen("XBH_HDMIRX_EDID_TYPE_20")) == 0)
    {
         *enType = XBH_HDMIRX_EDID_TYPE_20;
         s32Ret = XBH_SUCCESS;
    }
    else if(strncmp(value.c_str(),"XBH_HDMIRX_EDID_TYPE_21",strlen("XBH_HDMIRX_EDID_TYPE_21")) == 0)
    {
         *enType = XBH_HDMIRX_EDID_TYPE_21;
         s32Ret = XBH_SUCCESS;
    }
    else if(strncmp(value.c_str(),"XBH_HDMIRX_EDID_TYPE_AUTO",strlen("XBH_HDMIRX_EDID_TYPE_AUTO")) == 0)
    {
         *enType = XBH_HDMIRX_EDID_TYPE_AUTO;
         s32Ret = XBH_SUCCESS;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMR3576_U::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt1 = 0;
    XBH_U32 u32GpioPirInt2 = 0;

    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT1,&u32GpioPirInt1);
    s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_PIR_INT2,&u32GpioPirInt2);

    if(u32GpioPirInt1 == 0 || u32GpioPirInt2 == 0)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;
    }

    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_HALL_CHAN, &adcValue);

    if(adcValue > XBH_BOARD_ADC_HALL_CHAN_VALUE)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;
    }

    return s32Ret;
}

XBH_S32 XbhXMR3576_U::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_DP1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_ITE6563OUT, ver);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
	    case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename,force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_ITE6563OUT, filename, force);
            break;
        default:
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMR3576_U::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_ITE6563OUT, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMR3576_U::openEthWol(XBH_BOOL bEnable)
{
    int fd = open(RTK8367_WOL_DEV, O_WRONLY);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR mac_addr[17];
    if (bEnable)
    {
        if (fd == -1)
        {
            XLOGE("open_wol dev error!!!\n");
            return s32Ret;
        }

        getMacAddress(0, mac_addr);
        s32Ret = write(fd, mac_addr, 17);
        if (s32Ret < 0)
        {
            XLOGE("write wol dev error!!!\n");
        }
        close(fd);
        return s32Ret;
    }
    else
    {
        s32Ret = XBH_SUCCESS;
    }

    return s32Ret;
}

XBH_S32 XbhXMR3576_U::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{

    XLOGD("getSrcDetByDetGpio gpio = %d u32GpioValue = u32GpioValue %d\n", gpio ,u32GpioValue);
    XBH_S32 s32Ret = XBH_SUCCESS;
    #if 0
    switch(gpio)
    {
        case XBH_BOARD_GPIO_HDMI_1_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI1;
            if(u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
            }
            else
            {
                *bEnable = XBH_FALSE;
                 setHdmiRxCsiSteamState(false);
            }
            break;
        #ifdef XBH_BOARD_GPIO_HDMI_2_DET
        case XBH_BOARD_GPIO_HDMI_2_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI2;
            if(u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL)
            {
                *bEnable = XBH_TRUE;
            }
            else
            {
                *bEnable = XBH_FALSE;
                 setHdmiRxCsiSteamState(false);
            }
            break;
        #endif
        #ifdef XBH_BOARD_GPIO_USBC_1_DET
        case XBH_BOARD_GPIO_USBC_1_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_USBC1;
            if(u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
            }
            else
            {
                *bEnable = XBH_FALSE;
                 setHdmiRxCsiSteamState(false);
            }
            break;
        #endif
         case XBH_BOARD_GPIO_F_USBC_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_F_USBC1;
            if(u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
            }
            else
            {
                *bEnable = XBH_FALSE;
                 setHdmiRxCsiSteamState(false);
            }
            break;
        case XBH_BOARD_GPIO_F_HDMI_DET:
           *src = XBH_SOURCE_E::XBH_SOURCE_F_HDMI1;
           if(u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL)
           {
               *bEnable = XBH_TRUE;
           }
           else
           {
               *bEnable = XBH_FALSE;
                setHdmiRxCsiSteamState(false);
           }
           break;
        case XBH_BOARD_GPIO_DP_1_IN_DET_PIN:
           *src = XBH_SOURCE_E::XBH_SOURCE_DP1;
           if(u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL)
           {
               *bEnable = XBH_TRUE;
           }
           else
           {
               *bEnable = XBH_FALSE;
                setHdmiRxCsiSteamState(false);
           }
           break;
        default:
            *src = XBH_SOURCE_E::XBH_SOURCE_BUTT;
            break;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::setHdmiRx5vDet(XBH_BOOL enable)
{
    XLOGD("setHdmiRx5vDetFlag state:%d\n", enable);
    XBH_S32 s32Ret = XBH_SUCCESS;
    XbhRk_3576::setHdmiRx5vDet(enable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    XbhRk_3576::setOnStop();
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);

    freeAllGpioIrq();

    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    requestGpioIrq(XBH_WAKE_UP_POWER_KEY, XBH_WAKE_UP_POWER_KEY_TYPE);

    if (s8Buff[XBH_WAKEUP_SOURCE])
    {
        //判断前置板是否使用GSV芯片，如果是则在关机时然GVS芯片进入待机模式，也就是开启前置板typec信源唤醒
        char dev_type[PROPERTY_VALUE_MAX] = {0};
        property_get("persist.vendor.xbh.fboard.type", dev_type, "0");
        if(strcmp(dev_type, "0x20") == 0)
        {
            XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        }

        XLOGD("Source wake is open");
        requestGpioIrq(XBH_WAKE_UP_HDMI1_DET, XBH_WAKE_UP_HDMI1_DET_TYPE);
        #ifdef XBH_WAKE_UP_HDMI2_DET
        requestGpioIrq(XBH_WAKE_UP_HDMI2_DET, XBH_WAKE_UP_HDMI2_DET_TYPE);
        #endif
        requestGpioIrq(XBH_WAKE_UP_DP_DET, XBH_WAKE_UP_DP_DET_TYPE);
        requestGpioIrq(XBH_WAKE_UP_F_HDMI_DET, XBH_WAKE_UP_F_HDMI_DET_TYPE);
        #ifdef XBH_WAKE_UP_TYPE_C_DET
        requestGpioIrq(XBH_WAKE_UP_TYPE_C_DET, XBH_WAKE_UP_TYPE_C_DET_TYPE);
        #endif
        requestGpioIrq(XBH_WAKE_UP_F_TYPE_C_DET, XBH_WAKE_UP_F_TYPE_C_DET_TYPE);
    }
    else
    {
        #ifdef XBH_BOARD_GPIO_PD_POWER
        setGpioOutputValue(XBH_BOARD_GPIO_PD_POWER, XBH_BOARD_GPIO_PD_POWER_OFF);
        #endif
    }

    if (s8Buff[XBH_WAKEUP_ETH])
    {
        XLOGD("eth wake is open");
        openEthWol(XBH_TRUE);
        requestGpioIrq(XBH_WAKE_UP_ETH, XBH_WAKE_UP_ETH_TYPE);
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_ETH_PWR, XBH_BOARD_GPIO_ETH_PWR_OFF);
    }

    if (s8Buff[XBH_WAKEUP_PIR])
    {
        XLOGD("pir wake is open");
        requestGpioIrq(XBH_WAKE_UP_PIR1, XBH_WAKE_UP_PIR1_TYPE);
        requestGpioIrq(XBH_WAKE_UP_PIR2, XBH_WAKE_UP_PIR2_TYPE);
    }
    #ifdef XBH_BOARD_GPIO_NFC_EN
    setGpioOutputValue(XBH_BOARD_GPIO_NFC_EN, XBH_BOARD_GPIO_NFC_EN_LEVEL);
    #endif
    if (s8Buff[XBH_WAKEUP_NFC])
    {
        XLOGD("nfc wake is open");
        requestGpioIrq(XBH_WAKE_UP_MCU_IRQ, XBH_WAKE_UP_MCU_IRQ_TYPE);
        // 进入待机后关闭部分mcu引脚
        #ifdef XBH_BOARD_GPIO_ADC_RESET
        setGpioOutputValue(XBH_BOARD_GPIO_ADC_RESET, XBH_BOARD_GPIO_LOW);
        #endif
        setGpioOutputValue(XBH_BOARD_GPIO_TOUCH_DEV_PWR, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_DPOUT_HDMI_RESET, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU40_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_HDMI_SEL, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_DEVICE_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_WIFI_RESET, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU4_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, XBH_BOARD_GPIO_LOW);
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_MCU_PWR, !XBH_BOARD_GPIO_MCU_PWR_ON);
    }

    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::initSrcDetGpioIrq()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if 0
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_HDMI_1_DET, IRQF_TRIGGER_RISING);
    #ifdef XBH_BOARD_GPIO_HDMI_2_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_HDMI_2_DET, IRQF_TRIGGER_RISING);
    #endif
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_F_HDMI_DET, IRQF_TRIGGER_RISING);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_F_USBC_DET, IRQF_TRIGGER_FALLING);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, IRQF_TRIGGER_RISING);
    #ifdef XBH_BOARD_GPIO_USBC_1_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_USBC_1_DET, IRQF_TRIGGER_FALLING);
    #endif
    //requestGpioIrq(XBH_BOARD_GPIO_USBC_1_DET, IRQF_TRIGGER_FALLING);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_F_USB_PWR_LEVEL : !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
    // sider usb port
    #ifdef XBH_BOARD_GPIO_AN_PROT_DEV_PWR
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_AN_PROT_DEV_PWR, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_AN_PROT_DEV_PWR_ON : XBH_BOARD_GPIO_AN_PROT_DEV_PWR_OFF);
    #endif
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_PUB_USB30_DEV_PWR, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_PUB_USB30_DEV_PWR_ON : XBH_BOARD_GPIO_PUB_USB30_DEV_PWR_OFF);
    return s32Ret;
}

XBH_S32 XbhXMR3576_U::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

/**
 * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3576_U::setAvcEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setAvcEnable(bEnable);
    return  s32Ret;
}

/**
 * 获取AVC(自动电压控制)开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3576_U::getAvcEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getAvcEnable(bEnable);
    return  s32Ret;
}

XbhXMR3576_U *XbhXMR3576_U::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMR3576_U();
        }
    }
    return mInstance;
}
XBH_S32 XbhXMR3576_U::getGSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

XBH_S32 XbhXMR3576_U::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 ret = XbhRk_3576::setHdmiLowLatency(bEnable);
    return ret;
}