#ifndef _CHIP_ITE6265_H
#define _CHIP_ITE6265_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhAVOutInterface.h"
#include "XbhMutex.h"
#include "It6265.h"

class ChipIte6265 : public XbhAVOutInterface
{
//---------------- custom device interface start ----------------
public:
    It6265* m_pChipIt6265 =  XBH_NULL;

public:
    // static XbhIte6265Task* getInstance();

    // XBH_BOOL NoSaveEnv();
    // XBH_S32  setHdmiOutEnable(XBH_U32 onOff);
    // XBH_U32  getHdmiOutEnable();
    // XBH_S32  setHdmiOutSoundEnable(XBH_U32 onOff);
    // XBH_U32  getHdmiOutSoundEnable();
    // XBH_S32  setHdmiOutHdcpEnable(XBH_U32 onOff);
    // XBH_U32  getHdmiOutHdcpEnable();
    // XBH_S32  setHdmiOutResolution(XBH_U32 resolution);
    // XBH_U32  getHdmiOutResolution();

    ChipIte6265(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipIte6265();

    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip();
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

private:
    void run(const void* arg);

private:
    static ChipIte6265 *mInstance;
    static XbhMutex mLock;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
//---------------- custom device interface end ----------------
};

#endif //XBH_ITE6265_TASK_H