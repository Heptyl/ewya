#ifndef XBH_MTK_9679_H
#define XBH_MTK_9679_H

#define I2C_OK                              0x0000

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhPlatformInterface.h"
#include "XbhModuleInterface.h"

#define BITS(start, end)             ((0xFFFFFFFFUL << (start)) & (0xFFFFFFFFUL >> (31U - (uint32_t)(end))))
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

union XBH_MCU_GPIOPARA_U
 {
    XBH_U32 value;
    XBH_MCU_GPIO_ATTR_S attr;
};

class XbhMtk_9679 : public XbhPlatformInterface, public XbhModuleInterface
{
public:
    XbhMtk_9679();
    virtual ~XbhMtk_9679(){};
    //获取HDMI直连的情况下的连接状态
    XBH_S32 getHdmiDirectDet(int bd_src, XBH_BOOL *enable);
    //获取CVBS直连的情况下的连接状态
    XBH_S32 getCvbsDirectDet(int bd_src, XBH_BOOL *enable);
    //设置hdmi hpd 电平状态
    XBH_S32 setHdmiHPD(int bd_src, XBH_LEVEL_E level);
    //设置OPS开机时需要检测OPS信号是否及时输出的功能
    XBH_S32 setOpsReidentify(XBH_BOOL enable);
    //-------------------------------------------------------------------------------override---------------------------------------------------------------

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
    XBH_S32 getADCChannelValue(XBH_ADC_CHANNEL_NUM_E enChannel, XBH_U32 *u32Value);
    //override
    XBH_S32 getTemperatureSensorValue(XBH_FLOAT *fValue);
    //override
    XBH_S32 getDebugEnable(XBH_BOOL *bEnable);
    //override
    XBH_S32 setRtcTime(XBH_RTC_INFO_S *stRtcInfo);
    //override
    XBH_S32 getRtcTime(XBH_RTC_INFO_S *stRtcInfo);
    //override
    XBH_S32 setRtcAlarmTime(XBH_RTC_ALARM_INFO_S *stRtcAlarmInfo);
    //override
    XBH_S32 getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL *enable);
    //override
    XBH_S32 setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable);
    //override
    XBH_S32 loadEdlaCust(XBH_BOOL *enable);
    //override
    XBH_S32 setSn(const XBH_CHAR* strSn);
    //override
    XBH_S32 getSn(XBH_CHAR* strSn);
    //override
    XBH_S32 setMacAddress(XBH_S32 macType, const XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getMacAddress(XBH_S32 macType, XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getTemperatureSensorExist(XBH_BOOL* bEnable);
    //override
    XBH_S32 setChipRunTime(XBH_S32 time);
    //override
    XBH_S32 getChipRunTime(XBH_S32* time);
    //override
    XBH_S32 setStandbyMode(XBH_U32 u32Mode);
    //override
    XBH_S32 getStandbyMode(XBH_U32 *u32Mode);
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
    XBH_S32 getSocHdmiEdidStatus(XBH_BOOL *enable);
    //override
    XBH_S32 getBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* strMacAddress);
    //override
    XBH_S32 getWakeUpReason(XBH_WAKEUP_S *stWake);
    //override
    XBH_S32 setAudioOutput(XBH_AUDIO_OUTPUT_E enAudioOutput);
    //override
    XBH_S32 getAudioOutput(XBH_AUDIO_OUTPUT_E *enAudioOutput);
    //override
    XBH_S32 setSourceScreenOffset(XBH_S32 x, XBH_S32 y);
    //override
    XBH_S32 getSourceScreenOffset(XBH_S32 *x, XBH_S32 *y);
    //override
    XBH_S32 setBacklightWithOutSave(XBH_U32 u32Value, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setSocFreeRun(XBH_BOOL enable);
    //override
    XBH_S32 setPwmPolarity(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPwmPolarity(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getPanelResolution(XBH_CHAR* resolution, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 processOpsSignalStatus(XBH_SOURCE_E u32Source);
    //override
    XBH_S32 setMcuFattMode(XBH_S32 mode);
    //override
    XBH_S32 getMcuFattMode(XBH_S32 *mode);
    //override
    XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud);
    //override
    XBH_S32 setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    //override
    XBH_S32 setProjectId(const XBH_CHAR* strProjectId);
    //override
    XBH_S32 getProjectId(XBH_CHAR* strProjectId);
    //override
    XBH_S32 setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //override
    XBH_S32 setDeviceId(const XBH_CHAR* strSn);
    //override
    XBH_S32 getDeviceId(XBH_CHAR* strSn);
    //override
    XBH_S32 setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpKey(XBH_HDCP_TYPE_E type, XBH_CHAR* pBuff);
    //override
    XBH_S32 setHdcpKeyName(const XBH_CHAR* strHdcpFileName, XBH_HDCP_TYPE_E type);
    //override
    XBH_S32 getHdcpKeyName(XBH_HDCP_TYPE_E type, XBH_CHAR* strHdcpFileName);
    //override
    XBH_S32 setGoogleKeyName(const XBH_CHAR* strPath);
    //override
    XBH_S32 getGoogleKeyName(XBH_CHAR* strPath);
    //override
    XBH_S32 setMultiPanel(XBH_S32 type, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 getMultiPanel(XBH_S32* type, XBH_PANEL_NUM_E enPanel);
    //override
    XBH_S32 setHdmiTxVideoMuteState(XBH_BOOL state);
    //override
    XBH_S32 getHdmiTxVideoMuteState(XBH_BOOL *state);
    //override
    XBH_S32 setAgingModeState(XBH_S32 u32State);
    //override
    XBH_S32 getAgingModeState(XBH_S32 *u32State);
    //override
    XBH_S32 setMokaPModeEnableToCusdata(XBH_S32 u32State);
    //override
    XBH_S32 getMokaPModeEnableFromCusdata(XBH_S32 *u32State);
    //override
    XBH_S32 getEthPlugStatus(XBH_BOOL* bEnable);
    //override
    XBH_S32 setBrightMax(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightMax(XBH_U32 *u32Value);
    //override
    XBH_S32 setBrightMin(XBH_U32 u32Value);
    //override
    XBH_S32 getBrightMin(XBH_U32 *u32Value);
    //override
    XBH_S32 getHdmiPortClockStatus(XBH_U32 port, XBH_S32 *status);

private:
    XBH_U8 calMcuGpioChecksum(XBH_MCU_GPIOPARA_U *gpioPara);
    XBH_S32 setMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum,  XBH_U32 level);
    XBH_S32 getMcuGpioValue(XBH_S32 u8Group, XBH_S32 u8GpioNum, XBH_S32 mode, XBH_U32 *level);
    XBH_S32 open_i2c_dev(XBH_U32 i2cbus, XBH_CHAR *filename, size_t size, XBH_S32 quiet);
    XBH_S32 check_iic_funcs(XBH_S32 file);
    XBH_S32 set_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    XBH_S32 get_i2c_data(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    XBH_S32 initSerialPort();
    XBH_S32 setGpioOutput(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    XBH_S32 setGpioInput(XBH_U32 u32GpioNumber);
    XBH_S32 getGpioDir(XBH_U32 u32GpioNumber);
    XBH_S32 loadProperty();
    XBH_S32 loadWoTStatus();
    XBH_S32 initWolStatus();

private:
    XBH_S32 mGpioFd = -1;
    struct termios uart_debug_config;
    XBH_AUDIO_OUTPUT_E mAudioOutput;
public:
    //OPS开机后，判断是否需要拉HPD
    XBH_S32 s32OpsReidentify;
};

#endif //XBH_MTK_9679_H
