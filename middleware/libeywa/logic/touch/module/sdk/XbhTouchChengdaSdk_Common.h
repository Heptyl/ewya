#ifndef __HI_MW_XBHCHENGDA_SDK_COMMON_H__
#define __HI_MW_XBHCHENGDA_SDK_COMMON_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"

class XbhTouchChengdaSdk_Common: public XbhTouchInterface
{
public:
    static XbhTouchChengdaSdk_Common *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    XBH_S32 insmod(const XBH_CHAR *filename, const XBH_CHAR *args);
    XBH_S32 loadModules(XBH_VOID);

private:
    XbhTouchChengdaSdk_Common(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchChengdaSdk_Common();

    static XbhTouchChengdaSdk_Common *mInstance;
    static XbhMutex mLockObject;
    
    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
};
#endif

