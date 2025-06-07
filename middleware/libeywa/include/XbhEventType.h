#ifndef __XBH_MW_EVENT_TYPE_H__
#define __XBH_MW_EVENT_TYPE_H__

enum XBH_EVENT_TYPE_E
{
    /* 
     * eywa 传到  XbhApi里面去 
     * 跟InternalMessage.java里面的值保持一致
    */
    XBH_EVT_MSG_INTERNAL                            = 0X0000,     /* 0x0000 */
    XBH_SRC_EVT_PLUGIN,                                 /* 0X0001 plug in */
    XBH_SRC_EVT_PLUGOUT,                                /* 0X0002 plug out */
    XBH_SRC_EVT_SIGNALCHANGE,                           /* 0X0003 signal change */

    /* 
     * eywa 传到中间件  里面去 
     * 跟UserMessage.java里面的值保持一致
    */
    XBH_EVT_MSG_USER                                = 0X8000,     /* 0x8000 */
    XBH_HP_EVT_PLUGIN,                                 /* 0X8001 耳机plug in */
    XBH_HP_EVT_PLUGOUT,                                /* 0X8002 耳机plug out */
    
    XBH_OPS_DEV_PLUGIN,                                /* 0X8003 ops device plug in */
    XBH_OPS_DEV_PLUGOUT,                               /* 0X8004 ops device plug out */
    
    XBH_MIC_EVT_PLUGIN,                                /* 0X8005 模拟MIC plug in */
    XBH_MIC_EVT_PLUGOUT,                               /* 0X8006 模拟MIC plug out */
    
    XBH_ARC_EVT_PLUGIN,                                /* 0X8007 ARC plug in */
    XBH_ARC_EVT_PLUGOUT,                               /* 0X8008 ARC plug out */

    XBH_OPS_DEV_POWERON,                               /* 0X8009 ops device POWER ON */
    XBH_OPS_DEV_POWEROFF,                              /* 0X800A ops device POWER OFF */

    XBH_EVT_MSG_END,
};

#endif

