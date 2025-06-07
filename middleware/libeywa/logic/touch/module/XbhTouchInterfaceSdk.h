#ifndef __HI_MW_XBHTOUCHINTERFACE_SDK_H__
#define __HI_MW_XBHTOUCHINTERFACE_SDK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchControlInterface.h"

class XbhTouchInterfaceSdk: public XbhTouchControlInterface
{
public:
    static XbhTouchControlInterface *getInstance();

private:
    XBH_S32 Init(XBH_VOID);
    XbhTouchInterfaceSdk();
    ~XbhTouchInterfaceSdk();
    
    static XbhTouchInterfaceSdk *mInstance;
    static XbhMutex mLockObject;
};
#endif
