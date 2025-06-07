#ifndef XBH_USBC_2_HDMI_MANAGER_H
#define XBH_USBC_2_HDMI_MANAGER_H

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMutex.h"
#include "XbhUsbc2HdmiTask.h"

class XbhUsbc2HdmiManager
{
public:
    static XbhUsbc2HdmiManager* getInstance();
    XBH_S32 setRxHpd(XBH_SOURCE_E src, XBH_U8 value);
    XBH_S32 resetChip(XBH_SOURCE_E src);
    XBH_S32 getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State);
    XBH_S32 setTypecCapacity(XBH_SOURCE_E src, XBH_U32 value);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable);
    /**
    * 设置switch进行特殊的处理
    */
    XBH_S32 setSpecificMode(XBH_SOURCE_E src);
    /**
    * 根据OPS插入状态，动态设置PD功率
    */
    XBH_S32 setChargingPower(XBH_SOURCE_E src, XBH_PDCAPACITY_POWER data);



    ~XbhUsbc2HdmiManager();

private:
    XbhUsbc2HdmiManager();
    static XbhUsbc2HdmiManager *mInstance;
    static XbhMutex mLock;

    XbhUsbc2HdmiTask *m_pXbhUsbc2HdmiTask;
};

#endif //XBH_USBC_2_HDMI_MANAGER_H
