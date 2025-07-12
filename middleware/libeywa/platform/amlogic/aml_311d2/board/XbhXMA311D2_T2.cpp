#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMA311D2_T2"

#include "XbhLog.h"
#include "XbhXMA311D2_T2.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhVgaEdidManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhDp2HdmiManager.h"
#include "XbhMcuManager.h"
#include "XbhAVOutManager.h"
#include "XbhI2cSimulator.h"
#include "XbhAirSensorManager.h"
#include "SystemControlClient.h"
#include "XbhSourceManager.h"
#include "XbhPowerSensorManager.h"

#include <time.h>

extern SystemControlClient* mSystemControlClient;

XbhXMA311D2_T2            *XbhXMA311D2_T2::mInstance = XBH_NULL;
XbhMutex                   XbhXMA311D2_T2::mLock;

//HALL sensor MCU I2C register
#define CMD_I2C_GET_ADC         0xA6
#define RTK8367RB_VB_WOL_DEV            "/sys/devices/virtual/rtl8367/phy/wol"
#define PHY_PORT_STATE_PATH             "/sys/class/rtl8367/phy/update_port_state"
#define XBH_EEPROM_ADDR         0xA0   //eeprom的设备地址
/***************************************PD power config start**********************************/
#define ENABLE_DYNAMIC_CONFIG_PD_CHARGE

#define FRONT_TYPEC_PLUGIN  XBH_BOARD_GPIO_HIGH
#define FRONT_TYPEC_UNPLUG  XBH_BOARD_GPIO_LOW
#define REAR_TYPEC_PLUGIN   XBH_BOARD_GPIO_HIGH
#define REAR_TYPEC_UNPLUG   XBH_BOARD_GPIO_LOW

//PD charge config regiest.
//寄存器的值的意义：0为寄存器复位初始值；0x4为SOC配置100W时的值，0x84为PD在已经设置完成100W充电时更新的值（最高位置1）
#define REG_PD_CHARGE_POWER  0xFF81
#define PD_CHARGE_POWER_15W  1 // MAX 15W
#define PD_CHARGE_POWER_45W  2 // MAX 45W
#define PD_CHARGE_POWER_65W  3 // MAX 65W
#define PD_CHARGE_POWER_100W 4 // MAX 100W
#define PD_CHARGE_POWER_4P5W 5 // MAX 4.5W
#define PD_CHARGE_POWER_50W  6 // MAX 50W

static XBH_S32 curRearTypecDetStatus = REAR_TYPEC_UNPLUG;
static XBH_S32 preRearTypecDetStatus = REAR_TYPEC_UNPLUG;
static XBH_S32 curFrontTypecDetStatus  = FRONT_TYPEC_UNPLUG;
static XBH_S32 preFrontTypecDetStatus  = FRONT_TYPEC_UNPLUG;

enum PD_CHARGE_CFG_TYPE_E {
    PD_CONFIG_AUTO                  = 0, //根据插拔状态情况配置PC充电功率
    FRONT_PD_100W_ONLY              = 1,
    REAR_PD_100W_ONLY               = 2,
    FRONT_PD_50W_THEN_REAR_PD_50W   = 3,
    REAR_PD_50W_THEN_FRONT_PD_50W   = 4,
    FRONT_PD_50W_AND_REAR_PD_50W    = 5,
};

static XBH_U8  gConfiguedType = PD_CONFIG_AUTO;
static XBH_BOOL gIsPDchargeCfgOk = XBH_FALSE;

// 保存当前USBC-IN的工作模式,用于拔出时恢复
static XBH_S32 pre_HostDeviceStatus = XBH_HOST_MODE;
/***************************************PD power config end**********************************/

#ifdef XBH_BOARD_ADC_PIR_CHAN
/*标准时间转换为时间戳*/
int standardtime_to_stamp(int year, int month, int mday, int hour, int minute, int second)
{
    int ret = 0;
    struct tm stm;

    memset(&stm, 0, sizeof(stm));

    stm.tm_year = year-1900;
    stm.tm_mon  = month-1;
    stm.tm_mday = mday;
    stm.tm_hour = hour;
    stm.tm_min  = minute;
    stm.tm_sec  = second;

    ret = mktime(&stm);

    return ret;
}

void set_pir_work_time(void)
{
    std::string mode;
    if(mSystemControlClient && mSystemControlClient->getBootEnv("ubootenv.var.pir_wakeup_sw", mode))
    {
        if(mode.compare("1") == 0)
        {
            XLOGD("PIR wakeup sw is ON.\n");
            time_t now;
            time(&now);

            //get current date
            struct tm *t;
            t = localtime(&now);
            int cur_year  = t->tm_year + 1900;
            int cur_month = t->tm_mon + 1;
            int cur_mday  = t->tm_mday;
            int cur_hour  =  t->tm_hour;
            int cur_minute = t->tm_min;
            int cur_second = t->tm_sec;

            XLOGD("cur: %04d-%02d-%02d %02d:%02d:%02d",cur_year,cur_month,cur_mday,cur_hour,cur_minute,cur_second);
            //get current timestamp
            int cur_stamp = standardtime_to_stamp(cur_year,cur_month,cur_mday,cur_hour,cur_minute,cur_second);

            //XLOGD("cur_stamp=%d",cur_stamp);
            char cur_ts[256] = {0};
            sprintf(cur_ts, "%d", cur_stamp);
            XLOGD("cur_ts:%s\n",cur_ts);

            //get begin timestamp
            int begin_hour;
            int begin_minute;
            int begin_stamp = -1;
            char start_ts[256] = {0};
            char beginHHMM[PROPERTY_VALUE_MAX] = {0};
            if(!property_get("persist.vendor.xbh.proximity.sensor.begintime", beginHHMM, ""))
            {
                memset(beginHHMM, 0, PROPERTY_VALUE_MAX);
                XLOGE("property_get failed\n");
            }
            XLOGD("beginHHMM=%s",beginHHMM);

            if(strlen(beginHHMM) == 5)
            {
                char hour[2] = {0};
                char minute[2] = {0};
                strncpy(hour,beginHHMM,2);
                begin_hour = atoi(hour);
                strncpy(minute,beginHHMM+3,2);
                begin_minute = atoi(minute);
                XLOGD("begin: %04d-%02d-%02d %02d:%02d:00",cur_year,cur_month,cur_mday,begin_hour,begin_minute);
                begin_stamp = standardtime_to_stamp(cur_year,cur_month,cur_mday,begin_hour,begin_minute,0);
                //XLOGD("begin_stamp=%d",begin_stamp);
            }
            else
            {
                XLOGE("beginHHMM is not HH:MM");
            }

            if(begin_stamp > 0)
            {
                sprintf(start_ts, "%d", begin_stamp);
                XLOGD("start_ts:%s\n",start_ts);
                //send the time to BL30
                mSystemControlClient->writeSysfs("/sys/class/cec/start_time",start_ts);
            }
            else
            {
                XLOGE("begin_stamp is invalied !!!\n");
            }

            //get end timestamp
            int end_hour;
            int end_minute;
            int end_stamp = -1;
            char end_ts[256] = {0};
            char endHHMM[PROPERTY_VALUE_MAX] = {0};
            if(!property_get("persist.vendor.xbh.proximity.sensor.endtime", endHHMM, ""))
            {
                memset(endHHMM, 0, PROPERTY_VALUE_MAX);
                XLOGE("property_get failed\n");
            }
            XLOGD("endHHMM=%s",endHHMM);

            if(strlen(endHHMM) == 5)
            {
                char hour[2] = {0};
                char minute[2] = {0};
                strncpy(hour,endHHMM,2);
                end_hour = atoi(hour);
                strncpy(minute,endHHMM+3,2);
                end_minute = atoi(minute);
                XLOGD("end: %04d-%02d-%02d %02d:%02d:00",cur_year,cur_month,cur_mday,end_hour,end_minute);
                end_stamp = standardtime_to_stamp(cur_year,cur_month,cur_mday,end_hour,end_minute,0);
                //XLOGD("end_stamp=%d",end_stamp);
            }
            else
            {
                XLOGE("endHHMM is not HH:MM");
            }

            if(end_stamp > 0)
            {
                sprintf(end_ts, "%d", end_stamp);
                XLOGD("end_ts:%s\n",end_ts);

                //send the time to BL30
                mSystemControlClient->writeSysfs("/sys/class/cec/end_time",end_ts);
                mSystemControlClient->writeSysfs("/sys/class/cec/cur_time",cur_ts);
            }
            else
            {
                XLOGE("end_stamp is invalid !!!\n");
            }
        }
        else
        {
            XLOGE("PIR sw is OFF.\n");
        }

    }
}
#endif

XbhXMA311D2_T2::XbhXMA311D2_T2()
{
    XLOGD(" begin ");
    // 开机初始化USB-C in MODE
    Switch_UsbC_in_Mode();
    XbhMWThread::run(XbhMWThread::REPEAT);
    VgaEnable = XBH_FALSE;
    frontHpdState = XBH_TRUE;
    frontHdmiState = XBH_FALSE;
    frontUsbcState = XBH_FALSE;
    mSystemControlClient = SystemControlClient::getInstance();
    XLOGD(" end ");
}

XbhXMA311D2_T2::~XbhXMA311D2_T2()
{
}

/*记忆host/device开关状态,处理重启后开关状态问题*/
XBH_BOOL XbhXMA311D2_T2::Switch_UsbC_in_Mode(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[PROPERTY_VALUE_MAX] = {0};
    this->XbhAmlogic_311d2::getHostOrDeviceStatus((XBH_S32 *)s8Buff);
    XLOGI("%s, s8Buff[0]:%d",__func__,s8Buff[0]);
    if(s8Buff[0] == XBH_HOST_MODE)
    {
        s32Ret = this->XbhAmlogic_311d2::setHostOrDeviceStatus(XBH_HOST_MODE);
    }
    else if(s8Buff[0] == XBH_DEVICE_MODE)
    {
        s32Ret= this->XbhAmlogic_311d2::setHostOrDeviceStatus(XBH_DEVICE_MODE);
    }
    else
    {
        XLOGD("unkown USBC-C Mode!!!");
    }
    return s32Ret;
}

#define GSV6125_REG_ADDR_COUNT  2 //设备寄存器长度
#define GSV6125_REG_DATA_LEN    1 //写入的数据长度
#define TYPEC_OUT_USB_SWITCH_TO_OPS  1
#define TYPEC_OUT_USB_SWITCH_TO_ANDROID 2
XBH_S32 XbhXMA311D2_T2::followToOps1()
{
    XBH_U8 value = TYPEC_OUT_USB_SWITCH_TO_OPS;
    XBH_S32 s32Ret = XBH_FAILURE;


    XLOGD("switchTypecOutUsbToOPS,%02x, %02x, %02x", XBH_BOARD_I2C_GSV6125_I2C_NUM, XBH_BOARD_I2C_GSV6125_I2C_ADDR, REG_6125_USB_SWITCH_CTRL);
    XbhModuleInterface::followToOps1();
    s32Ret = setIICData(XBH_BOARD_I2C_GSV6125_I2C_NUM, XBH_BOARD_I2C_GSV6125_I2C_ADDR,
                REG_6125_USB_SWITCH_CTRL, GSV6125_REG_ADDR_COUNT, GSV6125_REG_DATA_LEN, &value);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::followToAndroid()
{
    XBH_U8 value = TYPEC_OUT_USB_SWITCH_TO_ANDROID;
    XBH_S32 s32Ret = XBH_FAILURE;

    XLOGD("switchTypecOutUsbToAndroid");
    XbhModuleInterface::followToAndroid();
    s32Ret = setIICData(XBH_BOARD_I2C_GSV6125_I2C_NUM, XBH_BOARD_I2C_GSV6125_I2C_ADDR,
                REG_6125_USB_SWITCH_CTRL, GSV6125_REG_ADDR_COUNT, GSV6125_REG_DATA_LEN, &value);
    return s32Ret;
}


XBH_S32 XbhXMA311D2_T2::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getHdmi3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
    {
        *enable = XBH_FALSE;
    }
    else
    {
        s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_MCU_GPIO_HDMI3_DET, &u32GpioValue);
        *enable =  u32GpioValue == XBH_BOARD_MCU_GPIO_HDMI_3_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }

    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_VGA_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_VGA_PIN_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
XBH_S32 XbhXMA311D2_T2::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhAmlogic_311d2::getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOpsPowerCtrl(XBH_SOURCE_E enSource)
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
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_STATUS_ON ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (bEnable)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
    }
    else
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, !XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
    *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOpsOvercurrentProtection(XBH_U32 srcIdx, XBH_U32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (srcIdx == XBH_SOURCE_OPS1)
    {
        s32Ret = XbhUsbc2HdmiManager::getInstance()->setTypecCapacity(XBH_SOURCE_F_USBC1, value);
    }
    return s32Ret;
}
/*
XBH_S32 XbhXMA311D2_T2::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32GpioHallInt1 = 0;
    XBH_U32 u32GpioHallInt2 = 0;
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT1,&u32GpioHallInt1);
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_INT2,&u32GpioHallInt2);
    if(u32GpioHallInt1 == 0 || u32GpioHallInt2 == 0){
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}*/


XBH_S32 XbhXMA311D2_T2::getHallSensorValue(XBH_S32* s32Value) {
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 mcuPens = 0;  // MCU部分笔数（HALL1+HALL2，0/1/2）
    XBH_S32 socPens = 0;  // SOC部分笔数（HALL3+HALL4，0/1/2）
    XBH_U32 adcValue = 0;

    // 1. 读取MCU的HALL1和HALL2组合ADC值
    // --------------------------------
    XBH_MCU_I2CBUFFDEF_S stOutData;
    stOutData.cmd = CMD_I2C_GET_ADC;
    stOutData.len = 3;
    s32Ret = this->XbhAmlogic_311d2::getIICData(
        XBH_MCU_I2C_CHN, 
        XBH_MCU_I2C_ADR, 
        stOutData.cmd, 
        0x01, 
        stOutData.len, 
        stOutData.data
    );

    if (s32Ret != XBH_SUCCESS) {
        XLOGD("MCU ADC read failed! Error: %d", s32Ret);
        return s32Ret;  
    }

    // 解析MCU ADC值并判断笔数
    adcValue = stOutData.data[0] << 8 | stOutData.data[1];
    if (adcValue > 1000 && adcValue < 2000) {
        mcuPens = 1;  
    } else if (adcValue < 950) {
        mcuPens = 2;  
    } else {
        mcuPens = 0;  
    }
    XLOGD("MCU ADC: %d -> Pens: %d", adcValue, mcuPens);

    // 2. 读取SOC的HALL3和HALL4组合ADC值
    // --------------------------------
#ifdef XBH_BOARD_GPIO_HALL_ADC_CHANNEL
    XBH_U32 socAdcValue = 0;
    s32Ret = getADCChannelValue(
        (XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_GPIO_HALL_ADC_CHANNEL, 
        &socAdcValue
    );
    if (s32Ret == XBH_SUCCESS) {
        if (socAdcValue > 300 && socAdcValue < 800) { 
            socPens = 1;  
        } else if (socAdcValue < 300) {                 
            socPens = 2; 
        } else {          
            socPens = 0;  
        } 
        XLOGD("SOC ADC: %d -> Pens: %d", socAdcValue, socPens);
    } else {
        XLOGD("SOC ADC read failed. Default: no pen.");
    }
#endif

    // 3. 计算总笔数并赋值给s32Value
    // --------------------------------
    *s32Value = mcuPens + socPens;
    XLOGD("Total pens: %d (MCU=%d, SOC=%d)", *s32Value, mcuPens, socPens);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;   //默认为开
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_PIR_CHAN, &adcValue);

    if(adcValue < 800)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;   
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}


XBH_S32 XbhXMA311D2_T2::getPowerSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhPowerSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getPowerSensorValue(XBH_FLOAT* s32Value)
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

XBH_S32 XbhXMA311D2_T2::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,    XBH_BOARD_GPIO_POWER_LED_RED_ON);
            s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE, XBH_BOARD_GPIO_POWER_LED_WHITE_OFF);
            break;
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE,XBH_BOARD_GPIO_POWER_LED_WHITE_ON);
            s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,  XBH_BOARD_GPIO_POWER_LED_RED_OFF);
            break;
        case XBH_LED_LIGHT_OFF:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE,XBH_BOARD_GPIO_POWER_LED_WHITE_OFF);
            s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,  XBH_BOARD_GPIO_POWER_LED_RED_OFF);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
        if(u32GpioValue == XBH_BOARD_GPIO_POWER_LED_RED_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_RED;
        }
        else
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_WHITE;
        }
    }
    //XLOGD("getLedPwrStatus : %d\n",u32GpioValue);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMA311D2_T2::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
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
        default:
            break;
    }
    XLOGD("getMute channel = %d mute = %d ",enAudioChannel, *bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setWolEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8Data;

    s32Ret = this->XbhAmlogic_311d2::setWolMode(bEnable);
    if (s32Ret < 0)
    {
        XLOGE("set wol to bl30 error!!!\n");
        return XBH_FAILURE;
    }

    u8Data = bEnable;
    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_WOL, 1, 1, &u8Data);
    if (s32Ret < 0)
    {
        XLOGE("set wol to mcu error!!!\n");
        return XBH_FAILURE;
    }

    XLOGD("setWolEnable: bEnable = %d, s32Ret = %d",bEnable,s32Ret);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setMacAddrToMCU(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 mac_addr[6] = {0};
    XBH_CHAR mac_addr_str[18] = {0};

    getMacAddress(0, mac_addr_str);
    mac_addr_str[17] = '\0';
    s32Ret = sscanf(mac_addr_str, "%02x:%02x:%02x:%02x:%02x:%02x", &mac_addr[0], &mac_addr[1], &mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);
    if(s32Ret != sizeof(mac_addr))
    {
        XLOGE("get mac addr error!!! scanf result: %d, mac_addr_str: %s, mac_addr: %02x:%02x:%02x:%02x:%02x:%02x\n",
            s32Ret, mac_addr_str, mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        return XBH_FAILURE;
    }
    XLOGD("write mac:%02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_MAC_ADDR, 1, sizeof(mac_addr), mac_addr);
    if (s32Ret < 0)
    {
        XLOGE("set mac addr error!!!\n");
        return XBH_FAILURE;
    }

    XLOGD("setMacAddrToMCU: s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setNetAddrToMCU(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8Data[12] = {0};
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};

    property_get("vendor.xbh.eth0.ip", propVal, "0.0.0.0");
    if (sscanf(propVal, "%u.%u.%u.%u", &u8Data[0], &u8Data[1], &u8Data[2], &u8Data[3]) != 4)
    {
        XLOGE("set ip error!!!\n");
        return XBH_FAILURE;
    }

    property_get("vendor.xbh.eth0.netmask", propVal, "0.0.0.0");
    if (sscanf(propVal, "%u.%u.%u.%u", &u8Data[4], &u8Data[5], &u8Data[6], &u8Data[7]) != 4)
    {
        XLOGE("set mask error!!!\n");
        return XBH_FAILURE;
    }

    property_get("vendor.xbh.eth0.gwip", propVal, "0.0.0.0");
    if (sscanf(propVal, "%u.%u.%u.%u", &u8Data[8], &u8Data[9], &u8Data[10], &u8Data[11]) != 4)
    {
        XLOGE("set gateway error!!!\n");
        return XBH_FAILURE;
    }

    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_NET_ADDR, 1, sizeof(u8Data), u8Data);
    if (s32Ret < 0)
    {
        XLOGE("set mac addr error!!!\n");
        return XBH_FAILURE;
    }

    XLOGD("setNetAddrToMCU: s32Ret = %d", s32Ret);
    return s32Ret;}

XBH_S32 XbhXMA311D2_T2::getVSPort1LinkState(void)
{   XBH_S32 s32Ret = XBH_FAILURE;
    std::string result1;
    mSystemControlClient->readSysfs(PHY_PORT_STATE_PATH, result1);
    std::istringstream iss(result1);
    std::string token;
    std::vector<int> values(5, 0); 

    for (int i = 0; i < 5; ++i) {
        if (!(iss >> token)) break; 
        if (token.empty() || token != "0" && token != "1") {
            XLOGE("Invalid token '%s' at position %d\n", token.c_str(), i);
            return XBH_FAILURE;
        }
        values[i] = std::stoi(token); 
    }

    if ((values[1] == 1)) {
        s32Ret = XBH_SUCCESS;
    }

    XLOGE(" getVSPort1LinkState: s32Ret = %d", s32Ret);
    return s32Ret; 
}

XBH_S32 XbhXMA311D2_T2::getVSPort2LinkState(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string result1;
    mSystemControlClient->readSysfs(PHY_PORT_STATE_PATH, result1);
    std::istringstream iss(result1);
    std::string token;
    std::vector<int> values(5, 0); 

    for (int i = 0; i < 5; ++i) {
        if (!(iss >> token)) break; 
        if (token.empty() || token != "0" && token != "1") {
            XLOGE("Invalid token '%s' at position %d\n", token.c_str(), i);
            return XBH_FAILURE;
        }
        values[i] = std::stoi(token); 
    }

    if ((values[2] == 1)) {
        s32Ret = XBH_SUCCESS;
    }

    XLOGE(" getVSPort2LinkState: s32Ret = %d", s32Ret);
    return s32Ret; 
}

XBH_S32 XbhXMA311D2_T2::getEthPlugStatus(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 eth_port1 =  XbhXMA311D2_T2::getInstance()->getVSPort1LinkState();
    XBH_S32 eth_port2 =  XbhXMA311D2_T2::getInstance()->getVSPort2LinkState();
     if(eth_port1 == XBH_SUCCESS || eth_port2 == XBH_SUCCESS ) //link down -> link up
        {
            XLOGD("NET IC LINK UP~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            *bEnable = 1;
        }
        else //link up -> link down
        {
            XLOGD("NET IC LINK DOWN~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            *bEnable = 0;
        }
    XLOGD("getEthPlugStatus : %d\n",*bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setContrast(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGD("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setContrast(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getContrast(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getContrast();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get Contrast = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::setBrightness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setBrightness(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getBrightness(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getBrightness();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get brightness = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::setHue(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setHue(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getHue(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getHue();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get hue = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::setSharpness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setSharpness(u32Value, 1/*unused*/, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getSharpness(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getSharpness();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI("%s: %d get Sharpness = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::setSaturation(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(u32Value < 0 || u32Value > 100)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }
    return mSystemControlClient->setSaturation(u32Value, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getSaturation(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getSaturation();
    if(s32Ret < 0 || s32Ret > 100)
    {
        XLOGE(" %s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *u32Value = s32Ret;
    XLOGI(" %s: %d get Saturation = %d\n",__func__,__LINE__,*u32Value);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::setSn(const XBH_CHAR* strSn)
{
    return mSystemControlClient->writeUnifyKey("usid", strSn) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string serial;
    if(mSystemControlClient->readUnifyKey("usid", serial)){
        strcpy(strSn,serial.c_str());
        XLOGI("%s %d: serial number -> [%s] \n",__func__,__LINE__,strSn);
    }
    else
    {
        XLOGE("%s %d: serial number error \n",__func__,__LINE__);
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setColorTempMode(XBH_COLORTEMP_E enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(enColorTemp < XBH_COLORTEMP_STANDARD || enColorTemp >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return s32Ret;
    }

    return mSystemControlClient->setColorTemperature(enColorTemp, 1) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhXMA311D2_T2::getColorTempMode(XBH_COLORTEMP_E *enColorTemp)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getColorTemperature();
    if(s32Ret < XBH_COLORTEMP_STANDARD || s32Ret >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("%s: %d value is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    *enColorTemp = (XBH_COLORTEMP_E)s32Ret;
    XLOGI("%s: %d get ColorTemp = %d\n",__func__,__LINE__,*enColorTemp);
    return XBH_SUCCESS;
}

XBH_S32 XbhXMA311D2_T2::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[1] = {0};
    XLOGD("setWoTStatus enWakeUpType=%d bEnable=%d ",enWakeUpType, bEnable);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            s32Ret = setWolEnable(bEnable);
            XLOGD("setWolEnable return=%d  ",s32Ret);
        break;
        case XBH_WAKEUP_SOURCE:
            this->XbhAmlogic_311d2::setWosEnable(bEnable);

            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_HDMI, 1, 1, pu8Data);
            XLOGD("set mcu wos status: %d\n", pu8Data[0]);
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getWoTStatus enWakeUpType=%d  ",enWakeUpType);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            *bEnable = property_get_bool("persist.vendor.xbh.wol.enable", false);
        break;
        case XBH_WAKEUP_SOURCE:
            *bEnable = this->XbhAmlogic_311d2::getWosEnable();
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = 0;
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            type = 1;
            break;
        case XBH_HDMIRX_EDID_TYPE_20:
            type = 2;
            break;
        case XBH_HDMIRX_EDID_TYPE_21:
            type = 3;
            break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            type = 4;
            break;
        default:
            break;
    }
    s32Ret = XbhAmlogic_311d2::setHdmirxEdidType(type);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = 2;
    s32Ret = XbhAmlogic_311d2::getHdmirxEdidType(&type);
    if(type == 1)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_14;
    }
    else if(type == 2)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_20;
    }
    else if(type == 3)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_21;
    }
    else if(type == 4)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_AUTO;
    }
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR sWolStatus[PROPERTY_VALUE_MAX];
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    std::string isWakeupEnableTCP;
    XLOGD("---- onStop ----");

    #ifdef XBH_BOARD_GPIO_RS232_TX_EN
    XBH_BOOL bUartDebugEnable = XBH_FALSE;
    this->XbhAmlogic_311d2::getDebugEnable(&bUartDebugEnable);
    //don't disable the RS232 TX if uart Debug is enabled, then can get the kernel log from the console
    if(!bUartDebugEnable)
    {
         s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_RS232_TX_EN, XBH_BOARD_GPIO_RS232_TX_EN_OFF);
         XLOGD("%s: disable RS232 TX %s", __func__, (s32Ret == 0)?"ok":"fail");
    }

    #endif

    XBH_BOOL wot_enable;
    s32Ret = getWoTStatus(XBH_WAKEUP_SOURCE, &wot_enable);
    if(wot_enable) {
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        XbhUsbc2HdmiManager::getInstance()->setSpecificMode(XBH_SOURCE_USBC1);
    }
    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_HDMI, 1, 1, &wot_enable);

    //red led on, and white led off
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_RED,   XBH_BOARD_GPIO_POWER_LED_RED_ON);
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_LED_WHITE, XBH_BOARD_GPIO_POWER_LED_WHITE_OFF);

    //should close these MCU GPIO when standby
    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_U3RE_EN3, XBH_BOARD_GPIO_U3RE_EN3_OFF);
    if (s32Ret < 0) {
        XLOGE("XBH_BOARD_GPIO_U3RE_EN3 set gpio val fail\n");
    }

    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_USB30_PWREN2, XBH_BOARD_GPIO_USB30_PWREN2_OFF);
    if (s32Ret < 0) {
        XLOGE("XBH_BOARD_GPIO_USB30_PWREN2 set gpio val fail\n");
    }

    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_POWER_DET_PWR, XBH_BOARD_GPIO_POWER_DET_PWR_OFF);
    if (s32Ret < 0) {
        XLOGE("XBH_BOARD_GPIO_POWER_DET_PWR set gpio val fail\n");
    }

    s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_CAM_PWR, XBH_BOARD_GPIO_CAM_PWR_OFF);
    if (s32Ret < 0) {
        XLOGE("XBH_BOARD_GPIO_POWER_DET_PWR set gpio val fail\n");
    }
    property_get("persist.vendor.xbh.wol.enable", sWolStatus, "false");
    if(strncmp(sWolStatus, "true",4) == 0) {
        setWolEnable(XBH_TRUE);
        s32Ret |= setMacAddrToMCU();
        s32Ret |= setNetAddrToMCU();

        /*通知MCU 是否支持TCP IP 唤醒*/
        pI2cBuff.cmd = CMD_I2C_SET_WAKEUP_TCPIP;
        pI2cBuff.len = 1;
        if (mSystemControlClient->getBootEnv("ubootenv.var.tcpipControlEnable", isWakeupEnableTCP)
            && (isWakeupEnableTCP.compare("1") == 0))
        {
            pI2cBuff.data[0] = 1;
            s32Ret |= setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
        }
        else
        {
            pI2cBuff.data[0] = 0;
            s32Ret |= setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
        }
    } else {
        setWolEnable(XBH_FALSE);
    }

#ifdef XBH_BOARD_ADC_PIR_CHAN
    set_pir_work_time();
#endif

    /*通知MCU SOC已待机*/
    pI2cBuff.cmd = CMD_I2C_SOC_POWER_MODE;
    pI2cBuff.len = 1;
    pI2cBuff.data[0] = SOC_STATUS_STANDBY; // 待机
    s32Ret |= setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        case XBH_UPDATE_BOARD_GSV6125:
            s32Ret = XbhAVOutManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV6125, ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV6125:
            s32Ret = XbhAVOutManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV6125, filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType) {
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2202E:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV6125:
            s32Ret = XbhAVOutManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV6125, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("Setvgaedid mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath,idx);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Checksum status mode\n");
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidStatus(idx,bEnable);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::dumpEdid(XBH_SOURCE_E idx)
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

XBH_S32 XbhXMA311D2_T2::setLineOutMode(XBH_LINEOUT_MODE_E enLineOutMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->setLineOutMode(enLineOutMode);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getLineOutMode(XBH_LINEOUT_MODE_E* enLineOutMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getLineOutMode(enLineOutMode);
    return  s32Ret;
}

XBH_S32 XbhXMA311D2_T2::setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length) 
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL, XBH_EEPROM_ADDR);
    if (pSimulator == nullptr) {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Write_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    I2CSimulator* pSimulator = I2CSimulator::getInstance(XBH_BOARD_GPIO_VGA_EEPROM_SDA, XBH_BOARD_GPIO_VGA_EEPROM_SCL, XBH_EEPROM_ADDR);
    if (pSimulator == nullptr) {
        XLOGD("Failed to create I2CSimulator instance\n");
        return XBH_FAILURE;
    }

    s32Ret = pSimulator->IIC_Read_Data(u32RegAddr, u8Data, u32Length);

    return s32Ret;
}

XBH_BOOL XbhXMA311D2_T2::isFrontTypecInDetChanged()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, (XBH_U32 *)&curFrontTypecDetStatus);
    //XLOGD("curFrontTypecDetStatus pdPowerConfig is %d", curFrontTypecDetStatus);
    if(curFrontTypecDetStatus != preFrontTypecDetStatus)
    {
        XLOGD("   ");
        XLOGD("isFrontTypecInDetChanged pdPowerConfig = true");
        preFrontTypecDetStatus = curFrontTypecDetStatus;
        if(curFrontTypecDetStatus == FRONT_TYPEC_UNPLUG)
        {
            XBH_U8 data = PD_CHARGE_POWER_4P5W;
            XBH_S32 ret = setIICData(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, 2, 1, &data);
            XLOGD("front typec unplug, pdPowerConfig set PD to 4.5W, ret = %d",ret);
        }
        else
        {
            XLOGD("front typec pdPowerConfig plugin");
        }

        return XBH_TRUE;
    }

    preFrontTypecDetStatus = curFrontTypecDetStatus;
    return XBH_FALSE;
}


XBH_BOOL XbhXMA311D2_T2::isRearTypecInDetChanged()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, (XBH_U32 *)&curRearTypecDetStatus);


    //XLOGD("curRearTypecDetStatus pdPowerConfig is %d", curRearTypecDetStatus);
    if(curRearTypecDetStatus != preRearTypecDetStatus)
    {
        XLOGD("   ");
        XLOGD("isRearTypecInDetChanged pdPowerConfig = true");
        preRearTypecDetStatus = curRearTypecDetStatus;
        if(curRearTypecDetStatus == REAR_TYPEC_UNPLUG)
        {
            XBH_U8 data = PD_CHARGE_POWER_4P5W;
            XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
            XBH_S32 ret = setIICData(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, 2, 1, &data);
            XLOGD("rear typec unplug, pdPowerConfig set PD to 4.5W and saved OTG mode: %d, ret = %d", pre_HostDeviceStatus, ret);

            // 拔出时恢复USB-C IN工作模式
            this->XbhAmlogic_311d2::setHostOrDeviceStatus(pre_HostDeviceStatus);
        }
        else
        {
            XLOGD("rear typec plugin pdPowerConfig");

            // 插入前保存当前的USB-C IN状态  HOST/DEVICE
            XBH_CHAR s8Buff[PROPERTY_VALUE_MAX] = {0};
            this->XbhAmlogic_311d2::getHostOrDeviceStatus((XBH_S32 *)s8Buff);
            pre_HostDeviceStatus = (XBH_S32)s8Buff[0]; // 保存原始状态
            // USB-C IN插入时强制设置为 DEVICE 模式
            this->XbhAmlogic_311d2::setHostOrDeviceStatus(XBH_DEVICE_MODE);
        }


        return XBH_TRUE;
    }

    preRearTypecDetStatus = curRearTypecDetStatus;
    return XBH_FALSE;
}

void XbhXMA311D2_T2::regValToPowerLevel(XBH_U8 regVal, XBH_CHAR * powerLevle)
{
    if(powerLevle == NULL)
    {
        return;
    }

    XBH_U8 level  = regVal & 0x0F;
    switch (level)
    {
        case 0x06:
            strcpy(powerLevle, "50W");
            break;
        case 0x05:
            strcpy(powerLevle, "4.5W");
            break;
        case 0x04:
            strcpy(powerLevle, "100W");
            break;
        case 0x03:
            strcpy(powerLevle, "65W");
            break;
        case 0x02:
            strcpy(powerLevle, "45W");
            break;
        case 0x01:
            strcpy(powerLevle, "15W");
            break;
        default:
            strcpy(powerLevle, "unkown");
            break;
    }

}

//PD charge config regiest.  100w: 0x04或0x84, 4.5W: 0x05或0x85, 50w: 0x06或0x86, default: 0x00
XBH_BOOL XbhXMA311D2_T2::isPDchargeConfiguedOK(XBH_U32 bus_index, XBH_U8 device_addr, XBH_U16 register_addr,XBH_U8 * data)
{
    XBH_S32 ret1 = -1;
    XBH_S32 ret2 = -1;
    XBH_BOOL bRet  = XBH_FALSE;
    XBH_U8 value = 0x0;
    XBH_CHAR powerLevel[16] = {0};

    if(data == NULL)
    {
        XLOGE("isPDchargeConfiguedOK: pdPowerConfig data is NULL!!!");
        return XBH_FALSE;
    }

    //send command to config the PD charge
    ret1 = setIICData(bus_index, device_addr, register_addr, 2, 1, data);

    if(ret1 != 0)
    {
        XLOGE("PD charge configue pdPowerConfig failed !!!");
    }
    else
    {
        XLOGD("PD charge configue pdPowerConfig send data: 0x%x  to bus:%d, devAddr:0x%x, reg:0x%0x  ok",*data,bus_index,device_addr,register_addr);
    }

    usleep(300*1000);

    //read back the config result
    ret2 = getIICData(bus_index, device_addr, register_addr, 2 ,1 ,&value);
    if(ret2 != 0)
    {
         XLOGE("read back the PD charge pdPowerConfig configue value failed !!!");
    }
    else
    {
        XLOGD("read back PD configued pdPowerConfig value: 0x%x ok",value);
    }

    //config and  get back:  0x04 and 0x84, 0x05 and 0x85, 0x06 and 0x86
    if((ret1 | ret2 == 0) && (value == ((1<<7) | ((*data) & 0x0F))))
    {
        bRet = XBH_TRUE;
    }

    regValToPowerLevel((*data) & 0x0F,powerLevel);
    if(bus_index == XBH_BOARD_I2C_GSV2202E_I2C_NUM)
    {
        XLOGD(">>> config Rear  USB_C pdPowerConfig to %s  %s",powerLevel,bRet?"OK":"failed");
    }
    else if(bus_index == XBH_BOARD_I2C_GSV2712_I2C_NUM)
    {
        XLOGD(">>> config Front USB_C pdPowerConfig %s %s",powerLevel, bRet?"OK":"failed");
    }

    return bRet;
}

//config PD charge to 100W+4.5W or 50W+50W
XBH_BOOL XbhXMA311D2_T2::handlePDcharge()
{
    XBH_S32 ret = -1;
    XBH_BOOL isOk = XBH_TRUE;
    XBH_U8 data = 0x00;

    XLOGD("handlePDcharge: gConfiguedType pdPowerConfig = %d", gConfiguedType);

    //gConfiguedType不是auto时，表示上次做相应配置时没有成功，在此强制重新做相应的充电配置

    if((gConfiguedType == REAR_PD_100W_ONLY ) || (curRearTypecDetStatus == REAR_TYPEC_PLUGIN && curFrontTypecDetStatus == FRONT_TYPEC_UNPLUG))
    {
        gConfiguedType = REAR_PD_100W_ONLY;
        data = PD_CHARGE_POWER_100W;
        isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
    }
    else if((gConfiguedType == FRONT_PD_100W_ONLY) || (curRearTypecDetStatus == REAR_TYPEC_UNPLUG && curFrontTypecDetStatus == FRONT_TYPEC_PLUGIN))
    {
        gConfiguedType = FRONT_PD_100W_ONLY;
        data = PD_CHARGE_POWER_100W;
        isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);

    }
    else if(curRearTypecDetStatus == REAR_TYPEC_PLUGIN && curFrontTypecDetStatus == FRONT_TYPEC_PLUGIN)
    {
        //50W + 50W
        XBH_U8 dataFront = 0x00;
        XBH_U8 dataRear  = 0x00;

        ret = getIICData(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, 2 ,1 ,&dataRear);
        ret = getIICData(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, 2 ,1 ,&dataFront);

        XLOGD("Both plugin: pdPowerConfig dataFront = 0x%x, dataRear = 0x%x , ret = %d",dataFront, dataRear,ret);
        if((gConfiguedType == FRONT_PD_50W_THEN_REAR_PD_50W) || ((ret == 0) && (dataFront != 0) && ((dataFront & 0x0F) != PD_CHARGE_POWER_4P5W)))
        {
            XLOGD("Add Rear pdPowerConfig USB_C plugin");
            gConfiguedType = FRONT_PD_50W_THEN_REAR_PD_50W;
            data = PD_CHARGE_POWER_50W;
            isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
            isOk &= isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);

        }
        else if((gConfiguedType == REAR_PD_50W_THEN_FRONT_PD_50W) || ((ret == 0) && (dataRear != 0) && ((dataRear & 0x0F) != PD_CHARGE_POWER_4P5W)))
        {
            XLOGD("Add Front pdPowerConfig USB_C plugin");
            gConfiguedType = REAR_PD_50W_THEN_FRONT_PD_50W;
            data = PD_CHARGE_POWER_50W;
            isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
            isOk &= isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
        }
        else if((gConfiguedType = FRONT_PD_50W_AND_REAR_PD_50W)
                || ((ret == 0)
                    && (dataRear == 0 || (dataRear & 0x0F) == PD_CHARGE_POWER_4P5W)
                    && (dataFront == 0 || ((dataFront & 0x0F) == PD_CHARGE_POWER_4P5W))))
        {
            //开机时两路typec都有连接设备或两路typec同时接入设备
            XLOGD("Both USB_C pdPowerConfig plugin at the same time");
            gConfiguedType = FRONT_PD_50W_AND_REAR_PD_50W;
            data = PD_CHARGE_POWER_50W;
            isOk = isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2712_I2C_NUM, XBH_BOARD_I2C_GSV2712_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
            isOk &= isPDchargeConfiguedOK(XBH_BOARD_I2C_GSV2202E_I2C_NUM, XBH_BOARD_I2C_GSV2202E_I2C_ADDR, REG_PD_CHARGE_POWER, &data);
        }
    }

    if(!isOk)
    {
        //做了充电配置，且失败了
        XLOGE("handlePDcharge pdPowerConfig failed !!!!");
    }
    else
    {
        //有配置过PD充电且成功了，设置根据两路typec插拔状态来动态配置PD充电功率的模式
        if(gConfiguedType != PD_CONFIG_AUTO)
        {
            gConfiguedType = PD_CONFIG_AUTO;
        }
    }

    return isOk;
}

void XbhXMA311D2_T2::run(const void* arg)
{
#ifdef ENABLE_DYNAMIC_CONFIG_PD_CHARGE
    if(isFrontTypecInDetChanged() || isRearTypecInDetChanged())
    {
        gConfiguedType = PD_CONFIG_AUTO; //typec插拔状态发生了变化，需要根据当前连接状态配置充电
        gIsPDchargeCfgOk = handlePDcharge();
    }
    else if(!gIsPDchargeCfgOk)
    {
        usleep(300*1000);
        //detect status have no change, but the latest configue failed, do it again now.
        XLOGE("the latest pdPowerConfig PD charge config failed,try config it again");
        gIsPDchargeCfgOk = handlePDcharge();
    }
#endif

    usleep(80 * 1000);
}

XBH_S32 XbhXMA311D2_T2::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXA, XBH_HDMI5_CEC_PHYSICAL_ADDR);// ops
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXC, XBH_HDMI1_CEC_PHYSICAL_ADDR);// to Fhdmi
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXD, XBH_HDMI4_CEC_PHYSICAL_ADDR);//hdmi4
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXB, XBH_HDMI6_CEC_PHYSICAL_ADDR);//vga-usbc
    return s32Ret;
}

XbhXMA311D2_T2 *XbhXMA311D2_T2::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMA311D2_T2();
        }
    }
    return mInstance;
}

XBH_S32 XbhXMA311D2_T2::setErrorDetectState(XBH_BOOL bEnable, XBH_U32 enDetectType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[3] = {0};
    pu8Data[0] = bEnable;
    pu8Data[1] = (enDetectType & 0x00ff);
    pu8Data[2] = ((enDetectType >> 8) & 0x00ff);
    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_SET_ERR_DET_STATE, 1, 3, pu8Data);
    XLOGD("setErrorDetectState: %d, enDetectType: 0x%04x\n", bEnable, enDetectType);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getErrorDetectState(XBH_BOOL* bEnable, XBH_U32* enDetectType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[3] = {0};
    s32Ret = getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_GET_ERR_DET_STATE, 1, 3, pu8Data);
    *bEnable = pu8Data[0];
    *enDetectType = pu8Data[1] | (pu8Data[2] << 8);
    XLOGD("getErrorDetectState: %d, enDetectType: 0x%04x\n", *bEnable, *enDetectType);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getErrorCode(XBH_U32* u32ErrorCode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[3] = {0};
    s32Ret = getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_READ_ERROR_CODE, 1, 2, pu8Data);
    *u32ErrorCode = pu8Data[0] | (pu8Data[1] << 8);
    XLOGD("getErrorCode: 0x%04x\n", *u32ErrorCode);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::clearErrorCode(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = setIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_CLEAR_ERROR_RECORDS, 1, 0, NULL);
    XLOGD("clearErrorCode, ret = %d\n", s32Ret);
    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std :: string result;
    std :: string data;
    std :: string wakeup;
    const std :: string strWakeupReason = "/sys/class/cec/wake_up_reason";
    const std :: string strWakeupData = "/sys/class/cec/wake_up_data";
    const std :: string strWakeup = "/sys/class/cec/wake_up";
    XBH_S32 s32WakeupReason = 0;
    XBH_S32 s32WakeupData = 0;
    XBH_CEC_WAKEUP_S stWakeup;

    mSystemControlClient->readSysfs(strWakeupReason, result);
    if(!result.empty())
    {
        s32WakeupReason = strtol(result.c_str(), XBH_NULL, 16);
        s32WakeupReason = s32WakeupReason & WAKEUP_REASON_MASK;
    }

    XLOGD("getWakeUpReason-->s32WakeupReason:%d\n", s32WakeupReason);

    switch(s32WakeupReason)
    {
        case REMOTE_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_IR;
            break;
        case POWER_KEY_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_KEYPAD;
            break;
        case RTC_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_RTC;
            break;
        case UART_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_UART;
            break;
        case WIFI_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_WIFI;
            break;
        case CEC_WAKEUP:
            stWake->enWakeup = XBH_WAKEUP_CEC;
            stWakeup.wk_logic_addr = 0;
            stWakeup.wk_phy_addr = 0;
            stWakeup.wk_port_id = 0;
            mSystemControlClient->readSysfs(strWakeup, wakeup);
            if(!wakeup.empty())
            {
                int cecWakeupPort = strtol(wakeup.c_str(), XBH_NULL, 16);
                memcpy(&stWakeup, &cecWakeupPort, sizeof(stWakeup));
            }
            if (stWakeup.wk_phy_addr == 0x1000)
            {
                stWake->enSrc = XBH_SOURCE_F_HDMI1;
            }
            else if (stWakeup.wk_phy_addr == 0x2000)
            {
                stWake->enSrc = XBH_SOURCE_HDMI1;
            }
            else if (stWakeup.wk_phy_addr == 0x3000)
            {
                stWake->enSrc = XBH_SOURCE_HDMI2;
            }
            else if (stWakeup.wk_phy_addr == 0x4000)
            {
                stWake->enSrc = XBH_SOURCE_HDMI3;
            }
            else
            {
                stWake->enWakeup = XBH_WAKEUP_BUTT;
            }
            break;
        case XBH_EXT_WAKEUP_REASON:
            {
                mSystemControlClient->readSysfs(strWakeupData, data);
                if(!data.empty())
                {
                    s32WakeupData = strtol(data.c_str(), XBH_NULL, 16);
                }
                switch (s32WakeupData)
                {
                    case XBH_EXT_WAKEUP_REASON_SOURCE_VGA:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_VGA1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_HDMI1:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_HDMI1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_HDMI2:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_HDMI2;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_F_HDMI:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_F_HDMI1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_TYPEC1:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_USBC1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_SOURCE_TYPEC2:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        stWake->enSrc = XBH_SOURCE_F_USBC1;
                        break;
                    case XBH_EXT_WAKEUP_REASON_MCU:
                        stWake->enWakeup = XBH_WAKEUP_SOURCE;
                        //stWake->enSrc = XBH_SOURCE_DP1;
                        getMcuWakeUpInfo(stWake);
                        XLOGD("XBH_EXT_WAKEUP_REASON_MCU enWakeup::%d enSrc:%d Value:%d\n",stWake->enWakeup,stWake->enSrc,stWake->u32Value);
                        break;
                    case XBH_EXT_WAKEUP_REASON_UART:
                        stWake->enWakeup = XBH_WAKEUP_UART;
                        break;
                    case XBH_EXT_WAKEUP_REASON_PIR:
                        stWake->enWakeup = XBH_WAKEUP_PIR;
                        break;
                    case XBH_EXT_WAKEUP_REASON_NFC:
                        stWake->enWakeup = XBH_WAKEUP_NFC;
                        break;
                    default:
                        s32Ret = XBH_FAILURE;
                        XLOGE("getWakeUpData fail %s\n",s32WakeupData);
                        break;
                }
            }
            break;
            default:
                s32Ret = XBH_FAILURE;
                XLOGE("getWakeUpReason fail %d\n",s32WakeupReason);
                break;
    }
    XLOGD("getWakeUpReason end %d %d %04x", stWake->enWakeup, stWake->enSrc, stWake->u32Value);

    return s32Ret;
}

XBH_S32 XbhXMA311D2_T2::getMcuWakeUpInfo(XBH_WAKEUP_S *stWake)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[10] = {0};

    s32Ret = this->XbhAmlogic_311d2::getIICData(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_INFO, 1, 10, pu8Data);

    stWake->enWakeup = (XBH_WAKEUP_E)pu8Data[0];
    stWake->u32Value = (uint16_t)(pu8Data[3] * 256 | pu8Data[2]) & 0xffff;

    if (XBH_WAKEUP_SOURCE == stWake->enWakeup)
    {
        stWake->enSrc = (XBH_SOURCE_E)pu8Data[1];
    }
    else
    {
        XLOGD("unknown XBH_WAKEUP_E type!");
    }

    XLOGD("getMcuWakeUpInfo end %d %d %04x", stWake->enWakeup, stWake->enSrc, stWake->u32Value);

    return s32Ret;
}
