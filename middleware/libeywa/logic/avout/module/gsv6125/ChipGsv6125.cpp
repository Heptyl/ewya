#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipGsv6125"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hardware/board.h>
#include "XbhLog.h"
#include "XbhService.h"
#include "ChipGsv6125.h"
#include <cutils/properties.h>
#include "XbhSysOpt.h"
#include "XbhPartitionData.h"

XbhMutex                        ChipGsv6125::mLock;


ChipGsv6125::ChipGsv6125(XBH_U32 i2cNumber, XBH_U32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel)
{
    this->mI2cNumber = i2cNumber;
    this->mI2cAddress = i2cAddress;
    this->mPowerGpio = powerGpio;
    this->mPowerLevel = powerLevel;
    this->mResetGpio = resetGpio;
    this->mResetLevel = resetLevel;

    if(mPowerGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mPowerGpio, mPowerLevel);
        usleep(100*1000);
    }

    //上电之后复位之前加入启动保护程序
    powerUpProtection();

    if(mResetGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
        usleep(100 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(50 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
        usleep(100 * 1000);
    }

    XLOGD(" end ");
}

ChipGsv6125::~ChipGsv6125()
{
    XLOGD("~ end");
}

XBH_S32 ChipGsv6125::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XbhMutex::Autolock _l(mLock);
    XLOGD("upgradeFirmware  strFilePath %s",strFilePath);
    property_set(GSV6125_VERSION, "000000");
    forceUpgrade = true;
    mDevType = 0x6125;
    XLOGD ("[%s:%d] dev type = 0x%04x, i2c bus = %d", __func__, __LINE__, mDevType, mI2cAddress);

    // 初始化更新状态
    // upgradeState = GSVUpgrade_IDLE;

    UpdConfigID[0] = 0x29;
    UpdConfigID[1] = 0x81;
    UpdConfigID[2] = 0x56;
    UpdConfigID[3] = 0x41;

    UpdReplyID[0] = 0x97;
    UpdReplyID[1] = 0x86;
    UpdReplyID[2] = 0x69;
    UpdReplyID[3] = 0x51;

    // 申请空间
    partition_data = (XBH_U8 *)malloc(252 * 1024);
    if (partition_data == NULL)
    {
        XLOGE("[%s:%d] malloc fiailed", __func__, __LINE__);
    }
    strcpy (file_name, strFilePath);
    XbhMWThread::run(XbhMWThread::ONCE);
    XBH_S32 s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 ChipGsv6125::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[] = {0x00, 0x00};
    //增加容错机制
    XBH_U8 u8Reset_time = 0;
RESET:
    if(u8Reset_time == 3)
    {
        XLOGD("getFirmwareVersion  fail !!!");
        return XBH_FAILURE;
    }
    u8Reset_time++;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_FW_VER, 2 ,2 ,data);
    if(s32Ret == XBH_FAILURE || (data[0] == 0 && data[1] == 0))
    {
        usleep(500*1000);
        goto RESET;
    }
    sprintf(strVersion, "%02X%02X", data[0], data[1]);
    XLOGD("getFirmwareVersion  =  %s",strVersion);
    property_set("persist.vendor.xbh.gsv6125.ver", strVersion);
    return s32Ret;
}

XBH_U8 ChipGsv6125::calcCRC8(const XBH_U8 *data, XBH_U32 len)
{
    XBH_U8 crc8 = 0;
    XBH_U16 i = 0;
    while (len --)
    {
        crc8 = crc8 ^ data[i];
        crc8 = crc8Gsv6125Table[crc8];
        i++;
    }
    return crc8;
}

XBH_S32 ChipGsv6125::writeData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth)
{
    XBH_S32 ret = XBH_FAILURE;
    // XLOGV("[%s:%d] reg = 0x%02x", __func__, __LINE__, reg_addr);
    XBH_S32 timeout = 0;
    while ((ret != XBH_SUCCESS) && (timeout < 3))
    {
        ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, XBH_GSV6125_DEVICE_ADDRESS, reg_addr, 2, data_lenth, p_data);
        if (ret != XBH_SUCCESS)
        {
            XLOGE("[%s:%d] error: %d", __func__, __LINE__, ret);
        }
        timeout++;
    }
    return ret;
}

XBH_S32 ChipGsv6125::readData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth)
{
    XBH_S32 ret = XBH_FAILURE;
    // XLOGV("[%s:%d] reg = 0x%08x", __func__, __LINE__, reg_addr);
    XBH_S32 timeout = 0;
    while ((ret != XBH_SUCCESS) && (timeout < 3))
    {
        ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, reg_addr, 2, data_lenth, p_data);
        if (ret != XBH_SUCCESS)
        {
            XLOGE("[%s:%d] error: %d", __func__, __LINE__, ret);
        }
        timeout++;
    }
    return ret;
}

XBH_S32 ChipGsv6125::getUpgradeState(XBH_S32 *pstat)
{
    *pstat = upgradeState;
    XLOGD("[%s:%d] status:%d", __func__, __LINE__, upgradeState);
    return (upgradeState == GSV6125Upgrade_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
}

void ChipGsv6125::reset(void)
{
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
    usleep(50*1000);
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
    usleep(10*1000);
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
    usleep(50*1000);
}

XBH_S32 ChipGsv6125::writeRXDDCUpdConfigID(void)
{
    XBH_U16 i;
    XBH_U8 tmp[4];
    //write
    writeData(XBH_GSV6125_RXDDC_UPDCONFIGID, (XBH_U8 *)UpdConfigID, 4);
    //wait reply
    for(i = 0; i < 100; i++)
    {
        usleep (20 * 1000);
        readData(XBH_GSV6125_RXDDC_UPDREPLYID, tmp, 4);
        XLOGD("writeRXDDCUpdConfigID Reply: 0x%02x%02x%02x%02x", tmp[0], tmp[1], tmp[2], tmp[3]);
        if((tmp[0] == UpdReplyID[0]) && (tmp[1] == UpdReplyID[1]) && (tmp[2] == UpdReplyID[2]))
        {
            if(tmp[3] == UpdReplyID[3])
            {
                McuRunAtBTag = 0;
                return 1;
            } 
            else if(tmp[3] == (UpdReplyID[3] + 1))
            {
                McuRunAtBTag = 1;
                return 1;
            }
        }
        else
        {
            usleep (20 * 1000);
            readData(XBH_GSV6125_RXDDC_UPDCONFIGID, tmp, 4);
            XLOGD("please check FF27 if write success : 0x%02x%02x%02x%02x", tmp[0], tmp[1], tmp[2], tmp[3]);
        }
    }
    //time out return
    return -1;
}

XBH_S8 ChipGsv6125::writeRXDDCUpdInfo(XBH_U32 appSize, XBH_U32 appAddr)
{
    GSV6125UpgradeInfo Info;
    Info.DataTotalLen = appSize;
    Info.DataOffsetAtFlash = appAddr;
    Info.VersionMajor = XBH_GSV6125_APP_VERSION_MAJOR;
    Info.VersionMinor = XBH_GSV6125_APP_VERSION_MINOR;
    Info.GsvChipNumber = mDevType;
    //checksum
    XBH_U8 dataSum = calcCRC8((XBH_U8 *)&Info, sizeof(GSV6125UpgradeInfo));
    //write
    writeData(XBH_GSV6125_RXDDC_UPDINFO, (XBH_U8 *)&Info, sizeof(GSV6125UpgradeInfo));
    writeData(XBH_GSV6125_RXDDC_INFOCHECKSUM, &dataSum, 1);
    //
    return 0;
}

XBH_S8 ChipGsv6125::writeRXDDCUpdData(XBH_U8 *pbuf)
{
    XBH_U8 dataSum = 0;
    dataSum = calcCRC8(pbuf, XBH_TRANSFER_DATA_BLKSIZE);
    writeData(XBH_GSV6125_RXDDC_UPDDATA, pbuf, XBH_TRANSFER_DATA_BLKSIZE);
    writeData(XBH_GSV6125_RXDDC_DATACHECKSUM, &dataSum, 1);
    //
    return 0;
}

XBH_S8 ChipGsv6125::waitMsgAckNew(void)
{
    XBH_U16 i;
    XBH_U8 msgAck = 0;
    for(i = 0; i < 5000; i++) {
        usleep (1000);
        readData(XBH_GSV6125_RXDDC_REPLY, &msgAck, 1);
        if((msgAck != 0) && ((msgAck & 0xf0) == (RXDDCCmd & 0xf0)))
            break;
    }
    if(i >= 5000)
        return -1;
    else {
        return 0;
    }
}

XBH_S8 ChipGsv6125::msgCmdAndWaitNew(void)
{
    writeData(XBH_GSV6125_RXDDC_COMMAND, &RXDDCCmd, 1);
    if(waitMsgAckNew() < 0)
        return -1;
    else {
        RXDDCCmd += XBH_GSV6125_ROLL_BACK_INCDATA;
        return 0;
    }
}

XBH_S8 ChipGsv6125::appI2CUpdateInit(void)
{
    RXDDCCmd = 0;
    // writeData(XBH_GSV6125_RXDDC_REBOOT, &RXDDCCmd, 1);	//need clear
    XbhService::getModuleInterface()->setI2cData(mI2cNumber, XBH_GSV6125_DEVICE_ADDRESS,XBH_GSV6125_RXDDC_REBOOT, 2, 1,&RXDDCCmd);
    if(writeRXDDCUpdConfigID() < 0)
        return -1;
    else
        return McuRunAtBTag;
}

XBH_S8 ChipGsv6125::appI2CUpdateServer(XBH_U8 * appData, XBH_U32 appSize, XBH_U32 appAddr)
{
    XBH_U32 i = 0;
app_start:
    if (i >= 3) {
        XLOGE("[%s:%d] -- upgrade fail over 3 times --", __func__, __LINE__);
        return 0;
    }
    XBH_U32 point = -1;
    RXDDCDataPoint = 0;
    //round to 256x for FLASH Page Program
    appSize = ((appSize + 255) >> 8) * 256;
    //
    XLOGD("[%s:%d] size: 0x%04x, addr: 0x%04x", __func__, __LINE__, appSize, appAddr);
    writeRXDDCUpdInfo(appSize, appAddr);
    //fisrt step
    RXDDCCmd = XBH_GSV6125_RXDDC_INFO_CMD;
    if(msgCmdAndWaitNew() < 0){
        XLOGE("[%s:%d] RXDDCCmd: 0x%04x", __func__, __LINE__, RXDDCCmd);
        i++;
        goto app_start;
    }

    XLOGD("[%s:%d] Start!", __func__, __LINE__);
    //start
    while(1)
    {
        //Data send
        if (point != (RXDDCDataPoint * 100 / appSize))
        {
            point = RXDDCDataPoint * 100 / appSize;
            XLOGD("[%s:%d] --%3d%%", __func__, __LINE__, point);
        }
        if(RXDDCDataPoint >= appSize)
        {
            XLOGD("[%s:%d] --100%%", __func__, __LINE__);
            //End
            RXDDCCmd = XBH_GSV6125_TRANSFER_END;
            writeData(XBH_GSV6125_RXDDC_UPDCONFIGID, &RXDDCCmd, 1); //write other value to exit loop
            writeData(XBH_GSV6125_RXDDC_REBOOT, &RXDDCCmd, 1);
            break;
        } 
        else
        {
            writeRXDDCUpdData(appData + RXDDCDataPoint);
            RXDDCDataPoint += XBH_TRANSFER_DATA_BLKSIZE;
            //next
            RXDDCCmd &= 0xf0;
            RXDDCCmd += XBH_GSV6125_RXDDC_DATA_CMD;
            if(msgCmdAndWaitNew() < 0)
            {
                XLOGE("[%s:%d] -- fail --", __func__, __LINE__);
                XLOGE("[%s:%d] -- Retry --", __func__, __LINE__);
                i++;
                goto app_start;
            }
        }
    }
    XLOGD("[%s:%d] Over!", __func__, __LINE__);
    return 1;
}


/**
    请将这段代码添加到SoC对GSV芯片控制上电时硬复位操作之前.
    其作用在于在恶劣的供电和PCB环境下,保证GSV芯片的正常启动,保证芯片量产良率.
*/
XBH_S32 ChipGsv6125::powerUpProtection()
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_U8 loop = 0;
    XBH_U8 temp = 0;
    XBH_U8 GsvTable[] =
    {
        0x00, 0x00, 0x00,
        0x00, 0x9C, 0x08,
        0x00, 0xB4, 0x0F,
        0x00, 0xB5, 0x0F,
        0x00, 0xB6, 0x0F,
        0x00, 0xB8, 0x0F,
        0x00, 0xB7, 0x0F,
        0x05, 0x34, 0x9E,
        0x05, 0x64, 0x9E,
        0x05, 0x94, 0x9E,
        0x05, 0xB4, 0x9E,
        0x05, 0xE4, 0x9E,
        0x05, 0x54, 0x9E,
        0x08, 0x53, 0x02,
        0x00, 0xA2, 0xFF,
        0x00, 0xA6, 0xFF,
        0x00, 0x01, 0x89,
        0x00, 0x02, 0x00,
        0x05, 0x31, 0x7E,
        0x05, 0x51, 0x7E,
        0x05, 0x61, 0x7E,
        0x05, 0x91, 0x7E,
        0x05, 0xB1, 0x7E,
        0x05, 0xE1, 0x7E,
        0xFE, 0xFE, 0xFE
    };
    /* Write GSV_DEVICE_ADDR.0x00FF = 0xFF */
    temp = 0xFF;
    //ManI2cWrite(GSV_DEVICE_ADDR, 0x00FF, &temp, 1, 0, 1);
    ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, 0X00FF, 2, 1, &temp);
    /* Read back GSV_DEVICE_ADDR.0xFFFF */
    //ManI2cRead (GSV_DEVICE_ADDR, 0xFFFF, &temp, 1, 0, 1);
    ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, 0XFFFF, 2, 1, &temp);
    for(loop=0; GsvTable[loop]!=0xFE; loop=loop+3)
    {
        //ManI2cWrite(GSV_DEVICE_ADDR, ((GsvTable[loop]<<8)+GsvTable[loop+1]), &GsvTable[loop+2], 1, 0, 1);
        ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, ((GsvTable[loop]<<8)+GsvTable[loop+1]), 2, 1, &GsvTable[loop+2]);
    }

    return ret;
}

void ChipGsv6125::run(const void* arg)
{
    XLOGI("[%s:%d]  file name: %s", __func__, __LINE__, file_name);
    XBH_S32 fd = 0;
    XBH_S32 ret = 0;
    XBH_S32 read_num = 0;
    XBH_CHAR* FWVer = (char *)malloc(10);

    RXDDCCmd = 0;
    McuRunAtBTag = -1;
    RXDDCDataPoint = 0;

    upgradeState = GSV6125Upgrade_RUNNING;
    property_set("persist.vendor.xbh.gsv6125.ver", "");

    if (appI2CUpdateInit() == 1)
    {
        XLOGD ("[%s:%d] mcu run at partition B, upgrade partition A", __func__, __LINE__);
        patition_start_addr = XBH_GSV6125_PARTITION_A_ADDR; //4k
    }
    else if (appI2CUpdateInit() == 0)
    {
        XLOGD ("[%s:%d] mcu run at partition A, upgrade partition B", __func__, __LINE__);
        patition_start_addr = XBH_GSV6125_PARTITION_B_ADDR; //256k
    }
    else
    {
        XLOGD ("[%s:%d] Error: Can`t Start Upgrade!!!", __func__, __LINE__);
        upgradeState = GSV6125Upgrade_FAILURE;
        goto end;
    }

    fd = open(file_name, O_RDONLY);
    if (fd <= 0)
    {
        XLOGE("[%s:%d] open Error: ", __func__, __LINE__);
        upgradeState = GSV6125Upgrade_FAILURE;
        goto end;
    }

    if (lseek (fd, patition_start_addr, SEEK_SET) < 0)
    {
        XLOGE("[%s:%d] lseek Error: ", __func__, __LINE__);
        close (fd);
        upgradeState = GSV6125Upgrade_FAILURE;
        goto end;
    }

    XLOGD ("[%s:%d] read file...", __func__,  __LINE__);
    while (read_num < (252 * 1024))
    {
        ret = read (fd, partition_data + read_num, 64);
        if (ret < 0)
        {
            XLOGE("[%s:%d] read partition Error!!!", __func__, __LINE__);
            close (fd);
            upgradeState = GSV6125Upgrade_FAILURE;
            goto end;
        }
        read_num +=  ret;
    }
    XLOGD ("[%s:%d] read file done", __func__,  __LINE__);
    close (fd);

    if ((partition_data[0x8e]  != dataGsv6125Buff[0]) || (partition_data[0x8f]  != dataGsv6125Buff[1]) || forceUpgrade)
    {
        upgradeState = GSV6125Upgrade_RUNNING;
        if (appI2CUpdateServer(partition_data, 252 * 1024, patition_start_addr))
        {
            upgradeState = GSV6125Upgrade_SUCCESS;
            usleep(2*1000*1000);
            reset();
            usleep(2*1000*1000);
            getFirmwareVersion(FWVer);
        }
        else
        {
            upgradeState = GSV6125Upgrade_FAILURE;
        }
    }
end:
    if (partition_data != NULL)
    {
        // 释放空间
        free (partition_data);
    }
}


