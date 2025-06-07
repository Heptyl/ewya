#ifndef __HI_MW_XBHHuaxin_SDK_COMMON_H__
#define __HI_MW_XBHHuaxin_SDK_COMMON_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"

class XbhTouchHuaxinSdk_Common: public XbhTouchInterface
{
public:
    static XbhTouchHuaxinSdk_Common *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    XBH_S32 insmod(const XBH_CHAR *filename, const XBH_CHAR *args);
    XBH_S32 loadModules(XBH_VOID);
    /**
     * 获取触摸框驱动版本
     */
    XBH_S32 getTpDriverVersion(XBH_CHAR* version);

    /**
     * 获取触摸框固件版本
     */
    XBH_S32 getTpFirmwareVersion(XBH_CHAR* version);

private:
    XbhTouchHuaxinSdk_Common(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchHuaxinSdk_Common();

    static XbhTouchHuaxinSdk_Common *mInstance;
    static XbhMutex mLockObject;

    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
};
#endif

