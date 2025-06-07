#ifndef XBH_HDMI_SWITCH_MANAGER_H
#define XBH_HDMI_SWITCH_MANAGER_H

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMsgQueue.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhHdmiSwitchTask.h"

class XbhHdmiSwitchManager : public XbhMWThread, public XbhMsgQueue
{
public:
    static XbhHdmiSwitchManager* getInstance();
    /**
     * 设置switch的port
    */
    XBH_S32 setActivePort(XBH_SOURCE_E source);
    /**
     * 通过switch设置当前通道的HPD状态
    */
    XBH_S32 setCurrentRxHpd(XBH_SOURCE_E source, XBH_BOOL high);
    /**
     * 通过switch判断端口的连接状态
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
    XBH_S32 getFirmwareVersion(XBH_U32 devType, XBH_CHAR *strVersion);
    /**s
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(XBH_U32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**s
     * 烧录 HDCP KEY
     */
    XBH_S32 setHdcpKey(XBH_U32 devType, const XBH_CHAR* strFilePath);
    /**s
     * 获取 HDCP KEY
     */
    XBH_S32 getHdmiSwitchHdcpKey(XBH_U32 devType,XBH_CHAR* hdcpValue,XBH_HDCP_TYPE_E hdcpType);
    /**
     *获取固件升级状态
      0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
    */
    XBH_S32 getUpgradeState(XBH_U32 devType, XBH_S32 *pState);
    /**s
     * 烧录 HDCP KEY Name
     */
    XBH_S32 setHdcpKeyName(XBH_U32 devType, const XBH_CHAR *strHdcpFileName);
    /**s
     * 读取 HDCP KEY Name
     */
    XBH_S32 getHdcpKeyName(XBH_U32 devType, XBH_CHAR *strHdcpFileName);
     /**
    * 设置switch进行特殊的处理
    */
    XBH_S32 setSpecificMode(XBH_S32 devType);
    /**
    * 设置OPS Charge Power
    */
    XBH_S32 setChargingPower( XBH_U32 devType, XBH_U8 data);
    /**
     * 设置switch portX 物理地址
     */
    XBH_S32 setPhysicalAddr(XBH_U32 devType, XBH_U8 port, XBH_U16 cecSPA);
    /**
     * 设置switch portX edid
     */
    XBH_S32 updateEdid(XBH_U32 devType, XBH_U8 edid_data[256],XBH_U8 port);
    /**
     * 设置type-c的VBUS复位
     */
    XBH_S32 setTypecReset(XBH_S32 devType);

    ~XbhHdmiSwitchManager();

private:
    void run(const void* arg);
    XbhHdmiSwitchManager();
    static XbhHdmiSwitchManager *mInstance;
    static XbhMutex mLock;
    XbhHdmiSwitchTask *m_pXbhHdmiSwitchTask;
};

#endif //XBH_HDMI_SWITCH_MANAGER_H
