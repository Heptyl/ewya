#ifndef __COMMON_XBH_F_WXQ_LG8195_311_3H_V11_BOARD_H__
#define __COMMON_XBH_F_WXQ_LG8195_311_3H_V11_BOARD_H__

#include <hardware/board.h>

#ifdef XBH_BOARD_GPIO_F_HDMI_SEL
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL                 XBH_BOARD_GPIO_F_HDMI_SEL //102 XBH_BOARD_GPIO_F_HDMI_SEL
#else
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL                -1
#endif

#ifdef XBH_BOARD_GPIO_F_HDMI_SEL_POARA
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL_PORTA          XBH_BOARD_GPIO_F_HDMI_SEL_POARA
#else
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL_PORTA          -1
#endif

#ifdef XBH_BOARD_GPIO_F_HDMI_SEL2
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL2                 XBH_BOARD_GPIO_F_HDMI_SEL2 //98 XBH_BOARD_GPIO_F_HDMI_SEL2
#else
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL2                -1
#endif

#ifdef XBH_BOARD_GPIO_F_HDMI_SEL_POARA2
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL2_PORTA            XBH_BOARD_GPIO_F_HDMI_SEL_POARA2
#else
#define F_WXQ_LG8195_311_3H_V11_HDMI_SEL2_PORTA                -1
#endif

#ifdef XBH_BOARD_GPIO_F_IIC_BUS
#define F_WXQ_LG8195_311_3H_V11_IIC_BUS                 XBH_BOARD_GPIO_F_IIC_BUS
#else
#define F_WXQ_LG8195_311_3H_V11_IIC_BUS                -1
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
#define XBH_BOARD_GPIO_F_USBC_RST_LEVEL         XBH_BOARD_GPIO_HIGH
#endif

#ifndef XBH_BOARD_GPIO_F_USB_PWR
#define XBH_BOARD_GPIO_F_USB_PWR              -1
#define XBH_BOARD_GPIO_F_USB_PWR_LEVEL         XBH_BOARD_GPIO_HIGH
#endif

//for map to actually gpio function name
#undef XBH_BOARD_GPIO_F_HUB_RST_ACTUALLY
#define XBH_BOARD_GPIO_F_HUB_RST_ACTUALLY        XBH_BOARD_GPIO_F_GPIO2

#undef XBH_BOARD_GPIO_F_HUB_RST_ACTUALLY_LEVEL
#define XBH_BOARD_GPIO_F_HUB_RST_ACTUALLY_LEVEL  XBH_BOARD_GPIO_F_GPIO2_LEVEL

#undef XBH_BOARD_GPIO_F_HUB_TX_RST
#define XBH_BOARD_GPIO_F_HUB_TX_RST              XBH_BOARD_GPIO_F_HUB_RST

#undef XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL
#define XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL        XBH_BOARD_GPIO_F_HUB_RST_LEVEL

#undef XBH_BOARD_GPIO_F_USB_SEL
#define XBH_BOARD_GPIO_F_USB_SEL                 -1

#undef XBH_BOARD_GPIO_F_USB_SEL_B
#define XBH_BOARD_GPIO_F_USB_SEL_B               XBH_BOARD_GPIO_F_GPIO1_LEVEL

#undef XBH_BOARD_GPIO_F_USBC_PWR                 
#define XBH_BOARD_GPIO_F_USBC_PWR                 -1





/*
 * HDMI SWITCH
 * 第一行表示第一级设备，第二行是第二级设备,必须是2x4的矩阵
 * 最大支持2x4的矩阵
 */
 
#undef XBH_F_WXQ_LG8195_311_3H_V11_HDMI_SWITCH_TABLE
#define XBH_F_WXQ_LG8195_311_3H_V11_HDMI_SWITCH_TABLE \
    WXQ_LT8641UX_U5

/*
 * struct HDMI_SW_OPT_TABLE{
 *      XBH_U32 iicBus;         //device iic bus，没有就填-1
 *      XBH_U8 iicAddr;         //device iic address，没有就填-1
 *      XBH_S32 sGpio;          //1切2的HDMI switch使用的切换引脚，没有就填-1
 *      XBH_S32 sLevel;         //1切2的HDMI switch 切到port0时的电平
 *      XBH_S32 pGpio;          //供电引脚，没有就填-1
 *      XBH_S32 pLevel;         //工作时的电平状态
 *      XBH_S32 rGpio;          //复位引脚，没有就填-1
 *      XBH_S32 rLevel;         //工作时的电平状态
 *      XBH_S32 defaultPort;    //默认端口
 *      XBH_S32 prePort;        //前一级hdmiswitch的序号id和对应的port,
                                //为了兼容以前的版本，switch id和 port都是从1开始
 *      XBH_S32 sGpio2;         // 新增：第二个HDMI switch切换GPIO,与上方1切2GPIO组合2切4 hdmi Switch
 *      XBH_S32 sLevel2;        // 新增：第二个HDMI switchGPIO电平
 *  };
 */
#undef XBH_F_WXQ_LG8195_311_3H_V11_HDMI_SWITCH_IIC_TABLE
#define XBH_F_WXQ_LG8195_311_3H_V11_HDMI_SWITCH_IIC_TABLE \
    {-1, -1, F_WXQ_LG8195_311_3H_V11_HDMI_SEL, F_WXQ_LG8195_311_3H_V11_HDMI_SEL_PORTA, -1, -1, -1, -1, 0, 0,F_WXQ_LG8195_311_3H_V11_HDMI_SEL2, F_WXQ_LG8195_311_3H_V11_HDMI_SEL2_PORTA}   

/*
 * HDMI SWITCH 器件的port口定义,必须是2x4的矩阵
 * struct HDMI_SWITCH_PORT{
 *      XBH_U8 port0;   //port0  0x00：悬空，0x01：连接下一级switch
 *      XBH_U8 port1;   //port1  0x00：悬空，0x01：连接下一级switch
 *      XBH_U8 port2;   //port2  0x00：悬空，0x01：连接下一级switch
 *      XBH_U8 port3;   //port3  0x00：悬空，0x01：连接下一级switch
 *  };
 */
#undef XBH_F_WXQ_LG8195_311_3H_V11_HDMI_SWITCH_PORT_TABLE
#define XBH_F_WXQ_LG8195_311_3H_V11_HDMI_SWITCH_PORT_TABLE \
    {XBH_SOURCE_F_USBC1,XBH_SOURCE_F_HDMI1, XBH_SOURCE_F_HDMI2, XBH_SOURCE_F_HDMI3}

/*
 * USBC转HDMI
 * USBC转HDMI IC的类型，按顺序排即可,不使用时填0
 */
#undef XBH_F_WXQ_LG8195_311_3H_V11_USBC_2_HDMI_TABLE
#define XBH_F_WXQ_LG8195_311_3H_V11_USBC_2_HDMI_TABLE \
    WXQ_LT8711UX
/*
 * USBC转HDMI 器件的操作元素
 struct COMMON_OPT_TABLE
 {
     XBH_S32 iicBus1;        //device iic bus
     XBH_S32 iicBus2;        //device iic bus，没有就填-1
     XBH_S32 iicAddr;        //device iic address, 自动识别的器件可以填-1，比如光感sensor
     XBH_S32 uart;           //如果使用串口通信，填使用的soc的uart的id，不使用时填-1
     XBH_S32 pGpio;          //供电引脚，没有就填-1
     XBH_S32 pLevel;         //工作时的电平状态
     XBH_S32 rGpio;          //复位引脚，没有就填-1
     XBH_S32 rLevel;         //复位工作时的电平状态
 };
 */
#undef XBH_F_WXQ_LG8195_311_3H_V11_USBC_2_HDMI_IIC_TABLE
#define XBH_F_WXQ_LG8195_311_3H_V11_USBC_2_HDMI_IIC_TABLE \
    {-1, -1, -1, -1, XBH_BOARD_GPIO_F_USBC_PWR, XBH_BOARD_GPIO_F_USBC_PWR_LEVEL, -1, -1}
/*
 * USBC 对应的 转型芯片，顺序跟XBH_USBC_2_HDMI_TABLE 一一对应
*/
#undef XBH_F_WXQ_LG8195_311_3H_V11_USBC_PORT_TABLE
#define XBH_F_WXQ_LG8195_311_3H_V11_USBC_PORT_TABLE \
    XBH_SOURCE_F_USBC1

/*******************************************************************************switch table*****************************************************************************/
static GPIO_OPT_TABLE source_android_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_android_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_src_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_src_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,           XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_SEL,               XBH_BOARD_GPIO_F_USB_SEL_B}},\
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,             !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_SEL,               XBH_BOARD_GPIO_F_USB_SEL_B}},\
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,             !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE source_f_hdmi3_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_SEL,               XBH_BOARD_GPIO_F_USB_SEL_B}},\
};

static GPIO_OPT_TABLE source_f_hdmi3_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,             !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

//FCT
static GPIO_OPT_TABLE fct_source_android_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_android_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_src_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_src_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               XBH_BOARD_GPIO_F_USB_PWR_LEVEL}}
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,              !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              !XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_RST,              XBH_BOARD_GPIO_F_HUB_RST_LEVEL}}, \
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_SEL,               XBH_BOARD_GPIO_F_USB_SEL_B}},\
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,             !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_SEL,               XBH_BOARD_GPIO_F_USB_SEL_B}},\
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,             !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};
static GPIO_OPT_TABLE fct_source_f_hdmi3_table_ext_before_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,               !XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,            XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_SEL,               XBH_BOARD_GPIO_F_USB_SEL_B}},\
};

static GPIO_OPT_TABLE fct_source_f_hdmi3_table_ext_after_wxq_lg8195_311_3h_v11[] = {
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_HUB_TX_RST,             !XBH_BOARD_GPIO_F_HUB_TX_RST_LEVEL}}, \
    {REQUEST_DELAY,              {300 * 1000}},\
    {REQUEST_DELAY,              {500 * 1000}},\
    {REQUEST_GPIO,               {XBH_BOARD_GPIO_F_USB_PWR,                XBH_BOARD_GPIO_F_USB_PWR_LEVEL}},\
};
#endif
