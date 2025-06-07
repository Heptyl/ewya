#ifndef _RTK_TYPES_H_
#define _RTK_TYPES_H_
#include "XbhType.h"
typedef unsigned long long      rtk_uint64;
typedef long long               rtk_int64;
typedef unsigned int            rtk_uint32;
typedef int                     rtk_int32;
typedef unsigned short          rtk_uint16;
typedef short                   rtk_int16;
typedef unsigned char           rtk_uint8;
typedef char                    rtk_int8;
//typedef unsigned char       XBH_U8;
typedef unsigned long long      uint64;
typedef long long               int64;
typedef unsigned int            uint32;
typedef int                     int32;
typedef unsigned short          uint16;
typedef short                   int16;
typedef unsigned char           uint8;
typedef char                    int8;

#define CONST_T                 const


typedef uint32                  ipaddr_t;
typedef uint32                  memaddr;

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN          6
#endif

typedef struct ether_addr_s {
    uint8 octet[ETHER_ADDR_LEN];
} ether_addr_t;

typedef enum rtk_port_e
{
    UTP_PORT0 = 0,
    UTP_PORT1,
    UTP_PORT2,
    UTP_PORT3,
    UTP_PORT4,
    UTP_PORT5,
    UTP_PORT6,
    UTP_PORT7,

    EXT_PORT0 = 16,
    EXT_PORT1,
    EXT_PORT2,

    UNDEFINE_PORT = 30,
    RTK_PORT_MAX = 31
} rtk_port_t;

/* type abstraction */
typedef int32                   rtk_api_ret_t;
typedef int32                   ret_t;
typedef uint64                  rtk_u_long_t;

typedef enum init_state_e
{
    INIT_NOT_COMPLETED = 0,
    INIT_COMPLETED,
    INIT_STATE_END
} init_state_t;

static init_state_t    init_state = INIT_COMPLETED;


typedef enum switch_chip_e
{
    CHIP_RTL8367C = 0, //RTL8367RB_VB
    CHIP_RTL8370B,
    CHIP_RTL8364B,
    CHIP_RTL8363SC_VB,
    CHIP_RTL8367D,   //RTL8367RB_VC
    CHIP_RTL8367E,
    CHIP_END
}switch_chip_t;

typedef enum port_type_e
{
    UTP_PORT = 0,
    EXT_PORT,
    UNKNOWN_PORT = 0xFF,
    PORT_TYPE_END
}port_type_t;

#define RTK_SWITCH_PORT_NUM (32)

typedef struct rtk_switch_halCtrl_s
{
    switch_chip_t   switch_type;
    XBH_S32      l2p_port[RTK_SWITCH_PORT_NUM];
    XBH_S32      p2l_port[RTK_SWITCH_PORT_NUM];
    port_type_t     log_port_type[RTK_SWITCH_PORT_NUM];
    XBH_S32      ptp_port[RTK_SWITCH_PORT_NUM];
    XBH_S32      valid_portmask;
    XBH_S32      valid_utp_portmask;
    XBH_S32      valid_ext_portmask;
    XBH_S32      valid_cpu_portmask;
    XBH_S32      min_phy_port;
    XBH_S32      max_phy_port;
    XBH_S32      phy_portmask;
    XBH_S32      combo_logical_port;
    XBH_S32      hsg_logical_portmask;
    XBH_S32      sg_logical_portmask;
    XBH_S32      max_meter_id;
    XBH_S32      max_lut_addr_num;
    XBH_S32      trunk_group_mask;
    XBH_S32      packet_buffer_page_num;

}rtk_switch_halCtrl_t;


typedef enum rtk_int_type_e
{
    INT_TYPE_LINK_STATUS = 0,
    INT_TYPE_METER_EXCEED,
    INT_TYPE_LEARN_LIMIT,
    INT_TYPE_LINK_SPEED,
    INT_TYPE_CONGEST,
    INT_TYPE_GREEN_FEATURE,
    INT_TYPE_LOOP_DETECT,
    INT_TYPE_8051,
    INT_TYPE_CABLE_DIAG,
    INT_TYPE_ACL,
    INT_TYPE_RESERVED, /* Unused */
    INT_TYPE_SLIENT,
    INT_TYPE_END
}rtk_int_type_t;

typedef enum rtk_enable_e
{
    DISABLED = 0,
    ENABLED,
    RTK_ENABLE_END
}rtk_enable_t;


typedef enum rtk_meter_type_e{
    METER_TYPE_KBPS = 0,    /* Kbps */
    METER_TYPE_PPS,         /* Packet per second */
    METER_TYPE_END
}rtk_meter_type_t;

static rtk_switch_halCtrl_t rtl8367c_hal_Ctrl =
{
    /* Switch Chip */
    CHIP_RTL8367C,

    /* Logical to Physical */
    {0, 1, 2, 3, 4, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
     6, 7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },

    /* Physical to Logical */
    {UTP_PORT0, UTP_PORT1, UTP_PORT2, UTP_PORT3, UTP_PORT4, UNDEFINE_PORT, EXT_PORT0, EXT_PORT1,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT,
     UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT, UNDEFINE_PORT},

    /* Port Type */
    {UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UTP_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     EXT_PORT, EXT_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT,
     UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT, UNKNOWN_PORT},

    /* PTP port */
    {1, 1, 1, 1, 1, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0 },

    /* Valid port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) | (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid UTP port mask */
    ( (0x1 << UTP_PORT0) | (0x1 << UTP_PORT1) | (0x1 << UTP_PORT2) | (0x1 << UTP_PORT3) | (0x1 << UTP_PORT4) ),

    /* Valid EXT port mask */
    ( (0x1 << EXT_PORT0) | (0x1 << EXT_PORT1) ),

    /* Valid CPU port mask */
    0x00,

    /* Minimum physical port number */
    0,

    /* Maxmum physical port number */
    7,

    /* Physical port mask */
    0xDF,

    /* Combo Logical port ID */
    4,

    /* HSG Logical portmask */
    (0x1 << EXT_PORT0),

    /* SGMII Logical portmask */
    (0x1 << EXT_PORT0),

    /* Max Meter ID */
    31,

    /* MAX LUT Address Number */
    2112,

    /* Trunk Group Mask */
    0x03,

    /* Packet buffer page number */
    1024
};

static rtk_switch_halCtrl_t *halCtrl = &rtl8367c_hal_Ctrl;

#endif /* _RTL8370_TYPES_H_ */
