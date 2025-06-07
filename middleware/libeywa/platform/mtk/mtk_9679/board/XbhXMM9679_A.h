#ifndef XBH_XMM9679_A_H
#define XBH_XMM9679_A_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhMtk_9679.h"

class XbhXMM9679_A : public XbhMtk_9679
{
public:
    static XbhXMM9679_A* getInstance();
    //override
    XBH_S32 getHdmi1Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi2Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi3Det(XBH_BOOL *enable);
    //override
    XBH_S32 getHdmi4Det(XBH_BOOL *enable);

private:
    XbhXMM9679_A();
    ~XbhXMM9679_A();

private:
    static XbhXMM9679_A *mInstance;
    static XbhMutex mLock;
};

#endif //XBH_XMM9679_A_H
