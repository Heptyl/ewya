#ifndef __XBH_POWERSENSOR_INTERFACE_H__
#define __XBH_POWERSENSOR_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhPowerSensorInterface
{
public:
    //获取有功功率
    virtual XBH_S32 getPowerSensorValue(XBH_FLOAT* s32State);
    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);

public:
    XbhPowerSensorInterface(){};
    virtual ~XbhPowerSensorInterface(){};
};

#endif
