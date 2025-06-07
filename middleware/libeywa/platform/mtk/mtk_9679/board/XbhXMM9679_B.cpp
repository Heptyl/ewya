#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_B"

#include "XbhLog.h"
#include "XbhXMM9679_B.h"
#include "XbhGSensorManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhGSensorManager.h"
#include "XbhMcuManager.h"
#include "XbhRj45ToUsbManager.h"
#include "XbhAVOutManager.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhLiftbracketManager.h"
#include "XbhNfcManager.h"
#include "XbhNetSwitchManager.h"

#include <stdio.h>
#include <string.h>

XbhXMM9679_B            *XbhXMM9679_B::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_B::mLock;

#define SENSORDATESIZE 20
#define SENSORDATESIZE_AM1008  32
#define TCONDATAZISE  2

#define USB_ETH_MAC_PATH    "/sys/devices/platform/1c7a3e00.usb/1c7a0000.xhci/usb2/2-1/2-1.1/2-1.1:1.0/net/eth0/address"
#define USB_ETH_DEV_NODE    "2-1.1:1.0"
#define USB_ETH_DEV_TYPE    "AX88179A"

#define XBH_HAL_CHECK_SIGNAL "vendor.xbh.hal_check_signal"

static XBH_U32 mUsbCBoard = 1;

XbhXMM9679_B::XbhXMM9679_B()
{
    XLOGD(" begin ");
    #ifdef XBH_USE_HAL_API_TO_CHECK_SIGNAL_STATUS
    property_set(XBH_HAL_CHECK_SIGNAL, "true");
    #endif
    memset(PortHpdState, XBH_TRUE, sizeof(PortHpdState));
    XLOGD(" end ");
}

XbhXMM9679_B::~XbhXMM9679_B()
{
}


static GPIO_OPT_TABLE source_init_table[] = {
    XBH_INIT_TABLE
};

static GPIO_OPT_TABLE source_ops1_table[] = {
    XBH_OPS1_TABLE
};

static GPIO_OPT_TABLE source_f_usbc1_table[] = {
    XBH_F_USBC1_TABLE
};


static GPIO_OPT_TABLE source_ops2_table[] = {
    XBH_OPS2_TABLE
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

static GPIO_OPT_TABLE fct_source_ops1_table[] = {
    XBH_OPS1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_ops2_table[] = {
    XBH_OPS2_FCT_TABLE
};

XBH_S32 XbhXMM9679_B::followToInit(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    select_table(source_init_table,sizeof(source_init_table) / sizeof(GPIO_OPT_TABLE));
    XbhUsbc2HdmiManager::getInstance()->getChipExist(XBH_SOURCE_USBC1, (XBH_BOOL *)&mUsbCBoard);
    XLOGE("mUsbCBoard = %d",mUsbCBoard);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMM9679_B::followToOps1(XBH_VOID)
{
    XLOGE("mUsbCBoard = %d",mUsbCBoard);
    if(mUsbCBoard == 1)
    {
        //FULL USBC
        //select_table(source_ops1_table,sizeof(source_ops1_table) / sizeof(GPIO_OPT_TABLE));
        SELECT_GPIO_OPT_TABLE(source_ops1_table);
    }
    else if(mUsbCBoard == 0)
    {
        //NOT FULL USBC
        //select_table(source_ops2_table,sizeof(source_ops2_table) / sizeof(GPIO_OPT_TABLE));
        SELECT_GPIO_OPT_TABLE(source_ops2_table);
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMM9679_B::followToFUsbc1(XBH_VOID)
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

XBH_S32 XbhXMM9679_B::followToFHdmi1(XBH_VOID)
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

XBH_S32 XbhXMM9679_B::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI1] = *enable;
    s32Ret = switchSourceHpd(2, XBH_SOURCE_HDMI1, XBH_SOURCE_DP1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_B::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI2] = *enable;
    s32Ret = switchSourceHpd(1, XBH_SOURCE_HDMI2, XBH_SOURCE_USBC1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_B::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_HDMI1] = *enable;
    s32Ret = switchSourceHpd(3, XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_USBC1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_B::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_USBC1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_B::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_USBC1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_B::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_DP1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_B::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_OPS1] = *enable;
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::switchSourceHpd(XBH_U8 port, XBH_SOURCE_E SourceHdmi, XBH_SOURCE_E SourceOther)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_SOURCE_E currSource;

    if (XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        return s32Ret;
    }

    s32Ret = XbhSourceManager::getInstance()->getCurrSource(&currSource);
    if (currSource == SourceHdmi && !SourceDetState[SourceHdmi])
    {
        s32Ret = setHdmiHPD(port, XBH_LEVEL_LOW);
        PortHpdState[port] = XBH_FALSE;
        return s32Ret;
    }

    if (!PortHpdState[port] && (SourceDetState[SourceHdmi] || SourceDetState[SourceOther]))
    {
        s32Ret = setHdmiHPD(port, XBH_LEVEL_HIGH);
        PortHpdState[port] = XBH_TRUE;
    }

    if (PortHpdState[port] && !SourceDetState[SourceHdmi] && !SourceDetState[SourceOther])
    {
        s32Ret = setHdmiHPD(port, XBH_LEVEL_LOW);
        PortHpdState[port] = XBH_FALSE;
    }

    return s32Ret;
}

XBH_S32 XbhXMM9679_B::syncBuildinEthMacAddress(void)
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

XBH_S32 XbhXMM9679_B::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XLOGD("setMacAddress\n");
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setMacAddress(strMacAddress);
    s32Ret &= XbhRj45ToUsbManager::getInstance()->syncBuildinEthMacAddress(USB_ETH_MAC_PATH, USB_ETH_DEV_NODE, USB_ETH_DEV_TYPE);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32OpspowerTimer = 500;
    XBH_BOOL status = XBH_FALSE;
    
    while (XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        XLOGD("setOpsPowerCtrl~ wait middleware inited \n");
        usleep(20*1000); /* sleep 20ms */
    }
    
    //OPS启动后几秒内还没有信号输出就会控一次HPD
    //关OPS的条件不需要做拉HPD的处理，否则会导致ops关机后不显示无信号画面，显示黑屏 见bug：#382965
    getOpsPowerStatus(&status, XBH_SOURCE_OPS1);
    if (!status)
    {
        setOpsReidentify(XBH_TRUE);
    }
    else
    {
        setOpsReidentify(XBH_FALSE);
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

XBH_S32 XbhXMM9679_B::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_B::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_B::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_B::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_B::setOpsResetEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (bEnable)
    {
        s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_RESET, XBH_BOARD_GPIO_OPS_RESET_LEVEL);
    }
    else
    {
        s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_BOARD_GPIO_OPS_RESET, !XBH_BOARD_GPIO_OPS_RESET_LEVEL);
    }
    XLOGD("setOpsResetEnable bEnable=%d\n", bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
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
        case XBH_UPDATE_FRONT_LT8711UXE1: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712: 
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        case XBH_UPDATE_FRONT_GSV2202E: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
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
        case XBH_UPDATE_FRONT_LT8711UXE1: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712: 
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2202E: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMM9679_B::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
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
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_BOARD_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC1, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getLightSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
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

/**
 * Tcon检测中获取mcu传过来的数值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM9679_B::getTCONValue(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = XBH_MCU_I2CCMDDEF_E::CMD_I2C_GET_TCON;
    pI2cBuff.len = TCONDATAZISE;
    s32Ret = getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    *value = pI2cBuff.data[0];
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::setLiftbracketOperate(XBH_S32 value)
{
    XBH_S32 ret = XbhLiftbracketManager::getInstance()->setLiftbracketOperate(value);
    return ret;
}

XBH_S32 XbhXMM9679_B::getLiftbracketExist(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLiftbracketManager::getInstance()->getLiftbracketExist(value);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getAirSensorExist(XBH_BOOL* bEnable)
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

XBH_S32 XbhXMM9679_B::getGSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt1 = 0;
    XBH_U32 u32GpioPirInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT1,&u32GpioPirInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT2,&u32GpioPirInt2);
    if(u32GpioPirInt1 == 0 || u32GpioPirInt1 == 0){
        *s32Value = 0;
    }else{
        *s32Value = 1;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 && u32GpioHallInt2 == 0){
        *s32Value = 2;
    }else if(u32GpioHallInt1 != 0 && u32GpioHallInt2 != 0){
        *s32Value = 0;
    }else{
        *s32Value = 1;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32GpioPirInt1 = 0;
    XBH_U32 u32GpioPirInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT1,&u32GpioPirInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT2,&u32GpioPirInt2);
    if(u32GpioPirInt1 == 0 || u32GpioPirInt1 == 0){
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 || u32GpioHallInt2 == 0){
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS0_D2, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_F_USB_PWR_LEVEL : !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
    // sider usb port
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D3, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_PORT_ANDROID_CONU25_PWR_ON : XBH_BOARD_GPIO_PORT_ANDROID_CONU25_PWR_OFF);
    s32Ret = this->XbhMtk_9679::setGpioOutputValue(XBH_TS1_D4, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_PORT_PUB20_PWR_ON : XBH_BOARD_GPIO_PORT_PUB20_PWR_OFF);
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
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

XBH_S32 XbhXMM9679_B::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_USBC1, value);
    }
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMM9679_B::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("set Hdmi Tx Info enInfo = %d data = %d ",enInfo, u32Data);
    switch(enInfo)
    {
        case XBH_HDMI_TX_VERSION:       //HDMI TX芯片的固件版本(可选支持)
            XLOGD("this func is not implement");
            break;
        case XBH_HDMI_TX_TIMING:        //HDMI TX输出分辨率 480P 1080P 4K 分辨率必须支持，其他的可选
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxTiming(MN869129, u32Data);
            break;
        case XBH_HDMI_TX_HDCP_ONOFF:    //HDMI TX HDCP开关
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxHdcp(MN869129, u32Data ? 1 : 0);
            break;
        case XBH_HDMI_TX_VIDEO_MUTE:    //HDMI TX VIDEO 开关(可选支持)
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxVideoMute(MN869129, u32Data ? 1 : 0);
            break;
        case XBH_HDMI_TX_AUDIO_MUTE:    //HDMI TX AUDIO 开关(可选支持)
            s32Ret = XbhAVOutManager::getInstance()->setHdmiTxAudioMute(MN869129, u32Data ? 1 : 0);
            break;
        default:
            break;
    }

    return s32Ret;
}

XBH_S32 XbhXMM9679_B::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getdebug getgetget\n\n");
    XBH_CHAR value[10] = {0};
    XBH_BOOL state = 0;
    XBH_CHAR getTiming = '#';
    switch(enInfo)
    {
        case XBH_HDMI_TX_VERSION:       //HDMI TX芯片的固件版本(可选支持)
            XLOGD("this func is not implement");
            break;
        case XBH_HDMI_TX_TIMING:        //HDMI TX输出分辨率 480P 1080P 4K 分辨率必须支持，其他的可选
            s32Ret = XbhAVOutManager::getInstance()->getHdmiTxTiming(MN869129, &getTiming);
            break;
        case XBH_HDMI_TX_HDCP_ONOFF:    //HDMI TX HDCP开关
            s32Ret = XbhAVOutManager::getInstance()->getHdmiTxHdcp(MN869129, &state);
            break;
        case XBH_HDMI_TX_VIDEO_MUTE:    //HDMI TX VIDEO 开关(可选支持)
            s32Ret = XbhAVOutManager::getInstance()->getHdmiTxVideoMute(MN869129, &state);
            break;
        case XBH_HDMI_TX_AUDIO_MUTE:    //HDMI TX AUDIO 开关(可选支持)
            s32Ret = XbhAVOutManager::getInstance()->getHdmiTxAudioMute(MN869129, &state);
            break;
        case XBH_HDMI_TX_CHIP_ACTIVE:
            s32Ret = XbhAVOutManager::getInstance()->getChipExist(MN869129, &state);
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

XBH_S32 XbhXMM9679_B::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getMute channel = %d",enAudioChannel);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->getHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->getMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
       case XBH_AUDIO_CHANNEL_STEREO:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_B::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("---- onStop ----");
    XBH_BOOL Enabled = XBH_FALSE;
    XBH_U8 nfcenbale;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    if(s8Buff[XBH_WAKEUP_FTYPEC])
    {
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        XLOGD("set front typec 2712 standby \n");
    }

    s32Ret = XbhNetSwitchManager::getInstance()->setWolPwer(Enabled);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("set rtl8367RB_VB WolPwer error \n");
    }

    XbhMtk_9679::getNfcEnable(&Enabled);
    if(Enabled == false)
    {
        nfcenbale = 0x00;
        XbhNfcManager::getInstance()->setNfcLowPower(&nfcenbale);
    }
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);

    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("ro.product.xbh.customer.code", type, "0");
    XLOGD("ro.product.xbh.customer.code : %s \n", type);
    if(strcmp(type, "IYM") == 0){
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);
    }
    return  s32Ret;
}

XbhXMM9679_B *XbhXMM9679_B::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_B();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMM9679_B::setOnResume(XBH_S32 state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(state == XBH_MID_SCREEN_OFF)
    {
        XLOGD("this case is empty\n");
        s32Ret = XBH_SUCCESS;
    }
    else if(state == XBH_MID_SCREEN_ON)
    {
        XLOGD("this case is empty\n");
        s32Ret = XBH_SUCCESS;
    }
    else if(state == XBH_KERNEL_SCREEN_OFF)
    {
        s32Ret = XbhAVOutManager::getInstance()->resetChip(MN869129);
    }
    else if(state == XBH_KERNEL_SCREEN_ON)
    {
        s32Ret = XbhAVOutManager::getInstance()->resumeChip(MN869129);
    }

    XLOGD("setOnResume state=%d \n",state);
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
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

XBH_S32 XbhXMM9679_B::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_TCON_PWR, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_TCON_PWR_ON ? XBH_TRUE : XBH_FALSE;
    // s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_BLK_ON_CTRL, &u32GpioValue);
    // *bEnable &= u32GpioValue == XBH_BOARD_GPIO_BLK_ON_CTRL_ON ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    ret = XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_USBC1, pdability);
    return ret;
}

XBH_S32 XbhXMM9679_B::setBacklightData(XBH_U32 data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 iic_data[7] = {0x00, 0x90, 0x00, 0x26, 0x00, 0x00, 0x00};
    XBH_CHAR hexData[3];
    snprintf(hexData, sizeof(hexData), "%02x", (XBH_U32)data);
    iic_data[6] = (XBH_U8)strtoul(hexData, NULL, 16);
    //XLOGD("kkk iic_data : %02x,%02x,%02x,%02x,%02x,%02x,%02x\n",iic_data[0], iic_data[1],iic_data[2],iic_data[3],iic_data[4],iic_data[5],iic_data[6]);
    s32Ret = this->XbhMtk_9679::setIICData(2,0x54,0xc0,1,7,iic_data);
    return s32Ret;
}

XBH_S32 XbhXMM9679_B::getHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(2, status);
}

XBH_S32 XbhXMM9679_B::getHdmi2SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(1, status);
}

XBH_S32 XbhXMM9679_B::getFHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(3, status);
}

XBH_S32 XbhXMM9679_B::getUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(1, status);
}

XBH_S32 XbhXMM9679_B::getFUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(3, status);
}

XBH_S32 XbhXMM9679_B::getDP1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(2, status);
}

XBH_S32 XbhXMM9679_B::getOps1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_9679::getHdmiPortClockStatus(0, status);
}

