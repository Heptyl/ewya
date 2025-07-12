#ifndef XBH_AMLOGIC_311D2_H
#define XBH_AMLOGIC_311D2_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhPlatformInterface.h"
#include "XbhModuleInterface.h"
#include <termios.h>

/* GPIO mode definitions */
#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
#define CTL_CLTR(regval)               (BITS(0,1) & ((uint32_t)(regval) << 0))
#define MCU_GPIO_MODE_INPUT            CTL_CLTR(0)           /*!< input mode */
#define MCU_GPIO_MODE_OUTPUT           CTL_CLTR(1)           /*!< output mode */

#define GPIO_MODE_AIN                    ((uint8_t)0x00U)          /*!< analog input mode */
#define GPIO_MODE_IN_FLOATING            ((uint8_t)0x04U)          /*!< floating input mode */
#define GPIO_MODE_IPD                    ((uint8_t)0x28U)          /*!< pull-down input mode */
#define GPIO_MODE_IPU                    ((uint8_t)0x48U)          /*!< pull-up input mode */
#define GPIO_MODE_OUT_OD                 ((uint8_t)0x14U)          /*!< GPIO output with open-drain */
#define GPIO_MODE_OUT_PP                 ((uint8_t)0x10U)          /*!< GPIO output with push-pull */
#define GPIO_MODE_AF_OD                  ((uint8_t)0x1CU)          /*!< AFIO output with open-drain */
#define GPIO_MODE_AF_PP                  ((uint8_t)0x18U)          /*!< AFIO output with push-pull */

/* GPIO output max speed value */
#define GPIO_OSPEED_10MHZ                ((uint8_t)0x01U)          /*!< output max speed 10MHz */
#define GPIO_OSPEED_2MHZ                 ((uint8_t)0x02U)          /*!< output max speed 2MHz */
#define GPIO_OSPEED_50MHZ                ((uint8_t)0x03U)          /*!< output max speed 50MHz */
#define GPIO_OSPEED_MAX                  ((uint8_t)0x04U)          /*!< GPIO very high output speed, max speed more than 50MHz */

struct XBH_MCU_GPIO_ATTR_S
{
    uint32_t port:4;
    uint32_t pin:4;
    uint32_t mode:8;
    uint32_t speed:3;
    uint32_t level:1;
    uint32_t checksum:8;
};

union XBH_MCU_GPIODEF_U
 {
    XBH_U32 value;
    XBH_MCU_GPIO_ATTR_S attr;
};

struct XBH_CEC_WAKEUP_S {
    uint32_t wk_logic_addr:8;
    uint32_t wk_phy_addr:16;
    uint32_t wk_port_id:8;
};

#ifndef MCU_I2C_BUFF_SIZE
#define MCU_I2C_BUFF_SIZE 10
#endif


/* common defines for gpio interface */
#define XBH_BOARD_GPIO_HIGH                      1
#define XBH_BOARD_GPIO_LOW                       0
#define XBH_BOARD_GPIO_OUTPUT                    1
#define XBH_BOARD_GPIO_INPUT                     0
#define INVALID_GPIO_NUM                         0xffffff

#define BIT(x)                          (1u << (x))

#define BIT_5V_NORMAL                   BIT(4)
#define BIT_12V_NORMAL                  BIT(5)
#define BIT_24V_NORMAL                  BIT(6)
#define BIT_CONN_BL_ON                  BIT(7)
#define BIT_PANEL_VCC                   BIT(8)
#define BIT_SOC_BL_ON                   BIT(9)
#define BIT_3V3_STB                     BIT(10)
#define BIT_5V_STB                      BIT(11)
#define BIT_STB_PWR                     BIT(12)

#define DEV_GPIO_NAME       "/dev/lg_gpio"
#define REQUEST_IO          _IOW('v', 0x00, unsigned int)
#define FREE_IO             _IOW('v', 0x01, unsigned int)
#define SET_IO_OUT          _IOW('v', 0x02, unsigned int)
#define SET_IO_VAL          _IOW('v', 0x03, unsigned int)
#define SET_IO_IN           _IOW('v', 0x04, unsigned int)
#define GET_IO_VAL          _IOR('v', 0x05, unsigned int)
#define GET_IO_DIR          _IOR('v', 0x06, unsigned int)
#define GET_IO_OUT_VAL      _IOR('v', 0x07, unsigned int)

#define WAKEUP_REASON_STICK_REG         0xFE0102DC

#define NTC_ADC_100K_0402_MIN -40
#define NTC_ADC_100K_0402_LEN 141

#define FILEPATH_MAX_LENGTH     128
#define XBH_BURN_HDCP_RX22 "persist.vendor.xbh.burn.hdcprx22"
#define XBH_BURN_HDCP_RX14 "persist.vendor.xbh.burn.hdcprx14"
#define XBH_BURN_HDCP_TX14 "persist.vendor.xbh.burn.hdcptx14"
#define XBH_BURN_HDCPKEY_PATH "vendor.xbh.burn.hdcpkey.path"

const XBH_U32 NTC_ADC_100K_0402[NTC_ADC_100K_0402_LEN] =
{
    4054000, 3781000, 3528000, 3294000, 3077000, 2875000, 2688000, 2514000, 2353000, 2203000,  //-40 ~ -31
    2063000, 1933000, 1812000, 1700000, 1595000, 1497000, 1405000, 1320000, 1241000, 1166000,  //-30 ~ -21
    1097000, 1032000,  971500,  914700,  861700,  812000,  765400,  721800,  680900,  642600,  //-20 ~ -11
     606600,  572900,  541200,  511500,  483500,  457300,  432600,  409400,  387500,  367000,  //-10 ~ -9
     347600,  329400,  312200,  296100,  280800,  266400,  252900,  240100,  228000,  216600,  //0 ~ 9
     205800,  195600,  186000,  176900,  168300,  160100,  152400,  145100,  138200,  131700,  //10 ~ 19
     125500,  119600,  114000,  108800,  103800,   99000,   94400,   90040,   85910,   81980,  //20 ~ 29
      78260,   74720,   71360,   68170,   65140,   62260,   59520,   56920,   54440,   52080,  //30 ~ 39
      43730,   41890,   40130,   38460,   36860,   35340,   33890,   32500,   31180,   29920,  //40 ~ 49
      28720,   27570,   26470,   25420,   24420,   23460,   22550,   21670,   20830,   20030,  //50 ~ 59
      17160,   16520,   15910,   15320,   14750,   14210,   13690,   13190,   12720,   12260,  //60 ~ 69
      11820,   11400,   11000,   10610,   10240,    9880,    9537,    9208,    8891,    8587,  //70 ~ 79
       8295,    8014,    7744,    7485,    7235,    6995,    6764,    6542,    6328,    6122,  //80 ~ 89
       5924,    5733,    5549,    5372,    5202,    4996,    4797,    4605,    4420,    4243,  //90 ~ 99
       4073  //100
};

/* wake up reason*/
#define UDEFINED_WAKEUP                           0
#define CHARGING_WAKEUP                           1
#define REMOTE_WAKEUP                             2
#define RTC_WAKEUP                                3
#define BT_WAKEUP                                 4
#define WIFI_WAKEUP                               5
#define POWER_KEY_WAKEUP                          6
#define AUTO_WAKEUP                               7
#define CEC_WAKEUP                                8
#define REMOTE_CUS_WAKEUP                         9
#define ETH_PMT_WAKEUP                            10
#define CECB_WAKEUP                               11
#define ETH_PHY_GPIO                              12
#define VAD_WAKEUP                                13
#define XBH_EXT_WAKEUP_REASON                     14
#define UART_WAKEUP                               19
#define WAKEUP_REASON_MASK                        0x0F

/*对XBH_EXT_WAKEUP_REASON的补充，amlogic最大支持16个唤醒reason,通过拓展一个数据来达到朗国自己新增的唤醒源*/
#define XBH_EXT_WAKEUP_REASON_SOURCE_VGA           5
#define XBH_EXT_WAKEUP_REASON_SOURCE_HDMI1         9
#define XBH_EXT_WAKEUP_REASON_SOURCE_HDMI2         10
#define XBH_EXT_WAKEUP_REASON_SOURCE_HDMI3         11
#define XBH_EXT_WAKEUP_REASON_SOURCE_F_HDMI        15
#define XBH_EXT_WAKEUP_REASON_SOURCE_DP1           17
#define XBH_EXT_WAKEUP_REASON_SOURCE_TYPEC1        29
#define XBH_EXT_WAKEUP_REASON_SOURCE_TYPEC2        30
#define XBH_EXT_WAKEUP_REASON_SOURCE_DVI           32
#define XBH_EXT_WAKEUP_REASON_MCU                  65
#define XBH_EXT_WAKEUP_REASON_UART                 66
#define XBH_EXT_WAKEUP_REASON_PIR                  67
#define XBH_EXT_WAKEUP_REASON_DVI                  68
#define XBH_EXT_WAKEUP_REASON_NFC                  69


class XbhAmlogic_311d2 : public XbhPlatformInterface, public XbhModuleInterface
{
public:
    XbhAmlogic_311d2();
    virtual ~XbhAmlogic_311d2(){};
    //override
    XBH_S32 setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    //override
    XBH_S32 getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue);
    //override
    XBH_S32 getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue);
    //override
    XBH_S32 setIICLock(XBH_U32 iicBus, XBH_BOOL lock);
    //override
    XBH_S32 setIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    //override
    XBH_S32 getIICData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    //override
    XBH_S32 getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable);
    //override
    XBH_S32 setDebugEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getDebugEnable(XBH_BOOL *bEnable);
    //override
    XBH_S32 setWolMode(XBH_BOOL bEnable);
    //override
    XBH_S32 setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getADCChannelValue(XBH_U32 u32Channel, XBH_U32 *u32Value);
    //override
    XBH_S32 getTemperatureSensorValue(XBH_FLOAT *s32Value);
    //override
    XBH_S32 isNfcExist(XBH_BOOL *enable);
    //override
    XBH_S32 setNfcEnable(XBH_BOOL enable);
    //override
    XBH_S32 getNfcEnable(XBH_BOOL *enable);
    //override
    XBH_S32 setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setTconPower(XBH_BOOL bEnable);
    //override
    XBH_S32 getTconPower(XBH_BOOL *bEnable);
    //override
    XBH_S32 setPoweronMode(XBH_POWERMODE_E enMode);
    //override
    XBH_S32 getPoweronMode(XBH_POWERMODE_E *enMode);
    //override
    XBH_S32 setPictureMode(XBH_PICMODE_E enPicMode);
    //override
    XBH_S32 getPictureMode(XBH_PICMODE_E* enPicMode);
    //override
    XBH_S32 setRtcTime(XBH_RTC_INFO_S *stRtcInfo);
    //override
    XBH_S32 getRtcTime(XBH_RTC_INFO_S *stRtcInfo);
    //override
    XBH_S32 setRtcAlarmTime(XBH_RTC_ALARM_INFO_S *stRtcAlarmInfo);
    //override
    XBH_S32 setWosEnable(XBH_BOOL bEnable);
    //override
    XBH_BOOL getWosEnable();
    //override
    XBH_S32 setBootLogo(const XBH_CHAR* path);
    //override
    XBH_S32 setProjectId(const XBH_CHAR* strProjectId);
    //override
    XBH_S32 getProjectId(XBH_CHAR* strProjectId);
    //override
    XBH_S32 setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 loadEdlaCust(XBH_BOOL *enable);
    //override
    XBH_S32 loadProperty();
    //override
    XBH_S32 setChipRunTime(XBH_S32 time);
    //override
    XBH_S32 getChipRunTime(XBH_S32* time);
    //override
    XBH_S32 setHdmirxEdidType(XBH_S32 type);
    //override
    XBH_S32 getHdmirxEdidType(XBH_S32 *type);
    //override
    XBH_S32 getSocHdmiEdidStatus(XBH_BOOL *enable);
    //override
    XBH_S32 setSn(const XBH_CHAR* strSn);
    //override
    XBH_S32 getSn(XBH_CHAR* strSn);
    //override
    XBH_S32 setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput);
    //override
    XBH_S32 getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput);
    //override
    XBH_S32 setPixelShiftEnable(XBH_BOOL value);
    //override
    XBH_S32 getPixelShiftEnable(XBH_BOOL* value);
    //override
    XBH_S32 setSourceScreenOffset(XBH_S32 x, XBH_S32 y);
    //override
    XBH_S32 getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y);
    //override
    XBH_S32 getGoogleKeyName(XBH_CHAR* strPath);
    //override
    XBH_S32 getGoogleKeyStatus(XBH_BOOL* bEnable);
    //override
    XBH_S32 getAttentionKeyStatus(XBH_BOOL* enable);
    //override
    XBH_S32 setHdcpKeyName(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName);
    //override
    XBH_S32 setHdcpKey(const XBH_CHAR* strPath, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 setNR(XBH_LEVEL_E enNrLevel);
    //override
    XBH_S32 getNR(XBH_LEVEL_E *enNrLevel);
    //override
    XBH_S32 setMultiUser(const XBH_CHAR* status);
    //override
    XBH_S32 getMultiUser(XBH_CHAR* status);
    //override
    XBH_S32 setVSPage(const XBH_CHAR* status);
    //override
    XBH_S32 getVSPage(XBH_CHAR* status);
    //override
    XBH_S32 setCustomSKU(const XBH_CHAR* status);
    //override
    XBH_S32 getCustomSKU(XBH_CHAR* status);
    //override
    XBH_S32 setBootMode(const XBH_CHAR* status);
    //override
    XBH_S32 getBootMode(XBH_CHAR* status);
    //override
    XBH_S32 getMicLicenseState(XBH_S32* status);
    //override
    XBH_S32 getHostOrDeviceStatus(XBH_S32* type);
    //override
    XBH_S32 setHostOrDeviceStatus(XBH_S32 type);
    //override
    XBH_S32 setContrast(XBH_U32 u32Value);
    //override
    XBH_S32 getContrast(XBH_U32 *u32Value);
    //override
    XBH_S32 setBrightness(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightness(XBH_U32 *u32Value);
    //override
    XBH_S32 setHue(XBH_U32 u32Value);
    //override
    XBH_S32 getHue(XBH_U32 *u32Value);
    //override
    XBH_S32 setSharpness(XBH_U32 u32Value);
    //override
    XBH_S32 getSharpness(XBH_U32 *u32Value);
    //override
    XBH_S32 setSaturation(XBH_U32 u32Value);
    //override
    XBH_S32 getSaturation(XBH_U32 *u32Value);
    //override
    XBH_S32 getHallSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getArcDetectStatus(XBH_BOOL* status);
    //override
    XBH_S32 ProcessTypeBHotplug(XBH_SOURCE_E src);
    //override
    XBH_S32 ProcessTypeCHotplug(XBH_SOURCE_E src);
    //override
    XBH_S32 setWatchDogManualModeEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getWatchDogManualModeEnable(XBH_BOOL *bEnable);
    //override
    XBH_S32 getEthPlugStatus(XBH_BOOL* bEnable);
    //override
    XBH_S32 setMcuFattMode(XBH_S32 mode);
    //override
    XBH_S32 getMcuFattMode(XBH_S32 *mode);
    //override
    XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 readAndUpdateEdidBinFileByEdidType(int dev, const char* edidBinFilePath, int port);

private:
    XBH_S32 open_i2c_dev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet);
    XBH_S32 check_iic_funcs(XBH_S32 file);
    XBH_S32 set_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    XBH_S32 get_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    XBH_S32 setGpioOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    XBH_S32 setGpioInput(XBH_U32 u32GpioNumber);
    XBH_S32 getGpioDir(XBH_U32 u32GpioNumber);
    XBH_S32 initSerialPort();
    XBH_S32 wall_clock_rtc();
    XBH_BOOL rtc_is_hctosys(XBH_U32 rtc_id);
    XBH_U32 getMcuGpioChecksum(XBH_MCU_GPIO_ATTR_S *p_attr);
    XBH_S32 getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level);
    XBH_S32 setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level);
    XBH_BOOL is_valid_mac_addr(const XBH_CHAR* macAddr);
    XBH_S32 handleLicenseFileMissing();
private:
    XBH_S32 mGpioFd = -1;
    XBH_S32 mRtcId = -1;
    struct termios uart_debug_config;
    static XbhMutex mLock;
    XBH_AUDIO_OUTPUT_E mAudioOutput;
};

#endif //XBH_AMLOGIC_311D2_H
