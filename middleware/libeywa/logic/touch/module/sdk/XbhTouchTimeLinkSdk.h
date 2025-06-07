#ifndef __HI_MW_XBHTIMELINK_SDK_H__
#define __HI_MW_XBHTIMELINK_SDK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"

class XbhTouchTimeLinkSdk: public XbhTouchInterface
{
public:
    static XbhTouchTimeLinkSdk *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    XBH_S32 insmod(const XBH_CHAR *filename, const XBH_CHAR *args);
    XBH_S32 loadModules(XBH_VOID);

private:
    XbhTouchTimeLinkSdk(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchTimeLinkSdk();

    static XbhTouchTimeLinkSdk *mInstance;
    static XbhMutex mLockObject;

    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
};
#endif