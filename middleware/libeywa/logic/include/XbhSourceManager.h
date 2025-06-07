#ifndef __XBH_SOURCE_MANAGER_H__
#define __XBH_SOURCE_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMWThread.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhFollowManager.h"

class XbhSourceManager : public XbhMWThread
{
public:
    static XbhSourceManager* getInstance();
    XBH_S32 selectSource(XBH_SOURCE_E u32Source, XBH_WINDOW_NUM_E win);
    XBH_S32 setSourceSwitchPort(XBH_SOURCE_E u32Source);
    XBH_S32 setSourceUsbPort(XBH_SOURCE_E u32Source);
    XBH_S32 getSourceDet(XBH_SOURCE_E u32Source, XBH_BOOL *bEnable);
    XBH_S32 getSourceSignalStatus(XBH_SOURCE_E u32Source, XBH_S32 *status);
    XBH_S32 getCurrSource(XBH_SOURCE_E *u32Source);
    XBH_S32 pushSourcePlugStatus(XBH_SOURCE_E u32Source, XBH_BOOL status);

    ~XbhSourceManager();

private:
    void run(const void* arg);
    XBH_S32 processSrcConnectStatus();
    XBH_S32 processSrcSignalStatus();
    XBH_S32 processOpsSignalStatus(XBH_SOURCE_E u32Source);
    XbhSourceManager();

    static XbhSourceManager *mInstance;
    static XbhMutex mLock;

    XbhHdmiSwitchManager *m_pXbhHdmiSwitchManager;
    XbhFollowManager *m_pXbhFollowManager;

    XBH_SOURCE_E mCurrentSrc;
};

#endif //__XBH_SOURCE_MANAGER_H__
