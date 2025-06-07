#ifndef __XBH_GSENSOR_TASK_H__
#define __XBH_GSENSOR_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include <hardware/board.h>
#include "XbhGSensorInterface.h"

class XbhGSensorTask : public XbhMWThread
{
public:
    static XbhGSensorTask* getInstance();
    XBH_S32 Get_GSensor_XData();
    XBH_S32 Get_GSensor_YData();
    XBH_S32 Get_GSensor_ZData();
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* bEnable);

    ~XbhGSensorTask();

private:
    void run(const void* arg);
    XbhGSensorTask();
    static XbhGSensorTask *mInstance;
    static XbhMutex mLock;

    XbhGSensorInterface *m_pXbhGSensorInterface[2];

    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif
