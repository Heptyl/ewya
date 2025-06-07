#ifndef __XBH_MSG_HANDLER_H__
#define __XBH_MSG_HANDLER_H__

#include "IMsgHandler.h"

/**
 * Message consumer parent class, which implements automatic registration of message consumers
 */
class XbhMsgHandler : public IMsgHandler {
public:
    virtual XBH_BOOL HandleMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size,
        const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL);

protected:
    XbhMsgHandler();
    virtual ~XbhMsgHandler();
};

#endif // !defined(__XBH_MSG_HANDLER_H__)
