#ifndef __HI_MW_XBHTIMELINKH3_SDK_H__
#define __HI_MW_XBHTIMELINKH3_SDK_H__

#include "XbhCommon.h"
#include "XbhMutex.h"
#include "XbhTouchInterface.h"
#include "XbhType.h"

class XbhTouchTimeLinkH3Sdk : public XbhTouchInterface
{
  public:
    static XbhTouchTimeLinkH3Sdk *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
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
     * 设置触摸框的防误触功能开关
     */
    XBH_S32 setPalmRejection(XBH_BOOL enable);

    /**
     * 获取触摸框的防误触功能开关
     */
    XBH_S32 getPalmRejection(XBH_BOOL *enable);

    /**
     * 发送数据到触摸框
     */
    XBH_S32 sendDataToTp(XBH_U32 length, XBH_U32 *touchData);

  private:
    XBH_S32 openTp(XBH_VOID);
    XBH_S32 insmod(const char *filename, const char *args);
    XBH_S32 sendCommand(XBH_U8 *cmd, XBH_S32 len);
    XBH_S32 readCommand(XBH_U8 *pbuf, XBH_S32 len);
    XbhTouchTimeLinkH3Sdk(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchTimeLinkH3Sdk();

    static XbhTouchTimeLinkH3Sdk *mInstance;
    static XbhMutex mLockObject;

    void* mTouchHandle;
    void (*mInitProcess)(void);

    XBH_BOOL isInitTouch;
    XBH_BOOL isSrcTouchEnable;
    XBH_BOOL isPalmRejectionEnable;
    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
};
#endif
