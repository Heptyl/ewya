#ifndef XBH_USBC_2_HDMI_TASK_H
#define XBH_USBC_2_HDMI_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhUsbc2HdmiInterface.h"

class XbhUsbc2HdmiTask : public XbhMWThread
{
public:
    static XbhUsbc2HdmiTask* getInstance();
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

    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable);

    /**
     * 限制type-c功率达到OPS过流保护效果
     */
    XBH_S32 setTypecCapacity(XBH_SOURCE_E src, XBH_U32 value);

    /**
    * 设置switch进入特殊模式
    */
    XBH_S32 setSpecificMode(XBH_SOURCE_E src);

    /**
    * 根据OPS插入状态，动态设置PD功率
    */
    XBH_S32 setChargingPower(XBH_SOURCE_E src, XBH_PDCAPACITY_POWER data);

    ~XbhUsbc2HdmiTask();

private:
    void run(const void* arg);
    XbhUsbc2HdmiTask();
    XbhUsbc2HdmiInterface *getCurrIC(XBH_SOURCE_E src);
    static XbhUsbc2HdmiTask *mInstance;
    static XbhMutex mLock;
    XbhUsbc2HdmiInterface *m_pXbhUsbc2HdmiInterface[8];
    XBH_SOURCE_E m_pXbhUsbcPortTable[8];

    XBH_U8 mList1[8];
    COMMON_OPT_TABLE mList2[8];
};

#endif //XBH_USBC_2_HDMI_TASK_H
