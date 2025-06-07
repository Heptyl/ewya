#ifndef XBH_CHIP_TAS5805M_FILE_PARSE_H
#define XBH_CHIP_TAS5805M_FILE_PARSE_H

#include "XbhType.h"

#define AMP_MAX_REG_NUM                     0xB7
#define MAX_FILE_SIZE                       (512 * 1024)
#define AUDIO_INI_TAS5805M_INIT_REG          "g_TAS5805M_init_reg"
#define AUDIO_INI_TAS5805M_RAM1_TAB          "g_TAS5805M_ram1_tab"
#define AUDIO_INI_TAS5805M_RAM2_TAB          "g_TAS5805M_ram2_tab"

typedef struct
{
    XBH_U8 addr;
    XBH_U8 size;
    XBH_U8 reg[20];
    XBH_U8 delay_ms;
} amp5805RegMap;

static XBH_S32 Is5805NewLine(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '\n' || cOsalChar == '\r') ? XBH_TRUE : XBH_FALSE);
}

static XBH_S32 Is5805End(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '\0') ? XBH_TRUE : XBH_FALSE);
}

static XBH_S32 Is5805LeftBracket(XBH_U8 cOsalChar)
{
    return ((cOsalChar == '{') ? XBH_TRUE : XBH_FALSE);
}

#endif
