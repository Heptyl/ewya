#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhMtk_9679"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

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

#include <cutils/properties.h>
#include <utils/String8.h>
#include "XbhSysOpt.h"

#include "XbhLog.h"
#include "XbhMtk_9679.h"
#include "XbhSysOpt.h"
#include "XbhParseINI.h"
#include "XbhService.h"
#include "XbhPartitionData.h"

#include "mi_common.h"
#include "mi_inputsrc.h"

#include "XbhNfcManager.h"
#include "XbhNetSwitchManager.h"
#include "XbhMcuManager.h"

using namespace android;
//----------------------------------------------------- static function begin ------------------------------------------------------------------------------------------------------
static XBH_U8 source_available[] = XBH_SRC_AVAILABLE_TABLE

#define MAX_HDCP_NAME_LENGTH    (20)

//gpio struct
typedef struct {
    XBH_U32 gpio;
    XBH_U32 state;
} UserData;


#define GPIO_DEV_PATH   "/dev/xbh_gpio"
#define ADC_DEV_PATH    "/sys/bus/iio/devices/iio:device0"
#define RTC_DEV_PATH    "/dev/rtc0"

#define HDMI_A_DEV_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/0/cable"
#define HDMI_B_DEV_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/1/cable"
#define HDMI_C_DEV_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/2/cable"
#define HDMI_D_DEV_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/3/cable"

#define HDMI_A_HPD_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/0/hpd"
#define HDMI_B_HPD_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/1/hpd"
#define HDMI_C_HPD_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/2/hpd"
#define HDMI_D_HPD_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/3/hpd"

#define HDMI_A_CLK_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/0/clock_detect"
#define HDMI_B_CLK_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/1/clock_detect"
#define HDMI_C_CLK_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/2/clock_detect"
#define HDMI_D_CLK_PATH  "/sys/devices/platform/mtk_srccap0/mtk_dbg/hdmirx/3/clock_detect"


#define MSDC_CUSDATA_PATH  "/dev/block/mmcblk0p7" //cusdata
#define MSDC_CUSPARAM_PATH "/dev/block/mmcblk0p8" //cusparam

#define PHY_WOL_PATH     "/sys/mtk_pm/wakeup_source/lan"
#define PHY_WOS_PATH     "/sys/mtk_pm/wakeup_source/wos"
#define PHY_WOW_PATH     "/sys/mtk_pm/wakeup_source/wifi-gpio"
#define PHY_CEC_PATH     "/sys/mtk_pm/wakeup_source/cec0"
#define PHY_PIR1_PATH    "/sys/mtk_pm/wakeup_source/pir1"
#define PHY_PIR2_PATH    "/sys/mtk_pm/wakeup_source/pir2"
#define PHY_FTYPEC_PATH  "/sys/mtk_pm/wakeup_source/ftypec"
#define PHY_BTYPEC_PATH  "/sys/mtk_pm/wakeup_source/btypec"
#define PHY_NFC_PATH     "/sys/mtk_pm/wakeup_source/nfc"
#define PHY_RTC_PATH     "/sys/mtk_pm/wakeup_source/rtc1"
#define PHY_IR_PATH      "/sys/mtk_pm/wakeup_source/ir0"
#define PHY_DP_PATH      "/sys/mtk_pm/wakeup_source/dp"
#define WAKE_UP_REASON   "/sys/mtk_pm/wakeup_reason/name"
#define RJ45_SPEED_PATH   "/sys/class/net/eth0/speed"
#define RJ45_DEFAULT_VALUE 10
#define NTC_ADC_100K_0402_MIN -40
#define NTC_ADC_100K_0402_LEN 141

#define BACKLIGHT_SYSFS  "/sys/devices/platform/mediatek-drm-tv-kms/panel/mtk_tv_drm_backlight_sysfs"

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


static XBH_S32 read_to_path(const XBH_CHAR* path, XBH_BOOL* bEnable)
{
    XBH_S32 fd = -1;
    XBH_CHAR state[2] = {0};
    XBH_S32 ret = XBH_FAILURE;
    fd = open(path, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        XLOGE("open %s failed !!!!",path);
        return XBH_FAILURE;
    }
    ret =  read(fd,state,1);
    if(strncmp(state,"1",1) == 0)
    {
        *bEnable = XBH_TRUE;
    }
    else if(strncmp(state,"0",1) == 0)
    {
        *bEnable = XBH_FALSE;
    }
    close(fd);
    return ret;
}

static XBH_S32 write_to_path(const XBH_CHAR* path, XBH_S32 bEnable) 
{
    XBH_S32 fd = -1;
    fd = open(path, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        XLOGD("open %s failed !!!!\n", path);
        return XBH_FAILURE;
    }
    write(fd,bEnable == XBH_TRUE ? "1" : "0", 1);
    close(fd);
    return XBH_SUCCESS;
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
XBH_S32 XbhMtk_9679::setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level)
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


XBH_S32 XbhMtk_9679::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
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

    if(XBH_TCON6 == u32GpioNumber && u32GpioValue == 1)
    {
        s32Ret = setGpioInput(u32GpioNumber);
        XLOGV("-- gpio %d is OD pin set input now", u32GpioNumber);
        return s32Ret;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;

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

    s32Ret = ioctl(mGpioFd, SET_IO_VAL, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- set gpio-%d out value : %d fail --",userdata.gpio,userdata.state);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
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

    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_OUT_VAL, &userdata);
    if(s32Ret < 0){
        XLOGE("-- get gpio-%d out value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *u32GpioValue = userdata.state;
    return s32Ret;

}

XBH_S32 XbhMtk_9679::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
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

    if(getGpioDir(u32GpioNumber) == XBH_BOARD_GPIO_OUTPUT){
        XLOGV("-- gpio %d is not input pin set output now",u32GpioNumber);
        setGpioInput(userdata.gpio);
    }

    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_VAL, &userdata);
    if(s32Ret < 0){
        XLOGE("-- get gpio-%d value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *u32GpioValue = userdata.state;
    return s32Ret;
}

XBH_U8 XbhMtk_9679::calMcuGpioChecksum(XBH_MCU_GPIOPARA_U *gpioPara)
{
    return (gpioPara->attr.port + gpioPara->attr.pin + gpioPara->attr.mode + gpioPara->attr.pupd + gpioPara->attr.otype + gpioPara->attr.speed + gpioPara->attr.level) & 0xff;
}


XBH_S32 XbhMtk_9679::getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level)
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
    XLOGD("--set_i2c_data cmd: %02x, len: %d, data %02x:%02x:%02x:%02x ,level:%d\n", stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3],unGpioGet.attr.level);
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
    XLOGD("--get_i2c_data port: %d, pin: %d, level:%d\n", unGpioGet.attr.port ,unGpioGet.attr.pin ,unGpioGet.attr.level);
    return ret;
}


XBH_S32 XbhMtk_9679::setIICLock(XBH_U32 iicBus, XBH_BOOL lock)
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

XBH_S32 XbhMtk_9679::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
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

XBH_S32 XbhMtk_9679::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
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

XBH_S32 XbhMtk_9679::getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable)
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

XBH_S32 XbhMtk_9679::setDebugEnable(XBH_BOOL bEnable)
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
        strcpy(ttyPath, "/dev/ttyS0");
        ttyfd = open(ttyPath, O_RDWR | O_NOCTTY | O_NONBLOCK);
        ret = tcsetattr(ttyfd, TCSANOW, &uart_debug_config);
        close(ttyfd);
        if(ret == 0)
        {
            property_set("ctl.start", "console");
        }
    }
    else
    {
        property_set("ctl.stop", "console");
    }
    return s32Ret;

}

XBH_S32 XbhMtk_9679::getDebugEnable(XBH_BOOL *bEnable)
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


XBH_S32 XbhMtk_9679::setHdmiHPD(int bd_src, XBH_LEVEL_E level)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int fp = 0;
    char buf[4];

    if(bd_src == 0)
    {
        fp = open(HDMI_A_HPD_PATH, O_WRONLY);
    }
    else if(bd_src == 1)
    {
        fp = open(HDMI_B_HPD_PATH, O_WRONLY);
    }
    else if(bd_src == 2)
    {
        fp = open(HDMI_C_HPD_PATH, O_WRONLY);
    }
    else if(bd_src == 3)
    {
        fp = open(HDMI_D_HPD_PATH, O_WRONLY);
    }

    if(fp == -1)
    {
        XLOGE("open src = %d error!\n", bd_src);
        return XBH_FAILURE;
    }

    if(level == XBH_LEVEL_HIGH)
    {
       s32Ret = write(fp, "high", 4);

    }
    else if(level == XBH_LEVEL_LOW)
    {
       s32Ret = write(fp, "low", 3);
    }

    if(s32Ret == -1)
    {
        XLOGE("write HDMI_%d_HPD_PATH error!\n", bd_src);
        close(fp);
        return XBH_FAILURE;
    }

    close(fp);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getHdmiDirectDet(int bd_src, XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_SUCCESS;
    int fp = 0;
    char buf[4];

    if(bd_src == 0)
    {
        fp = open(HDMI_A_DEV_PATH, O_RDONLY);
    }
    else if(bd_src == 1)
    {
        fp = open(HDMI_B_DEV_PATH, O_RDONLY);
    }
    else if(bd_src == 2)
    {
        fp = open(HDMI_C_DEV_PATH, O_RDONLY);
    }
    else if(bd_src == 3)
    {
        fp = open(HDMI_D_DEV_PATH, O_RDONLY);
    }

    if(fp == -1)
    {
        XLOGE("open src = %d error!\n", bd_src);
        return XBH_FAILURE;
    }
    XBH_S32 s32Ret = read(fp, buf, 4);
    if(s32Ret == -1)
    {
        XLOGE("write HDMI_%d_DEV_PATH error!\n", bd_src);
        close(fp);
        return XBH_FAILURE;
    }
    *enable = (XBH_BOOL)atoi(buf);
    close(fp);
    return ret;
}

XBH_S32 XbhMtk_9679::getCvbsDirectDet(int bd_src, XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_SUCCESS;
    return ret;
}

XBH_S32 XbhMtk_9679::getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    int fp = 0;
    char buf[10];
    char path[128];
    sprintf(path, "%s/in_voltage%d_raw", ADC_DEV_PATH, enChannel);
    //XLOGI("[%s:%d] Get Adc path =%s\r\n", __func__, __LINE__, path);

    fp = open(path, O_RDONLY);
    if(fp == -1)
    {
        XLOGE("open ADC_DEV_PATH error!\n");
        return XBH_FAILURE;
    }

    XBH_S32 s32Ret = read(fp, buf, 10);
    if(s32Ret == -1)
    {
        XLOGE("read ADC_DEV_PATH error!\n");
        return XBH_FAILURE;
    }

    *u32Value = atof(buf);
    close(fp);

    // XLOGD("[%s:%d] Get Adc voltage value %d=%u\r\n", __func__, __LINE__, u32Channel, *u32Value);
    return ret;
}

XBH_S32 XbhMtk_9679::getTemperatureSensorValue(XBH_FLOAT *fValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_ADC_CHANNEL_NUM_E adcChannel = XBH_BOARD_NTC_ADC_THERMISTOR_CHAN;
    XBH_U32 adcValue   = 0;
    s32Ret = getADCChannelValue(adcChannel, &adcValue);

    XBH_U32 value = adcValue;
    if ((1023 - value) > 0) {
        XBH_U32 tAdcVal = XBH_BOARD_NTC_ADC_THERMISTOR_VAL * 1000 * value / (1023 - value);
        XBH_S32 tempIndex = 0;
        for(tempIndex=0; tempIndex < NTC_ADC_100K_0402_LEN; tempIndex++)
        {
            if(tAdcVal > NTC_ADC_100K_0402[tempIndex])
            {
               *fValue = tempIndex - 40;
                //XLOGD("getBoardTemperature : ADC-CH%u = %u  tempIndex = %d convert to degree centigrade : %d",adcChannel, tempIndex, *fValue);
                s32Ret = XBH_SUCCESS;
                break;
            }
        }
        if(tempIndex >= NTC_ADC_100K_0402_LEN)
        {
            *fValue = 233;
            XLOGE("getBoardTemp: Invalid value = %d", value);
        }
    } else {
        XLOGE("getBoardTemp: get adc failed!!! value = %d", value);
        *fValue = 233;
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getRtcTime(XBH_RTC_INFO_S *stRtcInfo)
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

XBH_S32 XbhMtk_9679::setRtcTime(XBH_RTC_INFO_S *stRtcInfo)
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

XBH_S32 XbhMtk_9679::setRtcAlarmTime(XBH_RTC_ALARM_INFO_S *stRtcAlarmInfo)
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
    std::shared_ptr<void> devPtr(nullptr,xbh_fd_closer(fd));
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
            return XBH_FAILURE;
        }
        XLOGD("RTC_AIE_ON RTC_AIE_ON \n");
        s32Ret = ioctl(fd, RTC_AIE_ON, 0);
        if(s32Ret < 0)
        {
            XLOGE("RTC_AIE_ON ioctl failed: %s\n", strerror(errno));
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
            return XBH_FAILURE;
        }
    }
    setWoTStatus(XBH_WAKEUP_RTC,alarm.enabled);
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::getTemperatureSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_ADC_CHANNEL_NUM_E adcChannel = XBH_BOARD_NTC_ADC_THERMISTOR_CHAN;
    XBH_U32 adcValue = 0;
    s32Ret = getADCChannelValue(adcChannel, &adcValue);
    if(adcValue >= 1000)
    {
        *bEnable = 0;
    }else{
        *bEnable = 1;
    }
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::getWoTStatus(XBH_WAKEUP_E enWakeUpType ,XBH_BOOL *enable)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_S32 fd = -1;
    XBH_S32 fd1 = -1;
    XBH_CHAR state[2] = {0};
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
        if(property_get("persist.vendor.xbh.wol.enable", propVal, "false") > 0)
        {
            if(strcmp(propVal, "false") == 0)
            {
                *enable = XBH_FALSE;
            } else {
                *enable = XBH_TRUE;
            }
            ret = XBH_SUCCESS;
        }
        break;
        case XBH_WAKEUP_SOURCE:
            fd = read_to_path(PHY_WOS_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_WIFI:
            fd = read_to_path(PHY_WOW_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_PIR:
            fd = read_to_path(PHY_PIR1_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_PIR1:
            fd = read_to_path(PHY_PIR2_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_BTYPEC:
            fd = read_to_path(PHY_BTYPEC_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_FTYPEC:
            fd = read_to_path(PHY_FTYPEC_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_NFC:
            fd = read_to_path(PHY_NFC_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_RTC:
            fd = read_to_path(PHY_RTC_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_IR:
            fd = read_to_path(PHY_IR_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        case XBH_WAKEUP_DP:
            fd = read_to_path(PHY_DP_PATH, enable);
            if(fd < 0)
            {
                ret = XBH_FAILURE;
            }
        break;
        default:
            XLOGE("no type!");
        break;
    }
    return ret;
}


XBH_S32 XbhMtk_9679::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    XBH_S32 fd1 = -1;
    XBH_CHAR state[2] = {0};
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    memset(s8Buff, 0, sizeof(s8Buff));
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue(s8Buff);
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            property_set("persist.vendor.xbh.wol.enable",bEnable == XBH_TRUE ? "true" : "false");
            fd = write_to_path(PHY_WOL_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            #ifdef ENABLE_PMU_WOL_GPIO_WK
                XbhNetSwitchManager::getInstance()->setChipWol(bEnable);
            #endif
            s8Buff[XBH_WAKEUP_ETH] = (bEnable ? 1 : 0);
        break;
        case XBH_WAKEUP_SOURCE:
            fd = write_to_path(PHY_WOS_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            //是否独立开关控制TypeC待机唤醒:true表示独立开关，不受WOS开关控制;false表示WOS开关关联控制TypeC待机唤醒
            if(property_get("persist.vendor.xbh.indepen.source", propVal, "false") > 0)
            {
                if(strcmp(propVal, "false") == 0)
                {
                    write_to_path(PHY_FTYPEC_PATH,bEnable);
                    s8Buff[XBH_WAKEUP_FTYPEC] = (bEnable ? 1 : 0);
                    write_to_path(PHY_BTYPEC_PATH,bEnable);
                    s8Buff[XBH_WAKEUP_BTYPEC] = (bEnable ? 1 : 0);
                }
            }
            s8Buff[XBH_WAKEUP_SOURCE] = (bEnable ? 1 : 0);
        break;
        case XBH_WAKEUP_WIFI:
            fd = write_to_path(PHY_WOW_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_WIFI] = (bEnable ? 1 : 0);
        break;
        case XBH_WAKEUP_CEC:
            s8Buff[XBH_WAKEUP_CEC] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_PIR:
            fd = write_to_path(PHY_PIR1_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_PIR] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_PIR1:
            fd = write_to_path(PHY_PIR2_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_PIR1] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_FTYPEC:
            fd = write_to_path(PHY_FTYPEC_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_FTYPEC] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_BTYPEC:
            fd = write_to_path(PHY_BTYPEC_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_BTYPEC] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_NFC:
            fd = write_to_path(PHY_NFC_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_NFC] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_RTC:
            fd = write_to_path(PHY_RTC_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_RTC] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_IR:
            fd = write_to_path(PHY_IR_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_IR] = (bEnable ? 1 : 0);
            break;
        case XBH_WAKEUP_DP:
            fd = write_to_path(PHY_DP_PATH,bEnable);
            if(fd < 0)
            {
                return XBH_FAILURE;
            }
            s8Buff[XBH_WAKEUP_DP] = (bEnable ? 1 : 0);
            break;
        default:
            XLOGE("no type!");
        break;
    }
    s32Ret = XbhPartitionData::getInstance()->setWotParamValue(s8Buff);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setDeviceId(const XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->setDeviceId(strDeviceId);
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::getDeviceId(XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->getDeviceId(strDeviceId);
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

//set ethernet mac address
XBH_S32 XbhMtk_9679::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setMacAddress(strMacAddress);
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getMacAddress(strMacAddress);
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret  = XbhPartitionData::getInstance()->setHDCPKey_14((XBH_VOID *)pBuff);
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        s32Ret  = XbhPartitionData::getInstance()->setHDCPKey_22((XBH_VOID *)pBuff);
    }
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
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

XBH_S32 XbhMtk_9679::setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type)
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

XBH_S32 XbhMtk_9679::getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName)
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

XBH_S32 XbhMtk_9679::setGoogleKeyName(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setGoogleKeyName((XBH_VOID *)strPath);
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::getGoogleKeyName(XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getGoogleKeyName((XBH_VOID *)strPath);
    return  s32Ret;
}


XBH_S32 XbhMtk_9679::setAgingModeState(XBH_S32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_AGING_MODE_STATE_LEN] = {0};
    if (u32Value == 1)
    {
        s8Buff[0] = 1;
    }
    else
    {
        s8Buff[0] = 0;
    }
    s32Ret = XbhPartitionData::getInstance()->setAgingModeState((XBH_VOID *)s8Buff);
    XLOGD("setAgingModeState s8Buff[0]= %d \n", s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getAgingModeState(XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_AGING_MODE_STATE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getAgingModeState((XBH_VOID *)s8Buff);
    XLOGD("getAgingModeState s8Buff[0]= %d \n", s8Buff[0]);
    *u32Value =  s8Buff[0] == 1 ? 1 : 0;
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setMokaPModeEnableToCusdata(XBH_S32 u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MOKA_P_MODE_ENABLE_LEN] = {0};
    if (u32Value == 1)
    {
        s8Buff[0] = 1;
    }
    else
    {
        s8Buff[0] = 0;
    }
    s32Ret = XbhPartitionData::getInstance()->setMokaPModeEnableToCusdata((XBH_VOID *)s8Buff);
    XLOGD("setMokaPModeEnableToCusdata s8Buff[0]= %d \n", s8Buff[0]);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getMokaPModeEnableFromCusdata(XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MOKA_P_MODE_ENABLE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getMokaPModeEnableFromCusdata((XBH_VOID *)s8Buff);
    XLOGD("getMokaPModeEnableFromCusdata s8Buff[0]= %d \n", s8Buff[0]);
    *u32Value =  s8Buff[0] == 1 ? 1 : 0;
    return s32Ret;
}


//get buildin ethernet mac address
XBH_S32 XbhMtk_9679::getBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* strMacAddress)
{
    XBH_S32 ret = XBH_SUCCESS;
    int fp = 0;
    char buf[XBH_CUSDATA_MACADDRESS_LEN] = {0};

    fp = open(strPath, O_RDONLY);
    if(fp == -1)
    {
        XLOGE("open patch = %s error!\n", strPath);
        return XBH_FAILURE;
    }
    XBH_S32 s32Ret = read(fp, buf, XBH_CUSDATA_MACADDRESS_LEN);
    if(s32Ret == -1)
    {
        XLOGE("read %s error!\n", strPath);
        close(fp);
        return XBH_FAILURE;
    }

    memcpy(strMacAddress, buf, XBH_CUSDATA_MACADDRESS_LEN-1);
    ret = checkMacFormat(strMacAddress);
    close(fp);
    return ret;
}

XBH_S32 XbhMtk_9679::loadEdlaCust(XBH_BOOL *enable)
{
    XBH_S32 res = 0;
    res = loadProperty();
    //customerversion 一致不需要重启
    if (res == 1) {
        *enable = XBH_FALSE;
    } else {
        *enable = XBH_TRUE;
    }
    //loadPq();
    return res;
}

//Set serial number
XBH_S32 XbhMtk_9679::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setSn(strSn);
    property_set("vendor.mtk.rkp.serialno", strSn);
    return  s32Ret;
}

//Get serial number
XBH_S32 XbhMtk_9679::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSn(strSn);
    return  s32Ret;
}

//Set Chip RunTime
XBH_S32 XbhMtk_9679::setChipRunTime(XBH_S32 time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setChipRunTime(time);
    return  s32Ret;
}

//Get Chip RunTime
XBH_S32 XbhMtk_9679::getChipRunTime(XBH_S32* time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getChipRunTime(time);
    return  s32Ret;
}

//set standby mode
//0: shutdown 1: screenoff 2: str
XBH_S32 XbhMtk_9679::setStandbyMode(XBH_U32 u32Mode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32Mode);
    property_set("persist.vendor.xbh.power_standby_mode", type);

    XLOGW("setStandbyMode Mode=%d", u32Mode);
    return s32Ret;
}

//get standy mode
XBH_S32 XbhMtk_9679::getStandbyMode(XBH_U32 *u32Mode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *u32Mode = property_get_int32("persist.vendor.xbh.power_standby_mode", 1);
    XLOGW("getStandbyMode Mode=%d", *u32Mode);
    return s32Ret;
}

//----------------------------------------------------------------------------- private func start here -----------------------------------------------------------------
XBH_S32 XbhMtk_9679::open_i2c_dev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet)
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

XBH_S32 XbhMtk_9679:: check_iic_funcs(XBH_S32 file)
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

XBH_S32 XbhMtk_9679::set_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
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

XBH_S32 XbhMtk_9679::get_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
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

XBH_S32 XbhMtk_9679::initSerialPort()
{
    XBH_S32 ttyfd;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR ttyPath[64] = {0};
    strcpy(ttyPath, "/dev/ttyS0");
    ttyfd = open(ttyPath,O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(tcgetattr(ttyfd, &uart_debug_config))
    {
        XLOGE("tcgetattr error\n");
    }
    close(ttyfd);

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

XBH_S32 XbhMtk_9679::setGpioOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mGpioFd < 0){
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;

    s32Ret = ioctl(mGpioFd, SET_IO_OUT, &userdata);
    if(s32Ret < 0){
        XLOGE("-- set gpio-%d out value : %d fail --",userdata.gpio, userdata.state);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setGpioInput(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mGpioFd < 0){
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;

    s32Ret = ioctl(mGpioFd, SET_IO_IN, &userdata);
    if(s32Ret < 0){
        XLOGE("-- set gpio-%d input : %d fail --",userdata.gpio);
        return XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhMtk_9679::getGpioDir(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mGpioFd < 0){
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;

    s32Ret = ioctl(mGpioFd, GET_IO_DIR, &userdata);
    if(s32Ret < 0){
        XLOGE("-- get gpio-%d dir fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    return userdata.state;
}

XBH_S32 XbhMtk_9679::loadProperty()
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
    free(entries);
    return flag;
}

XBH_S32 XbhMtk_9679::loadWoTStatus()
{
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.sync_wot_flag", propVal, "false") > 0)
    {
        //synchronize the default standby wake-up status to cusparam
        if(strcmp(propVal, "false") == 0)
        {
            //eth
            property_get("persist.vendor.xbh.wol.enable", propVal, "false");
            setWoTStatus(XBH_WAKEUP_ETH, (strcmp(propVal, "true") == 0) ? XBH_TRUE : XBH_FALSE);
            //wifi
            property_get("persist.vendor.xbh.wow.enable", propVal, "false");
            setWoTStatus(XBH_WAKEUP_WIFI, (strcmp(propVal, "true") == 0) ? XBH_TRUE : XBH_FALSE);
            //wos
            property_get("persist.vendor.xbh.wos.enable", propVal, "false");
            setWoTStatus(XBH_WAKEUP_SOURCE, (strcmp(propVal, "true") == 0) ? XBH_TRUE : XBH_FALSE);
            //cec
            property_get("persist.vendor.xbh.cec_wakeup.enable", propVal, "false");
            setWoTStatus(XBH_WAKEUP_CEC, (strcmp(propVal, "true") == 0) ? XBH_TRUE : XBH_FALSE);

            property_set("persist.vendor.xbh.sync_wot_flag", "true");
        }
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::initWolStatus() {
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.wol.enable", propVal, "false");
    return write_to_path(PHY_WOL_PATH, (strcmp(propVal, "true") == 0) ? XBH_TRUE : XBH_FALSE);
}

XBH_S32 XbhMtk_9679::isNfcExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getChipExist(enable);
    XLOGW("isNfcExist s32Ret = %d", s32Ret);
   return s32Ret;
}

XBH_S32 XbhMtk_9679::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    property_set("persist.vendor.xbh.nfc.enable",enable == XBH_TRUE ? "true" : "false");
    s32Ret = XbhNfcManager::getInstance()->setNfcEnable(enable);
    XLOGW("setNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.nfc.enable", propVal, "false") > 0)
    {
        if(strcmp(propVal, "false") == 0)
        {
            *enable = XBH_FALSE;
        } else {
            *enable = XBH_TRUE;
        }
        s32Ret = XBH_SUCCESS;
    }
    XLOGW("getNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getNfcId(data, length);
    XLOGW("getNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->addNfcId(data, length);
    XLOGW("addNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delNfcId(data, length);
    XLOGW("delNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delAllNfcId();
    XLOGW("delAllNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuFattMode(mode);
    XLOGW("setMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuFattMode(mode);
    XLOGW("getMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuUartBypassPara(u8UartNum, baud);
    XLOGW("setMcuUartBypassPara s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuUartBypass(u8Len, u8Data);
    XLOGW("setMcuUartBypass s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuUartBypass(u8Len, u8Data);
    XLOGW("getMcuUartBypass s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getSocHdmiEdidStatus(XBH_BOOL *enable)
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

XBH_S32 XbhMtk_9679::setOpsReidentify(XBH_BOOL enable)
{
    if (enable)
    {
        property_set("vendor.mtk.nosignal.reidentify", "1");
        s32OpsReidentify = 1;
    }
    else
    {
        property_set("vendor.mtk.nosignal.reidentify", "0");
        s32OpsReidentify = 0;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::processOpsSignalStatus(XBH_SOURCE_E u32Source)
{
    switch(u32Source)
    {
        case XBH_SOURCE_OPS1:
            if (s32OpsReidentify)
            {
                int reidentify = property_get_int32("vendor.mtk.reidentify", 0);
                if (reidentify)
                {
                    setHdmiHPD(0, XBH_LEVEL_LOW);
                    usleep(500 * 1000);
                    setHdmiHPD(0, XBH_LEVEL_HIGH);
                    s32OpsReidentify = 0;
                    property_set("vendor.mtk.reidentify", "0");
                }
            }
            break;
        default:
            break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_S32 readRet = XBH_SUCCESS;
    XBH_S32 fd = -1;
    XBH_CHAR state[10] = {0};
    fd = open(WAKE_UP_REASON, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        XLOGE("open wakeup reason error!!\n");
        return XBH_FAILURE;
    }
    readRet = read(fd,state,10);
    if (strncmp(state,"bhdmi1",6) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_SOURCE;
        stWake->enSrc = XBH_SOURCE_HDMI1;
    }
    else if(strncmp(state,"bhdmi2",6) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_SOURCE;
        stWake->enSrc = XBH_SOURCE_HDMI2;
    }
    else if(strncmp(state,"dp",2) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_SOURCE;
        stWake->enSrc = XBH_SOURCE_DP1;
    }
    else if(strncmp(state,"btypec",6) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_SOURCE;
        stWake->enSrc = XBH_SOURCE_USBC1;
    }
    else if(strncmp(state,"fhdmi",5) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_SOURCE;
        stWake->enSrc = XBH_SOURCE_F_HDMI1;
    }
    else if(strncmp(state,"ftypec",6) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_SOURCE;
        stWake->enSrc = XBH_SOURCE_F_USBC1;
    }
    else if(strncmp(state,"lan",3) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_ETH;
    }
    else if(strncmp(state,"ir0",3) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_IR;
    }
    else if(strncmp(state,"rtc0",4) == 0 || strncmp(state,"rtc1",4) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_RTC;
    }
    else if(strncmp(state,"uart",4) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_UART;
    }
    else if(strncmp(state,"keypad0",7) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_KEYPAD;
    }
    else if( strncmp(state,"cec0",4) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_CEC;
    }
    else if(strncmp(state, "nfc", 3) == 0)
    {
        stWake->enWakeup = XBH_WAKEUP_NFC;
    }
    else
    {
        XLOGE("getWakeUpReason fail %s\n",state);
        ret = XBH_FAILURE;
    }
    XLOGE("getWakeUpReason state=%s,stWake->enWakeup=%d,ret=%d\n",state,stWake->enWakeup,ret);
    close(fd);
    return ret;
}

XBH_S32 XbhMtk_9679::setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL lienoutSingleAdjust = property_get_bool("persist.vendor.xbh.lineout_single_adjust", false);
    XBH_BOOL status = XBH_FALSE;
    XBH_BOOL supportOutputBothFlag = property_get_bool("persist.vendor.xbh.output.both.support", false);

    XbhService::getModuleInterface()->getHpDetectStatus(&status);
    mAudioOutput = (XBH_AUDIO_OUTPUT_E) property_get_int32("persist.vendor.xbh.audio_output", 0);
    XLOGD("setAudioOutput lienoutSingleAdjust=%d,status=%d,enAudioOutput=%d,mAudioOutput=%d,supportOutputBothFlag=%d",
        lienoutSingleAdjust,status,enAudioOutput,mAudioOutput,supportOutputBothFlag);
    if (lienoutSingleAdjust || supportOutputBothFlag)
    {
        if (enAudioOutput == XBH_AUDIO_OUTPUT_BOTH)
        {
            property_set("persist.vendor.xbh.spk.lineout.sync", "true");
        }
        else
        {
            property_set("persist.vendor.xbh.spk.lineout.sync", "false");
        }
    }
    if (mAudioOutput != enAudioOutput)
    {
        switch (enAudioOutput)
        {
            default:
            case XBH_AUDIO_OUTPUT_DET:
            {
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
        }
    }
    mAudioOutput = enAudioOutput;
    char strAudioOutput[8]={0};
	sprintf(strAudioOutput, "%d", enAudioOutput);
	property_set("persist.vendor.xbh.audio_output",strAudioOutput);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    mAudioOutput = (XBH_AUDIO_OUTPUT_E) property_get_int32("persist.vendor.xbh.audio_output", 0);
    *enAudioOutput = mAudioOutput;
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setSourceScreenOffset(XBH_S32 x, XBH_S32 y)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if( x == 0 && y == 0 ){
        property_set("vendor.xbh.source_offset_enable", "false");
    }else{
        property_set("vendor.xbh.source_offset_enable", "true");
    }
    XBH_FLOAT offsetX = (XBH_FLOAT) x/3840.f;
    XBH_FLOAT offsetY = (XBH_FLOAT) y/2160.f;
    XBH_CHAR offsetXVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR offsetYVal[PROPERTY_VALUE_MAX] = {0};
    snprintf(offsetXVal, sizeof(offsetXVal), "%f", offsetX);
    snprintf(offsetYVal, sizeof(offsetXVal), "%f", offsetY);
    property_set("vendor.xbh.source_offset_x", offsetXVal);
    property_set("vendor.xbh.source_offset_y", offsetYVal);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setBacklightWithOutSave(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel)
{
    XBH_CHAR name[64];
    XBH_CHAR write_str[64];
    XBH_S32 fd = -1;
    snprintf(name, sizeof(name)-1, "/sys/class/pwm/pwmchip1/export");
    fd = open(name, O_WRONLY);
    snprintf(write_str, sizeof(write_str)-1, "%d", 0);
    write(fd, write_str, sizeof(write_str));//enable pwm
    close(fd);
    //duty_cycle
    snprintf(name, sizeof(name), "/sys/class/pwm/pwmchip1/pwm0/duty_cycle");
    fd = open(name, O_WRONLY);
    if (fd < 0) {
        XLOGE("Failed to open duty_cycle file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    u32Value = (6250000/100)*u32Value;
    snprintf(write_str, sizeof(write_str)-1, "%d", u32Value);
    write(fd, write_str, sizeof(write_str));
    close(fd);
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR offsetXVal[PROPERTY_VALUE_MAX] = {0};
    property_get("vendor.xbh.source_offset_x", offsetXVal, "0.0");
    XBH_CHAR offsetYVal[PROPERTY_VALUE_MAX] = {0};
    property_get("vendor.xbh.source_offset_y", offsetYVal, "0.0");
    XBH_FLOAT floatOffsetX = std::stof(offsetXVal);
    XBH_FLOAT floatOffsetY = std::stof(offsetYVal);
    *x = (XBH_S32) (floatOffsetX * 3840.f);
    *y = (XBH_S32) (floatOffsetY * 2160.f);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setSocFreeRun(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_TRUE;
    int writeRet;
    int fd = open("/sys/devices/platform/mediatek-drm-tv-kms/mtk_dbg/force_freerun", O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        XLOGE("MYH open force_freerun failed");
        return XBH_FAILURE;
    }
    if(enable)
    {
        writeRet = write(fd, "1", 1);
    }
    else
    {
        writeRet = write(fd, "0", 1);
    }
    if(writeRet == -1)
    {
        XLOGE("MYH write force_freerun failed");
        s32Ret = XBH_FAILURE;
    }

    close(fd);
    XLOGD("write Free run end");
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel)
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

XBH_S32 XbhMtk_9679::getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSDATA_MULTIPANE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getMultiPanel((XBH_VOID *)s8Buff);
    *type =  s8Buff[0] == 2 ? 1 : 0;
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setHdmiTxVideoMuteState(XBH_BOOL state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_LEN] = {0};
    if (state)
    {
        s8Buff[0] = 1;
    }
    else
    {
        s8Buff[0] = 0;
    }
    s32Ret = XbhPartitionData::getInstance()->setHdmiTxAVMute((XBH_VOID *)s8Buff);
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getHdmiTxVideoMuteState(XBH_BOOL *state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s8Buff[XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getHdmiTxAVMute((XBH_VOID *)s8Buff);
    *state = s8Buff[0];
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_CHAR name[64] = {0};
    XBH_CHAR write_str[64] = {0};
    XBH_S32 fd = -1;
    //enable pwm
    snprintf(name, sizeof(name)-1, "/sys/class/pwm/pwmchip1/export");
    fd = open(name, O_WRONLY);
    if (fd < 0)
    {
        XLOGE("Failed to open pwmchip1/export file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    snprintf(write_str, sizeof(write_str)-1, "%d", 0);
    write(fd, write_str, sizeof(write_str));
    close(fd);
    //set polarity
    snprintf(name, sizeof(name), "/sys/class/pwm/pwmchip1/pwm0/polarity");
    fd = open(name, O_WRONLY);
    if (fd < 0) {
        XLOGE("Failed to open polarity file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    memset(write_str,0,sizeof(write_str));
    if(bEnable == XBH_TRUE)
    {
        snprintf(write_str, sizeof(write_str)-1, "%s", "inversed");
    }
    else
    {
        snprintf(write_str, sizeof(write_str)-1, "%s", "normal");
    }
    write(fd, write_str, sizeof(write_str));
    close(fd);
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_UNUSED(enPanel);
    XBH_CHAR name[64] = {0};
    XBH_CHAR write_str[16] = {0};
    XBH_CHAR read_str[16] = {0};
    XBH_S32 fd = -1;
    //enable pwm
    snprintf(name, sizeof(name)-1, "/sys/class/pwm/pwmchip1/export");
    fd = open(name, O_WRONLY);
    if (fd < 0)
    {
        XLOGE("Failed to open pwmchip1/export file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    snprintf(write_str, sizeof(write_str)-1, "%d", 0);
    write(fd, write_str, sizeof(write_str));
    close(fd);
    //get polarity
    snprintf(name, sizeof(name), "/sys/class/pwm/pwmchip1/pwm0/polarity");
    fd = open(name, O_RDONLY);
    if (fd < 0) {
        XLOGE("Failed to open polarity file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    read(fd, &read_str, sizeof(read_str));
    close(fd);
    if(strncmp(read_str, "inversed", 8) == 0)
    {
        *bEnable = XBH_TRUE;
    }
    else if (strncmp(read_str, "normal", 6) == 0)
    {
        *bEnable = XBH_FALSE;
    }
    else
    {
        return XBH_FAILURE;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_9679::getPanelResolution(XBH_CHAR* resolution, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    strcpy(resolution, "3840x2160");
    XLOGD("XbhMtk_9679::getPanelResolution: %s\n", resolution);
    return  s32Ret;
}

/**
 * 写入Project ID
 * param[in] value Project ID
 * retval true:success, false:failure
*/
XBH_S32 XbhMtk_9679::setProjectId(const XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setProjectId(strProjectId);
    return  s32Ret;
}

/**
 * 获取Project ID
 * param[out] strProjectId Project ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_9679::getProjectId(XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getProjectId(strProjectId);
    return  s32Ret;
}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_9679::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setColorTempPara(enColorTemp, data);
    return  s32Ret;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_9679::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getColorTempPara(enColorTemp, data);
    return  s32Ret;
}

/**
 * 获取有线网络插入状态
 * param[out] bEnable. 有线网插入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_9679::getEthPlugStatus(XBH_BOOL* bEnable)
{
    if (bEnable == nullptr) {
        XLOGE("bEnable is NULL\n");
        return XBH_FAILURE;
    }

    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR name[64] = {0};
    XBH_CHAR read_str[16] = {0};
    XBH_U32 speed = 0;
    XBH_S32 fd = -1;

    snprintf(name, sizeof(name)-1, RJ45_SPEED_PATH);
    fd = open(name, O_RDONLY);
    if (fd < 0)
    {
        XLOGE("Failed to open eth0/speed file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }

    ssize_t bytesRead = read(fd, &read_str, sizeof(read_str));
    close(fd);

    if (bytesRead < 0)
    {
        XLOGE("Failed to read the file: %s\n", strerror(errno));
        return XBH_FAILURE;
    }
    read_str[bytesRead] = '\0';
    speed = atoi(read_str);
    if (speed > RJ45_DEFAULT_VALUE)
    {
        *bEnable = XBH_TRUE;
    }
    else
    {
        *bEnable = XBH_FALSE;
    }
    return s32Ret;
}

XBH_S32 XbhMtk_9679::setBrightMax(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int fp = 0;
    char tmp[32];

    fp = open(BACKLIGHT_SYSFS, O_WRONLY);
    if(fp == -1)
    {
        XLOGE("open backlight sysfs error!\n");
        return XBH_FAILURE;
    }
    sprintf(tmp, "max_duty=%d", u32Value);
    s32Ret = write(fp, tmp, strlen(tmp));
    if (s32Ret < 0) {
        ALOGE("Error backlight sysfs error %s \n", tmp);
        close(fp);
        return s32Ret;
    }
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%d", u32Value);
    property_set("persist.vendor.xbh.BacklightDutyMax", tmp);
    close(fp);
    s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getBrightMax(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR backlightMax[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.BacklightDutyMax", backlightMax, "100");
    *u32Value = atoi(backlightMax);
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::setBrightMin(XBH_U32 u32Value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int fp = 0;
    char tmp[32];

    fp = open(BACKLIGHT_SYSFS, O_WRONLY);
    if(fp == -1)
    {
        XLOGE("open backlight sysfs error!\n");
        return XBH_FAILURE;
    }
    sprintf(tmp, "min_duty=%d", u32Value);
    s32Ret = write(fp, tmp, strlen(tmp));
    if (s32Ret < 0) {
        ALOGE("Error backlight sysfs error %s \n", tmp);
        close(fp);
        return s32Ret;
    }
    memset(tmp, 0, sizeof(tmp));
    sprintf(tmp, "%d", u32Value);
    property_set("persist.vendor.xbh.BacklightDutyMin", tmp);
    close(fp);
    s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 XbhMtk_9679::getBrightMin(XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR backlightMin[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.BacklightDutyMin", backlightMin, "20");
    *u32Value = atoi(backlightMin);
    return  s32Ret;
}

XBH_S32 XbhMtk_9679::getHdmiPortClockStatus(XBH_U32 port, XBH_S32 *status)
{
    XBH_S32 ret = XBH_SUCCESS;
    int fp = 0;
    char buf[4];

    if(port == 0)
    {
        fp = open(HDMI_A_CLK_PATH, O_RDONLY);
    }
    else if(port == 1)
    {
        fp = open(HDMI_B_CLK_PATH, O_RDONLY);
    }
    else if(port == 2)
    {
        fp = open(HDMI_C_CLK_PATH, O_RDONLY);
    }
    else if(port == 3)
    {
        fp = open(HDMI_D_CLK_PATH, O_RDONLY);
    }

    if(fp == -1)
    {
        XLOGE("open src = %d error!\n", port);
        return XBH_FAILURE;
    }
    XBH_S32 s32Ret = read(fp, buf, 4);
    if(s32Ret == -1)
    {
        XLOGE("write HDMI_%d_CLK_PATH error!\n", port);
        close(fp);
        return XBH_FAILURE;
    }
    *status = ((XBH_BOOL)atoi(buf) ? XBH_SIGNAL_STATUS_SUPPORT : XBH_SIGNAL_STATUS_NOSIGNAL);
    close(fp);

    return ret;
}

XbhMtk_9679::XbhMtk_9679()
{
    XLOGD(" begin ");
    XbhPartitionData::getInstance()->setDataPath(MSDC_CUSDATA_PATH, MSDC_CUSPARAM_PATH);
    //init console
    initSerialPort();
    //恢复出厂设置后会被清掉，需要重新初始化一次
    loadProperty();
    //synchronize the default standby wake-up status to cusparam
    loadWoTStatus();
    //init wol status
    initWolStatus();
    //gpio fd
    mGpioFd = open(GPIO_DEV_PATH, O_RDWR);
    if (mGpioFd < 0) {
        XLOGE("-- open device : /dev/xbh_gpio fail --\n");
    }

    //i2c lock
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        mLockIIC[i] = -1;
    }
#if 0
    /* init inputsrc */
    MI_RESULT errCode;
    MI_INPUTSRC_InitParams_t stInputsrcInitParam;
    memset(&stInputsrcInitParam, 0, sizeof(MI_INPUTSRC_InitParams_t));
    errCode = MI_INPUTSRC_Init(&stInputsrcInitParam);
    if (errCode != MI_OK && errCode != MI_HAS_INITED)
    {
        XLOGE(" Init MI_INPUTSRC failed!  errCode = %d \n",errCode);
    }
#endif
    //default is XBH_AUDIO_OUTPUT_DET
    mAudioOutput = XBH_AUDIO_OUTPUT_DET;

    XBH_CHAR backlightDuty[PROPERTY_VALUE_MAX] = {0};
    if (property_get("persist.vendor.xbh.BacklightDutyMax", backlightDuty, "100") > 0)
    {
        if(atoi(backlightDuty) >= 0 && atoi(backlightDuty) <= 100)
        {
            setBrightMax(atoi(backlightDuty));
        }
    }
    
    if (property_get("persist.vendor.xbh.BacklightDutyMin", backlightDuty, "20") > 0)
    {
        if(atoi(backlightDuty) >=0 && atoi(backlightDuty) <= 100)
        {
            setBrightMin(atoi(backlightDuty));
        }
    }


    XLOGD(" end ");
}

