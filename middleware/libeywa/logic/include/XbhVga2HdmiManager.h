#ifndef __XBH_VGA_2_HDMI_MANAGER_H__
#define __XBH_VGA_2_HDMI_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhVga2HdmiTask.h"

class XbhVga2HdmiManager
{
public:
    static XbhVga2HdmiManager* getInstance();
    XBH_S32 setCurrentRxHpd(XBH_U8 value);
    XBH_S32 resetChip();
    XBH_S32 getCurVgaStatus(XBH_SOURCE_E src, XBH_U8 *u8State);
    XBH_S32 getChipExist(XBH_SOURCE_E src, XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_SOURCE_E src, XBH_BOOL* enable);

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


    ~XbhVga2HdmiManager();

private:
    XbhVga2HdmiManager();
    static XbhVga2HdmiManager *mInstance;
    static XbhMutex mLock;

    XbhVga2HdmiTask *m_pXbhVga2HdmiTask;
};

#endif //__XBH_VGA_2_HDMI_MANAGER_H__
