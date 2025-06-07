#ifndef __XBH_AUDIO_TO_I2S_MANAGER_H__
#define __XBH_AUDIO_TO_I2S_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAudioToI2sTask.h"

class XbhAudioToI2sManager
{
public:
    static XbhAudioToI2sManager* getInstance();
    /**
     * 复位Audio to i2s芯片
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

    ~XbhAudioToI2sManager();

private:
    XbhAudioToI2sManager();
    static XbhAudioToI2sManager *mInstance;
    static XbhMutex mLock;

    XbhAudioToI2sTask *m_pXbhAudioToI2sTask;
};

#endif
