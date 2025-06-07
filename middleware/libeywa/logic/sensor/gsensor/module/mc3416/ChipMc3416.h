#ifndef XBH_MC3416_MANAGER_H
#define XBH_MC3416_MANAGER_H

#include "XbhType.h"
#include "XbhMWThread.h"
#include "XbhMutex.h"
#include "XbhCommon.h"

#include "XbhGSensorInterface.h"

//#define MC3416_I2C_SLAVE_ADDR         0x4C //7bit address
//#define MC3416_I2C_SLAVE_WRITE_ADDR   0x98 //8bit write address
//#define MC3416_I2C_SLAVE_READ_ADDR    0x99 //8bit read address

//#define MC3416_I2C_SLAVE_ADDR         0x6C //7bit address
//#define MC3416_I2C_SLAVE_WRITE_ADDR   0xD8 //8bit write address
//#define MC3416_I2C_SLAVE_READ_ADDR    0xD9 //8bit read address

#define MC3416_REG_MODE               0x07
#define MC3416_REG_RANGE_SELECT       0x20  //Bit6/5/4
                                            //000: 2g
                                            //001: 4g
                                            //010: 8g
                                            //011: 16g
                                            //100: 12g

#define MC3416_REG_SAMPLE_RATE        0x08  //Bit2/1/0
                                            //Output data rate, determined by the frequency of the main oscillator and the
                                            //000: 128 (default, slowest)
                                            //001: 256
                                            //010: 512
                                            //011: Reserved.
                                            //100: Reserved.
                                            //101: 1024 (fastest)
                                            //110: Reserved.
                                            //111: Reserved.

#define MC3416_REG_XOUT_L             0x0D
#define MC3416_REG_XOUT_H             0x0E
#define MC3416_REG_YOUT_L             0x0F
#define MC3416_REG_YOUT_H             0x10
#define MC3416_REG_ZOUT_L             0x11
#define MC3416_REG_ZOUT_H             0x12


class ChipMc3416: public XbhGSensorInterface
{
//---------------- custom device interface start ----------------
public:
	struct kernel_mc3416_value {
	    int mc_x;
	    int mc_y;
	    int mc_z;
	};
    void MC3416_Wakeup();
    void MC3416_Sleep();
    //获取X轴数据
	XBH_S32 Get_GSensor_XData();
    //获取Y轴数据
    XBH_S32 Get_GSensor_YData();
    //获取Z轴数据
    XBH_S32 Get_GSensor_ZData();
    //检测传感器是否存在
    XBH_S32 getChipExist(XBH_BOOL* bEnable);
    XBH_S32 getChipInitDone(XBH_BOOL* bEnable);
    ChipMc3416(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel);
    ~ChipMc3416();

private:
    void init();
    static XbhMutex mLock;
    XBH_S32 mI2cBus;
    XBH_S32 mI2cAddr;
    XBH_S32 mPGpio;
    XBH_S32 mPLevel;
    XBH_S32 mRGpio;
    XBH_S32 mRLevel;
	XBH_BOOL MC3416_Write(XBH_U8 addr, XBH_S32 len, XBH_U8 *buf);
    XBH_BOOL MC3416_Read(XBH_U8 addr, XBH_S32 len, XBH_U8 *buf);
    XBH_S32 m_bInitDone;
};
#endif //XBH_MC3416_TASK_H
