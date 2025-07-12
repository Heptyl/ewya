#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhRk_3576"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <cerrno>
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
#include "XbhRk_3576.h"
#include "XbhSysOpt.h"
#include "XbhParseINI.h"
#include "XbhCrc.h"

#include "XbhLightSensorManager.h"
#include <rockchip/hardware/outputmanager/1.0/IRkOutputManager.h>
#include "VmanModule.h"
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

extern "C" {
    #include "vendor_storage.h"
}
//kernel 默认打印等级
#ifndef ITEM_KERNEL_LOGLEVEL
#define ITEM_KERNEL_LOGLEVEL 7
#endif

#define PIP_MODE_PROPERTY "tvinput.hdmiin.buff_type"

#define XBH_HAL_CHECK_SIGNAL    "vendor.xbh.hal_check_signal"
#define XBH_INCREASE_SYS_INI "/mnt/xbhcust/systemproperty.ini"
#define XBH_INCREASE_PQ_DIR "/mnt/xbhcust/pq"

using namespace rockchip::hardware::outputmanager::V1_0;

using ::rockchip::hardware::outputmanager::V1_0::IRkOutputManager;
using ::rockchip::hardware::outputmanager::V1_0::Result;

using namespace android;

android::sp<IRkOutputManager>     mComposer = nullptr;
//----------------------------------------------------- static function begin ------------------------------------------------------------------------------------------------------
static XBH_U8 source_available[] = XBH_SRC_AVAILABLE_TABLE

map<int, string> XbhRk_3576::net_list = {
        {0, "/sys/class/net/eth0/speed"},
        {1, "/sys/class/net/eth1/speed"},
        {2, "/sys/class/net/dummy0/speed"}
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

static struct gpio_common_info gpio_common_table[] = {
#ifdef XBH_BOARD_GPIO_BOOT_COMMON_TABLE
    XBH_BOARD_GPIO_BOOT_COMMON_TABLE
#endif
};

//无效数值
#undef XBH_NO_CONFIG
#define XBH_NO_CONFIG                       9999

#define WATCHDOG_DEV_NAME "/dev/watchdog"
#define GPIO_DEV_PATH   "/dev/xbh_gpio"
#define ADC_DEV_PATH    "/sys/bus/iio/devices/iio:device0"
#define RTC_DEV_PATH    "/dev/rtc0"
#define LED_DEV_PATH    "/sys/class/leds/work/brightness"

#define BACKLIGHT_DIRECTORY        "/sys/devices/platform/backlight/backlight/backlight/"
#define BACKLIGHT_POLARITY         BACKLIGHT_DIRECTORY"polarity"
#define BACKLIGHT_MAX              BACKLIGHT_DIRECTORY"pwm_bl_max"
#define BACKLIGHT_MIX              BACKLIGHT_DIRECTORY"pwm_bl_min"
#define BACKLIGHT_DEF_BRIGHTNESS   BACKLIGHT_DIRECTORY"pwm_bl_def"
#define BACKLIGHT_BRIGHTNESS       BACKLIGHT_DIRECTORY"brightness"
#define BACKLIGHT_FREQUENCY        BACKLIGHT_DIRECTORY"freq"

#define BACKLIGHT1_DIRECTORY        "/sys/devices/platform/backlight1/backlight/backlight1/"
#define BACKLIGHT1_POLARITY         BACKLIGHT1_DIRECTORY"polarity"
#define BACKLIGHT1_MAX              BACKLIGHT1_DIRECTORY"brightMax"
#define BACKLIGHT1_MIX              BACKLIGHT1_DIRECTORY"brightMin"
#define BACKLIGHT1_BRIGHTNESS       BACKLIGHT1_DIRECTORY"brightness"
#define BACKLIGHT1_FREQUENCY        BACKLIGHT1_DIRECTORY"freq"

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

// #define MSDC_CUSDATA_PATH  "/dev/block/mmcblk1p15" //cusdata
// #define MSDC_CUSPARAM_PATH "/dev/block/mmcblk1p14" //cusparam

#define NTC_ADC_100K_0402_MIN -40
#define NTC_ADC_100K_0402_LEN 141

#define LED_ON_CMD "1"
#define LED_ON_CMD_LEN 1
#define LED_OFF_CMD "0"
#define LED_OFF_CMD_LEN 1

#define MSDC_CUSDATA_PATH  "/dev/block/by-name/cusdata" // mmcblk1p15/mmcblk0p15

#define MSDC_CUSPARAM_PATH "/dev/block/by-name/cusparam" //mmcblk1p14/mmcblk0p14
#define XBH_CUST_LOGO_PATH "/mnt/xbhcust/logo/logo.img"


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

#ifndef XBH_BOARD_NTC_ADC_THERMISTOR_CHAN;
#define XBH_BOARD_NTC_ADC_THERMISTOR_CHAN                XBH_ADC_CHANNEL_NUM_1   /*channel range: 0-7*/
#endif

#ifndef XBH_BOARD_NTC_ADC_THERMISTOR_VAL
#define XBH_BOARD_NTC_ADC_THERMISTOR_VAL                 100 /*value of resistance: 100K*/
#endif

#define ADC_COUNT           5 //ADC采样次数
#define VENDOR_SN_ID        1
#define VENDOR_RKP_ID       67


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

/**
 * 计算数组平均值（自动去除一个最大值和一个最小值）
 * @param array  输入数组指针
 * @param n      数组长度
 * @param avg    返回的平均值指针
 * @return       SUCCESS 表示成功，ERROR_INVALID_INPUT 表示无效输入
 */
 static XBH_S32 calculate_trimmed_avg(XBH_U32* array, XBH_U32 n, XBH_U32* avg)
 {
    // 输入有效性检查
    if (!array || n < 2)
    {
        return XBH_FAILURE;
    }

    XBH_U32 sum = 0;
    XBH_U32 max_val = array[0];
    XBH_U32 min_val = array[0];

    // 单次遍历找出极值并求和
    for (XBH_U32 i = 0; i < n; ++i)
    {
        sum += array[i];
        
        if (array[i] > max_val)
        {
            max_val = array[i];  // 更新最大值
        }
        if (array[i] < min_val)
        {
            min_val = array[i];  // 更新最小值
        }
    }

    // 处理所有元素相同的情况
    if (max_val == min_val)
    {
        *avg = (sum - max_val) / (n - 1);
    }
    else
    {
        // 确保剩余元素足够计算
        if (n < 2)
        {
            return XBH_FAILURE;
        }
        
        sum -= max_val + min_val;          // 去除一个最大值和一个最小值
        *avg = sum / (n - 2);      // 计算剩余元素平均值
    }

    return XBH_SUCCESS;
}

static XBH_COLORTEMP_E checkinitColorTempMode(const XBH_CHAR* ColorTempPara)
{
    if (strcmp(ColorTempPara,"XBH_COLORTEMP_STANDARD") == 0)
    {
        return XBH_COLORTEMP_STANDARD;
    }
    else if (strcmp(ColorTempPara,"XBH_COLORTEMP_WARM") == 0)
    {
        return XBH_COLORTEMP_WARM;
    }
    else if (strcmp(ColorTempPara,"XBH_COLORTEMP_COLD") == 0)
    {
        return XBH_COLORTEMP_COLD;
    }
    else if (strcmp(ColorTempPara,"XBH_COLORTEMP_USER") == 0)
    {
        return XBH_COLORTEMP_USER;
    }
    else
    {
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
XBH_S32 XbhRk_3576::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
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
        XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = setMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, u32GpioValue);
        return s32Ret;
    }
#endif

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;
    //rk android 14 GKI 不支持gpiod_get_direction接口，即无法获取GPIO的输入输出状态
    //此处不再判断，改成直接设置
#if 0
    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        XLOGV("-- gpio %d is not output pin set output now", u32GpioNumber);
        s32Ret = setGpioOutput(u32GpioNumber, u32GpioValue);
        if(s32Ret < 0)
        {
            XLOGE("-- set gpio-%d out value : %d fail --",userdata.gpio,userdata.state);
            return XBH_FAILURE;
        }
        else
        {
            return s32Ret;
        }
    }
#endif
    setGpioOutput(u32GpioNumber, u32GpioValue);

    s32Ret = ioctl(mGpioFd, SET_IO_VAL, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- set gpio-%d out value : %d fail --",userdata.gpio,userdata.state);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
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
            XLOGE("MCU is upgrading, getGpioOutputValue(MCU GPIO) has been disabled\n");
            return XBH_FAILURE;
        }

        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_OUTPUT, u32GpioValue);
        return s32Ret;
    }
#endif

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = XBH_BOARD_GPIO_OUTPUT;
    //rk android 14 GKI 不支持gpiod_get_direction接口，即无法获取GPIO的输入输出状态
    //此处不再判断，改成直接获取
#if 0
    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT){
        XLOGV("-- gpio %d is not output pin set output now",u32GpioNumber);
        userdata.state = 0;
        s32Ret = ioctl(mGpioFd, GET_IO_VAL, &userdata);
        if(s32Ret < 0){
            XLOGE("-- get gpio-%d value fail --",userdata.gpio);
            return XBH_FAILURE;
        }
        setGpioOutput(userdata.gpio, userdata.state);
        *u32GpioValue = userdata.state;
        return s32Ret;
    }
#endif
    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_OUT_VAL, &userdata);
    if(s32Ret < 0){
        XLOGE("-- get gpio-%d out value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *u32GpioValue = userdata.state;
    return s32Ret;

}

XBH_S32 XbhRk_3576::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;

#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        XLOGE("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_INPUT, u32GpioValue);
        return s32Ret;
    }
#endif

    if(mGpioFd < 0){
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = XBH_BOARD_GPIO_INPUT;
    //rk android 14 GKI 不支持gpiod_get_direction接口，即无法获取GPIO的输入输出状态
    //此处不再判断，改成直接获取
#if 0
    if(getGpioDir(u32GpioNumber) == XBH_BOARD_GPIO_OUTPUT){
        XLOGV("-- gpio %d is not input pin set output now",u32GpioNumber);
        setGpioInput(userdata.gpio);
    }
#endif
    setGpioInput(userdata.gpio);
    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_VAL, &userdata);
    if(s32Ret < 0){
        XLOGE("-- get gpio-%d value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *u32GpioValue = userdata.state;
    return s32Ret;
}

XBH_S32 XbhRk_3576::requestGpioIrq(XBH_U32 u32GpioNumber, XBH_U32 u32Type, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32Type;
    userdata.jitter = enable;

    s32Ret = ioctl(mGpioFd, REQUEST_IRQ, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- set gpio-%d irq : %d fail --", userdata.gpio, userdata.state);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::freeAllGpioIrq()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int freestatus = 1;
    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    s32Ret = ioctl(mGpioFd, FREE_ALL_IRQ, &freestatus);
    if(s32Ret < 0)
    {
        XLOGE("-- free all irq fail -- \n");
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::setPanelRotation(XBH_S32 value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    sprintf(propVal,"%d",(value*90));
    property_set("persist.sys.hwrotation", propVal);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setIICLock(XBH_U32 iicBus, XBH_BOOL lock)
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

XBH_S32 XbhRk_3576::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        if (mLockIIC[i] == u32I2cNum)
        {
            return XBH_LOCKSTATUS;
        }
    }
    s32Ret = set_i2c_data(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        if (mLockIIC[i] == u32I2cNum)
        {
            return XBH_LOCKSTATUS;
        }
    }
    s32Ret = get_i2c_data(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable)
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

XBH_S32 XbhRk_3576::setDebugEnable(XBH_BOOL bEnable)
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
        property_set("ctl.start", "console");
        XbhSysOpt::writeSysfs("/proc/sysrq-trigger", "7");
    }
    else
    {
        property_set("ctl.stop", "console");
        #if(ITEM_KERNEL_LOGLEVEL != 0)
        XbhSysOpt::writeSysfs("/proc/sysrq-trigger", "0");
        #endif
    }
    return s32Ret;

}

XBH_S32 XbhRk_3576::getDebugEnable(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.rs232.enable", propVal, "false") > 0)
    {
        if(strcmp(propVal, "false") == 0)
        {
            *bEnable = XBH_TRUE;
        }
        else
        {
            *bEnable = XBH_FALSE;
        }
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    int fp = 0;
    char buf[10] = {0};
    char path[128] = {0};
    XBH_S32 s32Ret = 0;
    XBH_U32 temp[ADC_COUNT] = {0};
    sprintf(path, "%s/in_voltage%d_raw", ADC_DEV_PATH, enChannel);
    //XLOGD("[%s:%d] Get Adc path =%s\r\n", __func__, __LINE__, path);
    for(int i = 0; i < ADC_COUNT; i++)
    {
        fp = open(path, O_RDONLY);
        if(fp == -1)
        {
            XLOGE("open ADC_DEV_PATH error!\n");
            return XBH_FAILURE;
        }
        memset(buf, 0, sizeof(buf));
        s32Ret = read(fp, buf, 10);
        if(s32Ret == -1)
        {
            XLOGE("read ADC_DEV_PATH error!\n");
            close(fp);
            return XBH_FAILURE;
        }
        close(fp);
        temp[i] = atoi(buf);
        //XLOGD("[%s:%d] Get Adc temp[%d] =%d\r\n", __func__, __LINE__, i, temp[i]);
    }
    calculate_trimmed_avg(temp, ADC_COUNT, u32Value);
    //XLOGD("[%s:%d] Get Adc voltage value = %d \r\n", __func__, __LINE__, *u32Value);
    return ret;
}

XBH_S32 XbhRk_3576::getTemperatureSensorValue(XBH_FLOAT *fValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_ADC_CHANNEL_NUM_E adcChannel = (XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_NTC_ADC_THERMISTOR_CHAN;
    XBH_U32 adcValue   = 0;
    s32Ret = getADCChannelValue(adcChannel, &adcValue);

    XBH_U32 value = adcValue;
    //XLOGD("getBoardTemperature : adcValue = %d", adcValue);
    if ((4095 - value) > 0)
    {
        XBH_U32 tAdcVal = XBH_BOARD_NTC_ADC_THERMISTOR_VAL * 1000 * value / (4095 - value);
        XBH_S32 tempIndex = 0;
        for(tempIndex=0; tempIndex < NTC_ADC_100K_0402_LEN; tempIndex++)
        {
            if(tAdcVal > NTC_ADC_100K_0402[tempIndex])
            {
               *fValue = tempIndex - 40;
                //XLOGD("getBoardTemperature : ADC-CH%d = %d  temp = %f ", adcChannel, tempIndex, *fValue);
                s32Ret = XBH_SUCCESS;
                break;
            }
        }
        if(tempIndex >= NTC_ADC_100K_0402_LEN)
        {
            *fValue = 36.0;
            XLOGE("getBoardTemp: Invalid value = %d", value);
        }
    }
    else
    {
        XLOGE("getBoardTemp: get adc failed!!! value = %d", value);
        *fValue = 36.0;
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::setRtcAlarmTime(XBH_RTC_ALARM_INFO_S *stRtcAlarmInfo)
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

XBH_S32 XbhRk_3576::getRtcTime(XBH_RTC_INFO_S *stRtcInfo)
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

XBH_S32 XbhRk_3576::setRtcTime(XBH_RTC_INFO_S *stRtcInfo)
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

XBH_S32 XbhRk_3576::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 ledFd = -1;
    ledFd = open(LED_DEV_PATH, O_RDWR);
    if (ledFd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        return s32Ret;
    }
    if (enState == XBH_LED_LIGHT_GREEN)
    {
        s32Ret = write(ledFd, LED_ON_CMD, LED_ON_CMD_LEN);
    }
    else if (enState == XBH_LED_LIGHT_RED)
    {
        s32Ret = write(ledFd, LED_OFF_CMD, LED_OFF_CMD_LEN);
    }
    else
    {
        XLOGE("%s not support color \n", __func__);
    }
    close(ledFd);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setBrightMax(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char tmp[32];
    const char *backlightPath;
    if (access(BACKLIGHT_MAX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT_MAX;
    }
    else if (access(BACKLIGHT1_MIX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT1_MIX;
    }
    else
    {
        return s32Ret;
    }

    fd = open(backlightPath, O_RDWR);
    if (fd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        close(fd);
        return s32Ret;
    }
    sprintf(tmp, "%d", u32Value);
    s32Ret = write(fd, tmp, strlen(tmp));
    if (s32Ret < 0) {
        ALOGE("Error writing to %s: %s\n", backlightPath, tmp);
        close(fd);
        return s32Ret;
    }
    property_set("persist.vendor.xbh.PrimaryBacklightMax", tmp);
    close(fd);
    s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 XbhRk_3576::getBrightMax(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char tmp[32];
    const char *backlightPath;
    if (access(BACKLIGHT_MAX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT_MAX;
    }
    else if (access(BACKLIGHT1_MIX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT1_MIX;
    }
    else
    {
        return s32Ret;
    }
    fd = open(backlightPath, O_RDONLY);
    if (fd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        close(fd);
        return s32Ret;
    }

    s32Ret = read(fd, tmp, 10);
    if(s32Ret == -1)
    {
        XLOGE("read ADC_DEV_PATH error!\n");
        close(fd);
        return XBH_FAILURE;
    }

    *u32Value = atoi(tmp);
    
    close(fd);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setBrightMin(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char tmp[32];
    const char *backlightPath;
    if (access(BACKLIGHT_MIX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT_MIX;
    }
    else if (access(BACKLIGHT1_MIX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT1_MIX;
    }
    else
    {
        return s32Ret;
    }
        
    fd = open(backlightPath, O_RDWR);
    if (fd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        close(fd);
        return s32Ret;
    }
    sprintf(tmp, "%d", u32Value);
    s32Ret = write(fd, tmp, strlen(tmp));
    if (s32Ret < 0)
    {
        ALOGE("Error writing to %s: %s\n", backlightPath, tmp);
        close(fd);
        return s32Ret;
    }
    property_set("persist.vendor.xbh.PrimaryBacklightMin", tmp);
    
    close(fd);
    s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 XbhRk_3576::getBrightMin(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char tmp[32];
    const char *backlightPath;
    if (access(BACKLIGHT_MIX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT_MIX;
    }
    else if (access(BACKLIGHT1_MIX, F_OK) == 0)
    {
        backlightPath = BACKLIGHT1_MIX;
    }
    else
    {
        return s32Ret;
    }

    fd = open(backlightPath, O_RDONLY);
    if (fd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        close(fd);
        return s32Ret;
    }
    s32Ret = read(fd, tmp, 10);
    if(s32Ret == -1)
    {
        XLOGE("read getBrightMin error!\n");
        close(fd);
        return XBH_FAILURE;
    }

    *u32Value = atoi(tmp);
    close(fd);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char tmp[32];
    const char *backlightPath;
    if (access(BACKLIGHT_POLARITY, F_OK) == 0)
    {
        backlightPath = BACKLIGHT_POLARITY;
    }
    else if (access(BACKLIGHT1_POLARITY, F_OK) == 0)
    {
        backlightPath = BACKLIGHT1_POLARITY;
    }
    else
    {
         return s32Ret;
    }

    fd = open(backlightPath, O_RDWR);
    if (fd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        return s32Ret;
    }
    sprintf(tmp, "%d", bEnable ? 1:0);
    s32Ret = write(fd, tmp, strlen(tmp));
    if (s32Ret < 0)
    {
        ALOGE("Error writing to %s: %s\n", backlightPath, tmp);
        close(fd);
        return s32Ret;
    }
    property_set("persist.vendor.xbh.PrimaryBacklightInv", tmp);

    close(fd);
    s32Ret = XBH_SUCCESS;
    return s32Ret;

}

XBH_S32 XbhRk_3576::getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    char tmp[32];
    const char *backlightPath;

    if (access(BACKLIGHT_POLARITY, F_OK) == 0)
    {
        backlightPath = BACKLIGHT_POLARITY;
    }
    else if (access(BACKLIGHT1_POLARITY, F_OK) == 0)
    {
        backlightPath = BACKLIGHT1_POLARITY;
    }
    else
    {
         return s32Ret;
    }

    fd = open(backlightPath, O_RDONLY);
    if (fd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        return s32Ret;
    }

    s32Ret = read(fd, tmp, 10);
    if(s32Ret == -1)
    {
        XLOGE("read getPwmPolarity error!\n");
        close(fd);
        return XBH_FAILURE;
    }

    *bEnable = (atoi(tmp)==1);
    close(fd);
    return s32Ret;
}


XBH_S32 XbhRk_3576::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 ledFd = -1;
    XBH_CHAR state[2] = {0};
    ledFd = open(LED_DEV_PATH, O_RDONLY);
    if (ledFd  < 0)
    {
        XLOGE("%s open led dev error \n", __func__);
        return s32Ret;
    }
    s32Ret = read(ledFd, state, 1);
    if(strncmp(state, LED_ON_CMD, LED_ON_CMD_LEN) == 0)
    {
        *enState = XBH_LED_LIGHT_GREEN;
    }
    else if(strncmp(state, LED_OFF_CMD, LED_OFF_CMD_LEN) == 0)
    {
        *enState = XBH_LED_LIGHT_RED;
    }
    close(ledFd);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getWoTStatus(XBH_WAKEUP_E enWakeUpType ,XBH_BOOL *enable)
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
        case XBH_WAKEUP_CEC:
        {
            *enable = s8Buff[XBH_WAKEUP_CEC];
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

XBH_S32 XbhRk_3576::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
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

XBH_S32 XbhRk_3576::setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel)
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
    s32Ret = XbhPartitionData::getInstance()->setMultiPanel((XBH_VOID *)s8Buff);
    XLOGD("set MultiPanel %d \n", s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getMultiPanel((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] == 2 ? 1 : 0;
    return s32Ret;
}

XBH_S32 XbhRk_3576::setPanelMapSwap(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    s8Buff[0] = type;
    s32Ret = XbhPartitionData::getInstance()->setPanelMapSwap((XBH_VOID *)s8Buff);
    XLOGD("set MultiPanel %d \n", s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getPanelMapSwap(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getPanelMapSwap((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] ;
    return s32Ret;
}

XBH_S32 XbhRk_3576::setUsbcConfig(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_USBC_CONFIG_LEN] = {0};
    s8Buff[0] = type;
    XLOGD("setUsbcConfig %d \n", s8Buff[0]);
    s32Ret = XbhPartitionData::getInstance()->setUsbcConfig((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getUsbcConfig(XBH_S32* type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_USBC_CONFIG_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getUsbcConfig((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] ;
     XLOGD("getUsbcConfig %d \n", *type);
    return s32Ret;
}


XBH_S32 XbhRk_3576::setIndependenceSourceEdidType(XBH_U32 source,XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN] = {0};
    s8Buff[0]= (XBH_CHAR)type;
    s32Ret = XbhPartitionData::getInstance()->setIndependenceSourceParamValue(source,(XBH_VOID *)s8Buff);
    //XLOGD("set setHdmirxEdidType %d \n",s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getIndependenceSourceEdidType(XBH_U32 source,XBH_S32 *type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getIndependenceSourceParamValue(source,(XBH_VOID *)s8Buff);
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

XBH_S32 XbhRk_3576::setHdmirxEdidType(XBH_S32 type)
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
    s32Ret = XbhPartitionData::getInstance()->setHdmiEdidParamValue((XBH_VOID *)s8Buff);
    XLOGD("set setHdmirxEdidType %d \n",s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getHdmirxEdidType(XBH_S32 *type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getHdmiEdidParamValue((XBH_VOID *)s8Buff);
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

XBH_S32 XbhRk_3576::getSourceEdidType(XBH_U32 u32SourceId, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getSourceEdidType no imp! \n");
    return s32Ret;
}

XBH_S32 XbhRk_3576::setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setSourceEdidType no imp! \n");
    return s32Ret;
}

XBH_S32 XbhRk_3576::isNfcExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getChipExist(enable);
    XLOGW("isNfcExist s32Ret = %d", s32Ret);
   return s32Ret;
}

XBH_S32 XbhRk_3576::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->setNfcEnable(enable);
    XLOGW("setNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getNfcEnable(enable);
    XLOGW("getNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getNfcId(data, length);
    XLOGW("getNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->addNfcId(data, length);
    XLOGW("addNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delNfcId(data, length);
    XLOGW("delNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delAllNfcId();
    XLOGW("delAllNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}
XBH_S32 XbhRk_3576::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuFattMode(mode);
    XLOGW("setMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuFattMode(mode);
    XLOGW("getMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuUartBypassPara(u8UartNum, baud);
    XLOGW("setMcuUartBypassPara s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuUartBypass(u8Len, u8Data);
    XLOGW("setMcuUartBypass s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuUartBypass(u8Len, u8Data);
    XLOGW("getMcuUartBypass s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhRk_3576::saveFattKeypadInfo(XBH_S32 keypad_enable, XBH_S32 key_count, XBH_S32 key_index)
{
    mKeypadEnable = keypad_enable;
    mKeypadCount = key_count;
    mKeypadIndex = key_index;
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::setFattKeypadenbale(XBH_S32 keypad_enable)
{
    XbhKernelMsgManager::getInstance()->sendMsgToKernel(1, keypad_enable, 0, 0, 0);
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getFattKeypadInfo(XBH_S32* keypad_enable, XBH_S32* key_count, XBH_S32* key_index)
{
    *keypad_enable = mKeypadEnable;
    *key_count = mKeypadCount;
    *key_index = mKeypadIndex;
    XLOGW("getFattKeypadInfo %d :%d :%d", *keypad_enable, *key_count, *key_index);
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    uint8_t buff[XBH_CUSPARAM_WAKEUP_INFO_LEN] = {0};
    struct WakeUp_Info wakedata;
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhPartitionData::getInstance()->getWakeupInfoParamValue(buff);
    wakedata.wakeup_flag = buff[0];
    wakedata.source = (buff[1] >> 4) - 1;
    wakedata.data = (uint16_t)(buff[3] * 256 | buff[2]) & 0xffff;
    XLOGD("wakeup data flag:%d source:%d data:%d \n", wakedata.wakeup_flag, wakedata.source, wakedata.data);
    if (wakedata.wakeup_flag == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_BUTT;
        XLOGD("getWakeUpReason is reboot \n");
        return XBH_FAILURE;
    }
    if (wakedata.source == WAKEUP_CEC)
    {
        stWake->enWakeup = XBH_WAKEUP_CEC;
        switch(wakedata.data)
        {
#ifdef XBH_WAKE_UP_HDMI1_DET
            case 0x1000:
            {
                stWake->enSrc = XBH_SOURCE_HDMI1;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_HDMI2_DET
            case 0x2000:
            {
                stWake->enSrc = XBH_SOURCE_HDMI2;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_F_HDMI_DET
            case 0x3000:
            {
                stWake->enSrc = XBH_SOURCE_F_HDMI1;
                break;
            }
#endif
        }
    }
    else if (wakedata.source == WAKEUP_IR)
    {
        stWake->enWakeup = XBH_WAKEUP_IR;
    }
    else if (wakedata.source == WAKEUP_UART)
    {
        stWake->enWakeup = XBH_WAKEUP_UART;
    }
    else if (wakedata.source == WAKEUP_GPIO)
    {
        switch(wakedata.data)
        {
#ifdef XBH_WAKE_UP_POWER_KEY
            case XBH_WAKE_UP_POWER_KEY:
            {
                stWake->enWakeup = XBH_WAKEUP_KEYPAD;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_ETH
            case XBH_WAKE_UP_ETH:
            {
                stWake->enWakeup = XBH_WAKEUP_ETH;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_MCU_IRQ
            case XBH_WAKE_UP_MCU_IRQ:
            {
                stWake->enWakeup = XBH_WAKEUP_NFC;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_PIR2
            case XBH_WAKE_UP_PIR2:
            {
                stWake->enWakeup = XBH_WAKEUP_PIR;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_PIR1
            case XBH_WAKE_UP_PIR1:
            {
                stWake->enWakeup = XBH_WAKEUP_PIR;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_HDMI1_DET
            case XBH_WAKE_UP_HDMI1_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_HDMI1;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_HDMI2_DET
            case XBH_WAKE_UP_HDMI2_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_HDMI2;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_DP_DET
            case XBH_WAKE_UP_DP_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_DP1;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_TYPE_C_DET
            case XBH_WAKE_UP_TYPE_C_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_USBC1;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_F_HDMI_DET
            case XBH_WAKE_UP_F_HDMI_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_F_HDMI1;
                break;
            }
#endif

#ifdef XBH_WAKE_UP_F_TYPE_C_DET
            case XBH_WAKE_UP_F_TYPE_C_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_F_USBC1;
                break;
            }
#endif
#ifdef XBH_WAKE_UP_VGA1_DET
            case XBH_WAKE_UP_VGA1_DET:
            {
                stWake->enWakeup = XBH_WAKEUP_SOURCE;
                stWake->enSrc = XBH_SOURCE_VGA1;
                break;
            }
#endif
        }
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::setShutdownFlag(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_SHUTDOWN_FLAG_LEN] = {0};
    XLOGD("setShutdownFlag : %d\n", enable);
    s8Buff[0] = (XBH_CHAR)enable;
    s32Ret = XbhPartitionData::getInstance()->setShutdownFlagParamValue((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setPoweronMode(XBH_POWERMODE_E enMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_POWER_ON_MODE_LEN] = {0};
    XLOGD("setPoweronMode : %d\n", enMode);
    s8Buff[0] = (XBH_CHAR)enMode;
    s32Ret = XbhPartitionData::getInstance()->setPowerModeParamValue((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getPoweronMode(XBH_POWERMODE_E *enMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_POWER_ON_MODE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getPowerModeParamValue((XBH_VOID *)s8Buff);
    *enMode = (XBH_POWERMODE_E)s8Buff[0];
    XLOGD("getPoweronMode : %d\n", *enMode);
    return s32Ret;
}

XBH_S32 XbhRk_3576::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    switch (enPanel)
    {
        case XBH_PANEL_MAIN:
            #if (XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_MAIN_BL_ON_PWR > 0)
            if (bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_BL_ON_PWR_ON : XBH_BOARD_GPIO_MAIN_BL_ON_PWR_OFF);
            }
            else
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_BL_ON_PWR_ON : XBH_BOARD_GPIO_MAIN_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_MAIN_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_1:
            #if (XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_1_BL_ON_PWR > 0)
            if (bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_OFF);
            }
            else
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_1_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_1_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_2:
            #if (XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_2_BL_ON_PWR > 0)
            if (bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_OFF);
            }
            else
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_2_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_2_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_3:
            #if (XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_3_BL_ON_PWR > 0)
            if (bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_OFF);
            }
            else
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_3_BL_ON_PWR_OFF);
                usleep(50*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_3_LCD_TCON_PWR_OFF);
            }
            #endif
            break;

        case XBH_PANEL_SUB_4:
            #if (XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR > 0) && (XBH_BOARD_GPIO_SUB_4_BL_ON_PWR > 0)
            if (bEnable)
            {
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR_ON : XBH_BOARD_GPIO_SUB_4_LCD_TCON_PWR_OFF);
                usleep(500*1000);
                s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_SUB_4_BL_ON_PWR, bEnable ? XBH_BOARD_GPIO_SUB_4_BL_ON_PWR_ON : XBH_BOARD_GPIO_SUB_4_BL_ON_PWR_OFF);
            }
            else
            {
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

    if(s32Ret < 0)
    {
        XLOGE("-- set PanelPowerGpio fail --\n");
    }

    return s32Ret;
}

XBH_S32 XbhRk_3576::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;

    switch (enPanel)
    {
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

    if(s32Ret < 0)
    {
        XLOGE("-- get PanelPowerGpio fail --\n");
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::getSocHdmiEdidStatus(XBH_BOOL *enable)
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

XBH_S32 XbhRk_3576::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->getVgaEdidCheckSum(idx, checksum);
    return s32Ret;
}

XBH_S32 XbhRk_3576::dumpEdid(XBH_SOURCE_E idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->dumpEdid();
    return s32Ret;
}

XBH_S32 XbhRk_3576::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhVgaEdidManager::getInstance()->setVgaEdid(strPath, idx);
    return s32Ret;
}
//----------------------------------------------------------------------------- private func start here -----------------------------------------------------------------
XBH_S32 XbhRk_3576::open_i2c_dev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet)
{
    XBH_S32 file, len;
    len = snprintf(filename, size, "/dev/i2c/%d", i2cbus);
    if (len >= (XBH_U32)size)
    {
        XLOGE(" == %s: path truncated\n", filename);
        return XBH_FAILURE;
    }

    file = open(filename, O_RDWR);
    if (file < 0 && (errno == ENOENT || errno == ENOTDIR))
    {
        len = snprintf(filename, size, "/dev/i2c-%d", i2cbus);
        if (len >= (XBH_U32)size)
        {
            XLOGE(" == %s: path truncated\n", filename);
            return XBH_FAILURE;
        }
        file = open(filename, O_RDWR);
    }

    if (file < 0 && !quiet)
    {
        if (errno == ENOENT)
        {
            XLOGE("Error: Could not open file /dev/i2c-%d or /dev/i2c/%d : %s\n",i2cbus, i2cbus, strerror(ENOENT));
        }
        else
        {
            XLOGE("Error: Could not open file %s : %s\n", filename, strerror(errno));
            if (errno == EACCES)
            {
                XLOGE("Run as root?\n");
            }
        }
    }
    return file;
}

XBH_S32 XbhRk_3576:: check_iic_funcs(XBH_S32 file)
{
    XBH_ULONG funcs;
    /* check adapter functionality */
    if (ioctl(file, I2C_FUNCS, &funcs) < 0)
    {
        XLOGE("Error: Could not get the adapter \n");
        return XBH_FAILURE;
    }

    if (!(funcs & I2C_FUNC_I2C))
    {
        XLOGE("I2C transfers");
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::set_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 i = 0;
    XBH_U8 regAddr[4];
    for (i = 0; i < u32RegAddrCount; i++)
    {
        regAddr[i] = (XBH_U8)((u32RegAddr >> (8 * (u32RegAddrCount -1 - i))) & 0xff);
        //XLOGD("regAddr[%d]: 0x%x\n", i, regAddr[i]);
    }
    //XLOGV("i2cNum=0x%x,devAddress=0x%x,regAddrValue=0x%x,regAddrCount=0x%x,length=0x%x\n",
    //             (S32)u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length);
    struct i2c_rdwr_ioctl_data data;
    XBH_S32 fd;
    XBH_UCHAR Addrbuff[1024];
    XBH_CHAR filename[20];
    fd = open_i2c_dev(u32I2cNum, filename, sizeof(filename), 0);
    if (fd < 0 || check_iic_funcs(fd) < 0)
    {
            return XBH_FAILURE;
    }

    data.nmsgs = 1;
    data.msgs = (struct i2c_msg *)malloc(data.nmsgs *sizeof(struct i2c_msg));
    if (data.msgs == NULL)
    {
        XLOGE("malloc error");
        close(fd);
        return XBH_FAILURE;
    }
    ioctl(fd, I2C_TIMEOUT, 2);
    ioctl(fd, I2C_RETRIES, 1);
    memset(Addrbuff, 0, 1024);

    switch (u32RegAddrCount)
    {
        case 1:
            Addrbuff[0] = regAddr[0];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 1, u8Data, u32Length);
                data.msgs[0].len = u32Length + 1;
            }
            else
            {
                data.msgs[0].len = 1;
            }
            break;
        case 2:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 2, u8Data, u32Length);
                data.msgs[0].len = u32Length + 2;
            }
            else
            {
                data.msgs[0].len = 2;
            }
            break;
        case 3:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 3, u8Data, u32Length);
                data.msgs[0].len = u32Length + 3;
            }
            else
            {
                data.msgs[0].len = 3;
            }
            break;
        case 4:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            Addrbuff[3] = regAddr[3];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 4, u8Data, u32Length);
                data.msgs[0].len = u32Length + 4;
            }
            else
            {
                data.msgs[0].len = 4;
            }
            break;
        default:
            close(fd);
            free(data.msgs);
            XLOGE("Error RegAddr Count n");
            return XBH_FAILURE;
    }
    data.msgs[0].addr = u8DevAddress;
    data.msgs[0].flags = 0;     // 0: write 1:read
    data.msgs[0].buf = Addrbuff;
    s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
    if (s32Ret < 0)
    {
        XLOGE("I2cNum : %d - 0x%x write reg 0x%x = 0x%x error : %d\r\n",u32I2cNum, u8DevAddress, u32RegAddr, *u8Data, s32Ret);
        close(fd);
        free(data.msgs);
        return XBH_FAILURE;
    }
    free(data.msgs);
    close(fd);

    return  XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::get_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{

    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 i = 0;
    XBH_U8 regAddr[4];
    for (i = 0; i < u32RegAddrCount; i++)
    {
        regAddr[i] = (XBH_U8)((u32RegAddr >> (8 * (u32RegAddrCount -1 - i))) & 0xff);
        //XLOGD("regAddr[%d]: 0x%x\n", i, regAddr[i]);
    }
    struct i2c_rdwr_ioctl_data data;
    XBH_S32 fd = -100;
    XBH_UCHAR Addrbuff[4];
    XBH_CHAR filename[20];

    fd = open_i2c_dev(u32I2cNum, filename, sizeof(filename), 0);

    if (fd < 0 || check_iic_funcs(fd) < 0)
    {
        XLOGE("fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
        return XBH_FAILURE;
    }

    data.nmsgs = 2;
    data.msgs = (struct i2c_msg *)malloc(data.nmsgs *sizeof(struct i2c_msg));
    if (data.msgs == NULL)
    {
        XLOGE("malloc error");
        close(fd);
        return XBH_FAILURE;
    }
    ioctl(fd, I2C_TIMEOUT, 2);
    ioctl(fd, I2C_RETRIES, 1);

    memset(Addrbuff, 0, 4);

    switch (u32RegAddrCount)
    {
        case 1:
            Addrbuff[0] = regAddr[0];
            data.msgs[0].len =  1;
            break;
        case 2:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            data.msgs[0].len = 2;
            break;
        case 3:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            data.msgs[0].len = 3;
            break;
        case 4:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            Addrbuff[3] = regAddr[3];
            data.msgs[0].len =  4;
            break;
        default:
            close(fd);
            free(data.msgs);
            XLOGE("Error RegAddr Count n");
            return XBH_FAILURE;
    }

    data.msgs[0].addr = u8DevAddress;
    data.msgs[0].flags = !I2C_M_RD;     // 0: write 1:read
    data.msgs[0].buf = Addrbuff;
    //read data
    data.msgs[1].len = u32Length;
    data.msgs[1].addr = u8DevAddress;
    data.msgs[1].flags = I2C_M_RD;    // 0: write 1:read
    data.msgs[1].buf = u8Data;

    s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
    if (s32Ret < 0)
    {
        XLOGE("I2cNum : %d - 0x%x read reg 0x%x error : %d\r\n",u32I2cNum, u8DevAddress, u32RegAddr, s32Ret);
        close(fd);
        free(data.msgs);
        return XBH_FAILURE;
    }
    free(data.msgs);
    close(fd);
    return XBH_SUCCESS;
}

XBH_U8 XbhRk_3576::calMcuGpioChecksum(XBH_MCU_GPIOPARA_U *gpioPara)
{
    return (gpioPara->attr.port + gpioPara->attr.pin + gpioPara->attr.mode + gpioPara->attr.pupd + gpioPara->attr.otype + gpioPara->attr.speed + gpioPara->attr.level) & 0xff;
}

XBH_S32 XbhRk_3576::setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_MCU_GPIOPARA_U unGpioGet;
    XBH_MCU_I2CBUFFDEF_S stInData;
    if ((0xff == u8Group) || (0xff == u8GpioNum))
    {
        XLOGE("-- mcu gpio port and pin error \n");
        return XBH_FAILURE;
    }

    unGpioGet.attr.mode = MCU_GPIO_MODE_OUTPUT;
    stInData.cmd = CMD_I2C_SET_GPIO_OUTPUT;
    unGpioGet.attr.port = u8Group;
    unGpioGet.attr.pin = u8GpioNum;
    unGpioGet.attr.speed = MCU_GPIO_OSPEED_10MHZ;
    unGpioGet.attr.pupd = MCU_GPIO_PUPD_NONE;
    unGpioGet.attr.otype = MCU_GPIO_OTYPE_PP;
    unGpioGet.attr.level = level;
    unGpioGet.attr.checksum = calMcuGpioChecksum(&unGpioGet);
    unGpioGet.attr.reserved = 0x00;
    stInData.len = sizeof(XBH_MCU_GPIOPARA_U);
    stInData.data[0]  = (unGpioGet.value >> 24) & 0xFF;
    stInData.data[1]  = (unGpioGet.value >> 16) & 0xFF;
    stInData.data[2]  = (unGpioGet.value >> 8) & 0xFF;
    stInData.data[3]  = unGpioGet.value & 0xFF;
#if defined(XBH_MCU_I2C_CHN) && defined(XBH_MCU_I2C_ADDR)
    ret = set_i2c_data(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    XLOGD("--set_i2c_data cmd: %02x, len: %d, data %02x:%02x:%02x:%02x \n", stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
#endif
    return ret;
}

XBH_S32 XbhRk_3576::getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_MCU_GPIOPARA_U unGpioGet;
    XBH_MCU_I2CBUFFDEF_S stInData;
    if ((0xff == u8Group) || (0xff == u8GpioNum))
    {
        XLOGE("-- mcu gpio port and pin error \n");
        return XBH_FAILURE;
    }

    if (mode == MCU_GPIO_MODE_OUTPUT)
    {
        unGpioGet.attr.mode = MCU_GPIO_MODE_OUTPUT;
        stInData.cmd = CMD_I2C_GET_GPIO_OUTPUT;
    }
    else
    {
        unGpioGet.attr.mode = MCU_GPIO_MODE_INPUT;
        stInData.cmd = CMD_I2C_GET_GPIO_INPUT;
    }
    unGpioGet.attr.port = u8Group;
    unGpioGet.attr.pin = u8GpioNum;
    unGpioGet.attr.speed = MCU_GPIO_OSPEED_10MHZ;
    unGpioGet.attr.pupd = MCU_GPIO_PUPD_NONE;
    unGpioGet.attr.otype = MCU_GPIO_OTYPE_PP;
    unGpioGet.attr.checksum = calMcuGpioChecksum(&unGpioGet);
    unGpioGet.attr.reserved = 0x00;
    stInData.len = sizeof(XBH_MCU_GPIOPARA_U);
    stInData.data[0]  = (unGpioGet.value >> 24) & 0xFF;
    stInData.data[1]  = (unGpioGet.value >> 16) & 0xFF;
    stInData.data[2]  = (unGpioGet.value >> 8) & 0xFF;
    stInData.data[3]  = unGpioGet.value & 0xFF;
#if defined(XBH_MCU_I2C_CHN) && defined(XBH_MCU_I2C_ADDR)
    ret = set_i2c_data(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    XLOGD("--set_i2c_data cmd: %02x, len: %d, data %02x:%02x:%02x:%02x \n", stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
    ret |= get_i2c_data(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    if (ret >= 0)
    {
        unGpioGet.value = stInData.data[0] << 24;
        unGpioGet.value |= stInData.data[1] << 16;
        unGpioGet.value |= stInData.data[2] << 8;
        unGpioGet.value |= stInData.data[3];
        *level = unGpioGet.attr.level;
    }
    else
    {
        XLOGE("--%s I2C ERROR \n",__func__);
    }
#endif
    return ret;
}

XBH_S32 XbhRk_3576::initSerialPort()
{
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.rs232.enable", propVal, "false") > 0)
    {
        if(strcmp(propVal, "true") == 0)
        {
            setDebugEnable(XBH_FALSE);
        }
        else
        {
            setDebugEnable(XBH_TRUE);
        }
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}

XBH_S32 XbhRk_3576::setGpioOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;

    s32Ret = ioctl(mGpioFd, SET_IO_OUT, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- set gpio-%d out value : %d fail --",userdata.gpio, userdata.state);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::setGpioInput(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;

    s32Ret = ioctl(mGpioFd, SET_IO_IN, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- set gpio-%d input : %d fail --",userdata.gpio);
        return XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhRk_3576::getGpioDir(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;

    s32Ret = ioctl(mGpioFd, GET_IO_DIR, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- get gpio-%d dir fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    return userdata.state;
}


XBH_S32 XbhRk_3576::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::loadProperty()
{
    int entryCount = 0;
    XBH_S32 flag = 0;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XLOGD("%s %d loadProperty start !!!! \n", __func__, __LINE__);
    IniEntry* entries = XbhParseINI::parseINI("/mnt/xbhcust/systemproperty.ini", &entryCount);
    for (int i = 0; i < entryCount; i++)
    {
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

XBH_S32 XbhRk_3576::setDeviceId(const XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->setDeviceId(strDeviceId);
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getDeviceId(XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->getDeviceId(strDeviceId);
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

//Set serial number
XBH_S32 XbhRk_3576::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL enable;
    s32Ret  = XbhPartitionData::getInstance()->setSn(strSn);
    property_set("vendor.xbh.rkp.serialno", strSn);
    if (vendor_storage_init() < 0)
    {
        XLOGE("Error initializing vendor storage");
        return XBH_FAILURE;
    }
    // 同步写入数据到vendor存储
    if (emmc_vendor_write(VENDOR_SN_ID, (void *)strSn, strlen(strSn) + 1) < 0)
    {
        // Include null-terminator
        XLOGE("Error writing to vendor storage with ID %d", VENDOR_SN_ID);
        return XBH_FAILURE;
    }
    //在烧录sn之后,根据AttentionKey是否有烧录,执行AttentionKey烧录
    getAttentionKeyStatus(&enable);
    if(!enable)
    {
        //烧录AttentionKey
        setAttentionKey("");
    }
    else
    {
        XLOGD("AttentionKey Already exist");
    }
    return  s32Ret;
}

//Get serial number
XBH_S32 XbhRk_3576::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSn(strSn);
    return  s32Ret;
}

//Set Chip RunTime
XBH_S32 XbhRk_3576::setChipRunTime(XBH_S32 time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setChipRunTime(time);
    return  s32Ret;
}

//Get Chip RunTime
XBH_S32 XbhRk_3576::getChipRunTime(XBH_S32* time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getChipRunTime(time);
    return  s32Ret;
}

//set ethernet mac address
XBH_S32 XbhRk_3576::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setMacAddress(strMacAddress);
    return  s32Ret;
}

//get ethernet mac address
XBH_S32 XbhRk_3576::getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getMacAddress(strMacAddress);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type)
{
    XBH_U8 key[XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN] = {0};
    XBH_S32 keyLen = XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN;
    XBH_S32 readKeyLen = 0;
    XBH_S32 err = 0;
    FILE * fp = NULL;

    if(type != XBH_HDCP1_4)
    {
        XLOGE("only dp-tx hdcp 1.4 key can be set\n");
        return XBH_FAILURE;
    }

    fp = fopen(pBuff,"rb");
    if(fp == NULL)
    {
        XLOGE("hdcp key file open fail\n");
        return XBH_FAILURE;
    }

    // 读取key
    readKeyLen = fread(key, 1, keyLen, fp);
    if(readKeyLen != keyLen)
    {
        XLOGE("hdcp1.4 key [%d] : %s length does not match,need length: %d\n",readKeyLen, pBuff, XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN);
        fclose(fp);
        return XBH_FAILURE;
    }
    fclose(fp);

    // 烧key
    err = hdcp_key_write(hdcp_key_id, key, keyLen);
    if(err)
    {
        XLOGE("hdcp_key_write fail\n");
        return XBH_FAILURE;
    }

    // 保存至我们xbhcust中
    err =XbhPartitionData::getInstance()->setHDCPKey_14(key);
    if(err)
    {
        XLOGE("hdcp_key write cusdata fail\n");
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable)
{
    XBH_U8 key[XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN] = {0};
    XBH_U8 cusDataKey[XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN] = {0};
    XBH_S32 keyLen = XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN;
    XBH_S32 err = 0;
    XBH_S32 i = 0;

    *bEnable = XBH_FALSE;

    if(type != XBH_HDCP1_4)
    {
        XLOGE("only dp-tx hdcp 1.4 key can be get\n");
        return XBH_FAILURE;
    }

    //读取烧写的key
    err = hdcp_key_read(hdcp_key_id, key, &keyLen);
    if(err)
    {
        XLOGE("hdcp_key_read fail\n");
        return XBH_FAILURE;
    }

    //读取我们保存的key
    err = XbhPartitionData::getInstance()->getHDCPKey_14(cusDataKey);
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

XBH_S32 XbhRk_3576::getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret  = XbhPartitionData::getInstance()->getHDCPKey_14((XBH_VOID *)pBuff);
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        s32Ret  = XbhPartitionData::getInstance()->getHDCPKey_22((XBH_VOID *)pBuff);
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getHdcpKeydata(XBH_SOURCE_E source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret  = XbhPartitionData::getInstance()->setHDCPKeyName_14((XBH_VOID *)strHdcpFileName);
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        s32Ret  = XbhPartitionData::getInstance()->setHDCPKeyName_22((XBH_VOID *)strHdcpFileName);
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret  = XbhPartitionData::getInstance()->getHDCPKeyName_14((XBH_VOID *)strHdcpFileName);
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        s32Ret  = XbhPartitionData::getInstance()->getHDCPKeyName_22((XBH_VOID *)strHdcpFileName);
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setGoogleKeyName(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setGoogleKeyName((XBH_VOID *)strPath);
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getGoogleKeyName(XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getGoogleKeyName((XBH_VOID *)strPath);
    return  s32Ret;
}


XBH_S32 XbhRk_3576::setGammaGroup(XBH_S32 s32Value)
{
    XLOGD("%s - value %d", __func__, s32Value);
    return VmanModule::getInstance()->set_gamma_mode(s32Value);
}

XBH_S32 XbhRk_3576::getGammaGroup(XBH_S32* s32Value)
{
    *s32Value = VmanModule::getInstance()->get_gamma_mode();
    XLOGD("%s - value %d", __func__, *s32Value);
    return 0;
}

XBH_S32 XbhRk_3576::setPictureMode(XBH_PICMODE_E enPicMode)
{
    XLOGD("%s - value %d", __func__, enPicMode);
    return VmanModule::getInstance()->set_picture_mode(enPicMode);
}

XBH_S32 XbhRk_3576::getPictureMode(XBH_PICMODE_E* enPicMode)
{
    int index;
    enum XBH_PICMODE_E modes[] =
    {
        XBH_PICMODE_STANDARD,
        XBH_PICMODE_DYNAMIC,
        XBH_PICMODE_VIVID,
        XBH_PICMODE_SOFTNESS,
        XBH_PICTURE_NATURAL,
        XBH_PICTURE_SPORTS,
        XBH_PICMODE_USER,
    };
    index = VmanModule::getInstance()->get_picture_mode();
    *enPicMode = modes[index];
    XLOGD("%s - value %d", __func__, *enPicMode);
    return 0;
}

XBH_S32 XbhRk_3576::setColorTempMode(XBH_COLORTEMP_E enColorTemp)
{
    XLOGD("%s - value %d", __func__, enColorTemp);
    return VmanModule::getInstance()->set_color_temp_mode(enColorTemp);
}

XBH_S32 XbhRk_3576::getColorTempMode(XBH_COLORTEMP_E *enColorTemp)
{
    int index;
    enum XBH_COLORTEMP_E modes[] =
    {
        XBH_COLORTEMP_STANDARD,
        XBH_COLORTEMP_WARM,
        XBH_COLORTEMP_COLD,
        XBH_COLORTEMP_USER,
    };

    index = VmanModule::getInstance()->get_color_temp_mode();
    *enColorTemp = modes[index];
    XLOGD("%s - value %d", __func__, *enColorTemp);
    return 0;
}

// range[0-100]
XBH_S32 XbhRk_3576::setBrightness(XBH_U32 u32Value) {
   return VmanModule::getInstance()->set_brightness(u32Value);
}

// range[0-100]
XBH_S32 XbhRk_3576::getBrightness(XBH_U32* u32Value)
{
    *u32Value = VmanModule::getInstance()->get_brightness();
    return 0;
}

// range[0-100]
XBH_S32 XbhRk_3576::setContrast(XBH_U32 u32Value)
{
    return VmanModule::getInstance()->set_contrast(u32Value);
}

// range[0-100]
XBH_S32 XbhRk_3576::getContrast(XBH_U32* u32Value)
{
    *u32Value = VmanModule::getInstance()->get_contrast();
    return 0;
}

// range[0-100]
XBH_S32 XbhRk_3576::setSaturation(XBH_U32 u32Value)
{
    return VmanModule::getInstance()->set_saturation(u32Value);
}

// range[0-100]
XBH_S32 XbhRk_3576::getSaturation(XBH_U32* u32Value)
{
    *u32Value = VmanModule::getInstance()->get_saturation();
    return 0;
}

XBH_S32 XbhRk_3576::setSharpEnable(XBH_U32 u32Value)
{
    return VmanModule::getInstance()->set_sharp_enable(u32Value);
}

// range[0-100]
XBH_S32 XbhRk_3576::setSharpness(XBH_U32 u32Value)
{
    return VmanModule::getInstance()->set_sharpness(u32Value);
}

// range[0-100]
XBH_S32 XbhRk_3576::getSharpness(XBH_U32* u32Value)
{
    *u32Value = VmanModule::getInstance()->get_sharpness();
    return 0;
}

// range[0-100]
XBH_S32 XbhRk_3576::setHue(XBH_U32 u32Value)
{
    return VmanModule::getInstance()->set_hue(u32Value);
}

// range[0-100]
XBH_S32 XbhRk_3576::getHue(XBH_U32* u32Value)
{
    *u32Value = VmanModule::getInstance()->get_hue();
    return 0;
}

XBH_S32 XbhRk_3576::setHDMIColorRange(XBH_COLOR_RANGE_E enColorRange)
{
    int index = 0;
    if (enColorRange == XBH_COLOR_RANGE_AUTO)
    {
        index = 2;
    }
    else if (enColorRange == XBH_COLOR_RANGE_LIMIT)
    {
        index = 1;
    }
    else if (enColorRange == XBH_COLOR_RANGE_FULL)
    {
        index = 0;
    }
    return VmanModule::getInstance()->set_color_range(index);
}

XBH_S32 XbhRk_3576::getHDMIColorRange(XBH_COLOR_RANGE_E* enColorRange)
{
    int index = VmanModule::getInstance()->get_color_range();
    if (index == 0)
    {
        *enColorRange = XBH_COLOR_RANGE_FULL;
    }
    else if (index == 1)
    {
        *enColorRange = XBH_COLOR_RANGE_LIMIT;
    }
    else
    {
        *enColorRange = XBH_COLOR_RANGE_AUTO;
    }
    return 0;
}

/**
 * backlight value, range[0-100]
 */
XBH_S32 XbhRk_3576::setBacklightWithOutSave(XBH_U32 value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    float float_value = static_cast<float>(value) * 255.0f / 100.0f;
    int rounded = static_cast<int>(std::round(float_value));
    XLOGD("value=%d,rounded=%d", value,rounded);
    if(rounded <= 0 )
    {
        rounded = 1;//当前的背光曲线配置参数，如果驱动节点值为0，则会完全黑屏，所以最小值不能设置为0
    }
    else if(rounded >= 255)
    {
        rounded = 255;
    }
    std::string valueStr = std::to_string(rounded);
    XbhSysOpt::getInstance()->writeSysfs("/sys/devices/platform/backlight/backlight/backlight/brightness",valueStr);
    return s32Ret;
}

/**
 * backlight value, range[0-100]
 */
XBH_S32 XbhRk_3576::getBacklightWithOutSave(XBH_U32* value, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std :: string brightnessValueStr;
    XbhSysOpt::getInstance()->readSysfs("/sys/devices/platform/backlight/backlight/backlight/brightness",brightnessValueStr);
    double temp = std::stod(brightnessValueStr);
    double valueDouble = (temp * 100.0) / 255.0;
    XBH_S32 valueInt = static_cast<signed int>(std::round(valueDouble));
    *value = valueInt;
    XLOGD("brightnessValueStr=%s,valueInt=%d", brightnessValueStr.c_str(),valueInt);
    return s32Ret;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhRk_3576::getSignalStatus(XBH_S32 *status)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_S32 signal_status = 0;

    int fd = open(VIDEO_FD, O_RDWR);
    int err = ioctl(fd, HDMIRX_CMD_GET_SIGNAL_STABLE_STATUS, &signal_status);
    if (err < 0)
    {
        //XLOGI("failed HDMIRX_CMD_GET_SIGNAL_STABLE_STATUS err: %d", err);
    }
    else
    {
        //XLOGI("HDMIRX_CMD_GET_SIGNAL_STABLE_STATUS %d", signal_status);
        *status = ((XBH_BOOL)signal_status ? XBH_SIGNAL_STATUS_SUPPORT : XBH_SIGNAL_STATUS_NOSIGNAL);
    }
    close(fd);
    return ret;
}

XBH_S32 XbhRk_3576::setDelayTimeForHdmiSwitch()
{
    XBH_S32 ret = XBH_SUCCESS;
    XLOGD(" delay 5000ms \n");
    usleep(5000 * 1000);
    return ret;
}

//setHdmiRx5vDet
XBH_S32 XbhRk_3576::setHdmiRx5vDet(XBH_BOOL enable)
{
    XLOGD("setHdmiRx5vDet enable=%d: \n", enable);
    int ret = XBH_SUCCESS;
    int fd = open(HDMIRX_CSI_HDMI_RX_5V_DET_PATH, O_WRONLY);
    if (fd < 0)
    {
        XLOGE("Error opening %s:\n", HDMIRX_CSI_HDMI_RX_5V_DET_PATH);
        return XBH_FAILURE;
    }
    if(enable)
    {
        XLOGD("setHdmiRx5vDet 1\n");
        write(fd, "1", 1);
    }
    else
    {
        XLOGD("setHdmiRx5vDet 0\n");
        write(fd, "0", 1);
    }
    close(fd);
    return ret;
}

XBH_S32 XbhRk_3576::setSourceScreenOffset(XBH_S32 x, XBH_S32 y)
{
    return VmanModule::getInstance()->set_screen_pixel_shift(x, y);
}

XBH_S32 XbhRk_3576::getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y)
{
    return VmanModule::getInstance()->get_screen_pixel_shift(x, y);
}

XBH_S32 XbhRk_3576::getTemperatureSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_ADC_CHANNEL_NUM_E adcChannel = (XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_NTC_ADC_THERMISTOR_CHAN;
    XBH_U32 adcValue = 0;
    s32Ret = getADCChannelValue(adcChannel, &adcValue);
    if(adcValue >= 4000)
    {
        *bEnable = 0;
    }
    else
    {
        *bEnable = 1;
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setBootLogo(const XBH_CHAR* path)
{
    FILE *devFile = NULL, *imgFile = NULL;
    XBH_CHAR buf[512] = {0};
    XBH_S32 readSize = 0;

    devFile = fopen("/dev/block/by-name/logo","wb");
    if(devFile == NULL)
    {
        XLOGE("open /dev/block/by-name/logo fail\n");
        return XBH_FAILURE;
    }

    imgFile = fopen(path,"rb");
    if(imgFile == NULL)
    {
        XLOGE("open %s fail\n",path);
        fclose(devFile);
        return XBH_FAILURE;
    }

    while((readSize = fread(buf, 1, sizeof(buf), imgFile)) > 0)
    {
        if(fwrite(buf, 1, readSize, devFile) != readSize)
        {
            XLOGE("write fail!!!\n");
            fclose(devFile);
            fclose(imgFile);
            return XBH_FAILURE;
        }
    }
    fclose(devFile);
    fclose(imgFile);
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
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
    mAudioOutput = enAudioOutput;
    return s32Ret;
}

XBH_S32 XbhRk_3576::getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *enAudioOutput = mAudioOutput;
    return s32Ret;
}

XBH_S32 XbhRk_3576::setWatchDogManualModeEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(mWatchDogFd < 0)
    {
        mWatchDogFd = open(WATCHDOG_DEV_NAME,  O_RDWR | O_CLOEXEC);
    }
    XLOGD("setWatchDogManualModeEnable: %d", bEnable);
    if (mWatchDogFd < 0) {
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    mWatchDogManualMode = bEnable;
    watchDogPing();

    return s32Ret;
}

XBH_S32 XbhRk_3576::getWatchDogManualModeEnable(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(mWatchDogFd < 0)
    {
        mWatchDogFd = open(WATCHDOG_DEV_NAME,  O_RDWR | O_CLOEXEC);
    }
    if (mWatchDogFd < 0)
    {
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    *bEnable = mWatchDogManualMode;
    //XLOGD("getWatchDogManualModeEnable: %d", *bEnable);

    return s32Ret;
}

XBH_S32 XbhRk_3576::setWatchDogEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(mWatchDogFd < 0)
    {
        mWatchDogFd = open(WATCHDOG_DEV_NAME,  O_RDWR | O_CLOEXEC);
    }
    XLOGD("setWatchDogManualModeEnable: %d", bEnable);
    if (mWatchDogFd < 0)
    {
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    int val;
    if (bEnable == 1)
    {
        val = WDIOS_ENABLECARD;
    }
    else
    {
        val = WDIOS_DISABLECARD;
    }
    int ret = ioctl(mWatchDogFd, WDIOC_SETOPTIONS, &val);
    if (ret)
    {
        XLOGE("setWatchDogEnable: Failed to set enable to %d", bEnable);
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 XbhRk_3576::setWatchDogTimeOut(XBH_U32 u32TimeOutSec)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(mWatchDogFd < 0)
    {
        mWatchDogFd = open(WATCHDOG_DEV_NAME,  O_RDWR | O_CLOEXEC);
    }
    if (mWatchDogFd < 0)
    {
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }

    int ret = ioctl(mWatchDogFd, WDIOC_SETTIMEOUT, &u32TimeOutSec);
    if (ret)
    {
        XLOGE("setWatchDogTimeOut: Failed to set timeout to %d", u32TimeOutSec);
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    XLOGD("setWatchDogTimeOut: %d", u32TimeOutSec);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getWatchDogTimeOut(XBH_U32 *u32TimeOutSec)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(mWatchDogFd < 0)
    {
        mWatchDogFd = open(WATCHDOG_DEV_NAME,  O_RDWR | O_CLOEXEC);
    }
    if (mWatchDogFd < 0)
    {
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    int timeout;
    int ret = ioctl(mWatchDogFd, WDIOC_GETTIMEOUT, &timeout);
    if (ret)
    {
        XLOGE("getWatchDogTimeOut: Failed to get timeout");
    }
    *u32TimeOutSec = timeout;
    //XLOGD("getWatchDogTimeOut: %d", *u32TimeOutSec);

    return s32Ret;
}

XBH_S32 XbhRk_3576::watchDogPing(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(mWatchDogFd < 0)
    {
        mWatchDogFd = open(WATCHDOG_DEV_NAME,  O_RDWR | O_CLOEXEC);
    }
    XLOGD("watchDogPing...");
    if (mWatchDogFd < 0)
    {
        s32Ret = XBH_FAILURE;
        return s32Ret;
    }
    write(mWatchDogFd, "", 1);

    return s32Ret;
}

XBH_S32 XbhRk_3576::getPwmFrequency(XBH_U32* u32PwmF, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);

    XBH_S32 fd = -1;
    XBH_CHAR tmp[32] = {0};

    if(access(BACKLIGHT_FREQUENCY, F_OK))
    {
        XLOGE("%s not exist\n",BACKLIGHT_FREQUENCY);
        return XBH_FAILURE;
    }

    if((fd = open(BACKLIGHT_FREQUENCY, O_RDONLY)) < 0)
    {
        XLOGE("%s open fail\n",BACKLIGHT_FREQUENCY);
        return XBH_FAILURE;
    }

    if(read(fd,tmp,sizeof(tmp)) < 0)
    {
        XLOGE("%s read fail\n",BACKLIGHT_FREQUENCY);
        close(fd);
        return XBH_FAILURE;
    }
    close(fd);
    *u32PwmF = atoi(tmp);
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::setPwmFrequency(XBH_U32 u32PwmF, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);

    XBH_S32 freqFd = -1;
    XBH_S32 curDuty = 0;
    XBH_CHAR tmp[32] = {0};
    XBH_U32 curFreq = 0;

    if(access(BACKLIGHT_FREQUENCY, F_OK))
    {
        XLOGE("%s not exist\n",BACKLIGHT_FREQUENCY);
        return XBH_FAILURE;
    }
    if((freqFd = open(BACKLIGHT_FREQUENCY, O_WRONLY)) < 0)
    {
        XLOGE("%s open fail\n",BACKLIGHT_FREQUENCY);
        return XBH_FAILURE;
    }
    /**
    * 通过示波器实验，有两个问题
    * 1. 单次调节不能超过当前值2.5倍；所以使用循环控制频率
    * 2. 多次调节时，会出现卡住现象，需调节一次占空比，才恢复正常；所以每次控制频率前控制一次占空比
    */
    getPwmFrequency(&curFreq, enPanel);
    while((XBH_U32)((XBH_FLOAT)curFreq * 2.5) < u32PwmF)
    {
        curFreq = (XBH_U32)((XBH_FLOAT)curFreq * 2.5);
        curDuty = VmanModule::getInstance()->get_backlight();
        VmanModule::getInstance()->set_backlight(curDuty - 1);
        memset(tmp, 0, sizeof(tmp));
        sprintf(tmp, "%d\n", curFreq);
        if(write(freqFd, tmp, strlen(tmp) + 1) < 0)
        {
            XLOGE("%s write fail\n",BACKLIGHT_FREQUENCY);
            close(freqFd);
            return XBH_FAILURE;
        }
        usleep(1000);
        VmanModule::getInstance()->set_backlight(curDuty);
    }
    curDuty = VmanModule::getInstance()->get_backlight();
    VmanModule::getInstance()->set_backlight(curDuty - 1);
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%d\n", u32PwmF);
    if(write(freqFd, tmp, strlen(tmp) + 1) < 0)
    {
        XLOGE("%s write fail\n",BACKLIGHT_FREQUENCY);
        close(freqFd);
        return XBH_FAILURE;
    }
    VmanModule::getInstance()->set_backlight(curDuty);
    close(freqFd);
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::loadEdlaCust(XBH_BOOL *enable)
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
    loadPq();
    res = loadProperty();
    //customerversion 一致不需要重启
    if (res == 1)
    {
        *enable = XBH_FALSE;
    }
    else
    {
        *enable = XBH_TRUE;
    }
    return res;
}

XBH_S32 XbhRk_3576::loadPq()
{
    Result result;
    result = mComposer->setCscMode(0, 0);
    if(result != Result::OK)
    {
        return XBH_FAILURE;
    }

    result = mComposer->setDciMode(0, 0);
    if(result != Result::OK)
    {
        return XBH_FAILURE;
    }

    result = mComposer->setAcmMode(0, 0);
    if(result != Result::OK)
    {
        return XBH_FAILURE;
    }

    result = mComposer->setGammaMode(0, 0);
    if(result != Result::OK)
    {
        return XBH_FAILURE;
    }

    result = mComposer->setSharpMode(0, 0);
    if(result != Result::OK)
    {
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::setGoogleKey(const XBH_CHAR* strPath)
{
    XBH_S32 ret = 0;
    void *handle = dlopen("/vendor/lib64/libKeyboxUtil.so", RTLD_LAZY);
    if(!handle)
    {
        XLOGD("open lib error:%s", dlerror());
        return XBH_FAILURE;
    }

    typedef int (*lib_t)(const char* keybox_file);
    lib_t funcPoint = (lib_t) dlsym(handle, "rk_install_attestation_keybox");
    if(!funcPoint)
    {
        XLOGD("dlsym:%s", dlerror());
        dlclose(handle);
        return XBH_FAILURE;
    }
    ret = funcPoint(strPath);
    if(ret != 0)
    {
        XLOGD("write Google Key: error");
        dlclose(handle);
        return XBH_FAILURE;
    }

    dlclose(handle);

    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getGoogleKeyStatus(XBH_BOOL* bEnable)
{
    XBH_S32 keyboxLen = 0;
    XBH_S32 deviceIdLen = 0;
    XBH_U8 deviceId[32] = {0};
    XBH_S32 ret = 0;
    *bEnable = XBH_FALSE;

    void *handle = dlopen("/vendor/lib64/libKeyboxUtil.so", RTLD_LAZY);
    if(!handle)
    {
        XLOGD("open lib error:%s", dlerror());
        return XBH_FAILURE;
    }

    typedef int (*read_t)(int *keybox_len,uint8_t* device_id,int *device_id_len);
    read_t readfuncPoint = (read_t) dlsym(handle, "rk_read_attestation_keybox_ex");
    if(!readfuncPoint)
    {
        XLOGD("dlsym:%s", dlerror());
        dlclose(handle);
        return XBH_FAILURE;
    }
    ret = readfuncPoint(&keyboxLen, deviceId, &deviceIdLen);
    if(ret != 0)
    {
        XLOGD("read Google Key: error");
        dlclose(handle);
        return XBH_FAILURE;
    }

    XLOGD("keyboxLen:%d deviceId:%s deviceIdLen:%d", keyboxLen, deviceId, deviceIdLen);
    dlclose(handle);
    if(keyboxLen != 3)
    {
        return XBH_FAILURE;
    }
    *bEnable = XBH_TRUE;
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::setAttentionKey(const XBH_CHAR* strPath)
{
    AttestationIds attestationId = {0};
    XBH_CHAR sn[64] = {0};
    std::string brand = "";
    std::string device_name = "";
    std::string product_name = "";
    std::string manufacturer = "";
    std::string model_name = "";
    getSn(sn);
    XbhSysOpt::getPropertyString("ro.product.system.brand", brand, "");
    XbhSysOpt::getPropertyString("ro.product.system.device", device_name, "");
    XbhSysOpt::getPropertyString("ro.product.system.name", product_name, "");
    XbhSysOpt::getPropertyString("ro.product.system.manufacturer", manufacturer, "");
    XbhSysOpt::getPropertyString("ro.product.system.model", model_name, "");

    memcpy(attestationId.brand, brand.c_str(), 32);
    memcpy(attestationId.device, device_name.c_str(), 32);
    memcpy(attestationId.product, product_name.c_str(), 32);
    memcpy(attestationId.manufacturer, manufacturer.c_str(), 64);
    memcpy(attestationId.model, model_name.c_str(), 64);
    memcpy(attestationId.serial, sn, 64);

    XLOGD("attestationId brand:%s device_name:%s product_name:%s manufacturer:%s model:%s serial:%s", attestationId.brand,
          attestationId.device, attestationId.product, attestationId.manufacturer, attestationId.model, attestationId.serial);
    XBH_S32 ret = 0;
    void *handle = dlopen("/vendor/lib64/libKeyboxUtil.so", RTLD_LAZY);
    if(!handle)
    {
        XLOGD("open lib error:%s", dlerror());
        return XBH_FAILURE;
    }

    typedef int (*lib_t)(AttestationIds* attestation_ids);
    lib_t funcPoint = (lib_t) dlsym(handle, "rk_set_id_attestation");
    if(!funcPoint)
    {
        XLOGD("dlsym:%s", dlerror());
        dlclose(handle);
        return XBH_FAILURE;
    }
    ret = funcPoint(&attestationId);
    if(ret != 0)
    {
        if (ret == TEE_ERROR_SECURITY)
        {
            XLOGE("***** FAILED! attestation_id already exist! You cannot update it! *****");
        }
        else
        {
            XLOGE("***** FAILED! write attestation_id failed with code 0x%x*****", ret);
        }
        dlclose(handle);
        return XBH_FAILURE;
    }
    else
    {
        XLOGD("***** SUCCESS! write attestation_id success! *****");
    }

    dlclose(handle);

    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getGoogleKeyID(XBH_CHAR* deviceID)
{
    XBH_S32 ret = 0;
    XBH_S32 deviceIdLen = 0;
    XBH_S32 s32Ret = XBH_SUCCESS;

    void *handle = dlopen("/vendor/lib64/libKeyboxUtil.so", RTLD_LAZY);
    if(!handle)
    {
        XLOGD("open lib error:%s", dlerror());
        return XBH_FAILURE;
    }

    typedef int (*read_t)(uint8_t* device_id,int *device_id_len);
    read_t readfuncPoint = (read_t) dlsym(handle, "rk_read_attestation_keybox");
    if(!readfuncPoint)
    {
        XLOGD("dlsym:%s", dlerror());
        dlclose(handle);
        return XBH_FAILURE;
    }

    ret = readfuncPoint((uint8_t*)deviceID, &deviceIdLen);
    if(ret != 0)
    {
        XLOGD("read Google Key: error");
        dlclose(handle);
        return XBH_FAILURE;
    }

    XLOGD("deviceId:%s deviceIdLen:%d", deviceID, deviceIdLen);
    dlclose(handle);
    return s32Ret;
}

XBH_S32 XbhRk_3576::getAttentionKeyStatus(XBH_BOOL* bEnable)
{
    XBH_S32 idsLen = 0;
    XBH_S32 ret = 0;
    *bEnable = XBH_FALSE;

    void *handle = dlopen("/vendor/lib64/libKeyboxUtil.so", RTLD_LAZY);
    if(!handle)
    {
        XLOGD("open lib error:%s", dlerror());
        return XBH_FAILURE;
    }

    typedef int (*read_t)(int *ids_len);
    read_t readfuncPoint = (read_t) dlsym(handle, "rk_check_id_attestation");
    if(!readfuncPoint)
    {
        XLOGD("dlsym:%s", dlerror());
        dlclose(handle);
        return XBH_FAILURE;
    }
    ret = readfuncPoint(&idsLen);
    if(ret != TEEC_SUCCESS)
    {
        if (ret == TEEC_ERROR_ITEM_NOT_FOUND)
        {
            XLOGE("***** FAILED! attestation_id does not exist! *****");
        }
        else
        {
            XLOGE("***** FAILED! read attestation_id failed with code 0x%x*****", ret);
        }
        dlclose(handle);
        return XBH_FAILURE;
    }

    XLOGD("AttestationId length:%d", idsLen);
    dlclose(handle);
    *bEnable = XBH_TRUE;
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getRkpCsrInfo(XBH_CHAR* read_buffer, int buffer_size)
{
    if (vendor_storage_init() < 0) {
        XLOGE("Error initializing vendor storage");
        return XBH_FAILURE;
    }
    // 从vendor存储读取数据
    if (emmc_vendor_read(VENDOR_RKP_ID, read_buffer, buffer_size) < 0) {
        XLOGE("Error reading from vendor storage with ID %d\n", VENDOR_RKP_ID);
        return XBH_FAILURE;
    }
    XLOGD("Data read successfully.");
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::setBacklightEnable(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);
    XBH_CHAR onOff[PROPERTY_VALUE_MAX] = {0};
    sprintf(onOff, "%d", bEnable ? 1 : 0);
    property_set("sys.bl.on", onOff);
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getBacklightEnable(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    property_get("sys.bl.on", propVal, "1");
    *bEnable = atoi(propVal) ? XBH_TRUE : XBH_FALSE;
    return XBH_SUCCESS;
}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数 ,范围0~255
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint32_t rgain, ggain, bgain;
    
    rgain = data->u32RedGain;
    ggain = data->u32GreenGain;
    bgain = data->u32BlueGain;
    /*
    设置方法最终调用hw_output.cpp-->hw_output_set_preset_white_balance()
    方法执行逻辑表明；PATH_AUTOPQ 修改自动白平衡rgb
    PATH_BASEPARAMETER 修改原厂预制模式rgb值
    */
    int PATH_BASEPARAMETER = 1;
    s32Ret = VmanModule::getInstance()->set_preset_color_temp_mode(PATH_BASEPARAMETER, enColorTemp, rgain, ggain, bgain);
    if(s32Ret == XBH_SUCCESS)
    {
        //project id被设置，到重启前都不能往partitiondata 里面写入色温数据
        if(!mIsProjectIdChange)
        {
            s32Ret = XbhPartitionData::getInstance()->setColorTempPara(enColorTemp, data);
        }
    }
    setcheckColorTempValue(true);
    return  s32Ret;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数 ,范围0~255
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    s32Ret = XbhPartitionData::getInstance()->getColorTempPara(enColorTemp, data);
    XBH_BOOL CheckColorTemp = XBH_FALSE; 
    checkColorTempParat(data);
    getcheckColorTempValue(&CheckColorTemp);
    //是否使用不可擦除分区里面的色温值
    if(!mColorTempStorage)
    {
        CheckColorTemp = XBH_FALSE;
        XLOGD(" mColorTempStorage = 0, must get_preset_color_temp_mode \n");
    }
    if(!CheckColorTemp)
    {
        //检查失败，则从系统预制读取后写入
        uint32_t rgain, ggain, bgain;
        /*
         原始定义路径见：hardware/rockchip/hw_output/hw_types.h -->enum pq_config_save_path
         PATH_BASEPARAMETER 表示获取指定模式RGB值
         PATH_AUTOPQ  表示获取自动白平衡RGB值
        */
        int PATH_BASEPARAMETER = 1;
        s32Ret = VmanModule::getInstance()->get_preset_color_temp_mode(PATH_BASEPARAMETER, enColorTemp, &rgain, &ggain, &bgain);
        data -> u32RedGain = rgain;
        data -> u32GreenGain = ggain;
        data -> u32BlueGain = bgain;
        //project id被设置，到重启前都不能往partitiondata 里面写入色温数据
        if(!mIsProjectIdChange)
        {
            XbhPartitionData::getInstance()->setColorTempPara(enColorTemp, data);
        }
    }
    return  s32Ret;
}

/**
 * 使用GPIO的名字来设置GPIO的方向和值
 * param[in] gpioName. gpio名字
 * param[in] gpioDirection. gpio方向 1:输出 2.输入
 * param[in] u32GpioValue. gpio输出值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32 gpioDirection, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 i = 0;
    XBH_S32 count = sizeof(gpio_common_table) / sizeof(struct gpio_common_info);
    XLOGD("gpioName = %s  gpioDirection = %d u32GpioValue\n",gpioName,gpioDirection);
    for (i = 0; i < count; i++)
    {
        // setGpioOutputValue( gpio_common_table[i].gpio_name, gpio_common_table[i].gpio_number);
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
                    setGpioInput(gpio_common_table[i].gpio_number);
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
XBH_S32 XbhRk_3576::getGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32* u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 i = 0;
    *u32GpioValue = XBH_NO_CONFIG;
    XBH_S32 count = sizeof(gpio_common_table) / sizeof(struct gpio_common_info);
    XLOGD("%s  gpioName = %s\n", __func__ ,gpioName);
    for (i = 0; i < count; i++)
    {
        // setGpioOutputValue( gpio_common_table[i].gpio_name, gpio_common_table[i].gpio_number);
        if (strcmp(gpioName,gpio_common_table[i].gpio_name) == 0 )
        {
            XLOGD("s gpio_common_table find name ", __func__);
            if (gpio_common_table[i].gpio_number != XBH_NO_CONFIG)
            {
                s32Ret = getGpioOutputValue(gpio_common_table[i].gpio_number,u32GpioValue);
            }
            XLOGD("%sgpio_common_table find gpio_number ", __func__);
            break;
        }
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::setcheckColorTempValue(XBH_BOOL CheckColorTemp)
{
    mCheckColorTemp = CheckColorTemp;
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getcheckColorTempValue(XBH_BOOL *CheckColorTemp)
{
    *CheckColorTemp = mCheckColorTemp;
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::checkColorTempParatGain(XBH_U32 gain)
{
    if(gain > COLOR_TEMP_GAIN_MIN && gain <= COLOR_TEMP_GAIN_MAX)
    {
        return gain;
    }
    setcheckColorTempValue(false);
    return COLOR_TEMP_GAIN_DEFAULT;
}

XBH_S32 XbhRk_3576::checkColorTempParatOffset(XBH_U32 gain)
{
    if(gain >=  COLOR_TEMP_GAIN_OFFSET_MIN  && gain <= COLOR_TEMP_GAIN_OFFSET_MAX )
    {
        return gain;
    }
    setcheckColorTempValue(false);
    return COLOR_TEMP_GAIN_OFFSET_DEFAULT;
}


XBH_S32 XbhRk_3576::checkColorTempParat(XBH_GAIN_OFFSET_DATA_S* data)
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

XBH_S32 XbhRk_3576::checkColorTempValue(XBH_GAIN_OFFSET_DATA_S* data)
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
XBH_S32 XbhRk_3576::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    bool inPipMode = property_get_bool(PIP_MODE_PROPERTY, 0);
    XLOGD("%s enable %d, inPipMode %d\n", __func__ ,bEnable, inPipMode);
    if(bEnable && !inPipMode)
    {
        XbhSysOpt::getInstance()->writeSysfs("/sys/devices/platform/rkcif-mipi-lvds/wait_line" ,"100");
        XbhSysOpt::getInstance()->writeSysfs("/sys/devices/platform/rkcif-mipi-lvds/low_latency" ,"1");
        s32Ret = XBH_SUCCESS;
    }
    else
    {
        XbhSysOpt::getInstance()->writeSysfs("/sys/devices/platform/rkcif-mipi-lvds/wait_line" ,"0");
        XbhSysOpt::getInstance()->writeSysfs("/sys/devices/platform/rkcif-mipi-lvds/low_latency" ,"0");
        s32Ret = XBH_SUCCESS;
    }
    return  s32Ret;
}

XBH_S32 XbhRk_3576::getEthPortSpeed(XBH_S32 port, XBH_S32* value)
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
XBH_S32 XbhRk_3576::setProjectId(const XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //由于色温是同步保存到了partitiondata 中，切换project id的时候需要把之前的数据清空
    XBH_GAIN_OFFSET_DATA_S data = {0, 0, 0, 0, 0, 0};
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_STANDARD, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_WARM, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_COLD, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_USER, &data);
    #ifdef XBH_USE_COLORTEMP_EXT
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_NATIVE, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_3000K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_4000K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_5000K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_6500K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_7500K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_9300K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_10000K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_11000K, &data);
    XbhPartitionData::getInstance()->setColorTempPara(XBH_COLORTEMP_E::XBH_COLORTEMP_13000K, &data);
    #endif
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
    s32Ret  = XbhPartitionData::getInstance()->setProjectId(strProjectId);
    XLOGD("setProjectId strProjectId=%s\n", strProjectId);
    return  s32Ret;
}

/**
 * 获取Project ID
 * param[out] strProjectId Project ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getProjectId(XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getProjectId(strProjectId);
    XLOGD("getProjectId strProjectId=%s\n", strProjectId);
    return  s32Ret;
}

/**
 * 设置待机ADC中断
 * param[in] bEnable 1：on  0:off
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setAdcIntEnabled(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(bEnable)
    {
        XbhSysOpt::getInstance()->writeSysfs(ADC_INT_EDABLED_PATH ,"1");
        s32Ret = XBH_SUCCESS;
    }else
    {
        XbhSysOpt::getInstance()->writeSysfs(ADC_INT_EDABLED_PATH ,"0");
        s32Ret = XBH_SUCCESS;
    }
    return  s32Ret;
}

/**
 * 写入SFT工厂数据
 * param[in] data 数据
 * retval true:success, false:failure
*/
XBH_S32 XbhRk_3576::setSftData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSftData(data);
    XLOGD("setSftData data=%s\n", data);
    return  s32Ret;
}

/**
 * 获取SFT工厂数据
 * param[out] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSftData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSftData(data);
    XLOGD("getSftData data=%s\n", data);
    return  s32Ret;
}

/**
 * 工厂需要存储产品的 RkpStatus
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setRkpStatus(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setRkpStatus(data);
    XLOGD("setRkpStatus data=%s\n", data);
    return  s32Ret;
}

/**
* 工厂需要存储产品的 RkpStatus
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getRkpStatus(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getRkpStatus(data);
    XLOGD("getRkpStatus data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setSmartPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSmartPnData(data);
    XLOGD("setSmartPnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储产品的PN码，一个系列对应一个PN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSmartPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSmartPnData(data);
    XLOGD("getSmartPnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储产品的SN码，一个系列对应一个SN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setSmartSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSmartSnData(data);
    //更新VGA EDID update标志
    //只要调用了setSmartSnData()这个函数,那么sn就一定改变了,那么就要改变vga的edid
    property_set("persist.vendor.xbh.vgaedid.update", "0");
    XLOGD("setSmartSnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储产品的SN码，一个系列对应一个SN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSmartSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSmartSnData(data);
    XLOGD("getSmartSnData data=%s\n", data);
    return  s32Ret;
}

/**
 * SMART希望罗盘关闭的状态能永久保存，复位不被清除
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setCompassPresist(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setCompassPresist(data);
    XLOGD("setCompassPresist data=%s\n", data);
    return  s32Ret;
}

/**
* SMART希望罗盘关闭的状态能永久保存，复位不被清除
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getCompassPresist(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getCompassPresist(data);
    XLOGD("getCompassPresist data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储底板的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setSmartScalerPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSmartScalerPnData(data);
    XLOGD("setSmartScalerPnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储底板的PN码，一个系列对应一个PN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSmartScalerPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSmartScalerPnData(data);
    XLOGD("getSmartScalerPnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储底板的SN码，一个系列对应一个SN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setSmartScalerSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSmartScalerSnData(data);
    XLOGD("setSmartScalerSnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储底板的SN码，一个系列对应一个SN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSmartScalerSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSmartScalerSnData(data);
    XLOGD("getSmartScalerSnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储核心板的PN码，一个系列对应一个PN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setSmartCorePnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSmartCorePnData(data);
    XLOGD("setSmartCorePnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储核心板的PN码，一个系列对应一个PN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSmartCorePnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSmartCorePnData(data);
    XLOGD("getSmartCorePnData data=%s\n", data);
    return  s32Ret;
}

/**
 * smart工厂需要存储核心板的SN码，一个系列对应一个SN码
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::setSmartCoreSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSmartCoreSnData(data);
    XLOGD("setSmartCoreSnData data=%s\n", data);
    return  s32Ret;
}

/**
* smart工厂需要存储核心板的SN码，一个系列对应一个SN码
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getSmartCoreSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSmartCoreSnData(data);
    XLOGD("getSmartCoreSnData data=%s\n", data);
    return  s32Ret;
}

/**
 * 设置工厂地址 地址
 * param[in] macType 地址类型
 * param[in] data 数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType, const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->saveFactoryMac(macType, data);
    XLOGD("macType:%d setMacAddress data=%s\n", macType, data);
    return  s32Ret;
}

/**
* 获取工厂地址
* param[out] macType 地址类型
* param[out] data 数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getFactoryMac(XBH_MAC_ADDRESS_TYPE macType, XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getFactoryMac(macType, data);
    XLOGD("macType:%d getMacAddress data=%s\n", macType, data);
    return  s32Ret;
}

/**
 * FATT烧录工厂SN
 * param[in] materialType 物料类型  0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
 * param[in] snInfo 写入数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::burnFactorySN(XBH_MATERAL_TYPE materialType, const XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->burnFactorySN(materialType, snInfo);
    XLOGD("materialType:%d snInfo=%s\n", materialType, snInfo);
    return  s32Ret;
}

/**
* 获取FATT烧录工厂SN
* param[out] materialType 物料类型 0: 底板(主板)SN  1:核心板SN   2:辅板1 SN  3:辅板2 SN  4:辅板3 SN
* param[out] snInfo 写入数据
* retval 0:success,-1:failure
*/
XBH_S32 XbhRk_3576::getBurnFactorySN(XBH_MATERAL_TYPE materialType,  XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getBurnFactorySN(materialType, snInfo);
    XLOGD("materialType:%d snInfo=%s\n", materialType, snInfo);
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
std::string XbhRk_3576::getUsbDeviceInfos(const std::string& input) 
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
            ||  name.find("usb") != std::string::npos || name.find(":") != std::string::npos)
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
    if (begin == std::string::npos)
    {
        return "";
    }
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
    if( projectidPwmBlInit == 0 )
    {
        if(config.global_dimming_target_pwm >0 )
        {
            char globalDimmingTargetPwmChar[32]= {0};
            sprintf(globalDimmingTargetPwmChar,"%d",config.global_dimming_target_pwm); 
            property_set("persist.vendor.xbh.global_dimming_target_pwm", globalDimmingTargetPwmChar);
        }
        if(config.pwm_bl_def >0 )
        {
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
XBH_S32 XbhRk_3576::readAndUpdateEdidBinFileByEdidType(int dev, const char* edidBinFilePath, int port)
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
        case XBH_UPDATE_MS9282:
            XbhVgaEdidManager::getInstance()->setVgaEdidData(file_buffer, XBH_SOURCE_VGA1);
            break;
        default:
            XbhHdmiSwitchManager::getInstance()->updateEdid(dev, file_buffer, port);
            break;
    }
    // ==================== 资源清理 ====================
    usleep(10*1000);
    //free(file_buffer);
    return EXIT_SUCCESS;
}

XBH_S32 XbhRk_3576::getHdmiRxLockStatus(XBH_BOOL* lock)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_S32 status;
    std :: string lockStatus;
    XbhSysOpt::getInstance()->readSysfs(HDMIRX_LOCK_STATUS, lockStatus);
    *lock = std::stoi(lockStatus) == 1;
    return ret;
}

XBH_S32 XbhRk_3576::setOnStop()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhRk_3576::deleteFile(const std::string& file_path)
{
    struct stat st;

    if (lstat(file_path.c_str(), &st) == XBH_FAILURE) 
    {
        if (errno == ENOENT) 
        {
            // 文件不存在，按需求返回成功
            XLOGD("File '%s' does not exist (treat as success)", file_path.c_str());
            return XBH_SUCCESS;
        } 
        else 
        {
            // 其他错误（如权限不足）
            XLOGD("Failed to access '%s': %s", file_path.c_str(), strerror(errno));
            return XBH_FAILURE;
        }
    }

    if (unlink(file_path.c_str()) == XBH_FAILURE) 
    {
        XLOGD("Failed to delete '%s': %s", file_path.c_str(), strerror(errno));
        return XBH_FAILURE;
    }

    XLOGD("Deleted: %s success", file_path.c_str());
    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::deleteDirectorySubfiles(const std::string& dir_path)
{
    DIR* dir = opendir(dir_path.c_str());
    if (!dir) {
        XLOGE("Failed to open '%s': %s", dir_path.c_str(), strerror(errno));
        return XBH_FAILURE;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // 跳过当前目录（.）和父目录（..），避免无限递归
        if (std::strcmp(entry->d_name, ".") == 0 || std::strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 构造完整路径（处理 dir_path 末尾是否有 '/' 的情况）
        std::string full_path;
        if (dir_path.back() == '/') {
            full_path = dir_path + entry->d_name;
        } else {
            full_path = dir_path + "/" + entry->d_name;
        }

        // 获取文件元信息（使用 lstat 避免跟随符号链接）
        struct stat st;
        if (lstat(full_path.c_str(), &st) == XBH_FAILURE) {
            XLOGE("Failed to access '%s': %s", full_path.c_str(), strerror(errno));
            closedir(dir);
            return XBH_FAILURE;
        }

        // 根据文件类型处理
        if (S_ISREG(st.st_mode)) {
            // 普通文件：直接删除
            if (deleteFile(full_path) == XBH_FAILURE) {
                XLOGE("Failed to deleteFile '%s': %s", full_path.c_str(), strerror(errno));
                closedir(dir);
                return XBH_FAILURE;
            }
        }else if (S_ISDIR(st.st_mode)) {
            // 子目录：递归删除其内容后，再删除空目录
            //注释掉删除目录的代码，因为涉及到Selinux权限问题,板控升级不需要删除目录,删除目录中的文件即可升级。
            if (deleteDirectorySubfiles(full_path) == XBH_FAILURE) {
                XLOGE("Failed to delete dir: '%s': %s", full_path.c_str(), strerror(errno));
                closedir(dir);
                return XBH_FAILURE;
            }
            // 删除空目录（此时子目录已清空）
            /* if (rmdir(full_path.c_str()) == -1) {
                XLOGE("Failed to delete empty dir: '%s': %s", full_path.c_str(), strerror(errno));
                closedir(dir);
                return XBH_FAILURE;
            } */
            XLOGD("Don't delete dir: '%s'", full_path.c_str());
        }else if (S_ISLNK(st.st_mode)) {
            // 符号链接：删除链接本身（不跟随）
            if (unlink(full_path.c_str()) == -1) {
                XLOGE("Failed to delete link file: '%s': %s", full_path.c_str(), strerror(errno));
                closedir(dir);
                return XBH_FAILURE;
            }
            XLOGD("Success to delete link file: '%s'", full_path.c_str());
        } else {
            // 其他类型文件（如设备文件、管道等），尝试删除
            if (unlink(full_path.c_str()) == -1) {
                XLOGE("Failed to delete special file: '%s': %s", full_path.c_str(), strerror(errno));
                closedir(dir);
                return XBH_FAILURE;
            }
            XLOGD("Success to delete special file: '%s'", full_path.c_str());
        }
    }

    closedir(dir);
    return XBH_SUCCESS;
}

/*
* 此函数实现的功能是在increase升级前的准备工作。
* 1. 删除/mnt/xbhcust目录下increase相关的文件
*/
XBH_S32 XbhRk_3576::prepareIncreaseBinUpgrade()
{
    XLOGD("Prepare for increase bin upgrade.Detele last increase bin related files\n");

    if (deleteFile(XBH_INCREASE_SYS_INI))
    {
        XLOGE("Failed to delete config file\n");
        return XBH_FAILURE;
    }

    if (deleteDirectorySubfiles(XBH_INCREASE_PQ_DIR))
    {
        XLOGE("Failed to delete directory: %s\n",XBH_INCREASE_PQ_DIR);
        return XBH_FAILURE;
    }

    XLOGD("Success to delete last increase bin files");

    return XBH_SUCCESS;
}

XBH_S32 XbhRk_3576::getEthPlugStatus(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XbhRk_3576::XbhRk_3576()
{
    XLOGD(" begin ");
    mIsProjectIdChange = XBH_FALSE;
    XbhPartitionData::getInstance()->setDataPath(MSDC_CUSDATA_PATH, MSDC_CUSPARAM_PATH);
    XBH_CHAR strProjectId[XBH_CUSDATA_PROJECT_ID_LEN] = {0};
    XbhPartitionData::getInstance()->getProjectId(strProjectId);
    XLOGD("getProjectId strProjectId=%s\n", strProjectId);
    property_set("persist.vendor.xbh.project_id", strProjectId);
    int projectid = atoi(strProjectId);
    initBacklightPwmConfig(projectid);
    //色温是否保存到不可擦除分区
    mColorTempStorage  = (XBH_BOOL)property_get_int32("persist.vendor.xbh.ColorTempStorage", 0);

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

    //init consoles
    initSerialPort();
    mCheckColorTemp =true;
    //gpio fd
    mGpioFd = open(GPIO_DEV_PATH, O_RDWR);
    if (mGpioFd < 0)
    {
        XLOGE("-- open device : /dev/xbh_gpio fail --\n");
    }

    //i2c lock
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        mLockIIC[i] = -1;
    }

    mComposer = IRkOutputManager::getService();
    if (mComposer != nullptr)
    {
        mComposer->initial();
    }
    else
    {
        XLOGD("Init failed to get IRkOutputManager");
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

    XBH_CHAR ColorTempPara[PROPERTY_VALUE_MAX] = {0};
    if (property_get("persist.vendor.xbh.ColorTempMode", ColorTempPara, NULL) > 0)
    {
        XBH_COLORTEMP_E enColorTemp = checkinitColorTempMode(ColorTempPara);
        if ( enColorTemp != XBH_COLORTEMP_BUTT)
        {
            //persist.vendor.xbh.ColorTempMode只起到第一次升级或者恢复出厂设置一次的作用，设置白平衡模式之后为空，之后正常开机系统会保存白平衡的模式
            property_set("persist.vendor.xbh.ColorTempMode", NULL);
            XBH_GAIN_OFFSET_DATA_S stInfo;
            setColorTempMode(enColorTemp);
            getColorTempPara(enColorTemp,&stInfo);
            //如果3576方案恢复出厂或升级第一次开机色温数据全是0则变成默认值
            checkColorTempValue(&stInfo);
            setColorTempPara(enColorTemp, &stInfo);
        }
    }

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
            XbhRk_3576::setPoweronMode(powerModeValue);
            property_set("persist.vendor.xbh.powermode", "-1");
        }
    }

    //根据版型选择烧写HDCP KEY的类型
    #if (TYPE_BOARD == XMR3576B)
    {
        hdcp_key_id = HDCP1X_KEY_DP_RK3588;
        //使能hdcp
        VmanModule::getInstance()->set_display_hdcp_status(1, XBH_TRUE);
    }
    #endif
    #if (TYPE_BOARD == XMR3576E)
    hdcp_key_id = HDCP1X_KEY_DP_RK3588;
    #endif
    #if (TYPE_BOARD == XMR3576U)
    hdcp_key_id = HDCP1X_KEY_DP_RK3588;
    #endif
    #if (TYPE_BOARD == XMR3588U)
    {
        hdcp_key_id = HDCP1X_KEY_HDMITX_RK3588;
        //使能hdcp
        VmanModule::getInstance()->set_display_hdcp_status(XBH_HDMI_A_2, XBH_TRUE);
    }
    #endif
    #ifdef XBH_USE_HAL_API_TO_CHECK_SIGNAL_STATUS
    property_set(XBH_HAL_CHECK_SIGNAL, "true");
    #endif
    XLOGD(" end ");
}

