#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhNetSwitchTask"

#include "XbhLog.h"
#include "XbhNetSwitchTask.h"
#include "XbhService.h"

#include "ChipRtl8367.h"
#include "ChipRtl8305.h"

XbhNetSwitchTask*              XbhNetSwitchTask::mInstance = XBH_NULL;
XbhMutex                       XbhNetSwitchTask::mLock;


/**
 * 设置交换机的port的开关状态，通常是设置OPS，TYPEC端口的网络开关
 * 连接Android的端口通常不需要操作
*/
XBH_S32 XbhNetSwitchTask::setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->setPortEnable(source, enable);
    }
    return s32Ret;
}

/**
 * 获取交换机的port的开关状态，通常是获取OPS，TYPEC端口的网络开关
*/
XBH_S32 XbhNetSwitchTask::getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->getPortEnable(source, enable);
    }
    return s32Ret;
}

/**
 * 获取交换机的RJ45的连接状态，通常是输入输出口
*/
XBH_S32 XbhNetSwitchTask::getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->getRJ45PlugState(port, pbStatus);
    }
    return s32Ret;
}

/**
 * 设置交换机的输入输出的RJ45端口的指示灯工作状态
*/
XBH_S32 XbhNetSwitchTask::setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->setRJ45PortLedStatus(port, enable);
    }
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhNetSwitchTask::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhNetSwitchTask::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->getChipExist(enable);
    }
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhNetSwitchTask::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhNetSwitchTask::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhNetSwitchTask::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->getUpgradeState(s32State);
    }
    return s32Ret;
}

/**
 * 设置chip网络唤醒使能
 */
XBH_S32 XbhNetSwitchTask::setChipWol(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->setChipWol(bEnable);
    }
    return s32Ret;
}

/**
 * 设置wol端口电压
 */
XBH_S32 XbhNetSwitchTask::setWolPwer(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNetSwitchInterface[0] != NULL)
    {
        s32Ret = m_pXbhNetSwitchInterface[0]->setWolPwer(bEnable);
    }
    return s32Ret;
}

void XbhNetSwitchTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case RTL8367:
                m_pXbhNetSwitchInterface[i] = new ChipRtl8367(mList2[i].scl, mList2[i].sda, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel, mList2[i].androidPort, mList2[i].typecPort, mList2[i].opsPort, mList2[i].rj45inPort, mList2[i].rj45outPort);
                break;
             case RTL8305:
                m_pXbhNetSwitchInterface[i] = new ChipRtl8305(mList2[i].scl, mList2[i].sda, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel, mList2[i].androidPort, mList2[i].typecPort, mList2[i].opsPort, mList2[i].rj45inPort, mList2[i].rj45outPort);
            default:
                break;
        }
    }
}

XbhNetSwitchTask::XbhNetSwitchTask()
{
    XLOGD(" XbhNetSwitchTask  begin ");
    XbhService::getModuleInterface()->getNetSwitchTable(mList1);
    XbhService::getModuleInterface()->getNetSwitchOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" XbhNetSwitchTask  end ");
}

XbhNetSwitchTask::~XbhNetSwitchTask()
{

}

XbhNetSwitchTask* XbhNetSwitchTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhNetSwitchTask();
    }
    return mInstance;
}

