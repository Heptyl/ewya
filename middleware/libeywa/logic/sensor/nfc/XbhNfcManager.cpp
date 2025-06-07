#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhNfcManager"

#include "XbhLog.h"
#include "XbhNfcManager.h"
#include "XbhService.h"

XbhNfcManager*                          XbhNfcManager::mInstance = XBH_NULL;
XbhMutex                                XbhNfcManager::mLock;


XBH_S32 XbhNfcManager::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->resetChip();
    }
    return  s32Ret;
}

/**
 * 获取chip是否存在
 */
XBH_S32 XbhNfcManager::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->getChipExist(bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->getChipInitDone(bEnable);
    }
    return  s32Ret;
}


XBH_S32 XbhNfcManager::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->setNfcEnable(enable);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->getNfcEnable(enable);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
    {
        XLOGE("MCU is upgrading, getNfcId has been disabled\n");
        return XBH_FAILURE;
    }

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->getNfcId(data,length);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length)
{

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->getWakeUpNfcId(data,length);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
    {
        XLOGE("MCU is upgrading, addNfcId has been disabled\n");
        return XBH_FAILURE;
    }

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->addNfcId(data,length);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
    {
        XLOGE("MCU is upgrading, delNfcId has been disabled\n");
        return XBH_FAILURE;
    }

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->delNfcId(data,length);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->delAllNfcId();
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::switchNfcType()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->switchNfcType();
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::setNfcLowPower(const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->setNfcLowPower(u8Data);
    }
    return  s32Ret;
}

XBH_S32 XbhNfcManager::getNfcLowPower(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhNfcTask != NULL)
    {
        s32Ret = m_pXbhNfcTask->getNfcLowPower(enable);
    }
    return  s32Ret;
}

XbhNfcManager::XbhNfcManager()
{
    XLOGD(" begin ");
    m_pXbhNfcTask = XBH_NULL;
    m_pXbhNfcTask = XbhNfcTask::getInstance();
    XLOGD(" end ");
}

XbhNfcManager::~XbhNfcManager()
{
    if(m_pXbhNfcTask != XBH_NULL)
    {
        delete m_pXbhNfcTask;
    }
    m_pXbhNfcTask = XBH_NULL;
}

XbhNfcManager* XbhNfcManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhNfcManager();
    }
    return mInstance;
}

