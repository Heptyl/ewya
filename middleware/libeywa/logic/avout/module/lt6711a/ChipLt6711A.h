#ifndef CHIP_LT6711A_H
#define CHIP_LT6711A_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAVOutInterface.h"

class ChipLt6711A : public XbhMWThread, public XbhAVOutInterface
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
    ChipLt6711A(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipLt6711A();
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
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);

private:
    //i2c
    XBH_S32 i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value);
    XBH_S32 i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    //command
    XBH_S32 open_upgrade_file(const char *upgrade_file_path);
    XBH_S32 lt6711_i2c_to_flash_config(void);
    XBH_S32 lt6711_flash_block_erase(void);
    XBH_S32 lt6711_download_core(void);
    XBH_S32 lt6711_read_byte_compare(unsigned int u32RegAddr, unsigned int RegAddrLen,unsigned char *p_data, unsigned int data_len);
    XBH_S32 lt6711_read_flash(void);
    XBH_S32 lt6711_load_code_to_flash(void);
    XBH_S32 checkVersion();

public:
    enum XBH_LT6711_STATE_E {
        LT6711_IDLE        = 0,
        LT6711_RUNNING     = 1,
        LT6711_SUCCESS     = 2,
        LT6711_FAILURE     = 3,
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

//---------------- custom device interface end ----------------
private:
    void run(const void* arg);
    static XbhMutex mLock;
};

#endif //XBH_CHIP_LT6711UXE1_H

