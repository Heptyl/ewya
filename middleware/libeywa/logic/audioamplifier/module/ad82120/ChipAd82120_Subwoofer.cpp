#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipAd82120_Subwoofer"
#include "XbhLog.h"

#include <iostream>
#include <fstream>
#include "XbhService.h"
#include "ChipAd82120_Subwoofer.h"

XbhMutex                 ChipAd82120_Subwoofer::mLock;

/* register address */
#define REG_DEVICE_ADDRESS	0x02

/* DEVICE_STATE register values */
#define DEVICE_STATE_MUTE	0x40
#define DEVICE_STATE_UNMUTE	0x00

/**
 * 复位功放芯片
*/
XBH_S32 ChipAd82120_Subwoofer::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

/**
 * 设置speaker 软mute的状态
*/
XBH_S32 ChipAd82120_Subwoofer::setMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    data[0] =(mute ? DEVICE_STATE_MUTE : DEVICE_STATE_UNMUTE);
    XLOGD("setSubMute addr: %#x,MUTE :%d\n",mI2cAddr,mute);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_DEVICE_ADDRESS, 0x01, 0x01, data);
    return  s32Ret;
}

/**
 * 获取speaker mute的状态
*/
XBH_S32 ChipAd82120_Subwoofer::getMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr,REG_DEVICE_ADDRESS, 0x01, 0x01, data);
    if( data[0] ==  DEVICE_STATE_MUTE)
    {
        *mute = XBH_TRUE;
    }
    else
    {
        *mute = XBH_FALSE;
    }
    return  s32Ret;
}

/**
 * 设置功放输出的增益值
*/
XBH_S32 ChipAd82120_Subwoofer::setGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 ChipAd82120_Subwoofer::getGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取amp是否初始化完成
*/
XBH_S32 ChipAd82120_Subwoofer::getChipInitDone(XBH_BOOL* enable)
{
    if(m_bInitDone)
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    return XBH_SUCCESS;
}
/**
 * 获取chip是否存在
*/
XBH_S32 ChipAd82120_Subwoofer::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 ChipAd82120_Subwoofer::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 进行固件升级
 */
XBH_S32 ChipAd82120_Subwoofer::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 获取固件升级时的状态
 */
XBH_S32 ChipAd82120_Subwoofer::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 ChipAd82120_Subwoofer::Ad82120_Write(XBH_U8 *buf, XBH_U8 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipAd82120_Subwoofer::Ad82120_Read(XBH_U8 *buf, XBH_U8 ctrl)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipAd82120_Subwoofer::writeRegMap(ampRegMap *reg_map, XBH_S32 iicBus, XBH_S32 iicAddr)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U32 addr;
    XBH_U32 size;
    XBH_U8 delay_ms;
    XBH_U8 *reg=NULL;
    while (1)
    {
        if (reg_map == NULL)
        {
            XLOGE("writeRegMap reg_map err\n");
            return XBH_FAILURE;
        }
        if((reg_map->addr == 0xff))
        {
            XLOGI("writeRegMap successed\n");
            return XBH_SUCCESS;
        }

        addr = reg_map->addr;
        reg = reg_map->reg;
        size = reg_map->size;
        delay_ms = reg_map->delay_ms;
        ret = XbhService::getModuleInterface()->setI2cData(iicBus, iicAddr ,addr, 1, size, reg);
        if (ret != XBH_SUCCESS)
        {
            XLOGE("writeRegMap ret = %d\n",ret);
            return XBH_FAILURE;
        }

        if (delay_ms != 0)
        {
            usleep(delay_ms);
        }
        reg_map++;
    }
    return XBH_SUCCESS;
}

XBH_S32 ChipAd82120_Subwoofer::writeEqRegMap(ampRegMap *reg_map, int *eq_reg, int value, XBH_S32 iicBus, XBH_S32 iicAddr)
{
    XBH_S32 ret = XBH_FAILURE;
    XBH_U8 addr;
    XBH_U32 size;
    XBH_U8 delay_ms;
    XBH_U8 *reg = NULL;
    XBH_U8 val = value;
    if (eq_reg == NULL)
    {
        return XBH_FAILURE;
    }
    while (1)
    {
        if (reg_map == NULL)
        {
            XLOGE("writeEqRegMap reg_map err\n");
            return XBH_FAILURE;
        }
        if((reg_map->addr == 0xff))
        {
            XLOGI("writeEqRegMap successed\n");
            return XBH_SUCCESS;
        }
        addr = reg_map->addr;
        reg = reg_map->reg;
        size = reg_map->size;
        delay_ms = reg_map->delay_ms;
        if (size > 0)
        {
            ret = XbhService::getModuleInterface()->setI2cData(iicBus, iicAddr, eq_reg[0],1, 1 ,&addr);
            if (ret != XBH_SUCCESS)
            {
                XLOGE("writeEqRegMap ret = %d bus = 0x%x addr = 0x%x\n",ret,iicBus,iicAddr);
                return XBH_FAILURE;
            }
            for (int i=0; i<size; i++)
            {
                ret = XbhService::getModuleInterface()->setI2cData(iicBus, iicAddr, eq_reg[i+1], 1, 1,&reg[i]);
                if (ret != XBH_SUCCESS)
                {
                    XLOGE("writeEqRegMap eq_reg[i+1] err\n");
                    return XBH_FAILURE;
                }
            }
            ret = XbhService::getModuleInterface()->setI2cData(iicBus, iicAddr, eq_reg[size+1], 1, 1, &val);
            if (ret != XBH_SUCCESS)
            {
                XLOGE("writeEqRegMap eq_reg[size+1] err\n");
                return XBH_FAILURE;
            }
            if (delay_ms != 0)
            {
                usleep(delay_ms);
            }
        }
        reg_map++;
    }
    return XBH_SUCCESS;
}


XBH_S32 ChipAd82120_Subwoofer::ParseIniConfig(const char *section, char *fileBuf, ampRegMap *pRegMap, XBH_U32 maxRegSize)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (section == NULL || fileBuf == NULL || pRegMap == NULL)
    {
        XLOGE("ParseFile:param error.\n");
        return XBH_FAILURE;
    }
    char *buf = fileBuf;
    char *tempBuf = NULL;
    XBH_U32 i = 0;
    XBH_U32 k = 0;
    XBH_U32 m = 0;
    XBH_U32 sectionLen = strlen(section);
    XBH_U32 isDiffSection = XBH_FALSE;

    tempBuf = strstr(buf, section);
    if (tempBuf == NULL)
    {
        XLOGE("ParseFile %s\n",section);
        return XBH_FAILURE;
    }
    i = tempBuf - buf;

    while (IsEnd(buf[i]) == XBH_FALSE)
    {
        while (!((IsNewLine(*(buf + i - 1))) && (IsLeftBracket(*(buf + i)))) &&(!IsEnd(*(buf + i)))) 
        {
            while ((!IsNewLine(*(buf + i))) &&(!IsEnd(*(buf + i))))
            {
                XBH_U32 j = 0;
                //取0x之后的数值
                if ((*(buf + i))=='0' && ((*(buf + i + 1))=='x' || (*(buf + i + 1))=='X'))
                {
                    int value = strtol(buf + i,NULL,0); //如果无法转换转换长整型整数类型(遇到ab 等 输出ab)
                    if (pRegMap[k].addr == 0xff)
                    {
                        pRegMap[k].addr = value;
                        pRegMap[k].delay_ms = 2;
                    }
                    else if (pRegMap[k].size == 0xff)
                    {
                        pRegMap[k].size = value;
                        m = 0;
                    }
                    else if (m < pRegMap[k].size)
                    {
                        pRegMap[k].reg[m] = value;
                        m++;
                        if (m == pRegMap[k].size)
                        {
                            k++;
                            if (k >= maxRegSize)
                            {
                                XLOGE("ParseFile too big\n");
                                return XBH_FAILURE;
                            }
                        }
                    }
                    i += 2;
                }
                else if((*(buf + i))=='E' && (*(buf + i +1 ))=='N' && (*(buf + i+2))=='D')
                {
                    pRegMap[k].addr = 0xff;
                    pRegMap[k].size = 0x01;
                    pRegMap[k].reg[0] = 0x00;
                    pRegMap[k].delay_ms = 0;
                    XLOGE("ParseFile end\n");
                    return XBH_SUCCESS;
                }
                i++;
            }
            i++;
        }
        i++;
    }
    return XBH_FAILURE;
}


XBH_S32 ChipAd82120_Subwoofer::LoadIniFileBuf(const XBH_CHAR *fileName, XBH_CHAR *buf, XBH_S32 *fileSize, XBH_S32 bufSize)
{
    if (fileName == NULL || buf == NULL || fileSize == NULL)
    {
        XLOGE("LoadIniFile:param error.\n");
        return XBH_FAILURE;
    }
    XBH_S32 file = 0;
    file = open(fileName, O_RDONLY);
    if (file < 0)
    {
        XLOGE("LoadIniFile:open file %s failed.\n", fileName);
        return XBH_FAILURE;
    }
    std::ifstream in(fileName);
    in.read(buf, bufSize);
    close(file);
    return XBH_SUCCESS;
}

XBH_S32 ChipAd82120_Subwoofer::WriteIniConfig(const XBH_CHAR *fileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 mutePaAddr = (XBH_U32)0x02;
    XBH_U8 mutePaValue = 0x40;
    XBH_S32 filesize = 0;
    XBH_CHAR *pRawBuffer = (XBH_CHAR *)malloc(MAX_FILE_SIZE);
    s32Ret = LoadIniFileBuf(fileName, pRawBuffer, &filesize, MAX_FILE_SIZE);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGE("WIniConfig(No file was found to replace)\n");
        free(pRawBuffer);
        return s32Ret;
    }
    //mute amp
    setMute(XBH_TRUE);
    usleep(20*1000);
    
    ampRegMap *pAd82120InitReg = (ampRegMap *)malloc(sizeof(ampRegMap) * AMP_MAX_REG_NUM);
    memset(pAd82120InitReg, 0xff, sizeof(ampRegMap) * AMP_MAX_REG_NUM);
    if(ParseIniConfig(AUDIO_INI_AD82120_BASS_INIT_REG, pRawBuffer, pAd82120InitReg, AMP_MAX_REG_NUM) == XBH_SUCCESS)
    {
        s32Ret = writeRegMap(pAd82120InitReg, mI2cBus, mI2cAddr);
        if(s32Ret == XBH_FAILURE)
        {
            goto Error;
        }
    }
    else
    {
        XLOGE("g_ad82120_init_reg(No file was found to replace)\n");
        goto Error;
    }
    usleep(20*1000);
    memset(pAd82120InitReg, 0xff, sizeof(ampRegMap) * AMP_MAX_REG_NUM);
    if (ParseIniConfig(AUDIO_INI_AD82120_BASS_RAM1_TAB, pRawBuffer, pAd82120InitReg, AMP_MAX_REG_NUM) == XBH_SUCCESS)
    {
        s32Ret = writeEqRegMap(pAd82120InitReg, g_ad82120_eq_reg, 0x01, mI2cBus, mI2cAddr);
        if(s32Ret == XBH_FAILURE)
        {
            goto Error;
        }
    }
    else
    {
        XLOGE("g_ad82120_ram1_tab(No file was found to replace)\n");
        goto Error;
    }
    usleep(20*1000);
    memset(pAd82120InitReg, 0xff, sizeof(ampRegMap) * AMP_MAX_REG_NUM);
    if (ParseIniConfig(AUDIO_INI_AD82120_BASS_RAM2_TAB, pRawBuffer, pAd82120InitReg, AMP_MAX_REG_NUM) == XBH_SUCCESS)
    {
        s32Ret = writeEqRegMap(pAd82120InitReg, g_ad82120_eq_reg, 0x41, mI2cBus, mI2cAddr);
        if(s32Ret == XBH_FAILURE)
        {
            goto Error;
        }
    }
    else
    {
        XLOGE("g_ad82120_ram2_tab(No file was found to replace)\n");
        goto Error;
    }
    usleep(50*1000);
    Error:
    if (pAd82120InitReg)
    {
        free(pAd82120InitReg);
    }
    if (pRawBuffer)
    {
        free(pRawBuffer);
    }

    return s32Ret;
}

XBH_S32 ChipAd82120_Subwoofer::setDirectPass(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 eq_length = 4;
    XBH_U8 addr[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31}; 
    XBH_U8 reg[1] = {0};
    XBH_U8 spencil_reg[4] = {0x02,0x00,0x00,0x00};
    XBH_U8 tab_reg[1] = {0x23};
    XBH_U8 tab1_reg[1] = {0x02};
    XBH_U8 val[1] = {0x01};
    XBH_U8 addr_eq[] = {  
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,  
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,  
        0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,  
        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,  
        0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,  
        0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,  
        0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,  
        0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 
    };

    setMute(XBH_TRUE);
    usleep(20*1000);
    for(int i = 0;i < sizeof(addr); i++)
    {   
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr ,addr[i], 1, 1, reg);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("setDirectPass ret = %d\n",s32Ret);
            setMute(XBH_FALSE);
            return XBH_FAILURE;
        }
        usleep(2*1000);
    }
    usleep(2*1000);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr ,0x1d, 1, 1, tab_reg);
    usleep(2*1000);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr ,0x2e, 1, 1, tab1_reg);
    
    //准备写eq参数需要切页延时
    usleep(50*1000);
    for(int i = 0;i < sizeof(addr_eq); i++)
    {   
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, g_ad82120_eq_reg[0], 1, 1 , &addr_eq[i]);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("setDirectPass ret = %d bus = 0x%x addr = 0x%x\n",s32Ret,mI2cBus,mI2cAddr);
            setMute(XBH_FALSE);
            return XBH_FAILURE;
        }
        for (int j = 0; j < eq_length; j++)
        {   
            if(addr_eq[i] == 0x04 + 0x05 * ((i - 4) / 5))
            {   
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, g_ad82120_eq_reg[j+1], 1, 1,&spencil_reg[j]);
                if (s32Ret != XBH_SUCCESS)
                {
                    XLOGE("setDirectPass eq_reg[i+1] err\n");
                    setMute(XBH_FALSE);
                    return XBH_FAILURE;
                }
            } 
            else
            {   
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, g_ad82120_eq_reg[j+1], 1, 1,reg);
                if (s32Ret != XBH_SUCCESS)
                {
                    XLOGE("setDirectPass eq_reg[i+1] err\n");
                    setMute(XBH_FALSE);
                    return XBH_FAILURE;
                }
            }
            usleep(2*1000);
        }
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, g_ad82120_eq_reg[5], 1, 1, val);
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("setDirectPass eq_reg[size+1] err\n");
            setMute(XBH_FALSE);
            return XBH_FAILURE;
        }
        usleep(2*1000);
    }
    setMute(XBH_FALSE);
    XLOGD("setDirectPass iicAddr %#x successed!!!\n",mI2cAddr);
    return s32Ret;
}

XBH_S32 ChipAd82120_Subwoofer::getDirectPass(XBH_BOOL* enable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(enable == NULL)
    {
        return s32Ret; 
    }
    *enable = XBH_TRUE;
    return s32Ret; 
}

XBH_S32 ChipAd82120_Subwoofer::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    data[0] = DEVICE_STATE_MUTE;
    m_bInitDone = XBH_FALSE;

    WriteIniConfig(XBH_AUDIO_AQ_SUB_INI_FILE);
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, REG_DEVICE_ADDRESS, 0x01, 0x01, data);

    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" chip address = %#x is not init!!!!",mI2cAddr);
        return s32Ret;
    }
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipAd82120_Subwoofer::ChipAd82120_Subwoofer(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mute_gpio, XBH_S32 mute_level)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    mPgpio = pGpio;
    mPLevel = pLevel;
    mRgpio = rGpio;
    mRLevel = rLevel;
    mMute_gpio = mute_gpio;
    mMute_level = mute_level;
    init();
    XLOGD(" end ");
}

ChipAd82120_Subwoofer::~ChipAd82120_Subwoofer()
{

}

