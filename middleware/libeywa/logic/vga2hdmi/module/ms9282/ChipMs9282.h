#ifndef XBH_CHIP_MS9282_H
#define XBH_CHIP_MS9282_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhVga2HdmiInterface.h"

class ChipMs9282 : public XbhMWThread, public XbhVga2HdmiInterface
{
public:
    /**
     * i2cBus       使用的IIC序号
     * Addr         芯片IIC地址
     * pGpio        供电引脚
     * pLevel       供电工作时的电平状态
     * rGpio        芯片复位引脚
     * rLevel       复位工作时的电平状态
    */
    ChipMs9282(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipMs9282();
    /**
     * 复位当前转换芯片
     */
    XBH_S32 resetChip();
    /**
     * VGA自动校准
     */
    XBH_S32 vgaAutoAdjust();
    /**
     * VGA设置水平位置
     * u32Value 水平位置参数
     */
    XBH_S32 setVgaHPosition(XBH_U32 u32Value);
    /**
     * VGA获取水平位置
     * u32Value 水平位置参数
     */
    XBH_S32 getVgaHPosition(XBH_U32 *u32Value);
    /**
     * VGA设置垂直位置
     * u32Value 垂直位置参数
     */
    XBH_S32 setVgaVPosition(XBH_U32 u32Value);
    /**
     * VGA获取垂直位置
     * u32Value 垂直位置参数
    */
    XBH_S32 getVgaVPosition(XBH_U32 *u32Value);
    /**
     * VGA设置clock
     * u32Value clock参数
     */
    XBH_S32 setVgaClock(XBH_U32 u32Value);
    /**
     * VGA获取clock
     * u32Value clock参数
    */
    XBH_S32 getVgaClock(XBH_U32 *u32Value);
    /**
     * VGA设置phase
     * u32Value phase参数
     */
    XBH_S32 setVgaPhase(XBH_U32 u32Value);
    /**
     * VGA获取phase
     * u32Value phase参数
    */
    XBH_S32 getVgaPhase(XBH_U32 *u32Value);
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
     * 获取VGA工作的状态
     */
    XBH_S32 getCurVgaStatus(XBH_U8 *u8State);
    /**
     * 获取VGA转换芯片是否存在
     */
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    /**
     * 获取VGA转换芯片是否初始化成功
     */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);

private:
    void run(const void* arg);
    void initVgaData();
    static XbhMutex mLock;

    XBH_BOOL mChipInit;
    XBH_BOOL mNeedInitData;

    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
};

#endif //XBH_CHIP_MS9282_H

