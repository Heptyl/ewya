#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipTas5805m_Stereo"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipTas5805m_Stereo.h"
#include <cutils/properties.h>
#include <iostream>
#include <fstream>

XbhMutex                 ChipTas5805m_Stereo::mLock;

/* register address */
#define REG_PAGE		0x00
#define REG_DEVICE_STATE	0x03
#define REG_STATE_REPORT	0x16
#define REG_GLOBAL_FAULT1	0x17
#define REG_GLOBAL_FAULT2	0x18
#define REG_GLOBAL_FAULT3	0x19

/* DEVICE_STATE register values */
#define DEVICE_STATE_DEEP_SLEEP	0x00
#define DEVICE_STATE_SLEEP	0x01
#define DEVICE_STATE_HIZ	0x02
#define DEVICE_STATE_PLAY	0x03

#define DEVICE_STATE_MUTE	0x02

#define REG_DEVICE_AVC	      0x05
#define DEVICE_STATE_AVC      0x46
#define DEVICE_STATE_UNAVC    0xC6

#define REG_PAGE_EQ1             0x06
#define REG_PAGE_EQ2             0x07
#define DEVICE_STATE_UNEQ     0x66

//tas5805 eq parameter
#define TAS5805_I2C_EQ_ADDR             0x66
#define TAS5805_EQ_OFF                  0x87
#define TAS5805_EQ_ON                   0x86

#define TAS5805_BASS_FILE_PATCH            "vendor/firmware/eq_file/basslevel%s.cfg"
#define TAS5805_TREBLE_FILE_PATCH          "vendor/firmware/eq_file/treblelevel%s.cfg"
#define TAS5805_BALANCE_FILE_PATCH         "vendor/firmware/eq_file/balancelevel%s.cfg"
#define TAS5805_SURROUND_TRUE_FILE_PATCH         "vendor/firmware/eq_file/surround_true.cfg"
#define TAS5805_SURROUND_FALSE_FILE_PATCH        "vendor/firmware/eq_file/surround_false.cfg"

static XBH_U8 u8Data[1];
static XBH_CHAR TrebleValue[255] = {0};
static XBH_CHAR BassValue[255] = {0};
static XBH_CHAR BalanceValue[255] = {0};
static XBH_CHAR SoundMode[255] = {0};
static XBH_CHAR oldBasslevel[255] = {0};
static XBH_CHAR oldTreblelevel[255] = {0};
static XBH_CHAR oldBalancelevel[255] = {0};
static XBH_CHAR EQ_File[256] = {0};

/**
 * 复位功放芯片
*/
XBH_S32 ChipTas5805m_Stereo::resetChip()
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
XBH_S32 ChipTas5805m_Stereo::setMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    data[0] =mute ? DEVICE_STATE_MUTE : DEVICE_STATE_PLAY ;
    XLOGD("setMute addr: %#x,MUTE :%d\n",mI2cAddr,mute);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
    return  s32Ret;
}

/**
 * 获取speaker mute的状态
*/
XBH_S32 ChipTas5805m_Stereo::getMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, REG_DEVICE_STATE, 0x01, 0x01, data);
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
XBH_S32 ChipTas5805m_Stereo::setGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取功放输出的增益值
*/
XBH_S32 ChipTas5805m_Stereo::getGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

/**
 * 获取amp是否初始化完成
*/
XBH_S32 ChipTas5805m_Stereo::getChipInitDone(XBH_BOOL* enable)
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
XBH_S32 ChipTas5805m_Stereo::getChipExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipTas5805m_Stereo::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 进行固件升级
 */
XBH_S32 ChipTas5805m_Stereo::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}
/**
 * 获取固件升级时的状态
 */
XBH_S32 ChipTas5805m_Stereo::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::Tas5805m_Write(XBH_U8 *buf, XBH_U8 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::Tas5805m_Read(XBH_U8 *buf, XBH_U8 ctrl)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::setAvcEnable(XBH_BOOL isAvcEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::getAvcEnable(XBH_BOOL *isAvcEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::writeRegMap(amp5805RegMap *reg_map, XBH_S32 iicBus, XBH_S32 iicAddr)
{
    XBH_S32 ret = XBH_FAILURE;
    return XBH_FAILURE;
}


XBH_S32 ChipTas5805m_Stereo::LoadIniFileBuf(const XBH_CHAR *fileName, XBH_CHAR *buf, XBH_S32 *fileSize, XBH_S32 bufSize)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return XBH_SUCCESS;
}

XBH_S32 ChipTas5805m_Stereo::WriteIniConfig(const XBH_CHAR *fileName)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::setEqEnable(XBH_BOOL enable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 i = 0;
    XBH_U8 data[1] = {0};
    XBH_U8 data_eqInitAdress[] = {0x00, 0x7f, 0x00};

    for(i = 0;i < sizeof(data_eqInitAdress); i++)
    {   
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, data_eqInitAdress[i], 0x01, 0x01, data);
    }
    
    data[0] = (enable ? TAS5805_EQ_ON : TAS5805_EQ_OFF);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, TAS5805_I2C_EQ_ADDR, 0x01, 0x01, data);

    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" setEqEnable setI2cData error!!");
    }
    XLOGD(" setEqEnable:%d,s32Ret = %d,data[0]=%#x", enable,s32Ret,data[0]);
    return s32Ret;   
}

XBH_S32 ChipTas5805m_Stereo::getEqEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 data[1] = {0};

    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, TAS5805_I2C_EQ_ADDR, 0x01, 0x01, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" getEqEnable getIICData error!!");
    }
    if(data[0] != TAS5805_EQ_OFF)
    {
        *enable = XBH_TRUE;
    }
    else
    {
        *enable = XBH_FALSE;
    }
    XLOGD(" getEqEnable:%d,s32Ret = %d,data[0]=%#x", *enable,s32Ret,data[0]);
    return s32Ret;   
}

XBH_S32 ChipTas5805m_Stereo::setBass(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 BassType = 0;
    sprintf(BassValue, "%d", s32Value);
    property_set("persist.vendor.xbh.bass.value", BassValue);
    if(matchlevel((XBH_SOUND_TYPE)BassType, s32Value) == XBH_SUCCESS)
    {   
        XLOGD(" setBass success and s32Value = %d\n", s32Value);
        return XBH_SUCCESS;
    }
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::getBass(XBH_S32 *s32Value)
{
    if(property_get("persist.vendor.xbh.bass.value", BassValue, "50") > 0)
    {
        *s32Value = atoi(BassValue);
        XLOGD(" getBass success and *s32Value = %d\n", *s32Value);
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}

XBH_S32 ChipTas5805m_Stereo::setTreble(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 TrebleType = 1;
    sprintf(TrebleValue, "%d", s32Value);
    property_set("persist.vendor.xbh.treble.value", TrebleValue);
    
    if(matchlevel((XBH_SOUND_TYPE)TrebleType, s32Value) == XBH_SUCCESS)
    {   
        XLOGD(" setTreble success and s32Value = %d\n", s32Value);
        return XBH_SUCCESS;
    }
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::getTreble(XBH_S32 *s32Value)
{
    if(property_get("persist.vendor.xbh.treble.value", TrebleValue, "50") > 0)
    {
        *s32Value = atoi(TrebleValue);
        XLOGD(" getTreble success and *s32Value = %d\n", *s32Value);
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}

XBH_S32 ChipTas5805m_Stereo::setBalance(XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 BalanceType = 2;
    sprintf(BalanceValue, "%d", s32Value);
    property_set("persist.vendor.xbh.balance.value", BalanceValue);
    if(matchlevel((XBH_SOUND_TYPE)BalanceType, s32Value) == XBH_SUCCESS)
    {   
        XLOGD(" setBalance success and s32Value = %d\n", s32Value);
        return XBH_SUCCESS;
    }
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::getBalance(XBH_S32 *s32Value)
{
    if(property_get("persist.vendor.xbh.balance.value", BalanceValue, "50") > 0)
    {
        *s32Value = atoi(BalanceValue);
        XLOGD(" getBalance success and *s32Value = %d\n", *s32Value);
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}

XBH_S32 ChipTas5805m_Stereo::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{   
    XLOGD(" setSoundMode success and enSoundMode = %d\n", enSoundMode);
    sprintf(SoundMode, "%d", enSoundMode);
    property_set("persist.vendor.xbh.soundmode.value", SoundMode);
    return XBH_SUCCESS;
}

XBH_S32 ChipTas5805m_Stereo::getSoundMode(XBH_SOUND_MODE_E *enSoundMode)
{   
    if(property_get("persist.vendor.xbh.soundmode.value", SoundMode, "0") > 0)
    {
        *enSoundMode = static_cast<XBH_SOUND_MODE_E>(atoi(SoundMode));
        XLOGD(" getSoundMode success and enSoundMode = %d\n", *enSoundMode);
        return XBH_SUCCESS;
    }
    return XBH_FAILURE;
}

XBH_S32 ChipTas5805m_Stereo::setEQfiles(XBH_CHAR EQ_Files[256])
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    mWrite_amp_status = XBH_AMP_RUNNING;
    FILE* file = fopen(EQ_Files, "r");
    if (file == NULL) 
    {
        XLOGE("%s Failed to open file.\n",__func__);
        mWrite_amp_status = XBH_AMP_IDLE;
        return s32Ret;
    }

    XBH_CHAR line[256];
    while (fgets(line, sizeof(line), file)) 
    {
        XBH_CHAR cmd[10];
        XBH_U8 u8DevAddress;
        XBH_U32 u32RegAddr;
        XBH_U8 u8Data[1];
    
        if (sscanf(line, "%s", cmd) != 1) 
        {
            continue; // 跳过无效行
        }
        if (strcmp(cmd, "w") == 0) 
        {
            if (sscanf(line, "%s %x %x %x", cmd, &u8DevAddress, &u32RegAddr, &u8Data)) 
            {
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, u8DevAddress, u32RegAddr, 0x01,0x01,u8Data);
                if(s32Ret == XBH_FAILURE)
                {
                    XLOGE("setEQfiles is setI2cData failed = %d\n", s32Ret);
                    mWrite_amp_status = XBH_AMP_IDLE;
                    fclose(file);
                    return s32Ret;
                }
                XLOGD("I2C Write:mI2cBus=%d dev=%#x, reg=%#x, data=%#x,s32Ret = %d\n",mI2cBus, u8DevAddress, u32RegAddr, u8Data[0],s32Ret);
            } 
            else 
            {
                XLOGE("Invalid format: %s", line);
                mWrite_amp_status = XBH_AMP_IDLE;
                fclose(file);
                return s32Ret;
            }
        }
    }
    fclose(file);
    XLOGD("%s setEQfiles = %s\n",__func__, EQ_Files);
    mWrite_amp_status = XBH_AMP_SUCCESS;
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::matchlevel(XBH_SOUND_TYPE Type, XBH_S32 s32Value)
{   
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR Volumelevel[2] = {0};

    if(s32Value == 0)
    {
        sprintf(Volumelevel, "%s", "0");
    }
    else if(0 < s32Value && s32Value <= 10)
    {
        sprintf(Volumelevel, "%s", "1");
    }
    else if(10 < s32Value && s32Value <= 20)
    {
        sprintf(Volumelevel, "%s", "2");
    }
    else if(20 < s32Value && s32Value <= 30)
    {
        sprintf(Volumelevel, "%s", "3");
    }
    else if(30 < s32Value && s32Value <= 40)
    {
        sprintf(Volumelevel, "%s", "4");
    }
    else if(40 < s32Value && s32Value <= 50)
    {
        sprintf(Volumelevel, "%s", "5");
    }
    else if(50 < s32Value && s32Value <= 60)
    {
        sprintf(Volumelevel, "%s", "6");
    }
    else if(60 < s32Value && s32Value <= 70)
    {
        sprintf(Volumelevel, "%s", "7");
    }
    else if(70 < s32Value && s32Value <= 80)
    {
        sprintf(Volumelevel, "%s", "8");
    }
    else if(80 < s32Value && s32Value <= 90)
    {
        sprintf(Volumelevel, "%s", "9");
    }
    else if(90 < s32Value && s32Value <= 100)
    {
        sprintf(Volumelevel, "%s", "A");
    }
    else
    {
        sprintf(Volumelevel, "%s", "5");
    }

    switch (Type)
    {
        case XBH_SOUND_TYPE_BASS:
        {
            if(strcmp(oldBasslevel, Volumelevel) != 0)
            {
                sprintf(EQ_File, TAS5805_BASS_FILE_PATCH, Volumelevel);
                sprintf(oldBasslevel, "%s", Volumelevel);
                property_set("persist.vendor.xbh.bass.level", Volumelevel);
            }
            else
            {
                return XBH_SUCCESS;
            }
            break;
        }
        case XBH_SOUND_TYPE_TREBLE:
        {
            if(strcmp(oldTreblelevel, Volumelevel) != 0)
            {
                sprintf(EQ_File, TAS5805_TREBLE_FILE_PATCH, Volumelevel);
                sprintf(oldTreblelevel, "%s", Volumelevel);
                property_set("persist.vendor.xbh.treble.level", Volumelevel);
            }
            else
            {
                return XBH_SUCCESS;
            }
            break;
        }
        case XBH_SOUND_TYPE_BALANCE:
        {
            if(strcmp(oldBalancelevel, Volumelevel) != 0)
            {
                sprintf(EQ_File, TAS5805_BALANCE_FILE_PATCH, Volumelevel);
                sprintf(oldBalancelevel, "%s", Volumelevel);
                property_set("persist.vendor.xbh.balance.level", Volumelevel);
            }
            else
            {
                return XBH_SUCCESS;
            }
            break;
        }
        default:
        {
            break;
        }      
    }
    if((mWrite_amp_status == XBH_AMP_SUCCESS || mWrite_amp_status == XBH_AMP_IDLE))
    {   
        s32Ret = setEQfiles(EQ_File);
        if(s32Ret == XBH_SUCCESS)
        {
            XLOGD("success setEQfiles=%s\n",EQ_File);
            return s32Ret;
        }
    }
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::setSurroundEnable(XBH_BOOL bEnable)
{   
    XBH_S32 s32Ret = XBH_FAILURE;

    mSurroundSoundStatus = bEnable;
    if(bEnable)
    {   
        s32Ret = setEQfiles(TAS5805_SURROUND_TRUE_FILE_PATCH);
    }
    else
    {
        s32Ret = setEQfiles(TAS5805_SURROUND_FALSE_FILE_PATCH);
    }
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" setSurroundSound setEQfiles error!!");
    }
    XLOGD(" setSurroundSound enable:%d,s32Ret = %d", bEnable,s32Ret);
    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::getSurroundEnable(XBH_BOOL *bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    *bEnable = mSurroundSoundStatus;
    XLOGD(" getSurroundSound enable:%d,s32Ret = %d", *bEnable,s32Ret);
    return s32Ret;   
}

XBH_S32 ChipTas5805m_Stereo::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

ChipTas5805m_Stereo::ChipTas5805m_Stereo(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mute_gpio, XBH_S32 mute_level)
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
    mWrite_amp_status = XBH_AMP_IDLE;
    init();
    XLOGD(" end ");
}

ChipTas5805m_Stereo::~ChipTas5805m_Stereo()
{

}

