#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_E"

#include "XbhLog.h"
#include "XbhXMM9679_E.h"
#include "XbhGSensorManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhGSensorManager.h"
#include "XbhVga2HdmiManager.h"
#include "XbhMcuManager.h"
#include "XbhAVOutManager.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhNetSwitchManager.h"
#include <string.h>
#include "XbhSysOpt.h"
#include "XbhLiftbracketManager.h"

XbhXMM9679_E            *XbhXMM9679_E::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_E::mLock;

#define SENSORDATESIZE 20
#define SENSORDATESIZE_AM1008  32
#define RJ45_SPEED_PATH     "/sys/class/net/eth0/speed"
#define IR_MONITOR_ID_PATH  "/sys/devices/platform/1c080000.ir_in/IRRc6id"
#define XBH_PMU_MONITOR_ID  "sys/mtk_pm/wakeup_key"  //wakeup_key对应寄存器未被定义使用，用于做monitor id传入pm中
#define XBH_PM_LED_ENABLE   "/sys/mtk_pm/wakeup_source/led"

static XBH_U32 mUsbCBoard = 1;

XbhXMM9679_E::XbhXMM9679_E()
{
    XLOGD(" begin ");
    VgaEnable = XBH_FALSE;
    XLOGD(" end ");
}

XbhXMM9679_E::~XbhXMM9679_E()
{
}

XBH_S32 XbhXMM9679_E::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMM9679_E::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMM9679_E::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) 
   {
        case XBH_UPDATE_BOARD_GSV2202E: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_GSV2202E: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMM9679_E::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC1, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMM9679_E::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("---- onStop ----");
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    if(s8Buff[XBH_WAKEUP_BTYPEC])
    {
        XbhUsbc2HdmiManager::getInstance()->setSpecificMode(XBH_SOURCE_USBC1);
        XLOGD("set board typec 2202E standby \n");
    }
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    XbhNetSwitchManager::getInstance()->exit();
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_GLED,!XBH_BOARD_GPIO_RJ45_LED_LEVEL);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_YLED,!XBH_BOARD_GPIO_RJ45_LED_LEVEL);
    return  s32Ret;
}

XbhXMM9679_E *XbhXMM9679_E::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_E();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMM9679_E::setOnResume(XBH_S32 state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(state == 1){
        this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_MOKA_85_TCON_WP, XBH_BOARD_GPIO_MOKA_85_TCON_BEGIN_WRITE);

        XBH_U8 xbh_tcon_60HZ_FrameBuffer[]  = {0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
        
        usleep(1400*1000);   // WP signal must be pulled high more than 50ms before SOC send I2C CMD to T-con IC.

        this->XbhMtk_9679::setIICData(2, 0x70, 0xCD, 1,6, xbh_tcon_60HZ_FrameBuffer);
        usleep(100*1000);   // WP signal must be pulled high more than 50ms before SOC send I2C CMD to T-con IC.
        this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_MOKA_85_TCON_WP, !XBH_BOARD_GPIO_MOKA_85_TCON_BEGIN_WRITE);
        
        // XbhAVOutManager::getInstance()->resetChip(MN869129);
    }
    XLOGE("setOnResume!!!!!!!!! \n");
    return s32Ret;
}
XBH_S32 XbhXMM9679_E::getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 speed = 0;
    std::string value;
    XbhSysOpt::getInstance()->readSysfs(RJ45_SPEED_PATH, value);
    speed = std::atoi(value.c_str());
    if(speed>10)
    {
        *pbStatus = XBH_TRUE;
    }
    else
    {
        *pbStatus = XBH_FALSE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::setRJ45LEDStatus(XBH_BOOL state)
{
    if (state)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_GLED,XBH_BOARD_GPIO_RJ45_LED_LEVEL);
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_YLED,XBH_BOARD_GPIO_RJ45_LED_LEVEL);
        usleep(100*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_YLED,!XBH_BOARD_GPIO_RJ45_LED_LEVEL);
        usleep(100*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_YLED,XBH_BOARD_GPIO_RJ45_LED_LEVEL);
        usleep(100*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_YLED,!XBH_BOARD_GPIO_RJ45_LED_LEVEL);
    }
    else
    {
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_GLED,!XBH_BOARD_GPIO_RJ45_LED_LEVEL);
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_RJ45_YLED,!XBH_BOARD_GPIO_RJ45_LED_LEVEL);
    }
    return 0;
}
XBH_S32 XbhXMM9679_E::setLiftbracketOperate(XBH_S32 value)
{
    XBH_S32 ret = XbhLiftbracketManager::getInstance()->setLiftbracketOperate(value);
    return ret;
}

XBH_S32 XbhXMM9679_E::getLiftbracketExist(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLiftbracketManager::getInstance()->getLiftbracketExist(value);
    return  s32Ret;
}


XBH_S32 XbhXMM9679_E::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, XBH_BOARD_GPIO_F_LED_G_ON);//关闭红灯，打开蓝灯
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_B, XBH_BOARD_GPIO_F_LED_B_ON);
            break;
        case XBH_LED_LIGHT_OFF:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_B, !XBH_BOARD_GPIO_F_LED_B_ON);
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_B, !XBH_BOARD_GPIO_F_LED_B_ON);//关闭蓝灯，打开红灯
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, !XBH_BOARD_GPIO_F_LED_G_ON);
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_E::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (bEnable)
    {
        // 控制 tcon
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, XBH_BOARD_GPIO_TCON_PWR_ON);
        // 等待
        usleep(500 * 1000);
        // 控制 blk
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, XBH_BOARD_GPIO_BLK_ON_CTRL_ON);
    }
    else
    {
        // 控制 blk
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, XBH_BOARD_GPIO_BLK_ON_CTRL_OFF);
        // 等待
        usleep(50 * 1000);
        // 控制 tcon
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, XBH_BOARD_GPIO_TCON_PWR_OFF);
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_TCON_PWR_ON ? XBH_TRUE : XBH_FALSE;
    // s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, &u32GpioValue);
    // *bEnable &= u32GpioValue == XBH_BOARD_GPIO_BLK_ON_CTRL_ON ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::setPowerLedLightEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_CHAR LedEnable[32]= {0};
    sprintf(LedEnable,"%d",bEnable);
    XbhSysOpt::getInstance()->writeSysfs(XBH_PM_LED_ENABLE, LedEnable);
    XLOGD("setPowerLedLightEnable bEnable=%d\n",bEnable);//给手动待机

    //给上电待机
    XBH_CHAR s8Buff[XBH_CUSPARAM_LED_LIGHT_ENBALE_LEN] = {0};//给上电待机
    s8Buff[0] = bEnable;
    s32Ret = XbhPartitionData::getInstance()->setLedLightEnable((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::getPowerLedLightEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string value;
    XbhSysOpt::getInstance()->readSysfs(XBH_PM_LED_ENABLE,value);
    *bEnable = std::stoi(value);
    XLOGD("getPowerLedLightEnable %s\n",*bEnable);
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::setMonitorId(XBH_S32 monitorId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("setMonitorId !! monitorId=%d\n",monitorId);
    XBH_CHAR monitorIdStr[32]= {0};
    sprintf(monitorIdStr,"%d",monitorId);  
    XbhSysOpt::getInstance()->writeSysfs(IR_MONITOR_ID_PATH,monitorIdStr);
    XbhSysOpt::getInstance()->writeSysfs(XBH_PMU_MONITOR_ID,monitorIdStr);
    XbhPartitionData::getInstance()->setMonitorId(monitorIdStr);
    return s32Ret;
}

XBH_S32 XbhXMM9679_E::getMonitorId(XBH_S32* monitorId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string value;
    XbhSysOpt::getInstance()->readSysfs(XBH_PMU_MONITOR_ID,value);
    *monitorId = std::stoi(value);
    XLOGD("getMonitorId =%d\n",*monitorId);
    return  s32Ret;
}
