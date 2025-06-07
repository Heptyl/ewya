#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPowerSensorInterface"

#include "XbhLog.h"
#include "XbhPowerSensorInterface.h"


//获取power
XBH_S32 XbhPowerSensorInterface::getPowerSensorValue(XBH_FLOAT* s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}


/**
 * 获取chip是否初始化完成
*/
XBH_S32 XbhPowerSensorInterface::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 XbhPowerSensorInterface::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}
