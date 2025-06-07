#ifndef __HI_MW_ISOLUTION_SDK_X_610A_H__
#define __HI_MW_ISOLUTION_SDK_X_610A_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"
#include "XbhMWThread.h"

//#define XBH_TOUCH_DATA_NODE "/dev/isolution-touchE"

#define XBH_TOUCH_WIDTH             (32767)
#define XBH_TOUCH_HEIGTH            (32767)
#define XBH_SOC_OSD_WIDTH_FULL      (1920)
#define XBH_SOC_OSD_HEIGTH_FULL     (1080)
#define XBH_SOC_OSD_WIDTH_4K        (3840)
#define XBH_SOC_OSD_HEIGTH_4K       (2160)
#define XBH_SOC_OSD_WIDTH_5K        (5210)
#define XBH_SOC_OSD_HEIGTH_5K       (2160)

#define Solution_StartPointId           0xA1
#define Solution_Cammand_Byte           0xE7
#define Solution_PackageID              0x02
#define Solution_PointStateDown         0x07
#define Solution_PointStateUp           0x04
#define Solution_PointStateNULL         0x00

// 帧长度
#define Solution_MAX_FRAME_LENGTH       64
// 一帧最大点数
#define Solution_POINTNUMMAX            6
// 点数据长度
#define Solution_POINTDDATALENGTH       10
// 最大帧数
#define Solution_MAX_FRAME_NUMBER       4

#define MIN_OF_TOW_VALUE(x, y)      ((x) > (y) ? (y) : (x))

class XbhTouchIsolutionXSdk: public XbhTouchInterface, public XbhMWThread
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
    * 设置触摸框的防误触功能开关
    */
    XBH_S32 setPalmRejection(XBH_BOOL enable);

    /**
    * 获取触摸框的防误触功能开关
    */
    XBH_S32 getPalmRejection(XBH_BOOL* enable);

    /**
     * 写数据接口
     */
    XBH_S32 writData(XBH_S32 fd, XBH_U8 *data, XBH_S32 len);

    /**
     * 读数据接口
     */
    XBH_S32 readData(XBH_S32 fd, XBH_U8 *data, XBH_S32 len);
private:
    enum XBH_TOUCH_STAT{
        XBH_TOUCH_UNCONNECT = -1,
        XBH_TOUCH_LOAD_MODULE = 0,
        XBH_TOUCH_OPEN_DATA_NODE = 1,
        XBH_TOUCH_RUNNING_IN_ANDROID = 2,
        XBH_TOUCH_RUNNING_IN_SOURCE = 3,
    };
    XBH_TOUCH_STAT xbhTouchStat = XBH_TOUCH_OPEN_DATA_NODE;
    XBH_S32 openTp(void);
    XBH_S32 sendCommand(XBH_U8* data, XBH_S32 len);
    XbhTouchIsolutionXSdk(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchIsolutionXSdk();
    static XbhTouchIsolutionXSdk *mInstance;
    static XbhMutex mLockObject;
    void* mTouchHandle;

    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
    XBH_S32 mFd;
    XBH_S32 mFdCmd;
};
#endif
