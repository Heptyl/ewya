/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_DB_DEF__
#define __HI_MW_DB_DEF__

#include "hi_type.h"
#include "hi_mw_common.h"
//#include "hi_mw_convert.h"



HI_VOID UpdateFacPanelDef(HI_MW_FACTORY_PANEL_S &stFactoryPanelData);
HI_VOID UpdateFacCtrlDef(HI_MW_FACTORY_CTRL_S &stFactoryCtrlData);
HI_VOID UpdateFacPanelIntAttrDef(HI_MW_FACTORY_PANEL_INTF_ATTR_S &stFactoryPanelIntfAttr);
HI_VOID UpdateFacLvdsDef(HI_MW_FACTORY_LVDS_S &stFactoryLvdsData);
HI_VOID UpdateFacPowerDef(HI_MW_FACTORY_POWER_S &stFactoryPowerData);
HI_VOID UpdateFacDdrDef(HI_MW_FACTORY_DDR_S &stFactoryDdrData);
HI_VOID UpdateFacEmmcDef(HI_MW_FACTORY_EMMC_S &stFactoryEmmcData);
HI_VOID UpdateFacVboDef(HI_MW_FACTORY_VBO_S &stFactoryVboData);
HI_VOID UpdateFacGmacDef(HI_MW_FACTORY_GMAC_S &stFactoryGmacData);
HI_VOID UpdateFacUsbDef(HI_MW_FACTORY_USB_S &stFactoryUsbData);
HI_VOID UpdateFacEmiDef(HI_MW_FACTORY_EMI_S &stFactoryEmiData);
HI_VOID UpdateFacPQDef(HI_MW_FACTORY_PQ_S &stFactoryPQData);
HI_VOID UpdateFacDeviceDef(HI_MW_FACTORY_DEVICE_S &stFactoryDeviceData);
HI_VOID UpdateFacCalibInfoDef(HI_MW_CALIBRATION_INFO_S *astCalibInfoData, HI_U32 size);
HI_VOID UpdateFacClrTmpDef(HI_MW_SRC_E enSource,
                           HI_MW_COLOR_TEMP_DATA_S(&astClrTmpData)[HI_MW_VIDEO_DEFINITION_BUTT][HI_MW_COLORTEMP_BUTT]);
HI_VOID UpdateFacNlaDef(HI_MW_NLA_INFO_S *astNlaInfoData, HI_U32 size);
HI_VOID UpdateFacPqSharpGainDef(HI_MW_FACTORY_SHARP_GAIN_S &stSharpGain);
HI_VOID UpdateFacPqAcmGainDef(HI_MW_FACTORY_ACM_GAIN_S &stAcmGain);
HI_VOID UpdateFacPqDciGainDef(HI_MW_FACTORY_DCI_GAIN_S &stDciGain);
HI_VOID UpdateFacPqGainDef(HI_MW_SRC_E enSource, HI_MW_FACTORY_PQ_GAIN_S &stFacPqGain);
HI_VOID UpdateFacAtvOverScanDef(HI_MW_RECT_S *astAtvOverScanData, HI_U32 size);
HI_VOID UpdateFacCvbsOverScanDef(HI_MW_RECT_S *astCvbsOverScanData, HI_U32 size);
HI_VOID UpdateFacScartOverScanDef(HI_MW_RECT_S astScartOverScanData[], HI_U32 size);
HI_VOID UpdateFacHdmiOverScanDef(HI_MW_RECT_S *astHdmiOverScanData, HI_U32 length);
HI_VOID UpdateFacYpbprOverScanDef(HI_MW_RECT_S *astYpbprOverScanData, HI_U32 size);
HI_VOID UpdateFacDtvOverScanDef(HI_MW_RECT_S astDtvOverScanData[], HI_U32 length);
HI_VOID UpdateFacMediaOverScanDef(HI_MW_RECT_S *astMediaOverScanData, HI_U32 size);

HI_VOID UpdatePicDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_S &stPictureData);
HI_VOID UpdatePicPQDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_PQ_S &stPicPQData);
HI_VOID UpdatePicDispDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_DISP_S &stPicDispData);
HI_VOID UpdatePicVideoDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData);
HI_VOID UpdatePicVideoDefSubOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData);
HI_VOID UpdatePicVideoDefSubTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData);
HI_VOID UpdatePicVideoDefSubThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData);
HI_VOID UpdatePicVideoDefSubFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_VIDEO_S &stPicVideoData);


HI_VOID UpdatePicEfDef(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData);
HI_VOID UpdatePicEfDefSubOne(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData);
HI_VOID UpdatePicEfDefSubTwo(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData);
HI_VOID UpdatePicEfDefSubThree(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData);
HI_VOID UpdatePicEfDefSubFour(HI_MW_SRC_E enSource, HI_MW_PICTURE_EF_S &stPicEfData);

HI_VOID UpdatePicImageDef(HI_MW_SRC_E enSource, HI_MW_IMAGE_S *astPicImageData, HI_U32 size);
HI_VOID UpdatePicPanelDef(HI_MW_PICTURE_PANEL_S *stPicPanelData);
HI_VOID UpdatePicPqHdrFixModeDef(HI_MW_PQ_HDR_FIXMODE_S &stpicHdrFixmodeData);
HI_VOID UpdateLocalDimDataDef(HI_MW_SRC_E enSource, HI_MW_LOCALDIM_S &stLocalDimData);
HI_VOID UpdateLocalDimDataDefSubOne(
    HI_MW_SRC_E enSource, HI_MW_LOCALDIM_S &stLocalDimData, Array<HI_S32> &localDimming);
HI_VOID UpdateLocalDimDataDefSubTwo(
    HI_MW_SRC_E enSource, HI_MW_LOCALDIM_S &stLocalDimData, Array<HI_S32> &localDimming);

HI_VOID UpdateAudSrsDef(HI_MW_AUDIO_SRS_S &stAudioSrsData);
HI_VOID UpdateAudSrsDefSubOne(HI_MW_AUDIO_SRS_S &stAudioSrsData);
HI_VOID UpdateAudSrsDefSubTwo(HI_MW_AUDIO_SRS_S &stAudioSrsData);
HI_VOID UpdateAudSrsDefSubThree(HI_MW_AUDIO_SRS_S &stAudioSrsData);
HI_VOID UpdateAudSrsDefSubFour(HI_MW_AUDIO_SRS_S &stAudioSrsData);

HI_VOID UpdateAudDbxDef(HI_MW_AUDIO_DBX_S &stAudioDbxData);
HI_VOID UpdateAudDbx2Def(HI_MW_AUDIO_DBX2_S &stAudioDbx2Data);
HI_VOID UpdateAudDapDef(HI_MW_AUDIO_DAP_S &stAudioDapData);
HI_VOID UpdateAudCtrlDef(HI_MW_AUDIO_CTRL_S &stAudioCtrlData);
HI_VOID UpdateAudBaseDef(HI_MW_AUDIO_BASE_S(&stAudioBaseData)[HI_MW_AUDIO_CHANNEL_BUTT]);
HI_VOID UpdateAudAmpDef(HI_MW_AUDIO_AMP_S &stAudioAmpData);
HI_VOID UpdateAudAvcDef(HI_MW_AVC_ATTR_S &stAvcAttrData);
HI_VOID UpdateAudDrcDef(HI_MW_DRC_ATTR_S(&stDrcAttrData)[HI_MW_AUDIO_CHANNEL_BUTT]);
HI_VOID UpdateAudSndDef(HI_MW_SND_MODE_ATTR_S(&astAudioSoundData)[HI_MW_SNDMODE_BUTT]);
HI_VOID UpdateAudPrecigainDef(HI_MW_SND_PRECIGAIN_ATTR_S &stAudioPrecigainData);
HI_VOID UpdateAudPeqQValueDef(HI_S32 pstPeqQValue[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_VALUE_BUTT]);
HI_VOID UpdateAudPeqGainDef(HI_S32 pstPeqGain[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_VALUE_BUTT]);
HI_VOID UpdateAudSws3DDef(HI_MW_AUDIO_SWS3D_S &stAudioSws3DData);
HI_VOID UpdateAudSws3DDefSubOne(HI_MW_AUDIO_SWS3D_S &stAudioSws3DData);
HI_VOID UpdateAudSws3DDefSubTwo(HI_MW_AUDIO_SWS3D_S &stAudioSws3DData);


HI_VOID UpdateSysDef(HI_MW_SYSTEM_S &stSystemData);
HI_VOID UpdateSysMainSrcDef(HI_MW_SRC_E &enMainSrc);
HI_VOID UpdateSysVideo3DDef(HI_MW_3D_MODE_S &st3DModeData);
HI_VOID UpdateSysPCTimingDef(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size);
HI_VOID UpdateSysPCTimingDefSubOne(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0);
HI_VOID UpdateSysPCTimingDefSubTwo(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0);
HI_VOID UpdateSysPCTimingDefSubThree(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0);
HI_VOID UpdateSysPCTimingDefSubFour(HI_MW_PC_TIMMING_S *astPcTimmingData, HI_U32 size, Array<HI_S32> timing0);


HI_VOID UpdateSystemLockDef(HI_MW_LOCK_S &stLockData);
HI_VOID UpdateSystemCcDef(HI_MW_CC_S &stCcData);
HI_VOID UpdateSystemWakeupDef(HI_MW_SYSTEM_WAKEUP_ENABLE_S &stSystemWakeupData);
HI_VOID UpdateCecDef(HI_MW_CEC_MENU_S &stCecMenuState);
HI_VOID UpdateTifCecOption(HI_MW_TIF_CEC_OPTION_S &stTifCecState);
HI_VOID UpdateSelectSource(HI_MW_SRC_E &enCecSelectSource);

HI_VOID UpdateChannleCurProNumDef(HI_U8 &u8CurrentProgramNum);
HI_VOID UpdateChannleFreezeDef(HI_U8 &u8FreezeMode);
HI_VOID UpdateChannleProgInfoDef(HI_MW_ATV_PROG_S &stProgInfo);
HI_VOID UpdateChannleNstdInfoDef(HI_MW_ATV_NSTD_S &stNstdInfo);
HI_VOID UpdateNtscScanDef(HI_MW_NTSC_SCAN_S &stNtscScan);
HI_VOID UpdateHdmiTxDataDef(HI_MW_HDMITX_DATA_S &stHdmiTxData);

HI_VOID RestoreDbAttr(HI_U32 u32Id, HI_U8 *pData);
HI_VOID RestoreDbAttrSubOne(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubTwo(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubThree(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubFour(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubFive(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubSix(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubSeven(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);
HI_VOID RestoreDbAttrSubEight(HI_U32 u32Id, HI_U8 *pData, HI_MW_SRC_E &enSource);

HI_VOID GetLengthFromID(HI_U32 u32Id, HI_U32 &u32Length);
HI_VOID GetLengthFromIDSubOne(HI_U32 u32Id, HI_U32 &u32Length);
HI_VOID GetLengthFromIDSubTwo(HI_U32 u32Id, HI_U32 &u32Length);
HI_VOID GetLengthFromIDSubThree(HI_U32 u32Id, HI_U32 &u32Length);
HI_VOID GetLengthFromIDSubFour(HI_U32 u32Id, HI_U32 &u32Length);
HI_VOID GetLengthFromIDSubFive(HI_U32 u32Id, HI_U32 &u32Length);
HI_VOID GetLengthFromIDSubSix(HI_U32 u32Id, HI_U32 &u32Length);

#endif /* __HI_MW_DB_DEF__ */
