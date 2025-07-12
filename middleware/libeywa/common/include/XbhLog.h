#ifndef XBH_LOG_H
#define XBH_LOG_H

#include <utils/Log.h>
#include "XbhType.h"
#include <android/log.h>

enum XBH_LOG_PRIO_E
{
    XBH_LOG_ALL = 0,
    XBH_LOG_VERBOSE,
    XBH_LOG_DEBUG,
    XBH_LOG_INFO,
    XBH_LOG_WARN,
    XBH_LOG_ERROR,
    XBH_LOG_FATAL,
    XBH_LOG_BUTT,
    XBH_LOG_ASSERT,
} ;

// 日志级别字符串映射,日志格式为DEBUG:0,INFO:1,WARN:2，ERROR:3....对应LOG_PRIO_STR[XBH_LOG_PRIO_E]
static const char* LOG_PRIO_STR[] = {
    "-1",//ALL
    "-1",//VERBOSE
    "0",//DEBUG
    "1",//INFO
    "2",//WARN
    "3",//ERROR
    "-1",//FATAL
    "-1",//BUTT
    "4",//ASSERT
};

/*
 * Simplified macro to send a verbose log message using the current LOG_TAG.
 */
#ifndef XLOGV
#if LOG_NDEBUG
#define XLOGV(...)   ((void)0)
#else
#define XLOGV(...) ((void)XLOG(XBH_LOG_VERBOSE, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send a debug log message using the current LOG_TAG.
 */
#ifndef XLOGD
#if LOG_NDEBUG
#define XLOGD(...)   ((void)0)
#else
#define XLOGD(...) ((void)XLOG(XBH_LOG_DEBUG, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send an info log message using the current LOG_TAG.
 */
#ifndef XLOGI
#if LOG_NDEBUG
#define XLOGI(...)   ((void)0)
#else
#define XLOGI(...) ((void)XLOG(XBH_LOG_INFO, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send a warning log message using the current LOG_TAG.
 */
#ifndef XLOGW
#if LOG_NDEBUG
#define XLOGW(...)   ((void)0)
#else
#define XLOGW(...) ((void)XLOG(XBH_LOG_WARN, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef XLOGE
#if LOG_NDEBUG
#define XLOGE(...)   ((void)0)
#else
#define XLOGE(...) ((void)XLOG(XBH_LOG_ERROR, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef XLOGF
#if LOG_NDEBUG
#define XLOGF(...)   ((void)0)
#else
#define XLOGF(...) ((void)XLOG(XBH_LOG_FATAL, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef XLOGA
#if LOG_NDEBUG
#define XLOGA(...)   ((void)0)
#else
#define XLOGA(...) ((void)XLOA(XBH_LOG_ASSERT, XBH_LOG_TAG, __VA_ARGS__))
#endif
#endif

#ifndef XLOG
#define XLOG(priority, tag, ...) \
    xbh_printLog(priority, tag, __VA_ARGS__)
#endif

/*
 * ===========================================================================
 *
 * The stuff in the rest of this file should not be used directly.
 */
#define xbh_printLog(prio, tag, fmt...) \
    XbhLog::PrintLog(prio, tag, __FILE__, __FUNCTION__, __LINE__, fmt)

#define RET_CHECK_DEBUG_LOG(s32Ret, ...) \
    do {                                 \
        if (s32Ret == XBH_SUCCESS) {         \
            XLOGD(__VA_ARGS__);           \
        } else {                         \
            XLOGE(__VA_ARGS__);           \
        }                                \
    } while (0)

#define RET_CHECK_ERR_LOG(s32Ret, ...) \
    do {                               \
        if (s32Ret != XBH_SUCCESS) {       \
            XLOGE(__VA_ARGS__);         \
        }                              \
    } while (0)

#define RET_CODE_CHECK(s32Ret, rtnFlg) \
    do {                           \
        if (XBH_SUCCESS != s32Ret){    \
            XLOGE("call failed");   \
            if(rtnFlg){            \
                return s32Ret;     \
            }                      \
            s32Ret = XBH_SUCCESS;      \
        }                          \
    } while (0)

#define RET_CODE_CHECK_LOG(s32Ret, rtnFlg, ...)\
    do {                                       \
        if (XBH_SUCCESS != s32Ret){                \
            XLOGE(__VA_ARGS__);                 \
            if(rtnFlg){                        \
                return s32Ret;                 \
            }                                  \
            s32Ret = XBH_SUCCESS;                  \
        }                                      \
    } while(0)

class XbhLog
{
public:
    static void init();
    /* 打印接口 */
    static XBH_S32 PrintLog(XBH_LOG_PRIO_E prio, const XBH_CHAR *tag, const XBH_CHAR *file, const XBH_CHAR *func, XBH_U32 line, const XBH_CHAR *fmt, ...);
    /* 低于参数 enPrintLevel 的级别将不打印 */
    static XBH_S32 setPrintLevel(XBH_LOG_PRIO_E printLevel = XBH_LOG_ERROR);
    /* 获取mw层的打印同步到api接口 */
    static XBH_S32 getPrintLevel(XBH_LOG_PRIO_E &printLevel);

private:
    static XBH_VOID AndroidPrintLog(XBH_LOG_PRIO_E prio, const XBH_CHAR *tag, const XBH_CHAR *file, const XBH_CHAR *func, XBH_U32 line, const XBH_CHAR *buf);
    static XBH_LOG_PRIO_E mLogLevel;
};


#endif //XBH_LOG_H
