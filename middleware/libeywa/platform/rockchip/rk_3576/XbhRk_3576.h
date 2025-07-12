#ifndef XBH_RK_3576_H
#define XBH_RK_3576_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhPlatformInterface.h"
#include "XbhModuleInterface.h"
#include "XbhSysOpt.h"
#include "keybox_util.h"
#include "hdcp.h"

#include <string>
#include <fstream>
#include <map>
using namespace std;

#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
/* GPIO mode definitions */
#define CTL_CLTR(regval)               (BITS(0,1) & ((uint32_t)(regval) << 0))
#define MCU_GPIO_MODE_INPUT            CTL_CLTR(0)           /*!< input mode */
#define MCU_GPIO_MODE_OUTPUT           CTL_CLTR(1)           /*!< output mode */
#define MCU_GPIO_MODE_AF               CTL_CLTR(2)           /*!< alternate function mode */
#define MCU_GPIO_MODE_ANALOG           CTL_CLTR(3)           /*!< analog mode */

#define PUD_PUPD(regval)               (BITS(0,1) & ((uint32_t)(regval) << 0))
#define MCU_GPIO_PUPD_NONE             PUD_PUPD(0)           /*!< floating mode, no pull-up and pull-down resistors */
#define MCU_GPIO_PUPD_PULLUP           PUD_PUPD(1)           /*!< with pull-up resistor */
#define MCU_GPIO_PUPD_PULLDOWN         PUD_PUPD(2)           /*!< with pull-down resistor */

#define MCU_GPIO_OTYPE_PP              ((uint8_t)(0x00U))    /*!< push pull mode */
#define MCU_GPIO_OTYPE_OD              ((uint8_t)(0x01U))    /*!< open drain mode */

#define OSPD_OSPD0(regval)             (BITS(0,1) & ((uint32_t)(regval) << 0))
#define MCU_GPIO_OSPEED_2MHZ           OSPD_OSPD0(0)                     /*!< output max speed 2MHz */
#define MCU_GPIO_OSPEED_10MHZ          OSPD_OSPD0(1)                     /*!< output max speed 10MHz */
#define MCU_GPIO_OSPEED_50MHZ          OSPD_OSPD0(3)                     /*!< output max speed 50MHz */
#define MCU_GPIO_OSPEED_MAX            ((uint32_t)0x0000FFFFU)           /*!< GPIO very high output speed, max speed more than 50MHz */

#define TV_INPUT_SRC_RANGE "persist.vendor.tvinput.rkpq.src.range"
#define BACKLIGHT_PATH "/sys/class/backlight/backlight/brightness"
#define HDMIRX_CSI_HDMI_RX_5V_DET_PATH "/sys/class/hdmirx/rk628/hdmi_rx_5v_det"
#define ADC_INT_EDABLED_PATH "/sys/bus/iio/devices/iio:device0/xbh_write_adc_int_enabled"
#define HDMIRX_LOCK_STATUS "/sys/class/hdmirx/rk628/hdmi_rx_lock"

#define WAKEUP_CEC                      0
#define WAKEUP_IR                       1
#define WAKEUP_GPIO                     2
#define WAKEUP_UART                     3

#define VIDEO_FD "/dev/v4l-subdev2"
#define BASE_VIDIOC_PRIVATE 192     /* 192-255 are private */
#define HDMIRX_CMD_GET_SIGNAL_STABLE_STATUS \
	_IOR('V', BASE_VIDIOC_PRIVATE + 1, int)

struct XBH_MCU_GPIO_ATTR_S
{
    uint32_t port:4;
    uint32_t pin:4;
    uint32_t mode:2;
    uint32_t pupd:2;
    uint32_t otype:1;
    uint32_t speed:2;
    uint32_t level:1;
    uint32_t checksum:8;
    uint32_t reserved:8;
};

struct WakeUp_Info {
	uint8_t wakeup_flag;
	uint8_t source;
	uint16_t data;
} ;

union XBH_MCU_GPIOPARA_U
 {
    XBH_U32 value;
    XBH_MCU_GPIO_ATTR_S attr;
};

class XbhRk_3576 : public XbhPlatformInterface, public XbhModuleInterface
{
public:
    XbhRk_3576();
    virtual ~XbhRk_3576(){};
    //override
    XBH_S32 setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    //override
    XBH_S32 getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue);
    //override
    XBH_S32 getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue);
    //override
    XBH_S32 requestGpioIrq(XBH_U32 u32GpioNumber, XBH_U32 u32Type,XBH_BOOL enable = XBH_FALSE);
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
    XBH_S32 getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32 *u32Value);
    //override
    XBH_S32 getTemperatureSensorValue(XBH_FLOAT *fValue);
    //override
    XBH_S32 getDebugEnable(XBH_BOOL *bEnable);
    //override
    XBH_S32 setRtcAlarmTime(XBH_RTC_ALARM_INFO_S *stRtcAlarmInfo);
    //override
    XBH_S32 setLedPwrStatus(XBH_LED_LIGHT_E enState);
    //override
    XBH_S32 getLedPwrStatus(XBH_LED_LIGHT_E* enState);
    //override
    XBH_S32 getLightSensorValue(XBH_S32* s32Value);
    //override
    XBH_S32 getLightSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 setDeviceId(const XBH_CHAR* strSn);
    //override
    XBH_S32 getDeviceId(XBH_CHAR* strSn);
    //override
    XBH_S32 setSn(const XBH_CHAR* strSn);
    //override
    XBH_S32 getSn(XBH_CHAR* strSn);
    //override
    XBH_S32 setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL *enable);
    //override
    XBH_S32 setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable);
    //override
    XBH_S32 setChipRunTime(XBH_S32 time);
    //override
    XBH_S32 getChipRunTime(XBH_S32* time);
    //override
    XBH_S32 setGammaGroup(XBH_S32 s32Value);
    //override
    XBH_S32 getGammaGroup(XBH_S32* s32Value);
    //override
    XBH_S32 setPictureMode(XBH_PICMODE_E enPicMode);
    //override
    XBH_S32 getPictureMode(XBH_PICMODE_E* enPicMode);
    //override
    XBH_S32 setColorTempMode(XBH_COLORTEMP_E enColorTemp);
    //override
    XBH_S32 getColorTempMode(XBH_COLORTEMP_E *enColorTemp);
    //override
    XBH_S32 getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setBrightness(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightness(XBH_U32* u32Value);
    //override
    XBH_S32 setContrast(XBH_U32 u32Value);
    //override
    XBH_S32 getContrast(XBH_U32* u32Value);
    //override
    XBH_S32 setSaturation(XBH_U32 u32Value);
    //override
    XBH_S32 getSaturation(XBH_U32* u32Value);
    //override
    XBH_S32 setSharpEnable(XBH_U32 u32Value);
    //override
    XBH_S32 setSharpness(XBH_U32 u32Value);
    //override
    XBH_S32 getSharpness(XBH_U32* u32Value);
    //override
    XBH_S32 setHue(XBH_U32 u32Value);
    //override
    XBH_S32 getHue(XBH_U32* u32Value);
    //override
    XBH_S32 setHDMIColorRange(XBH_COLOR_RANGE_E enColorRange);
    //override
    XBH_S32 getHDMIColorRange(XBH_COLOR_RANGE_E* enColorRange);
    //override
    XBH_S32 setBacklightWithOutSave(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setHdmiRx5vDet(XBH_BOOL enable);
    //override
    XBH_S32 setSourceScreenOffset(XBH_S32 x, XBH_S32 y);
    //override
    XBH_S32 getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y);
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
    XBH_S32 getWakeUpReason(XBH_WAKEUP_S *stWake);
    //override
    XBH_S32 setPoweronMode(XBH_POWERMODE_E enMode);
    //override
    XBH_S32 getPoweronMode(XBH_POWERMODE_E *enMode);
    //override
    XBH_S32 setPixelShift(int x, int y);
    //override
    XBH_S32 getPixelShift(int *x, int *y);
    //override
    XBH_S32 setBrightMax(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightMax(XBH_U32 *u32Value);
    //override
    XBH_S32 setBrightMin(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightMin(XBH_U32 *u32Value);
    //override
    XBH_S32 setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setRtcTime(XBH_RTC_INFO_S* stRtcInfo);
    //override
    XBH_S32 getRtcTime(XBH_RTC_INFO_S* stRtcInfo);
    //override
    XBH_S32 getTemperatureSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 setMcuFattMode(XBH_S32 mode);
    //override
    XBH_S32 getMcuFattMode(XBH_S32 *mode);
    //override
    XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 saveFattKeypadInfo(XBH_S32 keypad_enable, XBH_S32 key_count, XBH_S32 key_index);
    //override
    XBH_S32 setFattKeypadenbale(XBH_S32 keypad_enable);
    //override
    XBH_S32 getFattKeypadInfo(XBH_S32* keypad_enable, XBH_S32* key_count, XBH_S32* key_index);
    //override
    XBH_S32 setBootLogo(const XBH_CHAR* path);
    //override
    XBH_S32 setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput);
    //override
    XBH_S32 getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput);
    //override
    XBH_S32 loadEdlaCust(XBH_BOOL *enable);
    //override
    XBH_S32 setWatchDogManualModeEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 getWatchDogManualModeEnable(XBH_BOOL *bEnable);
    //override
    XBH_S32 setWatchDogEnable(XBH_BOOL bEnable);
    //override
    XBH_S32 setWatchDogTimeOut(XBH_U32 u32TimeOutSec);
    //override
    XBH_S32 getWatchDogTimeOut(XBH_U32 *u32TimeOutSec);
    //override
    XBH_S32 watchDogPing(XBH_VOID);
    //override
    XBH_S32 setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud);
    //override
    XBH_S32 setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getPwmFrequency(XBH_U32* u32PwmF, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setPwmFrequency(XBH_U32 u32PwmF, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setHdmirxEdidType(XBH_S32 type);
    //override
    XBH_S32 getHdmirxEdidType(XBH_S32 *type);
    //override
    XBH_S32 getSourceEdidType(XBH_U32 u32SourceId, XBH_HDMIRX_EDID_TYPE_E *enType);
    //override
    XBH_S32 setSourceEdidType(XBH_HDMIRX_EDID_TYPE_E enType, XBH_U32 u32SourceId);
    //override
    XBH_S32 setGoogleKey(const XBH_CHAR* strPath);
    //override
    XBH_S32 getGoogleKeyStatus(XBH_BOOL* bEnable);
    //override
    XBH_S32 getSocHdmiEdidStatus(XBH_BOOL *enable);
    //override
    XBH_S32 setBacklightEnable(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getBacklightEnable(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setPanelRotation(XBH_S32 value, XBH_PANEL_NUM_E enPanel = XBH_PANEL_MAIN);
    //override
    XBH_S32 setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable);
    //override
    XBH_S32 getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff);
    //override
    XBH_S32 getHdcpKeydata(XBH_SOURCE_E source,XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff);
    //override
    XBH_S32 setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName);
    //override
    XBH_S32 setGoogleKeyName(const XBH_CHAR* strPath);
    //override
    XBH_S32 getGoogleKeyName(XBH_CHAR* strPath);
    //override
    XBH_S32 setAttentionKey(const XBH_CHAR* strPath);
    //override
    XBH_S32 getAttentionKeyStatus(XBH_BOOL* enable);
    //override 
    XBH_S32 getRkpCsrInfo(XBH_CHAR* read_buffer, int buffer_size);
    //override
    XBH_S32 setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 getPanelMapSwap(XBH_S32 * pBuff);
    //override
    XBH_S32 setPanelMapSwap(XBH_S32  pBuff);
    //override
    XBH_S32 setUsbcConfig(XBH_S32 type);
    //override
    XBH_S32 getUsbcConfig(XBH_S32* type);
    //override
    XBH_S32 setGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32 gpioDirection, XBH_U32 u32GpioValue);
    //override
    XBH_S32 getGpioCommonCtrl(const XBH_CHAR* gpioName, XBH_U32* u32GpioValue);
    //override
    XBH_S32 setHdmiLowLatency(XBH_BOOL bEnable);
    //override
    XBH_S32 setShutdownFlag(XBH_BOOL enable);
    //override
    XBH_S32 getEthPortSpeed(XBH_S32 port, XBH_S32* value);
    //override
    XBH_S32 setProjectId(const XBH_CHAR* strProjectId);
    //override
    XBH_S32 getProjectId(XBH_CHAR* strProjectId);
    //override
    XBH_S32 setSftData(const XBH_CHAR* data);
    //override
    XBH_S32 getSftData(XBH_CHAR* data);
    //override
    XBH_S32 setRkpStatus(const XBH_CHAR* data);
    //override
    XBH_S32 getRkpStatus(XBH_CHAR* data);
    //override
    XBH_S32 setSmartPnData(const XBH_CHAR* data);
    //override
    XBH_S32 getSmartPnData(XBH_CHAR* data);
    //override
    XBH_S32 setSmartSnData(const XBH_CHAR* data);
    //override
    XBH_S32 getSmartSnData(XBH_CHAR* data);
    //override
    XBH_S32 setCompassPresist(const XBH_CHAR* data);
    //override
    XBH_S32 getCompassPresist(XBH_CHAR* data);
    //override
    XBH_S32 setSmartScalerPnData(const XBH_CHAR* data);
    //override
    XBH_S32 getSmartScalerPnData(XBH_CHAR* data);
    //override
    XBH_S32 setSmartScalerSnData(const XBH_CHAR* data);
    //override
    XBH_S32 getSmartScalerSnData(XBH_CHAR* data);
    //override
    XBH_S32 setSmartCorePnData(const XBH_CHAR* data);
    //override
    XBH_S32 getSmartCorePnData(XBH_CHAR* data);
    //override
    XBH_S32 setSmartCoreSnData(const XBH_CHAR* data);
    //override
    XBH_S32 getSmartCoreSnData(XBH_CHAR* data);
    //override
    XBH_S32 getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum);
    //override
    XBH_S32 dumpEdid(XBH_SOURCE_E idx);
    //override
    std::string getUsbDeviceInfos(const std::string& input);
    //override
    XBH_S32 saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType, const XBH_CHAR* data);
    //override
    XBH_S32 getFactoryMac(XBH_MAC_ADDRESS_TYPE macType, XBH_CHAR* data);
    //override
    XBH_S32 getGoogleKeyID(XBH_CHAR* deviceID);
    //override
    XBH_S32 burnFactorySN(XBH_MATERAL_TYPE materialType, const XBH_CHAR* snInfo);
    //override
    XBH_S32 getBurnFactorySN(XBH_MATERAL_TYPE materialType, XBH_CHAR* snInfo);
    //override
    XBH_S32 getBacklightWithOutSave(XBH_U32* value, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getSignalStatus(XBH_S32 *status);
    //override
    XBH_S32 setDelayTimeForHdmiSwitch();
    //override
    XBH_S32 getHdmiRxLockStatus(XBH_BOOL* lock);
    //override
    XBH_S32 setOnStop();
    //override
    XBH_S32 prepareIncreaseBinUpgrade();
    //override
    XBH_S32 getEthPlugStatus(XBH_BOOL* bEnable);
    //override
    XBH_S32 setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx);

public:
    XBH_S32 readAndUpdateEdidBinFileByEdidType(int dev, const char* edidBinFilePath, int port);
    XBH_S32 readAndUpdateEdidBinFile(int projectid);
    XBH_S32 setAdcIntEnabled(XBH_BOOL bEnable);
    XBH_S32 freeAllGpioIrq();
    XBH_S32 setIndependenceSourceEdidType(XBH_U32 source,XBH_S32 type);
    XBH_S32 getIndependenceSourceEdidType(XBH_U32 source,XBH_S32 *type);

private:
    XBH_S32 open_i2c_dev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet);
    XBH_S32 check_iic_funcs(XBH_S32 file);
    XBH_S32 set_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    XBH_S32 get_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    XBH_S32 initSerialPort();
    XBH_S32 setGpioOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    XBH_S32 setGpioInput(XBH_U32 u32GpioNumber);
    XBH_S32 getGpioDir(XBH_U32 u32GpioNumber);
    XBH_S32 getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level);
    XBH_S32 setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level);
    XBH_U8 calMcuGpioChecksum(XBH_MCU_GPIOPARA_U *gpioPara);
    XBH_S32 loadProperty();
    XBH_S32 loadPq();
    XBH_S32 checkColorTempParatGain(XBH_U32 gain);
    XBH_S32 checkColorTempParatOffset(XBH_U32 gain);
    XBH_S32 checkColorTempParat(XBH_GAIN_OFFSET_DATA_S* data);
    XBH_S32 setcheckColorTempValue(XBH_BOOL CheckColorTemp);
    XBH_S32 getcheckColorTempValue(XBH_BOOL *CheckColorTemp);
    XBH_S32 checkColorTempValue(XBH_GAIN_OFFSET_DATA_S* data);
    XBH_S32 deleteFile(const std::string& path);
    XBH_S32 deleteDirectorySubfiles(const std::string& dir_path);

private:
    XBH_S32 mGpioFd = -1;
    XBH_S32 mKeypadEnable = 0;
    XBH_S32 mKeypadCount = 0;
    XBH_S32 mKeypadIndex = 0;
    struct termios uart_debug_config;
    XBH_BOOL mCheckColorTemp ;
    XBH_AUDIO_OUTPUT_E mAudioOutput;
    XBH_S32 mWatchDogFd = -1;
    XBH_BOOL mWatchDogManualMode = 0;
    XBH_BOOL mIsProjectIdChange;
    enum HDCP_KEY_ID hdcp_key_id;
    static map<int, string> net_list;
    XBH_BOOL mColorTempStorage;     //色温参数是否存储到不可擦除分区
};

#endif //XBH_RK_3576_H
