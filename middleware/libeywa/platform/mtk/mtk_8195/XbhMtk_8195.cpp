#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhMtk_8195"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/rtc.h>

#include <hardware/board.h>
#include <cutils/properties.h>
#include <utils/String8.h>
#include "XbhSysOpt.h"

#include "XbhLog.h"
#include "XbhMtk_8195.h"
#include "XbhService.h"
#include "XbhNfcManager.h"
#include "XbhPartitionData.h"
#include "XbhHdmirx.h"

#include <dlfcn.h>
#include "libnvram.h"
#include "nvram_ethernet.h"
#include "CFG_ETHERNET_File.h"
#include "CFG_file_lid.h"
#include "Custom_NvRam_LID.h"
#include "CFG_PRODUCT_INFO_File.h"
#include "keyblock.h" //libxbhdrmkey
#include "XbhMcuManager.h"
#include <filesystem>
#include <sys/stat.h>

using namespace android;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;

//------------------ static function begin -----------------------------
#define ADC_DEV_PATH "/sys/devices/platform/11002000.adc/iio:device2"

#define NTC_ADC_100K_0402_MIN -40
#define NTC_ADC_100K_0402_LEN 141

#define MSDC_CUSDATA_PATH  "/dev/block/by-name/cusdata" //cusdata
#define MSDC_CUSPARAM_PATH "/dev/block/by-name/cusparam" //cusparam
#define BOOT_LOGO_FILE          "/dev/block/by-name/logo_a"
#define MAX_LOGO_BUFFER_SIZE    1024

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
#define XBH_BOARD_NTC_ADC_THERMISTOR_CHAN                XBH_ADC_CHANNEL_NUM_2   /*channel range: 0-7*/
#endif

#ifndef XBH_BOARD_NTC_ADC_THERMISTOR_VAL
#define XBH_BOARD_NTC_ADC_THERMISTOR_VAL                 100 /*value of resistance: 100K*/
#endif

//gpio struct
typedef struct {
    XBH_U32 gpio;
    XBH_U32 state;
    XBH_U32 jitter;
} UserData;

#define GPIO_DEV_PATH           "/dev/xbh_gpio"
#define MAC_BUFF_LEN            6
#define ETHERNET_MAC_ADDR_PATH  "/mnt/vendor/nvdata/APCFG/APRDEB/ETHERNET"

//source table
static XBH_U8 source_available[] = XBH_SRC_AVAILABLE_TABLE
XbhMutex  XbhMtk_8195::m_McuLock;
XbhMutex  XbhMtk_8195::m_NvmLock;
// drm key status
static XBH_BOOL gNeedKeyUpdate = XBH_TRUE;
static XBH_U32 gKeyStatus[DRM_KEY_MAX];
static XBH_U8 mCheckRx5v = 0;


struct lg_fd_closer
{
    lg_fd_closer(int fd):m_fd(fd){};
    void operator()(void *) const
    {
        if(m_fd >= 0)
        {
            close(m_fd);
        }
    }
    int m_fd{-1};
};

static std::string find_clock_rtc()
{
    const std::string sysfs_rtc = "/sys/class/rtc";
    // auto rtcs = SysWrite::ls(sysfs_rtc);
    std::vector<std::string> rtcs;
    DIR *pDirOfPath = nullptr;
    pDirOfPath = opendir(sysfs_rtc.data());
    if(pDirOfPath)
    {
        struct dirent * ptr = nullptr;
        while((ptr = readdir(pDirOfPath)) != nullptr)
        {
            rtcs.push_back(ptr->d_name);
        }
        closedir(pDirOfPath);
    }

    char buf[MAX_STR_LEN + 1] = {0};
    int fd, len = -1;
    for(auto &rtc:rtcs)
    {
        std::string hctosysOfrtc = sysfs_rtc + "/" + rtc + "/hctosys";
        fd = open(hctosysOfrtc.data(), O_RDONLY);
        if(fd >= 0)
        {
            len = read(fd, buf, MAX_STR_LEN);
            close(fd);
            if(len > 0)
            {
                if(strtol(buf,nullptr,10) != 0)
                {
                    return rtc;
                }
            }
        }
    }
    return "";
}

static XBH_BOOL fileExistsAndNonEmpty(const std::string& filePath)
{
    struct stat fileStat;
    if (stat(filePath.c_str(), &fileStat) == 0) {
        return fileStat.st_size > 0;
    }
    return XBH_FALSE;
}

static void copyFile(const char *src_filename, const char *dst_filename) {
    FILE *src_file = fopen(src_filename, "r");
    if (src_file == NULL) {
        XLOGE("source file open fail");
        return;
    }

    FILE *dst_file = fopen(dst_filename, "w");
    if (dst_file == NULL) {
        XLOGE("dst file open fial");
        fclose(src_file);
        return;
    }

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dst_file);
    }

    fclose(src_file);
    fclose(dst_file);
    XLOGD("file %s copy %s success!\n", src_filename, dst_filename);
}

static void chmodFile(const XBH_CHAR* dst_path)
{
    mode_t newPermissions = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    if (chmod(dst_path, newPermissions) == 0) {
        XLOGD("dir %s success!\n", dst_path);
    } else {
        XLOGD("dir %s fail!\n", dst_path);
    }
    return;
}

static XBH_BOOL copyFileToPartition(const XBH_CHAR* src_path, XBH_BOOL val)
{
    #define BOOTLOGO_FILE           "persist.vendor.xbh.bootlogo_file"
    #define BOOTANIMATION_FILE      "persist.vendor.xbh.bootanimation_file"

    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR* dst_path = NULL;
    std::string src_str;

    if(fileExistsAndNonEmpty(src_str.append(src_path)) == XBH_FALSE)
    {
        XLOGE("copyFileToLenovo : %s file not exist!\n", src_path);
        goto no_copy_exit;
    }

    if(property_get(BOOTLOGO_FILE, propVal, "") > 0 && val == XBH_TRUE)
    {
        if(strcmp(propVal, "") == 0)
        {
            goto no_copy_exit;
        }
        else
        {
            dst_path = propVal;
            copyFile(src_path, dst_path);
            chmodFile(dst_path);
        }
    }
    else if (property_get(BOOTANIMATION_FILE, propVal, "") > 0 && val == XBH_FALSE)
    {
        if(strcmp(propVal, "") == 0)
        {
            goto no_copy_exit;
        }
        else
        {
            dst_path = propVal;
            copyFile(src_path, dst_path);
            chmodFile(dst_path);
        }
    }
    else
    {
        goto no_copy_exit;
    }

    return XBH_TRUE;
no_copy_exit:
    XLOGD("copyFileToLenovo : no need copy\n", src_path);
    return XBH_FALSE;
}
//------------------ static function end -----------------------------

//------------------ public function begin -----------------------------

XbhMtk_8195::XbhMtk_8195()
{
    XLOGD(" begin ");
    mAudioOutput = XBH_AUDIO_OUTPUT_DET;
    XbhPartitionData::getInstance()->setDataPath(MSDC_CUSDATA_PATH, MSDC_CUSPARAM_PATH);
    //init console
    initSerialPort();
    //enable keypower.
    setMcuEnableKeyPower();
    handleLicenseFileMissing();
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
    XLOGD(" end ");
}

//override
XBH_S32 XbhMtk_8195::setDebugEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR ttyPath[64] = {0};
    XBH_S32 ttyfd,ret;

    if(bEnable)
    {
        strcpy(ttyPath, "/dev/ttyS0");
        ttyfd = open(ttyPath, O_RDWR | O_NOCTTY | O_NONBLOCK);
        ret = tcsetattr(ttyfd, TCSANOW, &uart_debug_config);
        XLOGE("ttyfd = %d, ret = %d\n",ttyfd,ret);
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

//override
XBH_S32 XbhMtk_8195::getDebugEnable(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if (property_get("init.svc.console", propVal, "stopped") > 0) {
        // 判断uart是否运行
        *bEnable = (strncmp(propVal, "running", PROPERTY_VALUE_MAX) == 0) ? XBH_TRUE : XBH_FALSE;
    } else {
        *bEnable = XBH_FALSE;
    }

    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setBootLogo(const XBH_CHAR* path)
{
    XBH_U32 s32Ret = XBH_FAILURE;
    XBH_U32 write_len = 0;
    XBH_U32 read_len = 0;
    XBH_U32 wirte_offset = 0;

    XBH_S32 fd = open(BOOT_LOGO_FILE, O_SYNC | O_WRONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open lendata partition fail. fd: %d, %s\n", fd, strerror(errno));
        return s32Ret;
    }

    copyFileToPartition(path, true);//判断Customer 客制化.cus文件是否配置BOOT_LOGO_COPY_FILE_PATH 来确认是否拷贝

    XBH_S32 fd_lenovo_logo = open(path, O_RDONLY | O_NONBLOCK);
    if (fd_lenovo_logo < 0)
    {
        XLOGE("Open fd_lenovo_logo partition fail. fd: %d, %s\n", fd_lenovo_logo, strerror(errno));
        goto end_lenovo_fd;
    }
    char buffer[MAX_LOGO_BUFFER_SIZE];
    memset(buffer, 0x00, MAX_LOGO_BUFFER_SIZE);
    while ((read_len = read(fd_lenovo_logo, buffer, MAX_LOGO_BUFFER_SIZE)) > 0){
        write_len = pwrite(fd, buffer, read_len, wirte_offset);
        if (read_len == -1)
        {
            XLOGE("Write lendata partition fail\n");
            goto end_logo;
        }
        else if (write_len != read_len)
        {
            XLOGE("Write lendata data size issue");
            goto end_logo;
        }
        wirte_offset += write_len;
        memset(buffer, 0x00, MAX_LOGO_BUFFER_SIZE);
    }
    s32Ret = XBH_SUCCESS;
end_logo:
    close(fd_lenovo_logo);
end_lenovo_fd:
    close(fd);
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setBootAnimation(const XBH_CHAR* path)
{
    XBH_CHAR dst_path[PROPERTY_VALUE_MAX] = {0};

    copyFileToPartition(path, false);
    return XBH_SUCCESS;
}

//override
XBH_S32 XbhMtk_8195::initSerialPort()
{
    XBH_S32 ttyfd;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR ttyPath[64] = {0};
    strcpy(ttyPath, "/dev/ttyS0");
    ttyfd = open(ttyPath,O_RDWR | O_NOCTTY | O_NONBLOCK);
    XLOGE("ttyfd = %d\n",ttyfd);
    if(tcgetattr(ttyfd, &uart_debug_config))
    {
        XLOGE("tcgetattr error\n");
    }
    close(ttyfd);

    if(property_get("persist.vendor.xbh.rs232.decouple", propVal, "false") > 0)
    {
        if(strcmp(propVal, "true") == 0)
        {
            if(property_get("persist.vendor.xbh.debug.enable", propVal, "false") > 0)
            {
                if(strcmp(propVal, "true") == 0)
                {
                    setDebugEnable(XBH_TRUE);
                }
                else
                {
                    setDebugEnable(XBH_FALSE);
                }
                return XBH_SUCCESS;
            }
        }
        else
        {
            goto do_debug_op;
        }
    }
    else
    {
do_debug_op:
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
    }
    return XBH_FAILURE;
}

//override
XBH_S32 XbhMtk_8195::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //chip gpio
    UserData userdata;
    //mcu gpio
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;

#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        //XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = setMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, u32GpioValue);
        if (s32Ret != 0)
        {
            XLOGE("Error! setMcuGpioValue numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        }
        return s32Ret;
    }
#endif

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;
    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        XLOGV("-- gpio %d is not output pin set output now", u32GpioNumber);
        s32Ret = setGpioDirOutput(u32GpioNumber, u32GpioValue);
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

//override
XBH_S32 XbhMtk_8195::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //chip gpio
    UserData userdata;
    //mcu gpio
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;


#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        //XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_OUTPUT, u32GpioValue);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("Error! getMcuGpioValue numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        }
        return s32Ret;
    }
#endif

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = XBH_BOARD_GPIO_OUTPUT;

    if(getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        XLOGV("-- gpio %d is not output pin set output now",u32GpioNumber);
        userdata.state = 0;
        s32Ret = ioctl(mGpioFd, GET_IO_VAL, &userdata);
        if(s32Ret < 0)
        {
            XLOGE("-- get gpio-%d value fail --",userdata.gpio);
            return XBH_FAILURE;
        }
        setGpioDirOutput(userdata.gpio, userdata.state);
        *u32GpioValue = userdata.state;
        return s32Ret;
    }

    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_OUT_VAL, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- get gpio-%d out value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *u32GpioValue = userdata.state;
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    //chip gpio
    UserData userdata;
    //mcu gpio
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;
#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        //XLOGD("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_INPUT, u32GpioValue);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("Error! getMcuGpioValue numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        }
        return s32Ret;
    }
#endif

    if(mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = XBH_BOARD_GPIO_INPUT;

    if(getGpioDir(u32GpioNumber) == XBH_BOARD_GPIO_OUTPUT)
    {
        XLOGV("-- gpio %d is not input pin set output now",u32GpioNumber);
        setGpioDirInput(userdata.gpio);
    }

    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_VAL, &userdata);
    if(s32Ret < 0)
    {
        XLOGE("-- get gpio-%d value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *u32GpioValue = userdata.state;
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setIICLock(XBH_U32 iicBus, XBH_BOOL lock)
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

//override
XBH_S32 XbhMtk_8195::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 retryConut = 0;
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


//override
XBH_S32 XbhMtk_8195::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
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

//override
XBH_S32 XbhMtk_8195::readI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        if (mLockIIC[i] == u32I2cNum)
        {
            return XBH_LOCKSTATUS;
        }
    }
    s32Ret = readChipI2cData(u32I2cNum, u8DevAddress, u32Length, u8Data);
    return  s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable)
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

//override
XBH_S32 XbhMtk_8195::getSocHdmiEdidStatus(XBH_BOOL *enable)
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

XBH_S32 XbhMtk_8195::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    F_ID eth_nvram_fd = {0};
    File_Ethernet_Struct write_ethernet_nvram;
    unsigned int *iFileEthMacAddrLID;
    int i = 0;
    int rec_size;
    int rec_num;
    int ret;
    void *hcustlib;
    unsigned char readyWriteMacAddress[MAC_BUFF_LEN] = {0};

    int retVal = sscanf(strMacAddress, "%02X:%02X:%02X:%02X:%02X:%02X", &readyWriteMacAddress[0], &readyWriteMacAddress[1],
            &readyWriteMacAddress[2], &readyWriteMacAddress[3],
            &readyWriteMacAddress[4], &readyWriteMacAddress[5]);

    XLOGD("%s dlopen custom NVRAM\n", __FUNCTION__);
    hcustlib = dlopen("libcustom_nvram.so", RTLD_LAZY);
    if (!hcustlib) {
        XLOGE("%s(), dlopen fail! (%s)\n", __FUNCTION__, dlerror());
        return XBH_FAILURE;
    }

    dlerror();  /* Clear any existing error */
    XLOGD("%s hcustlib: %p\n", __FUNCTION__, hcustlib);

    /* Fetch symbols and sanity check */
    iFileEthMacAddrLID = (unsigned int*)(dlsym(hcustlib, "iFileEthMacAddrLID")); /* AP_CFG_RDEB_FILE_ETHERNET_LID */
    if (!iFileEthMacAddrLID) {
        XLOGE("not support iFileEthMacAddrLID\n");
        return XBH_FAILURE;
    }

    eth_nvram_fd = NVM_GetFileDesc(*iFileEthMacAddrLID, &rec_size, &rec_num, ISWRITE);
    XLOGD("lid %d rec_size %d rec_num %d\n", iFileEthMacAddrLID, rec_size, rec_num);

    if (rec_num != 1)
    {
        XLOGE("Unexpected record num %d\n", rec_num);
        NVM_CloseFileDesc(eth_nvram_fd);
        return XBH_FAILURE;
    }

    if (rec_size != sizeof(File_Ethernet_Struct))
    {
        XLOGE("Unexpected record size %d ,File_Ethernet_Struct %d\n",
        rec_size, sizeof(File_Ethernet_Struct));
        NVM_CloseFileDesc(eth_nvram_fd);
        return XBH_FAILURE;
    }

    if(MAC_BUFF_LEN != retVal)
    {
        XLOGE("=============MAC_BUFF_LEN================\n");
        return XBH_FAILURE;
    }

    for(int index = 0; index < MAC_BUFF_LEN; index++) {
        write_ethernet_nvram.macaddr[index] = readyWriteMacAddress[index];
    }

    XLOGD("\n----------- ready to write mac addr ------------------\n");
    XLOGD("%02X:%02X:%02X:%02X:%02X:%02X", \
        write_ethernet_nvram.macaddr[0], write_ethernet_nvram.macaddr[1], \
        write_ethernet_nvram.macaddr[2], write_ethernet_nvram.macaddr[3], \
        write_ethernet_nvram.macaddr[4], write_ethernet_nvram.macaddr[5]);
    XLOGD("\n------------------------------------------------------\n");

    ret = write(eth_nvram_fd.iFileDesc, &write_ethernet_nvram, rec_num * rec_size);

    if((-1 == ret) || (rec_num*rec_size != ret))
    {
        XLOGE("error:write wifi addr fail!\n");
    }

    NVM_CloseFileDesc(eth_nvram_fd);
    XLOGD("write ethernet addr success!\n");
    sync();
    s32Ret = XbhPartitionData::getInstance()->setMacAddress(strMacAddress);
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_8195::getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress)
{
    unsigned char addr_buff[MAC_BUFF_LEN] = {0x00};
    int read_ret;
    char *file_name;
    char file_name_arry[1024] = {'\0'};
    int ret_val = -1;
    unsigned char conv_buf[3];
    int write_offset = 0;
    char write_buff[(10 * MAC_BUFF_LEN)]= {'\0'};

    memcpy(file_name_arry, ETHERNET_MAC_ADDR_PATH, strlen(ETHERNET_MAC_ADDR_PATH));
    file_name = file_name_arry;

    FILE *fp_in;
    fp_in = fopen(file_name, "rb");

    if (fp_in == NULL)
    {
        XLOGE("open %s failed\n", ETHERNET_MAC_ADDR_PATH);
        return XBH_FAILURE;
    }

    read_ret = fread(addr_buff ,(MAC_BUFF_LEN * sizeof(unsigned char)), 1, fp_in);
    if (read_ret < 1)
    {
        XLOGE("read macaddr error, read_ret: %d\n", read_ret);
        fclose(fp_in);
        return XBH_FAILURE;
    }

    for(int loop_index = 0; loop_index < MAC_BUFF_LEN; loop_index++)
    {
        memset(conv_buf, 0, sizeof(conv_buf)/sizeof(unsigned char));

        unsigned char input_hex = addr_buff[loop_index];
        unsigned char high_bit = 0x00;
        unsigned char low_bit = 0x00;

        low_bit = input_hex & (0x0f);
        high_bit = (input_hex >> 4) & (0x0f);

        sprintf((char *)conv_buf, "%x%x", high_bit, low_bit);

        snprintf(write_buff + write_offset, strlen((char *)conv_buf) + 1, "%s", conv_buf);
        snprintf(write_buff + 2 + write_offset, 2, "%s", ":");
        // ignore '\0' position
        write_offset = 2 + write_offset + 1;
    }

    snprintf(write_buff + write_offset - 1, 2, "%s", "\0");

    memcpy(strMacAddress, write_buff, write_offset - 1);

    fclose(fp_in);

    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_8195::getADCChannelValue(XBH_U32 u32Channel, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int fp = 0;
    char buf[10];
    char path[128];
    sprintf(path, "%s/in_voltage%d_input", ADC_DEV_PATH, u32Channel);
    XLOGI("BoardTemp[%s:%d] Get Adc path =%s\r\n", __func__, __LINE__, path);

    fp = open(path, O_RDONLY);
    if(fp == -1)
    {
        XLOGE("BoardTemp open ADC_DEV_PATH error! %d\n", errno);
        return -1;
    }

    s32Ret = read(fp, buf, 10);
    if(s32Ret == -1)
    {
        XLOGE("BoardTemp open ADC_DEV_PATH error! %d\n", errno);
        close(fp);
        return -1;
    }

    *u32Value = atof(buf);
    close(fp);

    //XLOGD("BoardTemp [%s:%d] Get Adc voltage value %d=%u\r\n", __func__, __LINE__, u32Channel, *u32Value);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getTemperatureSensorValue(XBH_FLOAT *fValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_ADC_CHANNEL_NUM_E adcChannel = XBH_BOARD_NTC_ADC_THERMISTOR_CHAN;
    XBH_U32 adcValue   = 0;
    s32Ret = getADCChannelValue(adcChannel, &adcValue);

    XBH_U32 value = adcValue;
    if ((1800 - value) > 0)
    {
        XBH_U32 tAdcVal = XBH_BOARD_NTC_ADC_THERMISTOR_VAL * 1000 * value / (1800 - value);
        XBH_S32 tempIndex = 0;
        for(tempIndex=0; tempIndex < NTC_ADC_100K_0402_LEN; tempIndex++)
        {
            if(tAdcVal > NTC_ADC_100K_0402[tempIndex])
            {
                *fValue = NTC_ADC_100K_0402_MIN + tempIndex;
                // XLOGD("getBoardTemperature : ADC-CH%u = %u  tempIndex = %d convert to degree centigrade : %d",adcChannel,adcValue,tempIndex,*s32Value);
                s32Ret = XBH_SUCCESS;
                break;
            }
        }
        if(tempIndex >= NTC_ADC_100K_0402_LEN)
        {
            *fValue = 233;
            XLOGE("getBoardTemp: Invalid value = %d", value);
        }
    }
    else
    {
        XLOGE("getBoardTemp: get adc failed!!! value = %d", value);
        *fValue = 233;
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getTemperatureSensorExist(XBH_BOOL* bEnable)
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

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_8195::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
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
XBH_S32 XbhMtk_8195::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getColorTempPara(enColorTemp, data);
    return  s32Ret;
}


/**
 * 设置android对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_8195::setAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setAndroidColorTempPara(enColorTemp, data);
    return  s32Ret;
}

/**
 * 获取android对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_8195::getAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getAndroidColorTempPara(enColorTemp, data);
    return  s32Ret;
}

XBH_S32 XbhMtk_8195::setRtcTime(XBH_RTC_INFO_S* stRtcInfo)
{
    auto rtcName = find_clock_rtc();
    if (rtcName.empty())
    {
        return XBH_FAILURE;
    }
    std::string rtc_dev = std::string("/dev/").append(rtcName);
    int fd = open(rtc_dev.data(), O_RDWR);
    std::shared_ptr<void> devPtr(nullptr,lg_fd_closer(fd));
    if (fd < 0)
    {
        XLOGE("setRtcTime Unable to open %s: %s\n", rtc_dev.data(), strerror(errno));
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

XBH_S32 XbhMtk_8195::getRtcTime(XBH_RTC_INFO_S* stRtcInfo)
{
    if(stRtcInfo == nullptr)
    {
        return XBH_FAILURE;
    }
    auto rtcName = find_clock_rtc();
    if (rtcName.empty())
    {
        return XBH_FAILURE;
    }
    std::string rtc_dev = std::string("/dev/").append(rtcName);
    int fd = open(rtc_dev.data(), O_RDONLY);
    std::shared_ptr<void> devPtr(nullptr,lg_fd_closer(fd));
    if (fd < 0)
    {
        XLOGE("setRtcTime Unable to open %s: %s\n", rtc_dev.data(), strerror(errno));
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

XBH_S32 XbhMtk_8195::setRtcAlarmTime(XBH_RTC_ALARM_INFO_S* stRtcAlarmInfo)
{
    if(stRtcAlarmInfo == nullptr)
    {
        return XBH_FAILURE;
    }
    XBH_S32 s32Ret = XBH_SUCCESS;
    struct rtc_wkalrm alarm;
    struct tm tmpTm, tm, *gmtime_res;
    time_t sec;

    auto rtcName = find_clock_rtc();
    if (rtcName.empty())
    {
        return XBH_FAILURE;
    }
    std::string rtc_dev = std::string("/dev/").append(rtcName);
    int fd = open(rtc_dev.data(), O_RDONLY);
    std::shared_ptr<void> devPtr(nullptr,lg_fd_closer(fd));
    if (fd < 0)
    {
        XLOGE("setRtcTime Unable to open %s: %s\n", rtc_dev.data(), strerror(errno));
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

    // tmpTm.tm_sec = stRtcAlarmInfo->u32RtcSec;
    // tmpTm.tm_mon = rtc.tm_mon;
    // tmpTm.tm_year = rtc.tm_year;
    // tmpTm.tm_mday = stRtcAlarmInfo->u32RtcDate;
    // tmpTm.tm_wday = stRtcAlarmInfo->u32RtcWeek;
    // tmpTm.tm_hour = stRtcAlarmInfo->u32RtcHour;
    // tmpTm.tm_min = stRtcAlarmInfo->u32RtcMinute;
    // tmpTm.tm_isdst = -1;
    // tmpTm.tm_yday = 0;

    // sec = mktime(&tmpTm);
    // XLOGD("set-rtcAlarm, sec=%ld\n", (long)sec);
    // gmtime_res = gmtime_r(&sec, &tm);
    // if (!gmtime_res) {
    //     XLOGD("gmtime_r() failed: %s\n", strerror(errno));
    //     return XBH_FAILURE;
    // }

    memset(&alarm, 0, sizeof(alarm));
    alarm.time.tm_year = rtc.tm_year;
    alarm.time.tm_mon = rtc.tm_mon;
    alarm.time.tm_mday = stRtcAlarmInfo->u32RtcDate;
    alarm.time.tm_wday = stRtcAlarmInfo->u32RtcWeek;
    alarm.time.tm_hour = stRtcAlarmInfo->u32RtcHour;
    alarm.time.tm_min = stRtcAlarmInfo->u32RtcMinute;
    alarm.time.tm_sec = stRtcAlarmInfo->u32RtcSec;
    alarm.time.tm_yday = 0;
    alarm.time.tm_isdst = -1;
    alarm.enabled = stRtcAlarmInfo->bEnable != 0;

    XLOGD("setRtcAlarmTime : rtc.tm_sec: %d | rtc.tm_min: %d | rtc.tm_hour: %d | rtc.tm_mday: %d | rtc.tm_wday: %d | rtc.tm_mon: %d | rtc.tm_year: %d\n",
                alarm.time.tm_sec, alarm.time.tm_min, alarm.time.tm_hour,
                alarm.time.tm_mday, alarm.time.tm_wday, alarm.time.tm_mon, alarm.time.tm_year);
    XLOGD("rtc.enable: %d\n", alarm.enabled);

    s32Ret = ioctl(fd, RTC_WKALM_SET, &alarm);
    if(s32Ret < 0)
    {
        XLOGE("RTC_WKALM_SET ioctl failed: %s\n", strerror(errno));
        return XBH_FAILURE;
    }

    if(!alarm.enabled)
    {
        XLOGD("RTC_AIE_OFF RTC_AIE_OFF \n");
        s32Ret = ioctl(fd, RTC_AIE_OFF, 0);
        if(s32Ret < 0)
        {
            XLOGE("RTC_AIE_OFF ioctl failed: %s\n", strerror(errno));
            return XBH_FAILURE;
        }
    }
    else
    {
        XLOGD("RTC_AIE_ON RTC_AIE_ON \n");
        s32Ret = ioctl(fd, RTC_AIE_ON, 0);
        if(s32Ret < 0)
        {
            XLOGE("RTC_AIE_ON ioctl failed: %s\n", strerror(errno));
            return XBH_FAILURE;
        }
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_8195::setHdmirxEdidType(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN] = {0};

    if (type == 1)
    {
        s8Buff[0] = 1;              // EDID 1.4
    }
    else if (type == 2)
    {
        s8Buff[0] = 2;              // EDID 2.0
    }
    else if (type == 3)
    {
        s8Buff[0] = 3;              // EDID 2.1,当前不支持
    }
    else if (type == 4)
    {
        s8Buff[0] = 4;              // 自动模式，当前不支持
    }

    s32Ret = XbhPartitionData::getInstance()->setHdmiEdidParamValue((XBH_VOID *)s8Buff);
    XLOGD("set setHdmirxEdidType %d, %d\n",s8Buff[0], s32Ret);

    return s32Ret;
}

XBH_S32 XbhMtk_8195::getHdmirxEdidType(XBH_S32 *type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getHdmiEdidParamValue((XBH_VOID *)s8Buff);

    if (s32Ret != XBH_SUCCESS || (s8Buff[0] == 0))
    {
        // 当getHdmiEdidParamValue分区值为0时,代表为空,则默认使用EDID 2.0
        *type = 2;
        setHdmirxEdidType(*type);
        XLOGD("set defult HdmirxEdidType: %d %d\n", *type, s32Ret);
        return XBH_SUCCESS;
    }
    if (s8Buff[0] == 1)
    {
        *type = 1;              // EDID 1.4
    }
    else if (s8Buff[0] == 2)
    {
        *type = 2;              // EDID 2.0
    }
    else if (s8Buff[0] == 3)
    {
        *type = 3;              // EDID 2.1
    }
    else if (s8Buff[0] == 4)
    {
        *type = 4;              // 自动模式
    }

    XLOGD("get HdmirxEdidType %d \n",s8Buff[0]);

    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_8195::isNfcExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getChipExist(enable);
    XLOGW("isNfcExist s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    property_set("persist.vendor.xbh.nfc.enable",enable == XBH_TRUE ? "true" : "false");
    s32Ret = XbhNfcManager::getInstance()->setNfcEnable(enable);
    XLOGW("setNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getNfcEnable(XBH_BOOL *enable)
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

XBH_S32 XbhMtk_8195::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getNfcId(data, length);
    XLOGW("getNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->addNfcId(data, length);
    XLOGW("addNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delNfcId(data, length);
    XLOGW("delNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delAllNfcId();
    XLOGW("delAllNfcId s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuFattMode(mode);
    XLOGW("setMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuFattMode(mode);
    XLOGW("getMcuFattMode s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhMcuManager::getInstance()->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->setSn(strSn);
    if (s32Ret == XBH_SUCCESS) {
        property_set("vendor.xbh.rkp.serialno", strSn);
    }
    s32Ret = setNvramValue(BARCODE_OFFSET, BARCODE_LEN, (XBH_VOID*)strSn);
    return  s32Ret;
}

XBH_S32 XbhMtk_8195::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSn(strSn);
    s32Ret = getNvramValue(BARCODE_OFFSET, BARCODE_LEN, (XBH_VOID*)strSn);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getMicLicenseState(XBH_S32* status)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    const char* lic_response = "/data/vendor/lic_response";
    const char* lic_status = "/data/vendor/lic_status";
    std::string result;

    property_set("persist.vendor.xbh.usb.uac_android_enable", "true");
    usleep(200 * 1000);
    property_set("persist.vendor.xbh.usb.uac_android_enable", "false");
    
    // Step 1: 判断 lic_response 文件是否存在
    if (access(lic_response, F_OK) == 0) 
    {
        // 文件存在，读取 lic_status 状态
        if (!XbhSysOpt::getInstance()->readSysfs(lic_status, result)) 
        {
            XLOGE("%s: %d Failed to read sysfs for lic_status\n", __FUNCTION__, __LINE__);
            *status = s32Ret;
            return s32Ret;
        }

        XLOGD("%s: %d MicLicenseState: %s\n", __FUNCTION__, __LINE__, result.c_str());

        if (strcmp("pass", result.c_str()) == 0) 
        {
            // 状态 pass，开始读取文件并写入系统键值对
            int fd = open(lic_response, O_RDONLY);
            if (fd == -1) 
            {
                XLOGE("%s: %d Failed to open lic_response file\n", __FUNCTION__, __LINE__);
                *status = s32Ret;
                return s32Ret;
            }

            off_t size = lseek(fd, 0, SEEK_END);
            if (size <= 0) 
            {
                close(fd);
                XLOGE("%s: %d lic_response file is empty or invalid\n", __FUNCTION__, __LINE__);
                *status = s32Ret;
                return s32Ret;
            }

            lseek(fd, 0, SEEK_SET);
            char* buffer = (char*)malloc(size + 1);
            if (!buffer) 
            {
                close(fd);
                XLOGE("%s: %d Memory allocation failed\n", __FUNCTION__, __LINE__);
                *status = s32Ret;
                return s32Ret;
            }

            ssize_t bytes_read = read(fd, buffer, size);
            close(fd);

            if (bytes_read <= 0 || bytes_read != size) 
            {
                free(buffer);
                XLOGE("%s: %d Failed to read lic_response file content\n", __FUNCTION__, __LINE__);
                *status = s32Ret;
                return s32Ret;
            }

            buffer[bytes_read] = '\0';
            s32Ret = XbhPartitionData::getInstance()->setMicLicResponse(buffer);
            //s32Ret = mSystemControlClient->writeUnifyKey("lic_response", buffer) ? XBH_SUCCESS : XBH_FAILURE;
            free(buffer);
            if (s32Ret != XBH_SUCCESS) 
            {
                XLOGE("%s: %d writeUnifyKey failed, s32Ret=%d\n", __FUNCTION__, __LINE__, s32Ret);
            }
            XLOGD("%s: %d writeUnifyKey success, size=%d\n", __FUNCTION__, __LINE__, size);
            *status = s32Ret;
            return s32Ret;
        }
        else 
        {
            // lic_status 不是 pass
            XLOGE("%s: %d MicLicenseState not pass: %s\n", __FUNCTION__, __LINE__, result.c_str());
            *status = s32Ret;
            return s32Ret;
        }
    }
    else 
    {
        XLOGE("%s: %d Wrote response is no exit !!\n", __FUNCTION__, __LINE__);
        *status = s32Ret;
        return s32Ret;
    }
}

XBH_S32 XbhMtk_8195::handleLicenseFileMissing()
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_CHAR* lic_response = "/data/vendor/lic_response";
    std::string strResponse;
    XBH_CHAR buffer[1024] = {0};
    if (access(lic_response, F_OK) != 0)
    {
        // 从统一存储区读取 lic_response 数据
        s32Ret = XbhPartitionData::getInstance()->getMicLicResponse(buffer);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGE("%s: %d Failed to getMicLicResponse from cusdata!\n", __FUNCTION__, __LINE__);
        }
        XLOGD("%s: %d getMicLicResponse success, buffer=%s\n", __FUNCTION__, __LINE__, buffer);
        strResponse = buffer;
        
        // 创建 lic_response 文件并写入数据
        int lic_response_fd = open(lic_response, O_CREAT | O_WRONLY | O_TRUNC, 0777);
        if (lic_response_fd < 0) {
            XLOGE("%s: %d Failed to create lic_response file\n", __FUNCTION__, __LINE__);
            return XBH_FAILURE;
        }

        ssize_t writelen = write(lic_response_fd, strResponse.c_str(), strResponse.size());
        close(lic_response_fd);

        if (writelen != (ssize_t)strResponse.size()) {
            XLOGE("%s: %d Failed to write data to lic_response\n", __FUNCTION__, __LINE__);
            return XBH_FAILURE;
        }

        XLOGD("%s: %d Wrote response data to lic_response: %s\n", __FUNCTION__, __LINE__, strResponse.c_str());
    }
    return XBH_SUCCESS;
}


//override
XBH_S32 XbhMtk_8195::setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4 || type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        int len = 0;
        unsigned char buff[2048] = {0};
        //open key file
        FILE *pfd = fopen(pBuff, "rb");
        if (pfd == NULL)
        {
            XLOGE("fopen file error: %s.", pBuff);
            return -1;
        }
        //get file size
        s32Ret = fseek(pfd, 0, SEEK_END);
        len = ftell(pfd);
        XLOGD("file length = %d\n", len);
        fseek(pfd, 0, SEEK_SET);
        //check file size
        if (len > 2048)
        {
            fclose(pfd);
            XLOGE("file is large than 2048: %d.", len);
            return XBH_FAILURE;
        }
        else if (len == 1216)
        {
            XLOGI("write HDCP_HDMI_1X_V4_RX_ID key");
        }
        else if (len == 1856)
        {
            XLOGI("write HDCP_2X_V2_RX_ID key");
        }
        //read file
        s32Ret = fread(buff, 1, len, pfd);
        if (s32Ret <= 0)
        {
            XLOGE("fread file error.");
            fclose(pfd);
            return -1;
        }
        XLOGD("fread length = %d\n", s32Ret);

        s32Ret  = writeDrmKeyByData(buff, len);
        if (s32Ret)
        {
            XLOGE("Error! writeDrmKeyByPath fail, ret = %d, path = %s", s32Ret, pBuff);
            fclose(pfd);
            return XBH_FAILURE;
        }

        if(type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
        {
            setHdcpKeyName("XBH_HDCP1_4", type);
        }
        else if( type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
        {
            setHdcpKeyName("XBH_HDCP2_2", type);
        }
        fclose(pfd);
        gNeedKeyUpdate = XBH_TRUE;
    }
    else if (type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_1_4 || type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_2_2)
    {
        s32Ret = writeDrmKeyByPath(pBuff);
        if (s32Ret)
        {
            XLOGE("HDCPTX key write error, filepatch: %s!!!", pBuff);
            return XBH_FAILURE;
        }

        if(type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_1_4)
        {
            setHdcpKeyName("XBH_HDCP_TX_1_4", type);
        }
        else if( type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_2_2)
        {
            setHdcpKeyName("XBH_HDCP_TX_2_2", type);
        }
        gNeedKeyUpdate = XBH_TRUE;
    }
    else
    {
        XLOGE("Error! Unknown hdcp type: %d", type);
        return XBH_FAILURE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff)
{
    (void)type;
    (void)pBuff;
    XLOGW("MT8195 do not support current inferface.");
    return XBH_FAILURE;
}

//override
XBH_S32 XbhMtk_8195::getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (gNeedKeyUpdate)
    {
        s32Ret = updateDrmKeyStatus(gKeyStatus);
        if (s32Ret)
        {
            *bEnable = XBH_FALSE;
            XLOGE("Error! updateDrmKeyStatus fail, ret = %d", s32Ret);
            return XBH_FAILURE;
        }
        gNeedKeyUpdate = XBH_FALSE;
    }
    if (type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        *bEnable = (gKeyStatus[HDCP_HDMI_1X_V4_RX_ID] == 1 ? XBH_TRUE : XBH_FALSE);
        XLOGD("gKeyStatus[HDCP_HDMI_1X_V4_RX_ID] = %d", *bEnable);
    }
    else if (type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        *bEnable = (gKeyStatus[HDCP_2X_V2_RX_ID] == 1 ? XBH_TRUE : XBH_FALSE);
        XLOGD("gKeyStatus[HDCP_2X_V2_RX_ID] = %d", *bEnable);
    }
    else if (type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_1_4)
    {
        *bEnable = (gKeyStatus[HDCP_1X_TX_ID] == 1 ? XBH_TRUE : XBH_FALSE);
        XLOGD("gKeyStatus[HDCP_1X_TX_ID] = %d", *bEnable);
    }
    else if (type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_2_2)
    {
        *bEnable = (gKeyStatus[HDCP_2X_V1_TX_ID] == 1 ? XBH_TRUE : XBH_FALSE);
        XLOGD("gKeyStatus[HDCP_2X_V1_TX_ID] = %d", *bEnable);
    }
    else
    {
        *bEnable = XBH_FALSE;
        XLOGE("Error! Unknown hdcp type: %d", type);
        return XBH_FAILURE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret = setNvramValue(HDCPRX_14_NAME_OFFSET, HDCPRX_14_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! setNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        s32Ret = setNvramValue(HDCPRX_22_NAME_OFFSET, HDCPRX_22_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! setNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_1_4)
    {
        s32Ret = setNvramValue(HDCPTX_14_NAME_OFFSET, HDCPTX_14_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! setNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_2_2)
    {
        s32Ret = setNvramValue(HDCPTX_22_NAME_OFFSET, HDCPTX_22_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! setNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else
    {
        XLOGE("Error! Unknown hdcp type: %d\n", type);
        return XBH_FAILURE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (type == XBH_HDCP_TYPE_E::XBH_HDCP1_4)
    {
        s32Ret = getNvramValue(HDCPRX_14_NAME_OFFSET, HDCPRX_14_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! getNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP2_2)
    {
        s32Ret = getNvramValue(HDCPRX_22_NAME_OFFSET, HDCPRX_22_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! getNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_1_4)
    {
        s32Ret = getNvramValue(HDCPTX_14_NAME_OFFSET, HDCPTX_14_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! getNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else if(type == XBH_HDCP_TYPE_E::XBH_HDCP_TX_2_2)
    {
        s32Ret = getNvramValue(HDCPTX_22_NAME_OFFSET, HDCPTX_22_NAME_LEN, (XBH_VOID*)strHdcpFileName);
        if (s32Ret)
        {
            XLOGE("Error! getNvramValue fail, ret = %d, name = %s, type = %d\n", s32Ret, strHdcpFileName, type);
        }
    }
    else
    {
        XLOGE("Error! Unknown hdcp type: %d\n", type);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhMtk_8195::setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL status = XBH_FALSE;
    XLOGD("setAudioOutput : %d\n", enAudioOutput);

    if (mAudioOutput != enAudioOutput)
    {
        switch (enAudioOutput)
        {
            default:
            case XBH_AUDIO_OUTPUT_DET:
            {
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

                setGpioOutputValue(XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW, XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW_SPIDF);
                break;
            }
            case XBH_AUDIO_OUTPUT_USB:
            {
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
                break;
            }
            case XBH_AUDIO_OUTPUT_ARC:
            {
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                setGpioOutputValue(XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW, XBH_BOARD_GPIO_DISPLAY_ARCSPIDF_SW_ARC);
                break;
            }
        }

    }

    mAudioOutput = enAudioOutput;
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *enAudioOutput = mAudioOutput;
    return s32Ret;
}
//override
XBH_S32 XbhMtk_8195::getEthPortSpeed(XBH_S32 port, XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

XBH_S32 XbhMtk_8195::requestGpioIrq(XBH_U32 u32GpioNumber, XBH_U32 u32Type, XBH_BOOL enable)
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

XBH_S32 XbhMtk_8195::freeAllGpioIrq()
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

XBH_S32 XbhMtk_8195::getWakeUpReason(XBH_WAKEUP_S *stWake)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_SOURCE_E sourceId = XBH_SOURCE_BUTT;
    XBH_U8  pu8Data[10] = {0};

    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_INFO, 1, 10, pu8Data);

    stWake->enWakeup = (XBH_WAKEUP_E)pu8Data[0];
    stWake->u32Value = (uint16_t)(pu8Data[3] * 256 | pu8Data[2]) & 0xffff;
    
    if (XBH_WAKEUP_SOURCE == stWake->enWakeup)
    {
        stWake->enSrc = (XBH_SOURCE_E)pu8Data[1];
    }
    else if (XBH_WAKEUP_CEC == stWake->enWakeup)
    {
        switch(stWake->u32Value)
        {
            case 0x1000:
                stWake->enSrc = XBH_SOURCE_HDMI1;
            break;

            case 0x2000:
                stWake->enSrc = XBH_SOURCE_F_HDMI1;
            break;

            default:
                XLOGD("unknown cec address!");
            break;
        }
    }
    else
    {
        XLOGD("unknown XBH_WAKEUP_E type!");
    }

    XLOGD("getWakeUpReason end %d %d %04x", stWake->enWakeup, stWake->enSrc, stWake->u32Value);

    return s32Ret;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhMtk_8195::getSignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    struct HDMIRX_DEV_INFO deviceInfo;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};

    int devFd = open(HDMIRX_DEV_PATH, O_RDWR);
    if (devFd < 0)
    {
        XLOGE("Error! Open device fail: %s\n", HDMIRX_DEV_PATH);
        return XBH_FAILURE;
    }
    s32Ret = ioctl(devFd, MTK_HDMIRX_DEV_INFO, &deviceInfo);
    if (s32Ret != 0)
    {
        XLOGE("Error! getDeviceInfo ioctl fail\n");
        close(devFd);
        return XBH_FAILURE;
    }

    //底层存在极低概率出现rx5v off。上层也存在第一次进信源无信号。
    //第一次进通道重拉hdmirx，通知上层提升出图稳定性。
    if(mCheckRx5v  < 1)
    {
        mCheckRx5v ++;
        s32Ret = ioctl(devFd, MTK_HDMIRX_ENABLE, 0);
        if (s32Ret != 0) 
        {
            XLOGE("Error! Disable HDMI RX failed\n");
            close(devFd);
            return XBH_FAILURE;
         }

         s32Ret = ioctl(devFd, MTK_HDMIRX_ENABLE, 1);
         if (s32Ret != 0)
         {
             XLOGE("Error! Enable HDMI RX failed\n");
             close(devFd);
             return XBH_FAILURE;
         }

         if ((deviceInfo.hdmirx5v & 0x1)!= 0x1)
         {
             XLOGD("hdmirx5v is off !,HDMI RX reset completed");
             property_set("persist.vendor.hdmirx.reset_error", "1");
         }
         else
         {
            property_get("persist.vendor.hdmirx.reset_error", propVal, "0");
            XLOGD("hdmirx5v is on (%d), force reset rx, propVal  %s", deviceInfo.hdmirx5v, propVal);
            usleep(1*1000*1000);//1s
         }
    }

    *status = ((XBH_BOOL)(XBH_SIGNAL_STATUS_8195_SIGNAL == deviceInfo.state ) ? XBH_SIGNAL_STATUS_SUPPORT : XBH_SIGNAL_STATUS_NOSIGNAL);
    close(devFd);
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getHdmiRxAudioSampleFreq(XBH_U32 *u32Data){
    XBH_S32 s32Ret = XBH_SUCCESS;
    std :: string sampleFreq;
    XbhSysOpt::getInstance()->readSysfs("/sys/class/hdmirxaud/rxaudio/aud_fs",sampleFreq);
    XBH_U32 sampleFreqValue = std::stoi(sampleFreq);
    *u32Data = sampleFreqValue;
    return s32Ret;
}

XBH_S32 XbhMtk_8195::getHdmiRxAudioLocked(XBH_U32 *u32Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    struct HDMIRX_DEV_INFO deviceInfo;
    int devFd = open(HDMIRX_DEV_PATH, O_RDWR);
    if (devFd < 0)
    {
        XLOGE("Error! Open device fail: %s\n", HDMIRX_DEV_PATH);
        return XBH_FAILURE;
    }
    s32Ret = ioctl(devFd, MTK_HDMIRX_DEV_INFO, &deviceInfo);
    if (s32Ret != 0)
    {
        XLOGE("Error! getDeviceInfo ioctl fail\n");
        close(devFd);
        return XBH_FAILURE;
    }

    *u32Data = deviceInfo.aud_locked;
    close(devFd);
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setGoogleKey(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDrmKeyByPath(strPath);
    if (s32Ret)
    {
        XLOGE("setGoogleKey error, filepatch: %s!!!", strPath);
        return XBH_FAILURE;
    }
    XLOGD("setGoogleKey success!!!");
    gNeedKeyUpdate = XBH_TRUE;
    return  s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getGoogleKeyStatus(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if (gNeedKeyUpdate)
    {
        s32Ret = updateDrmKeyStatus(gKeyStatus);
        if (s32Ret)
        {
            *bEnable = XBH_FALSE;
            XLOGE("Error! updateDrmKeyStatus fail, ret = %d", s32Ret);
            return XBH_FAILURE;
        }
        gNeedKeyUpdate = XBH_FALSE;
    }
    *bEnable = (gKeyStatus[KEYMASTER_ATTEST_KEY] == 1 ? XBH_TRUE : XBH_FALSE);
    XLOGD("gKeyStatus[KEYMASTER_ATTEST_KEY] = %d", *bEnable);

    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setGoogleKeyName(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setGoogleKeyName((XBH_VOID *)strPath);
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getGoogleKeyName(XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getGoogleKeyName((XBH_VOID *)strPath);
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setAttentionKey(const XBH_CHAR* strPath)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDrmKeyByPath(strPath);
    if (s32Ret)
    {
        XLOGE("setAttentionKey error, filepatch: %s!!!", strPath);
        return XBH_FAILURE;
    }
    XLOGD("setAttentionKey success!!!");
    gNeedKeyUpdate = XBH_TRUE;
    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getAttentionKeyStatus(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if (gNeedKeyUpdate)
    {
        s32Ret = updateDrmKeyStatus(gKeyStatus);
        if (s32Ret)
        {
            *enable = XBH_FALSE;
            XLOGE("Error! updateDrmKeyStatus fail, ret = %d", s32Ret);
            return XBH_FAILURE;
        }
        gNeedKeyUpdate = XBH_FALSE;
    }
    *enable = (gKeyStatus[KEYMASTER_ATTEST_ID] == 1 ? XBH_TRUE : XBH_FALSE);
    XLOGD("gKeyStatus[KEYMASTER_ATTEST_ID] = %d", *enable);

    return s32Ret;
}

//override
XBH_S32 XbhMtk_8195::setRkpStatus(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setRkpStatus(data);
    XLOGD("setRkpStatus data=%s\n", data);
    return  s32Ret;
}

//override
XBH_S32 XbhMtk_8195::getRkpStatus(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getRkpStatus(data);
    XLOGD("getRkpStatus data=%s\n", data);
    return  s32Ret;
}

/**通过本类public去调用setNvramValue**/
XBH_S32 XbhMtk_8195::updateNvmStr(XBH_CHAR* data)
{
    return setNvramValue(BARCODE_OFFSET, BARCODE_LEN, (XBH_VOID*)data);
}

/**
* 处理type-b信源下热拔插事件
* param[in] src 当前信源
* retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_8195::ProcessTypeBHotplug(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
* 处理type-c信源下热拔插事件
* param[in] src 当前信源
* retval 0:success,-1:failure
*/
XBH_S32 XbhMtk_8195::ProcessTypeCHotplug(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
//------------------ public function begin -----------------------------

//------------------ private function begin -----------------------------
XBH_S32 XbhMtk_8195::setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level)
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

XBH_S32 XbhMtk_8195::getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level)
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
        XLOGE("Error! setChipI2cData ret: %d, cmd: %02x, len: %d, data %02x:%02x:%02x:%02x,return failure \n", ret, stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
        return XBH_FAILURE;
    }
    usleep(1000);//1ms
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
        XLOGE("Error! getChipI2cData ret: %d, cmd: %02x, len: %d, data %02x:%02x:%02x:%02x,return failure\n", ret, stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
        return XBH_FAILURE;
    }
#endif
    //XLOGD("--getMcuGpioValue port: %d, pin: %d, level:%d\n", unGpioGet.attr.port ,unGpioGet.attr.pin ,unGpioGet.attr.level);
    return ret;
}

XBH_S32 XbhMtk_8195::setGpioDirOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
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

XBH_S32 XbhMtk_8195::setGpioDirInput(XBH_U32 u32GpioNumber)
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

XBH_S32 XbhMtk_8195::getGpioDir(XBH_U32 u32GpioNumber)
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

XBH_S32 XbhMtk_8195::openI2cDev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet)
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

XBH_S32 XbhMtk_8195::checkI2cFunc(XBH_S32 file)
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

XBH_S32 XbhMtk_8195::setChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 i = 0;
    XBH_U8 regAddr[4];
    XBH_U32 u32retries = 0;
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
    fd = openI2cDev(u32I2cNum, filename, sizeof(filename), 0);
    if (fd < 0)
    {
        XLOGE("open fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
        return XBH_FAILURE;
    }

    if (checkI2cFunc(fd) < 0)
    {
        XLOGE("checkI2cFunc  fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
        close(fd);
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

    do {
        s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
        if (s32Ret < 0) 
        {
           u32retries++;
           usleep(10000);//10 ms
        }
    } while (s32Ret < 0 && u32retries < MAX_RETRIES);

    if (s32Ret < 0)
    {
        XLOGE("u32I2cNum : %d - 0x%x write reg 0x%x = 0x%x error : %d. Retry: %d strerror:%s",
              u32I2cNum, u8DevAddress, u32RegAddr, *u8Data, s32Ret, MAX_RETRIES, strerror(errno));
        close(fd);
        free(data.msgs);
        return XBH_FAILURE;
    }
    free(data.msgs);
    close(fd);

    return  XBH_SUCCESS;
}

XBH_S32 XbhMtk_8195::getChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 i = 0;
    XBH_U32 u32retries = 0;
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

    fd = openI2cDev(u32I2cNum, filename, sizeof(filename), 0);


    if (fd < 0)
    {
        XLOGE("open fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
        return XBH_FAILURE;
    }

    if (checkI2cFunc(fd) < 0)
    {
        XLOGE("checkI2cFunc  fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
        close(fd);
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

    do {
        s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
        if (s32Ret < 0)
        {
            u32retries++;
            usleep(10000);//10 ms
        }
    } while (s32Ret < 0 && u32retries < MAX_RETRIES);

    if (s32Ret < 0)
    {
        XLOGE("u32I2cNum : %d - 0x%x read reg 0x%x error : %d. Retry: %d strerror:%s,",
              u32I2cNum, u8DevAddress, u32RegAddr, s32Ret, MAX_RETRIES, strerror(errno));
        close(fd);
        free(data.msgs);
        return XBH_FAILURE;
    }
    free(data.msgs);
    close(fd);
    return XBH_SUCCESS;
}

XBH_S32 XbhMtk_8195::readChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 i = 0;

    struct i2c_rdwr_ioctl_data data;
    XBH_S32 fd = -100;
    XBH_UCHAR Addrbuff[35];
    XBH_CHAR filename[20];

    fd = openI2cDev(u32I2cNum, filename, sizeof(filename), 0);

    if (fd < 0 || checkI2cFunc(fd) < 0)
    {
        XLOGE("fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
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

    //read data
    data.msgs[0].len = u32Length;
    data.msgs[0].addr = u8DevAddress;
    data.msgs[0].flags = I2C_M_RD;    // 0: write 1:read
    data.msgs[0].buf = u8Data;

    s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
    if (s32Ret < 0)
    {
        XLOGE("I2cNum : %d - 0x%x read error : %d\r\n",u32I2cNum, u8DevAddress,  s32Ret);
        close(fd);
        free(data.msgs);
        return XBH_FAILURE;
    }

    free(data.msgs);
    close(fd);
    return XBH_SUCCESS;
}

void XbhMtk_8195::convertHexStringToNumbers(const char* retStr, std::vector<uint8_t>& buffArr, size_t offset, size_t size)
{
    size_t retStrLength = strlen(retStr);

    size_t copyLength = (offset + size) * 2;
    if (copyLength > retStrLength) {
        copyLength = retStrLength;
    }

    buffArr.clear();
    buffArr.reserve(copyLength / 2);

    for (size_t i = 0; i < copyLength; i += 2)
    {
        if (i + 1 < retStrLength && isxdigit(retStr[i]) && isxdigit(retStr[i + 1]))
        {
            std::string hexStr(retStr + i, 2);
            uint8_t value = static_cast<uint8_t>(std::stoi(hexStr, nullptr, 16));
            buffArr.push_back(value);
        }
    }
}

void XbhMtk_8195::addHexStringToBuffer(const std::vector<std::string>& hexBuffArr, std::vector<uint8_t>& buffArr, size_t offset, size_t size)
{
    // 确保 buffArr 的大小足够大
    if (buffArr.size() < offset + size) {
        buffArr.resize(offset + size); // 调整 buffArr 的大小
    }

    // 遍历 hexBuffArr，将其转换为数值并存储到 buffArr 中，从 offset 开始
    for (size_t i = 0; i < size && i < hexBuffArr.size(); ++i)
    {
        const std::string& hexStr = hexBuffArr[i];

        // 验证每个字符对是否是有效的十六进制字符串
        if (hexStr.length() == 2 && isxdigit(hexStr[0]) && isxdigit(hexStr[1]))
        {
            // 将十六进制字符串转换为数值
            uint8_t value = static_cast<uint8_t>(std::stoi(hexStr, nullptr, 16));

            // 将数值添加到 buffArr 的 offset 位置
            buffArr[i + offset] = value;
            //XLOGE("Converted value: 0x%02X at position %zu\n", value, i + offset);

        }
        else
        {
            XLOGE("Skipping invalid hex string: %s\n", hexStr.c_str());
        }
    }
}


// 辅助函数：将字符串转换为十六进制字符串数组
std::vector<std::string> XbhMtk_8195::toHexStringArr(const std::string& s)
{
    std::vector<std::string> hexArr; // 用于存储结果的字符串数组

    // 逐字符转换为十六进制字符串
    for (char c : s)
    {
        char buffer[3]; // 用于存储单个字符的十六进制表示，包含2位十六进制数字和终止符
        snprintf(buffer, sizeof(buffer), "%02X", static_cast<unsigned char>(c)); // 转换字符为两位十六进制
        hexArr.push_back(buffer); // 将转换后的字符串添加到数组中
    }

    return hexArr; // 返回十六进制字符串数组
}

XBH_S32 XbhMtk_8195::setNvramValue(XBH_S32 offset, XBH_S32 size, XBH_VOID *buff)
{
    XbhMutex::Autolock lock(m_NvmLock); //预防多进程调用添加同步锁机制
    char retStr[CFG_FILE_PRODUCT_INFO_SIZE * 2] = {0};
    std::vector<uint8_t> buffArr(CFG_FILE_PRODUCT_INFO_SIZE);
    XLOGD("setNvramValue offset = %d, size = %d\n", offset, size);

    if ((offset + size) > CFG_FILE_PRODUCT_INFO_SIZE)
    {
        XLOGE("Size not support. offset = %d, size = %d\n", offset, size);
        return -1;
    }
    XLOGD("setNvramValue buff: %s\n", (char *)buff);

    std::string s_buff(static_cast<char*>(buff), size);

    std::vector<std::string> hexBuffArr = toHexStringArr(s_buff);

    if (mpINvram == nullptr)
    {
        mpINvram = INvram::getService();
        if( mpINvram == nullptr )
        {
            XLOGE("Can't find INvram service...");
            return -1;
        }
    }

    // Get value from nvram
    Return<void> ret = mpINvram->readFileByName(PRODUCT_INFO_FILENAME, offset + size, [&](hidl_string reply) {
        memcpy(retStr, reply.c_str(), reply.size());
    });

    if (!ret.isOk() && ret.isDeadObject())
    {
        XLOGE("Error, readFileByName failed, service is a DEAD_OBJECT.");
        return -1;
    }

    convertHexStringToNumbers(retStr, buffArr, offset, size);

    addHexStringToBuffer(hexBuffArr, buffArr, offset, size);

    hidl_vec<uint8_t> data;
    data.resize(size + offset);
    // 将 buffArr 的内容复制到 data 中
    std::copy(buffArr.begin(), buffArr.begin() + size + offset, data.begin());
    //XLOGE("\n data after copy  contents: \n");

    Return<int8_t> ret2 = mpINvram->writeFileByNamevec(PRODUCT_INFO_FILENAME, offset + size, data);
    if (ret2 != 0)
    {
        XLOGE("Error, readFileByName failed, service is a DEAD_OBJECT.");
        return -1;
    }
    return 0;

}

XBH_S32 XbhMtk_8195::getNvramValue(XBH_S32 offset, XBH_S32 size, XBH_VOID *buff)
{
    int i = 0;
    char tmp[3] = {0};
    char tmpStr[CFG_FILE_PRODUCT_INFO_SIZE] = {0};
    char retStr[CFG_FILE_PRODUCT_INFO_SIZE * 2] = {0};

    if ((offset + size) > CFG_FILE_PRODUCT_INFO_SIZE)
    {
        XLOGE("Size not support. offset = %d, size = %d\n", offset, size);
        return -1;
    }

    //Get INvram service handle
    if (mpINvram == nullptr)
    {
        mpINvram = INvram::getService();
        if(mpINvram == nullptr)
        {
            XLOGE("Can't find INvram service...");
            return -1;
        }
    }

    // Get value from nvram
    Return<void> ret = mpINvram->readFileByName(PRODUCT_INFO_FILENAME, offset + size, [&](hidl_string reply) {
        memcpy(retStr, reply.c_str(), reply.size());
    });
    if (!ret.isOk() && ret.isDeadObject())
    {
        XLOGE("Error, readFileByName failed, service is a DEAD_OBJECT.");
        return -1;
    }
    //XLOGE("getNvramValue origin retStr: %s\n", retStr);

    // Convert hex string to char*
    for (i = 0; i < size; i++)
    {
        memcpy(tmp, &retStr[(offset + i) * 2], 2);
        tmp[2] = '\0';
        tmpStr[i] = strtol(tmp, NULL, 16);
    }

    memcpy(buff, tmpStr, size);

    return 0;
}
XBH_S32 XbhMtk_8195::setMcuEnableKeyPower()
{
#define CMD_I2C_SET_KEY_POWER   0xE4
    XBH_U8  pu8Data[2] = {0x01, 0x02};

    return  setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_KEY_POWER, 1, 2, pu8Data);
}

//------------------ private function end -----------------------------
