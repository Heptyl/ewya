#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhPartitionData"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>    // for open(), O_SYNC, O_WRONLY, etc.
#include <hardware/board.h>
#include <unistd.h>   // for close(), read(), write(), etc.
#include <sys/types.h> // for file mode types like S_IRUSR
#include <sys/stat.h>  // for file permission constants like S_IRUSR, S_IRGRP, S_IROTH
#include "XbhCrc.h"
#include "XbhPartitionData.h"

XbhPartitionData*              XbhPartitionData::mInstance = NULL;
XbhMutex                       XbhPartitionData::mLock;

XbhPartitionData::XbhPartitionData()
{
}

XbhPartitionData::~XbhPartitionData()
{
}

static XBH_S32 checkSftDataFormat(const XBH_CHAR* sftdata)
{
     if (NULL == sftdata)
     {
        XLOGE("sftdata is null.");
        return XBH_FAILURE;
     }

     for (const XBH_CHAR* p = sftdata; *p != '\0'; ++p)
     {
        char c = *p;
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
        {
            XLOGE("sftdata format error: invalid character '%c'", c);
            return XBH_FAILURE;
        }
    }
    return XBH_SUCCESS;
}

static XBH_S32 checkMacFormat(const XBH_CHAR* strMacAddress)
{
    if (NULL == strMacAddress)
    {
        XLOGE("strMacAddress is null.");
        return XBH_FAILURE;
    }

    XBH_CHAR* p = const_cast<XBH_CHAR*>(strMacAddress);

    if (17 != strlen(strMacAddress))
    {
        XLOGE("Mac Address format error. Length not 17.");
        return XBH_FAILURE;
    }
    while (*p != '\0')
    {
        if (2 == (p - strMacAddress)%3)
        {
            if ( *p != ':')
            {
                XLOGE("Mac Address format error.");
                return XBH_FAILURE;
            }
            p++;
            continue;
        }
        if (*p < '0' || (*p > '9' && *p < 'A') ||(*p > 'F' && *p < 'a') || *p > 'f' )
        {
            XLOGE("Mac Address format error.");
            return XBH_FAILURE;
        }
        p++;
    }
    return XBH_SUCCESS;
}

XbhPartitionData* XbhPartitionData::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhPartitionData();
    }
    return mInstance;
}

XBH_S32 XbhPartitionData::setDataPath(const std::string& dataPath, const std::string& paramPath)
{
    mCusDataPath = dataPath;
    mCusParamPath = paramPath;
    XLOGD("mCusDataPath = %s", mCusDataPath.data());
    XLOGD("mCusParamPath = %s", mCusParamPath.data());
    return XBH_SUCCESS;
}

//Set serial number
XBH_S32 XbhPartitionData::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_BARCODE_LEN] = {0};
    memset(s8Buff, 0, sizeof(s8Buff));
    XBH_S32 s32Len = XBH_CUSDATA_BARCODE_LEN - 1;
    if (strlen(strSn) < s32Len)
    {
        s32Len = strlen(strSn);
    }
    strncpy(s8Buff, strSn, s32Len);
    s8Buff[s32Len] = '\0';
    XLOGD("setSn: %s\n", s8Buff);
    s32Ret = writeDataToCusdata(XBH_CUSDATA_BARCODE_OFFFSET, XBH_CUSDATA_BARCODE_LEN, (XBH_VOID *)s8Buff);

    return  s32Ret;
}

//Get serial number
XBH_S32 XbhPartitionData::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_BARCODE_LEN] = {0};
    XBH_S32 i = 0;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_BARCODE_OFFFSET, XBH_CUSDATA_BARCODE_LEN, (XBH_VOID *)s8Buff);

    // confirm serial number format
    for (i = 0; i < XBH_CUSDATA_BARCODE_LEN; i++)
    {
        if((s8Buff[i] >= 'A' && s8Buff[i] <= 'Z')
                || (s8Buff[i] >= 'a' && s8Buff[i] <= 'z')
                || (s8Buff[i] >= '0' && s8Buff[i] <= '9')
                || (s8Buff[i] == '-')
                || (s8Buff[i] == '_'))
        {
            strSn[i] = s8Buff[i];
        }
        else
        {
            strSn[i] = '\0';
            break;
        }
    }
    //use default sn
    if (strlen(strSn) == 0)
    {
        strcpy(strSn, "01234567");
    }
    XLOGD("getSn: %s\n", strSn);
    return  XBH_SUCCESS;
}

//set ethernet mac address
XBH_S32 XbhPartitionData::setMacAddress(const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = checkMacFormat(strMacAddress);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("macaddress format error.");
        return XBH_FAILURE;
    }

    s32Ret = writeDataToCusdata(XBH_CUSDATA_MACADDRESS_OFFSET, XBH_CUSDATA_MACADDRESS_LEN, (XBH_VOID *)strMacAddress);
    XLOGD("setMacAddress:%s, ret:%d", strMacAddress, s32Ret);
    return s32Ret;
}

//get ethernet mac address
XBH_S32 XbhPartitionData::getMacAddress(XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_MACADDRESS_OFFSET, XBH_CUSDATA_MACADDRESS_LEN, (XBH_VOID *)strMacAddress);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("getCusDataValue fail.");
        return XBH_FAILURE;
    }

    s32Ret = checkMacFormat(strMacAddress);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("macaddress format error. Use default value.");
        //Use the default MAC address
        strcpy(strMacAddress, "00:55:66:77:88:01");
    }

    XLOGD("getMacAddress:%s, ret:%d", strMacAddress, s32Ret);
    return XBH_SUCCESS;
}

/**
 * 写入Project ID
 * param[in] value Project ID
 * retval true:success, false:failure
*/
XBH_S32 XbhPartitionData::setProjectId(const XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_PROJECT_ID_LEN] = {0};
    memset(s8Buff, 0, sizeof(s8Buff));
    XBH_S32 s32Len = XBH_CUSDATA_PROJECT_ID_LEN - 1;
    if (strlen(strProjectId) < s32Len)
    {
        s32Len = strlen(strProjectId);
    }
    strncpy(s8Buff, strProjectId, s32Len);
    s8Buff[s32Len] = '\0';
    XLOGD("setProjectId: %s\n", s8Buff);
    s32Ret = writeDataToCusdata(XBH_CUSDATA_PROJECT_ID_OFFSET, XBH_CUSDATA_PROJECT_ID_LEN, (XBH_VOID *)s8Buff);

    return  s32Ret;
}

/**
 * 获取Project ID
 * param[out] strProjectId Project ID
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPartitionData::getProjectId(XBH_CHAR* strProjectId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_PROJECT_ID_LEN] = {0};
    XBH_S32 i = 0;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_PROJECT_ID_OFFSET, XBH_CUSDATA_PROJECT_ID_LEN, (XBH_VOID *)s8Buff);

    if (strlen(s8Buff) > 0)
    {
        memcpy(strProjectId, s8Buff, strlen(s8Buff));
    }
    else
    {
        memcpy(strProjectId, "0", strlen("0"));
    }
    XLOGD("getProjectId: %s\n", strProjectId);
    return  s32Ret;
}

/**
 * 设置对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[in] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPartitionData::setColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_GAIN_OFFSET_DATA_S colorTempValue[XBH_COLORTEMP_BUTT];
    memset(colorTempValue, 0, sizeof(colorTempValue));
    s32Ret = readDataFromCusdata(XBH_CUSDATA_COLORTEMP_DATA_OFFSET, XBH_CUSDATA_COLORTEMP_DATA_LEN, (XBH_CHAR *)colorTempValue);
    memcpy(colorTempValue + enColorTemp, data, sizeof(XBH_GAIN_OFFSET_DATA_S));
    s32Ret = writeDataToCusdata(XBH_CUSDATA_COLORTEMP_DATA_OFFSET, XBH_CUSDATA_COLORTEMP_DATA_LEN, (XBH_CHAR *)colorTempValue);
    return  s32Ret;
}

/**
 * 获取对应模式的具体色温数据
 * param[in] enColorTemp. 色温模式
 * param[out] data. 色温参数
 * retval 0:success,-1:failure
*/
XBH_S32 XbhPartitionData::getColorTempPara(XBH_COLORTEMP_E enColorTemp, XBH_GAIN_OFFSET_DATA_S* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_GAIN_OFFSET_DATA_S colorTempValue[XBH_COLORTEMP_BUTT];
    memset(colorTempValue, 0, sizeof(colorTempValue));
    s32Ret = readDataFromCusdata(XBH_CUSDATA_COLORTEMP_DATA_OFFSET, XBH_CUSDATA_COLORTEMP_DATA_LEN, (XBH_CHAR *)colorTempValue);

    if (s32Ret == XBH_SUCCESS) 
    {
        memcpy(data, colorTempValue + enColorTemp, sizeof(XBH_GAIN_OFFSET_DATA_S));
    }
    else
    {
        return XBH_FAILURE;
    }

    return  s32Ret;
}

XBH_S32 XbhPartitionData::setDeviceId(const XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_BARCODE_LEN] = {0};
    memset(s8Buff, 0, sizeof(s8Buff));
    XBH_S32 s32Len = XBH_CUSDATA_BARCODE_LEN - 1;
    if (strlen(strDeviceId) < s32Len)
    {
        s32Len = strlen(strDeviceId);
    }
    strncpy(s8Buff, strDeviceId, s32Len);
    s8Buff[s32Len] = '\0';
    XLOGD("setDeviceId: %s\n", s8Buff);
    s32Ret = writeDataToCusdata(XBH_CUSDATA_BOARD_DEVICE_ID_OFFSET, XBH_CUSDATA_BOARD_DEVICE_ID_LEN, (XBH_VOID *)s8Buff);
    return  s32Ret;
}

XBH_S32 XbhPartitionData::getDeviceId(XBH_CHAR* strDeviceId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_BARCODE_LEN] = {0};
    XBH_S32 i = 0;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_BOARD_DEVICE_ID_OFFSET, XBH_CUSDATA_BOARD_DEVICE_ID_LEN, (XBH_VOID *)s8Buff);
    // confirm serial number format
    for (i = 0; i < XBH_CUSDATA_BARCODE_LEN; i++)
    {
        if((s8Buff[i] >= 'A' && s8Buff[i] <= 'Z')
                || (s8Buff[i] >= 'a' && s8Buff[i] <= 'z')
                || (s8Buff[i] >= '0' && s8Buff[i] <= '9')
                || (s8Buff[i] == '-')
                || (s8Buff[i] == '_'))
        {
            strDeviceId[i] = s8Buff[i];
        }
        else
        {
            strDeviceId[i] = '\0';
            break;
        }
    }
    XLOGD("getDeviceId: %s\n", strDeviceId);
    return  s32Ret;
}

//Set multi panel
XBH_S32 XbhPartitionData::setMultiPanel(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_MULTIPANEL_OFFSET, XBH_CUSDATA_MULTIPANE_LEN, pBuff);
    return s32Ret;
}

//Get multi panel
XBH_S32 XbhPartitionData::getMultiPanel(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_MULTIPANEL_OFFSET, XBH_CUSDATA_MULTIPANE_LEN, pBuff);
    return s32Ret;
}

//Set hdmi tx video mute state
XBH_S32 XbhPartitionData::setHdmiTxAVMute(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_OFFSET, XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_LEN, pBuff);
    return s32Ret;
}

//Get hdmi tx video mute state
XBH_S32 XbhPartitionData::getHdmiTxAVMute(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_OFFSET, XBH_CUSPARAM_HDMI_OUT_VEDIO_MUTE_LEN, pBuff);
    return s32Ret;
}

//Set panel index
XBH_S32 XbhPartitionData::setPanelIndex(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_PANEL_IDX_OFFSET, XBH_CUSDATA_PANEL_IDX_LEN, pBuff);
    return s32Ret;
}

//Get panel index
XBH_S32 XbhPartitionData::getPanelIndex(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_PANEL_IDX_OFFSET, XBH_CUSDATA_PANEL_IDX_LEN, pBuff);
    return s32Ret;
}

//Set panel index
XBH_S32 XbhPartitionData::setPanelMapSwap(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_PANEL_MAP_SWAP_OFFSET, XBH_CUSDATA_PANEL_MAP_SWAP_LEN, pBuff);
    return s32Ret;
}

//Get panel index
XBH_S32 XbhPartitionData::getPanelMapSwap(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_PANEL_MAP_SWAP_OFFSET, XBH_CUSDATA_PANEL_MAP_SWAP_LEN, pBuff);
    return s32Ret;
}

//Set hdcp key 1.4
XBH_S32 XbhPartitionData::setHDCPKey_14(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_HDCP_KEY_14_OFFSET, XBH_CUSDATA_HDCP_KEY_14_LEN, pBuff);
    return s32Ret;
}

//Get hdcp key 1.4
XBH_S32 XbhPartitionData::getHDCPKey_14(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_HDCP_KEY_14_OFFSET, XBH_CUSDATA_HDCP_KEY_14_LEN, pBuff);
    return s32Ret;
}

//Set hdcp key 2.2
XBH_S32 XbhPartitionData::setHDCPKey_22(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_HDCP_KEY_22_OFFSET, XBH_CUSDATA_HDCP_KEY_22_LEN, pBuff);
    return s32Ret;
}

//Get hdcp key 2.2
XBH_S32 XbhPartitionData::getHDCPKey_22(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_HDCP_KEY_22_OFFSET, XBH_CUSDATA_HDCP_KEY_22_LEN, pBuff);
    return s32Ret;
}

//Set hdcp key name 1.4
XBH_S32 XbhPartitionData::setHDCPKeyName_14(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_HDCP_KEY_14_NAME_OFFSET, XBH_CUSDATA_HDCP_KEY_14_NAME_LEN, pBuff);
    return s32Ret;
}

//Get hdcp key name 1.4
XBH_S32 XbhPartitionData::getHDCPKeyName_14(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_HDCP_KEY_14_NAME_OFFSET, XBH_CUSDATA_HDCP_KEY_14_NAME_LEN, pBuff);
    return s32Ret;
}

//Set hdcp key name 1.4
XBH_S32 XbhPartitionData::setHDCPKeyName_22(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_HDCP_KEY_22_NAME_OFFSET, XBH_CUSDATA_HDCP_KEY_22_NAME_LEN, pBuff);
    return s32Ret;
}

//Get hdcp key name 1.4
XBH_S32 XbhPartitionData::getHDCPKeyName_22(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_HDCP_KEY_22_NAME_OFFSET, XBH_CUSDATA_HDCP_KEY_22_NAME_LEN, pBuff);
    return s32Ret;
}

//set google key name
XBH_S32 XbhPartitionData::setGoogleKeyName(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_GOOGLE_KEY_NAME_OFFSET, XBH_CUSDATA_GOOGLE_KEY_NAME_LEN, pBuff);
    return s32Ret;
}

//get google key name
XBH_S32 XbhPartitionData::getGoogleKeyName(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_GOOGLE_KEY_NAME_OFFSET, XBH_CUSDATA_GOOGLE_KEY_NAME_LEN, pBuff);
    return s32Ret;
}
//set usbc config
XBH_S32 XbhPartitionData::setUsbcConfig(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_USBC_CONFIG_OFFSET, XBH_CUSDATA_USBC_CONFIG_LEN, pBuff);
    return s32Ret;
}

//get usbc config
XBH_S32 XbhPartitionData::getUsbcConfig(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_USBC_CONFIG_OFFSET, XBH_CUSDATA_USBC_CONFIG_LEN, pBuff);
    return s32Ret;
}

//set Moka P Mode Enable To Cusdata
XBH_S32 XbhPartitionData::setMokaPModeEnableToCusdata(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_MOKA_P_MODE_ENABLE_OFFSET, XBH_CUSDATA_MOKA_P_MODE_ENABLE_LEN, pBuff);
    return s32Ret;
}

//get Moka P Mode Enable To Cusdata
XBH_S32 XbhPartitionData::getMokaPModeEnableFromCusdata(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_MOKA_P_MODE_ENABLE_OFFSET, XBH_CUSDATA_MOKA_P_MODE_ENABLE_LEN, pBuff);
    return s32Ret;
}

//set sft data
XBH_S32 XbhPartitionData::setSftData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = checkSftDataFormat(data);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("sft data format error.");
        return XBH_FAILURE;
    }

    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_SFT_OFFSET, XBH_CUSDATA_SMART_SFT_LEN, (XBH_VOID *)data);
    XLOGD("setSftData:%s, ret:%d", data, s32Ret);
    return s32Ret;
}

//get sft data
XBH_S32 XbhPartitionData::getSftData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_SFT_OFFSET, XBH_CUSDATA_SMART_SFT_LEN, (XBH_VOID *)data);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("getCusDataValue fail.");
        return XBH_FAILURE;
    }

    s32Ret = checkSftDataFormat(data);
    if (s32Ret != XBH_SUCCESS)
    {
         XLOGE("sft data format error.");
        return XBH_FAILURE;
    }

    XLOGD("getSftData:%s, ret:%d", data, s32Ret);
    return XBH_SUCCESS;
}

//set rkp data
XBH_S32 XbhPartitionData::setRkpStatus(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_RKP_STATUS_OFFSET, XBH_CUSDATA_SMART_RKP_STATUS_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get rkp data
XBH_S32 XbhPartitionData::getRkpStatus(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_RKP_STATUS_OFFSET, XBH_CUSDATA_SMART_RKP_STATUS_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set pn data smart的需求
XBH_S32 XbhPartitionData::setSmartPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_PN_OFFSET, XBH_CUSDATA_SMART_PN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get pn data smart的需求
XBH_S32 XbhPartitionData::getSmartPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_PN_OFFSET, XBH_CUSDATA_SMART_PN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set sn data smart的需求
XBH_S32 XbhPartitionData::setSmartSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_SN_OFFSET, XBH_CUSDATA_SMART_SN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get sn data smart的需求
XBH_S32 XbhPartitionData::getSmartSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_SN_OFFSET, XBH_CUSDATA_SMART_SN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set compass presist smart的需求
XBH_S32 XbhPartitionData::setCompassPresist(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_COMPASS_PRESIST_OFFSET, XBH_CUSDATA_COMPASS_PRESIST_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get compass presist smart的需求
XBH_S32 XbhPartitionData::getCompassPresist(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_COMPASS_PRESIST_OFFSET, XBH_CUSDATA_COMPASS_PRESIST_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set scaler pn data smart的需求
XBH_S32 XbhPartitionData::setSmartScalerPnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_SCALER_PN_OFFSET, XBH_CUSDATA_SMART_SCALER_PN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get scaler pn data smart的需求
XBH_S32 XbhPartitionData::getSmartScalerPnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_SCALER_PN_OFFSET, XBH_CUSDATA_SMART_SCALER_PN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set scaler sn data smart的需求
XBH_S32 XbhPartitionData::setSmartScalerSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_SCALER_SN_OFFSET, XBH_CUSDATA_SMART_SCALER_SN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get scaler sn data smart的需求
XBH_S32 XbhPartitionData::getSmartScalerSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_SCALER_SN_OFFSET, XBH_CUSDATA_SMART_SCALER_SN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set code pn data smart的需求
XBH_S32 XbhPartitionData::setSmartCorePnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_CORE_PN_OFFSET, XBH_CUSDATA_SMART_CORE_PN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get code pn data smart的需求
XBH_S32 XbhPartitionData::getSmartCorePnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_CORE_PN_OFFSET, XBH_CUSDATA_SMART_CORE_PN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set code sn data smart的需求
XBH_S32 XbhPartitionData::setSmartCoreSnData(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_CORE_SN_OFFSET, XBH_CUSDATA_SMART_CORE_SN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get code sn data smart的需求
XBH_S32 XbhPartitionData::getSmartCoreSnData(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_CORE_SN_OFFSET, XBH_CUSDATA_SMART_CORE_SN_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//set pn sn separator smart的需求
XBH_S32 XbhPartitionData::setSmartPNSNSeparator(const XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeDataToCusdata(XBH_CUSDATA_SMART_PN_SN_SEPARATOR_OFFSET, XBH_CUSDATA_SMART_PN_SN_SEPARATOR_LEN, (XBH_VOID *)data);
    return s32Ret;
}

//get pn sn separator smart的需求
XBH_S32 XbhPartitionData::getSmartPNSNSeparator(XBH_CHAR* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = readDataFromCusdata(XBH_CUSDATA_SMART_PN_SN_SEPARATOR_OFFSET, XBH_CUSDATA_SMART_PN_SN_SEPARATOR_LEN, (XBH_VOID *)data);
    return s32Ret;
}

// 设置工厂MAC地址
XBH_S32 XbhPartitionData::saveFactoryMac(XBH_MAC_ADDRESS_TYPE macType,const XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = checkMacFormat(strMacAddress);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("XBH_MAC_ADDRESS_TYPE  %d format error.", macType);
        return XBH_FAILURE;
    }
    switch (macType)
    {
        case XBH_MAC:
            s32Ret = writeDataToCusdata(XBH_CUSDATA_MACADDRESS_OFFSET, XBH_CUSDATA_MACADDRESS_LEN, (XBH_VOID *)strMacAddress);
            break;
        case XBH_TYPEC_1:
            s32Ret = writeDataToCusdata(XBH_CUSDATA_TPYEC_MAC_ADDRESS_OFFSET, XBH_CUSDATA_TPYEC_MAC_ADDRESS_LEN, (XBH_VOID *)strMacAddress);
            break;
        case XBH_TYPEC_2:
            s32Ret = writeDataToCusdata(XBH_CUSDATA_TPYEC_MAC_ADDRESS_OFFSET_2, XBH_CUSDATA_TPYEC_MAC_ADDRESS_LEN_2, (XBH_VOID *)strMacAddress);
            break;    
        case XBH_OPS_1:
             s32Ret = writeDataToCusdata(XBH_CUSDATA_OPS_MAC_ADDRESS_OFFSET, XBH_CUSDATA_OPS_MAC_ADDRESS_LEN, (XBH_VOID *)strMacAddress);
             break;      
        case XBH_OPS_2:
             s32Ret = writeDataToCusdata(XBH_CUSDATA_OPS_MAC_ADDRESS_OFFSET_2, XBH_CUSDATA_OPS_MAC_ADDRESS_LEN_2, (XBH_VOID *)strMacAddress);
             break;           
        default:
             s32Ret = XBH_FAILURE;
             break;
    }
  
    XLOGD("XBH_MAC_ADDRESS_TYPE: %d, setOpsMacAddress:%s, ret:%d", macType, strMacAddress, s32Ret);
    return s32Ret;
}

// 获取工厂MAC地址
XBH_S32 XbhPartitionData::getFactoryMac(XBH_MAC_ADDRESS_TYPE macType,XBH_CHAR* strMacAddress)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    switch (macType)
    {
        case XBH_MAC:
           s32Ret = readDataFromCusdata(XBH_CUSDATA_MACADDRESS_OFFSET, XBH_CUSDATA_MACADDRESS_LEN, (XBH_VOID *)strMacAddress);
          break;
        case XBH_TYPEC_1:
            s32Ret = readDataFromCusdata(XBH_CUSDATA_TPYEC_MAC_ADDRESS_OFFSET, XBH_CUSDATA_TPYEC_MAC_ADDRESS_LEN, (XBH_VOID *)strMacAddress);
            break;
        case XBH_TYPEC_2:
            s32Ret = readDataFromCusdata(XBH_CUSDATA_TPYEC_MAC_ADDRESS_OFFSET_2, XBH_CUSDATA_TPYEC_MAC_ADDRESS_LEN_2, (XBH_VOID *)strMacAddress);
            break;    
        case XBH_OPS_1:
             s32Ret = readDataFromCusdata(XBH_CUSDATA_OPS_MAC_ADDRESS_OFFSET, XBH_CUSDATA_OPS_MAC_ADDRESS_LEN, (XBH_VOID *)strMacAddress);
             break;      
        case XBH_OPS_2:
             s32Ret = readDataFromCusdata(XBH_CUSDATA_OPS_MAC_ADDRESS_OFFSET_2, XBH_CUSDATA_OPS_MAC_ADDRESS_LEN_2, (XBH_VOID *)strMacAddress);
             break;           
        default:
             s32Ret = XBH_FAILURE;
             break;  
    }
   
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("getCusDataValue fail.");
        return XBH_FAILURE;
    }

    s32Ret = checkMacFormat(strMacAddress);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("macaddress %d format error. Use default value.", macType);
        //Use the default MAC address
        strcpy(strMacAddress, "00:55:66:77:88:01");
    }

    XLOGD("macType:%d macAddress:%s, ret:%d", macType, strMacAddress, s32Ret);
    return XBH_SUCCESS;
}

//设置FATT调用烧录的SN接口
XBH_S32 XbhPartitionData::burnFactorySN(XBH_MATERAL_TYPE materialrype, const XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    switch (materialrype)
    {
        case XBH_MAIN_SN:
            s32Ret = setSn(snInfo);
            break;
        case XBH_CORE_SN:
            s32Ret = writeDataToCusdata(XBH_CUSDATA_CORE_SN_OFFSET, XBH_CUSDATA_CORE_SN_LEN, (XBH_VOID *)snInfo);
            break;
        case XBH_AUXILIARY_SN_1:
		    s32Ret = writeDataToCusdata(XBH_CUSDATA_AUXILIARY_SN_1_OFFSET, XBH_CUSDATA_AUXILIARY_SN_1_LEN, (XBH_VOID *)snInfo);
            break;
        case XBH_AUXILIARY_SN_2:
            s32Ret = writeDataToCusdata(XBH_CUSDATA_AUXILIARY_SN_2_OFFSET, XBH_CUSDATA_AUXILIARY_SN_2_LEN, (XBH_VOID *)snInfo);
            break;
        case XBH_AUXILIARY_SN_3:
            s32Ret = writeDataToCusdata(XBH_CUSDATA_AUXILIARY_SN_3_OFFSET, XBH_CUSDATA_AUXILIARY_SN_3_LEN, (XBH_VOID *)snInfo);
            break;
        default:
            s32Ret = XBH_FAILURE;
            break;
    }
    return s32Ret;
}

//获取FATT调用烧录的SN接口
XBH_S32 XbhPartitionData::getBurnFactorySN(XBH_MATERAL_TYPE materialrype, XBH_CHAR* snInfo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    switch (materialrype)
    {
        case XBH_MAIN_SN:
            s32Ret = getSn(snInfo);
            break;
        case XBH_CORE_SN:
            s32Ret = readDataFromCusdata(XBH_CUSDATA_CORE_SN_OFFSET, XBH_CUSDATA_CORE_SN_LEN, (XBH_VOID *)snInfo);
            break;
        case XBH_AUXILIARY_SN_1:
		    s32Ret = readDataFromCusdata(XBH_CUSDATA_AUXILIARY_SN_1_OFFSET, XBH_CUSDATA_AUXILIARY_SN_1_LEN, (XBH_VOID *)snInfo);
            break;
        case XBH_AUXILIARY_SN_2:
            s32Ret = readDataFromCusdata(XBH_CUSDATA_AUXILIARY_SN_2_OFFSET, XBH_CUSDATA_AUXILIARY_SN_2_LEN, (XBH_VOID *)snInfo);
            break;
        case XBH_AUXILIARY_SN_3:
            s32Ret = readDataFromCusdata(XBH_CUSDATA_AUXILIARY_SN_3_OFFSET, XBH_CUSDATA_AUXILIARY_SN_3_LEN, (XBH_VOID *)snInfo);
        default:
            s32Ret = XBH_FAILURE;
            break;
    }
    return s32Ret;
}



//======================================================================================================set cus param==================================================================================================
//Set Chip RunTime
XBH_S32 XbhPartitionData::setChipRunTime(XBH_S32 time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR times[XBH_CUSPARAM_CHIP_RUNTIME_LEN]= {0};
    sprintf(times,"%d",time);
#if defined(SUPPORT_XBH_CHIP_RUNTIME_CUSDATA) && (SUPPORT_XBH_CHIP_RUNTIME_CUSDATA == 1)
    s32Ret = writeDataToCusdata(XBH_CUSDATA_CHIP_RUNTIME_OFFSET, XBH_CUSDATA_CHIP_RUNTIME_LEN, (XBH_VOID *)&times);
#else
    s32Ret = setCusParamValue(XBH_CUSPARAM_CHIP_RUNTIME_OFFSET, XBH_CUSPARAM_CHIP_RUNTIME_LEN, (XBH_VOID *)&times);
#endif
    XLOGD("setChipRunTime:%d, ret:%d", time, s32Ret);
    return s32Ret;
}

//Get Chip RunTime
XBH_S32 XbhPartitionData::getChipRunTime(XBH_S32* time)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 t = 0;
    XBH_CHAR times[XBH_CUSPARAM_CHIP_RUNTIME_LEN]={0};
#if defined(SUPPORT_XBH_CHIP_RUNTIME_CUSDATA) && (SUPPORT_XBH_CHIP_RUNTIME_CUSDATA == 1)
    s32Ret = readDataFromCusdata(XBH_CUSDATA_CHIP_RUNTIME_OFFSET, XBH_CUSDATA_CHIP_RUNTIME_LEN, (XBH_VOID *)&times);
#else
    s32Ret = getCusParamValue(XBH_CUSPARAM_CHIP_RUNTIME_OFFSET, XBH_CUSPARAM_CHIP_RUNTIME_LEN, (XBH_VOID *)&times);
#endif
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("getChipRunTime fail.");
        return XBH_FAILURE;
    }
    t = atoi(times);
    *time = t;
    XLOGD("getChipRunTime :%d, ret:%d",*time, s32Ret);
    return s32Ret;
}

//Set wot param value
XBH_S32 XbhPartitionData::setWotParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_WAKEUP_SWITCH_OFFSET, XBH_CUSPARAM_WAKEUP_SWITCH_LEN, pBuff);
    return s32Ret;
}

//Get wot param value
XBH_S32 XbhPartitionData::getWotParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_WAKEUP_SWITCH_OFFSET, XBH_CUSPARAM_WAKEUP_SWITCH_LEN, pBuff);
    return s32Ret;
}

//Get wakeup info
XBH_S32 XbhPartitionData::getWakeupInfoParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_WAKEUP_INFO_OFFSET, XBH_CUSPARAM_WAKEUP_INFO_LEN, pBuff);
    return s32Ret;
}

//Set power mode param value
XBH_S32 XbhPartitionData::setPowerModeParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_POWER_ON_MODE_OFFSET, XBH_CUSPARAM_POWER_ON_MODE_LEN, pBuff);
    return s32Ret;
}

//Get power mode param value
XBH_S32 XbhPartitionData::getPowerModeParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_POWER_ON_MODE_OFFSET, XBH_CUSPARAM_POWER_ON_MODE_LEN, pBuff);
    return s32Ret;
}

//Set shutdown flag value
XBH_S32 XbhPartitionData::setShutdownFlagParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_SHUTDOWN_FLAG_OFFSET, XBH_CUSPARAM_SHUTDOWN_FLAG_LEN, pBuff);
    return s32Ret;
}

//Set hdmi edid type param value
XBH_S32 XbhPartitionData::setHdmiEdidParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_BOARD_HDMIRX_EDID_OFFSET, XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN, pBuff);
    return s32Ret;
}

//Get hdmi edid type param value
XBH_S32 XbhPartitionData::getHdmiEdidParamValue(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_BOARD_HDMIRX_EDID_OFFSET, XBH_CUSPARAM_BOARD_HDMIRX_EDID_LEN, pBuff);
    return s32Ret;
}

//Set Independence Source edid type param value
XBH_S32 XbhPartitionData::setIndependenceSourceParamValue(XBH_U32 source,XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    switch (source)
    {
        case XBH_SOURCE_HDMI1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_HDMI1_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_HDMI2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_HDMI2_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_HDMI3:
            s32Ret = setCusParamValue(XBH_CUSPARAM_HDMI3_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI3_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_HDMI4:
            s32Ret = setCusParamValue(XBH_CUSPARAM_HDMI4_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI4_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_DP1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_DP1_EDID_TYPE_OFFSET, XBH_CUSPARAM_DP1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_DP2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_DP2_EDID_TYPE_OFFSET, XBH_CUSPARAM_DP2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_USBC1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_USBC1_EDID_TYPE_OFFSET, XBH_CUSPARAM_USBC1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_USBC2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_USBC2_EDID_TYPE_OFFSET, XBH_CUSPARAM_USBC2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_OPS1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_OPS1_EDID_TYPE_OFFSET, XBH_CUSPARAM_OPS1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_OPS2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_OPS2_EDID_TYPE_OFFSET, XBH_CUSPARAM_OPS2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_USBC1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_F_USBC1_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_USBC1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_USBC2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_F_USBC2_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_USBC2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_HDMI1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_FRONT_HDMI1_EDID_TYPE_OFFSET, XBH_CUSPARAM_FRONT_HDMI1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_HDMI2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_FRONT_HDMI2_EDID_TYPE_OFFSET, XBH_CUSPARAM_FRONT_HDMI2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_DP1:
            s32Ret = setCusParamValue(XBH_CUSPARAM_F_DP1_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_DP1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_DP2:
            s32Ret = setCusParamValue(XBH_CUSPARAM_F_DP2_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_DP2_EDID_TYPE_LEN, pBuff);
            break;
        default:
            XLOGE("setIndependenceSourceParamValue edid USE default !!! ");
            s32Ret = XBH_FAILURE;
            break;
    }
    return s32Ret;
}

//Get Independence Source edid type param value
XBH_S32 XbhPartitionData::getIndependenceSourceParamValue(XBH_U32 source,XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    switch (source)
    {
        case XBH_SOURCE_HDMI1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_HDMI1_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_HDMI2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_HDMI2_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_HDMI3:
            s32Ret = getCusParamValue(XBH_CUSPARAM_HDMI3_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI3_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_HDMI4:
            s32Ret = getCusParamValue(XBH_CUSPARAM_HDMI4_EDID_TYPE_OFFSET, XBH_CUSPARAM_HDMI4_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_DP1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_DP1_EDID_TYPE_OFFSET, XBH_CUSPARAM_DP1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_DP2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_DP2_EDID_TYPE_OFFSET, XBH_CUSPARAM_DP2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_USBC1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_USBC1_EDID_TYPE_OFFSET, XBH_CUSPARAM_USBC1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_USBC2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_USBC2_EDID_TYPE_OFFSET, XBH_CUSPARAM_USBC2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_OPS1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_OPS1_EDID_TYPE_OFFSET, XBH_CUSPARAM_OPS1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_OPS2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_OPS2_EDID_TYPE_OFFSET, XBH_CUSPARAM_OPS2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_USBC1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_F_USBC1_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_USBC1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_USBC2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_F_USBC2_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_USBC2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_HDMI1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_FRONT_HDMI1_EDID_TYPE_OFFSET, XBH_CUSPARAM_FRONT_HDMI1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_HDMI2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_FRONT_HDMI2_EDID_TYPE_OFFSET, XBH_CUSPARAM_FRONT_HDMI2_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_DP1:
            s32Ret = getCusParamValue(XBH_CUSPARAM_F_DP1_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_DP1_EDID_TYPE_LEN, pBuff);
            break;
        case XBH_SOURCE_F_DP2:
            s32Ret = getCusParamValue(XBH_CUSPARAM_F_DP2_EDID_TYPE_OFFSET, XBH_CUSPARAM_F_DP2_EDID_TYPE_LEN, pBuff);
            break;
        default:
            XLOGE("setIndependenceSourceParamValue edid USE default !!! ");
            s32Ret = XBH_FAILURE;
            break;
    }
    return s32Ret;
}

//set Aging Mode State from cuspara
XBH_S32 XbhPartitionData::setAgingModeState(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_AGING_MODE_STATE_OFFSET, XBH_CUSPARAM_AGING_MODE_STATE_LEN, pBuff);
    return s32Ret;
}

//get Aging Mode State from cuspara
XBH_S32 XbhPartitionData::getAgingModeState(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_AGING_MODE_STATE_OFFSET, XBH_CUSPARAM_AGING_MODE_STATE_LEN, pBuff);
    return s32Ret;
}

//get led light enable to cuspara
XBH_S32 XbhPartitionData::getLedLightEnable(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = getCusParamValue(XBH_CUSPARAM_LED_LIGHT_ENBALE_OFFSET, XBH_CUSPARAM_LED_LIGHT_ENBALE_LEN, pBuff);
    return s32Ret;
}

//set led light enable to cuspara
XBH_S32 XbhPartitionData::setLedLightEnable(XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = setCusParamValue(XBH_CUSPARAM_LED_LIGHT_ENBALE_OFFSET, XBH_CUSPARAM_LED_LIGHT_ENBALE_LEN, pBuff);
    return s32Ret;
}


XBH_S32 XbhPartitionData::setMonitorId(const std::string& monitorId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_BARCODE_LEN] = {0};
    memset(s8Buff, 0, sizeof(s8Buff));
    strncpy(s8Buff, monitorId.c_str(), sizeof(s8Buff) - 1);

    XLOGD("setDeviceId: %s\n", s8Buff);
    s32Ret = setCusParamValue(XBH_CUSPARAM_BOARD_MONITOR_ID_OFFSET, XBH_CUSPARAM_BOARD_MONITOR_ID_LEN, (XBH_VOID *)s8Buff);

    return s32Ret;
}

XBH_S32 XbhPartitionData::getMonitorId(std::string& monitorId)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_CHAR s8Buff[XBH_CUSDATA_BARCODE_LEN] = {0};

    // 从存储中读取监视器 ID
    s32Ret = readDataFromCusdata(XBH_CUSPARAM_BOARD_MONITOR_ID_OFFSET, XBH_CUSPARAM_BOARD_MONITOR_ID_LEN, (XBH_VOID *)s8Buff);

    if (s32Ret == XBH_SUCCESS) {
        monitorId = std::string(s8Buff);
        XLOGD("getDeviceId: %s\n", monitorId.c_str());
    }

    return s32Ret;
}

XBH_S32 XbhPartitionData::writeDataToCusdata(XBH_U32 offset, XBH_U32 u32Length, const XBH_VOID* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U16 u16Crc16Value;
    XLOGD("offset=%d, u32Length=%d\n",offset,u32Length);
    s32Ret = setCusDataValue(offset, u32Length, (XBH_VOID *)data);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("writeDataToCusdata offset error\n");
        return XBH_FAILURE;
    }

    u16Crc16Value = XbhCrc::crc16((XBH_U8 *)data, u32Length);
    XLOGD("u16Crc16Value: 0x%x\n", u16Crc16Value);
    s32Ret = setCusDataValue(offset + u32Length, 2, (XBH_VOID *)&u16Crc16Value);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("writeDataToCusdata error\n");
        return XBH_FAILURE;
    }
    return s32Ret;
}

XBH_S32 XbhPartitionData::readDataFromCusdata(XBH_U32 offset, XBH_U32 u32Length, XBH_VOID* data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U16 u16Crc16Value;
    XBH_U16 u16FlashCrc16Value;
    // XBH_U8 u8Buff[64] = {0};
    XBH_U8* u8Buff;
    u8Buff = (XBH_U8*)malloc(u32Length);
    memset(u8Buff, 0, sizeof(XBH_U8));

    XLOGD("offset=%d, u32Length=%d\n",offset,u32Length);
    s32Ret = getCusDataValue(offset,u32Length, (XBH_VOID *)u8Buff);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("readDataToCusdata offset error\n");
        free(u8Buff);
        u8Buff = NULL;
        return XBH_FAILURE;
    }

    u16Crc16Value = XbhCrc::crc16((XBH_U8 *)u8Buff, u32Length);
    XLOGD("crc from read buff u16Crc16Value: 0x%x\n", u16Crc16Value);

    s32Ret = getCusDataValue(offset + u32Length, 2, (XBH_VOID *)&u16FlashCrc16Value);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("readDataToCusdata offset error\n");
        free(u8Buff);
        u8Buff = NULL;
        return XBH_FAILURE;
    }
    XLOGD("crc from last writte crc cusdata u16FlashCrc16Value: 0x%x\n", u16FlashCrc16Value);

    if (u16Crc16Value == u16FlashCrc16Value)
    {
        memcpy(data, u8Buff, u32Length);
    }
    else
    {
        XLOGE("check crc16 value fail.\n", u16FlashCrc16Value);
        XLOGE("u16Crc16Value: 0x%x\n", u16Crc16Value);
        XLOGE("u16FlashCrc16Value: 0x%x\n", u16FlashCrc16Value);
        //当CRC不匹配时，重新写入CRC值，兼容之前没有计算CRC的情况
        s32Ret = setCusDataValue(offset + u32Length, 2, (XBH_VOID *)&u16Crc16Value);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("writeDataToCusdata error\n");
            free(u8Buff);
            u8Buff = NULL;
            return XBH_FAILURE;
        }
        memcpy(data, u8Buff, u32Length);
    }
    free(u8Buff);
    u8Buff = NULL;
    return s32Ret;
}
//======================================================================================================private func==================================================================================================
/**
 * Write data to cusdata partition
 *
 * offset: The address in cusdata partition
 * size: The size of writing data
 * buff: The writing data buffer
 */
XBH_S32 XbhPartitionData::setCusDataValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = 0;
    if (mCusDataPath.empty()) {
        XLOGE("mCusDataPath is empty, please set this path first \n");
        return XBH_FAILURE;
    }
    fd = open(mCusDataPath.c_str(), O_SYNC | O_WRONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open cusdata partition fail. fd: %d\n", fd);
        return XBH_FAILURE;
    }

    s32Ret = pwrite(fd, pBuff, u32Size, u32Offset);
    if (s32Ret == -1)
    {
        XLOGE("Write cusdata partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != u32Size)
    {
        XLOGE("Write cusdata data size issue: s32Ret = %d, u32Size = %d\n",s32Ret, u32Size);
        close(fd);
        return XBH_FAILURE;
    }
    close(fd);

    return XBH_SUCCESS;
}

/**
 * Read data from cusdata partiton
 *
 * offset: The address in cusdata partition
 * size: The size of writing data
 * buff: The writing data buffer
 */
XBH_S32 XbhPartitionData::getCusDataValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = 0;
    if (mCusDataPath.empty()) {
        XLOGE("mCusDataPath is empty, please set this path first \n");
        return XBH_FAILURE;
    }
    fd = open(mCusDataPath.c_str(), O_SYNC | O_RDONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open cusdata partition fail. fd: %d\n", fd);
        return XBH_FAILURE;
    }

    s32Ret = pread(fd, pBuff, u32Size, u32Offset);
    if (s32Ret == -1)
    {
        XLOGE("Read cusdata partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != u32Size)
    {
        XLOGE("Read cusdata data size issue: s32Ret = %d, u32Size = %d\n",s32Ret, u32Size);
        close(fd);
        return XBH_FAILURE;
    }
    close(fd);
    return XBH_SUCCESS;
}

/**
 * Write data to cusparam partition
 *
 * offset: The address in cusdata partition
 * size: The size of writing data
 * buff: The writing data buffer
 */
XBH_S32 XbhPartitionData::setCusParamValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = 0;
    if (mCusParamPath.empty()) {
        XLOGE("mCusParamPath is empty, please set this path first \n");
        return XBH_FAILURE;
    }
    fd = open(mCusParamPath.c_str(), O_SYNC | O_WRONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open cusparam partition fail. fd: %d\n", fd);
        return XBH_FAILURE;
    }

    s32Ret = pwrite(fd, pBuff, u32Size, u32Offset);
    if (s32Ret == -1)
    {
        XLOGE("Write cusparam partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != u32Size)
    {
        XLOGE("Write cusparam data size issue: s32Ret = %d, u32Size = %d\n",s32Ret, u32Size);
        close(fd);
        return XBH_FAILURE;
    }
    close(fd);

    return XBH_SUCCESS;
}

/**
 * Read data from cusparam partiton
 *
 * offset: The address in cusparam partition
 * size: The size of writing data
 * buff: The writing data buffer
 */
XBH_S32 XbhPartitionData::getCusParamValue(XBH_U32 u32Offset, XBH_U32 u32Size, XBH_VOID * pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = 0;
    if (mCusParamPath.empty()) {
        XLOGE("mCusParamPath is empty, please set this path first \n");
        return XBH_FAILURE;
    }
    fd = open(mCusParamPath.c_str(), O_SYNC | O_RDONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open cusparam partition fail. fd: %d\n", fd);
        return XBH_FAILURE;
    }

    s32Ret = pread(fd, pBuff, u32Size, u32Offset);
    if (s32Ret == -1)
    {
        XLOGE("Read cusparam partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != u32Size)
    {
        XLOGE("Read cusparam data size issue: s32Ret = %d, u32Size = %d\n",s32Ret, u32Size);
        close(fd);
        return XBH_FAILURE;
    }
    close(fd);
    return XBH_SUCCESS;
}



