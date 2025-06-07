#ifndef XBH_RTL_8367_H
#define XBH_RTL_8367_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhNetSwitchInterface.h"
#include "rtk_types.h"
#include "rtl8367c_drv.h"
#include <hardware/board.h>


#define PHY_PAGE_ADDRESS                        31
#define RTK_MAX_NUM_OF_PORT                     10
#define RTK_PORT_ID_MAX                         (RTK_MAX_NUM_OF_PORT-1)
#define RTK_PHY_ID_MAX                          (RTK_MAX_NUM_OF_PORT-3)
#define RTL8367RB_PHY_ID_MAX                    5

class ChipRtl8367 : public XbhMWThread, public XbhNetSwitchInterface
{
//---------------- custom device interface start ----------------
public:
    /**
     * smi_SCK      mido clock 引脚
     * smi_SDA      mido data 引脚
     * pGpio        芯片供电引脚
     * pLevel       供电时的工作电平
     * rGpio        芯片复位引脚
     * rLevel       复位时的工作电平
    */
    ChipRtl8367(XBH_S32 smi_SCK, XBH_S32 smi_SDA, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 androidPort, XBH_S32 typecPort, XBH_S32 opsPort, XBH_S32 rj45inPort, XBH_S32 rj45outPort);
    ~ChipRtl8367();
    /**
     * 设置交换机的port的开关状态，通常是设置OPS，TYPEC端口的网络开关
     * 连接Android的端口通常不需要操作
    */
    XBH_S32 setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable);
    /**
     * 获取交换机的port的开关状态，通常是获取OPS，TYPEC端口的网络开关
    */
    XBH_S32 getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable);
    /**
     * 获取交换机的RJ45的连接状态，通常是输入输出口
    */
    XBH_S32 getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus);
    /**
     * 设置交换机的输入输出的RJ45端口的指示灯工作状态
    */
    XBH_S32 setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable);
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);
    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_S32 * s32State);
    XBH_S32 setOpsNetStatus(XBH_S32 bEnable);
    XBH_S32 getOpsNetStatus(XBH_S32 *pbEnable);
    /**
     * 设置chip网络唤醒使能
     */
    XBH_S32 setChipWol(XBH_BOOL bEnable);
     /**
     * 设置wol端口电压
     */
    XBH_S32 setWolPwer(XBH_BOOL bEnable);
    /**********rtl8367 general interface*************/
    rtk_int32 rtl8367c_smi_read(rtk_uint32 mAddrs, rtk_uint32 *rData);
    rtk_int32 rtl8367c_smi_write(rtk_uint32 mAddrs, rtk_uint32 rData);
    ret_t rtl8367c_setAsicRegBit(rtk_uint32 reg, rtk_uint32 bit, rtk_uint32 value);
    ret_t rtl8367c_getAsicRegBit(rtk_uint32 reg, rtk_uint32 bit, rtk_uint32 *pValue);
    ret_t rtl8367c_setAsicRegBits(rtk_uint32 reg, rtk_uint32 bits, rtk_uint32 value);
    ret_t rtl8367c_getAsicRegBits(rtk_uint32 reg, rtk_uint32 bits, rtk_uint32 *pValue);
    ret_t rtl8367c_setAsicReg(rtk_uint32 reg, rtk_uint32 value);
    ret_t rtl8367c_getAsicReg(rtk_uint32 reg, rtk_uint32 *pValue);
    ret_t rtl8367c_setAsicPHYOCPReg(rtk_uint32 phyNo, rtk_uint32 ocpAddr, rtk_uint32 ocpData );
    ret_t rtl8367c_getAsicPHYOCPReg(rtk_uint32 phyNo, rtk_uint32 ocpAddr, rtk_uint32 *pRegData );
    ret_t rtl8367c_setAsicPHYReg(rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 phyData );
    ret_t rtl8367c_getAsicPHYReg(rtk_uint32 phyNo, rtk_uint32 phyAddr, rtk_uint32 *pRegData );
    ret_t rtl8367_readId(void);

    /**********rtl8367RB_VB  interface*************/
    ret_t dal_rtl8367c_reset(void);
    rtk_api_ret_t dal_rtl8367c_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t mode);
    rtk_api_ret_t dal_rtl8367c_wol_init(void);
    rtk_api_ret_t dal_rtl8367c_wol_enable_set(rtk_uint32 enable);
    rtk_api_ret_t dal_rtl8367c_wol_enable_get(rtk_uint32* enable);
    rtk_api_ret_t dal_rtl8367c_wol_mac_addr_set(ether_addr_t *pMac);
    ret_t dal_rtl8367c_port_phyTestMode_get(rtk_port_t port);

    /**********rtl8367RB_VC interface*************/
    rtk_api_ret_t dal_rtl8367d_wol_enable_set(rtk_uint32 enable);
    rtk_api_ret_t dal_rtl8367d_wol_mac_addr_set(ether_addr_t *pMac);
    rtk_api_ret_t dal_rtl8367d_int_control_set(rtk_int_type_t type, rtk_enable_t enable);
    ret_t dal_rtl8367d_rate_shareMeter_set(rtk_uint32 index, rtk_meter_type_t type, rtk_uint32 rate, rtk_enable_t ifg_include);
    ret_t dal_rtl8367d_switch_init();
    ret_t rtk_switch_isExtPort(rtk_port_t logicalPort);
    ret_t rtk_switch_port_L2P_get(int logicalPort);
    ret_t dal_rtl8367d_port_macForceLink_set(rtk_port_t port, rtk_port_mac_ability_t *pPortability);
    ret_t dal_rtl8367d_port_macForceLink_get(rtk_port_t port, rtk_port_mac_ability_t *pPortability);
    ret_t dal_rtl8367d_setAsicPortExtMode(rtk_uint32 id, rtk_uint32 mode);
    ret_t dal_rtl8367d_port_phyComboPortMedia_get(rtk_port_t port, rtk_port_media_t *pMedia);
    ret_t dal_rtl8367d_port_macForceLinkExt_set(rtk_port_t port, rtk_mode_ext_t mode, rtk_port_mac_ability_t *pPortability);
    ret_t dal_rtl8367d_port_rgmiiDelayExt_set(rtk_port_t port, int txDelay, int rxDelay);
    ret_t rtk_8367rb_vc_init(void);
private:
    XBH_S32 mInitDone;
    XBH_S32 mInitFirst;
    XBH_S32 mOpsPortStatus;
    XBH_S32 mLinkStatus[RTL8367RB_PHY_ID_MAX];
    XBH_S32 mLinkStatusLast[RTL8367RB_PHY_ID_MAX];

    XBH_S32 mPowerGpio;
    XBH_S32 mRstGpio;
    XBH_S32 mPlevel;
    XBH_S32 mRlevel;
    XBH_S32 mMdioGpio;    /* GPIO used for SMI Data signal */ 
    XBH_S32 mMdcGpio;     /* GPIO used for SMI Clock Generation */
    XBH_S32 ETHERNET_ANDROID_PROT;
    XBH_S32 ETHERNET_TYPEC_PROT;
    XBH_S32 ETHERNET_OPS_PORT;
    XBH_S32 ETHERNET_RJ45_IN_PROT;
    XBH_S32 ETHERNET_RJ45_OUT_PROT;

//---------------- custom device interface end ----------------
private:
    void run(const void* arg);
    static XbhMutex mLock;
    int32_t _mtk_reset(void);
    void _smi_start(void);
    void _smi_writeBit(rtk_uint16 signal, rtk_uint32 bitLen);
    void _smi_readBit(rtk_uint32 bitLen, rtk_uint32 *rData);
    void _smi_stop(void);
};

#endif //XBH_RTL_8367_H
