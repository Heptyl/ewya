#if !defined(__IMsgHandler_H__)
#define __IMsgHandler_H__
#include "XbhType.h"
#include "binder/Parcel.h"

/**
 * Message mechanism interface class that defines message operation methods and module business scope
 */
class IMsgHandler {
public:
    class IMsgPublisher {
    public:
        virtual ~IMsgPublisher(){}

        virtual XBH_VOID RegisterHandler(IMsgHandler *pMsgHandler) = 0;
        virtual XBH_VOID UnregisterHandler(IMsgHandler *pMsgHandler) = 0;
        virtual XBH_BOOL SendMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size,
            const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL) = 0;
        virtual XBH_BOOL PostMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size,
            const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL) = 0;
    };

    virtual XBH_BOOL HandleMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size,
        const android::Parcel *request = XBH_NULL, android::Parcel *notifyReply = XBH_NULL) = 0;
    virtual ~IMsgHandler(){}
};

IMsgHandler::IMsgPublisher &MsgPublisher();

#endif // !defined(__IMsgHandler_H__)
