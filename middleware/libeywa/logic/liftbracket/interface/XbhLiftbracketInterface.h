#ifndef __XBH_LIFT_BRACKET_INTERFACE_H__
#define __XBH_LIFT_BRACKET_INTERFACE_H__

#include "XbhType.h"

class XbhLiftbracketInterface
{
public:
    XBH_U8 sendCmd(int fd, const XBH_U8* cmd, int len);
    XBH_S32 readCommand(int fd, XBH_U8* pbuf, XBH_S32 len);
    XBH_BOOL openSerialPort(void);
    XBH_S32 setLiftbracketOperate(int operat);
    XBH_S32 getLiftbracketExist(int *status);
    XBH_S32 get_usb_pvid_exist(void);

public:
    XbhLiftbracketInterface(){};
    virtual ~XbhLiftbracketInterface(){};
};

#endif
