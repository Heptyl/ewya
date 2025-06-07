/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __MW_DBO_H__
#define __MW_DBO_H__

#include <map>
#include "hi_type.h"
#include "hi_mw_db.h"
#include "hi_mw_common.h"

using namespace std;

/* 数据属性标示 */
enum HI_MW_ATTR_E {
    /* Factory Enum */
    HI_MW_ATTR_FAC = 0x0,
    HI_MW_ATTR_FAC_PANEL = 0x1,
    HI_MW_ATTR_FAC_CTRL = 0x2,
    HI_MW_ATTR_FAC_PANEL_INTF_ATTR = 0x3,
    HI_MW_ATRR_FAC_LVDS = 0x4,
    HI_MW_ATTR_FAC_POWER = 0x5,
    HI_MW_ATTR_FAC_DDR = 0x6,
    HI_MW_ATTR_FAC_EMMC = 0x7,
    HI_MW_ATTR_FAC_VBO = 0x8,
    HI_MW_ATTR_FAC_GMAC = 0x9,
    HI_MW_ATTR_FAC_USB = 0xa,
    HI_MW_ATTR_FAC_EMI = 0xb,
    HI_MW_ATTR_FAC_PQ = 0xc,
    HI_MW_ATTR_FAC_DEVICE = 0xd,
    HI_MW_ATTR_FAC_ADC = 0xe,
    HI_MW_ATTR_FAC_COLORTEMP = 0xf,
    HI_MW_ATTR_FAC_NLA_INFO = 0x10,

    /* PQ Gain */
    HI_MW_ATTR_PQ_SHARP_GAIN = 0x11,
    HI_MW_ATTR_PQ_ACM_GAIN = 0x12,
    HI_MW_ATTR_PQ_DCI_GAIN = 0x13,

    /* OverScan Enum */
    HI_MW_ATTR_ATV_OVERSCAN = 0x14,
    HI_MW_ATTR_CVBS_OVERSCAN = 0x15,
    HI_MW_ATTR_HDMI_OVERSCAN = 0x16,
    HI_MW_ATTR_YPBPR_OVERSCAN = 0x17,
    HI_MW_ATTR_DTV_OVERSCAN = 0x18,
    HI_MW_ATTR_MEDIA_OVERSCAN = 0x19,
    HI_MW_ATTR_FACTORY_PQ_GAIN = 0x1a,
    HI_MW_ATTR_SCART_OVERSCAN = 0x1b,
    //LANGO PATCH BEG
    HI_MW_ATTR_SND_VOL_CURVE_BASE = 0x1c,
    HI_MW_ATTR_SND_SPEAKER_VOL_CURVE = 0x1c,
    HI_MW_ATTR_SND_HEADPHONE_VOL_CURVE = 0x1d,
    //LANGO PATCH END

    HI_MW_ATTR_FAC_END,

    /* Picture Enum */
    HI_MW_ATTR_USR = 0x100,
    HI_MW_ATTR_PICTURE = 0x101,
    HI_MW_ATTR_PICTURE_PQ = 0x102,
    HI_MW_ATTR_PICTURE_DISP = 0x103,
    HI_MW_ATTR_PICTURE_VIDEO = 0x104,
    HI_MW_ATTR_PICTURE_EF = 0x105,
    HI_MW_ATTR_PICTURE_IMAGE = 0x106,
    HI_MW_ATTR_PICTURE_PANEL = 0x107,
    HI_MW_ATTR_PICTURE_HDR_FIXMODE = 0x108,
    HI_MW_ATTR_PICTURE_LOCALDIM = 0x109,

    /* Audio Enum */
    HI_MW_ATTR_AUDIO_SRS = 0x10a,
    HI_MW_ATTR_AUDIO_DBX = 0x10b,
    HI_MW_ATTR_AUDIO_CTRL = 0x10c,
    HI_MW_ATTR_AUDIO_BASE = 0x10d,
    HI_MW_ATTR_AUDIO_AMP = 0x10e,  // for AMP
    HI_MW_ATTR_AUDIO_AVC = 0x10f,
    HI_MW_ATTR_AUDIO_DRC = 0x110,
    HI_MW_ATTR_AUDIO_SOUND_MODE = 0x111,
    HI_MW_ATTR_AUDIO_PRECIGAIN = 0x112,
    HI_MW_ATTR_AUDIO_PEQ_QVALUE = 0x113,
    HI_MW_ATTR_AUDIO_PEQ_GAIN = 0x114,
    HI_MW_ATTR_AUDIO_SWS3D = 0x115,
    HI_MW_ATTR_AUDIO_DBX2 = 0x116,
    HI_MW_ATTR_AUDIO_DAP = 0x117,

    /* System && 3D Enum */
    HI_MW_ATTR_SYSTEM = 0x118,
    HI_MW_ATTR_SYSTEM_MAINSRC = 0x119,
    HI_MW_ATTE_3D_MODE = 0x11a,
    HI_MW_ATTR_PC_TIMMING = 0x11b,
    HI_MW_ATTR_SYSTEM_LOCK = 0x11c,
    HI_MW_ATTR_SYSTEM_CC = 0x11d,
    HI_MW_ATTR_SYSTEM_WAKEUP = 0x11e,

    /* CEC Enum */
    HI_MW_ATTR_CEC_MENU = 0x11f,

    /* Ntsc Scan Enum */
    HI_MW_ATTR_ATV_NTSC_SCAN = 0x120,

    /* Hdmi tx */
    HI_MW_ATTR_HDMITX_ATTR = 0x121,

    /* Hdmi Tif Cec Option */
    HI_MW_ATTR_TIF_CEC_OPTION = 0x122,

    HI_MW_ATTR_USR_END,

    /* ATV Enum */
    HI_MW_ATTR_ATV = 0x200,
    HI_MW_ATTR_ATV_CURRENT_PROG = 0x201,
    HI_MW_ATTR_ATV_FREEZE_MODE = 0x202,
    HI_MW_ATTR_ATV_INFO = 0x203,
    HI_MW_ATTR_ATV_NSTD = 0x204,

    HI_MW_ATTR_ATV_END,

    HI_MW_ATTR_BUTT = 0x300,
};

/* 数据库类型标示 */
enum HI_MW_DB_E {
    HI_MW_DB_FAC = HI_MW_ATTR_FAC,
    HI_MW_DB_USR = HI_MW_ATTR_USR,
    HI_MW_DB_CHN = HI_MW_ATTR_ATV,
    HI_MW_DB_BUTT = HI_MW_ATTR_BUTT
};

typedef struct HiMW_DBO_CB_S {
    HI_S32 (*pfnDbUpdateCb)(HiDB *pDb, HI_MW_ATTR_E enAttr, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row);
    HI_S32 (*pfnDbQueryCb)(HiDB *pDb, HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row);
    HI_S32(*pfnDbQueryManyCb)(HiDB *pDb, HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32StartRow,
           HI_U32 u32EndRow);
} HI_MW_DBO_CB_S;

class HiDBO {
public:
    /* 标准操作接口:插入数据 */
    HI_S32 Insert(HI_MW_ATTR_E enAttr, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Num = 1);

    /* 标准操作接口:删除数据 */
    HI_S32 Erase(HI_MW_ATTR_E enAttr, HI_U32 u32Row);
    HI_S32 Erase(HI_MW_ATTR_E enAttr);

    /* 标准操作接口:更新数据 */
    HI_S32 Update(HI_MW_ATTR_E enAttr, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row = 0);

    /* 标准操作接口:查询数据 */
    HI_S32 Query(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row = 0);

    /* 查询某个数据项的[u32StartRow,u32EndRow]区间内的所有记录,
    u32Length是一条记录的数据大小，非查询的所有数据的大小 */
    HI_S32 Query(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32StartRow, HI_U32 u32EndRow);
    /* 清空指定数据库 */
    HI_S32 Clear(HI_MW_DB_E enDBType);

    /* 获取同类数据的条数 */
    HI_S32 GetNum(HI_MW_ATTR_E enAttr, HI_U32 *pu32Num);

    /* 数据不同步到介质 */
    HI_S32 DisableSync(HI_MW_ATTR_E enAttr);

    /* 数据同步到介质 */
    HI_S32 EnableSync(HI_MW_ATTR_E enAttr, HI_BOOL bFlush = HI_FALSE);

    /* 数据与指定DB关联 */
    HI_S32 Attach(HI_MW_ATTR_E enAttr, HiDB *pDb);

    /* 获取指定类型DB */
    HI_S32 Obtain(HI_MW_DB_E enDBType, HiDB **pDb);

    /* 用户数据库恢复 */
    HI_VOID RestoreUsr();

    /* 工厂数据库恢复 */
    HI_VOID RestoreFac();

    /* 频道数据库恢复 */
    HI_VOID RestoreChn();

    HI_S32 RestorePic();
    HI_S32 RestorePicDbUsr();
    HI_S32 RestorePicDbFac();
    HI_S32 RestoreAud();
    HI_S32 RestoreAudEffect();
    HI_S32 RestoreAudSetting();
    HI_S32 RestoreSys();

    /* 重载按源查询 */
    HI_S32 Query(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_MW_SRC_E enSrc);

    /* 重载按源更新 */
    HI_S32 Update(HI_MW_ATTR_E enAttr, HI_U8 *pData, HI_U32 u32Length, HI_MW_SRC_E enSrc);

    /* 创建新数据库 */
    HI_U32 CreatDb(const string &dbFile, HI_U16 u16DbVersion, HiDB **ppDb);

    HI_U32 RegisterOpCb(const HI_MW_DBO_CB_S *pstDboCallBack);

private:
    HiDBO();
    ~HiDBO();

    friend class HiDBOFact;

private:
    map<HI_MW_ATTR_E, HiDB *> m_mapAttrDB;
    HI_MW_DBO_CB_S m_sDboCallBack;
};

#endif

