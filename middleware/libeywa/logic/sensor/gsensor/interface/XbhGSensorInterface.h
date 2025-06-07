#ifndef __XBH_GSENSOR_INTERFACE_H__
#define __XBH_GSENSOR_INTERFACE_H__

#include "XbhType.h"
#include "XbhCommon.h"

class XbhGSensorInterface
{
public:
    virtual XBH_S32 Get_GSensor_XData();
    virtual XBH_S32 Get_GSensor_YData();
    virtual XBH_S32 Get_GSensor_ZData();
    virtual XBH_S32 getChipInitDone(XBH_BOOL* bEnable);
    virtual XBH_S32 getChipExist(XBH_BOOL* bEnable);

public:
    XbhGSensorInterface(){};
    virtual ~XbhGSensorInterface(){};
};

#endif
