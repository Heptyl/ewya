#-------------------------------------------------------------------------------
# Copyright (c)2016-2024 Lango Corporation, All Rights Reserved.
#-------------------------------------------------------------------------------
#
# Description:
#   The apps config device.mk file.

# Include all Lango config
-include vendor/lango/product/cfg.mak

ifeq ($(ENABLE_HHT_LIB_TOUCH_PROCESS), y)
PRODUCT_PACKAGES += \
    libhhttouchprocess
endif

PRODUCT_PACKAGES += \
    k2_daemon

PRODUCT_PACKAGES += \
    libzyuarttouch \
    libxbhuarttouch \
    libycluarttouch \
    libistguarttouch \
    libtimelinkh3touch
