#ifndef XBH_XMR3576_D_H
#define XBH_XMR3576_D_H

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

class XbhXMR3576_D : public XbhRk_3576
{
public:
    static XbhXMR3576_D* getInstance();
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
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 loadEdlaCust(XBH_BOOL *enable);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getAmpInitStatus(XBH_BOOL* value);
private:
    XbhXMR3576_D();
    ~XbhXMR3576_D();

private:
    static XbhXMR3576_D *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_XMR3576_D_H
