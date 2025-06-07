#ifndef __HI_MW_XBHYCLTOUCH_UART_H__
#define __HI_MW_XBHYCLTOUCH_UART_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"

class XbhTouchYclUart : public  XbhTouchInterface
{
public:
    static XbhTouchInterface *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    /**
     * 设置信号源的触摸开关。
     * enable 开关状态
     */
    XBH_S32 setSourceTouchState(XBH_BOOL enable);

    /**
     * 获取信号源的触摸开关。
     * boolean 开关状态
     */
    XBH_S32 getSourceTouchState(XBH_BOOL *enable);

    /**
     * 设置不透传区域
     * id: 当前区域的id
     * x,y,w,h 当前区域的矩形参数
     * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
     */
    XBH_S32 setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 移除不透传区域
     * id: 当前区域的id
     */
    XBH_S32 deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
    * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
    */
    XBH_S32 setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 发送按键给触摸框，由触摸框转发
     */
    XBH_S32 sendKeyToTp(XBH_U32 keyCode);
   
    /**
    * 获取触摸框防误触开关
    */
    XBH_S32 getPalmRejection(XBH_BOOL* enable);

    /**
     * 设置触摸框防误触开关
     */
    XBH_S32 setPalmRejection(XBH_BOOL enable);

private:
    XBH_S32 Init(XBH_VOID);
    XbhTouchYclUart(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchYclUart();
    static XbhTouchYclUart *mInstance;
    static XbhMutex mLockObject;
    
    void* mTouchHandle;
    XBH_S32 (*mInitProcess)(const XBH_CHAR* path, XBH_U32 baudrate);
    XBH_S32 (*mSetSourceTouchState)(XBH_BOOL enable);
    XBH_S32 (*mGetSourceTouchState)(XBH_BOOL *enable);
    XBH_S32 (*mSetNonThroughTouchRegion)(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);
    XBH_S32 (*mDeleteNonThroughTouchRegion)(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);
    XBH_S32 (*mSetTouchScaleRect)(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);
    XBH_S32 (*mSendKeyToTp)(XBH_U32 keyCode);
    XBH_S32 (*mSetPalmRejection)(XBH_BOOL enable);
    XBH_S32 (*mGetPalmRejection)(XBH_BOOL *enable);

    XBH_BOOL isInitTouch;
    XBH_BOOL isSrcTouchEnable;
    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];

};

#endif
