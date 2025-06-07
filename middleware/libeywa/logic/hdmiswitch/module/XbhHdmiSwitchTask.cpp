#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhHdmiSwitchTask_InputSource"

#include "XbhLog.h"
#include "XbhHdmiSwitchTask.h"
#include "XbhService.h"
#include "XbhFrontBoard.h"
#include <string.h>
#include <cutils/properties.h>
#include "ChipIt66353.h"
#include "ChipIt66353_2.h"
#include "ChipAsw3642.h"
#include "ChipGsv2715.h"
#include "ChipGsv2705.h"
#include "ChipGsv6712.h"
#include "ChipLt8668SX.h"
#include "ChipGsv2712.h"
#include "ChipGsv2702.h"

XbhHdmiSwitchTask*              XbhHdmiSwitchTask::mInstance = XBH_NULL;
XbhMutex                        XbhHdmiSwitchTask::mLock;

XBH_S32 XbhHdmiSwitchTask::setConnectToSecondSwitch(XbhHdmiSwitchInterface *interface, XBH_U8 row)
{
    for (int j = 0; j < 4; j++)
    {
        if(0x01 == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[row][j].port0)
        {
            if(interface != XBH_NULL)
            {
                interface->setActivePort(0);
            }
        }
        else if(0x01 == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[row][j].port1)
        {
            if(interface != XBH_NULL)
            {
                interface->setActivePort(1);
            }
        }
        else if(0x01 == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[row][j].port2)
        {
            if(interface != XBH_NULL)
            {
                interface->setActivePort(2);
            }
        }
        else if(0x01 == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[row][j].port3)
        {
            if(interface != XBH_NULL)
            {
                interface->setActivePort(3);
            }
        }
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhHdmiSwitchTask::setConnectToSecondSwitch(XbhHdmiSwitchInterface *interface, XBH_S32 port)
{
    if(interface != XBH_NULL)
    {
        interface->setActivePort(port);
    }
    return XBH_SUCCESS;
}


XBH_S32 XbhHdmiSwitchTask::setActivePort(XBH_SOURCE_E source)
{
    XBH_BOOL findPort = XBH_FALSE;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port0)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->setActivePort(0);
                    findPort = XBH_TRUE;
                }
            }
            else if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port1)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->setActivePort(1);
                    findPort = XBH_TRUE;
                }
            }
            else if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port2)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->setActivePort(2);
                    findPort = XBH_TRUE;
                }
            }
            else if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port3)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->setActivePort(3);
                    findPort = XBH_TRUE;
                }
            }
            //次级switch依次切换到对应的port上
            if(findPort)
            {
                while(i > 0)
                {
                    //当指定了前一级的switch 和 port时的处理
                    if(mList2[i][j].prePort > 0)
                    {
                        XBH_S32 switchId = mList2[i][j].prePort >> 8 - 1;
                        XBH_S32 port = (mList2[i][j].prePort & 0x0f) - 1;
                        i--;
                        usleep(300 * 1000);
                        setConnectToSecondSwitch(m_pXbhHdmiSwitchInterface[i][switchId], port);
                    }
                    else
                    {
                        i--;
                        usleep(300 * 1000);
                        setConnectToSecondSwitch(m_pXbhHdmiSwitchInterface[i][j], (XBH_U8)i);
                    }
                }
            }
            if(findPort)
            {
                return XBH_SUCCESS;
            }
        }
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhHdmiSwitchTask::setCurrentRxHpd(XBH_SOURCE_E source, XBH_BOOL high)
{
    return XBH_FAILURE;
}

XBH_S32 XbhHdmiSwitchTask::getActivePortStatus(XBH_SOURCE_E source, XBH_U8* value)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port0)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->getRxPortStatus(0, value);
                }
            }
            else if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port1)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->getRxPortStatus(1, value);
                }
            }
            else if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port2)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->getRxPortStatus(2, value);
                }
            }
            else if(source == (XBH_SOURCE_E)m_pXbhHdmiSwitchPortTable[i][j].port3)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->getRxPortStatus(3, value);
                }
            }
        }
    }
    return XBH_SUCCESS;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhHdmiSwitchTask::getChipInitDone(XBH_BOOL* enable)
{
    *enable = minitdone;
    return XBH_SUCCESS;
}

/**
 * 获取chip是否存在
*/
XBH_S32 XbhHdmiSwitchTask::getChipExist(XBH_SOURCE_E src, XBH_BOOL* enable)
{
    return XBH_FAILURE;
}

/**
* 设置switch进入特殊的模式
*/
XBH_S32 XbhHdmiSwitchTask::setSpecificMode(XBH_U32 devType)
{
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->setSpecificMode();
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    XLOGD("setSpecificMode find device fail");
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 设置switch portX 物理地址
 */
XBH_S32 XbhHdmiSwitchTask::setPhysicalAddr(XBH_U32 devType, XBH_U8 port, XBH_U16 cecAddr)
{
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
        case XBH_UPDATE_BOARD_GSV2705_1:
            hdmiSwitchicID = GSV2705_1;
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            hdmiSwitchicID = GSV2715_1;
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            hdmiSwitchicID = GSV2712_1;
            break;
        case XBH_UPDATE_FRONT_GSV2702:
            hdmiSwitchicID = GSV2702_1;
            break;
        default:
            XLOGD("Out of range\n");
            break;
    }
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配
            if (mList1[i][j] == hdmiSwitchicID)
            {
                if(m_pXbhHdmiSwitchInterface[i][j] != XBH_NULL)
                {
                    m_pXbhHdmiSwitchInterface[i][j]->setPhysicalAddr(port, cecAddr);
                    return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
                }
                else
                {
                    continue;  // 开机过程中还未初始化完IC
                }
            }
        }
    }
    XLOGD("setPhysicalAddr find device fail");
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}


/**
 * 获取固件版本
 */
XBH_S32 XbhHdmiSwitchTask::getFirmwareVersion(XBH_U32 devType, XBH_CHAR* strVersion)
{
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_GSV2705_1:
             hdmiSwitchicID = GSV2705_1;
             break;
         case XBH_UPDATE_BOARD_GSV2705_2:
             hdmiSwitchicID = GSV2705_2;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_2:
             hdmiSwitchicID = GSV2715_2;
             break;
         case XBH_UPDATE_BOARD_GSV6712_1:
             hdmiSwitchicID = GSV6712_1;
             break;
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2702:
             hdmiSwitchicID = GSV2702_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }

    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->getFirmwareVersion(strVersion);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    XLOGD("getFirmwareVersion strVersion = %s ", strVersion);
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 进行固件升级
 */
XBH_S32 XbhHdmiSwitchTask::upgradeFirmware(XBH_U32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XLOGD(" upgradeFirmware devType =%d strFilePath = %s", devType, strFilePath);
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_GSV2705_1:
             hdmiSwitchicID = GSV2705_1;
             break;
         case XBH_UPDATE_BOARD_GSV2705_2:
             hdmiSwitchicID = GSV2705_2;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_2:
             hdmiSwitchicID = GSV2715_2;
             break;
         case XBH_UPDATE_BOARD_GSV6712_1:
             hdmiSwitchicID = GSV6712_1;
             break;
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2702:
             hdmiSwitchicID = GSV2702_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
             XLOGD(" upgradeFirmware i =%d j = %d ",i , j);
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->upgradeFirmware(strFilePath, bForceUpgrade);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 动态功率
 */
XBH_S32 XbhHdmiSwitchTask::setChargingPower(XBH_U32 devType, XBH_U8 data)
{
    XLOGD(" setChargingPower devType =%d data = %d", devType, data);
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_GSV2705_1:
             hdmiSwitchicID = GSV2705_1;
             break;
         case XBH_UPDATE_BOARD_GSV2705_2:
             hdmiSwitchicID = GSV2705_2;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_2:
             hdmiSwitchicID = GSV2715_2;
             break;
         case XBH_UPDATE_BOARD_GSV6712_1:
             hdmiSwitchicID = GSV6712_1;
             break;
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2702:
             hdmiSwitchicID = GSV2702_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后动态分配功率
             //XLOGD(" setChargingPower i =%d j = %d ",i , j);
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->setChargingPower(data);
                XLOGD(" setChargingPower i =%d j = %d ",i , j);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 *获取固件升级状态
  0: 不在升级  1: 正在升级  2: 升级成功  3: 升级失败
*/
XBH_S32 XbhHdmiSwitchTask::getUpgradeState(XBH_U32 devType, XBH_S32 *pState)
{
   XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_GSV2705_1:
             hdmiSwitchicID = GSV2705_1;
             break;
         case XBH_UPDATE_BOARD_GSV2705_2:
             hdmiSwitchicID = GSV2705_2;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_2:
             hdmiSwitchicID = GSV2715_2;
             break;
         case XBH_UPDATE_BOARD_GSV6712_1:
             hdmiSwitchicID = GSV6712_1;
             break;
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2702:
             hdmiSwitchicID = GSV2702_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
     }

    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->getUpgradeState(pState);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    XLOGD(" getUpgradeState  pState = %d", *pState);
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 烧录Hdcp Key
 */
XBH_S32 XbhHdmiSwitchTask::setHdcpKey(XBH_U32 devType, const XBH_CHAR* strFilePath)
{
    XLOGD(" setHdcpKey devType =%d strFilePath = %s", devType, strFilePath);
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
             XLOGD(" setHdcpKey i =%d j = %d ",i , j);
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->setHdcpKey(strFilePath);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}


/**
 * 读取Hdcp Key
 */
XBH_S32 XbhHdmiSwitchTask::getHdmiSwitchHdcpKey(XBH_U32 devType,XBH_CHAR* hdcpValue, XBH_HDCP_TYPE_E hdcpType)
{
    XLOGD("setHdcpKey devType =%d strFilePath = %s hdcpType=%d", devType, hdcpValue,hdcpType);
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
             XLOGD(" setHdcpKey i =%d j = %d ",i , j);
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->getHdmiSwitchHdcpKey(devType,hdcpValue,hdcpType);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 读取 Hdcp Key
 */
XBH_S32 XbhHdmiSwitchTask::getHdcpKeyName(XBH_U32 devType, XBH_CHAR* strHdcpFileName)
{
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
     }
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->getHdcpKeyName(strHdcpFileName);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 烧录 Hdcp Key
 */
XBH_S32 XbhHdmiSwitchTask::setHdcpKeyName(XBH_U32 devType, const XBH_CHAR* strHdcpFileName)
{
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
     }
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->setHdcpKeyName(strHdcpFileName);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
 * 动态修改edid
 */
XBH_S32 XbhHdmiSwitchTask::updateEdid(XBH_U32 devType,XBH_U8 edid_data[256],XBH_U8 port)
{
    XBH_U8 hdmiSwitchicID  = 0;
    hdmiSwitchicID  = devType ;
    
    switch (devType)
    {
         case XBH_UPDATE_BOARD_GSV2705_1:
             hdmiSwitchicID = GSV2705_1;
             break;
         case XBH_UPDATE_BOARD_GSV2705_2:
             hdmiSwitchicID = GSV2705_2;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_2:
             hdmiSwitchicID = GSV2715_2;
             break;
         case XBH_UPDATE_BOARD_GSV6712_1:
             hdmiSwitchicID = GSV6712_1;
             break;
         case XBH_UPDATE_BOARD_LT8668SX_1:
             hdmiSwitchicID = LT8668SX_1;
             break;
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2702:
             hdmiSwitchicID = GSV2702_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    
    // 遍历每个HdmiSwitch
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配，然后更新固件
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->updateEdid(edid_data,port);
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

/**
* 设置type-c的VBUS复位
*/
XBH_S32 XbhHdmiSwitchTask::setTypecReset(XBH_S32 devType)
{
    XBH_U8 hdmiSwitchicID  = 0;

    switch (devType)
    {
         case XBH_UPDATE_FRONT_GSV2712:
             hdmiSwitchicID = GSV2712_1;
             break;
         case XBH_UPDATE_BOARD_GSV2715_1:
             hdmiSwitchicID = GSV2715_1;
             break;
         default:
             XLOGD("Out of range\n");
             break;
    }
    // 遍历每个设备
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            // 检查设备类型是否匹配
            if (mList1[i][j] == hdmiSwitchicID)
            {
                m_pXbhHdmiSwitchInterface[i][j]->setTypecReset();
                return XBH_SUCCESS;  // 假设XBH_SUCCESS是一个有效的返回状态
            }
        }
    }
    XLOGD("setTypecReset find device fail");
    return XBH_FAILURE;  // XBH_FAILURE是一个表示错误或无匹配设备的状态
}

void XbhHdmiSwitchTask::run(const void* arg)
{
    for (int i = 3; i >= 0; i--)
    {
        for (int j = 0; j < 4; j++)
        {
            switch(mList1[i][j])
            {
                case ITE663335_1:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipIt66353(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case ITE663335_2:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipIt66353_2(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case GSV2712_1:
                     m_pXbhHdmiSwitchInterface[i][j] = new ChipGsv2712(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case GSV2712_2:
                    break;
                case LT8641_1:
                    break;
                case LT8641_2:
                    break;
                case ASW3642:
                case TS3DV642:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipAsw3642(mList2[i][j].sGpio, mList2[i][j].sLevel, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case GSV2715_1:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipGsv2715(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case GSV2705_1:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipGsv2705(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case GSV6712_1:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipGsv6712(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case LT8668SX_1:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipLt8668SX(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
                case GSV2702_1:
                    m_pXbhHdmiSwitchInterface[i][j] = new ChipGsv2702(mList2[i][j].iicBus, mList2[i][j].iicAddr, mList2[i][j].pGpio, mList2[i][j].pLevel, mList2[i][j].rGpio, mList2[i][j].rLevel, mList2[i][j].defaultPort, m_pXbhHdmiSwitchPortTable[i][j], 3 - i);
                    break;
            }
        }
    }
    //最里面的switch初始完之后，内部还有流程在跑，因此需要延时1S才算初始化完成
    usleep(1*1000*1000);
    minitdone = XBH_TRUE;
    //更新HDMI CEC地址
    XbhService::getModuleInterface()->initHdmiCecPhysicalAddr();
    //根据project_id改变edid
    XbhService::getModuleInterface()->initProjectIdConfig();

    //如果是FRONT2.u9前置版 需要将功率调整至4.5W
    XBH_U32 mFboardType = property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD);
    if(FRONT2_U9_BOARD == mFboardType)
    {
        XLOGD("mFboardType = FRONT2_U9_BOARD pd power should adjust 4.5W\n");
        setChargingPower(XBH_UPDATE_FRONT_GSV2712, XBH_PDCAPACITY_4_5W);
    }
}

XbhHdmiSwitchTask::XbhHdmiSwitchTask()
{
    XLOGD(" begin ");
    minitdone = XBH_FALSE;
    XbhService::getModuleInterface()->getHdmiSwitchTable(mList1);
    XbhService::getModuleInterface()->getHdmiSwitchI2cTable(mList2);
    XbhService::getModuleInterface()->getHdmiSwitchPortTable(m_pXbhHdmiSwitchPortTable);

    XBH_U8 list1Tmp;
    HDMI_SW_OPT_TABLE list2Tmp;
    HDMI_SWITCH_PORT tabletmp;
    XbhFrontBoard::getInstance()->getHdmiSwitchTable(&list1Tmp);
    XbhFrontBoard::getInstance()->getHdmiSwitchI2cTable(&list2Tmp);
    XbhFrontBoard::getInstance()->getHdmiSwitchPortTable(&tabletmp);
    //将前置版的数据接进来
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(mList1[i][j] == 0xff)
            {
                mList1[i][j] = list1Tmp;
                XBH_S32 prePort = mList2[i][j].prePort;//存储board.h里面定义的前一级switch的port
                memcpy(&mList2[i][j], &list2Tmp, sizeof(list2Tmp));
                mList2[i][j].prePort = prePort;//还原board.h里面定义的前一级switch的port
                memcpy(&m_pXbhHdmiSwitchPortTable[i][j], &tabletmp, sizeof(tabletmp));
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            XLOGV("=====switchlist[%d][%d] = 0x%02x ",i, j, mList1[i][j]);
        
            XLOGV("=====switch_iic_list[%d][%d].iicBus = 0x%02x ",i, j, mList2[i][j].iicBus);
            XLOGV("=====switch_iic_list[%d][%d].iicAddr = 0x%02x ",i, j, mList2[i][j].iicAddr);
            XLOGV("=====switch_iic_list[%d][%d].sGpio = 0x%02x ",i, j, mList2[i][j].sGpio);
            XLOGV("=====switch_iic_list[%d][%d].sLevel = 0x%02x ",i, j, mList2[i][j].sLevel);
            XLOGV("=====switch_iic_list[%d][%d].pGpio = 0x%02x ",i, j, mList2[i][j].pGpio);
            XLOGV("=====switch_iic_list[%d][%d].pLevel = 0x%02x ",i, j, mList2[i][j].pLevel);
            XLOGV("=====switch_iic_list[%d][%d].rGpio = 0x%02x ",i, j, mList2[i][j].rGpio);
            XLOGV("=====switch_iic_list[%d][%d].rLevel = 0x%02x ",i, j, mList2[i][j].rLevel);
            XLOGV("=====switch_iic_list[%d][%d].defaultPort = 0x%02x ",i, j, mList2[i][j].defaultPort);
            XLOGV("=====switch_iic_list[%d][%d].prePort = 0x%02x ",i, j, mList2[i][j].prePort);

            XLOGV("=====switch_port_list[%d][%d].port0 = %d ",i, j, m_pXbhHdmiSwitchPortTable[i][j].port0);
            XLOGV("=====switch_port_list[%d][%d].port1 = %d ",i, j, m_pXbhHdmiSwitchPortTable[i][j].port1);
            XLOGV("=====switch_port_list[%d][%d].port2 = %d ",i, j, m_pXbhHdmiSwitchPortTable[i][j].port2);
            XLOGV("=====switch_port_list[%d][%d].port3 = %d ",i, j, m_pXbhHdmiSwitchPortTable[i][j].port3);
        }
    }
    
    XbhMWThread::run();
    XLOGD(" end ");
}

XbhHdmiSwitchTask::~XbhHdmiSwitchTask()
{

}

XbhHdmiSwitchTask* XbhHdmiSwitchTask::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhHdmiSwitchTask();
    }
    return mInstance;
}
