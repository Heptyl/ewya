/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_COMMON_H__
#define __HI_MW_COMMON_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <climits>
#include "hi_type.h"
#include "hi_mw_log.h"
#include "hi_mw_array.h"
#include "securec.h"

using namespace std;


/* 资源文件相对路径定义 */
static const HI_CHAR* INI_CFG_INI_PATH  = "usr/ini/cfg.ini";
static const HI_CHAR* INI_DB_INI_PATH  = "usr/ini/db.ini";
static const HI_CHAR* INI_AQ_INI_PATH  = "usr/ini/aq.ini";

#define HI_CHANNEL_ERR_BASE           (0xfffffa00)
#define HI_CHANNEL_ERR_CREATE_FAIL    (HI_CHANNEL_ERR_BASE + 1) /* channel Create fail */
#define HI_CHANNEL_ERR_DESTROY_FAIL   (HI_CHANNEL_ERR_BASE + 2) /* channel destroy fail */
#define HI_CHANNEL_ERR_CONFLICT       (HI_CHANNEL_ERR_BASE + 3) /* channel conflict */
#define HI_CHANNEL_ERR_NOT_FOUND      (HI_CHANNEL_ERR_BASE + 4) /* no any channel resource */
#define HI_CHANNEL_ERR_NULL_POINTER   (HI_CHANNEL_ERR_BASE + 5) /* null pointer */
#define HI_CHANNEL_ERR_INVALID_PARA   (HI_CHANNEL_ERR_BASE + 6) /* input para err */
#define HI_CHANNEL_ERR_INVALID_MEMERY (HI_CHANNEL_ERR_BASE + 7) /* memery invalid */
#define HI_CHANNEL_ERR_NOT_ALLOWED    (HI_CHANNEL_ERR_BASE + 8) /* operator not allowed */
#define HI_CHANNEL_ERR_NO_RESOURCE    (HI_CHANNEL_ERR_BASE + 9) /* no resource to used */
#define HI_CHANNEL_SCAN_FLAG          (0x7FFF)

#define HI_MW_MULTI_OUTPUT   (HI_TRUE)
#define HI_MW_OUTPUT_TIMMING (HI_MW_HDMITX_FMT_1080P_60)
#define HI_MW_OUTPUT_COLOR   (HI_MW_HDMITX_COLOR_YCBCR444_8)

#define MAX_PROGRAM_COUNT (256)
#define NAME_LENGTH       (19)
#define UI_MAX            (100)
#define UI_MIN            (0)
#define BALANCE_MAX       (50)
#define BALANCE_MIN       (-50)
#define UHD_WIDTH_4096    (4096)
#define UHD_WIDTH_3840    (3840)
#define UHD_HEIGHT        (2160)

#define FHD_WIDTH  (1920)
#define FHD_HEIGHT (1080)

#define SD_WIDTH       (720)
#define HUE_MID        (50)
#define MAX_CHAR_COUNT (255)
#define I2C_TOTAL      (8)
#define INVALID_FREQ   (826000)

/* The length of language code */
#define HI_MW_LANG_CODE_LEN         (3)
#define PC_MODE_SAVE_MAX            (10)
#define HISTOGRAM_COUNT             (32)
#define HI_MW_SRS_MAX_GEQ_5BAND_NUM (5)
#define HI_MW_SWS3D_EQ_BANDS_MAXNUM (12)
#define HI_MW_SWS3D_MBC_BANDS       (3)
#define HI_MW_VOLUME_NUMBER_MAX  (11)


/* SRS GEQ范围映射,用户范围:0~100,驱动范围:-6.0~6.0dB,精度:0.125dB */
#define GEQ_CONVERT_RANGE_SRS(uservalue) \
    (((uservalue) - (UI_MIN)) * ((6.0) - (-6.0)) / ((UI_MAX) - (UI_MIN)) + (-6.0))

/* 自研GEQ范围映射,用户范围:0~100,驱动范围:-15.0~15.0dB,精度:0.125dB */
#define GEQ_CONVERT_RANGE_SWS(uservalue) \
    (((uservalue) - (UI_MIN)) * ((15.0) - (-15.0)) / ((UI_MAX) - (UI_MIN)) + (-15.0))

#define POWERON_PARTITION_NAME    "deviceinfo"
#define MAC_LENTH                 (18)
#define SYS_EEPROM_DEVICE_ADDRESS (0xA0)
#define SYS_EEPROM_DEVICE_LENGTH  (24)
#define UART_SWITCH_OFFSET        (0x3000)
#define LDM_MAX_HOR_NUM           (48)
#define LDM_MAX_VER_NUM           (32)
#define LDM_MAX_TOTAL_NUM         ((LDM_MAX_HOR_NUM) * (LDM_MAX_VER_NUM))

/* VGA Adjust Para */
#define SETTING_RANGE_POSITION_H (30) /* 在已经校正的基础上允许调节H Position 的最大+/- 范围 */
#define SETTING_RANGE_POSITION_V (30) /* 在已经校正的基础上允许调节V Position 的最大+/- 范围 */
#define SETTING_RANGE_CLOCK      (10) /* 在已经校正的基础上允许调节Clock 的最大+/- 范围 */
#define SETTING_RANGE_PHASE      (30) /* 在已经校正的基础上允许调节Phase 的最大+/- 范围 */

#define OPTIMAL_VALUE_POSITION_H (50) /* 自动校验后的最优H Position 值 */
#define OPTIMAL_VALUE_POSITION_V (50) /* 自动校验后的最优V Position 值 */
#define OPTIMAL_VALUE_CLOCK      (50) /* 自动校验后的最优Clock 值 */
#define OPTIMAL_VALUE_PHASE      (50) /* 自动校验后的最优Phase 值 */
#define DRV_VALUE_PHSAE_MAX      (31) /* 驱动最大Phase值 */

/* NTSC scan channel list number */
#define NTSC_SCAN_AIR_PROGRAM_MAX   (69)
#define NTSC_SCAN_CABLE_PROGRAM_MAX (135)

/* file path max length */
#define FILE_PATH_MAX_LEN (2048)

#define REGION_INDEX_MAX (32)

#define DISP_ABL_MAX_H_NUM (32)
#define DISP_ABL_MAX_V_NUM (20)

#define HI_MW_SND_TRACK_MAX (8)

/* panel config */
#define PANEL_SPREAD_RATIO_MIN (0)
#define PANEL_SPREAD_RATIO_MAX (31)
#define PANEL_SPREAD_NOTTCON_MIN 2
#define PANEL_SPREAD_NOTTCON_MAX 5

/* 3D config */
#define HI_UI_3D_DEPTH_MAX  (10)
#define HI_UI_3D_DEPTH_MIN  (0)
#define HI_UI_3D_VIEW_MAX   (10)
#define HI_UI_3D_VIEW_MIN   (0)

/* 8k码流非标处理限制范围 */
#define HI_MW_8K_WIDTH              7680
#define HI_MW_8K_HEIGHT             4320
#define HI_MW_MEDIA_8K_WIDTH_LIMIT  6000
#define HI_MW_MEDIA_8K_HEIGHT_LIMIT 2000

/* PQ 曲线参数定义 */
#define PQ_DCI_CURVE_POINT_NUM (33)

HI_S32 GetAndroidBootComplete();

Array<HI_U8> GetBoardSourceList();
Array<HI_U8> GetAllBoardSourceList();
HI_BOOL GetBoardSupportScart();
const HI_U8 *GetBoardSourceIdxList();

void GetBoardSourceListSubOne(Array<HI_BOOL> enable, HI_U32 index);
void GetBoardSourceListSubTwo(Array<HI_BOOL> enable, HI_U32 index);

#define DEBUG

#ifdef NOLINK
#define LNK(function) (HI_SUCCESS)
#else
#define LNK(function) (function)
#endif

/* [according to return print different color log]
 * Param:[1] s32Ret   : return code to be checked.
 *       [2] ...   : variable param, support to print user defined log with params.
 * [根据返回值打印不同颜色级别的调试信息]
 * 参数: [1] s32Ret   : 要检查的函数返回值
 *       [3] ...   : 可变参数，支持自定义的打印信息，包括支持打印变量值.参数不能为空 */
#define RET_CHECK_DEBUG_LOG(s32Ret, ...) \
    do {                                 \
        if (s32Ret == HI_SUCCESS) {      \
            LOGD(__VA_ARGS__);           \
        } else {                         \
            LOGE(__VA_ARGS__);           \
        }                                \
    } while (0)

#define RET_CHECK_ERR_LOG(s32Ret, ...) \
    do {                               \
        if (s32Ret != HI_SUCCESS) {    \
            LOGE(__VA_ARGS__);         \
        }                              \
    } while (0)

#define RET_CODE_CHECK(s32Ret, rtnFlg) \
    do {                           \
        if (HI_SUCCESS != s32Ret){ \
            LOGE("call failed");   \
            if(rtnFlg){            \
                return s32Ret;     \
            }                      \
            s32Ret = HI_SUCCESS;   \
        }                          \
    } while (0)

#define RET_CODE_CHECK_LOG(s32Ret, rtnFlg, ...)\
    do {                                       \
        if (HI_SUCCESS != s32Ret){             \
            LOGE(__VA_ARGS__);                 \
            if(rtnFlg){                        \
                return s32Ret;                 \
            }                                  \
            s32Ret = HI_SUCCESS;               \
        }                                      \
    } while(0)




            

#define HI_MW_MSLEEP(ms)                    \
    do {                                    \
        struct timespec ts;                 \
        ts.tv_sec = ms / 1000;              \
        ts.tv_nsec = (ms % 1000) * 1000000; \
        nanosleep(&ts, NULL);               \
    } while (0)

/* 检查空指针 */
#define NULL_PTR_CHECK(ptr) \
    do { \
        if(HI_NULL == ptr){ \
            LOGE("%s is null ptr", #ptr);\
            return HI_FAILURE; \
        } \
    } while(0)

/* 输入信号源序号定义 */
enum HI_MW_SRC_E {
    HI_MW_SRC_ATV = 0,

    HI_MW_SRC_DVBC = 1,
    HI_MW_SRC_DTMB = 2,

    HI_MW_SRC_CVBS1 = 3,
    HI_MW_SRC_CVBS2 = 4,
    HI_MW_SRC_CVBS3 = 5,
    HI_MW_SRC_CVBS_MAX = HI_MW_SRC_CVBS3,

    HI_MW_SRC_VGA = 6,

    HI_MW_SRC_YPBPR1 = 7,
    HI_MW_SRC_YPBPR2 = 8,
    HI_MW_SRC_YPBPR_MAX = HI_MW_SRC_YPBPR2,

    HI_MW_SRC_HDMI1 = 9,
    HI_MW_SRC_HDMI2 = 10,
    HI_MW_SRC_HDMI3 = 11,
    HI_MW_SRC_HDMI4 = 12,
    HI_MW_SRC_HDMI_MAX = HI_MW_SRC_HDMI4,

    HI_MW_SRC_MEDIA = 13,
    HI_MW_SRC_MEDIA1 = HI_MW_SRC_MEDIA,
    HI_MW_SRC_MEDIA2 = 14,
    HI_MW_SRC_MEDIA_MAX = HI_MW_SRC_MEDIA2,

    HI_MW_SRC_SCART1 = 15,
    HI_MW_SRC_SCART2 = 16,
    HI_MW_SRC_SCART_MAX = HI_MW_SRC_SCART2,

    HI_MW_SRC_AUTO = 17,

    HI_MW_SRC_DVBT = 18,
    HI_MW_SRC_ATSC = 19,
    HI_MW_SRC_DVBS = 20,
    HI_MW_SRC_ISDBT = 21,

    HI_MW_SRC_BUTT = 22,
};

/**panel DIVISION type */
enum HI_MW_PANEL_DIVISION_TYPE_E
{
    HI_MW_PANEL_DIVISION_ONE_OE = 0,       /**<DIVISION type is odd_even */
    HI_MW_PANEL_DIVISION_ONE,              /**<DIVISION type is one */
    HI_MW_PANEL_DIVISION_TWO,              /**<DIVISION type is two*/
    HI_MW_PANEL_DIVISION_FOUR,             /**<DIVISION type is four*/
    HI_MW_PANEL_DIVISION_BUTT,             /**<Invalid value*/
};
/* 定义3D模式的枚举 */
enum HI_MW_3DMODE_E {
    HI_MW_3DMODE_OFF = 0,   /* 3D off: 2D  */
    HI_MW_3DMODE_2DT3D = 1, /* 2D转3D */
    HI_MW_3DMODE_SBS = 2,   /* Side By Side */
    HI_MW_3DMODE_TAB = 3,   /* Top And Bottom */
    HI_MW_3DMODE_FP = 4,    /* frame paking:MVC */
    HI_MW_3DMODE_LI = 5,    /* Line Interlace 水平交织 */
    HI_MW_3DMODE_AUTO = 6,  /* 3D Auto */

    HI_MW_3DMODE_BUTT = 7,
};

/* *Defines 3D frame packing type */
enum HI_MW_FRAME_PACKING_TYPE_E {
    HI_MW_FRAME_PACKING_TYPE_NONE = 0,            /* Normal frame, not a 3D frame */
    HI_MW_FRAME_PACKING_TYPE_SIDE_BY_SIDE = 1,    /* Side by side */
    HI_MW_FRAME_PACKING_TYPE_TOP_AND_BOTTOM = 2,  /* Top and bottom */

    /* Time interlaced: one frame for left eye, the next frame for right eye */
    HI_MW_FRAME_PACKING_TYPE_TIME_INTERLACED = 3,
    HI_MW_FRAME_PACKING_TYPE_FRAME_PACKING = 4,   /* frame packing */
    HI_MW_FRAME_PACKING_TYPE_3D_TILE = 5,         /* Tile 3D */

    HI_MW_FRAME_PACKING_TYPE_BUTT = 6,
};

/* 定义3D显示格式的枚举 */
enum HI_MW_3DT2D_E {
    HI_MW_3DT2D_OFF = 0, /* 3D转2D关闭 */
    HI_MW_3DT2D_L = 1,   /* 3D转2D左眼 */
    HI_MW_3DT2D_R = 2,   /* 3D转2D右眼 */

    HI_MW_3DT2D_BUTT = 3,
};

/* 定义2D转3D的场景模式 */
enum HI_MW_2DT3D_SCENE_E {
    HI_MW_2DT3D_SCENE_MODE_OFF = 0,
    HI_MW_2DT3D_SCENE_MODE_1 = 1,
    HI_MW_2DT3D_SCENE_MODE_2 = 2,
    HI_MW_2DT3D_SCENE_MODE_3 = 3,
    HI_MW_2DT3D_SCENE_MODE_4 = 4,
    HI_MW_2DT3D_SCENE_MODE_5 = 5,
    HI_MW_2DT3D_SCENE_MODE_6 = 6,
    HI_MW_2DT3D_SCENE_MODE_7 = 7,
    HI_MW_2DT3D_SCENE_MODE_8 = 8,
    HI_MW_2DT3D_SCENE_MODE_9 = 9,
    HI_MW_2DT3D_SCENE_MODE_10 = 10,
    HI_MW_2DT3D_SCENE_MODE_CUSTOME = 11,
    HI_MW_2DT3D_SCENE_MODE_AUTO_1 = 12,
    HI_MW_2DT3D_SCENE_MODE_AUTO_2 = 13,

    HI_MW_2DT3D_SCENE_MODE_TOTAL = 14,
};

enum HI_MW_VIDEO_USERDATA_TYPE_E {
    HI_MW_VIDEO_USERDATA_UNKNOWN = 0,
    HI_MW_VIDEO_USERDATA_DVB1_CC = 0x1,
    HI_MW_VIDEO_USERDATA_DVB1_BAR = 0x2,
    HI_MW_VIDEO_USERDATA_AFD = 0x10000,

    HI_MW_VIDEO_USERDATA_BUTT = 0x10001,
};

enum HI_MW_VIDEO_BROADCAST_PROFILE_E {
    HI_MW_VIDEO_BROADCAST_DVB = 0,
    HI_MW_VIDEO_BROADCAST_DIRECTV = 1,
    HI_MW_VIDEO_BROADCAST_ATSC = 2,
    HI_MW_VIDEO_BROADCAST_DVD = 3,
    HI_MW_VIDEO_BROADCAST_ARIB = 4,

    HI_MW_VIDEO_BROADCAST_BUTT = 5,
};

enum HI_MW_VIDEO_USER_DATA_POSITION_E {
    HI_MW_VIDEO_USER_DATA_POSITION_UNKNOWN = 0,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG2_SEQ = 1,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG2_GOP = 2,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG2_FRAME = 3,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG4_VSOS = 4,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG4_VSO = 5,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG4_VOL = 6,
    HI_MW_VIDEO_USER_DATA_POSITION_MPEG4_GOP = 7,
    HI_MW_VIDEO_USER_DATA_POSITION_H264_REG = 8,
    HI_MW_VIDEO_USER_DATA_POSITION_H264_UNREG = 9,

    HI_MW_VIDEO_USER_DATA_POSITION_BUTT = 10,
};

/* vicap csc type 选择 */
typedef enum HiMW_VICAP_CSC_TYPE_E {
    HI_MW_VICAP_CSC_TYPE_AUTO = 0,
    HI_MW_VICAP_CSC_TYPE_RGB_FULL = 1,
    HI_MW_VICAP_CSC_TYPE_RGB_LIMIT = 2,

    HI_MW_VICAP_CSC_TYPE_BUTT = 3,
} HI_MW_VICAP_CSC_TYPE_E;

/* PC  Mode */
enum HI_MW_PCMODE_E {
    HI_MW_PCMODE_AUTO = 0,
    HI_MW_PCMODE_PC = 1,
    HI_MW_PCMODE_VIDEO = 2,

    HI_MW_PCMODE_BUTT = 3,
};

/* define the enum of ADC index */
enum HI_MW_ADC_INDEX_E {
    HI_MW_ADC_VGA = 0,
    HI_MW_ADC_YPBPR1 = 1,
    HI_MW_ADC_YPBPR2 = 2,
    HI_MW_ADC_SCART_RGB = 3,

    HI_MW_ADC_INDEX_BUTT = 4,
};

enum HI_MW_WINDOW_E {
    HI_MW_WINDOW_MAIN = 0,
    HI_MW_WINDOW_SUB = 1,
    HI_MW_WINDOW_AUTO = 2,

    HI_MW_WINDOW_BUTT = 3,
};

enum HI_MW_SCART_INPUT_E {
    HI_MW_SCART_INPUT_AV = 0,
    HI_MW_SCART_INPUT_SVIDEO = 1,
    HI_MW_SCART_INPUT_RGB = 2,

    HI_MW_SCART_INPUT_BUTT = 3,
};

enum HI_MW_SCREEN_COLOR_E {
    HI_MW_SCREEN_BLACK = 0,
    HI_MW_SCREEN_BLUE = 1,

    HI_MW_SCREEN_BUTT = 2,
};

enum HI_MW_LANGUAGE_E {
    HI_MW_LANG_CHINESE = 0,
    HI_MW_LANG_ENGLISH = 1,

    HI_MW_LANG_BUTT = 2,
};

/* defination here is different with HI_UNF_WINDOW_FREEZE_MODE_E! convert in hi_mw_drv_video_device.cpp */
/* window freeze mode def */
enum HI_MW_FREEZE_E {
    HI_MW_FREEZE_LAST = 0,  /* freeze window with last frame */
    HI_MW_FREEZE_BLACK = 1, /* freeze window with specify color,such as blue or black */

    HI_MW_FREEZE_BUTT = 2,
};

/* 定义video的mute类型 */
enum HI_MW_VIDEO_FREEZE_TYPE_E {
    HI_MW_VIDEO_FREEZE_SIGNAL = 0,       /* 无信号 mute */
    HI_MW_VIDEO_FREEZE_SOURCE_LOCK = 1,  /* 切源锁 mute */
    HI_MW_VIDEO_FREEZE_CHANNEL_LOCK = 2, /* 频道锁 mute */
    HI_MW_VIDEO_FREEZE_PARENT_LOCK = 3,  /* 父母锁 mute */

    HI_MW_VIDEO_FREEZE_BUTT = 4,
};

/* 视频区域描述 */
struct HI_MW_RECT_S {
    HI_U32 u32X;
    HI_U32 u32Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
};

struct HI_MW_RECT_EXT_S {
    HI_U32 u32X;
    HI_U32 u32Y;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_BOOL bFullScreen;
};

enum HI_MW_PANEL_ASPECT_E {
    HI_MW_PANEL_ASPECT_4_3 = 0,
    HI_MW_PANEL_ASPECT_16_9 = 1,
    HI_MW_PANEL_ASPECT_14_9 = 2,
    HI_MW_PANEL_ASPECT_21_9 = 3,
    HI_MW_PANEL_ASPECT_16_10 = 4,

    HI_MW_PANEL_ASPECT_BUTT = 5,
};

/* 屏的类型 */
enum HI_MW_DISP_3DMODE_E {
    HI_MW_DISP_3D_NONE = 0,
    HI_MW_DISP_3D_FRAME_PACKING = 1,
    HI_MW_DISP_3D_SIDE_BY_SIDE_HALF = 2,
    HI_MW_DISP_3D_TOP_AND_BOTTOM = 3,
    HI_MW_DISP_3D_FIELD_ALTERNATIVE = 4,
    HI_MW_DISP_3D_LINE_ALTERNATIVE = 5,
    HI_MW_DISP_3D_SIDE_BY_SIDE_FULL = 6,
    HI_MW_DISP_3D_L_DEPTH = 7,
    HI_MW_DISP_3D_L_DEPTH_GRAPHICS_GRAPHICS_DEPTH = 8,
    HI_MW_DISP_3D_LINE_ALTERNATIVE_RL = 9,

    HI_MW_DISP_3D_BUTT = 10,
};

/* INK绑定位置枚举 */
typedef enum hiMW_DISP_INK_POS_E {
    HI_MW_DISP_INK_POS_CSC_F = 0,    /* CSC前   */
    HI_MW_DISP_INK_POS_CSC_B = 1,    /* CSC后   */
    HI_MW_DISP_INK_POS_GAMMA_F = 2,  /* GAMMA前 */
    HI_MW_DISP_INK_POS_GAMMA_B = 3,  /* GAMMA后 */
    HI_MW_DISP_INK_POS_DITHER_B = 4, /* DITHER后 */

    HI_MW_DISP_INK_BUTT = 5,
} HI_MW_DISP_INK_POS_E;

/* INK功能属性 */
typedef struct hiMW_DISP_INK_ATTR_S {
    HI_MW_DISP_INK_POS_E enInkPos; /* 水印的绑定位置 */
    HI_U32 u32X;                   /* 水印的绑定x坐标轴的位置 */
    HI_U32 u32Y;                   /* 水印的绑定y坐标轴的位置 */
} HI_MW_DISP_INK_ATTR_S;

/* 统计显示颜色区域 */
typedef struct hiMW_DISP_REGION_ATTR_S {
    /* *< Enable or disable the region of u32RegionIndex  */
    /* CNcomment/:使能或去使能对应u32RegionIndex的区域统计功能   */
    HI_BOOL bEnable;
    /* The region coordinate,should be odd,each region the min rect is 32*32, */
    /* each region max region rect is 3840*2160 */
    /* CNcomment/:区域的坐标属性，必须为偶数，每个区域最小为32X32，每个区域最大为3840X2160 */
    HI_MW_RECT_S
    stRegionRect;
} HI_MW_DISP_REGION_ATTR_S;

/* 定义区域配置信息的结构体 */
typedef struct hiMW_DISP_AREA_ATTR_S {
    HI_BOOL bEnable; /* *< Enable or disable the area  */ /* CNcomment/:使能或去使能区域统计功能   */
    HI_MW_RECT_S stRegionRect;  /* *< The region coordinate,should be odd, max region rect is 3840*2160  */
                                /* CNcomment/:区域的坐标属性，必须为偶数，最大为3840X2160 */
    HI_U32 u32Hnum; /* *< horizontal partition number of area ,each part of area min is 32*32, */
                    /* coordinate,should be odd */
                    /* CNcomment/:区域水平分区数 最大32个，每个分区最小32*32，坐标属性为偶数 */
    HI_U32 u32Vnum; /* vertical partition mumber of area ,each part of area min is 32*32, coordinate,should be odd */
                    /* CNcomment/:区域垂直分区数 最大20个，每个分区最小32*32 ，坐标属性为偶数 */
} HI_MW_DISP_AREA_ATTR_S;

/* 视屏层与图形层在实体屏幕上的偏移量 */
typedef struct hiMW_DISP_OFFSET_S {
    HI_S32 s32XOffset;
    HI_S32 s32YOffset;
} HI_MW_DISP_OFFSET_S;

/* *Panel interface type  */ /* CNcomment/:Panel 的接口类型, 除了LVDS和VBONE，其他的都是TCON屏 */
enum HI_MW_PANEL_INTFTYPE_E {
    HI_MW_PANEL_INTFTYPE_LVDS = 0,
    /* *<interface type is LVDS  */ /* CNcomment/:接口类型为LVDS */
    HI_MW_PANEL_INTFTYPE_VBONE = 1,
    /* *<interface type is VBONE  */ /* CNcomment/:接口类型为VBONE */
    HI_MW_PANEL_INTFTYPE_MINLVDS = 2,
    /* *<interface type is MINLVDS  */ /* CNcomment/:接口类型为MINLVDS */
    HI_MW_PANEL_INTFTYPE_EPI = 3,
    /* *<interface type is EPI  */ /* CNcomment/:接口类型为EPI */
    HI_MW_PANEL_INTFTYPE_ISP = 4,
    /* *<interface type is ISP  */ /* CNcomment/:接口类型为ISP */
    HI_MW_PANEL_INTFTYPE_CEDS = 5,
    /* *<interface type is CEDS  */ /* CNcomment/:接口类型为CEDS */
    HI_MW_PANEL_INTFTYPE_CHPI = 6,
    /* *<interface type is CHPI  */ /* CNcomment/:接口类型为CHPI */
    HI_MW_PANEL_INTFTYPE_CMPI = 7,
    /* *<interface type is CMPI  */ /* CNcomment/:接口类型为CMPI */
    HI_MW_PANEL_INTFTYPE_CSPI = 8,
    /* *<interface type is CSPI  */ /* CNcomment/:接口类型为CSPI */
    HI_MW_PANEL_INTFTYPE_USIT = 9,
    /* *<interface type is USIT  */ /* CNcomment/:接口类型为USIT */

    HI_MW_PANEL_INTFTYPE_BUTT = 10 /* *<Invalid value */ /* CNcomment/:非法边界值 */
};

/* LVDS展频频率 */
enum HI_MW_PANEL_LVDS_SSFREQ_E {
    HI_MW_PANEL_LVDS_SSFREQ_93P75KHZ = 2,
    HI_MW_PANEL_LVDS_SSFREQ_62P5KHZ = 3,
    HI_MW_PANEL_LVDS_SSFREQ_46P875KHZ = 4,
    HI_MW_PANEL_LVDS_SSFREQ_37P5KHZ = 5,

    HI_MW_PANEL_LVDS_SSFREQ_BUTT = 6,
};

/* LVDS驱动电流 */
enum HI_MW_PANEL_LVDS_CURRENT_E {
    HI_MW_PANEL_LVDS_CURRENT_200MV = 0, /* 驱动电流200MV */
    HI_MW_PANEL_LVDS_CURRENT_250MV = 1, /* 驱动电流250MV */
    HI_MW_PANEL_LVDS_CURRENT_300MV = 2, /* 驱动电流300MV */
    HI_MW_PANEL_LVDS_CURRENT_350MV = 3, /* 驱动电流350MV */
    HI_MW_PANEL_LVDS_CURRENT_400MV = 4, /* 驱动电流400MV */
    HI_MW_PANEL_LVDS_CURRENT_450MV = 5, /* 驱动电流450MV */
    HI_MW_PANEL_LVDS_CURRENT_500MV = 6, /* 驱动电流500MV */

    HI_MW_PANEL_LVDS_CURRENT_BUTT = 7,
};

/* LVDS共模电压 */
enum HI_MW_PANEL_LVDS_VOLTAGE_E {
    HI_MW_PANEL_LVDS_VOLTAGE_400MV = 0,
    /* *< voltage value 400MV */ /* CNcomment/: 共模电压400MV */
    HI_MW_PANEL_LVDS_VOLTAGE_450MV = 1,
    /* *< voltage value 450MV */ /* CNcomment/: 共模电压450MV */
    HI_MW_PANEL_LVDS_VOLTAGE_500MV = 2,
    /* *< voltage value 500MV */ /* CNcomment/: 共模电压500MV */
    HI_MW_PANEL_LVDS_VOLTAGE_550MV = 3,
    /* *< voltage value 550MV */ /* CNcomment/: 共模电压550MV */
    HI_MW_PANEL_LVDS_VOLTAGE_600MV = 4,
    /* *< voltage value 600MV */ /* CNcomment/: 共模电压600MV */
    HI_MW_PANEL_LVDS_VOLTAGE_650MV = 5,
    /* *< voltage value 650MV */ /* CNcomment/: 共模电压650MV */
    HI_MW_PANEL_LVDS_VOLTAGE_700MV = 6,
    /* *< voltage value 700MV */ /* CNcomment/: 共模电压700MV */
    HI_MW_PANEL_LVDS_VOLTAGE_800MV = 7,
    /* *< voltage value 800MV */ /* CNcomment/: 共模电压800MV */
    HI_MW_PANEL_LVDS_VOLTAGE_900MV = 8,
    /* *< voltage value 900MV */ /* CNcomment/: 共模电压900MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1000MV = 9,
    /* *< voltage value 1000MV */ /* CNcomment/: 共模电压1000MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1100MV = 10,
    /* *< voltage value 1100MV */ /* CNcomment/: 共模电压1100MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1150MV = 11,
    /* *< voltage value 1150MV */ /* CNcomment/: 共模电压1150MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1200MV = 12,
    /* *< voltage value 1200MV */ /* CNcomment/: 共模电压1200MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1250MV = 13,
    /* *< voltage value 1250MV */ /* CNcomment/: 共模电压1250MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1300MV = 14,
    /* *< voltage value 1300MV */ /* CNcomment/: 共模电压1300MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1400MV = 15,
    /* *< voltage value 1400MV */ /* CNcomment/: 共模电压1400MV */
    HI_MW_PANEL_LVDS_VOLTAGE_1500MV = 16,
    /* *< voltage value 1500MV */ /* CNcomment/: 共模电压1500MV */

    HI_MW_PANEL_LVDS_VOLTAGE_BUTT = 17,
};

/* *Panel link type  */
enum HI_MW_PANEL_LINKTYPE_E {
    HI_MW_PANEL_LINKTYPE_1LINK = 0,
    HI_MW_PANEL_LINKTYPE_2LINK = 1,
    HI_MW_PANEL_LINKTYPE_4LINK = 2,
    HI_MW_PANEL_LINKTYPE_8LINK = 3,
    HI_MW_PANEL_LINKTYPE_16LINK = 4,

    HI_MW_LVDS_LINKTYPE_BUTT,
};

/* *LVDS link map  */
enum HI_MW_PANEL_LVDS_LINKMAP_E {
    HI_MW_PANEL_LVDS_LINKMAP_PIXEL0 = 0,
    HI_MW_PANEL_LVDS_LINKMAP_PIXEL1 = 1,
    HI_MW_PANEL_LVDS_LINKMAP_PIXEL2 = 2,
    HI_MW_PANEL_LVDS_LINKMAP_PIXEL3 = 3,

    HI_MW_PANEL_LVDS_LINKMAP_BUTT,
};

/* *LVDS bit width  */
enum HI_MW_PANEL_BITWIDTH_E {
    HI_MW_PANEL_BITWIDTH_8BIT = 0,  /* *<bit width is 8  */
    HI_MW_PANEL_BITWIDTH_10BIT = 1, /* *<bit width is 10  */
    HI_MW_PANEL_BITWIDTH_12BIT = 2, /* *<bit width is 12  */
    HI_MW_PANEL_BITWIDTH_6BIT = 3,  /* *<bit width is 6 */
    HI_MW_PANEL_BITWIDTH_16BIT = 4,  /* *<bit width is 16 */
    HI_MW_PANEL_BITWIDTH_24BIT = 5,  /* *<bit width is 24 */

    HI_MW_PANEL_BITWIDTH_BUTT, /* *<Invalid value */
};

/* *LVDS format  */ /* *CNcomment:LVDS<B5>母<F1>式  */
enum HI_MW_PANEL_LVDS_FMT_E {
    HI_MW_PANEL_LVDS_FMT_VESA = 0,  /* *<LVDS format:VESA  */
    HI_MW_PANEL_LVDS_FMT_JEIDA = 1, /* *<LVDS format:JEIDA  */
    HI_MW_PANEL_LVDS_FMT_FP = 2,    /* *<LVDS format:FP  */

    HI_MW_PANEL_LVDS_FMT_BUTT, /* *<Invalid value */
};

enum HI_MW_PANEL_LVDS_SWAP_E {
    HI_MW_PANEL_LVDS_SWAP_ODD = 0,
    HI_MW_PANEL_LVDS_SWAP_EVEN = 1,

    HI_MW_PANEL_LVDS_SWAP_BUTT,
};

enum HI_MW_PANEL_VBO_SSFREQ_E {
    //    HI_MW_PANEL_VBO_SSFREQ_93P75KHZ  = 1,
    HI_MW_PANEL_VBO_SSFREQ_46P875KHZ = 2,
    HI_MW_PANEL_VBO_SSFREQ_31P250KHZ = 3,
    HI_MW_PANEL_VBO_SSFREQ_23P438KHZ = 4,
    HI_MW_PANEL_VBO_SSFREQ_18P750KHZ = 5,
    HI_MW_PANEL_VBO_SSFREQ_93P875KHZ = 6,
    HI_MW_PANEL_VBO_SSFREQ_62P5KHZ = 7,
    HI_MW_PANEL_VBO_SSFREQ_37P5KHZ = 8,
    HI_MW_PANEL_VBO_SSFREQ_26P786KHZ = 9,
    HI_MW_PANEL_VBO_SSFREQ_20P833KHZ = 10,

    HI_MW_PANEL_VBO_SSFREQ_BUTT = 11,
};

enum HI_MW_PANEL_VBO_CURRENT_E {
    HI_MW_PANEL_VBO_CURRENT_200MV = 0,
    HI_MW_PANEL_VBO_CURRENT_250MV = 1,
    HI_MW_PANEL_VBO_CURRENT_300MV = 2,
    HI_MW_PANEL_VBO_CURRENT_350MV = 3,
    HI_MW_PANEL_VBO_CURRENT_400MV = 4,
    HI_MW_PANEL_VBO_CURRENT_450MV = 5,
    HI_MW_PANEL_VBO_CURRENT_500MV = 6,

    HI_MW_PANEL_VBO_CURRENT_BUTT = 7,
};

enum HI_MW_PANEL_VBO_EMP_E {
    HI_MW_PANEL_VBO_EMP_0DB = 0, /* CNcomment/:预加重为0DB */
    HI_MW_PANEL_VBO_EMP_1DB = 1, /* CNcomment/:预加重为1DB */
    HI_MW_PANEL_VBO_EMP_2DB = 2, /* CNcomment/:预加重为2DB */
    HI_MW_PANEL_VBO_EMP_3DB = 3, /* CNcomment/:预加重为3DB */
    HI_MW_PANEL_VBO_EMP_4DB = 4, /* CNcomment/:预加重为4DB */
    HI_MW_PANEL_VBO_EMP_5DB = 5, /* CNcomment/:预加重为5DB */
    HI_MW_PANEL_VBO_EMP_6DB = 6, /* CNcomment/:预加重为6DB */
    HI_MW_PANEL_VBO_EMP_7DB = 7, /* CNcomment/:预加重为7DB */
    HI_MW_PANEL_VBO_EMP_8DB = 8, /* CNcomment/:预加重为8DB */
    HI_MW_PANEL_VBO_EMP_9DB = 9, /* CNcomment/:预加重为9DB */

    HI_MW_PANEL_VBO_EMP_BUTT = 10,
};

enum HI_MW_PANEL_DIM_TYPE_E {
    HI_MW_PANEL_DIM_0D = 0,    /* 0D Dim Type */
    HI_MW_PANEL_DIM_LOCAL = 1, /* LOCAL Dim Type */
    HI_MW_PANEL_DIM_BUTT = 2,
};

enum HI_MW_PANEL_LDM_DEMO_MODE_E {
    HI_MW_PANEL_LDM_DEMO_RIGHT_SCREEN = 0,        /* 右边是处理后的图像，左边是源图像 */
    HI_MW_PANEL_LDM_DEMO_LEFT_SCREEN = 1,         /* 左边是处理后的图像，右边是源图像 */
    HI_MW_PANEL_LDM_DEMO_TOP_SCREEN = 2,          /* 上边是处理后的图像，下边是源图像 */
    HI_MW_PANEL_LDM_DEMO_BOTTOM_SCREEN = 3,       /* 下边是处理后的图像，上边是源图像 */
    HI_MW_PANEL_LDM_DEMO_WHITE_SCREEN = 4,        /* 全屏白屏 */
    HI_MW_PANEL_LDM_DEMO_RIGHT_WHITE_SCREEN = 5,  /* 右边是白屏的图像，左边是源图像 */
    HI_MW_PANEL_LDM_DEMO_LEFT_WHITE_SCREEN = 6,   /* 左边是白屏的图像，右边是源图像 */
    HI_MW_PANEL_LDM_DEMO_TOP_WHITE_SCREEN = 7,    /* 上边是白屏的图像，下边是源图像 */
    HI_MW_PANEL_LDM_DEMO_BOTTOM_WHITE_SCREEN = 8, /* 下边是白屏的图像，上边是源图像 */
    HI_MW_PANEL_LDM_DEMO_HORSELIGHT = 9,          /* 跑马灯 */
    HI_MW_PANEL_LDM_DEMO_OFF = 10,                /* 关闭Demo模式 */

    HI_MW_PANEL_LDM_DEMO_BUTT = 11,
};

/* panel fixate type */
enum HI_MW_FIX_FRMRAME_E {
    HI_MW_PANEL_FIXRATE_TYPE_NONE = 0,      /* 不固定输出帧率 */
    HI_MW_PANEL_FIXRATE_TYPE_FHD_50HZ = 1,  /* fixrate type is fhd50 */
    HI_MW_PANEL_FIXRATE_TYPE_FHD_60HZ = 2,  /* fixrate type is fhd60 */
    HI_MW_PANEL_FIXRATE_TYPE_FHD_OTHER = 3, /* fixrate type is fhd60 */
    HI_MW_PANEL_FIXRATE_TYPE_UHD_50HZ = 4,  /* fixrate type is uhd50 */
    HI_MW_PANEL_FIXRATE_TYPE_UHD_60HZ = 5,  /* fixrate type is uhd60 */
    HI_MW_PANEL_FIXRATE_TYPE_UHD_OTHER = 6, /* fixrate type is fhd60 */

    HI_MW_PANEL_FIXRATE_TYPE_BUTT = 7, /* Invalid value */
};

/* DDR展频频率 */
enum HI_MW_DDR_SSFREQ_E {
    HI_MW_DDR_SSFREQ_93KHZ = 0,
    HI_MW_DDR_SSFREQ_62KHZ = 1,
    HI_MW_DDR_SSFREQ_46KHZ = 2,
    HI_MW_DDR_SSFREQ_37KHZ = 3,
    HI_MW_DDR_SSFREQ_31KHZ = 4,
    HI_MW_DDR_SSFREQ_26KHZ = 5,
    HI_MW_DDR_SSFREQ_23KHZ = 6,
    HI_MW_DDR_SSFREQ_20KHZ = 7,
    HI_MW_DDR_SSFREQ_18KHZ = 8,
    HI_MW_DDR_SSFREQ_17KHZ = 9,
    HI_MW_DDR_SSFREQ_15KHZ = 10,
    HI_MW_DDR_SSFREQ_14KHZ = 11,
    HI_MW_DDR_SSFREQ_13KHZ = 12,
    HI_MW_DDR_SSFREQ_12KHZ = 13,

    HI_MW_DDR_SSFREQ_BUTT = 14,
};

/* define the enum of signal resolution */
enum HI_MW_HD_RESOLUTION_E {
    HI_MW_HD_480i_60 = 0,
    HI_MW_HD_480p_60 = 1,
    HI_MW_HD_576i_50 = 2,
    HI_MW_HD_576p_50 = 3,
    HI_MW_HD_720p_50 = 4,
    HI_MW_HD_720p_60 = 5,
    HI_MW_HD_1080i_25 = 6,
    HI_MW_HD_1080i_50 = 7,
    HI_MW_HD_1080i_60 = 8,
    HI_MW_HD_1080p_50 = 9,
    HI_MW_HD_1080p_60 = 10,
    HI_MW_HD_1080p_24 = 11,
    HI_MW_HD_1080p_25 = 12,
    HI_MW_HD_1080p_30 = 13,
    HI_MW_HD_720p_24 = 14,
    HI_MW_HD_720p_25 = 15,
    HI_MW_HD_720p_30 = 16,
    HI_MW_HD_1440x480i_60 = 17,
    HI_MW_HD_1440x480p_60 = 18,
    HI_MW_HD_1440x576i_50 = 19,
    HI_MW_HD_1440x576p_50 = 20,
    HI_MW_HD_288p_50 = 21,

    HI_MW_HD_BUTT = 22,
};

enum HI_MW_VIDEO_DEFINITION_E {
    HI_MW_VIDEO_DEFINITION_STANDARD = 0,
    HI_MW_VIDEO_DEFINITION_HIGH = 1,
    HI_MW_VIDEO_DEFINITION_4K = 2,

    HI_MW_VIDEO_DEFINITION_BUTT = 3,
};

/* HDMI 信号格式 */
enum HI_MW_HDMI_FORMAT_E {
    HI_MW_HDMI_FORMAT_HDMI = 0,
    HI_MW_HDMI_FORMAT_DVI = 1,
    HI_MW_HDMI_FORMAT_MHL = 2,

    HI_MW_HDMI_FORMAT_BUTT = 3,
};

/* define the enum of signal sync status */
enum HI_MW_SIGNAL_E {
    HI_MW_SIGNAL_SUPPORT = 0,
    HI_MW_SIGNAL_NOSIGNAL = 1,
    HI_MW_SIGNAL_UNSUPPORT = 2,
    HI_MW_SIGNAL_UNSTABLE = 3,

    HI_MW_SIGNAL_BUTT = 4,
};

/*  SCART信号格式 */
enum HI_MW_SCART_SIGNAL_FORMAT_E {
    HI_MW_SCART_NO_SIGNAL = 0,
    HI_MW_SCART_4_3_SIGNAL = 1,
    HI_MW_SCART_16_9_SIGNAL = 2,

    HI_MW_SCART_SIGNAL_BUTT = 3,
};

/* define the MHL in and out status */
enum HI_MW_MHL_E {
    HI_MW_MHL_IN = 8,
    HI_MW_MHL_OUT = 9,

    HI_MW_MHL_BUTT = 10,
};

/* define the enum of Aspect Config Type */
enum HI_MW_ASPECT_CONFIG_TYPE_E {
    HI_MW_ASPECT_CONFIG_TYPE_USER = 0,
    HI_MW_ASPECT_CONFIG_TYPE_WSS = 1,
    HI_MW_ASPECT_CONFIG_TYPE_AFD = 2,
    HI_MW_ASPECT_CONFIG_TYPE_HDMI = 3,
    HI_MW_ASPECT_CONFIG_TYPE_ASPECTCHANGE = 4,

    HI_MW_ASPECT_CONFIG_TYPE_BUTT = 5,
};

/* define the enum of aspect ratio type */
enum HI_MW_ASPECT_E {
    HI_MW_ASPECT_16_9 = 0,
    HI_MW_ASPECT_14_9 = 1,
    HI_MW_ASPECT_4_3 = 2,
    HI_MW_ASPECT_FULL = 3,
    HI_MW_ASPECT_POINT_TO_POINT = 4,
    HI_MW_ASPECT_AUTO = 5,
    HI_MW_ASPECT_SUBTITLE = 6,
    HI_MW_ASPECT_PERSON = 7,
    HI_MW_ASPECT_CINEMA = 8,
    HI_MW_ASPECT_PANORAMA = 9,
    HI_MW_ASPECT_ZOOM = 10,
    HI_MW_ASPECT_ZOOM1 = 11,
    HI_MW_ASPECT_ZOOM2 = 12,
    HI_MW_ASPECT_TRUTH = 13,
    HI_MW_ASPECT_21_9 = 14,

    HI_MW_ASPECT_BUTT = 15,
};

/* 图像模式 */
enum HI_MW_PICMODE_E {
    HI_MW_PICMODE_STANDARD = 0,
    HI_MW_PICMODE_DYNAMIC = 1,
    HI_MW_PICMODE_SOFTNESS = 2,
    HI_MW_PICMODE_USER = 3,
    HI_MW_PICMODE_VIVID = 4,
    HI_MW_PICMODE_NATURAL = 5,
    HI_MW_PICMODE_SPORTS = 6,

    HI_MW_PICMODE_BUTT = 7,
};

/* 色温 */
enum HI_MW_COLORTEMP_E {
    HI_MW_COLORTEMP_NATURE = 0,
    HI_MW_COLORTEMP_COOL = 1,
    HI_MW_COLORTEMP_WARM = 2,
    HI_MW_COLORTEMP_USER = 3,
    HI_MW_COLORTEMP_TOCOOL = 4,
    HI_MW_COLORTEMP_TOWARM = 5,

    HI_MW_COLORTEMP_BUTT = 6,
};

/* gamma type */
enum HI_MW_PQ_GAMMA_TABLE_E {
    HI_MW_PQ_GAMMA_NATURE = 0,
    HI_MW_PQ_GAMMA_COOL = 1,
    HI_MW_PQ_GAMMA_WARM = 2,
    HI_MW_PQ_GAMMA_TOCOOL = 3,
    HI_MW_PQ_GAMMA_TOWARM = 4,
    HI_MW_PQ_GAMMA_HDR10 = 5,
    HI_MW_PQ_GAMMA_DOLBY = 6,
    HI_MW_PQ_GAMMA_VIVID = 7,

    HI_MW_PQ_GAMMA_BUTT = 8,
};

/* dci type */
enum HI_MW_PQ_DCI_TYPE_E {
    HI_MW_PQ_DCI_CVBS = 0,
    HI_MW_PQ_DCI_YPBPR = 1,
    HI_MW_PQ_DCI_HDMI_NON4K = 2,
    HI_MW_PQ_DCI_HDMI_4K = 3,
    HI_MW_PQ_DCI_HDMI_3D = 4,
    HI_MW_PQ_DCI_ATV = 5,
    HI_MW_PQ_DCI_DTV_NON4K = 6,
    HI_MW_PQ_DCI_DTV_4K = 7,
    HI_MW_PQ_DCI_DTV_3D = 8,
    HI_MW_PQ_DCI_HDR10_HDMI = 9,
    HI_MW_PQ_DCI_HDR10_DTV = 10,
    HI_MW_PQ_DCI_DOLBY_HDMI = 11,
    HI_MW_PQ_DCI_DOLBY_DTV = 12,
    HI_MW_PQ_DCI_VIVID = 13,

    HI_MW_PQ_DCI_BUTT = 14,
};

/* COLOR 表类型 */
enum HI_MW_PQ_COLOR_TYPE_E {
    HI_MW_PQ_COLOR_CVBS = 0,
    HI_MW_PQ_COLOR_YPBPR = 1,
    HI_MW_PQ_COLOR_HDMI_NON4K = 2,
    HI_MW_PQ_COLOR_HDMI_4K = 3,
    HI_MW_PQ_COLOR_HDMI_3D = 4,
    HI_MW_PQ_COLOR_ATV = 5,
    HI_MW_PQ_COLOR_DTV_NON4K = 6,
    HI_MW_PQ_COLOR_DTV_4K = 7,
    HI_MW_PQ_COLOR_DTV_3D = 8,
    HI_MW_PQ_COLOR_HDR10_HDMI = 9,
    HI_MW_PQ_COLOR_HDR10_DTV = 10,
    HI_MW_PQ_COLOR_DOLBY_HDMI = 11,
    HI_MW_PQ_COLOR_DOLBY_DTV = 12,
    HI_MW_PQ_COLOR_VIVID = 13,
    HI_MW_PQ_COLOR_SDR2HDR = 14,

    HI_MW_PQ_COLOR_TYPE_BUTT = 15,
};

/* 关、低、中、高 、自动五等级 */
enum HI_MW_LEVEL_E {
    HI_MW_LEVEL_OFF = 0,
    HI_MW_LEVEL_LOW = 1,
    HI_MW_LEVEL_MID = 2,
    HI_MW_LEVEL_HIGH = 3,
    HI_MW_LEVEL_AUTO = 4,

    HI_MW_LEVEL_BUTT = 5,
};

/* 关、开、演示三等级 */
enum HI_MW_DEMOLEVEL_E {
    HI_MW_DEMOLEVEL_OFF = 0,
    HI_MW_DEMOLEVEL_ON = 1,
    HI_MW_DEMOLEVEL_DEMO = 2,

    HI_MW_DEMOLEVEL_BUTT = 3,
};

/* 卖场模式 */
enum HI_MW_DEMO_MODE_E {
    HI_MW_DEMO_DBDR = 0,
    HI_MW_DEMO_NR = 1,
    HI_MW_DEMO_SHARPNESS = 2,
    HI_MW_DEMO_DCI = 3,
    HI_MW_DEMO_WCG = 4,
    HI_MW_DEMO_MEMC = 5,
    HI_MW_DEMO_COLOR = 6,
    HI_MW_DEMO_SR = 7,
    HI_MW_DEMO_HDR = 8,
    HI_MW_DEMO_ALL = 9,
    HI_MW_DEMO_DOLBY = 10,

    HI_MW_DEMO_BUTT = 11,
};

/* PQ模块 */
enum HI_MW_PQ_MODULE_E {
    HI_MW_PQ_MODULE_FMD = 0,
    HI_MW_PQ_MODULE_NR = 1,
    HI_MW_PQ_MODULE_DB = 2,
    HI_MW_PQ_MODULE_DR = 3,
    HI_MW_PQ_MODULE_DS = 4,
    HI_MW_PQ_MODULE_HSHARPNESS = 5,
    HI_MW_PQ_MODULE_SHARPNESS = 6,
    HI_MW_PQ_MODULE_CCCL = 7,
    HI_MW_PQ_MODULE_COLOR_CORING = 8,
    HI_MW_PQ_MODULE_BLUE_STRETCH = 9,
    HI_MW_PQ_MODULE_GAMMA = 10,
    HI_MW_PQ_MODULE_DBC = 11,
    HI_MW_PQ_MODULE_DCI = 12,
    HI_MW_PQ_MODULE_COLOR = 13,
    HI_MW_PQ_MODULE_SR = 14,
    HI_MW_PQ_MODULE_WCG = 15,
    HI_MW_PQ_MODULE_HDR = 16,
    HI_MW_PQ_MODULE_HDR10PLUS = 17,
    HI_MW_PQ_MODULE_IQTM = 18,
    HI_MW_PQ_MODULE_SDR2HDR = 19,
    HI_MW_PQ_MODULE_ALL = 20, /* true:Restore last module  status;  false:Close all pq module */

    HI_MW_PQ_MODULE_BUTT = 21,
};

enum HI_MW_PQ_TCON_MODULE_E {
    HI_MW_PQ_TCON_MODULE_OD = 0,
    /* *<od MODULE */ /* CNcomment/:od 模块 */
    HI_MW_PQ_TCON_MODULE_RGBW = 1,
    /* *<rgbw MODULE */ /* CNcomment/:rgbw 模块 */
    HI_MW_PQ_TCON_MODULE_PCID = 2,
    /* *<pcid MODULE */ /* CNcomment/:pcid 模块 */
    HI_MW_PQ_TCON_MODULE_DEMURA = 3,
    /* *<demura MODULE */ /* CNcomment/:demura 模块 */

    HI_MW_PQ_TCON_MODULE_BUTT = 4,
    /* *<Invalid value */ /* CNcomment/:非法边界值 */
};

enum HI_MW_PQ_DOLBY_MODE_E {
    HI_MW_PQ_DOLBY_MODE_ENHANCE = 0,
    HI_MW_PQ_DOLBY_MODE_STANDARD = 1,

    HI_MW_PQ_DOLBY_MODE_BUTT = 2,
};

/* 声音模式 */
enum HI_MW_SNDMODE_E {
    HI_MW_SNDMODE_STANDARD = 0,
    HI_MW_SNDMODE_NEWS = 1,
    HI_MW_SNDMODE_MUSIC = 2,
    HI_MW_SNDMODE_MOVIE = 3,
    HI_MW_SNDMODE_SPORTS = 4,
    HI_MW_SNDMODE_USER = 5,
    HI_MW_SNDMODE_4K = 6,

    HI_MW_SNDMODE_BUTT = 7,
};

/* 均衡器分段 */
enum HI_MW_AUDIO_EQ_E {
    HI_MW_AUDIO_EQ_BAND0 = 0,
    HI_MW_AUDIO_EQ_BAND1 = 1,
    HI_MW_AUDIO_EQ_BAND2 = 2,
    HI_MW_AUDIO_EQ_BAND3 = 3,
    HI_MW_AUDIO_EQ_BAND4 = 4,
    HI_MW_AUDIO_EQ_BAND5 = 5,
    HI_MW_AUDIO_EQ_BAND6 = 6,
    HI_MW_AUDIO_EQ_BAND7 = 7,
    HI_MW_AUDIO_EQ_BAND8 = 8,
    HI_MW_AUDIO_EQ_BAND9 = 9,

    HI_MW_AUDIO_EQ_BUTT = 10,
};

/* PEQ均衡器分段 */
enum HI_MW_AUDIO_PEQ_TYPE_E {
    HI_MW_AUDIO_PEQ_TYPE_QVALUE = 0,
    HI_MW_AUDIO_PEQ_TYPE_GAIN = 1,

    HI_MW_AUDIO_PEQ_TYPE_BUTT = 2,
};

enum HI_MW_AUDIO_PEQ_QVALUE_E {
    HI_MW_AUDIO_PEQ_VALUE0 = 0,
    HI_MW_AUDIO_PEQ_VALUE1 = 1,
    HI_MW_AUDIO_PEQ_VALUE2 = 2,
    HI_MW_AUDIO_PEQ_VALUE3 = 3,
    HI_MW_AUDIO_PEQ_VALUE4 = 4,
    HI_MW_AUDIO_PEQ_VALUE5 = 5,
    HI_MW_AUDIO_PEQ_VALUE6 = 6,
    HI_MW_AUDIO_PEQ_VALUE7 = 7,
    HI_MW_AUDIO_PEQ_VALUE8 = 8,
    HI_MW_AUDIO_PEQ_VALUE9 = 9,

    HI_MW_AUDIO_PEQ_VALUE_BUTT = 10,
};

enum HI_MW_AUDIO_PEQ_GAIN_E {
    HI_MW_AUDIO_PEQ_GAIN0 = 0,
    HI_MW_AUDIO_PEQ_GAIN1 = 1,
    HI_MW_AUDIO_PEQ_GAIN2 = 2,
    HI_MW_AUDIO_PEQ_GAIN3 = 3,
    HI_MW_AUDIO_PEQ_GAIN4 = 4,
    HI_MW_AUDIO_PEQ_GAIN5 = 5,
    HI_MW_AUDIO_PEQ_GAIN6 = 6,
    HI_MW_AUDIO_PEQ_GAIN7 = 7,
    HI_MW_AUDIO_PEQ_GAIN8 = 8,
    HI_MW_AUDIO_PEQ_GAIN9 = 9,

    HI_MW_AUDIO_PEQ_GAIN_BUTT = 10,
};

/* 声场定位枚举类型 */
enum HI_MW_SNDLOCATE_E {
    HI_MW_SNDLOCATE_DESKTOP = 0,
    HI_MW_SNDLOCATE_WALL = 1,

    HI_MW_SNDLOCATE_BUTT = 2,
};

/* 声音输出通道 */
enum HI_MW_AUDIO_CHANNEL_E {
    HI_MW_AUDIO_CHANNEL_SPEAKER = 0,
    HI_MW_AUDIO_CHANNEL_HEADPHONE = 1,
    HI_MW_AUDIO_CHANNEL_BLUETOOTH = 2,
    HI_MW_AUDIO_CHANNEL_ARC = 3,
    HI_MW_AUDIO_CHANNEL_SPDIF = 4,
    HI_MW_AUDIO_CHANNEL_AVOUT = 5,
    HI_MW_AUDIO_CHANNEL_24G = 6,
    // HI_MW_AUDIO_CHANNEL_HORN      = 7,
    HI_MW_AUDIO_CHANNEL_I2S0 = 7,
    HI_MW_AUDIO_CHANNEL_HDMITX = 8,

    HI_MW_AUDIO_CHANNEL_BUTT = 9,
};

enum HI_MW_AUDIO_STREAM_TYPE_E {
    HI_MW_AUDIO_STREAM_TYPE_DTS = 0,
    HI_MW_AUDIO_STREAM_TYPE_DOLBY = 1,

    HI_MW_AUDIO_STREAM_TYPE_BUTT = 2,
};

enum HI_MW_AUDIO_DOLBY_TYPE_E {
    HI_MW_AUDIO_DOLBY_TYPE_ATMOS = 0,
    HI_MW_AUDIO_DOLBY_TYPE_AUDIO = 1,

    HI_MW_AUDIO_DOLBY_TYPE_BUTT = 2,
};

enum HI_MW_DTS_STREAM_TYPE_E {
    HI_MW_DTS_STREAM_TYPE_DTS = 0,
    HI_MW_DTS_STREAM_TYPE_DTSEX = 1,
    HI_MW_DTS_STREAM_TYPE_DTSHD = 2,
    HI_MW_DTS_STREAM_TYPE_UNKNOW = 3,

    HI_MW_DTS_STREAM_TYPE_BUTT = 4,
};

/* 音频输出端口 */
enum HI_MW_SND_OUTPORT_E {
    HI_MW_SND_OUTPORT_DAC0 = 0,
    HI_MW_SND_OUTPORT_I2S0 = 1,
    HI_MW_SND_OUTPORT_I2S1 = 2,
    HI_MW_SND_OUTPORT_I2S3 = 0x13,
    HI_MW_SND_OUTPORT_SPDIF0 = 3,
    HI_MW_SND_OUTPORT_HDMI0 = 4,
    HI_MW_SND_OUTPORT_ARC0 = 5,
    HI_MW_SND_OUTPORT_DAC1 = 0x50,
    HI_MW_SND_OUTPORT_DAC2 = 0x51,
    HI_MW_SND_OUTPORT_DAC3 = 0x52,
    HI_MW_SND_OUTPORT_ALL = 0x7fff,

    HI_MW_SND_OUTPORT_BUTT = 0x8000,
};

/* define the mode of arc */
enum HI_MW_SND_ARC_MODE_E {
    HI_MW_SND_ARC_AUDIO_MODE_LPCM = 0,    /* ARC LCPM2.0 */
    HI_MW_SND_ARC_AUDIO_MODE_RAW = 1,     /* ARC Pass-through */
    /* ARC Pass-through force high-bitrate transcoding to low-bitrate. DD+ transcoded to DD */
    HI_MW_SND_ARC_AUDIO_MODE_HBR2LBR = 2,
    HI_MW_SND_ARC_AUDIO_MODE_AUTO = 3, /* automatically match according to the audio capbility of ARC from HDMIRX */

    HI_MW_SND_ARC_AUDIO_MODE_BUTT = 4,
};

/* define the mode of hdmi tx */
enum HI_MW_SND_HDMI_MODE_E {
    HI_MW_SND_HDMI_AUDIO_MODE_LPCM = 0,
    /* *<HDMI LCPM2.0 */ /* CNcomment/: 立体声pcm */
    HI_MW_SND_HDMI_AUDIO_MODE_RAW = 1,
    /* *<HDMI Pass-through. */ /* CNcomment/: HDMI透传 */
    HI_MW_SND_HDMI_AUDIO_MODE_HBR2LBR = 2,
    /* *<HDMI Pass-through force high-bitrate to low-bitrate. */ /* CNcomment/: 蓝光次世代音频降规格输出 */
    HI_MW_SND_HDMI_AUDIO_MODE_AUTO = 3,
    /* *<Automatically match according to the EDID of HDMI  */ /* CNcomment/: 根据HDMI EDID能力自动匹配 */

    HI_MW_SND_HDMI_AUDIO_MODE_BUTT = 4,
};

/* 定义音频端口的静音类型 */
enum HI_MW_SND_MUTE_TYPE_E {
    HI_MW_SND_MUTE_IRCTRL = 0,    /* IR 静音 */
    HI_MW_SND_MUTE_POLICY = 1,    /* Policy  静音 */
    HI_MW_SND_MUTE_SIGNAL = 2,    /* 无信号静音 */
    HI_MW_SND_MUTE_SELSRC = 3,    /* 换源静音 */
    HI_MW_SND_MUTE_SELPRG = 4,    /* 换台静音 */
    HI_MW_SND_MUTE_SCNPRG = 5,    /* 搜台静音 */
    HI_MW_SND_MUTE_FOCUS = 6,     /* 失焦静音 */
    HI_MW_SND_MUTE_CONFIG = 7,    /* 配参静音 */
    HI_MW_SND_MUTE_HEADSET = 8,   /* 耳机链接 */
    HI_MW_SND_MUTE_SPDIF = 9,     /* SPDIF输出 */
    HI_MW_SND_MUTE_ARC = 10,      /* ARC 静音 */
    HI_MW_SND_MUTE_SRC_LOCK = 11, /* 频道锁静音 */
    HI_MW_SND_MUTE_CHANNEL_LOCK = 12,
    HI_MW_SND_MUTE_PARENT_LOCK = 13,

    HI_MW_SND_MUTE_BUTT = 14,
};

/* 立体声模式 */
enum HI_MW_STEREO_E {
    HI_MW_STEREO_MONO = 0,
    HI_MW_STEREO_STEREO = 1,

    HI_MW_STEREO_BUTT = 2,
};

/* 扬声器输出方式 */
enum HI_MW_SPEAKER_E {
    HI_MW_SPEAKER_INTERNAL = 0,
    HI_MW_SPEAKER_EXTERNAL = 1,

    HI_MW_SPEAKER_BUTT = 2,
};

enum HI_MW_SND_SPDIF_MODE_E {
    HI_MW_SND_SPDIF_MODE_PCM = 0,
    HI_MW_SND_SPDIF_MODE_RAW = 1,
    HI_MW_SND_SPDIF_MODE_AUTO = 2,

    HI_MW_SND_SPDIF_MODE_BUTT = 3,
};

enum HI_MW_SND_E {
    HI_MW_SND_0, /* *<AO device 0 */
    HI_MW_SND_1, /* *<AO device 1 */
    HI_MW_SND_2, /* *<AO device 2 */
    HI_MW_SND_BUTT
};

/* DOLBY MODE */
enum HI_MW_DOLBY_E {
    HI_MW_DOLBY_LINE = 0,
    HI_MW_DOLBY_RF = 1,

    HI_MW_DOLBY_BUTT = 2,
};

/* 显示输出Macrovision模式 */
enum HI_MW_MACROVISION_E {
    HI_MW_MACROVISION_TYPE0 = 0,
    /* *<type 0  */ /* CNcomment/:典型配置0 */
    HI_MW_MACROVISION_TYPE1 = 1,
    /* *<type 1  */ /* CNcomment/:典型配置1 */
    HI_MW_MACROVISION_TYPE2 = 2,
    /* *<type 2  */ /* CNcomment/:典型配置2 */
    HI_MW_MACROVISION_TYPE3 = 3,
    /* *<type 3  */ /* CNcomment/:典型配置3 */
    HI_MW_MACROVISION_CUSTOM = 4,
    /* *<type of configure by user  */ /* CNcomment/:用户自定义配置 */

    HI_MW_MACROVISION_BUTT = 5,
};

/* 音频伴音制式 */
enum HI_MW_AUDSYS_E {
    HI_MW_AUDSYS_BG = 0x00,
    /* *<Audio standard BG */ /* CNcomment/:音频伴音制式BG */
    HI_MW_AUDSYS_BG_A2 = 0x01,
    /* *<Audio standard BG A2 */ /* CNcomment/:音频伴音制式BG A2 */
    HI_MW_AUDSYS_BG_NICAM = 0x02,
    /* *<Audio standard BG NICAM */ /* CNcomment/:音频伴音制式BG NICAM */
    HI_MW_AUDSYS_I = 0x03,
    /* *<Audio standard I */ /* CNcomment/:音频伴音制式I */
    HI_MW_AUDSYS_DK = 0x04,
    /* *<Audio standard DK */ /* CNcomment/:音频伴音制式DK */
    HI_MW_AUDSYS_DK1_A2 = 0x05,
    /* *<Audio standard DK1 A2 */ /* CNcomment/:音频伴音制式DK1 A2 */
    HI_MW_AUDSYS_DK2_A2 = 0x06,
    /* *<Audio standard DK2 A2 */ /* CNcomment/:音频伴音制式DK2 A2 */
    HI_MW_AUDSYS_DK3_A2 = 0x07,
    /* *<Audio standard DK3 A2 */ /* CNcomment/:音频伴音制式DK3 A2 */
    HI_MW_AUDSYS_DK_NICAM = 0x08,
    /* *<Audio standard DK NICAM */ /* CNcomment/:音频伴音制式DK NICAM */
    HI_MW_AUDSYS_L = 0x09,
    /* *<Audio standard L */ /* CNcomment/:音频伴音制式L */
    HI_MW_AUDSYS_M = 0x0a,
    /* *<Audio standard M */ /* CNcomment/:音频伴音制式M */
    HI_MW_AUDSYS_M_BTSC = 0x0b,
    /* *<Audio standard M BTSC */ /* CNcomment/:音频伴音制式M BTSC */
    HI_MW_AUDSYS_M_A2 = 0x0c,
    /* *<Audio standard M A2 */ /* CNcomment/:音频伴音制式M A2 */
    HI_MW_AUDSYS_M_EIA_J = 0x0d,
    /* *<Audio standard M EIA J */ /* CNcomment/:音频伴音制式M EIA J */
    HI_MW_AUDSYS_NOTSTANDARD = 0x0e,

    HI_MW_AUDSYS_UNKNOW = 0x0f,
};

/* ATV制式信息 */
enum HI_MW_ATV_SYSTEM_E {
    HI_MW_ATV_SYSTEM_PAL_BG = 0,
    HI_MW_ATV_SYSTEM_PAL_DK,
    HI_MW_ATV_SYSTEM_PAL_I,
    HI_MW_ATV_SYSTEM_PAL_M,
    HI_MW_ATV_SYSTEM_PAL_N,
    HI_MW_ATV_SYSTEM_SECAM_BG,
    HI_MW_ATV_SYSTEM_SECAM_DK,
    HI_MW_ATV_SYSTEM_SECAM_L_PRIME,  // Secam L, freq > 100MHz
    HI_MW_ATV_SYSTEM_SECAM_LL,       // Secam L',freq < 100MHz
    HI_MW_ATV_SYSTEM_NTSC_M,
    HI_MW_ATV_SYSTEM_NTSC_I,
    HI_MW_ATV_SYSTEM_NTSC_DK,

    HI_MW_ATV_SYSTEM_BUTT
};

/* 过调制 Over Devaiton */
enum HI_MW_SIF_OVER_DEVIATION_E {
    HI_MW_SIF_OVER_DEVIATION_50K = 0,  /* 过调制 50K */
    HI_MW_SIF_OVER_DEVIATION_100K = 1, /* 过调制 100K */
    HI_MW_SIF_OVER_DEVIATION_200K = 2, /* 过调制 200K */
    HI_MW_SIF_OVER_DEVIATION_384K = 3, /* 过调制 384K */
    HI_MW_SIF_OVER_DEVIATION_540K = 4, /* 过调制 500K */

    HI_MW_SIF_OVER_DEVIATION_BUTT = 5,
};

/* 音频伴音模式 */
enum HI_MW_SIF_OUTMODE_E {
    HI_MW_SIF_OUTMODE_MONO = 0x00,              /* *<Audio Mode MONO */
    HI_MW_SIF_OUTMODE_STEREO = 0x01,            /* *<Audio Mode G Stereo */
    HI_MW_SIF_OUTMODE_DUAL_A = 0x02,            /* *<Audio Mode Dual A */
    HI_MW_SIF_OUTMODE_DUAL_B = 0x03,            /* *<Audio Mode Dual B */
    HI_MW_SIF_OUTMODE_DUAL_AB = 0x04,           /* *<Audio Mode Dual AB */
    HI_MW_SIF_OUTMODE_BTSC_MONO = 0x05,         /* *<Audio Mode BTSC MONO */
    HI_MW_SIF_OUTMODE_BTSC_STEREO = 0x06,       /* *<Audio Mode BTSC STEREO */
    HI_MW_SIF_OUTMODE_BTSC_SAP = 0x07,          /* *<Audio Mode BTSC SAP */
    HI_MW_SIF_OUTMODE_NICAM_FORCED_MONO = 0x08, /* *<Audio Mode NICAM Forced MONO */
    HI_MW_SIF_OUTMODE_NICAM_MONO = 0x09,        /* *<Audio Mode NICAM MONO */
    HI_MW_SIF_OUTMODE_NICAM_STEREO = 0x0a,      /* *<Audio Mode NICAM Stereo */
    HI_MW_SIF_OUTMODE_NICAM_DUAL_A = 0x0b,      /* *<Audio Mode NICAM DUAL A */
    HI_MW_SIF_OUTMODE_NICAM_DUAL_B = 0x0c,      /* *<Audio Mode NICAM DUAL B */
    HI_MW_SIF_OUTMODE_NICAM_DUAL_AB = 0x0d,     /* *<Audio Mode NICAM DUAL AB */
    HI_MW_SIF_OUTMODE_AUTO = 0x0e,              /* *<Audio Mode AUTO  */

    HI_MW_SIF_OUTMODE_BUTT = 0x0f,
};

/* 音频载波模式 */
enum HI_MW_SIF_AAOS_MODE_E {
    HI_MW_SIF_AAOS_MODE_MONO = 0,
    HI_MW_SIF_AAOS_MODE_STEREO = 1,
    HI_MW_SIF_AAOS_MODE_DUAL = 2,
    HI_MW_SIF_AAOS_MODE_MONO_SAP = 3,
    HI_MW_SIF_AAOS_MODE_STEREO_SAP = 4,
    HI_MW_SIF_AAOS_MODE_NICAM_MONO = 5,
    HI_MW_SIF_AAOS_MODE_NICAM_STEREO = 6,
    HI_MW_SIF_AAOS_MODE_NICAM_DUAL = 7,
    HI_MW_SIF_AAOS_MODE_NICAM_FM_MOMO = 8,

    HI_MW_SIF_AAOS_MODE_BUTT = 9,
};
/* RF 波段 */
enum HI_MW_ATVBAND_E {
    HI_MW_ATVBAND_VHF_LOW = 0,
    HI_MW_ATVBAND_VHF_HIGH = 1,
    HI_MW_ATVBAND_UHF = 2,
    HI_MW_ATVBAND_ALL = 3,

    HI_MW_ATVBAND_BUTT = 4,
};

/* i2s(Inter-IC Sound) channels */
enum HI_MW_I2S_CHNUM_E {
    HI_MW_I2S_CHNUM_1 = 1,
    HI_MW_I2S_CHNUM_2 = 2, /* only work for I2S mode */
    HI_MW_I2S_CHNUM_8 = 8, /* only work for I2S mode */

    HI_MW_I2S_CHNUM_BUTT = 9,
};

/* i2s(Inter-IC Sound) bit depth */
enum HI_MW_I2S_BITDEPTH_E {
    HI_MW_I2S_BITDEPTH_16 = 16,
    HI_MW_I2S_BITDEPTH_24 = 24, /* only work for I2S mode */

    HI_MW_I2S_BITDEPTH_BUTT = 25,
};

/* hdmi audio data format def */
enum HI_MW_HDMI_AUDIO_FMT_E {
    HI_MW_HDMI_AUDIO_FMT_LPCM = 0, /* LPCM 2/8 channels, Audio Sample Packet layout0 or layout1 */
    HI_MW_HDMI_AUDIO_FMT_LBR = 1,  /* IEC-61937 DD/DDP/DTS, Audio Sample Packet layout0 */
    HI_MW_HDMI_AUDIO_FMT_HBR = 8,  /* IEC-61937 DTSHD/TrueHD, High-Bitrate (HBR) Audio Stream Packet */

    HI_MW_HDMI_AUDIO_FMT_BUTT = 9,
};

enum HI_MW_HDMI_DATA_TYPE_E {
    HI_MW_HDMI_DATA_PCM = 0,
    HI_MW_HDMI_DATA_DD = 1,
    HI_MW_HDMI_DATA_DDP = 2,
    HI_MW_HDMI_DATA_DTS = 3,
    HI_MW_HDMI_DATA_DTSHD = 4,
    HI_MW_HDMI_DATA_TRUEHD = 5,
    HI_MW_HDMI_DATA_UNKNOWN = 6,
    HI_MW_HDMI_DATA_MAT = 7,

    HI_MW_HDMI_DATA_BUTT = 8,
};

/* HDMI port ID */
enum HI_MW_HDMIRX_PORT_E {
    HI_MW_HDMIRX_PORT0,
    /* *<HDMIRX port 0 */ /* *<CNcomment:HDMIRX port0  */
    HI_MW_HDMIRX_PORT1,
    /* *<HDMIRX port 1 */ /* *<CNcomment:HDMIRX port1  */
    HI_MW_HDMIRX_PORT2,
    /* *<HDMIRX port 2 */ /* *<CNcomment:HDMIRX port2  */
    HI_MW_HDMIRX_PORT3,
    /* *<HDMIRX port 3 */ /* *<CNcomment:HDMIRX port3  */
    // HI_MW_HDMIRX_PORT_ALL,    /* *<HDMIRX ALL port */ /* CNcomment/:HDMIRX 所有port口 */
    HI_MW_HDMIRX_PORT_BUTT
};

/* HDMI接口ID */
enum HI_MW_HDMITX_ID_E {
    HI_MW_HDMITX_ID_0 = 0,
    /* *<HDMI interface 0 */ /* CNcomment/:HDMI接口0 */
    HI_MW_HDMITX_ID_BUTT = 1,
};

/* CNcomment/: 定义视频制式枚举 */
enum HI_MW_HDMITX_FMT_E {
    HI_MW_HDMITX_FMT_AUTO = 0,
    HI_MW_HDMITX_FMT_861D_640X480_60 = 1,
    HI_MW_HDMITX_FMT_480P_60 = 2,
    HI_MW_HDMITX_FMT_720P_60 = 3,
    HI_MW_HDMITX_FMT_VESA_1366X768_60 = 4,
    HI_MW_HDMITX_FMT_1080P_50 = 5,
    HI_MW_HDMITX_FMT_1080P_60 = 6,
    HI_MW_HDMITX_FMT_3840X2160_30 = 7,
    HI_MW_HDMITX_FMT_3840X2160_50 = 8,
    HI_MW_HDMITX_FMT_3840X2160_60 = 9,

    HI_MW_HDMITX_FMT_BUTT = 10
};

/* 定义颜色空间和位深枚举 */
enum HI_MW_HDMITX_COLOR_FORMAT_E {
    HI_MW_HDMITX_COLOR_RGB444_8 = 0,
    HI_MW_HDMITX_COLOR_RGB444_10 = 1,
    HI_MW_HDMITX_COLOR_RGB444_12 = 2,
    HI_MW_HDMITX_COLOR_YCBCR444_8 = 3,
    HI_MW_HDMITX_COLOR_YCBCR444_10 = 4,
    HI_MW_HDMITX_COLOR_YCBCR444_12 = 5,
    HI_MW_HDMITX_COLOR_YCBCR422 = 6,
    HI_MW_HDMITX_COLOR_YCBCR420_8 = 7,
    HI_MW_HDMITX_COLOR_YCBCR420_10 = 8,
    HI_MW_HDMITX_COLOR_YCBCR420_12 = 9,

    HI_MW_HDMITX_COLOR_BUTT = 10,
};

enum HI_MW_HDMITX_COLOR_FORMAT_AUTO_E {
    HI_MW_HDMITX_COLOR_RGB444_AUTO_8 = 0,
    HI_MW_HDMITX_COLOR_YCBCR444_AUTO_8 = 1,
    HI_MW_HDMITX_COLOR_YCBCR420_AUTO_8 = 2,
    HI_MW_HDMITX_COLOR_YCBCR420_AUTO_10 = 3,
    HI_MW_HDMITX_COLOR_AUTO_BUTT = 4,
};

/* 定义音频采样率枚举 */
enum HI_MW_SAMPLE_RATE_E {
    HI_MW_SAMPLE_RATE_UNKNOWN = 0,   /* 未知采样频率 */
    HI_MW_SAMPLE_RATE_8K = 8000,     /* 8K采样频率 */
    HI_MW_SAMPLE_RATE_11K = 11025,   /* 11.025K采样频率 */
    HI_MW_SAMPLE_RATE_12K = 12000,   /* 12K采样频率 */
    HI_MW_SAMPLE_RATE_16K = 16000,   /* 16K采样频率 */
    HI_MW_SAMPLE_RATE_22K = 22050,   /* 22.050K采样频率 */
    HI_MW_SAMPLE_RATE_24K = 24000,   /* 24K采样频率 */
    HI_MW_SAMPLE_RATE_32K = 32000,   /* 32K采样频率 */
    HI_MW_SAMPLE_RATE_44K = 44100,   /* 44.1K采样频率 */
    HI_MW_SAMPLE_RATE_48K = 48000,   /* 48K采样频率 */
    HI_MW_SAMPLE_RATE_88K = 88200,   /* 88.2K采样频率 */
    HI_MW_SAMPLE_RATE_96K = 96000,   /* 96K采样频率 */
    HI_MW_SAMPLE_RATE_176K = 176400, /* 176K采样频率 */
    HI_MW_SAMPLE_RATE_192K = 192000, /* 192K采样频率 */
    HI_MW_SAMPLE_RATE_288K = 288000, /* 288K采样频率 */

    HI_MW_SAMPLE_RATE_BUTT
};

/* 开机音乐类型 */
enum HI_MW_POWERMUSIC_E {
    HI_MW_POWERMUSIC_FIRST = 1,
    HI_MW_POWERMUSIC_SECOND = 2,

    HI_MW_POWERMUSIC_BUTT = 3,
};

/* 上电开机模式 */
enum HI_MW_POWERMODE_E {
    HI_MW_POWERMODE_IMMEDIACY = 0,  // 立即开机
    HI_MW_POWERMODE_STANDBY = 1,    // 待机
    HI_MW_POWERMODE_RESTORE = 2,    // 恢复关机前状态

    HI_MW_POWERMODE_BUTT = 3,
};

/* 开机记忆模式的标志位 */
enum HI_MW_RESTORE_E {
    HI_MW_RESTORE_AC = 0,  // 记忆模式开机标志位
    HI_MW_RESTORE_DC = 1,  // 记忆模式待机标志位

    HI_MW_RESTORE_BUTT = 2,
};

/* 开机类型 */
enum HI_MW_POWERTYPE_E {
    HI_MW_POWERTYPE_TIMER = 0,   // 定时器
    HI_MW_POWERTYPE_OTHERS = 1,  // 非定时器的其他方式

    HI_MW_POWERTYPE_BUTT = 2,
};

/* 待机模式 */
enum HI_MW_SUSPEND_E {
    HI_MW_SUSPEND_OFF = 0,  /* *<Power off  */
    HI_MW_SUSPEND_MEM = 1,  /* *<Suspend to mem  */
    HI_MW_SUSPEND_DISK = 2, /* *<Suspend to disk  */

    HI_MW_SUSPEND_BUTT = 3,
};

enum HI_MW_WOLMODE_E {
    HI_MW_WOLMODE_OFF = 0x0,
    HI_MW_WOLMODE_MAGIC = 0x1,
    HI_MW_WOLMODE_BUTT = 0x100,
};

enum HI_MW_SYSTEM_WAKEUP_E {
    HI_MW_SYSTEM_WAKEUP_IR = 0,      /* Woken by the IR module */
    HI_MW_SYSTEM_WAKEUP_KEYLED = 1,  /* Woken by the keys on the front panel */
    HI_MW_SYSTEM_WAKEUP_TIMEOUT = 2, /* Woken by the timing interrupt */
    HI_MW_SYSTEM_WAKEUP_ETH = 3,     /* Woken by the ethernet interrupt */
    HI_MW_SYSTEM_WAKEUP_USB = 4,     /* Woken by the USB interrupt */
    HI_MW_SYSTEM_WAKEUP_GPIO = 5,    /* Woken by the GPIO interrupt */
    HI_MW_SYSTEM_WAKEUP_LSADC = 6,   /* Woken by the LSADC */
    HI_MW_SYSTEM_WAKEUP_CEC = 7,     /* Woken by the CEC */
    HI_MW_SYSTEM_WAKEUP_UART = 8,    /* Woken by the UART */
    HI_MW_SYSTEM_WAKEUP_VGA = 9,     /* Woken by the VGA */
    HI_MW_SYSTEM_WAKEUP_HDMI = 10,   /* Woken by the HDMI */
    HI_MW_SYSTEM_WAKEUP_SCART = 11,  /* Woken by the SCART */
    HI_MW_SYSTEM_WAKEUP_TIMEOUT_BG = 12, /* Woken by the timeout to scan channel */

    HI_MW_SYSTEM_WAKEUP_BUTT,
};

/* 定义色彩校正的状态 */
enum HI_MW_COLOR_CALIB_STATE_E {
    HI_MW_COLOR_CALIB_BEGIN = 0,   /* 颜色校正开始 */
    HI_MW_COLOR_CALIB_FINISH = 1,  /* 颜色校正完成 */
    HI_MW_COLOR_CALIB_TIMEOUT = 2, /* 颜色校正超时出错 */
    HI_MW_COLOR_CALIB_FAIL = 3,    /* 颜色校正失败 */

    HI_MW_COLOR_CALIB_BUTT = 4,
};

/* ADC / PC 自动校正状态 */
enum HI_MW_ADJSTAT_E {
    HI_MW_ADJSTAT_BEGIN = 0,
    HI_MW_ADJSTAT_FINISH = 1,
    HI_MW_ADJSTAT_TIMEOUT = 2,
    HI_MW_ADJSTAT_FAIL = 3,

    HI_MW_ADJSTAT_BUTT = 4,
};

/* PC手动调整的类型 */
enum HI_MW_VGA_ADJUST_E {
    HI_MW_VGA_ADJUST_HPOS = 0,  /* 手动水平位置调整 */
    HI_MW_VGA_ADJUST_VPOS = 1,  /* 手动垂直位置调整 */
    HI_MW_VGA_ADJUST_PHASE = 2, /* 手动相位调整 */
    HI_MW_VGA_ADJUST_CLOCK = 3, /* 手动时钟调整 */

    HI_MW_VGA_ADJUST_BUTT = 4,
};

/* 测试图像的枚举 */
enum HI_MW_PATTERN_E {
    HI_MW_PATTERN_OFF = 0,
    HI_MW_PATTERN_BLACK = 1,
    HI_MW_PATTERN_WHITE = 2,
    HI_MW_PATTERN_RED = 3,
    HI_MW_PATTERN_GREEN = 4,
    HI_MW_PATTERN_BLUE = 5,
    HI_MW_PATTERN_GRAY = 6,

    HI_MW_PATTERN_BUTT = 7,
};

/* 屏幕倒装方式 */
enum HI_MW_FLIPMODE_E {
    HI_MW_FLIPMODE_OFF = 0,         /* normal */
    HI_MW_FLIPMODE_FLIP = 1,        /* flip */
    HI_MW_FLIPMODE_MIRROR = 2,      /* mirror */
    HI_MW_FLIPMODE_MIRROR_FLIP = 3, /* mirror and flip */

    HI_MW_FLIPMODE_BUTT = 4,
};

/* 无信号时的操作 */
enum HI_MW_NOSIGNALOPERA_E {
    HI_MW_NOSIGNALOPERA_STANDBY = 0,   // 无信号进入待机
    HI_MW_NOSIGNALOPERA_POWEROFF = 1,  // 无信号进入关机

    HI_MW_NOSIGNALOPERA_BUTT = 2,
};

/* 色彩空间 */
enum HI_MW_COLOR_SPACE_E {
    HI_MW_COLOR_SPACE_UNKNOWN = 0,

    HI_MW_COLOR_SPACE_BT601_YUV_LIMITED = 0x10, /* ::::Current Used:::: BT.601  */
    HI_MW_COLOR_SPACE_BT601_YUV_FULL = 0x11,
    HI_MW_COLOR_SPACE_BT601_RGB_LIMITED = 0x12,
    HI_MW_COLOR_SPACE_BT601_RGB_FULL = 0x13,
    HI_MW_COLOR_SPACE_NTSC1953 = 0x20,
    HI_MW_COLOR_SPACE_BT470_SYSTEM_M = 0x30, /* These should be useful.  Assume 601 extents. */
    HI_MW_COLOR_SPACE_BT470_SYSTEM_BG = 0x31,
    HI_MW_COLOR_SPACE_BT709_YUV_LIMITED = 0x40, /* ::::Current Used:::: BT.709 */
    HI_MW_COLOR_SPACE_BT709_YUV_FULL = 0x41,
    HI_MW_COLOR_SPACE_BT709_RGB_LIMITED = 0x42,
    HI_MW_COLOR_SPACE_BT709_RGB_FULL = 0x43,     /* ::::Current Used:::: */
    HI_MW_COLOR_SPACE_BT2020_YUV_LIMITED = 0x44, /* ::::Current Used:::: BT.2020 */
    HI_MW_COLOR_SPACE_BT2020_YUV_FULL = 0x45,
    HI_MW_COLOR_SPACE_BT2020_RGB_LIMITED = 0x46,
    HI_MW_COLOR_SPACE_BT2020_RGB_FULL = 0x47, /* ::::Current Used:::: */
    HI_MW_COLOR_SPACE_REC709 = 0x50,          /* HD and modern captures. */
    HI_MW_COLOR_SPACE_SMPT170M = 0x60,        /* ITU-R 601 -- broadcast NTSC/PAL */
    HI_MW_COLOR_SPACE_SMPT240M = 0x61,        /* 1125-Line (US) HDTV */
    HI_MW_COLOR_SPACE_BT878 = 0x70,           /* broken BT878 extents(601, luma range 16-253 instead of 16-235) */
    HI_MW_COLOR_SPACE_XVYCC = 0x80,
    HI_MW_COLOR_SPACE_JPEG = 0x90,
    HI_MW_COLOR_SPACE_RGB = 0xa0,
    HI_MW_COLOR_SPACE_BUTT,
};

enum HI_MW_OVERSAMPLE_MODE_E {
    HI_MW_OVERSAMPLE_1X = 0,
    HI_MW_OVERSAMPLE_2X = 1,
    HI_MW_OVERSAMPLE_4X = 2,

    HI_MW_OVERSAMPLE_BUTT = 3,
};

enum HI_MW_PIXEL_BITWIDTH_E {
    HI_MW_PIXEL_BITWIDTH_8BIT = 0,
    HI_MW_PIXEL_BITWIDTH_10BIT = 1,
    HI_MW_PIXEL_BITWIDTH_12BIT = 2,
    HI_MW_PIXEL_BITWIDTH_16BIT = 3,

    HI_MW_PIXEL_BITWIDTH_BUTT = 4,
};

enum HI_MW_VIDEO_FORMAT_E {
    HI_MW_FORMAT_YUV_SEMIPLANAR_422 = 0, /* *<The YUV spatial sampling format is 4:2:2. */
    HI_MW_FORMAT_YUV_SEMIPLANAR_420 = 1, /* *<The YUV spatial sampling format is 4:2:0, V first. */
    HI_MW_FORMAT_YUV_SEMIPLANAR_400 = 2,
    HI_MW_FORMAT_YUV_SEMIPLANAR_411 = 3,
    HI_MW_FORMAT_YUV_SEMIPLANAR_422_1X2 = 4,
    HI_MW_FORMAT_YUV_SEMIPLANAR_444 = 5,
    HI_MW_FORMAT_YUV_SEMIPLANAR_420_UV = 6, /* *<The YUV spatial sampling format is 4:2:0,U first. */

    HI_MW_FORMAT_YUV_PACKAGE_UYVY = 7, /* format is package and the pixel arrangement sequence in the memory is UYVY */
    HI_MW_FORMAT_YUV_PACKAGE_YUYV = 8, /* format is package and the pixel arrangement sequence in the memory is YUYV */
    HI_MW_FORMAT_YUV_PACKAGE_YVYU = 9, /* format is package and the pixel arrangement sequence in the memory is YVYU */
    HI_MW_FORMAT_YUV_PACKAGE_YUV = 10, /* format is package and the pixel arrangement sequence in the memory is YUV */

    HI_MW_FORMAT_YUV_PLANAR_400 = 11,
    HI_MW_FORMAT_YUV_PLANAR_411 = 12,
    HI_MW_FORMAT_YUV_PLANAR_420 = 13,
    HI_MW_FORMAT_YUV_PLANAR_422_1X2 = 14,
    HI_MW_FORMAT_YUV_PLANAR_422_2X1 = 15,
    HI_MW_FORMAT_YUV_PLANAR_444 = 16,
    HI_MW_FORMAT_YUV_PLANAR_410 = 17,
    HI_MW_FORMAT_YUV_BUTT = 18,

    HI_MW_FORMAT_RGB_SEMIPLANAR_444 = 19,
    HI_MW_FORMAT_RGB_RGB888 = 20,
    HI_MW_FORMAT_RGB_ARGB8888 = 21,

    HI_MW_FORMAT_RGB_BUTT = 22,
};

/* Color Space */ /* CNcomment/:色域空间 */
enum HI_MW_PQ_COLOR_SPACE_E {
    HI_MW_CS_UNKNOWN = 0,
    HI_MW_CS_DEFAULT = 1,
    HI_MW_CS_BT601_YUV_LIMITED = 2, /* ::::Current Used:::: BT.601  */
    HI_MW_CS_BT601_YUV_FULL = 3,
    HI_MW_CS_BT601_RGB_LIMITED = 4,
    HI_MW_CS_BT601_RGB_FULL = 5,
    HI_MW_CS_NTSC1953 = 6,
    HI_MW_CS_BT470_SYSTEM_M = 7,
    HI_MW_CS_BT470_SYSTEM_BG = 8,
    HI_MW_CS_BT709_YUV_LIMITED = 9, /* ::::Current Used:::: BT.709 */
    HI_MW_CS_BT709_YUV_FULL = 10,
    HI_MW_CS_BT709_RGB_LIMITED = 11,
    HI_MW_CS_BT709_RGB_FULL = 12,     /* ::::Current Used:::: */
    HI_MW_CS_BT2020_YUV_LIMITED = 13, /* ::::Current Used:::: BT.2020 */
    HI_MW_CS_BT2020_YUV_FULL = 14,
    HI_MW_CS_BT2020_RGB_LIMITED = 15,
    HI_MW_CS_BT2020_RGB_FULL = 16, /* ::::Current Used:::: */
    HI_MW_CS_REC709 = 17,          /* HD and modern captures. */
    HI_MW_CS_SMPT170M = 18,        /* ITU-R 601 -- broadcast NTSC/PAL */
    HI_MW_CS_SMPT240M = 19,        /* 1125-Line (US) HDTV */
    HI_MW_CS_BT878 = 20,
    HI_MW_CS_XVYCC = 21,
    HI_MW_CS_JPEG = 22,
    HI_MW_CS_RGB = 23,

    HI_MW_CS_BUTT = 24,
};

/* Gamma Type */ /* CNcomment/:Gamma 类型 */
enum HI_MW_PQ_GAMMA_TYPE_E {
    HI_MW_GAMMA_TYPE_1886 = 0,
    HI_MW_GAMMA_TYPE_2084 = 1,

    HI_MW_GAMMA_TYPE_BUTT = 2,
};

/* color system type def */
enum HI_MW_COLOR_SYS_E {
    HI_MW_COLOR_SYS_AUTO = 0,
    HI_MW_COLOR_SYS_PAL = 1,
    HI_MW_COLOR_SYS_NTSC = 2,
    HI_MW_COLOR_SYS_SECAM = 3,
    HI_MW_COLOR_SYS_PALM = 4,
    HI_MW_COLOR_SYS_PALN = 5,
    HI_MW_COLOR_SYS_PAL60 = 6,
    HI_MW_COLOR_SYS_NTSC443 = 7,
    HI_MW_COLOR_SYS_NTSC50 = 8,

    HI_MW_COLOR_SYS_BUTT = 9,
};

// add for custom picture card
enum HI_MW_TEST_PATTERN_E {
    HI_MW_TEST_PATTERN_0IRE = 0,
    HI_MW_TEST_PATTERN_10IRE = 1,
    HI_MW_TEST_PATTERN_20IRE = 2,
    HI_MW_TEST_PATTERN_30IRE = 3,
    HI_MW_TEST_PATTERN_40IRE = 4,
    HI_MW_TEST_PATTERN_50IRE = 5,
    HI_MW_TEST_PATTERN_60IRE = 6,
    HI_MW_TEST_PATTERN_70IRE = 7,
    HI_MW_TEST_PATTERN_80IRE = 8,
    HI_MW_TEST_PATTERN_90IRE = 9,
    HI_MW_TEST_PATTERN_100IRE = 10,
    HI_MW_TEST_PATTERN_RED = 11,
    HI_MW_TEST_PATTERN_GREEN = 12,
    HI_MW_TEST_PATTERN_BLUE = 13,
    HI_MW_TEST_PATTERN_CUSTOM = 14,

    HI_MW_TEST_PATTERN_BUTT
};

enum HI_MW_PIXEL_FORMAT_E {
    /* RGB formats */
    HI_MW_PIX_FMT_RGB332 = 0,  /*  8  RGB-3-3-2     */
    HI_MW_PIX_FMT_RGB444 = 1,  /* 16  xxxxrrrr ggggbbbb */
    HI_MW_PIX_FMT_RGB555 = 2,  /* 16  RGB-5-5-5     */
    HI_MW_PIX_FMT_RGB565 = 3,  /* 16  RGB-5-6-5     */
    HI_MW_PIX_FMT_BGR565 = 4,  /* 16  RGB-5-6-5     */
    HI_MW_PIX_FMT_RGB555X = 5, /* 16  RGB-5-5-5 BE  */
    HI_MW_PIX_FMT_RGB565X = 6, /* 16  RGB-5-6-5 BE  */
    HI_MW_PIX_FMT_BGR666 = 7,  /* 18  BGR-6-6-6   */
    HI_MW_PIX_FMT_BGR24 = 8,   /* 24  BGR-8-8-8     */
    HI_MW_PIX_FMT_RGB24 = 9,   /* 24  RGB-8-8-8     */
    HI_MW_PIX_FMT_BGR32 = 10,  /* 32  BGR-8-8-8-8   */
    HI_MW_PIX_FMT_RGB32 = 11,  /* 32  RGB-8-8-8-8   */

    /* ***** HISI CLUT formats ***** */
    HI_MW_PIX_FMT_CLUT_1BPP = 12,
    HI_MW_PIX_FMT_CLUT_2BPP = 13,
    HI_MW_PIX_FMT_CLUT_4BPP = 14,
    HI_MW_PIX_FMT_CLUT_8BPP = 15,
    HI_MW_PIX_FMT_ACLUT_44 = 16,
    HI_MW_PIX_FMT_ACLUT_88 = 17,

    /* ***** HISI 16bit RGB formats ***** */
    HI_MW_PIX_FMT_ARGB4444 = 18,
    HI_MW_PIX_FMT_ABGR4444 = 19,
    HI_MW_PIX_FMT_RGBA4444 = 20,

    HI_MW_PIX_FMT_ARGB1555 = 21,
    HI_MW_PIX_FMT_ABGR1555 = 22,
    HI_MW_PIX_FMT_RGBA5551 = 23,

    /* ***** HISI 24bit RGB formats ***** */
    HI_MW_PIX_FMT_ARGB8565 = 24,
    HI_MW_PIX_FMT_ABGR8565 = 25,
    HI_MW_PIX_FMT_RGBA5658 = 26,

    HI_MW_PIX_FMT_ARGB6666 = 27,
    HI_MW_PIX_FMT_RGBA6666 = 28,

    /* ***** HISI 32bit RGB formats ***** */
    HI_MW_PIX_FMT_ARGB8888 = 29,
    HI_MW_PIX_FMT_ABGR8888 = 30,
    HI_MW_PIX_FMT_RGBA8888 = 31,

    HI_MW_PIX_FMT_AYUV8888 = 32,
    HI_MW_PIX_FMT_YUVA8888 = 33,

    /* Grey formats */
    HI_MW_PIX_FMT_GREY = 34, /*  8  Greyscale     */
    HI_MW_PIX_FMT_Y4 = 35,   /*  4  Greyscale     */
    HI_MW_PIX_FMT_Y6 = 36,   /*  6  Greyscale     */
    HI_MW_PIX_FMT_Y10 = 37,  /* 10  Greyscale     */
    HI_MW_PIX_FMT_Y12 = 38,  /* 12  Greyscale     */
    HI_MW_PIX_FMT_Y16 = 39,  /* 16  Greyscale     */

    /* Grey bit-packed formats */
    HI_MW_PIX_FMT_Y10BPACK = 40, /* 10  Greyscale bit-packed */

    /* Palette formats */
    HI_MW_PIX_FMT_PAL8 = 41, /*  8  8-bit palette */

    /* Luminance+Chrominance formats */
    HI_MW_PIX_FMT_YVU410 = 42,  /*  9  YVU 4:1:0     */
    HI_MW_PIX_FMT_YVU420 = 43,  /* 12  YVU 4:2:0     */
    HI_MW_PIX_FMT_YUYV = 44,    /* 16  YUV 4:2:2     */
    HI_MW_PIX_FMT_YYUV = 45,    /* 16  YUV 4:2:2     */
    HI_MW_PIX_FMT_YVYU = 46,    /* 16  YVU 4:2:2 */
    HI_MW_PIX_FMT_UYVY = 47,    /* 16  YUV 4:2:2     */
    HI_MW_PIX_FMT_VYUY = 48,    /* 16  YUV 4:2:2     */
    HI_MW_PIX_FMT_YUV422P = 49, /* 16  YVU422 planar */
    HI_MW_PIX_FMT_YUV411P = 50, /* 16  YVU411 planar */
    HI_MW_PIX_FMT_Y41P = 51,    /* 12  YUV 4:1:1     */
    HI_MW_PIX_FMT_YUV444 = 52,  /* 16  xxxxyyyy uuuuvvvv */
    HI_MW_PIX_FMT_YUV555 = 53,  /* 16  YUV-5-5-5     */
    HI_MW_PIX_FMT_YUV565 = 54,  /* 16  YUV-5-6-5     */
    HI_MW_PIX_FMT_YUV32 = 55,   /* 32  YUV-8-8-8-8   */
    HI_MW_PIX_FMT_YUV410 = 56,  /*  9  YUV 4:1:0     */
    HI_MW_PIX_FMT_YUV420 = 57,  /* 12  YUV 4:2:0     */
    HI_MW_PIX_FMT_HI240 = 58,   /*  8  8-bit color   */
    HI_MW_PIX_FMT_HM12 = 59,    /*  8  YUV 4:2:0 16x16 macroblocks */
    HI_MW_PIX_FMT_M420 = 60,    /* 12  YUV 4:2:0 2 lines y,1 line uv interleaved */

    /* two planes -- one Y, one Cr + Cb interleaved  */
    HI_MW_PIX_FMT_NV08 = 61,     /* 08  Y/CbCr 4:0:0 @ */
    HI_MW_PIX_FMT_NV80 = 62,     /* 08  Y/CrCb 4:0:0 @ */
    HI_MW_PIX_FMT_NV12 = 63,     /* 12  Y/CbCr 4:2:0  */
    HI_MW_PIX_FMT_NV21 = 64,     /* 12  Y/CrCb 4:2:0  */
    HI_MW_PIX_FMT_NV12_411 = 65, /* 12  Y/CbCr 4:1:1  @ */
    HI_MW_PIX_FMT_NV16 = 66,     /* 16  Y/CbCr 4:2:2  */
    HI_MW_PIX_FMT_NV61 = 67,     /* 16  Y/CrCb 4:2:2  */
    HI_MW_PIX_FMT_NV16_2X1 = 68, /* 16  Y/CbCr 4:2:2 2X1 @ */
    HI_MW_PIX_FMT_NV61_2X1 = 69, /* 16  Y/CrCb 4:2:2  2X1 @ */
    HI_MW_PIX_FMT_NV24 = 70,     /* 24  Y/CbCr 4:4:4  */
    HI_MW_PIX_FMT_NV42 = 71,     /* 24  Y/CrCb 4:4:4  */
    HI_MW_PIX_FMT_NV42_RGB = 72, /* 24  G/RB 4:4:4  */

    /* two non contiguous planes - one Y, one Cr + Cb interleaved  */
    HI_MW_PIX_FMT_NV12M = 73,  /* 12  Y/CbCr 4:2:0  */
    HI_MW_PIX_FMT_NV12MT = 74, /* 12  Y/CbCr 4:2:0 64x32 macroblocks */

    /* three non contiguous planes - Y, Cb, Cr */
    HI_MW_PIX_FMT_YUV420M = 75, /* 12  YUV420 planar */

    /* Bayer formats - see http://www.siliconimaging.com/RGB%20Bayer.htm */
    HI_MW_PIX_FMT_SBGGR8 = 76,  /*  8  BGBG.. GRGR.. */
    HI_MW_PIX_FMT_SGBRG8 = 77,  /*  8  GBGB.. RGRG.. */
    HI_MW_PIX_FMT_SGRBG8 = 78,  /*  8  GRGR.. BGBG.. */
    HI_MW_PIX_FMT_SRGGB8 = 79,  /*  8  RGRG.. GBGB.. */
    HI_MW_PIX_FMT_SBGGR10 = 80, /* 10  BGBG.. GRGR.. */
    HI_MW_PIX_FMT_SGBRG10 = 81, /* 10  GBGB.. RGRG.. */
    HI_MW_PIX_FMT_SGRBG10 = 82, /* 10  GRGR.. BGBG.. */
    HI_MW_PIX_FMT_SRGGB10 = 83, /* 10  RGRG.. GBGB.. */
    HI_MW_PIX_FMT_SBGGR12 = 84, /* 12  BGBG.. GRGR.. */
    HI_MW_PIX_FMT_SGBRG12 = 85, /* 12  GBGB.. RGRG.. */
    HI_MW_PIX_FMT_SGRBG12 = 86, /* 12  GRGR.. BGBG.. */
    HI_MW_PIX_FMT_SRGGB12 = 87, /* 12  RGRG.. GBGB.. */

    /* ***** HISI  contiguoustwo planes -- one Y, one Cr + Cb interleaved ***** */
    HI_MW_PIX_FMT_NV08_CMP = 88,       /* 08   Y/CbCr 4:0:0 compressed @ */
    HI_MW_PIX_FMT_NV80_CMP = 89,       /* 08   Y/CrCb 4:0:0 compressed @ */
    HI_MW_PIX_FMT_NV12_CMP = 90,       /* 12  Y/CbCr 4:2:0 compressed */
    HI_MW_PIX_FMT_NV21_CMP = 91,       /* 12  Y/CrCb 4:2:0 compressed */
    HI_MW_PIX_FMT_NV16_CMP = 92,       /* 16  Y/CbCr 4:2:2 compressed */
    HI_MW_PIX_FMT_NV61_CMP = 93,       /* 16  Y/CrCb 4:2:2 compressed */
    HI_MW_PIX_FMT_NV16_2X1_CMP = 94,   /* 16  Y/CbCr 4:2:2   2X1@ */
    HI_MW_PIX_FMT_NV61_2X1_CMP = 95,   /* 16  Y/CrCb 4:2:2  2X1@ */
    HI_MW_PIX_FMT_NV24_CMP = 96,       /* 24  Y/CbCr 4:4:4 compressed */
    HI_MW_PIX_FMT_NV42_CMP = 97,       /* 24  Y/CrCb 4:4:4 compressed */
    HI_MW_PIX_FMT_NV12_TILE = 98,      /* 12 tile  */
    HI_MW_PIX_FMT_NV21_TILE = 99,      /* 21 tile  */
    HI_MW_PIX_FMT_YUV400_TILE = 100,   /* 21 tile  */
    HI_MW_PIX_FMT_NV12_TILE_CMP = 101, /* 12 tile compressed */
    HI_MW_PIX_FMT_NV21_TILE_CMP = 102, /* 21 tile compressed */

    /* ***** HISI three non contiguous planes - Y, Cb, Cr ***** */
    HI_MW_PIX_FMT_YUV400 = 103,     /* 08  YUV400 planar @ */
    HI_MW_PIX_FMT_YUV410p = 104,    /* 10  YUV410 planar @ */
    HI_MW_PIX_FMT_YUV420p = 105,    /* 12  YUV420 planar @ */
    HI_MW_PIX_FMT_YUV411 = 106,     /* 12  YUV411  planar @ */
    HI_MW_PIX_FMT_YUV422_1X2 = 107, /* 16  YUV422  planar 1X2 @ */
    HI_MW_PIX_FMT_YUV422_2X1 = 108, /* 16  YUV422  planar 2X1@ */
    HI_MW_PIX_FMT_YUV_444 = 109,    /* 24  YUV444  planar @ */

    HI_MW_PIX_BUTT = 110,
};

enum HI_MW_BUF_ADDR_E {
    HI_MW_BUF_ADDR_LEFT = 0,
    HI_MW_BUF_ADDR_RIGHT = 1, /* only for right eye frame of 3D video */
    HI_MW_BUF_ADDR_MAX = 2,
};

/* video frame type */
enum HI_MW_FRAME_TYPE_E {
    HI_MW_FT_NOT_STEREO = 0,
    HI_MW_FT_SBS = 1,
    HI_MW_FT_TAB = 2,
    HI_MW_FT_FPK = 3,
    HI_MW_FT_FS = 4,

    HI_MW_FT_BUTT = 5,
};

enum HI_MW_VO_SOURCE_E {
    HI_MW_VO_SOURCE_DTV = 0,
    HI_MW_VO_SOURCE_USB = 1,
    HI_MW_VO_SOURCE_ATV = 2,
    HI_MW_VO_SOURCE_SCART = 3,
    HI_MW_VO_SOURCE_SVIDEO = 4,
    HI_MW_VO_SOURCE_CVBS = 5,
    HI_MW_VO_SOURCE_VGA = 6,
    HI_MW_VO_SOURCE_YPBPR = 7,
    HI_MW_VO_SOURCE_HDMI = 8,
    HI_MW_VO_SOURCE_GRAPHIC = 9,

    HI_MW_VO_SOURCE_BUTT = 10,
};

/* defines the ROTATION type */
/* 视频旋转角度 */
typedef enum hiMW_VO_ROTATION_E {
    HI_MW_VO_ROTATION_0 = 0,   /* 0度旋转 */
    HI_MW_VO_ROTATION_90 = 1,  /* 90度旋转 */
    HI_MW_VO_ROTATION_180 = 2, /* 180度旋转 */
    HI_MW_VO_ROTATION_270 = 3, /* 270度旋转 */
    HI_MW_VO_ROTATION_BUTT = 4,
} HI_MW_VO_ROTATION_E;

/* define the enum of tv video decoder signal type */
enum HI_MW_TVD_RESOLUTION_E {
    HI_MW_TVD_NTSC = 0,
    HI_MW_TVD_PAL = 1,
    HI_MW_TVD_SECAM = 2,
    HI_MW_TVD_NTSC_443 = 3,
    HI_MW_TVD_PAL_M = 4,
    HI_MW_TVD_PAL_NC = 5,

    HI_MW_TVD_BUTT = 6,
};

/* define the enum of no linear adjust items */
enum HI_MW_NLA_E {
    HI_MW_NLA_BRIGHTNESS = 0,
    HI_MW_NLA_CONTRAST = 1,
    HI_MW_NLA_SATURATION = 2,
    HI_MW_NLA_SHARPNESS = 3,
    HI_MW_NLA_HUE = 4,
    HI_MW_NLA_BACKLIGHT = 5,

    HI_MW_NLA_BUTT = 6,
};

/* EDID 的类型 */
enum HI_MW_EDID_TYPE_E {
    HI_MW_EDID_TYPE_1080P = 0,
    HI_MW_EDID_TYPE_4K = 1,

    HI_MW_EDID_TYPE_BUTT = 2,
};

/* bandwide  类型 */
enum HI_MW_ATV_SIF_BW_E {
    HI_MW_ATV_SIF_BW_WIDE = 0,   /* For Auto Scan Mode */
    HI_MW_ATV_SIF_BW_NORMAL = 1, /* For Normal Play Mode */
    HI_MW_ATV_SIF_BW_NARROW = 2, /* Not Recommended */

    HI_MW_ATV_SIF_BW_BUTT = 3,
};

/* TUNER设备类型 */
enum HI_MW_TUNER_TYPE_E {
    HI_MW_TUNER_TYPE_XG_3BL = 0,        /* 支持旭光高频头 */
    HI_MW_TUNER_TYPE_CD1616 = 1,        /* 支持CD1616高频头 */
    HI_MW_TUNER_TYPE_ALPS_TDAE = 2,     /* 支持ALPS_TDAE高频头 */
    HI_MW_TUNER_TYPE_TDCC = 3,          /* 支持TDCC高频头 */
    HI_MW_TUNER_TYPE_TDA18250 = 4,      /* 支持TDA18250高频头 */
    HI_MW_TUNER_TYPE_CD1616_DOUBLE = 5, /* 支持CD1616带双agc高频头 */
    HI_MW_TUNER_TYPE_MT2081 = 6,        /* 支持MT2081高频头 */
    HI_MW_TUNER_TYPE_TMX7070X = 7,      /* 支持THOMSON7070X高频头 */
    HI_MW_TUNER_TYPE_R820C = 8,         /* 支持R820C高频头 */
    HI_MW_TUNER_TYPE_MXL203 = 9,        /* 支持MXL203高频头 */
    HI_MW_TUNER_TYPE_AV2011 = 10,       /* 支持AV2011高频头 */
    HI_MW_TUNER_TYPE_SHARP7903 = 11,    /* 支持SHARP7903高频头 */
    HI_MW_TUNER_TYPE_MXL101 = 12,       /* 支持MXL101高频头 */
    HI_MW_TUNER_TYPE_MXL603 = 13,       /* 支持MXL603高频头 */
    HI_MW_TUNER_TYPE_IT9170 = 14,       /* 支持IT9170高频头 */
    HI_MW_TUNER_TYPE_IT9133 = 15,       /* 支持IT9133高频头 */
    HI_MW_TUNER_TYPE_TDA6651 = 16,      /* 支持TDA6651高频头 */
    HI_MW_TUNER_TYPE_TDA18250B = 17,    /* 支持TDA18250B高频头 */
    HI_MW_TUNER_TYPE_M88TS2022 = 18,    /* 支持M88TS2022高频头 */
    HI_MW_TUNER_TYPE_RDA5815 = 19,      /* 支持RDA5815高频头 */
    HI_MW_TUNER_TYPE_MXL254 = 20,       /* 支持MXL254高频头 */
    HI_MW_TUNER_TYPE_CXD2861 = 21,      /* 支持CXD2861高频头 */
    HI_MW_TUNER_TYPE_TDA18273 = 22,     /* 支持TDA18273HN高频头 */
    HI_MW_TUNER_TYPE_TDA18275 = 23,     /* 支持TDA18275高频头 */
    HI_MW_TUNER_TYPE_R840 = 24,         /* 支持R840高频头 */
    HI_MW_TUNER_TYPE_MXL661 = 25,       /* 支持MXL661高频头 */
    HI_MW_TUNER_TYPE_MXL601 = 26,       /* 支持MXL601高频头 */
    HI_MW_TUNER_TYPE_TDA18275A = 27,    /* 支持TDA18275A高频头 */
    HI_MW_TUNER_TYPE_SI2151 = 28,       /* 支持si2151高频头 */
    HI_MW_TUNER_TYPE_R842 = 29,         /* 支持R842高频头 */
    HI_MW_TUNER_TYPE_AV2018 = 30,       /* 支持AV2018高频头 */
    HI_MW_TUNER_TYPE_ATBM2040 = 31,     /* 支持ATBM2040高频头 */
    HI_MW_TUNER_TYPE_ATBM253 = 32,      /* 支持ATBM253高频头 */

    HI_MW_TUNER_TYPE_BUTT,              /* 非法边界值 */
};

enum HI_MW_MEMC_POLICY_E {
    HI_MW_MEMC_USER = 1,
    HI_MW_MEMC_4KUI = 2,
};

enum HI_MW_TVD_WORK_MODE_E {
    HI_MW_TVD_WORK_MODE_NORMAL = 0,    /* 正常工作模式 */
    HI_MW_TVD_WORK_MODE_CHAN_SCAN = 1, /* TV搜台模式   */

    HI_MW_TVD_WORK_MODE_BUTT = 2,
};

enum HI_MW_HDMIRX_EDID_TYPE_E {
    HI_MW_HDMIRX_EDID_TYPE_20 = 0,
    HI_MW_HDMIRX_EDID_TYPE_14 = 1,
    HI_MW_HDMIRX_EDID_TYPE_AUTO = 2,
    HI_MW_HDMIRX_EDID_TYPE_21 = 3,

    HI_MW_HDMIRX_EDID_TYPE_BUTT = 4,
};

enum HI_MW_HDMIRX_EDID_CONVERT_TYPE_E {
    HI_MW_HDMIRX_EDID_CONVERT_TYPE_14 = 0,
    HI_MW_HDMIRX_EDID_CONVERT_TYPE_20 = 1,
    HI_MW_HDMIRX_EDID_CONVERT_TYPE_21 = 2,

    HI_MW_HDMIRX_EDID_CONVERT_TYPE_BUTT = 3,
};

enum HI_MW_HDMIRX_EDID_CAP_E {
    HI_MW_HDMIRX_EDID_CAP_14 = 0,
    HI_MW_HDMIRX_EDID_CAP_20 = 1,
    HI_MW_HDMIRX_EDID_CAP_21 = 2,

    HI_MW_HDMIRX_EDID_CAP_MAX = 3,
};

enum HI_MW_HDMIRX_EDID_MODE_E {
    HI_MW_HDMIRX_EDID_MODE_AUTO = 0,
    HI_MW_HDMIRX_EDID_MODE_14 = 1,
    HI_MW_HDMIRX_EDID_MODE_20 = 2,
    HI_MW_HDMIRX_EDID_MODE_21 = 3,

    HI_MW_HDMIRX_EDID_MODE_BUTT = 4,
};

enum HI_MW_TVD_CMD_TYPE_E {
    HI_MW_TVD_CMD_TYPE_Y = 1,
    HI_MW_TVD_CMD_TYPE_CB = 2,
    HI_MW_TVD_CMD_TYPE_CR = 3,

    HI_MW_TVD_CMD_TYPE_BUTT = 4,
};

/* cec menu state */
enum HI_MW_CEC_UI_MENU_E {
    HI_MW_CEC_UI_MENU_ENABLE = 0,
    HI_MW_CEC_UI_MENU_ONETCHPLY = 1,
    HI_MW_CEC_UI_MENU_STANDBY = 2,
    HI_MW_CEC_UI_MENU_DEVMENUCTRL = 3,
    HI_MW_CEC_UI_MENU_AMPLIFIER = 4,
    HI_MW_CEC_UI_MENU_ARC = 5,

    HI_MW_CEC_UI_MENU_BUTT = 6,
};

/* TIF CEC 选项 */
enum HI_MW_TIF_CEC_OPTION {
    HI_MW_TIF_CEC_OPTION_WAKEUP = 0,
    HI_MW_TIF_CEC_OPTION_ENABLE_CEC = 1,
    HI_MW_TIF_CEC_OPTION_SYSTEM_CEC_CONTROL = 2,
    HI_MW_TIF_CEC_OPTION_STANDBY = 3,
    HI_MW_TIF_CEC_OPTION_MENUCTRL = 4,

    HI_MW_TIF_CEC_OPTION_BUTT,
};

enum HI_MW_CEC_DEVICE_PORT_E {
    HI_MW_CEC_DEVICE_PORT_HDMI1 = 0,
    HI_MW_CEC_DEVICE_PORT_HDMI2 = 1,
    HI_MW_CEC_DEVICE_PORT_HDMI3 = 2,

    HI_MW_CEC_DEVICE_PORT_BUTT = 3,
};

enum HI_MW_CEC_DEVICE_type_E {
    HI_MW_CEC_DEVICE_type_tv = 0,
    HI_MW_CEC_DEVICE_type_recorder1 = 1,
    HI_MW_CEC_DEVICE_type_recorder2 = 2,
    HI_MW_CEC_DEVICE_type_tuner1 = 3,
    HI_MW_CEC_DEVICE_type_player1 = 4,
    HI_MW_CEC_DEVICE_type_audiosystem = 5,
    HI_MW_CEC_DEVICE_type_tuner2 = 6,
    HI_MW_CEC_DEVICE_type_tuner3 = 7,
    HI_MW_CEC_DEVICE_type_player2 = 8,
    HI_MW_CEC_DEVICE_type_recorder3 = 9,
    HI_MW_CEC_DEVICE_type_tuner4 = 10,
    HI_MW_CEC_DEVICE_type_player3 = 11,
    HI_MW_CEC_DEVICE_type_reserved = 13,
    HI_MW_CEC_DEVICE_type_specficuse = 14,

    HI_MW_CEC_DEVICE_type_BUTT = 15,
};

enum HI_MW_PQ_HISTGRAM_SITE_E {
    HI_MW_PQ_HISTGRAM_VIDEO = 0,
    HI_MW_PQ_HISTGRAM_VIDEO_OSD = 1,

    HI_MW_PQ_HISTGRAM_BUTT = 2,
};

/* country or region definition , must keep consistent with EnumSystemCountry */
enum HI_MW_COUNTRY_E {
    HI_MW_COUNTRY_CHINA = 0,
    HI_MW_COUNTRY_HONGKONG_CHINA = 1,
    HI_MW_COUNTRY_TAIWAN_CHINA = 2,
    HI_MW_COUNTRY_INDONESIA = 3,
    HI_MW_COUNTRY_MALAYSIA = 4,
    HI_MW_COUNTRY_SINGAPORE = 5,
    HI_MW_COUNTRY_THAILAND = 6,
    HI_MW_COUNTRY_VIETNAM = 7,
    HI_MW_COUNTRY_INDIA = 8,
    HI_MW_COUNTRY_USA = 9,
    HI_MW_COUNTRY_BRAZIL = 10,  // Brazil is btsc
    HI_MW_COUNTRY_AUSTRIA = 11,
    HI_MW_COUNTRY_PHILIPPINES = 12,  // philippines is btsc
    HI_MW_COUNTRY_CANADA = 13,
    HI_MW_COUNTRY_MEXICO = 14,
    HI_MW_COUNTRY_CZECH = 15,
    HI_MW_COUNTRY_DENMARK = 16,
    HI_MW_COUNTRY_FINLAND = 17,
    HI_MW_COUNTRY_FRANCE = 18,
    HI_MW_COUNTRY_GERMANY = 19,
    HI_MW_COUNTRY_GREECE = 20,
    HI_MW_COUNTRY_HUNGARY = 21,
    HI_MW_COUNTRY_ITALY = 22,
    HI_MW_COUNTRY_LUXEMBOURG = 23,
    HI_MW_COUNTRY_NETHERLANDS = 24,
    HI_MW_COUNTRY_NORWAY = 25,
    HI_MW_COUNTRY_POLAND = 26,
    HI_MW_COUNTRY_PORTUGAL = 27,
    HI_MW_COUNTRY_SPAIN = 28,
    HI_MW_COUNTRY_SWEDEN = 29,
    HI_MW_COUNTRY_SWITZERLAND = 30,
    HI_MW_COUNTRY_SWITZERLAND_FRA = 31,  // use this table if menu language is french
    HI_MW_COUNTRY_SWITZERLAND_ITA = 32,  // use this table if menu language is italian
    HI_MW_COUNTRY_JAPAN = 33,
    HI_MW_COUNTRY_UNITED_KINGDOM = 34,
    HI_MW_COUNTRY_KOREA = 35,  // add new country or region enum following
    HI_MW_COUNTRY_ARGENTINA = 36,
    HI_MW_COUNTRY_CHILE = 37,
    HI_MW_COUNTRY_PERU = 38,
    HI_MW_COUNTRY_URUGUAY = 39,
    HI_MW_COUNTRY_MYANMAR = 40,
    HI_MW_COUNTRY_RUSSIA = 41,
    HI_MW_COUNTRY_BELGIUM = 42,
    HI_MW_COUNTRY_TURKEY = 43,
    HI_MW_COUNTRY_SERBIA = 44,
    HI_MW_COUNTRY_CROATIA = 45,
    HI_MW_COUNTRY_ROMANIA = 46,
    HI_MW_COUNTRY_COLOMBIA = 47,
    HI_MW_COUNTRY_NEWZEALAND = 48,
    HI_MW_COUNTRY_AUSTRALIA = 49,

    HI_MW_COUNTRY_BUTT = 50,
};
/* tvSystem definition , must keep consistent with EnumSystemTvSystem  */
enum HI_MW_TVSYSTEM_E {
    HI_MW_TVSYSTEM_ATV = 0,
    HI_MW_TVSYSTEM_DTMB_DVBC = 1,
    HI_MW_TVSYSTEM_ATSC = 2,
    HI_MW_TVSYSTEM_ISDBT = 3,
    HI_MW_TVSYSTEM_DVBT_T2_C = 4,
    HI_MW_TVSYSTEM_DVBT_T2_S_S2_C = 5,

    HI_MW_TVSYSTEM_BUTT = 6,
};

enum HI_MW_CHANNELMANTYPE_E {
    HI_MW_CHANNELMANTYPE_SCAN = 0, /* 频道管理类型:0:按搜台顺序存台, 1:自动排序, 2:按频道表存台 */
    HI_MW_CHANNELMANTYPE_AUTO = 1,
    HI_MW_CHANNELMANTYPE_CHALIST = 2,

    HI_MW_CHANNELMANTYPE_BUTT = 3,
};

enum HI_MW_VCHIP_SYSTEM_E {
    HI_MW_VCHIP_SYSTEM_MPA = 0,     /* US Motion Picture Association */
    HI_MW_VCHIP_SYSTEM_USTV = 1,    /* US TV parental guideline */
    HI_MW_VCHIP_SYSTEM_CAN_ENG = 2, /* Canadian english language rating */
    HI_MW_VCHIP_SYSTEM_CAN_FRE = 3, /* Canadian french language rating  */

    HI_MW_VCHIP_SYSTEM_BUTT = 4,
};

enum HI_MW_VCHIP_MPA_RATING_E {
    HI_MW_VCHIP_MPA_RATING_NA = 0,
    HI_MW_VCHIP_MPA_RATING_G = 1,
    HI_MW_VCHIP_MPA_RATING_PG = 2,
    HI_MW_VCHIP_MPA_RATING_PG13 = 3,
    HI_MW_VCHIP_MPA_RATING_R = 4,
    HI_MW_VCHIP_MPA_RATING_NC17 = 5,
    HI_MW_VCHIP_MPA_RATING_X = 6,
    HI_MW_VCHIP_MPA_RATING_NOTRATED = 7,

    HI_MW_VCHIP_MPA_RATING_BUTT = 8,
};

/* * Define the chip type.  */
enum HI_MW_CHIP_TYPE_E {
    HI_MW_CHIP_TYPE_HI3716M,
    HI_MW_CHIP_TYPE_HI3716H,
    HI_MW_CHIP_TYPE_HI3716C,
    HI_MW_CHIP_TYPE_HI3716CES,

    HI_MW_CHIP_TYPE_HI3720,
    HI_MW_CHIP_TYPE_HI3712,
    HI_MW_CHIP_TYPE_HI3715,

    HI_MW_CHIP_TYPE_HI3718M,
    HI_MW_CHIP_TYPE_HI3718C,
    HI_MW_CHIP_TYPE_HI3719M,
    HI_MW_CHIP_TYPE_HI3719C,
    HI_MW_CHIP_TYPE_HI3719M_A,
    HI_MW_CHIP_TYPE_HI3796C = 0x20,
    HI_MW_CHIP_TYPE_HI3798C,
    HI_MW_CHIP_TYPE_HI3796M,
    HI_MW_CHIP_TYPE_HI3798M,

    HI_MW_CHIP_TYPE_HI3796C_A = 0x40,
    HI_MW_CHIP_TYPE_HI3798C_A,

    HI_MW_CHIP_TYPE_HI3798M_A,
    HI_MW_CHIP_TYPE_HI3751,

    HI_MW_CHIP_TYPE_BUTT
};

/* * Define the chip version.  */
enum HI_MW_CHIP_VERSION_E {
    HI_MW_CHIP_VERSION_V100 = 0x100,
    HI_MW_CHIP_VERSION_V101 = 0x101,
    HI_MW_CHIP_VERSION_V200 = 0x200,
    HI_MW_CHIP_VERSION_V300 = 0x300,
    HI_MW_CHIP_VERSION_V350 = 0x350,
    HI_MW_CHIP_VERSION_V351 = 0x351,
    HI_MW_CHIP_VERSION_V550 = 0x550,
    HI_MW_CHIP_VERSION_V551 = 0x551,
    HI_MW_CHIP_VERSION_V620 = 0x620,
    HI_MW_CHIP_VERSION_V810 = 0x810,
    HI_MW_CHIP_VERSION_V811 = 0x811,

    HI_MW_CHIP_VERSION_BUTT,
};

enum HI_MW_CHIP_CAPABILITY_E {
    HI_MW_CHIP_CAP_DOLBY = 0,
    HI_MW_CHIP_CAP_DTS = 1,  // SRS, yes, this is SRS
    HI_MW_CHIP_CAP_ADVCA = 2,
    HI_MW_CHIP_CAP_MACROVISION = 3,
    HI_MW_CHIP_CAP_DD = 4,
    HI_MW_CHIP_CAP_DBX = 5,  // DBX和DBX2共用一个CHIP
    HI_MW_CHIP_CAP_DAP = 6,

    HI_MW_CHIP_CAP_BUTT = 7,
};

enum HI_MW_VCHIP_USTV_RATING_E {
    HI_MW_VCHIP_USTV_RATING_NONE = 0,
    HI_MW_VCHIP_USTV_RATING_TVY = 1,
    HI_MW_VCHIP_USTV_RATING_TVY7 = 2,
    HI_MW_VCHIP_USTV_RATING_TVG = 3,
    HI_MW_VCHIP_USTV_RATING_TVPG = 4,
    HI_MW_VCHIP_USTV_RATING_TV14 = 5,
    HI_MW_VCHIP_USTV_RATING_NONE1 = 6,
    HI_MW_VCHIP_USTV_RATING_TVMA = 7,

    HI_MW_VCHIP_USTV_RATING_BUTT = 8,
};

enum HI_MW_VCHIP_CAN_ENG_RATING_E {
    HI_MW_VCHIP_CAN_ENG_RATING_Ex = 0,
    HI_MW_VCHIP_CAN_ENG_RATING_C = 1,
    HI_MW_VCHIP_CAN_ENG_RATING_C8 = 2,
    HI_MW_VCHIP_CAN_ENG_RATING_G = 3,
    HI_MW_VCHIP_CAN_ENG_RATING_PG = 4,
    HI_MW_VCHIP_CAN_ENG_RATING_14 = 5,
    HI_MW_VICHP_CAN_ENG_RATING_18 = 6,

    HI_MW_VCHIP_CAN_ENG_RATING_BUTT = 7,
};

enum HI_MW_VCHIP_CAN_FRE_RATING_E {
    HI_MW_VCHIP_CAN_FRE_RATING_Ex = 0,
    HI_MW_VCHIP_CAN_FRE_RATING_G = 1,
    HI_MW_VCHIP_CAN_FRE_RATING_8ANS = 2,
    HI_MW_VCHIP_CAN_FRE_RATING_13ANS = 3,
    HI_MW_VCHIP_CAN_FRE_RATING_16ANS = 4,
    HI_MW_VCHIP_CAN_FRE_RATING_18ANS = 5,
    HI_MW_VCHIP_CAN_FRE_RATING_18 = 6,

    HI_MW_VCHIP_CAN_FRE_RATING_BUTT = 7,
};

enum HI_MW_VCHIP_USTV_CATEGORY_E {
    HI_MW_VCHIP_USTV_CATEGORY_ALL = 0,
    HI_MW_VCHIP_USTV_CATEGORY_FV = 1,
    HI_MW_VCHIP_USTV_CATEGORY_V = 2,
    HI_MW_VCHIP_USTV_CATEGORY_S = 3,
    HI_MW_VCHIP_USTV_CATEGORY_L = 4,
    HI_MW_VCHIP_USTV_CATEGORY_D = 5,

    HI_MW_VCHIP_USTV_CATEGORY_BUTT = 6,
};
enum HI_MW_USTV_CATEGORY_MODE_E {
    HI_MW_USTV_CATEGORY_UNLIMIT = 0, /* not control    */
    HI_MW_USTV_CATEGORY_LIMIT = 1,   /* limit category */
    HI_MW_USTV_CATEGORY_NONE = 2,    /* Not involved   */

    HI_MW_USTV_CATEGORY_BUTT = 3,
};

enum HI_MW_CAPTURE_TYPE_E {
    HI_MW_CAPTURE_TYPE_VIDEO = 0,
    HI_MW_CAPTURE_TYPE_GRAPHIC = 1,
    HI_MW_CAPTURE_TYPE_VIDEO_GRAPHIC = 2,

    HI_MW_CAPTURE_TYPE_BUTT = 3,
};

enum HI_MW_TUNER_STEP_E {
    HI_MW_TUNER_STEP_50KHZ = 0,
    HI_MW_TUNER_STEP_62KHZ5 = 1,
    HI_MW_TUNER_STEP_31KHZ25 = 2,

    HI_MW_TUNER_STEP_BUTT = 3,
};

enum HI_MW_HDMIRX_RCP_KEY_E {
    HI_MW_HDMIRX_RCP_KEY_SELECT = 0,
    HI_MW_HDMIRX_RCP_KEY_UP = 1,
    HI_MW_HDMIRX_RCP_KEY_DOWN = 2,
    HI_MW_HDMIRX_RCP_KEY_LEFT = 3,
    HI_MW_HDMIRX_RCP_KEY_RIGHT = 4,
    HI_MW_HDMIRX_RCP_KEY_ROOT_MENU = 5,
    HI_MW_HDMIRX_RCP_KEY_EXIT = 6,
    HI_MW_HDMIRX_RCP_KEY_PLAY = 7,
    HI_MW_HDMIRX_RCP_KEY_STOP = 8,
    HI_MW_HDMIRX_RCP_KEY_PAUSE = 9,
    HI_MW_HDMIRX_RCP_KEY_REWIND = 10,
    HI_MW_HDMIRX_RCP_KEY_FAST_FWD = 11,

    HI_MW_HDMIRX_RCP_KEY_BUTT = 12,
};

enum HI_MW_EDID_AUDIO_FORMAT_E {
    HI_MW_EDID_AUDIO_FORMAT_CODE_RESERVED = 0,   /* Audio coding type, refer stream,default type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_PCM = 1,        /* Audio coding PCM type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_AC3 = 2,        /* Audio coding AC3 type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_MPEG1 = 3,      /* Audio coding MPEG1 type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_MP3 = 4,        /* Audio coding MP3 type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_MPEG2 = 5,      /* Audio coding MPEG2 type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_AAC = 6,        /* Audio coding AAC type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_DTS = 7,        /* Audio coding DTS type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_ATRAC = 8,      /* Audio coding ATRAC type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_ONE_BIT = 9,    /* Audio coding ONE BIT AUDIO type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_DDP = 10,       /* Audio coding DDPLUS type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_DTS_HD = 11,    /* Audio coding DTS HD type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_MAT = 12,       /* Audio coding MAT type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_DST = 13,       /* Audio coding DST type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_WMA_PRO = 14,   /* Audio coding WMA PRO type  */
    HI_MW_EDID_AUDIO_FORMAT_CODE_DDP_ATMOS = 15, /* Audio coding DDPLUS ATMOS type  */

    HI_MW_EDID_AUDIO_FORMAT_CODE_BUTT = 16,
};

enum HI_MW_CODE_FRAME_E {
    HI_MW_CODE_FRAME_16_9 = 0,
    HI_MW_CODE_FRAME_4_3 = 1,

    HI_MW_CODE_FRAME_BUTT = 2,
};

/* Color Section */ /* CNcomment/:FIXMODE :颜色分段类型 */
enum HI_MW_PQ_COLOR_E {
    HI_MW_PQ_COLOR_R = 0,
    HI_MW_PQ_COLOR_G = 1,
    HI_MW_PQ_COLOR_B = 2,
    HI_MW_PQ_COLOR_CYAN = 3,
    HI_MW_PQ_COLOR_PURPLE = 4,
    HI_MW_PQ_COLOR_YELLOW = 5,
    HI_MW_PQ_COLOR_FLESH = 6,
    HI_MW_PQ_COLOR_HAIR = 7,
    HI_MW_PQ_COLOR_OLIVINE = 8,

    HI_MW_PQ_COLOR_BUTT = 9,
};

/* NR str type Y/C */
enum HI_MW_PQ_NRSTR_E {
    HI_MW_PQ_NRSTR_Y = 0,
    HI_MW_PQ_NRSTR_C = 1,

    HI_MW_PQ_NRSTR_BUTT = 2,
};
enum HI_MW_COLOR_PQ_E {
    HI_MW_COLOR_PQ_PAL = 0,
    HI_MW_COLOR_PQ_NTSC = 1,
    HI_MW_COLOR_PQ_SECAM = 2,

    HI_MW_COLOR_PQ_BUTT = 3,
};

/* * HDR TYPE info  */ /* CNcomment/: HDR 类型信息  */
enum HI_MW_PQ_HDR_TYPE_E {
    HI_MW_PQ_HDR_TYPE_SDR = 0,

    HI_MW_PQ_HDR_TYPE_SDR_BT2020_NONCONSTLUM = 1,
    HI_MW_PQ_HDR_TYPE_SDR_BT2020_CONSTLUM = 2,
    HI_MW_PQ_HDR_TYPE_SDR_XVYCC = 3,

    HI_MW_PQ_HDR_TYPE_HDR10 = 4,
    HI_MW_PQ_HDR_TYPE_HDR10PLUS = 5,

    HI_MW_PQ_HDR_TYPE_SLF = 6,
    HI_MW_PQ_HDR_TYPE_HLG = 7,
    HI_MW_PQ_HDR_TYPE_TECHNICOLOR = 8,
    HI_MW_PQ_HDR_TYPE_DOLBYVISION = 9,

    HI_MW_PQ_HDR_TYPE_BUTT = 10,
};

enum HI_MW_PQ_MPLUS_MODE_E {
    HI_MW_PQ_MPLUS_MODE_LOW_POWER = 0,
    HI_MW_PQ_MPLUS_MODE_HIGH_LUMINANCE_I = 1,
    HI_MW_PQ_MPLUS_MODE_HIGH_LUMINANCE_II = 2,
    HI_MW_PQ_MPLUS_MODE_HDR = 3,

    HI_MW_PQ_MPLUS_MODE_BUTT = 4,
};

enum HI_MW_PQ_LAYER_E {
    HI_MW_PQ_LAYER_ID_V0 = 0,     /* < vo layer  */    /* <CNcomment: 视频层v0 */
    HI_MW_PQ_LAYER_ID_V1 = 1,     /* < v1 layer  */    /* <CNcomment: 视频层v1 */
    HI_MW_PQ_LAYER_ID_GP = 2,     /* < gp layer  */    /* <CNcomment: 图形层gp */

    HI_MW_PQ_LAYER_ID_BUTT = 3    /* <Invalid value */ /* <CNcomment:非法边界值 */
};

/* ****************SRS data struct type define begin***************** */
enum HI_MW_SRS_MAINTECH_E {
    HI_MW_SRS_MAINTECH_NONE = 0,
    HI_MW_SRS_MAINTECH_WOWHD = 1,
    HI_MW_SRS_MAINTECH_TSHD = 2,
    HI_MW_SRS_MAINTECH_CC3D = 3,
    HI_MW_SRS_MAINTECH_CSHP = 4,
    HI_MW_SRS_MAINTECH_TBHD = 5,
    HI_MW_SRS_MAINTECH_NUMS = 6,

    HI_MW_SRS_MAINTECH_BUTT = 7,
};

enum HI_MW_SRS_SPACEHPF_FREQ_E {
    HI_MW_SRS_SPACEHPF_FREQ_80HZ = 0,
    HI_MW_SRS_SPACEHPF_FREQ_120HZ = 1,
    HI_MW_SRS_SPACEHPF_FREQ_200HZ = 2,
    HI_MW_SRS_SPACEHPF_FREQ_240HZ = 3,
    HI_MW_SRS_SPACEHPF_FREQ_300HZ = 4,
    HI_MW_SRS_SPACEHPF_FREQ_400HZ = 5,
    HI_MW_SRS_SPACEHPF_FREQ_500HZ = 6,
    HI_MW_SRS_SPACEHPF_FREQ_600HZ = 7,
    HI_MW_SRS_SPACEHPF_FREQ_800HZ = 8,
    HI_MW_SRS_SPACEHPF_FREQ_CUSTOM = 9,
    HI_MW_SRS_SPACEHPF_FREQ_SIZES = 10,

    HI_MW_SRS_SPACEHPF_FREQ_BUTT = 11,
};

enum HI_MW_SRS_CSHP_MODE_E {
    HI_MW_SRS_CSHP_PAS_DECODER = 0,
    HI_MW_SRS_CSHP_CAS_DECODER = 1,

    HI_MW_SRS_CSHP_REND_BUTT = 2,
};

enum HI_MW_SRS_CSD_MODE_E {
    HI_MW_SRS_CSDPROC_CINEMA = 1,
    HI_MW_SRS_CSDPROC_PRO = 2,
    HI_MW_SRS_CSDPROC_MUSIC = 4,
    HI_MW_SRS_CSDPROC_MONO = 8,
    HI_MW_SRS_CSDPROC_LCRS = 16,

    HI_MW_SRS_CSDPROC_BUTT = 17,
};

enum HI_MW_SRS_TBHD_SPKLFP_E {
    HI_MW_SRS_TBHD_SPK_LFP_40HZ = 0,
    HI_MW_SRS_TBHD_SPK_LFP_60HZ = 1,
    HI_MW_SRS_TBHD_SPK_LFP_100HZ = 2,
    HI_MW_SRS_TBHD_SPK_LFP_120HZ = 3,
    HI_MW_SRS_TBHD_SPK_LFP_150HZ = 4,
    HI_MW_SRS_TBHD_SPK_LFP_200HZ = 5,
    HI_MW_SRS_TBHD_SPK_LFP_250HZ = 6,
    HI_MW_SRS_TBHD_SPK_LFP_300HZ = 7,
    HI_MW_SRS_TBHD_SPK_LFP_400HZ = 8,
    HI_MW_SRS_TBHD_SPK_LFP_CUSTOM = 9,
    HI_MW_SRS_TBHD_SPK_LFP_SIZES = 10,

    HI_MW_SRS_TBHD_SPK_LFP_BUTT = 11,
};

enum HI_MW_SRS_TBHD_MODE_E {
    HI_MW_SRS_TBHD_MONO = 0,
    HI_MW_SRS_TBHD_STEREO = 1,
    HI_MW_SRS_TBHD_NUM = 2,

    HI_MW_SRS_TBHD_BUTT = 3,
};

/* AQ工具兼容性，SWS不删除 */
enum HI_MW_AEF_TYPE_E {
    HI_MW_AEF_TYPE_OFF = 0,   /* 所有音效关闭 */
    HI_MW_AEF_TYPE_SWS = 1,   /* 自研SWS音效类型 */
    HI_MW_AEF_TYPE_SRS = 2,   /* 付费SRS音效类型 */
    HI_MW_AEF_TYPE_DBX = 3,   /* 付费DBX音效类型 */
    HI_MW_AEF_TYPE_SWS3D = 4, /* 自研SWS3D 音效类型 */
    HI_MW_AEF_TYPE_DBX2 = 5,  /* 付费DBX2音效类型 */
    HI_MW_AEF_TYPE_DAP = 6,   /* 付费DAP音效类型 */

    HI_MW_AEF_TYPE_BUTT = 7, /* 无效类型 */
};

/* audio track mode */
enum HI_MW_TRACK_MODE_E {
    HI_MW_TRACK_MODE_STEREO = 0,
    HI_MW_TRACK_MODE_DOUBLE_MONO = 1,
    HI_MW_TRACK_MODE_DOUBLE_LEFT = 2,
    HI_MW_TRACK_MODE_DOUBLE_RIGHT = 3,
    HI_MW_TRACK_MODE_EXCHANGE = 4,
    HI_MW_TRACK_MODE_ONLY_RIGHT = 5,
    HI_MW_TRACK_MODE_ONLY_LEFT = 6,
    HI_MW_TRACK_MODE_MUTED = 7,

    HI_MW_TRACK_MODE_BUTT = 8,
};

enum HI_MW_TRACK_SOURCE_E {
    HI_MW_TRACK_SOURCE_ALSA = 0,
    HI_MW_TRACK_SOURCE_ATV = 1,
    HI_MW_TRACK_SOURCE_AV = 2,
    HI_MW_TRACK_SOURCE_VGA = 3,
    HI_MW_TRACK_SOURCE_YPBPR = 4,
    HI_MW_TRACK_SOURCE_HDMI = 5,
    HI_MW_TRACK_SOURCE_DTV = 6,
    HI_MW_TRACK_SOURCE_MEDIA = 7,
    HI_MW_TRACK_SOURCE_SCART = 8,

    HI_MW_TRACK_SOURCE_BUTT = 9,
};

/* *Audio Track Type: Master, Slave, Virtual channel,LowLatency channel */
/* CNcomment/:音频Track类型:主通道 辅通道 虚拟通道 低延时通道 */
/* *LowLatency channel not support attach avplay and the data only support 48K,stereo,16Bit.
Only support one lowlatency track in a sound */
/* CNcomment/:低延时通道不支持AVPLAY绑定，此通道只支持采样率为48K,双声道,16位位宽的音频数据,仅支持创建一个低延时通道 */
enum HI_MW_SND_TRACK_TYPE_E {
    HI_MW_SND_TRACK_TYPE_MASTER = 0,
    HI_MW_SND_TRACK_TYPE_SLAVE = 1,
    HI_MW_SND_TRACK_TYPE_VIRTUAL = 2,
    HI_MW_SND_TRACK_TYPE_LOWLATENCY = 3,
    HI_MW_SND_TRACK_TYPE_OTTSOUND = 4,    /* *< Ott sound(PCM) MS12 ONLY. */
    HI_MW_SND_TRACK_TYPE_SYSTEMAUDIO = 5, /* *< System sound(PCM) MS12 ONLY. */
    HI_MW_SND_TRACK_TYPE_APPAUDIO = 6,    /* *< App audio(PCM) MS12 ONLY. */

    HI_MW_SND_TRACK_TYPE_BUTT = 7,
};

enum HI_MW_DBX_TOTSON_E {
    HI_MW_DBX_TOTSON_ON = 0,
    HI_MW_DBX_TOTSON_ON2 = 1,
    HI_MW_DBX_TOTSON_ON3 = 2,
    HI_MW_DBX_TOTSON_ON4 = 3,
    HI_MW_DBX_TOTSON_OFF = 4,

    HI_MW_DBX_TOTSON_BUTT = 5,
};

enum HI_MW_DBX_TOTVOL_E {
    HI_MW_DBX_TOTVOL_NORMAL = 0,
    HI_MW_DBX_TOTVOL_NIGHT = 1,
    HI_MW_DBX_TOTVOL_OFF = 2,

    HI_MW_DBX_TOTVOL_BUTT = 3,
};

enum HI_MW_DBX_TOTSUR_E {
    HI_MW_DBX_TOTSUR_ON = 0,
    HI_MW_DBX_TOTSUR_ON2 = 1,
    HI_MW_DBX_TOTSUR_OFF = 2,

    HI_MW_DBX_TOTSUR_BUTT = 3,
};

enum HI_MW_DBX2_TOTSON_E {
    HI_MW_DBX2_SOUND_MODE_STANDARD = 0,
    HI_MW_DBX2_SOUND_MODE_MUSIC = 1,
    HI_MW_DBX2_SOUND_MODE_MOVIE = 2,
    HI_MW_DBX2_SOUND_MODE_SPEECH = 3,
    HI_MW_DBX2_SOUND_MODE_NIGHT = 4,
    HI_MW_DBX2_SOUND_MODE_SPORTS = 5,
    HI_MW_DBX2_SOUND_MODE_CONCERT = 6,
    HI_MW_DBX2_SOUND_MODE_USER1 = 7,
    HI_MW_DBX2_SOUND_MODE_USER2 = 8,
    HI_MW_DBX2_SOUND_MODE_USER3 = 9,
    HI_MW_DBX2_SOUND_MODE_OFF = 10,

    HI_MW_DBX2_SOUND_MODE_BUTT = 11,
};

enum HI_MW_DBX2_TOTVOL_E {
    HI_MW_DBX2_TOTVOL_NORMAL = 0,
    HI_MW_DBX2_TOTVOL_NIGHT = 1,
    HI_MW_DBX2_TOTVOL_OFF = 2,

    HI_MW_DBX2_TOTVOL_BUTT = 3,
};

enum HI_MW_DBX2_TOTSUR_E {
    HI_MW_DBX2_TOTSUR_ON = 0,
    HI_MW_DBX2_TOTSUR_ON2 = 1,
    HI_MW_DBX2_TOTSUR_OFF = 2,

    HI_MW_DBX2_TOTSUR_BUTT = 3,
};

enum HI_MW_DAP_SOUND_MODE_E {
    HI_MW_DAP_SOUND_MODE_AUTO = 0,
    HI_MW_DAP_SOUND_MODE_MOVIE = 1,
    HI_MW_DAP_SOUND_MODE_MUSIC = 2,
    HI_MW_DAP_SOUND_MODE_GAME = 3,
    HI_MW_DAP_SOUND_MODE_NEWS = 4,
    HI_MW_DAP_SOUND_MODE_NIGHT = 5,
    HI_MW_DAP_SOUND_MODE_VOICE = 6,
    HI_MW_DAP_SOUND_MODE_USER1 = 7,
    HI_MW_DAP_SOUND_MODE_USER2 = 8,
    HI_MW_DAP_SOUND_MODE_OFF = 9,

    HI_MW_DAP_SOUND_MODE_BUTT = 10,
};

enum HI_MW_DAP_DIALOG_ENHANCER_E {
    HI_MW_DAP_DIALOG_ENHANCER_OFF = 0,
    HI_MW_DAP_DIALOG_ENHANCER_LOW = 1,
    HI_MW_DAP_DIALOG_ENHANCER_MID = 2,
    HI_MW_DAP_DIALOG_ENHANCER_HIGH = 3,

    HI_MW_DAP_DIALOG_ENHANCER_BUTT = 4,
};

enum HI_MW_UART_VGADDC_SWITCH_E {
    HI_MW_SWITCH_UART_0 = 0,  // 串口0 为系统串口
    HI_MW_SWITCH_UART_1 = 1,  // 串口1 为Mdebug ,PQ工具用的串口
    HI_MW_SWITCH_STB_GPIOPWM = 2,
    HI_MW_SWITCH_VGA_DDC = 3,  // VGA DDC用于传输EDID

    HI_MW_SWITCH_BUTT = 4,
};

enum HI_MW_SCART_RESOLUTION_E {
    HI_MW_SCART_NTSC = 0,
    HI_MW_SCART_PAL = 1,
    HI_MW_SCART_SECAM = 2,
    HI_MW_SCART_NTSC_443 = 3,
    HI_MW_SCART_PAL_M = 4,
    HI_MW_SCART_PAL_NC = 5,
    HI_MW_SCART_480i_60 = 6,
    HI_MW_SCART_480p_60 = 7,
    HI_MW_SCART_576i_50 = 8,
    HI_MW_SCART_576p_50 = 9,

    HI_MW_SCART_BUTT = 10,
};

struct HI_MW_SYS_VERSION_S {
    HI_MW_CHIP_TYPE_E enChipTypeSoft;     /* *<Chip type corresponding to the SDK */
    HI_MW_CHIP_TYPE_E enChipTypeHardWare; /* *<Chip type that is detected when the SDK is running */
    HI_MW_CHIP_VERSION_E enChipVersion;   /* *<Chip version that is detected when the SDK is running */
    HI_CHAR u8Version[80];                /* *<Version string of the SDK */ /* 80 is array size */
    HI_CHAR u8BootVersion[80];            /* *<Version string of the Boot */ /* 80 is array size */
};

enum HI_MW_HDMITX_BOOT_MODE_E {
    HI_MW_HDMITX_BOOT_MODE_OFF = 0,
    /* HDMITX off */ /* HDMITX <B2><BB><CA><E4><B3><F6> */
    HI_MW_HDMITX_BOOT_MODE_MAIN = 1,
    /* only HDMITX output */ /* HDMITX <D7><F7>?<D6><F7>?<A1><A4><CA><E4><B3><F6><A3><AC>?<D3><D0>HDMITX */
    HI_MW_HDMITX_BOOT_MODE_AUX = 2,
    /* VBO and HDMITX output */
    /* HDMITX <D7><F7>?<B8><A8><D6><FA>?<A1><A4><CA><E4><B3><F6><A3><AC><D3><D0>VBO<BA><CD>HDMITX */
    HI_MW_HDMITX_BOOT_MODE_BUTT,
};

/* color space */ /* <D1><D5>?<BF>?<E4> */
enum HI_MW_HDMITX_COLORSPACE_E {
    HI_MW_HDMITX_COLORSPACE_RGB444 = 0,
    HI_MW_HDMITX_COLORSPACE_YUV422 = 1,
    HI_MW_HDMITX_COLORSPACE_YUV444 = 2,
    HI_MW_HDMITX_COLORSPACE_YUV420 = 3,
};

enum HI_MW_POWER_SAVE_MODE_E {
    HI_MW_POWER_SAVE_MODE_NORMAL = 0,
    HI_MW_POWER_SAVE_MODE_POWERSAVE = 1,

    HI_MW_POWER_SAVE_MODE_BUTT
};

/* *define video channel delay struct  */
/* 视频通路延时结构体 */
typedef struct hiMW_DISP_DELAY_S {
    HI_U32 u32PanelMemcDelay;
} HI_MW_DISP_DELAY_S;

struct HI_MW_VIDEO_HDR_S {
    HI_BOOL bValid;
    HI_MW_PQ_COLOR_SPACE_E enColorSpace;
    HI_MW_PQ_GAMMA_TYPE_E enGammaType;
};

/* *Defines the types of the video frame Private information. */
/* CNcomment/: 定义视频帧私有信息类型 */
/* sizeof(HI_MW_VIDEO_PRIVATE_INFO_S) 等于 64*4 */
struct HI_MW_VIDEO_PRIVATE_INFO_S {
    HI_BOOL bSecure;                  /* Secure frame flag */
    HI_MW_COLOR_SPACE_E enColorSpace; /* Video frame color space */
    HI_U32 u32CommReserved[22];       /* Only for common use, the size of common use is 22*4 */ /* 22 is array size */
    HI_U32 u32PDT1Reserved[20];       /* Only for STB use, the size of STB use is 20*4 */ /* 20 is array size */
    HI_U32 u32PDT2Reserved[20];       /* Only for DPT use, the size of DPT use is 20*4 */ /* 20 is array size */
    HI_U32 u32ImageId;
};

struct HI_MW_VO_TIMING_INFO_S {
    HI_MW_VO_SOURCE_E enSource;   // DTV Default HI_UNF_SOURCE_DTV
    HI_U32 u32Width;              // source width
    HI_U32 u32Height;             // source height
    HI_U32 u32FrmRate;            // source framerate
    HI_MW_FRAME_TYPE_E en3dType;  // source 3D Type
    HI_MW_COLOR_SPACE_E enSrcColorSpace;
    HI_MW_COLOR_SYS_E enColorSys;
    HI_BOOL bGraphicMode;  // DTV default HI_FALSE
    HI_BOOL bInterlace;    // source is Interlace or Progress
};

struct HI_MW_CALIBRATION_INFO_S {
    HI_U32 u32RedGain;
    HI_U32 u32GreenGain;
    HI_U32 u32BlueGain;
    HI_U32 u32RedOffset;
    HI_U32 u32GreenOffset;
    HI_U32 u32BlueOffset;
};

struct HI_MW_CALIBRATION_SCOP_S {
    HI_U32 u32GainMin;
    HI_U32 u32GainMax;
    HI_U32 u32OffsetMin;
    HI_U32 u32OffsetMax;
};

struct HI_MW_COLOR_TEMP_DATA_S {
    HI_U32 u32RedGain;
    HI_U32 u32GreenGain;
    HI_U32 u32BlueGain;
    HI_U32 u32RedOffset;
    HI_U32 u32GreenOffset;
    HI_U32 u32BlueOffset;
};

struct HI_MW_AUTO_COLOR_TEMP_DATA_S {
    HI_MW_COLOR_TEMP_DATA_S stColorTemp[HI_MW_COLORTEMP_BUTT];
};

/* 3D 视点、景深 调整范围 */
struct HI_MW_RANGE_S {
    HI_U32 min; /* 下限 */
    HI_U32 max; /* 上限 */
};

/* 屏显示 timming */
struct HI_MW_TIMING_SCREEN_S {
    HI_U32 u32Width;                       /* 宽 */
    HI_U32 u32Height;                      /* 高 */
    HI_U32 u32FrameRate;                   /* 帧率 */
    HI_BOOL bInterlace;                    /* 逐隔行 */
    HI_MW_HDMI_FORMAT_E enFormatHDMI;      /* HDMI 格式 */
    HI_MW_FRAME_PACKING_TYPE_E enFormat3d; /* 3D 格式 */
    HI_MW_COLOR_SPACE_E enColorSpace;      /* 色彩空间 */
    HI_BOOL bMHL;                          /* 获取MHL Timing 信息 */
};

struct HI_MW_TIMING_PARAM_S {
    struct {
        HI_U32 u32Width;     /* 宽 */
        HI_U32 u32Height;    /* 高 */
        HI_U32 u32FrameRate; /* 帧率 */
        HI_BOOL bInterlace;  /* 逐隔行 */
    } timingInfo;

    HI_MW_HD_RESOLUTION_E resolution; /* resolution */
};

struct HI_MW_CROP_RECT_S {
    HI_U32 u32LeftOffset;
    HI_U32 u32TopOffset;
    HI_U32 u32RightOffset;
    HI_U32 u32BottomOffset;
};

/* 节目属性 */
struct HI_MW_ATV_CHANNEL_ATTR_S {
    HI_S32 volumeOffset;
    HI_U8 audioSys;
    HI_U8 mtsMode;          /* mst mode */
    HI_U8 colorSys;         /* 用户设置的色彩制式 */
    HI_U8 colorSysOriginal; /* 搜索后的色彩制式，没有auto制式 */
    HI_U8 aft : 1;
    HI_U8 skip : 1;
    HI_U8 hide : 1; /* hide program */
    HI_U8 lock : 1; /* lock program */
    HI_U8 favorite : 1;
    HI_U8 bAir : 1;
    HI_U8 bCable : 1;
    HI_U8 bActive : 1;
    HI_S32 s32CarrierFreq; /* 音频载波频率 */
    HI_MW_SIF_AAOS_MODE_E enAaosMode;
};

/* 节目信息 */
struct HI_MW_ATV_PROG_S {
    HI_S32 s32Id;
    HI_U32 u32Freq;                         /* 结构体的首位置变量为 pll = 频率 / 步长  */
    HI_S32 s32FreqOffset;                   /* 用户手动微调偏移量 */
    HI_MW_ATV_CHANNEL_ATTR_S stChannelAttr; /* 节目属性 */
    HI_CHAR acName[NAME_LENGTH];            /* 节目名称(不超过 NAME_LENGTH-1 个字符) */
    HI_S32 sortIndex;
};

/* 节目非标信息 */
struct HI_MW_ATV_NSTD_S {
    HI_MW_ATV_SIF_BW_E enBandwidth;
    HI_MW_SIF_OVER_DEVIATION_E enOvermodulation;
    HI_U32 u32Freqoffset;
};

struct HI_MW_SIF_CARRIER_ATTR_S {
    HI_MW_AUDSYS_E enAudioSystem;
    HI_S32 s32CarrierFreq;
};

struct HI_MW_SIF_STANDARD_PRIORITY_LIST_S {
    HI_U32 u32ListLen;
    HI_MW_AUDSYS_E enPriorityList[HI_MW_AUDSYS_UNKNOW];
};

/* 颜色 RGB 三基色信息 */
struct HI_MW_COLOR_S {
    HI_U32 red;   /* 红色分量 */
    HI_U32 green; /* 绿色分量 */
    HI_U32 blue;  /* 蓝色分量 */
};

/* 定义区域RGB统计值结构体 */
typedef struct hiMW_DISP_AREA_RGB_DATA_S {
    HI_MW_COLOR_S stRGB[DISP_ABL_MAX_V_NUM][DISP_ABL_MAX_H_NUM];
} HI_MW_DISP_AREA_RGB_DATA_S;

/* 非线性缩放 */
struct HI_MW_NONLINEAR_SCL_S {
    HI_BOOL bEnable;
    HI_U32 u32SourceWidth;
    HI_U32 u32DisplayWidth;
};

/* 窗口属性 */
struct HI_MW_WINDOW_ATTR_S {
    HI_MW_WINDOW_E enWindow;               /* 窗口类型 */
    HI_MW_CROP_RECT_S stCropRect;          /* 输入源裁剪区域 */
    HI_MW_RECT_S stOutputRect;             /* 窗口大小 */
    HI_MW_RECT_S stVideoRect;              /* 实际视频大小 */
    HI_MW_NONLINEAR_SCL_S stNonLinearInfo; /* 非线性信息 */
    HI_BOOL bVirtual;
    HI_BOOL bOnLeft;             /* 是否双显置于左眼 */
    HI_MW_ASPECT_E enAspectType; /* 分为 TV, P2P, PANORAMA */
};

/* 指定播放器属性 */
struct HI_MW_PLAYER_ATTR_S {
    HI_MW_SRC_E enInputSource;        /* 指定输入源id */
    HI_BOOL bPip;                     /* 当前源是否处于PIP状态 */
    HI_BOOL bFocusWindow;             /* 当前源是否静音 */
    HI_MW_WINDOW_ATTR_S stWindowAttr; /* 播放器窗口属性 */
};

typedef enum {
    HI_MW_COLOR_PRIMARY_UNSPECIFY = 0,
    HI_MW_COLOR_PRIMARY_BT601_525,
    HI_MW_COLOR_PRIMARY_BT601_625,
    HI_MW_COLOR_PRIMARY_BT709,
    HI_MW_COLOR_PRIMARY_BT2020,
    HI_MW_COLOR_PRIMARY_CIE1931_XYZ,
    HI_MW_COLOR_PRIMARY_BT470_SYSM,
    HI_MW_COLOR_PRIMARY_SMPTE_240M,
    HI_MW_COLOR_PRIMARY_GENERIC_FILM,
    HI_MW_COLOR_PRIMARY_SMPTE_RP431,
    HI_MW_COLOR_PRIMARY_SMPTE_EG432_1,
    HI_MW_COLOR_PRIMARY_EBU_TECH_3213E,

    HI_MW_COLOR_PRIMARY_COLOR_MAX
} HI_MW_COLOR_PRIMARY_E;

typedef enum {
    HI_MW_COLOR_CS_YUV = 0,
    HI_MW_COLOR_CS_RGB,
    HI_MW_COLOR_CS_MAX
} HI_MW_COLOR_SPACE_CS_E;

typedef enum {
    HI_MW_COLOR_LIMITED_RANGE = 0,
    HI_MW_COLOR_FULL_RANGE,
    HI_MW_COLOR_RANGE_MAX
} HI_MW_COLOR_QUANTIFY_RANGE_E;

typedef enum {
    HI_MW_COLOR_TRANSFER_TYPE_GAMMA_SDR = 0, /* gammma sdr: ITU1886 */
    HI_MW_COLOR_TRANSFER_TYPE_GAMMA_HDR,     /* gammma hdr */
    HI_MW_COLOR_TRANSFER_TYPE_PQ,            /* PQ SMPTE:2084 */
    HI_MW_COLOR_TRANSFER_TYPE_HLG,           /* ARIB:STD B67 */
    HI_MW_COLOR_TRANSFER_TYPE_XVYCC,         /* IEC 61966-2-4 */
    HI_MW_COLOR_TRANSFER_TYPE_MAX
} HI_MW_COLOR_TRANSFER_CURVE_E;

typedef enum {
    HI_MW_COLOR_MATRIX_COEFFS_IDENTITY = 0,
    HI_MW_COLOR_MATRIX_COEFFS_UNSPECIFY,
    HI_MW_COLOR_MATRIX_COEFFS_BT601_525,
    HI_MW_COLOR_MATRIX_COEFFS_BT601_625,
    HI_MW_COLOR_MATRIX_COEFFS_BT709,
    HI_MW_COLOR_MATRIX_COEFFS_BT2020_NON_CONSTANT,
    HI_MW_COLOR_MATRIX_COEFFS_BT2020_CONSTANT,
    HI_MW_COLOR_MATRIX_COEFFS_BT2100_ICTCP,
    HI_MW_COLOR_MATRIX_COEFFS_USFCC,
    HI_MW_COLOR_MATRIX_COEFFS_SMPTE_240M,
    HI_MW_COLOR_MATRIX_COEFFS_YCGCO,
    HI_MW_COLOR_MATRIX_COEFFS_ST2085,
    HI_MW_COLOR_MATRIX_COEFFS_CHROMAT_NON_CONSTANT,
    HI_MW_COLOR_MATRIX_COEFFS_CHROMAT_CONSTANT,

    HI_MW_COLOR_MATRIX_COEFFS_MAX
} HI_MW_COLOR_MATRIX_COEFFS_E;

/* 添加此结构体，是为了适配V900中新的SDK接口，其他版本
不需要使用但在使用时需要注意，参考HI_MW_TIMING_INFO_S中的警告 */
typedef struct {
    HI_MW_COLOR_PRIMARY_E colorPrimary;
    HI_MW_COLOR_SPACE_CS_E colorSpace;
    HI_MW_COLOR_QUANTIFY_RANGE_E quantifyRange;
    HI_MW_COLOR_TRANSFER_CURVE_E transferType;
    HI_MW_COLOR_MATRIX_COEFFS_E matrixCoef;
} HI_MW_COLOR_DESCRIPTION_S;

/* 完整时序信息 */
struct HI_MW_TIMING_INFO_S {
    HI_U32 u32TimingIndex;                 /* Timing的序号 */
    HI_BOOL bBypassVpss;                   /* 是否bypass视频处理子系统 */
    HI_U32 u32Width;                       /* 图像宽度 */
    HI_U32 u32Height;                      /* P制与I制都为帧图像高度 */
    HI_U32 u32FrameRate;                   /* P制为输入帧频，I制为输入场频 */
    HI_MW_COLOR_SPACE_E enColorSpace;      /* 色彩空间 */
    HI_MW_COLOR_SYS_E enColorSys;          /* ATV/CVBS信源下的彩色制式 */
    HI_MW_VIDEO_FORMAT_E enPixelFmt;       /* 像素格式 */
    HI_MW_PIXEL_BITWIDTH_E enBitWidth;     /* 位宽色深 */
    HI_BOOL bInterlace;                    /* 是否隔行信号 */
    HI_MW_OVERSAMPLE_MODE_E enOverSample;  /* 过采样倍数 */
    HI_MW_FRAME_PACKING_TYPE_E enFormat3d; /* 3D格式 */
    HI_MW_HDMI_FORMAT_E enFormatHDMI;      /* HDMI/DVI/MHL */
    HI_U32 u32VBlank;                      /* 3D frame packing中的vblank */
    HI_BOOL bGraphicMode;                  /* Graphic? */
    HI_MW_SAMPLE_RATE_E enSampleRate;      /* HDMI 音频采样率 */
    HI_BOOL bMHL;                          /* HDMIRX MHL flag */
    HI_MW_COLOR_DESCRIPTION_S stColorDescrp; /** <HDMIRX color space */ /** <CNcomment:HDMIRX 色度空间 */
};

/* 非标信息 */
struct HI_MW_NONSTD_INFO_S {
    HI_BOOL bNonStd;  /* 是否非标 */
    HI_U32 u32Height; /* 非标图像高度 */
    HI_U32 u32Freq;   /* 非标输入场频 单位为每千秒场数目，61050表示61.05Hz */
};

/* 图像几何参数 */
struct HI_MW_SCOPE_S {
    HI_S32 s32HorPos;   /* 水平位置 */
    HI_S32 s32VerPos;   /* 垂直位置 */
    HI_S32 s32HorRange; /* 水平幅度 */
    HI_S32 s32VerRange; /* 垂直幅度 */
};

/* define the struct of picture setting */
struct HI_MW_PICTURE_S {
    HI_BOOL bDemo[HI_MW_DEMO_BUTT];
    HI_MW_PICMODE_E enPicMode; /* picture mode setting */
    HI_U32 u32Hue;
};

struct HI_MW_PICTURE_PQ_S {
    HI_BOOL bDci; /* dynamic contrast setting */
    HI_BOOL bBle; /* black(white) level extend setting */
    HI_BOOL bDbc; /* dynamic backlight */
    HI_BOOL bPqModule[HI_MW_PQ_MODULE_BUTT];
    HI_BOOL bHdmiclr; /* hdmi color range */
    HI_BOOL bOd;      /* PQTCON module OD */
    // HI_BOOL bRgbw;                           /* PQTCON module RGBW */   // 这三个后续实现时直接放开即可
    // HI_BOOL bPcid;                           /* PQTCON module PCID */
    // HI_BOOL bDemura;                         /* PQTCON module DEMURA */
};

struct HI_MW_PICTURE_DISP_S {
    HI_BOOL bOverscan;        /* vision */
    HI_BOOL bBacklightEnable; /* backlightenable */
    HI_BOOL bFreeze;          /* freeze frame */
    HI_MW_ASPECT_E enAspect;  /* aspect ratio type */
    HI_BOOL bMatrix;          /* matrix backlight */
    HI_MW_SCOPE_S stScope;    /* 图像几何参数 */
    HI_MW_RECT_S stVideoRect; /* 大窗口比例模式后的videorect */
};

struct HI_MW_PICTURE_VIDEO_S {
    HI_MW_COLOR_SYS_E enColorSys;
    HI_MW_LEVEL_E enMemc; /* memc level value */
    HI_BOOL bGameMode;
    HI_MW_PCMODE_E enPcMode;
    HI_U32 u32FilmMode; /* filmmode */
};

struct HI_MW_PICTURE_PANEL_S {
    HI_MW_DISP_REGION_ATTR_S stRegionAttr[REGION_INDEX_MAX]; /* 统计亚规显示颜色区域 */
    HI_MW_DISP_AREA_ATTR_S stAreaAttr[REGION_INDEX_MAX];     /* 统计欧规显示颜色区域 */
    HI_MW_DISP_OFFSET_S stVideoOffset;                       /* 视屏层与图形层在实体屏幕上的偏移量 */
    HI_MW_DISP_DELAY_S stDelayAttr;                          /* 视频通路延时 */
};

struct HI_MW_PICTURE_EF_S {
    HI_MW_LEVEL_E enNr;
    HI_MW_LEVEL_E enDeBlocking;
    HI_MW_LEVEL_E enDeRinging;
    HI_MW_LEVEL_E enFleshTone; /* flesh tone setting */
    HI_MW_DEMOLEVEL_E enSrLevel;
    HI_MW_PQ_COLOR_TYPE_E enColorGain; /* color gain setting */
    HI_MW_COLORTEMP_E enColorTemp;
};

struct HI_MW_IMAGE_S {
    HI_U32 u32Brightness;
    HI_U32 u32Contrast;
    HI_U32 u32Saturation;
    HI_U32 u32Sharpness;
    HI_U32 u32Hue;
    HI_U32 u32Backlight;
    HI_MW_LEVEL_E enNr;
    HI_MW_COLORTEMP_E enColorTemp;
};

struct HI_MW_PICTURE_PARA_S {
    HI_MW_PICTURE_S                 stPictureData;
    HI_MW_PICTURE_PQ_S              stPicPQData;
    HI_MW_PICTURE_DISP_S            stPicDispData;
    HI_MW_PICTURE_VIDEO_S           stPicVideoData;
    HI_MW_PICTURE_EF_S              stPicEfData;
    HI_MW_IMAGE_S                   astPicImageData[HI_MW_PICMODE_BUTT];
};

struct HI_MW_FACTORY_SHARP_GAIN_S {
    HI_U32 u32LtiGain;     // LTI增益 ，设置范围为[ 0 , 511]
    HI_U32 u32HPeakRatio;  // 水平Peaking增益 ，设置范围为[ 0 , 511]
    HI_U32 u32VPeakRatio;  // 垂直Peaking增益 ，设置范围为[ 0 , 511]
    HI_U32 u32DPeakRatio;  // 对角Peaking增益 ，设置范围为[ 0 , 511]
};

/* *Color GAIN */ /* CNcomment/:Color   增益 */
struct HI_MW_FACTORY_ACM_GAIN_S {
    HI_U32 u32LumGain;   // 亮度增益 ，设置范围[ 0 , 1023]
    HI_U32 u32HueGain;   // 色度增益 ，设置范围[ 0 , 1023]
    HI_U32 u32SatGain;   // 饱和度增益 ，设置范围[ 0 , 1023]
    HI_U32 u32Reserved;  // 保留位
};

/* *Color GAIN */ /* CNcomment/:对比度   增益 */
struct HI_MW_FACTORY_DCI_GAIN_S {
    HI_U32 u32LowGain;     // 低亮曲线增益 ，设置范围[ 0 , 63]
    HI_U32 u32MiddleGain;  // 中亮曲线增益 ，设置范围[ 0 , 63]
    HI_U32 u32HighGain;    // 高亮曲线增益 ，设置范围[ 0 , 63]
    HI_U32 u32Reserved;    // 保留位
};

struct HI_MW_PQ_HDR_INFO_S {
    HI_MW_PQ_COLOR_SPACE_E eColorSpace;
    HI_MW_PQ_GAMMA_TYPE_E eGammaType;
};

struct HI_MW_PQ_HDR_FIXMODE_S {
    HI_BOOL bFixed;
    HI_MW_PQ_HDR_INFO_S stHdrInfo;
};

struct HI_MW_LOCALDIM_S {
    HI_U32 u32LocDimStrength;                     /* local dimming strength level */
    HI_MW_PANEL_LDM_DEMO_MODE_E enLocDimDemoMode; /* local dimming demo mode */
    HI_BOOL bLocalDim;                            /* local dimming enable */
};

/* Color SATGAIN */ /* CNcomment/:颜色饱和度增益  */
struct HI_MW_PQ_SAT_GAIN_S {
    HI_MW_PQ_COLOR_E eColorType;
    HI_S32 s32Gain;
};

/* Color SATGAIN */ /* CNcomment/:颜色饱和度增益  */
struct HI_MW_PQ_HUE_GAIN_S {
    HI_MW_PQ_COLOR_E eColorType;
    HI_S32 s32Gain;
};

/* Color SATGAIN */ /* CNcomment/:颜色饱和度增益  */
struct HI_MW_PQ_BRI_GAIN_S {
    HI_MW_PQ_COLOR_E eColorType;
    HI_S32 s32Gain;
};

struct HI_MW_PQ_NRSTR_GAIN_S {
    HI_MW_PQ_NRSTR_E enNRSTR;
    HI_S32 u32Gain;
};

struct HI_MW_FACTORY_PQ_GAIN_S {
    HI_U32 u32pqCtiGain[HI_MW_COLOR_PQ_BUTT];
    HI_MW_PQ_SAT_GAIN_S stPqSatGain[HI_MW_PQ_COLOR_BUTT];
    HI_MW_PQ_HUE_GAIN_S stPqHueGain[HI_MW_PQ_COLOR_BUTT];
    HI_MW_PQ_BRI_GAIN_S stPqBriGain[HI_MW_PQ_COLOR_BUTT];
    HI_MW_PQ_NRSTR_GAIN_S stPqNrstrGain[HI_MW_PQ_NRSTR_BUTT];
};

struct HI_MW_PICMOD_PARA_S {
    HI_U32 u32Brightness;  /* < brightness  */ /* <CNcomment:亮度值 */
    HI_U32 u32Contrast;    /* < contrast  */   /* <CNcomment:对比度值 */
    HI_U32 u32Saturation;  /* < saturation  */ /* <CNcomment:饱和度值 */
    HI_U32 u32Hue;         /* < hue  */        /* <CNcomment:色度值 */
};

/* gamma结构 */
struct HI_MW_PQ_GAMMA_TABLE_S {
    HI_U32 au32R[1024]; /* 1024 is array size */
    HI_U32 au32G[1024]; /* 1024 is array size */
    HI_U32 au32B[1024]; /* 1024 is array size */
    HI_U32 au32W[1024]; /* 1024 is array size */
};

/* 音频音量属性 */
struct HI_MW_SND_ABSGAIN_S {
    HI_BOOL linearMode; /* ture:音量模式 false:dB模式 */
    HI_S32 gainLeft;    /* 左声道线性音量: 0~100 左声道dB音量:-70~0 */
    HI_S32 gainRight;   /* 右声道线性音量: 0~100 右声道dB音量:-70~0 */
};

/* 音频音量属性 */
struct HI_MW_SND_GAIN_S {
    HI_BOOL linearMode; /* ture:音量模式 false:dB模式 */
    HI_S32  gain; /* <bLinearMode is HI_TRUE, 线性音量: 0~100; linearMode is HI_FALSE, dB音量:-70~0 */
};

/* CNcomment/:高精度增益，范围从-81dB到18dB，步长为0.125dB */
struct HI_MW_SND_PRECIGAIN_ATTR_S {
    HI_S32 s32IntegerGain; /* 高精度增益的整数部分 */
    HI_S32 s32DecimalGain; /* 高精度增益的小数部分 如0.125，则值为125 */
};

struct HI_MW_LINE_IN_GAIN_ATTR_S {
    HI_U32 u32LineInGain; /* *<Line in gain, ranging from 0 to 100 */ /* CNcomment/:Line in音量: 0~100 */
};

/* CNcomment/:Sound设备Track信息 */
struct HI_MW_SND_TRACK_INFO_S {
    HI_U32 u32TrackNum;                    /* 绑定到Sound设备的Track数量 */
    HI_HANDLE hTrack[HI_MW_SND_TRACK_MAX]; /* 绑定到Sound设备的输出端口 */
};

/* CNcomment/:音频输出属性 */
struct HI_MW_AUDIOTRACK_ATTR_S {
    HI_MW_SND_TRACK_TYPE_E enTrackType; /* *<Track Type */ /* CNcomment/:Track类型 */
    HI_U32 u32FadeinMs; /* *<Fade in time(unit:ms) */        /* CNcomment/:淡入时间(单位: ms) */
    HI_U32 u32FadeoutMs; /* *<Fade out time(unit:ms) */          /* CNcomment/:淡出时间(单位: ms) */
    HI_U32 u32OutputBufSize; /* *<Track output buffer size */   /* CNcomment/:Track输出缓存大小 */
    HI_U32 u32BufLevelMs;   /* *<Output buffer data size control(ms),default 400ms */
                            /* CNcomment/:输出缓存中数据量控制(ms),默认400ms */
    HI_U32 u32StartThresholdMs; /* *<Track start threshold (ms), default value is 50ms, */
                                /* max value is equal to u32BufLevelMs, only valid for master track */
    /* CNcomment/:Track启动门限，默认值是50ms，最大值为u32BufLevelMs，此属性只对master track有效 */
};

/* hdmi audio input port attribute */
struct HI_MW_HDMI_AUDIO_ATTR_S {
    HI_MW_I2S_CHNUM_E enChannel;       /* channel number */
    HI_MW_I2S_BITDEPTH_E enBitDepth;   /* bit depth */
    HI_MW_SAMPLE_RATE_E enSampleRate;  /* sample Rate */
    HI_MW_HDMI_AUDIO_FMT_E enDataFmt;  /* audio data format */
    HI_MW_HDMI_DATA_TYPE_E enDataType; /* HDMI passthrough audio data type */
};

struct HI_MW_SYSTEM_WAKEUP_S {
    HI_MW_SYSTEM_WAKEUP_E enWakeup;
    HI_MW_SRC_E enSrc;
    HI_U64 u64Value;
};

struct HI_MW_SYSTEM_WAKEUP_ENABLE_S {
    HI_BOOL bVgaWakeup;   // enable VGA wakeup when signal plugin after no signal suspend.
    HI_BOOL bHdmiWakeup;  // enable HDMI wakeup when signal plugin after no signal suspend.
    HI_BOOL bScartWakeup; // enable SCART wakeup when signal plugin after no signal suspend.
    HI_BOOL bEPG;
    HI_BOOL bAutoUpdateProg;
    HI_U32  u32WKTimeoutVol; // wakeup Timeout volume
};

struct HI_MW_SRS_MAIN_CONFIG_S {
    HI_BOOL bEnable;
    HI_MW_SRS_MAINTECH_E enMainTech;
    HI_FLOAT f32HeadroomGain;
    HI_FLOAT f32BypassGain;
};

struct HI_MW_SRS_TVOLHD_CONFIG_S {
    HI_BOOL bEnable;
    HI_FLOAT f32LKFS;
    HI_FLOAT f32LevelIntensity;
};

struct HI_MW_SRS_TRUDLG_CONFIG_S {
    HI_BOOL bEnable;
    HI_FLOAT f32ClarityGain;
    HI_FLOAT f32ProcessGain;
};

struct HI_MW_SRS_CC3D_CONFIG_S {
    HI_BOOL bDlgClarityEnable;
    HI_FLOAT f32DlgClarityLevel;
    HI_FLOAT f32TruSurLevel;
    HI_FLOAT f32Srs3DSpaceLevel;
    HI_FLOAT f32Srs3DCenterLevel;
    HI_FLOAT f32FocusLevel;
    HI_BOOL bSpaceHPFEnable;
    HI_BOOL bCC3DSurroundBoostEnable;
    HI_MW_SRS_SPACEHPF_FREQ_E enSpaceHPFCutoffFreq;
};

struct HI_MW_SRS_CSHP_CONFIG_S {
    HI_MW_SRS_CSHP_MODE_E enCSDEnable;
    HI_MW_SRS_CSD_MODE_E enCSDMode;
    HI_BOOL bDlgClarityEnable;
    HI_FLOAT f32DlgClarityLevel;
};

struct HI_MW_SRS_TBHD_SPKSIZE_S {
    HI_MW_SRS_TBHD_SPKLFP_E enAudioFilter;
    HI_MW_SRS_TBHD_SPKLFP_E enAnalysisFilter;
};

struct HI_MW_SRS_COMM_CONFIG_S {
    HI_BOOL bTBHDEnable;
    HI_FLOAT f32TBHDLevel;
    HI_MW_SRS_TBHD_SPKSIZE_S stTBHDSpeakerSize;
    HI_BOOL bTBHDSplitAnalysisEnable;
    HI_BOOL bTBHDLevelIndependentEnable;
    HI_MW_SRS_TBHD_MODE_E enTBHDMode;
    HI_FLOAT f32TBHDCompressorLevel;
    HI_FLOAT f32TBHDBassClarityLevel;
    HI_BOOL bDefEnable;
    HI_FLOAT f32DefLevel;
};

struct HI_MW_SRS_GEQ_CONFIG_S {
    HI_BOOL bEnable;
    HI_FLOAT f32bgain[HI_MW_SRS_MAX_GEQ_5BAND_NUM];
};

struct HI_MW_SRS_AEQ_CONFIG_S {
    HI_BOOL bEnable;
    HI_FLOAT f32BypassGain;
};

struct HI_MW_SRS_HPF_CONFIG_S {
    HI_BOOL bEnable;
};

struct HI_MW_SRS_MVOL_CONFIG_S {
    HI_BOOL bEnable;
    HI_U32 u32VolStep;
    HI_S32 s32LRBalance;
    HI_BOOL bMute;
};

struct HI_MW_SRS_HDLMT_CONFIG_S {
    HI_BOOL bEnable;
    HI_FLOAT f32OutputGain;
    HI_S32 s32DelayLen;
    HI_BOOL bACEnable;
    HI_FLOAT f32ACLevel;
};

struct HI_MW_SRS_TONE_CONFIG_S {
    HI_BOOL bEnable;
    HI_S32 s32BassGain;
    HI_S32 s32TrebleGain;
};

struct HI_MW_SRS_ADVGAIN_CONFIG_S {
    HI_FLOAT f32MainTechInputGain[HI_MW_SRS_MAINTECH_NUMS];
    HI_FLOAT f32GeqInputGain;
    HI_FLOAT f32ToneInputGain;
    HI_FLOAT f32AeqInputGain;
    HI_FLOAT f32TBHDRefOffset;
    HI_DOUBLE f64HdLmtBoostOffset;
};

struct HI_MW_AUDIO_SRS_CUS_COEFS_CFG_S {
    HI_S32 s32LowPassAudioFilterCoef[2]; /* 2 is array size */
    HI_S32 s32LowBandAudioFilterCoef[3]; /* 3 is array size */
    HI_S32 s32MidBandAudioFilterCoef[3]; /* 3 is array size */
    HI_S32 s32LowPassAnalysisFilterCoef[2]; /* 2 is array size */
    HI_S32 s32LowBandAnalysisFilterCoef[3]; /* 3 is array size */
    HI_S32 s32MidBandAnalysisFilterCoef[3]; /* 3 is array size */
    HI_S32 s32QFilterCoef[8];  // TruBass Q filter: iwl, b0, b1, b2, a1, a2, gain_iwl, gain /* 8 is array size */
};

struct HI_MW_AUDIO_SRS_CUS_HPFCOEFS_CFG_S {
    HI_S32 s32HpfCoef[8]; /* 8 is array size */
};

struct HI_MW_AUDIO_SRS_CFG_S {
    HI_MW_SRS_MAIN_CONFIG_S stMainConfig;
    HI_MW_SRS_TVOLHD_CONFIG_S stTVolHDConfig;
    HI_MW_SRS_TRUDLG_CONFIG_S stTruDiagConfig;
    HI_MW_SRS_CC3D_CONFIG_S stCC3DConfig;
    HI_MW_SRS_CSHP_CONFIG_S stCSHPConfig;
    HI_MW_SRS_COMM_CONFIG_S stCommonConfig;
    HI_MW_SRS_GEQ_CONFIG_S stGEQConfig;
    HI_MW_SRS_AEQ_CONFIG_S stAEQConfig;
    HI_MW_SRS_HPF_CONFIG_S stHPFConfig;
    HI_MW_SRS_MVOL_CONFIG_S stMvolConfig;
    HI_MW_SRS_HDLMT_CONFIG_S stHdLmtConfig;
    HI_MW_SRS_TONE_CONFIG_S stToneConfig;
    HI_MW_SRS_ADVGAIN_CONFIG_S stAdvGainConfig;
};

struct HI_MW_AUDIO_SRS_S {
    HI_MW_AUDIO_SRS_CFG_S stSrsConfig;
    HI_MW_AUDIO_SRS_CUS_COEFS_CFG_S stCusCoefsCfg;
    HI_MW_AUDIO_SRS_CUS_HPFCOEFS_CFG_S stCusHpfCoefsCfg;
};
/* ****************SRS data struct type define end***************** */
/* ************* hisi sound effect process config define begin************** */
/* Define Pregain module Config */
struct HI_MW_SWS3D_PREGAIN_CONFIG_S {
    HI_BOOL bEnable;                          /* *<Define Pregain module convert ON/OFF */
    HI_S16 s16PreGain; /* *<Pregain [0,10] */ /* *CNComment: 前置增益[0, 10] */
};

/* Define DRC Gain module Config */
struct HI_MW_SWS3D_DRC_CONFIG_S {
    HI_BOOL bEnable;                           /* *<Define DRC Gain Module convert ON/OFF */
    HI_S16 s16DrcGain; /* *<DRC Gain [0,20] */ /* *<CNComment: DRC增益 [0, 20] */
};

/* Define Virtual Bass module Config */
struct HI_MW_SWS3D_BASS_CONFIG_S {
    HI_BOOL bEnable;                                                 /* *<Define Virtual Bass module convert ON/OFF */
    HI_S16 s16VbWetGain; /* *<Virtual Bass wet signal gain [0,12] */ /* *<CNComment: VB模块湿信号增益  [0, 12] */
    HI_S16 s16VbEarlyGain; /* *<Virtual Bass early gain [0,4] */     /* *<CNComment: VB模块早起反射增益  [0, 4] */
    HI_S16 s16VbNlpOutCutoff;    /* *<CNComment: VB模块谐波生成输出低通滤波器截止频率  [0, 5] */
    HI_S16 s16VbEarlyLPCutoff;   /* *<CNComment: VB模块早期反射低通滤波器截止频率  [0, 12] */
    HI_S16 s16VbNlpLPCutoff;     /* *<CNComment: VB模块谐波生成低通滤波器截止频率  [0, 12] */
    HI_S16 s16VbOutHPCutoff;     /* *<CNComment: VB模块信号输出高通滤波器截止频率  [0, 9] */
};

/* Define Treble Boost module Config */
struct HI_MW_SWS3D_TREBLE_CONFIG_S {
    HI_BOOL bEnable;                               /* *<Define Treble Boost module convert ON/OFF */
    HI_S16 s16TrbGain; /* *<Treble Gain [0,100] */ /* *<CNComment: 高频增强 [0, 100] */
};

/* Define Dialog EnhanceMent module Config */
struct HI_MW_SWS3D_DIALOG_CONFIG_S {
    HI_BOOL bEnable;                                 /* *<Define Dialog EnhanceMent module convert ON/OFF */
    HI_S16 s16SphGain; /* *<Speach Gain [0,100] */   /* *<CNComment: 人声增强增益 [0, 100] */
    HI_S16 s16SrcWidth; /* *<Source Width [0,100] */ /* *<CNComment: 音源增宽比例 [0, 100] */
};

/* Define Surround module Config */
struct HI_MW_SWS3D_SURROUND_CONFIG_S {
    HI_BOOL bEnable;                                 /* *<Define Surround module convert ON/OFF */
    HI_S16 s16SoundWidth; /* *Sound Width [0,100] */ /* *CNComment: 声场增度比例[0, 100] */
};

/* Define Back Gain module Config */
struct HI_MW_SWS3D_BACKGAIN_CONFIG_S {
    HI_BOOL bEnable;                             /* *<Define Back Gain Module convert ON/OFF */
    HI_S16 s16BackGain; /* *<Back Gain [0,10] */ /* *<CNComment: Back Gain增益 [0, 10] */
};

/* Define GEQ module Config */
struct HI_MW_SWS3D_GEQ_CONFIG_S {
    HI_BOOL bEnable;                                /* *<Define GEQ module convert ON/OFF */
    HI_S16 s16EQBands; /* *<EQ Band Num [0,12] */   /* *<CNComment: EQ段数 [0-12] */
    /* *<EQ Band Type: 0 peaking filter, 1 low-shelving filter, 2 high-shelving filter */
    /* *<CNComment: 滤波器类型， 0 峰值滤波器， 1 低搁架滤波， 2 高搁架滤波 */
    HI_S16 s16EQType[HI_MW_SWS3D_EQ_BANDS_MAXNUM];
    /* *<EQ Gain [0,40]corresponding to real range[-10db, 10db], step by 0.5db  */
    /* *<CNComment: EQ增益 (0, 40)， 【-10db， +10db】， 每个刻度间距0.5db， 20为0db， 0为-10db, 40为+10db */
    HI_S16 s16EQGain[HI_MW_SWS3D_EQ_BANDS_MAXNUM];
    /* *<EQ Q[100,10000] corresponding to real range[0.1, 10], step by 0.001  */
    /* *<CNComment: EQ Q值 (100,10000),对应实际Q只为【0.1， 10】，
    为了便于设置，采用整数表示，比如Q值1.414，表示为1414 */
    HI_S16 s16EQQ[HI_MW_SWS3D_EQ_BANDS_MAXNUM];
    /* *<EQ Cut Off Frequency [20,20000] */  /* *<CNComment: EQ Fc  [20,20000] */
    HI_S16 s16EQFc[HI_MW_SWS3D_EQ_BANDS_MAXNUM];
};

/* Define LPH module Config */
struct HI_MW_SWS3D_LPH_CONFIG_S {
    HI_BOOL bEnable; /* *<Define LPH Module convert ON/OFF */
};

/* Define MBC module Config */
struct HI_MW_SWS3D_MBC_CONFIG_S {
    HI_BOOL bEnable;                                  /* *<Define MBC module convert ON/OFF */
    HI_S16 s16MBCBands; /* *<MBC Bands Num [0, 3] */  /* *<CNComment: MBC段数 [0, 3] */
    /* *<MBC cutoff frequency [0, 20] */ /* *<CNComment: MBC各段分频点 [0, 20] */
    HI_S16 s16MBCFc[HI_MW_SWS3D_MBC_BANDS - 1];
    /* *<MBC gain [0, 20] */    /* *<CNComment: MBC各段增益 [0, 20] */
    HI_S16 s16MBCG[HI_MW_SWS3D_MBC_BANDS];
    HI_S16 s16MBCT[HI_MW_SWS3D_MBC_BANDS]; /* *<MBC threshold [-80, 10] */  /* *<CNComment: MBC各段门限 [-80, 10] */
    /* *<MBC compress percent [1, 100] */ /* *<CNComment: MBC各段压缩比 [1, 100] */
    HI_S16 s16MBCR[HI_MW_SWS3D_MBC_BANDS];
    HI_S16 s16MBCW[HI_MW_SWS3D_MBC_BANDS]; /* *<MBC corner [0, 40] */  /* *<CNComment: MBC各段软拐点 [0, 40] */
    /* *<MBC attack time [0,10000] */ /* *<CNComment: MBC各段建立时间 [0, 10000] */
    HI_S16 s16MBCAtt[HI_MW_SWS3D_MBC_BANDS];
    /* *<MBC Release time [0,10000] */  /* *<CNComment: MBC各段释放时间 [0, 10000] */
    HI_S16 s16MBCRel[HI_MW_SWS3D_MBC_BANDS];
};

/* Define Limit module Config */
struct HI_MW_SWS3D_LIMIT_CONFIG_S {
    HI_BOOL bEnable;                      /* *<Define Limit Module convert ON/OFF */
    HI_S16 s16Limit; /* *<Limit [0,20] */ /* *<CNComment: Limit阈值 [0, 20] */
};

/* Define SWS3D Config */
struct HI_MW_AUDIO_SWS3D_S {
    HI_BOOL bEnable;                            /* *<Define SWS3.0 convert ON/OFF */
    /* *<Device Type:0 Stereo Speaker,1 Mono Speaker,2 headphone */ /* *<CNComment: 0 双扬声器, 1 单扬声器, 2 耳机 */
    HI_S16 s16DevType;
    HI_MW_SWS3D_PREGAIN_CONFIG_S stPregainCfg;  /* *<Pregain module Config  */
    HI_MW_SWS3D_DRC_CONFIG_S stDrcCfg;          /* *<DRC Gain module Config  */
    HI_MW_SWS3D_BASS_CONFIG_S
    stBassCfg;                                             /* *<Virtual Bass module Config  */
    HI_MW_SWS3D_TREBLE_CONFIG_S
    stTrebleCfg;                                           /* *<Treble Boost module Config  */
    HI_MW_SWS3D_DIALOG_CONFIG_S
    stDialogCfg;                                           /* *<Dialog Enhancement module Config  */
    HI_MW_SWS3D_SURROUND_CONFIG_S stSurroundCfg;           /* *<Surround module Config  */
    HI_MW_SWS3D_BACKGAIN_CONFIG_S stBackgainCfg;           /* *<Back Gain module Config  */
    HI_MW_SWS3D_GEQ_CONFIG_S stGEQCfg;                     /* *<GEQ module Config  */
    HI_MW_SWS3D_LPH_CONFIG_S stLPHCfg;                     /* *<LPH module Config  */
    HI_MW_SWS3D_MBC_CONFIG_S stMBCCfg;                     /* *<MBC module Config  */
    HI_MW_SWS3D_LIMIT_CONFIG_S stLimitCfg;                 /* *<Limit module Config  */
};

struct HI_MW_DBX_TOTSON_CONFIG_S {
    HI_MW_DBX_TOTSON_E enTotSon;
};

struct HI_MW_DBX_TOTVOL_CONFIG_S {
    HI_MW_DBX_TOTVOL_E enTotVol;
};

struct HI_MW_DBX_TOTSUR_CONFIG_S {
    HI_MW_DBX_TOTSUR_E enTotSur;
};

struct HI_MW_AUDIO_DBX_S {
    HI_BOOL bEnable;
    HI_MW_DBX_TOTSON_CONFIG_S stTotSon;
    HI_MW_DBX_TOTVOL_CONFIG_S stTotVol;
    HI_MW_DBX_TOTSUR_CONFIG_S stTotSur;
    HI_U32 u32Volume;
};
/* dbx  END */
struct HI_MW_DBX2_TOTSON_CONFIG_S {
    HI_MW_DBX2_TOTSON_E enTotSon;
};

struct HI_MW_DBX2_TOTVOL_CONFIG_S {
    HI_MW_DBX2_TOTVOL_E enTotVol;
};

struct HI_MW_DBX2_TOTSUR_CONFIG_S {
    HI_MW_DBX2_TOTSUR_E enTotSur;
};

struct HI_MW_AUDIO_DBX2_S {
    HI_BOOL bEnable;
    HI_MW_DBX2_TOTSON_CONFIG_S stTotSon;
    HI_MW_DBX2_TOTVOL_CONFIG_S stTotVol;
    HI_MW_DBX2_TOTSUR_CONFIG_S stTotSur;
    HI_U32 u32Volume;
};
/* dbx2  END */
struct HI_MW_AUDIO_DAP_S {
    HI_BOOL bDoblyAtmosEnable;
    HI_MW_DAP_SOUND_MODE_E enDapMode;
    HI_BOOL bSurround;
    HI_MW_DAP_DIALOG_ENHANCER_E enEnhancerMode;
    HI_BOOL bVolumeLeveler;
};

/* ************* hisi sound effect process config define end ************** */
struct HI_MW_SPDIF_CTRL_S {
    HI_BOOL bSpdifEnable;
    HI_MW_SND_SPDIF_MODE_E enSpdifMode;
};

struct HI_MW_ARC_CTRL_S {
    HI_BOOL bArcEnable;                /* ARC enable or not */
    HI_MW_SND_ARC_MODE_E enSndArcMode; /* ARC mode setting */
};

// 音效功能控制，非属性设置的
struct HI_MW_AUDIO_CTRL_S {
    HI_MW_SPDIF_CTRL_S stSpdifCtrl;
    HI_MW_ARC_CTRL_S stArcCtrl;
    HI_BOOL bAvc;
    HI_BOOL bDrc[HI_MW_AUDIO_CHANNEL_BUTT];
    HI_MW_DOLBY_E enDolbyMode;
    HI_MW_SPEAKER_E enSpeaker;
    HI_MW_SNDMODE_E enSndMode;    /* audio mode setting */
    HI_MW_SNDLOCATE_E enHangMode; /* hang mode */
    HI_U32 u32StereoMode;
    HI_MW_AEF_TYPE_E enAefType; /* audio effect type 各音效是互斥存在的 */
    // audio device id, used for audio framework,in MW just save in and read from DB. Auto 2(speaker)
    HI_U32 u32Dev;
    HI_U32 u32AvSync;           /* audio and vedio Sync */
    HI_U32 u32SinkDelay;
    HI_BOOL bUsrAudioMute;
};

struct HI_MW_AUDIO_BASE_S {
    HI_U32 u32Volume;  /* Volume ,parallelism precivol */
    HI_S32 s32Balance; /* Balance */
    HI_BOOL bMute;
    HI_BOOL bAefByPass;
    HI_BOOL bAdOutputEnable;
    HI_MW_TRACK_MODE_E enTrackMode;
    HI_U32 u32Delay;
};

struct HI_MW_AUDIO_AMP_S {
    HI_BOOL bAmpMute;
    HI_BOOL bWooferEnable;
    HI_U32 u32Woofer;
    HI_BOOL bSuperBass;
};

struct HI_MW_AUDIO_STREAM_INFO_S {
    HI_MW_AUDIO_STREAM_TYPE_E enType;
    HI_S32 s32Value;
};

struct HI_MW_DRC_ATTR_S {
    HI_U32 u32AttackTime;    /* the attack time(unit:ms),    ranging from 20 to 2000 */
    HI_U32 u32ReleaseTime;   /* the Release time(unit:ms),   ranging from 20 to 2000 */
    /* the threshold value(driver unit:dB),ranging from -8 to -1,MW ranging from -80000 to -1000 */
    HI_S32 s32Thrhd;
    /* the maximum value(driver unit:dB),  ranging from -8 to -1,MW ranging from -80000 to -1000 */
    HI_S32 s32Limit;
    /* *<refmode(0:process both channel together mode, 1:process each channel independently mode) */
    HI_U32 u32RefMode;
    HI_U32 u32SpeedCtrlMode; /* *<speedctrlmode(0:normal mode, 1:fast mode) */
};

/* AVC attribute */
struct HI_MW_AVC_ATTR_S {
    HI_U32 u32AttackTime;     /* the attack time(unit:ms),       ranging from 20 to 2000 */
    HI_U32 u32ReleaseTime;    /* the Release time(unit:ms),      ranging from 20 to 2000 */
    /* the threshold value(driver unit:dB),   ranging from -40 to -16,MW ranging from -40000 to -16000 */
    HI_S32 s32ThresholdLevel;
    /* the threshold value(driver unit:dB),   ranging from 0 to 8,MW ranging from 0 to 8000    */
    HI_S32 s32Gain;
    /* the target level value(driver unit:dB),ranging from -40 to -0,MW ranging from -40000 to 0  */
    HI_S32 s32LimiterLevel;
    /* *<refmode(0:process both channel together mode, 1:process each channel independently mode) */
    HI_U32 u32RefMode;
    HI_U32 u32SpeedCtrlMode;  /* *<speedctrlmode(0:normal mode, 1:fast mode) */
};

struct HI_MW_SND_MODE_ATTR_S {
    HI_U32 au32EqBand[10]; /* 10 is array size */
    HI_BOOL bTruDialog;
    HI_BOOL bTruBass;
};

struct HI_MW_3D_MODE_S {
    HI_BOOL bLrSwitch;
    HI_S32 s32Depth;
    HI_S32 s32View;
    HI_MW_3DMODE_E enMode;
    HI_MW_3DT2D_E enDisplay3DT2D;
    HI_MW_2DT3D_SCENE_E enSence2DT3D;
};

struct HI_MW_PC_TIMMING_S {
    HI_BOOL bAutoAdj;
    HI_U32 u32ModeIndex;
    HI_U32 u32SaveOrder;
    HI_U32 u32PhaseForUI;  /* 保存给UI  使用的Phase  */
    HI_U32 u32HPosForUI;   /* 保存给UI  使用的H Position  */
    HI_U32 u32VPosForUI;   /* 保存给UI  使用的V Position  */
    HI_U32 u32ClockForUI;  /* 保存给UI  使用的Clock  */
    HI_U32 u32PhaseForDrv; /* 保存给驱动使用的Phase  */
    HI_U32 u32HPosForDrv;  /* 保存给驱动使用的H Position  */
    HI_U32 u32VPosForDrv;  /* 保存给驱动使用的V Position  */
    HI_U32 u32ClockForDrv; /* 保存给驱动使用的Clock  */
};

struct HI_MW_CEC_MENU_S {
    HI_BOOL abCecMenuState[HI_MW_CEC_UI_MENU_BUTT];
};

struct HI_MW_CEC_DEVINFO_ENABLE_S {
    HI_BOOL oneTouchPlayEnable;
    HI_BOOL cecEnable;
};

struct HI_MW_TIF_CEC_OPTION_S {
    HI_BOOL abTifCecState[HI_MW_TIF_CEC_OPTION_BUTT];
};

typedef HI_MW_PC_TIMMING_S HI_MW_PC_TIMMING_ARR[PC_MODE_SAVE_MAX];

struct HI_MW_PQ_DCI_HISTGRAM_S {
    HI_BOOL bNormalize;                       // 判断是否需要归一化以后的直方图还是点数
    HI_MW_PQ_HISTGRAM_SITE_E enHistgramSite;  // 获取直方图的类型
    HI_S32 histogram[HISTOGRAM_COUNT];
};

struct HI_MW_USTV_RATING_SYSTEM_S {
    HI_MW_VCHIP_USTV_RATING_E enUstvRating;
    HI_MW_USTV_CATEGORY_MODE_E aenUstvCategory[HI_MW_VCHIP_USTV_RATING_BUTT][HI_MW_VCHIP_USTV_CATEGORY_BUTT];
};

union HI_MW_PARENT_RATING_U {
    HI_MW_VCHIP_MPA_RATING_E enMpaRating;
    HI_MW_USTV_RATING_SYSTEM_S stUstvRating;
    HI_MW_VCHIP_CAN_ENG_RATING_E enCanEngRating;
    HI_MW_VCHIP_CAN_FRE_RATING_E enCanFreRating;
};

struct HI_MW_PARENT_LOCK_S {
    HI_MW_VCHIP_SYSTEM_E enSystem;
    HI_MW_PARENT_RATING_U unRating;
};

/* define the struct of system lock setting */
struct HI_MW_LOCK_S {
    HI_BOOL bLockEnable;              /* system lock enable  */
    HI_BOOL bSrcLockEnable;           /* system lock enable  */
    HI_BOOL bChanLockEnable;          /* system lock enable  */
    HI_BOOL bParentLockEnable;        /* system lock enable  */
    HI_BOOL bSrcLock[HI_MW_SRC_BUTT]; /* source lock enable  */
    HI_BOOL bUnRating;
    HI_MW_PARENT_LOCK_S astParentLock[HI_MW_VCHIP_SYSTEM_BUTT]; /* parent lock setting */
};

/* CC switch status */
enum HI_MW_CC_SWITCH_E {
    HI_MW_CC_SWITCH_OFF = 0,    /* 关闭 */
    HI_MW_CC_SWITCH_ON = 1,     /* 打开 */
    HI_MW_CC_SWITCH_ONMUTE = 2, /* 静音打开 */

    HI_MW_CC_SWITCH_BUTT = 3
};

/* define the struct of system closed caption setting */
struct HI_MW_CC_S {
    HI_MW_CC_SWITCH_E u32Enable; /* system cc switch status  */
    HI_U32 u32Chn;               /* atv cc channel */
};

/* TTX Language */
enum HI_MW_TTX_LANG_E {
    HI_MW_TTX_LANG_ARABIC = 0,  /* 阿拉伯 */
    HI_MW_TTX_LANG_EAST = 1,    /* 东欧 */
    HI_MW_TTX_LANG_WEST = 2,    /* 西欧 */
    HI_MW_TTX_LANG_FARSI = 3,   /* 波斯 */
    HI_MW_TTX_LANG_RUSSIAN = 4, /* 俄罗斯 */

    HI_MW_TTX_LANG_BUTT = 5
};

enum HI_MW_I2C_RATE_E {
    HI_MW_I2C_RATE_10K = 0, /* 标准速率：10kbit/s  */
    HI_MW_I2C_RATE_50K,     /* 标准速率：50kbit/s  */
    HI_MW_I2C_RATE_100K,    /* 标准速率：100kbit/s */
    HI_MW_I2C_RATE_200K,    /* 标准速率：200kbit/s */
    HI_MW_I2C_RATE_300K,    /* 标准速率：300kbit/s */
    HI_MW_I2C_RATE_400K,    /* 快速速率：400kbit/s */

    HI_MW_I2C_RATE_BUTT
};

/* define the struct of system setting */
struct HI_MW_SYSTEM_S {
    HI_MW_SRC_E enSubSrc;               /* current sub window source */
    HI_MW_SRC_E enBootSrc;              /* boot source */
    HI_MW_SRC_E enLastSrc;              /* last source */
    HI_MW_SCREEN_COLOR_E enScreenColor; /* black/blue screen  */
    HI_U32 u32OsdDuration;              /* osd duration */
    HI_MW_LANGUAGE_E enLanguage;        /* osd language */
    HI_BOOL bLsadckey;                  /* lsadckey */
    HI_BOOL bAp;
    HI_MW_HDMIRX_EDID_TYPE_E aenHdmiRxEdidType[HI_MW_HDMIRX_PORT_BUTT];
    HI_BOOL bTeletextShow;
    HI_BOOL bCaptureEnable;         /* capture enable */
    HI_MW_COUNTRY_E enCountry;      /* country or region name */
    HI_MW_TTX_LANG_E enTtxLanguage; /* Ttx Language */
    ATV_LOG_PRIO_E enLogLevel;      /* MW log level */
    HI_MW_TVSYSTEM_E enTvSystem;    /* tvSystem */
    HI_MW_CHANNELMANTYPE_E enChaManType[HI_MW_TVSYSTEM_BUTT]; /* channel manage type */
    HI_BOOL bWssEnable[HI_MW_TVSYSTEM_BUTT];                 /* WSS enable */
    HI_BOOL bAtsEnable[HI_MW_TVSYSTEM_BUTT];                 /* ATS enable */
    HI_BOOL bTtxEnable[HI_MW_TVSYSTEM_BUTT];                 /* TTX enable */
    HI_BOOL bCCEnable[HI_MW_TVSYSTEM_BUTT];                  /* CC enable */
    HI_BOOL bVChipEnable[HI_MW_TVSYSTEM_BUTT];               /* VChip enable */
    HI_BOOL bStandbyAutoScanEnable;                           /* standby auto scan enable */
    HI_MW_SCART_INPUT_E enScartInput;                         /* scart innput: AV/S-VIDEO */
};

struct HI_MW_FACTORY_PANEL_S {
    HI_MW_PATTERN_E enPatternIndex;
    HI_U32 u32PanelIndex;
    HI_S32 s32PanelSwing;
    HI_S32 s32PwmOffset;
    HI_MW_VO_ROTATION_E enRotation; /* 视频旋转角度 */
};

struct HI_MW_FACTORY_CTRL_S {
    HI_BOOL bPictureWhiteSaveBySrc;
    HI_BOOL bNonStdEnable;
    HI_BOOL bAgingMode;
    HI_BOOL bWatchDog;
    HI_U32 u32ListNum;
    HI_MW_NLA_E enNlaItemIndex;
    HI_MW_EDID_TYPE_E enEdidType;
    HI_BOOL bTvdPedestal;
    HI_U32 u32WatchDogTime;
};

struct HI_MW_FACTORY_PANEL_INTF_ATTR_S {
    HI_BOOL bSpreadEnable;
    HI_U32 u32ComVoltage;
    HI_U32 u32DrvCurrent;
    HI_U32 u32Emphasis;
    HI_U32 u32SpreadRatio;
    HI_U32 u32SpreadFreq;
};

struct HI_MW_FACTORY_LVDS_S {
    HI_BOOL bLvdsEnable;
    HI_BOOL bLvdsSpreadEnable;
    HI_U32 u32LvdsSpreadRatio;
    HI_MW_PANEL_LVDS_SSFREQ_E enLvdsSpreadFreq;
    HI_MW_PANEL_LVDS_CURRENT_E enDrvCurrent;
    HI_MW_PANEL_LVDS_VOLTAGE_E enComVoltage;
};

/* *LVDS link map  */
struct HI_MW_PANEL_LVDS_LINKMAP_S {
    HI_MW_PANEL_LVDS_LINKMAP_E enLink1;
    HI_MW_PANEL_LVDS_LINKMAP_E enLink2;
    HI_MW_PANEL_LVDS_LINKMAP_E enLink3;
    HI_MW_PANEL_LVDS_LINKMAP_E enLink4;
};

struct HI_MW_FACTORY_POWER_S {
    HI_MW_POWERMODE_E enPowerMode;
    HI_BOOL bPowerMusic;
    HI_MW_POWERMUSIC_E u32PowerMusicNo;
    HI_U32 u32PowerMusicVol;
    HI_MW_SUSPEND_E enSuspendMode;
};

struct HI_MW_FACTORY_DDR_S {
    HI_BOOL bDdrSpreadEnable;
    HI_U32 u32DdrSpreadRatio;
    HI_U32 u32DdrSpreadFreq;
};

struct HI_MW_FACTORY_EMMC_S {
    HI_BOOL bEmmcSpreadEnable;
    HI_U32 u32EmmcSpreadRatio;
    HI_U32 u32EmmcSpreadFreq;
};

struct HI_MW_FACTORY_VBO_S {
    HI_BOOL bVboClkEnable;
    HI_BOOL bVboSpreadEnable;
    HI_U32 u32VboSpreadRatio;
    HI_MW_PANEL_VBO_SSFREQ_E enVboSpreadFreq;
    HI_MW_PANEL_VBO_CURRENT_E enVboDrvCur;
    HI_MW_PANEL_VBO_EMP_E enVboEmphasis;
};

struct HI_MW_FACTORY_GMAC_S {
    HI_BOOL bGmacClkEnable;
    HI_BOOL bGmacSpreadEnable;
    HI_U32 u32GmacSpreadRatio;
    HI_U32 u32GmacSpreadFreq;
    HI_U32 u32GmacDrvCur;
};

struct HI_MW_FACTORY_USB_S {
    HI_BOOL bUsb3SpreadEnable;
    HI_U32 u32Usb3SpreadRatio;
    HI_U32 u32Usb3SpreadFreq;
    HI_U32 u32Usb3DrvCur;
};

struct HI_MW_FACTORY_EMI_S {
    HI_BOOL bPllEnable;
    HI_BOOL bCiClkEnable;  // CI 使能开关
    HI_BOOL bVdacEnable;   // AVOut 使能开关
};

struct HI_MW_FACTORY_PQ_S {
    HI_U32 u32NrThreshold;
    HI_U32 u32AvcThreshold;
    HI_U32 u32Gamma;
};

struct HI_MW_FACTORY_DEVICE_S {
    HI_BOOL bUart;
    HI_BOOL BuartDebug;
    HI_BOOL bI2c[I2C_TOTAL];
};

struct HI_MW_NLA_INFO_S {
    HI_U32 u32Point0;
    HI_U32 u32Point25;
    HI_U32 u32Point50;
    HI_U32 u32Point75;
    HI_U32 u32Point100;
};

/* 屏的属性 */
struct HI_MW_PANEL_ATTR_S {
    HI_U32 width;
    HI_U32 height;
    HI_MW_PANEL_ASPECT_E aspect;
    HI_MW_DISP_3DMODE_E mode3d;
};

struct HI_MW_PANEL_DIM_STRENGTH_RANGE_S {
    HI_U32 u32MinStrength; /* Dim strength Min Range */
    HI_U32 u32MaxStrength; /* Dim strength Max Range */
};

struct HI_MW_PANEL_DIM_STRENGTH_INFO_S {
    HI_MW_PANEL_DIM_TYPE_E enDimType;                    /* Dim Type */
    HI_MW_PANEL_DIM_STRENGTH_RANGE_S stDimStrengthRange; /* Dim strength Range */
};

struct HI_MW_PANEL_LDM_DATA_S {
    HI_U8 u8DataBuf[LDM_MAX_TOTAL_NUM]; /* 保存数据的Buf */
    HI_U32 u32DataCnt;                  /* 数据的个数 */
};

/* 开机模式 */
struct HI_MW_POWER_S {
    HI_MW_POWERMODE_E enPowerMode;
    HI_MW_RESTORE_E enRestoreFlag;
};

struct HI_MW_MAC_S {
    char mac[MAC_LENTH];
};

struct HI_MW_OVERSCAN_OPS {
    HI_S32 (*pfnGetOverscanEnbale)(HI_BOOL *);
    HI_S32 (*pfnGetAtvOverscan)(HI_MW_RECT_S *, HI_MW_TVD_RESOLUTION_E);
    HI_S32 (*pfnGetCvbsOverscan)(HI_MW_RECT_S *, HI_MW_TVD_RESOLUTION_E);
    HI_S32 (*pfnGetYpbprOverscan)(HI_MW_RECT_S *, HI_MW_HD_RESOLUTION_E);
    HI_S32 (*pfnGetHdmiOverscan)(HI_MW_RECT_S *, HI_MW_HD_RESOLUTION_E);
    HI_S32 (*pfnGetDTVOverscan)(HI_MW_RECT_S *, HI_MW_HD_RESOLUTION_E);
    HI_S32 (*pfnGetMediaOverscan)(HI_MW_RECT_S *, HI_MW_HD_RESOLUTION_E);
    HI_S32 (*pfnGetScopePara)(HI_MW_SCOPE_S *);
};

struct HI_MW_ARCCAP_S {
    HI_BOOL bAudioFmtSupported[HI_MW_EDID_AUDIO_FORMAT_CODE_BUTT]; /* Audio capability */
};

struct HI_MW_TTX_LANG_INFO_S {
    HI_CHAR aszLanguage[HI_MW_LANG_CODE_LEN + 1]; /* Language code */
    HI_U8 u8MagazineNum;                          /* Magazine number */
    HI_U8 u8PageNum;                              /* Page number */
};

struct HI_MW_CAPTURE_RECT_S {
    HI_U32 u32Height;
    HI_U32 u32Width;
};

struct HI_MW_CAPTURE_ADDR_S {
    HI_U32 u32PhyAddr;      /* 物理地址 */
    HI_U32 u32VirturalAddr; /* 虚拟地址 */
};

struct HI_MW_CAPTURE_INFO_S {
    HI_MW_CAPTURE_TYPE_E eType;
    HI_MW_CAPTURE_RECT_S stRect;
    HI_MW_CAPTURE_ADDR_S stAddr;
};

/* NTSC Scan information */
struct HI_MW_NTSC_SCAN_S {
    HI_BOOL bAntenna;  /* Antenna or cable table flag */
    HI_BOOL bNtscScan; /* Ntsc Scan flag */
    HI_U32  airProgNo;
    HI_U32  cableProgNo;
};

struct HI_MW_VCHIP_INFO_S {
    HI_U8 u8Class;
    HI_U8 u8Level;
    HI_U8 u8Content;
};

struct HI_MW_FRAME_INFO_S {
    HI_U32 u32YStride;
    HI_U32 u32Width;
    HI_U32 u32Height;
    HI_U32 u32bits;
    HI_U32 u32loadFrameNo;
    HI_U32 u32loadtype;
};

struct HI_MW_TEST_PATTERN_S {
    HI_BOOL bEnable;
    HI_MW_TEST_PATTERN_E enTestPattern;
    HI_MW_COLOR_S stColor;
};

struct HI_MW_VIDEO_USERDATA_S {
    HI_MW_VIDEO_BROADCAST_PROFILE_E enBroadcastProfile;
    HI_MW_VIDEO_USER_DATA_POSITION_E enPositionInStream;
    HI_U32 u32Pts;
    HI_U32 u32SeqCnt;
    HI_U32 u32SeqFrameCnt;
    HI_U8 *pu8Buffer;
    HI_U32 u32Length;
    HI_BOOL bBufferOverflow;
    HI_BOOL bTopFieldFirst;
};

struct HI_MW_PARA_RANGE_S {
    HI_U32 u32ParaMin;
    HI_U32 u32ParaMax;
};

struct HI_MW_HDMITX_DATA_S {
    HI_MW_HDMITX_FMT_E enTimmingFormat;
    HI_MW_HDMITX_COLOR_FORMAT_E enColorFormat;
};

/* boot mode for hdmitx info */ /* hdmitx<BF><AA><BB><FA><C6><F4><B6><AF><D0><C5>? */
struct HI_MW_HDMITX_BOOT_INFO_S {
    /* <C6><F4><B6><AF>?? 0:no hdmitx 1:disp1 output hdmitx  2:disp0 output hdmitx */
    HI_MW_HDMITX_BOOT_MODE_E enBootMode;
    /* <CF><D4>?<B8><F1>? 0:1080p60Hz, 1:1080p50Hz 29:1366x768p60Hz 258:2160p30Hz 259:2160p50Hz 260:2160p60Hz */
    HI_U32 u32DispFormat;
    HI_MW_PIXEL_BITWIDTH_E enDeepColor;     /* 0:24bit, 1:30bit, 2:36bit, 255:off */
    HI_MW_HDMITX_COLORSPACE_E enColorSpace; /* 0:RGB444, 1:YUV422, 2:YUV444, 3:YUV420 */
};

struct HI_MW_MICRODIM_BLOCK_DATA_S {
    HI_U8 u8WidthPartionIndex;
    HI_U8 u8HeightPartionIndex;
    HI_U8 u8WidthPartion;
    HI_U8 u8HeightPartion;
};

struct HI_MW_TEN_SPKER_VOLUME_CURVE {
	HI_S32  s32Point0;
    HI_S32  s32Point10;
    HI_S32  s32Point20;
    HI_S32  s32Point30;
    HI_S32  s32Point40;
    HI_S32  s32Point50;
    HI_S32  s32Point60;
    HI_S32  s32Point70;
    HI_S32  s32Point80;
    HI_S32  s32Point90;
    HI_S32  s32Point100;
};
struct HI_MW_TEN_SPKER_VOLUME_CURVE_GAIN {
	HI_DOUBLE  f64Point0;
    HI_DOUBLE  f64Point10;
    HI_DOUBLE  f64Point20;
    HI_DOUBLE  f64Point30;
    HI_DOUBLE  f64Point40;
    HI_DOUBLE  f64Point50;
    HI_DOUBLE  f64Point60;
    HI_DOUBLE  f64Point70;
    HI_DOUBLE  f64Point80;
    HI_DOUBLE  f64Point90;
    HI_DOUBLE  f64Point100;
};
void PrintSrsInfo(HI_BOOL bEnable, const HI_MW_AUDIO_SRS_S &stSrs);
void PrintSrsInfoSubOne(HI_BOOL bEnable, const HI_MW_AUDIO_SRS_S &stSrs);


inline const HI_CHAR *boolToString(HI_BOOL bYesNo)
{
    return (bYesNo == HI_TRUE ? "Yes" : "No");
};
inline const HI_CHAR *pointerToString(HI_VOID *pPara)
{
    return ((pPara) != HI_NULL ? "Yes" : "No");
};
const HI_CHAR *GetSrcName(const HI_MW_SRC_E enSrc);
const HI_CHAR *GetEdidTypeName(const HI_MW_HDMIRX_EDID_TYPE_E enEdidType);
const HI_CHAR *GetSndChannelName(const HI_MW_AUDIO_CHANNEL_E enChannel);
const HI_CHAR *GetOutportName(const HI_MW_SND_OUTPORT_E enOutPort);
const HI_CHAR *GetSndMuteTypeName(const HI_MW_SND_MUTE_TYPE_E enType);
const HI_CHAR *GetHdmiDataTypeName(const HI_MW_HDMI_DATA_TYPE_E enType);
const HI_CHAR *GetHdmiFormatName(const HI_MW_HDMI_FORMAT_E enType);
const HI_CHAR *GetWinTypeName(const HI_MW_WINDOW_E enType);
const HI_CHAR *GetScartFormatName(const HI_MW_SCART_SIGNAL_FORMAT_E enType);
const HI_CHAR *GetAspectName(const HI_MW_ASPECT_E enType);
const HI_CHAR *GetAspectCfgTypeName(const HI_MW_ASPECT_CONFIG_TYPE_E enAspectCfgType);
const HI_CHAR *GetClrSpaceName(const HI_MW_COLOR_SPACE_E enType);
const HI_CHAR *GetClrSpaceNameSubOne(const HI_MW_COLOR_SPACE_E enType);

const HI_CHAR *GetClrSysName(const HI_MW_COLOR_SYS_E enType);
const HI_CHAR *GetVideoFormatName(const HI_MW_VIDEO_FORMAT_E enType);
const HI_CHAR *GetBitWidthName(const HI_MW_PIXEL_BITWIDTH_E enType);
const HI_CHAR *GetOverSampleName(const HI_MW_OVERSAMPLE_MODE_E enType);
const HI_CHAR *GetFramPackingName(const HI_MW_FRAME_PACKING_TYPE_E enType);
const HI_CHAR *GetSampleRateName(const HI_MW_SAMPLE_RATE_E enType);
const HI_CHAR *GetSignalStatusName(HI_MW_SIGNAL_E enType);
const HI_CHAR *GetPqHdrName(HI_MW_PQ_HDR_TYPE_E enType);
const HI_CHAR *GetTrackModeName(const HI_MW_TRACK_MODE_E enType);
const HI_CHAR *GetTvdWorkModeName(HI_MW_TVD_WORK_MODE_E enType);
const HI_CHAR *GetVGAAdjustTypeName(HI_MW_VGA_ADJUST_E enType);
const HI_CHAR *GetHdmiRcpKeyName(HI_MW_HDMIRX_RCP_KEY_E enType);
const HI_CHAR *GetHdmiAudFmtName(HI_MW_HDMI_AUDIO_FMT_E enType);
const HI_CHAR *GetMhlSyncStatus(HI_MW_MHL_E enType);
const HI_CHAR *GetVICscTypeName(HI_MW_VICAP_CSC_TYPE_E enType);
const HI_CHAR *GetFreezModeName(HI_MW_FREEZE_E enType);
const HI_CHAR *GetWinFreezeTypeName(HI_MW_VIDEO_FREEZE_TYPE_E enType);
const HI_CHAR *GetAtvSystemName(const HI_MW_ATV_SYSTEM_E enType);
const HI_CHAR *GetAudSysName(HI_MW_AUDSYS_E enType);
const HI_CHAR *GetCountryName(HI_MW_COUNTRY_E enType);
const HI_CHAR *GetTvSystemName(HI_MW_TVSYSTEM_E enType);
const HI_CHAR *GetWakeupModeName(HI_MW_SYSTEM_WAKEUP_E enType);
const HI_CHAR *GetDemoModeName(HI_MW_DEMO_MODE_E enType);
const HI_CHAR *GetPqModuleName(HI_MW_PQ_MODULE_E enType);
const HI_CHAR *GetMwVerInfo(HI_VOID);
const HI_CHAR *GetMtsName(HI_MW_SIF_OUTMODE_E enSifSoundMode);
const HI_CHAR *GetAaosName(HI_MW_SIF_AAOS_MODE_E enAaosMode);
/* 将0-15的十进制数用二进制显示 */
const HI_CHAR *GetBinary(HI_U8 u8Decimal);
const HI_CHAR *GetMwTimName(HI_MW_HDMITX_FMT_E enMwFormat);
const HI_CHAR *GetClrFormatName(HI_MW_HDMITX_COLOR_FORMAT_E enClrFormat);
HI_S32 SafeSrcCheck(HI_MW_SRC_E src);
HI_S32 MakeDirent(string path);
HI_S32 RemoveDirent(const string &path);
HI_BOOL IsTifCecEnable();

#endif
