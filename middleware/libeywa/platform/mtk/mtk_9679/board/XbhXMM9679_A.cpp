#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_A"

#include "XbhLog.h"
#include "XbhXMM9679_A.h"

#include "mi_common.h"
#include "mi_inputsrc.h"

XbhXMM9679_A            *XbhXMM9679_A::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_A::mLock;

//----------------------------------------------------- static function begin ------------------------------------------------------------------------------------------------------
/*
 *  用户层信号状态定义
 *  public static final int SIGSTAT_SUPPORT                 = 0;
 *  public static final int SIGSTAT_NOSIGNAL                = 1;
 *  public static final int SIGSTAT_UNSUPPORT               = 2;
 *  public static final int SIGSTAT_UNSTABLE                = 3;
 *  public static final int SIGSTAT_SRCCOMPLETE             = 4;
 *  
 *  MI 接口的信号状态定义
 *  E_MI_INPUTSRC_SIGNAL_STATUS_STABLE = 0,
 *  E_MI_INPUTSRC_SIGNAL_STATUS_UNSTABLE,
 *  E_MI_INPUTSRC_SIGNAL_STATUS_NOSIGNAL
*/
//----------------------------------------------------- static function end---------------------------------------------------------------------------------------------------------
XBH_S32 XbhXMM9679_A::getHdmi1Det(XBH_BOOL *enable)
{
    return getHdmiDirectDet(XBH_SOURCE_HDMI1, enable);
}

XBH_S32 XbhXMM9679_A::getHdmi2Det(XBH_BOOL *enable)
{
    return getHdmiDirectDet(XBH_SOURCE_HDMI2, enable);
}

XBH_S32 XbhXMM9679_A::getHdmi3Det(XBH_BOOL *enable)
{
    return getHdmiDirectDet(XBH_SOURCE_HDMI3, enable);
}

XBH_S32 XbhXMM9679_A::getHdmi4Det(XBH_BOOL *enable)
{
    return getHdmiDirectDet(XBH_SOURCE_HDMI4, enable);
}

//----------------------------------------------------------------------------- private func start here -----------------------------------------------------------------
XbhXMM9679_A::XbhXMM9679_A()
{
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMM9679_A::~XbhXMM9679_A()
{
}

XbhXMM9679_A *XbhXMM9679_A::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_A();
        }
    }
    return mInstance;
}

