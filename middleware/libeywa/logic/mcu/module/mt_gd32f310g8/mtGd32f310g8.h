#ifndef XBH_MTGD32F310G8_H
#define XBH_MTGD32F310G8_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhMcuInterface.h"

#define ExtMcu_DEFAULT_FIREWARE_PATH  "/vendor/firmware/"


#define EXT_Mcu_FLAG_OFFSET        (0x3000)
#define EXT_Mcu_ENTRY_OFFSET       (0x4000)

#define EXT_MCU_DEF_VERSION        (0xA000)
#define MCU_I2C_DATA_MAX    128

class mtGd32f310g8 : public XbhMWThread, public XbhMcuInterface
{
public:
    mtGd32f310g8(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~mtGd32f310g8();

    XBH_S32 resetChip();
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    XBH_S32 getUpgradeState(XBH_S32 * s32State);
    XBH_S32 setMcuFattMode(XBH_S32 mode);
    XBH_S32 getMcuFattMode(XBH_S32 *mode);
    XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    XBH_S32 setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud);
    XBH_S32 setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    XBH_S32 getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);

private:
    void run(const void* arg);
    XBH_S32 upgradetask(void);
    XBH_S32 upgrade_download_data(XBH_U8 *buff, XBH_U32 length);
    XBH_S32 set_iap_status(XBH_U8 status);
    XBH_S32 set_enter_iap_mode(void);
    XBH_S32 WriteMcuMultiBytes(XBH_U8 s32RegAddr, XBH_U32 u32Length,  XBH_U8 *pu8Data);
    XBH_S32 ReadMcuMultiBytes(XBH_U8 s32RegAddr, XBH_U32 u32Length, XBH_U8* pu8Data);
    XBH_S32 calculate_CRC32_Mpeg2(void *pStart, unsigned int uiSize);
    XBH_S32 check_iap_CRC32(XBH_U32 pc_crc);
    XBH_U32 getPresetMcuFirewareCrc32(XBH_VOID* pStart, XBH_S32 s32Size);
    XBH_S32 getPresetMcuFireware(XBH_CHAR* path, XBH_CHAR* pFileName);
    XBH_S32 getPresetMcuVersion(XBH_CHAR* pFileName);
    XBH_S32 getPresetMcuData(XBH_U32* u32FileLen, XBH_U8** pFileBuff);
    XBH_S32 getExtMcuAppVersion();
    XBH_S32 setEnterIapMode();
    XBH_S32 getMcuVersion(XBH_S32 *mcuVer);
    XBH_S32 downloadPresetFirewareData(XBH_U8* pFileBuff, XBH_U32 u32FileLen);
    XBH_S32 checkPresetFirewareLen(XBH_U32 u32FileLen);
    XBH_S32 checkExtMcuCrc(XBH_U32 u32PresetFileCrc);
    XBH_S32 setIapReboot();
    XBH_S32 MCUUpgrade(XBH_BOOL bForceUpgrade);
    XBH_S32 mcuReadBytes(XBH_MCU_I2CBUFFDEF_S* pI2cBuff);


    static XbhMutex mLock;
    XBH_CHAR strPresetfilepath[80];
    char mFileName[80];
    XBH_S32 mI2cBus;            //i2c端口
    XBH_S32 mI2cAddr;           //设备地址
    XBH_S32 mState;             //升级状态
    XBH_S32 mUpgradeStatus;     //需要升级标志
    XBH_BOOL mForceUpgrade;     //强制更新
    XBH_S32 mPgpio;             //供电引脚，没有就填-1
    XBH_S32 mPLevel;            //工作时的电平状态
    XBH_S32 mRgpio;             //复位引脚，没有就填-1
    XBH_S32 mRLevel;            //工作时的电平状态
    enum XBH_GD32_STATE_E {
        GD32_IDLE        = 0,
        GD32_RUNNING     = 1,
        GD32_SUCCESS     = 2,
        GD32_FAILURE     = 3,
    };
};

#endif //XBH_mtGd32f310g8_H