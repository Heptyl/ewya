#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@ChipMcuNfc"
#include "XbhLog.h"

#include "XbhService.h"
#include "ChipMcuNfc.h"
#include "cutils/properties.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <hardware/board.h>

XbhMutex                 ChipMcuNfc::mLock;

XBH_S32 ChipMcuNfc::resetChip()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 ChipMcuNfc::getChipInitDone(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGW("this func is not implement");
    return  s32Ret;
}

XBH_S32 ChipMcuNfc::getChipExist(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGE("Func[%s] - Line[%d] \n\n", __func__, __LINE__);
    *enable = XBH_FALSE;

    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_IS_NFC_EXIST;
    pI2cBuff.len = 10;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
        
    *enable = (XBH_BOOL)pI2cBuff.data[0];

    return s32Ret;
}

XBH_S32 ChipMcuNfc::setNfcEnable(XBH_BOOL enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8 u8Enable;
    XBH_CHAR value[PROPERTY_VALUE_MAX] = {0};
    XLOGE("Func[%s] - Line[%d] %d\n\n", __func__, __LINE__,enable);
    u8Enable = (XBH_U8)enable;
    sprintf(value, "%d", u8Enable);
    property_set("persist.vendor.xbh.mcu.nfc.enable", value);
    //切换NFC开关时，清除掉MCU里之前保存的NFC卡ID数据
    //XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    //pI2cBuff.cmd = CMD_I2C_GET_NFC_ID;
    //pI2cBuff.len = 10;
    //s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    return s32Ret;
}

XBH_S32 ChipMcuNfc::getNfcEnable(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *enable = (XBH_BOOL)property_get_int32("persist.vendor.xbh.mcu.nfc.enable", 1);
    XLOGE("Func[%s] - Line[%d] %d\n\n", __func__, __LINE__,enable);
    return s32Ret;
}

XBH_S32 ChipMcuNfc::getNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_GET_NFC_ID;
    pI2cBuff.len = 16;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    int num = pI2cBuff.data[1]/16*10+pI2cBuff.data[1]%16;
    *length = num;
    if(num!=0){
        for(int i=0;i<num;i++){
            data[i]=pI2cBuff.data[2+i];
            XLOGE("getNfcId[%d]:0x%x \n",i,data[i]);
        }
        
    }
    return s32Ret;
}

XBH_S32 ChipMcuNfc::addNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_ADD_NFC_ID;
    pI2cBuff.len = length+2;
    pI2cBuff.data[0] = length;
    memcpy(&pI2cBuff.data[1],data,sizeof(XBH_U8)*length);
    for(int i=0;i<length;i++){
            // data[i]=pI2cBuff.data[2+i];
            XLOGE("addNfcId[%d]:0x%x \n",i,pI2cBuff.data[i]);
        }
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    return s32Ret;
}

XBH_S32 ChipMcuNfc::delNfcId(XBH_U8 *data, XBH_U32 length)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGE("Func[%s] - Line[%d] \n\n", __func__, __LINE__);
    
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_DEL_NFC_ID;
    pI2cBuff.len = length+2;
    pI2cBuff.data[0] = length;
    memcpy(&pI2cBuff.data[1],data,sizeof(XBH_U8)*length);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);

    return s32Ret;
}

XBH_S32 ChipMcuNfc::delAllNfcId()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGE("Func[%s] - Line[%d] \n\n", __func__, __LINE__);

    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_DEL_ALL_NFC;
    pI2cBuff.len = 8;
    memset(pI2cBuff.data,0,sizeof(XBH_U8)*8);
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);

    return s32Ret;
}

XBH_S32 ChipMcuNfc::getWakeUpNfcId(XBH_U8 *data, XBH_U32 *length)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_GET_NFC_WAKEUP_ID;
    pI2cBuff.len = 16;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    int num = pI2cBuff.data[1]/16*10+pI2cBuff.data[1]%16;
    *length = num;
    if(num!=0){
        for(int i=0;i<num;i++){
            data[i]=pI2cBuff.data[2+i];
            XLOGE("WakeUp getNfcId[%d]:0x%x \n",i,data[i]);
        }
    }
    return s32Ret;
}

XBH_S32 ChipMcuNfc::switchNfcType()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XLOGE("Func[%s] - Line[%d] \n\n", __func__, __LINE__);

    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_SWITCH_NFC_TYPE;
    pI2cBuff.len = 1;
    pI2cBuff.data[0] = 1;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    
    return s32Ret;
}

XBH_S32 ChipMcuNfc::setNfcLowPower(const XBH_U8* u8Data)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_SET_NFC_LOW_POWER;
    pI2cBuff.len = 1;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, u8Data);
    return s32Ret;
}

XBH_S32 ChipMcuNfc::getNfcLowPower(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_GET_NFC_LOW_POWER;
    pI2cBuff.len = 1;
    s32Ret = XbhService::getModuleInterface()->getI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    *enable = (XBH_BOOL)pI2cBuff.data[0];
    return s32Ret;
}

XBH_S32 ChipMcuNfc::init()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGE("Func[%s] - Line[%d] \n\n", __func__, __LINE__);
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_SWITCH_NFC_TYPE;
    pI2cBuff.len = 1;
    pI2cBuff.data[0] = 1;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    usleep(1000 * 1000);
    /*由于9679中CMD_I2C_SOC_POWER_MODE寄存器用于NFC的功能。但是在XMA311D2X、XMA311D2B中则是传输SOC开机状态故做此操作，故需要做此限制*/
#if !defined(XBH_BOARD_TYPE_XMA311D2X) && !defined(XBH_BOARD_TYPE_XMA311D2B)
    pI2cBuff.cmd = CMD_I2C_SOC_POWER_MODE;
    pI2cBuff.len = 8;
    pI2cBuff.data[0] = 1;
    s32Ret = XbhService::getModuleInterface()->setI2cData(mI2cBus, mI2cAddr, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
#endif
    return s32Ret;

}

ChipMcuNfc::ChipMcuNfc(XBH_S32 iicBus, XBH_S32 iicAddr, XBH_S32 pGpio, XBH_S32 pLevel, XBH_S32 rGpio, XBH_S32 rLevel)
{
    XLOGE(" begin ");
    mI2cBus = iicBus;
    mI2cAddr = iicAddr;

    mPGpio = pGpio;
    mPLevel = pLevel;
    mRGpio = rGpio;
    mRLevel = rLevel;

    init();
    XLOGE(" end ");
}

ChipMcuNfc::~ChipMcuNfc()
{

}


