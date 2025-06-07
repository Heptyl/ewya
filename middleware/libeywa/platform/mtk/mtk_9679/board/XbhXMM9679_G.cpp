#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_G"

#include "XbhRj45ToUsbManager.h"
#include "XbhLog.h"
#include "XbhXMM9679_G.h"
#include "XbhGSensorManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"
#include "XbhGSensorManager.h"
#include "XbhMcuManager.h"
#include "XbhAVOutManager.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhPowerSensorManager.h"
#include "XbhNetSwitchManager.h"

#include <string.h>

XbhXMM9679_G            *XbhXMM9679_G::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_G::mLock;

#define SENSORDATESIZE 20
#define SENSORDATESIZE_AM1008  32

#define USB_ETH_MAC_PATH    "/sys/devices/platform/1c7a3e00.usb/1c7a0000.xhci/usb2/2-1/2-1:1.0/net/eth0/address"
#define USB_ETH_DEV_NODE    "2-1:1.0"
#define USB_ETH_DEV_TYPE    "AX88179A"

static XBH_U32 mUsbCBoard = 1;
static XBH_U8 preSourceId = 0;

XbhXMM9679_G::XbhXMM9679_G()
{
    XLOGD(" begin ");
    VgaEnable = XBH_FALSE;
    XLOGD(" end ");
}

XbhXMM9679_G::~XbhXMM9679_G()
{
}

static GPIO_OPT_TABLE source_f_usbc1_table[] = {
    XBH_F_USBC1_TABLE
};

static GPIO_OPT_TABLE source_f_hdmi1_table[] = {
    XBH_F_HDMI1_TABLE
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table[] = {
    XBH_F_USBC1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table[] = {
    XBH_F_HDMI1_FCT_TABLE
};

XBH_S32 XbhXMM9679_G::followToFUsbc1(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[] = {0x00, 0x00};
    s32Ret = getIICData(3, 0x58, 0xFF3E, 2 ,2 ,data);
    XBH_U8 Value = 0x02;
    if(!s32Ret)
    {
        setIICData(3, 0x58, 0xFF05, 2, 1, &Value);
    }
    SELECT_GPIO_OPT_TABLE(source_f_usbc1_table);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMM9679_G::followToFHdmi1(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[] = {0x00, 0x00};
    s32Ret = getIICData(3, 0x58, 0xFF3E, 2 ,2 ,data);
    XBH_U8 Value = 0x01;
    if(!s32Ret)
    {
        setIICData(3, 0x58, 0xFF05, 2, 1, &Value);
    }
    SELECT_GPIO_OPT_TABLE(source_f_hdmi1_table);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMM9679_G::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_G::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI2] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_G::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_HDMI1] = *enable;
    s32Ret = switchSourceHpd(3, XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_USBC1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_G::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8State;
    s32Ret = XbhVga2HdmiManager::getInstance()->getCurVgaStatus(XBH_SOURCE_VGA1, &u8State);
    if (u8State == 0x85) //STATE_NO_INPUT_SIGNAL
        VgaEnable = XBH_FALSE;
    else if (u8State == 0x0) //STATE_SUCCESS
        VgaEnable = XBH_TRUE;
    *enable = VgaEnable;
    SourceDetState[XBH_SOURCE_VGA1] = *enable;
    return  s32Ret;
}
XBH_S32 XbhXMM9679_G::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_USBC1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_G::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_DP1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_G::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_OPS1] = *enable;

    return s32Ret;
}


XBH_S32 XbhXMM9679_G::switchSourceHpd(XBH_U8 port, XBH_SOURCE_E SourceHdmi, XBH_SOURCE_E SourceOther)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_SOURCE_E currSource;

    s32Ret = XbhSourceManager::getInstance()->getCurrSource(&currSource);

    if (currSource != SourceHdmi && currSource != SourceOther)
    {
        return XBH_FAILURE;
    }

    // 当前信源为拔出状态，HPD拉低
    if (currSource == SourceHdmi && !SourceDetState[SourceHdmi])
    {
        s32Ret = setHdmiHPD(port, XBH_LEVEL_LOW);
        PortHpdState[port] = XBH_FALSE;
        return s32Ret;
    }

    // 当switch上的两个信源设备其中一个设备为插入状态，且HPD为低，则拉高
    if (!PortHpdState[port] && (SourceDetState[SourceHdmi] || SourceDetState[SourceOther]))
    {
        s32Ret = setHdmiHPD(port, XBH_LEVEL_HIGH);
        PortHpdState[port] = XBH_TRUE;
    }

    if (preSourceId == SourceHdmi || preSourceId == SourceOther)
    {
        if (currSource != preSourceId)
        {
            s32Ret = setHdmiHPD(port, XBH_LEVEL_LOW);
            usleep(100*1000);
            s32Ret = setHdmiHPD(port, XBH_LEVEL_HIGH);
        }
    }

    // 当switch上的两个信源设备都为拔出状态，且HPD为高，则拉低
    if (PortHpdState[port] && !SourceDetState[SourceHdmi] && !SourceDetState[SourceOther])
    {
        s32Ret = setHdmiHPD(port, XBH_LEVEL_LOW);
        PortHpdState[port] = XBH_FALSE;
    }

    preSourceId = currSource;

    return s32Ret;
}

XBH_S32 XbhXMM9679_G::syncBuildinEthMacAddress(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char strBuildinMacAddr[XBH_CUSDATA_MACADDRESS_LEN] = {0};
    char strCurMacAddr[XBH_CUSDATA_MACADDRESS_LEN] = {0};
    
    XBH_S32 times = 0;
    while (access(USB_ETH_MAC_PATH, F_OK) == -1 && times < 10) {
        XLOGE("file not exist!\n");
        usleep(1000*1000);
        times++;
    }

    XLOGD("file exist!\n");

    s32Ret = getBuildinEthMacAddress(USB_ETH_MAC_PATH, strBuildinMacAddr);
    if (s32Ret == XBH_SUCCESS)
    {
        getMacAddress(0, strCurMacAddr);
        if (strcasecmp(strBuildinMacAddr, strCurMacAddr) != 0)
        {
            XbhRj45ToUsbManager::getInstance()->syncBuildinEthMacAddress(USB_ETH_MAC_PATH, USB_ETH_DEV_NODE, USB_ETH_DEV_TYPE);
            XLOGD("syncBuildinEthMacAddress sync mac to emmc\n");
        }
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XLOGD("setMacAddress\n");
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setMacAddress(strMacAddress);
    s32Ret &= XbhRj45ToUsbManager::getInstance()->syncBuildinEthMacAddress(USB_ETH_MAC_PATH, USB_ETH_DEV_NODE, USB_ETH_DEV_TYPE);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32OpspowerTimer = 500;
    while (XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        XLOGD("setOpsPowerCtrl~ wait middleware inited \n");
        usleep(20*1000); /* sleep 20ms */
    }
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_POWER_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_POWER_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMM9679_G::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (enSource == XBH_SOURCE_OPS1)
    {
        if (bEnable)
        {
            s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
        }
        else
        {
            s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, !XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
        }
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_9679::getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_DP1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712: 
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        case XBH_UPDATE_ITE68411:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_ITE68411, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712: 
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        case XBH_UPDATE_ITE68411: 
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_ITE68411, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMM9679_G::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_ITE68411:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_ITE68411, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getLightSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = XBH_MCU_I2CCMDDEF_E::CMD_I2C_GET_AM1008;
    pI2cBuff.len = SENSORDATESIZE_AM1008;
    s32Ret = getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    memcpy(sensorDate, pI2cBuff.data, SENSORDATESIZE_AM1008);
    *fCo2 = (sensorDate[0] * 256)  + sensorDate[1];
    *fTvoc = (sensorDate[2] * 256)  + sensorDate[3];
    *fHumidity =((sensorDate[4] * 256)  + sensorDate[5]) /10.0 ;
    *fTemperature =((sensorDate[6] * 256)  + sensorDate[7] -500) /10.0 ;
    *fPm2p5 = (sensorDate[16] * 256) + sensorDate[17];
    *fPm1p0 = 0;
    *fPm10 = 0;
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getAirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = XBH_MCU_I2CCMDDEF_E::CMD_I2C_GET_AM1008;
    pI2cBuff.len = SENSORDATESIZE_AM1008;
    s32Ret = getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    memcpy(sensorDate, pI2cBuff.data, SENSORDATESIZE_AM1008);
    if(sensorDate[7] != 0)
    {
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getGSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS0_D2, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    // sider usb port
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D3, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D4, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_HIGH : XBH_BOARD_GPIO_LOW);
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->setHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->setMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_STEREO:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL Enabled = XBH_FALSE;
    XLOGD("---- onStop ----");
    onStopstate = 1;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    if(s8Buff[XBH_WAKEUP_FTYPEC])
    {
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        XLOGD("set front typec 2712 standby \n");
    }
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, XBH_BOARD_GPIO_BLK_ON_CTRL_OFF);
    usleep(150 *1000);
    s32Ret = XbhAVOutManager::getInstance()->setVboTxDiffSignals(NT68411, VBO_TX_DIFF_SIGNALS_TURN_OFF);
    usleep(50 *1000);
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, XBH_BOARD_GPIO_LOW);

    s32Ret = XbhNetSwitchManager::getInstance()->setWolPwer(Enabled);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("set rtl8367RB_VC WolPwer error \n");
    }
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_AMP_MUTE, !XBH_BOARD_GPIO_AMP_MUTE_ON);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_NT68411_RESET, XBH_NT68411_RESET_OFF);
    return  s32Ret;
}



XBH_S32 XbhXMM9679_G::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::dumpEdid(XBH_SOURCE_E idx)
{
    XLOGD("dump edid\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(idx)
    {
        case XBH_SOURCE_E::XBH_SOURCE_VGA1:
            s32Ret = XbhVgaEdidManager::getInstance()->dumpEdid();
        break;
        default:
        break;
    }
    return s32Ret;
}

XbhXMM9679_G *XbhXMM9679_G::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_G();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMM9679_G::setOnResume(XBH_S32 state)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    if(state == XBH_MID_SCREEN_OFF)
    {
        XLOGD("VBO screen off, turn off vbo diff signals\n");
        s32Ret = XbhAVOutManager::getInstance()->setVboTxDiffSignals(NT68411, VBO_TX_DIFF_SIGNALS_TURN_OFF);
    }
    else if(state == XBH_MID_SCREEN_ON)
    {
        XLOGD("VBO screen on, turn on vbo diff signals\n");
        s32Ret = XbhAVOutManager::getInstance()->setVboTxDiffSignals(NT68411, VBO_TX_DIFF_SIGNALS_TURN_ON);
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (bEnable)
    {
        // 控制 tcon
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, XBH_BOARD_GPIO_TCON_PWR_ON);
        // 等待
        usleep(500 * 1000);
        // 控制 blk
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, XBH_BOARD_GPIO_BLK_ON_CTRL_ON);
    }
    else
    {
        // 控制 blk
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, XBH_BOARD_GPIO_BLK_ON_CTRL_OFF);
        // 等待
        usleep(50 * 1000);
        // 控制 tcon
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, XBH_BOARD_GPIO_TCON_PWR_OFF);
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_TCON_PWR_ON ? XBH_TRUE : XBH_FALSE;
    // s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, &u32GpioValue);
    // *bEnable &= u32GpioValue == XBH_BOARD_GPIO_BLK_ON_CTRL_ON ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("set Hdmi Tx Info enInfo = %d data = %d ",enInfo, u32Data);
    if(onStopstate) {
        XLOGD("power off, set hdmi tx quit");
        return XBH_SUCCESS;
    }
    switch(enInfo)
    {
        case XBH_HDMI_TX_VERSION:       //HDMI TX芯片的固件版本(可选支持)
            XLOGD("this func is not implement");
            break;
        case XBH_HDMI_TX_TIMING:        //HDMI TX输出分辨率 480P 1080P 4K 分辨率必须支持，其他的可选
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxTiming(NT68411, u32Data);
            break;
        case XBH_HDMI_TX_HDCP_ONOFF:    //HDMI TX HDCP开关
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxHdcp(NT68411, u32Data ? 1 : 0);
            break;
        case XBH_HDMI_TX_VIDEO_MUTE:    //HDMI TX VIDEO 开关(可选支持)
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxVideoMute(NT68411, u32Data ? 1 : 0);
            break;
        case XBH_HDMI_TX_AUDIO_MUTE:    //HDMI TX AUDIO 开关(可选支持)
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxAudioMute(NT68411, u32Data ? 1 : 0);
            break;
        default:
            break;
    }

    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getdebug getgetget\n\n");
    XBH_CHAR value[10] = {0};
    XBH_BOOL state = 0;
    XBH_CHAR getTiming = '#';
    if(onStopstate) {
        XLOGD("power off, get hdmi tx quit");
        info[0] = '0';
        return XBH_SUCCESS;
    }
    switch(enInfo)
    {
        case XBH_HDMI_TX_VERSION:       //HDMI TX芯片的固件版本(可选支持)
            XLOGD("this func is not implement");
            break;
        case XBH_HDMI_TX_PLUG_STATE:
            s32Ret = XbhAVOutManager::getInstance()->getHdmiTxPlugState(NT68411, &state);
        default:
            break;
    }
    if (getTiming != '#')
    {
        info[0] = getTiming + 48;
    }
    else
    {
        info[0] = (XBH_CHAR)state + 48;
    }

    return s32Ret;
}

XBH_S32 XbhXMM9679_G::setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("call parent setMultiPanel\n");
    s32Ret = XbhMtk_9679::setMultiPanel(type, enPanel);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Failed to call parent setMultiPanel\n");
        return s32Ret;
    }

    XLOGD("set Vx1 Tx Tcon Mode\n");
    s32Ret = XbhAVOutManager::getInstance()->setVx1TxTconMode(NT68411, type ? 1 : 0);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Failed to set Vx1 Tx Tcon Mode\n");
        return s32Ret;
    }

    return s32Ret;
}

XBH_S32 XbhXMM9679_G::getPowerSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhPowerSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getPowerSensorValue(XBH_FLOAT* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("kkk getPowerSensorValue ");
    s32Ret = XbhPowerSensorManager::getInstance()->getPowerSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getPowerSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    if(enSource == XBH_SOURCE_F_USBC1)
    {
        ret = XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_FRONT_GSV2712, pdability);
    }
    return ret;
}
XBH_S32 XbhXMM9679_G::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, !XBH_BOARD_GPIO_F_LED_G_ON);//0
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, !XBH_BOARD_GPIO_F_LED_R_ON);//1
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, XBH_BOARD_GPIO_F_LED_G_ON);//1
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);//0
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_G::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
        if(u32GpioValue == XBH_BOARD_GPIO_F_LED_R_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_RED;
        }
        else if (u32GpioValue == XBH_BOARD_GPIO_F_LED_G_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_GREEN;
        }
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_G::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXD, XBH_HDMI2_CEC_PHYSICAL_ADDR);
    return s32Ret;
}

