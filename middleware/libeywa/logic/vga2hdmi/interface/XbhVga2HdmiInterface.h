#ifndef XBH_VGA_2_HDMI_INTERFACE_H
#define XBH_VGA_2_HDMI_INTERFACE_H

#include "XbhType.h"

class XbhVga2HdmiInterface 
{
public:
    /**
     * 复位当前转换芯片
     */
    virtual XBH_S32 resetChip();
    /**
     * VGA自动校准
     */
    virtual XBH_S32 vgaAutoAdjust();
    /**
     * VGA设置水平位置
     * u32Value 水平位置参数
     */
    virtual XBH_S32 setVgaHPosition(XBH_U32 u32Value);
    /**
     * VGA获取水平位置
     * u32Value 水平位置参数
     */
    virtual XBH_S32 getVgaHPosition(XBH_U32 *u32Value);
    /**
     * VGA设置垂直位置
     * u32Value 垂直位置参数
     */
    virtual XBH_S32 setVgaVPosition(XBH_U32 u32Value);
    /**
     * VGA获取垂直位置
     * u32Value 垂直位置参数
    */
    virtual XBH_S32 getVgaVPosition(XBH_U32 *u32Value);
    /**
     * VGA设置clock
     * u32Value clock参数
     */
    virtual XBH_S32 setVgaClock(XBH_U32 u32Value);
    /**
     * VGA获取clock
     * u32Value clock参数
    */
    virtual XBH_S32 getVgaClock(XBH_U32 *u32Value);
    /**
     * VGA设置phase
     * u32Value phase参数
     */
    virtual XBH_S32 setVgaPhase(XBH_U32 u32Value);
    /**
     * VGA获取phase
     * u32Value phase参数
    */
    virtual XBH_S32 getVgaPhase(XBH_U32 *u32Value);
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
     * 获取VGA工作的状态
     */
    virtual XBH_S32 getCurVgaStatus(XBH_U8 *u8State);
    /**
     * 获取VGA转换芯片是否存在
     */
    virtual XBH_S32 getChipExist(XBH_BOOL* bEnable);
    /**
     * 获取VGA转换芯片是否初始化成功
     */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);

public:
    XbhVga2HdmiInterface(){};
    virtual ~XbhVga2HdmiInterface(){};
};

#endif //XBH_VGA_2_HDMI_INTERFACE_H
