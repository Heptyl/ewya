/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* create:
*/
#ifndef __HI_TYPE_H__
#define __HI_TYPE_H__

#if defined(__KERNEL__)
#include <linux/version.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36))
#define DECLARE_MUTEX DEFINE_SEMAPHORE
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* --------------------------------------------------------------------------------------------------------------*
 * Defintion of basic data types. The data types are applicable to both the application layer and kernel codes. *
 * CNcomment: 基本数据类型定义，应用层和内核代码均使用  CNend                                                   *
 * -------------------------------------------------------------------------------------------------------------- */
/* ************************** Structure Definition *************************** */
/* * \addtogroup      Common_TYPE  */
/* * @{  */ /* * <!-- [Common_TYPE]  */

typedef unsigned char HI_U8;    /* prefix character u8 */
typedef unsigned char HI_UCHAR; /*  */
typedef unsigned short HI_U16;  /* prefix character u16 */
typedef unsigned int HI_U32;    /* prefix character u32 */

typedef signed char HI_S8; /* prefix character s8 */
typedef short HI_S16;      /* prefix character s16 */
typedef int HI_S32;        /* prefix character s32 */

#ifndef _M_IX86
typedef unsigned long long HI_U64; /* prefix character u64 */
typedef long long HI_S64;          /* prefix character s64 */
#else
typedef __int64 HI_U64; /* prefix character u64 */
typedef __int64 HI_S64; /* prefix character s64 */
#endif

typedef char HI_CHAR;   /*  */
typedef char *HI_PCHAR; /*  */
typedef char16_t HI_CHAR16;

typedef float HI_FLOAT;   /* prefix character f */
typedef double HI_DOUBLE; /* prefix character d */
/* typedef void                    HI_VOID; */
#define HI_VOID void

typedef unsigned long HI_SIZE_T;
typedef unsigned long HI_LENGTH_T;

typedef HI_U32 HI_HANDLE;

typedef unsigned int HI_PHYS_ADDR_T;
typedef unsigned long long HI_VIRT_ADDR_T;

/* * Constant Definition  */
/** CNcomment: 常量定义  */
typedef enum {
    HI_FALSE = 0,
    HI_TRUE = 1,
} HI_BOOL;

#ifndef NULL
#define NULL 0L
#endif

#define HI_NULL     0L
#define HI_NULL_PTR 0L

#define HI_SUCCESS 0
#define HI_FAILURE (-1)

#define HI_INVALID_HANDLE (0xffffffff)

#define HI_INVALID_PTS  (0xffffffff)
#define HI_INVALID_TIME (0xffffffff)

#define HI_OS_LINUX 0xabcd
#define HI_OS_WIN32 0xcdef

/* MAX VALUE WITH THE TYPES */
#define HI_U32_MAX_VALUE 0xffffffff
#define HI_S32_MAX_VALUE 0x7fffffff
#define HI_U8_MAX_VALUE 0xff

#ifdef _WIN32
#define HI_OS_TYPE HI_OS_WIN32
#else
#define __OS_LINUX__
#define HI_OS_TYPE HI_OS_LINUX
#endif

#ifdef HI_ADVCA_SUPPORT
#define __INIT__
#define __EXIT__
#else
#define __INIT__ __init
#define __EXIT__ __exit
#endif

/**

define of HI_HANDLE :
bit31                                                           bit0
  |<----   16bit --------->|<---   8bit    --->|<---  8bit   --->|
  |--------------------------------------------------------------|
  |      HI_MOD_ID_E       |  mod defined data |     chnID       |
  |--------------------------------------------------------------|

mod defined data: private data define by each module(for example: sub-mod id), usually, set to 0.
*/
#define HI_HANDLE_MAKEHANDLE(mod, privatedata, chnid)\
    (HI_HANDLE)((((mod)&0xffff) << 16) | ((((privatedata)&0xff) << 8)) | (((chnid)&0xff)))

#define HI_HANDLE_GET_MODID(handle)   (((handle) >> 16) & 0xffff)
#define HI_HANDLE_GET_PriDATA(handle) (((handle) >> 8) & 0xff)
#define HI_HANDLE_GET_CHNID(handle)   (((handle)) & 0xff)

#define HI_HANDLE_INIT(mod, private_data, chan_id) (hi_handle)((((mod) & 0xff) << 24) | ((((private_data) & 0xff) << 16)) | (((chan_id) & 0xffff)))

#define HI_HANDLE_GET_MODULE_ID(handle)    (((handle) >> 24) & 0xff)
#define HI_HANDLE_GET_PRIVATE_DATA(handle) (((handle) >> 16) & 0xff)
#define HI_HANDLE_GET_CHAN_ID(handle)      (((handle)) & 0xffff)


#ifndef HI_UNUSED
#define HI_UNUSED(x) ((void)x)
#endif
typedef unsigned char hi_uchar;
typedef unsigned char hi_u8;
typedef unsigned short hi_u16;
typedef unsigned int hi_u32;
typedef unsigned long long hi_u64;
typedef unsigned long hi_ulong;

typedef char hi_char;
typedef signed char hi_s8;
typedef short hi_s16;
typedef int hi_s32;
typedef long long hi_s64;
typedef long hi_slong;

typedef float hi_float;
typedef double hi_double;

typedef void hi_void;

typedef unsigned long hi_size_t;
typedef unsigned long hi_length_t;

typedef unsigned long           hi_uintptr_t;

typedef hi_u32 hi_handle;

typedef HI_BOOL hi_bool;

typedef unsigned long long      hi_mem_size_t;
typedef long long               hi_mem_handle_t;

typedef struct {
    hi_mem_handle_t mem_handle; /* Handle fo buffer header address */ /* CNcomment: 内存句柄，模块之间传递的唯一标识 */
    hi_mem_size_t addr_offset;  /* buffer offset */ /* CNcomment: 地址偏移 */
} hi_mem_handle;

#ifdef CONFIG_ARCH_LP64_MODE
typedef unsigned int hi_phys_addr_t;
typedef unsigned long long hi_virt_addr_t;
#else
typedef unsigned int hi_phys_addr_t;
typedef unsigned int hi_virt_addr_t;
#endif


/* * @}  */ /* * <!-- ==== Structure Definition end ====  */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_TYPE_H__ */
