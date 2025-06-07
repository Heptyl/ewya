#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipAp8224"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipAp8224.h"
#include <hardware/board.h>

#define UPGRADE_PACK_SIZE 512

XbhMutex                 ChipAp8224::mLock;

/****************************
        codec init regs
****************************/
const static XBH_U8 Ap8224_EffectData[1715] =
{
    0xA5, 0x5A,/*sync head*/
    0x82,/*Mic Noise Suppressor*/
    0x0B,/*length*/
    0xFF,/*all parameter*/
    0x01, 0x00,/*enable*/
    0x6C, 0xEE,/*threshold*/
    0x03, 0x00,/*ratio*/
    0x03, 0x00,/*attack*/
    0x20, 0x03,/*release*/
    0x16,/*sync tail*/

    0xA5, 0x5A,/*sync head*/
    0x83,/*Mic Freq Shifter*/
    0x05,/*length*/
    0xFF,/*all parameter*/
    0x01, 0x00,/*enable*/
    0x04, 0x00,/*deltaf*/
    0x16,/*sync tail*/

    0xA5, 0x5A,/*sync head*/
    0x84,/*Mic Howling Dector*/
    0x05,/*length*/
    0xFF,/*all parameter*/
    0x01, 0x00,/*enable*/
    0x02, 0x00,/*suppression mode*/
    0x16,/*sync tail*/

    0xA5, 0x5A,/*sync head*/
    0x8D,/*Mic Silence Dector*/
    0x05,/*length*/
    0xFF,/*all parameter*/
    0x01, 0x00,/*enable*/
    0x0E, 0x00,/*amplitude*/
    0x16,/*sync tail*/

    0xA5, 0x5A,/*sync head*/
    0x03,/*PGA0*/
    0x25,/*length*/
    0xFF,/*all parameter*/
    0x00, 0x00,/*line1_left_enable*/
    0x00, 0x00,/*line1_right_enable*/
    0x00, 0x00,/*line2_left_enable*/
    0x00, 0x00,/*line2_right_enable*/
    0x01, 0x00,/*MIC4_enable*/
    0x01, 0x00,/*MIC3_enable*/
    0x33, 0x00,/*line1_left_gain*/
    0x33, 0x00,/*line1_right_gain*/
    0x33, 0x00,/*line2_left_gain*/
    0x33, 0x00,/*line2_right_gain*/
    0x10, 0x00,/*MIC4_gain*/
    0x00, 0x00,/*MIC4_gain_boost*/
    0x10, 0x00,/*MIC3_gain*/
    0x00, 0x00,/*MIC3_gain_boost*/
    0x00, 0x00,/*Zero_cross_enable*/
    0x00, 0x00,/*Diff_input_enable*/
    0x00, 0x00,/*Diff_left_gain*/
    0x00, 0x00,/*Diff_right_gain*/
    0x16,/*sync tail*/

    0xA5, 0x5A,/*sync head*/
    0x04,/*ADC0*/
    0x15,/*length*/
    0xFF,/*all parameter*/
    0x03, 0x00,/*enable*/
    0x00, 0x00,/*mute*/
    0x25, 0x14,/*left_volume*/
    0x25, 0x14,/*right_volume*/
    0x08, 0x00,/*sample_rate*/
    0x00, 0x00,/*LR_swap*/
    0x00, 0x00,/*hpc*/
    0x00, 0x00,/*fade_time*/
    0x00, 0x00,/*mclk_source*/
    0x01, 0x00,/*dc_blocker*/
    0x16,/*sync tail*/

    0xA5, 0x5A,/*sync head*/
    0x09,/*DAC0*/
    0x1D,/*length*/
    0xFF,/*all parameter*/
    0x03, 0x00,/*enable*/
    0x08, 0x00,/*sample_rate*/
    0x00, 0x00,/*mute*/
#if XHB_LINE_OUT_LEFT_VOLUME_1
    XHB_LINE_OUT_LEFT_VOLUME_1, XHB_LINE_OUT_LEFT_VOLUME_2,/*left_volume*/
    XHB_LINE_OUT_RIGHT_VOLUME_1, XHB_LINE_OUT_RIGHT_VOLUME_2,/*right_volume*/
#else
    0x01, 0x0D,/*left_volume*/
    0x01, 0x0D,/*right_volume*/
#endif
    0x00, 0x00,/*dither*/
    0x00, 0x00,/*scramble*/
    0x00, 0x00,/*mode*/
    0x00, 0x00,/*pause*/
    0x00, 0x00,/*sample_edge_mode*/
    0x00, 0x00,/*SCF_mute*/
    0x00, 0x00,/*fade_time*/
    0x00, 0x00,/*zero_num*/
    0x00, 0x00,/*mclk_source*/
    0x16,/*sync tail*/
};

static const uint16_t gCRC16TableNBS[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

/**
 * 复位codec芯片
*/
XBH_S32 ChipAp8224::resetChip()
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
 * 获取mic的插入状态
*/
XBH_S32 ChipAp8224::getMicPlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_bInitDone)
    {
        if(mMic_plug_gpio == -1)
        {
            *enable = XBH_FALSE;
            XLOGE(" can not define mic plug in gpio !!!!!");
            return s32Ret;
        }
        XBH_U32 u32GpioValue;
        XbhService::getModuleInterface()->getGpioInputValue(mMic_plug_gpio, &u32GpioValue);
        if(u32GpioValue == mMic_plug_level)
        {
            *enable = XBH_TRUE;
        }
        else
        {
            *enable = XBH_FALSE;
        }
        return XBH_SUCCESS;
    }
    else
    {
        XLOGE(" chip is not init !!!!!");
    }
    return s32Ret;
}

/**
 * 获取耳机的插入状态
*/
XBH_S32 ChipAp8224::getHeadphonePlugIn(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_bInitDone)
    {
        if(mHp_plug_gpio == -1)
        {
            *enable = XBH_FALSE;
            XLOGE(" can not define hp plug in gpio !!!!!");
            return s32Ret;
        }
        XBH_U32 u32GpioValue;
        XbhService::getModuleInterface()->getGpioInputValue(mHp_plug_gpio, &u32GpioValue);
        if(u32GpioValue == mHp_plug_level)
        {
            *enable = XBH_TRUE;
        }
        else
        {
            *enable = XBH_FALSE;
        }
        return XBH_SUCCESS;
    }
    else
    {
        XLOGE(" chip is not init !!!!!");
    }
    return s32Ret;
}

/**
 * 设置mic是否静音
*/
XBH_S32 ChipAp8224::setMuteMicIn(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_bInitDone)
    {
        if (mute)
        {
            Ap8224_Mic4Mute(XBH_TRUE);
            m_bMicMute = XBH_TRUE;
        }
        else
        {
            Ap8224_Mic4Mute(XBH_FALSE);
            m_bMicMute = XBH_FALSE;
        }
        XLOGE("mic detect mute = %d",mute);
        return XBH_SUCCESS;
    }
    else
    {
        XLOGE(" chip is not init !!!!!");
    }

    return s32Ret;
}
/**
 * 获取mic的静音状态
*/
XBH_S32 ChipAp8224::getMuteMicIn(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_bInitDone)
    {
        *mute = m_bMicMute;
        return XBH_SUCCESS;
    }
    else
    {
        XLOGE(" chip is not init !!!!!");
    }
    return s32Ret;
}
/**
 * 设置codec输出到speaker的那一路是否静音
*/
XBH_S32 ChipAp8224::setSpeakerOut(XBH_BOOL mute)
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
 * 获取输出到speaker的那一路是否静音
*/
XBH_S32 ChipAp8224::getSpeakerOut(XBH_BOOL* mute)
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
 * 设置line out是否静音
*/
XBH_S32 ChipAp8224::setMuteDAC0(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }

    XLOGD("setMuteDAC0 AP8232 mute:%d", mute);
    XBH_U8 len;
    len = 0;
    XBH_U8 reg_data[40] = {0};
    reg_data[len++] = 0xA5;
    reg_data[len++] = 0x5A;
    reg_data[len++] = 0x09;/*DAC0*/
    reg_data[len++] = 0x03;/*length*/
    reg_data[len++] = 2;/*index = 0:  all parameter*/
    reg_data[len++] = mute?0x03:0x00;/*mute L*/
    reg_data[len++] = 0x00;/*mute H*/

    s32Ret = Ap8224_Write(reg_data, len);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGD("setMuteDAC0:Ap8224_Write failure %d\n",s32Ret);
    }
    return s32Ret;
}
/**
 * 获取line out是否静音
*/
XBH_S32 ChipAp8224::getMuteDAC0(XBH_BOOL* mute)
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
 * 设置耳机输出是否静音
*/
XBH_S32 ChipAp8224::setHeadphoneMute(XBH_BOOL mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(m_bInitDone)
    {
       if (mute)
        {
            XbhService::getModuleInterface()->setGpioOutputValue(mHp_mute_gpio, mHp_mute_level);
            setMuteDAC0(XBH_TRUE);
            m_bHpMute = XBH_TRUE;
        }
        else
        {
            XbhService::getModuleInterface()->setGpioOutputValue(mHp_mute_gpio, !mHp_mute_level);
             setMuteDAC0(XBH_FALSE);
            m_bHpMute = XBH_FALSE;
        }
        XLOGE("Hp detect mute = %d",mute);
        return XBH_SUCCESS;

    }
    else
    {
        XLOGE(" chip is not init !!!!!");
    }
    return s32Ret;
}
/**
 * 获取耳机输出是否静音
*/
XBH_S32 ChipAp8224::getHeadphoneMute(XBH_BOOL* mute)
{
    XBH_S32 s32Ret = XBH_FAILURE;
     if(m_bInitDone)
    {
        *mute = m_bHpMute;
        return XBH_SUCCESS;
    }
    else
    {
        XLOGE(" chip is not init !!!!!");
    }
    return s32Ret;
}

XBH_S32 ChipAp8224::i2c_data_write_nbytes(XBH_U32 u32RegAddr, XBH_U32 RegAddrLen, XBH_U8 *p_data, XBH_U32 data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

XBH_S32 ChipAp8224::i2c_data_read_nbytes(XBH_U32 u32RegAddr, XBH_U32 RegAddrLen, XBH_U8 *p_data, XBH_U32 data_len)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, u32RegAddr, RegAddrLen, data_len, p_data);
    return s32Ret;
}

/*
****************************************************************
* crc16 校验计算
* buf:数据缓存首地址，
* length:缓存数据长度 <串口通讯及命令列表.xls>中Len_H、Len_L、Cmd_H、Cmd_L和Data部分
* return:0 校验失败
****************************************************************
*/
XBH_U16 ChipAp8224::GetCRC16NBS(XBH_U8* data, XBH_U32 length)
{
    if(data == NULL)
    {
      //  XLOGE("Func[%s] BUFF null", __func__);
        return 0;
    }

    XBH_U16 crc = 0;
    XBH_U8 temp;
    XBH_U8 *pData = data;

    while(length--)
    {
        temp  = (XBH_U8)(crc >> 8);      // get high byte
        crc <<= 8;
        crc  ^= gCRC16TableNBS[*pData ^ temp];   // get low byte
        pData++;
    }

    // XLOGD("Get crc value:[0x%x]", crc);
    return crc;
}

/*
****************************************************************
* 发送DU系列的升级固件
****************************************************************
*/
XBH_BOOL ChipAp8224::Ap8224_SendFileByIIC(XBH_U32 offset, XBH_U8 *buff, XBH_U32 lenght, XBH_U8 *pRepyFlag)
{
    XBH_U16 len = 0;
    XBH_U16 temp = 0;
    XBH_U16 crc = 0;
    XBH_U16 payloadlen = lenght + 3;
    XBH_U8 m_u8TxBuff[2048] = {0};
    XBH_U8 m_u8RxBuff[2048] = {0};

    memset(m_u8TxBuff, 0, sizeof(m_u8TxBuff));
    memset(m_u8RxBuff, 0, sizeof(m_u8RxBuff));
    //HEAD
    m_u8TxBuff[len++] = 0x5A;
    m_u8TxBuff[len++] = 0xFE;

    m_u8TxBuff[len++] = (payloadlen >> 8) & 0xFF;
    m_u8TxBuff[len++] = payloadlen & 0xFF;
    m_u8TxBuff[len++] = 0x70;
    m_u8TxBuff[len++] = 0x34;

    m_u8TxBuff[len++] = (offset >> 16) & 0xFF;
    m_u8TxBuff[len++] = (offset >> 8) & 0xFF;
    m_u8TxBuff[len++] = offset & 0xFF;

    memcpy(&m_u8TxBuff[len], buff, lenght);
    temp=len;
    len += lenght;

    //Get crc16
    crc = GetCRC16NBS((&m_u8TxBuff[temp]), lenght);
    if(crc > 0)
    {
        m_u8TxBuff[len++] = (crc >> 8) & 0xFF;
        m_u8TxBuff[len++] = (crc & 0xFF);
    }

    i2c_data_write_nbytes(0xA5, 1,m_u8TxBuff, len);
    usleep(20 * 1000);
    i2c_data_read_nbytes(0xA5, 1,m_u8RxBuff, 13);
    if((m_u8RxBuff[0] != 0x55 || m_u8RxBuff[1] != 0xAA)
        && (m_u8RxBuff[0] != 0xA5 || m_u8RxBuff[1] != 0x5A))
    {
        return XBH_FAILURE;
    }
    *pRepyFlag = m_u8RxBuff[7];

    return XBH_SUCCESS;
}

XBH_S32 ChipAp8224::getFwAndEffsetVer(XBH_CHAR *FWVer, XBH_CHAR *EffsetVer)
{
    XBH_S32 len=0;
    XBH_S32 VER = 0;

    memset(m_u8TxBuff, 0, sizeof(m_u8TxBuff));
    memset(m_u8RxBuff, 0, sizeof(m_u8RxBuff));

    m_u8TxBuff[len++] = 0x5A;
    m_u8TxBuff[len++] = 0x00;
    m_u8TxBuff[len++] = 0x00;
    //cmdid
    m_u8TxBuff[len++] = 0x16;

    i2c_data_write_nbytes(0xA5, 1,m_u8TxBuff, len);
    usleep(20 * 1000);
    i2c_data_read_nbytes(0xA5, 1,m_u8RxBuff, 11);

    VER = (m_u8RxBuff[5] << 16 | m_u8RxBuff[6] << 8 | m_u8RxBuff[7]);
    sprintf(FWVer, "%x", VER);
    XLOGI("%s FWVer=%s***************", __func__, FWVer);
    return XBH_SUCCESS;
}

//1.读取升级固件数据
XBH_S32 ChipAp8224::getPresetFileData(XBH_U32* u32FileLen, XBH_U8** pFileBuff)
{
    XBH_U32 u32FileSize = 0;
    FILE *pFile =  NULL;
    XBH_U8* pU8FileBuff = NULL;
    //---------------------- open upgrade file ----------------------
    XLOGI("---> open Preset_file");
    if ((pFile = fopen(mFileName, "rb")) == NULL){
        XLOGE("Error:Open %s file fail!", mFileName);
        return XBH_FAILURE;
    }

    //---------------------- read upgrade file ----------------------
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

    XLOGI("---> close upgrade file");
    fclose(pFile);

    *u32FileLen = u32FileSize;
    *pFileBuff = pU8FileBuff;

    return XBH_SUCCESS;
}

/*
****************************************************************
* 发送DU系列的升级固件大小，擦除SPI flash
****************************************************************
*2. Send iap mode
*/
XBH_S32 ChipAp8224::Ap8224_SendFileSizeByIIC(XBH_U32 filesize)
{
    XBH_U16 len = 0;
    XBH_U16 crc = 0;

    memset(m_u8RxBuff, 0, sizeof(m_u8RxBuff));
    memset(m_u8TxBuff, 0, sizeof(m_u8TxBuff));
    //HEAD
    m_u8TxBuff[len++] = 0x5A;
    m_u8TxBuff[len++] = 0xFE;
    //payloadlen
    m_u8TxBuff[len++] = 0x00;
    m_u8TxBuff[len++] = 0x03;
    //cmdid
    m_u8TxBuff[len++] = 0x70;
    m_u8TxBuff[len++] = 0x33;
    //payload
    m_u8TxBuff[len++] = (filesize >> 16) & 0xFF;
    m_u8TxBuff[len++] = (filesize >> 8) & 0xFF;
    m_u8TxBuff[len++] = filesize & 0xFF;
    //Get crc16
    crc = GetCRC16NBS((&m_u8TxBuff[2]), len-2);
    if(crc > 0)
    {
        m_u8TxBuff[len++] = (crc >> 8) & 0xFF;
        m_u8TxBuff[len++] = (crc & 0xFF);
    }

    i2c_data_write_nbytes(0xA5, 1,m_u8TxBuff, len);

    return XBH_SUCCESS;
}

//3、写入mcu数据
XBH_S32 ChipAp8224::downloadPresetFirewareData(XBH_U8* pFileBuff, XBH_U32 u32FileLen)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_BOOL ret = XBH_FALSE;

    XBH_U8 u8RepyFlag = 0;
    XBH_U32 u32Address = 0;
    XBH_U32 u32FileSize = u32FileLen;
    XBH_U32 u32FileIndex = 0;
    XBH_U32 u32DataLen = 0;
    XBH_U8  u8TmpBuff[UPGRADE_PACK_SIZE] = {0};
    XBH_U8  u8FailTime = 0;

    XLOGI("---> write data u32FileSize = %d", u32FileSize);
    while(u32FileSize > 0)
    {
        if(u32FileSize >= UPGRADE_PACK_SIZE){
            u32DataLen = UPGRADE_PACK_SIZE;
        } else {
            u32DataLen = u32FileSize;
        }
        memset(u8TmpBuff, 0x00, sizeof(u8TmpBuff));
        memcpy(&u8TmpBuff[0], &pFileBuff[u32FileIndex], u32DataLen);

        s32Ret = Ap8224_SendFileByIIC(u32Address, u8TmpBuff, u32DataLen, &u8RepyFlag);
        if(s32Ret != XBH_SUCCESS){
            XLOGE("func[%s]  Send File Error !!", __func__);
            break;
        }

        //check Rx Datas
        //写入成功
        if(u8RepyFlag == FLASH_UPGRADE_SUCCESS || u8RepyFlag == FLASH_WRITE_REPEAT) {
            u32Address += u32DataLen;
            u32FileIndex += u32DataLen;
            u32FileSize -= u32DataLen;
        }
        //持续重写数据过多、连接超时
        else if(u8RepyFlag == FLASH_RESEND_TIMEOUT || u8RepyFlag ==  FLASH_WRITE_FAIL) {
            XLOGE("func[%s] PersetFile write TimeOut ", __func__);
            s32Ret = XBH_FAILURE;
            if(u8FailTime++>5){
                break;
            }
        }
        else if(u8RepyFlag == FLASH_CONNECT_TIMEOUT ) {
            //Send iap mode
            if(s32Ret != XBH_SUCCESS)
            {
                XLOGE("FLASH_CONNECT_TIMEOUT Set Iap mode Fail !!!");
                if(u8FailTime++>5){
                    break;
                }
            }
        }
        //写入完成
        else if(u8RepyFlag == FLASH_UPGRADE_FINISH) {
            XLOGI("func[%s] PersetFile write Finish ", __func__);
            u32Address += u32DataLen;
            u32FileIndex += u32DataLen;
            u32FileSize -= u32DataLen;
            s32Ret = XBH_SUCCESS;
        }
        //写入数据异常重新发送当前数据
        else if(u8RepyFlag == FLASH_UPGRADE_FAIL || u8RepyFlag == FLASH_REWRIT_TIMEOUT) {
            XLOGI("func[%s] Rewrite Data ", __func__);
        }
        else {
            XLOGE("func[%s] line[%d] dump buffer ", __func__, __LINE__);
        }

        //XLOGD("u32Address:[%d] u32FileIndex:[%d] u32FileSize:[%d] ",u32Address,  u32FileIndex, u32FileSize);
    }

    if (s32Ret == XBH_SUCCESS){
        XLOGI("Func[%s] - Line[%d] XBH_SUCCESS!", __func__, __LINE__);
    } else {
        XLOGE("Func[%s] - Line[%d] Error: writeData fail!", __func__, __LINE__);
    }

   return s32Ret;
}

/**
 * 获取固件版本
 */
XBH_S32 ChipAp8224::getFirmwareVersion(XBH_CHAR* strVersion)
{
    XBH_S32 len=0;

    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return XBH_FAILURE;
    }
    if(strVersion == XBH_NULL) {
        XLOGD("===> getFirmwareVersion strVersion is NULL");
        return XBH_FAILURE;
    }
    if (mState == XBH_UPGRADE_RUNNING)
    {
        XLOGW("Warning: Ap8224 is updating...");
        return XBH_FAILURE;
    }

    memset(m_u8TxBuff, 0, sizeof(m_u8TxBuff));
    memset(m_u8RxBuff, 0, sizeof(m_u8RxBuff));

    m_u8TxBuff[len++] = 0x5A;
    m_u8TxBuff[len++] = 0x00;
    m_u8TxBuff[len++] = 0x00;
    //cmdid
    m_u8TxBuff[len++] = 0x16;

    i2c_data_write_nbytes(0xA5, 1,m_u8TxBuff, len);
    usleep(20 * 1000);
    i2c_data_read_nbytes(0xA5, 1,m_u8RxBuff, 11);

    sprintf(strVersion, "%x.%x.%x", m_u8RxBuff[5], m_u8RxBuff[6], m_u8RxBuff[7]);
    XLOGI("%s VER=T%s***************", __func__, strVersion);

    return XBH_SUCCESS;
}
/**
 * 进行固件升级
 */
XBH_S32 ChipAp8224::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }

    if (strFilePath == XBH_NULL)
    {
        XLOGE("Error: fileName is XBH_NULL");
        return s32Ret;
    }

    if (mState == XBH_UPGRADE_RUNNING)
    {
        XLOGW("Warning: Ap8224 is updating...");
        return s32Ret;
    }

    mState = XBH_UPGRADE_RUNNING;
    //set force upgrade flag
    mForceUpgrade = bForceUpgrade;

    //set upgrade file name
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
 * 获取固件升级时的状态
 */
XBH_S32 ChipAp8224::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    *s32State = mState;
    return XBH_SUCCESS;
}


/*
**********************************************************************
* AP8224 write,写入控制字的内容,输入的内容参照协议 <固件与PC机通信协议V2.x.x.pdf>中IIC部分
* buf:输入数据缓存首地址，
* len:输入数据的长度
* return:0 写入失败    1：写入成功
**********************************************************************
*/
XBH_S32 ChipAp8224::Ap8224_Write(XBH_U8 *buf, XBH_U8 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 regAddr = buf[2];  // control register
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, regAddr, 1, len, buf);
    if (s32Ret == XBH_FAILURE)
    {
        XLOGE("i2c-bus : %d, slave addr : 0x%x fail : %d\n", mI2cBus, mI2cAddr, s32Ret);
        return s32Ret;
    }

    XLOGD("regAddrr : 0x%x, data len = %d\n", regAddr, len);
    usleep(20*1000); //add delay for stable
    return s32Ret;
}

/*
****************************************************************
* Ap8224 read,读取相关的控制字的内容
* buf:输出数据缓存首地址，
* ctrl:控制字，参照协议规范中的定义 <固件与PC机通信协议V2.x.x.pdf>中IIC部分
* return:0 读取失败    1：读取成功
****************************************************************
*/
XBH_S32 ChipAp8224::Ap8224_Read(XBH_U8 *buf, XBH_U8 ctrl, XBH_U8 len)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 reg_data[40] = {0};
    
    reg_data[0] = 0xA5;
    reg_data[1] = 0x5A;
    reg_data[2] = ctrl;
    reg_data[3] = 0x00;
    reg_data[4] = 0x16;

    s32Ret = Ap8224_Write(reg_data, 5);
    if (s32Ret == XBH_FAILURE)
    {
        XLOGE("Ap8224_Read in Ap8224_Write faile:%d\n",s32Ret);
        return s32Ret;
    }
    usleep(5 * 1000);
    //read all data
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, ctrl, 1, len, buf);
    if (s32Ret == XBH_FAILURE)
    {
        XLOGE("getI2cData faile:%d\n",s32Ret);
    }
    return s32Ret;
}

XBH_S32 ChipAp8224::Ap8224_LoadEffect()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U16 k = 0;
    XBH_U16 packet_len = 0;
    XBH_U16 cmd_len = 0;
    XBH_U16 position = 0;
    XBH_U16 len = sizeof(Ap8224_EffectData);
    XBH_U8 end_code = 0;
    XBH_U8 communic_buf[256];

    XLOGD("%s %d \n",__func__,__LINE__);

    while (position < len)
    {
        if ((Ap8224_EffectData[position] == 0xa5) && (Ap8224_EffectData[position + 1] == 0x5a))
        {
            //Control    = buff[position + 2];//
            packet_len = Ap8224_EffectData[position + 3]; //length
            end_code   = Ap8224_EffectData[position + 4 + packet_len];//0x16

            if(end_code == 0x16)////end code ok
            {
                cmd_len = packet_len + 5;

                for(k = 0; k < cmd_len; k++)
                {
                    communic_buf[k] = Ap8224_EffectData[position++];
                    //XLOGD(" Write : %02X\n",communic_buf[k]);
                }

                //if(Control != 0x80)
                {
                  s32Ret = Ap8224_Write(&communic_buf[0], packet_len + 4); //include length byte and header
                }
            }
            else  // end code error
            {
                position++;
            }
        }
        else // serch start code....
        {
            position++;
        }
    }
    return s32Ret;
}

XBH_S32 ChipAp8224::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    m_bInitDone = XBH_FALSE;
    XBH_U8 unmuteMode = 0x00;
    //TODO
    XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_TRUE);
    if(mHp_mute_gpio >= 0)
    {
        XLOGD("hp mute");
        XbhService::getModuleInterface()->setGpioOutputValue(mHp_mute_gpio, mHp_mute_level);
    }
    if(mPgpio >= 0)
    {
        XLOGD("Ap8224 power enable");
        XbhService::getModuleInterface()->setGpioOutputValue(mPgpio, mPLevel);
        usleep(3000 * 1000);
    }
    if (mI2cBus == -1 && mI2cAddr == -1)
    {
        XLOGD("Ap8224 not exist");
        m_bInitDone = XBH_TRUE;
        return s32Ret;
    }
    s32Ret = Ap8224_LoadEffect();
    s32Ret |= Ap8224_Mic4Mute(XBH_TRUE);
    //解mute统一放到XbhAudioDetectManager里面处理，当判断codec和amp都初始化完成后再解mute
    //XbhService::getPlatformInterface()->setMute(XBH_AUDIO_CHANNEL_E::XBH_AUDIO_CHANNEL_SPEAKER, XBH_FALSE);
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

XBH_S32 ChipAp8224::Ap8224_Mic4Mute(XBH_BOOL mute)
{
    XBH_U8 reg_data[20] = {0};
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("mute:%d", mute);
#if 0
    reg_data[0] = 0xA5;
    reg_data[1] = 0x5A;
    reg_data[2] = 0x04; /* adc0 */
    reg_data[3] = 0x03;
    reg_data[4] = 0x01; /* mute setting */
    reg_data[5] = mute ? 0x03 : 0x00;
    reg_data[6] = 0x00;
    reg_data[7] = 0x16;

    s32Ret = Ap8224_Write(reg_data, 8);
#endif
    return s32Ret;
}

XBH_S32 ChipAp8224::waitAP8224Reboot()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 timeout = 15;  //timeout 15s
    XBH_CHAR FWVer[10] = {0};
    XBH_CHAR EffVer[10] = {0};

    while(timeout-- > 0)
	{
        XLOGE("WAIT AP8224 Reboot ...");
        getFwAndEffsetVer(FWVer,EffVer);
        if(strcmp(FWVer, "0")) {
            break;
        }
        usleep(1000 * 1000);
    }
    return s32Ret;
}

XBH_S32 ChipAp8224::startUpgradeFireware(XBH_VOID)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8* pPresetFile = NULL;
    XBH_U32 u32PresetFileLen = 0;
    XBH_CHAR FWVer[10] = {0};
    XBH_CHAR EffVer[10] = {0};

    mState = XBH_UPGRADE_RUNNING;

    XLOGI("[%d] Preset File path:[%s]", __LINE__, mFileName);

    getFwAndEffsetVer(FWVer,EffVer);

    s32Ret = getPresetFileData(&u32PresetFileLen, &pPresetFile);
    if(s32Ret != XBH_SUCCESS){
        XLOGE("Get data Fail !!!");
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }
    XLOGI("Read Preset Fireware Len:[%d]", u32PresetFileLen);

    //Send iap mode
    s32Ret = Ap8224_SendFileSizeByIIC(u32PresetFileLen);
    usleep(1000 * 1000);
    s32Ret = downloadPresetFirewareData(pPresetFile, u32PresetFileLen);
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGE("Download firmware Fail !!!");
        free(pPresetFile);
        mState = XBH_UPGRADE_FAILURE;
        return XBH_FAILURE;
    }

    XLOGI("Download Firmware Finish");
    usleep(3000 * 1000);
    s32Ret |= waitAP8224Reboot();
    if(s32Ret != XBH_SUCCESS)
    {
        XLOGE("Wait AP8224 Reboot failed !!!");
        return XBH_FAILURE;
    }
    mState = XBH_UPGRADE_SUCCESS;
    return XBH_SUCCESS;
}

/**
 * 设置line out的增益
*/
XBH_S32 ChipAp8224::setLineOutGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取line out的增益
*/
XBH_S32 ChipAp8224::getLineOutGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 设置mic in的增益
*/
XBH_S32 ChipAp8224::setMicInGain(XBH_S32 gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取mic in的增益
*/
XBH_S32 ChipAp8224::getMicInGain(XBH_S32* gain)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 设置MIC的防啸叫功能开关
*/
XBH_S32 ChipAp8224::setMicHowling(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取MIC的防啸叫功能开关
*/
XBH_S32 ChipAp8224::getMicHowling(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}

/**
 * 获取chip是否初始化完成
*/
XBH_S32 ChipAp8224::getChipInitDone(XBH_BOOL* enable)
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
XBH_S32 ChipAp8224::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *enable = m_bChipExsit;
    return s32Ret;
}

void ChipAp8224::run(const void* arg)
{
    if(XBH_UPGRADE_RUNNING == mState)
    {
        startUpgradeFireware();
    }
}

ChipAp8224::ChipAp8224(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 uart, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel, XBH_S32 mic_plug_gpio, XBH_S32 mic_plug_level, XBH_S32 hp_plug_gpio, XBH_S32 hp_plug_level, XBH_S32 hp_mute_gpio, XBH_S32 hp_mute_level)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    mUart = uart;
    mPgpio = pGpio;
    mPLevel = pLevel;
    mRgpio = rGpio;
    mRLevel = rLevel;
    mMic_plug_gpio = mic_plug_gpio;
    mMic_plug_level = mic_plug_level;
    mHp_plug_gpio = hp_plug_gpio;
    mHp_plug_level = hp_plug_level;
    mHp_mute_gpio = hp_mute_gpio;
    mHp_mute_level = hp_mute_level;
    mState = XBH_UPGRADE_IDLE;
    m_bChipExsit = init() == XBH_SUCCESS;
    XLOGD(" end ");
}

ChipAp8224::~ChipAp8224()
{

}


