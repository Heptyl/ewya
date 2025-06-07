#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMA311D2_T2"

#include "XbhLog.h"
#include "XbhXMA311D2_T2.h"
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
#include "XbhPowerSensorManager.h"
extern SystemControlClient* mSystemControlClient;

XbhXMA311D2_T2            *XbhXMA311D2_T2::mInstance = XBH_NULL;
XbhMutex                   XbhXMA311D2_T2::mLock;
//HALL sensor MCU I2C register
#define CMD_I2C_GET_ADC         0xA6
#define RTK8367RB_VB_WOL_DEV            "/sys/devices/virtual/rtl8367/phy/wol"
#define PHY_PORT_STATE_PATH             "/sys/class/rtl8367/phy/update_port_state"

/***************************************PD power config start**********************************/
#define ENABLE_DYNAMIC_CONFIG_PD_CHARGE

#define FRONT_TYPEC_PLUGIN  XBH_BOARD_GPIO_HIGH
#define FRONT_TYPEC_UNPLUG  XBH_BOARD_GPIO_LOW
#define REAR_TYPEC_PLUGIN   XBH_BOARD_GPIO_HIGH
#define REAR_TYPEC_UNPLUG   XBH_BOARD_GPIO_LOW

//PD charge config regiest.
//寄存器的值的意义：0为寄存器复位初始值；0x4为SOC配置100W时的值，0x84为PD在已经设置完成100W充电时更新的值（最高位置1）
#define REG_PD_CHARGE_POWER  0xFF81
#define PD_CHARGE_POWER_15W  1 // MAX 15W
#define PD_CHARGE_POWER_45W  2 // MAX 45W
#define PD_CHARGE_POWER_65W  3 // MAX 65W
#define PD_CHARGE_POWER_100W 4 // MAX 100W
#define PD_CHARGE_POWER_4P5W 5 // MAX 4.5W
#define PD_CHARGE_POWER_50W  6 // MAX 50W

static XBH_S32 curRearTypecDetStatus = REAR_TYPEC_UNPLUG;
static XBH_S32 preRearTypecDetStatus = REAR_TYPEC_UNPLUG;
static XBH_S32 curFrontTypecDetStatus  = FRONT_TYPEC_UNPLUG;
static XBH_S32 preFrontTypecDetStatus  = FRONT_TYPEC_UNPLUG;

enum PD_CHARGE_CFG_TYPE_E {
    PD_CONFIG_AUTO                  = 0, //根据插拔状态情况配置PC充电功率
    FRONT_PD_100W_ONLY              = 1,
    REAR_PD_100W_ONLY               = 2,
    FRONT_PD_50W_THEN_REAR_PD_50W   = 3,
    REAR_PD_50W_THEN_FRONT_PD_50W   = 4,
    FRONT_PD_50W_AND_REAR_PD_50W    = 5,
};

static XBH_U8  gConfiguedType = PD_CONFIG_AUTO;
static XBH_BOOL gIsPDchargeCfgOk = XBH_FALSE;
/***************************************PD power config end**********************************/

XbhXMA311D2_T2::XbhXMA311D2_T2()
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

XbhXMA311D2_T2::~XbhXMA311D2_T2()
{
}

XBH_S32 XbhXMA311D2_T2::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getHdmi3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
    {
        *enable = XBH_FALSE;
    }
    else
    {
        s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_MCU_GPIO_HDMI3_DET, &u32GpioValue);
        *enable =  u32GpioValue == XBH_BOARD_MCU_GPIO_HDMI_3_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }

    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_VGA_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_VGA_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_T2::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_STATUS_ON ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMA311D2_T2::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}
/*
XBH_S32 XbhXMA311D2_T2::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 || u32GpioHallInt2 == 0){
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}*/


XBH_S32 XbhXMA311D2_T2::getHallSensorValue(XBH_S32* s32Value) {
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 mcuPens = 0;  // MCU部分笔数（HALL1+HALL2，0/1/2）
    XBH_S32 socPens = 0;  // SOC部分笔数（HALL3+HALL4，0/1/2）
    XBH_U32 adcValue = 0;

    // 1. 读取MCU的HALL1和HALL2组合ADC值
    // --------------------------------
    XBH_MCU_I2CBUFFDEF_S stOutData;
    stOutData.cmd = CMD_I2C_GET_ADC;
    stOutData.len = 3;
    s32Ret = this->XbhAmlogic_311d2::getIICData(
        XBH_MCU_I2C_CHN, 
        XBH_MCU_I2C_ADR, 
        stOutData.cmd, 
        0x01, 
        stOutData.len, 
        stOutData.data
    );

    if (s32Ret != XBH_SUCCESS) {
        XLOGD("MCU ADC read failed! Error: %d", s32Ret);
        return s32Ret;  
    }

    // 解析MCU ADC值并判断笔数
    adcValue = stOutData.data[0] << 8 | stOutData.data[1];
    if (adcValue > 1000 && adcValue < 2000) {
        mcuPens = 1;  
    } else if (adcValue < 950) {
        mcuPens = 2;  
    } else {
        mcuPens = 0;  
    }
    XLOGD("MCU ADC: %d -> Pens: %d", adcValue, mcuPens);

    // 2. 读取SOC的HALL3和HALL4组合ADC值
    // --------------------------------
#ifdef XBH_BOARD_GPIO_HALL_ADC_CHANNEL
    XBH_U32 socAdcValue = 0;
    s32Ret = getADCChannelValue(
        (XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_GPIO_HALL_ADC_CHANNEL, 
        &socAdcValue
    );
    if (s32Ret == XBH_SUCCESS) {
        if (socAdcValue > 300 && socAdcValue < 800) { 
            socPens = 1;  
        } else if (socAdcValue < 300) {                 
            socPens = 2; 
        } else {          
            socPens = 0;  
        } 
        XLOGD("SOC ADC: %d -> Pens: %d", socAdcValue, socPens);
    } else {
        XLOGD("SOC ADC read failed. Default: no pen.");
    }
#endif

    // 3. 计算总笔数并赋值给s32Value
    // --------------------------------
    *s32Value = mcuPens + socPens;
    XLOGD("Total pens: %d (MCU=%d, SOC=%d)", *s32Value, mcuPens, socPens);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;   //默认为开
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_PIR_CHAN, &adcValue);

    if(adcValue < 800)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;   
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}


XBH_S32 XbhXMA311D2_T2::getPowerSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhPowerSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getPowerSensorValue(XBH_FLOAT* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("kkk getPowerSensorValue ");
    s32Ret = XbhPowerSensorManager::getInstance()->getPowerSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getPowerSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,    XBH_BOARD_GPIO_POWER_LED_RED_ON);
            s32Ret &= setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE, XBH_BOARD_GPIO_POWER_LED_WHITE_OFF);
            break;
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE,XBH_BOARD_GPIO_POWER_LED_WHITE_ON);
            s32Ret &= setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,  XBH_BOARD_GPIO_POWER_LED_RED_OFF);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
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

XBH_S32 XbhXMA311D2_T2::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_T2::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setWolEnable(XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_T2::getVSPort1LinkState(void)
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

XBH_S32 XbhXMA311D2_T2::getVSPort2LinkState(void)
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

XBH_S32 XbhXMA311D2_T2::getEthPlugStatus(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 eth_port1 =  XbhXMA311D2_T2::getInstance()->getVSPort1LinkState();
    XBH_S32 eth_port2 =  XbhXMA311D2_T2::getInstance()->getVSPort2LinkState();
     if(eth_port1 == XBH_SUCCESS || eth_port2 == XBH_SUCCESS ) //link down -> link up
        {
            XLOGD("NET IC LINK UP~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            *bEnable = 1;
        }
        else //link up -> link down
        {
            XLOGD("NET IC LINK DOWN~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            *bEnable = 0;
        }
    XLOGD("getEthPlugStatus : %d\n",*bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setContrast(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGD("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setContrast(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getContrast(XBH_U32 *u32Value)
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

XBH_S32 XbhXMA311D2_T2::setBrightness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setBrightness(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getBrightness(XBH_U32 *u32Value)
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

XBH_S32 XbhXMA311D2_T2::setHue(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setHue(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getHue(XBH_U32 *u32Value)
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

XBH_S32 XbhXMA311D2_T2::setSharpness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setSharpness(u32Value, 1/*unused*/, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getSharpness(XBH_U32 *u32Value)
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

XBH_S32 XbhXMA311D2_T2::setSaturation(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setSaturation(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getSaturation(XBH_U32 *u32Value)
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

XBH_S32 XbhXMA311D2_T2::setSn(const XBH_CHAR* strSn)
{
    return mSystemControlClient->writeUnifyKey("usid", strSn) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getSn(XBH_CHAR* strSn)
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

XBH_S32 XbhXMA311D2_T2::setColorTempMode(XBH_COLORTEMP_E enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(enColorTemp < XBH_COLORTEMP_STANDARD || enColorTemp >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }

    return mSystemControlClient->setColorTemperature(enColorTemp, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getColorTempMode(XBH_COLORTEMP_E *enColorTemp)
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

XBH_S32 XbhXMA311D2_T2::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
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

            pu8Data[0] = bEnable;
            s32Ret |= setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_HDMI, 1, 1, pu8Data);
            XLOGD("set mcu wos status: %d\n", pu8Data[0]);
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
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

XBH_S32 XbhXMA311D2_T2::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
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

XBH_S32 XbhXMA311D2_T2::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
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

XBH_S32 XbhXMA311D2_T2::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR sWolStatus[PROPERTY_VALUE_MAX];
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    XLOGD("---- onStop ----");
    XBH_BOOL wot_enable;
    s32Ret = getWoTStatus(XBH_WAKEUP_SOURCE, &wot_enable);
    if(wot_enable) {
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
    }

    //red led on, and white led off
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,   XBH_BOARD_GPIO_POWER_LED_RED_ON);
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE, XBH_BOARD_GPIO_POWER_LED_WHITE_OFF);

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

XBH_S32 XbhXMA311D2_T2::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        case XBH_UPDATE_BOARD_GSV6125:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV6125, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV6125:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV6125, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV6125:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV6125, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::dumpEdid(XBH_SOURCE_E idx)
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

XBH_S32 XbhXMA311D2_T2::setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length) 
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

XBH_S32 XbhXMA311D2_T2::getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length)
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

XBH_BOOL XbhXMA311D2_T2::isFrontTypecInDetChanged()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, (XBH_U32 *)&curFrontTypecDetStatus);
    //XLOGD("curFrontTypecDetStatus pdPowerConfig is %d", curFrontTypecDetStatus);
    if(curFrontTypecDetStatus != preFrontTypecDetStatus)
    {
        XLOGD("   ");
        XLOGD("isFrontTypecInDetChanged pdPowerConfig = true");
        preFrontTypecDetStatus = curFrontTypecDetStatus;
        if(curFrontTypecDetStatus == FRONT_TYPEC_UNPLUG)
        {
            XBH_U8 data = PD_CHARGE_POWER_4P5W;
            XBH_S32 ret = setIICData(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, 2, 1, &data);
            XLOGD("front typec unplug, pdPowerConfig set PD to 4.5W, ret = %d",ret);
        }
        else
        {
            XLOGD("front typec pdPowerConfig plugin");
        }

        return XBH_TRUE;
    }

    preFrontTypecDetStatus = curFrontTypecDetStatus;
    return XBH_FALSE;
}


XBH_BOOL XbhXMA311D2_T2::isRearTypecInDetChanged()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, (XBH_U32 *)&curRearTypecDetStatus);
    //XLOGD("curRearTypecDetStatus pdPowerConfig is %d", curRearTypecDetStatus);
    if(curRearTypecDetStatus != preRearTypecDetStatus)
    {
        XLOGD("   ");
        XLOGD("isRearTypecInDetChanged pdPowerConfig = true");
        preRearTypecDetStatus = curRearTypecDetStatus;
        if(curRearTypecDetStatus == REAR_TYPEC_UNPLUG)
        {
            XBH_U8 data = PD_CHARGE_POWER_4P5W;
            XBH_S32 ret = setIICData(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, 2, 1, &data);
            XLOGD("rear typec unplug, pdPowerConfig set PD to 4.5W, ret = %d",ret);
        }
        else
        {
             XLOGD("rear typec plugin pdPowerConfig");
        }


        return XBH_TRUE;
    }

    preRearTypecDetStatus = curRearTypecDetStatus;
    return XBH_FALSE;
}

void XbhXMA311D2_T2::regValToPowerLevel(XBH_U8 regVal, XBH_CHAR * powerLevle)
{
    if(powerLevle == NULL)
    {
        return;
    }

    XBH_U8 level  = regVal & 0x0F;
    switch (level)
    {
        case 0x06:
            strcpy(powerLevle, "50W");
            break;
        case 0x05:
            strcpy(powerLevle, "4.5W");
            break;
        case 0x04:
            strcpy(powerLevle, "100W");
            break;
        case 0x03:
            strcpy(powerLevle, "65W");
            break;
        case 0x02:
            strcpy(powerLevle, "45W");
            break;
        case 0x01:
            strcpy(powerLevle, "15W");
            break;
        default:
            strcpy(powerLevle, "unkown");
            break;
    }

}

//PD charge config regiest.  100w: 0x04或0x84, 4.5W: 0x05或0x85, 50w: 0x06或0x86, default: 0x00
XBH_BOOL XbhXMA311D2_T2::isPDchargeConfiguedOK(XBH_U32 bus_index, XBH_U8 device_addr, XBH_U16 register_addr,XBH_U8 * data)
{
    XBH_S32 ret1 = -1;
    XBH_S32 ret2 = -1;
    XBH_BOOL bRet  = XBH_FALSE;
    XBH_U8 value = 0x0;
    XBH_CHAR powerLevel[16] = {0};

    if(data == NULL)
    {
        XLOGE("isPDchargeConfiguedOK: pdPowerConfig data is NULL!!!");
        return XBH_FALSE;
    }

    //send command to config the PD charge
    ret1 = setIICData(bus_index, device_addr, register_addr, 2, 1, data);

    if(ret1 != 0)
    {
        XLOGE("PD charge configue pdPowerConfig failed !!!");
    }
    else
    {
        XLOGD("PD charge configue pdPowerConfig send data: 0x%x  to bus:%d, devAddr:0x%x, reg:0x%0x  ok",*data,bus_index,device_addr,register_addr);
    }

    usleep(300*1000);

    //read back the config result
    ret2 = getIICData(bus_index, device_addr, register_addr, 2 ,1 ,&value);
    if(ret2 != 0)
    {
         XLOGE("read back the PD charge pdPowerConfig configue value failed !!!");
    }
    else
    {
        XLOGD("read back PD configued pdPowerConfig value: 0x%x ok",value);
    }

    //config and  get back:  0x04 and 0x84, 0x05 and 0x85, 0x06 and 0x86
    if((ret1 | ret2 == 0) && (value == ((1<<7) | ((*data) & 0x0F))))
    {
        bRet = XBH_TRUE;
    }

    regValToPowerLevel((*data) & 0x0F,powerLevel);
    if(bus_index == XBH_BOARD_I2C_GSV2202E_I2C_NUM)
    {
        XLOGD(">>> config Rear  USB_C pdPowerConfig to %s  %s",powerLevel,bRet?"OK":"failed");
    }
    else if(bus_index == XBH_BOARD_I2C_GSV2712_I2C_NUM)
    {
        XLOGD(">>> config Front USB_C pdPowerConfig %s %s",powerLevel, bRet?"OK":"failed");
    }

    return bRet;
}

//config PD charge to 100W+4.5W or 50W+50W
XBH_BOOL XbhXMA311D2_T2::handlePDcharge()
{
    XBH_S32 ret = -1;
    XBH_BOOL isOk = XBH_TRUE;
    XBH_U8 data = 0x00;

    XLOGD("handlePDcharge: gConfiguedType pdPowerConfig = %d", gConfiguedType);

    //gConfiguedType不是auto时，表示上次做相应配置时没有成功，在此强制重新做相应的充电配置

    if((gConfiguedType == REAR_PD_100W_ONLY ) || (curRearTypecDetStatus == REAR_TYPEC_PLUGIN && curFrontTypecDetStatus == FRONT_TYPEC_UNPLUG))
    {
        gConfiguedType = REAR_PD_100W_ONLY;
        data = PD_CHARGE_POWER_100W;
        isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
    }
    else if((gConfiguedType == FRONT_PD_100W_ONLY) || (curRearTypecDetStatus == REAR_TYPEC_UNPLUG && curFrontTypecDetStatus == FRONT_TYPEC_PLUGIN))
    {
        gConfiguedType = FRONT_PD_100W_ONLY;
        data = PD_CHARGE_POWER_100W;
        isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);

    }
    else if(curRearTypecDetStatus == REAR_TYPEC_PLUGIN && curFrontTypecDetStatus == FRONT_TYPEC_PLUGIN)
    {
        //50W + 50W
        XBH_U8 dataFront = 0x00;
        XBH_U8 dataRear  = 0x00;

        ret = getIICData(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, 2 ,1 ,&dataRear);
        ret = getIICData(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, 2 ,1 ,&dataFront);

        XLOGD("Both plugin: pdPowerConfig dataFront = 0x%x, dataRear = 0x%x , ret = %d",dataFront, dataRear,ret);
        if((gConfiguedType == FRONT_PD_50W_THEN_REAR_PD_50W) || ((ret == 0) && (dataFront != 0) && ((dataFront & 0x0F) != PD_CHARGE_POWER_4P5W)))
        {
            XLOGD("Add Rear pdPowerConfig USB_C plugin");
            gConfiguedType = FRONT_PD_50W_THEN_REAR_PD_50W;
            data = PD_CHARGE_POWER_50W;
            isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
            isOk &= isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);

        }
        else if((gConfiguedType == REAR_PD_50W_THEN_FRONT_PD_50W) || ((ret == 0) && (dataRear != 0) && ((dataRear & 0x0F) != PD_CHARGE_POWER_4P5W)))
        {
            XLOGD("Add Front pdPowerConfig USB_C plugin");
            gConfiguedType = REAR_PD_50W_THEN_FRONT_PD_50W;
            data = PD_CHARGE_POWER_50W;
            isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
            isOk &= isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
        }
        else if((gConfiguedType = FRONT_PD_50W_AND_REAR_PD_50W)
                || ((ret == 0)
                    && (dataRear == 0 || (dataRear & 0x0F) == PD_CHARGE_POWER_4P5W)
                    && (dataFront == 0 || ((dataFront & 0x0F) == PD_CHARGE_POWER_4P5W))))
        {
            //开机时两路typec都有连接设备或两路typec同时接入设备
            XLOGD("Both USB_C pdPowerConfig plugin at the same time");
            gConfiguedType = FRONT_PD_50W_AND_REAR_PD_50W;
            data = PD_CHARGE_POWER_50W;
            isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
            isOk &= isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
        }
    }

    if(!isOk)
    {
        //做了充电配置，且失败了
        XLOGE("handlePDcharge pdPowerConfig failed !!!!");
    }
    else
    {
        //有配置过PD充电且成功了，设置根据两路typec插拔状态来动态配置PD充电功率的模式
        if(gConfiguedType != PD_CONFIG_AUTO)
        {
            gConfiguedType = PD_CONFIG_AUTO;
        }
    }

    return isOk;
}

void XbhXMA311D2_T2::run(const void* arg)
{
#ifdef ENABLE_DYNAMIC_CONFIG_PD_CHARGE
    if(isFrontTypecInDetChanged() || isRearTypecInDetChanged())
    {
        gConfiguedType = PD_CONFIG_AUTO; //typec插拔状态发生了变化，需要根据当前连接状态配置充电
        gIsPDchargeCfgOk = handlePDcharge();
    }
    else if(!gIsPDchargeCfgOk)
    {
        usleep(300*1000);
        //detect status have no change, but the latest configue failed, do it again now.
        XLOGE("the latest pdPowerConfig PD charge config failed,try config it again");
        gIsPDchargeCfgOk = handlePDcharge();
    }
#endif

    usleep(80 * 1000);
}

XbhXMA311D2_T2 *XbhXMA311D2_T2::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMA311D2_T2();
        }
    }
    return mInstance;
}
