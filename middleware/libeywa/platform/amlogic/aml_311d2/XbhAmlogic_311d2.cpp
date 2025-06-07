#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAmlogic_311d2"

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

#include <hardware/aml_gpio.h>
#include <hardware/board.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/rtc.h>

#include <cutils/properties.h>
#include <utils/String8.h>

#include "XbhLog.h"
#include "XbhAmlogic_311d2.h"
#include "XbhSysOpt.h"
#include "XbhParseINI.h"
#include "SystemControlClient.h"
#include "PQType.h"
#include "XbhNfcManager.h"
#include "XbhService.h"

#ifdef SUPPORT_TVSERVICE
#include "TvServerHidlClient.h"
#endif

using namespace android;

#define XBH_CUST_PROPERTY_NOT_INIT "persist.vendor.xbh.prop_not_init"

SystemControlClient *mSystemControlClient;
static const char rtc_sysfs[] = "/sys/class/rtc";

XbhMutex                    XbhAmlogic_311d2::mLock;

#define GOOGLE_KEY_PROVISION_ID   0x42        // Attestation key 的 provision 索引
#define GOOGLE_KEY_UNIFY_NODE     "prprivkeybox_1"
#define GOOGLE_KEY_MAX_LEN        64          
#define XBH_PATH_MAX              32


#ifdef SUPPORT_TVSERVICE
static Mutex amLock;
static sp<TvServerHidlClient> mTvService = nullptr;
static const sp<TvServerHidlClient> &getTvService()
{
    Mutex::Autolock _l(amLock);
    if (mTvService == nullptr) {
        mTvService = new TvServerHidlClient(CONNECT_TYPE_HAL);
    }
    return mTvService;
}
#endif

//----------------------------------------------------- static function begin ------------------------------------------------------------------------------------------------------
static XBH_U8 source_available[] = XBH_SRC_AVAILABLE_TABLE

struct UserData
{
    XBH_U32 gpio;
    XBH_U32 state;
};

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
//----------------------------------------------------- static function end---------------------------------------------------------------------------------------------------------
XBH_S32 XbhAmlogic_311d2::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_FAILURE;
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

    if (mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;
    if (getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        s32Ret = ioctl(mGpioFd, SET_IO_OUT, &userdata);
        if (s32Ret < 0)
        {
            XLOGE("-- set gpio-%d out value : %d fail --", userdata.gpio, userdata.state);
            return XBH_FAILURE;
        }
    }

    s32Ret = ioctl(mGpioFd, SET_IO_VAL, &userdata);
    if (s32Ret < 0)
    {
        XLOGE("-- set gpio-%d out value : %d fail --", userdata.gpio, userdata.state);
        return XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_FAILURE;
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
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_OUTPUT, u32GpioValue);
        return s32Ret;
    }
#endif

    if (mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = XBH_BOARD_GPIO_OUTPUT;

    if (getGpioDir(u32GpioNumber) != XBH_BOARD_GPIO_OUTPUT)
    {
        userdata.state = 0;
        s32Ret = ioctl(mGpioFd, GET_IO_VAL, &userdata);
        if (s32Ret < 0)
        {
            XLOGE("-- get gpio-%d value fail --", userdata.gpio);
            return XBH_FAILURE;
        }
        setGpioOutput(userdata.gpio, userdata.state);
        *u32GpioValue = (XBH_U32)userdata.state;
        return s32Ret;
    }

    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_OUT_VAL, &userdata);
    if (s32Ret < 0)
    {
        XLOGE("-- get gpio-%d out value fail --", userdata.gpio);
        return XBH_FAILURE;
    }

    *u32GpioValue = (XBH_U32)userdata.state;
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t mcu_gpio_port = 0xff;
    uint8_t mcu_gpio_pin  = 0xff;

#if defined(MCU_GPIO_MAP_INDEX_START) && defined(MCU_GPIO_MAP_INDEX_END)
    //MCU GPIP START OF 0X10000
    if (u32GpioNumber >= MCU_GPIO_MAP_INDEX_START && u32GpioNumber <= MCU_GPIO_MAP_INDEX_END)
    {
        if(XbhService::getModuleInterface()->mMcuUpdateLock == true)
        {
            XLOGE("MCU is upgrading, setGpioOutputValue(MCU GPIO) has been disabled\n");
            return XBH_FAILURE;
        }

        mcu_gpio_port = (u32GpioNumber >> 16) - 1;
        mcu_gpio_pin = u32GpioNumber & 0X0f;
        XLOGE("-- gpio is in mcu numner: %d, mcu port: %d, pin : %d \n",u32GpioNumber, mcu_gpio_port, mcu_gpio_pin);
        s32Ret = getMcuGpioValue(mcu_gpio_port, mcu_gpio_pin, MCU_GPIO_MODE_INPUT, u32GpioValue);
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
    userdata.state = XBH_BOARD_GPIO_INPUT;

    if(getGpioDir(u32GpioNumber) == XBH_BOARD_GPIO_OUTPUT)
    {
        setGpioInput(userdata.gpio);
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

XBH_S32 XbhAmlogic_311d2::setIICLock(XBH_U32 iicBus, XBH_BOOL lock)
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

XBH_S32 XbhAmlogic_311d2::setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
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

XBH_S32 XbhAmlogic_311d2::getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XbhMutex::Autolock _l(mLock);
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

XBH_S32 XbhAmlogic_311d2::getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable)
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

XBH_S32 XbhAmlogic_311d2::setDebugEnable(XBH_BOOL bEnable)
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

XBH_S32 XbhAmlogic_311d2::getDebugEnable(XBH_BOOL *bEnable)
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

XBH_S32 XbhAmlogic_311d2::isNfcExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getChipExist(enable);
    XLOGW("isNfcExist s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->setNfcEnable(enable);
    XLOGW("setNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->getNfcEnable(enable);
    XLOGW("getNfcEnable s32Ret = %d", s32Ret);
    return s32Ret;
}

//----------------------------------------------------------------------------- private func start here -----------------------------------------------------------------
XBH_S32 XbhAmlogic_311d2::setGpioOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    if (mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;
    userdata.state = u32GpioValue;

    s32Ret = ioctl(mGpioFd, SET_IO_OUT, &userdata);
    if (s32Ret < 0)
    {
        XLOGE("-- set gpio-%d out value : %d fail --", userdata.gpio, userdata.state);
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::setGpioInput(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    if (mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;

    s32Ret = ioctl(mGpioFd, SET_IO_IN, &userdata);
    if (s32Ret < 0)
    {
        XLOGE("-- set gpio-%d input : %d fail --", userdata.gpio);
        return XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getGpioDir(XBH_U32 u32GpioNumber)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    if (mGpioFd < 0)
    {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = u32GpioNumber;

    s32Ret = ioctl(mGpioFd, GET_IO_DIR, &userdata);
    if (s32Ret < 0)
    {
        XLOGE("-- get gpio-%d dir fail --", userdata.gpio);
        return XBH_FAILURE;
    }
    return userdata.state;
}

XBH_S32 XbhAmlogic_311d2::open_i2c_dev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet)
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

XBH_S32 XbhAmlogic_311d2:: check_iic_funcs(XBH_S32 file)
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

/**
 * 写入Project ID
 * param[in] value Project ID
 * retval true:success, false:failure
*/
XBH_S32 XbhAmlogic_311d2::setProjectId(const XBH_CHAR* strProjectId)
{
    int entryCount = 0;
    std::string prop_path = std::string("/vendor/etc/projects/projectid_") + std::string(strProjectId) + std::string("/cusprop/cust.prop");
    if (access(const_cast<char *>(prop_path.c_str()), F_OK) == 0){
        IniEntry* entries = XbhParseINI::parseINI(const_cast<char *>(prop_path.c_str()), &entryCount);
        for (int i = 0; i < entryCount; i++) {
            if (entries != NULL)
            {
                property_set(entries[i].key, entries[i].value);
            }
        }
        if(entries != NULL)
        {
            free(entries);
        }
    }
    return mSystemControlClient->writeUnifyKey("projectid", strProjectId) ? XBH_SUCCESS : XBH_FAILURE;
}

/**
 * 获取Project ID
 * param[out] strProjectId Project ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAmlogic_311d2::getProjectId(XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string projectid;
    if(mSystemControlClient->readUnifyKey("projectid", projectid)){
        strcpy(strProjectId,projectid.c_str());
        XLOGI("%s %d: projectid -> [%s] \n",__func__,__LINE__,strProjectId);
    }
    else
    {
        XLOGE("%s %d: projectid error \n",__func__,__LINE__);
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::loadProperty()
{
    int entryCount = 0;
    XBH_S32 flag = 0;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XLOGD("%s %d loadProperty start !!!! \n", __func__, __LINE__);
    IniEntry* entries = XbhParseINI::parseINI("/mnt/cust/systemproperty.ini", &entryCount);
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

XBH_S32 XbhAmlogic_311d2::setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (type > 0)
    {
        mSystemControlClient->setBootEnv("ubootenv.var.panel1_type", "vbyone_0");
    }
    else
    {
        mSystemControlClient->setBootEnv("ubootenv.var.panel1_type", "vbyone_1");
    }
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::loadEdlaCust(XBH_BOOL *enable)
{
    XBH_U32 res = XBH_FAILURE;
    res = loadProperty();
    //customerversion 一致不需要重启
    if (res == 1) {
        *enable = XBH_FALSE;
    } else {
        *enable = XBH_TRUE;
    }
    return res;
}

XBH_S32 XbhAmlogic_311d2::set_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
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

XBH_S32 XbhAmlogic_311d2::get_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
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

XBH_BOOL XbhAmlogic_311d2::rtc_is_hctosys(XBH_U32 rtc_id)
{
    android::String8 hctosys_path = String8::format("%s/rtc%u/hctosys", rtc_sysfs, rtc_id);
    FILE *file = fopen(hctosys_path.string(), "re");
    if (!file) 
    {
        XLOGE("failed to open %s: %s", hctosys_path.string(), strerror(errno));
        return XBH_FAILURE;
    }

    XBH_U32 hctosys;
    XBH_BOOL ret = XBH_FALSE;
    XBH_U32 err = fscanf(file, "%u", &hctosys);
    if (err == EOF)
        XLOGE("failed to read from %s: %s", hctosys_path.string(), strerror(errno));
    else if (err == 0)
        XLOGE("%s did not have expected contents", hctosys_path.string());
    else
        ret = (XBH_BOOL)hctosys;

    fclose(file);
    return ret;
}

XBH_S32 XbhAmlogic_311d2::wall_clock_rtc()
{
    std::unique_ptr<DIR, int(*)(DIR*)> dir(opendir(rtc_sysfs), closedir);
    if (!dir.get()) 
    {
        XLOGE("failed to open %s: %s", rtc_sysfs, strerror(errno));
        return XBH_FAILURE;
    }

    struct dirent *dirent;
    while (errno = 0, dirent = readdir(dir.get())) 
    {
        unsigned int rtc_id;
        int matched = sscanf(dirent->d_name, "rtc%u", &rtc_id);

        if (matched < 0)
            break;
        else if (matched != 1)
            continue;

        if (rtc_is_hctosys(rtc_id)) 
        {
            XLOGV("found wall clock RTC %u", rtc_id);
            return rtc_id;
        }
    }

    if (errno == 0)
        XLOGW("no wall clock RTC found");
    else
        XLOGE("failed to enumerate RTCs: %s", strerror(errno));
    return XBH_FAILURE;
}
XBH_S32 XbhAmlogic_311d2::setRtcTime(XBH_RTC_INFO_S* stRtcInfo)
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

XBH_S32 XbhAmlogic_311d2::getRtcTime(XBH_RTC_INFO_S* stRtcInfo)
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

XBH_S32 XbhAmlogic_311d2::setRtcAlarmTime(XBH_RTC_ALARM_INFO_S* stRtcAlarmInfo)
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

XBH_S32 XbhAmlogic_311d2::initSerialPort()
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

XBH_S32 XbhAmlogic_311d2::setWolMode(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 register_value = 0;

    XLOGE("%s %d update WOL switch,  bEnable = %d \n", __func__, __LINE__, bEnable);
    //XbhNeticTask::getInstance()->setWolEnable(bEnable);
    property_set("persist.vendor.xbh.wol.enable", bEnable ? "true" : "false");

    if(mSystemControlClient)
    {
        if( mSystemControlClient->readSysRegister(WAKEUP_REASON_STICK_REG, register_value) ) //操作寄存器，告知bl30 无需WOL唤醒
        {
            XLOGW("%s %d: set readSysRegister success register_value=%x\n",__func__,__LINE__, register_value);

            if(bEnable) //打开WOL唤醒
                register_value |= 0x20;
            else
                register_value &= ~0x20;

            if( mSystemControlClient->writeSysRegister(WAKEUP_REASON_STICK_REG, register_value) )
            {
                XLOGW("%s %d: set writeSysRegister success register_value=%x\n",__func__,__LINE__, register_value);
                s32Ret = XBH_SUCCESS;
            }
            else
            {
                XLOGW("%s %d: writeSysRegister fail!\n",__func__,__LINE__);
            }
        }
        else
        {
            XLOGW("%s %d: readSysRegister fail! register_value=%d\n",__func__,__LINE__, register_value);
        }
    }
    return s32Ret;
}

static int is_multicast_ethaddr(const int *addr)
{
    return 0x01 & addr[0];
}


static int is_zero_ethaddr(const int *addr)
{
    return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

XBH_BOOL XbhAmlogic_311d2::is_valid_mac_addr(const XBH_CHAR* macAddr) 
{

    if(macAddr==NULL || strlen(macAddr)==0) {
        XLOGE("macAddr is NULL\n");
        return false;
    }

    XBH_CHAR* p = const_cast<XBH_CHAR*>(macAddr);

    if (17 != strlen(macAddr))
    {
        XLOGE("Mac Address format error.");
        return false;
    }
    while (*p != '\0')
    {
        if (2 == (p - macAddr)%3)
        {
            if ( *p != ':')
            {
                XLOGE("Mac Address format error.");
                return false;
            }
            p++;
            continue;
        }
        if (*p < '0' || (*p > '9' && *p < 'A') ||(*p > 'F' && *p < 'a') || *p > 'f' )
        {
            XLOGE("Mac Address format error.");
            return false;
        }
        p++;
    }

    int macHex[6];
    sscanf(macAddr, "%02X:%02X:%02X:%02X:%02X:%02X", &macHex[0], &macHex[1], &macHex[2], &macHex[3], &macHex[4], &macHex[5]);

    if(is_multicast_ethaddr(macHex) || is_zero_ethaddr(macHex)){
        XLOGE("mac addr : %s is multicast or is zero!!!\n",macAddr);
        return false;
    }

    return true;
}

XBH_S32 XbhAmlogic_311d2::setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(is_valid_mac_addr(strMacAddress))
    {
        XLOGI("%s %d: mac address -> [%s] \n",__func__,__LINE__,strMacAddress);
    } else {
        XLOGE("%s %d: mac address error \n",__func__,__LINE__);
        return XBH_FAILURE;
    }
    return mSystemControlClient->writeUnifyKey("mac", strMacAddress) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhAmlogic_311d2::getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string macAddress;

    if(mSystemControlClient->readUnifyKey("mac", macAddress)){
        strcpy(strMacAddress,macAddress.c_str());
        XLOGD("%s %d: get mac from unifyKey : %s\n",__func__,__LINE__,strMacAddress);
    }

    if(!is_valid_mac_addr(strMacAddress)) {
        if(mSystemControlClient->getBootEnv("ubootenv.var.ethaddr", macAddress)){
            strcpy(strMacAddress,macAddress.c_str());
            XLOGW("%s %d: mac unifyKey invalid, get mac from BootEnv : %s\n",__func__,__LINE__,strMacAddress);
        }
    }

    if(is_valid_mac_addr(strMacAddress))
    {
        XLOGI("%s %d: mac address -> [%s] \n",__func__,__LINE__,strMacAddress);
    } else {
        XLOGE("%s %d: mac address error \n",__func__,__LINE__);
        s32Ret = XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::setTconPower(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    const std :: string tcon_path_0 = "/sys/class/aml_lcd/lcd0/power_on";
    const std :: string tcon_path_1 = "/sys/class/aml_lcd/lcd1/power_on";
    std::string outputmode;
    std :: string value;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    
    if(property_get("persist.xbh.panel.onoff", propVal, "true") > 0)
    {
        if(strcmp(propVal, "false") == 0)
        {
             XLOGE("%s: %d config don't set panel onOff\n",__func__,__LINE__);
             return XBH_FAILURE;
        }

    }
    if(bEnable == 1)
    {
        value = "1";
    }
    else if(bEnable == 0)
    {
        value = "0";
    }
    else
    {
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return XBH_FAILURE;
    }
    if(mSystemControlClient->getBootEnv("ubootenv.var.outputmode", outputmode)){
        if (outputmode == "panel") {
            s32Ret = mSystemControlClient->writeSysfs(tcon_path_0,value);
        } else if (outputmode == "panel1") {
            s32Ret = mSystemControlClient->writeSysfs(tcon_path_1,value);
        } else {
            s32Ret = (mSystemControlClient->writeSysfs(tcon_path_0,value) | mSystemControlClient->writeSysfs(tcon_path_1,value));
        }
    } else {
        s32Ret = (mSystemControlClient->writeSysfs(tcon_path_0,value) | mSystemControlClient->writeSysfs(tcon_path_1,value));
    }

    XLOGD("%s: %d set tcon power = %d,result = %d\n",__func__,__LINE__,bEnable,s32Ret);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getTconPower(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const std :: string tcon_path_0 = "/sys/class/aml_lcd/lcd0/power_on";
    const std :: string tcon_path_1 = "/sys/class/aml_lcd/lcd1/power_on";
    std :: string result0;
    std :: string result1;
    XBH_BOOL bState0 = XBH_FALSE;
    XBH_BOOL bState1 = XBH_FALSE;

    mSystemControlClient->readSysfs(tcon_path_0, result0);
    if(!result0.empty())
    {
        result0 = result0.substr(result0.length() - 1,1);
        if(strcmp(result0.c_str(), "0") || strcmp(result0.c_str(), "1"))
        {
            bState0 = atoi(result0.c_str());
            XLOGV("%s: %d bState0 = %d\n",__func__,__LINE__,bState0);
            s32Ret = XBH_SUCCESS;
        }
    }

    mSystemControlClient->readSysfs(tcon_path_1, result1);
    if(!result1.empty())
    {
        result1 = result1.substr(result1.length() - 1,1);
        if(strcmp(result1.c_str(), "0") || strcmp(result1.c_str(), "1"))
        {
            bState1 = atoi(result1.c_str());
            XLOGV("%s: %d bState1 = %d\n",__func__,__LINE__,bState1);
            s32Ret = XBH_SUCCESS;
        }
    }

    *bEnable = (bState0|bState1);
    XLOGD("%s: %d get tcon power = %d\n",__func__,__LINE__,*bEnable);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    XBH_BOOL bNeedPanelOnOff = XBH_TRUE;
    if(mGpioFd < 0)
     {
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }

    if(property_get("persist.xbh.panel.onoff", propVal, "true") > 0)
    {
        if(strcmp(propVal, "false") == 0)
        {
            bNeedPanelOnOff = XBH_FALSE;
            XLOGE("%s: %d config don't set panel onOff\n",__func__,__LINE__);
        }

    }
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(bEnable == 1)
    {
        if(bNeedPanelOnOff)
        {
            //panel VCC
            #ifdef XBH_BOARD_GPIO_TCON0_PWR_PIN
                XLOGI("%s: %d set XBH_BOARD_GPIO_TCON0_PWR_PIN = %d\n",__func__,__LINE__,bEnable);
                setGpioOutputValue(XBH_BOARD_GPIO_TCON0_PWR_PIN, XBH_BOARD_GPIO_TCON0_PWR_ON);
                usleep(50 * 1000);
                setTconPower(bEnable);
            #endif
                usleep(400 * 1000);
        }
    }

//backlight
#ifdef XBH_BOARD_GPIO_BL0_BACKLIGHT_PIN
    XLOGI("%s: %d set backlight OnOff = %d\n",__func__,__LINE__,bEnable);
    setGpioOutputValue(XBH_BOARD_GPIO_BL0_BACKLIGHT_PIN, ((bEnable == 1)? XBH_BOARD_GPIO_BL0_BACKLIGHT_ON : !XBH_BOARD_GPIO_BL0_BACKLIGHT_ON));
#endif
    if(bEnable != 1)
    {
        if(bNeedPanelOnOff)
        {
            #if (defined XBH_BOARD_GPIO_TCON0_PWR_PIN)
                    usleep(100 * 1000);
                    XLOGI("%s: %d set panel input data = %d\n",__func__,__LINE__,bEnable);
                    setTconPower(bEnable); //input Data
            #endif

            //panel VCC
            #ifdef XBH_BOARD_GPIO_TCON0_PWR_PIN
                XLOGI("%s: %d set XBH_BOARD_GPIO_TCON0_PWR_PIN = %d\n",__func__,__LINE__,bEnable);
                setGpioOutputValue(XBH_BOARD_GPIO_TCON0_PWR_PIN, !XBH_BOARD_GPIO_TCON0_PWR_ON);
            #endif

        }

    }
    XLOGI("%s: %d set Backlight enable = %d,result = %d\n",__func__,__LINE__,bEnable,s32Ret);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
#ifdef XBH_BOARD_GPIO_BL0_BACKLIGHT_PIN
    if(mGpioFd < 0){
        XLOGE("-- gpio node not init --\n");
        return XBH_FAILURE;
    }
    UserData userdata;
    memset(&userdata, 0, sizeof(UserData));
    userdata.gpio = XBH_BOARD_GPIO_BL0_BACKLIGHT_PIN;
    userdata.state = 0;
    s32Ret = ioctl(mGpioFd, GET_IO_OUT_VAL, &userdata);
    if(s32Ret < 0){
        XLOGE("-- get gpio-%d out value fail --",userdata.gpio);
        return XBH_FAILURE;
    }
    *bEnable = ((userdata.state == XBH_BOARD_GPIO_BL0_BACKLIGHT_ON)? 1 : 0 );
     XLOGI("%s: %d get Backlight enable = %d\n",__func__,__LINE__,*bEnable);
#endif
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::setPictureMode(XBH_PICMODE_E enPicMode)
{
    int picmode = enPicMode;
    XLOGI("%s: %d set enPicMode = %d\n",__func__,__LINE__,enPicMode);
    if(enPicMode < XBH_PICMODE_STANDARD || enPicMode >= XBH_PICMODE_BUTT){
        XLOGE("%s: %d param error\n",__func__,__LINE__);
        return XBH_FAILURE;
    }
    #ifdef XBH_BORAD_HAS_GAME_MODE
    if (enPicMode == XBH_PICMODE_STANDARD)
    {
        picmode = XBH_PICMODE_GAME;
        XLOGI("%s: %d set PicMode = %d\n",__func__,__LINE__,picmode);
    }
    #endif
    return mSystemControlClient->setPQmode(picmode, 1, 0) >= 0 ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhAmlogic_311d2::getPictureMode(XBH_PICMODE_E* enPicMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = mSystemControlClient->getPQmode();
    if(s32Ret < XBH_PICMODE_STANDARD || s32Ret >= XBH_PICMODE_BUTT){
        XLOGE("%s: %d enPicMode is error %d\n",__func__,__LINE__,s32Ret);
        return XBH_FAILURE;
    }
    #ifdef XBH_BORAD_HAS_GAME_MODE
    if(s32Ret == XBH_PICMODE_GAME)
        s32Ret = XBH_PICMODE_STANDARD;
    #endif
    *enPicMode = (XBH_PICMODE_E)s32Ret;
    XLOGI("%s: %d get enPicMode = %d\n",__func__,__LINE__,*enPicMode);
    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::getADCChannelValue(XBH_U32 u32Channel, XBH_U32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std :: string result;

    const std :: string adc0_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage0_input";
    const std :: string adc1_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage1_input";
    const std :: string adc2_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage2_input";
    const std :: string adc3_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage3_input";
    const std :: string adc4_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage4_input";
    const std :: string adc5_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage5_input";
    const std :: string adc6_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage6_input";
    const std :: string adc7_path = "/sys/devices/platform/fe026000.saradc/iio:device0/in_voltage7_input";

    switch (u32Channel)
    {
        case XBH_ADC_CHANNEL_NUM_0:
              mSystemControlClient->readSysfs(adc0_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_1:
              mSystemControlClient->readSysfs(adc1_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_2:
              mSystemControlClient->readSysfs(adc2_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_3:
              mSystemControlClient->readSysfs(adc3_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_4:
              mSystemControlClient->readSysfs(adc4_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_5:
              mSystemControlClient->readSysfs(adc5_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_6:
              mSystemControlClient->readSysfs(adc6_path, result);
              break;
        case XBH_ADC_CHANNEL_NUM_7:
              mSystemControlClient->readSysfs(adc7_path, result);
              break;
        default:
            XLOGE("Error Adc Channel");
            return XBH_FAILURE;
    }

    if(!result.empty())
    {
        *u32Value = atoi(result.c_str());
        //XLOGD("%s: %d get Adc value = %d\n",__func__,__LINE__,*u32Value);
        return XBH_SUCCESS;
    }

    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getTemperatureSensorValue(XBH_FLOAT *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
#ifdef CUSTOMER_AML_NTC_ADC_THERMISTOR_CHAN
    XBH_U32 adcChannel = CUSTOMER_AML_NTC_ADC_THERMISTOR_CHAN;
    XBH_U32 adcValue   = 0;
    s32Ret = getADCChannelValue(adcChannel,&adcValue);

    XBH_U32 value = adcValue * 255 / 1023;
    if ((255 - value) > 0) {
        XBH_U32 tAdcVal = CUSTOMER_AML_NTC_ADC_THERMISTOR_VAL * 1000 * value / (255 - value);
        XBH_S32 tempIndex = 0;
        for(tempIndex=0; tempIndex<NTC_ADC_100K_0402_LEN; tempIndex++)
        {
            if(tAdcVal > NTC_ADC_100K_0402[tempIndex])
            {
               *s32Value = NTC_ADC_100K_0402_MIN + tempIndex;
                //XLOGD("getBoardTemperature : ADC-CH%u = %u  tempIndex = %d convert to degree centigrade : %d",adcChannel,adcValue,tempIndex,*s32Value);
                s32Ret = XBH_SUCCESS;
                break;
            }
        }
        if(tempIndex >= NTC_ADC_100K_0402_LEN)
        {
            XLOGE("getBoardTemp: Invalid value = %d", value);
        }
    } else {
        XLOGE("getBoardTemp: get adc failed!!! value = %d", value);
        s32Ret = XBH_FAILURE;
    }
#else
    XLOGW("getBoardTemperature : no thermistor specified!");
#endif
    return s32Ret;
}

XBH_U32 XbhAmlogic_311d2::getMcuGpioChecksum(XBH_MCU_GPIO_ATTR_S *p_attr)
{
    return (p_attr->port ^ p_attr->pin ^ p_attr->mode ^ p_attr->speed ^ p_attr->level ^ 0x4c) & 0xff;
}

XBH_S32 XbhAmlogic_311d2::setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_MCU_GPIODEF_U unGpioSet;
    XBH_MCU_I2CBUFFDEF_S stOutData;
    if ((0xff == u8Group) || (0xff == u8GpioNum))
    {
        XLOGE("-- mcu gpio port and pin error \n");
        return XBH_FAILURE;
    }

    unGpioSet.attr.port = u8Group;
    unGpioSet.attr.pin = u8GpioNum;
    unGpioSet.attr.level = level;
    unGpioSet.attr.mode = GPIO_MODE_OUT_PP;
    unGpioSet.attr.speed = GPIO_OSPEED_10MHZ;
    unGpioSet.attr.checksum = getMcuGpioChecksum(&unGpioSet.attr);

    stOutData.len = sizeof(XBH_MCU_GPIODEF_U);
    stOutData.cmd = CMD_I2C_SET_GPIO_OUTPUT;
    stOutData.data[0] = (unGpioSet.value >> 24) & 0xFF;
    stOutData.data[1] = (unGpioSet.value >> 16) & 0xFF;
    stOutData.data[2] = (unGpioSet.value >> 8) & 0xFF;
    stOutData.data[3] = unGpioSet.value & 0xFF;

#if defined(XBH_MCU_I2C_CHN) && defined(XBH_MCU_I2C_ADDR)
    ret = set_i2c_data(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stOutData.cmd, 0x01, stOutData.len, stOutData.data);
    XLOGD("--set_i2c_data mcu cmd: %d, %02x, %02x, len: %d, data %02x:%02x:%02x:%02x \n", XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stOutData.cmd, stOutData.len, stOutData.data[0], stOutData.data[1], stOutData.data[2],stOutData.data[3]);
#endif
    return ret;
}

XBH_S32 XbhAmlogic_311d2::getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_MCU_GPIODEF_U unGpioGet;
    XBH_MCU_I2CBUFFDEF_S stInData;
    if ((0xff == u8Group) || (0xff == u8GpioNum))
    {
        XLOGE("-- mcu gpio port and pin error \n");
        return XBH_FAILURE;
    }

    if (mode == MCU_GPIO_MODE_OUTPUT)
    {
        unGpioGet.attr.mode = GPIO_MODE_OUT_PP;
        stInData.cmd = CMD_I2C_GET_GPIO_OUTPUT;
    }
    else
    {
        unGpioGet.attr.mode = GPIO_MODE_IN_FLOATING;
        stInData.cmd = CMD_I2C_GET_GPIO_INPUT;
    }
    unGpioGet.attr.port = u8Group;
    unGpioGet.attr.pin = u8GpioNum;
    unGpioGet.attr.speed = GPIO_OSPEED_10MHZ;
    unGpioGet.attr.checksum = getMcuGpioChecksum(&unGpioGet.attr);

    stInData.len = sizeof(XBH_MCU_GPIODEF_U);
    stInData.data[0] = (unGpioGet.value >> 24) & 0xFF;
    stInData.data[1] = (unGpioGet.value >> 16) & 0xFF;
    stInData.data[2] = (unGpioGet.value >> 8) & 0xFF;
    stInData.data[3] = unGpioGet.value & 0xFF;

#if defined(XBH_MCU_I2C_CHN) && defined(XBH_MCU_I2C_ADDR)
    ret = set_i2c_data(XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stInData.cmd, 0x01, stInData.len, stInData.data);
    XLOGD("--set_i2c_data mcu cmd: %d, %02x, %02x, len: %d, data %02x:%02x:%02x:%02x \n", XBH_MCU_I2C_CHN, XBH_MCU_I2C_ADDR, stInData.cmd, stInData.len, stInData.data[0], stInData.data[1], stInData.data[2],stInData.data[3]);
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

XBH_S32 XbhAmlogic_311d2::setWosEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U32 register_value = 0;
    XLOGE("%s %d enter : bEnable = %d \n", __func__, __LINE__, bEnable);
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    sprintf(propVal, "%s", bEnable == XBH_TRUE ? "true" : "false");
    property_set("persist.vendor.xbh.wos.enable", propVal);

    if(mSystemControlClient)
    {
        if( mSystemControlClient->readSysRegister(WAKEUP_REASON_STICK_REG, register_value) ) //操作寄存器，告知bl30 无需自动唤醒
        {
            XLOGW("%s %d: set readSysRegister success register_value=%x\n",__func__,__LINE__, register_value);

            if(bEnable) //打开自动唤醒
                register_value |= 0x10;
            else
                register_value &= ~0x10;

            register_value &= ~0x40;//disable code bootmode

            if( mSystemControlClient->writeSysRegister(WAKEUP_REASON_STICK_REG, register_value) )
            {
                mSystemControlClient->setBootEnv("ubootenv.var.auto_wakeup_sw", bEnable == XBH_TRUE ? "on" : "off");
                XLOGW("%s %d: writeSysRegister %x success! set auto_wakeup_sw to BootEnv : %s\n",
                    __func__,__LINE__, register_value, bEnable == XBH_TRUE ? "on" : "off");
                s32Ret = XBH_SUCCESS;
            }
            else
                XLOGW("%s %d: writeSysRegister fail!\n",__func__,__LINE__);
        }
        else
            XLOGW("%s %d: readSysRegister fail! register_value=%d\n",__func__,__LINE__, register_value);
    }

    return s32Ret;
}

XBH_BOOL XbhAmlogic_311d2::getWosEnable()
{
    XBH_BOOL bRet = XBH_FALSE;
    XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    if(property_get("persist.vendor.xbh.wos.enable", propVal, "true") > 0){
        if(strcmp(propVal, "true") == 0){
            bRet = XBH_TRUE;
        } else {
            bRet = XBH_FALSE;
        }
    }
    XLOGW("%s %d exit :isEnable = %d \n", __func__, __LINE__, bRet);
    return bRet;
}

XBH_S32 XbhAmlogic_311d2::setBootLogo(const XBH_CHAR* path)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    mSystemControlClient->setBootEnv("ubootenv.var.xbhCustLogoPath", path);//set uboot env
    return s32Ret;
}


XBH_S32 XbhAmlogic_311d2::setPoweronMode(XBH_POWERMODE_E enMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(enMode == XBH_POWERMODE_STANDBY){
        mSystemControlClient->setBootEnv("ubootenv.var.powermode", "standby");
    }else if(enMode == XBH_POWERMODE_MEMORY){
        mSystemControlClient->setBootEnv("ubootenv.var.powermode", "last");
    }else{
        mSystemControlClient->setBootEnv("ubootenv.var.powermode", "direct");
    }
    XLOGW("%s %d: Set power mode:%d to BootEnv\n",__func__,__LINE__, enMode);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getPoweronMode(XBH_POWERMODE_E *enMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    std::string mode;
    if(mSystemControlClient->getBootEnv("ubootenv.var.powermode", mode)){
        if(mode.compare("standby") == 0){
            *enMode = XBH_POWERMODE_STANDBY;
        } else if(mode.compare("last") == 0){
            *enMode = XBH_POWERMODE_MEMORY;
        } else{
            *enMode = XBH_POWERMODE_ON;
        }
        XLOGW("%s %d: Get power mode from BootEnv : %s\n",__func__,__LINE__,mode.c_str());
    }
    return s32Ret;
}


//Set Chip RunTime
XBH_S32 XbhAmlogic_311d2::setChipRunTime(XBH_S32 time)
{
    if (time < 0) {
        return XBH_FAILURE;
    }

    XBH_CHAR cTime[128] = {0};
    snprintf(cTime, sizeof(cTime), "%d", time);

    return mSystemControlClient->writeUnifyKey("chip_runtime", cTime) ? XBH_SUCCESS : XBH_FAILURE;
}

//Get Chip RunTime
XBH_S32 XbhAmlogic_311d2::getChipRunTime(XBH_S32* time)
{
    if (time == nullptr) {
        return XBH_FAILURE;
    }

    std::string strTime;

    if (!mSystemControlClient->readUnifyKey("chip_runtime", strTime)) {
        return XBH_FAILURE;
    }

    *time = atoi(strTime.c_str());

    if (*time < 0) {
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::setHdmirxEdidType(XBH_S32 type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 edidType;
    XBH_S32 port;

    if (type == 1)
    {
        edidType = 0;              // EDID 1.4
    }
    else if(type == 2)
    {
        edidType = 1;              // EDID 2.0
    }
    else if(type == 3)
    {
        edidType = 1;              // EDID 2.1
    }
    else if(type == 4)
    {
        edidType = 2;              // 自动模式
    }
#ifdef SUPPORT_TVSERVICE
    const sp<TvServerHidlClient> &TvService = getTvService();
    if ( TvService == NULL) {
        XLOGE("%s: %d get tvservice failed!\n", __FUNCTION__,__LINE__);
        return XBH_FAILURE;
    } else {
        //xbh patch
        //if (getHDMIEdidSwitchMode() == 1){
        //    TvService->updateAllEdidVersion(edidType);
        //    XLOGD("%s: %d hdmi edidType = %d\n",__FUNCTION__,__LINE__,edidType);
        //}else{
            port = TvService->getCurrentInputSrc() - SOURCE_YPBPR2;
            if(port < 1/*HDMI_PORT_1*/ || port > 3/*HDMI_PORT_3*/){
                XLOGE("%s: %d port(%d) error!\n", __FUNCTION__,__LINE__, port);
                for (port = 1; port < 4; port++){
                    XLOGD("%s: %d hdmi allport,edidType = %d\n",__func__,__LINE__,edidType);
                    TvService->setHdmiEdidVersion(port,edidType);
                    TvService->saveHdmiEdidVersion(port,edidType);
                }
            }else{
                XLOGD("%s: %d hdmi port = %d,edidType = %d\n",__func__,__LINE__,port,edidType);
                TvService->setHdmiEdidVersion(port,edidType);
                TvService->saveHdmiEdidVersion(port,edidType);
            }
            
        //}
    }
#else
    XLOGD("%s: %d not support tvservice\n",__func__,__LINE__);
#endif
    XLOGD("set setHdmirxEdidType %d \n", type);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getHdmirxEdidType(XBH_S32 *type)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 port;
    XBH_S32 edidType;
#ifdef SUPPORT_TVSERVICE
    const sp<TvServerHidlClient> &TvService = getTvService();
    if ( TvService == NULL) {
        XLOGE("%s: %d get current source is not hdmi source \n", __FUNCTION__,__LINE__);
        return XBH_FAILURE;
    } else {
        //xbh patch
        //if (getHDMIEdidSwitchMode() == 1){
        //    port = 4;//port 4 used for save android setting edid type
        //}else{
            port = TvService->getCurrentInputSrc() - SOURCE_YPBPR2;
            if(port < 1/*HDMI_PORT_1*/ || port > 3/*HDMI_PORT_3*/){
                //XLOGE("%s: %d port(%d) error!\n", __FUNCTION__,__LINE__, port);
                //return XBH_FAILURE;
                port = 1;
            }
        //}
        edidType = TvService->getHdmiEdidVersion(port);
        XLOGD("%s: %d source_input = %d,edidType = %d\n",__func__,__LINE__,port,edidType);
    }
#else
    XLOGD("%s: %d not support tvservice\n",__func__,__LINE__);
#endif
    if(edidType == 0)
    {
        *type = 1;              // EDID 1.4
    }
    else if(edidType == 1)
    {
        *type = 2;              // EDID 2.0
    }
    else if(edidType == 2)
    {
        *type = 4;              // 自动模式
    }
    XLOGD("get HdmirxEdidType %d \n", *type);
    return s32Ret;
}

XBH_S32 XbhAmlogic_311d2::getSocHdmiEdidStatus(XBH_BOOL *enable)
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

XBH_S32 XbhAmlogic_311d2::setSn(const XBH_CHAR* strSn)
{
    return mSystemControlClient->writeUnifyKey("usid", strSn) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 XbhAmlogic_311d2::getSn(XBH_CHAR* strSn)
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

XBH_S32 XbhAmlogic_311d2::setPixelShiftEnable(XBH_BOOL value)
{
    if(!property_get_bool("ro.vendor.xbh.enable.pixel.shift", false))
    {
        XLOGE("%s %d: screen offset not be enable \n",__func__,__LINE__);
        return XBH_FAILURE;
    }

    property_set("persist.vendor.xbh.pixel.shift.status", value == XBH_TRUE ? "true" : "false");
    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::getPixelShiftEnable(XBH_BOOL* value)
{
    if(!property_get_bool("ro.vendor.xbh.enable.pixel.shift", false))
    {
        XLOGE("%s %d: screen offset not be enable \n",__func__,__LINE__);
        return XBH_FAILURE;
    }
    
    *value = property_get_bool("persist.vendor.xbh.pixel.shift.status", false);
    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::setSourceScreenOffset(XBH_S32 x, XBH_S32 y)
{
    if(!property_get_bool("ro.vendor.xbh.enable.pixel.shift", false))
    {
        XLOGE("%s %d: screen offset not be enable \n",__func__,__LINE__);
        return XBH_FAILURE;
    }

    XBH_CHAR offsetXVal[PROPERTY_VALUE_MAX] = {0};
    XBH_CHAR offsetYVal[PROPERTY_VALUE_MAX] = {0};
    sprintf(offsetXVal, "%d", x);
    sprintf(offsetYVal, "%d", y);
    property_set("persist.vendor.xbh.pixel.shift.horizontal", offsetXVal);
    property_set("persist.vendor.xbh.pixel.shift.vertical", offsetYVal);
    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y)
{
    if(!property_get_bool("ro.vendor.xbh.enable.pixel.shift", false))
    {
        XLOGE("%s %d: screen offset not be enable \n",__func__,__LINE__);
        return XBH_FAILURE;
    }

    *x = property_get_int32("persist.vendor.xbh.pixel.shift.horizontal", 0);
    *y = property_get_int32("persist.vendor.xbh.pixel.shift.vertical", 0);
    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::getGoogleKeyName(XBH_CHAR* strPath)
{
    if (!strPath) {
        XLOGE("%s:%d strPath is NULL!", __func__, __LINE__);
        return XBH_FAILURE;
    }
    strPath[0] = '\0';                              /* 先清空缓冲区 */

    /* 只有设备烧录了 Google Attestation Key，才继续后续读取/回退 */
    if (mSystemControlClient &&
        mSystemControlClient->checkProvisionKey(0x42))
    {
        std::string googleKeyName;
        if (mSystemControlClient->readUnifyKey(GOOGLE_KEY_UNIFY_NODE,
                                               googleKeyName) &&
            !googleKeyName.empty())
        {
            /* 读取到具体名称，直接返回 */
            strncpy(strPath, googleKeyName.c_str(), XBH_PATH_MAX - 1);
            strPath[XBH_PATH_MAX - 1] = '\0';
            XLOGI("%s:%d googlekey -> [%s]", __func__, __LINE__, strPath);
        } else {
            /* 已烧录但读不到名称，按逻辑写入 "Success" 以兼容 */
            strcpy(strPath, "Success");
            XLOGI("%s:%d googlekey fallback -> [%s]",
                  __func__, __LINE__, strPath);
        }
        return XBH_SUCCESS;
    }

    /* 未烧录 Google Key，直接返回失败 */
    XLOGW("%s:%d no provision‑key(0x42) on this device", __func__, __LINE__);
    return XBH_FAILURE;

}

XBH_S32 XbhAmlogic_311d2::getGoogleKeyStatus(XBH_BOOL* bEnable)
{
    *bEnable = XBH_FALSE;          // 默认认为无效
    char      keyBuf[GOOGLE_KEY_MAX_LEN] = {0};
    XBH_S32   s32Ret = XBH_FAILURE;

    if (!bEnable) {
        XLOGE("%s:%d  bEnable is NULL!", __func__, __LINE__);
        return XBH_FAILURE;
    }

    /* 1. 先确认 key 是否 provision 在安全 OTP/efuse 中 */
    if (mSystemControlClient &&
        mSystemControlClient->checkProvisionKey(GOOGLE_KEY_PROVISION_ID))
    {
        *bEnable = XBH_TRUE;
        s32Ret   = XBH_SUCCESS;
        XLOGI("%s:%d googlekey -> [true]", __func__, __LINE__);
    } else {
        XLOGW("%s:%d checkProvisionKey(0x%02X) fail，not burn Google Key",
              __func__, __LINE__, GOOGLE_KEY_PROVISION_ID);
    }
    return s32Ret;
}
XBH_S32 XbhAmlogic_311d2::getAttentionKeyStatus(XBH_BOOL* bEnable)
{
    if (!bEnable) {
        XLOGE("%s:%d bEnable is NULL!", __func__, __LINE__);
        return XBH_FAILURE;
    }
    *bEnable = XBH_FALSE;

    /* 直接检查 0x43 Provision Key */
    if (mSystemControlClient &&
        mSystemControlClient->checkProvisionKey(0x43))
    {
        *bEnable = XBH_TRUE;
        XLOGI("%s:%d provisionKey(0x43) FOUND", __func__, __LINE__);
        return XBH_SUCCESS;
    }

    XLOGW("%s:%d provisionKey(0x43) NOT found", __func__, __LINE__);
    return XBH_SUCCESS;
}

XBH_S32 XbhAmlogic_311d2::getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName)
{
    if (!strHdcpFileName) {
        XLOGE("%s:%d strHdcpFileName is NULL!", __func__, __LINE__);
        return XBH_FAILURE;
    }
    strHdcpFileName[0] = '\0';

    const char* unifyNode = nullptr;

    switch (type) {
        case XBH_HDCP_TYPE_E::XBH_HDCP1_4:   unifyNode = "hdcp14name";    break;
        case XBH_HDCP_TYPE_E::XBH_HDCP2_2:   unifyNode = "hdcp22name";    break;
        case XBH_HDCP_TYPE_E::XBH_HDCP_TX_1_4:unifyNode = "hdcptx14name";  break;
        case XBH_HDCP_TYPE_E::XBH_HDCP_TX_2_2:unifyNode = "hdcptx22name";  break;
        default:
            XLOGW("%s:%d unsupported type=%d", __func__, __LINE__, type);
            return XBH_FAILURE;
    }

    std::string hdcpKey;
    bool readOk = (mSystemControlClient &&
                   mSystemControlClient->readUnifyKey(unifyNode, hdcpKey) &&
                   !hdcpKey.empty());

    if (readOk) {
        strncpy(strHdcpFileName, hdcpKey.c_str(), XBH_PATH_MAX - 1);
        strHdcpFileName[XBH_PATH_MAX - 1] = '\0';
        XLOGI("%s:%d [%s] -> %s", __func__, __LINE__, unifyNode, strHdcpFileName);
    } else {
        /* 直接写 Success，占位即可；不再耗时检测是否真的烧录 */
        strcpy(strHdcpFileName, "Success");
        XLOGI("%s:%d [%s] empty, fallback 'Success'", __func__, __LINE__, unifyNode);
    }

    return XBH_SUCCESS;

}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数 ,范围0~2048
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAmlogic_311d2::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XLOGD("%s:%d enter  enColorTemp=%d", __func__, __LINE__, enColorTemp);

    if (data == nullptr) {
        XLOGD("%s:%d data is nullptr", __func__, __LINE__);
        return XBH_FAILURE;
    }

    /* ---------- 1. 枚举 → keyName ---------- */
    const char* keyName = nullptr;
    switch (enColorTemp) {
        case XBH_COLORTEMP_STANDARD: keyName = "standard_color_temp"; break;
        case XBH_COLORTEMP_WARM:     keyName = "warm_color_temp";     break;
        case XBH_COLORTEMP_COLD:     keyName = "cold_color_temp";     break;
        default:
            XLOGD("%s:%d unknown mode=%d", __func__, __LINE__, enColorTemp);
            return XBH_SUCCESS;
    }
    XLOGD("%s:%d keyName=%s", __func__, __LINE__, keyName);

    /* ---------- 2. 手动裁剪函数（无 std::clamp） ---------- */
    auto safe = [](XBH_U32 v) -> int {
        /* 0~2048 合法；大于 2048 则返回 2048 */
        return (v > 2048u) ? 2048 : static_cast<int>(v);
    };

    /* ---------- 3. 生成数组并转成字符串 ---------- */
    XBH_U32 vals[6] = {
        static_cast<XBH_U32>(safe(data->u32RedGain)),
        static_cast<XBH_U32>(safe(data->u32GreenGain)),
        static_cast<XBH_U32>(safe(data->u32BlueGain)),
        static_cast<XBH_U32>(safe(data->u32RedOffset)),
        static_cast<XBH_U32>(safe(data->u32GreenOffset)),
        static_cast<XBH_U32>(safe(data->u32BlueOffset))
    };

    std::ostringstream oss;
    oss << vals[0] << ':' << vals[1] << ':' << vals[2] << ':'
        << vals[3] << ':' << vals[4] << ':' << vals[5];
    std::string valueStr = oss.str();

    XLOGD("%s:%d valueStr=\"%s\"", __func__, __LINE__, valueStr.c_str());

    /* ---------- 4. 写入 unify‑key ---------- */
    bool writeOk = mSystemControlClient->writeUnifyKey(keyName, valueStr);
    XLOGD("%s:%d writeUnifyKey ret=%d", __func__, __LINE__, writeOk);
    return XBH_SUCCESS;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数 ,范围0~2048
 * retval 0:success,-1:failure
*/
XBH_S32 XbhAmlogic_311d2::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XLOGD("%s:%d enter  enColorTemp=%d", __func__, __LINE__, enColorTemp);

    if (data == nullptr) {
        XLOGD("%s:%d data is nullptr", __func__, __LINE__);
        return XBH_FAILURE;
    }

    /* 1. 枚举 ➜ keyName */
    const char* keyName = nullptr;
    switch (enColorTemp) {
        case XBH_COLORTEMP_STANDARD: keyName = "standard_color_temp"; break;
        case XBH_COLORTEMP_WARM:     keyName = "warm_color_temp";     break;
        case XBH_COLORTEMP_COLD:     keyName = "cold_color_temp";     break;
        default:
            XLOGD("%s:%d unknown mode=%d", __func__, __LINE__, enColorTemp);
            return XBH_FAILURE;
    }
    XLOGD("%s:%d keyName=%s", __func__, __LINE__, keyName);

    /* 2. 读取字符串 */
    std::string raw;
    bool readOk = mSystemControlClient->readUnifyKey(keyName, raw);
    XLOGD("%s:%d readUnifyKey ret=%d raw=\"%s\"",
          __func__, __LINE__, readOk, raw.c_str());

    if (readOk && !raw.empty()) {
        /* 3. ":" 分割并用 from_chars 解析成 6 个整数 */
        std::vector<int> vals;
        std::stringstream ss(raw);
        std::string item;

        while (std::getline(ss, item, ':')) {
            long tmp = 1024;            // 默认值
            char* end = nullptr;
            tmp = std::strtol(item.c_str(), &end, 10);

            if (end == item.c_str() || *end != '\0' || tmp < 0 || tmp > 2048)
                tmp = 1024;             // 解析失败或越界

            vals.push_back(static_cast<int>(tmp));
        }

        XLOGD("%s:%d parsed size=%zu  vals=[%s]",
              __func__, __LINE__, vals.size(), raw.c_str());

        if (vals.size() == 6) {
            auto toU32 = [](int v) -> XBH_U32 {
                return static_cast<XBH_U32>(std::clamp(v, 0, 2048));
            };

            data->u32RedGain     = toU32(vals[0]);
            data->u32GreenGain   = toU32(vals[1]);
            data->u32BlueGain    = toU32(vals[2]);
            data->u32RedOffset   = toU32(vals[3]);
            data->u32GreenOffset = toU32(vals[4]);
            data->u32BlueOffset  = toU32(vals[5]);

            XLOGD("%s:%d SUCCESS  Rg=%u Gg=%u Bg=%u  Ro=%u Go=%u Bo=%u",
                  __func__, __LINE__,
                  data->u32RedGain, data->u32GreenGain, data->u32BlueGain,
                  data->u32RedOffset, data->u32GreenOffset, data->u32BlueOffset);
            return XBH_SUCCESS;
        }
    }

    /* 4. 回退默认值 1024 */
    constexpr XBH_U32 DEF_VAL = 1024;
    data->u32RedGain     = DEF_VAL;
    data->u32GreenGain   = DEF_VAL;
    data->u32BlueGain    = DEF_VAL;
    data->u32RedOffset   = DEF_VAL;
    data->u32GreenOffset = DEF_VAL;
    data->u32BlueOffset  = DEF_VAL;

    XLOGD("%s:%d FALLBACK default=1024  return FAILURE", __func__, __LINE__);
    return XBH_SUCCESS;
}




XbhAmlogic_311d2::XbhAmlogic_311d2()
{
    XLOGD(" begin ");
    mGpioFd = open(DEV_GPIO_NAME, O_RDWR);
    if (mGpioFd < 0) 
    {
        XLOGE("-- open device : %s fail --\n", DEV_GPIO_NAME);
    }
    //i2c lock
    for(int i = 0; i < sizeof(mLockIIC)/sizeof(XBH_S32); i ++)
    {
        mLockIIC[i] = -1;
    }

    mSystemControlClient = SystemControlClient::getInstance();

    mRtcId = wall_clock_rtc();

    initSerialPort();

    if (property_get_bool(XBH_CUST_PROPERTY_NOT_INIT, true))
    {
        property_set(XBH_CUST_PROPERTY_NOT_INIT, "false");
        loadProperty();
    }

    XLOGD(" end ");
}


