#ifndef __XBH_TOUCH_INTERFACE_H__
#define __XBH_TOUCH_INTERFACE_H__

#include "XbhType.h"

class XbhTouchInterface 
{
public:
    /**
     * 设置信号源的触摸开关。
     * enable 开关状态
     */
    virtual XBH_S32 setSourceTouchState(XBH_BOOL enable);

    /**
     * 获取信号源的触摸开关。
     * boolean 开关状态
     */
    virtual XBH_S32 getSourceTouchState(XBH_BOOL *enable);

    /**
     * 设置不透传区域
     * id: 当前区域的id
     * x,y,w,h 当前区域的矩形参数
     * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
     */
    virtual XBH_S32 setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 移除不透传区域
     * id: 当前区域的id
     */
    virtual XBH_S32 deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
    * 设置触摸区域，可用作实现屏幕下移或者信源小窗口时的信源触摸位置
    */
    virtual XBH_S32 setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 发送按键给触摸框，由触摸框转发
     */
    virtual XBH_S32 sendKeyToTp(XBH_U32 keyCode);

    /**
    * 发送数据到触摸框
    */
    virtual XBH_S32 sendDataToTp(XBH_U32 length, XBH_U32 *touchData);
    
    /**
    * 设置触摸框的防误触功能开关
    */
    virtual XBH_S32 setPalmRejection(XBH_BOOL enable);

    /**
    * 获取触摸框的防误触功能开关
    */
    virtual XBH_S32 getPalmRejection(XBH_BOOL* enable);

    /**
    * 获取触摸框的连接状态
    */
    virtual XBH_S32 getTouchPanelConnect(XBH_BOOL* enable);

    /**
    * 获取触摸框的名称
    */
    virtual XBH_S32 getTouchPanelName(XBH_CHAR *name);

    /**
    * 获取触摸框驱动版本
    */
    virtual XBH_S32 getTpDriverVersion(XBH_CHAR *version);

    /**
    * 获取触摸框固件版本
    */
    virtual XBH_S32 getTpFirmwareVersion(XBH_CHAR *version);
public:
    XbhTouchInterface(){};
    virtual ~XbhTouchInterface(){};
};

#endif