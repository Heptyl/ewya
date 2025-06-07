#ifndef XBH_LIFTBRACKET_H
#define XBH_LIFTBRACKET_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhLiftbracket.h"
#include "XbhLiftbracketInterface.h"

#define LIFTBRACKET_DEV  "/dev/ttyUSB0"

typedef enum LIFTBRACKET_CMD_E
{
    LIFTBRACKET_UP             = 0,
    LIFTBRACKET_DOWM,
    LIFTBRACKET_STOP,
    LIFTBRACKET_SET_TO_MEMORY1,
    LIFTBRACKET_SET_TO_MEMORY2,
    LIFTBRACKET_SET_MEMORY1_COLUMN,
    LIFTBRACKET_SET_MEMORY2_COLUMN,
} LIFTBRACKET_CMD_T;

class XbhLiftbracket : public XbhLiftbracketInterface ,public XbhMWThread
{
public:
    static XbhLiftbracket* getInstance();
    void init();
    XBH_U8 sendCmd(int fd, const XBH_U8* cmd, int len);
    XBH_S32 readCommand(int fd, XBH_U8* pbuf, XBH_S32 len);
    XBH_BOOL openSerialPort(void);
    XBH_S32 setLiftbracketOperate(int operat);
    XBH_S32 getLiftbracketExist(int *status);
    XBH_S32 get_usb_pvid_exist(void);
    static XbhLiftbracket *mInstance;

private:
    XbhLiftbracket();
    ~XbhLiftbracket();

public:
    static XbhMutex mLock;
};

#endif
