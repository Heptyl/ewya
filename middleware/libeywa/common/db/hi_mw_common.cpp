/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_common"

#include "hi_mw_common.h"
#include "hi_mw_cfg.h"
//#include "hi_mw_logic_system.h"
#include "cutils/properties.h"  // for property_get
//#include "hi_mw_version.h"

HI_U32 g_boardNum = 0xFF;

HI_S32 GetAndroidBootComplete()
{
    HI_CHAR acTmp[PROPERTY_VALUE_MAX];
    HI_S32 ret;

    errno_t rc = memset_s(acTmp, sizeof(acTmp), 0, sizeof(acTmp));
    if (rc != EOK) {
        LOGE("memset_s_func failed, rc = %d", rc);
        return HI_FAILURE;
    }
    ret = property_get("dev.bootcomplete", acTmp, "0");
    if (ret <= 0) {
        LOGE("no have property:bootcomplete");
        return HI_FAILURE;
    }
    if (!strncmp(acTmp, "1", 1)) {
        return HI_SUCCESS;
    } else {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static HI_U8 g_au8BoardSourceList[HI_MW_SRC_BUTT] = { 0xff };
static HI_U8 g_u8BoardSourceCount = 0;
static HI_U8 g_au8AllBoardSourceList[HI_MW_SRC_BUTT] = { 0xff };
static HI_U8 g_u8AllBoardSourceCount = 0;
static HI_U8 g_au8SourceIdxList[HI_MW_SRC_BUTT] = { 0xff };

Array<HI_U8> GetAllBoardSourceList()
{
    static HI_BOOL bFirstGet = HI_TRUE;
    if (bFirstGet) {
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_ATV;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_DVBC;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_DTMB;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_CVBS1;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_CVBS2;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_CVBS3;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_VGA;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_YPBPR1;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_YPBPR2;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_HDMI1;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_HDMI2;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_HDMI3;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_HDMI4;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_MEDIA;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_MEDIA2;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_SCART1;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_SCART2;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_DVBT;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_ATSC;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_DVBS;
        g_au8AllBoardSourceList[g_u8AllBoardSourceCount++] = HI_MW_SRC_ISDBT;
        bFirstGet = HI_FALSE;
    }

    Array<HI_U8> allBoardSourceList;
    for (HI_U8 i = 0; i < g_u8AllBoardSourceCount; i++) {
        allBoardSourceList.addTail(g_au8AllBoardSourceList[i]);
    }

    return allBoardSourceList;
}

HI_BOOL GetBoardSupportScart()
{
    Array<HI_U8> aSourceList = GetBoardSourceList();

    HI_U8 i = 0;
    for (; i < aSourceList.length(); i++) {
        if ((HI_MW_SRC_E)aSourceList[i] == HI_MW_SRC_SCART1) {
            return HI_TRUE;
        }
    }

    LOGW("scart source is not support");
    return HI_FALSE;
}

Array<HI_U8> GetBoardSourceList()
{
    static HI_BOOL bFirstGet = HI_TRUE;
    if (bFirstGet) {
        HI_U32 index = 1;

        Array<HI_BOOL> enable;

        enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:ATVRFEnable");
        HI_U32 numOfRegions = enable.length();
        if (numOfRegions == 1) {
            index = 0; /* v81x版本不区分地区，数组下标需设置为0 */
        }
        if (enable[index]) {
            g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_ATV;
        }
        GetBoardSourceListSubOne(enable, index);
        GetBoardSourceListSubTwo(enable, index);

        bFirstGet = HI_FALSE;
    }

    Array<HI_U8> aBoardSourceList;
    for (HI_U8 i = 0; i < g_u8BoardSourceCount; i++) {
        aBoardSourceList.addTail(g_au8BoardSourceList[i]);
    }
    return aBoardSourceList;
}

void GetBoardSourceListSubOne(Array < HI_BOOL > enable, HI_U32 index)
{
    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:DVBCEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_DVBC;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:DTMBEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_DTMB;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:CVBS1Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_CVBS1;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:CVBS2Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_CVBS2;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:CVBS3Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_CVBS3;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:VGAEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_VGA;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:YPBPR1Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_YPBPR1;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:YPBPR2Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_YPBPR2;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:HDMI1Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_HDMI1;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:HDMI2Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_HDMI2;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:HDMI3Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_HDMI3;
    }
}

void GetBoardSourceListSubTwo(Array < HI_BOOL > enable, HI_U32 index)
{
    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:HDMI4Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_HDMI4;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:MEDIA1Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_MEDIA;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:MEDIA2Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_MEDIA2;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:SCART1Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_SCART1;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:SCART2Enable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_SCART2;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:DVBTEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_DVBT;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:ATSCEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_ATSC;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:DVBSEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_DVBS;
    }

    enable = HiCfgFact::Get()->ParseBooleanArray("supportsource:ISDBTEnable");
    if (enable[index]) {
        g_au8BoardSourceList[g_u8BoardSourceCount++] = HI_MW_SRC_ISDBT;
    }
}

const HI_U8 *GetBoardSourceIdxList()
{
    static HI_BOOL bFirstGet = HI_TRUE;

    if (bFirstGet) {
        errno_t rc = memset_s(g_au8SourceIdxList, sizeof(g_au8SourceIdxList), \
            0xFF, sizeof(g_au8SourceIdxList));
        if (rc != EOK) {
            LOGE("memset_s_func failed, rc = %d", rc);
            return HI_NULL;
        }

        Array<HI_U8> g_aBoardSourceList = GetAllBoardSourceList();

        for (HI_U8 i = 0; i < g_aBoardSourceList.length(); i++) {
            g_au8SourceIdxList[g_aBoardSourceList[i]] = i;
        }
        bFirstGet = HI_FALSE;
    }
    return g_au8SourceIdxList;
}

void PrintSrsInfo(HI_BOOL bEnable, const HI_MW_AUDIO_SRS_S &stSrs)
{
    if (bEnable > HI_TRUE || bEnable < HI_FALSE) {
        LOGE("para stSrs error!");
        return;
    }
    LOGI("           bSrs:%d", bEnable);
    PrintSrsInfoSubOne(bEnable, stSrs);

    const HI_MW_SRS_ADVGAIN_CONFIG_S &stAdvGainCfg = stSrs.stSrsConfig.stAdvGainConfig;
    const HI_MW_AUDIO_SRS_CUS_COEFS_CFG_S &stCusCoefsCfg = stSrs.stCusCoefsCfg;
    const HI_MW_AUDIO_SRS_CUS_HPFCOEFS_CFG_S &stCusHpfCoefsCfg = stSrs.stCusHpfCoefsCfg;

    LOGI("     Adv config([%f, %f, %f, %f, %f, %f], %f, %f, %f, %f, %lf)",
         stAdvGainCfg.f32MainTechInputGain[HI_MW_SRS_MAINTECH_NONE],
         stAdvGainCfg.f32MainTechInputGain[HI_MW_SRS_MAINTECH_WOWHD],
         stAdvGainCfg.f32MainTechInputGain[HI_MW_SRS_MAINTECH_TSHD],
         stAdvGainCfg.f32MainTechInputGain[HI_MW_SRS_MAINTECH_CC3D],
         stAdvGainCfg.f32MainTechInputGain[HI_MW_SRS_MAINTECH_CSHP],
         stAdvGainCfg.f32MainTechInputGain[HI_MW_SRS_MAINTECH_TBHD],
         stAdvGainCfg.f32GeqInputGain, stAdvGainCfg.f32ToneInputGain,
         stAdvGainCfg.f32AeqInputGain, stAdvGainCfg.f32TBHDRefOffset, stAdvGainCfg.f64HdLmtBoostOffset);
    LOGI(" CusCoef config(%d, %d)", stCusCoefsCfg.s32LowPassAudioFilterCoef[0],
         stCusCoefsCfg.s32LowPassAudioFilterCoef[1]);
    LOGI(" CusCoef config(%d, %d, %d)", stCusCoefsCfg.s32LowBandAudioFilterCoef[0],
         stCusCoefsCfg.s32LowBandAudioFilterCoef[1],
         stCusCoefsCfg.s32LowBandAudioFilterCoef[2]); /* index is 2 */
    LOGI(" CusCoef config(%d, %d, %d)", stCusCoefsCfg.s32MidBandAudioFilterCoef[0],
         stCusCoefsCfg.s32MidBandAudioFilterCoef[1],
         stCusCoefsCfg.s32MidBandAudioFilterCoef[2]); /* index is 2 */
    LOGI(" CusCoef config(%d, %d)", stCusCoefsCfg.s32LowPassAnalysisFilterCoef[0],
         stCusCoefsCfg.s32LowPassAnalysisFilterCoef[1]);
    LOGI(" CusCoef config(%d, %d, %d)", stCusCoefsCfg.s32LowBandAnalysisFilterCoef[0],
         stCusCoefsCfg.s32LowBandAnalysisFilterCoef[1],
         stCusCoefsCfg.s32LowBandAnalysisFilterCoef[2]); /* index is 2 */
    LOGI(" CusCoef config(%d, %d, %d)", stCusCoefsCfg.s32MidBandAnalysisFilterCoef[0],
         stCusCoefsCfg.s32MidBandAnalysisFilterCoef[1],
         stCusCoefsCfg.s32MidBandAnalysisFilterCoef[2]); /* index is 2 */
    LOGI("    QFilterCoef(%d, %d, %d, %d, %d, %d, %d, %d)", stCusCoefsCfg.s32QFilterCoef[0],
        stCusCoefsCfg.s32QFilterCoef[1],
        stCusCoefsCfg.s32QFilterCoef[2], stCusCoefsCfg.s32QFilterCoef[3], stCusCoefsCfg.s32QFilterCoef[4], /* 2, 3, 4 */
        stCusCoefsCfg.s32QFilterCoef[5], stCusCoefsCfg.s32QFilterCoef[6],  /* index is 5, 6 */
        stCusCoefsCfg.s32QFilterCoef[7]); /* index is 7 */

    LOGI("     CusHpfCoef(%d, %d, %d, %d, %d, %d, %d, %d)", stCusHpfCoefsCfg.s32HpfCoef[0],
         stCusHpfCoefsCfg.s32HpfCoef[1],
         stCusHpfCoefsCfg.s32HpfCoef[2], stCusHpfCoefsCfg.s32HpfCoef[3], stCusHpfCoefsCfg.s32HpfCoef[4], /* 2, 3, 4 */
         stCusHpfCoefsCfg.s32HpfCoef[5], stCusHpfCoefsCfg.s32HpfCoef[6], stCusHpfCoefsCfg.s32HpfCoef[7]); /* 5, 6, 7 */
}

void PrintSrsInfoSubOne(HI_BOOL bEnable, const HI_MW_AUDIO_SRS_S &stSrs)
{
    const HI_MW_SRS_MAIN_CONFIG_S &stMainCfg = stSrs.stSrsConfig.stMainConfig;
    const HI_MW_SRS_TVOLHD_CONFIG_S &stTVolHDCfg = stSrs.stSrsConfig.stTVolHDConfig;
    const HI_MW_SRS_TRUDLG_CONFIG_S &stTruDiagCfg = stSrs.stSrsConfig.stTruDiagConfig;
    const HI_MW_SRS_CC3D_CONFIG_S &stCC3DCfg = stSrs.stSrsConfig.stCC3DConfig;
    const HI_MW_SRS_CSHP_CONFIG_S &stCSHPCfg = stSrs.stSrsConfig.stCSHPConfig;
    const HI_MW_SRS_COMM_CONFIG_S &stCommonCfg = stSrs.stSrsConfig.stCommonConfig;
    const HI_MW_SRS_GEQ_CONFIG_S &stGEQCfg = stSrs.stSrsConfig.stGEQConfig;
    const HI_MW_SRS_AEQ_CONFIG_S &stAEQCfg = stSrs.stSrsConfig.stAEQConfig;
    const HI_MW_SRS_HPF_CONFIG_S &stHPFCfg = stSrs.stSrsConfig.stHPFConfig;
    const HI_MW_SRS_MVOL_CONFIG_S &stMvolCfg = stSrs.stSrsConfig.stMvolConfig;
    const HI_MW_SRS_HDLMT_CONFIG_S &stHdLmtCfg = stSrs.stSrsConfig.stHdLmtConfig;
    const HI_MW_SRS_TONE_CONFIG_S &stToneCfg = stSrs.stSrsConfig.stToneConfig;

    LOGI("    Main config(%d, %d, %f, %f)", stMainCfg.bEnable, stMainCfg.enMainTech, stMainCfg.f32HeadroomGain,
         stMainCfg.f32BypassGain);
    LOGI("  TVOLHD config(%d, %f, %f)", stTVolHDCfg.bEnable, stTVolHDCfg.f32LKFS, stTVolHDCfg.f32LevelIntensity);
    LOGI("  TRUDLG config(%d, %f, %f)", stTruDiagCfg.bEnable, stTruDiagCfg.f32ClarityGain, stTruDiagCfg.f32ProcessGain);
    LOGI(" CC3DCFG config(%d, %d, %d, %d   %f, %f, %f, %f, %f)", stCC3DCfg.bDlgClarityEnable, stCC3DCfg.bSpaceHPFEnable,
         stCC3DCfg.bCC3DSurroundBoostEnable, stCC3DCfg.enSpaceHPFCutoffFreq, stCC3DCfg.f32DlgClarityLevel,
         stCC3DCfg.f32TruSurLevel, stCC3DCfg.f32Srs3DSpaceLevel,
         stCC3DCfg.f32Srs3DCenterLevel, stCC3DCfg.f32FocusLevel);
    LOGI("    CSHP config(%d, %d, %d, %f)", stCSHPCfg.enCSDEnable, stCSHPCfg.enCSDMode, stCSHPCfg.bDlgClarityEnable,
         stCSHPCfg.f32DlgClarityLevel);
    LOGI("    COMM config(%d, %d, %d,   %d, %d,  %d, %d,  %f, %f,  %f ,%f)",
         stCommonCfg.bTBHDEnable, stCommonCfg.stTBHDSpeakerSize.enAudioFilter,
         stCommonCfg.stTBHDSpeakerSize.enAnalysisFilter, stCommonCfg.bTBHDSplitAnalysisEnable,
         stCommonCfg.bTBHDLevelIndependentEnable, stCommonCfg.enTBHDMode, stCommonCfg.bDefEnable,
         stCommonCfg.f32TBHDLevel, stCommonCfg.f32TBHDCompressorLevel,
         stCommonCfg.f32TBHDBassClarityLevel, stCommonCfg.f32DefLevel);
    LOGI("     GEQ config(%d, %f, %f, %f, %f, %f)", stGEQCfg.bEnable,
        stGEQCfg.f32bgain[0], stGEQCfg.f32bgain[1], stGEQCfg.f32bgain[2], /* index is 0, 1, 2 */
        stGEQCfg.f32bgain[3], stGEQCfg.f32bgain[4]); /* index is 3, 4 */
    LOGI("     AEQ config(%d, %f)", stAEQCfg.bEnable, stAEQCfg.f32BypassGain);
    LOGI("     HPF config(%d)", stHPFCfg.bEnable);
    LOGI("    Mvol config(%d, %d, %d, %d)", stMvolCfg.bEnable, stMvolCfg.u32VolStep,
        stMvolCfg.s32LRBalance, stMvolCfg.bMute);
    LOGI("   HdLmt config(%d, %d,   %f, %d, %f)", stHdLmtCfg.bEnable, stHdLmtCfg.bACEnable,
         stHdLmtCfg.f32OutputGain, stHdLmtCfg.s32DelayLen, stHdLmtCfg.f32ACLevel);
    LOGI("    Tone config(%d, %d, %d)", stToneCfg.bEnable, stToneCfg.s32BassGain, stToneCfg.s32TrebleGain);
}

/************* get name string of data type for proc/log *************/
const HI_CHAR *GetSrcName(const HI_MW_SRC_E enSrc)
{
    if (enSrc > HI_MW_SRC_BUTT || enSrc < HI_MW_SRC_ATV) {
        return "NO THIS src";
    }

    static const HI_CHAR *sourceName[HI_MW_SRC_BUTT + 1] = {
        "[0]ATV",      // 0
        "[1]DVBC",     // 1
        "[2]DTMB",     // 2
        "[3]CVBS1",    // 3
        "[4]CVBS2",    // 4
        "[5]CVBS3",    // 5
        "[6]VGA",      // 6
        "[7]YPBPR1",   // 7
        "[8]YPBPR2",   // 8
        "[9]HDMI1",    // 9
        "[10]HDMI2",   // 10
        "[11]HDMI3",   // 11
        "[12]HDMI4",   // 12
        "[13]MEDIA1",  // 13
        "[14]MEDIA2",  // 14
        "[15]SCART1",  // 15
        "[16]SCART2",  // 16
        "[17]AUTO",    // 17
        "[18]DVBT",    // 18
        "[19]ATSC",    // 19
        "[20]DVBS",    // 20
        "[21]ISDBT",   // 21
        "[22]BUTT"     // 22
    };

    return sourceName[enSrc];
}

const HI_CHAR *GetEdidTypeName(const HI_MW_HDMIRX_EDID_TYPE_E enEdidType)
{
    switch (enEdidType) {
        case HI_MW_HDMIRX_EDID_TYPE_14:
            return "1.4";
        case HI_MW_HDMIRX_EDID_TYPE_20:
            return "2.0";
        case HI_MW_HDMIRX_EDID_TYPE_21:
            return "2.1";
        default:
            return "auto";
    }
}

const HI_CHAR *GetSndChannelName(const HI_MW_AUDIO_CHANNEL_E enChannel)
{
    if (enChannel > HI_MW_AUDIO_CHANNEL_BUTT || enChannel < HI_MW_AUDIO_CHANNEL_SPEAKER) {
        return "NO THIS channel";
    }

    static const HI_CHAR *soundChannelType[HI_MW_AUDIO_CHANNEL_BUTT + 1] = {
        "[0]Speaker",    // 0
        "[1]HeadPhone",  // 1
        "[2]BlueTooth",  // 2
        "[3]ARC",        // 3
        "[4]Spdif",      // 4
        "[5]AVOUT",      // 5
        "[6]24G",        // 6
        "[7]I2S0",       // 7
        "[8]HDMITX",     // 8
        "[9]BUTT",       // 9
    };

    return soundChannelType[enChannel];
}

const HI_CHAR *GetOutportName(const HI_MW_SND_OUTPORT_E enOutPort)
{
    switch (enOutPort) {
        case HI_MW_SND_OUTPORT_DAC0:
            return "[0]DAC0";
        case HI_MW_SND_OUTPORT_I2S0:
            return "[1]I2S0(I2S0)";
        case HI_MW_SND_OUTPORT_I2S1:
            return "[2]I2S1(Speaker)";
        case HI_MW_SND_OUTPORT_I2S3:
            return "[0x13]I2S3(Speaker)";
        case HI_MW_SND_OUTPORT_SPDIF0:
            return "[3]SPDIF0(Spdif)";
        case HI_MW_SND_OUTPORT_HDMI0:
            return "[4]HDMI0";
        case HI_MW_SND_OUTPORT_ARC0:
            return "[5]ARC0(Arc)";
        case HI_MW_SND_OUTPORT_DAC1:
            return "[0x50]DAC1";
        case HI_MW_SND_OUTPORT_DAC2:
            return "[0x51]DAC2";
        case HI_MW_SND_OUTPORT_DAC3:
            return "[0x52]DAC3";
        case HI_MW_SND_OUTPORT_ALL:
            return "ALL";
        case HI_MW_SND_OUTPORT_BUTT:
            return "BUTT";
        default:
            return "No this type";
    }
}

const HI_CHAR *GetSndMuteTypeName(const HI_MW_SND_MUTE_TYPE_E enType)
{
    if (enType > HI_MW_SND_MUTE_BUTT || enType < HI_MW_SND_MUTE_IRCTRL) {
        return "No this type";
    }

    static const HI_CHAR *muteTypeArray[HI_MW_SND_MUTE_BUTT + 1] = {
        "[0]IR Ctrl",      // 0
        "[1]Policy",       // 1
        "[2]Signal",       // 2
        "[3]Select Src",   // 3
        "[4]Select Prog",  // 4
        "[5]Scan Prog",    // 5
        "[6]Focus",        // 6
        "[7]Config",       // 7
        "[8]Headphone",    // 8
        "[9]Spdif",        // 9
        "[10]Arc",         // 10
        "[11]Src Lock",    // 11
        "[12]Chn Lock",    // 12
        "[13]Par Lock",    // 13
        "[14]BUTT"         // 14
    };

    return muteTypeArray[enType];
}

const HI_CHAR *GetHdmiDataTypeName(const HI_MW_HDMI_DATA_TYPE_E enType)
{
    if (enType > HI_MW_HDMI_DATA_BUTT || enType < HI_MW_HDMI_DATA_PCM) {
        return "No this type";
    }

    static const HI_CHAR *hdmiDataType[HI_MW_HDMI_DATA_BUTT + 1] = {"PCM", "DD", "DDP",
        "DTS", "DTSHD", "TRUEHD", "UNKNOWN", "MAT", "BUTT" };

    return hdmiDataType[enType];
}

const HI_CHAR *GetHdmiFormatName(const HI_MW_HDMI_FORMAT_E enType)
{
    if (enType > HI_MW_HDMI_FORMAT_BUTT || enType < HI_MW_HDMI_FORMAT_HDMI) {
        return "No this type";
    }

    static const HI_CHAR *hdmiFormatName[HI_MW_HDMI_FORMAT_BUTT + 1] = { "HDMI", "DVI", "MHL", "BUTT" };

    return hdmiFormatName[enType];
}

const HI_CHAR *GetTrackModeName(const HI_MW_TRACK_MODE_E enType)
{
    if (enType > HI_MW_TRACK_MODE_BUTT || enType < HI_MW_TRACK_MODE_STEREO) {
        return "No this type";
    }

    static const HI_CHAR *trackMode[HI_MW_TRACK_MODE_BUTT + 1] = {
        "[0]STEREO", "[1]DOUBLE_MONO", "[2]DOUBLE_LEFT", "[3]DOUBLE_RIGHT",
        "[4]EXCHANGE", "[5]ONLY_RIGHT", "[6]ONLY_LEFT", "[7]MUTED", "[8]BUTT"
    };

    return trackMode[enType];
}

const HI_CHAR *GetWinTypeName(const HI_MW_WINDOW_E enType)
{
    if (enType > HI_MW_WINDOW_BUTT || enType < HI_MW_WINDOW_MAIN) {
        return "No this type";
    }

    static const HI_CHAR *windowType[HI_MW_WINDOW_BUTT + 1] = { "MAIN", "SUB", "AUTO", "BUTT" };

    return windowType[enType];
}

const HI_CHAR *GetScartFormatName(const HI_MW_SCART_SIGNAL_FORMAT_E enType)
{
    if (enType > HI_MW_SCART_SIGNAL_BUTT || enType < HI_MW_SCART_NO_SIGNAL) {
        return "No this type";
    }

    static const HI_CHAR *scartFormatType[HI_MW_SCART_SIGNAL_BUTT + 1] = {
        "[0]NO_SG",    // 0
        "[1]4_3",      // 1
        "[2]16_9",     // 2
        "[3]BUTT",     // 3
    };

    return scartFormatType[enType];
}

const HI_CHAR *GetAspectName(const HI_MW_ASPECT_E enType)
{
    if (enType > HI_MW_ASPECT_BUTT || enType < HI_MW_ASPECT_16_9) {
        return "No this type";
    }

    static const HI_CHAR *aspectType[HI_MW_ASPECT_BUTT + 1] = {
        "[0]16_9",      // 0
        "[1]14_9",      // 1
        "[2]4_3",       // 2
        "[3]FULL",      // 3
        "[4]P2P",       // 4
        "[5]AUTO",      // 5
        "[6]SUBTITLE",  // 6
        "[7]PERSON",    // 7
        "[8]CINEMA",    // 8
        "[9]PANORAMA",  // 9
        "[10]ZOOM",     // 10
        "[11]ZOOM1",    // 11
        "[12]ZOOM2",    // 12
        "[13]TRUTH",    // 13
        "[14]21_9",     // 14
        "[15]BUTT",     // 15
    };

    return aspectType[enType];
}

const HI_CHAR *GetAspectCfgTypeName(const HI_MW_ASPECT_CONFIG_TYPE_E enAspectCfgType)
{
    if (enAspectCfgType > HI_MW_ASPECT_CONFIG_TYPE_BUTT || enAspectCfgType < HI_MW_ASPECT_CONFIG_TYPE_USER) {
        return "No this type";
    }

    static const HI_CHAR *aspectCfgType[HI_MW_ASPECT_CONFIG_TYPE_BUTT + 1] = {
        "[0]USER",          // 0
        "[1]WSS",           // 1
        "[2]AFD",           // 2
        "[3]HDMI",          // 3
        "[4]ASPECTCHANGE",  // 4
        "[5]BUTT",          // 5
    };

    return aspectCfgType[enAspectCfgType];
}

const HI_CHAR *GetClrSpaceName(const HI_MW_COLOR_SPACE_E enType)
{
    switch (enType) {
        case HI_MW_COLOR_SPACE_UNKNOWN:
            return "UNKNOWN";
        case HI_MW_COLOR_SPACE_BT601_YUV_LIMITED:
            return "BT601_YUV_LIMITED";
        case HI_MW_COLOR_SPACE_BT601_YUV_FULL:
            return "BT601_YUV_FULL";
        case HI_MW_COLOR_SPACE_BT601_RGB_LIMITED:
            return "BT601_RGB_LIMITED";
        case HI_MW_COLOR_SPACE_BT601_RGB_FULL:
            return "BT601_RGB_FULL";
        case HI_MW_COLOR_SPACE_NTSC1953:
            return "NTSC1953";
        case HI_MW_COLOR_SPACE_BT470_SYSTEM_M:
            return "BT470_SYSTEM_M";
        case HI_MW_COLOR_SPACE_BT470_SYSTEM_BG:
            return "BT470_SYSTEM_BG";
        case HI_MW_COLOR_SPACE_BT709_YUV_LIMITED:
            return "BT709_YUV_LIMITED";
        case HI_MW_COLOR_SPACE_BT709_YUV_FULL:
            return "BT709_YUV_FULL";
        case HI_MW_COLOR_SPACE_BT709_RGB_LIMITED:
            return "BT709_RGB_LIMITED";
        case HI_MW_COLOR_SPACE_BT709_RGB_FULL:
            return "BT709_RGB_FULL";
        case HI_MW_COLOR_SPACE_BT2020_YUV_LIMITED:
            return "BT2020_YUV_LIMITED";
        case HI_MW_COLOR_SPACE_BT2020_YUV_FULL:
            return "BT2020_YUV_FULL";
        default:
            break;
    }
    const HI_CHAR *tmp = GetClrSpaceNameSubOne(enType);
    return tmp;
}

const HI_CHAR *GetClrSpaceNameSubOne(const HI_MW_COLOR_SPACE_E enType)
{
    switch (enType) {
        case HI_MW_COLOR_SPACE_BT2020_RGB_LIMITED:
            return "BT2020_RGB_LIMITED";
        case HI_MW_COLOR_SPACE_BT2020_RGB_FULL:
            return "BT2020_RGB_FULL";
        case HI_MW_COLOR_SPACE_REC709:
            return "REC709";
        case HI_MW_COLOR_SPACE_SMPT170M:
            return "SMPT170M";
        case HI_MW_COLOR_SPACE_SMPT240M:
            return "SMPT240M";
        case HI_MW_COLOR_SPACE_BT878:
            return "BT878";
        case HI_MW_COLOR_SPACE_XVYCC:
            return "XVYCC";
        case HI_MW_COLOR_SPACE_JPEG:
            return "JPEG";
        case HI_MW_COLOR_SPACE_RGB:
            return "RGB";
        case HI_MW_COLOR_SPACE_BUTT:
            return "BUTT";
        default:
            return "NOT THIS TYPE";
        }
}

const HI_CHAR *GetClrSysName(const HI_MW_COLOR_SYS_E enType)
{
    if (enType > HI_MW_COLOR_SYS_BUTT || enType < HI_MW_COLOR_SYS_AUTO) {
        return "No this type";
    }
    static const HI_CHAR *colorSysTypeArray[HI_MW_COLOR_SYS_BUTT + 1] = {
        "[0]ATUO", "[1]PAL", "[2]NTSC", "[3]SECAM", "[4]PALM", "[5]PALN",
        "[6]PAL60", "[7]NTSC443", "[8]NTSC50", "[9]BUTT"
    };

    return colorSysTypeArray[enType];
}

const HI_CHAR *GetVideoFormatName(const HI_MW_VIDEO_FORMAT_E enType)
{
    if (enType > HI_MW_FORMAT_RGB_BUTT || enType < HI_MW_FORMAT_YUV_SEMIPLANAR_422) {
        return "No this type";
    }
    static const HI_CHAR *videoFormatName[HI_MW_FORMAT_RGB_BUTT + 1] = {
        "YUV_SEMIPLANAR_422",      // 0
        "YUV_SEMIPLANAR_420",      // 1
        "YUV_SEMIPLANAR_400",      // 2
        "YUV_SEMIPLANAR_411",      // 3
        "YUV_SEMIPLANAR_422_1X2",  // 4
        "YUV_SEMIPLANAR_444",      // 5
        "YUV_SEMIPLANAR_420_UV",   // 6
        "YUV_PACKAGE_UYVY",        // 7
        "YUV_PACKAGE_YUYV",        // 8
        "YUV_PACKAGE_YVYU",        // 9
        "YUV_PACKAGE_YUV",         // 10
        "YUV_PLANAR_400",          // 11
        "YUV_PLANAR_411",          // 12
        "YUV_PLANAR_420",          // 13
        "YUV_PLANAR_422_1X2",      // 14
        "YUV_PLANAR_422_2X1",      // 15
        "YUV_PLANAR_444",          // 16
        "YUV_PLANAR_410",          // 17
        "YUV_BUTT",                // 18
        "RGB_SEMIPLANAR_444",      // 19
        "RGB_RGB888",              // 20
        "RGB_ARGB8888",            // 21
        "RGB_BUTT"
    };  // 22

    return videoFormatName[enType];
}

const HI_CHAR *GetBitWidthName(const HI_MW_PIXEL_BITWIDTH_E enType)
{
    if (enType > HI_MW_PIXEL_BITWIDTH_BUTT || enType < HI_MW_PIXEL_BITWIDTH_8BIT) {
        return "No this type";
    }
    static const HI_CHAR *bitWidthName[HI_MW_PIXEL_BITWIDTH_BUTT + 1] = { "8Bit", "10Bit", "12Bit", "16Bit", "BUTT" };

    return bitWidthName[enType];
}

const HI_CHAR *GetOverSampleName(const HI_MW_OVERSAMPLE_MODE_E enType)
{
    if (enType > HI_MW_OVERSAMPLE_BUTT || enType < HI_MW_OVERSAMPLE_1X) {
        return "No this type";
    }
    static const HI_CHAR *overSampleName[HI_MW_OVERSAMPLE_BUTT + 1] = { "1X", "2X", "4X", "BUTT" };

    return overSampleName[enType];
}

const HI_CHAR *GetFramPackingName(const HI_MW_FRAME_PACKING_TYPE_E enType)
{
    if (enType > HI_MW_FRAME_PACKING_TYPE_BUTT || enType < HI_MW_FRAME_PACKING_TYPE_NONE) {
        return "No this type";
    }
    static const HI_CHAR *framPackingName[HI_MW_FRAME_PACKING_TYPE_BUTT + 1] = {
        "NONE", "SIDE_BY_SIDE", "TOP_AND_BOTTOM", "TIME_INTERLACED", "FRAME_PACKING", "3D_TILE", "BUTT"
    };

    return framPackingName[enType];
}

const HI_CHAR *GetSampleRateName(const HI_MW_SAMPLE_RATE_E enType)
{
    switch (enType) {
        case HI_MW_SAMPLE_RATE_UNKNOWN:
            return "UNKNOWN";
        case HI_MW_SAMPLE_RATE_8K:
            return "8K";
        case HI_MW_SAMPLE_RATE_11K:
            return "11K";
        case HI_MW_SAMPLE_RATE_12K:
            return "12K";
        case HI_MW_SAMPLE_RATE_16K:
            return "16K";
        case HI_MW_SAMPLE_RATE_22K:
            return "22K";
        case HI_MW_SAMPLE_RATE_24K:
            return "24K";
        case HI_MW_SAMPLE_RATE_32K:
            return "32K";
        case HI_MW_SAMPLE_RATE_44K:
            return "44K";
        case HI_MW_SAMPLE_RATE_48K:
            return "48K";
        case HI_MW_SAMPLE_RATE_88K:
            return "88K";
        case HI_MW_SAMPLE_RATE_96K:
            return "96K";
        case HI_MW_SAMPLE_RATE_176K:
            return "176K";
        case HI_MW_SAMPLE_RATE_192K:
            return "192K";
        case HI_MW_SAMPLE_RATE_BUTT:
            return "BUTT";
        default:
            return "No this type";
    }
}

const HI_CHAR *GetSignalStatusName(HI_MW_SIGNAL_E enType)
{
    if (enType > HI_MW_SIGNAL_BUTT || enType < HI_MW_SIGNAL_SUPPORT) {
        return "No this type";
    }

    static const HI_CHAR *signalStatusName[HI_MW_SIGNAL_BUTT + 1] = {
        "[0]Support", "[1]NoSignal", "[2]unSupport", "[3]unStable", "[4]BUTT"
    };

    return signalStatusName[enType];
}

const HI_CHAR *GetPqHdrName(HI_MW_PQ_HDR_TYPE_E enType)
{
    if (enType > HI_MW_PQ_HDR_TYPE_BUTT || enType < HI_MW_PQ_HDR_TYPE_SDR) {
        return "No this type";
    }

    static const HI_CHAR *pqHdrName[HI_MW_PQ_HDR_TYPE_BUTT + 1] = {
        "[0]SDR", "[1]SDR_BT2020_NONCONSTLUM", "[2]SDR_BT2020_CONSTLUM", "[3]SDR_XVYCC", "[4]HDR10",
        "[5]HDR10PLUS", "[6]SLF", "[7]HLG", "[8]TECHNICOLOR", "[9]DOLBYVISION", "BUTT"
    };

    return pqHdrName[enType];
}

const HI_CHAR *GetTvdWorkModeName(HI_MW_TVD_WORK_MODE_E enType)
{
    if (enType > HI_MW_TVD_WORK_MODE_BUTT || enType < HI_MW_TVD_WORK_MODE_NORMAL) {
        return "No this type";
    }

    static const HI_CHAR *workModeName[HI_MW_TVD_WORK_MODE_BUTT + 1] = { "NORMAL", "CHAN_SCAN", "BUTT" };

    return workModeName[enType];
}

const HI_CHAR *GetVGAAdjustTypeName(HI_MW_VGA_ADJUST_E enType)
{
    if (enType > HI_MW_VGA_ADJUST_BUTT || enType < HI_MW_VGA_ADJUST_HPOS) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_VGA_ADJUST_BUTT + 1] = { "HPOS", "VPOS", "PHASE", "CLOCK", "BUTT" };

    return name[enType];
}

const HI_CHAR *GetHdmiRcpKeyName(HI_MW_HDMIRX_RCP_KEY_E enType)
{
    if (enType > HI_MW_HDMIRX_RCP_KEY_BUTT || enType < HI_MW_HDMIRX_RCP_KEY_SELECT) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_HDMIRX_RCP_KEY_BUTT + 1] = {
        "[0]SELECT", "[1]UP", "[2]DOWN", "[3]LEFT", "[4]RIGHT", "[5]ROOT_MENU", "[6]EXIT",
        "[7]PLAY", "[8]STOP", "[9]PAUSE", "[10]REWIND", "[11]FAST_FWD", "[12]BUTT"
    };

    return name[enType];
}

const HI_CHAR *GetHdmiAudFmtName(HI_MW_HDMI_AUDIO_FMT_E enType)
{
    switch (enType) {
        case HI_MW_HDMI_AUDIO_FMT_LPCM:
            return "[0]LPCM";
        case HI_MW_HDMI_AUDIO_FMT_LBR:
            return "[1]LBR";
        case HI_MW_HDMI_AUDIO_FMT_HBR:
            return "[8]HBR";
        case HI_MW_HDMI_AUDIO_FMT_BUTT:
            return "BUTT";
        default:
            return "No this type";
    }
}

const HI_CHAR *GetMhlSyncStatus(HI_MW_MHL_E enType)
{
    switch (enType) {
        case HI_MW_MHL_IN:
            return "IN";
        case HI_MW_MHL_OUT:
            return "OUT";
        case HI_MW_MHL_BUTT:
            return "BUTT";
        default:
            return "No this type";
    }
}

const HI_CHAR *GetVICscTypeName(HI_MW_VICAP_CSC_TYPE_E enType)
{
    if (enType > HI_MW_VICAP_CSC_TYPE_BUTT || enType < HI_MW_VICAP_CSC_TYPE_AUTO) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_VICAP_CSC_TYPE_BUTT + 1] = { "Auto", "RGB_full", "RGB_limit", "BUTT" };

    return name[enType];
}

const HI_CHAR *GetFreezModeName(HI_MW_FREEZE_E enType)
{
    switch (enType) {
        case HI_MW_FREEZE_BLACK:
            return "Black";
        case HI_MW_FREEZE_LAST:
            return "Last";
        case HI_MW_FREEZE_BUTT:
            return "BUTT";
        default:
            return "No this mode";
    }
}

const HI_CHAR *GetWinFreezeTypeName(HI_MW_VIDEO_FREEZE_TYPE_E enType)
{
    if (enType > HI_MW_VIDEO_FREEZE_BUTT || enType < HI_MW_VIDEO_FREEZE_SIGNAL) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_VIDEO_FREEZE_BUTT + 1] = {
        "[0]Signal", "[1]Src_Lock", "[2]Chn_Lock", "[3]Parent_Lock", "[4]BUTT"
    };

    return name[enType];
}

const HI_CHAR *GetAtvSystemName(const HI_MW_ATV_SYSTEM_E enType)
{
    if (enType > HI_MW_ATV_SYSTEM_BUTT || enType < HI_MW_ATV_SYSTEM_PAL_BG) {
        return "NO THIS ATV_SYSTEM";
    }

    static const HI_CHAR *name[HI_MW_ATV_SYSTEM_BUTT + 1] = {
        "[0]PAL_BG",      // 0
        "[1]PAL_DK",      // 1
        "[2]PAL_I",       // 2
        "[3]PAL_M",       // 3
        "[4]PAL_N",       // 4
        "[5]SECAM_BG",    // 5
        "[6]SECAM_DK",    // 6
        "[7]SECAM_L_PR",  // 7
        "[8]SECAM_LL",    // 8
        "[9]NTSC_M",      // 9
        "[10]NTSC_I",     // 10
        "[11]NTSC_DK",    // 11
        "[12]BUTT"        // 12
    };

    return name[enType];
}

const HI_CHAR *GetAudSysName(HI_MW_AUDSYS_E enType)
{
    if (enType > HI_MW_AUDSYS_UNKNOW || enType < HI_MW_AUDSYS_BG) {
        return "No this audio system type";
    }

    static const HI_CHAR *name[HI_MW_AUDSYS_UNKNOW + 1] = {
        "[0]BG", "[1]BG_A2", "[2]BG_NC", "[3]I",
        "[4]DK", "[5]DK1_A2", "[6]DK2_A2", "[7]DK3_A2", "[8]DK_NC", "[9]L",
        "[10]M", "[11]M_BTSC", "[12]M_A2", "[13]EIAJ", "[14]NSTD", "[15]UNKNW"
    };

    return name[enType];
}

const HI_CHAR *GetCountryName(HI_MW_COUNTRY_E enType)
{
    if (enType > HI_MW_COUNTRY_BUTT || enType < HI_MW_COUNTRY_CHINA) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_COUNTRY_BUTT + 1] = {
        "[0]CHINA", "[1]HONGKONG_CHINA", "[2]TAIWAN_CHINA", "[3]INDONESIA", "[4]MALAYSIA",
        "[5]SINGAPORE", "[6]THAILAND", "[7]VIETNAM", "[8]INDIA", "[9]USA", "[10]BRAZIL", // 10 Brazil is btsc
        "[11]AUSTRIA", "[12]PHILIPPINES", "[13]CANADA", "[14]MEXICO", "[15]CZECH", // 12 philippines is btsc
        "[16]DENMARK", "[17]FINLAND", "[18]FRANCE", "[19]GERMANY", "[20]GREECE",
        "[21]HUNGARY", "[22]ITALY", "[23]LUXEMBOURG", "[24]NETHERLANDS", "[25]NORWAY",
        "[26]POLAND", "[27]PORTUGAL", "[28]SPAIN", "[29]SWEDEN", "[30]SWITZERLAND",
        "[31]SWITZERLAND_FRA",  // 31, // use this table if menu language is french
        "[32]SWITZERLAND_ITA",  // 32, // use this table if menu language is italian
        "[33]JAPAN",            // 33,
        "[34]UNITED_KINGDOM",   // 34,
        "[35]KOREA",            // 35, // add new country or region enum following
        "[36]ARGENTINA", "[37]CHILE", "[38]PERU", "[39]URUGUAY", "[40]MYANMAR",
        "[41]RUSSIA", "[42]BELGIUM", "[43]TURKEY", "[44]SERBIA", "[45]CROATIA",
        "[46]ROMANIA", "[47]COLOMBIA", "[48]NEWZEALAND", "[49]AUSTRALIA", "BUTT" // 50
    };

    return name[enType];
}

const HI_CHAR *GetTvSystemName(HI_MW_TVSYSTEM_E enType)
{
    if (enType > HI_MW_TVSYSTEM_BUTT || enType < HI_MW_TVSYSTEM_ATV) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_TVSYSTEM_BUTT + 1] = {
        "[0]ATV",             // 0,
        "[1]DTMB_DVBC",       // 1,
        "[2]ATSC",            // 2,
        "[3]ISDBT",           // 3,
        "[4]DVBT_T2_C",       // 4,
        "[5]DVBT_T2_S_S2_C",  // 5,
        "BUTT"
    };  // 6

    return name[enType];
}

const HI_CHAR *GetWakeupModeName(HI_MW_SYSTEM_WAKEUP_E enType)
{
    if (enType > HI_MW_SYSTEM_WAKEUP_BUTT || enType < HI_MW_SYSTEM_WAKEUP_IR) {
        return "No this wakeup mode!";
    }

    static const HI_CHAR *name[HI_MW_SYSTEM_WAKEUP_BUTT + 1] = {
        "[0]IR", "[1]KEYLED", "[2]TIMEOUT", "[3]ETH", "[4]USB", "[5]GPIO",
        "[6]LSADC", "[7]CEC", "[8]UART", "[9]VGA", "[10]HDMI", "[11]SCART",
        "[12]TIMEOUT_BG", "[13]BUTT"
    };
    return name[enType];
}

const HI_CHAR *GetDemoModeName(HI_MW_DEMO_MODE_E enType)
{
    if (enType > HI_MW_DEMO_BUTT || enType < HI_MW_DEMO_DBDR) {
        return "No this demo mode!";
    }

    static const HI_CHAR *name[HI_MW_DEMO_BUTT + 1] = {
        "[0]DBDR", "[1]NR", "[2]Sharpness", "[3]DCI", "[4]WCG", "[5]MEMC",
        "[6]COLOR", "[7]SR", "[8]HDR", "[9]ALL", "[10]DOLBY", "BUTT"
    };
    return name[enType];
}

const HI_CHAR *GetPqModuleName(HI_MW_PQ_MODULE_E enType)
{
    if (enType > HI_MW_PQ_MODULE_BUTT || enType < HI_MW_PQ_MODULE_FMD) {
        return "No this pq module!";
    }

    static const HI_CHAR *name[HI_MW_PQ_MODULE_BUTT + 1] = {
        "[0]FMD", "[1]NR", "[2]DB", "[3]DR", "[4]DS", "[5]HSHARPNESS", "[6]SHARPNESS",
        "[7]CCCL", "[8]COLOR_CORING", "[9]BLUE_STRETCH", "[10]GAMMA", "[11]DBC", "[12]DCI",
        "[13]COLOR", "[14]SR", "[15]WCG", "[16]HDR", "[17]HDR10PLUS", "[18]IQTM",
        "[19]SDR2HDR", "[20]ALL", "[21]BUTT"
    };
    return name[enType];
}

const HI_CHAR *GetMtsName(HI_MW_SIF_OUTMODE_E enSifSoundMode)
{
    if (enSifSoundMode > HI_MW_SIF_OUTMODE_BUTT || enSifSoundMode < HI_MW_SIF_OUTMODE_MONO) {
        return "No this MtsMode!";
    }

    static const HI_CHAR *name[HI_MW_SIF_OUTMODE_BUTT + 1] = {
        "[0]MONO", "[1]STEREO",
        "[2]DUAL_A", "[3]DUAL_B", "[4]DUAL_AB",
        "[5]BTSC_MONO", "[6]BTSC_STEREO", "[7]BTSC_SAP",
        "[8]NICAM_FORCED_MONO", "[9]NICAM_MONO", "[10]NICAM_STEREO",
        "[11]NICAM_DUAL_A", "[12]NICAM_DUAL_B", "[13]NICAM_DUAL_AB",
        "[14]AUTO", "[15]BUTT"
    };
    return name[enSifSoundMode];
}

const HI_CHAR *GetAaosName(HI_MW_SIF_AAOS_MODE_E enAaosMode)
{
    if (enAaosMode > HI_MW_SIF_AAOS_MODE_BUTT || enAaosMode < HI_MW_SIF_AAOS_MODE_MONO) {
        return "No this AaosMode!";
    }

    static const HI_CHAR *name[HI_MW_SIF_AAOS_MODE_BUTT + 1] = {
        "[0]MONO", "[1]STEREO",
        "[2]DUAL", "[3]MONO_SAP", "[4]STEREO_SAP",
        "[5]NICAM_MONO", "[6]NICAM_STEREO", "[7]NICAM_DUAL", "[8]NICAM_FM_MOMO", "[9]BUTT"
    };
    return name[enAaosMode];
}

/* 将0-15的十进制数用二进制显示 */
const HI_CHAR *GetBinary(HI_U8 u8Decimal)
{
    if (u8Decimal > 15) { /* Decimal is > 15 */
        return "invalid_val";
    }

    static const HI_CHAR *name[16] = { /* array length  is 16 */
        "0[0000]", "1[0001]", "2[0010]", "3[0011]", "4[0100]", "5[0101]", "6[0110]", "7[0111]",
        "8[1000]", "9[1001]", "10[1010]a", "11[1011]b", "12[1100]c", "13[1101]d", "14[1110]e", "15[1111]f"
    };
    return name[u8Decimal];
}

const HI_CHAR *GetMwTimName(HI_MW_HDMITX_FMT_E enMwFormat)
{
    if (enMwFormat > HI_MW_HDMITX_FMT_BUTT || enMwFormat < HI_MW_HDMITX_FMT_AUTO) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_HDMITX_FMT_BUTT + 1] = {
        "[0]AUTO", "[1]861D_640X480_60", "[2]480P_60", "[3]720P_60",
        "[4]VESA_1366X768_60", "[5]1080P_50", "[6]1080P_60", "[7]3840X2160_30",
        "[8]3840X2160_50", "[9]3840X2160_60", "[10]BUTT"
    };
    return name[enMwFormat];
}

const HI_CHAR *GetClrFormatName(HI_MW_HDMITX_COLOR_FORMAT_E enClrFormat)
{
    if (enClrFormat > HI_MW_HDMITX_COLOR_BUTT || enClrFormat < HI_MW_HDMITX_COLOR_RGB444_8) {
        return "No this type";
    }

    static const HI_CHAR *name[HI_MW_HDMITX_COLOR_BUTT + 1] = {
        "[0]RGB444_8", "[1]RGB444_10", "[2]RGB444_12", "[3]YCBCR444_8", "[4]YCBCR444_10", "[5]YCBCR444_12",
        "[6]YCBCR422", "[7]YCBCR420_8", "[8]YCBCR420_10", "[9]YCBCR420_12", "[10]BUTT"
    };
    return name[enClrFormat];
}

/* source 有效性检查 */
HI_S32 SafeSrcCheck(HI_MW_SRC_E src)
{
    if (src != HI_MW_SRC_ATV && src != HI_MW_SRC_CVBS1
        && src != HI_MW_SRC_CVBS2 && src != HI_MW_SRC_CVBS3
        && src != HI_MW_SRC_VGA && src != HI_MW_SRC_YPBPR1
        && src != HI_MW_SRC_YPBPR2 && src != HI_MW_SRC_HDMI1
        && src != HI_MW_SRC_HDMI2 && src != HI_MW_SRC_HDMI3
        && src != HI_MW_SRC_HDMI4 && src != HI_MW_SRC_MEDIA
        && src != HI_MW_SRC_MEDIA2
        && src != HI_MW_SRC_DTMB
        && src != HI_MW_SRC_DVBC
        && src != HI_MW_SRC_DVBT
        && src != HI_MW_SRC_ATSC
        && src != HI_MW_SRC_DVBS
        && src != HI_MW_SRC_ISDBT) {
        LOGE("invalid source");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* 创建多级目录 */
HI_S32 MakeDirent(string path)
{
    string::size_type start = 0;
    string::size_type i = 0;
    string newPath;
    char acurrentPath[PATH_MAX] = {0};
    char acResolvedPath[PATH_MAX] = {0};
    struct stat st = {};
    LOGI("makedirent %s start", path.c_str());

    for (i = 0; i < path.size() && path[i] == ' '; ++i) {
        // 删除开头的空格
    }

    if (path.length() < i) { // the pos of substr is i
        LOGE("substr invalid para..");
        return HI_FAILURE;
    }

    path = path.substr(i);
    // 相对路径
    if (path[0] != '/') {
        // 获取当前路径
        getcwd(acurrentPath, sizeof(acurrentPath));
        newPath.assign(acurrentPath);
        start = newPath.size();
        newPath = newPath + "/" + path;
    } else {
        // 绝对路径
        newPath = path;
        start = 0;
    }
    if (newPath[newPath.size()] != '/') {
        newPath = newPath + "/";
    }
    // 创建各级目录
    for (i = start; i < newPath.size() && i != string::npos;) {
        i = newPath.find('/', i);
        if (i == string::npos) {
            continue;
        }
        realpath(newPath.substr(0, i + 1).c_str(), acResolvedPath);
        if (strlen(newPath.substr(0, i + 1).c_str()) > PATH_MAX - 1 || 0 == strlen(acResolvedPath)) {
            LOGE("file path:%s error!", newPath.substr(0, i + 1).c_str());
            return HI_FAILURE;
        }
        HI_S32 s32Ret = stat(acResolvedPath, &st);
        if (s32Ret != HI_SUCCESS && ENOENT == errno) {
            if (mkdir(acResolvedPath, 0755) != HI_SUCCESS) { /* xxx is 0755 */
                LOGE("mkdir %s failed", acResolvedPath);
                return HI_FAILURE;
            }
        } else if (s32Ret != HI_SUCCESS || !S_ISDIR(st.st_mode)) {
            LOGE("%s not dirent", acResolvedPath);
            return HI_FAILURE;
        }
        i++;
    }
    LOGI("makedirent %s success", acResolvedPath);
    return HI_SUCCESS;
}


HI_S32 RemoveDirent(const string &path)
{
    DIR *dirp = NULL;
    struct dirent *dir = NULL;
    struct stat st = {};
    char acResolvedPath[PATH_MAX] = {0};

    if (lstat(path.c_str(), &st) != HI_SUCCESS) {
        LOGE("file path:%s error!", acResolvedPath);
        return HI_FAILURE;
    }
    if (!S_ISDIR(st.st_mode)) {
        if (unlink(path.c_str()) != HI_SUCCESS) {
            LOGE("unlink %s failed", acResolvedPath);
            return HI_FAILURE;
        }
        return HI_SUCCESS;
    }

    realpath(path.c_str(), acResolvedPath);

    if (strlen(path.c_str()) > PATH_MAX - 1 || 0 == strlen(acResolvedPath)) {
        LOGE("file path:%s error!", path.c_str());
        return HI_FAILURE;
    }

    dirp = opendir(acResolvedPath);
    if (dirp == NULL) {
        LOGE("opendir failed");
        return HI_FAILURE;
    }

    for (dir = readdir(dirp); dir != NULL; dir = readdir(dirp)) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }

        string subPath(acResolvedPath);
        subPath = subPath + "/" + dir->d_name;

        if (RemoveDirent(subPath) != HI_SUCCESS) { // 递归删除
            closedir(dirp);
            return HI_FAILURE;
        }
    }
    closedir(dirp);
    if (rmdir(acResolvedPath) != HI_SUCCESS) { // delete dirent itself.
        LOGE("rmdir failed");
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_BOOL IsTifCecEnable()
{
    static HI_BOOL tifEnable = HI_FALSE;
    return tifEnable;
}


