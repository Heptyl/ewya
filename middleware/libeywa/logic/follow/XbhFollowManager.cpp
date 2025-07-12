#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhFollowManager"

#include "XbhLog.h"
#include "XbhFollowManager.h"
#include "XbhService.h"
#include "XbhFrontBoard.h"
#include "XbhSourceManager.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <cutils/properties.h>

XbhFollowManager*       XbhFollowManager::mInstance = NULL;
XbhMutex                XbhFollowManager::mLock;

XBH_S32 XbhFollowManager::setFollowPort(XBH_SOURCE_E u32Source)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    mChangeToSrc = u32Source;
    postMsg(0, &u32Source, 1, 0, this);
    return s32Ret;
}

static XBH_BOOL initFollow = XBH_FALSE;

void XbhFollowManager::run(const void* arg)
{
    XBH_U32 msgType;
    XBH_U32 size;
    if(initFollow == XBH_FALSE)
    {
        //gpio init
        XbhService::getModuleInterface()->followToInit();
        //防止部分硬件初始化的状态错误，此处需要进行一次开机后的默认设置，比如连在一起的USB2.0和3.0设备，就需要先初始化3.0的设备，防止只能识别成2.0的设备
        //Some hardware needs to be reset or initialized after startup, for example, the USB 3.0 terminal is connected to both 2.0 and 3.0 USB drives through a distributor,
        //and only 2.0 USB drives can be recognized after startup
        XbhFrontBoard::getInstance()->followToAndroidExtBefore();
        XbhService::getModuleInterface()->followToAndroid();
        XbhFrontBoard::getInstance()->followToAndroidExtAfter();
        initFollow = XBH_TRUE;
    }
    if(getMsg(&msgType, &mChangeToSrc, &size, this))
    {
        if(mCurrentSrc != mChangeToSrc)
        {
            mCurrentSrc = mChangeToSrc;
            XLOGD(" XbhFollowManager  mChangeToSrc =  %d  \n", mChangeToSrc);
            switch(mChangeToSrc) {
                case XBH_SOURCE_ANDROID:
                    XbhFrontBoard::getInstance()->followToAndroidExtBefore();
                    XbhService::getModuleInterface()->followToAndroid();
                    XbhFrontBoard::getInstance()->followToAndroidExtAfter();
                    break;
                case XBH_SOURCE_HDMI1:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToHdmi1();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_HDMI2:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToHdmi2();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_HDMI3:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToHdmi3();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_HDMI4:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToHdmi4();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_HDMI5:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToHdmi5();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_HDMI6:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToHdmi6();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_F_HDMI1:
                    XbhFrontBoard::getInstance()->followToFHdmi1ExtBefore();
                    XbhService::getModuleInterface()->followToFHdmi1();
                    XbhFrontBoard::getInstance()->followToFHdmi1ExtAfter();
                    break;
                case XBH_SOURCE_F_HDMI2:
                    XbhFrontBoard::getInstance()->followToFHdmi2ExtBefore();
                    XbhService::getModuleInterface()->followToFHdmi2();
                    XbhFrontBoard::getInstance()->followToFHdmi2ExtAfter();
                    break;
                case XBH_SOURCE_F_HDMI3:
                    XbhFrontBoard::getInstance()->followToFHdmi3ExtBefore();
                    XbhService::getModuleInterface()->followToFHdmi3();
                    XbhFrontBoard::getInstance()->followToFHdmi3ExtAfter();
                    break;
                case XBH_SOURCE_F_HDMI4:
                    XbhService::getModuleInterface()->followToFHdmi4();
                    break;
                case XBH_SOURCE_USBC1:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToUsbc1();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_USBC2:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToUsbc2();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_USBC3:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToUsbc3();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_USBC4:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToUsbc4();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_F_USBC1:
                    XbhFrontBoard::getInstance()->followToFUsbc1ExtBefore();
                    XbhService::getModuleInterface()->followToFUsbc1();
                    XbhFrontBoard::getInstance()->followToFUsbc1ExtAfter();
                    break;
                case XBH_SOURCE_F_USBC2:
                    XbhFrontBoard::getInstance()->followToFUsbc2ExtBefore();
                    XbhService::getModuleInterface()->followToFUsbc2();
                    XbhFrontBoard::getInstance()->followToFUsbc2ExtAfter();
                    break;
                case XBH_SOURCE_F_USBC3:
                    XbhService::getModuleInterface()->followToFUsbc3();
                    break;
                case XBH_SOURCE_F_USBC4:
                    XbhService::getModuleInterface()->followToFUsbc4();
                    break;
                case XBH_SOURCE_DP1:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToDP1();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_DP2:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToDP2();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_DP3:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToDP3();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_DP4:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToDP4();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_F_DP1:
                    XbhService::getModuleInterface()->followToFDP1();
                    break;
                case XBH_SOURCE_F_DP2:
                    XbhService::getModuleInterface()->followToFDP2();
                    break;
                case XBH_SOURCE_F_DP3:
                    XbhService::getModuleInterface()->followToFDP3();
                    break;
                case XBH_SOURCE_F_DP4:
                    XbhService::getModuleInterface()->followToFDP4();
                    break;
                case XBH_SOURCE_OPS1:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToOps1();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_OPS2:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToOps2();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_SDM1:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToSdm1();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_SDM2:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToSdm2();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_VGA1:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToVga1();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                case XBH_SOURCE_VGA2:
                    XbhFrontBoard::getInstance()->followToSrcExtBefore();
                    XbhService::getModuleInterface()->followToVga2();
                    XbhFrontBoard::getInstance()->followToSrcExtAfter();
                    break;
                default:
                    XLOGE("error port !!!! mChangeToSrc = %d ", mChangeToSrc);
                    break;
            }
            XbhSourceManager::getInstance()->setFollowPort(mCurrentSrc);  //告知XbhSourceManager
            //lango array MIC
            static XBH_BOOL bExtUac = XBH_FALSE;
            char prop[PROPERTY_VALUE_MAX];
            property_get("persist.vendor.xbh.array.mic", prop, "null");
            if (!bExtUac && strcmp(prop, "lango") == 0)
            {
                XLOGI("------------ current array MIC is lango ------------");
                property_set("ctl.stop", "uac_app");
                property_set("ctl.stop", "uac_app_otg");
                sleep(1);

                //if other UAC devices exist, don't enable lango UAC
                if (access("/proc/asound/card2/usbid", F_OK) < 0)
                {//not exists
                    if (mChangeToSrc != XBH_SOURCE_ANDROID)
                    {
                        //property_set("sys.usb.config", "uac2");
                        //property_set("sys.usb.configfs", "1");
                        property_set("vendor.xbh.audproc.otg", "true");
                        sleep(1);
                        //char idStr[32];
                        //if (XbhFramework::getInstance()->getCardIdByName(CARD_NAME_UAC2GADGET, idStr) >= 0)
                        //    property_set("vendor.xbh.uac2.card.id", idStr);
                        property_set("ctl.start", "uac_app_otg");
                    }
                    else
                    {
                        //property_set("sys.usb.config", "adb");
                        property_set("vendor.xbh.audproc.otg", "false");
                    }
                }
                else
                {
                    bExtUac = XBH_TRUE;
                }
            }
            //加载UAC声卡
            property_set("vendor.xbh.usb.adb.uvc.uac", "1");
            //lango array MIC
        }
    }
}

XbhFollowManager::XbhFollowManager()
{
    XLOGD(" begin ");
    //default source is android
    mCurrentSrc = XBH_SOURCE_ANDROID;
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhFollowManager::~XbhFollowManager()
{
}

XbhFollowManager* XbhFollowManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhFollowManager();
    }
    return mInstance;
}

