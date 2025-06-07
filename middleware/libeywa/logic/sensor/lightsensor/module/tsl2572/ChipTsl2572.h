#ifndef XBH_CHIP_TSL2572_H
#define XBH_CHIP_TSL2572_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhLightSensorInterface.h"

#define TSL2572_I2CADDR             0x39

#define TSL25721_V                  0x34

#define TSL2572_GAIN_1X             1
#define TSL2572_GAIN_8X             8
#define TSL2572_GAIN_16X            16
#define TSL2572_GAIN_120X           120

#define TSL2572_CMD_REGISTER        0x80
#define TSL2572_TYPE_INC            0x20

#define TSL2572_CMDS_ENABLE         0x00
#define TSL2572_CMDS_ALS_TIMING     0x01
#define TSL2572_CMDS_WAIT_TIME      0x03
#define TSL2572_CMDS_PERSISTANCE    0x0c
#define TSL2572_CMDS_CONFIG         0x0d
#define TSL2572_CMDS_CONTROL        0x0f
#define TSL2572_CMDS_WHOAMI         0x12
#define TSL2572_CMDS_STATUS         0x13

#define TSL2572_REG_ALS_DATA1_L     0x94
#define TSL2572_REG_ALS_DATA1_H     0x95
#define TSL2572_REG_ALS_DATA2_L     0x96
#define TSL2572_REG_ALS_DATA2_H     0x97

#define TSL2572_REG_PRODUCT_ID      0x12

class ChipTsl2572 : public XbhLightSensorInterface
{
public:
    ChipTsl2572(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipTsl2572();

    XBH_S32 resetChip();
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);


private:
    XBH_S32 init();
    XBH_S32 TSL2572_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 TSL2572_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_TSL2572_H