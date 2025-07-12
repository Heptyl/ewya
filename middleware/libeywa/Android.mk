LOCAL_PATH := $(call my-dir)

## board define
XMM8195U := 1
XMM8195G := 2
XMM8195U_V2 := 3
MY_BOARD := ${XMM8195U_V2}

include $(CLEAR_VARS)

BUILD_DATE := $(shell date '+%Y-%m-%d-%H:%M:%S')
$(info ======================================= Building on $(BUILD_DATE))
LOCAL_CFLAGS += -DBUILD_DATE="\"$(BUILD_DATE)\""

## board marco define
LOCAL_CFLAGS += -DXMM8195U=${XMM8195U}
LOCAL_CFLAGS += -DXMM8195G=${XMM8195G}
LOCAL_CFLAGS += -DXMM8195U_V2=${XMM8195U_V2}

LOCAL_MODULE := libeywa
LOCAL_MODULE_TAGS := optional
LOCAL_32_BIT_ONLY := false
LOCAL_PROPRIETARY_MODULE := true

LOCAL_CFLAGS += -O0 -Wall -Wno-unused-parameter
LOCAL_CFLAGS += -DMTK_8195
LOCAL_CFLAGS += -DXBHBUILD_HW
LOCAL_CFLAGS += -DTYPE_BOARD=${MY_BOARD}
#采用RTC原生的实现
LOCAL_CFLAGS += -DXBH_USE_NATIVE_RTC

#使用hal层的接口来检测信号状态，如果不定义就是使用JAVA/kotlin API来实现
LOCAL_CFLAGS += -DXBH_USE_HAL_API_TO_CHECK_SIGNAL_STATUS

#引入logic里面参与编译的代码
include $(LOCAL_PATH)/common.mk

eywa_src_list += $(wildcard $(LOCAL_PATH)/platform/mtk/mtk_8195/*.cpp)

ifeq (${MY_BOARD}, ${XMM8195U})
$(info ======================================= Building on U board)
eywa_src_list += $(wildcard $(LOCAL_PATH)/platform/mtk/mtk_8195/board/XbhXMM8195_U.cpp)
endif
ifeq (${MY_BOARD}, ${XMM8195G})
$(info ======================================= Building on G board)
eywa_src_list += $(wildcard $(LOCAL_PATH)/platform/mtk/mtk_8195/board/XbhXMM8195_G.cpp)
endif
ifeq (${MY_BOARD}, ${XMM8195U_V2})
$(info ======================================= Building on U_V2 board)
eywa_src_list += $(wildcard $(LOCAL_PATH)/platform/mtk/mtk_8195/board/XbhXMM8195_U_V2.cpp)
endif
LOCAL_SRC_FILES := $(eywa_src_list:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/common/include \
    $(LOCAL_PATH)/common/db \
    $(LOCAL_PATH)/common/db/config \
    $(LOCAL_PATH)/common/db/config/iniparser \
    $(LOCAL_PATH)/common/db/config/iniprovider \
    $(LOCAL_PATH)/common/db/securec \
    $(LOCAL_PATH)/platform/include \
    $(LOCAL_PATH)/platform/fboard \
    $(LOCAL_PATH)/platform/mtk/mtk_8195 \
    $(LOCAL_PATH)/platform/mtk/mtk_8195/board \
    $(LOCAL_PATH)/../../../mediatek/proprietary/external/libnvram/nvram_daemon \
    $(LOCAL_PATH)/../../../mediatek/proprietary/external/libnvram/mt8195 \
    $(LOCAL_PATH)/../../../mediatek/proprietary/external/libnvram/nvram_hidl/1.1 \
    $(LOCAL_PATH)/../../../mediatek/proprietary/external/nvram/libnvram \
    $(LOCAL_PATH)/../../../mediatek/proprietary/external/nvram/libfile_op \
    $(LOCAL_PATH)/../../../mediatek/proprietary/custom/aiv8195p1_64_bsp/cgen/cfgfileinc \
    $(LOCAL_PATH)/../../../mediatek/proprietary/custom/common/cgen/inc \
    $(LOCAL_PATH)/../../../mediatek/proprietary/custom/aiv8195p1_64_bsp/cgen/inc \
    $(LOCAL_PATH)/../../../mediatek/proprietary/external/libnvram_ext \
    vendor/lango/preinstall/libxbhdrmkey/include \
    hardware/libhardware/include \
    system/core/libsystem/include \
    system/core/libutils/include


LOGIC_DIRS := $(shell find $(LOCAL_PATH)/logic -type d)
LOCAL_C_INCLUDES += $(LOGIC_DIRS)

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
    libxbhdrmkey \
    libnvram \
    vendor.mediatek.hardware.nvram@1.0 \
    vendor.xbh.hardware.xbhhwmw@1.0 \
    libcutils \
    libms9282 \
    libusb

#ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_MODULE)
include $(BUILD_SHARED_LIBRARY)

include vendor/xbh/middleware/libeywa/logic/vga2hdmi/module/ms9282/libms9282/Android.mk
