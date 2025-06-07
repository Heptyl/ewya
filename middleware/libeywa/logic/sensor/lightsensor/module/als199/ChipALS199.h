#ifndef XBH_CHIP_ALS199_H
#define XBH_CHIP_ALS199_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhLightSensorInterface.h"

#define ALS199_REG_SYSTEM_CTRL         (0x00)
#define ALS199_REG_INT_CTRL            (0x01)
#define ALS199_REG_INT_FLAG            (0x02)
#define ALS199_REG_WAIT_TIME           (0x03)
#define ALS199_REG_ALS_GAIN            (0x04)
#define ALS199_REG_ALS_TIME            (0x05)
#define ALS199_REG_PERSISTENCE         (0x0b)
#define ALS199_REG_ALS_TH_LL           (0x0c)
#define ALS199_REG_ALS_TH_LH           (0x0d)
#define ALS199_REG_ALS_TH_HL           (0x0e)
#define ALS199_REG_ALS_TH_HH           (0x0f)
#define ALS199_REG_DATA_STAT           (0x17)
#define ALS199_REG_ALS_DATA_L          (0x88)
#define ALS199_REG_ALS_DATA_H          (0x89)
#define ALS199_REG_PRODUCT_ID          (0x86)
#define ALS199_MAX_REG                 (0xbd)

#define ALS199_PRODUCT_ID              (0x1011)

#define ALS199_CONF_SWRST              (0x80)
#define ALS199_CONF_EN_ALS             (0x01)

class ChipALS199 : public XbhLightSensorInterface
{
public:
    ChipALS199(XBH_S32 iicBus, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipALS199();

    XBH_S32 resetChip();
    XBH_S32 getLightSensorValue(XBH_S32 * s32State);
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);


private:
    XBH_S32 init();
    XBH_S32 ALS199_Write(XBH_U8 reg, XBH_U8 data);
    XBH_S32 ALS199_Read(XBH_U8 reg, XBH_U8* data);
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_BOOL m_bInitDone;
};

#endif //XBH_CHIP_ALS199_H
