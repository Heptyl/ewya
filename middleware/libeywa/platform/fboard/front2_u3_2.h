#ifndef __COMMON_XBH_F2_U3_2_BOARD_H__
#define __COMMON_XBH_F2_U3_2_BOARD_H__

#include <hardware/board.h>

#ifdef XBH_BOARD_GPIO_F_IIC_BUS
#define F2U3_IIC_BUS                 XBH_BOARD_GPIO_F_IIC_BUS
#else
#define F2U3_IIC_BUS                -1
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_PWR
#define XBH_BOARD_GPIO_F_USBC_PWR -1
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_PWR_LEVEL
#define XBH_BOARD_GPIO_F_USBC_PWR_LEVEL -1
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_RST
#define XBH_BOARD_GPIO_F_USBC_RST -1
#endif

#ifndef XBH_BOARD_GPIO_F_USBC_RST_LEVEL
#define XBH_BOARD_GPIO_F_USBC_RST_LEVEL -1//工作时的电平状态(正常工作电平)
#endif

/*
 * USBC 转HDMI
 * USBC 转HDMI IC的类型，按顺序排即可,不使用时填0
 */
#undef XBH_F2U3_USBC_2_HDMI_TABLE
#define XBH_F2U3_USBC_2_HDMI_TABLE \
    GSV2201E
/*
 * DP转HDMI 器件的操作元素
 struct COMMON_OPT_TABLE
 {
     XBH_S32 iicBus1;        //device iic bus
     XBH_S32 iicBus2;        //device iic bus，没有就填-1
     XBH_S32 iicAddr;        //device iic address, 自动识别的器件可以填-1，比如光感sensor
     XBH_S32 uart;           //如果使用串口通信，填使用的soc的uart的id，不使用时填-1
     XBH_S32 pGpio;          //供电引脚，没有就填-1
     XBH_S32 pLevel;         //工作时的电平状态
     XBH_S32 rGpio;          //复位引脚，没有就填-1
     XBH_S32 rLevel;         //工作时的电平状态(正常工作电平)
 };
 */
#undef XBH_F2U3_USBC_2_HDMI_IIC_TABLE
#define XBH_F2U3_USBC_2_HDMI_IIC_TABLE \
    {F2U_IIC_BUS, -1, 0x00, -1, XBH_BOARD_GPIO_F_USBC_PWR, XBH_BOARD_GPIO_F_USBC_PWR_LEVEL, XBH_BOARD_GPIO_F_USBC_RST, XBH_BOARD_GPIO_F_USBC_RST_LEVEL}
/*
 * USBC 对应的 转型芯片，顺序跟XBH_USBC_2_HDMI_TABLE 一一对应
*/
#undef XBH_F2U3_USBC_PORT_TABLE
#define XBH_F2U3_USBC_PORT_TABLE \
    XBH_SOURCE_F_USBC1

/*******************************************************************************switch table*****************************************************************************/
static GPIO_OPT_TABLE source_android_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_android_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_src_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_src_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc1_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_usbc2_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi1_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE source_f_hdmi2_table_ext_after_2_u3_2[] = {
    
};

//FCT
static GPIO_OPT_TABLE fct_source_android_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_android_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_src_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_src_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc1_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_usbc2_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi1_table_ext_after_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_before_2_u3_2[] = {
    
};

static GPIO_OPT_TABLE fct_source_f_hdmi2_table_ext_after_2_u3_2[] = {
    
};
#endif
