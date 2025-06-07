#ifndef __XBH_MSG_QUEUE_OLD_H__
#define __XBH_MSG_QUEUE_OLD_H__

#include <set>
#include <queue>
#include <pthread.h>
#include "XbhMWThread.h"
#include "XbhType.h"

class XbhMsgQueue_old {
public:
    XBH_BOOL postMsg(XBH_U32 msgType, const XBH_VOID* args, XBH_U32 size, XBH_VOID* point);
    XBH_BOOL getMsg(XBH_U32* msgType, XBH_VOID* args, XBH_U32* size, XBH_VOID* point);

    XbhMsgQueue_old();
    ~XbhMsgQueue_old();
private:
    struct InnerMsg {
        XBH_U32 msgType;
        XBH_VOID *args;
        XBH_U32 size;
    };
    pthread_mutex_t m_msgQueueLock;   /* Post Message Queue Lock */
    typedef std::queue<InnerMsg> MsgQueue;
    MsgQueue m_msgQueue;
};

#endif
