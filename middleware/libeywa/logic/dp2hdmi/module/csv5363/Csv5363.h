#ifndef XBH_CSV5363_H
#define XBH_CSV5363_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhDp2HdmiInterface.h"
#include "XbhService.h"


#define CSV5363_I2C_ADDR            0x6A
//new FW version for update
#define New_FW_Ver  0x000006
//Chip Device_Address
#define ASL_DEV_I2C_ADDRESS 0x6A  //ASL DEV 
#define SPI_Address_Length 3
//The data for upgrading is converted from bin file or directly read from the file system.

#define CSV5363_GET_IC_ID            (0X010003)
#define CSV5363_GET_IC_VER_C1        (0X0100C1)
#define CSV5363_GET_IC_VER_C2        (0X0100C2)
#define CSV5363_GET_IC_VER_C3        (0X0100C3)

#define CSV5363_DEFAULT_FIREWARE    "/mnt/cust/firmware/CSV5363/CSV5363.bin"

class Csv5363 : public XbhMWThread, public XbhDp2HdmiInterface
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
    Csv5363(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~Csv5363();
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
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
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

private:
    //=======================Readme============================
    //The entry for upgrading is function: SPI_Flash_update()
    //Porting code needs to implement the following functions:
    //=========================================================

    //ASL I2C read&write funcion,for detail, please refer “ApplicationNote--ASL_I2C_Read_Write_V1.1.pdf”
    //Writ register funcition: ASL_DEV_I2C_ADDRESS(W)-->BLOCK_ID-->offsetH-->offsetL-->data0,data1,data2...
    //Read register funcition: ASL_DEV_I2C_ADDRESS(W)-->BLOCK_ID-->offsetH-->offsetL-->ASL_DEV_I2C_ADDRESS(R)-->data0,data1,data2...

    //The following functions are used to implement read or write one register from ASL chip
    XBH_S32 ASL_I2C2_Read_Reg(XBH_U32 BLOCK_ID, XBH_U8 *pData);
    XBH_S32 ASL_I2C2_Write_Reg(XBH_U32 BLOCK_ID, XBH_U8 wData);

    //The following functions are used to implement continuous read and write of ASL registers.
    XBH_S32 ASL_I2C2_Read_Reg_Len(XBH_U32 BLOCK_ID, XBH_U8 *pData, XBH_U8 Num);
    XBH_S32 ASL_I2C2_Write_Reg_Len(XBH_U32 BLOCK_ID, XBH_U8 *wData, XBH_U8 Num);

    XBH_U8 wdata[16];
    XBH_U32 Erased_Address_I2C=0; //Erased address+1
    XBH_U32 Each_Time_Erase_Size=0x1000; //erase block size 
    XBH_U32 Offset=0;

    XBH_S32 open_upgrade_file(const char *upgrade_file_path);
    XBH_S32 I2C_Wait_RW_DONE(XBH_U32 Timout);
    XBH_S32 I2C_write_done_check(XBH_U8 Timeout);
    XBH_S32 SPI_Flash_Protect_I2C(XBH_BOOL Protect_EN);
    void I2C_Flash_Erase(XBH_U32 StartAddr);
    XBH_S32 I2C2_SPI_Flash_Read(XBH_U32 ReadAddr,XBH_U8 *pBuffer,XBH_U32 NumToRead);
    XBH_S32 I2C2_SPI_Flash_Write(XBH_U32 WriteAddr,XBH_U8 *pBuffer,XBH_U32 NumByteToWrite);
    void Erase_Flash_init_I2C(XBH_U32 EAddr);
    void Erase_SPIFlash_I2C(XBH_U32 CurAddr);
    XBH_S32 Chip_init_5366();
    XBH_BOOL SPI_Flash_update_5366();

public:
    enum XBH_CSV5363_STATE_E {
        CSV5363_IDLE        = 0,
        CSV5363_RUNNING     = 1,
        CSV5363_SUCCESS     = 2,
        CSV5363_FAILURE     = 3,
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

#endif //XBH_CHIP_CSV5363_H
