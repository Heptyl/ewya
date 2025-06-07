#ifndef __XBH_VGA_2_HDMI_TASK_H__
#define __XBH_VGA_2_HDMI_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"

#include "XbhVga2HdmiInterface.h"

class XbhVga2HdmiTask : public XbhMWThread
{
public:
    static XbhVga2HdmiTask* getInstance();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip(XBH_SOURCE_E src);
    /**
     * VGA自动校准
     */
    XBH_S32 vgaAutoAdjust(XBH_SOURCE_E src);
    /**
     * VGA设置水平位置
     * u32Value 水平位置参数
     */
    XBH_S32 setVgaHPosition(XBH_SOURCE_E src, XBH_U32 u32Value);
    /**
     * VGA获取水平位置
     * u32Value 水平位置参数
     */
    XBH_S32 getVgaHPosition(XBH_SOURCE_E src, XBH_U32 *u32Value);
    /**
     * VGA设置垂直位置
     * u32Value 垂直位置参数
     */
    XBH_S32 setVgaVPosition(XBH_SOURCE_E src, XBH_U32 u32Value);
    /**
     * VGA获取垂直位置
     * u32Value 垂直位置参数
    */
    XBH_S32 getVgaVPosition(XBH_SOURCE_E src, XBH_U32 *u32Value);
    /**
     * VGA设置clock
     * u32Value clock参数
     */
    XBH_S32 setVgaClock(XBH_SOURCE_E src, XBH_U32 u32Value);
    /**
     * VGA获取clock
     * u32Value clock参数
    */
    XBH_S32 getVgaClock(XBH_SOURCE_E src, XBH_U32 *u32Value);
    /**
     * VGA设置phase
     * u32Value phase参数
     */
    XBH_S32 setVgaPhase(XBH_SOURCE_E src, XBH_U32 u32Value);
    /**
     * VGA获取phase
     * u32Value phase参数
    */
    XBH_S32 getVgaPhase(XBH_SOURCE_E src, XBH_U32 *u32Value);
    /**
     * 获取固件版本
     */
    XBH_S32 getFirmwareVersion(XBH_SOURCE_E src, XBH_CHAR* strVersion);
    /**
     * 进行固件升级
     */
    XBH_S32 upgradeFirmware(XBH_SOURCE_E src, const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade);
    /**
     * 获取固件升级时的状态
     */
    XBH_S32 getUpgradeState(XBH_SOURCE_E src, XBH_S32 * s32State);
    /**
     * 获取VGA工作的状态
     */
    XBH_S32 getCurVgaStatus(XBH_SOURCE_E src, XBH_U8 *u8State);

    /**
     * 获取VGA转换芯片是否存在
     */
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* bEnable);
    /**
     * 获取VGA转换芯片是否初始化成功
     */
    XBH_S32 getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable);

    ~XbhVga2HdmiTask();

private:
    void run(const void* arg);
    XbhVga2HdmiTask();
    XbhVga2HdmiInterface *getCurrIC(XBH_SOURCE_E src);
    static XbhVga2HdmiTask *mInstance;
    static XbhMutex mLock;

    XbhVga2HdmiInterface *m_pXbhVga2HdmiInterface[2];
    XBH_SOURCE_E m_pXbhVgaPortTable[2];

    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif
