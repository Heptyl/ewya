#ifndef CHIP_CS5803AQ_H
#define CHIP_CS5803AQ_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAVOutInterface.h"

#define CS5803AQ_GET_IC_ID           (0X010003)
#define CS5803AQ_GET_IC_VER_L        (0X0100C2)
#define CS5803AQ_GET_IC_VER_H        (0X0100C1)
#define CS5803AQ_DISABLE_SPI_MODE    (0X10004F)  //write 0X00
#define CS5803AQ_EXTERNAL_FLASH      (0X200030)  //write 0X11


class ChipCS5803AQ : public XbhMWThread, public XbhAVOutInterface
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
    ChipCS5803AQ(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipCS5803AQ();
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

public:
    enum XBH_CS5803_STATE_E {
        CS5803_IDLE        = 0,
        CS5803_RUNNING     = 1,
        CS5803_SUCCESS     = 2,
        CS5803_FAILURE     = 3,
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

#endif //XBH_CHIP_CS5803AQ_H

