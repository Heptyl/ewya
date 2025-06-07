#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@mtGd32f310g8"
#include "XbhLog.h"

#include "XbhService.h"
#include "mtGd32f310g8.h"
#include <iostream>
#include <cstring>
#include <fstream>

XbhMutex                 mtGd32f310g8::mLock;

static XBH_U32 CRC32_TAB[256] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,

    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,

    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,

    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,

    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,

    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,

    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,

    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

XBH_S32 mtGd32f310g8::WriteMcuMultiBytes(XBH_U8 s32RegAddr, XBH_U32 u32Length,  XBH_U8 *pu8Data)
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

XBH_S32 mtGd32f310g8::ReadMcuMultiBytes(XBH_U8 s32RegAddr, XBH_U32 u32Length, XBH_U8* pu8Data)
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

/**
 * 复位mcu芯片
*/
XBH_S32 mtGd32f310g8::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //XbhMutex::Autolock _l(mLock);
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
XBH_S32 mtGd32f310g8::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 mtGd32f310g8::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * CRC 校验
*/
//1、校验 预置mcu固件CRC
XBH_U32 mtGd32f310g8::getPresetMcuFirewareCrc32(XBH_VOID* pStart, XBH_S32 s32Size)
{
    /* init the start value */
    XBH_U32 u32CRCValue = 0xffffffff;
    XBH_U8 *pData;
	if(pStart == NULL)
	{
		XLOGE("Func[%s] BUFF null\n", __func__);
		return 0;
	}
	pData = (XBH_U8 *)pStart;
    /* calculate CRC */
    while(s32Size--)
    {
        u32CRCValue = CRC32_TAB[(u32CRCValue ^ *pData++) & 0xFF] ^ (u32CRCValue >> 8);
    }
    /* XOR the output value */
    return (u32CRCValue ^ ~0U);
}

/*获取SOL附带mcu固件信息*/
//1、查找文件是否存在
XBH_S32 mtGd32f310g8::getPresetMcuFireware(XBH_CHAR* path, XBH_CHAR* pFileName)
{
	DIR *dir;
	struct dirent *ptr;
	XBH_S32 s32Ret = XBH_FAILURE;

	memset(pFileName, 0, strlen(pFileName));
	if ((dir = opendir(path)) == NULL)
	  return s32Ret;

	while ((ptr = readdir(dir)) != NULL)
	{
	  if(strcmp(ptr->d_name, ".")==0 || strcmp(ptr->d_name, "..")==0){
		continue;
	  }else if(ptr->d_type == 8){//file
		if(strncmp(ptr->d_name, "mcu_", 3) == 0){
		  if(strstr(ptr->d_name, ".bin") != NULL){
			sprintf(pFileName, "%s/%s", path, ptr->d_name);
			s32Ret = XBH_SUCCESS;
		  }
		}
	  }else if(ptr->d_type == 10){//link file
	   ;//printf("%s/%s\n", path, ptr->d_name);
	  }
	}

	closedir(dir);
	usleep(100 * 1000);
	return s32Ret;
}

//2、获取文件版本信息         (目前使用文件名称判断：mcu_GD32F310G8_full_TIME_Vxxx_time.bin)
XBH_S32 mtGd32f310g8::getPresetMcuVersion(XBH_CHAR* pFileName)
{
	XBH_CHAR ch1 = 'V';
	XBH_CHAR* pf;
	XBH_S32 s32Ret = XBH_FAILURE;

	pf = strrchr(pFileName, ch1);
	if(pf == NULL){
		return XBH_FAILURE;
	}
	s32Ret = atoi(pf+1);

	XLOGD("func[%s]  [V%d.%d]\n", __func__, ((s32Ret >> 8) & 0xFF), (s32Ret & 0xFF));
	return s32Ret;
}

/* 升级操作 */

//1.1、读取升级固件数据
XBH_S32 mtGd32f310g8::getPresetMcuData(XBH_U32* u32FileLen, XBH_U8** pFileBuff)
{
	XBH_U32 u32FileSize = 0;
	FILE *pFile =  NULL;
	XBH_U8* pU8FileBuff = NULL;
	//---------------------- open gd32 file ----------------------
    XLOGD("---> open Preset_file \n");
    if ((pFile = fopen(strPresetfilepath, "rb")) == NULL){
		XLOGE("Error:Open %s file fail!", strPresetfilepath);
        return XBH_FAILURE;
    }

    //---------------------- read upgrade file ----------------------
    XLOGD("---> read upgrade file \n");
    fseek(pFile, 0, SEEK_END);
    u32FileSize = ftell(pFile);
    rewind(pFile);
    if(u32FileSize <= 0)
    {
        XLOGE("Func[%s] - Line[%d] pU8FileSize  ", __func__, __LINE__);
        fclose(pFile);
        return XBH_FAILURE;
    }

    pU8FileBuff = (XBH_U8 *)malloc(sizeof(XBH_U8) * u32FileSize);
    if (pU8FileBuff == NULL)
    {
        XLOGE("Func[%s] - Line[%d] pU8FileBuff malloc Error", __func__, __LINE__);
        fclose(pFile);
        return XBH_FAILURE;
    }
    memset(pU8FileBuff, 0x00, sizeof(XBH_U8) * u32FileSize);
    fread(pU8FileBuff, sizeof(XBH_U8), u32FileSize, pFile);

	XLOGD("---> close upgrade file \n");
    fclose(pFile);

	*u32FileLen = u32FileSize;
	*pFileBuff = pU8FileBuff;

	usleep(100 * 1000);
    return XBH_SUCCESS;
}

//1.2、获取mcu运行版本信息
XBH_S32 mtGd32f310g8::getExtMcuAppVersion()
{
	XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 version = 0;
    XBH_CHAR strVer[10]="0";
    s32Ret = getMcuVersion(&version);

	if(s32Ret != XBH_SUCCESS){
		//防止NFC连接异常或未接线状态无法升级
		usleep(3 * 1000 * 1000);
		s32Ret = getMcuVersion(&version);
	}
    if(s32Ret != XBH_SUCCESS){
        XLOGE("Func[%s] - Line[%d] getMcuVersion Fail",__func__, __LINE__);
        return s32Ret;
    }

    XLOGD("func[%s] [V%d.%d]\n", __func__, ((version>>8) & 0xFF), (version & 0xFF));
	sprintf(strVer, "%d.%d", ((version>>8) & 0xFF), (version & 0xFF));

	return version;
}

//获取当前Mcu升级状态
XBH_S32 mtGd32f310g8::getUpgradeState(XBH_S32* s32State)
{
	XBH_S32 s32Ret = XBH_SUCCESS;
	*s32State = mState;

	return s32Ret;
}

//2、mcu进入Iap模式
XBH_S32 mtGd32f310g8::setEnterIapMode()
{
	XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S stInData;

	stInData.cmd = CMD_I2C_ENTER_IAP;
    stInData.len = 1;
	stInData.data[0] = 0x01;
	XLOGI("[%s] Set MCU Enter Iap Mode\n", __func__);
	s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, stInData.cmd, 0x01, stInData.len, stInData.data);
	return s32Ret;
}

//3、写入mcu数据
XBH_S32 mtGd32f310g8::downloadPresetFirewareData(XBH_U8* pFileBuff, XBH_U32 u32FileLen)
{
	XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 s32RegAddr = CMD_I2C_WRITE_IAP_DATA;

    XBH_U32 u32Address = 0;
    XBH_U32 u32FileSize = u32FileLen;
    XBH_U32 u32FileIndex = 0;
    XBH_U32 u32DataLen = 0;
    XBH_U8  u8TmpBuff[128] = {0};

	XLOGD("---> write data u32FileSize = %d\n", u32FileSize);

   while(u32FileSize > 0)
   {
        if(u32FileSize >= 64){
            u32DataLen = 64;
        } else {
           u32DataLen = u32FileSize;
        }
       memset(u8TmpBuff, 0xff, sizeof(u8TmpBuff));
       u8TmpBuff[0] = (u32DataLen >>  8) & 0xff;
       u8TmpBuff[1] = (u32DataLen >>  0) & 0xff;
       u8TmpBuff[2] = (u32Address >> 24) & 0xff;
       u8TmpBuff[3] = (u32Address >> 16) & 0xff;
       u8TmpBuff[4] = (u32Address >>  8) & 0xff;
       u8TmpBuff[5] = (u32Address >>  0) & 0xff;
       //memcpy(&u8TmpBuff[6], &pFileBuff[u32FileIndex], 64);
       memcpy(&u8TmpBuff[6], &pFileBuff[u32FileIndex], u32DataLen);
       s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr,s32RegAddr, 0x01, u32DataLen+6, u8TmpBuff);
    //    if(s32Ret == XBH_SUCCESS){
    //        break;
    //    }
        if (s32Ret != XBH_SUCCESS) {
            XLOGE("Error: upgrade_download_data Error!");
        }

       if((u32Address % 1024) == 0){
           usleep(50 * 1000);
       } else {
           usleep(20 * 1000);
       }

       u32Address += u32DataLen;
       u32FileIndex += u32DataLen;
       u32FileSize -= u32DataLen;
   }

   usleep(100 * 1000);

   if (s32Ret == XBH_SUCCESS){
       XLOGD("Func[%s] - Line[%d] success!\n", __func__, __LINE__);
   } else {
       XLOGE("Func[%s] - Line[%d] Error: writeData fail u32Address:0x%x, u32FileIndex:0x%x!\n", __func__, __LINE__, u32Address, u32FileIndex);
   }

   return s32Ret;
}

//4、发送数据长度
XBH_S32 mtGd32f310g8::checkPresetFirewareLen(XBH_U32 u32FileLen)
{
	XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S stInData;
	XBH_U32 u32ReadFileLen = 0;
	stInData.cmd = CMD_I2C_WRITE_IAP_APP_LENGTH;
    stInData.len = 2;
	stInData.data[0] = ((u32FileLen >> 8) & 0xFF);
	stInData.data[1] = (u32FileLen & 0xFF);
	XLOGD("func[%s] send PresetFirewareLen:[%u]\n", __func__, u32FileLen);
	s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, stInData.cmd, 0x01, stInData.len, stInData.data);
	if(s32Ret == XBH_SUCCESS)
	{
		usleep(100 * 1000);
		memset(stInData.data, 0, MCU_I2C_BUFF_SIZE);
		s32Ret |= XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, stInData.cmd, 0x01, stInData.len+2, stInData.data);
	}

    if (s32Ret == XBH_SUCCESS){
		u32ReadFileLen = (stInData.data[2] << 8) | (stInData.data[3]);
		XLOGD("func[%s] get ExtMcuFirewareLen:[%u]\n", __func__, u32ReadFileLen);
		if(u32ReadFileLen != u32FileLen){
			XLOGE("Error: setAppLength fail!\n");
			s32Ret = XBH_FAILURE;
		}
        XLOGD("Func[%s] - Line[%d] success!\n", __func__, __LINE__);
    } else {
        XLOGE("Func[%s] - Line[%d] Error: setAppLength fail!\n", __func__, __LINE__);
        s32Ret = XBH_FAILURE;
    }

	//usleep(100 * 1000);
	return s32Ret;
}

//5、检测更新Mcu文件的CRC
XBH_S32 mtGd32f310g8::checkExtMcuCrc(XBH_U32 u32PresetFileCrc)
{
	XBH_S32 s32Ret = XBH_SUCCESS;
	XBH_U32 u32McuCrc = 0;
	XBH_MCU_I2CBUFFDEF_S stInData;

	stInData.cmd = CMD_I2C_READ_IAP_CRC32;
	stInData.len = 6;
	XLOGD("func[%s] Check ExtMcu Crc\n", __func__);
	s32Ret |= XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, stInData.cmd, 0x01, stInData.len, stInData.data);

	if(s32Ret == XBH_SUCCESS)
	{
		usleep(50 * 1000);
		u32McuCrc =( (stInData.data[2]<<24) | (stInData.data[3]<<16) | (stInData.data[4]<<8) | (stInData.data[5]) );
		XLOGD("func[%s] Check ExtMcu Crc:[%u] : ProsetFireware crc:[%u]\n", __func__, u32McuCrc, u32PresetFileCrc);
		if(u32McuCrc != u32PresetFileCrc){
			XLOGE("Func[%s] - Line[%d] ERROR: Check CRC Fail\n", __func__, __LINE__);
			s32Ret = XBH_FAILURE;
		}
	}

	return s32Ret;
}

//4、重启mcu
XBH_S32  mtGd32f310g8::setIapReboot()
{
	XBH_S32 s32Ret = XBH_SUCCESS;
	XBH_MCU_I2CBUFFDEF_S stInData;

	stInData.cmd = CMD_I2C_WRITE_IAP_VALID;
	stInData.len = 1;
	stInData.data[0] = 0x01;
	XLOGD("func[%s] Reboot mcu\n", __func__);
	s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, stInData.cmd, 0x01, stInData.len, stInData.data);

	if(s32Ret != XBH_SUCCESS){
		XLOGE("Func[%s] - Line[%d] reboot ExtMcu fail\n", __func__, __LINE__);
	}
	usleep(100 * 1000);
	return s32Ret;
}

//升级执行入口
XBH_S32 mtGd32f310g8::MCUUpgrade(XBH_BOOL bForceUpgrade)
{
	XBH_U32 u32PresetFileVer = 0;
	XBH_U32 u32ExtMcuVer = 0;
	XBH_S32 s32Ret  = XBH_SUCCESS;
	XBH_U8* pPresetFile = NULL;
	XBH_U32 u32PresetFileLen = 0;
	XBH_U32 u32PresetFileCrc = 0;

	mState = GD32_IDLE;
	//自动升级模式下，获取默认系统升级路径下升级包检索
	// if(strlen(strPresetfilepath) == 0)
	// {
	// 	s32Ret |= getPresetMcuFireware(ExtMcu_DEFAULT_FIREWARE_PATH,  strPresetfilepath);
	// 	if(s32Ret != XBH_SUCCESS)
	// 	{
	// 		XLOGE("Func[%s] - Line[%d] Get PresetMcu Fireware Fail\n", __func__, __LINE__);
	// 		return s32Ret;
	// 	}
	// }

	XLOGD("[%d] Preset File path:[%s]\n", __LINE__, strPresetfilepath);

	s32Ret = getExtMcuAppVersion();
	if(s32Ret == XBH_FAILURE)
	{
		XLOGE("Func[%s] - Line[%d]  Get ExtMcu Version Error\n", __func__, __LINE__);
		return s32Ret;
	}
	u32ExtMcuVer = s32Ret;

	s32Ret = getPresetMcuVersion(strPresetfilepath);
	if(s32Ret == XBH_FAILURE)
	{
		XLOGE("Func[%s] - Line[%d] Get Preset_Mcu Version Error\n", __func__, __LINE__);
		return s32Ret;
	}
	u32PresetFileVer = s32Ret;
	if((u32PresetFileVer <= u32ExtMcuVer) && (u32ExtMcuVer != EXT_MCU_DEF_VERSION))
	{
		XLOGE("Func[%s] - Line[%d] Check Version ExtMcuVer:[%d]   PresetFileVer:[%d]\n", __func__, __LINE__, u32ExtMcuVer, u32PresetFileVer);
        if(bForceUpgrade == XBH_FALSE){
            s32Ret = XBH_FAILURE;
            goto MCUUpgradeFail;
        }
	}

	mState = GD32_RUNNING;
	s32Ret = XBH_SUCCESS;

	//Mcu 非崩溃状态升级前操作
	if(u32ExtMcuVer != EXT_MCU_DEF_VERSION)
	{
		XLOGD("Set Iap Mode\n");
		s32Ret |= setEnterIapMode();
		if(s32Ret != XBH_SUCCESS)
		{
			XLOGE("Func[%s] - Line[%d] Set Iap Mode fail\n",__func__, __LINE__);
			goto MCUUpgradeFail;
		}
	}
	usleep(1000 * 1000);
	s32Ret |= getPresetMcuData(&u32PresetFileLen, &pPresetFile);
	if(s32Ret != XBH_SUCCESS){
		goto MCUUpgradeFail;
	}

	XLOGD("Read Preset Fireware Len:[%d]\n", u32PresetFileLen);
	s32Ret |= downloadPresetFirewareData(pPresetFile, u32PresetFileLen);
	if(s32Ret != XBH_SUCCESS)
	{
		free(pPresetFile);
		goto MCUUpgradeFail;
	}

	XLOGD("Download Fireware Finish\n");
	s32Ret |= checkPresetFirewareLen(u32PresetFileLen);
	if(s32Ret != XBH_SUCCESS)
	{
		XLOGE("Func[%s] - Line[%d] Check Fireware Len Fail !!!!\n", __func__, __LINE__);
		free(pPresetFile);
		goto MCUUpgradeFail;
	}

	u32PresetFileCrc = getPresetMcuFirewareCrc32((XBH_VOID* )pPresetFile, u32PresetFileLen);
	free(pPresetFile);
	s32Ret |= checkExtMcuCrc(u32PresetFileCrc);
	if(s32Ret != XBH_SUCCESS)
	{
		goto MCUUpgradeFail;
	}

	s32Ret |= setIapReboot();
	if(s32Ret != XBH_SUCCESS)
	{
		goto MCUUpgradeFail;
	}
	usleep(500 * 1000 * 4);
	s32Ret |= getExtMcuAppVersion();
	if(s32Ret == XBH_FAILURE)
	{
		XLOGE("Func[%s] - Line[%d] Get Upgrade ExtMcu Version Error\n", __func__, __LINE__);
		goto MCUUpgradeFail;
	}
    s32Ret = XBH_SUCCESS;

	mState = GD32_SUCCESS;
	return s32Ret;

MCUUpgradeFail:
	mState = GD32_FAILURE;
	return s32Ret;
}

XBH_S32 mtGd32f310g8::mcuReadBytes(XBH_MCU_I2CBUFFDEF_S* pI2cBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;

    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff->cmd, 0x01, pI2cBuff->len, pI2cBuff->data);

    return s32Ret;
}

/**
 * 获取MCU软件版本
 */
XBH_S32 mtGd32f310g8::getMcuVersion(XBH_S32 *mcuVer)
{
    //XbhMutex::Autolock _l(mLock);

    XBH_MCU_I2CBUFFDEF_S stInData;
    stInData.cmd = CMD_I2C_GET_APP_VER;
    stInData.len = 2;
    XLOGE("Func[%s] - Line[%d] yifan_getMcuVersion",__func__, __LINE__);
    if (mcuReadBytes(&stInData) == XBH_SUCCESS)
    {
        *mcuVer = (stInData.data[0] <<8)|stInData.data[1];
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}
/**
 * 获取升级固件版本
 */
XBH_S32 mtGd32f310g8::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 mcuVer = 0;
    unsigned char buf[3] = {0x00};

    //XbhMutex::Autolock _l(mLock);

    s32Ret = getMcuVersion(&mcuVer);

	if(s32Ret != XBH_SUCCESS){
		//防止NFC连接异常或未接线状态无法升级
		usleep(3 * 1000 * 1000);
		s32Ret = getMcuVersion(&mcuVer);
	}
    if(s32Ret != XBH_SUCCESS){
        XLOGE("Func[%s] - Line[%d] getMcuVersion Fail",__func__, __LINE__);
        return s32Ret;
    }

    XLOGD("func[%s] [V%d.%d]\n", __func__, ((mcuVer>>8) & 0xFF), (mcuVer & 0xFF));
	sprintf(strVersion, "%d.%d", ((mcuVer>>8) & 0xFF), (mcuVer & 0xFF));

    return s32Ret;
}
/**
 * 进行固件升级
 */
XBH_S32 mtGd32f310g8::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    //XbhMutex::Autolock _l(mLock);
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
    strncpy(strPresetfilepath, strFilePath, strlen(strFilePath));

    mUpgradeStatus = 1;

    XbhMWThread::run(XbhMWThread::ONCE);

    return XBH_SUCCESS;
}

XBH_S32 mtGd32f310g8::setMcuFattMode(XBH_S32 mode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 pu8Data[1];
    pu8Data[0] = (XBH_U8)mode;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_FATT_MODE, 1, pu8Data);
    return s32Ret;
}

XBH_S32 mtGd32f310g8::getMcuFattMode(XBH_S32 *mode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 buf[64] = {0x00};
    s32Ret = ReadMcuMultiBytes(CMD_I2C_GET_FATT_MODE, 1, buf);
    *mode = buf[0];
    return s32Ret;
}

XBH_S32 mtGd32f310g8::setMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
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

XBH_S32 mtGd32f310g8::getMcuIIcBypass(XBH_U8 u8IIcNum, XBH_U8 u8DeviceAddr, XBH_U8 u8RegAddr, XBH_U8 u8Len, XBH_U8* u8Data)
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

XBH_S32 mtGd32f310g8::setMcuUartBypassPara(XBH_U8 u8UartNum, XBH_U8 baud)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 param[2] = {0x00};
    param[0] = u8UartNum;
    param[1] = baud;
    s32Ret = WriteMcuMultiBytes(CMD_I2C_SET_BYPASS_UART_PARA, 2, param);
    XLOGE("setMcuUartBypassPara u8UartNum:%d, baud:0x%02x \n", u8UartNum, baud);
    return s32Ret;
}

XBH_S32 mtGd32f310g8::setMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
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

XBH_S32 mtGd32f310g8::getMcuUartBypass(XBH_U8 u8Len, XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = ReadMcuMultiBytes(CMD_I2C_GET_BYPASS_UART, u8Len, u8Data) ;
    XLOGE("getMcuUartBypass u8Len\n", u8Len);
    return s32Ret;
}

void mtGd32f310g8::run(const void* arg)
{
    //XbhMutex::Autolock _l(mLock);
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (mUpgradeStatus == 1)
    {
        s32Ret = MCUUpgrade(mForceUpgrade);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGD("upgrade error!");
        }
        mUpgradeStatus = 0;
    }
}

mtGd32f310g8::mtGd32f310g8(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
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

mtGd32f310g8::~mtGd32f310g8()
{

}
