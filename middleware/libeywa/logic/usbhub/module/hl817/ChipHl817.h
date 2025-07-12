#ifndef XBH_HL_817_H
#define XBH_HL_817_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhUsbHubInterface.h"

#define HL817_PID 0x2109
#define HL817_VID 0x8817

class ChipHl817 : public XbhMWThread, public XbhUsbHubInterface
{
public:
    ChipHl817(XBH_S32 i2cNumber, XBH_S32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取当前升级的进度
     */
    XBH_S32 getUpgradeState(XBH_S32 * s32State);
private:
    enum XBH_Hl817_UPDATE_STATE_E {
        Hl817_IDLE        = 0,
        Hl817_RUNNING     = 1,
        Hl817_SUCCESS     = 2,
        Hl817_FAILURE     = 3,
    };
    XBH_S32 mPortNumbers;
    XBH_S32 mI2cAddress;
    XBH_S32 mCurrentPort;
    XBH_S32 mPowerGpio;
    XBH_S32 mPowerLevel;
    XBH_S32 mResetGpio;
    XBH_S32 mResetLevel;

    XBH_S32 mState;
    void run(const void* arg);
    ~ChipHl817();
    static ChipHl817 *mInstance;
    static XbhMutex mLock;
    XBH_CHAR mUpdateFirmware[256];
};

#endif //XBH_HL_817_H