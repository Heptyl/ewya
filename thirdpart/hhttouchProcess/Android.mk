LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libhhttouchprocess
LOCAL_PROPRIETARY_MODULE := true
#ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_MODULE)
#LOCAL_MODULE_TAGS := optional
#LOCAL_CFLAGS += -O0 -Wall -Wno-unused-parameter

LOCAL_SRC_FILES := \
    TouchProcess.cpp \
    SerialProcess.cpp \
    IniFile.cpp \
    iniparser.cpp \
    dictionary.cpp \
    hht_src/hht_touch_device.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/hht_src 

LOCAL_HEADER_LIBRARIES := jni_headers

LOCAL_SHARED_LIBRARIES := libc liblog libutils libcutils

# LOCAL_LDFLAGS += $(LOCAL_PATH)/libs/libusb1.0.a

include $(BUILD_SHARED_LIBRARY)
