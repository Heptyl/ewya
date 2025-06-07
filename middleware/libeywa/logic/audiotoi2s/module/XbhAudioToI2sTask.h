#ifndef __XBH_AUDIO_TO_I2S_TASK_H__
#define __XBH_AUDIO_TO_I2S_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include "XbhAudioToI2sInterface.h"

class XbhAudioToI2sTask : public XbhMWThread
{
public:
    static XbhAudioToI2sTask* getInstance();
    /**
     * 复位audioToI2s芯片
    */
    XBH_S32 resetChip();
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

    ~XbhAudioToI2sTask();

private:
    void run(const void* arg);
    XbhAudioToI2sTask();
    static XbhAudioToI2sTask *mInstance;
    static XbhMutex mLock;

    XbhAudioToI2sInterface *m_pXbhAudioToI2sInterface[2];

    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif
