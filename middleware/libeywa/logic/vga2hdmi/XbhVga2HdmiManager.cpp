#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhVga2HdmiManager_InputSource"
#include "XbhLog.h"

#include <stdio.h>
#include <unistd.h>
#include "XbhVga2HdmiManager.h"

XbhVga2HdmiManager*              XbhVga2HdmiManager::mInstance = XBH_NULL;
XbhMutex                         XbhVga2HdmiManager::mLock;

XBH_S32 XbhVga2HdmiManager::setCurrentRxHpd(XBH_U8 value)
{
    return XBH_FAILURE;
}

XBH_S32 XbhVga2HdmiManager::resetChip()
{
    return XBH_FAILURE;
}

/**
 * 获取VGA工作的状态
 */
XBH_S32 XbhVga2HdmiManager::getCurVgaStatus(XBH_SOURCE_E src, XBH_U8 *u8State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getCurVgaStatus(src, u8State);
    }
    return  s32Ret;
}

XBH_S32 XbhVga2HdmiManager::getChipExist(XBH_SOURCE_E src, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getChipExist(src, bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhVga2HdmiManager::getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getChipInitDone(src, bEnable);
    }
    return  s32Ret;
}

/**
 * VGA自动校准
 */
XBH_S32 XbhVga2HdmiManager::vgaAutoAdjust(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->vgaAutoAdjust(src);
    }
    return  s32Ret;
}

/**
 * VGA设置水平位置
 * u32Value 水平位置参数
 */
XBH_S32 XbhVga2HdmiManager::setVgaHPosition(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->setVgaHPosition(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA获取水平位置
 * u32Value 水平位置参数
 */
XBH_S32 XbhVga2HdmiManager::getVgaHPosition(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getVgaHPosition(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA设置垂直位置
 * u32Value 垂直位置参数
 */
XBH_S32 XbhVga2HdmiManager::setVgaVPosition(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->setVgaVPosition(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA获取垂直位置
 * u32Value 垂直位置参数
*/
XBH_S32 XbhVga2HdmiManager::getVgaVPosition(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getVgaVPosition(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA设置clock
 * u32Value clock参数
 */
XBH_S32 XbhVga2HdmiManager::setVgaClock(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->setVgaClock(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA获取clock
 * u32Value clock参数
*/
XBH_S32 XbhVga2HdmiManager::getVgaClock(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getVgaClock(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA设置phase
 * u32Value phase参数
 */
XBH_S32 XbhVga2HdmiManager::setVgaPhase(XBH_SOURCE_E src, XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->setVgaPhase(src, u32Value);
    }
    return  s32Ret;
}

/**
 * VGA获取phase
 * u32Value phase参数
*/
XBH_S32 XbhVga2HdmiManager::getVgaPhase(XBH_SOURCE_E src, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhVga2HdmiTask != NULL)
    {
        s32Ret = m_pXbhVga2HdmiTask->getVgaPhase(src, u32Value);
    }
    return  s32Ret;
}


XbhVga2HdmiManager::XbhVga2HdmiManager()
{
    XLOGD(" begin ");
    m_pXbhVga2HdmiTask = XBH_NULL;
    m_pXbhVga2HdmiTask = XbhVga2HdmiTask::getInstance();
    XLOGD(" end ");
}

XbhVga2HdmiManager::~XbhVga2HdmiManager()
{
}

XbhVga2HdmiManager* XbhVga2HdmiManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhVga2HdmiManager();
    }
    return mInstance;
}


