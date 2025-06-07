#ifndef __XBH_LIGHT_SENSOR_INTERFACE_H__
#define __XBH_LIGHT_SENSOR_INTERFACE_H__

#include "XbhType.h"

class XbhLightSensorInterface
{
public:
    /**
     * 复位light sensor芯片
    */
    virtual XBH_S32 resetChip();
    /**
     * 获取lightsensor的值
     */
    virtual XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
     * 获取固件版本
     */
    virtual XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
     * 进行固件升级
     */
    virtual XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     * 获取固件升级时的状态
     */
    virtual XBH_S32 getUpgradeState(XBH_S32 * s32State);

public:
    XbhLightSensorInterface(){};
    virtual ~XbhLightSensorInterface(){};
};

#endif
