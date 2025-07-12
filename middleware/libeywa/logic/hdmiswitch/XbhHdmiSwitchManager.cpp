#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhHdmiSwitchManager_InputSource"

#include "XbhLog.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhService.h"
#include "XbhSourceManager.h"

XbhHdmiSwitchManager*              XbhHdmiSwitchManager::mInstance = XBH_NULL;
XbhMutex                           XbhHdmiSwitchManager::mLock;


XBH_S32 XbhHdmiSwitchManager::setActivePort(XBH_SOURCE_E source)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    postMsg(0, &source, 1, 0, this);
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::setCurrentRxHpd(XBH_SOURCE_E source, XBH_BOOL high)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setCurrentRxHpd(source, high);
    }
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::getActivePortStatus(XBH_SOURCE_E source, XBH_U8* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->getActivePortStatus(source, value);
    }
    return s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhHdmiSwitchManager::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->getChipInitDone(enable);
    }
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhHdmiSwitchManager::getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->getChipExist(src, enable);
    }
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhHdmiSwitchManager::getFirmwareVersion(XBH_U32 devType, XBH_CHAR*  strVersion){
     XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhHdmiSwitchTask != NULL)
     {
         s32Ret = m_pXbhHdmiSwitchTask->getFirmwareVersion(devType, strVersion);
     }
     return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::upgradeFirmware( XBH_U32 devType, const  XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{

    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->upgradeFirmware(devType, strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::setChargingPower( XBH_U32 devType, XBH_U8 data)
{

    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setChargingPower(devType, data);
    }
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::setHdcpKey(XBH_U32 devType, const XBH_CHAR* strFilePath)
{
    XLOGD("set HDCP Key start !!!");
    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setHdcpKey(devType, strFilePath);
    }
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::getHdmiSwitchHdcpKey(XBH_U32 devType, XBH_CHAR* hdcpValue,XBH_HDCP_TYPE_E hdcpType)
{
    //XLOGD("getHdmiSwitchHdcpKey start !!!");
    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->getHdmiSwitchHdcpKey(devType, hdcpValue,hdcpType);
    }
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::getUpgradeState(XBH_U32 devType, XBH_S32 *pState)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(m_pXbhHdmiSwitchTask != NULL)
     {
         s32Ret = m_pXbhHdmiSwitchTask->getUpgradeState(devType, pState);
     }

    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::setHdcpKeyName(XBH_U32 devType, const XBH_CHAR* strHdcpFileName)
{
    XLOGD("set HDCP key Name start !!!");
    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setHdcpKeyName(devType, strHdcpFileName);
    }
    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::getHdcpKeyName(XBH_U32 devType, XBH_CHAR* strHdcpFileName)
{
    XLOGD("get HDCP key Name start !!!");
    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->getHdcpKeyName(devType, strHdcpFileName);
    }
    return s32Ret;
}
XBH_S32 XbhHdmiSwitchManager::setSpecificMode(XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setSpecificMode(devType);
    }

    return s32Ret;
}
XBH_S32 XbhHdmiSwitchManager::setPhysicalAddr(XBH_U32 devType, XBH_U8 port, XBH_U16 cecAddr)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setPhysicalAddr(devType, port, cecAddr);
    }

    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::updateEdid(XBH_U32 devType, XBH_U8 edid_data[256],XBH_U8 port)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->updateEdid(devType,edid_data,port);
    }

    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::setTypecReset(XBH_S32 devType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->setTypecReset(devType);
    }

    return s32Ret;
}

XBH_S32 XbhHdmiSwitchManager::getUSBCForwardReverseInsertionDet(XBH_S32 devType,XBH_S32 *u32Value)
{
    XLOGD("getUSBCForwardReverseInsertionDet start !!!");
    XBH_S32 s32Ret = XBH_SUCCESS;
   if(m_pXbhHdmiSwitchTask != NULL)
    {
        s32Ret = m_pXbhHdmiSwitchTask->getUSBCForwardReverseInsertionDet(devType, u32Value);
    }
    return s32Ret;
}

void XbhHdmiSwitchManager::run(const void* arg)
{
    XBH_U32 msgType;
    XBH_U32 size;
    XBH_SOURCE_E changeToSrc;
    XBH_BOOL lockStatus = XBH_FALSE;
    XBH_BOOL det = XBH_FALSE;
    if(getMsg(&msgType, &changeToSrc, &size, this))
    {
        if(m_pXbhHdmiSwitchTask != NULL)
        {
            //类似RK3576等芯片需要在RK626的5V DET为高的时候再去切switch, 否则可能在1k 信号和4K信号间切换时出不来信号
            XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_TRUE);
            m_pXbhHdmiSwitchTask->setActivePort(changeToSrc);
            //切换完后在设置一次RK628的HPD
            XbhService::getModuleInterface()->setDelayTimeForHdmiSwitch();
            XbhService::getModuleInterface()->getHdmiRxLockStatus(&lockStatus);
            XbhSourceManager::getInstance()->getSourceDet(changeToSrc, &det);
            //信号没有接入时，发送5V DET 断开的事件给RK628
            if(det == XBH_FALSE)
            {
                //XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_FALSE);
            }
            else
            {
                //当信号还没锁住并且是接入信号的状态，需要拉一下RK628的HPD
                if(lockStatus == XBH_FALSE)
                {
                    XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_TRUE);
                }
            }
        }
    }
}

XbhHdmiSwitchManager::XbhHdmiSwitchManager()
{
    XLOGD(" begin ");
    m_pXbhHdmiSwitchTask = XBH_NULL;
    m_pXbhHdmiSwitchTask = XbhHdmiSwitchTask::getInstance();
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhHdmiSwitchManager::~XbhHdmiSwitchManager()
{
    if(m_pXbhHdmiSwitchTask != XBH_NULL)
    {
        delete m_pXbhHdmiSwitchTask;
    }
    m_pXbhHdmiSwitchTask = XBH_NULL;
}

XbhHdmiSwitchManager* XbhHdmiSwitchManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhHdmiSwitchManager();
    }
    return mInstance;
}
