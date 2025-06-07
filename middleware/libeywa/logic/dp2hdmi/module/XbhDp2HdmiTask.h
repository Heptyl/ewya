#ifndef XBH_DP_2_HDMI_TASK_H
#define XBH_DP_2_HDMI_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhDp2HdmiInterface.h"

class XbhDp2HdmiTask : public XbhMWThread
{
public:
    static XbhDp2HdmiTask* getInstance();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip(XBH_SOURCE_E src);

    /**
     * 设置当前信源的HPD状态
     * value 高低状态
     */
    XBH_S32 setRxHpd(XBH_SOURCE_E src, XBH_U8 value);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State);

    ~XbhDp2HdmiTask();

private:
    void run(const void* arg);
    XbhDp2HdmiTask();
    XbhDp2HdmiInterface *getCurrIC(XBH_SOURCE_E src);
    static XbhDp2HdmiTask *mInstance;
    static XbhMutex mLock;
    XbhDp2HdmiInterface *m_pXbhDp2HdmiInterface[8];
    XBH_SOURCE_E m_pXbhDpPortTable[8];

    XBH_U8 mList1[8];
    COMMON_OPT_TABLE mList2[8];
};

#endif //XBH_DP_2_HDMI_TASK_H
