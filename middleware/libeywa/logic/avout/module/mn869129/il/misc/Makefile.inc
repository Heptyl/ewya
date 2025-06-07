MISC_SRCS  =
MISC_HDRS  = ./misc/misc.h
if TARGET_HV_SC1H21BJ2
MISC_SRCS += ./misc/miscvborx.c ./misc/mischdmitx.c ./misc/miscvbotx.c ./misc/misclvdstx.c
MISC_HDRS += ./misc/miscvborx.h ./misc/mischdmitx.h ./misc/miscvbotx.h ./misc/misclvdstx.h
endif
if TARGET_HV_SC1H22BJ2
MISC_SRCS += ./misc/miscvborx.c ./misc/mischdmitx.c ./misc/miscvbotx.c ./misc/misclvdstx.c
MISC_HDRS += ./misc/miscvborx.h ./misc/mischdmitx.h ./misc/miscvbotx.h ./misc/misclvdstx.h
endif
if TARGET_HV_SC1H23BJ2
MISC_SRCS += ./misc/miscvborx.c ./misc/mischdmitx.c ./misc/miscvbotx.c ./misc/misclvdstx.c
MISC_HDRS += ./misc/miscvborx.h ./misc/mischdmitx.h ./misc/miscvbotx.h ./misc/misclvdstx.h
endif
if TARGET_HV_SC1H24BJ2
MISC_SRCS +=                    ./misc/mischdmitx.c
MISC_HDRS +=                    ./misc/mischdmitx.h
endif
if TARGET_HV_SC1H25BJ2
MISC_SRCS +=
MISC_HDRS +=
endif
if TARGET_HV_SC1H26BJ2
MISC_SRCS +=
MISC_HDRS +=
endif
if TARGET_HV_SC1H27BJ2
MISC_SRCS += ./misc/miscvborx.c ./misc/mischdmitx.c
MISC_HDRS += ./misc/miscvborx.h ./misc/mischdmitx.h
endif
if TARGET_HV_SC1H28BJ2
MISC_SRCS += ./misc/miscvborx.c
MISC_HDRS += ./misc/miscvborx.h
endif
if TARGET_HV_SC1H29BJ2
MISC_SRCS += ./misc/miscvborx.c
MISC_HDRS += ./misc/miscvborx.h
endif
