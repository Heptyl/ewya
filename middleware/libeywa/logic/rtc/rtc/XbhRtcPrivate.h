#ifndef __XBH_DRV_RTC_PRIVATE_H__
#define __XBH_DRV_RTC_PRIVATE_H__

#include "XbhType.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/***************************** Macro Definition ******************************/


/***************************** Struct Definition ******************************/
/* Global parameter */
typedef struct tagRTC_GLOBAL_PARAM_S
{
    XBH_U8                u8DeviceType;
} RTC_GLOBAL_PARAM_S;

typedef XBH_S32(*FN_RTC_SETTIME)(XBH_U32 s32Year, XBH_U32 s32Month, XBH_U32 s32Date, XBH_U32 s32Week, XBH_U32 s32Hour, XBH_U32 s32Minute, XBH_U32 s32Sec);
typedef XBH_S32(*FN_RTC_SETALARMTIME)(XBH_U32 s32Date, XBH_U32 s32Week, XBH_U32 s32Hour, XBH_U32 s32Minute, XBH_BOOL bEnable);
typedef XBH_S32(*FN_RTC_GETTIME)(XBH_U32* ps32Year, XBH_U32* ps32Month, XBH_U32* ps32Date, XBH_U32* ps32Week, XBH_U32* ps32Hour, XBH_U32* ps32Minute, XBH_U32* ps32Sec);
typedef XBH_VOID(*FN_RTC_DEINIT)(XBH_VOID);
typedef XBH_S32(*FN_RTC_INIT)();

 
typedef struct
{
    FN_RTC_SETTIME               pfnRtcSetTime;
    FN_RTC_SETALARMTIME          pfnRtcSetAlarmTime;
    FN_RTC_GETTIME               pfnRtcGetTime;
    FN_RTC_DEINIT                pfnRtcDeinit;
    FN_RTC_INIT                  pfnRtcInit;
} RTC_DRV_FUNC_S;

/**Defines the type of RTC.*/
/**CNcomment:定义RTC设备类型*/
typedef enum xbhDRV_RTC_E
{
    XBH_RTC_NONE = 0,
    XBH_RTC_HYM8563 = 1,
    XBH_RTC_PT7C4337 = 2,
    
} XBH_DRV_RTC_E;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __XBH_DRV_RTC_PRIVATE_H__ */

