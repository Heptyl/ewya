#ifndef XBH_CHIP_RTS5450M_H
#define XBH_CHIP_RTS5450M_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhPdIcInterface.h"

#define RTS5450_ROM_CODE  (0x00)
#define RTS5450_MCM_CODE  (0x01)
#define RTS5450_RUN_BANK0  (0x00)
#define RTS5450_RUN_BANK1  (0x10)
#define RTS5450_FLASH_SIZE (0x10000)
#define SMBUS_WRITE_FLASH_0_64K (0x04)
#define SMBUS_WRITE_FLASH_64_128K (0x06)
#define SMBUS_WRITE_FLASH_128_192K (0x13)
#define SMBUS_WRITE_FLASH_192_256K (0x14)

#define SMBUS_READ_FLASH_0_64K (0X24)
#define SMBUS_READ_FLASH_64_128K (0X26)
#define SMBUS_READ_FLASH_128_192K (0X33)
#define SMBUS_READ_FLASH_192_256K (0X34)

#define RTS5450_GET_IC_STATUS (0X3A030000)
#define RTS5450_SET_VENDOR_CMD (0x0103da0b)

// 65W（20V/3.25A）
static XBH_U8 RTS5450_SET_PDO_65W[] = {
    0x17, 0x03, 0x00, 0x0d,
    0x2c, 0x91, 0x91, 0x3f,
    0x2c, 0xd1, 0x02, 0x00,
    0x2c, 0xc1, 0x03, 0x00,
    0x2c, 0xb1, 0x04, 0x00,
    0x45, 0x41, 0x06, 0x00,
};
// 75W（20V/3.75A）
static XBH_U8 RTS5450_SET_PDO_75W[] = {
    0x17, 0x03, 0x00, 0x0d,
    0x2c, 0x91, 0x91, 0x3f,
    0x2c, 0xd1, 0x02, 0x00,
    0x2c, 0xc1, 0x03, 0x00,
    0x2c, 0xb1, 0x04, 0x00,
    0x77, 0x41, 0x06, 0x00,
};
// 85W（20V/4.25A）
static XBH_U8 RTS5450_SET_PDO_85W[] = {
    0x17, 0x03, 0x00, 0x0d,
    0x2c, 0x91, 0x91, 0x3f,
    0x2c, 0xd1, 0x02, 0x00,
    0x2c, 0xc1, 0x03, 0x00,
    0x2c, 0xb1, 0x04, 0x00,
    0xa9, 0x41, 0x06, 0x00,
};
// 90W（20V/4.5A）
static XBH_U8 RTS5450_SET_PDO_90W[] = {
    0x17, 0x03, 0x00, 0x0d,
    0x2c, 0x91, 0x91, 0x3f,
    0x2c, 0xd1, 0x02, 0x00,
    0x2c, 0xc1, 0x03, 0x00,
    0x2c, 0xb1, 0x04, 0x00,
    0xc2, 0x41, 0x06, 0x00,
};
// 96W（20V/4.8A）
static XBH_U8 RTS5450_SET_PDO_96W[] = {
    0x17, 0x03, 0x00, 0x0d,
    0x2c, 0x91, 0x91, 0x3f,
    0x2c, 0xd1, 0x02, 0x00,
    0x2c, 0xc1, 0x03, 0x00,
    0x2c, 0xb1, 0x04, 0x00,
    0xe0, 0x41, 0x06, 0x00,
};

class ChipRts5450M : public XbhMWThread, public XbhPdIcInterface
{
//---------------- custom device interface start ----------------
public:
    /**
     * i2cBus       使用的IIC序号
     * Addr         芯片IIC地址
     * pGpio        芯片供电引脚
     * pLevel       供电工作时的电平状态
     * rGpio        芯片复位引脚
     * rLevel       复位工作时的电平状态
    */
    ChipRts5450M(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipRts5450M();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip();

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

    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);

    /**
     设置typec功率
     */
    XBH_S32 setTypecCapacity(XBH_U8 value);

private:
    //i2c
    XBH_S32 i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 i2c_data_read(unsigned char *p_data, unsigned int data_len);
    XBH_S32 i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value);
    XBH_S32 i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    //command
    XBH_S32 open_upgrade_file(const char *upgrade_file_path);
    XBH_S32 MP_SM_Status(XBH_U8 read, XBH_U8 get_num, XBH_U8* ic_status);
    XBH_S32 MP_SM_Enable_VDCMD(XBH_U8 status);
    XBH_S32 MP_SM_FlashErase();
    XBH_S32 MP_SM_FlashWrite(XBH_U8 status, XBH_U8 run_brank);
    XBH_S32 MP_SM_FlashRead(XBH_U8 status, XBH_U8 run_brank);
    XBH_S32 MP_SM_Validation();
    XBH_S32 MP_SM_Reset2Flash();

    unsigned int bitsreverse(unsigned int inVal, unsigned char bits);

public:
    enum XBH_RTS5450_STATE_E {
        RTS5450_IDLE        = 0,
        RTS5450_RUNNING     = 1,
        RTS5450_SUCCESS     = 2,
        RTS5450_FAILURE     = 3,
    };

private:
    XBH_S32 mState;
    XBH_CHAR mFileName[80];
    XBH_BOOL mForceUpgrade;
    XBH_U32 mI2cNum;
    XBH_U32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    enum XBH_RTS5450_VDCMD_E {
        RTS5450_VDCMD_DISABLE       = 0,
        RTS5450_VDCMD_SMBUS_ENABLE  = 1,
        RTS5450_VDCMD_FLASH_ENABLE  = 2,
    };
//---------------- custom device interface end ----------------
private:
    void run(const void* arg);
    static XbhMutex mLock;
};

#endif //XBH_CHIP_RTS5450M_H
