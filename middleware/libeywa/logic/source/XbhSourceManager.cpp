#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhSourceManager"

#include "XbhLog.h"
#include "XbhSourceManager.h"
#include "XbhService.h"
#include "XbhEventType.h"
#include "cutils/properties.h"
#include <hardware/board.h>
#include <string>
XbhSourceManager*      XbhSourceManager::mInstance = XBH_NULL;
XbhMutex               XbhSourceManager::mLock;
XbhMutex               XbhSourceManager::mCheckLock;

static XBH_BOOL mCurrConnectState[XBH_SOURCE_BUTT] = {0};
static XBH_BOOL mPreConnectState[XBH_SOURCE_BUTT] = {0};

static XBH_S32 mCurrSignalState = -1;
static XBH_S32 mPreSignalState = -1;

#define  DEFAULT_DELAY_COUNT             6     //default 3s
#define  DEFAULT_OPS_BOOT_DELAY_COUNT    50    //default 25s
static XBH_S32 mDelayCount = 0;
static XBH_S32 mDelayTotalCnt = DEFAULT_DELAY_COUNT;
static XBH_BOOL mCheckState = false;
static XBH_BOOL mOpsBootDelay = XBH_FALSE;

#define SND_CARDS_NODE          "/proc/asound/cards"
#define READ_BUFFER_SIZE        128

XBH_S32 XbhSourceManager::selectSource(XBH_SOURCE_E u32Source, XBH_WINDOW_NUM_E win)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD(" selectSource u32Source = %d mCurrentSrc = %d start !!!!! ", u32Source, mCurrentSrc);
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.all_src_low_latency", type, "false");
    XLOGD("persist.vendor.xbh.all_src_low_latency : %s \n", type);

    if(u32Source == XBH_SOURCE_ANDROID)
    {
        XbhService::getModuleInterface()->setHdmiLowLatency(XBH_FALSE);
        mCurrentSrc = u32Source;
        mCurrSignalState = -1;
        mPreSignalState = -1;
        //set follow port
        m_pXbhFollowManager->setFollowPort(mCurrentSrc);
        m_pXbhHdmiSwitchManager->setActivePort(mCurrentSrc);
    }
    else if(u32Source != mCurrentSrc)
    {
        //部分SOC，比如RK3576，切换信源时需要先设置628驱动的5Vdet标志为0
        if(XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_FALSE) == XBH_SUCCESS)
        {
            usleep(200 * 1000);
        }
        XbhService::getModuleInterface()->setHdmiLowLatency(XBH_FALSE);
        //RK3588，使用5K OPS时切换，需要将其中的一个OPS信号的HPD拉低，防止WINDOWS识别到两个信号
        XbhService::getModuleInterface()->setMipiRxCsiControlState(u32Source);
        mCurrentSrc = u32Source;
        mCurrSignalState = -1;
        mPreSignalState = -1;
        //set follow port
        m_pXbhFollowManager->setFollowPort(mCurrentSrc);
        m_pXbhHdmiSwitchManager->setActivePort(mCurrentSrc);
        //RK3576动态加载低延时减少拔出花屏可能性
        if(strcmp(type, "true") == 0 )
        {
            XbhService::getModuleInterface()->setHdmiLowLatency(XBH_TRUE);
        }
        else if(u32Source == XBH_SOURCE_OPS1 || u32Source == XBH_SOURCE_OPS2)
        {
            XbhService::getModuleInterface()->setHdmiLowLatency(XBH_TRUE);
        }
        else
        {
            XbhService::getModuleInterface()->setHdmiLowLatency(XBH_FALSE);
        }
        //usleep(200 * 1000);
        //XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_TRUE);
        XbhService::getModuleInterface()->setTypecReset(u32Source);
    }
    XLOGD(" u32Source = %d end !!!!! ", u32Source);
    return s32Ret;
}

XBH_S32 XbhSourceManager::setSourceSwitchPort(XBH_SOURCE_E u32Source)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("setSourceSwitchPort  u32Source = %d mCurrentSrc = %d start !!!!! ", u32Source, mCurrentSrc);
    char type[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.all_src_low_latency", type, "false");
    XLOGD("persist.vendor.xbh.all_src_low_latency : %s \n", type);

    if(u32Source == XBH_SOURCE_ANDROID)
    {
        XbhService::getModuleInterface()->setHdmiLowLatency(XBH_FALSE);
        mCurrentSrc = u32Source;
        mCurrSignalState = -1;
        mPreSignalState = -1;
        //set follow port
        m_pXbhFollowManager->setFollowPort(mCurrentSrc);
        m_pXbhHdmiSwitchManager->setActivePort(mCurrentSrc);
    }
    else if(u32Source != mCurrentSrc)
    {
        //部分SOC，比如RK3576，切换信源时需要先设置628驱动的5Vdet标志为0
        if(XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_FALSE) == XBH_SUCCESS)
        {
            usleep(200 * 1000);
        }
        XbhService::getModuleInterface()->setHdmiLowLatency(XBH_FALSE);
        //RK3588，使用5K OPS时切换，需要将其中的一个OPS信号的HPD拉低，防止WINDOWS识别到两个信号
        XbhService::getModuleInterface()->setMipiRxCsiControlState(u32Source);
        mCurrentSrc = u32Source;
        mCurrSignalState = -1;
        mPreSignalState = -1;
        m_pXbhHdmiSwitchManager->setActivePort(mCurrentSrc);
        //RK3576动态加载低延时减少拔出花屏可能性
        if(strcmp(type, "true") == 0 )
        {
            XbhService::getModuleInterface()->setHdmiLowLatency(XBH_TRUE);
        }
        else if(u32Source == XBH_SOURCE_OPS1 || u32Source == XBH_SOURCE_OPS2)
        {
            XbhService::getModuleInterface()->setHdmiLowLatency(XBH_TRUE);
        }
        else
        {
            XbhService::getModuleInterface()->setHdmiLowLatency(XBH_FALSE);
        }
        //usleep(200 * 1000);
        //XbhService::getModuleInterface()->setHdmiRx5vDet(XBH_TRUE);
        XbhService::getModuleInterface()->setTypecReset(u32Source);
    }
    XLOGD(" u32Source = %d end !!!!! ", u32Source);
    return s32Ret;
}

XBH_S32 XbhSourceManager::setSourceUsbPort(XBH_SOURCE_E u32Source)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    m_pXbhFollowManager->setFollowPort(u32Source);
    return s32Ret;
}

XBH_S32 XbhSourceManager::getCurrSource(XBH_SOURCE_E *u32Source)
{
    *u32Source = mCurrentSrc;
    return XBH_SUCCESS;
}

XBH_S32 XbhSourceManager::getSourceDet(XBH_SOURCE_E u32Source, XBH_BOOL *bEnable)
{
    switch (u32Source)
    {
        case XBH_SOURCE_HDMI1:
            XbhService::getModuleInterface()->getHdmi1Det(bEnable);
            break;
        case XBH_SOURCE_HDMI2:
            XbhService::getModuleInterface()->getHdmi2Det(bEnable);
            break;
        case XBH_SOURCE_HDMI3:
            XbhService::getModuleInterface()->getHdmi3Det(bEnable);
            break;
        case XBH_SOURCE_HDMI4:
            XbhService::getModuleInterface()->getHdmi4Det(bEnable);
            break;
        case XBH_SOURCE_HDMI5:
            XbhService::getModuleInterface()->getHdmi5Det(bEnable);
            break;
        case XBH_SOURCE_HDMI6:
            XbhService::getModuleInterface()->getHdmi6Det(bEnable);
            break;
        case XBH_SOURCE_F_HDMI1:
            XbhService::getModuleInterface()->getFHdmi1Det(bEnable);
            break;
        case XBH_SOURCE_F_HDMI2:
            XbhService::getModuleInterface()->getFHdmi2Det(bEnable);
            break;
        case XBH_SOURCE_F_HDMI3:
            XbhService::getModuleInterface()->getFHdmi3Det(bEnable);
            break;
        case XBH_SOURCE_F_HDMI4:
            XbhService::getModuleInterface()->getFHdmi4Det(bEnable);
            break;
        case XBH_SOURCE_USBC1:
            XbhService::getModuleInterface()->getUsbc1Det(bEnable);
            break;
        case XBH_SOURCE_USBC2:
            XbhService::getModuleInterface()->getUsbc2Det(bEnable);
            break;
        case XBH_SOURCE_USBC3:
            XbhService::getModuleInterface()->getUsbc3Det(bEnable);
            break;
        case XBH_SOURCE_USBC4:
            XbhService::getModuleInterface()->getUsbc4Det(bEnable);
            break;
        case XBH_SOURCE_F_USBC1:
            XbhService::getModuleInterface()->getFUsbc1Det(bEnable);
            break;
        case XBH_SOURCE_F_USBC2:
            XbhService::getModuleInterface()->getFUsbc2Det(bEnable);
            break;
        case XBH_SOURCE_F_USBC3:
            XbhService::getModuleInterface()->getFUsbc3Det(bEnable);
            break;
        case XBH_SOURCE_F_USBC4:
            XbhService::getModuleInterface()->getFUsbc4Det(bEnable);
            break;
        case XBH_SOURCE_DP1:
            XbhService::getModuleInterface()->getDP1Det(bEnable);
            break;
        case XBH_SOURCE_DP2:
            XbhService::getModuleInterface()->getDP2Det(bEnable);
            break;
        case XBH_SOURCE_DP3:
            XbhService::getModuleInterface()->getDP3Det(bEnable);
            break;
        case XBH_SOURCE_DP4:
            XbhService::getModuleInterface()->getDP4Det(bEnable);
            break;
        case XBH_SOURCE_F_DP1:
            XbhService::getModuleInterface()->getFDP1Det(bEnable);
            break;
        case XBH_SOURCE_F_DP2:
            XbhService::getModuleInterface()->getFDP1Det(bEnable);
            break;
        case XBH_SOURCE_F_DP3:
            XbhService::getModuleInterface()->getFDP1Det(bEnable);
            break;
        case XBH_SOURCE_F_DP4:
            XbhService::getModuleInterface()->getFDP1Det(bEnable);
            break;
        case XBH_SOURCE_OPS1:
            XbhService::getModuleInterface()->getOps1Det(bEnable);
            break;
        case XBH_SOURCE_OPS2:
            XbhService::getModuleInterface()->getOps2Det(bEnable);
            break;
        case XBH_SOURCE_SDM1:
            XbhService::getModuleInterface()->getSdm1Det(bEnable);
            break;
        case XBH_SOURCE_SDM2:
            XbhService::getModuleInterface()->getSdm2Det(bEnable);
            break;
        case XBH_SOURCE_VGA1:
            XbhService::getModuleInterface()->getVga1Det(bEnable);
            break;
        case XBH_SOURCE_VGA2:
            XbhService::getModuleInterface()->getVga2Det(bEnable);
            break;
        case XBH_SOURCE_CVBS1:
            XbhService::getModuleInterface()->getCvbs1Det(bEnable);
            break;
        case XBH_SOURCE_CVBS2:
            XbhService::getModuleInterface()->getCvbs2Det(bEnable);
            break;
        case XBH_SOURCE_OUT1:
            XbhService::getModuleInterface()->getOut1Det(bEnable);
            break;
        case XBH_SOURCE_OUT2:
            XbhService::getModuleInterface()->getOut2Det(bEnable);
            break;        
        default:
            break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhSourceManager::getSourceSignalStatus(XBH_SOURCE_E u32Source, XBH_S32 *status)
{
    switch (u32Source)
    {
        case XBH_SOURCE_HDMI1:
            XbhService::getModuleInterface()->getHdmi1SignalStatus(status);
            break;
        case XBH_SOURCE_HDMI2:
            XbhService::getModuleInterface()->getHdmi2SignalStatus(status);
            break;
        case XBH_SOURCE_HDMI3:
            XbhService::getModuleInterface()->getHdmi3SignalStatus(status);
            break;
        case XBH_SOURCE_HDMI4:
            XbhService::getModuleInterface()->getHdmi4SignalStatus(status);
            break;
        case XBH_SOURCE_HDMI5:
            XbhService::getModuleInterface()->getHdmi5SignalStatus(status);
            break;
        case XBH_SOURCE_HDMI6:
            XbhService::getModuleInterface()->getHdmi6SignalStatus(status);
            break;
        case XBH_SOURCE_F_HDMI1:
            XbhService::getModuleInterface()->getFHdmi1SignalStatus(status);
            break;
        case XBH_SOURCE_F_HDMI2:
            XbhService::getModuleInterface()->getFHdmi2SignalStatus(status);
            break;
        case XBH_SOURCE_F_HDMI3:
            XbhService::getModuleInterface()->getFHdmi3SignalStatus(status);
            break;
        case XBH_SOURCE_F_HDMI4:
            XbhService::getModuleInterface()->getFHdmi4SignalStatus(status);
            break;
        case XBH_SOURCE_USBC1:
            XbhService::getModuleInterface()->getUsbc1SignalStatus(status);
            break;
        case XBH_SOURCE_USBC2:
            XbhService::getModuleInterface()->getUsbc2SignalStatus(status);
            break;
        case XBH_SOURCE_USBC3:
            XbhService::getModuleInterface()->getUsbc3SignalStatus(status);
            break;
        case XBH_SOURCE_USBC4:
            XbhService::getModuleInterface()->getUsbc4SignalStatus(status);
            break;
        case XBH_SOURCE_F_USBC1:
            XbhService::getModuleInterface()->getFUsbc1SignalStatus(status);
            break;
        case XBH_SOURCE_F_USBC2:
            XbhService::getModuleInterface()->getFUsbc2SignalStatus(status);
            break;
        case XBH_SOURCE_F_USBC3:
            XbhService::getModuleInterface()->getFUsbc3SignalStatus(status);
            break;
        case XBH_SOURCE_F_USBC4:
            XbhService::getModuleInterface()->getFUsbc4SignalStatus(status);
            break;
        case XBH_SOURCE_DP1:
            XbhService::getModuleInterface()->getDP1SignalStatus(status);
            break;
        case XBH_SOURCE_DP2:
            XbhService::getModuleInterface()->getDP2SignalStatus(status);
            break;
        case XBH_SOURCE_DP3:
            XbhService::getModuleInterface()->getDP3SignalStatus(status);
            break;
        case XBH_SOURCE_DP4:
            XbhService::getModuleInterface()->getDP4SignalStatus(status);
            break;
        case XBH_SOURCE_F_DP1:
            XbhService::getModuleInterface()->getFDP1SignalStatus(status);
            break;
        case XBH_SOURCE_F_DP2:
            XbhService::getModuleInterface()->getFDP1SignalStatus(status);
            break;
        case XBH_SOURCE_F_DP3:
            XbhService::getModuleInterface()->getFDP1SignalStatus(status);
            break;
        case XBH_SOURCE_F_DP4:
            XbhService::getModuleInterface()->getFDP1SignalStatus(status);
            break;
        case XBH_SOURCE_OPS1:
            XbhService::getModuleInterface()->getOps1SignalStatus(status);
            break;
        case XBH_SOURCE_OPS2:
            XbhService::getModuleInterface()->getOps2SignalStatus(status);
            break;
        case XBH_SOURCE_SDM1:
            XbhService::getModuleInterface()->getSdm1SignalStatus(status);
            break;
        case XBH_SOURCE_SDM2:
            XbhService::getModuleInterface()->getSdm2SignalStatus(status);
            break;
        case XBH_SOURCE_VGA1:
            XbhService::getModuleInterface()->getVga1SignalStatus(status);
            break;
        case XBH_SOURCE_VGA2:
            XbhService::getModuleInterface()->getVga2SignalStatus(status);
            break;
        case XBH_SOURCE_CVBS1:
            XbhService::getModuleInterface()->getCvbs1SignalStatus(status);
            break;
        case XBH_SOURCE_CVBS2:
            XbhService::getModuleInterface()->getCvbs2SignalStatus(status);
            break;

        default:
            break;
    }
    return XBH_SUCCESS;
}


static XBH_BOOL gFirstRun = XBH_TRUE;
void XbhSourceManager::run(const void* arg)
{
    if(gFirstRun)
    {
        XbhService::getModuleInterface()->initSrcDetGpioIrq();
        gFirstRun  = XBH_FALSE;
    }
    if(XbhService::getModuleInterface()->mIsBootCompleted == 0)
    {
        XbhService::getModuleInterface()->mIsBootCompleted = property_get_int32("sys.boot_completed", 0);
    }
    if(XbhService::getModuleInterface()->mIsMiddlewareCompleted == 0)
    {
        XbhService::getModuleInterface()->mIsMiddlewareCompleted = property_get_int32("vendor.xbh.middleware.init", 0);
    }
    //处理信号源插拔事件
    processSrcConnectStatus();
    //处理OPS的信号状态
    processOpsSignalStatus(mCurrentSrc);
    //处理信号源信号状态变化事件
#ifdef XBH_USE_HAL_API_TO_CHECK_SIGNAL_STATUS
    processSrcSignalStatus();
#endif
    //处理信源下type-b热拔插事件
    XbhService::getModuleInterface()->ProcessTypeBHotplug(mCurrentSrc);
    //处理type-c信源下type-c热拔插事件
    XbhService::getModuleInterface()->ProcessTypeCHotplug(mCurrentSrc);
    usleep(500 * 1000); //500ms
}

XBH_S32 XbhSourceManager::processSrcConnectStatus()
{
    XBH_BOOL bConnectStatus;
    XBH_S32 SRCId;
    XBH_SOURCE_E currSource;
    XbhSourceManager::getInstance()->getCurrSource(&currSource);
    for(int i = 0; i < XBH_SOURCE_BUTT; i ++)
    {
        switch (i)
        {
            case XBH_SOURCE_HDMI1:
            case XBH_SOURCE_HDMI2:
            case XBH_SOURCE_HDMI3:
            case XBH_SOURCE_HDMI4:
            case XBH_SOURCE_HDMI5:
            case XBH_SOURCE_HDMI6:
            case XBH_SOURCE_F_HDMI1:
            case XBH_SOURCE_F_HDMI2:
            case XBH_SOURCE_F_HDMI3:
            case XBH_SOURCE_F_HDMI4:
            case XBH_SOURCE_USBC1:
            case XBH_SOURCE_USBC2:
            case XBH_SOURCE_USBC3:
            case XBH_SOURCE_USBC4:
            case XBH_SOURCE_F_USBC1:
            case XBH_SOURCE_F_USBC2:
            case XBH_SOURCE_F_USBC3:
            case XBH_SOURCE_F_USBC4:
            case XBH_SOURCE_DP1:
            case XBH_SOURCE_DP2:
            case XBH_SOURCE_DP3:
            case XBH_SOURCE_DP4:
            case XBH_SOURCE_F_DP1:
            case XBH_SOURCE_F_DP2:
            case XBH_SOURCE_F_DP3:
            case XBH_SOURCE_F_DP4:
            case XBH_SOURCE_VGA1:
            case XBH_SOURCE_VGA2:
            case XBH_SOURCE_CVBS1:
            case XBH_SOURCE_CVBS2:
            case XBH_SOURCE_OUT1:
            case XBH_SOURCE_OUT2:
                bConnectStatus = XBH_FALSE;
                //判断信源是否被支持，只有被支持的信源才需要去获取连接状态
                XbhService::getModuleInterface()->getSrcExsit((XBH_SOURCE_E)i, &bConnectStatus);
                if(bConnectStatus == XBH_TRUE)
                {
                    //判断信源的连接状态
                    SRCId = i;
                    bConnectStatus = XBH_FALSE;
                    getSourceDet((XBH_SOURCE_E)i, &bConnectStatus);
                    if (bConnectStatus == XBH_TRUE)
                    {
                        mCurrConnectState[i] = XBH_TRUE;
                        if(mCurrConnectState[i] != mPreConnectState[i])
                        {
                            mPreConnectState[i] = XBH_TRUE;
                            XLOGD(" %d cable is connect !! ", SRCId);
                            if(currSource == i && i == XBH_SOURCE_DP1)
                            {
                                XbhService::getModuleInterface()->setHdmiRx5vDet(1);
                            }
                            MsgPublisher().PostMsg(XBH_SRC_EVT_PLUGIN, &SRCId, sizeof(SRCId));
                        }
                    }
                    else
                    {
                        mCurrConnectState[i] = XBH_FALSE;
                        if(mCurrConnectState[i] != mPreConnectState[i])
                        {
                            mPreConnectState[i] = XBH_FALSE;
                            XLOGD(" %d cable is disconnect !! ", SRCId);
                            if(currSource == i && i == XBH_SOURCE_DP1)
                            {
                                XbhService::getModuleInterface()->setHdmiRx5vDet(0);
                            }
                            MsgPublisher().PostMsg(XBH_SRC_EVT_PLUGOUT, &SRCId, sizeof(SRCId));
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhSourceManager::processSrcSignalStatus()
{
    XBH_S32 signalStatus = -1;
    XBH_S32 mic_card_id;
    XBH_S32 uac_card_id;
    XBH_BOOL power_status = XBH_FALSE;
    char mic_card_str[PROPERTY_VALUE_MAX];
    char uac_card_str[PROPERTY_VALUE_MAX];

    getSourceSignalStatus(mCurrentSrc, &signalStatus);
    //通过这里信源切换去切音频算法的通道
    if(mCurrentSrc == XBH_SOURCE_ANDROID && property_get_bool("persist.vendor.xbh.pdm_enable", false))
    {   
        if (property_get_bool("persist.vendor.xbh.usb.uac_otg_enable", false))
        {
            property_set("persist.vendor.xbh.usb.uac_otg_enable", "false");
            property_set("persist.vendor.xbh.pdm_type", "0");
        }
    }
    if (mCurrentSrc == XBH_SOURCE_OPS1 || mCurrentSrc == XBH_SOURCE_OPS2) //只针对ops关机的时候
    {
        XbhService::getPlatformInterface()->getOpsPowerStatus(&power_status, mCurrentSrc);
        if (power_status == XBH_FALSE)
        {    
            if (property_get_bool("persist.vendor.xbh.usb.uac_otg_enable", false))
            {   
                XLOGD("ops is power down, signalStatus = %d\n", signalStatus);
                property_set("persist.vendor.xbh.usb.uac_otg_enable", "false");
            }
        }
    }

    if(signalStatus != -1)
    {
        mCurrSignalState = signalStatus;
        if(mCurrSignalState != mPreSignalState)
        {
            mPreSignalState = signalStatus;
            XLOGD("signal is change, status = %d ", signalStatus);
            if (mCurrentSrc == XBH_SOURCE_OPS1 || mCurrentSrc == XBH_SOURCE_OPS2)
            {
                XbhService::getPlatformInterface()->getOpsPowerStatus(&power_status, mCurrentSrc);
                if (power_status == XBH_FALSE)
                    mOpsBootDelay = XBH_TRUE;
            }
            //通过这里信源切换去切音频算法的通道
            if(property_get_bool("persist.vendor.xbh.pdm_enable", false))
            {
                if (signalStatus == 0)
                {   
                    if (access("/vendor/bin/uac_app", F_OK) == 0)
                    {   
#ifdef XBH_BOARD_MIC_SOUND_CARD_NAME
                        //动态获取lango和uac声卡id并设置到属性中
                        uac_card_id = getCardIndexByName(XBH_BOARD_UAC_SOUND_CARD_NAME);
                        mic_card_id = getCardIndexByName(XBH_BOARD_MIC_SOUND_CARD_NAME);
                        if (mic_card_id < 0)
                        {
                            mic_card_id = XBH_BOARD_MIC_SND_CARD_ID_DEFAULT;
                        }
                        if (uac_card_id < 0)
                        {
                            uac_card_id = XBH_BOARD_UAC_SND_CARD_ID_DEFAULT;
                        }

                        snprintf(uac_card_str, sizeof(uac_card_str), "%d", uac_card_id);
                        snprintf(mic_card_str, sizeof(mic_card_str), "%d", mic_card_id);
                        property_set("persist.vendor.xbh.uac_card_id", uac_card_str);
                        property_set("persist.vendor.xbh.mic_card_id", mic_card_str);
                        XLOGD("mic card id:%d,uac card id:%d\n", mic_card_id,uac_card_id);
#endif
                        XLOGD("pdm mic changed otg mCurrentSrc = %d ", mCurrentSrc);
                        //property_set("persist.vendor.xbh.pdm_type", "1");
                        //usleep(30*1000);
                        property_set("persist.vendor.xbh.usb.uac_android_enable", "false");
                        //property_set("persist.vendor.xbh.usb.uac_otg_enable", "true");
                        switch(mCurrentSrc) //只针对typeC 插拔才会处理reset uac_app.
                        {
                            case XBH_SOURCE_USBC1:
                            case XBH_SOURCE_USBC2:
                            case XBH_SOURCE_USBC3:
                            case XBH_SOURCE_USBC4:
                            case XBH_SOURCE_F_USBC1:
                            case XBH_SOURCE_F_USBC2:
                            case XBH_SOURCE_F_USBC3:
                            case XBH_SOURCE_F_USBC4:
                                setFollowPort(mCurrentSrc);
                                break;
                            default:  // 针对typeB 检测插拔处理是否reset uac_app.
                                XLOGI("need not to reset uac_app");
                                break;
                        }
                    }
                }
            }
            MsgPublisher().PostMsg(XBH_SRC_EVT_SIGNALCHANGE, &signalStatus, sizeof(signalStatus));
        }
        {
            XbhMutex::Autolock lock(mCheckLock);
            if (mCheckState) // 使能延迟延迟启动
            {
                if (mDelayCount++ >= mDelayTotalCnt)// 每500ms循环一次，循环次数
                {   
                    XLOGD("mic checkout finished");
                    property_set("persist.vendor.xbh.usb.uac_android_enable", "false");
                    property_set("persist.vendor.xbh.pdm_type", "1");
                    property_set("persist.vendor.xbh.usb.uac_otg_enable", "true");
                    mDelayCount = 0;
                    mCheckState = false;
                }
            }
        }
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhSourceManager::getCardIndexByName(char *name)
{
    FILE *mCardFile = NULL;
    XBH_S32 mCardIndex = -1;
    if (!name)
    {
        return -1;
    }
    mCardFile = fopen(SND_CARDS_NODE, "r");
    if (mCardFile)
    {
        char tempbuffer[READ_BUFFER_SIZE];
        while (!feof(mCardFile))
        {
            fgets(tempbuffer, READ_BUFFER_SIZE, mCardFile);
            /* this line contain '[' character */
            if (strchr(tempbuffer, '['))
            {
                char *Rch = strtok(tempbuffer, "[");
                XBH_S32 id = atoi(Rch);
                ALOGD("\tcurrent card id = %d, Rch = %s", id, Rch);
                Rch = strtok(NULL, " ]");
                ALOGD("\tcurrent sound card name = %s", Rch);
                if (strcmp(Rch, name) == 0)
                {
                    ALOGD("\t sound cardIndex found = %d", id);
                    mCardIndex = id;
                    break;
                }
            }
            memset(tempbuffer, 0, READ_BUFFER_SIZE);
        }
        fclose(mCardFile);
    }
    return mCardIndex;
}

XBH_S32 XbhSourceManager::pushSourcePlugStatus(XBH_SOURCE_E u32Source, XBH_BOOL status)
{
    XBH_S32 SRCId;
    SRCId = (XBH_S32)u32Source;
    if(status)
    {
        XLOGD(" %d cable is connect !! ", SRCId);
        MsgPublisher().PostMsg(XBH_SRC_EVT_PLUGIN, &SRCId, sizeof(SRCId));
    }
    else
    {
        XLOGD(" %d cable is disconnect !! ", SRCId);
        MsgPublisher().PostMsg(XBH_SRC_EVT_PLUGOUT, &SRCId, sizeof(SRCId));
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhSourceManager::processOpsSignalStatus(XBH_SOURCE_E u32Source)
{
    XbhService::getModuleInterface()->processOpsSignalStatus(u32Source);
    return XBH_SUCCESS;
}

XBH_S32 XbhSourceManager::setFollowPort(XBH_SOURCE_E u32Source)
{
    if (property_get_bool("persist.vendor.xbh.pdm_enable", false))
    {
        XbhMutex::Autolock lock(mCheckLock);
        property_set("persist.vendor.xbh.usb.uac_otg_enable", "false");
        mDelayCount = 0;
        switch(u32Source)
        {
            case XBH_SOURCE_ANDROID:
                mCheckState = false;
                property_set("persist.vendor.xbh.pdm_type", "0");
                break;
            case XBH_SOURCE_OPS1:
            case XBH_SOURCE_OPS2:
                mCheckState = true;
                mDelayTotalCnt = property_get_int32("persist.vendor.xbh.DelayTotalCnt", DEFAULT_DELAY_COUNT);
                if (mOpsBootDelay == XBH_TRUE)
                {
                    mDelayTotalCnt = DEFAULT_OPS_BOOT_DELAY_COUNT;
                    mOpsBootDelay = XBH_FALSE;
                    XLOGD("OPS boot delay %d", mDelayTotalCnt);
                }
                break;
            default:
                mCheckState = true;
                mDelayTotalCnt = DEFAULT_DELAY_COUNT; //默认延迟3s.
                break;
        }
    }
    return XBH_SUCCESS;
}

XbhSourceManager::XbhSourceManager()
{
    XLOGD(" begin ");
    mCurrentSrc = XBH_SOURCE_ANDROID;
    m_pXbhHdmiSwitchManager = XBH_NULL;
    m_pXbhFollowManager = XBH_NULL;

    m_pXbhHdmiSwitchManager = XbhHdmiSwitchManager::getInstance();
    m_pXbhFollowManager = XbhFollowManager::getInstance();
    
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD(" end ");
}

XbhSourceManager::~XbhSourceManager()
{
    if(m_pXbhHdmiSwitchManager != XBH_NULL)
    {
        delete m_pXbhHdmiSwitchManager;
    }
    if(m_pXbhFollowManager != XBH_NULL)
    {
        delete m_pXbhFollowManager;
    }
    m_pXbhHdmiSwitchManager = XBH_NULL;
    m_pXbhFollowManager = XBH_NULL;
}

XbhSourceManager* XbhSourceManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhSourceManager();
    }
    return mInstance;
}

