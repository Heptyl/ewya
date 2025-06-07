#ifndef __XBH_POWERSENSOR_TASK_H__
#define __XBH_POWERSENSOR_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include <hardware/board.h>
#include "XbhPowerSensorInterface.h"

class XbhPowerSensorTask : public XbhMWThread
{
public:
    static XbhPowerSensorTask* getInstance();
    //获取有功功率
    XBH_S32 getPowerSensorValue(XBH_FLOAT * s32State);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    ~XbhPowerSensorTask();

private:
    void run(const void* arg);
    XbhPowerSensorTask();
    static XbhPowerSensorTask *mInstance;
    static XbhMutex mLock;

    XbhPowerSensorInterface *m_pXbhPowerSensorInterface[2];

    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif
