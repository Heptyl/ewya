#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhService"
#include "XbhLog.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/MemoryBase.h>
#include <utils/String8.h>
#include <hwbinder/Parcel.h>
#include <utils/Vector.h>
#include <cutils/properties.h>

#include "XbhService.h"
#include "XbhInstruct.h"

#include "XbhPlatformFactory.h"

#include "XbhSourceManager.h"
#include "XbhFollowManager.h"
#include "XbhTouchManager.h"
#include "XbhRtcManager.h"
#include "XbhDp2HdmiManager.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhVga2HdmiManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhAudioDetectManager.h"
#include "XbhNetSwitchManager.h"
#include "XbhRj45ToUsbManager.h"
#include "XbhAudioToI2sManager.h"
#include "XbhOpsManager.h"
#include "XbhGSensorManager.h"
#include "XbhLightSensorManager.h"
#include "XbhNfcManager.h"
#include "XbhAirSensorManager.h"
#include "XbhMcuManager.h"
#include "XbhFrontBoard.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhAVOutManager.h"
#include "XbhKernelMsgManager.h"
#include "XbhWatchDogManager.h"
#include "XbhPowerSensorManager.h"
//#include "hi_mw_dbo_factory.h"

using namespace android;

typedef XBH_S32 (*LocalAtomicPfn)(const Parcel &request, Parcel *reply);


typedef struct tagLocalAtomicInfo {
    XBH_U32 id;             /* interface id */
    LocalAtomicPfn pfn; /* interface function */
} LocalAtomicInfo;

static XbhPlatformInterface         *pXbhPlatformApiImpl;
static XbhModuleInterface           *pXbhModuleApiImpl;

XBH_BOOL XbhService::mVboToHdmiCreated;
XBH_BOOL XbhService::mCodecCreated;
XBH_BOOL XbhService::mAmpCreated;
XBH_BOOL XbhService::mAudioToI2sCreated;
XBH_BOOL XbhService::mNetSwitchCreated;
XBH_BOOL XbhService::mDp2HdmiCreated;
XBH_BOOL XbhService::mUsbc2HdmiCreated;
XBH_BOOL XbhService::mVga2HdmiCreated;

XbhPlatformFactory                  mXbhPlatformFactory;
XbhMutex                            XbhService::mNotifyLock;

//----------------------------------------------------------------------------- PICTURE start -------------------------------------------------------------------
static XBH_S32 setPictureMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPictureMode((XBH_PICMODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getPictureMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = XBH_PICMODE_STANDARD;
    s32Ret = pXbhPlatformApiImpl->getPictureMode((XBH_PICMODE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setBrightness(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBrightness(u32Value);
    return s32Ret;
}

static XBH_S32 getBrightness(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getBrightness(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setContrast(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setContrast(u32Value);
    return s32Ret;
}

static XBH_S32 getContrast(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getContrast(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSaturation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSaturation(u32Value);
    return s32Ret;
}

static XBH_S32 getSaturation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getSaturation(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSharpEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSharpEnable(u32Value);
    return s32Ret;
}

static XBH_S32 setSharpness(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSharpness(u32Value);
    return s32Ret;
}

static XBH_S32 getSharpness(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getSharpness(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setHue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHue(u32Value);
    return s32Ret;
}

static XBH_S32 getHue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getHue(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setBacklight(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBacklight(u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getBacklight(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 100;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getBacklight(&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setBacklightWithOutSave(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBacklightWithOutSave(u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getBacklightWithOutSave(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getBacklightWithOutSave(&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setColorTempMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setColorTempMode((XBH_COLORTEMP_E)u32Value);
    return s32Ret;
}

static XBH_S32 getColorTempMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getColorTempMode((XBH_COLORTEMP_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setNR(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setNR((XBH_LEVEL_E)u32Value);
    return s32Ret;
}

static XBH_S32 getNR(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getNR((XBH_LEVEL_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setGameModeEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGameModeEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getGameModeEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getGameModeEnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setPCMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPCMode((XBH_PCMODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getPCMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPCMode((XBH_PCMODE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setHDMIColorRange(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHDMIColorRange((XBH_COLOR_RANGE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getHDMIColorRange(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHDMIColorRange((XBH_COLOR_RANGE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setDCREnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setDCREnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getDCREnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getDCREnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMEMCLevel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMEMCLevel((XBH_LEVEL_E)u32Value);
    return s32Ret;
}

static XBH_S32 getMEMCLevel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getMEMCLevel((XBH_LEVEL_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setAspect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAspect((XBH_ASPECT_E)u32Value);
    return s32Ret;
}

static XBH_S32 getAspect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAspect((XBH_ASPECT_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getAvgPixelLuma(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAvgPixelLuma(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setGammaGroup(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGammaGroup(s32Value);
    return s32Ret;
}

static XBH_S32 getGammaGroup(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getGammaGroup(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 setColorTempPara(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_COLORTEMP_E enColorTemp = (XBH_COLORTEMP_E)request.readInt32();
    XBH_GAIN_OFFSET_DATA_S stInfo;
    stInfo.u32RedGain = request.readInt32();
    stInfo.u32GreenGain = request.readInt32();
    stInfo.u32BlueGain = request.readInt32();
    stInfo.u32RedOffset = request.readInt32();
    stInfo.u32GreenOffset = request.readInt32();
    stInfo.u32BlueOffset = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setColorTempPara(enColorTemp, &stInfo);
    return s32Ret;
}

static XBH_S32 getColorTempPara(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_COLORTEMP_E enColorTemp = (XBH_COLORTEMP_E)request.readInt32();
    XBH_GAIN_OFFSET_DATA_S stInfo;
    s32Ret = pXbhPlatformApiImpl->getColorTempPara(enColorTemp, &stInfo);
    reply->writeInt32(stInfo.u32RedGain);
    reply->writeInt32(stInfo.u32GreenGain);
    reply->writeInt32(stInfo.u32BlueGain);
    reply->writeInt32(stInfo.u32RedOffset);
    reply->writeInt32(stInfo.u32GreenOffset);
    reply->writeInt32(stInfo.u32BlueOffset);
    return s32Ret;
}

//------------------------------------------------------------------------ PICTURE end -----------------------------------------------------------------------

//------------------------------------------------------------------------ AUDIO start -----------------------------------------------------------------------
static XBH_S32 setSoundMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSoundMode((XBH_SOUND_MODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getSoundMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getSoundMode((XBH_SOUND_MODE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setVolume(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Channel = request.readInt32();
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setVolume((XBH_AUDIO_CHANNEL_E)u32Channel, u32Value);
    return s32Ret;
}

static XBH_S32 getVolume(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Channel = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getVolume((XBH_AUDIO_CHANNEL_E)u32Channel, &u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setAudioOutput(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAudioOutput((XBH_AUDIO_OUTPUT_E)u32Value);
    return s32Ret;
}

static XBH_S32 getAudioOutput(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAudioOutput((XBH_AUDIO_OUTPUT_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMute(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Channel = request.readInt32();
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMute((XBH_AUDIO_CHANNEL_E)u32Channel, (XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getMute(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Channel = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getMute((XBH_AUDIO_CHANNEL_E)u32Channel, (XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSpdifMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSpdifMode((XBH_SPDIF_MODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getSpdifMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getSpdifMode((XBH_SPDIF_MODE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSpdifEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSpdifEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getSpdifEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getSpdifEnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setARCMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setARCMode((XBH_ARC_MODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getARCMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getARCMode((XBH_ARC_MODE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setARCEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setARCEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getARCEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getARCEnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setPreScale(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPreScale(u32Value);
    return s32Ret;
}

static XBH_S32 getPreScale(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPreScale(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setAudioEq(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32EqMode = request.readInt32();
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAudioEq((XBH_EQ_MODE_E)u32EqMode, u32Value);
    return s32Ret;
}

static XBH_S32 getAudioEq(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32EqMode = request.readInt32();
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAudioEq((XBH_EQ_MODE_E)u32EqMode, &u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setBass(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBass(u32Value);
    return s32Ret;
}

static XBH_S32 getBass(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getBass(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setTreble(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setTreble(u32Value);
    return s32Ret;
}

static XBH_S32 getTreble(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getTreble(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setBalance(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBalance(u32Value);
    return s32Ret;
}

static XBH_S32 getBalance(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getBalance(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setLineOutMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setLineOutMode((XBH_LINEOUT_MODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getLineOutMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getLineOutMode((XBH_LINEOUT_MODE_E*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSurroundEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSurroundEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getSurroundEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getSurroundEnable((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setAvcEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAvcEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getAvcEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAvcEnable((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setEqEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setEqEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getEqEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getEqEnable((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setAmpMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAmpMode(u32Value);
    return s32Ret;
}

static XBH_S32 getAmpMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAmpMode(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getMicDetect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    XbhAudioDetectManager::getInstance()->getMicDetect((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getHeadphoneDetect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    XbhAudioDetectManager::getInstance()->getHeadphoneDetect((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getArcDetect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    XbhAudioDetectManager::getInstance()->getArcDetect((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setLineOutGain(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = XbhAudioCodecManager::getInstance()->setLineOutGain(u32Value);
    return s32Ret;
}

static XBH_S32 getLineOutGain(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = XbhAudioCodecManager::getInstance()->getLineOutGain(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMicInGain(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = XbhAudioCodecManager::getInstance()->setMicInGain(u32Value);
    return s32Ret;
}

static XBH_S32 getMicInGain(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicInGain(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMicHowling(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = XbhAudioCodecManager::getInstance()->setMicHowling((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getMicHowling(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicHowling((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSpeakerGain(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Channel = request.readInt32();
    XBH_S32 u32Value = request.readInt32();
    s32Ret = XbhAudioAmplifierManager::getInstance()->setGain((XBH_AUDIO_CHANNEL_E)u32Channel, u32Value);
    return s32Ret;
}

static XBH_S32 getSpeakerGain(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Channel = request.readInt32();
    XBH_S32 u32Value = 0;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getGain((XBH_AUDIO_CHANNEL_E)u32Channel, &u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}
//set speaker direct pass,  频率直通，不做频率抑制
static XBH_S32 setSpeakerDp(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Channel = request.readInt32();
    XBH_S32 u32Value = request.readInt32();
    s32Ret = XbhAudioAmplifierManager::getInstance()->setDirectPass((XBH_AUDIO_CHANNEL_E)u32Channel, (XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getSpeakerDp(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Channel = request.readInt32();
    XBH_S32 u32Value = 0;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getDirectPass((XBH_AUDIO_CHANNEL_E)u32Channel, (XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

//------------------------------------------------------------------------------ AUDIO end ---------------------------------------------------------

//------------------------------------------------------------------------------ PCSETTING start ---------------------------------------------------
static XBH_S32 autoAdc(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = pXbhPlatformApiImpl->autoAdc();
    return s32Ret;
}

static XBH_S32 autoAdjust(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = pXbhPlatformApiImpl->autoAdjust();
    return s32Ret;
}

static XBH_S32 setHPosition(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHPosition(u32Value);
    return s32Ret;
}

static XBH_S32 getHPosition(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getHPosition(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setVPosition(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setVPosition(u32Value);
    return s32Ret;
}

static XBH_S32 getVPosition(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getVPosition(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setClock(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setClock(u32Value);
    return s32Ret;
}

static XBH_S32 getClock(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getClock(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setPhase(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPhase(u32Value);
    return s32Ret;
}

static XBH_S32 getPhase(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 50;
    s32Ret = pXbhPlatformApiImpl->getPhase(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

//-------------------------------------------------------------------------------- PCSETTING end ------------------------------------------------------

//--------------------------------------------------------------------------------- CEC start ---------------------------------------------------------
static XBH_S32 setCecEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setCecEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getCecEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getCecEnable((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setCecAutoPowerOffEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setCecAutoPowerOffEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getCecAutoPowerOffEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getCecAutoPowerOffEnable((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setCecAutoWakeUpEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setCecAutoWakeUpEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getCecAutoWakeUpEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getCecAutoWakeUpEnable((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}
//---------------------------------------------------------------------------------- CEC end -------------------------------------------------------

//-------------------------------------------------------------------------------- SOURCE start ----------------------------------------------------
static XBH_S32 selectSource(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Win = request.readInt32();
    s32Ret = XbhSourceManager::getInstance()->selectSource((XBH_SOURCE_E)u32Value, (XBH_WINDOW_NUM_E)u32Win);
    return s32Ret;
}

static XBH_S32 setSourceSwitchPort(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = XbhSourceManager::getInstance()->setSourceSwitchPort((XBH_SOURCE_E)u32Value);
    return s32Ret;
}

static XBH_S32 setSourceUsbPort(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = XbhSourceManager::getInstance()->setSourceUsbPort((XBH_SOURCE_E)u32Value);
    return s32Ret;
}

static XBH_S32 setWindowRect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RECT_S stRect;
    stRect.u32X = request.readInt32();
    stRect.u32Y = request.readInt32();
    stRect.u32Width = request.readInt32();
    stRect.u32Height = request.readInt32();
    XBH_U32 u32Win = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setWindowRect(stRect, (XBH_WINDOW_NUM_E)u32Win);
    return s32Ret;
}

static XBH_S32 getWindowRect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RECT_S stRect;
    XBH_U32 u32Win = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getWindowRect(&stRect, (XBH_WINDOW_NUM_E)u32Win);
    reply->writeInt32(stRect.u32X);
    reply->writeInt32(stRect.u32Y);
    reply->writeInt32(stRect.u32Width);
    reply->writeInt32(stRect.u32Height);
    return s32Ret;
}

static XBH_S32 getTimingInfo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_TIMING_INFO_S timing;
    XBH_U32 u32Win = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getTimingInfo(&timing, (XBH_WINDOW_NUM_E)u32Win);
    reply->writeInt32(timing.u32Width);
    reply->writeInt32(timing.u32Height);
    reply->writeInt32(timing.u32FrameRate);
    reply->writeInt32(timing.bInterlace);
    reply->writeInt32(timing.enFormatHDMI);
    return s32Ret;
}

static XBH_S32 setHdmirxEdidType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdmirxEdidType((XBH_HDMIRX_EDID_TYPE_E)u32Value, u32Src);
    return s32Ret;
}

static XBH_S32 getHdmirxEdidType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getHdmirxEdidType(u32Src, (XBH_HDMIRX_EDID_TYPE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getSourceDet(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32SourceId = request.readInt32();
    XBH_U32 u32Value = 0;
    s32Ret = XbhSourceManager::getInstance()->getSourceDet((XBH_SOURCE_E)u32SourceId, (XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getOpsSystemType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getOpsSystemType((XBH_BOOL *)&u32Value, (XBH_SOURCE_E)u32Src);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setOpsPowerCtrl(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setOpsPowerCtrl((XBH_SOURCE_E)u32Src);
    return s32Ret;
}

static XBH_S32 getOpsPowerStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getOpsPowerStatus((XBH_BOOL *)&u32Value, (XBH_SOURCE_E)u32Src);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getOpsDetStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getOpsDetStatus((XBH_BOOL *)&u32Value, (XBH_SOURCE_E)u32Src);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setOpsPowerEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32State = request.readInt32();
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setOpsPowerEnable((XBH_BOOL)u32State, (XBH_SOURCE_E)u32Src);
    reply->writeInt32(u32State);
    return s32Ret;
}

static XBH_S32 getOpsPowerEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Data = 0;
    XBH_U32 u32Src = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getOpsPowerEnable((XBH_BOOL *)&u32Data, (XBH_SOURCE_E)u32Src);
    reply->writeInt32(u32Data);
    return s32Ret;
}

static XBH_S32 setHdmiTxInfo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32InfoID = request.readInt32();
    XBH_U32 u32Data = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdmiTxInfo((XBH_HDMI_TX_INFO_E)u32InfoID, u32Data);
    return s32Ret;
}

static XBH_S32 getHdmiTxInfo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32InfoID = request.readInt32();
    XBH_CHAR info[32] = {0};
    s32Ret = pXbhPlatformApiImpl->getHdmiTxInfo((XBH_HDMI_TX_INFO_E)u32InfoID, info);
    reply->writeString16(String16(info));
    return s32Ret;
}

static XBH_S32 setFlashEepData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 value8(request.readString16());
    std::string pBuff(value8.c_str());
    s32Ret = pXbhPlatformApiImpl->setFlashEepData(pBuff);
    return s32Ret;
}

static XBH_S32 getFlashEepData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string pBuff("");
    s32Ret = pXbhPlatformApiImpl->getFlashEepData(pBuff);
    reply->writeString16(String16(pBuff.c_str()));
    return s32Ret;
}

static XBH_S32 getInputId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

static XBH_S32 setOpsResetEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32State = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setOpsResetEnable((XBH_BOOL)u32State);
    reply->writeInt32(u32State);
    return s32Ret;
}


static XBH_S32 setBacklightData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32State = request.readInt32();
    XLOGD("setBacklightData service\n ");
    s32Ret = pXbhPlatformApiImpl->setBacklightData(u32State);
    reply->writeInt32(u32State);
    return s32Ret;
}

/*设置单独每一路信源的edid type*/
static XBH_S32 setSourceEdidType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Src = request.readInt32();
    //XLOGD("edid u32Value = %d u32Src = %d ",u32Value,u32Src);
    s32Ret = pXbhPlatformApiImpl->setSourceEdidType((XBH_HDMIRX_EDID_TYPE_E)u32Value, u32Src);
    return s32Ret;
}
/*获取单独每一路信源的edid type*/
static XBH_S32 getSourceEdidType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Src = request.readInt32();
    //XLOGD("edid u32Src = %d",u32Src);
    s32Ret = pXbhPlatformApiImpl->getSourceEdidType(u32Src, (XBH_HDMIRX_EDID_TYPE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

/**
* 设置HDMI TX/TYPEC TX 模式
* param[in] 按照HDMI TX还是TYPEC TX输出。0：hdmi tx， 1： typec tx
*/
XBH_S32 setVideoTxMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    //XLOGD("edid u32Value = %d u32Src = %d ",u32Value,u32Src);
    s32Ret = pXbhPlatformApiImpl->setVideoTxMode(u32Value);
    return s32Ret;
}

/**
* 获取HDMI TX/TYPEC TX 模式
* param[out] 按照HDMI TX还是TYPEC TX输出。0：hdmi tx， 1： typec tx
*/
XBH_S32 getVideoTxMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getVideoTxMode(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}
static XBH_S32 getHdmiRxAudioSampleFreq(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32Data = 0;
    s32Ret = pXbhPlatformApiImpl->getHdmiRxAudioSampleFreq(&u32Data);
    reply->writeInt32(u32Data);
    return s32Ret;
}
//------------------------------------------------------------------------------- SOURCE end ------------------------------------------------------

//------------------------------------------------------------------------------ SYSTEM start -----------------------------------------------------
static XBH_S32 getWakeUpReason(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_WAKEUP_S stWake;
    s32Ret = pXbhPlatformApiImpl->getWakeUpReason(&stWake);
    reply->writeInt32(stWake.enWakeup);
    reply->writeInt32(stWake.enSrc);
    reply->writeInt32(stWake.u32Value);
    return s32Ret;
}

static XBH_S32 setVendorProperties(const Parcel &request, Parcel *reply) {
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string key(String8(request.readString16()).c_str());
    std::string value(String8(request.readString16()).c_str());
    s32Ret = property_set(key.c_str(), value.c_str());
    //XLOGD("setVendorProperties %s: %s \r\n", key.c_str(), value.c_str());
    return s32Ret;
}

static XBH_S32 setPoweronMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPoweronMode((XBH_POWERMODE_E)u32Value);
    return s32Ret;
}

static XBH_S32 getPoweronMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPoweronMode((XBH_POWERMODE_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setWoTStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32WakeUpType = request.readInt32();
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setWoTStatus((XBH_WAKEUP_E)u32WakeUpType, (XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getWoTStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32WakeUpType = request.readInt32();
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getWoTStatus((XBH_WAKEUP_E)u32WakeUpType, (XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setSourceTouchState(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = XbhTouchManager::getInstance()->setSourceTouchState((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getSourceTouchState(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = XbhTouchManager::getInstance()->getSourceTouchState((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setNonThroughTouchRegion(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 id = request.readInt32();
    XBH_S32 resolution = request.readInt32();
    XBH_S32 x = request.readInt32();
    XBH_S32 y = request.readInt32();
    XBH_S32 w = request.readInt32();
    XBH_S32 h = request.readInt32();
    s32Ret = XbhTouchManager::getInstance()->setNonThroughTouchRegion(id, resolution, x, y, w, h);
    return s32Ret;
}

static XBH_S32 deleteNonThroughTouchRegion(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 id = request.readInt32();
    XBH_S32 resolution = request.readInt32();
    XBH_S32 x = request.readInt32();
    XBH_S32 y = request.readInt32();
    XBH_S32 w = request.readInt32();
    XBH_S32 h = request.readInt32();
    s32Ret = XbhTouchManager::getInstance()->deleteNonThroughTouchRegion(id, resolution, x, y, w, h);
    return s32Ret;
}

static XBH_S32 setTouchScaleRect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 resolution = request.readInt32();
    XBH_S32 x = request.readInt32();
    XBH_S32 y = request.readInt32();
    XBH_S32 w = request.readInt32();
    XBH_S32 h = request.readInt32();
    s32Ret = XbhTouchManager::getInstance()->setTouchScaleRect(resolution, x, y, w, h);
    return s32Ret;
}

static XBH_S32 sendKeyToTp(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 keycode = request.readInt32();
    s32Ret = XbhTouchManager::getInstance()->sendKeyToTp(keycode);
    return s32Ret;
}

static XBH_S32 sendDataToTp(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 buff[64] = {0};
    XBH_S32 len = request.readInt32();
    for (XBH_U32 i = 0; i < len; i++)
    {
        buff[i] = (XBH_U32)request.readInt32();
    }
    s32Ret = XbhTouchManager::getInstance()->sendDataToTp(len, buff);
    return s32Ret;
}

static XBH_S32 setChipRunTime(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 time = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setChipRunTime(time);
    return s32Ret;
}

static XBH_S32 getChipRunTime(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 time;
    s32Ret = pXbhPlatformApiImpl->getChipRunTime(&time);
    reply->writeInt32(time);
    return s32Ret;
}

static XBH_S32 setEywaLogLevel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 loglevel = request.readInt32();
    s32Ret = XbhLog::setPrintLevel((XBH_LOG_PRIO_E)loglevel);
    return s32Ret;
}

static XBH_S32 getEywaLogLevel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 loglevel;
    s32Ret = XbhLog::getPrintLevel((XBH_LOG_PRIO_E&)loglevel);
    reply->writeInt32(loglevel);
    return s32Ret;
}

static XBH_S32 setBootLogo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_CHAR *file = String8(request.readString16()).c_str();
    s32Ret = pXbhPlatformApiImpl->setBootLogo(file);
    return s32Ret;
}

static XBH_S32 setBootVideo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_CHAR *file = String8(request.readString16()).c_str();
    s32Ret = pXbhPlatformApiImpl->setBootVideo(file);
    return s32Ret;
}

static XBH_S32 setBootAnimation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_CHAR *file = String8(request.readString16()).c_str();
    s32Ret = pXbhPlatformApiImpl->setBootAnimation(file);
    return s32Ret;
}

static XBH_S32 setPalmRejection(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = XbhTouchManager::getInstance()->setPalmRejection((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getPalmRejection(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = XbhTouchManager::getInstance()->getPalmRejection((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 loadEdlaCust(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->loadEdlaCust((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getProjectIdVersion(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR version[32] = {0};
    s32Ret = pXbhPlatformApiImpl->getProjectIdVersion(version);
    reply->writeString16(String16(version));
    return s32Ret;
}

static XBH_S32 setStandbyMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Mode = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setStandbyMode(u32Mode);
    return s32Ret;
}

static XBH_S32 getStandbyMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getStandbyMode(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getTpConnect(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = XbhTouchManager::getInstance()->getTouchPanelConnect((XBH_BOOL*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getTpName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR name[32] = {0};
    s32Ret = XbhTouchManager::getInstance()->getTouchPanelName(name);
    reply->writeString16(String16(name));
    return s32Ret;
}

static XBH_S32 setOnStop(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = pXbhPlatformApiImpl->setOnStop();
    return s32Ret;
}
//--------------------------------------------------------------------------- SYSTEM end ----------------------------------------------------------

//-------------------------------------------------------------------------- FACTORY start --------------------------------------------------------
static XBH_S32 setOverscan(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RECT_S stRect;
    stRect.u32X = request.readInt32();
    stRect.u32Y = request.readInt32();
    stRect.u32Width = request.readInt32();
    stRect.u32Height = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setOverscan(&stRect);
    return s32Ret;
}

static XBH_S32 getOverscan(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RECT_S stRect;
    s32Ret = pXbhPlatformApiImpl->getOverscan(&stRect);
    reply->writeInt32(stRect.u32X);
    reply->writeInt32(stRect.u32Y);
    reply->writeInt32(stRect.u32Width);
    reply->writeInt32(stRect.u32Height);
    return s32Ret;
}

static XBH_S32 setTestPattern(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Pattrern = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setTestPattern((XBH_PATTERN_E)u32Pattrern, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getTestPattern(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getTestPattern((XBH_PATTERN_E *)&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setPanelIndex(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPanelIndex(u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getPanelIndex(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getPanelIndex(&s32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 setPwmFrequency(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPwmFrequency(u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getPwmFrequency(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getPwmFrequency(&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setPwmPolarity(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPwmPolarity((XBH_SOURCE_E)u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getPwmPolarity(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getPwmPolarity((XBH_BOOL *)&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setDebugEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setDebugEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getDebugEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getDebugEnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMacAddress(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_S32 macType = request.readInt32();
    String8 macString(request.readString16());
    const XBH_CHAR *macAddress = macString.c_str();
    s32Ret = pXbhPlatformApiImpl->setMacAddress(macType, macAddress);
    return s32Ret;
}

static XBH_S32 getMacAddress(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR macaddress[32] = {0};
    const XBH_S32 macType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getMacAddress(macType, macaddress);
    reply->writeString16(String16(macaddress));
    return s32Ret;
}

static XBH_S32 setSn(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *sn = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSn(sn);
    return s32Ret;
}

static XBH_S32 getSn(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //Align with XBH_CUSDATA_BARCODE_LEN
    XBH_CHAR sn[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSn(sn);
    reply->writeString16(String16(sn));
    return s32Ret;
}

static XBH_S32 setHdcp(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 hdcpString(request.readString16());
    const XBH_CHAR *path = hdcpString.c_str();
    int type = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdcpKey(path, XBH_HDCP_TYPE_E(type));
    return s32Ret;
}
static XBH_S32 getHdcpKeydata(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR pBuff[64] = {0};
    XBH_U32 source = request.readInt32();
    int type = request.readInt32();
    //XLOGD("devType=%d type=%d",devType,type);
    s32Ret = pXbhPlatformApiImpl->getHdcpKeydata(XBH_SOURCE_E(source),XBH_HDCP_TYPE_E(type),pBuff);
    reply->writeString16(String16(pBuff));
    return s32Ret;
}
static XBH_S32 getHdcpStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int type = request.readInt32();
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHdcpStatus(XBH_HDCP_TYPE_E(type), (XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setHdcpKeyName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 hdcpString(request.readString16());
    const XBH_CHAR *path = hdcpString.c_str();
    int type = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdcpKeyName(path, XBH_HDCP_TYPE_E(type));
    return s32Ret;
}

static XBH_S32 getHdcpKeyName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR hdcpKeyName[64] = {0};
    const XBH_S32 hdcpType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getHdcpKeyName(XBH_HDCP_TYPE_E(hdcpType), hdcpKeyName);
    reply->writeString16(String16(hdcpKeyName));
    return s32Ret;
}

static XBH_S32 setHdmiEdid(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 edidString(request.readString16());
    const XBH_CHAR *path = edidString.c_str();
    int idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdmiEdid(path, XBH_SOURCE_E(idx));
    return s32Ret;
}

static XBH_S32 getHdmiEdidStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int idx = request.readInt32();
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHdmiEdidStatus((XBH_SOURCE_E)idx, (XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setHdcpExtendIc(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 hdcpString(request.readString16());
    const XBH_CHAR *path = hdcpString.c_str();
    XBH_U32 type = request.readInt32();
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdcpExtendIc(path, XBH_HDCP_TYPE_E(type), devType);
    return s32Ret;
}

static XBH_S32 getHdcpExtIcStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 type = request.readInt32();
    XBH_U32 u32Value = 0;
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getHdcpExtIcStatus(XBH_HDCP_TYPE_E(type), (XBH_BOOL *)&u32Value, devType);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setHdcpExtIcKeyName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 hdcpString(request.readString16());
    const XBH_CHAR *hdcpKeyName = hdcpString.c_str();
    XBH_U32 type = request.readInt32();
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdcpExtIcKeyName(hdcpKeyName, XBH_HDCP_TYPE_E(type), devType);
    return s32Ret;
}

static XBH_S32 getHdcpExtIcKeyName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR hdcpKeyName[64] = {0};
    XBH_S32 type = request.readInt32();
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getHdcpExtIcKeyName(hdcpKeyName, XBH_HDCP_TYPE_E(type), devType);
    reply->writeString16(String16(hdcpKeyName));
    return s32Ret;
}

static XBH_S32 setVgaEdid(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 edidString(request.readString16());
    const XBH_CHAR *path = edidString.c_str();
    int idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setVgaEdid(path, XBH_SOURCE_E(idx));
    return s32Ret;
}

static XBH_S32 getVgaEdidStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int idx = request.readInt32();
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getVgaEdidStatus((XBH_SOURCE_E)idx, (XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getVgaEdidCheckSum(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int idx = request.readInt32();
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getVgaEdidCheckSum((XBH_SOURCE_E)idx, &u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 dumpEdid(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->dumpEdid((XBH_SOURCE_E)idx);
    return s32Ret;
}

static XBH_S32 setGoogleKey(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 keyString(request.readString16());
    const XBH_CHAR *path = keyString.c_str();
    s32Ret = pXbhPlatformApiImpl->setGoogleKey(path);
    return s32Ret;
}

static XBH_S32 getGoogleKeyStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getGoogleKeyStatus((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setGoogleKeyName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 googleString(request.readString16());
    const XBH_CHAR *path = googleString.c_str();
    s32Ret = pXbhPlatformApiImpl->setGoogleKeyName(path);
    return s32Ret;
}

static XBH_S32 getGoogleKeyName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR googleKeyName[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getGoogleKeyName(googleKeyName);
    reply->writeString16(String16(googleKeyName));
    return s32Ret;
}

static XBH_S32 setAttentionKey(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 keyString(request.readString16());
    const XBH_CHAR *path = keyString.c_str();
    s32Ret = pXbhPlatformApiImpl->setAttentionKey(path);
    return s32Ret;
}

static XBH_S32 getAttentionKeyStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAttentionKeyStatus((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setWifiChanelPlan(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int wifiType = request.readInt32();
    String8 gpioString(request.readString16());
    const XBH_CHAR *channel = gpioString.c_str();
    s32Ret = pXbhPlatformApiImpl->setWifiChanelPlan((XBH_WIFI_TYPE)wifiType, channel);
    return s32Ret;
}

static XBH_S32 getRkpCsrInfo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR buffer[4096] = {0};
    s32Ret = pXbhPlatformApiImpl->getRkpCsrInfo(buffer, sizeof(buffer));
    reply->writeString16(String16(buffer));
    return s32Ret;
}

static XBH_S32 setAgingModeState(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAgingModeState(u32Value);
    return s32Ret;
}

static XBH_S32 getAgingModeState(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAgingModeState(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMokaPModeEnableToCusdata(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMokaPModeEnableToCusdata(u32Value);
    return s32Ret;
}

static XBH_S32 getMokaPModeEnableFromCusdata(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getMokaPModeEnableFromCusdata(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 upgradeSystemFirmware(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->upgradeSystemFirmware((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

#if 0
static XBH_S32 setNLAPoint(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_NLA_E enNla = (XBH_NLA_E)request.readInt32();
    XBH_NLA_INFO_S stInfo;
    stInfo.u32Point0 = request.readInt32();
    stInfo.u32Point25 = request.readInt32();
    stInfo.u32Point50 = request.readInt32();
    stInfo.u32Point75 = request.readInt32();
    stInfo.u32Point100 = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setNLAPoint(enNla, &stInfo);
    return s32Ret;
}

static XBH_S32 getNLAPoint(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_NLA_E enNla = (XBH_NLA_E)request.readInt32();
    XBH_NLA_INFO_S stInfo;
    s32Ret = pXbhPlatformApiImpl->getNLAPoint(enNla, &stInfo);
    reply->writeInt32(stInfo.u32Point0);
    reply->writeInt32(stInfo.u32Point25);
    reply->writeInt32(stInfo.u32Point50);
    reply->writeInt32(stInfo.u32Point75);
    reply->writeInt32(stInfo.u32Point100);
    return s32Ret;
}

static XBH_S32 setNRThreshold(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //TODO TBD
    return s32Ret;
}

static XBH_S32 getNRThreshold(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //TODO TBD
    return s32Ret;
}

static XBH_S32 setAVCThreshold(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //TODO TBD
    return s32Ret;
}

static XBH_S32 getAVCThreshold(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //TODO TBD
    return s32Ret;
}

static XBH_S32 setSpreadEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSpreadEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getSpreadEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getSpreadEnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getLvdsMap(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getLvdsMap(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 setLvdsMap(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setLvdsMap(s32Value);
    return s32Ret;
}

static XBH_S32 setBoardType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 boardType(request.readString16());
    const XBH_CHAR *str = boardType.c_str();
    s32Ret = pXbhPlatformApiImpl->setBoardType(str);
    return s32Ret;
}

static XBH_S32 getBoardType(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR str[32] = {0};
    s32Ret = pXbhPlatformApiImpl->getBoardType(str);
    reply->writeString16(String16(str));
    return s32Ret;
}

static XBH_S32 setActivationCode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 code(request.readString16());
    const XBH_CHAR *str = code.c_str();
    s32Ret = pXbhPlatformApiImpl->setActivationCode(str);
    return s32Ret;
}

static XBH_S32 getActivationCode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR str[32] = {0};
    s32Ret = pXbhPlatformApiImpl->getActivationCode(str);
    reply->writeString16(String16(str));
    return s32Ret;
}
#endif
static XBH_S32 setUsbcConfig(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setUsbcConfig(type);
    return s32Ret;
}

static XBH_S32 getUsbcConfig(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = 0;
    s32Ret = pXbhPlatformApiImpl->getUsbcConfig(&type);
    reply->writeInt32(type);
    return s32Ret;
}

//--------------------------------------------------------------------------- FACTORY end -------------------------------------------------------

//--------------------------------------------------------------------------- DEVICE start ------------------------------------------------------
static XBH_S32 setGpioOutputValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioNumber = request.readInt32();
    XBH_U32 u32GpioValue = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGpioOutputValue(u32GpioNumber, u32GpioValue);
    return s32Ret;
}

static XBH_S32 getGpioOutputValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioNumber = request.readInt32();
    XBH_U32 u32GpioValue = 0;
    s32Ret = pXbhPlatformApiImpl->getGpioOutputValue(u32GpioNumber, &u32GpioValue);
    reply->writeInt32(u32GpioValue);
    return s32Ret;
}

static XBH_S32 getGpioInputValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioNumber = request.readInt32();
    XBH_U32 u32GpioValue = 0;
    s32Ret = pXbhPlatformApiImpl->getGpioInputValue(u32GpioNumber, &u32GpioValue);
    reply->writeInt32(u32GpioValue);
    return s32Ret;
}

static XBH_S32 setGpioCommonCtrl(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 gpioString(request.readString16());
    const XBH_CHAR *gpioName = gpioString.c_str();
    XBH_U32 gpioDirection = request.readInt32();
    XBH_U32 u32GpioValue = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGpioCommonCtrl(gpioName, gpioDirection, u32GpioValue);
    return s32Ret;
}

static XBH_S32 getGpioCommonCtrl(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 gpioString(request.readString16());
    const XBH_CHAR *gpioName = gpioString.c_str();
    XBH_U32 u32GpioValue = 0;
    s32Ret = pXbhPlatformApiImpl->getGpioCommonCtrl(gpioName, &u32GpioValue);
    reply->writeInt32(u32GpioValue);
    return s32Ret;
}

static XBH_S32 setRtcTime(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RTC_INFO_S stInfo;
    stInfo.u32RtcYear = request.readInt32();
    stInfo.u32RtcMonth = request.readInt32();
    stInfo.u32RtcDate = request.readInt32();
    stInfo.u32RtcWeek = request.readInt32();
    stInfo.u32RtcHour = request.readInt32();
    stInfo.u32RtcMinute = request.readInt32();
    stInfo.u32RtcSec = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setRtcTime(&stInfo);
    return s32Ret;
}

static XBH_S32 getRtcTime(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RTC_INFO_S stInfo;
    s32Ret = pXbhPlatformApiImpl->getRtcTime(&stInfo);
    reply->writeInt32(stInfo.u32RtcYear);
    reply->writeInt32(stInfo.u32RtcMonth);
    reply->writeInt32(stInfo.u32RtcDate);
    reply->writeInt32(stInfo.u32RtcWeek);
    reply->writeInt32(stInfo.u32RtcHour);
    reply->writeInt32(stInfo.u32RtcMinute);
    reply->writeInt32(stInfo.u32RtcSec);
    return s32Ret;
}

static XBH_S32 setRtcAlarmTime(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_RTC_ALARM_INFO_S stInfo;
    stInfo.u32RtcDate = request.readInt32();
    stInfo.u32RtcWeek = request.readInt32();
    stInfo.u32RtcHour = request.readInt32();
    stInfo.u32RtcMinute = request.readInt32();
    stInfo.bEnable = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setRtcAlarmTime(&stInfo);
    return s32Ret;
}

static XBH_S32 setIICData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 i = 0;
    XBH_U32 u32I2cNum = request.readInt32();
    XBH_U32 u8DevAddress = (XBH_U8)request.readInt32();
    XBH_U32 u32RegAddr = request.readInt32();
    XBH_U32 u32RegAddrCount = request.readInt32();
    XBH_U32 u32Length = request.readInt32();
    XBH_U8 u8Data[1024] = {0};
    for (i = 0; i < u32Length; i++)
    {
        u8Data[i] = (XBH_U8)request.readInt32();
    }
    s32Ret = pXbhPlatformApiImpl->setIICData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    return s32Ret;
}

static XBH_S32 getIICData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 i = 0;
    XBH_U32 u32I2cNum = request.readInt32();
    XBH_U32 u8DevAddress = (XBH_U8)request.readInt32();
    XBH_U32 u32RegAddr = request.readInt32();
    XBH_U32 u32RegAddrCount = request.readInt32();
    XBH_U32 u32Length = request.readInt32();
    XBH_U8 u8Data[1024] = {0};
    s32Ret = pXbhPlatformApiImpl->getIICData(u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length, u8Data);
    for (i = 0; i < u32Length; i++)
    {
        reply->writeInt32(u8Data[i]);
    }
    return s32Ret;
}

static XBH_S32 setBacklightEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBacklightEnable((XBH_BOOL)u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getBacklightEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getBacklightEnable((XBH_BOOL *)&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setPanelPower(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPanelPower((XBH_BOOL)u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    return s32Ret;
}

static XBH_S32 getPanelPower(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 u32Pnl = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getPanelPower((XBH_BOOL *)&u32Value, (XBH_PANEL_NUM_E)u32Pnl);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setLedPwrStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32State = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setLedPwrStatus((XBH_LED_LIGHT_E)u32State);
    return s32Ret;
}

static XBH_S32 getLedPwrStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = XBH_LED_LIGHT_OFF;
    s32Ret = pXbhPlatformApiImpl->getLedPwrStatus((XBH_LED_LIGHT_E *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}


static XBH_S32 setBrightMax(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBrightMax(u32Value);
    return s32Ret;
}

static XBH_S32 getBrightMax(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getBrightMax(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setBrightMin(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setBrightMin(u32Value);
    return s32Ret;
}

static XBH_S32 getBrightMin(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getBrightMin(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setMcuFattMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 mode = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMcuFattMode(mode);
    return s32Ret;
}

static XBH_S32 getMcuFattMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 mode = 0;
    s32Ret = pXbhPlatformApiImpl->getMcuFattMode(&mode);
    reply->writeInt32((XBH_U32)mode);
    return s32Ret;
}

static XBH_S32 setMcuIIcBypass(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8IIcNum = (XBH_U8)request.readInt32();
    XBH_U8 u8DeviceAddr = (XBH_U8)request.readInt32();
    XBH_U8 u8RegAddr= (XBH_U8)request.readInt32();
    XBH_U8 u8Len = (XBH_U8)request.readInt32();
    XBH_U8 data[255]= {0};
    for(int i=0; i<u8Len; i++) {
        data[i] = (XBH_U8)request.readInt32();
    }
    s32Ret = pXbhPlatformApiImpl->setMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, data);
    return s32Ret;
}

static XBH_S32 getMcuIIcBypass(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8IIcNum = (XBH_U8)request.readInt32();
    XBH_U8 u8DeviceAddr = (XBH_U8)request.readInt32();
    XBH_U8 u8RegAddr= (XBH_U8)request.readInt32();
    XBH_U8 u8Len = (XBH_U8)request.readInt32();
    XBH_U8 data[255] = {0};
    s32Ret = pXbhPlatformApiImpl->getMcuIIcBypass(u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, data);
    for(int i=0; i<u8Len; i++) {
        reply->writeInt32(data[i]);
    }
    return s32Ret;
}

static XBH_S32 getFattKeypadInfo(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 keypad_enable = 0;
    XBH_S32 key_count = 0;
    XBH_S32 key_index = 0;
    s32Ret = pXbhPlatformApiImpl->getFattKeypadInfo(&keypad_enable, &key_count, &key_index);
    reply->writeInt32(keypad_enable);
    reply->writeInt32(key_count);
    reply->writeInt32(key_index);
    return s32Ret;
}

static XBH_S32 setFattKeypadenbale(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 enable = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setFattKeypadenbale(enable);
    return s32Ret;
}

static XBH_S32 setMcuUartBypassPara(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 u8UartNum = (XBH_U8)request.readInt32();
    XBH_U8 u8Baud = (XBH_U8)request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMcuUartBypassPara(u8UartNum, u8Baud);
    return s32Ret;
}

static XBH_S32 setMcuUartBypass(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[255] = {0};
    XBH_U8 u8Len = (XBH_U8)request.readInt32();
    for(int i=0; i<u8Len; i++) {
        data[i] = (XBH_U8)request.readInt32();
    }
    s32Ret = pXbhPlatformApiImpl->setMcuUartBypass(u8Len, data);
    return s32Ret;
}

static XBH_S32 getMcuUartBypass(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[255] = {0};
    XBH_U8 u8Len = (XBH_U8)request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getMcuUartBypass(u8Len, data);
    for(int i=0; i<u8Len; i++) {
        reply->writeInt32(data[i]);
    }
    return s32Ret;
}

static XBH_S32 getEthPlugStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getEthPlugStatus((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getADCChannelValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Channel = request.readInt32();
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getADCChannelValue((XBH_ADC_CHANNEL_NUM_E)u32Channel, &u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getLightSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getLightSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getPirSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPirSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getHallSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHallSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getAirSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getAirSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getTemperatureSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getTemperatureSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getGSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getGSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getPowerSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_FLOAT s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPowerSensorValue(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getPowerSensorExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPowerSensorExist((XBH_BOOL *)&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getLightSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getLightSensorValue(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getPirSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPirSensorValue(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getHallSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHallSensorValue(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getHallSensorPen(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHallSensorPen(&s32Value);
    reply->writeInt32(s32Value);
    return s32Ret;
}

static XBH_S32 getAirSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_FLOAT fHumidity = 0;
    XBH_FLOAT fTemperature = 0;
    XBH_FLOAT fTvoc = 0;
    XBH_FLOAT fPm2p5 = 0;
    XBH_FLOAT fPm1p0 = 0;
    XBH_FLOAT fPm10 = 0;
    XBH_FLOAT fCo2 = 0;
    s32Ret = pXbhPlatformApiImpl->getAirSensorValue(&fHumidity, &fTemperature, &fTvoc, &fPm2p5, &fPm1p0, &fPm10, &fCo2);
    reply->writeFloat(fHumidity);
    reply->writeFloat(fTemperature);
    reply->writeFloat(fTvoc);
    reply->writeFloat(fPm2p5);
    reply->writeFloat(fPm1p0);
    reply->writeFloat(fPm10);
    reply->writeFloat(fCo2);
    return s32Ret;
}

static XBH_S32 getNTCValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    XBH_FLOAT fValue = 0;
    s32Ret = pXbhPlatformApiImpl->getNTCValue(u32Value, &fValue);
    reply->writeFloat(fValue);
    return s32Ret;
}

static XBH_S32 getTemperatureSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_FLOAT fValue = 0;
    s32Ret = pXbhPlatformApiImpl->getTemperatureSensorValue(&fValue);
    reply->writeFloat(fValue);
    return s32Ret;
}

static XBH_S32 getGSensorValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_FLOAT fXValue = 0;
    XBH_FLOAT fYValue = 0;
    XBH_FLOAT fZValue = 0;
    s32Ret = pXbhPlatformApiImpl->getGSensorValue(&fXValue, &fYValue, &fZValue);
    reply->writeFloat(fXValue);
    reply->writeFloat(fYValue);
    reply->writeFloat(fZValue);
    return s32Ret;
}

static XBH_S32 upgradeExtendIc(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 pathString(request.readString16());
    const XBH_CHAR *fileName = pathString.c_str();
    XBH_U32 force = request.readInt32();
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->upgradeExtendIc(fileName, (XBH_BOOL)force, devType);
    return s32Ret;
}

static XBH_S32 getUpgradeExtIcState(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getUpgradeExtIcState(devType, (XBH_S32*)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setHdcpTxEncryptionMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int hdcpTxEncryptionMode = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setHdcpTxEncryptionMode(XBH_HDCPTX_ENCRYPTION_MODE_E(hdcpTxEncryptionMode));
    return s32Ret;
}

static XBH_S32 getHdcpTxEncryptionMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_HDCPTX_ENCRYPTION_MODE_E hdcpTxEncryptionMode;
    s32Ret = pXbhPlatformApiImpl->getHdcpTxEncryptionMode(&hdcpTxEncryptionMode);
    reply->writeInt32(static_cast<XBH_S32>(hdcpTxEncryptionMode));
    return s32Ret;
}

static XBH_S32 getExtendIcVer(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR ver[32] = {0};
    XBH_U32 devType = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getExtendIcVer(devType, ver);
    reply->writeString16(String16(ver));
    return s32Ret;
}

static XBH_S32 setDdrSSC(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_SSC_INFO_S info;
    info.bEnable = request.readInt32();
    info.u32Freq = request.readInt32();
    info.u32Ratio = request.readInt32();
    XBH_S32 idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setDdrSSC(info, idx);
    return s32Ret;
}

static XBH_S32 getDdrSSC(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_SSC_INFO_S info;
    s32Ret = pXbhPlatformApiImpl->getDdrSSC(&info, idx);
    reply->writeInt32(info.bEnable ? 1 : 0);
    reply->writeInt32(info.u32Freq);
    reply->writeInt32(info.u32Ratio);
    return s32Ret;
}

static XBH_S32 setPanelSSC(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_SSC_INFO_S info;
    info.bEnable = request.readInt32();
    info.u32Freq = request.readInt32();
    info.u32Ratio = request.readInt32();
    XBH_S32 idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPanelSSC(info, (XBH_PANEL_NUM_E)idx);
    return s32Ret;
}

static XBH_S32 getPanelSSC(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_SSC_INFO_S info;
    s32Ret = pXbhPlatformApiImpl->getPanelSSC(&info, (XBH_PANEL_NUM_E)idx);
    reply->writeInt32(info.bEnable ? 1 : 0);
    reply->writeInt32(info.u32Freq);
    reply->writeInt32(info.u32Ratio);
    return s32Ret;
}

static XBH_S32 getPanelName(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_CHAR name[32] = {0};
    s32Ret = pXbhPlatformApiImpl->getPanelName(name, (XBH_PANEL_NUM_E)idx);
    reply->writeString16(String16(name));
    return s32Ret;
}

static XBH_S32 getPanelResolution(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_CHAR res[32] = {0};
    s32Ret = pXbhPlatformApiImpl->getPanelResolution(res, (XBH_PANEL_NUM_E)idx);
    reply->writeString16(String16(res));
    return s32Ret;
}

static XBH_S32 setPanelRotation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    XBH_S32 idx = request.readInt32();
    XLOGE("setPanelRotation value = ======");
    s32Ret = pXbhPlatformApiImpl->setPanelRotation(value, (XBH_PANEL_NUM_E)idx);
    return s32Ret;
}

static XBH_S32 getPanelRotation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_S32 value = 0;
    s32Ret = pXbhPlatformApiImpl->getPanelRotation(&value, (XBH_PANEL_NUM_E)idx);
    reply->writeInt32(value);
    return s32Ret;
}

static XBH_S32 setMultiPanel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = request.readInt32();
    XBH_S32 idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMultiPanel(type, (XBH_PANEL_NUM_E)idx);
    return s32Ret;
}

static XBH_S32 getMultiPanel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_S32 type = 0;
    s32Ret = pXbhPlatformApiImpl->getMultiPanel(&type, (XBH_PANEL_NUM_E)idx);
    reply->writeInt32(type);
    return s32Ret;
}

static XBH_S32 setPanelMapSwap(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPanelMapSwap(type);
    return s32Ret;
}

static XBH_S32 getPanelMapSwap(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type = 0;
    s32Ret = pXbhPlatformApiImpl->getPanelMapSwap(&type);
    reply->writeInt32(type);
    return s32Ret;
}

static XBH_S32 setPanelFlipMirror(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 enable = request.readInt32();
    XBH_S32 idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPanelFlipMirror((XBH_BOOL)enable, (XBH_PANEL_NUM_E)idx);
    return s32Ret;
}

static XBH_S32 getPanelFlipMirror(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_S32 enable = 0;
    s32Ret = pXbhPlatformApiImpl->getPanelFlipMirror((XBH_BOOL*)&enable, (XBH_PANEL_NUM_E)idx);
    reply->writeInt32(enable);
    return s32Ret;
}

static XBH_S32 getOpsLockStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 idx = request.readInt32();
    XBH_S32 enable = 0;
    s32Ret = pXbhPlatformApiImpl->getOpsLockStatus((XBH_BOOL*)&enable, (XBH_SOURCE_E)idx);
    reply->writeInt32(enable);
    return s32Ret;
}

static XBH_S32 setPixelShiftEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 enable = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPixelShiftEnable((XBH_BOOL)enable);
    return s32Ret;
}

static XBH_S32 getPixelShiftEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 enable = 0;
    s32Ret = pXbhPlatformApiImpl->getPixelShiftEnable((XBH_BOOL*)&enable);
    reply->writeInt32(enable);
    return s32Ret;
}

static XBH_S32 setGraphicBrightness(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGraphicBrightness(value);
    return s32Ret;
}

static XBH_S32 getGraphicBrightness(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 vlaue = 0;
    s32Ret = pXbhPlatformApiImpl->getGraphicBrightness((XBH_S32*)&vlaue);
    reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setGraphicContrast(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGraphicContrast(value);
    return s32Ret;
}

static XBH_S32 getGraphicContrast(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 vlaue = 0;
    s32Ret = pXbhPlatformApiImpl->getGraphicContrast((XBH_S32*)&vlaue);
    reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setGraphicSaturation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGraphicSaturation(value);
    return s32Ret;
}

static XBH_S32 getGraphicSaturation(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 vlaue = 0;
    s32Ret = pXbhPlatformApiImpl->getGraphicSaturation((XBH_S32*)&vlaue);
    reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setGraphicColorTempMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setGraphicColorTempMode((XBH_COLORTEMP_E)value);
    return s32Ret;
}

static XBH_S32 getGraphicColorTempMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_COLORTEMP_E vlaue;
    s32Ret = pXbhPlatformApiImpl->getGraphicColorTempMode(&vlaue);
    reply->writeInt32((XBH_S32)vlaue);
    return s32Ret;
}

static XBH_S32 setEnergyStarEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setEnergyStarEnable((XBH_BOOL)value);
    return s32Ret;
}

static XBH_S32 getEnergyStarEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 vlaue = 0;
    s32Ret = pXbhPlatformApiImpl->getEnergyStarEnable((XBH_BOOL*)&vlaue);
    reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setAdcGainOffset(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_GAIN_OFFSET_DATA_S stInfo;
    XBH_S32 value = request.readInt32();
    stInfo.u32RedGain = request.readInt32();
    stInfo.u32GreenGain = request.readInt32();
    stInfo.u32BlueGain = request.readInt32();
    stInfo.u32RedOffset = request.readInt32();
    stInfo.u32GreenOffset = request.readInt32();
    stInfo.u32BlueOffset = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setAdcGainOffset((XBH_SOURCE_E)value, stInfo);
    return s32Ret;
}

static XBH_S32 getAdcGainOffset(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 src = request.readInt32();
    XBH_GAIN_OFFSET_DATA_S stInfo;
    s32Ret = pXbhPlatformApiImpl->getAdcGainOffset((XBH_SOURCE_E)src, &stInfo);
    reply->writeInt32(stInfo.u32RedGain);
    reply->writeInt32(stInfo.u32GreenGain);
    reply->writeInt32(stInfo.u32BlueGain);
    reply->writeInt32(stInfo.u32RedOffset);
    reply->writeInt32(stInfo.u32GreenOffset);
    reply->writeInt32(stInfo.u32BlueOffset);
    return s32Ret;
}

static XBH_S32 getUsbLockMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getUsbLockMode(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setUsbLockMode(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Mode = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setUsbLockMode(u32Mode);
    return s32Ret;
}

static XBH_S32 getOpsOvercurrentStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 vlaue = 0;
    XBH_U32 idx = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->getOpsOvercurrentStatus(idx, (XBH_BOOL*)&vlaue);
    reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setOpsOvercurrentProtection(const Parcel &request, Parcel *reply) {
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 enable = 0;
    XBH_U32 idx = request.readInt32();
    XBH_U32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setOpsOvercurrentProtection(idx, value);
    return s32Ret;
}

static XBH_S32 isNfcExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->isNfcExist(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setNfcEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL u32Value = (XBH_BOOL)request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setNfcEnable(u32Value);
    return s32Ret;
}

static XBH_S32 getNfcEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getNfcEnable(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getNfcId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[16] = {0};
    XBH_U32 length = 0;
    s32Ret = XbhNfcManager::getInstance()->getNfcId(data, &length);
    reply->writeInt32(length);
    for(int i = 0; i<length ; i++) {
        reply->writeInt32(data[i]);
    }
    return s32Ret;
}

static XBH_S32 getWakeUpNfcId(const Parcel &request, Parcel *reply)
{
    //目前只有8195.U MCU适配此功能
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[16] = {0};
    XBH_U32 length = 0;
    s32Ret = XbhNfcManager::getInstance()->getWakeUpNfcId(data, &length);
    reply->writeInt32(length);
    for(int i = 0; i<length ; i++) {
        reply->writeInt32(data[i]);
    }
    return s32Ret;
}

static XBH_S32 addNfcId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 length = request.readInt32();
    XBH_U8 data[16] = { 0 };
    for(int i = 0 ; i < length; i++) {
        data[i] = (XBH_U8)request.readInt32();
    }
    s32Ret = XbhNfcManager::getInstance()->addNfcId(data,length);
    return s32Ret;
}

static XBH_S32 delNfcId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 length = request.readInt32();
    XBH_U8 data[16]= { 0 };
    for(int i = 0 ; i < length; i++) {
        data[i] = (XBH_U8)request.readInt32();
    }
    s32Ret = XbhNfcManager::getInstance()->delNfcId(data,length);
    return s32Ret;
}

static XBH_S32 delAllNfcId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhNfcManager::getInstance()->delAllNfcId();
    return s32Ret;
}

static XBH_S32 setSourceScreenOffset(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 x = request.readInt32();
    XBH_S32 y = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setSourceScreenOffset(x, y);
    return s32Ret;
}

static XBH_S32 getSourceScreenOffset(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 x = 0;
    XBH_S32 y = 0;
    s32Ret = pXbhPlatformApiImpl->getSourceScreenOffset(&x, &y);
    reply->writeInt32(x);
    reply->writeInt32(y);
    return s32Ret;
}

static XBH_S32 setOnResume(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 state = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setOnResume(state);
    return s32Ret;
}

static XBH_S32 setWatchDogManualModeEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setWatchDogManualModeEnable((XBH_BOOL)value);
    return s32Ret;
}

static XBH_S32 getWatchDogManualModeEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = 0;
    s32Ret = pXbhPlatformApiImpl->getWatchDogManualModeEnable((XBH_BOOL *)&value);
    reply->writeInt32(value);

    return s32Ret;
}

static XBH_S32 setWatchDogEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setWatchDogEnable((XBH_BOOL)value);
    return s32Ret;
}

static XBH_S32 setWatchDogTimeOut(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setWatchDogTimeOut(value);
    return s32Ret;
}

static XBH_S32 getWatchDogTimeOut(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 value = 0;
    s32Ret = pXbhPlatformApiImpl->getWatchDogTimeOut(&value);
    reply->writeInt32(value);

    return s32Ret;
}

static XBH_S32 watchDogPing(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = pXbhPlatformApiImpl->watchDogPing();
    return s32Ret;
}

static XBH_S32 upgradeTpFirmware(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XBH_U32 u32Value = request.readInt32();
    //s32Ret = XbhTouchManager::getInstance()->upgradeTpFirmware((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getUpgradeTpFirmwareState(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XBH_U32 u32Value = request.readInt32();
    //s32Ret = XbhTouchManager::getInstance()->getUpgradeTpFirmwareState((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getTpDriverVersion(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR version[128] = {0};
    s32Ret = XbhTouchManager::getInstance()->getTpDriverVersion(version);
    reply->writeString16(String16(version));
    return s32Ret;
}

static XBH_S32 getTpFirmwareVersion(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR version[128] = {0};
    s32Ret = XbhTouchManager::getInstance()->getTpFirmwareVersion(version);
    reply->writeString16(String16(version));
    return s32Ret;
}

static XBH_S32 getTpProductModel(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XBH_U32 u32Value = request.readInt32();
    //s32Ret = XbhTouchManager::getInstance()->getTpProductModel((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getTCONValue(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 vlaue = 0;
    s32Ret = pXbhPlatformApiImpl->getTCONValue(&vlaue);
    reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setLiftbracketOperate(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setLiftbracketOperate(s32Value);
    return s32Ret;
}

static XBH_S32 getLiftbracketExist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getLiftbracketExist(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 setLocalDimmingEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XBH_S32 value = request.readInt32();
    //s32Ret = pXbhPlatformApiImpl->setLocalDimmingEnable((XBH_BOOL)value);
    return s32Ret;
}

static XBH_S32 getLocalDimmingEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XBH_S32 vlaue = 0;
    //s32Ret = pXbhPlatformApiImpl->getLocalDimmingEnable((XBH_BOOL*)&vlaue);
    //reply->writeInt32(vlaue);
    return s32Ret;
}

static XBH_S32 setProjectId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *strProjectId = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setProjectId(strProjectId);
    return s32Ret;
}

static XBH_S32 getProjectId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //Align with XBH_CUSDATA_BARCODE_LEN
    XBH_CHAR strProjectId[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getProjectId(strProjectId);
    reply->writeString16(String16(strProjectId));
    return s32Ret;
}

static XBH_S32 setDeviceId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *strProjectId = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setDeviceId(strProjectId);
    return s32Ret;
}

static XBH_S32 getDeviceId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //Align with XBH_CUSDATA_BARCODE_LEN
    XBH_CHAR strProjectId[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getDeviceId(strProjectId);
    reply->writeString16(String16(strProjectId));
    return s32Ret;
}

static XBH_S32 setPowerLedLightEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setPowerLedLightEnable((XBH_BOOL)u32Value);
    return s32Ret;
}

static XBH_S32 getPowerLedLightEnable(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getPowerLedLightEnable((XBH_BOOL *)&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}
static XBH_S32 executeLightSensorInit(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->executeLightSensorInit();
    return s32Ret;
}

static XBH_S32 setMonitorId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 s32Value = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setMonitorId(s32Value);
    return s32Ret;
}

static XBH_S32 getMonitorId(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getMonitorId(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getEthPortSpeed(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int u32Port = request.readInt32();
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getEthPortSpeed(u32Port, &u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getHardwareVer(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int u32Port = request.readInt32();
    int u32Value = 0;
    s32Ret = pXbhPlatformApiImpl->getHardwareVer(&u32Value);
    reply->writeInt32(u32Value);
    return s32Ret;
}

static XBH_S32 getUsbDeviceInfos(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    std::string input = "";
    std::string jsonStr = pXbhPlatformApiImpl->getUsbDeviceInfos(input);
    // XLOGD("jsonStr=%s",jsonStr.c_str());
    const char* byteData = jsonStr.data();
    size_t length = jsonStr.size();
    reply->writeInt32(length);

    for (size_t i = 0; i < length; ++i) {
        unsigned char byte = static_cast<unsigned char>(byteData[i]);
        reply->writeByte(byte);
    }

    s32Ret = XBH_SUCCESS;
    return s32Ret;
}

static XBH_S32 setSftData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 sftString(request.readString16());
    const XBH_CHAR *data = sftString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSftData(data);
    return s32Ret;
}

static XBH_S32 getSftData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSftData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setRkpStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 pnString(request.readString16());
    const XBH_CHAR *data = pnString.c_str();
    s32Ret = pXbhPlatformApiImpl->setRkpStatus(data);
    return s32Ret;
}

static XBH_S32 getRkpStatus(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getRkpStatus(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setSmartPnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 pnString(request.readString16());
    const XBH_CHAR *data = pnString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSmartPnData(data);
    return s32Ret;
}

static XBH_S32 getSmartPnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSmartPnData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setSmartSnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *data = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSmartSnData(data);
    return s32Ret;
}

static XBH_S32 getSmartSnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSmartSnData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setCompassPresist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *data = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setCompassPresist(data);
    return s32Ret;
}

static XBH_S32 getCompassPresist(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getCompassPresist(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setSmartScalerPnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *data = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSmartScalerPnData(data);
    return s32Ret;
}

static XBH_S32 getSmartScalerPnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSmartScalerPnData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setSmartScalerSnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *data = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSmartScalerSnData(data);
    return s32Ret;
}

static XBH_S32 getSmartScalerSnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSmartScalerSnData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setSmartCorePnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *data = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSmartCorePnData(data);
    return s32Ret;
}

static XBH_S32 getSmartCorePnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSmartCorePnData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 setSmartCoreSnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    String8 snString(request.readString16());
    const XBH_CHAR *data = snString.c_str();
    s32Ret = pXbhPlatformApiImpl->setSmartCoreSnData(data);
    return s32Ret;
}

static XBH_S32 getSmartCoreSnData(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getSmartCoreSnData(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 getAntennaRssi(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getAntennaRssi(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 saveFactoryMac(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int macType = request.readInt32();
    String8 macString(request.readString16());
    const XBH_CHAR *macAddress = macString.c_str();
    s32Ret = pXbhPlatformApiImpl->saveFactoryMac((XBH_MAC_ADDRESS_TYPE)macType, macAddress);
    return s32Ret;
}

static XBH_S32 getFactoryMac(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_S32 macType = request.readInt32();
    XBH_CHAR macaddress[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getFactoryMac((XBH_MAC_ADDRESS_TYPE)macType, macaddress);
    reply->writeString16(String16(macaddress));
    return s32Ret;
}

static XBH_S32 getGoogleKeyID(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR data[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getGoogleKeyID(data);
    reply->writeString16(String16(data));
    return s32Ret;
}

static XBH_S32 getMicLicenseReqStr(const Parcel &request, Parcel *reply)
{
    char *lic_request = "/data/vendor/lic_request";
    int lic_count = 0;
    int lic_request_fd = -1;
    int read_lic_len = 0;
    std::string lic_str("");
    char read_buffer[100];
    memset(read_buffer, 0x00, 100);

    char prop[PROPERTY_VALUE_MAX];
    property_get("persist.vendor.xbh.array.mic", prop, "null");
    // 311D2.X板pdm算法启动流程不同
    if (strcmp(prop, "lango") == 0)
    {
        XLOGI("------------ current array MIC is lango ------------");
        if (access(lic_request, F_OK) != 0)
        {
            property_set("ctl.start", "uac_app");
        }
        while (access(lic_request, F_OK) != 0 && lic_count++ < 100)
        {
            usleep(20 * 1000);
        }
        property_set("ctl.stop", "uac_app");
    }
    else
    {
        if (access(lic_request, F_OK) != 0)
        {
            property_set("persist.vendor.xbh.usb.uac_android_enable", "true");
        }
        while (access(lic_request, F_OK) != 0 && lic_count++ < 100)
        {
            usleep(20 * 1000);
        }
        property_set("persist.vendor.xbh.usb.uac_android_enable", "false");
    }
    if (lic_count >= 100)
    {
        return XBH_FAILURE;
    }
    lic_request_fd = open(lic_request, O_RDONLY | O_NONBLOCK);
    if(lic_request_fd < 0)
    {
        return XBH_FAILURE;
    }

    while ((read_lic_len = read(lic_request_fd, read_buffer, 99)) > 0)
    {
        lic_str += std::string(read_buffer);
        memset(read_buffer, 0x00, 100);
    }
    reply->writeString16(String16(lic_str.c_str()));
    return XBH_SUCCESS;
}

static XBH_S32 setMicCertificate(const Parcel &request, Parcel *reply)
{
    char *lic_response = "/data/vendor/lic_response";
    int lic_response_fd = -1;
    String8 value8(request.readString16());
    std::string cer_lic(value8.c_str());
    int writelen = 0;
    if (access(lic_response, F_OK) == 0)
    {
        return XBH_FAILURE;
    }

    lic_response_fd = open(lic_response, O_CREAT | O_WRONLY, 0777);
    if (lic_response_fd < 0)
    {
        return XBH_FAILURE;
    }

    writelen = write(lic_response_fd, cer_lic.c_str(), cer_lic.size());
    if (writelen != cer_lic.size())
    {
        return XBH_FAILURE;
    }

    return XBH_SUCCESS;
}

static XBH_S32 getMicLicenseState(const Parcel &request, Parcel *reply)
{
    char *lic_response = "/data/vendor/lic_response";
    if(access(lic_response, F_OK) == 0)
    {
        reply->writeInt32(0);
        return XBH_SUCCESS;
    }
    else
    {
        reply->writeInt32(1);
        return XBH_FAILURE;
    }
}

static XBH_S32 burnFactorySN(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int materialType = request.readInt32();
    String8 macString(request.readString16());
    const XBH_CHAR *snInfo = macString.c_str();
    s32Ret = pXbhPlatformApiImpl->burnFactorySN((XBH_MATERAL_TYPE)materialType, snInfo);
    return s32Ret;
}

static XBH_S32 getBurnFactorySN(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    const XBH_S32 materialType = request.readInt32();
    XBH_CHAR snInfo[64] = {0};
    s32Ret = pXbhPlatformApiImpl->getBurnFactorySN((XBH_MATERAL_TYPE)materialType, snInfo);
    reply->writeString16(String16(snInfo));
    return s32Ret;
}

static XBH_S32 setTypeCPdAbility(const Parcel &request, Parcel *reply)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 srcIdx = request.readInt32();
    XBH_U32 pdability = request.readInt32();
    s32Ret = pXbhPlatformApiImpl->setTypeCPdAbility((XBH_PDCAPACITY_POWER)pdability, (XBH_SOURCE_E)srcIdx);
    //reply->writeString16(String16(snInfo));
    return s32Ret;
}

//---------------------------------------------------------------------------- DEVICE end -----------------------------------------------------------

static LocalAtomicInfo g_astCustomerModuleLocalAtomicInfo[] = {
    //PICTURE
    {CMD_PICTURE_SETPICTUREMODE,                    setPictureMode},
    {CMD_PICTURE_GETPICTUREMODE,                    getPictureMode},
    {CMD_PICTURE_SETBRIGHTNESS,                     setBrightness},
    {CMD_PICTURE_GETBRIGHTNESS,                     getBrightness},
    {CMD_PICTURE_SETCONTRAST,                       setContrast},
    {CMD_PICTURE_GETCONTRAST,                       getContrast},
    {CMD_PICTURE_SETSATURATION,                     setSaturation},
    {CMD_PICTURE_GETSATURATION,                     getSaturation},
    {CMD_PICTURE_SETSHARPNESS,                      setSharpness},
    {CMD_PICTURE_GETSHARPNESS,                      getSharpness},
    {CMD_PICTURE_SETHUE,                            setHue},
    {CMD_PICTURE_GETHUE,                            getHue},
    {CMD_PICTURE_SETBACKLIGHT,                      setBacklight},
    {CMD_PICTURE_GETBACKLIGHT,                      getBacklight},
    {CMD_PICTURE_SETBACKLIGHT_WITHOUTSAVE,          setBacklightWithOutSave},
    {CMD_PICTURE_SETCOLORTEMPMODE,                  setColorTempMode},
    {CMD_PICTURE_GETCOLORTEMPMODE,                  getColorTempMode},
    {CMD_PICTURE_SETNR,                             setNR},
    {CMD_PICTURE_GETNR,                             getNR},
    {CMD_PICTURE_SETGAMEMODEENABLE,                 setGameModeEnable},
    {CMD_PICTURE_GETGAMEMODEENABLE,                 getGameModeEnable},
    {CMD_PICTURE_SETPCMODE,                         setPCMode},
    {CMD_PICTURE_GETPCMODE,                         getPCMode},
    {CMD_PICTURE_SETHDMICOLORRANGE,                 setHDMIColorRange},
    {CMD_PICTURE_GETHDMICOLORRANGE,                 getHDMIColorRange},
    {CMD_PICTURE_SETDCRENABLE,                      setDCREnable},
    {CMD_PICTURE_GETDCRENABLE,                      getDCREnable},
    {CMD_PICTURE_SETMEMCLEVEL,                      setMEMCLevel},
    {CMD_PICTURE_GETMEMCLEVEL,                      getMEMCLevel},
    {CMD_PICTURE_SETASPECT,                         setAspect},
    {CMD_PICTURE_GETASPECT,                         getAspect},
    {CMD_PICTURE_GETAVGLUMA,                        getAvgPixelLuma},
    {CMD_PICTURE_SETGAMMAGROUP,                     setGammaGroup},
    {CMD_PICTURE_GETGAMMAGROUP,                     getGammaGroup},
    {CMD_PICTURE_SETCOLORTEMPPARE,                  setColorTempPara},
    {CMD_PICTURE_GETCOLORTEMPPARE,                  getColorTempPara},
    {CMD_PICTURE_SET_LOCALDIIMMNG,                  setLocalDimmingEnable},
    {CMD_PICTURE_GET_LOCALDIIMMNG,                  getLocalDimmingEnable},
    {CMD_PICTURE_GETBACKLIGHTWITHOUTSAVE,           getBacklightWithOutSave},
    {CMD_PICTURE_SETSHARPENABLE,                    setSharpEnable},

    //AUDIO
    {CMD_AUDIO_SETSOUNDMODE,                        setSoundMode},
    {CMD_AUDIO_GETSOUNDMODE,                        getSoundMode},
    {CMD_AUDIO_SETVOLUME,                           setVolume},
    {CMD_AUDIO_GETVOLUME,                           getVolume},
    {CMD_AUDIO_SETAUDIOOUTPUT,                      setAudioOutput},
    {CMD_AUDIO_GETAUDIOOUTPUT,                      getAudioOutput},
    {CMD_AUDIO_SETMUTE,                             setMute},
    {CMD_AUDIO_GETMUTE,                             getMute},
    {CMD_AUDIO_SETSPDIFMODE,                        setSpdifMode},
    {CMD_AUDIO_GETSPDIFMODE,                        getSpdifMode},
    {CMD_AUDIO_SETSPDIFENABLE,                      setSpdifEnable},
    {CMD_AUDIO_GETSPDIFENABLE,                      getSpdifEnable},
    {CMD_AUDIO_SETARCMODE,                          setARCMode},
    {CMD_AUDIO_GETARCMODE,                          getARCMode},
    {CMD_AUDIO_SETARCENABLE,                        setARCEnable},
    {CMD_AUDIO_GETARCENABLE,                        getARCEnable},
    {CMD_AUDIO_SETPRESCALE,                         setPreScale},
    {CMD_AUDIO_GETPRESCALE,                         getPreScale},
    {CMD_AUDIO_SETAUDIOEQ,                          setAudioEq},
    {CMD_AUDIO_GETAUDIOEQ,                          getAudioEq},
    {CMD_AUDIO_SETBASS,                             setBass},
    {CMD_AUDIO_GETBASS,                             getBass},
    {CMD_AUDIO_SETTREBLE,                           setTreble},
    {CMD_AUDIO_GETTREBLE,                           getTreble},
    {CMD_AUDIO_SETBALANCE,                          setBalance},
    {CMD_AUDIO_GETBALANCE,                          getBalance},
    {CMD_AUDIO_SETLINEOUTMODE,                      setLineOutMode},
    {CMD_AUDIO_GETLINEOUTMODE,                      getLineOutMode},
    {CMD_AUDIO_SETSURROUNDENABLE,                   setSurroundEnable},
    {CMD_AUDIO_GETSURROUNDENABLE,                   getSurroundEnable},
    {CMD_AUDIO_SETAVCENABLE,                        setAvcEnable},
    {CMD_AUDIO_GETAVCENABLE,                        getAvcEnable},
    {CMD_AUDIO_SETEQENABLE,                         setEqEnable},
    {CMD_AUDIO_GETEQENABLE,                         getEqEnable},
    {CMD_AUDIO_SETAMPMODE,                          setAmpMode},
    {CMD_AUDIO_GETAMPMODE,                          getAmpMode},
    {CMD_AUDIO_GETMICDET,                           getMicDetect},
    {CMD_AUDIO_GETHPDET,                            getHeadphoneDetect},
    {CMD_AUDIO_GETARCDET,                           getArcDetect},
    {CMD_AUDIO_SETLNIEOUTGAIN,                      setLineOutGain},
    {CMD_AUDIO_GETLNIEOUTGAIN,                      getLineOutGain},
    {CMD_AUDIO_SETMICINGAIN,                        setMicInGain},
    {CMD_AUDIO_GETMICINGAIN,                        getMicInGain},
    {CMD_AUDIO_SETMICHOWLING,                       setMicHowling},
    {CMD_AUDIO_GETMICHOWLING,                       getMicHowling},
    {CMD_AUDIO_SETSPEAKERGAIN,                      setSpeakerGain},
    {CMD_AUDIO_GETSPEAKERGAIN,                      getSpeakerGain},
    {CMD_AUDIO_SETSPEAKERDP,                        setSpeakerDp},
    {CMD_AUDIO_GETSPEAKERDP,                        getSpeakerDp},

    //VGA
    {CMD_VGA_AUTOADC,                               autoAdc},
    {CMD_VGA_AUTOADJUST,                            autoAdjust},
    {CMD_VGA_SETHPOSITION,                          setHPosition},
    {CMD_VGA_GETHPOSITION,                          getHPosition},
    {CMD_VGA_SETVPOSITION,                          setVPosition},
    {CMD_VGA_GETVPOSITION,                          getVPosition},
    {CMD_VGA_SETCLOCK,                              setClock},
    {CMD_VGA_GETCLOCK,                              getClock},
    {CMD_VGA_SETPHASE,                              setPhase},
    {CMD_VGA_GETPHASE,                              getPhase},

    //CEC
    {CMD_CEC_SETCECENABLE,                          setCecEnable},
    {CMD_CEC_GETCECENABLE,                          getCecEnable},
    {CMD_CEC_SETAUTOPOWEROFFENABLE,                 setCecAutoPowerOffEnable},
    {CMD_CEC_GETAUTOPOWEROFFENABLE,                 getCecAutoPowerOffEnable},
    {CMD_CEC_SETAUTOWAKEUPENABLE,                   setCecAutoWakeUpEnable},
    {CMD_CEC_GETAUTOWAKEUPENABLE,                   getCecAutoWakeUpEnable},

    //SOURCE
    {CMD_SOURCE_SELECTSOURCE,                       selectSource},
    {CMD_SOURCE_SETSOURCESWITCHPORT,                setSourceSwitchPort},
    {CMD_SOURCE_SETSOURCEUSBPORT,                   setSourceUsbPort},
    {CMD_SOURCE_SETWINDOWRECT,                      setWindowRect},
    {CMD_SOURCE_GETWINDOWRECT,                      getWindowRect},
    {CMD_SOURCE_GETTIMINGINFO,                      getTimingInfo},
    {CMD_SOURCE_SETHDMIRXEDIDTYPE,                  setHdmirxEdidType},
    {CMD_SOURCE_GETHDMIRXEDIDTYPE,                  getHdmirxEdidType},
    {CMD_SOURCE_GETSOURCEDET,                       getSourceDet},
    {CMD_SOURCE_SETOPSPOWERCTRL,                    setOpsPowerCtrl},
    {CMD_SOURCE_GETOPSPOWERSTATUS,                  getOpsPowerStatus},
    {CMD_SOURCE_GETOPSDETSTATUS,                    getOpsDetStatus},
    {CMD_SOURCE_SETOPSPOWERENABLE,                  setOpsPowerEnable},
    {CMD_SOURCE_GETOPSPOWERENABLE,                  getOpsPowerEnable},
    {CMD_SOURCE_SETHDMITXINFO,                      setHdmiTxInfo},
    {CMD_SOURCE_GETHDMITXINFO,                      getHdmiTxInfo},
    {CMD_SOURCE_GETINPUTID,                         getInputId},
    {CMD_SOURCE_SETOPSRESETENABLE,                  setOpsResetEnable},
    {CMD_SOURCE_GETSOURCEEDIDTYPE,                  getSourceEdidType},
    {CMD_SOURCE_SETSOURCEEDIDTYPE,                  setSourceEdidType},
    {CMD_SOURCE_SETVIDEOTXMODE,                     setVideoTxMode},
    {CMD_SOURCE_GETVIDEOTXMODE,                     getVideoTxMode},

    //SYSTEM
    {CMD_SYSTEM_GETWAKEUP,                          getWakeUpReason},
    {CMD_SYSTEM_SETVENDORPROP,                      setVendorProperties},
    {CMD_SYSTEM_SETPOWERONMODE,                     setPoweronMode},
    {CMD_SYSTEM_GETPOWERONMODE,                     getPoweronMode},
    {CMD_SYSTEM_SETWOTSTATUS,                       setWoTStatus},
    {CMD_SYSTEM_GETWOTSTATUS,                       getWoTStatus},
    {CMD_SYSTEM_SETSRCTPSTATUS,                     setSourceTouchState},
    {CMD_SYSTEM_GETSRCTPSTATUS,                     getSourceTouchState},
    {CMD_SYSTEM_SETNONTPREGION,                     setNonThroughTouchRegion},
    {CMD_SYSTEM_DELNONTPREGION,                     deleteNonThroughTouchRegion},
    {CMD_SYSTEM_SETTOUCHSCALERRECT,                 setTouchScaleRect},
    {CMD_SYSTEM_SENDKEYTOTP,                        sendKeyToTp},
    {CMD_SYSTEM_SENDDATATOTP,                       sendDataToTp},
    {CMD_SYSTEM_SETCHIPRUNTIME,                     setChipRunTime},
    {CMD_SYSTEM_GETCHIPRUNTIME,                     getChipRunTime},
    {CMD_SYSTEM_SETEYWALOGLEVEL,                    setEywaLogLevel},
    {CMD_SYSTEM_GETEYWALOGLEVEL,                    getEywaLogLevel},
    {CMD_SYSTEM_SETBOOTLOGO,                        setBootLogo},
    {CMD_SYSTEM_SETBOOTVIDEO,                       setBootVideo},
    {CMD_SYSTEM_SETBOOTANIMATION,                   setBootAnimation},
    {CMD_SYSTEM_SETPALMREJECTION,                   setPalmRejection},
    {CMD_SYSTEM_GETPALMREJECTION,                   getPalmRejection},
    {CMD_SYSTEM_LOADEDLACUST,                       loadEdlaCust},
    {CMD_SYSTEM_GETPROJECTIDVERSION,                getProjectIdVersion},
    {CMD_SYSTEM_SETSTANDBYMODE,                     setStandbyMode},
    {CMD_SYSTEM_GETSTANDBYMODE,                     getStandbyMode},
    {CMD_SYSTEM_GETTPCONNECT,                       getTpConnect},
    {CMD_SYSTEM_GETTPNAME,                          getTpName},
    {CMD_SYSTEM_SETONSTOP,                          setOnStop},
    {CMD_SYSTEM_SETSCREENOFFSET,                    setSourceScreenOffset},
    {CMD_SYSTEM_GETSCREENOFFSET,                    getSourceScreenOffset},
    {CMD_SYSTEM_SETONRESUME,                        setOnResume},
    {CMD_SYSTEM_UPGRADETP,                          upgradeTpFirmware},
    {CMD_SYSTEM_GETUPGRADETPSTATE,                  getUpgradeTpFirmwareState},
    {CMD_SYSTEM_GETTPDRIVERVERSION,                 getTpDriverVersion},
    {CMD_SYSTEM_GETTPPRODUCTMODEL,                  getTpProductModel},
    {CMD_SYSTEM_GETTPFIRMWAREVERSION,               getTpFirmwareVersion},
    {CMD_SYSTEM_GETCONVALUE,                        getTCONValue},
    {CMD_SYSTEM_SETMONITORID,                       setMonitorId},
    {CMD_SYSTEM_GETMONITORID,                       getMonitorId},
    {CMD_SYSTEM_GETETHPORTSPEED,                    getEthPortSpeed},
    {CMD_SYSTEM_GETHARDWAREVER,                     getHardwareVer},
    {CMD_SYSTEM_GETUSBDEVICEINFOS,                  getUsbDeviceInfos},
    {CMD_SYSTEM_GETMICLICENSEREQSTR,                getMicLicenseReqStr},
    {CMD_SYSTEM_GETMICLICENSESTATE,                 getMicLicenseState},
    {CMD_SYSTEM_SETMICCERTIFICATE,                  setMicCertificate},
    {CMD_SYSTEM_SETSETTYPECPDABILITY,               setTypeCPdAbility},
    //FACTORY
    {CMD_FACTORY_SETOVERSCAN,                       setOverscan},
    {CMD_FACTORY_GETOVERSCAN,                       getOverscan},
    {CMD_FACTORY_SETTESTPATTERN,                    setTestPattern},
    {CMD_FACTORY_GETTESTPATTERN,                    getTestPattern},
    {CMD_FACTORY_SETPANELINDEX,                     setPanelIndex},
    {CMD_FACTORY_GETPANELINDEX,                     getPanelIndex},
    {CMD_FACTORY_SETPWMFREQUENCY,                   setPwmFrequency},
    {CMD_FACTORY_GETPWMFREQUENCY,                   getPwmFrequency},
    {CMD_FACTORY_SETPWMPOLARITY,                    setPwmPolarity},
    {CMD_FACTORY_GETPWMPOLARITY,                    getPwmPolarity},
    {CMD_FACTORY_SETDEBUGENABLE,                    setDebugEnable},
    {CMD_FACTORY_GETDEBUGENABLE,                    getDebugEnable},
    {CMD_FACTORY_SETMACADDRESS,                     setMacAddress},
    {CMD_FACTORY_GETMACADDRESS,                     getMacAddress},
    {CMD_FACTORY_SETSERIALNUMBER,                   setSn},
    {CMD_FACTORY_GETSERIALNUMBER,                   getSn},
    {CMD_FACTORY_SETHDCPKEY,                        setHdcp},
    {CMD_FACTORY_GETHDCPKEYSTATUS,                  getHdcpStatus},
    {CMD_FACTORY_SETHDMIEDID,                       setHdmiEdid},
    {CMD_FACTORY_GETHDMIEDIDSTATUS,                 getHdmiEdidStatus},
    {CMD_FACTORY_SETVGAEDID,                        setVgaEdid},
    {CMD_FACTORY_GETVGAEDIDSTATUS,                  getVgaEdidStatus},
    {CMD_FACTORY_SETGOOGLEKEY,                      setGoogleKey},
    {CMD_FACTORY_GETGOOGLEKEYSTATUS,                getGoogleKeyStatus},
    {CMD_FACTORY_SETATTENTIONKEY,                   setAttentionKey},
    {CMD_FACTORY_GETATTENTIONKEYSTATUS,             getAttentionKeyStatus},
    {CMD_FACTORY_UPDATESYSTEMFIRMWARE,              upgradeSystemFirmware},
    {CMD_FACTORY_SETHDCPKEYNAME,                    setHdcpKeyName},
    {CMD_FACTORY_GETHDCPKEYNAME,                    getHdcpKeyName},
    {CMD_FACTORY_SETGOOGLEKEYNAME,                  setGoogleKeyName},
    {CMD_FACTORY_GETGOOGLEKEYNAME,                  getGoogleKeyName},
    {CMD_FACTORY_DUMPEDID,                          dumpEdid},
    {CMD_FACTORY_GETVGAEDIDCHECKSUM,                getVgaEdidCheckSum},
    {CMD_FACTORY_SETPROJECTID,                      setProjectId},
    {CMD_FACTORY_GETPROJECTID,                      getProjectId},
    {CMD_FACTORY_SETDEVICEID,                       setDeviceId},
    {CMD_FACTORY_GETDEVICEID,                       getDeviceId},
    {CMD_FACTORY_SETPANELMAPSWAP,                   setPanelMapSwap},
    {CMD_FACTORY_GETPANELMAPSWAP,                   getPanelMapSwap},
    {CMD_FACTORY_SETAGINGMODESTATE,                 setAgingModeState},
    {CMD_FACTORY_GETAGINGMODESTATE,                 getAgingModeState},
    {CMD_FACTORY_SETUSBCCONFIG,                     setUsbcConfig},
    {CMD_FACTORY_GETUSBCCONFIG,                     getUsbcConfig},
    {CMD_FACTORY_SETMOKAPMODEENABLETOCUSDATA,       setMokaPModeEnableToCusdata},
    {CMD_FACTORY_GETMOKAPMODEENABLEFROMCUSDATA,     getMokaPModeEnableFromCusdata},
    {CMD_FACTORY_SETSFTDATA,                        setSftData},
    {CMD_FACTORY_GETSFTDATA,                        getSftData},
    {CMD_FACTORY_SETSMARTPNDATA,                    setSmartPnData},
    {CMD_FACTORY_GETSMARTPNDATA,                    getSmartPnData},
    {CMD_FACTORY_SETSMARTSNDATA,                    setSmartSnData},
    {CMD_FACTORY_GETSMARTSNDATA,                    getSmartSnData},
    {CMD_FACTORY_SETCOMPASSPRESIST,                 setCompassPresist},
    {CMD_FACTORY_GETCOMPASSPRESIST,                 getCompassPresist},
    {CMD_FACTORY_SETSMARTSCALERPNDATA,              setSmartScalerPnData},
    {CMD_FACTORY_GETSMARTSCALERPNDATA,              getSmartScalerPnData},
    {CMD_FACTORY_SETSMARTSCALERSNDATA,              setSmartScalerSnData},
    {CMD_FACTORY_GETSMARTSCALERSNDATA,              getSmartScalerSnData},
    {CMD_FACTORY_SETSMARTCOREPNDATA,                setSmartCorePnData},
    {CMD_FACTORY_GETSMARTCOREPNDATA,                getSmartCorePnData},
    {CMD_FACTORY_SETSMARTCORESNDATA,                setSmartCoreSnData},
    {CMD_FACTORY_GETSMARTCORESNDATA,                getSmartCoreSnData},
    {CMD_FACTORY_GETANTENNARSSI,                    getAntennaRssi},
    {CMD_FACTORY_SAVEFACTORYMAC,                    saveFactoryMac},
    {CMD_FACTORY_GETFACTORYMAC,                     getFactoryMac},
    {CMD_FACTORY_GETGOOGLEKEYID,                    getGoogleKeyID},
    {CMD_FACTORY_BURNFACTORYSN,                     burnFactorySN},
    {CMD_FACTORY_GETBURNFACTORYSN,                  getBurnFactorySN},
    {CMD_FACTORY_SETRKPSTATUSDATA,                  setRkpStatus},
    {CMD_FACTORY_GETRKPSTATUSDATA,                  getRkpStatus},
    {CMD_FACTORY_GETHDCPKEYDATA,                    getHdcpKeydata},
    //DEVICE
    {CMD_DEVICE_SETGPIOOUTPUTVALUE,                 setGpioOutputValue},
    {CMD_DEVICE_GETGPIOOUTPUTVALUE,                 getGpioOutputValue},
    {CMD_DEVICE_GETGPIOINPUTVALUE,                  getGpioInputValue},
    {CMD_DEVICE_SETRTCTIME,                         setRtcTime},
    {CMD_DEVICE_GETRTCTIME,                         getRtcTime},
    {CMD_DEVICE_SETRTCALARMTIME,                    setRtcAlarmTime},
    {CMD_DEVICE_SETIICVALUE,                        setIICData},
    {CMD_DEVICE_GETIICVALUE,                        getIICData},
    {CMD_DEVICE_SETBACKLIGHTENABLE,                 setBacklightEnable},
    {CMD_DEVICE_GETBACKLIGHTENABLE,                 getBacklightEnable},
    {CMD_DEVICE_SETPANELPOWER,                      setPanelPower},
    {CMD_DEVICE_GETPANELPOWER,                      getPanelPower},
    {CMD_DEVICE_SETLEDPOWERSTATUS,                  setLedPwrStatus},
    {CMD_DEVICE_GETLEDPOWERSTATUS,                  getLedPwrStatus},
    {CMD_DEVICE_GETETHPLUGSTATUS,                   getEthPlugStatus},
    {CMD_DEVICE_GETADCCHANNELVALUE,                 getADCChannelValue},
    {CMD_DEVICE_GETLIGHTSENSOREXIST,                getLightSensorExist},
    {CMD_DEVICE_GETPIRSENSOREXIST,                  getPirSensorExist},
    {CMD_DEVICE_GETHALLSENSOREXIST,                 getHallSensorExist},
    {CMD_DEVICE_GETAIRSENSOREXIST,                  getAirSensorExist},
    {CMD_DEVICE_GETTEMPERATURESENSOREXIST,          getTemperatureSensorExist},
    {CMD_DEVICE_GETGSENSOREXIST,                    getGSensorExist},
    {CMD_DEVICE_GETLIGHTSENSORVALUE,                getLightSensorValue},
    {CMD_DEVICE_GETPIRSENSORVALUE,                  getPirSensorValue},
    {CMD_DEVICE_GETHALLSENSORVALUE,                 getHallSensorValue},
    {CMD_DEVICE_GETAIRSENSORVALUE,                  getAirSensorValue},
    {CMD_DEVICE_GETTEMPERATURESENSORVALUE,          getTemperatureSensorValue},
    {CMD_DEVICE_GETGSENSORVALUE,                    getGSensorValue},
    {CMD_DEVICE_UPGRADEEXTERNDIC,                   upgradeExtendIc},
    {CMD_DEVICE_GETUPGRADEEXTICSTATUS,              getUpgradeExtIcState},
    {CMD_DEVICE_GETEXTENDICVER,                     getExtendIcVer},
    {CMD_DEVICE_SETDDRSSC,                          setDdrSSC},
    {CMD_DEVICE_GETDDRSSC,                          getDdrSSC},
    {CMD_DEVICE_SETPANELSSC,                        setPanelSSC},
    {CMD_DEVICE_GETPANELSSC,                        getPanelSSC},
    {CMD_DEVICE_GETPANELNAME,                       getPanelName},
    {CMD_DEVICE_GETPANELRESOLUTION,                 getPanelResolution},
    {CMD_DEVICE_SETPANELROTATION,                   setPanelRotation},
    {CMD_DEVICE_GETPANELROTATION,                   getPanelRotation},
    {CMD_DEVICE_SETMULTIPANEL,                      setMultiPanel},
    {CMD_DEVICE_GETMULTIPANEL,                      getMultiPanel},
    {CMD_DEVICE_SETPANELMIRROR,                     setPanelFlipMirror},
    {CMD_DEVICE_GETPANELMIRROR,                     getPanelFlipMirror},
    {CMD_DEVICE_GETOPSLOCKSTATUS,                   getOpsLockStatus},
    {CMD_DEVICE_SETPIXELSHIFTENABLE,                setPixelShiftEnable},
    {CMD_DEVICE_GETPIXELSHIFTENABLE,                getPixelShiftEnable},
    {CMD_DEVICE_SETGRAPHICBRIGHTNESS,               setGraphicBrightness},
    {CMD_DEVICE_GETGRAPHICBRIGHTNESS,               getGraphicBrightness},
    {CMD_DEVICE_SETGRAPHICCONTRAST,                 setGraphicContrast},
    {CMD_DEVICE_GETGRAPHICCONTRAST,                 getGraphicContrast},
    {CMD_DEVICE_SETGRAPHICSATURATION,               setGraphicSaturation},
    {CMD_DEVICE_GETGRAPHICSATURATION,               getGraphicSaturation},
    {CMD_DEVICE_SETGRAPHICTEMPMODE,                 setGraphicColorTempMode},
    {CMD_DEVICE_GETGRAPHICTEMPMODE,                 getGraphicColorTempMode},
    {CMD_DEVICE_SETENERGYSTARENABLE,                setEnergyStarEnable},
    {CMD_DEVICE_GETENERGYSTARENABLE,                getEnergyStarEnable},
    {CMD_DEVICE_SETADCGAINOFFSET,                   setAdcGainOffset},
    {CMD_DEVICE_GETADCGAINOFFSET,                   getAdcGainOffset},
    {CMD_DEVICE_GETUSBLOCKMODE,                     getUsbLockMode},
    {CMD_DEVICE_SETUSBLOCKMODE,                     setUsbLockMode},
    {CMD_DEVICE_GETOPSOVERCURRENTSTATUS,            getOpsOvercurrentStatus},
    {CMD_DEVICE_ISNFCEXIST,                         isNfcExist},
    {CMD_DEVICE_SETNFCENABLE,                       setNfcEnable},
    {CMD_DEVICE_GETNFCENABLE,                       getNfcEnable},
    {CMD_DEVICE_GETNFCID,                           getNfcId},
    {CMD_DEVICE_ADDNFCID,                           addNfcId},
    {CMD_DEVICE_DELNFCID,                           delNfcId},
    {CMD_DEVICE_DELALLNFCID,                        delAllNfcId},
    {CMD_DEVICE_SETOPSOVERCURRENTPROTECTION,        setOpsOvercurrentProtection},
    {CMD_DEVICE_SETBACKLIGHTMAX,                    setBrightMax},
    {CMD_DEVICE_GETBACKLIGHTMAX,                    getBrightMax},
    {CMD_DEVICE_SETBACKLIGHTMIN,                    setBrightMin},
    {CMD_DEVICE_GETBACKLIGHTMIN,                    getBrightMin},
    {CMD_DEVICE_SETMCUFATTMODE,                     setMcuFattMode},
    {CMD_DEVICE_GETMCUFATTMODE,                     getMcuFattMode},
    {CMD_DEVICE_SETMCUIICBYPASS,                    setMcuIIcBypass},
    {CMD_DEVICE_GETMCUIICBYPASS,                    getMcuIIcBypass},
    {CMD_DEVICE_GETFATTKEYPADINFO,                  getFattKeypadInfo},
    {CMD_DEVICE_SETFATTKEYPADENBALE,                setFattKeypadenbale},
    {CMD_DEVICE_SETMCUUARTBYPASSPARA,               setMcuUartBypassPara},
    {CMD_DEVICE_SETMCUUARTBYPASS,                   setMcuUartBypass},
    {CMD_DEVICE_GETMCUUARTBYPASS,                   getMcuUartBypass},
    {CMD_DEVICE_SET_WATCHDOGMANUALMODEENABLE,       setWatchDogManualModeEnable},
    {CMD_DEVICE_GET_WATCHDOGMANUALMODEENABLE,       getWatchDogManualModeEnable},
    {CMD_DEVICE_SET_WATCHDOGENABLE,                 setWatchDogEnable},
    {CMD_DEVICE_SET_WATCHDOGETIMEOUT,               setWatchDogTimeOut},
    {CMD_DEVICE_GET_WATCHDOGETIMEOUT,               getWatchDogTimeOut},
    {CMD_DEVICE_WATCHDOGPING,                       watchDogPing},
    {CMD_DEVICE_SETHDCPEXTENDIC,                    setHdcpExtendIc},
    {CMD_DEVICE_GETHDCPEXTICSTATUS,                 getHdcpExtIcStatus},
    {CMD_DEVICE_SETHDCPEXTICKEYNAME,                setHdcpExtIcKeyName},
    {CMD_DEVICE_GETHDCPEXTICKEYNAME,                getHdcpExtIcKeyName},
    {CMD_DEVICE_SETLIFTBRACKETOPERATE,              setLiftbracketOperate},
    {CMD_DEVICE_GETLIFTBRACKETEXIST,                getLiftbracketExist},
    {CMD_DEVICE_SETBACKLIGHTDATA,                   setBacklightData},
    {CMD_DEVICE_GETPOWERSENSORVALUE,                getPowerSensorValue},
    {CMD_DEVICE_GETPOWERSENSOREXIST,                getPowerSensorExist},
    {CMD_DEVICE_SETGPIOCOMMONCTRL,                  setGpioCommonCtrl},
    {CMD_DEVICE_GETGPIOCOMMONCTRL,                  getGpioCommonCtrl},
    {CMD_DEVICE_GETRKPCSRINFO,                      getRkpCsrInfo},
    {CMD_DEVICE_SETPOWERLEDLIGHTENABLE,             setPowerLedLightEnable},
    {CMD_DEVICE_GETPOWERLEDLIGHTENABLE,             getPowerLedLightEnable},
    {CMD_DEVICE_EXECUTELIGHTSENSORINIT,             executeLightSensorInit},
    {CMD_DEVICE_SETWIFICHANElPLAN,                  setWifiChanelPlan},
    {CMD_DEVICE_SETHDCPTXENCRYPTIONMODE,            setHdcpTxEncryptionMode},
    {CMD_DEVICE_GETHDCPTXENCRYPTIONMODE,            getHdcpTxEncryptionMode},
    {CMD_DEVICE_GETWAKEUPNFCID,                     getWakeUpNfcId},
    {CMD_DEVICE_SETFLASHEEPDATA,                    setFlashEepData},
    {CMD_DEVICE_GETFLASHEEPDATA,                    getFlashEepData},
    {CMD_DEVICE_GETHALLSENSORPEN,                   getHallSensorPen},
    {CMD_DEVICE_GETOPSSYSTEMTYPE,                   getOpsSystemType},
    {CMD_DEVICE_GETNTCVALUE,                        getNTCValue},
};

static void platformCreate()
{
    XLOGE("--------------- platformCreate begin  TYPE_BOARD = %d -----------------", TYPE_BOARD);
    #ifdef AML_311D2
    XLOGE("--------------- platformCreate AML311D2 begin -----------------");
    pXbhPlatformApiImpl = (XbhAmlogic_311d2*)mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::AMLOGIC_311D2);
    pXbhModuleApiImpl = (XbhAmlogic_311d2*)mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::AMLOGIC_311D2);
    #endif

    #ifdef MTK_9679
    XLOGE("--------------- platformCreate MTK_9679 begin -----------------");
    pXbhPlatformApiImpl = (XbhMtk_9679* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::MEDIATEK_9679);
    pXbhModuleApiImpl = (XbhMtk_9679* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::MEDIATEK_9679);
    #endif

    #ifdef RK_3576
    XLOGE("--------------- platformCreate RK_3576 begin -----------------");
    pXbhPlatformApiImpl = (XbhRk_3576* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::ROCKCHIP_3576);
    pXbhModuleApiImpl = (XbhRk_3576* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::ROCKCHIP_3576);
    #endif

    #ifdef MTK_8195
    XLOGE("--------------- platformCreate MTK_8195 begin -----------------");
    pXbhPlatformApiImpl = (XbhMtk_8195* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::MEDIATEK_8195);
    pXbhModuleApiImpl = (XbhMtk_8195* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::MEDIATEK_8195);
    #endif

    #ifdef HISI_610
    XLOGE("--------------- platformCreate HISI_610 begin -----------------");
    pXbhPlatformApiImpl = (XbhHi_610* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::HISILICON_610);
    pXbhModuleApiImpl = (XbhHi_610* )mXbhPlatformFactory.CreatePlatform(PLATFORM_TYPE::HISILICON_610);
    #endif

    if(pXbhPlatformApiImpl == XBH_NULL)
    {
        XLOGE("--------------- pXbhPlatformApiImpl is null -----------------");
    }
    if(pXbhModuleApiImpl == XBH_NULL)
    {
        XLOGE("--------------- pXbhModuleApiImpl is null -----------------");
    }
    XLOGE("--------------- platformCreate end -----------------");
}

static void init()
{
    //When the SIGPIPE signal occurs, it is ignored. Common SIGPIPE, such as read and write in sockets
    signal(SIGPIPE, SIG_IGN);
	property_set("vendor.xbh.eywa.build.date", BUILD_DATE);
    XbhLog::init();
    XLOGI("######################## Xbh hidl init start ##########################");
    //--------------------------- custom interface start -----------------------
    platformCreate();
    //kernel msg
    XbhKernelMsgManager::getInstance();
    //front board init
    XbhFrontBoard::getInstance();
    //HiDBOFact::GetDBO()->RestoreFac();

    //light sensor
    /**
    光感会读取两组IIC，如果其中一组上面的其他IIC器件的地址跟适配过的光感地址一样就会出现误识别的情况，
    比如MS9852 VGA转HDMI芯片的地址位0x49，跟tsl2561的地址一致，就会导致没有接tsl2561时，只要这两路IIC
    上面挂载了MS9852就有可能将光感器件识别成tsl2561。解决方案就是将XbhLightSensorManager往前移，通知在
    开机的时候先不要让其他的外挂器件工作起来
     */
    XbhLightSensorManager::getInstance();

    //input source include xbhfollowmanager & xbhhdmiswitchmanager
    XbhSourceManager::getInstance();

    // video out
    XbhAVOutManager::getInstance();

    //audio codec
    XbhAudioCodecManager::getInstance();

    //audio to i2s
    XbhAudioToI2sManager::getInstance();

    //audio amplifer
    XbhAudioAmplifierManager::getInstance();

    //usbc convert ic
    XbhUsbc2HdmiManager::getInstance();

    //net switch
    XbhNetSwitchManager::getInstance();

    //dp convert ic
    XbhDp2HdmiManager::getInstance();

    //vga convert ic
    XbhVga2HdmiManager::getInstance();

    //touch
    XbhTouchManager::getInstance();

    //audio detect
    XbhAudioDetectManager::getInstance();

    //ops process
    XbhOpsManager::getInstance();

    //mcu
    XbhMcuManager::getInstance();

    //G-Sensor
    XbhGSensorManager::getInstance();

    //NFC
    XbhNfcManager::getInstance();

    //Power sensor
    XbhPowerSensorManager::getInstance();

    //Air sensor
    XbhAirSensorManager::getInstance();

    // WatchDog
    XbhWatchDogManager::getInstance();

    // Rj45 to Usb
    XbhRj45ToUsbManager::getInstance();

    //是否使用Android原生的rtc实现
    #ifdef XBH_USE_NATIVE_RTC
        XLOGD("<=================== USE NATIVE RTC ===================>");
    #else
        XbhRtcManager::getInstance();
    #endif
    //--------------------------- custom interface end -------------------------
    XLOGI("--------------- Xbh hidl init end -----------------");
}


class XbhService::XbhServiceImpl
{
public:
    XbhServiceImpl()
    {
        XBH_U32 i = 0;
        XLOGD("XbhService()");
        for (i = 0; i < sizeof(g_astCustomerModuleLocalAtomicInfo) / sizeof(LocalAtomicInfo); i++) {
            insertInterface(g_astCustomerModuleLocalAtomicInfo[i].id, g_astCustomerModuleLocalAtomicInfo[i].pfn);
        }
        init();
    }

    void insertInterface(XBH_U32 id, LocalAtomicPfn pfn)
    {
        ModuleMapIter moduleMapIter = mDispatchMap.find(id);
        if (moduleMapIter == mDispatchMap.end())
        {
            mDispatchMap.insert(ModuleMap::value_type(id, pfn));
        }
    }

    XBH_S32 invoke(const Parcel& request, Parcel *reply)
    {
        XbhMutex::Autolock _l(mLock);
        Parcel output;
        XBH_U32 id = request.readInt32();
        if (reply == XBH_NULL)
        {
            XLOGE("invoke: reply is null!");
            return XBH_FAILURE;
        }

        ModuleMapIter moduleMapIter = mDispatchMap.find(id);
        if (moduleMapIter != mDispatchMap.end())
        {
            if (moduleMapIter->second)
            {
                reply->writeInt32(moduleMapIter->second(request, &output));
                reply->write(output.data(), output.dataSize());
                reply->setDataPosition(0);
                return XBH_SUCCESS;
            }
            XLOGE("local pfn is null:%d", id);
            return XBH_FAILURE;
        }
        XLOGE("id: 0x%04x not found!", id);
        return XBH_FAILURE;
    }

private:
    typedef typename std::map<XBH_U32, LocalAtomicPfn> ModuleMap;
    typedef typename ModuleMap::iterator ModuleMapIter;
    ModuleMap mDispatchMap;

private:
    static XbhMutex mLock;
};

XbhMutex XbhService::XbhServiceImpl::mLock;

XbhService::XbhService()
{
    pXbhServiceImpl = new XbhServiceImpl;
}

XbhService::~XbhService()
{
    if (pXbhServiceImpl != XBH_NULL)
    {
        delete pXbhServiceImpl;
        pXbhServiceImpl = XBH_NULL;
    }
}

XBH_S32 XbhService::invoke(const Parcel& request, Parcel *reply)
{
    return pXbhServiceImpl->invoke(request, reply);
}

int XbhService::CbHwNotify(XBH_U32 &type, hidl_string &inputStr, hidl_string &inputStrReq, android::Parcel *notifyReply) const
{
    hardware::Parcel hwreply;

    for (int i = 0; i < (int)mCallback.size(); i++) {
        String8 retReply;
        auto cb = [&](hidl_string strReply) {
            retReply = String8(strReply.c_str(), strReply.size());
        };
        sp<IXbhHwmwCallback> c = mCallback[i];
        if (c.get() == XBH_NULL) {
            continue;
        }

        XLOGD("INFO: mCallback->hwNotify(type, inputStr); client:%d", i);

        Return<void> ret = c->hwNotify(type, inputStr, inputStrReq, cb);
        if (ret.isDeadObject()) {
            XLOGE("Error, hwNotify failed, client:%d is a DEAD_OBJECT", i);
        }

        if (notifyReply != XBH_NULL) {
            hwreply.setDataSize(retReply.size());
            hwreply.setDataPosition(0);
            hwreply.write(retReply.c_str(), retReply.size());
            hwreply.setDataPosition(0);
            notifyReply->write(hwreply.data(), hwreply.dataSize());
            notifyReply->setDataPosition(0);
        }
    }

    return XBH_SUCCESS;
}

int XbhService::notify(XBH_U32 type, const XBH_VOID *data, XBH_U32 len,
    const android::Parcel *request, android::Parcel *notifyReply) const
{
    XbhMutex::Autolock _l(mNotifyLock);
    hardware::Parcel pParcelObj;
    pParcelObj.write(data, len);

    hidl_string inputStr((const char *)pParcelObj.data(), pParcelObj.dataSize());
    hardware::Parcel hwrequest;

    hidl_string inputStrReq = XBH_NULL;
    if (request != XBH_NULL && request->dataSize() > 0) {
        hwrequest.setDataSize(request->dataSize());
        hwrequest.setDataPosition(0);
        hwrequest.write(request->data(), request->dataSize());
        request->setDataPosition(0);
        if (0 < hwrequest.dataSize()) {
            hidl_string inputStrTmp((const char *)hwrequest.data(), hwrequest.dataSize());
            inputStrReq = inputStrTmp;
        }
    }

    CbHwNotify(type, inputStr, inputStrReq, notifyReply);

    XLOGD("INFO: notify enter type=0x%X, clientsCnt:%d", type, mCallback.size());

    return 0;
}

int XbhService::setCallback(const sp<IXbhHwmwCallback> &callback)
{
    mCallback.add(callback);
    return 0;
}

int XbhService::disconnect(const sp<IXbhHwmwCallback> &callback)
{
    SortedVector<sp<IXbhHwmwCallback> >::iterator itr = mCallback.begin();
    while (itr != mCallback.end()) {
        if (*itr == callback) {
            itr = mCallback.erase(itr);
        } else {
            itr++;
        }
    }
    return 0;
}

XBH_BOOL XbhService::HandleMsg(XBH_U32 msgType, const XBH_VOID *args, XBH_U32 size, const android::Parcel *request, android::Parcel *notifyReply)
{
    notify(msgType, args, size, request, notifyReply);
    return XBH_FALSE;
}

XbhModuleInterface* XbhService::getModuleInterface()
{
    return pXbhModuleApiImpl;
}

XbhPlatformInterface* XbhService::getPlatformInterface()
{
    return pXbhPlatformApiImpl;
}
