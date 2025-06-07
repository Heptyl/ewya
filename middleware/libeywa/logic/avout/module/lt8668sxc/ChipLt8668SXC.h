#ifndef CHIP_LT8668SXC_H
#define CHIP_LT8668SXC_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include <list>
#include <string>
#include "XbhMWThread.h"
#include "XbhAVOutInterface.h"

#define LT8668SXC_VERSION                             "persist.vendor.xbh.ver.lt8668sxc"

enum
{
    LT8668SXC_PORT_RX_DEF,
    LT8668SXC_PORT_RX_A,
    LT8668SXC_PORT_RX_B,
    LT8668SXC_PORT_RX_C,
    LT8668SXC_PORT_RX_D,
};

#define LT8668SXC_UPGRADE_START       (0x2000)
#define LT8668SXC_UPGRADE_END         (0x2400)
#define LT8668SXC_HEAD_LENGTH         (0x1000)
#define LT8668SXC_EMMC_START          (0x1000)
#define LT8668SXC_EMMC_END            (0x10000)
#define LT8668SXC_END_LENGTH          (0xB000)
#define LT8668SXC_FILE_WRITE_OFFSET   (0x18)
#define LT8668SXC_HEAD_OFFSET         (0x0)
#define LT8668SXC_BODY_OFFSET         (0x1000)
#define LT8668SXC_RW_DATA_ONCE        (0x80)
#define LT8668SXC_SECTOR_LENGTH       (0x400) //1k
#define LT8668SXC_VERSION_OFFSET      (0x2080)
#define LT8668SXC_VERSION_LENGTH      (16)

typedef struct
{
    unsigned char Width;
    unsigned int  Poly;
    unsigned int  CrcInit;
    unsigned int  XorOut;
    unsigned char RefIn;
    unsigned char RefOut;
}CrcInfoTypeS;

class ChipLt8668SXC : public XbhMWThread, public XbhAVOutInterface
{
public:
    /**
     * i2cBus           i2c bus 序号
     * Addr             i2c 设备地址
     * powerGpio        上电GPIO
     * powerLevel       工作电平
     * resetGpio        复位GPIO
     * resetLevel       工作电平
     */
    ChipLt8668SXC(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);

    ~ChipLt8668SXC();

    XBH_S32 resetChip(XBH_BOOL hw);
    
    XBH_S32 getChipExist(XBH_BOOL* enable);

    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    XBH_S32 getUpgradeState(XBH_S32 *pState);

private:
    //i2c
    XBH_S32 i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value);
    XBH_S32 i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 lt8668sxc_i2c_write_data_to_flash(int Addr,unsigned char data);
    XBH_S32 lt8668sxc_i2c_read_data_from_flash(int Addr);
    //command
    XBH_S32 open_upgrade_file(const char *upgrade_file_path);
    XBH_S32 lt8668sxc_i2c_to_flash_config(void);
    XBH_S32 lt8668sxc_flash_block_erase(int Addr);
    XBH_S32 lt8668sxc_download_core(int Addr);
    XBH_S32 lt8668sxc_read_byte_compare(unsigned int u32RegAddr, unsigned int RegAddrLen,unsigned char *p_data, unsigned int data_len);
    XBH_S32 lt8668sxc_read_flash(int Addr);
    XBH_S32 lt8668sxc_load_code_to_flash(int Addr);
    XBH_S32 checkVersion();
    XBH_U32 bitsreverse(unsigned int inVal, unsigned char bits);
    XBH_U32 getcrc(CrcInfoTypeS type, unsigned char *buf, unsigned int bufLen);
    XBH_U8  lt8668sxc_get_crc(unsigned char *upgradeData, unsigned int len);

    void run(const void* arg);
    XBH_S32 mState;
    XBH_CHAR mFileName[80];
    XBH_BOOL mForceUpgrade;

    XBH_S32 mI2cNumber;
    XBH_S32 mI2cAddress;
    XBH_S32 mPowerGpio;
    XBH_S32 mPowerLevel;
    XBH_S32 mResetGpio;
    XBH_S32 mResetLevel;
};

#endif //CHIP_LT8668SXC_H
