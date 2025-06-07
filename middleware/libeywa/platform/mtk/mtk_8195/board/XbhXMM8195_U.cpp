#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM8195_U"

#include <string.h>

#include "XbhLog.h"
#include "XbhXMM8195_U.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhGSensorManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhAudioAmplifierManager.h"


XbhXMM8195_U            *XbhXMM8195_U::mInstance = XBH_NULL;
XbhMutex                 XbhXMM8195_U::mLock;

//------------------ static function begin -----------------------------
//override
XBH_S32 XbhXMM8195_U::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_FHDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_FHDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_F_Type_C_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_DPIN_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DPIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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
        s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (enSource == XBH_SOURCE_OPS1)
    {
        if (bEnable)
        {
            s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWR_EN, XBH_BOARD_GPIO_OPS_PWR_EN_LEVEL);
        }
        else
        {
            s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWR_EN, !XBH_BOARD_GPIO_OPS_PWR_EN_LEVEL);
        }
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWR_EN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_EN_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getGSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt1 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_ADC, &u32GpioPirInt1);
    //TODO 8195 get value in ADC not gpio
    if(u32GpioPirInt1 == 0)
    {
        *s32Value = 0;
    }
    else
    {
        *s32Value = 1;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    //TODO 8195 get value in ADC not gpio
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_ADC, &u32GpioHallInt1);
    if(u32GpioHallInt1 == 0)
    {
        *s32Value = 0;
    }
    else
    {
        *s32Value = 1;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
        if(u32GpioValue == XBH_BOARD_GPIO_F_LED_R_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_RED;
        }
        else if (u32GpioValue == XBH_BOARD_GPIO_F_LED_G_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_GREEN;
        }
    }
    return s32Ret;
}

XBH_S32 XbhXMM8195_U::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMM8195_U::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}
//------------------ static function end -----------------------------


//------------------ public function begin -----------------------------

//------------------ public function end -----------------------------


//------------------ private function begin -----------------------------

//------------------ private function end -----------------------------

XbhXMM8195_U::XbhXMM8195_U()
{
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMM8195_U::~XbhXMM8195_U()
{
}

XbhXMM8195_U *XbhXMM8195_U::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM8195_U();
        }
    }
    return mInstance;
}
