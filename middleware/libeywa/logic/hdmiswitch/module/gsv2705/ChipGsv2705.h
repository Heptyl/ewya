#ifndef CHIP_GSV2705_H
#define CHIP_GSV2705_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include <list>
#include <string>
#include "XbhMWThread.h"
#include "XbhHdmiSwitchInterface.h"

#define REG_RX_PORT 0xFF05

#define REG_FW_VER 0xFF3E
#define FW_VER_LEN  2

#define REG_TOGGLE_HPD 0xFF08

#define REG_RESET  0xFF09
#define CMD_RESET  0xA2

#define REG_HPD_ASSERT_CTRL 0xFF1D

#define REG_SPA_UPDATE_CMD  0xFF1A
#define REG_SPA_HIGH_ADDR   0xFF1B
#define REG_SPA_LOW_ADDR    0xFF1C

#define REG_SPA_UPDATE_RXA    0x90
#define REG_SPA_UPDATE_RXB    0xA0
#define REG_SPA_UPDATE_RXC    0xB0
#define REG_SPA_UPDATE_RXD    0xC0

#define REG_RX_STATUS 0xFF07
#define REG_TX_STATUS 0xFF0C

#define XBH_GSV2705_DEVICE_ADDRESS                  (0x58)

#define XBH_GSV2705_DEVICE_CHIP_NAME_ADD            (0xff3c)
#define XBH_GSV2705_DEVICE_CHIP_NAME_LEN            (2)

#define XBH_GSV2705_DEVICE_FIRMWARE_VERSION_ADD     (0xff3e)
#define XBH_GSV2705_DEVICE_FIRMWARE_VERSION_LEN     (2)

#define XBH_GSV2705_MAILBOX_MAP_ADDR                (0xff)

#define XBH_GSV2705_APP_OFFSET_AT_FLASH             (0x0)
#define XBH_GSV2705_APP_VERSION_MAJOR               (1)
#define XBH_GSV2705_APP_VERSION_MINOR               (0)

#define XBH_GSV2705_DEVICE_ADDRESS                  (0x58)
#define XBH_GSV2705_TRANSFER_START                  (0x55)
#define XBH_GSV2705_TRANSFER_END                    (0xAA)

#define XBH_GSV2705_ROLL_BACK_INCDATA               (0x10)
#define XBH_GSV2705_RXDDC_INFO_CMD                  (0x09)
#define XBH_GSV2705_RXDDC_DATA_CMD                  (0x06)
#define XBH_GSV2705_RXDDC_INFO_ACK                  (0x09)
#define XBH_GSV2705_RXDDC_DATA_ACK                  (0x0A)

//I2C
#define XBH_GSV2705_RXDDC_UPDCONFIGID               (0xFF22)
#define XBH_GSV2705_RXDDC_UPDREPLYID                (0xFF27)
#define XBH_GSV2705_RXDDC_REPLY                     (0xFF2B)
#define XBH_GSV2705_RXDDC_COMMAND                   (0xFF2C)
#define XBH_GSV2705_RXDDC_REBOOT                    (0xFF2D)
#define XBH_GSV2705_RXDDC_UPDINFO                   (0xFF30)
#define XBH_GSV2705_RXDDC_INFOCHECKSUM              (0xFF2F)
#define XBH_GSV2705_RXDDC_UPDDATA                   (0xFF40)
#define XBH_GSV2705_RXDDC_DATACHECKSUM              (0xFF80)

#define XBH_GSV2705_PARTITION_A_ADDR                (0x0001000)
#define XBH_GSV2705_PARTITION_B_ADDR                (0x0040000)

#define XBH_GSV2705_PARTTION_A                      (0)
#define XBH_GSV2705_PARTTION_B                      (1)

#define XBH_TRANSFER_DATA_BLKSIZE                   (64)

#define GSV2705_VERSION                "persist.vendor.xbh.ver.gsv2705"


enum GSV2705Upgrade_STATE_E {
       GSV2705Upgrade_IDLE          = 0,
       GSV2705Upgrade_RUNNING       = 1,
       GSV2705Upgrade_SUCCESS       = 2,
       GSV2705Upgrade_FAILURE       = 3,
};



enum
{
    GSV2705_PORT_RX_DEF,
    GSV2705_PORT_RX_A,
    GSV2705_PORT_RX_B,
    GSV2705_PORT_RX_C,
    GSV2705_PORT_RX_D,
};

enum GSV2705_WDT_STATE_E {  
    GSV2705_WDT_READY       = 0,  // 即将开始
    GSV2705_WDT_STARTING    = 1,  // 开始运行
    GSV2705_WDT_RUNNING     = 2,  // 运行中
    GSV2705_WDT_UPGRADING   = 3,  // 升级中
};


static const unsigned char crc8Gsv2705Table[256] =
{
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

static XBH_U8 dataGsv2705Buff[128] =  {0};


typedef struct {
    XBH_U32 DataTotalLen;
    XBH_U32 DataOffsetAtFlash;
    XBH_U8 VersionMajor;
    XBH_U8 VersionMinor;
    XBH_U16 GsvChipNumber;
}GSV2705UpgradeInfo;

class ChipGsv2705 : public XbhMWThread, public XbhHdmiSwitchInterface
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
    ChipGsv2705(XBH_S32 i2cNumber, XBH_S32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel , XBH_S32 defaultPort , HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level);

    ~ChipGsv2705();

    XBH_S32 setPowerChip(XBH_BOOL onoff);

    XBH_S32 resetChip(XBH_BOOL hw);

    XBH_S32 setRxHpd(XBH_U8 port, XBH_BOOL high);

    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    XBH_S32 setActivePort(XBH_U8 port);

    XBH_S32 getActivePort(XBH_U8* port);

    XBH_S32 getRxPortStatus(XBH_U8 port, XBH_U8* status);

    XBH_S32 getTxPortStatus(XBH_U8* status);

    XBH_S32 getUpgradeState(XBH_S32 *pState);

    XBH_S32 powerUpProtection(void);

    XBH_S32 setPhysicalAddr(XBH_U8 port, XBH_U16 cecAddr);

    void wdtStart();

private:
    XBH_S32 mI2cNumber;
    XBH_S32 mI2cAddress;
    XBH_S32 mCurrentPort;
    XBH_S32 mPowerGpio;
    XBH_S32 mPowerLevel;
    XBH_S32 mResetGpio;
    XBH_S32 mResetLevel;
    XBH_S32 mDefaultPort;
    XBH_S32 mWdtRunning;
    XBH_S32 mWdtResetCount;
    XBH_BOOL mHasNextPort;
    HDMI_SWITCH_PORT m_u8SwitchPort;

    //upgrade
    static XbhMutex mLock;
    XBH_U8 *partition_data;
    XBH_S32 upgradeState;
    XBH_S32 mDevType;
    XBH_CHAR file_name[128];
    XBH_U8 buffer[16];
    XBH_U32 patition_start_addr;
    XBH_U8 RXDDCCmd;
    XBH_S8 McuRunAtBTag;
    XBH_U32 RXDDCDataPoint;
    XBH_BOOL forceUpgrade;
    XBH_U8 UpdConfigID[4];
    XBH_U8 UpdReplyID[4];
    void reset(void);
    void run(const void* arg);
    XBH_U8 calcCRC8(const XBH_U8 *data, XBH_U32 len);
    XBH_S32 readData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth);
    XBH_S32 writeData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth);
    XBH_S32 writeRXDDCUpdConfigID(void);
    XBH_S8 writeRXDDCUpdInfo(XBH_U32 appSize, XBH_U32 appAddr);
    XBH_S8 writeRXDDCUpdData(XBH_U8 *pbuf);
    XBH_S8 writeMsgCmd(void);
    XBH_S8 waitMsgAckNew(void);
    XBH_S8 msgCmdAndWaitNew(void);
    XBH_S8 appI2CUpdateInit(void);
    XBH_S8 appI2CUpdateServer(XBH_U8 * appData, XBH_U32 appSize, XBH_U32 appAddr);
    XBH_S32 updateEdid(XBH_U8 edid_data[256],XBH_U8 port);
};

#endif //CHIP_GSV2705_H
