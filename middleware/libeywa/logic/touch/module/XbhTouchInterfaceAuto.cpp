#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhTouchInterfaceAuto"

#include "XbhLog.h"
#include "cutils/properties.h"
#include <unistd.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/hidraw.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "XbhTouchInterfaceAuto.h"

// Touch Device
#include "XbhTouchHuaxinUsb_Common.h"
#include "XbhTouchHuaxinSdk_Common.h"
#include "XbhTouchIsolutionUsb_E.h"
#include "XbhTouchKtcSdk_Common.h"
#include "XbhTouchHuaxinUsb_SQ.h"
#include "XbhTouchTimeLinkSdk.h"
#include "XbhTouchCommonUsb.h"
#include "XbhTouchZhongYuanUsb.h"
#include "XbhTouchCommonUart.h"
#include "XbhTouchZyUart.h"
#include "XbhTouchYclUart.h"
#include "XbhTouchIsolutionUsb_E_HHT.h"
#include "XbhTouchLangoUsb_K2.h"
#include "XbhTouchIsolutionUart_G.h"
#include "XbhTouchIsolutionXSdk.h"

XbhTouchInterfaceAuto*               XbhTouchInterfaceAuto::mInstance = XBH_NULL;
XbhMutex                             XbhTouchInterfaceAuto::mLockObject;

XBH_U32 matchID = -1;
int MaxDeviceNum = 0;
deviceInfolist mDevices[256] = {{0,0}};

/**
* Match1 为0时，表示需要获取第一路USB的VID PID，并跟表中的USB1 VID PID进行匹配
* Match2 为0时，表示需要获取第二路USB的VID PID，并跟表中的USB2 VID PID进行匹配
* matchID = Match1 + Match2，只有matchID = 2才表示匹配上，例如SKG-T52框，Match1 = 0，Match2=1，表示需要匹配第一路USB的VID,PID，如果匹配上Match1 = 1
* 此时Match1 + Match2 = 2，表示触摸框匹配上，matchID就跟autoMatchTouchList中的第0组数据下标，在getTouchInterface里面就可以找到是XbhTouchKtcSdk_Common
* 的触摸框。
* 注意：getTouchInterface里面case的值一定要跟autoMatchTouchList数组的下标对应，否则就会匹配错误
*/
autoMatchTouchNode autoMatchTouchList[]
{
/*  USB1 VID        USB1 PID        USB2 VID       USB2 PID         Touch Ctrl              Match1      Match2             ko                  NAME      */
    {0x314c,        0x4c52,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          1,         "skg_t52_touch.ko",       "SKG-T52"},          //0
    {0x314c,        0x4c60,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       1,          0,         "skg_t80_touch.ko",       "SKG-T80"},          //1
    {0x2fe9,        0x0008,         0x2fe9,        0x4108,          XBH_TP_IF_USB_CTRL,       0,          1,         "HuaXin_HE.ko",           "HE-SS"},            //2
    {0x2fe9,        0x0425,         0x2fe9,        0xa425,          XBH_TP_IF_USB_CTRL,       0,          1,         "HuaXin_HE.ko",           "HE-SS-SKY90"},      //3
    {0x2fe9,        0x0325,         0x2fe9,        0xa325,          XBH_TP_IF_USB_CTRL,       0,          1,         "HuaXin_HE.ko",           "HE-SS-SKY86"},      //4
    {0x2fe9,        0x0225,         0x2fe9,        0xa225,          XBH_TP_IF_USB_CTRL,       0,          1,         "HuaXin_HE.ko",           "HE-SS-SKY75"},      //5
    {0x2fe9,        0x0125,         0x2fe9,        0xa125,          XBH_TP_IF_USB_CTRL,       0,          1,         "HuaXin_HE.ko",           "HE-SS-SKY65"},      //6
    {0x2757,        0x0117,         0xffff,        0xffff,          XBH_TP_IF_USB_CTRL,       0,          1,         "IstTouchE.ko",           "IstTouch-E-HHT"},   //7
    {0xffff,        0xffff,         0x28e1,        0xb006,          XBH_TP_IF_UART_CTRL,      1,          0,         "",                       "IstTouch-F"},       //8
    {0xffff,        0xffff,         0x28e1,        0xb020,          XBH_TP_IF_UART_CTRL,      1,          0,         "",                       "IstTouch-G"},       //9
    {0x2fe9,        0x000a,         0x2fe9,        0x400a,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "HE-SQ"},            //10
    {0x2fe9,        0x0009,         0x2fe9,        0x5009,          XBH_TP_IF_SDK_CTRL,       0,          1,         "HuaXin_SF.ko",           "HX-SF-92"},         //11
    {0x2575,        0x0103,         0x2575,        0x6738,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "KASA-BOE"},         //12//KASA电容
    {0x1a86,        0x55d3,         0x2309,        0x1006,          XBH_TP_IF_SDK_CTRL,       0,          1,         "ch343.ko",               "TIMELINK"},         //13
    {0x2fe9,        0x0209,         0x2fe9,        0x4209,          XBH_TP_IF_SDK_CTRL,       0,          1,         "HuaXin_SF.ko",           "HX-SF-65"},         //14
    {0x3632,        0x0204,         0x3632,        0x0203,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "ISTARIC"},          //15 芯璨电容
    {0x314c,        0x4c90,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          1,         "skg_t90_touch.ko",       "SKG-T90"},          //16
    {0x1FF7,        0x0F32,         0x1FF7,        0x0F33,          XBH_TP_IF_USB_CTRL,       0,          0,         "ZYDeviceR.ko",           "ZYR20"},            //17
    {0xffff,        0xffff,         0x08d3,        0x1000,          XBH_TP_IF_UART_CTRL,      1,          0,         "",                       "YCL-Q9"},           //18
    {0x314c,        0x4c60,         0x0b8c,        0x320b,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t85_touch.ko",       "SKG-T85"},          //19
    {0xffff,        0xffff,         0x1ff7,        0x0f32,          XBH_TP_IF_UART_CTRL,      1,          0,         "",                       "ZYE30"},            //20
    {0xffff,        0xffff,         0x2e3d,        0x5745,          XBH_TP_IF_USB_CTRL,       1,          0,         "",                       "k2_touch_legacy"},  //21
    {0x314c,        0x4c60,         0x0b8c,        0x320f,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t85_touch.ko",       "SKG-T85"},          //22
    {0x2fe9,        0x0236,         0x2fe9,        0xa236,          XBH_TP_IF_SDK_CTRL,       0,          1,         "HuaXin_SF.ko",           "HX-SF-75"},         //23
    {0x2fe9,        0x5008,         0x2fe9,        0x0208,          XBH_TP_IF_SDK_CTRL,       0,          1,         "HuaXin_SF.ko",           "HX-SF-65"},         //24
    {0x2fe9,        0x0008,         0x2fe9,        0x4008,          XBH_TP_IF_SDK_CTRL,       0,          0,         "HuaXin_SF.ko",           "HX-SF-65"},         //25
    {0xffff,        0xffff,         0x2575,        0x6840,          XBH_TP_IF_UART_CTRL,      1,          0,         "",                       "KASA-IYM"},         //26
    {0x314c,        0x4c60,         0x0b8c,        0x3211,          XBH_TP_IF_SDK_CTRL,       0,          0,         "skg_t89_touch.ko",       "SKG-T89"},          //27
    {0x2575,        0x7400,         0x2575,        0x6738,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "KASA-BOE"},         //28
    {0xffff,        0xffff,         0x1101,        0x0060,          XBH_TP_IF_UART_CTRL,      1,          0,         "",                       "JIARUITUO"},        //29
    {0x2fe9,        0x0009,         0x2fe9,        0x4009,          XBH_TP_IF_SDK_CTRL,       0,          0,         "HuaXin_SF.ko",           "HX-SF-75"},         //30
    {0x28e1,        0xb100,         0x28e1,        0xb006,          XBH_TP_IF_USB_CTRL,       0,          1,         "IstTouchE.ko",           "IstTouch-E"},       //31
    {0x314c,        0x4c60,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       0,          1,         "skg_tA0_touch.ko",       "SKG-TA0"},          //32
    {0x365f,        0x4003,         0x365f,        0x4004,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "LANGO-K2"},         //33
    {0xffff,        0xffff,         0x314c,        0x0100,          XBH_TP_IF_SDK_CTRL,       1,          0,         "skg_t89_touch.ko",       "SKG-T89"},          //34
    {0x2309,        0x0120,         0x2309,        0xa120,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "TIMELINK_H_65"},    //35
    {0x2309,        0x0220,         0x2309,        0xa220,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "TIMELINK_H_75"},    //36
    {0x2309,        0x0320,         0x2309,        0xa320,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "TIMELINK_H_86"},    //37
    {0x2fe9,        0x0128,         0x2fe9,        0xa128,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "HE-SQ-SKY65"},      //38
    {0x2fe9,        0x0228,         0x2fe9,        0xa228,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "HE-SQ-SKY75"},      //39
    {0x2fe9,        0x0328,         0x2fe9,        0xa328,          XBH_TP_IF_USB_CTRL,       0,          1,         "",                       "HE-SQ-SKY86"},      //40
    {0x28e1,        0xb101,         0x28e1,        0xb006,          XBH_TP_IF_SDK_CTRL,       0,          1,         "",                       "IstTouch-X-610a"},  //41
};

/*
* XbhOldTouchCommonUart和XbhTouchCommonUart 的差别是setSourceTouchState接口的实现不同，
* XbhTouchCommonUart的实现是按照
* 协议在线文档：
* https://doc.weixin.qq.com/doc/w3_ABUA9wb6AIgWbOLrk0GSR00JMjKLO?scode=ABUAPgfAABA79KnY92AAkA9wb6AIg&version=4.1.32.6005&platform=win
* 来实现的
*/
XBH_S32 XbhTouchInterfaceAuto::getTouchInterface(void)
{
    switch (matchID)
    {
        case 0:
        case 1:
        case 16:
        case 19:
        case 22:
        case 27:
        case 32:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchKtcSdk_Common::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchHuaxinUsb_Common::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 11:
        case 14:
        case 23:
        case 24:
        case 25:
        case 30:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchHuaxinSdk_Common::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 7:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchIsolutionUsb_E_HHT::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 10:
        case 21:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchHuaxinUsb_SQ::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 13:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchTimeLinkSdk::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        //电容触摸屏的USB转换驱动，需要重新适配
        case 12:
        case 15:
        case 28:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchCommonUsb::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name, \
                autoMatchTouchList[matchID].usb1_vid,autoMatchTouchList[matchID].usb1_pid);
            break;
        case 17:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchZhongYuanUsb::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 18:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchYclUart::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 20:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchZyUart::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 8:
        case 9:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchIsolutionUart_G::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 26:
        case 29:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchCommonUart::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 31:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchIsolutionUsb_E::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 33:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchLangoUsb_K2::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
        case 41:
            m_pXbhTouchInterfaceAuto = (XbhTouchInterface *)XbhTouchIsolutionXSdk::getInstance(autoMatchTouchList[matchID].ko, autoMatchTouchList[matchID].name);
            break;
         default:
             XLOGE("unKnow Touch!!!");
    }
    if(autoMatchTouchList[matchID].ctrl == XBH_TP_IF_E::XBH_TP_IF_SDK_CTRL)
    {
        property_set("vendor.xbh.touch.type.sdk", "1");
    }
    else
    {
        property_set("vendor.xbh.touch.type.sdk", "0");
    }
    return XBH_SUCCESS;
}

/**
 * 设置信号源的触摸开关。
 * onoff 开关状态
 */
XBH_S32 XbhTouchInterfaceAuto::setSourceTouchState(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->setSourceTouchState(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("setSourceTouchState enable = %d , ret = %d", enable, ret);
    return ret;
}

/**
 * 获取信号源的触摸开关。
 * boolean 开关状态
 */
XBH_S32 XbhTouchInterfaceAuto::getSourceTouchState(XBH_BOOL *enable)
{
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        m_pXbhTouchInterfaceAuto->getSourceTouchState(enable);
        XLOGD("getSourceTouchState enable = %d ", *enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL ");
        *enable = 0;
    }
    return XBH_SUCCESS;
}

/**
 * 设置不透传区域
 * id: 当前区域的id
 * x,y,w,h 当前区域的矩形参数
 * is4k: 主要是针对有2K 4KUI混合的场景
 */
XBH_S32 XbhTouchInterfaceAuto::setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->setNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("setNonThroughTouchRegion id = %d resolution = %d x = %d y = %d w = %d h = %d , ret = %d",id, resolution, x, y, w, h, ret);
    return ret;
}

/**
 * 移除不透传区域
 * id: 当前区域的id
 */
XBH_S32 XbhTouchInterfaceAuto::deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->deleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("deleteNonThroughTouchRegion id = %d , ret = %d", id, ret);
    return ret;
}

/**
 * 设置信源的触摸区域，可实现屏幕下移功能的windows触摸下移
 */
XBH_S32 XbhTouchInterfaceAuto::setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->setTouchScaleRect(resolution, x, y, w, h);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("setTouchScaleRect resolution = %d x = %d y = %d w = %d h = %d , ret = %d", resolution, x, y, w, h, ret);
    return ret;
}

/**
 * 发送按键给触摸框，然后触摸框发给OPS
 */
XBH_S32 XbhTouchInterfaceAuto::sendKeyToTp(XBH_U32 keyCode)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->sendKeyToTp(keyCode);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("sendKeyToTp keyCode = %d ", keyCode);
    return ret;
}

/**
 * 发送数据给触摸框
 */
XBH_S32 XbhTouchInterfaceAuto::sendDataToTp(XBH_U32 length, XBH_U32 *touchData)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->sendDataToTp(length, touchData);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("sendDataToTp length = %d ", length);
    return ret;
}

/**
 * 设置信源的触摸区域，可实现屏幕下移功能的windows触摸下移
 */
XBH_S32 XbhTouchInterfaceAuto::setPalmRejection(XBH_BOOL enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->setPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("setPalmRejection enable = %d ", enable);
    return ret;
}

/**
* 设置触摸框的防误触功能开关
*/
XBH_S32 XbhTouchInterfaceAuto::getPalmRejection(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->getPalmRejection(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("getPalmRejection enable = %d ", *enable);
    return ret;
}

/**
* 获取触摸框的连接状态
*/
XBH_S32 XbhTouchInterfaceAuto::getTouchPanelConnect(XBH_BOOL* enable)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->getTouchPanelConnect(enable);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("getTouchPanelConnect enable = %d ", *enable);
    return ret;
}

/**
* 获取触摸框的名称
*/
XBH_S32 XbhTouchInterfaceAuto::getTouchPanelName(XBH_CHAR *name)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->getTouchPanelName(name);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    XLOGD("getTouchPanelName name = %s ", name);
    return ret;
}

/**
* 获取触摸框驱动版本
*/
XBH_S32 XbhTouchInterfaceAuto::getTpDriverVersion(XBH_CHAR *version)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->getTpDriverVersion(version);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    return ret;
}

/**
* 获取触摸框固件版本
*/
XBH_S32 XbhTouchInterfaceAuto::getTpFirmwareVersion(XBH_CHAR *version)
{
    XBH_S32 ret = XBH_FAILURE;
    if(m_pXbhTouchInterfaceAuto != NULL)
    {
        ret = m_pXbhTouchInterfaceAuto->getTpFirmwareVersion(version);
    }
    else
    {
        XLOGE("m_pXbhTouchInterfaceAuto is NULL");
    }
    return ret;
}

XBH_S32 XbhTouchInterfaceAuto::getAllUsbDevices()
{
    DIR *dir;
    struct dirent *next;
    FILE *fp = NULL;
    XBH_U32 count = 0;

    dir = opendir(USB_DIR);
    if (!dir)
    {
        return XBH_FAILURE;
    }

    while ((next = readdir(dir)) != NULL)
    {
        char line[256];
        char uevent_file[256] = {0};
        sprintf(uevent_file, "%s/%s/uevent", USB_DIR, next->d_name);

        fp = fopen(uevent_file, "r");
        if (NULL == fp)
        {
            continue;
        }
        while (fgets(line, sizeof(line), fp))
        {
            char *pos = NULL;
            XBH_S32 product_vid, product_pid, product_bcdev;
            char temp[10] = {0};
            pos = strstr(line, "PRODUCT=");
            if (NULL != pos)
            {
                if (sscanf(pos + 8, "%x/%x/%x", &product_vid, &product_pid, &product_bcdev) <= 0)
                {
                    continue;
                }
                XLOGV("get vid = 0x%02x pid = 0x%02x ", product_vid, product_pid);
                mDevices[count].device_vid = product_vid;
                mDevices[count].device_pid = product_pid;
                count ++;
            }
        }
        fclose(fp);
    }
    MaxDeviceNum = count;
    closedir(dir);
    return XBH_SUCCESS;
}

XBH_S32 XbhTouchInterfaceAuto::matchUsbNum()
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_S32 VID = 0xFFFF, PID = 0xFFFF;
    XBH_CHAR path[64];
    for(int i = 0; i < MaxDeviceNum; i++)
    {
        VID = mDevices[i].device_vid;
        PID = mDevices[i].device_pid;
        for(int j = 0; j < (sizeof(autoMatchTouchList)/sizeof(autoMatchTouchNode)); j++)
        {
            if ((VID == autoMatchTouchList[j].usb1_vid) && (PID == autoMatchTouchList[j].usb1_pid) && autoMatchTouchList[j].match1 != 1)
            {
                autoMatchTouchList[j].match1++;
                if(autoMatchTouchList[j].match2 != 0){
                    continue;
                }
            }
            else if ((VID == autoMatchTouchList[j].usb2_vid) && (PID == autoMatchTouchList[j].usb2_pid) && autoMatchTouchList[j].match2 != 1)
            {
                autoMatchTouchList[j].match2++;
                if(autoMatchTouchList[j].match1 != 0){
                    continue;
                }
            }
        }
    }
    for(int k = 0; k < (sizeof(autoMatchTouchList)/sizeof(autoMatchTouchNode)); k++)
    {
        matchID = autoMatchTouchList[k].match1 + autoMatchTouchList[k].match2;
        XLOGD("matchID = %d, match1 = %d, match2 = %d\n",matchID, autoMatchTouchList[k].match1, autoMatchTouchList[k].match2);
        memset(path, 0, sizeof(path));
        snprintf(path, sizeof(path), "%s%s", "/vendor/lib/modules/", autoMatchTouchList[k].ko);
        if(matchID == 2 && access(path, F_OK) == 0)
        {
            matchID = k;
            break;
        }
        else
        {
            matchID = -1;
            XLOGD("unKnow Touch!!!\n");
        }
    }
    return ret;
}


XBH_S32 XbhTouchInterfaceAuto::Init(XBH_VOID)
{
    XBH_S32 ret =  XBH_FAILURE;
    XBH_S32 mForceID;
    XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};

    if(getAllUsbDevices() == XBH_SUCCESS)
    {
        matchUsbNum();
    }

    mForceID = property_get_int32("persist.vendor.xbh.force.touch", -1);
    if(mForceID != -1)
    {
        matchID = mForceID;
    }

    if (matchID != -1)
    {
        XLOGD("matchID = %d, touch name is %s \n", matchID, autoMatchTouchList[matchID].name);
        property_set("persist.vendor.xbh.bezel.name", autoMatchTouchList[matchID].name);
        //将触摸框的第二路USB的PID VID存起来
        sprintf(value, "%d", autoMatchTouchList[matchID].usb2_pid);
        property_set("persist.vendor.xbh.bezel.pid", value);
        sprintf(value, "%d", autoMatchTouchList[matchID].usb2_vid);
        property_set("persist.vendor.xbh.bezel.vid", value);

        getTouchInterface();
        ret =  XBH_SUCCESS;
    }
    else
    {
        XLOGD(" can not find touch devices !!!!!!!!! \n");
    }
    return ret;
}

XbhTouchInterfaceAuto::XbhTouchInterfaceAuto()
{
    XLOGD(" begin ");
    Init();
    XLOGD(" end ");
}

XbhTouchInterfaceAuto::~XbhTouchInterfaceAuto()
{
    if(mInstance != NULL)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

XbhTouchControlInterface *XbhTouchInterfaceAuto::getInstance()
{
    XbhMutex::Autolock lock(mLockObject);
    if (mInstance == XBH_NULL) {
        mInstance = new XbhTouchInterfaceAuto();
    }
    return mInstance;
}

