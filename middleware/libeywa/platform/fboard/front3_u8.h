#ifndef __COMMON_XBH_F3_U8_BOARD_H__
#define __COMMON_XBH_F3_U8_BOARD_H__

#include <hardware/board.h>

#ifdef XBH_BOARD_GPIO_F_IIC_BUS
#define F3U8_IIC_BUS                 XBH_BOARD_GPIO_F_IIC_BUS
#else
#define F3U8_IIC_BUS                -1
#endif

#ifndef XBH_BOARD_GPIO_F_USB_PWR
#define XBH_BOARD_GPIO_F_USB_PWR              -1
#define XBH_BOARD_GPIO_F_USB_PWR_LEVEL         XBH_BOARD_GPIO_HIGH
#endif

#ifndef XBH_BOARD_GPIO_FHUB30_RST
#define XBH_BOARD_GPIO_FHUB30_RST              -1
#define XBH_BOARD_GPIO_FHUB30_RST_ON         XBH_BOARD_GPIO_LOW
#endif

#ifndef XBH_BOARD_GPIO_FHUB20_RST
#define XBH_BOARD_GPIO_FHUB20_RST              -1
#define XBH_BOARD_GPIO_FHUB20_RST_ON         XBH_BOARD_GPIO_LOW
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_SW
#define XBH_BOARD_GPIO_F_USBC_SW              -1
#endif

/*
 * HDMI SWITCH
 * 第一行表示第一级设备，第二行是第二级设备,必须是2x4的矩阵
 * 最大支持2x4的矩阵
 */
 #undef XBH_F3U8_HDMI_SWITCH_TABLE
 #define XBH_F3U8_HDMI_SWITCH_TABLE \
     GSV2712_1
 
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
 #undef XBH_F3U8_HDMI_SWITCH_IIC_TABLE
 #define XBH_F3U8_HDMI_SWITCH_IIC_TABLE \
     {F3U8_IIC_BUS, 0x58, -1, -1, XBH_BOARD_GPIO_F_USBC_PWR, XBH_BOARD_GPIO_F_USBC_PWR_LEVEL, XBH_BOARD_GPIO_F_USBC_RST, XBH_BOARD_GPIO_F_USBC_RST_LEVEL, 0}
 
 /*
  * HDMI SWITCH 器件的port口定义,必须是2x4的矩阵
  * struct HDMI_SWITCH_PORT{
  *      XBH_U8 port0;   //port0  0x00：悬空，0x01：连接下一级switch
  *      XBH_U8 port1;   //port1  0x00：悬空，0x01：连接下一级switch
  *      XBH_U8 port2;   //port2  0x00：悬空，0x01：连接下一级switch
  *      XBH_U8 port3;   //port3  0x00：悬空，0x01：连接下一级switch
  *  };
  */
 #undef XBH_F3U8_HDMI_SWITCH_PORT_TABLE
 #define XBH_F3U8_HDMI_SWITCH_PORT_TABLE \
     {XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_USBC1, 0x00, 0x00}

/*******************************************************************************switch table*****************************************************************************/
static GPIO_OPT_TABLE source_android_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_android_table_ext_after_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_src_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_src_table_ext_after_3_u8[] = {
    {REQUEST_DELAY,              {10 * 1000}},\
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             !XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USBC_SW,              XBH_BOARD_GPIO_LOW}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_after_3_u8[] = {
    {REQUEST_DELAY,              {10 * 1000 }}, \
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_before_3_u8[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_after_3_u8[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             !XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USBC_SW,              XBH_BOARD_GPIO_HIGH}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_after_3_u8[] = {
    {REQUEST_DELAY,              {10 * 1000 }}, \
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_before_3_u8[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_after_3_u8[] = {
    
};

//FCT
static GPIO_OPT_TABLE fct_source_android_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_android_table_ext_after_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_src_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_src_table_ext_after_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             !XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USBC_SW,              XBH_BOARD_GPIO_LOW}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_after_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_DELAY,              {500 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_before_3_u8[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_after_3_u8[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_before_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             !XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             !XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USBC_SW,              XBH_BOARD_GPIO_HIGH}}, \
    {REQUEST_DELAY,              {50 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB20_RST,             XBH_BOARD_GPIO_FHUB20_RST_ON}}, \
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_after_3_u8[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_FHUB30_RST,             XBH_BOARD_GPIO_FHUB30_RST_ON}}, \
    {REQUEST_DELAY,              {500 * 1000 }}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_before_3_u8[] = {

};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_after_3_u8[] = {

};
#endif
