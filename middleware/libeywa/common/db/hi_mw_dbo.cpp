/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_dbo"

#include "hi_mw_dbo.h"
#include "hi_mw_log.h"
#include "hi_mw_error.h"
#include "hi_mw_cfg.h"
#include "hi_mw_db_def.h"
//#include "hi_mw_logic_scan_define.h"
//#include "hi_mw_convert.h"


static Array<HI_U8> g_supportSrcList = GetAllBoardSourceList();

static HI_VOID InsertUsrDefaultOne(HiDBO *pDbo, HiDB *pDb)
{
    /* create record of picture in db */
    pDb->CreateRecord(HI_MW_ATTR_PICTURE);
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_PQ);
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_DISP);
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_VIDEO);
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_EF);
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_IMAGE);
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_PANEL);

    /* insert default data of picture */
    HI_MW_PICTURE_S stPictureData;
    HI_MW_PICTURE_PQ_S stPicPQData;
    HI_MW_PICTURE_DISP_S stPicDispData;
    HI_MW_PICTURE_VIDEO_S stPicVideoData;
    HI_MW_PICTURE_EF_S stPicEfData;
    HI_MW_IMAGE_S astPicImageData[HI_MW_PICMODE_BUTT];
    HI_MW_PICTURE_PANEL_S stPicPanelData;

    for (HI_U8 i = 0; i < g_supportSrcList.length(); i++) {
        UpdatePicDef((HI_MW_SRC_E)g_supportSrcList[i], stPictureData);
        UpdatePicPQDef((HI_MW_SRC_E)g_supportSrcList[i], stPicPQData);
        UpdatePicDispDef((HI_MW_SRC_E)g_supportSrcList[i], stPicDispData);
        UpdatePicVideoDef((HI_MW_SRC_E)g_supportSrcList[i], stPicVideoData);
        UpdatePicEfDef((HI_MW_SRC_E)g_supportSrcList[i], stPicEfData);
        UpdatePicImageDef((HI_MW_SRC_E)g_supportSrcList[i], astPicImageData, HI_MW_PICMODE_BUTT);

        pDb->Insert(HI_MW_ATTR_PICTURE, (HI_U8 *)&stPictureData, sizeof(stPictureData));
        pDb->Insert(HI_MW_ATTR_PICTURE_PQ, (HI_U8 *)&stPicPQData, sizeof(stPicPQData));
        pDb->Insert(HI_MW_ATTR_PICTURE_DISP, (HI_U8 *)&stPicDispData, sizeof(stPicDispData));
        pDb->Insert(HI_MW_ATTR_PICTURE_VIDEO, (HI_U8 *)&stPicVideoData, sizeof(stPicVideoData));
        pDb->Insert(HI_MW_ATTR_PICTURE_EF, (HI_U8 *)&stPicEfData, sizeof(stPicEfData));
        pDb->Insert(HI_MW_ATTR_PICTURE_IMAGE, (HI_U8 *)&astPicImageData, sizeof(astPicImageData));
    }

    UpdatePicPanelDef(&stPicPanelData);
    pDb->Insert(HI_MW_ATTR_PICTURE_PANEL, (HI_U8 *)&stPicPanelData, sizeof(stPicPanelData));

    pDbo->Attach(HI_MW_ATTR_PICTURE, pDb);
    pDbo->Attach(HI_MW_ATTR_PICTURE_PQ, pDb);
    pDbo->Attach(HI_MW_ATTR_PICTURE_DISP, pDb);
    pDbo->Attach(HI_MW_ATTR_PICTURE_VIDEO, pDb);
    pDbo->Attach(HI_MW_ATTR_PICTURE_EF, pDb);
    pDbo->Attach(HI_MW_ATTR_PICTURE_IMAGE, pDb);
    pDbo->Attach(HI_MW_ATTR_PICTURE_PANEL, pDb);
}


static HI_VOID InsertUsrDefaultTwo(HiDBO *pDbo, HiDB *pDb)
{
    /* create record of PQ HDR FixMode */
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_HDR_FIXMODE);

    /* create record of LocalDimming data in db */
    pDb->CreateRecord(HI_MW_ATTR_PICTURE_LOCALDIM);

    /* create record of audio in db */
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_SRS);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_DBX);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_CTRL);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_BASE);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_AMP);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_AVC);

    HI_MW_PQ_HDR_FIXMODE_S stpicHdrFixmodeData;
    UpdatePicPqHdrFixModeDef(stpicHdrFixmodeData);

    pDb->Insert(HI_MW_ATTR_PICTURE_HDR_FIXMODE, (HI_U8 *)&stpicHdrFixmodeData, sizeof(stpicHdrFixmodeData));
    pDbo->Attach(HI_MW_ATTR_PICTURE_HDR_FIXMODE, pDb);

    HI_MW_LOCALDIM_S stLocalDimData;
    for (HI_U8 i = 0; i < g_supportSrcList.length(); i++) {
        UpdateLocalDimDataDef((HI_MW_SRC_E)g_supportSrcList[i], stLocalDimData);
        pDb->Insert(HI_MW_ATTR_PICTURE_LOCALDIM, (HI_U8 *)&stLocalDimData, sizeof(HI_MW_LOCALDIM_S));
    }

    pDbo->Attach(HI_MW_ATTR_PICTURE_LOCALDIM, pDb);

    HI_MW_AUDIO_SRS_S stAudioSrsData;
    HI_MW_AUDIO_DBX_S stAudioDbxData;
    HI_MW_AUDIO_CTRL_S stAudioCtrlData;
    HI_MW_AUDIO_BASE_S stAudioBaseData[HI_MW_AUDIO_CHANNEL_BUTT];
    HI_MW_AUDIO_AMP_S stAudioAmpData;
    HI_MW_AVC_ATTR_S stAvcAttrData;

    UpdateAudSrsDef(stAudioSrsData);
    UpdateAudDbxDef(stAudioDbxData);
    UpdateAudCtrlDef(stAudioCtrlData);
    UpdateAudBaseDef(stAudioBaseData);
    UpdateAudAmpDef(stAudioAmpData);
    UpdateAudAvcDef(stAvcAttrData);

    pDb->Insert(HI_MW_ATTR_AUDIO_SRS, (HI_U8 *)&stAudioSrsData, sizeof(stAudioSrsData));
    pDb->Insert(HI_MW_ATTR_AUDIO_DBX, (HI_U8 *)&stAudioDbxData, sizeof(stAudioDbxData));
    pDb->Insert(HI_MW_ATTR_AUDIO_CTRL, (HI_U8 *)&stAudioCtrlData, sizeof(stAudioCtrlData));
    pDb->Insert(HI_MW_ATTR_AUDIO_BASE, (HI_U8 *)&stAudioBaseData, sizeof(stAudioBaseData));
    pDb->Insert(HI_MW_ATTR_AUDIO_AMP, (HI_U8 *)&stAudioAmpData, sizeof(stAudioAmpData));
    pDb->Insert(HI_MW_ATTR_AUDIO_AVC, (HI_U8 *)&stAvcAttrData, sizeof(stAvcAttrData));

    pDbo->Attach(HI_MW_ATTR_AUDIO_SRS, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_DBX, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_CTRL, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_BASE, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_AMP, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_AVC, pDb);
}

static HI_VOID InsertUsrDefaultThree(HiDBO *pDbo, HiDB *pDb)
{
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_DRC);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_SOUND_MODE);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_PRECIGAIN);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_PEQ_QVALUE);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_PEQ_GAIN);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_SWS3D);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_DBX2);
    pDb->CreateRecord(HI_MW_ATTR_AUDIO_DAP);

    HI_MW_DRC_ATTR_S stDrcAttrData[HI_MW_AUDIO_CHANNEL_BUTT];
    HI_MW_AUDIO_DBX2_S stAudioDbx2Data;
    HI_MW_AUDIO_DAP_S stAudioDapData;
    HI_MW_SND_MODE_ATTR_S astAudioSoundData[HI_MW_SNDMODE_BUTT];
    HI_MW_SND_PRECIGAIN_ATTR_S stAudioPrecigainData;
    HI_S32 stPeqQValue[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_VALUE_BUTT];
    HI_S32 stPeqGain[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_GAIN_BUTT];
    HI_MW_AUDIO_SWS3D_S stAudioSws3DData;

    UpdateAudDrcDef(stDrcAttrData);
    UpdateAudSndDef(astAudioSoundData);
    UpdateAudPrecigainDef(stAudioPrecigainData);
    UpdateAudPeqQValueDef(stPeqQValue);
    UpdateAudPeqGainDef(stPeqGain);
    UpdateAudSws3DDef(stAudioSws3DData);
    UpdateAudDbx2Def(stAudioDbx2Data);
    UpdateAudDapDef(stAudioDapData);

    pDb->Insert(HI_MW_ATTR_AUDIO_DRC, (HI_U8 *)&stDrcAttrData, sizeof(stDrcAttrData));
    pDb->Insert(HI_MW_ATTR_AUDIO_SOUND_MODE, (HI_U8 *)&astAudioSoundData, sizeof(astAudioSoundData));
    pDb->Insert(HI_MW_ATTR_AUDIO_PRECIGAIN, (HI_U8 *)&stAudioPrecigainData, sizeof(stAudioPrecigainData));
    pDb->Insert(HI_MW_ATTR_AUDIO_PEQ_QVALUE, (HI_U8 *)stPeqQValue, sizeof(stPeqQValue));
    pDb->Insert(HI_MW_ATTR_AUDIO_PEQ_GAIN, (HI_U8 *)stPeqGain, sizeof(stPeqGain));
    pDb->Insert(HI_MW_ATTR_AUDIO_SWS3D, (HI_U8 *)&stAudioSws3DData, sizeof(stAudioSws3DData));
    pDb->Insert(HI_MW_ATTR_AUDIO_DBX2, (HI_U8 *)&stAudioDbx2Data, sizeof(stAudioDbx2Data));
    pDb->Insert(HI_MW_ATTR_AUDIO_DAP, (HI_U8 *)&stAudioDapData, sizeof(stAudioDapData));

    pDbo->Attach(HI_MW_ATTR_AUDIO_DRC, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_SOUND_MODE, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_PRECIGAIN, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_PEQ_QVALUE, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_PEQ_GAIN, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_SWS3D, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_DBX2, pDb);
    pDbo->Attach(HI_MW_ATTR_AUDIO_DAP, pDb);
}

static HI_VOID InsertUsrDefaultFour(HiDBO *pDbo, HiDB *pDb)
{
    /* create record of system && 3D in db */
    pDb->CreateRecord(HI_MW_ATTR_SYSTEM);
    pDb->CreateRecord(HI_MW_ATTR_SYSTEM_MAINSRC);
    pDb->CreateRecord(HI_MW_ATTE_3D_MODE);
    pDb->CreateRecord(HI_MW_ATTR_SYSTEM_LOCK);

    /* create record of pc-setting in db */
    pDb->CreateRecord(HI_MW_ATTR_PC_TIMMING);
    pDb->CreateRecord(HI_MW_ATTR_SYSTEM_CC);
    pDb->CreateRecord(HI_MW_ATTR_SYSTEM_WAKEUP);

    /* insert default data of system */
    HI_MW_SYSTEM_S stSystemData;
    HI_MW_SRC_E enMainSrc;
    HI_MW_3D_MODE_S st3DModeData;
    HI_MW_PC_TIMMING_S astPcTimmingData[PC_MODE_SAVE_MAX];
    HI_MW_LOCK_S stSystemLockData;
    HI_MW_CC_S stSystemCcData;
    HI_MW_SYSTEM_WAKEUP_ENABLE_S stSystemWakeupData;

    UpdateSysDef(stSystemData);
    UpdateSysMainSrcDef(enMainSrc);
    UpdateSysVideo3DDef(st3DModeData);
    UpdateSysPCTimingDef(astPcTimmingData, PC_MODE_SAVE_MAX);
    UpdateSystemLockDef(stSystemLockData);
    UpdateSystemCcDef(stSystemCcData);
    UpdateSystemWakeupDef(stSystemWakeupData);

    pDb->Insert(HI_MW_ATTR_SYSTEM, (HI_U8 *)&stSystemData, sizeof(stSystemData));
    pDb->Insert(HI_MW_ATTR_SYSTEM_MAINSRC, (HI_U8 *)&enMainSrc, sizeof(enMainSrc));
    pDb->Insert(HI_MW_ATTE_3D_MODE, (HI_U8 *)&st3DModeData, sizeof(st3DModeData), g_supportSrcList.length());
    pDb->Insert(HI_MW_ATTR_PC_TIMMING, (HI_U8 *)&astPcTimmingData, sizeof(astPcTimmingData));
    pDb->Insert(HI_MW_ATTR_SYSTEM_LOCK, (HI_U8 *)&stSystemLockData, sizeof(stSystemLockData));
    pDb->Insert(HI_MW_ATTR_SYSTEM_CC, (HI_U8 *)&stSystemCcData, sizeof(stSystemCcData));
    pDb->Insert(HI_MW_ATTR_SYSTEM_WAKEUP, (HI_U8 *)&stSystemWakeupData, sizeof(stSystemWakeupData));

    pDbo->Attach(HI_MW_ATTR_SYSTEM, pDb);
    pDbo->Attach(HI_MW_ATTR_SYSTEM_MAINSRC, pDb);
    pDbo->Attach(HI_MW_ATTE_3D_MODE, pDb);
    pDbo->Attach(HI_MW_ATTR_PC_TIMMING, pDb);
    pDbo->Attach(HI_MW_ATTR_SYSTEM_LOCK, pDb);
    pDbo->Attach(HI_MW_ATTR_SYSTEM_CC, pDb);
    pDbo->Attach(HI_MW_ATTR_SYSTEM_WAKEUP, pDb);
}

static HI_VOID InsertUsrDefaultFive(HiDBO *pDbo, HiDB *pDb)
{
    /* create record of cec in db */
    pDb->CreateRecord(HI_MW_ATTR_CEC_MENU);

    /* create record of ntsc scan in db */
    pDb->CreateRecord(HI_MW_ATTR_ATV_NTSC_SCAN);
    pDb->CreateRecord(HI_MW_ATTR_HDMITX_ATTR);

    /* create record of tif cec option */
    pDb->CreateRecord(HI_MW_ATTR_TIF_CEC_OPTION);

    /* insert default data of cec menu state */
    HI_MW_CEC_MENU_S stCecMenuState;
    HI_MW_NTSC_SCAN_S stNtscScan;
    HI_MW_HDMITX_DATA_S stHdmiTxData;

    /* insert default data of tif cec state */
    HI_MW_TIF_CEC_OPTION_S stTifCecState;

    UpdateCecDef(stCecMenuState);
    UpdateNtscScanDef(stNtscScan);
    UpdateHdmiTxDataDef(stHdmiTxData);
    UpdateTifCecOption(stTifCecState);

    pDb->Insert(HI_MW_ATTR_CEC_MENU, (HI_U8 *)&stCecMenuState, sizeof(stCecMenuState));
    /* insert default data of ntsc scan */
    pDb->Insert(HI_MW_ATTR_ATV_NTSC_SCAN, (HI_U8 *)&stNtscScan, sizeof(stNtscScan));
    pDb->Insert(HI_MW_ATTR_HDMITX_ATTR, (HI_U8 *)&stHdmiTxData, sizeof(stHdmiTxData));

    /* insert default data of tif cec state */
    pDb->Insert(HI_MW_ATTR_TIF_CEC_OPTION, reinterpret_cast<HI_U8 *>(&stTifCecState), sizeof(stTifCecState));

    pDbo->Attach(HI_MW_ATTR_CEC_MENU, pDb);
    pDbo->Attach(HI_MW_ATTR_ATV_NTSC_SCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_HDMITX_ATTR, pDb);
    pDbo->Attach(HI_MW_ATTR_TIF_CEC_OPTION, pDb);
}

static HI_VOID InsertUsrDefault(HiDBO *pDbo, HiDB *pDb)
{
    if ((pDbo == HI_NULL) || (pDb == HI_NULL)) {
        LOGE("pDbo or pDb is null !");
        return;
    }

    InsertUsrDefaultOne(pDbo, pDb);
    InsertUsrDefaultTwo(pDbo, pDb);
    InsertUsrDefaultThree(pDbo, pDb);
    InsertUsrDefaultFour(pDbo, pDb);
    InsertUsrDefaultFive(pDbo, pDb);

    LOGI("insert Usr default data done.");
}

static HI_VOID InsertFacDefaultOne(HiDBO *pDbo, HiDB *pDb)
{
    /* create records of factory in db */
    pDb->CreateRecord(HI_MW_ATTR_FAC_PANEL);
    pDb->CreateRecord(HI_MW_ATTR_FAC_CTRL);
    pDb->CreateRecord(HI_MW_ATTR_FAC_PANEL_INTF_ATTR);
    pDb->CreateRecord(HI_MW_ATRR_FAC_LVDS);
    pDb->CreateRecord(HI_MW_ATTR_FAC_POWER);
    pDb->CreateRecord(HI_MW_ATTR_FAC_DDR);
    pDb->CreateRecord(HI_MW_ATTR_FAC_EMMC);
    pDb->CreateRecord(HI_MW_ATTR_FAC_VBO);

    HI_MW_FACTORY_PANEL_S stFactoryPanelData;
    HI_MW_FACTORY_CTRL_S stFactoryCtrlData;
    HI_MW_FACTORY_PANEL_INTF_ATTR_S stFactoryPanelIntfAttr;
    HI_MW_FACTORY_LVDS_S stFactoryLvdsData;
    HI_MW_FACTORY_POWER_S stFactoryPowerData;
    HI_MW_FACTORY_DDR_S stFactoryDdrData;
    HI_MW_FACTORY_EMMC_S stFactoryEmmcData;
    HI_MW_FACTORY_VBO_S stFactoryVboData;

    UpdateFacPanelDef(stFactoryPanelData);
    UpdateFacCtrlDef(stFactoryCtrlData);
    UpdateFacPanelIntAttrDef(stFactoryPanelIntfAttr);
    UpdateFacLvdsDef(stFactoryLvdsData);
    UpdateFacPowerDef(stFactoryPowerData);
    UpdateFacDdrDef(stFactoryDdrData);
    UpdateFacEmmcDef(stFactoryEmmcData);
    UpdateFacVboDef(stFactoryVboData);

    /* insert default data of facorty */
    pDb->Insert(HI_MW_ATTR_FAC_PANEL, (HI_U8 *)&stFactoryPanelData, sizeof(stFactoryPanelData));
    pDb->Insert(HI_MW_ATTR_FAC_CTRL, (HI_U8 *)&stFactoryCtrlData, sizeof(stFactoryCtrlData));
    pDb->Insert(HI_MW_ATTR_FAC_PANEL_INTF_ATTR, (HI_U8 *)&stFactoryPanelIntfAttr, sizeof(stFactoryPanelIntfAttr));
    pDb->Insert(HI_MW_ATRR_FAC_LVDS, (HI_U8 *)&stFactoryLvdsData, sizeof(stFactoryLvdsData));
    pDb->Insert(HI_MW_ATTR_FAC_POWER, (HI_U8 *)&stFactoryPowerData, sizeof(stFactoryPowerData));
    pDb->Insert(HI_MW_ATTR_FAC_DDR, (HI_U8 *)&stFactoryDdrData, sizeof(stFactoryDdrData));
    pDb->Insert(HI_MW_ATTR_FAC_EMMC, (HI_U8 *)&stFactoryEmmcData, sizeof(stFactoryEmmcData));
    pDb->Insert(HI_MW_ATTR_FAC_VBO, (HI_U8 *)&stFactoryVboData, sizeof(stFactoryVboData));

    pDbo->Attach(HI_MW_ATTR_FAC_PANEL, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_CTRL, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_PANEL_INTF_ATTR, pDb);
    pDbo->Attach(HI_MW_ATRR_FAC_LVDS, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_POWER, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_DDR, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_EMMC, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_VBO, pDb);
}

static HI_VOID InsertFacDefaultTwo(HiDBO *pDbo, HiDB *pDb)
{
    pDb->CreateRecord(HI_MW_ATTR_FAC_GMAC);
    pDb->CreateRecord(HI_MW_ATTR_FAC_USB);
    pDb->CreateRecord(HI_MW_ATTR_FAC_EMI);
    pDb->CreateRecord(HI_MW_ATTR_FAC_PQ);
    pDb->CreateRecord(HI_MW_ATTR_FAC_DEVICE);
    pDb->CreateRecord(HI_MW_ATTR_FAC_ADC);
    pDb->CreateRecord(HI_MW_ATTR_FAC_COLORTEMP);
    pDb->CreateRecord(HI_MW_ATTR_FAC_NLA_INFO);
    /* create record of PQ gain */
    pDb->CreateRecord(HI_MW_ATTR_PQ_SHARP_GAIN);

    HI_MW_FACTORY_GMAC_S stFactoryGmacData;
    HI_MW_FACTORY_USB_S stFactoryUsbData;
    HI_MW_FACTORY_EMI_S stFactoryEmiData;
    HI_MW_FACTORY_PQ_S stFactoryPQData;
    HI_MW_FACTORY_DEVICE_S stFactoryDeviceData;
    HI_MW_CALIBRATION_INFO_S astCalibInfoData[HI_MW_ADC_INDEX_BUTT];
    HI_MW_COLOR_TEMP_DATA_S astFacColorTempData[HI_MW_VIDEO_DEFINITION_BUTT][HI_MW_COLORTEMP_BUTT];
    HI_MW_NLA_INFO_S astNlaInfoData[HI_MW_NLA_BUTT];
    HI_MW_FACTORY_SHARP_GAIN_S stSharpGain;

    UpdateFacGmacDef(stFactoryGmacData);
    UpdateFacUsbDef(stFactoryUsbData);
    UpdateFacEmiDef(stFactoryEmiData);
    UpdateFacPQDef(stFactoryPQData);
    UpdateFacDeviceDef(stFactoryDeviceData);
    UpdateFacCalibInfoDef(astCalibInfoData, HI_MW_ADC_INDEX_BUTT);
    UpdateFacNlaDef(astNlaInfoData, HI_MW_NLA_BUTT);
    UpdateFacPqSharpGainDef(stSharpGain);

    pDb->Insert(HI_MW_ATTR_FAC_GMAC, (HI_U8 *)&stFactoryGmacData, sizeof(stFactoryGmacData));
    pDb->Insert(HI_MW_ATTR_FAC_USB, (HI_U8 *)&stFactoryUsbData, sizeof(stFactoryUsbData));
    pDb->Insert(HI_MW_ATTR_FAC_EMI, (HI_U8 *)&stFactoryEmiData, sizeof(stFactoryEmiData));
    pDb->Insert(HI_MW_ATTR_FAC_PQ, (HI_U8 *)&stFactoryPQData, sizeof(stFactoryPQData));
    pDb->Insert(HI_MW_ATTR_FAC_DEVICE, (HI_U8 *)&stFactoryDeviceData, sizeof(stFactoryDeviceData));
    pDb->Insert(HI_MW_ATTR_FAC_ADC, (HI_U8 *)&astCalibInfoData, sizeof(astCalibInfoData));

    for (HI_U8 i = 0; i < g_supportSrcList.length(); i++) {
        UpdateFacClrTmpDef((HI_MW_SRC_E)g_supportSrcList[i], astFacColorTempData);
        pDb->Insert(HI_MW_ATTR_FAC_COLORTEMP, (HI_U8 *)&astFacColorTempData, sizeof(astFacColorTempData));
    }

    pDb->Insert(HI_MW_ATTR_FAC_NLA_INFO, (HI_U8 *)&astNlaInfoData, sizeof(astNlaInfoData), g_supportSrcList.length());
    pDb->Insert(HI_MW_ATTR_PQ_SHARP_GAIN, (HI_U8 *)&stSharpGain, sizeof(stSharpGain));

    pDbo->Attach(HI_MW_ATTR_FAC_GMAC, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_USB, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_EMI, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_PQ, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_DEVICE, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_ADC, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_COLORTEMP, pDb);
    pDbo->Attach(HI_MW_ATTR_FAC_NLA_INFO, pDb);
    pDbo->Attach(HI_MW_ATTR_PQ_SHARP_GAIN, pDb);
}

static HI_VOID InsertFacDefaultThree(HiDBO *pDbo, HiDB *pDb)
{
    pDb->CreateRecord(HI_MW_ATTR_PQ_ACM_GAIN);
    pDb->CreateRecord(HI_MW_ATTR_PQ_DCI_GAIN);

    /* create records of overscan in db */
    pDb->CreateRecord(HI_MW_ATTR_ATV_OVERSCAN);
    pDb->CreateRecord(HI_MW_ATTR_CVBS_OVERSCAN);
    pDb->CreateRecord(HI_MW_ATTR_SCART_OVERSCAN);
    pDb->CreateRecord(HI_MW_ATTR_HDMI_OVERSCAN);
    pDb->CreateRecord(HI_MW_ATTR_YPBPR_OVERSCAN);
    pDb->CreateRecord(HI_MW_ATTR_DTV_OVERSCAN);
    pDb->CreateRecord(HI_MW_ATTR_MEDIA_OVERSCAN);

    HI_MW_FACTORY_ACM_GAIN_S stAcmGain;
    HI_MW_FACTORY_DCI_GAIN_S stDciGain;
    HI_MW_RECT_S astAtvOverScanData[HI_MW_TVD_BUTT];
    HI_MW_RECT_S astCvbsOverScanData[HI_MW_TVD_BUTT];
    HI_MW_RECT_S astScartOverScanData[HI_MW_SCART_BUTT];
    HI_MW_RECT_S astHdmiOverScanData[HI_MW_HD_BUTT];
    HI_MW_RECT_S astYpbprOverScanData[HI_MW_HD_BUTT];
    HI_MW_RECT_S astDtvOverScanData[HI_MW_HD_BUTT];
    HI_MW_RECT_S astMediaOverScanData[HI_MW_HD_BUTT];

    UpdateFacPqAcmGainDef(stAcmGain);
    UpdateFacPqDciGainDef(stDciGain);
    UpdateFacAtvOverScanDef(astAtvOverScanData, HI_MW_TVD_BUTT);
    UpdateFacCvbsOverScanDef(astCvbsOverScanData, HI_MW_TVD_BUTT);
    UpdateFacScartOverScanDef(astScartOverScanData, HI_MW_SCART_BUTT);
    UpdateFacHdmiOverScanDef(astHdmiOverScanData, HI_MW_HD_BUTT);
    UpdateFacYpbprOverScanDef(astYpbprOverScanData, HI_MW_HD_BUTT);
    UpdateFacDtvOverScanDef(astDtvOverScanData, HI_MW_HD_BUTT);
    UpdateFacMediaOverScanDef(astMediaOverScanData, HI_MW_HD_BUTT);

    pDb->Insert(HI_MW_ATTR_PQ_ACM_GAIN, (HI_U8 *)&stAcmGain, sizeof(stAcmGain));
    pDb->Insert(HI_MW_ATTR_PQ_DCI_GAIN, (HI_U8 *)&stDciGain, sizeof(stDciGain));
    pDb->Insert(HI_MW_ATTR_ATV_OVERSCAN, (HI_U8 *)&astAtvOverScanData, sizeof(astAtvOverScanData));
    pDb->Insert(HI_MW_ATTR_CVBS_OVERSCAN, (HI_U8 *)&astCvbsOverScanData, sizeof(astCvbsOverScanData));
    pDb->Insert(HI_MW_ATTR_SCART_OVERSCAN,
        reinterpret_cast<HI_U8 *>(&astScartOverScanData), sizeof(astScartOverScanData));
    pDb->Insert(HI_MW_ATTR_HDMI_OVERSCAN, (HI_U8 *)&astHdmiOverScanData, sizeof(astHdmiOverScanData));
    pDb->Insert(HI_MW_ATTR_YPBPR_OVERSCAN, (HI_U8 *)&astYpbprOverScanData, sizeof(astYpbprOverScanData));
    pDb->Insert(HI_MW_ATTR_DTV_OVERSCAN, (HI_U8 *)&astDtvOverScanData, sizeof(astDtvOverScanData));
    pDb->Insert(HI_MW_ATTR_MEDIA_OVERSCAN, (HI_U8 *)&astMediaOverScanData, sizeof(astMediaOverScanData));

    pDbo->Attach(HI_MW_ATTR_PQ_ACM_GAIN, pDb);
    pDbo->Attach(HI_MW_ATTR_PQ_DCI_GAIN, pDb);
    pDbo->Attach(HI_MW_ATTR_ATV_OVERSCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_CVBS_OVERSCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_SCART_OVERSCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_HDMI_OVERSCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_YPBPR_OVERSCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_DTV_OVERSCAN, pDb);
    pDbo->Attach(HI_MW_ATTR_MEDIA_OVERSCAN, pDb);
}

static HI_VOID InsertFacDefault(HiDBO *pDbo, HiDB *pDb)
{
    if ((pDbo == HI_NULL) || (pDb == HI_NULL)) {
        LOGE("pDbo or pDb is null !");
        return;
    }

    InsertFacDefaultOne(pDbo, pDb);
    InsertFacDefaultTwo(pDbo, pDb);
    InsertFacDefaultThree(pDbo, pDb);

    pDb->CreateRecord(HI_MW_ATTR_FACTORY_PQ_GAIN);
    HI_MW_FACTORY_PQ_GAIN_S stFacPqGain;
    for (HI_U8 i = 0; i < g_supportSrcList.length(); i++) {
        UpdateFacPqGainDef((HI_MW_SRC_E)g_supportSrcList[i], stFacPqGain);
        pDb->Insert(HI_MW_ATTR_FACTORY_PQ_GAIN, (HI_U8 *)&stFacPqGain, sizeof(stFacPqGain));
    }
    pDbo->Attach(HI_MW_ATTR_FACTORY_PQ_GAIN, pDb);

    LOGI("insert Fac default data done.");
}

static HI_VOID InsertChnDefaultOne(HiDBO *pDbo, HiDB *pDb)
{
    /* create db records of channel */
    pDb->CreateRecord(HI_MW_ATTR_ATV_CURRENT_PROG);
    pDb->CreateRecord(HI_MW_ATTR_ATV_FREEZE_MODE);
    pDb->CreateRecord(HI_MW_ATTR_ATV_NSTD);

    HI_U8 u8CurrentProgramNum;
    HI_U8 u8FreezeMode;
    HI_MW_ATV_NSTD_S stNstdInfo;

    // HI_MW_COUNTRY_E   enCountry;
    UpdateChannleCurProNumDef(u8CurrentProgramNum);
    UpdateChannleFreezeDef(u8FreezeMode);
    UpdateChannleNstdInfoDef(stNstdInfo);

    /* insert default data of channel */
    pDb->Insert(HI_MW_ATTR_ATV_CURRENT_PROG, &u8CurrentProgramNum, sizeof(u8CurrentProgramNum));
    pDb->Insert(HI_MW_ATTR_ATV_FREEZE_MODE, &u8FreezeMode, sizeof(u8FreezeMode));
    pDb->Insert(HI_MW_ATTR_ATV_NSTD, (HI_U8 *)&stNstdInfo, sizeof(stNstdInfo), MAX_PROGRAM_COUNT);

    pDbo->Attach(HI_MW_ATTR_ATV_CURRENT_PROG, pDb);
    pDbo->Attach(HI_MW_ATTR_ATV_FREEZE_MODE, pDb);
    pDbo->Attach(HI_MW_ATTR_ATV_NSTD, pDb);
}

static HI_VOID InsertChnDefaultTwo(HiDBO *pDbo, HiDB *pDb, HI_MW_TVSYSTEM_E enTvSystem = HI_MW_TVSYSTEM_BUTT)
{
    pDb->CreateRecord(HI_MW_ATTR_ATV_INFO);

    HI_MW_ATV_PROG_S stProgInfo;
    HI_U32 i;

    UpdateChannleProgInfoDef(stProgInfo);
    if (enTvSystem == HI_MW_TVSYSTEM_BUTT) {
        enTvSystem = (HI_MW_TVSYSTEM_E)HiCfgFact::Get()->ParseInt("databasedef:System_tvSystem");
        LOGI("get default Tv system from ini. country = %d.", enTvSystem);
    }

    if ((enTvSystem == HI_MW_TVSYSTEM_ATSC) || (enTvSystem == HI_MW_TVSYSTEM_ISDBT)) {
    } else {
        pDb->Insert(HI_MW_ATTR_ATV_INFO, (HI_U8 *)&stProgInfo, sizeof(stProgInfo), MAX_PROGRAM_COUNT);
    }

    pDbo->Attach(HI_MW_ATTR_ATV_INFO, pDb);
}


static HI_VOID InsertChnDefault(HiDBO *pDbo, HiDB *pDb, HI_MW_TVSYSTEM_E enTvSystem = HI_MW_TVSYSTEM_BUTT)
{
    if ((pDbo == HI_NULL) || (pDb == HI_NULL)) {
        LOGE("pDbo or pDb is null !");
        return;
    }

    InsertChnDefaultOne(pDbo, pDb);
    InsertChnDefaultTwo(pDbo, pDb, enTvSystem);

    LOGI("insert Chn default data done.Tv system: %d", enTvSystem);
}

static HI_S32 CreateFile(const string &dbFile, HiDB **db)
{
    HI_U32 delimiterPos = dbFile.rfind('/');
    LOGI("CreateFile %s start", dbFile.c_str());
    if (delimiterPos != std::string::npos) {
        MakeDirent(dbFile.substr(0, delimiterPos));
    }
    HiStorageFile *storage = new (std::nothrow) HiStorageFile();
    if (storage == HI_NULL) {
        LOGE("Error, Malloc data(HiStorageFile) failed!");
        return HI_MW_COM_MEMORY_EER;
    }

    HI_S32 s32Ret = storage->Init(dbFile);
    if (s32Ret != HI_SUCCESS) {
        LOGE("database %s open fail.", dbFile.data());
        RemoveDirent(dbFile);
        storage->Init(dbFile);
    }

    *db = new HiDB(storage);
    if (*db == HI_NULL) {
        LOGE("Error, Malloc data failed!");
        return HI_MW_COM_MEMORY_EER;
    }

    return HI_SUCCESS;
}

static HI_S32 LoadUsrSetting(HiDBO *pDbo)
{
    string dbFile = HiCfgFact::Get()->ParseString("databasepath:databaseusr");
    HI_U16 u16UserVersion = (HI_U16)HiCfgFact::Get()->ParseInt("databasedef:DatabaseUserVersion");
    HI_S32 s32Ret;
    HiDB *db = HI_NULL;
    s32Ret = CreateFile(dbFile, &db);
    if (s32Ret != HI_SUCCESS) {
        return s32Ret;
    }

    HI_U16 u16Version = 0;
    HI_BOOL bRestoreDefault = HI_FALSE;

    s32Ret = db->QueryVersion(u16Version);
    if (s32Ret == HI_MW_DB_FIRSTBOOT_EER) {
        bRestoreDefault = HI_TRUE;
    } else {
        if (u16Version != u16UserVersion) {
            LOGI("Update data version!");
            db->Clear();
            bRestoreDefault = HI_TRUE;
        }
    }

    if (bRestoreDefault) {
        LOGI("Insert default data!");
        /* update database version */
        db->InsertVersion(u16UserVersion);
        InsertUsrDefault(pDbo, db);
        return HI_SUCCESS;
    }
    s32Ret = db->Load();
    if (s32Ret == HI_SUCCESS) {
        list<HI_U32> records;
        list<HI_U32>::iterator it;
        db->GetRecords(records);
        if (records.size() < (HI_MW_ATTR_USR_END - HI_MW_ATTR_USR - 1)) {
            LOGI("Record is lose [%d, %d]!", records.size(), (HI_MW_ATTR_USR_END - HI_MW_ATTR_USR - 1));
        } else {
            for (it = records.begin(); it != records.end(); ++it) {
                pDbo->Attach((HI_MW_ATTR_E)*it, db);
            }

            LOGI("load Usr Setting data success!");
            return HI_SUCCESS;
        }
    }
    LOGW("Load data fail!, clear db file");
    db->Clear();

    LOGI("Insert default data!");
    /* update database version */
    db->InsertVersion(u16UserVersion);
    InsertUsrDefault(pDbo, db);

    return HI_SUCCESS;
}

static HI_S32 LoadFacSetting(HiDBO *pDbo)
{
    string dbFile = HiCfgFact::Get()->ParseString("databasepath:databasefac");
    HI_U16 u16FactoryVersion = (HI_U16)HiCfgFact::Get()->ParseInt("databasedef:DatabaseFactoryVersion");
    HI_S32 s32Ret;
    HiDB *db = HI_NULL;
    s32Ret = CreateFile(dbFile, &db);
    if (s32Ret != HI_SUCCESS) {
        return s32Ret;
    }

    HI_U16 u16Version = 0;
    HI_BOOL bRestoreDefault = HI_FALSE;
    s32Ret = db->QueryVersion(u16Version);
    if (s32Ret == HI_MW_DB_FIRSTBOOT_EER) {
        bRestoreDefault = HI_TRUE;
    } else {
        if (u16Version != u16FactoryVersion) {
            LOGI("Update data version!");
            db->Clear();
            bRestoreDefault = HI_TRUE;
        }
    }

    if (bRestoreDefault) {
        LOGI("Insert default data!");
        /* update db version of factory */
        db->InsertVersion(u16FactoryVersion);
        InsertFacDefault(pDbo, db);
        return HI_SUCCESS;
    }

    s32Ret = db->Load();
    if (s32Ret == HI_SUCCESS) {
        list<HI_U32> records;
        list<HI_U32>::iterator it;
        db->GetRecords(records);
        if (records.size() != (HI_MW_ATTR_FAC_END - HI_MW_ATTR_FAC - 1)) {
            LOGI("Record is lose [%d, %d]!", records.size(), (HI_MW_ATTR_FAC_END - HI_MW_ATTR_FAC - 1));
        } else {
            for (it = records.begin(); it != records.end(); ++it) {
                pDbo->Attach((HI_MW_ATTR_E)*it, db);
            }

            LOGI("load Fac Setting data success!");
            return HI_SUCCESS;
        }
    }
    LOGI("Load data fail!");
    db->Clear();

    LOGI("Insert default data!");
    /* update db version of factory */
    db->InsertVersion(u16FactoryVersion);
    InsertFacDefault(pDbo, db);

    return HI_SUCCESS;
}

static HI_S32 LoadChnSetting(HiDBO *pDbo)
{
    string dbFile = HiCfgFact::Get()->ParseString("databasepath:databaseatv");
    HI_U16 u16ChannelVersion = (HI_U16)HiCfgFact::Get()->ParseInt("databasedef:DatabaseChannelVersion");
    HI_S32 s32Ret;
    HiDB *db = HI_NULL;
    s32Ret = CreateFile(dbFile, &db);
    if (s32Ret != HI_SUCCESS) {
        return s32Ret;
    }

    HI_U16 u16Version = 0;
    HI_BOOL bRestoreDefault = HI_FALSE;

    s32Ret = db->QueryVersion(u16Version);
    if (s32Ret == HI_MW_DB_FIRSTBOOT_EER) {
        bRestoreDefault = HI_TRUE;
    } else {
        if (u16Version != u16ChannelVersion) {
            LOGI("Update data version!");
            db->Clear();
            bRestoreDefault = HI_TRUE;
        }
    }

    if (bRestoreDefault) {
        LOGI("Insert default data!");
        /* update db version of channel */
        db->InsertVersion(u16ChannelVersion);
        InsertChnDefault(pDbo, db, HI_MW_TVSYSTEM_BUTT);
        return HI_SUCCESS;
    }

    s32Ret = db->Load();
    if (s32Ret == HI_SUCCESS) {
        list<HI_U32> records;
        list<HI_U32>::iterator it;
        db->GetRecords(records);
        if (records.size() != (HI_MW_ATTR_ATV_END - HI_MW_ATTR_ATV - 1)) {
            LOGI("Record is lose [%d, %d]!", records.size(), (HI_MW_ATTR_ATV_END - HI_MW_ATTR_ATV - 1));
        } else {
            for (it = records.begin(); it != records.end(); ++it) {
                pDbo->Attach((HI_MW_ATTR_E)*it, db);
            }

            LOGI("load Chn Setting data success!");
            return HI_SUCCESS;
        }
    }
    LOGI("Load data fail!");
    db->Clear();

    LOGI("Insert default data!");
    /* update db version of channel */
    db->InsertVersion(u16ChannelVersion);
    InsertChnDefault(pDbo, db, HI_MW_TVSYSTEM_BUTT);

    return HI_SUCCESS;
}

HiDBO::HiDBO()
{
    HI_S32 ret;
    ret = LoadUsrSetting(this);
    RET_CHECK_ERR_LOG(ret, "LoadUsrSetting failure.ret: 0x%x", ret);
#if 0
    ret = LoadFacSetting(this);
    RET_CHECK_ERR_LOG(ret, "LoadFacSetting failure.ret: 0x%x", ret);

    ret = LoadChnSetting(this);
    RET_CHECK_ERR_LOG(ret, "LoadChnSetting failure.ret: 0x%x", ret);
#endif
    m_sDboCallBack.pfnDbUpdateCb = NULL;
    m_sDboCallBack.pfnDbQueryCb = NULL;
    m_sDboCallBack.pfnDbQueryManyCb = NULL;
}

HiDBO::~HiDBO()
{
}

HI_S32 HiDBO::Query(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row)
{
    if (pData == HI_NULL) {
        LOGE("pData is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }

    if (m_sDboCallBack.pfnDbQueryCb != NULL) {
        if (m_sDboCallBack.pfnDbQueryCb(it->second, enAttr, pData, u32Length, u32Row) == HI_SUCCESS) {
            return HI_SUCCESS;
        }
    }

    return it->second->Query(enAttr, pData, u32Length, u32Row);
}

HI_S32 HiDBO::Query(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32StartRow, HI_U32 u32EndRow)
{
    if (pData == HI_NULL) {
        LOGE("pData is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }

    if (m_sDboCallBack.pfnDbQueryManyCb != NULL) {
        if (m_sDboCallBack.pfnDbQueryManyCb(it->second, enAttr, pData, u32Length,
            u32StartRow, u32EndRow) == HI_SUCCESS) {
            return HI_SUCCESS;
        }
    }

    if (it->second) {
        return it->second->Query(enAttr, pData, u32Length, u32StartRow, u32EndRow);
    } else {
        LOGE("the DB object is null");
        return HI_MW_DB_DATA_EER;
    }
}

HI_S32 HiDBO::Update(HI_MW_ATTR_E enAttr, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row)
{
    if (pData == HI_NULL) {
        LOGE("pData is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }

    if (m_sDboCallBack.pfnDbUpdateCb != NULL) {
        if (m_sDboCallBack.pfnDbUpdateCb(it->second, enAttr, pData, u32Length, u32Row) == HI_SUCCESS) {
            return HI_SUCCESS;
        }
    }

    return it->second->Update(enAttr, pData, u32Length, u32Row);
}

HI_S32 HiDBO::GetNum(HI_MW_ATTR_E enAttr, HI_U32 *pu32Num)
{
    if (pu32Num == HI_NULL) {
        LOGE("pu32Num is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }
    return it->second->GetRecordNum(enAttr, pu32Num);
}

HI_S32 HiDBO::Insert(HI_MW_ATTR_E enAttr, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Num)
{
    if (pData == HI_NULL) {
        LOGE("pData is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }
    return it->second->Insert(enAttr, pData, u32Length, u32Num);
}

HI_S32 HiDBO::Erase(HI_MW_ATTR_E enAttr, HI_U32 u32Row)
{
    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }
    return it->second->Erase(enAttr, u32Row);
}

HI_S32 HiDBO::Erase(HI_MW_ATTR_E enAttr)
{
    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
        return HI_MW_COM_PARAM_EER;
    }
    return it->second->Erase(enAttr);
}

HI_S32 HiDBO::Clear(HI_MW_DB_E enDBType)
{
    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(HI_MW_ATTR_E(enDBType + 1));
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enDBType + 1);
        return HI_MW_COM_PARAM_EER;
    }
    return it->second->Clear();
}

HI_S32 HiDBO::DisableSync(HI_MW_ATTR_E enAttr)
{
    // disable sync singal attr enum
    map<HI_MW_ATTR_E, HiDB *>::iterator it;
    if (0x0FFF & (HI_U32)enAttr) {
        it = m_mapAttrDB.find(enAttr);
        if (it == m_mapAttrDB.end()) {
            LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
            return HI_MW_COM_PARAM_EER;
        }

        it->second->DisableSync((HI_U32)enAttr);
        return HI_SUCCESS;
    }
    // disable sync all factory / user / channel
    list<HI_U32> records;
    list<HI_U32>::iterator attr;
    it = m_mapAttrDB.find(HI_MW_ATTR_E(enAttr + 1));
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    it->second->GetRecords(records);
    for (attr = records.begin(); attr != records.end(); ++attr) {
        it->second->DisableSync(*attr);
    }

    return HI_SUCCESS;
}

HI_S32 HiDBO::EnableSync(HI_MW_ATTR_E enAttr, HI_BOOL bFlush)
{
    map<HI_MW_ATTR_E, HiDB *>::iterator it;
    if (0x0FFF & (HI_U32)enAttr) {
        it = m_mapAttrDB.find(enAttr);
        if (it == m_mapAttrDB.end()) {
            LOGE("Error, Input parameter invalid! not find DB attr Id:0x%x", enAttr);
            return HI_MW_COM_PARAM_EER;
        }

        it->second->EnableSync((HI_U32)enAttr, bFlush);
        return HI_SUCCESS;
    }

    list<HI_U32> records;
    list<HI_U32>::iterator attr;
    it = m_mapAttrDB.find(HI_MW_ATTR_E(enAttr + 1));
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    it->second->GetRecords(records);
    for (attr = records.begin(); attr != records.end(); ++attr) {
        it->second->EnableSync(*attr, bFlush);
    }

    return HI_SUCCESS;
}

HI_S32 HiDBO::Attach(HI_MW_ATTR_E enAttr, HiDB *pDb)
{
    if (pDb == HI_NULL) {
        LOGE("pDb is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(enAttr);
    if (it != m_mapAttrDB.end()) {
        LOGI("Info, enAttr 0x%x exist!", enAttr);
        return HI_SUCCESS;
    }

    m_mapAttrDB[enAttr] = pDb;
    return HI_SUCCESS;
}

HI_S32 HiDBO::Obtain(HI_MW_DB_E enDBType, HiDB **pDb)
{
    if (pDb == HI_NULL) {
        LOGE("pDb is null ptr");
        return HI_FAILURE;
    }

    map<HI_MW_ATTR_E, HiDB *>::iterator it = m_mapAttrDB.find(HI_MW_ATTR_E(enDBType + 1));
    if (it == m_mapAttrDB.end()) {
        LOGE("Error, Input parameter invalid! attr:0x%x", enDBType + 1);
        return HI_MW_COM_PARAM_EER;
    }
    *pDb = it->second;

    return HI_SUCCESS;
}

HI_VOID HiDBO::RestoreUsr()
{
    HiDB *db = NULL;

    if (!Obtain(HI_MW_DB_USR, &db) && (db != NULL)) {
        db->Clear();
        db->InsertVersion(HiCfgFact::Get()->ParseInt("databasedef:DatabaseUserVersion"));
        InsertUsrDefault(this, db);
        LOGI("restore Usr data success.");
    }
}

HI_VOID HiDBO::RestoreFac()
{
    HiDB *db = NULL;

    if (!Obtain(HI_MW_DB_FAC, &db) && (db != NULL)) {
        db->Clear();
        db->InsertVersion(HiCfgFact::Get()->ParseInt("databasedef:DatabaseFactoryVersion"));
        InsertFacDefault(this, db);
        LOGI("restore Fac data success.");
    }
}

HI_VOID HiDBO::RestoreChn()
{
    HiDB *db = NULL;

    if (!Obtain(HI_MW_DB_CHN, &db) && (db != NULL)) {
        db->Clear();
        db->InsertVersion(HiCfgFact::Get()->ParseInt("databasedef:DatabaseChannelVersion"));

        // 在此查询DB中的国家地区, 根据国家地区决定重置为盲扫方式的频道列表还是美规的固定频点的频道列表.
        HI_MW_SYSTEM_S stSystemData;
        errno_t rc = memset_s(&stSystemData, sizeof(stSystemData), 0, sizeof(stSystemData));
        if (rc != EOK) {
            LOGE("memset_s_func failed, rc = %d", rc);
            return;
        }
        Query(HI_MW_ATTR_SYSTEM, (HI_U8 *)&stSystemData, sizeof(HI_MW_SYSTEM_S));

        InsertChnDefault(this, db, stSystemData.enTvSystem);
        LOGI("restore Chn data success.");
    }
}

HI_S32 HiDBO::RestorePic()
{
    HI_S32 ret;

    ret = RestorePicDbUsr();
    if (ret != HI_SUCCESS) {
        LOGE("RestorePicDbUsr failure ret: 0x%x", ret);
        return ret;
    }

    ret = RestorePicDbFac();
    if (ret != HI_SUCCESS) {
        LOGE("RestorePicDbFac failure ret: 0x%x", ret);
        return ret;
    }

    return ret;
}

HI_S32 HiDBO::RestorePicDbUsr()
{
    HiDB *db = NULL;
    HI_S32 ret;

    if (!Obtain(HI_MW_DB_USR, &db) && (db != NULL)) {
        /* insert default data of picture */
        HI_MW_PICTURE_S pictureData;
        HI_MW_PICTURE_PQ_S picPQData;
        HI_MW_PICTURE_DISP_S picDispData;
        HI_MW_PICTURE_VIDEO_S picVideoData;
        HI_MW_PICTURE_EF_S picEfData;
        HI_MW_IMAGE_S picImageData[HI_MW_PICMODE_BUTT];

        for (HI_U8 i = 0; i < g_supportSrcList.length(); i++) {
            UpdatePicDef((HI_MW_SRC_E)g_supportSrcList[i], pictureData);
            UpdatePicPQDef((HI_MW_SRC_E)g_supportSrcList[i], picPQData);
            UpdatePicDispDef((HI_MW_SRC_E)g_supportSrcList[i], picDispData);
            UpdatePicVideoDef((HI_MW_SRC_E)g_supportSrcList[i], picVideoData);
            UpdatePicEfDef((HI_MW_SRC_E)g_supportSrcList[i], picEfData);
            UpdatePicImageDef((HI_MW_SRC_E)g_supportSrcList[i], picImageData, HI_MW_PICMODE_BUTT);

            ret = db->Update(HI_MW_ATTR_PICTURE, (HI_U8 *)&pictureData, sizeof(pictureData), i);
            RET_CHECK_ERR_LOG(ret, "update pictureData failure ret: 0x%x", ret);
            ret = db->Update(HI_MW_ATTR_PICTURE_PQ, (HI_U8 *)&picPQData, sizeof(picPQData), i);
            RET_CHECK_ERR_LOG(ret, "update picPQData failure ret: 0x%x", ret);
            ret = db->Update(HI_MW_ATTR_PICTURE_DISP, (HI_U8 *)&picDispData, sizeof(picDispData), i);
            RET_CHECK_ERR_LOG(ret, "update picDispData failure ret: 0x%x", ret);
            ret = db->Update(HI_MW_ATTR_PICTURE_VIDEO, (HI_U8 *)&picVideoData, sizeof(picVideoData), i);
            RET_CHECK_ERR_LOG(ret, "update picVideoData failure ret: 0x%x", ret);
            ret = db->Update(HI_MW_ATTR_PICTURE_EF, (HI_U8 *)&picEfData, sizeof(picEfData), i);
            RET_CHECK_ERR_LOG(ret, "update picEfData failure ret: 0x%x", ret);
            ret = db->Update(HI_MW_ATTR_PICTURE_IMAGE, (HI_U8 *)&picImageData, sizeof(picImageData), i);
            RET_CHECK_ERR_LOG(ret, "update picImageData failure ret: 0x%x", ret);
        }

        HI_MW_PQ_HDR_FIXMODE_S picHdrFixmodeData;
        UpdatePicPqHdrFixModeDef(picHdrFixmodeData);
        ret = db->Update(HI_MW_ATTR_PICTURE_HDR_FIXMODE,
            (HI_U8 *)&picHdrFixmodeData, sizeof(picHdrFixmodeData));
        if (ret != HI_SUCCESS) {
            LOGE("update picHdrFixmodeData failure ret: 0x%x", ret);
            return ret;
        }
        return ret;
    }

    return HI_FAILURE;
}

HI_S32 HiDBO::RestorePicDbFac()
{
    HiDB *db = NULL;
    HI_S32 ret;

    if (!Obtain(HI_MW_DB_FAC, &db) && (db != NULL)) {
        /* insert defalut data of PQ gain */
        HI_MW_FACTORY_SHARP_GAIN_S sharpGain;
        HI_MW_FACTORY_ACM_GAIN_S acmGain;
        HI_MW_FACTORY_DCI_GAIN_S dciGain;
        HI_MW_FACTORY_PQ_GAIN_S facPqGain;

        UpdateFacPqSharpGainDef(sharpGain);
        UpdateFacPqAcmGainDef(acmGain);
        UpdateFacPqDciGainDef(dciGain);
        for (HI_U8 i = 0; i < g_supportSrcList.length(); i++) {
            UpdateFacPqGainDef((HI_MW_SRC_E)g_supportSrcList[i], facPqGain);
            ret = db->Update(HI_MW_ATTR_FACTORY_PQ_GAIN, (HI_U8 *)&facPqGain, sizeof(facPqGain), i);
            RET_CHECK_ERR_LOG(ret, "update facPqGain %d faillure ret: 0x%x", i, ret);
        }
        ret = db->Update(HI_MW_ATTR_PQ_SHARP_GAIN, (HI_U8 *)&sharpGain, sizeof(sharpGain));
        RET_CHECK_ERR_LOG(ret, "update sharpGain failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_PQ_ACM_GAIN, (HI_U8 *)&acmGain, sizeof(acmGain));
        RET_CHECK_ERR_LOG(ret, "update acmGain failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_PQ_DCI_GAIN, (HI_U8 *)&dciGain, sizeof(dciGain));
        if (ret != HI_SUCCESS) {
            LOGE("Update dciGain failure ret: 0x%x", ret);
            return ret;
        }
        return ret;
    }

    return HI_FAILURE;
}

HI_S32 HiDBO::RestoreAud()
{
    HI_S32 ret;

    ret = RestoreAudEffect();
    if (ret != HI_SUCCESS) {
        LOGE("RestoreAudEffect failure ret: 0x%x", ret);
        return ret;
    }

    ret = RestoreAudSetting();
    if (ret != HI_SUCCESS) {
        LOGE("RestorePicDbFac failure ret: 0x%x", ret);
        return ret;
    }

    return ret;
}

HI_S32 HiDBO::RestoreAudEffect()
{
    HiDB *db = NULL;
    HI_S32 ret;

    if (!Obtain(HI_MW_DB_USR, &db) && (db != NULL)) {
        /* insert default data of audio */
        HI_MW_AUDIO_SRS_S audioSrsData;
        HI_MW_AUDIO_DBX_S audioDbxData;
        HI_MW_AVC_ATTR_S avcAttrData;
        HI_MW_DRC_ATTR_S drcAttrData[HI_MW_AUDIO_CHANNEL_BUTT];
        HI_MW_AUDIO_DBX2_S audioDbx2Data;
        HI_MW_AUDIO_DAP_S audioDapData;

        UpdateAudSrsDef(audioSrsData);
        UpdateAudDbxDef(audioDbxData);
        UpdateAudAvcDef(avcAttrData);
        UpdateAudDrcDef(drcAttrData);
        UpdateAudDbx2Def(audioDbx2Data);
        UpdateAudDapDef(audioDapData);

        ret = db->Update(HI_MW_ATTR_AUDIO_SRS, (HI_U8 *)&audioSrsData, sizeof(audioSrsData));
        RET_CHECK_ERR_LOG(ret, "Update audioSrsData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_DBX, (HI_U8 *)&audioDbxData, sizeof(audioDbxData));
        RET_CHECK_ERR_LOG(ret, "Update audioDbxData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_AVC, (HI_U8 *)&avcAttrData, sizeof(avcAttrData));
        RET_CHECK_ERR_LOG(ret, "Update avcAttrData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_DRC, (HI_U8 *)&drcAttrData,
            HI_MW_AUDIO_CHANNEL_BUTT * sizeof(HI_MW_DRC_ATTR_S));
        RET_CHECK_ERR_LOG(ret, "Update drcAttrData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_DBX2, (HI_U8 *)&audioDbx2Data, sizeof(audioDbx2Data));
        RET_CHECK_ERR_LOG(ret, "Update audioDbx2Data failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_DAP, (HI_U8 *)&audioDapData, sizeof(audioDapData));
        if (ret != HI_SUCCESS) {
            LOGE("Update audioDapData failure ret: 0x%x", ret);
            return ret;
        }
        return ret;
    }
    return HI_FAILURE;
}

HI_S32 HiDBO::RestoreAudSetting()
{
    HiDB *db = NULL;
    HI_S32 ret;

    if (!Obtain(HI_MW_DB_USR, &db) && (db != NULL)) {
        /* insert default data of audio */
        HI_MW_AUDIO_CTRL_S audioCtrlData;
        HI_MW_AUDIO_BASE_S audioBaseData[HI_MW_AUDIO_CHANNEL_BUTT];
        HI_MW_AUDIO_AMP_S audioAmpData;
        HI_MW_SND_MODE_ATTR_S audioSoundData[HI_MW_SNDMODE_BUTT];
        HI_MW_SND_PRECIGAIN_ATTR_S audioPrecigainData;
        HI_S32 peqQValue[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_VALUE_BUTT];
        HI_S32 peqGain[HI_MW_AUDIO_CHANNEL_BUTT][HI_MW_AUDIO_PEQ_GAIN_BUTT];

        UpdateAudCtrlDef(audioCtrlData);
        UpdateAudBaseDef(audioBaseData);
        UpdateAudAmpDef(audioAmpData);
        UpdateAudSndDef(audioSoundData);
        UpdateAudPrecigainDef(audioPrecigainData);
        UpdateAudPeqQValueDef(peqQValue);
        UpdateAudPeqGainDef(peqGain);

        ret = db->Update(HI_MW_ATTR_AUDIO_CTRL, (HI_U8 *)&audioCtrlData, sizeof(audioCtrlData));
        RET_CHECK_ERR_LOG(ret, "Update audioCtrlData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_BASE, (HI_U8 *)&audioBaseData, sizeof(audioBaseData));
        RET_CHECK_ERR_LOG(ret, "Update audioBaseData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_AMP, (HI_U8 *)&audioAmpData, sizeof(audioAmpData));
        RET_CHECK_ERR_LOG(ret, "Update audioAmpData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_SOUND_MODE, (HI_U8 *)&audioSoundData, sizeof(audioSoundData));
        RET_CHECK_ERR_LOG(ret, "Update audioSoundData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_PRECIGAIN, (HI_U8 *)&audioPrecigainData, sizeof(audioPrecigainData));
        RET_CHECK_ERR_LOG(ret, "Update audioPrecigainData failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_PEQ_QVALUE, (HI_U8 *)peqQValue, sizeof(peqQValue));
        RET_CHECK_ERR_LOG(ret, "Update peqQValue failure ret: 0x%x", ret);
        ret = db->Update(HI_MW_ATTR_AUDIO_PEQ_GAIN, (HI_U8 *)peqGain, sizeof(peqGain));
        if (ret != HI_SUCCESS) {
            LOGE("Update peqGain failure ret: 0x%x", ret);
            return ret;
        }
        return ret;
    }
    return HI_FAILURE;
}

HI_S32 HiDBO::RestoreSys()
{
    HiDB *pDb = NULL;
    HI_S32 s32Ret = HI_SUCCESS;

    if (!Obtain(HI_MW_DB_USR, &pDb) && (pDb != NULL)) {
        /* insert default data of system */
        HI_MW_SYSTEM_S stSystemData;
        HI_MW_SRC_E enMainSrc;
        HI_MW_3D_MODE_S st3DModeData;
        HI_MW_PC_TIMMING_S astPcTimmingData[PC_MODE_SAVE_MAX];
        HI_MW_LOCK_S stSystemLockData;

        UpdateSysDef(stSystemData);
        UpdateSysMainSrcDef(enMainSrc);
        UpdateSysVideo3DDef(st3DModeData);
        UpdateSysPCTimingDef(astPcTimmingData, PC_MODE_SAVE_MAX);
        UpdateSystemLockDef(stSystemLockData);

        s32Ret = pDb->Update(HI_MW_ATTR_SYSTEM, (HI_U8 *)&stSystemData, sizeof(stSystemData));
        RET_CHECK_ERR_LOG(s32Ret, "Update stSystemData failure s32Ret: 0x%x", s32Ret);
        s32Ret = pDb->Update(HI_MW_ATTR_SYSTEM_MAINSRC, (HI_U8 *)&enMainSrc, sizeof(enMainSrc));
        RET_CHECK_ERR_LOG(s32Ret, "Update enMainSrc failure s32Ret: 0x%x", s32Ret);
        s32Ret = pDb->Update(HI_MW_ATTE_3D_MODE, (HI_U8 *)&st3DModeData, sizeof(st3DModeData),
                             g_supportSrcList.length() - 1);
        RET_CHECK_ERR_LOG(s32Ret, "Update st3DModeData failure s32Ret: 0x%x", s32Ret);
        s32Ret = pDb->Update(HI_MW_ATTR_PC_TIMMING, (HI_U8 *)&astPcTimmingData, sizeof(astPcTimmingData));
        RET_CHECK_ERR_LOG(s32Ret, "Update astPcTimmingData failure s32Ret: 0x%x", s32Ret);
        s32Ret = pDb->Update(HI_MW_ATTR_SYSTEM_LOCK, (HI_U8 *)&stSystemLockData, sizeof(stSystemLockData));
        RET_CHECK_ERR_LOG(s32Ret, "Update stSystemLockData failure s32Ret: 0x%x", s32Ret);

        return s32Ret;
    }

    return HI_FAILURE;
}

HI_S32 HiDBO::Query(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_MW_SRC_E enSrc)
{
    const HI_U8 *pu8Index = GetBoardSourceIdxList();
    if (enSrc == HI_MW_SRC_BUTT) {
        LOGE("Input source parameter invalid!");
        return HI_MW_COM_INIT_ERR;
    }

    HI_U32 u32Row = pu8Index[enSrc];

    if (u32Row == 0xFF) {
        LOGE("Error, Input source parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }

    HI_BOOL isDbSaveBySource = HiCfgFact::Get()->ParseBoolean("globalconfig:DbSaveBySource");
    if (isDbSaveBySource) {
        return Query(enAttr, pData, u32Length, u32Row);
    } else {
    return Query(enAttr, pData, u32Length);
    }
}

HI_S32 HiDBO::Update(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_MW_SRC_E enSrc)
{
    const HI_U8 *pu8Index = GetBoardSourceIdxList();
    if (enSrc == HI_MW_SRC_BUTT) {
        LOGE("Input source parameter invalid!");
        return HI_MW_COM_INIT_ERR;
    }

    HI_U32 u32Row = pu8Index[enSrc];

    if (u32Row == 0xFF) {
        LOGE("Error, Input source parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }

    HI_BOOL isDbSaveBySource = HiCfgFact::Get()->ParseBoolean("globalconfig:DbSaveBySource");
    if (isDbSaveBySource) {
    return Update(enAttr, pData, u32Length, u32Row);
    } else {
    return Update(enAttr, pData, u32Length);
    }
}

HI_U32 HiDBO::CreatDb(const string &dbFile, HI_U16 u16DbVersion, HiDB **ppDb)
{
    if (ppDb == HI_NULL) {
        LOGE("null pointer.");
        return HI_FAILURE;
    }
    LOGI("CreatDb %s start", dbFile.c_str());
    HiDB *db = HI_NULL;
    HI_S32 s32Ret = CreateFile(dbFile, &db);
    if (s32Ret != HI_SUCCESS) {
        return s32Ret;
    }

    *ppDb = db;

    HI_U16 u16Version = 0;
    HI_BOOL bRestoreDefault = HI_FALSE;

    s32Ret = db->QueryVersion(u16Version);
    if (s32Ret == HI_MW_DB_FIRSTBOOT_EER) {
        bRestoreDefault = HI_TRUE;
    } else {
        if (u16Version != u16DbVersion) {
            LOGI("Update data version!");
            db->Clear();
            bRestoreDefault = HI_TRUE;
        }
    }

    if (!bRestoreDefault) {
        s32Ret = db->Load();
        if (s32Ret == HI_SUCCESS) {
            list<HI_U32> records;
            list<HI_U32>::iterator it;
            db->GetRecords(records);
            for (it = records.begin(); it != records.end(); ++it) {
                Attach((HI_MW_ATTR_E)*it, db);
            }

            return HI_SUCCESS;
        }
        LOGI("Load data fail!");
        db->Clear();
    }

    LOGI("Insert default data!");

    /* update db version of customer */
    db->InsertVersion(u16DbVersion);

    return HI_FAILURE;
}

HI_U32 HiDBO::RegisterOpCb(const HI_MW_DBO_CB_S *pstDboCallBack)
{
    if (pstDboCallBack == HI_NULL) {
        LOGE("null pointer.");
        return HI_FAILURE;
    }

    m_sDboCallBack = *pstDboCallBack;

    return HI_SUCCESS;
}

