#ifndef XBH_PD_IC_MANAGER_H
#define XBH_PD_IC_MANAGER_H

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMutex.h"
#include "XbhPdIcTask.h"

class XbhPdIcManager
{
public:
    static XbhPdIcManager* getInstance();
    XBH_S32 resetChip(XBH_S32 devType);
    XBH_S32 getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 devType, XBH_S32 * s32State);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_S32 devType, XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_S32 devType, XBH_BOOL* enable);
    /**
     * 设置type-c功率
     */
    XBH_S32 setTypecCapacity( XBH_U32 devType, XBH_U8 data);

    ~XbhPdIcManager();

private:
    XbhPdIcManager();
    static XbhPdIcManager *mInstance;
    static XbhMutex mLock;

    XbhPdIcTask *m_pXbhPdIcTask;
};

#endif //XBH_PD_IC_MANAGER_H
