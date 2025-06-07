#ifndef __XBH_AIRSENSOR_INTERFACE_H__
#define __XBH_AIRSENSOR_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhAirSensorInterface
{
public:
    //获取二氧化碳值
    virtual XBH_S32 getCo2Value();
    //获取VOC值
    virtual XBH_S32 getVocValue();
    //获取湿度值
    virtual XBH_S32 getHumidityValue();
    //获取温度
    virtual XBH_S32 getTemperatureValue();
    //获取pm25
    virtual XBH_S32 getPm25Value();
    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);

public:
    XbhAirSensorInterface(){};
    virtual ~XbhAirSensorInterface(){};
};

#endif
