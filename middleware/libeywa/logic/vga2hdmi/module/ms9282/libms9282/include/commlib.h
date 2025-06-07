/**
******************************************************************************
* @file    CommLib.h
* @author  
* @version V2.1.0
* @date    15-Oct-2013
* @brief   This file contains all header files
* @history  
*
* Copyright (c) 2009-2013, MacroSilicon Technology Co.,Ltd.
******************************************************************************/

#ifndef _MACROSILICON_TECH_COMMON_H_
#define _MACROSILICON_TECH_COMMON_H_
#include "mst_typedef.h"

// KEIL C51
//#define __KEIL_C__
// KEIL ARM (STM32)
//#define __KEIL_ARM_STM32__
// Standard GCC
//#define __STD_GCC__
//Code Composer Studio
//#define __CCS__


#ifdef __KEIL_ARM_STM32__
#ifndef _PLATFORM_ARM_
#define _PLATFORM_ARM_
#endif
#endif


// Compiler issue.
//#if defined (__CSMC__)
//#elif defined (__KEIL_C__)
//#elif defined (_PLATFORM_ARM_)
//#elif defined (__STD_GCC__)
//#elif defined (__CCS__)
//#else
//    #error "Please define the Compiler type!!!"
//#endif 

// SW Version.
#define SW_MAJOR_VERSION        (0x02)    // Major version 
#define SW_MINOR_VERSION        (0x04)    // Minor version

#if defined(__KEIL_C__)

#ifndef SM39R16A3
    #define STC_11LxxXE
#endif

    // For keil c compiler
    #include <intrins.h>
    #include <stdio.h>  
    #include <math.h>
    #include <string.h>

    // Add 8051 register header 
#if defined STC_11LxxXE
    #include "STC_11LxxXE.h"
#elif defined SM39R16A3
    #include "SM39R16A3.h"
#endif
    #include "mst_typedef.h"
    // Add peripheral configuration
    #include "Periph_Config.h"

#elif defined(_PLATFORM_ARM_) || defined(__CCS__)
    #include <stdio.h>  
    #include <math.h>
    #include <string.h>

    #include "mst_typedef.h"

#elif defined(__STD_GCC__)
    #if defined(__KERNEL__)
        #include <linux/string.h>
    #else
        #include <string.h>
    #endif
    #include "mst_typedef.h"

#elif defined(__CSMC__)
    #include <string.h>
    #include "stm8s.h"
    #include "mst_typedef.h"
    #include "Periph_Config.h"
    #include "stm8s\stm8s_lib.h"    
    #include "stm8s\stm8s_it.h"
#ifdef SUPPORT_SYSTEM_EDID
        #include "mst_i2c_slave.h"
#endif

#endif 

#if defined(__CSMC__) || defined(__KEIL_C__)
    // Add mcu peripherals' header files.
    #include "mst_periph.h"
    #include "mst_debug.h"
    #include "mst_timer.h"
    #include "mst_i2c.h"
    #include "mst_keys.h"    
#endif
    
    // Platform Porting Related APIs
    #include "MS928x_mpi.h"

#endif //_MACROSILICON_TECH_COMMON_H_

