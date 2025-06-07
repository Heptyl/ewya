#ifndef __XBH_AIRSENSOR_MANAGER_H__
#define __XBH_AIRSENSOR_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAirSensorTask.h"

class XbhAirSensorManager
{
public:
    static XbhAirSensorManager* getInstance();
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
    ~XbhAirSensorManager();

private:
    XbhAirSensorManager();
    static XbhAirSensorManager *mInstance;
    static XbhMutex mLock;

    XbhAirSensorTask *m_pXbhAirSensorTask;
};

#endif
