#ifndef __XBH_NFC_MANAGER_H__
#define __XBH_NFC_MANAGER_H__

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"
#include "XbhNfcTask.h"

class XbhNfcManager
{
public:
    static XbhNfcManager* getInstance();

    XBH_S32 resetChip();
    /**
     * 获取chip是否初始化完成
    */
    XBH_S32 getChipInitDone(XBH_BOOL* enable);
    /**
     * 获取chip是否存在
    */
    XBH_S32 getChipExist(XBH_BOOL* enable);
    XBH_S32 setNfcEnable(XBH_BOOL enable);
    XBH_S32 getNfcEnable(XBH_BOOL *enable);
    XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    XBH_S32 addNfcId(XBH_U8 *data, XBH_U32 length);
    XBH_S32 delNfcId(XBH_U8 *data, XBH_U32 length);
    XBH_S32 delAllNfcId();
    XBH_S32 switchNfcType();
    XBH_S32 setNfcLowPower(const XBH_U8* u8Data);
    XBH_S32 getNfcLowPower(XBH_BOOL *enable);
    /**
     * 获取NFC唤醒的卡ID
    */
    XBH_S32 getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length);

    ~XbhNfcManager();

private:
    XbhNfcManager();
    static XbhNfcManager *mInstance;
    static XbhMutex mLock;

    XbhNfcTask *m_pXbhNfcTask;
};

#endif
