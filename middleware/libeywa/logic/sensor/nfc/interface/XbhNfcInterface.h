#ifndef __XBH_NFC_INTERFACE_H__
#define __XBH_NFC_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhNfcInterface
{
public:
    /**
     * 复位芯片
    */
    virtual XBH_S32 resetChip();
    /**
     * 获取chip是否初始化完成
    */
    virtual XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    virtual XBH_S32 getChipExist(XBH_BOOL* enable);
    /**
     * 使能nfc
    */
    virtual XBH_S32 setNfcEnable(XBH_BOOL enable);
    /**
     * 得到NFC状态
    */
    virtual XBH_S32 getNfcEnable(XBH_BOOL *enable);
    /**
     * 获取Nfc 卡ID
    */
    virtual XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    /**
     * 添加NFC 卡
    */
    virtual XBH_S32 addNfcId(XBH_U8 *data, XBH_U32 length);
    /**
     * 删除NFC 卡
    */
    virtual XBH_S32 delNfcId(XBH_U8 *data, XBH_U32 length);
    /**
     * 删除所有NFC卡
    */
    virtual XBH_S32 delAllNfcId();
    /**
     * 选择NFC模块
    */
    virtual XBH_S32 switchNfcType();
    /**
     * 设置NFC低功耗模式
    */
    virtual XBH_S32 setNfcLowPower(const XBH_U8* u8Data);
    /**
     * 获取NFC低功耗模式
    */
    virtual XBH_S32 getNfcLowPower(XBH_BOOL *enable);
    /**
     * 获取NFC唤醒的卡ID
    */
    virtual XBH_S32 getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length);
public:
    XbhNfcInterface(){};
    virtual ~XbhNfcInterface(){};
};

#endif
