
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@CSV5363_InputSource"
#include "XbhLog.h"


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <string.h>
#include "Csv5363.h"
#include "XbhService.h"
#define MAX_DATA_LENGTH (512 * 1024)    //buffer最大长度，升级文件大小不超过512
#define MAX_UX_FIRMWARE_LENGTH      (0x10000)

XbhMutex                     Csv5363::mLock;
XBH_U8 SPI_Flash_data[MAX_DATA_LENGTH] = {0};
static unsigned int  g_data_buffer_len = 0;
static unsigned int  g_read_buffer_len = 0;

//---------------- custom device interface start ----------------
/**
 * 复位当前转换芯片
 */
XBH_S32 Csv5363::resetChip()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (mRGpio == -1)
        return s32Ret;

    s32Ret |= XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
    usleep(50 * 1000);
    s32Ret |= XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);

    XLOGW("func: %s", __FUNCTION__);

    return s32Ret;
}

/**
 * 设置当前信源的HPD状态
 * value 高低状态
 */
XBH_S32 Csv5363::setRxHpd(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FALSE;
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 Csv5363::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FALSE;
    return s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 Csv5363::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FALSE;
    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 Csv5363::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGD("===> ChipCSV5363 getFirmwareVersion");

    if (mState == CSV5363_RUNNING)
    {
        XLOGE("Warning: CSV5363 is updating...");
        return XBH_FAILURE;
    }

    XBH_U8 RDat, ver_l, ver_h,ver_c3;
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_ID, &RDat);
    if(RDat != 0x90)
    {
        XLOGE("===> getFirmwareVersion strVersion is NULL");
        return XBH_FAILURE;
    }
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C1, &ver_h);
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C2, &ver_l);
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C3, &ver_c3);
    XLOGD("===> CSV5363 version = [%02X %02X %02X]!\n",ver_h, ver_l,ver_c3);
    sprintf(strVersion,"%02X%02X%02X", ver_h, ver_l,ver_c3);
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 Csv5363::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == CSV5363_RUNNING)
    {
        XLOGW("Warning: CSV5363 is updating...");
        return XBH_SUCCESS;
    }

    if (strFilePath == NULL)
    {
        XLOGE("Error: fileName is NULL");
        return XBH_FALSE;
    }

    mForceUpgrade = bForceUpgrade;

    //set upgrade file name
    memset(mFileName, 0x00, sizeof(mFileName));
    if (strlen(strFilePath) >= sizeof(mFileName))
    {
        XLOGE("Error: fileName is too long: %d", strlen(strFilePath));
        return XBH_FALSE;
    }
    strncpy(mFileName,strFilePath, strlen(strFilePath));

    XLOGE("prepare to upgradeFirmware %s", strFilePath);
    XbhMWThread::run(XbhMWThread::ONCE);
    return XBH_SUCCESS;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 Csv5363::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

XBH_S32 Csv5363::ASL_I2C2_Write_Reg(XBH_U32 BLOCK_ID, XBH_U8 wData)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, 1, &wData);
    usleep(5);
    return s32Ret;
}

XBH_S32 Csv5363::ASL_I2C2_Read_Reg(XBH_U32 BLOCK_ID, XBH_U8 *pData)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, 1, pData);
    usleep(5);
    return s32Ret;
}

XBH_S32 Csv5363::ASL_I2C2_Write_Reg_Len(XBH_U32 BLOCK_ID, XBH_U8 *data, XBH_U8 Num)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, Num, data);
    return s32Ret;
}

XBH_S32 Csv5363::ASL_I2C2_Read_Reg_Len(XBH_U32 BLOCK_ID, XBH_U8 *data, XBH_U8 Num)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, Num, data);
    return s32Ret;
}

XBH_S32 Csv5363::I2C_Wait_RW_DONE(XBH_U32 Timout)
{
    XBH_U8 rdata=1;
    while(0!=rdata)
    {
        ASL_I2C2_Read_Reg(0x20003f,&rdata); //bit0=RW_DONE
        Timout--;
        if(0==Timout)
        {
            return XBH_FALSE;
        }
    }
    return 0;
}

//SPI Flash I2C write_done_check
XBH_S32 Csv5363::I2C_write_done_check(XBH_U8 Timeout)
{
    unsigned char write_done= 0x01;

    while(write_done==0x01)
    {
        ASL_I2C2_Write_Reg(0x200025,0x80); //General instruction type enable
        ASL_I2C2_Read_Reg(0x200038, &write_done);
        write_done = write_done & 0x01;
        if (Timeout-- == 0)
        {
            XLOGD("--I2C_write_done_check Timeout-- == 0 false\r\n");
            return XBH_FALSE;
        }
        usleep(10);
    }
    return 0;
}


XBH_S32 Csv5363::SPI_Flash_Protect_I2C(XBH_BOOL Protect_EN)
{
    XBH_U8 ProtectValue = Protect_EN;
    ProtectValue =Protect_EN? 0x7C : 0x00;

    ASL_I2C2_Write_Reg(0x200036,0x06);		 //General instruction type Write_Enable for WEL bit
    ASL_I2C2_Write_Reg(0x200025,0x40);		 //General instruction type enable

    //SPI flash protection 
    ASL_I2C2_Write_Reg(0x200034,ProtectValue);  //STATUS_REGISTER_IN
    ASL_I2C2_Write_Reg(0x200025,0x04); //WRITE_STATUS_EN=1

    return I2C_write_done_check(100);
}

void Csv5363::I2C_Flash_Erase(XBH_U32 StartAddr)
{
    ASL_I2C2_Write_Reg(0x200036,0x06); //General instruction type
    ASL_I2C2_Write_Reg(0x200025,0x40); //General instruction type enable

    //erase SPI flash
    //0 Flash_addr
    if (3==SPI_Address_Length) //3 byte address
    {
        wdata[0]  = StartAddr & 0x0000ff;
        wdata[1]  = (StartAddr & 0x00ff00)>>8;
        wdata[2]  = (StartAddr & 0xff0000)>>16;
        ASL_I2C2_Write_Reg(0x200020,wdata[0]);
        ASL_I2C2_Write_Reg(0x200021,wdata[1]);
        ASL_I2C2_Write_Reg(0x200022,wdata[2]);
    }
    else //2byte address
    {
        wdata[0]  = StartAddr & 0x0000ff;
        wdata[1]  = (StartAddr & 0x00ff00)>>8;
        ASL_I2C2_Write_Reg(0x200020,wdata[0]);
        ASL_I2C2_Write_Reg(0x200021,wdata[1]);
    }

    //1Flash Erase type
    ASL_I2C2_Write_Reg(0x200037,0x20);    //sector Earse
    //2Flash Erase En
    ASL_I2C2_Write_Reg(0x200025,0x20);

    I2C_write_done_check(100);
}

//read spi data by I2C mode
//ReadAddr :start address
//pBuffer  :read buffer
//NumToRead: read count
XBH_S32 Csv5363::I2C2_SPI_Flash_Read(XBH_U32 ReadAddr,XBH_U8 *pBuffer,XBH_U32 NumToRead)
{
    XBH_U8 ReadCount=0x20; //fixed=32
    XBH_U8 flash_data_addr = 0x60;

    while(NumToRead)
    {
        //0Flash address
        if (3==SPI_Address_Length) //3 byte address
        {
            wdata[0]  = ReadAddr & 0x0000ff;
            wdata[1]  = (ReadAddr & 0x00ff00)>>8;
            wdata[2]  = (ReadAddr & 0xff0000)>>16;
            ASL_I2C2_Write_Reg(0x200020,wdata[0]);
            ASL_I2C2_Write_Reg(0x200021,wdata[1]);
            ASL_I2C2_Write_Reg(0x200022,wdata[2]);
            //1flash length
            ASL_I2C2_Write_Reg(0x200023,ReadCount-1);
        }
        else //2byte address
        {
            wdata[0]  = ReadAddr & 0x0000ff;
            wdata[1]  = (ReadAddr & 0x00ff00)>>8;
            ASL_I2C2_Write_Reg(0x200020,wdata[0]);
            ASL_I2C2_Write_Reg(0x200021,wdata[1]);
            //1flash length
            ASL_I2C2_Write_Reg(0x200022,ReadCount);
            ASL_I2C2_Write_Reg(0x200023,0x00);
        }

        //2 Flash read_en
        ASL_I2C2_Write_Reg(0x200025,0x02);
        I2C_Wait_RW_DONE(100);
        ASL_I2C2_Read_Reg_Len(0x200060, pBuffer, ReadCount);

        pBuffer = pBuffer + ReadCount;
        ReadAddr= ReadAddr + ReadCount;

        //for  last data<32
        if (NumToRead>=ReadCount)
        {
            NumToRead = NumToRead - ReadCount;
        }
        else
        {
            NumToRead=ReadCount;
        }
    }
    return 0;
}

//write SPI FLASH by I2C
//WriteAddr: write address(24bit)
//pBuffer: data buffer
//NumByteToWrite: write count(max 65535)

XBH_S32 Csv5363::I2C2_SPI_Flash_Write(XBH_U32 WriteAddr,XBH_U8 *pBuffer,XBH_U32 NumByteToWrite)
{
    XBH_U32 j=0;
    XBH_U8 flash_len=32;

    XBH_U8 data_addr =0x00;
    for (j=0;j<NumByteToWrite;)
    {
        Erase_SPIFlash_I2C(WriteAddr);

        if (3==SPI_Address_Length) //new mode for 3 byte address
        {
            wdata[0]  = WriteAddr & 0x0000ff;
            wdata[1]  = (WriteAddr & 0x00ff00)>>8;
            wdata[2]  = (WriteAddr & 0xff0000)>>16;
            ASL_I2C2_Write_Reg(0x200020,wdata[0]);
            ASL_I2C2_Write_Reg(0x200021,wdata[1]);
            ASL_I2C2_Write_Reg(0x200022,wdata[2]);
            //1flash length
            ASL_I2C2_Write_Reg(0x200023,0x1f);
        }
        else //old mode for 2byte address
        {
            wdata[0]  = WriteAddr & 0x0000ff;
            wdata[1]  = (WriteAddr & 0x00ff00)>>8;
            ASL_I2C2_Write_Reg(0x200020,wdata[0]);
            ASL_I2C2_Write_Reg(0x200021,wdata[1]);
            //1flash length
            ASL_I2C2_Write_Reg(0x200022,0x1f);
            ASL_I2C2_Write_Reg(0x200023,0x00);
       }

        ASL_I2C2_Write_Reg_Len(0x200000, &pBuffer[j], flash_len);
        j=j+flash_len;
        WriteAddr = WriteAddr +0x20;

        ASL_I2C2_Write_Reg(0x200036,0x06);         //General instruction type
        ASL_I2C2_Write_Reg(0x200025,0x40);         //General instruction type enable
        usleep(1 * 1000);
        //3flash write_en
        ASL_I2C2_Write_Reg(0x200025,0x01);         // flash write enable

        //delay and make sure all data has been wrote.
        I2C_write_done_check(100);

    }
    return 0;
}

//init chip before flash update
XBH_S32 Csv5363::Chip_init_5366()
{
    XBH_U8 RDat;
    XBH_U32 CurVer;
    //check chip ID first
    mState = CSV5363_RUNNING;
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_ID, &RDat); 	//# expect value is. 0x58
    if (0x90==RDat)
    {
        XLOGD("---Read CS5366 chipID ok.\r\n");
    }
    else
    {
        XLOGD("---Read CS5366 Chip ID error.Get : 0x%02x.\r\n",RDat);
        return 1;
    }
    //Read version
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C1, &RDat);
    CurVer=RDat;
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C2, &RDat);
    CurVer = (CurVer<<8) | RDat;
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C3, &RDat);\
    CurVer = (CurVer<<8) | RDat;

    XLOGD("---Current Version is : 0x%06x.\r\n",CurVer);

    //init chip
    ASL_I2C2_Write_Reg(0x100008,0x24);	//disable OCM
    ASL_I2C2_Write_Reg(0x030025,0xE0);//r_sw_spi_ctrl=1
    ASL_I2C2_Write_Reg(0x100084,0x08);//disable ext flash
    //init offset
    Offset=0x4000;
    XLOGD("---Read CS5366 disable ext Offset=0x4000; ok.\r\n");
    return 0;
}

XBH_BOOL Csv5363::SPI_Flash_update_5366()
{
    XBH_U8 read_back_data[32];   // this data arrary is to save the data read back from flash
    XBH_U32 write_len;
    XBH_U8 RDat;
    XBH_U32 ChipID,CurVer;
    XBH_U32 i,j;

    if (Chip_init_5366())
    {
        XLOGD("Chip_init_5366: xbh_false\n");
        mState = CSV5363_FAILURE;
        return XBH_FALSE;
   }
    XLOGD("Erase_Flash_init_I2C \n");
    Erase_Flash_init_I2C(Offset); //init flash erase addrese
    //#---------------Check Flash ID--------------------------------
    XLOGD("Step1:\r\n");
    ASL_I2C2_Write_Reg(0x200025,0x08);     //# Read status Enable before read back from SPI flash
    ASL_I2C2_Read_Reg(0x200039, &RDat);     //# expect value is. 0x0B for XTX, 0x85 for PURAN
    XLOGD("---Flash manufactory ID 0x%02X.\r\n",RDat);
    ASL_I2C2_Read_Reg(0x20003A, &RDat);     //# expect value is, 0x10
    XLOGD("---Flash Chip ID 0x%02X.\r\n",RDat);

    XLOGD("---SPI flash protect disable.\r\n");
    SPI_Flash_Protect_I2C(0);

    write_len = g_data_buffer_len/32;

    XLOGD("Step2: Flash update and verify ......");
    for(i=0;i<write_len;i++)
    {
        // 32bytes data will be write into flash per time
        I2C2_SPI_Flash_Write(Offset+i*32, (XBH_U8 *)&SPI_Flash_data[(i*32)],32);
        XLOGD("-- Flash I2C2_SPI_Flash_Write, address=0x%x. ",Offset+i*32);
        // I2C2_SPI_Flash_Read(Offset+i*32, read_back_data, 32);
        // for(j=0;j<32;j++)
        // {
        //     if(SPI_Flash_data[i*32+j] !=read_back_data[j])
        //     {
        //         XLOGD("\r\n---Verify Flash data error, address=0x%x. \r\n",Offset+i*32);
        //         mState = CSV5363_FAILURE;
        //         return XBH_FALSE;
        //     }
        // }
        // XLOGD("I2C2_SPI_Flash_Write .");
    }

    XLOGD("Done.\r\n\r\n");

    XLOGD("Step3:\r\n");
    XLOGD("---SPI flash protect enable.\r\n");
    SPI_Flash_Protect_I2C(1);

    XLOGD("Step4: Reset and check Version...\r\n");

    ASL_I2C2_Write_Reg(0x100008,0x00); //reset chip
    usleep(100 * 1000);
    //check CRC
    ASL_I2C2_Read_Reg(0x010011,&RDat);
    RDat=RDat&0x0F;
    if (RDat!=0x04)
    {
        XLOGE("---flash CRC error: 0x%02x, udpate fail.\r\n",RDat);
        mState = CSV5363_FAILURE;
        return XBH_FALSE;
    }
    //Read version
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C1,&RDat);
    CurVer=RDat;
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C2,&RDat);
    CurVer = (CurVer<<8) | RDat;
    ASL_I2C2_Read_Reg(CSV5363_GET_IC_VER_C3,&RDat);
    CurVer = (CurVer<<8) | RDat;

    XLOGD("---New Version is : 0x%06x.\r\n",CurVer);
    // if (New_FW_Ver !=CurVer)
    // {
    //     XLOGE("Error: Version is not right.\r\n");
    //     mState = CSV5363_FAILURE;
    //     return XBH_FALSE;
    // }
    mState = CSV5363_SUCCESS;
    XLOGD(">>>>>>>>>SPI Flash Update Success<<<<<<<<<<\r\n");

    return XBH_TRUE;

}

void Csv5363::Erase_Flash_init_I2C(XBH_U32 EAddr)
{
    XLOGD(" Erase_Flash_init_I2C :\r\n");
    Erased_Address_I2C=EAddr;
}

void Csv5363::Erase_SPIFlash_I2C(XBH_U32 CurAddr)
{
    if (CurAddr<Erased_Address_I2C)
    {
        return;
    }
    XLOGD("-- Flash Erase_SPIFlash_I2C, address=0x%x. ",CurAddr);
    I2C_Flash_Erase(CurAddr);
    Erased_Address_I2C+=Each_Time_Erase_Size;

}

void Csv5363::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32FileSize = 0;
    XBH_U32 u32FwLength = 0;
    XBH_U32 u32FwEndLength = 0;
    XBH_U32 u32FwWriteOffset = 0;
    XBH_U32 u32FwWriteLength = 0;
    XBH_U32 u32FileCrc = 0;
    XBH_U32 u32GenFileCrc = 0;
    FILE* pFile = NULL;

    //set task status
    mState = CSV5363_RUNNING;

    //---------------------- open CSV5363 file ----------------------
    XLOGD("---> open CSV5363 file ");
    s32Ret = open_upgrade_file(mFileName);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Error:Open %s file fail!", mFileName);
        mState = CSV5363_FAILURE;
        return ;
    }

    if(SPI_Flash_update_5366() != XBH_SUCCESS){
        return ;
    }
}

XBH_S32 Csv5363::open_upgrade_file(const char *upgrade_file_path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("\n===> %s\n",__FUNCTION__);

    FILE *fp = XBH_NULL;
    fp = fopen(upgrade_file_path, "r");
    if(fp != XBH_NULL)
    {
        fseek(fp,0,SEEK_END);
        //---------------------- read upgrade file ----------------------
        g_data_buffer_len = ftell(fp);        //计算文件大小
        fseek(fp,0,SEEK_SET);
        XLOGD("==> read upgrade file size:%d\n",g_data_buffer_len);

        if(g_data_buffer_len < MAX_DATA_LENGTH)
        {
            XLOGD("==> read upgrade file:\n");
            fread(SPI_Flash_data, 1, g_data_buffer_len, fp);
            s32Ret = XBH_SUCCESS;
        }
        else
            XLOGE("==> upgrade file size more than %d bytes.\n",MAX_DATA_LENGTH);
        fclose(fp);
    }

    if (g_data_buffer_len <= MAX_UX_FIRMWARE_LENGTH)
    {
        XLOGE("==> Please check if the firmware is correct.\n");
        s32Ret = XBH_FAILURE;
    }

    return s32Ret;
}

//---------------- custom device interface end ----------------

Csv5363::Csv5363(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mState = CSV5363_IDLE;
    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mForceUpgrade = XBH_FALSE;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    //power on
    if (mPGpio != -1) {
        XbhService::getModuleInterface()->setGpioOutputValue(mPGpio, mPLevel);
        usleep(50 * 1000);
    }
    //reset
    if (mRGpio != -1) {
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, !mRLevel);
        usleep(50 * 1000);
        XbhService::getModuleInterface()->setGpioOutputValue(mRGpio, mRLevel);
    }

    XLOGD(" end ");
}

Csv5363::~Csv5363()
{

}

