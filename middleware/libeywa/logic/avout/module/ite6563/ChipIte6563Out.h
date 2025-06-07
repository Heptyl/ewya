#ifndef XBH_CHIP_ITE6563_OUT_H
#define XBH_CHIP_ITE6563_OUT_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAVOutInterface.h"
#include <hardware/board.h>
#include "XbhService.h"


#define ITE6563_I2C_ADDR            0x5C
#define ITE6563_UPGRADE_START       (0x2000)
#define ITE6563_UPGRADE_END         (0x2400)
#define ITE6563_HEAD_LENGTH         (0x1000)
#define ITE6563_EMMC_START          (0x1000)
#define ITE6563_EMMC_END            (0x10000)
#define ITE6563_END_LENGTH          (0xB000)
#define ITE6563_FILE_WRITE_OFFSET   (0x18)
#define ITE6563_HEAD_OFFSET         (0x0)
#define ITE6563_BODY_OFFSET         (0x1000)
#define ITE6563_RW_DATA_ONCE        (0x80)
#define ITE6563_SECTOR_LENGTH       (0x400) //1k
#define ITE6563_VERSION_OFFSET      (0x2080)
#define ITE6563_VERSION_LENGTH      (16)

class ChipIte6563Out : public XbhMWThread, public XbhAVOutInterface
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
    ChipIte6563Out(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipIte6563Out();
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
    //command
    XBH_U32 genCrc32(XBH_VOID* pBuff, XBH_S32 s32Size);
    XBH_S32 writeI2cByte(XBH_S32 s32RegAddr, XBH_U8* pu8Data);
    XBH_S32 readI2cByte(XBH_S32 s32RegAddr, XBH_U8* pu8Data);
    XBH_S32 writeI2cByteArray(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff);
    XBH_S32 readI2cByteArray(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff);

    //update state
    XBH_S32 enterIspMode();
    XBH_S32 exitIspMode();
    XBH_S32 checkHead(XBH_U8* pu8Buff);
    XBH_S32 enableInterruptMask();
    XBH_S32 eraseSector(XBH_U32 u32WriteOffset, XBH_U32 u32WriteEnd);
    XBH_S32 writeData(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff);
    XBH_S32 verifyData(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff);

    void run(const void* arg);

    XBH_S32 mState;
    XBH_CHAR mFileName[80];
    XBH_BOOL mForceUpgrade;

    XBH_U32 mI2cNum;
    XBH_U32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_CHAR mVersion[ITE6563_VERSION_LENGTH]; 
    XBH_BOOL mUpdateEnd;
//---------------- custom device interface end ----------------
};

#endif //XBH_CHIP_ITE6563_OUT_H
