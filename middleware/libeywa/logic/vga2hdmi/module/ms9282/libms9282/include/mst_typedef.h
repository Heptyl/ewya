/**
******************************************************************************
* @file    mst_typedef.h
* @author  
* @version V2.1.0
* @date    15-Oct-2013
* @brief   Definitions for typedefs.
*
* Copyright (c) 2009-2013, MacroSilicon Technology Co.,Ltd.
******************************************************************************/

#ifndef _MACROSILICON_TECH_TYPEDEF_H_
#define _MACROSILICON_TECH_TYPEDEF_H_

/*
** Global typedefs.
*/

#ifndef NULL
#define NULL ((void*)0)
#endif

// For ARM platform
#if defined (_PLATFORM_ARM_)
#define  __CODE const
#define  __XDATA
#define  __DATA
#define __IDATA
#define  __NEAR
#define  __IO volatile


typedef _Bool BOOL;

#elif defined (__STD_GCC__)
#define  __CODE const
#define  __XDATA
#define  __DATA
#define __IDATA
#define  __NEAR
#define  __IO volatile


typedef _Bool BOOL;

typedef unsigned char BOOL;


#elif defined (_PLATFORM_WINDOWS_)
#define  __CODE
#define  __XDATA
#define  __DATA
#define __IDATA
#define  __NEAR
#define  __IO

#elif defined (__KEIL_C__)
#define __CODE code
#define __XDATA xdata
#define __DATA data
#define __IDATA idata
#define __NEAR
#define __IO volatile

//bool bype
typedef bit BOOL;

#elif defined (__CSMC__)
#define __CODE const
#define __XDATA
#define __DATA 
#define __IDATA 
#define __NEAR @near
#define __IO volatile

//bool bype
typedef _Bool BOOL;
#elif defined (_IAR_)
#define __CODE const
#define __XDATA
#define __DATA 
#define __IDATA 
#define __NEAR @near
#define __IO volatile

//bool bype
typedef _Bool BOOL;
#endif // end of compiler platform define 


//unsigned integer type
typedef unsigned char UINT8;
typedef char          CHAR;
typedef unsigned short UINT16;

//signed integer type
typedef signed char INT8;
typedef signed short INT16;

//32bit type
#if defined (_PLATFORM_ARM_) || defined (_PLATFORM_WINDOWS_)
typedef unsigned int UINT32;
typedef signed int INT32;
#else
typedef unsigned long int UINT32;
typedef signed long int INT32;
#endif

#define VOID void

#define FALSE 0
#define TRUE  1

#define DISABLE 0
#define ENABLE  1

#define LOW     0
#define HIGH    1

#define OFF     0
#define ON      1


// Helper macros.
#define _UNUSED_(arg)     ((arg) = (arg))

#ifndef _countof
#define _countof(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))
#endif

#if 0
#ifndef max
#define max(a, b)   (((a)>(b))?(a):(b)) 
#endif

#ifndef min
#define min(a, b)   (((a)<(b))?(a):(b))
#endif
#endif


// For porting to different compilers
//typedef  UINT8  __XDATA   XUINT8;
//typedef  UINT16 __XDATA   XUINT16;
//typedef  UINT32 __XDATA   XUINT32;

//typedef  INT8   __XDATA   XINT8;
//typedef  INT16  __XDATA   XINT16;
//typedef  INT32  __XDATA   XINT32;

typedef unsigned char BOOL;


#endif  // _MACROSILICON_TECH_TYPEDEF_H_

