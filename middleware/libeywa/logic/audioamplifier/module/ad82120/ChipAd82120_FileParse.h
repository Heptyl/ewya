#ifndef XBH_CHIP_AD82120_FILE_PARSE_H
#define XBH_CHIP_AD82120_FILE_PARSE_H

#include "XbhType.h"

#define CFADDR    0x1d
#define A1CF1     0x1e
#define A1CF2     0x1f
#define A1CF3     0x20
#define A1CF4     0x21
#define CFUD      0x32
#define AMP_MAX_REG_NUM                     0xB7
#define MAX_FILE_SIZE                       (512 * 1024)
#define AUDIO_INI_AD82120_INIT_REG          "g_ad82120_init_reg"
#define AUDIO_INI_AD82120_RAM1_TAB          "g_ad82120_ram1_tab"
#define AUDIO_INI_AD82120_RAM2_TAB          "g_ad82120_ram2_tab"
#define AUDIO_INI_AD82120_BASS_INIT_REG          "g_ad82120_bass_init_reg"
#define AUDIO_INI_AD82120_BASS_RAM1_TAB          "g_ad82120_bass_ram1_tab"
#define AUDIO_INI_AD82120_BASS_RAM2_TAB          "g_ad82120_bass_ram2_tab"
#define XBH_AUDIO_AQ_STR_SIZE_43_INI_FILE               "/vendor/firmware/aq/audio_benq_43.ini"
#define XBH_AUDIO_AQ_STR_SIZE_55_INI_FILE               "/vendor/firmware/aq/audio_benq_55.ini"
#define XBH_AUDIO_AQ_STR_SIZE_65_INI_FILE               "/vendor/firmware/aq/audio_benq_65.ini"
typedef struct
{
    XBH_U8 addr;
    XBH_U8 size;
    XBH_U8 reg[20];
    XBH_U8 delay_ms;
} ampRegMap;

static int g_ad82120_eq_reg[] = {CFADDR, A1CF1, A1CF2, A1CF3, A1CF4, CFUD};

static XBH_S32 IsNewLine(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '\n' || cOsalChar == '\r') ? XBH_TRUE : XBH_FALSE);
}

static XBH_S32 IsEnd(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '\0') ? XBH_TRUE : XBH_FALSE);
}

static XBH_S32 IsLeftBracket(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '{') ? XBH_TRUE : XBH_FALSE);
}

#endif
