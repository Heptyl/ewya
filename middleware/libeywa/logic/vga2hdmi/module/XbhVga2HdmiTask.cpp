#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhVga2HdmiTask_InputSource"

#include "XbhLog.h"
#include "XbhVga2HdmiTask.h"
#include "XbhService.h"

#include "ChipMs9282.h"

XbhVga2HdmiTask*              XbhVga2HdmiTask::mInstance = XBH_NULL;
XbhMutex                      XbhVga2HdmiTask::mLock;

/**
 * 复位codec芯片
*/
XBH_S32 XbhVga2HdmiTask::resetChip(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->resetChip();
    }
    return s32Ret;
}

/**
 * VGA自动校准
 */
XBH_S32 XbhVga2HdmiTask::vgaAutoAdjust(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->vgaAutoAdjust();
    }
    return s32Ret;
}

/**
 * VGA设置水平位置
 * u32Value 水平位置参数
 */
XBH_S32 XbhVga2HdmiTask::setVgaHPosition(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setVgaHPosition(u32Value);
    }
    return s32Ret;
}

/**
 * VGA获取水平位置
 * u32Value 水平位置参数
 */
XBH_S32 XbhVga2HdmiTask::getVgaHPosition(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getVgaHPosition(u32Value);
    }
    return s32Ret;
}

/**
 * VGA设置垂直位置
 * u32Value 垂直位置参数
 */
XBH_S32 XbhVga2HdmiTask::setVgaVPosition(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setVgaVPosition(u32Value);
    }
    return s32Ret;
}

/**
 * VGA获取垂直位置
 * u32Value 垂直位置参数
*/
XBH_S32 XbhVga2HdmiTask::getVgaVPosition(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getVgaVPosition(u32Value);
    }
    return s32Ret;
}

/**
 * VGA设置clock
 * u32Value clock参数
 */
XBH_S32 XbhVga2HdmiTask::setVgaClock(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setVgaClock(u32Value);
    }
    return s32Ret;
}

/**
 * VGA获取clock
 * u32Value clock参数
*/
XBH_S32 XbhVga2HdmiTask::getVgaClock(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getVgaClock(u32Value);
    }
    return s32Ret;
}

/**
 * VGA设置phase
 * u32Value phase参数
 */
XBH_S32 XbhVga2HdmiTask::setVgaPhase(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->setVgaPhase(u32Value);
    }
    return s32Ret;
}

/**
 * VGA获取phase
 * u32Value phase参数
*/
XBH_S32 XbhVga2HdmiTask::getVgaPhase(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getVgaPhase(u32Value);
    }
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhVga2HdmiTask::getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhVga2HdmiTask::upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhVga2HdmiTask::getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getUpgradeState(s32State);
    }
    return s32Ret;
}
/**
 * 获取VGA工作的状态
*/
XBH_S32 XbhVga2HdmiTask::getCurVgaStatus(XBH_SOURCE_E src, XBH_U8 *u8State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getCurVgaStatus(u8State);
    }
    return s32Ret;
}

XBH_S32 XbhVga2HdmiTask::getChipExist(XBH_SOURCE_E src, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipExist(bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhVga2HdmiTask::getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhVga2HdmiInterface *tmp = getCurrIC(src);
    if(tmp != NULL)
    {
        s32Ret = tmp->getChipInitDone(enable);
    }
    return s32Ret;
}

XbhVga2HdmiInterface *XbhVga2HdmiTask::getCurrIC(XBH_SOURCE_E src)
{
    for(int i = 0; i < 8; i ++)
    {
        if((XBH_SOURCE_E)m_pXbhVgaPortTable[i] == src)
        {
            return m_pXbhVga2HdmiInterface[i];
        }
    }
    XLOGE("not found ic");
    return NULL;
}

void XbhVga2HdmiTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case MS9282:
                m_pXbhVga2HdmiInterface[i] = new ChipMs9282(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }

}

XbhVga2HdmiTask::XbhVga2HdmiTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getVga2HdmiTable(mList1);
    XbhService::getModuleInterface()->getVga2HdmiI2cTable(mList2);
    XbhService::getModuleInterface()->getVgaPortTable(m_pXbhVgaPortTable);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhVga2HdmiTask::~XbhVga2HdmiTask()
{
}

XbhVga2HdmiTask* XbhVga2HdmiTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhVga2HdmiTask();
    }
    return mInstance;
}

