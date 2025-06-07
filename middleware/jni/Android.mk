LOCAL_PATH := $(call my-dir)

# libxbhapijni
include $(CLEAR_VARS)
LOCAL_MODULE := libxbhapijni
LOCAL_MODULE_TAGS := optional
LOCAL_SYSTEM_EXT_MODULE := true
LOCAL_SRC_FILES := $(call all-cpp-files-under)

LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-variable

LOCAL_SHARED_LIBRARIES := \
    libbase \
    liblog \
    libdl \
    libutils \
    libhardware \
    libhidlbase \
    libhidltransport \
    libbinder \
    libhwbinder \
    vendor.xbh.hardware.xbhhwmw@1.0 \
    libandroid \
    libandroid_runtime

LOCAL_C_INCLUDES := frameworks/base/core/jni \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../libeywa/include

include $(BUILD_SHARED_LIBRARY)
