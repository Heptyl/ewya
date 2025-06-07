#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipAm1008"
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
#include "ChipAm1008.h"

XbhMutex                        ChipAm1008::mLock;

/*
 * 1） 本协议数据，均为16进制数据。如 “46”为十进制的[70]；
 * 2） [xx]为单字节无符号类型，双字节数据高字节在前，低字节在后；
 * 3） 波特率：9600, DataBits: 8, StopBits: 1, Parity: No。
 * 4） 此版本兼容主动上传和被动接收模式。开机默认会主动上传数据，当接收到任意一条查询指令，会切换到被动接收模式，一收一答。
 *
 * 传感器工作逻辑
 * 1、空品传感器上电。
 * 2、PM2.5、CO2 传感器都处于关闭状态，状态都为 1。
 * 3、传感器接收到开启测量指令后，相应传感器开启测量。状态位变为 3（检测中）。
 * 4、传感器接收到关闭测量指令后，相应传感器关闭测量。状态位变为 1（关闭）。
 * 5、开启测量后，各个传感器的数据是实时输出的,但是不建议读取检测中测量值（预热中测量值准确性不好，仅作参考）。
 *      a)PM2.5 传感器测量（预热 8S）：前 2S 输出上一次最后测量结果（若为模块第一次上电，输出 值为 0），2S 后实时输出粉尘
 *      浓度。
 *      b)CO2 传感器（预热 120S）：上电输出浓度为 550ppm，120S 后实时输出当前 CO2 浓度。
 *      c)温湿度传感器：开机 5S 内会自动检测，5S 后会输出稳定的温湿度值。
 *      d)VOC 传感器：预热120S 后输出当前VOC 浓度等级。
 * 6、预热检测完成。传感器状态变为完成检测（0XA5），此时主控板可以读取准确的测量值。
 * 7、预热检测完成后，传感器还是在一直工作，直到接收到停止测量指令。
 * 8、主控板读取完成测量值后，可以发送停止测量指令，关闭相应的传感器。

 */

#define PACKAGE_LEN 28

int DEBUG = 0;

char path[128];

int mFd = -1;
char detValue[PACKAGE_LEN];
static XBH_S32 co2Value = -1;
static XBH_S32 vocValue = -1 ;
static XBH_S32 humidityValue = -1;
static XBH_S32 temperatureValue = -100;
static XBH_S32 pm25Value = -1;

XBH_S32 g_timeInterval = 3;

XBH_U8 AM1008_CMD_GET_DET_VALUE[] = {0X11, 0X02, 0X01, 0X01, 0XEB};                     //获取测量结果
XBH_U8 AM1008_CMD_SET_VOC_DET_ENABLE[] = {0X11, 0X03, 0X0C, 0X02, 0X1E, 0XC0};          //开启VOC测量 传感器接受到开启测量指令后，默认进入连续测量状态。
XBH_U8 AM1008_CMD_SET_VOC_DET_DISABLE[] = {0X11, 0X03, 0X0C, 0X01, 0X1E, 0XC1};         //停止VOC测量
XBH_U8 AM1008_CMD_GET_PM25_DETECTION_TIME[] = {0X11, 0X01, 0X0D, 0XE1};                 //读取粉尘测量时间
XBH_U8 AM1008_CMD_GET_PM25_DETECTION_MODE[] = {0X11, 0X01, 0X05, 0XE9};                 //读取粉尘测量工作模式
XBH_U8 AM1008_CMD_GET_PM25_CALIBRATE_COEFFICENT[] = {0X11, 0X01, 0X07, 0XE7};           //读取模块校准系数
XBH_U8 AM1008_CMD_GET_FW_VERSION[] = {0X11, 0X01, 0X1E, 0XD0};                          //查询固件版本号


speed_t ChipAm1008::getBaudrate(XBH_S32 baudrate)
{
    switch(baudrate) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return B0;
    }
}

void ChipAm1008::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("openSerialPort...\n");
    s32Ret = openSerialPort(path, 9600);
    if(s32Ret == XBH_FALSE)
    {
        XLOGE("open chipAm1008 error !!!! \n");
    }
}

/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
XBH_BOOL ChipAm1008::openSerialPort(const char *path, XBH_S32 baudrate)
{
    struct termios newtio, oldtio;
    int ret = -1;
    if (mFd < 0)
    {
        mFd = open(path, O_RDWR|O_NOCTTY|O_NDELAY);
        if (mFd == -1)
        {
            XLOGE("Open /dev/ttyS Error.");
            return XBH_FALSE;
        }
        if (tcgetattr(mFd,&oldtio)) 
        {
            XLOGE("Get /dev/ttyS Error.");
            return XBH_FALSE;
        }
        // memcpy(&newtio, &oldtio, sizeof(newtio));
        cfmakeraw(&oldtio);
        cfsetispeed(&oldtio, B9600);
        cfsetospeed(&oldtio, B9600); 
        tcflush(mFd,TCIFLUSH);
        oldtio.c_cc[VMIN] =  PACKAGE_LEN;
        if ((tcsetattr(mFd,TCSANOW,&oldtio))!=0)
        {
            XLOGE("Set /dev/ttyS Error.");
            return XBH_FALSE;
        }
    }
    return XBH_TRUE;
}

XBH_S32 ChipAm1008::readCommand(XBH_U8* pbuf, XBH_S32 len)
{
    XBH_S32 ret = XBH_FAILURE;
    XLOGD("readCommand mFd = %d\n", mFd);
    if(mFd != XBH_FAILURE)
    {
        ret = read(mFd, pbuf, len);
        XLOGE("readCommand ret = %d errno = %s\n", ret, strerror(errno));
    }
    return ret;
}

void ChipAm1008::run(const void* arg)
{
    int ret, pos = 0;//, fd = -1;
    XBH_S32 s32Ret;
    struct termios newtio, oldtio;
    int length = 25;
    int offset = PACKAGE_LEN - 1;
    m_bInitDone = XBH_FALSE;

    XLOGD("ChipAm1008  start ...");

    fd_set rd;
    struct timeval tv; 
    int err;
    while(1){
        // XLOGE("ChipAm1008  readSensorData by serial ...\n\n\n");
        if (mFd < 0) {
            openSerialPort(path, 9600);
            // write(mFd, AM1008_CMD_GET_DET_VALUE, 5);
            XLOGE("ChipAm1008 write cmd. ...\n\n\n");
        }else{
            write(mFd, AM1008_CMD_GET_DET_VALUE, 5);
            memset(detValue,0,PACKAGE_LEN);
            sleep(1);
            ret = read(mFd, detValue,PACKAGE_LEN);
            if(DEBUG) {
                XLOGE("ret:%d\n",ret);
                XLOGE("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n\n\n\n\n",
                detValue[0],detValue[1],detValue[2],detValue[3],detValue[4],detValue[5],detValue[6],detValue[7],detValue[8],detValue[9],
                detValue[10],detValue[11],detValue[12],detValue[13],detValue[14],detValue[15],detValue[16],detValue[17],detValue[18],detValue[19],
                detValue[20],detValue[21],detValue[22],detValue[23],detValue[24]);
            }
            if(detValue[0] == 0x16 && detValue[1] == 0x16 && detValue[2] == 0x01 ){
                m_bInitDone = XBH_TRUE;
                co2Value = detValue[3]*256 + detValue[4];
                vocValue = detValue[5]*256 + detValue[6];
                humidityValue = (detValue[7]*256 + detValue[8]) / 10;
                temperatureValue = (detValue[9]*256 + detValue[10] - 500) / 10;
                pm25Value = detValue[19]*256 + detValue[20];
                //XLOGE("pos = %d co2Value = %d, vocValue = %d, humidityValue = %d, temperatureValue = %d, pm25Value = %d", pos,co2Value, vocValue, humidityValue, temperatureValue, pm25Value);
            }
        }
        sleep(10); //Update the data every 10 seconds
    }
}

/**
 * 获取Airsensor是否存在
 */
XBH_S32 ChipAm1008::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipAm1008::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 检查Sum
 */
XBH_U8 ChipAm1008::getCheckSum(char* arr, XBH_U8 length)
{
    XBH_U8 sum = 0, cs = 0;
    for(int i=0; i<length; i++){
        //XLOGE("arr[%d]=%x", i, arr[i]);
        sum += arr[i];
    }
    cs = 256 - sum;
    //XLOGE(" sum is %x    checksum is %x", sum, cs);
    return cs;
}

XBH_U8 ChipAm1008::sendCmd(int fd, const XBH_U8* cmd, int len)
{
    int s32Ret = -1;
    if(fd >= 0)
    {
        write(fd, cmd,len);
    }
    return s32Ret;

}

XBH_S32 ChipAm1008::getCo2Value(){
    return co2Value;
}

XBH_S32 ChipAm1008::getVocValue(){
    return vocValue;
}

XBH_S32 ChipAm1008::getHumidityValue(){
    return humidityValue;
}

XBH_S32 ChipAm1008::getTemperatureValue(){
    return temperatureValue;
}

XBH_S32 ChipAm1008::getPm25Value(){
    return pm25Value;
}

ChipAm1008::ChipAm1008(XBH_S32 uart, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    muart = uart;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    sprintf(path, "/dev/ttyS%d", muart);
    init();
    XbhMWThread::run();
    XLOGD(" end ");
}

ChipAm1008::~ChipAm1008()
{
    if (mFd >= 0) {
        close(mFd);
        mFd = -1;
    }
}
