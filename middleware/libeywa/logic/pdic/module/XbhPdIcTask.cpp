#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPdIcTask"

#include "XbhLog.h"
#include "XbhPdIcTask.h"
#include "XbhService.h"
#include "XbhFrontBoard.h"

#include "ChipRts5450M.h"

XbhPdIcTask*              XbhPdIcTask::mInstance = XBH_NULL;
XbhMutex                       XbhPdIcTask::mLock;

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhPdIcTask::resetChip(XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhPdIcInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->resetChip();
    }
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhPdIcTask::getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhPdIcInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhPdIcTask::upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhPdIcInterface *tmp = getCurrIC(devType);
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
XBH_S32 XbhPdIcTask::getUpgradeState(XBH_S32 devType, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhPdIcInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getUpgradeState(s32State);
    }
    return s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhPdIcTask::getChipInitDone(XBH_S32 devType, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhPdIcInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipInitDone(enable);
    }
    return s32Ret;
}

XBH_S32 XbhPdIcTask::getChipExist(XBH_S32 devType, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhPdIcInterface *tmp = getCurrIC(devType);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipExist(enable);
    }
    return s32Ret;
}

/**
 * 动态功率
 */
XBH_S32 XbhPdIcTask::setTypecCapacity(XBH_U32 devType, XBH_U8 data)
{
    XLOGD(" setTypecCapacity devType =%d data = %d", devType, data);
    XBH_U8 hdmiSwitchicID  = 0;
    XBH_BOOL* enable;
    switch (devType)
    {
         case XBH_UPDATE_FRONT_RTS5450M:
             hdmiSwitchicID = FRONT_RTS5450M;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    XLOGD(" setTypecCapacity hdmiSwitchicID =%d", hdmiSwitchicID);
    //遍历
    for (int i = 0; i < 8; i++)
    {
        // 检查设备类型是否匹配，然后动态分配功率
         XLOGD(" setTypecCapacity mList1[%d] =%d",i,(mList1[i]));//  2

        if (mList1[i] == hdmiSwitchicID && m_pXbhPdIcInterfacee[i] != NULL )
        {
            m_pXbhPdIcInterfacee[i]->setTypecCapacity(data);
            XLOGD(" setTypecCapacity i =%d",i);
            return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}


XbhPdIcInterface *XbhPdIcTask::getCurrIC(XBH_S32 devType)
{
    XBH_U8 pdIcID  = 0;
    switch (devType)
    {
        case XBH_UPDATE_BOARD_RTS5450M:
            pdIcID = BOARD_RTS5450M;
        break;
        case XBH_UPDATE_FRONT_RTS5450M:
            pdIcID = FRONT_RTS5450M;
        break;
        default:
            XLOGD("Out of range\n");
        break;
    }

    // 遍历每个设备
    for (int i = 0; i < 8; i++)
    {
        if (mList1[i] == pdIcID)
        {
            return m_pXbhPdIcInterfacee[i];
        }
    }
    XLOGE("not found ic");
    return NULL;
}

void XbhPdIcTask::run(const void* arg)
{
    for (int i = 0; i < 8; i++)
    {
        switch(mList1[i])
        {
            case BOARD_RTS5450M:
                m_pXbhPdIcInterfacee[i] = new ChipRts5450M(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case FRONT_RTS5450M:
                m_pXbhPdIcInterfacee[i] = new ChipRts5450M(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }

}

XbhPdIcTask::XbhPdIcTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getPdIcTable(mList1);
    XbhService::getModuleInterface()->getPdIcI2cTable(mList2);

    XBH_U8 list1Tmp[2];
    COMMON_OPT_TABLE list2Tmp[2];
    XbhFrontBoard::getInstance()->getPdIcTable(list1Tmp);
    XbhFrontBoard::getInstance()->getPdIcI2cTable(list2Tmp);

    for (int i = 0; i < 8; i++)
    {
        if(mList1[i] == 0x00)
        {
            memcpy(&mList1[i], &list1Tmp, sizeof(list1Tmp));
            memcpy(&mList2[i], &list2Tmp, sizeof(list2Tmp));
            break;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        XLOGV("=====>>PD_IC_list[%d] = 0x%02x ",i, mList1[i]);

        XLOGV("=====>>PD_IC_iic_table[%d].iicBus1 = 0x%02x ",i, mList2[i].iicBus1);
        XLOGV("=====>>PD_IC_iic_table[%d].iicBus2 = 0x%02x ",i, mList2[i].iicBus2);
        XLOGV("=====>>PD_IC_iic_table[%d].iicAddr = 0x%02x ",i, mList2[i].iicAddr);
        XLOGV("=====>>PD_IC_iic_table[%d].uart = 0x%02x ",i, mList2[i].uart);
        XLOGV("=====>>PD_IC_iic_table[%d].pGpio = 0x%02x ",i, mList2[i].pGpio);
        XLOGV("=====>>PD_IC_iic_table[%d].pLevel = 0x%02x ",i, mList2[i].pLevel);
        XLOGV("=====>>PD_IC_iic_table[%d].rGpio = 0x%02x ",i, mList2[i].rGpio);
        XLOGV("=====>>PD_IC_iic_table[%d].rLevel = 0x%02x ",i, mList2[i].rLevel);
    }

    XbhMWThread::run();
    XLOGD(" end ");
}

XbhPdIcTask::~XbhPdIcTask()
{

}

XbhPdIcTask* XbhPdIcTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhPdIcTask();
    }
    return mInstance;
}

