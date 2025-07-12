#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhOpsManager"

#include "cutils/properties.h"
#include <stdlib.h>
#include "XbhLog.h"
#include "XbhOpsManager.h"
#include "XbhEventType.h"
#include "XbhService.h"
#include "XbhUsbc2HdmiManager.h"
#include "XbhHdmiSwitchManager.h"
#include "XbhPdIcManager.h"
XbhOpsManager*       XbhOpsManager::mInstance = NULL;
XbhMutex             XbhOpsManager::mLock;

static XBH_BOOL mCurrPowerState[XBH_SOURCE_BUTT] = {0};
static XBH_BOOL mPrePowerState[XBH_SOURCE_BUTT] = {0};

static XBH_S32 s32OpsStartCompleted = 0;
void XbhOpsManager::run(const void* arg)
{
    usleep(300 * 1000); //300ms 分离是为了先让TypeC芯片启动，后检测OPS插入状态，以实现根据OPS插入状态，动态调整PD充电功率
    //动态调整功率
    XBH_BOOL enable = property_get_bool("persist.vendor.xbh.dynadjpow", false);
    if(enable)
    {
        XBH_BOOL powerenable = XBH_FALSE;
        XbhHdmiSwitchManager::getInstance()->getChipInitDone(&powerenable);
        if(powerenable)
        {
            dynamicallyAdjustPower();
        }
    }
    XBH_BOOL enable_rst5450 = property_get_bool("persist.vendor.xbh.dynadjpow_rst5450", false);
    if(enable_rst5450)
    {
        XBH_BOOL powerenable_rst5450 = XBH_FALSE;
        XbhPdIcManager::getInstance()->getChipInitDone(&powerenable_rst5450);
        if(powerenable_rst5450)
        {
            dynamicallyAdjustPower();
        }
    }

    //ops模块插拔
    processOpsPlug();
    //ops开关机事件上报
    processOpsStatusReport();
    //处理OPS开机启动逻辑
    if(s32OpsStartCompleted == 0)
    {
        processOpsStart();
    }
    usleep(200 * 1000); //200ms
}

/**
 * 动态调整功率
*/
void XbhOpsManager::dynamicallyAdjustPower()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_PDCAPACITY_POWER pd,fpd;
    XBH_BOOL status = XBH_FALSE;
    XBH_CHAR based_cmode[PROPERTY_VALUE_MAX] = {0};

    property_get("persist.vendor.xbh.ops_based_mode", based_cmode, "0");
    XBH_U32 based_mode = atoi(based_cmode);

    //判断改变功率的条件是拔插OPS还是开关OPS
    if(based_mode == XBH_BASED_ON_OPS_PLUG)
    {
        XbhService::getPlatformInterface()->getOpsDetStatus(&status, XBH_SOURCE_E::XBH_SOURCE_OPS1);
    }
    else if(based_mode == XBH_BASED_ON_OPS_POWER)
    {
        XbhService::getPlatformInterface()->getOpsPowerStatus(&status, XBH_SOURCE_E::XBH_SOURCE_OPS1);
    }
    mCurrOps1DetStatusPower = status;
    if ((mPreOps1DetStatusPower == XBH_FALSE) && (mCurrOps1DetStatusPower == XBH_TRUE))
    {
        XBH_CHAR cmode[PROPERTY_VALUE_MAX] = {0};
        XLOGD("xbh this is OPS PLUG IN\n");
        property_get("persist.vendor.xbh.ops_plugin_usbc_charge_mode", cmode, "0");
        XBH_U32 mode = atoi(cmode);
        switch(mode)
        {
            case XBH_F_USBC_0W_USBC_0W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_0W_USBC_15W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_0W_USBC_65W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_0W_USBC_100W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_15W_USBC_0W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_15W_USBC_15W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_15W_USBC_65W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_15W_USBC_100W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_65W_USBC_0W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_65W_USBC_15W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_65W_USBC_65W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_65W_USBC_100W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_100W_USBC_0W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_100W_USBC_15W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_100W_USBC_65W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_100W_USBC_100W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_100W_USBC_96W:
                fpd = XBH_PDCAPACITY_96W;
                pd = XBH_PDCAPACITY_96W;
                break;
            default:
                XLOGD("Out of range\n");
            break;
        }
        //XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_USBC1, pd);
        //XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_F_USBC1, fpd);
        //XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_BOARD_GSV2715_1, pd);
        //XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_FRONT_GSV2712, fpd);
        XbhService::getPlatformInterface()->setSourcePdAbility(pd, XBH_SOURCE_USBC1);
        XbhService::getPlatformInterface()->setSourcePdAbility(fpd, XBH_SOURCE_F_USBC1);
        mSetCharging = XBH_FALSE;
    }else if ( (mPreOps1DetStatusPower == XBH_TRUE) && (mCurrOps1DetStatusPower == XBH_FALSE) )
    {
        XLOGD("xbh this is OPS PLUG OUT\n");
        char cmode[PROPERTY_VALUE_MAX] = {0};
        property_get("persist.vendor.xbh.ops_plugout_usbc_charge_mode", cmode, "0");
        XBH_U32 mode = atoi(cmode);
        switch(mode)
        {
            case XBH_F_USBC_0W_USBC_0W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_0W_USBC_15W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_0W_USBC_65W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_0W_USBC_100W:
                fpd = XBH_PDCAPACITY_0W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_15W_USBC_0W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_15W_USBC_15W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_15W_USBC_65W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_15W_USBC_100W:
                fpd = XBH_PDCAPACITY_15W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_65W_USBC_0W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_65W_USBC_15W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_65W_USBC_65W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_65W_USBC_100W:
                fpd = XBH_PDCAPACITY_65W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_100W_USBC_0W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_0W;
                break;
            case XBH_F_USBC_100W_USBC_15W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_15W;
                break;
            case XBH_F_USBC_100W_USBC_65W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_65W;
                break;
            case XBH_F_USBC_100W_USBC_100W:
                fpd = XBH_PDCAPACITY_100W;
                pd = XBH_PDCAPACITY_100W;
                break;
            case XBH_F_USBC_100W_USBC_96W:
                fpd = XBH_PDCAPACITY_96W;
                pd = XBH_PDCAPACITY_96W;
                break;
            default:
                XLOGD("Out of range\n");
            break;
        }
        //XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_USBC1, pd);
        //XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_F_USBC1, fpd);
        //XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_BOARD_GSV2715_1, pd);
        //XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_FRONT_GSV2712, fpd);
        XbhService::getPlatformInterface()->setSourcePdAbility(pd, XBH_SOURCE_USBC1);
        XbhService::getPlatformInterface()->setSourcePdAbility(fpd, XBH_SOURCE_F_USBC1);
        mSetCharging = XBH_FALSE;
    }else if ( (mPreOps1DetStatusPower == XBH_FALSE) && (mCurrOps1DetStatusPower == XBH_FALSE) )
    {
        if (mSetCharging)
        {
            XLOGD("xbh this is NO PLUG OPS AC ON\n");
            char cmode[PROPERTY_VALUE_MAX] = {0};
            property_get("persist.vendor.xbh.ops_no_start_usbc_charge_mode", cmode, "0");
            XBH_U32 mode = atoi(cmode);
            switch(mode)
            {
                case XBH_F_USBC_0W_USBC_0W:
                    fpd = XBH_PDCAPACITY_0W;
                    pd = XBH_PDCAPACITY_0W;
                    break;
                case XBH_F_USBC_0W_USBC_15W:
                    fpd = XBH_PDCAPACITY_0W;
                    pd = XBH_PDCAPACITY_15W;
                    break;
                case XBH_F_USBC_0W_USBC_65W:
                    fpd = XBH_PDCAPACITY_0W;
                    pd = XBH_PDCAPACITY_65W;
                    break;
                case XBH_F_USBC_0W_USBC_100W:
                    fpd = XBH_PDCAPACITY_0W;
                    pd = XBH_PDCAPACITY_100W;
                    break;
                case XBH_F_USBC_15W_USBC_0W:
                    fpd = XBH_PDCAPACITY_15W;
                    pd = XBH_PDCAPACITY_0W;
                    break;
                case XBH_F_USBC_15W_USBC_15W:
                    fpd = XBH_PDCAPACITY_15W;
                    pd = XBH_PDCAPACITY_15W;
                    break;
                case XBH_F_USBC_15W_USBC_65W:
                    fpd = XBH_PDCAPACITY_15W;
                    pd = XBH_PDCAPACITY_65W;
                    break;
                case XBH_F_USBC_15W_USBC_100W:
                    fpd = XBH_PDCAPACITY_15W;
                    pd = XBH_PDCAPACITY_100W;
                    break;
                case XBH_F_USBC_65W_USBC_0W:
                    fpd = XBH_PDCAPACITY_65W;
                    pd = XBH_PDCAPACITY_0W;
                    break;
                case XBH_F_USBC_65W_USBC_15W:
                    fpd = XBH_PDCAPACITY_65W;
                    pd = XBH_PDCAPACITY_15W;
                    break;
                case XBH_F_USBC_65W_USBC_65W:
                    fpd = XBH_PDCAPACITY_65W;
                    pd = XBH_PDCAPACITY_65W;
                    break;
                case XBH_F_USBC_65W_USBC_100W:
                    fpd = XBH_PDCAPACITY_65W;
                    pd = XBH_PDCAPACITY_100W;
                    break;
                case XBH_F_USBC_100W_USBC_0W:
                    fpd = XBH_PDCAPACITY_100W;
                    pd = XBH_PDCAPACITY_0W;
                    break;
                case XBH_F_USBC_100W_USBC_15W:
                    fpd = XBH_PDCAPACITY_100W;
                    pd = XBH_PDCAPACITY_15W;
                    break;
                case XBH_F_USBC_100W_USBC_65W:
                    fpd = XBH_PDCAPACITY_100W;
                    pd = XBH_PDCAPACITY_65W;
                    break;
                case XBH_F_USBC_100W_USBC_100W:
                    fpd = XBH_PDCAPACITY_100W;
                    pd = XBH_PDCAPACITY_100W;
                    break;
                case XBH_F_USBC_100W_USBC_96W:
                    fpd = XBH_PDCAPACITY_96W;
                    pd = XBH_PDCAPACITY_96W;
                    break;
                default:
                    XLOGD("Out of range\n");
                break;
            }
            //XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_USBC1, pd);
            //XbhUsbc2HdmiManager::getInstance()->setChargingPower(XBH_SOURCE_F_USBC1, fpd);
            //XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_BOARD_GSV2715_1, pd);
            //XbhHdmiSwitchManager::getInstance()->setChargingPower(XBH_UPDATE_FRONT_GSV2712, fpd);
            XbhService::getPlatformInterface()->setSourcePdAbility(pd, XBH_SOURCE_USBC1);
            XbhService::getPlatformInterface()->setSourcePdAbility(fpd, XBH_SOURCE_F_USBC1);
        }
        mSetCharging = XBH_FALSE;
    }
    XLOGV("xbh pre = %d, curr = %d\n", mPreOps1DetStatusPower, mCurrOps1DetStatusPower);
    mPreOps1DetStatusPower = mCurrOps1DetStatusPower;
}
/**
 * 处理OPS模块插拔
*/
XBH_S32 XbhOpsManager::processOpsPlug()
{
    XBH_BOOL status = XBH_FALSE;
    XBH_S32 SRCId;
    XbhService::getPlatformInterface()->getOpsDetStatus(&status, XBH_SOURCE_E::XBH_SOURCE_OPS1);
    if(status != mPreOps1DetStatus)
    {
        mPreOps1DetStatus = status;
        SRCId = XBH_SOURCE_E::XBH_SOURCE_OPS1;
        if(!status)
        {
            property_set("vendor.xbh.ops1crc.result", "0");
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGOUT, &SRCId, sizeof(SRCId));
            //拔掉OPS后立即断掉OPS的供电
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_FALSE, XBH_SOURCE_E::XBH_SOURCE_OPS1);
        }
        else
        {
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGIN, &SRCId, sizeof(SRCId));
            //插入OPS后是否自动给OPS供电
            bool enable = property_get_bool("persist.vendor.xbh.ops_plugin_auto_power", false);
            if(enable)
            {
                //插入OPS后延时给OPS供电，标准最低2S
                XBH_U32 time = property_get_int32("persist.vendor.xbh.ops_plugin_auto_power.time", 2000);
                usleep(time * 1000); //2s
                XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_OPS1);
            }
        }
    }
    status = XBH_FALSE;
    XbhService::getPlatformInterface()->getOpsDetStatus(&status, XBH_SOURCE_E::XBH_SOURCE_OPS2);
    if(status != mPreOps2DetStatus)
    {
        mPreOps2DetStatus = status;
        SRCId = XBH_SOURCE_E::XBH_SOURCE_OPS2;
        if(!status)
        {
            property_set("vendor.xbh.ops2crc.result", "0");
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGOUT, &SRCId, sizeof(SRCId));
            //拔掉OPS后立即断掉OPS的供电
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_FALSE, XBH_SOURCE_E::XBH_SOURCE_OPS2);
        }
        else
        {
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGIN, &SRCId, sizeof(SRCId));
            //插入OPS后是否自动给OPS供电
            bool enable = property_get_bool("persist.vendor.xbh.ops_plugin_auto_power", false);
            if(enable)
            {
                //插入OPS后延时给OPS供电，标准最低2S
                XBH_U32 time = property_get_int32("persist.vendor.xbh.ops_plugin_auto_power.time", 2000);
                usleep(time * 1000); //2s
                XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_OPS2);
            }
        }
    }
    status = XBH_FALSE;
    XbhService::getPlatformInterface()->getOpsDetStatus(&status, XBH_SOURCE_E::XBH_SOURCE_SDM1);
    if(status != mPreSdm1DetStatus)
    {
        mPreSdm1DetStatus = status;
        SRCId = XBH_SOURCE_E::XBH_SOURCE_SDM1;
        if(!status)
        {
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGOUT, &SRCId, sizeof(SRCId));
            //拔掉OPS后立即断掉OPS的供电
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_FALSE, XBH_SOURCE_E::XBH_SOURCE_SDM1);
        }
        else
        {
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGIN, &SRCId, sizeof(SRCId));
            //插入OPS后是否自动给OPS供电
            bool enable = property_get_bool("persist.vendor.xbh.ops_plugin_auto_power", false);
            if(enable)
            {
                //插入OPS后延时给OPS供电，标准最低2S
                XBH_U32 time = property_get_int32("persist.vendor.xbh.ops_plugin_auto_power.time", 2000);
                usleep(time * 1000); //2s
                XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_SDM1);
            }
        }
    }
    status = XBH_FALSE;
    XbhService::getPlatformInterface()->getOpsDetStatus(&status, XBH_SOURCE_E::XBH_SOURCE_SDM2);
    if(status != mPreSdm2DetStatus)
    {
        mPreSdm2DetStatus = status;
        SRCId = XBH_SOURCE_E::XBH_SOURCE_SDM2;
        if(!status)
        {
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGOUT, &SRCId, sizeof(SRCId));
            //拔掉OPS后立即断掉OPS的供电
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_FALSE, XBH_SOURCE_E::XBH_SOURCE_SDM2);
        }
        else
        {
            MsgPublisher().PostMsg(XBH_OPS_DEV_PLUGIN, &SRCId, sizeof(SRCId));
            //插入OPS后是否自动给OPS供电
            bool enable = property_get_bool("persist.vendor.xbh.ops_plugin_auto_power", false);
            if(enable)
            {
                //插入OPS后延时给OPS供电，标准最低2S
                XBH_U32 time = property_get_int32("persist.vendor.xbh.ops_plugin_auto_power.time", 2000);
                usleep(time * 1000); //2s
                XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_SDM2);
            }
        }
    }
    return XBH_SUCCESS;
}

/*
* todo 后续还需要针对经过switch芯片的OPS端口进行EDID是否设置完成的判断
* 如果OPS/SDM 启动时没有读取到EDID，可能不会输出画面
*/
XBH_S32 XbhOpsManager::processOpsStart()
{
    XBH_BOOL bIsDet = XBH_FALSE;
    XBH_BOOL edidIsOk = XBH_FALSE;
    XBH_BOOL power_status = XBH_FALSE;

    #if 1
    //获取OPS SINK的EDID 的准备状态，开启OPS前需要EDID准备好，否则可能出现OPS开机后无法读取EDID，导致没有信号输出
    XbhService::getModuleInterface()->getSocHdmiEdidStatus(&edidIsOk);

    if(!edidIsOk)
    {
        XLOGE("getModuleInterface()->getSocHdmiEdidStatus = %d", edidIsOk);
        return XBH_FALSE;
    }
    #endif
    //ops1 on boot start
    if(property_get_int32("persist.vendor.xbh.ops1_start_mode", 1) == 0)
    {
        //ops is detect
        XbhService::getPlatformInterface()->getOpsDetStatus(&bIsDet, XBH_SOURCE_E::XBH_SOURCE_OPS1);
        XLOGD("start ops1 on boot mode, getOpsDetStatus=%d", bIsDet);

        if(bIsDet)
        {   //set power enable for ops (19V)
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_OPS1);
            XBH_U32 time = property_get_int32("persist.vendor.xbh.ops_ps_on_delay.time", 2000);
            usleep(time * 1000);
            //读取OPS上电情况
            XbhService::getPlatformInterface()->getOpsPowerStatus(&power_status, XBH_SOURCE_E::XBH_SOURCE_OPS1);
            {
                if (power_status == XBH_FALSE)    //如果OPS没有上电再拉PS_ON开机
                {
                    //ops1 power on
                    XbhService::getPlatformInterface()->setOpsPowerCtrl(XBH_SOURCE_E::XBH_SOURCE_OPS1);
                }
            }
        }
    }

    //ops2 on boot start
    if(property_get_int32("persist.vendor.xbh.ops2_start_mode", 1) == 0)
    {
        bIsDet = XBH_FALSE;
        //ops2 is detect
        XbhService::getPlatformInterface()->getOpsDetStatus(&bIsDet, XBH_SOURCE_E::XBH_SOURCE_OPS2);
        XLOGD("start ops2 on boot mode, getOpsDetStatus=%d", bIsDet);

        if(bIsDet)
        {   //set power enable for ops (19V)
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_OPS2);
            usleep(2000*1000); /* sleep 2s */
            //读取OPS上电情况
            XbhService::getPlatformInterface()->getOpsPowerStatus(&power_status, XBH_SOURCE_E::XBH_SOURCE_OPS2);
            {
                if (power_status == XBH_FALSE)    //如果OPS没有上电再拉PS_ON开机
                {
                    //ops2 power on
                    XbhService::getPlatformInterface()->setOpsPowerCtrl(XBH_SOURCE_E::XBH_SOURCE_OPS2);
                }
            }
        }
    }

    //sdm1 on boot start
    if(property_get_int32("persist.vendor.xbh.sdm1_start_mode", 1) == 0)
    {
        bIsDet = XBH_FALSE;
        //sdm1 is detect
        XbhService::getPlatformInterface()->getOpsDetStatus(&bIsDet, XBH_SOURCE_E::XBH_SOURCE_SDM1);
        XLOGD("start sdm1 on boot mode, getOpsDetStatus=%d", bIsDet);

        if(bIsDet)
        {   //set power enable for sdm1 (19V)
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_SDM1);
            usleep(2000*1000); /* sleep 2s */
            //读取OPS上电情况
            XbhService::getPlatformInterface()->getOpsPowerStatus(&power_status, XBH_SOURCE_E::XBH_SOURCE_SDM1);
            {
                if (power_status == XBH_FALSE)    //如果OPS没有上电再拉PS_ON开机
                {
                    //sdm1 power on
                    XbhService::getPlatformInterface()->setOpsPowerCtrl(XBH_SOURCE_E::XBH_SOURCE_SDM1);
                }
            }
        }
    }

    //sdm2 on boot start
    if(property_get_int32("persist.vendor.xbh.sdm2_start_mode", 1) == 0)
    {
        bIsDet = XBH_FALSE;
        //sdm2 is detect
        XbhService::getPlatformInterface()->getOpsDetStatus(&bIsDet, XBH_SOURCE_E::XBH_SOURCE_SDM2);
        XLOGD("start sdm2 on boot mode, getOpsDetStatus=%d", bIsDet);

        if(bIsDet)
        {   //set power enable for sdm2 (19V)
            XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_TRUE, XBH_SOURCE_E::XBH_SOURCE_SDM2);
            usleep(2000*1000); /* sleep 2s */
            //读取OPS上电情况
            XbhService::getPlatformInterface()->getOpsPowerStatus(&power_status, XBH_SOURCE_E::XBH_SOURCE_SDM2);
            {
                if (power_status == XBH_FALSE)    //如果OPS没有上电再拉PS_ON开机
                {
                    //sdm2 power on
                    XbhService::getPlatformInterface()->setOpsPowerCtrl(XBH_SOURCE_E::XBH_SOURCE_SDM2);
                }
            }
        }
    }
    s32OpsStartCompleted = 1;
    return XBH_SUCCESS;
}

/**
    * 处理OPS模块开关机状态上报
*/
XBH_S32 XbhOpsManager::processOpsStatusReport()
{
    XBH_BOOL status = XBH_FALSE;
    XBH_BOOL bIsDet = XBH_FALSE;
    XBH_S32 SRCId;
    for(int i = 0; i < XBH_SOURCE_BUTT; i ++)
    {
        switch (i)
        {
            case XBH_SOURCE_OPS1:
            case XBH_SOURCE_OPS2:
            case XBH_SOURCE_SDM1:
            case XBH_SOURCE_SDM2:
                bIsDet = XBH_FALSE;
                XbhService::getPlatformInterface()->getOpsDetStatus(&bIsDet, (XBH_SOURCE_E)i);
                if(bIsDet)
                {
                    SRCId = i;
                    status = XBH_FALSE;
                    XbhService::getPlatformInterface()->getOpsPowerStatus(&status, (XBH_SOURCE_E)i);
                    if(status == XBH_TRUE)
                    {
                        mCurrPowerState[i] = XBH_TRUE;
                        if(mCurrPowerState[i] != mPrePowerState[i])
                        {
                            mPrePowerState[i] = XBH_TRUE;
                            XLOGD(" %d is power on !! ", SRCId);
                            MsgPublisher().PostMsg(XBH_OPS_DEV_POWERON, &SRCId, sizeof(SRCId));
                        }
                    }
                    else
                    {
                        mCurrPowerState[i] = XBH_FALSE;
                        if(mCurrPowerState[i] != mPrePowerState[i])
                        {
                            mPrePowerState[i] = XBH_FALSE;
                            if (i == XBH_SOURCE_OPS1)
                                property_set("vendor.xbh.ops1crc.result", "0");
                            else if (i == XBH_SOURCE_OPS2)
                                property_set("vendor.xbh.ops2crc.result", "0");
                            XLOGD(" %d is power off !! ", SRCId);
                            MsgPublisher().PostMsg(XBH_OPS_DEV_POWEROFF, &SRCId, sizeof(SRCId));
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

//此处判断上电的时候是否插OPS，如果没有插入OPS再断OPS的电
XBH_S32 XbhOpsManager::processFirstPowerOnOPS()
{
    XBH_BOOL firstStatus = XBH_FALSE;
    for(int i = 0; i < XBH_SOURCE_BUTT; i ++)
    {
        switch (i)
        {
            case XBH_SOURCE_OPS1:
            case XBH_SOURCE_OPS2:
            case XBH_SOURCE_SDM1:
            case XBH_SOURCE_SDM2:
                firstStatus = XBH_FALSE;
                XbhService::getPlatformInterface()->getOpsDetStatus(&firstStatus, (XBH_SOURCE_E)i);
                if (firstStatus == XBH_FALSE)       //如果开机没插OPS，需要断电对应通道的OPS电源
                {
                    XbhService::getPlatformInterface()->setOpsPowerEnable(XBH_FALSE, (XBH_SOURCE_E)i);
                }
                break;
            default:    //仅处理OPS源，其他的源不做处理
                break;
        }
    }
    return XBH_SUCCESS;
}

XbhOpsManager::XbhOpsManager()
{
    XLOGD(" begin ");
    //开启线程之前，先处理OPS上电逻辑：如果OPS没有插入，则需要关闭OPS的供电来确保通路正常；如果OPS插入，不需要做额外特殊处理。
    processFirstPowerOnOPS();
    XbhMWThread::run(XbhMWThread::REPEAT);
    mSetCharging = XBH_TRUE;
    XLOGD(" end ");
}

XbhOpsManager::~XbhOpsManager()
{
}

XbhOpsManager* XbhOpsManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhOpsManager();
    }
    return mInstance;
}

