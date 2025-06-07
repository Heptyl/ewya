#define LOG_NDEBUG 0
#define XBH_LOG_TAG    "xbh_mw@XbhMsgHub"

#include "XbhMsgHub.h"
#include <string.h>
#include <stdlib.h>
#include "XbhLog.h"

XbhMsgHub::XbhMsgHub()
{
    m_InitFlag = XBH_FALSE;
}

XBH_VOID XbhMsgHub::Init()
{
    if (m_InitFlag == XBH_FALSE) {
        m_InitFlag = XBH_TRUE;
        XBH_S32 s32Ret;
        s32Ret = pthread_mutex_init(&m_handlerSetLock, 0);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
        s32Ret = pthread_mutex_init(&m_msgQueueLock, 0);
        RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
        XbhMWThread::run(XbhMWThread::REPEAT);
    }
}

XbhMsgHub::~XbhMsgHub()
{
    XBH_S32 s32Ret;
    s32Ret = pthread_mutex_destroy(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    s32Ret = pthread_mutex_destroy(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
}

void XbhMsgHub::run(const void* arg)
{
    MsgReader();
}

/**
 * The object that registers the actual message recipient must implement the message processing function handleMsg.
 */
XBH_VOID XbhMsgHub::RegisterHandler(IMsgHandler *pMsgHandler)
{
    XBH_S32 s32Ret;
    if (pMsgHandler == XBH_NULL) {
        return;
    }

    s32Ret = pthread_mutex_lock(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    m_handlerSet.insert(pMsgHandler);

    s32Ret = pthread_mutex_unlock(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
}

/**
 * Unregister the object of the actual message recipient.
 */
XBH_VOID XbhMsgHub::UnregisterHandler(IMsgHandler *pMsgHandler)
{
    XBH_S32 s32Ret;
    if (pMsgHandler == XBH_NULL) {
        return;
    }

    s32Ret = pthread_mutex_lock(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    m_handlerSet.erase(pMsgHandler);

    s32Ret = pthread_mutex_unlock(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
}

/**
 * Send synchronization message
 
 * Transfer Parcel data as follows:
 * Parcel tmpParcel;
 * Parcel *pParcel = &tmpParcel;
 * size_t tmpPos = tmpParcel.dataPosition();
 * Parcel outParcel;
 
 * Fix the CodeX HW_CBG_C_INT_MUL_ASSIGN
 * const HI_S64 mulValue = (s32StrNum + 1) * sizeof(HI_U16);
 * if (mulValue > HI_S32_MAX_VALUE) {
 *     LOGE("multiplication overflow..");
 *     return HI_FAILURE;
 * }
 * HI_S32 textSize = static_cast<HI_S32>(mulValue);
 * tmpParcel.writeInt32(textSize);
 * tmpParcel.write(u16Str, textSize);
 * tmpParcel.setDataPosition(tmpPos);
 * MsgPublisher().SendMsg(HI_TV_EVT_CC_ONGETTEXTSIZE, HI_NULL, 0, pParcel, &outParcel);
 * ps32Width = outParcel.readInt32();
 * ps32Heigth = outParcel.readInt32();

 * Transfer Universal Structure follows:
 * CcBlitInfo onBlitBufMsg = {};
 * onBlitBufMsg.srcX = pstSrcRect->x;
 * onBlitBufMsg.srcY = pstSrcRect->y;
 * onBlitBufMsg.srcWidth = pstSrcRect->width;
 * onBlitBufMsg.srcHeight = pstSrcRect->height;
 * onBlitBufMsg.desX = pstDestRect->x;
 * onBlitBufMsg.desY = pstDestRect->y;
 * onBlitBufMsg.desWidth = pstDestRect->width;
 * onBlitBufMsg.desHeight = pstDestRect->height;
 
 * MsgPublisher().SendMsg(HI_TV_EVT_CC_ONBLITBUF, &onBlitBufMsg, sizeof(onBlitBufMsg));
 */
 XBH_BOOL XbhMsgHub::SendMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request, android::Parcel *notifyReply)
{
    XBH_S32 s32Ret;
    if (m_handlerSet.empty()) {
        XLOGE("XbhMsgHub::SendMsg m_handlerSet is empty");
        return XBH_FALSE;
    }
    s32Ret = pthread_mutex_lock(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    HandlerSet set = m_handlerSet;
    XLOGD("XbhMsgHub::SendMsg step1, Msg type:%x", msgType);
    s32Ret = pthread_mutex_unlock(&m_handlerSetLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    for (HandlerSetIter it = set.begin(); it != set.end(); ++it) {
        if ((*it)->HandleMsg(msgType, args, size, request, notifyReply)) {
            break;
        }
    }
    XLOGD("XbhMsgHub::SendMsg end, Msg type:%x", msgType);
    return XBH_TRUE;
}

/**
 * Send asynchronous messages
 */
XBH_BOOL XbhMsgHub::PostMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request, android::Parcel *notifyReply)
{
    XBH_S32      s32Ret = XBH_SUCCESS;
    InnerMsg    msg;
    memset(&msg, 0, sizeof(InnerMsg));

    if (args != XBH_NULL && size > 0)
    {
        msg.args = malloc(size);
        if (!msg.args) 
            return XBH_FALSE;
        memcpy(msg.args, args, size);
        msg.size = size;

        if (request != XBH_NULL && request->dataSize() > 0)
        {
            msg.request = new android::Parcel();//(android::Parcel*)malloc(request->dataSize());
            if (!msg.request) 
                return XBH_FALSE;
            msg.request->setDataSize(request->dataSize());
            msg.request->setDataPosition(0);
            msg.request->write(request->data(), request->dataSize());
        }
        if (notifyReply != XBH_NULL && notifyReply->dataSize() > 0)
        {
            msg.notifyReply = new android::Parcel();//(android::Parcel*)malloc(request->dataSize());
            if (!msg.notifyReply) 
                return XBH_FALSE;
            msg.notifyReply->setDataSize(notifyReply->dataSize());
            msg.notifyReply->setDataPosition(0);
            msg.notifyReply->write(notifyReply->data(), notifyReply->dataSize());
        }
    }

    msg.msgType = msgType;

    s32Ret = pthread_mutex_lock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
    m_msgQueue.push(msg);
    //resume reader thread
    resume();
    s32Ret = pthread_mutex_unlock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
    return XBH_TRUE;
}

/**
 * Asynchronous message read function that creates a write message thread after reading a message
 */
XBH_BOOL XbhMsgHub::MsgReader()
{
    InnerMsg *msg = new (std::nothrow) InnerMsg;
    XBH_S32 s32Ret = XBH_FAILURE;
    if (msg == XBH_NULL) {
        XLOGE("msg = null  !!! ");
        return XBH_FALSE;
    }

    s32Ret = pthread_mutex_lock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
    if(m_msgQueue.size() <= 0)
    {
        XLOGE("m_msgQueue.size() = %d !!! ", m_msgQueue.size());
        //suspend read thread
        suspend();
        pthread_mutex_unlock(&m_msgQueueLock);
        return XBH_FALSE;
    }
    *msg = m_msgQueue.front();
    m_msgQueue.pop();
    s32Ret = pthread_mutex_unlock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    SendMsg(msg->msgType, msg->args, msg->size, msg->request, msg->notifyReply);

    if (msg->args != XBH_NULL) {
        free(msg->args);
        msg->args = XBH_NULL;
    }

    delete msg;
    msg = XBH_NULL;
    return XBH_TRUE;
}

static XbhMsgHub g_msgHub;

IMsgHandler::IMsgPublisher &MsgPublisher()
{
    g_msgHub.Init();
    return g_msgHub;
}

