#define LOG_TAG "vendor.xbh.hardware.xbhhwmw@1.0"
#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/xbh/hardware/xbhhwmw/1.0/IXbhHwmw.h>

#include <hidl/LegacySupport.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

using vendor::xbh::hardware::xbhhwmw::V1_0::IXbhHwmw;
using android::hardware::defaultPassthroughServiceImplementation;

int main() {
    android::ProcessState::initWithDriver("/dev/vndbinder");
    android::ProcessState::self()->setThreadPoolMaxThreadCount(4);
    android::ProcessState::self()->startThreadPool();
    return defaultPassthroughServiceImplementation<IXbhHwmw>();//passthrough mode
}

