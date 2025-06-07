#ifndef __XBH_MSG_HUB_H__
#define __XBH_MSG_HUB_H__

#include <set>
#include <queue>
#include <pthread.h>
#include "XbhMWThread.h"
#include "IMsgHandler.h"
#include "XbhType.h"
/**
 * A message processing center that receives the registration of various message consumers and the injection and distribution of new messages.
 */
class XbhMsgHub : public XbhMWThread, public IMsgHandler::IMsgPublisher {
public:
    XbhMsgHub();
    XBH_VOID Init();
    virtual ~XbhMsgHub();

    XBH_VOID RegisterHandler(IMsgHandler *pMsgHandler);
    XBH_VOID UnregisterHandler(IMsgHandler *pMsgHandler);
    XBH_BOOL SendMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL);
    XBH_BOOL PostMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL);

private:
    /**
     *  Message content description
     */
    struct InnerMsg {
        XBH_U32 msgType;
        XBH_VOID *args;
        XBH_U32 size;
        android::Parcel *request;
        android::Parcel *notifyReply;
    };

    typedef std::set<IMsgHandler *> HandlerSet;
    typedef HandlerSet::iterator HandlerSetIter;
    typedef std::queue<InnerMsg> MsgQueue;

    HandlerSet m_handlerSet; /* Message Consumer Collection */
    MsgQueue m_msgQueue;     /* Post asynchronous message queue */

    pthread_mutex_t m_handlerSetLock; /* Consumer Collection Lock */
    pthread_mutex_t m_msgQueueLock;   /* Post Message Queue Lock */
    XBH_BOOL m_InitFlag;               /* XbhMsgHub initialization flag */
    XBH_BOOL MsgReader();
    void run(const void* arg);
};
#endif  // !defined(__XBH_MSG_HUB_H__)
