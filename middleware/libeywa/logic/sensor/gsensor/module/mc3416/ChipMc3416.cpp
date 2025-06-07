#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipMc3416"
#include "XbhLog.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "XbhService.h"
#include "ChipMc3416.h"

XbhMutex                        ChipMc3416::mLock;

//---------------- custom device interface start ----------------

#define MC3416_VAL_LEN           3
#define MC3416_VAL_X             0
#define MC3416_VAL_Y             1
#define MC3416_VAL_Z             2


/*
****************************************************************
* MC3416_Write
****************************************************************
*/
XBH_BOOL ChipMc3416::MC3416_Write(XBH_U8 addr, XBH_S32 len, XBH_U8 *buf)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    //XLOGD("[MC3416] %s - %d : 0x%x -> 0x%x, len = %d\n",__func__,__LINE__,buf[0],addr,len);

    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus,mI2cAddr, addr, 1, len, buf);

    return s32Ret;
}

/*
****************************************************************
* MC3416_Read
****************************************************************
*/
XBH_BOOL ChipMc3416::MC3416_Read(XBH_U8 addr, XBH_S32 len, XBH_U8 *buf)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, addr, 1, len, buf);

    //XLOGD("[MC3416] %s - %d : 0x%x = 0x%x, len = %d\n",__func__,__LINE__,addr,buf[0], len);

    return s32Ret;
}

/*
**********************************************************************
* MC3416_Wakeup
**********************************************************************
*/
void ChipMc3416::MC3416_Wakeup()
{
    XBH_U8 data = 0x41; //wake up
    XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, MC3416_REG_MODE, 1, 1, &data);
}

/**********************************************************************
* MC3416_Sleep
**********************************************************************
*/
void ChipMc3416::MC3416_Sleep()
{
    XBH_U8 data = 0x43; //sleep
    XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, MC3416_REG_MODE, 1, 1, &data);
}

XBH_S32 ChipMc3416::Get_GSensor_XData()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 data;

    int mc_x = 0;

    //Read X
    s32Ret |= MC3416_Read(MC3416_REG_XOUT_H, 1, &data);
    mc_x = data;
    s32Ret |= MC3416_Read(MC3416_REG_XOUT_L, 1, &data);
    mc_x = ((mc_x << 8) | data);
    //XLOGD("[MC3416] mc_z=%d,%f\n",mc_x,mc_x);
    return mc_x;
}

XBH_S32 ChipMc3416::Get_GSensor_YData()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 data;

    int mc_y = 0;
    
    //Read Y
    s32Ret |= MC3416_Read(MC3416_REG_YOUT_H, 1, &data);
    mc_y = data;
    s32Ret |= MC3416_Read(MC3416_REG_YOUT_L, 1, &data);
    mc_y = ((mc_y << 8) | data);
    //XLOGD("[MC3416] mc_z=%d,%f\n",mc_y,mc_y);
    return mc_y;
}

XBH_S32 ChipMc3416::Get_GSensor_ZData()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 data;

    int mc_z = 0;
    
    //Read Z
    s32Ret |= MC3416_Read(MC3416_REG_ZOUT_H, 1, &data);
    mc_z = data;
    s32Ret |= MC3416_Read(MC3416_REG_ZOUT_L, 1, &data);
    mc_z = ((mc_z << 8) | data);
    //XLOGD("[MC3416] mc_z=%d,%f\n",mc_z,mc_z);
    return mc_z;
}

/**********************************************************************
* init()
**********************************************************************
*/
void ChipMc3416::init()
{
    XLOGD("[MC3416] %s - %d\n",__func__,__LINE__);
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;

    XBH_U8 data = 0x03; //standby up
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, MC3416_REG_MODE, 1, 1, &data);
    if (s32Ret < 0) {
        XLOGE("standby up failed!\n");
        return;
    }

    data = 0x29; //range select
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, MC3416_REG_RANGE_SELECT, 1, 1, &data);
    if (s32Ret < 0) {
        XLOGE("range select failed!\n");
        return;
    }

    data = 0x05; //sample rate
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, MC3416_REG_SAMPLE_RATE, 1, 1, &data);
    if (s32Ret < 0) {
        XLOGE("sample rate failed!\n");
        return;
    }

    data = 0x41; //bit7
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, MC3416_REG_MODE, 1, 1, &data);
    if (s32Ret < 0) {
        XLOGE("set bit7 failed!\n");
        return;
    }
    //i2c_write_byte(0x40,0x50);    //Tilt/Flip Threshold LSB
    //i2c_write_byte(0x41,0x00);    //Tilt/Flip Threshold MSB
    //i2c_write_byte(0x42,0x01);    //Tilt/Flip Debounce

    //i2c_write_byte(0x43,0x10);    //AnyMotion Threshold LSB
    //i2c_write_byte(0x44,0x00);    //AnyMotion Threshold MSB

    //i2c_write_byte(0x45,0x01);    //AnyMotion Debounce
    //i2c_write_byte(0x46,0x88);    //Shake Threshold LSB
    //i2c_write_byte(0x47,0x00);    //Shake Threshold MSB


    //i2c_write_byte(0x48,0x10);    //Peak-to-Peak Duration LSB 
    //i2c_write_byte(0x49,0x10);    //Shake Duration and Peak-to-Peak Duration MSB

    //i2c_write_byte(0x06,0x04);    //_M_DRV_MC36XX_SetSniffAGAIN
    //i2c_write_byte(0x09,0x04);    //Motion Control

    m_bInitDone = XBH_TRUE;
}

/**
 * 获取Gsensor是否存在
 */
XBH_S32 ChipMc3416::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        *bEnable = XBH_FALSE;
        return s32Ret;
    }
    *bEnable = XBH_TRUE;
    return  XBH_SUCCESS;
}

XBH_S32 ChipMc3416::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

ChipMc3416::ChipMc3416(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    init();
    XLOGD(" end ");
}

ChipMc3416::~ChipMc3416()
{

}
