#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhDp2HdmiTask_InputSource"

#include "XbhLog.h"
#include "XbhDp2HdmiTask.h"
#include "XbhService.h"
#include "XbhFrontBoard.h"

#include "ChipIte6563.h"
#include "Csv5363.h"

XbhDp2HdmiTask*              XbhDp2HdmiTask::mInstance = XBH_NULL;
XbhMutex                     XbhDp2HdmiTask::mLock;

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhDp2HdmiTask::resetChip(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->resetChip();
    }
    return s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 XbhDp2HdmiTask::setRxHpd(XBH_SOURCE_E src, XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setRxHpd(value);
    }
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhDp2HdmiTask::getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipInitDone(enable);
    }
    return s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhDp2HdmiTask::getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipExist(enable);
    }
    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhDp2HdmiTask::getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhDp2HdmiTask::upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhDp2HdmiTask::getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhDp2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getUpgradeState(s32State);
    }
    return s32Ret;
}

XbhDp2HdmiInterface *XbhDp2HdmiTask::getCurrIC(XBH_SOURCE_E src)
{
    for(int i = 0; i < 8; i ++)
    {
        if((XBH_SOURCE_E)m_pXbhDpPortTable[i] == src)
        {
            return m_pXbhDp2HdmiInterface[i];
        }
    }
    XLOGE("not found ic");
    return NULL;
}

void XbhDp2HdmiTask::run(const void* arg)
{
    for (int i = 0; i < 8; i++)
    {
        switch(mList1[i])
        {
            case ITE6563:
                m_pXbhDp2HdmiInterface[i] = new ChipIte6563(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case CSV5363:
                m_pXbhDp2HdmiInterface[i] = new Csv5363(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhDp2HdmiTask::XbhDp2HdmiTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getDp2HdmiTable(mList1);
    XbhService::getModuleInterface()->getDp2HdmiI2cTable(mList2);
    XbhService::getModuleInterface()->getDpPortTable(m_pXbhDpPortTable);

    XBH_U8 list1Tmp[2];
    COMMON_OPT_TABLE list2Tmp[2];
    XBH_SOURCE_E porttmp[2];
    XbhFrontBoard::getInstance()->getDp2HdmiTable(list1Tmp);
    XbhFrontBoard::getInstance()->getDp2HdmiI2cTable(list2Tmp);
    XbhFrontBoard::getInstance()->getDp2HdmiPortTable(porttmp);
    
    for (int i = 0; i < 8; i++)
    {
        if(mList1[i] == 0x00)
        {
            memcpy(&mList1[i], &list1Tmp, sizeof(list1Tmp));
            memcpy(&mList2[i], &list2Tmp, sizeof(list2Tmp));
            memcpy(&m_pXbhDpPortTable[i], &porttmp, sizeof(porttmp));
            break;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        XLOGV("=====>>dp_to_hdmi_list[%d] = 0x%02x ",i, mList1[i]);
        
        XLOGV("=====>>dp_to_hdmi_list[%d].iicBus1 = 0x%02x ",i, mList2[i].iicBus1);
        XLOGV("=====>>dp_to_hdmi_list[%d].iicBus2 = 0x%02x ",i, mList2[i].iicBus2);
        XLOGV("=====>>dp_to_hdmi_list[%d].iicAddr = 0x%02x ",i, mList2[i].iicAddr);
        XLOGV("=====>>dp_to_hdmi_list[%d].uart = 0x%02x ",i, mList2[i].uart);
        XLOGV("=====>>dp_to_hdmi_list[%d].pGpio = 0x%02x ",i, mList2[i].pGpio);
        XLOGV("=====>>dp_to_hdmi_list[%d].pLevel = 0x%02x ",i, mList2[i].pLevel);
        XLOGV("=====>>dp_to_hdmi_list[%d].rGpio = 0x%02x ",i, mList2[i].rGpio);
        XLOGV("=====>>dp_to_hdmi_list[%d].rLevel = 0x%02x ",i, mList2[i].rLevel);
        
        XLOGV("=====>>dp_to_hdmi_list[%d] = %d ",i, m_pXbhDpPortTable[i]);
    }
    
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhDp2HdmiTask::~XbhDp2HdmiTask()
{

}

XbhDp2HdmiTask* XbhDp2HdmiTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhDp2HdmiTask();
    }
    return mInstance;
}

