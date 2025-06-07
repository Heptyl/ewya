#define LOG_NDEBUG 0
#define XBH_LOG_TAG    "xbh_mw@XbhMsgQueue_old"

#include "XbhMsgQueue_old.h"
#include <string.h>
#include <stdlib.h>
#include "XbhLog.h"

XBH_BOOL XbhMsgQueue_old::postMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, XBH_VOID * point)
{
    XBH_S32     s32Ret = XBH_SUCCESS;
    InnerMsg    msg;
    memset(&msg, 0, sizeof(InnerMsg));

    if (args != XBH_NULL && size > 0)
    {
        msg.args = malloc(size);
        if (!msg.args) 
            return XBH_FALSE;
        memcpy(msg.args, args, size);
        msg.size = size;
    }

    msg.msgType = msgType;

    s32Ret = pthread_mutex_lock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
    m_msgQueue.push(msg);
    //resume reader thread
    if(point != XBH_NULL)
    {
        ((XbhMWThread* )point)->resume();
    }
    s32Ret = pthread_mutex_unlock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
    return XBH_TRUE;
}

XBH_BOOL XbhMsgQueue_old::getMsg(XBH_U32* msgType, XBH_VOID* args, XBH_U32* size, XBH_VOID* point)
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
        if(point != XBH_NULL)
        {
            ((XbhMWThread* )point)->suspend();
        }
        pthread_mutex_unlock(&m_msgQueueLock);
        return XBH_FALSE;
    }
    *msg = m_msgQueue.front();
    m_msgQueue.pop();
    s32Ret = pthread_mutex_unlock(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);

    *msgType = msg->msgType;
    memcpy(args, msg->args, msg->size);
    *size = msg->size;

    if (msg->args != XBH_NULL) {
        free(msg->args);
        msg->args = XBH_NULL;
    }

    delete msg;
    msg = XBH_NULL;
    return XBH_TRUE;
}

XbhMsgQueue_old::XbhMsgQueue_old()
{
    XBH_S32 s32Ret;
    s32Ret = pthread_mutex_init(&m_msgQueueLock, 0);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
}

XbhMsgQueue_old::~XbhMsgQueue_old()
{
    XBH_S32 s32Ret;
    s32Ret = pthread_mutex_destroy(&m_msgQueueLock);
    RET_CHECK_ERR_LOG(s32Ret, "pthread_cond_init fail, ret:0x%x", s32Ret);
}
