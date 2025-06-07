#ifndef __COMMON_XBH_F2_P2_BOARD_H__
#define __COMMON_XBH_F2_P2_BOARD_H__

#include <hardware/board.h>

#ifdef XBH_BOARD_GPIO_F_HUB_RST_FRONT2_P2
#undef XBH_BOARD_GPIO_F_HUB_RST
#define XBH_BOARD_GPIO_F_HUB_RST                 XBH_BOARD_GPIO_F_HUB_RST_FRONT2_P2
#endif

#ifdef XBH_BOARD_GPIO_F_HUB_RST_LEVEL_FRONT2_P2
#undef XBH_BOARD_GPIO_F_HUB_RST_LEVEL
#define XBH_BOARD_GPIO_F_HUB_RST_LEVEL           XBH_BOARD_GPIO_F_HUB_RST_LEVEL_FRONT2_P2
#endif

#ifndef XBH_BOARD_GPIO_F_HUB_RST
#define XBH_BOARD_GPIO_F_HUB_RST                -1
#define XBH_BOARD_GPIO_F_HUB_RST_LEVEL           XBH_BOARD_GPIO_HIGH
#endif

#ifndef XBH_BOARD_GPIO_F_GPIO1
#define XBH_BOARD_GPIO_F_GPIO1                  -1
#define XBH_BOARD_GPIO_F_GPIO1_LEVEL            XBH_BOARD_GPIO_HIGH
#endif

#ifndef XBH_BOARD_GPIO_F_GPIO2
#define XBH_BOARD_GPIO_F_GPIO2                  -1
#define XBH_BOARD_GPIO_F_GPIO2_LEVEL            XBH_BOARD_GPIO_HIGH
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_PWR
#define XBH_BOARD_GPIO_F_USBC_PWR              -1
#define XBH_BOARD_GPIO_F_USBC_PWR_LEVEL         XBH_BOARD_GPIO_HIGH
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_RST
#define XBH_BOARD_GPIO_F_USBC_RST              -1
#define XBH_BOARD_GPIO_F_USBC_RST_LEVEL         XBH_BOARD_GPIO_HIGH//工作时的电平状态(正常工作电平)
#endif

#ifndef XBH_BOARD_GPIO_F_USB_PWR
#define XBH_BOARD_GPIO_F_USB_PWR              -1
#define XBH_BOARD_GPIO_F_USB_PWR_LEVEL         XBH_BOARD_GPIO_HIGH
#endif

/*
 * HDMI SWITCH
 * 第一行表示第一级设备，第二行是第二级设备,必须是2x4的矩阵
 * 最大支持2x4的矩阵
 */
#undef XBH_F2P2_HDMI_SWITCH_TABLE
#define XBH_F2P2_HDMI_SWITCH_TABLE \
    ASW3642

/*
 * struct HDMI_SW_OPT_TABLE{
 *      XBH_U32 iicBus;         //device iic bus，没有就填-1
 *      XBH_U8 iicAddr;         //device iic address，没有就填-1
 *      XBH_S32 sGpio;          //1切2的HDMI switch使用的切换引脚，没有就填-1
 *      XBH_S32 sLevel;         //1切2的HDMI switch 切到port0时的电平
 *      XBH_S32 pGpio;          //供电引脚，没有就填-1
 *      XBH_S32 pLevel;         //工作时的电平状态
 *      XBH_S32 rGpio;          //复位引脚，没有就填-1
 *      XBH_S32 rLevel;         //工作时的电平状态(正常工作电平)
 *      XBH_S32 defaultPort;    //默认端口
 *  };
 */
#undef XBH_F2P2_HDMI_SWITCH_IIC_TABLE
#define XBH_F2P2_HDMI_SWITCH_IIC_TABLE \
    {-1, -1, -1, -1, -1, -1, -1, -1, -1}

/*
 * HDMI SWITCH 器件的port口定义,必须是2x4的矩阵
 * struct HDMI_SWITCH_PORT{
 *      XBH_U8 port0;   //port0  0x00：悬空，0x01：连接下一级switch
 *      XBH_U8 port1;   //port1  0x00：悬空，0x01：连接下一级switch
 *      XBH_U8 port2;   //port2  0x00：悬空，0x01：连接下一级switch
 *      XBH_U8 port3;   //port3  0x00：悬空，0x01：连接下一级switch
 *  };
 */
#undef XBH_F2P2_HDMI_SWITCH_PORT_TABLE
#define XBH_F2P2_HDMI_SWITCH_PORT_TABLE \
    {XBH_SOURCE_F_HDMI1, 0x00, 0x00, 0x00}

/*******************************************************************************switch table*****************************************************************************/
static GPIO_OPT_TABLE source_android_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}
};

static GPIO_OPT_TABLE source_android_table_ext_after_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}
};

static GPIO_OPT_TABLE source_src_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              !XBH_BOARD_GPIO_F_HUB_RST_LEVEL}},
};

static GPIO_OPT_TABLE source_src_table_ext_after_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              !XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_after_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_before_2_p2[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_after_2_p2[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              !XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_after_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_before_2_p2[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_after_2_p2[] = {
    
};

//FCT
static GPIO_OPT_TABLE fct_source_android_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              !XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_android_table_ext_after_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_src_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,               XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_src_table_ext_after_2_p2[] = {
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_before_2_p2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_after_2_p2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_before_2_p2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_after_2_p2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_before_2_p2[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,               XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_after_2_p2[] = {
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_before_2_p2[] = {

};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_after_2_p2[] = {

};
#endif
