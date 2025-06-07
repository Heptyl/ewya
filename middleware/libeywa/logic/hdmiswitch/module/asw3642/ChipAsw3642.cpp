#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipAsw3642"

#include "XbhLog.h"
#include "XbhService.h"
#include "ChipAsw3642.h"

ChipAsw3642::ChipAsw3642(XBH_S32 u32SGpio, XBH_S32 u32SLevel, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel, XBH_S32 defaultPort, HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level)
{
    XLOGD(" start level = %d ", level);
    this->mSwitchGpio = u32SGpio;
    this->mPortALevel = u32SLevel;
    u8DefaultPort = defaultPort;
    setActivePort(u8DefaultPort);
    XLOGD(" end");
}

ChipAsw3642::~ChipAsw3642()
{
    XLOGD(" end");
}

/**
 * 切换switch的port
 * port: 指定的port
 */
XBH_S32 ChipAsw3642::setActivePort(XBH_U8 port)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mSwitchGpio == -1 || mPortALevel == -1)
    {
        XLOGD("mSwitchGpio = %d, mPortALevel = %d  error !!!!", mSwitchGpio, mPortALevel);
        return s32Ret;
    }
    if (port == 0)
    {
        s32Ret = XbhService::getModuleInterface()->setGpioOutputValue(mSwitchGpio, mPortALevel);
    }
    else if (port == 1)
    {
        s32Ret = XbhService::getModuleInterface()->setGpioOutputValue(mSwitchGpio, !mPortALevel);
    }
    XLOGD("setActivePort port:%d", port);
    return s32Ret;
}

/**
 * 获取switch当前切换到的port
 * port: 指定的port
 */
XBH_S32 ChipAsw3642::getActivePort(XBH_U8* port)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(mSwitchGpio == -1 || mPortALevel == -1)
    {
        XLOGD("mSwitchGpio = %d, mPortALevel = %d  error !!!!", mSwitchGpio, mPortALevel);
        return s32Ret;
    }
    XBH_U32 u32GpioValue;
    s32Ret = XbhService::getModuleInterface()->getGpioOutputValue(mSwitchGpio, &u32GpioValue);
    *port = u32GpioValue == mPortALevel ? 0 : 1;
    return s32Ret;
}

