#ifndef XBH_MUTEX_H
#define XBH_MUTEX_H

#include <log/log.h>
#include <pthread.h>
#include "XbhType.h"

class XbhMutex
{
public:
    XbhMutex()
    {
        int s32Ret = XBH_SUCCESS;
        s32Ret = pthread_mutex_init(&mMutex, NULL);
        if (s32Ret != XBH_SUCCESS)
        {
            ALOG(LOG_ERROR, "xbh_mw@XbhMutex", "XbhMutex() ret: %d\n", s32Ret);
        }
    }

    ~XbhMutex()
    {
        int s32Ret = XBH_SUCCESS;
        s32Ret = pthread_mutex_init(&mMutex, NULL);
        if (s32Ret != XBH_SUCCESS)
        {
            ALOG(LOG_DEBUG, "xbh_mw@XbhMutex", "~XbhMutex() ret: %d\n", s32Ret);
        }
    }

    int lock()
    {
        int s32Ret = XBH_SUCCESS;
        s32Ret = pthread_mutex_lock(&mMutex);
        if (s32Ret != XBH_SUCCESS)
        {
            ALOG(LOG_DEBUG, "xbh_mw@XbhMutex", "XbhMutex::lock() ret: %d\n", s32Ret);
        }
        return s32Ret;
    }

    void unlock()
    {
        int s32Ret = XBH_SUCCESS;
        s32Ret = pthread_mutex_unlock(&mMutex);
        if (s32Ret != XBH_SUCCESS)
        {
            ALOG(LOG_DEBUG, "xbh_mw@XbhMutex", "XbhMutex::unlock() ret: %d\n", s32Ret);
        }
    }

    class Autolock
    {
        public:
            inline Autolock(XbhMutex& mutex) : mLock(mutex) { mLock.lock(); }
            inline Autolock(XbhMutex* mutex) : mLock(*mutex) { mLock.lock(); }
            inline ~Autolock() { mLock.unlock(); }
        private:
            XbhMutex& mLock;
    };

private:
    pthread_mutex_t mMutex;
};



#endif //XBH_MUTEX_H
