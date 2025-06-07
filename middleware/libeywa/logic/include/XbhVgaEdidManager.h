#ifndef __XBH_VGA_EDID_MANAGER_H__
#define __XBH_VGA_EDID_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMWThread.h"
#include "XbhService.h"




class XbhVgaEdidManager:public XbhMWThread
{
public:
    static XbhVgaEdidManager* getInstance();
    XBH_S32 setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx);
    XBH_S32 setVgaEdidData(XBH_U8 edid_data[128], XBH_SOURCE_E idx);
    XBH_S32 getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable);
    XBH_S32 getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum);
    XBH_S32 dumpEdid();

    ~XbhVgaEdidManager();

private:
    void run(const void* arg);
    XbhVgaEdidManager();
    static XbhVgaEdidManager *mInstance;
    static XbhMutex mLock;

};

#endif //__XBH_SOURCE_MANAGER_H__


