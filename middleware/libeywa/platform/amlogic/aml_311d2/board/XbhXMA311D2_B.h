#ifndef XBH_XMA311D2B_H
#define XBH_XMA311D2B_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhAmlogic_311d2.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>
#include "XbhVga2HdmiManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"


class XbhXMA311D2_B : public XbhAmlogic_311d2
{
public:
    static XbhXMA311D2_B* getInstance();
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
    //XBH_S32 getDP1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getVga1Det(XBH_BOOL *enable);
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
    XBH_S32 setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value);
    // //override
    // XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getPirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getPirSensorValue(XBH_S32 *s32Value);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 getLightSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getLightSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable);
    //override
    XBH_S32 getMicDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 getHpDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //override
    XBH_S32 setUsbLockMode(XBH_U32 u32UsbType);
    //override
    XBH_S32 getUsbLockMode(XBH_U32 *u32UsbType);
    //override
    XBH_S32 setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx);
    //override
    XBH_S32 getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable);
    //override
    XBH_S32 dumpEdid(XBH_SOURCE_E idx);
    //override
    XBH_S32 getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum);
    //override
    XBH_S32 setWolEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
private:
    XbhXMA311D2_B();
    ~XbhXMA311D2_B();

private:
    static XbhXMA311D2_B *mInstance;
    static XbhMutex mLock;
};


#endif //XBH_XMA311D2B2_H
