/*
* Copyright (c) CompanyNameMagicTag. 2012-2019. All rights reserved.
* Description:
* Author:
* create:
*/
#ifndef __TD_TYPE_H__
#define __TD_TYPE_H__

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

/* * Constant Definition  */
/** CNcomment: 常量定义  */
typedef enum {
    TD_FALSE = 0,
    TD_TRUE = 1,
} td_bool;

#ifndef NULL
#define NULL 0L
#endif

#define TD_NULL     0L
#define TD_NULL_PTR 0L


#ifdef _WIN32
#define TD_OS_TYPE OSAL_OS_WIN32
#else
#define __OS_LINUX__
#define TD_OS_TYPE TD_OS_LINUX
#endif

#ifdef EXT_ADVCA_SUPPORT
#define __INIT__
#define __EXIT__
#else
#define __INIT__ __init
#define __EXIT__ __exit
#endif

/**

define of td_handle :
bit31                                                           bit0
  |<----   16bit --------->|<---   8bit    --->|<---  8bit   --->|
  |--------------------------------------------------------------|
  |      SOC_MOD_ID_E       |  mod defined data |     chnID       |
  |--------------------------------------------------------------|

mod defined data: private data define by each module(for example: sub-mod id), usually, set to 0.
*/
#define td_handle_MAKEHANDLE(mod, privatedata, chnid) \
    (td_handle)((((mod)&0xffff) << 16) | ((((privatedata)&0xff) << 8)) | (((chnid)&0xff)))

#define td_handle_GET_MODID(handle)   (((handle) >> 16) & 0xffff)
#define td_handle_GET_PriDATA(handle) (((handle) >> 8) & 0xff)
#define td_handle_GET_CHNID(handle)   (((handle)) & 0xff)

#define TD_HANDLE_INIT(mod, private_data, chan_id) (td_handle)((((mod) & 0xff) << 24) | \
    ((((private_data) & 0xff) << 16)) | (((chan_id) & 0xffff)))

#define TD_HANDLE_GET_MODULE_ID(handle)    (((handle) >> 24) & 0xff)
#define TD_HANDLE_GET_PRIVATE_DATA(handle) (((handle) >> 16) & 0xff)
#define TD_HANDLE_GET_CHAN_ID(handle)      (((handle)) & 0xffff)


#ifndef TD_UNUSED
#define TD_UNUSED(x) ((x) = (x))
#endif
typedef unsigned char td_uchar;
typedef unsigned char td_u8;
typedef unsigned short td_u16;
typedef unsigned int td_u32;
typedef unsigned long long td_u64;
typedef unsigned long td_ulong;

typedef char td_char;
typedef char16_t td_char16;
typedef signed char td_s8;
typedef short td_s16;
typedef int td_s32;
typedef long long td_s64;
typedef long td_slong;

typedef float td_float;
typedef double td_double;

typedef void td_void;

typedef unsigned long td_size_t;
typedef unsigned long td_length_t;

typedef unsigned long           td_uintptr_t;

typedef td_u32 td_handle;


typedef unsigned long long      td_mem_size_t;
typedef long long               td_mem_handle_t;

typedef struct {
    td_mem_handle_t mem_handle; /* Handle fo buffer header address */ /* CNcomment: 内存句柄，模块之间传递的唯一标识 */
    td_mem_size_t addr_offset;  /* buffer offset */ /* CNcomment: 地址偏移 */
} td_mem_handle;

#ifdef CONFIG_ARCH_LP64_MODE
typedef unsigned int td_phys_addr_t;
typedef unsigned long long td_virt_addr_t;
#else
typedef unsigned int td_phys_addr_t;
typedef unsigned int td_virt_addr_t;
#endif

const td_s32 TD_SUCCESS = 0;
const td_s32 TD_FAILURE = (-1);

const td_u32 TD_INVALID_HANDLE = (0xffffffff);

const td_s32 TD_INVALID_PTS = (0xffffffff);
const td_s32 TD_INVALID_TIME = (0xffffffff);

const td_s32 TD_OS_LINUX = 0xabcd;
const td_s32 OSAL_OS_WIN32 = 0xcdef;

/* MAX VALUE WITH THE TYPES */
const td_u32 td_u32_max_value = 0xffffffff;
const td_s32 td_s32_max_value = 0x7fffffff;
const td_s32 td_s32_min_value = 0x80000000;
const td_u8 td_u8_max_value = 0xff;
/* * @}  */ /* * <!-- ==== Structure Definition end ====  */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TD_TYPE_H__ */

