#ifndef XBH_XMM9679_B_H
#define XBH_XMM9679_B_H

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

class XbhXMM9679_B : public XbhMtk_9679
{
public:
    static XbhXMM9679_B* getInstance();
    XBH_S32 followToOps1(XBH_VOID);
    XBH_S32 followToInit(XBH_VOID);
    //-------------------------------------------------------------------------------override---------------------------------------------------------------
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getUsbc1Det(XBH_BOOL *enable);
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
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //override
    XBH_S32 getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2);
    //override
	XBH_S32 getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue);
    //override
    XBH_S32 getPirSensorValue(XBH_S32 *s32Value);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getPirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getLightSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getLightSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 setUsbLockMode(XBH_U32 u32UsbType);
    //override
    XBH_S32 getUsbLockMode(XBH_U32 *u32UsbType);
    //override
    XBH_S32 setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable);
    //override
    XBH_S32 getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable);
    //override
    XBH_S32 setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data);
    //override
    XBH_S32 getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info);
    //override
    XBH_S32 getMicDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getHpDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getArcDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 getAirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getGSensorExist(XBH_BOOL* bEnable);
    XBH_S32 switchSourceHpd(XBH_U8 port, XBH_SOURCE_E SourceHdmi, XBH_SOURCE_E SourceOther);
    //override
    XBH_S32 syncBuildinEthMacAddress(void);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value);
    //override
    XBH_S32 followToFUsbc1(XBH_VOID);
    //override
    XBH_S32 followToFHdmi1(XBH_VOID);
    //override
    XBH_S32 setOnResume(XBH_S32 state);
    //override
    XBH_S32 setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getLedPwrStatus(XBH_LED_LIGHT_E* enState);
    //override
    XBH_S32 setOpsResetEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getTCONValue(XBH_S32* value);
    //override
    XBH_S32 setLiftbracketOperate(XBH_S32 value);
    //override
    XBH_S32 getLiftbracketExist(XBH_S32* value);
    //override
    XBH_S32 setBacklightData(XBH_U32 data);
    //override
    XBH_S32 setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress);
    //override
    XBH_S32 setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getHdmi2SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getDP1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getOps1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
private:
    XbhXMM9679_B();
    ~XbhXMM9679_B();

private:
    static XbhXMM9679_B *mInstance;
    static XbhMutex mLock;
    XBH_U8 tconDate[4];
    XBH_U8 sensorDate[32];
    XBH_U8 PortHpdState[4];
    XBH_U8 SourceDetState[XBH_SOURCE_BUTT];
};

#endif //XBH_XMM9679_B_H
