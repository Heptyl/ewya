
#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipIte6563_InputSource"
#include "XbhLog.h"


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "ChipIte6563.h"
#include "XbhService.h"

XbhMutex                     ChipIte6563::mLock;

static XBH_U32 CRC32_TAB[] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832,
    0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b,
    0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb,
    0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0,
    0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4,
    0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd,
    0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87,
    0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
    0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934,
    0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1,
    0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
    0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c,
    0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525,
    0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
    0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e,
    0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27,
    0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1,
    0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda,
    0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a,
    0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
    0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b,
    0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
    0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6,
    0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff,
    0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7,
    0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330,
    0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729,
    0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

//---------------- custom device interface start ----------------
/**
 * 复位当前转换芯片
 */
XBH_S32 ChipIte6563::resetChip()
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
XBH_S32 ChipIte6563::setRxHpd(XBH_U8 value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取chip是否初始化完成
*/
XBH_S32 ChipIte6563::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 ChipIte6563::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 ChipIte6563::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (mState == ITE6563_RUNNING)
    {
        XLOGW("Warning: ite6563 is updating...");
        return XBH_FAILURE;
    }

    if (mVersion[0] != '\0')
    {
        strncpy(strVersion, mVersion, ITE6563_VERSION_LENGTH);
        XLOGD("[%s,%d] Version by cache: %s", __FUNCTION__, __LINE__, strVersion);
        return XBH_SUCCESS;
    }

    if (strVersion == NULL)
    {
        XLOGE("Error: strVersion is NULL");
        return XBH_FAILURE;
    }

    s32Ret |= exitIspMode();
    s32Ret |= enterIspMode();//如果当前通道 DP IN, 会有黑屏
    s32Ret |= enableInterruptMask();
    if(s32Ret  == XBH_FAILURE)
    {
        return s32Ret;
    }
    s32Ret |= readI2cByteArray(ITE6563_VERSION_OFFSET, ITE6563_VERSION_LENGTH, (XBH_U8 *)strVersion);
    s32Ret |= exitIspMode();
    strVersion[ITE6563_VERSION_LENGTH-1]='\0';
    XLOGD("[%s,%d] Version by i2c: %s", __FUNCTION__, __LINE__, strVersion);

    return s32Ret;
}

/**
 * 进行固件升级
 */
XBH_S32 ChipIte6563::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    if (mState == ITE6563_RUNNING)
    {
        XLOGW("Warning: ite6563 is updating...");
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
    strncpy(mFileName,strFilePath, strlen(strFilePath));

    XLOGE("prepare to upgradeFirmware %s", strFilePath);
    XbhMWThread::run(XbhMWThread::ONCE);
    return XBH_SUCCESS;
}

/**
 * 获取当前升级的进度
 */
XBH_S32 ChipIte6563::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *s32State = mState;
    return s32Ret;
}

XBH_U32 ChipIte6563::genCrc32(XBH_VOID* pBuff, XBH_S32 s32Size)
{
    XBH_U32 crc = 0x55aa3cc3;
    XBH_U8 *p;
    p = (XBH_U8 *)pBuff;
    crc = crc ^ ~0U;
    while(s32Size--)
    {
        crc = CRC32_TAB[(crc ^ *p++) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ ~0U;
}

XBH_S32 ChipIte6563::writeI2cByte(XBH_S32 s32RegAddr, XBH_U8* pu8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (pu8Data == NULL)
    {
        XLOGE("[%s,%d] i2c Error", __FUNCTION__, __LINE__);
        return XBH_FAILURE;
    }
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, s32RegAddr, 1, 1, pu8Data);
    usleep(5);
    return s32Ret;
}

XBH_S32 ChipIte6563::readI2cByte(XBH_S32 s32RegAddr, XBH_U8* pu8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (pu8Data == NULL)
    {
        XLOGE("[%s,%d] i2c Error", __FUNCTION__, __LINE__);
        return XBH_FAILURE;
    }
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, s32RegAddr, 1, 1, pu8Data);
    usleep(5);
    return s32Ret;
}

XBH_S32 ChipIte6563::writeI2cByteArray(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32CurOffset = 0;
    XBH_S32 s32RegAddr = 0;
    XBH_U32 u32WriteTotalLength = 0;
    XBH_U32 u32WriteLength = 0;
    XBH_U8  pu8Data[3] = {0};
    XBH_S32 s32RetryCount = 0;

    if (pu8Buff == NULL)
    {
        XLOGE("[%s,%d] i2c Error", __FUNCTION__, __LINE__);
        return XBH_FAILURE;
    }

    if (u32Offset + u32Length > ITE6563_EMMC_END)
    {
        XLOGE("[%s,%d] s32Offset is error: 0x%x", __FUNCTION__, __LINE__, u32Offset);
        XLOGE("[%s,%d] u32Length is error: 0x%x", __FUNCTION__, __LINE__, u32Length);
        return XBH_FAILURE;
    }

    while (u32WriteTotalLength < u32Length)
    {
        //write offset
        s32RegAddr = 0x0f;
        pu8Data[0] = (XBH_U8)((u32Offset + u32WriteTotalLength) & 0xff);
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0e;
        pu8Data[0] = (XBH_U8)(((u32Offset + u32WriteTotalLength) >> 8) & 0xff);
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0d;
        pu8Data[0] = (XBH_U8)(((u32Offset + u32WriteTotalLength) >> 16) & 0xff);
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0f;
        pu8Data[0] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[0]);

        s32RegAddr = 0x0e;
        pu8Data[1] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[1]);

        s32RegAddr = 0x0d;
        pu8Data[2] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[2]);

        u32CurOffset = (pu8Data[2] << 16) | (pu8Data[1] << 8) | pu8Data[0];
        if ((u32CurOffset & 0xffffff) != ((u32Offset + u32WriteTotalLength) & 0xffffff))
        {
            s32RetryCount++;
            if (s32RetryCount > 5)
            {
                XLOGE("[%s,%d] Error, s32RetryCount 5 time error.", __FUNCTION__, __LINE__);
                XLOGE("[%s,%d] u32Offset + u32WriteTotalLength: 0x%x", __FUNCTION__, __LINE__,(u32Offset + u32WriteTotalLength));
                XLOGE("[%s,%d] u32CurOffset: 0x%x", __FUNCTION__, __LINE__,u32CurOffset);
                return XBH_FAILURE;
            }
            continue;
        }

        if (u32Length - u32WriteTotalLength > ITE6563_RW_DATA_ONCE)
        {
            u32WriteLength = ITE6563_RW_DATA_ONCE;
        }
        else
        {
            u32WriteLength = u32Length - u32WriteTotalLength;
        }

        //XLOGD("[%s,%d]u32WriteLength: 0x%x", __FUNCTION__, __LINE__, u32WriteLength);
        //write length
        s32RegAddr = 0x10;
        pu8Data[0] = u32WriteLength;
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0c;
        pu8Data[0] = 0x04;
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        //write Data
        s32RegAddr = 0x11;
        s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cNum, mI2cAddr, s32RegAddr, 1, u32WriteLength, &pu8Buff[u32WriteTotalLength]);

        do {
            s32RegAddr = 0x08;
            pu8Data[0] = 0x00;
            s32Ret = readI2cByte(s32RegAddr, pu8Data);
            //XLOGD("[%s,%d] pu8Data[0]: 0x%x", __FUNCTION__, __LINE__, pu8Data[0]);
            usleep(100);
        } while((pu8Data[0] & 0x08) == 0);

        u32WriteTotalLength += u32WriteLength;
    }

    return s32Ret;
}

XBH_S32 ChipIte6563::readI2cByteArray(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32CurOffset = 0;
    XBH_S32 s32RegAddr = 0;
    XBH_U32 u32ReadLength = 0;
    XBH_U32 u32ReadTotalLength = 0;
    XBH_U8  pu8Data[3] = {0};
    XBH_S32 s32RetryCount = 0;

    if (pu8Buff == NULL)
    {
        XLOGE("[%s,%d] i2c Error", __FUNCTION__, __LINE__);
        return XBH_FAILURE;
    }

    if (u32Offset + u32Length > ITE6563_EMMC_END)
    {
        XLOGE("[%s,%d] s32Offset is error: 0x%x", __FUNCTION__, __LINE__, u32Offset);
        XLOGE("[%s,%d] u32Length is error: 0x%x", __FUNCTION__, __LINE__, u32Length);
        return XBH_FAILURE;
    }

    //XLOGD("[%s,%d] u32Offset: 0x%x", __FUNCTION__, __LINE__, u32Offset);
    //XLOGD("[%s,%d] u32Length: 0x%x", __FUNCTION__, __LINE__, u32Length);
    while (u32ReadTotalLength < u32Length)
    {
        //clear buff
        s32RegAddr = 0x16;
        pu8Data[0] = 0x40;
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x16;
        pu8Data[0] = 0x00;
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        //write offset
        s32RegAddr = 0x0f;
        pu8Data[0] = (XBH_U8)((u32Offset + u32ReadTotalLength) & 0xff);
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0e;
        pu8Data[0] = (XBH_U8)(((u32Offset + u32ReadTotalLength) >> 8) & 0xff);
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0d;
        pu8Data[0] = (XBH_U8)(((u32Offset + u32ReadTotalLength) >> 16) & 0xff);
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0f;
        pu8Data[0] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[0]);

        s32RegAddr = 0x0e;
        pu8Data[1] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[1]);

        s32RegAddr = 0x0d;
        pu8Data[2] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[2]);

        u32CurOffset = (pu8Data[2] << 16) | (pu8Data[1] << 8) | pu8Data[0];
        //XLOGD("[%s,%d] u32Offset + u32ReadTotalLength: 0x%x", __FUNCTION__, __LINE__, (u32Offset + u32ReadTotalLength));
        //XLOGD("[%s,%d] u32CurOffset: 0x%x", __FUNCTION__, __LINE__, u32CurOffset);
        if ((u32CurOffset & 0xffffff) != ((u32Offset + u32ReadTotalLength) & 0xffffff))
        {
            s32RetryCount++;
            if (s32RetryCount > 5)
            {
                XLOGE("[%s,%d] Error, s32RetryCount 5 time error.", __FUNCTION__, __LINE__);
                return XBH_FAILURE;
            }
            continue;
        }

        if (u32Length - u32ReadTotalLength > ITE6563_RW_DATA_ONCE)
        {
            u32ReadLength = ITE6563_RW_DATA_ONCE;
        }
        else
        {
            u32ReadLength = u32Length - u32ReadTotalLength;
        }

        //XLOGD("[%s,%d]u32ReadLength: 0x%x", __FUNCTION__, __LINE__, u32ReadLength);

        //write length
        s32RegAddr = 0x10;
        pu8Data[0] = u32ReadLength;
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0c;
        pu8Data[0] = 0x08;
        s32Ret = writeI2cByte(s32RegAddr, pu8Data);

        do {
            s32RegAddr = 0x18;
            pu8Data[0] = 0x00;
            s32Ret = readI2cByte(s32RegAddr, pu8Data);
            //XLOGD("[%s,%d] pu8Data[0]: 0x%x", __FUNCTION__, __LINE__, pu8Data[0]);
            usleep(100);
        } while((pu8Data[0] & 0x08) == 0);

        //read Data
        s32RegAddr = 0x14;
        s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cNum, mI2cAddr, s32RegAddr, 1, u32ReadLength, pu8Buff + u32ReadTotalLength);
        u32ReadTotalLength += u32ReadLength;
    }

    return s32Ret;
}

XBH_S32 ChipIte6563::enterIspMode()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32RegAddr = 0;
    XBH_U8  pu8Data[1] = {0};

    s32RegAddr = 0x37;
    pu8Data[0] = 0x96;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    s32RegAddr = 0xf2;
    pu8Data[0] = 0x5a;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    s32RegAddr = 0x5e;
    pu8Data[0] = 0xe7;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    s32RegAddr = 0xd4;
    pu8Data[0] = 0x18;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    s32RegAddr = 0x50;
    pu8Data[0] = 0x00;
    s32Ret |= readI2cByte(s32RegAddr, pu8Data);
    if ((s32Ret == XBH_SUCCESS) && (pu8Data[0] & 0x80))
    {
        XLOGD("enterIspMode success!");
    }
    else
    {
        XLOGE("Error: enterIspMode fail!");
        s32Ret = XBH_FAILURE;
    }

    return s32Ret;
}

XBH_S32 ChipIte6563::exitIspMode()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32RegAddr = 0;
    XBH_U8  pu8Data[1] = {0};

    s32RegAddr = 0xf2;
    pu8Data[0] = 0xff;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    s32RegAddr = 0xf2;
    pu8Data[0] = 0xff;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    if (s32Ret == XBH_SUCCESS)
    {
        XLOGD("exitIspMode success!");
    }
    else
    {
        XLOGE("Error: exitIspMode fail!");
    }

    return s32Ret;
}

XBH_S32 ChipIte6563::enableInterruptMask()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32RegAddr = 0;
    XBH_U8  pu8Data[1] = {0};

    s32RegAddr = 0x0a;
    pu8Data[0] = 0xfe;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    s32RegAddr = 0x0b;
    pu8Data[0] = 0xff;
    s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

    if (s32Ret == XBH_SUCCESS)
    {
        XLOGD("enableInterruptMask success!");
    }
    else
    {
        XLOGE("Error: enableInterruptMask fail!");
    }
    return s32Ret;
}

XBH_S32 ChipIte6563::checkHead(XBH_U8* pu8Buff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8* pu8HeadBuff = NULL;
    pu8HeadBuff = (XBH_U8 *)malloc(ITE6563_HEAD_LENGTH);
    if (pu8HeadBuff == NULL)
    {
        XLOGE("pu8HeadBuff malloc buffer(size:%d) error!!", ITE6563_HEAD_LENGTH);
        return XBH_FAILURE;
    }
    memset(pu8HeadBuff, 0x00, ITE6563_HEAD_LENGTH);

    s32Ret |= readI2cByteArray(0x00, ITE6563_HEAD_LENGTH, pu8HeadBuff);

    if (memcmp(pu8HeadBuff, &pu8Buff[0x18], ITE6563_HEAD_LENGTH) != 0)
    {
        s32Ret = XBH_FAILURE;
    }

    if (pu8HeadBuff != NULL)
    {
        free(pu8HeadBuff);
    }

    if (s32Ret == XBH_SUCCESS)
    {
        XLOGD("Head is same", __FUNCTION__, __LINE__);
    }
    else
    {
        XLOGD("Head is not same", __FUNCTION__, __LINE__);
    }
    return s32Ret;
}

XBH_S32 ChipIte6563::eraseSector(XBH_U32 u32WriteOffset, XBH_U32 u32WriteEnd)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32RegAddr = 0;
    XBH_U8  pu8Data[3] = {0};
    XBH_S32 s32RetryCount = 0;
    XBH_U32 u32CurOffset = 0;

    while (u32WriteOffset < u32WriteEnd)
    {
        //erase data
        s32RegAddr = 0x0f;
        pu8Data[0] = (XBH_U8)(u32WriteOffset & 0xff);
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0e;
        pu8Data[0] = (XBH_U8)((u32WriteOffset >> 8) & 0xff);
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0d;
        pu8Data[0] = (XBH_U8)((u32WriteOffset >> 16) & 0xff);
        s32Ret |= writeI2cByte(s32RegAddr, pu8Data);

        s32RegAddr = 0x0f;
        pu8Data[0] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[0]);

        s32RegAddr = 0x0e;
        pu8Data[1] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[1]);

        s32RegAddr = 0x0d;
        pu8Data[2] = 0x00;
        s32Ret |= readI2cByte(s32RegAddr, &pu8Data[2]);

        u32CurOffset = (pu8Data[2] << 16) | (pu8Data[1] << 8) | pu8Data[0];
        if ((u32CurOffset & 0xffffff) == (u32WriteOffset & 0xffffff))
        {
            s32RegAddr = 0x0c;
            pu8Data[0] = 0x20;
            s32Ret |= writeI2cByte(s32RegAddr, pu8Data);
            usleep(100);

            s32RetryCount = 0;
            do {
                s32RegAddr = 0x09;
                pu8Data[0] = 0x00;
                s32Ret |= readI2cByte(s32RegAddr, pu8Data);
                //XLOGD("[%s,%d] pu8Data: 0x%x", __FUNCTION__, __LINE__, pu8Data[0]);
                usleep(100);
                s32RetryCount++;
                if (s32RetryCount > 5)
                {
                    XLOGE("[%s,%d] i2c Error", __FUNCTION__, __LINE__);
                    return XBH_FAILURE;
                }
            } while ((pu8Data[0] & 0x40) == 0);
            u32WriteOffset += ITE6563_SECTOR_LENGTH;
        }
        else
        {
            XLOGE("[%s,%d] i2c Error", __FUNCTION__, __LINE__);
            XLOGE("[%s,%d] u32CurOffset: %d", __FUNCTION__, __LINE__, u32CurOffset);
            XLOGE("[%s,%d] u32WriteOffset: %d", __FUNCTION__, __LINE__, u32WriteOffset);
            return XBH_FAILURE;
        }
    }

    if (s32Ret == XBH_SUCCESS)
    {
        XLOGD("eraseSector success!");
    }
    else
    {
        XLOGE("Error: eraseSector fail!");
    }

    return s32Ret;
}

XBH_S32 ChipIte6563::writeData(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = writeI2cByteArray(u32Offset, u32Length, pu8Buff);
    if (s32Ret == XBH_SUCCESS)
    {
        XLOGD("writeData success!");
    }
    else
    {
        XLOGE("Error: writeData fail!");
    }
    return s32Ret;
}

XBH_S32 ChipIte6563::verifyData(XBH_U32 u32Offset, XBH_U32 u32Length, XBH_U8* pu8Buff)
{
    XBH_U8 pu8TempData[ITE6563_RW_DATA_ONCE] = {0};
    XBH_U32 u32ReadLength = 0;
    XBH_U32 u32ReadTotalLength = 0;

    while (u32ReadTotalLength < u32Length)
    {
        if (u32ReadTotalLength + ITE6563_RW_DATA_ONCE > u32Length)
        {
            u32ReadLength = u32Length - u32ReadTotalLength;
        }
        else
        {
            u32ReadLength = ITE6563_RW_DATA_ONCE;
        }

        readI2cByteArray(u32Offset + u32ReadTotalLength, u32ReadLength, pu8TempData);
        #if 1
        XLOGD("pu8TempData:0x%x:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", pu8TempData[0],
                pu8TempData[1], pu8TempData[2],
                pu8TempData[3], pu8TempData[4],
                pu8TempData[5], pu8TempData[6],
                pu8TempData[7], pu8TempData[8]);
        XLOGD("pu8Buff:0x%x:0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x ", pu8Buff[u32ReadTotalLength],
                pu8Buff[u32ReadTotalLength+1], pu8Buff[u32ReadTotalLength+2],
                pu8Buff[u32ReadTotalLength+3], pu8Buff[u32ReadTotalLength+4],
                pu8Buff[u32ReadTotalLength+5], pu8Buff[u32ReadTotalLength+6],
                pu8Buff[u32ReadTotalLength+7], pu8Buff[u32ReadTotalLength+8]);
        #endif

        if (memcmp(pu8TempData, &pu8Buff[u32ReadTotalLength], u32ReadLength) != 0)
        {
            XLOGE("[%s,%d] verifyData error", __FUNCTION__, __LINE__);
            XLOGE("[%s,%d] u32Offset + u32ReadTotalLength: 0x%x", __FUNCTION__, __LINE__, (u32Offset + u32ReadTotalLength));
            XLOGE("[%s,%d] u32ReadLength: 0x%x", __FUNCTION__, __LINE__, u32ReadLength);
            return XBH_FAILURE;
        }
        u32ReadTotalLength += u32ReadLength;
    }

    XLOGD("verifyData success!");
    return XBH_SUCCESS;
}

void ChipIte6563::run(const void* arg)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 u32FileSize = 0;
    XBH_U32 u32FwLength = 0;
    XBH_U32 u32FwEndLength = 0;
    XBH_U32 u32FwWriteOffset = 0;
    XBH_U32 u32FwWriteLength = 0;
    XBH_U32 u32FileCrc = 0;
    XBH_U32 u32GenFileCrc = 0;
    XBH_U8* pXBH_U8FileBuff = NULL;
    XBH_BOOL bIsUpdateHead = XBH_FALSE;
    XBH_U8  u8WriteCompleteFlag[8]={0x55, 0x44, 0xC3, 0x65, 0x16, 0xA0, 0x1A, 0x59};
    XBH_U8  u8FirewareVersion[16 + 1] = {0};
    FILE* pFile = NULL;

    //set task status
    mState = ITE6563_RUNNING;

    //---------------------- open ite6563 file ----------------------
    XLOGD("---> open ite6563 file ");
    pFile = fopen(mFileName, "rb");
    if (pFile == NULL)
    {
        XLOGE("Error:Open %s file fail!", mFileName);
        return ;
    }

    //---------------------- read upgrade file ----------------------
    XLOGD("---> read upgrade file ");
    fseek(pFile, 0, SEEK_END);
    u32FileSize = ftell(pFile);
    rewind(pFile);
    //XLOGD("[%s,%d] 1: s32FileSize: 0x%x", __FUNCTION__, __LINE__, u32FileSize);

    //File buff size(u32FwEndLength), do not less than ITE6563_END_LENGTH
    //The buff size except 0x18, requires 0x08 alignment.
    if (u32FileSize < ITE6563_END_LENGTH + 0x18)
    {
        u32FwEndLength = ITE6563_END_LENGTH + 0x18;
    }
    else
    {
        //buff size except top 0x18, requires 0x08 alignment.
        if ((u32FileSize & 0x07) != 0)
        {
            u32FwEndLength = (u32FileSize & ~0x7U) + 0x08;
        }
        else
        {
            u32FwEndLength = u32FileSize;
        }
    }

    pXBH_U8FileBuff = (XBH_U8 *)malloc(sizeof(XBH_U8) * u32FwEndLength);
    if (pXBH_U8FileBuff == NULL)
    {
        XLOGE("[%s,%d] pXBH_U8FileBuff malloc Error", __FUNCTION__, __LINE__);
        fclose(pFile);
        mState = ITE6563_FAILURE;
        return ;
    }
    memset(pXBH_U8FileBuff, 0x00, sizeof(XBH_U8) * u32FwEndLength);
    fread(pXBH_U8FileBuff, sizeof(XBH_U8), u32FileSize, pFile);

    //---------------------- compare size ----------------------
    XLOGD("---> compare size ");
    u32FwLength = ((pXBH_U8FileBuff[19] & 0xff) << 24)
                    | ((pXBH_U8FileBuff[18] & 0xff) << 16)
                    | ((pXBH_U8FileBuff[17] & 0xff) << 8)
                    | (pXBH_U8FileBuff[16] & 0xff);
    if ((u32FileSize - 0x18) != u32FwLength)
    {
        XLOGE("[%s,%d] File size fail!", __FUNCTION__, __LINE__);
        XLOGE("[%s,%d] u32FwLength: 0x%x", __FUNCTION__, __LINE__, u32FwLength);
        XLOGE("[%s,%d] s32FileSize: 0x%x", __FUNCTION__, __LINE__, u32FileSize);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- compare checksum ----------------------
    XLOGD("---> compare checksum ");
    u32FileCrc = ((pXBH_U8FileBuff[23] & 0xff) << 24)
                    | ((pXBH_U8FileBuff[22] & 0xff) << 16)
                    | ((pXBH_U8FileBuff[21] & 0xff) << 8)
                    | (pXBH_U8FileBuff[20] & 0xff);
    u32GenFileCrc = genCrc32(&pXBH_U8FileBuff[24], u32FwLength);

    if (u32GenFileCrc != u32FileCrc)
    {
        XLOGE("[%s,%d] Error check crc32 fail!", __FUNCTION__, __LINE__);
        XLOGE("[%s,%d] u32FileCrc: 0x%x", __FUNCTION__, __LINE__, u32FileCrc);
        XLOGE("[%s,%d] u32GenFileCrc: 0x%x", __FUNCTION__, __LINE__, u32GenFileCrc);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- enter isp mode ----------------------
    XLOGD("---> enter isp mode ");
    s32Ret = exitIspMode();//remove last status
    s32Ret |= enterIspMode();
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] enterIspMode fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- enable interrupt ----------------------
    XLOGD("---> enable interrupt ");
    s32Ret = enableInterruptMask();
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] enableInterruptMask fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- check version ----------------------
    XLOGD("---> check version ");
    s32Ret = readI2cByteArray(ITE6563_VERSION_OFFSET, ITE6563_VERSION_LENGTH, u8FirewareVersion);
    if (memcmp(u8FirewareVersion, &pXBH_U8FileBuff[ITE6563_VERSION_OFFSET + ITE6563_FILE_WRITE_OFFSET], ITE6563_VERSION_LENGTH) == 0)
    {
        if (!mForceUpgrade)
        {
            XLOGD("[%s,%d] fireware version is same. No need upgrade!", __FUNCTION__, __LINE__);
            XLOGD("[%s,%d] VERSION: %s", __FUNCTION__, __LINE__, u8FirewareVersion);
            free(pXBH_U8FileBuff);
            fclose(pFile);
            exitIspMode();
            mState = ITE6563_SUCCESS;
            return;
        }
        else
        {
            XLOGD("[%s,%d] fireware version is same!", __FUNCTION__, __LINE__);
            XLOGD("[%s,%d] force upgrade!", __FUNCTION__, __LINE__);
        }
    }

    // //---------------------- check header ----------------------
    // XLOGD("---> check header ");
    // s32Ret = checkHead(pXBH_U8FileBuff);
    // if (s32Ret != XBH_SUCCESS)
    // {
    //     bIsUpdateHead = XBH_TRUE;
    // }

    //---------------------- erase upgrade flag ----------------------
    XLOGD("---> erase upgrade flag ");
    s32Ret = eraseSector(ITE6563_UPGRADE_START, ITE6563_UPGRADE_END);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] eraseSector upgrade flag fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- erase emmc ----------------------
    XLOGD("---> erase emmc ");
    if (bIsUpdateHead)
    {
        s32Ret = eraseSector(ITE6563_HEAD_OFFSET, ITE6563_EMMC_END);// erase emmc
    }
    else
    {
        s32Ret = eraseSector(ITE6563_BODY_OFFSET, ITE6563_EMMC_END);// erase emmc
    }

    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] eraseSector borad fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- write data ----------------------
    XLOGD("---> write data ");
    if (bIsUpdateHead)
    {
        u32FwWriteOffset = ITE6563_FILE_WRITE_OFFSET;
        u32FwWriteLength = u32FwEndLength;
        s32Ret = writeData(ITE6563_HEAD_OFFSET, u32FwWriteLength, &pXBH_U8FileBuff[u32FwWriteOffset]);
    }
    else
    {
        u32FwWriteOffset = ITE6563_FILE_WRITE_OFFSET + ITE6563_HEAD_LENGTH;
        u32FwWriteLength = u32FwEndLength - ITE6563_HEAD_LENGTH;
        s32Ret = writeData(ITE6563_BODY_OFFSET, u32FwWriteLength, &pXBH_U8FileBuff[u32FwWriteOffset]);
    }
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] write data fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- verify data ----------------------
    XLOGD("---> verify data ");
    if (bIsUpdateHead)
    {
        XLOGD("---> ITE6563_HEAD_OFFSET verify data ");
        s32Ret = verifyData(ITE6563_HEAD_OFFSET, u32FwWriteLength, &pXBH_U8FileBuff[u32FwWriteOffset]);
    }
    else
    {
        XLOGD("---> ITE6563_BODY_OFFSET verify data ");
        s32Ret = verifyData(ITE6563_BODY_OFFSET, u32FwWriteLength, &pXBH_U8FileBuff[u32FwWriteOffset]);
    }
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] verify data fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- write complete flag ----------------------
    XLOGD("---> write complete flag ");
    s32Ret = writeData(0x2060, sizeof(u8WriteCompleteFlag), u8WriteCompleteFlag);
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] write complete flag fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        exitIspMode();
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- exit isp mode ----------------------
    XLOGD("---> exit isp mode ");
    s32Ret = exitIspMode();
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("[%s,%d] exit isp mode fail!", __FUNCTION__, __LINE__);
        free(pXBH_U8FileBuff);
        fclose(pFile);
        mState = ITE6563_FAILURE;
        return;
    }

    //---------------------- upgrade finish ----------------------
    XLOGD("upgrade finish!");
    free(pXBH_U8FileBuff);
    fclose(pFile);
    //需要重新拿版本号。
    memset(mVersion, 0, sizeof(mVersion));
    mState = ITE6563_SUCCESS;
}
//---------------- custom device interface end ----------------

ChipIte6563::ChipIte6563(XBH_U32 i2cBus, XBH_U32 Addr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mState = ITE6563_IDLE;
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
    memset(mVersion, 0, sizeof(mVersion));
    usleep(100 * 1000);
    getFirmwareVersion(mVersion);

    XLOGD(" end ");
}

ChipIte6563::~ChipIte6563()
{

}

