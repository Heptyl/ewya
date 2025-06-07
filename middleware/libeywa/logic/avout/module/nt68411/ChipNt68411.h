#ifndef XBH_CHIP_NT_68411_H
#define XBH_CHIP_NT_68411_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhAVOutInterface.h"
#include <hardware/board.h>
#include "XbhService.h"
#include <cutils/properties.h>

#define XBH_BOARD_GPIO_NT68411_SDA       135
#define XBH_BOARD_GPIO_NT68411_SCL       136

#define NORMAL_MODE 0
#define FE2P_MODE 1
#define DIRECT_FE2P_MODE 2

#define CMD_SET_EXTEND_ADDR     (0x0f)
#define CMD_GET_VERSION         (0x11)
#define CMD_CHIP_ERASE          (0x1E)
#define CMD_BLOCK_ERASE         (0x2D)
#define CMD_GET_CHECK_SUM       (0x36)
#define CMD_WRITE_PAGE_24M      (0x3C)
#define CMD_SPI_CMD             (0x53)
#define CMD_SET_REG_SRAM        (0x78)
#define CMD_GET_REG_SRAM        (0x87)
#define CMD_MULTI_FUNC          (0x99)

#define TOTAL_PAGE_SIZE         (4096)
#define ONE_PAGE_SIZE           (512)

#define VERSION_DEFAULT                 0xffff
#define REG_ADDR_COUNT                  1

#define NT68411_HOST_ADDR               0x51
#define NT68411_CMD_VERSION             {0x82, 0x01, 0xc9, 0x6e ^ 0x51 ^ 0x82 ^ 0x01 ^ 0xc9}
#define NT68411_HDMI_TX_PLUG_STATE      {0x82, 0x01, 0xa6, 0x6e ^ 0x51 ^ 0x82 ^ 0x01 ^ 0xa6}
#define NT68411_CMD_LEN                 5
#define NT68411_RSP_LEN                 10
#define NT68411_READ_HIGH_BYTE          8
#define NT68411_READ_LOW_BYTE           9

#define Nt68411_Set_Isp_ADDR            0x04
#define Nt68411_Set_Isp_ADDR2           0x34
#define Nt68411_Set_Isp_ADDR3           0x06
#define Nt68411_Set_Isp_ADDR4           0x36
#define Nt68411_Exit_Isp_ADDR4          0x3C

#define Nt68411_HDMI_TX_RGB             0X01
#define Nt68411_HDMI_TX_YUV422          0X02
#define Nt68411_HDMI_TX_YUV444          0X03

#define Nt68411_HDMI_TX_PLUG_IN_STATE   0X01
#define Nt68411_HDMI_TX_PLUG_OUT_STATE  0X00

#define PINCTRL_NARCOS_DEV         "/proc/pinctrl/pinctrl_narcos"
#define PANEL_DIV_SEC              "/sys/devices/platform/mediatek-drm-tv-kms/panel/panel_div_sec"

typedef struct{
    uint8_t En;
    uint8_t ID1;
    uint8_t ID2;
    uint8_t ID3;
    uint8_t FE2P;
    uint8_t ByteWrite;
    uint8_t SE_Code;
    uint8_t CE_Code;
    uint16_t Page;
    uint16_t SE_Time;
    uint16_t CE_Time;
    uint8_t Name[22];
}FLASH_DEF;

class ChipNt68411 : public XbhMWThread, public XbhAVOutInterface
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
    ChipNt68411(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipNt68411();

    /**
     * 设置HDMI TX的音频mute状态
    */
    XBH_S32 setHdmiTxAudioMute(XBH_BOOL bEnable);
    /**
     * 获取HDMI TX的音频mute状态
    */
    XBH_S32 getHdmiTxAudioMute(XBH_BOOL* info);
    /**
     * 设置HDMI TX的视频mute状态
    */
    XBH_S32 setHdmiTxVideoMute(XBH_BOOL bEnable);
    /**
     * 获取HDMI TX的视频mute状态
    */
    XBH_S32 getHdmiTxVideoMute(XBH_BOOL* info);
    /**
     * 设置HDMI TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 setHdmiTxTiming(XBH_U8 value);
    /**
     * 获取HDMI TX的timing
     * 0：480P，1：1080P， 2：21600P ，0xFF Auto
    */
    XBH_S32 getHdmiTxTiming(XBH_CHAR* info);
    /**
     * 设置HDMI(DP)TX的HDCP开关状态
     * 0：OFF，1：ON
    */
    XBH_S32 setHdmiTxHdcp(XBH_BOOL bEnable);
    /**
     * 获取HDMI TX的HDCP开关状态
    */
    XBH_S32 getHdmiTxHdcp(XBH_BOOL* info);
    /**
    * 设置Vx1 Tx的Tcon模式
     */
    XBH_S32 setVx1TxTconMode(XBH_BOOL bEnable);
    /**
     * 获取68411固件版本
    */
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
     * 设置Pmux模式
    */
    XBH_S32 setPmuxGpio(XBH_BOOL bEnable);
    /**
     * 升级68411固件
    */
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFileName, XBH_BOOL bForceUpgrade);
    /**
     * 获取当前升级的进度
     */
    XBH_S32 getUpgradeState(XBH_S32 * s32State);
    /**
    * 获取HDMI(DP)TX的设备接入情况
    */
    XBH_S32 getHdmiTxPlugState(XBH_BOOL* info);
    /**
     * 开启/关闭 vbo tx 差分信号
    */
    XBH_S32 setVboTxDiffSignals(XBH_BOOL bEnable);
    /**
     * 设置HDMI TX Color Space
    */
    XBH_S32 setHdmiTxColorSpace(XBH_U8 value);



private:

    XBH_U32 mI2cNum;
    XBH_U32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    XBH_S32 mState;
    XBH_CHAR mFileName[80];
    XBH_BOOL mForceUpgrade;
    XBH_S32 mForceMode;

    uint16_t MaxPages;	//Max Flash pages
    uint16_t CodePages;    // f/w programming pages
    uint16_t FlashOffset;   // start page of Flash
    uint16_t SPageSum;
    uint8_t percentage = 0;
    FLASH_DEF Flash;
    uint8_t MultiID;
    uint16_t McuId;
    uint8_t Fe2pMode;
    uint16_t DirFE2P_Offset;
    uint16_t DirFE2P_Length;
    int32_t DualFwEnable = 1;
    uint16_t usSPICmdBufAddr;
    uint8_t ucSPI_BackUpBuf[7] = {0};
    uint32_t WpDefine = 0x1000000;
    uint8_t Wp_PortE = 0;
    uint8_t Wp_PortF = 0;
    uint32_t Wp_PortG_J = 0;
    uint32_t Wp_PortK_M = 0;

    void run(const void* arg);
    void IIC_Start();
    void IIC_Stop();
    void IIC_SendByte(XBH_U8 _ucByte);
    void PulseDelay(uint32_t time);
    void FnSetSPICmdBufAndBackUpValue(void);
    void RestoreSPIRegValue(void);
    bool CheckOverLap(uint16_t PageNo);
    bool CheckSpecialFlash(void);
    uint8_t SearchFlashID(uint8_t mcuFlashID[3]);
    int32_t T_ISPMode(uint8_t enable);
    int32_t T_ISPStatusReply();
    int32_t T_SetRegSRAMWithReply(uint8_t AddrH,uint8_t AddrL,uint8_t Data);
    int32_t T_GetRegSRAMWithReply(uint8_t AddrH,uint8_t AddrL,uint8_t * Data);
    int32_t T_RunNewSPICmdWithReply(void);
    int32_t T_MultiFunc(uint8_t Data);
    int32_t T_GetVersion(void);
    int32_t T_SpiCommandWithReply(uint8_t *Data,uint8_t WLen,uint8_t RLen);
    int32_t T_BlockEraseWithReply(uint8_t From,uint8_t To, uint16_t delayTime);
    int32_t T_ChipEraseWithReply(uint16_t delayTime);
    int32_t T_SetExtendAddrWithReply(uint8_t Block);
    int32_t T_WritePagedataWithReply(uint8_t Pg, uint8_t CurBuf[]);
    int32_t T_GetCheckSumWithReply(uint8_t PageNo);
    int32_t T_GetNt68411DivisionMode(uint32_t *pu32DivisionMode);
    int32_t T_SetNt68411DivisionMode(uint32_t u32DivisionMode);
    int32_t Cmd_T_SendSpecialCommand();
    int32_t Cmd_T_GetFlashID(void);
    int32_t Cmd_T_BlockProtect(bool Wp);
    int32_t Cmd_T_Erase(void);
    int32_t Cmd_T_SetExntend(uint16_t Pg);
    int32_t Cmd_T_Program(uint8_t Pg, uint8_t CurBuf[]);
    int32_t Cmd_T_GetCheckSum(uint8_t Pg);
    int32_t Nt68411VersionSupport(uint16_t version);
    int32_t HalNt68411SetIsp(uint8_t flag);
    int32_t WpEnable(void);
    int32_t SetSectorErasePage(uint8_t  page);
    int32_t BlockErase(uint16_t StartBlock, uint16_t EndBlock);
    int32_t FnEraseIDSector(void);
    int32_t Fn68390ProgramEnable(bool En);
    int32_t FnUpdateIDSector(uint8_t buf[]);
    uint32_t HalNt68411GetDivisionType();

    void T_HandShake(XBH_U32 Timeout);
    XBH_S32 HalNt68411WriteI2c(XBH_U32 i2cNum, XBH_U32 addr, XBH_U32 offset, XBH_U32 length, XBH_U8 *buffer);
    XBH_S32 HalNt68411ReadI2c(XBH_U32 i2cNum, XBH_U32 addr, XBH_U32 offset, XBH_U32 length, XBH_U8 *buffer);
    XBH_S32 T_GetNtVersion(XBH_U32 *version);
//---------------- custom device interface end ----------------
};

#endif  //XBH_CHIP_NT_68411_H
