#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhVgaEdidManager"

#include "XbhLog.h"
#include "XbhVgaEdidManager.h"
#include "XbhEventType.h"
#include "cutils/properties.h"
#include "XbhPlatformInterface.h"
#include <hardware/board.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include "XbhService.h"
#include <stdio.h>
#include <stdlib.h>
#include "XbhPartitionData.h"

XbhVgaEdidManager*               XbhVgaEdidManager::mInstance = XBH_NULL;
XbhMutex                         XbhVgaEdidManager::mLock;
// /vendor/etc/project_ids/projectid_1/edid/BIN/EDID_VGA.bin
XBH_S32 XbhVgaEdidManager::setVgaEdid(const XBH_CHAR* strPath, XBH_SOURCE_E idx)
{
    XLOGD("VGA_EDID in upgrade mode");
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 dataBuf[128] = {0};
    XBH_U8 len = 0;
    FILE *fp = NULL;
    //清除EDID 数据
    if(idx == XBH_SOURCE_HDMI1)
    {
        memset(dataBuf, 0x00, 128);
        return setVgaEdidData(dataBuf, XBH_SOURCE_VGA1);
    }
    idx = XBH_SOURCE_VGA1;

    fp = fopen(strPath, "rb");
    property_set("persist.vendor.xbh.vga_edid.ver", "");
    
    // 读取EDID
    if(fp != NULL)
    {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        XLOGD("Read file size:%d\n", len);
        fseek(fp, 0, SEEK_SET);

        if(len == 128)
        {   
            XLOGD("Read edid file success!!!");
            fread(dataBuf, 1, 128, fp);
        } 
        else 
        {
            XLOGE("EDID file size bytes.error!!!");
            fclose(fp);
            return s32Ret;
        }
        fclose(fp);
    }
    else 
    {
        XLOGE("Open upgrade file %s fail!", strPath);
        return XBH_FAILURE;
    }

    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //关闭EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_OFF);
    #else
    XLOGE("no defined XBH_BOARD_VGA_EEPROM_WP_GPIO");
    return XBH_FAILURE;
    #endif
    usleep(10 * 1000);
    XLOGD("Set edid start !!! ");
    for(int i = 0; i < 128; i ++)
    {
        // 按Byte方式写入EDID
        #ifdef VGA_EEPROM_I2C_BUS
        //使用标准iic
        s32Ret = XbhService::getPlatformInterface()->setIICData(VGA_EEPROM_I2C_BUS, VGA_EEPROM_I2C_ADDR, i, 1, 1, &dataBuf[i]);
        #else
        //使用模拟iic
        s32Ret = XbhService::getModuleInterface()->setVgaEdidI2cData(i, &dataBuf[i], 1);
        #endif
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("Set edid failed, return");
            #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
            //打开EEPROM WP保护
            XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
            #endif
            return s32Ret;
        }
        usleep(10 * 1000);
    }
    usleep(10 * 1000);
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //打开EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
    #endif
    XLOGD("Set edid successful");

    usleep(10 * 1000);
    XBH_S32 checksum[10] = {0};
    getVgaEdidCheckSum(XBH_SOURCE_VGA1, checksum);

    return s32Ret;
}

XBH_S32 XbhVgaEdidManager::setVgaEdidData(XBH_U8 edid_data[128], XBH_SOURCE_E idx)
{
    XLOGD("VGA_EDID in upgrade mode");
    XBH_S32 s32Ret = XBH_FAILURE;
    idx = XBH_SOURCE_VGA1;

    //mode 是不同的EDID处理方式 ，1是一种模式 ，2是一种模式，通过系统属性客制化
    XBH_CHAR cmode[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.vendor.xbh.gsv.edid.dycchange", cmode, "0");
    XBH_U32 mode = atoi(cmode);
    if(mode == 2)
    {
        int id_num = 0,sum = 0;
        XBH_CHAR strSn[64] = {0};
        XbhPartitionData::getInstance()->getSmartSnData(strSn);
        int length = strlen(strSn);
        XLOGD("%s ",strSn);
        if(length == XBH_CUSDATA_SMART_SN_REAL_LEN )
        {
            std::string serial(strSn);

            // 提取各部分字符串
            std::string stage_str = serial.substr(3, 1);
            std::string year_str = serial.substr(4, 1);
            std::string week_str = serial.substr(6, 2);
            std::string product_str = serial.substr(8, 1);
            std::string sn_str = serial.substr(9);

            //转成数字
            XBH_U32 stage = (stage_str[0] - '0') << 30;
            XBH_U32 year = (year_str[0] - 'A') << 25;
            XBH_U32 week = std::stoi(week_str) << 19;
            XBH_U32 product = (product_str[0] - 'A') << 14;
            XBH_U32 sn = std::stoi(sn_str);
            XLOGD("stage = %d| year=%d | week=%d | product=%d | sn=%d ", (stage_str[0] - '0'),(year_str[0] - 'A'),std::stoi(week_str) ,(product_str[0] - 'A'),std::stoi(sn_str));
            // 合成 final_serial
            XBH_U32 final_serial = stage | year | week | product | sn;
            //
            //客户要求 final_serial 小端字节  其它不变
            edid_data[15] = (final_serial >> 24) & 0xff;
            edid_data[14] = (final_serial >> 16) & 0xff;
            edid_data[13] = (final_serial >> 8) & 0xff;
            edid_data[12] = (final_serial) & 0xff;
            edid_data[16] = std::stoi(week_str);//week
            edid_data[17] = (year_str[0] - 'A') + 18 ;//A = 2008 B= 2009
            //更新校验码
            for(id_num = 0;id_num < 127 ; id_num++)
            {
                sum += edid_data[id_num];
            }
            edid_data[127] = (256 - (sum % 256)) % 256;
        }
        else
        {
            XLOGD("smart sn length error = %x",length);
        }
    }

    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //关闭EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_OFF);
    #else
    XLOGE("no defined XBH_BOARD_VGA_EEPROM_WP_GPIO");
    return XBH_FAILURE;
    #endif
    usleep(10 * 1000);
    XLOGD("Set edid start !!! ");
    for(int i = 0; i < 128; i ++)
    {
        // 按Byte方式写入EDID
        #ifdef VGA_EEPROM_I2C_BUS
        //使用标准iic
        s32Ret = XbhService::getPlatformInterface()->setIICData(VGA_EEPROM_I2C_BUS, VGA_EEPROM_I2C_ADDR, i, 1, 1, &edid_data[i]);
        #else
        //使用模拟iic
        s32Ret = XbhService::getModuleInterface()->setVgaEdidI2cData(i, &edid_data[i], 1);
        #endif
        if (s32Ret != XBH_SUCCESS)
        {
            XLOGE("Set edid failed, return");
            #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
            //打开EEPROM WP保护
            XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
            #endif
            return s32Ret;
        }
        usleep(10 * 1000);
    }
    usleep(10 * 1000);
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //打开EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
    #endif
    XLOGD("Set edid successful");

    usleep(10 * 1000);
    XBH_S32 checksum[10] = {0};
    getVgaEdidCheckSum(XBH_SOURCE_VGA1, checksum);

    return s32Ret;
}

XBH_S32 XbhVgaEdidManager::dumpEdid()
{
    XLOGD("Print edid data");
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 dataBuf[128] = {0};
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //关闭EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_OFF);
    #else
    XLOGE("no defined XBH_BOARD_VGA_EEPROM_WP_GPIO");
    return XBH_FAILURE;
    #endif

    #ifdef VGA_EEPROM_I2C_BUS
    //使用标准iic
    s32Ret = XbhService::getPlatformInterface()->getIICData(VGA_EEPROM_I2C_BUS, VGA_EEPROM_I2C_ADDR, 0, 1, 128, dataBuf);
    #else
    //使用模拟iic
    s32Ret = XbhService::getModuleInterface()->getVgaEdidI2cData(0, dataBuf, 128);
    #endif
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Get edid fail, return");
        #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
        //打开EEPROM WP保护
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
        #endif
        return s32Ret;
    }
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //打开EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
    #endif
    XLOGD("Get edid success, data : ");

    for(int i = 0; i < 8; i ++)
    {
        XLOGI("==>> %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x", dataBuf[i * 16 + 0], dataBuf[i * 16 + 1], dataBuf[i * 16 + 2], dataBuf[i * 16 + 3], dataBuf[i * 16 + 4], 
                                                                                                      dataBuf[i * 16 + 5], dataBuf[i * 16 + 6], dataBuf[i * 16 + 7], dataBuf[i * 16 + 8], dataBuf[i * 16 + 9], 
                                                                                                      dataBuf[i * 16 + 10], dataBuf[i * 16 + 11], dataBuf[i * 16 + 12], dataBuf[i * 16 + 13], dataBuf[i * 16 + 14],
                                                                                                      dataBuf[i * 16 + 15]);
    }
    
    return s32Ret;
}

XBH_S32 XbhVgaEdidManager::getVgaEdidStatus(XBH_SOURCE_E idx, XBH_BOOL* bEnable)
{
    XLOGD("Read edid mode");
    idx = XBH_SOURCE_VGA1;    
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 checksum = 0;
    XBH_U8 index = 0;
    XBH_U8 dataBuf[128] = {0};
    
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //关闭EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_OFF);
    #else
    XLOGE("no defined XBH_BOARD_VGA_EEPROM_WP_GPIO");
    return XBH_FAILURE;
    #endif

    // 获取EDID
    #ifdef VGA_EEPROM_I2C_BUS
    //使用标准iic
    s32Ret = XbhService::getPlatformInterface()->getIICData(VGA_EEPROM_I2C_BUS, VGA_EEPROM_I2C_ADDR, 0, 1, 128, dataBuf);
    #else
    //使用模拟iic
    s32Ret = XbhService::getModuleInterface()->getVgaEdidI2cData(0, dataBuf, 128);
    #endif
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Get edid fail, return");
        #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
        //打开EEPROM WP保护
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
        #endif
        return s32Ret;
    }
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //打开EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
    #endif
    XLOGD("Get edid success");
    
    //验证
    for(index = 0; index < sizeof(dataBuf) -1; index++)
    {
        checksum += dataBuf[index];
    }
 
    if(dataBuf[sizeof(dataBuf) - 1] == 256 - (checksum % 256))
    {
        *bEnable = XBH_TRUE;
        XLOGD("Edid checksum 0x%x, success!", checksum);
    }
    else
    {
        *bEnable = XBH_FALSE;
        XLOGE("Edid checksum 0x%x, fail!", checksum);
    }
    return s32Ret;
}

XBH_S32 XbhVgaEdidManager::getVgaEdidCheckSum(XBH_SOURCE_E idx, XBH_S32* checksum)
{
    XLOGD("Read edid checksum");
    idx = XBH_SOURCE_VGA1;    
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8 index = 0;
    XBH_U8 dataBuf[128] = {0};
    XBH_CHAR checksum_str[10] = {0};
    
    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //关闭EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_OFF);
    #else
    XLOGE("no defined XBH_BOARD_VGA_EEPROM_WP_GPIO");
    return XBH_FAILURE;
    #endif

    // 获取EDID
    #ifdef VGA_EEPROM_I2C_BUS
    //使用标准iic
    s32Ret = XbhService::getPlatformInterface()->getIICData(VGA_EEPROM_I2C_BUS, VGA_EEPROM_I2C_ADDR, 0, 1, 128, dataBuf);
    #else
    //使用模拟iic
    s32Ret = XbhService::getModuleInterface()->getVgaEdidI2cData(0, dataBuf, 128);
    #endif
    if (s32Ret != XBH_SUCCESS)
    {
        XLOGE("Get edid fail, return");
        #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
        //打开EEPROM WP保护
        XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
        #endif
        return s32Ret;
    }

    #ifdef XBH_BOARD_VGA_EEPROM_WP_GPIO
    //打开EEPROM WP保护
    XbhService::getModuleInterface()->setGpioOutputValue(XBH_BOARD_VGA_EEPROM_WP_GPIO, XBH_BOARD_VGA_EEPROM_WP_ON);
    #endif

    XLOGD("Get edid success");
    *checksum = dataBuf[127];
    sprintf(checksum_str, "%02x", *checksum);
    XLOGD("Edid checksum_str = %s ", checksum_str);
    property_set("persist.vendor.xbh.vga_edid.ver", checksum_str);
    return s32Ret;
}

void XbhVgaEdidManager::run(const void* arg)
{
    //XLOGD(" begin run ");
    XLOGD("VGA_EDID in upgrade mode!");
    //XBH_SOURCE_E idx;
    //XBH_BOOL* bEnable;
    //XBH_CHAR * strPath;
    //setVgaEdid(strPath, idx);
    //getVgaEdidStatus(idx,bEnable);
}

XbhVgaEdidManager::XbhVgaEdidManager()
{

} 

XbhVgaEdidManager::~XbhVgaEdidManager()
{

}

XbhVgaEdidManager* XbhVgaEdidManager::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhVgaEdidManager();
    }
    return mInstance;
}

