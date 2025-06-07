#ifndef XBH_CHIP_TSL2561_H
#define XBH_CHIP_TSL2561_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhLightSensorInterface.h"

#define TSL2561_I2CADDR             0x49

#define TSL25611_V                  0x34

#define TSL2561_CMD_REGISTER        0x80

#define TSL2561_REG_ALS_DATA_L      0xAC
#define TSL2561_REG_ALS_DATA_H      0xAD


class ChipTsl2561 : public XbhLightSensorInterface
{
public:
    ChipTsl2561(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipTsl2561();

    XBH_S32 resetChip();
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);


private:
    XBH_S32 init();
    XBH_S32 TSL2561_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 TSL2561_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_TSL2561_H