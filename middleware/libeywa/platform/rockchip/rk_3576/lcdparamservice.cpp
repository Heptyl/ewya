/* monitor the /mnt/external_sd/rk_lcd_parameters.    */
/* if the parameters has been changed, and then update the lcdparamers.  */
/* the parameters will work after reboot. */
/* lxt@rock-chips.com */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <sys/utsname.h>
#include <cutils/list.h>
#include <cutils/log.h>
#include <cutils/sockets.h>
#include <sys/reboot.h>
#include <cutils/iosched_policy.h>
#include "lcdparamservice.h"

#undef LOG_TAG
#define LOG_TAG "LcdParamService"
#define ENABLE_USB_UPDATE_TP_GT9XX 0

typedef     unsigned short    uint16;
typedef     unsigned long     uint32;
typedef     unsigned char     uint8;

#define LCD_PARAMETE_FILE_PATH         "/system/bin/busybox find /mnt/media_rw/ -name PanelSetting.txt"
#define TP_FIRMWARE_FILE_PATH         "/system/bin/busybox find /mnt/media_rw/ -name LCD_PARAM_TP_GT911_RK356X.txt"
#define OEM_PARTITIOM_NODE_PATH         "/dev/block/by-name/lcdparam"
// #define BASEPARAMETER_PATH              "/dev/block/platform/fe310000.sdhci/by-name/baseparameter"
#define BASEPARAMETER_PATH              "/dev/block/by-name/baseparameter"
#define I2CTRANSFER_CMD "i2ctransfer -f -y 4 w188@0x5d 0x80 0x47 "  //default 3399 I2C4
/*#define OEM_PARTITIOM_NODE_PATH         "panel1_data/oem"*/

/*#define RKNAND_GET_VENDOR_SECTOR0       _IOW('v', 16, unsigned int)*/
/*#define RKNAND_STORE_VENDOR_SECTOR0     _IOW('v', 17, unsigned int)*/

#define RKNAND_SYS_STORGAE_DATA_LEN (512*4)  //oem size 2k
/*#define VENDOR_SECTOR_OP_TAG        0x444E4556 // "VEND"*/

#define POLY    0xEDB88320L              //CRC stand
#define CONFIG_NUM  36
#define BASE_OFFSET (256 * 1024)
#define BACKUP_BASE_OFFSET (512 * 1024)

#define LCDPARAM_BACKUP_OFFSET (BASE_OFFSET + BACKUP_BASE_OFFSET)

struct display_fixup_data {
	int type;

	int delay_prepare;
	int delay_enable;
	int delay_disable;
	int delay_unprepare;
	int delay_reset;
	int delay_init;

	int size_width;
	int size_height;

	int clock_frequency;
	int hactive;
	int hfront_porch;
	int hsync_len;
	int hback_porch;
	int vactive;
	int vfront_porch;
	int vsync_len;
	int vback_porch;
	int hsync_active;
	int vsync_active;
	int de_active;
	int pixelclk_active;

	int lane_rate;
	int output_port; // 1: 1port  2:2port
	int output_format; // 0:vesa  1:jeida
    int swap_port;
	int auto_dsi_cmds_en; // tc358775
	
	/* for Backlight */
	int default_brightness;
	int pwm_freq;
	int pwm_polarity;
	int pwm_brightMin;
	int pwm_brightMax;
    
	/* for DSI Panel */
	int flags;	
    int format;
	int lanes;
	char *init_cmd;
 
};

enum {
	PANEL_TYPE_INVALID,
	PANEL_TYPE_DSI_MIPI,
	PANEL_TYPE_DSI_EXTERN,
	PANEL_TYPE_EDP,
	PANEL_TYPE_LVDS,
};

char *key[CONFIG_NUM] =
{
    "panel-type",
    "unprepare-delay-ms",
    "enable-delay-ms",
    "disable-delay-ms",
    "prepare-delay-ms",
    "reset-delay-ms",
    "init-delay-ms",
    "width-mm",
    "height-mm",

    "clock-frequency",
    "hactive",
    "hfront-porch",
    "hsync-len",
    "hback-porch",
    "vactive",
    "vfront-porch",
    "vsync-len",
    "vback-porch",
    "hsync-active",
    "vsync-active",
    "de-active",
    "pixelclk-active",

    "rockchip,lane-rate",
    "rockchip,output",
    "rockchip,data-mapping",
    "swap-port",
    "auto-dsi-cmds-en",

    "default-brightness-level",
    "pwm-freq",
    "pwm-polarity",
    "pwm-brightMin",
    "pwm-brightMax",

    "dsi,flags",
    "dsi,format",
    "dsi,lanes",
    "panel-init-sequence",
    
};

typedef struct tagRKNAND_SYS_STORGAE
{
    unsigned char panel1_data[RKNAND_SYS_STORGAE_DATA_LEN];
    unsigned char panel2_data[RKNAND_SYS_STORGAE_DATA_LEN];
}RKNAND_SYS_STORGAE;

static uint32 crc32_tab[256];
static uint32 nand_crc = 0;

char *strreplace(char *s, char old, char _new)
{
    for (; *s; ++s)
        if (*s == old)
            *s = _new;
    return s;
}

void rknand_print_hex_data(uint8 *s,uint32 * buf,uint32 len)
{
    uint32 i;
    ALOGE("%s",s);
    for(i=0;i<len;i+=4) {
        ALOGE("%lx %lx %lx %lx",buf[i],buf[i+1],buf[i+2],buf[i+3]);
    }
}

void init_crc32_tab(void)
{
    int i = 0;
    int j = 0;
    uint32 crc = 0;

    for(i = 0; i < 256; i++) {
        crc = (uint32)i;
        for(j = 0; j < 8; j++) {
            if(crc & 0x00000001L)
                crc = (crc >> 1) ^ POLY;
            else
                crc = crc >> 1;
        }
        crc32_tab[i] = crc;
    }
}

uint32 get_crc32(uint32 crc_init, uint8 *crc_buf, uint32 buf_size)
{
    uint32 crc = crc_init ^ 0xffffffff;

    init_crc32_tab();
    while(buf_size--)
        crc = (crc >> 8) ^ crc32_tab[(crc & 0xff) ^ *crc_buf++];
    return crc ^ 0xfffffff;
}

uint32 getfile_crc(FILE *fp)
{
    uint32 size = 4 *1024; // 4k
    uint8 crc_buf[size];
    uint32 readln = 0;
    uint crc = 0;

    while((readln = fread(crc_buf, sizeof(uint8), size, fp)) > 0)
        crc = get_crc32(crc, crc_buf, readln);
    return crc;
}

/*
 *去除字符串左端空格
 */
char *strtriml(char *pstr)
{
    int i = 0,j;
    j = strlen(pstr) - 1;
    while (isspace(pstr[i]) && (i <= j))
        i++;
    if (0<i)
        strcpy(pstr, &pstr[i]);
    return pstr;
}

/*
 *去除字符串右端空格
 */
char *strtrimr(char *pstr)
{
    int i;
    i = strlen(pstr) - 1;
    while (isspace(pstr[i]) && (i >= 0))
        pstr[i--] = '\0';
    return pstr;
}

/*
 *去除字符串两端空格
 */
char *strtrim(char *pstr)
{
    char *p;
    p = strtrimr(pstr);
    return strtriml(p);
}

char *strdelchr(char *pstr, int chr)
{
    int i = 0;
    int l = 0;
    int ll = 0;
   ll = l = strlen(pstr);

    while(i < l) {
        if(pstr[i] == chr) {
            memmove((pstr + i), (pstr + i + 1), (ll - i -1));
            pstr[ll - 1] = '\0';
            ll--;
        }
        i++;
    }
    return pstr;
}

void strrmspace(char * str)
{
    char *p1, *p2;
    char ch;
    p1 = str; //first pointer
    p2 = str;  // second pointer to the remaining string
    if (p1==NULL) return;
    while (*p1) {
        if (*p1 != ' ') {
            ch = *p1;
            *p2 = ch;
            p1++;
            p2++;
        } else {
            p1++;
        }
    }
    *p2 = '\0';
}

uint32  getfile_crc_from_nand(void)
{
    int ret = 0;
    uint32 crc = 0;
    RKNAND_SYS_STORGAE sysData;

    int sys_fd;
    ALOGE("-- reading lcd parameters partition");
    if (0 != access(BASEPARAMETER_PATH, R_OK | W_OK)) {
        ALOGE("-- access Error !%s",BASEPARAMETER_PATH);
        return -1;
    }

    memset(sysData.panel1_data, '\0', sizeof(sysData.panel1_data));
    sys_fd = open(BASEPARAMETER_PATH, O_RDONLY);
    if (sys_fd < 0) {
        ALOGE("-- open Error !%s err =,%d\n",BASEPARAMETER_PATH, sys_fd);
        return -1;
    }

    lseek(sys_fd, BASE_OFFSET, SEEK_SET);
    ret = read (sys_fd, (void*)&sysData, sizeof(sysData)); // panel1 + panel2
    if (ret < 0) {
        ALOGE("-- read Error !%s err =,%d",BASEPARAMETER_PATH, sys_fd );
        close(sys_fd);
        return -1;
    }
    for(int i = 0; i < CONFIG_NUM - 1; i++) {
        crc = sysData.panel1_data[i * 4];
        crc = (crc << 8) + sysData.panel1_data[i * 4 + 1];
        crc = (crc << 8) + sysData.panel1_data[i * 4 + 2];
        crc = (crc << 8) + sysData.panel1_data[i * 4 + 3];
        // ALOGE("-- %s %s = %lu ",BASEPARAMETER_PATH , key[i], crc);
    }

    crc = sysData.panel1_data[(CONFIG_NUM-1) * 4];
    crc = (crc << 8) + sysData.panel1_data[(CONFIG_NUM-1) * 4 + 1];
    crc = (crc << 8) + sysData.panel1_data[(CONFIG_NUM-1) * 4 + 2];
    crc = (crc << 8) + sysData.panel1_data[(CONFIG_NUM-1) * 4 + 3];
    ALOGE("-- %s panel-init-sequence length = %lu ",BASEPARAMETER_PATH , crc);

    // for (int i = 0 ; (i < crc)&&(i <= 1000);i++ ) {
        // ALOGE("-- %s panel-init-sequence %d = 0x%x ",BASEPARAMETER_PATH , i, sysData.panel1_data[CONFIG_NUM * 4 + i]);
    // }

    crc = sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-4];
    crc = (crc << 8) + sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-3];
    crc = (crc << 8) + sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-2];
    crc = (crc << 8) + sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-1];
    ALOGE("-- nand crc panel1_data = 0X%02X%02X%02X%02X", sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-4],
        sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-3], sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-2], sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-1]);
    close(sys_fd);
    return crc;
}

// void pre_handle_panel_data(RKNAND_SYS_STORGAE *panelData)
// {
//     struct display_fixup_data *panel1_data;
//     struct display_fixup_data *panel2_data;
//     int i;
//     int panel1Param[CONFIG_NUM] = {0};
//     int panel2Param[CONFIG_NUM] = {0};
//     int panel1_type, panel2_type;

//     panel1_data = (struct display_fixup_data *)panel1Param;
//     panel2_data = (struct display_fixup_data *)panel2Param;

//     //  debug
//     for (i = 0; i < CONFIG_NUM; i++)
// 	{
// 		panel1Param[i] = panelData->panel1_data[i * 4];
// 		panel1Param[i] = (panel1Param[i] << 8) + panelData->panel1_data[i * 4 + 1];
// 		panel1Param[i] = (panel1Param[i] << 8) + panelData->panel1_data[i * 4 + 2];
// 		panel1Param[i] = (panel1Param[i] << 8) + panelData->panel1_data[i * 4 + 3];
// 		if (panel1Param[i] < 0)
// 			panel1Param[i] = -panel1Param[i];

// 		if (panel1Param[i] > 100000 && i != 9) // only clk
// 			panel1Param[i] = 0;

// 		ALOGE("-- [panel1]lcd_param %s: %d\n", key[i], panel1Param[i]);
// 	}

// 	for (i = 0; i < CONFIG_NUM; i++)
// 	{
// 		panel2Param[i] = panelData->panel2_data[i * 4];
// 		panel2Param[i] = (panel2Param[i] << 8) + panelData->panel2_data[i * 4 + 1];
// 		panel2Param[i] = (panel2Param[i] << 8) + panelData->panel2_data[i * 4 + 2];
// 		panel2Param[i] = (panel2Param[i] << 8) + panelData->panel2_data[i * 4 + 3];
// 		if (panel2Param[i] < 0)
// 			panel2Param[i] = -panel2Param[i];

// 		if (panel2Param[i] > 100000 && i != 9) // only clk
// 			panel2Param[i] = 0;

// 		ALOGE("-- [panel2]lcd_param %s: %d\n", key[i], panel2Param[i]);
// 	}

//     panel1_type = panel1_data->type;
//     panel2_type = panel2_data->type;

//     if (panel1_type == PANEL_TYPE_INVALID || (panel1_type == panel2_type)) {
//         char primaryPanelBak[PROPERTY_VALUE_MAX] = {0};
//         char extendPanelBak[PROPERTY_VALUE_MAX] = {0};

//         SysWrite::getInstance()->getPropertyString("vendor.hwc.device.primaryBak", primaryPanelBak, " ");
//         SysWrite::getInstance()->getPropertyString("vendor.hwc.device.extendBak", extendPanelBak, " ");

//         char cmdPrimary[1024] = {0};
//         char cmdExtend[1024] = {0};
//         sprintf(cmdPrimary, "echo 'vendor.hwc.device.primary=%s' >> /vendor/build.prop", primaryPanelBak);
//         sprintf(cmdExtend, "echo 'vendor.hwc.device.extend=%s' >> /vendor/build.prop", extendPanelBak);

//         // 还原
//         XbhFramework::getInstance()->shellExecute("mount -o rw,remount /vendor");
//         XbhFramework::getInstance()->shellExecute("rm -rf vendor/empty.bin");
//         XbhFramework::getInstance()->shellExecute("sed -i '/vendor.hwc.device.primary=/d' /vendor/build.prop");
//         XbhFramework::getInstance()->shellExecute("sed -i '/vendor.hwc.device.extend=/d' /vendor/build.prop");
//         XbhFramework::getInstance()->shellExecute(cmdPrimary);
//         XbhFramework::getInstance()->shellExecute(cmdExtend);
//         XbhFramework::getInstance()->shellExecute("sync");
//         return;
//     }

//     if (panel1_type == PANEL_TYPE_DSI_EXTERN || panel2_type == PANEL_TYPE_DSI_EXTERN) {
//         SysWrite::getInstance()->setProperty("persist.sys.dsi2Extern", "1");
//     } else {
//         SysWrite::getInstance()->setProperty("persist.sys.dsi2Extern", "0");
//     }

//     if (panel2_type == PANEL_TYPE_INVALID) { // 单屏
//         XbhFramework::getInstance()->shellExecute("mount -o rw,remount /vendor");
//         XbhFramework::getInstance()->shellExecute("rm -rf vendor/empty.bin");
//         XbhFramework::getInstance()->shellExecute("sed -i '/vendor.hwc.device.primary=/d' /vendor/build.prop");
//         XbhFramework::getInstance()->shellExecute("sed -i '/vendor.hwc.device.extend=/d' /vendor/build.prop");
//         XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=HDMI-A' >> /vendor/build.prop");
//     } else { // 双屏
//         XbhFramework::getInstance()->shellExecute("mount -o rw,remount /vendor");
//         XbhFramework::getInstance()->shellExecute("rm -rf vendor/empty.bin");
//         XbhFramework::getInstance()->shellExecute("sed -i '/vendor.hwc.device.primary=/d' /vendor/build.prop");
//         XbhFramework::getInstance()->shellExecute("sed -i '/vendor.hwc.device.extend=/d' /vendor/build.prop");

//         if (panel1_type == PANEL_TYPE_DSI_MIPI && panel2_type == PANEL_TYPE_DSI_EXTERN) {
//             int mipiPanelOnDsi = SysWrite::getInstance()->getPropertyInt("persist.sys.mipiPanelDsiN", -1);
//             if (mipiPanelOnDsi == 0) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=DSI-1' >> /vendor/build.prop");
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=DSI-2' >> /vendor/build.prop");
//             } else if (mipiPanelOnDsi == 1) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=DSI-2' >> /vendor/build.prop");
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=DSI-1' >> /vendor/build.prop");
//             } else {
//                 ALOGE("-- mipiPanelOnDsi Error undefined!\n");
//             }
//         } else if (panel1_type == PANEL_TYPE_DSI_EXTERN && panel2_type == PANEL_TYPE_DSI_MIPI) {
//             int mipiPanelOnDsi = SysWrite::getInstance()->getPropertyInt("persist.sys.mipiPanelDsiN", -1);
//             if (mipiPanelOnDsi == 0) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=DSI-2' >> /vendor/build.prop");
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=DSI-1' >> /vendor/build.prop");
//             } else if (mipiPanelOnDsi == 1) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=DSI-1' >> /vendor/build.prop");
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=DSI-2' >> /vendor/build.prop");
//             } else {
//                 ALOGE("-- mipiPanelOnDsi Error undefined!\n");
//             }
//         } else {
//             if (panel1_type == PANEL_TYPE_EDP) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=eDP' >> /vendor/build.prop");
//             } else if (panel1_type == PANEL_TYPE_DSI_MIPI) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=DSI' >> /vendor/build.prop");
//             } else if (panel1_type == PANEL_TYPE_DSI_EXTERN) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.primary=DSI' >> /vendor/build.prop");
//             }

//             if (panel2_type == PANEL_TYPE_EDP) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=eDP' >> /vendor/build.prop");
//             } else if (panel2_type == PANEL_TYPE_DSI_MIPI) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=DSI' >> /vendor/build.prop");
//             } else if (panel2_type == PANEL_TYPE_DSI_EXTERN) {
//                 XbhFramework::getInstance()->shellExecute("echo 'vendor.hwc.device.extend=DSI' >> /vendor/build.prop");
//             }
//         }
//     }
//     XbhFramework::getInstance()->shellExecute("sync");

// }

XBH_U32 lcdparamservice:: rk_update_lcd_parameters_from_sdcard()
{
    int ret = 0;
    FILE *fp = 0;
    char line[RKNAND_SYS_STORGAE_DATA_LEN] = {0};
    int i = 0;
    RKNAND_SYS_STORGAE sysData;
    int sys_fd = -1;
    uint32 total=0;
    uint32 test_crc = 0;
    static uint32 file_crc = 0; //file lcdparameter  crc panel1_data
    static int updated = 0;  // had store the param into the nand
    static char got_crc = 0; //get file crc flag
    char   lcdparameter_buf[128];
    FILE   *stream;
    memset( lcdparameter_buf, '\0', sizeof(lcdparameter_buf) );
    memset(sysData.panel1_data, '\0', sizeof(sysData.panel1_data));
    memset(sysData.panel2_data, '\0', sizeof(sysData.panel2_data));
    stream = popen(LCD_PARAMETE_FILE_PATH, "r");
    fread( lcdparameter_buf, sizeof(char), sizeof(lcdparameter_buf), stream);
    strreplace(lcdparameter_buf,'\n','\0');
    pclose(stream);
    // ALOGE("-- reading SD card parameters\n");
    if(access(lcdparameter_buf, 0)) {
        // ALOGE("-- access Error ! path:%s",LCD_PARAMETE_FILE_PATH);
        return -1;
    }

    nand_crc = getfile_crc_from_nand();
    if(!updated) {
        if (!got_crc) {
            fp = fopen(lcdparameter_buf, "r");
            ALOGE("-- ======== %s %d ========\n", __func__, __LINE__);
            if(fp == NULL)
            {
                ALOGE("open %s failed", lcdparameter_buf);
                return -1;
            }
            else
            {
                ALOGE("success  open  file %s", lcdparameter_buf);
            }

            file_crc = getfile_crc(fp);
            got_crc = 1;
            ALOGE(" file crc is 0X%08lX nand_crc is 0X%08lX", file_crc, nand_crc );
            fclose(fp);
        }

        if((nand_crc != file_crc)) {
            fp = fopen(lcdparameter_buf, "r");
            if(fp == NULL) {
                ALOGE("open %s failed", lcdparameter_buf);
                return -1;
            } else {
                ALOGE("success  open  file %s", lcdparameter_buf);
            }

            int panel_flag = 0;

            // find panel1
            while(fgets(line, RKNAND_SYS_STORGAE_DATA_LEN-1, fp)) {
                char cmd[5] = {'\0'};
                bool attr_vaild_flag = false;
                char *p = strtrim(line);
                int len = strlen(p);

                if((p[0] == '#') || (len <= 0))
                    continue;

                if (strstr(p,"panel1")) {
                    panel_flag = 1;
                    continue;
                } else if (strstr(p, "panel2")) {
                    panel_flag = 2;
                    continue;
                }

                if((!strstr(p,"=")) || (!strstr(p,";"))) {
                    ALOGE("%d    %d     \n",(!strstr(p,"=")),(!strstr(p,";")));
                    ALOGE("-- Error! current attr %s is invalid!! \n", p);
                    continue;
                }

                unsigned char *panel_data = NULL;

                if (panel_flag == 1) {
                    panel_data = sysData.panel1_data;
                } else if (panel_flag == 2) {
                    panel_data = sysData.panel2_data;
                } else {
                    ALOGE("-- Error! no found [panel1] or [panel2] \n");
                    continue;
                }

                char *key_val_str = strtok(p, ";");
                char *value = strchr(key_val_str, '=');
                if(value==NULL)
                    continue ;

                for(i = 0; i < CONFIG_NUM; i++) {
                    if(strstr(p, key[i])) {
                        char *val = strdelchr(value, '=');
                        strrmspace(val);
                        uint32 config1 = atoi(val);//chat to uint32

                        if (strstr(p, "panel-init-sequence")) {
                            int count=0;
                            int cmdlen=strlen(val)/2;
                            panel_data[i * 4] =(uint8) (cmdlen >> 24);
                            panel_data[i * 4 + 1] =(uint8) (cmdlen >> 16);
                            panel_data[i * 4 + 2] =(uint8) (cmdlen >> 8);
                            panel_data[i * 4 + 3] =(uint8) (cmdlen >> 0);

                            ALOGE("-- [panel%d]panel-init-sequence length = %d ", panel_flag, cmdlen);
                            ALOGE("-- [panel%d]panel-init-sequence(char) = %s ", panel_flag, val);
                            i ++;
                            while(1) {
                                cmd [0] = '0';
                                cmd [1] = 'x';
                                cmd [2] = *val;
                                cmd [3] = *(++val);
                                //string to uint8
                                //eg "0x12" to 0x12
                                // ALOGE("-- [panel%d]cmd[%d] = %s", panel_flag, count, cmd);
                                uint8 cmdu8 = strtol(cmd,NULL,16);
                                panel_data[i * 4 + count++] = cmdu8;
                                // ALOGE("-- [panel%d]count=%d panel-init-sequence(u8)= 0x%x", panel_flag, count, cmdu8);
                                if (count < cmdlen)
                                    ++val; //next cmd
                                else
                                    break;
                            }
                            total = i * 4 + cmdlen;
                            ALOGE("-- lcd parameters partition is A total of use %luB + 4B(the last of CRC)",total);
                        } else {
                            panel_data[i * 4] =(uint8) (config1 >> 24);
                            panel_data[i * 4 + 1] =(uint8) (config1 >> 16);
                            panel_data[i * 4 + 2] =(uint8) (config1 >> 8);
                            panel_data[i * 4 + 3] =(uint8) (config1 >> 0);
                            // ALOGE("-- [panel%d]%s=%lu ", panel_flag, key[i], config1);
                        }
                        //current attr is valid!
                        attr_vaild_flag=true;
                        break;
                    }
                }
                if(!attr_vaild_flag) {
                    ALOGE("-- [panel%d]Error! current attr %s=%s is invalid!! ", panel_flag, p, value);
                }
            }

            ALOGE("-- lcd parameters partition allows of size %dB ", RKNAND_SYS_STORGAE_DATA_LEN*2);
            if((total+4) >  RKNAND_SYS_STORGAE_DATA_LEN*2) {
                ALOGE("-- Error! lcd parameters partition is to small ");
                goto err_fd;
            }
            // file crc panel1_data
            sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-4] = (uint8) (file_crc >> 24);
            sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-3] = (uint8) (file_crc >> 16);
            sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-2] = (uint8) (file_crc >> 8);
            sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-1] = (uint8) (file_crc >> 0);
            ALOGE("-- The CRC of partition = 0X%02X%02X%02X%02X", sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-4],
                    sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-3],
                    sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-2],
                    sysData.panel1_data[RKNAND_SYS_STORGAE_DATA_LEN-1]);

            if (access(BASEPARAMETER_PATH, R_OK | W_OK)) {
                ALOGE("-- access Error !%s",BASEPARAMETER_PATH);
                goto err_fd;
            }

            //pre_handle_panel_data(&sysData);

            sys_fd = open(BASEPARAMETER_PATH, O_WRONLY);
            if (sys_fd < 0) {
                ALOGE("-- open Error %s err=%d",BASEPARAMETER_PATH, sys_fd);
                goto err_fd;
            }
            lseek(sys_fd, BASE_OFFSET, SEEK_SET);

            ret = write(sys_fd, (void*)&sysData.panel1_data[0], RKNAND_SYS_STORGAE_DATA_LEN * 2); // panel1 and panel2
            if (ret < 0) {
                ALOGE("-- write Error %s err=%d",BASEPARAMETER_PATH, sys_fd);
                goto err_sys_fd;
            }

            lseek(sys_fd, LCDPARAM_BACKUP_OFFSET, SEEK_SET);
            ret = write(sys_fd, (void*)&sysData.panel1_data[0], RKNAND_SYS_STORGAE_DATA_LEN * 2); // backup panel1 and panel2
            if (ret < 0) {
                ALOGE("-- write backup Error %s err=%d",BASEPARAMETER_PATH, sys_fd);
                goto err_sys_fd;
            }

            close(sys_fd);
            sync();
            ALOGE("-- test %s",BASEPARAMETER_PATH);

            test_crc=getfile_crc_from_nand();
            if(test_crc != file_crc)
            {
                ALOGE("-- write Error %s err=%d",BASEPARAMETER_PATH, sys_fd);
                goto err_fd;
            }
            ALOGE("-- update lcd parameters succed");
            updated = 1;
            nand_crc = file_crc;
            fclose(fp);
            //property_set("sys.xbh.lcd_param_update", "OK");
            reboot(RB_AUTOBOOT);
            return 0;
        }
    }
err_sys_fd:
    if(sys_fd >= 0)
    {
        close(sys_fd);
    }
err_fd:
    if(fp)
    {
        fclose(fp);
    }
    return -1;
}


// #if ENABLE_USB_UPDATE_TP_GT9XX
// int update_tp_gt9xx_firmware()
// {
//     static int flag_send = 0;
//     FILE * stream;
//     FILE *fp = 0;  //用于获取TP固件的文件的句柄
//     char shell_cmd_i2ctransfer[1024] ={0};
//     char tp_firmware_buf[128];
//     char tp_firmware_send_buf[1024];
//     sprintf(shell_cmd_i2ctransfer,I2CTRANSFER_CMD);
//     memset(tp_firmware_buf,'\0',sizeof(tp_firmware_buf));
//     memset(tp_firmware_send_buf,'\0',sizeof(tp_firmware_send_buf));
//     stream = popen(TP_FIRMWARE_FILE_PATH, "r");
//     fread(tp_firmware_buf, sizeof(char), sizeof(tp_firmware_buf), stream);
//     pclose(stream);
//     strreplace(tp_firmware_buf,'\n','\0');

//     if(access(tp_firmware_buf, 0)) {
//         //ALOGE("-- access Error ! path:%s",tp_firmware_buf);
//         return -1;
//     }
//     if(!flag_send) {
//         flag_send = 1;
//         fp = fopen(tp_firmware_buf, "r");
//         if(fp == NULL) {
//             ALOGE("-- failed to open %s ", tp_firmware_buf);
//             return -1;
//         } else {
//             ALOGE("-- success open file %s", tp_firmware_buf); //TP固件文件路径打印
//         }
//         fgets(tp_firmware_send_buf,sizeof(tp_firmware_send_buf), fp); //获取文件内容到buf
//         fclose(fp);
//         strreplace(tp_firmware_send_buf,',',' ');

//         strcat(shell_cmd_i2ctransfer,tp_firmware_send_buf); //拼接固件信息到命令
//         ALOGE("-- shell_cmd_i2ctransfer_gt9xx : %s",shell_cmd_i2ctransfer);

//         XbhFramework::getInstance()->shellExecute(shell_cmd_i2ctransfer);
//         XbhFramework::getInstance()->shellExecute("reboot");
//     }
//     return - 1;
// }
// #endif

// int main(void)
// {
//     ALOGE("-ldp- setting sdcard\n");
//     nand_crc = getfile_crc_from_nand();
//     while(1) {
//        rk_update_lcd_parameters_from_sdcard();
//     #if ENABLE_USB_UPDATE_TP_GT9XX
//         update_tp_gt9xx_firmware();
//     #endif
//     //    ALOGE("-- uslppe 1000ms---");
//         usleep(5 * 1000 * 1000);
//     }
//     return 0;
// }

