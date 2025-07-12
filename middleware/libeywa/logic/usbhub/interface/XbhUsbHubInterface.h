#ifndef __XBH_USB_HUB_INTERFACE_H__
#define __XBH_USB_HUB_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhUsbHubInterface
{
public:
    /**
     * 获取固件版本
     */
    virtual XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    virtual XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
    virtual XBH_S32 getUpgradeState(XBH_S32 * s32State);
public:
    XbhUsbHubInterface(){};
    virtual ~XbhUsbHubInterface(){};
};

#endif
