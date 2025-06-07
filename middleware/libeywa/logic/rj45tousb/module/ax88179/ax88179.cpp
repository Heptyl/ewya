#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ax88179"

#include "XbhLog.h"
#include "ax88179.h"
#include "XbhPartitionData.h"
#include "XbhService.h"

ax88179*              ax88179::mInstance = XBH_NULL;
XbhMutex                     ax88179::mLock;

XBH_S32 ax88179::syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char strBuildinMacAddr[XBH_CUSDATA_MACADDRESS_LEN] = {0};
    char strCurMacAddr[XBH_CUSDATA_MACADDRESS_LEN] = {0};

    s32Ret = XbhService::getModuleInterface()->getBuildinEthMacAddress(strPath, strBuildinMacAddr);
    XLOGD("syncBuildinEthMacAddress strBuildinMacAddr %s \n", strBuildinMacAddr);
    if (s32Ret == XBH_SUCCESS)
    {
        XbhPartitionData::getInstance()->getMacAddress(strCurMacAddr);
        XLOGD("syncBuildinEthMacAddress strCurMacAddr %s \n", strCurMacAddr);
        if (strcasecmp(strBuildinMacAddr, strCurMacAddr) != 0)
        {
            property_set("vendor.xbh.usb.devicenode", devNode);
            property_set("vendor.xbh.usb.macaddr", strCurMacAddr);
            property_set("vendor.xbh.usb.devicetype", devType);
            XbhMWThread::run();
        }
    }
    return s32Ret;
}


void ax88179::run(const void* arg)
{
    XLOGD("begin burn factory mac to eth mac\n");
    property_set("vendor.xbh.burn.ax88179.macaddr", "true");
    usleep(1000*1000);

    char value[PROP_VALUE_MAX];
    property_get("vendor.mtk.ax88179.cmd.state", value, "unknown");
    XLOGD("ax88179 state value %s \n", value);
    XBH_U32 times = 0;
    while (strcmp(value, "running") == 0 && times < 20)
    {
        usleep(1000*1000);
        property_get("vendor.mtk.ax88179.cmd.state", value, "unknown");
        XLOGD("ax88179 state value %s \n", value);
        times++;
    }

    if (strcmp(value, "success") == 0) {
        XLOGD("ax88179 begin reload\n");
        property_set("vendor.xbh.reload.ax88179", "true");
    } else {
        XLOGD("ax88179 no need to reload\n");
    }
}

ax88179::ax88179()
{
    XLOGD(" ax88179   ");
}

ax88179::~ax88179()
{
    XLOGD(" ~ax88179   ");
}

ax88179* ax88179::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new ax88179();
    }
    return mInstance;
}
