#ifndef XBH_CHIP_ES7243_H
#define XBH_CHIP_ES7243_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMWThread.h"

#include "XbhAudioToI2sInterface.h"

class ChipEs7243 : public XbhAudioToI2sInterface, public XbhMWThread
{
public:

    ChipEs7243(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipEs7243();

    XBH_S32 resetChip();
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);

private:
    XBH_S32 getChipId();
    XBH_S32 init();
    void run(const void* arg);
    XBH_S32 Es7243_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 Es7243_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;

    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_ES7243_H
