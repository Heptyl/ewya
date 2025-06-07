#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAVOutTask"

#include "XbhLog.h"
#include "XbhAVOutTask.h"
#include "XbhService.h"
#include "ChipMn869129.h"
#include "ChipIte6265.h"
#include "ChipLt8668SXC.h"
#include "ChipIte6563Out.h"
#include "ChipNt68411.h"
#include "ChipLt6711A.h"
#include "ChipCs5803aq.h"
#include "ChipGsv6125.h"

XbhAVOutTask*                XbhAVOutTask::mInstance = XBH_NULL;
XbhMutex                     XbhAVOutTask::mLock;

/**
 * 复位芯片
*/
XBH_S32 XbhAVOutTask::resetChip(XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->resetChip();
            return s32Ret;
        }
    }
    return s32Ret;
}

/**
 * 恢复芯片
*/
XBH_S32 XbhAVOutTask::resumeChip(XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->resumeChip();
            return s32Ret;
        }
    }
    return s32Ret;
}

/**
 * 设置HDMI(DP)TX的音频mute状态
*/
XBH_S32 XbhAVOutTask::setHdmiTxAudioMute(XBH_U32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->setHdmiTxAudioMute(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 获取HDMI(DP)TX的音频mute状态
*/
XBH_S32 XbhAVOutTask::getHdmiTxAudioMute(XBH_U32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getHdmiTxAudioMute(info);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 设置HDMI(DP)TX的视频mute状态
*/
XBH_S32 XbhAVOutTask::setHdmiTxVideoMute(XBH_U32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->setHdmiTxVideoMute(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 获取HDMI(DP)TX的视频mute状态
*/
XBH_S32 XbhAVOutTask::getHdmiTxVideoMute(XBH_U32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getHdmiTxVideoMute(info);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 设置HDMI(DP)TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 XbhAVOutTask::setHdmiTxTiming(XBH_U32 devType, XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->setHdmiTxTiming(value);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 获取HDMI(DP)TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 XbhAVOutTask::getHdmiTxTiming(XBH_U32 devType, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getHdmiTxTiming(info);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 设置HDMI(DP)TX的HDCP开关状态
 * 0：OFF，1：ON
*/
XBH_S32 XbhAVOutTask::setHdmiTxHdcp(XBH_U32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->setHdmiTxHdcp(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 开启/关闭 vbo tx 差分信号
*/
XBH_S32 XbhAVOutTask::setVboTxDiffSignals(XBH_U32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->setVboTxDiffSignals(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
 * 获取HDMI(DP)TX的HDCP开关状态
*/
XBH_S32 XbhAVOutTask::getHdmiTxHdcp(XBH_U32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getHdmiTxHdcp(info);
            return s32Ret;
        }
    }
    return s32Ret;
}
/**
* 设置Vx1 Tx的Tcon模式
*/
XBH_S32 XbhAVOutTask::setVx1TxTconMode(XBH_U32 devType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->setVx1TxTconMode(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}

XBH_S32 XbhAVOutTask::getChipExist(XBH_U32 devType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getChipExist(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}

XBH_S32 XbhAVOutTask::getChipInitDone(XBH_U32 devType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getChipInitDone(bEnable);
            return s32Ret;
        }
    }
    return s32Ret;
}

/**
 * 获取HDMI(DP)TX的设备接入情况
*/
XBH_S32 XbhAVOutTask::getHdmiTxPlugState(XBH_U32 devType, XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == devType)
        {
            s32Ret  = m_pXbhAVOutInterface[i]->getHdmiTxPlugState(info);
            return s32Ret;
        }
    }
    return s32Ret;
}

/**
 * 获取固件版本
*/
XBH_S32 XbhAVOutTask::getFirmwareVersion(XBH_U32 devType, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhAVOutInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getFirmwareVersion(strVersion);
    }
    XLOGD("getFirmwareVersion strVersion = %s ", strVersion);
    return s32Ret;
}

/**
 * 进行固件升级
*/
XBH_S32 XbhAVOutTask::upgradeFirmware(XBH_U32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhAVOutInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 *获取固件升级状态
  0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
*/
XBH_S32 XbhAVOutTask::getUpgradeState(XBH_U32 devType, XBH_S32 *pState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhAVOutInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getUpgradeState(pState);
    }
    XLOGD(" getUpgradeState  pState = %d", *pState);
    return s32Ret;
}

void XbhAVOutTask::run(const void* arg)
{
    for (int i = 0; i < 4; i++)
    {
        switch(mList1[i])
        {
            case MN869129:
                m_pXbhAVOutInterface[i] = new ChipMn869129(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case ITE6265:
                m_pXbhAVOutInterface[i] = new ChipIte6265(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case LT8668SXC:
                m_pXbhAVOutInterface[i] = new ChipLt8668SXC(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case ITE6563OUT:
                m_pXbhAVOutInterface[i] = new ChipIte6563Out(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case NT68411:
                m_pXbhAVOutInterface[i] = new ChipNt68411(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case LT6711A:
                m_pXbhAVOutInterface[i] = new ChipLt6711A(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case CS5803AQ:
                m_pXbhAVOutInterface[i] = new ChipCS5803AQ(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case GSV6125:
                m_pXbhAVOutInterface[i] = new ChipGsv6125(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}


XbhAVOutInterface *XbhAVOutTask::getCurrIC(XBH_S32 devType)
{
    XBH_U8 avoutIcID  = 0;
    switch (devType)
    {
        case XBH_UPDATE_BOARD_LT8668SXC_1:
            avoutIcID = LT8668SXC;
        break;
        case XBH_UPDATE_ITE6563OUT:
            avoutIcID = ITE6563OUT;
        break;
        case XBH_UPDATE_ITE68411:
            avoutIcID = NT68411;
        break;
        case XBH_UPDATE_LT6711A:
            avoutIcID = LT6711A;
        break;
        case XBH_UPDATE_CS5803AQ:
            avoutIcID = CS5803AQ;
        break;
        case XBH_UPDATE_BOARD_GSV6125:
            avoutIcID = GSV6125;
        break;
        default:
            XLOGD("Out of range\n");
        break;
    }
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        if (mList1[i] == avoutIcID)
        {
            return m_pXbhAVOutInterface[i];
        }
    }
    XLOGE("not found ic");
    return NULL;
}

XbhAVOutTask::XbhAVOutTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getVideoOutTable(mList1);
    XbhService::getModuleInterface()->getVideoOutOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhAVOutTask::~XbhAVOutTask()
{
}

XbhAVOutTask* XbhAVOutTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAVOutTask();
    }
    return mInstance;
}

