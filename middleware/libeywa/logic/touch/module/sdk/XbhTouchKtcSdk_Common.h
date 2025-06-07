#ifndef __HI_MW_XBHKTC_SDK_COMMON_H__
#define __HI_MW_XBHKTC_SDK_COMMON_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhTouchInterface.h"

typedef struct MatchTouchNode{
    XBH_U32 usb1_vid; //第一路USB的 VID
    XBH_U32 usb1_pid; //第一路USB的 PID
    XBH_U32 usb2_vid; //第二路USB的 VID
    XBH_U32 usb2_pid; //第二路USB的 VID
    XBH_U8 ctrl; //控制类型
    XBH_U32 match1; //第一路USB的VID PID匹配，为0时表示需要去匹配，为1时不需要匹配
    XBH_U32 match2; //第二路USB的VID PID匹配，为0时表示需要去匹配，为1时不需要匹配
    XBH_CHAR ko[32]; //需要加载的ko的文件名称
    XBH_CHAR name[32]; //触摸框的名称
}MatchTouchNode;

class XbhTouchKtcSdk_Common: public XbhTouchInterface
{
public:
    static XbhTouchKtcSdk_Common *getInstance(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    XBH_S32 insmod(const XBH_CHAR *filename, const XBH_CHAR *args);
    XBH_S32 loadModules(XBH_VOID);
    XBH_S32 getAllUsbDevices();
    XBH_S32 getTouchPanelConnect(XBH_BOOL* enable);

private:
    XbhTouchKtcSdk_Common(XBH_CHAR *ko = XBH_NULL, XBH_CHAR *tpName = XBH_NULL);
    ~XbhTouchKtcSdk_Common();

    static XbhTouchKtcSdk_Common *mInstance;
    static XbhMutex mLockObject;
    
    XBH_CHAR mKoName[32];
    XBH_CHAR mTpName[32];
};
#endif