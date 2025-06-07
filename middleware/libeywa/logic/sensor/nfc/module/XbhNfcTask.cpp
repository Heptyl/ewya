#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhNfcTask"

#include "XbhLog.h"
#include "XbhNfcTask.h"
#include "XbhService.h"
#include "ChipPn7160Task.h"
#include "ChipMcuNfc.h"

XbhNfcTask*                         XbhNfcTask::mInstance = XBH_NULL;
XbhMutex                            XbhNfcTask::mLock;

/**
 * 复位芯片
*/
XBH_S32 XbhNfcTask::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->resetChip();
    }
    return s32Ret;
}

/**
 * 检测Nfc是否存在
*/
XBH_S32 XbhNfcTask::getChipExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->getChipExist(enable);
    }
    return s32Ret;
}

XBH_S32 XbhNfcTask::getChipInitDone(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->getChipInitDone(enable);
    }
    return s32Ret;
}
/**
 * 使能nfc
*/
XBH_S32 XbhNfcTask::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->setNfcEnable(enable);
    }
    return s32Ret;
}

/**
 * 得到NFC状态
*/
XBH_S32 XbhNfcTask::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->getNfcEnable(enable);
    }
    return s32Ret;
}

/**
 * 获取Nfc 卡ID
*/
XBH_S32 XbhNfcTask::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->getNfcId(data,length);
    }
    return s32Ret;
}

/**
 * 获取NFC唤醒的卡ID
*/
XBH_S32 XbhNfcTask::getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->getWakeUpNfcId(data,length);
    }
    return s32Ret;
}

/**
 * 添加NFC 卡
*/
XBH_S32 XbhNfcTask::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->addNfcId(data,length);
    }
    return s32Ret;
}

/**
 * 删除NFC 卡
*/
XBH_S32 XbhNfcTask::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->delNfcId(data,length);
    }
    return s32Ret;
}

/**
 * 删除所有NFC卡
*/
XBH_S32 XbhNfcTask::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->delAllNfcId();
    }
    return s32Ret;
}

/**
 * 选择NFC模块
*/
XBH_S32 XbhNfcTask::switchNfcType()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->switchNfcType();
    }
    return s32Ret;
}

/**
 * 设置NFC低功耗模式
*/
XBH_S32 XbhNfcTask::setNfcLowPower(const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->setNfcLowPower(u8Data);
    }
    return s32Ret;
}
    /**
     * 获取NFC低功耗模式
    */
XBH_S32 XbhNfcTask::getNfcLowPower(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcInterface[0] != NULL)
    {
        s32Ret = m_pXbhNfcInterface[0]->getNfcLowPower(enable);
    }
    return s32Ret;
}

void XbhNfcTask::run(const void* arg)
{
    for (int i = 0; i < 2; i++)
    {
        switch(mList1[i])
        {
            case PN7160:
                m_pXbhNfcInterface[i] = new ChipPn7160Task(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            case MCUNFC:
                m_pXbhNfcInterface[i] = new ChipMcuNfc(mList2[i].iicBus1, mList2[i].iicAddr, mList2[i].pGpio, mList2[i].pLevel, mList2[i].rGpio, mList2[i].rLevel);
                break;
            default:
                break;
        }
    }
}

XbhNfcTask::XbhNfcTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getNfcTable(mList1);
    XbhService::getModuleInterface()->getNfcOptTable(mList2);
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhNfcTask::~XbhNfcTask()
{
}

XbhNfcTask* XbhNfcTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhNfcTask();
    }
    return mInstance;
}

