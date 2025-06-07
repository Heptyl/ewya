#ifndef VENDOR_XBH_HARDWARE_XBHHWMW_V1_0_XBHHWMW_H
#define VENDOR_XBH_HARDWARE_XBHHWMW_V1_0_XBHHWMW_H

#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmw.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmwCallback.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/types.h>

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include "XbhService.h"
#include "XbhMutex.h"

#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>

namespace vendor {
namespace xbh {
namespace hardware {
namespace xbhhwmw {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_death_recipient;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

class XbhmwDeathRecipient;

class XbhHwmw : public IXbhHwmw {
public:
    XbhHwmw();
    Return<void> hwInvoke(const hidl_string& request, hwInvoke_cb _hidl_cb) override;
    Return<int32_t> hwRegistCallback(const sp<IXbhHwmwCallback>& callback) override;
    Return<void> onObjectDeath(uint64_t cookie);

private:
    sp<XbhService> mXbhService;
    std::vector<sp<IXbhHwmwCallback>>  mCallbacks;
    XbhMutex                           mNotifyLock;
    sp<XbhmwDeathRecipient>            mDeathRecipient;
};

extern "C" IXbhHwmw* HIDL_FETCH_IXbhHwmw(const char* name);

}  // namespace implementation
}  // namespace V1_0
}  // namespace xbhhwmw
}  // namespace hardware
}  // namespace xbh
}  // namespace vendor

#endif  // VENDOR_XBH_HARDWARE_XBHHWMW_V1_0_XBHHWMW_H
