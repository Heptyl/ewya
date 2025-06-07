#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchInterfaceSdk"

#include "XbhLog.h"
#include "cutils/properties.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "XbhTouchInterfaceSdk.h"
//#include "XbhTouchChengdaSdk.h"
//#include "XbhTouchKtcSdk.h"

XbhTouchInterfaceSdk            *XbhTouchInterfaceSdk::mInstance = XBH_NULL;
XbhMutex                        XbhTouchInterfaceSdk::mLockObject;

XBH_S32 XbhTouchInterfaceSdk::Init(XBH_VOID)
{
    char tpProduct[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.xbh.touch.product", tpProduct, "Common");
    property_set("vendor.xbh.touch.type.sdk", "1");
/*
    if(strcmp(tpVendor, "ChengDa") == 0)
    {
        XbhTouchChengdaSdk::getInstance();
    } else if (strcmp(tpVendor, "Ktc") == 0) {
        XbhTouchKtcSdk::getInstance();
    }
*/
    return XBH_SUCCESS;
}

XbhTouchInterfaceSdk::XbhTouchInterfaceSdk()
{
    XLOGD(" begin ");
    Init();
    XLOGD(" end ");
}

XbhTouchInterfaceSdk::~XbhTouchInterfaceSdk()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchControlInterface *XbhTouchInterfaceSdk::getInstance()
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchInterfaceSdk();
    }
    return mInstance;
}
