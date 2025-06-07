#ifndef XBH_XMM9679_D_H
#define XBH_XMM9679_D_H

#include "XbhType.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhMtk_9679.h"
#include <hardware/board.h>
#include <unistd.h>
#include <cutils/properties.h>
#include "XbhDp2HdmiManager.h"
#include "XbhSourceManager.h"


class XbhXMM9679_D : public XbhMtk_9679
{
public:
    static XbhXMM9679_D* getInstance();

private:
    XbhXMM9679_D();
    ~XbhXMM9679_D();

private:
    static XbhXMM9679_D *mInstance;
    static XbhMutex mLock;
    XBH_U8 sensorDate[32];
    XBH_U8 PortHpdState[4];
    XBH_U8 SourceDetState[XBH_SOURCE_BUTT];
};

#endif //XBH_XMM9679_B_H
