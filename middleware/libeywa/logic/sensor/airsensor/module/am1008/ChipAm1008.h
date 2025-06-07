#ifndef XBH_AM1008_MANAGER_H
#define XBH_AM1008_MANAGER_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhAirSensorInterface.h"


class ChipAm1008: public XbhMWThread, public XbhAirSensorInterface
{
//---------------- custom device interface start ----------------
public:

    XBH_U8 sendCmd(int fd, const XBH_U8* cmd, int len);
    XBH_BOOL openSerialPort(const char *path, XBH_S32 baudrate);
    speed_t getBaudrate(XBH_S32 baudrate);
    XBH_S32 readCommand(XBH_U8* pbuf, XBH_S32 len);
    XBH_U8 getCheckSum(char* arr, XBH_U8 length);

    //获取二氧化碳值
    XBH_S32 getCo2Value();
    //获取VOC值
    XBH_S32 getVocValue();
    //获取湿度值
    XBH_S32 getHumidityValue();
    //获取温度
    XBH_S32 getTemperatureValue();
    //获取pm25
    XBH_S32 getPm25Value();
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);


    ChipAm1008(XBH_S32 uart, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipAm1008();

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
