#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipRtl8305"
#include "XbhLog.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <cutils/properties.h>
#include "XbhNetSwitchTask.h"
#include "ChipRtl8305.h"
#include "XbhService.h"

#define RTL8306_MAX_PHY_ID                   10
#define PROPERTY_VALUE_MAX                   92
XbhMutex                    ChipRtl8305::mLock;
XBH_S32                     ChipRtl8305::m_MaxPort = 5;
XBH_S32                     Port[5] = {2,0,5,6,7};

#define RTL8305_DELAY                       100
#define RTL8305_CLK_DURATION(clk)           { int i; for(i=0; i<clk; i++); }

#define RTL8305_MutexLock()
#define RTL8305_MutexUnlock()

//---------------- custom device interface start ----------------
/**
 * 设置交换机的port的开关状态，通常是设置OPS，TYPEC端口的网络开关
 * 连接Android的端口通常不需要操作
*/
XBH_S32 ChipRtl8305::setPortEnable(XBH_SOURCE_E source, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取交换机的port的开关状态，通常是获取OPS，TYPEC端口的网络开关
*/
XBH_S32 ChipRtl8305::getPortEnable(XBH_SOURCE_E source, XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取交换机的RJ45的连接状态，通常是输入输出口
*/
XBH_S32 ChipRtl8305::getRJ45PlugState(XBH_U32 port, XBH_BOOL *pbStatus)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 ret = 0;
    if (port == RJ45_IN_PORT)
    {
         ret = m_s32LinkStatus[RJ45_IN_PORT];
        *pbStatus = ret;
    }
    else if (port == RJ45_OUT_PORT)
    {
         ret = m_s32LinkStatus[RJ45_OUT_PORT];
        *pbStatus =ret;
    }
    XLOGE("getRJ45PlugState phy %d ret %x\n", port, ret);
    return s32Ret;
}

/**
 * 设置交换机的输入输出的RJ45端口的指示灯工作状态
*/
XBH_S32 ChipRtl8305::setRJ45PortLedStatus(XBH_U32 port, XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 ChipRtl8305::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 ChipRtl8305::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipRtl8305::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 ChipRtl8305::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 ChipRtl8305::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 设置chip网络唤醒使能
 */
XBH_S32 ChipRtl8305::setChipWol(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/*Generate  1 -> 0 transition and sampled at 1 to 0 transition time,
 *should not sample at 0->1 transition because some chips stop outputing
 *at the last bit at rising edge
 */
void ChipRtl8305::_smiReadBit(XBH_U32 * pdata)
{
    XBH_U32 value = 0;
    RTL8305_CLK_DURATION(RTL8305_DELAY);
    XbhService::getModuleInterface()->setGpioOutputValue(mdcGpio, XBH_BOARD_GPIO_HIGH);
    RTL8305_CLK_DURATION(RTL8305_DELAY);
    XbhService::getModuleInterface()->setGpioOutputValue(mdcGpio, XBH_BOARD_GPIO_LOW);
    XbhService::getModuleInterface()->getGpioInputValue(mdioGpio, &value);
    *pdata = value;
}

/* Generate  0 -> 1 transition and put data ready during 0 to 1 whole period */
void ChipRtl8305::_smiWriteBit(XBH_U32 data) 
{
    if (data)
    {  /*write 1*/
        XbhService::getModuleInterface()->setGpioOutputValue(mdioGpio, XBH_BOARD_GPIO_HIGH);
        RTL8305_CLK_DURATION(RTL8305_DELAY);
        XbhService::getModuleInterface()->setGpioOutputValue(mdcGpio, XBH_BOARD_GPIO_HIGH);
        RTL8305_CLK_DURATION(RTL8305_DELAY);
        XbhService::getModuleInterface()->setGpioOutputValue(mdcGpio, XBH_BOARD_GPIO_LOW);
    }
    else 
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mdioGpio, XBH_BOARD_GPIO_LOW);
        RTL8305_CLK_DURATION(RTL8305_DELAY);
        XbhService::getModuleInterface()->setGpioOutputValue(mdcGpio, XBH_BOARD_GPIO_HIGH);
        RTL8305_CLK_DURATION(RTL8305_DELAY);
        XbhService::getModuleInterface()->setGpioOutputValue(mdcGpio, XBH_BOARD_GPIO_LOW);
    }
}

/* Function Name:
 *      smiRead
 * Description:
 *      Read data from phy register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31) 
 * Output:
 *      data    -  Register value 
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 *     This function could read register through MDC/MDIO serial 
 *     interface, and it is platform  related. It use two GPIO pins 
 *     to simulate MDC/MDIO timing. MDC is sourced by the Station Management 
 *     entity to the PHY as the timing reference for transfer of information
 *     on the MDIO signal. MDC is an aperiodic signal that has no maximum high 
 *     or low times. The minimum high and low times for MDC shall be 160 ns each, 
 *     and the minimum period for MDC shall be 400 ns. Obeying frame format defined
 *     by IEEE802.3 standard, you could access Phy registers. If you want to 
 *     port it to other CPU, please modify static functions which are called 
 *      by this function.
 */
int ChipRtl8305::smiRead(XBH_U32 phyad, XBH_U32 regad, XBH_U32 * data) 
{
    int i;
    XBH_U32 value = 0;

    XBH_U32 readBit;

    if ((phyad > RTL8306_MAX_PHY_ID) || (regad > 31) || (data == NULL))
        return -1;

    /*it lock the resource to ensure that SMI opertion is atomic, 
     *the API is based on RTL865X, it is used to disable CPU interrupt,
     *if porting to other platform, please rewrite it to realize the same function
     */
    RTL8305_MutexLock();

    /* 32 continuous 1 as preamble*/
    for(i=0; i<32; i++)
        _smiWriteBit(1);

    /* ST: Start of Frame, <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);

    /* OP: Operation code, read is <10>*/
    _smiWriteBit(1);
    _smiWriteBit(0);

    /* PHY Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((phyad>>i)&0x1);

    /* Register Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((regad>>i)&0x1);

    //gpioMDIO->SetInput();
    XbhService::getModuleInterface()->getGpioInputValue(mdioGpio, &value);
    
    /* TA: Turnaround <z0>, zbit has no clock in order to steal a clock to
     *  sample data at clock falling edge 
     */
    //_smiZbit();
    //_smiWriteBit(0);

    /* check the turnaround bit: the PHY should be driving it to zero */
    _smiReadBit(&readBit);

    /* Data */
    *data = 0;
    for(i=15; i>=0; i--) 
    {
        _smiReadBit(&readBit);
        *data = ((*data) << 1) | readBit;
    }

    /*add  an extra clock cycles for robust reading , ensure partner stop 
     *output signal and not affect the next read operation, because TA 
     *steal a clock*/
    //_smiWriteBit(1);
    //_smiZbit();
    _smiReadBit(&readBit);

    XbhService::getModuleInterface()->setGpioOutputValue(mdioGpio, XBH_BOARD_GPIO_LOW);

    /*unlock the source, enable interrupt*/
    RTL8305_MutexUnlock();

    return  0;
}

/* Function Name:
 *      smiWrite
 * Description:
 *      Write data to Phy register
 * Input:
 *      phyad   - PHY address (0~31)
 *      regad   -  Register address (0 ~31)
 *      data    -  Data to be written into Phy register
 * Output:
 *      none
 * Return:
 *      SUCCESS         -  Success
 *      FAILED            -  Failure
 * Note:
 *     This function could read register through MDC/MDIO serial 
 *     interface, and it is platform  related. It use two GPIO pins 
 *     to simulate MDC/MDIO timing. MDC is sourced by the Station Management 
 *     entity to the PHY as the timing reference for transfer of information
 *     on the MDIO signal. MDC is an aperiodic signal that has no maximum high 
 *     or low times. The minimum high and low times for MDC shall be 160 ns each, 
 *     and the minimum period for MDC shall be 400 ns. Obeying frame format defined
 *     by IEEE802.3 standard, you could access Phy registers. If you want to 
 *     port it to other CPU, please modify static functions which are called 
*      by this function.
 */
int ChipRtl8305::smiWrite(XBH_U32 phyad, XBH_U32 regad, XBH_U16 data)
{
    int i;
    XBH_U32 readBit;

    if ((phyad > RTL8306_MAX_PHY_ID) || (regad > 31) || (data > 0xFFFF))
        return -1;

    /*it lock the resource to ensure that SMI opertion is atomic, 
      *the API is based on RTL865X, it is used to disable CPU interrupt,
      *if porting to other platform, please rewrite it to realize the same function
      */
    RTL8305_MutexLock();

    /* 32 continuous 1 as preamble*/
    for(i=0; i<32; i++)
        _smiWriteBit(1);

    /* ST: Start of Frame, <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);

    /* OP: Operation code, write is <01>*/
    _smiWriteBit(0);
    _smiWriteBit(1);

    /* PHY Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((phyad>>i)&0x1);

    /* Register Address */
    for(i=4; i>=0; i--) 
        _smiWriteBit((regad>>i)&0x1);

    /* TA: Turnaround <10>*/
    _smiWriteBit(1);
    _smiWriteBit(0);

    /* Data */
    for(i=15; i>=0; i--) 
        _smiWriteBit((data>>i)&0x1);

    //_smiZbit();
    _smiReadBit(&readBit);

    /*unlock the source, enable interrupt*/
    RTL8305_MutexUnlock();

    return 0; 
}

/* Function Name:
 *      get_max_port
 * Description:
 * Input:
 * Output:
 *      None 
 * Return: MAX PHY ID
 * Note:
 *      This API can set PHY register data of the specific port.
 */
XBH_S32 ChipRtl8305::get_max_port(){
    return RTL8306_MAX_PHY_ID;
}

/* Function Name:
 *      rtk_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port.
 * Input:
 *      port - port id.
 *      reg - Register id
 *      regData - Register data
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 * Note:
 *      This API can set PHY register data of the specific port.
 */
XBH_S32 ChipRtl8305::rtk_port_phyReg_set(XBH_U32 port, rtk_port_phy_reg_t reg, XBH_U32 regData)
{
    if(port > RTL8306_MAX_PHY_ID)
        return RT_ERR_FAILED;

    /*write register*/
    regData &= 0xFFFF;
    smiWrite(port, reg, regData);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port.
 * Input:
 *      port - Port id.
 *      reg - Register id
 * Output:
 *      pData - Register data
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 * Note:
 *      This API can get PHY register data of the specific port. 
 */
XBH_S32 ChipRtl8305::rtk_port_phyReg_get(XBH_U32 port, rtk_port_phy_reg_t reg, XBH_U32 *pData) 
{
    XBH_U32 regval;

    if(port > RTL8306_MAX_PHY_ID)
        return RT_ERR_FAILED;

    if (NULL == pData)
        return RT_ERR_FAILED;

    *pData = 0;

    /*read phy register*/
    smiRead(port, reg, &regval);

    if((regval & 0xFFFF) == 0xFFFF)
        return RT_ERR_FAILED;

    *pData = regval & 0xFFFF;

    return 0;
}

XBH_S32 ChipRtl8305::rtk_port_phyAutoNegoAbility_set(int port, rtk_port_phy_ability_t *pAbility)
{
    int retVal;
    XBH_U32 phyData;
    XBH_U32 phyEnMsk0;
    XBH_U32 phyEnMsk4;

    if(port > RTK_PHY_ID_MAX)
        return RT_ERR_PORT_ID;

    if(NULL == pAbility)
        return RT_ERR_NULL_POINTER;

    if(pAbility->Half_10 >= 2  || pAbility->Full_10 >= 2 ||
       pAbility->Half_100 >= 2 || pAbility->Full_100 >= 2 ||
       pAbility->AutoNegotiation >= 2 ||pAbility->AsyFC >= 2 ||
       pAbility->FC >= 2)
        return RT_ERR_INPUT;

    /*for PHY auto mode setup*/
    pAbility->AutoNegotiation = 1; 

    phyEnMsk0 = 0;
    phyEnMsk4 = 0;
    
    if(1 == pAbility->Half_10)
    {
        /*10BASE-TX half duplex capable in reg 4.5*/
        phyEnMsk4 = phyEnMsk4 | (1<<5);
    }

    if(1 == pAbility->Full_10)
    {
        /*10BASE-TX full duplex capable in reg 4.6*/
        phyEnMsk4 = phyEnMsk4 | (1<<6);
    }

    if(1 == pAbility->Half_100)
    {
        /*100BASE-TX half duplex capable in reg 4.7*/
        phyEnMsk4 = phyEnMsk4 | (1<<7);
    }

    if(1 == pAbility->Full_100)
    {
        /*100BASE-TX full duplex capable in reg 4.8*/
        phyEnMsk4 = phyEnMsk4 | (1<<8);
    }
    
    if(1 == pAbility->AutoNegotiation)
    {
        /*Auto-Negotiation enable and restart in reg 0.12*/
        phyEnMsk0 |= (1 << 12);

    }

    if(1 == pAbility->AsyFC)
    {
        /*Asymetric flow control in reg 4.11*/
        phyEnMsk4 = phyEnMsk4 | (1<<11);
    }
    
    if(1 == pAbility->FC)
    {
        /*Flow control in reg 4.10*/
        phyEnMsk4 = phyEnMsk4 | (1<<10);
    }

    /*Auto-Negotiation control register setting*/
    if((retVal = rtk_port_phyReg_get(port, (rtk_port_phy_reg_t)PHY_REG_AN_ADVERTISEMENT, &phyData)) != RT_ERR_OK)
        return retVal;

    phyData = (phyData & (~0x0DE0)) | phyEnMsk4;
    if((retVal = rtk_port_phyReg_set(port, (rtk_port_phy_reg_t)PHY_REG_AN_ADVERTISEMENT, phyData)) != RT_ERR_OK)
        return retVal;

    /*Control register setting and restart auto*/
    if((retVal = rtk_port_phyReg_get(port, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData)) != RT_ERR_OK)
        return retVal;
    phyData = (phyData & (~0x3100)) | phyEnMsk0;
    
    /*If have auto-negotiation capable, then restart auto negotiation*/
    if(1 == pAbility->AutoNegotiation)
    {
        phyData = phyData | (1 << 9);
    }

    if((retVal = rtk_port_phyReg_set(port, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData)) != RT_ERR_OK)
        return retVal;    

    return RT_ERR_OK;
}

XBH_S32 ChipRtl8305::setOpsNetStatus(XBH_U32 bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 phyData = 0;
    if (bEnable)
    {
        s32Ret |= rtk_port_phyReg_get(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
        XLOGD("setOpsNetStatus phyData:%d,bEnable:%d\n", phyData, bEnable);
        phyData |= (0x1<<15);//reset
        phyData &= ~(0x1<<11);//power on
        s32Ret |= rtk_port_phyReg_set(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update OPS port status
    }
    else
    {
        s32Ret |= rtk_port_phyReg_get(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
        XLOGD("setOpsNetStatus phyData:%d,bEnable:%d\n", phyData, bEnable);
        phyData |= (0x1<<11);//power down
        s32Ret |= rtk_port_phyReg_set(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update OPS port status
    }
    m_bOpsPortStatus = bEnable;
    return s32Ret;
}

XBH_S32 ChipRtl8305::getOpsNetStatus(XBH_U32 *pbEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *pbEnable = m_bOpsPortStatus;
    return s32Ret;
}

XBH_S32 ChipRtl8305::setTypecNetStatus(XBH_U32 bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 phyData = 0;
    if (bEnable)
    {
        s32Ret |= rtk_port_phyReg_get(TYPEC_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
        XLOGD("setTypecNetStatus phyData:%d,bEnable:%d\n", phyData, bEnable);
        phyData |= (0x1<<15);//reset
        phyData &= ~(0x1<<11);//power on
        s32Ret |= rtk_port_phyReg_set(TYPEC_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update TYPEC port status
    }
    else
    {
        s32Ret |= rtk_port_phyReg_get(TYPEC_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
        XLOGD("setTypecNetStatus phyData:%d,bEnable:%d\n", phyData, bEnable);
        phyData |= (0x1<<11);//power down
        s32Ret |= rtk_port_phyReg_set(TYPEC_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update OPS port status
    }
    m_bTypecPortStatus = bEnable;
    return s32Ret;
}

XBH_S32 ChipRtl8305::getTypecNetStatus(XBH_U32 *pbEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *pbEnable = m_bTypecPortStatus;
    return s32Ret;
}

XBH_S32 ChipRtl8305::setAndroidNetStatus(XBH_U32 bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 phyData = 0;
    if (bEnable)
    {
        s32Ret |= rtk_port_phyReg_get(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
        XLOGD("setAndroidNetStatus phyData:%d,bEnable:%d\n", phyData, bEnable);
        phyData |= (0x1<<15);//reset
        phyData &= ~(0x1<<11);//power on
        s32Ret |= rtk_port_phyReg_set(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update ANDROID port status
    }
    else
    {
        s32Ret |= rtk_port_phyReg_get(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
        XLOGD("setAndroidNetStatus phyData:%d,bEnable:%d\n", phyData, bEnable);
        phyData |= (0x1<<11);//power down
        s32Ret |= rtk_port_phyReg_set(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update ANDROID port status
    }
    m_bAndroidPortStatus = bEnable;
    return s32Ret;
}

XBH_S32 ChipRtl8305::getAndroidNetStatus(XBH_U32 *pbEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *pbEnable = m_bAndroidPortStatus;
    return s32Ret;
}


XBH_S32 ChipRtl8305::getSupportExChange(XBH_S32 nPort)
{
    XBH_U32 phyData = 0;
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = rtk_port_phyReg_get(RJ45_IN_PORT, (rtk_port_phy_reg_t)PHY_REG_AN_ADVERTISEMENT, &phyData);
    XLOGE("getPortStatus PHY_REG_AN_ADVERTISEMENT phy %d ret %x\n", RJ45_IN_PORT, s32Ret);
    return s32Ret;
}

XBH_S32 ChipRtl8305::setEnable(XBH_SOURCE_E eSource, XBH_U32 bEnable)
{
    XBH_S32 nRet = XBH_FAILURE;

    XbhMutex::Autolock _l(mLock);
    switch(eSource)
    {
        case XBH_SOURCE_ANDROID:
            nRet = setAndroidNetStatus(bEnable);
            break;
        case XBH_SOURCE_USBC1:
            nRet = setTypecNetStatus(bEnable);
            break;
        case XBH_SOURCE_OPS1:
            nRet = setOpsNetStatus(bEnable);
            break;
        default:
            break;
    }

    return nRet;
}

XBH_S32 ChipRtl8305::getEnable(XBH_SOURCE_E eSource, XBH_U32 *pbEnable)
{
    XBH_S32 nRet = XBH_FAILURE;

    XbhMutex::Autolock _l(mLock);
    if (pbEnable == XBH_NULL)
    {
        XLOGD("getEnable pbEnable is null!!!");
        return nRet;
    }
    switch(eSource)
    {
        case XBH_SOURCE_ANDROID:
            nRet = getAndroidNetStatus(pbEnable);
            break;
        case XBH_SOURCE_F_USBC1:
            nRet = getTypecNetStatus(pbEnable);
            break;
        case XBH_SOURCE_OPS1:
            nRet = getOpsNetStatus(pbEnable);
            break;
        default:
            break;
    }

    return nRet;
}

void ChipRtl8305::run(const void* arg)
{
    int ret;
    XBH_S32 i = 0;
    XBH_U32 phyData = 0;
    //XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
    //XLOGD("rtk8305 task start--m_bInitDone:%d\n", m_bInitDone);
    /*if (!m_bResetDone)
    {
        if(property_get("sys.xbh.usblogic.inited", propVal, "false") > 0)
        {
            if(strcmp(propVal, "true") == 0)
            {
                XLOGD("XbhNeticTask reset");
                m_bResetDone = XBH_TRUE;
                usleep(3000 * 1000);
                reset();
                return;
            }
        }
        usleep(1000 * 1000);
        return;
    }*/

    if(!mInitDone)
    {
        XBH_CHAR flowctrl[PROPERTY_VALUE_MAX];
        property_get("persist.vendor.xbh.flowctrl.enable", flowctrl, "false");
        rtk_port_phy_ability_t adility;
        adility.AsyFC = 0;
        adility.FC = 0;
        adility.AutoNegotiation = 1;
        adility.Half_10 = 1;
        adility.Full_10 = 1;
        adility.Half_100 = 1;
        adility.Full_100 = 1;
        adility.Full_1000 = 1;

        //add to enable/disable flow control capability, lgh 20190505
        XLOGD("rtk8305 task m_MaxPort:%d\n", m_MaxPort);
        for(i = 0; i < m_MaxPort; i ++)
        {
            if(strcmp(flowctrl, "true") == 0)
            {
                break;
            }

            ret = rtk_port_phyReg_get(Port[i], (rtk_port_phy_reg_t)PHY_REG_AN_ADVERTISEMENT, &phyData);
            if(ret != 0)
            {
                XLOGE("rtk8305 PHY_REG_AN_ADVERTISEMENT phy %d ret %x\n", Port[i], ret);
                continue;
            }
            XLOGD("rtk8305 PHY_REG_AN_ADVERTISEMENT before phy %d %#x\n", Port[i], phyData);

            //disable flow control capability
            #if 0
            phyData &= ~(0x1<<10);
            rtk_port_phyReg_set(Port[i], (rtk_port_phy_reg_t)PHY_REG_AN_ADVERTISEMENT, phyData);
            #endif
            //Restart Auto Negotiation
            rtk_port_phyReg_get(Port[i], (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
            phyData |= (0x1<<9);
            rtk_port_phyReg_set(Port[i], (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);
            rtk_port_phyAutoNegoAbility_set(Port[i], &adility);//fixed for RTL8306MBs       
            ret = rtk_port_phyReg_get(Port[i], (rtk_port_phy_reg_t)PHY_REG_AN_ADVERTISEMENT, &phyData);
            if(ret != 0)
            {
                XLOGE("rtk8305 PHY_REG_AN_ADVERTISEMENT phy %d ret %x\n", Port[i], ret);
                continue;
            }
            XLOGD("rtk8305 PHY_REG_AN_ADVERTISEMENT after phy %d %#x\n", Port[i], phyData);
        }

        mInitDone = XBH_TRUE;
    }

    for(i = 0; i < m_MaxPort; i ++)
    {
       if (Port[i] !=  RJ45_IN_PORT && Port[i] != RJ45_OUT_PORT)
        {
            continue;
        }
        ret = rtk_port_phyReg_get(Port[i], (rtk_port_phy_reg_t)PHY_REG_STATUS, &phyData);
        if(ret != 0)
        {
            XLOGD("rtk8305 phy %d ret %x\n", Port[i], ret);
            continue;
        }
        
        //XLOGD("rtk8305 phy %d data %x\n", Port[i], phyData);

        m_s32LinkStatus[Port[i]] = ((phyData >> 2) & 0x1) ? 1 : 0;
        if(m_s32LinkStatus[Port[i]] == m_s32LinkStatusLast[Port[i]])
        {
            //XLOGD("rtk8305 phy %d Link Status \n", Port[i]);
            continue;
        }

        XLOGD("rtk8305 phy %d link status : %d -> %d\n",Port[i],m_s32LinkStatusLast[Port[i]], m_s32LinkStatus[Port[i]]);
        m_s32LinkStatusLast[Port[i]] = m_s32LinkStatus[Port[i]];
        if(m_s32LinkStatus[Port[i]] == 1) //link down -> link up
        {
            XLOGD("rtk8305 NET IC LINK UP~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            m_bLinkUpStatus = XBH_TRUE;
            if(Port[i] == RJ45_IN_PORT)
            {
                m_bRJ45INPortStatus = XBH_TRUE;
                XbhService::getModuleInterface()->setGpioOutputValue(ETHERNET_LED_G1, ETHERNET_LED_G1_ON);
            }
            else if(Port[i] == RJ45_OUT_PORT)
            {
                XbhService::getModuleInterface()->setGpioOutputValue(ETHERNET_LED_G2, ETHERNET_LED_G2_ON);
            }
        }
        else //link up -> link down
        {
            XLOGD("rtk8305 NET IC LINK DOWN~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            m_bLinkUpStatus = XBH_FALSE;
           
            if(Port[i] == RJ45_IN_PORT)
            {
                m_bRJ45INPortStatus = XBH_FALSE;
                XbhService::getModuleInterface()->setGpioOutputValue(ETHERNET_LED_G1, !ETHERNET_LED_G1_ON);
            }
            else if(Port[i] == RJ45_OUT_PORT)
            {
                XbhService::getModuleInterface()->setGpioOutputValue(ETHERNET_LED_G2, !ETHERNET_LED_G2_ON);
            }
        }
        if(Port[i] == RJ45_IN_PORT || Port[i] == RJ45_OUT_PORT)
        {
            XbhMutex::Autolock _l(mLock);
            #if 0
            rtk_port_phyReg_get(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
            phyData |= (0x1<<11);//power down
            rtk_port_phyReg_set(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update Android port status
            rtk_port_phyReg_set(TYPEC_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update TypeC port status
            rtk_port_phyReg_set(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update OPS port status
            usleep(500*1000);
            phyData |= (0x1<<15);//reset
            phyData &= ~(0x1<<11);//power on
            rtk_port_phyReg_set(ANDROID_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update Android port status
            rtk_port_phyReg_set(TYPEC_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update TypeC port status
            rtk_port_phyReg_set(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update OPS port status
            #endif
            if ((m_bAndroidPortStatus && m_s32LinkStatus[6] == 0 && m_s32LinkStatus[7] == 0) || (m_bAndroidPortStatus && m_s32LinkStatus[6] == 0 && m_s32LinkStatus[7] == 1))
            {
                setAndroidNetStatus(0);
                m_bAndroidPortStatus=1;
                XLOGD("rtk8305 android down ~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            }
            if (m_bOpsPortStatus)
            {
                setOpsNetStatus(0);
                m_bOpsPortStatus=1;
            }
            if (m_bTypecPortStatus)
            {
                setTypecNetStatus(0);
                m_bTypecPortStatus=1;
            }
            usleep(500*1000);
            //if (m_bAndroidPortStatus)
            if ((m_bLinkUpStatus && m_bAndroidPortStatus && m_s32LinkStatus[6] == 0 && m_s32LinkStatus[7] != 0) || (m_bLinkUpStatus && m_bAndroidPortStatus && m_s32LinkStatus[6] != 0 && m_s32LinkStatus[7] == 0) || (m_bRJ45INPortStatus && m_bAndroidPortStatus && m_s32LinkStatus[7] == 1))
            {
                setAndroidNetStatus(1);
                m_bRJ45INPortStatus = XBH_FALSE;
                XLOGD("rtk8305 android up ~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
            }
            if (m_bOpsPortStatus)
            {
                usleep(50*1000);
                setOpsNetStatus(1);
            }
            if (m_bTypecPortStatus)
            {
                usleep(50*1000);
                setTypecNetStatus(1);
            }
        }
    }
    usleep(2000 * 1000);
}

void ChipRtl8305::reset()
{
    XBH_U32 phyData = 0;
    rtk_port_phyReg_get(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
    phyData |= (0x1<<11);//power down
    rtk_port_phyReg_set(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update port status
    usleep(500*1000);
    phyData |= (0x1<<15);//reset
    phyData &= ~(0x1<<11);//power on
    rtk_port_phyReg_set(OPS_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update port status

    phyData = 0;
    rtk_port_phyReg_get(RJ45_IN_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
    phyData |= (0x1<<11);//power down
    rtk_port_phyReg_set(RJ45_IN_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update port status
    usleep(500*1000);
    phyData |= (0x1<<15);//reset
    phyData &= ~(0x1<<11);//power on
    rtk_port_phyReg_set(RJ45_IN_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update port status

    phyData = 0;
    rtk_port_phyReg_get(RJ45_OUT_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, &phyData);
    phyData |= (0x1<<11);//power down
    rtk_port_phyReg_set(RJ45_OUT_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update port status
    usleep(500*1000);
    phyData |= (0x1<<15);//reset
    phyData &= ~(0x1<<11);//power on
    rtk_port_phyReg_set(RJ45_OUT_PORT, (rtk_port_phy_reg_t)PHY_REG_CONTROL, phyData);   //update port status
}

ChipRtl8305::ChipRtl8305(XBH_S32 smi_SCK, XBH_S32 smi_SDA, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 androidPort, XBH_S32 typecPort, XBH_S32 opsPort, XBH_S32 rj45inPort, XBH_S32 rj45outPort)
{
    XLOGD("  begin ");
    memset(m_s32LinkStatus, 0, sizeof(m_s32LinkStatus));
    memset(m_s32LinkStatusLast, 0, sizeof(m_s32LinkStatusLast));
    m_bOpsPortStatus = XBH_TRUE;
    m_bTypecPortStatus = XBH_TRUE;
    m_bAndroidPortStatus = XBH_TRUE;
    m_bResetDone = XBH_FALSE;
    mInitDone = XBH_FALSE;
    m_bLinkUpStatus = XBH_TRUE;
    m_bRJ45INPortStatus = XBH_FALSE;

    mPowerGpio = pGpio;
    mPlevel = pLevel;
    mRstGpio = rGpio;
    mRlevel= rLevel;
    mdioGpio = smi_SDA;
    mdcGpio = smi_SCK;
    ANDROID_PORT = androidPort;
    TYPEC_PORT = typecPort;
    OPS_PORT = opsPort;
    RJ45_IN_PORT = rj45inPort;
    RJ45_OUT_PORT= rj45outPort;

    if(mPowerGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mPowerGpio, mPlevel);
        usleep(100*1000);
    }
    if(mRstGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mRstGpio, mRlevel);
        usleep(100*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRstGpio, !mRlevel);
        usleep(20*1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRstGpio, mRlevel);
        usleep(100*1000);
    }

    XbhService::getModuleInterface()->setGpioOutputValue(ETHERNET_LED_G1, !ETHERNET_LED_G1_ON);
    XbhService::getModuleInterface()->setGpioOutputValue(ETHERNET_LED_G2, !ETHERNET_LED_G2_ON);
    XbhMWThread::run(XbhMWThread::REPEAT);
    XLOGD("  end ");
}

ChipRtl8305::~ChipRtl8305()
{

}