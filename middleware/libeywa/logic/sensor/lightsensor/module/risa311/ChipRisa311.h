#ifndef XBH_CHIP_RISA311_H
#define XBH_CHIP_RISA311_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhLightSensorInterface.h"

#define RISA311_REG_SYSTEM_CTRL         (0x00)
#define RISA311_REG_INT_CTRL            (0x01)
#define RISA311_REG_ATIME0              (0x02)
#define RISA311_REG_ATIME1              (0x03)
#define RISA311_REG_WAIT_TIME           (0x04)
#define RISA311_REG_AILTL               (0x05)
#define RISA311_REG_AILTH               (0x06)
#define RISA311_REG_AIHTL               (0x07)
#define RISA311_REG_AIHTH               (0x08)
#define RISA311_REG_ALS_DATA0_L         (0x0E)
#define RISA311_REG_ALS_DATA0_H         (0x0F)
#define RISA311_REG_ALS_DATA1_L         (0x10)
#define RISA311_REG_ALS_DATA1_H         (0x11)
#define RISA311_REG_PRODUCT_ID          (0x12)
#define RISA311_REG_AGC0                (0x14)
#define RISA311_REG_AGC1                (0x15)
#define RISA311_MAX_REG                 (0xbd)

#define RISA311_PRODUCT_ID_1            (0xA1)
#define RISA311_PRODUCT_ID_2            (0xA5)

#define RISA311_CONF_EN_ALS             (0x02)

class ChipRisa311 : public XbhLightSensorInterface
{
public:
    ChipRisa311(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipRisa311();

    XBH_S32 resetChip();
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);


private:
    XBH_S32 init();
    XBH_S32 RISA311_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 RISA311_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_RISA311_H