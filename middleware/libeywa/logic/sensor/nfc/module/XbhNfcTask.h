#ifndef __XBH_NFC_TASK_H__
#define __XBH_NFC_TASK_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhMWThread.h"
#include "XbhCommon.h"
#include <hardware/board.h>
#include "XbhNfcInterface.h"

class XbhNfcTask : public XbhMWThread
{
public:
    static XbhNfcTask* getInstance();
    /**
     * 复位芯片
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
     * 使能nfc
    */
    XBH_S32 setNfcEnable(XBH_BOOL enable);
    /**
     * 得到NFC状态
    */
    XBH_S32 getNfcEnable(XBH_BOOL *enable);
    /**
     * 获取Nfc 卡ID
    */
    XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    /**
     * 添加NFC 卡
    */
    XBH_S32 addNfcId(XBH_U8 *data, XBH_U32 length);
    /**
     * 删除NFC 卡
    */
    XBH_S32 delNfcId(XBH_U8 *data, XBH_U32 length);
    /**
     * 删除所有NFC卡
    */
    XBH_S32 delAllNfcId();
    /**
     * 选择NFC模块
    */
    XBH_S32 switchNfcType();
    /**
     * 设置NFC低功耗模式
    */
    XBH_S32 setNfcLowPower(const XBH_U8* u8Data);
    /**
     * 获取NFC低功耗模式
    */
    XBH_S32 getNfcLowPower(XBH_BOOL *enable);
    /**
     * 获取NFC唤醒的卡ID
    */
    XBH_S32 getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length);

    ~XbhNfcTask();

private:
    void run(const void* arg);
    XbhNfcTask();
    static XbhNfcTask *mInstance;
    static XbhMutex mLock;

    XbhNfcInterface *m_pXbhNfcInterface[2];

    XBH_U8 mList1[2];
    COMMON_OPT_TABLE mList2[2];
};

#endif
