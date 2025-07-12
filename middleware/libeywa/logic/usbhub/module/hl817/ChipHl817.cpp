#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipHl817"

#include "XbhLog.h"
#include "ChipHl817.h"
#include "XbhPartitionData.h"
#include "XbhService.h"
#include <cutils/properties.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "hub_update.h"
#ifdef __cplusplus
}
#endif

XbhMutex              ChipHl817::mLock;

/**
 * 获取固件版本
 */
XBH_S32 ChipHl817::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char verResult[3] = {0};
    getVersion(HL817_PID, HL817_VID, mPortNumbers, strVersion);
    XLOGD("817 version  (%s)\n", strVersion);
    property_set("persist.vendor.xbh.hl817.ver", strVersion);
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 ChipHl817::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == Hl817_RUNNING)
    {
        XLOGW("Warning: Hl817 is updating...");
        return XBH_FAILURE;
    }
    mState = Hl817_RUNNING;
    XLOGD("817: upgradeFirmware run");
    if (strFilePath == XBH_NULL)
    {
        XLOGE("Error: Hl817 fileName is XBH_NULL");
        mState = Hl817_FAILURE;
        return XBH_FAILURE;
    }
    if (snprintf(mUpdateFirmware, MAX_PATH, "%s", strFilePath) >= MAX_PATH) {
        XLOGE("Error: Hl817 strFilePath cpy");
        mState = Hl817_FAILURE;
        return Fail_State;
    }
    XbhMWThread::run(XbhMWThread::ONCE);
    return XBH_SUCCESS;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 ChipHl817::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

void ChipHl817::run(const void* arg)
{
    property_set("persist.vendor.xbh.hl817.ver", "");
    XLOGD("Hl817 mUpdateFirmware %s", mUpdateFirmware);
    int _State = updateFirmware(HL817_PID, HL817_VID, mPortNumbers, mUpdateFirmware);
    if(_State == Success_State) {
        // reset 后版本号才会更新
        XLOGD("817: reset gpio %d", mResetGpio);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(50*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
        usleep(10*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(50*1000);
        XLOGD("817: upgradeFirmware run success");
        mState = Hl817_SUCCESS;
    } else {
        XLOGD("817: upgradeFirmware run Faild %d", mState);
        mState = Hl817_FAILURE;
    }
}

ChipHl817::ChipHl817(XBH_S32 portNumbers, XBH_S32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel)
{
    XLOGD(" ChipHl817 init");
    mPortNumbers = portNumbers;
    mI2cAddress = i2cAddress;
    mPowerGpio = powerGpio;
    mPowerLevel = powerLevel;
    mResetGpio = resetGpio;
    mResetLevel = resetLevel;

}

ChipHl817::~ChipHl817()
{
    XLOGD(" ~ChipHl817 ");
}