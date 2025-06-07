#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipEs7243"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipEs7243.h"
#include <hardware/board.h>

XbhMutex                 ChipEs7243::mLock;


/***************参数定义***************/
#define CHIPID              0x7A431x        //Read 0xFD/FE/FF == 0x7A431X，判断IC型号是否为ES7243E
#define STATEconfirm        0xFC            //状态机确认 回读STATEconfirm的寄存值是否为0x20，确认IC正常工作状态

#define NORMAL_I2S          0x00
#define NORMAL_LJ           0x01
#define NORMAL_DSPA         0x03
#define NORMAL_DSPB         0x23
#define Format_Len24        0x00
#define Format_Len20        0x01
#define Format_Len18        0x02
#define Format_Len16        0x03
#define Format_Len32        0x04

#define VDDA_3V3            0x00
#define VDDA_1V8            0x01
#define MCLK_PIN            0x00
#define SCLK_PIN            0x01
/***************参数定义***************/

/***************参数选择***************/
#define CHANNELS_MAX                2               //TDM_NFS模式下配置,多少CH做级联,偶数
#define MSMode_MasterSelOn          1               //产品主从模式选择:默认选择0为SlaveMode,打开为1选择MasterMode
#define Ratio                       256             //实际Ratio=MCLK/LRCK比率，需要和实际时钟比例匹配
#define Format                      NORMAL_I2S      //数据格式选择,需要和实际时序匹配
#define Format_Len                  Format_Len16    //数据长度选择,需要和实际时序匹配
#define TDMINSelon                  0               //TDMIN级联模式选择:默认选择关闭0,打开为1
#define SCLK_DIV                    4               //SCLK分频选择:(选择范围1~128),SCLK=MCLK/SCLK_DIV
#define SCLK_INV                    0               //默认对齐方式为下降沿,1为上升沿对齐,需要和实际时序匹配
#define MCLK_SOURCE                 MCLK_PIN        //是否硬件没接MCLK需要用SCLK当作MCLK

#define VDDA_VOLTAGE                VDDA_3V3        //模拟电压选择为3V3还是1V8,需要和实际硬件匹配
#define ADC_PGA_GAIN                0               //ADC模拟增益:(选择范围0~14),具体对应关系见相应DS说明
#define ADC_Volume                  191             //ADC数字增益:(选择范围0~255),191:0DB,±0.5dB/Step
#define Dmic_Selon                  0               //DMIC选择:默认选择关闭0,打开为1
#define Dmic_GAIN                   0               //DMIC增益:(选择范围0~7),6dB/Step
/***************参数选择***************/

/**
 * 复位audioToI2s芯片
*/
XBH_S32 ChipEs7243::resetChip()
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
 * 获取chip是否初始化完成
*/
XBH_S32 ChipEs7243::getChipInitDone(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取chip是否存在
*/
XBH_S32 ChipEs7243::getChipExist(XBH_BOOL* enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return s32Ret;
}
/**
 * 获取固件版本
 */
XBH_S32 ChipEs7243::getFirmwareVersion(XBH_CHAR* strVersion)
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
 * 进行固件升级
 */
XBH_S32 ChipEs7243::upgradeFirmware(const XBH_CHAR* strFilePath, XBH_BOOL bForceUpgrade)
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
 * 获取固件升级时的状态
 */
XBH_S32 ChipEs7243::getUpgradeState(XBH_S32 * s32State)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if(!m_bInitDone)
    {
        XLOGE(" chip is not init !!!!!");
        return s32Ret;
    }
    return s32Ret;
}

XBH_S32 ChipEs7243::Es7243_Write(XBH_U8 reg, XBH_U8 data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, reg, 1, 1, &data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGE(" write  %d  data = 0x%x error ", reg, data);
    }
    return s32Ret;
}

XBH_S32 ChipEs7243::Es7243_Read(XBH_U8 reg, XBH_U8* data)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, reg, 1, 1, data);
    if(s32Ret == XBH_FAILURE)
    {
        XLOGE(" read  %d error ", reg);
    }
    return s32Ret;
}

XBH_S32 ChipEs7243::getChipId()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    m_bInitDone = XBH_FALSE;
    //#define CHIPID    0x7A431x    //Read 0xFD/FE/FF == 0x7A431X，判断IC型号是否为ES7243E
    XBH_U8 u8Data = 0x00;
    s32Ret |= Es7243_Read(0xFD, &u8Data);
    if (u8Data == 0x7A)
    {
        XLOGD(" Found ES7243E ");
    }
    else if(u8Data == 0x72)
    {
        XLOGD(" Found ES7243L ");
    }
    else
    {
        XLOGD(" not Found ES7243L ");
        return XBH_FAILURE;
    }
    #if 0
    s32Ret |= Es7243_Read(0xFE, &u8Data);
    if (u8Data != 0x43)
    {
        return XBH_FAILURE;
    }
    s32Ret |= Es7243_Read(0xFF, &u8Data);
    if (u8Data != 0x01)
    {
        return XBH_FAILURE;
    }
    #endif
    XLOGD(" Found es7243e ");
    m_bInitDone = XBH_TRUE;
    return s32Ret;
}

XBH_S32 ChipEs7243::init()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret |= Es7243_Write(0x01, 0x3A);
    s32Ret |= Es7243_Write(0x00, 0x80);
    s32Ret |= Es7243_Write(0xF9, 0x00);
    s32Ret |= Es7243_Write(0x04, 0x02);
    s32Ret |= Es7243_Write(0x04, 0x01);
    s32Ret |= Es7243_Write(0xF9, 0x01);
    s32Ret |= Es7243_Write(0x00, 0x1E);
    s32Ret |= Es7243_Write(0x01, 0x00);
    if(Ratio == 64)//Ratio=MCLK/LRCK=64：3M072/48K；1M024/16K; 512K/8K
    {//8K采样率下频率太低注意DVDD接1V8
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x04, 0x03);//X4
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0xF9, 0x00);
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 128)//Ratio=MCLK/LRCK=128：6M144/48K；2M048/16K; 1M024/8K
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x04, 0x13);//*4/2
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0xF9, 0x00);
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 256)//Ratio=MCLK/LRCK=256：12M288/48K；4M096/16K; 2M048/8K
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x04, 0x01);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 384)//Ratio=MCLK/LRCK=384：12M288/32K；6M144/16K；3M072/8K；
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0xF9, 0x00);
        s32Ret |= Es7243_Write(0x04, 0x22);//384/3*4=512
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 400)//Ratio=MCLK/LRCK=400：19M2/96K?
    {
        s32Ret |= Es7243_Write(0x03, 0x19);//D25
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x04, 0x00);
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 512)//Ratio=MCLK/LRCK=512：12M288/24K；6M144/12K；4M096/8K；
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x04, 0x00);//512=512
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 768)//Ratio=MCLK/LRCK=768：12M288/16K；6M144/8K；
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x04, 0x21);//768/3*2=512
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 800)//Ratio=MCLK/LRCK=800：19M2/48K
    {
        s32Ret |= Es7243_Write(0x03, 0x19);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x04, 0x10);
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 1024)//Ratio=MCLK/LRCK=512：12M288/12K；
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x04, 0x10);//1024/2=512
        s32Ret |= Es7243_Write(0x05, 0x00);
    }
    if(Ratio == 1536)//Ratio=MCLK/LRCK=1536：12M288/8K；
    {
        s32Ret |= Es7243_Write(0x03, 0x20);
        s32Ret |= Es7243_Write(0x0D, Dmic_GAIN);
        s32Ret |= Es7243_Write(0x04, 0x20);//1536/3=512
        s32Ret |= Es7243_Write(0x05, 0x00);
    }

    //Es7243_Write(0x06,SCLK_DIV - 1);    //SCLK DIV
    //Es7243_Write(0x07,(Ratio >> 8));      //LRCK DIV
    //Es7243_Write(0x08,(Ratio & 0xFF));  //LRCK DIV

    //设置SCLK输出为3.07M
    s32Ret |= Es7243_Write(0x06, 0x03);    //SCLK DIV
    //设置LCLK输出为48K
    s32Ret |= Es7243_Write(0x08, 0xff);    //LRCK DIV
    s32Ret |= Es7243_Write(0x07, 0x00);    //LRCK DIV
    
    s32Ret |= Es7243_Write(0x02, (MCLK_SOURCE << 7) + SCLK_INV);

    s32Ret |= Es7243_Write(0x09, 0xD0);//S1_TIME
    s32Ret |= Es7243_Write(0x0A, 0xD0);//S2_TIME
    s32Ret |= Es7243_Write(0x0B, 0xC0 + Format + (Format_Len << 2));
    s32Ret |= Es7243_Write(0x0C, 0x00 + (TDMINSelon << 3));
#if XBH_ADC_Volume
    s32Ret |= Es7243_Write(0x0E, XBH_ADC_Volume);
#else
    s32Ret |= Es7243_Write(0x0E, ADC_Volume);
#endif
    s32Ret |= Es7243_Write(0x10, 0x38);
    s32Ret |= Es7243_Write(0x11, 0x16);
    s32Ret |= Es7243_Write(0x14, 0x0C);
    s32Ret |= Es7243_Write(0x15, 0x0C);
    s32Ret |= Es7243_Write(0x17, 0x02);
    s32Ret |= Es7243_Write(0x18, 0x26);

    s32Ret |= Es7243_Write(0x0F, 0x80);
    s32Ret |= Es7243_Write(0x19, 0x77);
    s32Ret |= Es7243_Write(0x1F, (Dmic_Selon << 5) + 0x0C - 0x05 * VDDA_VOLTAGE); //3V3配置0C，1V8配置07

    s32Ret |= Es7243_Write(0x1A, 0xF4);
    s32Ret |= Es7243_Write(0x1B, 0x66);
    s32Ret |= Es7243_Write(0x1C, 0x44);
    s32Ret |= Es7243_Write(0x1E, 0x00);
#if XBH_ADC_PGA_GAIN
    s32Ret |= Es7243_Write(0x20, 0x10 + XBH_ADC_PGA_GAIN);//PGA 模拟增益:
    s32Ret |= Es7243_Write(0x21, 0x10 + XBH_ADC_PGA_GAIN);//PGA 模拟增益:
#else     
    s32Ret |= Es7243_Write(0x20, 0x10 + ADC_PGA_GAIN);//PGA 模拟增益:
    s32Ret |= Es7243_Write(0x21, 0x10 + ADC_PGA_GAIN);//PGA 模拟增益:
#endif
    s32Ret |= Es7243_Write(0x00, 0x80 + (MSMode_MasterSelOn << 6));//Slave  Mode

    s32Ret |= Es7243_Write(0x01, 0x3A);
    s32Ret |= Es7243_Write(0x16, 0x3F);
    s32Ret |= Es7243_Write(0x16, 0x00);
    usleep(5 * 1000);
    s32Ret |= Es7243_Write(0x0B, Format + (Format_Len << 2));

    if(s32Ret != XBH_SUCCESS)
    {
        XLOGE(" init failed !!!!!!!!!!!!!!!!!!! ");
    }
    else
    {
        XLOGE(" init success !!!!!!!!!!!!!!!!!!! ");
    }
    return s32Ret;
}

void ChipEs7243::run(const void* arg)
{
    //usleep(3 * 1000 * 1000);
    init();
}

ChipEs7243::ChipEs7243(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGD(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;
    getChipId();
    if(m_bInitDone)
    {
        XbhMWThread::run();
    }
    XLOGD(" end ");
}

ChipEs7243::~ChipEs7243()
{

}


