/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include <stdlib.h>
#include <string.h>
#include "opts.h"
//------------------------------------------------------------------------------
void del_line_comment(char *s)
{
	int i, len;

	len = strlen(s);
	for (i=0; i<len; i++) {
		if (s[i] == ';') {
			s[i] = '\0';
			break;
		}
	}
}
//------------------------------------------------------------------------------
static void del_crlf(char *s)
{
	int i, len;

	len = strlen(s);
	for (i=0; i<len; i++) {
		if (s[i] == '\r' || s[i] == '\n') {
			s[i] = '\0';
		}
	}
}
//------------------------------------------------------------------------------
//
// main command & option
//
//------------------------------------------------------------------------------
static void opts_main_usage_reg(void)
{
	MES("    reg:      select I2C channel");
	MES("              <0|1|2|3>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_preset(void)
{
      // 0        1         2         3         4         5         6         7         8
      // 12345678901234567890123456789012345678901234567890123456789012345678901234567890
	MES("    preset: preset video/audio settings before executing set command");
	MES("              video <port> tx|rx <resolution> <fps> <color format> <deep color>");
	MES("                    [<cf detail>] [dvi_mode]");
	MES("              audio <port> tx|rx <if_cc> <if_sf> <if_ss> <if_ca> <if_lsv>");
	MES("                    <cb_sf> <cb_ss> <ratio>");
	MES("                    <i2s_em> <i2s_po> <i2s_if> <i2s_sp>");
	MES("              edid  <filename>");
	MES("              skew lvds tx <ch#> <a> <b> <c> <d> <e> <clk>");
	MES("              chip  <0|1|2|3>");
	MES("              bus   <0|1>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_config(void)
{
	MES("    config: configure a port");
	MES("              <port> tx|rx retry <cdrlock|hotplug|plllock|initauth>");
	MES("                                 <times> <interval>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_init(void)
{
	MES("    init:   initialize a port");
	MES("              <port> tx|rx [<init opt>]");
}
//------------------------------------------------------------------------------
static void opts_main_usage_read(void)
{
	MES("    read:   read data from DDC (HDMI Tx)");
	MES("              edid");
	MES("              scdc <offset addr>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_write(void)
{
	MES("    write:  write data to DDC (HDMI Rx)");
	MES("              edid sram");
	MES("              edid via hdmi tx");
	MES("              scdc <offset addr> <data>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_set(void)
{
	MES("    set:    set preset parameters");
	MES("              video <port> tx|rx");
	MES("              audio <port> [tx|rx]");
	MES("              hdcp tx|rx [14|22|auto] [repeater]");
	MES("                  * [14|22|auto] [repeater]: rx only");
	MES("              spd infoframe [disable|pass]");
	MES("              hdr infoframe [disable|pass]");
	MES("              skew lvds tx");
	MES("              vcdb hdmi tx <byte#3>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_get(void)
{
	MES("    get:    get parameters");
	MES("              video <port> rx");
	MES("              audio <port> rx");
	MES("              status <hpd|hdcp|port|av> [tx|rx]");
	MES("              format <hdmi|vbo> rx");
	MES("              spd infoframe");
	MES("              hdr infoframe");
	MES("              skew lvds tx");
}
//------------------------------------------------------------------------------
static void opts_main_usage_reset(void)
{
	MES("    reset:  reset a function");
	MES("              hdcp tx|rx");
}
//------------------------------------------------------------------------------
static void opts_main_usage_select(void)
{
	MES("    select: select HDMI Rx port number");
	MES("            <port_num>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_mute(void)
{
	MES("    mute:   mute video/audio");
	MES("              video <port> tx [avmute|black]");
	MES("              audio <port> tx");
}
//------------------------------------------------------------------------------
static void opts_main_usage_unmute(void)
{
	MES("    unmute: unmute video/audio");
	MES("              video <port> tx [avmute|black]");
	MES("              audio <port> tx");
}
//------------------------------------------------------------------------------
static void opts_main_usage_abort(void)
{
	MES("    abort:  abort API processing");
	MES("              <hdmi|vbo|lvds|hdcp> tx|rx");
}
//------------------------------------------------------------------------------
static void opts_main_usage_sleep(void)
{
	MES("    sleep:  sleep msec");
	MES("              <msec>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_time(void)
{
	MES("    time:   show monotonic time since \"time clear\"");
}
//------------------------------------------------------------------------------
static void opts_main_usage_auto(void)
{
	MES("    auto:   auto processing");
	MES("              hdmi rx sink <port_num>");
	MES("              hdmi tx sg <resolution> [dis_auto_unmute]");
	MES("              hdmi tx hdcp [dis_auto_unmute]");
}
//------------------------------------------------------------------------------
static void opts_main_usage_exit(void)
{
	MES("    exit:   exit a port");
	MES("              <port> tx|rx");
}
//------------------------------------------------------------------------------
static void opts_main_usage_help(void)
{
	MES("    help:   show usage");
	MES("              [preset|init|read|write|set|get|reset|select|mute|unmute|sleep|auto");
	MES("               |exit|help|quit|option]");
	MES("              fig hdmi");
}
//------------------------------------------------------------------------------
static void opts_main_usage_quit(void)
{
	MES("    quit:   quit this console");
}
//------------------------------------------------------------------------------
static void opts_main_usage_option(void)
{
      // 0        1         2         3         4         5         6         7         8
      // 12345678901234567890123456789012345678901234567890123456789012345678901234567890
	MES("    port:          <hdmi|vbo|lvds|spdif|i2s>");
	MES("    init opt:      [inv_on|inv_off|vesa8|vesa10|jeida8|jeida10|composite]");
	MES("    resolution:    <uhd|fhd|fhdi|dci4k|fwxga|720|480|480i|576|576i|640x400");
	MES("                    |720x400|vga|svga|848x480|xga|wxga|wsxga|qvga|sxga|1360x768");
	MES("                    |sxgap|wxgap|uxga|wsxgap|wuxga|1792x1344|1856x1392|1920x1440");
	MES("                    |qxga|wqhd|1920x2160>");
	MES("                    uhd:       3840x2160    fwxga:     1366x 768");
	MES("                    fhd, fhdi: 1920x1080    720:       1280x 720");
	MES("                    dci4k:     4096x2160    480, 480i:  720x 480");
	MES("                    576, 576i:  720x 576    640x400:    640x 400");
	MES("                    720x400:    720x 400    vga:        640x 480");
	MES("                    svga:       800x 600    848x480:    848x 480");
	MES("                    xga:       1024x 768    wxga:      1280x 768");
	MES("                    wsxga:     1280x 800    qvga:      1280x 960");
	MES("                    sxga:      1280x1024    1360x768:  1360x 768");
	MES("                    sxgap:     1400x1050    wxgap:     1440x 900");
	MES("                    uxga:      1600x1200    wsxgap:    1680x1050");
	MES("                    wuxga:     1920x1200    1792x1344: 1792x1344");
	MES("                    1856x1392: 1856x1392    1920x1440: 1920x1440");
	MES("                    qxga:      2048x1536    wqhd:      2560x1440");
	MES("                    1920x2160: 1920x2160    fhdpc:     1980x1080");
	MES("    fps:           <24p|25p|30p|50p|60p|60prb|120p|120prb|56p|65p|70p|72p|75p");
	MES("                    |85p|144p|240p>");
	MES("    color format:  <420|422|444|rgb>");
	MES("    color depth:    <8|10|12>");
	MES("    cf detail:     [ycbcr601|ycbcr709|ycbcr2020|xvycc601|xvycc709|full1|full2");
	MES("                    |limited|rgb2020]");
	MES("    if_cc:         channel count of audio infoframe");
	MES("                     <rsh|2|3|4|5|6|7|8>");
	MES("    if_sf:         sampling frequency of audio infoframe");
	MES("    cb_sf:         sampling frequency of audio channel status bit");
	MES("                     <rsh|32|44.1|48|88.2|96|176.4|192|64|128|256|352.8");
	MES("                      |384|512|705.6|768|1024|1411.2|1536>");
	MES("    if_ss:         sample size of audio infoframe");
	MES("    cb_ss:         sample word length of audio channel status bit");
	MES("                     <rsh|16|20|24>");
	MES("    if_ca:         channel number of audio infoframe");
	MES("                     [0x]XX (X=0123456789abcdef)");
	MES("    if_lsv:        level shift value of audio infoframe");
	MES("                     [0x]X  (X=0123456789abcdef)");
	MES("    ratio:         down-/over-sampling mode for L-PCM");
	MES("                     <1/4|1/2|x1|x2|x4>");
	MES("    i2s_em:        i2s audio endian mode            <msb|lsb>");
	MES("    i2s_po:        i2s audio polarity LR            <low|high>");
	MES("    i2s_if:        i2s audio I2S format             <disable|enable>");
	MES("    i2s_sp:        i2s audio data start position    <right|left>");
	MES("    port_num:      <0|1|2|3>");
}
//------------------------------------------------------------------------------
static void opts_main_usage_fig_hdmi(void)
{
	MES("HDMI period:");
	MES("");
	MES("                           |               HPE          |"  );
	MES("         Vsync             |<-------------------------->|"  );
	MES("           |            -->| |<--HSW                    |"  );
	MES("           |                                             "  );
	MES("     VSW   |     Hsync ----+ +--------------------------+ +");
	MES("      |    |               | |                          | |");
	MES("  -,--`--  +--+  --,-      +-+                          +-+");
	MES("   |          |    |                                       ");
	MES("   | -,--  +--+    |VSD    |<-HSD->|<-     HDE     ->|   ");
	MES("   |  |    |       |                                     ");
	MES("   |       |     --;-------------  +-----------------+   ");
	MES("   |       |       |               |                 |   ");
	MES("   |VPE    |       |VDE            |    Effective    |   ");
	MES("   |       |       |               |    pixels       |   ");
	MES("   |       |       |               |                 |   ");
	MES("   |       |     --`-------------  +-----------------+   ");
	MES("   |       |                                             ");
	MES("  -`------ +--+                                          ");
	MES("              |                                          ");
	MES("           +--+                                          ");
	MES("");
}
//------------------------------------------------------------------------------
static void opts_main_usage_fig(cmds_main *cmd)
{
	if (cmd->opts.port == OPT_PORT_HDMI) {
		opts_main_usage_fig_hdmi();
	}
}
//------------------------------------------------------------------------------
static void opts_main_usage_all(void)
{
      // 0        1         2         3         4         5         6         7         8
      // 12345678901234567890123456789012345678901234567890123456789012345678901234567890
	MES("usage: <command> [options]");
	MES("  command: <reg|preset|config|init|read|write|set|get|reset|select|mute|unmute");
	MES("            |sleep|time|auto|exit|help|quit|i2c>");
	opts_main_usage_reg();
	opts_main_usage_preset();
	opts_main_usage_config();
	opts_main_usage_init();
	opts_main_usage_read();
	opts_main_usage_write();
	opts_main_usage_set();
	opts_main_usage_get();
	opts_main_usage_reset();
	opts_main_usage_select();
	opts_main_usage_mute();
	opts_main_usage_unmute();
	opts_main_usage_sleep();
	opts_main_usage_time();
	opts_main_usage_auto();
	opts_main_usage_exit();
	opts_main_usage_help();
	opts_main_usage_quit();
	MES("  options: ");
	opts_main_usage_option();
	MES("  sub-command: ");
	MES("    i2c: I2C free access");
}
//------------------------------------------------------------------------------
void opts_main_usage(cmds_main *cmd)
{
	if (cmd == NULL) {
		opts_main_usage_all();
		return;
	}
	if (cmd->id == CMD_MAIN_ID_HELP) {
		switch  (cmd->opts.id_help) {
			case CMD_MAIN_ID_NONE:		opts_main_usage_all();		break;
			case CMD_MAIN_ID_UNKNOWN:	opts_main_usage_help();		break;
			case CMD_MAIN_ID_REG:		opts_main_usage_reg();		break;
			case CMD_MAIN_ID_PRESET:	opts_main_usage_preset();	break;
			case CMD_MAIN_ID_CONFIG:	opts_main_usage_config();	break;
			case CMD_MAIN_ID_INIT:		opts_main_usage_init();		break;
			case CMD_MAIN_ID_READ:		opts_main_usage_read();		break;
			case CMD_MAIN_ID_WRITE:		opts_main_usage_write();	break;
			case CMD_MAIN_ID_SET:		opts_main_usage_set();		break;
			case CMD_MAIN_ID_GET:		opts_main_usage_get();		break;
			case CMD_MAIN_ID_RESET:		opts_main_usage_reset();	break;
			case CMD_MAIN_ID_SELECT:	opts_main_usage_select();	break;
			case CMD_MAIN_ID_MUTE:		opts_main_usage_mute();		break;
			case CMD_MAIN_ID_UNMUTE:	opts_main_usage_unmute();	break;
			case CMD_MAIN_ID_ABORT:		opts_main_usage_abort();	break;
			case CMD_MAIN_ID_SLEEP:		opts_main_usage_sleep();	break;
			case CMD_MAIN_ID_TIME:		opts_main_usage_time();		break;
			case CMD_MAIN_ID_AUTO:		opts_main_usage_auto();		break;
			case CMD_MAIN_ID_EXIT:		opts_main_usage_exit();		break;
			case CMD_MAIN_ID_HELP:		opts_main_usage_help();		break;
			case CMD_MAIN_ID_QUIT:		opts_main_usage_quit();		break;
			case CMD_MAIN_ID_OPTION:	opts_main_usage_option();	break;
			case CMD_MAIN_ID_FIG:		opts_main_usage_fig(cmd);	break;
			default:												break;
		}
	}
}
//------------------------------------------------------------------------------
static void parse_main_id(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "reg"))			cmd->id = CMD_MAIN_ID_REG;
	else if (!strcmp(tok, "preset"))	cmd->id = CMD_MAIN_ID_PRESET;
	else if (!strcmp(tok, "config"))	cmd->id = CMD_MAIN_ID_CONFIG;
	else if (!strcmp(tok, "init"))		cmd->id = CMD_MAIN_ID_INIT;
	else if (!strcmp(tok, "read"))		cmd->id = CMD_MAIN_ID_READ;
	else if (!strcmp(tok, "write"))		cmd->id = CMD_MAIN_ID_WRITE;
	else if (!strcmp(tok, "set"))		cmd->id = CMD_MAIN_ID_SET;
	else if (!strcmp(tok, "get"))		cmd->id = CMD_MAIN_ID_GET;
	else if (!strcmp(tok, "reset"))		cmd->id = CMD_MAIN_ID_RESET;
	else if (!strcmp(tok, "select"))	cmd->id = CMD_MAIN_ID_SELECT;
	else if (!strcmp(tok, "mute"))		cmd->id = CMD_MAIN_ID_MUTE;
	else if (!strcmp(tok, "unmute"))	cmd->id = CMD_MAIN_ID_UNMUTE;
	else if (!strcmp(tok, "abort"))		cmd->id = CMD_MAIN_ID_ABORT;
	else if (!strcmp(tok, "sleep"))		cmd->id = CMD_MAIN_ID_SLEEP;
	else if (!strcmp(tok, "time"))		cmd->id = CMD_MAIN_ID_TIME;
	else if (!strcmp(tok, "auto"))		cmd->id = CMD_MAIN_ID_AUTO;
	else if (!strcmp(tok, "misc"))		cmd->id = CMD_MAIN_ID_MISC;
	else if (!strcmp(tok, "exit"))		cmd->id = CMD_MAIN_ID_EXIT;
	else if (!strcmp(tok, "help"))		cmd->id = CMD_MAIN_ID_HELP;
	else if (!strcmp(tok, "quit"))		cmd->id = CMD_MAIN_ID_QUIT;
	else if (!strcmp(tok, "i2c"))		cmd->id = CMD_MAIN_ID_SUB_I2C;
	// shortcut command
	else if (!strcmp(tok, "q"))			cmd->id = CMD_MAIN_ID_QUIT;
	else if (!strcmp(tok, "0"))			cmd->id = CMD_MAIN_ID_SUB_I2C;
	else								cmd->id = CMD_MAIN_ID_UNKNOWN;
}
//------------------------------------------------------------------------------
static void parse_target(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "video"))			cmd->opts.target = OPT_TARGET_VIDEO;
	else if (!strcmp(tok, "audio"))		cmd->opts.target = OPT_TARGET_AUDIO;
	else if (!strcmp(tok, "edid"))		cmd->opts.target = OPT_TARGET_EDID;
	else if (!strcmp(tok, "chip"))		cmd->opts.target = OPT_TARGET_CHIP;
	else if (!strcmp(tok, "bus"))		cmd->opts.target = OPT_TARGET_BUS;
	else if (!strcmp(tok, "scdc"))		cmd->opts.target = OPT_TARGET_SCDC;
	else if (!strcmp(tok, "hdcp"))		cmd->opts.target = OPT_TARGET_HDCP;
	else if (!strcmp(tok, "spd"))		cmd->opts.target = OPT_TARGET_SPD_IF;
	else if (!strcmp(tok, "hdr"))		cmd->opts.target = OPT_TARGET_HDR_IF;
	else if (!strcmp(tok, "status"))	cmd->opts.target = OPT_TARGET_STATUS;
	else if (!strcmp(tok, "format"))	cmd->opts.target = OPT_TARGET_FORMAT;
	else if (!strcmp(tok, "retry"))		cmd->opts.target = OPT_TARGET_RETRY;
	else if (!strcmp(tok, "clear"))		cmd->opts.target = OPT_TARGET_CLEAR;
	else if (!strcmp(tok, "skew"))		cmd->opts.target = OPT_TARGET_SKEW;
	else if (!strcmp(tok, "vcdb"))		cmd->opts.target = OPT_TARGET_VCDB;
}
//------------------------------------------------------------------------------
static void parse_target2(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "sram"))			cmd->opts.target2 = OPT_TARGET2_SRAM;
	else if (!strcmp(tok, "via"))		cmd->opts.target2 = OPT_TARGET2_VIA;
	else if (!strcmp(tok, "cdrlock"))	cmd->opts.target2 = OPT_TARGET2_CDR_LOCK;
	else if (!strcmp(tok, "hotplug"))	cmd->opts.target2 = OPT_TARGET2_HOT_PLUG;
	else if (!strcmp(tok, "plllock"))	cmd->opts.target2 = OPT_TARGET2_PLL_LOCK;
	else if (!strcmp(tok, "initauth"))	cmd->opts.target2 = OPT_TARGET2_INIT_AUTH;
	else if (!strcmp(tok, "sg"))		cmd->opts.target2 = OPT_TARGET2_SG;
	else if (!strcmp(tok, "hdcp"))		cmd->opts.target2 = OPT_TARGET2_HDCP;
	else if (!strcmp(tok, "sink"))		cmd->opts.target2 = OPT_TARGET2_SINK;
}
//------------------------------------------------------------------------------
static void parse_port(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "hdmi"))			cmd->opts.port = OPT_PORT_HDMI;
	else if (!strcmp(tok, "vbo"))		cmd->opts.port = OPT_PORT_VBO;
	else if (!strcmp(tok, "lvds"))		cmd->opts.port = OPT_PORT_LVDS;
	else if (!strcmp(tok, "spdif"))		cmd->opts.port = OPT_PORT_SPDIF;
	else if (!strcmp(tok, "i2s"))		cmd->opts.port = OPT_PORT_I2S;
	else if (!strcmp(tok, "hdcp"))		cmd->opts.port = OPT_PORT_HDCP;
}
//------------------------------------------------------------------------------
static void parse_stat(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "hpd"))			cmd->opts.stat = OPT_STAT_HPD;
	else if (!strcmp(tok, "hdcp"))		cmd->opts.stat = OPT_STAT_HDCP;
	else if (!strcmp(tok, "port"))		cmd->opts.stat = OPT_STAT_PORT;
	else if (!strcmp(tok, "av"))		cmd->opts.stat = OPT_STAT_AV;
}
//------------------------------------------------------------------------------
static void parse_txrx(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "tx"))				cmd->opts.txrx = OPT_TXRX_TX;
	else if (!strcmp(tok, "rx"))		cmd->opts.txrx = OPT_TXRX_RX;
}
//------------------------------------------------------------------------------
static void parse_resolution(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "uhd"))			cmd->opts.resolution = OPT_RESOLUTION_UHD;
	else if (!strcmp(tok, "fhd"))		cmd->opts.resolution = OPT_RESOLUTION_FHD;
	else if (!strcmp(tok, "fhdi"))		cmd->opts.resolution = OPT_RESOLUTION_FHDI;
	else if (!strcmp(tok, "dci4k"))		cmd->opts.resolution = OPT_RESOLUTION_DCI4K;
	else if (!strcmp(tok, "fwxga"))		cmd->opts.resolution = OPT_RESOLUTION_FWXGA;
	else if (!strcmp(tok, "720"))		cmd->opts.resolution = OPT_RESOLUTION_720;
	else if (!strcmp(tok, "480"))		cmd->opts.resolution = OPT_RESOLUTION_480;
	else if (!strcmp(tok, "480i"))		cmd->opts.resolution = OPT_RESOLUTION_480I;
	else if (!strcmp(tok, "576"))		cmd->opts.resolution = OPT_RESOLUTION_576;
	else if (!strcmp(tok, "576i"))		cmd->opts.resolution = OPT_RESOLUTION_576I;
	else if (!strcmp(tok, "640x400"))	cmd->opts.resolution = OPT_RESOLUTION_640X400;
	else if (!strcmp(tok, "720x400"))	cmd->opts.resolution = OPT_RESOLUTION_720X400;
	else if (!strcmp(tok, "vga"))		cmd->opts.resolution = OPT_RESOLUTION_VGA;
	else if (!strcmp(tok, "svga"))		cmd->opts.resolution = OPT_RESOLUTION_SVGA;
	else if (!strcmp(tok, "848x480"))	cmd->opts.resolution = OPT_RESOLUTION_848X480;
	else if (!strcmp(tok, "xga"))		cmd->opts.resolution = OPT_RESOLUTION_XGA;
	else if (!strcmp(tok, "xgap"))		cmd->opts.resolution = OPT_RESOLUTION_XGAP;
	else if (!strcmp(tok, "wxga"))		cmd->opts.resolution = OPT_RESOLUTION_WXGA;
	else if (!strcmp(tok, "wsxga"))		cmd->opts.resolution = OPT_RESOLUTION_WSXGA;
	else if (!strcmp(tok, "qvga"))		cmd->opts.resolution = OPT_RESOLUTION_QVGA;
	else if (!strcmp(tok, "sxga"))		cmd->opts.resolution = OPT_RESOLUTION_SXGA;
	else if (!strcmp(tok, "1360x768"))	cmd->opts.resolution = OPT_RESOLUTION_1360X768;
	else if (!strcmp(tok, "sxgap"))		cmd->opts.resolution = OPT_RESOLUTION_SXGAP;
	else if (!strcmp(tok, "wxgap"))		cmd->opts.resolution = OPT_RESOLUTION_WXGAP;
	else if (!strcmp(tok, "uxga"))		cmd->opts.resolution = OPT_RESOLUTION_UXGA;
	else if (!strcmp(tok, "wsxgap"))	cmd->opts.resolution = OPT_RESOLUTION_WSXGAP;
	else if (!strcmp(tok, "wuxga"))		cmd->opts.resolution = OPT_RESOLUTION_WUXGA;
	else if (!strcmp(tok, "1792x1344")) cmd->opts.resolution = OPT_RESOLUTION_1792x1344;
	else if (!strcmp(tok, "1856x1392"))	cmd->opts.resolution = OPT_RESOLUTION_1856x1392;
	else if (!strcmp(tok, "1920x1440"))	cmd->opts.resolution = OPT_RESOLUTION_1920x1440;
	else if (!strcmp(tok, "wqxga"))		cmd->opts.resolution = OPT_RESOLUTION_WQXGA;
	else if (!strcmp(tok, "qxga"))		cmd->opts.resolution = OPT_RESOLUTION_QXGA;
	else if (!strcmp(tok, "wqhd"))		cmd->opts.resolution = OPT_RESOLUTION_WQHD;
	else if (!strcmp(tok, "1920x2160"))	cmd->opts.resolution = OPT_RESOLUTION_1920x2160;
	else if (!strcmp(tok, "fhdpc"))		cmd->opts.resolution = OPT_RESOLUTION_FHDPC;
}
//------------------------------------------------------------------------------
static void parse_dis_auto_unmute(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "dis_auto_unmute"))	cmd->opts.data = 1;
}
//------------------------------------------------------------------------------
static void parse_fps(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "24p"))			cmd->opts.fps = OPT_FPS_24P;
	else if (!strcmp(tok, "25p"))		cmd->opts.fps = OPT_FPS_25P;
	else if (!strcmp(tok, "30p"))		cmd->opts.fps = OPT_FPS_30P;
	else if (!strcmp(tok, "50p"))		cmd->opts.fps = OPT_FPS_50P;
	else if (!strcmp(tok, "60p"))		cmd->opts.fps = OPT_FPS_60P;
	else if (!strcmp(tok, "60prb"))		cmd->opts.fps = OPT_FPS_60PRB;
	else if (!strcmp(tok, "120p"))		cmd->opts.fps = OPT_FPS_120P;
	else if (!strcmp(tok, "120prb"))	cmd->opts.fps = OPT_FPS_120PRB;
	else if (!strcmp(tok, "56p"))		cmd->opts.fps = OPT_FPS_56P;
	else if (!strcmp(tok, "65p"))		cmd->opts.fps = OPT_FPS_65P;
	else if (!strcmp(tok, "70p"))		cmd->opts.fps = OPT_FPS_70P;
	else if (!strcmp(tok, "72p"))		cmd->opts.fps = OPT_FPS_72P;
	else if (!strcmp(tok, "75p"))		cmd->opts.fps = OPT_FPS_75P;
	else if (!strcmp(tok, "85p"))		cmd->opts.fps = OPT_FPS_85P;
	else if (!strcmp(tok, "144p"))		cmd->opts.fps = OPT_FPS_144P;
	else if (!strcmp(tok, "240p"))		cmd->opts.fps = OPT_FPS_240P;
}
//------------------------------------------------------------------------------
static void parse_color_format(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "420"))			cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR420_BT709;
	else if (!strcmp(tok, "422"))		cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR422_BT709;
	else if (!strcmp(tok, "444"))		cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR444_BT709;
	else if (!strcmp(tok, "rgb"))		cmd->opts.color_format = OPT_COLOR_FORMAT_RGB_FULL1;
}
//------------------------------------------------------------------------------
static void parse_deep_color(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "8"))				cmd->opts.deep_color = OPT_DEEP_COLOR_8;
	else if (!strcmp(tok, "10"))		cmd->opts.deep_color = OPT_DEEP_COLOR_10;
	else if (!strcmp(tok, "12"))		cmd->opts.deep_color = OPT_DEEP_COLOR_12;
}
//------------------------------------------------------------------------------
static void parse_dvi_mode(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "dvi_mode")) {
		cmd->opts.dvi_mode = OPT_DVI_MODE_ENABLE;
	}
}
//------------------------------------------------------------------------------
static void parse_cf_detail(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "ycbcr601")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_YCBCR420_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR420_BT601;	break;
			case OPT_COLOR_FORMAT_YCBCR422_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR422_BT601;	break;
			case OPT_COLOR_FORMAT_YCBCR444_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR444_BT601;	break;
			default:								break;
		}
	} else if (!strcmp(tok, "ycbcr709")) {
		// same as parse_color_format()
		;
	} else if (!strcmp(tok, "ycbcr2020")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_YCBCR420_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR420_BT2020;	break;
			case OPT_COLOR_FORMAT_YCBCR422_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR422_BT2020;	break;
			case OPT_COLOR_FORMAT_YCBCR444_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_YCBCR444_BT2020;	break;
			default:								break;
		}
	} else if (!strcmp(tok, "xvycc601")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_YCBCR422_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_XVYCC422_BT601;	break;
			case OPT_COLOR_FORMAT_YCBCR444_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_XVYCC444_BT601;	break;
			default:								break;
		}
	} else if (!strcmp(tok, "xvycc709")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_YCBCR422_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_XVYCC422_BT709;	break;
			case OPT_COLOR_FORMAT_YCBCR444_BT709:	cmd->opts.color_format = OPT_COLOR_FORMAT_XVYCC444_BT709;	break;
			default:								break;
		}
	} else if (!strcmp(tok, "full1")) {
		// same as parse_color_format()
		;
	} else if (!strcmp(tok, "full2")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_RGB_FULL1:		cmd->opts.color_format = OPT_COLOR_FORMAT_RGB_FULL2;		break;
			default:								break;
		}
	} else if (!strcmp(tok, "limited")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_RGB_FULL1:		cmd->opts.color_format = OPT_COLOR_FORMAT_RGB_LIMITED;		break;
			default:								break;
		}
	} else if (!strcmp(tok, "rgb2020")) {
		switch (cmd->opts.color_format) {
			case OPT_COLOR_FORMAT_RGB_FULL1:		cmd->opts.color_format = OPT_COLOR_FORMAT_RGB_BT2020;		break;
			default:								break;
		}
	} else {
		parse_dvi_mode(tok, cmd);
	}
}
//------------------------------------------------------------------------------
static void parse_init_opt(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "inv_on"))			cmd->opts.init_opt = OPT_INIT_OPT_INVERSION_ON;
	else if (!strcmp(tok, "inv_off"))	cmd->opts.init_opt = OPT_INIT_OPT_INVERSION_OFF;
	else if (!strcmp(tok, "vesa8"))		cmd->opts.init_opt = OPT_INIT_OPT_VESA08;
	else if (!strcmp(tok, "vesa10"))	cmd->opts.init_opt = OPT_INIT_OPT_VESA10;
	else if (!strcmp(tok, "jeida8"))	cmd->opts.init_opt = OPT_INIT_OPT_JEIDA08;
	else if (!strcmp(tok, "jeida10"))	cmd->opts.init_opt = OPT_INIT_OPT_JEIDA10;
	else if (!strcmp(tok, "composite"))	cmd->opts.init_opt = OPT_INIT_OPT_COMPOSITE;
}
//------------------------------------------------------------------------------
static void parse_if_cc(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "rsh"))			cmd->opts.if_cc = OPT_CC_RSH;
	else if (!strcmp(tok, "2"))			cmd->opts.if_cc = OPT_CC_2CH;
	else if (!strcmp(tok, "3"))			cmd->opts.if_cc = OPT_CC_3CH;
	else if (!strcmp(tok, "4"))			cmd->opts.if_cc = OPT_CC_4CH;
	else if (!strcmp(tok, "5"))			cmd->opts.if_cc = OPT_CC_5CH;
	else if (!strcmp(tok, "6"))			cmd->opts.if_cc = OPT_CC_6CH;
	else if (!strcmp(tok, "7"))			cmd->opts.if_cc = OPT_CC_7CH;
	else if (!strcmp(tok, "8"))			cmd->opts.if_cc = OPT_CC_8CH;
}
//------------------------------------------------------------------------------
static void parse_if_sf(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "rsh"))			cmd->opts.if_sf = OPT_FREQ_RSH;
	else if (!strcmp(tok, "32"))		cmd->opts.if_sf = OPT_FREQ_32;
	else if (!strcmp(tok, "44.1"))		cmd->opts.if_sf = OPT_FREQ_44_1;
	else if (!strcmp(tok, "48"))		cmd->opts.if_sf = OPT_FREQ_48;
	else if (!strcmp(tok, "88.2"))		cmd->opts.if_sf = OPT_FREQ_88_2;
	else if (!strcmp(tok, "96"))		cmd->opts.if_sf = OPT_FREQ_96;
	else if (!strcmp(tok, "176.4"))		cmd->opts.if_sf = OPT_FREQ_176_4;
	else if (!strcmp(tok, "192"))		cmd->opts.if_sf = OPT_FREQ_192;
}
//------------------------------------------------------------------------------
static void parse_cb_sf(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "32"))				cmd->opts.cb_sf = OPT_FREQ_32;
	else if (!strcmp(tok, "44.1"))		cmd->opts.cb_sf = OPT_FREQ_44_1;
	else if (!strcmp(tok, "48"))		cmd->opts.cb_sf = OPT_FREQ_48;
	else if (!strcmp(tok, "88.2"))		cmd->opts.cb_sf = OPT_FREQ_88_2;
	else if (!strcmp(tok, "96"))		cmd->opts.cb_sf = OPT_FREQ_96;
	else if (!strcmp(tok, "176.4"))		cmd->opts.cb_sf = OPT_FREQ_176_4;
	else if (!strcmp(tok, "192"))		cmd->opts.cb_sf = OPT_FREQ_192;
	else if (!strcmp(tok, "64"))		cmd->opts.cb_sf = OPT_FREQ_64;
	else if (!strcmp(tok, "128"))		cmd->opts.cb_sf = OPT_FREQ_128;
	else if (!strcmp(tok, "256"))		cmd->opts.cb_sf = OPT_FREQ_256;
	else if (!strcmp(tok, "352.8"))		cmd->opts.cb_sf = OPT_FREQ_352_8;
	else if (!strcmp(tok, "384"))		cmd->opts.cb_sf = OPT_FREQ_384;
	else if (!strcmp(tok, "512"))		cmd->opts.cb_sf = OPT_FREQ_512;
	else if (!strcmp(tok, "705.6"))		cmd->opts.cb_sf = OPT_FREQ_705_6;
	else if (!strcmp(tok, "768"))		cmd->opts.cb_sf = OPT_FREQ_768;
	else if (!strcmp(tok, "1024"))		cmd->opts.cb_sf = OPT_FREQ_1024;
	else if (!strcmp(tok, "1411.2"))	cmd->opts.cb_sf = OPT_FREQ_1411_2;
	else if (!strcmp(tok, "1536"))		cmd->opts.cb_sf = OPT_FREQ_1536;
}
//------------------------------------------------------------------------------
static void parse_if_ss(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "rsh"))			cmd->opts.if_ss = OPT_SS_RSH;
	else if (!strcmp(tok, "16"))		cmd->opts.if_ss = OPT_SS_16BIT;
	else if (!strcmp(tok, "20"))		cmd->opts.if_ss = OPT_SS_20BIT;
	else if (!strcmp(tok, "24"))		cmd->opts.if_ss = OPT_SS_24BIT;
}
//------------------------------------------------------------------------------
static void parse_cb_ss(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "16"))				cmd->opts.cb_ss = OPT_SS_16BIT;
	else if (!strcmp(tok, "20"))		cmd->opts.cb_ss = OPT_SS_20BIT;
	else if (!strcmp(tok, "24"))		cmd->opts.cb_ss = OPT_SS_24BIT;
}
//------------------------------------------------------------------------------
static void parse_sf_ratio(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "1/4"))			cmd->opts.sf_ratio = OPT_FREQ_RATIO_1_4;
	else if (!strcmp(tok, "1/2"))		cmd->opts.sf_ratio = OPT_FREQ_RATIO_1_2;
	else if (!strcmp(tok, "x1"))		cmd->opts.sf_ratio = OPT_FREQ_RATIO_1_1;
	else if (!strcmp(tok, "x2"))		cmd->opts.sf_ratio = OPT_FREQ_RATIO_2_1;
	else if (!strcmp(tok, "x4"))		cmd->opts.sf_ratio = OPT_FREQ_RATIO_4_1;
}
//------------------------------------------------------------------------------
static void parse_i2s_em(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "msb"))			cmd->opts.i2s_em = OPT_EM_MSB;
	else if (!strcmp(tok, "lsb"))		cmd->opts.i2s_em = OPT_EM_LSB;
}
//------------------------------------------------------------------------------
static void parse_i2s_po(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "low"))			cmd->opts.i2s_po = OPT_PO_LOW;
	else if (!strcmp(tok, "high"))		cmd->opts.i2s_po = OPT_PO_HIGH;
}
//------------------------------------------------------------------------------
static void parse_i2s_if(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "disable"))		cmd->opts.i2s_if = OPT_IF_DISABLE;
	else if (!strcmp(tok, "enable"))	cmd->opts.i2s_if = OPT_IF_ENABLE;
}
//------------------------------------------------------------------------------
static void parse_i2s_sp(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "right"))			cmd->opts.i2s_sp = OPT_SP_RIGHT;
	else if (!strcmp(tok, "left"))		cmd->opts.i2s_sp = OPT_SP_LEFT;
}
//------------------------------------------------------------------------------
static void parse_mute_opt(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "avmute"))			cmd->opts.mute_opt = OPT_MUTE_OPT_AVMUTE;
	else if (!strcmp(tok, "black"))		cmd->opts.mute_opt = OPT_MUTE_OPT_BLACK;
}
//------------------------------------------------------------------------------
static void parse_hdcp_ver(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "14"))				cmd->opts.hdcp_ver = OPT_HDCP_VER_14;
	else if (!strcmp(tok, "22"))		cmd->opts.hdcp_ver = OPT_HDCP_VER_22;
	else if (!strcmp(tok, "auto"))		cmd->opts.hdcp_ver = OPT_HDCP_VER_AUTO;
	else if (!strcmp(tok, "repeater")) {
										cmd->opts.hdcp_ver = OPT_HDCP_VER_AUTO;
										cmd->opts.repeater = OPT_REPEATER_ON;
	}
	else								cmd->opts.hdcp_ver = OPT_HDCP_VER_AUTO;
}
//------------------------------------------------------------------------------
static void parse_rep(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "repeater"))		cmd->opts.repeater = OPT_REPEATER_ON;
	else {
		if (cmd->opts.repeater != OPT_REPEATER_ON)
										cmd->opts.repeater = OPT_REPEATER_OFF;
	}
}
//------------------------------------------------------------------------------
static void parse_info(const char *tok, cmds_main *cmd, opt_target tgt)
{
	if (!strcmp(tok, "disable"))		cmd->opts.data = 0xFF;
	else if (!strcmp(tok, "pass")) {
		if (tgt == OPT_TARGET_SPD_IF)		cmd->opts.data = 0x83;
		else if (tgt == OPT_TARGET_HDR_IF)	cmd->opts.data = 0x87;
	}
}
//------------------------------------------------------------------------------
static void parse_main_id_help(const char *tok, cmds_main *cmd)
{
	if (!strcmp(tok, "reg"))			cmd->opts.id_help = CMD_MAIN_ID_REG;
	else if (!strcmp(tok, "preset"))	cmd->opts.id_help = CMD_MAIN_ID_PRESET;
	else if (!strcmp(tok, "config"))	cmd->opts.id_help = CMD_MAIN_ID_CONFIG;
	else if (!strcmp(tok, "init"))		cmd->opts.id_help = CMD_MAIN_ID_INIT;
	else if (!strcmp(tok, "read"))		cmd->opts.id_help = CMD_MAIN_ID_READ;
	else if (!strcmp(tok, "write"))		cmd->opts.id_help = CMD_MAIN_ID_WRITE;
	else if (!strcmp(tok, "set"))		cmd->opts.id_help = CMD_MAIN_ID_SET;
	else if (!strcmp(tok, "get"))		cmd->opts.id_help = CMD_MAIN_ID_GET;
	else if (!strcmp(tok, "reset"))		cmd->opts.id_help = CMD_MAIN_ID_RESET;
	else if (!strcmp(tok, "select"))	cmd->opts.id_help = CMD_MAIN_ID_SELECT;
	else if (!strcmp(tok, "mute"))		cmd->opts.id_help = CMD_MAIN_ID_MUTE;
	else if (!strcmp(tok, "unmute"))	cmd->opts.id_help = CMD_MAIN_ID_UNMUTE;
	else if (!strcmp(tok, "abort"))		cmd->opts.id_help = CMD_MAIN_ID_ABORT;
	else if (!strcmp(tok, "sleep"))		cmd->opts.id_help = CMD_MAIN_ID_SLEEP;
	else if (!strcmp(tok, "time"))		cmd->opts.id_help = CMD_MAIN_ID_TIME;
	else if (!strcmp(tok, "auto"))		cmd->opts.id_help = CMD_MAIN_ID_AUTO;
	else if (!strcmp(tok, "exit"))		cmd->opts.id_help = CMD_MAIN_ID_EXIT;
	else if (!strcmp(tok, "help"))		cmd->opts.id_help = CMD_MAIN_ID_HELP;
	else if (!strcmp(tok, "quit"))		cmd->opts.id_help = CMD_MAIN_ID_QUIT;
	else if (!strcmp(tok, "option"))	cmd->opts.id_help = CMD_MAIN_ID_OPTION;
	else if (!strcmp(tok, "fig"))		cmd->opts.id_help = CMD_MAIN_ID_FIG;
	else								cmd->opts.id_help = CMD_MAIN_ID_UNKNOWN;
}
//------------------------------------------------------------------------------
static void parse_main_token0(const char *tok, cmds_main *cmd)
{
	parse_main_id(tok, cmd);
}
//------------------------------------------------------------------------------
static void parse_main_token1(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_REG:		cmd->opts.data = (uint8_t)strtol(tok, NULL, 10);	break;
		case CMD_MAIN_ID_PRESET:	parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_CONFIG:	parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_INIT:		parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_READ:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_WRITE:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_SET:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_GET:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_RESET:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_SELECT:	
			cmd->opts.port_num = (uint8_t)strtol(tok, NULL, 10);
																	break;
		case CMD_MAIN_ID_MUTE:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_UNMUTE:	parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_ABORT:		parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_SLEEP:		cmd->opts.msec = (uint32_t)strtoul(tok, NULL, 10);	break;
		case CMD_MAIN_ID_TIME:		parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_AUTO:		parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_EXIT:		parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_HELP:		parse_main_id_help(tok, cmd);	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token2(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:	parse_port(tok, cmd);		break;
				case OPT_TARGET_AUDIO:	parse_port(tok, cmd);		break;
				case OPT_TARGET_CHIP:	cmd->opts.chip_num = (uint8_t)strtol(tok, NULL, 10);	break;
				case OPT_TARGET_BUS:	cmd->opts.bus_num = (uint8_t)strtol(tok, NULL, 10);		break;
				case OPT_TARGET_SKEW:	parse_port(tok, cmd);		break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_CONFIG:	parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_INIT:	parse_txrx(tok, cmd);				break;
		case CMD_MAIN_ID_READ:
			switch (cmd->opts.target) {
				case OPT_TARGET_SCDC:	cmd->opts.oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_WRITE:
			switch (cmd->opts.target) {
				case OPT_TARGET_EDID:	parse_target2(tok, cmd);	break;
				case OPT_TARGET_SCDC:	cmd->opts.oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
				default:											break;
			}
		case CMD_MAIN_ID_SET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_port(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_port(tok, cmd);	break;
				case OPT_TARGET_HDCP:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_SPD_IF:								break;
				case OPT_TARGET_HDR_IF:								break;
				case OPT_TARGET_SKEW:		parse_port(tok, cmd);	break;
				case OPT_TARGET_VCDB:		parse_port(tok, cmd);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_GET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_port(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_port(tok, cmd);	break;
				case OPT_TARGET_STATUS:		parse_stat(tok, cmd);	break;
				case OPT_TARGET_FORMAT:		parse_port(tok, cmd);	break;
				case OPT_TARGET_SPD_IF:								break;
				case OPT_TARGET_HDR_IF:								break;
				case OPT_TARGET_SKEW:		parse_port(tok, cmd);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_RESET:		parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_MUTE:		parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_UNMUTE:	parse_port(tok, cmd);			break;
		case CMD_MAIN_ID_ABORT:		parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_AUTO:		parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_EXIT:		parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_HELP:		parse_port(tok, cmd);			break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token3(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_SKEW:		parse_txrx(tok, cmd);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_CONFIG:	parse_target(tok, cmd);			break;
		case CMD_MAIN_ID_INIT:		parse_init_opt(tok, cmd);		break;
		case CMD_MAIN_ID_WRITE:
			switch (cmd->opts.target) {
				case OPT_TARGET_EDID:	parse_port(tok, cmd);		break;
				case OPT_TARGET_SCDC:	cmd->opts.data = (uint8_t)strtol(tok, NULL, 16);	break;
				default:											break;
			}
		case CMD_MAIN_ID_SET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_HDCP:		parse_hdcp_ver(tok, cmd);	break;
				case OPT_TARGET_SPD_IF:		parse_info(tok, cmd, OPT_TARGET_SPD_IF);	break;
				case OPT_TARGET_HDR_IF:		parse_info(tok, cmd, OPT_TARGET_HDR_IF);	break;
				case OPT_TARGET_SKEW:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_VCDB:		parse_txrx(tok, cmd);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_GET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_STATUS:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_FORMAT:		parse_txrx(tok, cmd);	break;
				case OPT_TARGET_SKEW:		parse_txrx(tok, cmd);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_MUTE:		parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_UNMUTE:	parse_txrx(tok, cmd);			break;
		case CMD_MAIN_ID_AUTO:
			switch (cmd->opts.txrx) {
				case OPT_TXRX_TX:		parse_target2(tok, cmd);	break;
				case OPT_TXRX_RX:		parse_target2(tok, cmd);	break;
				default:											break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token4(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_resolution(tok, cmd);		break;
				case OPT_TARGET_AUDIO:		parse_if_cc(tok, cmd);			break;
				case OPT_TARGET_SKEW:		cmd->opts.data = (uint8_t)strtol(tok, NULL, 10);	break;
				default:													break;
			}
																	break;
		case CMD_MAIN_ID_CONFIG:	parse_target2(tok, cmd);		break;
		case CMD_MAIN_ID_WRITE:
			switch (cmd->opts.target) {
				case OPT_TARGET_EDID:	parse_txrx(tok, cmd);		break;
				case OPT_TARGET_SCDC:								break;
				default:											break;
			}
		case CMD_MAIN_ID_SET:
			switch (cmd->opts.target) {
				case OPT_TARGET_HDCP:		parse_rep(tok, cmd);	break;
				case OPT_TARGET_VCDB:		cmd->opts.data = (uint8_t)strtol(tok, NULL, 16);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_MUTE:		parse_mute_opt(tok, cmd);		break;
		case CMD_MAIN_ID_UNMUTE:	parse_mute_opt(tok, cmd);		break;
		case CMD_MAIN_ID_AUTO:
			switch (cmd->opts.txrx) {
				case OPT_TXRX_TX:
					switch (cmd->opts.target2) {
						case OPT_TARGET2_SG:	parse_resolution(tok, cmd);			break;
						case OPT_TARGET2_HDCP:	parse_dis_auto_unmute(tok, cmd);	break;
						default:													break;
					}
																			break;
				case OPT_TXRX_RX:
					switch (cmd->opts.target2) {
						case OPT_TARGET2_SINK:
							cmd->opts.port_num = (uint8_t)strtol(tok, NULL, 10);	break;
						default:													break;
					}
																			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token5(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_fps(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_if_sf(tok, cmd);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_CONFIG:
			switch (cmd->opts.target) {
				case OPT_TARGET_RETRY:	cmd->opts.times = (uint32_t)strtoul(tok, NULL, 10);	break;
				default:											break;
			}
																	break;
		case CMD_MAIN_ID_AUTO:
			switch (cmd->opts.txrx) {
				case OPT_TXRX_TX:
					switch (cmd->opts.target2) {
						case OPT_TARGET2_SG:	parse_dis_auto_unmute(tok, cmd);	break;
						default:													break;
					}
																			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token6(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_color_format(tok, cmd);	break;
				case OPT_TARGET_AUDIO:		parse_if_ss(tok, cmd);			break;
				default:													break;
			}
																	break;
		case CMD_MAIN_ID_CONFIG:
			switch (cmd->opts.target) {
				case OPT_TARGET_RETRY:	cmd->opts.msec = (uint32_t)strtoul(tok, NULL, 10);	break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token7(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_deep_color(tok, cmd);		break;
				case OPT_TARGET_AUDIO:
					cmd->opts.if_ca = (uint8_t)strtol(tok, NULL, 16);		break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token8(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_cf_detail(tok, cmd);		break;
				case OPT_TARGET_AUDIO:
					cmd->opts.if_lsv = (uint8_t)strtol(tok, NULL, 16);		break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token9(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_VIDEO:		parse_dvi_mode(tok, cmd);		break;
				case OPT_TARGET_AUDIO:		parse_cb_sf(tok, cmd);			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token10(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_AUDIO:		parse_cb_ss(tok, cmd);			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token11(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_AUDIO:		parse_sf_ratio(tok, cmd);		break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token12(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_AUDIO:		parse_i2s_em(tok, cmd);			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token13(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_AUDIO:		parse_i2s_po(tok, cmd);			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token14(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_AUDIO:		parse_i2s_if(tok, cmd);			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
static void parse_main_token15(const char *tok, cmds_main *cmd)
{
	switch (cmd->id) {
		case CMD_MAIN_ID_PRESET:
			switch (cmd->opts.target) {
				case OPT_TARGET_AUDIO:		parse_i2s_sp(tok, cmd);			break;
				default:													break;
			}
																	break;
		default:													break;
	}
}
//------------------------------------------------------------------------------
void opts_main_parsing(cmds_main *cmd)
{
	char buf[MAX_LINE];
	//char *tok, *str, *savep, *delim = " \n\0";
	char *tok, *str, *delim = " \r\n\0";
	int idx;

	//init
	cmd->id = CMD_MAIN_ID_NONE;
	memset(&cmd->opts, 0, sizeof(opts_main));
	memset(&cmd->tok, 0, sizeof(cmd->tok));

	strcpy(buf, cmd->str);
	del_line_comment(buf);
	del_crlf(cmd->str);
	for (str = buf, idx = 0; idx < MAX_TOK_NUM; str = NULL, idx++) {
		//cmd->tok[idx] = strtok_r(str, delim, &savep);
		tok = strtok(str, delim);
		if (tok == NULL) break;
		switch (idx) {
			case 0: parse_main_token0(tok, cmd);	break;
			case 1: parse_main_token1(tok, cmd);	break;
			case 2: parse_main_token2(tok, cmd);	break;
			case 3: parse_main_token3(tok, cmd);	break;
			case 4: parse_main_token4(tok, cmd);	break;
			case 5: parse_main_token5(tok, cmd);	break;
			case 6: parse_main_token6(tok, cmd);	break;
			case 7: parse_main_token7(tok, cmd);	break;
			case 8: parse_main_token8(tok, cmd);	break;
			case 9: parse_main_token9(tok, cmd);	break;
			case 10: parse_main_token10(tok, cmd);	break;
			case 11: parse_main_token11(tok, cmd);	break;
			case 12: parse_main_token12(tok, cmd);	break;
			case 13: parse_main_token13(tok, cmd);	break;
			case 14: parse_main_token14(tok, cmd);	break;
			case 15: parse_main_token15(tok, cmd);	break;
			default:								break;
		}
		strncpy(cmd->tok[idx], tok, MAX_TOK_SIZE);
	}
}
//------------------------------------------------------------------------------
//
// i2c command & option
//
//------------------------------------------------------------------------------
static void opts_i2c_usage_i2cr(void)
{
	MES("    i2cr:   read data from <slv>:<off> address up to [size]");
	MES("              <slv> <off> [size]");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_i2cw(void)
{
	MES("    i2cw:   write <data> and/or [data ...] to <slv>:<off>");
	MES("              <slv> <off> <data> [data ...]");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_i2cp(void)
{
	MES("    i2cp:   copy <slv1>:<off1> to <slv2>:<off2> based on <p_slv>:<p_off>");
	MES("              <slv1> <off1> <slv2> <off2> <p_slv> <p_off> <p_rd> <p_wr>");
	MES("              * <p_rd> is read setting from <slv1>:<off1>");
	MES("              * <p_wr> is write setting to <slv2>:<off2>");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_sleep(void)
{
	MES("    sleep:  sleep <time> milliseconds. * <time>: DN");
	MES("              <time>");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_wait(void)
{
	MES("    wait:   wait while((<data> & <mask>) != (i2cr(<slv>, <off>) & <mask>))");
	MES("              <slv> <off> <mask> <data>");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_help(void)
{
	MES("    help:   show usage");
	MES("              [i2cr|i2cw|i2cp|sleep|wait|help|quit]");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_quit(void)
{
	MES("    quit:   quit this console");
}
//------------------------------------------------------------------------------
static void opts_i2c_usage_all(void)
{
      // 0        1         2         3         4         5         6         7         8
      // 12345678901234567890123456789012345678901234567890123456789012345678901234567890
	MES("usage: <command> [options]");
	MES("  command: <i2cr|i2cw|i2cp|sleep|wait|help|quit>");
	opts_i2c_usage_i2cr();
	opts_i2c_usage_i2cw();
	opts_i2c_usage_i2cp();
	opts_i2c_usage_sleep();
	opts_i2c_usage_wait();
	opts_i2c_usage_help();
	opts_i2c_usage_quit();
}
//------------------------------------------------------------------------------
void opts_i2c_usage(cmds_i2c *cmd)
{
	if (cmd == NULL) {
		opts_i2c_usage_all();
		return;
	}
	if (cmd->id == CMD_I2C_ID_HELP) {
		switch (cmd->opts.id_help) {
			case CMD_I2C_ID_NONE:		opts_i2c_usage_all();		break;
			case CMD_I2C_ID_UNKNOWN:	opts_i2c_usage_help();		break;
			case CMD_I2C_ID_I2CR:		opts_i2c_usage_i2cr();		break;
			case CMD_I2C_ID_I2CW:		opts_i2c_usage_i2cw();		break;
			case CMD_I2C_ID_I2CP:		opts_i2c_usage_i2cp();		break;
			case CMD_I2C_ID_SLEEP:		opts_i2c_usage_sleep();		break;
			case CMD_I2C_ID_WAIT:		opts_i2c_usage_wait();		break;
			case CMD_I2C_ID_HELP:		opts_i2c_usage_help();		break;
			case CMD_I2C_ID_QUIT:		opts_i2c_usage_quit();		break;
			default:												break;
		}
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_id(const char *tok, cmds_i2c *cmd)
{
	if (!strcmp(tok, "i2cr"))			cmd->id = CMD_I2C_ID_I2CR;
	else if (!strcmp(tok, "i2cw"))		cmd->id = CMD_I2C_ID_I2CW;
	else if (!strcmp(tok, "i2cp"))		cmd->id = CMD_I2C_ID_I2CP;
	else if (!strcmp(tok, "sleep"))		cmd->id = CMD_I2C_ID_SLEEP;
	else if (!strcmp(tok, "wait"))		cmd->id = CMD_I2C_ID_WAIT;
	else if (!strcmp(tok, "help"))		cmd->id = CMD_I2C_ID_HELP;
	else if (!strcmp(tok, "quit"))		cmd->id = CMD_I2C_ID_QUIT;
	// shortcut command
	else if (!strcmp(tok, "q"))			cmd->id = CMD_I2C_ID_QUIT;
	else								cmd->id = CMD_I2C_ID_UNKNOWN;
}
//------------------------------------------------------------------------------
static void parse_i2c_id_help(const char *tok, cmds_i2c *cmd)
{
	if (!strcmp(tok, "i2cr"))			cmd->opts.id_help = CMD_I2C_ID_I2CR;
	else if (!strcmp(tok, "i2cw"))		cmd->opts.id_help = CMD_I2C_ID_I2CW;
	else if (!strcmp(tok, "i2cp"))		cmd->opts.id_help = CMD_I2C_ID_I2CP;
	else if (!strcmp(tok, "sleep"))		cmd->opts.id_help = CMD_I2C_ID_SLEEP;
	else if (!strcmp(tok, "wait"))		cmd->opts.id_help = CMD_I2C_ID_WAIT;
	else if (!strcmp(tok, "help"))		cmd->opts.id_help = CMD_I2C_ID_HELP;
	else if (!strcmp(tok, "quit"))		cmd->opts.id_help = CMD_I2C_ID_QUIT;
	else								cmd->opts.id_help = CMD_I2C_ID_UNKNOWN;
}
//------------------------------------------------------------------------------
static void parse_i2c_token0(const char *tok, cmds_i2c *cmd)
{
	parse_i2c_id(tok, cmd);
}
//------------------------------------------------------------------------------
static void parse_i2c_token1(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CR:	cmd->opts.saddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_I2CW:	cmd->opts.saddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_I2CP:	cmd->opts.saddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_SLEEP:	cmd->opts.msec = (uint32_t)strtoul(tok, NULL, 10);	break;
		case CMD_I2C_ID_WAIT:	cmd->opts.saddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_HELP:	parse_i2c_id_help(tok, cmd);						break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token2(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CR:	cmd->opts.oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_I2CW:	cmd->opts.oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_I2CP:	cmd->opts.oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_WAIT:	cmd->opts.oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token3(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CR:	cmd->opts.size = (uint16_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_I2CW:
			cmd->opts.data[0] = (uint8_t)strtol(tok, NULL, 16);
			cmd->opts.size = 1;
																					break;
		case CMD_I2C_ID_I2CP:	cmd->opts.saddr2 = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_WAIT:	cmd->opts.mask = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token4(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CW:
			cmd->opts.data[1] = (uint8_t)strtol(tok, NULL, 16);
			cmd->opts.size = 2;
																					break;
		case CMD_I2C_ID_I2CP:	cmd->opts.oaddr2 = (uint8_t)strtol(tok, NULL, 16);	break;
		case CMD_I2C_ID_WAIT:	cmd->opts.data[0] = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token5(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CW:
			cmd->opts.data[2] = (uint8_t)strtol(tok, NULL, 16);
			cmd->opts.size = 3;
																					break;
		case CMD_I2C_ID_I2CP:	cmd->opts.p_saddr = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token6(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CW:
			cmd->opts.data[3] = (uint8_t)strtol(tok, NULL, 16);
			cmd->opts.size = 4;
																					break;
		case CMD_I2C_ID_I2CP:	cmd->opts.p_oaddr = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token7(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CW:
			cmd->opts.data[4] = (uint8_t)strtol(tok, NULL, 16);
			cmd->opts.size = 5;
																					break;
		case CMD_I2C_ID_I2CP:	cmd->opts.p_rd = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
static void parse_i2c_token8(const char *tok, cmds_i2c *cmd)
{
	switch (cmd->id) {
		case CMD_I2C_ID_I2CW:
			cmd->opts.data[5] = (uint8_t)strtol(tok, NULL, 16);
			cmd->opts.size = 6;
																					break;
		case CMD_I2C_ID_I2CP:	cmd->opts.p_wr = (uint8_t)strtol(tok, NULL, 16);	break;
		default:																	break;
	}
}
//------------------------------------------------------------------------------
void opts_i2c_parsing(cmds_i2c *cmd)
{
	char buf[MAX_LINE];
	//char *tok, *str, *savep, *delim = " \n\0";
	char *tok, *str, *delim = " \r\n\0";
	int idx;

	//init
	cmd->id = CMD_I2C_ID_NONE;
	memset(&cmd->opts, 0, sizeof(opts_i2c));

	strcpy(buf, cmd->str);
	del_line_comment(buf);
	del_crlf(cmd->str);
	for (str = buf, idx = 0; ; str = NULL, idx++) {
		//tok = strtok_r(str, delim, &savep);
		tok = strtok(str, delim);
		if (tok == NULL) break;
		switch (idx) {
			case 0: parse_i2c_token0(tok, cmd);		break;
			case 1: parse_i2c_token1(tok, cmd);		break;
			case 2: parse_i2c_token2(tok, cmd);		break;
			case 3: parse_i2c_token3(tok, cmd);		break;
			case 4: parse_i2c_token4(tok, cmd);		break;
			case 5: parse_i2c_token5(tok, cmd);		break;
			case 6: parse_i2c_token6(tok, cmd);		break;
			case 7: parse_i2c_token7(tok, cmd);		break;
			case 8: parse_i2c_token8(tok, cmd);		break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
				if (cmd->id == CMD_I2C_ID_I2CW) {
					cmd->opts.data[idx - 3] = (uint8_t)strtol(tok, NULL, 16);
					cmd->opts.size = idx - 3 + 1;
				}
													break;
			default:								break;
		}
	}
}
//------------------------------------------------------------------------------
