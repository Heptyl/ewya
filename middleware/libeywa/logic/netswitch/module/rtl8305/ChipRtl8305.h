#ifndef _PYH_ASIC_DRV_RTL8305_H_
#define _PYH_ASIC_DRV_RTL8305_H_
#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include <hardware/board.h>
#include "XbhNetSwitchInterface.h"

#include "rtl8367c_drv.h"

//---------------- custom device interface start ----------------
#define PHY_PAGE_ADDRESS                        31
#define RTK_MAX_NUM_OF_PORT                     10
#define RTK_PORT_ID_MAX                         (RTK_MAX_NUM_OF_PORT-1)
#define RTK_PHY_ID_MAX                          (RTK_MAX_NUM_OF_PORT-3)
#define RTL8367N_PHY_ID_MAX                     10

//  RJ45 In
#ifdef XBH_BOARD_ETHERNET_RJ45_IN_P1_G_LED
#define ETHERNET_LED_G1                         XBH_BOARD_ETHERNET_RJ45_IN_P1_G_LED
#define ETHERNET_LED_G1_ON                      XBH_BOARD_ETHERNET_RJ45_IN_P1_G_LED_ON
#else
#define ETHERNET_LED_G1                         -1
#define ETHERNET_LED_G1_ON                      -1
#endif

//  RJ45 Out
#ifdef XBH_BOARD_ETHERNET_RJ45_OUT_P2_G_LED
#define ETHERNET_LED_G2                         XBH_BOARD_ETHERNET_RJ45_OUT_P2_G_LED
#define ETHERNET_LED_G2_ON                      XBH_BOARD_ETHERNET_RJ45_OUT_P2_G_LED_ON
#else
#define ETHERNET_LED_G2                         -1
#define ETHERNET_LED_G2_ON                      -1
#endif

//---------------- custom device interface end ----------------

class ChipRtl8305 : public XbhMWThread, public XbhNetSwitchInterface
{
public:
    ChipRtl8305(XBH_S32 smi_SCK, XBH_S32 smi_SDA, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 androidPort, XBH_S32 typecPort, XBH_S32 opsPort, XBH_S32 rj45inPort, XBH_S32 rj45outPort);
    ~ChipRtl8305();
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
    /**
     * 设置chip网络唤醒使能
     */
    XBH_S32 setChipWol(XBH_BOOL bEnable);

    XBH_S32 rtk_port_phyReg_set(XBH_U32 port, rtk_port_phy_reg_t reg, XBH_U32 regData);
    XBH_S32 rtk_port_phyReg_get(XBH_U32 port, rtk_port_phy_reg_t reg, XBH_U32 *regData);
    XBH_S32 get_max_port();
    XBH_S32 rtk_port_phyAutoNegoAbility_set(int port, rtk_port_phy_ability_t *pAbility);

    XBH_S32 setOpsNetStatus(XBH_U32 bEnable);
    XBH_S32 getOpsNetStatus(XBH_U32 *pbEnable);
    XBH_S32 setTypecNetStatus(XBH_U32 bEnable);
    XBH_S32 getTypecNetStatus(XBH_U32 *pbEnable);
    XBH_S32 setAndroidNetStatus(XBH_U32 bEnable);
    XBH_S32 getAndroidNetStatus(XBH_U32 *pbEnable);
    XBH_S32 getSupportExChange(XBH_S32 nPort);
    XBH_S32 setEnable(XBH_SOURCE_E eSource, XBH_U32 bEnable);
    XBH_S32 getEnable(XBH_SOURCE_E eSource, XBH_U32 *pbEnable);
    void reset(void);

private:
    void _smiReadBit(XBH_U32 * pdata);
    void _smiWriteBit(XBH_U32 data);
    int smiRead(XBH_U32 phyad, XBH_U32 regad, XBH_U32 * data);
    int smiWrite(XBH_U32 phyad, XBH_U32 regad, XBH_U16 data);


private:
    XBH_S32 mInitDone;
    XBH_S32 m_bResetDone;
    XBH_S32 m_bTestModeDone;
    XBH_U32 m_s32HwVersion;
    XBH_U32 m_bOpsPortStatus;
    XBH_U32 m_bTypecPortStatus;
    XBH_U32 m_bAndroidPortStatus;
    XBH_U32 m_bRJ45INPortStatus;
    XBH_U32 m_bRJ45OUTPortStatus;
    XBH_U32 m_bLinkUpStatus;
    XBH_S32 m_s32LinkStatus[RTL8367N_PHY_ID_MAX];
    XBH_S32 m_s32LinkStatusLast[RTL8367N_PHY_ID_MAX];

    XBH_S32 mPowerGpio;
    XBH_S32 mRstGpio;
    XBH_S32 mPlevel;
    XBH_S32 mRlevel;
    XBH_S32 mdioGpio;
    XBH_S32 mdcGpio;
    XBH_S32 ANDROID_PORT;
    XBH_S32 TYPEC_PORT;
    XBH_S32 OPS_PORT;
    XBH_S32 RJ45_IN_PORT;
    XBH_S32 RJ45_OUT_PORT;

private:
    void run(const void* arg);
    static XbhMutex mLock;
    static XBH_S32 m_MaxPort;
};

#endif //_PYH_ASIC_DRV_RTL8305_H_

