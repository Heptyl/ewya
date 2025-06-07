#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipGsv2702"

#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XbhLog.h"
#include "XbhService.h"
#include "ChipGsv2702.h"
#include <cutils/properties.h>
#include "XbhSysOpt.h"
#include "XbhPartitionData.h"

XbhMutex                        ChipGsv2702::mLock;
ChipGsv2702::ChipGsv2702(XBH_S32 i2cNumber, XBH_S32 i2cAddress, XBH_S32 powerGpio, XBH_S32 powerLevel ,XBH_S32 resetGpio, XBH_S32 resetLevel , XBH_S32 defaultPort , HDMI_SWITCH_PORT u8SwitchPort, XBH_U8 level)
{
    XLOGD(" start level = %d ", level);
    this->mI2cNumber = i2cNumber;
    this->mI2cAddress = i2cAddress;
    this->mPowerGpio = powerGpio;
    this->mPowerLevel = powerLevel;
    this->mResetGpio = resetGpio;
    this->mResetLevel = resetLevel;
    this->mDefaultPort = defaultPort;
    this->mCurrentPort = defaultPort;
    memcpy(&m_u8SwitchPort, &u8SwitchPort, sizeof(HDMI_SWITCH_PORT));

    if(mPowerGpio != -1)
    {
        XbhService::getModuleInterface()->setGpioOutputValue(mPowerGpio, mPowerLevel);
        usleep(100*1000);
    }
    //上电之后复位之前加入启动保护程序
    powerUpProtection();
    if(0x01 == m_u8SwitchPort.port0 || 0x01 == m_u8SwitchPort.port1 || 0x01 == m_u8SwitchPort.port2 || 0x01 == m_u8SwitchPort.port3)
    {
        mHasNextPort = XBH_TRUE;
    }
    if(mResetGpio != -1)
    {
        //当存在后一级switch的时候需要等待后一级switch初始化完成
        if(mHasNextPort)
        {
            usleep(2 * level * 1000 * 1000);
        }
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(100 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
        usleep(50 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
        usleep(100 * 1000);
    }

    if(0x01 == m_u8SwitchPort.port0)
    {
        setRxHpd(0, true);
    }
    if(0x01 == m_u8SwitchPort.port1)
    {
        setRxHpd(1, true);
    }
    if(0x01 == m_u8SwitchPort.port2)
    {
        setRxHpd(2, true);
    }
    if(0x01 == m_u8SwitchPort.port3)
    {
        setRxHpd(3, true);
    }
    setActivePort(mDefaultPort);

    XLOGD(" end ");
}

ChipGsv2702::~ChipGsv2702()
{
    XLOGD("~ end");
}

XBH_S32 ChipGsv2702::setPowerChip(XBH_BOOL onoff)
{
    XLOGD("setPowerChip: onoff = %d",onoff);
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipGsv2702::setActivePort(XBH_U8 port)
{
    XbhMutex::Autolock _l(mLock);
    XLOGD("setActivePort: port = %d",port);
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data;
    switch (port)
    {
        case 0:
            data = GSV2702_PORT_RX_A;
            break;
        case 1:
            data = GSV2702_PORT_RX_B;;
            break;
        case 2:
            data = GSV2702_PORT_RX_C;
            break;
        case 3:
            data = GSV2702_PORT_RX_D;
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

XBH_S32 ChipGsv2702::setPhysicalAddr(XBH_U8 port, XBH_U16 cecAddr)
{
    XbhMutex::Autolock _l(mLock);
    XLOGD("port = %d",port);
    XBH_S32 s32Ret = XBH_FAILURE, k = 0;
    XBH_U8 physicalAddr[2] = {0};
    physicalAddr[0] = cecAddr >> 8;
    physicalAddr[1] = cecAddr & 0xFF;

    XBH_U8 updatePort;
    switch (port)
    {
        case XBH_HDMI_RXA:
            updatePort = REG_SPA_UPDATE_RXA;
            break;
        case XBH_HDMI_RXB:
            updatePort = REG_SPA_UPDATE_RXB;
            break;
        case XBH_HDMI_RXC:
            updatePort = REG_SPA_UPDATE_RXC;
            break;
        case XBH_HDMI_RXD:
            updatePort = REG_SPA_UPDATE_RXD;
            break;
        default:
            XLOGE("Error port\n");
            break;
    }
    XLOGD("REG_SPA_UPDATE_RXD = 0x%x",updatePort);
    if(XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_SPA_HIGH_ADDR, 2, 1, &physicalAddr[0]) == XBH_SUCCESS)
    {
        usleep(1000 * 100);
        if(XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_SPA_LOW_ADDR, 2, 1, &physicalAddr[1]) == XBH_SUCCESS)
        {
            usleep(1000 * 100);
            if(XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_SPA_UPDATE_CMD, 2, 1, &updatePort) == XBH_SUCCESS)
            {
                do {
                    usleep(10 * 1000);
                    k++;
                    XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_SPA_UPDATE_CMD, 2, 1, &updatePort);
                } while (updatePort != 0x02 && (k < 50));

                if(k >= 50)
                {
                    XLOGD(" setPhysicalAddr error ,port: %d,  cecAddr 0x%x\n", port, cecAddr);
                    return s32Ret;
                }
                s32Ret = XBH_SUCCESS;
            }
        }
    }
    XLOGD("s32Ret = %d",s32Ret);
    return s32Ret;
}

XBH_S32 ChipGsv2702::getActivePort(XBH_U8* port)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *port = mCurrentPort;
    return s32Ret;
}

XBH_S32 ChipGsv2702::getRxPortStatus(XBH_U8 port, XBH_U8* status)
{
    XbhMutex::Autolock _l(mLock);
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

XBH_S32 ChipGsv2702::getTxPortStatus(XBH_U8* status)
{
    XbhMutex::Autolock _l(mLock);
    XBH_U8 data;
    XBH_S32 s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_TX_STATUS, 2 ,1 ,&data);
    *status = (data & 0x01);
    XLOGD("getSwitchTxPortStatus: status = %d",*status);
    return s32Ret;
}

XBH_S32 ChipGsv2702::setRxHpd(XBH_U8 port, XBH_BOOL high)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 origin = 0x00;
    XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, REG_HPD_ASSERT_CTRL, 2, 1, &origin);
    XLOGD("setRxHpd: port = %d  high = %d origin = 0x%02x", port, high, origin);
    XBH_U8 data = 0x00;
    if(port == 0) //bits:4 0
    {
        data = XbhSysOpt::getInstance()->set_bit(origin, 4);
        if(high)
        {
            data = XbhSysOpt::getInstance()->set_bit(origin, 0);
        }
        else
        {
            data = XbhSysOpt::getInstance()->clear_bit(origin, 0);
        }
    }
    else if(port == 1) //bits:6 2
    {
        data = XbhSysOpt::getInstance()->set_bit(origin, 6);
        if(high)
        {
            data = XbhSysOpt::getInstance()->set_bit(origin, 2);
        }
        else
        {
            data = XbhSysOpt::getInstance()->clear_bit(origin, 2);
        }
    }
    else if(port == 2) //bits:5 1
    {
        data = XbhSysOpt::getInstance()->set_bit(origin, 5);
        if(high)
        {
            data = XbhSysOpt::getInstance()->set_bit(origin, 1);
        }
        else
        {
           data = XbhSysOpt::getInstance()->clear_bit(origin, 1);
        }
    }
    else if(port == 3)  //bits:7 3
    {
        data = XbhSysOpt::getInstance()->set_bit(origin, 7);
        if(high)
        {
            data = XbhSysOpt::getInstance()->set_bit(origin, 3);
        }
        else
        {
            data = XbhSysOpt::getInstance()->clear_bit(origin, 3);
        }
    }
    XLOGD("setRxHpd: port = %d  high = %d data = 0x%02x", port, high, data);
    if(data != 0x00)
    {
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_HPD_ASSERT_CTRL, 2, 1, &data);
    }
    return s32Ret;
}

XBH_S32 ChipGsv2702::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XbhMutex::Autolock _l(mLock);
    XLOGD("upgradeFirmware  strFilePath %s",strFilePath);
    property_set(GSV2702_VERSION, "000000");
    forceUpgrade = true;
    mDevType = 0x2702;
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

XBH_S32 ChipGsv2702::getFirmwareVersion(XBH_CHAR* strVersion)
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
    return s32Ret;
}

XBH_S32 ChipGsv2702::resetChip(XBH_BOOL hw)
{
    XbhMutex::Autolock _l(mLock);
    XBH_U8 data[1] = {CMD_RESET};
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("resetChip  =  %d",hw);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, REG_HPD_ASSERT_CTRL, 2, 1, data);
    return s32Ret;
}

XBH_U8 ChipGsv2702::calcCRC8(const XBH_U8 *data, XBH_U32 len)
{
    XBH_U8 crc8 = 0;
    XBH_U16 i = 0;
    while (len --)
    {
        crc8 = crc8 ^ data[i];
        crc8 = crc8Gsv2702Table[crc8];
        i++;
    }
    return crc8;
}

XBH_S32 ChipGsv2702::writeData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth)
{
    XBH_S32 ret = XBH_FAILURE;
    XLOGV("[%s:%d] reg = 0x%02x", __func__, __LINE__, reg_addr);
    XBH_S32 timeout = 0;
    while ((ret != XBH_SUCCESS) && (timeout < 3))
    {
        ret = XbhService::getModuleInterface()->setI2cData(mI2cNumber, XBH_GSV2702_DEVICE_ADDRESS, reg_addr, 2, data_lenth, p_data);
        if (ret != XBH_SUCCESS)
        {
            XLOGE("[%s:%d] error: %d", __func__, __LINE__, ret);
        }
        timeout++;
    }
    return ret;
}

XBH_S32 ChipGsv2702::readData(XBH_S32 reg_addr, XBH_U8 *p_data, XBH_U32 data_lenth)
{
    XBH_S32 ret = XBH_FAILURE;
    XLOGV("[%s:%d] reg = 0x%08x", __func__, __LINE__, reg_addr);
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

XBH_S32 ChipGsv2702::getUpgradeState(XBH_S32 *pstat)
{
    *pstat = upgradeState;
    XLOGD("[%s:%d] status:%d", __func__, __LINE__, upgradeState);
    return (upgradeState == GSV2702Upgrade_SUCCESS) ? XBH_SUCCESS : XBH_FAILURE;
}

void ChipGsv2702::reset(void)
{
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
    usleep(50*1000);
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, !mResetLevel);
    usleep(10*1000);
    XbhService::getModuleInterface()->setGpioOutputValue(mResetGpio, mResetLevel);
    usleep(50*1000);
}

XBH_S32 ChipGsv2702::writeRXDDCUpdConfigID(void)
{
    XBH_U16 i;
    XBH_U8 tmp[4];
    //write
    writeData(XBH_GSV2702_RXDDC_UPDCONFIGID, (XBH_U8 *)UpdConfigID, 4);
    //wait reply
    for(i = 0; i < 100; i++)
    {
        usleep (20 * 1000);
        readData(XBH_GSV2702_RXDDC_UPDREPLYID, tmp, 4);
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
            readData(XBH_GSV2702_RXDDC_UPDCONFIGID, tmp, 4);
            XLOGD("please check FF27 if write success : 0x%02x%02x%02x%02x", tmp[0], tmp[1], tmp[2], tmp[3]);
        }
    }
    //time out return
    return -1;
}

XBH_S8 ChipGsv2702::writeRXDDCUpdInfo(XBH_U32 appSize, XBH_U32 appAddr)
{
    GSV2702UpgradeInfo Info;
    Info.DataTotalLen = appSize;
    Info.DataOffsetAtFlash = appAddr;
    Info.VersionMajor = XBH_GSV2702_APP_VERSION_MAJOR;
    Info.VersionMinor = XBH_GSV2702_APP_VERSION_MINOR;
    Info.GsvChipNumber = mDevType;
    //checksum
    XBH_U8 dataSum = calcCRC8((XBH_U8 *)&Info, sizeof(GSV2702UpgradeInfo));
    //write
    writeData(XBH_GSV2702_RXDDC_UPDINFO, (XBH_U8 *)&Info, sizeof(GSV2702UpgradeInfo));
    writeData(XBH_GSV2702_RXDDC_INFOCHECKSUM, &dataSum, 1);
    //
    return 0;
}

XBH_S8 ChipGsv2702::writeRXDDCUpdData(XBH_U8 *pbuf)
{
    XBH_U8 dataSum = 0;
    dataSum = calcCRC8(pbuf, XBH_TRANSFER_DATA_BLKSIZE);
    writeData(XBH_GSV2702_RXDDC_UPDDATA, pbuf, XBH_TRANSFER_DATA_BLKSIZE);
    writeData(XBH_GSV2702_RXDDC_DATACHECKSUM, &dataSum, 1);
    //
    return 0;
}

 XBH_S8 ChipGsv2702::writeMsgCmd(void)
{
    writeData(XBH_GSV2702_RXDDC_COMMAND, &RXDDCCmd, 1);
    return 0;
}

XBH_S8 ChipGsv2702::waitMsgAckNew(void)
{
    XBH_U16 i;
    XBH_U8 msgAck = 0;
    for(i = 0; i < 5000; i++) {
        usleep (1000);
        readData(XBH_GSV2702_RXDDC_REPLY, &msgAck, 1);
        if((msgAck != 0) && ((msgAck & 0xf0) == (RXDDCCmd & 0xf0)))
            break;
    }
    if(i >= 5000)
        return -1;
    else {
        return 0;
    }
}

XBH_S8 ChipGsv2702::msgCmdAndWaitNew(void)
{
    writeData(XBH_GSV2702_RXDDC_COMMAND, &RXDDCCmd, 1);
    if(waitMsgAckNew() < 0)
        return -1;
    else {
        RXDDCCmd += XBH_GSV2702_ROLL_BACK_INCDATA;
        return 0;
    }
}

XBH_S8 ChipGsv2702::appI2CUpdateInit(void)
{
    RXDDCCmd = 0;
    // writeData(XBH_GSV2702_RXDDC_REBOOT, &RXDDCCmd, 1);	//need clear
    XbhService::getModuleInterface()->setI2cData(mI2cNumber, XBH_GSV2702_DEVICE_ADDRESS,XBH_GSV2702_RXDDC_REBOOT, 2, 1,&RXDDCCmd);
    if(writeRXDDCUpdConfigID() < 0)
        return -1;
    else
        return McuRunAtBTag;
}

XBH_S8 ChipGsv2702::appI2CUpdateServer(XBH_U8 * appData, XBH_U32 appSize, XBH_U32 appAddr)
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
    RXDDCCmd = XBH_GSV2702_RXDDC_INFO_CMD;
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
            RXDDCCmd = XBH_GSV2702_TRANSFER_END;
            writeData(XBH_GSV2702_RXDDC_UPDCONFIGID, &RXDDCCmd, 1); //write other value to exit loop
            writeData(XBH_GSV2702_RXDDC_REBOOT, &RXDDCCmd, 1);
            break;
        } 
        else
        {
            writeRXDDCUpdData(appData + RXDDCDataPoint);
            RXDDCDataPoint += XBH_TRANSFER_DATA_BLKSIZE;
            //next
            RXDDCCmd &= 0xf0;
            RXDDCCmd += XBH_GSV2702_RXDDC_DATA_CMD;
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
XBH_S32 ChipGsv2702::powerUpProtection()
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
XBH_S32 ChipGsv2702::updateEdid(XBH_U8 edid_data[256],XBH_U8 port)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 ret = XBH_SUCCESS;
    XBH_U8 data = 0xa0;
    XBH_U8 en_data = 0xa;
    XBH_U8 page = 0;
    XBH_U8 edid_flag = 0;
    XBH_U8 edid_data_temp,edid_data_temp_1;
    XBH_U32 sum = 0;
    XBH_U32 id_num = 0;
    XBH_U32 i = 0;
    XBH_U8 tmp[64] = {0};
    //mode 是不同的EDID 处理方式 ，1是一种模式 ，2是一种模式，通过系统属性客制化
    XBH_CHAR cmode[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.gsv.edid.dycchange", cmode, "0");
    XBH_U32 mode = atoi(cmode);
    XBH_U8 count = 0;
    XBH_U8 k = 0; //计数校验，防止老的固件无法更新EDID时不能退出
START:
    //循环超过3次就退出
    if(count > 3)
    {
        XLOGD("EDID update much times, return \n");
        return XBH_FAILURE;
    }
    count ++;
    if(mode == 1)
    {
        //写入edid时，可以动态修改ID serial ID 值,但是必须修改校验值，否则不出图。
        XBH_CHAR strSn[64] = {0};
        ret  = XbhPartitionData::getInstance()->getSn(strSn);
        //这里有三种情况（1）没有烧录SN号，getSn此时会返回一个01234567的默认SN号
        //（2）SN号的长度大于8,从倒数第八个字符开始，截取到字符串末尾
        //（3）SN号的长度小于8，数组初始化值默认填0
        //XBH_CHAR strSn[PROPERTY_VALUE_MAX] = {0};
        //property_get("persist.vendor.xbh.guwei.test", strSn, NULL);
        int length = strlen(strSn);
        XLOGD("length = %x",length);
        if(length>8)
        {
            memmove(strSn, strSn + (length - 8), 8);
        }
        //ID serial 值 是四个字节代表8个16进制的数字
        for(id_num = 0;id_num < 8;id_num = id_num + 2)
        {
            //如果是1-9数字
            if(strSn[id_num] >= '0' && strSn[id_num] <= '9')
            {
                edid_data_temp = strSn[id_num] - '0';
            }//如果是字符a-z
            else if(strSn[id_num] >= 'a' && strSn[id_num] <= 'z')
            {
                edid_data_temp = strSn[id_num] - 'a' + 10;
            }//如果是字符A-Z
            else if(strSn[id_num] >= 'A' && strSn[id_num] <= 'Z')
            {
                edid_data_temp = strSn[id_num] - 'A' + 10;
            }//其它值
            else
            {
                XLOGE("ID serial not convert to edid , usb default num 0 ");
                edid_data_temp = 0;
            }
            //如果是1-9数字
            if(strSn[id_num + 1] >= '0' && strSn[id_num + 1] <= '9')
            {
                edid_data_temp_1 = strSn[id_num + 1] - '0';
            }//如果是字符a-z
            else if(strSn[id_num + 1] >= 'a' && strSn[id_num + 1] <= 'z')
            {
                edid_data_temp_1 = strSn[id_num + 1] - 'a' + 10;
            }//如果是字符A-Z
            else if(strSn[id_num + 1] >= 'A' && strSn[id_num + 1] <= 'Z')
            {
                edid_data_temp_1 = strSn[id_num + 1] - 'A' + 10;
            }//其它值
            else
            {
                XLOGE("ID serial not convert to edid , usb default num 0 ");
                edid_data_temp_1 = 0;
            }
            edid_data_temp = (edid_data_temp << 4)  + edid_data_temp_1;
            edid_data[12 + i++] = edid_data_temp;
        }
        //修改校验值 begin
        for(id_num = 0;id_num < 127 ; id_num++)
        {
            sum += edid_data[id_num];
        }
        edid_data[127] = (256 - (sum % 256)) % 256;
        //修改校验值 end        
    }
    else if(mode == 2)
    {
        //smart 客户要求
        //Q       Y     DDD     P     XXXX
        //31-30 29-25   24-19   18-14  13-0
        /*
            std::string serial = "K010JW00W0001";
        */
        int id_num = 0,sum = 0;
        XBH_CHAR strSn[64] = {0};
        XbhPartitionData::getInstance()->getSmartSnData(strSn);
        int length = strlen(strSn);
        //XLOGD("length = %x",length);
        if(length == XBH_CUSDATA_SMART_SN_REAL_LEN )
        {
            std::string serial(strSn);

            // 提取各部分字符串
            std::string stage_str = serial.substr(3, 1);
            std::string year_str = serial.substr(4, 1);
            std::string week_str = serial.substr(6, 2);
            std::string product_str = serial.substr(8, 1);
            std::string sn_str = serial.substr(9);

            //转成数字
            XBH_U32 stage = (stage_str[0] - '0') << 30;
            XBH_U32 year = (year_str[0] - 'A') << 25;
            XBH_U32 week = std::stoi(week_str) << 19;
            XBH_U32 product = (product_str[0] - 'A') << 14;
            XBH_U32 sn = std::stoi(sn_str);

            // 合成 final_serial
            XBH_U32 final_serial = stage | year | week | product | sn;
            //XLOGD("stage = %d| year=%d | week=%d | product=%d | sn=%d final_serial=%d \n", stage,year,week,product,sn,final_serial);
            //大端字节
            //edid_data[12] = (final_serial >> 24) & 0xff;
            //edid_data[13] = (final_serial >> 16) & 0xff;
            //edid_data[14] = (final_serial >> 8) & 0xff;
            //edid_data[15] = (final_serial) & 0xff;
            //edid_data[16] = std::stoi(week_str);//week
            //edid_data[17] = (year_str[0] - 'A') + 18 ;//A = 2008 B= 2009
            //客户要求 final_serial 小端字节  其它不变
            edid_data[15] = (final_serial >> 24) & 0xff;
            edid_data[14] = (final_serial >> 16) & 0xff;
            edid_data[13] = (final_serial >> 8) & 0xff;
            edid_data[12] = (final_serial) & 0xff;
            edid_data[16] = std::stoi(week_str);//week
            edid_data[17] = (year_str[0] - 'A') + 18 ;//A = 2008 B= 2009
            //更新校验码
            for(id_num = 0;id_num < 127 ; id_num++)
            {
                sum += edid_data[id_num];
            }
            edid_data[127] = (256 - (sum % 256)) % 256;
        }
        else
        {
            XLOGD("smart sn length error = %x",length);
        }
    }

    if(port == 0)//port a => bit 6:4  = 1
    {
        data = 0x90;
        en_data = 0x98;//en_data = data + 8 ==> 作用是拉hpd
    }
    else if(port == 1)//port b => bit 6:4  = 2
    {
        data = 0xa0;
        en_data = 0xa8;//en_data = data + 8 ==> 作用是拉hpd
    }
    else if(port == 2)//port c => bit 6:4  = 3
    {
        data = 0xb0;
        en_data = 0xb8;//en_data = data + 8 ==> 作用是拉hpd
    }
    else if(port == 3)//port d => bit 6:4  = 4
    {
        data = 0xc0;
        en_data = 0xc8;//en_data = data + 8 ==> 作用是拉hpd
    }
    for (page = 0; page < 4; ++page)
    {
        // 指定 EDID 页的地址.每次写入64个字节
        //XLOGD("EDID Page %d updated successfully!\n", page);
        if(XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress,0xff40, 2, 64, &(edid_data[page * 64])) == XBH_SUCCESS)
        {
            usleep(10 * 1000);//10ms
            memset(tmp, 0, 64);
            XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, 0xff40, 2, 64, tmp);
            // 逐元素比较
            for (XBH_U8 i = 0; i < 64; i++)
            {
                if (tmp[i] != edid_data[page * 64 + i])
                {
                    XLOGD("EDID Page %d compare data error !\n", page);
                    goto START;
                }
            }
            if(XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, 0xff19, 2, 1, &data) == XBH_SUCCESS)
            {
                data += 1;
                k = 0;
                // 等待直到 EDID 写入完成（读取状态）
                do {
                    usleep(10 * 1000);//10ms
                    k++;
                    XbhService::getModuleInterface()->getI2cData(mI2cNumber, mI2cAddress, 0xff19, 2, 1, &edid_flag);
                } while (edid_flag != 0x02 && (k < 50));
                if(k >= 50)
                {
                    XLOGD(" current fireware is not support update edid \n");
                    return ret;
                }
                // 打印 EDID 页已更新
                XLOGD("EDID Page %d updated successfully!\n", page);
            }
            else
            {
                XLOGD("EDID Page %d updated data error !\n", page);
                goto START;
            }
        }
        else
        {
            XLOGD("EDID Page %d updated data error !\n", page);
            goto START;
        }
    }
    if(XbhService::getModuleInterface()->setI2cData(mI2cNumber, mI2cAddress, 0xff19, 2, 1, &en_data) == XBH_FAILURE)
    {
        XLOGD("EDID updated set en_data error !\n");
        goto START;
    }
    return ret;
}
void ChipGsv2702::run(const void* arg)
{
    XLOGI("[%s:%d]  file name: %s", __func__, __LINE__, file_name);
    XBH_S32 fd = 0;
    XBH_S32 ret = 0;
    XBH_S32 read_num = 0;
    XBH_CHAR* FWVer = (char *)malloc(10);

    RXDDCCmd = 0;
    McuRunAtBTag = -1;
    RXDDCDataPoint = 0;

    upgradeState = GSV2702Upgrade_RUNNING;
    if (appI2CUpdateInit() == 1)
    {
        XLOGD ("[%s:%d] mcu run at partition B, upgrade partition A", __func__, __LINE__);
        patition_start_addr = XBH_GSV2702_PARTITION_A_ADDR; //4k
    }
    else if (appI2CUpdateInit() == 0)
    {
        XLOGD ("[%s:%d] mcu run at partition A, upgrade partition B", __func__, __LINE__);
        patition_start_addr = XBH_GSV2702_PARTITION_B_ADDR; //256k
    }
    else
    {
        XLOGD ("[%s:%d] Error: Can`t Start Upgrade!!!", __func__, __LINE__);
        upgradeState = GSV2702Upgrade_FAILURE;
        goto end;
    }

    fd = open(file_name, O_RDONLY);
    if (fd <= 0)
    {
        XLOGE("[%s:%d] open Error: ", __func__, __LINE__);
        upgradeState = GSV2702Upgrade_FAILURE;
        goto end;
    }

    if (lseek (fd, patition_start_addr, SEEK_SET) < 0)
    {
        XLOGE("[%s:%d] lseek Error: ", __func__, __LINE__);
        close (fd);
        upgradeState = GSV2702Upgrade_FAILURE;
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
            upgradeState = GSV2702Upgrade_FAILURE;
            goto end;
        }
        read_num +=  ret;
    }
    XLOGD ("[%s:%d] read file done", __func__,  __LINE__);
    close (fd);

    if ((partition_data[0x8e]  != dataGsv2702Buff[0]) || (partition_data[0x8f]  != dataGsv2702Buff[1]) || forceUpgrade)
    {
        upgradeState = GSV2702Upgrade_RUNNING;
        if (appI2CUpdateServer(partition_data, 252 * 1024, patition_start_addr))
        {
            upgradeState = GSV2702Upgrade_SUCCESS;
            usleep(2*1000*1000);
            reset();
            usleep(2*1000*1000);
            getFirmwareVersion(FWVer);
        }
        else
        {
            upgradeState = GSV2702Upgrade_FAILURE;
        }
    }
end:
    if (partition_data != NULL)
    {
        // 释放空间
        free (partition_data);
    }
}

