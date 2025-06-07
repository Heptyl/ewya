#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMA311D2_X"

#include "XbhLog.h"
#include "XbhXMA311D2_X.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhDp2HdmiManager.h"
#include "XbhMcuManager.h"
#include "XbhAVOutManager.h"
#include "XbhI2cSimulator.h"
#include "XbhAirSensorManager.h"
#include "SystemControlClient.h"
#include "XbhSourceManager.h"
extern SystemControlClient* mSystemControlClient;



XbhXMA311D2_X            *XbhXMA311D2_X::mInstance = XBH_NULL;
XbhMutex                   XbhXMA311D2_X::mLock;

#define RTK8367RB_VB_WOL_DEV            "/sys/devices/virtual/rtl8367/phy/wol"
#define PHY_PORT_STATE_PATH             "/sys/class/rtl8367/phy/update_port_state"

XbhXMA311D2_X::XbhXMA311D2_X()
{
    XLOGD(" begin ");
    XbhMWThread::run(XbhMWThread::REPEAT);
    VgaEnable = XBH_FALSE;
    frontHpdState = XBH_TRUE;
    frontHdmiState = XBH_FALSE;
    frontUsbcState = XBH_FALSE;
    mSystemControlClient = SystemControlClient::getInstance();
    XLOGD(" end ");
}

XbhXMA311D2_X::~XbhXMA311D2_X()
{
}

XBH_S32 XbhXMA311D2_X::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_X::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getHdmi3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_MCU_GPIO_HDMI3_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_MCU_GPIO_HDMI_3_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_X::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_X::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getUsbc2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_X::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_DP_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_X::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_VGA_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_VGA_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_X::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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
        s32Ret = setGpioOutputValue(XBH_BOARD_MCU_GPIO_OPS_PS_ON, !XBH_BOARD_MCU_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_MCU_GPIO_OPS_PS_ON, XBH_BOARD_MCU_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_STATUS_ON ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_X::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 || u32GpioHallInt2 == 0){
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 && u32GpioHallInt2 == 0){
        *s32Value = 2;
    }else if(u32GpioHallInt1 != 0 && u32GpioHallInt2 != 0){
        *s32Value = 0;
    }else{
        *s32Value = 1;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getHallSensorPen(XBH_S32 *state) {
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 value1 = 0; 
    XBH_U32 value2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2, &value1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1, &value2);
    *state = value2;
    *state = (*state << 8) + value1;
    XLOGD("value1=%d,  value2=%d",value1,value2);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SYSTEM_LED_GPIO,XBH_BOARD_GPIO_LOW);
            break;
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SYSTEM_LED_GPIO,XBH_BOARD_GPIO_HIGH);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_SYSTEM_LED_GPIO, &u32GpioValue);
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
    //XLOGD("getLedPwrStatus : %d\n",u32GpioValue);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_X::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setWolEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    XLOGD("setWolEnable   =  %d ", bEnable);
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

XBH_S32 XbhXMA311D2_X::getVSPort1LinkState(void)
{   XBH_S32 s32Ret = XBH_FAILURE;
    std::string result1;
    mSystemControlClient->readSysfs(PHY_PORT_STATE_PATH, result1);
    std::istringstream iss(result1);
    std::string token;
    std::vector<int> values(5, 0); 

    for (int i = 0; i < 5; ++i) {
        if (!(iss >> token)) break; 
        if (token.empty() || token != "0" && token != "1") {
            XLOGE("Invalid token '%s' at position %d\n", token.c_str(), i);
            return XBH_FAILURE;
        }
        values[i] = std::stoi(token); 
    }

    if ((values[1] == 1)) {
        s32Ret = XBH_SUCCESS;
    }

    XLOGE(" getVSPort1LinkState: s32Ret = %d", s32Ret);
    return s32Ret; 
}

XBH_S32 XbhXMA311D2_X::getVSPort2LinkState(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string result1;
    mSystemControlClient->readSysfs(PHY_PORT_STATE_PATH, result1);
    std::istringstream iss(result1);
    std::string token;
    std::vector<int> values(5, 0); 

    for (int i = 0; i < 5; ++i) {
        if (!(iss >> token)) break; 
        if (token.empty() || token != "0" && token != "1") {
            XLOGE("Invalid token '%s' at position %d\n", token.c_str(), i);
            return XBH_FAILURE;
        }
        values[i] = std::stoi(token); 
    }

    if ((values[2] == 1)) {
        s32Ret = XBH_SUCCESS;
    }

    XLOGE(" getVSPort2LinkState: s32Ret = %d", s32Ret);
    return s32Ret; 
}

XBH_S32 XbhXMA311D2_X::getEthPlugStatus(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 eth_port1 =  XbhXMA311D2_X::getInstance()->getVSPort1LinkState();
    XBH_S32 eth_port2 =  XbhXMA311D2_X::getInstance()->getVSPort2LinkState();
     if(eth_port1 == XBH_SUCCESS || eth_port2 == XBH_SUCCESS ) //link down -> link up
        {
            XLOGD("NET IC LINK UP~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            XbhXMA311D2_X::getInstance()->setGpioOutputValue(XBH_BOARD_GPIO_ETH_LED_G1, XBH_BOARD_GPIO_ETH_LED_G1_ON);
            XbhXMA311D2_X::getInstance()->setGpioOutputValue(XBH_BOARD_GPIO_ETH_LED_G2, XBH_BOARD_GPIO_ETH_LED_G2_ON);
            *bEnable = 1;
        }
        else //link up -> link down
        {
            XLOGD("NET IC LINK DOWN~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            XbhXMA311D2_X::getInstance()->setGpioOutputValue(XBH_BOARD_GPIO_ETH_LED_G1, !XBH_BOARD_GPIO_ETH_LED_G1_ON);
            XbhXMA311D2_X::getInstance()->setGpioOutputValue(XBH_BOARD_GPIO_ETH_LED_G2, !XBH_BOARD_GPIO_ETH_LED_G2_ON);
            *bEnable = 0;
        }
    XLOGD("getEthPlugStatus : %d\n",*bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setContrast(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGD("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setContrast(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getContrast(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getContrast();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get Contrast = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_X::setBrightness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setBrightness(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getBrightness(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getBrightness();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get brightness = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_X::setHue(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setHue(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getHue(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getHue();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get hue = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_X::setSharpness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setSharpness(u32Value, 1/*unused*/, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getSharpness(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getSharpness();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get Sharpness = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_X::setSaturation(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setSaturation(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getSaturation(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getSaturation();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE(" %s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI(" %s: %d get Saturation = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_X::setSn(const XBH_CHAR* strSn)
{
    return mSystemControlClient->writeUnifyKey("usid", strSn) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string serial;
    if(mSystemControlClient->readUnifyKey("usid", serial)){
        strcpy(strSn,serial.c_str());
        XLOGI("%s %d: serial number -> [%s] \n",__func__,__LINE__,strSn);
    }
    else
    {
        XLOGE("%s %d: serial number error \n",__func__,__LINE__);
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setDeviceId(const XBH_CHAR* strDeviceId)
{
    return mSystemControlClient->writeUnifyKey("deviceid", strDeviceId) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getDeviceId(XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string temp;
    if(mSystemControlClient->readUnifyKey("deviceid", temp)){
        strcpy(strDeviceId,temp.c_str());
        XLOGI("%s %d: deviceid-> [%s] \n",__func__,__LINE__,strDeviceId);
    }
    else
    {
        XLOGE("%s %d: deviceid error \n",__func__,__LINE__);
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setColorTempMode(XBH_COLORTEMP_E enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(enColorTemp < XBH_COLORTEMP_STANDARD || enColorTemp >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }

    return mSystemControlClient->setColorTemperature(enColorTemp, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_X::getColorTempMode(XBH_COLORTEMP_E *enColorTemp)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getColorTemperature();
    if(s32Ret < XBH_COLORTEMP_STANDARD || s32Ret >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *enColorTemp = (XBH_COLORTEMP_E)s32Ret;
    XLOGI("%s: %d get ColorTemp = %d\n",__func__,__LINE__,*enColorTemp);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_X::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
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
            XBH_MCU_I2CBUFFDEF_S pI2cBuff;
            this->XbhAmlogic_311d2::setWosEnable(bEnable);
            /*设置MCU信源唤醒总开关*/
            pI2cBuff.cmd = CMD_I2C_SET_WAKEUP_F_USBC;
            pI2cBuff.len = 1;
            pI2cBuff.data[0] = bEnable;
            s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
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

XBH_S32 XbhXMA311D2_X::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
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

XBH_S32 XbhXMA311D2_X::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
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

XBH_S32 XbhXMA311D2_X::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR sWolStatus[PROPERTY_VALUE_MAX];
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    XLOGD("---- onStop ----");
    /*通知MCU SOC已待机*/
    pI2cBuff.cmd = CMD_I2C_SOC_POWER_MODE;
    pI2cBuff.len = 8;
    pI2cBuff.data[0] = XBH_POWERMODE_STANDBY;
    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);


    property_get("persist.vendor.xbh.wol.enable", sWolStatus, "false");
    if(strncmp(sWolStatus, "true",4) == 0) {
        setWolEnable(XBH_TRUE);
    } else {
        setWolEnable(XBH_FALSE);
    }

    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC2, ver);
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

XBH_S32 XbhXMA311D2_X::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC2, filename, force);
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

XBH_S32 XbhXMA311D2_X::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC2, s32Value);
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
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_X::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::dumpEdid(XBH_SOURCE_E idx)
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

XBH_S32 XbhXMA311D2_X::setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length) 
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL);
    if (pSimulator == nullptr) {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Write_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL);
    if (pSimulator == nullptr) {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Read_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}


XBH_S32 XbhXMA311D2_X::setUsbNetStatus(XBH_S32 bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (bEnable) {
        //USB-C Ethernet sharing is on
        setGpioOutputValue(XBH_BOARD_MCU_USB20_RJ45_PWR1, XBH_BOARD_GPIO_HIGH);
        setGpioOutputValue(XBH_BOARD_MCU_USB20_RJ54_PWR2, XBH_BOARD_GPIO_HIGH);
    } else {
        //USB-C Ethernet sharing is off
        setGpioOutputValue(XBH_BOARD_MCU_USB20_RJ45_PWR1, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_MCU_USB20_RJ54_PWR2, XBH_BOARD_GPIO_LOW);
    }

    m_bUsbPortStatus = bEnable;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getUsbNetStatus(XBH_S32 *pbEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *pbEnable = m_bUsbPortStatus;
    return s32Ret;
}


void XbhXMA311D2_X::run(const void* arg)
{
    XBH_SOURCE_E u32Source;
    XBH_CHAR SKUStatus[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.xbh.vs_sell_zone", SKUStatus, "ww");
    XbhSourceManager::getInstance()->getCurrSource(&u32Source);
    if((u32Source == XBH_SOURCE_HDMI3) && strncmp(SKUStatus, "tw",2) == 0){
        setGpioOutputValue(XBH_BOARD_GPIO_SW_SEL10, XBH_BOARD_GPIO_HIGH);
    XLOGD("VS TW VERSION");
    }
    else
    {
    XLOGD("VS WW VERSION");
    }

    XBH_CHAR sUsbEthStatus[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.usb.ethernet.enable", sUsbEthStatus, "false");
    if(strncmp(sUsbEthStatus, "true",4) == 0) {
        m_bUsbPortStatus = XBH_TRUE;
    } else {
        m_bUsbPortStatus = XBH_FALSE;
    }
        setUsbNetStatus(m_bUsbPortStatus);
        XLOGD("USB Ethernet status changed to: %s", m_bUsbPortStatus ? "true" : "false");

    usleep(500 * 1000);
}

XBH_S32 XbhXMA311D2_X::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXA, XBH_HDMI5_CEC_PHYSICAL_ADDR);// ops 
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXB, XBH_HDMI4_CEC_PHYSICAL_ADDR);// to next 
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXD, XBH_HDMI6_CEC_PHYSICAL_ADDR);//dp

    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std :: string result;
    std :: string data;
    std :: string wakeup;
    const std :: string strWakeupReason = "/sys/class/cec/wake_up_reason";
    const std :: string strWakeupData = "/sys/class/cec/wake_up_data";
    const std :: string strWakeup = "/sys/class/cec/wake_up";
    XBH_S32 s32WakeupReason = 0;
    XBH_S32 s32WakeupData = 0;
    XBH_CEC_WAKEUP_S stWakeup;

    mSystemControlClient->readSysfs(strWakeupReason, result);
    if(!result.empty())
    {
        s32WakeupReason = strtol(result.c_str(), XBH_NULL, 16);
        s32WakeupReason = s32WakeupReason & 0x0f;
    }

    switch(s32WakeupReason)
    {
        case REMOTE_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_IR;
            break;
        case POWER_KEY_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_KEYPAD;
            break;
        case RTC_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_RTC;
            break;
        case ETH_PMT_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_ETH;
            break;
        case CEC_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_CEC;
            stWakeup.wk_logic_addr = 0;
            stWakeup.wk_phy_addr = 0;
            stWakeup.wk_port_id = 0;
            mSystemControlClient->readSysfs(strWakeup, wakeup);
            if(!wakeup.empty())
            {
                int cecWakeupPort = strtol(wakeup.c_str(), XBH_NULL, 16);
                memcpy(&stWakeup, &cecWakeupPort, sizeof(stWakeup));
            }
            if (stWakeup.wk_phy_addr == 0x1000)
            {
                stWake->enSrc = XBH_SOURCE_HDMI1;
            }
            else if (stWakeup.wk_phy_addr == 0x2000)
            {
                stWake->enSrc = XBH_SOURCE_HDMI2;
            }
            else if (stWakeup.wk_phy_addr == 0x3000)
            {
                stWake->enSrc = XBH_SOURCE_HDMI3;
            }
            else if (stWakeup.wk_phy_addr == 0x4000)
            {
                stWake->enSrc = XBH_SOURCE_F_HDMI1;
            }
            else
            {
                stWake->enWakeup = XBH_WAKEUP_BUTT;
            }
            break;
        case XBH_EXT_WAKEUP_REASON:
            {
                mSystemControlClient->readSysfs(strWakeupData, data);
                if(!data.empty())
                {
                    s32WakeupData = strtol(data.c_str(), XBH_NULL, 16);
                }
                switch (s32WakeupData)
                {
                    case XBH_EXT_WAKEUP_REASON_SOURCE_VGA:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_VGA1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_HDMI1:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_HDMI1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_HDMI2:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_HDMI2;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_HDMI3:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_HDMI3;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_F_HDMI:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_F_HDMI1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_DP1:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_DP1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_TYPEC1:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_USBC2;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_TYPEC2:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_F_USBC1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_MCU:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        //stWake->enSrc = XBH_SOURCE_DP1;
                        getMcuWakeUpInfo(stWake);
                        XLOGD("XBH_EXT_WAKEUP_REASON_MCU enWakeup::%d enSrc:%d Value:%d\n",stWake->enWakeup,stWake->enSrc,stWake->u32Value);
                        break;
                    case XBH_EXT_WAKEUP_REASON_UART:
                        stWake->enWakeup = XBH_WAKEUP_UART;
                        break;
                    case XBH_EXT_WAKEUP_REASON_PIR:
                        stWake->enWakeup = XBH_WAKEUP_PIR;
                        break;
                    case XBH_EXT_WAKEUP_REASON_NFC:
                        stWake->enWakeup = XBH_WAKEUP_NFC;
                        break;
                    default:
                        s32Ret = XBH_FAILURE;
                        XLOGE("getWakeUpData fail %s\n",s32WakeupData);
                        break;
                }
            }
            break;
            default:
                s32Ret = XBH_FAILURE;
                XLOGE("getWakeUpReason fail %d\n",s32WakeupReason);
                break;
    }
    XLOGD("getWakeUpReason end %d %d %04x", stWake->enWakeup, stWake->enSrc, stWake->u32Value);

    return s32Ret;
}

XBH_S32 XbhXMA311D2_X::getMcuWakeUpInfo(XBH_WAKEUP_S *stWake)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[10] = {0};

    s32Ret = this->XbhAmlogic_311d2::getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_INFO, 1, 10, pu8Data);

    stWake->enWakeup = (XBH_WAKEUP_E)pu8Data[0];
    stWake->u32Value = (uint16_t)(pu8Data[3] * 256 | pu8Data[2]) & 0xffff;

    if (XBH_WAKEUP_SOURCE == stWake->enWakeup)
    {
        stWake->enSrc = (XBH_SOURCE_E)pu8Data[1];
    }
    else
    {
        XLOGD("unknown XBH_WAKEUP_E type!");
    }

    XLOGD("getMcuWakeUpInfo end %d %d %04x", stWake->enWakeup, stWake->enSrc, stWake->u32Value);

    return s32Ret;
}



XbhXMA311D2_X *XbhXMA311D2_X::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMA311D2_X();
        }
    }
    return mInstance;
}
