#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhAudioDetectManager"

#include "cutils/properties.h"

#include "XbhLog.h"
#include "XbhAudioDetectManager.h"
#include "XbhService.h"
#include "XbhEventType.h"
#include "XbhAudioCodecManager.h"
#include "XbhAudioAmplifierManager.h"

XbhAudioDetectManager*       XbhAudioDetectManager::mInstance = NULL;
XbhMutex                     XbhAudioDetectManager::mLock;

#define AUDIO_DETECT_MIC_PLUGIN          0x00
#define AUDIO_DETECT_MIC_PLUGOUT         0x01

XBH_S32 XbhAudioDetectManager::processMicDet()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL status = XBH_FALSE;
    XbhMutex::Autolock _l(mLock);
    XbhService::getModuleInterface()->getMicDetectStatus(&status);
    if(status != mMicConnectStatus)
    {
        XLOGD(" processMicDet status = %d ", status);
        mMicConnectStatus = status;
        XBH_U8 args = 0;
        if(status)
        {
            //mic plugin 先取消之前的消息
            //插入模拟MIC后延时1S再打开MIC的输入，防止插入的瞬间的爆破音
            removeMsg(AUDIO_DETECT_MIC_PLUGOUT);
            removeMsg(AUDIO_DETECT_MIC_PLUGIN);
            postMsg(AUDIO_DETECT_MIC_PLUGIN, &args, 1, 1000, this);
            //发送回调消息到中间件
            MsgPublisher().PostMsg(XBH_MIC_EVT_PLUGIN, XBH_NULL, 0);
        }
        else
        {   
            //mic plugout 先取消之前的消息
            //拔掉模拟MIC时需要关闭mic的输入
            removeMsg(AUDIO_DETECT_MIC_PLUGOUT);
            removeMsg(AUDIO_DETECT_MIC_PLUGIN);
            postMsg(AUDIO_DETECT_MIC_PLUGOUT, &args, 1, 0, this);
            //发送回调消息到中间件
            MsgPublisher().PostMsg(XBH_MIC_EVT_PLUGOUT, XBH_NULL, 0);
        }
    }
    return s32Ret;
}

XBH_S32 XbhAudioDetectManager::processHpDet()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL status = XBH_FALSE;
    XBH_AUDIO_OUTPUT_E enAudioOutput = XBH_AUDIO_OUTPUT_DET;
    XbhMutex::Autolock _l(mLock);
    XbhService::getModuleInterface()->getHpDetectStatus(&status);
    XbhService::getPlatformInterface()->getAudioOutput(&enAudioOutput);
    if(status != mHpConnectStatus)
    {
        XBH_BOOL spkLineoutSyncFlag = property_get_bool("persist.vendor.xbh.spk.lineout.sync", false);
        XLOGD(" processHpDet status = %d ,spkLineoutSyncFlag=%d", status,spkLineoutSyncFlag);
        mHpConnectStatus = status;
        if(status)
        {
            if (enAudioOutput == XBH_AUDIO_OUTPUT_DET)
            {
                //hp is plugin, so mute speaker
                if(spkLineoutSyncFlag){
                    XLOGD("processHpDet no need mute speaker!!!");
                }else{
                    XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
                }
                usleep(500 * 1000); //500ms
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_FALSE);
            }
            //发送回调消息到中间件
            MsgPublisher().PostMsg(XBH_HP_EVT_PLUGIN, XBH_NULL, 0);
        }
        else
        {
            if (enAudioOutput == XBH_AUDIO_OUTPUT_DET)
            {
                //hp is plugout, so unmute speaker
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
                usleep(500 * 1000); //500ms
                XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
            }
            //发送回调消息到中间件
            MsgPublisher().PostMsg(XBH_HP_EVT_PLUGOUT, XBH_NULL, 0);
        }
    }
    return s32Ret;
}

XBH_S32 XbhAudioDetectManager::processArcDet()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL status = XBH_FALSE;
    XbhMutex::Autolock _l(mLock);
    XbhService::getModuleInterface()->getArcDetectStatus(&status);
    if(status != mArcConnectStatus)
    {
        XLOGD(" processArcDet status = %d ", status);
        mArcConnectStatus = status;
        if(status)
        {
            //arc is plugin, so mute speaker
            XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
            //发送回调消息到中间件
            MsgPublisher().PostMsg(XBH_ARC_EVT_PLUGIN, XBH_NULL, 0);
        }
        else
        {
            //arc is plugout, so unmute speaker
            XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
            //发送回调消息到中间件
            MsgPublisher().PostMsg(XBH_ARC_EVT_PLUGOUT, XBH_NULL, 0);
        }
    }
    return s32Ret;
}

/**
 * 获取模拟MIC的连接状态
*/
XBH_S32 XbhAudioDetectManager::getMicDetect(XBH_BOOL* connect)
{
    XbhMutex::Autolock _l(mLock);
    *connect = mMicConnectStatus;
    return XBH_SUCCESS;
}
/**
 * 获取耳机/lineout的连接状态
*/
XBH_S32 XbhAudioDetectManager::getHeadphoneDetect(XBH_BOOL* connect)
{
    XbhMutex::Autolock _l(mLock);
    *connect = mHpConnectStatus;
    return XBH_SUCCESS;
}
/**
 * 获取ARC的连接状态
*/
XBH_S32 XbhAudioDetectManager::getArcDetect(XBH_BOOL* connect)
{
    XbhMutex::Autolock _l(mLock);
    *connect = mArcConnectStatus;
    return XBH_SUCCESS;
}


static XBH_S32 s32BootCompleted = 0;
XBH_U32 msgType;
XBH_U32 size;
XBH_U32 args;
XBH_BOOL ampInitStatus;
//codec和功放完成初始化的状态
XBH_BOOL codecInitdone;
XBH_BOOL ampInitdone;
XBH_BOOL ampUnmute = XBH_FALSE;
void XbhAudioDetectManager::run(const void* arg)
{
    if (s32BootCompleted != 1)
    {
        //判断CODEC是否初始化完成，当codec不存在时设置为codec为初始化完成
        if(XbhService::mCodecCreated && XbhAudioCodecManager::getInstance()->getChipInitDone(&codecInitdone) == XBH_FAILURE)
        {
            XLOGE("none codec !!!! ");
            codecInitdone = XBH_TRUE;
        }
        //3576E的smart项目概率性存在eywa这里解mute会比kernel的初始化快
        XbhService::getModuleInterface()->getAmpInitStatus(&ampInitStatus);
        //判断功放是否初始化完成，当功放不存在时设置为codec为初始化完成
        if(XbhService::mAmpCreated && XbhAudioAmplifierManager::getInstance()->getChipInitDone(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, &ampInitdone) == XBH_FAILURE)
        {
            XLOGE("none amp !!!! ");
            ampInitdone = XBH_TRUE;
        }
        //当功放和codec都初始化完成时，才解除speaker的mute状态
        if(codecInitdone && ampInitdone && !ampUnmute && ampInitStatus)
        {   
            
            XLOGI("all audio codec Initdone!\n");
            XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_HEADPHONE, XBH_TRUE);
            XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
            ampUnmute = XBH_TRUE;
        }
        
        s32BootCompleted = property_get_int32("sys.boot_completed", 0);
        if(s32BootCompleted != 1)
        {
            usleep(1000*1000); /* sleep 1s */
            return;
        }
    }
    processMicDet();
    processHpDet();
    processArcDet();

    if(getMsg(&msgType, &args, &size, this, XBH_FALSE))
    {
        if(msgType == AUDIO_DETECT_MIC_PLUGIN)
        {
            XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_MIC, XBH_FALSE);
        }
        else if(msgType == AUDIO_DETECT_MIC_PLUGOUT)
        {
            XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_MIC, XBH_TRUE);
        }
    }

    usleep(500 * 1000); //500ms
}

XbhAudioDetectManager::XbhAudioDetectManager()
{
    XLOGD(" begin ");
    XbhService::getModuleInterface()->initAudioMuteStatus();
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhAudioDetectManager::~XbhAudioDetectManager()
{
}

XbhAudioDetectManager* XbhAudioDetectManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhAudioDetectManager();
    }
    return mInstance;
}

