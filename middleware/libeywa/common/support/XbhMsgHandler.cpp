#include "XbhMsgHandler.h"

XbhMsgHandler::XbhMsgHandler()
{
    MsgPublisher().RegisterHandler(this);
}

XbhMsgHandler::~XbhMsgHandler()
{
    MsgPublisher().UnregisterHandler(this);
}

XBH_BOOL XbhMsgHandler::HandleMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request,
                                android::Parcel *notifyReply)
{
    XBH_UNUSED(msgType);
    XBH_UNUSED(args);
    XBH_UNUSED(size);
    return XBH_FALSE;
}

