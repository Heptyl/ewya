#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipHym8563.cpp"

/* Sys headers */
#include "XbhRtcPrivate.h"
#include "XbhService.h"
#include "XbhLog.h"

//pgz add, set rtc driver version
#define HYM8563_DRIVER_VERSION          "rtc8563_1"

#define HYM8563_I2C_TIME_REG                  0x02
#define HYM8563_I2C_ALARM_ENABLE_REG          0x01
#define HYM8563_I2C_ALARM_TIME_REG            0x09
/*
* The RTC chip will calculate whether the next day on February 28th is February 29th or March 1st based on whether it is currently a leap year. 
* Therefore, it is required that the starting year of RTC chips be calculated from 1900
* The storage range of RTC chips is 0-99
*/
static XBH_S32 RTC_SetTime(XBH_U32 s32Year, XBH_U32 s32Month, XBH_U32 s32Date, XBH_U32 s32Week, XBH_U32 s32Hour, XBH_U32 s32Minute, XBH_U32 s32Sec)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 relTime[7];
    XBH_U32 iRtcYear     = s32Year;
    XBH_U32 iRtcMonth    = s32Month;
    XBH_U32 iRtcDate     = s32Date;
    XBH_U32 iRtcWeek     = s32Week;
    XBH_U32 iRtcHour     = s32Hour;
    XBH_U32 iRtcMinute   = s32Minute;
    XBH_U32 iRtcSec      = s32Sec;

    /* Years <= 19XX is to early, and 2100 is too far, year must > 100 and < 200 */
    if (iRtcYear  < 100 || iRtcYear > 200)
    {
        XLOGW("hym8563 RTC_SetTime fail, year invalid %d \n",iRtcYear);
        return XBH_FAILURE;
    }
    else
    {
        iRtcYear = s32Year - 100;
    }

    iRtcYear = ((iRtcYear > 99) || iRtcYear < 0) ? 0 : iRtcYear;
    iRtcMonth = (iRtcMonth > 12 || iRtcMonth < 1) ? 1 : iRtcMonth;
    iRtcDate = (iRtcDate > 31 || iRtcDate < 1) ? 1 : iRtcDate;
    iRtcWeek = (iRtcWeek > 6 || iRtcWeek < 0) ? 0 : iRtcWeek;
    iRtcHour = (iRtcHour > 23 || iRtcHour < 0) ? 0 : iRtcHour;
    iRtcMinute = (iRtcMinute > 59 || iRtcMinute < 0) ? 0 : iRtcMinute;
    iRtcSec = (iRtcSec >= 59 || iRtcSec < 0) ? 0 : iRtcSec;
    memset(relTime, 0, sizeof(relTime));
    relTime[6] = (iRtcYear / 10 << 4) | (iRtcYear % 10);
    relTime[5] = (iRtcMonth / 10 << 4) | (iRtcMonth % 10);
    relTime[4] = (iRtcWeek / 10 << 4) | (iRtcWeek % 10);
    relTime[3] = (iRtcDate / 10 << 4) | (iRtcDate % 10);
    relTime[2] = (iRtcHour / 10 << 4) | (iRtcHour % 10);
    relTime[1] = (iRtcMinute / 10 << 4) | (iRtcMinute % 10);
    relTime[0] = ((iRtcSec / 10 << 4) & 0x70) | (iRtcSec % 10);
    XLOGW("hym8563 RTC_SetTime  Y-%d M-%d D-%d W-%d H-%d m-%d \n", s32Year, s32Month, s32Date, s32Week, s32Hour, s32Minute);
    s32Ret = XbhService::getModuleInterface()->setRtcI2cData(HYM8563_IC, HYM8563_I2C_TIME_REG, 1, sizeof(relTime), relTime);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 RTC_SetTime write time error\n");
        return XBH_FAILURE;
    }
    return s32Ret;
}

static XBH_S32 RTC_SetAlarmTime(XBH_U32 s32Date, XBH_U32 s32Week, XBH_U32 s32Hour, XBH_U32 s32Minute, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 iRtcWeek     = s32Week;
    XBH_U32 iRtcDate     = s32Date;
    XBH_U32 iRtcHour     = s32Hour;
    XBH_U32 iRtcMinute   = s32Minute;
    XBH_BOOL enable      = bEnable;
    XBH_U8 iRtcEnable[1];
    XBH_U8 iRtcAlarmTime[4];

    iRtcDate = (iRtcDate > 31 || iRtcDate < 1) ? 1 : iRtcDate;
    iRtcWeek = (iRtcWeek > 6 || iRtcWeek < 0) ? 0 : iRtcWeek;
    iRtcHour = (iRtcHour > 23 || iRtcHour < 0) ? 0 : iRtcHour;
    iRtcMinute = (iRtcMinute > 59 || iRtcMinute < 0) ? 0 : iRtcMinute;

    // alarm disable
    memset(iRtcEnable, 0, sizeof(iRtcEnable));
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(HYM8563_IC, HYM8563_I2C_ALARM_ENABLE_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 RTC_SetAlarmTime Read i2c error\n");
        return XBH_FAILURE;
    }
    iRtcEnable[0] &= ~0x02;
    iRtcEnable[0] &= ~0x08;
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(HYM8563_IC, HYM8563_I2C_ALARM_ENABLE_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 RTC_SetAlarmTime  Write i2c error\n");
        return XBH_FAILURE;
    }

    // alarm time
    memset(iRtcAlarmTime, 0, sizeof(iRtcAlarmTime));
    iRtcAlarmTime[0] = (iRtcMinute / 10 << 4) | (iRtcMinute % 10);
    iRtcAlarmTime[1] = (iRtcHour / 10 << 4) | (iRtcHour % 10);
    iRtcAlarmTime[2] = (iRtcDate / 10 << 4) | (iRtcDate % 10);
    iRtcAlarmTime[3] = (iRtcWeek & 0x7);
    XLOGW("hym8563 RTC_SetAlarmTime  Enable=%d D-%d H-%d M-%d \n", bEnable, s32Date, s32Hour, s32Minute);
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(HYM8563_IC, HYM8563_I2C_ALARM_TIME_REG, 1, sizeof(iRtcAlarmTime), iRtcAlarmTime);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 RTC_SetAlarmTime  Write i2c error\n");
        return XBH_FAILURE;
    }

    // alarm enable
    memset(iRtcEnable, 0, sizeof(iRtcEnable));
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(HYM8563_IC, HYM8563_I2C_ALARM_ENABLE_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 read alarm enable error\n");
        return XBH_FAILURE;
    }
    if (enable == XBH_TRUE)
    {
        iRtcEnable[0] |= 0x02;
    }
    else
    {
        iRtcEnable[0] &= ~0x02;
    }
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(HYM8563_IC, HYM8563_I2C_ALARM_ENABLE_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 write alarm enable error\n");
        return XBH_FAILURE;
    }
    return s32Ret;
}

static XBH_S32 RTC_GetTime(XBH_U32* ps32Year, XBH_U32* ps32Month, XBH_U32* ps32Date, XBH_U32* ps32Week, XBH_U32* ps32Hour, XBH_U32* ps32Minute, XBH_U32* ps32Sec)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    
    XBH_U32 iRtcYear     = 0;
    XBH_U32 iRtcMonth    = 0;
    XBH_U32 iRtcDate     = 0;
    XBH_U32 iRtcWeek     = 0;
    XBH_U32 iRtcHour     = 0;
    XBH_U32 iRtcMinute   = 0;
    XBH_U32 iRtcSec      = 0;
    XBH_U8 relTime[7];
    memset(relTime, 0, sizeof(relTime));
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(HYM8563_IC, HYM8563_I2C_TIME_REG, 1, sizeof(relTime), relTime);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("hym8563 RTC_GetTime Read i2c error\n");
        return XBH_FAILURE;
    }

    iRtcSec = ((relTime[0] & 0x70) >> 4) * 10 + (relTime[0] & 0x0f);
    iRtcMinute = ((relTime[1] & 0x70) >> 4) * 10 + (relTime[1] & 0x0f);
    iRtcHour = ((relTime[2] & 0x30) >> 4) * 10 + (relTime[2] & 0x0f);
    iRtcDate = ((relTime[3] & 0x30) >> 4) * 10 + (relTime[3] & 0x0f);
    iRtcWeek = (relTime[4] & 0x07);
    iRtcMonth = ((relTime[5] & 0x10) >> 4) * 10 + (relTime[5] & 0x0f);
    iRtcYear = ((relTime[6] & 0xf0) >> 4) * 10 + (relTime[6] & 0x0f);

    //year range is 100 ~ 199
    iRtcYear = iRtcYear + 100;
    XLOGW("hym8563 RTC_GetTime convert base 1900->2000\n");

    XLOGD("hym8563 iRtcSec: %d\n", iRtcSec);
    XLOGD("hym8563 iRtcMinute: %d\n", iRtcMinute);
    XLOGD("hym8563 iRtcHour: %d\n", iRtcHour);
    XLOGD("hym8563 iRtcWeek: %d\n", iRtcWeek);
    XLOGD("hym8563 iRtcDate: %d\n", iRtcDate);
    XLOGD("hym8563 iRtcMonth: %d\n", iRtcMonth);
    XLOGD("hym8563 iRtcYear: %d\n", iRtcYear);

    if (iRtcSec > 59 || iRtcMinute > 59 || iRtcHour > 23 || iRtcWeek > 6
        || iRtcDate > 31 || iRtcMonth > 12 || iRtcYear > 199 || iRtcYear < 100)
    {
        XLOGW("hym8563 RTC_GetTime value is error.\n");
        return XBH_FAILURE;
    }
    *ps32Year    = iRtcYear;
    *ps32Month   = iRtcMonth;
    *ps32Date    = iRtcDate;
    *ps32Week    = iRtcWeek;
    *ps32Hour    = iRtcHour;
    *ps32Minute  = iRtcMinute;
    *ps32Sec     = iRtcSec;
     return s32Ret;

}

static XBH_S32 RTC_Init()
{
    XLOGD(" begin ");
    XLOGD(" end ");
    return XBH_SUCCESS;
}

static XBH_VOID RTC_Deinit(XBH_VOID)
{
    return;
}

RTC_DRV_FUNC_S stHym8563Func =
{
    .pfnRtcSetTime         = RTC_SetTime,
    .pfnRtcSetAlarmTime    = RTC_SetAlarmTime,
    .pfnRtcGetTime         = RTC_GetTime,
    .pfnRtcDeinit          = RTC_Deinit,
    .pfnRtcInit            = RTC_Init,
};

