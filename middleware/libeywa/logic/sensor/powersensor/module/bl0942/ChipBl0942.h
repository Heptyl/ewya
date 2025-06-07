#ifndef XBH_BL0942_MANAGER_H
#define XBH_BL0942_MANAGER_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhPowerSensorInterface.h"


class ChipBl0942: public XbhMWThread, public XbhPowerSensorInterface
{
//---------------- custom device interface start ----------------
public:

    XBH_U8 sendCmd(int fd, const XBH_U8* cmd, int len);
    XBH_BOOL openSerialPort(const char *path, XBH_S32 baudrate);
    speed_t getBaudrate(XBH_S32 baudrate);
    XBH_S32 readCommand(XBH_U8* pbuf, XBH_S32 len);
    XBH_U8 getCheckSum(char* arr, XBH_U8 length);

    //获取有功功率
    XBH_S32 getPowerSensorValue(XBH_FLOAT * s32State);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);


    ChipBl0942(XBH_S32 uart, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipBl0942();

private:
    void run(const void* arg);
    void init();
    static XbhMutex mLock;
    XBH_S32 muart;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;

    XBH_S32 m_bInitDone;

    XBH_S32 readSensorData();
};
#endif
