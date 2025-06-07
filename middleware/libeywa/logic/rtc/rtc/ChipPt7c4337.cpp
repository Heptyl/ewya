#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipPt7c4337.cpp"

/* Sys headers */
#include "XbhRtcPrivate.h"
#include "XbhService.h"
#include "XbhLog.h"

#define PT7C4337_BYTE_SECS               0
#define PT7C4337_BYTE_MINS               1
#define PT7C4337_BYTE_HOURS              2
#define PT7C4337_BYTE_WDAY               3
#define PT7C4337_BYTE_DAY                4
#define PT7C4337_BYTE_MONTH              5
#define PT7C4337_BYTE_YEAR               6

#define PT7C4337_I2C_SECS_REG            0x00
#define PT7C4337_I2C_MINS_REG            0x01
#define PT7C4337_I2C_HOURS_REG           0x02
#define PT7C4337_I2C_WDAY_REG            0x03
#define PT7C4337_I2C_DAY_REG             0x04
#define PT7C4337_I2C_MONTH_REG           0x05
#define PT7C4337_I2C_YEAR_REG            0x06

#define PT7C4337_ALARM_BYTE_SECS         0
#define PT7C4337_ALARM_BYTE_MINS         1
#define PT7C4337_ALARM_BYTE_HOURS        2
#define PT7C4337_ALARM_BYTE_DAY          3

#define PT7C4337_ALARM_REG_SECS          0x07
#define PT7C4337_ALARM_REG_MINS          0x08
#define PT7C4337_ALARM_REG_HOURS         0x09
#define PT7C4337_ALARM_REG_DAY           0x0A
#define PT7C4337_ALARM_DAY_DATE_BIT      (1<<6)  //0:date of month  1:day of week

#define PT7C4337_CMD_CONTROL_REG         0x0E
#define PT7C4337_ALARM_ENABLE_BIT        (1<<0)  //Alarm1 enable bit

#define PT7C4337_CMD_STATUS_REG          0x0F
/* flags for STATUS */
#define PT7C4337_HOURS_24H_BIT           (1<<6)  //0:24h 1:12h

/*
* The RTC chip will calculate whether the next day on February 28th is February 29th or March 1st based on whether it is currently a leap year. 
* Therefore, it is required that the starting year of RTC chips be calculated from 1900
* The storage range of RTC chips is 0-99
*/

//BCD码转为二进制 
XBH_U8 bcd2bin(XBH_U8 val)
{
    return (val & 0x0F) + (val >> 4) * 10;
}
 
//二进制转为BCD码 
XBH_U8 bin2bcd(XBH_U8 val)
{
    return ((val / 10) << 4) + val % 10;
}

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
        XLOGW("pt7c4337 RTC_SetTime fail, year invalid %d \n",iRtcYear);
        return XBH_FAILURE;
    }
    else
    {
        iRtcYear = s32Year - 100;
    }

    iRtcYear = ((iRtcYear > 99) || iRtcYear < 0) ? 0 : iRtcYear;
    iRtcMonth = (iRtcMonth > 12 || iRtcMonth < 1) ? 1 : iRtcMonth;
    iRtcDate= (iRtcDate > 31 || iRtcDate < 1) ? 1 : iRtcDate;
    iRtcWeek= (iRtcWeek > 6 || iRtcWeek < 0) ? 0 : iRtcWeek;
    iRtcHour = (iRtcHour > 23 || iRtcHour < 0) ? 0 : iRtcHour;
    iRtcMinute = (iRtcMinute > 59 || iRtcMinute < 0) ? 0 : iRtcMinute;
    iRtcSec = (iRtcSec >= 59 || iRtcSec < 0) ? 0 : iRtcSec;
    memset(relTime, 0, sizeof(relTime));
    relTime[PT7C4337_BYTE_YEAR]   = bin2bcd(iRtcYear);
    relTime[PT7C4337_BYTE_MONTH]  = bin2bcd(iRtcMonth);
    relTime[PT7C4337_BYTE_DAY]    = bin2bcd(iRtcDate);
    relTime[PT7C4337_BYTE_WDAY]   = bin2bcd(iRtcWeek+1);
    relTime[PT7C4337_BYTE_HOURS]  = bin2bcd(iRtcHour);
    relTime[PT7C4337_BYTE_MINS]   = bin2bcd(iRtcMinute);
    relTime[PT7C4337_BYTE_SECS]   = bin2bcd(iRtcSec);
    XLOGW("pt7c4337 RTC_SetTime  Y-%d M-%d D-%d W-%d H-%d m-%d \n", s32Year, s32Month, s32Date, s32Week, s32Hour, s32Minute);
    XLOGW("pt7c4337 RTC_SetTime  I2C  Y-0x%x M-0x%x D-0x%x W-0x%x H-0x%x m-0x%x\n", relTime[PT7C4337_BYTE_YEAR], relTime[PT7C4337_BYTE_MONTH], relTime[PT7C4337_BYTE_DAY],
                relTime[PT7C4337_BYTE_WDAY], relTime[PT7C4337_BYTE_HOURS], relTime[PT7C4337_BYTE_MINS]);
    s32Ret = XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_IC, PT7C4337_I2C_SECS_REG, 1, sizeof(relTime), relTime);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 RTC_SetTime write time error\n");
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
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(PT7C4337_IC, PT7C4337_CMD_CONTROL_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 RTC_SetAlarmTime Read CMD_CONTROL error\n");
        return XBH_FAILURE;
    }
    iRtcEnable[0] &= ~PT7C4337_ALARM_ENABLE_BIT;
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_IC, PT7C4337_CMD_CONTROL_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 RTC_SetAlarmTime  write CMD_CONTROL error\n");
        return XBH_FAILURE;
    }

    // alarm time
    memset(iRtcAlarmTime, 0, sizeof(iRtcAlarmTime));
    iRtcAlarmTime[PT7C4337_ALARM_BYTE_SECS]   = bin2bcd(0x00);
    iRtcAlarmTime[PT7C4337_ALARM_BYTE_MINS]   = bin2bcd(iRtcMinute);
    iRtcAlarmTime[PT7C4337_ALARM_BYTE_HOURS]  = bin2bcd(iRtcHour);
    iRtcDate &= ~PT7C4337_ALARM_DAY_DATE_BIT;
    iRtcAlarmTime[PT7C4337_ALARM_BYTE_DAY]    = bin2bcd(iRtcDate);
    XLOGW("pt7c4337 RTC_SetAlarmTime  Enable=%d D-%d H-%d M-%d \n", bEnable, s32Date, s32Hour, s32Minute);
    XLOGW("pt7c4337 RTC_SetAlarmTime  I2C  D-0x%x H-0x%x M-0x%x\n", iRtcAlarmTime[PT7C4337_ALARM_BYTE_DAY], iRtcAlarmTime[PT7C4337_ALARM_BYTE_HOURS], iRtcAlarmTime[PT7C4337_ALARM_BYTE_MINS]);
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_I2C_ADDR_WR, PT7C4337_ALARM_REG_SECS, 1, sizeof(iRtcAlarmTime), iRtcAlarmTime);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 RTC_SetAlarmTime  Write alarm time error\n");
        return XBH_FAILURE;
    }

    // alarm enable
    memset(iRtcEnable, 0, sizeof(iRtcEnable));
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(PT7C4337_IC, PT7C4337_CMD_CONTROL_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 read alarm enable error\n");
        return XBH_FAILURE;
    }
    if (enable == XBH_TRUE)
    {
        iRtcEnable[0] |= PT7C4337_ALARM_ENABLE_BIT;
    }
    else
    {
        iRtcEnable[0] &= ~PT7C4337_ALARM_ENABLE_BIT;
    }
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_IC, PT7C4337_CMD_CONTROL_REG, 1, sizeof(iRtcEnable), iRtcEnable);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 write alarm enable error\n");
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
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(PT7C4337_IC, PT7C4337_I2C_SECS_REG, 1, sizeof(relTime), relTime);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGW("pt7c4337 RTC_GetTime Read i2c error\n");
        return XBH_FAILURE;
    }

    iRtcSec    = bcd2bin(relTime[PT7C4337_BYTE_SECS]);
    iRtcMinute = bcd2bin(relTime[PT7C4337_BYTE_MINS]);
    iRtcHour   = bcd2bin(relTime[PT7C4337_BYTE_HOURS]);
    iRtcWeek   = bcd2bin(relTime[PT7C4337_BYTE_WDAY])-1;
    iRtcDate   = bcd2bin(relTime[PT7C4337_BYTE_DAY]);
    iRtcMonth  = bcd2bin(relTime[PT7C4337_BYTE_MONTH]);
    iRtcYear   = bcd2bin(relTime[PT7C4337_BYTE_YEAR]);

    //year range is 100 ~ 199
    iRtcYear = iRtcYear + 100;
    XLOGW("pt7c4337 RTC_GetTime convert base 1900->2000\n");

    XLOGD("pt7c4337 iRtcSec: %d\n",    iRtcSec);
    XLOGD("pt7c4337 iRtcMinute: %d\n", iRtcMinute);
    XLOGD("pt7c4337 iRtcHour: %d\n",   iRtcHour);
    XLOGD("pt7c4337 iRtcWeek: %d\n",   iRtcWeek);
    XLOGD("pt7c4337 iRtcDate: %d\n",   iRtcDate);
    XLOGD("pt7c4337 iRtcMonth: %d\n",  iRtcMonth);
    XLOGD("pt7c4337 iRtcYear: %d\n",   iRtcYear);

    if (iRtcSec > 59 || iRtcMinute > 59 || iRtcHour > 23 || iRtcWeek > 6
        || iRtcDate > 31 || iRtcMonth > 12 || iRtcYear > 199 || iRtcYear < 100)
    {
        XLOGW("pt7c4337 RTC_GetTime value is error.\n");
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
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  buf[1]={0};
    
    XLOGD(" begin ");

    //Enable oscillator and time count chain
    buf[0] = 0x1D;
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_IC, PT7C4337_CMD_CONTROL_REG, 1, sizeof(buf), buf);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("pt7c4337 RTC_Init enable oscillator and time count chain error\n");
        return XBH_FAILURE;
    }
    //clear Oscillator Stop Flag
    buf[0] = 0x00;
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_IC, PT7C4337_CMD_STATUS_REG, 1, sizeof(buf), buf);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("pt7c4337 RTC_Init clear Oscillator Stop Flag error\n");
        return XBH_FAILURE;
    }
    //set 24 hour mode
    buf[0] = 0x00;
    s32Ret |= XbhService::getModuleInterface()->getRtcI2cData(PT7C4337_IC, PT7C4337_I2C_HOURS_REG, 1, sizeof(buf), buf);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("pt7c4337 RTC_Init read  24 hour mode error\n");
        return XBH_FAILURE;
    } 

    buf[0] &= ~PT7C4337_HOURS_24H_BIT;
    s32Ret |= XbhService::getModuleInterface()->setRtcI2cData(PT7C4337_IC, PT7C4337_I2C_HOURS_REG, 1, sizeof(buf), buf);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("pt7c4337 RTC_Init set 24 hour mode error\n");
        return XBH_FAILURE;
    }
    XLOGD(" end ");
    return XBH_SUCCESS;
}

static XBH_VOID RTC_Deinit(XBH_VOID)
{
    return;
}

RTC_DRV_FUNC_S stPt7c4337Func =
{
    .pfnRtcSetTime         = RTC_SetTime,
    .pfnRtcSetAlarmTime    = RTC_SetAlarmTime,
    .pfnRtcGetTime         = RTC_GetTime,
    .pfnRtcDeinit          = RTC_Deinit,
    .pfnRtcInit            = RTC_Init,
};


