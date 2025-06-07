#ifndef XBH_WATCHDOG_TASK_H
#define XBH_WATCHDOG_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

class XbhWatchDogManager : public XbhMWThread
{
//---------------- custom device interface start ----------------

//---------------- custom device interface end ----------------

public:
    static XbhWatchDogManager* getInstance();
    ~XbhWatchDogManager();

private:
    void run(const void* arg);
    XbhWatchDogManager();

    static XbhWatchDogManager *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_WATCHDOG_TASK_H
