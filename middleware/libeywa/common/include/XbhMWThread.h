#ifndef __XBH_MW_THREAD_H__
#define __XBH_MW_THREAD_H__

#include <pthread.h>
#include <unistd.h>
#include "XbhType.h"

/*
 * XbhMWThread Event callback. Used for listen running state.
 */
class XbhMWThread;
class IXbhMWThreadEvt
{
public:
    virtual void onXbhMWThreadExit(XbhMWThread* pHiMWThread) = 0;
    virtual ~IXbhMWThreadEvt(){}
};

/*v
 * Base XbhMWThread class.  The implementation is system-dependent.
 */
class XbhMWThread
{
public:
    typedef void (*XbhMWThreadFunc)(const void* arg);

    typedef struct {
        volatile bool wait;   /* enable or disable XbhMWThread */
        volatile bool once;   /* select loop once or always */
        volatile bool exit;   /* exit and recovery resource */
    } XbhMWThreadStat;

    typedef enum {
        ONCE   = 0x1 << 0,  /* just loop once */
        REPEAT = 0x1 << 1,  /* repeat loop  */
        WAIT   = 0x1 << 2,  /* wait for resume,only valid at first running */
    } XbhMWThreadFlag;
public:
    XbhMWThread(XbhMWThreadFunc func = XBH_NULL, const void* arg = XBH_NULL, IXbhMWThreadEvt* pEvt = XBH_NULL);
    virtual ~XbhMWThread();

    void run(unsigned int flag = ONCE);
    void suspend();
    void resume();
    void exit();/* quit immediately */
    void exitAndJoin();

protected:
    virtual void run(const void* arg){((void)arg); exit();};
private:
    static void* loop(void* arg);
    void task();
private:
    XbhMWThreadFunc  mThreadFunc;
    const void*     mArg;
    IXbhMWThreadEvt* mEvt;
    pthread_t       mId;
    XbhMWThreadStat  mStat;
    pthread_mutex_t mLock;
    pthread_cond_t  mCond;
};
#endif /*__XBH_MW_THREAD_H__*/

