#ifndef XBH_CHIP_LT8711_H
#define XBH_CHIP_LT8711_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhUsbc2HdmiInterface.h"


#define LT8711_UPGRADE_START       (0x2000)
#define LT8711_UPGRADE_END         (0x2400)
#define LT8711_HEAD_LENGTH         (0x1000)
#define LT8711_EMMC_START          (0x1000)
#define LT8711_EMMC_END            (0x10000)
#define LT8711_END_LENGTH          (0xB000)
#define LT8711_FILE_WRITE_OFFSET   (0x18)
#define LT8711_HEAD_OFFSET         (0x0)
#define LT8711_BODY_OFFSET         (0x1000)
#define LT8711_RW_DATA_ONCE        (0x80)
#define LT8711_SECTOR_LENGTH       (0x400) //1k
#define LT8711_VERSION_OFFSET      (0x2080)
#define LT8711_VERSION_LENGTH      (16)

class ChipLt8711 : public XbhMWThread, public XbhUsbc2HdmiInterface
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
    ChipLt8711(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipLt8711();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip();

    /**
     * 设置当前信源的HPD状态
     * value 高低状态
     */
    XBH_S32 setRxHpd(XBH_U8 value);

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

private:
    //i2c
    XBH_S32 i2c_data_read(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    XBH_S32 i2c_data_write(unsigned char u8RegAddr, unsigned char reg_value);
    XBH_S32 i2c_data_write_nbytes(unsigned int u32RegAddr, unsigned int RegAddrLen, unsigned char *p_data, unsigned int data_len);
    //command
    XBH_S32 lt8711_get_chip_id();
    XBH_S32 lt8711_get_version();
    XBH_S32 lt8711_init(const char *upgrade_file_path);
    XBH_S32 lt8711_i2c_to_flash_config(void);
    XBH_S32 lt8711_read_key();
    XBH_S32 lt8711_flash_block_erase(void);
    XBH_S32 lt8711_download_core(void);
    XBH_S32 lt8711_download_key(unsigned char *Data);
    XBH_S32 lt8711_read_byte_compare(unsigned int u32RegAddr, unsigned int RegAddrLen,unsigned char *p_data, unsigned int data_len);
    XBH_S32 lt8711_read_key_compare(unsigned char *Data);
    XBH_S32 lt8711_read_flash(void);
    XBH_S32 lt8711_load_code_to_flash(void);
    XBH_S32 lt8711_uninit(void);

    XBH_S32 checkVersion();

public:
    enum XBH_LT8711_STATE_E {
        LT8711_IDLE        = 0,
        LT8711_RUNNING     = 1,
        LT8711_SUCCESS     = 2,
        LT8711_FAILURE     = 3,
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

#endif //XBH_CHIP_LT8711_H
