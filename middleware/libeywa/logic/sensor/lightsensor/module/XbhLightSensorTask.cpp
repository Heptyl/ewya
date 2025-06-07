#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLightSensorTask"

#include "XbhLog.h"
#include "XbhLightSensorTask.h"
#include "XbhService.h"

#include "ChipJSA1141.h"
#include "ChipV2000.h"
#include "ChipRisa311.h"
#include "ChipTsl2572.h"
#include "ChipTsl2561.h"
#include "ChipLtrx140a.h"
#include "ChipALS199.h"
#include "ChipStk3210.h"

XbhLightSensorTask*             XbhLightSensorTask::mInstance = XBH_NULL;
XbhMutex                        XbhLightSensorTask::mLock;

/**
 * 复位light sensor芯片
*/
XBH_S32 XbhLightSensorTask::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->resetChip();
    }
    return s32Ret;
}

/**
 * 获取lightsensor的值
 */
XBH_S32 XbhLightSensorTask::getLightSensorValue(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->getLightSensorValue(s32State);
    }
    return  s32Ret;
}

/**
 * 获取lightsensor是否存在
 */
XBH_S32 XbhLightSensorTask::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->getChipExist(bEnable);
    }
    return  s32Ret;
}

XBH_S32 XbhLightSensorTask::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->getChipInitDone(bEnable);
    }
    return  s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 XbhLightSensorTask::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->getFirmwareVersion(strVersion);
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 XbhLightSensorTask::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->upgradeFirmware(strFilePath, bForceUpgrade);
    }
    return s32Ret;
}

/**
 * 获取固件升级时的状态
 */
XBH_S32 XbhLightSensorTask::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_pXbhLightSensorInterface[0] != NULL)
    {
        s32Ret = m_pXbhLightSensorInterface[0]->getUpgradeState(s32State);
    }
    return s32Ret;
}

/**
 * 执行光感模块初始化
 */
XBH_S32 XbhLightSensorTask::executeLightSensorInit()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    init();
    if(m_pXbhLightSensorInterface[0] != XBH_NULL)
    {
        s32Ret = XBH_SUCCESS;
    }
    return s32Ret;
}

//note:JSA1141 和 SDFDALSZ1 是同一个光感
void XbhLightSensorTask::init()
{
    XBH_BOOL bExist = XBH_FALSE;
    for (int i = 0; i < LIGHTSENSOR_MAX; i++)
    {
        switch(i)
        {
            case JSA1141:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipJSA1141(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipJSA1141(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            case V2000:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipV2000(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipV2000(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            case RISA311:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipRisa311(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipRisa311(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            case TSL25721:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipTsl2572(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipTsl2572(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            case TSL2561:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipTsl2561(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipTsl2561(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            case LTRX140A:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipLtrx140a(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipLtrx140a(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
			case ALS199:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipALS199(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipALS199(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            case STK3210:
                //使用第一组IIC
                m_pXbhLightSensorInterface[0] = new ChipStk3210(mList1[0].iicBus1, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                if(bExist == XBH_FALSE)
                {
                    delete m_pXbhLightSensorInterface[0];
                    m_pXbhLightSensorInterface[0] = XBH_NULL;
                    //使用第二组IIC
                    m_pXbhLightSensorInterface[0] = new ChipStk3210(mList1[0].iicBus2, mList1[0].pGpio, mList1[0].pLevel, mList1[0].rGpio, mList1[0].rLevel);
                    m_pXbhLightSensorInterface[0]->getChipExist(&bExist);
                    if(bExist == XBH_FALSE)
                    {
                        delete m_pXbhLightSensorInterface[0];
                        m_pXbhLightSensorInterface[0] = XBH_NULL;
                    }
                    else
                    {
                        return;
                    }
                }
                else
                {
                    return;
                }
                break;
            default:
                break;
        }
    }
}

XbhLightSensorTask::XbhLightSensorTask()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->getLightSensorOptTable(mList1);
    init();
    //XbhMWThread::run();
    XLOGD(" end ");
}

XbhLightSensorTask::~XbhLightSensorTask()
{
    if(m_pXbhLightSensorInterface[0] != XBH_NULL)
    {
        delete m_pXbhLightSensorInterface[0];
        m_pXbhLightSensorInterface[0] = XBH_NULL;
    }
}

XbhLightSensorTask* XbhLightSensorTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhLightSensorTask();
    }
    return mInstance;
}

