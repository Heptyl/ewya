#ifndef __XBH_TOUCH_INTERFACE_AUTO_H__
#define __XBH_TOUCH_INTERFACE_AUTO_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"
#include "XbhTouchControlInterface.h"

// XBH_TP_IF_UART_CTRL,    //触摸框支持串口控制
// XBH_TP_IF_MCU_CTRL,     //控制MCU来开关信源的触摸
// XBH_TP_IF_SDK_CTRL,     //触摸框支持SDK包
// XBH_TP_IF_USB_CTRL,     //触摸框支持USB协议

#define USB_DIR "/sys/bus/usb/devices"

typedef struct autoMatchTouchNode{
    XBH_U32 usb1_vid;       //第一路USB的 VID
    XBH_U32 usb1_pid;       //第一路USB的 PID
    XBH_U32 usb2_vid;       //第二路USB的 VID
    XBH_U32 usb2_pid;       //第二路USB的 VID
    XBH_U8 ctrl;            //控制类型
    XBH_U32 match1;         //第一路USB的VID PID匹配，为0时表示需要去匹配，为1时不需要匹配
    XBH_U32 match2;         //第二路USB的VID PID匹配，为0时表示需要去匹配，为1时不需要匹配
    XBH_CHAR ko[32];        //需要加载的ko的文件名称
    XBH_CHAR name[32];      //触摸框的名称
}autoMatchTouchNode;

typedef struct deviceInfolist {
    XBH_U32 device_vid;
    XBH_U32 device_pid;
}deviceInfolist;

class XbhTouchInterfaceAuto: public XbhTouchControlInterface
{
public:
    static XbhTouchControlInterface *getInstance();
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
     * is4k: 主要是针对有2K 4KUI混合的场景
     */
    XBH_S32 setNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 移除不透传区域
     * id: 当前区域的id
     */
    XBH_S32 deleteNonThroughTouchRegion(XBH_S32 id, XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 设置信源的触摸区域，可实现屏幕下移功能的windows触摸下移
     */
    XBH_S32 setTouchScaleRect(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
    * 设置触摸缩放区域
    * x,y,w,h 当前区域的矩形参数
    * resolution: 分辨率, 0：1920x1080 1:3840x2160 2 5210x2160
    * 该接口可用来实现触摸缩放或者设置信源小窗时的触摸
    * 需要使用屏幕下移时须调用setTouchScaleRect
    */
    XBH_S32 setTouchScalingRegion(XBH_S32 resolution, XBH_S32 x, XBH_S32 y, XBH_S32 w, XBH_S32 h);

    /**
     * 发送按键给触摸框，然后触摸框发给OPS
     */
    XBH_S32 sendKeyToTp(XBH_U32 keyCode);

    /**
    * 发送数据到触摸框
    */
    XBH_S32 sendDataToTp(XBH_U32 length, XBH_U32 *touchData);

    /**
    * 设置触摸框的防误触功能开关
    */
    XBH_S32 setPalmRejection(XBH_BOOL enable);

    /**
    * 获取触摸框的防误触功能开关
    */
    XBH_S32 getPalmRejection(XBH_BOOL* enable);

    /**
    * 获取触摸框的连接状态
    */
    XBH_S32 getTouchPanelConnect(XBH_BOOL* enable);
    /**
    * 获取触摸框的名称
    */
    XBH_S32 getTouchPanelName(XBH_CHAR *name);
    /**
    * 获取触摸框驱动版本
    */
    XBH_S32 getTpDriverVersion(XBH_CHAR *version);
    /**
    * 获取触摸框固件版本
    */
    XBH_S32 getTpFirmwareVersion(XBH_CHAR *version);
private:
    XBH_S32 Init(XBH_VOID);
    XBH_S32 getAllUsbDevices(void);
    XBH_S32 matchUsbNum();
    XBH_S32 getTouchInterface(void);
    XbhTouchInterfaceAuto();
    ~XbhTouchInterfaceAuto();
    
    static XbhTouchInterfaceAuto *mInstance;
    static XbhMutex mLockObject;

    XbhTouchInterface *m_pXbhTouchInterfaceAuto;
};
#endif //__XBH_TOUCH_INTERFACE_AUTO_H__
