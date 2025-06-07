#ifndef XBH_PD_IC_TASK_H
#define XBH_PD_IC_TASK_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhPdIcInterface.h"

class XbhPdIcTask : public XbhMWThread
{
public:
    static XbhPdIcTask* getInstance();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip(XBH_S32 devType);

    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_S32 devType, XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(XBH_S32 devType, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
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
    XBH_S32 setTypecCapacity(XBH_U32 devType, XBH_U8 data);

    ~XbhPdIcTask();

private:
    void run(const void* arg);
    XbhPdIcTask();
    XbhPdIcInterface *getCurrIC(XBH_S32 devType);
    static XbhPdIcTask *mInstance;
    static XbhMutex mLock;
    XbhPdIcInterface *m_pXbhPdIcInterfacee[8];

    XBH_U8 mList1[8];
    COMMON_OPT_TABLE mList2[8];
};

#endif //XBH_PD_IC_TASK_H
