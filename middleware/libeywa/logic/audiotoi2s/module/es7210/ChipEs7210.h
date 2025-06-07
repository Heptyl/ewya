#ifndef XBH_CHIP_ES7210_H
#define XBH_CHIP_ES7210_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhAudioToI2sInterface.h"

class ChipEs7210 : public XbhAudioToI2sInterface
{
public:
    ChipEs7210(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipEs7210();

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
    XBH_S32 init();
    XBH_S32 Es7210_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 Es7210_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;

    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_ES7210_H
