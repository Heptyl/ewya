#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipMs9282"

#include "XbhLog.h"
#include "ChipMs9282.h"
#include "commlib.h"
#include "MS928x.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "cutils/properties.h"

#include "XbhSourceManager.h"
#include "XbhService.h"

XbhMutex                    ChipMs9282::mLock;

SYSPARAM_T              g_stParam;
static UINT8            g_vgastatus  = 0;

UINT8 g_hposition = 50;
UINT8 g_vposition = 50;
UINT8 g_clock = 50;
UINT8 g_phase = 0;

XBH_S32 MS9282_Config(void)
{
    UINT8  u8State;
    // Set MS9281 system parameters.
    memset(&g_stParam, 0, sizeof(SYSPARAM_T));
    g_stParam.u8chipAddr              = MS928X_I2C_ADDR_92; //MS928x chip address
    g_stParam.u16sysConfig            = SYS_CFG_SIG_CHANNEL_1 | SYS_CFG_DDC_DISABLED; //choose channel
    g_stParam.videoParam.u8sourceIn   = IN_ANALOG_VGA;	//VGA input
    
    g_stParam.audioParam.u8audMode    = MS928X_AUD_MODE_I2S;
    g_stParam.audioParam.u8audRate    = MS928X_AUD_RATE_48K;
    g_stParam.audioParam.u8audBits    = MS928X_AUD_LEN_16;
    g_stParam.audioParam.u8audChannel = MS928X_AUD_CH_2CH;

    u8State = MS928X_Init(&g_stParam);
    XLOGD("System Init status: %d", u8State);
    return u8State;
}

void Media_Service(void)
{
    BOOL b_hpd;
    //Poling System Status
    g_vgastatus = MS928X_Poll();
    XLOGV("Current Status:  0x%x ", g_vgastatus);
    // $BUGBUG: Checking the polling status.
    if (g_vgastatus  == STATE_SUCCESS)
    {
        MS928X_SetVideo();
        MS928X_SetAudio();
    }
    b_hpd = MS928X_HPD_Detect();
    XLOGV("Current b_hpd: %d ", b_hpd);
}

void ChipMs9282::initVgaData()
{
    // init vga data
    XLOGD("initVgaData");
    g_hposition = (UINT8)property_get_int32("persist.vendor.xbh.source.vga.hposition", 50);
    g_vposition = (UINT8)property_get_int32("persist.vendor.xbh.source.vga.vposition", 50);
    g_clock = (UINT8)property_get_int32("persist.vendor.xbh.source.vga.clock", 50);
    g_phase = (UINT8)property_get_int32("persist.vendor.xbh.source.vga.phase", 0);
    MS928X_ShiftConfig(g_hposition, g_vposition);
    if(g_phase == 0)
    {
        vgaAutoAdjust();
    }
    else
    {
        setVgaPhase((XBH_U32)g_phase);
    }
}

XBH_S32 ChipMs9282::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mRGpio >= 0)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        usleep(500 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
        usleep(1000 * 1000);
        s32Ret = XBH_SUCCESS;
    }
    return  s32Ret;
}

XBH_S32 ChipMs9282::vgaAutoAdjust()
{
    XBH_S32 ret = XBH_SUCCESS;
    XLOGD("vgaAutoAdjust");

    //auto phase
    MS928X_Video_Mute(TRUE);
    MS928X_Audio_Mute(TRUE);
    MS928X_PhaseConfig(1, 0);
    MS928X_Video_Mute(FALSE);
    MS928X_Audio_Mute(FALSE);

    //auto position
    MS928X_SetVideo();
    MS928X_SetAudio();

    g_phase = 0;
    property_set("persist.vendor.xbh.source.vga.phase", "0");
    g_hposition = 50;
    property_set("persist.vendor.xbh.source.vga.hposition", "50");
    g_vposition = 50;
    property_set("persist.vendor.xbh.source.vga.vposition", "50");

    return ret;
}

XBH_S32 ChipMs9282::setVgaHPosition(XBH_U32 u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};
    g_hposition = (UINT8)u32Value;
    XLOGD("setVgaHPosition: %d", g_hposition);
    MS928X_ShiftConfig(g_hposition, g_vposition);
    sprintf(value, "%d", g_hposition);
    property_set("persist.vendor.xbh.source.vga.hposition", value);
    return ret;
}

XBH_S32 ChipMs9282::getVgaHPosition(XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    *u32Value = g_hposition;
    XLOGD("getVgaHPosition: %d", *u32Value);
    return ret;
}

XBH_S32 ChipMs9282::setVgaVPosition(XBH_U32 u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};
    g_vposition = (UINT8)u32Value;
    XLOGD("setVgaVPosition: %d", g_vposition);
    MS928X_ShiftConfig(g_hposition, g_vposition);
    sprintf(value, "%d", g_vposition);
    property_set("persist.vendor.xbh.source.vga.vposition", value);
    return ret;
}

XBH_S32 ChipMs9282::getVgaVPosition(XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    *u32Value = g_vposition;
    XLOGD("getVgaVPosition: %d", *u32Value);
    return ret;
}

XBH_S32 ChipMs9282::setVgaClock(XBH_U32 u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};
    g_clock = (UINT8)u32Value;
    XLOGD("setVgaClock: %d", g_clock);
    sprintf(value, "%d", g_clock);
    property_set("persist.vendor.xbh.source.vga.clock", value);
    return ret;
}

XBH_S32 ChipMs9282::getVgaClock(XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    *u32Value = g_clock;
    XLOGD("getVgaClock: %d", *u32Value);
    return ret;
}

XBH_S32 ChipMs9282::setVgaPhase(XBH_U32 u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};
    // Phase from 0~31
    UINT8 phase = (UINT8)u32Value / 3;
    if(phase < 0)
    {
        phase = 0;
    } 
    else if(phase > 31)
    {
        phase = 31;
    }
    XLOGD("setVgaPhase: u32Value=%d phase=%d", u32Value, phase);
    g_phase = u32Value;
    MS928X_PhaseConfig(0, phase);
    sprintf(value, "%d", g_phase);
    property_set("persist.vendor.xbh.source.vga.phase", value);
    return ret;
}

XBH_S32 ChipMs9282::getVgaPhase(XBH_U32 *u32Value)
{
    XBH_S32 ret = XBH_SUCCESS;
    *u32Value = g_phase;
    XLOGD("getVgaPhase: %d", *u32Value);
    return ret;
}

XBH_S32 ChipMs9282::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 ChipMs9282::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 ChipMs9282::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 ChipMs9282::getCurVgaStatus(XBH_U8 *u8State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *u8State = g_vgastatus;
    return s32Ret;
}

XBH_S32 ChipMs9282::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 ChipMs9282::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_SOURCE_E u32Source;
void ChipMs9282::run(const void* arg)
{
    XbhSourceManager::getInstance()->getCurrSource(&u32Source);
    if((u32Source == XBH_SOURCE_VGA1 || u32Source == XBH_SOURCE_VGA2) && property_get_bool("vendor.xbh.livetv.running", XBH_FALSE))
    {
        XLOGV("Media_Service quick scan");
        usleep(200 * 1000);
        //VGA有信号的时候才能做auto just
        if(mNeedInitData &&(g_vgastatus == STATE_SUCCESS || g_vgastatus == STATE_MODE_NOT_CHANGED))
        {
            initVgaData();
            mNeedInitData = FALSE;
        }
    }
    else
    {
        XLOGV("Media_Service slow scan");
        usleep(1000 * 1000);
    }
    Media_Service();
}

ChipMs9282::ChipMs9282(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    MS928X_SetI2c(i2cBus, Addr);
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    mNeedInitData = XBH_TRUE;
    mChipInit = XBH_FALSE;
    if (mPGpio > 0)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mPGpio, mPLevel);
        usleep(20 * 1000);
    }
    if(mRGpio >= 0)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        usleep(500 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
        usleep(1500 * 1000);
    }
    if(MS9282_Config() == STATE_SUCCESS)
    {
        mChipInit = XBH_TRUE;
        XbhMWThread::run(XbhMWThread::REPEAT);
    }
    else
    {
        XLOGD(" not found ms9282 !!!!!");
    }
    XLOGD(" end ");
}

ChipMs9282::~ChipMs9282()
{

}

