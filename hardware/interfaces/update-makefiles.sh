#!/bin/bash

source $ANDROID_BUILD_TOP/system/tools/hidl/update-makefiles-helper.sh

do_makefiles_update \
  "vendor.xbh.hardware:vendor/xbh/hardware/interfaces" \
  "android.hardware:hardware/interfaces" \
  "android.hidl:system/libhidl/transport"

#hidl-gen -L hash -r vendor.xbh.hardware:vendor/xbh/hardware/interfaces -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport vendor.xbh.hardware.xbhhwmw@1.0 ::type.hal>> vendor/xbh/hardware/interfaces/current.txt
#hidl-gen -L hash -r vendor.xbh.hardware:vendor/xbh/hardware/interfaces -r android.hardware:hardware/interfaces -r android.hidl:system/libhidl/transport vendor.xbh.hardware.xbhhwmw@1.0 >> vendor/xbh/hardware/interfaces/current.txt