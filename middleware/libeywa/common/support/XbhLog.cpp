#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>

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
