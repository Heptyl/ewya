#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@I2CSimulator"

#include "XbhLog.h"
#include "XbhI2cSimulator.h"

I2CSimulator*           I2CSimulator::mInstance = NULL;
XbhMutex                I2CSimulator::mLock;

void I2CSimulator::IIC_Start()
{
    XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, 1);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, 0);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
    usleep(5);
}

void I2CSimulator::IIC_Stop()
{
    XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, 0);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, 1);
    usleep(5);
}

XBH_U8 I2CSimulator::IIC_WaitAck()
{
    XBH_U8 ucErrTime = 0;
    XBH_U32 temp = 0x00;
    XbhService::getModuleInterface()->getGpioInputValue(m_sdaGpioPin, &temp);      //SDA设置为输入
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
    usleep(100);

    do {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
        XbhService::getModuleInterface()->getGpioInputValue(m_sdaGpioPin, &temp);
    }while (temp);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);//时钟输出0
    return 0;
}

void I2CSimulator::IIC_Ack()
{
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, 0);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
}

void I2CSimulator::IIC_NAck()
{
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, 1);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
    usleep(5);
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
}

void I2CSimulator::IIC_SendByte(XBH_U8 _ucByte)
{
    XBH_U8 t;
    XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);//拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(m_sdaGpioPin, (_ucByte & 0x80) >> 7);//提取当前字节的最高位
        _ucByte <<= 1;
        usleep(5);   //对TEA5767这三个延时都是必须的
        XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
        usleep(5);
        XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
        usleep(5);
    }
}

XBH_U8 I2CSimulator::IIC_ReadByte(XBH_BOOL ack)
{
    XBH_U8 i;
    XBH_U8 value = 0x00;
    XBH_U32 temp = 0;
    XbhService::getModuleInterface()->getGpioInputValue(m_sdaGpioPin, &temp);
    usleep(5);

    for(i = 0; i < 8; i++ )
    {
        XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 0);
        usleep(5);
        XbhService::getModuleInterface()->setGpioOutputValue(m_sclGpioPin, 1);
        value<<=1;
        XbhService::getModuleInterface()->getGpioInputValue(m_sdaGpioPin, &temp);
        if(temp)
        {
            value++;
        }
        usleep(5);
    }
    if(!ack)
    {
        IIC_NAck();
    }
    else
    {
        IIC_Ack();
    }

    return value;
}

XBH_U8 I2CSimulator::IIC_Read_Data(XBH_U8 reg, XBH_U8 *pData, XBH_U8 Len)
{
    XBH_U8 i;
    IIC_Start();

    IIC_SendByte(0xA0);
    if (IIC_WaitAck()){
        XLOGE("Device no ack (device w)!!!");
        return -1;
    }

    IIC_SendByte(reg);
    if (IIC_WaitAck()){
        XLOGE("Device no ack (regist)!!!");
        return -1;
    }

    IIC_Start();
    IIC_SendByte(0xA1);
    if (IIC_WaitAck()){
        XLOGE("Device no ack (device r)!!!");
        return -1;
    }

    for(i = 0; i < Len; i++){
        pData[i] = IIC_ReadByte(i != (Len - 1));
    }

    IIC_Stop();
    return 0;
}

XBH_U8 I2CSimulator::IIC_Write_Data(XBH_U8 reg, const XBH_U8 *pData, XBH_U8 Len)
{
    XBH_U32 i = 0;
    IIC_Start();
    IIC_SendByte(0xA0);
    if (IIC_WaitAck()){
        XLOGE("Device no ack (device w)!!!");
        IIC_Stop();
        return -1;
    }
    IIC_SendByte(reg);
    if (IIC_WaitAck()){
        XLOGE("Device no ack (regist)!!!");
        IIC_Stop();
        return -1;
    }
    for(i = 0; i < Len; i++)
    {
        IIC_SendByte(pData[i]);
        if (IIC_WaitAck()){
            XLOGE("Device no ack (regist)!!!");
            IIC_Stop();
            return -1;
        }
    }
    IIC_Stop();
    usleep(1000);
    return 0;
}

I2CSimulator::I2CSimulator(XBH_U32 sdaGpioPin, XBH_U32 sclGpioPin)
{
    this->m_sdaGpioPin = sdaGpioPin;
    this->m_sclGpioPin = sclGpioPin;
}

I2CSimulator::~I2CSimulator()
{
}

I2CSimulator *I2CSimulator::getInstance(XBH_U32 sdaGpioPin, XBH_U32 sclGpioPin)
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new I2CSimulator(sdaGpioPin, sclGpioPin);
        }
    }
    return mInstance;
}
