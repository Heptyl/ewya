#ifndef __XBH_MCU_MANAGER_H__
#define __XBH_MCU_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhMcuTask.h"

class XbhMcuManager
{
public:
    static XbhMcuManager* getInstance();
    /**
     * 复位MCU芯片
    */
    XBH_S32 resetChip();
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
    * 设置fatt模式
    */
    XBH_S32 setMcuFattMode(XBH_S32 mode);

    /**
    * 获取fatt模式
    */
    XBH_S32 getMcuFattMode(XBH_S32 *mode);

    /**
    * mcu 外置iic数据转发
    */
    XBH_S32 setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);

    /**
    * mcu 外置iic数据获取
    */
    XBH_S32 getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data);
    /**
    * mcu 外置uart配置
    */
    XBH_S32 setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud);
    /**
    * mcu 外置uart数据转发
    */
    XBH_S32 setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);
    /**
    * mcu 外置uart数据接收
    */
    XBH_S32 getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data);

    ~XbhMcuManager();

private:
    XbhMcuManager();
    static XbhMcuManager *mInstance;
    static XbhMutex mLock;

    XbhMcuTask *m_pXbhMcuTask;
};

#endif
