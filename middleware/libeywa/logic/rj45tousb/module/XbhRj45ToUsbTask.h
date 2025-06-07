#ifndef XBH_RJ45_TO_USB_TASK_H
#define XBH_RJ45_TO_USB_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhModuleInterface.h"
#include "XbhRj45ToUsbInterface.h"

class XbhRj45ToUsbTask : public XbhMWThread
{
public:
    static XbhRj45ToUsbTask* getInstance();
    /**
     * 同步mac地址到芯片
    */
    XBH_S32 syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType);

    ~XbhRj45ToUsbTask();

private:
    void run(const void* arg);
    XbhRj45ToUsbTask();
    static XbhRj45ToUsbTask *mInstance;
    static XbhMutex mLock;

    XbhRj45ToUsbInterface *m_pXbhRj45ToUsbInterface[2];
    XBH_U8 mList1[1];
    COMMON_OPT_TABLE mList2[1];
};

#endif //XBH_RJ45_TO_USB_TASK_H