#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipAcm8625_Subwoofer"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipAcm8625_Subwoofer.h"
#include <iostream>
#include <fstream>
XbhMutex                 ChipAcm8625_Subwoofer::mLock;

/* register address */
#define REG_PAGE		0x00
#define REG_DEVICE_STATE	0x04
#define REG_STATE_REPORT	0x16
#define REG_GLOBAL_FAULT1	0x17
#define REG_GLOBAL_FAULT2	0x18
#define REG_GLOBAL_FAULT3	0x19

/* DEVICE_STATE register values */
#define DEVICE_STATE_DEEP_SLEEP	0x00
#define DEVICE_STATE_SLEEP	0x01
#define DEVICE_STATE_HIZ	0x02
#define DEVICE_STATE_PLAY	0x03

#define DEVICE_STATE_MUTE	0x0C

#define REG_DEVICE_AVC	      0x05
#define DEVICE_STATE_AVC      0x46
#define DEVICE_STATE_UNAVC    0xC6

#define REG_PAGE_EQ1             0x06
#define REG_PAGE_EQ2             0x07
#define DEVICE_STATE_UNEQ        0x66
#define REG_ADRESS_UNEQ1         0xB0
#define REG_ADRESS_UNEQ2         0xE0
/**
 * 复位功放芯片
*/
XBH_S32 ChipAcm8625_Subwoofer::resetChip()
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
XBH_S32 ChipAcm8625_Subwoofer::setMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    data[0] =(mute ? DEVICE_STATE_MUTE : 0) | DEVICE_STATE_PLAY ;
    XLOGD("setMute addr: %#x,MUTE :%d\n",mI2cAddr,mute);
    s32Ret |= XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
    return  s32Ret;
}

/**
 * 获取speaker mute的状态
*/
XBH_S32 ChipAcm8625_Subwoofer::getMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    s32Ret |= XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
    if (data[0] == DEVICE_STATE_PLAY)
    {
        *mute = XBH_FALSE;
    }
    else
    {
        *mute = XBH_TRUE;
    }
    return  s32Ret;
}

/**
 * 设置功放输出的增益值
*/
XBH_S32 ChipAcm8625_Subwoofer::setGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 ChipAcm8625_Subwoofer::getGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取amp是否初始化完成
*/
XBH_S32 ChipAcm8625_Subwoofer::getChipInitDone(XBH_BOOL* enable)
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
XBH_S32 ChipAcm8625_Subwoofer::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipAcm8625_Subwoofer::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 进行固件升级
 */
XBH_S32 ChipAcm8625_Subwoofer::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 获取固件升级时的状态
 */
XBH_S32 ChipAcm8625_Subwoofer::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 ChipAcm8625_Subwoofer::Acm8625_Write(XBH_U8 *buf, XBH_U8 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipAcm8625_Subwoofer::Acm8625_Read(XBH_U8 *buf, XBH_U8 ctrl)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipAcm8625_Subwoofer::setAvcEnable(XBH_BOOL isAvcEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD("setAvcEnable setI2cData error!!");
    }
    data[0] =(isAvcEnable ? DEVICE_STATE_AVC : DEVICE_STATE_UNAVC);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_DEVICE_AVC, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD("setAvcEnable setI2cData error!!");
    }
    XLOGD("setAvcEnable:%d,s32Ret = %d", isAvcEnable,s32Ret);
    return s32Ret;
}

XBH_S32 ChipAcm8625_Subwoofer::getAvcEnable(XBH_BOOL *isAvcEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD("getAvcEnable setI2cData error!!");
    }
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, REG_DEVICE_AVC, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD("getAvcEnable setI2cData error!!");
    }
    if (data[0] == DEVICE_STATE_UNAVC)
    {
        *isAvcEnable = XBH_FALSE;
    }
    else
    {
        *isAvcEnable = XBH_TRUE;
    }
    XLOGD("getAvcEnable:%d,s32Ret = %d", *isAvcEnable,s32Ret);
    return  s32Ret;
}

XBH_S32 ChipAcm8625_Subwoofer::setDirectPass(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 i = 0;
    XBH_U8 data[1] = {0};
    XBH_U8 data_eqAdress[] = {0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2, 0xC3};
    XBH_U8 data_eq2Adress[] = {0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3};
     
    data[0] = REG_PAGE_EQ1;
    usleep(20 * 1000);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    data[0] = 0x08;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_ADRESS_UNEQ1, 0x01, 0x01, data);

    for(i = 0;i < sizeof(data_eqAdress); i++)
    {   
        data[0] = REG_PAGE;
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, data_eqAdress[i], 0x01, 0x01, data);
        
    }

    data[0] = REG_PAGE_EQ2;
    usleep(20 * 1000);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    data[0] = 0x08;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_ADRESS_UNEQ2, 0x01, 0x01, data);
    for(i = 0;i < sizeof(data_eq2Adress); i++)
    {   
        data[0] = REG_PAGE;
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, data_eq2Adress[i], 0x01, 0x01, data);
    }

    usleep(20 * 1000);
    data[0] = REG_PAGE;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_PAGE, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" setEqEnable setI2cData error!!");
    }
    data[0] =(enable ? DEVICE_STATE_UNEQ : DEVICE_STATE_AVC);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_DEVICE_AVC, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" setEqEnable setI2cData error!!");
    }
    XLOGD(" setEqEnable:%d,s32Ret = %d,data[0]=%#x", enable,s32Ret,data[0]);
    return s32Ret;   
}

XBH_S32 ChipAcm8625_Subwoofer::getDirectPass(XBH_BOOL* enable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    if(enable == NULL)
    {
        return s32Ret; 
    }
    *enable = XBH_TRUE;
    return s32Ret; 
}

XBH_S32 ChipAcm8625_Subwoofer::writeRegMap(amp8625RegMap *reg_map, XBH_S32 iicBus, XBH_S32 iicAddr)
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
        //XLOGE("writeRegMap bus=%d,Addr=%#x, addr = %#x,reg[0] = %#x,size = %d\n",iicBus,iicAddr,addr,reg[0],size);
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

XBH_S32 ChipAcm8625_Subwoofer::ParseIniConfig(const char *section, char *fileBuf, amp8625RegMap *pRegMap, XBH_U32 maxRegSize)
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

    while (Is8625End(buf[i]) == XBH_FALSE)
    {
        while (!((Is8625NewLine(*(buf + i - 1))) && (Is8625LeftBracket(*(buf + i)))) &&(!Is8625End(*(buf + i)))) 
        {
            while ((!Is8625NewLine(*(buf + i))) &&(!Is8625End(*(buf + i))))
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
                        //XLOGE("lipu end k =%d addr = %#x\n",k,pRegMap[k].addr);
                    }
                    else if (pRegMap[k].size == 0xff)
                    {
                        pRegMap[k].size = value;
                        m = 0;
                    }
                    else if (m < pRegMap[k].size)
                    {
                        pRegMap[k].reg[m] = value;
                        //XLOGE("lipu1 end k =%d m =%d value = %#x\n",k,m,pRegMap[k].reg[m]);
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
                    XLOGE("ParseFile end k =%d \n",k);
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


XBH_S32 ChipAcm8625_Subwoofer::LoadIniFileBuf(const XBH_CHAR *fileName, XBH_CHAR *buf, XBH_S32 *fileSize, XBH_S32 bufSize)
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

XBH_S32 ChipAcm8625_Subwoofer::WriteIniConfig(const XBH_CHAR *fileName)
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
    
    amp8625RegMap *pAcm8625InitReg = (amp8625RegMap *)malloc(sizeof(amp8625RegMap) * AMP_MAX_REG_NUM);
    memset(pAcm8625InitReg, 0xff, sizeof(amp8625RegMap) * AMP_MAX_REG_NUM);
    if(ParseIniConfig(AUDIO_INI_ACM8625_INIT_REG, pRawBuffer, pAcm8625InitReg, AMP_MAX_REG_NUM) == XBH_SUCCESS)
    {
        s32Ret = writeRegMap(pAcm8625InitReg, mI2cBus, mI2cAddr);
        if(s32Ret == XBH_FAILURE)
        {
            goto Error;
        }
    }
    else
    {
        XLOGE("g_Acm8625_init_reg(No file was found to replace)\n");
        goto Error;
    }
    usleep(20*1000);
    memset(pAcm8625InitReg, 0xff, sizeof(amp8625RegMap) * AMP_MAX_REG_NUM);
    if (ParseIniConfig(AUDIO_INI_ACM8625_RAM1_TAB, pRawBuffer, pAcm8625InitReg, AMP_MAX_REG_NUM) == XBH_SUCCESS)
    {
        s32Ret = writeRegMap(pAcm8625InitReg, mI2cBus, mI2cAddr);
        if(s32Ret == XBH_FAILURE)
        {
            goto Error;
        }
    }
    else
    {
        XLOGE("g_Acm8625_ram1_tab(No file was found to replace)\n");
        goto Error;
    }
    usleep(20*1000);
    memset(pAcm8625InitReg, 0xff, sizeof(amp8625RegMap) * AMP_MAX_REG_NUM);
    if (ParseIniConfig(AUDIO_INI_ACM8625_RAM2_TAB, pRawBuffer, pAcm8625InitReg, AMP_MAX_REG_NUM) == XBH_SUCCESS)
    {
        s32Ret = writeRegMap(pAcm8625InitReg,mI2cBus, mI2cAddr);
        if(s32Ret == XBH_FAILURE)
        {
            goto Error;
        }
    }
    else
    {
        XLOGE("g_Acm8625_ram2_tab(No file was found to replace)\n");
        goto Error;
    }
    usleep(50*1000);
    Error:
    if (pAcm8625InitReg)
    {
        free(pAcm8625InitReg);
    }
    if (pRawBuffer)
    {
        free(pRawBuffer);
    }

    return s32Ret;
}

XBH_S32 ChipAcm8625_Subwoofer::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" chip address = %#x is not init!!!!",mI2cAddr);
        return s32Ret;
    }
    WriteIniConfig(XBH_AUDIO_ACM8625S_AQ_SUB_INI_FILE);
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipAcm8625_Subwoofer::ChipAcm8625_Subwoofer(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mute_gpio, XBH_S32 mute_level)
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

ChipAcm8625_Subwoofer::~ChipAcm8625_Subwoofer()
{

}

