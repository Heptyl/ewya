#ifndef XBH_CRC_H
#define XBH_CRC_H
#include "XbhType.h"

class XbhCrc
{
public:
    static XBH_U16 crc16(XBH_U8 *pBuffer, XBH_U32 ucLength);
};



#endif //XBH_CRC_H