LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := K2_daemon.cpp 

SRC_PATH := .
LOCAL_CFLAGS := \
    -DMSOS_TYPE_LINUX \
    -Wint-to-pointer-cast

$(warning "the value of LOCAL_PATH is $(SRC_PATH)")

LOCAL_SHARED_LIBRARIES := libc liblog libutils libcutils

LOCAL_MODULE    := k2_daemon

LOCAL_MODULE_PATH := $(PRODUCT_OUT)/vendor/bin/k2_touch

include $(BUILD_EXECUTABLE)
