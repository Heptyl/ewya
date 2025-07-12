#ifndef XBH_CRC_H
#define XBH_CRC_H
#include "XbhType.h"

class XbhCrc
{
public:
    static XBH_U16 crc16(XBH_U8 *pBuffer, XBH_U32 ucLength);
    // 使用多项式0x8005的CRC校验，量产过的方案别用，只用于H610
    // Calculate the crc16 value with polynomial 0x8005
    static XBH_U16 calCrc16By8005(XBH_U8 *pBuffer, XBH_U32 ucLength);
};


#endif //XBH_CRC_H
