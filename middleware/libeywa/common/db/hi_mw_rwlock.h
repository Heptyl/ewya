/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* create:
*/

#ifndef __HI_MW_RWLOCK_H__
#define __HI_MW_RWLOCK_H__

#include <pthread.h>
/* *************************************************************************** */
class HiRWLock {
public:
    HiRWLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_init(&mRwlock, NULL);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_init fail, ret:0x%x", s32Ret);
    }
    ~HiRWLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_destroy(&mRwlock);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_destroy fail, ret:0x%x", s32Ret);
    }

    // lock or unlock the rwlock
    HI_S32 rLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_rdlock(&mRwlock);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_rdlock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }
    HI_S32 wLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_wrlock(&mRwlock);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_wrlock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }
    HI_S32 unlock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_unlock(&mRwlock);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_unlock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }

    // lock if possible; returns 0 on success, error otherwise
    HI_S32 TryRLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_tryrdlock(&mRwlock);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_tryrdlock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }
    HI_S32 TryWLock()
    {
        HI_S32 s32Ret;
        s32Ret = pthread_rwlock_trywrlock(&mRwlock);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_rwlock_trywrlock fail, ret:0x%x", s32Ret);
        return s32Ret;
    }

    // Manages the wrlock automatically. It'll be locked when Autolock is
    // constructed and released when Autolock goes out of scope.
    class AutoRLock {
    public:
        inline AutoRLock(HiRWLock &lock):mLock(lock)
        {
            mLock.rLock();
        }
        inline AutoRLock(HiRWLock *lock):mLock(*lock)
        {
            mLock.rLock();
        }
        inline ~AutoRLock()
        {
            mLock.unlock();
        }

    private:
        HiRWLock &mLock;
    };

    class AutoWLock {
    public:
        inline AutoWLock(HiRWLock &lock):mLock(lock)
        {
            mLock.wLock();
        }
        inline AutoWLock(HiRWLock *lock):mLock(*lock)
        {
            mLock.wLock();
        }
        inline ~AutoWLock()
        {
            mLock.unlock();
        }

    private:
        HiRWLock &mLock;
    };

private:
    // A HiRWLock cannot be copied
    HiRWLock(const HiRWLock &);
    HiRWLock &operator=(const HiRWLock &);
    pthread_rwlock_t mRwlock;
};

#endif


