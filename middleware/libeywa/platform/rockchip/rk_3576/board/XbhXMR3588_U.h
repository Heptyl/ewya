#ifndef XBH_XMR3588_U_H
#define XBH_XMR3588_U_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhRk_3576.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>
#include "XbhDp2HdmiManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhAudioCodecManager.h"

class XbhXMR3588_U : public XbhMWThread,public XbhMsgQueue,public XbhRk_3576
{
public:
    static XbhXMR3588_U* getInstance();
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getDP2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getDP1Det(XBH_BOOL *enable);
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
    XBH_S32 getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver);
    //override
    XBH_S32 upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType);
    //override
    XBH_S32 getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value);
    //override
    XBH_S32 setHdcpExtendIc(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type, XBH_S32 devType);
    //override
    XBH_S32 getHdcpExtIcStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable, XBH_S32 devType);
    //override
    XBH_S32 setHdcpExtIcKeyName(const XBH_CHAR* strHdcpFileName, XBH_S32 hdcpType, XBH_S32 devType);
    //override
    XBH_S32 getHdcpExtIcKeyName(XBH_CHAR* strHdcpFileName, XBH_S32 hdcpType, XBH_S32 devType);
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
    XBH_S32 getAirSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2);
    //override
    XBH_S32 getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue);
    //overrides
    XBH_S32 getGSensorExist(XBH_BOOL* bEnable);
    //overrides
    XBH_S32 initSrcDetGpioIrq();
    //overrides
    XBH_S32 getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable);
    //overrides
    XBH_S32 getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput);
    //overrides
    XBH_S32 processOpsSignalStatus(XBH_SOURCE_E u32Source);
    //override
    XBH_S32 setAvcEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getAvcEnable(XBH_BOOL* bEnable);
    //override
    XBH_S32 setMipiRxCsiControlState(XBH_SOURCE_E u32Source);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 setHdmiLowLatency(XBH_BOOL bEnable);
    //override
    XBH_S32 setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx);
    //override
    XBH_S32 getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    XBH_S32 readAndUpdateEdidBinFileByEdidType(int dev,int projectid,const char* edidBinFilePath,int port);
    //override
    XBH_S32 readAndUpdateEdidBinFile(int projectid);
    //override
    XBH_S32 initProjectIdConfig();
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    //override
    XBH_S32 getHdmiRxLockStatus(XBH_BOOL* lock);
private:
    void run(const void* arg);
    XbhXMR3588_U();
    ~XbhXMR3588_U();
    XBH_SOURCE_E mChangeToSrc;
private:
    static XbhXMR3588_U *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_XMR3588_U_H
