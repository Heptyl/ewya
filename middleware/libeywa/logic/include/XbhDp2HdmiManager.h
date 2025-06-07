#ifndef __XBH_DP_2_HDMI_MANAGER_H__
#define __XBH_DP_2_HDMI_MANAGER_H__

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMutex.h"
#include "XbhDp2HdmiTask.h"

class XbhDp2HdmiManager
{
public:
    static XbhDp2HdmiManager* getInstance();
    XBH_S32 setRxHpd(XBH_SOURCE_E src, XBH_U8 value);
    XBH_S32 resetChip(XBH_SOURCE_E src);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State);

    ~XbhDp2HdmiManager();

private:
    XbhDp2HdmiManager();
    static XbhDp2HdmiManager *mInstance;
    static XbhMutex mLock;

    XbhDp2HdmiTask *m_pXbhDp2HdmiTask;
};

#endif //__XBH_DP_2_HDMI_MANAGER_H__
