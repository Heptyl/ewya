#ifndef __XBH_RJ45_TO_USB_INTERFACE_H__
#define __XBH_RJ45_TO_USB_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhRj45ToUsbInterface
{
public:

    /**
     * 同步mac地址到芯片
    */
    virtual XBH_S32 syncBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* devNode, XBH_CHAR* devType);

public:
    XbhRj45ToUsbInterface(){};
    virtual ~XbhRj45ToUsbInterface(){};
};

#endif
