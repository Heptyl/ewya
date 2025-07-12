#ifndef XBH_I2C_SIMULATOR_H
#define XBH_I2C_SIMULATOR_H

#include "XbhType.h"
#include "XbhService.h"
#include "XbhMutex.h"

class I2CSimulator {
public:
    static I2CSimulator* getInstance(XBH_U32 sdaGpioPin, XBH_U32 sclGpioPin, XBH_U8 slaveAddr);
    XBH_U8 IIC_Read_Data(XBH_U8 reg, XBH_U8 *pData, XBH_U8 Len);
    XBH_U8 IIC_Write_Data(XBH_U8 reg, const XBH_U8 *pData, XBH_U8 Len);

private:
    I2CSimulator(XBH_U32 sdaGpioPin, XBH_U32 sclGpioPin, XBH_U8 slaveAddr);
    ~I2CSimulator();
    void IIC_Start();
    void IIC_Stop();
    XBH_U8 IIC_WaitAck();
    void IIC_Ack();
    void IIC_NAck();
    void IIC_SendByte(XBH_U8 _ucByte);
    XBH_U8 IIC_ReadByte(XBH_BOOL ack);

    XBH_U32 m_sdaGpioPin;
    XBH_U32 m_sclGpioPin;
    XBH_U8  m_slaveAddr;

    static I2CSimulator*   mInstance;
    static XbhMutex        mLock;
};

#endif // I2C_SIMULATOR_H