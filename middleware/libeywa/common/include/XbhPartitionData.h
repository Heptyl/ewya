#ifndef XBH_PARTITION_DATA
#define XBH_PARTITION_DATA

#include "XbhLog.h"
#include <cutils/properties.h>
#include <string>
#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

class XbhPartitionData {
public:
    XbhPartitionData();
    ~XbhPartitionData();
    static XbhPartitionData* getInstance();
    XBH_S32 setDataPath(const std::string& dataPath, const std::string& paramPath);

    //Set serial number
    XBH_S32 setSn(const XBH_CHAR* strSn);
    //Get serial number
    XBH_S32 getSn(XBH_CHAR* strSn);
    //set ethernet mac address
    XBH_S32 setMacAddress(const XBH_CHAR* strMacAddress);
    //get ethernet mac address
    XBH_S32 getMacAddress(XBH_CHAR* strMacAddress);
    //set project id
    XBH_S32 setProjectId(const XBH_CHAR* strProjectId);
    //get project id
    XBH_S32 getProjectId(XBH_CHAR* strProjectId);
    //set color temp value
    XBH_S32 setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //get color temp value
    XBH_S32 getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data);
    //set device id, 广告机部分客户的需求
    XBH_S32 setDeviceId(const XBH_CHAR* strDeviceId);
    //get device id, 广告机部分客户的需求
    XBH_S32 getDeviceId(XBH_CHAR* strDeviceId);
    //Set multi panel
    XBH_S32 setMultiPanel(XBH_VOID * pBuff);
    //Get multi panel
    XBH_S32 getMultiPanel(XBH_VOID * pBuff);
    //Set multi panel
    XBH_S32 setHdmiTxAVMute(XBH_VOID * pBuff);
    //Get multi panel
    XBH_S32 getHdmiTxAVMute(XBH_VOID * pBuff);
    //Set panel index
    XBH_S32 setPanelIndex(XBH_VOID * pBuff);
    //Get panel index
    XBH_S32 getPanelIndex(XBH_VOID * pBuff);
    //Set panel index
    XBH_S32 getPanelMapSwap(XBH_VOID * pBuff);
    //Get panel index
    XBH_S32 setPanelMapSwap(XBH_VOID * pBuff);

    //Set hdcp key 1.4
    XBH_S32 setHDCPKey_14(XBH_VOID * pBuff);
    //Get hdcp key 1.4
    XBH_S32 getHDCPKey_14(XBH_VOID * pBuff);
    //Set hdcp key 2.2
    XBH_S32 setHDCPKey_22(XBH_VOID * pBuff);
    //Get hdcp key 2.2
    XBH_S32 getHDCPKey_22(XBH_VOID * pBuff);

    //Set hdcp key name 1.4
    XBH_S32 setHDCPKeyName_14(XBH_VOID * pBuff);
    //Get hdcp key name 1.4
    XBH_S32 getHDCPKeyName_14(XBH_VOID * pBuff);
    //Set hdcp key name 2.2
    XBH_S32 setHDCPKeyName_22(XBH_VOID * pBuff);
    //Get hdcp key name 2.2
    XBH_S32 getHDCPKeyName_22(XBH_VOID * pBuff);
    //set google key name
    XBH_S32 setGoogleKeyName(XBH_VOID * pBuff);
    //get google key name
    XBH_S32 getGoogleKeyName(XBH_VOID * pBuff);
    //set usbc config for edla certification 0:usbc source 1:otg
    XBH_S32 setUsbcConfig(XBH_VOID * pBuff);
    //get usbc config
    XBH_S32 getUsbcConfig(XBH_VOID * pBuff);


    //Set Chip RunTime
    XBH_S32 setChipRunTime(XBH_S32 time);
    //Get Chip RunTime
    XBH_S32 getChipRunTime(XBH_S32* time);
    //Set wot param value
    XBH_S32 setWotParamValue(XBH_VOID * pBuff);
    //Get wot param value
    XBH_S32 getWotParamValue(XBH_VOID * pBuff);
    //Get wakeup info
    XBH_S32 getWakeupInfoParamValue(XBH_VOID * pBuff);
    //Set power mode param value
    XBH_S32 setPowerModeParamValue(XBH_VOID * pBuff);
    //Get power mode param value
    XBH_S32 getPowerModeParamValue(XBH_VOID * pBuff);
    //Set hdmi edid type param value
    XBH_S32 setHdmiEdidParamValue(XBH_VOID * pBuff);
    //Get hdmi edid type param value
    XBH_S32 getHdmiEdidParamValue(XBH_VOID * pBuff);
    //Set Independence Source edid type param value
    XBH_S32 setIndependenceSourceParamValue(XBH_U32 source,XBH_VOID * pBuff);
    //Get Independence Source edid type param value
    XBH_S32 getIndependenceSourceParamValue(XBH_U32 source,XBH_VOID * pBuff);
    //set Aging Mode State to cuspara
    XBH_S32 setAgingModeState(XBH_VOID * pBuff);
    //get Aging Mode State from cuspara
    XBH_S32 getAgingModeState(XBH_VOID * pBuff);
    //set Moka P Mode Enable To Cusdata
    XBH_S32 setMokaPModeEnableToCusdata(XBH_VOID * pBuff);
    //get Moka P Mode Enable From Cusdata
    XBH_S32 getMokaPModeEnableFromCusdata(XBH_VOID * pBuff);
    //set led light enable to cuspara
    XBH_S32 setLedLightEnable(XBH_VOID * pBuff);
    //get lend light enable from cuspara
    XBH_S32 getLedLightEnable(XBH_VOID * pBuff);
    //Set shutdown flag value
    XBH_S32 setShutdownFlagParamValue(XBH_VOID * pBuff);
    //set Monitor id(UART ID), 广告机部分客户的需求
    XBH_S32 setMonitorId(const std::string& monitorId);
    //get Monitor id(UART ID), 广告机部分客户的需求
    XBH_S32 getMonitorId(std::string& monitorId);
    //set sft data smart的需求
    XBH_S32 setSftData(const XBH_CHAR* data);
    //get sft data smart的需求
    XBH_S32 getSftData(XBH_CHAR* data);
    //set pn data smart的需求
    XBH_S32 setSmartPnData(const XBH_CHAR* data);
    //get pn data smart的需求
    XBH_S32 getSmartPnData(XBH_CHAR* data);
    //set sn data smart的需求
    XBH_S32 setSmartSnData(const XBH_CHAR* data);
    //get sn data smart的需求
    XBH_S32 getSmartSnData(XBH_CHAR* data);
    //set compass presist smart的需求
    XBH_S32 setCompassPresist(const XBH_CHAR* data);
    //set compass presist smart的需求
    XBH_S32 getCompassPresist(XBH_CHAR* data);
    //set scaler pn data smart的需求
    XBH_S32 setSmartScalerPnData(const XBH_CHAR* data);
    //get scaler pn data smart的需求
    XBH_S32 getSmartScalerPnData(XBH_CHAR* data);
    //set scaler sn data smart的需求
    XBH_S32 setSmartScalerSnData(const XBH_CHAR* data);
    //get scaler sn data smart的需求
    XBH_S32 getSmartScalerSnData(XBH_CHAR* data);
    //set code pn data smart的需求
    XBH_S32 setSmartCorePnData(const XBH_CHAR* data);
    //get code pn data smart的需求
    XBH_S32 getSmartCorePnData(XBH_CHAR* data);
    //set code sn data smart的需求
    XBH_S32 setSmartCoreSnData(const XBH_CHAR* data);
    //get code sn data smart的需求
    XBH_S32 getSmartCoreSnData(XBH_CHAR* data);
    //set pn sn separator smart的需求
    XBH_S32 setSmartPNSNSeparator(const XBH_CHAR* data);
    //get pn sn separator smart的需求
    XBH_S32 getSmartPNSNSeparator(XBH_CHAR* data);
    //设置工厂MAC地址
    XBH_S32 saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType, const XBH_CHAR* strMacAddress);
    //获取工厂MAC地址
    XBH_S32 getFactoryMac(XBH_MAC_ADDRESS_TYPE macType, XBH_CHAR* strMacAddress);
    //设置FATT调用烧录的SN/PN接口
    XBH_S32 burnFactorySN(XBH_MATERAL_TYPE materialrype, const XBH_CHAR* snInfo);
    //获取FATT调用烧录的SN/PN接口
    XBH_S32 getBurnFactorySN(XBH_MATERAL_TYPE materialrype, XBH_CHAR* snInfo);
    //set rkpStatus data
    XBH_S32 setRkpStatus(const XBH_CHAR* data);
    //get rkpStatus data
    XBH_S32 getRkpStatus(XBH_CHAR* data);

    XBH_S32 writeDataToCusdata(XBH_U32 offset, XBH_U32 u32Length, const XBH_VOID* data);
    XBH_S32 readDataFromCusdata(XBH_U32 offset, XBH_U32 u32Length, XBH_VOID* data);
private:
    XBH_S32 setCusDataValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff);
    XBH_S32 getCusDataValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff);

    XBH_S32 setCusParamValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff);
    XBH_S32 getCusParamValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff);

    std::string mCusDataPath = "";
    std::string mCusParamPath = "";

    static XbhPartitionData*   mInstance;
    static XbhMutex     mLock;
};

#endif // XBH_PARTITION_DATA
