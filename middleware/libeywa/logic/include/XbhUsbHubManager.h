#ifndef XBH_USB_HUB_MANAGER_H
#define XBH_USB_HUB_MANAGER_H

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMutex.h"
#include "XbhUsbHubTask.h"
#include <cutils/properties.h>
#include <hardware/board.h>


class XbhUsbHubManager
{
public:
    static XbhUsbHubManager* getInstance();
    /* 获取固件版本*/
    XBH_S32 getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion);
    /* 进行固件升级*/
    XBH_S32 upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /* 获取固件升级的状态*/
    XBH_S32 getUpgradeState(XBH_S32 devType, XBH_S32 * s32State);

    ~XbhUsbHubManager();

private:
    XbhUsbHubManager();
    static XbhUsbHubManager *mInstance;
    static XbhMutex mLock;
    XbhUsbHubTask *m_pXbhUsbHubTask;
};

#endif //XBH_USB_HUB_MANAGER_H
