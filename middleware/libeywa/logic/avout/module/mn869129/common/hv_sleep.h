/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __HV_SLEEP_H__
#define __HV_SLEEP_H__

#ifdef _WIN32
	#include <windows.h>
	#define SLEEP(s)				Sleep((s))
#elif defined(__linux)
	#include <unistd.h>
	#define SLEEP(s)				usleep((s)*1000)
#elif defined(__XC8)
	#include <xc.h>
	#define SLEEP(s)				__delay_ms((s))
#elif (1)
       /*
	#include <xc.h>
	#include "pic24_sys_config.h"	// for FCY
	#include <libpic30.h>
	#define SLEEP(s)				__delay_ms((s))
       */
	//#define LF9129_CMD_Delay(x)         udelay(x*1000)
    #define SLEEP(x)  MsOS_DelayTaskUs(x*1000)//usleep((x)*1000)
#else
	#error "unsupported compiler"
#endif

#endif // __HV_SLEEP_H__
