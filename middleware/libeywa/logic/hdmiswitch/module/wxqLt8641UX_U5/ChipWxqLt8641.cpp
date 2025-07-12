#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipWxqLt8641"

#include "XbhLog.h"
#include "XbhService.h"
#include "ChipWxqLt8641.h"

ChipWXQLT8641::ChipWXQLT8641(XBH_S32 u32SGpio, XBH_S32 u32SLevel, XBH_S32 u32SGpio2, XBH_S32 u32SLevel2, XBH_S32 powerGpio, XBH_S32 powerLevel, XBH_S32 resetGpio, XBH_S32 resetLevel, XBH_S32 defaultPort, HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level)
{
    XLOGD(" ChipWXQLT8641 start");
    this->mSwitchGpio = u32SGpio;
    this->mPortALevel = u32SLevel;
    this->mSwitchGpio2 = u32SGpio2;    // 新增成员变量
    this->mPortALevel2 = u32SLevel2;   // 新增成员变量
    u8DefaultPort = defaultPort;
    setActivePort(u8DefaultPort);
    XLOGD(" end");
}
ChipWXQLT8641::~ChipWXQLT8641()
{
    XLOGD(" end");
}

XBH_S32 ChipWXQLT8641::setActivePort(XBH_U8 port)
{
    if(mSwitchGpio == -1 || mPortALevel == -1 || 
       mSwitchGpio2 == -1 || mPortALevel2 == -1)  // 检查两个GPIO
    {
        return XBH_FAILURE;
    }
    
    XBH_U32 value1, value2;
    switch(port) {
        case 0: // USBC: 11
            value1 = !mPortALevel;
            value2 = !mPortALevel2;
            XLOGD(" ChipWXQLT8641 value1=%d value2=%d\n",value1,value2);
            break;   
        case 1:   // HDMI1: 00
            value1 = mPortALevel;
            value2 = mPortALevel2;
            XLOGD(" ChipWXQLT8641 value1=%d value2=%d\n",value1,value2);
            break;
        case 2:  // HDMI2: 10
            value1 = !mPortALevel;
            value2 = mPortALevel2;
            XLOGD(" ChipWXQLT8641 value1=%d value2=%d\n",value1,value2);
            break;
        case 3:  // HDMI3: 01
            value1 = mPortALevel;
            value2 = !mPortALevel2;
            XLOGD(" ChipWXQLT8641 value1=%d value2=%d\n",value1,value2);
            break;
        
        default:
            return XBH_FAILURE;
    }
    
    XbhService::getModuleInterface()->setGpioOutputValue(mSwitchGpio, value1);
    XbhService::getModuleInterface()->setGpioOutputValue(mSwitchGpio2, value2);
    XLOGD(" ChipWXQLT8641 mSwitchGpio=%d value1=%d  mSwitchGpio2=%d value2=%d\n",mSwitchGpio,value1,mSwitchGpio2,value2);
    
    return XBH_SUCCESS;
}

XBH_S32 ChipWXQLT8641::getActivePort(XBH_U8* port)
{
    XBH_U32 value1, value2;
    XbhService::getModuleInterface()->getGpioOutputValue(mSwitchGpio, &value1);
    XbhService::getModuleInterface()->getGpioOutputValue(mSwitchGpio2, &value2);
    
    if (value1 == mPortALevel && value2 == mPortALevel2) *port = 0;
    else if (value1 == mPortALevel && value2 != mPortALevel2) *port = 1;
    else if (value1 != mPortALevel && value2 == mPortALevel2) *port = 2;
    else if (value1 != mPortALevel && value2 != mPortALevel2) *port = 3;
    else return XBH_FAILURE;
    
    return XBH_SUCCESS;
}