#ifndef __XBH_FOLLOW_MANAGER_H__
#define __XBH_FOLLOW_MANAGER_H__

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMsgQueue.h"

class XbhFollowManager : public XbhMWThread, public XbhMsgQueue
{
public:
    static XbhFollowManager* getInstance();
    /**
     * 切换逻辑，根据TABLE数据时序进行USB,GPIO等的切换
    */
    XBH_S32 setFollowPort(XBH_SOURCE_E u32Source);
    ~XbhFollowManager();

private:
    void run(const void* arg);
    XbhFollowManager();
    static XbhFollowManager *mInstance;
    static XbhMutex mLock;

    XBH_SOURCE_E mCurrentSrc;
    XBH_SOURCE_E mChangeToSrc;
};

#endif //__XBH_FOLLOW_MANAGER_H__
