#ifndef XBH_CHIP_JSA1141_H
#define XBH_CHIP_JSA1141_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhNfcInterface.h"

//MCU CMD
#define CMD_I2C_GET_NFC_EVENT                0xC0
#define CMD_I2C_GET_NFC_COUNT                0xC1
#define CMD_I2C_SET_NFC_INDEX                0xC2
#define CMD_I2C_GET_NFC_INDEX                0xC3
#define CMD_I2C_GET_NFC_ID                   0xC4
#define CMD_I2C_ADD_NFC_ID                   0xC5
#define CMD_I2C_DEL_NFC_ID                   0xC6
#define CMD_I2C_DEL_ALL_NFC                  0xC7
#define CMD_I2C_IS_NFC_EXIST                 0xC8
#define CMD_I2C_SET_NFC_LOW_POWER            0xC9
#define CMD_I2C_GET_NFC_LOW_POWER            0xCA
#define CMD_I2C_GET_NFC_WAKEUP_ID            0xCE
#define CMD_I2C_SWITCH_NFC_TYPE              0xCF

#define CMD_I2C_SOC_POWER_MODE               0xB0

// struct XBH_MCU_I2CBUFFDEF_S
// {
//     XBH_U8 len;
//     XBH_U8 cmd;
//     XBH_U8 data[MCU_I2C_BUFF_SIZE];
// };


class ChipMcuNfc : public XbhNfcInterface
{
public:
    ChipMcuNfc(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipMcuNfc();

    XBH_S32 resetChip();
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    XBH_S32 getChipExist(XBH_BOOL* enable);
    XBH_S32 setNfcEnable(XBH_BOOL enable);
    XBH_S32 getNfcEnable(XBH_BOOL *enable);
    XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    XBH_S32 addNfcId(XBH_U8 *data, XBH_U32 length);
    XBH_S32 delNfcId(XBH_U8 *data, XBH_U32 length);
    XBH_S32 delAllNfcId();
    XBH_S32 switchNfcType();
    XBH_S32 setNfcLowPower(const XBH_U8* u8Data);
    XBH_S32 getNfcLowPower(XBH_BOOL *enable);
    XBH_S32 getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length);

private:
    XBH_S32 init();
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    
    XBH_BOOL m_bInitDone;
};

#endif 
