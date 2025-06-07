#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM9679_D"

#include "XbhLog.h"
#include "XbhXMM9679_D.h"
#include "XbhGSensorManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhGSensorManager.h"
#include "XbhMcuManager.h"


#include <string.h>

XbhXMM9679_D            *XbhXMM9679_D::mInstance = XBH_NULL;
XbhMutex                 XbhXMM9679_D::mLock;

#define SENSORDATESIZE 20
#define SENSORDATESIZE_AM1008  32

static XBH_U32 mUsbCBoard = 1;

XbhXMM9679_D::XbhXMM9679_D()
{
    XLOGD(" begin ");
    XLOGD(" end ");
}

XbhXMM9679_D::~XbhXMM9679_D()
{
}


XbhXMM9679_D *XbhXMM9679_D::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM9679_D();
        }
    }
    return mInstance;
}
