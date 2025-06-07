#ifndef XBH_RJ45_TO_USB_MANAGER_H
#define XBH_RJ45_TO_USB_MANAGER_H

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMutex.h"
#include "XbhRj45ToUsbTask.h"
#include <cutils/properties.h>
#include <hardware/board.h>


class XbhRj45ToUsbManager
{
public:
    static XbhRj45ToUsbManager* getInstance();

    /**
     * 同步mac地址到芯片
    */
    XBH_S32 syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType);

    ~XbhRj45ToUsbManager();

private:
    XbhRj45ToUsbManager();
    static XbhRj45ToUsbManager *mInstance;
    static XbhMutex mLock;
    XbhRj45ToUsbTask *m_pXbhRj45ToUsbTask;
};

#endif //XBH_RJ45_TO_USB_MANAGER_H
