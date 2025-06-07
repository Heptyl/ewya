/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* create:
*/

#ifndef __HI_MW_MUTEX_H__
#define __HI_MW_MUTEX_H__

#include <pthread.h>

#include "hi_mw_common.h"
#include "hi_type.h"

/* *************************************************************************** */
/*
 * Simple mutex class.  The implementation is system-dependent.
 *
 * The mutex must be unlocked by the thread that locked it.  They are not
 * recursive, i.e. the same thread can't lock it multiple times.
 */
#ifndef HI_UNUSED
#define HI_UNUSED(x) ((void)x)
#endif

class HiMutex {
public:
    HiMutex()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_mutex_init(&mMutex, NULL);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_init fail, ret:0x%x", s32Ret);
    }
    HiMutex(const char *name)
    {
        HI_S32 s32Ret;
        HI_UNUSED(name);
        s32Ret = pthread_mutex_init(&mMutex, NULL);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_init fail, ret:0x%x", s32Ret);
    }
    ~HiMutex()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_mutex_destroy(&mMutex);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_destroy fail, ret:0x%x", s32Ret);
    }

    // lock or unlock the mutex
    HI_S32 lock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_mutex_lock(&mMutex);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_lock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }
    void unlock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_mutex_unlock(&mMutex);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_unlock fail, ret:0x%x", s32Ret);
    }

    // lock if possible; returns 0 on success, error otherwise
    HI_S32 tryLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_mutex_trylock(&mMutex);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_mutex_trylock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }

    // Manages the mutex automatically. It'll be locked when Autolock is
    // constructed and released when Autolock goes out of scope.
    class Autolock {
    public:
        inline Autolock(HiMutex &mutex):mLock(mutex)
        {
            mLock.lock();
        }
        inline Autolock(HiMutex *mutex):mLock(*mutex)
        {
            mLock.lock();
        }
        inline ~Autolock()
        {
            mLock.unlock();
        }

    private:
        HiMutex &mLock;
    };

private:
    // A mutex cannot be copied
    HiMutex(const HiMutex &);
    HiMutex &operator=(const HiMutex &);
    pthread_mutex_t mMutex;
};

#endif

