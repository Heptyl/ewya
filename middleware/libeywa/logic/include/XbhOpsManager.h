#ifndef __XBH_OPS_MANAGER_H__
#define __XBH_OPS_MANAGER_H__

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

class XbhOpsManager : public XbhMWThread
{
public:
    static XbhOpsManager* getInstance();
    ~XbhOpsManager();
    /**
     * 处理OPS开机启动的逻辑
    */
    XBH_S32 processOpsStart();
    /**
     * 处理OPS模块插拔
    */
    XBH_S32 processOpsPlug();
    /**
     * 处理OPS模块开关机状态上报
    */
    XBH_S32 processOpsStatusReport();
    /**
    * 动态调整功率
    */
    void dynamicallyAdjustPower();
private:
    void run(const void* arg);
    XbhOpsManager();
    XBH_S32 processFirstPowerOnOPS();
    static XbhOpsManager *mInstance;
    static XbhMutex mLock;

    XBH_BOOL mPreOps1DetStatus;
    XBH_BOOL mPreOps2DetStatus;
    XBH_BOOL mPreSdm1DetStatus;
    XBH_BOOL mPreSdm2DetStatus;
    XBH_BOOL mPreOps1DetStatusPower;
    XBH_BOOL mCurrOps1DetStatusPower;
    XBH_BOOL mSetCharging;
};

#endif //__XBH_OPS_MANAGER_H__
