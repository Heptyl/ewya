#define LOG_NDEBUG 0
#define XBH_LOG_TAG    "xbh_mw@XbhMsgQueue"

#include "XbhMsgQueue.h"
#include <string.h>
#include <stdlib.h>

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include "XbhLog.h"

XbhMutex                        XbhMsgQueue::mLock;

XBH_BOOL XbhMsgQueue::postMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, XBH_U32 delay, XBH_VOID * object)
{
    XbhMutex::Autolock _l(mLock);
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
    
    struct timeval tv;
    gettimeofday(&tv, NULL);

    msg.msgType = msgType;
    msg.delay = delay;
    msg.cTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    
    XLOGD("delay = %d  cTime = %ld ", delay, msg.cTime);

    m_msgList.push_back(msg);
    //resume reader thread
    if(object != XBH_NULL)
    {
        ((XbhMWThread* )object)->resume();
    }
    return XBH_TRUE;
}

XBH_BOOL XbhMsgQueue::getMsg(XBH_U32* msgType, XBH_VOID* args, XBH_U32* size, XBH_VOID* object, XBH_BOOL block)
{
    XbhMutex::Autolock _l(mLock);

    if(m_msgList.size() <= 0)
    {
        if(!block)
        {
            return XBH_FALSE;
        }
        XLOGE("m_msgList.size() = %d block = %d ", m_msgList.size(), block);
        //suspend read thread
        if(object != XBH_NULL)
        {
            ((XbhMWThread* )object)->suspend();
        }
        return XBH_FALSE;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    XBH_ULONG time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    std::list<InnerMsg>::iterator it;
    for (it = m_msgList.begin(); it != m_msgList.end(); ++ it) 
    {
        if(time >= (it->delay + it->cTime))
        {
            XLOGD("get msg === type = %d ", it->msgType);
            break;
        }
    }
    //找到元素后，返回元素的数据并删除此元素
    if(it != m_msgList.end())
    {
        *msgType = it->msgType;
        memcpy(args, it->args, it->size);
        *size = it->size;
        m_msgList.erase(it);
    }
    else
    {
        //没有找到合适的元素
        return XBH_FALSE;
    }
    
    return XBH_TRUE;
}

XBH_BOOL XbhMsgQueue::removeMsg(XBH_U32 msgType)
{
    XbhMutex::Autolock _l(mLock);

    if(m_msgList.size() <= 0)
    {
        XLOGE("m_msgList.size() = %d !!! ", m_msgList.size());
        return XBH_FALSE;
    }
    std::list<InnerMsg>::iterator it;
    for (it = m_msgList.begin(); it != m_msgList.end(); ++ it) 
    {
        if(msgType == it->msgType)
        {
            XLOGD("removeMsg msg type = %d ",msgType);
            m_msgList.erase(it);
        }
    }
    return XBH_TRUE;
}

XbhMsgQueue::XbhMsgQueue()
{
}

XbhMsgQueue::~XbhMsgQueue()
{
}

