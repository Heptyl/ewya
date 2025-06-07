#ifndef __XBH_DP_2_HDMI_INTERFACE_H__
#define __XBH_DP_2_HDMI_INTERFACE_H__

#include "XbhType.h"

class XbhDp2HdmiInterface 
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
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);
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

public:
    XbhDp2HdmiInterface(){};
    virtual ~XbhDp2HdmiInterface(){};
};

#endif