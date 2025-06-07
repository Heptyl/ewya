#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhMcuTask"

#include "XbhLog.h"
#include "XbhMcuTask.h"
#include "XbhService.h"

#include "RkGd32f310g8.h"
#include "mtGd32f310g8.h"
#include "Gd32f330r8.h"
#include "Gd32f305rb.h"
#include "At32f407rc.h"

XbhMcuTask*                     XbhMcuTask::mInstance = XBH_NULL;
XbhMutex                        XbhMcuTask::mLock;

/**
 * 复位MCU芯片
*/
XBH_S32 XbhMcuTask::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->resetChip();
    }
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhMcuTask::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhMcuTask::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getChipExist(enable);
    }
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhMcuTask::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhMcuTask::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhMcuTask::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getUpgradeState(s32State);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->setMcuFattMode(mode);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getMcuFattMode(mode);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->setMcuUartBypassPara(u8UartNum, baud);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->setMcuUartBypass(u8Len, u8Data);
    }
    return s32Ret;
}

XBH_S32 XbhMcuTask::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuInterface[0] != NULL)
    {
        s32Ret = m_pXbhMcuInterface[0]->getMcuUartBypass(u8Len, u8Data);
    }
    return s32Ret;
}

void XbhMcuTask::run(const void* arg)
{
    for (int i = 0; i < 1; i++)
    {
        switch(mList1[i])
        {
            case RK_GD32F310G8:
                m_pXbhMcuInterface[i] = new RkGd32f310g8(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case MT_GD32F310G8:
                m_pXbhMcuInterface[i] = new mtGd32f310g8(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case GD32F330R8:
                m_pXbhMcuInterface[i] = new Gd32f330r8(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case GD32F305RB:
                m_pXbhMcuInterface[i] = new Gd32f305rb(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case AT32F407RC:
                m_pXbhMcuInterface[i] = new At32f407rc(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhMcuTask::XbhMcuTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getMcuTable(mList1);
    XbhService::getModuleInterface()->getMcuOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhMcuTask::~XbhMcuTask()
{
}

XbhMcuTask* XbhMcuTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhMcuTask();
    }
    return mInstance;
}

