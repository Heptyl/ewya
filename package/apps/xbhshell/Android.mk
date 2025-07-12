LOCAL_PATH :=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := XbhShell.cpp \
                   XbhShell_fasync.cpp \
                   XbhShell_uinput_ir.cpp \
                   XbhShell_uinput_keypad.cpp


SRC_PATH := .
LOCAL_CFLAGS := \
    -DMSOS_TYPE_LINUX \
    -Wint-to-pointer-cast

$(warning "the value of LOCAL_PATH is $(SRC_PATH)")

LOCAL_SHARED_LIBRARIES := \
                        liblog \
                        libcutils \
                        libandroid \
                        libandroid_runtime \

LOCAL_MODULE    := xbhshell

include $(BUILD_EXECUTABLE)
