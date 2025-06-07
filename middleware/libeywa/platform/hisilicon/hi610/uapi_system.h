/*
 * Copyright (c) CompanyNameMagicTag 2009-2019. All rights reserved.
 * Description: uapi_system.h
 */

#ifndef __UAPI_SYSTEM_H__
#define __UAPI_SYSTEM_H__

#include "soc_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Define the chip name */
typedef enum {
    UAPI_CHIP_NAME_WUDANGSTICK = 0x1001,
    UAPI_CHIP_NAME_KONGTONGV100 = 0x1002,
    UAPI_CHIP_NAME_SHAOLINGUN = 0x2001,
    UAPI_CHIP_NAME_SHAOLINAXE = 0x2002,
    UAPI_CHIP_NAME_SHAOLINLASH = 0x2003,
    UAPI_CHIP_NAME_SHAOLINKNIVE = 0x2004,
    UAPI_CHIP_NAME_SHAOLINSWORD = 0x2005,
    UAPI_CHIP_NAME_HENGSHANV200 = 0x2006,
    UAPI_CHIP_NAME_HI3751V811 = 0x2007,
    UAPI_CHIP_NAME_SHAOLINFIST = 0x2008,
    UAPI_CHIP_NAME_SHAOLINSPEAR = 0x3001,
} uapi_chip_name_id;

typedef enum {
    UAPI_CHIP_REVISION_A = 0x01,
    UAPI_CHIP_REVISION_B = 0x02,
    UAPI_CHIP_REVISION_C = 0x03,
    UAPI_CHIP_REVISION_D = 0x04,

    UAPI_CHIP_REVISION_MAX
} uapi_chip_revision;

typedef enum {
    UAPI_CHIP_PACKAGE_TYPE_BGA_15_15 = 0,
    UAPI_CHIP_PACKAGE_TYPE_BGA_16_16,
    UAPI_CHIP_PACKAGE_TYPE_BGA_19_19,
    UAPI_CHIP_PACKAGE_TYPE_BGA_23_23,
    UAPI_CHIP_PACKAGE_TYPE_BGA_31_31,
    UAPI_CHIP_PACKAGE_TYPE_QFP_216,
    UAPI_CHIP_PACKAGE_TYPE_BGA_21_21,
    UAPI_CHIP_PACKAGE_TYPE_BGA_14_14,
    UAPI_CHIP_PACKAGE_TYPE_BGA_35_35,
    UAPI_CHIP_PACKAGE_TYPE_BGA_19_22,
    UAPI_CHIP_PACKAGE_TYPE_MAX
} uapi_chip_package_type;

typedef struct {
    td_s32 tm_sec;
    td_s32 tm_min;
    td_s32 tm_hour;
    td_s32 tm_mday;
    td_s32 tm_mon;
    td_s32 tm_year;
} uapi_sys_build_time;

/* Define the chip capability */
typedef struct {
    td_bool dd_support;
    td_bool ddp_support;
    td_bool dbx_support;
    td_bool dts_support;
    td_bool advca_support;
    td_bool macrovision_support;
    td_bool hdr10_support;
    td_bool dolbyvision_support;
    td_bool smp_support; /* Whether support SMP or not */ /* CNcomment:系统是否支持安全媒体通路 */
    td_bool ks_support;
    td_bool hvs1_support;
    td_bool hvs2_support;
} uapi_sys_chip_cap;

typedef enum {
    UAPI_SYS_POLICY_EXTRA_BUFF = 0,
    UAPI_SYS_POLICY_CTS_FLAG,
    UAPI_SYS_POLICY_GTS_FLAG,
    UAPI_SYS_POLICY_DISP_WIDTH,
    UAPI_SYS_POLICY_VCODEC_WIDTH,
    UAPI_SYS_POLICY_VCODEC_DISABLE,
    UAPI_SYS_POLICY_MAX
} uapi_sys_policy_type;

typedef struct {
    td_bool policy_valid;
    td_u32 policy_value;
} uapi_sys_policy;

/* System version, that is, the version of the software developer's kit (SDK) */
typedef struct {
    /* CNcomment: SDK运行时检测到芯片名 */
    uapi_chip_name_id chip_name_id;

    /* Chip revision */
    uapi_chip_revision chip_revision;

    /* Version string of the SDK */
    /* CNcomment: SDK软件版本号字符串，最大长度80个字符 */
    td_char sdk_version[80];

    /* Version string of the Boot */
    /* CNcomment: Boot版本号字符串，最大长度80个字符 */
    td_char boot_version[80];

    /* Version string of the secure os image */
    /* CNcomment: 安全OS版本字符串，最大长度96个字符 */
    td_char secure_os_version[96];
} uapi_sys_version;

/* Define the chip id */
typedef struct {
    td_u64 chip_id_64; /* the unique 64 bits chipid */ /* CNcomment:64位芯片唯一ID */
    td_u32 chip_id_32;                                 /* the unique 32 bits chipid */
                                                       /* CNcomment:32位芯片唯一ID */
} uapi_sys_chip_id;

/*
 * Defines user mode proc show buffer
 * CNcomment: 用户态PROC buffer定义
 */
typedef struct {
    td_u8 *buf; /* Buffer address */ /* CNcomment: Buffer地址 */
    td_u32 size; /* Buffer size */   /* CNcomment: Buffer大小 */
    td_u32 offset; /* Offset */      /* CNcomment: 打印偏移地址 */
} uapi_user_proc_show_buffer;

/*
 * Proc show function
 * CNcomment: Proc信息显示回调函数
 */
typedef td_s32 (*uapi_user_proc_show_fn)(uapi_user_proc_show_buffer *buf, td_void *priv_data);

/*
 * Proc command function
 * CNcomment: Proc控制回调函数
 */
typedef td_s32 (*uapi_user_proc_cmd_fn)(uapi_user_proc_show_buffer *buf, td_u32 argc, td_u8 *argv[],
    td_void *priv_data);

/*
 * Defines user mode proc entry
 * CNcomment: 用户态PROC入口定义
 */
typedef struct {
    td_char *entry_name; /* Entry name */ /* CNcomment: 入口文件名 */
    td_char *directory;  /* Directory name. If null, the entry will be added to /proc/xxx/ */
                         /* CNcomment: 目录名，如果为空，将创建到/proc/soc目录下 */
    uapi_user_proc_show_fn show_proc; /* Proc show function */ /* CNcomment: Proc信息显示回调函数 */
    uapi_user_proc_cmd_fn cmd_proc; /* Proc command function */ /* CNcomment: Proc控制回调函数 */
    td_void *private_data; /* Private data */ /* CNcomment: Buffer地址 */
} uapi_user_proc_entry;

typedef struct {
    td_u32 min; /* the minimum value */ /* CNcomment 范围最小值 */
    td_u32 max; /* the maximum value */ /* CNcomment 范围最大值 */
} uapi_sys_range;

/*
 * brief Initializes the system. CNcomment: 系统初始化 CNend
 * attention \n
 * You must call this API to initialize the system before using the APIs of all modules.
 * Though you can call other APIs successfully before calling this API, the subsequent operations may fail.\n
 * CNcomment: 在使用所有模块的接口之前都需要先调用此接口对系统进行初始化\n
 * 在调用这个接口之前调用其他接口，不会返回失败，但是不保证执行的正确性 CNend
 * param N/A CNcomment: 无 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_init(td_void);

/*
 * brief Deinitializes the system. CNcomment: 系统去初始化 CNend
 * attention \n
 * If all modules are not used, you need to call this API to deinitialize the system.\n
 * CNcomment: 所有模块都不再使用后调用此接口去初始化 CNend
 * param N/A CNcomment: 无 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_deinit(td_void);

/*
 * brief Obtains the version number. CNcomment: 获取版本号 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[out] version Pointer to the version number (output). CNcomment: 指针类型，输出版本号 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_version(uapi_sys_version *version);

/*
 * brief Obtains the package type. CNcomment: 获取封装类型 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[out] package_type Pointer to the package type (output). CNcomment: 指针类型，输出封装类型 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_chip_package_type(uapi_chip_package_type *package_type);

/*
 * brief Obtains the chip support attributes. CNcomment: 获取芯片支持的能力 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[in] chip_cap Indicate which chip capability. CNcomment: 输入参数，指明获取何种芯片能力 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_chip_cap(uapi_sys_chip_cap *chip_cap);

/*
 * brief Obtains the chip attributes. CNcomment: 获取芯片属性 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[out] chip_id Pointer to the chip id(output). CNcomment: 指针类型，输出芯片id CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_chip_id(uapi_sys_chip_id *chip_id);

/*
 * brief Obtains the chip attributes. CNcomment: 获取内存配置信息 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[out] total Pointer to memory size. CNcomment: 指针类型，输出内存大小 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_mem_size(td_u32 *total);

/*
 * brief Obtains the board id. CNcomment: 获取单板的板级 id CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[out] Pointer a board id value. CNcomment: 指针类型，输出board id 值 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_board_id(td_u32 *board_id);

/*
 * brief Get timestamp. CNcomment: 获取时间戳 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[out] time_ms Pointer to the timestamp value (output) CNcomment: 输出时间戳 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 * N/A CNcomment: 无 CNend
 */
td_s32 uapi_sys_get_time_stamp(td_u32 *time_ms);

/*
 * brief Sets the debugging information level of a module. CNcomment: 设置模块的debugging信息级别 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[in] module_id Debugging ID of a module CNcomment: 模块的debuggingID。 CNend
 * param[in] log_level Debugging information level of a module CNcomment: 模块的debugging信息级别 CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 */
td_s32 uapi_log_set_level(td_u32 module_id, soc_log_level log_level);

/*
 * brief Sets the debugging information file size(MB), range from 1 to 120.
 *        CNcomment: 设置日志大小(MB)，范围为1到120 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[in]  size Debugging information file size(MB). CNcomment: 日志的大小(MB) CNend
 * retval ::TD_SUCCESS Success CNcomment: 成功 CNend
 * retval ::TD_FAILURE Calling this API fails. CNcomment: API系统调用失败 CNend
 * see \n
 */
td_s32 uapi_log_set_size(td_u32 size);

/*
 * brief User mode proc cretea directory. CNcomment:用户态proc创建目录 CNend
 * attention You need register module before calling this API. Only support create one level directory.
   CNcomment:需要先注册模块，只支持创建一级目录 CNend
 * param[in] pszName The directory name. CNcomment:目录名 CNend
 * param[out] None CNcomment:无 CNend
 * retval ::TD_SUCCESS Success CNcomment:成功 CNend
 * retval ::TD_FAILURE Failure CNcomment:失败 CNend
 * see \n
 * N/A CNcomment:无 CNend
 */
td_s32 uapi_user_proc_add_dir(const td_char *name);

/*
 * brief User mode proc remove directory. CNcomment:用户态proc删除目录 CNend
 * attention It will return fail if there are entries in the directory.
   CNcomment:如果目录下还有入口文件,将会删除失败 CNend
 * param[in] name The directory name. CNcomment:目录名 CNend
 * retval ::TD_SUCCESS Success CNcomment:成功 CNend
 * retval ::TD_FAILURE Failure CNcomment:失败 CNend
 * see \n
 * N/A CNcomment:无 CNend
 */
td_s32 uapi_user_proc_remove_dir(const td_char *name);

/*
 * brief User mode proc add entry. CNcomment:用户态proc创建入口 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[in] module_id Module ID. CNcomment:模块ID CNend
 * param[in] entry Parameter of entry. CNcomment:创建入口参数 CNend
 * retval ::TD_SUCCESS Success CNcomment:成功 CNend
 * retval ::TD_FAILURE Failure CNcomment:失败 CNend
 * see \n
 * N/A CNcomment:无 CNend
 */
td_s32 uapi_user_proc_add_entry(td_u32 module_id, const uapi_user_proc_entry *entry);

/*
 * brief User mode proc remove entry. CNcomment:用户态proc删除入口 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[in] module_id Module ID. CNcomment:模块ID CNend
 * param[in] entry Parameter of entry. CNcomment:删除入口参数 CNend
 * retval ::TD_SUCCESS Success CNcomment:成功 CNend
 * retval ::TD_FAILURE Failure CNcomment:失败 CNend
 * see \n
 * N/A CNcomment:无 CNend
 */
td_s32 uapi_user_proc_remove_entry(td_u32 module_id, const uapi_user_proc_entry *entry);

/*
 * brief User mode proc print function. CNcomment:用户态proc打印内容的函数 CNend
 * attention \n
 * N/A CNcomment: 无 CNend
 * param[in] show_buf Output buffer parameter. CNcomment:输出buffer参数 CNend
 * param[in] fmt   Format parameter. CNcomment:打印格式化参数 CNend
 * retval ::TD_SUCCESS Success CNcomment:成功 CNend
 * retval ::TD_FAILURE Failure CNcomment:失败 CNend
 * see \n
 * N/A CNcomment:无 CNend
 */
td_s32 uapi_user_proc_printf(uapi_user_proc_show_buffer *show_buf, const td_char *fmt, ...);

/* *
@brief Register one module to manager. CNcomment:模块注册，用于管理 CNend
@attention Before manager someone module, calling this interface. CNcomment:如需管理模块，用此接口先注册 CNend
@param[in] module_id   The module ID. CNcomment:模块ID CNend
@param[in] module_name The module name CNcomment:模块名称 CNend
@retval ::TD_SUCCESS Success CNcomment:成功 CNend
@retval ::TD_FAILURE Failure CNcomment:失败 CNend
@see \n
N/A CNcomment:无 CNend
*/
td_s32 uapi_module_register(td_u32 module_id, const td_char *module_name);

/* *
@brief UnRegister one module to trace. CNcomment:模块移除 CNend
@attention Before stopping to manage someone module, calling this interface.
           CNcomment:不需要管理此模块时，使用此接口移除模块 CNend
@param[in] module_id The module ID. CNcomment:模块ID CNend
@retval ::TD_SUCCESS Success CNcomment:成功 CNend
@retval ::TD_FAILURE Failure CNcomment:失败 CNend
@see \n
N/A CNcomment:无 CNend
*/
td_s32 uapi_module_unregister(td_u32 module_id);

#ifdef __cplusplus
}
#endif

#endif /* __UAPI_SYSTEM_H__ */
