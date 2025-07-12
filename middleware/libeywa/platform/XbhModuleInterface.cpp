#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhModuleInterface"

#include "XbhLog.h"
#include "XbhModuleInterface.h"
#include <hardware/board.h>
#include "XbhService.h"
#include "cutils/properties.h"
/**********************************************************************************************i2c/gpio*********************************************************************************************/
/**
 * 模块的IIC写入接口
 * param[in] u8Dev 器件地址
 * param[in] u32RegAddr 偏移地址
 * param[in] u32RegAddrCount 偏移地址长度
 * param[in] u32Length 写入数据的长度
 * param[in] u8Data 写入的数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setRtcI2cData(XBH_U32 u8Dev, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32I2cBus = 0;
    XBH_U8 u8DevAddress = 0;
    switch(u8Dev)
    {
        case HYM8563_IC:
            u32I2cBus = HYM8563_I2C_BUS;
            u8DevAddress = HYM8563_I2C_ADDR_WR;
            break;
        case PT7C4337_IC:
            u32I2cBus = PT7C4337_I2C_BUS;
            u8DevAddress = PT7C4337_I2C_ADDR_WR;
            break;
        default:
            break;
    }
    s32Ret = setI2cData(u32I2cBus, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return  s32Ret;
}
/**
 * 模块的IIC读取接口
 * param[in] u8Dev 器件地址
 * param[in] u32RegAddr 偏移地址
 * param[in] u32RegAddrCount 偏移地址长度
 * param[in] u32Length 读取数据的长度
 * param[in] u8Data 读取的数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getRtcI2cData(XBH_U32 u8Dev, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32I2cBus = 0;
    XBH_U8 u8DevAddress = 0;
    switch(u8Dev)
    {
        case HYM8563_IC:
            u32I2cBus = HYM8563_I2C_BUS;
            u8DevAddress = HYM8563_I2C_ADDR_WR;
            break;
        case PT7C4337_IC:
            u32I2cBus = PT7C4337_I2C_BUS;
            u8DevAddress = PT7C4337_I2C_ADDR_WR;
            break;
        default:
            break;
    }
    s32Ret = getI2cData(u32I2cBus, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return  s32Ret;
}
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
XBH_S32 XbhModuleInterface::setI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    for(int i = 0; i < 3; i++)
    {
        XBH_S32 ret = XbhService::getPlatformInterface()->setIICData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
        if(ret == XBH_SUCCESS)
        {
            return XBH_SUCCESS;
        }
        else if(ret == XBH_LOCKSTATUS)
        {
            usleep(1000 * 1000); //1000ms
        }
        else
        {
            usleep(100); //0.1ms
        }
    }
    return  s32Ret;
}
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
XBH_S32 XbhModuleInterface::getI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    for(int i = 0; i < 3; i++)
    {
        XBH_S32 ret = XbhService::getPlatformInterface()->getIICData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
        if(ret == XBH_SUCCESS)
        {
            return XBH_SUCCESS;
        }
        else if(ret == XBH_LOCKSTATUS)
        {
            usleep(1000 * 1000); //1000ms
        }
        else
        {
            usleep(100); //0.1ms
        }
    }
    return  s32Ret;
}
/**
 * 模块的IIC读取取接口(无寄存器地址参数)
 * 此接口在iic被锁住的时候会retry的间隔为1S。
 * param[in] u32I2cNum iic bus
 * param[in] u8DevAddress 设备地址
 * param[in] u32Length 读取的数据长度
 * param[out] u8Data 读取的数据
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::readI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32Length, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 模块的VGA EDID模拟IIC写入接口
 * param[in] u32RegAddr 设备寄存器地址
 * param[in] u8Data 写入的数据
 * param[in] u32Length 写入的数据长度
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setVgaEdidI2cData(XBH_U32 u32RegAddr, const XBH_U8* u8Data, XBH_U32 u32Length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 模块的VGA EDID模拟IIC读取接口
 * param[in] u32RegAddr 设备寄存器地址
 * param[in] u8Data 读取的数据
 * param[in] u32Length 读取的数据长度
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVgaEdidI2cData(XBH_U32 u32RegAddr, XBH_U8* u8Data, XBH_U32 u32Length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 模块的设置IO输出的电平
 * param[in] u32GpioNumber IO引脚的序号
 * param[in] u32GpioValue 电平值 0：低电平，1：高电平
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhService::getPlatformInterface()->setGpioOutputValue(u32GpioNumber, u32GpioValue);
    return  s32Ret;
}

/**
 * 模块的IO输出的电平
 * param[in] u32GpioNumber IO引脚的序号
 * param[in] u32GpioValue 电平值 0：低电平，1：高电平
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhService::getPlatformInterface()->getGpioOutputValue(u32GpioNumber, u32GpioValue);
    return  s32Ret;
}

/**
 * 模块的设置IO输入的电平
 * param[in] u32GpioNumber IO引脚的序号
 * param[out] u32GpioValue 电平值 0：低电平，1：高电平
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhService::getPlatformInterface()->getGpioInputValue(u32GpioNumber, u32GpioValue);
    return  s32Ret;
}

/**
 * 模块的设置GPIO的中断
 * param[in] u32GpioNumber IO引脚的序号
 * param[in] u32Type 中断方式
            IRQF_TRIGGER_RISING         0x01//:上升沿触发
            IRQF_TRIGGER_FALLING        0x02//:下降沿触发
            IRQF_TRIGGER_HIGH           0x04//:高电平触发
            IRQF_TRIGGER_LOW            0x08//:低电平触发
 * param[in] enable 		IO引脚是否需要消抖
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::requestGpioIrq(XBH_U32 u32GpioNumber, XBH_U32 u32Type, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**********************************************************************************************others************************************************************************************************/
/**
 * 同步Android指定存储地址到线网的MAC地址
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::syncBuildinEthMacAddress(void)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取有线网的MAC地址
 * param[in] strPath
 *          内置MAC地址的设备节点
 * param[out] strMacAddress 有线网的MAC地址
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getBuildinEthMacAddress(XBH_CHAR* strPath, XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 保存keypad信息
 * keypad_enable keypad使能状态
 * key_count     keypad按键数量
 * key_index     keypad当前按下按键的序号
 */
XBH_S32 XbhModuleInterface::saveFattKeypadInfo(XBH_S32 keypad_enable, XBH_S32 key_count, XBH_S32 key_index)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**********************************************************************************************source************************************************************************************************/
/**
 * 获取当前信源是否被支持
 * param[out] enable 信号支持状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSrcExsit(XBH_SOURCE_E src, XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI 3的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 4的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi4Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI 5的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi5Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI 6的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi6Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front HDMI 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front HDMI 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front HDMI 3的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front HDMI 4的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi4Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Usbc 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Usbc 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Usbc 3的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Usbc 4的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc4Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front Usbc 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front Usbc 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front Usbc 3的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front Usbc 4的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc4Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取DP 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取DP 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取DP 3的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取DP 4的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP4Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front DP 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front DP 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front DP 3的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP3Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取Front DP 4的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP4Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OPS 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取OPS 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getOps2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取SDM 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSdm1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取SDM 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSdm2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取VGA 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVga1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取VGA 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVga2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取CVBS 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getCvbs1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取CVBS 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getCvbs2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取out 1的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getOut1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取out 2的接入状态
 * param[out] enable 信号接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getOut2Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取HDMI 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 3的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi3SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 4的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi4SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 5的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi5SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 6的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmi6SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front HDMI 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front HDMI 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front HDMI 3的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi3SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front HDMI 4的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFHdmi4SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Usbc 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Usbc 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Usbc 3的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc3SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Usbc 4的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc4SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front Usbc 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front Usbc 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front Usbc 3的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc3SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front Usbc 4的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFUsbc4SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取DP 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取DP 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取DP 3的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP3SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取DP 4的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDP4SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front DP 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front DP 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front DP 3的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP3SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取Front DP 4的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getFDP4SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取OPS 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getOps1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取OPS 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getOps2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取SDM 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSdm1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取SDM 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSdm2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取VGA 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVga1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取VGA 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVga2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取CVBS 1的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getCvbs1SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取CVBS 2的信号状态
 * param[out] enable 信号状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getCvbs2SignalStatus(XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取SOC的HDMI EDID加载状态
 * param[out] enable edid加载成功
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSocHdmiEdidStatus(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置628驱动的HDMIRX 5v检测标志位
 * param[in] state HDMIRX 5v检测标志位， 0:off 1:on
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setHdmiRx5vDet(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 设置低延时
* param[in] ，enable  0:off 1:on
* retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setHdmiLowLatency(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置信源的MIPIRX控制器状态
 * param[in] state HDMIRX控制器
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setMipiRxCsiControlState(XBH_SOURCE_E u32Source)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 初始化src det的GPIO的中断，只针对soc直连的gpio有效，如果通过扩展芯片控制的GPIO则不适宜
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::initSrcDetGpioIrq()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 根据GPIO 获取对应的source 序号，只针对soc直连的gpio有效，如果通过扩展芯片控制的GPIO则不适宜
 * param[in] gpio det gpio
 * param[out] src 信源
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getSrcDetByDetGpio(XBH_S32 gpio, XBH_U32 u32GpioValue, XBH_SOURCE_E *src, XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 处理OPS的信号状态，比如OPS开机后没有信号输出
 * param[in] u32Source 信号源序号
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::processOpsSignalStatus(XBH_SOURCE_E u32Source)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 重新设置HDMI CEC的物理地址
 * 要支持CEC功能，必须保证每个HDMI的EDID的地址不一样
 * 0x1000, 0x2000, 0x3000, 0x4000,...
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 根据Project_ID 动态调整  HDMI SWITCH  edid
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::initProjectIdConfig()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取RJ45的连接状态
 * param[in] port RJ45端口
 * retval true:success,flase:failure
*/
XBH_S32 XbhModuleInterface::getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 设置RJ45灯的状态
 * param[in] state 灯状态
 * retval true:on,flase:off
*/
XBH_S32 XbhModuleInterface::setRJ45LEDStatus(XBH_BOOL state)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 设置前置type-c复位VBUS
* param[in] src 当前信源
* retval true:success,flase:failure
*/
XBH_S32 XbhModuleInterface::setTypecReset(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 处理type-b信源下热拔插事件
* param[in] src 当前信源
* retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::ProcessTypeBHotplug(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 处理type-c信源下热拔插事件
* param[in] src 当前信源
* retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::ProcessTypeCHotplug(XBH_SOURCE_E src)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI 指定端口的clock状态
 * param[in] port HDMI 端口
 * param[out] port 口clock的状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmiPortClockStatus(XBH_U32 port, XBH_S32 *status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
* 切换switch后，需要延时一定时间才能进行后面的动作
* 比如rk3576 这种切换完switch后等一段时间再去拉高HPD
* retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setDelayTimeForHdmiSwitch()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
* 获取HDMI RX lock的状态
* retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmiRxLockStatus(XBH_BOOL* lock)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**********************************************************************************************audio detect************************************************************************************************/
/**
 * 初始化audio mute的状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::initAudioMuteStatus()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取模拟mic的接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取headphones/line out的接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取HDMI ARC的接入状态
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 获取功放端kernel的初始化状态
 * param[out] value 初始化状态
 * retval 1:success,0:failure
*/
XBH_S32 XbhModuleInterface::getAmpInitStatus(XBH_BOOL* value)
{

    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return s32Ret;
}

/**********************************************************************************************follow************************************************************************************************/
XBH_S32 XbhModuleInterface::select_table(GPIO_OPT_TABLE *table, XBH_U32 count)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    XBH_U32 i = 0;
    XBH_U32 cmd = 0;
    XBH_U32 u32GpioValue = 0;
    for(i = 0; i < count; i++)
    {
        cmd = table[i].cmd;
        switch(cmd)
        {
            case REQUEST_GPIO_IN:
                if(table[i].param_list[0] < 0)
                {
                    break;
                }
                s32Ret = XbhService::getPlatformInterface() ->getGpioInputValue(table[i].param_list[0], &u32GpioValue);
                if(s32Ret == XBH_FAILURE){
                    XLOGE("exec %d cmd %d , value %d %d %d fail\n", i, cmd, table[i].param_list[0], table[i].param_list[1], table[i].param_list[2]);
                    return XBH_FALSE;
                }
                break;
            case REQUEST_GPIO:
                if(table[i].param_list[0] < 0)
                {
                    break;
                }
                s32Ret = XbhService::getPlatformInterface() ->setGpioOutputValue(table[i].param_list[0], table[i].param_list[1]);
                if(s32Ret == XBH_FAILURE){
                    XLOGE("exec %d cmd %d , value %d %d %d fail\n", i, cmd, table[i].param_list[0], table[i].param_list[1], table[i].param_list[2]);
                    return XBH_FALSE;
                }
                break;
            case REQUEST_DELAY:
                usleep(table[i].param_list[0]);
                break;
        }
    }
    return XBH_SUCCESS;
}

static GPIO_OPT_TABLE source_usb_plug_event_table[] = {
    XBH_PLUG_EVENT_TABLE
};

static GPIO_OPT_TABLE source_init_table[] = {
    XBH_INIT_TABLE
};

static GPIO_OPT_TABLE source_android_table[] = {
    XBH_ANDROID_TABLE
};

static GPIO_OPT_TABLE source_hdmi1_table[] = {
    XBH_HDMI1_TABLE
};

static GPIO_OPT_TABLE source_hdmi2_table[] = {
    XBH_HDMI2_TABLE
};

static GPIO_OPT_TABLE source_hdmi3_table[] = {
    XBH_HDMI3_TABLE
};

static GPIO_OPT_TABLE source_hdmi4_table[] = {
    XBH_HDMI4_TABLE
};

static GPIO_OPT_TABLE source_hdmi5_table[] = {
    XBH_HDMI5_TABLE
};

static GPIO_OPT_TABLE source_hdmi6_table[] = {
    XBH_HDMI6_TABLE
};

static GPIO_OPT_TABLE source_f_hdmi1_table[] = {
    XBH_F_HDMI1_TABLE
};

static GPIO_OPT_TABLE source_f_hdmi2_table[] = {
    XBH_F_HDMI2_TABLE
};

static GPIO_OPT_TABLE source_f_hdmi3_table[] = {
    XBH_F_HDMI3_TABLE
};

static GPIO_OPT_TABLE source_f_hdmi4_table[] = {
    XBH_F_HDMI4_TABLE
};

static GPIO_OPT_TABLE source_usbc1_table[] = {
    XBH_USBC1_TABLE
};

static GPIO_OPT_TABLE source_usbc2_table[] = {
    XBH_USBC2_TABLE
};

static GPIO_OPT_TABLE source_usbc3_table[] = {
    XBH_USBC3_TABLE
};

static GPIO_OPT_TABLE source_usbc4_table[] = {
    XBH_USBC4_TABLE
};

static GPIO_OPT_TABLE source_f_usbc1_table[] = {
    XBH_F_USBC1_TABLE
};

static GPIO_OPT_TABLE source_f_usbc2_table[] = {
    XBH_F_USBC2_TABLE
};

static GPIO_OPT_TABLE source_f_usbc3_table[] = {
    XBH_F_USBC3_TABLE
};

static GPIO_OPT_TABLE source_f_usbc4_table[] = {
    XBH_F_USBC4_TABLE
};

static GPIO_OPT_TABLE source_dp1_table[] = {
    XBH_DP1_TABLE
};

static GPIO_OPT_TABLE source_dp2_table[] = {
    XBH_DP2_TABLE
};

static GPIO_OPT_TABLE source_dp3_table[] = {
    XBH_DP3_TABLE
};

static GPIO_OPT_TABLE source_dp4_table[] = {
    XBH_DP4_TABLE
};

static GPIO_OPT_TABLE source_f_dp1_table[] = {
    XBH_F_DP1_TABLE
};

static GPIO_OPT_TABLE source_f_dp2_table[] = {
    XBH_F_DP2_TABLE
};

static GPIO_OPT_TABLE source_f_dp3_table[] = {
    XBH_F_DP3_TABLE
};

static GPIO_OPT_TABLE source_f_dp4_table[] = {
    XBH_F_DP4_TABLE
};

static GPIO_OPT_TABLE source_ops1_table[] = {
    XBH_OPS1_TABLE
};

static GPIO_OPT_TABLE source_ops2_table[] = {
    XBH_OPS2_TABLE
};

static GPIO_OPT_TABLE source_sdm1_table[] = {
    XBH_SDM1_TABLE
};

static GPIO_OPT_TABLE source_sdm2_table[] = {
    XBH_SDM2_TABLE
};

static GPIO_OPT_TABLE source_vga1_table[] = {
    XBH_VGA1_TABLE
};

static GPIO_OPT_TABLE source_vga2_table[] = {
    XBH_VGA2_TABLE
};

/**
 * gpio table for fatt
*/
static GPIO_OPT_TABLE fct_source_android_table[] = {
    XBH_ANDROID_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_hdmi1_table[] = {
    XBH_HDMI1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_hdmi2_table[] = {
    XBH_HDMI2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_hdmi3_table[] = {
    XBH_HDMI3_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_hdmi4_table[] = {
    XBH_HDMI4_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_hdmi5_table[] = {
    XBH_HDMI5_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_hdmi6_table[] = {
    XBH_HDMI6_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table[] = {
    XBH_F_HDMI1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table[] = {
    XBH_F_HDMI2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_hdmi3_table[] = {
    XBH_F_HDMI3_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_hdmi4_table[] = {
    XBH_F_HDMI4_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_usbc1_table[] = {
    XBH_USBC1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_usbc2_table[] = {
    XBH_USBC2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_usbc3_table[] = {
    XBH_USBC3_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_usbc4_table[] = {
    XBH_USBC4_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table[] = {
    XBH_F_USBC1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table[] = {
    XBH_F_USBC2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_usbc3_table[] = {
    XBH_F_USBC3_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_usbc4_table[] = {
    XBH_F_USBC4_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_dp1_table[] = {
    XBH_DP1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_dp2_table[] = {
    XBH_DP2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_dp3_table[] = {
    XBH_DP3_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_dp4_table[] = {
    XBH_DP4_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_dp1_table[] = {
    XBH_F_DP1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_dp2_table[] = {
    XBH_F_DP2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_dp3_table[] = {
    XBH_F_DP3_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_f_dp4_table[] = {
    XBH_F_DP4_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_ops1_table[] = {
    XBH_OPS1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_ops2_table[] = {
    XBH_OPS2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_sdm1_table[] = {
    XBH_SDM1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_sdm2_table[] = {
    XBH_SDM2_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_vga1_table[] = {
    XBH_VGA1_FCT_TABLE
};

static GPIO_OPT_TABLE fct_source_vga2_table[] = {
    XBH_VGA2_FCT_TABLE
};

/**
* 检测到usb拔插，实现相关的复位逻辑
* retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::usb_plug_event(XBH_VOID)
{
    select_table(source_usb_plug_event_table,GPIO_OPT_TABLE_SIZE(source_usb_plug_event_table));
    return XBH_SUCCESS;
}

/**
 * gpio的初始化逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToInit(XBH_VOID)
{
    select_table(source_init_table,GPIO_OPT_TABLE_SIZE(source_init_table));
    return XBH_SUCCESS;
}

/**
 * 跟随到Android，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToAndroid(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_android_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到HDMI1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToHdmi1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_hdmi1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到HDMI2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToHdmi2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_hdmi2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到HDMI3通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToHdmi3(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_hdmi3_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到HDMI4通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToHdmi4(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_hdmi4_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到HDMI5通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToHdmi5(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_hdmi5_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到HDMI6通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToHdmi6(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_hdmi6_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front HDMI1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFHdmi1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_hdmi1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front HDMI2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFHdmi2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_hdmi2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front HDMI3通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFHdmi3(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_hdmi3_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front HDMI4通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFHdmi4(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_hdmi4_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Usbc 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToUsbc1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_usbc1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Usbc 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToUsbc2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_usbc2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Usbc 3通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToUsbc3(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_usbc3_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Usbc 4通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToUsbc4(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_usbc4_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front Usbc 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFUsbc1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_usbc1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front Usbc 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFUsbc2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_usbc2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front Usbc 3通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFUsbc3(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_usbc3_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front Usbc 4通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFUsbc4(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_usbc4_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到DP1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToDP1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_dp1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到DP2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToDP2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_dp2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到DP3通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToDP3(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_dp3_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到DP4通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToDP4(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_dp4_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front DP 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFDP1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_dp1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front DP 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFDP2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_dp2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front DP 3通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFDP3(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_dp3_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到Front DP 4通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToFDP4(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_f_dp4_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到OPS 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToOps1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_ops1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到OPS 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToOps2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_ops2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到SDM 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToSdm1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_sdm1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到SDM 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToSdm2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_sdm2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到VGA 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToVga1(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_vga1_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到VGA 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToVga2(XBH_VOID)
{
    SELECT_GPIO_OPT_TABLE(source_vga2_table);
    return XBH_SUCCESS;
}

/**
 * 跟随到CVBS 1通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToCvbs1(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/**
 * 跟随到CVBS 2通道，实现相关的切换逻辑
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::followToCvbs2(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/****************************************************************************************hdmi switch/convert****************************************************************************************/
/**
 * 获取HDMI SWITCH的排列table
*/
static XBH_U8 switch_table[][4] = {
    XBH_HDMI_SWITCH_TABLE
};

static HDMI_SW_OPT_TABLE switch_iic_table[][4] = {
    XBH_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT switch_port_table[][4] = {
    XBH_HDMI_SWITCH_PORT_TABLE
};

/**
 * 获取Dp to Hdmi的排列table
*/
static XBH_U8 dp_to_hdmi_table[] = {
    XBH_DP_2_HDMI_TABLE
};

static COMMON_OPT_TABLE dp_to_hdmi_iic_table[] = {
    XBH_DP_2_HDMI_IIC_TABLE
};

static XBH_U8 dp_port_table[] = {
    XBH_DP_PORT_TABLE
};

/**
 * 获取usbc to Hdmi的排列table
*/
static XBH_U8 usbc_to_hdmi_table[] = {
    XBH_USBC_2_HDMI_TABLE
};

static COMMON_OPT_TABLE usbc_to_hdmi_iic_table[] = {
    XBH_USBC_2_HDMI_IIC_TABLE
};

static XBH_U8 usbc_port_table[] = {
    XBH_USBC_PORT_TABLE
};

/**
 * 获取vga to Hdmi的排列table
*/
static XBH_U8 vga_to_hdmi_table[] = {
    XBH_VGA_2_HDMI_TABLE
};

static COMMON_OPT_TABLE vga_to_hdmi_iic_table[] = {
    XBH_VGA_2_HDMI_IIC_TABLE
};

static XBH_U8 vga_port_table[] = {
    XBH_VGA_PORT_TABLE
};

XBH_S32 XbhModuleInterface::getHdmiSwitchTable(XBH_U8 list[][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            list[i][j] = switch_table[i][j];
        }
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            XLOGV("switchlist[%d][%d] = 0x%02x ",i, j, list[i][j]);
        }
    }
    return XBH_SUCCESS;
}

/**
 * 获取HDMI SWITCH的IIC排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmiSwitchI2cTable(HDMI_SW_OPT_TABLE list[][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            list[i][j].iicBus = switch_iic_table[i][j].iicBus;
            list[i][j].iicAddr = switch_iic_table[i][j].iicAddr;
            list[i][j].sGpio = switch_iic_table[i][j].sGpio;
            list[i][j].sLevel = switch_iic_table[i][j].sLevel;
            list[i][j].pGpio = switch_iic_table[i][j].pGpio;
            list[i][j].pLevel = switch_iic_table[i][j].pLevel;
            list[i][j].rGpio = switch_iic_table[i][j].rGpio;
            list[i][j].rLevel = switch_iic_table[i][j].rLevel;
            list[i][j].defaultPort = switch_iic_table[i][j].defaultPort;
            list[i][j].prePort = switch_iic_table[i][j].prePort;
            list[i][j].sGpio2 = switch_iic_table[i][j].sGpio2;
            list[i][j].sLevel2 = switch_iic_table[i][j].sLevel2;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            XLOGV("switch_iic_list[%d][%d].iicBus = 0x%02x ",i, j, list[i][j].iicBus);
            XLOGV("switch_iic_list[%d][%d].iicAddr = 0x%02x ",i, j, list[i][j].iicAddr);
            XLOGV("switch_iic_list[%d][%d].sGpio = 0x%02x ",i, j, list[i][j].sGpio);
            XLOGV("switch_iic_list[%d][%d].sLevel = 0x%02x ",i, j, list[i][j].sLevel);
            XLOGV("switch_iic_list[%d][%d].pGpio = 0x%02x ",i, j, list[i][j].pGpio);
            XLOGV("switch_iic_list[%d][%d].pLevel = 0x%02x ",i, j, list[i][j].pLevel);
            XLOGV("switch_iic_list[%d][%d].rGpio = 0x%02x ",i, j, list[i][j].rGpio);
            XLOGV("switch_iic_list[%d][%d].rLevel = 0x%02x ",i, j, list[i][j].rLevel);
            XLOGV("switch_iic_list[%d][%d].defaultPort = 0x%02x ",i, j, list[i][j].defaultPort);
            XLOGV("switch_iic_list[%d][%d].prePort = 0x%02x ",i, j, list[i][j].prePort);
            XLOGV("switch_iic_list[%d][%d].sGpio2 = 0x%02x ",i, j, list[i][j].sGpio2);
            XLOGV("switch_iic_list[%d][%d].sLevel2 = 0x%02x ",i, j, list[i][j].sLevel2);
        }
    }
    return XBH_SUCCESS;
}

/**
 * 获取HDMI SWITCH的port排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getHdmiSwitchPortTable(HDMI_SWITCH_PORT list[][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            list[i][j].port0 = switch_port_table[i][j].port0;
            list[i][j].port1 = switch_port_table[i][j].port1;
            list[i][j].port2 = switch_port_table[i][j].port2;
            list[i][j].port3 = switch_port_table[i][j].port3;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            XLOGV("switch_port_list[%d][%d].port0 = %d ",i, j, list[i][j].port0);
            XLOGV("switch_port_list[%d][%d].port1 = %d ",i, j, list[i][j].port1);
            XLOGV("switch_port_list[%d][%d].port2 = %d ",i, j, list[i][j].port2);
            XLOGV("switch_port_list[%d][%d].port3 = %d ",i, j, list[i][j].port3);
        }
    }
    return XBH_SUCCESS;
}

/**
 * 获取DP to HDMI的排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDp2HdmiTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(dp_to_hdmi_table) / sizeof(dp_to_hdmi_table[0]); i ++)
    {
        list[i] = dp_to_hdmi_table[i];
    }

    for (int i = 0; i < sizeof(dp_to_hdmi_table) / sizeof(dp_to_hdmi_table[0]); i ++)
    {
        XLOGV("dp_to_hdmi_list[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取DP to HDMI的IIC排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDp2HdmiI2cTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(dp_to_hdmi_iic_table) / sizeof(dp_to_hdmi_iic_table[0]); i++)
    {
        list[i].iicBus1 = dp_to_hdmi_iic_table[i].iicBus1;
        list[i].iicBus2 = dp_to_hdmi_iic_table[i].iicBus2;
        list[i].iicAddr = dp_to_hdmi_iic_table[i].iicAddr;
        list[i].uart = dp_to_hdmi_iic_table[i].uart;
        list[i].pGpio = dp_to_hdmi_iic_table[i].pGpio;
        list[i].pLevel = dp_to_hdmi_iic_table[i].pLevel;
        list[i].rGpio = dp_to_hdmi_iic_table[i].rGpio;
        list[i].rLevel = dp_to_hdmi_iic_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(dp_to_hdmi_iic_table) / sizeof(dp_to_hdmi_iic_table[0]); i++)
    {
        XLOGV("dp_to_hdmi_iic_list[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("dp_to_hdmi_iic_list[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("dp_to_hdmi_iic_list[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("dp_to_hdmi_iic_list[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("dp_to_hdmi_iic_list[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("dp_to_hdmi_iic_list[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("dp_to_hdmi_iic_list[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("dp_to_hdmi_iic_list[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/**
 * 获取DP to HDMI 的port排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getDpPortTable(XBH_SOURCE_E list[])
{
    for (int i = 0; i < sizeof(dp_port_table) / sizeof(dp_port_table[0]); i++)
    {
        list[i] = (XBH_SOURCE_E)dp_port_table[i];
    }

    for (int i = 0; i < sizeof(dp_port_table) / sizeof(dp_port_table[0]); i++)
    {
        XLOGV("dp_port_list[%d] = %d ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取USBC to HDMI的排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc2HdmiTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(usbc_to_hdmi_table) / sizeof(usbc_to_hdmi_table[0]); i ++)
    {
        list[i] = usbc_to_hdmi_table[i];
    }

    for (int i = 0; i < sizeof(usbc_to_hdmi_table) / sizeof(usbc_to_hdmi_table[0]); i ++)
    {
        XLOGV("usbc_to_hdmi_list[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取USBC to HDMI的IIC排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbc2HdmiI2cTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(usbc_to_hdmi_iic_table) / sizeof(usbc_to_hdmi_iic_table[0]); i++)
    {
        list[i].iicBus1 = usbc_to_hdmi_iic_table[i].iicBus1;
        list[i].iicBus2 = usbc_to_hdmi_iic_table[i].iicBus2;
        list[i].iicAddr = usbc_to_hdmi_iic_table[i].iicAddr;
        list[i].uart = usbc_to_hdmi_iic_table[i].uart;
        list[i].pGpio = usbc_to_hdmi_iic_table[i].pGpio;
        list[i].pLevel = usbc_to_hdmi_iic_table[i].pLevel;
        list[i].rGpio = usbc_to_hdmi_iic_table[i].rGpio;
        list[i].rLevel = usbc_to_hdmi_iic_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(usbc_to_hdmi_iic_table) / sizeof(usbc_to_hdmi_iic_table[0]); i++)
    {
        XLOGV("usbc_to_hdmi_iic_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("usbc_to_hdmi_iic_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("usbc_to_hdmi_iic_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("usbc_to_hdmi_iic_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("usbc_to_hdmi_iic_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("usbc_to_hdmi_iic_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("usbc_to_hdmi_iic_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("usbc_to_hdmi_iic_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/**
 * 获取USBC to HDMI 的port排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbcPortTable(XBH_SOURCE_E list[])
{
    for (int i = 0; i < sizeof(usbc_port_table) / sizeof(usbc_port_table[0]); i++)
    {
        list[i] = (XBH_SOURCE_E)usbc_port_table[i];
    }

    for (int i = 0; i < sizeof(usbc_port_table) / sizeof(usbc_port_table[0]); i++)
    {
        XLOGV("usbc_port_list[%d] = %d ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取VGA to HDMI的排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVga2HdmiTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(vga_to_hdmi_table) / sizeof(vga_to_hdmi_table[0]); i ++)
    {
        list[i] = vga_to_hdmi_table[i];
    }

    for (int i = 0; i < sizeof(vga_to_hdmi_table) / sizeof(vga_to_hdmi_table[0]); i ++)
    {
        XLOGV("vga_to_hdmi_list[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取VGA to HDMI的IIC排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVga2HdmiI2cTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(vga_to_hdmi_iic_table) / sizeof(vga_to_hdmi_iic_table[0]); i++)
    {
        list[i].iicBus1 = vga_to_hdmi_iic_table[i].iicBus1;
        list[i].iicBus2 = vga_to_hdmi_iic_table[i].iicBus2;
        list[i].iicAddr = vga_to_hdmi_iic_table[i].iicAddr;
        list[i].uart = vga_to_hdmi_iic_table[i].uart;
        list[i].pGpio = vga_to_hdmi_iic_table[i].pGpio;
        list[i].pLevel = vga_to_hdmi_iic_table[i].pLevel;
        list[i].rGpio = vga_to_hdmi_iic_table[i].rGpio;
        list[i].rLevel = vga_to_hdmi_iic_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(vga_to_hdmi_iic_table) / sizeof(vga_to_hdmi_iic_table[0]); i++)
    {
        XLOGV("vga_to_hdmi_iic_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("vga_to_hdmi_iic_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("vga_to_hdmi_iic_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("vga_to_hdmi_iic_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("vga_to_hdmi_iic_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("vga_to_hdmi_iic_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("vga_to_hdmi_iic_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("vga_to_hdmi_iic_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/**
 * 获取VGA to HDMI 的port排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVgaPortTable(XBH_SOURCE_E list[])
{
    for (int i = 0; i < sizeof(vga_port_table) / sizeof(vga_port_table[0]); i++)
    {
        list[i] = (XBH_SOURCE_E)vga_port_table[i];
    }

    for (int i = 0; i < sizeof(vga_port_table) / sizeof(vga_port_table[0]); i++)
    {
        XLOGV("vga_port_list[%d] = %d ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**********************************************************************************************codec************************************************************************************************/
/**
 * 获取Codec 的器件类型
*/
static XBH_U8 codec_table[] = {
    XBH_CODEC_TABLE
};

static CODEC_OPT_TABLE codec_opt_table[] = {
    XBH_CODEC_OPT_TABLE
};

/**
 * 获取codec的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getCodecTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(codec_table) / sizeof(codec_table[0]); i ++)
    {
        list[i] = codec_table[i];
    }

    for (int i = 0; i < sizeof(codec_table) / sizeof(codec_table[0]); i ++)
    {
        XLOGV("codec_list[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取codec的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getCodecOptTable(CODEC_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(codec_opt_table) / sizeof(codec_opt_table[0]); i++)
    {
        list[i].iicBus = codec_opt_table[i].iicBus;
        list[i].iicAddr = codec_opt_table[i].iicAddr;
        list[i].uart = codec_opt_table[i].uart;
        list[i].pGpio = codec_opt_table[i].pGpio;
        list[i].pLevel = codec_opt_table[i].pLevel;
        list[i].rGpio = codec_opt_table[i].rGpio;
        list[i].rLevel = codec_opt_table[i].rLevel;

        list[i].mic_plug_gpio = codec_opt_table[i].mic_plug_gpio;
        list[i].mic_plug_level = codec_opt_table[i].mic_plug_level;
        list[i].hp_plug_gpio = codec_opt_table[i].hp_plug_gpio;
        list[i].hp_plug_level = codec_opt_table[i].hp_plug_level;
        list[i].hp_mute_gpio = codec_opt_table[i].hp_mute_gpio;
        list[i].hp_mute_level = codec_opt_table[i].hp_mute_level;
    }

    for (int i = 0; i < sizeof(codec_opt_table) / sizeof(codec_opt_table[0]); i++)
    {
        XLOGV("codec_opt_list[%d].iicBus = 0x%02x ",i, list[i].iicBus);
        XLOGV("codec_opt_list[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("codec_opt_list[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("codec_opt_list[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("codec_opt_list[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("codec_opt_list[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("codec_opt_list[%d].rLevel = 0x%02x ",i, list[i].rLevel);

        XLOGV("codec_opt_list[%d].mic_plug_gpio = 0x%02x ",i, list[i].mic_plug_gpio);
        XLOGV("codec_opt_list[%d].mic_plug_level = 0x%02x ",i, list[i].mic_plug_level);
        XLOGV("codec_opt_list[%d].hp_plug_gpio = 0x%02x ",i, list[i].hp_plug_gpio);
        XLOGV("codec_opt_list[%d].hp_plug_level = 0x%02x ",i, list[i].hp_plug_level);
        XLOGV("codec_opt_list[%d].hp_mute_gpio = 0x%02x ",i, list[i].hp_mute_gpio);
        XLOGV("codec_opt_list[%d].hp_mute_level = 0x%02x ",i, list[i].hp_mute_level);
    }
    return XBH_SUCCESS;
}

/**********************************************************************************************NFC************************************************************************************************/
/**
 * 获取NFC 的器件类型
*/
static XBH_U8 nfc_table[] = {
    XBH_NFC_TABLE
};

static COMMON_OPT_TABLE nfc_opt_table[] = {
    XBH_NFC_OPT_TABLE
};

/**
 * 获取NFC的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getNfcTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(nfc_table) / sizeof(nfc_table[0]); i ++)
    {
        list[i] = nfc_table[i];
    }

    for (int i = 0; i < sizeof(nfc_table) / sizeof(nfc_table[0]); i ++)
    {
        XLOGV("g_sensor_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取NFC的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getNfcOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(nfc_opt_table) / sizeof(nfc_opt_table[0]); i++)
    {
        list[i].iicBus1 = nfc_opt_table[i].iicBus1;
        list[i].iicBus2 = nfc_opt_table[i].iicBus2;
        list[i].iicAddr = nfc_opt_table[i].iicAddr;
        list[i].uart = nfc_opt_table[i].uart;
        list[i].pGpio = nfc_opt_table[i].pGpio;
        list[i].pLevel = nfc_opt_table[i].pLevel;
        list[i].rGpio = nfc_opt_table[i].rGpio;
        list[i].rLevel = nfc_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(nfc_opt_table) / sizeof(nfc_opt_table[0]); i++)
    {
        XLOGV("nfc_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("nfc_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("nfc_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("nfc_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("nfc_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("nfc_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("nfc_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("nfc_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;

}

/**********************************************************************************************G-Sensor************************************************************************************************/
/**
 * 获取G-Sensor 的器件类型
*/
static XBH_U8 g_sensor_table[] = {
    XBH_G_SENSOR_TABLE
};

static COMMON_OPT_TABLE g_sensor_opt_table[] = {
    XBH_G_SENSOR_OPT_TABLE
};

/**
 * 获取G-Sensor的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getGSensorTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(g_sensor_table) / sizeof(g_sensor_table[0]); i ++)
    {
        list[i] = g_sensor_table[i];
    }

    for (int i = 0; i < sizeof(g_sensor_table) / sizeof(g_sensor_table[0]); i ++)
    {
        XLOGV("g_sensor_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取G-Sensor的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getGSensorOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(g_sensor_opt_table) / sizeof(g_sensor_opt_table[0]); i++)
    {
        list[i].iicBus1 = g_sensor_opt_table[i].iicBus1;
        list[i].iicBus2 = g_sensor_opt_table[i].iicBus2;
        list[i].iicAddr = g_sensor_opt_table[i].iicAddr;
        list[i].uart = g_sensor_opt_table[i].uart;
        list[i].pGpio = g_sensor_opt_table[i].pGpio;
        list[i].pLevel = g_sensor_opt_table[i].pLevel;
        list[i].rGpio = g_sensor_opt_table[i].rGpio;
        list[i].rLevel = g_sensor_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(g_sensor_opt_table) / sizeof(g_sensor_opt_table[0]); i++)
    {
        XLOGV("g_sensor_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("g_sensor_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("g_sensor_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("g_sensor_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("g_sensor_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("g_sensor_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("g_sensor_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("g_sensor_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;

}

/**********************************************************************************************net switch************************************************************************************************/
/**
 * 获取Net Switch 的器件类型
*/
static XBH_U8 net_switch_table[] = {
    XBH_NET_SWITCH_TABLE
};

static NET_SWITCH_OPT_TABLE net_switch_opt_table[] = {
    XBH_NET_SWITCH_OPT_TABLE
};

/**
 * 获取Net Switch 的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getNetSwitchTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(net_switch_table) / sizeof(net_switch_table[0]); i ++)
    {
        list[i] = net_switch_table[i];
    }

    for (int i = 0; i < sizeof(net_switch_table) / sizeof(net_switch_table[0]); i ++)
    {
        XLOGV("net_switch_list[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取Net Switch 的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getNetSwitchOptTable(NET_SWITCH_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(net_switch_opt_table) / sizeof(net_switch_opt_table[0]); i++)
    {
        list[i].scl = net_switch_opt_table[i].scl;
        list[i].sda = net_switch_opt_table[i].sda;
        list[i].pGpio = net_switch_opt_table[i].pGpio;
        list[i].pLevel = net_switch_opt_table[i].pLevel;
        list[i].rGpio = net_switch_opt_table[i].rGpio;
        list[i].rLevel = net_switch_opt_table[i].rLevel;
        list[i].androidPort = net_switch_opt_table[i].androidPort;
        list[i].typecPort = net_switch_opt_table[i].typecPort;
        list[i].opsPort = net_switch_opt_table[i].opsPort;
        list[i].rj45inPort = net_switch_opt_table[i].rj45inPort;
        list[i].rj45outPort = net_switch_opt_table[i].rj45outPort;
    }

    for (int i = 0; i < sizeof(net_switch_opt_table) / sizeof(net_switch_opt_table[0]); i++)
    {
        XLOGV("net_switch_opt_list[%d].scl = 0x%02x ",i, list[i].scl);
        XLOGV("net_switch_opt_list[%d].sda = 0x%02x ",i, list[i].sda);
        XLOGV("net_switch_opt_list[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("net_switch_opt_list[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("net_switch_opt_list[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("net_switch_opt_list[%d].rLevel = 0x%02x ",i, list[i].rLevel);
        XLOGV("net_switch_opt_list[%d].androidPort = 0x%02x ",i, list[i].androidPort);
        XLOGV("net_switch_opt_list[%d].typecPort = 0x%02x ",i, list[i].typecPort);
        XLOGV("net_switch_opt_list[%d].opsPort = 0x%02x ",i, list[i].opsPort);
        XLOGV("net_switch_opt_list[%d].rj45inPort = 0x%02x ",i, list[i].rj45inPort);
        XLOGV("net_switch_opt_list[%d].rj45outPort = 0x%02x ",i, list[i].rj45outPort);
    }
    return XBH_SUCCESS;
}
/**********************************************************************************************audioToI2s************************************************************************************************/
/**
 * 获取AudioToI2s 的器件类型
*/
static XBH_U8 audioToI2s_table[] = {
    XBH_AUDIO_TO_I2S_TABLE
};

static COMMON_OPT_TABLE audioToI2s_opt_table[] = {
    XBH_AUDIO_TO_I2S_OPT_TABLE
};

/**
 * 获取AudioToI2s的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getAudioToI2sTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(audioToI2s_table) / sizeof(audioToI2s_table[0]); i ++)
    {
        list[i] = audioToI2s_table[i];
    }

    for (int i = 0; i < sizeof(audioToI2s_table) / sizeof(audioToI2s_table[0]); i ++)
    {
        XLOGV("audioToI2s_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * AudioToI2s的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getAudioToI2sOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(audioToI2s_opt_table) / sizeof(audioToI2s_opt_table[0]); i++)
    {
        list[i].iicBus1 = audioToI2s_opt_table[i].iicBus1;
        list[i].iicBus2 = audioToI2s_opt_table[i].iicBus2;
        list[i].iicAddr = audioToI2s_opt_table[i].iicAddr;
        list[i].uart = audioToI2s_opt_table[i].uart;
        list[i].pGpio = audioToI2s_opt_table[i].pGpio;
        list[i].pLevel = audioToI2s_opt_table[i].pLevel;
        list[i].rGpio = audioToI2s_opt_table[i].rGpio;
        list[i].rLevel = audioToI2s_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(audioToI2s_opt_table) / sizeof(audioToI2s_opt_table[0]); i++)
    {
        XLOGV("audioToI2s_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("audioToI2s_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("audioToI2s_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("audioToI2s_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("audioToI2s_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("audioToI2s_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("audioToI2s_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("audioToI2s_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/**********************************************************************************************light sensor************************************************************************************************/
/**
 * 获取Light sensor 的器件类型
*/
static COMMON_OPT_TABLE lightsensor_opt_table[] = {
    XBH_LIGHT_SENSOR_OPT_TABLE
};

/**
 * 获取Light Sensor的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getLightSensorOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(lightsensor_opt_table) / sizeof(lightsensor_opt_table[0]); i++)
    {
        list[i].iicBus1 = lightsensor_opt_table[i].iicBus1;
        list[i].iicBus2 = lightsensor_opt_table[i].iicBus2;
        list[i].iicAddr = lightsensor_opt_table[i].iicAddr;
        list[i].uart = lightsensor_opt_table[i].uart;
        list[i].pGpio = lightsensor_opt_table[i].pGpio;
        list[i].pLevel = lightsensor_opt_table[i].pLevel;
        list[i].rGpio = lightsensor_opt_table[i].rGpio;
        list[i].rLevel = lightsensor_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(lightsensor_opt_table) / sizeof(lightsensor_opt_table[0]); i++)
    {
        XLOGV("lightsensor_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("lightsensor_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("lightsensor_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("lightsensor_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("lightsensor_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("lightsensor_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("lightsensor_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("lightsensor_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/**********************************************************************************************air sensor************************************************************************************************/
/**
 * 获取air sensor 的器件类型
*/
static XBH_U8 airsensor_table[] = {
    XBH_AIR_SENSOR_TABLE
};

static COMMON_OPT_TABLE airsensor_opt_table[] = {
    XBH_AIR_SENSOR_OPT_TABLE
};

/**
 * 获取Air Sensor的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getAirSensorTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(airsensor_table) / sizeof(airsensor_table[0]); i ++)
    {
        list[i] = airsensor_table[i];
    }

    for (int i = 0; i < sizeof(airsensor_table) / sizeof(airsensor_table[0]); i ++)
    {
        XLOGV("airsensor_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**********************************************************************************************power sensor************************************************************************************************/
/**
 * 获取power sensor 的器件类型
*/
static XBH_U8 powersensor_table[] = {
    XBH_POWER_SENSOR_TABLE
};

static COMMON_OPT_TABLE powersensor_opt_table[] = {
    XBH_POWER_SENSOR_OPT_TABLE
};

/**
 * 获取power Sensor的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getPowerSensorTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(powersensor_table) / sizeof(powersensor_table[0]); i ++)
    {
        list[i] = powersensor_table[i];
    }

    for (int i = 0; i < sizeof(powersensor_table) / sizeof(powersensor_table[0]); i ++)
    {
        XLOGV("powersensor_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取Power Sensor的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getPowerSensorOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(powersensor_opt_table) / sizeof(powersensor_opt_table[0]); i++)
    {
        list[i].iicBus1 = powersensor_opt_table[i].iicBus1;
        list[i].iicBus2 = powersensor_opt_table[i].iicBus2;
        list[i].iicAddr = powersensor_opt_table[i].iicAddr;
        list[i].uart = powersensor_opt_table[i].uart;
        list[i].pGpio = powersensor_opt_table[i].pGpio;
        list[i].pLevel = powersensor_opt_table[i].pLevel;
        list[i].rGpio = powersensor_opt_table[i].rGpio;
        list[i].rLevel = powersensor_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(powersensor_opt_table) / sizeof(powersensor_opt_table[0]); i++)
    {
        XLOGV("powersensor_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("powersensor_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("powersensor_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("powersensor_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("powersensor_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("powersensor_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("powersensor_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("powersensor_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}
/**
 * 获取Air Sensor的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getAirSensorOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(airsensor_opt_table) / sizeof(airsensor_opt_table[0]); i++)
    {
        list[i].iicBus1 = airsensor_opt_table[i].iicBus1;
        list[i].iicBus2 = airsensor_opt_table[i].iicBus2;
        list[i].iicAddr = airsensor_opt_table[i].iicAddr;
        list[i].uart = airsensor_opt_table[i].uart;
        list[i].pGpio = airsensor_opt_table[i].pGpio;
        list[i].pLevel = airsensor_opt_table[i].pLevel;
        list[i].rGpio = airsensor_opt_table[i].rGpio;
        list[i].rLevel = airsensor_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(airsensor_opt_table) / sizeof(airsensor_opt_table[0]); i++)
    {
        XLOGV("airsensor_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("airsensor_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("airsensor_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("airsensor_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("airsensor_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("airsensor_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("airsensor_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("airsensor_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}
/*********************************************************************************************mcu************************************************************************************************/
/**
 * 获取MCU的器件类型
*/
static XBH_U8 mcu_table[] = {
    XBH_MCU_TABLE
};

static COMMON_OPT_TABLE mcu_opt_table[] = {
    XBH_MCU_OPT_TABLE
};

/**
 * 获取MCU的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getMcuTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(mcu_table) / sizeof(mcu_table[0]); i ++)
    {
        list[i] = mcu_table[i];
    }

    for (int i = 0; i < sizeof(mcu_table) / sizeof(mcu_table[0]); i ++)
    {
        XLOGV("mcu_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取MCU的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getMcuOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(mcu_opt_table) / sizeof(mcu_opt_table[0]); i++)
    {
        list[i].iicBus1 = mcu_opt_table[i].iicBus1;
        list[i].iicBus2 = mcu_opt_table[i].iicBus2;
        list[i].iicAddr = mcu_opt_table[i].iicAddr;
        list[i].uart = mcu_opt_table[i].uart;
        list[i].pGpio = mcu_opt_table[i].pGpio;
        list[i].pLevel = mcu_opt_table[i].pLevel;
        list[i].rGpio = mcu_opt_table[i].rGpio;
        list[i].rLevel = mcu_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(mcu_opt_table) / sizeof(mcu_opt_table[0]); i++)
    {
        XLOGV("mcu_opt_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("mcu_opt_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("mcu_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("mcu_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("mcu_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("mcu_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("mcu_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("mcu_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/*********************************************************************************************amplifer************************************************************************************************/
/**
 * 获取Amp 的器件类型
*/
static XBH_U8 amp_table[] = {
    XBH_AMP_TABLE
};

static AMP_OPT_TABLE amp_opt_table[] = {
    XBH_AMP_OPT_TABLE
};

/**
 * 获取codec的器件类型
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getAmpTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(amp_table) / sizeof(amp_table[0]); i ++)
    {
        list[i] = amp_table[i];
    }

    for (int i = 0; i < sizeof(amp_table) / sizeof(amp_table[0]); i ++)
    {
        XLOGV("amp_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}

/**
 * 获取codec的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getAmpOptTable(AMP_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(amp_opt_table) / sizeof(amp_opt_table[0]); i++)
    {
        list[i].iicBus = amp_opt_table[i].iicBus;
        list[i].iicAddr = amp_opt_table[i].iicAddr;
        list[i].pGpio = amp_opt_table[i].pGpio;
        list[i].pLevel = amp_opt_table[i].pLevel;
        list[i].rGpio = amp_opt_table[i].rGpio;
        list[i].rLevel = amp_opt_table[i].rLevel;
        list[i].mute_gpio = amp_opt_table[i].mute_gpio;
        list[i].mute_level = amp_opt_table[i].mute_gpio;
    }

    for (int i = 0; i < sizeof(amp_opt_table) / sizeof(amp_opt_table[0]); i++)
    {
        XLOGV("amp_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus);
        XLOGV("amp_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("amp_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("amp_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("amp_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("amp_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
        XLOGV("amp_opt_table[%d].mute_gpio = 0x%02x ",i, list[i].mute_gpio);
        XLOGV("amp_opt_table[%d].mute_level = 0x%02x ",i, list[i].mute_level);
    }
    return XBH_SUCCESS;
}
/*********************************************************************************************video out************************************************************************************************/
/**
 * 获取video out的器件类型
*/
static XBH_U8 video_out_table[] = {
    XBH_VIDEO_OUT_TABLE
};

static COMMON_OPT_TABLE video_out_opt_table[] = {
    XBH_VIDEO_OUT_IIC_TABLE
};
/**
 * 获取video out的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVideoOutTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(video_out_table) / sizeof(video_out_table[0]); i ++)
    {
        list[i] = video_out_table[i];
    }

    for (int i = 0; i < sizeof(video_out_table) / sizeof(video_out_table[0]); i ++)
    {
        XLOGV("video_out_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取video out的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getVideoOutOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(video_out_opt_table) / sizeof(video_out_opt_table[0]); i++)
    {
        list[i].iicBus1 = video_out_opt_table[i].iicBus1;
        list[i].iicBus2 = video_out_opt_table[i].iicBus2;
        list[i].iicAddr = video_out_opt_table[i].iicAddr;
        list[i].uart = video_out_opt_table[i].uart;
        list[i].pGpio = video_out_opt_table[i].pGpio;
        list[i].pLevel = video_out_opt_table[i].pLevel;
        list[i].rGpio = video_out_opt_table[i].rGpio;
        list[i].rLevel = video_out_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(video_out_opt_table) / sizeof(video_out_opt_table[0]); i++)
    {
        XLOGV("video_out_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus1);
        XLOGV("video_out_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus2);
        XLOGV("video_out_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("video_out_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("video_out_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("video_out_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("video_out_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("video_out_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/*********************************************************************************************rj45 to Usb************************************************************************************************/
/**
 * 获取rj45 to Usb的器件类型
*/
static XBH_U8 rj45_to_usb_table[] = {
    XBH_RJ45_TO_USB_TABLE
};

static COMMON_OPT_TABLE rj45_to_usb_opt_table[] = {
    XBH_RJ45_TO_USB_OPT_TABLE
};
/**
 * 获取rj45 to Usb的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getRj45ToUsbTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(rj45_to_usb_table) / sizeof(rj45_to_usb_table[0]); i ++)
    {
        list[i] = rj45_to_usb_table[i];
    }

    for (int i = 0; i < sizeof(rj45_to_usb_table) / sizeof(rj45_to_usb_table[0]); i ++)
    {
        XLOGV("rj45_to_usb_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取rj45 to Usbt的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getRj45ToUsbOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(rj45_to_usb_opt_table) / sizeof(rj45_to_usb_opt_table[0]); i++)
    {
        list[i].iicBus1 = rj45_to_usb_opt_table[i].iicBus1;
        list[i].iicBus2 = rj45_to_usb_opt_table[i].iicBus2;
        list[i].iicAddr = rj45_to_usb_opt_table[i].iicAddr;
        list[i].uart = rj45_to_usb_opt_table[i].uart;
        list[i].pGpio = rj45_to_usb_opt_table[i].pGpio;
        list[i].pLevel = rj45_to_usb_opt_table[i].pLevel;
        list[i].rGpio = rj45_to_usb_opt_table[i].rGpio;
        list[i].rLevel = rj45_to_usb_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(rj45_to_usb_opt_table) / sizeof(rj45_to_usb_opt_table[0]); i++)
    {
        XLOGV("rj45_to_usb_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus1);
        XLOGV("rj45_to_usb_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus2);
        XLOGV("rj45_to_usb_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("rj45_to_usb_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("rj45_to_usb_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("rj45_to_usb_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("rj45_to_usb_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("rj45_to_usb_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/*********************************************************************************************PD IC************************************************************************************************/
/**
 * 获取PD IC的排列table
*/
static XBH_U8 pd_ic_table[] = {
    XBH_PD_IC_TABLE
};

static COMMON_OPT_TABLE pd_ic_iic_table[] = {
    XBH_PD_IC_IIC_TABLE
};

/**
 * 获取PD IC的排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getPdIcTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(pd_ic_table) / sizeof(pd_ic_table[0]); i ++)
    {
        list[i] = pd_ic_table[i];
    }

    for (int i = 0; i < sizeof(pd_ic_table) / sizeof(pd_ic_table[0]); i ++)
    {
        XLOGV("pd_ic_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取PD IC的IIC排列table
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getPdIcI2cTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(pd_ic_iic_table) / sizeof(pd_ic_iic_table[0]); i++)
    {
        list[i].iicBus1 = pd_ic_iic_table[i].iicBus1;
        list[i].iicBus2 = pd_ic_iic_table[i].iicBus2;
        list[i].iicAddr = pd_ic_iic_table[i].iicAddr;
        list[i].uart = pd_ic_iic_table[i].uart;
        list[i].pGpio = pd_ic_iic_table[i].pGpio;
        list[i].pLevel = pd_ic_iic_table[i].pLevel;
        list[i].rGpio = pd_ic_iic_table[i].rGpio;
        list[i].rLevel = pd_ic_iic_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(pd_ic_iic_table) / sizeof(pd_ic_iic_table[0]); i++)
    {
        XLOGV("pd_ic_iic_table[%d].iicBus1 = 0x%02x ",i, list[i].iicBus1);
        XLOGV("pd_ic_iic_table[%d].iicBus2 = 0x%02x ",i, list[i].iicBus2);
        XLOGV("pd_ic_iic_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("pd_ic_iic_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("pd_ic_iic_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("pd_ic_iic_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("pd_ic_iic_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("pd_ic_iic_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}

/*********************************************************************************************MISC************************************************************************************************/
/**
 * 检查是否自动测试模式
 * retval true:yes,false:no
*/
XBH_BOOL XbhModuleInterface::getFattMode(XBH_VOID)
{
    return property_get_bool("vendor.xbh.fattmode", XBH_FALSE);
}

/**
 * 开启SOC Free Run功能, 使输出信号固定不变
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::setSocFreeRun(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

/*********************************************************************************************USB HUB************************************************************************************************/
/**
 * 获取USB HUB的器件类型
*/
static XBH_U8 usb_hub_table[] = {
    XBH_USB_HUB_TABLE
};

static COMMON_OPT_TABLE usb_hub_opt_table[] = {
    XBH_USB_HUB_OPT_TABLE
};
/**
 * 获取USB HUB的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbHubTable(XBH_U8 list[])
{
    for (int i = 0; i < sizeof(usb_hub_table) / sizeof(usb_hub_table[0]); i ++)
    {
        list[i] = usb_hub_table[i];
    }

    for (int i = 0; i < sizeof(usb_hub_table) / sizeof(usb_hub_table[0]); i ++)
    {
        XLOGV("usb_hub_table[%d] = 0x%02x ",i, list[i]);
    }
    return XBH_SUCCESS;
}
/**
 * 获取USB HUB的器件的操作参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhModuleInterface::getUsbHubOptTable(COMMON_OPT_TABLE list[])
{
    for (int i = 0; i < sizeof(usb_hub_opt_table) / sizeof(usb_hub_opt_table[0]); i++)
    {
        list[i].iicBus1 = usb_hub_opt_table[i].iicBus1;
        list[i].iicBus2 = usb_hub_opt_table[i].iicBus2;
        list[i].iicAddr = usb_hub_opt_table[i].iicAddr;
        list[i].uart = usb_hub_opt_table[i].uart;
        list[i].pGpio = usb_hub_opt_table[i].pGpio;
        list[i].pLevel = usb_hub_opt_table[i].pLevel;
        list[i].rGpio = usb_hub_opt_table[i].rGpio;
        list[i].rLevel = usb_hub_opt_table[i].rLevel;
    }

    for (int i = 0; i < sizeof(usb_hub_opt_table) / sizeof(usb_hub_opt_table[0]); i++)
    {
        XLOGV("usb_hub_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus1);
        XLOGV("usb_hub_opt_table[%d].iicBus = 0x%02x ",i, list[i].iicBus2);
        XLOGV("usb_hub_opt_table[%d].iicAddr = 0x%02x ",i, list[i].iicAddr);
        XLOGV("usb_hub_opt_table[%d].uart = 0x%02x ",i, list[i].uart);
        XLOGV("usb_hub_opt_table[%d].pGpio = 0x%02x ",i, list[i].pGpio);
        XLOGV("usb_hub_opt_table[%d].pLevel = 0x%02x ",i, list[i].pLevel);
        XLOGV("usb_hub_opt_table[%d].rGpio = 0x%02x ",i, list[i].rGpio);
        XLOGV("usb_hub_opt_table[%d].rLevel = 0x%02x ",i, list[i].rLevel);
    }
    return XBH_SUCCESS;
}
#if 1
// C风格接口函数
extern "C" XBH_S32 XbhC_setI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data);
extern "C" XBH_S32 XbhC_getI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data);

extern "C" XBH_S32 XbhC_setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue);
extern "C" XBH_S32 XbhC_getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue);
extern "C" XBH_S32 XbhC_getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue);

XBH_S32 XbhC_setI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, const XBH_U8* u8Data)
{
    return XbhService::getModuleInterface()->setI2cData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
}

XBH_S32 XbhC_getI2cData(XBH_U32 u32I2cNum, XBH_U8 u8DevAddress, XBH_U32 u32RegAddr, XBH_U32 u32RegAddrCount, XBH_U32 u32Length, XBH_U8* u8Data)
{
    return XbhService::getModuleInterface()->getI2cData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
}

XBH_S32 XbhC_setGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32 u32GpioValue)
{
    return XbhService::getModuleInterface()->setGpioOutputValue(u32GpioNumber, u32GpioValue);
}

XBH_S32 XbhC_getGpioOutputValue(XBH_U32 u32GpioNumber, XBH_U32* u32GpioValue)
{
    return XbhService::getModuleInterface()->getGpioOutputValue(u32GpioNumber, u32GpioValue);
}

XBH_S32 XbhC_getGpioInputValue(XBH_U32 u32GpioNumber, XBH_U32 *u32GpioValue)
{
    return XbhService::getModuleInterface()->getGpioInputValue(u32GpioNumber, u32GpioValue);
}
#endif
