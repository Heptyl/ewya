/*
 * Copyright (c) CompanyNameMagicTag 2017-2019. All rights reserved.
 * Description: internal debug define.
 * Author: AuthorNameMagicTag
 * Create: 2017-05-31
 */

#ifndef __SOC_LOG_H__
#define __SOC_LOG_H__

#include "td_type.h"
#include "soc_module.h"

#define SOC_LOG_MARK

#ifdef CONFIG_SOCT_LOG_SUPPORT
#define SOC_DEBUG

#ifdef __KERNEL__
#define soc_print printk
#define soc_panic printk
#else
#include <stdio.h>
#define soc_print printf
#define soc_panic printf
#endif

#else
#undef SOC_DEBUG
#ifndef __KERNEL__
#include <stdio.h>
#endif
static inline td_void soc_empty_print(__attribute__((unused)) const td_char *format, ...)
{
    return;
}

#define soc_panic(fmt...) do {                 \
    soc_empty_print(fmt); \
    } while (0)

#define soc_print(fmt...) do {                 \
    soc_empty_print(fmt); \
    } while (0)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Default level of the output debugging information.
 * CNcomment: Ĭ�ϵĵ�����Ϣ�������.
 */
#define SOC_LOG_LEVEL_DEFAULT SOC_LOG_LEVEL_ERROR

/*
 * SDK internal Use only.
 * Allow modules to modify, If the related module does not define it, no information output.
 */
#ifndef LOG_FUNC_TRACE
#define LOG_FUNC_TRACE 0
#endif

/*
 * SDK internal Use Only.
 * Allow modules to modify, If the related module does not define it, no information output.
 */
#ifndef LOG_IAPI_TRACE
#define LOG_IAPI_TRACE 0
#endif

/*
 * SDK internal use only.
 * Allow modules to modify, default value is SOC_ID_SYS.
 */
#ifndef LOG_MODULE_ID
#define LOG_MODULE_ID SOC_ID_SYS
#endif

#define LOG_BLOCK_PERLINE 16
#define LOG_BLOCK_BUFSIZE (LOG_BLOCK_PERLINE * 3 + 1)

#define ENABLE_FUNC_LINE

/*
 * Level of the output debugging information.
 * CNcomment: ������Ϣ�������.
 */
typedef enum {
    /* Alert information.
     * It indicates that a important exception occurs in the system.action must be taken immediately
     * CNcomment: ������Ϣ, �������Ĭ�ϲ���ӡ�����ڣ�Ĭ�ϴ�ӡ��BBOX��̨��־�ռ�ϵͳ��
     * �û�ϵͳ��Ҫ״̬�仯���ǳ���Ҫ���쳣��
     */
    SOC_LOG_LEVEL_ALERT = 0,
    /* Fatal error.
     * It indicates that a Fatal problem occurs in the system. Therefore, you must pay attention to it
     * CNcomment: ��������, ���������Ҫ�ر��ע��һ����ִ���������ϵͳ�������ش�����
     */
    SOC_LOG_LEVEL_FATAL = 1,
    /* Major error.
     * It indicates that a major problem occurs in the system and the system cannot run
     * CNcomment: һ�����, һ����ִ���������ϵͳ�����˱Ƚϴ�����⣬��������������
     */
    SOC_LOG_LEVEL_ERROR = 2,
    /* Warning.
     * It indicates that a minor problem occurs in the system, but the system still can run properly
     * CNcomment: �澯��Ϣ, һ����ִ�����Ϣ����ϵͳ���ܳ������⣬���ǻ��ܼ�������
     */
    SOC_LOG_LEVEL_WARNING = 3,
    /* Notice.
     * It is used to prompt users. Users can open the message when locating problems.
     * CNcomment: ��������Ҫ����Ϣ, һ�������ϵͳ�Ĺؼ�·��������
     */
    SOC_LOG_LEVEL_NOTICE = 4,
    /* INFO.
     * It is used to prompt users. Users can open the message when locating problems.
     * It is recommended to disable this message in general.
     * CNcomment: ��ʾ��Ϣ, һ����Ϊ�����û���������ڶ�λ�����ʱ����Դ򿪣�һ������½���ر�
     */
    SOC_LOG_LEVEL_INFO = 5,
    /* Debug.
     * It is used to prompt developers. Developers can open the message when locating problems
     * It is recommended to disable this message in general.
     * CNcomment: ��ʾ��Ϣ, һ����Ϊ������Ա����������趨�Ĵ�ӡ����һ������½���ر�
     */
    SOC_LOG_LEVEL_DBG = 6,
    /* Trace.
     * It is used to track the entry and exit of function when the interface is called.
     * CNcomment: ��ʾ��Ϣ��һ�����ڸ��ٽӿڵ���ʱ�����Ľ������˳�
     */
    SOC_LOG_LEVEL_TRACE = 7,
    SOC_LOG_LEVEL_MAX
} soc_log_level;

/* Just only for alert level print. */ /* CNcomment: Ϊ�˴�ӡ�澯��Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_ALERT (0)
/* Just only for fatal level print. */ /* CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_FATAL (1)
/* Just only for error level print. */ /* CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_ERROR (2)
/* Just only for warning level print. */ /* CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_WARN (3)
/* Just only for notice level print.  */ /* CNcomment: Ϊ�˴�ӡע����Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_NOTICE (4)
/* Just only for info level print. */ /* CNcomment: Ϊ�˴�ӡ��Ϣ������ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_INFO (5)
/* Just only for debug level print. */ /* CNcomment: Ϊ�˴�ӡ������Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_DBG (6)
/* Just only for trace level print. */ /* CNcomment: Ϊ�˴�ӡ�ӿڸ�����Ϣ���ƶ��ĺ��ӡ���� */
#define SOC_TRACE_LEVEL_TRACE (7)

#ifndef CONFIG_SOCT_LOG_LEVEL
#define CONFIG_SOCT_LOG_LEVEL (SOC_TRACE_LEVEL_INFO)
#endif

#ifdef ENABLE_FUNC_LINE
#define SOC_LOG_LINE __LINE__
#else
#define SOC_LOG_LINE 0
#endif

/* Just only debug output, MUST BE NOT calling it.
 * CNcomment: ���������Ϣ�ӿڣ����Ƽ�ֱ�ӵ��ô˽ӿ�
 */
td_void soc_log_print(td_u32 level, td_u32 module_id, const td_u8 *fn_name, td_u32 line_num, const td_char *format, ...);

/* CNcomment: ���������Ϣ�ӿڣ��������Ϣ�������� */
td_void soc_log_simple_print(const td_char *format, ...);

/* CNcomment: ��ӡ������ */
td_void soc_log_print_block(td_u32 level, td_u32 module_id, const td_char *block, td_u32 size);

#ifdef SOC_DEBUG

#define soc_trace(level, module_id, fmt...)                                               \
    do {                                                                                 \
        soc_log_print(level, (td_u32)module_id, (td_u8 *)__FUNCTION__, SOC_LOG_LINE, fmt); \
    } while (0)

#define soc_simple_trace(fmt...) do {                          \
        soc_log_simple_print(fmt); \
    } while (0)

#define soc_assert(expr)                                                                                               \
    do {                                                                                                              \
        if (!(expr)) {                                                                                                \
            soc_panic("\nASSERT failed at:\n  >File name: %s\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n", \
                __FILE__, __FUNCTION__, __LINE__, #expr);                                                             \
        }                                                                                                             \
    } while (0)

/* Supported for debug output to serial/network/u-disk.
 * CNcomment: ����ģ����Ҫ�������º�������������Ϣ������������ڡ����ڡ�U�̴洢��
 * Just only reserve the alert level output.
 * CNcomment: ��������BBOX�ؼ��ĵ�����Ϣ
 */
#if (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_ALERT)
#define soc_log_alert(fmt...) soc_trace(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)
#define soc_log_err(fmt...)
#define soc_log_warn(fmt...)
#define soc_log_notice(fmt...)
#define soc_log_info(fmt...)
#define soc_log_dbg(fmt...)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size) soc_log_print_block(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)
#define soc_err_print_block(block, size)
#define soc_warn_print_block(block, size)
#define soc_notice_print_block(block, size)
#define soc_info_print_block(block, size)
#define soc_dbg_print_block(block, size)

/* Supported for debug output to serial/network/u-disk.
 * CNcomment: ����ģ����Ҫ�������º�������������Ϣ������������ڡ����ڡ�U�̴洢��
 * Just only reserve the fatal level output.
 * CNcomment: �������������ĵ�����Ϣ
 */
#elif (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_FATAL)
#define soc_log_alert(fmt...) soc_trace(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...) soc_trace(SOC_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)
#define soc_log_warn(fmt...)
#define soc_log_notice(fmt...)
#define soc_log_info(fmt...)
#define soc_log_dbg(fmt...)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL, LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)
#define soc_warn_print_block(block, size)
#define soc_notice_print_block(block, size)
#define soc_info_print_block(block, size)
#define soc_dbg_print_block(block, size)

/* Just only reserve the alert/fatal/error level output.
 * CNcomment: �������������ĺʹ��󼶱�ĵ�����Ϣ
 */
#elif (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_ERROR)
#define soc_log_alert(fmt...)                soc_trace(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)                soc_trace(SOC_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)                  soc_trace(SOC_TRACE_LEVEL_ERROR, LOG_MODULE_ID, fmt)
#define soc_log_warn(fmt...)
#define soc_log_notice(fmt...)
#define soc_log_info(fmt...)
#define soc_log_dbg(fmt...)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL, LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_ERROR, LOG_MODULE_ID, block, size)
#define soc_warn_print_block(block, size)
#define soc_notice_print_block(block, size)
#define soc_info_print_block(block, size)
#define soc_dbg_print_block(block, size)

/* Just only reserve the alert/fatal/error/warning level output.
 * CNcomment: �������������ġ�����ġ����漶��ĵ�����Ϣ
 */
#elif (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_WARN)
#define soc_log_alert(fmt...)                soc_trace(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)                soc_trace(SOC_TRACE_LEVEL_FATAL, LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)                  soc_trace(SOC_TRACE_LEVEL_ERROR, LOG_MODULE_ID, fmt)
#define soc_log_warn(fmt...)                 soc_trace(SOC_TRACE_LEVEL_WARN,  LOG_MODULE_ID, fmt)
#define soc_log_notice(fmt...)
#define soc_log_info(fmt...)
#define soc_log_dbg(fmt...)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT, LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL, LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_ERROR, LOG_MODULE_ID, block, size)
#define soc_warn_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_WARN,  LOG_MODULE_ID, block, size)
#define soc_notice_print_block(block, size)
#define soc_info_print_block(block, size)
#define soc_dbg_print_block(block, size)

/* Just only reserve the alert/fatal/error/warning/notice level output.
 * CNcomment: �������������ġ�����ġ����桢��Ҫ����ĵ�����Ϣ
 */
#elif (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_NOTICE)
#define soc_log_alert(fmt...)                soc_trace(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)                soc_trace(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)                  soc_trace(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, fmt)
#define soc_log_warn(fmt...)                 soc_trace(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, fmt)
#define soc_log_notice(fmt...)               soc_trace(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, fmt)
#define soc_log_info(fmt...)
#define soc_log_dbg(fmt...)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, block, size)
#define soc_warn_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, block, size)
#define soc_notice_print_block(block, size)  soc_log_print_block(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, block, size)
#define soc_info_print_block(block, size)
#define soc_dbg_print_block(block, size)

/* Just only reserve the alert/fatal/error/warning/info level output.
 * CNcomment: �������������ġ�����ġ��������Ϣ����ĵ�����Ϣ
 */
#elif (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_INFO)
#define soc_log_alert(fmt...)                soc_trace(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)                soc_trace(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)                  soc_trace(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, fmt)
#define soc_log_warn(fmt...)                 soc_trace(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, fmt)
#define soc_log_notice(fmt...)               soc_trace(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, fmt)
#define soc_log_info(fmt...)                 soc_trace(SOC_TRACE_LEVEL_INFO,   LOG_MODULE_ID, fmt)
#define soc_log_dbg(fmt...)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, block, size)
#define soc_warn_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, block, size)
#define soc_notice_print_block(block, size)  soc_log_print_block(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, block, size)
#define soc_info_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_INFO,   LOG_MODULE_ID, block, size)
#define soc_dbg_print_block(block, size)

/* Just only reserve the alert/fatal/error/warning/info/debug level output.
 * CNcomment: �������������ġ�����ġ��������Ϣ����ĵ�����Ϣ
 */
#elif (CONFIG_SOCT_LOG_LEVEL == SOC_TRACE_LEVEL_DBG)
#define soc_log_alert(fmt...)                soc_trace(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)                soc_trace(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)                  soc_trace(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, fmt)
#define soc_log_warn(fmt...)                 soc_trace(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, fmt)
#define soc_log_notice(fmt...)               soc_trace(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, fmt)
#define soc_log_info(fmt...)                 soc_trace(SOC_TRACE_LEVEL_INFO,   LOG_MODULE_ID, fmt)
#define soc_log_dbg(fmt...)                  soc_trace(SOC_TRACE_LEVEL_DBG,    LOG_MODULE_ID, fmt)
#define soc_log_trace(fmt...)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, block, size)
#define soc_warn_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, block, size)
#define soc_notice_print_block(block, size)  soc_log_print_block(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, block, size)
#define soc_info_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_INFO,   LOG_MODULE_ID, block, size)
#define soc_dbg_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_DBG,    LOG_MODULE_ID, block, size)

/* Reserve all the levels output.
 * CNcomment: �������м��������Ϣ
 */
#else
#define soc_log_alert(fmt...)                soc_trace(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, fmt)
#define soc_log_fatal(fmt...)                soc_trace(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, fmt)
#define soc_log_err(fmt...)                  soc_trace(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, fmt)
#define soc_log_warn(fmt...)                 soc_trace(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, fmt)
#define soc_log_notice(fmt...)               soc_trace(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, fmt)
#define soc_log_info(fmt...)                 soc_trace(SOC_TRACE_LEVEL_INFO,   LOG_MODULE_ID, fmt)
#define soc_log_dbg(fmt...)                  soc_trace(SOC_TRACE_LEVEL_DBG,    LOG_MODULE_ID, fmt)
#define soc_log_trace(fmt...)                soc_trace(SOC_TRACE_LEVEL_TRACE,  LOG_MODULE_ID, fmt)

#define soc_alert_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_ALERT,  LOG_MODULE_ID, block, size)
#define soc_fatal_print_block(block, size)   soc_log_print_block(SOC_TRACE_LEVEL_FATAL,  LOG_MODULE_ID, block, size)
#define soc_err_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_ERROR,  LOG_MODULE_ID, block, size)
#define soc_warn_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_WARN,   LOG_MODULE_ID, block, size)
#define soc_notice_print_block(block, size)  soc_log_print_block(SOC_TRACE_LEVEL_NOTICE, LOG_MODULE_ID, block, size)
#define soc_info_print_block(block, size)    soc_log_print_block(SOC_TRACE_LEVEL_INFO,   LOG_MODULE_ID, block, size)
#define soc_dbg_print_block(block, size)     soc_log_print_block(SOC_TRACE_LEVEL_DBG,    LOG_MODULE_ID, block, size)
#endif

#else
#define soc_trace(level, module_id, fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)

#define soc_assert(expr)
#define soc_assert_ret(expr)

#define soc_log_alert(fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)
#define soc_log_fatal(fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)
#define soc_log_err(fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)
#define soc_log_warn(fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)
#define soc_log_notice(fmt...) do {                          \
    soc_empty_print(fmt); \
} while (0)
#define soc_log_info(fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)
#define soc_log_dbg(fmt...) do {                          \
    soc_empty_print(fmt); \
    } while (0)
#define soc_log_trace(fmt...) do {                          \
    soc_empty_print(fmt); \
} while (0)

#define soc_alert_print_block(block, size)
#define soc_fatal_print_block(block, size)
#define soc_err_print_block(block, size)
#define soc_warn_print_block(block, size)
#define soc_notice_print_block(block, size)
#define soc_info_print_block(block, size)
#define soc_dbg_print_block(block, size)
#endif /* endif SOC_DEBUG */

/* function trace log, strictly prohibited to expand */
#define soc_alert_print_err_code(err_code) soc_log_alert("Error Code: [0x%08X]\n", err_code)
/* function trace log, print the called function name when function is error */
#define soc_alert_print_call_fun_err(func, err_code) \
    soc_log_alert("Call %s Failed, Error Code: [0x%08X]\n", #func, err_code)
/* Function trace log, print the pointer name when pointer is null */
#define soc_alert_print_null_pointer(val) soc_log_alert("%s = %p,  Null Pointer!\n", #val, val)

/* function trace log, strictly prohibited to expand */
#define soc_fatal_print_err_code(err_code) soc_log_fatal("Error Code: [0x%08X]\n", err_code)
/* function trace log, print the called function name when function is error */
#define soc_fatal_print_call_fun_err(func, err_code) \
    soc_log_fatal("Call %s Failed, Error Code: [0x%08X]\n", #func, err_code)
/* Function trace log, print the pointer name when pointer is null */
#define soc_fatal_print_null_pointer(val) soc_log_fatal("%s = %p,  Null Pointer!\n", #val, val)

/* function trace log, strictly prohibited to expand */
#define soc_err_print_err_code(err_code) soc_log_err("Error Code: [0x%08X]\n", err_code)
/* function trace log, print the called function name when function is error */
#define soc_err_print_call_fun_err(func, err_code) soc_log_err("Call %s Failed, Error Code: [0x%08X]\n", #func, err_code)
/* Function trace log, print the pointer name when pointer is null */
#define soc_err_print_null_pointer(val) soc_log_err("%s = %p,  Null Pointer!\n", #val, val)

/* function trace log, strictly prohibited to expand */
#define soc_warn_print_err_code(err_code) soc_log_warn("Error Code: [0x%08X]\n", err_code)
/* function trace log, print the called function name when function is error */
#define soc_warn_print_call_fun_err(func, err_code) \
    soc_log_warn("Call %s Failed, Error Code: [0x%08X]\n", #func, err_code)
/* Function trace log, print the pointer name when pointer is null */
#define soc_warn_print_null_pointer(val) soc_log_warn("%s = %p,  Null Pointer!\n", #val, val)

/* Used for displaying more detailed alert information */
#define soc_alert_print_s32(val)   soc_log_alert("%s = %d\n",        #val, val)
#define soc_alert_print_u32(val)   soc_log_alert("%s = %u\n",        #val, val)
#define soc_alert_print_s64(val)   soc_log_alert("%s = %lld\n",      #val, val)
#define soc_alert_print_u64(val)   soc_log_alert("%s = %llu\n",      #val, val)
#define soc_alert_print_h32(val)   soc_log_alert("%s = 0x%08X\n",    #val, val)
#define soc_alert_print_h64(val)   soc_log_alert("%s = 0x%016llX\n", #val, val)
#define soc_alert_print_str(val)   soc_log_alert("%s = %s\n",        #val, val)
#define soc_alert_print_void(val)  soc_log_alert("%s = %p\n",        #val, val)
#define soc_alert_print_float(val) soc_log_alert("%s = %f\n",        #val, val)
#define soc_alert_print_bool(val)  soc_log_alert("%s = %s\n",        #val, val ? "True" : "False")
#define soc_alert_print_info(val)  soc_log_alert("<%s>\n",            val)

/* Used for displaying more detailed fatal information */
#define soc_fatal_print_s32(val)   soc_log_fatal("%s = %d\n",        #val, val)
#define soc_fatal_print_u32(val)   soc_log_fatal("%s = %u\n",        #val, val)
#define soc_fatal_print_s64(val)   soc_log_fatal("%s = %lld\n",      #val, val)
#define soc_fatal_print_u64(val)   soc_log_fatal("%s = %llu\n",      #val, val)
#define soc_fatal_print_h32(val)   soc_log_fatal("%s = 0x%08X\n",    #val, val)
#define soc_fatal_print_h64(val)   soc_log_fatal("%s = 0x%016llX\n", #val, val)
#define soc_fatal_print_str(val)   soc_log_fatal("%s = %s\n",        #val, val)
#define soc_fatal_print_void(val)  soc_log_fatal("%s = %p\n",        #val, val)
#define soc_fatal_print_float(val) soc_log_fatal("%s = %f\n",        #val, val)
#define soc_fatal_print_bool(val)  soc_log_fatal("%s = %s\n",        #val, val ? "True" : "False")
#define soc_fatal_print_info(val)  soc_log_fatal("<%s>\n",            val)

/* Used for displaying more detailed error information */
#define soc_err_print_s32(val)     soc_log_err("%s = %d\n",        #val, val)
#define soc_err_print_u32(val)     soc_log_err("%s = %u\n",        #val, val)
#define soc_err_print_s64(val)     soc_log_err("%s = %lld\n",      #val, val)
#define soc_err_print_u64(val)     soc_log_err("%s = %llu\n",      #val, val)
#define soc_err_print_h32(val)     soc_log_err("%s = 0x%08X\n",    #val, val)
#define soc_err_print_h64(val)     soc_log_err("%s = 0x%016llX\n", #val, val)
#define soc_err_print_str(val)     soc_log_err("%s = %s\n",        #val, val)
#define soc_err_print_void(val)    soc_log_err("%s = %p\n",        #val, val)
#define soc_err_print_float(val)   soc_log_err("%s = %f\n",        #val, val)
#define soc_err_print_bool(val)    soc_log_err("%s = %s\n",        #val, val ? "True" : "False")
#define soc_err_print_info(val)    soc_log_err("<%s>\n",            val)

/* Used for displaying more detailed warning information */
#define soc_warn_print_s32(val)    soc_log_warn("%s = %d\n",        #val, val)
#define soc_warn_print_u32(val)    soc_log_warn("%s = %u\n",        #val, val)
#define soc_warn_print_s64(val)    soc_log_warn("%s = %lld\n",      #val, val)
#define soc_warn_print_u64(val)    soc_log_warn("%s = %llu\n",      #val, val)
#define soc_warn_print_h32(val)    soc_log_warn("%s = 0x%08X\n",    #val, val)
#define soc_warn_print_h64(val)    soc_log_warn("%s = 0x%016llX\n", #val, val)
#define soc_warn_print_str(val)    soc_log_warn("%s = %s\n",        #val, val)
#define soc_warn_print_void(val)   soc_log_warn("%s = %p\n",        #val, val)
#define soc_warn_print_float(val)  soc_log_warn("%s = %f\n",        #val, val)
#define soc_warn_print_bool(val)   soc_log_warn("%s = %s\n",        #val, val ? "True" : "False")
#define soc_warn_print_info(val)   soc_log_warn("<%s>\n",            val)

/* Used for displaying more detailed key info information */
#define soc_notice_print_s32(val)    soc_log_notice("%s = %d\n",        #val, val)
#define soc_notice_print_u32(val)    soc_log_notice("%s = %u\n",        #val, val)
#define soc_notice_print_s64(val)    soc_log_notice("%s = %lld\n",      #val, val)
#define soc_notice_print_u64(val)    soc_log_notice("%s = %llu\n",      #val, val)
#define soc_notice_print_h32(val)    soc_log_notice("%s = 0x%08X\n",    #val, val)
#define soc_notice_print_h64(val)    soc_log_notice("%s = 0x%016llX\n", #val, val)
#define soc_notice_print_str(val)    soc_log_notice("%s = %s\n",        #val, val)
#define soc_notice_print_void(val)   soc_log_notice("%s = %p\n",        #val, val)
#define soc_notice_print_float(val)  soc_log_notice("%s = %f\n",        #val, val)
#define soc_notice_print_bool(val)   soc_log_notice("%s = %s\n",        #val, val ? "True" : "False")
#define soc_notice_print_info(val)   soc_log_notice("<%s>\n",            val)

/* Only used for key info, Can be expanded as needed */
#define soc_info_print_s32(val)    soc_log_info("%s = %d\n",        #val, val)
#define soc_info_print_u32(val)    soc_log_info("%s = %u\n",        #val, val)
#define soc_info_print_s64(val)    soc_log_info("%s = %lld\n",      #val, val)
#define soc_info_print_u64(val)    soc_log_info("%s = %llu\n",      #val, val)
#define soc_info_print_h32(val)    soc_log_info("%s = 0x%08X\n",    #val, val)
#define soc_info_print_h64(val)    soc_log_info("%s = 0x%016llX\n", #val, val)
#define soc_info_print_str(val)    soc_log_info("%s = %s\n",        #val, val)
#define soc_info_print_void(val)   soc_log_info("%s = %p\n",        #val, val)
#define soc_info_print_float(val)  soc_log_info("%s = %f\n",        #val, val)
#define soc_info_print_bool(val)   soc_log_info("%s = %s\n",        #val, val ? "True" : "False")
#define soc_info_print_info(val)   soc_log_info("<%s>\n",            val)

/* Only used for self debug, Can be expanded as needed */
#define soc_dbg_print_s32(val)     soc_log_dbg("%s = %d\n",        #val, val)
#define soc_dbg_print_u32(val)     soc_log_dbg("%s = %u\n",        #val, val)
#define soc_dbg_print_s64(val)     soc_log_dbg("%s = %lld\n",      #val, val)
#define soc_dbg_print_u64(val)     soc_log_dbg("%s = %llu\n",      #val, val)
#define soc_dbg_print_h32(val)     soc_log_dbg("%s = 0x%08X\n",    #val, val)
#define soc_dbg_print_h64(val)     soc_log_dbg("%s = 0x%016llX\n", #val, val)
#define soc_dbg_print_str(val)     soc_log_dbg("%s = %s\n",        #val, val)
#define soc_dbg_print_void(val)    soc_log_dbg("%s = %p\n",        #val, val)
#define soc_dbg_print_float(val)   soc_log_dbg("%s = %f\n",        #val, val)
#define soc_dbg_print_bool(val)    soc_log_dbg("%s = %s\n",        #val, val ? "True" : "False")
#define soc_dbg_print_info(val)    soc_log_dbg("<%s>\n",            val)

/* define function trace */
#define soc_notice_func_enter() soc_log_notice(" ===>[Enter]\n")
#define soc_notice_func_exit()  soc_log_notice(" <===[Exit]\n")
#define soc_notice_func_trace() soc_log_notice(" =TRACE=\n")

#define soc_info_func_enter() soc_log_info(" ===>[Enter]\n")
#define soc_info_func_exit()  soc_log_info(" <===[Exit]\n")
#define soc_info_func_trace() soc_log_info(" =TRACE=\n")

#define soc_dbg_func_enter() soc_log_dbg(" ===>[Enter]\n")
#define soc_dbg_func_exit()  soc_log_dbg(" <===[Exit]\n")
#define soc_dbg_func_trace() soc_log_dbg(" =TRACE=\n")

#define mk_str(exp) #exp
#define mk_marco_to_str(exp) mk_str(exp)
#define VERSION_STRING ("SDK_VERSION: [" mk_marco_to_str(SDK_VERSION) "] Build Time: [" __DATE__ ", " __TIME__ "]")
#define USER_VERSION_STRING ("SDK_VERSION: [" mk_marco_to_str(SDK_VERSION) "]")

/* Function trace log for user input low frequency counts
 * CNcomment: �������뽵Ƶ��ӡ�Ĵ���
 */
#define soc_dbg_func_trace_low_freq_cnt(freq)                                \
    do {                                                                    \
        static unsigned int freqtimes = 0;                                  \
        if ((freqtimes % (unsigned int)freq) == 0) {                        \
            soc_log_dbg(" =TRACE Freq=%d,freqtimes=%d=\n", freq, freqtimes); \
        }                                                                   \
        freqtimes++;                                                        \
    } while (0)

/* function trace log for user add extra print log as need
 * CNcomment: �û������������Ӷ���Ĵ�ӡ
 * need use soc_func_trace_low_freq_cnt_end at the same time, to repesent the addtional prints is the end
 * CNcomment: ��Ҫͬʱʹ��soc_func_trace_low_freq_cnt_end�������������ӵĴ�ӡ��ֹ
 */
#define soc_func_trace_low_freq_cnt_begin(freq)       \
    do {                                             \
        static unsigned int freqtimes = 0;           \
        if ((freqtimes % (unsigned int)freq) == 0) { \
            do {                                     \
        } while (0)

#define soc_func_trace_low_freq_cnt_end() \
    }                                    \
    freqtimes++;                         \
    }                                    \
    while (0)

/* Function trace log for user input low frequency time
 * CNcomment: �������뽵Ƶ��ӡ��ʱ��
 */
#define soc_dbg_func_trace_low_freq_time(time)                               \
    do {                                                                    \
        static td_u32 last_time = 0;                                        \
        td_u32 curr_time;                                                   \
        curr_time = log_get_time_ms();                                      \
        if (time <= (curr_time - last_time)) {                              \
            soc_log_dbg(" =TRACE freq=%d,freqtimes=%d=\n", time, curr_time); \
            last_time = curr_time;                                          \
        }                                                                   \
    } while (0)

/* function trace log for user add extra print log as need
 * CNcomment: �û������������Ӷ���Ĵ�ӡ
 * need use soc_func_trace_low_freq_time_end at the same time, to repesent the addtional prints is the end
 * CNcomment: ��Ҫͬʱʹ��soc_func_trace_low_freq_time_end�������������ӵĴ�ӡ��ֹ
 */
#define soc_func_trace_low_freq_time_begin(time)     \
    do {                                            \
        static td_u32 last_time = 0;                \
        td_u32 curr_time;                           \
        curr_time = log_get_time_ms();              \
        if (time <= (curr_time - last_time)) {      \
            do {                                    \
        } while (0)

#define soc_func_trace_low_freq_time_end()   \
    last_time = curr_time;                  \
    }                                       \
    }                                       \
    while (0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SOC_LOG_H__ */
