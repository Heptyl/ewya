#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhHi_610"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/sysinfo.h>
#include <regex.h>

#include <hardware/board.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/rtc.h>
#include <linux/gpio.h>
#include <linux/watchdog.h>

#include <cutils/properties.h>
#include <utils/String8.h>
#include "XbhSysOpt.h"

#include "XbhLog.h"
#include "XbhHi_610.h"
#include "XbhSysOpt.h"
#include "XbhParseINI.h"
#include "XbhCrc.h"

#include "XbhLightSensorManager.h"
#include "XbhNfcManager.h"
#include "XbhMcuManager.h"
#include "XbhKernelMsgManager.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhVgaEdidManager.h"
#include "XbhHdmiSwitchManager.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include "uapi_gpio.h"
#include "uapi_i2c.h"
#include "uapi_uart.h"
#include "uapi_system.h"
#include "uapi_hdmirx.h"
#include "uapi_otp.h"
#include "uapi_lsadc.h"
#include "uapi_pq.h"
#include "uapi_disp.h"
#include "uapi_pdm.h"
#include "hi_mw_cfg.h"
#include "hi_mw_array.h"

//kernel 默认打印等级
#ifndef ITEM_KERNEL_LOGLEVEL
#define ITEM_KERNEL_LOGLEVEL 7
#endif
#define RTC_DEV_PATH    "/dev/rtc0"
#define PQ_BIN_PATH     "/odm/atv/pq"


#ifndef XBH_BOARD_NTC_ADC_THERMISTOR_VAL
#define XBH_BOARD_NTC_ADC_THERMISTOR_VAL                 100 /*value of resistance: 100K*/
#endif
#define ADC_COUNT           5 //ADC采样次数
#define NTC_ADC_100K_0402_MIN -40
#define NTC_ADC_100K_0402_LEN 141

using namespace android;

//----------------------------------------------------- static function begin ------------------------------------------------------------------------------------------------------
static XBH_U8 source_available[] = XBH_SRC_AVAILABLE_TABLE
XbhMutex  XbhHi_610::m_McuLock;
XBH_BOOL m_uapi_gpio_init_ok = XBH_FALSE;
XBH_BOOL m_uapi_i2c_init_ok = XBH_FALSE;
XBH_BOOL m_uapi_hdmirx_init_ok = XBH_FALSE;

map<int, string> XbhHi_610::net_list = {
        {0, "/sys/class/net/eth0/speed"},
        {1, "/sys/class/net/eth1/speed"},
        {2, "/sys/class/net/dummy0/speed"}
    };

struct xbh_fd_closer
{
    xbh_fd_closer(int fd):m_fd(fd){
    };
    void operator()(void *) const{
        if(m_fd >= 0){
            close(m_fd);
        }
    }
    int m_fd{-1};
};

//gpio struct
typedef struct {
    XBH_U32 gpio;
    XBH_U32 state;
    XBH_U32 jitter;
} UserData;

struct gpio_common_info {
    XBH_CHAR* gpio_name;
    XBH_U32 gpio_number;
};

const XBH_U32 NTC_ADC_100K_0402[NTC_ADC_100K_0402_LEN] =
{
    4054000, 3781000, 3528000, 3294000, 3077000, 2875000, 2688000, 2514000, 2353000, 2203000,  //-40 ~ -31
    2063000, 1933000, 1812000, 1700000, 1595000, 1497000, 1405000, 1320000, 1241000, 1166000,  //-30 ~ -21
    1097000, 1032000,  971500,  914700,  861700,  812000,  765400,  721800,  680900,  642600,  //-20 ~ -11
     606600,  572900,  541200,  511500,  483500,  457300,  432600,  409400,  387500,  367000,  //-10 ~ -9
     347600,  329400,  312200,  296100,  280800,  266400,  252900,  240100,  228000,  216600,  //0 ~ 9
     205800,  195600,  186000,  176900,  168300,  160100,  152400,  145100,  138200,  131700,  //10 ~ 19
     125500,  119600,  114000,  108800,  103800,   99000,   94400,   90040,   85910,   81980,  //20 ~ 29
      78260,   74720,   71360,   68170,   65140,   62260,   59520,   56920,   54440,   52080,  //30 ~ 39
      49000,   47100,   46000,   43000,   41890,   40130,   38460,   36860,   35340,   33890,  //40 ~ 49
      32500,   31180,   29920,   28720,   27570,   26470,   25420,   24420,   23460,   22550,  //50 ~ 59
      21670,   20830,   20030,   19200,   18500,   17800,   17000,   16300,   15910,   15320,  //60 ~ 69
      14750,   14210,   13690,   13190,   12720,   12260,   11820,   11400,   11000,   10610,  //70 ~ 79
      10240,    9880,    9300,    8800,    8600,    8300,    8000,    7700,    7500,    7200,  //80 ~ 89
       6995,    6764,    6542,    6328,    6122,    5924,    5733,    5549,    5372,    5202,  //90 ~ 99
       4996  //100
};

static struct gpio_common_info gpio_common_table[] = {
#ifdef XBH_BOARD_GPIO_BOOT_COMMON_TABLE
    XBH_BOARD_GPIO_BOOT_COMMON_TABLE
#endif
};

//Xbh Partition
#define MSDC_CUSDATA_PATH  "/dev/block/by-name/cusdata"
#define MSDC_CUSPARAM_PATH "/dev/block/by-name/cusparam"
//HuaWei starting logo
#define XBH_CUST_LOGO_PATH "/mnt/xbhcust/logo/bootlogo.jpg"

//无效数值
#undef XBH_NO_CONFIG
#define XBH_NO_CONFIG                       9999

#define EXT_I2C_MAX_LENGTH 2048

#ifndef ENABLE_COLOR_TEMP_CHECK
#define ENABLE_COLOR_TEMP_CHECK 0
#endif

#ifndef COLOR_TEMP_GAIN_MAX
#define COLOR_TEMP_GAIN_MAX 255
#endif

#ifndef COLOR_TEMP_GAIN_MIN
#define COLOR_TEMP_GAIN_MIN  0
#endif

#ifndef COLOR_TEMP_GAIN_DEFAULT 
#define COLOR_TEMP_GAIN_DEFAULT 127
#endif

#ifndef COLOR_TEMP_GAIN_OFFSET_MAX 
#define COLOR_TEMP_GAIN_OFFSET_MAX 255
#endif

#ifndef COLOR_TEMP_GAIN_OFFSET_MIN 
#define COLOR_TEMP_GAIN_OFFSET_MIN 0
#endif

#ifndef COLOR_TEMP_GAIN_OFFSET_DEFAULT 
#define COLOR_TEMP_GAIN_OFFSET_DEFAULT 127
#endif

static XBH_COLORTEMP_E checkinitColorTempMode(const XBH_CHAR* ColorTempPara)
{
    if (strcmp(ColorTempPara,"XBH_COLORTEMP_STANDARD") == 0) {
        return XBH_COLORTEMP_STANDARD;
    } else if (strcmp(ColorTempPara,"XBH_COLORTEMP_WARM") == 0) {
        return XBH_COLORTEMP_WARM;
    } else if (strcmp(ColorTempPara,"XBH_COLORTEMP_COLD") == 0) {
        return XBH_COLORTEMP_COLD;
    } else if (strcmp(ColorTempPara,"XBH_COLORTEMP_USER") == 0) {
        return XBH_COLORTEMP_USER;
    } else {
        return XBH_COLORTEMP_BUTT;
    }
}

static XBH_S32 checkMacFormat(const XBH_CHAR* strMacAddress)
{
    if (NULL == strMacAddress)
    {
        XLOGE("strMacAddress is null.");
        return XBH_FAILURE;
    }

    XBH_CHAR* p = const_cast<XBH_CHAR*>(strMacAddress);

    if (17 != strlen(strMacAddress))
    {
        XLOGE("Mac Address format error. Length not 17.");
        return XBH_FAILURE;
    }
    while (*p != '\0')
    {
        if (2 == (p - strMacAddress)%3)
        {
            if ( *p != ':')
            {
                XLOGE("Mac Address format error.");
                return XBH_FAILURE;
            }
            p++;
            continue;
        }
        if (*p < '0' || (*p > '9' && *p < 'A') ||(*p > 'F' && *p < 'a') || *p > 'f' )
        {
            XLOGE("Mac Address format error.");
            return XBH_FAILURE;
        }
        p++;
    }
    return XBH_SUCCESS;
}

//----------------------------------------------------- static function end---------------------------------------------------------------------------------------------------------
XBH_S32 XbhHi_610::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;

#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        //MCU升级锁锁定则不进行MCU GPIO控制
        if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
        {
            XLOGE("MCU is upgrading, setGpioOutputValue(MCU GPIO) has been disabled\n");
            return XBH_FAILURE;
        }

        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        //XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = setMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, u32GpioValue);
        return s32Ret;
    }
#endif

    /*
    此接口总是读取错误,错误码:0x80470005
    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        XLOGV("-- gpio %d is not output pin set output now", u32GpioNumber);
        s32Ret = setGpioDirOutput(u32GpioNumber, u32GpioValue);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGE("set gpio-%d out dir error\n", u32GpioNumber);
            return XBH_FAILURE;
        }
    }
    */
    s32Ret = setGpioDirOutput(u32GpioNumber, u32GpioValue);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGE("set gpio-%d out dir error\n", u32GpioNumber);
    }

    s32Ret = uapi_gpio_write(u32GpioNumber / 8, u32GpioNumber % 8, (td_bool)u32GpioValue);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("uapi_gpio_write ErrorCode=0x%x, set gpio-%d out value fail\n", s32Ret, u32GpioNumber);
        return XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhHi_610::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;
    td_bool value;

#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        //MCU升级锁锁定则不进行MCU GPIO控制
        if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
        {
            XLOGE("MCU is upgrading, getGpioOutputValue(MCU GPIO) has been disabled\n");
            return XBH_FAILURE;
        }

        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        //XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_OUTPUT, u32GpioValue);
        return s32Ret;
    }
#endif

    /*
    此接口总是读取错误,错误码:0x80470005
    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        XLOGV("-- gpio %d is not output pin set output now", u32GpioNumber);
        s32Ret = setGpioDirOutput(u32GpioNumber, *u32GpioValue);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGE("set gpio-%d out dir error\n", u32GpioNumber);
            return XBH_FAILURE;
        }
    }
    */
    s32Ret = setGpioDirOutput(u32GpioNumber, *u32GpioValue);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGE("set gpio-%d out dir error\n", u32GpioNumber);
    }

    s32Ret = uapi_gpio_read(u32GpioNumber / 8, u32GpioNumber % 8, &value);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("uapi_gpio_read ErrorCode=0x%x, get gpio-%d out value fail\n", s32Ret, u32GpioNumber);
        return XBH_FAILURE;
    }

    *u32GpioValue = value;
    return s32Ret;

}

XBH_S32 XbhHi_610::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;
    td_bool value;

#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        //XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_INPUT, u32GpioValue);
        return s32Ret;
    }
#endif

    /*
    此接口总是读取错误,错误码:0x80470005
    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_INPUT){
        XLOGV("-- gpio %d is not input pin set input now",u32GpioNumber);
        if(setGpioDirInput(u32GpioNumber) != XBH_SUCCESS)
        {
            XLOGE("set gpio-%d dir input error\n", u32GpioNumber);
            return XBH_FAILURE;
        }
    }
    */
    if(setGpioDirInput(u32GpioNumber) != XBH_SUCCESS)
    {
        XLOGE("set gpio-%d dir input error\n", u32GpioNumber);
    }

    s32Ret = uapi_gpio_read(u32GpioNumber / 8, u32GpioNumber % 8, &value);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("uapi_gpio_read ErrorCode=0x%x, get gpio-%d out value fail\n", s32Ret, u32GpioNumber);
        return XBH_FAILURE;
    }

    *u32GpioValue = value;

    return s32Ret;
}

XBH_S32 XbhHi_610::setPanelRotation(XBH_S32 value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    sprintf(propVal,"%d",(value*90));
    property_set("persist.sys.hwrotation", propVal);
    return  s32Ret;
}


XBH_S32 XbhHi_610::setIICLock(XBH_U32 iicBus, XBH_BOOL lock)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        if(lock == XBH_TRUE)
        {
            if(mLockIIC[i] == -1)
            {
                mLockIIC[i] = iicBus;
            }
        }
        else
        {
            if(mLockIIC[i] == iicBus)
            {
                mLockIIC[i] = -1;
            }
        }
    }
    return  s32Ret;
}

XBH_S32 XbhHi_610::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        if (mLockIIC[i] == u32I2cNum)
        {
            return XBH_LOCKSTATUS;
        }
    }
    s32Ret = setChipI2cData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return  s32Ret;
}

XBH_S32 XbhHi_610::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        if (mLockIIC[i] == u32I2cNum)
        {
            return XBH_LOCKSTATUS;
        }
    }
    s32Ret = getChipI2cData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return  s32Ret;
}

XBH_S32 XbhHi_610::getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable)
{
    for(XBH_U8 i = 0; i < sizeof(source_available); i ++)
    {
        if((XBH_U8)src == source_available[i])
        {
            *enable = XBH_TRUE;
            return XBH_SUCCESS;
        }
    }
    *enable = XBH_FALSE;
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setDebugEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR ttyPath[64] = {0};
    XBH_S32 ttyfd,ret;

    sprintf(propVal, "%s", bEnable == XBH_TRUE ? "false" : "true");
    XLOGE(" ==== %s \n", propVal);
    property_set("persist.vendor.xbh.rs232.enable", propVal);
    if(bEnable)
    {
        //debug
        uapi_uart_switch(0);
        XLOGE("%s : debug\n",__func__);
    }
    else
    {
        //data
        uapi_uart_switch(1);
        XLOGE("%s : data\n",__func__);
    }
    return s32Ret;

}

XBH_S32 XbhHi_610::getDebugEnable(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.rs232.enable", propVal, "false") > 0)
    {
        if(strcmp(propVal, "false") == 0)
        {
            *bEnable = XBH_TRUE;
        } else {
            *bEnable = XBH_FALSE;
        }
    }
    return s32Ret;
}

XBH_S32 XbhHi_610::getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U32 value = 0;

    if (enChannel > XBH_ADC_CHANNEL_NUM_3)
    {
        return XBH_FAILURE;
    }

    ret = uapi_lsadc_get_value(enChannel, &value);
    if (ret == XBH_SUCCESS)
    {
        *u32Value = value;
    }
    return ret;
}

XBH_S32 XbhHi_610::getTemperatureSensorValue(XBH_FLOAT *fValue)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U32 value = 0;
    XBH_U32 adcValue   = 0;

    ret = getADCChannelValue(XBH_ADC_CHANNEL_NUM_3, &value);

    if (ret == XBH_SUCCESS)
    {
        adcValue = value;
    }

    //XLOGD("getBoardTemperature : adcValue = %d", adcValue);
    if ((1023 - adcValue) > 0)
    {
        XBH_U32 tAdcVal = XBH_BOARD_NTC_ADC_THERMISTOR_VAL * 1000 * adcValue / (1023 - adcValue);
        XBH_S32 tempIndex = 0;
        for(tempIndex=0; tempIndex < NTC_ADC_100K_0402_LEN; tempIndex++)
        {
            if(tAdcVal > NTC_ADC_100K_0402[tempIndex])
            {
               *fValue = tempIndex - 40;
                XLOGD("getBoardTemperature : ADC-CH%d = %d  temp = %f ", XBH_ADC_CHANNEL_NUM_3, tempIndex, *fValue);
                ret = XBH_SUCCESS;
                break;
            }
        }
        if(tempIndex >= NTC_ADC_100K_0402_LEN)
        {
            *fValue = 233;
            XLOGE("getBoardTemp: Invalid value = %d", adcValue);
        }
    }
    else
    {
        XLOGE("getBoardTemp: get adc failed!!! value = %d", adcValue);
        *fValue = 233;
        ret = XBH_FAILURE;
    }

    return ret;
}

XBH_S32 XbhHi_610::setRtcAlarmTime(XBH_RTC_ALARM_INFO_S *stRtcAlarmInfo)
{
    if(stRtcAlarmInfo == nullptr)
    {
        return XBH_FAILURE;
    }
    XBH_S32 s32Ret = XBH_SUCCESS;
    struct rtc_wkalrm alarm;
    struct tm tmpTm, tm, *gmtime_res;
    time_t sec;

    int fd = open(RTC_DEV_PATH, O_RDWR);
    if (fd < 0)
    {
        XLOGE("setRtcTime Unable to open %s: %s\n", RTC_DEV_PATH, strerror(errno));
        return XBH_FAILURE;
    }

    struct rtc_time rtc;
    memset(&rtc, 0, sizeof(rtc));
    s32Ret = ioctl(fd, RTC_RD_TIME, &rtc);
    if (s32Ret < 0)
    {
        XLOGE("RTC_GET_TIME ioctl failed: %s\n", strerror(errno));
        close(fd);
        return XBH_FAILURE;
    }
    XLOGD("RTC_GET_TIME : rtc.tm_sec: %d | rtc.tm_min: %d | rtc.tm_hour: %d | rtc.tm_mday: %d | rtc.tm_wday: %d | rtc.tm_mon: %d | rtc.tm_year: %d\n",
            rtc.tm_sec, rtc.tm_min, rtc.tm_hour, rtc.tm_mday, rtc.tm_wday, rtc.tm_mon, rtc.tm_year);

    memset(&alarm, 0, sizeof(alarm));
    alarm.time.tm_year = rtc.tm_year;
    alarm.time.tm_mon = rtc.tm_mon;
    alarm.time.tm_mday = stRtcAlarmInfo->u32RtcDate;
    alarm.time.tm_wday = stRtcAlarmInfo->u32RtcWeek;
    alarm.time.tm_hour = stRtcAlarmInfo->u32RtcHour;
    alarm.time.tm_min = stRtcAlarmInfo->u32RtcMinute;
    alarm.time.tm_sec = 0;
    alarm.time.tm_yday = 0;
    alarm.time.tm_isdst = -1;
    alarm.enabled = stRtcAlarmInfo->bEnable != 0;

    XLOGD("setRtcAlarmTime : rtc.tm_sec: %d | rtc.tm_min: %d | rtc.tm_hour: %d | rtc.tm_mday: %d | rtc.tm_wday: %d | rtc.tm_mon: %d | rtc.tm_year: %d\n",
                alarm.time.tm_sec, alarm.time.tm_min, alarm.time.tm_hour,
                alarm.time.tm_mday, alarm.time.tm_wday, alarm.time.tm_mon, alarm.time.tm_year);
    XLOGD("rtc.enable: %d\n", alarm.enabled);

    if(alarm.enabled)
    {
        s32Ret = ioctl(fd, RTC_WKALM_SET, &alarm);
        if(s32Ret < 0)
        {
            XLOGE("RTC_WKALM_SET ioctl failed: %s\n", strerror(errno));
            close(fd);
            return XBH_FAILURE;
        }
        XLOGD("RTC_AIE_ON RTC_AIE_ON \n");
        s32Ret = ioctl(fd, RTC_AIE_ON, 0);
        if(s32Ret < 0)
        {
            XLOGE("RTC_AIE_ON ioctl failed: %s\n", strerror(errno));
            close(fd);
            return XBH_FAILURE;
        }
    }
    else
    {
        XLOGD("RTC_AIE_OFF RTC_AIE_OFF \n");
        s32Ret = ioctl(fd, RTC_AIE_OFF, 0);
        if(s32Ret < 0)
        {
            XLOGE("RTC_AIE_OFF ioctl failed: %s\n", strerror(errno));
            close(fd);
            return XBH_FAILURE;
        }
    }

    close(fd);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::getRtcTime(XBH_RTC_INFO_S *stRtcInfo)
{
    if(stRtcInfo == nullptr)
    {
        return XBH_FAILURE;
    }

    int fd = open(RTC_DEV_PATH, O_RDONLY);
    std::shared_ptr<void> devPtr(nullptr,xbh_fd_closer(fd));
    if (fd < 0)
    {
        XLOGE("setRtcTime Unable to open %s: %s\n", RTC_DEV_PATH, strerror(errno));
        return XBH_FAILURE;
    }
    struct rtc_time rtc;
    memset(&rtc, 0, sizeof(rtc));
    auto s32Ret = ioctl(fd, RTC_RD_TIME, &rtc);
    if (s32Ret < 0)
    {
        XLOGE("RTC_GET_TIME ioctl failed: %s\n", strerror(errno));
        return XBH_FAILURE;
    }

    stRtcInfo->u32RtcSec    = rtc.tm_sec;
    stRtcInfo->u32RtcMinute = rtc.tm_min;
    stRtcInfo->u32RtcHour   = rtc.tm_hour;
    stRtcInfo->u32RtcDate   = rtc.tm_mday;
    stRtcInfo->u32RtcMonth  = rtc.tm_mon + 1;
    stRtcInfo->u32RtcYear   = rtc.tm_year + 1900;
    stRtcInfo->u32RtcWeek   = rtc.tm_wday;

    XLOGD("getRtcTime : rtc.tm_sec: %d | rtc.tm_min: %d | rtc.tm_hour: %d | rtc.tm_mday: %d | rtc.tm_wday: %d | rtc.tm_mon: %d | rtc.tm_year: %d\n",
            rtc.tm_sec, rtc.tm_min, rtc.tm_hour, rtc.tm_mday, rtc.tm_wday, rtc.tm_mon, rtc.tm_year);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setRtcTime(XBH_RTC_INFO_S *stRtcInfo)
{

    int fd = open(RTC_DEV_PATH, O_RDWR);
    std::shared_ptr<void> devPtr(nullptr,xbh_fd_closer(fd));
    if (fd < 0)
    {
        XLOGE("setRtcTime Unable to open %s: %s\n", RTC_DEV_PATH, strerror(errno));
        return XBH_FAILURE;
    }
    struct tm tmpTm;
    memset(&tmpTm,0,sizeof(tmpTm));
    tmpTm.tm_sec  = stRtcInfo->u32RtcSec;
    tmpTm.tm_min  = stRtcInfo->u32RtcMinute;
    tmpTm.tm_hour = stRtcInfo->u32RtcHour;
    tmpTm.tm_mday = stRtcInfo->u32RtcDate;
    tmpTm.tm_mon  = stRtcInfo->u32RtcMonth - 1;
    tmpTm.tm_year = stRtcInfo->u32RtcYear - 1900;
    tmpTm.tm_wday = stRtcInfo->u32RtcWeek;
    tmpTm.tm_yday = 0;
    tmpTm.tm_isdst = -1;

    auto eposec = mktime(&tmpTm);
    struct tm tm;
    auto gmtime_res = gmtime_r(&eposec, &tm);
    if (!gmtime_res)
    {
        XLOGD("gmtime_r() failed: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    struct rtc_time rtc;
    memset(&rtc, 0, sizeof(rtc));
    rtc.tm_year = tm.tm_year;
    rtc.tm_mon = tm.tm_mon;
    rtc.tm_mday = tm.tm_mday;
    rtc.tm_wday = tm.tm_wday;
    rtc.tm_hour = tm.tm_hour;
    rtc.tm_min = tm.tm_min;
    rtc.tm_sec = tm.tm_sec;
    rtc.tm_yday = tm.tm_yday;
    rtc.tm_isdst = tm.tm_isdst;

    auto s32Ret = ioctl(fd, RTC_SET_TIME, &rtc);
    if (s32Ret < 0)
    {
        XLOGE("RTC_SET_TIME ioctl failed: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    FILE * file = fdopen(fd, "w");
    if (!file)
    {
        XLOGE("setRtcTime flush error");
    }
    fflush(file);
    fsync(fd);
    fclose(file);
    XLOGD("setRtcTime : rtc.tm_sec: %d | rtc.tm_min: %d | rtc.tm_hour: %d | rtc.tm_mday: %d | rtc.tm_wday: %d | rtc.tm_mon: %d | rtc.tm_year: %d\n",
            rtc.tm_sec, rtc.tm_min, rtc.tm_hour, rtc.tm_mday, rtc.tm_wday, rtc.tm_mon, rtc.tm_year);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setBrightMax(XBH_U32 u32Value)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getBrightMax(XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::setBrightMin(XBH_U32 u32Value)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getBrightMin(XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getWoTStatus(XBH_WAKEUP_E enWakeUpType ,XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    ret = XbhPartitionData::getInstance()->getWotParamValue(s8Buff);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
        {
            *enable = s8Buff[XBH_WAKEUP_ETH];
            break;
        }
        case XBH_WAKEUP_SOURCE:
        {
            *enable = s8Buff[XBH_WAKEUP_SOURCE];
            break;
        }
        case XBH_WAKEUP_WIFI:
        {
            *enable = s8Buff[XBH_WAKEUP_WIFI];
            break;
        }
        case XBH_WAKEUP_NFC:
        {
            *enable = s8Buff[XBH_WAKEUP_NFC];
            break;
        }
        case XBH_WAKEUP_PIR:
        {
            *enable = s8Buff[XBH_WAKEUP_PIR];
            break;
        }
        default:
        {
            XLOGE("no type!");
            break;
        }
    }
    return ret;
}

XBH_S32 XbhHi_610::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue(s8Buff);
    XLOGD("setWoTStatus enWakeUpType=%d bEnable=%d ",enWakeUpType,bEnable);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
        {
            s8Buff[XBH_WAKEUP_ETH] = (bEnable ? 1 : 0);
            break;
        }
        case XBH_WAKEUP_SOURCE:
        {
            //是否独立开关控制TypeC待机唤醒:true表示独立开关，不受WOS开关控制;false表示WOS开关关联控制TypeC待机唤醒
            if(property_get("persist.vendor.xbh.indepen.source", propVal, "false") > 0)
            {
                if(strcmp(propVal, "false") == 0)
                {
                    s8Buff[XBH_WAKEUP_FTYPEC] = (bEnable ? 1 : 0);
                    s8Buff[XBH_WAKEUP_BTYPEC] = (bEnable ? 1 : 0);
                }
                else if(strcmp(propVal, "true") == 0)
                {
                    s8Buff[XBH_WAKEUP_FTYPEC] = 0;
                    s8Buff[XBH_WAKEUP_BTYPEC] = 0;
                }
            }
         
            s8Buff[XBH_WAKEUP_SOURCE] = (bEnable ? 1 : 0);
            break;
        }
        case XBH_WAKEUP_WIFI:
        {
            s8Buff[XBH_WAKEUP_WIFI] = (bEnable ? 1 : 0);
            break;
        }
        case XBH_WAKEUP_PIR:
        {
            s8Buff[XBH_WAKEUP_PIR] = (bEnable ? 1 : 0);
            break;
        }
        case XBH_WAKEUP_NFC:
        {
            s8Buff[XBH_WAKEUP_NFC] = (bEnable ? 1 : 0);
            break;
        }
        case XBH_WAKEUP_CEC:
        {
            s8Buff[XBH_WAKEUP_CEC] = (bEnable ? 1 : 0);
            break;
        }
        default:
        {
            XLOGE("no type!");
            break;
        }
    }
    s32Ret = XbhPartitionData::getInstance()->setWotParamValue(s8Buff);
    return s32Ret;
}

XBH_S32 XbhHi_610::setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    if (type > 0)
    {
        s8Buff[0] = 2;
    }
    else
    {
        s8Buff[0] = 1;
    }
    //s32Ret = XbhPartitionData::getInstance()->setMultiPanel((XBH_VOID *)s8Buff);
    XLOGD("set MultiPanel %d \n", s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhHi_610::getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    //s32Ret = XbhPartitionData::getInstance()->getMultiPanel((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] == 2 ? 1 : 0;
    return s32Ret;
}

XBH_S32 XbhHi_610::setPanelMapSwap(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    s8Buff[0] = type;
    //s32Ret = XbhPartitionData::getInstance()->setPanelMapSwap((XBH_VOID *)s8Buff);
    XLOGD("set MultiPanel %d \n", s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhHi_610::getPanelMapSwap(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    //s32Ret = XbhPartitionData::getInstance()->getPanelMapSwap((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] ;
    return s32Ret;
}

XBH_S32 XbhHi_610::setUsbcConfig(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_USBC_CONFIG_LEN] = {0};
    s8Buff[0] = type;
    XLOGD("setUsbcConfig %d \n", s8Buff[0]);
    //s32Ret = XbhPartitionData::getInstance()->setUsbcConfig((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhHi_610::getUsbcConfig(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_USBC_CONFIG_LEN] = {0};
    //s32Ret = XbhPartitionData::getInstance()->getUsbcConfig((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] ;
     XLOGD("getUsbcConfig %d \n", *type);
    return s32Ret;
}


XBH_S32 XbhHi_610::setIndependenceSourceEdidType(XBH_U32 source,XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN] = {0};
    s8Buff[0]= (XBH_CHAR)type;
    //s32Ret = XbhPartitionData::getInstance()->setIndependenceSourceParamValue(source,(XBH_VOID *)s8Buff);
    //XLOGD("set setHdmirxEdidType %d \n",s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhHi_610::getIndependenceSourceEdidType(XBH_U32 source,XBH_S32 *type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN] = {0};
    //s32Ret = XbhPartitionData::getInstance()->getIndependenceSourceParamValue(source,(XBH_VOID *)s8Buff);
    if (s32Ret != XBH_SUCCESS || (s8Buff[0] == 0))
    {
        // 当getIndependenceSourceParamValue分区值为0时,代表为空,则默认使用EDID 2.0
        *type = 2;
        setIndependenceSourceEdidType(source,*type);
        XLOGD("set defult HdmirxEdidType: %d \n",*type);
        return s32Ret;
    }
    if(s8Buff[0] == 1)
    {
        *type = 1;              // EDID 1.4
    }
    else if(s8Buff[0] == 2)
    {
        *type = 2;              // EDID 2.0
    }
    else if(s8Buff[0] == 3)
    {
        *type = 3;              // EDID 2.1
    }
    else if(s8Buff[0] == 4)
    {
        *type = 4;              // 自动模式
    }
    XLOGD("get HdmirxEdidType %d \n",s8Buff[0]);

    return s32Ret;
}

XBH_S32 XbhHi_610::setHdmirxEdidType(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN] = {0};
    if (type == 1)
    {
        s8Buff[0] = 1;              // EDID 1.4
    }
    else if(type == 2)
    {
        s8Buff[0] = 2;              // EDID 2.0
    }
    else if(type == 3)
    {
        s8Buff[0] = 3;              // EDID 2.1
    }
    else if(type == 4)
    {
        s8Buff[0] = 4;              // 自动模式
    }
    //s32Ret = XbhPartitionData::getInstance()->setHdmiEdidParamValue((XBH_VOID *)s8Buff);
    XLOGD("set setHdmirxEdidType %d \n",s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhHi_610::getHdmirxEdidType(XBH_S32 *type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN] = {0};
    //s32Ret = XbhPartitionData::getInstance()->getHdmiEdidParamValue((XBH_VOID *)s8Buff);
    if (s32Ret != XBH_SUCCESS || (s8Buff[0] == 0)) 
    {
        // 当getHdmiEdidParamValue分区值为0时,代表为空,则默认使用EDID 2.0
        *type = 2;
        setHdmirxEdidType(*type);
        XLOGD("set defult HdmirxEdidType: %d \n",*type);
        return s32Ret;
    }
    if(s8Buff[0] == 1)
    {
        *type = 1;              // EDID 1.4
    }
    else if(s8Buff[0] == 2)
    {
        *type = 2;              // EDID 2.0
    }
    else if(s8Buff[0] == 3)
    {
        *type = 3;              // EDID 2.1
    }
    else if(s8Buff[0] == 4)
    {
        *type = 4;              // 自动模式
    }
    XLOGD("get HdmirxEdidType %d \n",s8Buff[0]);
    return s32Ret;
}


XBH_S32 XbhHi_610::getSourceEdidType(XBH_U32 u32SourceId, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getSourceEdidType no imp! \n");
    return s32Ret;
}

XBH_S32 XbhHi_610::setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setSourceEdidType no imp! \n");
    return s32Ret;
}


XBH_S32 XbhHi_610::isNfcExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->getChipExist(enable);
    XLOGW("isNfcExist s32Ret = %d", s32Ret);
   return s32Ret;
}

XBH_S32 XbhHi_610::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->setNfcEnable(enable);
    XLOGW("setNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->getNfcEnable(enable);
    XLOGW("getNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->getNfcId(data, length);
    XLOGW("getNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->addNfcId(data, length);
    XLOGW("addNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->delNfcId(data, length);
    XLOGW("delNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhNfcManager::getInstance()->delAllNfcId();
    XLOGW("delAllNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}
XBH_S32 XbhHi_610::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuFattMode(mode);
    XLOGW("setMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuFattMode(mode);
    XLOGW("getMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhHi_610::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhHi_610::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuUartBypassPara(u8UartNum, baud);
    XLOGW("setMcuUartBypassPara s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuUartBypass(u8Len, u8Data);
    XLOGW("setMcuUartBypass s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuUartBypass(u8Len, u8Data);
    XLOGW("getMcuUartBypass s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhHi_610::saveFattKeypadInfo(XBH_S32 keypad_enable, XBH_S32 key_count, XBH_S32 key_index)
{
    mKeypadEnable = keypad_enable;
    mKeypadCount = key_count;
    mKeypadIndex = key_index;
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setFattKeypadenbale(XBH_S32 keypad_enable)
{
    //XbhKernelMsgManager::getInstance()->sendMsgToKernel(1, keypad_enable, 0, 0, 0);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::getFattKeypadInfo(XBH_S32* keypad_enable, XBH_S32* key_count, XBH_S32* key_index)
{
    *keypad_enable = mKeypadEnable;
    *key_count = mKeypadCount;
    *key_index = mKeypadIndex;
    XLOGW("getFattKeypadInfo %d :%d :%d", *keypad_enable, *key_count, *key_index);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    if (stWake == NULL)
    {
        return XBH_FAILURE;
    }
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[10] = {0};

    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_INFO, 0x01, 10, pu8Data);

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

XBH_S32 XbhHi_610::setShutdownFlag(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_SHUTDOWN_FLAG_LEN] = {0};
    XLOGD("setShutdownFlag : %d\n", enable);
    s8Buff[0] = (XBH_CHAR)enable;
    //s32Ret = XbhPartitionData::getInstance()->setShutdownFlagParamValue((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhHi_610::setPoweronMode(XBH_POWERMODE_E enMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_POWER_ON_MODE_LEN] = {0};
    XLOGD("setPoweronMode : %d\n", enMode);
    s8Buff[0] = (XBH_CHAR)enMode;
    //s32Ret = XbhPartitionData::getInstance()->setPowerModeParamValue((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhHi_610::getPoweronMode(XBH_POWERMODE_E *enMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_POWER_ON_MODE_LEN] = {0};
    //s32Ret = XbhPartitionData::getInstance()->getPowerModeParamValue((XBH_VOID *)s8Buff);
    *enMode = (XBH_POWERMODE_E)s8Buff[0];
    XLOGD("getPoweronMode : %d\n", *enMode);
    return s32Ret;
}

XBH_S32 XbhHi_610::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

#if 0
    switch (enPanel) {
        case XBH_PANEL_MAIN:
            #if (XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_MAIN_BL_ON_PWR > 0)
            if (bEnable) {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_BL_ON_PWR_ON : XBH_BOARD_GPIO_MAIN_BL_ON_PWR_OFF);
            } else {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_BL_ON_PWR_ON : XBH_BOARD_GPIO_MAIN_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_1:
            #if (XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_1_BL_ON_PWR > 0)
            if (bEnable) {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_OFF);
            } else {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_2:
            #if (XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_2_BL_ON_PWR > 0)
            if (bEnable) {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_OFF);
            } else {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_3:
            #if (XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_3_BL_ON_PWR > 0)
            if (bEnable) {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_OFF);
            } else {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_4:
            #if (XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_4_BL_ON_PWR > 0)
            if (bEnable) {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_4_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_4_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_4_BL_ON_PWR_OFF);
            } else {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_4_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_4_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_4_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        default:
            XLOGE("XBH_PANEL_NUM_E enPanel is not exist \n");
            return XBH_FAILURE;
    }
#endif

    if(s32Ret < 0){
        XLOGE("-- set PanelPowerGpio fail --\n");
    }

    return s32Ret;
}

XBH_S32 XbhHi_610::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;

#if 0
    switch (enPanel) {
        case XBH_PANEL_MAIN:
            #if (XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR > 0)
                s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR, &u32GpioValue);
                *bEnable =  u32GpioValue == XBH_BOARD_GPIO_MAIN_LCD_TCON_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
            #endif
            break;

        case XBH_PANEL_SUB_1:
            #if (XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR > 0)
                s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR, &u32GpioValue);
                *bEnable =  u32GpioValue == XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
            #endif
            break;

        case XBH_PANEL_SUB_2:
            #if (XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR > 0)
                s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR, &u32GpioValue);
                *bEnable =  u32GpioValue == XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
            #endif
            break;

        case XBH_PANEL_SUB_3:
            #if (XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR > 0)
                s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR, &u32GpioValue);
                *bEnable =  u32GpioValue == XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
            #endif
            break;

        case XBH_PANEL_SUB_4:
            #if (XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR > 0)
                s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR, &u32GpioValue);
                *bEnable =  u32GpioValue == XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
            #endif
            break;

        default:
            XLOGE("XBH_PANEL_NUM_E enPanel is not exist \n");
            return XBH_FAILURE;
    }
#endif

    if(s32Ret < 0){
        XLOGE("-- get PanelPowerGpio fail --\n");
    }
    return s32Ret;
}

XBH_S32 XbhHi_610::dumpEdid(XBH_SOURCE_E idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::getSocHdmiEdidStatus(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(XbhService::getModuleInterface()->mIsMiddlewareCompleted == 1)
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    return s32Ret;
}
//----------------------------------------------------------------------------- private func start here -----------------------------------------------------------------
XBH_S32 XbhHi_610::updateHdmiRxHdcp(XBH_S32 type){
    XBH_S32 s32Ret;
    XBH_U8 *key_14,*key_22;
    uapi_hdmirx_hdcp hdcp_14,hdcp_22;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4){
        key_14 = (XBH_U8 *)malloc(XBH_H610_CUSDATA_HDCP_KEY_14_LEN);
        getHdcpKey(XBH_HDCP_TYPE_E::XBH_HDCP1_4,(XBH_CHAR *)key_14);
        hdcp_14.hdcp_data = key_14;
        hdcp_14.hdcp_length = XBH_H610_CUSDATA_HDCP_KEY_14_LEN;
        for (XBH_S32 i = UAPI_HDMIRX_PORT0; i < UAPI_HDMIRX_PORT_MAX; i++) {
            s32Ret = uapi_hdmirx_load_hdcp((uapi_hdmirx_port)i, UAPI_HDMIRX_HDCP_VER_1X, &hdcp_14);
            XLOGW("uapi_hdmirx_load_hdcp enHdmiType = %d, ret:0x%x", UAPI_HDMIRX_HDCP_VER_1X, s32Ret);
        }
        if(s32Ret != XBH_SUCCESS){
            XLOGW("load hdcp14 key error,ret:0x%x",s32Ret);
        }
        free(key_14);
        XLOGW("update hdcp14 key");
    }
    if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2) 
        ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3)){
        key_22 = (XBH_U8 *)malloc(XBH_H610_CUSDATA_HDCP_KEY_22_LEN);
        getHdcpKey(XBH_HDCP_TYPE_E::XBH_HDCP2_2,(XBH_CHAR *)key_22);
        hdcp_22.hdcp_data = key_22;
        hdcp_22.hdcp_length = XBH_H610_CUSDATA_HDCP_KEY_22_LEN;
        for (XBH_S32 i = UAPI_HDMIRX_PORT0; i < UAPI_HDMIRX_PORT_MAX; i++) {
            s32Ret = uapi_hdmirx_load_hdcp((uapi_hdmirx_port)i, UAPI_HDMIRX_HDCP_VER_2X, &hdcp_22);
            XLOGW("uapi_hdmirx_load_hdcp enHdmiType = %d, ret:0x%x", UAPI_HDMIRX_HDCP_VER_2X, s32Ret);
        }
        if(s32Ret != XBH_SUCCESS){
            XLOGW("load hdcp22 key error,ret:0x%x",s32Ret);
        }
        free(key_22);
        XLOGW("load hdcp22 key");
    }
    return s32Ret;
}

XBH_S32 XbhHi_610::setChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uapi_i2c_addr_info i2c_addr_info;

    if (u32RegAddrCount <= 0 || u32RegAddrCount > 4) {                                /* 4 byte */
        XLOGE("i2c reg address length must less than 5.\n");
        return XBH_FAILURE;
    }

    if(u32Length > EXT_I2C_MAX_LENGTH) {
        XLOGE("i2c write data length must less than %d.\n", EXT_I2C_MAX_LENGTH);
        return XBH_FAILURE;
    }

    //XLOGV("%s: i2cNum=0x%x,devAddress=0x%x,regAddrValue=0x%x,regAddrCount=0x%x,length=0x%x\n",
    //             __func__,(S32)u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length);
    
    if(!m_uapi_i2c_init_ok)
    {
        s32Ret = uapi_i2c_init();
        if (s32Ret != TD_SUCCESS)
        {
            XLOGE("uapi_i2c_init fail, ErrorCode=0x%x\n", s32Ret);
            return XBH_FAILURE;
        }
        m_uapi_i2c_init_ok = XBH_TRUE;
    }

    i2c_addr_info.dev_address = u8DevAddress << 1;
    // cut the reg addr by u32RegAddrCount
    i2c_addr_info.reg_addr = u32RegAddr & (0xFFFFFFFF >> (8 * (4 - u32RegAddrCount)));
    i2c_addr_info.reg_addr_count = u32RegAddrCount;
   
    /* write data to device */
    s32Ret = uapi_i2c_write(u32I2cNum, &i2c_addr_info, (XBH_U8*) u8Data, u32Length);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("I2cNum : %d - 0x%x write reg 0x%x = 0x%x ErrorCode=0x%x\r\n",
            u32I2cNum, u8DevAddress, u32RegAddr, *u8Data, s32Ret);
        return XBH_FAILURE;
    }

    return  XBH_SUCCESS;
}

XBH_S32 XbhHi_610::getChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uapi_i2c_addr_info i2c_addr_info;

    if (u32RegAddrCount <= 0 || u32RegAddrCount > 4) {                                /* 4 byte */
        XLOGE("i2c reg address length must less than 5.\n");
        return XBH_FAILURE;
    }

    if(u32Length > EXT_I2C_MAX_LENGTH) {
        XLOGE("i2c write data length must less than %d.\n", EXT_I2C_MAX_LENGTH);
        return XBH_FAILURE;
    }

    //XLOGV("i2cNum=0x%x,devAddress=0x%x,regAddrValue=0x%x,regAddrCount=0x%x,length=0x%x\n",
    //             (S32)u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length);
    
    if(!m_uapi_i2c_init_ok) {
        s32Ret = uapi_i2c_init();
        if (s32Ret != TD_SUCCESS)
        {
            XLOGE("uapi_i2c_init fail, ErrorCode=0x%x\n", s32Ret);
            return XBH_FAILURE;
        }
        m_uapi_i2c_init_ok = XBH_TRUE;
    }

    i2c_addr_info.dev_address = u8DevAddress << 1;
    // cut the reg addr by u32RegAddrCount
    i2c_addr_info.reg_addr = u32RegAddr & (0xFFFFFFFF >> (8 * (4 - u32RegAddrCount)));
    i2c_addr_info.reg_addr_count = u32RegAddrCount;
   
    /* write data to device */
    s32Ret = uapi_i2c_read(u32I2cNum, &i2c_addr_info, (XBH_U8*) u8Data, u32Length);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("I2cNum : %d - 0x%x read reg 0x%x ErrorCode=0x%x\r\n",
            u32I2cNum, u8DevAddress, u32RegAddr, s32Ret);
        return XBH_FAILURE;
    }

    return  XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_MCU_GPIOPARA_U unGpioGet;
    XBH_MCU_I2CBUFFDEF_S stInData;
    if ((0xff == u8Group) || (0xff == u8GpioNum))
    {
        XLOGE("-- mcu gpio port and pin error \n");
        return XBH_FAILURE;
    }

    XbhMutex::Autolock _l(m_McuLock);
    unGpioGet.attr.port = u8Group;
    unGpioGet.attr.pin = u8GpioNum;
    unGpioGet.attr.level = level;
    unGpioGet.attr.mode = GPIO_MODE_OUT_PP;
    unGpioGet.attr.speed = GPIO_OSPEED_10MHZ;
    unGpioGet.attr.reserved = 0x00;
    stInData.cmd = CMD_I2C_SET_GPIO_OUTPUT;
    stInData.len = sizeof(XBH_MCU_GPIOPARA_U);
    stInData.data[0]  = (unGpioGet.value >> 24) & 0xFF;
    stInData.data[1]  = (unGpioGet.value >> 16) & 0xFF;
    stInData.data[2]  = (unGpioGet.value >> 8) & 0xFF;
    stInData.data[3]  = unGpioGet.value & 0xFF;
#if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    ret = setChipI2cData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    //XLOGD("--setChipI2cData cmd: %02x, len: %d, data %02x:%02x:%02x:%02x \n", stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
    if (ret != XBH_SUCCESS)
    {
        XLOGE("Error! setChipI2cData ret: %d, cmd: %02x, len: %d, data %02x:%02x:%02x:%02x \n", ret, stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
    }
#endif
    return ret;
}

XBH_S32 XbhHi_610::getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_MCU_GPIOPARA_U unGpioGet;
    XBH_MCU_I2CBUFFDEF_S stInData;
    if ((0xff == u8Group) || (0xff == u8GpioNum))
    {
        XLOGE("-- mcu gpio port and pin error \n");
        return XBH_FAILURE;
    }
    XbhMutex::Autolock _l(m_McuLock);
    unGpioGet.attr.port = u8Group;
    unGpioGet.attr.pin = u8GpioNum;
    unGpioGet.attr.mode = (mode == MCU_GPIO_MODE_OUTPUT ? GPIO_MODE_OUT_PP : GPIO_MODE_IN_FLOATING);
    unGpioGet.attr.speed = GPIO_OSPEED_10MHZ;
    unGpioGet.attr.reserved = 0x00;
    stInData.cmd = (mode == MCU_GPIO_MODE_OUTPUT ? CMD_I2C_GET_GPIO_OUTPUT: CMD_I2C_GET_GPIO_INPUT);
    stInData.len = sizeof(XBH_MCU_GPIOPARA_U);
    stInData.data[0]  = (unGpioGet.value >> 24) & 0xFF;
    stInData.data[1]  = (unGpioGet.value >> 16) & 0xFF;
    stInData.data[2]  = (unGpioGet.value >> 8) & 0xFF;
    stInData.data[3]  = unGpioGet.value & 0xFF;
#if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    ret = setChipI2cData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    //XLOGD("--setChipI2cData cmd: %02x, len: %d, data %02x:%02x:%02x:%02x ,level:%d\n", stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3],unGpioGet.attr.level);
    if (ret != XBH_SUCCESS)
    {
        XLOGE("Error! setChipI2cData ret: %d, cmd: %02x, len: %d, data %02x:%02x:%02x:%02x \n", ret, stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
    }
    ret |= getChipI2cData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    if (ret == XBH_SUCCESS)
    {
        unGpioGet.value = stInData.data[0] << 24;
        unGpioGet.value |= stInData.data[1] << 16;
        unGpioGet.value |= stInData.data[2] << 8;
        unGpioGet.value |= stInData.data[3];
        *level = unGpioGet.attr.level;
    }
    else
    {
        XLOGE("Error! getChipI2cData ret: %d, cmd: %02x, len: %d, data %02x:%02x:%02x:%02x\n", ret, stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
    }
#endif
    //XLOGD("--getMcuGpioValue port: %d, pin: %d, level:%d\n", unGpioGet.attr.port ,unGpioGet.attr.pin ,unGpioGet.attr.level);
    return ret;
}

XBH_S32 XbhHi_610::initSerialPort()
{
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.rs232.enable", propVal, "false") > 0)
    {
        if(strcmp(propVal, "true") == 0)
        {
            //setDebugEnable(XBH_FALSE);
        }
        else
        {
            //setDebugEnable(XBH_TRUE);
        }
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}

XBH_S32 XbhHi_610::setGpioDirOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_UNUSED(u32GpioValue);

    //XLOGE(">> setGpioDirOutput(gpio_num=%d)\n", u32GpioNumber);
    if (!m_uapi_gpio_init_ok)
    {
        s32Ret = uapi_gpio_init();
        if (s32Ret != TD_SUCCESS)
        {
            XLOGE("uapi_gpio_init ErrorCode=0x%x\n", s32Ret);
            return XBH_FAILURE;
        }
        m_uapi_gpio_init_ok = XBH_TRUE;
    }

    s32Ret = uapi_gpio_set_direction(u32GpioNumber / 8, u32GpioNumber % 8, TD_FALSE);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("uapi_gpio_set_direction ErrorCode=0x%x, gpionum %d\n", s32Ret, u32GpioNumber);
        return XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhHi_610::setGpioDirInput(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    //XLOGE(">> setGpioDirInput(gpio_num=%d)\n", u32GpioNumber);
    if (!m_uapi_gpio_init_ok)
    {
        s32Ret = uapi_gpio_init();
        if (s32Ret != TD_SUCCESS)
        {
            XLOGE("uapi_gpio_init ErrorCode=0x%x\n", s32Ret);
            return XBH_FAILURE;
        }
        m_uapi_gpio_init_ok = XBH_TRUE;
    }
    s32Ret = uapi_gpio_set_direction(u32GpioNumber / 8, u32GpioNumber % 8, TD_TRUE);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("uapi_gpio_set_direction ErrorCode=0x%x, gpionum %d\n", s32Ret, u32GpioNumber);
        return XBH_FAILURE;
    }

    return s32Ret;
}

/*
此接口总是读取错误,错误码:0x80470005
XBH_S32 XbhHi_610::getGpioDir(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    td_bool *input;

    s32Ret = uapi_gpio_init();
    if (s32Ret != TD_SUCCESS) {
        XLOGE("uapi_gpio_init ErrorCode=0x%x\n", s32Ret);
        return XBH_FAILURE;
    }

    s32Ret = uapi_gpio_get_direction(u32GpioNumber / 8, u32GpioNumber % 8, input);
    if (s32Ret != TD_SUCCESS) {
        XLOGE("uapi_gpio_get_direction ErrorCode=0x%x, gpionum %d\n", s32Ret, u32GpioNumber);
        return XBH_FAILURE;
    }

    return (*input? XBH_BOARD_GPIO_INPUT: XBH_BOARD_GPIO_OUTPUT);
}
*/

XBH_S32 XbhHi_610::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhHi_610::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}


XBH_S32 XbhHi_610::loadProperty()
{
    int entryCount = 0;
    XBH_S32 flag = 0;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XLOGD("%s %d loadProperty start !!!! \n", __func__, __LINE__);
    IniEntry* entries = XbhParseINI::parseINI("/mnt/xbhcust/systemproperty.ini", &entryCount);
    for (int i = 0; i < entryCount; i++) {
        XLOGD("LYF==== %s = %s\n\n", entries[i].key, entries[i].value);
        if(strcmp("persist.vendor.xbh.customerversion", entries[i].key) == 0)
        {
            if(property_get("persist.vendor.xbh.customerversion", propVal, "0000") > 0)
            {
                if(strcmp(propVal, entries[i].value) == 0)
                {
                    flag = 1;
                    XLOGD("LYF==== cust project as same as old !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n\n");
                    return flag;
                }
            }
        }
        property_set(entries[i].key, entries[i].value);
    }
    //如果解析成功,释放entries指向的内存,避免内存泄漏
    if(entries != NULL)
    {
        free(entries);
    }
    return flag;
}

XBH_S32 XbhHi_610::setDeviceId(const XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->setDeviceId(strDeviceId);
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

XBH_S32 XbhHi_610::getDeviceId(XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->getDeviceId(strDeviceId);
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

//Set serial number
XBH_S32 XbhHi_610::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSn(strSn);
    return  s32Ret;
}

//Get serial number
XBH_S32 XbhHi_610::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSn(strSn);
    return  s32Ret;
}

//override
XBH_S32 XbhHi_610::setCustProductInfo(const XBH_CHAR* pBUff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setCustProductInfo(pBUff);
    return s32Ret;
}
//override
XBH_S32 XbhHi_610::getCustProductInfo(XBH_CHAR* pBUff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getCustProductInfo(pBUff);
    return s32Ret;
}

//Set Chip RunTime
XBH_S32 XbhHi_610::setChipRunTime(XBH_S32 time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setChipRunTime(time);
    return  s32Ret;
}

//Get Chip RunTime
XBH_S32 XbhHi_610::getChipRunTime(XBH_S32* time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getChipRunTime(time);
    return  s32Ret;
}

//set ethernet mac address
XBH_S32 XbhHi_610::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (macType == XBH_MACTYPE_RNDIS)   //RNDIS
    {
        s32Ret = XbhPartitionData::getInstance()->saveFactoryMac(XBH_RNDIS, strMacAddress);
    }
    //s32Ret  = XbhPartitionData::getInstance()->setMacAddress(strMacAddress);
    return  s32Ret;
}

//get ethernet mac address
XBH_S32 XbhHi_610::getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (macType == XBH_MACTYPE_RNDIS)   //RNDIS
    {
        s32Ret = XbhPartitionData::getInstance()->getFactoryMac(XBH_RNDIS, strMacAddress);
    }
    //s32Ret  = XbhPartitionData::getInstance()->getMacAddress(strMacAddress);
    return  s32Ret;
}

XBH_S32 XbhHi_610::setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = -1;
    XBH_CHAR *buf = NULL;
    XBH_S32 BUF_LEN = 2048;
    XBH_S32 SIZE = 256;
    XLOGD("%s,%d,pBuff:%s,type:%d\n", __FUNCTION__,__LINE__,pBuff,(int)type);
    fd = open(pBuff, O_RDWR);
    if (fd < 0) {
        XLOGE("[ERROR] Fail to open %s!\n", pBuff);
        return XBH_FAILURE;
    }
    buf = (XBH_CHAR *)malloc(BUF_LEN);
    if (buf == NULL) {
        XLOGE("[ERROR] malloc failed!\n");
        close(fd);
        fd = -1;
        return XBH_FAILURE;
    }
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4){
        SIZE = XBH_H610_CUSDATA_HDCP_KEY_14_LEN;
    }else if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
             ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3)){
        SIZE = XBH_H610_CUSDATA_HDCP_KEY_22_LEN;
    }else{
        XLOGE("[ERROR] type error!\n");
        close(fd);
        fd = -1;
        return XBH_FAILURE;
    }
    s32Ret = read(fd, buf, SIZE);
    if (s32Ret != SIZE) {
        XLOGE("[ERROR] read failed!\n");
        free(buf);
        close(fd);
        fd = -1;
        return XBH_FAILURE;
    }
    // for(int i = 0;i<SIZE;i++){
    //     XLOGW("setHdcpKey[%d]:0x%x \n",i,buf[i]);
    // }
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4){
        s32Ret = XbhPartitionData::getInstance()->writeDataToCusdata(XBH_H610_CUSDATA_HDCP_KEY_14_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_14_LEN, buf);
    }else if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
            ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3)){
        s32Ret = XbhPartitionData::getInstance()->writeDataToCusdata(XBH_H610_CUSDATA_HDCP_KEY_22_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_22_LEN, buf);
    }else{
        XLOGE("[ERROR] type error!\n");
    }
    if(s32Ret == XBH_SUCCESS){
        s32Ret = updateHdmiRxHdcp(type);
    }
    free(buf);
    close(fd);
    return s32Ret;
}

XBH_S32 XbhHi_610::getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable)
{
    XBH_U8 key[XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN] = {0};
    XBH_U8 cusDataKey[XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN] = {0};
    XBH_S32 keyLen = XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN;
    XBH_S32 err = 0;
    XBH_S32 i = 0;

    *bEnable = XBH_FALSE;
    XLOGD("%s,%d\n", __FUNCTION__,__LINE__);
    if(type != XBH_HDCP1_4)
    {
        XLOGE("only dp-tx hdcp 1.4 key can be get\n");
        return XBH_FAILURE;
    }

    //TODO: 读取烧写的key
    err = -1;//hdcp_key_read(hdcp_key_id, key, &keyLen);
    if(err)
    {
        XLOGE("hdcp_key_read fail\n");
        return XBH_FAILURE;
    }

    //读取我们保存的key
    //err = XbhPartitionData::getInstance()->getHDCPKey_14(cusDataKey);
    if(err)
    {
        XLOGE("cusdata read hdcp_key fail\n");
        return XBH_FAILURE;
    }

    //比较
    for(i = 0; i < keyLen; i++)
    {
        if(key[i] != cusDataKey[i])
        {
            XLOGE("the key is different!!!\n");
            return XBH_FAILURE;
        }
    }
    *bEnable = XBH_TRUE;
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 SIZE = 256;
    XLOGD("%s,%d,type:%d\n", __FUNCTION__,__LINE__,(int)type);
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4){
        SIZE = XBH_H610_CUSDATA_HDCP_KEY_14_LEN;
    }else if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
             ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3)){
        SIZE = XBH_H610_CUSDATA_HDCP_KEY_22_LEN;
    }else{
        XLOGE("[ERROR] type error!\n");
        return XBH_FAILURE;
    }
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret  = XbhPartitionData::getInstance()->readDataFromCusdata(XBH_H610_CUSDATA_HDCP_KEY_14_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_14_LEN, (XBH_VOID *)pBuff);
    }
    else if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
            ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3))
    {
        s32Ret  = XbhPartitionData::getInstance()->readDataFromCusdata(XBH_H610_CUSDATA_HDCP_KEY_22_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_22_LEN, (XBH_VOID *)pBuff);
    }
    // for(int i = 0;i<SIZE;i++){
    //     XLOGW("getHdcpKey[%d]:0x%x \n",i,pBuff[i]);
    // }
    return  s32Ret;
}

XBH_S32 XbhHi_610::getHdcpKeydata(XBH_SOURCE_E source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
XBH_S32 XbhHi_610::setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("%s,%d,strHdcpFileName:%s,type:%d\n", __FUNCTION__,__LINE__,strHdcpFileName,(int)type);
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4){
        s32Ret = XbhPartitionData::getInstance()->writeDataToCusdata(XBH_H610_CUSDATA_HDCP_KEY_14_NAME_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_14_NAME_LEN, strHdcpFileName);
    }else if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
            ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3)){
        s32Ret = XbhPartitionData::getInstance()->writeDataToCusdata(XBH_H610_CUSDATA_HDCP_KEY_22_NAME_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_22_NAME_LEN, strHdcpFileName);
    }else{
        XLOGE("[ERROR] type error!\n");
    }
    return  s32Ret;
}

XBH_S32 XbhHi_610::getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret  = XbhPartitionData::getInstance()->readDataFromCusdata(XBH_H610_CUSDATA_HDCP_KEY_14_NAME_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_14_NAME_LEN, (XBH_VOID *)strHdcpFileName);
    }
    else if((type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
            ||(type == XBH_HDCP_TYPE_E::XBH_HDCP2_3))
    {
        s32Ret  = XbhPartitionData::getInstance()->readDataFromCusdata(XBH_H610_CUSDATA_HDCP_KEY_22_NAME_OFFSET, XBH_H610_CUSDATA_HDCP_KEY_22_NAME_LEN, (XBH_VOID *)strHdcpFileName);
    }else{
        XLOGE("[ERROR] type error!\n");
    }
    XLOGD("%s,%d,strHdcpFileName:%s,type:%d\n", __FUNCTION__,__LINE__,strHdcpFileName,(int)type);
    return  s32Ret;
}

XBH_S32 XbhHi_610::setGammaGroup(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_BOE_GAMMA_VALUE_LEN] = {0};
    s8Buff[0] = s32Value;
    XLOGD("setGammaGroup %d \n", s8Buff[0]);
    
    s32Ret = uapi_pq_set_table(UAPI_DISPLAY1, UAPI_PQ_IMAGE_ALL, UAPI_PQ_TABLE_MODULE_GAMMA, s32Value);
    if (s32Ret != TD_SUCCESS) 
    {
        XLOGD("current s32Value:%d not support, set s32Value:0", s32Value);
    } 
    else 
    {
        XbhPartitionData::getInstance()->setGammaGroup((XBH_VOID *)s8Buff);
    }

    return s32Ret;
}

XBH_S32 XbhHi_610::getGammaGroup(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_BOE_GAMMA_VALUE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getGammaGroup((XBH_VOID *)s8Buff);
    *s32Value =  s8Buff[0] ;
    XLOGD("getGammaGroup %d \n", *s32Value);
    return s32Ret;
}

XBH_S32 XbhHi_610::setPictureMode(XBH_PICMODE_E enPicMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::getPictureMode(XBH_PICMODE_E* enPicMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::setColorTempMode(XBH_COLORTEMP_E enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::getColorTempMode(XBH_COLORTEMP_E *enColorTemp)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::setBrightness(XBH_U32 u32Value) {
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::getBrightness(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::setContrast(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::getContrast(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::setSaturation(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::getSaturation(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::setSharpEnable(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::setSharpness(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::getSharpness(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::setHue(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

// range[0-100]
XBH_S32 XbhHi_610::getHue(XBH_U32* u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::setHDMIColorRange(XBH_COLOR_RANGE_E enColorRange)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::getHDMIColorRange(XBH_COLOR_RANGE_E* enColorRange)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

/**
 * backlight value, range[0-100]
 */
XBH_S32 XbhHi_610::setBacklightWithOutSave(XBH_U32 value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

/**
 * backlight value, range[0-100]
 */
XBH_S32 XbhHi_610::getBacklightWithOutSave(XBH_U32* value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhHi_610::getSignalStatus(XBH_SOURCE_E currSource,XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_CHAR* hdmirx_path = nullptr;
    switch(currSource) {
        case XBH_SOURCE_OPS1:
            hdmirx_path = "/proc/msp/hdmirx1";
            break;
        case XBH_SOURCE_OPS2:
            hdmirx_path = "/proc/msp/hdmirx2";
            break;
        case XBH_SOURCE_F_HDMI1:
        case XBH_SOURCE_F_USBC1:
            hdmirx_path = "/proc/msp/hdmirx0";
            break;
        default:
            XLOGE("getSignalStatus: unsupported source %d", currSource);
            return XBH_FAILURE;
    }

    FILE *devFile = fopen(hdmirx_path, "r");
    if (devFile == NULL) {
        XLOGE("getSignalStatus: open %s failed: %s", hdmirx_path, strerror(errno));
        return XBH_FAILURE;
    }

    std::unique_ptr<FILE, decltype(&fclose)> fileCloser(devFile, &fclose);
    XBH_CHAR line[512];
    XBH_BOOL found = XBH_FALSE;

    while (fgets(line, sizeof(line), devFile)) {
        // 查找包含 cur_port_state 的行
        XBH_CHAR* statePtr = strstr(line, "cur_port_state");
        if (statePtr != nullptr) {
            // 找到冒号位置
            XBH_CHAR* colonPtr = strchr(statePtr, ':');
            if (colonPtr != nullptr) {
                colonPtr++; // 跳过冒号

                // 去除前导空格
                while (*colonPtr == ' ' || *colonPtr == '\t') colonPtr++;

                // 找到下一个竖线或行尾
                XBH_CHAR* endPtr = strchr(colonPtr, '|');
                if (endPtr != nullptr) {
                    *endPtr = '\0'; // 截断字符串
                } else {
                    // 如果没有竖线，则去除行尾换行符
                    XBH_CHAR* newlinePtr = strchr(colonPtr, '\n');
                    if (newlinePtr != nullptr) *newlinePtr = '\0';
                }

                // 去除尾部空格
                XBH_CHAR* trimPtr = colonPtr + strlen(colonPtr) - 1;
                while (trimPtr > colonPtr && (*trimPtr == ' ' || *trimPtr == '\t')) {
                    *trimPtr = '\0';
                    trimPtr--;
                }

                // 判断状态
                if (strcmp(colonPtr, "no signal") == 0 || strcmp(colonPtr, "no") == 0) {
                    *status = XBH_TRUE;
                } else if (strcmp(colonPtr, "stable") == 0) {
                    *status = XBH_FALSE;
                } else {
                    XLOGW("getSignalStatus: unknown state value: %s", colonPtr);
                    continue;
                }
                found = XBH_TRUE;
                break;
            }
        }
    }

    if (!found) {
        XLOGE("getSignalStatus: cur_port_state not found or invalid in %s", hdmirx_path);
        return XBH_FAILURE;
    }
    XLOGD("getSignalStatus: source=%d, status=%d", currSource, *status);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setDelayTimeForHdmiSwitch()
{
    XBH_S32 ret = XBH_SUCCESS;
    XLOGD(" delay 5000ms \n");
    usleep(5000 * 1000);
    return ret;
}

//setHdmiRx5vDet
XBH_S32 XbhHi_610::setHdmiRx5vDet(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::setSourceScreenOffset(XBH_S32 x, XBH_S32 y)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = uapi_disp_set_screen_relative_position(UAPI_DISPLAY0, x, y);
    if (s32Ret != TD_SUCCESS)
    {
        XLOGE("uapi_disp_set_screen_relative_position wrong\n");
        return XBH_FAILURE;
    }
    XLOGE("uapi_disp_set_screen_relative_position finished");
    return  s32Ret;
}

XBH_S32 XbhHi_610::getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::getTemperatureSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::setBootLogo(const XBH_CHAR* path)
{
    XBH_S32 ret = 0;
    if (path == NULL) 
    {
        XLOGE("Update Logo open fail :/mnt/xbhcust/logo/logo.jpg is no exist\n");
        return XBH_FAILURE;
    }

    struct stat statbuf;

    stat(path, &statbuf);

    size_t fileSize = statbuf.st_size;

    FILE* fpSrc = fopen(path, "r");
    if (fpSrc == nullptr) 
    {
        XLOGE("Update Logo open fail :/mnt/xbhcust/logo/logo.jpg\n");
        return XBH_FAILURE;
    }
    td_u8* pDatabuf = new td_u8[fileSize];
    memset(pDatabuf, 0x0, fileSize);

    td_s32 readSize = static_cast<td_s32>(fread(reinterpret_cast<void *>(pDatabuf), sizeof(char), fileSize, fpSrc));
    if (readSize != fileSize) 
    {
        XLOGE("Update Logo fread fail");
        delete [] pDatabuf;
        fclose(fpSrc);
        return XBH_FAILURE;
    }

    uapi_pdm_disp_param paramData = {};
    ret = uapi_pdm_get_disp_param(UAPI_DISPLAY0, &paramData);
    if (ret != TD_SUCCESS)
    {
        XLOGE("call failed");
        delete [] pDatabuf;
        fclose(fpSrc);
        return XBH_FAILURE;
    }

    ret = uapi_pdm_update_logo_content(paramData.logo_index, pDatabuf, readSize);
    if (ret != TD_SUCCESS)
    {
        XLOGE("uapi_pdm_update_logo_content fail");
        delete [] pDatabuf;
        fclose(fpSrc);
        return XBH_FAILURE;
    }

    delete [] pDatabuf;
    fclose(fpSrc);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
#if 0
    if (mAudioOutput != enAudioOutput)
    {
        switch (enAudioOutput)
        {
            default:
            case XBH_AUDIO_OUTPUT_DET:
            {
                XBH_BOOL status = XBH_FALSE;
                XbhService::getModuleInterface()->getHpDetectStatus(&status);
                if (status)
                {
                    XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                    XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_FALSE);
                }
                else
                {
                    XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                    XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
                }
                break;
            }
            case XBH_AUDIO_OUTPUT_BOTH:
            {
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_FALSE);
                break;
            }
            case XBH_AUDIO_OUTPUT_EXTERNAL:
            {
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_FALSE);
                break;
            }
            case XBH_AUDIO_OUTPUT_INTERNAL:
            {
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
                break;
            }
            case XBH_AUDIO_OUTPUT_USB:
            {
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
                break;
            }
        }
    }
#endif
    mAudioOutput = enAudioOutput;
    return s32Ret;
}

XBH_S32 XbhHi_610::getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *enAudioOutput = mAudioOutput;
    return s32Ret;
}

XBH_S32 XbhHi_610::setWatchDogManualModeEnable(XBH_BOOL bEnable)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getWatchDogManualModeEnable(XBH_BOOL *bEnable)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::setWatchDogEnable(XBH_BOOL bEnable)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::setWatchDogTimeOut(XBH_U32 u32TimeOutSec)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getWatchDogTimeOut(XBH_U32 *u32TimeOutSec)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::watchDogPing(XBH_VOID)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getPwmFrequency(XBH_U32* u32PwmF, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::setPwmFrequency(XBH_U32 u32PwmF, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::loadEdlaCust(XBH_BOOL *enable)
{
    FILE *fp = NULL;
    XBH_S32 res = 0;

    fp = fopen(XBH_CUST_LOGO_PATH, "r");
    if (NULL != fp)
    {
        XLOGD("%s  exist ", XBH_CUST_LOGO_PATH);
        setBootLogo(XBH_CUST_LOGO_PATH);
        fclose(fp);
    }
    else
    {
        XLOGD("%s no exist ", XBH_CUST_LOGO_PATH);
    }
    //loadPq();
    //res = loadProperty();
    //customerversion 一致不需要重启
    // if (res == 1) {
    //     *enable = XBH_FALSE;
    // } else {
    //     *enable = XBH_TRUE;
    // }
    return res;
}

XBH_S32 XbhHi_610::loadPq()
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::setBacklightEnable(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

XBH_S32 XbhHi_610::getBacklightEnable(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数 ,范围0~511
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数 ,范围0~511
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 保存对应模式的具体色温数据，保存到cusdata分区
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数 ,范围0~511
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::saveColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_GAIN_OFFSET_H610_DATA_S colorTempValue[XBH_COLORTEMP_BUTT];
    XBH_S16 u32Crc16Value = 0xff;

    //Confirm the input param
    if (enColorTemp >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("Error! saveColorTempPara fail, enColorTemp = %d\n", enColorTemp);
        return XBH_FAILURE;
    }
    if (data == NULL)
    {
        XLOGE("Error! saveColorTempPara data is null\n");
        return XBH_FAILURE;
    }
    //Get data from cusdata partition
    memset(colorTempValue, 0, sizeof(colorTempValue));
    s32Ret = XbhPartitionData::getInstance()->readDataFromCusdata(XBH_CUSDATA_H610_COLORTEMP_DATA_OFFSET, XBH_CUSDATA_H610_COLORTEMP_DATA_LEN, (XBH_CHAR *)colorTempValue);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Error! saveColorTempPara readDataFromCusdata fail, ret = %d\n", s32Ret);
        return XBH_FAILURE;
    }
    //Copy data form user and calculate the crc16 value
    colorTempValue[enColorTemp].u32RedGain = data->u32RedGain;
    colorTempValue[enColorTemp].u32GreenGain = data->u32GreenGain;
    colorTempValue[enColorTemp].u32BlueGain = data->u32BlueGain;
    colorTempValue[enColorTemp].u32RedOffset = data->u32RedOffset;
    colorTempValue[enColorTemp].u32GreenOffset = data->u32GreenOffset;
    colorTempValue[enColorTemp].u32BlueOffset = data->u32BlueOffset;
    colorTempValue[enColorTemp].u32CrcValue = XbhCrc::calCrc16By8005((XBH_U8 *)&colorTempValue[enColorTemp], sizeof(XBH_GAIN_OFFSET_H610_DATA_S) - sizeof(XBH_U32));
    XLOGD("colorTempValue[%d].u32CrcValue = 0x%x\n", enColorTemp, colorTempValue[enColorTemp].u32CrcValue);
    // Write data to cusdata partition
    s32Ret = XbhPartitionData::getInstance()->writeDataToCusdata(XBH_CUSDATA_H610_COLORTEMP_DATA_OFFSET, XBH_CUSDATA_H610_COLORTEMP_DATA_LEN, (XBH_CHAR *)colorTempValue);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Error! saveColorTempPara writeDataToCusdata fail, ret = %d\n", s32Ret);
        return XBH_FAILURE;
    }
    return  s32Ret;
}

/**
 * 读取对应模式的具体色温数据，从cusdata分区读取
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数 ,范围0~511
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::loadColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32Crc16Value = 0xff;
    XBH_GAIN_OFFSET_H610_DATA_S colorTempValue[XBH_COLORTEMP_BUTT];

    //Confirm the input param
    if (enColorTemp >= XBH_COLORTEMP_BUTT)
    {
        XLOGE("Error! loadColorTempPara fail, enColorTemp = %d\n", enColorTemp);
        return XBH_FAILURE;
    }
    if (data == NULL)
    {
        XLOGE("Error! loadColorTempPara data is null\n");
        return XBH_FAILURE;
    }

    //Get data from cusdata partition
    memset(colorTempValue, 0, sizeof(colorTempValue));
    s32Ret = XbhPartitionData::getInstance()->readDataFromCusdata(XBH_CUSDATA_H610_COLORTEMP_DATA_OFFSET, XBH_CUSDATA_H610_COLORTEMP_DATA_LEN, (XBH_CHAR *)colorTempValue);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Error! loadColorTempPara readDataFromCusdata fail, ret = %d\n", s32Ret);
        //return XBH_FAILURE;
        goto hisiData;
    }
    //Confirm the crc16 value
    u32Crc16Value = XbhCrc::calCrc16By8005((XBH_U8 *)&colorTempValue[enColorTemp], sizeof(XBH_GAIN_OFFSET_H610_DATA_S) - sizeof(XBH_U32));
    if (colorTempValue[enColorTemp].u32CrcValue != u32Crc16Value)
    {
        XLOGE("loadColorTempPara check crc16 value fail.\n");
        XLOGE("u32Crc16Value: 0x%x\n", u32Crc16Value);
        XLOGE("u32FlashCrc16Value: 0x%x\n", colorTempValue[enColorTemp].u32CrcValue);
        //return XBH_FAILURE;
        goto hisiData;
    }
    else
    {
        // For debug
        //XLOGD("loadColorTempPara crc ok! u32Crc16Value: 0x%x\n", u32Crc16Value);
    }
    // Crc can not calculate all zero value. But all zero is in issue in color temp
    if (colorTempValue[enColorTemp].u32RedGain == 0
            && colorTempValue[enColorTemp].u32GreenGain == 0
            && colorTempValue[enColorTemp].u32GreenGain == 0
            && colorTempValue[enColorTemp].u32RedOffset == 0
            && colorTempValue[enColorTemp].u32GreenOffset == 0
            && colorTempValue[enColorTemp].u32BlueOffset == 0)
    {
        XLOGI("The color temp in cusdata is all zero. It is not vaild data.\n");
        //return XBH_FAILURE;
        goto hisiData;
    }
    //Copy data to interface
    XLOGI("Load colortemp from cusdata.\n");
    data->u32RedGain = colorTempValue[enColorTemp].u32RedGain;
    data->u32GreenGain = colorTempValue[enColorTemp].u32GreenGain;
    data->u32BlueGain = colorTempValue[enColorTemp].u32BlueGain;
    data->u32RedOffset = colorTempValue[enColorTemp].u32RedOffset;
    data->u32GreenOffset = colorTempValue[enColorTemp].u32GreenOffset;
    data->u32BlueOffset = colorTempValue[enColorTemp].u32BlueOffset;
    return  s32Ret;

hisiData:
    Array<XBH_S32> hisiColorTempArray;
    if (enColorTemp == XBH_COLORTEMP_STANDARD)
    {
        hisiColorTempArray = HiCfgFact::Get()->ParseIntArray("databasedef:FacColorTemp_hdmi_4K_nature");
    }
    else if(enColorTemp == XBH_COLORTEMP_WARM)
    {
        hisiColorTempArray = HiCfgFact::Get()->ParseIntArray("databasedef:FacColorTemp_hdmi_4K_warm");
    }
    else if (enColorTemp == XBH_COLORTEMP_COLD)
    {
        hisiColorTempArray = HiCfgFact::Get()->ParseIntArray("databasedef:FacColorTemp_hdmi_4K_cool");
    }
    else if (enColorTemp == XBH_COLORTEMP_USER)
    {
        hisiColorTempArray = HiCfgFact::Get()->ParseIntArray("databasedef:FacColorTemp_hdmi_4K_user");
    }
    else
    {
        hisiColorTempArray = HiCfgFact::Get()->ParseIntArray("databasedef:FacColorTemp_hdmi_4K_nature");
    }
    if (hisiColorTempArray.length() != 6)
    {
        XLOGE("Error! Read color temp from hisi fail\n");
        return XBH_FAILURE;
    }
    XLOGI("Load colortemp from hisi db.ini.\n");
    data->u32RedGain = hisiColorTempArray[0];
    data->u32GreenGain = hisiColorTempArray[1];
    data->u32BlueGain = hisiColorTempArray[2];
    data->u32RedOffset = hisiColorTempArray[3];
    data->u32GreenOffset = hisiColorTempArray[4];
    data->u32BlueOffset = hisiColorTempArray[5];

    return  XBH_SUCCESS;
}

/**
 * 使用GPIO的名字来设置GPIO的方向和值
 * param[in] gpioName. gpio名字
 * param[in] gpioDirection. gpio方向 1:输出 2.输入
 * param[in] u32GpioValue. gpio输出值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32 gpioDirection, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 i = 0;
    XBH_S32 count = sizeof(gpio_common_table) / sizeof(struct gpio_common_info);
    XLOGD("gpioName = %s  gpioDirection = %d u32GpioValue\n",gpioName,gpioDirection);

    for (i = 0; i < count; i++)
    {
        if (strcmp(gpioName, gpio_common_table[i].gpio_name) == 0 )
        {
            XLOGD(" gpio_common_table find name ");
            if (gpio_common_table[i].gpio_number != XBH_NO_CONFIG)
            {
                if (gpioDirection)
                {
                    setGpioOutputValue(gpio_common_table[i].gpio_number, u32GpioValue);
                } 
                else
                { 
                    setGpioDirInput(gpio_common_table[i].gpio_number);
                }
                s32Ret = XBH_SUCCESS;
                XLOGD(" gpio_common_table find gpio_number = %d\n" ,gpio_common_table[i].gpio_number);
            }
            XLOGD(" gpio_common_table no find gpio_number ");
            break;
        }
    }

    return  s32Ret;
}

/**
 * 使用GPIO的名字来设获取GPIO的值
 * param[in] gpioName. gpio名字
 * param[out] u32GpioValue. gpio获取返回值 
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32* u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 i = 0;
    *u32GpioValue = XBH_NO_CONFIG;
    XBH_S32 count = sizeof(gpio_common_table) / sizeof(struct gpio_common_info);
    XLOGD("%s  gpioName = %s\n", __func__ ,gpioName);
    for (i = 0; i < count; i++) {
            if (strcmp(gpioName,gpio_common_table[i].gpio_name) == 0 ) {
                XLOGD("s gpio_common_table find name ", __func__);

                if (gpio_common_table[i].gpio_number != XBH_NO_CONFIG) {

                    s32Ret = getGpioOutputValue(gpio_common_table[i].gpio_number,u32GpioValue);

                    
                }
                XLOGD("%sgpio_common_table find gpio_number ", __func__);
                break;
            }
    }
    return  s32Ret;
}

XBH_S32 XbhHi_610::setcheckColorTempValue(XBH_BOOL CheckColorTemp)
{
    mCheckColorTemp = CheckColorTemp;
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::getcheckColorTempValue(XBH_BOOL *CheckColorTemp)
{
    *CheckColorTemp = mCheckColorTemp;
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::checkColorTempParatGain(XBH_U32 gain)
{
    if(gain > COLOR_TEMP_GAIN_MIN && gain <= COLOR_TEMP_GAIN_MAX)
    {
        return gain;
    }
    setcheckColorTempValue(false);
    return COLOR_TEMP_GAIN_DEFAULT;
}

XBH_S32 XbhHi_610::checkColorTempParatOffset(XBH_U32 gain)
{
    if(gain >=  COLOR_TEMP_GAIN_OFFSET_MIN  && gain <= COLOR_TEMP_GAIN_OFFSET_MAX )
    {
        return gain;
    }
    setcheckColorTempValue(false);
    return COLOR_TEMP_GAIN_OFFSET_DEFAULT;
}


XBH_S32 XbhHi_610::checkColorTempParat(XBH_GAIN_OFFSET_DATA_S* data)
{
    //执行check操作前复位状态，避免第二次校验开始一直为false
    mCheckColorTemp = true;
    data -> u32RedGain = checkColorTempParatGain(data -> u32RedGain);
    data -> u32GreenGain = checkColorTempParatGain(data -> u32GreenGain);
    data -> u32BlueGain = checkColorTempParatGain(data -> u32BlueGain);
    data -> u32RedOffset = checkColorTempParatOffset(data -> u32RedOffset);
    data -> u32GreenOffset = checkColorTempParatOffset(data -> u32GreenOffset);
    data -> u32BlueOffset = checkColorTempParatOffset(data -> u32BlueOffset);
    return XBH_SUCCESS;
}

XBH_S32 XbhHi_610::checkColorTempValue(XBH_GAIN_OFFSET_DATA_S* data)
{
    if (data -> u32RedGain == COLOR_TEMP_GAIN_MIN &&
        data -> u32GreenGain == COLOR_TEMP_GAIN_MIN &&
        data -> u32BlueGain == COLOR_TEMP_GAIN_MIN ) 
    {
        data -> u32RedGain = COLOR_TEMP_GAIN_DEFAULT ;
        data -> u32GreenGain =  COLOR_TEMP_GAIN_DEFAULT;
        data -> u32BlueGain  = COLOR_TEMP_GAIN_DEFAULT;
    }
     return XBH_SUCCESS;
}

/**
 * 设置信源低延时
 * param[in] bEnable 1：on  0:off
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

XBH_S32 XbhHi_610::getEthPortSpeed(XBH_S32 port, XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    string net_speed;
    ifstream net_cfg;
    net_cfg.open(net_list[port].c_str());
    if (net_cfg.is_open())
    {
        net_cfg >> net_speed;
        *value = atoi(net_speed.c_str());
        s32Ret = XBH_SUCCESS;
        net_cfg.close();
    }
    return s32Ret;
}

/**
 * 写入Project ID
 * param[in] value Project ID
 * retval true:success, false:failure
*/
XBH_S32 XbhHi_610::setProjectId(const XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //由于色温是同步保存到了partitiondata 中，切换project id的时候需要把之前的数据清空
    XBH_GAIN_OFFSET_DATA_S data = {0, 0, 0, 0, 0, 0};
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_STANDARD, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_WARM, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_COLD, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_USER, &data);
    //#ifdef XBH_USE_COLORTEMP_EXT
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_NATIVE, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_3000K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_4000K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_5000K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_6500K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_7500K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_9300K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_10000K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_11000K, &data);
    //XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_13000K, &data);
    //#endif
    //重启后hw_output将会重新将json里面预设的pq参数写入到baseparament数据库里面
    property_set("persist.vendor.xbh.pq.init", "0");
    //设置标志, 一旦project id被设置，到重启前都不能往partitiondata 里面写入色温数据
    property_set("persist.vendor.xbh.projectid.pwm_bl_init", "0");
    property_set("persist.vendor.xbh.PrimaryBacklightMax", "");
    property_set("persist.vendor.xbh.PrimaryBacklightMin", "");
    property_set("persist.vendor.xbh.PrimaryBacklightDef", "");
    property_set("persist.vendor.xbh.global_dimming_target_pwm", "");
    //更新VGA EDID update标志
    property_set("persist.vendor.xbh.vgaedid.update", "0");
    mIsProjectIdChange = XBH_TRUE;
    //更新project id
    //s32Ret  = XbhPartitionData::getInstance()->setProjectId(strProjectId);
    XLOGD("setProjectId strProjectId=%s\n", strProjectId);
    return  s32Ret;
}

/**
 * 获取Project ID
 * param[out] strProjectId Project ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getProjectId(XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getProjectId(strProjectId);
    XLOGD("getProjectId strProjectId=%s\n", strProjectId);
    return  s32Ret;
}
/**
 * 设置待机ADC中断
 * param[in] bEnable 1：on  0:off
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setAdcIntEnabled(XBH_BOOL bEnable)
{
    XBH_S32 ret = XBH_FAILURE;

    return ret;
}

/**
 * 写入SFT工厂数据
 * param[in] data 数据
 * retval true:success, false:failure
*/
XBH_S32 XbhHi_610::setSftData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSftData(data);
    XLOGD("setSftData data=%s\n", data);
    return  s32Ret;
}

/**
 * 获取SFT工厂数据
 * param[out] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSftData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSftData(data);
    XLOGD("getSftData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setSmartPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSmartPnData(data);
    XLOGD("setSmartPnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储产品的PN码，一个系列对应一个PN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSmartPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSmartPnData(data);
    XLOGD("getSmartPnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的SN码，一个系列对应一个SN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setSmartSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSmartSnData(data);
    XLOGD("setSmartSnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储产品的SN码，一个系列对应一个SN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSmartSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSmartSnData(data);
    XLOGD("getSmartSnData data=%s\n", data);
    return  s32Ret;
}

/**
 * SMART希望罗盘关闭的状态能永久保存，复位不被清除
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setCompassPresist(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setCompassPresist(data);
    XLOGD("setCompassPresist data=%s\n", data);
    return  s32Ret;
}

/**
* SMART希望罗盘关闭的状态能永久保存，复位不被清除
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getCompassPresist(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getCompassPresist(data);
    XLOGD("getCompassPresist data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储底板的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setSmartScalerPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSmartScalerPnData(data);
    XLOGD("setSmartScalerPnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储底板的PN码，一个系列对应一个PN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSmartScalerPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSmartScalerPnData(data);
    XLOGD("getSmartScalerPnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储底板的SN码，一个系列对应一个SN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setSmartScalerSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSmartScalerSnData(data);
    XLOGD("setSmartScalerSnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储底板的SN码，一个系列对应一个SN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSmartScalerSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSmartScalerSnData(data);
    XLOGD("getSmartScalerSnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储核心板的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setSmartCorePnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSmartCorePnData(data);
    XLOGD("setSmartCorePnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储核心板的PN码，一个系列对应一个PN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSmartCorePnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSmartCorePnData(data);
    XLOGD("getSmartCorePnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储核心板的SN码，一个系列对应一个SN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::setSmartCoreSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->setSmartCoreSnData(data);
    XLOGD("setSmartCoreSnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储核心板的SN码，一个系列对应一个SN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getSmartCoreSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getSmartCoreSnData(data);
    XLOGD("getSmartCoreSnData data=%s\n", data);
    return  s32Ret;
}

/**
 * 设置工厂地址 地址
 * param[in] macType 地址类型
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType, const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->saveFactoryMac(macType, data);
    XLOGD("macType:%d setMacAddress data=%s\n", macType, data);
    return  s32Ret;
}

/**
* 获取工厂地址
* param[out] macType 地址类型
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getFactoryMac(XBH_MAC_ADDRESS_TYPE macType, XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getFactoryMac(macType, data);
    XLOGD("macType:%d getMacAddress data=%s\n", macType, data);
    return  s32Ret;
}

/**
 * FATT烧录工厂SN
 * param[in] materialType 物料类型  0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
 * param[in] snInfo 写入数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::burnFactorySN(XBH_MATERAL_TYPE materialType, const XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->burnFactorySN(materialType, snInfo);
    XLOGD("materialType:%d snInfo=%s\n", materialType, snInfo);
    return  s32Ret;
}

/**
* 获取FATT烧录工厂SN
* param[out] materialType 物料类型 0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
* param[out] snInfo 写入数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhHi_610::getBurnFactorySN(XBH_MATERAL_TYPE materialType,  XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //s32Ret  = XbhPartitionData::getInstance()->getBurnFactorySN(materialType, snInfo);
    XLOGD("materialType:%d snInfo=%s\n", materialType, snInfo);
    return  s32Ret;
}

XBH_S32 XbhHi_610::setHdmiHPD(int portId, XBH_LEVEL_E level)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL hpdLevel = XBH_FALSE;
    switch(level)
    {
        case XBH_LEVEL_HIGH:
            hpdLevel = XBH_TRUE;
            break;
        case XBH_LEVEL_LOW:
            hpdLevel = XBH_FALSE;
            break;
        default:
            XLOGE("Hpd level invalid");
            s32Ret = XBH_FAILURE;
            break;
    }

    if(s32Ret == XBH_FAILURE)
    {
        XLOGE("Hpd level invalid, ready return");
        return s32Ret;
    }

    switch(portId)
    {
        case H610_HDMIRX_0_PORT_ID:
            s32Ret = uapi_hdmirx_set_hpd_value(UAPI_HDMIRX_PORT0, (td_bool)hpdLevel);
            break;
        case H610_HDMIRX_1_PORT_ID:
            s32Ret = uapi_hdmirx_set_hpd_value(UAPI_HDMIRX_PORT1, (td_bool)hpdLevel);
            break;
        case H610_HDMIRX_2_PORT_ID:
            s32Ret = uapi_hdmirx_set_hpd_value(UAPI_HDMIRX_PORT2, (td_bool)hpdLevel);
            break;
        default:
            XLOGE("Hdmi port id invalid");
            s32Ret = XBH_FAILURE;
            break;
    }

    return s32Ret;
}

//override
XBH_S32 XbhHi_610::getSecurityHvbKeyStatus(XBH_U32 *u32Status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    uapi_otp_asymmetric_key_type key_type = UAPI_OTP_ASYMMETRIC_KEY_ECC;
    td_bool status = TD_FALSE;
    td_bool stat = TD_FALSE;
    s32Ret = uapi_otp_get_asymmetric_key_lock_stat(key_type, &status);
    XLOGD("%s,%d,uapi_otp_get_asymmetric_key_lock_stat s32Ret = %d, status = %d\n",
            __FUNCTION__,__LINE__,s32Ret,status);
    s32Ret |= uapi_otp_get_scs_stat(&stat);
    XLOGD("%s,%d,uapi_otp_get_scs_stat s32Ret = %d, stat = %d\n",
            __FUNCTION__,__LINE__,s32Ret,stat);
    if (status == TD_TRUE && stat == TD_TRUE)
    {
        *u32Status = XBH_TRUE;
    }
    else
    {
        *u32Status = XBH_FALSE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhHi_610::upgradeSystemFirmware(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL enable = XBH_TRUE;
    s32Ret = XbhPartitionData::getInstance()->setUpgradeSystemFlag(&enable);
    *bEnable = XBH_TRUE;
    return  s32Ret;
}

// 读取文件内容工具函数
std::string readFileContent(const std::string& path) 
{
    std::ifstream file(path);
    if (!file.is_open()) 
        return "";
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    // 去除换行符和空格
    content.erase(content.find_last_not_of(" \n\r\t") + 1);
    return content;
}

// 遍历USB设备节点并生成JSON
std::string XbhHi_610::getUsbDeviceInfos(const std::string& input) 
{
    const std::string basePath = "/sys/bus/usb/devices/";
    DIR *dir = opendir(basePath.c_str());
    std::vector<std::string> deviceDirs;
    std::string jsonStr = input;
    jsonStr += "[";

    if (!dir) 
    {
        std::cerr << "Failed to open directory" << std::endl;
        return "[]";
    }

    // 遍历目录并筛选设备节点
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) 
    {
        std::string name = entry->d_name;
        if ( name == "." || name == ".." 
            ||  name.find("usb") != std::string::npos || name.find(":") != std::string::npos
            )
        {
            // XLOGD("%s: %d continue by dir name=%s", __FUNCTION__, __LINE__,name.c_str());
            continue;
        }
        std::string fileProductPath = basePath + name + "/product";
        std::string fileProductName = readFileContent(fileProductPath);
        if (!fileProductName.empty()) 
        {
            if( fileProductName.find("Hub") != std::string::npos )
            {
                // XLOGD("%s: %d continue by product node name=%s,fileProductName=%s", __FUNCTION__, __LINE__,name.c_str(),fileProductName.c_str());
                continue;
            }
        }
        deviceDirs.push_back(name);
        // XLOGD("%s: %d add dir name=%s", __FUNCTION__, __LINE__,name.c_str());
    }
    closedir(dir);

    // 处理每个设备节点
    for (const auto& dirName : deviceDirs)
    {
        std::string devicePath = basePath + dirName + "/";
        std::map<std::string, std::string> deviceInfo;

        // 定义需要读取的属性字段
        const std::vector<std::string> fields = 
        {
            "idVendor", "idProduct", "speed","bDeviceClass", "manufacturer", "product","bInterfaceClass"
        };

        // 遍历字段并读取文件内容
        bool hasValidData = false;
        for (const auto& field : fields) 
        {
            std::string filePath = devicePath + field;
            if(field.find("bInterfaceClass") != std::string::npos){
                filePath = basePath + dirName  +":1.0/"+ field ;
            }
            // XLOGD("%s: %d  filePath=%s", __FUNCTION__, __LINE__,filePath.c_str());
            std::string value = readFileContent(filePath);
            if (!value.empty()) 
            {
                hasValidData = true;
                deviceInfo[field] = value;
                // XLOGD("%s: %d filePath=%s,value=%s", __FUNCTION__, __LINE__,filePath.c_str(),value.c_str());
            }
        }

        // 生成JSON对象
        if (hasValidData) 
        {
            jsonStr += "{";
            jsonStr += "\"node\":\"" + dirName + "\",";
            for (auto it = deviceInfo.begin(); it != deviceInfo.end(); ++it) {
                jsonStr += "\"" + it->first + "\":\"" + it->second + "\",";
                
            }
            jsonStr.pop_back(); // 删除最后一个逗号
            jsonStr += "},";
        }
    }

    if (jsonStr.size() > 1)
    {
        jsonStr.pop_back(); // 删除最后一个逗号
    }

    jsonStr += "]";
    XLOGD("%s: %d jsonStr=%s", __FUNCTION__, __LINE__,jsonStr.c_str());
    return jsonStr;
}

//辅助函数：去除字符串两端指定字符
std::string trim(const std::string& s, const std::string& delimiters = " \t\n\r\"")
{
    size_t begin = s.find_first_not_of(delimiters);
    if (begin == std::string::npos) return "";
    
    size_t end = s.find_last_not_of(delimiters);
    return s.substr(begin, end - begin + 1);
}

// JSON解析主函数
void parseAndUpdateBacklightConfig(const std::string& file_path)
{
    XLOGD("%s: %d file_path=%s", __FUNCTION__, __LINE__,file_path.c_str());
    // 读取文件内容
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        XLOGE("%s: %d Error: Cannot open file ", __FUNCTION__, __LINE__);
        return;
    }
    
    // 读取并预处理内容（移除所有空白字符）
    std::string content;
    std::string line;
    while (std::getline(file, line))
    {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        content += line;
    }
    
    // 验证基本结构 [ { ... } ]
    if (content.empty() || content.front() != '[' || content.back() != ']')
    {
        XLOGE("%s: %d Error: Invalid JSON array format ", __FUNCTION__, __LINE__);
        return;
    }
    
    // 提取第一个对象内容
    size_t obj_start = content.find('{');
    size_t obj_end = content.find('}', obj_start);
    if (obj_start == std::string::npos || obj_end == std::string::npos)
    {
        XLOGE("%s: %d Error: Missing object structure ", __FUNCTION__, __LINE__);
        return;
    }
    
    std::string obj_str = content.substr(obj_start + 1, obj_end - obj_start - 1);
    
    // 分割键值对
    std::vector<std::string> pairs;
    size_t pos = 0;
    while (pos < obj_str.length())
    {
        size_t comma_pos = obj_str.find(',', pos);
        if (comma_pos == std::string::npos) comma_pos = obj_str.length();
        pairs.push_back(obj_str.substr(pos, comma_pos - pos));
        pos = comma_pos + 1;
    }
    
    // 解析目标参数
    struct Config {
        int pwm_bl_max = -1;
        int pwm_bl_min = -1;
        int pwm_bl_def = -1;
        int global_dimming_target_pwm = -1;
    } config;

    for (const auto& pair : pairs)
    {
        size_t colon = pair.find(':');
        if (colon == std::string::npos) continue;
        
        std::string key = trim(pair.substr(0, colon));
        std::string val = trim(pair.substr(colon + 1));
        
        int num = std::stoi(val);
        if (key == "pwm_bl_max") config.pwm_bl_max = num;
        else if (key == "pwm_bl_min") config.pwm_bl_min = num;
        else if (key == "pwm_bl_def") config.pwm_bl_def= num;
        else if (key == "global_dimming_target_pwm") config.global_dimming_target_pwm= num;
    }

    XLOGD("%s: %d config.pwm_bl_max=%d,config.pwm_bl_min=%d,config.pwm_bl_def=%d,config.global_dimming_target_pwm=%d",
         __FUNCTION__, __LINE__,config.pwm_bl_max,config.pwm_bl_min,config.pwm_bl_def,config.global_dimming_target_pwm);

    if(config.pwm_bl_max >0 )
    {
        char backlightMaxChar[32]= {0};
        sprintf(backlightMaxChar,"%d",config.pwm_bl_max);  
        property_set("persist.vendor.xbh.PrimaryBacklightMax", backlightMaxChar);
    }
    if(config.pwm_bl_min >0 )
    {
        char backlightMinChar[32]= {0};
        sprintf(backlightMinChar,"%d",config.pwm_bl_min);  
        property_set("persist.vendor.xbh.PrimaryBacklightMin", backlightMinChar);
    }

    int projectidPwmBlInit = property_get_int32("persist.vendor.xbh.projectid.pwm_bl_init", 0);
    XLOGD("projectidPwmBlInit=%d",projectidPwmBlInit);
    if( projectidPwmBlInit == 0 ){
        if(config.global_dimming_target_pwm >0 ){  
            char globalDimmingTargetPwmChar[32]= {0};
            sprintf(globalDimmingTargetPwmChar,"%d",config.global_dimming_target_pwm); 
            property_set("persist.vendor.xbh.global_dimming_target_pwm", globalDimmingTargetPwmChar);
        }
        if(config.pwm_bl_def >0 ){  
            char backlightDefChar[32]= {0};
            sprintf(backlightDefChar,"%d",config.pwm_bl_def); 
            property_set("persist.vendor.xbh.PrimaryBacklightDef", backlightDefChar);
            property_set("persist.vendor.xbh.projectid.pwm_bl_init", "1");
        }
    }   
}

void initBacklightPwmConfig(int projectid)
{
    XLOGD("%s: %d projectid=%d", __FUNCTION__, __LINE__,projectid);
    if(projectid > 0)
	{
        std::string configPath = "/vendor/etc/project_ids/projectid_" + std::to_string(projectid) 
            + "/backlight/backlight_config.json";
        parseAndUpdateBacklightConfig(configPath); 
    }
}

/**
 * 读取预置EDID的bin文件值，并通过Edidtype更新EDID的bin数据到HDMI switch
 * @param projecitd
 * @param edidBinFilePath edidbin文件存储路径
 * @param node edidbin文件存储节点
 */
XBH_S32 XbhHi_610::readAndUpdateEdidBinFileByEdidType(int dev, const char* edidBinFilePath, int port)
{
    // ==================== 第一部分：读取二进制文件 ====================
    FILE *src_file = nullptr;
    unsigned char *file_buffer = nullptr; // 使用无符号类型便于十六进制打印
    // 打开源文件
    if (!(src_file = fopen(edidBinFilePath, "rb")))
    {
        XLOGD("打开源文件失败");
        return EXIT_FAILURE;
    }
    // 获取文件大小
    struct stat file_stat;
    if (fstat(fileno(src_file), &file_stat) == -1)
    {
        XLOGD("获取文件大小失败");
        fclose(src_file);
        return EXIT_FAILURE;
    }
    const size_t file_size = file_stat.st_size;
    // 分配内存缓冲区
    if (!(file_buffer = static_cast<unsigned char*>(malloc(file_size))))
    {
        XLOGD("内存分配失败");
        fclose(src_file);
        return EXIT_FAILURE;
    }
    // 读取文件内容
    if (fread(file_buffer, 1, file_size, src_file) != file_size)
    {
        XLOGD("文件读取不完整");
        free(file_buffer);
        fclose(src_file);
        return EXIT_FAILURE;
    }
    fclose(src_file);
    // ==================== 数据打印模块 ====================
    // for (size_t i = 0; i < file_size; ++i) {
    //     // 每16字节换行
    //     if ( (i+1) % 16 == 0) {
    //         XLOGD("i=%d,file_buffer[i]=%02X", i,file_buffer[i]);
    //     }
    // }

    // ==================== 第二部分：更新EDID ====================
    switch(dev)
    {
        default:
            XbhHdmiSwitchManager::getInstance()->updateEdid(dev, file_buffer, port);
            break;
    }
    // ==================== 资源清理 ====================
    usleep(10*1000);
    //free(file_buffer);
    return EXIT_SUCCESS;
}

XBH_S32 XbhHi_610::getHdmiRxLockStatus(XBH_BOOL* lock)
{
    XBH_S32 ret = XBH_FAILURE;
    *lock = XBH_FALSE;
    return ret;
}

XBH_S32 XbhHi_610::getMokaPModeEnableFromCusdata(XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::getBacklight(XBH_U32* u32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::setMonitorId(XBH_S32 monitorId)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::ProcessTypeCHotplug(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::ProcessTypeBHotplug(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhHi_610::getEmmcVersion(XBH_U32 *pu32Version)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string value;
    XBH_CHAR* endPtr = nullptr;

    // 读取EMMC版本信息
    XbhSysOpt::getInstance()->readSysfs("/sys/class/mmc_host/mmc0/mmc0:0001/rev", value);

    if (value.empty())
    {
        s32Ret = XBH_FAILURE;
        *pu32Version = 0;
        return s32Ret;
    }

    XBH_ULONG Version = 0;
    size_t verPos = value.find("0x");
    if (verPos != std::string::npos) {
        Version = strtoul(value.c_str() + verPos + 2, &endPtr, 16);
    }
    *pu32Version = static_cast<XBH_U32>(Version);
    XLOGD("getEmmcVersion: 0x%x\n", *pu32Version);
    return s32Ret;
}

XBH_S32 XbhHi_610::getEmmcLifeTime(XBH_U32 *pu32LifeTime)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string value;
    XBH_CHAR* endPtr = nullptr;

    // 读取EMMC寿命信息
    XbhSysOpt::getInstance()->readSysfs("/sys/class/mmc_host/mmc0/mmc0:0001/life_time", value);

    if (value.empty())
    {
        s32Ret = XBH_FAILURE;
        *pu32LifeTime = 0;
        return s32Ret;
    }

    // 解析两个16进制值（MLC和SLC）
    XBH_ULONG mlcLife = 0;
    XBH_ULONG slcLife = 0;

    // 查找第一个0x前缀
    size_t slcPos = value.find("0x");
    if (slcPos != std::string::npos) {
        slcLife = strtoul(value.c_str() + slcPos + 2, &endPtr, 16);
    }

    // 查找第二个0x前缀
    size_t mlcPos = value.find("0x", mlcPos + 1);
    if (mlcPos != std::string::npos) {
        mlcLife = strtoul(value.c_str() + mlcPos + 2, &endPtr, 16);
    }

    // 取两者中的最大值
    *pu32LifeTime = static_cast<XBH_U32>(std::max(mlcLife, slcLife));

    XLOGD("getEmmcLifeTime: MLC=0x%lx SLC=0x%lx Max=0x%x",
          mlcLife, slcLife, *pu32LifeTime);
    return s32Ret;
}


XbhHi_610::XbhHi_610()
{
    XLOGD(" begin ");
    td_s32 ret;
    uapi_lsadc_config config;
    td_u32 mask = 0;
    td_u32 channel_mask = 0;

    /*H610 uapi gpio component initial*/
    if (uapi_gpio_init() != TD_SUCCESS) {
        m_uapi_gpio_init_ok = XBH_FALSE;
        XLOGE("uapi_gpio_init fail, ErrorCode=0x%x\n", uapi_gpio_init());
    } else {
        m_uapi_gpio_init_ok = XBH_TRUE;
    }

    /*H610 uapi i2c component initial*/
    if (uapi_i2c_init() != TD_SUCCESS) {
        m_uapi_i2c_init_ok = XBH_FALSE;
        XLOGE("uapi_i2c_init fail, ErrorCode=0x%x\n", uapi_i2c_init());
    } else {
        m_uapi_i2c_init_ok = XBH_TRUE;
    }
    /*H610 uapi HDMI RX component initial*/
    if (uapi_hdmirx_init() != TD_SUCCESS) {
        m_uapi_hdmirx_init_ok = XBH_FALSE;
        XLOGE("uapi_hdmirx_init fail, ErrorCode=0x%x\n", uapi_hdmirx_init());
    } else {
        m_uapi_hdmirx_init_ok = XBH_TRUE;
    }
    /*H610 uapi disp component initial*/
    if (uapi_disp_init() != TD_SUCCESS)
    {
        XLOGE("uapi_disp_init fail, ErrorCode=0x%x\n", uapi_disp_init());
    }

    mIsProjectIdChange = XBH_FALSE;
    XbhPartitionData::getInstance()->setDataPath(MSDC_CUSDATA_PATH, MSDC_CUSPARAM_PATH);
    XBH_CHAR strProjectId[XBH_CUSDATA_PROJECT_ID_LEN] = {0};
    //XbhPartitionData::getInstance()->getProjectId(strProjectId);
    XLOGD("getProjectId strProjectId=%s\n", strProjectId);
    property_set("persist.vendor.xbh.project_id", strProjectId);
    int projectid = atoi(strProjectId);
    initBacklightPwmConfig(projectid);
    //默认EDID类型设置
    if(property_get_int32("persist.vendor.xbh.edid.type.init", 0) == 0)
    {
        property_set("persist.vendor.xbh.edid.type.init", "1");
        int edidType = property_get_int32("ro.vendor.xbh.edid.type", 2);
        setIndependenceSourceEdidType(XBH_SOURCE_HDMI1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_HDMI2, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_HDMI3, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_HDMI4, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_DP1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_DP2, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_USBC1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_USBC2, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_OPS1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_OPS2, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_F_USBC1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_F_USBC2, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_F_HDMI1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_F_HDMI2, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_F_DP1, edidType);
        setIndependenceSourceEdidType(XBH_SOURCE_F_DP2, edidType);
    }
    //hisi init serial
    ret = uapi_sys_init();
    if (ret != TD_SUCCESS)
    {
        XLOGE("uapi_sys_init fail, ErrorCode=0x%x\n", ret);
    }
    if (uapi_uart_init()) {
        XLOGD("h610 serial init failed!!\n");
    }
    
    /* Open LSADC */
    ret = uapi_lsadc_init();
    if (ret < 0)
    {
        XLOGE("uapi_lsadc_init fail, ErrorCode=0x%x\n", ret);
    }
    ret = uapi_lsadc_get_config(&config);
    if (ret != TD_SUCCESS)
    {
        XLOGE("uapi_lsadc_get_config fail, ErrorCode=0x%x\n", ret);
    }
    mask = (config.channel_mask | (0x0f)); /* 4 groups */
    channel_mask = mask;
    config.channel_mask = channel_mask;
    config.active_bit = 0x3fc;
    config.data_delta = 0x8;
    config.deglitch_bypass = 0x1;
    config.glitch_sample = 0;
    config.lsadc_reset = 0;
    config.lsadc_zero = 0x3ff;
    config.model_sel = 0x1;
    config.power_down_mod = 0x0;
    config.time_scan = 600; /* lsacd chn scan time gap 600 us */
    ret = uapi_lsadc_set_config(&config);
    if (ret != TD_SUCCESS)
    {
        XLOGE("uapi_lsadc_set_config fail, ErrorCode=0x%x\n", ret);
    }

    ret = uapi_pq_init(PQ_BIN_PATH);
    if (ret == TD_SUCCESS)
    {
        XLOGD("uapi_pq_init successed !");
    }
    else
    {
        XLOGD("uapi_pq_init failed! ret: 0x%x", ret);
    }

    //hisi init otp service
    ret = uapi_otp_init();
    if (ret != TD_SUCCESS)
    {
        XLOGE("uapi_otp_init fail, ErrorCode=0x%x\n", ret);
    }
    else
    {
        XLOGD("uapi_otp_init success!\n");
    }

    //init consoles
    initSerialPort();
    mCheckColorTemp =true;

    //i2c lock
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        mLockIIC[i] = -1;
    }

    /* RK Indicates the port index used by HDMI out
       Unknown        = 0
       VGA            = 1
       DVII           = 2
       DVID           = 3
       DVIA           = 4
       Composite      = 5
       SVIDEO         = 6
       LVDS           = 7
       Component      = 8
       9PinDIN        = 9
       DisplayPort    = 10
       HDMIA          = 11
       HDMIB          = 12
       TV             = 13
       eDP            = 14
       VIRTUAL        = 15
       DSI            = 16
    */
    #ifdef HDMI_TX_DRM_PORT_INDEX
    XBH_CHAR strDrmPortIndex[PROPERTY_VALUE_MAX] = {0};
    sprintf(strDrmPortIndex, "%d", HDMI_TX_DRM_PORT_INDEX);
    property_set("vendor.xbh.hdmitx.drm.index", strDrmPortIndex);
    #endif
    mAudioOutput = XBH_AUDIO_OUTPUT_DET;

    XBH_CHAR Polarity[PROPERTY_VALUE_MAX] = {0};;
    if (property_get("persist.vendor.xbh.PrimaryBacklightInv", Polarity, NULL) > 0)
    {
        if(atoi(Polarity)==0 || atoi(Polarity)==1)
        {
            setPwmPolarity(atoi(Polarity) == 1,XBH_PANEL_MAIN);
        }
    }
    
    XBH_CHAR backlightMax[PROPERTY_VALUE_MAX] = {0};
    if (property_get("persist.vendor.xbh.PrimaryBacklightMax", backlightMax, NULL) > 0)
    {
        if(atoi(backlightMax)>=0 && atoi(backlightMax)<=255)
        {
            setBrightMax(atoi(backlightMax));
        }
    }
    
    XBH_CHAR backlightMin[PROPERTY_VALUE_MAX] = {0};
    if (property_get("persist.vendor.xbh.PrimaryBacklightMin", backlightMin, NULL) > 0)
    {
        if(atoi(backlightMin)>=0 && atoi(backlightMin)<=255)
        {
            setBrightMin(atoi(backlightMin));
        }
    }

    XBH_POWERMODE_E powerModeValue = XBH_POWERMODE_ON;
    XBH_CHAR powerMode[PROPERTY_VALUE_MAX] = {0};
 
    if(property_get("persist.vendor.xbh.powermode", powerMode, "0") > 0)
    {
        if(!(strcmp(powerMode, "-1") == 0))
        {
            powerModeValue = static_cast<XBH_POWERMODE_E>(atoi(powerMode));
            XbhHi_610::setPoweronMode(powerModeValue);

            property_set("persist.vendor.xbh.powermode", "-1");
        }
    }
    XLOGD(" end ");
}

