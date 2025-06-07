#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_U"

#include "XbhLog.h"
#include "XbhXMM9679_U.h"
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
#include "XbhNfcManager.h"

#include <string.h>

XbhXMM9679_U            *XbhXMM9679_U::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_U::mLock;

#define SENSORDATESIZE 20
#define SENSORDATESIZE_AM1008  32

static XBH_U32 mUsbCBoard = 1;
static XBH_U8 preSourceId = 0;

XbhXMM9679_U::XbhXMM9679_U()
{
    XLOGD(" begin ");
    VgaEnable = XBH_FALSE;
    XLOGD(" end ");
}

XbhXMM9679_U::~XbhXMM9679_U()
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

XBH_S32 XbhXMM9679_U::followToFUsbc1(XBH_VOID)
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

XBH_S32 XbhXMM9679_U::followToFHdmi1(XBH_VOID)
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

XBH_S32 XbhXMM9679_U::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI1] = *enable;
    s32Ret = switchSourceHpd(2, XBH_SOURCE_HDMI1, XBH_SOURCE_DP1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_U::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI2] = *enable;
    s32Ret = switchSourceHpd(1, XBH_SOURCE_HDMI2, XBH_SOURCE_VGA1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_U::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_HDMI1] = *enable;
    s32Ret = switchSourceHpd(3, XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_USBC1);
    return s32Ret;
}
XBH_S32 XbhXMM9679_U::getVga1Det(XBH_BOOL *enable)
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
XBH_S32 XbhXMM9679_U::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_USBC1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_U::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_DP1] = *enable;
    return s32Ret;
}
XBH_S32 XbhXMM9679_U::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_OPS1] = *enable;
    return s32Ret;
}


XBH_S32 XbhXMM9679_U::switchSourceHpd(XBH_U8 port, XBH_SOURCE_E SourceHdmi, XBH_SOURCE_E SourceOther)
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

XBH_S32 XbhXMM9679_U::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_U::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_U::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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


XBH_S32 XbhXMM9679_U::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_U::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMM9679_U::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
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
        case XBH_UPDATE_FRONT_GSV2202E: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712: 
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
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
        case XBH_UPDATE_FRONT_GSV2202E: 
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712: 
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMM9679_U::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
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
        case XBH_UPDATE_FRONT_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getLightSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
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

XBH_S32 XbhXMM9679_U::getAirSensorExist(XBH_BOOL* bEnable)
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

XBH_S32 XbhXMM9679_U::getGSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getPirSensorValue(XBH_S32 *s32Value)
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

XBH_S32 XbhXMM9679_U::getHallSensorValue(XBH_S32* s32Value)
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

XBH_S32 XbhXMM9679_U::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::setUsbLockMode(XBH_U32 u32UsbType)
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

XBH_S32 XbhXMM9679_U::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMM9679_U::setLedPwrStatus(XBH_LED_LIGHT_E enState)
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

XBH_S32 XbhXMM9679_U::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
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

XBH_S32 XbhXMM9679_U::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMM9679_U::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM9679_U::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM9679_U::setOnStop()
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
    XbhMtk_9679::getNfcEnable(&Enabled);
    if(Enabled == false)
    {
        nfcenbale = 0x00;
        XbhNfcManager::getInstance()->setNfcLowPower(&nfcenbale);
    }
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    setGpioOutputValue(XBH_BOARD_GPIO_VGA_PWR, !XBH_BOARD_GPIO_VGA_PWR_LEVEL);
    return  s32Ret;
}



XBH_S32 XbhXMM9679_U::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMM9679_U::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMM9679_U::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMM9679_U::dumpEdid(XBH_SOURCE_E idx)
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

XbhXMM9679_U *XbhXMM9679_U::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_U();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMM9679_U::setOnResume(XBH_S32 state)
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

XBH_S32 XbhXMM9679_U::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
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

XBH_S32 XbhXMM9679_U::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
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
