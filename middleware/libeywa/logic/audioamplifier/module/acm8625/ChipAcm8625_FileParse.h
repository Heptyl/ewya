#ifndef XBH_CHIP_ACM8625_FILE_PARSE_H
#define XBH_CHIP_ACM8625_FILE_PARSE_H

#include "XbhType.h"

#define AMP_MAX_REG_NUM                     0xB7
#define MAX_FILE_SIZE                       (512 * 1024)
#define AUDIO_INI_ACM8625_INIT_REG          "g_acm8625_init_reg"
#define AUDIO_INI_ACM8625_RAM1_TAB          "g_acm8625_ram1_tab"
#define AUDIO_INI_ACM8625_RAM2_TAB          "g_acm8625_ram2_tab"

typedef struct
{
    XBH_U8 addr;
    XBH_U8 size;
    XBH_U8 reg[20];
    XBH_U8 delay_ms;
} amp8625RegMap;

static XBH_S32 Is8625NewLine(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '\n' || cOsalChar == '\r') ? XBH_TRUE : XBH_FALSE);
}

static XBH_S32 Is8625End(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '\0') ? XBH_TRUE : XBH_FALSE);
}

static XBH_S32 Is8625LeftBracket(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '{') ? XBH_TRUE : XBH_FALSE);
}

#endif
