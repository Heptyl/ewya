#ifndef __XBH_AUDIO_DETECT_MANAGER_H__
#define __XBH_AUDIO_DETECT_MANAGER_H__

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMsgQueue.h"

class XbhAudioDetectManager : public XbhMWThread, public XbhMsgQueue
{
public:
    static XbhAudioDetectManager* getInstance();
    /**
     * 处理模拟MIC插拔
    */
    XBH_S32 processMicDet();
    /**
     * 处理模拟耳机/line out插拔
    */
    XBH_S32 processHpDet();
    /**
     * 处理HDMI ARC插拔
    */
    XBH_S32 processArcDet();
    /**
     * 获取模拟MIC的连接状态
    */
    XBH_S32 getMicDetect(XBH_BOOL* connect);
    /**
     * 获取耳机/lineout的连接状态
    */
    XBH_S32 getHeadphoneDetect(XBH_BOOL* connect);
    /**
     * 获取ARC的连接状态
    */
    XBH_S32 getArcDetect(XBH_BOOL* connect);
    
    ~XbhAudioDetectManager();

private:
    void run(const void* arg);
    XbhAudioDetectManager();
    static XbhAudioDetectManager *mInstance;
    static XbhMutex mLock;

    XBH_BOOL mMicConnectStatus = XBH_FALSE;
    XBH_BOOL mHpConnectStatus = XBH_FALSE;
    XBH_BOOL mArcConnectStatus = XBH_FALSE;
};

#endif //__XBH_AUDIO_DETECT_MANAGER_H__
