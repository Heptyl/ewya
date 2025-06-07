#ifndef XBH_AX_88279_H
#define XBH_AX_88279_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhRj45ToUsbInterface.h"

class ax88279 : public XbhMWThread, public XbhRj45ToUsbInterface
{
public:
    static ax88279* getInstance();
    /**
     * 同步mac地址到芯片
    */
    XBH_S32 syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType);

    ~ax88279();

private:
    void run(const void* arg);
    ax88279();
    static ax88279 *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_AX_88279_H