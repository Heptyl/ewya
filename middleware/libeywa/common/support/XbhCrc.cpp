#include "XbhCrc.h"

const static XBH_U16 CRC_TABLE[16] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
};

XBH_U16 XbhCrc::crc16(XBH_U8 *pBuffer, XBH_U32 ucLength)
{
    XBH_U16 u16CRC= 0xFFFF;
    XBH_U8 ucTemp;

    while (ucLength--)
    {
        ucTemp = (XBH_U8)(u16CRC >> 0x0C);
        u16CRC <<= 4;
        u16CRC ^= CRC_TABLE[ucTemp ^ ((*pBuffer) >> 0x04)];
        ucTemp = (XBH_U8)(u16CRC >> 0x0C);
        u16CRC <<= 4;
        u16CRC ^= CRC_TABLE[ucTemp ^ ((*pBuffer) & 0x0F)];
        pBuffer++;
    }
    return u16CRC;
}