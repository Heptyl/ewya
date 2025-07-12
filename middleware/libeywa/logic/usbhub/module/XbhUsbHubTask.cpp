#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhUsbHubTask"

#include "XbhLog.h"
#include "XbhUsbHubTask.h"
#include "XbhPartitionData.h"
#include "XbhService.h"
#include "ChipHl817.h"
#include "ChipHl817.h"

XbhUsbHubTask*              XbhUsbHubTask::mInstance = XBH_NULL;
XbhMutex                    XbhUsbHubTask::mLock;

/**
 * 获取固件版本
 */
XBH_S32 XbhUsbHubTask::getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbHubInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhUsbHubTask::upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbHubInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        XLOGD(" tmp != NULL ");
        s32Ret = tmp->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhUsbHubTask::getUpgradeState(XBH_S32 devType, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbHubInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getUpgradeState(s32State);
    }
    return s32Ret;
}

XbhUsbHubInterface *XbhUsbHubTask::getCurrIC(XBH_S32 devType)
{
    XBH_U8 usbHub  = 0;
    switch (devType)
    {
        case XBH_UPDATE_BOARD_HL817:
            usbHub = BOARD_HL817;
        break;
        default:
            XLOGD("Out of range\n");
        break;
    }

    // 遍历每个设备
    for (int i = 0; i < 2; i++)
    {
        if (mList1[i] == usbHub)
        {
            return m_pXbhUsbHubInterface[i];
        }
    }
    XLOGE("not found ic");
    return NULL;
}

void XbhUsbHubTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case BOARD_HL817:
                m_pXbhUsbHubInterface[i] = new ChipHl817(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhUsbHubTask::XbhUsbHubTask()
{
    XLOGD(" XbhUsbHubTask begin ");
    XbhService::getModuleInterface()->getUsbHubTable(mList1);
    XbhService::getModuleInterface()->getUsbHubOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" XbhUsbHubTask end ");
}

XbhUsbHubTask::~XbhUsbHubTask()
{

}

XbhUsbHubTask* XbhUsbHubTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhUsbHubTask();
    }
    return mInstance;
}