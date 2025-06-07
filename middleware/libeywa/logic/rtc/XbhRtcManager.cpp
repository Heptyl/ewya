#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhRtcManager"
#include "XbhLog.h"

#include <stdio.h>
#include "XbhRtcManager.h"
#include "XbhRtcPrivate.h"
#include "XbhService.h"
#include <unistd.h>

static RTC_DRV_FUNC_S* pfnRTCFunc;
static RTC_GLOBAL_PARAM_S s_stRtcDrv =
{
    .u8DeviceType            = XBH_RTC_NONE,
};

XbhRtcManager*              XbhRtcManager::mInstance = NULL;
XbhMutex                    XbhRtcManager::mLock;

//Used to detect which module exists.
XBH_U8 rtcTest[][2] = 
{
    //dev, data reg
    {HYM8563_IC, 0x02},   //XBH_RTC_HYM8563
    {PT7C4337_IC, 0x00},   //XBH_RTC_PT7C4337
};
    
XBH_S32 init()
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U8 relTime[7];
    memset(relTime, 0, sizeof(relTime));

    for(XBH_U8 i = 0; i < sizeof(rtcTest)/(sizeof(XBH_U8) * 2); i ++)
    {
        ret = XbhService::getModuleInterface()->getRtcI2cData(rtcTest[i][0], rtcTest[i][1], 1, sizeof(relTime), relTime);
        if(ret == XBH_SUCCESS)
        {
            s_stRtcDrv.u8DeviceType = i + 1;
            break;
        }
    }
    XLOGD("RTC INIT type : %d \n", s_stRtcDrv.u8DeviceType);
    
    switch (s_stRtcDrv.u8DeviceType)
    {
        case XBH_RTC_HYM8563:
        {
            extern RTC_DRV_FUNC_S stHym8563Func;
            pfnRTCFunc = &stHym8563Func;
            break;
        }
        case XBH_RTC_PT7C4337:
        {
            extern RTC_DRV_FUNC_S stPt7c4337Func;
            pfnRTCFunc = &stPt7c4337Func;
            break;
        }
        default:
        {
            pfnRTCFunc = XBH_NULL;
            XLOGE("not found rtc chip !!!!! \n");
            return XBH_FAILURE;
        }
    }
    if(pfnRTCFunc != XBH_NULL)
    {
        ret = pfnRTCFunc->pfnRtcInit();
    }

    if (XBH_SUCCESS != ret)
    {
        XLOGE("RTC INIT err:%#x!\n", ret);
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

//---------------- custom device interface start ----------------

XBH_S32 XbhRtcManager::setTime(XBH_U32 year, XBH_U32 month, XBH_U32 date, XBH_U32 week, XBH_U32 hour, XBH_U32 minute, XBH_U32 sec)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    if(pfnRTCFunc != XBH_NULL)
    {
        s32Ret = pfnRTCFunc->pfnRtcSetTime(year, month, date, week, hour, minute, sec);
    }
    
    return s32Ret;
}

XBH_S32 XbhRtcManager::setAlarmTime(XBH_U32 date, XBH_U32 week, XBH_U32 hour, XBH_U32 minute, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(pfnRTCFunc != XBH_NULL)
    {
        s32Ret = pfnRTCFunc->pfnRtcSetAlarmTime(date, week, hour, minute, bEnable);
    }
    return s32Ret;
}

XBH_S32 XbhRtcManager::getTime(XBH_U32* year, XBH_U32* month, XBH_U32* date, XBH_U32* week, XBH_U32* hour, XBH_U32* minute, XBH_U32* sec)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(pfnRTCFunc != XBH_NULL)
    {
        s32Ret = pfnRTCFunc->pfnRtcGetTime(year, month, date, week, hour, minute, sec);
    }
    return s32Ret;
}

//void XbhRtcManager::run(const void* arg)
//{
    // XBH_U32 year;
    // XBH_U32 month;
    // XBH_U32 date;
    // XBH_U32 week;
    // XBH_U32 hour;
    // XBH_U32 minute;
    // XBH_U32 sec;
    // getTime(&year, &month, &date, &week, &hour, &minute, &sec);
    //usleep(2000*1000);
//}

XbhRtcManager::XbhRtcManager()
{
    XLOGD("init begin");
    init();
    //XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD("init end");
}

XbhRtcManager::~XbhRtcManager()
{

}

//---------------- custom device interface end ----------------
XbhRtcManager* XbhRtcManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhRtcManager();
    }
    return mInstance;
}

