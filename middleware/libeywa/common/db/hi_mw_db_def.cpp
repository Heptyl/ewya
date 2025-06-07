/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_db_def"

#include "hi_mw_db_def.h"
//#include "hi_mw_convert.h"
#include "hi_mw_cfg.h"

#include "hi_mw_dbo.h"

static const HI_U32 ATTR_BW = 16;

HI_VOID UpdateFacPanelDef(HI_MW_FACTORY_PANEL_S &stFactoryPanelData)
{

}
HI_VOID UpdateFacCtrlDef(HI_MW_FACTORY_CTRL_S &stFactoryCtrlData)
{

}

HI_VOID UpdateFacPanelIntAttrDef(HI_MW_FACTORY_PANEL_INTF_ATTR_S &stFactoryPanelIntfAttr)
{

}

HI_VOID UpdateFacLvdsDef(HI_MW_FACTORY_LVDS_S &stFactoryLvdsData)
{

}
HI_VOID UpdateFacPowerDef(HI_MW_FACTORY_POWER_S &stFactoryPowerData)
{

}

HI_VOID UpdateFacDdrDef(HI_MW_FACTORY_DDR_S &stFactoryDdrData)
{

}

HI_VOID UpdateFacEmmcDef(HI_MW_FACTORY_EMMC_S &stFactoryEmmcData)
{

}

HI_VOID UpdateFacVboDef(HI_MW_FACTORY_VBO_S &stFactoryVboData)
{

}
HI_VOID UpdateFacGmacDef(HI_MW_FACTORY_GMAC_S &stFactoryGmacData)
{

}
HI_VOID UpdateFacUsbDef(HI_MW_FACTORY_USB_S &stFactoryUsbData)
{

}
HI_VOID UpdateFacEmiDef(HI_MW_FACTORY_EMI_S &stFactoryEmiData)
{

}

HI_VOID UpdateFacPQDef(HI_MW_FACTORY_PQ_S &stFactoryPQData)
{

}
HI_VOID UpdateFacDeviceDef(HI_MW_FACTORY_DEVICE_S &stFactoryDeviceData)
{

}
HI_VOID UpdateFacCalibInfoDef(HI_MW_CALIBRATION_INFO_S *astCalibInfoData, HI_U32 size)
{

}

static HI_VOID SetClrTmpData(const string &strColorSource, HI_MW_COLOR_TEMP_DATA_S(&astClrTmpData)[HI_MW_VIDEO_DEFINITION_BUTT][HI_MW_COLORTEMP_BUTT])
{

}

static HI_VOID INIClrTmpOne(HI_MW_SRC_E enSource, HI_MW_COLOR_TEMP_DATA_S(&astClrTmpData)[HI_MW_VIDEO_DEFINITION_BUTT][HI_MW_COLORTEMP_BUTT])
{

}

static HI_VOID INIClrTmpTwo(HI_MW_SRC_E enSource, HI_MW_COLOR_TEMP_DATA_S(&astClrTmpData)[HI_MW_VIDEO_DEFINITION_BUTT][HI_MW_COLORTEMP_BUTT])
{

}

HI_VOID UpdateFacClrTmpDef(HI_MW_SRC_E enSource, HI_MW_COLOR_TEMP_DATA_S(&astClrTmpData)[HI_MW_VIDEO_DEFINITION_BUTT][HI_MW_COLORTEMP_BUTT])
{

}

HI_VOID UpdateFacNlaDef(HI_MW_NLA_INFO_S *astNlaInfoData, HI_U32 size)
{

}

HI_VOID UpdateFacPqSharpGainDef(HI_MW_FACTORY_SHARP_GAIN_S &stSharpGain)
{

}
HI_VOID UpdateFacPqAcmGainDef(HI_MW_FACTORY_ACM_GAIN_S &stAcmGain)
{

}
HI_VOID UpdateFacPqDciGainDef(HI_MW_FACTORY_DCI_GAIN_S &stDciGain)
{

}

static HI_VOID INIPQGainOneForAtv(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainOne(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainTwoForVGA(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainTwo(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainThreeForHdmi(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainThree(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainFourForDvbc(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainFour(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainFiveForDvbt(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainFive(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainSixForDvbs(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainSix(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

static HI_VOID INIPQGainDefault(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

HI_VOID UpdateFacPqGainDef(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain)
{

}

HI_VOID UpdateFacAtvOverScanDef(HI_MW_RECT_S *astAtvOverScanData, HI_U32 size)
{

}
HI_VOID UpdateFacCvbsOverScanDef(HI_MW_RECT_S *astCvbsOverScanData, HI_U32 size)
{

}

HI_VOID UpdateFacScartHDOverScanDef(HI_MW_RECT_S astScartOverScanData[], HI_U32 size)
{

}

HI_VOID UpdateFacScartOverScanDef(HI_MW_RECT_S astScartOverScanData[], HI_U32 size)
{

}

static HI_VOID INIHdmiOverScanOne(HI_MW_RECT_S *astHdmiOverScanData)
{

}

static HI_VOID INIHdmiOverScanTwo(HI_MW_RECT_S *astHdmiOverScanData)
{

}

static HI_VOID INIHdmiOverScanThree(HI_MW_RECT_S *astHdmiOverScanData)
{

}

static HI_VOID INIHdmiOverScanFour(HI_MW_RECT_S *astHdmiOverScanData)
{

}

HI_VOID UpdateFacHdmiOverScanDef(HI_MW_RECT_S *astHdmiOverScanData, HI_U32 length)
{

}

static HI_VOID INIYpbprOverScanOne(HI_MW_RECT_S *astYpbprOverScanData, HI_U32 size)
{

}

static HI_VOID INIYpbprOverScanTwo(HI_MW_RECT_S *astYpbprOverScanData, HI_U32 size)
{

}

static HI_VOID INIYpbprOverScanThree(HI_MW_RECT_S *astYpbprOverScanData, HI_U32 size)
{

}

static HI_VOID INIYpbprOverScanFour(HI_MW_RECT_S *astYpbprOverScanData, HI_U32 size)
{

}

HI_VOID UpdateFacYpbprOverScanDef(HI_MW_RECT_S *astYpbprOverScanData, HI_U32 size)
{

}

static HI_VOID INIDtvOverScanOne(HI_MW_RECT_S astDtvOverScanData[], HI_U32 length)
{

}

static HI_VOID INIDtvOverScanTwo(HI_MW_RECT_S astDtvOverScanData[], HI_U32 length)
{

}

static HI_VOID INIDtvOverScanThree(HI_MW_RECT_S astDtvOverScanData[], HI_U32 length)
{

}

static HI_VOID INIDtvOverScanFour(HI_MW_RECT_S astDtvOverScanData[], HI_U32 length)
{

}

HI_VOID UpdateFacDtvOverScanDef(HI_MW_RECT_S astDtvOverScanData[], HI_U32 length)
{

}

static HI_VOID INIMediaOverScanOne(HI_MW_RECT_S *astMediaOverScanData)
{

}

static HI_VOID INIMediaOverScanTwO(HI_MW_RECT_S *astMediaOverScanData)
{

}

static HI_VOID INIMediaOverScanThree(HI_MW_RECT_S *astMediaOverScanData)
{

}

static HI_VOID INIMediaOverScanFour(HI_MW_RECT_S *astMediaOverScanData)
{

}

HI_VOID UpdateFacMediaOverScanDef(HI_MW_RECT_S *astMediaOverScanData, HI_U32 size)
{

}

static HI_VOID INIPicDemoOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_S &stPictureData)
{

}

static HI_VOID INIPicDemoTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_S &stPictureData)
{

}

static HI_VOID INIPicDemoThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_S &stPictureData)
{

}

static HI_VOID INIPicDemoFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_S &stPictureData)
{

}

HI_VOID UpdatePicDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_S &stPictureData)
{

}

static HI_VOID INIPicPQOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData)
{

}

static HI_VOID INIPicPQTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData)
{

}

static HI_VOID INIPicPQThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData)
{

}

static HI_VOID INIPicPQFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData)
{

}

static HI_VOID INIPicPQFive(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData)
{

}

HI_VOID UpdatePicPQDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData)
{

}

static HI_VOID INIPicDispOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

static HI_VOID INIPicDispTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

static HI_VOID INIPicDispThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

static HI_VOID INIPicDispFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

static HI_VOID INIPicDispFive(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

static HI_VOID INIPicDispSix(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

static HI_VOID INIPicDispDefault(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

HI_VOID UpdatePicDispDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData)
{

}

HI_VOID UpdatePicVideoDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData)
{

}

HI_VOID UpdatePicVideoDefSubOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData)
{

}

HI_VOID UpdatePicVideoDefSubTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData)
{

}

HI_VOID UpdatePicVideoDefSubThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData)
{

}

HI_VOID UpdatePicVideoDefSubFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData)
{

}


HI_VOID UpdatePicEfDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData)
{

}

HI_VOID UpdatePicEfDefSubOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData)
{

}

HI_VOID UpdatePicEfDefSubTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData)
{

}

HI_VOID UpdatePicEfDefSubThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData)
{

}

HI_VOID UpdatePicEfDefSubFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData)
{

}

HI_VOID UpdatePicImageDef(HI_MW_SRC_E enSource, HI_MW_IMAGE_S *astPicImageData, HI_U32 size)
{

}

HI_VOID UpdatePicPanelDef(HI_MW_PICTURE_PANEL_S *stPicPanelData)
{

}

HI_VOID UpdatePicPqHdrFixModeDef(HI_MW_PQ_HDR_FIXMODE_S &stpicHdrFixmodeData)
{

}

HI_VOID UpdateLocalDimDataDef(HI_MW_SRC_E enSource, HI_MW_LOCALDIM_S &stLocalDimData)
{

}

HI_VOID UpdateLocalDimDataDefSubOne(HI_MW_SRC_E enSource, HI_MW_LOCALDIM_S &stLocalDimData, Array<HI_S32> &localDimming)
{

}

HI_VOID UpdateLocalDimDataDefSubTwo(HI_MW_SRC_E enSource, HI_MW_LOCALDIM_S &stLocalDimData, Array<HI_S32> &localDimming)
{

}

HI_VOID UpdateAudSrsDef(HI_MW_AUDIO_SRS_S &stAudioSrsData)
{

}

HI_VOID UpdateAudSrsDefSubOne(HI_MW_AUDIO_SRS_S &stAudioSrsData)
{

}

HI_VOID UpdateAudSrsDefSubTwo(HI_MW_AUDIO_SRS_S &stAudioSrsData)
{

}

HI_VOID UpdateAudSrsDefSubThree(HI_MW_AUDIO_SRS_S &stAudioSrsData)
{

}

HI_VOID UpdateAudSrsDefSubFour(HI_MW_AUDIO_SRS_S &stAudioSrsData)
{

}

HI_VOID UpdateAudDbxDef(HI_MW_AUDIO_DBX_S &stAudioDbxData)
{

}

HI_VOID UpdateAudDbx2Def(HI_MW_AUDIO_DBX2_S &stAudioDbx2Data)
{

}

HI_VOID UpdateAudDapDef(HI_MW_AUDIO_DAP_S &stAudioDapData)
{

}

HI_VOID UpdateAudCtrlDef(HI_MW_AUDIO_CTRL_S &stAudioCtrlData)
{

}

HI_VOID UpdateAudBaseDef(HI_MW_AUDIO_BASE_S(&astAudioBaseData)[HI_MW_AUDIO_CHANNEL_BUTT])
{
}

HI_VOID UpdateAudAmpDef(HI_MW_AUDIO_AMP_S &stAudioAmpData)
{

}

HI_VOID UpdateAudAvcDef(HI_MW_AVC_ATTR_S &stAvcAttrData)
{

}

HI_VOID UpdateAudDrcDef(HI_MW_DRC_ATTR_S(&stDrcAttrData)[HI_MW_AUDIO_CHANNEL_BUTT])
{

}

static string GetSoundModeName(HI_MW_SNDMODE_E enSoundMode)
{
    return "user";
}

HI_VOID UpdateAudSndDef(HI_MW_SND_MODE_ATTR_S(&astAudioSoundData)[HI_MW_SNDMODE_BUTT])
{

}
HI_VOID UpdateAudPrecigainDef(HI_MW_SND_PRECIGAIN_ATTR_S &stAudioPrecigainData)
{

}

HI_VOID UpdateAudPeqQValueDef(HI_S32 pstPeqQValue[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_VALUE_BUTT])
{

}

HI_VOID UpdateAudPeqGainDef(HI_S32 pstPeqGain[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_VALUE_BUTT])
{

}

HI_VOID UpdateAudSws3DDef(HI_MW_AUDIO_SWS3D_S &stAudioSws3DData)
{

}

HI_VOID UpdateAudSws3DDefSubOne(HI_MW_AUDIO_SWS3D_S &stAudioSws3DData)
{

}

HI_VOID UpdateAudSws3DDefSubTwo(HI_MW_AUDIO_SWS3D_S &stAudioSws3DData)
{

}

HI_VOID UpdateNtscScanDef(HI_MW_NTSC_SCAN_S &stNtscScan)
{

}

HI_VOID UpdateHdmiTxDataDef(HI_MW_HDMITX_DATA_S &stHdmiTxData)
{

}

HI_VOID UpdateSysDef(HI_MW_SYSTEM_S &stSystemData)
{

}
HI_VOID UpdateSysMainSrcDef(HI_MW_SRC_E &enMainSrc)
{

}

HI_VOID UpdateSysVideo3DDef(HI_MW_3D_MODE_S &st3DModeData)
{

}

HI_VOID UpdateSysPCTimingDef(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size)
{

}

HI_VOID UpdateSysPCTimingDefSubOne(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0)
{

}

HI_VOID UpdateSysPCTimingDefSubTwo(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0)
{

}

HI_VOID UpdateSysPCTimingDefSubThree(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0)
{

}

HI_VOID UpdateSysPCTimingDefSubFour(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0)
{

}


HI_VOID UpdateSystemLockDef(HI_MW_LOCK_S &stLockData)
{

}

HI_VOID UpdateSystemCcDef(HI_MW_CC_S &stCcData)
{

}

HI_VOID UpdateSystemWakeupDef(HI_MW_SYSTEM_WAKEUP_ENABLE_S &stSystemWakeupData)
{

}

HI_VOID UpdateCecDef(HI_MW_CEC_MENU_S &stCecMenuState)
{

}

HI_VOID UpdateTifCecOption(HI_MW_TIF_CEC_OPTION_S &stTifCecState)
{

}

HI_VOID UpdateSelectSource(HI_MW_SRC_E &enCecSelectSource)
{

}

HI_VOID UpdateChannleCurProNumDef(HI_U8 &u8CurrentProgramNum)
{

}
HI_VOID UpdateChannleFreezeDef(HI_U8 &u8FreezeMode)
{

}
HI_VOID UpdateChannleProgInfoDef(HI_MW_ATV_PROG_S &stProgInfo)
{

}
HI_VOID UpdateChannleNstdInfoDef(HI_MW_ATV_NSTD_S &stNstdInfo)
{

}

HI_VOID RestoreDbAttr(HI_U32 u32Id, HI_U8 *pData)
{

}

HI_VOID RestoreDbAttrSubOnePartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubOnePartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubOne(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubTwoParA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubTwoParB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubTwo(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubThreePartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubThreePartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubThree(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubFourPartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubFourPartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubFour(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubFivePartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubFivePartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubFive(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSixPartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSixPartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSixPartC(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSix(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSevenPartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSevenPartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubSeven(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubEightPartA(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID RestoreDbAttrSubEightPartB(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}


HI_VOID RestoreDbAttrSubEight(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource)
{

}

HI_VOID GetLengthFromID(HI_U32 u32Id, HI_U32 &u32Length)
{
    if (u32Id >= HI_MW_ATTR_FAC_PANEL && u32Id <= HI_MW_ATTR_FAC_ADC) {
        GetLengthFromIDSubOne(u32Id, u32Length);
    } else if (u32Id >= HI_MW_ATTR_PQ_SHARP_GAIN && u32Id <= HI_MW_ATTR_SCART_OVERSCAN) {
        GetLengthFromIDSubTwo(u32Id, u32Length);
    } else if (u32Id >= HI_MW_ATTR_PICTURE && u32Id <= HI_MW_ATTR_PICTURE_LOCALDIM) {
        GetLengthFromIDSubThree(u32Id, u32Length);
    } else if (u32Id >= HI_MW_ATTR_AUDIO_CTRL && u32Id <= HI_MW_ATTR_AUDIO_PEQ_GAIN) {
        GetLengthFromIDSubFour(u32Id, u32Length);
    } else if (u32Id >= HI_MW_ATTR_SYSTEM && u32Id <= HI_MW_ATTR_HDMITX_ATTR) {
        GetLengthFromIDSubFive(u32Id, u32Length);
    } else {
        GetLengthFromIDSubSix(u32Id, u32Length);
    }
    return;
}

HI_VOID GetLengthFromIDSubOne(HI_U32 u32Id, HI_U32 &u32Length)
{
    switch (u32Id) {
        /* Factory Enum */
        case HI_MW_ATTR_FAC_PANEL:
            u32Length = sizeof(HI_MW_FACTORY_PANEL_S);
            break;
        case HI_MW_ATTR_FAC_CTRL:
            u32Length = sizeof(HI_MW_FACTORY_CTRL_S);
            break;
        case HI_MW_ATTR_FAC_PANEL_INTF_ATTR:
            u32Length = sizeof(HI_MW_FACTORY_PANEL_INTF_ATTR_S);
            break;
        case HI_MW_ATRR_FAC_LVDS:
            u32Length = sizeof(HI_MW_FACTORY_LVDS_S);
            break;
        case HI_MW_ATTR_FAC_POWER:
            u32Length = sizeof(HI_MW_FACTORY_POWER_S);
            break;
        case HI_MW_ATTR_FAC_DDR:
            u32Length = sizeof(HI_MW_FACTORY_DDR_S);
            break;
        case HI_MW_ATTR_FAC_EMMC:
            u32Length = sizeof(HI_MW_FACTORY_EMMC_S);
            break;
        case HI_MW_ATTR_FAC_VBO:
            u32Length = sizeof(HI_MW_FACTORY_VBO_S);
            break;
        case HI_MW_ATTR_FAC_GMAC:
            u32Length = sizeof(HI_MW_FACTORY_GMAC_S);
            break;
        case HI_MW_ATTR_FAC_USB:
            u32Length = sizeof(HI_MW_FACTORY_USB_S);
            break;
        case HI_MW_ATTR_FAC_EMI:
            u32Length = sizeof(HI_MW_FACTORY_EMI_S);
            break;
        case HI_MW_ATTR_FAC_PQ:
            u32Length = sizeof(HI_MW_FACTORY_PQ_S);
            break;
        case HI_MW_ATTR_FAC_DEVICE:
            u32Length = sizeof(HI_MW_FACTORY_DEVICE_S);
            break;
        case HI_MW_ATTR_FAC_ADC:
            u32Length = sizeof(HI_MW_CALIBRATION_INFO_S) * HI_MW_ADC_INDEX_BUTT;
            break;
        }
}

HI_VOID GetLengthFromIDSubTwo(HI_U32 u32Id, HI_U32 &u32Length)
{
    switch (u32Id) {
        /* PQ Gain */
        case HI_MW_ATTR_PQ_SHARP_GAIN:
            u32Length = sizeof(HI_MW_FACTORY_SHARP_GAIN_S);
            break;
        case HI_MW_ATTR_PQ_ACM_GAIN:
            u32Length = sizeof(HI_MW_FACTORY_ACM_GAIN_S);
            break;
        case HI_MW_ATTR_PQ_DCI_GAIN:
            u32Length = sizeof(HI_MW_FACTORY_DCI_GAIN_S);
            break;

        /* OverScan Enum */
        case HI_MW_ATTR_ATV_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_TVD_BUTT;
            break;
        case HI_MW_ATTR_CVBS_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_TVD_BUTT;
            break;
        case HI_MW_ATTR_SCART_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_SCART_BUTT;
            break;
        case HI_MW_ATTR_HDMI_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_HD_BUTT;
            break;
        case HI_MW_ATTR_YPBPR_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_HD_BUTT;
            break;
        case HI_MW_ATTR_DTV_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_HD_BUTT;
            break;
        case HI_MW_ATTR_MEDIA_OVERSCAN:
            u32Length = sizeof(HI_MW_RECT_S) * HI_MW_HD_BUTT;
            break;
        case HI_MW_ATTR_SND_SPEAKER_VOL_CURVE:
            break;
        case HI_MW_ATTR_FACTORY_PQ_GAIN:
            u32Length = sizeof(HI_MW_FACTORY_PQ_GAIN_S);
            break;
        }
}

HI_VOID GetLengthFromIDSubThree(HI_U32 u32Id, HI_U32 &u32Length)
{
    switch (u32Id) {
        /* Picture Enum */
        case HI_MW_ATTR_PICTURE:
            u32Length = sizeof(HI_MW_PICTURE_S);
            break;
        case HI_MW_ATTR_PICTURE_PQ:
            u32Length = sizeof(HI_MW_PICTURE_PQ_S);
            break;
        case HI_MW_ATTR_PICTURE_DISP:
            u32Length = sizeof(HI_MW_PICTURE_DISP_S);
            break;
        case HI_MW_ATTR_PICTURE_VIDEO:
            u32Length = sizeof(HI_MW_PICTURE_VIDEO_S);
            break;
        case HI_MW_ATTR_PICTURE_EF:
            u32Length = sizeof(HI_MW_PICTURE_EF_S);
            break;
        case HI_MW_ATTR_PICTURE_IMAGE:
            u32Length = sizeof(HI_MW_IMAGE_S) * HI_MW_PICMODE_BUTT;
            break;
        case HI_MW_ATTR_PICTURE_PANEL:
            u32Length = sizeof(HI_MW_PICTURE_PANEL_S);
            break;
        case HI_MW_ATTR_PICTURE_HDR_FIXMODE:
            u32Length = sizeof(HI_MW_PQ_HDR_FIXMODE_S);
            break;
        case HI_MW_ATTR_PICTURE_LOCALDIM:
            u32Length = sizeof(HI_MW_LOCALDIM_S);
            break;
        }
}

HI_VOID GetLengthFromIDSubFour(HI_U32 u32Id, HI_U32 &u32Length)
{
    switch (u32Id) {
        /* Audio Enum */
        case HI_MW_ATTR_AUDIO_SRS:
            u32Length = sizeof(HI_MW_AUDIO_SRS_S);
            break;
        case HI_MW_ATTR_AUDIO_DBX:
            u32Length = sizeof(HI_MW_AUDIO_DBX_S);
            break;
        case HI_MW_ATTR_AUDIO_CTRL:
            u32Length = sizeof(HI_MW_AUDIO_CTRL_S);
            break;
        case HI_MW_ATTR_AUDIO_BASE:
            u32Length = sizeof(HI_MW_AUDIO_BASE_S) * HI_MW_AUDIO_CHANNEL_BUTT;
            break;
        case HI_MW_ATTR_AUDIO_AMP:
            u32Length = sizeof(HI_MW_AUDIO_AMP_S);
            break;
        case HI_MW_ATTR_AUDIO_AVC:
            u32Length = sizeof(HI_MW_AVC_ATTR_S);
            break;
        case HI_MW_ATTR_AUDIO_DRC:
            u32Length = sizeof(HI_MW_DRC_ATTR_S) * HI_MW_AUDIO_CHANNEL_BUTT;
            break;
        case HI_MW_ATTR_AUDIO_SOUND_MODE:
            u32Length = sizeof(HI_MW_SND_MODE_ATTR_S) * HI_MW_SNDMODE_BUTT;
            break;
        case HI_MW_ATTR_AUDIO_PRECIGAIN:
            u32Length = sizeof(HI_MW_SND_PRECIGAIN_ATTR_S);
            break;
        case HI_MW_ATTR_AUDIO_PEQ_QVALUE:
            u32Length = sizeof(HI_S32) * HI_MW_AUDIO_CHANNEL_BUTT * HI_MW_AUDIO_PEQ_VALUE_BUTT;
            break;
        case HI_MW_ATTR_AUDIO_PEQ_GAIN:
            u32Length = sizeof(HI_S32) * HI_MW_AUDIO_CHANNEL_BUTT * HI_MW_AUDIO_PEQ_GAIN_BUTT;
            break;
        case HI_MW_ATTR_AUDIO_SWS3D:
            u32Length = sizeof(HI_MW_AUDIO_SWS3D_S);
            break;
        case HI_MW_ATTR_AUDIO_DBX2:
            u32Length = sizeof(HI_MW_AUDIO_DBX2_S);
            break;
        case HI_MW_ATTR_AUDIO_DAP:
            u32Length = sizeof(HI_MW_AUDIO_DAP_S);
            break;
        }
}

HI_VOID GetLengthFromIDSubFive(HI_U32 u32Id, HI_U32 &u32Length)
{
    switch (u32Id) {
        /* System && 3D Enum */
        case HI_MW_ATTR_SYSTEM:
            u32Length = sizeof(HI_MW_SYSTEM_S);
            break;
        case HI_MW_ATTR_SYSTEM_MAINSRC:
            u32Length = sizeof(HI_MW_SRC_E);
            break;
        case HI_MW_ATTE_3D_MODE:
            u32Length = sizeof(HI_MW_3D_MODE_S);
            break;
        case HI_MW_ATTR_PC_TIMMING:
            u32Length = sizeof(HI_MW_PC_TIMMING_S) * PC_MODE_SAVE_MAX;
            break;
        case HI_MW_ATTR_SYSTEM_LOCK:
            u32Length = sizeof(HI_MW_LOCK_S);
            break;
        case HI_MW_ATTR_SYSTEM_CC:
            u32Length = sizeof(HI_MW_CC_S);
            break;
        case HI_MW_ATTR_SYSTEM_WAKEUP:
            u32Length = sizeof(HI_MW_SYSTEM_WAKEUP_ENABLE_S);
            break;
        /* CEC Enum */
        case HI_MW_ATTR_CEC_MENU:
            u32Length = sizeof(HI_MW_CEC_MENU_S);
            break;

        /* NTSC scan Enum */
        case HI_MW_ATTR_ATV_NTSC_SCAN:
            u32Length = sizeof(HI_MW_NTSC_SCAN_S);
            break;

        /* HDMITX Enum */
        case HI_MW_ATTR_HDMITX_ATTR:
            u32Length = sizeof(HI_MW_HDMITX_DATA_S);
            break;
        }
}

HI_VOID GetLengthFromIDSubSix(HI_U32 u32Id, HI_U32 &u32Length)
{
    switch (u32Id) {
        /* ATV Enum */
        case HI_MW_ATTR_ATV_CURRENT_PROG:
            u32Length = sizeof(HI_U8);
            break;
        case HI_MW_ATTR_ATV_FREEZE_MODE:
            u32Length = sizeof(HI_U8);
            break;
        case HI_MW_ATTR_ATV_INFO:
            u32Length = sizeof(HI_MW_ATV_PROG_S);
            break;
        case HI_MW_ATTR_ATV_NSTD:
            u32Length = sizeof(HI_MW_ATV_NSTD_S);
            break;
        case HI_MW_ATTR_FAC_COLORTEMP:
            u32Length = sizeof(HI_MW_COLOR_TEMP_DATA_S) * HI_MW_VIDEO_DEFINITION_BUTT * HI_MW_COLORTEMP_BUTT;
            break;
        case HI_MW_ATTR_FAC_NLA_INFO:
            u32Length = sizeof(HI_MW_NLA_INFO_S) * HI_MW_NLA_BUTT;
            break;
        case HI_MW_ATTR_TIF_CEC_OPTION:
            u32Length = sizeof(HI_MW_TIF_CEC_OPTION_S);
            break;
        case HI_MW_ATTR_AUDIO_SRS:
            u32Length = sizeof(HI_MW_AUDIO_SRS_S);
            break;
        case HI_MW_ATTR_AUDIO_DBX:
            u32Length = sizeof(HI_MW_AUDIO_DBX_S);
            break;
        case HI_MW_ATTR_AUDIO_SWS3D:
            u32Length = sizeof(HI_MW_AUDIO_SWS3D_S);
            break;
        case HI_MW_ATTR_AUDIO_DBX2:
            u32Length = sizeof(HI_MW_AUDIO_DBX2_S);
            break;
        case HI_MW_ATTR_AUDIO_DAP:
            u32Length = sizeof(HI_MW_AUDIO_DAP_S);
            break;
        default:
            LOGE("data type erro ,u32Id:0x%x", u32Id);
        }
}

