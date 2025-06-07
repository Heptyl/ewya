/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_EVENT_H__
#define __HI_MW_EVENT_H__

#include <deque>
#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#include "hi_type.h"
#include "hi_mw_log.h"
#include "hi_mw_common.h"

template<class Param>
class Event {
public:
    Event();
    ~Event();

    /* send event */
    HI_S32 send(const Param &param, HI_BOOL pushFront = HI_FALSE);

    /* wait event with timeout */
    HI_S32 wait(Param *param, HI_U32 timeOutMs);

    /* wait event forever */
    HI_S32 wait(Param *param);

    /* clear event buffer */
    HI_S32 clear();

private:
    std::deque<Param> mQueueParam;
    pthread_mutex_t mMutEvent;
    pthread_cond_t mCondEvent;
};

template<class Param> Event<Param>::Event()
{
    HI_S32 s32Ret;
    pthread_condattr_t attr;
    s32Ret = pthread_condattr_init(&attr);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_condattr_init fail, ret:0x%x", s32Ret);

    s32Ret = pthread_mutex_init(&mMutEvent, NULL);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_init fail, ret:0x%x", s32Ret);

    s32Ret = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_condattr_setclock fail, ret:0x%x", s32Ret);

    s32Ret = pthread_cond_init(&mCondEvent, &attr);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    s32Ret = pthread_condattr_destroy(&attr);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_condattr_destroy fail, ret:0x%x", s32Ret);
}

template<class Param> Event<Param>::~Event()
{
    HI_S32 s32Ret;
    s32Ret = pthread_mutex_destroy(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_destroy fail, ret:0x%x", s32Ret);

    s32Ret = pthread_cond_destroy(&mCondEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_destroy fail, ret:0x%x", s32Ret);

    mQueueParam.erase(mQueueParam.begin(), mQueueParam.end());
}

template<class Param>
HI_S32 Event<Param>::send(const Param &param, HI_BOOL pushFront)
{
    HI_S32 s32Ret;
    s32Ret = pthread_mutex_lock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_lock fail, ret:0x%x", s32Ret);

    if (pushFront == HI_FALSE) {
        mQueueParam.push_back(param);
    } else {
        mQueueParam.push_front(param);
    }

    s32Ret = pthread_cond_signal(&mCondEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_signal fail, ret:0x%x", s32Ret);

    s32Ret = pthread_mutex_unlock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_unlock fail, ret:0x%x", s32Ret);

    return s32Ret;
}

template<class Param>
HI_S32 Event<Param>::wait(Param *param, HI_U32 timeOutMs)
{
    HI_S32 s32Ret;

    s32Ret = pthread_mutex_lock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_lock fail, ret:0x%x", s32Ret);

    if (!mQueueParam.empty()) {
        *param = mQueueParam.front();
        mQueueParam.pop_front();
    } else if (timeOutMs == 0) {
        s32Ret = pthread_mutex_unlock(&mMutEvent);
        if (s32Ret != HI_SUCCESS) {
            LOGE("pthread_mutex_unlock fail, ret:0x%x", s32Ret);
            return s32Ret;
        }
        return HI_FAILURE;
    } else {
        struct timespec ts;

        /* 获取系统的相对时间:从单板启动开始计时的时间，不受系统时间更新而变化 */
        s32Ret = clock_gettime(CLOCK_MONOTONIC, &ts);
        if (s32Ret != HI_SUCCESS) {
            LOGE("clock_gettime fail, ret:0x%x", s32Ret);
            s32Ret = pthread_mutex_unlock(&mMutEvent);
            RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_unlock fail, ret:0x%x", s32Ret);
            return s32Ret;
        }
        ts.tv_sec = ts.tv_sec + timeOutMs / 1000; /* 1000ms */
        ts.tv_nsec = ts.tv_nsec + (timeOutMs % 1000) * 1000 * 1000; /* 1000ms */
        if (ts.tv_nsec > 1000000000) { /* time is > 1000000000 */
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000; /* time is 1000000000 */
        }
        s32Ret = pthread_cond_timedwait(&mCondEvent, &mMutEvent, &ts);
        if (s32Ret == HI_SUCCESS && !mQueueParam.empty()) {
            *param = mQueueParam.front();
            mQueueParam.pop_front();
        }
    }

    s32Ret = pthread_mutex_unlock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_unlock fail, ret:0x%x", s32Ret);

    return s32Ret;
}

template<class Param>
HI_S32 Event<Param>::wait(Param *param)
{
    HI_S32 s32Ret;
    s32Ret = pthread_mutex_lock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_lock fail, ret:0x%x", s32Ret);

    if (!mQueueParam.empty()) {
        *param = mQueueParam.front();
        mQueueParam.pop_front();
    } else {
        s32Ret = pthread_cond_wait(&mCondEvent, &mMutEvent);
        if (s32Ret == 0) {
            *param = mQueueParam.front();
            mQueueParam.pop_front();
        } else {
            LOGE("pthread_cond_wait fail, ret:0x%x", s32Ret);
        }
    }

    s32Ret = pthread_mutex_unlock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_unlock fail, ret:0x%x", s32Ret);

    return s32Ret;
}

template<class Param>
HI_S32 Event<Param>::clear()
{
    HI_S32 s32Ret;
    s32Ret = pthread_mutex_lock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_lock fail, ret:0x%x", s32Ret);

    mQueueParam.clear();

    s32Ret = pthread_mutex_unlock(&mMutEvent);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_unlock fail, ret:0x%x", s32Ret);

    return s32Ret;
}

#endif

