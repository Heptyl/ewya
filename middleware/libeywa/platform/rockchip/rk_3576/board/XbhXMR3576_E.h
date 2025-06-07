#ifndef XBH_XMR3576_E_H
#define XBH_XMR3576_E_H

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
#include "XbhAVOutManager.h"
#include "XbhPlatformInterface.h"
#include "XbhVga2HdmiManager.h"
class XbhXMR3576_E : public XbhRk_3576, public XbhMWThread
{
public:
    static XbhXMR3576_E* getInstance();
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
    XBH_S32 getVga1Det(XBH_BOOL *enable);
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
    XBH_S32 autoAdjust(XBH_VOID);
    //override
    XBH_S32 setHPosition(XBH_U32 u32Value);
    //override
    XBH_S32 getHPosition(XBH_U32* u32Value);
    //override
    XBH_S32 setVPosition(XBH_U32 u32Value);
    //override
    XBH_S32 getVPosition(XBH_U32* u32Value);
    //override
    XBH_S32 setClock(XBH_U32 u32Value);
    //override
    XBH_S32 getClock(XBH_U32* u32Value);
    //override
    XBH_S32 setPhase(XBH_U32 u32Value);
    //override
    XBH_S32 getPhase(XBH_U32* u32Value);
    //override
    XBH_S32 setHdmiLowLatency(XBH_BOOL bEnable);
    //override
    XBH_S32 setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource);
    //override
    XBH_S32 setWifiChanelPlan(XBH_WIFI_TYPE wifiType, const XBH_CHAR* channel);
    //override
    XBH_S32 setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length);
    //override
    XBH_S32 getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length);
    //override
    XBH_S32 setHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E hdcpTxEncryptionMode);
    //override
    XBH_S32 getHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E *hdcpTxEncryptionMode);
    //override
    XBH_S32 getAntennaRssi(XBH_CHAR* ver);
    //override
    XBH_S32 burnFactorySN(XBH_MATERAL_TYPE materialType, const XBH_CHAR* snInfo);
    //override
    XBH_S32 getBurnFactorySN(XBH_MATERAL_TYPE materialType, XBH_CHAR* snInfo);
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
    XBH_S32 getFHdmi1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getFUsbc1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getVga1SignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getEthPortSpeed(XBH_S32 port, XBH_S32* value);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    //override
    XBH_S32 setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId);
    //override
    XBH_S32 getSourceEdidType(XBH_U32 u32SourceId,XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    /*
    返回单独每个通道的hdcpvalue
    */
    XBH_S32 getHdcpKeydata(XBH_SOURCE_E source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff);
    //override
    XBH_S32 setTypecReset(XBH_SOURCE_E src);
    //override
    XBH_S32 ProcessTypeBHotplug(XBH_SOURCE_E src);
    //override
    XBH_S32 ProcessTypeCHotplug(XBH_SOURCE_E src);
private:
    XBH_S32 openEthWol(XBH_BOOL bEnable);
    XBH_BOOL isOTGMode(XBH_VOID);
    XBH_S32 readAndUpdateEdidBinFile(int projectid);
    XBH_S32 processHpdUsbSwitch(XBH_SOURCE_E src);
    XBH_S32 processNetSwitch(XBH_S32 netchoice);
    XbhXMR3576_E();
    ~XbhXMR3576_E();

private:
    void run(const void *arg);
    static XbhXMR3576_E *mInstance;
    static XbhMutex mLock;
    static XBH_BOOL mOTGMode;
    const static XBH_U32 M_PS_ON_BASE_DELAY_TIME;
    const static XBH_U32 M_PS_ON_DELAY_TIME_STEP;
    const static XBH_U32 M_PWR_STA_DECT_TIMES;
    XBH_BOOL mOpsOnFlag = XBH_FALSE;
};

#endif //XBH_XMR3576_E_H
