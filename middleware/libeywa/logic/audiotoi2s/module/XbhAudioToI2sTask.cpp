#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioToI2sTask"

#include "XbhLog.h"
#include "XbhAudioToI2sTask.h"
#include "XbhService.h"

#include "ChipEs7243.h"
#include "ChipEs7210.h"

XbhAudioToI2sTask*              XbhAudioToI2sTask::mInstance = XBH_NULL;
XbhMutex                        XbhAudioToI2sTask::mLock;

/**
 * 复位audioToI2s芯片
*/
XBH_S32 XbhAudioToI2sTask::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioToI2sInterface[0]->resetChip();
    }
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhAudioToI2sTask::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioToI2sInterface[0]->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhAudioToI2sTask::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioToI2sInterface[0]->getChipExist(enable);
    }
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhAudioToI2sTask::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioToI2sInterface[0]->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhAudioToI2sTask::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioToI2sInterface[0]->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhAudioToI2sTask::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhAudioToI2sInterface[0] != NULL)
    {
        s32Ret = m_pXbhAudioToI2sInterface[0]->getUpgradeState(s32State);
    }
    return s32Ret;
}

void XbhAudioToI2sTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case ES7210:
                m_pXbhAudioToI2sInterface[i] = new ChipEs7210(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case ES7243:
                m_pXbhAudioToI2sInterface[i] = new ChipEs7243(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhAudioToI2sTask::XbhAudioToI2sTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getAudioToI2sTable(mList1);
    XbhService::getModuleInterface()->getAudioToI2sOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhAudioToI2sTask::~XbhAudioToI2sTask()
{
}

XbhAudioToI2sTask* XbhAudioToI2sTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioToI2sTask();
    }
    return mInstance;
}

