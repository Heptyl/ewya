#ifndef XBH_XMR3576_B_H
#define XBH_XMR3576_B_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhRk_3576.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>
#include "XbhDp2HdmiManager.h"
#include "XbhMcuManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhAirSensorManager.h"
#include "XbhPlatformInterface.h"

class XbhXMR3576_B : public XbhRk_3576
{
public:
    static XbhXMR3576_B* getInstance();
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
    XBH_S32 getOut1Det(XBH_BOOL *enable);
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
    XBH_S32 getAirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2);
    //override
    XBH_S32 setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable);
    //override
    XBH_S32 getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable);
    //override
    XBH_S32 getMicDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getHpDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getArcDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getPirSensorValue(XBH_S32 *s32Value);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getPirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //overrides
    XBH_S32 getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable);
    //overrides
    XBH_S32 setHdmiRx5vDet(XBH_BOOL enable);
    //overrides
    XBH_S32 initSrcDetGpioIrq();
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 setUsbLockMode(XBH_U32 u32UsbType);
    //override
    XBH_S32 getUsbLockMode(XBH_U32 *u32UsbType);
    //override
    XBH_S32 setAvcEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getAvcEnable(XBH_BOOL* bEnable);
    //override
    XBH_S32 setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data);
    //override
    XBH_S32 getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info);
    //override
    XBH_S32 setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx);
    //override
    XBH_S32 getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    XBH_S32 setLiftbracketOperate(XBH_S32 value);
    //override
    XBH_S32 getLiftbracketExist(XBH_S32* value);
    //override
    XBH_S32 setHdmiLowLatency(XBH_BOOL bEnable);
    //override
    XBH_S32 setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);
    //override
    XBH_S32 setOpsResetEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 initProjectIdConfig();
    //override
    XBH_S32 getHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getHdmi2SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getDP1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getOps1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
private:
    XBH_S32 openEthWol(XBH_BOOL bEnable);
    XBH_S32 readAndUpdateEdidBinFile(int projectid);
    XbhXMR3576_B();
    ~XbhXMR3576_B();

private:
    static XbhXMR3576_B *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_XMR3576_B_H
