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
static XBH_CHAR OldGainLevel[255] = {0};
static XBH_CHAR getFrqValue[255] = {0};
static XBH_CHAR setFrqValue[255] = {0};
static XBH_CHAR setFrqProperty[255] = {0};
static XBH_CHAR getFrqProperty[255] = {0};


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

XBH_S32 ChipTas5805m_Stereo::setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    if(frqGainLevel(enEqMode,s32Value) == XBH_SUCCESS)
    {
        XLOGD(" setAudioEq success and frqGainValue = %d\n", s32Value);
        return XBH_SUCCESS;
    }

    return s32Ret;
}

XBH_S32 ChipTas5805m_Stereo::getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    switch (enEqMode)
    {
        case XBH_EQ_MODE_0:
        case XBH_EQ_MODE_1:
        case XBH_EQ_MODE_2:
        case XBH_EQ_MODE_3:
        case XBH_EQ_MODE_4:
        {
            sprintf(getFrqProperty, "persist.vendor.xbh.frqgain.value.%u", enEqMode);

            if(property_get(getFrqProperty, getFrqValue, "50") > 0)
            {
                *s32Value = atoi(getFrqValue);
                XLOGD(" getaudioeq = %d success and *s32Value = %d\n", enEqMode,*s32Value);

                return XBH_SUCCESS;
            }
            return XBH_FAILURE;
        }
        default:
        {
            break;
        }
    }
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

XBH_S32 ChipTas5805m_Stereo::setEqReg(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    mWrite_amp_status = XBH_AMP_RUNNING;
    XBH_S32 gainindex = s32Value;
    XBH_S32 i = 0;

    //mute reg
    XBH_U8 muteAdress[] = {0x00, 0x7f, 0x00, 0x03, 0x00, 0x7f, 0x00, 0x03};
    XBH_U8 muteAdress_data[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0b};
    XBH_U8 playAdress_data[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x03};

    //init reg
    XBH_U8 eqInitAdress[] = {0x00, 0x7f, 0x00};
    XBH_U8 eqInitAdress_data[] = {0x00, 0xaa, 0x24};

    //frequency 120
    XBH_U8 eqAdress_120[] = {0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b};

    XBH_U8 eq_120HZ_0[] = {0x07, 0xee, 0xb5, 0x55, 0xf0, 0x4f, 0x83, 0xb8, 0x07, 0xc2, 0x45, 0xd0, 0x0f, 0xb0, 0x7c, 0x48, 0xf8, 0x4f, 0x04, 0xdb};
    XBH_U8 eq_120HZ_1[] = {0x07, 0xf2, 0xfa, 0x46, 0xf0, 0x47, 0x12, 0x05, 0x07, 0xc6, 0x72, 0xd6, 0x0f, 0xb8, 0xed, 0xfb, 0xf8, 0x46, 0x92, 0xe4};
    XBH_U8 eq_120HZ_2[] = {0x07, 0xf6, 0xcc, 0x3d, 0xf0, 0x3f, 0x83, 0xbb, 0x07, 0xca, 0x2f, 0x67, 0x0f, 0xc0, 0x7c, 0x45, 0xf8, 0x3f, 0x04, 0x5c};
    XBH_U8 eq_120HZ_3[] = {0x07, 0xfa, 0x36, 0xf3, 0xf0, 0x38, 0xc1, 0xa9, 0x07, 0xcd, 0x86, 0xf9, 0x0f, 0xc7, 0x3e, 0x57, 0xf8, 0x38, 0x42, 0x14};
    XBH_U8 eq_120HZ_4[] = {0x07, 0xfd, 0x45, 0x03, 0xf0, 0x32, 0xb6, 0xd7, 0x07, 0xd0, 0x83, 0xec, 0x0f, 0xcd, 0x49, 0x29, 0xf8, 0x32, 0x37, 0x11};
    XBH_U8 eq_120HZ_5[] = {0x07, 0xf0, 0xe4, 0x16, 0xf0, 0x1e, 0x37, 0xd3, 0x07, 0xf0, 0xe4, 0x16, 0x0f, 0xe1, 0xab, 0xa4, 0xf8, 0x1e, 0x1b, 0x4a};
    XBH_U8 eq_120HZ_6[] = {0x08, 0x02, 0xbb, 0xec, 0xf0, 0x2d, 0x50, 0x54, 0x07, 0xd0, 0x73, 0xb2, 0x0f, 0xd2, 0xaf, 0xac, 0xf8, 0x2c, 0xd0, 0x62};
    XBH_U8 eq_120HZ_7[] = {0x08, 0x05, 0xcd, 0x3f, 0xf0, 0x2d, 0x50, 0x54, 0x07, 0xcd, 0x62, 0x5f, 0x0f, 0xd2, 0xaf, 0xac, 0xf8, 0x2c, 0xd0, 0x62};
    XBH_U8 eq_120HZ_8[] = {0x08, 0x09, 0x3e, 0x65, 0xf0, 0x2d, 0x50, 0x54, 0x07, 0xc9, 0xf1, 0x39, 0x0f, 0xd2, 0xaf, 0xac, 0xf8, 0x2c, 0xd0, 0x62};
    XBH_U8 eq_120HZ_9[] = {0x08, 0x0d, 0x1b, 0x0f, 0xf0, 0x2d, 0x50, 0x54, 0x07, 0xc6, 0x14, 0x8e, 0x0f, 0xd2, 0xaf, 0xac, 0xf8, 0x2c, 0xd0, 0x62};
    XBH_U8 eq_120HZ_10[] = {0x08, 0x11, 0x70, 0x5c, 0xf0, 0x2d, 0x50, 0x54, 0x07, 0xc1, 0xbf, 0x41, 0x0f, 0xd2, 0xaf, 0xac, 0xf8, 0x2c, 0xd0, 0x62};

    XBH_U8* eqArrays_120HZ[] = {
        eq_120HZ_0, eq_120HZ_1, eq_120HZ_2, eq_120HZ_3, eq_120HZ_4,
        eq_120HZ_5, eq_120HZ_6, eq_120HZ_7, eq_120HZ_8, eq_120HZ_9,
        eq_120HZ_10
    };

    //frequency 500
    XBH_U8 eqAdress_500[] = {0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f};

    XBH_U8 eq_500HZ_0[] = {0x07, 0xbc, 0x19, 0x77, 0xf1, 0x3e, 0x65, 0x39, 0x07, 0x0d, 0x9c, 0x52, 0x0e, 0xc1, 0x9a, 0xc7, 0xf9, 0x36, 0x4a, 0x37};
    XBH_U8 eq_500HZ_1[] = {0x07, 0xcc, 0x8c, 0x26, 0xf1, 0x1f, 0x04, 0x23, 0x07, 0x1c, 0x9b, 0xf5, 0x0e, 0xe0, 0xfb, 0xdd, 0xf9, 0x16, 0xd7, 0xe5};
    XBH_U8 eq_500HZ_2[] = {0x07, 0xdb, 0x70, 0x52, 0xf1, 0x02, 0x9b, 0x55, 0x07, 0x2a, 0x30, 0x32, 0x0e, 0xfd, 0x64, 0xab, 0xf8, 0xfa, 0x5f, 0x7b};
    XBH_U8 eq_500HZ_3[] = {0x07, 0xe8, 0xe6, 0x2c, 0xf0, 0xe8, 0xed, 0x64, 0x07, 0x36, 0x76, 0x65, 0x0f, 0x17, 0x12, 0x9c, 0xf8, 0xe0, 0xa3, 0x6f};
    XBH_U8 eq_500HZ_4[] = {0x07, 0xf5, 0x0c, 0x64, 0xf0, 0xd1, 0xbf, 0xc2, 0x07, 0x41, 0x8a, 0x89, 0x0f, 0x2e, 0x40, 0x3e, 0xf8, 0xc9, 0x69, 0x13};
    XBH_U8 eq_500HZ_5[] = {0x08, 0x00, 0x00, 0x00, 0xf0, 0xbc, 0xdb, 0x13, 0x07, 0x4b, 0x87, 0x16, 0x0f, 0x43, 0x24, 0xed, 0xf8, 0xb4, 0x78, 0xea};
    XBH_U8 eq_500HZ_6[] = {0x08, 0x0b, 0x02, 0xaf, 0xf0, 0xbc, 0xdb, 0x13, 0x07, 0x40, 0x84, 0x68, 0x0f, 0x43, 0x24, 0xed, 0xf8, 0xb4, 0x78, 0xea};
    XBH_U8 eq_500HZ_7[] = {0x08, 0x17, 0x5d, 0x4c, 0xf0, 0xbc, 0xdb, 0x13, 0x07, 0x34, 0x29, 0xcb, 0x0f, 0x43, 0x24, 0xed, 0xf8, 0xb4, 0x78, 0xea};
    XBH_U8 eq_500HZ_8[] = {0x08, 0x25, 0x39, 0xce, 0xf0, 0xbc, 0xdb, 0x13, 0x07, 0x26, 0x4d, 0x48, 0x0f, 0x43, 0x24, 0xed, 0xf8, 0xb4, 0x78, 0xea};
    XBH_U8 eq_500HZ_9[] = {0x08, 0x34, 0xc7, 0x4d, 0xf0, 0xbc, 0xdb, 0x13, 0x07, 0x16, 0xbf, 0xca, 0x0f, 0x43, 0x24, 0xed, 0xf8, 0xb4, 0x78, 0xea};
    XBH_U8 eq_500HZ_10[] = {0x08, 0x46, 0x3a, 0x9c, 0xf0, 0xbc, 0xdb, 0x13, 0x07, 0x05, 0x4c, 0x7a, 0x0f, 0x43, 0x24, 0xed, 0xf8, 0xb4, 0x78, 0xea};

    XBH_U8* eqArrays_500HZ[] = {
        eq_500HZ_0, eq_500HZ_1, eq_500HZ_2, eq_500HZ_3, eq_500HZ_4,
        eq_500HZ_5, eq_500HZ_6, eq_500HZ_7, eq_500HZ_8, eq_500HZ_9,
        eq_500HZ_10
    };

    //frequency 1500
    XBH_U8 eqAdress_1500[] = {0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, 0x51, 0x52, 0x53};

    XBH_U8 eq_1500HZ_0[] = {0x07, 0x4f, 0x19, 0xaa, 0xf3, 0x67, 0x8f, 0xd4, 0x05, 0x88, 0x82, 0x26, 0x0c, 0x98, 0x70, 0x2c, 0xfb, 0x28, 0x64, 0x30};
    XBH_U8 eq_1500HZ_1[] = {0x07, 0x78, 0x23, 0xe5, 0xf3, 0x20, 0xd6, 0x9b, 0x05, 0xa7, 0x93, 0xd8, 0x0c, 0xdf, 0x29, 0x65, 0xfa, 0xe0, 0x48, 0x42};
    XBH_U8 eq_1500HZ_2[] = {0x07, 0x9e, 0x43, 0xc1, 0xf2, 0xdf, 0x23, 0x98, 0x05, 0xc4, 0x70, 0x80, 0x0d, 0x20, 0xdc, 0x68, 0xfa, 0x9d, 0x4b, 0xbf};
    XBH_U8 eq_1500HZ_3[] = {0x07, 0xc1, 0x8c, 0x2b, 0xf2, 0xa2, 0x56, 0x2c, 0x05, 0xdf, 0x26, 0x74, 0x0d, 0x5d, 0xa9, 0xd4, 0xfa, 0x5f, 0x4d, 0x60};
    XBH_U8 eq_1500HZ_4[] = {0x07, 0xe2, 0x16, 0x45, 0xf2, 0x6a, 0x43, 0x09, 0x05, 0xf7, 0xc8, 0xb9, 0x0d, 0x95, 0xbc, 0xf7, 0xfa, 0x26, 0x21, 0x02};
    XBH_U8 eq_1500HZ_5[] = {0x08, 0x00, 0x00, 0x00, 0xf2, 0x36, 0xb6, 0x97, 0x06, 0x0e, 0x6d, 0xf9, 0x0d, 0xc9, 0x49, 0x69, 0xf9, 0xf1, 0x92, 0x07};
    XBH_U8 eq_1500HZ_6[] = {0x08, 0x1e, 0x5b, 0x3c, 0xf2, 0x36, 0xb6, 0x97, 0x05, 0xf0, 0x12, 0xbd, 0x0d, 0xc9, 0x49, 0x69, 0xf9, 0xf1, 0x92, 0x07};
    XBH_U8 eq_1500HZ_7[] = {0x08, 0x40, 0x6a, 0xb4, 0xf2, 0x36, 0xb6, 0x97, 0x05, 0xce, 0x03, 0x45, 0x0d, 0xc9, 0x49, 0x69, 0xf9, 0xf1, 0x92, 0x07};
    XBH_U8 eq_1500HZ_8[] = {0x08, 0x66, 0xa2, 0x1c, 0xf2, 0x36, 0xb6, 0x97, 0x05, 0xa7, 0xcb, 0xdd, 0x0d, 0xc9, 0x49, 0x69, 0xf9, 0xf1, 0x92, 0x07};
    XBH_U8 eq_1500HZ_9[] = {0x08, 0x91, 0x83, 0x45, 0xf2, 0x36, 0xb6, 0x97, 0x05, 0x7c, 0xea, 0xb4, 0x0d, 0xc9, 0x49, 0x69, 0xf9, 0xf1, 0x92, 0x07};
    XBH_U8 eq_1500HZ_10[] = {0x08, 0xc1, 0x9f, 0xd9, 0xf2, 0x36, 0xb6, 0x97, 0x05, 0x4c, 0xce, 0x20, 0x0d, 0xc9, 0x49, 0x69, 0xf9, 0xf1, 0x92, 0x07};

    XBH_U8* eqArrays_1500HZ[] = {
        eq_1500HZ_0, eq_1500HZ_1, eq_1500HZ_2, eq_1500HZ_3, eq_1500HZ_4,
        eq_1500HZ_5, eq_1500HZ_6, eq_1500HZ_7, eq_1500HZ_8, eq_1500HZ_9,
        eq_1500HZ_10
    };

    //frequency 5000
    XBH_U8 eqAdress_5000[] = {0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67};

    XBH_U8 eq_5000HZ_0[] = {0x06, 0x6c, 0x42, 0x9b, 0xf9, 0x06, 0x29, 0x1a, 0x02, 0x5e, 0xbd, 0xbf, 0x06, 0xf9, 0xd6, 0xe6, 0xff, 0x34, 0xff, 0xa7};
    XBH_U8 eq_5000HZ_1[] = {0x06, 0xc0, 0xf3, 0xdf, 0xf8, 0xaa, 0x2c, 0xf7, 0x02, 0x7d, 0xfe, 0x41, 0x07, 0x55, 0xd3, 0x09, 0xfe, 0xc1, 0x0d, 0xe0};
    XBH_U8 eq_5000HZ_2[] = {0x07, 0x14, 0x24, 0x47, 0xf8, 0x4f, 0xd2, 0xd4, 0x02, 0x9c, 0xb0, 0xbf, 0x07, 0xb0, 0x2d, 0x2c, 0xfe, 0x4f, 0x2a, 0xfa};
    XBH_U8 eq_5000HZ_3[] = {0x07, 0x65, 0x55, 0x49, 0xf7, 0xf7, 0xa4, 0x1f, 0x02, 0xba, 0xa6, 0x88, 0x08, 0x08, 0x5b, 0xe1, 0xfd, 0xe0, 0x04, 0x2e};
    XBH_U8 eq_5000HZ_4[] = {0x07, 0xb4, 0x14, 0xf0, 0xf7, 0xa2, 0x1c, 0x9f, 0x02, 0xd7, 0xb5, 0x8e, 0x08, 0x5d, 0xe3, 0x61, 0xfd, 0x74, 0x35, 0x81};
    XBH_U8 eq_5000HZ_5[] = {0x08, 0x00, 0x00, 0x00, 0xf7, 0x4f, 0xa8, 0x19, 0x02, 0xf3, 0xb9, 0x34, 0x08, 0xb0, 0x57, 0xe7, 0xfd, 0x0c, 0x46, 0xcc};
    XBH_U8 eq_5000HZ_6[] = {0x08, 0x4e, 0xd7, 0x3e, 0xf7, 0x4f, 0xa8, 0x19, 0x02, 0xa4, 0xe1, 0xf6, 0x08, 0xb0, 0x57, 0xe7, 0xfd, 0x0c, 0x46, 0xcc};
    XBH_U8 eq_5000HZ_7[] = {0x08, 0xa7, 0x4d, 0x37, 0xf7, 0x4f, 0xa8, 0x19, 0x02, 0x4c, 0x6b, 0xfd, 0x08, 0xb0, 0x57, 0xe7, 0xfd, 0x0c, 0x46, 0xcc};
    XBH_U8 eq_5000HZ_8[] = {0x09, 0x0a, 0x8e, 0x69, 0xf7, 0x4f, 0xa8, 0x19, 0x01, 0xe9, 0x2a, 0xca, 0x08, 0xb0, 0x57, 0xe7, 0xfd, 0x0c, 0x46, 0xcc};
    XBH_U8 eq_5000HZ_9[] = {0x09, 0x79, 0xec, 0x00, 0xf7, 0x4f, 0xa8, 0x19, 0x01, 0x79, 0xcd, 0x34, 0x08, 0xb0, 0x57, 0xe7, 0xfd, 0x0c, 0x46, 0xcc};
    XBH_U8 eq_5000HZ_10[] = {0x09, 0xf6, 0xe0, 0x49, 0xf7, 0x4f, 0xa8, 0x19, 0x00, 0xfc, 0xd8, 0xeb, 0x08, 0xb0, 0x57, 0xe7, 0xfd, 0x0c, 0x46, 0xcc};

    XBH_U8* eqArrays_5000HZ[] = {
        eq_5000HZ_0, eq_5000HZ_1, eq_5000HZ_2, eq_5000HZ_3, eq_5000HZ_4,
        eq_5000HZ_5, eq_5000HZ_6, eq_5000HZ_7, eq_5000HZ_8, eq_5000HZ_9,
        eq_5000HZ_10
    };

    //frequency 10000
    XBH_U8 eqAdress_10000[] = {0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b};

    XBH_U8 eq_10000HZ_0[] = {0x05, 0xd3, 0x49, 0x3c, 0xfe, 0x6e, 0x53, 0xc0, 0x00, 0x3c, 0xa8, 0x9e, 0x01, 0x91, 0xac, 0x40, 0x01, 0xf0, 0x0e, 0x25};
    XBH_U8 eq_10000HZ_1[] = {0x06, 0x3f, 0x4f, 0xb0, 0xfe, 0x51, 0x3b, 0x1e, 0x00, 0x41, 0x0d, 0x7a, 0x01, 0xae, 0xc4, 0xe2, 0x01, 0x7f, 0xa2, 0xd6};
    XBH_U8 eq_10000HZ_2[] = {0x06, 0xad, 0xb0, 0x60, 0xfe, 0x33, 0x80, 0x46, 0x00, 0x45, 0x8a, 0xd4, 0x01, 0xcc, 0x7f, 0xba, 0x01, 0x0c, 0xc4, 0xcc};
    XBH_U8 eq_10000HZ_3[] = {0x07, 0x1d, 0xbe, 0xc3, 0xfe, 0x15, 0x51, 0xb1, 0x00, 0x4a, 0x19, 0xa8, 0x01, 0xea, 0xae, 0x4f, 0x00, 0x98, 0x27, 0x95};
    XBH_U8 eq_10000HZ_4[] = {0x07, 0x8e, 0xc3, 0x4c, 0xfd, 0xf6, 0xe0, 0xd0, 0x00, 0x4e, 0xb2, 0x80, 0x02, 0x09, 0x1f, 0x30, 0x00, 0x22, 0x8a, 0x34};
    XBH_U8 eq_10000HZ_5[] = {0x08, 0x00, 0x00, 0x00, 0xfd, 0xd8, 0x60, 0xcd, 0x00, 0x53, 0x4d, 0xa1, 0x02, 0x27, 0x9f, 0x33, 0xff, 0xac, 0xb2, 0x5f};
    XBH_U8 eq_10000HZ_6[] = {0x08, 0x77, 0xdd, 0x58, 0xfd, 0xd8, 0x60, 0xcd, 0xff, 0xdb, 0x70, 0x48, 0x02, 0x27, 0x9f, 0x33, 0xff, 0xac, 0xb2, 0x5f};
    XBH_U8 eq_10000HZ_7[] = {0x08, 0xfe, 0x5a, 0xdf, 0xfd, 0xd8, 0x60, 0xcd, 0xff, 0x54, 0xf2, 0xc2, 0x02, 0x27, 0x9f, 0x33, 0xff, 0xac, 0xb2, 0x5f};
    XBH_U8 eq_10000HZ_8[] = {0x09, 0x95, 0x41, 0x6e, 0xfd, 0xd8, 0x60, 0xcd, 0xfe, 0xbe, 0x0c, 0x32, 0x02, 0x27, 0x9f, 0x33, 0xff, 0xac, 0xb2, 0x5f};
    XBH_U8 eq_10000HZ_9[] = {0x0a, 0x3e, 0x91, 0xa2, 0xfd, 0xd8, 0x60, 0xcd, 0xfe, 0x14, 0xbb, 0xfe, 0x02, 0x27, 0x9f, 0x33, 0xff, 0xac, 0xb2, 0x5f};
    XBH_U8 eq_10000HZ_10[] = {0x0a, 0xfc, 0x8a, 0xa1, 0xfd, 0xd8, 0x60, 0xcd, 0xfd, 0x56, 0xc3, 0x00, 0x02, 0x27, 0x9f, 0x33, 0xff, 0xac, 0xb2, 0x5f};

    XBH_U8* eqArrays_10000HZ[] = {
        eq_10000HZ_0, eq_10000HZ_1, eq_10000HZ_2, eq_10000HZ_3, eq_10000HZ_4,
        eq_10000HZ_5, eq_10000HZ_6, eq_10000HZ_7, eq_10000HZ_8, eq_10000HZ_9,
        eq_10000HZ_10
    };

    for(i = 0;i < sizeof(muteAdress);i++)
    {
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, muteAdress[i], 0x01, 0x01, &muteAdress_data[i]);
    }
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" set eqreg mute error!!");
        mWrite_amp_status = XBH_AMP_IDLE;
        return s32Ret;
    }

    //init eqreg
    for(i = 0;i < sizeof(eqInitAdress);i++)
    {
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, eqInitAdress[i], 0x01, 0x01, &eqInitAdress_data[i]);
    }
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" set eqreg init error!!");
        mWrite_amp_status = XBH_AMP_IDLE;
        return s32Ret;
    }

    switch (enEqMode)
    {
        case XBH_EQ_MODE_0:
        {
            //set eqreg
            for(i = 0;i < sizeof(eqAdress_120);i++)
            {
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, eqAdress_120[i], 0x01, 0x01, &eqArrays_120HZ[gainindex][i]);
                //XLOGD("mode 0 gainindex: %d ,reg_address: %x , reg_data %x", gainindex, eqAdress_120[i], eqArrays_120HZ[gainindex][i]);

            }
            if(s32Ret == XBH_FAILURE)
            {
                XLOGD("set XBH_EQ_MODE_0 eqreg error!!!!");
                mWrite_amp_status = XBH_AMP_IDLE;
                return s32Ret;
            }

            break;
        }
        case XBH_EQ_MODE_1:
        {
            for(i = 0;i < sizeof(eqAdress_500);i++)
            {
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, eqAdress_500[i], 0x01, 0x01, &eqArrays_500HZ[gainindex][i]);
                //XLOGD("mode 1 gainindex: %d ,reg_address: %x , reg_data %x", gainindex, eqAdress_500[i], eqArrays_500HZ[gainindex][i]);
            }
            if(s32Ret == XBH_FAILURE)
            {
                XLOGD(" set XBH_EQ_MODE_1 eqreg error!!!!");
                mWrite_amp_status = XBH_AMP_IDLE;
                return s32Ret;
            }

            break;
        }
        case XBH_EQ_MODE_2:
        {
            for(i = 0;i < sizeof(eqAdress_1500);i++)
            {
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, eqAdress_1500[i], 0x01, 0x01, &eqArrays_1500HZ[gainindex][i]);
                //XLOGD("mode 2 gainindex: %d ,reg_address: %x , reg_data %x", gainindex, eqAdress_1500[i], eqArrays_1500HZ[gainindex][i]);
            }
            if(s32Ret == XBH_FAILURE)
            {
                XLOGD(" set XBH_EQ_MODE_2 eqreg error!!!!");
                mWrite_amp_status = XBH_AMP_IDLE;
                return s32Ret;
            }

            break;
        }
        case XBH_EQ_MODE_3:
        {
            for(i = 0;i < sizeof(eqAdress_5000);i++)
            {
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, eqAdress_5000[i], 0x01, 0x01, &eqArrays_5000HZ[gainindex][i]);
                //XLOGD("mode 3 gainindex: %d ,reg_address: %x , reg_data %x", gainindex, eqAdress_5000[i], eqArrays_5000HZ[gainindex][i]);
            }
            if(s32Ret == XBH_FAILURE)
            {
                XLOGD("set XBH_EQ_MODE_3 eqreg error!!!!");
                mWrite_amp_status = XBH_AMP_IDLE;
                return s32Ret;
            }

            break;
        }
        case XBH_EQ_MODE_4:
        {
            for(i = 0;i < sizeof(eqAdress_10000);i++)
            {
                s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, eqAdress_10000[i], 0x01, 0x01, &eqArrays_10000HZ[gainindex][i]);
                //XLOGD("mode 4 gainindex: %d ,reg_address: %x , reg_data %x", gainindex, eqAdress_10000[i], eqArrays_10000HZ[gainindex][i]);
            }
            if(s32Ret == XBH_FAILURE)
            {
                XLOGD(" set XBH_EQ_MODE_4 eqreg error!!!!");
                mWrite_amp_status = XBH_AMP_IDLE;
                return s32Ret;
            }

            break;
        }
        default:
        {
            XLOGD("set XBH_EQ_MOD:%d eqreg out of mode error!!!!",enEqMode);
            mWrite_amp_status = XBH_AMP_IDLE;
            return XBH_FAILURE;
        }
    }

    for(i = 0;i < sizeof(muteAdress);i++)
    {
        s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, muteAdress[i], 0x01, 0x01, &playAdress_data[i]);
    }
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD(" set eqreg play error!!");
        mWrite_amp_status = XBH_AMP_IDLE;
        return s32Ret;
    }
    mWrite_amp_status = XBH_AMP_SUCCESS;
    XLOGD("set XBH_EQ_MOD:%d ,gainindex =%d eqreg success!!!!",enEqMode,gainindex);

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

XBH_S32 ChipTas5805m_Stereo::frqGainLevel(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR OldEqGainLevel[5][3] = {0};
    XBH_CHAR GainLevel[3] = {0};
    XBH_S32 gainindex = 0;

    if (s32Value < 0 || s32Value > 100) {
        sprintf(GainLevel, "%s", "5");
        gainindex = 5;
    }
    else if(s32Value % 10 == 0)
    {
        XBH_S32 level = s32Value / 10;
        sprintf(GainLevel, "%d", level);
        gainindex = level;
    }
    else
    {
        XBH_S32 level = s32Value / 10 + 1;
        sprintf(GainLevel, "%d", level);
        gainindex = level;
    }

    XLOGD("gainindex %d,%d",s32Value,gainindex);
    sprintf(setFrqValue, "%d", s32Value);

    switch (enEqMode)
    {
        case XBH_EQ_MODE_0:
        case XBH_EQ_MODE_1:
        case XBH_EQ_MODE_2:
        case XBH_EQ_MODE_3:
        case XBH_EQ_MODE_4:
        {
            if(strcmp(OldEqGainLevel[enEqMode], GainLevel) != 0)
            {
                sprintf(OldEqGainLevel[enEqMode], "%s", GainLevel);
                sprintf(setFrqProperty,"persist.vendor.xbh.frqgain.value.%u", enEqMode);
                property_set(setFrqProperty, setFrqValue);
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
        s32Ret = setEqReg(enEqMode, gainindex);
        if(s32Ret == XBH_SUCCESS)
        {
            XLOGD("success setEqReg=%d,gainindex=%d\n",enEqMode,gainindex);
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

