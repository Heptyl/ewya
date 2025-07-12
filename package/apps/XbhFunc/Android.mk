# Copyright 2008 The Android Open Source Project
#
LOCAL_PATH:= $(call my-dir)

#include $(CLEAR_VARS)
#LOCAL_MODULE := xbhapitest
#LOCAL_SRC_FILES := $(call all-subdir-java-files)
#LOCAL_STATIC_JAVA_LIBRARIES := XbhApi
#include $(BUILD_JAVA_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := xbhfunc
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_SRC_FILES := xbhfunc
include $(BUILD_PREBUILT)

#include $(CLEAR_VARS)
#LOCAL_MODULE := xbhfunc_new
#LOCAL_MODULE_CLASS := EXECUTABLES
#LOCAL_SRC_FILES := xbhfunc_new
#@include $(BUILD_PREBUILT)
