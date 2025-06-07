#ifndef XBH_AX_88179_H
#define XBH_AX_88179_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhRj45ToUsbInterface.h"

class ax88179 : public XbhMWThread, public XbhRj45ToUsbInterface
{
public:
    static ax88179* getInstance();
    /**
     * 同步mac地址到芯片
    */
    XBH_S32 syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType);

    ~ax88179();

private:
    void run(const void* arg);
    ax88179();
    static ax88179 *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_AX_88179_H