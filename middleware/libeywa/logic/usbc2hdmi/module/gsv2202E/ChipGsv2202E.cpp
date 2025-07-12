#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipGsv2202E"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhService.h"
#include "ChipGsv2202E.h"
#include <cutils/properties.h>
#include "XbhSysOpt.h"

XbhMutex                        ChipGsv2202E::mLock;

ChipGsv2202E::ChipGsv2202E(XBH_S32 i2cNumber, XBH_S32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel)
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

    if(mResetGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(100 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
        usleep(50 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(100 * 1000);
    }

    #if (TYPE_BOARD == XMA311D2T2)
    {
       XBH_S32 ret = XBH_FAILURE;
       usleep(3000 * 1000); //T2项目的2202e子固件在硬复位后，需要延时大概3s后才能读到正确的版本信息
       ret = setNormalMode();
       if(ret == XBH_SUCCESS)
       {
           XLOGD("setNormalMode: success ");
       }else
       {
           XLOGD("setNormalMode: fail ");
       }
    }
    #endif
    
    XLOGD(" end ");
}

ChipGsv2202E::~ChipGsv2202E()
{
    XLOGD("~ end");
}

XBH_S32 ChipGsv2202E::setPowerChip(XBH_BOOL onoff)
{
    XLOGD("setPowerChip: onoff = %d",onoff);
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipGsv2202E::setActivePort(XBH_U8 port)
{
    XLOGD("setActivePort: port = %d",port);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data;
    switch (port)
    {
        case 0:
            data = GSV2202E_PORT_RX_A;
            break;
        case 1:
            data = GSV2202E_PORT_RX_B;;
            break;
        case 2:
            data = GSV2202E_PORT_RX_C;
            break;
        case 3:
            data = GSV2202E_PORT_RX_D;
            break;
        default:
            XLOGE("Error port\n");
            break;
    }
    XLOGD("setActivePort: REG_RX_PORT = %d",data);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_RX_PORT, 2, 1, &data);
    XLOGD("setActivePort: s32Ret = %d",s32Ret);
    mCurrentPort = port;
    return s32Ret;
}

XBH_S32 ChipGsv2202E::getActivePort(XBH_U8* port)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *port = mCurrentPort;
    return s32Ret;
}

XBH_S32 ChipGsv2202E::getRxPortStatus(XBH_U8 port, XBH_U8* status)
{
    XBH_U8 data;
    * status = 0;
    XBH_S32 s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_RX_STATUS, 2, 1,&data);
    XLOGD("getPortStatus: port = %d",port);  //BIT(0~3)=1 RXA~D is plug   //BIT(4~7) RXA~D clock locked
    switch (port)
    {
        case 0:
            if ((data & 0x11) == 0x11) *status = 1;
            break;
        case 1:
            if ((data & 0x22) == 0x22) *status = 1;
            break;
        case 2:
            if ((data & 0x44) == 0x44) *status = 1;
            break;
        case 3:
            if ((data & 0x88) == 0x88) *status = 1;
            break;
        default:
            XLOGE("Error port\n");
            break;
    }
    XLOGD("getPortStatus: s32Ret = %d status = %d",s32Ret, *status);
    return s32Ret;
}

XBH_S32 ChipGsv2202E::getTxPortStatus(XBH_U8* status)
{
    XBH_U8 data;
    XBH_S32 s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_TX_STATUS, 2 ,1 ,&data);
    *status = (data & 0x01);
    XLOGD("getSwitchTxPortStatus: status = %d",*status);
    return s32Ret;
}

XBH_S32 ChipGsv2202E::setRxHpd(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipGsv2202E::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XLOGD("upgradeFirmware  strFilePath %s",strFilePath);
    property_set(GSV2202E_VERSION, "000000");
    forceUpgrade = true;
    mDevType = GSV2202E_CHIP_NUMBER;
    XLOGD ("[%s:%d] dev type = 0x%04x, i2c bus = %d, i2c addr = 0x%2x", __func__, __LINE__, mDevType, mI2cNumber, mI2cAddress);

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
    partition_data = (XBH_U8 *)malloc(248 * 1024);
    if (partition_data == NULL)
    {
        XLOGE("[%s:%d] malloc fiailed", __func__, __LINE__);
    }
    strcpy (file_name, strFilePath);
    XbhMWThread::run(XbhMWThread::ONCE);
    XBH_S32 s32Ret = XBH_SUCCESS;
    return s32Ret;
}

XBH_S32 ChipGsv2202E::setSpecificMode(void)
{
    XBH_S32 ret = XBH_SUCCESS;
    XBH_U8 standby_flag = 0x00;
    XBH_U8 data = 0xA1;
    //将Gsv2202E设置为standby模式
    XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress,REG_RESET,2,1,&data);
    XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress,REG_RESET,2,1,&standby_flag);
    if(standby_flag == 0xA1)
    {
        XLOGD("gsv2202E success enter standby!!! %x\n",standby_flag);
    }
    else
    {
        XLOGD("gsv2202E enter standby fail!!! %x\n",standby_flag);
        ret = XBH_FAILURE;
    }
    return ret;
}

XBH_S32 ChipGsv2202E::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[] = {0x00, 0x00};
    //增加容错机制
    XBH_U8 u8Reset_time = 0;
RESET:
    if(u8Reset_time ==3)
    {
        XLOGD("getFirmwareVersion  fail !!!");
        return XBH_FAILURE;
    }
    u8Reset_time++;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_FW_VER, 2 ,2 ,data);
     //XLOGD("getdata  data0[%x] date1[%x] mI2cNumb[%d] mI2cAddr[%x] ", data[0], data[1], mI2cNumber, mI2cAddress);
    if(s32Ret == XBH_FAILURE || (data[0] == 0 && data[1] == 0))
    {
        usleep(500*1000);
        goto RESET;
    }
    sprintf(strVersion, "%02X%02X", data[0], data[1]);
    XLOGD("getFirmwareVersion  =  %s",strVersion);
    property_set("persist.vendor.xbh.gsv2202e.ver", strVersion);
    return s32Ret;
}

XBH_S32 ChipGsv2202E::resetChip()
{
    XBH_U8 data[1] = {CMD_RESET};
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_HPD_ASSERT_CTRL, 2, 1, data);
    return s32Ret;
}

XBH_S32 ChipGsv2202E::setHPD(XBH_U8 portId, XBH_BOOL bHigh)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data = 0x00;
    XLOGD(" setHPD [%d] = %d",portId,bHigh);
    if(portId == 0) //bits:4 0
    {
        if(bHigh)
        {
            data = 0x11;
        }
        else
        {
            data = 0x10;
        }
    }
    else if(portId == 1) //bits:6 2
    {
        if(bHigh)
        {
            data = 0x44;
        }
        else
        {
            data = 0x40;
        }
    }
    else if(portId == 2) //bits:5 1
    {
        if(bHigh)
        {
            data = 0x22;
        }
        else
        {
           data = 0x20;
        }
    }
    else if(portId == 3)  //bits:7 3
    {
        if(bHigh)
        {
            data = 0x88;
        }
        else
        {
            data = 0x80;
        }
    }

    if(data != 0x00)
    {
         s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_HPD_ASSERT_CTRL, 2, 1, &data);
    }
    return s32Ret;
}

XBH_U8 ChipGsv2202E::calcCRC8(const XBH_U8 *data, XBH_U32 len)
{
    XBH_U8 crc8 = 0;
    XBH_U16 i = 0;
    while (len --)
    {
        crc8 = crc8 ^ data[i];
        crc8 = crc8Gsv2202ETable[crc8];
        i++;
    }
    return crc8;
}

XBH_S32 ChipGsv2202E::writeData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth)
{
    XBH_S32 ret = XBH_FAILURE;
    // XLOGV("[%s:%d] reg = 0x%02x, date = 0x%02x", __func__, __LINE__, reg_addr, &p_data);
    XBH_S32 timeout = 0;
    while ((ret != XBH_SUCCESS) && (timeout < 3))
    {
        ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, XBH_GSV2202E_DEVICE_ADDRESS, reg_addr, 2, data_lenth, p_data);
        if (ret != XBH_SUCCESS)
        {
            XLOGE("[%s:%d] error: %d", __func__, __LINE__, ret);
        }
        timeout++;
    }
    return ret;
}

XBH_S32 ChipGsv2202E::readData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth)
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

XBH_S32 ChipGsv2202E::getUpgradeState(XBH_S32 *pstat)
{
    *pstat = upgradeState;
    XLOGD("[%s:%d] status:%d", __func__, __LINE__, upgradeState);
    return (upgradeState == GSV2202EUpgrade_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
}

XBH_S32 ChipGsv2202E::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipGsv2202E::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

void ChipGsv2202E::reset(void)
{
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
    usleep(50*1000);
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
    usleep(10*1000);
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
    usleep(50*1000);
}

XBH_S32 ChipGsv2202E::writeRXDDCUpdConfigID(void)
{
    XBH_U16 i;
    XBH_U8 tmp[4];
    //write
    writeData(XBH_GSV2202E_RXDDC_UPDCONFIGID, (XBH_U8 *)UpdConfigID, 4);
    //wait reply
    for(i = 0; i < 100; i++)
    {
        usleep (20 * 1000);
        readData(XBH_GSV2202E_RXDDC_UPDREPLYID, tmp, 4);
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
            readData(XBH_GSV2202E_RXDDC_UPDCONFIGID, tmp, 4);
            XLOGD("please check FF27 if write success : 0x%02x%02x%02x%02x", tmp[0], tmp[1], tmp[2], tmp[3]);
        }
    }
    //time out return
    return -1;
}

XBH_S8 ChipGsv2202E::writeRXDDCUpdInfo(XBH_U32 appSize, XBH_U32 appAddr)
{
    GSV2202EUpgradeInfo Info;
    Info.DataTotalLen = appSize;
    Info.DataOffsetAtFlash = appAddr;
    Info.VersionMajor = XBH_GSV2202E_APP_VERSION_MAJOR;
    Info.VersionMinor = XBH_GSV2202E_APP_VERSION_MINOR;
    Info.GsvChipNumber = mDevType;

    //checksum
    XBH_U8 dataSum = calcCRC8((XBH_U8 *)&Info, sizeof(GSV2202EUpgradeInfo));
    //write
    writeData(XBH_GSV2202E_RXDDC_UPDINFO, (XBH_U8 *)&Info, sizeof(GSV2202EUpgradeInfo));
    writeData(XBH_GSV2202E_RXDDC_INFOCHECKSUM, &dataSum, 1);
    //
    return 0;
}

XBH_S8 ChipGsv2202E::writeRXDDCUpdData(XBH_U8 *pbuf)
{
    XBH_U8 dataSum = 0;
    dataSum = calcCRC8(pbuf, XBH_TRANSFER_DATA_BLKSIZE);
    writeData(XBH_GSV2202E_RXDDC_UPDDATA, pbuf, XBH_TRANSFER_DATA_BLKSIZE);
    writeData(XBH_GSV2202E_RXDDC_DATACHECKSUM, &dataSum, 1);
    //
    return 0;
}

 XBH_S8 ChipGsv2202E::writeMsgCmd(void)
{
    writeData(XBH_GSV2202E_RXDDC_COMMAND, &RXDDCCmd, 1);
    return 0;
}

XBH_S8 ChipGsv2202E::waitMsgAckNew(void)
{
    XBH_U16 i;
    XBH_U8 msgAck = 0;
    for(i = 0; i < 5000; i++) {
        usleep (1000);
        readData(XBH_GSV2202E_RXDDC_REPLY, &msgAck, 1);
        if((msgAck != 0) && ((msgAck & 0xf0) == (RXDDCCmd & 0xf0)))
            break;
    }
    if(i >= 5000)
        return -1;
    else {
        return 0;
    }
}

XBH_S8 ChipGsv2202E::msgCmdAndWaitNew(void)
{
    writeData(XBH_GSV2202E_RXDDC_COMMAND, &RXDDCCmd, 1);
    if(waitMsgAckNew() < 0)
        return -1;
    else {
        RXDDCCmd += XBH_GSV2202E_ROLL_BACK_INCDATA;
        return 0;
    }
}

XBH_S8 ChipGsv2202E::appI2CUpdateInit(void)
{
    RXDDCCmd = 0;
    // writeData(XBH_GSV2202E_RXDDC_REBOOT, &RXDDCCmd, 1);	//need clear
    XbhService::getModuleInterface()->setI2cData(mI2cNumber, XBH_GSV2202E_DEVICE_ADDRESS,XBH_GSV2202E_RXDDC_REBOOT, 2, 1,&RXDDCCmd);
    if(writeRXDDCUpdConfigID() < 0)
        return -1;
    else
        return McuRunAtBTag;
}

XBH_S8 ChipGsv2202E::appI2CUpdateServer(XBH_U8 * appData, XBH_U32 appSize, XBH_U32 appAddr)
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
    RXDDCCmd = XBH_GSV2202E_RXDDC_INFO_CMD;
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
            RXDDCCmd = XBH_GSV2202E_TRANSFER_END;
            writeData(XBH_GSV2202E_RXDDC_UPDCONFIGID, &RXDDCCmd, 1); //write other value to exit loop
            writeData(XBH_GSV2202E_RXDDC_REBOOT, &RXDDCCmd, 1);
            break;
        } 
        else
        {
            writeRXDDCUpdData(appData + RXDDCDataPoint);
            RXDDCDataPoint += XBH_TRANSFER_DATA_BLKSIZE;
            //此处打印为了防止SOC与基石芯片不同步
            XLOGD("RXDDCDataPoint = 0x%02x", RXDDCDataPoint);
            //next
            RXDDCCmd &= 0xf0;
            RXDDCCmd += XBH_GSV2202E_RXDDC_DATA_CMD;
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

 /*
 * gsv2202e退出低功耗模式
 * @return XBH_SUCCESS 退出低功耗模式成功，XBH_FAILURE 退出低功耗模式失败
 */
XBH_S32 ChipGsv2202E::setNormalMode(void)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U8 retryCount;
    XBH_U8 normal_flag = 0x00; //0xFF09 reg初始值
    XBH_U8 normal_data = CMD_WAKEUP; //2202e wakeup值
    XBH_CHAR FwVer[10] = {0};
    //基石建议拿到正确的版本信息后才能操作功耗寄存器0xFF09
    if((ret = getFirmwareVersion(FwVer)) != XBH_SUCCESS)
    {
        return ret;
    }
    //成功拿到2202e的版本信息后，将2202e从低功耗模式唤醒
    for(retryCount = 0; retryCount < 3; retryCount++)
    {
        if((ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_RESET, 2, 1, &normal_data)) != XBH_SUCCESS)
        {
            XLOGE("[%s:%d] Set I2C error: %d", __func__, __LINE__, ret);
            continue;
        }
        usleep(500 * 1000);
        if((ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_RESET, 2, 1, &normal_flag)) != XBH_SUCCESS)
        {
            XLOGE("[%s:%d] Get I2C error: %d", __func__, __LINE__, ret);
            continue;
        }

        if(normal_flag == normal_data)
        {
            XLOGD("Wakeup success! Status: 0x%02X", normal_flag);
            return XBH_SUCCESS;
        }
    }

    XLOGD("Wakeup failed after 3 attempts! Final status: 0x%02X", normal_flag);
    return XBH_FAILURE;
}

void ChipGsv2202E::run(const void* arg)
{
    XLOGI("[%s:%d]  file name: %s", __func__, __LINE__, file_name);
    XBH_S32 fd = 0;
    XBH_S32 ret = 0;
    XBH_S32 read_num = 0;
    XBH_CHAR* FWVer = (char *)malloc(10);

    RXDDCCmd = 0;
    McuRunAtBTag = -1;
    RXDDCDataPoint = 0;

    upgradeState = GSV2202EUpgrade_RUNNING;
    property_set("persist.vendor.xbh.gsv2202e.ver", "");

    if (appI2CUpdateInit() == 1)
    {
        XLOGD ("[%s:%d] mcu run at partition B, upgrade partition A", __func__, __LINE__);
        patition_start_addr = XBH_GSV2202E_PARTITION_A_ADDR; //4k
    }
    else if (appI2CUpdateInit() == 0)
    {
        XLOGD ("[%s:%d] mcu run at partition A, upgrade partition B", __func__, __LINE__);
        patition_start_addr = XBH_GSV2202E_PARTITION_B_ADDR; //256k
    }
    else
    {
        XLOGD ("[%s:%d] Error: Can`t Start Upgrade!!!", __func__, __LINE__);
        upgradeState = GSV2202EUpgrade_FAILURE;
        goto end;
    }

    fd = open(file_name, O_RDONLY);
    if (fd <= 0)
    {
        XLOGE("[%s:%d] open Error: ", __func__, __LINE__);
        upgradeState = GSV2202EUpgrade_FAILURE;
        goto end;
    }

    if (lseek (fd, patition_start_addr, SEEK_SET) < 0)
    {
        XLOGE("[%s:%d] lseek Error: ", __func__, __LINE__);
        close (fd);
        upgradeState = GSV2202EUpgrade_FAILURE;
        goto end;
    }

    XLOGD ("[%s:%d] read file...", __func__,  __LINE__);
    while (read_num < (248 * 1024))
    {
        ret = read (fd, partition_data + read_num, 64);
        if (ret < 0)
        {
            XLOGE("[%s:%d] read partition Error!!!", __func__, __LINE__);
            close (fd);
            upgradeState = GSV2202EUpgrade_FAILURE;
            goto end;
        }
        read_num +=  ret;
    }
    XLOGD ("[%s:%d] read file done", __func__,  __LINE__);
    close (fd);

    if ((partition_data[0x8e]  != dataGsv2202EBuff[0]) || (partition_data[0x8f]  != dataGsv2202EBuff[1]) || forceUpgrade)
    {
        upgradeState = GSV2202EUpgrade_RUNNING;
        if (appI2CUpdateServer(partition_data, 248 * 1024, patition_start_addr))
        {
            upgradeState = GSV2202EUpgrade_SUCCESS;
            usleep(5*1000*1000);
            reset();
            usleep(10*1000*1000);
            getFirmwareVersion(FWVer);
        }
        else
        {
            upgradeState = GSV2202EUpgrade_FAILURE;
        }
    }
end:
    if (partition_data != NULL)
    {
        // 释放空间
        free (partition_data);
    }
}

