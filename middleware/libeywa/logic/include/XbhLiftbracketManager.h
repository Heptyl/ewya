#ifndef XBH_LIFTBRACKET_MANAGER_H
#define XBH_LIFTBRACKET_MANAGER_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhLiftbracketTask.h"

class XbhLiftbracketManager
{
public:
    static XbhLiftbracketManager* getInstance();
    void init();
    XBH_U8 sendCmd(int fd, const XBH_U8* cmd, int len);
    XBH_S32 readCommand(int fd, XBH_U8* pbuf, XBH_S32 len);
    XBH_BOOL openSerialPort(void);
    XBH_S32 setLiftbracketOperate(int operat);
    XBH_S32 getLiftbracketExist(int *status);
    XBH_S32 get_usb_pvid_exist(void);

private:
    XbhLiftbracketManager();
    ~XbhLiftbracketManager();

private:
    static XbhLiftbracketManager *mInstance;
    static XbhMutex mLock;

    XbhLiftbracketTask *m_pXbhLiftbracketTask;
};

#endif
