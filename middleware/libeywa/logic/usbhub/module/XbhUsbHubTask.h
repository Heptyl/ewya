#ifndef XBH_USB_HUB_TASK_H
#define XBH_USB_HUB_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhModuleInterface.h"
#include "XbhUsbHubInterface.h"

class XbhUsbHubTask : public XbhMWThread
{
public:
    static XbhUsbHubTask* getInstance();
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_S32 devType, XBH_S32 * s32State);

    ~XbhUsbHubTask();

private:
    void run(const void* arg);
    XbhUsbHubTask();
    XbhUsbHubInterface *getCurrIC(XBH_S32 devType);
    static XbhUsbHubTask *mInstance;
    static XbhMutex mLock;

    XbhUsbHubInterface *m_pXbhUsbHubInterface[2];
    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif //XBH_USB_HUB_TASK_H