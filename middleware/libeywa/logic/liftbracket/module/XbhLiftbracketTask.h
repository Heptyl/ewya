#ifndef XBH_LIFTBRACKET_TASK_H
#define XBH_LIFTBRACKET_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhLiftbracket.h"
#include "XbhLiftbracketInterface.h"

class XbhLiftbracketTask : public XbhMWThread
{
public:
    static XbhLiftbracketTask* getInstance();
    void init();
    XBH_U8 sendCmd(int fd, const XBH_U8* cmd, int len);
    XBH_S32 readCommand(int fd, XBH_U8* pbuf, XBH_S32 len);
    XBH_BOOL openSerialPort(void);
    XBH_S32 setLiftbracketOperate(int operat);
    XBH_S32 getLiftbracketExist(int *status);
    XBH_S32 get_usb_pvid_exist(void);
    ~XbhLiftbracketTask();

private:
    void run(const void* arg);
    XbhLiftbracketTask();

private:
    XbhLiftbracketInterface *m_pXbhLiftbracketInterface[1];
    static XbhLiftbracketTask *mInstance;
    static XbhMutex mLock;

    XBH_U8 mList1[1];
};

#endif
