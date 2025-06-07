#ifndef __XBH_HDMI_SWITCH_INTERFACE_H__
#define __XBH_HDMI_SWITCH_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"
class XbhHdmiSwitchInterface 
{
public:

    /**
     * 给指定的芯片上下电
     * onoff：XBH_TRUE 上电，XBH_FALSE 下电
     */
    virtual XBH_S32 setPowerChip(XBH_BOOL onoff);

    /**
     * 复位当前转换芯片
     * hw: 采用硬复位还是软复位
     */
    virtual XBH_S32 resetChip(XBH_BOOL hw);

    /**
     * 设置当前信源的HPD状态
     * value 高低状态
     */
    virtual XBH_S32 setRxHpd(XBH_U8 port, XBH_BOOL high);

    /**
     * 获取固件版本
     */
    virtual XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    virtual XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     *获取固件升级状态
      0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
   */
    virtual XBH_S32 getUpgradeState(XBH_S32 *pState);

    /**
     * 烧录 HDCP KEY
     */
    virtual XBH_S32 setHdcpKey(const XBH_CHAR* strFilePath);

    /**
    * 读取Hdcp Key
    */
    virtual XBH_S32 getHdmiSwitchHdcpKey(XBH_U32 devType, XBH_CHAR* hdcpValue, XBH_HDCP_TYPE_E hdcpType);

    /**
    * 读取 HDCP KEY NAME
    */
    virtual XBH_S32 getHdcpKeyName(XBH_CHAR* strHdcpFileName);

    /**
    * 烧录 HDCP KEY NAME
    */
    virtual XBH_S32 setHdcpKeyName(const XBH_CHAR* strHdcpFileName);

    /**
     * 切换switch的port
     * port: 指定的port
     */
    virtual XBH_S32 setActivePort(XBH_U8 port);

    /**
     * 获取switch当前切换到的port
     * port: 指定的port
     */
    virtual XBH_S32 getActivePort(XBH_U8* port);

    /**
     * 获取switch上面连接的port的信号状态 
     * （部分场景下使用，比如有些dp转换过来的信号无法导致det pin为高电平就需要switch来判断）
     * port: 指定的port
     * status：信号状态
     */
    virtual XBH_S32 getRxPortStatus(XBH_U8 port, XBH_U8* status);

     /**
     * 获取switch上面输出的TxPort port的信号状态
     * （部分场景下使用，需要根据TxPort port   处理上一级得信号或者通知， 或者复位）
     * status：信号状态
     */
    virtual XBH_S32 getTxPortStatus(XBH_U8* status);

    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
    * 设置OPS Charge Power
    */
    virtual XBH_S32 setChargingPower(XBH_U8 data);


    /**
     * 设置SWITCH进入一些特定的模式，有些switch在跟SOC断开后就无法正确处理信号的插拔
     * 需要对这些switch进行特殊处理
     */
    virtual XBH_S32 setSpecificMode();

    /**
     * 设置SWITCH RX CEC 物理地址，不同版型认证要求不同，需要开机时重新设置
     */
    virtual XBH_S32 setPhysicalAddr(XBH_U8 port, XBH_U16 cecAddr);

    /**
     * 设置SWITCH RX EDID，
     */
    virtual XBH_S32 updateEdid(XBH_U8 edid_data[256],XBH_U8 port);

    /**
     * 设置type-c的VBUS复位
     */
    virtual XBH_S32 setTypecReset();

public:
    XbhHdmiSwitchInterface(){};
    virtual ~XbhHdmiSwitchInterface(){};
};

#endif