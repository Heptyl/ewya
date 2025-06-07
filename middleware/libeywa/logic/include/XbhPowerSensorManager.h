#ifndef __XBH_POWERSENSOR_MANAGER_H__
#define __XBH_POWERSENSOR_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhPowerSensorTask.h"

class XbhPowerSensorManager
{
public:
    static XbhPowerSensorManager* getInstance();
    //获取有功功率
    XBH_S32 getPowerSensorValue(XBH_FLOAT* s32State);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    ~XbhPowerSensorManager();

private:
    XbhPowerSensorManager();
    static XbhPowerSensorManager *mInstance;
    static XbhMutex mLock;

    XbhPowerSensorTask *m_pXbhPowerSensorTask;
};

#endif
