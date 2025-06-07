#define LOG_TAG "vendor.xbh.hardware.xbhhwmw@1.0"
#include <log/log.h>
#include "XbhHwmw.h"
#include <hwbinder/Parcel.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <inttypes.h>
#include <dlfcn.h>

#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>

namespace vendor {
namespace xbh {
namespace hardware {
namespace xbhhwmw {
namespace V1_0 {
namespace implementation {

class XbhmwDeathRecipient : public hidl_death_recipient
{
public:
    XbhmwDeathRecipient(const sp<XbhHwmw> hTvmw) : mHwtvmw(hTvmw) {}
    ~XbhmwDeathRecipient() {}

    virtual void serviceDied(uint64_t cookie, const wp<::android::hidl::base::V1_0::IBase>& )
    {
       mHwtvmw->onObjectDeath(cookie);
    }
private:
    sp<XbhHwmw> mHwtvmw;
};


XbhHwmw::XbhHwmw():mDeathRecipient(new XbhmwDeathRecipient(this))
{
    if (mXbhService == nullptr)
    {
        mXbhService = new XbhService();
    }
}

// Methods from ::vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmw follow.
Return<void> XbhHwmw::hwInvoke(const hidl_string& request, hwInvoke_cb _hidl_cb) {
    //ALOGD("XbhHwmw::hwInvoke()");

    const char* data = request.c_str();
    int length = request.size();

    android::Parcel parcelReq;
    android::Parcel parcelReply;

    parcelReq.setDataSize(length);
    parcelReq.setDataPosition(0);
    parcelReq.write(data, length);
    parcelReq.setDataPosition(0);

    if (mXbhService != nullptr)
    {
        mXbhService->invoke(parcelReq, &parcelReply);
    }
    else
    {
        ALOGE("XbhHwmw mXbhService is null");
    }

    hidl_string inputStr((char *)parcelReply.data(), parcelReply.dataSize());
    _hidl_cb(inputStr);
    return Void();

}

Return<void> XbhHwmw::onObjectDeath(uint64_t cookie)
{
    XbhMutex::Autolock autoLock(mNotifyLock);

    uintptr_t cookiePtr;
    cookiePtr = static_cast<uintptr_t>(cookie);
    IXbhHwmwCallback *cb = reinterpret_cast<IXbhHwmwCallback*>(cookiePtr);
    std::vector<sp<IXbhHwmwCallback>>::iterator itr =  mCallbacks.begin();
    while (itr != mCallbacks.end())
    {
        if (*itr == cb)
        {
            ALOGD("delete callback");
            itr = mCallbacks.erase(itr);
            if(mXbhService != NULL)
            {
                mXbhService->disconnect(cb);
            }
        }
        else
        {
            itr++;
        }
    }

    ALOGD("after delete callback count = %d", mCallbacks.size());
    return Void();
}


Return<int32_t> XbhHwmw::hwRegistCallback(const sp<IXbhHwmwCallback>& callback)
{
    ALOGD("registCallback, line:%d", __LINE__);

    XbhMutex::Autolock autoLock(mNotifyLock);

    uintptr_t cookiePtr;
    uint64_t cookie;
    cookiePtr = (uintptr_t)(callback.get());
    cookie = static_cast<uint64_t>(cookiePtr);
    if (!callback->linkToDeath(mDeathRecipient, cookie)) {
        ALOGE("setCallback Failed to register death notification");
        return -1;
    }

    mCallbacks.push_back(callback);

    if (mXbhService != NULL) {
        mXbhService->setCallback(callback);
    }

    return 0;
}



// Methods from ::android::hidl::base::V1_0::IBase follow.

IXbhHwmw* HIDL_FETCH_IXbhHwmw(const char* /* name */) {
    return new XbhHwmw();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace xbhhwmw
}  // namespace hardware
}  // namespace xbh
}  // namespace vendor
