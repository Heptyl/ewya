#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipIte6265_InputSource"
#include "XbhLog.h"

#include <stdio.h>
#include "ChipIte6265.h"
#include "XbhService.h"
#include "XbhPlatformInterface.h"
#include <unistd.h>
#include <cutils/properties.h>
#include <hardware/board.h>

ChipIte6265*              ChipIte6265::mInstance = XBH_NULL;
XbhMutex                     ChipIte6265::mLock;

XBH_U32 mHdmiOutEnable;
XBH_U32 mHdmiOutSoundEnable;
XBH_U32 mHdmiOutHdcpEnable;
XBH_U32 mHdmiOutResolution;


XBH_S32 ChipIte6265::resetChip()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (mRGpio == -1)
        return s32Ret;

    XbhService::getPlatformInterface()->setGpioOutputValue(mRGpio, !mRLevel);
    usleep(50*1000);
    XbhService::getPlatformInterface()->setGpioOutputValue(mRGpio, mRLevel);

    // if (196 == -1)
    //     return s32Ret;

    // XbhService::getPlatformInterface()->setGpioOutputValue(196, 1);
    // usleep(50*1000);
    // XbhService::getPlatformInterface()->setGpioOutputValue(196, 0);

    XLOGD("func: %s", __FUNCTION__);

    return s32Ret;
}

void ChipIte6265::run(const void* arg)
{
    XBH_S32 ret = 0;
    return ;
}

XBH_S32 ChipIte6265::getChipExist(XBH_BOOL* bEnable)
{
    if(m_pChipIt6265 == XBH_NULL)
    {
        *bEnable = XBH_FALSE;
    }
    else
    {
        *bEnable = XBH_TRUE;
    }
    return XBH_SUCCESS;
}

XBH_S32 ChipIte6265::getChipInitDone(XBH_BOOL* bEnable)
{
    return XBH_SUCCESS;
}

XBH_S32 ChipIte6265::getFirmwareVersion(XBH_CHAR* strVersion)
{
    return XBH_SUCCESS;
}

ChipIte6265::ChipIte6265(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    /*Set HDMI out to default OFF after upgrading bin or reseting system after delay one second*/
    XLOGD(" ChipIte6265 init begin ");
    usleep(1000 * 1000);

    // XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    // if(property_get("persist.xbh.hdmiout.inited", propVal, "false") > 0){
    //     if(strcmp(propVal, "false") == 0){
    //         XLOGD("ChipIte6265, init hdmi out to default off.");
    //         XbhService::getPlatformInterface()->setBootEnv("hdmi_out_mute_mode", "1");
    //         XbhService::getPlatformInterface()->setBootEnv("hdmi_out_sound_mute_mode", "1");
    //         property_set("persist.xbh.hdmiout.inited", "true");
    //     }
    // }
    mRGpio = rGpio;
    mRLevel = rLevel;

    m_pChipIt6265 = new It6265(i2cBus, rGpio, rLevel);
    m_pChipIt6265->start();

    // mHdmiOutEnable = m_pChipIt6265->getHdmiOutEnable();
    // mHdmiOutSoundEnable = m_pChipIt6265->getHdmiOutSoundEnable();
    // mHdmiOutHdcpEnable = m_pChipIt6265->getHdmiOutHdcpEnable();
    // mHdmiOutResolution = m_pChipIt6265->getHdmiOutResolution();
    XLOGD(" ChipIte6265 init end ");
}

ChipIte6265::~ChipIte6265()
{
}

// ChipIte6265* ChipIte6265::getInstance()
// {
//     XbhMutex::Autolock _l(mLock);
//     if (!mInstance)
//     {
//         mInstance = new ChipIte6265();
//         XLOGD("ChipIte6265 getInstance");
//     }
//     return mInstance;
// }

// //In sleep mode or screen recorer, do not save env 
// XBH_BOOL ChipIte6265::NoSaveEnv() {
//     XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
//     if(property_get("sys.xbh.screen.blank.status", propVal, "false") > 0){
//         if(strcmp(propVal, "true") == 0){
//             return XBH_TRUE;
//         }
//     }

//     if(property_get("sys.screen.recorder.running", propVal, "false") > 0){
//         if(strcmp(propVal, "true") == 0){
//             return XBH_TRUE;
//         }
//     }

//     return XBH_FALSE;
// }

// // Set OUT ON/OFF
// XBH_S32 ChipIte6265::setHdmiOutEnable(XBH_U32 onOff)
// {
//     if(mHdmiOutEnable == onOff) {
//         XLOGE("[%s:%d] Same status, return - %d", __func__, __LINE__, onOff);
//         return XBH_FAILURE;
//     }
//     mHdmiOutEnable = onOff;

//     if(m_pChipIt6265 != XBH_NULL)
//         m_pChipIt6265->setHdmiOutEnable(onOff);

//     /*Set sound ON or OFF at same time*/
//     setHdmiOutSoundEnable(onOff);

//     if(NoSaveEnv()) {
//         XLOGE("[%s:%d] do not save env", __func__, __LINE__);
//         return XBH_SUCCESS;
//     }

//     XBH_CHAR value[10] = {0};
//     sprintf(value, "%d", onOff);
//     XLOGD("[%s:%d] %s", __func__, __LINE__, value);

//     return XbhService::getPlatformInterface()->setBootEnv("hdmi_out_mute_mode", value);
// }

// // Get OUT ON/OFF
// XBH_U32 ChipIte6265::getHdmiOutEnable()
// {
//     return mHdmiOutEnable;
// }

// // Set SOUND ON/OFF
// XBH_S32 ChipIte6265::setHdmiOutSoundEnable(XBH_U32 onOff)
// {
//     if(mHdmiOutSoundEnable == onOff) {
//         XLOGE("[%s:%d] Same status, return - %d", __func__, __LINE__, onOff);
//         return XBH_FAILURE;
//     }
//     mHdmiOutSoundEnable = onOff;

//     if(m_pChipIt6265 != XBH_NULL)
//         m_pChipIt6265->setHdmiOutSoundEnable(onOff);

//     if(NoSaveEnv()) {
//         XLOGE("[%s:%d] do not save env", __func__, __LINE__);
//         return XBH_SUCCESS;
//     }

//     XBH_CHAR value[10] = {0};
//     sprintf(value, "%d", onOff);
//     XLOGD("[%s:%d] %s", __func__, __LINE__, value);

//     return XbhService::getPlatformInterface()->setBootEnv("hdmi_out_sound_mute_mode", value);
// }

// // Get SOUND ON/OFF
// XBH_U32 ChipIte6265::getHdmiOutSoundEnable()
// {
//     return mHdmiOutSoundEnable;
// }

// // Set HDCP ON/OFF
// XBH_S32 ChipIte6265::setHdmiOutHdcpEnable(XBH_U32 onOff)
// {
//     if(mHdmiOutHdcpEnable == onOff) {
//         XLOGE("[%s:%d] Same status, return - %d", __func__, __LINE__, onOff);
//         return XBH_FAILURE;
//     }
//     mHdmiOutHdcpEnable = onOff;

//     if(m_pChipIt6265 != XBH_NULL)
//         m_pChipIt6265->setHdmiOutHdcpEnable(onOff);

//     if(NoSaveEnv()) {
//         XLOGE("[%s:%d] do not save env", __func__, __LINE__);
//         return XBH_SUCCESS;
//     }

//     XBH_CHAR value[10] = {0};
//     sprintf(value, "%d", onOff);
//     XLOGD("[%s:%d] %s", __func__, __LINE__, value);

//     return XbhService::getPlatformInterface()->setBootEnv("hdmi_out_hdcp_enable", value);
// }

// // Get HDCP ON/OFF
// XBH_U32 ChipIte6265::getHdmiOutHdcpEnable()
// {
//     return mHdmiOutHdcpEnable;
// }

// // Set Resolution
// XBH_S32 ChipIte6265::setHdmiOutResolution(XBH_U32 resolution)
// {
//     if(mHdmiOutResolution == resolution) {
//         XLOGE("[%s:%d] Same resolution, return - %d", __func__, __LINE__, resolution);
//         return XBH_FAILURE;
//     }
//     mHdmiOutResolution = resolution;

//     switch(resolution) {
//         case 0: //480P
//             property_set("vendor.mstar.dualdisp.timing", "1280x720");
//             break;
//         case 1: //720P
//             property_set("vendor.mstar.dualdisp.timing", "1280x720");
//             break;
//         case 2: //1080P
//         default:
//             property_set("vendor.mstar.dualdisp.timing", "1920x1080");
//             break;
//     }

//     if(m_pChipIt6265 != XBH_NULL)
//         m_pChipIt6265->setHdmiOutResolution(resolution);

//     if(NoSaveEnv()) {
//         XLOGE("[%s:%d] do not save env", __func__, __LINE__);
//         return XBH_SUCCESS;
//     }

//     XBH_CHAR value[10] = {0};
//     sprintf(value, "%d", resolution);
//     XLOGD("[%s:%d] %s", __func__, __LINE__, value);

//     return XbhService::getPlatformInterface()->setBootEnv("hdmi_out_resolution", value);
// }

// // Get Resolution
// XBH_U32 ChipIte6265::getHdmiOutResolution()
// {
//     return mHdmiOutResolution;
// }