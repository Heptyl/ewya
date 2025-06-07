/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* create:
*/

#ifndef __HI_MW_LOG_H__
#define __HI_MW_LOG_H__

#include <string>

#include "hi_type.h"

#include "utils/Log.h"  // Linux version also uses log system of Android.

// using namespace std;
/*
 * atv log priority values, in ascending priority order.
 */
enum ATV_LOG_PRIO_E {
    ATV_LOG_UNKNOWN = 0,
    ATV_LOG_DEFAULT,
    ATV_LOG_VERBOSE,
    ATV_LOG_DEBUG,
    ATV_LOG_INFO,
    ATV_LOG_WARN,
    ATV_LOG_ERROR,
    ATV_LOG_FATAL,
    ATV_LOG_BUTT,
};

#ifndef NDEBUG
#define NDEBUG
#endif
#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

/*
 * Simplified macro to send a verbose log message using the current LOG_TAG.
 */
#ifndef LOGV
#if LOG_NDEBUG
#define LOGV(...) ((void)0)
#else
#define LOGV(...) ((void)LOG(ATV_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send a debug log message using the current LOG_TAG.
 */
#ifndef LOGD
#if LOG_NDEBUG
#define LOGD(...) ((void)0)
#else
#define LOGD(...) ((void)LOG(ATV_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send an info log message using the current LOG_TAG.
 */
#ifndef LOGI
#if LOG_NDEBUG
#define LOGI(...) ((void)0)
#else
#define LOGI(...) ((void)LOG(ATV_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send a warning log message using the current LOG_TAG.
 */
#ifndef LOGW
#if LOG_NDEBUG
#define LOGW(...) ((void)0)
#else
#define LOGW(...) ((void)LOG(ATV_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif
#endif
/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef LOGE
#if LOG_NDEBUG
#define LOGE(...) ((void)0)
#else
#define LOGE(...) ((void)LOG(ATV_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef LOGF
#if LOG_NDEBUG
#define LOGF(...) ((void)0)
#else
#define LOGF(...) ((void)LOG(ATV_LOG_FATAL, LOG_TAG, __VA_ARGS__))
#endif
#endif

/*
 * Basic log message macro.
 *
 * Example: LOG(LOG_WARN, NULL, "Failed with error %d", errno);
 *
 * The second argument may be NULL or "" to indicate the "global" tag.
 */
#ifndef LOG
#define LOG(priority, tag, ...) \
    atv_printLog(priority, tag, __VA_ARGS__)
#endif

/*
 * ===========================================================================
 *
 * The stuff in the rest of this file should not be used directly.
 */
#define atv_printLog(prio, tag, fmt...) \
    HiLog::PrintLog(prio, tag, __FUNCTION__, __LINE__, fmt)

class HiLog {
public:
    /* 打印接口 */
    static HI_S32 PrintLog(ATV_LOG_PRIO_E prio, const HI_CHAR *tag, const HI_CHAR *func, HI_U32 line,
                           const HI_CHAR *fmt, ...);

    /* 低于参数 enPrintLevel 的级别将不打印 */
    static HI_S32 SetPrintLevel(ATV_LOG_PRIO_E printLevel = ATV_LOG_ERROR);

    /* 获取mw层的打印同步到api接口 */
    static HI_S32 GetPrintLevel(ATV_LOG_PRIO_E &printLevel);

    /* 打开打印文件 */
    static HI_S32 OpenLogFile(const std::string &file);

private:
    static HI_VOID AndroidPrintLog(ATV_LOG_PRIO_E prio, const HI_CHAR *tag, const HI_CHAR *func,
                                   HI_U32 line, const HI_CHAR *buf);
private:
    static ATV_LOG_PRIO_E mPrintLevel;
    static HI_S32 mFd;
};

#endif

