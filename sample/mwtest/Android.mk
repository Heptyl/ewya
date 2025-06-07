LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := mwt
LOCAL_PROPRIETARY_MODULE := true
LOCAL_SRC_FILES := \
    mwt.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libhidlbase \
    libhwbinder \
    vendor.xbh.hardware.xbhhwmw@1.0

include $(BUILD_EXECUTABLE)

