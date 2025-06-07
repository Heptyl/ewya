#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchTimeLinkSdk"

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

#include "XbhTouchTimeLinkSdk.h"
#include "XbhTouchInterfaceAuto.h"

#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>


XbhTouchTimeLinkSdk*         XbhTouchTimeLinkSdk::mInstance = XBH_NULL;
XbhMutex                           XbhTouchTimeLinkSdk::mLockObject;

XBH_S32 XbhTouchTimeLinkSdk::insmod(const XBH_CHAR *filename, const XBH_CHAR *args)
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
    XLOGD("insmod ko return: %d", ret);
    close(fd);
    return ret;
}

XBH_S32 XbhTouchTimeLinkSdk::loadModules(XBH_VOID)
{
    XBH_CHAR path[64];
    strcpy(path, "/vendor/lib/modules/");
    strcat(path, mKoName);
    insmod(path, "");
    insmod("/vendor/lib/modules/tl_multi_dev.ko","");//天英 usb转uart驱动
    return XBH_SUCCESS;
}

XbhTouchTimeLinkSdk::XbhTouchTimeLinkSdk(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    property_set("vendor.xbh.touch.timelink","1"); //当为1时后台启动tltouch算法。
    property_set("persist.vendor.xbh.touch.product","TimeLink");
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

XbhTouchTimeLinkSdk::~XbhTouchTimeLinkSdk()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchTimeLinkSdk *XbhTouchTimeLinkSdk::getInstance(XBH_CHAR *ko, XBH_CHAR *tpName)
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchTimeLinkSdk(ko, tpName);
    }
    return mInstance;
}