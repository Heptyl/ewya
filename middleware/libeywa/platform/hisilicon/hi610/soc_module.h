/*
 * Copyright (c) CompanyNameMagicTag 2009-2019. All rights reserved.
 * Description: Module ID definition.
 */

#ifndef __SOC_MODULE_H__
#define __SOC_MODULE_H__

#define SOC_INVALID_MODULE_ID (0xffffffff)
#define SOC_MAX_MODULE_NUM 256

typedef enum {
    /* common */ /* CNcomment: 系统通用模块 */
    SOC_ID_SYS = 0x00,
    SOC_ID_MODULE,
    SOC_ID_LOG,
    SOC_ID_PROC,
    SOC_ID_MEM,
    SOC_ID_STAT,
    SOC_ID_PDM,
    SOC_ID_MEMDEV,
    SOC_ID_MDDRC,
    SOC_ID_MAILBOX,
    SOC_ID_RM, /* Resource management */
    SOC_ID_PM, /* Power management */
    SOC_ID_DVFS,
    SOC_ID_RUNTIME, /* Run time check */
    SOC_ID_SPREAD,
    SOC_ID_CUSTOM, /* Customize */

    /* Peripheral */ /* CNcomment: 外设相关模块 */
    SOC_ID_FLASH = 0x10,
    SOC_ID_IR,
    SOC_ID_I2C,
    SOC_ID_GPIO,
    SOC_ID_GPIO_I2C,
    SOC_ID_PWM,
    SOC_ID_LSADC,
    SOC_ID_SPI,
    SOC_ID_KEYLED,
    SOC_ID_WDG,
    SOC_ID_CI,
    SOC_ID_SCI,
    SOC_ID_BEIDOU,
    SOC_ID_BT,
    SOC_ID_FRONTEND,
    SOC_ID_SENSORHUB,
    SOC_ID_ECHUB,

    /* event */ /* CNcomment: 故障事件用相关模块 */
    SOC_ID_EVENT = 0x2b,

    /* Demux */ /* CNcomment: 解复用相关模块 */
    SOC_ID_DEMUX = 0x2c,

    /* Security */ /* CNcomment: 安全相关模块 */
    SOC_ID_OTP = 0x30,
    SOC_ID_KLAD,
    SOC_ID_KEYSLOT,
    SOC_ID_CIPHER,
    SOC_ID_TSR2RCIPHER,
    SOC_ID_CERT,
    SOC_ID_TSIO,
    SOC_ID_SSM,
    SOC_ID_VMX_ULTRA,
    SOC_ID_CASIMAGE,
    SOC_ID_HDCP,
    SOC_ID_RPMB,

    /* Audio */ /* CNcomment: 音频相关模块 */
    SOC_ID_SIF = 0x40,
    SOC_ID_AIAO,
    SOC_ID_AI,
    SOC_ID_AENC,
    SOC_ID_ADEC,
    SOC_ID_AFLT, /* HIFI */
    SOC_ID_ADSP,
    SOC_ID_ASR,
    SOC_ID_TTS,
    SOC_ID_AO,
    SOC_ID_AMP,
    SOC_ID_EARCTX,

    /* Video and input/output */ /* CNcomment: 视频及输入输出相关模块 */
    SOC_ID_VFE = 0x50,
    SOC_ID_TVD,
    SOC_ID_HDDEC,
    SOC_ID_VBI,
    SOC_ID_VICAP,
    SOC_ID_VI,
    SOC_ID_VENC,
    SOC_ID_VFMW,
    SOC_ID_VDEC,
    SOC_ID_PQ,
    SOC_ID_MEMC,
    SOC_ID_FRC,
    SOC_ID_VPSS,
    SOC_ID_VPLUGIN,
    SOC_ID_WIN,
    SOC_ID_DISP,
    SOC_ID_HDMIRX,
    SOC_ID_HDMITX,
    SOC_ID_PANEL,
    SOC_ID_MIPI,
    SOC_ID_FDMNG, /* 3D detect */
    SOC_ID_DISPMNG,
    SOC_ID_DISPCTRL,
    SOC_ID_DMCU,
    SOC_ID_DPRX,
    SOC_ID_GPMI,

    SOC_ID_OMXVENC = 0x6A,
    SOC_ID_OMXVDEC = 0x6B,
    SOC_ID_VAVDEC = 0x6C,

    /* Graphics */ /* CNcomment: 图形相关模块 */
    SOC_ID_FB = 0x70,
    SOC_ID_GPU,
    SOC_ID_TDE,
    SOC_ID_GFX2D,
    SOC_ID_JPGDEC,
    SOC_ID_JPGENC,
    SOC_ID_PNG,
    SOC_ID_FUX,
    SOC_ID_DRM,

    /* Player */ /* CNcomment: 播放器相关模块 */
    SOC_ID_AVPLAY = 0x80,
    SOC_ID_SYNC,
    SOC_ID_VSYNC,
    SOC_ID_ASYNC,
    SOC_ID_PVR,

    /* ISP definition */ /* CNcomment: ISP相关模块 */
    SOC_ID_VB = 0x86,
    SOC_ID_SCM,
    SOC_ID_ISP,
    SOC_ID_VIU,
    SOC_ID_VGS,
    SOC_ID_GDC,
    SOC_ID_PPU,
    SOC_ID_DIS,
    SOC_ID_MIPI_RX,
    SOC_ID_CIS,

    /* Component */ /* CNcomment: 组件相关模块 */
    SOC_ID_SUBT = 0x90,
    SOC_ID_TTX,
    SOC_ID_CC,
    SOC_ID_LOADER,
    SOC_ID_KARAOKE,
    SOC_ID_VP,
    SOC_ID_TOKEN,

    /* Middleware */ /* CNcomment: 中间件相关模块 */
    SOC_ID_NETFLIX = 0xA0,
    SOC_ID_TVMW,

    /* svp_npu */
    SOC_ID_SVP_NPUDEV = 0xA9,
    /* NPU */ /* CNcomment: 智能相关模块 */
    SOC_ID_NPUDEV = 0xAA,
    SOC_ID_AICPU = 0xAB,
    SOC_ID_NPUDFX = 0xAC,
    SOC_ID_TSFW = 0xAD,

    /* User definition */ /* CNcomment: 为上层应用保留的自定义区 */
    SOC_ID_USR = 0xB0,

    SOC_ID_MAX = 0xFF
} soc_mod_id;

#endif /* __SOC_MODULE_H__ */
