#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMR3588_U"

#include "XbhLog.h"
#include "XbhXMR3588_U.h"
#include "XbhGSensorManager.h"
#include "XbhAirSensorManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhAVOutManager.h"
#include "XbhSysOpt.h"
#include "XbhMcuManager.h"
#include <hardware/xbh_edid.h>
#include <sys/stat.h>

//#include "XbhRk_3576.h"
#include "XbhPartitionData.h"

#define BACKLIIGHT_CTRL    "/sys/devices/platform/backlight/backlight/backlight/bl_power"

XbhXMR3588_U            *XbhXMR3588_U::mInstance = XBH_NULL;
XbhMutex                 XbhXMR3588_U::mLock;

XbhXMR3588_U::XbhXMR3588_U()
{
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMR3588_U::~XbhXMR3588_U()
{
}

XBH_S32 XbhXMR3588_U::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_5K_HDMI_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_5K_HDMI_IN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getDP2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_5K_DP_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_5K_DP_IN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getOps2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PS_ON
    XBH_U32 u32OpspowerTimer = 500;
    XBH_U32 u32Ops12VTimer = 200;
    XBH_BOOL bIsDet = XBH_FALSE;
    if (enSource == XBH_SOURCE_OPS1 || enSource == XBH_SOURCE_OPS2)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000); 
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_POWER_STATUS
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1 || enSource == XBH_SOURCE_OPS2)
    {
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_POWER_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_POWER_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (enSource == XBH_SOURCE_OPS1 || enSource == XBH_SOURCE_OPS2)
    {
        XBH_U32 u32GpioValue;
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PWREN
    if (enSource == XBH_SOURCE_OPS1 || enSource == XBH_SOURCE_OPS2)
    {
        if (bEnable)
        {
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
        }
        else
        {
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, !XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
        }
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PWREN
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1 || enSource == XBH_SOURCE_OPS2)
    {
        s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
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
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_BOARD_GSV6712_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV6712_1, ver);
            break;
        case XBH_UPDATE_BOARD_LT8668SX_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_LT8668SX_1, ver);
            break;
        case XBH_UPDATE_BOARD_LT8668SXC_1:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_LT8668SXC_1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        default:
            XLOGE(" NOT FOUND UPGRADE IC\n");
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
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
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV6712_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV6712_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_LT8668SX_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_LT8668SX_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_LT8668SXC_1:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_LT8668SXC_1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        default:
            XLOGE(" NOT FOUND UPGRADE IC\n");
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMR3588_U::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
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
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV6712_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV6712_1, s32Value);
            break;
        case XBH_UPDATE_BOARD_LT8668SX_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_LT8668SX_1, s32Value);
            break;
        case XBH_UPDATE_BOARD_LT8668SXC_1:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_LT8668SXC_1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        default:
            XLOGE(" NOT FOUND UPGRADE IC\n");
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::setHdcpExtendIc(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_LT8668SX_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->setHdcpKey(XBH_UPDATE_BOARD_LT8668SX_1, strPath);
            break;
        default:
            XLOGE(" NOT FOUND set IC\n");
            break;
    }
    XLOGD("setHdcpKey file:%s\n", strPath);
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getHdcpExtIcStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_LT8668SX_1:
            // s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(bEnable, XBH_UPDATE_BOARD_LT8668SX_1);
            break;
        default:
            XLOGE(" NOT FOUND UPGRADE IC\n");
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::setHdcpExtIcKeyName(const XBH_CHAR* strHdcpFileName, XBH_S32 hdcpType, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_LT8668SX_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->setHdcpKeyName(XBH_UPDATE_BOARD_LT8668SX_1, strHdcpFileName);
            break;
        default:
            XLOGE(" NOT FOUND set IC\n");
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getHdcpExtIcKeyName(XBH_CHAR* strHdcpFileName, XBH_S32 hdcpType, XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_LT8668SX_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getHdcpKeyName(XBH_UPDATE_BOARD_LT8668SX_1, strHdcpFileName);
            break;
        default:
            XLOGE(" NOT FOUND set IC\n");
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getGSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
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

XBH_S32 XbhXMR3588_U::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            //s32Ret |= XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
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

XBH_S32 XbhXMR3588_U::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMR3588_U::getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *enAudioOutput = XBH_AUDIO_OUTPUT_E::XBH_AUDIO_OUTPUT_DET;
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getAirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAirSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::initSrcDetGpioIrq()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if 0
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_5K_DP_IN_DET_PIN, &u32GpioValue);
    s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_5K_HDMI_IN_DET_PIN, &u32GpioValue);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_5K_DP_IN_DET_PIN, XBH_BOARD_GPIO_5K_DP_IN_DET_IRQ_TYPE);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_5K_HDMI_IN_DET_PIN, XBH_BOARD_GPIO_5K_HDMI_IN_DET_IRQ_TYPE);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    #if 0
    switch(gpio)
    {
        case XBH_BOARD_GPIO_5K_DP_IN_DET_PIN:
            *src = XBH_SOURCE_E::XBH_SOURCE_DP2;
            if(u32GpioValue == XBH_BOARD_GPIO_5K_DP_IN_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
            }
            else
            {
                *bEnable = XBH_FALSE;
            }
        break;
        case XBH_BOARD_GPIO_5K_HDMI_IN_DET_PIN:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI2;
            if(u32GpioValue == XBH_BOARD_GPIO_5K_HDMI_IN_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
            }
            else
            {
                *bEnable = XBH_FALSE;
            }
        break;
        default:
            *src = XBH_SOURCE_E::XBH_SOURCE_BUTT;
        break;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    XLOGD("---- onStop ----");
    //pgz add, do poweroff sequence: hdmi2eDp panel
     XbhSysOpt::getInstance()->writeSysfs(BACKLIIGHT_CTRL,"1");//关闭背光
    //setBacklightEnable(XBH_FALSE,XBH_PANEL_MAIN);
    usleep(150 * 1000);
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_HDMI2EDP_CHIP_RST, XBH_BOARD_GPIO_HDMI2EDP_CHIP_RST_ON);
    usleep(10 * 1000);
    #ifdef XBH_ENABLE_3588_4K
    if (XBH_ENABLE_3588_4K == 0)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_PANEL_PWR, XBH_BOARD_GPIO_PANEL_PWR_OFF);
    }
    #endif
    

    freeAllGpioIrq();

    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    requestGpioIrq(XBH_WAKE_UP_POWER_KEY, XBH_WAKE_UP_POWER_KEY_TYPE);
    requestGpioIrq(XBH_WAKE_UP_MCU_IRQ,XBH_WAKE_UP_MCU_IRQ_TYPE);

    // 进入待机后关闭部分mcu引脚
    setGpioOutputValue(XBH_BOARD_GPIO_GSV2705_HDMISW_RST, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_GSV6712_HDMISW_RST, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_CODEC_POWER, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_DP_2_H_RST, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_PUB_USB20_DEV_PWR, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_AN_USB30_DEV_POWER, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_WIFI_AP_POWER, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_WIFI_POWER, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_REDRIVER_POWER, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU3_RST, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU1_RST, XBH_BOARD_GPIO_LOW);
    setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU31_RST, XBH_BOARD_GPIO_LOW);

    if (s8Buff[XBH_WAKEUP_SOURCE])
    {
        XLOGD("Source wake is open");
        //判断前置板是否使用GSV芯片，如果是则在关机时然GVS芯片进入待机模式，也就是开启前置板typec信源唤醒
        char dev_type[PROPERTY_VALUE_MAX] = {0};
        property_get("persist.vendor.xbh.fboard.type", dev_type, "0");
        if(strcmp(dev_type, "0x80") == 0)
        {
            XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        }
        requestGpioIrq(XBH_WAKE_UP_HDMI1_DET, XBH_WAKE_UP_HDMI1_DET_TYPE);
        requestGpioIrq(XBH_WAKE_UP_HDMI2_DET, XBH_WAKE_UP_HDMI2_DET_TYPE);
        requestGpioIrq(XBH_WAKE_UP_DP_DET, XBH_WAKE_UP_DP_DET_TYPE);
        requestGpioIrq(XBH_WAKE_UP_DP2_DET, XBH_WAKE_UP_DP2_DET_TYPE);
        requestGpioIrq(XBH_WAKE_UP_F_HDMI_DET, XBH_WAKE_UP_F_HDMI_DET_TYPE);
        //requestGpioIrq(XBH_WAKE_UP_TYPE_C_DET, XBH_WAKE_UP_TYPE_C_DET_TYPE);
        requestGpioIrq(XBH_WAKE_UP_F_TYPE_C_DET, XBH_WAKE_UP_F_TYPE_C_DET_TYPE);
        if(s8Buff[XBH_WAKEUP_FTYPEC] == 0)
        {
            setGpioOutputValue(XBH_BOARD_GPIO_F_USBC_PWR,!XBH_BOARD_GPIO_F_USBC_PWR_LEVEL);
        }
     }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_F_USBC_PWR,!XBH_BOARD_GPIO_F_USBC_PWR_LEVEL);
    }
    if (s8Buff[XBH_WAKEUP_ETH])
    {
        XLOGD("eth wake is open");
        setGpioOutputValue(XBH_BOARD_GPIO_GMAC0_RXCLK, XBH_BOARD_GPIO_GMAC0_RXCLK_OFF);//开启网络唤醒时,该脚存在漏电情况,所以将该脚设置成输出脚，强制拉低，避免漏电情况
    }
    else
    {
        //setGpioOutputValue(XBH_BOARD_GPIO_ETH_PWR, XBH_BOARD_GPIO_ETH_PWR_OFF);
    }

    
    /*
    if (s8Buff[XBH_WAKEUP_PIR])
    {
        XLOGD("pir wake is open");
        requestGpioIrq(XBH_WAKE_UP_PIR1, XBH_WAKE_UP_PIR1_TYPE);
        requestGpioIrq(XBH_WAKE_UP_PIR2, XBH_WAKE_UP_PIR2_TYPE);
    }
    #ifdef XBH_BOARD_GPIO_NFC_EN
    setGpioOutputValue(XBH_BOARD_GPIO_NFC_EN, XBH_BOARD_GPIO_NFC_EN_LEVEL);
    #endif
 */
    if (s8Buff[XBH_WAKEUP_NFC])
    {
        XLOGD("nfc wake is open");
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_SOC_NFC_EN, XBH_BOARD_GPIO_SOC_NFC_EN_OFF);
    }
    if (!s8Buff[XBH_WAKEUP_ETH] && !s8Buff[XBH_WAKEUP_NFC])
    {
        setGpioOutputValue(XBH_BOARD_GPIO_MCU_PWR, !XBH_BOARD_GPIO_MCU_PWR_ON);
    }


    return  s32Ret;
}

XBH_S32 XbhXMR3588_U::processOpsSignalStatus(XBH_SOURCE_E u32Source)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3588_U::setAvcEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhXMR3588_U::getAvcEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getAvcEnable(bEnable);
    return  s32Ret;
}

XbhXMR3588_U *XbhXMR3588_U::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMR3588_U();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMR3588_U::setMipiRxCsiControlState(XBH_SOURCE_E u32Source)
{

    XLOGD("setMipiRxCsiControlState u32Source:%d\n", u32Source);
    XBH_S32 s32Ret = XBH_SUCCESS;
#ifdef XBH_ENABLE_3588_4K
    if (XBH_ENABLE_3588_4K == 1)
    {
        return s32Ret;
    }
#endif
    XBH_U8 DATA1 = 0x10; // 定义为单个值  
    const XBH_U8* pData1 = &DATA1; // 创建指向 DATA 的指针 
    XBH_U8 DATA4 = 0x22; // 定义为单个值  
    const XBH_U8* pData4 = &DATA4; // 创建指向 DATA 的指针 
    switch(u32Source)
    {
        case  XBH_SOURCE_OPS2:
            setIICData(GSV6712_I2C_BUS, XBH_GSV_DEVICE_ADDRESS, HPD_REGISTER_ADDRESS, HPD_REGISTER_LEN, DATA_LENGTH, pData4);//拉高OPS2 的HPD
            usleep(500 *1000);
            setIICData(GSV2705_I2C_BUS, XBH_GSV_DEVICE_ADDRESS, HPD_REGISTER_ADDRESS, HPD_REGISTER_LEN, DATA_LENGTH, pData1);//拉低OPS1 的HPD
            break;
        case  XBH_SOURCE_OPS1:
             postMsg(0, &u32Source, 1, 0, this);
            break;
        default:
            break;
    }
    return  s32Ret;
}

void XbhXMR3588_U::run(const void* arg)
{
    XBH_U32 msgType;
    XBH_U32 size;
    if(getMsg(&msgType, &mChangeToSrc, &size, this))
    {
         XBH_U8 DATA2 = 0x11; // 定义为单个值  
         const XBH_U8* pData2 = &DATA2; // 创建指向 DATA 的指针 
         XBH_U8 DATA3 = 0x20; // 定义为单个值  
         const XBH_U8* pData3 = &DATA3; // 创建指向 DATA 的指针 
         if(mChangeToSrc == XBH_SOURCE_OPS1)
         {
              setIICData(GSV2705_I2C_BUS, XBH_GSV_DEVICE_ADDRESS, HPD_REGISTER_ADDRESS, HPD_REGISTER_LEN, DATA_LENGTH, pData2);//拉高OPS1 的HPD
              usleep(1000 *1000);
              setIICData(GSV6712_I2C_BUS, XBH_GSV_DEVICE_ADDRESS, HPD_REGISTER_ADDRESS, HPD_REGISTER_LEN, DATA_LENGTH, pData3); //拉低OPS2 的HPD
         }
    }
    XLOGD("run u32Source:%d\n", mChangeToSrc);
}

XBH_S32 XbhXMR3588_U::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_HALL_CHAN, &adcValue);

    if(adcValue > XBH_BOARD_HALL_SENSOR_THRESHOLD && adcValue < XBH_BOARD_ADC_HALL_CHAN_VALUE)
    {
        *s32Value = XBH_BOARD_HALL_ONE_PEN;   //当只有一支笔,不管是A笔还是B笔,范围都在1300<adcValue<2500
    }
    else if(adcValue > XBH_BOARD_ADC_HALL_CHAN_VALUE)
    {
        *s32Value = XBH_BOARD_HALL_TWO_PENS;   //当两只笔都放上,adcValue=4095
    }
    else
    {
        *s32Value = XBH_BOARD_HALL_ZERO_PENS;   //当没有笔放上,adcValue=1093
    }

    return s32Ret;
}

XBH_S32 XbhXMR3588_U::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 ret = XbhRk_3576::setHdmiLowLatency(bEnable);
    return ret;
}

XBH_S32 XbhXMR3588_U::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
   XBH_S32 s32Ret = XBH_FAILURE;
    //XLOGD("set Hdmi Tx Info enType = %d",enType);
    //根据project_id 更新 HDMI switch 的edid 从而达到切换edid type的作用
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    XBH_S32 type  = 2;
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"0");
            if(projectid == 0)
            {
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID1_v1_4_2K,0);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID2_v1_4_2K,1);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v1_4_2K,2);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID4_v1_4_2K,3);
                setHdmiRx5vDet(XBH_TRUE);
                XLOGD("setHdmirxEdidType edid 1.4 \n");
            }
            else
            {
                char edid20BinFilePath[128] = {0};
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,0);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,1);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,2);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,3);
                setHdmiRx5vDet(XBH_TRUE);
            }
            type = 1;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_20:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"1");
            if(projectid == 0)
            {
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID1_v2_0_4K_2K,0);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID2_v2_0_4K_2K,1);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v2_0_4K_2K,2);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID4_v2_0_4K_2K,3);
                setHdmiRx5vDet(XBH_TRUE);
                XLOGD("setHdmirxEdidType edid 2.0 \n");
            }
            else
            {
                char edid20BinFilePath[128] = {0};
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,0);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,1);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,2);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,3);
                setHdmiRx5vDet(XBH_TRUE);
            }
            type = 2;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_21:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"2");
            type = 3;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"3");
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

XBH_S32 XbhXMR3588_U::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type  = 2;
    s32Ret = XbhRk_3576::getHdmirxEdidType(&type);
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

/**
 * 读取预置EDID的bin文件值，并通过Edidtype更新EDID的bin数据到HDMI switch
 * @param projecitd
 * @param edidBinFilePath edidbin文件存储路径
 * @param node edidbin文件存储节点
 */
XBH_S32 XbhXMR3588_U::readAndUpdateEdidBinFileByEdidType(int dev,int projectid,const char* edidBinFilePath,int port)
{
    XLOGD("projectid=%d,edidBinFilePath=%s\n",projectid,edidBinFilePath);
    // ==================== 第一部分：读取二进制文件 ====================
    FILE *src_file = nullptr;
    unsigned char *file_buffer = nullptr; // 使用无符号类型便于十六进制打印
    // 打开源文件
    if (!(src_file = fopen(edidBinFilePath, "rb")))
    {
        XLOGD("打开源文件失败");
        return EXIT_FAILURE;
    }
    // 获取文件大小
    struct stat file_stat;
    if (fstat(fileno(src_file), &file_stat) == -1)
    {
        XLOGD("获取文件大小失败");
        fclose(src_file);
        return EXIT_FAILURE;
    }
    const size_t file_size = file_stat.st_size;
    // 分配内存缓冲区
    if (!(file_buffer = static_cast<unsigned char*>(malloc(file_size))))
    {
        XLOGD("内存分配失败");
        fclose(src_file);
        return EXIT_FAILURE;
    }
    // 读取文件内容
    if (fread(file_buffer, 1, file_size, src_file) != file_size)
    {
        XLOGD("❌ 文件读取不完整");
        free(file_buffer);
        fclose(src_file);
        return EXIT_FAILURE;
    }
    fclose(src_file);
    // ==================== 数据打印模块 ====================
    // for (size_t i = 0; i < file_size; ++i) {
    //     // 每16字节换行
    //     if ( (i+1) % 16 == 0) {
    //         XLOGD("i=%d,file_buffer[i]=%02X", i,file_buffer[i]);
    //     }
    // }

    // ==================== 第二部分：更新EDID ====================
    XbhHdmiSwitchManager::getInstance()->updateEdid(dev,file_buffer,port);
    // ==================== 资源清理 ====================
    usleep(10*1000);
    //free(file_buffer);
    return EXIT_SUCCESS;
}

/**
 * 读取预置EDID的bin文件值，并通过Edidtype更新EDID的bin数据到HDMI switch
 * @param projecitd
 */
XBH_S32 XbhXMR3588_U::readAndUpdateEdidBinFile(int projectid)
{
    //XLOGD("readAndUpdateEdidBinFile projectid=%d\n",projectid);
    if(projectid > 0)//分别读取4个bin文件，对应不同端口的edid
    {
        char edid20BinFilePath[128] = {0};
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,0);
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,1);
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,2);
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1,projectid,edid20BinFilePath,3);
    }
    return XBH_SUCCESS;
}

/**
 * 初始化project_id配置
 */
XBH_S32 XbhXMR3588_U::initProjectIdConfig()
{
    //每次开机都需要更新edid的数据
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    //projectid = property_get_int32("persist.vendor.xbh.project_id_test", 0);
    if(projectid == 0)//默认edid从xbh_edid.h（CUSTOM目录转化而来）中读取
    {
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID1_v2_0_4K_2K,0);
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID2_v2_0_4K_2K,1);
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v2_0_4K_2K,2);
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID4_v2_0_4K_2K,3);
    }
    else//其它projectid 从bin文件中获取
    {
        readAndUpdateEdidBinFile(projectid);
    }
    return XBH_SUCCESS;
}

/**
* 获取HDMI RX lock的状态
* retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3588_U::getHdmiRxLockStatus(XBH_BOOL* lock)
{

    XBH_S32 s32Ret = XBH_SUCCESS;
    *lock = XBH_TRUE;
    return s32Ret;
}