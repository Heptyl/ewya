#ifndef __XBH_LIGHT_SENSOR_TASK_H__
#define __XBH_LIGHT_SENSOR_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include "XbhLightSensorInterface.h"

class XbhLightSensorTask : public XbhMWThread
{
public:
    static XbhLightSensorTask* getInstance();
    /**
     * 复位light sensor芯片
    */
    XBH_S32 resetChip();
    /**
     * 获取lightsensor的值
     */
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    /**
     * 获取lightsensor是否存在
     */
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
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
     * 执行光感初始化
     */
    XBH_S32 executeLightSensorInit();

    ~XbhLightSensorTask();

private:
    void init();
    XbhLightSensorTask();
    static XbhLightSensorTask *mInstance;
    static XbhMutex mLock;

    XbhLightSensorInterface *m_pXbhLightSensorInterface[1];

    COMMON_OPT_TABLE mList1[1];
};

#endif
