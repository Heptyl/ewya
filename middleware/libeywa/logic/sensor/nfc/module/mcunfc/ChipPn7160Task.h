#ifndef __HI_MW_CHIPPN7160Task_H__
#define __HI_MW_CHIPPN7160Task_H__

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhNfcInterface.h"

class ChipPn7160Task  : public XbhNfcInterface , public XbhMWThread
{
public:
    ChipPn7160Task(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    XBH_U32 resetflag;
    XBH_U8  modeflag;
    XBH_S32 Init(void);
    XBH_U8* GetID(void);
    XBH_U32 NFCSTATUS(void);
    XBH_S32 getNfcId(XBH_U8 *data, XBH_U32 *length);
    void XBH_T4T_NDEF_EMU_SetMessage(XBH_U8 NDEF_MESSAGE[],XBH_U32 length);
    void set_nfc_mode(XBH_U8 mode);
    XBH_U8* NDEF_MESSAGE_temp;
    static XBH_S32 mPGpio;
    static XBH_S32 mPLevel;
    static XBH_S32 mI2cBus;
    static XBH_S32 mI2cAddr;

private:
    ChipPn7160Task();
    ~ChipPn7160Task();
    static ChipPn7160Task *mInstance;
    static XbhMutex mLockObject;
    void run(const void* arg);
    static XbhMutex mLock;


    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
    
    XBH_BOOL m_bInitDone;
};

#endif

