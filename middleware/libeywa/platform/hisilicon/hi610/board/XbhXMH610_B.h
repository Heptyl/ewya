#ifndef XBH_XMH610_B_H
#define XBH_XMH610_B_H

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

class XbhXMH610_B : public XbhHi_610, public XbhMWThread
{
public:
    static XbhXMH610_B* getInstance();
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
    XBH_S32 getOps2SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFHdmi1SignalStatus(XBH_S32 *status);
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
    //override
    XBH_S32 getOpsWakeSoc(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getEdidModelName(std::string &pBuff);
    //override
    XBH_S32 getEdidSn(std::string &pBuff);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 getLedPwrStatus(XBH_LED_LIGHT_E* enState);
    //override
    XBH_S32 getOpsOperationStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 setOpsRecovery(XBH_BOOL bEnable, XBH_SOURCE_E enSource);
    //override
    XBH_S32 getOPSBypassDataLen(XBH_U8* u8Data);
    //override
    XBH_S32 SendOPSBypassData(XBH_SOURCE_E type, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 setReferRTCInfo(const XBH_CHAR* pBuff);
    //override
    XBH_S32 getReferRTCInfo(XBH_CHAR* pBuff);
    //override
    XBH_S32 getUSBCForwardReverseInsertionDet(XBH_SOURCE_E idx, XBH_S32 *u32Value);
    //override
    XBH_S32 setSecurityHvbKey();
    //override
    XBH_S32 getEdidInvaild(XBH_HDMIRX_EDID_TYPE_E edidType, XBH_U8 *edidInvaild);
    //override
    XBH_S32 setTypecReset(XBH_SOURCE_E src);
    

private:
    XBH_S32 openEthWol(XBH_BOOL bEnable);
    XBH_S32 readAndUpdateEdidBinFile(int projectid);
    XBH_U8 CalcCRC8(uint8_t* Buffer, uint8_t BufferLen);
    XBH_U16 crc16(XBH_U16 crc, XBH_U8* buffer, XBH_U8 size);
    XBH_U16 Crc16CCIT(XBH_U8* data, XBH_U8 size);
    XBH_S32 OPSByPassProcess();
    XBH_S32 OPSCRCBypassData(XBH_SOURCE_E type, XBH_BOOL RWflag, XBH_U8 u8Len, XBH_U8* u8Data);
    XBH_S32 CRCTimeoutCheck();
    XBH_S32 setOpsPowerCtrlWithDurationMs(XBH_SOURCE_E enSource);
    void run(const void* arg);
    static void* setOpsPowerCtrlTask(void* arg);
    XBH_CRC_RESULT_STATUS CrcResult;
    XbhXMH610_B();
    ~XbhXMH610_B();

private:
    static XbhXMH610_B *mInstance;
    static XbhMutex mLock;
    const static XBH_U32 M_PS_ON_BASE_DELAY_TIME;
    const static XBH_U32 M_PS_ON_DELAY_TIME_STEP;
    const static XBH_U32 M_PWR_STA_DECT_TIMES;
    //mOps1OnFlag:ture turn on ops1,XBH_FALSE:need'nt to turn on ops1
    XBH_BOOL mOps1OnFlag = XBH_FALSE;
    //mOps1OnFlag:ture turn on ops2,XBH_FALSE:need'nt to turn on ops2
    XBH_BOOL mOps2OnFlag = XBH_FALSE;
};

#endif //XBH_XMH610_B_H
