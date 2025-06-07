#ifndef __IXBHMANAGER_H__
#define __IXBHMANAGER_H__

#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <utils/Mutex.h>
#include <hwbinder/Parcel.h>

namespace android {

class IXbhListener : virtual public RefBase {
public:
    virtual void notify(unsigned int type, const void *data, unsigned int len, const void *prev) = 0;
};

};  // namespace android
#endif
