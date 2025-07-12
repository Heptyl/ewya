#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>

#include "XbhLog.h"

#define LOG_BUF_SIZE 1024
XBH_LOG_PRIO_E XbhLog::mLogLevel = XBH_LOG_DEBUG;

/**
 * 有些编译器__FILE__会输出带绝对路径的字符串，比如：vendor/xbh/middleware/libeywa/logic/XbhService.cpp
 * 这里重新定义一个宏用来去掉绝对路径字符串
 */
#define XBH_FILE(x) strrchr(x,'/') ? strrchr(x,'/') + 1 : x

void XbhLog::init()
{
    XBH_CHAR logLevel[PROPERTY_VALUE_MAX] = {0};
    if (property_get("persist.vendor.xbh.hidl.loglevel", logLevel, "2") >= 0)
    {
        XBH_S32 level = atoi(logLevel);
        if (level >= XBH_LOG_ALL && level < XBH_LOG_BUTT)
        {
            mLogLevel = (XBH_LOG_PRIO_E)level;
        }
    }
    ALOGD("XbhLog::XbhLog() %d\n", mLogLevel);
}

XBH_VOID XbhLog::AndroidPrintLog(XBH_LOG_PRIO_E prio, const XBH_CHAR *tag, const XBH_CHAR *file, const XBH_CHAR *func, XBH_U32 line, const XBH_CHAR *buf)
{
//$XBH_PATCH_START
#ifdef TYPE_BOARD
#if (TYPE_BOARD == XMH610A)
    char timeStr[64];
    struct timeval tv;
    struct tm tm_time;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &tm_time);

    size_t timeLen = strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S", &tm_time);
    if (timeLen == 0) {
        strncpy(timeStr, "[time_error]", sizeof(timeStr)-1);
    }

    int timeAppend = snprintf(timeStr + timeLen, sizeof(timeStr) - timeLen, ".%03ld]", tv.tv_usec / 1000);
    if (timeAppend < 0 || timeAppend >= (int)(sizeof(timeStr) - timeLen)) {
        strncpy(timeStr, "[time_error]", sizeof(timeStr)-1);
    }

    pid_t pid = getpid();
    pid_t tid = (pid_t)syscall(SYS_gettid);
    char threadInfo[32];
    int threadInfoLen = snprintf(threadInfo, sizeof(threadInfo), "[%d:%d]", pid, tid);
    if (threadInfoLen < 0 || threadInfoLen >= (int)sizeof(threadInfo)) {
        strncpy(threadInfo, "[thread_error]", sizeof(threadInfo)-1);
    }

    const char* className = XBH_FILE(file);
    char simplifiedClassName[128];
    int classNameLen = snprintf(simplifiedClassName, sizeof(simplifiedClassName), "[%s:%s:%d]",
                               className, func, line);
    if (classNameLen < 0 || classNameLen >= (int)sizeof(simplifiedClassName)) {
        strncpy(simplifiedClassName, "[class_error]", sizeof(simplifiedClassName)-1);
    }

    char filteredBuf[LOG_BUF_SIZE] = {0};
    size_t i = 0;
    while (buf[i] != '\0' && i < sizeof(filteredBuf) - 1) {
        filteredBuf[i] = (buf[i] == '\n' || buf[i] == '\r') ? ' ' : buf[i];
        i++;
    }//去掉原始日志中的\n\r
    filteredBuf[i] = '\0';

    char formattedLog[LOG_BUF_SIZE];
    //对接中间件，eywa的日志都归类为调试日志在日志前面加[3]做标识。
    //日志级别字符串按中间件解析格式DEBUG为0、INFO为1.....
    int logLen = snprintf(formattedLog, sizeof(formattedLog), "[3]%s[%s][%s]%s%s[%s]",
             timeStr,
             LOG_PRIO_STR[prio],  // 日志级别字符串
             tag,                 // 模块名
             threadInfo,          // 线程信息
             simplifiedClassName, // 类名+行号
             filteredBuf);        // 原始日志内容
    if (logLen < 0 || logLen >= (int)sizeof(formattedLog)) {
        strncpy(formattedLog, "[log_format_error]", sizeof(formattedLog)-1);
    }
    switch (prio) {
    case XBH_LOG_FATAL:
        android_printLog(ANDROID_LOG_FATAL, tag, "%s", formattedLog);
        break;
    case XBH_LOG_ERROR:
        android_printLog(ANDROID_LOG_ERROR, tag, "%s",  formattedLog);
        break;
    case XBH_LOG_WARN:
        android_printLog(ANDROID_LOG_WARN, tag, "%s",  formattedLog);
        break;
    case XBH_LOG_INFO:
        android_printLog(ANDROID_LOG_INFO, tag, "%s",  formattedLog);
        break;
    case XBH_LOG_DEBUG:
        android_printLog(ANDROID_LOG_DEBUG, tag, "%s",  formattedLog);
        break;
    case XBH_LOG_VERBOSE:
        android_printLog(ANDROID_LOG_VERBOSE, tag, "%s",  formattedLog);
        break;
    case XBH_LOG_ASSERT:
        android_printLog(XBH_LOG_ASSERT, tag, "%s", formattedLog);
        break;
    default:
        android_printLog(ANDROID_LOG_UNKNOWN, tag, "%s",  formattedLog);
        break;
    }
    return;
#endif
#endif
//$XBH_PATCH_END
    switch (prio) {
        case XBH_LOG_FATAL:
            android_printLog(ANDROID_LOG_FATAL, tag, "\033[31m[%s:%s:%d] %s\033[0m", XBH_FILE(file), func, line, buf);  // red
            break;
        case XBH_LOG_ERROR:
            android_printLog(ANDROID_LOG_ERROR, tag, "\033[31m[%s:%s:%d] %s\033[0m", XBH_FILE(file), func, line, buf);  // red
            break;
        case XBH_LOG_WARN:
            android_printLog(ANDROID_LOG_WARN, tag, "\033[35m[%s:%s:%d] %s\033[0m", XBH_FILE(file), func, line, buf);  // purple
            break;
        case XBH_LOG_INFO:
            android_printLog(ANDROID_LOG_INFO, tag, "\e[36m[%s:%s:%d] %s\e[0m", XBH_FILE(file), func, line, buf);  // deep green
            break;
        case XBH_LOG_DEBUG:
            android_printLog(ANDROID_LOG_DEBUG, tag, "[%s:%s:%d] %s", XBH_FILE(file), func, line, buf);
            break;
        case XBH_LOG_VERBOSE:
            android_printLog(ANDROID_LOG_VERBOSE, tag, "[%s:%s:%d] %s", XBH_FILE(file), func, line, buf);
            break;
        default:
            android_printLog(ANDROID_LOG_UNKNOWN, tag, "[%s:%s:%d] %s", XBH_FILE(file), func, line, buf);  // auto
            break;
    }
}

XBH_S32 XbhLog::PrintLog(XBH_LOG_PRIO_E prio, const XBH_CHAR *tag, const XBH_CHAR *file, const XBH_CHAR *func, XBH_U32 line, const XBH_CHAR *fmt, ...)
{
    if (tag == NULL || prio < mLogLevel) {
        return XBH_FAILURE;
    }

    XBH_CHAR buf[LOG_BUF_SIZE] = { 0 };

    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    if (ret == -1) {
        XLOGE("vsnprintf failed, ret = %d", ret);
        va_end(ap);
        return XBH_FAILURE;
    }
    va_end(ap);

    AndroidPrintLog(prio, tag, file, func, line, buf);

    return XBH_SUCCESS;
}

XBH_S32 XbhLog::setPrintLevel(XBH_LOG_PRIO_E loglevel)
{
    XBH_CHAR logLevel[PROPERTY_VALUE_MAX] = {0};
    mLogLevel = loglevel;
    sprintf(logLevel, "%d", mLogLevel);
    property_set("persist.vendor.xbh.hidl.loglevel", logLevel);

    return XBH_SUCCESS;
}

XBH_S32 XbhLog::getPrintLevel(XBH_LOG_PRIO_E& loglevel)
{
    loglevel = mLogLevel;
    return XBH_SUCCESS;
}
