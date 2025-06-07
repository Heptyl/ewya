#ifndef CHIP_LT8668SX_H
#define CHIP_LT8668SX_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include <list>
#include <string>
#include "XbhMWThread.h"
#include "XbhHdmiSwitchInterface.h"

#define LT8668SX_VERSION                             "persist.vendor.xbh.ver.lt8668sx"
#define LT8668SX_KEY                                 "persist.vendor.xbh.key.lt8668sx"

enum
{
    LT8668SX_PORT_RX_DEF,
    LT8668SX_PORT_RX_A,
    LT8668SX_PORT_RX_B,
    LT8668SX_PORT_RX_C,
    LT8668SX_PORT_RX_D,
};

#define LT8668SX_UPGRADE_START       (0x2000)
#define LT8668SX_UPGRADE_END         (0x2400)
#define LT8668SX_HEAD_LENGTH         (0x1000)
#define LT8668SX_EMMC_START          (0x1000)
#define LT8668SX_EMMC_END            (0x10000)
#define LT8668SX_END_LENGTH          (0xB000)
#define LT8668SX_FILE_WRITE_OFFSET   (0x18)
#define LT8668SX_HEAD_OFFSET         (0x0)
#define LT8668SX_BODY_OFFSET         (0x1000)
#define LT8668SX_RW_DATA_ONCE        (0x80)
#define LT8668SX_SECTOR_LENGTH       (0x400) //1k
#define LT8668SX_VERSION_OFFSET      (0x2080)
#define LT8668SX_VERSION_LENGTH      (16)

typedef struct
{
    unsigned char Width;
    unsigned int  Poly;
    unsigned int  CrcInit;
    unsigned int  XorOut;
    unsigned char RefIn;
    unsigned char RefOut;
}CrcInfoTypeS;

class ChipLt8668SX : public XbhMWThread, public XbhHdmiSwitchInterface
{
public:
    /**
     * i2cNumber        i2c bus 序号
     * i2cAddress       i2c 设备地址
     * powerGpio        上电GPIO
     * powerLevel       工作电平
     * resetGpio        复位GPIO
     * resetLevel       复位电平
     * defaultPort      默认port
     * u8SwitchPort     各个port对应的通道序号
     */
    ChipLt8668SX(XBH_S32 i2cNumber, XBH_S32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel , XBH_S32 defaultPort , HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level);

    ~ChipLt8668SX();

    XBH_S32 setPowerChip(XBH_BOOL onoff);

    XBH_S32 resetChip(XBH_BOOL hw);
    
    XBH_S32 getChipExist(XBH_BOOL* enable);

    XBH_S32 setRxHpd(XBH_U8 port, XBH_BOOL high);

    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    XBH_S32 setHdcpKey(const XBH_CHAR* strFilePath);

    XBH_S32 getHdcpKeyName(XBH_CHAR* strHdcpFileName);

    XBH_S32 setHdcpKeyName(const XBH_CHAR* strHdcpFileName);

    XBH_S32 setActivePort(XBH_U8 port);

    XBH_S32 getActivePort(XBH_U8* port);

    XBH_S32 getRxPortStatus(XBH_U8 port, XBH_U8* status);

    XBH_S32 getTxPortStatus(XBH_U8* status);

    XBH_S32 getUpgradeState(XBH_S32 *pState);

private:
    //i2c
    XBH_S32 i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value);
    XBH_S32 i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 lt8668sx_i2c_write_data_to_flash(int Addr,unsigned char data);
    XBH_S32 lt8668sx_i2c_write_key_to_flash(int Addr);
    XBH_S32 lt8668sx_i2c_read_data_from_flash(int Addr);
    XBH_S32 lt8668sx_i2c_read_key_from_flash(int Addr);
    //command
    XBH_S32 open_upgrade_file(const char *upgrade_file_path);
    XBH_S32 lt8668sx_i2c_to_flash_config(void);
    XBH_S32 lt8668sx_flash_block_erase(int Addr);
    XBH_S32 lt8668sx_download_core(int Addr);
    XBH_S32 lt8668sx_read_byte_compare(unsigned int u32RegAddr, unsigned int RegAddrLen,unsigned char *p_data, unsigned int data_len);
    XBH_S32 lt8668sx_read_flash(int Addr);
    XBH_S32 lt8668sx_load_code_to_flash(int Addr);
    XBH_S32 checkVersion();
    XBH_U32 bitsreverse(unsigned int inVal, unsigned char bits);
    XBH_U32 getcrc(CrcInfoTypeS type, unsigned char *buf, unsigned int bufLen);
    XBH_U8  lt8668sx_get_crc(unsigned char *upgradeData, unsigned int len);
    XBH_U8  lt8668sx_get_key(const char *upgrade_file_path);


    void run(const void* arg);
    XBH_S32 mState;
    XBH_CHAR mFileName[128];
    XBH_UCHAR mHdcpKey[64];
    XBH_CHAR g_key_val[64];
    XBH_BOOL mForceUpgrade;
    XBH_BOOL mHasNextPort;

    XBH_S32 mI2cNumber;
    XBH_S32 mI2cAddress;
    XBH_S32 mCurrentPort;
    XBH_S32 mPowerGpio;
    XBH_S32 mPowerLevel;
    XBH_S32 mResetGpio;
    XBH_S32 mResetLevel;
    XBH_S32 mDefaultPort;
    HDMI_SWITCH_PORT m_u8SwitchPort;
};

#endif //CHIP_LT8668SX_H
