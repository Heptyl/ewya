#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@xbh_mw_thread.cpp"

#include "XbhMWThread.h"

XbhMWThread::XbhMWThread(XbhMWThreadFunc func, const void* arg, IXbhMWThreadEvt* pEvt)
    : mThreadFunc(func), mArg(arg), mEvt(pEvt), mId(0)
{
    mStat.wait = false;
    mStat.once = false;
    mStat.exit = true;
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_init(&mLock, XBH_NULL);

    s32Ret = pthread_cond_init(&mCond, XBH_NULL);
}

XbhMWThread::~XbhMWThread()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    if (!mStat.exit)
    {
        s32Ret = pthread_mutex_unlock(&mLock);

        s32Ret = pthread_join(mId, XBH_NULL);
    }
    else
    {
        s32Ret = pthread_mutex_unlock(&mLock);
    }

    s32Ret = pthread_mutex_destroy(&mLock);

    s32Ret = pthread_cond_destroy(&mCond);
}

void XbhMWThread::exit()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    if (!mStat.exit)
    {
        mStat.exit = true;
        mStat.wait = false;
        s32Ret = pthread_cond_signal(&mCond);
    }
    s32Ret = pthread_mutex_unlock(&mLock);
    //if (mEvt) mEvt->onXbhMWThreadExit(this);
}

void XbhMWThread::exitAndJoin()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    if (!mStat.exit)
    {
       mStat.exit = true;
       mStat.wait = false;

       s32Ret = pthread_cond_signal(&mCond);

       s32Ret = pthread_mutex_unlock(&mLock);

       s32Ret = pthread_join(mId, XBH_NULL);
       return;
    }

    s32Ret = pthread_mutex_unlock(&mLock);

}

void XbhMWThread::run(unsigned int flag)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    if (!mStat.exit)
    {
        mStat.exit = true;
        mStat.wait = false;

        s32Ret = pthread_cond_signal(&mCond);

        s32Ret = pthread_mutex_unlock(&mLock);

        s32Ret = pthread_join(mId, XBH_NULL);

        s32Ret = pthread_mutex_lock(&mLock);
    }

    mStat.exit = false;
    mStat.wait = true;
    mStat.once = flag & ONCE;

    if (~flag & WAIT)
    {
        mStat.wait = false;
    }
    s32Ret = pthread_mutex_unlock(&mLock);

    s32Ret = pthread_create(&mId, XBH_NULL, loop, this);
}

void XbhMWThread::suspend()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    if (!mStat.exit)
    {
        if (mStat.wait)
        {
            s32Ret = pthread_mutex_unlock(&mLock);
            return;
        }

        mStat.wait = true;
    }
    s32Ret = pthread_mutex_unlock(&mLock);
}

void XbhMWThread::resume()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    if (!mStat.exit)
    {
        if (!mStat.wait)
        {
            s32Ret = pthread_mutex_unlock(&mLock);
            return;
        }

        mStat.wait = false;
        s32Ret = pthread_cond_signal(&mCond);
    }
    s32Ret = pthread_mutex_unlock(&mLock);
}

void* XbhMWThread::loop(void* arg)
{
    XbhMWThread* ptr = reinterpret_cast<XbhMWThread*>(arg);

    while (!ptr->mStat.exit)
    {
        ptr->task();
    }

    if (ptr->mEvt)
    {
        ptr->mEvt->onXbhMWThreadExit(ptr);
    }

    return XBH_NULL;
}

void XbhMWThread::task()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = pthread_mutex_lock(&mLock);

    while (mStat.wait)
    {
        s32Ret = pthread_cond_wait(&mCond, &mLock);
    }

    if (mStat.exit)
    {
        s32Ret = pthread_mutex_unlock(&mLock);
        return;
    }

    mStat.wait = mStat.once;
    s32Ret = pthread_mutex_unlock(&mLock);

    if (!mThreadFunc)
    {
        return run(mArg);
    }

    return mThreadFunc(mArg);
}
