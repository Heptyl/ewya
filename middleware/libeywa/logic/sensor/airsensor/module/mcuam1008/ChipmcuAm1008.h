#ifndef XBH_MCUAM1008_MANAGER_H
#define XBH_MCUAM1008_MANAGER_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhAirSensorInterface.h"

#define SENSORDATESIZE      20

class ChipmcuAm1008: public XbhMWThread, public XbhAirSensorInterface
{
//---------------- custom device interface start ----------------
public:
    //获取MCU上am1008传感器的值
    XBH_S32 getAirSensorFormMcu();
    //获取VOC值
    XBH_S32 getVocValue();
    //获取湿度值
    XBH_S32 getHumidityValue();
    //获取温度
    XBH_S32 getTemperatureValue();
    //获取pm25
    XBH_S32 getPm25Value();
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);

    ChipmcuAm1008(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipmcuAm1008();

private:
    void run(const void* arg);
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_U8 sensorDate[SENSORDATESIZE];

};
#endif
