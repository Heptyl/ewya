#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipNt68411"
#include "XbhLog.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "ChipNt68411.h"
#include "XbhService.h"

FLASH_DEF FlashIDTable[]={
    //En   ID1  ID2  ID3  FE2P  BYTE  SE  CE  PAGE SE_T CE_T  NAME
    {0x01 ,0x55,0xaa,0x00,0x00 ,0x00,0xd7,0xc7,256 ,0   ,100  , "Embeded 128KB S-flash"},
    {0x01 ,0x9d,0x7f,0x7c,0x01 ,0x00,0xd7,0xc7,256 ,70  ,100  , "PMC Pm25LV010A 128KB "},
    {0x01 ,0x9d,0x7f,0x7d,0x01 ,0x00,0xd7,0xc7,512 ,70  ,100  , "PMC Pm25LV020A 256KB "},
    {0x01 ,0x9d,0x7f,0x7e,0x01 ,0x00,0xd7,0xc7,1024,70  ,100  , "PMC Pm25LV040A 512KB "},
    {0x01 ,0x9d,0x7f,0x10,0x01 ,0x00,0xd7,0xc7,256 ,70  ,100  , "PMC_Pm25LD010C 128KB "},
    {0x01 ,0x9d,0x7f,0x11,0x01 ,0x00,0xd7,0xc7,512 ,70  ,100  , "PMC_Pm25LD020C 256KB "},
    {0x01 ,0x9d,0x7f,0x12,0x01 ,0x00,0xd7,0xc7,1024,70  ,100  , "PMC_Pm25LD040C 512KB "},
    {0x01 ,0xc2,0x20,0x11,0x01 ,0x00,0x20,0xc7,256 ,120 ,2000 , "MXIC MX25L1005 128K  "},
    {0x01 ,0xc2,0x20,0x12,0x01 ,0x00,0x20,0xc7,512 ,120 ,3000 , "MXIC MX25L2005 256K  "},
    {0x01 ,0xc2,0x20,0x13,0x01 ,0x00,0x20,0xc7,1024,120 ,5000 , "MXIC MX25L4005A 512K "},
    {0x01 ,0xc2,0xc2,0x14,0x01 ,0x00,0x20,0xc7,4096,300 ,30000, "MXIC MX25L1606E 2MB  "},
    {0x01 ,0xef,0x10,0xff,0x01 ,0x00,0x20,0xc7,256 ,150 ,3000 , "WINBOND W25X10 128K  "},
    {0x01 ,0xef,0x11,0xff,0x01 ,0x00,0x20,0xc7,512 ,150 ,3000 , "WINBOND W25X20 256K  "},
    {0x01 ,0xef,0x12,0xff,0x01 ,0x00,0x20,0xc7,1024,150 ,5000 , "WINBOND W25X40 512K  "},
    {0x01 ,0x00,0x10,0x10,0x00 ,0x00,0xd8,0xc7,256 ,0   ,3000 , "ST M25P10 128K       "},
    {0x01 ,0x00,0x11,0x11,0x00 ,0x00,0xd8,0xc7,512 ,0   ,3000 , "ST M25P20 256K       "},
    {0x01 ,0x00,0x12,0x12,0x00 ,0x00,0xd8,0xc7,1024,0   ,5000 , "ST M25P40 512K       "},
    {0x01 ,0x37,0x11,0x30,0x01 ,0x00,0x20,0xc7,256 ,120 ,2000 , "AMIC A25L010 128K    "},
    {0x01 ,0x37,0x12,0x30,0x01 ,0x00,0x20,0xc7,512 ,120 ,4000 , "AMIC A25L020 256K    "},
    {0x01 ,0x37,0x13,0x30,0x01 ,0x00,0x20,0xc7,1024,120 ,6000 , "AMIC A25L040 512K    "},
    {0x01 ,0x1c,0x10,0x31,0x01 ,0x00,0x20,0xc7,256 ,300 ,3000 , "EON EN25F10 128K     "},
    {0x01 ,0x1c,0x11,0x31,0x01 ,0x00,0x20,0xc7,512 ,300 ,4000 , "EON EN25F20 256K     "},
    {0x01 ,0x1c,0x12,0x31,0x01 ,0x00,0x20,0xc7,1024,300 ,6000 , "EON EN25F40 512K     "},
    {0x01 ,0xC8,0x12,0xff,0x01 ,0x00,0x20,0xc7,512 ,150 ,5000 , "GIGA GD25Q40 512K    "},
    {0x01 ,0xff,0xff,0xff,0x00 ,0x00,0xff,0   ,0   ,0   ,0    , "Unkown flash type    "},
};

static XBH_U8 MDrv_NT68411_CheckSum(XBH_U8* pu8data, XBH_U8 u8size)
{
    XBH_U8 u8Sum = 0x6E;
    do{ u8Sum ^=  pu8data[--u8size]; }while(u8size);
    return u8Sum;
}

/**
 * 设置HDMI TX的音频mute状态
*/
XBH_S32 ChipNt68411::setHdmiTxAudioMute(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 wBytes[7] = {0};
    wBytes[0] = 0x50;
    wBytes[1] = 0x84;
    wBytes[2] = 0x03;
    wBytes[3] = 0xE2;
    wBytes[4] = 0x00;
    wBytes[5] = bEnable;    //0x00: Mute, 0x01: Unmute

    XLOGD("this is hdmi tx AUDIO MUTE, AUDIO MUTE = %d\n", bEnable);

    wBytes[6] = MDrv_NT68411_CheckSum(wBytes, 6);
    XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, wBytes[0], 1, 6, &wBytes[1]);

    return  s32Ret;
}

/**
 * 获取HDMI TX的音频mute状态
*/
XBH_S32 ChipNt68411::getHdmiTxAudioMute(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("hehe ChipNt68411 getHdmiTxTiming is empty func\n");
    return  s32Ret;
}

/**
 * 设置HDMI TX的视频mute状态
*/
XBH_S32 ChipNt68411::setHdmiTxVideoMute(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 wBytes[7] = {0};
    wBytes[0] = 0x50;
    wBytes[1] = 0x84;
    wBytes[2] = 0x03;
    wBytes[3] = 0xE1;
    wBytes[4] = 0x00;
    wBytes[5] = bEnable;    //0x00: Mute, 0x01: Unmute

    XLOGD("this is hdmi tx VIDEO MUTE, MUTE = %d\n", bEnable);

    wBytes[6] = MDrv_NT68411_CheckSum(wBytes, 6);
    XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, wBytes[0], 1, 6, &wBytes[1]);

    return  s32Ret;
}

/**
 * 获取HDMI TX的视频mute状态
*/
XBH_S32 ChipNt68411::getHdmiTxVideoMute(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("hehe ChipNt68411 getHdmiTxTiming is empty func\n");
    return  s32Ret;
}

/**
 * 设置HDMI TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 ChipNt68411::setHdmiTxTiming(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 wBytes[7] = {0};
    wBytes[0] = 0x50;
    wBytes[1] = 0x84;
    wBytes[2] = 0x03;
    wBytes[3] = 0xFC;
    wBytes[4] = 0x00;
    if(value == 0)
    {
        wBytes[5] = 0x02; //480p
    }
    else if(value == 1)
    {
        wBytes[5] = 0x01; //1080p
    }
    else if(value == 2)
    {
        wBytes[5] = 0x00; //2160p
    }
    else
    {
        wBytes[5] = 0x0a;//0x00:2160p, 0x01:1080p, 0x02:480p, 0x03:1280x800, 0x04:1280x720, 0x0A:Auto
    }
    XLOGD("this is hdmi tx timing, timing = %d\n", value);

    wBytes[6] = MDrv_NT68411_CheckSum(wBytes, 6);
    XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, wBytes[0], 1, 6, &wBytes[1]);

    return  s32Ret;
}

/**
 * 获取HDMI TX的timing
 * 0：480P，1：1080P， 2：21600P ，0xFF Auto
*/
XBH_S32 ChipNt68411::getHdmiTxTiming(XBH_CHAR* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("hehe ChipNt68411 getHdmiTxTiming is empty func\n");
    return  s32Ret;
}

/**
 * 设置HDMI(DP)TX的HDCP开关状态
 * 0：OFF，1：ON
*/
XBH_S32 ChipNt68411::setHdmiTxHdcp(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 wBytes[7] = {0};
    wBytes[0] = 0x50;
    wBytes[1] = 0x84;
    wBytes[2] = 0x03;
    wBytes[3] = 0xFD;        // hdcp
    wBytes[4] = 0x00;
    wBytes[5] = !bEnable;    //0x00: OFF, 0x01: ON  其他方案0x00为HDCP ON，故反向一下，与其他项目传参保持一致


    XLOGD("this is hdmi tx hdcp, hdcp = %d\n", bEnable);

    wBytes[6] = MDrv_NT68411_CheckSum(wBytes, 6);
    XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, wBytes[0], 1, 6, &wBytes[1]);

    return  s32Ret;
}

/**
 * 开启/关闭 vbo tx 差分信号
*/
XBH_S32 ChipNt68411::setVboTxDiffSignals(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 wBytes[7] = {0};
    wBytes[0] = 0x50;
    wBytes[1] = 0x84;
    wBytes[2] = 0x03;
    wBytes[3] = 0xE4;        // VBO TX
    wBytes[4] = 0x00;
    wBytes[5] = bEnable;    //0x00: OFF, 0x01: ON


    XLOGD("this is VBO TX POWER, STATE = %d\n", bEnable);

    wBytes[6] = MDrv_NT68411_CheckSum(wBytes, 6);
    XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, wBytes[0], 1, 6, &wBytes[1]);

    return  s32Ret;
}

/**
 * 设置HDMI TX Color Space
*/
XBH_S32 ChipNt68411::setHdmiTxColorSpace(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 wBytes[7] = {0};
    wBytes[0] = 0x50;
    wBytes[1] = 0x84;
    wBytes[2] = 0x03;
    wBytes[3] = 0xF6;        // HDMI TX Color Space switch
    wBytes[4] = 0x00;
    wBytes[5] = value;       // 0x01:RGB, 0x02:YUV 422, 0X03:YUV 444, others：RGB

    XLOGD("this is VBO TX POWER, STATE = %d\n", value);

    wBytes[6] = MDrv_NT68411_CheckSum(wBytes, 6);
    XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, wBytes[0], 1, 6, &wBytes[1]);

    return  s32Ret;
}

/**
* 设置Vx1 Tx的Tcon模式
*/
XBH_S32 ChipNt68411::setVx1TxTconMode(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("nt68411 try to change dual mode to %d\n", bEnable);
    s32Ret = T_SetNt68411DivisionMode(bEnable ? 1 : 0);

    return  s32Ret;
}

/**
 * 获取HDMI TX的HDCP开关状态
*/
XBH_S32 ChipNt68411::getHdmiTxHdcp(XBH_BOOL* info)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("ChipNt68411 getHdmiTxTiming is empty func\n");
    return  s32Ret;
}

void ChipNt68411::PulseDelay(uint32_t time)
{
    usleep(time * 100);
}
/**
 * 获取HDMI(DP)TX的设备接入情况
*/
XBH_S32 ChipNt68411::getHdmiTxPlugState(XBH_BOOL* info)
{
    XBH_S32 retryNums = 10;
    XBH_U8 hdmiTxPlugInNums = 0;
    XBH_U8 hdmiTxPlugOutNums = 0;
    XBH_S32 s32Ret = XBH_SUCCESS;

    //如果68411在升级中则此函数直接返回
    if(mState == XBH_UPGRADE_RUNNING)
    {
        XLOGD("ChipNt68411 upgrade in progress,getHdmiTxPlugState Failure\n");
        return s32Ret;
    }

    while(retryNums--)
    {
        XBH_U8 pu1WData[] = NT68411_HDMI_TX_PLUG_STATE;
        XBH_U8 pu1RData[11] = {0};
        s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, NT68411_HOST_ADDR, NT68411_CMD_LEN, pu1WData);

        T_HandShake(5);

        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, NT68411_HOST_ADDR, NT68411_RSP_LEN, pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            *info = pu1RData[NT68411_READ_LOW_BYTE];
        }

        if(*info == Nt68411_HDMI_TX_PLUG_IN_STATE)
        {
            hdmiTxPlugInNums += 1;
        }

        if(*info == Nt68411_HDMI_TX_PLUG_OUT_STATE)
        {
            hdmiTxPlugOutNums += 1;
        }

        T_HandShake(5);
    }

    if(hdmiTxPlugInNums > hdmiTxPlugOutNums)
    {
        *info = Nt68411_HDMI_TX_PLUG_IN_STATE;
    }
    else
    {
        *info = Nt68411_HDMI_TX_PLUG_OUT_STATE;
    }

    XLOGD("getHdmiTxPlugState = %d\n", *info);

    return s32Ret;
}

void ChipNt68411::T_HandShake(XBH_U32 Timeout)
{
    usleep(Timeout * 1000);
}

XBH_S32 ChipNt68411::HalNt68411WriteI2c(XBH_U32 i2cNum, XBH_U32 addr, XBH_U32 offset, XBH_U32 length, XBH_U8 *buffer)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret |= XbhService::getModuleInterface()->setI2cData(i2cNum, addr, offset, REG_ADDR_COUNT, length, buffer);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("ChipNt68411 HalNt68411WriteI2c failed\n");
    }
    return s32Ret;
}

XBH_S32 ChipNt68411::HalNt68411ReadI2c(XBH_U32 i2cNum, XBH_U32 addr, XBH_U32 offset, XBH_U32 length, XBH_U8 *buffer)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret |= XbhService::getModuleInterface()->getI2cData(i2cNum, addr, offset, REG_ADDR_COUNT, length, buffer);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("ChipNt68411 HalNt68411ReadI2c failed\n");
    }
    return s32Ret;
}

XBH_S32 ChipNt68411::T_GetNtVersion(XBH_U32 *version)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 pu1WData[] = NT68411_CMD_VERSION;
    XBH_U8 pu1RData[11] = {0};
    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, NT68411_HOST_ADDR, NT68411_CMD_LEN, pu1WData);

    T_HandShake(5);

    s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, NT68411_HOST_ADDR, NT68411_RSP_LEN, pu1RData);
    if(s32Ret == XBH_SUCCESS)
    {
        *version = (((XBH_U32)pu1RData[NT68411_READ_HIGH_BYTE] << 8) | pu1RData[NT68411_READ_LOW_BYTE]);
        XLOGD("T_GetNtVersion = 0x%x\n", *version);
    }
    return s32Ret;
}

XBH_S32 ChipNt68411::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 version = {VERSION_DEFAULT};
    T_GetNtVersion(&version);
    if (strVersion != NULL)
    {
        sprintf(strVersion, "%04X", version);
        s32Ret = XBH_SUCCESS;
    }
    else
    {
        XLOGE("strVersion is NULL\n");
    }
    return s32Ret;
}

XBH_S32 ChipNt68411::setPmuxGpio(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 fd = -1;
    fd = open(PINCTRL_NARCOS_DEV, O_WRONLY);
    if (fd < 0)
    {
        XLOGE("setPmux : open %s failed !!!!", PINCTRL_NARCOS_DEV);
        return XBH_FAILURE;
    }

    const char *cmd = bEnable ? "os2_to_gpio" : "os2_to_i2c";
    size_t cmd_len = bEnable ? 11 : 10;

    s32Ret = write(fd, cmd, cmd_len);
    if (s32Ret < 0)
    {
        XLOGE("setPmux : write to %s failed !!!!", PINCTRL_NARCOS_DEV);
        s32Ret = XBH_FAILURE;
    }
    else
    {
        s32Ret = XBH_SUCCESS;
    }

    return s32Ret;
}

int32_t ChipNt68411::Nt68411VersionSupport(uint16_t version)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int32_t i = 0;
    //limit the version we can upgrade to prevent 68411 read version issue.
    uint16_t whitelist[] = {0x2316, 0x2927, 0x4a17, 0xeedd, 0x5123, 0x5124, 0x5207, 0x5209, 0x5213};
    for(i = 0; i < sizeof(whitelist); i++)
    {
        if(whitelist[i] == version)
        {
            return s32Ret;
        }
    }

    return XBH_FAILURE;
}

void ChipNt68411::IIC_Start()
{
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SDA, 1);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 1);
    usleep (400);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SDA, 0);
    usleep (400);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 0);
}

void ChipNt68411::IIC_Stop()
{
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SDA, 0);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 0);
    usleep (400);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 1);
    usleep (400);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SDA, 1);
    usleep (400);
}

void ChipNt68411::IIC_SendByte(XBH_U8 _ucByte) 
{
    for (int i = 0; i < 8; ++i) {
        // 释放SCL，准备发送数据位
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 0);

        // 设置SDA为当前位的值
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SDA, (_ucByte & 0x80) ? 1 : 0);
        _ucByte <<= 1; // 移位，准备下一个数据位
        usleep(400);

        // 拉高SCL，数据位被采样
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 1);
        usleep(400);
    }
    //特殊波段
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 0);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SDA, 1);
    usleep(400);
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 1);
    usleep(400);

    // 释放SCL，完成数据发送
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_GPIO_NT68411_SCL, 0);
    usleep(100);
}

int32_t ChipNt68411::HalNt68411SetIsp(uint8_t flag) {
    XBH_S32 s32Ret = XBH_SUCCESS;
    setPmuxGpio(true);

    const uint8_t* addresses;
    uint8_t addressList1[] = {Nt68411_Set_Isp_ADDR, Nt68411_Set_Isp_ADDR2, Nt68411_Set_Isp_ADDR3, Nt68411_Set_Isp_ADDR4};
    uint8_t addressList2[] = {Nt68411_Set_Isp_ADDR, Nt68411_Set_Isp_ADDR2, Nt68411_Set_Isp_ADDR3, Nt68411_Exit_Isp_ADDR4};

    if(flag)
    {
        addresses = addressList1;
    }
    else
    {
        addresses = addressList2;
    }

    for (uint8_t i = 0; i < sizeof(addressList1) / sizeof(addressList1[0]); ++i) 
    {
        IIC_Start();
        IIC_SendByte(addresses[i]);
    }

    IIC_Stop();

    setPmuxGpio(false);
    return s32Ret;
}

int32_t ChipNt68411::T_ISPMode(uint8_t enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    XLOGD("---> T_ISPMode enable=%d\n", enable);

    if(enable)
    {
        XLOGD("T_ISPMode EnterIsp\n");
        s32Ret |= HalNt68411SetIsp(0x01);
    }
    else
    {
        XLOGD("T_ISPMode ExitIsp\n");
        s32Ret |= HalNt68411SetIsp(0x00);
    }
    return s32Ret;
}

int32_t ChipNt68411::T_ISPStatusReply()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x6e;
    uint8_t pu1RData[3] = {0};
    uint8_t ID_H = 0;
    uint8_t ID_L = 0;

    XLOGD("---> T_ISPStatusReply\n");

    s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
    if(s32Ret == 0)
    {
        ID_H = pu1RData[1];
        ID_L = pu1RData[2];
        XLOGD("T_ISPStatusReply ID_H=0x%x,ID_L=0x%x\n", ID_H, ID_L);
        if(ID_H == 0xa4 && ID_L == 0x00)
        {
            McuId = (((uint16_t)ID_H << 8) | ID_L);
        }
        else
        {
            return XBH_FAILURE;
        }
    }
    return s32Ret;
}

int32_t ChipNt68411::T_SetRegSRAMWithReply(uint8_t AddrH,uint8_t AddrL,uint8_t Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x04;
    uint8_t pu1WData[5] = {0x00};
    uint8_t pu1RData[3] = {0};
    uint8_t ReplyCode = 0;

    pu1WData[0] = CMD_SET_REG_SRAM;
    pu1WData[1] = AddrH;
    pu1WData[2] = AddrL;
    pu1WData[3] = Data;
    pu1WData[4] = (mI2cAddr*2)^u1Addr^CMD_SET_REG_SRAM^AddrH^AddrL^Data;

    //XLOGD("---> T_SetRegSRAMWithReply AddrH=0x%x,AddrL=0x%x,Data=0x%x\n", AddrH, AddrL, Data);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    T_HandShake(5);

    if(s32Ret == XBH_SUCCESS)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ CMD_SET_REG_SRAM) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_SendSpecialCommand()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t i;
    uint8_t NT_Table[]={
        0x54,0xf0,0x00,
        0x4f,0xf0,0x11,
        0x50,0xf0,0x90,
        0x53,0xf0,0x83,
        0x57,0x8b,0xff,
        0x04,0x8b,0x00,
        0xd0,0x80,0x41,
        0x46,0x8b,0x00,
        0x22,0xf0,0x00,
        0x24,0xf0,0x00,
        0x67,0xf0,0x20,
        0x4c,0xf0,0x40,
    };
    uint8_t NT68370_380Table[] = {
        0xde,0xf0,0x23,
        0x71,0xf0,0x01,
        0x54,0xf0,0x00,
        0x4f,0xf0,0x11,
        0x50,0xf0,0x94,
        0x53,0xf0,0x83,
        0x57,0x8b,0xff,
        0x04,0x8b,0x00,
        0xd0,0x80,0x41,
        0x46,0x8b,0x00,
        0x22,0xf0,0x00,
        0x24,0xf0,0x00,
    };
    uint8_t NT810_870Table[] = {
        0x54,0xf0,0x00,
        0x4f,0xf0,0x11,
        0x50,0xf0,0x90,
        0x53,0xf0,0x83,
        0x57,0x8b,0xff,
        0x04,0x8b,0x00,
        0xd0,0x80,0x41,
        0x22,0xf0,0x00,
        0x24,0xf0,0x00,
        0x50,0xf0,0x91,
        0x3e,0x8b,0xf2,
        0x3f,0x8b,0x51,
        0x45,0x8b,0x60,
    };
    uint8_t NT68390_Table[]={
        0xde,0xf0,0x23,
        0xe0,0xf1,0x01,
        0xe6,0xf1,0x01,
        0xeb,0xf1,0x01,
        0x54,0xf0,0x00,
        0x4f,0xf0,0x11,
        0x53,0xf0,0x83,
        0x22,0xf0,0x00,
        0x24,0xf0,0x00,
        0x67,0xf0,0x20,
        0x04,0xd9,0xf6,
        0x05,0xd9,0xd1,
        0x08,0xd9,0x05,
        0x09,0xd9,0x61,
        0x0a,0xd9,0xff,
        0x0b,0xd9,0x84,
        0x0c,0xd9,0x30,
        0x29,0xd9,0x04,
    };
    uint8_t NT68411_Table[]={
        // 0xf0,0x54,0x00,
        0xf0,0xde,0x23,
        0xf1,0xe0,0x01,
        0xf1,0xe6,0x01,
        0xf1,0xeb,0x01,
        0xf0,0x54,0x00,
        0xf0,0x4f,0x11,
        0xf0,0x53,0x83,
        0xf0,0x22,0x00,
        0xf0,0x24,0x00,
        0xd9,0x04,0xf6,
        0xd9,0x05,0xd1,
        0xd9,0x08,0x05,
        0xd9,0x09,0x61,
        0xd9,0x0a,0xff,
        0xd9,0x0b,0x84,
        0xd9,0x0c,0x30,
    };

    XLOGD("### Cmd_T_SendSpecialCommand McuId: 0x%x\n", McuId);

    if(McuId == 0xa515)
    {
        s32Ret = T_SetRegSRAMWithReply(0xf0, 0x80, 0x00);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGD("Cmd_T_SendSpecialCommand fail\n");
            return s32Ret;
        }
    }

    if(McuId == 0xA390)
    {
        for(i = 0; i < 54; i += 3)
        {
            s32Ret = T_SetRegSRAMWithReply(NT68390_Table[i], NT68390_Table[i + 1], NT68390_Table[i + 2]);
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGD("Cmd_T_SendSpecialCommand fail\n");
                return s32Ret;
            }
        }
    }
    else if(McuId == 0xA400)
    {
        for(i = 0; i < 48; i += 3)
        {
            s32Ret = T_SetRegSRAMWithReply(NT68411_Table[i], NT68411_Table[i + 1], NT68411_Table[i + 2]);
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGD("Cmd_T_SendSpecialCommand fail\n");
                return s32Ret;
            }
        }
        // set NT68400 clock speed
        s32Ret = T_SetRegSRAMWithReply(0xF0, 0x4C, 0x40);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGD("Cmd_T_SendSpecialCommand set NT68400 clock speed fail\n");
            return s32Ret;
        }
        T_HandShake(10);
        s32Ret = T_SetRegSRAMWithReply(0xD9, 0x29, 0x04);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGD("Cmd_T_SendSpecialCommand set NT68400 clock speed fail\n");
            return s32Ret;
        }
        T_HandShake(50);
    }
    else if(McuId == 0xa537 || McuId == 0xa380)
    {
        for(i = 0; i < 36; i += 3)
        {
            s32Ret = T_SetRegSRAMWithReply(NT68370_380Table[i], NT68370_380Table[i + 1], NT68370_380Table[i + 2]);
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGD("Cmd_T_SendSpecialCommand fail\n");
                return s32Ret;
            }
        }
    }
    else if(McuId == 0xa810 || McuId == 0xa870)
    {
        for(i = 0; i < 39; i += 3)
        {
            s32Ret = T_SetRegSRAMWithReply(NT810_870Table[i], NT810_870Table[i + 1], NT810_870Table[i + 2]);
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGD("Cmd_T_SendSpecialCommand fail\n");
                return s32Ret;
            }
        }
    }
    else
    {
        for(i = 0; i < 36; i += 3)
        {
            s32Ret = T_SetRegSRAMWithReply(NT_Table[i], NT_Table[i + 1], NT_Table[i + 2]);
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGD("Cmd_T_SendSpecialCommand fail\n");
                return s32Ret;
            }
        }
    }
    return s32Ret;
}

int32_t ChipNt68411::T_GetRegSRAMWithReply(uint8_t AddrH,uint8_t AddrL,uint8_t * Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x03;
    uint8_t pu1WData[4] = {0x00};
    uint8_t pu1RData[4] = {0};

    pu1WData[0] = CMD_GET_REG_SRAM;
    pu1WData[1] = AddrH;
    pu1WData[2] = AddrL;
    pu1WData[3] = (mI2cAddr*2) ^ u1Addr ^ CMD_GET_REG_SRAM ^ AddrH ^ AddrL;

    //XLOGD("---> T_GetRegSRAMWithReply AddrH=0x%x,AddrL=0x%x\n", AddrH, AddrL);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    T_HandShake(5);

    if(s32Ret == XBH_SUCCESS)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            *Data = pu1RData[2];
        }
    }

    return s32Ret;
}

void ChipNt68411::FnSetSPICmdBufAndBackUpValue(void)
{
    uint8_t Dat;
    uint8_t tmp;
    uint8_t i;

    XLOGD("### FnSetSPICmdBufAndBackUpValue\n");
    T_HandShake(10);
    T_GetRegSRAMWithReply(0xa0, 0x00, &Dat);
    tmp = Dat;

    T_SetRegSRAMWithReply(0xa0, 0x00, 0xaa);
    T_HandShake(10);

    T_GetRegSRAMWithReply(0xa0, 0x00, &Dat);

    if(Dat == 0xaa)
    {
        T_SetRegSRAMWithReply(0xa0, 0x00, 0x55);
        T_HandShake(100);

        T_GetRegSRAMWithReply(0xa0, 0x00, &Dat);

        if(Dat == 0x55)
        {
            usSPICmdBufAddr = 0xa800;
        }
        else
        {
            usSPICmdBufAddr = 0x9800;
        }
    }
    else
    {
        usSPICmdBufAddr = 0x9800;
    }
    T_SetRegSRAMWithReply(0xa0,0x00,0x55);
    T_HandShake(10);

    for(i = 0; i < sizeof(ucSPI_BackUpBuf); i++)
    {
        T_GetRegSRAMWithReply(0xf0, 0xc0 + i, &Dat);
        ucSPI_BackUpBuf[i] = Dat;
    }
}

int32_t ChipNt68411::T_RunNewSPICmdWithReply(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x01;
    uint8_t pu1WData[2] = {0x00};
    uint8_t pu1RData[4] = {0};
    uint8_t ReplyCode = 0;

    pu1WData[0] = 0x25;
    pu1WData[1] = (mI2cAddr*2) ^ u1Addr ^ 0x25;

    XLOGD("---> T_RunNewSPICmdWithReply\n");

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    T_HandShake(100);

    if(s32Ret == XBH_SUCCESS)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ 0x25) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

uint8_t ChipNt68411::SearchFlashID(uint8_t mcuFlashID[3])
{
    uint8_t i;
    uint8_t j;
    uint8_t m1;
    uint8_t m2;
    uint8_t m3;

    for(i = 0; i < 128; i++)
    {
        m1 = 0;
        m2 = 0;
        m3 = 0;
        if(FlashIDTable[i].ID1 == 0xff)
            break;
        for(j = 0; j < 3; j++)
        {
            if(mcuFlashID[j] == FlashIDTable[i].ID1 && !m1)
                m1 = 1;
            else if(mcuFlashID[j] == FlashIDTable[i].ID2 && !m2)
                m2 = 1;
            else if(mcuFlashID[j] == FlashIDTable[i].ID3 && !m3)
                m3 = 1;
        }
        if(FlashIDTable[i].ID3 == 0xff)
            m3 = 1;
        if(m1 && m2 && m3)
            return i;
    }
    return 0xff;
}

int32_t ChipNt68411::T_MultiFunc(uint8_t Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x02;
    uint8_t pu1WData[3] = {0x00};

    pu1WData[0] = CMD_MULTI_FUNC;
    pu1WData[1] = Data;
    pu1WData[2] = (mI2cAddr*2) ^ u1Addr ^ CMD_MULTI_FUNC ^ Data;

    XLOGD("---> T_MultiFunc Data=0x%x\n", Data);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    T_HandShake(5);

    return s32Ret;
}

bool ChipNt68411::CheckSpecialFlash(void)
{
    uint8_t WrtCntTab[16] = {0};
    uint8_t WData[16] = {0};
    uint8_t WrtCntTabLength = 0;
    uint8_t WDataLength = 0;
    uint16_t SPI_Cmd_Buf_Addr = usSPICmdBufAddr;
    uint16_t TmpAddr;
    uint16_t i;
    uint8_t F0C0_value;
    uint8_t dummy;
    uint8_t RData[16] = {0};

    uint8_t Id;
    uint8_t fID[3];
    uint8_t ret;

    XLOGD("### CheckSpecialFlash\n");

    WrtCntTab[WrtCntTabLength++] = 1;
    WrtCntTab[WrtCntTabLength++] = 3;
    WData[WDataLength++] = 0x9f;

    TmpAddr = SPI_Cmd_Buf_Addr;
    F0C0_value = (WrtCntTabLength / 2) << 4 | 0x03;

    T_SetRegSRAMWithReply(0xf0, 0xc0, F0C0_value);

    T_SetRegSRAMWithReply(0xf0, 0xc1, SPI_Cmd_Buf_Addr >> 8);

    dummy = SPI_Cmd_Buf_Addr & 0xFF;
    T_SetRegSRAMWithReply(0xf0, 0xc2, dummy);

    T_SetRegSRAMWithReply(0xf0, 0xc3, SPI_Cmd_Buf_Addr >> 8);

    dummy = (SPI_Cmd_Buf_Addr + WrtCntTabLength) & 0xFF;
    T_SetRegSRAMWithReply(0xf0, 0xc4, dummy);

    T_SetRegSRAMWithReply(0xf0, 0xc5, SPI_Cmd_Buf_Addr >> 8);

    dummy = (SPI_Cmd_Buf_Addr + WrtCntTabLength + WDataLength) & 0xFF;
    T_SetRegSRAMWithReply(0xf0, 0xc6, dummy);

    for(i = 0; i < WrtCntTabLength; i++)
    {
        T_SetRegSRAMWithReply((uint8_t)(TmpAddr >> 8), (uint8_t)TmpAddr, WrtCntTab[i]);
        TmpAddr++;
    }
    for(i = 0; i < WDataLength; i++)
    {
        T_SetRegSRAMWithReply((uint8_t)(TmpAddr >> 8), (uint8_t)TmpAddr, WData[i]);
        TmpAddr++;
    }

    T_RunNewSPICmdWithReply();
    T_HandShake(200);

    for(i = 0; i < sizeof(RData); i++)
    {
        T_GetRegSRAMWithReply((uint8_t)((SPI_Cmd_Buf_Addr + i) >> 8), (uint8_t)(SPI_Cmd_Buf_Addr + i), &RData[i]);
    }

    fID[0] = RData[WrtCntTabLength + WDataLength];
    fID[1] = RData[WrtCntTabLength + WDataLength + 1];
    fID[2] = RData[WrtCntTabLength + WDataLength + 2];

    Id = SearchFlashID(fID);
    if(Id != 0xff)
    {
        Flash = FlashIDTable[Id];
        MaxPages = Flash.Page;
        ret = T_SetRegSRAMWithReply(0xf0, 0x67, Flash.SE_Code);

        MultiID = 0x00;

        if(Flash.ByteWrite && ret == 0)
            MultiID |= 0x02;
        ret = T_MultiFunc(MultiID);
        T_HandShake(5);
        return true;
    }

    return false;
}

void ChipNt68411::RestoreSPIRegValue(void)
{
    uint8_t i;

    XLOGD("### RestoreSPIRegValue\n");
    for(i = 0; i < sizeof(ucSPI_BackUpBuf); i++)
    {
        T_SetRegSRAMWithReply(0xf0, 0xc0 + i, ucSPI_BackUpBuf[i]);
        T_HandShake(10);
    }
}

int32_t ChipNt68411::T_GetVersion(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x01;
    uint8_t pu1WData[2] = {0x00};

    pu1WData[0] = CMD_GET_VERSION;
    pu1WData[1] = (mI2cAddr*2) ^ u1Addr ^ CMD_GET_VERSION;

    XLOGD("---> T_GetVersion\n");
    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    T_HandShake(5);

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_GetFlashID(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t SPI_ID_Tab[]={0x90, 0x9f, 0xab};
    uint8_t i;
    uint8_t l;
    uint8_t Id;
    uint8_t Data;
    uint8_t fID[3];

    Flash.En = 0;

    FnSetSPICmdBufAndBackUpValue();
    if(CheckSpecialFlash())
    {
        RestoreSPIRegValue();
        return XBH_FAILURE;
    }
    RestoreSPIRegValue();
    for(l = 0; l < 3; l++)
    {
        s32Ret = T_SetRegSRAMWithReply(0xf0, 0x68, SPI_ID_Tab[l]);
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGD("Cmd_T_GetFlashID error\n");
            return XBH_FAILURE;
        }

        T_GetVersion();
        T_HandShake(20);
        for(i = 0; i < 3; i++)
        {
            s32Ret = T_GetRegSRAMWithReply(0xf0, 0x64 + i, &Data);
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGD("Cmd_T_GetFlashID error\n");
                return XBH_FAILURE;
            }
            fID[i] = Data;
            if(fID[0] == 0x00 || fID[0] == 0xff)
                break;
        }
        XLOGD("nt68411 flash id 0x%02x%02x%02x\n", fID[0], fID[1], fID[2]);
        Id = SearchFlashID(fID);
        if(Id != 0xff)
        {
            Flash = FlashIDTable[Id];
            MaxPages = Flash.Page;
            s32Ret = T_SetRegSRAMWithReply(0xf0, 0x67, Flash.SE_Code);

            MultiID = 0x00;
            if(Flash.ByteWrite && s32Ret == 0)
                MultiID |= 0x02;

            T_MultiFunc(MultiID);
            T_HandShake(5);
            return s32Ret;
        }
    }
    return s32Ret;
}

int32_t ChipNt68411::WpEnable(void)
{
    const unsigned short McuAddr[][3]={
        {0xf005,0xf000,0xf0b0}, //PA
        {0xf006,0xf001,0xf0b1}, //PB
        {0xf007,0xf002,0xf0b2}, //PC
        {0xf008,0xf003,0xf0b3}, //PD
        {0xf009,0xf004,0xf0b4}, //PE
        {0xf011,0xf010,0xf0b5}, //PF

        {0xf012,0xf013,0xf0b6}, //PG
        {0xf114,0xf115,0xf130}, //PH
        {0xf116,0xf117,0xf131}, //PI
        {0xf118,0xf119,0xf132}, //PJ
        {0xf11A,0xf11B,0xf133}, //PK
        {0xf11C,0xf11D,0xf134}, //PL
        {0xf11E,0xf11F,0xf135}, //PM
    };
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t temp;
    uint8_t Mask;
    int32_t i;
    int32_t j;

    for(i = 0; i < 13; i++)
    {
        if(i >= 5 && i <= 6)
        {
            if(McuId != 0xa537 && McuId != 0xa380 && McuId != 0xa390 && McuId != 0xA400)
            {
                break;
            }
        }
        else if(i >= 7)
        {
            if(McuId != 0xa390 && McuId != 0xA400)
            {
                break;
            }
        }

        switch(i)
        {
            case 0:
                Mask = WpDefine >> 24;
                Mask &= 0xff;
                break;
            case 1:
                Mask = WpDefine >> 16;
                Mask &= 0xff;
                break;
            case 2:
                Mask = WpDefine >> 8;
                Mask &= 0xff;
                break;
            case 3:
                Mask = (uint8_t)WpDefine;
                Mask &= 0xff;
                break;
            case 4:
                Mask = Wp_PortE;
                Mask &= 0xff;
                break;
            case 5:
                Mask = Wp_PortF;
                Mask &= 0xff;
                break;
            case 6:
                Mask = (uint8_t)Wp_PortG_J;
                Mask &= 0xff;
                break;
            case 7:
                Mask = Wp_PortG_J >> 8;
                Mask &= 0xff;
                break;
            case 8:
                Mask = Wp_PortG_J >> 16;
                Mask &= 0xff;
                break;
            case 9:
                Mask = Wp_PortG_J >> 24;
                Mask &= 0xff;
                break;
            case 10:
                Mask = (uint8_t)Wp_PortK_M;
                Mask &= 0xff;
                break;
            case 11:
                Mask = Wp_PortK_M >> 8;
                Mask &= 0xff;
                break;
            case 12:
                Mask = Wp_PortK_M >> 16;
                Mask &= 0xff;
                break;
        };

        for(j = 0; j < 3; j++)
        {
            s32Ret |= T_GetRegSRAMWithReply(McuAddr[i][j] >> 8, (uint8_t)McuAddr[i][j], &temp);
            PulseDelay(100);

            if(j == 0)
                temp &= ~Mask;
            else if(j == 1)
            {
                temp |= Mask;
            }
            else if(j == 2)
                temp |= Mask;

            s32Ret |= T_SetRegSRAMWithReply(McuAddr[i][j] >> 8, (uint8_t)McuAddr[i][j], temp);
            PulseDelay(20);
        }
    }
    return s32Ret;
}

int32_t ChipNt68411::T_SpiCommandWithReply(uint8_t *Data,uint8_t WLen,uint8_t RLen)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x09;
    uint8_t pu1WData[10] = {0x00};
    uint8_t pu1RData[3] = {0};
    uint8_t ReplyCode = 0;
    uint8_t i = 0;

    pu1WData[0] = CMD_SPI_CMD;
    for(i = 0; i < 7; i++)
    {
        pu1WData[1 + i] = Data[i];
    }
    pu1WData[8] = ((RLen << 4) | WLen);
    pu1WData[9] = (mI2cAddr*2) ^ u1Addr;
    for(i = 0; i < 9; i++)
    {
        pu1WData[9] ^= pu1WData[i];
    }

    XLOGD("---> T_SpiCommandWithReply WLen=0x%x,RLen=0x%x\n", WLen, RLen);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    T_HandShake(3);

    if(s32Ret == XBH_SUCCESS)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ CMD_SPI_CMD) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_BlockProtect(bool Wp)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t i;
    uint8_t Cmd[7];
    uint8_t sta;

    for(i = 0; i < 7; i++)
    {
        Cmd[i] = 0;
    }

    if(Flash.ByteWrite)
    {
        Cmd[0] = 0x50;
    }
    else
    {
        Cmd[0] = 0x06;
    }

    if(Wp)
    {
        sta = 0x9c;
    }
    else
    {
        sta = 0;
    }

    s32Ret |= T_SpiCommandWithReply(Cmd, 1, 0);
    T_HandShake(2);

    Cmd[0] = 0x01;
    Cmd[1] = sta;
    s32Ret |= T_SpiCommandWithReply(Cmd, 2, 0);
    T_HandShake(2);

    PulseDelay(Flash.SE_Time*10);
    return s32Ret;
}

int32_t ChipNt68411::SetSectorErasePage(uint8_t  page)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if(page)
    {
        MultiID |= 0x08;
    }
    else
    {
        MultiID &= ~0x08;
    }

    s32Ret |= T_MultiFunc(MultiID);
    T_HandShake(10);
    return s32Ret;
}

int32_t ChipNt68411::T_BlockEraseWithReply(uint8_t From,uint8_t To, uint16_t delayTime)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x09;
    uint8_t pu1WData[10] = {0x00};
    uint8_t pu1RData[3] = {0};
    uint8_t ReplyCode = 0;
    uint8_t i = 0;

    pu1WData[0] = CMD_BLOCK_ERASE;
    pu1WData[1] = 0xaa;
    pu1WData[2] = 0x55;
    pu1WData[3] = 0xcc;
    pu1WData[4] = 0x33;
    pu1WData[5] = 0xe2;
    pu1WData[6] = 0x1d;
    pu1WData[7] = From;
    pu1WData[8] = (To);
    pu1WData[9] = (mI2cAddr*2) ^ u1Addr;
    for(i = 0; i < 9; i++)
    {
        pu1WData[9] ^= pu1WData[i];
    }

    XLOGD("---> T_BlockEraseWithReply From=0x%x,To=0x%x,delayTime=%d\n", From, To, delayTime);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);

    T_HandShake(delayTime * 1.5);

    if(s32Ret == XBH_SUCCESS)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ CMD_BLOCK_ERASE) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::BlockErase(uint16_t StartBlock, uint16_t EndBlock)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint16_t TimeOutBlocks;

    TimeOutBlocks = EndBlock - StartBlock;

    if(TimeOutBlocks == 0)
    {
        TimeOutBlocks = 1; // this means only erase 1 sector, so we delay at least 1 sector time
    }

    s32Ret = T_BlockEraseWithReply(StartBlock, EndBlock, Flash.SE_Time*TimeOutBlocks);
    return s32Ret;
}

int32_t ChipNt68411::FnEraseIDSector(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint16_t SectorNo;
    SectorNo = (0xEFF0 / 4096); // make ID string address as sector unit.

    if(Flash.Page >= 4096)
    {
        SetSectorErasePage(0);
        s32Ret = BlockErase(SectorNo, SectorNo);
    }
    else
    {
        s32Ret = BlockErase(SectorNo, SectorNo);
    }
    return s32Ret;
}

int32_t ChipNt68411::Fn68390ProgramEnable(bool En)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t result;

    s32Ret |= T_GetRegSRAMWithReply(0xF0, 0xDE, &result);
    PulseDelay(100);

    if(En)
    {
        result &= ~0x02;
        s32Ret |=T_SetRegSRAMWithReply(0xF0, 0xDE, result);
        PulseDelay(20);
    }
    else
    {
        result |= 0x02;
        s32Ret |= T_SetRegSRAMWithReply(0xF0, 0xDE, result);
        PulseDelay(20);
    }
    return s32Ret;
}

int32_t ChipNt68411::T_ChipEraseWithReply(uint16_t delayTime)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x07;
    uint8_t pu1WData[8] = {0x00};
    uint8_t pu1RData[3] = {0};
    uint8_t ReplyCode = 0;
    uint8_t i = 0;

    pu1WData[0] = CMD_CHIP_ERASE;
    pu1WData[1] = 0xaa;
    pu1WData[2] = 0x55;
    pu1WData[3] = 0xcc;
    pu1WData[4] = 0x33;
    pu1WData[5] = 0xe2;
    pu1WData[6] = 0x1d;
    pu1WData[7] = (mI2cAddr*2) ^ u1Addr;
    for(i = 0; i < 7; i++)
    {
        pu1WData[7] ^= pu1WData[i];
    }

    XLOGD("---> T_ChipEraseWithReply delayTime=%d\n", delayTime);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);

    T_HandShake(delayTime);

    if(s32Ret == 0)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == 0)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ CMD_CHIP_ERASE) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_Erase(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t i;
    uint16_t j;
    uint16_t BlkStart;
    uint16_t BlkEnd;

    for(i = 0; i < 5; i++)
    {
        s32Ret = XBH_SUCCESS;
        XLOGD("nt68411 start erasing flash ...\n");

        if(Fe2pMode == FE2P_MODE)
        {
            for(j = 0; j < Flash.Page / 8; j += 16)   //each block = 4k
            {
                BlkStart = j;
                BlkEnd = (j + 16) - 2;  //with FE2P, do not erase 60~64K

                if(Flash.Page >= 4096)
                {
                    if(BlkStart < 256)
                    {
                        s32Ret |= SetSectorErasePage(0);
                    }
                    else
                    {
                        s32Ret |= SetSectorErasePage(1);
                    }
                }
                s32Ret |= BlockErase(BlkStart,BlkEnd);
            }
        }
        else if(Fe2pMode == DIRECT_FE2P_MODE)
        {
            if(Flash.Page >= 4096)
            {
                if(DirFE2P_Offset >= 256)
                {
                    //Erase first half 8M of 16M flash.
                    s32Ret |= SetSectorErasePage(0);
                    s32Ret |= BlockErase(0,255);

                    //Erase from 8M to DirFE2P_Offset.
                    s32Ret |= SetSectorErasePage(1);
                    s32Ret |= BlockErase(0,DirFE2P_Offset - 1 - 256);

                    if(DirFE2P_Length)
                    {
                        //Erase from DirFE2P_Offset+DirFE2P_Length to Flash End.
                        BlkStart = DirFE2P_Offset + DirFE2P_Length - 256;
                    }
                    else
                    {
                        //Erase from DirFE2P_Offset+4 to Flash End.
                        BlkStart = DirFE2P_Offset + 4 - 256;
                    }
                    BlkEnd = Flash.Page / 8 - 1 - 256;
                    if(BlkEnd > BlkStart)
                    {
                        s32Ret |= BlockErase(BlkStart, BlkEnd);
                    }
                }
                else
                {
                    //Erase from 0 to DirFE2P_Offset.
                    s32Ret |= SetSectorErasePage(0);
                    s32Ret |= BlockErase(0, DirFE2P_Offset - 1);

                    if(DirFE2P_Length)
                    {
                        BlkStart = DirFE2P_Offset + DirFE2P_Length; // DirFE2P_Length unit is 4k.
                    }
                    else
                    {
                        BlkStart = DirFE2P_Offset + 4;
                    }
                    s32Ret |= BlockErase(BlkStart, 255);

                    //Erase second half 8M of 16M flash.
                    s32Ret |= SetSectorErasePage(1);
                    s32Ret |= BlockErase(0, 255);
                }
            }
            else
            {
                s32Ret |= BlockErase(0, DirFE2P_Offset - 1); //Erase all Flash data address < Offset

                if(DirFE2P_Length)
                {
                    BlkStart = DirFE2P_Offset + DirFE2P_Length; // DirFE2P_Length unit is 4k.
                }
                else
                {
                    BlkStart = DirFE2P_Offset + 4;
                }
                BlkEnd = Flash.Page/8 -1;

                if(BlkEnd > BlkStart)
                {
                    s32Ret |= BlockErase(BlkStart, BlkEnd);     //Erase all Flash data address > Offset
                }

            }
        }
        else if(DualFwEnable)
        {
            // only erase Bottom/second half flash.
            if(Flash.Page >= 4096)
            {
                s32Ret |= SetSectorErasePage(1);
                s32Ret |= BlockErase(0, 255);
                XLOGD("Flash.Page >= 4096 s32Ret = %d \n", s32Ret);
            }
            else
            {
                XLOGD("Flash.Page < 4096\n");
                BlkStart = (Flash.Page / 8) / 2; // Mcu.Page/8 = how many sectors.
                BlkEnd = (Flash.Page/8)-1;
                s32Ret |= BlockErase(BlkStart, BlkEnd);
            }
        }
        else
        {
            //6seconds M25P20 erase time max<6sec
            //5seconds w25x40b erase time max<4sec
            s32Ret |= T_ChipEraseWithReply(Flash.CE_Time);
        }

        if(s32Ret == XBH_SUCCESS)
        {
            break;
        }
        T_HandShake(5);
        XLOGD("nt68411 erasing flash error, try again ...\n");
    }
    XLOGD("nt68411 finish erasing flash.\n");
    return s32Ret;
}

int32_t ChipNt68411::T_SetExtendAddrWithReply(uint8_t Block)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x02;
    uint8_t pu1WData[3] = {0x00};
    uint8_t pu1RData[3] = {0};
    uint8_t ReplyCode = 0;

    pu1WData[0] = CMD_SET_EXTEND_ADDR;
    pu1WData[1] = Block;
    pu1WData[2] = (mI2cAddr*2) ^ u1Addr ^ CMD_SET_EXTEND_ADDR ^ Block;

    XLOGD("---> T_SetExtendAddrWithReply Block=%d\n", Block);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);

    T_HandShake(3);

    if(s32Ret == XBH_SUCCESS)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == XBH_SUCCESS)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ CMD_SET_EXTEND_ADDR) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_SetExntend(uint16_t Pg)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if((Pg % 256) == 0 && (Pg / 256) <= 15)
    {
        s32Ret = T_SetExtendAddrWithReply(Pg >> 8);
    }
    return s32Ret;
}

bool ChipNt68411::CheckOverLap(uint16_t PageNo)
{
    uint32_t i;
    uint32_t Offset;

    if(Fe2pMode == DIRECT_FE2P_MODE)
    {
        Offset = DirFE2P_Offset * 4096;

        if(DirFE2P_Length)
        {
            if((PageNo * 512) >= Offset && (PageNo * 512) < (Offset + DirFE2P_Length * 4096))
            {
                return true;
            }
        }
        else
        {
            if((PageNo * 512) >= Offset && (PageNo * 512) < (Offset + 16384))
            {
                return true;
            }
        }
    }
    else if(Fe2pMode == FE2P_MODE)
    {
        for(i = 0; i < 1024; i += 128)
        {
            if(PageNo > 119 + i && PageNo < 128 + i)
            {
                return true;
            }
        }
    }

    return false;
}

int32_t ChipNt68411::T_WritePagedataWithReply(uint8_t Pg, uint8_t CurBuf[])
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x00;
    uint8_t pu1WData[521] = {0x00};//len=512+9
    uint8_t pu1RData[3] = {0};
    uint8_t ReplyCode = 0;
    uint16_t i = 0;

    pu1WData[0] = CMD_WRITE_PAGE_24M;
    pu1WData[1] = Pg;
    pu1WData[2] = 0xf0;
    pu1WData[3] = 0x0f;
    pu1WData[4] = 0x6e;
    pu1WData[5] = 0x91;
    pu1WData[6] = 0x13;
    pu1WData[7] = 0xec;

    pu1WData[520] = (mI2cAddr*2) ^ u1Addr;

    for(i = 0; i < 520; i++)
    {
        if(i >= 8)
        {
            pu1WData[i] = CurBuf[i - 8];
        }
        pu1WData[520] ^= pu1WData[i];
    }

    //DEBUG_XBH("---> T_WritePagedataWithReply Pg=%d\n", Pg);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);

    T_HandShake(50);

    if(s32Ret == 0)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == 0)
        {
            ReplyCode = pu1RData[1];
            if((ReplyCode ^ CMD_WRITE_PAGE_24M) != 0xff)
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_Program(uint8_t Pg, uint8_t CurBuf[])
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = T_WritePagedataWithReply(Pg, CurBuf);
    return s32Ret;
}

int32_t ChipNt68411::T_GetCheckSumWithReply(uint8_t PageNo)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x02;
    uint8_t pu1WData[3] = {0x00};
    uint8_t pu1RData[5] = {0};
    uint8_t ReplyCode = 0;
    uint8_t ChkH = 0;
    uint8_t ChkL = 0;

    pu1WData[0] = CMD_GET_CHECK_SUM;
    pu1WData[1] = PageNo;
    pu1WData[2] = (mI2cAddr*2) ^ u1Addr ^ CMD_GET_CHECK_SUM ^ PageNo;

    //DEBUG_XBH("---> T_GetCheckSumWithReply PageNo=%d\n", PageNo);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);

    T_HandShake(50);

    if(s32Ret == 0)
    {
        u1Addr = 0x6e;

        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == 0)
        {
            ReplyCode = pu1RData[1];
            ChkH = pu1RData[2];
            ChkL = pu1RData[3];
            if((ReplyCode ^ CMD_GET_CHECK_SUM) != 0xff
                    || SPageSum != ((uint16_t)ChkH * 0x100 + ChkL))
            {
                XLOGD("Error! ReplyCode = 0x%x\n", ReplyCode);
                XLOGD("Error! ChkH = 0x%x\n", ChkH);
                XLOGD("Error! ChkL = 0x%x\n", ChkL);
                XLOGD("Error! SPageSum = 0x%x\n", SPageSum);
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::Cmd_T_GetCheckSum(uint8_t Pg)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = T_GetCheckSumWithReply(Pg);
    return s32Ret;
}

int32_t ChipNt68411::FnUpdateIDSector(uint8_t buf[])
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t Data[512];
    uint16_t Checksum = 0;
    uint16_t i;
    uint16_t IDPageNo;

    IDPageNo = (0xEFF0 / 512); // make ID string address as page unit.

    //backup bin buffer then update data
    memcpy(&Data, &buf[ONE_PAGE_SIZE * IDPageNo], ONE_PAGE_SIZE);
    memset(&buf[ONE_PAGE_SIZE * IDPageNo], 0xFF, ONE_PAGE_SIZE);

    // copy ID to the buffer
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F0] = 'D';
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F1] = 'U';
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F2] = 'A';
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F3] = 'L';
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F4] = 'F';
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F5] = 'W';

    // set flash bank number
    buf[ONE_PAGE_SIZE * IDPageNo + 0x1F6] = Flash.Page / 128;

    // calculate checksum
    for(i = 0; i < 512; i++)
    {
        Checksum += buf[ONE_PAGE_SIZE * IDPageNo + i];
    }
    SPageSum = Checksum;

    Fn68390ProgramEnable(true);
    //Cmd_T_SetExntend(IDPageNo);
    s32Ret = Cmd_T_SetExntend(0);
    if(s32Ret == XBH_SUCCESS)
    {
        s32Ret = Cmd_T_Program(IDPageNo, &buf[ONE_PAGE_SIZE * IDPageNo]);
        if(s32Ret != XBH_SUCCESS)
        {
            Fn68390ProgramEnable(false);
            // restore buffer
            memcpy(&buf[ONE_PAGE_SIZE * IDPageNo], &Data, ONE_PAGE_SIZE);
            return s32Ret;
        }

        s32Ret = Cmd_T_GetCheckSum(IDPageNo);
        if(s32Ret != XBH_SUCCESS)
        {
            Fn68390ProgramEnable(false);
            // restore buffer
            memcpy(&buf[ONE_PAGE_SIZE * IDPageNo], &Data, ONE_PAGE_SIZE);
            return s32Ret;
        }
    }
    Fn68390ProgramEnable(false);

    // restore buffer
    memcpy(&buf[ONE_PAGE_SIZE * IDPageNo], &Data, ONE_PAGE_SIZE);
    return s32Ret;
}

uint32_t ChipNt68411::HalNt68411GetDivisionType()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint32_t u32CusDualMode = 0;
    XBH_S32 fd = -1;
    XBH_CHAR u32PanelIndex[2]= {0};
    fd = open(PANEL_DIV_SEC, O_RDWR | O_NONBLOCK);
    if (fd < 0)
    {
        XLOGE("setPmux : open %s failed !!!!", PANEL_DIV_SEC);
        return XBH_FAILURE;
    }
    s32Ret = read(fd, u32PanelIndex, 1);
    if (s32Ret < 0) {
        XLOGE("getPanel : read from %s failed !!!!", PANEL_DIV_SEC);
        s32Ret = XBH_FAILURE;
    }
    else
    {
        s32Ret = XBH_SUCCESS;
    }

    if(s32Ret == XBH_SUCCESS && u32PanelIndex[0] == '2')
    {
        u32CusDualMode = 1;
    }
    return u32CusDualMode;
}

int32_t ChipNt68411::T_GetNt68411DivisionMode(uint32_t *pu32DivisionMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int32_t i = 0;
    uint8_t u1Addr = 0x51;
    uint8_t pu1WData[4] = {0x82, 0x01, 0xa2, 0x1e};
    uint8_t pu1RData[11] = {0};
    uint8_t cks = 0;

    //DEBUG_XBH("---> T_GetNt68411DivisionMode\n");

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);

    T_HandShake(50);

    if(s32Ret == 0)
    {
        u1Addr = 0x6e;
        s32Ret |= HalNt68411ReadI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1RData), pu1RData);
        if(s32Ret == 0)
        {
            cks = 0x50;
            for(i = 0; i < 10; i++)
            {
                //XLOGD("pu1RData[%d]=0x%x\n", i, pu1RData[i]);
                cks ^= pu1RData[i];
            }
            //DEBUG_XBH("cks = 0x%x, pu1RData[10]=0x%x\n", cks, pu1RData[10]);
            if(cks == pu1RData[10])
            {
                *pu32DivisionMode = pu1RData[9];
                XLOGD("T_GetNt68411DivisionMode = 0x%x\n", *pu32DivisionMode);
            }
            else
            {
                return XBH_FAILURE;
            }
        }
    }

    return s32Ret;
}

int32_t ChipNt68411::T_SetNt68411DivisionMode(uint32_t u32DivisionMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    uint8_t u1Addr = 0x50;
    uint8_t pu1WData[6] = {0x00};

    pu1WData[0] = 0x84;
    pu1WData[1] = 0x03;
    pu1WData[2] = 0xfe;
    pu1WData[3] = 0x00;
    pu1WData[4] = (u32DivisionMode == 1 ? 1 : 0);
    pu1WData[5] = (mI2cAddr*2) ^ u1Addr ^ pu1WData[0] ^ pu1WData[1] ^ pu1WData[2] ^ pu1WData[3] ^ pu1WData[4];

    //DEBUG_XBH("---> T_SetNt68411DivisionMode=%d\n", u32DivisionMode);

    s32Ret |= HalNt68411WriteI2c(mI2cNum, mI2cAddr, u1Addr, sizeof(pu1WData), pu1WData);
    return s32Ret;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 ChipNt68411::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    if(mState == XBH_UPGRADE_SUCCESS)
    {
        XLOGW("ChipNt68411 upgrade success");
    }
    return s32Ret;
}

XBH_S32 ChipNt68411::upgradeFirmware(const XBH_CHAR* strFileName, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if(mState == XBH_UPGRADE_RUNNING)
    {
        XLOGW("Warning: nt68411 is updating...");
        return XBH_SUCCESS;
    }

    if(strFileName == NULL)
    {
        XLOGE("Error: fileName is NULL");
        return XBH_FAILURE;
    }
    XLOGD("strFileName : %s\r\n", strFileName);

    strcpy(mFileName, strFileName);
    XLOGD("bForceUpgrade : %d\r\n", bForceUpgrade);
    mForceUpgrade = bForceUpgrade;
    mForceMode = 1;
    XbhMWThread::run(XbhMWThread::ONCE);
    return s32Ret;
}

void ChipNt68411::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    mState = XBH_UPGRADE_RUNNING;

    uint8_t i;
    uint8_t j;
    uint16_t systemversion = 0x00;
    uint16_t firmwareversion = 0x00;
    uint16_t page = 0;
    uint16_t offset = 0;
    uint16_t skip = 0;
    uint8_t p_last = 0;
    uint8_t retry = 5;
    uint8_t buftmp[ONE_PAGE_SIZE] = {0};
    uint8_t *buf = NULL;
    uint16_t total_page = 0;
    uint32_t verindex = 0;
    XBH_U32 tmpversion[3] = {0xffff};
    XBH_U32 success_flag = 0;

    FILE *fp = NULL;
    uint32_t filesize = 0;

    fp = fopen(mFileName, "rb");
    if(NULL == fp)
    {
        XLOGD("Error: Open %s file fail!\n", mFileName);
        mState = XBH_UPGRADE_FAILURE;
        return;
    }

    buf = (uint8_t*)malloc(TOTAL_PAGE_SIZE * ONE_PAGE_SIZE);
    memset(buf, 0xff, TOTAL_PAGE_SIZE * ONE_PAGE_SIZE);
    //get file size
    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    XLOGD("filesize = %d\n", filesize);
    rewind(fp);
    fread(buf, sizeof(char), filesize/sizeof(char), fp);

    total_page = filesize/ONE_PAGE_SIZE + (filesize%ONE_PAGE_SIZE ? 1 : 0);

    Fe2pMode = NORMAL_MODE;

    for(i = 0; i < 5; i++)
    {
        s32Ret = XBH_SUCCESS;
        for(j = 0; j < 3; j++)
        {
            s32Ret |= T_GetNtVersion(&tmpversion[j]);
            T_HandShake(10);
        }
        //confirm the 68411 version from chip
        if((s32Ret == XBH_SUCCESS)
                && (tmpversion[0] != 0xffff)
                && (tmpversion[0] == tmpversion[1])
                && (tmpversion[0] == tmpversion[2]))
        {
            if(Nt68411VersionSupport(tmpversion[0]) == 0)
            {
                //The firmware version support to upgrade 68411.
                systemversion = tmpversion[0];
                break;
            }
        }
        T_HandShake(1000);
    }

    // get firmware version value
    for(verindex = 0x50000; verindex < 0x50500; verindex++)
    {
        if(buf[verindex] == 0xa5 && buf[verindex + 1] == 0xa5 && buf[verindex + 2] == 0x20)
        {
            firmwareversion = (((uint16_t)buf[verindex+11] << 8) | buf[verindex+12]);
            if(firmwareversion == 0x0)
            {
                firmwareversion = (((uint16_t)buf[verindex+13] << 8) | buf[verindex+14]);
            }
            break;
        }
    }
    XLOGD("firmwareversion = 0x%x\n", firmwareversion);
    mForceUpgrade = XBH_TRUE;

    if(Nt68411VersionSupport(firmwareversion) != 0 || Nt68411VersionSupport(systemversion) != 0)
    {
        if(mForceUpgrade)
        {
            XLOGD("firmware version not in whitelist[23.16, 29.27, 4a.17]. Force upgrade.\n");
        }
        else
        {
            XLOGD("firmware version not in whitelist[23.16, 29.27, 4a.17]. Ignore upgrade.\n");
            mState = XBH_UPGRADE_FAILURE;
            return;
        }
    }

    if(firmwareversion == systemversion && firmwareversion != 0x00)
    {
        if(mForceUpgrade)
        {
            XLOGD("firmware is same. Force upgrade.\n");
        }
        else
        {
            XLOGD("firmware is same. No need to upgrade.\n");
            mState = XBH_UPGRADE_SUCCESS;
            return;
        }
    }

    if(systemversion == 0x00)
    {
        if(mForceUpgrade)
        {
            XLOGD("May be flash is empty. Force uprade.\n");
        }
        else
        {
            XLOGD("May be flash is empty. Ignore upgrade.\n");
            mState = XBH_UPGRADE_FAILURE;
            return;
        }
    }

    #if 0 // Remove dual fw mode.
    if(systemversion < 0x0A00)
    {
        DualFwEnable = 0;
        XLOGD("The 68411 version do not support Dual Firmware\n");
        mState = XBH_UPGRADE_FAILURE;
        return;
    }
    #endif
    DualFwEnable = (mForceMode == 0 ? 0 : 1);
    XLOGD("DualFwEnable = %d\n", DualFwEnable);
    if(DualFwEnable == 1 && systemversion < 0x0A00)
    {
        XLOGD("The 68411 version do not support Dual Firmware\n");
        mState = XBH_UPGRADE_FAILURE;
        return;
    }

    // enter ISP mode
    for(i = 0; i < 5; i++)
    {
        T_ISPMode(1);
        PulseDelay(1);
        T_ISPMode(1);
        T_HandShake(1);
        s32Ret = T_ISPStatusReply();
        if(s32Ret == XBH_SUCCESS)
        {
            break;
        }
    }
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("Cmd_T_EnterIsp fail\n");
        goto end;
    }

    //start update after enter ISP mode
    percentage = 1;

    //send special command
    for(i = 0; i < 5; i++)
    {
        XLOGD("nt68411 start sending spec cmd ...\n");
        s32Ret = Cmd_T_SendSpecialCommand();
        if(s32Ret == XBH_SUCCESS)
        {
            break;
        }
        T_HandShake(5);
    }
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("Cmd_T_SendSpecialCommand fail\n");
        percentage = 0;
        goto end;
    }


    for(i = 0; i < 5; i++)
    {
        XLOGD("nt68411 start geting flash id ...\n");
        s32Ret = Cmd_T_GetFlashID();
        if(s32Ret == XBH_SUCCESS)
        {
            break;
        }
        T_HandShake(5);
    }
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("Cmd_T_GetFlashID fail\n");
        percentage = 0;
        goto end;
    }


    for(i = 0; i < 5; i++)
    {
        XLOGD("nt68411 wpenable ...\n");
        s32Ret = WpEnable();
        if(s32Ret == XBH_SUCCESS)
        {
            break;
        }
        T_HandShake(5);
    }
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("WpEnable fail\n");
        percentage = 0;
        goto end;
    }


    for(i = 0; i < 5; i++)
    {
        XLOGD("nt68411 disable flash protect ...\n");
        s32Ret = Cmd_T_BlockProtect(0);
        if(s32Ret == XBH_SUCCESS)
        {
            break;
        }
        T_HandShake(5);
    }
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("WpEnable fail\n");
        percentage = 0;
        goto end;
    }

    if(DualFwEnable)
    {
        for(i = 0; i < 5; i++)
        {
            XLOGD("nt68411 FnEraseIDSector ...\n");
            s32Ret = FnEraseIDSector();
            if(s32Ret == XBH_SUCCESS)
            {
                break;
            }
            T_HandShake(5);
        }
        if(s32Ret != XBH_SUCCESS)
        {
            XLOGD("FnEraseIDSector fail\n");
            percentage = 0;
            goto end;
        }
    }

    if(McuId == 0xa390 || McuId == 0xA400 || McuId == 0xA450)
    {
        s32Ret = Fn68390ProgramEnable(true);
    }

    XLOGD("nt68411 Cmd_T_Erase ...\n");
    s32Ret = Cmd_T_Erase();
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGD("Cmd_T_Erase fail\n");
        percentage = 0;
        goto end;
    }

    if(DualFwEnable)
    {
        CodePages = MaxPages / 2;
        FlashOffset = MaxPages / 2;
    }
    else
    {
        CodePages = MaxPages;
        FlashOffset = 0;
    }

    for(page = FlashOffset; page < (CodePages + FlashOffset); page++)	// 1024pages for 4M flash
    {
        for(i = 0; i < 5; i++)
        {
            s32Ret = Cmd_T_SetExntend(page);
            if(s32Ret == XBH_SUCCESS)
            {
                break;
            }
            T_HandShake(5);
        }
        if(s32Ret != XBH_SUCCESS)
        {
            goto end;
        }

        skip = 1;
        SPageSum = 0;
        memcpy(&buftmp, &buf[ONE_PAGE_SIZE * (page - FlashOffset)], ONE_PAGE_SIZE);
        for(offset = 0; offset < ONE_PAGE_SIZE; offset++)
        {
            if(buftmp[offset] != 0xff)
            {
                skip = 0;
            }
            SPageSum += buftmp[offset];
        }

        if(CheckOverLap(page))
        {
            skip = 1;
        }

        //XLOGD("%s-page=%d,skip=%d\n", __func__,page,skip);
        percentage = (page - FlashOffset) * 100 / total_page;
        if(((page - FlashOffset) < total_page) && percentage && !(percentage % 10) && (p_last != percentage))
        {
            p_last = percentage;
            //XLOGD("nt68411 upgrading %d%%.\n", percentage);
        }

        s32Ret = XBH_SUCCESS;
        if(!skip){
#ifdef UPDATE_LED_GPIO
            led_flag = ~ led_flag;
            gpio_direction_output(nti2c.update_led, led_flag);
#endif
            s32Ret |= Cmd_T_Program(page & 0xff, buftmp);
        }

        if(!skip)   //Bypass compare?
        {
            s32Ret |= Cmd_T_GetCheckSum(page & 0xff);
        }

        //start add by xbh lgh 20180814
        if(s32Ret != XBH_SUCCESS && retry--)
        {
            XLOGD("nt68411 upgrade page %d retry %d.\n", page, 5 - retry);
            page -= 1;
            T_HandShake(5);
            continue;
        }
        //end add by xbh lgh 20180814

        if(s32Ret != XBH_SUCCESS)
            goto end;

        retry = 5;
        //T_HandShake(1);
    }
    percentage = 100;//update success.
    XLOGD("nt68411 upgrade success.\n");

    if(McuId == 0xa390 || McuId == 0xA400 || McuId == 0xA450)
    {
        Fn68390ProgramEnable(false);
    }

    if(DualFwEnable)
    {
        for(i = 0; i < 5; i++)
        {
            s32Ret = FnUpdateIDSector(buf);
            if(s32Ret == XBH_SUCCESS)
            {
                break;
            }
            T_HandShake(5);
        }
        if(s32Ret != XBH_SUCCESS)
            goto end;
    }
    XLOGD("nt68411 upgrade end.\n");
    success_flag = 1;
end:
    if(buf != NULL)
    {
        free(buf);
        buf = NULL;
    }
    //exit ISP mode
    s32Ret = T_ISPMode(0);

    //confirm the division mode of 68411
    XLOGD("HalNt68411GetDivisionType = %d.\n", HalNt68411GetDivisionType());
    if(HalNt68411GetDivisionType() == 1)
    {
        //Get 68411 division to confirm the 68411 running status
        uint32_t u32Nt68411DivisionMode = 0;
        for(i = 0; i < 20; i++)
        {
            s32Ret = T_GetNt68411DivisionMode(&u32Nt68411DivisionMode);
            if(s32Ret == XBH_SUCCESS)
            {
                break;
            }
            T_HandShake(100);
        }

        XLOGD("delay 3s wait nt68411 init.\n");
        //等待nt68411初始化
        T_HandShake(3000);

        XLOGD("nt68411 try to change dual mode to 1.\n");
        for(i = 0; i < 3; i++)
        {
            s32Ret = T_SetNt68411DivisionMode(0x01);//set division to dual mode
            T_HandShake(100);
        }
    }
    setHdmiTxColorSpace(Nt68411_HDMI_TX_YUV444);
    if(success_flag)
    {
        mState = XBH_UPGRADE_SUCCESS;
    }
    else
    {
        mState = XBH_UPGRADE_FAILURE;
    }

    return;
}
//---------------- custom device interface end ----------------

ChipNt68411::ChipNt68411(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD("begin ");

    property_set("persist.vendor.xbh.nt68411", "true");

    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    XLOGD(" end ");
}

ChipNt68411::~ChipNt68411()
{

}