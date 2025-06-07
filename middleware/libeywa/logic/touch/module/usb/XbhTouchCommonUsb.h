#ifndef __HI_MW_XBHTOUCHCOMMONUSB_H__
#define __HI_MW_XBHTOUCHCOMMONUSB_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"

class XbhTouchCommonUsb: public XbhTouchInterface
{
public:
    static XbhTouchInterface *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL,XBH_U32 vid = 0xFFFF,XBH_U32 pid = 0xFFFF);
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

private:
    XBH_S32 openTp(XBH_VOID);
    XBH_S32 sendCommand(XBH_U8* data, XBH_S32 len);
    XBH_S32 loadModules(XBH_VOID);
    XBH_S32 initTouchProcess(XBH_U32 vid,XBH_U32 pid);
    XbhTouchCommonUsb(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL,XBH_U32 vid = 0xFFFF,XBH_U32 pid = 0xFFFF);
    ~XbhTouchCommonUsb();
    static XbhTouchCommonUsb *mInstance;
    static XbhMutex mLockObject;


    XBH_VOID (*mInitProcess)(XBH_U32 vid, XBH_U32 pid);
    XBH_S32 (*mSetTouchState)(XBH_S32 type, XBH_S32 enable);
    XBH_S32 (*mGetTouchState)(XBH_S32 type);
    XBH_S32 (*mSetNonThroughTouchRegion)(XBH_S32 id, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h, XBH_S32 is4kUi);
    XBH_S32 (*mRemoveNonThroughTouchRegion)(XBH_S32 id,XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h, XBH_S32 is4kUi);
    XBH_S32 (*mRemoveNonThroughTouchRegionById)(XBH_S32 id);
    XBH_S32 (*mIsTouchPanelOk)(XBH_VOID);
    XBH_S32 (*mSetScreenOffsetHV)(XBH_S32 offsetX,XBH_S32 offsetY,XBH_S32 is4kUi);
    XBH_S32 (*mSetPalmType)(XBH_S32 type);
    void* mTouchHandle;

    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
    //XBH_S32 mFd;
    XBH_U32 mVid = 0xFFFF;
    XBH_U32 mPid = 0xFFFF;
};
#endif
