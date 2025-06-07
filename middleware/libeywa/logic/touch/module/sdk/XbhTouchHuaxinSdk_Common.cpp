#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchHuaxinSdk_Common"

#include "XbhLog.h"
#include "cutils/properties.h"
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "XbhSysOpt.h"


#include "XbhTouchHuaxinSdk_Common.h"
#include "XbhTouchInterfaceAuto.h"

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/hidraw.h>


XbhTouchHuaxinSdk_Common*         XbhTouchHuaxinSdk_Common::mInstance = XBH_NULL;
XbhMutex                           XbhTouchHuaxinSdk_Common::mLockObject;

XBH_S32 XbhTouchHuaxinSdk_Common::insmod(const XBH_CHAR *filename, const XBH_CHAR *args)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_S32 fd;
    fd = TEMP_FAILURE_RETRY(open(filename, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
    if (fd < 0) 
    {
        XLOGE("Failed to open %s", filename);
        return XBH_FAILURE;
    }
    ret = syscall(__NR_finit_module, fd, args, 0);
    if (ret < 0) 
    {
        XLOGE("finit_module return: %d", ret);
    }
    close(fd);
    return ret;
}

XBH_S32 XbhTouchHuaxinSdk_Common::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    return XBH_SUCCESS;
}

XBH_S32 XbhTouchHuaxinSdk_Common::getTpDriverVersion(XBH_CHAR* version)
{
    XBH_S32 ret = XBH_FAILURE;
    FILE* file = fopen("/sys/bus/usb/drivers/touchX/drvinfo", "r");
    char line[128];  // 用于读取每行的缓冲区
    const char* prefix = "DRIVER_VERSION:";  // 要查找的前缀
    // 初始化 version 为一个空字符串
    version[0] = '\0';

    if (file != nullptr)
    {
        // 逐行读取文件内容
        while (fgets(line, sizeof(line), file))
        {
            // 查找包含 "DRIVER_VERSION" 的行
            if (strstr(line, prefix))
            {
                // 查找 "DRIVER_VERSION:" 后面的版本号
                char* pos = strstr(line, prefix);
                if (pos != nullptr)
                {
                    // 跳过 "DRIVER_VERSION:" 和空格（如果有）
                    pos += strlen(prefix);  // 跳过 "DRIVER_VERSION:" 字符串

                    // 跳过可能的空格
                    while (*pos == ' ' || *pos == '\t')
                    {
                        pos++;
                    }

                    // 将版本号从 "V" 开始的部分复制到 version
                    strncpy(version, pos, 127);  // 限制最大字符数
                    version[127] = '\0';  // 确保字符串结束

                    // 去除可能存在的换行符
                    size_t len = strlen(version);
                    if (len > 0 && version[len - 1] == '\n')
                    {
                        version[len - 1] = '\0';  // 删除换行符
                    }

                    ret = XBH_SUCCESS;
                }
                break;  // 找到版本号后退出循环
            }
        }
        fclose(file);  // 关闭文件
    } else
    {
        // 如果文件打开失败，输出错误信息
        XLOGE("getTpDriverVersion fail");
    }

    return ret;
}



XBH_S32 XbhTouchHuaxinSdk_Common::getTpFirmwareVersion(XBH_CHAR* version)
{
    XBH_S32 ret = XBH_FAILURE;
    FILE* file = fopen("/sys/bus/usb/drivers/touchX/drvinfo", "r");
    char line[128];  // 用于读取每行的缓冲区
    const char* prefix = "DEVICE_VERSION:";  // 要查找的前缀
    // 初始化 version 为一个空字符串
    version[0] = '\0';

    if (file != nullptr)
    {
        // 逐行读取文件内容
        while (fgets(line, sizeof(line), file))
        {
            // 查找包含 "DEVICE_VERSION" 的行
            if (strstr(line, prefix))
            {
                // 查找 "DEVICE_VERSION:" 后面的版本号
                char* pos = strstr(line, prefix);
                if (pos != nullptr)
                {
                    // 跳过 "DEVICE_VERSION:" 和空格（如果有）
                    pos += strlen(prefix);  // 跳过 "DEVICE_VERSION:" 字符串

                    // 跳过可能的空格
                    while (*pos == ' ' || *pos == '\t')
                    {
                        pos++;
                    }

                    // 将版本号从 "V" 开始的部分复制到 version
                    strncpy(version, pos, 127);  // 限制最大字符数
                    version[127] = '\0';  // 确保字符串结束

                    // 去除可能存在的换行符
                    size_t len = strlen(version);
                    if (len > 0 && version[len - 1] == '\n')
                    {
                        version[len - 1] = '\0';  // 删除换行符
                    }

                    ret = XBH_SUCCESS;
                }
                break;  // 找到版本号后退出循环
            }
        }
        fclose(file);  // 关闭文件
    } else
    {
        // 如果文件打开失败，输出错误信息
        XLOGE("getTpFirmwareVersion fail");
    }

    return ret;
}

XbhTouchHuaxinSdk_Common::XbhTouchHuaxinSdk_Common(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mKoName));
    if(ko != XBH_NULL)
    {
        memcpy(mKoName, ko, strlen(ko));
    }
    else
    {
        memcpy(mKoName, "", strlen(""));
    }
    if(tpName != XBH_NULL)
    {
        memcpy(mTpName, tpName, strlen(tpName));
    }
    else
    {
        memcpy(mTpName, "", strlen(""));
    }

    if(strcmp(mKoName, "") != 0)
    {
        loadModules();
    }
}

XbhTouchHuaxinSdk_Common::~XbhTouchHuaxinSdk_Common()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchHuaxinSdk_Common *XbhTouchHuaxinSdk_Common::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchHuaxinSdk_Common(ko, tpName);
    }
    return mInstance;
}
