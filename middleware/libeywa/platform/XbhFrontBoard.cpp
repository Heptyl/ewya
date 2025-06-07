#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhFrontBoard"

#include "XbhLog.h"
#include "XbhFrontBoard.h"
#include "XbhService.h"
#include "XbhModuleInterface.h"

#include <cutils/properties.h>
#include <string.h>

#include "front2_p2.h"
#include "front2_u.h"
#include "front2_u2.h"
#include "front2_u2_2.h"
#include "front2_u3.h"
#include "front2_u3_2.h"
#include "front2_u4.h"
#include "front2_u8.h"
#include "front3_u5.h"
#include "front2_t2.h"
#include "front2_vs34.h"
#include "front2_s2.h"
#include "front2_u9.h"
#include "front3_u8.h"

XbhFrontBoard*                          XbhFrontBoard::mInstance = XBH_NULL;
XbhMutex                                XbhFrontBoard::mLock;
/*******************************************************************************FRONT 3U5*****************************************************************************/
static XBH_U8 f3u5_switch_table[1] = {
    XBH_F3U5_HDMI_SWITCH_TABLE
};

static HDMI_SW_OPT_TABLE f3u5_switch_iic_table[1] = {
    XBH_F3U5_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT f3u5_switch_port_table[1] = {
    XBH_F3U5_HDMI_SWITCH_PORT_TABLE
};
/*******************************************************************************FRONT 2P2*****************************************************************************/
static XBH_U8 f2p2_switch_table[1] = {
    XBH_F2P2_HDMI_SWITCH_TABLE
};

static HDMI_SW_OPT_TABLE f2p2_switch_iic_table[1] = {
    XBH_F2P2_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT f2p2_switch_port_table[1] = {
    XBH_F2P2_HDMI_SWITCH_PORT_TABLE
};
/*******************************************************************************FRONT U*****************************************************************************/
static XBH_U8 f2u_switch_table[1] = {
    XBH_F2U_HDMI_SWITCH_TABLE
};

static HDMI_SW_OPT_TABLE f2u_switch_iic_table[1] = {
    XBH_F2U_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT f2u_switch_port_table[1] = {
    XBH_F2U_HDMI_SWITCH_PORT_TABLE
};

static XBH_U8 f2u_usbc_table[2] = {
    XBH_F2U_USBC_2_HDMI_TABLE
};

static COMMON_OPT_TABLE f2u_usbc_iic_table[2] = {
    XBH_F2U_USBC_2_HDMI_IIC_TABLE
};

static XBH_SOURCE_E f2u_usbc_port_table[2] = {
    XBH_F2U_USBC_PORT_TABLE
};
/*******************************************************************************FRONT U2*****************************************************************************/
static XBH_U8 f2u2_switch_table[1] = {
    XBH_F2U2_HDMI_SWITCH_TABLE
};

static HDMI_SW_OPT_TABLE f2u2_switch_iic_table[1] = {
    XBH_F2U2_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT f2u2_switch_port_table[1] = {
    XBH_F2U2_HDMI_SWITCH_PORT_TABLE
};
/*******************************************************************************FRONT U3*****************************************************************************/
static XBH_U8 f2u3_usbc_table[2] = {
    XBH_F2U3_USBC_2_HDMI_TABLE
};

static COMMON_OPT_TABLE f2u3_usbc_iic_table[2] = {
    XBH_F2U3_USBC_2_HDMI_IIC_TABLE
};

static XBH_SOURCE_E f2u3_usbc_port_table[2] = {
    XBH_F2U3_USBC_PORT_TABLE
};
/*******************************************************************************FRONT U4*****************************************************************************/
static XBH_U8 f2u4_switch_table[1] = {
    XBH_F2U4_HDMI_SWITCH_TABLE
};

static COMMON_OPT_TABLE f2u4_switch_iic_table[1] = {
    XBH_F2U4_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT f2u4_switch_port_table[1] = {
    XBH_F2U4_HDMI_SWITCH_PORT_TABLE
};

static XBH_U8 f2u4_usbc_table[2] = {
    XBH_F2U4_USBC_2_HDMI_TABLE
};

static COMMON_OPT_TABLE f2u4_usbc_iic_table[2] = {
    XBH_F2U4_USBC_2_HDMI_IIC_TABLE
};

static XBH_SOURCE_E f2u4_usbc_port_table[2] = {
    XBH_F2U4_USBC_PORT_TABLE
};
/*******************************************************************************FRONT U8*****************************************************************************/
static XBH_U8 f2u8_switch_table[1] = {
    XBH_F2U8_HDMI_SWITCH_TABLE
};
static HDMI_SW_OPT_TABLE f2u8_switch_iic_table[1] = {
    XBH_F2U8_HDMI_SWITCH_IIC_TABLE
};
static HDMI_SWITCH_PORT f2u8_switch_port_table[1] = {
    XBH_F2U8_HDMI_SWITCH_PORT_TABLE
};
/*******************************************************************************FRONT U9*****************************************************************************/
static XBH_U8 f2u9_switch_table[1] = {
    XBH_F2U9_HDMI_SWITCH_TABLE
};
static HDMI_SW_OPT_TABLE f2u9_switch_iic_table[1] = {
    XBH_F2U9_HDMI_SWITCH_IIC_TABLE
};
static HDMI_SWITCH_PORT f2u9_switch_port_table[1] = {
    XBH_F2U9_HDMI_SWITCH_PORT_TABLE
};
/*******************************************************************************FRONT VS34*****************************************************************************/
static XBH_U8 fvs34_switch_table[1] = {
    XBH_FVS34_HDMI_SWITCH_TABLE
};
static HDMI_SW_OPT_TABLE fvs34_switch_iic_table[1] = {
    XBH_FVS34_HDMI_SWITCH_IIC_TABLE
};
static HDMI_SWITCH_PORT fvs34_switch_port_table[1] = {
    XBH_FVS34_HDMI_SWITCH_PORT_TABLE
};

/*******************************************************************************FRONT2 T2*****************************************************************************/
static XBH_U8 f2t2_switch_table[1] = {
    XBH_F2T2_HDMI_SWITCH_TABLE
};
static HDMI_SW_OPT_TABLE f2t2_switch_iic_table[1] = {
    XBH_F2T2_HDMI_SWITCH_IIC_TABLE
};
static HDMI_SWITCH_PORT f2t2_switch_port_table[1] = {
    XBH_F2T2_HDMI_SWITCH_PORT_TABLE
};

static XBH_U8 f2t2_pd_table[2] = {
    XBH_F2T2_PD_IC_TABLE
};

static COMMON_OPT_TABLE f2t2_pd_iic_table[2] = {
    XBH_F2T2_PD_IC_IIC_TABLE
};

static XBH_U8 f2t2_usbc_table[2] = {
    XBH_F2T2_USBC_TABLE
};

static COMMON_OPT_TABLE f2t2_usbc_iic_table[2] = {
    XBH_F2T2_USBC_IIC_TABLE
};

static XBH_SOURCE_E f2t2_usbc_port_table[2] = {
    XBH_F2T2_USBC_PORT_TABLE
};


/*******************************************************************************FRONT2 S2*****************************************************************************/

static XBH_U8 f2s2_switch_table[1] = {
    XBH_F2S2_HDMI_SWITCH_TABLE
};

static HDMI_SW_OPT_TABLE f2s2_switch_iic_table[1] = {
    XBH_F2S2_HDMI_SWITCH_IIC_TABLE
};

static HDMI_SWITCH_PORT f2s2_switch_port_table[1] = {
    XBH_F2S2_HDMI_SWITCH_PORT_TABLE
};

/*******************************************************************************FRONT3 U8*****************************************************************************/
static XBH_U8 f3u8_switch_table[1] = {
    XBH_F3U8_HDMI_SWITCH_TABLE
};
static HDMI_SW_OPT_TABLE f3u8_switch_iic_table[1] = {
    XBH_F3U8_HDMI_SWITCH_IIC_TABLE
};
static HDMI_SWITCH_PORT f3u8_switch_port_table[1] = {
    XBH_F3U8_HDMI_SWITCH_PORT_TABLE
};

/*******************************************************************************FUNC*****************************************************************************/
XBH_S32 XbhFrontBoard::getFrontBoardType(XBH_U8 *type)
{
    *type = mFboardType;
    return XBH_SUCCESS;
}
//hdmi switch
XBH_S32 XbhFrontBoard::getHdmiSwitchTable(XBH_U8 *table)
{
    *table = mSwitchType;
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getHdmiSwitchI2cTable(HDMI_SW_OPT_TABLE *table)
{
    table->iicBus = mOptTable.iicBus;
    table->iicAddr = mOptTable.iicAddr;
    table->sGpio = mOptTable.sGpio;
    table->sLevel = mOptTable.sLevel;
    table->pGpio = mOptTable.pGpio;
    table->pLevel = mOptTable.pLevel;
    table->rGpio = mOptTable.rGpio;
    table->rLevel = mOptTable.rLevel;
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getHdmiSwitchPortTable(HDMI_SWITCH_PORT *table)
{
    table->port0 = mPortTable.port0;
    table->port1 = mPortTable.port1;
    table->port2 = mPortTable.port2;
    table->port3 = mPortTable.port3;
    return XBH_SUCCESS;
}
//usbc to hdmi
XBH_S32 XbhFrontBoard::getUsbc2HdmiTable(XBH_U8 table[])
{
    for (int i = 0; i < sizeof(mUsbcType) / sizeof(mUsbcType[0]); i ++)
    {
        table[i] = mUsbcType[i];
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getUsbc2HdmiI2cTable(COMMON_OPT_TABLE table[])
{
    for (int i = 0; i < sizeof(mUsbcOptTable) / sizeof(mUsbcOptTable[0]); i++)
    {
        table[i].iicBus1 = mUsbcOptTable[i].iicBus1;
        table[i].iicBus2 = mUsbcOptTable[i].iicBus2;
        table[i].iicAddr = mUsbcOptTable[i].iicAddr;
        table[i].uart = mUsbcOptTable[i].uart;
        table[i].pGpio = mUsbcOptTable[i].pGpio;
        table[i].pLevel = mUsbcOptTable[i].pLevel;
        table[i].rGpio = mUsbcOptTable[i].rGpio;
        table[i].rLevel = mUsbcOptTable[i].rLevel;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getUsbc2HdmiPortTable(XBH_SOURCE_E port[])
{
    for (int i = 0; i < sizeof(mUsbcPort) / sizeof(mUsbcPort[0]); i++)
    {
        port[i] = (XBH_SOURCE_E)mUsbcPort[i];
    }
    return XBH_SUCCESS;
}
//dp to hdmi
XBH_S32 XbhFrontBoard::getDp2HdmiTable(XBH_U8 table[])
{
    for (int i = 0; i < sizeof(mDpType) / sizeof(mDpType[0]); i ++)
    {
        table[i] = mDpType[i];
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getDp2HdmiI2cTable(COMMON_OPT_TABLE table[])
{
    for (int i = 0; i < sizeof(mDpOptTable) / sizeof(mDpOptTable[0]); i++)
    {
        table[i].iicBus1 = mDpOptTable[i].iicBus1;
        table[i].iicBus2 = mDpOptTable[i].iicBus2;
        table[i].iicAddr = mDpOptTable[i].iicAddr;
        table[i].uart = mDpOptTable[i].uart;
        table[i].pGpio = mDpOptTable[i].pGpio;
        table[i].pLevel = mDpOptTable[i].pLevel;
        table[i].rGpio = mDpOptTable[i].rGpio;
        table[i].rLevel = mDpOptTable[i].rLevel;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getDp2HdmiPortTable(XBH_SOURCE_E port[])
{
    for (int i = 0; i < sizeof(mDpPort) / sizeof(mDpPort[0]); i++)
    {
        port[i] = (XBH_SOURCE_E)mDpPort[i];
    }
    return XBH_SUCCESS;
}
// pd芯片
XBH_S32 XbhFrontBoard::getPdIcTable(XBH_U8 table[])
{
    for (int i = 0; i < sizeof(mPdType) / sizeof(mPdType[0]); i ++)
    {
        table[i] = mPdType[i];
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::getPdIcI2cTable(COMMON_OPT_TABLE table[])
{
    for (int i = 0; i < sizeof(mPdOptTable) / sizeof(mPdOptTable[0]); i++)
    {
        table[i].iicBus1 = mPdOptTable[i].iicBus1;
        table[i].iicBus2 = mPdOptTable[i].iicBus2;
        table[i].iicAddr = mPdOptTable[i].iicAddr;
        table[i].uart = mPdOptTable[i].uart;
        table[i].pGpio = mPdOptTable[i].pGpio;
        table[i].pLevel = mPdOptTable[i].pLevel;
        table[i].rGpio = mPdOptTable[i].rGpio;
        table[i].rLevel = mPdOptTable[i].rLevel;
    }
    return XBH_SUCCESS;
}

/*******************************************************************************switch table*****************************************************************************/
/**
 * 前置板gpio的初始化逻辑
 * retval 0:success,-1:failure
*/
#define SELECT_GPIO_OPT_TABLE_EXT(tbl)    { \
        if (XbhService::getModuleInterface()->getFattMode() && GPIO_OPT_TABLE_SIZE(fct_##tbl) > 1) \
        { \
            XbhService::getModuleInterface()->select_table(fct_##tbl, GPIO_OPT_TABLE_SIZE(fct_##tbl)); \
            return XBH_SUCCESS; \
        } \
        XbhService::getModuleInterface()->select_table(tbl, GPIO_OPT_TABLE_SIZE(tbl)); \
    }

XBH_S32 XbhFrontBoard::followToAndroidExtBefore(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_before_3_u8);
        break;
        default:
            XLOGE(" followToAndroidExtBefore not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToAndroidExtAfter(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_android_table_ext_after_3_u8);
        break;
        default:
            XLOGE(" followToAndroidExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToSrcExtBefore(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_before_3_u8);
        break;
        default:
            XLOGE(" followToSrcExtBefore not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToSrcExtAfter(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_src_table_ext_after_3_u8);
        break;
        default:
            XLOGE(" followToSrcExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFUsbc1ExtBefore(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_before_3_u8);
        break;
        default:
            XLOGE(" followToFUsbc1ExtBefore not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFUsbc1ExtAfter(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc1_table_ext_after_3_u8);
        break;
        default:
            XLOGE(" followToFUsbc1ExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFUsbc2ExtBefore(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_before_3_u8);
        break;
        default:
            XLOGE(" followToFUsbc1ExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFUsbc2ExtAfter(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_usbc2_table_ext_after_3_u8);
        break;
        default:
            XLOGE(" followToFUsbc2ExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFHdmi1ExtBefore(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_before_3_u8);
        break;
        default:
            XLOGE(" followToFHdmi1ExtBefore not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFHdmi1ExtAfter(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_2_s2);
        break;
        case FRONT3_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi1_table_ext_after_3_u8);
        break;
        default:
            XLOGE(" followToFHdmi1ExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFHdmi2ExtBefore(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_before_2_s2);
        break;
        default:
            XLOGE(" followToFHdmi2ExtBefore not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::followToFHdmi2ExtAfter(XBH_VOID)
{
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_p2);
        break;
        case FRONT2_U_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u);
        break;
        case FRONT2_U2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u2);
        break;
        case FRONT2_U2_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u2_2);
        break;
        case FRONT2_U3_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u3);
        break;
        case FRONT2_U3_2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u3_2);
        break;
        case FRONT2_U4_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u4);
        break;
        case FRONT2_U8_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u8);
        break;
        case FRONT2_U9_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_u9);
        break;
        case FRONT3_U5_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_3_u5);
        break;
        case FRONT2_VS34_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_vs34);
        break;
        case FRONT2_T2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_t2);
        break;
        case FRONT2_S2_BOARD:
            SELECT_GPIO_OPT_TABLE_EXT(source_f_hdmi2_table_ext_after_2_s2);
        break;
        default:
            XLOGE(" followToFHdmi2ExtAfter not found front board ");
        break;
    }
    return XBH_SUCCESS;
}

XBH_S32 XbhFrontBoard::init()
{
    mFboardType = property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD);
    XLOGD("mFboardType = 0x%02x\n", mFboardType);
    switch(mFboardType)
    {
        case FRONT2_P2_BOARD:
            mSwitchType = f2p2_switch_table[0];
            memcpy(&mOptTable, &f2p2_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2p2_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT2_U_BOARD:
            mSwitchType = f2u_switch_table[0];
            memcpy(&mOptTable, &f2u_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2u_switch_port_table[0], sizeof(mPortTable));

            memcpy(&mUsbcType, &f2u_usbc_table[0], sizeof(mUsbcType));
            memcpy(&mUsbcOptTable, &f2u_usbc_iic_table[0], sizeof(mUsbcOptTable));
            memcpy(&mUsbcPort, &f2u_usbc_port_table[0], sizeof(mUsbcPort));
        break;
        case FRONT2_U2_BOARD:
            mSwitchType = f2u2_switch_table[0];
            memcpy(&mOptTable, &f2u2_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2u2_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT2_U2_2_BOARD:
        break;
        case FRONT2_U3_BOARD:
            memcpy(&mUsbcType, &f2u3_usbc_table[0], sizeof(mDpType));
            memcpy(&mUsbcOptTable, &f2u3_usbc_iic_table[0], sizeof(mDpOptTable));
            memcpy(&mUsbcPort, &f2u3_usbc_port_table[0], sizeof(mDpPort));
        break;
        case FRONT2_U3_2_BOARD:
        break;
        case FRONT2_U4_BOARD:
            mSwitchType = f2u4_switch_table[0];
            memcpy(&mPortTable, &f2u4_switch_port_table[0], sizeof(mPortTable));
            memcpy(&mOptTable, &f2u4_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mUsbcType, &f2u4_usbc_table[0], sizeof(mDpType));
            memcpy(&mUsbcOptTable, &f2u4_usbc_iic_table[0], sizeof(mDpOptTable));
            memcpy(&mUsbcPort, &f2u4_usbc_port_table[0], sizeof(mDpPort));
        break;
        case FRONT2_U8_BOARD:
            mSwitchType = f2u8_switch_table[0];
            memcpy(&mOptTable, &f2u8_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2u8_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT2_U9_BOARD:
            mSwitchType = f2u9_switch_table[0];
            memcpy(&mOptTable, &f2u9_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2u9_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT3_U5_BOARD:
            mSwitchType = f3u5_switch_table[0];
            memcpy(&mOptTable, &f3u5_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f3u5_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT2_VS34_BOARD:
            mSwitchType = fvs34_switch_table[0];
            memcpy(&mOptTable, &fvs34_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &fvs34_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT2_T2_BOARD:
            mSwitchType = f2t2_switch_table[0];
            memcpy(&mOptTable, &f2t2_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2t2_switch_port_table[0], sizeof(mPortTable));

            memcpy(&mPdType, &f2t2_pd_table[0], sizeof(mPdType));
            memcpy(&mPdOptTable, &f2t2_pd_iic_table[0], sizeof(mPdOptTable));

            memcpy(&mDpType, &f2t2_usbc_table[0], sizeof(mDpType));
            memcpy(&mDpOptTable, &f2t2_usbc_iic_table[0], sizeof(mDpOptTable));
            memcpy(&mDpPort, &f2t2_usbc_port_table[0], sizeof(mDpPort));
        break;
        case FRONT2_S2_BOARD:
            mSwitchType = f2s2_switch_table[0];
            memcpy(&mOptTable, &f2s2_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f2s2_switch_port_table[0], sizeof(mPortTable));
        break;
        case FRONT3_U8_BOARD:
            mSwitchType = f3u8_switch_table[0];
            memcpy(&mOptTable, &f3u8_switch_iic_table[0], sizeof(mOptTable));
            memcpy(&mPortTable, &f3u8_switch_port_table[0], sizeof(mPortTable));
        break;
        default:
        break;
    }
    return XBH_SUCCESS;
}

XbhFrontBoard::XbhFrontBoard()
{
    XLOGD(" begin ");
    init();
    XLOGD(" end ");
}

XbhFrontBoard::~XbhFrontBoard()
{

}

XbhFrontBoard* XbhFrontBoard::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhFrontBoard();
    }
    return mInstance;
}

