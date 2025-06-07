#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhUsbc2HdmiTask_InputSource"

#include "XbhLog.h"
#include "XbhUsbc2HdmiTask.h"
#include "XbhService.h"
#include "XbhFrontBoard.h"

#include "ChipLt8711.h"
#include "ChipLt8711UXE1.h"
#include "ChipGsv2202E.h"

XbhUsbc2HdmiTask*              XbhUsbc2HdmiTask::mInstance = XBH_NULL;
XbhMutex                       XbhUsbc2HdmiTask::mLock;

/**
 * 复位当前转换芯片
 */
XBH_S32 XbhUsbc2HdmiTask::resetChip(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
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
XBH_S32 XbhUsbc2HdmiTask::setRxHpd(XBH_SOURCE_E src, XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setRxHpd(value);
    }
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhUsbc2HdmiTask::getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 根据OPS插入状态，动态设置PD功率
 */
XBH_S32 XbhUsbc2HdmiTask::setChargingPower(XBH_SOURCE_E src, XBH_PDCAPACITY_POWER data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setChargingPower(data);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhUsbc2HdmiTask::upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
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
XBH_S32 XbhUsbc2HdmiTask::getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getUpgradeState(s32State);
    }
    return s32Ret;
}

/**
 * 获取芯片状态
 */
XBH_S32 XbhUsbc2HdmiTask::getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipInitDone(enable);
    }
    return s32Ret;
}

XBH_S32 XbhUsbc2HdmiTask::getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipExist(enable);
    }
    return s32Ret;
}

/**
 * 限制type-c功率达到OPS过流保护效果
 */
XBH_S32 XbhUsbc2HdmiTask::setTypecCapacity(XBH_SOURCE_E src, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setTypecCapacity(value);
    }
    return s32Ret;
}

XbhUsbc2HdmiInterface *XbhUsbc2HdmiTask::getCurrIC(XBH_SOURCE_E src)
{
    for(int i = 0; i < 8; i ++)
    {
        if((XBH_SOURCE_E)m_pXbhUsbcPortTable[i] == src)
        {
            return m_pXbhUsbc2HdmiInterface[i];
        }
    }
    XLOGE("not found ic");
    return NULL;
}

/**
* 设置switch进入特殊的模式
*/
XBH_S32 XbhUsbc2HdmiTask::setSpecificMode(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhUsbc2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setSpecificMode();
    }
    return s32Ret;
}

void XbhUsbc2HdmiTask::run(const void* arg)
{
    for (int i = 0; i < 8; i++)
    {
        switch(mList1[i])
        {
            case LT8711:
                m_pXbhUsbc2HdmiInterface[i] = new ChipLt8711(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case LT8711UXE1:
                m_pXbhUsbc2HdmiInterface[i] = new ChipLt8711UXE1(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case GSV2201E:
                m_pXbhUsbc2HdmiInterface[i] = new ChipGsv2202E(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case GSV2202E:
                m_pXbhUsbc2HdmiInterface[i] = new ChipGsv2202E(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhUsbc2HdmiTask::XbhUsbc2HdmiTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getUsbc2HdmiTable(mList1);
    XbhService::getModuleInterface()->getUsbc2HdmiI2cTable(mList2);
    XbhService::getModuleInterface()->getUsbcPortTable(m_pXbhUsbcPortTable);

    XBH_U8 list1Tmp[2];
    COMMON_OPT_TABLE list2Tmp[2];
    XBH_SOURCE_E porttmp[2];
    XbhFrontBoard::getInstance()->getUsbc2HdmiTable(list1Tmp);
    XbhFrontBoard::getInstance()->getUsbc2HdmiI2cTable(list2Tmp);
    XbhFrontBoard::getInstance()->getUsbc2HdmiPortTable(porttmp);
    
    for (int i = 0; i < 8; i++)
    {
        if(mList1[i] == 0x00)
        {
            memcpy(&mList1[i], &list1Tmp, sizeof(list1Tmp));
            memcpy(&mList2[i], &list2Tmp, sizeof(list2Tmp));
            memcpy(&m_pXbhUsbcPortTable[i], &porttmp, sizeof(porttmp));
            break;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        XLOGV("=====>>usbc_to_hdmi_list[%d] = 0x%02x ",i, mList1[i]);
        
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].iicBus1 = 0x%02x ",i, mList2[i].iicBus1);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].iicBus2 = 0x%02x ",i, mList2[i].iicBus2);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].iicAddr = 0x%02x ",i, mList2[i].iicAddr);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].uart = 0x%02x ",i, mList2[i].uart);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].pGpio = 0x%02x ",i, mList2[i].pGpio);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].pLevel = 0x%02x ",i, mList2[i].pLevel);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].rGpio = 0x%02x ",i, mList2[i].rGpio);
        XLOGV("=====>>usbc_to_hdmi_iic_table[%d].rLevel = 0x%02x ",i, mList2[i].rLevel);
        
        XLOGV("=====>>usbc_port_list[%d] = %d ",i, m_pXbhUsbcPortTable[i]);
    }
    
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhUsbc2HdmiTask::~XbhUsbc2HdmiTask()
{

}

XbhUsbc2HdmiTask* XbhUsbc2HdmiTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhUsbc2HdmiTask();
    }
    return mInstance;
}

