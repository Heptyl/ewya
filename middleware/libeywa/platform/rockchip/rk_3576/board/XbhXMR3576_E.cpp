#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMR3576_E"

#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhXMR3576_E.h"
#include "XbhSysOpt.h"
#include "XbhSourceManager.h"
#include "XbhPartitionData.h"
#include "XbhRk_3576.h"
#include "XbhI2cSimulator.h"
#include <hardware/xbh_edid.h>
#include "XbhKernelMsgManager.h"
#include "XbhVgaEdidManager.h"
#include "XbhSysOpt.h"

XbhXMR3576_E            *XbhXMR3576_E::mInstance = XBH_NULL;
XbhMutex                 XbhXMR3576_E::mLock;
XBH_BOOL                 XbhXMR3576_E::mOTGMode = XBH_FALSE;

#define TAS5805M_INIT_STATUS    "/proc/tas5805m/tas5805m_status"
#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"
#define HDMI_RX_EDID            "/sys/class/hdmirx/rk628/edid_index"

#define XBH_USBC_NET            1   //跟随到板载type-c
#define XBH_FUSBC_NET           2   //跟随到前置type-c
#define XBH_EEPROM_ADDR         0xA0   //eeprom的设备地址

const  XBH_U32           XbhXMR3576_E::M_PS_ON_BASE_DELAY_TIME(700);
const  XBH_U32           XbhXMR3576_E::M_PS_ON_DELAY_TIME_STEP(200);
const  XBH_U32           XbhXMR3576_E::M_PWR_STA_DECT_TIMES(3000);

void XbhXMR3576_E::run(const void *arg)
{
    #ifdef XBH_BOARD_GPIO_OPS_PS_ON
    XBH_S32 staTimes = 3;
    if(mOpsOnFlag)
    {
        mOpsOnFlag = XBH_FALSE;
        XBH_U32 u32OpspowerTimer = M_PS_ON_BASE_DELAY_TIME;
        XBH_U32 u32Ops12VTimer = 200;
        XBH_BOOL bIsDet = XBH_FALSE;
        while(staTimes--)
        {
            XBH_BOOL bEnable;
            setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
            usleep(u32OpspowerTimer * 1000);
            setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
            if (u32OpspowerTimer >= (M_PS_ON_BASE_DELAY_TIME + (M_PS_ON_DELAY_TIME_STEP * 2)))
            {
                break;
            }
            usleep(M_PWR_STA_DECT_TIMES*1000);
            getOpsPowerStatus(&bEnable, XBH_SOURCE_OPS1);
            if (bEnable)
            {
                break;
            }
            else
            {
                u32OpspowerTimer += M_PS_ON_DELAY_TIME_STEP;
            }
        }
    }
    #endif
    /**
     * VTS测试时，需要HDMI TX和DP TX输出不同的分辨率位于两个VP上面，因此需要将DP TX的分辨率设置为1920x1080
     * 将HDMI RX的EDID设置为1.4，再将HDMI OUT接入到HDMI1上，DP TX就会输出1920x1080
    */
    char prop[PROPERTY_VALUE_MAX] = {0};
    property_get("vendor.vts_test", prop, "false");
    XBH_HDMIRX_EDID_TYPE_E type;
    if(strcmp(prop, "true") == 0)
    {
        getSourceEdidType(XBH_SOURCE_HDMI1, &type);
        if(type != XBH_HDMIRX_EDID_TYPE_14)
        {
            setSourceEdidType(XBH_HDMIRX_EDID_TYPE_14, XBH_SOURCE_HDMI1);
            setSourceEdidType(XBH_HDMIRX_EDID_TYPE_14, XBH_SOURCE_HDMI2);
        }
    }
    usleep(500 * 1000);//500ms
}

XbhXMR3576_E::XbhXMR3576_E()
{
    XLOGD(" begin ");
    if(isOTGMode() == XBH_TRUE)
    {
        #ifdef XBH_BOARD_GPIO_USBC_1_DET
        XLOGI("%s,set USBC to otg",__func__);
        setGpioOutputValue(XBH_BOARD_GPIO_USBC_1_DET, XBH_BOARD_GPIO_USBC_1_DET_LEVEL);
        #endif
    }
    setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, XBH_BOARD_GPIO_HP_MUTE_LEVEL);//HP MUTE
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhXMR3576_E::~XbhXMR3576_E()
{
}

XBH_BOOL XbhXMR3576_E::isOTGMode(XBH_VOID){
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_USBC_CONFIG_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getUsbcConfig(s8Buff);
    XLOGI("%s,s8Buff[0]:%d",__func__,s8Buff[0]);
    if(s32Ret == XBH_SUCCESS)
    {
        mOTGMode = (s8Buff[0] == 1)?XBH_TRUE:XBH_FALSE;
    }
    return mOTGMode;
}
XBH_S32 XbhXMR3576_E::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_2_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mOTGMode == XBH_FALSE)
    {
        #ifdef XBH_BOARD_GPIO_USBC_1_DET
        XBH_U32 u32GpioValue;
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
        *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8State;
    s32Ret = XbhVga2HdmiManager::getInstance()->getCurVgaStatus(XBH_SOURCE_VGA1, &u8State);
    if (u8State == 0x85) //STATE_NO_INPUT_SIGNAL
    {
        *enable = XBH_FALSE;
    }
    else if (u8State == 0x00 || u8State == 0x84) //STATE_SUCCESS
    {
        *enable = XBH_TRUE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (enSource == XBH_SOURCE_OPS1)
    {
        mOpsOnFlag = XBH_TRUE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_E::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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


XBH_S32 XbhXMR3576_E::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_E::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMR3576_E::getAirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAirSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
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

XBH_S32 XbhXMR3576_E::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL bFlag = XBH_FALSE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:  
        {   
            if (bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, XBH_BOARD_GPIO_HP_MUTE_LEVEL);
            }
            else
            {
                getHpDetectStatus(&bFlag);
                if(bFlag)
                {
                    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, !XBH_BOARD_GPIO_HP_MUTE_LEVEL);
                }
                else
                {
                    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, XBH_BOARD_GPIO_HP_MUTE_LEVEL);//没有插入耳机时需要关闭输出，防止影响ADC
                    XLOGD("hp is not insert !!!!!! ");
                }
            }
            break;
        }  
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

XBH_S32 XbhXMR3576_E::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, &u32GpioValue);
            *bEnable =  u32GpioValue == XBH_BOARD_GPIO_HP_MUTE_LEVEL ? XBH_TRUE : XBH_FALSE;
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
XBH_S32 XbhXMR3576_E::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
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
XBH_S32 XbhXMR3576_E::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
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

XBH_S32 XbhXMR3576_E::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    *status = XBH_FALSE;//No Anolog MIC
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

    //init value
    *status = XBH_FALSE;

#ifdef XBH_BOARD_ADC_HP_DET_CHAN
    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_HP_DET_CHAN, &adcValue);
    #if (XBH_BOARD_ADC_HP_VAL_INVERT)
    if(adcValue < XBH_BOARD_ADC_HP_PLUGIN_VAL)
    {
        *status = XBH_TRUE;
    }
    #else
    if(adcValue > XBH_BOARD_ADC_HP_PLUGIN_VAL)
    {
        *status = XBH_TRUE;
    }
    #endif
#endif

    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std :: string result;

    XbhSysOpt::getInstance()->readSysfs(TAS5805M_INIT_STATUS, result);
    if (!result.empty())
    {   
        *value =  atoi(result.c_str());
        XLOGD("%s: %d ampInitStatus:%d!\n", __FUNCTION__, __LINE__, *value);
        s32Ret = XBH_SUCCESS;     
    }
    return s32Ret; 
}

XBH_S32 XbhXMR3576_E::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt1 = 0;
    XBH_U32 u32GpioPirInt2 = 0;

#ifdef XBH_BOARD_GPIO_PIR_INT1
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT1,&u32GpioPirInt1);
#endif

#ifdef XBH_BOARD_GPIO_PIR_INT2
    s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_PIR_INT2,&u32GpioPirInt2);
#endif

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

XBH_S32 XbhXMR3576_E::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

#ifdef XBH_BOARD_ADC_HALL_CHAN
    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_HALL_CHAN, &adcValue);

    if(adcValue < XBH_BOARD_ADC_HALL_CHAN_VALUE)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;
    }
#endif

    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_ITE6563OUT, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2715_1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_ITE6563OUT, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2715_1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename,force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        default:
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_ITE6563OUT, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2715_1, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::setHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E hdcpTxEncryptionMode)
{
    switch(hdcpTxEncryptionMode)
    {
        case XBH_HDCPTX_ENCRYPTION_1P4:
            XLOGD("[%s,%d] set hdcp encryption mode to 1.4", __FUNCTION__, __LINE__);
            setGpioOutputValue(XBH_BOARD_GPIO_DPOUT_HDMI_CHANNEL_CTRL, XBH_BOARD_GPIO_HIGH);
        break;

        case XBH_HDCPTX_ENCRYPTION_2P2:
            XLOGD("[%s,%d] set hdcp encryption mode to 2.2", __FUNCTION__, __LINE__);
            setGpioOutputValue(XBH_BOARD_GPIO_DPOUT_HDMI_CHANNEL_CTRL, XBH_BOARD_GPIO_LOW);
        break;
        
        default:
            XLOGE("[%s,%d] unknown hdcp encryption mode!", __FUNCTION__, __LINE__);
        break;
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::getHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E *hdcpTxEncryptionMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_DPOUT_HDMI_CHANNEL_CTRL, &u32GpioValue);
    switch (u32GpioValue)
    {
        case XBH_BOARD_GPIO_HIGH:
            *hdcpTxEncryptionMode = XBH_HDCPTX_ENCRYPTION_1P4;
            break;
        case XBH_BOARD_GPIO_LOW:
            *hdcpTxEncryptionMode = XBH_HDCPTX_ENCRYPTION_2P2;
            break;
        default:
            s32Ret = XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhXMR3576_E::openEthWol(XBH_BOOL bEnable)
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

XBH_S32 XbhXMR3576_E::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{

    XLOGD("getSrcDetByDetGpio gpio = %d u32GpioValue = %d\n", gpio ,u32GpioValue);
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_SOURCE_E currSource;
    s32Ret = XbhSourceManager::getInstance()->getCurrSource(&currSource);
    XBH_BOOL isSpeakerMute = XBH_FALSE;
    XBH_S32 result = getMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, &isSpeakerMute);

    #if 1
    switch(gpio)
    {
        #ifdef XBH_BOARD_GPIO_HDMI_1_DET
        case XBH_BOARD_GPIO_HDMI_1_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI1;
            if(u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
                *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
            }
            break;
        #endif

        #ifdef XBH_BOARD_GPIO_HDMI_2_DET
        case XBH_BOARD_GPIO_HDMI_2_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI2;
            if(u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL)
            {
                *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI2)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
                *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI2)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
            }
            break;
        #endif

        #ifdef XBH_BOARD_GPIO_USBC_1_DET
        case XBH_BOARD_GPIO_USBC_1_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_USBC1;
            if(u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_USBC1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
                *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_USBC1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
            }
            break;
        #endif

        #ifdef XBH_BOARD_GPIO_F_USBC_DET
        case XBH_BOARD_GPIO_F_USBC_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_F_USBC1;
            if(u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL)
            {
                *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_USBC1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
                *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_USBC1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
            }
            break;
        #endif

        #ifdef XBH_BOARD_GPIO_F_HDMI_DET
        case XBH_BOARD_GPIO_F_HDMI_DET:
           *src = XBH_SOURCE_E::XBH_SOURCE_F_HDMI1;
           if(u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL)
           {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_HDMI1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
           }
           else
           {
               *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_HDMI1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
           }
           break;
        #endif

        #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
        case XBH_BOARD_GPIO_DP_1_IN_DET_PIN:
        //#444371 【通道功能测试】拔掉DP通道信号线再插入，画面显示无信号
        //DP通道时，稍微动一下插头/抬一下线会无信号，看log发现中断出现，因此将DP改为轮询，中断注释掉
           *src = XBH_SOURCE_E::XBH_SOURCE_DP1;
           if(u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL)
           {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_DP1)
               {
                   //setHdmiRx5vDet(XBH_TRUE);
               }
           }
           else
           {
               *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_DP1)
               {
                   //setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
           }
           break;
        #endif

        #ifdef XBH_BOARD_GPIO_VGA_1_DET
        case XBH_BOARD_GPIO_VGA_1_DET:
           *src = XBH_SOURCE_E::XBH_SOURCE_VGA1;
           if(u32GpioValue == XBH_BOARD_GPIO_VGA_1_DET_LEVEL)
           {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_VGA1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
           }
           else
           {
               *bEnable = XBH_FALSE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_VGA1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
           }
           break;
        #endif

        default:
            *src = XBH_SOURCE_E::XBH_SOURCE_BUTT;
            break;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::setHdmiRx5vDet(XBH_BOOL enable)
{
    XLOGD("setHdmiRx5vDetFlag state:%d\n", enable);
    XBH_S32 s32Ret = XBH_SUCCESS;
    XbhRk_3576::setHdmiRx5vDet(enable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    XbhRk_3576::setOnStop();
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);

    freeAllGpioIrq();
    //CEC 唤醒时需要使能CEC引脚的中断
    if(s8Buff[XBH_WAKEUP_CEC])
    {
        requestGpioIrq(XBH_BOARD_GPIO_HDMI_CEC, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING);
    }

    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);

    #ifdef XBH_WAKE_UP_POWER_KEY
    requestGpioIrq(XBH_WAKE_UP_POWER_KEY, XBH_WAKE_UP_POWER_KEY_TYPE);
    #endif

    if (s8Buff[XBH_WAKEUP_SOURCE])
    {
        XLOGD("Source wake is open");

        //判断前置板是否使用GSV芯片，如果是则在关机时然GVS芯片进入待机模式，也就是开启前置板typec信源唤醒
        char dev_type[PROPERTY_VALUE_MAX] = {0};
        property_get("persist.vendor.xbh.fboard.type", dev_type, "0");
        if(strcmp(dev_type, "0x50") == 0)
        {
            XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        }
        
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_BOARD_GSV2715_1);
        #ifdef XBH_WAKE_UP_HDMI1_DET
        requestGpioIrq(XBH_WAKE_UP_HDMI1_DET, XBH_WAKE_UP_HDMI1_DET_TYPE);
        #endif

        #ifdef XBH_WAKE_UP_HDMI2_DET
        requestGpioIrq(XBH_WAKE_UP_HDMI2_DET, XBH_WAKE_UP_HDMI2_DET_TYPE);
        #endif

        #ifdef XBH_WAKE_UP_DP_DET
        requestGpioIrq(XBH_WAKE_UP_DP_DET, XBH_WAKE_UP_DP_DET_TYPE);
        #endif

        #ifdef XBH_WAKE_UP_F_HDMI_DET
        requestGpioIrq(XBH_WAKE_UP_F_HDMI_DET, XBH_WAKE_UP_F_HDMI_DET_TYPE);
        #endif

        #ifdef XBH_WAKE_UP_TYPE_C_DET
        requestGpioIrq(XBH_WAKE_UP_TYPE_C_DET, XBH_WAKE_UP_TYPE_C_DET_TYPE);
        #endif

        #ifdef XBH_WAKE_UP_F_TYPE_C_DET
        requestGpioIrq(XBH_WAKE_UP_F_TYPE_C_DET, XBH_WAKE_UP_F_TYPE_C_DET_TYPE);
        #endif
        
        #ifdef XBH_WAKE_UP_VGA1_DET
        requestGpioIrq(XBH_WAKE_UP_VGA1_DET, XBH_WAKE_UP_VGA1_DET_TYPE);
        #endif

        if(s8Buff[XBH_WAKEUP_BTYPEC] == 0)
        {
           #ifdef XBH_BOARD_GPIO_PD_POWER
           setGpioOutputValue(XBH_BOARD_GPIO_PD_POWER, XBH_BOARD_GPIO_PD_POWER_OFF);
           #endif
        }
        if(s8Buff[XBH_WAKEUP_FTYPEC] == 0)
        {
           XbhKernelMsgManager::getInstance()->sendMsgToKernel(2, 23, 0, 0, 0);
        }
    }
    else
    {
        #ifdef XBH_BOARD_GPIO_PD_POWER
        setGpioOutputValue(XBH_BOARD_GPIO_PD_POWER, XBH_BOARD_GPIO_PD_POWER_OFF);
        #endif
        XbhKernelMsgManager::getInstance()->sendMsgToKernel(2, 23, 0, 0, 0);
    }

    if (s8Buff[XBH_WAKEUP_ETH])
    {
        XLOGD("eth wake is open");
        openEthWol(XBH_TRUE);
        #ifdef XBH_WAKE_UP_ETH
        requestGpioIrq(XBH_WAKE_UP_ETH, XBH_WAKE_UP_ETH_TYPE);
        #endif
    }
    else
    {
        #ifdef XBH_BOARD_GPIO_ETH_PWR
        setGpioOutputValue(XBH_BOARD_GPIO_ETH_PWR, XBH_BOARD_GPIO_ETH_PWR_OFF);
        #endif
    }

    if (s8Buff[XBH_WAKEUP_PIR])
    {
        XLOGD("pir wake is open");
        #ifdef XBH_WAKE_UP_PIR1
        requestGpioIrq(XBH_WAKE_UP_PIR1, XBH_WAKE_UP_PIR1_TYPE);
        #endif

        #ifdef XBH_WAKE_UP_PIR2
        requestGpioIrq(XBH_WAKE_UP_PIR2, XBH_WAKE_UP_PIR2_TYPE);
        #endif
    }

    #ifdef XBH_BOARD_GPIO_NFC_EN
    setGpioOutputValue(XBH_BOARD_GPIO_NFC_EN, XBH_BOARD_GPIO_NFC_EN_LEVEL);
    #endif

    #ifdef XBH_BOARD_GPIO_SD_PWR
    setGpioOutputValue(XBH_BOARD_GPIO_SD_PWR,XBH_BOARD_GPIO_LOW);
    #endif
    if (s8Buff[XBH_WAKEUP_NFC])
    {
        XLOGD("nfc wake is open");
        #ifdef XBH_WAKE_UP_MCU_IRQ
        requestGpioIrq(XBH_WAKE_UP_MCU_IRQ, XBH_WAKE_UP_MCU_IRQ_TYPE);
        #endif

        // 进入待机后关闭部分mcu引脚
        #ifdef XBH_BOARD_GPIO_ADC_RESET
        setGpioOutputValue(XBH_BOARD_GPIO_ADC_RESET, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_TOUCH_DEV_PWR
        setGpioOutputValue(XBH_BOARD_GPIO_TOUCH_DEV_PWR, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_HP_MUTE
        setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_OPS_PWREN
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_F_HUB_HUB_RST
        setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_HUB_RST, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_DPOUT_HDMI_RESET
        setGpioOutputValue(XBH_BOARD_GPIO_DPOUT_HDMI_RESET, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_HUB_HUB20_UU40_RST
        setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU40_RST, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_SW_F_GPIO0_SEL
        setGpioOutputValue(XBH_BOARD_GPIO_SW_F_GPIO0_SEL, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_FRONT_PROT_DEV_PWR
        setGpioOutputValue(XBH_BOARD_GPIO_FRONT_PROT_DEV_PWR, XBH_BOARD_GPIO_LOW);
        #endif

        #ifdef XBH_BOARD_GPIO_OPS_DEVICE_RST
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_DEVICE_RST, XBH_BOARD_GPIO_LOW);
        #endif  

        #ifdef XBH_BOARD_GPIO_WIFI_RESET
        setGpioOutputValue(XBH_BOARD_GPIO_WIFI_RESET, XBH_BOARD_GPIO_LOW);
        #endif  

        #ifdef XBH_BOARD_GPIO_HUB_HUB20_UU4_RST
        setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU4_RST, XBH_BOARD_GPIO_LOW);
        #endif  

        #ifdef XBH_BOARD_GPIO_F_HUB30_RST
        setGpioOutputValue(XBH_BOARD_GPIO_F_HUB30_RST, XBH_BOARD_GPIO_LOW);
        #endif
    }
    else
    {
        #ifdef XBH_BOARD_GPIO_MCU_PWR
        setGpioOutputValue(XBH_BOARD_GPIO_MCU_PWR, !XBH_BOARD_GPIO_MCU_PWR_ON);
        #endif
    }

    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhXMR3576_E::initSrcDetGpioIrq()
{
    XBH_S32 s32Ret = XBH_FAILURE;
#if 1
    #ifdef XBH_BOARD_GPIO_HDMI_1_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_HDMI_1_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    #endif

    #ifdef XBH_BOARD_GPIO_HDMI_2_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_HDMI_2_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    #endif

    #ifdef XBH_BOARD_GPIO_F_HDMI_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_F_HDMI_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    #endif

    #ifdef XBH_BOARD_GPIO_F_USBC_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_F_USBC_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    #endif

    #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT,XBH_TRUE);
    #endif

    #ifdef XBH_BOARD_GPIO_VGA_1_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_VGA_1_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT,XBH_FALSE);
    #endif

    if(mOTGMode == XBH_FALSE)
    {
        #ifdef XBH_BOARD_GPIO_USBC_1_DET
        s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_USBC_1_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT,XBH_FALSE);
        #endif
    }
   #endif
    return  s32Ret;
}
XBH_S32 XbhXMR3576_E::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    if(enSource == XBH_SOURCE_USBC1)
    {
        ret = XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_BOARD_GSV2715_1, pdability);
    }
    else if(enSource == XBH_SOURCE_F_USBC1)
    {
        ret = XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_FRONT_GSV2712, pdability);
    }
    else
    {
        XLOGE("The 'enSource' parameter is invalid.\n");
        return XBH_FAILURE;
    }
    return ret;
}
XBH_S32 XbhXMR3576_E::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    #ifdef XBH_BOARD_GPIO_FRONT_PROT_DEV_PWR
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_FRONT_PROT_DEV_PWR, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_FRONT_PROT_DEV_PWR_ON : XBH_BOARD_GPIO_FRONT_PROT_DEV_PWR_OFF);
    #endif

    // sider usb port
    #ifdef XBH_BOARD_GPIO_AN_PROT_DEV_PWR
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_AN_PROT_DEV_PWR, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_AN_PROT_DEV_PWR_ON : XBH_BOARD_GPIO_AN_PROT_DEV_PWR_OFF);
    #endif

    #ifdef XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_ON : XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_OFF);
    #endif

    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getUsbLockMode(XBH_U32 *u32UsbType)
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
XBH_S32 XbhXMR3576_E::setAvcEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhXMR3576_E::getAvcEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getAvcEnable(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::autoAdjust(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->vgaAutoAdjust(XBH_SOURCE_VGA1);
    return s32Ret;
}
//override
XBH_S32 XbhXMR3576_E::setHPosition(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->setVgaHPosition(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::getHPosition(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->getVgaHPosition(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::setVPosition(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->setVgaVPosition(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::getVPosition(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->getVgaVPosition(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::setClock(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->setVgaClock(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::getClock(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->getVgaClock(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::setPhase(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->setVgaPhase(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMR3576_E::getPhase(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVga2HdmiManager::getInstance()->getVgaPhase(XBH_SOURCE_VGA1, u32Value);
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 ret = XbhRk_3576::setHdmiLowLatency(bEnable);
    return ret;
}

XbhXMR3576_E *XbhXMR3576_E::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMR3576_E();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMR3576_E::setWifiChanelPlan(XBH_WIFI_TYPE wifiType, const XBH_CHAR* code)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std :: string chanelPlanPath;
    switch (wifiType)
    {
        case XBH_RTL8852BE:
            chanelPlanPath = "/proc/net/rtl8852be/wlan0/chan_plan";
            break;
        default:
            chanelPlanPath = "";
            break;
    }
    std :: string result;
    XbhSysOpt::getInstance()->readSysfs(chanelPlanPath, result);

    if (!result.empty())
    {
        XLOGD("%s: %d result:%s!\n", __FUNCTION__, __LINE__, result.c_str());
        XbhSysOpt::getInstance()->writeSysfs(chanelPlanPath, code);
    }
    else
    {
        XLOGE("%s: %d result empty!\n", __FUNCTION__, __LINE__);
        return s32Ret;
    }
    s32Ret = XBH_SUCCESS;
    return s32Ret;
}
/*
XBH_S32 XbhXMR3576_E::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXA, XBH_HDMI1_CEC_PHYSICAL_ADDR);
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXB, XBH_HDMI2_CEC_PHYSICAL_ADDR);
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXC, XBH_HDMI3_CEC_PHYSICAL_ADDR);
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXD, XBH_HDMI4_CEC_PHYSICAL_ADDR);
    
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2715_1, XBH_HDMI_RXA, XBH_HDMI1_CEC_PHYSICAL_ADDR);
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2715_1, XBH_HDMI_RXB, XBH_HDMI2_CEC_PHYSICAL_ADDR);
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2715_1, XBH_HDMI_RXC, XBH_HDMI3_CEC_PHYSICAL_ADDR);
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2715_1, XBH_HDMI_RXD, XBH_HDMI4_CEC_PHYSICAL_ADDR);
    if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT3_U5_BOARD)
    {
        s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_FRONT_GSV2712, XBH_HDMI_RXA, XBH_HDMI3_CEC_PHYSICAL_ADDR);
        s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_FRONT_GSV2712, XBH_HDMI_RXB, XBH_HDMI1_CEC_PHYSICAL_ADDR);
    }
    return s32Ret;
}
*/

/**
 * 读取预置EDID的bin文件值，并通过Edidtype更新EDID的bin数据到HDMI switch
 * @param projecitd
 */
XBH_S32 XbhXMR3576_E::readAndUpdateEdidBinFile(int projectid)
{
    if(projectid > 0)//分别读取4个bin文件，对应不同端口的edid
    {
        char edid20BinFilePath[128] = {0};
        XBH_HDMIRX_EDID_TYPE_E type;
        //当VGA EDID没有更新时需要更新
        if(property_get_int32("persist.vendor.xbh.vgaedid.update", 0) == 0)
        {
            property_set("persist.vendor.xbh.vgaedid.update", "1");
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/EDID_VGA.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_MS9282, edid20BinFilePath, XBH_HDMI_RXA);
        }
        getSourceEdidType(XBH_SOURCE_HDMI1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXA);//hdmi1
        }
        else
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXA);//hdmi1
        }
        getSourceEdidType(XBH_SOURCE_HDMI2, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXB);//hdmi2
        }
        else
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXB);//hdmi2
        }
        getSourceEdidType(XBH_SOURCE_DP1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXC);//dp
        }
        else
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXC);//dp
        }
        getSourceEdidType(XBH_SOURCE_USBC1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID7_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXD);//typec
        }
        else
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID7_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXD);//typec
        }
        getSourceEdidType(XBH_SOURCE_OPS1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, XBH_HDMI_RXA);//ops
        }
        else
        {
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, XBH_HDMI_RXA);//ops
        }
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, XBH_HDMI_RXB);//front
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, XBH_HDMI_RXC);//vga
        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v2.0_4K_2K.bin", projectid);
        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, XBH_HDMI_RXD);//switch

        if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT3_U5_BOARD)
        {
            getSourceEdidType(XBH_SOURCE_F_HDMI1, &type);
            if(type == XBH_HDMIRX_EDID_TYPE_14)
            {
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid20BinFilePath, XBH_HDMI_RXA);//front hdmi
            }
            else
            {
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid20BinFilePath, XBH_HDMI_RXA);//front hdmi
            }
            getSourceEdidType(XBH_SOURCE_F_USBC1, &type);
            if(type == XBH_HDMIRX_EDID_TYPE_14)
            {
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID6_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid20BinFilePath, XBH_HDMI_RXB);//front typec
            }
            else
            {
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID6_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid20BinFilePath, XBH_HDMI_RXB);//front typec
            }
        }
    }
    return XBH_SUCCESS;
}

/**
 * 初始化project_id配置
 */
XBH_S32 XbhXMR3576_E::initProjectIdConfig()
{
    //每次开机都需要更新edid的数据
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    XLOGD("initProjectIdConfig projectid=%d \n", projectid);
    XBH_HDMIRX_EDID_TYPE_E type;
    //projectid = property_get_int32("persist.vendor.xbh.project_id_test", 0);
    if(projectid == 0)//默认edid从xbh_edid.h（CUSTOM目录转化而来）中读取
    {
        //当VGA EDID没有更新时需要更新
        if(property_get_int32("persist.vendor.xbh.vgaedid.update", 0) == 0)
        {
            property_set("persist.vendor.xbh.vgaedid.update", "1");
            XbhVgaEdidManager::getInstance()->setVgaEdidData(EDID_VGA, XBH_SOURCE_VGA1);
        }
        getSourceEdidType(XBH_SOURCE_HDMI1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID1_v1_4_2K, XBH_HDMI_RXA);//hdmi1
        }
        else
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID1_v2_0_4K_2K, XBH_HDMI_RXA);//hdmi1
        }
        getSourceEdidType(XBH_SOURCE_HDMI2, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID2_v1_4_2K, XBH_HDMI_RXB);//hdmi2
        }
        else
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID2_v2_0_4K_2K, XBH_HDMI_RXB);//hdmi2
        }
        getSourceEdidType(XBH_SOURCE_DP1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID4_v1_4_2K, XBH_HDMI_RXC);//dp
        }
        else
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID4_v2_0_4K_2K, XBH_HDMI_RXC);//dp
        }        
        getSourceEdidType(XBH_SOURCE_USBC1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID7_v1_4_2K, XBH_HDMI_RXD);//typec
        }
        else
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID7_v2_0_4K_2K, XBH_HDMI_RXD);//typec
        }  
        getSourceEdidType(XBH_SOURCE_OPS1, &type);
        if(type == XBH_HDMIRX_EDID_TYPE_14)
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v1_4_2K, XBH_HDMI_RXA);//ops
        }
        else
        {
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v2_0_4K_2K, XBH_HDMI_RXA);//ops
        }
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v2_0_4K_2K, XBH_HDMI_RXB);//front
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v2_0_4K_2K, XBH_HDMI_RXC);//vga
        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v2_0_4K_2K, XBH_HDMI_RXD);//switch
        
        if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT3_U5_BOARD)
        {
            getSourceEdidType(XBH_SOURCE_F_HDMI1,&type);
            if(type == XBH_HDMIRX_EDID_TYPE_14)
            {
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID3_v1_4_2K, XBH_HDMI_RXA);//front hdmi
            }
            else
            {
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID3_v2_0_4K_2K, XBH_HDMI_RXA);//front hdmi
            }
            getSourceEdidType(XBH_SOURCE_F_USBC1,&type);
            if(type == XBH_HDMIRX_EDID_TYPE_14)
            {
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID6_v1_4_2K, XBH_HDMI_RXB);//front typec
            }
            else
            {
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID6_v2_0_4K_2K, XBH_HDMI_RXB);//front typec
            }
        }
    }
    else//其它projectid 从bin文件中获取
    {
        readAndUpdateEdidBinFile(projectid);
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length) 
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL, XBH_EEPROM_ADDR);
    if (pSimulator == nullptr)
    {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Write_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL, XBH_EEPROM_ADDR);
    if (pSimulator == nullptr)
    {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Read_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_S32 XbhXMR3576_E::getAntennaRssi(XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std :: string result;
    XbhSysOpt::getInstance()->writeSysfs("/proc/net/rtl8852be/wlan0/odm/phl_cmd","phl asoc_sta 1 0");
    XbhSysOpt::getInstance()->readSysfs("/proc/net/rtl8852be/wlan0/odm/phl_cmd", result);
    XbhSysOpt::getInstance()->writeSysfs("/proc/net/rtl8852be/wlan0/odm/phl_cmd","btc show");
    XbhSysOpt::getInstance()->readSysfs("/proc/net/rtl8852be/wlan0/odm/phl_cmd", result);

    std :: string antennaRssiPath = "/proc/net/rtl8852be/wlan0/odm/wifi_detect";
    result.clear();
    XbhSysOpt::getInstance()->readSysfs(antennaRssiPath, result);
    if (!result.empty())
    {
        XLOGD("%s: %d result:%s!\n", __FUNCTION__, __LINE__, result.c_str());
        strcpy(ver, result.c_str());
        s32Ret = XBH_SUCCESS;
    }
    else
    {
        XLOGE("%s: %d result empty!\n", __FUNCTION__, __LINE__);
    }

    return s32Ret;
}

/**
 * FATT烧录工厂SN
 * param[in] materialType 物料类型  0: 底板(主板)SN  1:核心板SN 
 * param[in] snInfo 写入数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3576_E::burnFactorySN(XBH_MATERAL_TYPE materialType, const XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32Ret1 = XBH_SUCCESS;
    XBH_S32 s32Ret2 = XBH_SUCCESS;
    XBH_S32 s32Ret3 = XBH_SUCCESS;
    XBH_S32 s32Ret4 = XBH_SUCCESS;
    XBH_CHAR sn[XBH_CUSDATA_BARCODE_LEN + 1] = {0};
    XBH_CHAR pn[XBH_CUSDATA_SMART_SCALER_PN_LEN + 1] = {0};

    // 查找第一个出现的 '-' 或 '_'
    std::string snInfoStr(snInfo);
    size_t delimiterPos = snInfoStr.find_first_of("-_");
    if (delimiterPos == std::string::npos) 
    {
        return XBH_FAILURE;
    }

    char delimiter = snInfo[delimiterPos];
    XbhPartitionData::getInstance()->setSmartPNSNSeparator(&delimiter);
    // 传入的字符串为PN-SN 或PN_SN ，拆分出来, 同时指定两种分隔符
    char* token = strtok(const_cast<XBH_CHAR*>(snInfo), "-_");
    if (token != nullptr) {
        strncpy(pn, token, XBH_CUSDATA_SMART_SCALER_PN_LEN);
        pn[XBH_CUSDATA_SMART_SCALER_PN_LEN] = '\0';
    }

    token = strtok(nullptr, "-_");
    if (token != nullptr) {
        strncpy(sn, token, XBH_CUSDATA_BARCODE_LEN);
        sn[XBH_CUSDATA_BARCODE_LEN] = '\0';
    }
    switch (materialType)
    {
        case XBH_MAIN_SN:
            s32Ret1 = XbhPartitionData::getInstance()->setSmartScalerPnData(pn);
            s32Ret2 = XbhRk_3576::setSn(sn);
            s32Ret = (s32Ret1 == XBH_SUCCESS && s32Ret2 == XBH_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
            break;
        case XBH_CORE_SN:
            s32Ret3 = XbhPartitionData::getInstance()->setSmartCorePnData(pn);
            s32Ret4 = XbhPartitionData::getInstance()->setSmartCoreSnData(sn);
            s32Ret = (s32Ret3 == XBH_SUCCESS && s32Ret4 == XBH_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
            break;
        default:
            s32Ret = XBH_FAILURE;
            break;
    }
    return s32Ret;
}

/**
* 获取FATT烧录工厂SN
* param[out] materialType 物料类型 0: 底板(主板)SN  1:核心板SN 
* retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3576_E::getBurnFactorySN(XBH_MATERAL_TYPE materialType, XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32Ret1 = XBH_SUCCESS;
    XBH_S32 s32Ret2 = XBH_SUCCESS;
    XBH_S32 s32Ret3 = XBH_SUCCESS;
    XBH_S32 s32Ret4 = XBH_SUCCESS;
    XBH_CHAR pn[XBH_CUSDATA_SMART_SCALER_PN_LEN + 1] = {0};
    XBH_CHAR sn[XBH_CUSDATA_BARCODE_LEN + 1] = {0};
    switch (materialType)
    {
        case XBH_MAIN_SN:
            s32Ret1 = XbhPartitionData::getInstance()->getSmartScalerPnData(pn);
            s32Ret2 = XbhRk_3576::getSn(sn);
            s32Ret = (s32Ret1 == XBH_SUCCESS && s32Ret2 == XBH_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
            break;
        case XBH_CORE_SN:
            s32Ret3 = XbhPartitionData::getInstance()->getSmartCorePnData(pn);
            s32Ret4 = XbhPartitionData::getInstance()->getSmartCoreSnData(sn);
            s32Ret = (s32Ret3 == XBH_SUCCESS && s32Ret4 == XBH_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
            break;
        default:
            s32Ret = XBH_FAILURE;
            break;
    }
    XBH_CHAR delimiter[XBH_CUSDATA_SMART_PN_SN_SEPARATOR_LEN + 1] = {0};
    XbhPartitionData::getInstance()->getSmartPNSNSeparator(delimiter);
     // 拼接 pn 和 sn 为 pn-sn
    snprintf(snInfo, XBH_CUSDATA_SMART_SCALER_PN_LEN + XBH_CUSDATA_BARCODE_LEN + 2, "%s%s%s", pn , delimiter , sn);
    return s32Ret;
}


XBH_S32 XbhXMR3576_E::getHdmi1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getHdmi2SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getUsbc1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getDP1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getOps1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getFHdmi1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getFUsbc1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getVga1SignalStatus(XBH_S32 *status)
{
    return XbhRk_3576::getSignalStatus(status);
}

XBH_S32 XbhXMR3576_E::getEthPortSpeed(XBH_S32 port, XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR buf[32];
    std :: string result;

    // 中间件端口是从0开始计数，这里是从1开始计数，对此做一个转换
    port += 1;
    
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", port);
    XbhSysOpt::getInstance()->writeSysfs("/proc/rtk_gsw/port_status", buf);
    XbhSysOpt::getInstance()->readSysfs("/proc/rtk_gsw/port_status", result);
    
    if (!result.empty())
    {
        XLOGD("%s: %d result:%s!\n", __FUNCTION__, __LINE__, result.c_str());
        if (sscanf(result.c_str(), "%d_%s", value, &buf) == 2)
        {
            s32Ret = XBH_SUCCESS;
        }
    }
    
    return s32Ret;
}

/*
    设置单独每个通道的edid
*/
XBH_S32 XbhXMR3576_E::setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId)
{    
    XBH_S32 s32Ret = XBH_FAILURE;
    //根据project_id 更新 HDMI switch 的edid 从而达到切换edid type的作用
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    XLOGD(" projectid = %d enType = %d u32SourceId = %d ", projectid, enType, u32SourceId);
    XBH_S32 type  = 2;
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            if(projectid == 0)
            {
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                switch (u32SourceId)
                {
                    case XBH_SOURCE_HDMI1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID1_v1_4_2K, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_HDMI2:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID2_v1_4_2K, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_DP1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID4_v1_4_2K, XBH_HDMI_RXC);
                        break;
                    case XBH_SOURCE_USBC1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID7_v1_4_2K, XBH_HDMI_RXD);
                        break;
                    case XBH_SOURCE_OPS1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v1_4_2K, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_F_USBC1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID6_v1_4_2K, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_F_HDMI1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID3_v1_4_2K, XBH_HDMI_RXA);
                        break;
                    default:
                        s32Ret = XBH_FAILURE;
                        break;
                }
                setHdmiRx5vDet(XBH_TRUE);
            }
            else
            {
                char edid14BinFilePath[128] = {0};
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                switch (u32SourceId)
                {
                    case XBH_SOURCE_HDMI1:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_HDMI2:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_DP1:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, XBH_HDMI_RXC);
                        break;
                    case XBH_SOURCE_USBC1:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID7_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, XBH_HDMI_RXD);
                        break;
                    case XBH_SOURCE_OPS1:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid14BinFilePath, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_F_USBC1:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID6_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid14BinFilePath, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_F_HDMI1:
                        snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid14BinFilePath, XBH_HDMI_RXA);
                        break;
                    default:
                        s32Ret = XBH_FAILURE;
                        break;
                }
                setHdmiRx5vDet(XBH_TRUE);
            }
            type = 1;
            s32Ret = XBH_SUCCESS;
            break;
        case XBH_HDMIRX_EDID_TYPE_20:
            if(projectid == 0)
            {
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                switch (u32SourceId)
                {
                    case XBH_SOURCE_HDMI1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID1_v2_0_4K_2K, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_HDMI2:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID2_v2_0_4K_2K, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_DP1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID4_v2_0_4K_2K, XBH_HDMI_RXC);
                        break;
                    case XBH_SOURCE_USBC1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID7_v2_0_4K_2K, XBH_HDMI_RXD);
                        break;
                    case XBH_SOURCE_OPS1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID5_v2_0_4K_2K, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_F_USBC1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID6_v2_0_4K_2K, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_F_HDMI1:
                        XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_FRONT_GSV2712, XBH_EDID3_v2_0_4K_2K, XBH_HDMI_RXA);
                        break;
                    default:
                        s32Ret = XBH_FAILURE;
                        break;
                }
                setHdmiRx5vDet(XBH_TRUE);
            }
            else
            {
                char edid20BinFilePath[128] = {0};
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                switch (u32SourceId)
                {
                    case XBH_SOURCE_HDMI1:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_HDMI2:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_DP1:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXC);
                        break;
                    case XBH_SOURCE_USBC1:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID7_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, XBH_HDMI_RXD);
                        break;
                    case XBH_SOURCE_OPS1:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID5_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, XBH_HDMI_RXA);
                        break;
                    case XBH_SOURCE_F_USBC1:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID6_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid20BinFilePath, XBH_HDMI_RXB);
                        break;
                    case XBH_SOURCE_F_HDMI1:
                        snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                        readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_FRONT_GSV2712, edid20BinFilePath, XBH_HDMI_RXA);
                        break;
                    default:
                        s32Ret = XBH_FAILURE;
                        break;
                }
                setHdmiRx5vDet(XBH_TRUE);
            }
            type = 2;
            s32Ret = XBH_SUCCESS;
            break;
        case XBH_HDMIRX_EDID_TYPE_21:
            type = 3;
            s32Ret = XBH_SUCCESS;
            break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            type = 4;
            s32Ret = XBH_SUCCESS;
            break;
        default:
            break;
    }
    s32Ret = XbhRk_3576::setIndependenceSourceEdidType(u32SourceId, type);
    return s32Ret;
}

/*
    返回单独每个通道的edid type
*/
XBH_S32 XbhXMR3576_E::getSourceEdidType(XBH_U32 u32SourceId,XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type  = 2;
    s32Ret = XbhRk_3576::getIndependenceSourceEdidType(u32SourceId,&type);
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
/*
    返回单独每个通道的hdcpvalue
*/
XBH_S32 XbhXMR3576_E::getHdcpKeydata(XBH_SOURCE_E source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch (source)
    {
        case XBH_SOURCE_HDMI1:
        case XBH_SOURCE_HDMI2:
        case XBH_SOURCE_DP1:
        case XBH_SOURCE_USBC1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getHdmiSwitchHdcpKey(XBH_UPDATE_BOARD_GSV2715_1,pBuff,type);
            break;
        case XBH_SOURCE_F_USBC1:
        case XBH_SOURCE_F_HDMI1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getHdmiSwitchHdcpKey(XBH_UPDATE_FRONT_GSV2712, pBuff, type);
            break;
        default:
            s32Ret = XBH_FAILURE;
            break;
    }
    return s32Ret;
}

XBH_S32 XbhXMR3576_E::setTypecReset(XBH_SOURCE_E src)
{
    if ((src != XBH_SOURCE_F_USBC1) && (src != XBH_SOURCE_F_USBC2) && (src != XBH_SOURCE_F_USBC3) && (src != XBH_SOURCE_F_USBC4)) //不是前置type-c
    {
        if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT3_U5_BOARD)
        {
            XbhHdmiSwitchManager::getInstance()->setTypecReset(XBH_UPDATE_FRONT_GSV2712);
        }
    }

    if ((src != XBH_SOURCE_USBC1) && (src != XBH_SOURCE_USBC2) && (src != XBH_SOURCE_USBC3) && (src != XBH_SOURCE_USBC4)) //不是板载type-c
    {
        XbhHdmiSwitchManager::getInstance()->setTypecReset(XBH_UPDATE_BOARD_GSV2715_1);
    }
    
    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::processHpdUsbSwitch(XBH_SOURCE_E src)
{
    switch (src) {
        case XBH_SOURCE_F_HDMI1:
            //Power down front tx reset
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_RST, !XBH_BOARD_GPIO_F_HUB_RST_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, !XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            //Power down
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_U3_RED1_EN, XBH_BOARD_GPIO_U3_RED1_EN_OFF);
            usleep(50 * 1000);
            //no switch
            usleep(50 * 1000);
            //Power up
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_RST, XBH_BOARD_GPIO_F_HUB_RST_LEVEL);
            usleep(300 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_OFF);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            break;
        case XBH_SOURCE_USBC1:
            //Power down front tx reset
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, !XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            //Power down usbc hub
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU21_RST, XBH_BOARD_GPIO_HUB_HUB20_UU21_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, !XBH_BOARD_GPIO_USBC_NET_PWR_ON);
            //Power down
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_U3_RED1_EN, XBH_BOARD_GPIO_U3_RED1_EN_OFF);
            usleep(50 * 1000);
            //no switch
            usleep(50 * 1000);
            //Power up
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU21_RST, XBH_BOARD_GPIO_HUB_HUB20_UU21_RST_OFF);
            usleep(300 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, XBH_BOARD_GPIO_USBC_NET_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            break;
        case XBH_SOURCE_F_USBC1:
            //Power down front tx reset
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_RST, !XBH_BOARD_GPIO_F_HUB_RST_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, !XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB20_UU8_RST, XBH_BOARD_GPIO_HUB20_UU8_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, !XBH_BOARD_GPIO_USBC_NET_PWR_ON);
            //Power down
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_U3_RED1_EN, XBH_BOARD_GPIO_U3_RED1_EN_OFF);
            usleep(50 * 1000);
            //no switch
            usleep(50 * 1000);
            //Power up
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_RST, XBH_BOARD_GPIO_F_HUB_RST_LEVEL);
            usleep(300 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB20_UU8_RST, XBH_BOARD_GPIO_HUB20_UU8_RST_OFF);
            usleep(300 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, XBH_BOARD_GPIO_USBC_NET_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            break;
        case XBH_SOURCE_VGA1:
        case XBH_SOURCE_HDMI1:
        case XBH_SOURCE_HDMI2:
        case XBH_SOURCE_DP1:
            //Power down front tx reset
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, !XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            //Power down
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_U3_RED1_EN, XBH_BOARD_GPIO_U3_RED1_EN_OFF);
            usleep(50 * 1000);
            //no switch
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_U3_RED1_EN, XBH_BOARD_GPIO_U3_RED1_EN_ON);
            usleep(50 * 1000);
            //Power up
             XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU10_RST, XBH_BOARD_GPIO_HUB_HUB30_UU10_RST_OFF);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB30_UU5_RST, XBH_BOARD_GPIO_HUB_HUB30_UU5_RST_OFF);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB3_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, XBH_BOARD_GPIO_F_GPIO2_LEVEL);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB1_USB30_DEV_PWR_ON);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR, XBH_BOARD_GPIO_PUB2_USB30_DEV_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
            break;
        default:
            break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::processNetSwitch(XBH_S32 netchoice)
{
    static XBH_S32 current_net = XBH_FUSBC_NET; //默认网络是跟随到在前置type-c的
    if (current_net != netchoice)
    {
        if (netchoice == XBH_USBC_NET)
        {
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, !XBH_BOARD_GPIO_USBC_NET_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_SW, XBH_BOARD_GPIO_USBC_NET_SW_BOARD);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, XBH_BOARD_GPIO_USBC_NET_PWR_ON);
        }
        else if (netchoice == XBH_FUSBC_NET)
        {
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, !XBH_BOARD_GPIO_USBC_NET_PWR_ON);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_SW, XBH_BOARD_GPIO_USBC_NET_SW_F_BOARD);
            usleep(500 * 1000);
            XbhService::getPlatformInterface() ->setGpioOutputValue(XBH_BOARD_GPIO_USBC_NET_PWR, XBH_BOARD_GPIO_USBC_NET_PWR_ON);
        }
    }
    current_net = netchoice;
    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::ProcessTypeBHotplug(XBH_SOURCE_E src)
{
    static XBH_BOOL PrevTypeBPlugIn = XBH_TRUE;
    static XBH_BOOL PrevFrontTypeBPlugIn = XBH_TRUE;

    XBH_BOOL CurrentPlugIn = XBH_FALSE;

    XBH_U32 adcValue = 0;

    switch (src)
    {
        case XBH_SOURCE_VGA1:
        case XBH_SOURCE_HDMI1:
        case XBH_SOURCE_HDMI2:
        case XBH_SOURCE_DP1:
            XbhService::getPlatformInterface()->getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_TYPE_B_DET_CHAN, &adcValue);    //获取type-b的拔插情况
            CurrentPlugIn = (adcValue >= XBH_BOARD_ADC_TYPE_B_PLUGIN_VAL_MIN && adcValue <= XBH_BOARD_ADC_TYPE_B_PLUGIN_VAL_MID1);
            if (!PrevTypeBPlugIn && CurrentPlugIn)
            {
                XLOGD("CurrentSrc : %d, type_b plugin! \n", src);
                processHpdUsbSwitch(src);
            }
            PrevTypeBPlugIn = CurrentPlugIn;
        break;
        case XBH_SOURCE_F_HDMI1:
            XbhService::getPlatformInterface()->getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_TYPE_B_DET_CHAN, &adcValue);    //获取front type-b的拔插情况
            CurrentPlugIn = ((adcValue >= XBH_BOARD_ADC_TYPE_B_PLUGIN_VAL_MID1 && adcValue <= XBH_BOARD_ADC_TYPE_B_PLUGIN_VAL_MAX)
                            || (adcValue >= XBH_BOARD_ADC_TYPE_B_PLUGIN_VAL_MIN && adcValue <= XBH_BOARD_ADC_TYPE_B_PLUGIN_VAL_MID2));
            if (!PrevFrontTypeBPlugIn && CurrentPlugIn)
            {
                XLOGD("CurrentSrc : %d, front type_b plugin! \n", src);
                //XbhService::getModuleInterface()->usb_plug_event();
                processHpdUsbSwitch(src);
            }
            PrevFrontTypeBPlugIn = CurrentPlugIn;
            break;
        default:
            PrevTypeBPlugIn = XBH_TRUE;
            PrevFrontTypeBPlugIn = XBH_TRUE;
            return XBH_SUCCESS;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::ProcessTypeCHotplug(XBH_SOURCE_E src)
{
    static XBH_BOOL PrevTypeCPlugIn = XBH_TRUE;
    static XBH_BOOL PrevFrontTypeCPlugIn = XBH_TRUE;

    XBH_BOOL CurrentPlugIn = XBH_FALSE;

    XBH_BOOL current_usbc1, current_fusbc1;
    static XBH_BOOL prev_usbc1 = XBH_FALSE;
    static XBH_BOOL prev_fusbc1 = XBH_FALSE;
    XBH_S32 netchoice = 0;

    switch (src)
    {
        case XBH_SOURCE_USBC1:
            XbhSourceManager::getInstance()->getSourceDet(src, &CurrentPlugIn);
            if (!PrevTypeCPlugIn && CurrentPlugIn)
            {
                XLOGD("CurrentSrc : %d, type_c plugin! \n", src);
                //XbhService::getModuleInterface()->usb_plug_event();
                processHpdUsbSwitch(src);
            }
            PrevTypeCPlugIn = CurrentPlugIn;
            break;
        case XBH_SOURCE_F_USBC1:
            XbhSourceManager::getInstance()->getSourceDet(src, &CurrentPlugIn);
            if (!PrevFrontTypeCPlugIn && CurrentPlugIn)
            {
                XLOGD("CurrentSrc : %d, front type_c plugin! \n", src);
                //XbhService::getModuleInterface()->usb_plug_event();
                processHpdUsbSwitch(src);
            }
            PrevFrontTypeCPlugIn = CurrentPlugIn;
            break;
        
        default:    //不在type-c信源下
            PrevTypeCPlugIn = XBH_TRUE;
            PrevFrontTypeCPlugIn = XBH_TRUE;
            getUsbc1Det(&current_usbc1);
            getFUsbc1Det(&current_fusbc1);
            if (current_usbc1 && current_fusbc1)    //处理两个type-c都插入的状态
            {
                if (!prev_usbc1)                    //板载type-c后插入，需要保留网络到前置type-c
                {
                    netchoice = XBH_FUSBC_NET;
                }
                else if (!prev_fusbc1)              //前置type-c后插入，需要保留网络到板载type-c
                {
                    netchoice = XBH_USBC_NET;
                }
            }
            else if (current_usbc1)                 //板载type-c
            {
                netchoice = XBH_USBC_NET;
            }
            else if (current_fusbc1)                //前置type-c
            {
                netchoice = XBH_FUSBC_NET;
            }
            processNetSwitch(netchoice);
            prev_usbc1 = current_usbc1;
            prev_fusbc1 = current_fusbc1;
            return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMR3576_E::getKtcTouchInfo(XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string value;
    XbhSysOpt::getInstance()->readSysfs("/sys/devices/virtual/skg/skg_touch/drvinfo", value);
    strcpy(pBuff, value.c_str());
    XLOGD("getKtcTouchInfo=%s\n", value.c_str());
    return s32Ret;
}
