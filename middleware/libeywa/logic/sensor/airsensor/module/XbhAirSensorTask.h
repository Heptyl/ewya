#ifndef __XBH_AIRSENSOR_TASK_H__
#define __XBH_AIRSENSOR_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include <hardware/board.h>
#include "XbhAirSensorInterface.h"

class XbhAirSensorTask : public XbhMWThread
{
public:
    static XbhAirSensorTask* getInstance();
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
    ~XbhAirSensorTask();

private:
    void run(const void* arg);
    XbhAirSensorTask();
    static XbhAirSensorTask *mInstance;
    static XbhMutex mLock;

    XbhAirSensorInterface *m_pXbhAirSensorInterface[2];

    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif
