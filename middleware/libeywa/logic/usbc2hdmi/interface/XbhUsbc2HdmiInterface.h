#ifndef __XBH_USBC_2_HDMI_INTERFACE_H__
#define __XBH_USBC_2_HDMI_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhUsbc2HdmiInterface 
{
public:
    /**
     * 复位当前转换芯片
     */
    virtual XBH_S32 resetChip();

    /**
     * 设置当前信源的HPD状态
     * value 高低状态
     */
    virtual XBH_S32 setRxHpd(XBH_U8 value);

    /**
     * 获取固件版本
     */
    virtual XBH_S32 getFirmwareVersion(XBH_CHAR* strVersion);

    /**
     * 进行固件升级
     */
    virtual XBH_S32 upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);

    /**
     * 获取固件升级时的状态
     */
    virtual XBH_S32 getUpgradeState(XBH_S32 * s32State);

    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);

    /**
     * 限制type-c功率达到OPS过流保护效果
     */
    virtual XBH_S32 setTypecCapacity(XBH_U32 value);

    /**
     * 设置SWITCH进入一些特定的模式，有些switch在跟SOC断开后就无法正确处理信号的插拔
     * 需要对这些switch进行特殊处理
     */
    virtual XBH_S32 setSpecificMode();

    /**
     * 根据OPS插入状态，动态设置PD功率
     */
    virtual XBH_S32 setChargingPower(XBH_PDCAPACITY_POWER data);

public:
    XbhUsbc2HdmiInterface(){};
    virtual ~XbhUsbc2HdmiInterface(){};
};

#endif