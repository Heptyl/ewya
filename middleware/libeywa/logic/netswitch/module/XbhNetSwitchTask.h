#ifndef XBH_NET_SWTICH_TASK_H
#define XBH_NET_SWTICH_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhNetSwitchInterface.h"

class XbhNetSwitchTask : public XbhMWThread
{
public:
    static XbhNetSwitchTask* getInstance();
    /**
     * 设置交换机的port的开关状态，通常是设置OPS，TYPEC端口的网络开关
     * 连接Android的端口通常不需要操作
    */
    XBH_S32 setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable);

    /**
     * 获取交换机的port的开关状态，通常是获取OPS，TYPEC端口的网络开关
    */
    XBH_S32 getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable);

    /**
     * 获取交换机的RJ45的连接状态，通常是输入输出口
    */
    XBH_S32 getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus);

    /**
     * 设置交换机的输入输出的RJ45端口的指示灯工作状态
    */
    XBH_S32 setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_S32 * s32State);

    /**
     * 设置chip网络唤醒使能
     */
    XBH_S32 setChipWol(XBH_BOOL bEnable);

    /**
     * 设置wol端口电压
     */
    XBH_S32 setWolPwer(XBH_BOOL bEnable);

    ~XbhNetSwitchTask();

private:
    void run(const void* arg);
    XbhNetSwitchTask();
    static XbhNetSwitchTask *mInstance;
    static XbhMutex mLock;
    XbhNetSwitchInterface *m_pXbhNetSwitchInterface[2];

    XBH_U8 mList1[2];
    NET_SWITCH_OPT_TABLE mList2[2];
};

#endif //XBH_NET_SWTICH_TASK_H
