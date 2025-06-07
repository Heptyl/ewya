#ifndef __XBH_LIGHT_SENSOR_MANAGER_H__
#define __XBH_LIGHT_SENSOR_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhLightSensorTask.h"

class XbhLightSensorManager
{
public:
    static XbhLightSensorManager* getInstance();
    /**
     * 复位light sensor芯片
    */
    XBH_S32 resetChip();
    /**
     * 获取lightsensor的值
     */
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_S32 * s32State);
    /**
     * 执行光感模块初始化
     */
    XBH_S32 executeLightSensorInit();

    ~XbhLightSensorManager();

private:
    XbhLightSensorManager();
    static XbhLightSensorManager *mInstance;
    static XbhMutex mLock;

    XbhLightSensorTask *m_pXbhLightSensorTask;
};

#endif
