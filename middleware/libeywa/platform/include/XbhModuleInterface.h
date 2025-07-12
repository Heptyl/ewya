#ifndef XBH_MODULE_INTERFACE_H
#define XBH_MODULE_INTERFACE_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

//-------------------------------------------------CODEC------------------------------------
#ifndef XBH_CODEC_TABLE
#define XBH_CODEC_TABLE \
    0
#endif

#ifndef XBH_CODEC_OPT_TABLE
#define XBH_CODEC_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------G-SENSOR------------------------------------
#ifndef XBH_G_SENSOR_TABLE
#define XBH_G_SENSOR_TABLE \
    0
#endif

#ifndef XBH_G_SENSOR_OPT_TABLE
#define XBH_G_SENSOR_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------NFC------------------------------------
#ifndef XBH_NFC_TABLE
#define XBH_NFC_TABLE \
    0
#endif

#ifndef XBH_NFC_OPT_TABLE
#define XBH_NFC_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------HDMI SWITCH------------------------------------
#ifndef XBH_HDMI_SWITCH_TABLE
#define XBH_HDMI_SWITCH_TABLE \
    {0, 0, 0, 0}, \
    {0, 0, 0, 0}, \
    {0, 0, 0, 0}, \
    {0, 0, 0, 0}
#endif

#ifndef XBH_HDMI_SWITCH_IIC_TABLE
#define XBH_HDMI_SWITCH_IIC_TABLE \
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, \
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, \
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, \
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
#endif

#ifndef XBH_HDMI_SWITCH_PORT_TABLE
#define XBH_HDMI_SWITCH_PORT_TABLE \
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, \
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, \
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}, \
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}
#endif

//-------------------------------------------------DP 2 HDMI------------------------------------
#ifndef XBH_DP_2_HDMI_TABLE
#define XBH_DP_2_HDMI_TABLE \
    0, 0
#endif

#ifndef XBH_DP_2_HDMI_IIC_TABLE
#define XBH_DP_2_HDMI_IIC_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
#endif

#ifndef XBH_DP_PORT_TABLE
#define XBH_DP_PORT_TABLE \
    0, 0
#endif
//-------------------------------------------------USBC 2 HDMI------------------------------------
#ifndef XBH_USBC_2_HDMI_TABLE
#define XBH_USBC_2_HDMI_TABLE \
    0, 0, 0
#endif

#ifndef XBH_USBC_2_HDMI_IIC_TABLE
#define XBH_USBC_2_HDMI_IIC_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
#endif

#ifndef XBH_USBC_PORT_TABLE
#define XBH_USBC_PORT_TABLE \
    0, 0, 0
#endif
//-------------------------------------------------VGA 2 HDMI------------------------------------
#ifndef XBH_VGA_2_HDMI_TABLE
#define XBH_VGA_2_HDMI_TABLE \
    0
#endif

#ifndef XBH_VGA_2_HDMI_IIC_TABLE
#define XBH_VGA_2_HDMI_IIC_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif

#ifndef XBH_VGA_PORT_TABLE
#define XBH_VGA_PORT_TABLE \
    0
#endif
//-------------------------------------------------NET SWITCH------------------------------------
#ifndef XBH_NET_SWITCH_TABLE
#define XBH_NET_SWITCH_TABLE \
    0
#endif

#ifndef XBH_NET_SWITCH_OPT_TABLE
#define XBH_NET_SWITCH_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------AUDIO TO I2S------------------------------------
#ifndef XBH_AUDIO_TO_I2S_TABLE
#define XBH_AUDIO_TO_I2S_TABLE \
    0
#endif

#ifndef XBH_AUDIO_TO_I2S_OPT_TABLE
#define XBH_AUDIO_TO_I2S_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------LIGHT SENSOR------------------------------------
#ifndef XBH_LIGHT_SENSOR_OPT_TABLE
#define XBH_LIGHT_SENSOR_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------AIR SENSOR------------------------------------
#ifndef XBH_AIR_SENSOR_TABLE
#define XBH_AIR_SENSOR_TABLE \
    0
#endif

#ifndef XBH_AIR_SENSOR_OPT_TABLE
#define XBH_AIR_SENSOR_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------POWER SENSOR------------------------------------
#ifndef XBH_POWER_SENSOR_TABLE
#define XBH_POWER_SENSOR_TABLE \
    0
#endif

#ifndef XBH_POWER_SENSOR_OPT_TABLE
#define XBH_POWER_SENSOR_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------  MCU -------------------------------------------
#ifndef XBH_MCU_TABLE
#define XBH_MCU_TABLE \
    0
#endif

#ifndef XBH_MCU_OPT_TABLE
#define XBH_MCU_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif
//-------------------------------------------------CODEC------------------------------------
#ifndef XBH_AMP_TABLE
#define XBH_AMP_TABLE \
    0
#endif

#ifndef XBH_AMP_OPT_TABLE
#define XBH_AMP_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------VIDEO OUT------------------------------------
#ifndef XBH_VIDEO_OUT_TABLE
#define XBH_VIDEO_OUT_TABLE \
    0, 0, 0, 0
#endif

#ifndef XBH_VIDEO_OUT_IIC_TABLE
#define XBH_VIDEO_OUT_IIC_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------Rj45ToUsb------------------------------------
#ifndef XBH_RJ45_TO_USB_TABLE
#define XBH_RJ45_TO_USB_TABLE \
    0
#endif

#ifndef XBH_RJ45_TO_USB_OPT_TABLE
#define XBH_RJ45_TO_USB_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------PD IC------------------------------------
#ifndef XBH_PD_IC_TABLE
#define XBH_PD_IC_TABLE \
    0, 0, 0, 0
#endif

#ifndef XBH_PD_IC_IIC_TABLE
#define XBH_PD_IC_IIC_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------USB HUB------------------------------------
#ifndef XBH_USB_HUB_TABLE
#define XBH_USB_HUB_TABLE \
    0, 0
#endif

#ifndef XBH_USB_HUB_OPT_TABLE
#define XBH_USB_HUB_OPT_TABLE \
    {0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}
#endif

//-------------------------------------------------FOLLOW SOURCE------------------------------------
#ifndef REQUEST_GPIO
#define REQUEST_GPIO                0xF0
#endif
#ifndef REQUEST_GPIO_IN
#define REQUEST_GPIO_IN             0xF1
#endif
#ifndef REQUEST_DELAY
#define REQUEST_DELAY               0xFE
#endif

#define SELECT_GPIO_OPT_TABLE(tbl)    { \
        if (getFattMode() && GPIO_OPT_TABLE_SIZE(fct_##tbl) > 1) \
        { \
            select_table(fct_##tbl,GPIO_OPT_TABLE_SIZE(fct_##tbl)); \
            return XBH_SUCCESS; \
        } \
        select_table(tbl,GPIO_OPT_TABLE_SIZE(tbl)); \
    }

#ifndef XBH_PLUG_EVENT_TABLE
#define XBH_PLUG_EVENT_TABLE \
    {   }
#endif

#ifndef XBH_INIT_TABLE
#define XBH_INIT_TABLE \
    {   }
#endif

#ifndef XBH_ANDROID_TABLE
#define XBH_ANDROID_TABLE \
    {   }
#endif

#ifndef XBH_HDMI1_TABLE
#define XBH_HDMI1_TABLE \
    {   }
#endif

#ifndef XBH_HDMI2_TABLE
#define XBH_HDMI2_TABLE \
    {   }
#endif

#ifndef XBH_HDMI3_TABLE
#define XBH_HDMI3_TABLE \
    {   }
#endif

#ifndef XBH_HDMI4_TABLE
#define XBH_HDMI4_TABLE \
    {   }
#endif

#ifndef XBH_HDMI5_TABLE
#define XBH_HDMI5_TABLE \
    {   }
#endif

#ifndef XBH_HDMI6_TABLE
#define XBH_HDMI6_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI1_TABLE
#define XBH_F_HDMI1_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI2_TABLE
#define XBH_F_HDMI2_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI3_TABLE
#define XBH_F_HDMI3_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI4_TABLE
#define XBH_F_HDMI4_TABLE \
    {   }
#endif

#ifndef XBH_USBC1_TABLE
#define XBH_USBC1_TABLE \
    {   }
#endif

#ifndef XBH_USBC2_TABLE
#define XBH_USBC2_TABLE \
    {   }
#endif

#ifndef XBH_USBC3_TABLE
#define XBH_USBC3_TABLE \
    {   }
#endif

#ifndef XBH_USBC4_TABLE
#define XBH_USBC4_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC1_TABLE
#define XBH_F_USBC1_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC2_TABLE
#define XBH_F_USBC2_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC3_TABLE
#define XBH_F_USBC3_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC4_TABLE
#define XBH_F_USBC4_TABLE \
    {   }
#endif

#ifndef XBH_DP1_TABLE
#define XBH_DP1_TABLE \
    {   }
#endif

#ifndef XBH_DP2_TABLE
#define XBH_DP2_TABLE \
    {   }
#endif

#ifndef XBH_DP3_TABLE
#define XBH_DP3_TABLE \
    {   }
#endif

#ifndef XBH_DP4_TABLE
#define XBH_DP4_TABLE \
    {   }
#endif

#ifndef XBH_F_DP1_TABLE
#define XBH_F_DP1_TABLE \
    {   }
#endif

#ifndef XBH_F_DP2_TABLE
#define XBH_F_DP2_TABLE \
    {   }
#endif

#ifndef XBH_F_DP3_TABLE
#define XBH_F_DP3_TABLE \
    {   }
#endif

#ifndef XBH_F_DP4_TABLE
#define XBH_F_DP4_TABLE \
    {   }
#endif

#ifndef XBH_OPS1_TABLE
#define XBH_OPS1_TABLE \
    {   }
#endif

#ifndef XBH_OPS2_TABLE
#define XBH_OPS2_TABLE \
    {   }
#endif

#ifndef XBH_SDM1_TABLE
#define XBH_SDM1_TABLE \
    {   }
#endif

#ifndef XBH_SDM2_TABLE
#define XBH_SDM2_TABLE \
    {   }
#endif

#ifndef XBH_VGA1_TABLE
#define XBH_VGA1_TABLE \
    {   }
#endif

#ifndef XBH_VGA2_TABLE
#define XBH_VGA2_TABLE \
    {   }
#endif

/**
 * gpio table for fatt
*/
#ifndef XBH_ANDROID_FCT_TABLE
#define XBH_ANDROID_FCT_TABLE \
    {   }
#endif

#ifndef XBH_HDMI1_FCT_TABLE
#define XBH_HDMI1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_HDMI2_FCT_TABLE
#define XBH_HDMI2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_HDMI3_FCT_TABLE
#define XBH_HDMI3_FCT_TABLE \
    {   }
#endif

#ifndef XBH_HDMI4_FCT_TABLE
#define XBH_HDMI4_FCT_TABLE \
    {   }
#endif

#ifndef XBH_HDMI5_FCT_TABLE
#define XBH_HDMI5_FCT_TABLE \
    {   }
#endif

#ifndef XBH_HDMI6_FCT_TABLE
#define XBH_HDMI6_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI1_FCT_TABLE
#define XBH_F_HDMI1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI2_FCT_TABLE
#define XBH_F_HDMI2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI3_FCT_TABLE
#define XBH_F_HDMI3_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_HDMI4_FCT_TABLE
#define XBH_F_HDMI4_FCT_TABLE \
    {   }
#endif

#ifndef XBH_USBC1_FCT_TABLE
#define XBH_USBC1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_USBC2_FCT_TABLE
#define XBH_USBC2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_USBC3_FCT_TABLE
#define XBH_USBC3_FCT_TABLE \
    {   }
#endif

#ifndef XBH_USBC4_FCT_TABLE
#define XBH_USBC4_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC1_FCT_TABLE
#define XBH_F_USBC1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC2_FCT_TABLE
#define XBH_F_USBC2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC3_FCT_TABLE
#define XBH_F_USBC3_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_USBC4_FCT_TABLE
#define XBH_F_USBC4_FCT_TABLE \
    {   }
#endif

#ifndef XBH_DP1_FCT_TABLE
#define XBH_DP1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_DP2_FCT_TABLE
#define XBH_DP2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_DP3_FCT_TABLE
#define XBH_DP3_FCT_TABLE \
    {   }
#endif

#ifndef XBH_DP4_FCT_TABLE
#define XBH_DP4_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_DP1_FCT_TABLE
#define XBH_F_DP1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_DP2_FCT_TABLE
#define XBH_F_DP2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_DP3_FCT_TABLE
#define XBH_F_DP3_FCT_TABLE \
    {   }
#endif

#ifndef XBH_F_DP4_FCT_TABLE
#define XBH_F_DP4_FCT_TABLE \
    {   }
#endif

#ifndef XBH_OPS1_FCT_TABLE
#define XBH_OPS1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_OPS2_FCT_TABLE
#define XBH_OPS2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_SDM1_FCT_TABLE
#define XBH_SDM1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_SDM2_FCT_TABLE
#define XBH_SDM2_FCT_TABLE \
    {   }
#endif

#ifndef XBH_VGA1_FCT_TABLE
#define XBH_VGA1_FCT_TABLE \
    {   }
#endif

#ifndef XBH_VGA2_FCT_TABLE
#define XBH_VGA2_FCT_TABLE \
    {   }
#endif

class XbhModuleInterface
{
public:
    /**********************************************************************************************i2c/gpio********************************************************************************************/
    /**
     * 模块的IIC写入接口
     * param[in] u8Dev 器件类型
     * param[in] u32RegAddr 偏移地址
     * param[in] u32RegAddrCount 偏移地址长度
     * param[in] u32Length 写入数据的长度
     * param[in] u8Data 写入的数据
     * retval 0:success,-1:failure
    */
    XBH_S32 setRtcI2cData(XBH_U32 u8Dev, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    /**
     * 模块的读IIC取接口
     * param[in] u8Dev 器件类型
     * param[in] u32RegAddr 偏移地址
     * param[in] u32RegAddrCount 偏移地址长度
     * param[in] u32Length 读取数据的长度
     * param[in] u8Data 读取的数据
     * retval 0:success,-1:failure
    */
    XBH_S32 getRtcI2cData(XBH_U32 u8Dev, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    /**
     * 模块的IIC写入接口,如果主板上存在多个相同的IC，使用此接口
     * 此接口在iic被锁住的时候会retry的间隔为1S。
     * param[in] u32I2cNum iic bus
     * param[in] u8DevAddress 设备地址
     * param[in] u32RegAddr 设备寄存器地址
     * param[in] u32RegAddrCount 设备寄存器长度
     * param[in] u32Length 写入的数据长度
     * param[in] u8Data 写入的数据
     * retval 0:success,-1:failure
    */
    XBH_S32 setI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
    /**
     * 模块的IIC读取取接口,如果主板上存在多个相同的IC，使用此接口
     * 此接口在iic被锁住的时候会retry的间隔为1S。
     * param[in] u32I2cNum iic bus
     * param[in] u8DevAddress 设备地址
     * param[in] u32RegAddr 设备寄存器地址
     * param[in] u32RegAddrCount 设备寄存器长度
     * param[in] u32Length 读取的数据长度
     * param[out] u8Data 读取的数据
     * retval 0:success,-1:failure
    */
    XBH_S32 getI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);
    /**
     * 模块的设置IO输出的电平
     * param[in] u32GpioNumber IO引脚的序号
     * param[in] u32GpioValue 电平值 0：低电平，1：高电平
     * retval 0:success,-1:failure
    */
    XBH_S32 setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
    /**
     * 获取模块的IO输出的电平
     * param[in] u32GpioNumber IO引脚的序号
     * param[in] u32GpioValue 电平值 0：低电平，1：高电平
     * retval 0:success,-1:failure
    */
    XBH_S32 getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue);
    /**
     * 模块的设置IO输入的电平
     * param[in] u32GpioNumber IO引脚的序号
     * param[out] u32GpioValue 电平值 0：低电平，1：高电平
     * retval 0:success,-1:failure
    */
    XBH_S32 getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue);
    /**
     * 模块的IIC读取取接口,如果主板上存在多个相同的IC，使用此接口
     * 此接口在iic被锁住的时候会retry的间隔为1S。
     * param[in] u32I2cNum iic bus
     * param[in] u8DevAddress 设备地址
     * param[in] u32Length 读取的数据长度
     * param[out] u8Data 读取的数据
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 readI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32Length, XBH_U8* u8Data);
    /**
     * 模块的VGA EDID模拟IIC写入接口
     * param[in] u32RegAddr 设备寄存器地址
     * param[in] u8Data 写入的数据
     * param[in] u32Length 写入的数据长度
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length);
    /**
     * 模块的VGA EDID模拟IIC读取接口
     * param[in] u32RegAddr 设备寄存器地址
     * param[in] u8Data 读取的数据
     * param[in] u32Length 读取的数据长度
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length);
    /**
     * 模块的设置GPIO的中断
     * param[in] u32GpioNumber IO引脚的序号
     * param[in] u32Type 中断方式
                IRQF_TRIGGER_FALLING        0x01//:下降沿触发
                IRQF_TRIGGER_RISING         0x02//:上升沿触发
                IRQF_TRIGGER_HIGH           0x04//:高电平触发
                IRQF_TRIGGER_LOW            0x08//:低电平触发
     * param[in] enable 		IO引脚是否需要消抖
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 requestGpioIrq(XBH_U32 u32GpioNumber, XBH_U32 u32Type, XBH_BOOL enable = XBH_FALSE);

    /**********************************************************************************************others************************************************************************************************/
    /**
     * 获取有线网的MAC地址
     * param[in] strPath
     *          内置MAC地址的设备节点
     * param[out] strMacAddress 有线网的MAC地址
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* strMacAddress);
    /**
     * 同步Android指定存储地址到线网的MAC地址
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 syncBuildinEthMacAddress(void);
    /**
    * 保存keypad信息
    * keypad_enable keypad使能状态
    * key_count     keypad按键数量
    * key_index     keypad当前按下按键的序号
    */
    virtual XBH_S32 saveFattKeypadInfo(XBH_S32 keypad_enable, XBH_S32 key_count, XBH_S32 key_index);
    /**
     * 获取RJ45的连接状态
     * param[in] port RJ45端口
     * retval true:success,flase:failure
    */
    virtual XBH_S32 getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus);
    /**
    * 设置RJ45灯的状态
    * param[in] state 灯状态
    * retval true:on,flase:off
    */
    virtual XBH_S32 setRJ45LEDStatus(XBH_BOOL state);
    /**
    * 设置前置type-c复位VBUS
    * param[in] src 当前信源
    * retval true:success,flase:failure
    */
    virtual XBH_S32 setTypecReset(XBH_SOURCE_E src);
    /**
    * 处理type-b信源下热拔插事件
    * param[in] src 当前信源
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 ProcessTypeBHotplug(XBH_SOURCE_E src);
    /**
    * 处理type-c信源下热拔插事件
    * param[in] src 当前信源
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 ProcessTypeCHotplug(XBH_SOURCE_E src);
    /**********************************************************************************************source************************************************************************************************/
    /**
     * 获取当前信源是否被支持
     * param[out] enable 信号支持状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable);
    /**
     * 获取HDMI 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    /**
     * 获取HDMI 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi2Det(XBH_BOOL *enable);
    /**
     * 获取HDMI 3的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi3Det(XBH_BOOL *enable);
    /**
     * 获取HDMI 4的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi4Det(XBH_BOOL *enable);
    /**
     * 获取HDMI 5的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi5Det(XBH_BOOL *enable);
    /**
     * 获取HDMI 6的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi6Det(XBH_BOOL *enable);
    /**
     * 获取Front HDMI 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi1Det(XBH_BOOL *enable);
    /**
     * 获取Front HDMI 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi2Det(XBH_BOOL *enable);
    /**
     * 获取Front HDMI 3的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi3Det(XBH_BOOL *enable);
    /**
     * 获取Front HDMI 4的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi4Det(XBH_BOOL *enable);
    /**
     * 获取Usbc 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc1Det(XBH_BOOL *enable);
    /**
     * 获取Usbc 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc2Det(XBH_BOOL *enable);
    /**
     * 获取Usbc 3的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc3Det(XBH_BOOL *enable);
    /**
     * 获取Usbc 4的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc4Det(XBH_BOOL *enable);
    /**
     * 获取Front Usbc 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc1Det(XBH_BOOL *enable);
    /**
     * 获取Front Usbc 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc2Det(XBH_BOOL *enable);
    /**
     * 获取Front Usbc 3的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc3Det(XBH_BOOL *enable);
    /**
     * 获取Front Usbc 4的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc4Det(XBH_BOOL *enable);
    /**
     * 获取DP 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP1Det(XBH_BOOL *enable);
    /**
     * 获取DP 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP2Det(XBH_BOOL *enable);
    /**
     * 获取DP 3的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP3Det(XBH_BOOL *enable);
    /**
     * 获取DP 4的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP4Det(XBH_BOOL *enable);
    /**
     * 获取Front DP 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP1Det(XBH_BOOL *enable);
    /**
     * 获取Front DP 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP2Det(XBH_BOOL *enable);
    /**
     * 获取Front DP 3的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP3Det(XBH_BOOL *enable);
    /**
     * 获取Front DP 4的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP4Det(XBH_BOOL *enable);
    /**
     * 获取OPS 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOps1Det(XBH_BOOL *enable);
    /**
     * 获取OPS 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOps2Det(XBH_BOOL *enable);
    /**
     * 获取SDM 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSdm1Det(XBH_BOOL *enable);
    /**
     * 获取SDM 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSdm2Det(XBH_BOOL *enable);
    /**
     * 获取VGA 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVga1Det(XBH_BOOL *enable);
    /**
     * 获取VGA 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVga2Det(XBH_BOOL *enable);
    /**
     * 获取CVBS 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCvbs1Det(XBH_BOOL *enable);
    /**
     * 获取CVBS 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCvbs2Det(XBH_BOOL *enable);
    /**
     * 获取out 1的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOut1Det(XBH_BOOL *enable);
    /**
     * 获取out 2的接入状态
     * param[out] enable 信号接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOut2Det(XBH_BOOL *enable);
    /**
     * 获取HDMI 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi1SignalStatus(XBH_S32 *status);
    /**
     * 获取HDMI 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi2SignalStatus(XBH_S32 *status);
    /**
     * 获取HDMI 3的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi3SignalStatus(XBH_S32 *status);
    /**
     * 获取HDMI 4的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi4SignalStatus(XBH_S32 *status);
    /**
     * 获取HDMI 5的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi5SignalStatus(XBH_S32 *status);
    /**
     * 获取HDMI 6的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmi6SignalStatus(XBH_S32 *status);
    /**
     * 获取Front HDMI 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi1SignalStatus(XBH_S32 *status);
    /**
     * 获取Front HDMI 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi2SignalStatus(XBH_S32 *status);
    /**
     * 获取Front HDMI 3的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi3SignalStatus(XBH_S32 *status);
    /**
     * 获取Front HDMI 4的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFHdmi4SignalStatus(XBH_S32 *status);
    /**
     * 获取Usbc 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc1SignalStatus(XBH_S32 *status);
    /**
     * 获取Usbc 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc2SignalStatus(XBH_S32 *status);
    /**
     * 获取Usbc 3的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc3SignalStatus(XBH_S32 *status);
    /**
     * 获取Usbc 4的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getUsbc4SignalStatus(XBH_S32 *status);
    /**
     * 获取Front Usbc 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc1SignalStatus(XBH_S32 *status);
    /**
     * 获取Front Usbc 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc2SignalStatus(XBH_S32 *status);
    /**
     * 获取Front Usbc 3的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc3SignalStatus(XBH_S32 *status);
    /**
     * 获取Front Usbc 4的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFUsbc4SignalStatus(XBH_S32 *status);
    /**
     * 获取DP 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP1SignalStatus(XBH_S32 *status);
    /**
     * 获取DP 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP2SignalStatus(XBH_S32 *status);
    /**
     * 获取DP 3的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP3SignalStatus(XBH_S32 *status);
    /**
     * 获取DP 4的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getDP4SignalStatus(XBH_S32 *status);
    /**
     * 获取Front DP 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP1SignalStatus(XBH_S32 *status);
    /**
     * 获取Front DP 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP2SignalStatus(XBH_S32 *status);
    /**
     * 获取Front DP 3的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP3SignalStatus(XBH_S32 *status);
    /**
     * 获取Front DP 4的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getFDP4SignalStatus(XBH_S32 *status);
    /**
     * 获取OPS 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOps1SignalStatus(XBH_S32 *status);
    /**
     * 获取OPS 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getOps2SignalStatus(XBH_S32 *status);
    /**
     * 获取SDM 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSdm1SignalStatus(XBH_S32 *status);
    /**
     * 获取SDM 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSdm2SignalStatus(XBH_S32 *status);
    /**
     * 获取VGA 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVga1SignalStatus(XBH_S32 *status);
    /**
     * 获取VGA 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getVga2SignalStatus(XBH_S32 *status);
    /**
     * 获取CVBS 1的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCvbs1SignalStatus(XBH_S32 *status);
    /**
     * 获取CVBS 2的信号状态
     * param[out] enable 信号状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getCvbs2SignalStatus(XBH_S32 *status);
    /**
     * 获取SOC的HDMI EDID加载状态
     * param[out] enable edid加载成功
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSocHdmiEdidStatus(XBH_BOOL *enable);
     /**
     * 设置628驱动的HDMIRX 5v检测标志位
     * param[in] state HDMIRX 5v检测标志位， 0:off 1:on
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdmiRx5vDet(XBH_BOOL enable);
     /**
     * 设置低延时
     * param[in] ，enable  0:off 1:on
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setHdmiLowLatency(XBH_BOOL bEnable);
     /**
     * 设置信源的HDMIRX控制器状态
     * u32Source 根据具体的信号源来控制 
    */
    virtual XBH_S32 setMipiRxCsiControlState(XBH_SOURCE_E u32Source);
    /**
     * 初始化src det的GPIO的中断，只针对soc直连的gpio有效，如果通过扩展芯片(MCU)控制的GPIO则不适宜
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 initSrcDetGpioIrq();
    /**
     * 根据GPIO 获取对应的source 序号，只针对soc直连的gpio有效，如果通过扩展芯片控制的GPIO则不适宜
     * param[in] gpio det gpio
     * param[out] src 信源
     * param[out] bEnable 插入/拔出
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable);
    /**
     * 处理OPS的信号状态，比如OPS开机后没有信号输出
     * param[in] u32Source 信号源序号
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 processOpsSignalStatus(XBH_SOURCE_E u32Source);
    /**
     * 重新设置HDMI CEC的物理地址
     * 要支持CEC功能，必须保证每个HDMI的EDID的地址不一样
     * 0x1000, 0x2000, 0x3000, 0x4000,...
     * retval 0:success,-1:failure
    */
   virtual XBH_S32 initHdmiCecPhysicalAddr();
    /**
    * 根据Project_ID 动态调整  HDMI SWITCH  edid
    * retval 0:success,-1:failure
    */
   virtual XBH_S32 initProjectIdConfig();
   /**
    * 获取HDMI 指定端口的clock状态
    * param[in] port HDMI 端口
    * param[out] port 口clock的状态
    * retval 0:success,-1:failure
   */
   virtual XBH_S32 getHdmiPortClockStatus(XBH_U32 port, XBH_S32 *status);
    /**
    * 切换switch后，需要延时一定时间才能进行后面的动作
    * 比如rk3576 这种切换完switch后等一段时间再去拉高HPD
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 setDelayTimeForHdmiSwitch();
    /**
    * 获取HDMI RX lock的状态
    * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHdmiRxLockStatus(XBH_BOOL* lock);
/**********************************************************************************************audio detect************************************************************************************************/
    /**
     * 初始化audio mute的状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 initAudioMuteStatus();
    /**
     * 获取模拟mic的接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getMicDetectStatus(XBH_BOOL* status);
    /**
     * 获取headphones/line out的接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getHpDetectStatus(XBH_BOOL* status);
    /**
     * 获取HDMI ARC的接入状态
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 getArcDetectStatus(XBH_BOOL* status);
    /**
     * 获取功放端kernel的初始化状态
     * param[out] value 初始化状态
     * retval 1:success,0:failure
    */
    virtual XBH_S32 getAmpInitStatus(XBH_BOOL* value);
    /**********************************************************************************************follow************************************************************************************************/
    XBH_S32 select_table(GPIO_OPT_TABLE *table, XBH_U32 count);
    /**
     * 检测到usb拔插，实现相关的复位逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 usb_plug_event(XBH_VOID);
    /**
     * gpio的初始化逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToInit(XBH_VOID);
    /**
     * 跟随到Android，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToAndroid(XBH_VOID);
    /**
     * 跟随到HDMI1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToHdmi1(XBH_VOID);
    /**
     * 跟随到HDMI2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToHdmi2(XBH_VOID);
    /**
     * 跟随到HDMI3通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToHdmi3(XBH_VOID);
    /**
     * 跟随到HDMI4通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToHdmi4(XBH_VOID);
    /**
     * 跟随到HDMI5通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToHdmi5(XBH_VOID);
    /**
     * 跟随到HDMI6通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToHdmi6(XBH_VOID);
    /**
     * 跟随到Front HDMI1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFHdmi1(XBH_VOID);
    /**
     * 跟随到Front HDMI2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFHdmi2(XBH_VOID);
    /**
     * 跟随到Front HDMI3通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFHdmi3(XBH_VOID);
    /**
     * 跟随到Front HDMI4通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFHdmi4(XBH_VOID);
    /**
     * 跟随到Usbc 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToUsbc1(XBH_VOID);
    /**
     * 跟随到Usbc 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToUsbc2(XBH_VOID);
    /**
     * 跟随到Usbc 3通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToUsbc3(XBH_VOID);
    /**
     * 跟随到Usbc 4通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToUsbc4(XBH_VOID);
    /**
     * 跟随到Front Usbc 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFUsbc1(XBH_VOID);
    /**
     * 跟随到Front Usbc 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFUsbc2(XBH_VOID);
    /**
     * 跟随到Front Usbc 3通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFUsbc3(XBH_VOID);
    /**
     * 跟随到Front Usbc 4通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFUsbc4(XBH_VOID);
    /**
     * 跟随到DP1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToDP1(XBH_VOID);
    /**
     * 跟随到DP2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToDP2(XBH_VOID);
    /**
     * 跟随到DP3通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToDP3(XBH_VOID);
    /**
     * 跟随到DP4通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToDP4(XBH_VOID);
    /**
     * 跟随到Front DP 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFDP1(XBH_VOID);
    /**
     * 跟随到Front DP 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFDP2(XBH_VOID);
    /**
     * 跟随到Front DP 3通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFDP3(XBH_VOID);
    /**
     * 跟随到Front DP 4通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToFDP4(XBH_VOID);
    /**
     * 跟随到OPS 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToOps1(XBH_VOID);
    /**
     * 跟随到OPS 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToOps2(XBH_VOID);
    /**
     * 跟随到SDM 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToSdm1(XBH_VOID);
    /**
     * 跟随到SDM 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToSdm2(XBH_VOID);
    /**
     * 跟随到VGA 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToVga1(XBH_VOID);
    /**
     * 跟随到VGA 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToVga2(XBH_VOID);
    /**
     * 跟随到CVBS 1通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToCvbs1(XBH_VOID);
    /**
     * 跟随到CVBS 2通道，实现相关的切换逻辑
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 followToCvbs2(XBH_VOID);
/**********************************************************************************************hdmi switch************************************************************************************************/
    /**
     * 获取HDMI SWITCH的排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getHdmiSwitchTable(XBH_U8 list[][4]);
    /**
     * 获取HDMI SWITCH的IIC排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getHdmiSwitchI2cTable(HDMI_SW_OPT_TABLE list[][4]);
    /**
     * 获取HDMI SWITCH的port排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getHdmiSwitchPortTable(HDMI_SWITCH_PORT list[][4]);
    /**
     * 获取DP to HDMI的排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getDp2HdmiTable(XBH_U8 list[]);
    /**
     * 获取DP to HDMI的IIC排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getDp2HdmiI2cTable(COMMON_OPT_TABLE list[]);
    /**
     * 获取DP to HDMI的port排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getDpPortTable(XBH_SOURCE_E list[]);

    /**
     * 获取USBC to HDMI的排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getUsbc2HdmiTable(XBH_U8 list[]);
    /**
     * 获取USBC to HDMI的IIC排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getUsbc2HdmiI2cTable(COMMON_OPT_TABLE list[]);
    /**
     * 获取USBC to HDMI的port排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getUsbcPortTable(XBH_SOURCE_E list[]);

    /**
     * 获取VGA to HDMI的排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getVga2HdmiTable(XBH_U8 list[]);
    /**
     * 获取VGA to HDMI的IIC排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getVga2HdmiI2cTable(COMMON_OPT_TABLE list[]);
    /**
     * 获取VGA to HDMI的port排列table
     * retval 0:success,-1:failure
    */
    XBH_S32 getVgaPortTable(XBH_SOURCE_E list[]);

/**********************************************************************************************codec************************************************************************************************/
    /**
     * 获取codec的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getCodecTable(XBH_U8 list[]);
    /**
     * 获取codec的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getCodecOptTable(CODEC_OPT_TABLE list[]);
/**********************************************************************************************g-sensor************************************************************************************************/
    /**
     * 获取g-sensor的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getGSensorTable(XBH_U8 list[]);
    /**
     * 获取g-sensor的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getGSensorOptTable(COMMON_OPT_TABLE list[]);
/**********************************************************************************************net switch************************************************************************************************/
    /**
     * 获取net switch的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getNetSwitchTable(XBH_U8 list[]);
    /**
     * 获取net switch的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getNetSwitchOptTable(NET_SWITCH_OPT_TABLE list[]);
/**********************************************************************************************audio to i2s************************************************************************************************/
    /**
     * 获取AudioToI2的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getAudioToI2sTable(XBH_U8 list[]);
    /**
     * 获取AudioToI2的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getAudioToI2sOptTable(COMMON_OPT_TABLE list[]);

/**********************************************************************************************light sensor************************************************************************************************/
    /**
     * 获取light sensor的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getLightSensorOptTable(COMMON_OPT_TABLE list[]);
/**********************************************************************************************Air sensor************************************************************************************************/
    /**
     * 获取获取air sensor的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getAirSensorTable(XBH_U8 list[]);
    /**
     * 获取air sensor的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getAirSensorOptTable(COMMON_OPT_TABLE list[]);
/**********************************************************************************************NFC************************************************************************************************/
    /**
     * 获取获取light sensor的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getNfcTable(XBH_U8 list[]);
    /**
     * 获取light sensor的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getNfcOptTable(COMMON_OPT_TABLE list[]);
/**********************************************************************************************Power sensor************************************************************************************************/
    /**
     * 获取获取Power sensor的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getPowerSensorTable(XBH_U8 list[]);
    /**
     * 获取Power sensor的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getPowerSensorOptTable(COMMON_OPT_TABLE list[]);
/**********************************************************************************************MCU************************************************************************************************/
    /**
     * 获取获取Mcu的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getMcuTable(XBH_U8 list[]);
    /**
     * 获取Mcu的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getMcuOptTable(COMMON_OPT_TABLE list[]);
/**********************************************************************************************amp************************************************************************************************/
    /**
     * 获取amp的器件类型
     * retval 0:success,-1:failure
    */
    XBH_S32  getAmpTable(XBH_U8 list[]);
    /**
     * 获取amp的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getAmpOptTable(AMP_OPT_TABLE list[]);
/**********************************************************************************************video out************************************************************************************************/
    /**
     * 获取video out的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getVideoOutTable(XBH_U8 list[]);
    /**
     * 获取video out的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32  getVideoOutOptTable(COMMON_OPT_TABLE list[]);

/*********************************************************************************************rj45 to Usb************************************************************************************************/
    /**
     * 获取rj45 to Usb的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32 getRj45ToUsbTable(XBH_U8 list[]);
    /**
     * 获取rj45 to Usbt的器件的操作参数
     * retval 0:success,-1:failure
    */
    XBH_S32 getRj45ToUsbOptTable(COMMON_OPT_TABLE list[]);

/*********************************************************************************************PD************************************************************************************************/
    /**
        * 获取PD IC的器件的操作参数
        * retval 0:success,-1:failure
    */
    XBH_S32 getPdIcTable(XBH_U8 list[]);
    /**
        * 获取PD IC的器件的操作参数
        * retval 0:success,-1:failure
    */
    XBH_S32 getPdIcI2cTable(COMMON_OPT_TABLE list[]);

/*********************************************************************************************MISC************************************************************************************************/
    /**
     * 检查是否自动测试模式
     * retval true:yes,false:no
    */
    XBH_BOOL getFattMode(XBH_VOID);
    /**
     * 开启SOC Free Run功能, 使输出信号固定不变
     * retval 0:success,-1:failure
    */
    virtual XBH_S32 setSocFreeRun(XBH_BOOL enable);

/******************************************************************************************USB HUB******************************************************************************************/
    /**
        * 获取usb hub的器件的操作参数
        * retval 0:success,-1:failure
    */
    XBH_S32 getUsbHubTable(XBH_U8 list[]);
    /**
        * 获取usb hub的器件的操作参数
        * retval 0:success,-1:failure
    */
    XBH_S32 getUsbHubOptTable(COMMON_OPT_TABLE list[]);

public:
    XbhModuleInterface(){};
    virtual ~XbhModuleInterface(){};

    XBH_S32 mIsBootCompleted = 0;
    XBH_S32 mIsMiddlewareCompleted = 0;
    XBH_BOOL mMcuUpdateLock = false;    //默认是解锁状态
};

#endif //XBH_MODULE_INTERFACE_H
