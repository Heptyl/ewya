# vendor xbh
XBH_HAL_PATH := vendor/xbh

# selinux
include $(XBH_HAL_PATH)/system/sepolicy/BoardSepolicy.mk

# xbh hidl
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += \
    $(XBH_HAL_PATH)/hardware/vendor.xbh.hardware.xbhhwmw@1.0-service.xml

# xbh hidl soong namespaces
PRODUCT_SOONG_NAMESPACES += \
    vendor/xbh

# xbh hidl
PRODUCT_PACKAGES += \
    vendor.xbh.hardware.xbhhwmw@1.0-impl \
    vendor.xbh.hardware.xbhhwmw@1.0-service \
    libeywa \
    libxbhapijni

# apps
PRODUCT_PACKAGES += \
    xbhfunc \
    xbhshell

# sample code
PRODUCT_PACKAGES += \
    mwt

include $(XBH_HAL_PATH)/thirdpart/third_part.mk
