#ifndef XBH_XMH610_A_H
#define XBH_XMH610_A_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhHi_610.h"
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
#include "XbhMWThread.h"

class XbhXMH610_A : public XbhHi_610, public XbhMWThread
{
public:
    static XbhXMH610_A* getInstance();
    //override
    XBH_VOID checkAdcValueEffective(XBH_U32 *adcValue, XBH_U32 mDelay);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getOps1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getOps2Det(XBH_BOOL *enable);
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
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //overrides
    XBH_S32 getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable);
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
    XBH_S32 getOps1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    //override
    XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getOpsSystemType(XBH_BOOL *type, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getNTCValue(XBH_S32 s32Value, XBH_FLOAT* fValue);

private:
    XBH_S32 openEthWol(XBH_BOOL bEnable);
    XBH_S32 readAndUpdateEdidBinFile(int projectid);
    void run(const void* arg);
    XbhXMH610_A();
    ~XbhXMH610_A();

private:
    static XbhXMH610_A *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_XMH610_A_H
