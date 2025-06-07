#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMR3576_B"

#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhXMR3576_B.h"
#include "XbhSysOpt.h"
#include "XbhSourceManager.h"
#include "XbhRk_3576.h"
#include "XbhPartitionData.h"
#include "XbhLiftbracketManager.h"
#include <hardware/xbh_edid.h>
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
XbhXMR3576_B            *XbhXMR3576_B::mInstance = XBH_NULL;
XbhMutex                 XbhXMR3576_B::mLock;

#define RTK8367_WOL_DEV         "/proc/rtk_gsw/wol"
#define HDMI_RX_EDID            "/sys/class/hdmirx/rk628/edid_index"
#define USBC1_OUT_DET_VALUE      0x0f

XbhXMR3576_B::XbhXMR3576_B()
{
    XLOGD(" begin ");
    XbhRk_3576::setShutdownFlag(XBH_FALSE);
    XLOGD(" end ");
}

XbhXMR3576_B::~XbhXMR3576_B()
{
}

XBH_S32 XbhXMR3576_B::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_HDMI_2_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HDMI_2_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_HDMI_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_HDMI_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_USBC_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_USBC_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_F_USBC_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_F_USBC_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_DP_1_IN_DET_PIN
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_1_DET
    XBH_U32 u32GpioValue;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
    *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    #endif
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::getOut1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    char sys_boot[PROPERTY_VALUE_MAX] = {0};
    char uac_type[PROPERTY_VALUE_MAX] = {0};
    s32Ret = XbhRk_3576::getIICData(XBH_ET7303_I2C_BUS,XBH_ET7303_I2C_DEVICES_ADDR,XBH_ET7303_I2C_INTERRUPT_ADDR,0x01,0x01,data);
    property_get("persist.vendor.xbh.usb.uac_enable", uac_type, "false");
    if(data[0] & USBC1_OUT_DET_VALUE)//此处通过中断寄存器的值来检查tyep-c out的插拔状态
    {
        *enable = XBH_TRUE;
        property_get("sys.boot_completed", sys_boot, "0");
        if(strcmp(sys_boot, "1") == XBH_SUCCESS)
        {     
            if(!(strcmp(uac_type, "true") == XBH_SUCCESS))
            {
                property_set("persist.vendor.xbh.usb.uac_enable", "true");
            }
        }
        else
        {   
            if(!(strcmp(uac_type, "false") == XBH_SUCCESS))
            {
                property_set("persist.vendor.xbh.usb.uac_enable", "false");
            }
        }
    }
    else
    {   
        *enable = XBH_FALSE;
        if(!(strcmp(uac_type, "false") == XBH_SUCCESS))
        {
            property_set("persist.vendor.xbh.usb.uac_enable", "false");
        }
    }
    return s32Ret;
}

XBH_S32 XbhXMR3576_B::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PS_ON
    XBH_U32 u32OpspowerTimer = 500;
    XBH_U32 u32Ops12VTimer = 200;
    XBH_BOOL bIsDet = XBH_FALSE;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_POWER_STATUS
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_POWER_STATUS, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_POWER_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_1_DET
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioInputValue(XBH_BOARD_GPIO_OPS_1_DET, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_1_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}


XBH_S32 XbhXMR3576_B::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PWREN
    if (enSource == XBH_SOURCE_OPS1)
    {
        if (bEnable)
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
        else
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, !XBH_BOARD_GPIO_OPS_PWREN_LEVEL);
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #ifdef XBH_BOARD_GPIO_OPS_PWREN
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWREN_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getAirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAirSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getAirSensorValue(XBH_FLOAT* fHumidity, XBH_FLOAT* fTemperature, XBH_FLOAT* fTvoc, XBH_FLOAT* fPm2p5, XBH_FLOAT* fPm1p0, XBH_FLOAT* fPm10, XBH_FLOAT* fCo2)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *fHumidity = XbhAirSensorManager::getInstance()->getHumidityValue();
    *fTemperature = XbhAirSensorManager::getInstance()->getTemperatureValue();
    *fTvoc = XbhAirSensorManager::getInstance()->getVocValue();
    *fPm2p5 = XbhAirSensorManager::getInstance()->getPm25Value();
    *fCo2 = XbhAirSensorManager::getInstance()->getCo2Value();
    *fPm1p0 = 0;
    *fPm10 = 0;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
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
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("getMute channel = %d",enAudioChannel);
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
    return  s32Ret;
}
XBH_S32 XbhXMR3576_B::getHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string value;
    XLOGD("get Hdmi Tx Info enInfo = %d",enInfo);
    switch(enInfo)
    {
        case XBH_HDMI_TX_VIDEO_MUTE:
            XbhSysOpt::getInstance()->readSysfs("/sys/class/drm/card0-DP-1/status", value);
            if(strncmp(value.c_str(),"connected",strlen("connected")) == 0)
            {
                strcpy(info,"0");
            }
             else if(strncmp(value.c_str(),"disconnected",strlen("disconnected")) == 0)
            {
                strcpy(info,"1");
            }
            s32Ret = XBH_SUCCESS;
            break;
        default:
            break;
    }
    return s32Ret;
}
XBH_S32 XbhXMR3576_B::setHdmiTxInfo(XBH_HDMI_TX_INFO_E enInfo, XBH_U32 u32Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("set Hdmi Tx Info enInfo = %d data = %d ",enInfo, u32Data);
    switch(enInfo)
    {
        case XBH_HDMI_TX_VIDEO_MUTE:    //HDMI TX VIDEO 开关(可选支持)
            if(u32Data == 0)
            {
                XbhSysOpt::getInstance()->writeSysfs("/sys/class/drm/card0-DP-1/status","detect");
            }
            else if(u32Data == 1)
            {
                XbhSysOpt::getInstance()->writeSysfs("/sys/class/drm/card0-DP-1/status","off");
            }
            s32Ret = XBH_SUCCESS;
            break;
        default:
            break;
    }
    return s32Ret;
}

XBH_S32 XbhXMR3576_B::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XLOGD("set Hdmi Tx Info enType = %d",enType);
    //根据project_id 更新 HDMI switch 的edid 从而达到切换edid type的作用
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    XBH_S32 type  = 2;
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"0");
            if(projectid == 0)
            {
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v1_4_2K, 0);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v1_4_2K, 1);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v1_4_2K, 2);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v1_4_2K, 3);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID1_v1_4_2K, 0);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID2_v1_4_2K, 1);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID3_v1_4_2K, 2);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID4_v1_4_2K, 3);

                setHdmiRx5vDet(XBH_TRUE);
            }
            else
            {
                char edid20BinFilePath[128] = {0};
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 0);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 1);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 2);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 3);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 0);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 1);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 2);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 3);

                setHdmiRx5vDet(XBH_TRUE);
            }
            type = 1;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_20:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"1");
            if(projectid == 0)
            {
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000); 
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v2_0_4K_2K, 0);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v2_0_4K_2K, 1);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v2_0_4K_2K, 2);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1, XBH_EDID3_v2_0_4K_2K, 3);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID1_v2_0_4K_2K, 0);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID2_v2_0_4K_2K, 1);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID3_v2_0_4K_2K, 2);
                XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1, XBH_EDID4_v2_0_4K_2K, 3);

                setHdmiRx5vDet(XBH_TRUE);
            }
            else
            {
                char edid20BinFilePath[128] = {0};
                setHdmiRx5vDet(XBH_FALSE);
                usleep(200*1000);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 0);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 1);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 2);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 3);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 0);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 1);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 2);
                snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v2.0_4K_2K.bin", projectid);
                readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 3);

                setHdmiRx5vDet(XBH_TRUE);
            }
            type = 2;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_21:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"2");
            type = 3;
            s32Ret = XBH_SUCCESS;
        break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            //XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"3");
            type = 4;
            s32Ret = XBH_SUCCESS;
        break;
        default:
        break;
    }
    s32Ret = XbhRk_3576::setHdmirxEdidType(type);
    XLOGD("setHdmirxEdidType %d \n", type);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type  = 2;
    s32Ret = XbhRk_3576::getHdmirxEdidType(&type);
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

XBH_S32 XbhXMR3576_B::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMR3576_B::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt1 = 0;
    XBH_U32 u32GpioPirInt2 = 0;

    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT1,&u32GpioPirInt1);
    s32Ret |= getGpioInputValue(XBH_BOARD_GPIO_PIR_INT2,&u32GpioPirInt2);

    if(u32GpioPirInt1 == 0 || u32GpioPirInt2 == 0)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;
    }

    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 adcValue = 0;

    s32Ret = getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)XBH_BOARD_ADC_HALL_CHAN, &adcValue);

    if(adcValue > 1300 && adcValue < XBH_BOARD_ADC_HALL_CHAN_VALUE)
    {
        *s32Value = 1;   //当只有一支笔,不管是A笔还是B笔,范围都在1300<adcValue<2500
    }
    else if(adcValue > XBH_BOARD_ADC_HALL_CHAN_VALUE)
    {
        *s32Value = 2;   //当两只笔都放上,adcValue=4095
    }
    else
    {
        *s32Value = 0;   //当没有笔放上,adcValue=1093
    }

    return s32Ret;
}

XBH_S32 XbhXMR3576_B::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32GpioPirInt1 = 0;
    XBH_U32 u32GpioPirInt2 = 0;

    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT1, &u32GpioPirInt1);
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_INT2, &u32GpioPirInt2);
    if(u32GpioPirInt1 == 0 || u32GpioPirInt2 == 0 ){
        *bEnable = 1;
    }else{
        *bEnable = 0;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_DP1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2715_1, ver);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getFirmwareVersion(ver);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_GSV2712, ver);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
	    case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_DP1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2715_1, filename, force);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->upgradeFirmware(filename,force);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_GSV2712, filename, force);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        default:
            break;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}
XBH_S32 XbhXMR3576_B::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_FRONT_LT8711UXE1:
            s32Ret = XbhUsbc2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_BOARD_DEV6563_1:
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_DP1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2715_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2715_1, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2712:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_GSV2712, s32Value);
            break;
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_AUDIO_CODEC:
            s32Ret = XbhAudioCodecManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}


XBH_S32 XbhXMR3576_B::openEthWol(XBH_BOOL bEnable)
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

XBH_S32 XbhXMR3576_B::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{

    XLOGD("getSrcDetByDetGpio gpio = %d u32GpioValue = u32GpioValue %d\n", gpio ,u32GpioValue);
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_SOURCE_E currSource;
    s32Ret = XbhSourceManager::getInstance()->getCurrSource(&currSource);
    XBH_BOOL isSpeakerMute = XBH_FALSE;
    XBH_S32 result = getMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, &isSpeakerMute);

    #if 1
    switch(gpio)
    {
        case XBH_BOARD_GPIO_HDMI_1_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI1;
            if(u32GpioValue == XBH_BOARD_GPIO_HDMI_1_DET_LEVEL)
            {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
               *bEnable = XBH_FALSE;
               //setHdmirxControlState(false);
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI1)
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
        case XBH_BOARD_GPIO_HDMI_2_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_HDMI2;
            if(u32GpioValue == XBH_BOARD_GPIO_HDMI_2_DETLEVEL)
            {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI2)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
               *bEnable = XBH_FALSE;
               //setHdmirxControlState(false);
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_HDMI2)
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
        case XBH_BOARD_GPIO_USBC_1_DET:
            *src = XBH_SOURCE_E::XBH_SOURCE_USBC1;
            if(u32GpioValue == XBH_BOARD_GPIO_USBC_1_DET_LEVEL)
            {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_USBC1)
               {
                   setHdmiRx5vDet(XBH_TRUE);
               }
            }
            else
            {
               *bEnable = XBH_FALSE;
               //setHdmirxControlState(false);
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_USBC1)
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
        case XBH_BOARD_GPIO_DP_1_IN_DET_PIN:
        //#444371 【通道功能测试】拔掉DP通道信号线再插入，画面显示无信号
        //DP通道时，稍微动一下插头/抬一下线会无信号，看log发现中断出现，因此将DP改为轮询，中断注释掉
           *src = XBH_SOURCE_E::XBH_SOURCE_DP1;
           if(u32GpioValue == XBH_BOARD_GPIO_DP_1_IN_DEF_LEVEL)
           {
               *bEnable = XBH_TRUE;
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_DP1)
               {
                   //setHdmiRx5vDet(XBH_TRUE);
               }
           }
           else
           {
               *bEnable = XBH_FALSE;
               //setHdmirxControlState(false);
               if(currSource == XBH_SOURCE_E::XBH_SOURCE_DP1)
               {
                   //setHdmiRx5vDet(XBH_FALSE);
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


XBH_S32 XbhXMR3576_B::setHdmiRx5vDet(XBH_BOOL enable)
{
    XLOGD("setHdmiRx5vDetFlag state:%d\n", enable);
    XBH_S32 s32Ret = XBH_SUCCESS;
	XbhRk_3576::setHdmiRx5vDet(enable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR s8Buff[XBH_CUSPARAM_WAKEUP_SWITCH_LEN] = {0};
    XbhRk_3576::setOnStop();
    s32Ret = XbhPartitionData::getInstance()->getWotParamValue((XBH_VOID *)s8Buff);
    freeAllGpioIrq();
    //CEC 唤醒时需要使能CEC引脚的中断
    if(s8Buff[XBH_WAKEUP_CEC])
    {
        requestGpioIrq(XBH_BOARD_GPIO_HDMI_CEC, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING);
    }

    s32Ret = XbhRk_3576::setShutdownFlag(XBH_TRUE);
    s32Ret = setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    s32Ret |= setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
    setGpioOutputValue(XBH_BOARD_GPIO_AP8224_PWR,XBH_BOARD_GPIO_CODEC_POWER_OFF);//关闭codes电源
    setGpioOutputValue(XBH_BOARD_GPIO_AMP_MUTE,XBH_BOARD_GPIO_AMP_MUTE_ON);//关闭AMP PD
    setHdmiTxInfo(XBH_HDMI_TX_VIDEO_MUTE, 1);//关闭 DP
    requestGpioIrq(XBH_WAKE_UP_POWER_KEY, XBH_WAKE_UP_POWER_KEY_TYPE,XBH_FALSE);

    #ifdef XBH_BOARD_GPIO_NFC_EN
    setGpioOutputValue(XBH_BOARD_GPIO_NFC_EN, XBH_BOARD_GPIO_NFC_EN_LEVEL);
    #endif
    if (s8Buff[XBH_WAKEUP_NFC])
    {
        XLOGD("nfc wake is open");
        requestGpioIrq(XBH_WAKE_UP_MCU_IRQ, XBH_WAKE_UP_MCU_IRQ_TYPE,XBH_FALSE);
        // 进入待机后关闭部分mcu引脚
        setGpioOutputValue(XBH_BOARD_GPIO_ADC_RESET, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_TOUCH_DEV_PWR, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_HP_MUTE, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWREN, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_HUB_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_DPOUT_HDMI_RESET, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU40_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_HDMI_SEL, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_OPS_DEVICE_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_WIFI_RESET, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_HUB_HUB20_UU4_RST, XBH_BOARD_GPIO_LOW);
        setGpioOutputValue(XBH_BOARD_GPIO_F_GPIO2, XBH_BOARD_GPIO_LOW);
        //1.设置NFC唤醒后，需屏蔽按键板power功能，屏蔽PIR唤醒（PIR）、信源唤醒（WOS）
        //2.需要保留遥控IR Power唤醒，DMS/wol开机 ,有线 rs232、（考虑异常情况，可以通过远程开机）
        setAdcIntEnabled(XBH_FALSE);
        s8Buff[XBH_WAKEUP_SOURCE] = 0;
        s8Buff[XBH_WAKEUP_PIR] = 0;
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_MCU_PWR, !XBH_BOARD_GPIO_MCU_PWR_ON);
    }

    if (s8Buff[XBH_WAKEUP_SOURCE])
    {
        //判断前置板是否使用GSV芯片，如果是则在关机时然GVS芯片进入待机模式，也就是开启前置板typec信源唤醒
        char dev_type[PROPERTY_VALUE_MAX] = {0};
        property_get("persist.vendor.xbh.fboard.type", dev_type, "0");
        if(strcmp(dev_type, "0x20") == 0)
        {
            XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_FRONT_GSV2712);
        }
        XbhHdmiSwitchManager::getInstance()->setSpecificMode(XBH_UPDATE_BOARD_GSV2715_1);
        XLOGD("Source wake is open");
        requestGpioIrq(XBH_WAKE_UP_HDMI1_DET, XBH_WAKE_UP_HDMI1_DET_TYPE,XBH_FALSE);
        requestGpioIrq(XBH_WAKE_UP_HDMI2_DET, XBH_WAKE_UP_HDMI2_DET_TYPE,XBH_FALSE);
        requestGpioIrq(XBH_WAKE_UP_DP_DET, XBH_WAKE_UP_DP_DET_TYPE,XBH_FALSE);
        requestGpioIrq(XBH_WAKE_UP_F_HDMI_DET, XBH_WAKE_UP_F_HDMI_DET_TYPE,XBH_FALSE);
        requestGpioIrq(XBH_WAKE_UP_TYPE_C_DET, XBH_WAKE_UP_TYPE_C_DET_TYPE,XBH_FALSE);
        requestGpioIrq(XBH_WAKE_UP_F_TYPE_C_DET, XBH_WAKE_UP_F_TYPE_C_DET_TYPE,XBH_FALSE);
        if(s8Buff[XBH_WAKEUP_BTYPEC] == 0)
        {
            setGpioOutputValue(XBH_BOARD_GPIO_PD_POWER, XBH_BOARD_GPIO_PD_POWER_OFF);
        }
        if(s8Buff[XBH_WAKEUP_FTYPEC] == 0)
        {
            setGpioOutputValue(XBH_BOARD_GPIO_F_USBC_PWR,!XBH_BOARD_GPIO_F_USBC_PWR_LEVEL);
        }
    }
    else
    {
        setGpioOutputValue(XBH_BOARD_GPIO_PD_POWER, XBH_BOARD_GPIO_PD_POWER_OFF);
        setGpioOutputValue(XBH_BOARD_GPIO_F_USBC_PWR,!XBH_BOARD_GPIO_F_USBC_PWR_LEVEL);
    }

    if (s8Buff[XBH_WAKEUP_ETH])
    {
        XLOGD("eth wake is open");
        openEthWol(XBH_TRUE);
        requestGpioIrq(XBH_WAKE_UP_ETH, XBH_WAKE_UP_ETH_TYPE,XBH_FALSE);
    }
    else
    {
        XLOGD("eth wake is close");
        setGpioOutputValue(XBH_BOARD_GPIO_ETH_PWR, XBH_BOARD_GPIO_ETH_PWR_OFF);
    }

    if (s8Buff[XBH_WAKEUP_PIR])
    {
        XLOGD("pir wake is open");
        requestGpioIrq(XBH_WAKE_UP_PIR1, XBH_WAKE_UP_PIR1_TYPE,XBH_FALSE);
        requestGpioIrq(XBH_WAKE_UP_PIR2, XBH_WAKE_UP_PIR2_TYPE,XBH_FALSE);
    }


    XLOGD("---- onStop ----");
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::initSrcDetGpioIrq()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    #if 1
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_HDMI_1_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_HDMI_2_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_F_HDMI_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_F_USBC_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT, XBH_FALSE);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_DP_1_IN_DET_PIN, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT,XBH_TRUE);
    s32Ret |= requestGpioIrq(XBH_BOARD_GPIO_USBC_1_DET, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |IRQF_ONESHOT,XBH_FALSE);
    //requestGpioIrq(XBH_BOARD_GPIO_USBC_1_DET, IRQF_TRIGGER_FALLING);
    #endif
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::setUsbLockMode(XBH_U32 u32UsbType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    char type[PROPERTY_VALUE_MAX] = {0};
    sprintf(type, "%d", u32UsbType);
    property_set("persist.vendor.xbh.usb_lock_mode", type);
    usleep(300 *1000);
    // front usb port
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_F_USB_PWR, (u32UsbType == 0 || u32UsbType == 2) ? XBH_BOARD_GPIO_F_USB_PWR_LEVEL : !XBH_BOARD_GPIO_F_USB_PWR_LEVEL);
    // sider usb port
    #ifdef XBH_BOARD_GPIO_AN_PROT_DEV_PWR
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_AN_PROT_DEV_PWR, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_AN_PROT_DEV_PWR_ON : XBH_BOARD_GPIO_AN_PROT_DEV_PWR_OFF);
    #endif
    s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_PUB_USB30_DEV_PWR, (u32UsbType == 0 || u32UsbType == 1) ? XBH_BOARD_GPIO_PUB_USB30_DEV_PWR_ON : XBH_BOARD_GPIO_PUB_USB30_DEV_PWR_OFF);
    return s32Ret;
}

XBH_S32 XbhXMR3576_B::getUsbLockMode(XBH_U32 *u32UsbType)
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
XBH_S32 XbhXMR3576_B::setAvcEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setAvcEnable(bEnable);
    return  s32Ret;
}

/**
 * 获取AVC(自动电压控制)开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMR3576_B::getAvcEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getAvcEnable(bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::setLiftbracketOperate(XBH_S32 value)
{
    XBH_S32 ret = XbhLiftbracketManager::getInstance()->setLiftbracketOperate(value);
    return ret;
}

XBH_S32 XbhXMR3576_B::getLiftbracketExist(XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLiftbracketManager::getInstance()->getLiftbracketExist(value);
    return  s32Ret;
}

XBH_S32 XbhXMR3576_B::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 ret = XbhRk_3576::setHdmiLowLatency(bEnable);
    return ret;
}

XBH_S32 XbhXMR3576_B::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    if(enSource == XBH_SOURCE_USBC1)
    {
        ret = XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_BOARD_GSV2715_1, pdability);
    }
    else if(enSource == XBH_SOURCE_F_USBC1)
    {
        ret = XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_F_USBC1, pdability);
    }
    else
    {
        XLOGE("The 'enSource' parameter is invalid.\n");
        return XBH_FAILURE;
    }
    return ret;
}
XBH_S32 XbhXMR3576_B::setOpsResetEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (bEnable)
    {
        s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_OPS_DEVICE_RST, !XBH_BOARD_GPIO_OPS_DEVICE_RST_OFF);
    }
    else
    {
        s32Ret = this->XbhRk_3576::setGpioOutputValue(XBH_BOARD_GPIO_OPS_DEVICE_RST, XBH_BOARD_GPIO_OPS_DEVICE_RST_OFF);
    }
    XLOGD("setOpsResetEnable bEnable=%d\n", bEnable);
    return  s32Ret;
}

/**
 * 读取预置EDID的bin文件值，并通过Edidtype更新EDID的bin数据到HDMI switch
 * @param projecitd
 */
XBH_S32 XbhXMR3576_B::readAndUpdateEdidBinFile(int projectid)
{
    //XLOGD("readAndUpdateEdidBinFile projectid=%d\n",projectid);
    if(projectid > 0)//分别读取4个bin文件，对应不同端口的edid
    {
        XBH_S32 type = 1;
        XbhRk_3576::getHdmirxEdidType(&type); 
        if(type == 1)
        {
            char edid14BinFilePath[128] = {0};
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid14BinFilePath, 0);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid14BinFilePath, 1);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid14BinFilePath, 2);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid14BinFilePath, 3);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, 0);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, 1);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, 2);
            snprintf(edid14BinFilePath, sizeof(edid14BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v1.4_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid14BinFilePath, 3);
        }
        else if(type == 2)
        {
            char edid20BinFilePath[128] = {0};
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 0);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 1);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 2);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2705_1, edid20BinFilePath, 3);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID1_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 0);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID2_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 1);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID3_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 2);
            snprintf(edid20BinFilePath, sizeof(edid20BinFilePath), "/vendor/etc/project_ids/projectid_%d/edid/BIN/XBH_EDID4_v2.0_4K_2K.bin", projectid);
            readAndUpdateEdidBinFileByEdidType(XBH_UPDATE_BOARD_GSV2715_1, edid20BinFilePath, 3);
        }   

    }
    return XBH_SUCCESS;
}

/**
 * 初始化project_id配置
 */
XBH_S32 XbhXMR3576_B::initProjectIdConfig()
{
    //每次开机都需要更新edid的数据
    XBH_S32 projectid = property_get_int32("persist.vendor.xbh.project_id", 0);
    XBH_S32 type = 1;
    //projectid = property_get_int32("persist.vendor.xbh.project_id_test", 0);
    if(projectid == 0)//默认edid从xbh_edid.h（CUSTOM目录转化而来）中读取
    {
        XbhRk_3576::getHdmirxEdidType(&type);
        if(type == 1)
        {
            //*enType = XBH_HDMIRX_EDID_TYPE_14;
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v1_4_2K,0);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v1_4_2K,1);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v1_4_2K,2);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v1_4_2K,3);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID1_v1_4_2K,0);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID2_v1_4_2K,1);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID3_v1_4_2K,2);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID4_v1_4_2K,3);
        }
        else if(type == 2)
        {
            //*enType = XBH_HDMIRX_EDID_TYPE_20;
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v2_0_4K_2K,0);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v2_0_4K_2K,1);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v2_0_4K_2K,2);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2705_1,XBH_EDID3_v2_0_4K_2K,3);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID1_v2_0_4K_2K,0);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID2_v2_0_4K_2K,1);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID3_v2_0_4K_2K,2);
            XbhHdmiSwitchManager::getInstance()->updateEdid(XBH_UPDATE_BOARD_GSV2715_1,XBH_EDID4_v2_0_4K_2K,3);
        }    

    }
    else//其它projectid 从bin文件中获取
    {
        readAndUpdateEdidBinFile(projectid);
    }
    return XBH_SUCCESS;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMR3576_B::getHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhRk_3576::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMR3576_B::getHdmi2SignalStatus(XBH_S32 *status)
{
    return this->XbhRk_3576::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMR3576_B::getUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhRk_3576::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMR3576_B::getDP1SignalStatus(XBH_S32 *status)
{
    return this->XbhRk_3576::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMR3576_B::getOps1SignalStatus(XBH_S32 *status)
{
    return this->XbhRk_3576::getSignalStatus(status);
}

XbhXMR3576_B *XbhXMR3576_B::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMR3576_B();
        }
    }
    return mInstance;
}