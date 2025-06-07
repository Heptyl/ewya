#ifndef XBH_CHIP_Ltrx140a_H
#define XBH_CHIP_Ltrx140a_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhLightSensorInterface.h"

#define Ltrx140a_I2CADDR             0x23
#define LTRX140A_ALS_CONFIG_REG      0x7F
#define LTRX140A_CMD_REGISTER        0x80
#define LTRX140A_MAIN_CTRL_REG       0xAD
#define LTRX140A_ALS_STATUS_REG      0x88
#define Ltrx140a_REG_ALS_DATA_L      0x8D
#define Ltrx140a_REG_ALS_DATA_H      0x8C


class ChipLtrx140a : public XbhLightSensorInterface
{
public:
    ChipLtrx140a(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipLtrx140a();

    XBH_S32 resetChip();
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);


private:
    XBH_S32 init();
    XBH_S32 Ltrx140a_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 Ltrx140a_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_Ltrx140a_H