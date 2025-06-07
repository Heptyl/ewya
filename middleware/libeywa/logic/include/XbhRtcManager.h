#ifndef __XBH_RTC_MANAGER_H__
#define __XBH_RTC_MANAGER_H__

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMutex.h"

class XbhRtcManager
{
public:
    static XbhRtcManager* getInstance();
    XBH_S32 setTime(XBH_U32 year, XBH_U32 month, XBH_U32 date, XBH_U32 week, XBH_U32 hour, XBH_U32 minute, XBH_U32 sec);
    XBH_S32 setAlarmTime(XBH_U32 date, XBH_U32 week, XBH_U32 hour, XBH_U32 minute, XBH_BOOL bEnable);
    XBH_S32 getTime(XBH_U32* year, XBH_U32* month, XBH_U32* date, XBH_U32* week, XBH_U32* hour, XBH_U32* minute, XBH_U32* sec);

    ~XbhRtcManager();

private:
    XbhRtcManager();
    static XbhRtcManager *mInstance;
    static XbhMutex mLock;
};

#endif //__XBH_RTC_MANAGER_H__

