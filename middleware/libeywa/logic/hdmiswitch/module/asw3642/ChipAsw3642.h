#ifndef _CHIP_ASW3642_H_
#define _CHIP_ASW3642_H_

#include "XbhType.h"
#include "XbhCommon.h"
#include "XbhMWThread.h"
#include "XbhHdmiSwitchInterface.h"

class ChipAsw3642 : public XbhHdmiSwitchInterface
{
public:

    /**
     * u32SGpio       A/B端口切换gpio
     * u32SLevel      A端口电平
     * powerGpio       上电GPIO
     * powerLevel      工作电平
     * resetGpio       复位GPIO
     * resetLevel      复位电平
     * defaultPort     默认port
     * u8SwitchPort    各个port对应的通道序号
     */
    ChipAsw3642(XBH_S32 u32SGpio, XBH_S32 u32SLevel, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel, XBH_S32 defaultPort, HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level);
    ~ChipAsw3642();
    
    XBH_S32 setActivePort(XBH_U8 port);
    XBH_S32 getActivePort(XBH_U8* port);

private:
    XBH_S32 mSwitchGpio;
    XBH_S32 mPortALevel;
    XBH_U8  u8DefaultPort;
};


#endif //_CHIP_ASW3642_H_

