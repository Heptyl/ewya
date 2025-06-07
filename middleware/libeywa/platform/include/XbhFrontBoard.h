#ifndef XBH_FRONT_BOARD_H
#define XBH_FRONT_BOARD_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

class XbhFrontBoard
{
public:
    static XbhFrontBoard* getInstance();
    XBH_S32 getFrontBoardType(XBH_U8 *type);
    XBH_S32 getHdmiSwitchTable(XBH_U8 *table);
    XBH_S32 getHdmiSwitchI2cTable(HDMI_SW_OPT_TABLE *table);
    XBH_S32 getHdmiSwitchPortTable(HDMI_SWITCH_PORT *table);

    XBH_S32 getUsbc2HdmiTable(XBH_U8 table[]);
    XBH_S32 getUsbc2HdmiI2cTable(COMMON_OPT_TABLE table[]);
    XBH_S32 getUsbc2HdmiPortTable(XBH_SOURCE_E port[]);
    XBH_S32 getDp2HdmiTable(XBH_U8 table[]);
    XBH_S32 getDp2HdmiI2cTable(COMMON_OPT_TABLE table[]);
    XBH_S32 getDp2HdmiPortTable(XBH_SOURCE_E port[]);
    XBH_S32 getPdIcTable(XBH_U8 table[]);
    XBH_S32 getPdIcI2cTable(COMMON_OPT_TABLE table[]);

    XBH_S32 followToAndroidExtBefore(XBH_VOID);
    XBH_S32 followToAndroidExtAfter(XBH_VOID);
    XBH_S32 followToSrcExtBefore(XBH_VOID);
    XBH_S32 followToSrcExtAfter(XBH_VOID);
    XBH_S32 followToFUsbc1ExtBefore(XBH_VOID);
    XBH_S32 followToFUsbc1ExtAfter(XBH_VOID);
    XBH_S32 followToFUsbc2ExtBefore(XBH_VOID);
    XBH_S32 followToFUsbc2ExtAfter(XBH_VOID);
    XBH_S32 followToFHdmi1ExtBefore(XBH_VOID);
    XBH_S32 followToFHdmi1ExtAfter(XBH_VOID);
    XBH_S32 followToFHdmi2ExtBefore(XBH_VOID);
    XBH_S32 followToFHdmi2ExtAfter(XBH_VOID);
    ~XbhFrontBoard();

private:
    XBH_S32 init();
    XbhFrontBoard();
    static XbhFrontBoard *mInstance;
    static XbhMutex mLock;
    XBH_U32 mFboardType;
    //switch
    XBH_U8 mSwitchType;
    HDMI_SW_OPT_TABLE mOptTable;
    HDMI_SWITCH_PORT mPortTable;
    //usbc
    XBH_U8 mUsbcType[2];
    COMMON_OPT_TABLE mUsbcOptTable[2];
    XBH_SOURCE_E mUsbcPort[2];
    //dp
    XBH_U8 mDpType[2];
    COMMON_OPT_TABLE mDpOptTable[2];
    XBH_SOURCE_E mDpPort[2];
    //pd
    XBH_U8 mPdType[2];
    COMMON_OPT_TABLE mPdOptTable[2];
};

#endif //XBH_FRONT_BOARD_H
