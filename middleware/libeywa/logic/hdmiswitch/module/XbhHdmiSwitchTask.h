#ifndef XBH_HDMI_SWITCH_TASK_H
#define XBH_HDMI_SWITCH_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhHdmiSwitchInterface.h"

class XbhHdmiSwitchTask : public XbhMWThread
{
public:
    static XbhHdmiSwitchTask* getInstance();
    /**
     * 根据source设置switch的port口
    */
    XBH_S32 setActivePort(XBH_SOURCE_E source);
    /**
     * 设置指定的source进行hpd
    */
    XBH_S32 setCurrentRxHpd(XBH_SOURCE_E source, XBH_BOOL high);
    /**
     * 获取switch上面连接的port的信号状态 
     * （部分场景下使用，比如有些dp转换过来的信号无法导致det pin为高电平就需要switch来判断）
     * source: 指定的source
     * status：连接状态
     */
    XBH_S32 getActivePortStatus(XBH_SOURCE_E source, XBH_U8* value);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable);
     /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_U32 devType, XBH_CHAR* strVersion);
    /**
     * 升级固件
     */

    XBH_S32 upgradeFirmware(XBH_U32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
     /**
      *获取固件升级状态ss
       0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
    */
    XBH_S32 getUpgradeState(XBH_U32 devType, XBH_S32 *pState);
    /**
     * 烧录HDCP Key
     */
    XBH_S32 setHdcpKey(XBH_U32 devType, const XBH_CHAR* strFilePath);
    /**
     * 读取HDCP Key
     */
    XBH_S32 getHdmiSwitchHdcpKey(XBH_U32 devType,XBH_CHAR* hdcpValue, XBH_HDCP_TYPE_E hdcpType);
    /**
     * 读取HDCP Key Name
     */
    XBH_S32 getHdcpKeyName(XBH_U32 devType, XBH_CHAR* strHdcpFileName);
    /**
     * 烧录HDCP Key Name
     */
    XBH_S32 setHdcpKeyName(XBH_U32 devType, const XBH_CHAR* strHdcpFileName);
    /**
    * 设置switch进入特殊模式
    */
    XBH_S32 setSpecificMode(XBH_U32 devType);
    /**
    * 设置OPS Charge Power
    */
    XBH_S32 setChargingPower(XBH_U32 devType, XBH_U8 data);
    /**
     * 设置switch portX 物理地址
     */
    XBH_S32 setPhysicalAddr(XBH_U32 devType, XBH_U8 port, XBH_U16 cecAddr);
    /**
     * 设置switch portX edid
     */
    XBH_S32 updateEdid(XBH_U32 devType,XBH_U8 edid_data[256],XBH_U8 port);
    /**
     * 设置type-c的VBUS复位
     */
    XBH_S32 setTypecReset(XBH_S32 devType);
    /**
     * get type-c Forward and Reverse Insertion Det
     */
     XBH_S32 getUSBCForwardReverseInsertionDet(XBH_S32 devType,XBH_S32 *u32Value);

    ~XbhHdmiSwitchTask();

private:
    void run(const void* arg);
    /**
     * 切换次级switch连接到前一级switch上
    */
    XBH_S32 setConnectToSecondSwitch(XbhHdmiSwitchInterface *interface, XBH_U8 row);
    XBH_S32 setConnectToSecondSwitch(XbhHdmiSwitchInterface *interface, XBH_S32 port);
    XbhHdmiSwitchTask();
    static XbhHdmiSwitchTask *mInstance;
    static XbhMutex mLock;
    XbhHdmiSwitchInterface *m_pXbhHdmiSwitchInterface[4][4];

    XBH_U8 mList1[4][4];
    HDMI_SW_OPT_TABLE mList2[4][4];
    HDMI_SWITCH_PORT m_pXbhHdmiSwitchPortTable[4][4];
    XBH_BOOL minitdone;
};

#endif //XBH_HDMI_SWITCH_TASK_H
