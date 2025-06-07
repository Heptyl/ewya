#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ax88279"

#include "XbhLog.h"
#include "ax88279.h"
#include "XbhPartitionData.h"
#include "XbhService.h"

ax88279*              ax88279::mInstance = XBH_NULL;
XbhMutex              ax88279::mLock;

XBH_S32 ax88279::syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}


void ax88279::run(const void* arg)
{

}

ax88279::ax88279()
{
    XLOGD(" ax88279 ");
}

ax88279::~ax88279()
{
    XLOGD(" ~ax88279 ");
}

ax88279* ax88279::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new ax88279();
    }
    return mInstance;
}