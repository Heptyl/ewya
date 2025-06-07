#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM8195_U_V2"

#include <string.h>
#include <hardware/custom_edid.h>

#include "XbhLog.h"
#include "XbhXMM8195_U_V2.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhGSensorManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhDp2HdmiManager.h"
#include "XbhAVOutManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhMcuManager.h"
#include "XbhNetSwitchManager.h"
#include "XbhSysOpt.h"

XbhXMM8195_U_V2           *XbhXMM8195_U_V2::mInstance = XBH_NULL;
XbhMutex                  XbhXMM8195_U_V2::mLock;


#define RTK8367RB_VB_WOL_DEV            "/sys/devices/virtual/rtl8367rb/phy/wol"
#define HDMI_RX_EDID                    "/sys/class/hdmirxswitch/hdmi/switch_edid"
#define XBH_HAL_CHECK_SIGNAL    "vendor.xbh.hal_check_signal"

//------------------ static function begin -----------------------------
//override
XBH_S32 XbhXMM8195_U_V2::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_FHDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_FHDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_F_Type_C_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_DPIN_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DPIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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
XBH_S32 XbhXMM8195_U_V2::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

//TODO for hardware test
static XBH_BOOL gOpsDetGpioValue = XBH_FALSE;
//override
XBH_S32 XbhXMM8195_U_V2::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
	if (gOpsDetGpioValue != u32GpioValue)
        {
            XLOGE("Fixed me! 1: s32Ret = %d, gOpsDetGpioValue = %d, u32GpioValue = %d\n", s32Ret, gOpsDetGpioValue, u32GpioValue);
            usleep(300 * 1000);
            s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
            XLOGE("Fixed me! 2: s32Ret = %d, gOpsDetGpioValue = %d, u32GpioValue = %d\n", s32Ret, gOpsDetGpioValue, u32GpioValue);
            gOpsDetGpioValue = u32GpioValue;
        }
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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
XBH_S32 XbhXMM8195_U_V2::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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
XBH_S32 XbhXMM8195_U_V2::getGSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 0;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 adcValue = 0;

#if defined(XBH_BOARD_GPIO_HALL_ADC) && defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    s32Ret = this->XbhMtk_8195::getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_HALL, 0x01, 0x01, &adcValue);
    if (s32Ret == XBH_SUCCESS)
    {
        if (adcValue <= XBH_BOARD_GPIO_HALL_NOT_TRIG_ADC_VALUE)
        {
            *s32Value = 2;
        }
        else if (adcValue > XBH_BOARD_GPIO_HALL_BOTH_TRIG_ADC_VALUE)
        {
            *s32Value = 0;
        }
        else
        {
            *s32Value = 1;
        }
    }
    else
    {
        XLOGE("read adc value error \n");
    }
#endif
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

/**
* 设置EQ曲线(频率分布曲线)开关，可以用来打开或者关闭该曲线用于测试
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::setEqEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setEqEnable(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXA, XBH_HDMI4_CEC_PHYSICAL_ADDR);// ops 
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXB, XBH_HDMI2_CEC_PHYSICAL_ADDR);// to next 
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXC, XBH_HDMI1_CEC_PHYSICAL_ADDR);//HDMI  IN
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXD, XBH_HDMI3_CEC_PHYSICAL_ADDR);//dp

    //默认继承上级switch，需要GSV版本支持此命令
    if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT2_S2_BOARD)
    {
       s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_FRONT_GSV2712, XBH_HDMI_RXA, XBH_HDMI5_CEC_PHYSICAL_ADDR); //typec 
       s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_FRONT_GSV2712, XBH_HDMI_RXB, XBH_HDMI2_CEC_PHYSICAL_ADDR);// HDMI
    }
    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::setPoweronMode(XBH_POWERMODE_E enMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[1] = {0};

    pu8Data[0] = enMode;

    s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_POWERON_MODE, 1, 1, pu8Data);

    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getPoweronMode(XBH_POWERMODE_E *enMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[1] = {0};

    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_POWERON_MODE, 1, 1, pu8Data);
    *enMode = (XBH_POWERMODE_E)pu8Data[0];

    return s32Ret;
}


/**
 * 设置低音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::setBass(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setBass(s32Value);
    return  s32Ret;
}

/**
 * 获取低音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::getBass(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getBass(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getBass fail!");
    }
    return  s32Ret;
}

/**
 * 设置高音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::setTreble(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setTreble(s32Value);
    return  s32Ret;
}

/**
 * 获取高音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::getTreble(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getTreble(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getTreble fail!");
    }
    return  s32Ret;
}

/**
 * 设置左右声道平衡
 * param[in] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::setBalance(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setBalance(s32Value);
    return  s32Ret;
}

/**
 * 获取左右声道平衡
 * param[out] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::getBalance(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getBalance(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getInstance fail!");
    }
    return  s32Ret;
}
/**
 * 设置音效模式
 * param[in] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setSoundMode(enSoundMode);
    return  s32Ret;
}

/**
 * 获取音效模式
 * param[out] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::getSoundMode(XBH_SOUND_MODE_E *enSoundMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getSoundMode(enSoundMode);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getInstance fail!");
    }
    return  s32Ret;
}

/**
 * 设置环绕声开关
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::setSurroundEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setSurroundEnable(bEnable);
    return  s32Ret;
}

/**
 * 获取环绕声开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::getSurroundEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getSurroundEnable(bEnable);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getInstance fail!");
    }
    return  s32Ret;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_U_V2::getHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_U_V2::getFHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_U_V2::getFUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_U_V2::getDP1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_U_V2::getOps1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
* 获取EQ曲线(频率分布曲线)开关
* param[in] bEnable XBH_TRUE：开EQ曲线，XBH_FALSE：关EQ曲线
* retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_U_V2::getEqEnable(XBH_BOOL* bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getEqEnable(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::setLedPwrStatus(XBH_LED_LIGHT_E enState)
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

XBH_S32 XbhXMM8195_U_V2::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
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

XBH_S32 XbhXMM8195_U_V2::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMM8195_U_V2::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_DP1, ver);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_ITE6563OUT, ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;

        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_CS5803AQ:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_CS5803AQ, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_ITE6563OUT, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;

        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}


XBH_S32 XbhXMM8195_U_V2::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        case XBH_UPDATE_ITE6563OUT:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_ITE6563OUT, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::setWolEnable(XBH_BOOL bEnable)
{
    int fd = open(RTK8367RB_VB_WOL_DEV, O_WRONLY);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR mac_addr[17] = {0};
    getMacAddress(0, mac_addr);
    if (bEnable)
    {
        if (fd == -1)
        {
            XLOGE(" YANDONG open_wol dev error!!!\n");
            return s32Ret;
        }
        //1，初始化
        XLOGD("write wol init\n");
        s32Ret = write(fd, "init", sizeof("init"));
        if (s32Ret < 0)
        {
            XLOGE("write wol init dev error!!!\n");
        }
        //2，写入MAC地址
        getMacAddress(0, mac_addr);
        XLOGD("write mac:%s\n",mac_addr);
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
XBH_S32 XbhXMM8195_U_V2::getAndClearMCUNotice(){
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR) 
    XBH_U8 pu8Data[1] = {0xFF};
    do{
        s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_NOTICE, 1, 1,pu8Data);
        XLOGD("%s: %d pu8Data:0x%02X\n", __FUNCTION__, __LINE__,pu8Data[0]);
        if(s32Ret == XBH_FAILURE){
            break;
        }
        usleep(30*1000);
    }while(pu8Data[0] != CMD_NOTICE_NONE);
    #endif
    return s32Ret;
}
XBH_S32 XbhXMM8195_U_V2::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{
    XLOGD("%s: %d gpio = %d u32GpioValue = %d\n", __FUNCTION__, __LINE__, gpio ,u32GpioValue);
    XBH_S32 s32Ret = XBH_SUCCESS;
    switch(gpio+XBH_GPIO_8195_BASE){
        #ifdef XBH_BOARD_GPIO_SOC_MCU_DET
        case XBH_BOARD_GPIO_SOC_MCU_DET:
            s32Ret = getAndClearMCUNotice();
            break;
        #endif
        default:
            break;
    }
    return  s32Ret;
}
XBH_S32 XbhXMM8195_U_V2::initSrcDetGpioIrq()
{
    XLOGD("%s: %d\n", __FUNCTION__, __LINE__);
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_SOC_MCU_DET
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_SOC_MCU_DET, IRQF_TRIGGER_RISING|IRQF_ONESHOT, XBH_FALSE);
    #endif
    s32Ret |= getAndClearMCUNotice();
    return  s32Ret;
}
XBH_S32 XbhXMM8195_U_V2::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    char sWolStatus[PROPERTY_VALUE_MAX] = {0};
    XBH_BOOL wot_enable;
    s32Ret = getWoTStatus(XBH_WAKEUP_FTYPEC, &wot_enable);
    if(wot_enable) {
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
    }

    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    XLOGD("---- onStop ----");
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    setGpioOutputValue(XBH_BOARD_GPIO_AMP_MUTE,XBH_BOARD_GPIO_AMP_MUTE_OFF);//关闭AMP PD
    usleep(10*1000);
    setGpioOutputValue(XBH_BOARD_GPIO_AMP_EN,XBH_BOARD_GPIO_AMP_EN_OFF);//关闭AMP PVDD
    setGpioOutputValue(XBH_BOARD_GPIO_USB_SW_C_EN,XBH_BOARD_GPIO_LOW);//关闭USB3.0 switch使能
    setGpioOutputValue(XBH_BOARD_GPIO_F_USBC_DET,!XBH_BOARD_GPIO_F_USBC_DET_LEVEL);//关闭前置板Typec插入检测脚
    setGpioOutputValue(XBH_BOARD_GPIO_LCD_CHIP_POWER,!XBH_BOARD_GPIO_LCD_CHIP_POWER_ON);//关闭cs5803供电脚
    /*通知MCU SOC已待机*/
    pI2cBuff.cmd = CMD_I2C_SOC_POWER_MODE;
    pI2cBuff.len = 1;
    pI2cBuff.data[0] = 0XFE; // 待机
    s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    #endif
    /*网络唤醒设置*/
    property_get("persist.vendor.xbh.wol.enable", sWolStatus, "false");
    XLOGE("YANDONG persist.vendor.xbh.wol.enable : %s \n",sWolStatus);
    if(strncmp(sWolStatus, "true",4) == 0) {
        setWolEnable(XBH_TRUE);
    } else {
        setWolEnable(XBH_FALSE);
    }

    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_U_V2::setARCEnable(XBH_BOOL bEnable)
{
      XBH_S32 s32Ret = XBH_SUCCESS;
      XLOGD("setARCEnable :%d, speaker mute:%s", bEnable, bEnable?"off":"on");

      if (bEnable)
      {
          setGpioOutputValue(XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW, XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW_ARC);
          setMute(XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
      }
      else
      {
          setGpioOutputValue(XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW, XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW_SPIDF);
          setMute(XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
      }

      return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::setCecEnable(XBH_BOOL enable) {
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    XLOGD("setCecEnable: %d", enable);
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_SET_WAKEUP_CEC;
    pI2cBuff.len = 1;
    pI2cBuff.data[0] = enable ? 1 : 0;
    s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    #endif
    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getCecEnable(XBH_BOOL* enable) {

    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    XBH_U8 cec_enable = 0;
    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_CEC, 1, 1, &cec_enable);
    XLOGD("getCecEnable: %d", cec_enable);
    *enable = cec_enable; // XBH_TRUE : XBH_FALSE
    #endif
    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[1] = {0};
    XLOGD("enWakeUpType :%d setWoTStatus: %d", enWakeUpType, bEnable);
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    pu8Data[0] = bEnable;
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_FHDMI:
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_HDMI, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_FTYPEC:
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_USBC, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_NFC:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_NFC, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_SOURCE:
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_HDMI, 1, 1, pu8Data);
            s32Ret |= setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_USBC, 1, 1, pu8Data);
            //mcu 包括了DP 和HDMI IN
            s32Ret |= setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_HDMI, 1, 1, pu8Data);
            break;
        case XBH_WAKEUP_ETH:
            //通知MCU打开WOL
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_WOL, 1, 1, pu8Data);
            //控制交换机开启网络唤醒
            if(bEnable)
            {
                property_set("persist.vendor.xbh.wol.enable", "true");
            }
            else
            {
                property_set("persist.vendor.xbh.wol.enable", "false");
            }
        break;
        default:
            XLOGE("no type!");
        break;
    }
    #endif
    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[1] = {0};
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_FHDMI:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_F_HDMI, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_FTYPEC:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_F_USBC, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_SOURCE:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_HDMI, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_NFC:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_NFC, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_ETH:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_WOL, 1, 1, pu8Data);
        break;
        default:
            XLOGE("no type!");
        break;
    }
    *bEnable = pu8Data[0];
    #endif
    XLOGD("enWakeUpType :%d getWoTStatus: %d", enWakeUpType, *bEnable);
    return s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 type  = 2;

    //XLOGD("set Hdmi rx Info enType = %d",enType);
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            type = 1;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"0");
        break;
        case XBH_HDMIRX_EDID_TYPE_20:
            type = 2;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"1");
        break;
        case XBH_HDMIRX_EDID_TYPE_21:
            type = 3;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"2");
        break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            type = 4;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"3");
        break;
        default:
            XLOGE("no type!\n");
            s32Ret = XBH_FAILURE;
        break;
    }

    s32Ret = XbhMtk_8195::setHdmirxEdidType(type);
    XLOGD("setHdmirxEdidType %d, ret %d\n", type, s32Ret);

    return  s32Ret;
}

XBH_S32 XbhXMM8195_U_V2::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type  = 2;
    s32Ret = XbhMtk_8195::getHdmirxEdidType(&type);
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

#if 1
/**
 * 切换hdmirx edid
 */
XBH_S32 XbhXMM8195_U_V2::switchHdmiRxEdid(XBH_HDMIRX_EDID_TYPE_E enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int bd_idx = 0; 

    //当前 8195.U所有通道都使用同一个EDID
    s32Ret = setHdmirxEdidType(enType, bd_idx);

    return s32Ret;
}

/**
 * 初始化project_id配置
 */
XBH_S32 XbhXMM8195_U_V2::initProjectIdConfig()
{
    XBH_S32 s32Ret = XBH_SUCCESS; 
    int bd_idx = 0; 
    XBH_HDMIRX_EDID_TYPE_E type  = XBH_HDMIRX_EDID_TYPE_BUTT;

    //每次开机都需要更新edid的数据
    getHdmirxEdidType(bd_idx, &type);
    s32Ret = switchHdmiRxEdid((XBH_HDMIRX_EDID_TYPE_E)type);

    return s32Ret;
}
#endif
//------------------ static function end -----------------------------


//------------------ public function begin -----------------------------

//------------------ public function end -----------------------------


//------------------ private function begin -----------------------------

//------------------ private function end -----------------------------

XbhXMM8195_U_V2::XbhXMM8195_U_V2()
{
    XLOGD(" begin ");
    #ifdef XBH_USE_HAL_API_TO_CHECK_SIGNAL_STATUS
    property_set(XBH_HAL_CHECK_SIGNAL, "true");
    #endif
    XLOGD(" end ");
}

XbhXMM8195_U_V2::~XbhXMM8195_U_V2()
{
}

XbhXMM8195_U_V2 *XbhXMM8195_U_V2::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM8195_U_V2();
        }
    }
    return mInstance;
}
