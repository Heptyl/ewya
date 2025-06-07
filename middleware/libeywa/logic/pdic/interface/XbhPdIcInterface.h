#ifndef __XBH_PD_IC_INTERFACE_H__
#define __XBH_PD_IC_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhPdIcInterface
{
public:
    /**
     * 复位当前转换芯片
     */
    virtual XBH_S32 resetChip();

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
     设置typec功率
     */
    virtual XBH_S32 setTypecCapacity(XBH_U8 value);
public:
    XbhPdIcInterface(){};
    virtual ~XbhPdIcInterface(){};
};

#endif