#ifndef XBH_XMM8195_G_H
#define XBH_XMM8195_G_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhMtk_8195.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>
#include "XbhHdmiSwitchManager.h"
#include "XbhPdIcManager.h"

using namespace std;

//LED 的模式
enum XBH_LED_CTRL_MODS {
    XBH_LED_HOME_ON,       // HOME键白灯（常亮）
    XBH_LED_HOME_BREATH,   // HOME键呼吸灯（呼吸效果）
    XBH_LED_HOME_OFF,      // HOME键灯灭（关闭）
    XBH_LED_HOME_BUTT
};

class XbhXMM8195_G : public XbhMtk_8195, public XbhMWThread
{
public:
    static XbhXMM8195_G* getInstance();
    //----------------------override--------------------------------------
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getDP1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getOps1Det(XBH_BOOL *enable);
    //override
    XBH_S32 setOpsPowerCtrl(XBH_SOURCE_E enSource);
    //override
    XBH_S32 getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getGSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue);
    //override
    XBH_S32 getPirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getPirSensorValue(XBH_S32 *s32Value);
    //override
    XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getLightSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getLightSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 getLedPwrStatus(XBH_LED_LIGHT_E* enState);
    //override
    XBH_S32 setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable);
    //override
    XBH_S32 getMicDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getHpDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getArcDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //overrride
    XBH_S32 initHdmiCecPhysicalAddr();
    //overrride
    XBH_S32 setPoweronMode(XBH_POWERMODE_E enMode);
    //override
    XBH_S32 getPoweronMode(XBH_POWERMODE_E *enMode);
    //override
    XBH_S32 setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable);
    //override
    XBH_S32 getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    //override
    XBH_S32 setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx);
    //override
    XBH_S32 getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    XBH_S32 switchHdmiRxEdid(XBH_HDMIRX_EDID_TYPE_E enType);
    //override
    XBH_S32 initProjectIdConfig();
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 setARCEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 setCecEnable(XBH_BOOL enable);
    //override
    XBH_S32 getCecEnable(XBH_BOOL* enable);
    /*
    设置呼吸灯模式
    */
    XBH_S32 setLedMode(XBH_LED_CTRL_MODS mode);
    /*
    判断当前是否是呼吸灯模式
    */
    XBH_BOOL isBreathMode();
    //override
    XBH_S32 setFlashEepData(string pBuff);
    //override
    XBH_S32 getFlashEepData(string &pBuff);
    //override
    XBH_S32 setBass(XBH_S32 s32Value);
    //override
    XBH_S32 getBass(XBH_S32 *s32Value);
    //override
    XBH_S32 setTreble(XBH_S32 s32Value);
    //override
    XBH_S32 getTreble(XBH_S32 *s32Value);
    //override
    XBH_S32 setBalance(XBH_S32 s32Value);
    //override
    XBH_S32 getBalance(XBH_S32 *s32Value);
    //override
    XBH_S32 setSoundMode(XBH_SOUND_MODE_E enSoundMode);
    //override
    XBH_S32 getSoundMode(XBH_SOUND_MODE_E *enSoundMode);
    //override
    XBH_S32 setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable);
    //override
    XBH_S32 setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setSurroundEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getSurroundEnable(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getOps1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 setTypeCPdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);
    //override
    XBH_S32 setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);
private:
    XbhXMM8195_G();
    ~XbhXMM8195_G();
    XBH_BOOL pwmInit(XBH_S32 number);
    XBH_BOOL privatePwmWrite(string path, string value);

private:
    void run(const void *arg);
    static XbhXMM8195_G *mInstance;
    static XbhMutex mLock;
    string mPwmindex;
    int pwm_duty_fd;
    static XBH_S32 mBreathMode;
    static const string PWM_PREFIX;
    static const XBH_U32 MAX_PWM_NAME_SIZE;
    static const string PWMEXPORT;
    static const string PWM_PERIOD;
    static const string PWM_PERIOD_PATH;
    static const string PWM_ENABLE_PATH;
    static const string PWM_DYTY_CYCLE_PATH;
    static XBH_U32      BASE_DUTY_CYCLE;
    static const XBH_U32 BREATH_DUTY;
    static const XBH_U32 VERY_LOW_BRIGHTNESS_SPLIT;
    static const XBH_U32 LOW_BRIGHTNESS_SPLIT;
    static const XBH_U32 MIDDLE_LOW_BRIGHTNESS_SPLIT;
    static const XBH_U32 MIDDLE_BRIGHTNESS_SPLIT;
    static const XBH_U32 HIGH_BRIGHTNESS_SPLIT;
    static const XBH_U32 VERY_LOW_BRIGHTNESS;
    static const XBH_U32 LOW_BRIGHTNEWSS;
    static const XBH_U32 MIDDLE_LOW_BRIGHTNESS;
    static const XBH_U32 MIDDLE_BRIGHTNESS;
    static const XBH_U32 MIDDLE_HIGH_BRIGHTNESS;
    static const XBH_U32 HIGH_BRIGHTNESS;
    static const XBH_U32 MAX_BRIGHTNESS;
    static const string  LENDATAPART;
    static const XBH_U32 LENDATALENTPART;
    static const XBH_U32 MAXLENDATAONCEREADLEN;
};

#endif //XBH_XMM8195_G_H
