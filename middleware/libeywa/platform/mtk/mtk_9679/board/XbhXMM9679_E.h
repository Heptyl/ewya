#ifndef XBH_XMM9679_E_H
#define XBH_XMM9679_E_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhMtk_9679.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>
#include "XbhDp2HdmiManager.h"
#include "XbhSourceManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhAudioCodecManager.h"

class XbhXMM9679_E : public XbhMtk_9679
{
public:
    static XbhXMM9679_E* getInstance();
    //-------------------------------------------------------------------------------override---------------------------------------------------------------
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //override
    XBH_S32 setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable);
    //override
    XBH_S32 getMicDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getHpDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getArcDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 setOnResume(XBH_S32 state);
    //override
    XBH_S32 getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus);
    //override
    XBH_S32 setRJ45LEDStatus(XBH_BOOL state);
    //override
    XBH_S32 setLiftbracketOperate(XBH_S32 value);
    //override
    XBH_S32 getLiftbracketExist(XBH_S32* value);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setPanelPower(XBH_BOOL bEnable);
    //override
    XBH_S32 getPanelPower(XBH_BOOL* bEnable);
    //override
    XBH_S32 setPowerLedLightEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getPowerLedLightEnable(XBH_BOOL* bEnable);
    //override
    XBH_S32 setMonitorId(XBH_S32 monitorId);
    //override
    XBH_S32 getMonitorId(XBH_S32* monitorId);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
private:
    XbhXMM9679_E();
    ~XbhXMM9679_E();

private:
    static XbhXMM9679_E *mInstance;
    static XbhMutex mLock;
    XBH_BOOL VgaEnable;
    XBH_U8 sensorDate[32];
    XBH_U8 PortHpdState[4];
    XBH_U8 SourceDetState[XBH_SOURCE_BUTT];
};

#endif //XBH_XMM9679_E_H
