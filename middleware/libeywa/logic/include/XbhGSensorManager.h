#ifndef __XBH_GSENSOR_MANAGER_H__
#define __XBH_GSENSOR_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhGSensorTask.h"

class XbhGSensorManager
{
public:
    static XbhGSensorManager* getInstance();
    XBH_S32 Get_GSensor_XData();
    XBH_S32 Get_GSensor_YData();
    XBH_S32 Get_GSensor_ZData();
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    ~XbhGSensorManager();

private:
    XbhGSensorManager();
    static XbhGSensorManager *mInstance;
    static XbhMutex mLock;

    XbhGSensorTask *m_pXbhGSensorTask;
};

#endif
