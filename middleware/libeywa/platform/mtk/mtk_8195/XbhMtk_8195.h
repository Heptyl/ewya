#ifndef XBH_MTK_8195_H
#define XBH_MTK_8195_H

#define I2C_OK                              0x0000

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhPlatformInterface.h"
#include "XbhModuleInterface.h"
#include <vendor/mediatek/hardware/nvram/1.0/INvram.h>
#include <hardware/board.h>

#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
#define CTL_CLTR(regval)               (BITS(0,1) & ((uint32_t)(regval) << 0))
#define MCU_GPIO_MODE_INPUT            CTL_CLTR(0)           /*!< input mode */
#define MCU_GPIO_MODE_OUTPUT           CTL_CLTR(1)           /*!< output mode */
#define MCU_GPIO_MODE_AF               CTL_CLTR(2)           /*!< alternate function mode */
#define MCU_GPIO_MODE_ANALOG           CTL_CLTR(3)           /*!< analog mode */

/* GPIO mode definitions */
#define GPIO_MODE_AIN                    ((XBH_U8)0x00U)          /*!< analog input mode */
#define GPIO_MODE_IN_FLOATING            ((XBH_U8)0x04U)          /*!< floating input mode */
#define GPIO_MODE_IPD                    ((XBH_U8)0x28U)          /*!< pull-down input mode */
#define GPIO_MODE_IPU                    ((XBH_U8)0x48U)          /*!< pull-up input mode */
#define GPIO_MODE_OUT_OD                 ((XBH_U8)0x14U)          /*!< GPIO output with open-drain */
#define GPIO_MODE_OUT_PP                 ((XBH_U8)0x10U)          /*!< GPIO output with push-pull */
#define GPIO_MODE_AF_OD                  ((XBH_U8)0x1CU)          /*!< AFIO output with open-drain */
#define GPIO_MODE_AF_PP                  ((XBH_U8)0x18U)          /*!< AFIO output with push-pull */

/* GPIO output max speed value */
#define GPIO_OSPEED_10MHZ                ((XBH_U8)0x01U)          /*!< output max speed 10MHz */
#define GPIO_OSPEED_2MHZ                 ((XBH_U8)0x02U)          /*!< output max speed 2MHz */
#define GPIO_OSPEED_50MHZ                ((XBH_U8)0x03U)          /*!< output max speed 50MHz */
#define GPIO_OSPEED_MAX                  ((XBH_U8)0x04U)          /*!< GPIO very high output speed, max speed more than 50MHz */



/*  MAX I2C RETRY TIMES*/
#define MAX_RETRIES                      5

#define XBH_SIGNAL_STATUS_8195_SIGNAL    5
#define HDMIRX_DEV_PATH "/dev/hdmirx"
#define MTK_HDMIRX_DEV_INFO HDMI_IOWR(4, struct HDMIRX_DEV_INFO)
#define HDMI_IOWR(num, dtype) _IOWR('H', num, dtype)

struct HDMIRX_DEV_INFO {
        unsigned char hdmirx5v;
        unsigned char hpd;
        unsigned int power_on;
        unsigned char state;
        unsigned char vid_locked;
        unsigned char aud_locked;
        unsigned char hdcp_version;
        unsigned char hdcp14_decrypted;
        unsigned char hdcp22_decrypted;
};

struct XBH_MCU_GPIO_ATTR_S
{
    uint32_t port:4;
    uint32_t pin:4;
    uint32_t mode:8;
    uint32_t speed:3;
    uint32_t level:1;
    uint32_t reserved:12;
};

union XBH_MCU_GPIOPARA_U
{
    XBH_U32 value;
    XBH_MCU_GPIO_ATTR_S attr;
};

using ::vendor::mediatek::hardware::nvram::V1_0::INvram;
using ::android::sp;

class XbhMtk_8195 : public XbhPlatformInterface, public XbhModuleInterface
{
public:
    XbhMtk_8195();
    virtual ~XbhMtk_8195(){};

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
    XBH_S32 readI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32Length, XBH_U8* u8Data);
    //override
    XBH_S32 getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable);
    //override
    XBH_S32 getSocHdmiEdidStatus(XBH_BOOL *enable);
    //override
    XBH_S32 setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress);
    //override
    XBH_S32 setDebugEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getDebugEnable(XBH_BOOL *bEnable);
    //override
    XBH_S32 getADCChannelValue(XBH_U32 u32Channel, XBH_U32 *u32Value);
    //override
    XBH_S32 getTemperatureSensorValue(XBH_FLOAT *fValue);
    //override
    XBH_S32 getTemperatureSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
     //override
    XBH_S32 setAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 getAndroidColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 setRtcTime(XBH_RTC_INFO_S* stRtcInfo);
    //override
    XBH_S32 getRtcTime(XBH_RTC_INFO_S* stRtcInfo);
    //override
    XBH_S32 setHdmirxEdidType(XBH_S32 type);
    //override
    XBH_S32 getHdmirxEdidType(XBH_S32 *type);
    //override
    XBH_S32 setRtcAlarmTime(XBH_RTC_ALARM_INFO_S* stRtcAlarmInfo);
    //override
    XBH_S32 isNfcExist(XBH_BOOL *enable);
    //override
    XBH_S32 setNfcEnable(XBH_BOOL enable);
    //override
    XBH_S32 getNfcEnable(XBH_BOOL *enable);
    //override
    XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    //override
    XBH_S32 addNfcId(XBH_U8 *data, XBH_U32 length);
    //override
    XBH_S32 delNfcId(XBH_U8 *data, XBH_U32 length);
    //override
    XBH_S32 delAllNfcId();
    //override
    XBH_S32 setSn(const XBH_CHAR* strSn);
    //override
    XBH_S32 getSn(XBH_CHAR* strSn);
    //override
    XBH_S32 setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff);
    //override
    XBH_S32 getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable);
    //override
    XBH_S32 setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName);
    //override
    XBH_S32 setMcuFattMode(XBH_S32 mode);
    //override
    XBH_S32 getMcuFattMode(XBH_S32 *mode);
    //override
    XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 setBootLogo(const XBH_CHAR* path);
    //override
    XBH_S32 setBootAnimation(const XBH_CHAR* path);
    //override
    XBH_S32 setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput);
    //override
    XBH_S32 getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput);
    //override
    XBH_S32 getEthPortSpeed(XBH_S32 port, XBH_S32* value);
    //override
    XBH_S32 requestGpioIrq(XBH_U32 u32GpioNumber, XBH_U32 u32Type,XBH_BOOL enable = XBH_FALSE);
    //override
    XBH_S32 freeAllGpioIrq();
    //override
    XBH_S32 getWakeUpReason(XBH_WAKEUP_S *stWake);
    //override
    XBH_S32 getSignalStatus(XBH_S32 *status);
    //override
    XBH_S32 getHdmiRxAudioSampleFreq(XBH_U32 *u32Data);
    //override
    XBH_S32 getHdmiRxAudioLocked(XBH_U32 *u32Data);
    //override
    XBH_S32 setGoogleKey(const XBH_CHAR* strPath);
    //override
    XBH_S32 getGoogleKeyStatus(XBH_BOOL* bEnable);
    //override
    XBH_S32 setGoogleKeyName(const XBH_CHAR* strPath);
    //override
    XBH_S32 getGoogleKeyName(XBH_CHAR* strPath);
    //override
    XBH_S32 setAttentionKey(const XBH_CHAR* strPath);
    //override
    XBH_S32 getAttentionKeyStatus(XBH_BOOL* enable);
    //override
    XBH_S32 setRkpStatus(const XBH_CHAR* data);
    //override
    XBH_S32 getRkpStatus(XBH_CHAR* data);
    //override
    XBH_S32 getMicLicenseState(XBH_S32* status);
    //override
    XBH_S32 ProcessTypeBHotplug(XBH_SOURCE_E src);
    //override
    XBH_S32 ProcessTypeCHotplug(XBH_SOURCE_E src);

    //子类调用更新去写SN.
    XBH_S32 updateNvmStr(XBH_CHAR* data);
public:
    XBH_AUDIO_OUTPUT_E mAudioOutput;

private:
    //mcu gpio
    XBH_U8 calMcuGpioChecksum(XBH_MCU_GPIOPARA_U *gpioPara);
    XBH_S32 setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level);
    XBH_S32 getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level);
    //chip gpio
    XBH_S32 setGpioDirOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    XBH_S32 setGpioDirInput(XBH_U32 u32GpioNumber);
    XBH_S32 getGpioDir(XBH_U32 u32GpioNumber);
    //i2c func
    XBH_S32 openI2cDev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet);
    XBH_S32 checkI2cFunc(XBH_S32 file);
    XBH_S32 setChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    XBH_S32 getChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    XBH_S32 readChipI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32Length, XBH_U8* u8Data);
    //init debug
    XBH_S32 initSerialPort();
    //nvram func
    void convertHexStringToNumbers(const char* retStr, std::vector<uint8_t>& buffArr, size_t offset, size_t size);
    void addHexStringToBuffer(const std::vector<std::string>& hexBuffArr, std::vector<uint8_t>& buffArr, size_t offset, size_t size);
    std::vector<std::string> toHexStringArr(const std::string& s);
    XBH_S32 setNvramValue(XBH_S32 offset, XBH_S32 size, XBH_VOID *buff);
    XBH_S32 getNvramValue(XBH_S32 offset, XBH_S32 size, XBH_VOID *buff);
    //notify mcu enable keypower
    XBH_S32 setMcuEnableKeyPower();
    XBH_S32 handleLicenseFileMissing();
private:
    XBH_S32 mGpioFd = -1;
    sp<INvram> mpINvram = nullptr;
    static XbhMutex m_McuLock;
    static XbhMutex m_NvmLock;
    struct termios uart_debug_config;
};

#endif //XBH_MTK_8195_H
