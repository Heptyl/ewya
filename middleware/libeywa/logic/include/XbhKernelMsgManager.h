#ifndef __XBH_KERNEL_MSG_MANAGER_H__
#define __XBH_KERNEL_MSG_MANAGER_H__

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMsgQueue.h"

class XbhKernelMsgManager : public XbhMWThread, public XbhMsgQueue
{
public:
    static XbhKernelMsgManager* getInstance();
    XBH_S32 sendMsgToKernel(XBH_U8 data0, XBH_U8 data1, XBH_U8 data2, XBH_U8 data3, XBH_U8 data4);
    ~XbhKernelMsgManager();

private:
    void run(const void* arg);
    XbhKernelMsgManager();
    static XBH_VOID xbh_msg_handler(int num);
    static XbhKernelMsgManager *mInstance;
    static XbhMutex mLock;
};

#endif //__XBH_KERNEL_MSG_MANAGER_H__
