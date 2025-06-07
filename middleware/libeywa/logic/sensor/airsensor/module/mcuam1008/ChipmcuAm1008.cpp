#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipmcuAm1008"
#include "XbhLog.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include "cutils/properties.h"
#include "errno.h"

#include "XbhService.h"
#include "ChipmcuAm1008.h"

XbhMutex                        ChipmcuAm1008::mLock;

/*
 *  此功能实现是通过解析MCU上传的数据来完成的
 */

static XBH_S32 co2Value = -1;
static XBH_S32 vocValue = -1 ;
static XBH_S32 humidityValue = -1;
static XBH_S32 temperatureValue = -100;
static XBH_S32 pm25Value = -1;

void ChipmcuAm1008::run(const void* arg)
{
    while(1)
    {
        getAirSensorFormMcu();
        co2Value = sensorDate[3] * 256 + sensorDate[4]; //二氧化碳含量
        vocValue = sensorDate[5] * 256 + sensorDate[6];  //总挥发性有机化合物 TVOC
        humidityValue = (sensorDate[7] * 256 + sensorDate[8]) / 10;    //湿度
        temperatureValue = (sensorDate[9] * 256 + sensorDate[10] - 500) / 10;   //温度
        pm25Value = sensorDate[19] * 256 + sensorDate[20];  //直径小于2.5微米的颗粒物浓度 PM2.5
        XLOGD("get AIRSensor data : vocValue[%d] pm25Value[%d] temperatureValue[%d] humidityValue[%d] co2Value[%d]\n",vocValue,pm25Value,temperatureValue,humidityValue,co2Value);
        sleep(10); //每10s获取一次空气传感器数据
    }
}

/**
 * 通过MCU获取Airsensor值
 */
XBH_S32 ChipmcuAm1008::getAirSensorFormMcu()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("Func[%s] - Line[%d] \n\n", __func__, __LINE__);
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_GET_PM2_5;
    pI2cBuff.len = 22;  //空气传感器数据长度
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    memcpy(sensorDate, pI2cBuff.data, pI2cBuff.len);
    return s32Ret;
}

/**
 * 获取Airsensor是否存在
 */
XBH_S32 ChipmcuAm1008::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = XBH_TRUE;
    return  s32Ret;
}

XBH_S32 ChipmcuAm1008::getVocValue(){
    return vocValue;
}

XBH_S32 ChipmcuAm1008::getHumidityValue(){
    return humidityValue;
}

XBH_S32 ChipmcuAm1008::getTemperatureValue(){
    return temperatureValue;
}

XBH_S32 ChipmcuAm1008::getPm25Value(){
    return pm25Value;
}

ChipmcuAm1008::ChipmcuAm1008(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    XbhMWThread::run();
    XLOGD(" end ");
}

ChipmcuAm1008::~ChipmcuAm1008()
{
}
