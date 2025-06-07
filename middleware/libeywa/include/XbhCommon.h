#ifndef XBH_COMMON_H
#define XBH_COMMON_H

#include "XbhType.h"

#ifndef MCU_I2C_BUFF_SIZE
#define MCU_I2C_BUFF_SIZE                   (30)
#endif

//图像模式
enum XBH_PICMODE_E
{
    XBH_PICMODE_STANDARD,
    XBH_PICMODE_DYNAMIC,
    XBH_PICMODE_VIVID,
    XBH_PICMODE_SOFTNESS,
    XBH_PICTURE_NATURAL,
    XBH_PICTURE_SPORTS,
    XBH_PICMODE_USER,
    XBH_PICMODE_GAME,

    XBH_PICMODE_BUTT
};

//色温模式
enum XBH_COLORTEMP_E
{
    XBH_COLORTEMP_STANDARD,
    XBH_COLORTEMP_WARM,
    XBH_COLORTEMP_COLD,
    XBH_COLORTEMP_USER,

    #ifdef XBH_USE_COLORTEMP_EXT
    XBH_COLORTEMP_NATIVE,
    XBH_COLORTEMP_3000K,
    XBH_COLORTEMP_4000K,
    XBH_COLORTEMP_5000K,
    XBH_COLORTEMP_6500K,
    XBH_COLORTEMP_7500K,
    XBH_COLORTEMP_9300K,
    XBH_COLORTEMP_10000K,
    XBH_COLORTEMP_11000K,
    XBH_COLORTEMP_13000K,
    #endif

    XBH_COLORTEMP_BUTT
};

//挡位等级
enum XBH_LEVEL_E
{
    XBH_LEVEL_OFF,
    XBH_LEVEL_LOW,
    XBH_LEVEL_MID,
    XBH_LEVEL_HIGH,
    XBH_LEVEL_AUTO,

    XBH_LEVEL_BUTT
};

//PC模式
enum XBH_PCMODE_E
{
    XBH_PCMODE_PC,
    XBH_PCMODE_VIDEO,
    XBH_PCMODE_AUTO,

    XBH_PCMODE_BUTT
};

//比例模式
enum XBH_ASPECT_E
{
    XBH_ASPECT_16_9,
    XBH_ASPECT_4_3,
    XBH_ASPECT_POINT_TO_POINT,

    XBH_ASPECT_AUTO,
    XBH_ASPECT_BUTT
};

//HDMI Color Range
enum XBH_COLOR_RANGE_E
{
    XBH_COLOR_RANGE_FULL,
    XBH_COLOR_RANGE_LIMIT,
    XBH_COLOR_RANGE_AUTO,

    XBH_COLOR_RANGE_BUTT
};

//声音模式
enum XBH_SOUND_MODE_E
{
    XBH_SOUND_MODE_STANDARD,
    XBH_SOUND_MODE_CLASSROOM,
    XBH_SOUND_MODE_MEETING,
    XBH_SOUND_MODE_MOVIE,
    XBH_SOUND_MODE_SPORT,
    XBH_SOUND_MODE_MUSIC,
    XBH_SOUND_MODE_NEWS,
    XBH_SOUND_MODE_USER,

    XBH_SOUND_MODE_BUTT
};

//声音输出通路
enum XBH_AUDIO_CHANNEL_E
{
    XBH_AUDIO_CHANNEL_SPEAKER,      //喇叭(立体声+低音炮)         输出
    XBH_AUDIO_CHANNEL_HEADPHONE,    //耳机/line out         输出
    XBH_AUDIO_CHANNEL_BLUETOOTH,    //蓝牙
    XBH_AUDIO_CHANNEL_ARC,          //HDMI ARC            输出
    XBH_AUDIO_CHANNEL_SPDIF,        //同轴/光纤               输出
    XBH_AUDIO_CHANNEL_AVOUT,        //audio out           输出
    XBH_AUDIO_CHANNEL_24G,          //2.4G
    XBH_AUDIO_CHANNEL_I2SOUT,       //第二路I2S
    XBH_AUDIO_CHANNEL_HDMITX,       //hdmi tx             输出
    XBH_AUDIO_CHANNEL_SUBWOOFER,    //低音炮                 输出
    XBH_AUDIO_CHANNEL_MIC,          //mic                 输入
    XBH_AUDIO_CHANNEL_STEREO,       //立体声喇叭               输出

    XBH_AUDIO_CHANNEL_BUTT
};

//声音输出策略
enum XBH_AUDIO_OUTPUT_E
{
    XBH_AUDIO_OUTPUT_DET,       //根据检测到接入的设备输出到设备上
    XBH_AUDIO_OUTPUT_BOTH,      //speaker和接入设备同时输出
    XBH_AUDIO_OUTPUT_EXTERNAL,  //只在接入设备上输出
    XBH_AUDIO_OUTPUT_INTERNAL,  //只在speaker上输出
    XBH_AUDIO_OUTPUT_USB,  //只在外接USB上输出

    XBH_AUDIO_OUTPUT_BUTT
};

//spdif的输出信号格式
enum XBH_SPDIF_MODE_E
{
    XBH_SPDIF_MODE_PCM,
    XBH_SPDIF_MODE_RAW,
    XBH_SPDIF_MODE_AUTO,

    XBH_SPDIF_MODE_BUTT
};

//ARC的输出信号格式
enum XBH_ARC_MODE_E
{
    XBH_ARC_MODE_LPCM,
    XBH_ARC_MODE_RAW,
    XBH_ARC_MODE_AUTO,

    XBH_ARC_MODE_BUTT
};

/**
 * 7段EQ值，通常使用前面的5端，对应的EQ为：
 * 0: EQ0 120 HZ
 * 1: EQ1 500 HZ
 * 2: EQ2 1.5k HZ
 * 3: EQ3 5K HZ
 * 4: EQ4 10K HZ
 * 5: EQ5 12K HZ
 * 6: EQ6 15K HZ
*/
enum XBH_EQ_MODE_E
{
    XBH_EQ_MODE_0,
    XBH_EQ_MODE_1,
    XBH_EQ_MODE_2,
    XBH_EQ_MODE_3,
    XBH_EQ_MODE_4,
    XBH_EQ_MODE_5,
    XBH_EQ_MODE_6,

    XBH_EQ_MODE_BUTT
};

//line out输出模式。用line out接耳机和外接功放时，输出幅度不一样
enum XBH_LINEOUT_MODE_E
{
    XBH_LINEOUT_MODE_EARPHONE,
    XBH_LINEOUT_MODE_LINEOUT,

    XBH_LINEOUT_MODE_BUTT
};

//信源序号，跟APP层的信源序号一致
enum XBH_SOURCE_E
{
    XBH_SOURCE_ATV      = 0,
    XBH_SOURCE_DVBC     = 1,
    XBH_SOURCE_DTMB     = 2,
    XBH_SOURCE_CVBS1    = 3,
    XBH_SOURCE_CVBS2    = 4,
    XBH_SOURCE_VGA1     = 5,
    XBH_SOURCE_VGA2     = 6,
    XBH_SOURCE_YPBPR1   = 7,
    XBH_SOURCE_YPBPR2   = 8,
    XBH_SOURCE_HDMI1    = 9,    //板载HDMI
    XBH_SOURCE_HDMI2    = 10,
    XBH_SOURCE_HDMI3    = 11,
    XBH_SOURCE_HDMI4    = 12,
    XBH_SOURCE_HDMI5    = 13,
    XBH_SOURCE_HDMI6    = 14,
    XBH_SOURCE_F_HDMI1  = 15,   //前置HDMI
    XBH_SOURCE_F_HDMI2  = 16,
    XBH_SOURCE_F_HDMI3  = 17,
    XBH_SOURCE_F_HDMI4  = 18,
    XBH_SOURCE_USBC1    = 19,   //板载USBC
    XBH_SOURCE_USBC2    = 20,
    XBH_SOURCE_USBC3    = 21,
    XBH_SOURCE_USBC4    = 22,
    XBH_SOURCE_F_USBC1  = 23,   //前置USBC
    XBH_SOURCE_F_USBC2  = 24,
    XBH_SOURCE_F_USBC3  = 25,
    XBH_SOURCE_F_USBC4  = 26,
    XBH_SOURCE_DP1      = 27,   //板载DP
    XBH_SOURCE_DP2      = 28,
    XBH_SOURCE_DP3      = 29,
    XBH_SOURCE_DP4      = 30,
    XBH_SOURCE_F_DP1    = 31,   //前置DP
    XBH_SOURCE_F_DP2    = 32,
    XBH_SOURCE_F_DP3    = 33,
    XBH_SOURCE_F_DP4    = 34,
    XBH_SOURCE_OPS1     = 35,   //OPS
    XBH_SOURCE_OPS2     = 36,
    XBH_SOURCE_SDM1     = 37,   //SDM
    XBH_SOURCE_SDM2     = 38,
    XBH_SOURCE_ANDROID  = 39,   //Android
    XBH_SOURCE_OUT1     = 40,   //信号输出(tv芯片通常用作VBO+HDMI OUT)，通常使用了转接芯片，升级时需要控制
    XBH_SOURCE_OUT2     = 41,   //信号输出，通常使用了转接芯片，升级时需要控制
    XBH_SOURCE_OUT3     = 42,   //信号输出，通常使用了转接芯片，升级时需要控制
    XBH_SOURCE_OUT4     = 43,   //信号输出，通常使用了转接芯片，升级时需要控制
    XBH_SOURCE_SCART,
    XBH_SOURCE_DVBT,
    XBH_SOURCE_ATSC,
    XBH_SOURCE_DVBS,
    XBH_SOURCE_ISDBT,
    XBH_SOURCE_BUTT = 0xFF
};

//信源窗口位置及大小
struct XBH_RECT_S
{
    XBH_U32 u32X;
    XBH_U32 u32Y;
    XBH_U32 u32Width;
    XBH_U32 u32Height;
};

//gpio 控制二元组
struct GPIO_OPT_TABLE
{
    XBH_S32 cmd;
    XBH_S32 param_list[4];
};

#define GPIO_OPT_TABLE_SIZE(x) sizeof(x)/sizeof(GPIO_OPT_TABLE)

/**
* 通用操作接口，主要针对IIC，UART设备，并且控制上下电和复位
* 部分需要自动适配的IIC sensor可能会使用另外的一路IIC，比如光感模块，
*/
struct COMMON_OPT_TABLE
{
    XBH_S32 iicBus1;        //device iic bus
    XBH_S32 iicBus2;        //device iic bus，没有就填-1
    XBH_S32 iicAddr;        //device iic address, 自动识别的器件可以填-1，比如光感sensor
    XBH_S32 uart;           //如果使用串口通信，填使用的soc的uart的id，不使用时填-1
    XBH_S32 pGpio;          //供电引脚，没有就填-1
    XBH_S32 pLevel;         //工作时的电平状态
    XBH_S32 rGpio;          //复位引脚，没有就填-1
    XBH_S32 rLevel;         //复位工作时的电平状态
};

//HDMI SWTICH 设备操作
struct HDMI_SW_OPT_TABLE
{
    XBH_S32 iicBus;         //device iic bus
    XBH_S32 iicAddr;        //device iic address
    XBH_S32 sGpio;          //1切2的HDMI switch使用的切换引脚，没有就填-1
    XBH_S32 sLevel;         //1切2的HDMI switch 切到port0时的电平
    XBH_S32 pGpio;          //供电引脚，没有就填-1
    XBH_S32 pLevel;         //工作时的电平状态
    XBH_S32 rGpio;          //复位引脚，没有就填-1
    XBH_S32 rLevel;         //复位工作时的电平状态
    XBH_S32 defaultPort;    //默认端口
    XBH_S32 prePort;        //前一级hdmiswitch的序号id和对应的port,
                            //为了兼容以前的版本，switch id和 port都是从1开始
};

//HDMI switch port
struct HDMI_SWITCH_PORT
{
    XBH_U8 port0;   //port0  0x00：悬空，0x01：连接下一级switch
    XBH_U8 port1;   //port1  0x00：悬空，0x01：连接下一级switch
    XBH_U8 port2;   //port2  0x00：悬空，0x01：连接下一级switch
    XBH_U8 port3;   //port3  0x00：悬空，0x01：连接下一级switch
};

//CODEC设备操作
struct CODEC_OPT_TABLE
{
    XBH_S32 iicBus;           //device iic bus，不使用时填-1
    XBH_S32 iicAddr;          //device iic address
    XBH_S32 uart;             //如果使用串口通信，填使用的soc的uart的id，不使用时填-1
    //复位或者上电控制引脚
    XBH_S32 pGpio;             //供电引脚，没有就填-1
    XBH_S32 pLevel;            //工作时的电平状态
    XBH_S32 rGpio;             //复位引脚，没有就填-1
    XBH_S32 rLevel;            //复位工作时的电平状态
    //模拟mic的插入检测引脚，一般用来处理插入MIC时的噪声,没有就填-1
    XBH_S32 mic_plug_gpio;    //mic plug in pin
    XBH_S32 mic_plug_level;   //mic plug in level
    //使用codec做hp输出时使用,没有就填-1
    XBH_S32 hp_plug_gpio;     //headphone plug in pin
    XBH_S32 hp_plug_level;    //headphone plug in level
    XBH_S32 hp_mute_gpio;     //headphone mute pin
    XBH_S32 hp_mute_level;    //headphone mute level
};

//功放设备操作
struct AMP_OPT_TABLE
{
    XBH_S32 iicBus;           //device iic bus
    XBH_S32 iicAddr;          //device iic address
    //复位或者上电控制引脚
    XBH_S32 pGpio;            //供电引脚，没有就填-1
    XBH_S32 pLevel;           //工作时的电平状态
    XBH_S32 rGpio;            //复位引脚，没有就填-1
    XBH_S32 rLevel;           //复位工作时的电平状态
    //功放的MUTE引脚
    XBH_S32 mute_gpio;        //功放的MUTE引脚
    XBH_S32 mute_level;       //功放MUTE时的电平
};

//网络交换机芯片设备操作
struct NET_SWITCH_OPT_TABLE
{
    XBH_S32 scl;            //mido clock pin
    XBH_S32 sda;            //mido data pin
    XBH_S32 pGpio;          //供电引脚，没有就填-1
    XBH_S32 pLevel;         //工作时的电平状态
    XBH_S32 rGpio;          //复位引脚，没有就填-1
    XBH_S32 rLevel;         //复位工作时的电平状态
    XBH_S32 androidPort;    //android port id
    XBH_S32 typecPort;      //typec port id
    XBH_S32 opsPort;        //ops port id
    XBH_S32 rj45inPort;     //rj45 in port id
    XBH_S32 rj45outPort;    //rj45 out port id
};

//HDMI 信号格式
enum XBH_HDMI_FORMAT_E
{
    XBH_HDMI_FORMAT_HDMI,
    XBH_HDMI_FORMAT_DVI,

    XBH_HDMI_FORMAT_BUTT
};

//信源timing的基本信息
struct XBH_TIMING_INFO_S
{
    XBH_U32 u32Width;
    XBH_U32 u32Height;
    XBH_U32 u32FrameRate;
    XBH_BOOL bInterlace;
    XBH_HDMI_FORMAT_E enFormatHDMI;
};

//信号的显示状态
enum XBH_SIGNAL_STATUS_E
{
    XBH_SIGNAL_STATUS_SUPPORT,
    XBH_SIGNAL_STATUS_NOSIGNAL,
    XBH_SIGNAL_STATUS_UNSUPPORT,
    XBH_SIGNAL_STATUS_UNSTABLE,

    XBH_SIGNAL_STATUS_BUTT
};

struct XBH_MCU_I2CBUFFDEF_S
{
    XBH_U8 len;
    XBH_U8 cmd;
    XBH_U8 data[MCU_I2C_BUFF_SIZE];
};

enum XBH_MCU_I2CCMDDEF_E
{
     //Tcon DATA
    CMD_I2C_GET_TCON              = 0x10,

    // Notice
    CMD_I2C_GET_ADC_KEY0          = 0X20,
    CMD_I2C_GET_IR_VALUE          = 0X21,
    CMD_I2C_GET_EXIT_ADC          = 0x22,
    CMD_I2C_GET_ADC_KEY1          = 0X23,
    CMD_I2C_GET_GPIO_KEY0         = 0X24,
    CMD_I2C_GET_GPIO_KEY1         = 0X25,
    CMD_I2C_GET_HALL              = 0X26,
    CMD_I2C_GET_HALL1             = 0X27,
    CMD_I2C_GET_PIR               = 0X28,
    CMD_I2C_GET_PIR1              = 0X29,
    CMD_I2C_GET_NOTICE            = 0x2F,

    CMD_I2C_GET_POWERON_MODE          = 0X30,
    CMD_I2C_SET_WAKEUP_WOL            = 0X31,
    CMD_I2C_SET_WAKEUP_HDMI           = 0X32,
    CMD_I2C_GET_WAKEUP_HDMI           = 0X33,
    CMD_I2C_SET_POWERON_MODE          = 0X34,
    CMD_I2C_GET_WAKEUP_WOL            = 0X35,
    CMD_I2C_SET_POWER_KEYPAD_ADCVALUE = 0X36,
    CMD_I2C_GET_POWER_KEYPAD_ADCVALUE = 0X37,
    CMD_I2C_ADD_IR_KEYCODE            = 0X38,
    CMD_I2C_DEL_IR_KEYCODE            = 0X39,
    CMD_I2C_DEL_ALL_IR_KEYCODE        = 0X3A,
    CMD_I2C_GET_ALL_IR_KEYCODE        = 0X3B,
    CMD_I2C_SET_WAKEUP_CEC            = 0X3C,
    CMD_I2C_GET_WAKEUP_CEC            = 0X3D,
    CMD_I2C_GET_WAKEUP_INFO           = 0x3E,
    CMD_I2C_SET_WAKEUP_USBC           = 0x3F,
    CMD_I2C_GET_WAKEUP_USBC           = 0x40,
    CMD_I2C_SET_WAKEUP_F_USBC         = 0x41,
    CMD_I2C_GET_WAKEUP_F_USBC         = 0x42,
    CMD_I2C_SET_WAKEUP_F_HDMI         = 0x43,
    CMD_I2C_GET_WAKEUP_F_HDMI         = 0x44,
    CMD_I2C_SET_WAKEUP_NFC            = 0x45,
    CMD_I2C_GET_WAKEUP_NFC            = 0x46,
    CMD_I2C_SET_WAKEUP_PIR            = 0x47,
    CMD_I2C_GET_WAKEUP_PIR            = 0x48,

    CMD_I2C_SET_MAC_ADDR              = 0x90,

    CMD_I2C_GET_APP_VER           = 0xA0,
    CMD_I2C_SET_GPIO_OUTPUT       = 0xA1,
    CMD_I2C_GET_GPIO_OUTPUT       = 0xA2,
    CMD_I2C_GET_GPIO_INPUT        = 0xA3,
    CMD_I2C_SET_PWM               = 0xA4,
    CMD_I2C_SET_USB_MODE          = 0xA5,
    CMD_I2C_POWER_ON_STANDBY      = 0xAB,
    CMD_I2C_SAVE_GPIO_PIN         = 0xAE,

    /* SOC POWERMODE */
    CMD_I2C_SOC_POWER_MODE        = 0xB0,

    /* SENSOR */
    CMD_I2C_GET_PM2_5             = 0xD0,
    CMD_I2C_GET_AM1008            = 0xD1,
    CMD_I2C_RTC_GET_TIME          = 0XD2,
    CMD_I2C_RTC_SET_TIME          = 0XD3,
    CMD_I2C_RTC_SET_ALAMETIME     = 0XD4,

    /* FATT */
    CMD_I2C_SET_FATT_MODE         = 0xE0,
    CMD_I2C_GET_FATT_MODE         = 0xE1,
    CMD_I2C_SET_BYPASS_IIC_PARA   = 0XE2,
    CMD_I2C_SET_BYPASS_IIC        = 0XE3,
    CMD_I2C_GET_BYPASS_IIC        = 0XE4,
    CMD_I2C_SET_BYPASS_UART_PARA  = 0XE5,
    CMD_I2C_SET_BYPASS_UART       = 0XE6,
    CMD_I2C_GET_BYPASS_UART       = 0XE7,

    /* I2C IAP */
    CMD_I2C_ENTER_IAP             = 0xF0,
    CMD_I2C_READ_IAP_VER          = 0xF1,
    CMD_I2C_WRITE_IAP_APP_LENGTH  = 0xF2,
    CMD_I2C_WRITE_IAP_DATA        = 0xF3,
    CMD_I2C_READ_IAP_CRC32        = 0xF4,
    CMD_I2C_WRITE_IAP_VALID       = 0xF5,

    CMD_I2C_MCU_FIRMWARE_VER      = 0xF6,
    CMD_I2C_CHIP_FIRMWARE_VER     = 0xF7,
    CMD_I2C_GET_TEST_MODE         = 0xF8,
    CMD_I2C_SET_TEST_MODE         = 0xF9,
};
enum XBH_MCU_NOTICE_E
{
    CMD_NOTICE_IR              = 0x00,
    CMD_NOTICE_KEY             = 0x01,
    CMD_NOTICE_HDMI1           = 0x02,
    CMD_NOTICE_HDMI2           = 0x03,
    CMD_NOTICE_F_HDMI          = 0x04,
    CMD_NOTICE_VGA             = 0x05,
    CMD_NOTICE_DP              = 0x06,
    CMD_NOTICE_F_TYPE_C        = 0x07,
    CMD_NOTICE_OPS_DET         = 0x08,
    CMD_NOTICE_OPS_POWER       = 0x09,
    CMD_NOTICE_HALL            = 0x0A,
    CMD_NOTICE_HALL2           = 0x0B,
    CMD_NOTICE_PIR             = 0x0C,
    CMD_NOTICE_TYPE_C          = 0x0D,
    CMD_NOTICE_AHA_NFC         = 0x0E,
    CMD_NOTICE_F_HDMI2         = 0x0F,
    CMD_NOTICE_F_HDMI3         = 0x10,
    CMD_NOTICE_WOL             = 0x11,
    CMD_NOTICE_NONE            = 0xFF,
};

enum XBH_UPGRADE_STATE_E {
    XBH_UPGRADE_IDLE        = 0,
    XBH_UPGRADE_RUNNING     = 1,
    XBH_UPGRADE_SUCCESS     = 2,
    XBH_UPGRADE_FAILURE     = 3,
};

//EDID数据类型
enum XBH_HDMIRX_EDID_TYPE_E
{
    XBH_HDMIRX_EDID_TYPE_14,
    XBH_HDMIRX_EDID_TYPE_20,
    XBH_HDMIRX_EDID_TYPE_21,

    XBH_HDMIRX_EDID_TYPE_AUTO,
    XBH_HDMIRX_EDID_TYPE_BUTT
};

//唤醒源
enum XBH_WAKEUP_E
{
    XBH_WAKEUP_IR,
    XBH_WAKEUP_KEYPAD,
    XBH_WAKEUP_RTC,
    XBH_WAKEUP_ETH,
    XBH_WAKEUP_USB,
    XBH_WAKEUP_GPIO,
    XBH_WAKEUP_ADC,
    XBH_WAKEUP_CEC,
    XBH_WAKEUP_UART,
    XBH_WAKEUP_SOURCE,
    XBH_WAKEUP_WIFI,
    XBH_WAKEUP_PIR,
    XBH_WAKEUP_PIR1,
    XBH_WAKEUP_BTYPEC,
    XBH_WAKEUP_FTYPEC,
    XBH_WAKEUP_NFC,
    XBH_WAKEUP_DP,
    XBH_WAKEUP_BHDMI,
    XBH_WAKEUP_FHDMI,
    XBH_WAKEUP_BUTT,
};

//唤醒源的数据结构体
struct XBH_WAKEUP_S
{
    XBH_WAKEUP_E enWakeup;  //唤醒源类型
    XBH_SOURCE_E enSrc;     //信号源唤醒的信源序号
    XBH_U32 u32Value;       //其他源唤醒的标志位
};

//gain offset数据结构体
struct XBH_GAIN_OFFSET_DATA_S
{
    XBH_U32 u32RedGain;
    XBH_U32 u32GreenGain;
    XBH_U32 u32BlueGain;
    XBH_U32 u32RedOffset;
    XBH_U32 u32GreenOffset;
    XBH_U32 u32BlueOffset;
};

//图像参数的非线性类型的定义
enum XBH_NLA_E
{
    XBH_NLA_BRIGHTNESS,
    XBH_NLA_CONTRAST,
    XBH_NLA_SATURATION,
    XBH_NLA_SHARPNESS,
    XBH_NLA_HUE,
    XBH_NLA_BACKLIGHT,

    XBH_NLA_BUTT
};

//5段非线性值的定义
struct XBH_NLA_INFO_S
{
    XBH_U32 u32Point0;
    XBH_U32 u32Point25;
    XBH_U32 u32Point50;
    XBH_U32 u32Point75;
    XBH_U32 u32Point100;
};

//SOC发出的test pattern类型定义
enum XBH_PATTERN_E
{
    XBH_PATTERN_OFF,
    XBH_PATTERN_BLACK,
    XBH_PATTERN_WHITE,
    XBH_PATTERN_RED,
    XBH_PATTERN_GREEN,
    XBH_PATTERN_BLUE,
    XBH_PATTERN_GRAY,

    XBH_PATTERN_BUTT
};

//设备上电模式
enum XBH_POWERMODE_E
{
    XBH_POWERMODE_ON,
    XBH_POWERMODE_STANDBY,
    XBH_POWERMODE_MEMORY,

    XBH_POWERMODE_BUTT
};

//RTC数据结构体
struct XBH_RTC_INFO_S
{
    XBH_U32 u32RtcYear;//公元yyyy
    XBH_U32 u32RtcMonth;//1-12
    XBH_U32 u32RtcDate;//1-31
    XBH_U32 u32RtcWeek;
    XBH_U32 u32RtcHour;
    XBH_U32 u32RtcMinute;
    XBH_U32 u32RtcSec;
};

//RTC唤醒数据结构体
struct XBH_RTC_ALARM_INFO_S
{
    XBH_U32 u32RtcDate;
    XBH_U32 u32RtcWeek;
    XBH_U32 u32RtcHour;
    XBH_U32 u32RtcMinute;
    XBH_U32 u32RtcSec;
    XBH_BOOL bEnable;
};

//ADC采集通道定义
enum XBH_ADC_CHANNEL_NUM_E
{
    XBH_ADC_CHANNEL_NUM_0,
    XBH_ADC_CHANNEL_NUM_1,
    XBH_ADC_CHANNEL_NUM_2,
    XBH_ADC_CHANNEL_NUM_3,
    XBH_ADC_CHANNEL_NUM_4,
    XBH_ADC_CHANNEL_NUM_5,
    XBH_ADC_CHANNEL_NUM_6,
    XBH_ADC_CHANNEL_NUM_7,
    XBH_ADC_CHANNEL_NUM_BUTT
};

//触摸框类型定义
enum XBH_TP_IF_E
{
    XBH_TP_IF_AUTO_CTRL,        //根据触摸框的VID PID选择合适的协议
    XBH_TP_IF_UART_CTRL,        //触摸框支持串口控制
    XBH_TP_IF_MCU_CTRL,         //控制MCU来开关信源的触摸
    XBH_TP_IF_SDK_CTRL,         //触摸框支持SDK包
    XBH_TP_IF_USB_CTRL,         //触摸框支持USB协议
    XBH_TP_IF_BUTT,
};

//HDMI TX
enum XBH_HDMI_TX_INFO_E
{
    XBH_HDMI_TX_VERSION,            //HDMI TX芯片的固件版本(可选支持)
    XBH_HDMI_TX_TIMING,             //HDMI TX输出分辨率 480P 1080P 4K 分辨率必须支持，其他的可选
    XBH_HDMI_TX_HDCP_ONOFF,         //HDMI TX HDCP开关
    XBH_HDMI_TX_VIDEO_MUTE,         //HDMI TX VIDEO 开关(可选支持)
    XBH_HDMI_TX_AUDIO_MUTE,         //HDMI TX AUDIO 开关(可选支持)
    XBH_HDMI_TX_CHIP_ACTIVE,        //HDMI TX 芯片是否在线
    XBH_HDMI_TX_PLUG_STATE,        //HDMI TX 是否接入设备

    XBH_HDMI_TX_INFO_BUTT
};

//息屏亮屏控制
enum XBH_BACKLIGHT_CONTROL
{
    XBH_MID_SCREEN_OFF,             // 息屏时，中间件调用
    XBH_MID_SCREEN_ON,              // 亮屏时，中间件调用
    XBH_KERNEL_SCREEN_OFF,          // 息屏时，kernel传参到eywa
    XBH_KERNEL_SCREEN_ON,           // 亮屏时，kernel传参到eywa

    XBH_SCREEN_CONTORL
};

//LED指示灯,前置板使用两个IO来控制LED灯,实际颜色要看灯的实物
enum XBH_LED_LIGHT_E
{
    XBH_LED_LIGHT_OFF,            //LED指示灯熄灭
    XBH_LED_LIGHT_RED,            //LED指示灯红色
    XBH_LED_LIGHT_GREEN,          //LED指示灯绿色
    XBH_LED_LIGHT_YELLOW,         //LED指示灯黄色，两个IO同时控制出现混合色
    XBH_LED_LIGHT_WHITE,          //LED指示灯白色

    XBH_LED_LIGHT_BUTT
};

//panel的序号
enum XBH_PANEL_NUM_E
{
    XBH_PANEL_MAIN,            //主屏
    XBH_PANEL_SUB_1,           //副屏1
    XBH_PANEL_SUB_2,           //副屏2
    XBH_PANEL_SUB_3,           //副屏3
    XBH_PANEL_SUB_4,           //副屏4

    XBH_PANEL_BUTT
};

//信源窗口的序号
enum XBH_WINDOW_NUM_E
{
    XBH_WINDOW_MAIN,            //窗口
    XBH_WINDOW_SUB1,            //副窗口1
    XBH_WINDOW_SUB2,            //副窗口2
    XBH_WINDOW_SUB3,            //副窗口3

    XBH_WINDOW_BUTT
};

//HDCP 类型
enum XBH_HDCP_TYPE_E
{
    XBH_HDCP1_4,            //HDCP 1.4
    XBH_HDCP2_2,            //HDCP 2.2
    XBH_HDCP2_3,            //HDCP 2.3
    XBH_HDCP_TX_1_4,        //HDCP TX 1.4
    XBH_HDCP_TX_2_2,        //HDCP TX 2.2
    
    XBH_HDCP_BUTT
};

//HDCPTX 加密模式
enum XBH_HDCPTX_ENCRYPTION_MODE_E
{
    XBH_HDCPTX_ENCRYPTION_1P4,            //HDCP KEY 1.4
    XBH_HDCPTX_ENCRYPTION_2P2,            //HDCP KEY 2.2

    XBH_HDCPTX_ENCRYPTION_BUTT
};

//SSC数据结构体
struct XBH_SSC_INFO_S
{
    XBH_BOOL bEnable;
    XBH_U32 u32Freq;
    XBH_U32 u32Ratio;
};

/**
* 充电功率
*/
enum XBH_PDCAPACITY_POWER
{
    XBH_PDCAPACITY_15W           ,//= 0;
    XBH_PDCAPACITY_45W           ,//= 1;
    XBH_PDCAPACITY_65W           ,//= 2;
    XBH_PDCAPACITY_100W          ,//= 3;
    XBH_PDCAPACITY_0W            ,//= 4;
    XBH_PDCAPACITY_4_5W          ,//= 5;
    XBH_PDCAPACITY_75W           ,//= 6;
    XBH_PDCAPACITY_85W           ,//= 7;
    XBH_PDCAPACITY_90W           ,//= 8;
    XBH_PDCAPACITY_96W           ,//= 9;
};

/**
*  wifi 模组类型
*/
enum XBH_WIFI_TYPE
{
    XBH_RTL8852BE              ,//= 0;
};

/**
*  MAC 地址类型
*/
enum XBH_MAC_ADDRESS_TYPE
{
    XBH_MAC = 0           ,
    XBH_TYPEC_1 = 1       ,
    XBH_TYPEC_2 = 2       ,
    XBH_OPS_1 = 11        ,
    XBH_OPS_2 = 12        ,
};

/**
*  FATT调用烧录的SN/PN接口:物料类型
*/
enum XBH_MATERAL_TYPE
{
    XBH_MAIN_SN = 0         , // 底板(主板)SN
    XBH_CORE_SN = 1         , // 核心板SN
    XBH_AUXILIARY_SN_1 = 2  , // 辅板1 SN
    XBH_AUXILIARY_SN_2 = 3  , // 辅板2 SN
    XBH_AUXILIARY_SN_3 = 4  , // 辅板3 SN
};

/**
* hdmi switch port口定义
*/
enum XBH_HDMI_RXPORT
{
    XBH_HDMI_RXA             ,//= 0;
    XBH_HDMI_RXB             ,//= 1;
    XBH_HDMI_RXC             ,//= 2;
    XBH_HDMI_RXD             ,//= 3;
};

//hdmi cec 地址定义
#define XBH_HDMI1_CEC_PHYSICAL_ADDR     0x1000
#define XBH_HDMI2_CEC_PHYSICAL_ADDR     0x2000
#define XBH_HDMI3_CEC_PHYSICAL_ADDR     0x3000
#define XBH_HDMI4_CEC_PHYSICAL_ADDR     0x4000
#define XBH_HDMI5_CEC_PHYSICAL_ADDR     0x5000
#define XBH_HDMI6_CEC_PHYSICAL_ADDR     0x6000

/**
*动态分配充电功率
# 0:  F-USBC 0W USBC 0W
# 1:  F-USBC 0W USBC 15W
# 2:  F-USBC 0W USBC 65W
# 3:  F-USBC 0W USBC 100W
# 4:  F-USBC 15W USBC 0W
# 5:  F-USBC 15W USBC 15W
# 6:  F-USBC 15W USBC 65W
# 7:  F-USBC 15W USBC 100W
# 8:  F-USBC 65W USBC 0W
# 9:  F-USBC 65W USBC 15W
# 10:  F-USBC 65W USBC 65W
# 11:  F-USBC 65W USBC 100W
# 12:  F-USBC 100W USBC 0W
# 13:  F-USBC 100W USBC 15W
# 14:  F-USBC 100W USBC 65W
# 15:  F-USBC 100W USBC 100W
*/
#define    XBH_F_USBC_0W_USBC_0W      0
#define    XBH_F_USBC_0W_USBC_15W     1
#define    XBH_F_USBC_0W_USBC_65W     2
#define    XBH_F_USBC_0W_USBC_100W    3
#define    XBH_F_USBC_15W_USBC_0W     4
#define    XBH_F_USBC_15W_USBC_15W    5
#define    XBH_F_USBC_15W_USBC_65W    6
#define    XBH_F_USBC_15W_USBC_100W   7
#define    XBH_F_USBC_65W_USBC_0W     8
#define    XBH_F_USBC_65W_USBC_15W    9
#define    XBH_F_USBC_65W_USBC_65W    10
#define    XBH_F_USBC_65W_USBC_100W   11
#define    XBH_F_USBC_100W_USBC_0W    12
#define    XBH_F_USBC_100W_USBC_15W   13
#define    XBH_F_USBC_100W_USBC_65W   14
#define    XBH_F_USBC_100W_USBC_100W  15
#define    XBH_F_USBC_100W_USBC_96W   16

#define XBH_BASED_ON_OPS_PLUG 0     //根据OPS拔插改变功率
#define XBH_BASED_ON_OPS_POWER 1    //根据OPS开关机改变功率

enum XBH_UPDATE_FIRMWARE_E
{
    XBH_UPDATE_FRONT_DEV6563_1    = 0x000,//dp to hdmi in
    XBH_UPDATE_FRONT_DEV6563_2    = 0x001,//dp to hdmi in
    XBH_UPDATE_FRONT_DEV6563_3    = 0x002,//dp to hdmi in
    XBH_UPDATE_FRONT_DEV6563_4    = 0x003,//dp to hdmi in
    XBH_UPDATE_BOARD_DEV6563_1    = 0x004,//dp to hdmi in
    XBH_UPDATE_BOARD_DEV6563_2    = 0x004,//dp to hdmi in
    XBH_UPDATE_BOARD_DEV6563_3    = 0x006,//dp to hdmi in
    XBH_UPDATE_BOARD_DEV6563_4    = 0x007,//dp to hdmi in
    XBH_UPDATE_BOARD_GSV2705_1    = 0x008,
    XBH_UPDATE_BOARD_GSV2705_2    = 0x009,
    XBH_UPDATE_BOARD_GSV2715_1    = 0x00a,
    XBH_UPDATE_BOARD_GSV2715_2    = 0x00b,
    XBH_UPDATE_BOARD_GSV6712_1    = 0x00c,
    XBH_UPDATE_BOARD_GSV2712      = 0x00d,
    XBH_UPDATE_FRONT_GSV2712      = 0x00e,
    XBH_UPDATE_BOARD_GSV2202E     = 0x00f,
    XBH_UPDATE_FRONT_GSV2202E     = 0x010,
    XBH_UPDATE_BOARD_GSV2702      = 0x011,
    XBH_UPDATE_FRONT_GSV2702      = 0x012,
    XBH_UPDATE_BOARD_CSV5363      = 0x013,//DP to hdmi
    XBH_UPDATE_FRONT_CSV5363      = 0x014,//DP to hdmi

    XBH_UPDATE_BOARD_LT8711       = 0x100,//TYPEC to HDMI
    XBH_UPDATE_FRONT_LT8711       = 0x101,//TYPEC to HDMI
    XBH_UPDATE_BOARD_LT8711UXE1   = 0x102,//TYPEC to HDMI
    XBH_UPDATE_FRONT_LT8711UXE1   = 0x103,//TYPEC to HDMI
    XBH_UPDATE_BOARD_LT8668SX_1   = 0x104,//hdmi + dp to hdmi
    XBH_UPDATE_BOARD_LT8668SXC_1  = 0x105,//hdmi to edp
    XBH_UPDATE_BOARD_RTS5450M     = 0x106,//board 5450
    XBH_UPDATE_FRONT_RTS5450M     = 0x107,//front 5450
    XBH_UPDATE_MS9282             = 0x108,//VGA TO HDMI

    XBH_UPDATE_ITE68411           = 0x200,//vbo + hdmi out
    XBH_UPDATE_MN869129           = 0x201,//vbo + hdmi out
    XBH_UPDATE_ITE6265            = 0x202,//vbo + hdmi out
    XBH_UPDATE_ITE6563OUT         = 0x203,//dp to hdmi out
    XBH_UPDATE_LT6711A            = 0x204,//hdmi to typec out
    XBH_UPDATE_CS5803AQ           = 0X205,//vbo + hdmi out
    XBH_UPDATE_BOARD_GSV6125      = 0X206,//hdmi to typec out

    XBH_UPDATE_FRONT_VL105        = 0x300,//TYPEC to HDMI
    XBH_UPDATE_BOARD_VL105        = 0x301,//TYPEC to HDMI

    XBH_UPDATE_MCU                = 0x400,
    XBH_UPDATE_AUDIO_CODEC        = 0x500
};

#define HID_MAX_LENGTH          128

//ext rtc define
#define HYM8563_IC          0x01
#define PT7C4337_IC         0x02

//kernel message to eywa for screen state
#define XBH_KERNEL_SEND_TO_EYWA_SCREEN_OFF          (0x64)
#define XBH_KERNEL_SEND_TO_EYWA_SCREEN_ON           (0x65)

//hdmi switch ic id >0
#define ITE663335_1         0x01    //66353 一级
#define ITE663335_2         0x02    //66353 二级
#define GSV2712_1           0x03    //2712 一级
#define GSV2712_2           0x04    //2712 二级
#define LT8641_1            0x05    //8641 一级
#define LT8641_2            0x06    //8641 二级
#define GSV2715_1           0x07    //2715 一级
#define GSV2715_2           0x08    //2715 二级
#define GSV2705_1           0x09    //2705 一级
#define GSV2705_2           0x0A    //2705 二级
#define GSV6712_1           0x0B    //6712 一级
#define GSV6712_2           0x0C    //6712 二级
#define LT8668SX_1          0x0D    //8668SX 一级
#define LT8668SX_2          0x0E    //8668SX 二级
#define GSV2702_1           0x0F    //2702 一级
#define GSV2702_2           0x10    //2702 二级

#define ASW3642             0xA0 //1切2的hdmi switch
#define TS3DV642            0xA1 //1切2的hdmi switch

//video out ic, vbo to hdmi, hdmi to edp etc
#define MN869129            0x01
#define ITE6265             0x02
#define LT8668SXC           0X03
#define ITE6563OUT          0X04    //dp to hdmi out
#define NT68411             0x05
#define LT6711A             0x06
#define CS5803AQ            0x07
#define GSV6125             0x08

//dp 2 hdmi ic
#define ITE6563             0x01
#define CSV5363             0x02

//usbc 2 hdmi ic
#define LT8711              0x01
#define LT8711UXE1          0x02
#define GSV2201E            0x03
#define GSV2202E            0x04

//vga 2 hdmi ic
#define MS9282              0x01
#define MSTV56              0x02

//codec
#define AP8224              0x01
#define AP8232              0x02

//rj45toUsb
#define AX88179              0x01
#define AX88279              0x02

//amplifer
#define AD82120_STEREO           0x01    //82120功放立体声
#define AD82120_SUBWOOFER        0x02    //82120功放低音炮
#define ACM8625_STEREO           0x03    //8625功放立体声
#define ACM8625_SUBWOOFER        0x04    //8625功放低音炮
#define TAS5805M_STEREO          0x05    //5805功放立体声
#define TAS5805M_SUBWOOFER       0x06    //5805功放低音炮
//G-Sensor
#define MC3416              0x01

//AIR Sensor
#define AM1008              0x01
#define MCUAM1008           0x02

//power sensor
#define Bl0942              0x01

//NFC
#define PN7160              0x01
#define MCUNFC              0x02

//net switch
#define RTL8367             0x01
#define RTL8305             0x02

//audio to I2s
#define ES7210              0x01
#define ES7243              0x02

// vbo tx signal switch
#define VBO_TX_DIFF_SIGNALS_TURN_OFF                0x00
#define VBO_TX_DIFF_SIGNALS_TURN_ON                 0x01

//light sensor
#define JSA1141             0x01
#define V2000               0x02
#define RISA311             0x03
#define TSL25721            0x04
#define TSL2561             0x05
#define LTRX140A            0x06
#define ALS199              0x07
#define STK3210             0x08
//支持的light sensor的最大数量
#define LIGHTSENSOR_MAX     STK3210 + 1

//Mcu
#define RK_GD32F310G8       0x01
#define MT_GD32F310G8       0x02
#define GD32F330R8          0x03
#define GD32F305RB          0x04
#define AT32F407RC          0x05

// PD IC
#define BOARD_RTS5450M            0x01
#define FRONT_RTS5450M            0x02

//前置辅板
#define FRONT_NONE_BOARD        0x00
#define FRONT2_P2_BOARD         0x01    //HDMI直连，usbc只支持usb连接

//与FRONT3.G共用
#define FRONT2_U_BOARD          0x10    //HDMI + usbc. ASW3642 1切2 SWITCH, USBC使用LT8711转HDMI

#define FRONT2_U2_BOARD         0x20    //HDMI + usbc 全功能，switch为2712
#define FRONT2_U2_2_BOARD       0x21    //HDMI + usbc 只有USB，switch为2702

#define FRONT2_U3_BOARD         0x30    //单USBC, USBC 支持4K 60HZ，转换芯片为GSV2201E
#define FRONT2_U3_2_BOARD       0x31    //单USBC, USBC 支持4K 30HZ，转换芯片为GSV2201E残血版

#define FRONT2_U4_BOARD         0x40    //单路usbc直连，转换芯片LT8711

#define FRONT2_U8_BOARD         0x80    //HDMI + usbc 全功能，switch为2712

#define FRONT2_U9_BOARD         0x9000    //HDMI + usbc 全功能，switch为2712，和U8只有充电硬件差异且共用固件

#define FRONT3_U5_BOARD         0x50    //HDMI + usbc 全功能，switch为2712

#define FRONT2_VS34_BOARD       0X34    //HDMI + usbc 全功能，switch为2712

#define FRONT2_T2_BOARD         0x1000  //HDMI + usbc 全功能，switch为2702

#define FRONT2_S2_BOARD         0x2000  //HDMI + usbc 全功能，switch为2712

#define FRONT3_U8_BOARD         0x3008  //HDMI + usbc 全功能，switch为2712

#ifndef HYM8563_I2C_ADDR_WR
#define HYM8563_I2C_ADDR_WR 0x51
#endif

#ifndef PT7C4337_I2C_ADDR_WR
#define PT7C4337_I2C_ADDR_WR 0x68
#endif


#define XBH_AUDIO_AQ_STR_INI_FILE                        "/mnt/xbhcust/aq/audio.ini"
#define XBH_AUDIO_AQ_SUB_INI_FILE                        "/mnt/xbhcust/aq/audio_subwoofer.ini"

#define XBH_AUDIO_ACM8625S_AQ_STR_INI_FILE                       "/mnt/xbhcust/aq/audio_acm8625.ini"
#define XBH_AUDIO_ACM8625S_AQ_SUB_INI_FILE                       "/mnt/xbhcust/aq/audio_subwoofer_acm8625.ini"

//GPIO
#define SET_IO_OUT          _IOW('v', 0x02, unsigned int)
#define SET_IO_VAL          _IOW('v', 0x03, unsigned int)
#define SET_IO_IN           _IOW('v', 0x04, unsigned int)
#define GET_IO_VAL          _IOR('v', 0x05, unsigned int)
#define GET_IO_DIR          _IOR('v', 0x06, unsigned int)
#define GET_IO_OUT_VAL      _IOR('v', 0x07, unsigned int)
#define REQUEST_IRQ         _IOW('v', 0x08, unsigned int)
#define FREE_ALL_IRQ        _IOW('v', 0x09, unsigned int)


#define IRQF_TRIGGER_RISING         0x01//:上升沿触发
#define IRQF_TRIGGER_FALLING        0x02//:下降沿触发
#define IRQF_TRIGGER_HIGH           0x04//:高电平触发
#define IRQF_TRIGGER_LOW            0x08//:低电平触发
#define IRQF_ONESHOT                0x00002000//:Interrupt is not reenabled after the hardirq handler finished
//IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING:双边沿

//HDCP使能通道
#define XBH_HDMI_A_1    0
#define XBH_HDMI_A_2    1
#define XBH_DP_1        2

//------------------------ cusdata start ------------------------
// one block is 0x100(256), the end of offset need to follow value:
// 0x00, 0x200, 0x400, 0x600, 0x800, 0xa00, 0xc00, 0xe00
// Ethernet mac address
#define XBH_CUSDATA_MACADDRESS_OFFSET           0x00
#define XBH_CUSDATA_MACADDRESS_LEN              0x12//18

// HDCP1.4 key for HDMI in
#define XBH_CUSDATA_HDCP_KEY_14_OFFSET          0x200//0x00 + 0x200(0x12)
#define XBH_CUSDATA_HDCP_KEY_14_LEN             0x1b4//436

// HDCP1.4 name for HDMI in
#define XBH_CUSDATA_HDCP_KEY_14_NAME_OFFSET     XBH_CUSDATA_HDCP_KEY_14_OFFSET + XBH_CUSDATA_HDCP_KEY_14_LEN + 2//950
#define XBH_CUSDATA_HDCP_KEY_14_NAME_LEN        0x40//64

// HDCP2.2 key for HDMI in
#define XBH_CUSDATA_HDCP_KEY_22_OFFSET          0x400//0x200 + 0x200(0x140)
#define XBH_CUSDATA_HDCP_KEY_22_LEN             0x414//1044

// HDCP2.2 name for HDMI in
#define XBH_CUSDATA_HDCP_KEY_22_NAME_OFFSET     XBH_CUSDATA_HDCP_KEY_22_OFFSET + XBH_CUSDATA_HDCP_KEY_22_LEN + 2//2070
#define XBH_CUSDATA_HDCP_KEY_22_NAME_LEN        0x40//64

// Serial number of Lango or customer
#define XBH_CUSDATA_BARCODE_OFFFSET             0xa00//0x400 + 0x600
#define XBH_CUSDATA_BARCODE_LEN                 0x40//64

// Proejct id for one bin and multi panel.
#define XBH_CUSDATA_PROJECT_ID_OFFSET           0xe00//0xa00 + 0x400
#define XBH_CUSDATA_PROJECT_ID_LEN              0x40//64

// Edid infomation such as product date, sn.
// Not the edid.bin
#define XBH_CUSDATA_EDID_INFO_OFFSET            0x1000//0xe00 + 0x200
#define XBH_CUSDATA_EDID_INFO_LEN               0x20//32

// CI KEY of TV
#define XBH_CUSDATA_CI_KEY_OFFSET               0x1200//0x1000 + 0x200
#define XBH_CUSDATA_CI_KEY_LEN                  0x1a00

// Eshare attestation code
#define XBH_CUSDATA_ESHARE_CODE_OFFSET          0x2c00//0x1200 + 0x1a00
#define XBH_CUSDATA_ESHARE_CODE_LEN             0x08

// Feitu attestation code
#define XBH_CUSDATA_FEITU_CODE_OFFSET           0x2e00//0x2c00 + 0x200
#define XBH_CUSDATA_FEITU_CODE_LEN              0x12//18

// Lango attestation code, most use in array mic
#define XBH_CUSDATA_LANGO_ATTESTATIONE_OFFSET   0x3000//0x2e00 + 0x200
#define XBH_CUSDATA_LANGO_ATTESTATIONE_LEN      0x20//32

// Google key name
#define XBH_CUSDATA_GOOGLE_KEY_NAME_OFFSET      0x3200//0x3000 + 0x200
#define XBH_CUSDATA_GOOGLE_KEY_NAME_LEN         0x40//64

// Board type
#define XBH_CUSDATA_BOARD_TYPE_OFFSET           0x3400//0x3200 + 0x200
#define XBH_CUSDATA_BOARD_TYPE_LEN              0x20//32

// Color temp data
#define XBH_CUSDATA_COLORTEMP_DATA_OFFSET       0x3600//0x3400 + 0x200
#define XBH_CUSDATA_COLORTEMP_DATA_LEN          (sizeof(struct XBH_GAIN_OFFSET_DATA_S) * XBH_COLORTEMP_BUTT) //336

//device_id
#define XBH_CUSDATA_BOARD_DEVICE_ID_OFFSET      0x3800//0x3600 + 0x200
#define XBH_CUSDATA_BOARD_DEVICE_ID_LEN         0x40//

//MultiPanel vbo 单分屏/双分屏
#define XBH_CUSDATA_MULTIPANEL_OFFSET           0x4000//0x3800 + 0x200
#define XBH_CUSDATA_MULTIPANE_LEN               0x01//

//Panel index 屏参序号
#define XBH_CUSDATA_PANEL_IDX_OFFSET            0x4100//0x4000 + 0x100
#define XBH_CUSDATA_PANEL_IDX_LEN               0x02//

// tx hdcp1.4 key
#define XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_OFFSET   0x4200//0x4100 + 0x100
#define XBH_CUSDATA_TX_RAW_HDCP1_4_KEY_LEN      0x134// 308

//Panel 奇偶和map
#define XBH_CUSDATA_PANEL_MAP_SWAP_OFFSET       0x4400//0x4200 + 0x200
#define XBH_CUSDATA_PANEL_MAP_SWAP_LEN          0x01//

// chip runtime
#define XBH_CUSDATA_CHIP_RUNTIME_OFFSET         0x4600//0x4400 + 0x200
#define XBH_CUSDATA_CHIP_RUNTIME_LEN            0x40//64

// usbc config for edla certification
#define XBH_CUSDATA_USBC_CONFIG_OFFSET          0x4700//0x4600 + 0x100
#define XBH_CUSDATA_USBC_CONFIG_LEN             0x01//

// moka工厂P模式要求升级固件保存标志位 set Moka P Mode Enable To Cusdata
#define XBH_CUSDATA_MOKA_P_MODE_ENABLE_OFFSET   0x4800//0x4700 + 0x100
#define XBH_CUSDATA_MOKA_P_MODE_ENABLE_LEN      0x01//

// smart-KTC工厂指令FLEX数据存储用于SFT
#define XBH_CUSDATA_SMART_SFT_OFFSET            0x4900//0x4800 + 0x100
#define XBH_CUSDATA_SMART_SFT_LEN               0x40//64

// smart-KTC工厂指令PN，同一个系列的机型是一样的
#define XBH_CUSDATA_SMART_PN_OFFSET             0x4A00//0x4900 + 0x100
#define XBH_CUSDATA_SMART_PN_LEN                0x20//32

// smart-KTC工厂指令SN，同一个系列的机型是一样的
#define XBH_CUSDATA_SMART_SN_OFFSET             0x4B00//0x4A00 + 0x100
#define XBH_CUSDATA_SMART_SN_LEN                0x20//32
#define XBH_CUSDATA_SMART_SN_REAL_LEN           0x0D//13
// SMART希望罗盘关闭的状态能永久保存，复位不被清除
#define XBH_CUSDATA_COMPASS_PRESIST_OFFSET             0x4C00//0x4B00 + 0x100
#define XBH_CUSDATA_COMPASS_PRESIST_LEN                0x01//


// smart工厂需要存储底板的PN码，一个系列对应一个PN码
#define XBH_CUSDATA_SMART_SCALER_PN_OFFSET             0x4D00//0x4C00 + 0x100
#define XBH_CUSDATA_SMART_SCALER_PN_LEN                0x20//32

// smart工厂需要存储底板的SN码，一个系列对应一个SN码
#define XBH_CUSDATA_SMART_SCALER_SN_OFFSET             0x4E00//0x4D00 + 0x100
#define XBH_CUSDATA_SMART_SCALER_SN_LEN                0x20//32

// smart工厂需要存储核心板的PN码，一个系列对应一个PN码
#define XBH_CUSDATA_SMART_CORE_PN_OFFSET             0x4F00//0x4E00 + 0x100
#define XBH_CUSDATA_SMART_CORE_PN_LEN                0x20//32

// smart工厂需要存储核心板的SN码，一个系列对应一个SN码
#define XBH_CUSDATA_SMART_CORE_SN_OFFSET             0x5000//0x4F00 + 0x100
#define XBH_CUSDATA_SMART_CORE_SN_LEN                0x20//32

// OPS MAC 1 地址
#define XBH_CUSDATA_OPS_MAC_ADDRESS_OFFSET           0x5100//0x5000 + 0x100
#define XBH_CUSDATA_OPS_MAC_ADDRESS_LEN              0x20//32

// TYPE-C MAC 1 地址
#define XBH_CUSDATA_TPYEC_MAC_ADDRESS_OFFSET         0x5200//0x5100 + 0x100
#define XBH_CUSDATA_TPYEC_MAC_ADDRESS_LEN            0x20//32

// OPS MAC 2 地址
#define XBH_CUSDATA_OPS_MAC_ADDRESS_OFFSET_2       0x5300//0x5200 + 0x100
#define XBH_CUSDATA_OPS_MAC_ADDRESS_LEN_2          0x20//32

// TYPE-C MAC 2 地址
#define XBH_CUSDATA_TPYEC_MAC_ADDRESS_OFFSET_2       0x5400//0x5300 + 0x100
#define XBH_CUSDATA_TPYEC_MAC_ADDRESS_LEN_2          0x20//32

// FATT物料-核心板SN
#define XBH_CUSDATA_CORE_SN_OFFSET                   0x5500//0x5400 + 0x100
#define XBH_CUSDATA_CORE_SN_LEN                      0x20//32

// FATT物料-辅板SN_1
#define XBH_CUSDATA_AUXILIARY_SN_1_OFFSET            0x5600//0x5500 + 0x100
#define XBH_CUSDATA_AUXILIARY_SN_1_LEN               0x20//32

// FATT物料-辅板SN_2
#define XBH_CUSDATA_AUXILIARY_SN_2_OFFSET            0x5700//0x5600 + 0x100
#define XBH_CUSDATA_AUXILIARY_SN_2_LEN               0x20//32

// FATT物料-辅板SN_3
#define XBH_CUSDATA_AUXILIARY_SN_3_OFFSET            0x5800//0x5700 + 0x100
#define XBH_CUSDATA_AUXILIARY_SN_3_LEN               0x20//32

// smart-KTC工厂指令RkpStatus，同一个系列的机型是一样的
#define XBH_CUSDATA_SMART_RKP_STATUS_OFFSET          0x5900//0x5800 + 0x100
#define XBH_CUSDATA_SMART_RKP_STATUS_LEN             0x40//64

// smart pn sn separator smart的需求
#define XBH_CUSDATA_SMART_PN_SN_SEPARATOR_OFFSET     0x5a00//0x5900 + 0x100
#define XBH_CUSDATA_SMART_PN_SN_SEPARATOR_LEN        0x20//32

// googlekey data
#define XBH_CUSDATA_GOOGLE_KEY_DATA_OFFSET           0x500000//0x50000 + 0x19000
#define XBH_CUSDATA_GOOGLE_KEY_DATA_LEN              0x19000//100*1024=102400=0x19000=100k
//------------------------ cusdata end ------------------------



//------------------------ cusparam start ------------------------
// for example
#define XBH_CUSPARAM_CHIP_RUNTIME_OFFSET                0x00
#define XBH_CUSPARAM_CHIP_RUNTIME_LEN                   0x40//64

// Wakeup switch, wot 开关
#define XBH_CUSPARAM_WAKEUP_SWITCH_OFFSET               0x200
#define XBH_CUSPARAM_WAKEUP_SWITCH_LEN                  0x20//32

// Wakeup info，唤醒的信息
#define XBH_CUSPARAM_WAKEUP_INFO_OFFSET                 0x400
#define XBH_CUSPARAM_WAKEUP_INFO_LEN                    0x20//4

// power mode，上电模式
#define XBH_CUSPARAM_POWER_ON_MODE_OFFSET               0x600
#define XBH_CUSPARAM_POWER_ON_MODE_LEN                  0x01//1

//hdmirx edid type
#define XBH_CUSPARAM_BOARD_HDMIRX_EDID_OFFSET           0x800
#define XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN              0x01//

//set Aging Mode State
#define XBH_CUSPARAM_AGING_MODE_STATE_OFFSET            0xA00
#define XBH_CUSPARAM_AGING_MODE_STATE_LEN               0x01//

// set led light enable
#define XBH_CUSPARAM_LED_LIGHT_ENBALE_OFFSET            0xC00
#define XBH_CUSPARAM_LED_LIGHT_ENBALE_LEN               0x01//

// shudownFlag
#define XBH_CUSPARAM_SHUTDOWN_FLAG_OFFSET               0xE00
#define XBH_CUSPARAM_SHUTDOWN_FLAG_LEN                  0x01//1

//monitor_id
//用来存储设备编号，iiyama rs232协议和遥控器码值中可以带monitor id，monitor id匹配上才能响应
#define XBH_CUSPARAM_BOARD_MONITOR_ID_OFFSET             0x1000
#define XBH_CUSPARAM_BOARD_MONITOR_ID_LEN                0x40//

// Hdmi out vedio mute state
#define XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_OFFSET          0x1200
#define XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_LEN             0x01

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_HDMI1_EDID_TYPE_OFFSET              0x1400//0x1200 + 0x200
#define XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_HDMI2_EDID_TYPE_OFFSET              0x1600//0x1400 + 0x200
#define XBH_CUSPARAM_HDMI2_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_HDMI3_EDID_TYPE_OFFSET              0x1800//0x1600 + 0x200
#define XBH_CUSPARAM_HDMI3_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_HDMI4_EDID_TYPE_OFFSET              0x1A00//0x1800 + 0x200
#define XBH_CUSPARAM_HDMI4_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_DP1_EDID_TYPE_OFFSET                0x1C00//0x1A00 + 0x200
#define XBH_CUSPARAM_DP1_EDID_TYPE_LEN                   0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_DP2_EDID_TYPE_OFFSET                0x1E00//0x1C00 + 0x200
#define XBH_CUSPARAM_DP2_EDID_TYPE_LEN                   0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_USBC1_EDID_TYPE_OFFSET              0x2000//0x1E00 + 0x200
#define XBH_CUSPARAM_USBC1_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_USBC2_EDID_TYPE_OFFSET              0x2200//0x2000 + 0x200
#define XBH_CUSPARAM_USBC2_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_OPS1_EDID_TYPE_OFFSET               0x2400//0x2200 + 0x200
#define XBH_CUSPARAM_OPS1_EDID_TYPE_LEN                  0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_OPS2_EDID_TYPE_OFFSET               0x2600//0x2400 + 0x200
#define XBH_CUSPARAM_OPS2_EDID_TYPE_LEN                  0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_FRONT_HDMI1_EDID_TYPE_OFFSET        0x2800//0x2600 + 0x200
#define XBH_CUSPARAM_FRONT_HDMI1_EDID_TYPE_LEN           0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_FRONT_HDMI2_EDID_TYPE_OFFSET        0x2A00//0x2800 + 0x200
#define XBH_CUSPARAM_FRONT_HDMI2_EDID_TYPE_LEN           0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_F_USBC1_EDID_TYPE_OFFSET            0x2C00//0x2A00 + 0x200
#define XBH_CUSPARAM_F_USBC1_EDID_TYPE_LEN               0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_F_USBC2_EDID_TYPE_OFFSET            0x2E00//0x2C00 + 0x200
#define XBH_CUSPARAM_F_USBC2_EDID_TYPE_LEN               0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_F_DP1_EDID_TYPE_OFFSET              0x3000//0x2E00 + 0x200
#define XBH_CUSPARAM_F_DP1_EDID_TYPE_LEN                 0x20//32

// 记录每个信源的edid 类型
#define XBH_CUSPARAM_F_DP2_EDID_TYPE_OFFSET              0x3200//0x3000 + 0x200
#define XBH_CUSPARAM_F_DP2_EDID_TYPE_LEN                 0x20//32
//------------------------ cusparam end ------------------------


#endif //XBH_COMMON_H
