#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMH610_A"

#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhXMH610_A.h"
#include "XbhSysOpt.h"
#include "XbhSourceManager.h"
#include "XbhHi_610.h"
#include "XbhPartitionData.h"
#include "XbhLiftbracketManager.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <hardware/board.h>

XbhXMH610_A            *XbhXMH610_A::mInstance = XBH_NULL;
XbhMutex                XbhXMH610_A::mLock;

#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"
#define HDMI_RX_EDID            "/sys/class/hdmirx/rk628/edid_index"
#define UART_IRQ_DEV            "/proc/interrupts"
#define USBC1_OUT_DET_VALUE      0x0f
#define H610_FHDMI_MAX_ADC_VALUE            360
#define H610_FHDMI_MIN_ADC_VALUE            340
#define H610_FUSBC_MAX_ADC_VALUE            530
#define H610_FUSBC_MIN_ADC_VALUE            510
#define H610_BOTH_PLUG_IN_MAX_ADC_VALUE     280
#define H610_BOTH_PLUG_IN_MIN_ADC_VALUE     250
#define H610_DET_SOURCE_ADC_MDELAY          10

static XBH_U32 gYLedValue = XBH_BOARD_GPIO_UART_LED_Y_ON;
static XBH_U32 gGLedValue = XBH_BOARD_GPIO_UART_LED_G_ON;
static long cur_rxcount = 0;
static long cur_txcount = 0;
void XbhXMH610_A::run(const void *arg)
{
    XBH_CHAR buffer[4096];
    int fd;
    XBH_S32 bytes_read;
    XBH_CHAR *line;
    long rx_count = 0;
    long tx_count = 0;

    fd = open(UART_IRQ_DEV, O_RDONLY);
    if (fd < 0)
    {
        XLOGE("open /proc/interrupts error!!!\n");
    }
    else
    {
        bytes_read = read(fd, buffer, sizeof(buffer) - 1);
        close(fd);

        if (bytes_read <= 0)
        {
            XLOGE("Error reading file");
            close(fd);
        }
        buffer[bytes_read] = '\0';

        line = strtok(buffer, "\n");;
        while (line != NULL)
        {
            while (isspace((unsigned char)*line)) line++;
            
            if (strncmp(line, "14:", 3) == 0)   //tx中断触发次数
            {
                tx_count = 0;
                XBH_CHAR *ptr = line + 3;

                while (*ptr)
                {
                    while (*ptr && !isdigit((unsigned char)*ptr)) ptr++;
                    if (!*ptr) break;

                    tx_count += strtol(ptr, &ptr, 10);
                }
            }
            if (strncmp(line, "15:", 3) == 0)   //rx中断触发次数
            {
                rx_count = 0;
                XBH_CHAR *ptr = line + 3;

                while (*ptr)
                {
                    while (*ptr && !isdigit((unsigned char)*ptr)) ptr++;
                    if (!*ptr) break;

                    rx_count += strtol(ptr, &ptr, 10);
                }
            }
            line = strtok(NULL, "\n");
        }

        if (cur_txcount == tx_count)
        {
            gGLedValue = XBH_BOARD_GPIO_UART_LED_G_ON;
        }
        else if (cur_txcount < tx_count)
        {
            gGLedValue = !gGLedValue;
        }
        
        if (cur_rxcount == rx_count)
        {
            gYLedValue = XBH_BOARD_GPIO_UART_LED_Y_ON;
        }
        else if (cur_rxcount < rx_count)
        {
            gYLedValue = !gYLedValue;
        }

        setGpioOutputValue(XBH_BOARD_GPIO_UART_LED_G, gGLedValue);
        setGpioOutputValue(XBH_BOARD_GPIO_UART_LED_Y, gYLedValue);

        cur_txcount = tx_count;
        cur_rxcount = rx_count;
    }

    usleep(50 * 1000); //50ms
}

XbhXMH610_A::XbhXMH610_A()
{
    XLOGD(" begin ");
    XbhHi_610::setShutdownFlag(XBH_FALSE);

    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhXMH610_A::~XbhXMH610_A()
{
}

XBH_VOID XbhXMH610_A::checkAdcValueEffective(XBH_U32 *adcValue, XBH_U32 mDelay)
{
    XBH_U8 hdmiPlugIn = 0;
    XBH_U8 usbcPlugIn = 0;
    XBH_U8 bothPlugIn = 0;
    XBH_U32 value = 0;
    for(int i = 0; i < 5; ++i)
    {
        getADCChannelValue(XBH_ADC_CHANNEL_NUM_2, &value);
        if( value >= (H610_FHDMI_MIN_ADC_VALUE) && value <= (H610_FHDMI_MAX_ADC_VALUE) )
        {
            hdmiPlugIn++;
        }
        else if( value >= (H610_FUSBC_MIN_ADC_VALUE) && value <= (H610_FUSBC_MAX_ADC_VALUE) )
        {
            usbcPlugIn++;
        }
        else if( value >= (H610_BOTH_PLUG_IN_MIN_ADC_VALUE) && value <= (H610_BOTH_PLUG_IN_MAX_ADC_VALUE) )
        {
            bothPlugIn++;
        }
        usleep(mDelay * 1000);
    }

    if( (hdmiPlugIn >= usbcPlugIn) && (hdmiPlugIn >= bothPlugIn) )
    {
        *adcValue = H610_FHDMI_MIN_ADC_VALUE + (H610_FHDMI_MAX_ADC_VALUE - H610_FHDMI_MIN_ADC_VALUE) / 2;
    }
    else if( (usbcPlugIn >= hdmiPlugIn) && (usbcPlugIn >= bothPlugIn) )
    {
        *adcValue = H610_FUSBC_MIN_ADC_VALUE + (H610_FUSBC_MAX_ADC_VALUE - H610_FUSBC_MIN_ADC_VALUE) / 2;
    }
    else
    {
        *adcValue = H610_BOTH_PLUG_IN_MIN_ADC_VALUE + (H610_BOTH_PLUG_IN_MAX_ADC_VALUE - H610_BOTH_PLUG_IN_MIN_ADC_VALUE) / 2;
    }
    if ( !(hdmiPlugIn || usbcPlugIn || bothPlugIn) )
    {
        *adcValue = 0;
    }
}

XBH_S32 XbhXMH610_A::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_DET
    XBH_U32 u32GpioValue;
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    XBH_U32 value = 0;
    XBH_U8 fHdmiDetVal = 0;

    checkAdcValueEffective(&value, H610_DET_SOURCE_ADC_MDELAY);
    if( (value >= H610_FHDMI_MIN_ADC_VALUE) && (value <= H610_FHDMI_MAX_ADC_VALUE) ) //单独接入前置hdmi
    {
        fHdmiDetVal = 1;
    }

    if( (value >= H610_BOTH_PLUG_IN_MIN_ADC_VALUE) && (value <= H610_BOTH_PLUG_IN_MAX_ADC_VALUE) ) //前置typec和前置hdmi同时接入
    {
        fHdmiDetVal = 1;
    }

    *enable = u32GpioValue == fHdmiDetVal ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_A::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_DET
    XBH_U32 u32GpioValue;
    //s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    XBH_U32 value = 0;
    XBH_U8 fUsbcDetVal = 0;

    checkAdcValueEffective(&value, H610_DET_SOURCE_ADC_MDELAY);
    if( (value >= H610_FUSBC_MIN_ADC_VALUE) && (value <= H610_FUSBC_MAX_ADC_VALUE) ) //单独接入前置typec
    {
        fUsbcDetVal = 1;
    }

    if( (value >= H610_BOTH_PLUG_IN_MIN_ADC_VALUE) && (value <= H610_BOTH_PLUG_IN_MAX_ADC_VALUE) ) //前置typec和前置hdmi同时接入
    {
        fUsbcDetVal = 1;
    }

    *enable = u32GpioValue == fUsbcDetVal ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_A::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS1_PB_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_PB_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS1_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_A::getOps2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS2_PB_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_PB_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS2_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_A::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_PS_ON) || defined(XBH_BOARD_GPIO_OPS2_PS_ON)
    XBH_U32 u32OpspowerTimer = 500;
    XBH_U32 u32Ops12VTimer = 200;
    XBH_BOOL bIsDet = XBH_FALSE;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_PS_ON
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS1_PS_ON, XBH_BOARD_GPIO_OPS1_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS1_PS_ON, !XBH_BOARD_GPIO_OPS1_PS_ON_LEVEL);
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_PS_ON
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS2_PS_ON, XBH_BOARD_GPIO_OPS2_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS2_PS_ON, !XBH_BOARD_GPIO_OPS2_PS_ON_LEVEL);
        #endif
    }
    
    XLOGD("setOpsPowerCtrl(source=%d)\n", enSource);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_PWR_STATUS) || defined(XBH_BOARD_GPIO_OPS2_PWR_STATUS)
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_PWR_STATUS
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_PWR_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS1_PWR_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_PWR_STATUS
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_PWR_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS2_PWR_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    #endif
    return  s32Ret;
}

//获取接入的ops的系统类型,XBH_TRUE:android,XBH_FALSE:windows
XBH_S32 XbhXMH610_A::getOpsSystemType(XBH_BOOL *type, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_ANDROID_DET) || defined(XBH_BOARD_GPIO_OPS2_ANDROID_DET)
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_ANDROID_DET
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_ANDROID_DET, &u32GpioValue);
        *type =  (u32GpioValue == XBH_BOARD_GPIO_OPS1_ANDROID_DET_LEVEL) ? XBH_TRUE : XBH_FALSE;
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_ANDROID_DET
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_ANDROID_DET, &u32GpioValue);
        *type =  (u32GpioValue == XBH_BOARD_GPIO_OPS2_ANDROID_DET_LEVEL) ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_PB_DET) || defined(XBH_BOARD_GPIO_OPS2_PB_DET)
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_PB_DET
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_PB_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS1_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_PB_DET
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_PB_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS2_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_PWR_EN) || defined(XBH_BOARD_GPIO_OPS2_PWR_EN)
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_PWR_EN
        if (bEnable) {
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS1_PWR_EN, XBH_BOARD_GPIO_OPS1_PWR_EN_LEVEL);
        } else {
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS1_PWR_EN, !XBH_BOARD_GPIO_OPS1_PWR_EN_LEVEL);
        }
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_PWR_EN
        if (bEnable) {
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS2_PWR_EN, XBH_BOARD_GPIO_OPS2_PWR_EN_LEVEL);
        } else {
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS2_PWR_EN, !XBH_BOARD_GPIO_OPS2_PWR_EN_LEVEL);
        }
        #endif
    }
    XLOGD("setOpsPowerEnable(source=%d) enable=%d\n", enSource, bEnable);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_PWR_EN) || defined(XBH_BOARD_GPIO_OPS2_PWR_EN)
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_PWR_EN
        s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS1_PWR_EN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS1_PWR_EN_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_PWR_EN
        s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS2_PWR_EN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS2_PWR_EN_LEVEL ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::setOpsResetEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS1_RST
    if (bEnable)
    {
        s32Ret = this->XbhHi_610::setGpioOutputValue(XBH_BOARD_GPIO_OPS1_RST, XBH_BOARD_GPIO_OPS1_RST_LEVEL);
    }
    else
    {
        s32Ret = this->XbhHi_610::setGpioOutputValue(XBH_BOARD_GPIO_OPS1_RST, !XBH_BOARD_GPIO_OPS1_RST_LEVEL);
    }
    #endif
    XLOGD("setOpsResetEnable bEnable=%d\n", bEnable);
    return  s32Ret;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMH610_A::getOps1SignalStatus(XBH_S32 *status)
{
    return XBH_FAILURE;//this->XbhHi_610::getSignalStatus(status);
}

XBH_S32 XbhXMH610_A::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

//override
XBH_S32 XbhXMH610_A::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
#if defined(XBH_BOARD_GPIO_HALL_DET)
    *bEnable = 1;
#else
    *bEnable = 0;
#endif
    return  s32Ret;
}

//override
XBH_S32 XbhXMH610_A::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
#if defined(XBH_BOARD_GPIO_HALL_DET)
    XBH_U32 u32GpioValue = 0;
    s32Ret = XbhHi_610::getGpioInputValue(XBH_BOARD_GPIO_HALL_DET, &u32GpioValue );
    if (s32Ret == XBH_SUCCESS && u32GpioValue == XBH_BOARD_GPIO_HALL_DET_LEVEL)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;
    }
#endif
    return s32Ret;
}


XBH_S32 XbhXMH610_A::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type  = 2;
    //s32Ret = XbhHi_610::getHdmirxEdidType(&type);
    if(type == 1)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_14;
    }
    else if(type == 2)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_20;
    }
    else if(type == 3)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_21;
    }
    else if(type == 4)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_AUTO;
    }
    return s32Ret;
}

XBH_S32 XbhXMH610_A::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    #if 0
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->setHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->setMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_STEREO:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        default:
            break;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getMute channel = %d",enAudioChannel);
    #if 0
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_STEREO:
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->getHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->getMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->getMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        default:
            break;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMH610_A::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename,force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        default:
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMH610_A::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMH610_A::openEthWol(XBH_BOOL bEnable)
{
    int fd = open(RTK8367_WOL_DEV, O_WRONLY);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR mac_addr[17];
    if (bEnable)
    {
        if (fd == -1)
        {
            XLOGE("open_wol dev error!!!\n");
            return s32Ret;
        }

        getMacAddress(0, mac_addr);
        s32Ret = write(fd, mac_addr, 17);
        if (s32Ret < 0)
        {
            XLOGE("write wol dev error!!!\n");
        }
        close(fd);
        return s32Ret;
    }
    else
    {
        s32Ret = XBH_SUCCESS;
    }

    return s32Ret;
}

XBH_S32 XbhXMH610_A::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{

    XLOGD("getSrcDetByDetGpio gpio = %d u32GpioValue = u32GpioValue %d\n", gpio ,u32GpioValue);
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_SOURCE_E currSource;
    s32Ret = XbhSourceManager::getInstance()->getCurrSource(&currSource);
    XBH_BOOL isSpeakerMute = XBH_FALSE;
    //XBH_S32 result = getMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, &isSpeakerMute);

    #if 0
    switch(gpio)
    {
         case XBH_BOARD_GPIO_F_USBC_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_F_USBC1;
            if(u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL)
            {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_USBC1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
               *bEnable = XBH_FALSE;
               //setHdmirxControlState(false);
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_USBC1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
            }
            break;
        case XBH_BOARD_GPIO_F_HDMI_DET:
           *src = XBH_SOURCE_E::XBH_SOURCE_F_HDMI1;
           if(u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL)
           {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_HDMI1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
           }
           else
           {
               *bEnable = XBH_FALSE;
               //setHdmirxControlState(false);
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_F_HDMI1)
               {
                   setHdmiRx5vDet(XBH_FALSE);
                   if (result == XBH_SUCCESS && isSpeakerMute == XBH_FALSE) {
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                       usleep(1000 *1000);
                       setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
                   }
               }
               //setHdmiRxCsiSteamState(false);
           }
           break;
        default:
            *src = XBH_SOURCE_E::XBH_SOURCE_BUTT;
            break;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    #if defined(XBH_BOARD_GPIO_OPS1_PWR_EN) && defined(XBH_BOARD_GPIO_OPS1_PB_DET) \
            && defined(XBH_BOARD_GPIO_OPS2_PWR_EN) && defined(XBH_BOARD_GPIO_OPS2_PB_DET) \
            && defined(XBH_BOARD_GPIO_OPS_STB_PWR)
    if (s32Ret == XBH_SUCCESS && s8Buff[XBH_WAKEUP_SOURCE] == 1)
    {
        XBH_U32 ops1Pwr = 0;
        XBH_U32 ops1Status = 0;
        XBH_U32 ops2Pwr = 0;
        XBH_U32 ops2Status = 0;
        s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS1_PWR_EN, &ops1Pwr);
        s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_OPS1_PB_DET, &ops1Status);
        s32Ret |= getGpioOutputValue(XBH_BOARD_GPIO_OPS2_PWR_EN, &ops2Pwr);
        s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_OPS2_PB_DET, &ops2Status);
        if (s32Ret == XBH_SUCCESS
                && ((ops1Pwr  == XBH_BOARD_GPIO_OPS1_PWR_EN_LEVEL && ops1Status == XBH_BOARD_GPIO_OPS1_PB_DET_LEVEL)
                || (ops2Pwr  == XBH_BOARD_GPIO_OPS2_PWR_EN_LEVEL && ops2Status == XBH_BOARD_GPIO_OPS2_PB_DET_LEVEL)))
        {
            setGpioOutputValue(XBH_BOARD_GPIO_OPS_STB_PWR, XBH_BOARD_GPIO_OPS_STB_PWR_ON);
        }
        else
        {
            setGpioOutputValue(XBH_BOARD_GPIO_OPS_STB_PWR, !XBH_BOARD_GPIO_OPS_STB_PWR_ON);
        }
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_STB_PWR, !XBH_BOARD_GPIO_OPS_STB_PWR_ON);
    }
    #endif

    //s32Ret = XbhHi_610::setShutdownFlag(XBH_TRUE);
    //s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    //s32Ret |= setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    //setGpioOutputValue(XBH_BOARD_GPIO_AMP_MUTE,XBH_BOARD_GPIO_AMP_MUTE_ON);//关闭AMP PD
    #ifdef XBH_BOARD_GPIO_FUSBC_PWR
    setGpioOutputValue(XBH_BOARD_GPIO_FUSBC_PWR, !XBH_BOARD_GPIO_FUSBC_PWR_ON);
    #endif
    #ifdef XBH_BOARD_GPIO_NFC_SOC_NFC_PWR
    setGpioOutputValue(XBH_BOARD_GPIO_NFC_SOC_NFC_PWR, !XBH_BOARD_GPIO_NFC_SOC_NFC_PWR_ON);
    #endif

    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhXMH610_A::getUsbLockMode(XBH_U32 *u32UsbType)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.usb_lock_mode", type, "0");
    *u32UsbType = atoi(type);
    return s32Ret;
}

/**
 * 设置AVC(自动电压控制)开关，当输入源的声音过大时，能够限制输入，防止输出过大
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMH610_A::setAvcEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioAmplifierManager::getInstance()->setAvcEnable(bEnable);
    return  s32Ret;
}

/**
 * 获取AVC(自动电压控制)开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMH610_A::getAvcEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioAmplifierManager::getInstance()->getAvcEnable(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::setLiftbracketOperate(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhLiftbracketManager::getInstance()->setLiftbracketOperate(value);
    return s32Ret;
}

XBH_S32 XbhXMH610_A::getLiftbracketExist(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhLiftbracketManager::getInstance()->getLiftbracketExist(value);
    return  s32Ret;
}

XBH_S32 XbhXMH610_A::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //ret = XbhHi_610::setHdmiLowLatency(bEnable);
    return s32Ret;
}

XBH_S32 XbhXMH610_A::getNTCValue(XBH_S32 s32Value, XBH_FLOAT* fValue)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_U8 u8Data[1024] = {0};
    XBH_FLOAT result = 0;

    if(s32Value == 0)
    {
        ret = getTemperatureSensorValue(&result);
        XLOGD("getBoardTemperature :id = %d, temp = %f ", s32Value, result);
    } else {
       
        ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, 0xc2, 0x01, 4, u8Data);

        // memcpy(&result, u8Data, sizeof(result));
        result = u8Data[0] << 24 | u8Data[1] << 16 | u8Data[2] << 8 | u8Data[3];

        XLOGD("getBoardTemperature :id = %d,  temp = %f ",s32Value,  result);
    }
    *fValue = result;
    return ret;
}

XBH_S32 XbhXMH610_A::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    if(enSource == XBH_SOURCE_USBC1)
    {
        //ret = XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_BOARD_GSV2715_1, pdability);
    }
    else if(enSource == XBH_SOURCE_F_USBC1)
    {
        //ret = XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_F_USBC1, pdability);
    }
    else
    {
        XLOGE("The 'enSource' parameter is invalid.\n");
        return XBH_FAILURE;
    }
    return ret;
}

XbhXMH610_A *XbhXMH610_A::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMH610_A();
        }
    }
    return mInstance;
}



/**
 * 初始化project_id配置
 */
 XBH_S32 XbhXMH610_A::initProjectIdConfig()
 {
     //每次开机都需要更新edid的数据
     XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);

     return XBH_SUCCESS;
 }
