#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhWatchDogManager"
#include "XbhLog.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#include "XbhWatchDogManager.h"
#include "XbhService.h"

XbhWatchDogManager*            XbhWatchDogManager::mInstance = NULL;
XbhMutex                       XbhWatchDogManager::mLock;

//---------------- custom device interface start ----------------
void XbhWatchDogManager::run(const void* arg)
{
    XBH_BOOL bManualModeEnable = 1;
    XbhService::getPlatformInterface()->getWatchDogManualModeEnable(&bManualModeEnable);
    if (bManualModeEnable == 0) {
        XBH_U32 timeout = 0;
        XBH_U32 interval = 0;
        XbhService::getPlatformInterface()->getWatchDogTimeOut(&timeout);
        interval = timeout / 2;
        if (interval <= 0) {
            interval = 1;
        }
        interval *= 1000000;
        XbhService::getPlatformInterface()->watchDogPing();
        usleep(interval);
    } else {
        usleep(5*1000*1000);
    }
}

XbhWatchDogManager::XbhWatchDogManager()
{
    XLOGD(" begin ");
    XbhService::getPlatformInterface()->setWatchDogTimeOut(22);

    // 设置喂狗模式为自动喂狗模式
    XbhService::getPlatformInterface()->setWatchDogManualModeEnable(0);

    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhWatchDogManager::~XbhWatchDogManager()
{

}

//---------------- custom device interface end ----------------


XbhWatchDogManager* XbhWatchDogManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhWatchDogManager();
    }

    return mInstance;
}

