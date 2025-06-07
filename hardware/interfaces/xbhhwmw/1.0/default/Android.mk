LOCAL_PATH := $(call my-dir)

############ vendor.xbh.hardware.xbhhwmw@1.0-impl ##########
include $(CLEAR_VARS)
LOCAL_MODULE := vendor.xbh.hardware.xbhhwmw@1.0-impl
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    XbhHwmw.cpp \

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidltransport \
    liblog \
    libutils \
    libbase \
    libhardware \
    libbinder \
    libhwbinder \
    libsync \
    vendor.xbh.hardware.xbhhwmw@1.0 \
    libeywa

LOCAL_C_INCLUDES := vendor/xbh/middleware/libeywa/include
LOCAL_C_INCLUDES += vendor/xbh/middleware/libeywa/common/include
LOCAL_C_INCLUDES += vendor/xbh/middleware/libeywa/platform/include

include $(BUILD_SHARED_LIBRARY)


############ vendor.xbh.hardware.xbhhwmw@1.0-service ##########
include $(CLEAR_VARS)
LOCAL_MODULE := vendor.xbh.hardware.xbhhwmw@1.0-service
LOCAL_INIT_RC := vendor.xbh.hardware.xbhhwmw@1.0-service.rc
LOCAL_VINTF_FRAGMENTS := vendor.xbh.hardware.xbhhwmw@1.0-service.xml
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
    service.cpp \

LOCAL_SHARED_LIBRARIES := \
    libhidlbase \
    libhidltransport \
    liblog \
    libutils \
    libbase \
    libhardware \
    libbinder \
    libhwbinder \
    libsync \
    vendor.xbh.hardware.xbhhwmw@1.0

include $(BUILD_EXECUTABLE)

