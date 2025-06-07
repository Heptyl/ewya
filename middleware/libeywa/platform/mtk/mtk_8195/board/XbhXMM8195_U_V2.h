#ifndef XBH_XMM8195_U_V2_H
#define XBH_XMM8195_U_V2_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhMtk_8195.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>

class XbhXMM8195_U_V2 : public XbhMtk_8195
{
public:
    static XbhXMM8195_U_V2* getInstance();
    //----------------------override--------------------------------------
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
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
    //overrides
    XBH_S32 getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable);
    //overrides
    XBH_S32 initSrcDetGpioIrq();
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 setARCEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 setEqEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getEqEnable(XBH_BOOL* bEnable);
    //override
    XBH_S32 setCecEnable(XBH_BOOL enable);
    //override
    XBH_S32 getCecEnable(XBH_BOOL* enable);
    //override
    XBH_S32 setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable);
    //override
    XBH_S32 setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx);
    //override
    XBH_S32 getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    XBH_S32 switchHdmiRxEdid(XBH_HDMIRX_EDID_TYPE_E enType);
    //override
    XBH_S32 initProjectIdConfig();
    //override
    XBH_S32 getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    XBH_S32 initHdmiCecPhysicalAddr();
    //overrride
    XBH_S32 setPoweronMode(XBH_POWERMODE_E enMode);
    //override
    XBH_S32 getPoweronMode(XBH_POWERMODE_E *enMode);
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
    XBH_S32 getAndClearMCUNotice();
    //override
    XBH_S32 setSurroundEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getSurroundEnable(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getDP1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getOps1SignalStatus(XBH_S32 *status);

private:
    XBH_S32 setWolEnable(XBH_BOOL bEnable);
    XbhXMM8195_U_V2();
    ~XbhXMM8195_U_V2();

private:
    static XbhXMM8195_U_V2 *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_XMM8195_U_H
