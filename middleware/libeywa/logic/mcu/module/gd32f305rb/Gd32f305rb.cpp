/*
    此型号MCU用于项目：
    311D2.X
*/

#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@Gd32f305rb"
#include "XbhLog.h"

#include "XbhService.h"
#include "Gd32f305rb.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <hardware/board.h>

XbhMutex                 Gd32f305rb::mLock;

const uint32_t g_crc32_mepg2_table[256] = {
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
    0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
    0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
    0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
    0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
    0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
    0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
    0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
    0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
    0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
    0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
    0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
    0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
    0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
    0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
    0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
    0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
    0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
    0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
    0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
    0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
    0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
    0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
    0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
    0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
    0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
    0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
    0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
    0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
    0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

/**
 * 复位mcu芯片
*/
XBH_S32 Gd32f305rb::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XbhMutex::Autolock _l(mLock);
    if (mRgpio > 0)
    {
        s32Ret = XbhService::getModuleInterface()->setGpioOutputValue(mRgpio, mRLevel);
        usleep(100 * 1000);         //gd32f310 min reset time 20us
        s32Ret |= XbhService::getModuleInterface()->setGpioOutputValue(mRgpio, !mRLevel);
    }
    else
    {
        XLOGE(" mcu chip is not reset pin !!!!!");
    }
    return s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 Gd32f305rb::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 Gd32f305rb::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 Gd32f305rb::calculate_CRC32_Mpeg2(void *pStart, unsigned int uiSize)
{
#define INIT  0xffffffff

    unsigned int  uiCRCValue;
    unsigned char *pData;

    if (NULL == pStart)
    {
        printf (" Input Value Error!\r\n");

        return (0xFFFFFFFF);
    }

    if (uiSize <= 0 )
    {
        printf(" Input len Error! \r\n");

        return (0xFFFFFFFF);
    }

    /* init the start value */
    uiCRCValue = INIT;
    pData      = (unsigned char *)pStart;

    /* calculate CRC */
    while (uiSize --)
    {
        uint8_t index = ((uiCRCValue >> 24) ^ *pData++) & 0xFF;
        uiCRCValue = g_crc32_mepg2_table[index] ^ (uiCRCValue << 8);
    }

    return uiCRCValue;
}

XBH_S32 Gd32f305rb::WriteMcuMultiBytes(XBH_U8 s32RegAddr, XBH_U32 u32Length,  XBH_U8 *pu8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if (pu8Data == NULL && (u32Length > MCU_I2C_DATA_MAX))
    {
        XLOGE("[%s,%d] i2c Error len oversize", __FUNCTION__, __LINE__);
        return XBH_FAILURE;
    }

    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, s32RegAddr, 1, u32Length, pu8Data);

    return s32Ret;
}

XBH_S32 Gd32f305rb::ReadMcuMultiBytes(XBH_U8 s32RegAddr, XBH_U32 u32Length, XBH_U8* pu8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    if (pu8Data == NULL)
    {
        XLOGE("[%s,%d] rec buffer", __FUNCTION__, __LINE__);
        return XBH_FAILURE;
    }

    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, s32RegAddr, 1, u32Length, pu8Data);

    return s32Ret;
}

XBH_S32 Gd32f305rb::set_enter_iap_mode(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    XBH_U8  pu8Data[1] = {0x01};

    s32Ret = WriteMcuMultiBytes(CMD_I2C_ENTER_IAP, 1, pu8Data);

    usleep(100 * 1000); //进入iap_mode后延时100ms等待MCU进行crc校验初始化
    return s32Ret;
}

XBH_S32 Gd32f305rb::check_iap_CRC32(XBH_U32 pc_crc)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XBH_U8 buf[64] = {0x00};

    XBH_U32 mcu_crc = 0xFFFFFFFF;

    s32Ret = ReadMcuMultiBytes(CMD_I2C_READ_IAP_CRC32, 4, buf);

    if(s32Ret == XBH_SUCCESS){
        mcu_crc = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];

        XLOGE("app crc: %04x, mcu crc: %04x\n", pc_crc, mcu_crc);
        if(pc_crc == mcu_crc)
        {
            return 1;
        }
    }
    return 0;
}

XBH_S32 Gd32f305rb::set_iap_status(XBH_U8 status)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    XBH_U8 buf[1] = {0x00};

    if(status)
        buf[0] = 0x01;

    s32Ret = WriteMcuMultiBytes(CMD_I2C_WRITE_IAP_VALID, 1, buf);

    return s32Ret;
}

XBH_S32 Gd32f305rb::upgrade_download_data(XBH_U8 *buff, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U16 fileSize = length;
    XBH_U32 address = 0;
    XBH_U16 fileindex = 0;
    XBH_U16 dataLen = 0;
    XBH_U8  w_buff[128];
    static int n = 0;

    address = 0;
    while(fileSize)
    {
        if (fileSize >= 64){
            dataLen = 64;
        }else {
            dataLen = fileSize;
        }

        memset(w_buff, 0xFF, sizeof(w_buff));
        // data length, high to low
        w_buff[0] = (dataLen >>  8) & 0xFF;
        w_buff[1] = (dataLen >>  0) & 0xFF;
        // bin file address, high to low
        w_buff[2] = (address >> 24) & 0xFF;
        w_buff[3] = (address >> 16) & 0xFF;
        w_buff[4] = (address >>  8) & 0xFF;
        w_buff[5] = (address >>  0) & 0xFF;
        memcpy(&w_buff[6], &buff[fileindex], dataLen);   // copy bin file date start to w_buff[6] every time

        s32Ret = WriteMcuMultiBytes(CMD_I2C_WRITE_IAP_DATA, dataLen + 6, w_buff);
        if (s32Ret != XBH_SUCCESS) {
            XLOGE("Error: upgrade_download_data Error!");
            return XBH_FAILURE;
        }

        if((address % 1024) == 0)
            usleep(20 * 1000);
        else
            usleep(10 * 1000);

        address += dataLen;
        fileindex += dataLen;
        fileSize -= dataLen;
        XLOGD("address:0x%x n:%d\n",0x8011800 + address,n++);
        usleep(50 * 1000);  //每次写入多延时50ms
    }
    XLOGD("END address:0x%x n:%d\n",0x8011800 + address,n);
    usleep(1000 * 1000);

    return s32Ret;
}


XBH_S32 Gd32f305rb::upgradetask(void)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    XBH_U32 u32FileSize = 0;
    XBH_U32 u32FileCrc = 0;
    XBH_U8* pU8FileBuff = NULL;
    //XBH_U8 strAppVersion[2];
    FILE* pFile = NULL;

    //set task status
    mState = XBH_UPGRADE_RUNNING;

    //---------------------- open gd32 file ----------------------
    XLOGD("---> open upgrade file ");
    pFile = fopen(mFileName, "rb");
    if (pFile == NULL)
    {
        XLOGE("Error:Open %s file fail!", mFileName);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }

    //---------------------- read upgrade file ----------------------
    XLOGD("---> read upgrade file ");
    fseek(pFile, 0, SEEK_END);
    u32FileSize = ftell(pFile);
    rewind(pFile);
    if(u32FileSize <= 0)
    {
        XLOGE("[%s,%d] pU8FileSize  ", __FUNCTION__, __LINE__);
        fclose(pFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }

    pU8FileBuff = (XBH_U8 *)malloc(sizeof(XBH_U8) * u32FileSize);
    if (pU8FileBuff == NULL)
    {
        XLOGE("[%s,%d] pU8FileBuff malloc Error", __FUNCTION__, __LINE__);
        fclose(pFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }
    memset(pU8FileBuff, 0x00, sizeof(XBH_U8) * u32FileSize);
    fread(pU8FileBuff, sizeof(XBH_U8), u32FileSize, pFile);

    //---------------------- enter iap mode --------------------------
    XLOGD("---> enter iap mode ");
    s32Ret = set_enter_iap_mode();
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] enterIapMode fail!", __FUNCTION__, __LINE__);
        free(pU8FileBuff);
        fclose(pFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }

    //---------------------- write data ----------------------
    XLOGD("---> write data ");
    s32Ret = upgrade_download_data(pU8FileBuff, u32FileSize);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] write data fail!", __FUNCTION__, __LINE__);
        free(pU8FileBuff);
        fclose(pFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }

    //---------------------- compare checksum ----------------------
    XLOGD("---> compare checksum ");
    u32FileCrc = calculate_CRC32_Mpeg2(pU8FileBuff, u32FileSize);

    if (check_iap_CRC32(u32FileCrc) != 1)
    {
        XLOGE("[%s,%d] Error check crc32 fail!", __FUNCTION__, __LINE__);
        free(pU8FileBuff);
        fclose(pFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }

    //---------------------- enter app mode --------------------------
    XLOGD("---> enter app mode ");
    s32Ret = set_iap_status(1);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] enterAppMode fail!", __FUNCTION__, __LINE__);
        free(pU8FileBuff);
        fclose(pFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }
    sleep(2);
    //---------------------- upgrade finish ----------------------
    XLOGD("upgrade finish!");
    free(pU8FileBuff);
    fclose(pFile);
    mState = XBH_UPGRADE_SUCCESS;

    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 Gd32f305rb::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    unsigned char buf[3] = {0x00};
    XbhMutex::Autolock _l(mLock);
    s32Ret = ReadMcuMultiBytes(CMD_I2C_GET_APP_VER, 3, buf);
    if( s32Ret == XBH_SUCCESS) {
        if (strVersion != NULL) {
            sprintf (strVersion, "v.%02x%02x%02x", buf[0], buf[1], buf[2]);
        }
        XLOGE("mcu version = %d.%d.%d", buf[0], buf[1], buf[2]);
    } else {
        XLOGE("ERROR: get mcu app ver\n");
        s32Ret = XBH_FAILURE;
    }
    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 Gd32f305rb::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XbhMutex::Autolock _l(mLock);
    if (mState == XBH_UPGRADE_RUNNING)
    {
        XLOGW("Warning: mcu is updating...");
        return XBH_SUCCESS;
    }

    if (strFilePath == NULL)
    {
        XLOGE("Error: fileName is NULL");
        return XBH_FAILURE;
    }

    mForceUpgrade = bForceUpgrade;
    //set upgrade file name
    memset(mFileName, 0x00, sizeof(mFileName));
    if (strlen(strFilePath) >= sizeof(mFileName))
    {
        XLOGE("Error: fileName is too long: %d", strlen(strFilePath));
        return XBH_FAILURE;
    }
    strncpy(mFileName, strFilePath, strlen(strFilePath));

    mUpgradeStatus = 1;

    XbhMWThread::run(XbhMWThread::ONCE);

    return XBH_SUCCESS;
}
/**
 * 获取固件升级时的状态
 */
XBH_S32 Gd32f305rb::getUpgradeState(XBH_S32 * s32State)
{
    *s32State = mState;
    return XBH_SUCCESS;
}

XBH_S32 Gd32f305rb::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 pu8Data[1];
    pu8Data[0] = (XBH_U8)mode;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_FATT_MODE, 1, pu8Data);
    return s32Ret;
}

XBH_S32 Gd32f305rb::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 buf[64] = {0x00};
    s32Ret = ReadMcuMultiBytes(CMD_I2C_GET_FATT_MODE, 1, buf);
    *mode = buf[0];
    return s32Ret;
}

XBH_S32 Gd32f305rb::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 param[4] = {0x00};
    param[0] = u8IIcNum;
    param[1] = u8DeviceAddr;
    param[2] = u8RegAddr;
    param[3] = u8Len;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_BYPASS_IIC_PARA, 4, param);
    usleep(100 * 1000);
    s32Ret |= WriteMcuMultiBytes(CMD_I2C_SET_BYPASS_IIC, u8Len, u8Data) ;
    XLOGE("setMcuIIcBypass num:%d, addr:0x%02x, reg:0x%02x, len:0x%d, data[0]:%02x", u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data[0]);
    return s32Ret;
}

XBH_S32 Gd32f305rb::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 param[4] = {0x00};
    param[0] = u8IIcNum;
    param[1] = u8DeviceAddr;
    param[2] = u8RegAddr;
    param[3] = u8Len;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_BYPASS_IIC_PARA, 4, param);
    usleep(100 * 1000);
    s32Ret |= ReadMcuMultiBytes(CMD_I2C_GET_BYPASS_IIC, u8Len, u8Data) ;
    XLOGE("getMcuIIcBypass num:%d, addr:0x%02x, reg:0x%02x, len:0x%d, data[0]:%02x", u8IIcNum, u8DeviceAddr, u8RegAddr, u8Len, u8Data[0]);
    return s32Ret;
}

XBH_S32 Gd32f305rb::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 param[2] = {0x00};
    param[0] = u8UartNum;
    param[1] = baud;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_BYPASS_UART_PARA, 2, param);
    XLOGE("setMcuUartBypassPara u8UartNum:%d, baud:0x%02x \n", u8UartNum, baud);
    return s32Ret;
}

XBH_S32 Gd32f305rb::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 data[128] = {0x00};
    data[0] = u8Len;
    memcpy(data + 1, u8Data, u8Len);
    u8Len += 1;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_BYPASS_UART, u8Len+1, data);
    XLOGE("setMcuUartBypass len:%d\n", u8Len);
    return s32Ret;
}

XBH_S32 Gd32f305rb::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = ReadMcuMultiBytes(CMD_I2C_GET_BYPASS_UART, u8Len, u8Data) ;
    XLOGE("getMcuUartBypass u8Len\n", u8Len);
    return s32Ret;
}

void Gd32f305rb::run(const void* arg)
{
    XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (mUpgradeStatus == 1)
    {
        s32Ret = upgradetask();
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGD("upgrade error!");
        }
        mUpgradeStatus = 0;
    }
}

Gd32f305rb::Gd32f305rb(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    mUpgradeStatus   = XBH_UPGRADE_IDLE;
    mForceUpgrade    = false;
    mPgpio = pGpio;
    mPLevel = pLevel;
    mRgpio = rGpio;
    mRLevel = rLevel;
    XLOGD(" end ");
}

Gd32f305rb::~Gd32f305rb()
{

}
