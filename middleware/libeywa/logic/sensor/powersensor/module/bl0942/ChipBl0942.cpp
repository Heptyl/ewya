#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipBl0942"
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
#include "ChipBl0942.h"

XbhMutex                        ChipBl0942::mLock;

/*
 * 1） 本协议数据，均为16进制数据。如 “46”为十进制的[70]；
 * 2） [xx]为单字节无符号类型，双字节数据高字节在前，低字节在后；
 * 3） 波特率：9600, DataBits: 8, StopBits: 1, Parity: No。
 * 4） 此版本兼容主动上传和被动接收模式。开机默认会主动上传数据，当接收到任意一条查询指令，会切换到被动接收模式，一收一答。
 *
 */

#define PACKAGE_LEN 23
#define Power_K		688	//	功率转换系数
#define Current_K	298944	//	电流转换系数
#define Voltage_K	15883	//	电压转换系数

char PATH[128];

int pFd = -1;
char DetValue[PACKAGE_LEN];
static XBH_S32 powerValue = -1;
static XBH_S32 ApparentpowerValue = -1;
static XBH_S32 IValue = -1;
static XBH_S32 VValue = -1;

XBH_U8 BL0942_CMD_GET_DET_VALUE[] = {0X58, 0XAA};                     //获取测量结果



speed_t ChipBl0942::getBaudrate(XBH_S32 baudrate)
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

void ChipBl0942::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("powerDetection openSerialPort...\n");
    if(pFd<0)
    {
        s32Ret = openSerialPort(PATH, 9600);
        if(s32Ret == XBH_FALSE)
        {
            XLOGE("open ChipBl0942 error !!!! \n");
        }
    }
}

/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
XBH_BOOL ChipBl0942::openSerialPort(const char *PATH, XBH_S32 baudrate)
{
    struct termios newtio, oldtio;
    int ret = -1;
    if (pFd < 0)
    {
        pFd = open(PATH, O_RDWR|O_NOCTTY|O_NDELAY);
        if (pFd == -1)
        {
            XLOGE("Open /dev/ttyS Error.");
            return XBH_FALSE;
        }
        if (tcgetattr(pFd,&oldtio))
        {
            XLOGE("Get /dev/ttyS Error.");
            return XBH_FALSE;
        }
        // memcpy(&newtio, &oldtio, sizeof(newtio));
        cfmakeraw(&oldtio);
        cfsetispeed(&oldtio, B9600);
        cfsetospeed(&oldtio, B9600);
        tcflush(pFd,TCIFLUSH);
        oldtio.c_cc[VMIN] =  PACKAGE_LEN;
        if ((tcsetattr(pFd,TCSANOW,&oldtio))!=0)
        {
            XLOGE("Set /dev/ttyS Error.");
            return XBH_FALSE;
        }
    }
    return XBH_TRUE;
}

XBH_S32 ChipBl0942::readCommand(XBH_U8* pbuf, XBH_S32 len)
{
    XBH_S32 ret = XBH_FAILURE;
    XLOGD("readCommand pFd = %d\n", pFd);
    if(pFd != XBH_FAILURE)
    {
        ret = read(pFd, pbuf, len);
        XLOGE("readCommand ret = %d errno = %s\n", ret, strerror(errno));
    }
    return ret;
}

void ChipBl0942::run(const void* arg)
{
    int ret, pos = 0;//, fd = -1;
    XBH_S32 s32Ret;
    struct termios newtio, oldtio;
    int length = 23;
    int offset = PACKAGE_LEN - 1;
    XBH_FLOAT s32State;
    m_bInitDone = XBH_FALSE;

    XLOGD("ChipBl0942  start ...");

    fd_set rd;
    struct timeval tv;
    int err;

    //Delay 30s to begin
    usleep(30 * 1000 * 1000);

    while(1){
        // XLOGE("ChipBl0942  readSensorData by serial ...\n\n\n");
        if (pFd < 0) {
            openSerialPort(PATH, 9600);
            // write(pFd, AM1008_CMD_GET_DET_VALUE, 5);
            XLOGE("ChipBl0942 write cmd. ...\n\n\n");
        }else{
            write(pFd, BL0942_CMD_GET_DET_VALUE, 2);
            memset(DetValue,0,PACKAGE_LEN);
            sleep(1);
            ret = read(pFd, DetValue,PACKAGE_LEN);
            if(DetValue[ret-1] != getCheckSum(DetValue,ret-1)){
                   continue;
            }
            XLOGD("while ChipBl0942 DetValue:%02x %02x %02x \n", DetValue[10], DetValue[11], DetValue[12]);
            if(DetValue[ret-1] == getCheckSum(DetValue,ret-1)){
                m_bInitDone = XBH_TRUE;
                powerValue = DetValue[12]<<16 | DetValue[11]<<8 | DetValue[10];
                powerValue = powerValue * 100 / Power_K;
                VValue = DetValue[6]<<16 | DetValue[5]<<8 | DetValue[4];
                VValue = VValue * 100 / Voltage_K;
                IValue = DetValue[3]<<16 | DetValue[2]<<8 | DetValue[1];
                IValue = IValue * 100 / Current_K;
                s32State = (XBH_FLOAT)(powerValue / 100.00);
                XLOGD("read SensorValue  value = %f ...", s32State);

                //If read data is 0.0, do init again.
                if (s32State == 0.0)
                {
                    pFd = -1;
                    XLOGD("do init again.");
                    continue;
                }
                XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};
                sprintf(value, "%f", s32State);
                property_set("persist.vendor.xbh.powersensor.value", value);
            }
        }
        sleep(10); //Update the data every 10 seconds
    }
}

/**
 * 获取Powersensor是否存在
 */
XBH_S32 ChipBl0942::getChipExist(XBH_BOOL* bEnable)
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

XBH_S32 ChipBl0942::getChipInitDone(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 检查Sum
 */
XBH_U8 ChipBl0942::getCheckSum(char* arr, XBH_U8 length)
{
    XBH_U8 sum = 0x58;
    for(int i=0; i<length; i++){
        //XLOGE("arr[%d]=%x", i, arr[i]);
        sum += arr[i];
    }
    sum = ~sum;
    //XLOGE(" sum is %x    checksum is %x", sum, DetValue[22]);
    return sum;
}

XBH_U8 ChipBl0942::sendCmd(int fd, const XBH_U8* cmd, int len)
{
    int s32Ret = -1;
    if(fd >= 0)
    {
        write(fd, cmd,len);
    }
    return s32Ret;

}

XBH_S32 ChipBl0942::getPowerSensorValue(XBH_FLOAT * s32State){
    int ret, pos = 0;//, fd = -1;
    XBH_S32 s32Ret;
    struct termios newtio, oldtio;
    int length = 23;
    int offset = PACKAGE_LEN - 1;
    m_bInitDone = XBH_FALSE;
    XLOGD("getPowerSensorValue  start ...");
    fd_set rd;
    struct timeval tv;
    int err;
    write(pFd, BL0942_CMD_GET_DET_VALUE, 2);
    memset(DetValue,0,PACKAGE_LEN);
    usleep(40*1000);
    ret = read(pFd, DetValue,PACKAGE_LEN);
    XLOGD("getPowerSensorValue ChipBl0942 DetValue: %02x %02x %02x\n", DetValue[10], DetValue[11], DetValue[12]);
    if(DetValue[ret-1] == getCheckSum(DetValue,ret-1)){
        m_bInitDone = XBH_TRUE;
        powerValue = DetValue[12]<<16 | DetValue[11]<<8 | DetValue[10];
        powerValue = powerValue * 100 / Power_K;
        VValue = DetValue[6]<<16 | DetValue[5]<<8 | DetValue[4];
        VValue = VValue * 100 / Voltage_K;
        IValue = DetValue[3]<<16 | DetValue[2]<<8 | DetValue[1];
        IValue = IValue * 100 / Current_K;
        *s32State = (XBH_FLOAT)(powerValue / 100.00);
        return XBH_SUCCESS;
    }
    return XBH_FALSE;
}

ChipBl0942::ChipBl0942(XBH_S32 uart, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD("  begin ");
    muart = uart;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    property_get("persist.vendor.xbh.powersensoruart.path", PATH, NULL);
    init();
    XbhMWThread::run();
    XLOGD(" end ");
}

ChipBl0942::~ChipBl0942()
{
    if (pFd >= 0) {
        close(pFd);
        pFd = -1;
    }
}