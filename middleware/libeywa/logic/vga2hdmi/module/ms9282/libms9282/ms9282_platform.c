#include "ms9282_platform.h"

INT32 mI2cBus = -1;
INT32 mI2cDev = -1;

 // MS928X chip reset and initialization.
UINT8 MS9282_MS928X_Init(SYSPARAM_T* pstSysParam){
    return MS928X_Init(pstSysParam);
}

// Determines the running status of the current system.
UINT8 MS9282_MS928X_Poll(void){
    return MS928X_Poll();
}

//
void MS9282_MS928X_SetVideo(void){
    return MS928X_SetVideo();
}

// Note: This function should be called after the MS928X_SetVideo function.
void MS9282_MS928X_SetAudio(void){
    return MS928X_SetAudio();
}

// Operations
void MS9282_MS928X_Video_Mute(BOOL bmuteON){
    return MS928X_Video_Mute(bmuteON);
}

void MS9282_MS928X_Audio_Mute(BOOL bmuteON){
    return MS928X_Audio_Mute(bmuteON);
}

// Returns TRUE if the sink device connected.
BOOL MS9282_MS928X_HPD_Detect(void){
    return MS928X_HPD_Detect();
}

// Returns TRUE if MS928x chip exists.
BOOL MS9282_MS928X_IsValid(void){
    return MS928X_IsValid();
}

// Switch I2S/SPDIF audio.
UINT8 MS9282_MS928X_UpdateAudio(AUDIOPARAM_T* pstAudParam){
    return MS928X_UpdateAudio(pstAudParam);
} 

// Get current input video mode.
// Returns the corresponding VIC code. Returns 0xFF if no signal detected.
UINT8 MS9282_MS928X_GetInputVideoMode(void){
    return MS928X_GetInputVideoMode();
}

// HDMI DDC access enable
void MS9282_MS928X_DDC_Enable(BOOL b_enable){
    return MS928X_DDC_Enable(b_enable);
}

// Horizontal or vertical pixel shift
void MS9282_MS928X_ShiftConfig(UINT8 u8_h_position, UINT8 u8_v_position){
    return MS928X_ShiftConfig(u8_h_position, u8_v_position);
}

// Do auto phase, or set manual phase
UINT8 MS9282_MS928X_PhaseConfig(BOOL b_auto_enable, UINT8 u8_phase_sel){
    return MS928X_PhaseConfig(b_auto_enable, u8_phase_sel);
}

// Adjust brightness
void MS9282_MS928X_SetVideoBrightness(INT8 i8_brightness){
    return MS928X_SetVideoBrightness(i8_brightness);
}

// Adjust contrast
void MS9282_MS928X_SetVideoContrast(UINT8 u8_contrast){
    return MS928X_SetVideoContrast(u8_contrast);
}

// Adjust tx data driver(max = 0xf)
void MS9282_MS928X_SetHdmiDataDriver(UINT8 u8_main_driver, UINT8 u8_post_driver){
    return MS928X_SetHdmiDataDriver(u8_main_driver, u8_post_driver);
}

/*****************************************************************************************接口移植*****************************************************************************************************************/
VOID MS928X_SetI2c(INT32 i2cBus, INT32 devAddr)
{
    LOGD(" == MS928X_SetI2c i2cBus  = %ld devAddr = 0x%x\n", i2cBus, devAddr);
    mI2cBus = i2cBus;
    mI2cDev = devAddr;
}

INT32 open_i2c_dev(INT32 i2cbus, char *filename, size_t size, INT32 quiet)
{
    INT32 file, len;
    len = snprintf(filename, size, "/dev/i2c/%d", i2cbus);
    if (len >= (INT32)size)
    {
        LOGE(" == %s: path truncated\n", filename);
        return -1;
    }

    file = open(filename, O_RDWR);
    if (file < 0 && (errno == ENOENT || errno == ENOTDIR))
    {
        len = snprintf(filename, size, "/dev/i2c-%d", i2cbus);
        if (len >= (INT32)size)
        {
            LOGE(" == %s: path truncated\n", filename);
            return -1;
        }
        file = open(filename, O_RDWR);
    }

    if (file < 0 && !quiet)
    {
        if (errno == ENOENT)
        {
            LOGE("Error: Could not open file /dev/i2c-%d or /dev/i2c/%d : %s\n",i2cbus, i2cbus, strerror(ENOENT));
        } 
        else
        {
            LOGE("Error: Could not open file %s : %s\n", filename, strerror(errno));
            if (errno == EACCES)
            {
                LOGE("Run as root?\n");
            }
        }
    }
    return file;
}

INT32 check_iic_funcs(INT32 file)
{
    unsigned long funcs;
    /* check adapter functionality */
    if (ioctl(file, I2C_FUNCS, &funcs) < 0)
    {
        LOGE("Error: Could not get the adapter \n");
        return -1;
    }

    if (!(funcs & I2C_FUNC_I2C))
    {
        LOGE("I2C transfers");
        return -1;
    }
    return 0;
}

INT32 set_i2c_data(INT32 u32I2cNum, unsigned char u8DevAddress, INT32 u32RegAddr, INT32 u32RegAddrCount, INT32 u32Length, const unsigned char* u8Data)
{
    INT32 s32Ret = 0;
    INT32 i = 0;
    unsigned char regAddr[4];
    for (i = 0; i < u32RegAddrCount; i++)
    {
        regAddr[i] = (unsigned char)((u32RegAddr >> (8 * (u32RegAddrCount -1 - i))) & 0xff);
        //XLOGD("regAddr[%d]: 0x%x\n", i, regAddr[i]);
    }
    //LOGD("set_i2c_data i2cNum=0x%x,devAddress=0x%x,regAddrValue=0x%x,regAddrCount=0x%x,length=0x%x\n", u32I2cNum, u8DevAddress, u32RegAddr, u32RegAddrCount, u32Length);
    struct i2c_rdwr_ioctl_data data;
    INT32 fd;
    unsigned char Addrbuff[1024];
    char filename[20];
    fd = open_i2c_dev(u32I2cNum, filename, sizeof(filename), 0);
    if (fd < 0 || check_iic_funcs(fd) < 0)
    {
        return -1;
    }

    data.nmsgs = 1;
    data.msgs = (struct i2c_msg *)malloc(data.nmsgs *sizeof(struct i2c_msg));
    if (data.msgs == NULL)
    {
        LOGE("malloc error");
        close(fd);
        return -1;
    }
    ioctl(fd, I2C_TIMEOUT, 2);
    ioctl(fd, I2C_RETRIES, 1);
    memset(Addrbuff, 0, 1024);

    switch (u32RegAddrCount)
    {
        case 1:
            Addrbuff[0] = regAddr[0];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 1, u8Data, u32Length);
                data.msgs[0].len = u32Length + 1;
            }
            else
            {
                data.msgs[0].len = 1;
            }
            break;
        case 2:
            ///Addrbuff[0] = regAddr[0];
            //Addrbuff[1] = regAddr[1];
            Addrbuff[0] = regAddr[1];
            Addrbuff[1] = regAddr[0];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 2, u8Data, u32Length);
                data.msgs[0].len = u32Length + 2;
            }
            else
            {
                data.msgs[0].len = 2;
            }
            break;
        case 3:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 3, u8Data, u32Length);
                data.msgs[0].len = u32Length + 3;
            }
            else
            {
                data.msgs[0].len = 3;
            }
            break;
        case 4:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            Addrbuff[3] = regAddr[3];
            if (u32Length > 0)
            {
                memcpy(Addrbuff + 4, u8Data, u32Length);
                data.msgs[0].len = u32Length + 4;
            }
            else
            {
                data.msgs[0].len = 4;
            }
            break;
        default:
            LOGE("Error RegAddr Count n");
            close(fd);
            free(data.msgs);
            return -1;
    }
    data.msgs[0].addr = u8DevAddress;
    data.msgs[0].flags = 0;     // 0: write 1:read
    data.msgs[0].buf = Addrbuff;
    s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
    if (s32Ret < 0)
    {
        LOGE("I2cNum : %d - 0x%x write reg 0x%x = 0x%x error : %d\r\n",u32I2cNum, u8DevAddress, u32RegAddr, *u8Data, s32Ret);
        close(fd);
        free(data.msgs);
        return -1;
    }
    free(data.msgs);
    close(fd);

    return  0;
}

INT32 get_i2c_data(INT32 u32I2cNum, unsigned char u8DevAddress, INT32 u32RegAddr, INT32 u32RegAddrCount, INT32 u32Length, unsigned char* u8Data)
{
    INT32 s32Ret = 0;
    INT32 i = 0;
    unsigned char regAddr[4];
    for (i = 0; i < u32RegAddrCount; i++)
    {
        regAddr[i] = (unsigned char)((u32RegAddr >> (8 * (u32RegAddrCount -1 - i))) & 0xff);
        //XLOGD("regAddr[%d]: 0x%x\n", i, regAddr[i]);
    }
    //LOGD("get_i2c_data u32I2cNum = %d u8DevAddress: 0x%x\n", u32I2cNum, u8DevAddress);
    struct i2c_rdwr_ioctl_data data;
    INT32 fd = -100;
    unsigned char Addrbuff[4];
    char filename[20];

    fd = open_i2c_dev(u32I2cNum, filename, sizeof(filename), 0);

    if (fd < 0 || check_iic_funcs(fd) < 0)
    {
        LOGE("fd error !!!! fd = %d u32I2cNum = 0x%02x ", fd, u32I2cNum);
        return -1;
    }

    data.nmsgs = 2;
    data.msgs = (struct i2c_msg *)malloc(data.nmsgs *sizeof(struct i2c_msg));
    if (data.msgs == NULL)
    {
        LOGE("malloc error");
        close(fd);
        return -1;
    }
    ioctl(fd, I2C_TIMEOUT, 2);
    ioctl(fd, I2C_RETRIES, 1);

    memset(Addrbuff, 0, 4);

    switch (u32RegAddrCount)
    {
        case 1:
            Addrbuff[0] = regAddr[0];
            data.msgs[0].len =  1;
            break;
        case 2:
            //Addrbuff[0] = regAddr[0];
            //Addrbuff[1] = regAddr[1];
            Addrbuff[0] = regAddr[1];
            Addrbuff[1] = regAddr[0];
            data.msgs[0].len = 2;
            break;
        case 3:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            data.msgs[0].len = 3;
            break;
        case 4:
            Addrbuff[0] = regAddr[0];
            Addrbuff[1] = regAddr[1];
            Addrbuff[2] = regAddr[2];
            Addrbuff[3] = regAddr[3];
            data.msgs[0].len =  4;
            break;
        default:
            LOGE("Error RegAddr Count n");
            close(fd);
            free(data.msgs);
            return -1;
    }

    data.msgs[0].addr = u8DevAddress;
    data.msgs[0].flags = !I2C_M_RD;     // 0: write 1:read
    data.msgs[0].buf = Addrbuff;
    //read data
    data.msgs[1].len = u32Length;
    data.msgs[1].addr = u8DevAddress;
    data.msgs[1].flags = I2C_M_RD;    // 0: write 1:read
    data.msgs[1].buf = u8Data;

    s32Ret = ioctl(fd, I2C_RDWR, (unsigned long)&data);
    if (s32Ret < 0)
    {
        LOGE("I2cNum : %d - 0x%x read reg 0x%x error : %d\r\n",u32I2cNum, u8DevAddress, u32RegAddr, s32Ret);
        close(fd);
        free(data.msgs);
        return -1;
    }
    free(data.msgs);
    close(fd);
    return 0;
}


VOID Sys_Reset(VOID)
{
    LOGD("Enter Sys_Reset");
   // setGpioOutputValue(XBH_BOARD_GPIO_VGA_RST,!XBH_BOARD_GPIO_VGA_RST_ON);
    Delay_ms(200);
   // setGpioOutputValue(XBH_BOARD_GPIO_VGA_RST, XBH_BOARD_GPIO_VGA_RST_ON);
    LOGD("Exit Sys_Reset");
}

// Delay a period of time.
VOID Delay_ms(UINT8 u8ms)
{
    usleep(u8ms * 1000);
}

VOID Delay_us(UINT8 u8us)
{
    usleep(u8us);
}

VOID I2C_Sel_Speed(UINT8 u8speed)
{
    _UNUSED_(u8speed);
}

UINT8 I2C16_ReadByte(UINT8 u8addr, UINT16 u16index)
{
    unsigned char u8Data[1] = {0};
    u8addr = (u8addr >> 1 & 0xff);
    get_i2c_data(mI2cBus, u8addr, u16index, 2, 1, u8Data);
    return u8Data[0];
}

VOID I2C16_WriteByte(UINT8 u8addr, UINT16 u16index, UINT8 u8value)
{
    unsigned char u8Data[1] = {u8value};
    u8addr = (u8addr >> 1 & 0xff);
    set_i2c_data(mI2cBus, u8addr, u16index, 2, 1, u8Data);
}

BOOL I2C_ReadBytes(UINT8 u8addr, UINT8 u8index, UINT8 *pIn, UINT8 u8len)
{
    u8addr = (u8addr >> 1 & 0xff);
    if(set_i2c_data(mI2cBus, u8addr, u8index, 1, u8len, pIn) == -1)
    {
        LOGE(" I2C_ReadBytes error !!!!!");
        return 0;
    }
    return 1;
}

UINT8  I2C_ReadByte(UINT8 u8addr, UINT8 u8index)
{
    unsigned char u8Data[1] = {0};
    u8addr = (u8addr >> 1 & 0xff);
    set_i2c_data(mI2cBus, u8addr, u8index, 1, 1, u8Data);
    return u8Data[0];
}

VOID I2C_WriteByte(UINT8 u8addr, UINT8 u8index, UINT8 u8value)
{
    unsigned char u8Data[1] = {u8value};
    u8addr = (u8addr >> 1 & 0xff);
    set_i2c_data(mI2cBus, u8addr, u8index, 1, 1, u8Data);
}

