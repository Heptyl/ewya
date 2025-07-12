#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMH610_B"

#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhXMH610_B.h"
#include "XbhSysOpt.h"
#include "XbhSourceManager.h"
#include "XbhHi_610.h"
#include "XbhPartitionData.h"
#include "XbhLiftbracketManager.h"
#include "XbhUsbHubManager.h"
#include "hi_mw_cfg.h"
#include "hi_mw_array.h"
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
#include "XbhEventType.h"
#include "XbhService.h"
#include <time.h>
#include "uapi_otp.h"

XbhXMH610_B            *XbhXMH610_B::mInstance = XBH_NULL;
XbhMutex                XbhXMH610_B::mLock;

#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"
#define HDMI_RX_EDID            "/sys/class/hdmirx/rk628/edid_index"
#define USBC1_OUT_DET_VALUE      0x0f
#define H610_FHDMI_MAX_ADC_VALUE            360
#define H610_FHDMI_MIN_ADC_VALUE            340
#define H610_FUSBC_MAX_ADC_VALUE            530
#define H610_FUSBC_MIN_ADC_VALUE            510
#define H610_BOTH_PLUG_IN_MAX_ADC_VALUE     280
#define H610_BOTH_PLUG_IN_MIN_ADC_VALUE     250
#define H610_DET_SOURCE_ADC_MDELAY          10

//第1次ps_on信号低电平持续时间
const  XBH_U32           XbhXMH610_B::M_PS_ON_BASE_DELAY_TIME(700);
//低电平时长递增步长
const  XBH_U32           XbhXMH610_B::M_PS_ON_DELAY_TIME_STEP(200);
//不同低电平间隔时间
const  XBH_U32           XbhXMH610_B::M_PWR_STA_DECT_TIMES(3000);

//CRC TABLE
const static unsigned short CRC16_CCIT_TBL[256] =
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

void XbhXMH610_B::run(const void *arg)
{
    XBH_CHAR buf[32];
    snprintf(buf, sizeof(buf), "%d", getFattMode());
    XbhSysOpt::getInstance()->writeSysfs("/proc/tty_proc/fatt_status", buf);
    /*CRC超时检测*/
    CRCTimeoutCheck();

    OPSByPassProcess();
    usleep(250 * 1000); //250ms
}

void* XbhXMH610_B::setOpsPowerCtrlTask(void* arg)
{
    XbhXMH610_B* self = static_cast<XbhXMH610_B*>(arg);
#if defined(XBH_BOARD_GPIO_OPS1_PS_ON) || defined(XBH_BOARD_GPIO_OPS2_PS_ON)
    while(1)
    {
        if(self->mOps1OnFlag)
        {
            self->mOps1OnFlag = XBH_FALSE;
            self->setOpsPowerCtrlWithDurationMs(XBH_SOURCE_E::XBH_SOURCE_OPS1);
        }
        else if(self->mOps2OnFlag)
        {
            self->mOps2OnFlag = XBH_FALSE;
            self->setOpsPowerCtrlWithDurationMs(XBH_SOURCE_E::XBH_SOURCE_OPS2);
        }
        usleep(500 * 1000); //500ms
    }
#endif
}

XbhXMH610_B::XbhXMH610_B()
{
    XLOGD(" begin ");
    XbhHi_610::setShutdownFlag(XBH_FALSE);

    srand(time(NULL));

    if (property_get_bool("persist.vendor.xbh.crc.enable", XBH_FALSE) == XBH_TRUE)
    {
        setHdmiHPD(H610_HDMIRX_1_PORT_ID, XBH_LEVEL_LOW);   //OPS1
        setHdmiHPD(H610_HDMIRX_2_PORT_ID, XBH_LEVEL_LOW);   //OPS2
    }

    XBH_CHAR mac_address[PROPERTY_VALUE_MAX] = {0}; 
    if(getMacAddress(XBH_MACTYPE_RNDIS, mac_address) == XBH_SUCCESS)
    {
        XLOGD("mac_address: %s", mac_address);
        property_set("persist.vendor.xbh.rndis.macaddr", mac_address);
    }

    XbhMWThread::run(XbhMWThread::REPEAT);

    pthread_t setOpsPowerCtrlTid;

    if (pthread_create(&setOpsPowerCtrlTid, XBH_NULL, setOpsPowerCtrlTask, this) != XBH_SUCCESS)
    {
        XLOGE("Thread creation failed");
    }
    XLOGD("Thread create success");

    XLOGD(" end ");
}

XbhXMH610_B::~XbhXMH610_B()
{
}

XBH_VOID XbhXMH610_B::checkAdcValueEffective(XBH_U32 *adcValue, XBH_U32 mDelay)
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

XBH_S32 XbhXMH610_B::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_DET
    XBH_U32 u32GpioValue = 0;
    // s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
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

    *enable = (fHdmiDetVal == 1) ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_DET
    XBH_U32 u32GpioValue = 0;
    // s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
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

    *enable = (fUsbcDetVal == 1) ? XBH_TRUE : XBH_FALSE;
    #endif
    
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS1_PB_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_PB_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS1_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getOps2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS2_PB_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_PB_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS2_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}

XBH_S32 XbhXMH610_B::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (enSource == XBH_SOURCE_OPS1)
    {
        mOps1OnFlag = XBH_TRUE;
    }
    else if (enSource == XBH_SOURCE_OPS2)
    {
        mOps2OnFlag = XBH_TRUE;
    }

    XLOGD("setOpsPowerCtrl(source=%d)\n", enSource);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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
XBH_S32 XbhXMH610_B::getOpsSystemType(XBH_BOOL *type, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMH610_B::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMH610_B::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMH610_B::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMH610_B::getOpsWakeSoc(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_WAKE) || defined(XBH_BOARD_GPIO_OPS2_WAKE)
    XBH_U32 u32GpioValue = 0;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_WAKE_LEVEL
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_WAKE, &u32GpioValue);
        *bEnable =  (u32GpioValue == XBH_BOARD_GPIO_OPS1_WAKE_LEVEL) ? XBH_TRUE : XBH_FALSE;
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_WAKE_LEVEL
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_WAKE, &u32GpioValue);
        *bEnable =  (u32GpioValue == XBH_BOARD_GPIO_OPS2_WAKE_LEVEL) ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::getOpsOperationStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_BOARD_GPIO_OPS1_STATUS_DET) || defined(XBH_BOARD_GPIO_OPS2_STATUS_DET)
    XBH_U32 u32GpioValue = 0;
    if (enSource == XBH_SOURCE_OPS1) {
        #ifdef XBH_BOARD_GPIO_OPS1_STATUS_DET
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS1_STATUS_DET, &u32GpioValue);
        *bEnable =  (u32GpioValue == XBH_BOARD_GPIO_OPS1_STATUS_DET_LEVEL) ? XBH_TRUE : XBH_FALSE;
        #endif
    } else if (enSource == XBH_SOURCE_OPS2) {
        #ifdef XBH_BOARD_GPIO_OPS2_STATUS_DET
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS2_STATUS_DET, &u32GpioValue);
        *bEnable =  (u32GpioValue == XBH_BOARD_GPIO_OPS2_STATUS_DET_LEVEL) ? XBH_TRUE : XBH_FALSE;
        #endif
    }
    #endif
    XLOGD("getOpsOperationStatus(source=%d) enable=%d\n", enSource, *bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::setOpsRecovery(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (enSource == XBH_SOURCE_OPS1) 
    {
 #ifdef XBH_BOARD_GPIO_OPS1_RST
        if (bEnable)
        {
            s32Ret = this->XbhHi_610::setGpioOutputValue(XBH_BOARD_GPIO_OPS1_RST, !XBH_BOARD_GPIO_OPS1_RST_LEVEL);
        }
        else
        {
            s32Ret = this->XbhHi_610::setGpioOutputValue(XBH_BOARD_GPIO_OPS1_RST, XBH_BOARD_GPIO_OPS1_RST_LEVEL);
        }
#endif
    } else if (enSource == XBH_SOURCE_OPS2) {
 #ifdef XBH_BOARD_GPIO_OPS2_RST
        if (bEnable)
        {
            s32Ret = this->XbhHi_610::setGpioOutputValue(XBH_BOARD_GPIO_OPS2_RST, !XBH_BOARD_GPIO_OPS2_RST_LEVEL);
        }
        else
        {
            s32Ret = this->XbhHi_610::setGpioOutputValue(XBH_BOARD_GPIO_OPS2_RST, XBH_BOARD_GPIO_OPS2_RST_LEVEL);
        }
#endif
    }

    XLOGD("setOpsResetEnable enSource:%d bEnable=%d\n", enSource, bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::setOpsResetEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhXMH610_B::getOps1SignalStatus(XBH_S32 *status)
{
    return this->XbhHi_610::getSignalStatus(XBH_SOURCE_OPS1,status);
}

XBH_S32 XbhXMH610_B::getOps2SignalStatus(XBH_S32 *status)
{
    return this->XbhHi_610::getSignalStatus(XBH_SOURCE_OPS2,status);
}

XBH_S32 XbhXMH610_B::getFUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhHi_610::getSignalStatus(XBH_SOURCE_F_USBC1,status);
}

XBH_S32 XbhXMH610_B::getFHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhHi_610::getSignalStatus(XBH_SOURCE_F_HDMI1,status);
}

XBH_S32 XbhXMH610_B::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return  s32Ret;
}

//override
XBH_S32 XbhXMH610_B::getHallSensorExist(XBH_BOOL* bEnable)
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
XBH_S32 XbhXMH610_B::getHallSensorValue(XBH_S32* s32Value)
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


XBH_S32 XbhXMH610_B::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
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

XBH_S32 XbhXMH610_B::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
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

XBH_S32 XbhXMH610_B::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
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

XBH_S32 XbhXMH610_B::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
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
        case XBH_UPDATE_BOARD_HL817:
            s32Ret = XbhUsbHubManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_HL817, ver);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
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
        case XBH_UPDATE_BOARD_HL817:
            s32Ret = XbhUsbHubManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_HL817, filename, force);
            break;
        default:
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMH610_B::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
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
        case XBH_UPDATE_BOARD_HL817:
            s32Ret = XbhUsbHubManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_HL817, s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMH610_B::openEthWol(XBH_BOOL bEnable)
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

XBH_S32 XbhXMH610_B::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
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

XBH_S32 XbhXMH610_B::getEdidInvaild(XBH_HDMIRX_EDID_TYPE_E edidType, XBH_U8 *edidInvaild)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 i = 0;
    XBH_S32 begin_index = -1;
    XBH_S32 end_index = -1;
    Array<XBH_S32> aEdid20_1 = HiCfgFact::Get()->ParseIntArray("databasedef:HDMI_EDID_20_1");
    Array<XBH_S32> aEdid20_2 = HiCfgFact::Get()->ParseIntArray("databasedef:HDMI_EDID_20_2");
    Array<XBH_S32> aEdid14_1 = HiCfgFact::Get()->ParseIntArray("databasedef:HDMI_EDID_14_1");
    Array<XBH_S32> aEdid14_2 = HiCfgFact::Get()->ParseIntArray("databasedef:HDMI_EDID_14_2");
    XBH_U8 edid14Invaild = XBH_TRUE;
    XBH_U8 edid20Invaild = XBH_TRUE;

    XBH_U8 edidSum = 0;
    for ( i = 0; i < 127; ++i) {
        edidSum += aEdid20_1[i];
    }
    XBH_U8 expected_checksum = (0x100 - edidSum) & 0xFF;
    XBH_U8 actual_checksum = aEdid20_1[127];
    if(expected_checksum != actual_checksum)
    {
        XLOGD("mayinghui edid 2.0 block0 invaild");
        edid20Invaild = XBH_FALSE;
    }

    edidSum = 0;
    for ( i = 0; i < 127; ++i) {
        edidSum += aEdid20_2[i];
    }
    expected_checksum = (0x100 - edidSum) & 0xFF;
    actual_checksum = aEdid20_2[127];
    if(expected_checksum != actual_checksum)
    {
        XLOGD("mayinghui edid 2.0 block1 invaild");
        edid20Invaild = XBH_FALSE;
    }

    edidSum = 0;
    for ( i = 0; i < 127; ++i) {
        edidSum += aEdid14_1[i];
    }
    expected_checksum = (0x100 - edidSum) & 0xFF;
    actual_checksum = aEdid14_1[127];
    if(expected_checksum != actual_checksum)
    {
        XLOGD("mayinghui edid 1.4 block0 invaild");
        edid14Invaild = XBH_FALSE;
    }

    edidSum = 0;
    for ( i = 0; i < 127; ++i) {
        edidSum += aEdid14_2[i];
    }
    expected_checksum = (0x100 - edidSum) & 0xFF;
    actual_checksum = aEdid14_2[127];
    if(expected_checksum != actual_checksum)
    {
        XLOGD("mayinghui edid 1.4 block1 invaild");
        edid14Invaild = XBH_FALSE;
    }

    if(edidType == XBH_HDMIRX_EDID_TYPE_14)
    {
        *edidInvaild = edid14Invaild;
    }

    if(edidType ==  XBH_HDMIRX_EDID_TYPE_20)
    {
        *edidInvaild = edid20Invaild;
    }

    return s32Ret;
}

XBH_S32 XbhXMH610_B::getEdidModelName(std::string &pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 i = 0;
    XBH_S32 begin_index = -1;
    XBH_S32 end_index = -1;
    Array<XBH_S32> aEdid20_1 = HiCfgFact::Get()->ParseIntArray("databasedef:HDMI_EDID_20_1");
    for (i = 0; i < aEdid20_1.length(); i++){
        if (begin_index == -1)
        {
            if ((XBH_CHAR)aEdid20_1[i] == (XBH_CHAR)(0xfc)){
                if ((XBH_CHAR)aEdid20_1[i -1 ] == (XBH_CHAR)0x00 && (XBH_CHAR)aEdid20_1[i - 2 ] == (XBH_CHAR)0x00 && (XBH_CHAR)aEdid20_1[i + 1 ] == (XBH_CHAR)0x00)
                {
                    i += 2;
                    begin_index = i;
                }
            }
        }else
        {
            if ((XBH_CHAR)aEdid20_1[i] == (XBH_CHAR)0x0a)
            {
                end_index = i;
            }
        }
    }
    for (i = begin_index; i != end_index; i++)
    {
        pBuff.push_back(aEdid20_1[i]);
    }
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getEdidSn(std::string &pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 i = 0;
    char buff[5];
    memset(buff, 0x00, 5);
    Array<XBH_S32> aEdid20_1 = HiCfgFact::Get()->ParseIntArray("databasedef:HDMI_EDID_20_1");
    sprintf(buff, "%02x", (unsigned char)(aEdid20_1[11]));
    for (i = 11; i < 15; i++)
    {
        memset(buff, 0x00, 5);
        sprintf(buff, "%02x", (unsigned char)(aEdid20_1[i]));
        pBuff += buff;
    }

    return s32Ret;
}

XBH_S32 XbhXMH610_B::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    #if defined(XBH_BOARD_GPIO_OPS1_PB_DET) && defined(XBH_BOARD_GPIO_OPS2_PB_DET) \
            && defined(XBH_BOARD_GPIO_OPS_STB_PWR)
    if (s32Ret == XBH_SUCCESS && s8Buff[XBH_WAKEUP_SOURCE] == 1)
    {
        XBH_U32 ops1Pwr = 0;
        XBH_U32 ops1Status = 0;
        XBH_U32 ops2Pwr = 0;
        XBH_U32 ops2Status = 0;
        s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_OPS1_PB_DET, &ops1Status);
        s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_OPS2_PB_DET, &ops2Status);
        if (s32Ret == XBH_SUCCESS
                && (ops1Status == XBH_BOARD_GPIO_OPS1_PB_DET_LEVEL || ops2Status == XBH_BOARD_GPIO_OPS2_PB_DET_LEVEL))
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
    //DSP
    #ifdef XBH_BOARD_GPIO_DSP_USB_PWR
    setGpioOutputValue(XBH_BOARD_GPIO_DSP_USB_PWR, !XBH_BOARD_GPIO_DSP_USB_PWR_ON);
    #endif

    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    return s32Ret;
}

XBH_S32 XbhXMH610_B::getUsbLockMode(XBH_U32 *u32UsbType)
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
XBH_S32 XbhXMH610_B::setAvcEnable(XBH_BOOL bEnable)
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
XBH_S32 XbhXMH610_B::getAvcEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhAudioAmplifierManager::getInstance()->getAvcEnable(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::setLiftbracketOperate(XBH_S32 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhLiftbracketManager::getInstance()->setLiftbracketOperate(value);
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getLiftbracketExist(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //s32Ret = XbhLiftbracketManager::getInstance()->getLiftbracketExist(value);
    return  s32Ret;
}

XBH_S32 XbhXMH610_B::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //ret = XbhHi_610::setHdmiLowLatency(bEnable);
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getNTCValue(XBH_S32 s32Value, XBH_FLOAT* fValue)
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

XBH_S32 XbhXMH610_B::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("%s,%d enState:%d\n",__FUNCTION__,__LINE__,(int)enState);
    switch(enState)
    {
        case XBH_LED_LIGHT_OFF:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R,!XBH_BOARD_GPIO_F_LED_R_ON);
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G,!XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_RED:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R,XBH_BOARD_GPIO_F_LED_R_ON);
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G,!XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_GREEN:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R,!XBH_BOARD_GPIO_F_LED_R_ON);
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G,XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_YELLOW:
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R,XBH_BOARD_GPIO_F_LED_R_ON);
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G,XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        default:
            break;
    }
    return  s32Ret;
}


XBH_S32 XbhXMH610_B::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32RedGpioValue;
    XBH_U32 u32GreenGpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, &u32RedGpioValue);
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, &u32GreenGpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
        if((u32RedGpioValue == !XBH_BOARD_GPIO_F_LED_R_ON) && (u32GreenGpioValue == !XBH_BOARD_GPIO_F_LED_G_ON))
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_OFF;
        }
        else if ((u32RedGpioValue == XBH_BOARD_GPIO_F_LED_R_ON) && (u32GreenGpioValue == !XBH_BOARD_GPIO_F_LED_G_ON))
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_RED;
        }
        else if ((u32RedGpioValue == !XBH_BOARD_GPIO_F_LED_R_ON) && (u32GreenGpioValue == XBH_BOARD_GPIO_F_LED_G_ON))
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_GREEN;
        }
        else if ((u32RedGpioValue == XBH_BOARD_GPIO_F_LED_R_ON) && (u32GreenGpioValue == XBH_BOARD_GPIO_F_LED_G_ON))
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_YELLOW;
        }
    }
    return s32Ret;
}

XBH_S32 XbhXMH610_B::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
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

XBH_S32 XbhXMH610_B::getOPSBypassDataLen(XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_OPS_BYPASS_DATA_LEN, 0x01, 2, u8Data);

    return s32Ret;
}

XBH_S32 XbhXMH610_B::SendOPSBypassData(XBH_SOURCE_E type, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_MCU_I2CCMDDEF_E CMD;
    if (type == XBH_SOURCE_OPS1)
    {
        CMD = CMD_I2C_OPS1_BYPASS_DATA;
    }
    else if (type == XBH_SOURCE_OPS2)
    {
        CMD = CMD_I2C_OPS2_BYPASS_DATA;
    }
    else
    {
        XLOGE("Unsupported OPS channel.\n");
        return XBH_FAILURE;
    }

    if ((u8Data[0] == 0xCD) && (u8Data[1] == 0xCD))
    {
        XLOGE("Invalid data");
        return s32Ret;
    }
    s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD, 0x01, u8Len, u8Data);

    return s32Ret;
}

XBH_S32 XbhXMH610_B::OPSCRCBypassData(XBH_SOURCE_E type, XBH_BOOL RWflag, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    XBH_MCU_I2CCMDDEF_E CMD;
    if (type == XBH_SOURCE_OPS1)
    {
        CMD = CMD_I2C_OPS1_BYPASS_DATA;
    }
    else if (type == XBH_SOURCE_OPS2)
    {
        CMD = CMD_I2C_OPS2_BYPASS_DATA;
    }
    else
    {
        XLOGE("Unsupported OPS channel.\n");
        return XBH_FAILURE;
    }

    if (RWflag == XBH_READ_OPS_DATA)
        s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD, 0x01, u8Len, u8Data);
    else
        s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD, 0x01, u8Len, u8Data);
    return s32Ret;
}

XBH_U8 XbhXMH610_B::CalcCRC8(XBH_U8* Buffer, XBH_U8 BufferLen)
{
    XBH_U8* data = Buffer;
    XBH_U16 crc = 0;
    int i, j;
    for (j = BufferLen; j; j--, data++)
    {
        crc ^= (*data << 8);
        for (i = 8; i; i--)
        {
            if (crc & 0x8000)
                crc ^= (0x1070 << 3);
            crc <<= 1;
        }
    }
    return (XBH_U8)(crc >> 8);
}

XBH_U16 XbhXMH610_B::crc16(XBH_U16 crc, XBH_U8* buffer, XBH_U8 size)
{
    XBH_U8 i = 0;
    for (; i < size; i++)
    {
        crc = (crc >> 8) ^ CRC16_CCIT_TBL[(crc ^ buffer[i]) & 0xff];
    }
    return crc;
}

XBH_U16 XbhXMH610_B::Crc16CCIT(XBH_U8* data, XBH_U8 size)
{
    XBH_U16 crc = 0;
    crc = crc16(0, data, size);
    return crc;
}

XBH_S32 XbhXMH610_B::OPSByPassProcess()
{
    XBH_U8 header[2] = {0};
    XBH_U8 recv_data[256] = {0};
    XBH_SOURCE_E ops_type;
    static XBH_U8 stored_random_ops1[4] = {0};
    static XBH_U8 stored_random_ops2[4] = {0};
    static XBH_U8 last_valid_packet_ops1[8] = {0};      // 存放上一次收到的包
    static XBH_U8 last_valid_packet_ops2[8] = {0};
    static XBH_BOOL has_last_packet_ops1 = XBH_FALSE;   // 标记是否有有效历史包
    static XBH_BOOL has_last_packet_ops2 = XBH_FALSE;

    if (property_get_bool("persist.vendor.xbh.crc.enable", XBH_FALSE) == XBH_TRUE)
    {
        if (property_get_int32("vendor.xbh.ops1crc.result", XBH_CRC_NOT_PERFORMED) != XBH_CRC_CHECK_SUCCESS)
            setHdmiHPD(H610_HDMIRX_1_PORT_ID, XBH_LEVEL_LOW);   //OPS1
        if (property_get_int32("vendor.xbh.ops2crc.result", XBH_CRC_NOT_PERFORMED) != XBH_CRC_CHECK_SUCCESS)
            setHdmiHPD(H610_HDMIRX_2_PORT_ID, XBH_LEVEL_LOW);   //OPS2
    }
    else    //  如果不开启CRC，默认验证结果为成功
    {
        CrcResult = XBH_CRC_CHECK_SUCCESS;
        XBH_CHAR result_status[PROPERTY_VALUE_MAX] = {0};
        sprintf(result_status, "%d", CrcResult);

        property_set("vendor.xbh.ops1crc.result", result_status);
        property_set("vendor.xbh.ops2crc.result", result_status);
    }

    getOPSBypassDataLen(header);

    // 1. 获取头部信息
    XBH_U8 data_len = header[0];

    if ((data_len <= 0) || (data_len > 256))
    {
        return XBH_FAILURE;
    }

    if (header[1] == XBH_BYPASS_TYPE_OPS1)
    {
        ops_type = XBH_SOURCE_OPS1;
    }
    else if (header[1] == XBH_BYPASS_TYPE_OPS2)
    {
        ops_type = XBH_SOURCE_OPS2;
    }
    else
    {
        return XBH_FAILURE;
    }

    XLOGD("get data: header[0] = %d, header[1] = %d", header[0], header[1]);
    
    // 2. 读取数据
    OPSCRCBypassData(ops_type, XBH_READ_OPS_DATA, data_len, recv_data);

    // 透传数据到中间件
    //对齐中间件数据
    XBH_U8 type_data[8] = {
        static_cast<XBH_U8>((ops_type >> 0) & 0xFF),
        static_cast<XBH_U8>((ops_type >> 8) & 0xFF),
        static_cast<XBH_U8>((ops_type >> 16) & 0xFF),
        static_cast<XBH_U8>((ops_type >> 24) & 0xFF),
        static_cast<XBH_U8>((data_len >> 0) & 0xFF),
        static_cast<XBH_U8>((data_len >> 8) & 0xFF),
        static_cast<XBH_U8>((data_len >> 16) & 0xFF),
        static_cast<XBH_U8>((data_len >> 24) & 0xFF)
    };
    XBH_U8 pass_data[264] = {0};            //size = 256 + 8:因为对齐中间件数据需要插入8个字节，此处防止内存泄漏
    memmove(pass_data + 8, recv_data, data_len);
    memcpy(pass_data, type_data, 8);
    MsgPublisher().PostMsg(XBH_OPS_BYPASS, pass_data, data_len + 8);
    XLOGD("PostMsg to middleWare\n");
    
    // 3. 验证包头 (0xAB 0xAB)
    if (recv_data[0] == 0xAB && recv_data[1] == 0xAB)   //CRC
    {
        // 4. CRC长度
        if (data_len == 16)         //两个包连续发送
        {
            // 5. 检查数据包重复部分：无间隔发送2次
            if (memcmp(recv_data, recv_data + 8, 8) != 0)
            {
                XLOGE("recv_data is error!\n");
                return XBH_FAILURE; // 前后8字节不匹配
            }
        }
        else if (data_len == 8)     //两个包分开发送
        {
            XBH_U8 *last_valid_packet = (ops_type == XBH_SOURCE_OPS1) 
                                        ? last_valid_packet_ops1 
                                        : last_valid_packet_ops2;

            XBH_BOOL *has_last_packet = (ops_type == XBH_SOURCE_OPS1) 
                                        ? (&has_last_packet_ops1) 
                                        : (&has_last_packet_ops1);

            if ((*has_last_packet) && memcmp(recv_data, last_valid_packet, 8) == 0)
            {
                XLOGD("repeat packet!");
                *has_last_packet = XBH_FALSE;
                return XBH_SUCCESS; // 重复包直接返回成功
            }

            memcpy(last_valid_packet, recv_data, 8);
            *has_last_packet = XBH_TRUE;
        }
        else
        {
            return XBH_FAILURE; // 长度不符合要求
        }
        
        // 6. 计算并验证CRC8（前7字节）
        XBH_U8 calc_crc = CalcCRC8(recv_data, 7);
        if (calc_crc != recv_data[7])
        {
            XLOGE("calc_crc is %d, recv_data[7] is %d, different!\n", calc_crc, recv_data[7]);
            return XBH_FAILURE; // CRC校验失败
        }
        
        // 7. 提取事件码
        XBH_U8 event_code = recv_data[2];
        
        // 状态机处理
        switch (event_code)
        {
            case XBH_WAIT_FOR_AUTH:
            {
                // 确定目标随机数存储位置
                XBH_U8 *stored_random = (ops_type == XBH_SOURCE_OPS1) 
                                        ? stored_random_ops1 
                                        : stored_random_ops2;
                // 生成4字节随机数
                for (int i = 0; i < 4; i++)
                {
                    stored_random[i] = rand() & 0xFF;
                }
                
                // 构建响应包
                XBH_U8 send_data[16] =
                {
                    0xCD, 0xCD,                 // 包头
                    XBH_WAIT_FOR_AUTH,          // 事件码
                    stored_random[0],           // 随机数
                    stored_random[1],
                    stored_random[2],
                    stored_random[3],
                    0,                  // 占位CRC（稍后计算）
                    // 后8字节重复前8字节
                };
                
                // 计算前7字节CRC
                send_data[7] = CalcCRC8(send_data, 7);
                // 复制前8字节到后8字节
                memcpy(send_data + 8, send_data, 8);
                
                // 发送数据
                OPSCRCBypassData(ops_type, XBH_WRITE_OPS_DATA, data_len, send_data);
                
                break;
            }
                
            case XBH_WAIT_FOR_VERIFY:
            {
                // 确定目标随机数存储位置
                XBH_U8 *stored_random = (ops_type == XBH_SOURCE_OPS1) 
                                        ? stored_random_ops1 
                                        : stored_random_ops2;
                // 构造验证数据：随机数 + 收到的第6、7字节 + 0xAA, 0xBB
                XBH_U8 verify_data[8] =
                {
                    stored_random[0],
                    stored_random[1],
                    stored_random[2],
                    stored_random[3],
                    recv_data[5],  // 数据位第3字节（总索引5）
                    recv_data[6],  // 数据位第4字节（总索引6）
                    0xAA,
                    0xBB
                };
                
                // 计算CRC16
                XBH_U16 crc_result = Crc16CCIT(verify_data, 8);
                XBH_U8 crc_high = (crc_result >> 8) & 0xFF;
                XBH_U8 crc_low = crc_result & 0xFF;

                XLOGD("recv_data[3] = %d, crc_low = %d, recv_data[4] = %d, crc_high = %d", recv_data[3], crc_low, recv_data[4], crc_high);
                
                // 验证收到的CRC（数据位第1、2字节）
                if (recv_data[3] == crc_low && recv_data[4] == crc_high)    //验证成功
                {
                    CrcResult = XBH_CRC_CHECK_SUCCESS;
                    // 构建成功响应
                    XBH_U8 send_data[16] =
                    {
                        0xCD, 0xCD,             // 包头
                        XBH_WAIT_FOR_VERIFY,    // 事件码
                        0, 0, 0, 0,             // 数据位（全0）
                        0,                      // 占位CRC
                    };
                    
                    // 计算CRC并复制数据
                    send_data[7] = CalcCRC8(send_data, 7);
                    memcpy(send_data + 8, send_data, 8);
                    
                    // 发送响应
                    OPSCRCBypassData(ops_type, XBH_WRITE_OPS_DATA, data_len, send_data);

                    // 校验成功，打开出图
                    if (ops_type == XBH_SOURCE_OPS1)
                        setHdmiHPD(H610_HDMIRX_1_PORT_ID, XBH_LEVEL_HIGH);
                    else
                        setHdmiHPD(H610_HDMIRX_2_PORT_ID, XBH_LEVEL_HIGH);
                }
                else        //验证失败
                {
                    CrcResult = XBH_CRC_CHECK_FAILED;
                    // 构建验证失败响应
                    XBH_U8 send_data[16] =
                    {
                        0xCD, 0xCD,             // 包头
                        XBH_WAIT_FOR_VERIFY,    // 事件码
                        1, 1, 1, 1,             // 数据位（全1）
                        0,                      // 占位CRC
                    };
                    
                    // 计算CRC并复制数据
                    send_data[7] = CalcCRC8(send_data, 7);
                    memcpy(send_data + 8, send_data, 8);
                    
                    // 发送响应
                    OPSCRCBypassData(ops_type, XBH_WRITE_OPS_DATA, data_len, send_data);
                }

                XBH_CHAR result_status[PROPERTY_VALUE_MAX] = {0};
                sprintf(result_status, "%d", CrcResult);

                if (ops_type == XBH_SOURCE_OPS1)
                    property_set("vendor.xbh.ops1crc.result", result_status);
                else
                    property_set("vendor.xbh.ops2crc.result", result_status);

                XLOGD("OPS%d : CrcResult = %d", ops_type, CrcResult);

                break;
            }

            case XBH_QUERY_SCREEN_INFO:
            {
                int module_type = property_get_int32("persist.vendor.xbh.product.model", XBH_BOE);

                // 构建大屏响应
                XBH_U8 send_data[16] =
                {
                    0xCD, 0xCD,                 // 包头
                    XBH_QUERY_SCREEN_INFO,      // 事件码
                    static_cast<XBH_U8>((module_type >> 8) & 0xFF), // 厂商型号
                    static_cast<XBH_U8>(module_type & 0xFF),
                    0,0,                        // 数据位（全0）
                    0,                          // 占位CRC
                };

                // 计算CRC并复制数据
                send_data[7] = CalcCRC8(send_data, 7);
                memcpy(send_data + 8, send_data, 8);
                // 发送响应
                OPSCRCBypassData(ops_type, XBH_WRITE_OPS_DATA, data_len, send_data);

                break;
            }
                
            default:
                break;
        }
    }
    
    return XBH_SUCCESS;
}

//CRC超时检测
XBH_S32 XbhXMH610_B::CRCTimeoutCheck()
{
    XBH_BOOL bIsDet = XBH_FALSE;
    XBH_BOOL bEnable = XBH_FALSE;

    static XBH_U8 ops1_counter = 0;
    static XBH_U8 ops2_counter = 0;

    if (property_get_int32("vendor.xbh.ops1crc.result", XBH_CRC_NOT_PERFORMED) == XBH_CRC_NOT_PERFORMED)    //未校验
    {
        getOpsDetStatus(&bIsDet, XBH_SOURCE_OPS1);
        if (bIsDet)
        {
            getOpsPowerStatus(&bEnable, XBH_SOURCE_OPS1);
            if (bEnable)
            {
                if (ops1_counter >= 0) // 正在计数或准备计数
                    ops1_counter++;

                if (ops1_counter > 20)  // 达到5秒（21次调用）
                {
                    XBH_CHAR result_status[PROPERTY_VALUE_MAX] = {0};

                    CrcResult = XBH_CRC_CHECK_FAILED;

                    sprintf(result_status, "%d", CrcResult);
                    XLOGD("OPS1 CRC Timeout");
                    property_set("vendor.xbh.ops1crc.result", result_status);     //设置成非法
                    ops1_counter = 0;
                }
            }
            else    //如果掉电了，及时清空计数
            {
                ops1_counter = 0;
            }
        }
        else   //如果拔出了，及时清空计数
        {
            ops1_counter = 0;
        }
    }

    if (property_get_int32("vendor.xbh.ops2crc.result", XBH_CRC_NOT_PERFORMED) == XBH_CRC_NOT_PERFORMED)    //未校验
    {
        getOpsDetStatus(&bIsDet, XBH_SOURCE_OPS2);
        if (bIsDet)
        {
            getOpsPowerStatus(&bEnable, XBH_SOURCE_OPS2);
            if (bEnable)
            {
                if (ops2_counter >= 0) // 正在计数或准备计数
                    ops2_counter++;

                if (ops2_counter > 20)  // 达到5秒（21次调用）
                {
                    XBH_CHAR result_status[PROPERTY_VALUE_MAX] = {0};

                    CrcResult = XBH_CRC_CHECK_FAILED;

                    sprintf(result_status, "%d", CrcResult);
                    XLOGD("OPS2 CRC Timeout");
                    property_set("vendor.xbh.ops2crc.result", result_status);     //设置成非法
                    ops2_counter = 0;
                }
            }
            else    //如果掉电了，及时清空计数
            {
                ops2_counter = 0;
            }
        }
        else   //如果拔出了，及时清空计数
        {
            ops2_counter = 0;
        }
    }
    return XBH_SUCCESS;
}

/**
 * 减小因ops不响应导致开机失败可能性，当 OPS_PS_ON 发送 OPS
 * 开机时序后,必须监测OPS_PWR_STATUS,若OPS无响应,则将OPS_PS_ON
 * 脉冲宽度从700ms增加到900ms,再增加到 1100ms,即重试2次
 * @param[in] enSource 指定的信源通道
 * @return 0:success,-1:failure
*/
XBH_S32 XbhXMH610_B::setOpsPowerCtrlWithDurationMs(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32OpspowerTimer = M_PS_ON_BASE_DELAY_TIME;
    XBH_S32 staTimes = 3;
    XBH_BOOL bIsDet = XBH_FALSE;
    static XBH_BOOL PrePowerState[XBH_SOURCE_BUTT] = {0};
    while(staTimes--)
    {
        XLOGD("setOpsPowerCtrlWithDurationMs(staTimes=%d)\n", staTimes);
        XBH_BOOL bEnable;
        if (enSource == XBH_SOURCE_OPS1)
        {
            #ifdef XBH_BOARD_GPIO_OPS1_PS_ON
            XLOGD("setOpsPowerCtrlWithDurationMs(XBH_BOARD_GPIO_OPS1_PS_ON)\n");
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS1_PS_ON, XBH_BOARD_GPIO_OPS1_PS_ON_LEVEL);
            if(s32Ret == XBH_FAILURE)
            {
                break;
            }

            usleep(u32OpspowerTimer * 1000);

            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS1_PS_ON, !XBH_BOARD_GPIO_OPS1_PS_ON_LEVEL);
            if(s32Ret == XBH_FAILURE)
            {
                break;
            }
            #endif
        }
        else if (enSource == XBH_SOURCE_OPS2)
        {
            #ifdef XBH_BOARD_GPIO_OPS2_PS_ON
            XLOGD("setOpsPowerCtrlWithDurationMs(XBH_BOARD_GPIO_OPS2_PS_ON)\n");
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS2_PS_ON, XBH_BOARD_GPIO_OPS2_PS_ON_LEVEL);
            if(s32Ret == XBH_FAILURE)
            {
                break;
            }

            usleep(u32OpspowerTimer * 1000);

            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS2_PS_ON, !XBH_BOARD_GPIO_OPS2_PS_ON_LEVEL);
            if(s32Ret == XBH_FAILURE)
            {
                break;
            }
            #endif
        }

        if (u32OpspowerTimer >= (M_PS_ON_BASE_DELAY_TIME + (M_PS_ON_DELAY_TIME_STEP * 2)))
        {
            break;
        }

        usleep(M_PWR_STA_DECT_TIMES*1000);
        getOpsPowerStatus(&bEnable, enSource);

        if (bEnable != PrePowerState[enSource])
        {
            PrePowerState[enSource] = bEnable;
            break;
        }
        else
        {
            u32OpspowerTimer += M_PS_ON_DELAY_TIME_STEP;
        }
    }
    XLOGD("setOpsPowerCtrlWithDurationMs(source=%d)\n", enSource);
    return  s32Ret;
}

//override
XBH_S32 XbhXMH610_B::setReferRTCInfo(const XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->setReferRTCInfo(pBuff);
    return s32Ret;
}
//override
XBH_S32 XbhXMH610_B::getReferRTCInfo(XBH_CHAR* pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getReferRTCInfo(pBuff);
    return s32Ret;
}

XBH_S32 XbhXMH610_B::getUSBCForwardReverseInsertionDet(XBH_SOURCE_E idx, XBH_S32 *u32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch (idx)
    {
        case XBH_SOURCE_F_USBC1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUSBCForwardReverseInsertionDet(XBH_UPDATE_FRONT_GSV2712, u32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
            break;
    }
    XLOGD("%s,%d,idx:%d,u32Value:%d\n",__FUNCTION__,__LINE__,idx,*u32Value);
    return s32Ret;
}

//override
XBH_S32 XbhXMH610_B::setSecurityHvbKey()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    uapi_otp_asymmetric_key_type key_type = UAPI_OTP_ASYMMETRIC_KEY_ECC;
    td_bool status = TD_FALSE;
    td_bool stat = TD_FALSE;
    td_u8 key[0x40] = {
        0x3f, 0xb5, 0xca, 0xc8, 0x2b, 0xc5, 0x13, 0xd7,
        0xc4, 0xbe, 0xe9, 0x0e, 0x87, 0x1b, 0xad, 0x3c,
        0x10, 0x53, 0x7c, 0xfe, 0x0c, 0xf6, 0xea, 0xdb,
        0x76, 0x38, 0x6c, 0x5c, 0x76, 0x53, 0x6a, 0x93,

        0x7d, 0xb4, 0xf4, 0xeb, 0xee, 0x46, 0x53, 0xc1,
        0xf8, 0x49, 0x8a, 0xba, 0xc8, 0x47, 0x9f, 0x12,
        0xc5, 0x2a, 0x8f, 0xee, 0x09, 0xc4, 0x46, 0xc1,
        0x99, 0xb2, 0xc8, 0xd0, 0x96, 0x2b, 0x40, 0xc3,
    };


    s32Ret = uapi_otp_get_asymmetric_key_lock_stat(key_type, &status);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("%s,%d,Error! uapi_otp_get_asymmetric_key_lock_stat s32Ret = %d\n",__FUNCTION__,__LINE__,s32Ret);
        return s32Ret;
    }
    if (status == TD_FALSE)
    {
        s32Ret = uapi_otp_set_asymmetric_key(key_type, key, sizeof(key));
        XLOGD("%s,%d,uapi_otp_set_asymmetric_key s32Ret = %d\n",__FUNCTION__,__LINE__,s32Ret);
        if (s32Ret != XBH_SUCCESS)
        {
            return s32Ret;
        }
    }
    else
    {
        XLOGD("%s,%d, HVB key is existed. \n",__FUNCTION__,__LINE__);
    }

    s32Ret = uapi_otp_get_scs_stat(&stat);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("%s,%d,Error! uapi_otp_get_scs_stat s32Ret = %d\n",__FUNCTION__,__LINE__,s32Ret);
        return s32Ret;
    }
    if (stat == TD_FALSE)
    {
        s32Ret = uapi_otp_enable_scs();
        XLOGD("%s,%d,uapi_otp_enable_scs s32Ret = %d\n",__FUNCTION__,__LINE__,s32Ret);
        if (s32Ret != XBH_SUCCESS)
        {
            return s32Ret;
        }
    }
    else
    {
        XLOGD("%s,%d, Chip is already security.\n",__FUNCTION__,__LINE__);
    }

    XLOGD("%s,%d,setSecurityHvbKey s32Ret = %d\n",__FUNCTION__,__LINE__,s32Ret);
    return  s32Ret;
}

XbhXMH610_B *XbhXMH610_B::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMH610_B();
        }
    }
    return mInstance;
}

/**
 * 初始化project_id配置
 */
XBH_S32 XbhXMH610_B::initProjectIdConfig()
{
    //每次开机都需要更新edid的数据
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);

    return XBH_SUCCESS;
}

/**
 * 设置type-c复位VBUS
*/
XBH_S32 XbhXMH610_B::setTypecReset(XBH_SOURCE_E src)
{
    if ((src != XBH_SOURCE_F_USBC1)) //当前信源不在前置type-c信源，只有type-c不作为信源才需要设置复位。
    {
        if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT3_U8_BOARD) //根据前置板型号确认使用的基石芯片。
        {
            XbhHdmiSwitchManager::getInstance()->setTypecReset(XBH_UPDATE_FRONT_GSV2712);
        }
    }
    
    return XBH_SUCCESS;
}


