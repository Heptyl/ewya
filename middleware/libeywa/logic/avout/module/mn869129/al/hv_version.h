/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HV_VERSION_H__
#define __HV_VERSION_H__

#if HAVE_CONFIG_H
#include <config.h>
#define HV_VERSION			PACKAGE_VERSION
#else
#define HV_VERSION			"x.x.x"
#endif

#ifdef CONFIG_BUILD_DATE
#define CONV_STR(s)			CONV_STR_AGAIN(s)
#define CONV_STR_AGAIN(s)	#s
#define HV_BUILD_DATE		CONV_STR(CONFIG_BUILD_DATE)
#else
#define HV_BUILD_DATE		"YYYY-MM-DD hh:mm:ss JST"
#endif

#define HV_COPYRIGHT		"Copyright (C) 2015-2020 Socionext Inc."

#define CONFIG_HV_SC1H21BJ2 
#define __linux

#if defined(CONFIG_HV_SC1H21BJ2)
#define HV_PRODUCT_NO		"SC1H21BJ2"
#elif defined(CONFIG_HV_SC1H22BJ2)
#define HV_PRODUCT_NO		"SC1H22BJ2"
#elif defined(CONFIG_HV_SC1H23BJ2)
#define HV_PRODUCT_NO		"SC1H23BJ2"
#elif defined(CONFIG_HV_SC1H24BJ2)
#define HV_PRODUCT_NO		"SC1H24BJ2"
#elif defined(CONFIG_HV_SC1H25BJ2)
#define HV_PRODUCT_NO		"SC1H25BJ2"
#elif defined(CONFIG_HV_SC1H26BJ2)
#define HV_PRODUCT_NO		"SC1H26BJ2"
#elif defined(CONFIG_HV_SC1H27BJ2)
#define HV_PRODUCT_NO		"SC1H27BJ2"
#elif defined(CONFIG_HV_SC1H28BJ2)
#define HV_PRODUCT_NO		"SC1H28BJ2"
#elif defined(CONFIG_HV_SC1H29BJ2)
#define HV_PRODUCT_NO		"SC1H29BJ2"
#else
#error "unsupported product number of HV series"
#endif

// #if defined(CONFIG_WIN_86)
// #define TARGET_HW_NAME		"win86"
// #elif defined(CONFIG_WIN_64)
// #define TARGET_HW_NAME		"win64"
// #elif defined(CONFIG_LIN_86)
// #define TARGET_HW_NAME		"linux86"
// #elif defined(CONFIG_LIN_64)
// #define TARGET_HW_NAME		"linux64"
// #elif defined(CONFIG_PIC24)
// #define TARGET_HW_NAME		"pic24"
// #else
// #error "unsupported target hardware"
// #endif

#endif // __HV_VERSION_H__
