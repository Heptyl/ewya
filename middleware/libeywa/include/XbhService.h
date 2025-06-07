#ifndef XBHSERVICE_H
#define XBHSERVICE_H

#include "XbhType.h"
#include "XbhMsgHandler.h"
#include "XbhMutex.h"

#include "XbhPlatformInterface.h"
#include "XbhModuleInterface.h"

#include "binder/Parcel.h"

#include <utils/List.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include <utils/Vector.h>

#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmw.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmwCallback.h>

using namespace android;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmw;
using ::vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmwCallback;

class XbhService : public RefBase, public XbhMsgHandler
{
public:
    XbhService();
    ~XbhService();
    virtual XBH_S32 invoke(const Parcel& request, Parcel *reply);
    XBH_S32 setCallback(const sp<IXbhHwmwCallback> &callback);
    XBH_S32 disconnect(const sp<IXbhHwmwCallback> &callback);
    static XbhModuleInterface* getModuleInterface();
    static XbhPlatformInterface* getPlatformInterface();
    
    static XBH_BOOL mVboToHdmiCreated;
    static XBH_BOOL mCodecCreated;
    static XBH_BOOL mAmpCreated;
    static XBH_BOOL mAudioToI2sCreated;
    static XBH_BOOL mNetSwitchCreated;
    static XBH_BOOL mDp2HdmiCreated;
    static XBH_BOOL mUsbc2HdmiCreated;
    static XBH_BOOL mVga2HdmiCreated;

private:
    static XbhMutex mNotifyLock;
    class XbhServiceImpl;
    XbhServiceImpl *pXbhServiceImpl;
    SortedVector<sp<IXbhHwmwCallback>> mCallback;

    virtual int notify(unsigned int type, const XBH_VOID *data, unsigned int len, const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL) const;
    int CbHwNotify(unsigned int &type, hidl_string &inputStr, hidl_string &inputStrReq, android::Parcel *notifyReply) const;
    XBH_BOOL HandleMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL);
};


#endif //XBHSERVICE_H
