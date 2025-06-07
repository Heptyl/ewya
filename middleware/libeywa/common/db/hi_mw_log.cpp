/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/


#include "hi_mw_log.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <android/log.h>
#include "securec.h"

#define LOG_BUF_SIZE 1024

ATV_LOG_PRIO_E HiLog::mPrintLevel = ATV_LOG_DEBUG;
HI_S32 HiLog::mFd = 0;

HI_VOID HiLog::AndroidPrintLog(ATV_LOG_PRIO_E prio, const HI_CHAR *tag, const HI_CHAR *func,
                               HI_U32 line, const HI_CHAR *buf)
{
    switch (prio) {
        case ATV_LOG_FATAL:
            android_printLog(ANDROID_LOG_FATAL, tag, "\033[31m[%s:%d] %s\033[0m", func, line, buf);  // red
            break;
        case ATV_LOG_ERROR:
            android_printLog(ANDROID_LOG_ERROR, tag, "\033[31m[%s:%d] %s\033[0m", func, line, buf);  // red
            break;
        case ATV_LOG_WARN:
            android_printLog(ANDROID_LOG_WARN, tag, "\033[35m[%s:%d] %s\033[0m", func, line, buf);  // purple
            break;
        case ATV_LOG_INFO:
            android_printLog(ANDROID_LOG_INFO, tag, "\e[36m[%s:%d] %s\e[0m", func, line, buf);  // deep green
            break;
        case ATV_LOG_DEBUG:
            android_printLog(ANDROID_LOG_DEBUG, tag, "[%s:%d] %s", func, line, buf);
            break;
        case ATV_LOG_VERBOSE:
            android_printLog(ANDROID_LOG_VERBOSE, tag, "[%s:%d] %s", func, line, buf);
            break;
        case ATV_LOG_DEFAULT:
            android_printLog(ANDROID_LOG_DEFAULT, tag, "[%s:%d] %s", func, line, buf);
            break;
        case ATV_LOG_UNKNOWN:
        default:
            android_printLog(ANDROID_LOG_UNKNOWN, tag, "[%s:%d] %s", func, line, buf);  // auto
            break;
    }
}

HI_S32 HiLog::PrintLog(ATV_LOG_PRIO_E prio, const HI_CHAR *tag, const HI_CHAR *func, HI_U32 line,
                       const HI_CHAR *fmt, ...)
{
    if (tag == NULL || prio < mPrintLevel) {
        return HI_FAILURE;
    }

    HI_CHAR buf[LOG_BUF_SIZE] = { 0 };

    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf_s(buf, sizeof(buf), LOG_BUF_SIZE - 1, fmt, ap);
    if (ret == -1) {
        LOGE("vsnprintf_s failed, ret = %d", ret);
        va_end(ap);
        return HI_FAILURE;
    }
    va_end(ap);

    AndroidPrintLog(prio, tag, func, line, buf);

    return HI_SUCCESS;
}

HI_S32 HiLog::SetPrintLevel(ATV_LOG_PRIO_E printLevel)
{
    if (printLevel >= ATV_LOG_BUTT || printLevel < ATV_LOG_UNKNOWN) {
        return HI_FAILURE;
    }
    mPrintLevel = printLevel;

    return HI_SUCCESS;
}

HI_S32 HiLog::GetPrintLevel(ATV_LOG_PRIO_E &printLevel)
{
    printLevel = mPrintLevel;

    return HI_SUCCESS;
}

HI_S32 HiLog::OpenLogFile(const std::string &file)
{
    HI_CHAR acResolvedPath[PATH_MAX];
    errno_t rc = memset_s(&acResolvedPath, sizeof(acResolvedPath), 0, sizeof(acResolvedPath));
    if (rc != EOK) {
        LOGE("memset_s_func failed, rc = %d", rc);
        return HI_FAILURE;
    }

    realpath(file.c_str(), acResolvedPath);
    if (strlen(file.c_str()) > PATH_MAX - 1 || 0 == strlen(acResolvedPath)) {
        LOGE("file path:%s error!", file.c_str());
        return HI_FAILURE;
    }
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    mFd = open(acResolvedPath, O_WRONLY | O_CREAT, mode);
    if (mFd < 0) {
        LOGE("Error! %s open failed!", file.c_str());

        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

