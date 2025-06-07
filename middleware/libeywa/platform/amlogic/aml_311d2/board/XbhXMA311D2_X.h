#ifndef XBH_XMA311D2X_H
#define XBH_XMA311D2X_H

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


class XbhXMA311D2_X : public XbhAmlogic_311d2,public XbhMWThread
{
public:
    static XbhXMA311D2_X* getInstance();
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi3Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getUsbc2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getDP1Det(XBH_BOOL *enable);
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
    //override
    XBH_S32 getHallSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 getLedPwrStatus(XBH_LED_LIGHT_E* enState);
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
    XBH_S32 setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length);
    //override
    XBH_S32 getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length);
    //override
    XBH_S32 getEthPlugStatus(XBH_BOOL *bEnable); 
    //override
    XBH_S32 setContrast(XBH_U32 u32Value);
    //override
    XBH_S32 getContrast(XBH_U32 *u32Value);
    //override
    XBH_S32 setBrightness(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightness(XBH_U32 *u32Value);
    //override
    XBH_S32 setHue(XBH_U32 u32Value);
    //override
    XBH_S32 getHue(XBH_U32 *u32Value);
    //override
    XBH_S32 setSharpness(XBH_U32 u32Value);
    //override
    XBH_S32 getSharpness(XBH_U32 *u32Value);
    //override
    XBH_S32 setSaturation(XBH_U32 u32Value);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    //override
    XBH_S32 getSaturation(XBH_U32 *u32Value);
    //override
    XBH_S32 setSn(const XBH_CHAR* strSn);
    //override
    XBH_S32 getSn(XBH_CHAR* strSn);
    //override
    XBH_S32 setColorTempMode(XBH_COLORTEMP_E enColorTemp);
    //override
    XBH_S32 getColorTempMode(XBH_COLORTEMP_E *enColorTemp);
    //override
    XBH_S32 setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable);
    //override
    XBH_S32 getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable);
    //override
    XBH_S32 setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx);
    //override
    XBH_S32 getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    XBH_S32 initHdmiCecPhysicalAddr();
    //override
    XBH_S32 getWakeUpReason(XBH_WAKEUP_S *stWake);
    /*
    获取MCU唤醒信息
    */
    XBH_S32 getMcuWakeUpInfo(XBH_WAKEUP_S *stWake);
    //override
    XBH_S32 setDeviceId(const XBH_CHAR* strDeviceId);
    //override
    XBH_S32 getDeviceId(XBH_CHAR* strDeviceId);

private:
    void run(const void* arg);
    XbhXMA311D2_X();
    ~XbhXMA311D2_X();
    XBH_BOOL VgaEnable;
    XBH_BOOL frontHpdState;
    XBH_BOOL frontHdmiState;
    XBH_BOOL frontUsbcState;
    XBH_S32  m_bUsbPortStatus;
    XBH_S32 getVSPort2LinkState(void);
    XBH_S32 getVSPort1LinkState(void);
    XBH_S32 setUsbNetStatus(XBH_S32 bEnable);
    XBH_S32 getUsbNetStatus(XBH_S32 *pbEnable);
    XBH_S32 getHallSensorPen(XBH_S32 *state);
    
private:
    static XbhXMA311D2_X *mInstance;
    static XbhMutex mLock;
    XBH_U8 SourceDetState[XBH_SOURCE_BUTT];
};


#endif //XBH_XMA311D2B2_H
