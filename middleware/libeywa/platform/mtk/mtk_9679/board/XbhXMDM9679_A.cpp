#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMDM9679_A"

#include "XbhLog.h"
#include "XbhXMDM9679_A.h"
#include "XbhGSensorManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhGSensorManager.h"
#include "XbhMcuManager.h"
#include "XbhAVOutManager.h"
#include "XbhService.h"

#include <string.h>

XbhXMDM9679_A            *XbhXMDM9679_A::mInstance = XBH_NULL;
XbhMutex                  XbhXMDM9679_A::mLock;

#define SENSORDATESIZE 20
#define SENSORDATESIZE_AM1008  32

static XBH_U32 mUsbCBoard = 1;

XbhXMDM9679_A::XbhXMDM9679_A()
{
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMDM9679_A::~XbhXMDM9679_A()
{
}


static GPIO_OPT_TABLE source_init_table[] = {
    XBH_INIT_TABLE
};

static GPIO_OPT_TABLE source_ops1_table[] = {
    XBH_OPS1_TABLE
};

static GPIO_OPT_TABLE source_ops2_table[] = {
    XBH_OPS2_TABLE
};

XBH_S32 XbhXMDM9679_A::followToInit(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    select_table(source_init_table,sizeof(source_init_table) / sizeof(GPIO_OPT_TABLE));
    XbhUsbc2HdmiManager::getInstance()->getChipExist(XBH_SOURCE_USBC1, (XBH_BOOL *)&mUsbCBoard);
    XLOGE(" ==yifan== mUsbCBoard = %d",mUsbCBoard);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMDM9679_A::followToOps1(XBH_VOID)
{
    if(mUsbCBoard == 1)
    {
        //FULL USBC
        select_table(source_ops1_table,sizeof(source_ops1_table) / sizeof(GPIO_OPT_TABLE));
    }
    else if(mUsbCBoard == 0)
    {
        //NOT FULL USBC
        select_table(source_ops2_table,sizeof(source_ops2_table) / sizeof(GPIO_OPT_TABLE));
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhXMDM9679_A::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_HDMI1] = *enable;
    s32Ret = switchSourceHpd(2, XBH_SOURCE_HDMI1, XBH_SOURCE_DP1);
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_HDMI1] = *enable;
    s32Ret = switchSourceHpd(3, XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_USBC1);
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_F_USBC1] = *enable;
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_DP1] = *enable;
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    SourceDetState[XBH_SOURCE_OPS1] = *enable;
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::switchSourceHpd(XBH_U8 port, XBH_SOURCE_E SourceHdmi, XBH_SOURCE_E SourceOther)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_SOURCE_E currSource;

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

XBH_S32 XbhXMDM9679_A::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMDM9679_A::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1) {
        s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_POWER_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_POWER_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1) {
        s32Ret = this->XbhMtk_9679::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMDM9679_A::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMDM9679_A::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMDM9679_A::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
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
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;

        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMDM9679_A::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;

        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getLightSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
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

XBH_S32 XbhXMDM9679_A::getAirSensorExist(XBH_BOOL* bEnable)
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

XBH_S32 XbhXMDM9679_A::getGSensorExist(XBH_BOOL* bEnable) 
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getPirSensorValue(XBH_S32 *s32Value)
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

XBH_S32 XbhXMDM9679_A::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 || u32GpioHallInt2 == 0){
        *s32Value = 0;
    }else{
        *s32Value = 1;
    }
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::setUsbLockMode(XBH_U32 u32UsbType)
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

XBH_S32 XbhXMDM9679_A::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            if(bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_AMP_MUTE, XBH_BOARD_GPIO_AMP_MUTE_ON);
            }
            else
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_AMP_MUTE, !XBH_BOARD_GPIO_AMP_MUTE_ON);
            }
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->setHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->setMuteMicIn(bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMDM9679_A::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMDM9679_A::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("---- onStop ----");
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, !XBH_BOARD_GPIO_F_LED_G_ON);
    s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);
    return  s32Ret;
}

XbhXMDM9679_A *XbhXMDM9679_A::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMDM9679_A();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMDM9679_A::setOnResume(XBH_S32 state)
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

XBH_S32 XbhXMDM9679_A::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
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

XBH_S32 XbhXMDM9679_A::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
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