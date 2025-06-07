#ifndef __XBH_NET_SWITCH_INTERFACE_H__
#define __XBH_NET_SWITCH_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhNetSwitchInterface 
{
public:
    /**
     * 设置交换机的port的开关状态，通常是设置OPS，TYPEC端口的网络开关
     * 连接Android的端口通常不需要操作
    */
    virtual XBH_S32 setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable);
    /**
     * 获取交换机的port的开关状态，通常是获取OPS，TYPEC端口的网络开关
    */
    virtual XBH_S32 getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable);
    /**
     * 获取交换机的RJ45的连接状态，通常是输入输出口
    */
    virtual XBH_S32 getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus);
    /**
     * 设置交换机的输入输出的RJ45端口的指示灯工作状态
    */
    virtual XBH_S32 setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable);
    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);
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
    
    /**
     * 设置chip网络唤醒使能
     */
    virtual XBH_S32 setChipWol(XBH_BOOL bEnable);

    /**
     * 设置wol端口电压
     */
    virtual XBH_S32 setWolPwer(XBH_BOOL bEnable);
public:
    XbhNetSwitchInterface(){};
    virtual ~XbhNetSwitchInterface(){};
};

#endif
