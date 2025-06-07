#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhMcuManager"

#include "XbhLog.h"
#include "XbhMcuManager.h"
#include "XbhService.h"

XbhMcuManager*             XbhMcuManager::mInstance = XBH_NULL;
XbhMutex                           XbhMcuManager::mLock;

/**
 * 复位MCU芯片
*/
XBH_S32 XbhMcuManager::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->resetChip();
    }
    return  s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhMcuManager::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhMcuManager::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getChipExist(enable);
    }
    return  s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 XbhMcuManager::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getFirmwareVersion(strVersion);
    }
    return  s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhMcuManager::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    //升级MCU时锁定MCU升级锁，由于MCU升级后就会关机，此锁开机后会重置为false。故不需要在其他地方赋值false
    XbhService::getModuleInterface()->mMcuUpdateLock = true;

    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return  s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhMcuManager::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getUpgradeState(s32State);
    }
    return  s32Ret;
}

/**
 * 设置fatt模式
 */
XBH_S32 XbhMcuManager::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->setMcuFattMode(mode);
    }
    return  s32Ret;
}

/**
 * 获取fatt模式
 */
XBH_S32 XbhMcuManager::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getMcuFattMode(mode);
    }
    return  s32Ret;
}

/**
 * mcu 外置iic数据转发
 */
XBH_S32 XbhMcuManager::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    }
    return  s32Ret;
}

/**
 * mcu 外置iic数据获取
 */
XBH_S32 XbhMcuManager::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    }
    return  s32Ret;
}
/**
* mcu 外置uart配置
*/
XBH_S32 XbhMcuManager::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->setMcuUartBypassPara(u8UartNum, baud);
    }
    return s32Ret;
}
/**
* mcu 外置uart数据转发
*/
XBH_S32 XbhMcuManager::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->setMcuUartBypass(u8Len, u8Data);
    }
    return s32Ret;
}
/**
* mcu 外置uart数据接收
*/
XBH_S32 XbhMcuManager::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhMcuTask != NULL)
    {
        s32Ret = m_pXbhMcuTask->getMcuUartBypass(u8Len, u8Data);
    }
    return s32Ret;
}

XbhMcuManager::XbhMcuManager()
{
    XLOGD(" begin ");
    m_pXbhMcuTask = XBH_NULL;
    m_pXbhMcuTask = XbhMcuTask::getInstance();
    XLOGD(" end ");
}

XbhMcuManager::~XbhMcuManager()
{
    if(m_pXbhMcuTask != XBH_NULL)
    {
        delete m_pXbhMcuTask;
    }
    m_pXbhMcuTask = XBH_NULL;
}

XbhMcuManager* XbhMcuManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhMcuManager();
    }
    return mInstance;
}

