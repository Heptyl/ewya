
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipCS5803AQ"
#include "XbhLog.h"

#include <cutils/properties.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "ChipCs5803aq.h"
#include "XbhService.h"

XbhMutex                       ChipCS5803AQ::mLock;

#define I2C_SLAVE_ADDR      0x41
#define MAX_DATA_LENGTH (1024 * 1024)    //buffer最大长度，升级文件大小不超过1M

static unsigned char g_data_buffer[MAX_DATA_LENGTH]={0};
static unsigned int  g_data_buffer_len = 0;

XBH_S32 ChipCS5803AQ::ASL_I2C2_Write_Reg(XBH_U32 BLOCK_ID, XBH_U8 wData)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, 1, &wData);
    usleep(5);
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::ASL_I2C2_Read_Reg(XBH_U32 BLOCK_ID, XBH_U8 *pData)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, 1, pData);
    usleep(5);
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::ASL_I2C2_Write_Reg_Len(XBH_U32 BLOCK_ID, XBH_U8 *data, XBH_U8 Num)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, Num, data);
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::ASL_I2C2_Read_Reg_Len(XBH_U32 BLOCK_ID, XBH_U8 *data, XBH_U8 Num)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, BLOCK_ID, 3, Num, data);
    return s32Ret;
}

XBH_BOOL ChipCS5803AQ::I2C_Wait_RW_DONE(XBH_U32 Timout)
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
    return XBH_TRUE;
}

//SPI Flash I2C write_done_check
XBH_BOOL ChipCS5803AQ::I2C_write_done_check(XBH_U8 Timeout)
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
    return XBH_TRUE;
}

XBH_BOOL ChipCS5803AQ::SPI_Flash_Protect_I2C(XBH_BOOL Protect_EN)
{
    XBH_U8 ProtectValue = Protect_EN;

    if (mOffset==0x10000) {ProtectValue =Protect_EN? 0x7C : 0x04;}
    else if (mOffset==0x20000) {ProtectValue =Protect_EN? 0x7C : 0x08;}
    else {ProtectValue =Protect_EN? 0x7C : 0x00;}


    ASL_I2C2_Write_Reg(0x200036,0x06);		 //General instruction type Write_Enable for WEL bit
    ASL_I2C2_Write_Reg(0x200025,0x40);		 //General instruction type enable

    //SPI flash protection 
    ASL_I2C2_Write_Reg(0x200034,ProtectValue);//STATUS_REGISTER_IN
    ASL_I2C2_Write_Reg(0x200025,0x04); //WRITE_STATUS_EN=1

    return I2C_write_done_check(100);
}

void ChipCS5803AQ::I2C_Flash_Erase(XBH_U32 StartAddr)
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
    ASL_I2C2_Write_Reg(0x200037,0x20);  //sector Earse
    //2Flash Erase En
    ASL_I2C2_Write_Reg(0x200025,0x20);

    I2C_write_done_check(100);
}

//read spi data by I2C mode
//ReadAddr :start address
//pBuffer  :read buffer
//NumToRead: read count
XBH_BOOL ChipCS5803AQ::I2C2_SPI_Flash_Read(XBH_U32 ReadAddr,XBH_U8 *pBuffer,XBH_U32 NumToRead)
{
    XBH_U8 ReadCount=0x20; //fixed=32

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
            ASL_I2C2_Write_Reg(0x200022,ReadCount-1);
            ASL_I2C2_Write_Reg(0x200023,0x00);
        }

        //2 Flash read_en
        ASL_I2C2_Write_Reg(0x200025,0x02);
        I2C_Wait_RW_DONE(100);
        ASL_I2C2_Read_Reg_Len(0x200060, pBuffer, ReadCount);

        pBuffer = pBuffer + ReadCount;
        ReadAddr= ReadAddr + ReadCount;

        //for  last data<32
        //if (NumToRead>=ReadCount)
        //{
            NumToRead = NumToRead - ReadCount;
        //}
        //else
        //{
        //    NumToRead=ReadCount;
        //}
    }
    return XBH_TRUE;
}

//write SPI FLASH by I2C
//WriteAddr: write address(24bit)
//pBuffer: data buffer
//NumByteToWrite: write count(max 65535)

XBH_BOOL ChipCS5803AQ::I2C2_SPI_Flash_Write(XBH_U32 WriteAddr,XBH_U8 *pBuffer,XBH_U32 NumByteToWrite)
{
    XBH_U32 j=0;
    XBH_U8 flash_len=32;

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

        ASL_I2C2_Write_Reg(0x200036,0x06);       //General instruction type
        ASL_I2C2_Write_Reg(0x200025,0x40);       //General instruction type enable
        usleep(1 * 1000);
        //3flash write_en
        ASL_I2C2_Write_Reg(0x200025,0x01);       // flash write enable

        //delay and make sure all data has been wrote.
        I2C_write_done_check(100);

    }
    return XBH_TRUE;
}

void ChipCS5803AQ::Erase_Flash_init_I2C(XBH_U32 EAddr)
{
    XLOGD(" Erase_Flash_init_I2C :\r\n");
    Erased_Address_I2C=EAddr;
}

void ChipCS5803AQ::Erase_SPIFlash_I2C(XBH_U32 CurAddr)
{
    if (CurAddr<Erased_Address_I2C)
    {
        return;
    }
    XLOGD("-- Flash Erase_SPIFlash_I2C, address=0x%x. ",CurAddr);
    I2C_Flash_Erase(CurAddr);
    Erased_Address_I2C+=Each_Time_Erase_Size;

}


XBH_BOOL ChipCS5803AQ::chip_init_5803()
{
    XBH_CHAR strVersion[16];

    mUpdateStart = XBH_TRUE;
    if (XBH_SUCCESS != getFirmwareVersion(strVersion))
    {
        XLOGD("---Read CS5803 Chip ID or version error");
        return XBH_FALSE;
    }

    // Init chip
    ASL_I2C2_Write_Reg(0x10004f, 0x00);  // disable SPI mode
    ASL_I2C2_Write_Reg(0x200030, 0x11);  // external flash
      
    // Init offset
    mOffset = 0x0;
      
    return XBH_TRUE;
}

XBH_BOOL ChipCS5803AQ::spi_flash_update_5803()
{
    //XBH_U8 read_back_data[32];
    XBH_U32 write_len;
    XBH_U8 RDat;
    XBH_U16 CurVer;
    XBH_U32 i, j;

    Erase_Flash_init_I2C(mOffset);

    XLOGD("Step1: Check Flash ID");
    ASL_I2C2_Write_Reg(0x200025, 0x08);   // 读状态使能
    ASL_I2C2_Read_Reg(0x200039, &RDat);   // 制造商ID: 0x0B for XTX, 0x85 for PURAN  
    XLOGD("---Flash manufactory ID 0x%02X", RDat);
    ASL_I2C2_Read_Reg(0x20003A, &RDat);     // 芯片ID: 0x10
    XLOGD("---Flash Chip ID 0x%02X", RDat);
  
    XLOGD("---SPI flash protect disable");
    if (!SPI_Flash_Protect_I2C(XBH_FALSE))
    {
        XLOGE("===> disable flash protect failed");
        return XBH_FALSE;
    }

    write_len = g_data_buffer_len / 32;

    XLOGD("Step2: Flash update and verify, total blocks: %d", write_len);
    for (i = 0; i < write_len; i++) 
    {
        if (!I2C2_SPI_Flash_Write(mOffset + i * 32, &g_data_buffer[i * 32], 32))
        {
            XLOGE("===> Flash write failed at address 0x%x", mOffset + i * 32);
            return XBH_FALSE;
        }

//如果校验失败，是需要重刷的。  为了加快升级速度，减去校验过程。
#if 0
        if (!I2C2_SPI_Flash_Read(mOffset + i * 32, read_back_data, 32))
        {
            XLOGE("===> Flash read failed at address 0x%x", mOffset + i * 32);
            return XBH_FALSE;
        }
  
        for(j=0;j<32;j++)
        {
            if(g_data_buffer[i*32+j] !=read_back_data[j])
            {
                XLOGE("Verify  error at addr=0x%x, offset=%d: expected=0x%02x, actual=0x%02x",
                      mOffset+i*32, j, g_data_buffer[i*32+j], read_back_data[j]);
                return XBH_FALSE;
            }  
        }
#endif

        if (i % 10 == 0)
        {
            XLOGD("Progress: %d/%d blocks completed", i, write_len);
        }
    }

    XLOGD("Flash update and verify completed successfully");

    XLOGD("Step3: Enable SPI flash protect");
    if (!SPI_Flash_Protect_I2C(XBH_TRUE))
    {
        XLOGE("===> enable flash protect failed");
        return XBH_FALSE;
    }

    XLOGD("Step4: Reset and check Version");
    ASL_I2C2_Write_Reg(0x100042, 0x00);
    usleep(100 * 1000);

    // check CRC
    ASL_I2C2_Read_Reg(0x030018, &RDat);
    if (RDat != 0x80)
    {
        XLOGE("===> flash CRC error: 0x%02x, update failed", RDat);
        mState = CS5803_FAILURE;
        return XBH_FALSE;
    }

    ASL_I2C2_Read_Reg(0x0101C2, &RDat);
    CurVer = RDat;
    ASL_I2C2_Read_Reg(0x0101C1, &RDat);
    CurVer = CurVer | (RDat << 8);
    XLOGD("---New Version is: 0x%04x", CurVer);

    XLOGD(">>>>>>>>>SPI Flash Update Success<<<<<<<<<<");
    return XBH_TRUE;
}


/**
 * 复位当前转换芯片
 */
XBH_S32 ChipCS5803AQ::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;

    s32Ret = ASL_I2C2_Write_Reg(0x100042, 0x00);
    usleep(100 * 1000);
    
    return s32Ret;
}


XBH_S32 ChipCS5803AQ::open_upgrade_file(const char *upgrade_file_path)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("\n===> %s\n",__FUNCTION__);

    FILE *fp = XBH_NULL;
    fp = fopen(upgrade_file_path, "r");
    if(fp != XBH_NULL)
    {
        fseek(fp,0,SEEK_END);
        g_data_buffer_len = ftell(fp);      //计算文件大小
        fseek(fp,0,SEEK_SET);
        XLOGD("==> read upgrade file size:%d\n",g_data_buffer_len);
        if(g_data_buffer_len < MAX_DATA_LENGTH)
        {
            fread(g_data_buffer, 1, g_data_buffer_len, fp);
            s32Ret = XBH_SUCCESS;
        }
        else
        {
            XLOGD("==> upgrade file size more than %d bytes.\n",MAX_DATA_LENGTH);
        }
        fclose(fp);
    }
    else
    {
        XLOGD("==> open upgrade file fail!\n");
    }

    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipCS5803AQ::getFirmwareVersion(XBH_CHAR* strVersion)  
{  
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 RDat;
    XBH_U32 CurVer;
    XLOGD("===> ChipCS5803AQ getFirmwareVersion");

    if (mState == CS5803_RUNNING && !mUpdateStart)
    {
        XLOGE("Warning: CS5803AQ is updating...");
        return XBH_FAILURE;
    }
    mUpdateStart = XBH_FALSE;

    if (strVersion == XBH_NULL)
    {
        XLOGE("Error: strVersion is NULL");
        return XBH_FAILURE;
    }

    // Check chip ID first
    ASL_I2C2_Read_Reg(CS5803AQ_GET_IC_ID, &RDat);
    if (0x58 != RDat)
    {
        XLOGE("===> getFirmwareVersion chip ID error. Got: 0x%02x", RDat);
        return XBH_FAILURE;
    }

    ASL_I2C2_Read_Reg(CS5803AQ_GET_IC_VER_L, &RDat);
    CurVer = RDat;
    ASL_I2C2_Read_Reg(CS5803AQ_GET_IC_VER_H, &RDat);
    CurVer = CurVer | (RDat << 8);
  
    XLOGD("===> CS5803AQ version = 0x%04x", CurVer);
    sprintf(strVersion, "%04X", CurVer);
    property_set("persist.vendor.xbh.cs5803.ver", strVersion);

    return s32Ret;
}


/**
 * 进行固件升级
 */
XBH_S32 ChipCS5803AQ::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)  
{  
    if (mState == CS5803_RUNNING)  
    {  
        XLOGW("Warning: cs5803 is updating...");
        return XBH_FAILURE;
    }

    if (strFilePath == XBH_NULL)
    {
        XLOGE("Error: fileName is XBH_NULL");
        return XBH_FAILURE;
    }

    mForceUpgrade = bForceUpgrade;

    memset(mFileName, 0x00, sizeof(mFileName));
    if (strlen(strFilePath) >= sizeof(mFileName))  
    {  
        XLOGE("Error: fileName is too long: %d", strlen(strFilePath));
        return XBH_FAILURE;
    }
    strncpy(mFileName, strFilePath, strlen(strFilePath));

    XbhMWThread::run(XbhMWThread::ONCE);
    return XBH_SUCCESS;
}


/**
 * 获取当前升级的进度
 */
XBH_S32 ChipCS5803AQ::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

XBH_S32 ChipCS5803AQ::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGE("%s %d \n", __func__, __LINE__);

    return s32Ret;
}

void ChipCS5803AQ::run(const void* arg)  
{
    XBH_S32 s32Ret = XBH_FAILURE;
    mState = CS5803_RUNNING;
    property_set("persist.vendor.xbh.cs5803.ver", "");

    s32Ret = open_upgrade_file(mFileName);
    if(XBH_SUCCESS != s32Ret)
    {
        XLOGE("===> %s open upgrade file fail, file:%s", __FUNCTION__, mFileName);
        mState = CS5803_FAILURE;
        return;
    }

    if (!chip_init_5803())
    {
        XLOGE("===> chip_init_5803 failed");
        mState = CS5803_FAILURE;
        return;
    }

    if (spi_flash_update_5803() == XBH_FALSE)
    {
        XLOGE("===> %s flash update fail!", __FUNCTION__);
        mState = CS5803_FAILURE;
        return;
    }

    mState = CS5803_SUCCESS;
    return;
}

ChipCS5803AQ::ChipCS5803AQ(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mState = CS5803_IDLE;
    mForceUpgrade = XBH_FALSE;

    mI2cNum = i2cBus;
    mI2cAddr = Addr;
    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;
    mUpdateStart = XBH_FALSE;

    XLOGD(" end ");
}

ChipCS5803AQ::~ChipCS5803AQ()
{

}
