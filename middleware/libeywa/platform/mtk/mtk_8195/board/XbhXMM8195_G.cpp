#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhXMM8195_G"

#include <string.h>

#include "XbhLog.h"
#include "XbhXMM8195_G.h"
#include "XbhService.h"
#include "XbhPartitionData.h"
#include "XbhGSensorManager.h"
#include "XbhLightSensorManager.h"
#include "XbhAudioCodecManager.h"
#include "XbhAudioAmplifierManager.h"
#include "XbhDp2HdmiManager.h"
#include "XbhAVOutManager.h"
#include "XbhHdmiSwitchTask.h"
#include "XbhPdIcManager.h"
#include "XbhMcuManager.h"
#include "XbhSysOpt.h"
#include "keyblock.h"

#define PWM_DEVICE_NODE 3
#define XBH_HAL_CHECK_SIGNAL    "vendor.xbh.hal_check_signal"

//init
XbhXMM8195_G            *XbhXMM8195_G::mInstance = XBH_NULL;
XbhMutex                XbhXMM8195_G::mLock;
const XBH_U32           XbhXMM8195_G::MAX_PWM_NAME_SIZE(100);
XBH_S32                 XbhXMM8195_G::mBreathMode(XBH_LED_HOME_ON);
const string            XbhXMM8195_G::PWM_PREFIX("/sys/class/pwm/pwmchip1/");
const string            XbhXMM8195_G::PWMEXPORT("export");
const string            XbhXMM8195_G::PWM_PERIOD("5000000");//pwm周期为200HZ
const string            XbhXMM8195_G::PWM_PERIOD_PATH("period");
const string            XbhXMM8195_G::PWM_ENABLE_PATH("enable");
const string            XbhXMM8195_G::PWM_DYTY_CYCLE_PATH("duty_cycle");
XBH_U32                 XbhXMM8195_G::BASE_DUTY_CYCLE(25000);
const XBH_U32           XbhXMM8195_G::BREATH_DUTY(100);
const XBH_U32           XbhXMM8195_G::VERY_LOW_BRIGHTNESS_SPLIT(50);
const XBH_U32           XbhXMM8195_G::LOW_BRIGHTNESS_SPLIT(60);
const XBH_U32           XbhXMM8195_G::MIDDLE_LOW_BRIGHTNESS_SPLIT(70);
const XBH_U32           XbhXMM8195_G::MIDDLE_BRIGHTNESS_SPLIT(80);
const XBH_U32           XbhXMM8195_G::HIGH_BRIGHTNESS_SPLIT(95);
const XBH_U32           XbhXMM8195_G::VERY_LOW_BRIGHTNESS(500);
const XBH_U32           XbhXMM8195_G::LOW_BRIGHTNEWSS(1000);
const XBH_U32           XbhXMM8195_G::MIDDLE_LOW_BRIGHTNESS(20000);
const XBH_U32           XbhXMM8195_G::MIDDLE_BRIGHTNESS(50000);
const XBH_U32           XbhXMM8195_G::MIDDLE_HIGH_BRIGHTNESS(150000);
const XBH_U32           XbhXMM8195_G::HIGH_BRIGHTNESS(50000);
const XBH_U32           XbhXMM8195_G::MAX_BRIGHTNESS(5000000);
const std::string       XbhXMM8195_G::LENDATAPART("/dev/block/by-name/lendata");
const XBH_U32           XbhXMM8195_G::MAXLENDATAONCEREADLEN(1024);
const XBH_U32           XbhXMM8195_G::LENDATALENTPART(4);
char pwm_arr[100][100];

//init end
#define HDMI_RX_EDID                    "/sys/class/hdmirxswitch/hdmi/switch_edid"
//------------------ static function begin -----------------------------
//override
XBH_S32 XbhXMM8195_G::getHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_HDMI_DET, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
        *enable =  u32GpioValue == XBH_BOARD_GPIO_HDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getFHdmi1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_FHDMI_DET, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
      *enable =  u32GpioValue == XBH_BOARD_GPIO_FHDMI_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getFUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_F_Type_C_DET, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
       *enable =  u32GpioValue == XBH_BOARD_GPIO_F_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getUsbc1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_USBC_DET, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
         *enable =  u32GpioValue == XBH_BOARD_GPIO_USBC_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getDP1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    //Do nothing but remove useless log
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::setFlashEepData(string pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = 0;
    XBH_U32 u32Offset = 0;
    XBH_CHAR lenPart[LENDATALENTPART];
    XBH_U32 lenDataPartLength = pBuff.size();
    XBH_S32 i = 0;

    memset(lenPart, 0x00, LENDATALENTPART);
    for (i = 0; i < LENDATALENTPART; i++)
    {
        lenPart[i] = ((lenDataPartLength >> (i * 8)) & (0x000000ff));
    }
    if (LENDATAPART.empty()) {
        XLOGE("lendata filename is empty, please set this path first \n");
        return XBH_FAILURE;
    }
    fd = open(LENDATAPART.c_str(), O_SYNC | O_WRONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open lendata partition fail. fd: %d\n", fd);
        return XBH_FAILURE;
    }

    s32Ret = pwrite(fd, lenPart, LENDATALENTPART, u32Offset);
    if (s32Ret == -1)
    {
        XLOGE("Write lendata partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != LENDATALENTPART)
    {
        XLOGE("Write lendata data size issue: s32Ret = %d, u32Size = %d\n",s32Ret, LENDATALENTPART);
        close(fd);
        return XBH_FAILURE;
    }
    u32Offset += LENDATALENTPART;
    s32Ret = pwrite(fd, static_cast<void *>(const_cast<char *>(pBuff.c_str())), pBuff.size(), u32Offset);
    if (s32Ret == -1)
    {
        XLOGE("Write lendata partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != pBuff.size())
    {
        XLOGE("Write lendata data size issue: s32Ret = %d, u32Size = %d\n",s32Ret, pBuff.size());
        close(fd);
        return XBH_FAILURE;
    }
    close(fd);


    return XBH_SUCCESS;
}
//override
XBH_S32 XbhXMM8195_G::getFlashEepData(string &pBuff)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 fd = 0;
    XBH_U32 currentReadLen = 0;
    XBH_U32 totalReadLen = 0;
    XBH_CHAR ReadBuffer[MAXLENDATAONCEREADLEN + 1];
    XBH_U32 onceReadLen = 0;
    XBH_S32 i = 0;
    if (LENDATAPART.empty()) {
        XLOGE("lendata filename is empty, please set this path first \n");
        return XBH_FAILURE;
    }
    fd = open(LENDATAPART.c_str(), O_SYNC | O_RDONLY, S_IRUSR | S_IRGRP  | S_IROTH);
    if (fd < 0)
    {
        XLOGE("Open lendata partition fail. fd: %d\n", fd);
        return XBH_FAILURE;
    }
    memset(ReadBuffer, 0x00, MAXLENDATAONCEREADLEN + 1);
    s32Ret = pread(fd, ReadBuffer, LENDATALENTPART, currentReadLen);
    if (s32Ret == -1)
    {
        XLOGE("Read lendata partition fail\n");
        close(fd);
        return XBH_FAILURE;
    }
    else if (s32Ret != LENDATALENTPART)
    {
        XLOGE("Read lendata data size issue: s32Ret = %d, readlen = %d\n",s32Ret, LENDATALENTPART);
        close(fd);
        return XBH_FAILURE;
    }
    for (i = 0; i < LENDATALENTPART; i++){
        XBH_S32 tmp_len = 0;
        tmp_len = ReadBuffer[i];
        totalReadLen += ((tmp_len << (i * 8)) & (0x000000ff));
    }
    currentReadLen += LENDATALENTPART;
    while (totalReadLen > 0){
        memset(ReadBuffer, 0x00, MAXLENDATAONCEREADLEN + 1);
        onceReadLen = totalReadLen > MAXLENDATAONCEREADLEN ? MAXLENDATAONCEREADLEN : totalReadLen;
        s32Ret = pread(fd, ReadBuffer, onceReadLen, currentReadLen);
        if (s32Ret == -1)
        {
            XLOGE("Read lendata partition fail\n");
            close(fd);
            return XBH_FAILURE;
        }
        else if (s32Ret != onceReadLen)
        {
            XLOGE("Read lendata data size issue: s32Ret = %d, pBuff.size() = %d\n",s32Ret, pBuff.size());
            close(fd);
            return XBH_FAILURE;
        }
        pBuff += string(ReadBuffer);
        if (totalReadLen > MAXLENDATAONCEREADLEN)
        {
            totalReadLen -= MAXLENDATAONCEREADLEN;
            currentReadLen += MAXLENDATAONCEREADLEN;
        }else{
            break;
        }
    }
    close(fd);
    return XBH_SUCCESS;
}

/**
 * 设置某个频段的喇叭声音均衡
 * param[in] enEqMode. 频段
 * param[out] s32Value. 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::setAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setAudioEq(enEqMode, s32Value);
    return  s32Ret;    
}

/**
 * 获取某个频段的喇叭声音均衡
 * param[in] enEqMode. 频段
 * param[out] s32Value. 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::getAudioEq(XBH_EQ_MODE_E enEqMode, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getAudioEq(enEqMode, s32Value);
    return  s32Ret;    
}

/**
 * 设置低音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::setBass(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL isLineOutConnected = XBH_FALSE;
    getHpDetectStatus(&isLineOutConnected);
    if (mAudioOutput == XBH_AUDIO_OUTPUT_ARC || isLineOutConnected)
    {
       XLOGD("setBass only for speaker, return fail when ARC/LineOut connected");
       return s32Ret;
    }
    s32Ret = XbhAudioAmplifierManager::getInstance()->setBass(s32Value);
    return  s32Ret;
}

/**
 * 获取低音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::getBass(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getBass(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getBass fail!");
    }
    return  s32Ret;
}

/**
 * 设置高音增益
 * param[in] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::setTreble(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL isLineOutConnected = XBH_FALSE;
    getHpDetectStatus(&isLineOutConnected);
    if (mAudioOutput == XBH_AUDIO_OUTPUT_ARC || isLineOutConnected)
    {
       XLOGD("setTreble only for speaker, return fail when ARC/LineOut connected");
       return s32Ret;
    }
    s32Ret = XbhAudioAmplifierManager::getInstance()->setTreble(s32Value);
    return  s32Ret;
}

/**
 * 获取高音增益
 * param[out] s32Value 增益值
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::getTreble(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getTreble(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getTreble fail!");
    }
    return  s32Ret;
}

/**
 * 设置左右声道平衡
 * param[in] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::setBalance(XBH_S32 s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_BOOL isLineOutConnected = XBH_FALSE;
    getHpDetectStatus(&isLineOutConnected);
    if (mAudioOutput == XBH_AUDIO_OUTPUT_ARC || isLineOutConnected)
    {
       XLOGD("setBalance only for speaker, return fail when ARC/LineOut connected");
       return s32Ret;
    }
    s32Ret = XbhAudioAmplifierManager::getInstance()->setBalance(s32Value);
    return  s32Ret;
}

/**
 * 获取左右声道平衡
 * param[out] s32Value 平衡值 【-50~+50】
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::getBalance(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getBalance(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getInstance fail!");
    }
    return  s32Ret;
}

/**
 * 设置音效模式
 * param[in] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::setSoundMode(XBH_SOUND_MODE_E enSoundMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setSoundMode(enSoundMode);
    return  s32Ret;
}

/**
 * 获取音效模式
 * param[out] s32Value
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::getSoundMode(XBH_SOUND_MODE_E *enSoundMode)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getSoundMode(enSoundMode);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getInstance fail!");
    }
    return  s32Ret;
}

/**
 * 设置环绕声开关
 * param[in] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::setSurroundEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->setSurroundEnable(bEnable);
    return  s32Ret;
}

/**
 * 获取环绕声开关
 * param[out] bEnable XBH_TRUE：开环绕声，XBH_FALSE：关环绕声
 * retval 0:success,-1:failure
*/
XBH_S32 XbhXMM8195_G::getSurroundEnable(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioAmplifierManager::getInstance()->getSurroundEnable(bEnable);
    if(XBH_FAILURE == s32Ret)
    {
        XLOGD("getInstance fail!");
    }
    return  s32Ret;
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_G::getHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_G::getFHdmi1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_G::getUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_G::getFUsbc1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

/**
 * 获取当前信道信号状态
 * status 1:无信号，0：有信号
 */
XBH_S32 XbhXMM8195_G::getOps1SignalStatus(XBH_S32 *status)
{
    return this->XbhMtk_8195::getSignalStatus(status);
}

//override
XBH_S32 XbhXMM8195_G::setHdcpKey(const XBH_CHAR* pBuff, XBH_HDCP_TYPE_E type)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    s32Ret = this->XbhMtk_8195::setHdcpKey(pBuff, type);

    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getHdcpStatus(XBH_HDCP_TYPE_E type, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;

    this->XbhMtk_8195::getHdcpStatus(type, bEnable);

    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getOps1Det(XBH_BOOL *enable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
      *enable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::setOpsPowerCtrl(XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32OpspowerTimer = 500;
    while (XbhService::getModuleInterface()->mIsMiddlewareCompleted != 1)
    {
        XLOGD("setOpsPowerCtrl~ wait middleware inited \n");
        usleep(20*1000); /* sleep 20ms */
    }
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
        usleep(u32OpspowerTimer * 1000);
        s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PS_ON, !XBH_BOARD_GPIO_OPS_PS_ON_LEVEL);
    }
    XLOGD("setOpsPowerCtrl~\n");
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getOpsPowerStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PWR_STATUS, &u32GpioValue);
        if(s32Ret == XBH_SUCCESS)
        {
             *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_ON_LEVEL ? XBH_TRUE : XBH_FALSE;
        }
    }
    return  s32Ret;
}


XBH_S32 XbhXMM8195_G::getOpsDetStatus(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioInputValue(XBH_BOARD_GPIO_OPS_PB_DET, &u32GpioValue);
        if(s32Ret == XBH_SUCCESS)
        {
             *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PB_DET_LEVEL ? XBH_TRUE : XBH_FALSE;
        }
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::setOpsPowerEnable(XBH_BOOL bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (enSource == XBH_SOURCE_OPS1)
    {
        if (bEnable)
        {
            s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWR_EN, XBH_BOARD_GPIO_OPS_PWR_EN_LEVEL);
        }
        else
        {
            s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_PWR_EN, !XBH_BOARD_GPIO_OPS_PWR_EN_LEVEL);
        }
    }
    XLOGD("setOpsPowerEnable %d\n", bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getOpsPowerEnable(XBH_BOOL *bEnable, XBH_SOURCE_E enSource)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    if (enSource == XBH_SOURCE_OPS1)
    {
        s32Ret = this->XbhMtk_8195::getGpioOutputValue(XBH_BOARD_GPIO_OPS_PWR_EN, &u32GpioValue);
        *bEnable =  u32GpioValue == XBH_BOARD_GPIO_OPS_PWR_EN_LEVEL ? XBH_TRUE : XBH_FALSE;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getGSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhGSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getGSensorValue(XBH_FLOAT* fXValue, XBH_FLOAT* fYValue, XBH_FLOAT* fZValue)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U32 X,Y,Z;
    *fXValue = XbhGSensorManager::getInstance()->Get_GSensor_XData();
    *fYValue = XbhGSensorManager::getInstance()->Get_GSensor_YData();
    *fZValue = XbhGSensorManager::getInstance()->Get_GSensor_ZData();
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getPirSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getPirSensorValue(XBH_S32 *s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioPirInt1 = 0;
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_PIR_ADC, &u32GpioPirInt1);
    //TODO 8195 get value in ADC not gpio
    if(u32GpioPirInt1 == 0)
    {
        *s32Value = 0;
    }
    else
    {
        *s32Value = 1;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getHallSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *bEnable = 1;
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getHallSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioHallInt1 = 0;
    //TODO 8195 get value in ADC not gpio
    s32Ret = getGpioInputValue(XBH_BOARD_GPIO_HALL_ADC, &u32GpioHallInt1);
    if(u32GpioHallInt1 == 0)
    {
        *s32Value = 1;
    }
    else
    {
        *s32Value = 0;
    }
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getLightSensorExist(XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getChipExist(bEnable);
    return  s32Ret;
}

//override
XBH_S32 XbhXMM8195_G::getLightSensorValue(XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhLightSensorManager::getInstance()->getLightSensorValue(s32Value);
    if(XBH_FAILURE == s32Ret) 
    {
        XLOGD("getLightSensorValue fail!");
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::setLedPwrStatus(XBH_LED_LIGHT_E enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(enState)
    {
        case XBH_LED_LIGHT_GREEN:       //8195G:POWER键白亮色
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_G, XBH_BOARD_GPIO_F_LED_G_ON);
            break;
        case XBH_LED_LIGHT_RED:         //8195G:POWER键琥珀色
            s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, XBH_BOARD_GPIO_F_LED_R_ON);
            break;
        case XBH_LED_LIGHT_YELLOW:      //8195G:HOME键白亮色
            setLedMode(XBH_LED_HOME_ON);
            s32Ret = XBH_SUCCESS;
            break;
        case XBH_LED_LIGHT_WHITE:   //8195G:HOME键呼吸灯
            setLedMode(XBH_LED_HOME_BREATH);
            s32Ret = XBH_SUCCESS;
            break;
        case XBH_LED_LIGHT_OFF:         //8195G:HOME键灯光关闭
            setLedMode(XBH_LED_HOME_OFF);
            s32Ret = XBH_SUCCESS;
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getLedPwrStatus(XBH_LED_LIGHT_E* enState)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_F_LED_R, &u32GpioValue);
    if(s32Ret == XBH_SUCCESS)
    {
        if(u32GpioValue == XBH_BOARD_GPIO_F_LED_R_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_RED;
        }
        else if (u32GpioValue == XBH_BOARD_GPIO_F_LED_G_ON)
        {
            *enState = XBH_LED_LIGHT_E::XBH_LED_LIGHT_GREEN;
        }
    }
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::setMute(XBH_AUDIO_CHANNEL_E enAudioChannel, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XLOGD("setMute channel = %d mute = %d ",enAudioChannel, bEnable);
    switch(enAudioChannel)
    {
        case XBH_AUDIO_CHANNEL_SPEAKER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SPEAKER, bEnable);
            s32Ret |= XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_HEADPHONE:
            s32Ret = XbhAudioCodecManager::getInstance()->setHeadphoneMute(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_MIC:
            s32Ret = XbhAudioCodecManager::getInstance()->setMuteMicIn(bEnable);
            break;
        case XBH_AUDIO_CHANNEL_SUBWOOFER:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_SUBWOOFER, bEnable);
            break;
        case XBH_AUDIO_CHANNEL_STEREO:
            s32Ret = XbhAudioAmplifierManager::getInstance()->setMute(XBH_AUDIO_CHANNEL_STEREO, bEnable);
            break;
        default:
            break;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getMicDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getMicPlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getHpDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret = XbhAudioCodecManager::getInstance()->getHeadphonePlugIn(status);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getArcDetectStatus(XBH_BOOL* status)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getAmpInitStatus(XBH_BOOL* value)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    *value = XBH_TRUE;
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::getExtendIcVer(XBH_S32 devType, XBH_CHAR* ver)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2705_1, ver);
            break;
        case XBH_UPDATE_FRONT_GSV2702:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_GSV2702, ver);
            break;
        case XBH_UPDATE_BOARD_RTS5450M:
            XLOGD("getExtendIcVer BOARD RTS5450M getExtendIcVer\n");
            s32Ret = XbhPdIcManager::getInstance()->getFirmwareVersion(XBH_UPDATE_BOARD_RTS5450M, ver);
            break;
        case XBH_UPDATE_FRONT_RTS5450M:
            XLOGD("getExtendIcVer FRONT RTS5450M getExtendIcVer\n");
            s32Ret = XbhPdIcManager::getInstance()->getFirmwareVersion(XBH_UPDATE_FRONT_RTS5450M, ver);
            break;
        case XBH_UPDATE_BOARD_CSV5363:
            XLOGD("getExtendIcVer BOARD CSV5363 getExtendIcVer\n");
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_USBC1, ver);
            break;
        case XBH_UPDATE_FRONT_CSV5363:
            XLOGD("getExtendIcVer FRONT CSV5363 getExtendIcVer\n");
            s32Ret = XbhDp2HdmiManager::getInstance()->getFirmwareVersion(XBH_SOURCE_F_USBC1, ver);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getFirmwareVersion(ver);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::upgradeExtendIc(const XBH_CHAR* filename, XBH_BOOL force, XBH_U32 devType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2705_1, filename, force);
            break;
        case XBH_UPDATE_FRONT_GSV2702:
            s32Ret = XbhHdmiSwitchManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_GSV2702, filename, force);
            break;
        case XBH_UPDATE_BOARD_RTS5450M:
            XLOGD("getExtendIcVer BOARD RTS5450M upgradeFirmware\n");
            s32Ret = XbhPdIcManager::getInstance()->upgradeFirmware(XBH_UPDATE_BOARD_RTS5450M, filename, force);
            break;
        case XBH_UPDATE_FRONT_RTS5450M:
            XLOGD("getExtendIcVer FRONT RTS5450M upgradeFirmware\n");
            s32Ret = XbhPdIcManager::getInstance()->upgradeFirmware(XBH_UPDATE_FRONT_RTS5450M, filename, force);
            break;
        case XBH_UPDATE_BOARD_CSV5363:
            XLOGD("getExtendIcVer BOARD CSV5363 upgradeFirmware\n");
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_USBC1, filename, force);
            break;
        case XBH_UPDATE_FRONT_CSV5363:
            XLOGD("getExtendIcVer FRONT CSV5363 upgradeFirmware\n");
            s32Ret = XbhDp2HdmiManager::getInstance()->upgradeFirmware(XBH_SOURCE_F_USBC1, filename, force);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->upgradeFirmware(filename, force);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    XLOGD("upgradeExtendIc file:%s\n", filename);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getUpgradeExtIcState(XBH_S32 devType, XBH_S32* s32Value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    switch(devType)
    {
        case XBH_UPDATE_BOARD_GSV2705_1:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2705_1, s32Value);
            break;
        case XBH_UPDATE_FRONT_GSV2702:
            s32Ret = XbhHdmiSwitchManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_GSV2702, s32Value);
            break;
        case XBH_UPDATE_BOARD_RTS5450M:
            XLOGD("getExtendIcVer BOARD RTS5450M getUpgradeState\n");
            s32Ret = XbhPdIcManager::getInstance()->getUpgradeState(XBH_UPDATE_BOARD_RTS5450M, s32Value);
            break;
        case XBH_UPDATE_FRONT_RTS5450M:
            XLOGD("getExtendIcVer FRONT RTS5450M getUpgradeState\n");
            s32Ret = XbhPdIcManager::getInstance()->getUpgradeState(XBH_UPDATE_FRONT_RTS5450M, s32Value);
            break;
        case XBH_UPDATE_BOARD_CSV5363:
            XLOGD("getExtendIcVer BOARD CSV5363 getUpgradeState\n");
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_USBC1, s32Value);
            break;
        case XBH_UPDATE_FRONT_CSV5363:
            XLOGD("getExtendIcVer FRONT CSV5363 getUpgradeState\n");
            s32Ret = XbhDp2HdmiManager::getInstance()->getUpgradeState(XBH_SOURCE_F_USBC1, s32Value);
            break;
        case XBH_UPDATE_MCU:
            s32Ret = XbhMcuManager::getInstance()->getUpgradeState(s32Value);
            break;
        default:
            s32Ret = XBH_FAILURE;
    }
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::initHdmiCecPhysicalAddr()
{
    XBH_S32 s32Ret = XBH_FAILURE;
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXA, XBH_HDMI1_CEC_PHYSICAL_ADDR);//HDMI  IN
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXB, XBH_HDMI4_CEC_PHYSICAL_ADDR);//TYPEC 
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXC, XBH_HDMI3_CEC_PHYSICAL_ADDR);//ops
    s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_BOARD_GSV2705_1, XBH_HDMI_RXD, XBH_HDMI2_CEC_PHYSICAL_ADDR);//to next switch

    if(property_get_int32("persist.vendor.xbh.fboard.type", FRONT2_U_BOARD) == FRONT2_T2_BOARD)
    {
       s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_FRONT_GSV2702, XBH_HDMI_RXA, XBH_HDMI5_CEC_PHYSICAL_ADDR); //typec 
       s32Ret |= XbhHdmiSwitchManager::getInstance()->setPhysicalAddr(XBH_UPDATE_FRONT_GSV2702, XBH_HDMI_RXB, XBH_HDMI2_CEC_PHYSICAL_ADDR);// HDMI
    }
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::setPoweronMode(XBH_POWERMODE_E enMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[1] = {0};

    pu8Data[0] = enMode;

    s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_POWERON_MODE, 1, 1, pu8Data);

    return s32Ret;
}

XBH_S32 XbhXMM8195_G::getPoweronMode(XBH_POWERMODE_E *enMode)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_U8  pu8Data[1] = {0};

    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_POWERON_MODE, 1, 1, pu8Data);
    *enMode = (XBH_POWERMODE_E)pu8Data[0];

    return s32Ret;
}

XBH_S32 XbhXMM8195_G::setWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[1] = {0};
    XBH_CHAR mac_addr_str[18] = {0};
    XBH_U8 mac_addr[6] = {0};

    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_WOL, 1, 1, pu8Data);
            if (s32Ret < 0)
            {
                XLOGE("set wol error!!!\n");
                break;
            }

            getMacAddress(0, mac_addr_str);
            mac_addr_str[17] = '\0';
            s32Ret = sscanf(mac_addr_str, "%02x:%02x:%02x:%02x:%02x:%02x", &mac_addr[0], &mac_addr[1], &mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);
            if(s32Ret != sizeof(mac_addr))
            {
                XLOGE("get mac addr error!!! scanf result: %d, mac_addr_str: %s, mac_addr: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                    s32Ret, mac_addr_str, mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
                return XBH_FAILURE;
            }
            XLOGD("write mac:%02x:%02x:%02x:%02x:%02x:%02x\n", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_MAC_ADDR, 1, sizeof(mac_addr), mac_addr);
            if (s32Ret < 0)
            {
                XLOGE("set mac addr error!!!\n");
            }
        break;
        case XBH_WAKEUP_SOURCE:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_USBC, 1, 1, pu8Data);
            s32Ret |= setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_USBC, 1, 1, pu8Data);
            s32Ret |= setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_HDMI, 1, 1, pu8Data);
            s32Ret |= setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_HDMI, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_BTYPEC:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_USBC, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_FTYPEC:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_USBC, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_BHDMI:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_HDMI, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_FHDMI:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_F_HDMI, 1, 1, pu8Data);
        break;
        case XBH_WAKEUP_PIR:
            pu8Data[0] = bEnable;
            s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_SET_WAKEUP_PIR, 1, 1, pu8Data);
        break;
        default:
            XLOGE("no type!");
        break;
    }
    
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::getWoTStatus(XBH_WAKEUP_E enWakeUpType, XBH_BOOL* bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U8  pu8Data[1] = {0};
    XBH_U8 bTemp = 0;
    switch(enWakeUpType)
    {
        case XBH_WAKEUP_ETH:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_WOL, 1, 1, pu8Data);
            *bEnable = pu8Data[0];
        break;
        case XBH_WAKEUP_SOURCE:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_USBC, 1, 1, pu8Data);
            bTemp |= pu8Data[0];
            s32Ret |= getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_F_USBC, 1, 1, pu8Data);
            bTemp |= pu8Data[0];
            s32Ret |= getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_HDMI, 1, 1, pu8Data);
            bTemp |= pu8Data[0];
            s32Ret |= getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_F_HDMI, 1, 1, pu8Data);
            bTemp |= pu8Data[0];
            *bEnable = bTemp ? 1 : 0;
        break;
        case XBH_WAKEUP_BTYPEC:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_USBC, 1, 1, pu8Data);
            *bEnable = pu8Data[0];
        break;
        case XBH_WAKEUP_FTYPEC:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_F_USBC, 1, 1, pu8Data);
            *bEnable = pu8Data[0];
        break;
        case XBH_WAKEUP_BHDMI:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_HDMI, 1, 1, pu8Data);
            *bEnable = pu8Data[0];
        break;
        case XBH_WAKEUP_FHDMI:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_F_HDMI, 1, 1, pu8Data);
            *bEnable = pu8Data[0];
        break;
        case XBH_WAKEUP_PIR:
            s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_PIR, 1, 1, pu8Data);
            *bEnable = pu8Data[0];
        break;
        default:
            XLOGE("no type!");
        break;
    }

    return s32Ret;
}

XBH_S32 XbhXMM8195_G::setHdmirxEdidType(XBH_HDMIRX_EDID_TYPE_E enType, int bd_idx)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    XBH_S32 type  = 2;

    //XLOGD("set Hdmi rx Info enType = %d",enType);
    switch(enType)
    {
        case XBH_HDMIRX_EDID_TYPE_14:
            type = 1;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"0");
        break;
        case XBH_HDMIRX_EDID_TYPE_20:
            type = 2;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"1");
        break;
        case XBH_HDMIRX_EDID_TYPE_21:
            type = 3;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"2");
        break;
        case XBH_HDMIRX_EDID_TYPE_AUTO:
            type = 4;
            XbhSysOpt::getInstance()->writeSysfs(HDMI_RX_EDID,"3");
        break;
        default:
            XLOGE("no type!\n");
            s32Ret = XBH_FAILURE;
        break;
    }
    s32Ret = XbhMtk_8195::setHdmirxEdidType(type);
    XLOGD("setHdmirxEdidType %d, ret %d\n", type, s32Ret);

    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getHdmirxEdidType(int bd_idx, XBH_HDMIRX_EDID_TYPE_E *enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_S32 type  = 2;
    s32Ret = XbhMtk_8195::getHdmirxEdidType(&type);
    if(type == 1)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_14;
    }
    else if(type == 2)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_20;
    }
    else if(type == 3)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_21;
    }
    else if(type == 4)
    {
        *enType = XBH_HDMIRX_EDID_TYPE_AUTO;
    }

    return s32Ret;
}

#if 1
/**
 * 切换hdmirx edid
 */
XBH_S32 XbhXMM8195_G::switchHdmiRxEdid(XBH_HDMIRX_EDID_TYPE_E enType)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    int bd_idx = 0;

    //当前 8195.U所有通道都使用同一个EDID
    s32Ret = setHdmirxEdidType(enType, bd_idx);

    return s32Ret;
}

/**
 * 初始化project_id配置
 */
XBH_S32 XbhXMM8195_G::initProjectIdConfig()
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    int bd_idx = 0;
    XBH_HDMIRX_EDID_TYPE_E type  = XBH_HDMIRX_EDID_TYPE_BUTT;

    //每次开机都需要更新edid的数据
    getHdmirxEdidType(bd_idx, &type);
    s32Ret = switchHdmiRxEdid((XBH_HDMIRX_EDID_TYPE_E)type);

    return s32Ret;
}
#endif

XBH_S32 XbhXMM8195_G::getEthPortSpeed(XBH_S32 port, XBH_S32* value)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_CHAR buf[32];
    std :: string result;

    // 中间件端口是从0开始计数，这里是从1开始计数，对此做一个转换
    port += 1;
    
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%d", port);
    XbhSysOpt::getInstance()->writeSysfs("/proc/rtk_gsw/port_status", buf);
    XbhSysOpt::getInstance()->readSysfs("/proc/rtk_gsw/port_status", result);
    
    if (!result.empty())
    {
        XLOGD("%s: %d result:%s!\n", __FUNCTION__, __LINE__, result.c_str());
        if (sscanf(result.c_str(), "%d_%s", value, &buf) == 2)
        {
            s32Ret = XBH_SUCCESS;
        }
    }
    
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::setOnStop()
{
    XBH_S32 s32Ret = XBH_FAILURE;

    XLOGD("---- onStop ----");
    XbhSysOpt::getInstance()->writeSysfs("/proc/rtk_gsw/extport_mode","0");//让phy芯片gmac1_clk停止输出时钟信号
    setGpioOutputValue(XBH_BOARD_GPIO_NETIC_OPS_PWR,!XBH_BOARD_GPIO_NETIC_OPS_PWR_ON);//关闭
    setGpioOutputValue(XBH_BOARD_GPIO_FHUB20_RST,XBH_BOARD_GPIO_FHUB20_RST_ON);//关闭
    setGpioOutputValue(XBH_BOARD_GPIO_AMP_EN,!XBH_BOARD_GPIO_AMP_EN_ON);//关闭 AMP_EN
    setGpioOutputValue(XBH_BOARD_GPIO_NETIC_OPS_PWR,!XBH_BOARD_GPIO_NETIC_OPS_PWR_ON);//关闭 OPS网卡供电
    setGpioOutputValue(XBH_BOARD_GPIO_OPS_RST,XBH_BOARD_GPIO_OPS_RST_ENABLE);//拉低OPS_RST引脚，降低待机时功耗

    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::setCecEnable(XBH_BOOL enable) {
    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    XLOGD("setCecEnable: %d", enable);
    XBH_MCU_I2CBUFFDEF_S pI2cBuff;
    pI2cBuff.cmd = CMD_I2C_SET_WAKEUP_CEC;
    pI2cBuff.len = 1;
    pI2cBuff.data[0] = enable ? 1 : 0;
    s32Ret = setIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, pI2cBuff.cmd, 0x01, pI2cBuff.len, pI2cBuff.data);
    #endif
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::getCecEnable(XBH_BOOL* enable) {

    XBH_S32 s32Ret = XBH_FAILURE;
    #if defined(XBH_FUNC_MCU_I2C_NUM) && defined(XBH_FUNC_MCU_I2C_ADDR)
    XBH_U8 cec_enable = 0;
    s32Ret = getIICData(XBH_FUNC_MCU_I2C_NUM, XBH_FUNC_MCU_I2C_ADDR, CMD_I2C_GET_WAKEUP_CEC, 1, 1, &cec_enable);
    XLOGD("getCecEnable: %d", cec_enable);
    *enable = cec_enable; // XBH_TRUE : XBH_FALSE
    #endif
    return s32Ret;
}
//------------------ static function end -----------------------------


//------------------ public function begin -----------------------------
XBH_S32 XbhXMM8195_G::setLedMode(XBH_LED_CTRL_MODS mode)
{
    mBreathMode = mode;
    return XBH_SUCCESS;
}

XBH_BOOL XbhXMM8195_G::isBreathMode()
{
    return mBreathMode;
}
//------------------ public function end -----------------------------


XBH_S32 XbhXMM8195_G::setPanelPower(XBH_BOOL bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    if (bEnable)
    {
        // 控制 tcon
        //s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_PANEL_TCON_PWR, XBH_BOARD_GPIO_PANEL_TCON_PWR_OFF);
        // 等待
        //usleep(500 * 1000);
        // 控制 blk
        s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_PANEL_BLON, XBH_BOARD_GPIO_PANEL_BLON_ON);
    }
    else
    {
        // 控制 blk
        s32Ret |= setGpioOutputValue(XBH_BOARD_GPIO_PANEL_BLON, XBH_BOARD_GPIO_PANEL_BLON_OFF);
        // 等待
        //usleep(50 * 1000);
        // 控制 tcon
        //s32Ret = setGpioOutputValue(XBH_BOARD_GPIO_PANEL_TCON_PWR, XBH_BOARD_GPIO_PANEL_TCON_PWR_ON);
    }
    return s32Ret;
}

XBH_S32 XbhXMM8195_G::getPanelPower(XBH_BOOL* bEnable, XBH_PANEL_NUM_E enPanel)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    XBH_U32 u32GpioValue;
    //s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_PANEL_TCON_PWR, &u32GpioValue);
    //*bEnable =  u32GpioValue == XBH_BOARD_GPIO_PANEL_TCON_PWR_ON ? XBH_TRUE : XBH_FALSE;
     s32Ret = getGpioOutputValue(XBH_BOARD_GPIO_PANEL_BLON, &u32GpioValue);
     *bEnable = u32GpioValue == XBH_BOARD_GPIO_PANEL_BLON_ON ? XBH_TRUE : XBH_FALSE;
    return s32Ret;
}
//------------------ private function begin -----------------------------
/*
初始化pwm
*/
XBH_BOOL XbhXMM8195_G::pwmInit(XBH_S32 number)
{
    //打开PWM设备节点
    char pwm_name[MAX_PWM_NAME_SIZE];
    memset(pwm_name, 0x00, MAX_PWM_NAME_SIZE);
    sprintf(pwm_name, "pwm%d", number);
    mPwmindex = string(pwm_name) + string("/");
    string pwm_path = PWM_PREFIX + PWMEXPORT;
    char pwm_index[MAX_PWM_NAME_SIZE];
    memset(pwm_index, 0x00, MAX_PWM_NAME_SIZE);
    sprintf(pwm_index, "%d\n", number);
    string pwm_inndex_path = PWM_PREFIX + string(pwm_name);
    if (access(const_cast<char *>(pwm_inndex_path.c_str()), F_OK) != 0)
    {
        if (!privatePwmWrite(pwm_path, string(pwm_index)))
        {
            return XBH_FALSE;
        }
    }

    string pwm_period_path = PWM_PREFIX + mPwmindex + PWM_PERIOD_PATH;
    if (!privatePwmWrite(pwm_period_path, PWM_PERIOD))
    {
        return XBH_FALSE;
    }

    string pwm_enable_path = PWM_PREFIX + mPwmindex + PWM_ENABLE_PATH;
    if (!privatePwmWrite(pwm_enable_path, string("1")))
    {
        return XBH_FALSE;
    }

    string pwm_duty_path = PWM_PREFIX + mPwmindex + PWM_DYTY_CYCLE_PATH;
    pwm_duty_fd = open(pwm_duty_path.c_str(), O_WRONLY|O_NONBLOCK);
    if (pwm_duty_fd < 0)
    {
        return XBH_FALSE;
    }
    //生成PWM参数数组
    XBH_U32 base_duty = BASE_DUTY_CYCLE;
    for (XBH_U32 i = 0 ; i < BREATH_DUTY; i++)
    {
        memset(pwm_arr[i], 0x00, sizeof(pwm_arr[i]));
        if (i <= VERY_LOW_BRIGHTNESS_SPLIT)
        {
            base_duty += VERY_LOW_BRIGHTNESS;
        }
        else if (i < LOW_BRIGHTNESS_SPLIT)
        {
            base_duty += LOW_BRIGHTNEWSS;
        }
        else if (i < MIDDLE_LOW_BRIGHTNESS_SPLIT)
        {
            base_duty += MIDDLE_LOW_BRIGHTNESS;
        }
        else if (i < MIDDLE_BRIGHTNESS_SPLIT)
        {
            base_duty += MIDDLE_BRIGHTNESS;
        }
        else if (i < HIGH_BRIGHTNESS_SPLIT)
        {
            base_duty += MIDDLE_HIGH_BRIGHTNESS;
        }
        else    // i >= HIGH_BRIGHTNESS_SPLIT
        {
            // 动态计算增量，让亮度变化更加平滑
            XBH_U32 remaining_steps = BREATH_DUTY - i;
            XBH_U32 needed_increment = MAX_BRIGHTNESS - base_duty;

            if (remaining_steps == 0) {
                base_duty = MAX_BRIGHTNESS;
            } else {
                XBH_U32 step_increment = needed_increment / remaining_steps;
                base_duty += step_increment;
            }

            // 确保不超过最大值
            if (base_duty >= MAX_BRIGHTNESS) {
                base_duty = MAX_BRIGHTNESS;
            }
        }
        sprintf(pwm_arr[i], "%d", base_duty);
    }
    XLOGD("################################# %s %d pwm Init sucess ##############################", __FUNCTION__, __LINE__);

    return XBH_SUCCESS;
}

XBH_BOOL XbhXMM8195_G::privatePwmWrite(string path, string value)
{
    int pwm_fd = open(path.c_str(), O_WRONLY|O_NONBLOCK);
    if (pwm_fd < 0)
    {
        return XBH_FALSE;
    }
    if(write(pwm_fd, const_cast<char *>(value.c_str()), value.size()) < 0)
    {
        close(pwm_fd);
        return XBH_FALSE;
    }
    close(pwm_fd);

    return XBH_TRUE;
}

XBH_S32 XbhXMM8195_G::setTypeCPdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    if(enSource == XBH_SOURCE_F_USBC1)
    {
        ret = XbhPdIcManager::getInstance()->setTypecCapacity(XBH_UPDATE_FRONT_RTS5450M, pdability);
        XLOGD("setTypeCPdAbility  XBH_SOURCE_USBC1  pdability == %d\n",pdability);
    }else
    {
        XLOGE("wait for imp!\n");
    }
    return ret;
}

XBH_S32 XbhXMM8195_G::setSourcePdAbility(XBH_PDCAPACITY_POWER pdability, XBH_SOURCE_E enSource)
{
    XBH_S32 ret = XBH_SUCCESS;
    if(enSource == XBH_SOURCE_F_USBC1)
    {
        ret = XbhPdIcManager::getInstance()->setTypecCapacity(XBH_UPDATE_FRONT_RTS5450M, pdability);
        XLOGD("setSourcePdAbility  XBH_SOURCE_USBC1  pdability == %d\n",pdability);
    }
    else
    {
        XLOGE("The 'enSource' parameter is invalid.\n");
        return XBH_FAILURE;
    }
    return ret;
}

XBH_S32 XbhXMM8195_G::setOpsResetEnable(XBH_BOOL bEnable)
{
    XBH_S32 s32Ret = XBH_FAILURE;
    if (bEnable)
    {
        s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_RST, XBH_BOARD_GPIO_OPS_RST_ENABLE);
    }
    else
    {
        s32Ret = this->XbhMtk_8195::setGpioOutputValue(XBH_BOARD_GPIO_OPS_RST, !XBH_BOARD_GPIO_OPS_RST_ENABLE);
    }
    XLOGD("setOpsResetEnable bEnable=%d\n", bEnable);
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::setSn(const XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret = XbhPartitionData::getInstance()->setSn(strSn);
    if (s32Ret == XBH_SUCCESS)
    {
        property_set("vendor.xbh.rkp.serialno", strSn);
    }
    /*设置标志位，判断标志为true 在线程里执行。直接执行setNvramValue 可能存在等待阻塞，异步在线程里写SN*/
    mIsSetEnabled = true;
    return  s32Ret;
}

XBH_S32 XbhXMM8195_G::getSn(XBH_CHAR* strSn)
{
    XBH_S32 s32Ret = XBH_SUCCESS;
    s32Ret  = XbhPartitionData::getInstance()->getSn(strSn);
    /*getNvramValue 可能存在等待阻塞，不用获取里面sn号*/
    //s32Ret = getNvramValue(BARCODE_OFFSET, BARCODE_LEN, (XBH_VOID*)strSn);
    return s32Ret;
}

//------------------ private function end -----------------------------

XbhXMM8195_G::XbhXMM8195_G()
    : mPwmindex(),
      pwm_duty_fd(-1)
{
    XLOGD(" begin ");
    /*呼吸灯功能初始化*/
    //初始化呼吸灯PWM设备节点
    pwmInit(PWM_DEVICE_NODE);
    //开启HOME键呼吸灯线程
    XbhMWThread::run(XbhMWThread::REPEAT);
    //G版临时设置，当整机未烧录HDCP key时，开机自动烧录
    XBH_BOOL enable = XBH_FALSE;
    getHdcpStatus(XBH_HDCP1_4,&enable);
    XLOGD("getHdcpStatus hdcp key 1.4: %d",enable);
    if(!enable){
        setHdcpKey("/vendor/firmware/HDCPRX14.bin",XBH_HDCP1_4);
        setHdcpKeyName("M0000001.bin",XBH_HDCP1_4);
        XLOGD("burn hdcp key 1.4 sucess!");
    }
    getHdcpStatus(XBH_HDCP2_2,&enable);
    XLOGD("getHdcpStatus hdcp key 2.2: %d",enable);
    if(!enable){
        setHdcpKey("/vendor/firmware/HDCPRX22.bin",XBH_HDCP2_2);
        setHdcpKeyName("M0000001.bin",XBH_HDCP2_2);
        XLOGD("burn hdcp key 2.2 sucess!");
    }

    #ifdef XBH_USE_HAL_API_TO_CHECK_SIGNAL_STATUS
    property_set(XBH_HAL_CHECK_SIGNAL, "true");
    #endif

    XLOGD(" end ");
}

XbhXMM8195_G::~XbhXMM8195_G()
{
}

XbhXMM8195_G *XbhXMM8195_G::getInstance()
{
    if (!mInstance)
    {
        XbhMutex::Autolock _l(mLock);
        if (!mInstance)
        {
            mInstance = new XbhXMM8195_G();
        }
    }
    return mInstance;
}

void XbhXMM8195_G::run(const void *arg)
{
    if (pwm_duty_fd < 0)
    {
        pwmInit(PWM_DEVICE_NODE);
        return;
    }

    if (mIsSetEnabled)
    { //异步去调用写SN.
        XBH_CHAR propVal[PROPERTY_VALUE_MAX] = {0};
        if (property_get("vendor.xbh.rkp.serialno", propVal, "") > 0)
        {
            XbhMtk_8195::updateNvmStr(propVal);
        }
        mIsSetEnabled = false;
    }
    switch (mBreathMode)
    {
        case XBH_LED_HOME_ON:
        {
            char max_brightness_str[8] = {0}; //最大亮度为7位
            snprintf(max_brightness_str, sizeof(max_brightness_str), "%u", MAX_BRIGHTNESS);
            if (write(pwm_duty_fd,max_brightness_str, sizeof(max_brightness_str)) <= 0)    //当呼吸功能关闭时，LED亮度选择最大值
            {
                pwm_duty_fd = -1;
                return;
            }
            usleep(100*1000);
            break;
        }
        case XBH_LED_HOME_BREATH:
        {
            for (int i = 0; i < BREATH_DUTY; i++)
            {
                if (write(pwm_duty_fd, pwm_arr[i], strlen(pwm_arr[i])) <= 0)
                {
                    pwm_duty_fd = -1;
                    return;
                }
                if (i < 50)
                {
                    usleep(20*1000);
                }
                else
                {
                    usleep(10*1000);
                }
            } 
            for (int i = BREATH_DUTY - 1; i > 0; i--)
            {
                if (write(pwm_duty_fd, pwm_arr[i], strlen(pwm_arr[i])) <= 0)
                {
                    pwm_duty_fd = -1;
                    return;
                }
                if (i < 50)
                {
                    usleep(20*1000);
                }
                else
                {
                    usleep(10*1000);
                }
            }
            break;
        }
        case XBH_LED_HOME_OFF:
        {
            char led_off_value[2] = {'0', 0};   //设置PWM频率为0，关灯
            if (write(pwm_duty_fd, led_off_value, 1) <= 0)
            {
                pwm_duty_fd = -1;
                return;
            }
            usleep(100*1000);
            break;
        }
        default:
        {
            usleep(100*1000);
            break;
        }
    }
}
