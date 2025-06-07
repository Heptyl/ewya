#ifndef __XBH_MSG_QUEUE_H__
#define __XBH_MSG_QUEUE_H__

#include <list>
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhType.h"

class XbhMsgQueue {
public:
    /**
    * 将消息post到消息队列中
    * msgType 消息类型
    * args 参数
    * size 参数长度
    * delay 消息延时，单位ms
    * object 消息处理对象
    */
    XBH_BOOL postMsg(XBH_U32 msgType, const XBH_VOID* args, XBH_U32 size, XBH_U32 delay, XBH_VOID* object);
    
    /**
    * 获取消息队列
    * msgType 消息类型
    * args 参数
    * size 参数长度
    * object 消息处理对象
    */
    XBH_BOOL getMsg(XBH_U32* msgType, XBH_VOID* args, XBH_U32* size, XBH_VOID* object, XBH_BOOL block = XBH_TRUE);

    /**
    * 删除消息队列中对应的消息
    * msgType 消息类型
    */
    XBH_BOOL removeMsg(XBH_U32 msgType);

    XbhMsgQueue();
    ~XbhMsgQueue();
private:
    struct InnerMsg {
        XBH_U32 msgType;    //消息类型
        XBH_VOID *args;     //消息参数
        XBH_U32 size;       //消息长度
        XBH_U32 delay;      //消息延时多久后处理
        XBH_ULONG cTime;    //进入消息队列的时间
    };
    static XbhMutex mLock;
    std::list<InnerMsg> m_msgList;
};

#endif
