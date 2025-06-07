#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchKtcSdk_Common"

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

#include "XbhTouchKtcSdk_Common.h"
#include "XbhTouchInterfaceAuto.h"

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

XbhTouchKtcSdk_Common*             XbhTouchKtcSdk_Common::mInstance = XBH_NULL;
XbhMutex                           XbhTouchKtcSdk_Common::mLockObject;

int DeviceNum = 0;
deviceInfolist Devices[256] = {{0,0}};

MatchTouchNode MatchTouchList[]
{
/*  USB1 VID        USB1 PID        USB2 VID       USB2 PID         Touch Ctrl              Match1      Match2             ko                  NAME      */
    {0x314c,        0x4c52,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          1,         "skg_t52_touch.ko",       "SKG-T52"},
    {0x314c,        0x4c60,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t80_touch.ko",       "SKG-T80"},
    {0x314c,        0x4c90,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          1,         "skg_t90_touch.ko",       "SKG-T90"},
    {0x314c,        0x4c60,         0x0b8c,        0x320b,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t85_touch.ko",       "SKG-T85"},
    {0x314c,        0x4c60,         0x0b8c,        0x320f,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t85_touch.ko",       "SKG-T85"},
    {0x314c,        0x4c60,         0x0b8c,        0x3211,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t89_touch.ko",       "SKG-T89"},
    {0x314c,        0x4c60,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          1,         "skg_tA0_touch.ko",       "SKG-TA0"},
};

XBH_S32 XbhTouchKtcSdk_Common::insmod(const XBH_CHAR *filename, const XBH_CHAR *args)
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

XBH_S32 XbhTouchKtcSdk_Common::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    return XBH_SUCCESS;
}

XbhTouchKtcSdk_Common::XbhTouchKtcSdk_Common(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    property_set("persist.vendor.xbh.touch.product","Ktc");
    memset(mKoName, 0, sizeof(mKoName));
    memset(mTpName, 0, sizeof(mTpName));
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

XBH_S32 XbhTouchKtcSdk_Common::getAllUsbDevices()
{
    DIR *dir;
    struct dirent *next;
    FILE *fp = NULL;
    XBH_U32 count = 0;

    dir = opendir(USB_DIR);
    if (!dir)
    {
        return XBH_FAILURE;
    }

    while ((next = readdir(dir)) != NULL)
    {
        char line[256];
        char uevent_file[256] = {0};
        sprintf(uevent_file, "%s/%s/uevent", USB_DIR, next->d_name);

        fp = fopen(uevent_file, "r");
        if (NULL == fp)
        {
            continue;
        }
        while (fgets(line, sizeof(line), fp))
        {
            char *pos = NULL;
            XBH_S32 product_vid, product_pid, product_bcdev;
            char temp[10] = {0};
            pos = strstr(line, "PRODUCT=");
            if (NULL != pos)
            {
                if (sscanf(pos + 8, "%x/%x/%x", &product_vid, &product_pid, &product_bcdev) <= 0)
                {
                    continue;
                }
                XLOGV("get vid = 0x%02x pid = 0x%02x ", product_vid, product_pid);
                Devices[count].device_vid = product_vid;
                Devices[count].device_pid = product_pid;
                count ++;
            }
        }
        fclose(fp);
    }
    DeviceNum = count;
    closedir(dir);
    return XBH_SUCCESS;
}

XBH_S32 XbhTouchKtcSdk_Common::getTouchPanelConnect(XBH_BOOL* enable)
{
    XBH_S32 VID = 0xFFFF, PID = 0xFFFF;
    if(getAllUsbDevices() == XBH_SUCCESS)
    {
        // 遍历所有 USB 设备
        for (int i = 0; i < DeviceNum; i++)
        {
            VID = Devices[i].device_vid;
            PID = Devices[i].device_pid;

            // 遍历 MatchTouchNode 数组，检查是否匹配
            for (int j = 0; j < (sizeof(MatchTouchList) / sizeof(MatchTouchNode)); j++)
            {
                // 如果匹配到第一路USB（usb1_vid 和 usb1_pid）
                if ((VID == MatchTouchList[j].usb1_vid) && (PID == MatchTouchList[j].usb1_pid))
                {
                    MatchTouchList[j].match1++;  // 标记为已匹配
                    if (MatchTouchList[j].match2 != 0)
                    {
                        *enable = 1;  // 匹配成功，启用触摸面板
                        return XBH_SUCCESS;
                    }
                }
                // 如果匹配到第二路USB（usb2_vid 和 usb2_pid）
                else if ((VID == MatchTouchList[j].usb2_vid) && (PID == MatchTouchList[j].usb2_pid))
                {
                    MatchTouchList[j].match2++;  // 标记为已匹配
                    if (MatchTouchList[j].match1 != 0)
                    {
                        *enable = 1;  // 匹配成功，启用触摸面板
                        return XBH_SUCCESS;
                    }
                }
            }
        }
    }

    // 如果没有匹配到，返回 XBH_FAILURE
    return XBH_FAILURE;
}


XbhTouchKtcSdk_Common::~XbhTouchKtcSdk_Common()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchKtcSdk_Common *XbhTouchKtcSdk_Common::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchKtcSdk_Common(ko, tpName);
    }
    return mInstance;
}