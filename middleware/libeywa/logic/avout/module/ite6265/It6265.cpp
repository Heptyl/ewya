#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@It6265"
#include <stdio.h>
#include "It6265.h"
#include "XbhSourceManager.h"
#include "XbhService.h"
#include "XbhLog.h"
#include <unistd.h>
#include <cutils/properties.h>
#include <hardware/board.h>

XBH_BOOL isHdmioutOn = XBH_TRUE;
XBH_U32  regValue    = 0x0000;
XBH_U32  tmpOutNum   = 0;

//xbh patch start
#define hdmi_out_mute_mode 0
#define hdmi_out_sound_mute_mode 1
#define hdmi_out_hdcp_enable 0
#define hdmi_out_resolution 1
//xbh patch end

//-------------------- config.h start ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <config.h>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
// i2c address
#define H2TxAddr        0x98 >> 1   // 0x4c
#define VBORxAddr       0x2c >> 1   // 0x16
#define CECAddr         0x9c >> 1   // 0x4E
// #define EDIDAddr        0xA0 >> 1 // 

// input color mode
// color mode can be :
//
//   RGB444
//   YCbCr422
//   YCbCr444
//   YCbCr420
#define INPUT_COLOR_MODE  RGB444

// output color mode
// color mode can be :
//
//   RGB444
//   YCbCr422
//   YCbCr444
//   YCbCr420 ( only if the input color mode is also YCbCr420 )
#define OUTPUT_COLOR_MODE  RGB444


// output color depth
// can be :
//   VID8BIT
//   VID10BIT
//   VID12BIT
#define OUTPUT_COLOR_DEPTH  VID10BIT

// Dynamic range: CEA VESA
#define DYNAMIC_RANGE  DynCEA

// Color space conversion :
// ITU601, ITU709, ITU2020
#define CSC_COEFF  ITU709

// color depth

//
//  Audio Options :
//

//  Enable audio Output :  true or false
#define AUDIO_ENABLE true

// Output Audio channel count
#define AUDIO_CHANNELS 2

// Output Audio frequency
#define AUDIO_FREQ  AUD48K

// Audio Type
// LPCM, NLPCM, DSD, HBR
#define AUDIO_TYPE  LPCM

//  Input Audio Source;
//  AUDIO_SELECT : I2S or SPDIF
//#define AUDIO_SELECT I2S
#define AUDIO_SELECT SPDIF

// Audio I2S Options :
    //  0: 16 bits, 1: 18 bits, 2: 20 bits, 3: 24 bits
    #define I2S_WORD_LENGTH  3

    // 0 : standard I2s, 1 : 32-bit I2s
    #define I2S_FMT_0  1

    // 0: Left-justified, 1: Right-justified
    #define I2S_FMT_1  0

    // 0: Data delay 1T correspond to WS
    // 1: No data delay correspond to WS
    #define I2S_FMT_2  0

    // 0: WS=0 is left channel
    // 1: WS=0 is right channel
    #define I2S_FMT_3  0

    // 0: MSB shift first, 1: LSB shift first
    #define I2S_FMT_4  0

    #define I2S_FREQ
    #define I2S_1T_DELAY

// Audio SPIDF ptions :
    //  SPDIF Schmitt trigger 0:disable, 1:enable
    #define SPDIF_SCHMITT_TRIGGER 0

//
//  V-by-One RX options
//

// V-by-One Rx lane count
// 0 : 1 lane
// 1 : 2 lanes
// 2 : 4 lanes
// 3 : 8 lanes
#define VBO_DEFAULT_LANE_COUNT 3

// V-by-One Rx byte mode
// 0 : 3 byte mode
// 1 : 4 byte mode
// 2 : 5 byte mode
#define VBO_DEFAULT_BYTE_MODE 1

// V-by-One Rx SSC option
// 0 : disable DeSSC
// 1 : enable DeSSC
#define VBO_ENABLE_DESSC 0

//  V-by-One RX
//  Data mappings
//  0..3
#define VBO_SWAP_SEL  0

//  V-by-One RX  2-section mode:
//  0 : disable
//  1 : enable
#define VBO_2_SECTION_MODE 0

//
#define VBO_SCLK_DELAY 0

//
#define VBO_SCLK_INVERSE 0
//
#define VBO_PCLK_INVERSE 0

// video latch edge inverse
// audio latch edge inverse

//LM, RG, YC swap:

// Setup INT Pin: Active Low & Open-Drain


// enable 3d
#define ENABLE_3D  0

// FrmPkt, TopBtm, SbSHalf, SbSFull
#define SELECT_3D_FORMAT  FrmPkt

// Blue screen mute
#define ENABLE_BLUE_SCREEN_MUTE  false

#define MAX_HDCP2_FAIL_COUNT 20
#define SWITCH_TO_HDCP14_WHEN_HDCP22_FAIL 0
#define DISABLE_HDCP22_UNDER_3G 0
//
//
//
#define SHOW_VBORX_INPUT_INFO 1
#define SHOW_VBORX_OUTPUT_INFO 1
#define SHOW_HDMI_OUTPUT_INFO 1
#define SUPPORT_HDCP_REPEATER 1
#define ENABLE_EDID_PARSING 1
#define SUPPORT_DEONLY 0
#define ENABLE_CEC 0

#define ENABLE_CALLBACK 0
#define ENABLE_HDCP_REPEATER_FUNCTION_ON_IT6265 0

#define DEBUG_SHOW_REG_RW 0
#define AUTO_LANE_DETECT 0

// if there is any SCDC signal when HPD is low,
// please set CHECK_HPD_BEFORE_RELAY_SCDC to 0
#define CHECK_HPD_BEFORE_RELAY_SCDC 1

#define CHECK_MAX_TMDS_CLOCK 1
#define CHECK_EDID_COLOR_DEPTH 0
//
#define ENABLE_HDCP_REPEATER_DEVICE_COUNT_OVER_6 0
#define HDCP_1P4_MAX_DEVICE_COUNT 40
#define SHA_BUFFER_SIZE ((((HDCP_1P4_MAX_DEVICE_COUNT*5+2+8+1+8)+63)/64)*64)

#define HDCP_ENABLE 0
//-------------------- config.h end   ---------------------------

//-------------------- it6265.h start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265.h>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
#ifdef __cplusplus
extern "C" {
#endif

// callback define:
typedef void (*fn_it6265_hpd_change_cb)(unsigned char hpd_state, void *callback_data);
typedef void (*fn_it6265_rxsen_change_cb)(unsigned char rxsen_state, void *callback_data);
typedef void (*fn_it6265_edid_read_cb)(unsigned char block_index, void *edid_data, void *callback_data);

typedef void (*fn_it6265_hdcp_state_cb)(unsigned char hdcp_version, void *callback_data);
typedef void (*fn_it6265_hdcp_fail_state_cb)(unsigned char hdcp_version, unsigned char status, void *callback_data);

typedef void (*fn_it6265_hdcp_BStatus_cb)(unsigned short bstatus, void *callback_data);
typedef void (*fn_it6265_hdcp_BKSV_cb)(unsigned char *bksv, void *callback_data);
typedef void (*fn_it6265_hdcp_ksv_list_cb)(unsigned char ksv_count, unsigned char *ksv_list, void *callback_data);

typedef void (*fn_it6265_hdcp2_rxinfo_cb)(unsigned short rx_info, void *callback_data);
typedef void (*fn_it6265_hdcp2_rxid_cb)(unsigned char *rxid, void *callback_data);
typedef void (*fn_it6265_hdcp2_rxid_list_cb)(unsigned char rxid_count, unsigned char *rxid_list, void *callback_data);

typedef void (*fn_it6265_hdcp2_repeater_wait_tx_stream_manage_cb)(void *callback_data);
typedef void (*fn_it6265_hdcp2_repeater_wait_tx_eks_cb)(void *callback_data);
typedef void (*fn_it6265_hdcp2_rx_reauth_req_cb)(void *callback_data);
typedef void (*fn_it6265_hdcp2_stream_manage_fail_cb)(void *callback_data);

typedef void (*fn_it6265_hdcp_version_cb)(unsigned char hdcp_version, void *callback_data);

typedef struct
{
    fn_it6265_hpd_change_cb it6265_hpd_change_cb;
    fn_it6265_rxsen_change_cb it6265_rxsen_change_cb;
    fn_it6265_edid_read_cb  it6265_edid_read_cb;

    fn_it6265_hdcp_state_cb it6265_hdcp_start_cb;
    fn_it6265_hdcp_state_cb it6265_hdcp_done_cb;
    fn_it6265_hdcp_fail_state_cb it6265_hdcp_fail_cb;

    fn_it6265_hdcp_BStatus_cb  it6265_hdcp_BStatus_cb;
    fn_it6265_hdcp_BKSV_cb  it6265_hdcp_BKSV_cb;
    fn_it6265_hdcp_ksv_list_cb it6265_hdcp_ksv_list_cb;

    fn_it6265_hdcp2_rxinfo_cb it6265_hdcp2_RxInfo_cb;
    fn_it6265_hdcp2_rxid_cb  it6265_hdcp2_rxid_cb;
    fn_it6265_hdcp2_rxid_list_cb it6265_hdcp2_rxid_list_cb;

    fn_it6265_hdcp2_repeater_wait_tx_stream_manage_cb it6265_hdcp2_repeater_wait_tx_stream_manage_cb;
    fn_it6265_hdcp2_repeater_wait_tx_eks_cb it6265_hdcp2_repeater_wait_tx_eks_cb;
    fn_it6265_hdcp2_rx_reauth_req_cb it6265_hdcp2_rx_reauth_req_cb;
    fn_it6265_hdcp2_stream_manage_fail_cb it6265_hdcp2_stream_manage_fail_cb;

    fn_it6265_hdcp_version_cb it6265_hdcp_version_cb;

}it6265_callbacks;

typedef struct
{
    void* hpd_change_cb_data;
    void* rxsen_change_cb_data;
    void* edid_read_cb_data;

    void* hdcp_start_cb_data;
    void* hdcp_done_cb_data;
    void* hdcp_fail_cb_data;

    void* hdcp_BStatus_cb_data;
    void* hdcp_BKSV_cb_data;
    void* hdcp_ksv_list_cb_data;

    void* hdcp2_rxinfo_cb_data;
    void* hdcp2_rxid_cb_data;
    void* hdcp2_rxid_list_cb_data;

    void* hdcp2_repeater_wait_tx_stream_manage_cb_data;
    void* hdcp2_repeater_wait_tx_eks_cb_data;
    void* hdcp2_rx_reauth_req_cb_data;
    void* hdcp2_stream_manage_fail_cb_data;

    void* hdcp_version_cb_data;

}it6265_callback_data;

#ifdef __cplusplus
}
#endif
//-------------------- it6265.h end   ---------------------------

//--------------------  it6265_arch.h start -------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_arch.h>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
// include C librarys of your platform
// define the non-exist types
#define CONST const
#define true 1
#define false 0
#define PRINT_EDID_INFO 1

//
//  assign the print function
//
/* jason
#define pr_err    printf
#define pr_info   printf
#define pr_info2  printf
*/
#define pr_err    XLOGE
#define pr_info   XLOGD
#define pr_info2  XLOGD

#define pr_edid   XLOGD

//-------------------- it6265_arch.h end  ---------------------------

//--------------------  it6265_cec.h start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_cec.h>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
//---------------------------------------------------------------------------
#define CEC_RX_SELF_DIS                (1)
#define CEC_RST                        (0)
#define CEC_NACK_EN                    (0)
#define CEC_CAL_CNT                    (1)
#define CEC_RESEND_MAX                (5)

#ifdef __cplusplus
extern "C" {
#endif

void it6265_cec_init(u8 phyAddrL, u8 phyAddrH, u32 ulClkCnt);

void it6265_cec_tx_polling(void);
void it6265_cec_maganer(void);
char it6265_cec_send( u8 opcode, u8 *operand, u8 operand_len, u8 follower);

#ifdef __cplusplus
}
#endif
//--------------------  it6265_cec.h end   ---------------------------

//--------------------  it6265_drv.h start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_drv.h>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
#define SUCCESS         1
#define FAIL            -1

#define TRUE            1
#define FALSE           0

#define HIGH            1
#define LOW             0

#define HDMI            0
#define DVI             1
#define RSVD            2

#define NORM            0
#define FAST            1
#define SLOW            2


#define CLK13P5M        0
#define CLK27M          1
#define CLK54M          2
#define CLK108M         3
#define CLK37P125M      4
#define CLK74P25M       5
#define CLK148P5M       6
#define CLK297M         7

#define VID8BIT         0
#define VID10BIT        1
#define VID12BIT        2
#define VID16BIT        3

#define RGB444          0
#define YCbCr422        1
#define YCbCr444        2
#define YCbCr420        3

#define xvYCC601        0
#define xvYCC709        1
#define sYCC601            2
#define AdobeYCC601        3
#define AdobeRGB        4
#define BT2020cYCC        5
#define BT2020YCC        6

#define DynVESA         0
#define DynCEA          1

#define ITU601          0
#define ITU709          1
#define ITU2020         2

#define AUD32K          0x03
#define AUD44K          0x00
#define AUD48K          0x02
#define AUD64K          0x0B
#define AUD88K          0x08
#define AUD96K          0x0A
#define AUD128K         0x2B
#define AUD176K         0x0C
#define AUD192K         0x0E
#define AUD256K         0x1B
#define AUD352K         0x0D
#define AUD384K         0x05
#define AUD512K         0x3B
#define AUD705K         0x2D
#define AUD768K         0x09
#define AUD1024K        0x35
#define AUD1411K        0x1D
#define AUD1536K        0x15

#define I2S             0
#define SPDIF           1

#define LPCM            0
#define NLPCM           1
#define HBR             2
#define DSD             3

#define TDM2CH          0x0
#define TDM4CH          0x1
#define TDM6CH          0x2
#define TDM8CH          0x3

#define NOCSC           0
#define RGB2YUV         2
#define YUV2RGB         3

#define FrmPkt          0
#define SbSFull         3
#define TopBtm          6
#define SbSHalf         8

#define DDC75K          0
#define DDC125K         1
#define DDC312K         2

#define AUD16BIT        0x2
#define AUD18BIT        0x4
#define AUD20BIT        0x3
#define AUD24BIT        0xB

#define AUDCAL1         0x4
#define AUDCAL2         0x0
#define AUDCAL3         0x8

#define PICAR_NO        0
#define PICAR4_3        1
#define PICAR16_9       2

#define ACTAR_PIC       8
#define ACTAR4_3        9
#define ACTAR16_9       10
#define ACTAR14_9       11

#define CKSOURCE_Internal 1
#define CKSOURCE_ExtXTal 0

// Video Option
#define cfg_LMSwap   0
#define cfg_YCSwap   0
#define cfg_RBSwap   0

#define cfg_XPStableTime   2       // 0: 75us, 1: 50us, 2: 100us
#define cfg_EnXPLockChk   0
#define cfg_EnFFAutoRst   1
#define cfg_EnPLLBufRst   1

#define cfg_ADEShift   0
#define cfg_BDEShift   0


// H2TX Video Format
#define cfg_EnInSwap   0
#define cfg_InSyncAutoSwapOff   1

#define cfg_UpSampSel  0
#define cfg_DNFreeGo  0
#define cfg_EnUdFilt  0
#define cfg_EnDither  0

#define cfg_CINC 0
#define cfg_DIEN 1

#define cfg_SSCPPos   true
#define cfg_SSCPOpt   false
#define cfg_SSCPTest   false
#define cfg_EnSSCPTrgPkt   false
#define cfg_EnAutoPwdAll   true
#define cfg_EnVBOErrPoll   false
#define cfg_EnAlnDetTO   false
#define cfg_DisH2Auto   true
#define cfg_AutoPhReSync   true
#define cfg_RCLKFreqSel   1

#define cfg_TxCLKStbRef   2
#define cfg_VidStbSen   false
#define cfg_EnGRCLKPD   false

#define cfg_EnVBORxVidBypass   false

#define cfg_EnVBORxPCLKStable   false
#define cfg_EnVBORxforceVOut   false
#define cfg_VBORxCLKStbRef   2
#define cfg_EnRxDEOnly   false
#define cfg_EnPiRecParaChgInt   true

#define cfg_EnDDCMasterSel   false
#define cfg_EnDDCMasterArb   true

#define cfg_EnInDDR   false
#define cfg_En24BDDR   false

#define cfg_VidLatEdge   false

#define cfg_RecChStSel   false
#define cfg_AudLatEdge   false
#define cfg_ForceRxOn false

#define cfg_EnSyncDetChk false
#define cfg_EnHDCPAutoMute 1
#define cfg_EnSyncDet2FailInt 0
#define cfg_EnRiChk2DoneInt 0
#define cfg_EnAutoReAuth 0
#define cfg_LClimit 0
#define cfg_CKSOURCE CKSOURCE_Internal
#define cfg_EnPNSwap 0
#define cfg_EnFlagUpdRd 1
#define cfg_KeepOutOpt 1

#define RCLK_TYPICAL 11059

//--------------------  it6265_drv.h end   ---------------------------

//--------------------  it6265_io.h start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_io.h>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
//
//  the declare of I/O functions
//  Must be implemented
//
#ifdef __cplusplus
extern "C" {
#endif

void __reset_vbotx( int vic, int lane_count, int byte_mode );

    //  Declare this because the print functions
    void _trace(char *format, ...);
    void _trace2(char *format, ...);
    void _trace3(char *format, ...);
    void _error(char *format, ...);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
//  callback when HPD ( Hot-plug detection ) is changed,
//  'hpd_state' is the current HPD status and will be '0' or '1' .
void it6265_hpd_change_callback(unsigned char hpd_state, void *callback_data);

//
//  callback when RX termination is changed,
//  'rxsen_state' is the current termination status and will be '0' or '1' .
void it6265_rxsen_change_callback(unsigned char rxsen_state, void *callback_data);

//
//  callback when EDID is read.
//  the '*edid_data' will contain 128 bytes EDID data
//  the 'block_index' indicate the EDID block index,
//  for example:
//      if 'block_index' == 0, '*edid_data' contains the EDID data on offset 0~127
//      if 'block_index' == 1, '*edid_data' contains the EDID data on offset 128~255
void it6265_edid_read_callback(unsigned char block_index, void *edid_data, void *callback_data);

//
//  callback when HDCP authentication fired.
//  if 'hdcp_version' == 1, means HDCP version 1.4 is running.
//  if 'hdcp_version' == 2, means HDCP version 2.2 is running.
void it6265_hdcp_start_callback(unsigned char hdcp_version, void *callback_data);

//
//  callback when HDCP authentication done.
//  if 'hdcp_version' == 1, means HDCP version 1.4 is running.
//  if 'hdcp_version' == 2, means HDCP version 2.2 is running.
void it6265_hdcp_done_callback(unsigned char hdcp_version, void *callback_data);

//
//  callback when HDCP authentication failed.
//  if 'hdcp_version' == 1, means HDCP version 1.4 is running.
//  if 'hdcp_version' == 2, means HDCP version 2.2 is running.
//  'status' will report the failure reason.
void it6265_hdcp_fail_callback(unsigned char hdcp_version, unsigned char status, void *callback_data);

//
//  callback when HDCP 1.4 BStatus is readback.
//  'bstatus' contains the information of BStatus (2 bytes).
void it6265_hdcp_BStatus_callback(unsigned short bstatus, void *callback_data);

//
//  callback when HDCP 1.4 BKSV is readback.
//  '*bksv' contains the information of BKSV (5 bytes).
void it6265_hdcp_BKSV_callback(unsigned char *bksv, void *callback_data);

//
//  callback when HDCP 1.4 KSV list is arrived.
//  'ksv_count' means the numbers of keys in list ( 5 bytes for each )
//  '*ksv_list' contains the information of KSV list.
void it6265_hdcp_ksv_list_callback(unsigned char ksv_count, unsigned char *ksv_list, void *callback_data);

//
//  callback when HDCP 2.2 RxInfo is readback.
//  'rx_info' contains the information of RxInfo (2 bytes).
void it6265_hdcp2_rxinfo_callback(unsigned short rx_info, void *callback_data);

//
//  callback when HDCP 2.2 RxID is readback.
//  '*rxid' contains the information of RxID (5 bytes).
void it6265_hdcp2_rxid_callback(unsigned char *rxid, void *callback_data);

//
//  callback when HDCP 2.2 RxID list is arrived.
//  'rxid_count' means the numbers of RxID in list ( 5 bytes for each )
//  '*rxid_list' contains the information of RxID list.
void it6265_hdcp2_rxid_list_callback(unsigned char rxid_count, unsigned char *rxid_list, void *callback_data);

//
//  callback when HDCP 2.2 Repeater state ready to fire stream manage.
//  IT6265 will hold the current state and inform the repeater RX side.
//
//  If the upstream TX stream manage is ready,
//  "it6265_hdcp2_continue_stream_manage()" should be called to continue the HDCP process.
void it6265_hdcp2_repeater_wait_tx_stream_manage_callback(void *callback_data);

//
//  callback when HDCP 2.2 Repeater state goes to SKE_Send_Eks.
//  IT6265 will hold the current state and inform the repeater RX side.
//
//  If the upstream TX completed SKE_Send_Eks,
//  "it6265_hdcp2_continue_SEK_send_Eks()" should be called to continue the HDCP process.
void it6265_hdcp2_repeater_wait_tx_eks_callback(void *callback_data);

//
//  callback when HDCP 2.2 Reauthentication is required.
void it6265_hdcp2_rx_reauth_req_callback(void *callback_data);

//
//  callback when HDCP 2.2 Stream manage failed.
void it6265_hdcp2_stream_manage_fail_callback(void *callback_data);

//
//  callback when read HDCP version.
void it6265_hdcp_version_callback(unsigned char hdcp_version, void *callback_data);

#ifdef __cplusplus
}
#endif
//--------------------  it6265_io.h end   ---------------------------

//--------------------  it6265.c start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265.c>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
#ifdef __cplusplus
extern "C" {
#endif

struct it6265dev g_dev;

#ifdef __cplusplus
}
#endif

#if DEBUG_SHOW_REG_RW
char g_is_set = 0;
#endif


u8 It6265::hdmitxrd(u8 offset)
{
    u8 tmp;

    it6265_i2c_read(H2TxAddr, offset, 1, &tmp);

    return tmp;
}

void It6265::hdmitxwr(u8 offset, u8 rddata)
{
    bool ret;

    #if DEBUG_SHOW_REG_RW
    if ( g_is_set == 0 )     {
        pr_info("hdmitxwr(0x%02x, 0x%02x);\r\n", offset, rddata);
        //pr_info("i2creg -w %02x %02x %02x\n", H2TxAddr, offset, rddata );
        //pr_info("     pccmd w %02x %02x %02x\n", offset, rddata, H2TxAddr);
    }
    else {
        g_is_set = 0;
    }
    #endif

    //for(;;)
    {
        ret = it6265_i2c_write(H2TxAddr, offset, 1, &rddata);
        if ( ret ) {
            return ;
        }
    }

}

void It6265::hdmitxset(u8 offset, u8 mask, u8 rddata)
{   
    u8 temp;

    #if DEBUG_SHOW_REG_RW
    pr_info("hdmitxset(0x%02x, 0x%02x, 0x%02x);\r\n", offset, mask, rddata);
    //pr_info("i2creg -s %02x %02x %02x %02x\n", H2TxAddr, offset, mask, rddata );
    //pr_info("     pccmd s %02x %02x %02x %02x\n", offset, mask, rddata, H2TxAddr);
    g_is_set = 1;
    #endif
    // hdmitxset(0x05, 0x48, 0x48);
    temp = hdmitxrd(offset);
    temp = (temp & ((~mask) & 0xFF)) + (mask & rddata);

    hdmitxwr(offset, temp);
}

void It6265::hdmitxbrd(u8 offset, u8 length, u8 *rddata)
{
//    pr_info("hdmitxbrd length is %d\n", length);
    it6265_i2c_read(H2TxAddr, offset, length, rddata);
}

void It6265::vborxwr(u8 offset, u8 rddata)
{
    #if DEBUG_SHOW_REG_RW
    if ( g_is_set == 0 ) {
        pr_info("vborxwr(0x%02x, 0x%02x);\r\n", offset, rddata);
    }
    else {
        g_is_set = 0;
    }
    #endif

    it6265_i2c_write(VBORxAddr, offset, 1, &rddata);
}

u8 It6265::vborxrd(u8 offset)
{
    u8 tmp;

    it6265_i2c_read(VBORxAddr, offset, 1, &tmp);

    return tmp;
}

void It6265::vborxset(u8 offset, u8 mask, u8 rddata)
{
    u8 temp;

    #if DEBUG_SHOW_REG_RW
    pr_info("vborxset(0x%02x, 0x%02x, 0x%02x);\r\n", offset, mask, rddata);
    g_is_set = 1;
    #endif

    temp = vborxrd(offset);
    temp = (temp & ((~mask) & 0xFF)) + (mask & rddata);

    vborxwr(offset, temp);
}

void It6265::vborxbrd(u8 offset, u8 byteno, u8 *rddata)
{
    it6265_i2c_read(VBORxAddr, offset, byteno, rddata);
}

void It6265::cecwr(u8 offset, u8 rddata)
{
    #if DEBUG_SHOW_REG_RW
    if ( g_is_set == 0 ) {
        pr_info("cecwr(0x%02x, 0x%02x);\r\n", offset, rddata);
    }
    else {
        g_is_set = 0;
    }
    #endif

    it6265_i2c_write(CECAddr, offset, 1, &rddata);
}

u8 It6265::cecrd(u8 offset)
{
    u8 tmp;

    it6265_i2c_read(CECAddr, offset, 1, &tmp);

    return tmp;
}

void It6265::cecset( u8 offset, u8 mask, u8 rddata )
{
    u8 temp;

    #if DEBUG_SHOW_REG_RW
    pr_info("cecset(0x%02x, 0x%02x, 0x%02x);\r\n", offset, mask, rddata);
    g_is_set = 1;
    #endif

    temp = cecrd(offset);
    temp = (temp&((~mask)&0xFF))+(mask&rddata);

    cecwr(offset, temp);
}

void It6265::chgrxbank(u8 bankno)
{
    vborxset(0x0f, 0x03, bankno & 0x03);
}

void It6265::chgtxbank(u8 bankno)
{
    hdmitxset(0x0f, 0x03, bankno & 0x03);
}


CONST u8 csc_rgb2yuv_ITU601_DynCEA[] =
    {0x00,0x80,0x10,0xB2,0x04,0x65,0x02,0xE9,0x00,0x93,0x3C,0x18,0x04,0x55,0x3F,0x49,0x3D,0x9F,0x3E,0x18,0x04};

CONST u8 csc_rgb2yuv_ITU601_DynVESA[] =
    {0x10,0x80,0x10,0x09,0x04,0x0E,0x02,0xC9,0x00,0x0F,0x3D,0x84,0x03,0x6D,0x3F,0xAB,0x3D,0xD1,0x3E,0x84,0x03};

CONST u8 csc_rgb2yuv_ITU709_DynCEA[] =
    {0x00,0x80,0x10,0xB8,0x05,0xB4,0x01,0x94,0x00,0x4A,0x3C,0x17,0x04,0x9F,0x3F,0xD9,0x3C,0x10,0x3F,0x17,0x04};

CONST u8 csc_rgb2yuv_ITU709_DynVESA[] =
    {0x10,0x80,0x10,0xEA,0x04,0x77,0x01,0x7F,0x00,0xD0,0x3C,0x83,0x03,0xAD,0x3F,0x4B,0x3D,0x32,0x3F,0x83,0x03};

CONST u8 csc_yuv2rgb_ITU601_DynCEA[] =
    {0x00,0x00,0x00,0x00,0x08,0x6B,0x3A,0x50,0x3D,0x00,0x08,0xF5,0x0A,0x02,0x00,0x00,0x08,0xFD,0x3F,0xDA,0x0D};

CONST u8 csc_yuv2rgb_ITU601_DynVESA[] =
    {0x04,0x00,0xA7,0x4F,0x09,0x81,0x39,0xDD,0x3C,0x4F,0x09,0xC4,0x0C,0x01,0x00,0x4F,0x09,0xFD,0x3F,0x1F,0x10};

CONST u8 csc_yuv2rgb_ITU709_DynCEA[] =
    {0x00,0x00,0x00,0x00,0x08,0x55,0x3C,0x88,0x3E,0x00,0x08,0x51,0x0C,0x00,0x00,0x00,0x08,0x00,0x00,0x84,0x0E};

CONST u8 csc_yuv2rgb_ITU709_DynVESA[] =
    {0x04,0x00,0xA7,0x4F,0x09,0xBA,0x3B,0x4B,0x3E,0x4F,0x09,0x57,0x0E,0x02,0x00,0x4F,0x09,0xFE,0x3F,0xE8,0x10};

// BT2020
//CONST u8 csc_rgb2yuv_ITU2020_DynCEA[] =  // 16-235
//    {0x00, 0x80, 0x10, 0x6C, 0x05, 0x1A, 0x02, 0x7A, 0x00, 0x40, 0x3C, 0x18, 0x04, 0xA8, 0x3F, 0x0C, 0x3D, 0xDD, 0x3E, 0x18, 0x04};

CONST u8 csc_rgb2yuv_ITU2020_DynCEA[] =  // 16-235 - new 2018 11/21
    //{0x00, 0x80, 0x10, 0x6C, 0x05, 0x1A, 0x02, 0x7A, 0x00, 0x53, 0x3C, 0x00, 0x04, 0xAE, 0x3F, 0x1E, 0x3D, 0xE3, 0x3E, 0x00, 0x04};
    {0x00 ,0x80 ,0x10 ,0x6C ,0x05 ,0x1A ,0x02 ,0x7A ,0x00 ,0x40 ,0x3C ,0x18 ,0x04 ,0xA8 ,0x3F ,0x0C ,0x3D ,0xDD, 0x3E ,0x18 ,0x04};

CONST u8 csc_rgb2yuv_ITU2020_DynVESA[] = // 0-255
    //{0x10, 0x80, 0x10, 0xA8, 0x04, 0xCE, 0x01, 0x68, 0x00, 0xC7, 0x3C, 0x84, 0x03, 0xB5, 0x3F, 0x77, 0x3D, 0x06, 0x3F, 0x83, 0x03};
    {0x10, 0x80, 0x10, 0xA8, 0x04, 0xCE, 0x01, 0x68, 0x00, 0xC7, 0x3C, 0x84, 0x03, 0xB5, 0x3F, 0x77, 0x3D, 0x06, 0x3F, 0x83, 0x03};

CONST u8 csc_yuv2rgb_ITU2020_DynCEA[] =
    //{0x00, 0x00, 0x00, 0x00, 0x80, 0x87, 0x3B, 0xB4, 0x3E, 0x00, 0x80, 0x87, 0x0B, 0x0A, 0x00, 0x00, 0x80, 0xFE, 0x3F, 0xB5, 0x0E};
    {0x00, 0x00, 0x00, 0x00, 0x08, 0x87, 0x3B, 0xB4, 0x3E, 0x00, 0x08, 0x87, 0x0B, 0x0A, 0x00, 0x00, 0x08, 0xFE, 0x3F, 0xB5, 0x0E};

CONST u8 csc_yuv2rgb_ITU2020_DynVESA[] =
    {0x04, 0x00, 0xA7, 0x4F, 0x09, 0xCC, 0x3A, 0x7E, 0x3E, 0x4F, 0x09, 0x69, 0x0D, 0x0B, 0x00, 0x4F, 0x09, 0xFE, 0x3F, 0x1D, 0x11};

#define PROG 1
#define INTR 0
#define Vneg 0
#define Hneg 0
#define Vpos 1
#define Hpos 1

#define AR_4_3 1
#define AR_16_9 2
#define AR_14_9 3
#define AR_64_27 4
#define AR_256_135 5

CONST VTiming s_VMTable[] = {
    {640,480,800,525,25175,16,96,48,10,2,33,59940,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt1 640x480P@59.94Hz",1},

    {720,480,858,525,27000,16,62,60,9,6,30,59940,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt2 720x480P@59.94Hz",2},
    {720,480,858,525,27000,16,62,60,9,6,30,59940,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt3 720x480P@59.94Hz",3},

    {1280,720,1650,750,74250,110,40,220,5,5,20,60000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt4 1280x720P@60Hz",4},
    {1280,720,1650,750,74250,110,40,220,5,5,20,60000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt69 1280x720P@60Hz",69},

    {1920,540,2200,562,74250,88,44,148,2,5,15,60000,INTR,Vpos,Hpos,1,AR_16_9,"CEA Fmt5 1920x1080I@60Hz",5},

    {1440,240,1716,262,27000,38,124,114,4,3,15,59940,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt6 1440x480I@59.94Hz",6},
    {1440/2,240,1716/2,262,27000/2,38/2,124/2,114/2,4,3,15,59940,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt6 720x480I@59.94Hz",6},

    {1440,240,1716,262,27000,38,124,114,4,3,15,59940,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt7 1440x480I@59.94Hz",7},
    {1440/2,240,1716/2,262,27000/2,38/2,124/2,114/2,4,3,15,59940,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt7 720x480I@59.94Hz",7},

    {1440,240,1716,262,27000,38,124,114,4,3,15,60054,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt8 (mode 1) 1440x240P@60.054Hz",8},
    {1440,240,1716,263,27000,38,124,114,5,3,15,59826,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt8 (mode 2) 1440x240P@59.826Hz",8},
    {1440/2,240,1716/2,262,27000/2,38/2,124/2,114/2,4,3,15,60054,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt8 (mode 1) 720x240P@60.054Hz",8},
    {1440/2,240,1716/2,263,27000/2,38/2,124/2,114/2,5,3,15,59826,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt8 (mode 2) 720x240P@59.826Hz",8},

    {1440,240,1716,262,27000,38,124,114,4,3,15,60054,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt9 (mode 1) 1440x240P@60.054Hz",9},
    {1440,240,1716,263,27000,38,124,114,5,3,15,59826,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt9 (mode 2) 1440x240P@59.826Hz",9},
    {1440/2,240,1716/2,262,27000/2,38/2,124/2,114/2,4,3,15,60054,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt9 (mode 1) 720x240P@60.054Hz",9},
    {1440/2,240,1716/2,263,27000/2,38/2,124/2,114/2,5,3,15,59826,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt9 (mode 2) 720x240P@59.826Hz",9},

    {2880,240,3432,262,54000,76,248,228,4,3,15,59940,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt10 2880x480I@59.94Hz",10},
    {2880/2,240,3432/2,262,54000/2,76/2,248/2,228/2,4,3,15,59940,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt10 1440x480I@59.94Hz",10},
    {2880/4,240,3432/4,262,54000/4,76/4,248/4,228/4,4,3,15,59940,INTR,Vneg,Hneg,4,AR_4_3,"CEA Fmt10 720x480I@59.94Hz",10},

    {2880,240,3432,262,54000,76,248,228,4,3,15,59940,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt11 2880x480I@59.94Hz",11},
    {2880/2,240,3432/2,262,54000/2,76/2,248/2,228/2,4,3,15,59940,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt11 1440x480I@59.94Hz",11},
    {2880/4,240,3432/4,262,54000/4,76/4,248/4,228/4,4,3,15,59940,INTR,Vneg,Hneg,4,AR_16_9,"CEA Fmt11 720x480I@59.94Hz",11},

    {2880,240,3432,262,54000,76,248,228,4,3,15,60054,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt12 (mode 1) 2880x240P@60.054Hz",12},
    {2880,240,3432,263,54000,76,248,228,5,3,15,59826,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt12 (mode 2) 2880x240P@59.826Hz",12},
    {2880/2,240,3432/2,262,54000/2,76/2,248/2,228/2,4,3,15,60054,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt12 (mode 1) 1440x240P@60.054Hz",12},
    {2880/2,240,3432/2,263,54000/2,76/2,248/2,228/2,5,3,15,59826,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt12 (mode 2) 1440x240P@59.826Hz",12},
    {2880/4,240,3432/4,262,54000/4,76/4,248/4,228/4,4,3,15,60054,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt12 (mode 1) 720x240P@60.054Hz",12},
    {2880/4,240,3432/4,263,54000/4,76/4,248/4,228/4,5,3,15,59826,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt12 (mode 2) 720x240P@59.826Hz",12},

    {2880,240,3432,262,54000,76,248,228,4,3,15,60054,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt13 (mode 1) 2880x240P@60.054Hz",13},
    {2880,240,3432,263,54000,76,248,228,5,3,15,59826,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt13 (mode 2) 2880x240P@59.826Hz",13},
    {2880/2,240,3432/2,262,54000/2,76/2,248/2,228/2,4,3,15,60054,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt13 (mode 1) 1440x240P@60.054Hz",13},
    {2880/2,240,3432/2,263,54000/2,76/2,248/2,228/2,5,3,15,59826,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt13 (mode 2) 1440x240P@59.826Hz",13},
    {2880/4,240,3432/4,262,54000/4,76/4,248/4,228/4,4,3,15,60054,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt13 (mode 1) 720x240P@60.054Hz",13},
    {2880/4,240,3432/4,263,54000/4,76/4,248/4,228/4,5,3,15,59826,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt13 (mode 2) 720x240P@59.826Hz",13},

    {1440,480,1716,525,54000,32,124,120,9,6,30,59940,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt14 1440x480P@59.94Hz",14},
    {1440/2,480,1716/2,525,54000/2,32/2,124/2,120/2,9,6,30,59940,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt14 720x480P@59.94Hz",14},

    {1440,480,1716,525,54000,32,124,120,9,6,30,59940,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt15 1440x480P@59.94Hz",15},
    {1440/2,480,1716/2,525,54000/2,32/2,124/2,120/2,9,6,30,59940,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt15 720x480P@59.94Hz",15},

    {1920,1080,2200,1125,148500,88,44,148,4,5,36,60000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt16 1920x1080P@60Hz",16},
    {1920,1080,2200,1125,148500,88,44,148,4,5,36,60000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt76 1920x1080P@60Hz",76},

    {720,576,864,625,27000,12,64,68,5,5,39,50000,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt17 720x576P@50Hz",17},
    {720,576,864,625,27000,12,64,68,5,5,39,50000,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt18 720x576P@50Hz",18},

    {1280,720,1980,750,74250,440,40,220,5,5,20,50000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt19 1280x72P0@50Hz",19},
    {1280,720,1980,750,74250,440,40,220,5,5,20,50000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt68 1280x72P0@50Hz",68},

    {1920,540,2640,562,74250,528,44,148,2,5,15,50000,INTR,Vpos,Hpos,1,AR_16_9,"CEA Fmt20 1920x1080I@50Hz",20},

    {1440,288,1728,312,27000,24,126,138,2,3,19,50000,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt21 1440x576I@50Hz",21},
    {1440/2,288,1728/2,312,27000/2,24/2,126/2,138/2,2,3,19,50000,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt21 720x576I@50Hz",21},

    {1440,288,1728,312,27000,24,126,138,2,3,19,50000,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt22 1440x576I@50Hz",22},
    {1440/2,288,1728/2,312,27000/2,24/2,126/2,138/2,2,3,19,50000,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt22 720x576I@50Hz",22},

    {1440,288,1728,312,27000,24,126,138,2,3,19,50080,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt23 (mode 1) 1440x288P@50.080Hz",23},
    {1440,288,1728,313,27000,24,126,138,3,3,19,49920,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt23 (mode 2) 1440x288P@49.920Hz",23},
    {1440,288,1728,314,27000,24,126,138,4,3,19,49761,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt23 (mode 3) 1440x288P@49.761Hz",23},
    {1440/2,288,1728/2,312,27000/2,24/2,126/2,138/2,2,3,19,50080,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt23 (mode 1) 720x288P@50.080Hz",23},
    {1440/2,288,1728/2,313,27000/2,24/2,126/2,138/2,3,3,19,49920,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt23 (mode 2) 720x288P@49.920Hz",23},
    {1440/2,288,1728/2,314,27000/2,24/2,126/2,138/2,4,3,19,49761,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt23 (mode 3) 720x288P@49.761Hz",23},

    {1440,288,1728,312,27000,24,126,138,2,3,19,50080,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt24 (mode 1) 1440x288P@50.080Hz",24},
    {1440,288,1728,313,27000,24,126,138,3,3,19,49920,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt24 (mode 2) 1440x288P@49.920Hz",24},
    {1440,288,1728,314,27000,24,126,138,4,3,19,49761,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt24 (mode 3) 1440x288P@49.761Hz",24},
    {1440/2,288,1728/2,312,27000/2,24/2,126/2,138/2,2,3,19,50080,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt24 (mode 1) 720x288P@50.080Hz",24},
    {1440/2,288,1728/2,313,27000/2,24/2,126/2,138/2,3,3,19,49920,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt24 (mode 2) 720x288P@49.920Hz",24},
    {1440/2,288,1728/2,314,27000/2,24/2,126/2,138/2,4,3,19,49761,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt24 (mode 3) 720x288P@49.761Hz",24},

    {2880,288,3456,312,54000,48,252,276,2,3,19,50000,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt25 2880x576I@50Hz",25},
    {2880/2,288,3456/2,312,54000/2,48/2,252/2,276/2,2,3,19,50000,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt25 1440x576I@50Hz",25},
    {2880/4,288,3456/4,312,54000/4,48/4,252/4,276/4,2,3,19,50000,INTR,Vneg,Hneg,4,AR_4_3,"CEA Fmt25 720x576I@50Hz",25},

    {2880,288,3456,312,54000,48,252,276,2,3,19,50000,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt26 2880x576I@50Hz",26},
    {2880/2,288,3456/2,312,54000/2,48/2,252/2,276/2,2,3,19,50000,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt26 1440x576I@50Hz",26},
    {2880/4,288,3456/4,312,54000/4,48/4,252/4,276/4,2,3,19,50000,INTR,Vneg,Hneg,4,AR_16_9,"CEA Fmt26 720x576I@50Hz",26},

    {2880,288,3456,312,54000,48,252,276,2,3,19,50080,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt27 (mode 1) 2880x288P@50.080Hz",27},
    {2880,288,3456,313,54000,48,252,276,3,3,19,49920,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt27 (mode 2) 2880x288P@49.920Hz",27},
    {2880,288,3456,314,54000,48,252,276,4,3,19,49761,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt27 (mode 3) 2880x288P@49.761Hz",27},
    {2880/2,288,3456/2,312,54000/2,48/2,252/2,276/2,2,3,19,50080,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt27 (mode 1) 1440x288P@50.080Hz",27},
    {2880/2,288,3456/2,313,54000/2,48/2,252/2,276/2,3,3,19,49920,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt27 (mode 2) 1440x288P@49.920Hz",27},
    {2880/2,288,3456/2,314,54000/2,48/2,252/2,276/2,4,3,19,49761,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt27 (mode 3) 1440x288P@49.761Hz",27},
    {2880/4,288,3456/4,312,54000/4,48/4,252/4,276/4,2,3,19,50080,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt27 (mode 1) 720x288P@50.080Hz",27},
    {2880/4,288,3456/4,313,54000/4,48/4,252/4,276/4,3,3,19,49920,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt27 (mode 2) 720x288P@49.920Hz",27},
    {2880/4,288,3456/4,314,54000/4,48/4,252/4,276/4,4,3,19,49761,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt27 (mode 3) 720x288P@49.761Hz",27},

    {2880,288,3456,312,54000,48,252,276,2,3,19,50080,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt28 (mode 1) 2880x288P@50.080Hz",28},
    {2880,288,3456,313,54000,48,252,276,3,3,19,49920,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt28 (mode 2) 2880x288P@49.920Hz",28},
    {2880,288,3456,314,54000,48,252,276,4,3,19,49761,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt28 (mode 3) 2880x288P@49.761Hz",28},
    {2880/2,288,3456/2,312,54000/2,48/2,252/2,276/2,2,3,19,50080,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt28 (mode 1) 1440x288P@50.080Hz",28},
    {2880/2,288,3456/2,313,54000/2,48/2,252/2,276/2,3,3,19,49920,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt28 (mode 2) 1440x288P@49.920Hz",28},
    {2880/2,288,3456/2,314,54000/2,48/2,252/2,276/2,4,3,19,49761,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt28 (mode 3) 1440x288P@49.761Hz",28},
    {2880/4,288,3456/4,312,54000/4,48/4,252/4,276/4,2,3,19,50080,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt28 (mode 1) 720x288P@50.080Hz",28},
    {2880/4,288,3456/4,313,54000/4,48/4,252/4,276/4,3,3,19,49920,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt28 (mode 2) 720x288P@49.920Hz",28},
    {2880/4,288,3456/4,314,54000/4,48/4,252/4,276/4,4,3,19,49761,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt28 (mode 3) 720x288P@49.761Hz",28},

    {1440,576,1728,625,54000,24,128,136,5,5,39,50000,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt29 1440x576P@50Hz",29},
    {1440/2,576,1728/2,625,54000/2,24/2,128/2,136/2,5,5,39,50000,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt29 720x576P@50Hz",29},

    {1440,576,1728,625,54000,24,128,136,5,5,39,50000,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt30 1440x576P@50Hz",30},
    {1440/2,576,1728/2,625,54000/2,24/2,128/2,136/2,5,5,39,50000,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt30 720x576P@50Hz",30},

    {1920,1080,2640,1125,148500,528,44,148,4,5,36,50000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt31 1920x1080P@50Hz",31},
    {1920,1080,2640,1125,148500,528,44,148,4,5,36,50000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt75 1920x1080P@50Hz",75},

    {1920,1080,2750,1125,74250,638,44,148,4,5,36,24000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt32 1920x1080P@24Hz",32},
    {1920,1080,2750,1125,74250,638,44,148,4,5,36,24000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt72 1920x1080P@24Hz",72},

    {1920,1080,2640,1125,74250,528,44,148,4,5,36,25000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt33 1920x1080P@25Hz",33},
    {1920,1080,2640,1125,74250,528,44,148,4,5,36,25000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt73 1920x1080P@25Hz",73},

    {1920,1080,2200,1125,74250,88,44,148,4,5,36,30000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt34 1920x1080P@30Hz",34},
    {1920,1080,2200,1125,74250,88,44,148,4,5,36,30000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt74 1920x1080P@30Hz",74},

    {2880,480,3432,525,108000,64,248,240,9,6,30,59940,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt35 2880x480P@59.94Hz",35},
    {2880/2,480,3432/2,525,108000/2,64/2,248/2,240/2,9,6,30,59940,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt35 1440x480P@59.94Hz",35},
    {2880/4,480,3432/4,525,108000/4,64/4,248/4,240/4,9,6,30,59940,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt35 720x480P@59.94Hz",35},

    {2880,480,3432,525,108000,64,248,240,9,6,30,59940,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt36 2880x480P@59.94Hz",36},
    {2880/2,480,3432/2,525,108000/2,64/2,248/2,240/2,9,6,30,59940,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt36 1440x480P@59.94Hz",36},
    {2880/4,480,3432/4,525,108000/4,64/4,248/4,240/4,9,6,30,59940,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt36 720x480P@59.94Hz",36},

    {2880,576,3456,625,108000,48,256,272,5,5,39,50000,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt37 2880x576P@50Hz",37},
    {2880/2,576,3456/2,625,108000/2,48/2,256/2,272/2,5,5,39,50000,PROG,Vneg,Hneg,2,AR_4_3,"CEA Fmt37 1440x576P@50Hz",37},
    {2880/4,576,3456/4,625,108000/4,48/4,256/4,272/4,5,5,39,50000,PROG,Vneg,Hneg,4,AR_4_3,"CEA Fmt37 720x576P@50Hz",37},

    {2880,576,3456,625,108000,48,256,272,5,5,39,50000,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt38 2880x576P@50Hz",38},
    {2880/2,576,3456/2,625,108000/2,48/2,256/2,272/2,5,5,39,50000,PROG,Vneg,Hneg,2,AR_16_9,"CEA Fmt38 1440x576P@50Hz",38},
    {2880/4,576,3456/4,625,108000/4,48/4,256/4,272/4,5,5,39,50000,PROG,Vneg,Hneg,4,AR_16_9,"CEA Fmt38 720x576P@50Hz",38},

    {1920,540,2304,625,72000,32,168,184,23,5,57,50000,INTR,Vneg,Hpos,1,AR_16_9,"CEA Fmt39 1920x1080I@50Hz",39},

    {1920,540,2640,562,148500,528,44,148,2,5,15,100000,INTR,Vpos,Hpos,1,AR_16_9,"CEA Fmt40 1920x1080I@100Hz",40},

    {1280,720,1980,750,148500,440,40,220,5,5,20,100000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt41 1280x720P@100Hz",41},
    {1280,720,1980,750,148500,440,40,220,5,5,20,100000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt70 1280x720P@100Hz",70},

    {720,576,864,625,54000,12,64,68,5,5,39,100000,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt42 720x576P@100Hz",42},
    {720,576,864,625,54000,12,64,68,5,5,39,100000,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt43 720x576P@100Hz",43},

    {1440,288,1728,312,54000,24,126,138,2,3,19,100000,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt44 1440x576I@100Hz",44},
    {1440/2,288,1728/2,312,54000/2,24/2,126/2,138/2,2,3,19,100000,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt44 720x576I@100Hz",44},

    {1440,288,1728,312,54000,24,126,138,2,3,19,100000,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt45 1440x576I@100Hz",45},
    {1440/2,288,1728/2,312,54000/2,24/2,126/2,138/2,2,3,19,100000,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt45 720x576I@100Hz",45},

    {1920,540,2200,562,148500,88,44,148,2,5,15,120000,INTR,Vpos,Hpos,1,AR_16_9, "CEA Fmt46 1920x1080I@120Hz",46},

    {1280,720,1650,750,148500,110,40,220,5,5,20,120000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt47 1280x720P@120Hz",47},
    {1280,720,1650,750,148500,110,40,220,5,5,20,120000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt71 1280x720P@120Hz",71},

    {720,480,858,525,54000,16,62,60,9,6,30,119880,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt48 720x480P@119.88HzHz",48},
    {720,480,858,525,54000,16,62,60,9,6,30,119880,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt49 720x480P@119.88HzHz",49},

    {1440,240,1716,262,54000,38,124,114,4,3,15,119880,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt50 1440x480I@119.88Hz",50},
    {1440/2,240,1716/2,262,54000/2,38/2,124/2,114/2,4,3,15,119880,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt50 720x480I@119.88Hz",50},

    {1440,240,1716,262,54000,38,124,114,4,3,15,119880,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt51 1440x480I@119.88Hz",51},
    {1440/2,240,1716/2,262,54000/2,38/2,124/2,114/2,4,3,15,119880,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt51 720x480I@119.88Hz",51},

    {720,576,864,625,108000,12,64,68,5,5,39,200000,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt52 720x576P@200Hz",52},
    {720,576,864,625,108000,12,64,68,5,5,39,200000,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt53 720x576P@200Hz",53},

    {1440,288,1728,312,108000,24,126,138,2,3,19,200000,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt54 1440x576I@200Hz",54},
    {1440/2,288,1728/2,312,108000/2,24/2,126/2,138/2,2,3,19,200000,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt54 720x576I@200Hz",54},

    {1440,288,1728,312,108000,24,126,138,2,3,19,200000,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt55 1440x576I@200Hz",55},
    {1440/2,288,1728/2,312,108000/2,24/2,126/2,138/2,2,3,19,200000,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt55 720x576I@200Hz",55},

    {720,480,858,525,108000,16,62,60,9,6,30,239760,PROG,Vneg,Hneg,1,AR_4_3,"CEA Fmt56 720x480P@239.76HzHz",56},
    {720,480,858,525,108000,16,62,60,9,6,30,239760,PROG,Vneg,Hneg,1,AR_16_9,"CEA Fmt57 720x480P@239.76HzHz",57},

    {1440,240,1716,262,108000,38,124,114,4,3,15,239760,INTR,Vneg,Hneg,1,AR_4_3,"CEA Fmt58 1440x480I@239.76Hz",58},
    {1440/2,240,1716/2,262,108000/2,38/2,124/2,114/2,4,3,15,239760,INTR,Vneg,Hneg,2,AR_4_3,"CEA Fmt58 1440x480I@239.76Hz",58},

    {1440,240,1716,262,108000,38,124,114,4,3,15,239760,INTR,Vneg,Hneg,1,AR_16_9,"CEA Fmt59 1440x480I@239.76Hz",59},
    {1440/2,240,1716/2,262,108000/2,38/2,124/2,114/2,4,3,15,239760,INTR,Vneg,Hneg,2,AR_16_9,"CEA Fmt59 1440x480I@239.76Hz",59},

    {1280,720,3300,750,59400,1760,40,220,5,5,20,24000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt60 1280x720P@24Hz",60},
    {1280,720,3300,750,59400,1760,40,220,5,5,20,24000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt65 1280x720P@24Hz",65},

    {1280,720,3960,750,74250,2420,40,220,5,5,20,25000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt61 1280x720P@25Hz",61},
    {1280,720,3960,750,74250,2420,40,220,5,5,20,25000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt66 1280x720P@25Hz",66},

    {1280,720,3300,750,74250,1760,40,220,5,5,20,30000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt62 1280x720P@30Hz",62},
    {1280,720,3300,750,74250,1760,40,220,5,5,20,30000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt67 1280x720P@30Hz",67},

    {1920,1080,2200,1125,297000,88,44,148,4,5,36,119880,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt63 1920x1080P@119.88Hz",63},
    {1920,1080,2200,1125,297000,88,44,148,4,5,36,119880,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt78 1920x1080P@119.88Hz",78},

    {1920,1080,2640,1125,297000,528,44,148,4,5,36,100000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt64 1920x1080P@100Hz",64},
    {1920,1080,2640,1125,297000,528,44,148,4,5,36,100000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt77 1920x1080P@100Hz",77},

    {1680,720,3300,750,59400,1360,40,220,5,5,20,24000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt79 1680x720P@24Hz",79},
    {1680,720,3168,750,59400,1228,40,220,5,5,20,25000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt80 1680x720P@25Hz",80},
    {1680,720,2640,750,59400,700,40,220,5,5,20,30000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt81 1680x720P@30Hz",81},
    {1680,720,2200,750,82500,260,40,220,5,5,20,50000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt82 1680x720P@50Hz",82},
    {1680,720,2200,750,99000,260,40,220,5,5,20,60000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt83 1680x720P@60Hz",83},
    {1680,720,2000,825,165000,60,40,220,5,5,95,100000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt84 1680x720P@100Hz",84},
    {1680,720,2000,825,198000,60,40,220,5,5,95,120000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt85 1680x720P@120Hz",85},

    {2560,1080,3750,1100,99000,998,44,148,4,5,11,24000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt86 2560x1080P@24Hz",86},
    {2560,1080,3200,1125,90000,448,44,148,4,5,36,25000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt87 2560x1080P@25Hz",87},
    {2560,1080,3520,1125,118800,768,44,148,4,5,36,30000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt88 2560x1080P@30Hz",88},
    {2560,1080,3300,1125,185625,548,44,148,4,5,36,50000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt89 2560x1080P@50Hz",89},
    {2560,1080,3000,1100,198000,248,44,148,4,5,11,60000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt90 2560x1080P@60Hz",90},
    {2560,1080,2970,1250,371250,218,44,148,4,5,161,100000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt91 2560x1080P@100Hz",91},
    {2560,1080,3300,1250,495000,548,44,148,4,5,161,120000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt92 2560x1080P@120Hz",92},

    {3840,2160,5500,2250,297000,1276,88,296,8,10,72,24000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt93 3840x2160P@24Hz",93},
    {3840,2160,5280,2250,297000,1056,88,296,8,10,72,25000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt94 3840x2160P@25Hz",94},
    {3840,2160,4400,2250,297000,176,88,296,8,10,72,30000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt95 3840x2160P@30Hz",95},
    {3840,2160,5280,2250,594000,1056,88,296,8,10,72,50000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt96 3840x2160P@50Hz",96},
    {3840,2160,4400,2250,594000,176,88,296,8,10,72,60000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt97 3840x2160P@60Hz",97},

    {3840,2160,5500,2250,297000,1276,88,296,8,10,72,24000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt103 3840x2160P@24Hz",103},
    {3840,2160,5280,2250,297000,1056,88,296,8,10,72,25000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt104 3840x2160P@25Hz",104},
    {3840,2160,4400,2250,297000,176,88,296,8,10,72,30000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt105 3840x2160P@30Hz",105},
    {3840,2160,5280,2250,594000,1056,88,296,8,10,72,50000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt106 3840x2160P@50Hz",106},
    {3840,2160,4400,2250,594000,176,88,296,8,10,72,60000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt107 3840x2160P@60Hz",107},

    {4096,2160,5500,2250,297000,1020,88,296,8,10,72,24000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt98 3840x2160P@24Hz",98},
    {4096,2160,5280,2250,297000,968,88,128,8,10,72,25000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt99 3840x2160P@25Hz",99},
    {4096,2160,4400,2250,297000,88,88,128,8,10,72,30000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt100 3840x2160P@30Hz",100},
    {4096,2160,5280,2250,594000,968,88,128,8,10,72,50000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt101 3840x2160P@50Hz",101},
    {4096,2160,4400,2250,594000,88,88,128,8,10,72,60000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt102 3840x2160P@60Hz",102},

    {3840,2160,4400,2250,594000,32,88,440,78,6,6,60000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt999 3840x2160P@60Hz",999},

    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"Unknown Format Error !!!",0}
};

#define BUF_LEN 16
void It6265::it6265_dump_register( u8 addr, char *reg_desc)
{
    u8 regbuf[BUF_LEN];
    int i,j;

    // print reg description
//    pr_info(reg_desc);

    // print table
    pr_info("   | ");
    for (j=0;j<BUF_LEN;j++)
    {
        if ( j==(BUF_LEN/2) )
        {
            pr_info("- ");
        }
        pr_info("%02X ", j);
    }
    pr_info("\r\n");

    // print split line
    for (j=0;j<BUF_LEN+2;j++)
    {
        pr_info("---");
    }
    pr_info("\r\n");

    // print register values
    for (i=0;i<256;i+=BUF_LEN)
    {
        pr_info("%02X | ", i);
        it6265_i2c_read(addr, (u8)i, BUF_LEN, regbuf);
        for (j=0;j<BUF_LEN;j++)
        {
            if ( j==(BUF_LEN/2) )
            {
                pr_info("- ");
            }
            pr_info("%02x ", regbuf[j]);
        }
        pr_info("\r\n");
    }
    pr_info("\r\n");
}

void It6265::it6265_dump_register_all(void)
{
    it6265_dump_register(H2TxAddr, "\r\n*** TXB 0:\r\n");

    chgtxbank(1);
    it6265_dump_register(H2TxAddr, "\r\n*** TXB 1:\r\n");
    chgtxbank(0);

    chgtxbank(2);
    it6265_dump_register(H2TxAddr, "\r\n*** TXB 2:\r\n");
    chgtxbank(0);

    it6265_dump_register(VBORxAddr, "\r\n*** VBO 0:\n");

    chgrxbank(1);
    it6265_dump_register(VBORxAddr, "\r\n*** VBO 1:\r\n");
    chgrxbank(0);
}
void It6265::it6265_print_buffer( void *buffer, int length )
{
    u8 *buf = (u8 *)buffer;
    int pr_len = 8;

    while (length) {

        if (length < pr_len) {
            pr_len = length;
        }
        else {
            pr_len = 8;
        }

        length -= pr_len;

        while( pr_len ) {
            pr_info( "%02X ", (int)(*buf) );
            pr_len--;
            buf++;
        }
        pr_info( "\r\n" );
    }
}

#define VMTableCount    (sizeof(s_VMTable)/sizeof(VTiming))

const VTiming *It6265::it6265_find_timing_by_vic(u16 vic)
{
    const VTiming *ret = NULL;
    int i;

    for( i=0 ; i<(int)VMTableCount ; i++ )
    {
        if ( s_VMTable[i].VIC == vic )
        {
            ret = &s_VMTable[i];
            break;
        }
    }

    return ret;
}

bool It6265::it6265_get_timing_by_vic(int vic)
{
    const VTiming *vt = it6265_find_timing_by_vic(vic);

    if ( vt ) {
        pr_info("[if] copy vt, vic=%d\r\n", vt->VIC);
        g_dev.input = *vt;
        return true;
    }
    else{
        vt = it6265_find_timing_by_vic(4);
        pr_info("[else] copy vt, vic=%d\r\n", vt->VIC);
        g_dev.input = *vt;
        return false;
    }
}

bool It6265::_value_in_range( u32 value, u32 comp, int tor_percent )
{
    u32 min = comp * (100-tor_percent);
    u32 max = comp * (100+tor_percent);

    value = value *100;
    if ( (value<=max) && (value>=min) )
    {
        return true;
    }
    return false;
}

int It6265::it6265_get_vic_by_timing(VTiming *CurVTiming)
{
    const VTiming *vt;
    VTiming *ct = CurVTiming;
    int i;

    for( i=0 ; i<(int)VMTableCount ; i++ )
    {
        vt = &s_VMTable[i];
        if ( ( vt->HTotal == ct->HTotal ) &&
             ( vt->HActive == ct->HActive ) &&
             ( vt->VTotal == ct->VTotal ) &&
             ( vt->VActive == ct->VActive ) &&
             ( vt->HSyncWidth == ct->HSyncWidth ) &&
             ( vt->VSyncWidth == ct->VSyncWidth ) &&
             ( vt->HFrontPorch == ct->HFrontPorch ) &&
             ( vt->VFrontPorch == ct->VFrontPorch ) &&
             ( _value_in_range(ct->PCLK, vt->PCLK,15))
            )
        {
            return vt->VIC;
        }
    }

    return 0;
}

bool It6265::it6265_is_video_in_stable( void )
{
    pr_info("it6265_is_video_in_stable = %x\r\n", (hdmitxrd(0x07) & 0x18));
    if ((hdmitxrd(0x07) & 0x18) == 0x18) {
        return true;
    }
    return false;
}

bool It6265::it6265_is_rx_sense( void )
{
    if (hdmitxrd(0x07) & 0x04) {
        return true;
    }
    return false;
}

bool It6265::it6265_is_hpd_high( void )
{
    if (hdmitxrd(0x07) & 0x02)  {
        pr_info("Hot Plug detect status\r\n");
        return true;
    }
    pr_info("Hot Plug not detect status\r\n");

    return false;
}

bool It6265::it6265_disable_hdcp2(void)
{
    return 0;
}

void It6265::it6265_enable_timestamp( u8 active )
{
    if (active) {
        g_dev.vars.sys_tick_enabled = 1;
        hdmitxwr(0x09, 0x00);
        hdmitxwr(0x09, 0x80);
    } else {
        g_dev.vars.sys_tick_enabled = 0;
        hdmitxwr(0x09, 0x00);
    }
}

void It6265::it6265_set_vx1_htpdn(u8 active)
{
    if (active) {
        vborxset(0x32, 0x04, 0x04); // EnHTPDN = active
    } else {
        vborxset(0x32, 0x04, 0x00); // EnHTPDN = inactive
    }
}

void It6265::it6265_set_avmute(u8 mute)
{
    u8 reg;
    u8 retry = 10;

__rewrite:
    retry--;
    if ( retry == 0 ) {
        return;
    }

    hdmitxset(0xC1, 0x01, mute);
    reg = hdmitxrd(0xc1);
    if ( mute == 0 )
    {
        if ( reg & 0x01 )
        {
            pr_err("*** it6265_set_avmute error mute=%d\r\n", (int)mute);
            goto __rewrite;
        }
    }
    else
    {
        if ( (reg & 0x01)==0 )
        {
            pr_err("*** it6265_set_avmute error  mute=%d\r\n", (int)mute);
            goto __rewrite;
        }
    }
}

void It6265::it6265_ddc_abort( void )
{
    u8 reg = hdmitxrd(0x28);
    u8 i,j, uc ;

    hdmitxwr(0x28, reg&0xFE); // set FW auto and avoid SCDC fail.
    for( i = 0 ;i < 2 ; i++ )
    {
        hdmitxwr(0x2E, 0x0F);
        for(j = 0 ; j < 50 ; j++ )
        {
            uc = hdmitxrd(0x2F);
            if( uc &0x80)
            {
                // DDC_FW_Stus_DONE
                break ;
            }
            if( uc & 0x38)
            {
                // DDC has something error
                break ;
            }
            idle(1) ;
        }
    }
    hdmitxwr(0x2E, 0x0A);
    hdmitxwr(0x2E, 0x09);
    hdmitxwr(0x28, reg);
}

void It6265::it6265_ddc_fifo_clear( void )
{
    u8 reg = hdmitxrd(0x28);
    hdmitxwr(0x28, reg|0x01);
    hdmitxwr(0x2E, 0x09);
    hdmitxwr(0x28, reg);
}

void It6265::it6265_ddc_status_check(void)
{
    u8 reg;

    reg = hdmitxrd(0x12);
    if ( reg )
    {
        if ( reg & 0x04 ) {
            hdmitxset(0x28, 0x01, 0x01);
            hdmitxwr(0x2E, 0x0F);  // Abort DDC Command
            hdmitxset(0x35, 0x10, 0x10);
            hdmitxset(0x35, 0x10, 0x00);
            hdmitxset(0x28, 0x01, 0x00);
            pr_info("DDCEH: Abort DDC Command\r\n");
        }
        if ( reg & 0x08 ) {
            hdmitxset(0x28, 0x01, 0x01);
            hdmitxwr(0x2E, 0x09);  // DDC FIFO clear
            hdmitxset(0x35, 0x10, 0x10);
            hdmitxset(0x35, 0x10, 0x00);
            hdmitxset(0x28, 0x01, 0x00);
            pr_info("DDCEH: DDC FIFO clear\r\n");
        }
        if ( reg & 0x10 ) {

        }
    }
    reg = hdmitxrd(0x16);
    if ( reg & 0x08 )
    {
        hdmitxset(0x28, 0x01, 0x01);
        hdmitxwr(0x2E, 0x0A);  // DDC bus hang
        hdmitxset(0x35, 0x10, 0x10);
        hdmitxset(0x35, 0x10, 0x00);
        hdmitxset(0x28, 0x01, 0x00);
        pr_info("DDCEH: DDC bus hang\r\n");
    }
}

void It6265::it6265_scdc_status_check( void )
{
    u8 TxReg17;

    if ( g_dev.sink.HDMIVer < 2 ) {
        return;
    }

    TxReg17 = hdmitxrd(0x17);
    hdmitxwr(0x17, 0x30);

    if (TxReg17 & 0x10) {
        pr_info2("HDMI2 SCDC update flag change Interrupt !!!\r\n");
        it6265_scdc_update_flag_handler();
    }

    if (TxReg17 & 0x20) {
        pr_info2("HDMI2 SCDC detect read request Interrupt !!!\r\n");
        it6265_scdc_rr_handler();
    }
}

#define DDCWAITNUM 200
bool It6265::it6265_ddcwait(void)
{
    int ddc_wait_count, ddc_status;

    ddc_wait_count = 0;
    do {
        ddc_wait_count++;
        idle(1);
    } while ((hdmitxrd(0x2F) & 0x80) == 0x00 && ddc_wait_count < DDCWAITNUM);
    pr_info("ddc_wait_count = %d, ddcwaitnum = %d\r\n", ddc_wait_count, DDCWAITNUM);
    if (ddc_wait_count == DDCWAITNUM) {
        ddc_status = hdmitxrd(0x2F) & 0xFE;

        pr_err("ERROR: DDC Bus Wait TimeOut, Status = %02X\r\n", ddc_status);

        if (hdmitxrd(0x16) & 0x08) {
            pr_err("ERROR: DDC Bus Hang !!!\r\n");
        }
        else if (ddc_status & 0x20) {
            pr_err("ERROR: DDC NoACK !!!\r\n");
        }
        else if (ddc_status & 0x10) {
            pr_err("ERROR: DDC WaitBus !!!\r\n");
        }
        else if (ddc_status & 0x08) {
            pr_err("ERROR: DDC ArbiLose !!!\r\n");
        }
        else {
            pr_err("ERROR: UnKnown Issue !!!\r\n");
        }

        //it6265_ddc_status_check();
        hdmitxset(0x28, 0x01, 0x01);
        hdmitxset(0x35, 0x10, 0x10);
        //idle(5000);
        hdmitxset(0x35, 0x10, 0x00);
        hdmitxset(0x28, 0x01, 0x00);

        return false;  // will try again
    } else
    {
        pr_info("it6265_ddcwait success\r\n");
        return true;
    }
}

bool It6265::it6265_scdc_write(u8 offset, u8 scdc_data)
{
    int result;

    pr_info("it6265_scdc_write()\r\n");

    #if CHECK_HPD_BEFORE_RELAY_SCDC
    if (false == it6265_is_hpd_high()) {
        pr_err("ERROR: Abort SCDC write becasue of detecting unplug !!!\r\n");
        return false;
    }
    #endif

    hdmitxset(0x28, 0x01, 0);       // DDC Master set to FW
    hdmitxwr(0x2E, 0x09);           // DDC FIFO Clear
    hdmitxwr(0x29, 0xA8);           // I2C address
    hdmitxwr(0x2A, offset);         // Offset
    hdmitxwr(0x2B, 0x01);           // ReqByte[7:0]
    hdmitxset(0x2C, 0x03, 0x00);    // ReqByte[9:8]
    hdmitxwr(0x30, scdc_data);      // WrData
    hdmitxwr(0x2E, 0x01);           // Sequential Burst Write

    result = it6265_ddcwait();
    hdmitxset(0x28, 0x01, 0x00);

    return result;
}

bool It6265::it6265_scdc_read(u8 offset, u8 bytenum)
{
    bool result;

    #if CHECK_HPD_BEFORE_RELAY_SCDC
    if (false == it6265_is_hpd_high()) {
        pr_err("Abort SCDC read becasue of detecting unplug !!!\r\n");
        return false;
    }
    #endif

    hdmitxset(0x28, 0x01, 0);
    hdmitxwr(0x2E, 0x09);           // DDC FIFO Clear
    hdmitxwr(0x29, 0xA8);           // Header
    hdmitxwr(0x2A, offset);         // Offset
    hdmitxwr(0x2B, bytenum);        // ReqByte[7:0]
    hdmitxset(0x2C, 0x03, 0x00);    // ReqByte[9:8]
    hdmitxwr(0x2E, 0x00);           // Sequential Burst Read
    hdmitxset(0x28, 0x01, 0x00);

    result = it6265_ddcwait();
    hdmitxset(0x28, 0x01, 0x00);

    return result;
}

bool It6265::it6265_hdcp_read(u8 offset, u16 bytenum)
{
    bool result;
    int retry = 1;
    u8 tmp;

__RETRY:

    if (false == it6265_is_hpd_high()) {
        pr_err("Abort hdcp read becasue of detecting unplug !!!\r\n");
        return false;
    }

    it6265_scdc_status_check();

    hdmitxset(0x28, 0x01, 0);
    //hdmitxwr(0x2E, 0x0F);         // Abort DDC Command
    hdmitxwr(0x2E, 0x09);           // DDC FIFO Clear
    hdmitxwr(0x29, 0x74);           // HDCP Address
    hdmitxwr(0x2A, offset);         // HDCP Offset

    tmp = bytenum & 0xFF;
    hdmitxwr(0x2B, tmp);            // Read ByteNum[7:0]

    bytenum = bytenum>>8;
    tmp = bytenum & 0x03;
    hdmitxwr(0x2C, tmp);            // ByteNum[9:8]
                                    //
    hdmitxwr(0x2E, 0x00);           // HDCP Read Fire

    result = it6265_ddcwait();

    hdmitxset(0x28, 0x01, 0x00);    // Disable PC DDC Mode

    if ( result == false ) {
        if ( retry > 0 ) {
            retry--;
            idle( 10 );
            goto __RETRY;
        }
    }

    return result;
}

u8 It6265::it6265_vborx_lane_detect(void)
{
    u8 result;

    vborxset(0x30, 0x80, 0x80);
    idle(500);
    result = vborxrd(0x39);

    pr_info(" >>>> Lane Clock Detection = 0x%02X\r\n", (int)result );

    if ( result == 0 ) {
        pr_err(" >>>> No lane clock detected !!");
    }
    else {

        if (result == 0xFF) { // 8 lane detected
            result = 3; // 8 lane
        }
        else if ((result &0xF)==0x0f) { // lane 0...3 detected, include 0x7f, 0x3f, 0x1f, 0x0f
            result = 2; // 4 lane
        }
        else if ((result &0x3)==0x03) { // lane 0...1 detected, include 0x07, 0x03
            result = 1; // 2 lane
        }
        else if ((result &0x1)==0x01) { // lane 0 detected, include 0x07, 0x03
            result = 0; // 1 lane
        }

        pr_info("Lane setting = %d lane(s),  Detected = %d lane(s)\r\n", (int)(1<<g_dev.vars.VBOLaneNum), (int)(1<<result) );
    }

    return result;
}

void It6265::it6265_check_vborx_fifo(void)
{
    #if AUTO_LANE_DETECT
    u8 lane_det;
    #endif

    g_dev.vars.fifo_err_count++;

    pr_info2("fifo_err_count = %d\r\n", (int) g_dev.vars.fifo_err_count);

    if ( g_dev.vars.fifo_err_count > 30 ) {
        if ( g_dev.vars.fifo_err_handled == 0 ) {

            #if AUTO_LANE_DETECT
            g_dev.vars.VBOLaneNum = 3;
            it6265_setup_vborx();
            #endif

            g_dev.vars.fifo_err_handled ++;
            pr_info2(" >>>> Lane Setting = %d Lane(s)\r\n\r\n", (int)g_dev.vars.lanenum);

            #if AUTO_LANE_DETECT
            lane_det = it6265_vborx_lane_detect();
            if ( g_dev.vars.VBOLaneNum != lane_det ) {

                pr_err("Lane setting = %d lane(s), but detected = %d lane(s)", (int)(1<<g_dev.vars.VBOLaneNum), (int)(1<<lane_det) );
                if ( g_dev.vars.VBOLaneNum > lane_det ) {
                    g_dev.vars.VBOLaneNum = lane_det;
                }
            }
            #endif

            //it6265_avi_task_sys_unplug();

            it6265_set_avmute(1);
            it6265_video_state(Vid_Reset);
            it6265_audio_state(Aud_Reset);
            idle(500);
            it6265_video_state(Vid_WaitForVidIn);
            it6265_audio_state(Aud_WaitForAudIn);

        }
    }
}
void It6265::it6265_gate_rclk(u8 gating)
{
    if (gating) {
        //hdmitxset(0x25, 0x01, 0x01);
    } else {
        hdmitxset(0x25, 0x01, 0x00);
    }
}

void It6265::it6265_power_on( void )
{
     hdmitxset(0x25, 0x01, 0x00);

     #if ENABLE_CEC
     if ( g_dev.vars.rid >= 0xA1 ) {
         hdmitxwr(0xFC, ((CECAddr << 1)|0x01));
         cecset(0x0D, 0x10, 0x10);
     }
     #endif

     hdmitxset(0x25, 0x0E, 0x00);

     hdmitxset(0x86, 0x08, 0x08); // XP_ALPWDB=1

     hdmitxset(0x84, 0x60, 0x00); // XP_PWDI=0, XP_PWDPLL=0
     hdmitxset(0x81, 0x80, 0x00); // IP_PWDI=0

     hdmitxset(0x84, 0x80, 0x00); // XP_RESETB=0
     hdmitxset(0x80, 0x80, 0x00); // IP_RESETB=0

     hdmitxset(0x88, 0x02, 0x00); // DRV_PWD=0

     hdmitxset(0x84, 0x80, 0x80); // XP_RESETB=1
     hdmitxset(0x80, 0x80, 0x80); // IP_RESETB=1



     vborxset(0xB0, 0x30, 0x30);
     vborxset(0xB1, 0x30, 0x30);
     vborxset(0xB5, 0x01, 0x01);
     vborxset(0xB6, 0x03, 0x03);
     vborxset(0xB7, 0x03, 0x03);
     vborxset(0x8F, 0xE0, 0xE0);

     if( g_dev.cfg.EnDESSC==1 || g_dev.cfg.CKSOURCE==1 ) {
         vborxset(0xB8, 0x40, 0x00);
     }


     if ( g_dev.vars.rid < 0xA1 ) {
         hdmitxset(0x0C, 0x01, 0x00);
     }
     hdmitxwr(0xFF, 0xFF);

     //GRCLKPD = FALSE;

    pr_info("Power On HDMITX \r\n");
}

void It6265::it6265_power_down( void )
{
     hdmitxset(0x25, 0x01, 0x00);

     #if ENABLE_CEC
     if ( g_dev.vars.rid >= 0xA1 ) {
         hdmitxwr(0xFC, ((CECAddr << 1)|0x01));
         cecset(0x0D, 0x10, 0x00);
         hdmitxwr(0xFC, ((CECAddr << 1)&0xFE));
     }
     #endif

     if ( g_dev.cfg.keep_vborx_connection == 0 )
     {
         vborxset(0xB0, 0x30, 0x00);
         vborxset(0xB1, 0x30, 0x00);
         vborxset(0xB5, 0x01, 0x00);
         vborxset(0xB6, 0x03, 0x00);
         vborxset(0xB7, 0x03, 0x00);
         vborxset(0x8F, 0xE0, 0x00);
     }

     if( g_dev.cfg.EnDESSC==false || g_dev.cfg.CKSOURCE==0 ) {
         vborxset(0xB8, 0x40, 0x40);
     }

     hdmitxset(0x88, 0x01, 0x01);

     if ( g_dev.cfg.keep_vborx_connection == 0 )
     {
        hdmitxset(0x84, 0x80, 0x00);
        hdmitxset(0x80, 0x80, 0x00);
     }

     idle(1);

     hdmitxset(0x88, 0x02, 0x02);
     hdmitxset(0x84, 0x60, 0x60);
     hdmitxset(0x81, 0x80, 0x80);

     hdmitxset(0x25, 0x0E, 0x0E);
     hdmitxset(0x26, 0x01, 0x01);
     hdmitxset(0xFC, 0x01, 0x00);

     hdmitxwr(0xFF, 0xC3);
     hdmitxwr(0xFF, 0xA5);

     if( (hdmitxrd(0x22)&0x80)==0x00 ) {
         if ( g_dev.vars.rid < 0xA1 ) {
             hdmitxset(0x0C, 0x01, 0x01);
         }
         hdmitxset(0x86, 0x08, 0x00);
     }

     //hdmitxset(0x25, 0x01, 0x01);
     it6265_gate_rclk(1);

     //GRCLKPD = TRUE;

     pr_info("Power Down HDMITX \r\n");
}

bool It6265::it6265_is_valid_rclk( u32 rclk )
{
    #define RCLK_DIFF  25UL
    #define RCLK_MAX  (RCLK_TYPICAL*(100UL+RCLK_DIFF)/100UL)
    #define RCLK_MIN  (RCLK_TYPICAL*(100UL-RCLK_DIFF)/100UL)

    if ( (rclk <= RCLK_MAX) && (rclk >=RCLK_MIN) ) {
        return true;
    }
    pr_err("RCLK=%u, max=%u, min=%u, avg=%u, range=%u%%\r\n", rclk, (u32)RCLK_MAX, (u32)RCLK_MIN, (u32)RCLK_TYPICAL, (u32)RCLK_DIFF);
    return false;
}

void It6265::it6265_set_calib_data( u32 rclk )
{
    u8 tmp;

    pr_info2("Set RCLK = %u KHz\r\n", rclk);

    #if ENABLE_CEC
    g_dev.vars.cec_time_unit = (rclk * 100 / 16)/1000;
    pr_info2("Set CEC time unit = 0x%02X\r\n", (int)g_dev.vars.cec_time_unit);
    #endif

    rclk = 11 * rclk;

    tmp = rclk & 0xFF;
    hdmitxwr(0x67, tmp);

    rclk >>= 8;
    tmp = rclk & 0xFF;
    hdmitxwr(0x68, tmp);

    rclk >>= 8;
    tmp = rclk & 0x03;
    hdmitxset(0x69, 0x03, tmp);

    pr_info2("Set RCLK 0x67 = %02X %02X %02X\r\n", (int)hdmitxrd(0x67), (int)hdmitxrd(0x68), (int)hdmitxrd(0x69));
}

void It6265::it6265_load_rclk( void )
{
    u32 sum;
    u8 sip_data[4];

    if ( g_dev.vars.RCLK == 0 )
    {
        chgtxbank(2);
        hdmitxwr(0x5f, 0x04);
        hdmitxwr(0x5f, 0x05);
        hdmitxwr(0x58, 0x12);
        hdmitxwr(0x58, 0x02);
        hdmitxwr(0x57, 0x01);
        hdmitxwr(0x50, 0x00);
        hdmitxwr(0x50, 0x00);
        hdmitxwr(0x51, 0xb0);
        hdmitxwr(0x54, 0x04);

        hdmitxbrd(0x61,4,sip_data);

        hdmitxset(0x5f, 0x05, 0x00);

        chgtxbank(0);

        if ( (sip_data[0] == 0x00) && (sip_data[1] == 0x00) && (sip_data[2] == 0xff) )
        {
            pr_info2("data = %02x %02x %02x %02x\r\n", (int)sip_data[0], (int)sip_data[1], (int)sip_data[2], (int)sip_data[3]);
        }
        else
        {
            sum = sip_data[2];
            sum <<=8;
            sum |= sip_data[1];
            sum <<=8;
            sum |= sip_data[0];

            g_dev.vars.RCLK = (sum * 16) / 100;
            pr_info("g_dev.vars.RCLK = (sum * 16) / 100 = %u \r\n", g_dev.vars.RCLK);

            if ( false == it6265_is_valid_rclk(g_dev.vars.RCLK) )
            {
                pr_info("g_dev.vars.RCLK = 0\r\n");
                g_dev.vars.RCLK = 0;
            }
        }
    }

    pr_info2("load RCLK=%u\r\n", g_dev.vars.RCLK);
    //g_dev.vars.RCLK = 0;
}

void It6265::it6265_cal_rclk( void )
{
    u32 rddata, i;
    u32 sum;

    hdmitxwr(0xFC, ((CECAddr << 1) | 0x01));    // Enable CRCLK

    sum = 0;
    for (i = 0; i < 5; i++) {

        cecwr(0x09, 0x01);
        idle(99);
        cecwr(0x09, 0x00);

        rddata = cecrd(0x45);
        rddata += (cecrd(0x46) << 8);
        rddata += (cecrd(0x47) << 16);

        sum += rddata;
    }
    sum /= 5;

    cecset(0x0d, 0x10, 0x00);      // Disable CEC_IOPU
    hdmitxwr(0xFC, ((CECAddr << 1) & 0xFE));

    g_dev.vars.RCLK = (sum * 16) / 100;

    pr_info2("cal RCLK=%u\r\n", g_dev.vars.RCLK);

    if ( false == it6265_is_valid_rclk(g_dev.vars.RCLK)  ) {
        g_dev.vars.RCLK = 0;
    }
}

void It6265::it6265_cal_vclk( void )
{
    //calculate VCLK
    if (g_dev.cfg.OutColorMode == YCbCr422) {
        g_dev.vars.VCLK = g_dev.vars.TXCLK * 2 * g_dev.vars.PixRpt * 1;
    } else {
        switch (g_dev.cfg.ColorDepth) {
        case VID10BIT:
            g_dev.vars.VCLK = g_dev.vars.TXCLK * 2 * g_dev.vars.PixRpt * 5 / 4;
            break;
        case VID12BIT:
            g_dev.vars.VCLK = g_dev.vars.TXCLK * 2 * g_dev.vars.PixRpt * 3 / 2;
            break;
        default:
            g_dev.vars.VCLK = g_dev.vars.TXCLK * 2 * g_dev.vars.PixRpt * 1;
            break;
        }
    }
}

void It6265::it6265_set_asclk(void)
{
    if ( (g_dev.cfg.AudSel==SPDIF) && (g_dev.vars.VCLK > 250000UL))
    {
        hdmitxset(0x21, 0xF8, 0x00);
        hdmitxwr(0x23, 0x03);
    }
    else
    {
        hdmitxset(0x21, 0xF8, 0xC8);
        hdmitxwr(0x23, 0x00);
    }
}

void It6265::it6265_assign_txclk( u32 PixelClock )
{
    #if CHECK_MAX_TMDS_CLOCK
    u32 max_tmds_clk;
    #endif

    if (g_dev.vars.RegPCLKDiv2)
        g_dev.vars.TXCLK = PixelClock;
    else
        g_dev.vars.TXCLK = PixelClock/2;

    #if CHECK_MAX_TMDS_CLOCK
    // calculate VCLK and check max TMDS from EDID:
    max_tmds_clk = g_dev.sink.max_TMDS_clk*5*1024;
    pr_info2("max_tmds_clk from EDID=%u\r\n", max_tmds_clk);
    for (;;) {

        it6265_cal_vclk();

        if ( g_dev.cfg.ColorDepth == VID8BIT ) {
            break;
        }

        // check max TMDS from EDID:
        if ( max_tmds_clk )  {
            if ( g_dev.vars.VCLK > max_tmds_clk ) {
                switch ( g_dev.cfg.ColorDepth ) {
                case VID12BIT:
                    g_dev.cfg.ColorDepth = VID10BIT;
                    pr_err("ERROR: Exceed EDID maximum TMDS clock !!! => Change 36bit to 30-bit mode ...\r\n");
                    break;
                case VID10BIT:
                    g_dev.cfg.ColorDepth = VID8BIT;
                    pr_err("ERROR: Exceed EDID maximum TMDS clock !!! => Change 30bit to 24-bit mode ...\r\n");
                    break;
                }
            }
            else {
                // g_dev.vars.VCLK <= max_tmds_clk
                // no need to change color depth
                break;
            }
        }
        else {
            pr_err("ERROR: Unknown EDID maximum TMDS clock !!! => Change to 24-bit mode\r\n");
            g_dev.cfg.ColorDepth = VID8BIT;
        }
    }
    #else

    g_dev.cfg.ColorDepth = OUTPUT_COLOR_DEPTH;
    it6265_cal_vclk();

    pr_info2("force output color depth=%d\r\n", g_dev.cfg.ColorDepth);

    #endif


    if (g_dev.cfg.InColorMode == YCbCr420 && g_dev.cfg.OutColorMode != YCbCr420)
        g_dev.vars.PCLK = g_dev.vars.TXCLK / 2;
    else
        g_dev.vars.PCLK = g_dev.vars.TXCLK;

    g_dev.vars.ICLK = g_dev.vars.PCLK;

    pr_info("\r\nCount TXCLK=%u\r\n", g_dev.vars.TXCLK);
}

void It6265::it6265_cal_txclk( void )
{
    u32 rddata;
    u32 txclk;
    u8 rclk_freq;
    int predivsel, i;
    u32 sum;
    u8 regB, regE;

    hdmitxset(0x0B, 0x80, 0x80);
    idle(1);
    rclk_freq = hdmitxrd(0x21) & 0x03;
    hdmitxset(0x0B, 0x80, 0x00);

    rddata = hdmitxrd(0x0A);
    rddata += ((hdmitxrd(0x0B) & 0x0F) << 8);

    if (rclk_freq == 0) {
        rddata *= 2;
    }

    if (rddata < 16)
        predivsel = 7;
    else if (rddata < 32)
        predivsel = 6;
    else if (rddata < 64)
        predivsel = 5;
    else if (rddata < 128)
        predivsel = 4;
    else if (rddata < 256)
        predivsel = 3;
    else if (rddata < 512)
        predivsel = 2;
    else if (rddata < 1024)
        predivsel = 1;
    else
        predivsel = 0;

    pr_info2("predivsel=%d\r\n", (int)predivsel);

    sum = 0;

    hdmitxset(0x0B, 0xF0, (predivsel << 4));
    regE = hdmitxrd(0x0B);
    regB = hdmitxrd(0x0B)|0x80;

    for (i = 0; i < 10; i++) {
        //hdmitxset(0x0B, 0xF0, (0x80 + (predivsel << 4)));
        //idle(50);
        //hdmitxset(0x0B, 0xF0, (predivsel << 4));

        hdmitxwr(0x0B, regB);
        idle(10);
        hdmitxwr(0x0B, regE);
        idle(4);
        //rddata = hdmitxrd(0x0A);
        //rddata += ((hdmitxrd(0x0B) & 0x0F) << 8);

        //rddata = hdmitxrd(0x0B) & 0x0F;
        //rddata <<= 8;
        //rddata |= hdmitxrd(0x0A);

        rddata = ((hdmitxrd(0x0B) & 0x0F)<<8) | hdmitxrd(0x0A);

        if (rclk_freq == 0) {
            rddata *= 2;
        }

        sum += rddata;
    }

    sum /= (10* (1 << predivsel));

    pr_info2("predivsel=%d, sum=%u\r\n", predivsel, sum);

    txclk = g_dev.vars.RCLK * 2048 / sum ;

    it6265_assign_txclk(txclk*2);
}

void It6265::it6265_ddc_reset(void)
{
    // DDC Master Reset
    hdmitxset(0x35, 0x10, 0x10);
    hdmitxset(0x35, 0x10, 0x00);

    // Generate SCL Clock
    // to recover the SCL/SDA stop
    hdmitxset(0x28, 0x01, 0x01);
    hdmitxwr(0x2E, 0x0A);
    idle(5);
    hdmitxwr(0x2E, 0x0F);
    idle(10);
    hdmitxset(0x28, 0x01, 0x00);
}

void It6265::it6265_set_pattern_gen(u16 vic)
{
    const VTiming *vt;
    VTiming SetVTiming;
    VTiming *pSetVTiming;

    int HTotal, HDES, HRS, HRE, VTotal, VDES, VRS, VRE, HPol, VPol, Prog;
    int HDEW, VDEW, HFP, HSW, HBP, VFP, VSW, VBP, HDEE, VDEE;
    int VRS2nd, VRE2nd, VDES2nd, VDEE2nd, H2ndVRRise;

    vt = it6265_find_timing_by_vic(vic);
    SetVTiming = *vt;
    pSetVTiming = &SetVTiming;

    pr_info(">> it6265_set_pattern_gen, %u\r\n", vic );

    if ( NULL == vt ) {
        pr_err(">> set_syncde_gen, VIC=%u not found\r\n", vic);
        return;
    }

    if (g_dev.cfg.InColorMode == YCbCr420)
        pSetVTiming->PCLK /= 2;

    if( g_dev.cfg.En3D && (g_dev.cfg.Sel3DFmt==FrmPkt||g_dev.cfg.Sel3DFmt==SbSFull) )
        pSetVTiming->PCLK *= 2;

    HTotal = pSetVTiming->HTotal-1;
    HDES = pSetVTiming->HFrontPorch+pSetVTiming->HSyncWidth+pSetVTiming->HBackPorch-1;
    HRS = pSetVTiming->HFrontPorch-1;
    HRE = pSetVTiming->HFrontPorch+pSetVTiming->HSyncWidth-1;

    VDES = pSetVTiming->VFrontPorch+pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
    VDEE = pSetVTiming->VTotal;
    VRS = pSetVTiming->VFrontPorch;
    VRE = pSetVTiming->VFrontPorch+pSetVTiming->VSyncWidth;

    HPol = pSetVTiming->HPolarity;
    VPol = pSetVTiming->VPolarity;

    Prog = pSetVTiming->ScanMode;

    if( !Prog ) {
        if( vic==39 ) {
            VDES2nd = VDES+pSetVTiming->VTotal;
            VDEE2nd = VDEE+pSetVTiming->VTotal;
            VRS2nd = VRS+pSetVTiming->VTotal;
            VRE2nd = VRE+pSetVTiming->VTotal;
            H2ndVRRise = HRS+pSetVTiming->HTotal/2;
            VTotal = pSetVTiming->VTotal*2-1;
        }
        else {
            VDES2nd = VDES+pSetVTiming->VTotal+1;
            VDEE2nd = VDEE+pSetVTiming->VTotal+1;
            VRS2nd = VRS+pSetVTiming->VTotal+1;
            VRE2nd = VRE+pSetVTiming->VTotal+1;
            H2ndVRRise = HRS+pSetVTiming->HTotal/2;
            VTotal = pSetVTiming->VTotal*2;
        }
    }
    else {
        VDES2nd = 0xFFF;
        VDEE2nd = 0xFFF;
        VRS2nd = 0xFFF;
        VRE2nd = 0xFF;
        H2ndVRRise = 0xFFF;
        VTotal = pSetVTiming->VTotal-1;
    }
/*
    if( abs(pSetVTiming->PCLK-27000)<(27000*0.05) )
        IntPCLKSel = CLK13P5M;
    else if( abs(pSetVTiming->PCLK-54000)<(54000*0.05) )
        IntPCLKSel = CLK27M;
    else if( abs(pSetVTiming->PCLK-108000)<(108000*0.05) )
        IntPCLKSel = CLK54M;
    else if( abs(pSetVTiming->PCLK-216000)<(216000*0.05) )
        IntPCLKSel = CLK108M;
    else if( abs(pSetVTiming->PCLK-74250)<(74250*0.05) )
        IntPCLKSel = CLK37P125M;
    else if( abs(pSetVTiming->PCLK-148500)<(148500*0.05) )
        IntPCLKSel = CLK74P25M; //CLK74P25M;
    else if( abs(pSetVTiming->PCLK-297000)<(297000*0.05) )
        IntPCLKSel = CLK148P5M;
    else if( abs(pSetVTiming->PCLK-594000)<(594000*0.05) )
        IntPCLKSel = CLK297M;
    else {
        if( ExtPCLK==FALSE ) {
            printf("ERROR: No valid internal clock for this video format !!!\n");
            printf("Press any key to continue ...\n");
            getch();
        }

        IntPCLKSel = CLK74P25M;
    }
*/

    if( g_dev.cfg.EnIntPG ) {
        if( g_dev.cfg.En3D && g_dev.cfg.Sel3DFmt==SbSFull ) {
            HTotal = (pSetVTiming->HTotal)*2;
            HDEW = (pSetVTiming->HActive)*2;
            HDES = (pSetVTiming->HSyncWidth+pSetVTiming->HBackPorch)*2;
            HFP = (pSetVTiming->HFrontPorch)*2;
            HSW = (pSetVTiming->HSyncWidth)*2;
            HBP = (pSetVTiming->HBackPorch)*2;
        }
        else {
            HTotal = pSetVTiming->HTotal;
            HDEW = pSetVTiming->HActive;
            HDES = pSetVTiming->HSyncWidth+pSetVTiming->HBackPorch;
            HFP = pSetVTiming->HFrontPorch;
            HSW = pSetVTiming->HSyncWidth;
            HBP = pSetVTiming->HBackPorch;
        }

        if( g_dev.cfg.En3D && g_dev.cfg.Sel3DFmt==FrmPkt ) {
            if( pSetVTiming->ScanMode ) {
                VTotal = pSetVTiming->VTotal*2;
                VDEW = pSetVTiming->VActive+pSetVTiming->VTotal;
                VDES = pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
                VFP = pSetVTiming->VFrontPorch;
                VSW = pSetVTiming->VSyncWidth;
                VBP = pSetVTiming->VBackPorch;
            }
            else {
                VTotal = pSetVTiming->VTotal*4+2;
                VDEW = pSetVTiming->VActive*4+(pSetVTiming->VBackPorch+pSetVTiming->VFrontPorch+pSetVTiming->VSyncWidth)*3+2;
                VDES = pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
                VFP = pSetVTiming->VFrontPorch;
                VSW = pSetVTiming->VSyncWidth;
                VBP = pSetVTiming->VBackPorch;
            }
            Prog = PROG;
        }
        else {
            VTotal = pSetVTiming->VTotal;
            VDEW = pSetVTiming->VActive;
            VDES = pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
            VFP = pSetVTiming->VFrontPorch;
            VSW = pSetVTiming->VSyncWidth;
            VBP = pSetVTiming->VBackPorch;

            Prog = pSetVTiming->ScanMode;
        }

        HPol = (pSetVTiming->HPolarity)&0x01;
        VPol = (pSetVTiming->VPolarity)&0x01;

        HRS = HFP-1;
        HRE = HRS+HSW;
        HDES = HRE+HBP;
        HDEE = HDES+HDEW;

        VRS = VFP;
        VRE = VRS+VSW;
        VDES = VRE+VBP-1;
        VDEE = VDES+VDEW;

        if( !Prog ) {
            if( vic==39 ) {
                VDES2nd = VTotal+VDES;
                VDEE2nd = VDES2nd+VDEW;
                VRS2nd = VTotal+VRS-1;
                VRE2nd = VTotal+VRE-1;
                H2ndVRRise = HRS+HTotal/2;
                VTotal = VTotal*2-1;
            }
            else {
                VDES2nd = VTotal+VDES+1;
                VDEE2nd = VDES2nd+VDEW;
                VRS2nd = VTotal+VRS;
                VRE2nd = VTotal+VRE;
                H2ndVRRise = HRS+HTotal/2;
                VTotal = VTotal*2;
            }
        }
        else {
            VDES2nd = 0xFFF;
            VDEE2nd = 0xFFF;
            VRS2nd = 0xFFF;
            VRE2nd = 0xFF;
            H2ndVRRise = 0xFFF;
            VTotal = VTotal-1;
        }
        HTotal = HTotal-1;
/*
        // temp for ATC HF1-15
        if( HTotal==5503 ) {
            HTotal -= 4;
            HRS -= 4;
            HRE -= 4;
            HDES -= 4;
            HDEE -= 4;
        }
*/
        chgtxbank(2);
        // HTotal
        hdmitxwr(0x10, HTotal&0xFF);
        hdmitxwr(0x11, (HTotal&0x3F00)>>8);
        // HDES/HDEE
        hdmitxwr(0x12, HDES&0xFF);
        hdmitxwr(0x13, (HDES&0x3F00)>>8);
        hdmitxwr(0x14, HDEE&0xFF);
        hdmitxwr(0x15, (HDEE&0x3F00)>>8);
        // HRS/HRE
        hdmitxwr(0x16, HRS&0xFF);
        hdmitxwr(0x17, (HRS&0x3F00)>>8);
        hdmitxwr(0x18, HRE&0xFF);
        hdmitxwr(0x19, (HRE&0x3F00)>>8);
        // VTotal
        hdmitxwr(0x1A, VTotal&0xFF);
        hdmitxwr(0x1B, (VTotal&0x1F00)>>8);
        // VDES/VDEE
        hdmitxwr(0x1C, VDES&0xFF);
        hdmitxwr(0x1D, (VDES&0x1F00)>>8);
        hdmitxwr(0x1E, VDEE&0xFF);
        hdmitxwr(0x1F, (VDEE&0x1F00)>>8);
        // VDES2nd/VDEE2nd
        hdmitxwr(0x20, VDES2nd&0xFF);
        hdmitxwr(0x21, (VDES2nd&0x1F00)>>8);
        hdmitxwr(0x22, VDEE2nd&0xFF);
        hdmitxwr(0x23, (VDEE2nd&0x1F00)>>8);
        // VRS/VRE
        hdmitxwr(0x24, VRS&0xFF);
        hdmitxwr(0x25, (VRS&0x1F00)>>8);
        hdmitxwr(0x26, VRE&0xFF);
        // VRS2nd/VRE2nd
        hdmitxwr(0x28, VRS2nd&0xFF);
        hdmitxwr(0x29, (VRS2nd&0x1F00)>>8);
        hdmitxwr(0x2A, VRE2nd&0xFF);
        // H2ndVRRise
        hdmitxwr(0x2C, H2ndVRRise&0xFF);
        hdmitxwr(0x2D, (H2ndVRRise&0x3F00)>>8);
        // Sync Polarity
        hdmitxset(0x2E, 0x07, ((!Prog)<<2)+(VPol<<1)+HPol);


        hdmitxwr(0x30, 0x00);
        hdmitxwr(0x31, 0x00);
        hdmitxwr(0x32, 0x80);
        hdmitxwr(0x33, 0x80);
        hdmitxwr(0x34, 0x80);
        hdmitxwr(0x35, 0x10);
        hdmitxwr(0x36, 0x10);
        hdmitxwr(0x37, 0x20);
        hdmitxwr(0x38, 0x00);

        // setting for VBORX PatGen
        chgrxbank(1);
        // HTotal
        vborxwr(0x30, HTotal&0xFF);
        vborxwr(0x31, (HTotal&0x3F00)>>8);
        // HDES/HDEE
        vborxwr(0x32, HDES&0xFF);
        vborxwr(0x33, (HDES&0x3F00)>>8);
        vborxwr(0x34, HDEE&0xFF);
        vborxwr(0x35, (HDEE&0x3F00)>>8);
        // HRS/HRE
        vborxwr(0x36, HRS&0xFF);
        vborxwr(0x37, (HRS&0x3F00)>>8);
        vborxwr(0x38, HRE&0xFF);
        vborxwr(0x39, (HRE&0x3F00)>>8);
        // VTotal
        vborxwr(0x3A, VTotal&0xFF);
        vborxwr(0x3B, (VTotal&0x1F00)>>8);
        // VDES/VDEE
        vborxwr(0x3C, VDES&0xFF);
        vborxwr(0x3D, (VDES&0x1F00)>>8);
        vborxwr(0x3E, VDEE&0xFF);
        vborxwr(0x3F, (VDEE&0x1F00)>>8);
        // VDES2nd/VDEE2nd
        vborxwr(0x40, VDES2nd&0xFF);
        vborxwr(0x41, (VDES2nd&0x1F00)>>8);
        vborxwr(0x42, VDEE2nd&0xFF);
        vborxwr(0x43, (VDEE2nd&0x1F00)>>8);
        // VRS/VRE
        vborxwr(0x44, VRS&0xFF);
        vborxwr(0x45, (VRS&0x1F00)>>8);
        vborxwr(0x46, VRE&0xFF);
        // VRS2nd/VRE2nd
        vborxwr(0x48, VRS2nd&0xFF);
        vborxwr(0x49, (VRS2nd&0x1F00)>>8);
        vborxwr(0x4A, VRE2nd&0xFF);
        // H2ndVRRise
        vborxwr(0x4C, H2ndVRRise&0xFF);
        vborxwr(0x4D, (H2ndVRRise&0x3F00)>>8);
        // Sync Polarity
        vborxset(0x4E, 0x07, ((!Prog)<<2)+(VPol<<1)+HPol);
        chgrxbank(0);
    }
    //hdmitxset(0x2F, 0x0B, (EnDEOnly<<3)+(EnPatMux<<1)+g_dev.cfg.EnIntPG);
    hdmitxset(0x2F, 0x0B, (0<<3)+(1<<1)+g_dev.cfg.EnIntPG);
    chgtxbank(0);
}

void It6265::it6265_set_syncde_gen(u16 vic)
{
#if SUPPORT_DEONLY
    const VTiming *vt;
    VTiming SetVTiming;
    VTiming *pSetVTiming;
    int HTotal, VTotal, HPol, VPol;
    int HFP, HSW, VFP, VSW;
    //int HDEW, VDEW, HDES, VDES;
    int HRS, HRE;
    int VRS, VRE;
    int H2ndVRRise;
    int VRS2nd, VRE2nd;
    int tmpi;
    u8 Reg2x656Clk = 0;
    u8 EnSavVSync = 0;
    u8 tmp;

    vt = it6265_find_timing_by_vic(vic);
    SetVTiming = *vt;
    pSetVTiming = &SetVTiming;

    pr_info(">> set_syncde_gen, %u\r\n", vic );

    if ( NULL == vt ) {
        pr_err(">> set_syncde_gen, VIC=%u not found\r\n", vic);
        return;
    }

    if (g_dev.cfg.InColorMode == YCbCr420)
        pSetVTiming->PCLK /= 2;

    if( g_dev.cfg.En3D && (g_dev.cfg.Sel3DFmt==FrmPkt||g_dev.cfg.Sel3DFmt==SbSFull) )
        pSetVTiming->PCLK *= 2;

    chgtxbank(2);
    hdmitxset(0x2F, 0x0B, (g_dev.cfg.EnDEOnly<<3)+(0<<1)+0);
    chgtxbank(0);


     if( g_dev.cfg.En3D && g_dev.cfg.Sel3DFmt==SbSFull ) {
         HTotal = (pSetVTiming->HTotal)*2;
         //HDEW = (pSetVTiming->HActive)*2;
         //HDES = (pSetVTiming->HSyncWidth+pSetVTiming->HBackPorch)*2;
         HFP = (pSetVTiming->HFrontPorch)*2;
         HSW = (pSetVTiming->HSyncWidth)*2;
     }
     else {
         HTotal = pSetVTiming->HTotal;
         //HDEW = pSetVTiming->HActive;
         //HDES = pSetVTiming->HSyncWidth+pSetVTiming->HBackPorch;
         HFP = pSetVTiming->HFrontPorch;
         HSW = pSetVTiming->HSyncWidth;
     }

     if( g_dev.cfg.En3D && g_dev.cfg.Sel3DFmt==FrmPkt ) {
         if( pSetVTiming->ScanMode ) {
             VTotal = pSetVTiming->VTotal*2;
             //VDEW = pSetVTiming->VActive+pSetVTiming->VTotal;
             //VDES = pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
             VFP = pSetVTiming->VFrontPorch;
             VSW = pSetVTiming->VSyncWidth;
         }
         else {
             VTotal = pSetVTiming->VTotal*4+2;
             //VDEW = pSetVTiming->VActive*4+(pSetVTiming->VBackPorch+pSetVTiming->VFrontPorch+pSetVTiming->VSyncWidth)*3+2;
             //VDES = pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
             VFP = pSetVTiming->VFrontPorch;
             VSW = pSetVTiming->VSyncWidth;
         }
         pSetVTiming->ScanMode = PROG;
     }
     else {
         VTotal = pSetVTiming->VTotal;
         //VDEW = pSetVTiming->VActive;
         //VDES = pSetVTiming->VSyncWidth+pSetVTiming->VBackPorch;
         VFP = pSetVTiming->VFrontPorch;
         VSW = pSetVTiming->VSyncWidth;
     }

     HPol = (pSetVTiming->HPolarity)&0x01;
     VPol = (pSetVTiming->VPolarity)&0x01;

     // SyncEmb case
     if( !Reg2x656Clk )
         HRS = HFP - 2;
     else
         HRS = HFP - 1;

     // DEOnly case
     if( g_dev.cfg.EnDEOnly || g_dev.cfg.EnRxDEOnly)
         HRS = HFP - 1;

     HRE = HRS + HSW;
     H2ndVRRise = HRS+ HTotal/2;

     VRS = VFP;
     VRE = VRS + VSW;
     if( pSetVTiming->ScanMode ){ // progressive mode
         VRS2nd = 0xFFF;
         VRE2nd = 0x3F;
     }
     else { // interlaced mode
         if( g_dev.input.VIC ==39 ) {
             VRS2nd = VRS + VTotal - 1;
             VRE2nd = VRS2nd + VSW;
         }
         else {
             VRS2nd = VRS + VTotal;
             VRE2nd = VRS2nd + VSW;
         }
     }

     if( EnSavVSync ) {
         VRS -= 1;
         VRE -= 1;

         if( !pSetVTiming->ScanMode ) {
             VRS2nd -= 1;
             VRE2nd -= 1;
         }
      }

     chgtxbank(2);

     // HRS/HRE
     tmpi = HRS;
     tmp = tmpi&0xFF;
     hdmitxwr(0x16, tmp);

     tmpi>>=8;
     tmp = tmpi&0x3F;
     hdmitxwr(0x17, tmp);

     tmpi = HRE;
     tmp = tmpi&0xFF;
     hdmitxwr(0x18, tmp);

     tmpi>>=8;
     tmp = tmpi&0x3F;
     hdmitxwr(0x19, tmp);

     // VRS/VRE
     tmpi = VRS;
     tmp = tmpi&0xFF;
     hdmitxwr(0x24, tmp);

     tmpi>>=8;
     tmp = tmpi&0x1F;
     hdmitxwr(0x25, tmp);

     tmpi = VRE;
     tmp = tmpi&0xFF;
     hdmitxwr(0x26, tmp);

     tmpi>>=8;
     tmp = tmpi&0x1F;
     hdmitxwr(0x27, tmp);

     // VRS2nd/VRE2nd
     hdmitxwr(0x28, VRS2nd&0xFF);
     hdmitxwr(0x29, (VRS2nd&0x1F00)>>8);
     hdmitxwr(0x2A, VRE2nd&0xFF);
     hdmitxwr(0x2B, (VRE2nd&0x1F00)>>8);
     // H2ndVRRise
     hdmitxwr(0x2C, H2ndVRRise&0xFF);
     hdmitxwr(0x2D, (H2ndVRRise&0x3F00)>>8);
     // Sync Polarity and ScanMode
     hdmitxset(0x2E, 0x07, ((!pSetVTiming->ScanMode)<<2)+(VPol<<1)+HPol);
     // DEOnly
     hdmitxset(0x2F, 0x08, g_dev.cfg.EnDEOnly<<3);

     //hdmitxset(0x2F, 0x0B, (1<<3)+(1<<1)+1);

     if( g_dev.cfg.EnDEOnly ) {
         HTotal = HTotal - 1;

         // HTotal
         hdmitxwr(0x10, HTotal&0xFF);
         hdmitxwr(0x11, (HTotal&0x3F00)>>8);

         if( !pSetVTiming->ScanMode )
             VTotal *= 2;
         else
             VTotal = VTotal - 1;

         // VTotal
         hdmitxwr(0x1A, VTotal&0xFF);
         hdmitxwr(0x1B, (VTotal&0x1F00)>>8);
     }
     chgtxbank(0);

     if( g_dev.cfg.EnRxDEOnly ) {
         chgrxbank(1);
         HTotal = HTotal - 1;
         // HTotal
         vborxwr(0x30, HTotal&0xFF);
         vborxwr(0x31, (HTotal&0x3F00)>>8);
         // HRS/HRE
         vborxwr(0x36, HRS&0xFF);
         vborxwr(0x37, (HRS&0x3F00)>>8);
         vborxwr(0x38, HRE&0xFF);
         vborxwr(0x39, (HRE&0x3F00)>>8);
         // VRS/VRE
         vborxwr(0x44, VRS&0xFF);
         vborxwr(0x45, (VRS&0x1F00)>>8);
         vborxwr(0x46, VRE&0xFF);
         vborxwr(0x47, (VRE&0x1F00)>>8);
         // VRS2nd/VRE2nd
         vborxwr(0x48, VRS2nd&0xFF);
         vborxwr(0x49, (VRS2nd&0x1F00)>>8);
         vborxwr(0x4A, VRE2nd&0xFF);
         vborxwr(0x4B, (VRE2nd&0x1F00)>>8);
         // H2ndVRRise
         vborxwr(0x4C, H2ndVRRise&0xFF);
         vborxwr(0x4D, (H2ndVRRise&0x3F00)>>8);
         // Sync Polarity and ScanMode
         vborxset(0x4E, 0x07, ((!pSetVTiming->ScanMode)<<2)+(VPol<<1)+HPol);
         // DEOnly
         vborxset(0x4F, 0x08, g_dev.cfg.EnRxDEOnly<<3);

         if( !pSetVTiming->ScanMode )
             VTotal *= 2;
         else
             VTotal = VTotal - 1;

         // VTotal
         vborxwr(0x3A, VTotal&0xFF);
         vborxwr(0x3B, (VTotal&0x1F00)>>8);

         chgrxbank(0);
     }
#else
    vic = vic;
#endif
}

void It6265::it6265_setup_vborx( void )
{
    u32 CR_CLK, Bnd, temp;

    vborxset(0x0B, 0xF0, (g_dev.vars.VBOByteMode << 6) + (g_dev.vars.VBOLaneNum << 4));

    g_dev.vars.lanenum = 1<<g_dev.vars.VBOLaneNum;
    g_dev.vars.bytenum = g_dev.vars.VBOByteMode + 3;

    CR_CLK = 100000UL * g_dev.vars.bytenum / g_dev.vars.lanenum / 4;

    pr_info2("Auto OPDIV2Bnd = 100MHz PixClk, CR_CLK = %u kHz, ", CR_CLK);
    Bnd = ((g_dev.vars.RCLK * 128) + (CR_CLK - 1)) / CR_CLK;

    pr_info2("Calculated OPDIV2Bnd = 0x%02X\r\n", (int)Bnd);
    vborxwr(0x27, Bnd);

    if (g_dev.vars.bytenum == 5)
        temp = 1000000UL;
    else
        temp = 800000UL;

    CR_CLK = temp / 40;
    pr_info2("Auto OPPREDBnd = %dMHz BitRate, CR_CLK = %dMHz, ", (int)temp / 1000, (int)CR_CLK / 1000);
    Bnd = ((g_dev.vars.RCLK * 128) + (CR_CLK - 1)) / CR_CLK;
    pr_info2("Calculated OPPREDBnd = 0x%02X\r\n", (int)Bnd);
    vborxwr(0x28, Bnd);

    temp = 1000000;
    CR_CLK = temp / 40;
    pr_info2("Auto CSELBnd = %dMHz BitRate, CR_CLK  = %dMHz, ", (int)temp / 1000, (int)CR_CLK / 1000);
    Bnd = ((g_dev.vars.RCLK * 128) + (CR_CLK - 1)) / CR_CLK;
    pr_info2("Calculated CSELBnd = 0x%02X\r\n", (int)Bnd);
    vborxwr(0x29, Bnd);

    temp = 1600000;
    CR_CLK = temp / 40;
    pr_info2("Auto CPPRED = %dMHz BitRate, CR_CLK  = %dMHz, ", (int)temp / 1000, (int)CR_CLK / 1000);
    Bnd = ((g_dev.vars.RCLK * 128) + (CR_CLK - 1)) / CR_CLK;
    pr_info2("Calculated CPPRED = 0x%02X\r\n", (int)Bnd);
    vborxwr(0x2A, Bnd);
}

void It6265::it6265_setup_hdmitx_afe( void )
{
    u8 DRV_TERMON, DRV_RTERM, DRV_ISW, DRV_ISWC, DRV_TPRE, DRV_NOPE;
    u8 tmp;

    pr_info("it6265_setup_hdmitx_afe()\r\n");
    if ( g_dev.vars.ICLK > ((u32)80000) ) {  // ICLKIN > 80MHz
        hdmitxset(0x81, 0x07, 0x04);

        if (g_dev.vars.AFESPEED == LOW) {
            pr_info2("Change TX AFE setting to High Speed mode ...\r\n");

            g_dev.vars.AFESPEED = HIGH;
        }
    } else {
        hdmitxset(0x81, 0x07, 0x03);

        if (g_dev.vars.AFESPEED == HIGH) {
            pr_info2("Change TX AFE setting to Low Speed mode ...\r\n");

            g_dev.vars.AFESPEED = LOW;
        }
    }

    if ( g_dev.vars.VCLK > ((u32)100000) ) // IP_VCLK05 > 50MHz
        hdmitxset(0x84, 0x07, 0x04);
    else
        hdmitxset(0x84, 0x07, 0x03);

    if ( g_dev.vars.VCLK > ((u32)162000) ) // IP_VCLK05 > 81MHz
        hdmitxset(0x88, 0x04, 0x04);
    else
        hdmitxset(0x88, 0x04, 0x00);

    if ( g_dev.vars.VCLK > ((u32)340000) ) {
        hdmitxset(0x83, 0x04, 0x04);
    }
    else{
        hdmitxset(0x83, 0x04, 0x00);
    }

    if ( g_dev.vars.VCLK > ((u32)375000) ) {
        DRV_TERMON = 1;
        DRV_RTERM = 0x6;
        DRV_ISW = 0x1B;
        DRV_ISWC = 0x0B;
        DRV_TPRE = 0x0;
        DRV_NOPE = 0;
    } else if ( g_dev.vars.VCLK > ((u32)340000) ) {
        if ( g_dev.vars.rid >= 0xA1 ) {
            DRV_TERMON = 1;
            DRV_RTERM = 0x6;
            DRV_ISW = 0x0C;
            DRV_ISWC = 0x0B;
            DRV_TPRE = 0x0;
            DRV_NOPE = 1;
        }
        else {
            DRV_TERMON = 1;
            DRV_RTERM = 0x6;
            DRV_ISW = 0x1A;
            DRV_ISWC = 0x0B;
            DRV_TPRE = 0x0;
            DRV_NOPE = 0;
        }
    } else if ( g_dev.vars.VCLK > ((u32)150000) ) {
        DRV_TERMON = 1;
        DRV_RTERM = 0x1;
        DRV_ISW = 0x7;
        DRV_ISWC = 0x7;
        DRV_TPRE = 0x0;
        DRV_NOPE = 1;
    } else {
        DRV_TERMON = 0;
        DRV_RTERM = 0x0;
        DRV_ISW = 0x3;
        DRV_ISWC = 0x3;
        DRV_TPRE = 0;
        DRV_NOPE = 1;
    }

    if ( g_dev.cfg.EnHDMI == 0 ) //DVI mode
    {
        DRV_TERMON=1;
    }

    hdmitxset(0x87, 0x1F, DRV_ISW);

    tmp = (DRV_NOPE << 7) | (DRV_TERMON << 5) | DRV_RTERM;
    hdmitxset(0x89, 0xBF, tmp);

    tmp = DRV_ISWC & 0x0F;
    tmp <<=4;
    tmp = tmp | DRV_TPRE;
    hdmitxwr(0x8A, tmp);

    tmp = DRV_ISWC & 0x10;
    tmp <<=1;
    hdmitxset(0x8C, 0x20, tmp);
}

void It6265::it6265_set_infoframe( void )
{
    u8 chksum;
    int i;
    u8 AVIQ, AVIYQ;
    u8 tmp;
    u8 NewAVIPR = 2;

    if (g_dev.cfg.ColorClip == false && g_dev.cfg.OutColorMode == RGB444 && g_dev.cfg.DynRange == DynVESA ) {
        if (g_dev.sink.RGBQCap == false) {
            pr_info2("AVIQ is disabled because EDID do not support AVI RGB Selectable Quantization Ragne !!!\r\n");
            AVIQ = 0;
        } else {
            AVIQ = 2;
        }
    } else {
        AVIQ = 0;
        //g_dev.cfg.DynRange = DynCEA;
    }

    if (g_dev.cfg.ColorClip == false && g_dev.cfg.OutColorMode != RGB444 && g_dev.cfg.DynRange == DynVESA ) {
        if (g_dev.sink.YCCQCap == true) {
            pr_info2("AVIYQ is disabled because EDID do not support AVI YCC Selectable Quantization Ragne !!!\r\n");
            AVIYQ = 1;
        } else {
            AVIYQ = 0;
            //g_dev.cfg.DynRange = DynCEA;
        }
    } else {
        AVIYQ = 0;
    }

    if ( g_dev.cfg.EnExtCol && g_dev.cfg.ChkExtCol )
    {
        if ((g_dev.cfg.ExtColFmt == xvYCC601    && (g_dev.sink.ExtColCap & 0x01) == 0x00) ||
            (g_dev.cfg.ExtColFmt == xvYCC709    && (g_dev.sink.ExtColCap & 0x02) == 0x00) ||
            (g_dev.cfg.ExtColFmt == sYCC601     && (g_dev.sink.ExtColCap & 0x04) == 0x00) ||
            (g_dev.cfg.ExtColFmt == AdobeYCC601 && (g_dev.sink.ExtColCap & 0x08) == 0x00) ||
            (g_dev.cfg.ExtColFmt == AdobeRGB    && (g_dev.sink.ExtColCap & 0x10) == 0x00) ||
            (g_dev.cfg.ExtColFmt == BT2020cYCC  && (g_dev.sink.ExtColCap & 0x20) == 0x00) ||

            (g_dev.cfg.ExtColFmt == BT2020YCC   && (((g_dev.sink.ExtColCap & 0x40) == 0x00 &&
             g_dev.cfg.OutColorMode != RGB444) || ((g_dev.sink.ExtColCap & 0x80) == 0x00 &&
             g_dev.cfg.OutColorMode == RGB444)))
           )
        {
            g_dev.cfg.EnExtCol = false;
            pr_info2("\r\nExtended Colorimetry is disabled because EDID do not support !!!\r\n\r\n");
        }
    }

    // AVI InfoFrame
    chgtxbank(1);

    AVIQ = 0;

    // scan info / bar info / Active Format Information Preset:
    tmp = 0;
    tmp |= g_dev.cfg.OutColorMode << 5;
    hdmitxwr(0x58,tmp);

    // Active Format Aspect Ratio:
    tmp = 0;
    if ( g_dev.input.PicAR == AR_4_3 ) {
        tmp |= 0x09;
    }
    if ( g_dev.input.PicAR == AR_16_9 ) {
        tmp |= 0x0A;
    }
    if ( g_dev.input.PicAR == AR_14_9 ) {
        tmp |= 0x0B;
    }
    else {
        tmp |= 0x08;
    }

    // Picture Aspect Ratio:
    if ( g_dev.input.PicAR == AR_4_3 ) {
        //hdmitxset(0x59, 0x30, 0x10);
        tmp |= 0x10;
    }
    if ( g_dev.input.PicAR == AR_16_9 ) {
        //hdmitxset(0x59, 0x30, 0x20);
        tmp |= 0x20;
    }
    else {
        //hdmitxset(0x59, 0x30, 0x00);
    }

    if (g_dev.cfg.EnExtCol == true) {
        tmp |= 0xc0;
    }

    hdmitxwr(0x59,tmp);

    tmp = 0;
    if (g_dev.cfg.EnExtCol == true) {
        tmp |= ((g_dev.cfg.ExtColFmt << 4)&0x70);
    }

    tmp |= ((AVIQ << 2)&0x8F);

    hdmitxwr(0x5A, tmp );
    if ( g_dev.cfg.En4Kx2K ) {
        hdmitxwr(0x5B, 0);
    }
    else {
        hdmitxwr(0x5B, g_dev.input.VIC);
    }
    //hdmitxwr(0x5C, (AVIYQ << 6) + (g_dev.vars.PixRpt - 1));

    if( g_dev.cfg.EnChgAVIPR )
        hdmitxwr(0x5C, (AVIYQ<<6)+(NewAVIPR-1));
    else
        hdmitxwr(0x5C, (AVIYQ<<6)+(g_dev.vars.PixRpt-1));

    hdmitxwr(0x5D, 0x00);
    hdmitxwr(0x5E, 0x00);
    hdmitxwr(0x5F, 0x00);
    hdmitxwr(0x60, 0x00);
    hdmitxwr(0x61, 0x00);
    hdmitxwr(0x62, 0x00);
    hdmitxwr(0x63, 0x00);
    hdmitxwr(0x64, 0x00);
    hdmitxwr(0x65, 0x00);

    chksum = 0x82;
    chksum += 0x02;
    chksum += 0x0D;
    for (i = 0x58; i <= 0x65; i++)
        chksum += hdmitxrd(i);

    hdmitxwr(0x5D, 0x100 - chksum);

    chgtxbank(0);

    hdmitxset(0xC7, 0x03, 0x03);     // Enable AVI InfoFrame


    // HDMI Vendor Specific InfoFrame
    if (g_dev.cfg.EnDV3D) {
        g_dev.cfg.EnOSD3D = false;
        g_dev.cfg.EnIVS3D = false;
    }

    if (g_dev.cfg.En3D && (g_dev.cfg.EnOSD3D || g_dev.cfg.EnDV3D || g_dev.cfg.EnIVS3D)) { // HDMI2.0 HF_VSIF
        chgtxbank(1);
        i = 0x92;
        hdmitxwr(i++, 0x01);                                  // Version=1 (PB4)
        hdmitxwr(i++, 0x01);                                  // 3D_Valid=1 (PB5)
        hdmitxwr(i++, (g_dev.cfg.Sel3DFmt << 4) +              // 3D_Structure (PB6)
            ((g_dev.cfg.EnDV3D || g_dev.cfg.EnIVS3D) << 3) +    // 3D_Additional_Info_present
            ((g_dev.cfg.EnOSD3D) << 2));                       // 3D_DisparityData_present, 3D_Meta_present=0
        if (g_dev.cfg.Sel3DFmt == SbSHalf)
            hdmitxwr(i++, 0x00);                              // 3D_Ext_Data

        if (g_dev.cfg.EnDV3D)
            hdmitxwr(i++, (g_dev.cfg.EnDV3D << 4));            // 3D_DualView=1, 3DViewDependency=0b00, 3D_Preferred2DView=0b00
        else if (g_dev.cfg.EnIVS3D)
            hdmitxwr(i++, (3 << 2) + 1);                      // 3D_DualView=0, 3DVideDependency=0b11, 3D_Preferred2DVies=0b01

        if (g_dev.cfg.EnOSD3D)
            hdmitxwr(i++, 0x00);                              // 3D_DisparityData_version=0b000, 3D_DisparitData_lenght=0x00

        hdmitxwr(0x90, i - 0x92 + 3);

        for (; i <= 0xA9; i++)
            hdmitxwr(i, 0x00);

        chksum = 0x81;
        chksum += 0x01;
        chksum += hdmitxrd(0x90);
        chksum += 0xD8;
        chksum += 0x5D;
        chksum += 0xC4;
        for (i = 0x92; i <= 0xA9; i++)
            chksum += hdmitxrd(i);

        hdmitxwr(0x91, 0x100 - chksum);
        hdmitxset(0x90, 0x80, 0x80);
        chgtxbank(0);

        hdmitxset(0xC7, 0xC0, 0xC0);    // Enable HDMI Vendor Specific InfoFrame
    } else if (g_dev.cfg.En3D) {         // HDMI1.4 VSIF
        chgtxbank(1);
        if (g_dev.cfg.Sel3DFmt == SbSHalf)
            hdmitxwr(0x90, 0x06);
        else
            hdmitxwr(0x90, 0x05);

        hdmitxwr(0x92, 0x40);                      // for 3D
        hdmitxwr(0x93, g_dev.cfg.Sel3DFmt << 4);    // 3D_Structure, 3D_Meta_present=0
        hdmitxwr(0x94, 0x00);                      // 3D_Ext_Data
        for (i = 0x95; i <= 0xA9; i++)
            hdmitxwr(i, 0x00);

        chksum = 0x81;
        chksum += 0x01;
        chksum += hdmitxrd(0x90);
        chksum += 0x03;
        chksum += 0x0C;
        chksum += 0x00;
        for (i = 0x92; i <= 0xA9; i++)
            chksum += hdmitxrd(i);

        hdmitxwr(0x91, 0x100 - chksum);
        chgtxbank(0);

        hdmitxset(0xC7, 0xC0, 0xC0);    // Enable HDMI Vendor Specific InfoFrame
    } else if (g_dev.cfg.En4Kx2K) {
        chgtxbank(1);
        hdmitxset(0x90, 0x80, 0x00);
        hdmitxwr(0x90, 0x05);
        hdmitxwr(0x92, 0x20);       // for 4Kx2K
        hdmitxwr(0x93, g_dev.vars.HDMI_VIC);   // HDMI_VIC
        for (i = 0x94; i <= 0xA9; i++)
            hdmitxwr(i, 0x00);

        chksum = 0x81;
        chksum += 0x01;
        chksum += 0x05;
        chksum += 0x03;
        chksum += 0x0C;
        chksum += 0x00;
        for (i = 0x92; i <= 0xA9; i++)
            chksum += hdmitxrd(i);

        hdmitxwr(0x91, 0x100 - chksum);
        chgtxbank(0);

        hdmitxset(0xC7, 0xC0, 0xC0);    // Enable HDMI Vendor Specific InfoFrame
    } else
        hdmitxset(0xC7, 0xC0, 0x00);    // Disable HDMI Vendor Specific InfoFrame

    if (g_dev.cfg.EnDRMInfo) {
        if ((g_dev.cfg.DRMEOTF == 0 && (g_dev.sink.HDR_EOTF & 0x01) == 0x00) ||
            (g_dev.cfg.DRMEOTF == 1 && (g_dev.sink.HDR_EOTF & 0x02) == 0x00) ||
            (g_dev.cfg.DRMEOTF == 2 && (g_dev.sink.HDR_EOTF & 0x04) == 0x00) ||
            (g_dev.cfg.DRMEOTF == 3 && (g_dev.sink.HDR_EOTF & 0x08) == 0x00)) {
            g_dev.cfg.EnDRMInfo = false;
            pr_info2("\r\nEDID do not support EOTF=%d, Dynamic Range and Mastering InfoFrame is diabled !!!\r\n", (int)g_dev.cfg.DRMEOTF);
        }
    }

    if (g_dev.cfg.EnDRMInfo) {   // use NULL packet
        chgtxbank(1);
        hdmitxwr(0x38, 0x87);   // HD00
        hdmitxwr(0x39, 0x01);   // HD01
        hdmitxwr(0x3A, 0x1A);   // HD02 (Length=26)

        if (g_dev.cfg.DRMZERO) {
            hdmitxwr(0x3C, 0x00);   // zeroed PB01 for ATC
            pr_info2("\r\nEnable Zeroed Dynamic Range and Mastering InfoFrame ...\r\n\r\n");
        } else {
            hdmitxwr(0x3C, g_dev.cfg.DRMEOTF); // PB01
            pr_info2("\r\nEnable Dynamic Range and Mastering InfoFrame, EOTF=%d ...\r\n\r\n", (int)g_dev.cfg.DRMEOTF);
        }

        hdmitxwr(0x3D, 0x00);   // PB02 (Static_Metadata_Descriptor_ID=0)
        for (i = 0x3E; i <= 0x55; i++)
            if (g_dev.cfg.DRMZERO)
                hdmitxwr(i, 0); // zeroed content for ATC
            else
                hdmitxwr(i, i); // dummy content for test
        hdmitxwr(i, 0x00);      // PB27

        chksum = 0x87;
        chksum += 0x01;
        chksum += 0x1A;
        for (i = 0x3C; i <= 0x55; i++)
            chksum += hdmitxrd(i);

        hdmitxwr(0x3B, 0x100 - chksum);
        chgtxbank(0);

        hdmitxset(0xC8, 0x03, 0x03);
    } else
        hdmitxset(0xC8, 0x03, 0x00);

    if (g_dev.cfg.EnExtCol == true && (g_dev.cfg.ExtColFmt == xvYCC601 || g_dev.cfg.ExtColFmt == xvYCC709)) {
        chgtxbank(1);
        hdmitxwr(0xB0, 0x00);
        hdmitxwr(0xB1, 0xB0);
        hdmitxwr(0xB2, (g_dev.cfg.ColorDepth << 3) + (g_dev.cfg.ExtColFmt + 1));
        hdmitxwr(0xB3, 0x00);
        hdmitxwr(0xB4, 0x04);

        // The following GBD is used for 8-bit only
        if (g_dev.cfg.ExtColFmt == xvYCC601) {
            hdmitxwr(0xB5, 16);     // Black(Y)
            hdmitxwr(0xB6, 128);    // Black(Cb)
            hdmitxwr(0xB7, 128);    // Black(Cr)

            hdmitxwr(0xB8, 65);     // Red(Y)
            hdmitxwr(0xB9, 100);    // Red(Cb)
            hdmitxwr(0xBA, 212);    // Red(Cr)

            hdmitxwr(0xBB, 112);    // Green(Y)
            hdmitxwr(0xBC, 72);     // Green(Cb)
            hdmitxwr(0xBD, 58);     // Green(Cr)

            hdmitxwr(0xBE, 35);     // Blue(Y)
            hdmitxwr(0xBF, 212);    // Blue(Cb)
            hdmitxwr(0xC0, 114);    // Blue(Cr)
        } else {    // xvYCC709
            hdmitxwr(0xB5, 16);     // Black(Y)
            hdmitxwr(0xB6, 128);    // Black(Cb)
            hdmitxwr(0xB7, 128);    // Black(Cr)

            hdmitxwr(0xB8, 51);     // Red(Y)
            hdmitxwr(0xB9, 109);    // Red(Cb)
            hdmitxwr(0xBA, 212);    // Red(Cr)

            hdmitxwr(0xBB, 133);    // Green(Y)
            hdmitxwr(0xBC, 63);     // Green(Cb)
            hdmitxwr(0xBD, 52);     // Green(Cr)

            hdmitxwr(0xBE, 28);     // Blue(Y)
            hdmitxwr(0xBF, 212);    // Blue(Cb)
            hdmitxwr(0xC0, 120);    // Blue(Cr)
        }

        for (i = 0xC0; i <= 0xC6; i++)
            hdmitxwr(i, 0x00);

        chgtxbank(0);

        hdmitxwr(0xC6, 0x02);   // RegPktGBDUpd=1
        hdmitxwr(0xC6, 0x01);   // RegPktGBDEn=1
    }
}

void It6265::it6265_set_csc_matrix( const u8 *csc_matrix )
{
    u8 offset;

    for( offset=0xA4 ; offset<=0xB9 ; offset++ )
    {
        if ( offset != 0xA6 ) {
            hdmitxwr( offset, *csc_matrix );
            csc_matrix++;
        }
    }
}

void It6265::it6265_set_video_format( void )
{
    int cscsel, fmtsel;

    pr_info("it6265_set_video_format(): CD=%d, i=%d, o=%d\r\n", (int)g_dev.cfg.ColorDepth, (int)g_dev.cfg.InColorMode, (int)g_dev.cfg.OutColorMode);
    hdmitxset(0x91, 0x03, g_dev.cfg.InColorMode);
    hdmitxset(0xC0, 0x01, g_dev.cfg.EnHDMI);

    if (g_dev.cfg.ColorDepth == VID8BIT || g_dev.cfg.OutColorMode == YCbCr422)
        hdmitxset(0xC1, 0x70, 0);
    else
        hdmitxset(0xC1, 0x70, (0x04 + g_dev.cfg.ColorDepth) << 4);

    // CSC
    if (g_dev.cfg.InColorMode == RGB444 &&
        (g_dev.cfg.OutColorMode == YCbCr444 || g_dev.cfg.OutColorMode == YCbCr422)) {
        cscsel = RGB2YUV;
        hdmitxset(0x0F, 0x10, 0x00);
    } else if ((g_dev.cfg.InColorMode == YCbCr444 || g_dev.cfg.InColorMode == YCbCr422 || g_dev.cfg.InColorMode == YCbCr420) &&
                g_dev.cfg.OutColorMode == RGB444) {
        cscsel = YUV2RGB;
        hdmitxset(0x25, 0x04, 0x00);
    } else {
        cscsel = NOCSC;
        hdmitxset(0x25, 0x04, 0x04);
    }

    if (g_dev.cfg.YCbCrCoef == ITU601 && g_dev.cfg.DynRange == DynCEA)
        fmtsel = 0;
    else if (g_dev.cfg.YCbCrCoef == ITU601 && g_dev.cfg.DynRange == DynVESA)
        fmtsel = 1;
    else if (g_dev.cfg.YCbCrCoef == ITU709 && g_dev.cfg.DynRange == DynCEA)
        fmtsel = 2;
    else if (g_dev.cfg.YCbCrCoef == ITU709 && g_dev.cfg.DynRange == DynVESA)
        fmtsel = 3;
    else if (g_dev.cfg.YCbCrCoef == ITU2020 && g_dev.cfg.DynRange == DynCEA)
        fmtsel = 4;
    else if (g_dev.cfg.YCbCrCoef == ITU2020 && g_dev.cfg.DynRange == DynVESA)
        fmtsel = 5;
    else
    {
        pr_err("ERR:YCbCrCoef=%d, DynRange=%d\r\n", g_dev.cfg.YCbCrCoef, g_dev.cfg.DynRange);
        fmtsel = -1;
    }

    switch (cscsel) {
    case RGB2YUV :
        switch (fmtsel) {
        case 0 : // YCbCrCoef==ITU601 && DynRange==DynCEA
            it6265_set_csc_matrix( csc_rgb2yuv_ITU601_DynCEA );
            break;
        case 1 : // YCbCrCoef==ITU601 && DynRange==DynVESA
            it6265_set_csc_matrix( csc_rgb2yuv_ITU601_DynVESA );
            break;
        case 2 : // YCbCrCoef==ITU709 && DynRange==DynCEA
            it6265_set_csc_matrix( csc_rgb2yuv_ITU709_DynCEA );
            break;
        case 3 : // YCbCrCoef==ITU709 && DynRange==DynVESA
            it6265_set_csc_matrix( csc_rgb2yuv_ITU709_DynVESA );
            break;
        case 4 : // YCbCrCoef==ITU2020 && DynRange==DynCEA
            it6265_set_csc_matrix(csc_rgb2yuv_ITU2020_DynCEA);
            break;
        case 5 : // YCbCrCoef==ITU2020 && DynRange==DynVESA
            it6265_set_csc_matrix( csc_rgb2yuv_ITU2020_DynVESA );
            break;
        default:
            pr_err("ERROR: CSC Format Select Error !!!\r\n");
            break;
        }
        break;
    case YUV2RGB :
        switch (fmtsel) {
        case 0 : // YCbCrCoef==ITU601 && DynRange==DynCEA
            it6265_set_csc_matrix( csc_yuv2rgb_ITU601_DynCEA );
            break;
        case 1 : // YCbCrCoef==ITU601 && DynRange==DynVESA
            it6265_set_csc_matrix( csc_yuv2rgb_ITU601_DynVESA );
            break;
        case 2 : // YCbCrCoef==ITU709 && DynRange==DynCEA
            it6265_set_csc_matrix( csc_yuv2rgb_ITU709_DynCEA );
            break;
        case 3 : // YCbCrCoef==ITU709 && DynRange==DynVESA
            it6265_set_csc_matrix( csc_yuv2rgb_ITU709_DynVESA );
            break;
        case 4 : // YCbCrCoef==ITU709 && DynRange==DynCEA
            it6265_set_csc_matrix(csc_yuv2rgb_ITU2020_DynCEA);
            break;
        case 5 : // YCbCrCoef==ITU709 && DynRange==DynVESA
            it6265_set_csc_matrix(csc_yuv2rgb_ITU2020_DynVESA);
            break;
        default:
            pr_err("ERROR: CSC Format Select Error !!!\r\n");
            break;
        }
        break;
    default :
        break;
    }

    hdmitxset(0xA0, 0x03, cscsel);
    hdmitxset(0x98, 0x02, g_dev.cfg.EnBlueScrMute << 1);
}

void It6265::it6265_config_hdmi_pg(void)
{
    u8 vic;

    vic = g_dev.input.VIC;
    hdmitxset(0x22, 0x01, 0x01);
    hdmitxset(0x1A, 0x38, 0x00);  // Enable InLBOvFlw, InFFOvFlw, InDESyncLose Interrupt
    hdmitxset(0x95, 0x80, 0x00);
    //it6265_set_syncde_gen(vic);
    it6265_set_pattern_gen(vic);
}

static const u8 s_audsrc[] =
{
    0x01, 0x03, 0x03, 0x03, 0x05, 0x07, 0x07, 0x07,
    0x05, 0x07, 0x07, 0x07, 0x0d, 0x0f, 0x0f, 0x0f,
    0x0d, 0x0f, 0x0f, 0x0f, 0x09, 0x0b, 0x0b, 0x0b,
    0x0d, 0x0f, 0x0f, 0x0f, 0x0d, 0x0f, 0x0f, 0x0f,
};

void It6265::it6265_set_audio_format( void )
{
    int i, audsrc, infoca;
    u8 chksum;

    hdmitxset(0xDA, 0x7F, (g_dev.cfg.AudTypSWL << 5) + g_dev.cfg.AudTypFmt);
    if (g_dev.cfg.AudSel == SPDIF) {
        hdmitxwr(0xDC, 0x00);
        hdmitxwr(0xDD, 0x00);
        hdmitxwr(0xDE, 0x00);
        hdmitxset(0x20, 0x80, (g_dev.cfg.EnMCLKSample << 7)+(g_dev.cfg.MCLKFreq<<4));
    } else {
        hdmitxwr(0xDC, 0x10);
        hdmitxwr(0xDD, 0x32);
        hdmitxwr(0xDE, 0x54);
    }

    hdmitxset(0xE3, 0x01, cfg_RecChStSel);

    if (g_dev.cfg.AudType == HBR)
        hdmitxwr(0xD8, (g_dev.cfg.EnMSAud << 5) + (g_dev.cfg.En3DAud << 4) + 0x08);
    else if (g_dev.cfg.AudType == DSD)
        hdmitxwr(0xD8, (g_dev.cfg.EnMSAud << 5) + (g_dev.cfg.En3DAud << 4) + 0x04);
    else if (g_dev.cfg.EnTDM) {
        hdmitxwr(0xD8, (g_dev.cfg.EnMSAud << 5) + (g_dev.cfg.En3DAud << 4) + 0x02);
        hdmitxset(0xE2, 0x07, g_dev.cfg.TDMCh);
    } else
        hdmitxwr(0xD8, (g_dev.cfg.EnMSAud << 5) + (g_dev.cfg.En3DAud << 4));

    chgtxbank(1);

    // Set Audio N Value
    switch (g_dev.cfg.AudFmt) {
    case AUD32K :   // 4096 = 0x1000
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x10);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD44K :   // 6272 = 0x1880
        hdmitxwr(0x33, 0x80);
        hdmitxwr(0x34, 0x18);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD48K :   // 6144 = 0x1800
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x18);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD64K :   // 8192 = 0x2000
    case AUD256K :
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x20);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD88K :   // 12544 = 0x3100
    case AUD352K :
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x31);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD96K :   // 12288 = 0x3000
    case AUD384K :
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x30);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD128K :   // 16384 = 0x4000
    case AUD512K :
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x40);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD176K :   // 25088 = 0x6200
    case AUD705K :
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x62);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD192K :   // 24576 = 0x6000
    case AUD768K :
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x60);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD1024K :  // 32768 = 0x8000
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0x80);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD1411K :  // 50176 = 0xC400
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0xC4);
        hdmitxwr(0x35, 0x00);
        break;
    case AUD1536K :  // 49152 = 0xC000
        hdmitxwr(0x33, 0x00);
        hdmitxwr(0x34, 0xC0);
        hdmitxwr(0x35, 0x00);
        break;
    default :
        pr_err("Error: AudFmt Error !!!\r\n");
    }

    // Channel Status
    if (g_dev.cfg.AudType == LPCM)
        hdmitxwr(0xF0, 0x00);
    else
        hdmitxwr(0xF0, 0x02);
    hdmitxwr(0xF1, 0x00);
    hdmitxwr(0xF2, 0x00);
    hdmitxwr(0xF3, ((g_dev.cfg.AudFmt & 0x30) << 2) + (g_dev.cfg.AudFmt & 0x0F));
    hdmitxwr(0xF4, ((~(g_dev.cfg.AudFmt << 4)) & 0xF0) + 0x0B);

    // Audio InfoFrame
    if (g_dev.cfg.En3DAud)
        infoca = 0x00;
    else
        switch (g_dev.cfg.AudCh) {
        case 0 :
            infoca = 0xFF; break;  // no audio
        case 2 :
            infoca = 0x00; break;
        case 3 :
            infoca = 0x01; break;  // 0x01,0x02,0x04
        case 4 :
            infoca = 0x03; break;  // 0x03,0x05,0x06,0x08,0x14
        case 5 :
            infoca = 0x07; break;  // 0x07,0x09,0x0A,0x0C,0x15,0x16,0x18
        case 6 :
            infoca = 0x0B; break;  // 0x0B,0x0D,0x0E,0x10,0x17,0x19,0x1A,0x1C
        case 7 :
            infoca = 0x0F; break;  // 0x0F,0x11,0x12,0x1B,0x1D,0x1E
        case 8 :
            infoca = 0x1F; break;  // 0x13,0x1F
        default :
            pr_err("Error: Audio Channel Number Error !!!\r\n");
        }

    if (g_dev.cfg.En3DAud)
        hdmitxwr(0x68, 0x00);
    else
        hdmitxwr(0x68, g_dev.cfg.AudCh - 1);

    if (g_dev.cfg.AudType == DSD) {
        switch (g_dev.cfg.AudFmt) {
        case AUD32K  :
            hdmitxwr(0x69, 0x04); break;
        case AUD44K  :
            hdmitxwr(0x69, 0x08); break;
        case AUD48K  :
            hdmitxwr(0x69, 0x0C); break;
        case AUD88K  :
            hdmitxwr(0x69, 0x10); break;
        case AUD96K  :
            hdmitxwr(0x69, 0x14); break;
        case AUD176K :
            hdmitxwr(0x69, 0x18); break;
        case AUD192K :
            hdmitxwr(0x69, 0x1C); break;
        default :
            pr_err("Error: AudFmt Error for DSD !!!\r\n");
        }
    } else
        hdmitxwr(0x69, 0x00);

    hdmitxwr(0x6A, 0x00);
    hdmitxwr(0x6B, infoca);
    hdmitxwr(0x6C, 0x00);

    chksum = 0x84;
    chksum += 0x01;
    chksum += 0x0A;
    for (i = 0x68; i <= 0x6C; i++)
        chksum += hdmitxrd(i);

    hdmitxwr(0x6D, 0x100 - chksum);

    if (g_dev.cfg.En3DAud || g_dev.cfg.EnMSAud) {
        if (g_dev.cfg.En3DAud) {
            hdmitxwr(0xC8, 0x01);
            hdmitxwr(0xC9, 0x00);
            hdmitxwr(0xCA, g_dev.cfg.AudCh - 1);
            hdmitxwr(0xCB, 0x01);
            hdmitxwr(0xCC, 0x01);
            for (i = 0xCD; i <= 0xDD; i++)
                hdmitxwr(i, 0x00);
        } else {
            hdmitxwr(0xC8, 0x00);
            hdmitxwr(0xC9, (((g_dev.cfg.AudCh >> 1) - 1) << 2) + g_dev.cfg.EnDV3D);
            for (i = 0; i <= (g_dev.cfg.AudCh >> 1); i++) {
                if (g_dev.cfg.EnDV3D)
                    hdmitxwr(0xCA + i * 5, 0x03);
                else
                    hdmitxwr(0xCA + i * 5, 0x00);
                hdmitxwr(0xCB + i * 5, 0x00);
                hdmitxwr(0xCC + i * 5, 0x00);
                hdmitxwr(0xCD + i * 5, 0x00);
                hdmitxwr(0xCE + i * 5, 0x00);
            }
            for (i = 0xCA + i * 5; i <= 0xDD; i++)
                hdmitxwr(i, 0x00);
        }
    }

    chgtxbank(0);


    if (infoca >= 0 && infoca < (int)sizeof(s_audsrc)) {
        audsrc = s_audsrc[infoca];
    }
    else {
        audsrc = 0;
    }

    if (g_dev.cfg.AudType == HBR && g_dev.cfg.AudSel == I2S)
        hdmitxwr(0xDB, 0x0F);
    else if (g_dev.cfg.En3DAud) {
        if (g_dev.cfg.AudCh <= 10)
            hdmitxwr(0xDB, 0x1F);   // 9/10-channel audio
        else
            hdmitxwr(0xDB, 0x3F);   // 11/12-channel audio
    } else
        hdmitxwr(0xDB, audsrc);

    hdmitxset(0xD8, 0x01, g_dev.cfg.AudSel);
}

void It6265::it6265_scdc_cancel_rr(void)
{
    u8 rddata;
    u8 retry;
    pr_info("it6265_scdc_cancel_rr\r\n");

    for( retry = 0 ; retry < 2 ; retry++ )
    {
        if ( it6265_scdc_write(0x30, 0x00) )
        {
            it6265_scdc_read(0x30, 1);
            rddata = hdmitxrd(0x30);
            if ( 0 == (rddata&0x01) ) {
                break;
            }
        }
        idle(5);
    }

}

bool It6265::it6265_scdc_set_rr(u8 rr_enable)
{
    u8 rddata;
    int retry;

    if ( g_dev.sink.HDMIVer < 2 ) {
        return false;
    }

    if ( g_dev.sink.rr_capable == 0 && (rr_enable>0) ) {
        return false;
    }

    rr_enable = 0;

    for( retry = 0 ; retry < 3 ; retry++ )
    {
        if ( it6265_scdc_write(0x30, rr_enable) )
        {
            it6265_scdc_read(0x30, 1);
            rddata = hdmitxrd(0x30);
            if ( rr_enable == (rddata&0x01) ) {
                pr_info("RX SCDC RR_Enable=%d\r\n", rr_enable);
                return true;
            }
            else{
                pr_info("it6265_scdc_set_rr fail %d != %d\r\n", (int)rr_enable, (int)rddata);
            }
        }
        idle(5);
    }

    return false;
}

bool It6265::it6265_read_edid(u8 block, u8 offset, int length, u8 *edid_buffer)
{
    bool result = false;
    int off = block*128 + offset;
    int retry = 0;

    offset = off % 256;

    // Enable DDC Bus Reset
    // hdmitxset(0x28, 0x01, EnDDCMasterSel);   // Enable PC DDC Mode
    // Do not handle the DDC Bus Hang here
    // hdmitxwr(0x2E, 0x0F);  // Abort DDC Command
__RETRY:

    hdmitxset(0x28, 0x01, 0x01);    // Enable PC DDC Mode FW Only
    hdmitxset(0x19, 0x04, 0x04);    // Enable DDC Command Fail Interrupt
    hdmitxset(0x1D, 0x08, 0x08);    // Enable DDC Bus Hang Interrupt

    hdmitxwr(0x2E, 0x09);                  // DDC FIFO Clear
    hdmitxwr(0x29, 0xA0);                  // EDID Address
    hdmitxwr(0x2A, offset);                // EDID Offset
    hdmitxwr(0x2B, length);                // Read ByteNum[7:0]
    hdmitxwr(0x2C, (length & 0x300) >> 8); // Read ByteNum[9:8]
    hdmitxwr(0x2D, block / 2);           // EDID Segment

    if (it6265_is_hpd_high()) {
        hdmitxwr(0x2E, 0x03);  // EDID Read Fire
        pr_info("into it6265_ddcwait()\r\n");
        if (it6265_ddcwait()) {
            hdmitxbrd(0x30, length, edid_buffer);
            // xbh print edid start
            pr_info("edid_buffer\r\n");
            for(int i = 0; i < length; ++i)
            {
                pr_info("0X%02x ", edid_buffer[i]);
            }
            pr_info("\r\n");
            // xbh print edid start
            pr_info("it6265_read_edid length %d\n", length);
            result = true;
        } else {
            pr_info("ERROR: DDC EDID Read Fail !!!\r\n");
            pr_err("ERROR: DDC EDID Read Fail !!!\r\n");
            if ( retry > 0 ) {
                retry--;
                idle( 100 );
                goto __RETRY;
            }
        }
    } else {
        pr_info("Abort EDID read becasue of detecting unplug !!!\r\n");
        pr_err("Abort EDID read becasue of detecting unplug !!!\r\n");
    }

    hdmitxset(0x28, 0x01, 0x00);   // Disable PC DDC Mode

    return result;
}

bool It6265::it6265_read_one_block_edid(u8 block, u8 *edid_buffer)
{
    u8 offset;
    u8 i;
    u8 read_len = 32;
    u8 retry = 2;

__RETRY:
    offset = 0;
    for (i = 0; i < 128 / read_len; i++) {
        pr_info("it6265_read_edid start\r\n");
        if (it6265_read_edid(block, offset, read_len, edid_buffer)) {
            pr_info("it6265_read_one_block_edid if()\r\n");
            edid_buffer += read_len;
            offset += read_len;
            continue;
        } else {
            pr_info("it6265_read_one_block_edid else(), retry = %d\r\n", retry);
            it6265_ddc_abort();

            if ( retry > 0 ) {
                retry--;
                goto __RETRY;
            }
            else {
                pr_err("ERROR: read edid block 0, offset %d, length %d fail.\r\n", (int)offset, (int)read_len);
                return false;
            }
        }
    }

    return true;
}

void It6265::it6265_reset_vbotx( void )
{
    //__reset_vbotx( g_dev.input.VIC, 1<<g_dev.vars.VBOLaneNum, g_dev.vars.bytenum+3 );
}

bool It6265::it6265_edid_parse( void )
{
    //u8 edid_tmp[128];
    u8 *edid_tmp = g_dev.data_buffer;
    u8 extblock;
    u8 i;

    // clear sink data after HPD toggle
    memset(&g_dev.sink, 0, sizeof(g_dev.sink));
    g_dev.cfg.EnHDMI = false;

    pr_info("it6265_read_one_block_edid start!!!!\r\n");
    if (false == it6265_read_one_block_edid(0, edid_tmp)) {
        pr_err("ERROR: read edid block 0\r\n");
        goto __err_exit;
    }

    pr_info("it6265_read_one_block_edid succcess\r\n");
    it6265_parse_edid_block0( edid_tmp );


    #if ENABLE_CALLBACK
    if ( g_dev.cb.it6265_edid_read_cb ) {
        g_dev.cb.it6265_edid_read_cb(0, edid_tmp, g_dev.cb_data.edid_read_cb_data);
    }
    #endif

    // read Ext block no
    extblock = edid_tmp[0x7E];

    if (extblock > 3) {
        pr_err("Warning: Extblock = %d \r\n", (int)extblock);
        extblock = 3;
    }


    for (i = 1; i <= extblock; i++) {

        if (false == it6265_read_one_block_edid(i, edid_tmp)) {
            pr_err("ERROR: read edid block %d\r\n", (int)i);
            goto __err_exit;
        }

        it6265_parse_CEA_Block(edid_tmp);

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_edid_read_cb ) {
            g_dev.cb.it6265_edid_read_cb(i, edid_tmp, g_dev.cb_data.edid_read_cb_data);
        }
        #endif
    }

    //g_dev.cfg.ColorDepth = OUTPUT_COLOR_DEPTH;
    #if CHECK_EDID_COLOR_DEPTH

    switch(g_dev.vars.VBOByteMode)
    {
        case 1:
            g_dev.cfg.ColorDepth = VID10BIT;
            break;
        case 2:
            g_dev.cfg.ColorDepth = VID12BIT;
            break;
        default :
            g_dev.cfg.ColorDepth = VID8BIT;
            break;
    }

    if ( g_dev.cfg.ColorDepth != VID8BIT ) {

        if ( g_dev.cfg.ColorDepth == VID16BIT ) {
            if ( g_dev.sink.dc_48bit == 0 ) {
                g_dev.cfg.ColorDepth = VID12BIT;
            }
        }
        if ( g_dev.cfg.ColorDepth == VID12BIT ) {
            if ( g_dev.sink.dc_36bit == 0 ) {
                g_dev.cfg.ColorDepth = VID10BIT;
            }
        }
        if ( g_dev.cfg.ColorDepth == VID10BIT ) {
            if ( g_dev.sink.dc_30bit == 0 ) {
                g_dev.cfg.ColorDepth = VID8BIT;
            }
        }
    }
    #else
    #endif

    if ( (g_dev.sink.HDMIVer > 1) && (g_dev.sink.max_TMDS2 != 0) ) {
        g_dev.sink.max_TMDS_clk = g_dev.sink.max_TMDS2;
    }
    else{
        g_dev.sink.max_TMDS_clk = g_dev.sink.max_TMDS1;
    }
    //g_dev.cfg.EnHDMI = 1;
    //g_dev.sink.HDMIVer = 2;


    pr_info( "### Sink is HDMI %d.x device, EnHDMI=%d\r\n", (int)g_dev.sink.HDMIVer, (int)g_dev.cfg.EnHDMI );

    return true;

__err_exit:
    pr_info( "### Sink is HDMI %d.x device, EnHDMI=%d\r\n", (int)g_dev.sink.HDMIVer, (int)g_dev.cfg.EnHDMI );
    return false;
}

void It6265::it6265_hdcp_show_ri( void )
{
    u8 tmp[4];

    // Read ARi/BRi
    hdmitxbrd(0x58, 2, tmp);
    hdmitxbrd(0x60, 2, &tmp[2]);
//    pr_info2("ARi = 0x%02X%02X\r\n", (int)tmp[0],(int)tmp[1]);
//    pr_info2("BRi = 0x%02X%02X\r\n\r\n", (int)tmp[2],(int)tmp[3]);
}

void It6265::it6265_hdcp_show_pj( void )
{
    u8 APj, BPj;

    APj = hdmitxrd(0x5A);
    BPj = hdmitxrd(0x62);

    pr_info2("APj = 0x%02X\r\n", (int)APj);
    pr_info2("BPj = 0x%02X\r\n\r\n", (int)BPj);
}

static u8 it6265_hdcp_revocation_key_list[][5]=
{
    {0x23, 0xde, 0x5c, 0x43, 0x93},
    {0x0b, 0x37, 0x21, 0xb4, 0x7d},
};

bool It6265::it6265_hdcp_is_revocation_key(u8 *BKSV)
{
    u16 i;

    for (i = 0; i < sizeof(it6265_hdcp_revocation_key_list) / sizeof(it6265_hdcp_revocation_key_list[0]); i++) {
        if (0 == memcmp(BKSV, &it6265_hdcp_revocation_key_list[i][0], 5)) {
            return true;
        }
    }

    return false;
}

static u8 it6265_hdcp_revocation_rxid_list[][5]=
{
    {0xa6, 0x50, 0x3D, 0x09, 0xFD},
    //{0xa6, 0x50, 0x3D, 0x09, 0xdf},
};

bool It6265::it6265_hdcp_is_revocation_rxid(u8 *RxID)
{
    u16 i;

    for (i = 0; i < sizeof(it6265_hdcp_revocation_rxid_list) / sizeof(it6265_hdcp_revocation_rxid_list[0]); i++) {
        if (0 == memcmp(RxID, &it6265_hdcp_revocation_rxid_list[i][0], 5)) {
            return true;
        }
    }

    return false;
}

void It6265::it6265_hdcp1_show_status( void )
{
    u8 tmp[8];
    int BCaps, BStatus1, BStatus2;
    int status;

    status = hdmitxrd(0x66);

    BCaps = hdmitxrd(0x63);
    BStatus1 = hdmitxrd(0x64);
    BStatus2 = hdmitxrd(0x65);

    // Read An
    hdmitxbrd(0x48, 8, tmp);
    pr_info2("An = %02X %02X %02X %02X - %02X %02X %02X %02X\r\n",
        (int)tmp[0],(int)tmp[1],(int)tmp[2],(int)tmp[3],(int)tmp[4],(int)tmp[5],(int)tmp[6],(int)tmp[7]);

    // Read AKSV
    hdmitxbrd(0x43, 5, tmp);
    pr_info2("AKSV = %02X %02X %02X %02X %02X\r\n",
        (int)tmp[0],(int)tmp[1],(int)tmp[2],(int)tmp[3],(int)tmp[4]);

    // Read BKSV
    hdmitxbrd(0x5B, 5, tmp);
    pr_info2("BKSV = %02X %02X %02X %02X %02X\r\n",
        (int)tmp[0],(int)tmp[1],(int)tmp[2],(int)tmp[3],(int)tmp[4]);

    it6265_hdcp_show_ri();

    pr_info2("Downstream count = %d\r\n", (int)(BStatus1 & 0x7F));
    pr_info2("MAX_DEVS_EXCEEDED = %d\r\n", (int)((BStatus1 >> 7)&0x01));
    pr_info2("BStatus[15:8] = 0x%02X\r\n", (int)(BStatus2));


    pr_info2("Rx HDCP Fast Reauthentication = %d \r\n", BCaps & 0x01);
    pr_info2("Rx HDCP 1.1 Features = %d ", (int)((BCaps & 0x02) >> 1));

    if( (BCaps&0x02) && g_dev.cfg.EnHDCP1p1 ) {
        pr_info2("Enabled\r\n");
    }
    else{
        pr_info2("Disabled\r\n");
    }

    if (BCaps & 0x10) {
        pr_info2("Rx HDCP Maximum DDC Speed = 400KHz\r\n");
    }
    else {
        pr_info2("Rx HDCP Maximum DDC Speed = 100KHz\r\n");
    }


    pr_info2("Rx HDCP Repeater = %d \r\n", (int)((BCaps & 0x40) >> 6));

    pr_info2("Tx Authentication Status = 0x%02X \r\n", (int)status);

    if ((status & 0x80) != 0x80) {
        if (status & 0x01) {
            pr_info2(": DDC NAck too may times !!!\r\n");
        }

        if (status & 0x02) {
            pr_info2(": BKSV Invalid !!!\r\n");
        }

        if (status & 0x04) {
            pr_info2(": Link Check Fail (AR0/=BR0) !!!\r\n");
        }

        if (status & 0x08) {
            pr_info2(": Link Integrity Ri Check Fail !!!\r\n");
            it6265_hdcp_show_ri();
        }

        if (status & 0x10) {
            pr_info2(": Link Integrity Pj Check Fail !!!\r\n");
        }

        if (status & 0x20) {
            u8 ARi1, ARi2, BRi1, BRi2;
            u8 preARi1, preARi2, preBRi1, preBRi2;

            pr_info2(": Link Integrity HDCP 1.2 Sync Detect Fail !!!\r\n");

            ARi1 = hdmitxrd(0x58);
            ARi2 = hdmitxrd(0x59);
            BRi1 = hdmitxrd(0x60);
            BRi2 = hdmitxrd(0x61);
            pr_info2("Current ARi = 0x%02X%02X \r\n", (int)ARi2 , (int)ARi1);
            pr_info2("Current BRi = 0x%02X%02X \r\n", (int)BRi2 , (int)BRi1);

            hdmitxset(0x70, 0x08, 0x00);
            preARi1 = hdmitxrd(0x76);
            preARi2 = hdmitxrd(0x77);
            hdmitxset(0x70, 0x08, 0x08);
            preBRi1 = hdmitxrd(0x76);
            preBRi2 = hdmitxrd(0x77);
            hdmitxset(0x70, 0x08, 0x00);
            pr_info2("Previous ARi = 0x%02X%02X \r\n", (int)preARi2 , (int)preARi1);
            pr_info2("Previous BRi = 0x%02X%02X \r\n", (int)preBRi2 , (int)preBRi1);

            if (BRi1 == preARi1 && BRi2 == preARi2) {
                pr_info2("CurBRi==PreARi ==> Sync Detection Error because of Miss CTLx signal\r\n");
            }
            if (ARi1 == preBRi1 && ARi2 == preBRi2) {
                pr_info2("CurARi==PreBRi ==> Sync Detection Error because of Extra CTLx signal\r\n");
            }

        }

        if (status & 0x40) {
            pr_info2(": DDC Bus Hang TimeOut !!!\r\n");
        }
    }
}

void It6265::it6265_hdcp2_show_status( void )
{
    u16 HDCP2FailStatus;
    u8 tmp[5];

    // Read RxID
    hdmitxbrd(0x5B, 5, tmp);
    pr_info2("RxID = %02X %02X %02X %02X %02X\r\n",
        (int)tmp[4],(int)tmp[3],(int)tmp[2],(int)tmp[1],(int)tmp[0]);

    HDCP2FailStatus = (hdmitxrd(0x4F) << 8) + hdmitxrd(0x4E);

    g_dev.vars.hdcp2_fail_status = HDCP2FailStatus;
    pr_info2("HDCP2Tx Authentication Fail Status %04X \r\n", (int)HDCP2FailStatus);
    if (HDCP2FailStatus != 0x0000) {
        if (HDCP2FailStatus & 0x0001) {
            pr_info2("Auth Fail: RxHDCP2 Capability=0 !\r\n");
        }
        if (HDCP2FailStatus & 0x0002) {
            pr_info2("Auth Fail: Locality Check fail !\r\n");
        }
        if (HDCP2FailStatus & 0x0004) {
            pr_info2("Auth Fail: Read AKE Send Cert 100ms timeout !\r\n");
        }
        if (HDCP2FailStatus & 0x0008) {
            pr_info2("Auth Fail: Read AKE Send H prime (km_stored_rdy=1) 200ms timeout !\r\n");
        }
        if (HDCP2FailStatus & 0x0010) {
            pr_info2("Auth Fail: Read AKE Send H prime (km_stored_rdy=0) 1sec timeout !\r\n");
        }
        if (HDCP2FailStatus & 0x0020) {
            pr_info2("Auth Fail: Read AKE Send Pairing Info 200ms timeout !\r\n");
        }
        if (HDCP2FailStatus & 0x0040) {
            pr_info2("Auth Fail: Read Repeater Receiver ID List 3sec timeout !\r\n");
        }
        if (HDCP2FailStatus & 0x0080) {
            pr_info2("Auth Fail: Signature Verified fail !\r\n");
        }
        if (HDCP2FailStatus & 0x0100) {
            pr_info2("Auth Fail: H Verified fail !\r\n");
        }
        if (HDCP2FailStatus & 0x0200) {
            pr_info2("Auth Fail: A1/A5 polling Rxstatus ReAuth_Req = 1 !\r\n");
        }
        if (HDCP2FailStatus & 0x0400) {
            pr_info2("Auth Fail: Repeater device count/cascade exceed or seq_num_V Error !\r\n");
        }
        if (HDCP2FailStatus & 0x0800) {
            pr_info2("Auth Fail: A6 polling Rxstatus ReAuth_Req = 1 !\r\n");
        }
        if (HDCP2FailStatus & 0x1000) {
            pr_info2("Auth Fail: V Verified fail !\r\n");
        }
        if (HDCP2FailStatus & 0x2000) {
            pr_info2("Auth Fail: seq_num_M rolls over error !\r\n");
        }
        if (HDCP2FailStatus & 0x4000) {
            pr_info2("Auth Fail: DDC Bus command error !\r\n");

            /*
            hdmitxset(0x28, 0x01, 0x01);
            hdmitxset(0x35, 0x10, 0x10);
            idle(5000);
            hdmitxset(0x35, 0x10, 0x00);
            hdmitxset(0x28, 0x01, 0x00);
            */
        }
    }
}

void It6265::it6265_hdcp_auth_fail( void )
{
    hdmitxwr(0x13, 0x03);  // also clear previous Authentication Done Interrupt

    if (g_dev.vars.HDCP2Sel == false) {
        pr_info2("HDCP1 Authentication Fail Interrupt ...\r\n\r\n");
        it6265_hdcp1_show_status();
    }
    else {
        pr_info2("HDCP2 Authentication Fail Interrupt ...\r\n\r\n");
        it6265_hdcp2_show_status();
        if ( g_dev.vars.hdcp2_fail_count <= MAX_HDCP2_FAIL_COUNT ) {
            g_dev.vars.hdcp2_fail_count++;
        }
    }

    it6265_hdcp_task_retry_auth();
}

void It6265::it6265_set_audio_option( void )
{
    u8 tmp;

    tmp = (0 << 5) + (0 << 4) ;
    tmp |= (g_dev.cfg.AudCTSOpt << 3);
    tmp |= (g_dev.cfg.EnhAudCTS << 1);
    hdmitxset(0xC4, 0x3A, tmp);
    hdmitxset(0xE3, 0x08, 0 << 3);
    hdmitxset(0xE4, 0x02, 1 << 1);
    hdmitxset(0xC5, 0x06, (1 << 2) + (0 << 1));

    if (g_dev.vars.PCLK > 150000)
        tmp = 0;
    else if (g_dev.vars.PCLK > 75000)
        tmp = 1;
    else if (g_dev.vars.PCLK > 37500)
        tmp = 2;
    else
        tmp = 3;

    tmp <<= 6;
    hdmitxset(0x21, 0xC0, tmp);

    hdmitxset(0xF3, 0x10, SPDIF_SCHMITT_TRIGGER<<4);
}

void It6265::it6265_setup_hdmi2_scdc( void )
{
    u8 EnFlagPolling;
    u8 rddata;
    u32 vclk;

    u8 scdc_success;
    u8 retry_scdc;

    if ( g_dev.cfg.InColorMode == YCbCr420 ) {
            vclk = g_dev.vars.VCLK/2;
    }
    else {
        vclk = g_dev.vars.VCLK;
    }

    // if not HDMI 2.0
    // 20160510 add (g_dev.vars.VCLK <= 320000)
    //          : the FW will force H2.0 encode if not a H2.0 EDID
    if ( (g_dev.sink.HDMIVer < 2) && ( vclk <= ((u32)320000)) ) {

        g_dev.cfg.EnH2Enc = false;
        g_dev.cfg.EnH2Scr = false;
        g_dev.cfg.H2ClkRatio = false;
    } else {
        if ( vclk <= ((u32)340000) ) {
            // check if sink support  LTE_340Mcsc ( accept scrambling less than 3.4G )
            g_dev.cfg.EnH2Enc &= g_dev.sink.lte_340M_csc_scramble;
            g_dev.cfg.EnH2Scr = g_dev.cfg.EnH2Enc;
            g_dev.cfg.H2ClkRatio = false;
        } else {
            g_dev.cfg.EnH2Enc = true;
            g_dev.cfg.EnH2Scr = true;
            g_dev.cfg.H2ClkRatio = true;
        }

    }

    EnFlagPolling =  g_dev.sink.scdc_present;

    // When EnH2DetRR is enabled, EnFlagPolling is disabled.
    if (g_dev.sink.rr_capable) {
        EnFlagPolling = false;
    }

    hdmitxset(0x83, 0x04, (g_dev.cfg.H2ClkRatio << 2)); // HDMI2ONPLL
    hdmitxset(0xC0, 0x46, (g_dev.cfg.H2ClkRatio << 6) + (g_dev.cfg.EnH2Scr << 2) + (g_dev.cfg.EnH2Enc << 1));

    hdmitxset(0x3A, 0x03, (g_dev.sink.rr_capable << 1) + EnFlagPolling);

    if (EnFlagPolling)
        hdmitxset(0x1E, 0x10, 0x10);   // Enable SCDC update flag change interrupt

    if (g_dev.sink.rr_capable)
        hdmitxset(0x1E, 0x20, 0x20);   // Enable SCDC detect read request interrupt

    retry_scdc = 0;
__RETRY_SCDC:
    scdc_success = 0;

    if (g_dev.cfg.EnH2Enc /* &&  g_dev.sink.scdc_present*/ ) {
        it6265_scdc_write(0x02, 0x01);
        if (it6265_scdc_read(0x02, 1) == true) {
            rddata = hdmitxrd(0x30);
            pr_info("RX SCDC Sink Version = 0x%02X\r\n", rddata);
        }

        it6265_scdc_write(0x20, (g_dev.cfg.H2ClkRatio << 1) + (g_dev.cfg.EnH2Scr << 0));
        g_dev.vars.prev_clock_ratio = 1;
        if (it6265_scdc_read(0x20, 1) == true) {
            rddata = hdmitxrd(0x30);
            pr_info("RX SCDC H2ClkRatio=%d, EnH2Scr=%d\r\n", (rddata & 0x02) >> 1, rddata & 0x01);

            if ( ((rddata & 0x01)==(g_dev.cfg.EnH2Scr&0x01)) && (((rddata & 0x02) >> 1)==(g_dev.cfg.H2ClkRatio&0x01)) )
            {
                scdc_success = 1;
            }

        }

    } else if ((g_dev.vars.prev_clock_ratio) || ((g_dev.sink.HDMIVer >= 2) &&  g_dev.sink.scdc_present)) {
        it6265_scdc_write(0x20, 0x00);
        if (it6265_scdc_read(0x20, 1) == true) {
            rddata = hdmitxrd(0x30);
            pr_info("RX SCDC H2ClkRatio=%d, EnH2Scr=%d\r\n", (rddata & 0x02) >> 1, rddata & 0x01);

            if ( ((rddata & 0x03)==0) )
            {
                scdc_success = 1;
            }
        }
    }

    if ( scdc_success == 0 )
    {
        if ( retry_scdc < 3)
        {
            it6265_ddc_reset();
            retry_scdc++;
            goto __RETRY_SCDC;
        }
    }

    if (g_dev.sink.rr_capable) {
        if ( it6265_scdc_set_rr(1) ) {
            idle(5);
            it6265_scdc_status_check();
        }
    }
    else if ((g_dev.sink.HDMIVer >= 2) &&  g_dev.sink.scdc_present) {
        it6265_scdc_set_rr(0);
    }

    pr_info("\r\nCurrent Setting: VBOLaneNum=%d, VBOByteMode=%d, H2ClkRatio=%d, EnH2Enc=%d, EnH2Scr=%d\r\n\r\n",
            (int)g_dev.vars.VBOLaneNum, (int)g_dev.vars.VBOByteMode, (int)g_dev.cfg.H2ClkRatio, (int)g_dev.cfg.EnH2Enc, (int)g_dev.cfg.EnH2Scr);
}

bool It6265::it6265_check_audio_config( void )
{
    bool result = false;

    if (g_dev.cfg.AudType == LPCM) {
        switch (g_dev.cfg.AudFmt) {
        case AUD32K:
        case AUD64K:
        case AUD128K:
        case AUD44K:
        case AUD88K:
        case AUD176K:
        case AUD48K:
        case AUD96K:
        case AUD192K:
            result = true;
            break;
        default:
            pr_err("ERROR: Incorrect LPCM Audio Sampling Frequency !!!\r\n");
            break;
        }
    } else if (g_dev.cfg.AudType == NLPCM) {
        switch (g_dev.cfg.AudFmt) {
        case AUD32K:
        case AUD44K:
        case AUD48K:
        case AUD88K:
        case AUD96K:
        case AUD176K:
        case AUD192K:
            result = true;
            break;
        default:
            pr_err("ERROR: Incorrect NLPCM Audio Sampling Frequency !!!\r\n");
            break;
        }
    } else if (g_dev.cfg.AudType == DSD) {
        switch (g_dev.cfg.AudFmt) {
        case AUD32K:
        case AUD44K:
        case AUD48K:
        case AUD88K:
        case AUD96K:
        case AUD176K:
        case AUD192K:
            result = true;
            break;
        default:
            pr_err("ERROR: Incorrect DSD Audio Sampling Frequency !!!\r\n");
            break;
        }
    } else if (g_dev.cfg.AudType == HBR) {
        switch (g_dev.cfg.AudFmt) {
        case AUD256K:
        case AUD352K:
        case AUD384K:
        case AUD512K:
        case AUD705K:
        case AUD768K:
        case AUD1024K:
        case AUD1411K:
        case AUD1536K:
            result = true;
            break;
        default:
            pr_err("ERROR: Incorrect HBR Audio Sampling Frequency !!!\r\n");
            break;
        }
    } else {
        pr_err("ERROR: Unknown Audio Type !!!\r\n");
        result = false;
    }

    if (g_dev.cfg.EnMSAud == true && (g_dev.cfg.AudCh == 1 || g_dev.cfg.AudCh == 2)) {
        pr_err("ERROR: Incorrect Multi-Stream Audio Channel Setting !!!\r\n");
        result = false;
    }

    return result;
}

void It6265::it6265_audio_state(AudState_Type NewState)
{
    int i;
    u8 rddata;

    if ( g_dev.cfg.EnHDMI == 0 || g_dev.cfg.AudEn_ori == false )
    {
        return;
    }

    if (g_dev.vars.AState == NewState) {
        if (NewState != Aud_Reset) {
            return;
        }
    }
    g_dev.vars.AState = NewState;

    switch (g_dev.vars.AState) {
    case Aud_Reset :
        pr_info2("AudState change to Reset state\r\n");
        hdmitxset(0x05, 0xA2, 0xA2);    // Audio Clock Domain Reset
        hdmitxset(0xC7, 0x3C, 0x00);    // Disable AudInfoFrame and AudMetaInfo
        hdmitxset(0x19, 0x20, 0x00);    // Disable Audio FIFO OverFlow Interrupt
        hdmitxset(0x1A, 0x40, 0x00);    // Disable Audio CTS Error Interrupt
        hdmitxset(0x1D, 0x03, 0x00);    // Disable Audio Decode Error and No Audio Interrupt
        break;

    case Aud_WaitForAudIn :
        pr_info2("AudState change to WaitForAudIn state\r\n");
        it6265_set_audio_format();
        it6265_set_audio_option();
        hdmitxset(0x25, 0x02, 0x00);
        hdmitxset(0x05, 0x82, 0x00);    // Release Audio Clock Domain Reset and SPDIF OverSampling Reset
                                        // Enable Synchronous Audio Reset
        hdmitxset(0xC7, 0x3C, 0x00);    // Disable AudInfoFrame and AudMetaInfo

        for (i = 0; i < 10; i++) {
            if ((hdmitxrd(0xEF) & 0x40) == 0x00) {
                rddata = hdmitxrd(0xEE);
                if (hdmitxrd(0xEE) == rddata && rddata != 0xFF) {
                    g_dev.vars.AState = Aud_AudInStable;
                } else {
                    pr_info2("Wait For Audio Input Stable !!!\r\n\r\n");
                }
            }
            idle(2);
        }

        if (g_dev.vars.AState != Aud_AudInStable) {
            it6265_avi_task_wait_for_audio_stable();
            break;
        }

    case Aud_AudInStable :
        pr_info2("AudState change to AudInStable state\r\n");

    case Aud_AudOutSetup :
        pr_info2("AudState change to AudOutSetup State state\r\n");
        hdmitxset(0x05, 0x20, 0x00);    // Release Synchronous Audio Reset
        hdmitxset(0xC7, 0x0C, 0x0C);    // Enable Audio InfoFrame
        if (g_dev.cfg.En3DAud || g_dev.cfg.EnMSAud) {
            hdmitxset(0xC7, 0x30, 0x30); // Enable AudMetaInfo
        }


    case Aud_AudOutEnable :
        pr_info2("AudState change to AudOutEnable State state\r\n");
        hdmitxset(0x19, 0x20, 0x20);    // Enable Audio FIFO OverFlow Interrupt
        hdmitxset(0x10, 0x0F, 0x08);    // Select Audio CTS Packet Interrupt
        hdmitxset(0x1A, 0x40, 0x40);    // Enable Audio CTS Error Interrupt
        hdmitxwr(0x16, 0x03);           // Clear Audio Decode Error and No Audio Interrupt
        hdmitxset(0x1D, 0x03, 0x03);    // Enable Audio Decode Error and No Audio Interrupt
        it6265_avi_task_show_audio_info();
        break;

    default :
        pr_err("ERROR: AudState change to Unknown state !!!\r\n");
        break;
    }
}

void It6265::it6265_show_vbo_input_info( void )
{
#if SHOW_VBORX_INPUT_INFO
 u8 Reg110, Reg111, Reg112, Reg113, Reg114, Reg115;
 u8 Reg118, Reg119, Reg11A, Reg11B, Reg11C, Reg11D, Reg11E, Reg11F;
 u8 Reg120, Reg121, Reg122, Reg123, Reg126, Reg127;
 u8 Reg128, Reg129, Reg12A, Reg12B, Reg12C, Reg12D, Reg12E;

 int HTotal, HDES, HDEE, HDEW, HSyncS, HSyncE, HSyncW, VTotal, VDES, VDEE, VDEW;
 int VDES2nd, VDEE2nd, VDEW2nd, VSyncS, VSyncE, VSyncW, VSyncS2nd, VSyncE2nd, VSyncW2nd, VSyncRise, Intrlaced;
 int HSyncPol, VSyncPol, HFP, HBP, VFP, VBP, VFPH2nd, VBPH2nd;

     idle(10);

     chgrxbank(1);
     Reg110 = vborxrd(0x10);
     Reg111 = vborxrd(0x11);
     Reg112 = vborxrd(0x12);
     Reg113 = vborxrd(0x13);
     Reg114 = vborxrd(0x14);
     Reg115 = vborxrd(0x15);
     //Reg116 = vborxrd(0x16);
     //Reg117 = vborxrd(0x17);
     Reg118 = vborxrd(0x18);
     Reg119 = vborxrd(0x19);
     Reg11A = vborxrd(0x1A);
     Reg11B = vborxrd(0x1B);
     Reg11C = vborxrd(0x1C);
     Reg11D = vborxrd(0x1D);
     Reg11E = vborxrd(0x1E);
     Reg11F = vborxrd(0x1F);
     Reg120 = vborxrd(0x20);
     Reg121 = vborxrd(0x21);
     Reg122 = vborxrd(0x22);
     Reg123 = vborxrd(0x23);
     //Reg124 = vborxrd(0x24);
     //Reg125 = vborxrd(0x25);
     Reg126 = vborxrd(0x26);
     Reg127 = vborxrd(0x27);
     Reg128 = vborxrd(0x28);
     Reg129 = vborxrd(0x29);
     Reg12A = vborxrd(0x2A);
     Reg12B = vborxrd(0x2B);
     Reg12C = vborxrd(0x2C);
     Reg12D = vborxrd(0x2D);
     Reg12E = vborxrd(0x2E);
     chgrxbank(0);

     HTotal     = (Reg111<<8)+Reg110+1;
     HDES       = (Reg113<<8)+Reg112;
     HDEE       = (Reg115<<8)+Reg114;
     HSyncS     = 0;
     HSyncE     = (Reg119<<8)+Reg118;
     VTotal     = (Reg11B<<8)+Reg11A+1;
     VDES       = (Reg11D<<8)+Reg11C;
     VDEE       = (Reg11F<<8)+Reg11E;
     VDES2nd    = (Reg121<<8)+Reg120;
     VDEE2nd    = (Reg123<<8)+Reg122;
     VSyncS     = 0;
     VSyncE     = (Reg127<<8)+Reg126;
     VSyncS2nd  = (Reg129<<8)+Reg128;
     VSyncE2nd  = (Reg12B<<8)+Reg12A;
     VSyncRise  = (Reg12D<<8)+Reg12C;
     Intrlaced  = (Reg12E&0x04)>>2;

     HSyncPol   = Reg12E&0x01;
     VSyncPol   = (Reg12E&0x02)>>1;

     HDEW       = HDEE - HDES;
     HSyncW     = HSyncE - HSyncS + 1;
     VDEW       = VDEE - VDES;
     VSyncW     = VSyncE - VSyncS + 1;
     VDEW2nd    = VDEE2nd - VDES2nd;
     VSyncW2nd  = VSyncE2nd - VSyncS2nd + 1;

     HBP        = HDES - HSyncW + 1;
     HFP        = HTotal - HDEW - HSyncW - HBP;
     VBP        = VDES - VSyncW + 1;
     VFP        = VTotal - VDEW - VSyncW - VBP;

     pr_info2("\r\nShow VBORX Video Input Timing...\r\n");
     pr_info2("HTotal     = %d\r\n", HTotal   );
     pr_info2("HDEW       = %d\r\n", HDEW     );
     pr_info2("HFP        = %d\r\n", HFP      );
     pr_info2("HSyncW     = %d\r\n", HSyncW   );
     pr_info2("HBP        = %d\r\n", HBP      );

     pr_info2("VTotal     = %d\r\n", VTotal   );
     pr_info2("VDEW       = %d\r\n", VDEW     );
     pr_info2("VFP        = %d\r\n", VFP      );
     pr_info2("VSyncW     = %d\r\n", VSyncW   );
     pr_info2("VBP        = %d\r\n", VBP      );

     pr_info2("HSyncPol   = %d\r\n", HSyncPol);
     pr_info2("VSyncPol   = %d\r\n", VSyncPol);
     pr_info2("Intrlaced  = %d\r\n", Intrlaced);

     if( Intrlaced ) {
         VDEW2nd = VDEE2nd-VDES2nd;
         VFPH2nd = VSyncS2nd-VDEE;
         VBPH2nd = VDES2nd-VSyncS2nd-VSyncW2nd;

         pr_info2("VDEW2nd    = %d\r\n", VDEW2nd);
         pr_info2("VFPH2nd    = %d\r\n", VFPH2nd);
         pr_info2("VSyncW2nd  = %d\r\n", VSyncW2nd);
         pr_info2("VBPH2nd    = %d\r\n", VBPH2nd);
         pr_info2("VSyncRise  = %d\r\n", VSyncRise);
     }

     pr_info2("\r\n");
#endif
}

void It6265::it6265_show_vbo_output_info( void )
{
#if SHOW_VBORX_OUTPUT_INFO
 u8 Reg130, Reg131, Reg132, Reg133, Reg134, Reg135, Reg136, Reg137;
 u8 Reg13A, Reg13B, Reg13C, Reg13D, Reg13E, Reg13F;
 u8 Reg140, Reg141, Reg142, Reg143, Reg146;
 u8 Reg14A, Reg14C, Reg14D, Reg14E;

 int HTotal, HDES, HDEE, HDEW, HSyncW, VTotal, VDES, VDEE, VDEW;
 int VDES2nd, VDEE2nd, VDEW2nd, VSyncW, VSyncW2nd, VSyncRise, Intrlaced;
 //int VSyncS2nd;
 int HSyncPol, VSyncPol, HFP, HBP, VFP, VBP;

     idle(10);

     chgrxbank(1);
     vborxset(0x4F, 0x80, 0x80);
     Reg130 = vborxrd(0x30);
     Reg131 = vborxrd(0x31);
     Reg132 = vborxrd(0x32);
     Reg133 = vborxrd(0x33);
     Reg134 = vborxrd(0x34);
     Reg135 = vborxrd(0x35);
     Reg136 = vborxrd(0x36);
     Reg137 = vborxrd(0x37);
     //Reg138 = vborxrd(0x38);
     //Reg139 = vborxrd(0x39);
     Reg13A = vborxrd(0x3A);
     Reg13B = vborxrd(0x3B);
     Reg13C = vborxrd(0x3C);
     Reg13D = vborxrd(0x3D);
     Reg13E = vborxrd(0x3E);
     Reg13F = vborxrd(0x3F);
     Reg140 = vborxrd(0x40);
     Reg141 = vborxrd(0x41);
     Reg142 = vborxrd(0x42);
     Reg143 = vborxrd(0x43);
     //Reg144 = vborxrd(0x44);
     //Reg145 = vborxrd(0x45);
     Reg146 = vborxrd(0x46);
     //Reg147 = vborxrd(0x47);
     //Reg148 = vborxrd(0x48);
     //Reg149 = vborxrd(0x49);
     Reg14A = vborxrd(0x4A);
     //Reg14B = vborxrd(0x4B);
     Reg14C = vborxrd(0x4C);
     Reg14D = vborxrd(0x4D);
     Reg14E = vborxrd(0x4E);
     vborxset(0x4F, 0x80, 0x00);
     chgrxbank(0);

     HSyncPol = (Reg14E&0x01)>>0;
     VSyncPol = (Reg14E&0x02)>>1;

     HTotal = (Reg131<<8)+Reg130;
     HDES   = (Reg133<<8)+Reg132;
     HDEE   = (Reg135<<8)+Reg134;
     HDEW   = HDEE - HDES;
     HSyncW = (Reg137<<8)+Reg136;

     VTotal = (Reg13B<<8)+Reg13A;
     VDES   = (Reg13D<<8)+Reg13C;
     VDEE   = (Reg13F<<8)+Reg13E;
     VDEW   = VDEE - VDES;
     VSyncW = Reg146;

     HBP        = HDES - HSyncW;
     HFP        = HTotal - HDEW - HSyncW - HBP;
     VBP        = VDES - VSyncW;
     VFP        = VTotal - VDEW - VSyncW - VBP;

     Intrlaced = (Reg14E&0x04)>>2;

     if( Intrlaced ) {
         VDES2nd    = (Reg141<<8)+Reg140;
         VDEE2nd    = (Reg143<<8)+Reg142;
         VDEW2nd    = VDEE2nd - VDES2nd;
         //VSyncS2nd  = (Reg149<<8)+Reg148;
         VSyncW2nd  = Reg14A;
         VSyncRise  = (Reg14D<<8)+Reg14C;
     }

     pr_info2("\r\nShow VBORX Video Output Timing...\r\n");
     pr_info2("HTotal     = %d\r\n", HTotal   );
     pr_info2("HDEW       = %d\r\n", HDEW     );
     pr_info2("HFP        = %d\r\n", HFP      );
     pr_info2("HSyncW     = %d\r\n", HSyncW   );
     pr_info2("HBP        = %d\r\n", HBP      );

     pr_info2("VTotal     = %d\r\n", VTotal   );
     pr_info2("VDEW       = %d\r\n", VDEW     );
     pr_info2("VFP        = %d\r\n", VFP      );
     pr_info2("VSyncW     = %d\r\n", VSyncW   );
     pr_info2("VBP        = %d\r\n", VBP      );

     pr_info2("HSyncPol   = %d\r\n", HSyncPol);
     pr_info2("VSyncPol   = %d\r\n", VSyncPol);
     pr_info2("Intrlaced  = %d\r\n", Intrlaced);

     if( Intrlaced ) {
         pr_info2("VDEW2nd    = %d\r\n", VDEW2nd  );
         pr_info2("VSyncW2nd  = %d\r\n", VSyncW2nd);
         pr_info2("VSyncRise  = %d\r\n", VSyncRise);
     }

     pr_info2("\r\n");
#endif
}

void It6265::it6265_show_audio_cts(void)
{
#if 1
    u32 ACLK, audN, aud_cts_cal, aud_freq_cnt, aud_cts_cnt, tmp;

    chgtxbank( 1 );
    audN = hdmitxrd(0x33);
    tmp = hdmitxrd(0x34);
    audN |= (tmp<<8);
    tmp =hdmitxrd(0x35)&0x0F;
    audN |= (tmp<<16);

    aud_cts_cnt = ((hdmitxrd(0x35)&0xF0)>>4);
    tmp = hdmitxrd(0x36);
    aud_cts_cnt |= (tmp<<4);
    tmp = hdmitxrd(0x37);
    aud_cts_cnt |= (tmp<<12);
    chgtxbank( 0 );

    switch ( g_dev.cfg.AudFmt )
    {
        case AUD32K  :
            ACLK = 320; break;
        case AUD44K  :
            ACLK = 441; break;
        case AUD48K  :
            ACLK = 480; break;
        case AUD64K  :
        case AUD256K :
            ACLK = 640; break;
        case AUD88K  :
        case AUD352K :
            ACLK = 882; break;
        case AUD96K  :
        case AUD384K :
            ACLK = 960; break;
        case AUD128K :
        case AUD512K :
            ACLK = 1280; break;
        case AUD176K :
        case AUD705K :
            ACLK = 1764; break;
        case AUD192K :
        case AUD768K :
            ACLK = 1920; break;
        case AUD1024K:
            ACLK = 2560; break;
        case AUD1411K:
            ACLK = 3528; break;
        case AUD1536K:
            ACLK = 3840; break;
        default :
            pr_info2( "ACLK Error!!!\r\n" );
    }

    aud_cts_cal = (u32)(g_dev.vars.VCLK*audN/12.8/ACLK);

    pr_info2( "Audio Time Stamp N = 0x%X\r\n", (int)audN );
    pr_info2( "Audio Time Stamp CTSCal = 0x%X\r\n", (int)aud_cts_cal );
    pr_info2( "Audio Time Stamp CTSCnt = 0x%X\r\n", (int)aud_cts_cnt );
    pr_info2( "\r\n" );

    aud_freq_cnt = hdmitxrd(0xEE);

    if ( (hdmitxrd(0x21)&0x03) == 0 ) aud_freq_cnt *= 2;

    pr_info2( "Audio Frequecncy Count = 0x%X\r\n", aud_freq_cnt );
    if ( g_dev.cfg.AudType == DSD )
    {
        tmp = 28*1000*g_dev.vars.RCLK / 2 / aud_freq_cnt / 64;
        pr_info2( "Input Audio Frequency = %u.%u KHz\r\n\r\n", tmp/1000, tmp%1000 );
    }
    else
    {
        tmp = 1000*g_dev.vars.RCLK / 2 / aud_freq_cnt;
        pr_info2( "Input Audio Frequency = %u.%u KHz\r\n\r\n", tmp/1000, tmp%1000 );
    }

    if ( g_dev.cfg.AudSel == SPDIF && g_dev.cfg.EnMCLKSample )
    {
         pr_info2( "Enable SPDIF MCLK-Sampling Mode \r\n" );
    }
    else if ( g_dev.cfg.AudSel == SPDIF && g_dev.cfg.EnhSpdifMode && g_dev.cfg.AudType != DSD )
    {
        u16 RefOSFreqCnt;
        RefOSFreqCnt = hdmitxrd( 0xE5 );
        RefOSFreqCnt += ((hdmitxrd( 0xE6 ) & 0x3F) << 8);
        pr_info2( "Enable Enhanced SPDIF Over-Sampling Mode \r\n" );
        pr_info2( "Current RefAutoOSCLKSel=%d, RefASCLKDiv2=%d, RefASCLKDiv4=%d, RefOSFreqCnt=0x%04X\r\n",
                (hdmitxrd( 0x21 ) & 0x30) >> 4,
                (hdmitxrd( 0x23 ) & 0x02) >> 1,
                (hdmitxrd( 0x23 ) & 0x04) >> 2, RefOSFreqCnt );

        if ( (RefOSFreqCnt >> 6) > 8 && (hdmitxrd( 0x23 ) & 0x06) == 0x00 )
        {
            pr_info2( "Over-sampling frequency is too high, should use ASCLK Division !!!\r\n" );
            hdmitxset( 0x05, 0x80, 0x80 );
            hdmitxset( 0x05, 0x80, 0x00 );
        }
        pr_info2( "\r\n" );
    }

#endif
}

void It6265::it6265_show_txvid_info( void )
{
#if SHOW_HDMI_OUTPUT_INFO
    VTiming CurVTiming;
    u8 Reg210, Reg211, Reg212, Reg213, Reg214, Reg215, Reg216, Reg217;
    u8 Reg21A, Reg21B, Reg21C, Reg21D, Reg21E, Reg21F;
    u8 Reg220, Reg221, Reg222, Reg223, Reg226;
    u8 Reg228, Reg229, Reg22A, Reg22C, Reg22D, Reg22E, RegD7;

    int HSyncPol, VSyncPol, InterLaced;
    int HTotal, HDES, HDEE, HSYNCW, H2ndVRRise, HBlank;
    int VTotal, VDES, VDEE, VSYNCW, VDES2nd, VDEE2nd, VSYNCW2nd, VRS2nd;
    int VDEW2nd, VFPH2nd, VBPH2nd;
    int TMDSHTotal, TMDSHBlank;
    u8 index;

    idle(10);

    chgtxbank(2);
    hdmitxset(0x2F, 0x80, 0x80);    // Enable Video Timing Read Back

    Reg210 = hdmitxrd(0x10);
    Reg211 = hdmitxrd(0x11);
    Reg212 = hdmitxrd(0x12);
    Reg213 = hdmitxrd(0x13);
    Reg214 = hdmitxrd(0x14);
    Reg215 = hdmitxrd(0x15);
    Reg216 = hdmitxrd(0x16);
    Reg217 = hdmitxrd(0x17);
    //Reg218 = hdmitxrd(0x18);
    //Reg219 = hdmitxrd(0x19);
    Reg21A = hdmitxrd(0x1A);
    Reg21B = hdmitxrd(0x1B);
    Reg21C = hdmitxrd(0x1C);
    Reg21D = hdmitxrd(0x1D);
    Reg21E = hdmitxrd(0x1E);
    Reg21F = hdmitxrd(0x1F);
    Reg220 = hdmitxrd(0x20);
    Reg221 = hdmitxrd(0x21);
    Reg222 = hdmitxrd(0x22);
    Reg223 = hdmitxrd(0x23);
    //Reg224 = hdmitxrd(0x24);
    //Reg225 = hdmitxrd(0x25);
    Reg226 = hdmitxrd(0x26);
    //Reg227 = hdmitxrd(0x27);
    Reg228 = hdmitxrd(0x28);
    Reg229 = hdmitxrd(0x29);
    Reg22A = hdmitxrd(0x2A);
    //Reg22B = hdmitxrd(0x2B);
    Reg22C = hdmitxrd(0x2C);
    Reg22D = hdmitxrd(0x2D);
    Reg22E = hdmitxrd(0x2E);

    HSyncPol = (Reg22E & 0x01) >> 0;
    VSyncPol = (Reg22E & 0x02) >> 1;

    HTotal = (Reg211 << 8) + Reg210;
    HDES   = (Reg213 << 8) + Reg212;
    HDEE   = (Reg215 << 8) + Reg214;
    HSYNCW = (Reg217 << 8) + Reg216;

    VTotal = (Reg21B << 8) + Reg21A;
    VDES   = (Reg21D << 8) + Reg21C;
    VDEE   = (Reg21F << 8) + Reg21E;
    VSYNCW = Reg226;

    InterLaced = (Reg22E & 0x04) >> 2;

    if (InterLaced) {
        VDES2nd    = (Reg221 << 8) + Reg220;
        VDEE2nd    = (Reg223 << 8) + Reg222;
        VRS2nd     = (Reg229 << 8) + Reg228;
        VSYNCW2nd  = Reg22A;
        H2ndVRRise = (Reg22D << 8) + Reg22C;
    }
    hdmitxset(0x2F, 0x80, 0x00);    // Disable Video Timing Read Back
    chgtxbank(0);

    CurVTiming.HTotal = HTotal;
    CurVTiming.HActive = HDEE - HDES;
    CurVTiming.HBackPorch = HDES - HSYNCW;
    CurVTiming.HSyncWidth = HSYNCW;
    CurVTiming.HFrontPorch = HTotal - CurVTiming.HActive - CurVTiming.HBackPorch - CurVTiming.HSyncWidth;

    HBlank = CurVTiming.HBackPorch + CurVTiming.HSyncWidth + CurVTiming.HFrontPorch;

    if (InterLaced)
        CurVTiming.VTotal = VTotal / 2;
    else
        CurVTiming.VTotal = VTotal;
    CurVTiming.VActive = VDEE - VDES;
    CurVTiming.VBackPorch = VDES - VSYNCW;
    CurVTiming.VSyncWidth = VSYNCW;
    CurVTiming.VFrontPorch = CurVTiming.VTotal - CurVTiming.VActive - CurVTiming.VBackPorch - CurVTiming.VSyncWidth;

    CurVTiming.ScanMode = (!InterLaced);
    CurVTiming.VPolarity = VSyncPol;
    CurVTiming.HPolarity = HSyncPol;
    CurVTiming.PCLK = ((int)g_dev.vars.TXCLK) * 2;
    CurVTiming.PixRpt = g_dev.vars.PixRpt;
    //CurVTiming.PicAR = SetVTiming.PicAR;

    g_dev.vars.HActive = CurVTiming.HActive;
    g_dev.vars.VActive = CurVTiming.VActive;

    if ( g_dev.cfg.AutoVIC ) {
        index = it6265_get_vic_by_timing( &CurVTiming );

        if ( index ) {
            g_dev.input.VIC = index;
        }

        if ( index && g_dev.cfg.En3D == 0 && g_dev.cfg.En4Kx2K == true )
        {


            switch ( index )
            {
                case 95: g_dev.vars.HDMI_VIC = 0x01; break;
                case 94: g_dev.vars.HDMI_VIC = 0x02; break;
                case 93: g_dev.vars.HDMI_VIC = 0x03; break;
                case 98: g_dev.vars.HDMI_VIC = 0x04; break;
                default: g_dev.vars.HDMI_VIC = 0x00; break;
            }
            switch ( index )
            {
                case 93:
                case 94:
                case 95:
                case 98:
                    g_dev.cfg.En4Kx2K = true;
                    break;
                default:
                    g_dev.cfg.En4Kx2K = false;
                    break;
            }
        }
    }
    else{
        index = g_dev.input.VIC;
    }


    switch (g_dev.cfg.ColorDepth) {
    case VID10BIT :
        pr_info2("\r\nColorDepth = 10bpc\r\n"); break;
    case VID12BIT :
        pr_info2("\r\nColorDepth = 12bpc\r\n"); break;
    default :
        pr_info2("\r\nColorDepth = 8bpc\r\n"); break;
    }

    switch (g_dev.cfg.InColorMode) {
    case RGB444 :
        pr_info2("InColorMode = RGB444\r\n"); break;
    case YCbCr422 :
        pr_info2("InColorMode = YCbCr422\r\n"); break;
    case YCbCr444 :
        pr_info2("InColorMode = YCbCr444\r\n"); break;
    default :
        pr_info2("InColorMode = YCbCr420\r\n"); break;
    }

    switch (g_dev.cfg.OutColorMode) {
    case RGB444 :
        pr_info2("OutColorMode = RGB444\r\n"); break;
    case YCbCr422 :
        pr_info2("OutColorMode = YCbCr422\r\n"); break;
    case YCbCr444 :
        pr_info2("OutColorMode = YCbCr444\r\n"); break;
    default :
        pr_info2("OutColorMode = YCbCr420\r\n"); break;
    }

    if (g_dev.cfg.DynRange)
        pr_info2("Dynamic Range = CEA (Limit Range)\r\n");
    else
        pr_info2("Dynamic Range = VESA (Full Range)\r\n");

    if (g_dev.cfg.InColorMode) {

        switch ( g_dev.cfg.YCbCrCoef )
        {
            case ITU601:
                pr_info2("YCbCr Colorimetry = ITU-R BT601-5\r\n");
                break;
            case ITU709:
                pr_info2("YCbCr Colorimetry = ITU-R BT709-5\r\n");
                break;
            case ITU2020:
                pr_info2("YCbCr Colorimetry = ITU-R BT2020\r\n");
                break;
            default:
                pr_info2("YCbCr Colorimetry = Unknown\r\n");
                break;
        }
    }

     if( g_dev.cfg.EnExtCol )
         switch( g_dev.cfg.ExtColFmt ) {
         case xvYCC601    : pr_info2("Enable Extended Colorimetry => xvYCC601\r\n"); break;
         case xvYCC709    : pr_info2("Enable Extended Colorimetry => xvYCC709\r\n"); break;
         case sYCC601     : pr_info2("Enable Extended Colorimetry => sYCC601\r\n"); break;
         case AdobeYCC601 : pr_info2("Enable Extended Colorimetry => AdobeYCC601\r\n"); break;
         case AdobeRGB    : pr_info2("Enable Extended Colorimetry => AdobeRGB\r\n"); break;
         case BT2020cYCC  : pr_info2("Enable Extended Colorimetry => BT2020cYCC\r\n"); break;
         case BT2020YCC   : pr_info2("Enable Extended Colorimetry => BT2020YCC\r\n"); break;
         default          : pr_info2("Unsupport Extended Colorimetry !!!\r\n"); break;
     }


    if (g_dev.cfg.EnHDMI)
        pr_info2("\r\nHDMI ");
    else
        pr_info2("\r\nDVI ");

    pr_info2("\r\nVIC        = %d\r\n", (int)index);
    pr_info2("PCLK       = %u kHz\r\n", g_dev.vars.TXCLK * 2);
    pr_info2("HTotal     = %d\r\n", (int)CurVTiming.HTotal);
    pr_info2("HDEW       = %d\r\n", (int)CurVTiming.HActive);
    pr_info2("HFPH       = %d\r\n", (int)CurVTiming.HFrontPorch);
    pr_info2("HSyncW     = %d\r\n", (int)CurVTiming.HSyncWidth);
    pr_info2("HBPH       = %d\r\n", (int)CurVTiming.HBackPorch);
    pr_info2("HSyncPol   = %d\r\n", (int)CurVTiming.HPolarity);
    if (InterLaced)
        pr_info2("VTotal     = %d\r\n", VTotal);
    else
        pr_info2("VTotal     = %d\r\n", (int)CurVTiming.VTotal);
    pr_info2("VDEW       = %d\r\n", (int)CurVTiming.VActive);
    pr_info2("VFPH       = %d\r\n", (int)CurVTiming.VFrontPorch);
    pr_info2("VSyncW     = %d\r\n", (int)CurVTiming.VSyncWidth);
    pr_info2("VBPH       = %d\r\n", (int)CurVTiming.VBackPorch);
    pr_info2("VSyncPol   = %d\r\n", (int)CurVTiming.VPolarity);
    pr_info2("InterLaced = %d\r\n\r\n", (int)(!CurVTiming.ScanMode));

    if (InterLaced) {
        VDEW2nd = VDEE2nd - VDES2nd;
        VFPH2nd = VRS2nd - VDEE;
        VBPH2nd = VDES2nd - VRS2nd - VSYNCW2nd;
        pr_info2("VDEW2nd    = %d\r\n", VDEW2nd);
        pr_info2("VFPH2nd    = %d\r\n", VFPH2nd);
        pr_info2("VSyncW2nd  = %d\r\n", VSYNCW2nd);
        pr_info2("VBPH2nd    = %d\r\n", VBPH2nd);
        pr_info2("H2ndVRRise = %d\r\n", H2ndVRRise);
    }

    if (g_dev.cfg.ColorDepth != VID8BIT) {
        pr_info2("\r\n");
        TMDSHTotal = ((hdmitxrd(0xD4) & 0x3F) << 8) + hdmitxrd(0xD3);
        TMDSHBlank = ((hdmitxrd(0xD6) & 0x1F) << 8) + hdmitxrd(0xD5);

        pr_info2("TMDS HTotal = %d", (int)TMDSHTotal);
        if (TMDSHTotal != HTotal){
            pr_info2(" ==> Error !!!\r\n");
        }
        else{
            pr_info2("\r\n");
        }

        pr_info2("TMDS HBlank = %d", (int)TMDSHBlank);
        if (TMDSHBlank != HBlank) {
            pr_info2(" ==> Error !!!\r\n");
        }
        else{
            pr_info2("\r\n");
        }

        RegD7 = hdmitxrd(0xD7);
        pr_info2("DefPhase = %d, PackPhase = %d\r\n\r\n", (int)((RegD7 & 0x04) >> 2), (int)(RegD7 & 0x03));
    }

    pr_info2("\r\n");
#endif
}

void It6265::it6265_video_state(VidState_Type NewState)
{
    u8 rechk;
#if AUTO_LANE_DETECT
    u8 lane_det;
#endif
    if (g_dev.vars.VState == NewState) {
        if (NewState != Vid_Reset) {
            return;
        }
    }

    g_dev.vars.VState = NewState;

    switch (g_dev.vars.VState) {
    case Vid_Reset :
        pr_info2("VidState change to Reset state\r\n");
        if ( (g_dev.cfg.keep_vborx_connection == 0 ) || (g_dev.vars.reset_vborx) )
        {
            it6265_set_vx1_htpdn(0);
        }

        it6265_hdcp_state(CP_Reset);
        hdmitxset(0x05, 0x04, 0x04);
        break;

    case Vid_WaitForVidIn :

        #if AUTO_LANE_DETECT
        g_dev.vars.VBOLaneNum = 3;
        __RESET_LANE:
        #endif

        pr_info2("VidState change to WaitForVidIn state\r\n");

        if ( (g_dev.cfg.keep_vborx_connection == 0 ) || (g_dev.vars.reset_vborx) )
        {
            it6265_setup_vborx();

            vborxwr(0x10, 0xFF);    // Clear Interrupt
            vborxwr(0x11, 0xFF);
            vborxwr(0x12, 0xFF);
            vborxwr(0x05, 0x00);
            idle(1);

            it6265_set_vx1_htpdn(1);
            g_dev.vars.reset_vborx = 0;
        }

        it6265_audio_state(Aud_Reset);
        it6265_set_video_format();

        hdmitxset(0x88, 0x01, 0x01);
        hdmitxset(0x19, 0x80, 0x80);

        #if AUTO_LANE_DETECT
        pr_info2(" >>>> Lane Setting = %d Lane(s)\r\n\r\n", (int)g_dev.vars.lanenum);

        //idle(500);
        lane_det = it6265_vborx_lane_detect();
        if ( g_dev.vars.VBOLaneNum != lane_det ) {

            pr_err("Lane setting = %d lane(s), but detected = %d lane(s)", (int)(1<<g_dev.vars.VBOLaneNum), (int)(1<<lane_det) );
            if ( g_dev.vars.VBOLaneNum > lane_det ) {
                g_dev.vars.VBOLaneNum = lane_det;
            }
            goto __RESET_LANE;
        }
        #endif

        //if( EnPatGen==false && InClkSel==false )
        hdmitxset(0x1A, 0x38, 0x38);  // Enable InLBOvFlw, InFFOvFlw, InDESyncLose Interrupt

        hdmitxset(0x05, 0x04, 0x00);

        g_dev.vars.fifo_err_handled = 0;
        g_dev.vars.fifo_err_count = 0;

        it6265_avi_task_check_video_in_stable() ;

        break;

    case Vid_VidInStable :
        pr_info2("VidState change to VidInStable state\r\n");
        it6265_cal_txclk();
        it6265_set_video_format();
        it6265_setup_hdmitx_afe();
        it6265_setup_hdmi2_scdc();
        it6265_set_asclk();
        rechk = 10;

    __recheck:
        if (it6265_is_video_in_stable()) {
            g_dev.vars.VState = Vid_VidOutEnable;
            goto __vid_output;
        }
        else {
            if ( rechk > 0 )
            {
                idle(2);
                rechk--;
                goto __recheck;
            }
            // for the next video stable change to stable
            // we can go to Vid_VidInStable again
            g_dev.vars.VState = Vid_WaitForVidIn;
            break;
        }

        break;

    case Vid_VidPG :
        pr_info2("VidState change to Vid_VidPG state\r\n");
        vborxwr(0x14, 0x00);
        vborxwr(0x15, 0x00);
        vborxwr(0x16, 0x00);
        vborxwr(0x17, 0x00);
        vborxset(0x0c, 0x40, 0x00);
        vborxwr(0x0b, 0x00);
        vborxwr(0xb6, 0x17);
        it6265_config_hdmi_pg();

        hdmitxset(0x05, 0x04, 0x00);
        idle(100);

        it6265_cal_txclk();
        it6265_set_video_format();
        it6265_setup_hdmitx_afe();
        it6265_setup_hdmi2_scdc();
        it6265_set_asclk();

        break;

    case Vid_VidOutEnable :
    __vid_output:
        pr_info2("VidState change to VidOutEnable state\r\n");

        g_dev.cfg.keep_vborx_connection = g_dev.cfg.keep_vborx_connection_prev;

        hdmitxwr(0x16, 0x04);           // Clear Video Input FIFO Interrupt
        hdmitxset(0x1D, 0x04, 0x04);    // Enable Video Input FIFO Interrupt

        pr_info2("\r\n");
        pr_info2("RCLK=%u KHz\r\n", g_dev.vars.RCLK);
        pr_info2("ICLK=%u KHz\r\n", g_dev.vars.ICLK);
        pr_info2("PCLK=%u KHz\r\n", g_dev.vars.PCLK);
        pr_info2("TXCLK=%u KHz\r\n", g_dev.vars.TXCLK);
        pr_info2("VCLK=%u KHz\r\n", g_dev.vars.VCLK);
        pr_info2("\r\n");

        it6265_show_txvid_info();

        it6265_set_infoframe();

        hdmitxset(0xC8, 0x30, 0x30);    // Enable General Control Packet

        hdmitxset(0x1D, 0x40, 0x40);    // Enable Video Parameter Change Interrupt

        it6265_setup_hdmi2_scdc();

        hdmitxset(0x6F, 0x08, 0x08);    // Enable auto mute
        it6265_set_avmute(0);

        #if 0 // pattern mux
        chgtxbank(2);
        hdmitxwr(0x30, 0x00);
        hdmitxwr(0x31, 0x00);
        hdmitxwr(0x32, 0xff);
        hdmitxwr(0x33, 0xff);
        hdmitxwr(0x34, 0xff);
        hdmitxwr(0x35, 0xff);
        hdmitxwr(0x36, 0xff);
        hdmitxwr(0x37, 0x8);
        hdmitxwr(0x38, 0x8);
        hdmitxset(0x2F, 0x02, 0x02);
        chgtxbank(0);
        #endif

        hdmitxset(0x88, 0x01, 0x00);

        it6265_audio_state(Aud_WaitForAudIn);

        if ( g_dev.cfg.EnHDCP ) {
            idle(100);
            it6265_hdcp_state(CP_Going);
        }

//        it6265_dump_register_all();
        break;
    default :
        pr_err("ERROR: VidState change to Unknown state !!!\r\n");
        break;
    }

}

void It6265::it6265_enable_hdcp1( void )
{
    int wait_count;
    u8 BKSV[5];

    hdmitxset(0x42, 0x10, 0x00);    // RegHDCPSel=0
    hdmitxset(0x19, 0x04, 0x04);    // Enable DDC Command Fail Interrupt
    hdmitxset(0x1D, 0x08, 0x08);    // Enable DDC Bus Hang Interrupt
    hdmitxset(0x41, 0x01, 0x00);    // Disable CP_Desired
    hdmitxset(0xC2, 0x80, 0x00);

    // Reset HDCP Module
    hdmitxset(0x05, 0x40, 0x40);
    idle(2);
    hdmitxset(0x05, 0x40, 0x00);

    // set HDCP Option
    hdmitxwr(0xFF, 0xC3);
    hdmitxwr(0xFF, 0xA5);
    hdmitxset(0x41, 0x80, 0x80);
    hdmitxset(0x57, 0x01, 0x00);
    hdmitxset(0x41, 0x80, 0x00);
    hdmitxset(0xC2, 0x40, 0x00);
    hdmitxset(0xC2, 0x1F, 0x0A);    // For UITA-2000 ATC
    hdmitxset(0x6B, 0x03, (g_dev.cfg.DisRiShortRead << 1) + g_dev.cfg.DisR0ShortRead);
    hdmitxwr(0xFF, 0xFF);

    hdmitxset(0x41, 0x0E, (cfg_KeepOutOpt<<3) + (cfg_EnSyncDetChk << 2) + (g_dev.cfg.EnHDCP1p1 << 1));
    hdmitxset(0x6F, 0x0F, (cfg_EnHDCPAutoMute << 3) + (cfg_EnSyncDet2FailInt << 2) + (cfg_EnRiChk2DoneInt << 1) + cfg_EnAutoReAuth);

    if (g_dev.vars.rid < 0xA1) {
        hdmitxset(0xc0, 0x08, 0x00);
    }

    // Enable Authentication Fail/Done/KSVListChk Interrupt
    hdmitxset(0x1A, 0x07, 0x07);

    // Clear Ri/Pj Done Interrupt
    hdmitxwr(0x16, 0x30);

    // Enable Ri Done Interrupt
    hdmitxset(0x1D, 0x30, 0x10);
    // Disable Ri Done Interrupt
    // hdmitxset(0x1D, 0x30, 0x10);

    hdmitxset(0x40, 0x01, 0x01);  // Enable An Generator
    idle(1);
    hdmitxset(0x40, 0x01, 0x00);  // Stop An Generator

    #if 1
    hdmitxwr(0x48, hdmitxrd(0x50));
    hdmitxwr(0x49, hdmitxrd(0x51));
    hdmitxwr(0x4A, hdmitxrd(0x52));
    hdmitxwr(0x4B, hdmitxrd(0x53));
    hdmitxwr(0x4C, hdmitxrd(0x54));
    hdmitxwr(0x4D, hdmitxrd(0x55));
    hdmitxwr(0x4E, hdmitxrd(0x56));
    hdmitxwr(0x4F, hdmitxrd(0x57));
    #else // fixed An
    hdmitxwr(0x48, (0x50));
    hdmitxwr(0x49, (0x51));
    hdmitxwr(0x4A, (0x52));
    hdmitxwr(0x4B, (0x53));
    hdmitxwr(0x4C, (0x54));
    hdmitxwr(0x4D, (0x55));
    hdmitxwr(0x4E, (0x56));
    hdmitxwr(0x4F, (0x57));
    #endif

    //it6265_ddc_fifo_clear();

    // Enable CP_Desired
    hdmitxset(0x41, 0x01, 0x01);
    g_dev.vars.RxHDMIMode = 0;
    wait_count = 0;

    while (g_dev.cfg.EnHDMI && !g_dev.vars.RxHDMIMode) {
        if (it6265_hdcp_read(0x41, 2)) {    // BStatus;
            g_dev.vars.RxHDMIMode = (hdmitxrd(0x65) & 0x10) >> 4;
            pr_info("Enable HDCP Fire %d => Current RX HDMI MODE=%d \r\n", g_dev.vars.HDCPFireCnt++, g_dev.vars.RxHDMIMode);
        } else {
            pr_err("DDC ERROR: When waiting for RX HDMI_MODE change to 1 ...\r\n");
        }

        if (wait_count++ == 10) {
            pr_err("\r\nERROR: RX HDMI_MODE keeps in 0 Time-Out !!!\r\n\r\n");
            break;
        }

        if (false == it6265_is_hpd_high()) {
            pr_err("Abort hdcp read becasue of detecting unplug !!!\r\n");
            break;
        }
    }

    #if ENABLE_CALLBACK
    if ( g_dev.cb.it6265_hdcp_BStatus_cb ) {
        u16 bstatus;
        bstatus = hdmitxrd(0x65);
        bstatus <<= 8;
        bstatus |= hdmitxrd(0x64);

        g_dev.cb.it6265_hdcp_BStatus_cb(bstatus, g_dev.cb_data.hdcp_BStatus_cb_data);
    }
    #endif

    if (it6265_hdcp_read(0x00, 5)) {
        hdmitxbrd(0x5B, 5, &BKSV[0]);

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp_BKSV_cb ) {
            g_dev.cb.it6265_hdcp_BKSV_cb( BKSV, g_dev.cb_data.hdcp_BKSV_cb_data);
        }
        #endif

        if (it6265_hdcp_is_revocation_key(BKSV)) {
            g_dev.vars.HdcpState = CP_Fail;
        } else {
            // HDCP fire
            hdmitxset(0x42, 0x01, 0x01);
            g_dev.vars.HdcpState = CP_Going;

            #if ENABLE_CALLBACK
            if ( g_dev.cb.it6265_hdcp_start_cb ) {
                g_dev.cb.it6265_hdcp_start_cb( 1, g_dev.cb_data.hdcp_start_cb_data );
            }
            #endif
        }
    } else {
        pr_err("Failed to read BKSV !!!\r\n");
        g_dev.vars.HdcpState = CP_Fail;
    }

    if (g_dev.vars.HdcpState == CP_Fail) {
        it6265_hdcp_task_retry_auth();
    }
}

void It6265::it6265_enable_hdcp2( void )
{
    u8 RxHDCP2;

    hdmitxset(0x42, 0x10, 0x10);    // RegHDCPSel=1
    hdmitxset(0x19, 0x04, 0x04);    // Enable DDC Command Fail Interrupt
    hdmitxset(0x1D, 0x08, 0x08);    // Enable DDC Bus Hang Interrupt
    hdmitxset(0x41, 0x01, 0x00);    // Disable CP_Desired
    hdmitxset(0xC2, 0x80, 0x00);

    // Reset HDCP Module
    hdmitxset(0x05, 0x48, 0x48);
    idle(2);
    hdmitxset(0x05, 0x48, 0x00);
    hdmitxset(0x05, 0x10, 0x00);

    if ( g_dev.cfg.IsHdcpRepeater ) {
        g_dev.cfg.EnRepWaitTxMgm = 1;
        g_dev.cfg.EnRepWaitTxEks = 1;
        g_dev.cfg.EnHDCP2ListSRMChk = 1;
    }
    else{
        g_dev.cfg.EnRepWaitTxMgm = 0;
        g_dev.cfg.EnRepWaitTxEks = 0;
        g_dev.cfg.EnHDCP2ListSRMChk = 0;
    }

    // set HDCP Option
    hdmitxwr(0xFF, 0xC3);
    hdmitxwr(0xFF, 0xA5);
    hdmitxset(0x6B, 0x01, g_dev.cfg.DisR0ShortRead);
    hdmitxset(0x6B, 0x30, (g_dev.cfg.EnRepWaitTxMgm<<5)+(g_dev.cfg.EnRepWaitTxEks<<4));
    hdmitxwr(0xFF, 0xFF);

    hdmitxset(0x48, 0x0F, cfg_LClimit);
    hdmitxset(0x6F, 0x09, (cfg_EnHDCPAutoMute<<3)+cfg_EnAutoReAuth);

    if (g_dev.vars.rid < 0xA1) {
        hdmitxset(0xc0, 0x08, 0x08);
        if ( (g_dev.input.VIC == 2) || (g_dev.input.VIC == 1) ) {
            hdmitxset(0xC5, 0x02, 0x02);
        }
    }

    // Enable Authentication Fail/Done/KSVListChk Interrupt
    hdmitxset(0x1A, 0x07, 0x07);

    // Enable CP_Desired
    hdmitxset(0x41, 0x01, 0x01);

    if (it6265_hdcp_read(0x50, 1)) {        // HDCP2Version
        RxHDCP2 = hdmitxrd(0x4B) & 0x01;
        pr_info("HDCP2Version = %d \r\n", RxHDCP2);

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp_version_cb )
        {
            g_dev.cb.it6265_hdcp_version_cb( RxHDCP2+1, g_dev.cb_data.hdcp_version_cb_data);
        }
        #endif

    } else {
        pr_err("HDCP2Version read fail, set to HDCP1\r\n");
        RxHDCP2 = 0;

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp_version_cb )
        {
            g_dev.cb.it6265_hdcp_version_cb( 0, g_dev.cb_data.hdcp_version_cb_data);
        }
        #endif
    }


    #if DISABLE_HDCP22_UNDER_3G
    if ( RxHDCP2 ) {
        if ((g_dev.vars.HActive < 3800) || (g_dev.vars.VActive < 2000)) {
            pr_info("Disable HDCP2.2 for non-4k mode %ux%u\r\n", g_dev.vars.HActive, g_dev.vars.VActive);
            RxHDCP2 = 0;
        }
    }
    #endif

    #if SWITCH_TO_HDCP14_WHEN_HDCP22_FAIL
    if ( RxHDCP2 ) {
        u8 BKSV[5];
        hdmitxset(0x42, 0x10, 0x00);
    if (it6265_hdcp_read(0x00, 5)) {
            pr_info("### Key=");
            hdmitxbrd(0x5B, 5, &BKSV[0]);
            it6265_print_buffer(BKSV, 5);
            if ( 0 != memcmp( BKSV, g_dev.vars.rx_key, 5 ) ) {
                memcpy( g_dev.vars.rx_key, BKSV, 5 );
                g_dev.vars.hdcp2_fail_count = 0;
            }
        }
        hdmitxset(0x42, 0x10, 0x10);
    }

    pr_info("### HDCP2.2 FC=%d / %d\r\n", g_dev.vars.hdcp2_fail_count, MAX_HDCP2_FAIL_COUNT);
    if ( g_dev.vars.hdcp2_fail_count >= MAX_HDCP2_FAIL_COUNT )
    {
        RxHDCP2 = 0;
    }

    #endif


    // force to fire HDCP2.2
    // RxHDCP2 = 1;

    if (RxHDCP2) {
        pr_info("Support HDCP2.2 = TRUE\r\n");

        hdmitxset(0x70, 0x10, 0x10);

        // [6]:RegEnHDCP2ListSRMChk, [5]:RegEnHDCP2SRMChk
        hdmitxset(0x42, 0x60, (g_dev.cfg.EnHDCP2ListSRMChk << 6) + (g_dev.cfg.EnHDCP2ATC << 5));

        // [7]=RHDCP2RepWaitTxMgm_Int
        hdmitxset(0x1D, 0x80, 0x80);

        // [7]=RHDCP2RepWaitTxEks_Int
        // [6]=RHDCP2ListSRMChk_Int
        // [3]=RHDCP2StrmMgmFail_Int
        // [2]=RHDCP2RxReauthReq_Int
        // [1]=RHDCP2 SRMChk_Int
        hdmitxset(0x1E, 0xCE, 0xCE);

        // [7]=RegFixRandErr,
        // [6]=RegEnMrolloverChk, [5]=RegEnMFailRetry
        // [4]=RegDisMRetryLimit, [3]=RegEnSeq_num_VChk
        // [2]=RegEnPollRdyOnly,  [1:0]=RegAuthEncTol
        // RegAuthEncTol="11" will cause encryption enable before 200ms
        hdmitxset(0x4A, 0xFF, 0x26);

        // [7:6]=RegAuthPollTol
        // [5]=RegForceNo_Stored_Km, [4]=RegEnHwVerChk
        // [3]=RegMsgPollStart,      [2]=RegMsgPollGap
        // [1:0]=RegMsgPollTOTol
        hdmitxset(0x49, 0xFF, 0x00);
        hdmitxset(0x42, 0x08, 0x08);

        idle(200);

        it6265_ddc_fifo_clear();

        //hdmitxset(0x88, 0x01, 0x00);
        // HDCP2 authentication fire
        hdmitxset(0x42, 0x01, 0x01);

        g_dev.vars.hdcp_going = 1;

        //idle(5);
        //hdmitxset(0x88, 0x01, 0x00);

        g_dev.vars.HDCP2Sel = true;

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp_start_cb ) {
            g_dev.cb.it6265_hdcp_start_cb( 2, g_dev.cb_data.hdcp_start_cb_data );
        }
        #endif

    } else {
        pr_info("Support HDCP2.2 = FALSE\r\n");
        pr_info("Try HDCP1 Authentication ... \r\n");

        g_dev.vars.HDCP2Sel = false;
        it6265_enable_hdcp1();
    }
}

void It6265::it6265_hdcp_state(HdcpState_Type CPState)
{
    if (g_dev.vars.HdcpState == CPState) {
        pr_info2("HdcpState == CPState = %d\r\n", CPState);
        if (CPState != CP_Reset) {
            return;
        }
    }
    g_dev.vars.HdcpState = CPState;

    if ( g_dev.vars.HdcpState > CP_Reset ) {
        if (false == it6265_is_hpd_high()) {
            pr_info2("HDCP=> CP_Reset because HPD Low\r\n");
            CPState = CP_Reset;
            it6265_avi_task_sys_unplug();
        }
    }

    switch (g_dev.vars.HdcpState) {
    case CP_Reset:
        pr_info2("HDCP=> CP_Reset\r\n");
        g_dev.vars.hdcp_cmd_timeout = 0;
        break;

    case CP_Going:
        pr_info2("HDCP=> CP_Going\r\n");
        hdmitxset(0x6F, 0x08, 0x01);    // Enable auto mute
        hdmitxset(0x3A, 0x03, 0x00);    // disable RR and Flag polling during HDCP auth
        g_dev.vars.hdcp_cmd_timeout = 0;
        it6265_set_avmute(0);
        it6265_scdc_set_rr(0);
        if (it6265_is_video_in_stable()) {
        // clear the video stable change interrupt
        // before starting HDCP process
        hdmitxwr(0x12, 0x80);
        }

        if ( g_dev.vars.hdcp_going == 0 )
        {
            if ( it6265_disable_hdcp2() )
            {
                it6265_enable_hdcp1();
            }
            else
            {
                it6265_enable_hdcp2();
            }
        }

        break;

    case CP_Done:
        pr_info2("HDCP=> CP_Done\r\n");
        if ( g_dev.sink.HDMIVer >=2 )
        {
            if ( g_dev.sink.rr_capable )
            {
                it6265_scdc_set_rr(1);
            }
            else
            {
                hdmitxset(0x3A, 0x03, 0x01);    // enable Flag polling after HDCP auth
            }
        }
        g_dev.vars.hdcp_cmd = HDCP_NONE;
        g_dev.cfg.keep_vborx_connection = g_dev.cfg.keep_vborx_connection_prev;
        //g_dev.vars.hdcp2_fail_status = 0;
        //g_dev.vars.hdcp2_fail_status_prev = 0;
        break;

    case CP_Fail:
        pr_info2("HDCP=> CP_Fail\r\n");
        if (g_dev.vars.rid < 0xA1) {
            hdmitxset(0xc0, 0x08, 0x00);
        }
        it6265_set_avmute(1);
        if ( g_dev.sink.HDMIVer >=2 )
        {
            if ( g_dev.sink.rr_capable )
            {
                it6265_scdc_set_rr(1);
            }
            else
            {
                hdmitxset(0x3A, 0x03, 0x01);    // enable Flag polling after HDCP auth
            }
        }

        if (g_dev.vars.VState == Vid_VidOutEnable) {
            g_dev.vars.HdcpState = CP_Reset;
        }
        break;

    default :
        pr_err("ERROR: HdcpState change to Unknown state !!!\r\n");
        break;
    }
}

void It6265::it6265_reset( u8 rst_all )
{
    if (g_dev.cfg.InColorMode != YCbCr420 && g_dev.cfg.OutColorMode == YCbCr420) {
        pr_err("ERROR: Incorrect Video Format Setting !!!\r\n");
    }

    if (g_dev.cfg.InColorMode == YCbCr420 && g_dev.cfg.OutColorMode != YCbCr420 && g_dev.vars.PixRpt != 1) {
        pr_err("ERROR: YCbCr420 to 444 can not support PixRpt!=1 !!!\r\n");
    }

    it6265_check_audio_config();

    if (g_dev.cfg.EnExtCol) {
        if (g_dev.cfg.OutColorMode == RGB444 && g_dev.cfg.ExtColFmt != AdobeRGB && g_dev.cfg.ExtColFmt != BT2020YCC) {
            pr_err("ERROR: RGB444 must set ExtColFmt to AdobeRGB or BT2020YCC !!!\r\n");
        }

        if (g_dev.cfg.OutColorMode != RGB444 && g_dev.cfg.ExtColFmt == AdobeRGB) {
            pr_err("ERROR: YCbCr can not set ExtColFmt to AdobeRGB !!!\r\n");
        }
    }

    if ((g_dev.cfg.EnInDDR && g_dev.cfg.InColorMode == YCbCr422) ||
        (g_dev.cfg.EnInDDR && g_dev.cfg.En24BDDR && g_dev.cfg.InColorMode != YCbCr422))
        g_dev.vars.RegPCLKDiv2 = true;
    else
        g_dev.vars.RegPCLKDiv2 = false;

    if (g_dev.cfg.AudCh > 8) {
        g_dev.cfg.En3DAud = true;
        g_dev.cfg.EnMSAud = false;
    } else
        g_dev.cfg.En3DAud = false;

    if (g_dev.cfg.EnTDM) {
        g_dev.cfg.AudSel = I2S;
    }

    if( g_dev.cfg.EnIntPG || g_dev.cfg.EnDVMode || (g_dev.cfg.InColorMode==YCbCr420 && g_dev.cfg.OutColorMode!=YCbCr420) ) {
        g_dev.cfg.EnH2InLBBypass = false;
    }
    else{
        g_dev.cfg.EnH2InLBBypass = true;
    }

    hdmitxset(0x25, 0x01, 0x00);

    pr_err("Enable VBORX slave address !!!\r\n");
    // Enable VBORX slave address
    hdmitxwr(0xFB, ((VBORxAddr << 1) | 0x01));

    if (rst_all) {
        // VBORX PLL Reset
        vborxset(0xB0, 0x10, 0x00);
        vborxset(0xB6, 0x01, 0x00);
        vborxset(0xB7, 0x01, 0x00);
    }

    // H2TX PLL Reset
    hdmitxset(0x80, 0x80, 0x00);
    hdmitxset(0x84, 0x80, 0x00);

    idle(2);

    if (rst_all && (g_dev.cfg.EnIntPG==0)) {
        // HDMITX Reset
        hdmitxset(0x22, 0x20, 0x20);
        hdmitxset(0x05, 0x01, 0x01);
        while(0==it6265_is_device_ready()){}

        g_dev.vars.rid = hdmitxrd(0x04);
    }
    else{
        hdmitxset(0x80, 0x80, 0x80);
        hdmitxset(0x84, 0x80, 0x80);
    }

    hdmitxset(0xf1, 0x0f, 0x00);
    hdmitxset(0xf4, 0x3f, 0x00);
    idle(1);

    if ( rst_all ) {
        hdmitxset(0x05, 0xFE, 0xEE);   // Reset All but RegStorekmRst
        //hdmitxset(0x05, 0xFE, 0xFE);   // Reset All include RegStorekmRst (QD980B must reset StoreKm)
    }
    else{
        hdmitxset(0x3A, 0x13, 0x00); // disable all HW SCDC access
        if ( g_dev.cfg.EnHDCP )
        {
            hdmitxset(0xC2, 0x80, 0x80); // disable HDCP encryption
            idle(1000/24); // wait for at least 1 frame to notify the sink for encryption disabled
            hdmitxset(0x41, 0x01, 0x01); // disalbe HDCP hardware

            idle(70); // wait for the longest case of 528 bytes read in HDCP2.2
                          // remove this if you don't want to wait for the worst case
                          //
            hdmitxset(0x05, 0x40, 0x40); // do HDCP reset
            hdmitxset(0xC2, 0x80, 0x00); // enable HDCP encryption
        }
        else
        {
            idle(10); // wait for previous DDC traffic done.
        }
    }


    it6265_ddc_reset();

    g_dev.vars.AFESPEED = HIGH;   // set default AFE speed as High after reset

    if (rst_all) {
        pr_err("Enable VBORX slave address again!!!\r\n");
        // Enable VBORX slave address again
        hdmitxwr(0xFB, ((VBORxAddr << 1) | 0x01));

        // VBORX Reset
        vborxset(0x05, 0x0E, 0x0E);    // VBORX Reset
    }

    hdmitxset(0x21, 0x03, cfg_RCLKFreqSel);

    if (rst_all) {
        // VBORX Initial Setting
        vborxset(0x0F, 0x08, 0x08);
        vborxset(0x20, 0x33, (g_dev.cfg.SCLKDly << 4) + (g_dev.cfg.EnSCLKInv << 1) + g_dev.cfg.EnPCLKInv);
        vborxwr(0x21, g_dev.cfg.EnLxCLKInv);
        vborxset(0x30, 0x08, cfg_EnVBOErrPoll << 3);

        vborxset(0xB7, 0x04, (g_dev.cfg.EnDESSC) << 2);
        vborxset(0xB8, 0x80, g_dev.cfg.CKSOURCE<<7);
        vborxset(0xC0, 0x10, (!g_dev.cfg.EnDESSC) << 4);
        vborxset(0xD0, 0x2e, (cfg_EnVBORxVidBypass << 5) + (g_dev.cfg.SwapModeSel<<2) + (g_dev.cfg.EnDVMode << 1));
        vborxset(0xD2, 0xc8, (cfg_VBORxCLKStbRef << 6) + (cfg_EnVBORxPCLKStable << 3));
        vborxset(0x0B, 0x05, (cfg_EnPNSwap<<2)+cfg_EnVBORxforceVOut);
        vborxset(0x32, 0xF0, 0x30);
        vborxset(0x35, 0x02, cfg_EnAlnDetTO << 1);
        vborxset(0x5C, 0x01, cfg_EnAutoPwdAll);


        // DESSC setting for long term jitter
        vborxset(0xC0, 0x0E, (cfg_CINC<<2)+(cfg_DIEN<<1));

        // VBORX CPLL bandwidth setting for Lane7 bit error rate
        vborxset(0xB0, 0x0C, 0x00);
        vborxset(0xB1, 0x0C, 0x00);

        // OPLL_DEI 0->1
        vborxset(0xB6, 0x04, 0x04);

        vborxwr(0x86, 0xFF);
        vborxset(0x31, 0x02, 0x02);
        vborxwr(0x34, 0xFF);
        vborxset(0x35, 0x01, 0x01);

        if( g_dev.cfg.EnDESSC ) {
            vborxwr(0xBC, 0xB4);
            vborxwr(0xBB, 0x66);
        }
    }

    // Enable LC/XTAL Auto Power-down
    if ( 0 == g_dev.cfg.EnIntPG ) {
        vborxset(0xB8, 0x03, 0x00);
    }
    else{
        vborxset(0xB8, 0x03, 0x03);
    }

    // H2TX Initial Setting
    hdmitxwr(0xFF, 0xC3);
    hdmitxwr(0xFF, 0xA5);
    //hdmitxset(0x0C, 0x1C, (ForceTMDSStb<<4)+(ForceVOut<<3)+(ForceRxOn<<2));
    hdmitxset(0x0C, 0x1C, (0 << 4) + (0 << 3) + (cfg_ForceRxOn << 2));
    // hdmitxwr(0xF8, 0xFF);
    // Disable, Password

    hdmitxset(0x20, 0x0E, ((1 - 1) << 2) + (g_dev.cfg.DisLockPR << 1));
    hdmitxset(0x22, 0x07, (cfg_AudLatEdge << 2) + (cfg_VidLatEdge << 1) + g_dev.cfg.EnIntPG);
    hdmitxset(0x34, 0xC0, (1 << 7) + (0 << 6));
    hdmitxset(0x35, 0x03, g_dev.cfg.DDCSpeed);
    hdmitxset(0x3A, 0xFC, (cfg_EnDDCMasterArb << 7) + (cfg_DisH2Auto << 6) + (g_dev.cfg.SCDCFlagByte << 5) + (cfg_EnFlagUpdRd << 4) + (g_dev.cfg.FlagPollSel << 2));
    hdmitxset(0x90, 0xEA, (cfg_LMSwap << 7) + (cfg_YCSwap << 6) + (cfg_RBSwap << 5) + (cfg_InSyncAutoSwapOff << 3) + (cfg_EnInSwap << 1));

    hdmitxset(0x93, 0x1F, (cfg_EnDither << 4) + (cfg_EnUdFilt << 3) + (cfg_DNFreeGo << 2) + (cfg_UpSampSel << 1) + g_dev.cfg.ColorClip);
    hdmitxset(0x94, 0x3E, (cfg_XPStableTime << 4) + (cfg_EnXPLockChk << 3) + (cfg_EnPLLBufRst << 2) + (cfg_EnFFAutoRst << 1));
    hdmitxset(0xBF, 0xC8, (cfg_TxCLKStbRef << 6) + (g_dev.cfg.EnIntPG << 3) + (cfg_VidStbSen << 1));
    hdmitxset(0xC0, 0x10, cfg_AutoPhReSync << 4);
    hdmitxset(0xC3, 0x0F, (cfg_EnSSCPTrgPkt << 3) + (cfg_SSCPTest << 2) + (cfg_SSCPOpt << 1) + cfg_SSCPPos);
    hdmitxset(0x95, 0x80, (g_dev.cfg.EnH2InLBBypass << 7));
    hdmitxset(0x96, 0x77, (cfg_BDEShift << 4) + cfg_ADEShift);

    // TXPLL setting for jitter tolerance
     if( g_dev.cfg.EnDESSC )
     {
         hdmitxwr(0x85, 0x59);  // for testing IPLL DEI 0->1
        if ( g_dev.vars.rid >= 0xA1 ) {
            if (rst_all) {
                vborxset(0xB5, 0x80, 0x80);
            }
        }
    }
     else
     {
         hdmitxwr(0x85, 0x49);
        if ( g_dev.vars.rid >= 0xA1 ) {
            if (rst_all) {
                vborxset(0xB5, 0x80, 0x00);
            }
        }
     }

    // Debug Port Selection
    //hdmitxset(0xF0, 0xcf,(EnDebug<<7)+(EnDebug<<6)+(DbgVBO<<3)+DbgSel);
    hdmitxset(0xF0, 0xcf, (0 << 7) + (0 << 6) + (0 << 3) + 0);
    if ( g_dev.vars.rid >= 0xA1 ) {
        hdmitxset(0xFE, 0x10, 0x10);
    }

    if ( rst_all ) {
        if (g_dev.vars.RCLK == 0)
        {
            it6265_load_rclk();
            if ( g_dev.vars.RCLK == 0 )
            {
                it6265_cal_rclk();
                if ( g_dev.vars.RCLK == 0 )
                {   
                    pr_info("g_dev.vars.RCLK = RCLK_TYPICAL;");
                    g_dev.vars.RCLK = RCLK_TYPICAL;
                }
            }
        }

        //it6265_cal_rclk();
        it6265_set_calib_data(g_dev.vars.RCLK);
    }

    // Setup INT Pin: Active Low & Open-Drain
    hdmitxset(0x0D, 0x07, 0x03);

    hdmitxset(0xc0, 0x08, 0x00);

    if (rst_all) {
        vborxwr(0x14, 0xff);
        vborxwr(0x15, 0x38|0x03);
        vborxwr(0x16, 0x20);
        vborxwr(0x17, 0x04);
    }

    hdmitxwr(0xFF, 0xF8 );

    hdmitxset(0x19, 0x03, 0x03);   // Enable HPD and RxSen Interrupt
    hdmitxwr(0x1e, 0x30);

    hdmitxset(0xA0, 0xC0, 0x40);

    it6265_video_state(Vid_Reset);
    it6265_audio_state(Aud_Reset);
    it6265_hdcp_state(CP_Reset);

    g_dev.vars.ICLK = 0;
    g_dev.vars.PCLK = 0;
    g_dev.vars.TXCLK = 0;
    g_dev.vars.VCLK = 0;
    g_dev.vars.param_chg_cnt = 0;
    g_dev.vars.fifo_err_count = 0;
    g_dev.vars.fifo_err_handled = 0;

    #if AUTO_LANE_DETECT
    g_dev.vars.VBOLaneNum = 3;
    #endif
}

void It6265::it6265_sys_state(SysState_Type NewState)
{
    if (g_dev.vars.SysState == NewState) {
        if (NewState != SYS_Unplug) {
            return;
        }
    }
    g_dev.vars.SysState = NewState;

    switch (g_dev.vars.SysState) {
    case SYS_Unplug:
        pr_info("SysState change to SYS_Unplug\r\n");
        it6265_enable_timestamp(0);
        hdmitxset(0x05, 0x48, 0x48);
        if ( g_dev.cfg.keep_vborx_connection )  // keep_vborx_connection == 0
        {
            pr_info("ite6265_reset = 0\r\n");
            it6265_reset(0);
        }
        else
        {
            pr_info("ite6265_reset = 1\r\n");
            it6265_reset(1);
        }

        //g_dev.vars.reset_vborx = 1;
        g_dev.vars.prev_hpd = 0;
        g_dev.vars.prev_clock_ratio = 0;
        // restore HDMI output settings
        g_dev.cfg.ColorDepth = OUTPUT_COLOR_DEPTH;
        g_dev.cfg.InColorMode = INPUT_COLOR_MODE;
        g_dev.cfg.OutColorMode = OUTPUT_COLOR_MODE;
        g_dev.cfg.DynRange = DYNAMIC_RANGE;
        g_dev.cfg.YCbCrCoef = CSC_COEFF;
        g_dev.cfg.ColorClip = false;      // 1: clip RGBY to 16~235, CbCr to 16~240

        it6265_power_down();
        idle(10);

        // for the case when keep_vborx_connection==true, and HPD keeps high
        // HPD interrupt will not happen and we have to change the state by checking HPD status
        if (it6265_is_hpd_high()) {
            pr_info("g_dev.vars.SysState = SYS_HPD;\r\n");
            g_dev.vars.SysState = SYS_HPD;
        }
        else {
            break;
        }

    case SYS_HPD:
        pr_info("SysState change to SYS_HPD\r\n");
        g_dev.vars.prev_hpd = 1;
        it6265_power_on();
        hdmitxset(0x25, 0x01, 0x00);
        //it6265_scdc_cancel_rr();
        it6265_edid_parse();

        #if ENABLE_CEC
            hdmitxwr(0xFC, ((CECAddr << 1) | 0x01));
            //
            // g_dev.vars.phy_addrL = 0x00;
            // g_dev.vars.phy_addrH = 0x10;
            //
            // physical address is parsed from EDID ( please set ENABLE_EDID_PARSING=1 (config.h))
            // stored in g_dev.vars.phy_addrL, g_dev.vars.phy_addrH
            //
            it6265_cec_init( g_dev.vars.phy_addrL, g_dev.vars.phy_addrH, g_dev.vars.cec_time_unit );
            it6265_cec_tx_polling();
        #endif

        g_dev.vars.SysState = SYS_WaitRxSen;
        idle(10);

    case SYS_WaitRxSen:
        pr_info("SysState change to SYS_WaitRxSen\r\n");
        if (it6265_is_rx_sense()) {
            g_dev.vars.SysState = SYS_RxSen;
        } else {
            break;
        }

    case SYS_RxSen:
        pr_info("SysState change to SYS_RxSen\r\n");

        if ( g_dev.cfg.EnDEOnly ) {
            pr_info("g_dev.cfg.EnDEOnly == true\r\n");
            it6265_set_syncde_gen(g_dev.input.VIC);
        }

        if ( g_dev.cfg.EnIntPG ) {
            pr_info("g_dev.cfg.EnIntPG == true\r\n");
            it6265_video_state(Vid_VidPG) ;
            idle(100);
            it6265_video_state(Vid_VidOutEnable) ;
            break;
        }

        pr_info("into it6265_is_video_in_stable\r\n");
        if( it6265_is_video_in_stable() )
        {
            pr_info("it6265_is_video_in_stable()\r\n");
            it6265_video_state(Vid_VidInStable) ;
        }
        else
        {
            pr_info("g_dev.cfg.EnDEOnly == true\r\n");
            it6265_reset_vbotx();
            it6265_video_state(Vid_WaitForVidIn);
        }
        g_dev.cfg.keep_vborx_connection = g_dev.cfg.keep_vborx_connection_prev;
        break;

    default :
        pr_err("ERROR: SysState change to Unknown state !!!\r\n");
        break;
    }
}

void It6265::it6265_ddc_error_handler( void )
{
    u8 rddata;

    rddata = hdmitxrd(0x41) & 0x01;

    if (rddata == 0x01) {
        hdmitxset(0x41, 0x01, 0x00);   // Disable CP_Desired
        hdmitxset(0x05, 0x40, 0x40);   // Enable HDCP Reset
    }

    hdmitxset(0x28, 0x01, 1);
    hdmitxwr(0x2E, 0x0A);              // Generate SCL Clock
    hdmitxset(0x28, 0x01, 0x00);
    pr_info2("Use HW DDC GenCLK ...\r\n");

    if (rddata == 0x01) {
        hdmitxset(0x05, 0x40, 0x00);   // Disable HDCP Reset
        hdmitxset(0x41, 0x01, 0x01);   // Enable CP_Desired
    }
}

#if SUPPORT_HDCP_REPEATER

#if ENABLE_HDCP_REPEATER_DEVICE_COUNT_OVER_6
u8 It6265::softi2crd(int offset, int length)
{
    //int i;
    u8 rd_datab7, rd_datab6, rd_datab5, rd_datab4;
    u8 rd_datab3, rd_datab2, rd_datab1, rd_datab0;
    u8  rd_byte;
    u8 reg28b, reg28f, reg289, reg28d;

    reg28b = hdmitxrd(0x28);
    reg28f = (reg28b &0xF0) | 0x0F;
    reg289 = (reg28b &0xF0) | 0x09;
    reg28d = (reg28b &0xF0) | 0x0D;
    reg28b = (reg28b &0xF0) | 0x0B;

    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab7 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab6 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab5 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab4 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab3 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab2 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab1 = hdmitxrd(0x2E) >> 7;
    hdmitxwr(0x28, reg28b);
    hdmitxwr(0x28, reg28f);
    rd_datab0 = hdmitxrd(0x2E) >> 7;
    if ( offset == (length - 1) )
    {
        hdmitxwr(0x28, reg28b);
        hdmitxwr(0x28, reg289);
        hdmitxwr(0x28, reg28d); // Master ACK
        hdmitxwr(0x28, reg28f);
    }
    else
    {
        hdmitxwr(0x28, reg28b);
        hdmitxwr(0x28, reg289);
        hdmitxwr(0x28, reg28d); // Master ACK
        hdmitxwr(0x28, reg289);
    }
    rd_byte = (rd_datab7 << 7 & 0x80) | (rd_datab6 << 6 & 0x40) |
              (rd_datab5 << 5 & 0x20) | (rd_datab4 << 4 & 0x10) |
              (rd_datab3 << 3 & 0x08) | (rd_datab2 << 2 & 0x04) |
              (rd_datab1 << 1 & 0x02) | (rd_datab0 & 0x01);

    return rd_byte;
}

void It6265::softi2cwr(u8 data)
{
    u8 data_b7, data_b6, data_b5, data_b4;
    u8 data_b3, data_b2, data_b1, data_b0;

    data_b7 = (data & 0x80) >> 7;
    data_b6 = (data & 0x40) >> 6;
    data_b5 = (data & 0x20) >> 5;
    data_b4 = (data & 0x10) >> 4;
    data_b3 = (data & 0x08) >> 3;
    data_b2 = (data & 0x04) >> 2;
    data_b1 = (data & 0x02) >> 1;
    data_b0 = (data & 0x01);

    // after detect_start
    hdmitxset(0x28, 0x0f, 0x09);
    if ( data_b7 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //   hdmitxset(0x28,0x0f,0x09);
    }
    else
    {
        hdmitxset(0x28, 0x0f, 0x0d);
        hdmitxset(0x28, 0x0f, 0x09);
    }
    if ( data_b6 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //   hdmitxset(0x10,0x0f,0x09);
    }
    else
    {
        if ( data_b7 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    if ( data_b5 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //   hdmitxset(0x10,0x0f,0x09);
    }
    else
    {
        if ( data_b6 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    if ( data_b4 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //   hdmitxset(0x28,0x0f,0x09);
    }
    else
    {
        if ( data_b5 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    if ( data_b3 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //  hdmitxset(0x28,0x0f,0x09);
    }
    else
    {
        if ( data_b4 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    if ( data_b2 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //  hdmitxset(0x28,0x0f,0x09);
    }
    else
    {
        if ( data_b3 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    if ( data_b1 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //  hdmitxset(0x28,0x0f,0x09);
    }
    else
    {
        if ( data_b2 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    if ( data_b0 )
    {
        hdmitxset(0x28, 0x0f, 0x0b);
        hdmitxset(0x28, 0x0f, 0x0f);
        hdmitxset(0x28, 0x0f, 0x0b);
        //  hdmitxset(0x28,0x0f,0x09);
    }
    else
    {
        if ( data_b1 == 1 )
        {
            hdmitxset(0x28, 0x0f, 0x09);
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
        else
        {
            hdmitxset(0x28, 0x0f, 0x0d);
            hdmitxset(0x28, 0x0f, 0x09);
        }
    }
    hdmitxset(0x28, 0x0f, 0x0b); // Slave ACK
    hdmitxset(0x28, 0x0f, 0x0f); // Slave ACK
    hdmitxset(0x28, 0x0f, 0x0b); // Slave ACK
    hdmitxset(0x28, 0x0f, 0x09); // Slave ACK
}

void It6265::softhdcprd(u8 offset, u8 length, u8 *databuf)
{
    u8 i;

    hdmitxset(0x28, 0x0f, 0x0f); // enable software DDC mode
    hdmitxset(0x28, 0x0f, 0x0d); // detect_start

    softi2cwr(0x74);
    softi2cwr(offset);

    hdmitxset(0x28, 0x0f, 0x0b);
    hdmitxset(0x28, 0x0f, 0x0f);
    hdmitxset(0x28, 0x0f, 0x0d); // detect_start

    softi2cwr(0x75);
    for ( i = 0; i < length; i++ )
    {
        databuf[i] = softi2crd(i, length);
    }
    hdmitxset(0x28, 0x0f, 0x06); // disable software DDC mode
}

#define ENH_DDC_SPEED 0
bool It6265::it6265_hdcp_readksv(u8 offset,u16 bytenum, u8* KSVLIST)
{
    bool result;
    int retry = 1;
    u16 remaincnt, cnt,i ;
    u8 tmp;
    u8 *pKSV ;

    pr_info2("it6265_hdcp_readksv(%x, %d, %p)\r\n\r",offset,bytenum,KSVLIST) ;

    if( !KSVLIST ) return false ;
    if( bytenum < 1) return false ;

__RETRY:

    #if CHECK_HPD_BEFORE_RELAY_SCDC
    if (false == it6265_is_hpd_high()) {
        pr_err("Abort hdcp read becasue of detecting unplug !!!\r\n");
        return false;
    }
    #endif
    it6265_scdc_status_check();

    cnt = bytenum  ;
    // hdmitxset(0x35, 0x10, 0x10) ;
    // hdmitxset(0x35, 0x13, 0x01) ;

    #if ENH_DDC_SPEED
    hdmitxset(0x35, 0x3, 0x01) ;
    #endif

    hdmitxset(0x28, 0x01, 0);
    //hdmitxwr(0x2E, 0x0F);         // Abort DDC Command
    hdmitxwr(0x2E, 0x0F);           // DDC FIFO Clear
    hdmitxwr(0x2E, 0x09);           // DDC FIFO Clear


    hdmitxwr(0x29, 0x74);           // HDCP Address
    hdmitxwr(0x2A, offset);         // HDCP Offset

    tmp = cnt & 0xFF;
    hdmitxwr(0x2B, tmp);            // Read ByteNum[7:0]

    cnt >>= 8;
    tmp = cnt & 0x03;
    hdmitxwr(0x2C, tmp);            // ByteNum[9:8]
                                    //
    hdmitxwr(0x2E, 0x00);           // HDCP Read Fire
    pr_info2(" ========== it6265_hdcp_read_ksvlist \r\n\r");        // for sigange test

    // result = it6265_ddcwait();
    remaincnt = bytenum ;
    pKSV = KSVLIST ;
    do
    {
    int ddc_wait_count, ddc_status;

        for( ddc_wait_count = 0; ddc_wait_count < DDCWAITNUM;ddc_wait_count++)
        {
            ddc_status = hdmitxrd(0x2F);
            printf("ddc_status = %02X,\r\n",ddc_status) ;

            if( ddc_status & 0x84 ) // break if done or full
            {
                break ;
            }
            if( (ddc_status & 0x40)==0)
            {
        break ;
            }
        idle(1);
    }

    if( (ddc_status & 0x84 )||(0==(ddc_status&0x40)))
    {
        if( ddc_status & 0x4 )
        {
        cnt = (remaincnt>32)?32:remaincnt ;
        }
        else
        {
        cnt = hdmitxrd(0x36)&0x3F ;
        cnt = (remaincnt>cnt)?cnt:remaincnt ;
        }
        printf("ddc_status = %02X, read %d/%d data: ",ddc_status, cnt,remaincnt) ;
        remaincnt -= cnt ;

        for(i = 0 ; i < cnt ; i++)
        {
        pKSV[i] = hdmitxrd(0x30) ;
        }

        for(i = 0 ; i < cnt ; i++ )
        {
        printf("%02X ",pKSV[i]) ;
        }
        printf("\r\n") ;
        pKSV += cnt ;
        result = true ;
            ddc_status = hdmitxrd(0x2F);
            printf("ddc_status = %02X, reg36 = %02X\r\n\r",ddc_status,hdmitxrd(0x36)) ;
            if(ddc_status & 0x2 ) break ; // FIFO empty

    }
    else // if (ddc_wait_count == DDCWAITNUM)
    {
        ddc_status = hdmitxrd(0x2F) & 0xFE;

        pr_err("ERROR: DDC Bus Wait TimeOut, Status = %02X\r\n", ddc_status);

        if (hdmitxrd(0x16) & 0x08) {
            pr_err("ERROR: DDC Bus Hang !!!\r\n");
        }
            else if (ddc_status & 0x20) {
            pr_err("ERROR: DDC NoACK !!!\r\n");
            }
            else if (ddc_status & 0x10) {
                pr_err("ERROR: DDC WaitBus !!!\r\n");
            }
            else if (ddc_status & 0x08) {
                pr_err("ERROR: DDC ArbiLose !!!\r\n");
            }
            else {
                pr_err("ERROR: UnKnown Issue !!!\r\n");
            }

            //it6265_ddc_status_check();
            hdmitxset(0x28, 0x01, 0x01);
            hdmitxset(0x35, 0x10, 0x10);
            //idle(5000);
            hdmitxset(0x35, 0x10, 0x00);
            hdmitxset(0x28, 0x01, 0x00);

        result =  false;  // will try again
        break ;
    }
    }while(remaincnt > 0 );
    pr_info2("read done.\r\n") ;
    hdmitxwr(0x2E, 0x0A);           // DDC FIFO Clear
    hdmitxwr(0x2E, 0x09);           // DDC FIFO Clear
    hdmitxwr(0x2E, 0x0F);           // DDC FIFO Clear
    /*
    hdmitxwr(0x2E, 0x0A);           // DDC FIFO Clear
    hdmitxwr(0x2E, 0x09);           // DDC FIFO Clear
    hdmitxwr(0x2E, 0x0F);           // DDC FIFO Clear
    */
    /*hdmitxset(0x35, 0x10, 0x10) ;
    hdmitxset(0x35, 0x13, 0x00) ;*/
    pr_info2("reg2F = %02X.\r\n",hdmitxrd(0x2F)) ;
    hdmitxset(0x28, 0x01, 0x00);    // Disable PC DDC Mode

    if ( result == false ) {
        if ( retry > 0 ) {
            retry--;
            idle( 10 );
            goto __RETRY;
        }
    }

    return result;
}

void SHA1Transform(
    u32 state[5],
    u8 buffer[64]
    );

void SHA1Init(
    SHA1_CTX *context
    );

void SHA1Update(
    SHA1_CTX *context,
    u8 *data,
    u32 len
    );

void SHA1Final(
    u8 digest[20],
    SHA1_CTX *context
    );

void SHA1(
    char *hash_out,
    const char *str,
    int len);

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

/* blk0() and blk() perform the initial expand. */
/* I got the idea of expanding during the round function from SSLeay */
#if BYTE_ORDER == LITTLE_ENDIAN
    #define blk0(i) (block->l[i] = (rol(block->l[i],24)&0xFF00FF00) \
    |(rol(block->l[i],8)&0x00FF00FF))
#elif BYTE_ORDER == BIG_ENDIAN
    #define blk0(i) block->l[i]
#else
    #error "Endianness not defined!"
#endif
#define blk(i) (block->l[i&15] = rol(block->l[(i+13)&15]^block->l[(i+8)&15] \
    ^block->l[(i+2)&15]^block->l[i&15],1))

/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=((w&(x^y))^y)+blk(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=(((w|x)&y)|(w&x))+blk(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=(w^x^y)+blk(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

void It6265::SHA1Transform(u32 state[5], u8 buffer[64])
{
    u32 a, b, c, d, e;

    typedef union
    {
        u8 c[64];
        u32 l[16];
    } CHAR64LONG16;

#ifdef SHA1HANDSOFF
    CHAR64LONG16 block[1];      /* use array to appear as a pointer */

    memcpy(block, buffer, 64);
#else
    /* The following had better never be used because it causes the
     * pointer-to-const buffer to be cast into a pointer to non-const.
     * And the result is written through.  I threw a "const" in, hoping
     * this will cause a diagnostic.
     */
    CHAR64LONG16 *block = (CHAR64LONG16 *)buffer;
#endif
    /* Copy context->state[] to working vars */
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    /* 4 rounds of 20 operations each. Loop unrolled. */
    R0(a, b, c, d, e, 0);
    R0(e, a, b, c, d, 1);
    R0(d, e, a, b, c, 2);
    R0(c, d, e, a, b, 3);
    R0(b, c, d, e, a, 4);
    R0(a, b, c, d, e, 5);
    R0(e, a, b, c, d, 6);
    R0(d, e, a, b, c, 7);
    R0(c, d, e, a, b, 8);
    R0(b, c, d, e, a, 9);
    R0(a, b, c, d, e, 10);
    R0(e, a, b, c, d, 11);
    R0(d, e, a, b, c, 12);
    R0(c, d, e, a, b, 13);
    R0(b, c, d, e, a, 14);
    R0(a, b, c, d, e, 15);
    R1(e, a, b, c, d, 16);
    R1(d, e, a, b, c, 17);
    R1(c, d, e, a, b, 18);
    R1(b, c, d, e, a, 19);
    R2(a, b, c, d, e, 20);
    R2(e, a, b, c, d, 21);
    R2(d, e, a, b, c, 22);
    R2(c, d, e, a, b, 23);
    R2(b, c, d, e, a, 24);
    R2(a, b, c, d, e, 25);
    R2(e, a, b, c, d, 26);
    R2(d, e, a, b, c, 27);
    R2(c, d, e, a, b, 28);
    R2(b, c, d, e, a, 29);
    R2(a, b, c, d, e, 30);
    R2(e, a, b, c, d, 31);
    R2(d, e, a, b, c, 32);
    R2(c, d, e, a, b, 33);
    R2(b, c, d, e, a, 34);
    R2(a, b, c, d, e, 35);
    R2(e, a, b, c, d, 36);
    R2(d, e, a, b, c, 37);
    R2(c, d, e, a, b, 38);
    R2(b, c, d, e, a, 39);
    R3(a, b, c, d, e, 40);
    R3(e, a, b, c, d, 41);
    R3(d, e, a, b, c, 42);
    R3(c, d, e, a, b, 43);
    R3(b, c, d, e, a, 44);
    R3(a, b, c, d, e, 45);
    R3(e, a, b, c, d, 46);
    R3(d, e, a, b, c, 47);
    R3(c, d, e, a, b, 48);
    R3(b, c, d, e, a, 49);
    R3(a, b, c, d, e, 50);
    R3(e, a, b, c, d, 51);
    R3(d, e, a, b, c, 52);
    R3(c, d, e, a, b, 53);
    R3(b, c, d, e, a, 54);
    R3(a, b, c, d, e, 55);
    R3(e, a, b, c, d, 56);
    R3(d, e, a, b, c, 57);
    R3(c, d, e, a, b, 58);
    R3(b, c, d, e, a, 59);
    R4(a, b, c, d, e, 60);
    R4(e, a, b, c, d, 61);
    R4(d, e, a, b, c, 62);
    R4(c, d, e, a, b, 63);
    R4(b, c, d, e, a, 64);
    R4(a, b, c, d, e, 65);
    R4(e, a, b, c, d, 66);
    R4(d, e, a, b, c, 67);
    R4(c, d, e, a, b, 68);
    R4(b, c, d, e, a, 69);
    R4(a, b, c, d, e, 70);
    R4(e, a, b, c, d, 71);
    R4(d, e, a, b, c, 72);
    R4(c, d, e, a, b, 73);
    R4(b, c, d, e, a, 74);
    R4(a, b, c, d, e, 75);
    R4(e, a, b, c, d, 76);
    R4(d, e, a, b, c, 77);
    R4(c, d, e, a, b, 78);
    R4(b, c, d, e, a, 79);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;

    a = b = c = d = e = 0;
#ifdef SHA1HANDSOFF
    memset(block, '\0', sizeof(block));
#endif
}

void It6265::SHA1Init(SHA1_CTX *context)
{
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = context->count[1] = 0;
}

void It6265::SHA1Update(SHA1_CTX *context, u8 *data, u32 len)
{
    u32 i;
    u32 j;

    j = context->count[0];
    if ( (context->count[0] += len << 3) < j )
        context->count[1]++;
    context->count[1] += (len >> 29);
    j = (j >> 3) & 63;
    if ( (j + len) > 63 )
    {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        SHA1Transform(context->state, context->buffer);
        for (; i + 63 < len; i += 64 )
        {
            SHA1Transform(context->state, &data[i]);
        }
        j = 0;
    }
    else
        i = 0;
    memcpy(&context->buffer[j], &data[i], len - i);
}


bool It6265::it6265_hdcp_repeater_check( void )
{
    int i;
    int ksvcnt = 0;
    int msgcnt;
    bool rep_chk = true;
    int sha_cnt_size;

    u8 bstatus[2];
    u8 av[5][4];
    u8 bv[5][4];
    u8 ksvlist[SHA_BUFFER_SIZE];
    SHA1_CTX sha_ctx;
    u8 m0[8];

    hdmitxset(0x70, 0x07, 0x05);
    for (i = 0; i < 4; i++)
        m0[i] = hdmitxrd(0x71 + i);

    for (i = 0; i < 4; i++) {
        hdmitxset(0x70, 0x07, i);
        m0[4 + i] = hdmitxrd(0x75);
    }

    pr_info("IT6265: M0: 0x ");

    for(i=0; i<8; i++)
        pr_info("%02X ", m0[7-i]);


    it6265_hdcp_read(0x41, 2);     // BStatus
    bstatus[0] = hdmitxrd(0x64);
    bstatus[1] = hdmitxrd(0x65);

    pr_info("IT6265: Device Count = %X\r\n", bstatus[0]&0x7F);
    pr_info("IT6265: Max. Device Exceeded = %02X\r\n", (bstatus[0]&0x80)>>7);


    pr_info("IT6265: Depth = %X\r\n", bstatus[1]&0x07);
    pr_info("IT6IT6265682: Max. Cascade Exceeded = %02X\r\n", (bstatus[1]&0x08)>>3);
    pr_info("IT6265: HDMI_MODE = %d\r\n", (bstatus[1]&0x10)>>4);
    pr_info("\r\n");

    if( (bstatus[0]&0x80) || (bstatus[1]&0x08) ) {
        pr_info("IT6265: ERROR: Max. Device or Cascade Exceeded !!!\r\n");
        return false;
    }
    else {

        // read ksv list from ddc fifo
        ksvcnt = 5*(bstatus[0]&0x7F);

        //
        //read back HDCP offset KSV LIST (0x43) back to internal register
        //

        if( ksvcnt ){
            if ( ksvcnt<=5*6 )
            {
                it6265_hdcp_read(0x43, ksvcnt);     // KSV LIST FIFO
                hdmitxbrd(0x30, ksvcnt, &ksvlist[0]);
            }
            else
            {
                if ( ksvcnt<=5*HDCP_1P4_MAX_DEVICE_COUNT )
                {
                    //softhdcprd(0x43, ksvcnt, &ksvlist[0]);
                    it6265_hdcp_readksv(0x43, ksvcnt, &ksvlist[0]);
                }
                else
                {
                    pr_err("ERROR: Device Count %d > HDCP_1P4_MAX_DEVICE_COUNT !!!\r\n", ksvcnt/5);
                    return false;
                }
            }
        }
        else{
            pr_info("IT6265: WARNING: Device Count = 0 !!!\r\n");
            return false;
        }

        msgcnt = ksvcnt;

        ksvlist[msgcnt++] = bstatus[0];
        ksvlist[msgcnt++] = bstatus[1];

        ksvlist[msgcnt++] = m0[0];
        ksvlist[msgcnt++] = m0[1];
        ksvlist[msgcnt++] = m0[2];
        ksvlist[msgcnt++] = m0[3];
        ksvlist[msgcnt++] = m0[4];
        ksvlist[msgcnt++] = m0[5];
        ksvlist[msgcnt++] = m0[6];
        ksvlist[msgcnt++] = m0[7];

        ksvlist[msgcnt] = 0x80;

        sha_cnt_size = msgcnt+1+8;

        //sha_cnt_size = msgcnt+1+2;
        sha_cnt_size = (((sha_cnt_size+63)/64)*64);
        //sha_cnt_size = SHA_BUFFER_SIZE;

        for(i=msgcnt+1; i<sha_cnt_size-2 ;i++)
        {
            ksvlist[i] = 0x00;
        }

        ksvlist[sha_cnt_size-2] = ((8*msgcnt) >> 8)&0xFF ;
        ksvlist[sha_cnt_size-1] = (8*msgcnt) &0xFF ;

        pr_info("IT6265:ksvlist[]=\r\n");
        for (i=0 ; i<sha_cnt_size ; i++)
        {
            pr_info("%02X ", ksvlist[i]);
            if ( (i+1)%16 == 0 )
            {
                pr_info("\r\n");
            }
        }
        pr_info("\r\n");

        pr_info("IT6265: SHA Message Count = %d, buf = %d\r\n\r\n", msgcnt, sha_cnt_size);

        msgcnt = sha_cnt_size;
        SHA1Init(&sha_ctx);
        SHA1Update(&sha_ctx,ksvlist,msgcnt);

        memcpy( av, (u8*)sha_ctx.state, 20 );

        for(i=0; i<5; i++)
            pr_info("IT6265: AV.H%d = 0x %02X %02X %02X %02X\r\n", i, (int)av[i][3], (int)av[i][2], (int)av[i][1], (int)av[i][0]);

        pr_info("\r\n");

        for(i=0; i<5; i++) {
            it6265_hdcp_read(0x20 + i * 4, 4);   // V'Hi
            hdmitxset(0x70, 0x07, i);
            bv[i][0] = (u8)hdmitxrd(0x71);
            bv[i][1] = (u8)hdmitxrd(0x72);
            bv[i][2] = (u8)hdmitxrd(0x73);
            bv[i][3] = (u8)hdmitxrd(0x74);

            pr_info("BV.H%d = 0x %02X %02X %02X %02X\r\n", i, (int)bv[i][3], (int)bv[i][2], (int)bv[i][1], (int)bv[i][0]);

            if( av[i][0]!=bv[i][0] || av[i][1]!=bv[i][1] || av[i][2]!=bv[i][2] || av[i][3]!=bv[i][3])
            rep_chk = false;
            // return false;
        }
    }

    #if ENABLE_CALLBACK
    // call back only when SHA pass
    if ( g_dev.cb.it6265_hdcp_ksv_list_cb ) {
        g_dev.cb.it6265_hdcp_ksv_list_cb(ksvcnt, ksvlist, g_dev.cb_data.hdcp_ksv_list_cb_data);
    }
    #endif

    return rep_chk;
}

#else // #else for ENABLE_HDCP_REPEATER_DEVICE_COUNT_OVER_6

#define rol(x,y) (((x) << (y)) | (((unsigned long)x) >> (32-y)))
void It6265::it6265_sha_transform( u32 *h, u32 *w )
{
    int t;
    u32 tmp;

    for (t = 16; t < 80; t++) {
         tmp = w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16];
         w[t] = rol(tmp,1);
    }

    h[0] = 0x67452301 ;
    h[1] = 0xefcdab89;
    h[2] = 0x98badcfe;
    h[3] = 0x10325476;
    h[4] = 0xc3d2e1f0;

    for (t = 0; t < 20; t++) {
         tmp = rol(h[0],5) + ((h[1] & h[2]) | (h[3] & ~h[1])) + h[4] + w[t] + 0x5a827999;

         h[4] = h[3];
         h[3] = h[2];
         h[2] = rol(h[1],30);
         h[1] = h[0];
         h[0] = tmp;

    }
    for (t = 20; t < 40; t++) {
         tmp = rol(h[0],5) + (h[1] ^ h[2] ^ h[3]) + h[4] + w[t] + 0x6ed9eba1;
         h[4] = h[3];
         h[3] = h[2];
         h[2] = rol(h[1],30);
         h[1] = h[0];
         h[0] = tmp;
    }
    for (t = 40; t < 60; t++) {
         tmp = rol(h[0],5) + ((h[1] & h[2]) | (h[1] & h[3]) | (h[2] & h[3])) + h[4] + w[t] + 0x8f1bbcdc;
         h[4] = h[3];
         h[3] = h[2];
         h[2] = rol(h[1],30);
         h[1] = h[0];
         h[0] = tmp;
    }
    for (t = 60; t < 80; t++) {
         tmp = rol(h[0],5) + (h[1] ^ h[2] ^ h[3]) + h[4] + w[t] + 0xca62c1d6;
         h[4] = h[3];
         h[3] = h[2];
         h[2] = rol(h[1],30);
         h[1] = h[0];
         h[0] = tmp;
    }

    h[0] += 0x67452301 ;
    h[1] += 0xefcdab89;
    h[2] += 0x98badcfe;
    h[3] += 0x10325476;
    h[4] += 0xc3d2e1f0;
}

void It6265::it6265_sha_simple( u8 *p, int msgcnt , u8 *output)
{
    int i, t ;
    u32 c ;
    u8 *pBuff = p ;
    //u32 w[20];
    // 64 + 20*4 = 144;
    u32 *w = (u32*) ( g_dev.data_buffer +64 );  // +64 because m[64]
    u32 sha[5];

    for( i = 0 ; i < msgcnt ; i++ ){
        t = i/4 ;
        if( i%4 == 0 ){
            w[t] = 0 ;
        }
        c = pBuff[i] ;
        c &= 0xFF ;
        c <<= (3-(i%4))*8 ;
        w[t] |= c ;
    }
    t = i/4 ;
    if( i%4 == 0 ){
        w[t] = 0 ;
    }
    c = 0x80 << ((3-i%4)*8) ;
    w[t]|= c ; t++ ;
    for( ; t < 15 ; t++ ){
        w[t] = 0 ;
    }
    w[15] = msgcnt*8  ;

    it6265_sha_transform(sha,w);

    for(i=0; i<5; i++) {
        output[i*4]   = (unsigned char) (sha[i]&0xFF) ;
        output[i*4+1] = (unsigned char)((sha[i]>>8)&0xFF) ;
        output[i*4+2] = (unsigned char)((sha[i]>>16)&0xFF) ;
        output[i*4+3] = (unsigned char)((sha[i]>>24)&0xFF) ;
    }
}

bool It6265::it6265_hdcp_repeater_check( void )
{
    u8 i;
    int ksvcnt = 0;
    int msgcnt;
    bool rep_chk = true;

    u8 bstatus[2];
    u8 av[5][4],bv[5][4],ksvlist[32];
    //u8 m[64];
    u8 *m = g_dev.data_buffer;
    u8 m0[8];

    hdmitxset(0x70, 0x07, 0x05);
    for (i = 0; i < 4; i++)
        m0[i] = hdmitxrd(0x71 + i);

    for (i = 0; i < 4; i++) {
        hdmitxset(0x70, 0x07, i);
        m0[4 + i] = hdmitxrd(0x75);
    }

    pr_info("IT6265: M0: 0x ");

    for(i=0; i<8; i++)
        pr_info("%02X ", m0[7-i]);


    it6265_hdcp_read(0x41, 2);     // BStatus
    bstatus[0] = hdmitxrd(0x64);
    bstatus[1] = hdmitxrd(0x65);

    pr_info("IT6265: Device Count = %X\r\n", bstatus[0]&0x7F);
    pr_info("IT6265: Max. Device Exceeded = %02X\r\n", (bstatus[0]&0x80)>>7);


    pr_info("IT6265: Depth = %X\r\n", bstatus[1]&0x07);
    pr_info("IT6IT6265682: Max. Cascade Exceeded = %02X\r\n", (bstatus[1]&0x08)>>3);
    pr_info("IT6265: HDMI_MODE = %d\r\n", (bstatus[1]&0x10)>>4);
    pr_info("\r\n");

    if( (bstatus[0]&0x80) || (bstatus[1]&0x08) ) {
        pr_info("IT6265: ERROR: Max. Device or Cascade Exceeded !!!\r\n");
        return false;
    }
    else {

        // read ksv list from ddc fifo
        ksvcnt = 5*(bstatus[0]&0x7F);

        //
        //read back HDCP offset KSV LIST (0x43) back to internal register
        //

        if( ksvcnt ){
            it6265_hdcp_read(0x43, ksvcnt);     // KSV LIST FIFO
            hdmitxbrd(0x30, ksvcnt, &ksvlist[0]);
        }
        else{
            pr_info("IT6265: WARNING: Device Count = 0 !!!\r\n");
            return false;
        }

        msgcnt = 0;

        for(i=0; i<(bstatus[0]&0x7F); i++) {
            m[msgcnt++] = (unsigned char) ksvlist[i*5+0];
            m[msgcnt++] = (unsigned char) ksvlist[i*5+1];
            m[msgcnt++] = (unsigned char) ksvlist[i*5+2];
            m[msgcnt++] = (unsigned char) ksvlist[i*5+3];
            m[msgcnt++] = (unsigned char) ksvlist[i*5+4];

             pr_info("IT6265: KSV List %d = 0x %02X %02X %02X %02X %02X\r\n", i, (int)m[i*5+4], (int)m[i*5+3], (int)m[i*5+2], (int)m[i*5+1], (int)m[i*5+0]);
        }

        pr_info("\r\n");

        m[msgcnt++] = bstatus[0];
        m[msgcnt++] = bstatus[1];

        m[msgcnt++] = m0[0];
        m[msgcnt++] = m0[1];
        m[msgcnt++] = m0[2];
        m[msgcnt++] = m0[3];
        m[msgcnt++] = m0[4];
        m[msgcnt++] = m0[5];
        m[msgcnt++] = m0[6];
        m[msgcnt++] = m0[7];

        pr_info("IT6265: SHA Message Count = %d\r\n\r\n", msgcnt);

        m[msgcnt] = 0x80;

        for(i=msgcnt+1; i<62 ;i++)
            m[i] = 0x00;

        m[62] = ((8*msgcnt) >> 8)&0xFF ;
        m[63] = (8*msgcnt) &0xFF ;

        it6265_sha_simple(&m[0], msgcnt, &av[0][0]) ;

        for(i=0; i<5; i++)
            pr_info("IT6265: AV.H%d = 0x %02X %02X %02X %02X\r\n", i, (int)av[i][3], (int)av[i][2], (int)av[i][1], (int)av[i][0]);

        pr_info("\r\n");


        for(i=0; i<5; i++) {

            it6265_hdcp_read(0x20 + i * 4, 4);   // V'Hi
            hdmitxset(0x70, 0x07, i);
            bv[i][0] = (u8)hdmitxrd(0x71);
            bv[i][1] = (u8)hdmitxrd(0x72);
            bv[i][2] = (u8)hdmitxrd(0x73);
            bv[i][3] = (u8)hdmitxrd(0x74);

            pr_info("BV.H%d = 0x %02X %02X %02X %02X\r\n", i, (int)bv[i][3], (int)bv[i][2], (int)bv[i][1], (int)bv[i][0]);

            if( av[i][0]!=bv[i][0] || av[i][1]!=bv[i][1] || av[i][2]!=bv[i][2] || av[i][3]!=bv[i][3])
                rep_chk = false;
            // return false;
        }
    }

    #if ENABLE_CALLBACK
    // call back only when SHA pass
    if ( g_dev.cb.it6265_hdcp_ksv_list_cb ) {
        g_dev.cb.it6265_hdcp_ksv_list_cb(ksvcnt, ksvlist, g_dev.cb_data.hdcp_ksv_list_cb_data);
    }
    #endif

    return rep_chk;
}

#endif // #if ENABLE_HDCP_REPEATER_DEVICE_COUNT_OVER_6

#else  // #else for SUPPORT_HDCP_REPEATER

bool It6265::it6265_hdcp_repeater_check( void )
{
    pr_err("### HDCP Repeater supporting is disabled\r\n");
    return false;
}

#endif

void It6265::it6265_timer_task( void )
{
    u8 bcaps;
    u8 ksvready;
    u8 tmp;

    if ( g_dev.vars.avi_cmd_timeout ) {

        g_dev.vars.avi_cmd_tick = g_dev.vars.sys_tick_inc;

        if ( g_dev.vars.avi_cmd_tick >= g_dev.vars.avi_cmd_timeout ) {
            g_dev.vars.avi_cmd_timeout = 0;

            pr_info("IT6265: it6265_timer_task avi (%d, %d) ... \r\n", (int)g_dev.vars.avi_cmd, (int)g_dev.vars.avi_cmd_tick);

            switch (g_dev.vars.avi_cmd)
            {
                case AVICMD_WAIT_FOR_AUDIO:

                    if ((hdmitxrd(0xEF) & 0x40) == 0x00) {
                        tmp = hdmitxrd(0xEE);
                        if (hdmitxrd(0xEE) == tmp && tmp != 0xFF) {
                            it6265_audio_state(Aud_AudInStable);
                        }
                        else {
                            // wait and check agian next 200 ms
                            it6265_avi_task_wait_for_audio_stable();
                        }
                    }
                    else {
                        pr_info2("IT6265: No audio input\r\n");
                        // wait and check agian next 200 ms
                        if ( g_dev.vars.VState == Vid_VidOutEnable )
                        {
                            it6265_avi_task_wait_for_audio_stable();
                        }
                    }
                    break;

                case AVICMD_SYS_UNPLUG:
                    // shall we check this ????
                    //if ( 0 == it6265_is_hpd_high() )
                    {
                        it6265_sys_state(SYS_Unplug);
                    }
                    break;

                case AVICMD_SHOW_AUD_INFO:
                    it6265_show_audio_cts();
                    break;

                case AVICMD_CHECK_VIDEO_IN_STABLE:

            if (it6265_is_video_in_stable()) {
                if (g_dev.vars.VState == Vid_WaitForVidIn) {
                    it6265_video_state(Vid_VidInStable);
                }
                        else {
                            pr_info2("Stable On VState=%d\r\n", g_dev.vars.VState);
                        }
            }
                    else {
                        it6265_avi_task_check_video_in_stable();
                    }
                    break;

                default:
                    pr_info("IT6265: avi_cmd default cmd\r\n");
                    break;
            }

        }
    }

    if ( g_dev.vars.hdcp_cmd_timeout ) {

        g_dev.vars.hdcp_cmd_tick = g_dev.vars.sys_tick_inc;

        if ( g_dev.vars.hdcp_cmd_tick >= g_dev.vars.hdcp_cmd_timeout ) {
            g_dev.vars.hdcp_cmd_timeout = 0;
            hdmitxwr(0x09, 0x00); // todo: check this

            pr_info("IT6265: it6265_timer_task hdcp (%d, %d) ... \r\n", (int)g_dev.vars.hdcp_cmd, (int)g_dev.vars.hdcp_cmd_tick);

            switch (g_dev.vars.hdcp_cmd)
            {
                case HDCP_AUTH_RETRY:
                    pr_info("IT6265: HDCP_AUTH_RETRY, VState=%d\r\n", (int)g_dev.vars.VState);
                    pr_info("IT6265: HDCP_AUTH_RETRY::fail=%04x\r\n", (int)g_dev.vars.hdcp2_fail_status);
                    if (g_dev.vars.VState == Vid_VidOutEnable)
                    {
                        #if 1
                        //idle(200);
                        //if ( (g_dev.vars.hdcp2_fail_status & 0x0010) || (g_dev.vars.hdcp2_fail_status & 0x0100) || (g_dev.vars.hdcp2_fail_status & 0x0008) )
                        //if (  g_dev.cfg.keep_vborx_connection==1 &&  (g_dev.vars.hdcp2_fail_status & (0x0018|0x1200|0x0440) ))
                        if (  g_dev.cfg.keep_vborx_connection==1 )
                        {
                            if ( (g_dev.vars.hdcp2_fail_status & (0x0010) ) )
                            {
                                g_dev.vars.sys_tick_enabled = 0;
                                g_dev.cfg.keep_vborx_connection_prev = g_dev.cfg.keep_vborx_connection;
                                g_dev.cfg.keep_vborx_connection = 0;
                                it6265_sys_state(SYS_Unplug);
                            }
                            else
                            {
                                if ( (g_dev.vars.hdcp2_fail_status & (0x0100|0x0008)) && (g_dev.vars.hdcp2_fail_status_prev != g_dev.vars.hdcp2_fail_status) )
                                {
                                    g_dev.vars.sys_tick_enabled = 0;
                                    g_dev.cfg.keep_vborx_connection_prev = g_dev.cfg.keep_vborx_connection;
                                    g_dev.cfg.keep_vborx_connection = 0;
                                    it6265_sys_state(SYS_Unplug);
                                    //hdmitxset(0x05,0x10,0x10);
                                }
                                else
                                {
                                    it6265_hdcp_state(CP_Reset);
                                    it6265_hdcp_state( CP_Going );
                                }
                            }
                            g_dev.vars.hdcp2_fail_status_prev = g_dev.vars.hdcp2_fail_status;
                        }

                        else
                        #endif
                        {
                            if ( (g_dev.vars.hdcp2_fail_status & (0x0010) ) )
                            {
                                    it6265_hdcp_state(CP_Reset);
                                    it6265_hdcp_state( CP_Going );
                            }
                            else
                            {
                                if ( (g_dev.vars.hdcp2_fail_status & (0x0100|0x0008)) && (g_dev.vars.hdcp2_fail_status_prev != g_dev.vars.hdcp2_fail_status) )
                                {
                                    it6265_hdcp_state(CP_Reset);
                                    it6265_hdcp_state( CP_Going );
                                }
                                else
                                {
                                    it6265_hdcp_state(CP_Reset);
                                    it6265_hdcp_state( CP_Going );
                                }
                            }
                        }
                    }
                    else
                    {
                        g_dev.vars.sys_tick_enabled = 0;
                        it6265_sys_state(SYS_Unplug);
                    }
                    break;

                case HDCP_POLLING_BCAPS:
                    pr_info("IT6265: HDCP_POLLING_BCAPS\r\n");
                    ksvready = false;

                    if ( g_dev.vars.hdcp_cmd_count > 60 ) { // polling over 5 seconds
                        pr_err("IT6265:  ksv list not read, hdcp fail...\r\n");
                        it6265_hdcp_task_retry_auth();
                        break;
                    }

                    if ( (g_dev.vars.hdcp_cmd_count & 0x1) == 0 ) { // polling every 200ms

                        it6265_hdcp_read(0x40, 1); // BCaps
                        bcaps = hdmitxrd(0x63);

                        if((bcaps &0x20) ==0x00){
                            pr_info2("IT6265:  HDCP KSV list not ready ... \r\n");
                        }
                        else{
                            ksvready = true;
                            pr_info("IT6265:  HDCP KSV list ready ... \r\n");
                        }
                    }

                    g_dev.vars.hdcp_cmd_count++;

                    if(ksvready == true){
                        // do not break and check ksv list immediatelly
                        // g_dev.vars.hdcp_cmd = HDCP_KSV_CHK;
                        it6265_hdcp_task_check_ksv_list();
                        break;
                    }
                    else {
                        // enable next polling
                        g_dev.vars.hdcp_cmd_timeout = 1;
                        g_dev.vars.hdcp_cmd_tick = 0;
                        hdmitxwr(0x09, 0x80);
                        break;
                    }

                case HDCP_KSV_CHK:
                    pr_info("IT6265: HDCP_KSV_CHK\r\n");
                    if(it6265_hdcp_repeater_check() == true){

                        hdmitxset(0x42, 0x02, 0x02);
                        pr_info("IT6265: SHA Check Result = PASS\r\n");
                    }
                    else{

                        hdmitxset(0x42, 0x06, 0x06);
                        pr_err("IT6265 :SHA Check Result = FAIL\r\n");
                    }

                    break;

                default:
                    pr_info("IT6265: default cmd\r\n");
                    break;
            }
        }
    }

    if ( g_dev.vars.avi_cmd_timeout == 0 && g_dev.vars.hdcp_cmd_timeout == 0 )
    {
        // stop count
        pr_info("IT6265: it6265_timer_task Disabled... \r\n");
        hdmitxwr(0x09, 0x00);
    }
}

void It6265::it6265_hdcp_task_retry_auth( void )
{
    pr_info2("---> it6265_hdcp_task_retry_auth ::");
    g_dev.vars.hdcp_cmd = HDCP_AUTH_RETRY;
    g_dev.vars.hdcp_cmd_tick = 0;
    g_dev.vars.hdcp_cmd_timeout = 3;    // do command after 3*100 ms
    it6265_enable_timestamp(1);
}

void It6265::it6265_hdcp_task_check_ksv_list( void )
{
    pr_info2("---> it6265_hdcp_task_check_ksv_list ::");
    g_dev.vars.hdcp_cmd = HDCP_KSV_CHK;
    g_dev.vars.hdcp_cmd_tick = 0;
    g_dev.vars.hdcp_cmd_timeout = 1;    // do command after 1*100 ms
    it6265_enable_timestamp(1);
}

void It6265::it6265_hdcp_task_polling_bcaps( void )
{
    pr_info2("---> it6265_hdcp_task_polling_bcaps ::");
    g_dev.vars.hdcp_cmd = HDCP_POLLING_BCAPS;
    g_dev.vars.hdcp_cmd_tick = 0;
    g_dev.vars.hdcp_cmd_count = 0;
    g_dev.vars.hdcp_cmd_timeout = 1;    // do command after 1*100 ms
    it6265_enable_timestamp(1);
}

void It6265::it6265_avi_task_wait_for_audio_stable( void )
{
    g_dev.vars.avi_cmd = AVICMD_WAIT_FOR_AUDIO;
    g_dev.vars.avi_cmd_tick = 0;
    g_dev.vars.avi_cmd_timeout = 4;    // do command after 4*100 ms
    it6265_enable_timestamp(1);
}

void It6265::it6265_avi_task_sys_unplug( void )
{
    g_dev.vars.avi_cmd = AVICMD_SYS_UNPLUG;
    g_dev.vars.avi_cmd_tick = 0;
    g_dev.vars.avi_cmd_timeout = 1;    // do command after 100 ms
    it6265_enable_timestamp(1);
}

void It6265::it6265_avi_task_show_audio_info( void )
{
    g_dev.vars.avi_cmd = AVICMD_SHOW_AUD_INFO;
    g_dev.vars.avi_cmd_tick = 0;
    g_dev.vars.avi_cmd_timeout = 10;    // do command after 1000 ms
    it6265_enable_timestamp(1);
}

void It6265::it6265_avi_task_check_video_in_stable( void )
{
    g_dev.vars.avi_cmd = AVICMD_CHECK_VIDEO_IN_STABLE;
    g_dev.vars.avi_cmd_tick = 0;
    g_dev.vars.avi_cmd_timeout = 2;    // do command after 2*100 ms
    it6265_enable_timestamp(1);
}

// reg10 show info only
void It6265::it6265_irq_vborx_reg10(u8 RxReg10)
{
    u8 rddata;

    if (RxReg10 & 0x01) {
        pr_info2("VBO RX CDR Done Interrupt ...\r\n");
        pr_info2("Lane Clock Detection = 0x%02X\r\n\r\n", vborxrd(0x39));
    }

    if (RxReg10 & 0x02) {
        pr_info2("VBO RX Align Pass Interrupt ...\r\n\r\n");
    }

    if (RxReg10 & 0x04) {

        pr_info2("VBO RX Pixel Align Stable Interrupt ...\r\n");

        rddata = vborxrd(0x84) & 0x03;
        pr_info2("Current Lx_CSEL = %d => ", rddata);
        switch (rddata) {
        case 0 :
            pr_info2("Default\r\n"); break;
        case 1 :
            pr_info2("1.0GHz~2.0GHz\r\n"); break;
        case 2 :
            pr_info2("500MHz~1.0GHz\r\n"); break;
        default:
            pr_info2("Incorrect !!!\r\n"); break;
        }

        rddata = vborxrd(0xB0) & 0x03;
        pr_info2("Current CPLL_PRED = %d => ", rddata);
        switch (rddata) {
        case 0 :
            pr_info2("600MHz~800MHz\r\n"); break;
        case 1 :
            pr_info2("800MHz~1.6GHz\r\n"); break;
        case 2 :
            pr_info2("1.6GHz~3.2GHz\r\n"); break;
        default:
            pr_info2("3.2GHz~4.0GHz\r\n"); break;
        }

        rddata = vborxrd(0xB6);
        pr_info2("Current OPLL_PRED = %d => ", (rddata & 0x10) >> 4);
        if ((rddata & 0x10) == 0x00) {
            if (g_dev.vars.bytenum == 3)
                pr_info2("ERROR: 3-Byte Mode should be set '1' !!!\r\n");
            if (g_dev.vars.bytenum == 4)
                pr_info2("4-Byte Mode and BitRate<800MHz\r\n");
            if (g_dev.vars.bytenum == 5)
                pr_info2("5-Byte Mode and BitRate<1.0GHz\r\n");
        } else {
            if (g_dev.vars.bytenum == 3)
                pr_info2("3-Byte Mode should be set '1'\r\n");
            if (g_dev.vars.bytenum == 4)
                pr_info2("4-Byte Mode and BitRate>800MHz\r\n");
            if (g_dev.vars.bytenum == 5)
                pr_info2("5-Byte Mode and BitRate>1.0GHz\r\n");
        }

        pr_info2("Current OPLL_PCLKDIV2 = %d => ", (rddata & 0x40) >> 6);
        if (rddata & 0x40)
            pr_info2("PixClk>100MHz or RegEnAutoOPDiv2=1 at 8-lane mode\r\n");
        else
            pr_info2("PixClk<100MHz\r\n");
        pr_info2("Current OPLL_ENPIXCLK = %d\r\n", (rddata & 0x80) >> 7);
        pr_info2("\r\n");
    }

    if (RxReg10 & 0x08) {
        pr_info2("VBO RX Align TimeOut Interrupt ...\r\n\r\n");
    }

    if (RxReg10 & 0x10) {
        pr_info2("VBO RX Pixel Align TimeOut Interrupt ...\r\n\r\n");
    }

    if (RxReg10 & 0x20) {
        pr_info2("VBO RX Normal Operation Fail Interrupt ...\r\n\r\n");
    }

    if (RxReg10 & 0x40) {
        pr_info2("*** VBO RX ReTrain Interrupt ... \r\n\r\n");
    }

    if (RxReg10 & 0x80) {
        pr_info2("VBO RX Pixel FIFO Auto Reset Interrupt ... \r\n\r\n");
        it6265_check_vborx_fifo();
    }
}

void It6265::it6265_irq_vborx_reg11(u8 RxReg11)
{
    if (RxReg11 & 0x01) {
        pr_info2("VBO RX S2S FIFO Over-Flow Interrupt ...\r\n\r\n");
        it6265_check_vborx_fifo();
    }

    if (RxReg11 & 0x02) {
        pr_info2("VBO RX S2S FIFO Under-Flow Interrupt ...\r\n\r\n");
        it6265_check_vborx_fifo();
    }

    if (RxReg11 & 0x04) {
        pr_info2("VBO RX Symbol Align End Interrupt ...\r\n\r\n");
    }

    if (RxReg11 & 0x08) {

        pr_info2("VBO RX Lock Stable Change Interrupt ...\r\n");
        pr_info2("Lane Symbol Lock Stable = 0x%02X\r\n", vborxrd(0x3B));
        pr_info2("Lane1/0 Symbol Lock Position = 0x%02X\r\n", vborxrd(0x3C));
        pr_info2("Lane3/2 Symbol Lock Position = 0x%02X\r\n", vborxrd(0x3D));
        pr_info2("Lane5/4 Symbol Lock Position = 0x%02X\r\n", vborxrd(0x3E));
        pr_info2("Lane7/6 Symbol Lock Position = 0x%02X\r\n", vborxrd(0x3F));
    }

    if (RxReg11 & 0x10) {

        pr_info2("VBO RX Symbol Lock Change Interrupt ...\r\n");
        pr_info2("Lane Symbol Lock Status = 0x%02X\r\n", vborxrd(0x3A));
    }

    if (RxReg11 & 0x20) {

        pr_info2("VBO RX Detect All Boundary Change Interrupt ...\r\n");
        pr_info2("Pixel Boundary Detection = 0x%02X\r\n", vborxrd(0x4C));

        pr_info2("Lane0 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x48)&0x07)>>0);
        pr_info2("Lane1 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x48)&0x70)>>4);
        pr_info2("Lane2 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x49)&0x07)>>0);
        pr_info2("Lane3 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x49)&0x70)>>4);
        pr_info2("Lane4 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x4A)&0x07)>>0);
        pr_info2("Lane5 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x4A)&0x70)>>4);
        pr_info2("Lane6 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x4B)&0x07)>>0);
        pr_info2("Lane7 Pixel Align Boundary = 0x%X\r\n", (vborxrd(0x4B)&0x70)>>4);
    }

    if (RxReg11 & 0x40) {

        pr_info2("VBO RX IPLL_LOCK Change Interrupt ... IPLL_LOCK = %d\r\n", (vborxrd(0x07) & 0x04) >> 2);
        pr_info2("\r\n");
    }

    if (RxReg11 & 0x80) {

        pr_info2("VBO RX OPLL_LOCK Change Interrupt ... OPLL_LOCK = %d\r\n", (vborxrd(0x07) & 0x10) >> 4);
        pr_info2("\r\n");
    }
}

void It6265::it6265_irq_vborx_reg12(u8 RxReg12)
{
    u8 rddata;

    // Interrupt RxReg12
    if (RxReg12 & 0x01) {

        pr_info2("VBO RX Clock Detect Stable Interrupt ...\r\n");
        pr_info2("VBO RX Clock Detect Status = 0x%02X\r\n\r\n", vborxrd(0x39));
    }
    if (RxReg12 & 0x02) {

        pr_info2("VBO RX Align Detection TimeOut Interrupt ...\r\n");
    }

    if (RxReg12 & 0x04) {

        pr_info2("VBO RX Pixel Align TimeOut Interrupt ...\r\n");
    }

    if (RxReg12 & 0x08) {

        pr_info2("VBO RX Pixel K-code Detection TimeOut Interrupt ...\r\n");
    }

    if (RxReg12 & 0x10) {

        pr_info2("*** VBO RX Detectk Skew Pixel K-code Interrupt ... \r\n");
    }

    if (RxReg12 & 0x20) {

        pr_info2("VBO RX Video Stable Change Interrupt ...\r\n");

        if( g_dev.cfg.EnPiRecParaChgInt )
        {
            vborxset(0x17, 0x08, 0x00);
            idle(100);
        }

        if (vborxrd(0x07) & 0x20) {
            pr_info2("VBO RX Video is Stable ...\r\n");

            if( g_dev.cfg.EnPiRecParaChgInt==false )
            {
                vborxwr(0x13, 0x08);           // Clear PiRecParaChg Interrupt
                vborxset(0x17, 0x08, 0x08);    // Enable PiRecParaChg Interrupt
            }

            it6265_cal_txclk();
            it6265_setup_hdmitx_afe();
        }
        else {
            pr_info2("VBO RX Video is UnStable ...\r\n");
            if( g_dev.cfg.EnPiRecParaChgInt ==false )
            {
                vborxset(0x17, 0x08, 0x00);    // Disable PiRecParaChg Interrupt
                vborxwr(0x13, 0x08);           // Clear PiRecParaChg Interrupt
            }
        }

        it6265_show_vbo_input_info();
        it6265_show_vbo_output_info();

        if( g_dev.cfg.EnPiRecParaChgInt )
        vborxset(0x17, 0x08, 0x08);
    }

    if (RxReg12 & 0x40) {
        pr_info2("VBORX Video Parameter Change Interrupt ...\r\n");

        rddata = vborxrd(0xDB) & 0x0F;
        vborxwr(0x12, 0x40);

        pr_info2("VBORX Video Parameter Change Status = 0x%01X => ", rddata);
        switch (rddata) {
        case 0x0:
            pr_info2("No Video Parameter Change !!!\r\n"); break;
        case 0x1:
            pr_info2("VSync2nd Width Change !!!\r\n"); break;
        case 0x2:
            pr_info2("VSync Width Change !!!\r\n"); break;
        case 0x3:
            pr_info2("VDE2nd End Change !!!\r\n"); break;
        case 0x4:
            pr_info2("VDE2nd Start Change !!!\r\n"); break;
        case 0x5:
            pr_info2("VSync2nd Start Change !!!\r\n"); break;
        case 0x6:
            pr_info2("VDE End Change !!!\r\n"); break;
        case 0x7:
            pr_info2("VDE Start Change !!!\r\n"); break;
        case 0x8:
            pr_info2("Field2nd VRRise Change !!!\r\n"); break;
        case 0x9:
            pr_info2("HSync Width Change !!!\r\n"); break;
        case 0xA:
            pr_info2("HDE End Change !!!\r\n"); break;
        case 0xB:
            pr_info2("HDE Start Change !!!\r\n"); break;
        case 0xC:
            pr_info2("Line Cnt Change !!!\r\n"); break;
        case 0xD:
            pr_info2("Pixel Cnt Change !!!\r\n"); break;
        }

        g_dev.vars.param_chg_cnt++;
        if( g_dev.vars.param_chg_cnt==20 ) {
            if( vborxrd(0x07)&0x20 ) {
                pr_info2("VBO RX Video is Stable ....\r\n");
                it6265_show_vbo_input_info();
                it6265_show_vbo_output_info();
            }
            else {
                pr_info2("VBO RX Video is UnStable ....\r\n");
            }
            g_dev.vars.param_chg_cnt = 0;
        }

        pr_info2("\r\n");
    }

    if (RxReg12 & 0x80) {

        pr_info2("VBO RX K-code Error Interrupt ...\r\n");
        pr_info2("\r\n");
    }
}

void It6265::it6265_irq_vborx_reg13(u8 RxReg13)
{
    int PiRecParaChgSts;
    u8 rddata;
    u8 csel, cppred, oppred;

    // Interrupt RxReg13
    if (RxReg13 & 0x01) {

        pr_info2("VBO RX CPLL_LOCK Change Interrupt ... CPLL_LOCK is %d \r\n", (vborxrd(0x07) & 0x08) >> 3);
        pr_info2("\r\n");
    }

    if (RxReg13 & 0x02) {

        pr_info2("VBO RX DSSC_LOCK Change Interrupt ... DSSC_LOCK is %d \r\n", (vborxrd(0x07) & 0x20) >> 5);
        pr_info2("\r\n");
    }

    if (RxReg13 & 0x04) {
        u32 HwDetBitRate, HwDetPCLKOut;
        u8 reg, lrst;

        pr_info2("VBO RX CR_CLK Detected Interrupt ... \r\n");

        rddata = ((vborxrd(0x2C) & 0x01) << 8) + vborxrd(0x2B);

        if (rddata == 0) {
            pr_info2("ERROR: RAutoClkCnt=0 !!!\r\n");
        }
        else {
            HwDetBitRate = 128 * 40 * g_dev.vars.RCLK / rddata;
            pr_info2("VBO RX CR_CLK Count = 0x%03X, HW Detection BitRate = %u kHz\r\n", rddata, HwDetBitRate);
            HwDetPCLKOut = HwDetBitRate / 10 * g_dev.vars.lanenum / g_dev.vars.bytenum;
            pr_info2("VBO RX HW Detection OPLL_PCLKOUT = %u kHz\r\n", HwDetPCLKOut);

            reg = vborxrd(0x86);
            lrst = 0;

            if ( HwDetBitRate > 1000000UL )  { // bitrate > 1G
                if ( reg != 0xFF ) {
                    vborxwr(0x86, 0xFF);
                    lrst = 1;
                }
            }
            else {
                if ( reg != 0x00 ) {
                    vborxwr(0x86, 0x00);
                    lrst = 1;
                }
            }

            if ( lrst ) {
                vborxwr(0x8A, 0xFF);
                vborxwr(0x8A, 0x00);
                pr_info2("LRST ::0x%02X\r\n", vborxrd(0x86));
            }

            if ((vborxrd(0x2C) & 0x02) == 0x00 || rddata == 0 || HwDetBitRate < 595 * 1000) {
                pr_info2("Invalid VBO RX CR_CLK detected !!!\r\n");

                vborxset(0x0C, 0x23, 0x00);    // RegEnRxClkDet=0
                vborxset(0x0C, 0x23, 0x23);    // RegEnRxClkDet=1
                vborxset(0x0D, 0x04, 0x00);

            }
            else {
                pr_info2("Current Lane Number = %d\r\n", (int)g_dev.vars.lanenum);
                pr_info2("Current Byte Mode: %d-byte Mode\r\n\r\n", (int)g_dev.vars.bytenum);

                csel = vborxrd(0x84);
                cppred = vborxrd(0xB0) & 0x03;
                oppred = vborxrd(0xB6) & 0x10;

                pr_info2("VBO RX Current CSEL=%d, CPPRED=%d, OPPRED=%d\r\n", (int)(csel & 0x03), (int)cppred, (int)oppred >> 4);
            }

        }
    }

    if (RxReg13 & 0x08) {


        pr_info2("VBO RX PiRec Parameter Change Interrupt ... \r\n");
        PiRecParaChgSts = (vborxrd(0xDA) << 8) + vborxrd(0xD9);


        pr_info2("PiRecParaChgSts = 0x%04X\r\n", PiRecParaChgSts);
        chgrxbank(1);
        if (PiRecParaChgSts & 0x0001) {
            pr_info2("HTotal    ParaChg, PiRecValue = %d\r\n", (vborxrd(0x11) << 8) + vborxrd(0x10));
        }
        if (PiRecParaChgSts & 0x0002) {
            pr_info2("HDES      ParaChg, PiRecValue = %d\r\n", (vborxrd(0x13) << 8) + vborxrd(0x12));
        }
        if (PiRecParaChgSts & 0x0004) {
            pr_info2("HDEE      ParaChg, PiRecValue = %d\r\n", (vborxrd(0x15) << 8) + vborxrd(0x14));
        }
        if (PiRecParaChgSts & 0x0008) {
            pr_info2("HSyncE    ParaChg, PiRecValue = %d\r\n", (vborxrd(0x19) << 8) + vborxrd(0x18));
        }
        if (PiRecParaChgSts & 0x0010) {
            pr_info2("VTotal    ParaChg, PiRecValue = %d\r\n", (vborxrd(0x1b) << 8) + vborxrd(0x1a));
        }
        if (PiRecParaChgSts & 0x0020) {
            pr_info2("VDES      ParaChg, PiRecValue = %d\r\n", (vborxrd(0x1d) << 8) + vborxrd(0x1c));
        }
        if (PiRecParaChgSts & 0x0040) {
            pr_info2("VDEE      ParaChg, PiRecValue = %d\r\n", (vborxrd(0x1f) << 8) + vborxrd(0x1e));
        }
        if (PiRecParaChgSts & 0x0080) {
            pr_info2("VDES2nd   ParaChg, PiRecValue = %d\r\n", (vborxrd(0x21) << 8) + vborxrd(0x20));
        }
        if (PiRecParaChgSts & 0x0100) {
            pr_info2("VDEE2nd   ParaChg, PiRecValue = %d\r\n", (vborxrd(0x23) << 8) + vborxrd(0x22));
        }
        if (PiRecParaChgSts & 0x0200) {
            pr_info2("VSyncE    ParaChg, PiRecValue = %d\r\n", (vborxrd(0x27) << 8) + vborxrd(0x26));
        }
        if (PiRecParaChgSts & 0x0400) {
            pr_info2("VRS2nd    ParaChg, PiRecValue = %d\r\n", (vborxrd(0x29) << 8) + vborxrd(0x28));
        }
        if (PiRecParaChgSts & 0x0800) {
            pr_info2("VRE2nd    ParaChg, PiRecValue = %d\r\n", (vborxrd(0x2b) << 8) + vborxrd(0x2a));
        }
        if (PiRecParaChgSts & 0x1000) {
            pr_info2("2ndVRRise ParaChg, PiRecValue = %d\r\n", (vborxrd(0x2d) << 8) + vborxrd(0x2c));
        }
        pr_info2("\r\n");
        chgrxbank(0);
    }
}

void It6265::it6265_irq_hdmitx_reg12(u8 TxReg12)
{
    u8 rddata;

    if (TxReg12 & 0x01) {

        pr_info2("HPD Change Interrupt => ");

        if (it6265_is_hpd_high()) {
            pr_info2("HPD ON\r\n");
            //it6265_sys_state(SYS_Unplug);

            #if 0
            if ( g_dev.vars.avi_cmd == AVICMD_SYS_UNPLUG )
            {
                g_dev.vars.avi_cmd = AVICMD_NONE;
                g_dev.vars.avi_cmd_timeout = 0;
            }
            #endif

            if ( g_dev.vars.prev_hpd == 1 )
            {
                g_dev.vars.hdcp_going = 0;
                it6265_sys_state(SYS_Unplug);
                //it6265_sys_state(SYS_HPD);
                hdmitxwr(0x12, hdmitxrd(0x12));
            }
            else
            {
                if ( g_dev.vars.SysState == SYS_Unplug )
                {
                    it6265_sys_state(SYS_HPD);
                }
            }

            //g_dev.vars.prev_hpd = 1;

            #if ENABLE_CALLBACK
            if ( g_dev.cb.it6265_hpd_change_cb ) {
                g_dev.cb.it6265_hpd_change_cb(1,g_dev.cb_data.hpd_change_cb_data);
            }
            #endif
        }
        else {
            pr_info2("HPD OFF\r\n");
            g_dev.vars.hdcp_going = 0;
            it6265_sys_state(SYS_Unplug);

            g_dev.vars.prev_hpd = 0;

            #if ENABLE_CALLBACK
            if ( g_dev.cb.it6265_hpd_change_cb ) {
                g_dev.cb.it6265_hpd_change_cb(0,g_dev.cb_data.hpd_change_cb_data);
            }
            #endif

            return;
        }
    }

    if (TxReg12 & 0x02) {

        it6265_gate_rclk(0);

        pr_info2("RxSen Change Interrupt => ");

        if (it6265_is_hpd_high()) {
            pr_info2("HPD ON\r\n");

            if ( g_dev.vars.SysState == SYS_Unplug ) {
                it6265_sys_state(SYS_HPD);
            }

            if (it6265_is_rx_sense()) {
                pr_info2("RxSen ON\r\n");
                if ( g_dev.vars.SysState == SYS_WaitRxSen )
                {
                    it6265_sys_state( SYS_RxSen );
                }

                #if ENABLE_CALLBACK
                if ( g_dev.cb.it6265_rxsen_change_cb ) {
                    g_dev.cb.it6265_rxsen_change_cb(1,g_dev.cb_data.rxsen_change_cb_data);
                }
                #endif
            }
            else {
                pr_info2("RxSen OFF\r\n");
                it6265_sys_state(SYS_WaitRxSen);

                #if ENABLE_CALLBACK
                if ( g_dev.cb.it6265_rxsen_change_cb ) {
                    g_dev.cb.it6265_rxsen_change_cb(0,g_dev.cb_data.rxsen_change_cb_data);
                }
                #endif
            }
        }
        else {
            pr_info2("HPD OFF\r\n");
            it6265_sys_state(SYS_Unplug);
            return;
        }
    }

    if (TxReg12 & 0x04) {
        pr_info2("DDC Command Fail Interrupt ...\r\n");

        //hdmitxset(0x28, 0x01, 0x01);
        //hdmitxwr(0x2E, 0x0F);
        //hdmitxset(0x28, 0x01, 0x00);
        it6265_ddc_abort();
    }

    if (TxReg12 & 0x08) {
        pr_info2("DDC FIFO Error Interrupt ...\r\n");

        hdmitxset(0x28, 0x01, 0x01);
        hdmitxwr(0x2E, 0x09);
        hdmitxset(0x28, 0x01, 0x00);
    }

    if (TxReg12 & 0x10) {
        pr_info2("DDC FIFO Valid Interrupt ...\r\n");
        // Add function here to read the data
    }

    if (TxReg12 & 0x20) {
        pr_info2("Audio FIFO Error Interrupt ...\r\n");

        hdmitxset(0x05, 0x20, 0x20);
        idle(1);
        hdmitxset(0x05, 0x20, 0x00);
        it6265_audio_state(Aud_Reset);
        it6265_audio_state(Aud_WaitForAudIn);
    }

    if (TxReg12 & 0x40) {
        pr_info2("VSync Event Interrupt ...\r\n");
    }

    if (TxReg12 & 0x80) {
        pr_info2("H2TX Video Stable change Interrupt ...\r\n");

        rddata = hdmitxrd(0x07);

        if (rddata & 0x10) {
            pr_info2("TMDS is Stable, ");
        }
        else {
            pr_info2("TMDS is Unstable, ");

            //TX FIFO manual reset
            hdmitxset(0x94, 0x01, 0x01);
            hdmitxset(0x94, 0x01, 0x00);

            if ( g_dev.vars.VState >= Vid_VidInStable ) {
                // For Video Input Change Without Information
                pr_info2("INT: Vid_VidOutEnable = > Vid_Reset ...\r\n");
                it6265_set_avmute(1);
                it6265_video_state(Vid_Reset);
                it6265_audio_state(Aud_Reset);
                it6265_video_state(Vid_WaitForVidIn);
                it6265_audio_state(Aud_WaitForAudIn);
            } else {
                pr_info2("INT: Vid %d = > Vid_Reset ...\r\n", g_dev.vars.VState);
            }
        }

        pr_info2("TxReg07=0x%02X\r\n", rddata);

        if (it6265_is_video_in_stable()) {
            pr_info2("H2TX Video Stable On Interrupt ...VState=%d\r\n", g_dev.vars.VState);

            if (g_dev.vars.VState == Vid_WaitForVidIn) {
                it6265_video_state(Vid_VidInStable);
            }
            else {
                if (  (g_dev.vars.HdcpState == CP_Done) || (g_dev.vars.HdcpState == CP_Going) )
                {
                    #if 0
                    pr_info2("#### it6265_hdcp_task_retry_auth---- by H2TX Video Stable On Interrupt ");
                    // this may cause 1B-02 fail with SL-8800
                    // this may cause 1B-02 fail with SL-8800
                    // this may cause 1B-02 fail with SL-8800
                    it6265_hdcp_state(CP_Fail);
                    it6265_hdcp_task_retry_auth();
                    #endif
                }
            }
        } else {
            pr_info2("H2TX Video Stable Off Interrupt ...VState=%d\r\n", g_dev.vars.VState);

            if ( g_dev.vars.VState >= Vid_VidInStable ) {
                // For Video Input Change Without Information
                pr_info2("INT: Vid_VidOutEnable = > Vid_Reset ...\r\n");
                it6265_set_avmute(1);
                it6265_video_state(Vid_Reset);
                it6265_audio_state(Aud_Reset);
                it6265_video_state(Vid_WaitForVidIn);
                it6265_audio_state(Aud_WaitForAudIn);
            } else {
                pr_info2("INT: Vid %d = > Vid_Reset ...\r\n", g_dev.vars.VState);
            }
        }
    }
}

void It6265::it6265_irq_hdmitx_reg13(u8 TxReg13)
{
    if (TxReg13 & 0x01) {
        g_dev.vars.hdcp_going = 0;
        it6265_hdcp_state(CP_Fail);
        it6265_hdcp_auth_fail();

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp_fail_cb ) {
            g_dev.cb.it6265_hdcp_fail_cb( g_dev.vars.HDCP2Sel, g_dev.vars.hdcp2_fail_status, g_dev.cb_data.hdcp_fail_cb_data );
        }
        #endif
    }
    if (TxReg13 & 0x02)
    {
        g_dev.vars.hdcp_going = 0;
    }

    if ((TxReg13 & 0x02) && (g_dev.vars.HdcpState == CP_Going)) {

        if (g_dev.vars.HDCP2Sel == false) {
            pr_info2("HDCP1 Authentication Done Interrupt ...\r\n\r\n");
            it6265_hdcp1_show_status();

            #if ENABLE_CALLBACK
            if ( g_dev.cb.it6265_hdcp_done_cb ) {
                g_dev.cb.it6265_hdcp_done_cb( 1, g_dev.cb_data.hdcp_done_cb_data );
            }
            #endif
        }
        else {
            pr_info2("HDCP2 Authentication Done Interrupt ...\r\n\r\n");
            it6265_hdcp2_show_status();
            g_dev.vars.hdcp2_fail_count = 0;

            #if ENABLE_CALLBACK
            if ( g_dev.cb.it6265_hdcp2_rxid_cb ) {
                u8 rxid[5];
                hdmitxbrd(0x5B, 5, rxid);
                g_dev.cb.it6265_hdcp2_rxid_cb( rxid, g_dev.cb_data.hdcp2_rxid_cb_data );
            }
            #endif

            #if ENABLE_CALLBACK
            if ( g_dev.cb.it6265_hdcp_done_cb ) {
                g_dev.cb.it6265_hdcp_done_cb( 2, g_dev.cb_data.hdcp_done_cb_data );
            }
            #endif
        }

        it6265_hdcp_state(CP_Done);
    }

    if (TxReg13 & 0x04) {
        pr_info2("HDCP KSV List Check Interrupt ... \r\n");
        it6265_hdcp_task_polling_bcaps();
    }

    if (TxReg13 & 0x08) {
        pr_info2("Input Line Buffer Over-Flow Interrupt ...\r\n\r\n");
        hdmitxset(0x95, 0x0a, 0x0a);
        hdmitxset(0x95, 0x0a, 0x00);
    }

    if (TxReg13 & 0x10) {
        pr_info2("Input Synchronization FIFO Over-Flow Interrupt ...\r\n\r\n");
        hdmitxset(0x95, 0x02, 0x02);
        hdmitxset(0x95, 0x02, 0x00);
    }

    if (TxReg13 & 0x20) {
        pr_info2("Input DE Synchronization Lose Interrupt ...\r\n\r\n");
    }

    if (TxReg13 & 0x40) {
        pr_info2("Packet Event Done Interrupt ...\r\n");
        pr_info2("Current Event is ");

        switch ( (hdmitxrd(0x10) & 0x0F) ) {
        case 0x0:
            pr_info2("GenCtrl Packet\r\n"); break;
        case 0x1:
            pr_info2("Null Packet\r\n"); break;
        case 0x2:
            pr_info2("ACP Packet\r\n"); break;
        case 0x3:
            pr_info2("AVI Packet\r\n"); break;
        case 0x4:
            pr_info2("Audio Sample Packet\r\n"); break;
        case 0x5:
            pr_info2("MPEG Packet\r\n"); break;
        case 0x6:
            pr_info2("GBD Packet\r\n"); break;
        case 0x7:
            pr_info2("Vendor Specific Packet\r\n"); break;
        case 0x8:
            pr_info2("Audio CTS Change, Step = %d\r\n", (hdmitxrd(0x10) & 0x30) >> 4);
            {
                int AudCTSCnt;
                chgtxbank(1);
                AudCTSCnt = ((hdmitxrd(0x35) & 0xF0) >> 4);
                AudCTSCnt += (hdmitxrd(0x36) << 4);
                AudCTSCnt += (hdmitxrd(0x37) << 12);
                chgtxbank(0);
                pr_info2("Audio Time Stamp CTSCnt = 0x%04X\r\n", AudCTSCnt);
            }
            break;
        case 0x9:
            pr_info2("Audio Meta Packet\r\n"); break;
        default :
            pr_info2("Reserved\r\n"); break;
        }
    }
}

void It6265::it6265_irq_hdmitx_reg16(u8 TxReg16)
{
    u8 rddata;

    if (TxReg16 & 0x01) {
        pr_info2("Audio Decode Error Interrupt ...\r\n");
    }

    if (TxReg16 & 0x02) {
        pr_info2("No Audio Interrupt ...\r\n");

        it6265_audio_state(Aud_Reset);
        it6265_audio_state(Aud_WaitForAudIn);
    }

    if (TxReg16 & 0x04) {
        pr_info2("Video FIFO Error Interrupt (AutoReset) ...\r\n");
    }

    if (TxReg16 & 0x08) {
        pr_info2("DDC Bus Hang Interrupt ...\r\n");
        it6265_ddc_error_handler();
    }

    if (TxReg16 & 0x10) {
        it6265_hdcp_show_ri();
    }

    if (TxReg16 & 0x20) {
        it6265_hdcp_show_pj();
    }

    if (TxReg16 & 0x40) {
        pr_info2("H2TX Video Parameter Change Interrupt ...\r\n");

        rddata = hdmitxrd(0x9F) & 0x0F;
        pr_info2("H2TX Video Parameter Change Status = 0x%02X => ", (int)rddata);
        switch (rddata) {
        case 0x0:
            pr_info2("No Video Parameter Change !!!\r\n"); break;
        case 0x1:
            pr_info2("VSync2nd Width Change !!!\r\n"); break;
        case 0x2:
            pr_info2("VSync Width Change !!!\r\n"); break;
        case 0x3:
            pr_info2("VDE2nd End Change !!!\r\n"); break;
        case 0x4:
            pr_info2("VDE2nd Start Change !!!\r\n"); break;
        case 0x5:
            pr_info2("VSync2nd Start Change !!!\r\n"); break;
        case 0x6:
            pr_info2("VDE End Change !!!\r\n"); break;
        case 0x7:
            pr_info2("VDE Start Change !!!\r\n"); break;
        case 0x8:
            pr_info2("Field2nd VRRise Change !!!\r\n"); break;
        case 0x9:
            pr_info2("HSync Width Change !!!\r\n"); break;
        case 0xA:
            pr_info2("HDE End Change !!!\r\n"); break;
        case 0xB:
            pr_info2("HDE Start Change !!!\r\n"); break;
        case 0xC:
            pr_info2("Line Cnt Change !!!\r\n"); break;
        case 0xD:
            pr_info2("Pixel Cnt Change !!!\r\n"); break;
        }
        hdmitxwr(0x16,0x40);
    }

    if (TxReg16 & 0x80) {
        u8 auth_state = (hdmitxrd(0x40) & 0x7f) >> 1 ;
        pr_info2("HDCP2RepWaitTxMgm Interrupt !!!\r\n");
        pr_info2("HDCP2Tx auth current state = %x\r\n", (int)(auth_state));

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp2_repeater_wait_tx_stream_manage_cb ) {
            g_dev.cb.it6265_hdcp2_repeater_wait_tx_stream_manage_cb( g_dev.cb_data.hdcp2_repeater_wait_tx_stream_manage_cb_data );
        }
        #endif

        if ( g_dev.cfg.IsHdcpRepeater < 1 ) {
            it6265_hdcp2_continue_stream_manage();
        }
    }
}

void It6265::it6265_scdc_update_flag_status_handler( u8 rddata )
{
    #if CHECK_HPD_BEFORE_RELAY_SCDC
    if (false == it6265_is_hpd_high())
    {
        return;
    }
    #endif

    if (rddata & 0x01) {

        it6265_scdc_write(0x10, 0x01); // W1C

        if (it6265_scdc_read(0x21, 1) == true){
            pr_info2("SCDC Scrambling_Status = %d\r\n", hdmitxrd(0x30) & 0x01);
        }

        if (it6265_scdc_read(0x40, 1) == true) {
            u8 sts_flag;

            sts_flag = hdmitxrd(0x30);
            pr_info2("SCDC Clock_Detected = %d\r\n", sts_flag & 0x01);
            pr_info2("SCDC Ch0_Locked = %d\r\n", (sts_flag & 0x02) >> 1);
            pr_info2("SCDC Ch1_Locked = %d\r\n", (sts_flag & 0x04) >> 2);
            pr_info2("SCDC Ch2_Locked = %d\r\n", (sts_flag & 0x08) >> 3);
        }
    }

    if (rddata & 0x02) {
        u8 i;

        it6265_scdc_write(0x10, 0x02); // W1C
        if (it6265_scdc_read(0x50, 7) == true) {
            u8 CED[7], sum, clip_sum;

            hdmitxbrd(0x30, 7, &CED[0]);
            pr_info2("SCDC Ch0 Valid=%d, Channel 0 Error Count = 0x%04X\r\n", (CED[1] & 0x80) >> 7, ((CED[1] & 0xEF) << 8) + CED[0]);
            pr_info2("SCDC Ch1 Valid=%d, Channel 1 Error Count = 0x%04X\r\n", (CED[3] & 0x80) >> 7, ((CED[3] & 0xEF) << 8) + CED[2]);
            pr_info2("SCDC Ch2 Valid=%d, Channel 2 Error Count = 0x%04X\r\n", (CED[5] & 0x80) >> 7, ((CED[5] & 0xEF) << 8) + CED[4]);
            pr_info2("SCDC Check Sum  = 0x%04X\r\n", CED[6]);

            sum = CED[0];

            for (i = 1; i < 7; i++){
                sum += CED[i];
            }

            clip_sum = (sum & 0xFF);

            if (clip_sum == 0){
                //pr_info2("SCDC Checksum of Character Error Detection is Correct ...\r\n");
            }
            else{
                pr_info2("SCDC Checksum of Character Error Detection is Error !!!\r\n");
            }
        }
    }

    if (rddata & 0x04) {
        it6265_scdc_write(0x10, 0x04); // W1C
        pr_info2("SCDC RR_Test ...\r\n");
    }
}

void It6265::it6265_scdc_update_flag_handler( void )
{
    u8 rddata;

    #if CHECK_HPD_BEFORE_RELAY_SCDC
    if (false == it6265_is_hpd_high())
    {
        return;
    }
    #endif

    rddata = hdmitxrd(0x3E);
    pr_info2("SCDC Update Flags = 0x%02X\r\n", rddata);
    it6265_scdc_update_flag_status_handler(rddata);
}

void It6265::it6265_scdc_rr_handler( void )
{
    u8 rddata;

    #if CHECK_HPD_BEFORE_RELAY_SCDC
    if (false == it6265_is_hpd_high())
    {
        return;
    }
    #endif

    if (it6265_scdc_read(0x10, 1) == true) {
        rddata = hdmitxrd(0x30);
        pr_info2("SCDC Update Flag = 0x%02X\r\n", rddata);
        it6265_scdc_update_flag_status_handler(rddata);
    } else {
        pr_info2("ERROR: Read SCDC Update Flag Error !!!\r\n");
        rddata = 0x00;
    }

}

void It6265::it6265_irq_hdmitx_reg17(u8 TxReg17)
{
    u8 rxid[5];

    if (TxReg17 & 0x01) {
        pr_info2("HDCP 1.2 Sync. Detection Fail Interrupt !!!\r\n");
    }

    if (TxReg17 & 0x02) {
        pr_info2("HDCP2 SRM check Interrupt !!!\r\n");

        hdmitxbrd(0x5B, 5, &rxid[0]);

        pr_info2("HDCP2 RxID= %02X%02X%02X%02X%02X \r\n", (int)rxid[4],(int)rxid[3],(int)rxid[2],(int)rxid[1],(int)rxid[0]);

        if (it6265_hdcp_is_revocation_rxid(rxid)) {

            pr_info2("This is revoked RxID in HDCP2 ATC test! \r\n");
            pr_info2("Authentication Abort !\r\n");

            it6265_hdcp_state(CP_Fail);
        }
    }

    if (TxReg17 & 0x04) {
        pr_info2("HDCP2RxReauthReq Interrupt !!!\r\n");

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp2_rx_reauth_req_cb ) {
            g_dev.cb.it6265_hdcp2_rx_reauth_req_cb( g_dev.cb_data.hdcp2_rx_reauth_req_cb_data );
        }
        #endif
    }

    if (TxReg17 & 0x08) {
        pr_info2("RHDCP2 StrmMgmFail Interrupt !!!\r\n");

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp2_stream_manage_fail_cb ) {
            g_dev.cb.it6265_hdcp2_stream_manage_fail_cb( g_dev.cb_data.hdcp2_stream_manage_fail_cb_data );
        }
        #endif
    }

    if (TxReg17 & 0x10) {
        pr_info2("HDMI2 SCDC update flag change Interrupt !!!\r\n");
        it6265_scdc_update_flag_handler();
    }

    if (TxReg17 & 0x20) {
        pr_info2("HDMI2 SCDC detect read request Interrupt !!!\r\n");
        it6265_scdc_rr_handler();
    }

    // Read RxID List
    if (TxReg17 & 0x40) {
        int RxMsgSize, RxListTotalBytes, MsgfifoWrptr, MsgfifoRdptr, MsgfifoRdStartAdr;
        //u8 RxListID[155];
        u8 *RxListID = g_dev.data_buffer;
        int i;

        pr_info2("HDCP2ListSRMChk Interrupt !!!\r\n");
        //pr_info2("HDCP2Tx auth current state = %x\r\n", (hdmitxrd(0x40)&0x7f)>>1);

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp2_RxInfo_cb ) {
            u16 rxinfo;
            rxinfo = hdmitxrd(0x65);
            rxinfo <<= 8;
            rxinfo |= hdmitxrd(0x64);

            g_dev.cb.it6265_hdcp2_RxInfo_cb(rxinfo, g_dev.cb_data.hdcp2_rxinfo_cb_data);
        }
        #endif

        RxMsgSize = ((hdmitxrd(0x4d) & 0x03) << 8) + hdmitxrd(0x4c);
        RxListTotalBytes = RxMsgSize - 0x16;
        hdmitxset(0x70, 0x10, 0x10); // set hdcp2 debug enable reg70h[4]=1

        MsgfifoWrptr = ((hdmitxrd(0x74) & 0x30) << 4) + hdmitxrd(0x76);
        MsgfifoRdptr = ((hdmitxrd(0x74) & 0xc0) << 2) + hdmitxrd(0x75);
        pr_info2("RxMsgSize=%03x, RxListTotalBytes=%03X, MsgfifoWrptr=%03x, MsgfifoRdptr=%03x \r\n", RxMsgSize, RxListTotalBytes, MsgfifoWrptr, MsgfifoRdptr);
        hdmitxset(0x72, 0x80, 0x80); // set hdcp2 FSM debug enable to control message fifo read start address

        MsgfifoRdStartAdr = MsgfifoRdptr - RxListTotalBytes;
        pr_info2("MsgfifoRdStartAdr=0x%03x \r\n", MsgfifoRdStartAdr);
        hdmitxwr(0x73, (MsgfifoRdStartAdr & 0x0ff)); // set Message fifo read start address
        hdmitxwr(0x74, (MsgfifoRdStartAdr & 0xf00) >> 8); // set Message fifo read start address

        for (i = 0; i < (RxListTotalBytes / 5); i++) {
            hdmitxbrd(0x77, 5, RxListID+i*5);
            pr_info2("RxListID %d =%02x %02x %02x %02x %02x \r\n", i, RxListID[0], RxListID[1], RxListID[2], RxListID[3], RxListID[4]);
        }

        hdmitxset(0x72, 0x80, 0x00); // clear hdcp2 FSM debug enable
        hdmitxset(0x70, 0x10, 0x00); // clear hdcp2 debug enable
        hdmitxset(0x42, 0x80, 0x80); // set RHDCP2ListRdDone=1 to let auth process continue


        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp2_rxid_list_cb ) {
            g_dev.cb.it6265_hdcp2_rxid_list_cb( RxListTotalBytes/5, RxListID, g_dev.cb_data.hdcp2_rxid_list_cb_data );
        }
        #endif
    }

    if (TxReg17 & 0x80) {

        pr_info2("HDCP2RepWaitTxEks Interrupt !!!\r\n");
        pr_info2("HDCP2Tx auth current state = %x\r\n", (hdmitxrd(0x40) & 0x7f) >> 1);

        #if ENABLE_CALLBACK
        if ( g_dev.cb.it6265_hdcp2_repeater_wait_tx_eks_cb ) {
            g_dev.cb.it6265_hdcp2_repeater_wait_tx_eks_cb( g_dev.cb_data.hdcp2_repeater_wait_tx_eks_cb_data );
        }
        #endif

        if ( g_dev.cfg.IsHdcpRepeater < 1 ) {
            it6265_hdcp2_continue_SEK_send_Eks();
        }
    }
}

void It6265::it6265_irq(void)
{
    u8 RxReg10, RxReg11, RxReg12, RxReg13;
    u8 TxReg12, TxReg13, TxReg16, TxReg17;
    u8 TxReg0F;

    TxReg0F = hdmitxrd(0x0F);

    if ( (TxReg0F & 0x04) != 0x00)  { // HDMI INT
        pr_info("if ( (TxReg0F & 0x04) != 0x00) // HDMI INT \r\n");

        TxReg12 = hdmitxrd(0x12);
        if (TxReg12) {
            pr_info("into TxReg12\r\n");
            hdmitxwr(0x12, TxReg12);
            it6265_irq_hdmitx_reg12(TxReg12);
        }

        TxReg13 = hdmitxrd(0x13);
        if (TxReg13) {
            pr_info("into TxReg13\r\n");
            hdmitxwr(0x13, TxReg13);
            it6265_irq_hdmitx_reg13(TxReg13);
        }

        TxReg16 = hdmitxrd(0x16);
        if (TxReg16) {
            pr_info("into TxReg16\r\n");
            hdmitxwr(0x16, TxReg16&(~0x40));
            it6265_irq_hdmitx_reg16(TxReg16);
        }

        TxReg17 = hdmitxrd(0x17);
        if (TxReg17) {
            pr_info("into TxReg17\r\n");
            hdmitxwr(0x17, TxReg17);
            it6265_irq_hdmitx_reg17(TxReg17);
        }
    }
    else
    {
        pr_info("else // HDMI not INT \r\n");
    }

    if ( (TxReg0F & 0x10) != 0x00) { // VBORX INT
        pr_info("if ( (TxReg0F & 0x10) != 0x00) // VBORX INT \r\n");

        RxReg10 = vborxrd(0x10);
        if (RxReg10) {
            pr_info("into RxReg10\r\n");
            vborxwr(0x10, RxReg10);
            it6265_irq_vborx_reg10(RxReg10);
        }
        RxReg11 = vborxrd(0x11);
        if (RxReg11) {
            pr_info("into RxReg11\r\n");
            vborxwr(0x11, RxReg11);
            it6265_irq_vborx_reg11(RxReg11);
        }
        RxReg12 = vborxrd(0x12);
        if (RxReg12) {
            pr_info("into RxReg12\r\n");
            vborxwr(0x12, RxReg12);
            it6265_irq_vborx_reg12(RxReg12);
        }
        RxReg13 = vborxrd(0x13);
        if (RxReg13) {
            pr_info("into RxReg13\r\n");
            vborxwr(0x13, RxReg13);
            it6265_irq_vborx_reg13(RxReg13);
        }
    }
    else
    {
        pr_info("else // VBORX not INT \r\n");
    }

    #if ENABLE_CEC
        if ( g_dev.vars.SysState >= SYS_HPD )
        {
            it6265_cec_maganer();
        }
    #endif

    if ( g_dev.vars.sys_tick_enabled ) {
        pr_info("if ( g_dev.vars.sys_tick_enabled ) { \r\n");
        g_dev.vars.sys_tick_inc = hdmitxrd(0x09)&0x7f;

        if ( g_dev.vars.sys_tick_inc ) {
            pr_info("if ( g_dev.vars.sys_tick_inc ) {\r\n");
            //pr_info2(":: g_dev.vars.sys_tick_inc = %d\r\n", g_dev.vars.sys_tick_inc);
            it6265_timer_task();
        }
    }
    else
    {
        pr_info("else ( g_dev.vars.sys_tick_enabled ) { \r\n");
    }

}

/*
Dev : Can be NULL if there is only one IT6265 device.

vic : CEA defined video code

byte_mode : acceptable values : 3, 4 or 5.
             3 : Vx1 3 byte mode (  8-bit color depth )
             4 : Vx1 4 byte mode ( 10-bit color depth )
             5 : Vx1 5 byte mode ( 12-bit color depth )

lane_count : acceptable values : 1, 2, 4 or 8.
             1 : Vx1 1 lane mode
             2 : Vx1 2 lane mode
             4 : Vx1 4 lane mode
             8 : Vx1 8 lane mode
*/
int It6265::it6265_set_video_format_by_vic( void *Dev, unsigned short vic, unsigned char byte_mode, unsigned char lane_count )
{
    //VTiming *vt;
    struct it6265dev *dev = (struct it6265dev*)Dev;
    bool ret = false;

    if ( dev == NULL ) {
        dev = &g_dev;
    }

    pr_info("******** config change **********\r\n");
    pr_info("*   Lane = %d, byte mode = %d   *\r\n", lane_count, byte_mode);
    pr_info("*   DeSSC = %d                  *\r\n", g_dev.cfg.EnDESSC);
    pr_info("*********************************\r\n");

    ret = it6265_get_timing_by_vic(vic);

    if ( ret ) {

        switch(lane_count)
        {
            case 8: g_dev.vars.VBOLaneNum = 3; break;
            case 4: g_dev.vars.VBOLaneNum = 2; break;
            case 2: g_dev.vars.VBOLaneNum = 1; break;
            case 1: g_dev.vars.VBOLaneNum = 0; break;
            default :
                ret = false;
                break;
        }

        switch(byte_mode)
        {
            case 3:
                g_dev.vars.VBOByteMode = 0;
                g_dev.cfg.ColorDepth = VID8BIT;
                break;
            case 4:
                g_dev.vars.VBOByteMode = 1;
                g_dev.cfg.ColorDepth = VID10BIT;
                    break;
            case 5:
                g_dev.vars.VBOByteMode = 2;
                g_dev.cfg.ColorDepth = VID12BIT;
                break;
            default :
                ret = false;
                break;
        }
    }

    if ( ret )
    {
        it6265_sys_state(SYS_Unplug);
    }

    return ret;
}

bool It6265::it6265_set_output_format( it6265_video_fmt format )
{
    bool ret = false;

    if ( g_dev.vars.RCLK > 0 ) // the it6265 is initialized
    {
        switch ( format )
        {
            case VF_4K2K60:
                ret = it6265_set_video_format_by_vic(NULL, 97, 4, 8 );
                break;
            case VF_4K2K30:
                ret = it6265_set_video_format_by_vic(NULL, 95, 4, 4 );
                break;
            case VF_1080p:
                ret = it6265_set_video_format_by_vic(NULL, 16, 4, 2 );
                break;
            case VF_720p:
                ret = it6265_set_video_format_by_vic(NULL, 4, 4, 1 );
                break;
            case VF_480p:
                ret = it6265_set_video_format_by_vic(NULL, 3, 4, 1 );
                break;
            default:
                pr_err("Unknown video format : %d\r\n", format);
                break;
        }

        if ( ret )
        {
            it6265_sys_state(SYS_Unplug);
        }
    }

    return ret;
}

void It6265::it6265_init_vars( void )
{
    memset(&g_dev, 0, sizeof(struct it6265dev));
    pr_info("ite6265_init_vars\r\n");

    g_dev.cfg.EnHDMI = true;
    g_dev.cfg.EnHDCP = HDCP_ENABLE;
    //g_dev.cfg.EnCEC = false;

    // VBORX Option
    g_dev.vars.VBOLaneNum = VBO_DEFAULT_LANE_COUNT;     // 0: 1-lane, 1: 2-lane, 2: 4-lane, 3: 8-lane
    g_dev.vars.VBOByteMode = VBO_DEFAULT_BYTE_MODE;    // 0: 3-byte, 1: 4-byte, 2: 5-byte
    g_dev.cfg.EnLxCLKInv = 0xff;   // bit[X]: for LaneX
    g_dev.cfg.EnSCLKInv = VBO_SCLK_INVERSE;  // VBORX SCLK inversion
    g_dev.cfg.EnPCLKInv = VBO_PCLK_INVERSE;  // VBORX PCLK inversion
    g_dev.cfg.SCLKDly = VBO_SCLK_DELAY;      // 0: 0ns, 1: 1ns, 2: 2ns, 3: 3ns
    g_dev.cfg.EnDESSC = VBO_ENABLE_DESSC;

    g_dev.cfg.EnChgAVIPR = false;  // true: for ATC format 6,7,21,22
    g_dev.cfg.DisLockPR = false;   // true: for ATC format 6,7,21,22

    // for HDMI2 SCDC settings
    g_dev.cfg.SCDCFlagByte = false;   //false: 1 byte, true: 2 byte
    g_dev.cfg.FlagPollSel = 0;

    // HDMI output settings
    g_dev.cfg.ColorDepth = OUTPUT_COLOR_DEPTH;
    g_dev.cfg.InColorMode = INPUT_COLOR_MODE;
    g_dev.cfg.OutColorMode = OUTPUT_COLOR_MODE;
    g_dev.cfg.DynRange = DYNAMIC_RANGE;
    g_dev.cfg.YCbCrCoef = CSC_COEFF;
    g_dev.cfg.ColorClip = false;      // 1: clip RGBY to 16~235, CbCr to 16~240

    // Extended Colorimetry option
    g_dev.cfg.ChkExtCol = true;       // check Extended Colorimetry
    g_dev.cfg.EnExtCol = false;
    g_dev.cfg.ExtColFmt = xvYCC709;  // 0: xvYCC601, 1: xvYCC709, 2: sYCC601, 3: AdobeYCC601,
                                     // 4: AdobeRGB, 5: BT2020cYCC, 6: BT2020YCC BT2020 is used for deep-color
    // 3D Option
    g_dev.cfg.En3D = ENABLE_3D;
    g_dev.cfg.Sel3DFmt = SELECT_3D_FORMAT;


    //g_dev.cfg.AutoDis3DVISF = true;
    g_dev.cfg.EnDV3D = false;         // true: Enable 3D Dual-View Signaling (not real 3D)
    g_dev.cfg.EnOSD3D = false;        // true: Enable 3D OSD Disparity Indication (the depth informataion for real 3D)
    g_dev.cfg.EnIVS3D = false;        // true: Enable 3D Independent View Signaling (the dependency information for real 3D)
                                      // Note: when EnDV3D=true, EnOSD3D=EnIVS3D=false and vice versa

    // Dynamic Range and Mastering InfoFrame
    g_dev.cfg.EnDRMInfo = false;
    g_dev.cfg.DRMEOTF = 0;     // EOTF=0: Traditional SDR, 1: Traditional HDR, 2: SMPTE ST 2084 HDR
    g_dev.cfg.DRMZERO = false; // set true for HDR ATC sub-item 17

    // Video Option
    g_dev.cfg.EnBlueScrMute = ENABLE_BLUE_SCREEN_MUTE;

    // H2TX Video Format
    g_dev.cfg.EnDVMode = VBO_2_SECTION_MODE;   // 0: Single, 1: Dual (Left-Right Mode) 2-section
    g_dev.cfg.SwapModeSel = VBO_SWAP_SEL;

    // Audio Option
    g_dev.cfg.AudFullPkt = false;
    g_dev.cfg.AudEn_ori = AUDIO_ENABLE;
    g_dev.cfg.AudSel = AUDIO_SELECT;
    g_dev.cfg.AudFmt = AUDIO_FREQ;
    g_dev.cfg.AudCh = AUDIO_CHANNELS;
    g_dev.cfg.AudType = AUDIO_TYPE;
    g_dev.cfg.AudTypFmt = (I2S_FMT_4<<4)|(I2S_FMT_3<<3)|(I2S_FMT_2<<2)|(I2S_FMT_1<<1)|(I2S_FMT_0<<0);
    g_dev.cfg.AudTypSWL = I2S_WORD_LENGTH;
    g_dev.cfg.EnhSpdifMode = true;
    g_dev.cfg.EnNoAud2Mute = false;
    g_dev.cfg.EnMCLKSample = false;
    g_dev.cfg.MCLKFreq = 0;
    g_dev.cfg.EnhAudCTS = true;  // Average 4 CTS
    g_dev.cfg.AudCTSOpt = true;  // false: Nx2(+1), true: Nx1(+2) for ATC

    g_dev.cfg.EnTDM = false;      // if true, AudSel must be I2S and AudType cannot be DSD and EnAudGen must be true for Internal AudGen
    g_dev.cfg.TDMCh = TDM2CH;     // defautl set TDMCh base on AudCh, but TDM6CH is not allowed for Internal AudGen
    g_dev.cfg.En3DAud = false;    // only for LPCM and DSD when AudCh is more than 8
    g_dev.cfg.EnMSAud = false;    // only for LPCM/NLPCM and DSD when AudCh is less than or equal to 8

    g_dev.cfg.DDCSpeed = DDC75K;      // DDC75K, DDC125K, DDC312K
    g_dev.cfg.DisR0ShortRead = true;  // 980 only support hdcp combine read
    g_dev.cfg.DisRiShortRead = true;  // for ASTRO HDCP CTS
    g_dev.cfg.CKSOURCE = cfg_CKSOURCE;
    g_dev.vars.PixRpt = 1;

    g_dev.cfg.EnDEOnly = 0;
    g_dev.cfg.EnRxDEOnly = 0;
    g_dev.cfg.EnIntPG = 0;
    g_dev.cfg.IsHdcpRepeater = ENABLE_HDCP_REPEATER_FUNCTION_ON_IT6265;

    it6265_get_timing_by_vic(2);
    g_dev.cfg.AutoVIC = 1;

    g_dev.cfg.keep_vborx_connection = 0;
    g_dev.cfg.keep_vborx_connection_prev = g_dev.cfg.keep_vborx_connection;

    // auto variables:
    g_dev.cfg.EnH2Enc = true;
    g_dev.cfg.EnH2Scr = true;
    g_dev.cfg.H2ClkRatio = true;
    g_dev.cfg.EnH2InLBBypass = true;
    g_dev.cfg.En4Kx2K = false;  // VIC93~VIC95, VIC98 can not be used in 2D mode,
                               // En4Kx2K must be true for these 2D mode

    g_dev.vars.HDMI_VIC = 0x04;  // HDMI_VIC = 0x00;  // VIC=95 (HDMI_VIC=1), VIC=94 (HDMI_VIC=2), VIC=93 (HDMI_VIC=3), VIC=98 (HDMI_VIC=4)

    g_dev.vars.VState = Vid_Unknown;
    g_dev.vars.AState = Aud_Unknown;
    g_dev.vars.HdcpState = CP_Unknown;

    g_dev.vars.hdcp2_fail_count = 0;
    g_dev.vars.hdcp2_fail_status_prev = 0;
    g_dev.vars.prev_hpd = 0;
    g_dev.vars.hdcp_going = 0;
}


bool It6265::it6265_is_device_ready(void)
{
    u8 vender_id[2];
    u8 device_id[2];
    static u8 show_id=1;

    vender_id[0] = hdmitxrd(0x00);
    vender_id[1] = hdmitxrd(0x01);
    device_id[0] = hdmitxrd(0x02);
    device_id[1] = hdmitxrd(0x03);

    if ( show_id )
    {
        pr_info("IT6265: Current DevID=%02X%02X\r\n", (int)vender_id[1], (int)vender_id[0]);
        pr_info("IT6265: Current VenID=%02X%02X\r\n", (int)device_id[1], (int)device_id[0]);
    }

    if ((vender_id[0] == 0x54) && (vender_id[1] == 0x49) && (device_id[0] == 0x65) && (device_id[1] == 0x62)) {

        if ( show_id )
        {
            pr_info("IT6265:  IT6265 Device was found!!!\r\n");
            show_id = 0;
        }
        else
        {
            pr_info("IT6265 ready\r\n");
        }

        return true;
    }
    else {
        pr_err("IT6265:  ERROR: Can not find IT6265 Device !!!\r\n");
        return false;
    }
}

void It6265::it6265_chip_init(void)
{
    it6265_init_vars();

    if ( g_dev.cfg.keep_vborx_connection ) // keep_vborx_connection == 0
    {
        pr_info("IT6265: it6265_chip_init if()\r\n");
        g_dev.vars.reset_vborx = 1;
        g_dev.vars.SysState = SYS_Unplug;
        it6265_enable_timestamp(0);
        hdmitxset(0x05, 0x48, 0x48);
        it6265_reset(1);
    }
    else
    {
        pr_info("IT6265: it6265_chip_init else()\r\n");
        it6265_sys_state(SYS_Unplug);
    }
}

#if ENABLE_CALLBACK
void it6265_register_callbacks( it6265_callbacks *callback, it6265_callback_data *callback_data )
{
    g_dev.cb = *callback;
    g_dev.cb_data = *callback_data;
}
#endif

bool It6265::it6265_hdcp2_get_rx_status( unsigned short *rx_status)
{
    u8 tmp;
    u16 sts;

    tmp = hdmitxrd(0x42);
    if ( tmp & 0x10 ) {  // is HDCP2.2 enabled

        if ( g_dev.vars.HdcpState != CP_Going ) {
            it6265_hdcp_read(0x70,2);
        }

        tmp = hdmitxrd(0x4d);
        sts = tmp;
        sts <<= 8;

        tmp = hdmitxrd(0x4c);
        sts |= tmp;

        if ( rx_status ) {
            *rx_status = sts;
            return true;
        }
    }

    return false;
}

void It6265::it6265_hdcp2_continue_stream_manage(void)
{
    hdmitxwr(0xFF, 0xC3);
    hdmitxwr(0xFF, 0xA5);
    hdmitxset(0x6B, 0x80, 0x80);// RRepWaitTxMgmDone
    hdmitxwr(0xFF, 0xFF);
}

void It6265::it6265_hdcp2_continue_SEK_send_Eks(void)
{
    hdmitxwr(0xFF, 0xC3);
    hdmitxwr(0xFF, 0xA5);
    hdmitxset(0x6B, 0x40, 0x40);// RRepWaitTxEksDone
    hdmitxwr(0xFF, 0xFF);
}

int It6265::it6265_hdcp_enable( int enable_hdcp )
{
    int prev_hdcp_en;

    prev_hdcp_en = g_dev.cfg.EnHDCP;
    if ( enable_hdcp )
    {
        if ( g_dev.cfg.EnHDCP == 0 )
        {
            g_dev.cfg.EnHDCP = 1;
            it6265_hdcp_state(CP_Going);
        }
    }
    else
    {
        if ( g_dev.cfg.EnHDCP )
        {
            it6265_hdcp_state(CP_Reset);
            g_dev.cfg.EnHDCP = 0;

            hdmitxset(0xC2, 0x80, 0x80); // disable HDCP encryption
            idle(1000/24); // wait for at least 1 frame to notify the sink for encryption disabled
            hdmitxset(0x41, 0x01, 0x01); // disalbe HDCP hardware
            hdmitxset(0x05, 0x40, 0x40); // do HDCP reset
            hdmitxset(0x6F, 0x08, 0x00);
            it6265_set_avmute(0);
        }
    }

    return prev_hdcp_en;
}

void It6265::it6265_enable_480p_pattern_gen(void)
{
    g_dev.cfg.EnIntPG = 1;
    it6265_get_timing_by_vic(2); // 720x480p60
    it6265_reset(1);
    // enable external 27M clock here

    it6265_sys_state(SYS_Unplug);
}
//--------------------  it6265.c end   ---------------------------

//--------------------  it6265_cec.c start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_cec.c>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
//#define pr_info _trace
//#define pr_cec(x)                    do{    pr_info x;}while(0)
#define pr_cec(x) pr_info x

#define CEC_RX_QUEUE_SIZE    8
#define CEC_TX_QUEUE_SIZE    8

#if ENABLE_CEC
// for check Invalid CEC Cmd


static const CECcmdFormat  LookUpTable_CEC[]=
//static _CODE  LookUpTable_CEC[][3]=
 {

    {eFeatureAbort,            eDirectly,        4,    eCDCFeature, "Feature Abort"},            // [Header] + [0x00] + [Feature OpCode] + [Abort Reason]
    {eAbort,                    eDirectly,        2,    eCDCFeature, "Abort"},            // [Header] + [0xFF]

    // One Touch Play Feature
    {eActiveSource,            eBroadcast,    4,    eOTPFeature, "Active Source"},            // Header + [0x82] + [Physical Address]
    {eImageViewOn,            eDirectly,        2,    eOTPFeature, "Image View On"},            // Header + [0x04]
    {eTextViewOn,            eDirectly,        2,    eOTPFeature, "Text View On"},            // Header + [0x0D]

    // Routing Control Feature
    {eInactiveSource,            eBroadcast,    2,    eRoutingCtlFeature, "Inactive Source"},            // Header + [0x9D]
    {eRequestActiveSource,        eBroadcast,    2,    eRoutingCtlFeature, "Request Active Source"},            // Header + [0x85]
    {eRoutingChange,            eBroadcast,    6,    eRoutingCtlFeature, "Routing Change"},            // Header + [0x80] + [Org Address] + [New Address]
    {eRoutingInformation,        eBroadcast,    4,    eRoutingCtlFeature, "Routing Information"},            // Header + [0x80] + [Org Address] + [New Address]
    {eSetStreamPath,            eBroadcast,    4,    eRoutingCtlFeature, "Set Stream Path"},            // Header + [0x86] + [Physical Address]

    {eStandBy,                eBoth,        2,    eStandByFeature, "Standby"},            // Header + [0x36]

    // System Information Feature
    {eCECVersioin,            eDirectly,        3,    eSysInfoFeature, "CEC Version"},            // Header + [0x82] + [CEC Version]
    {eGetCECVersion,            eDirectly,        2,    eSysInfoFeature, "Get CEC Version"},            // Header + [0x9F]
    {eGivePhysicalAddress,        eDirectly,        2,    eSysInfoFeature, "Give Physical Address"},            // Header + [0x83]
    {eGetMenuLanguage,        eDirectly,        2,    eSysInfoFeature, "Get Menu Language"},            // Header + [0x91]
    {eReportPhysicalAddress,    eBroadcast,    5,    eSysInfoFeature, "Report Physical Address"},            // Header + [0x84] + [Physical Address]+ [Device Type]
    {eSetMenuLanguage,        eBroadcast,    3,    eSysInfoFeature, "Set Menu Language"},            // Header + [0x32] + [Language]

    // Vendor Specific Commands Feature
    {eDeviceVendorID,        eBroadcast,    5,    eVenSpecCmdFeature, "Device Vendor ID"},            // Header + [0x87] + [Vendor ID]
    {eGiveDeviceVendorID,        eDirectly,        2,    eVenSpecCmdFeature, "Give Device Vendor ID"},            // Header + [0x8C]
    {eVendorCommand,        eDirectly,        3,    eVenSpecCmdFeature, "Vendor Command"},            // Header + [0x89] + [Vendor Specific Data]
    {eVendorCommandWithID,    eBoth,        6,    eVenSpecCmdFeature, "Vendor Command With ID"},            // Header + [0xA0] + [Vendor ID]+ [Vendor Specific Data]
    {eRemoteButtonDown,        eBoth,        3,    eVenSpecCmdFeature, "Vendor Remote Button Down"},            // Header + [0x8A] + [Vendor Specific RC code]
    {eRemoteButtonUp,        eBoth,        3,    eVenSpecCmdFeature, "Vendor Remote Button Up"},            // Header + [0x8B] + [Vendor Specific RC code]

    // Remote Control PassThrough Feature
    {eUserPressed,            eDirectly,        3,    eRemoteCtlPassthroughFeature, "User Control Pressed"},            // Header + [0x44] + [UI command]
    {eUserReleased,            eDirectly,        2,    eRemoteCtlPassthroughFeature, "User Control Released"},            // Header + [0x45]

    // Power Status Feature
    {eGiveDevicePowerStatus,    eDirectly,        2,    ePowerStatusFeature, "Give Device Power Status"},            // Header + [0x8F]
    {eReportPowerStatus,        eDirectly,        3,    ePowerStatusFeature, "Report Power Status"},            // Header + [0x90] + [Power Status]

    // System Audio Control Feature
    {eGiveAudioStatus,        eDirectly,        2,    eSysAudioCtlFeature, "Give Audio Status"},            // Header + [0x71]
    {eGiveSystemAudIoModeStatus,eDirectly,    2,    eSysAudioCtlFeature, "Give System Audio Mode Status"},            // Header + [0x7D]
    {eReportAudioStatus,        eDirectly,        3,    eSysAudioCtlFeature, "Report Audio Status"},            // Header + [0x7A] + [Audio Status]
    {eReportAudioDescriptor,    eDirectly,        4,    eSysAudioCtlFeature, "Report Audio Descriptor"},            // Header + [0xA3] + [Audio Descriptor]
    {eRequestAudioDescriptor,    eDirectly,        3,    eSysAudioCtlFeature, "Request Short Audio Descriptor"},            // Header + [0xA4] + [Audio Format ID and Code]
    {eSetSystemAudioMode    ,    eBoth,        3,    eSysAudioCtlFeature, "Set System Audio Mode"},            // Header + [0x72] + [System Audio Status]
    {eAudioModeRequest,        eDirectly,        4,    eSysAudioCtlFeature, "System Audio Mode Request"},            // Header + [0x70] + [Physical Address]
    {eSystemAudIoModeStatus,    eDirectly,        3,    eSysAudioCtlFeature, "System Audio Mode Status"},            // Header + [0x7E] + [System Audio Status]

    // ACR
    {eInitiateARC,                eDirectly,        2,    eACRFeature, "Initiate ARC"},            // Header + [0xC0]
    {eReportARCInitiated,        eDirectly,        2,    eACRFeature, "Report ARC Initiated"},            // Header + [0xC1]
    {eReportARCTerminated,    eDirectly,        2,    eACRFeature, "Report ARC Terminated"},            // Header + [0xC2]
    {eRequestARCInitiation,        eDirectly,        2,    eACRFeature, "Request ARC Initiation"},            // Header + [0xC3]
    {eRequestARCTermination,    eDirectly,        2,    eACRFeature, "Request ARC Termination"},            // Header + [0xC4]
    {eTerminateARC,            eDirectly,        2,    eACRFeature, "Termiate ARC"},            // Header + [0xC5]

    // HEC
    {eCDC,                    eDirectly,        3,    eHECFeature, "CDC"},            // Header + [0xF8]

    // Deck Control Feature
    {ePlay,                    eDirectly,        3,    eDeckCtlFeature, "Play"},            // Header + [0x41] + [Play Mode]
    {eDeckControl,            eDirectly,        3,    eDeckCtlFeature, "Deck Control"},            // Header + [0x41] + [Play Mode]
    {eDeckStatus,                eDirectly,        3,    eDeckCtlFeature, "Deck Status"},            // Header + [0x41] + [Play Mode]
    {eGiveDeckStatus,            eDirectly,        3,    eDeckCtlFeature, "Give Deck Status"},            // Header + [0x41] + [Play Mode]

    // Polling Mesage
    //ePollingMessage,
//    {eCDC+1,                eBoth,        0,    eNoneFeature},            //end of     lookup table !!!
 };

#define     SizeOfLookUpTable_CEC    (sizeof(LookUpTable_CEC)/sizeof(CECcmdFormat))

struct cec_ctx {
    u8 my_logic_addr;
    u8 phy_addrL;
    u8 phy_addrH;
    u8 cec_initialized;
    u8 tx_done;
    u8 tx_resend_count;
    u8 la_index;
};

struct cec_ctx g_cec = {0};

CEC_FRAME s_cec_tx_queue[CEC_TX_QUEUE_SIZE];
CEC_FRAME s_cec_rx_queue[CEC_RX_QUEUE_SIZE];


circular_buffer s_cec_rx_cb[1] ;
circular_buffer s_cec_tx_cb[1] ;

void cbInit(circular_buffer *cb, int size, void *data_queue) {
    cb->size  = size;
    cb->start = 0;
    cb->end   = 0;
    cb->elems = (CEC_FRAME*)data_queue;
}

void cbPrint(circular_buffer *cb) {
    pr_cec(("size=0x%x, start=%d, end=%d\r\n", cb->size, cb->start, cb->end));
}

int cbIsFull(circular_buffer *cb) {
    return cb->end == (cb->start ^ cb->size); /* This inverts the most significant bit of start before comparison */ }

int cbIsEmpty(circular_buffer *cb) {
    return cb->end == cb->start; }

int cbIncr(circular_buffer *cb, int p) {
    return (p + 1)&(2*cb->size-1); /* start and end pointers incrementation is done modulo 2*size */
}

void* cbAdd(circular_buffer *cb) {
    void *ret;
    ret = &cb->elems[cb->end&(cb->size-1)];

    if (cbIsFull(cb)) /* full, overwrite moves start pointer */
        cb->start = cbIncr(cb, cb->start);
    cb->end = cbIncr(cb, cb->end);

    return ret;
}

void *cbRemove(circular_buffer *cb) {
    void *ret;
    ret = &cb->elems[cb->start&(cb->size-1)];
    cb->start = cbIncr(cb, cb->start);

    return ret;
}

#if DEBUG_SHOW_REG_RW
extern char g_is_set;
#endif

void cecbrd( u8 offset, u8 byteno, u8 *rddata )
{
    if( byteno>0 )
        it6265_i2c_read(CECAddr, offset, byteno, rddata );
}

void cecbwr( u8 offset, u8 byteno, u8 *rddata )
{
    #if DEBUG_SHOW_REG_RW
    u8 i;
    pr_info("cecbwr(0x%02x, 0x%02x, *);\r\n  rddata[]=", offset, byteno);
    for( i=0 ; i<byteno ; i++ )
    {
        pr_info("%02x ", (int) rddata[i]);
    }
    pr_info("\r\n");
    #endif

    if( byteno>0 ) {
        it6265_i2c_write(CECAddr, offset, byteno, rddata);
    }
}

static u8 s_logic_addr_list[] =
{
    DEVICE_ID_PLAYBACK1,        // 4
    DEVICE_ID_PLAYBACK2,        // 8
    DEVICE_ID_PLAYBACK3,        // 11
    DEVICE_ID_FREEUSE,          // 14
};

char Cec_SwitchLA(void)
{
    g_cec.la_index++;
    if ( g_cec.la_index < sizeof(s_logic_addr_list) ) {
        g_cec.my_logic_addr = s_logic_addr_list[g_cec.la_index];
    }
    else {
        return 0;
    }

    return 1;
}

void Cec_RxFifoReset(void)
{
    cecwr(0x52, 0x60);
    cecwr(0x52, 0xE0);
    cecwr(0x52, 0x80);
}

int Cec_RxCmdGet(void)
{
    u8    pu8Reg51[3];
    CEC_FRAME *cec;

    for (;;)
    {
        cecbrd(0x51, 0x03, pu8Reg51);
        pr_cec(("Cec_RxCmdGet [%02X, %02X, %02X]\r\n", pu8Reg51[0], pu8Reg51[1], pu8Reg51[2]));

        pu8Reg51[1] &= 0xF;

        // check fifo status reg0x51
        if( pu8Reg51[0] & 0xCC){
            pr_cec(("Cec Rx FIFO RESET\r\n"));
            Cec_RxFifoReset();

            return false;
        }
        else{

            // check cmd stage reg0x52[3:0]
            if ( pu8Reg51[1] ) {

                // get an empty cec frame from cb
                cec = (CEC_FRAME*) cbAdd(s_cec_rx_cb);
                cecbrd(0x4D, 2, (u8*)&cec->Header );
                cec->Size = 0x1F & cecrd(0x4F);
                if ( cec->Size >= 3 )
                {
                    cecbrd(0x50, cec->Size-2, cec->operand );
                }
            }
            else {
                break;
            }
        }
    }

    return true;
}

void Cec_TxFire(void)
{
    cecset(0x08, 0x88, 0x08);
    cecset(0x08, 0x88, 0x88);
}

void Cec_send(CEC_FRAME *cec)
{
    cecbwr(0x10, cec->Size, (u8*)&cec->Header);
    cecwr(0x23, cec->Size);

    Cec_TxFire();
}

void Cec_TxPolling(u8 u8LogicalAdr)
{
    CEC_FRAME *cec;

    cec = cbAdd(s_cec_tx_cb);
    if ( cec ) {
        cec->Header = u8LogicalAdr<<4 | u8LogicalAdr;
        cec->Size = 1;
    }
}

void Cec_TxFeatureAbort(u8 Follower, u8 CecRxCmd, CecAbortReson eAbortReson)
{
    CEC_FRAME *cec;

    cec = cbAdd(s_cec_tx_cb);
    if ( cec ) {
        cec->operand[0] = CecRxCmd;
        cec->operand[1] = eAbortReson;
        cec->Opcode = eFeatureAbort;
        cec->Header = Follower;
        cec->Size = 4;
    }
}

void Cec_TxReportPA(void)
{
    CEC_FRAME *cec;

    cec = cbAdd(s_cec_tx_cb);
    if ( cec ) {
        cec->Size = 5;
        cec->Header = (g_cec.my_logic_addr << 4) | DEVICE_ID_BROADCAST;
        cec->Opcode = eReportPhysicalAddress;
        cec->operand[0] = g_cec.phy_addrL;
        cec->operand[1] = g_cec.phy_addrH;
        cec->operand[2] = CEC_DEV_TYPE_VIDEO_PROCESSOR;
    }

    pr_cec(("Cec_TxReportPA\r\n"));
}


void Cec_rx_cmd_handler(void)
{
    CEC_FRAME *cec;
    int i;

    if ( cbIsEmpty(s_cec_rx_cb) )
    {
        return;
    }

    cec = cbRemove( s_cec_rx_cb );

    switch ( cec->Opcode )
    {
        case eFeatureAbort:
            pr_cec(("::eFeatureAbort\r\n"));
            break;
        case eImageViewOn:
            pr_cec(("::eImageViewOn\r\n"));
            break;
        default:
            pr_cec(("H=%02X Opcode = %02x\r\n", cec->Header, cec->Opcode));
            pr_cec(("operand= "));
            for( i=0 ; i<cec->Size-2 ; i++ )
            {
                pr_cec(("%02x ", cec->operand[i] ));
            }
            break;
    }
}

void Cec_tx_cmd_handler(void)
{
    CEC_FRAME *cec;

    if ( cbIsEmpty(s_cec_tx_cb) || (g_cec.tx_done==0) )
    {
        return;
    }

    cec = cbRemove( s_cec_tx_cb );

    pr_cec(("Cec Send: H=0x%02X, Op=0x%02X, Size=%d\r\n", cec->Header, cec->Opcode, cec->Size-2 ));

    Cec_send(cec);
    g_cec.tx_done = 0;
    g_cec.tx_resend_count = 0;
}

void Cec_Irq(void)
{
    u8    u8CecStatus;
    u8    u8Initiator, u8Follower;
    u8  tx_len;
    u8  status;

    u8CecStatus = cecrd(0x4C);
    cecwr(0x4C, u8CecStatus);

    // TxFail_Int
    if(u8CecStatus & 0x20){

        g_cec.tx_done = 1;

        tx_len = cecrd(0x23) & 0x1f;
        if ( tx_len < 2 )
        {
            // is polling message
            u8Initiator = cecrd(0x10);
            u8Follower = u8Initiator & 0x0F;
            u8Initiator >>= 4;

            if(u8Initiator == u8Follower){

                status = cecrd(0x54);

                if ( status & 0x02 ) // NACK
                {
                    // no one ack this logical address
                    cecwr(0x22, g_cec.my_logic_addr);
                    Cec_TxReportPA();
                }
                else
                {
                    // some one ack this logical address
                    // needs to change our logical address
                    if ( Cec_SwitchLA() ) {
                        Cec_TxPolling( g_cec.my_logic_addr );
                    }
                    else {
                        pr_cec(("***ERROR: No free logic address\r\n"));
                    }
                }
            }
        }
        else
        {
            // other cec protocol sent fail

            pr_cec(("***Cec_Tx Fail\r\n"));
            if(++g_cec.tx_resend_count < CEC_RESEND_MAX){

                g_cec.tx_done = 0; // avoid other tx cmd to be sent

                Cec_TxFire();
            }else{
                // give up after CEC_RESEND_MAX retry
            }
        }
    }

   // TxDone_Int
    if(u8CecStatus & 0x08){

        g_cec.tx_done = 1;
        tx_len = cecrd(0x23) & 0x1f;
        if ( tx_len < 2 )
        {
            // is polling message
            u8Initiator = cecrd(0x10);
            u8Follower = u8Initiator & 0x0F;
            u8Initiator >>= 4;

            if(u8Initiator == u8Follower){

                status = cecrd(0x54);

                if ( status & 0x01 ) // ACK
                {
                    // some one ack this logical address
                    // needs to change our logical address
                    if ( Cec_SwitchLA() ) {
                        Cec_TxPolling( g_cec.my_logic_addr );
                    }
                    else {
                        pr_cec(("***ERROR: No free logic address\r\n"));
                    }
                }
                else
                {
                    // no one ack this logical address
                    cecwr(0x22, g_cec.my_logic_addr);
                    Cec_TxReportPA();
                }
            }
        }
        else
        {
            // other cec protocol sent done
        }
    }


    if(u8CecStatus & 0xD4){

        // Rx Fail , Rx Done Int
        if(u8CecStatus & 0x14){
            Cec_RxCmdGet();
        }

        // Cmd[6], Data[7] over flow
        if(u8CecStatus & 0xC0){
            u8    u8Temp[3];
            cecbrd(0x51, 0x03, u8Temp);
            pr_cec(("***0x%02X, 0x%02X, 0x%02X\r\n", u8Temp[0], u8Temp[1], u8Temp[2]));
            pr_cec(("***Cec_Rx Cmd/Data FIFO overflow  [%02X]\r\n", u8CecStatus));
            Cec_RxFifoReset();
        }
    }

}

void it6265_cec_init(u8 phyAddrL, u8 phyAddrH, u32 ulClkCnt)
{
    pr_cec(("::Cec_Init++\r\n"));

    g_cec.my_logic_addr = s_logic_addr_list[0];
    g_cec.phy_addrH = phyAddrH;
    g_cec.phy_addrL = phyAddrL;

    cbInit(s_cec_rx_cb, CEC_RX_QUEUE_SIZE, s_cec_rx_queue);
    cbInit(s_cec_tx_cb, CEC_TX_QUEUE_SIZE, s_cec_tx_queue);

    cecwr(0x08, 0x4C);
    cecwr(0x08, 0x48);
    cecwr(0x22, 0x0F);

    pr_cec(("ulCLKCnt=0x%x\r\n", (int)(ulClkCnt&0xff)));

    cecwr(0x09,  0x40 | (CEC_RX_SELF_DIS<< 5));
    cecwr(0x0B, 0x14);
    cecwr(0x0C, ulClkCnt);
    cecset(0x08, 0x04, CEC_RST << 2);
    cecset(0x09, 0x02, 0x00);
    //cecwr(0x4C, 0xFF);

    cecwr(0x06, 0x00);
    cecset(0x08, 0x01, 0x01);

    Cec_RxFifoReset();
    //cecwr(0x0A, 0x20);                // Disable CEC HW retry
    cecwr(0x0A, 0x03);
    cecset(0x0A, 0x40, 0x40);            // Enable CEC

    g_cec.la_index = 0;
    g_cec.cec_initialized = 1;
    g_cec.tx_done = 1;

    //Cec_TxPolling( g_cec.my_logic_addr );

    pr_cec(("::Cec_Init--\r\n"));
}

void it6265_cec_tx_polling(void)
{
    Cec_TxPolling( g_cec.my_logic_addr );
}

void it6265_cec_maganer(void)
{
    if ( g_cec.cec_initialized ) {
        Cec_Irq();
        Cec_rx_cmd_handler();
        Cec_tx_cmd_handler();
    }
}

char it6265_cec_send( u8 opcode, u8 *operand, u8 operand_len, u8 follower)
{
    CEC_FRAME *cec;
    u8 i;

    cec = cbAdd(s_cec_tx_cb);
    if ( cec ) {
        cec->Size = operand_len+2;
        cec->Header = (g_cec.my_logic_addr << 4) | (follower&0x0F);
        cec->Opcode = opcode;
        for (i=0 ; i<operand_len ; i++)
        {
            cec->operand[i] = operand[i];
        }

        return 1; // success
    }
    else{
        return 0; // fail
    }
}
#endif
//--------------------  it6265_cec.c end   ---------------------------

//--------------------  it6265_edid.c start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_edid.c>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
typedef struct {
    u16 HActive;
    u16 VActive;
    u16 HTotal;
    u16 VTotal;
    u32 PCLK;
    u16 HFrontPorch;
    u16 HSyncWidth;
    u16 HBackPorch;
    u16 VFrontPorch;
    u16 VSyncWidth;
    u16 VBackPorch;
    u8 ScanMode;
    u8 VPolarity;
    u8 HPolarity;
    u8 PixRpt;
    char *format ;
    u8 VIC;
} MODEINFO ;

#define PROG 1
#define INTERLACE 0

#if ENABLE_EDID_PARSING
CONST char *strAudFs[] = {
    "32KHz",
    "44.1KHz",
    "48KHz",
    "88.2KHz",
    "96KHz",
    "176.4KHz",
    "192KHz",
    "reserve"
};

CONST char *strAudioCode[] = {
    "Refer to stream header",
    "LPCM",
    "AC3",
    "MPEG-1",
    "MP3",
    "MPEG-2",
    "AAC LC",
    "DTS",
    "ATRAC",
    "DSD",
    "E-AC-3",
    "DTS-HD",
    "MLP",
    "DST",
    "WMA Pro",
    "Audio Extension Type"
};

CONST char *strVIC[] = {
    "Unknown VIC",
    "640x480p60",
    "720x480p60 4:3",
    "720x480p60 16:9",
    "1280x720p60",
    "1920x1080i60",
    "1440x480i60 4:3",
    "1440x480i60 16:9",
    "1440x240p60 4:3",
    "1440x240p60 16:9",
    "2880x480i60 4:3",
    "2880x480i60 16:9",
    "2880x240p60 4:3",
    "2880x240p60 16:9",
    "1440x480p60 4:3",
    "1440x480p60 16:9",
    "1920x1080p60",
    "720x576p50 4:3",
    "720x576p50 16:9",
    "1280x720p50",
    "1920x1080i50",
    "1440x576i50 4:3",
    "1440x576i50 16:9",
    "1440x288p50 4:3",
    "1440x288p50 16:9",
    "2880x576i50 4:3",
    "2880x576i50 16:9",
    "2880x288p50 4:3",
    "2880x288p50 16:9",
    "1440x576p50 4:3",
    "1440x576p50 16:9",
    "1920x1080p50",
    "1920x1080p24",
    "1920x1080p25",
    "1920x1080p30",
    "2880x480p60 4:3",
    "2880x480p60 16:9",
    "2880x576p50 4:3",
    "2880x576p50 16:9",
    "1920x1080p50 (HTotal = 2304) 72MHz",
    "1920x1080i100", //40
    "1280x720p100", // 41
    "720x576p100 4:3",   //42
    "720x576p100 16:9",
    "1440x576i100 4:3",    //44
    "1440x576i100 16:9",
    "1920x1080i120", //46
    "1280x720p120",//47
    "720x480p120 4:3", //48
    "720x480p120 16:9",
    "1440x480i120 4:3",  //50
    "1440x480i120 16:9",
    "720x576p100 4:3",   //52
    "720x576p100 16:9",
    "1440x576i100 4:3",    //54
    "1440x576i100 16:9",
    "720x480p120 4:3", //56
    "720x480p120 16:9",
    "1440x480i120 4:3",  //58
    "1440x480i120 16:9",
    "1280x720p24",//60
    "1280x720p25",//61
    "1280x720p30",//62
    "1920x1080p120", //63
    "1920x1080p100", //64
    "1280x720p24 (64:27)",//65
    "1280x720p25 (64:27)",//66
    "1280x720p30 (64:27)",//67
    "1280x720p50 (64:27)",//68
    "1280x720p60 (64:27)",//69
    "1280x720p100 (64:27)",//70
    "1280x720p120 (64:27)",//71

    "1920x1080p24 (64:27)", //72
    "1920x1080p25 (64:27)", //73
    "1920x1080p30 (64:27)", //74
    "1920x1080p50 (64:27)", //75
    "1920x1080p60 (64:27)", //76
    "1920x1080p100 (64:27)", //77
    "1920x1080p120 (64:27)", //78

    "1680x720p24 (64:27)", //79
    "1680x720p25 (64:27)", //80
    "1680x720p30 (64:27)", //81
    "1680x720p50 (64:27)", //82
    "1680x720p60 (64:27)", //83
    "1680x720p100 (64:27)", //84
    "1680x720p120 (64:27)", //85
    //86-92
    "2560x1080p24",
    "2560x1080p25",
    "2560x1080p30",
    "2560x1080p50",
    "2560x1080p60",
    "2560x1080p100",
    "2560x1080p120",

    //93-97
    "3840x2160p24",
    "3840x2160p25",
    "3840x2160p30",
    "3840x2160p50",
    "3840x2160p60",

    //98-102
    "4096x2160p24",
    "4096x2160p25",
    "4096x2160p30",
    "4096x2160p50",
    "4096x2160p60",

    //103-107
    "3840x2160p24 (64:27)",
    "3840x2160p25 (64:27)",
    "3840x2160p30 (64:27)",
    "3840x2160p50 (64:27)",
    "3840x2160p60 (64:27)",

    // Future use.
    "CEA 861F Undefined VIC=108",
};

int It6265::checksumEDID(u8 EDID[])
{
    int i;
    int sum;
    sum = 0;
    for( i = 0; i < 128; i++ )
    {
        sum+=EDID[i]; sum &= 0xFF;
    }
    return sum;
}

CONST char *ES23[] =
{
    "800x600@60Hz",
    "800x600@56Hz",
    "640x480@75Hz",
    "640x480@72Hz",
    "640x480@67Hz",
    "640x480@60Hz",
    "720x400@88Hz",
    "720x400@70Hz"
};

CONST char *ES24[] =
{
    "1280x1024@75Hz",
    "1024x768@75Hz",
    "1024x768@70Hz",
    "1024x768@60Hz",
    "1024x768@87iHz",
    "832x624@75Hz",
    "800x600@75Hz",
    "800x600@72Hz"
};

CONST char *strDataBlock[] = {
    "Reserved",
    "Audio Data Block",
    "Video Data Block",
    "Vendor Specific Data Block",
    "Speaker Allocation Data Blcok",
    "VESA  DTC Data Block",
    "Reserved",
    "Use Extended Tag"
};

CONST char *str3DFormat[] = {
    "Frame Packing",
    "Field Alternative",
    "Line alternative",
    "Side-by-Side (Full)",
    "L+Depth",
    "L+depth+graphics+graphics-depth",
    "Top-and-Bottom",
    "reserved for future use",
    "Side-By-Side (Half)",
    "reserved for future use",
    "reserved for future use",
    "reserved for future use",
    "reserved for future use",
    "reserved for future use",
    "reserved for future use",
    "reserved for future use",
};

int VDBCount;
u8 ModeSupport[0x80];

enum PROJCMD {
    CMD_READ = 0 ,
    CMD_WRITE,
    CMD_AND,
    CMD_OR,
    CMD_SET,
    CMD_LOAD
};

void It6265::parseDTD(u8 *dtd)
{
    MODEINFO Mode ;
    u16 HBlank, VBlank;

    //int HTotal, VTotal, HActive, VActive, HBlank, VBlank;
    //int HFP, HSW, VFP, VSW;

    #if PRINT_EDID_INFO
    int HSize, VSize;
    #endif

    unsigned VF;
    //unsigned long PCLK;
    int interlace;

    pr_edid("::[DTD]\r\n");

    Mode.PCLK = ((unsigned long) dtd[1])<<8 | (unsigned long)dtd[0];
    Mode.PCLK *= 10000;

    Mode.HActive = dtd[4] & 0xF0; Mode.HActive <<= 4; Mode.HActive |= dtd[2];

    HBlank = dtd[4] & 0xF; HBlank <<= 8; HBlank |= dtd[3];

    Mode.HTotal = Mode.HActive + HBlank;
    Mode.HFrontPorch = dtd[8] | ((dtd[11]&0xC0)<<2);
    Mode.HSyncWidth = dtd[9] | ((dtd[11]&0x30)<<4);
    Mode.VFrontPorch = ((dtd[10]&0xF0)>>4) | ((dtd[11]&0xC)<<2);
    Mode.VSyncWidth = (dtd[10]&0xF) | ((dtd[11]&0x3)<<4);

    Mode.VActive = dtd[7] & 0xF0; Mode.VActive <<= 4; Mode.VActive |= dtd[5];
    VBlank = dtd[7] & 0xF; VBlank <<= 8; VBlank |= dtd[6];
    Mode.VTotal = Mode.VActive + VBlank;

    Mode.HBackPorch = HBlank - Mode.HFrontPorch - Mode.HSyncWidth ;
    Mode.VBackPorch = VBlank - Mode.VFrontPorch - Mode.VSyncWidth ;


    VF = Mode.PCLK;
    if( Mode.HTotal != 0 && Mode.VTotal != 0 )
    {
        VF /= Mode.HTotal; VF += Mode.VTotal / 2;
        VF /= Mode.VTotal;
    }
    else
    {
        VF = 0;
    }
    interlace = dtd[17]&(1<<7)?true:false;
    Mode.ScanMode = interlace?INTERLACE:PROG ;

    #if PRINT_EDID_INFO
    HSize = ((int)dtd[14]&0xF0)<<4 | (int)dtd[12];
    VSize = ((int)dtd[14]&0xF)<<8 | (int)dtd[13];
    #endif

//    pr_edid("PCLK = %d.%dMHz  Dimension:(%4dmm x %4dmm)\r\n",Mode.PCLK/1000000,(Mode.PCLK/10000)%100,HSize,VSize);
//    pr_edid("Mode - %d x %d @%uHz (%dx%d)\r\n", Mode.HActive, Mode.VActive, VF, Mode.HTotal, Mode.VTotal );
//    pr_edid("{%4d,%4d,%4d,%4d,%4d,%4d,%4d,%4d,%4d,%4d}",
//        Mode.HActive, Mode.VActive, Mode.HTotal, Mode.VTotal,
//        Mode.HFrontPorch, Mode.HSyncWidth, Mode.HBackPorch, Mode.VFrontPorch, Mode.VSyncWidth, Mode.VBackPorch);
}

void It6265::parseDisplayLimitDescriptor(u8 *descriptor)
{
    int maxHrate, maxVrate, minHrate, minVrate;
    #if PRINT_EDID_INFO
    int maxPCLK;
    #endif

    switch(descriptor[4] & 3)
    {
    case 3:
        maxVrate = 255;
        minVrate = 255;
        break;
    case 2:
        maxVrate = 255;
        minVrate = 0;
        break;
    default:
        maxVrate = 0;
        minVrate = 0;
        break;
    }
    switch(descriptor[4] & 0xc)
    {
    case 0xC:
        maxHrate = 255;
        minHrate = 255;
        break;
    case 0x8:
        maxHrate = 255;
        minHrate = 0;
        break;
    default:
        maxHrate = 0;
        minHrate = 0;
        break;
    }
    pr_edid("Display Range Limit:\r\n");

    minVrate += (int)descriptor[5];
    maxVrate += (int)descriptor[6];
    minHrate += (int)descriptor[7];
    maxHrate += (int)descriptor[8];

    #if PRINT_EDID_INFO
    maxPCLK = ((int)descriptor[9] )*10;
    #endif
    pr_edid("HFreq = %dKHz - %dKHz, VFreq = %dHz - %dHz, maximum PCLK = %dMHz\r\n",minHrate,maxHrate, minVrate,maxVrate,maxPCLK);

}

void It6265::parseMonitorDescriptor(u8 *descriptor)
{
    if( descriptor[0] == 0 && descriptor[1] == 0 && descriptor[2] == 0 )
    {
        switch(descriptor[3])
        {
        case 0xFD:
            parseDisplayLimitDescriptor(descriptor);
            break;
        default:
            break;
        }
    }
}

void It6265::ParseColorCharateristic(unsigned char *pCh)
{
    pCh = pCh;
    #if 0
    unsigned short Rx,Ry,Gx,Gy,Bx,By,Wx,Wy;

    Rx = (unsigned short)(pCh[2]<<2); Rx |= (unsigned short)((pCh[0]&0xC0)>>6);
    Ry = (unsigned short)(pCh[3]<<2); Ry |= (unsigned short)((pCh[0]&0x30)>>4);
    Gx = (unsigned short)(pCh[4]<<2); Gx |= (unsigned short)((pCh[0]&0xC)>>2);
    Gy = (unsigned short)(pCh[5]<<2); Gy |= (unsigned short)((pCh[0]&0x3));
    Bx = (unsigned short)(pCh[6]<<2); Bx |= (unsigned short)((pCh[1]&0xC0)>>6);
    By = (unsigned short)(pCh[7]<<2); By |= (unsigned short)((pCh[1]&0x30)>>4);
    Wx = (unsigned short)(pCh[8]<<2); Wx |= (unsigned short)((pCh[1]&0xC)>>2);
    Wy = (unsigned short)(pCh[9]<<2); Wy |= (unsigned short)((pCh[1]&0x3));
    #endif
}

void It6265::parseHDMI1VSDB(u8 VSDB[])
{
    int len, i , next_offset=0;
    bool check3D_Multi = false;
    len = VSDB[0]&0x1F;

    // hdmi 1 oui
    if (g_dev.sink.HDMIVer < 1) {
        g_dev.sink.HDMIVer = 1;
        g_dev.cfg.EnHDMI = 1;
    }
    pr_info("parseHDMI1VSDB HDMIVer = %d", g_dev.sink.HDMIVer);

    #if ENABLE_CEC
    g_dev.vars.phy_addrH = VSDB[4];
    g_dev.vars.phy_addrL = VSDB[5];
    #endif

    pr_edid("Physical address %X:%X:%X:%X\r\n",(VSDB[4]>>4)&0xF,(VSDB[4]>>0)&0xF,(VSDB[5]>>4)&0xF,(VSDB[5]>>0)&0xF);
    if( len >= 6 )
    {
        if( VSDB[6] & 0x5 ) { pr_edid("Error!! VSDB[6] reserved bit is non-zero!! %02X\r\n",VSDB[6]); }
        pr_edid("deep color support %s%s%s%s%s%s%s\r\n"
            ,(VSDB[6]&(1<<7))?"AI,": ""
            ,(VSDB[6]&(1<<6))?"DC_48bit,": ""
            ,(VSDB[6]&(1<<5))?"DC_36bit,": ""
            ,(VSDB[6]&(1<<4))?"DC_30bit,": ""
            ,(VSDB[6]&(1<<3))?"DC_Y444,": ""
            ,(VSDB[6]&(1<<1))?"Dual DVI": ""
            ,(VSDB[6]&0xF9)?"": "24 bit only");

        g_dev.sink.dc_48bit = (VSDB[6]>>6)&0x01;
        g_dev.sink.dc_36bit = (VSDB[6]>>5)&0x01;
        g_dev.sink.dc_30bit = (VSDB[6]>>4)&0x01;
        g_dev.sink.dc_y444 = (VSDB[6]>>3)&0x01;
    }
    else
    {
        return;
    }
    if( len >= 7)
    {
        pr_edid("Max TMDS Clock=%d MHz\r\n",(int)VSDB[7] * 5 );
        g_dev.sink.max_TMDS1 = VSDB[7];
    }
    else
    {
        return;
    }

    if( len >= 8 )
    {
        if( VSDB[8] & 0x20)
        {
            pr_edid("VDSB8: HDMI_Video_present!\r\n");
            if( len < 13 ){ pr_edid("HDMI Video present is fail by length is not enough.\r\n"); }
        }
        if( VSDB[8] & 0x80)
        {
            next_offset = 11;
        }
        else
        {
            next_offset = 9;
        }

        if( VSDB[8] & 0x40)
        {
            next_offset += 2;
        }


    }
    if( len >= next_offset )
    {

        if( VSDB[next_offset] & 0x80 )
        {
            pr_edid("VSDB[%d] %02X: 3D presents.\r\n",next_offset, VSDB[next_offset]);
            switch(VSDB[next_offset] & 0xE0)
            {
            case 0x80: pr_edid("no 3D_multi present.\r\n");break;
            case 0xA0:
                pr_edid("3D_multi present 01. 3D_Struct_ALL present, 3D_MASK not present.\r\n");
                check3D_Multi = 1;
                break;
            case 0xC0:
                pr_edid("3D_multi present 10. 3D_Struct_ALL present, 3D_MASK present.\r\n");
                check3D_Multi = 2;
                break;
            default: pr_edid("no 3D_multi present.\r\n");break;
            }
        }
        next_offset++;
    }

    while( len >= next_offset )
    {
        int HDMI_VIC_LEN;
        int HDMI_3D_LEN,_3D_Struct;
        #if PRINT_EDID_INFO
        int VIC_ORDER;
        #endif
        int idx3D;

        HDMI_VIC_LEN = (VSDB[next_offset] & 0xE0)>>5;
        HDMI_3D_LEN = VSDB[next_offset] & 0x1F;

        pr_edid("HDMI_VIC_LEN = %d , HDMI_3D_LEN = %d\r\n",HDMI_VIC_LEN, HDMI_3D_LEN );

        next_offset++;

        for( i = 0; i < HDMI_VIC_LEN; i++)
        {
            pr_edid("HDMI_VIC_%d = %02X ",i,VSDB[next_offset+i]);

            switch(VSDB[next_offset+i])
            {
            case 0: pr_edid("Reserved\r\n"); break;
            case 1: pr_edid("3840x2160@30Hz\r\n"); break;
            case 2: pr_edid("3840x2160@25Hz\r\n"); break;
            case 3: pr_edid("3840x2160@24Hz\r\n"); break;
            case 4: pr_edid("4096x2160@24Hz\r\n"); break;
            default: pr_edid("Invalid VIC Value\r\n");
            }
        }
        next_offset += HDMI_VIC_LEN;

        //----------------------------------------- todo: check 3d

        if( check3D_Multi > 0 )
        {
            pr_edid("3D_Struct_All_15..8 = %02X\r\n3D_Struct_All_7..0 = %02X\r\n",VSDB[next_offset],VSDB[next_offset+1]);
            for( i = 0; i < 8; i++ )
            {
                if( VSDB[next_offset+1] & (1<<i))
                {
                    pr_edid("3D format is %s\r\n",str3DFormat[i]);
                }
            }
            for( i = 0; i < 8; i++ )
            {
                if( VSDB[next_offset] & (1<<i))
                {
                    pr_edid("3D format is %s\r\n",str3DFormat[i+8]);
                }
            }
            next_offset += 2;
            if( HDMI_3D_LEN >= 2 ) HDMI_3D_LEN -= 2;
        }

        if( check3D_Multi == 2 )
        {
            pr_edid("3D_mask_15..8 = %02X\r\n3D_mask_7..0 = %02X\r\n",VSDB[next_offset],VSDB[next_offset+1]);
            for( i = 0; i < 8; i++ )
            {
                if( VSDB[next_offset+1] & (1<<i))
                {
                    pr_edid("3D Support mode is %s\r\n",strVIC[ModeSupport[i]]);
                }
            }
            for( i = 0; i < 8; i++ )
            {
                if( VSDB[next_offset] & (1<<i))
                {
                    pr_edid("3D Support mode is %s\r\n",strVIC[ModeSupport[i+8]]);
                }
            }
            next_offset += 2;
            if( HDMI_3D_LEN >= 2 ) HDMI_3D_LEN -= 2;
        }
        idx3D = 0;
        for( i = 0; i < HDMI_3D_LEN; idx3D++)
        {
            #if PRINT_EDID_INFO
            VIC_ORDER = VSDB[next_offset+i] >>4;
            #endif

            _3D_Struct = VSDB[next_offset+i]&0xF;
//            pr_edid("2D_VIC_order_%d = %15s,",idx3D,strVIC[ModeSupport[VIC_ORDER]]);
//            pr_edid("3D_Struct_%d = %s\r\n",idx3D,str3DFormat[_3D_Struct]);

            if( _3D_Struct > 7 )
            {
                if((VSDB[next_offset+i+1]&0xF0) == 0x10 )
                {
                    pr_edid("3D_Detail_%d = %d\r\n",idx3D,VSDB[next_offset+i+1]>>4);
                }
                i+=2;
            }
            else
            {
                i++;
            }
        }
        next_offset+=HDMI_3D_LEN;
    }

    //----------------------------------------- todo: check 3d

    for( i = next_offset; i<=len; i++ )
    {
        if( VSDB[i] != 0 )
        {
            pr_edid("Fail! reserved VSDB[%d] %02X is not zero.\r\n",i,VSDB[i]);
        }
    }

}
void It6265::parseHDMI2VSDB(u8 VSDB[])
{
    int len, i;
    len = VSDB[0]&0x1F;

    // hdmi 2.0 oui
    if (g_dev.sink.HDMIVer < 2) {
        g_dev.sink.HDMIVer = 2;
        g_dev.cfg.EnHDMI = 1;
    }
    pr_info("parseHDMI2VSDB HDMIVer = %d", g_dev.sink.HDMIVer);

    pr_edid("Version = %d\r\n",VSDB[4]);
    if( len < 7 ) { pr_edid("Invalid Len %d\r\n", len); }

    g_dev.sink.max_TMDS2 = VSDB[5];

    if( VSDB[5] == 0 )
    {
        pr_edid("Max TMDS Character Rate is lower than 340MHz.\r\n");
    }
    else
    {
        pr_edid("Max TMDS Character Rate = %dMHz.\r\n", (int)VSDB[5] * 5);
    }
    if( VSDB[6] & 0x80){

        pr_edid("Support SCDC functionality.\r\n");
        g_dev.sink.scdc_present = 1;

        if(VSDB[6] & 0x40){
            pr_edid("Capable of initiating an SCDC Read Request.\r\n");
            //g_dev.sink.rr_capable = 1;
        }
        if( VSDB[6] & 0x08){
            pr_edid("Support scrambling for TMDS character Rate or below 340Mcsc.\r\n");
            g_dev.sink.lte_340M_csc_scramble = 1;
        }
    }
    else
    {
        if(VSDB[6] & 0x40) {pr_edid(">Invalid VSDB[6] bit 6 as 1 while bit 7 = 0\r\n");}
        if(VSDB[6] & 0x20) {pr_edid(">Invalid VSDB[6] bit 5 as 1 while bit 7 = 0\r\n");}
    }


    if( VSDB[6] & 0x04){
        pr_edid("Supports Receiving 3D independent view signaling in the HF-VSIF.\r\n");
        g_dev.sink.independent_view = 1;
    }

    if( VSDB[6] & 0x02){
        pr_edid("Supports Receiving 3D Dual view signaling in the HF-VSIF.\r\n");
        g_dev.sink.daul_view2 = 1;
    }

    if( VSDB[6] & 0x01){
        pr_edid("Supports Receiving 3D OSD_Disparity indication in the HF-VSIF.\r\n");
        g_dev.sink.osd_3d_disparity = 1;

    }

    if( VSDB[7] & 0x04){
        pr_edid("Support 48 bit Deep Color 4:2:0 Pixel Encoding.\r\n");
        g_dev.sink.dc_48bit_420 = 1;
    }

    if( VSDB[7] & 0x02){
        pr_edid("Support 36 bit Deep Color 4:2:0 Pixel Encoding.\r\n");
        g_dev.sink.dc_36bit_420 = 1;
    }


    if( VSDB[7] & 0x01){
        pr_edid("Support 30 bit Deep Color 4:2:0 Pixel Encoding.\r\n");
        g_dev.sink.dc_30bit_420 = 1;
    }

    if( VSDB[7] & 0xF8 ){pr_edid("Invalid bit %02X is not zero\r\n",VSDB[7]&0xF8 );}

    for( i = 8; i<=len; i++ )
    {
        if( VSDB[i] != 0 ) { pr_edid("Fail! reserved VSDB[%d] %02X is not zero.\r\n",i,VSDB[i]); }
    }

}
void It6265::parseVSDB(u8 VSDB[])
{
    long IEEEOUI;

    IEEEOUI = VSDB[3] << 16;
    IEEEOUI |= VSDB[2] << 8;
    IEEEOUI |= VSDB[1];

    if( IEEEOUI == 0x0c03 )
    {
        parseHDMI1VSDB(VSDB);
    }

    if( IEEEOUI == 0xC45DD8 )
    {
        parseHDMI2VSDB(VSDB);
    }
}

void It6265::parseVideoCapabilityBlock(u8 Descriptor[])
{
    //extended tag 0x00
    if( Descriptor[1] != 0x00 ) return ;

    switch(Descriptor[2] & 0x3 )// S_CE
    {
    //case 0: pr_edid("CE video format NOT supported over-and-underscan..\r\n"); break ;
    //case 1: pr_edid("CE Always Overscanned.\r\n"); break ;
    //case 2: pr_edid("CE Always Underscanned.\r\n"); break ;
    //case 3: pr_edid("CE supports both over-and-underscan.\r\n");break ;
    }
    switch(Descriptor[2] & 0xC )// S_IT
    {
    //case 0: pr_edid("IT video format NOT supported over-and-underscan..\r\n"); break ;
    //case 4: pr_edid("IT Always Overscanned.\r\n"); break ;
    //case 8: pr_edid("IT Always Underscanned.\r\n"); break ;
    //case 0xc: pr_edid("IT supports both over-and-underscan.\r\n");break ;
    }
    switch(Descriptor[2] & 0x30 )// S_CE
    {
    //case 0x00: break ;
    //case 0x10: pr_edid("Perfer timing Always Overscanned.\r\n"); break ;
    //case 0x20: pr_edid("Perfer timing Always Underscanned.\r\n"); break ;
    //case 0x30: pr_edid("Perfer timing supports both over-and-underscan.\r\n");break ;
    }

    if ( (Descriptor[2] & 0x40) ) {
        pr_edid("Quantization Range for RGB is Selectable\r\n");
    }
    if ( (Descriptor[2] & 0x80) ) {
        pr_edid("Quantization Range for YCbCr is Selectable\r\n");
    }

    return ;
}

void It6265::parseVendorSpecificVideoDataBlock(u8 Descriptor[])
{
    //extended tag 0x01
    if( Descriptor[1] != 0x01 ) return ;
    return ;
}

void It6265::parseVesaDisplayDeviceDataBlock(u8 Descriptor[])
{
    //extended tag 0x02
    if( Descriptor[1] != 0x02 ) return ;
    return ;
}

void It6265::parseVesaTimingDataBlock(u8 Descriptor[])
{
    //extended tag 0x03
    if( Descriptor[1] != 0x03 ) return ;
    return ;
}

char *strExtTag05_ColormetryDataBlock[] ={
    "xvYCC601",
    "xvYCC709",
    "sYCC601",
    "Adobe_YCC601",
    "Adobe_RGB",
    "BT2020_RGB",
    "BT2020_YCC",
    "BT2020_C_YCC",
};

//char *strExtTag05_ColormetryDataBlock[] ={
//    "xvYCC601 Standard Definition Colorimetry based on IEC 61966-2-4",
//    "xvYCC709 High Definition Colorimetry based on IEC 61966-2-4",
//    "sYCC601 Colorimetry based on IEC 61966-2-1/Amendment 1",
//    "Adobe_YCC601 Colorimetry based on IEC61966-2-5, Annex A",
//    "Adobe_RGB  Colorimetry based on IEC61966-2-5",
//    "BT2020_RGB Colorimetry based on ITU-R BT.2020 RGB",
//    "BT2020_YCC Colorimetry based on ITU-R BT.2020 YCbCr",
//    "BT2020_C_YCC Colorimetry based on ITU-R BT.2020 YcCbcCrc",
//};
//
//char *strExtTag05_Byte4[] ={
//    "MD0: Future metadata profile",
//    "MD1: Future metadata profile",
//    "MD2: Future metadata profile",
//    "MD3: Future metadata profile",
//
//    "DB[4][4] Invalid Bit!!\r\n",
//    "DB[4][5] Invalid Bit!!\r\n",
//    "DB[4][7] Invalid Bit!!\r\n",
//    "DB[4][7] Invalid Bit!!\r\n",
//};

void It6265::parseColorDataBlock(u8 Descriptor[])
{
    int i ;

    //extended tag 0x05
    if( Descriptor[1] != 0x05 ) return ;

    for( i = 0 ; i < 8 ; i++ )
    {
        if( Descriptor[2] & (1<<i)) { pr_edid("Support %s\r\n",strExtTag05_ColormetryDataBlock[i]); }
    }
    //for( i = 0 ; i < 8 ; i++ )
    //{
    //    if( Descriptor[3] & (1<<i)) pr_edid("Support %s\r\n",strExtTag05_Byte4[i]);
    //}
    return ;
}

void It6265::parseVideoFormatPreferenceDataBlock(u8 Descriptor[])
{
    int i ,len ;

    //extended tag 0x0D
    if( Descriptor[1] != 0x0D ) return ;

    len = (Descriptor[0] & 0x1f) -1 ;

    pr_edid("prefer mode :\r\n");
    for( i = 0 ;i < len ; i++ )
    {
        if(( Descriptor[2+i] >= 1 )&&( Descriptor[2+i] <= 127 ))
        {
            pr_edid("%d-th = %d\r\n",(i+1), Descriptor[2+i]);
        }
        else if(( Descriptor[2+i] >= 129 )&&( Descriptor[2+i] <= 144 ))
        {
            pr_edid("%d-th = EDID[%d]\r\n",(i+1), Descriptor[2+i]-129);
        }
    }

    return ;
}

void It6265::parseYCC420VideoDataBlock(u8 Descriptor[])
{
    int i , len ;

    //extended tag 0x0E
    if( Descriptor[1] != 0x0E ) return ;

    len = (Descriptor[0] & 0x1f) -1 ;

    pr_edid("YCbCr4:2:0 only VIC:\r\n");
    for( i = 0 ; i< len ; i++ )
    {
        pr_edid("%d\r\n", Descriptor[2+i]);
    }

    return ;
}

// todo: support mode
void It6265::parseYCC420MapDataBlock(u8 Descriptor[])
{
    int i , j , k, len ;
    //extended tag 0x0F
    if( Descriptor[1] != 0x0F ) return ;
    len = (Descriptor[0] & 0x1f)-1 ;

    // ModeSupport[VDBCount] = EDID[i+j]&0x7F; VDBCount++;

    if( (len * 8) < VDBCount )
    {
        len *= 8 ;
    }
    else
    {
        len = VDBCount ;
    }

    for(i = 0,k=0 ; i < len ; i+=8 )
    {
        for( j = 0 ;j < 8 ; j++, k++)
        {
            if(Descriptor[2+(i/8)] & (1<<j))
            {
                pr_edid("%s support YCbCr4:2:0\r\n",strVIC[ModeSupport[k]]);
            }

        }
    }

    return ;
}

//void It6265::parseVendorSpecificAudioDataBlock(uint8 Descriptor[])
//{
//    //extended tag 0x11
//    if( Descriptor[1] != 0x11 ) return ;
//    return ;
//}

//void It6265::parseHDMI2_3DAudioDataBlock(uint8 Descriptor[])
//{
//    //extended tag 0x12
//    if( Descriptor[1] != 0x12 ) return ;
//    return ;
//}

//void It6265::parseInfoframeDataBlock(uint8 Descriptor[])
//{
//    //extended tag 0x20
//    if( Descriptor[1] != 0x20 ) return ;
//    return ;
//}

void It6265::parseCEAExtensionDescriptor(u8 Descriptor[])
{
    int len,i;
    u8 tag;

    tag = Descriptor[0] & 0xE0;
    if( tag != 0xE0 ) return ;

    len = Descriptor[0] & 0x1F;
    tag = Descriptor[1];

    pr_edid("Extension Tag = %02X, len = %d\r\n",tag,len);
    for( i = 0; i <= len; i++)
    {
        pr_edid("%02X ",Descriptor[i]);
    }
    pr_edid("\r\n");

    switch(tag)
    {
    case 0x00: parseVideoCapabilityBlock(Descriptor); break;
    //case 0x01: parseVendorSpecificVideoDataBlock(Descriptor); break;
    //case 0x02: parseVesaDisplayDeviceDataBlock(Descriptor); break;
    //case 0x03: parseVesaTimingDataBlock(Descriptor); break;
    case 0x05: parseColorDataBlock(Descriptor); break;
    case 0x0D: parseVideoFormatPreferenceDataBlock(Descriptor); break;
    case 0x0E: parseYCC420VideoDataBlock(Descriptor); break;
    case 0x0F: parseYCC420MapDataBlock(Descriptor); break;
    //case 0x11: parseVendorSpecificAudioDataBlock(Descriptor); break;
    //case 0x12: parseHDMI2_3DAudioDataBlock(Descriptor); break;
    //case 0x20: parseInfoframeDataBlock(Descriptor); break;
   }
}

bool It6265::it6265_parse_CEA_Block( u8 *EDID )
{
    int i,j,sum;
    int DTDoffset ,offset;
    int len,tag;

    pr_edid("[EDID Extension Block]\r\n");

    sum =  checksumEDID(EDID);

    if( sum != 0 )
    {
        pr_edid("\r\nCorrect Checksum should be %02X\r\n",(EDID[127] - sum)&0xFF);
    }

    if( EDID[0] != 0x02 && EDID[1] != 0x03 )
    {
        pr_edid("Not a CEA861B extension. stop parsing.\r\n"); return true;
    }

    DTDoffset = EDID[2];

    if(EDID[3] & (1<<7) ) {pr_edid("under scan\r\n");}
    if(EDID[3] & (1<<6) ) {pr_edid("Audio Support\r\n");}
    if(EDID[3] & (1<<5) ) {pr_edid("YCbCr 4:4:4 support\r\n");}
    if(EDID[3] & (1<<4) ) {pr_edid("YCbCr 4:2:2 support\r\n");}
    if(EDID[3] & 0xF ) {pr_edid("Support %d native format\r\n",EDID[3] & 0xF);}

    VDBCount = 0;

    for( offset = 4; offset < DTDoffset; )
    {
        len = EDID[offset] & 0x1F;
        tag = EDID[offset] & 0xE0;
        tag >>= 5;

        switch(tag)
        {
        case 1://audio data block.
            pr_edid("Audio data block\r\n");
            i = offset + 1;
            for( j = 0; j < len; j+= 3 )
            {
                int k,ac;
                ac = (EDID[i+j]&0x78)>>3;
                pr_edid("Audio code = %X(%s)",ac,strAudioCode[ac]);
                pr_edid(", %d channel%s: ",(EDID[i+j]&7)+1,((EDID[i+j]&7)>0)?"s": "");
                for( k = 0; k < 7; k++ )
                {
                    if( EDID[i+j+1] & (1<<k)){ pr_edid(" %s",strAudFs[k]);}
                }
                pr_edid("\r\n");
                if( ac >=2 && ac <= 8 ) { pr_edid("Maximum bit rate = %d KHz\r\n",EDID[i+j+2]);}

            }
            break;

        case 2://video data block.
            i = offset + 1;
//            pr_edid("Video Mode:\r\n");
            for( j = 0; j < len; j++ )
            {
//                pr_edid(" VIC %2d ",EDID[i+j]&0x7F);

                //if(EDID[i+j]&0x80) pr_edid("(native)");
                //pr_edid("%s\r\n",strVIC[EDID[i+j]&0x7F]);

                ModeSupport[VDBCount] = EDID[i+j]&0x7F; VDBCount++;
            }
            pr_edid("\r\n");
            break;
        case 3://vendor data block.
            parseVSDB(EDID+offset);
            break;
        case 4://speaker allocation
            i = offset + 1;
            break;
        case 5://VESA DTC data block
            i = offset + 1;
            break;
        case 7://Use Extension tag
            i = offset + 1;
            parseCEAExtensionDescriptor(EDID+offset);
            break;
        }
        offset += len + 1;
    }

    for( i = DTDoffset; i < 128; i+=18)
    {
        if( EDID[i] == 0 && EDID[i+1] == 0 )
        {
        }
        else if( i < 127 )
        {
            parseDTD(EDID+i);
        }
    }
    return true;
}

bool It6265::it6265_parse_edid_block0( u8 *EDID )
{
    int i,sum;

    sum =  checksumEDID(EDID);
    if( sum != 0 ) {
        pr_edid("Correct Checksum should be %02X\r\n",(EDID[127]-sum)&0xFF);
    }

    if( EDID[0] != 0x00 ||EDID[7] != 0x00 ||
        EDID[1] != 0xFF ||EDID[2] != 0xFF ||EDID[3] != 0xFF ||
        EDID[4] != 0xFF ||EDID[5] != 0xFF ||EDID[6] != 0xFF )
    {
        pr_edid("Header is not 00 FF FF FF  FF FF FF 00\r\n");
    }

    pr_edid("EDID v%d.%d\r\n",EDID[0x12],EDID[0x13]);


    if( EDID[0x14] & 0x80 ) // is Digital
    {
        // digital
        pr_edid("Monitor Input is Digital.\r\n");
        if( EDID[0x13] == 0x04 )
        {
            switch(EDID[0x14]&0x70)
            {
            case 0x00: {pr_edid("Color Bit Depth is undefined\r\n");} break;
            case 0x10: {pr_edid("6 Bits per Primary Color\r\n");} break;
            case 0x20: {pr_edid("8 Bits per Primary Color\r\n");} break;
            case 0x30: {pr_edid("10 Bits per Primary Color\r\n");} break;
            case 0x40: {pr_edid("12 Bits per Primary Color\r\n");} break;
            case 0x50: {pr_edid("14 Bits per Primary Color\r\n");} break;
            case 0x60: {pr_edid("16 Bits per Primary Color\r\n");} break;
            case 0x70: {pr_edid("Reserved (Do Not Use)\r\n");} break;
            }
            switch(EDID[0x14]&0xF)
            {
            case 0x0: {pr_edid("Digital Interface is not defined (see Note 4)\r\n");} break;
            case 0x1: {pr_edid("DVI is supported\r\n");} break;
            case 0x2: {pr_edid("HDMI-a is supported\r\n");} break;
            case 0x3: {pr_edid("HDMI-b is supported\r\n");} break;
            case 0x4: {pr_edid("MDDI is supported\r\n");} break;
            case 0x5: {pr_edid("DisplayPort is supported\r\n");} break;
            default: {pr_edid("All remaining values for Bits 3 to 0 are Reserved: Do Not Use\r\n");} break;
            }
        }
    }
    else
    {
        // analog
        pr_edid("Monitor Input is Analog.\r\n");
    }

    if( EDID[0x15] != 0 && EDID[0x16] != 0 ) // Hcm, Vcm
    {
        pr_edid("Screen Size: %ucm x %ucm.\r\n",(unsigned int)EDID[0x15],(unsigned int)EDID[0x16]);
    }
    else
    {
        if( EDID[0x13] == 4 )
        {
            if( EDID[0x15] != 0 )
            {
                pr_edid("Screen Aspect Ration is %u:100\r\n",(unsigned int)EDID[0x15]+99);
            }
            else if( EDID[0x16] != 0 )
            {
                pr_edid("Screen Aspect Ration is 100:%u\r\n",(unsigned int)EDID[0x16]+99);
            }
            else
            {
                // both zero.
                pr_edid("The screen size or aspect ratio are unknown or undefined.");
            }
        }
        else
        {
                pr_edid("The system shall make no assumptions regarding the display size..");
        }
    }

    if( EDID[0x17] == 0xFF )
    {
        pr_edid("Unknow Gamma.\r\n");
    }
    else
    {
        pr_edid("Gamma is 1+(%u/100)\r\n", (unsigned int)EDID[0x17] );
    }

    if( EDID[0x13] == 4 )
    {
        pr_edid("EDID v1.4, Feature support:\r\n");
        if ( (EDID[0x18]&0x80) ) {
            pr_edid("Standby\r\n");
        }
        if ( (EDID[0x18]&0x40) ) {
            pr_edid("Suspend\r\n");
        }
        if ( (EDID[0x18]&0x20) ) {
            pr_edid("Active Off/VeryLow Power\r\n");
        }

        if(EDID[0x14]&0x80)
        {
            switch(EDID[0x18]&0x18)
            {
            case 0x00: {pr_edid("Color Encoding: RGB 4:4:4\r\n");} break;
            case 0x08: {pr_edid("Color Encoding: RGB 4:4:4 & YCrCb 4:4:4\r\n");} break;
            case 0x10: {pr_edid("Color Encoding: RGB 4:4:4 & YCrCb 4:2:2\r\n");} break;
            case 0x18: {pr_edid("Color Encoding: RGB 4:4:4 & YCrCb 4:4:4 & YCrCb 4:2:2\r\n");} break;
            }
        }
        else
        {
            switch(EDID[0x18]&0x18)
            {
            case 0x00: {pr_edid("Monochrome Display.\r\n");} break;
            case 0x08: {pr_edid("RGB Display.\r\n");} break;
            case 0x10: {pr_edid("Non-RGB Multicolor Display.\r\n");} break;
            case 0x18: {pr_edid("Undefined Display.\r\n");} break;
            }
        }

        if( EDID[0x18] & 0x4 ) { pr_edid("Standard Default Color Space, sRGB\r\n"); }

        pr_edid("%sSupport Prefer timing.\r\n",( EDID[0x18] & 0x2 ) ?"":"Not ");
        pr_edid("Display is %scontinuous frequency.\r\n",(EDID[0x18]&1)?"":"non-");

    }
    else if( EDID[0x13] == 3 )
    {
        pr_edid("EDID v1.3, Feature support\r\n");
        if ( (EDID[0x18]&0x80) ) {
            pr_edid("Standby\r\n");
        }
        if ( (EDID[0x18]&0x40) ) {
            pr_edid("Suspend\r\n");
        }
        if ( (EDID[0x18]&0x20) ) {
            pr_edid("Active Off/VeryLow Power\r\n");
        }

        switch(EDID[0x18]&0x18)
        {
        case 0x00: {pr_edid("Monochrome Display.\r\n");} break;
        case 0x08: {pr_edid("RGB Display.\r\n");} break;
        case 0x10: {pr_edid("Non-RGB Multicolor Display.\r\n");} break;
        case 0x18: {pr_edid("Undefined Display.\r\n");} break;
        }

        if( EDID[0x18] & 0x4 ) {pr_edid("Standard Default Color Space, sRGB\r\n");}
        if( EDID[0x18] & 0x2 ) {pr_edid("Support Prefer timing.\r\n");}
        if( EDID[0x18] & 0x1 ) {pr_edid("Default GTF Supported.\r\n");}

    }

    ParseColorCharateristic(EDID+0x19);

    for( i = 7; i >= 0; i-- )
    {
        if( EDID[0x23] & (1<<i) ) { pr_edid("established %s\r\n",ES23[i]); }
    }
    for( i = 7; i >= 0; i-- )
    {
        if( EDID[0x24] & (1<<i) ) { pr_edid("established %s\r\n",ES24[i]); }
    }
    if( EDID[0x25] & (1<<7) ) { pr_edid("established 1152x864@75Hz\r\n"); }

    for( i = 0; i < 8; i++ )
    {
        int width, height;
        #if PRINT_EDID_INFO
        int vf;
        #endif
        pr_edid("Standard timing %d: %02X %02X",i,EDID[0x26+i*2],EDID[0x26+i*2+1]);
        if( EDID[0x26+i*2+1] == 0x01 )
        {
            pr_edid(" - not determined\r\n");
        }
        else
        {
            width = EDID[0x26+i*2];
            width += 31; width *= 8;
            switch(EDID[0x26+i*2+1] & 0xc0)
            {
            case 0x00:
                height = width / 16; height *=10;
                break;
            case 0x40:
                height = width / 4; height *=3;
                break;
            case 0x80:
                height = width / 5; height *=4;
                break;
            case 0xC0:
                height = width / 16; height *=9;
                break;
            default:
                height = 0;
                break;
            }
            //vf = (EDID[0x26+i*2+1]&0x3F) + 60;
            pr_edid(" - %dx%d@%dHz\r\n",width,height,vf);
        }
    }

    for( i = 0x36; i < (0x36+18*4); i+=18 )
    {
        if( EDID[i] == 0 && EDID[i+1] == 0 ) {
            parseMonitorDescriptor(EDID+i);
        }
        else {
            parseDTD(EDID+i);
        }
    }

    return true;
}

#else

void It6265::it6265_print_buffer( void *buffer, int length );

bool It6265::it6265_parse_edid_block0( u8 *EDID )
{
    it6265_print_buffer( EDID, 128 );
    return true;
}

bool It6265::it6265_parse_CEA_Block( u8 *EDID )
{
    u8 *tmp;
    u8 offset;
    u8 offset_max;
    u8 length;

    it6265_print_buffer( EDID, 128 );

    offset = 0;
    tmp = &EDID[offset];

    if (tmp[0] == 0x02 && tmp[1] == 0x03) {
        int cea_type;

        if (tmp[3] & 0x80) {
            pr_edid("underscan\r\n");
        }
        if (tmp[3] & 0x40) {
            pr_edid("Audio support\r\n");
        }
        if (tmp[3] & 0x20) {
            pr_edid("YCbCr 444\r\n");
        }
        if (tmp[3] & 0x10) {
            pr_edid("YCbCr 422\r\n");
        }

        pr_edid("%d native DTD\r\n", (int)(tmp[3] & 0xF));

        // start to parse cea tag
        offset = 4;
        length = (tmp[4] & 0x1F) + 2;
        offset_max = tmp[2];

        for (;;) {

            tmp = &EDID[offset];

            cea_type = (tmp[0] & 0xE0) >> 5;

            pr_edid("CEA Type=0x%02X, len=%d\r\n", (int)cea_type, (int)(length - 1));

            switch (cea_type) {
            case 0x02:
                {
                    int i;
                    for( i=1 ; i<length-1 ; i++ )
                    {
                        pr_edid("SVD[%d]=%d\r\n", (int)i, (int)tmp[i]);
                    }
                }
                break;
            case 0x03:
                if (tmp[1] == 0xD8 && tmp[2] == 0x5D && tmp[3] == 0xC4) {
                    // hdmi 2.0 oui
                    if (g_dev.sink.HDMIVer < 2) {
                        g_dev.sink.HDMIVer = 2;
                        g_dev.cfg.EnHDMI = 1;
                    }

                    g_dev.sink.max_TMDS_clk = tmp[5];

                    if (tmp[6] & 0x80) {
                        g_dev.sink.scdc_present = 1;
                    }
                    if (tmp[6] & 0x40) {
                        //g_dev.sink.rr_capable = 1;
                    }
                    if (tmp[6] & 0x08) {
                        g_dev.sink.lte_340M_csc_scramble = 1;
                    }
                    if (tmp[6] & 0x04) {
                        g_dev.sink.independent_view = 1;
                    }
                    if (tmp[6] & 0x02) {
                        g_dev.sink.daul_view2 = 1;
                    }
                    if (tmp[6] & 0x01) {
                        g_dev.sink.osd_3d_disparity = 1;
                    }
                    if (tmp[7] & 0x04) {
                        g_dev.sink.dc_48bit_420 = 1;
                    }
                    if (tmp[7] & 0x02) {
                        g_dev.sink.dc_36bit_420 = 1;
                    }
                    if (tmp[7] & 0x01) {
                        g_dev.sink.dc_30bit_420 = 1;
                    }

                    pr_edid("HDMI 2.0 HF_VSDB\r\n");

                } else if (tmp[1] == 0x03 && tmp[2] == 0x0C && tmp[3] == 0x00) {
                    // hdmi 1 oui
                    if (g_dev.sink.HDMIVer < 1) {
                        g_dev.sink.HDMIVer = 1;
                        g_dev.cfg.EnHDMI = 1;
                    }
                    g_dev.sink.max_TMDS_clk = tmp[7];

                    if (tmp[6] & 0x40) {
                        g_dev.sink.dc_48bit = 1;
                    }
                    if (tmp[6] & 0x20) {
                        g_dev.sink.dc_36bit = 1;
                    }
                    if (tmp[6] & 0x10) {
                        g_dev.sink.dc_30bit = 1;
                    }
                    if (tmp[6] & 0x08) {
                        g_dev.sink.dc_y444 = 1;
                    }

                    pr_edid("HDMI 1.0 VSDB\r\n");

                }
                break;

            case 0x07:
                switch (tmp[1]) {
                case 0x0E: // 420 VDB
                    {
                        //int ilen = (tmp[0]&0x1F)-1;

                        g_dev.sink.yuv420_svd_or_cmdb = 1;
                    }
                    break;
                case 0x0F: // 420 CMDB
                    {
                        //int ilen = tmp[0]&0x1F;

                        g_dev.sink.yuv420_svd_or_cmdb = 1;
                    }
                    break;
                case 0x05:
                    g_dev.sink.ExtColCap = tmp[2];   // for get Colorimetry infomation
                    break;
                }
                break;

            default:
                break;
            } // switch( cea_type )

            offset += (length - 1);
            length = (tmp[length - 1] & 0x1F) + 2;

            if (offset >= offset_max) {
                break;
            }
        } // for(;;)
    } // if ( tmp[0] == 0x02 && tmp[1] == 0x03 )

    return true;
}

#endif
//-------------------- it6265_edid.c end   ---------------------------

//--------------------  it6265_io.c start   ---------------------------
///*****************************************
//  Copyright (C) 2009-2022
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <it6265_io.c>
//   @author Kenneth.Hung@ite.com.tw
//   @date   2022/08/29
//   @fileversion: IT6265_EVB_1.36
//******************************************/
// include --- headers for I/O
unsigned char g_i2c_dev = 1;
// include --- headers for I/O

#define _SHOW_IO 0

void It6265::idle(u16 ms)
{
    usleep(ms * 1000);
}

XBH_U8 It6265::it6265_i2c_write(u8 addr, u8 offset, u8 length, u8 *buffer)
{
    XBH_U8 ret = XBH_FAILURE;
    int i;
/*
    for(i = 0;i < 10;i++){
//    ret = i2c_block_dma_write(g_i2c_dev, addr, offset, buffer, length );
        ret = XbhService::getModuleInterface()->setI2cData(Device_I2CBus, addr, offset, 0x01, length, (const XBH_U8 *)buffer) == 0 ? TRUE : FALSE;

        usleep(5);
        if(ret){
            return TRUE;
        }
        usleep(5);
        //pr_err("i2c write %02x %02x %d, ret=%d\r\r\n", (int)addr, (int)offset, (int)length, ret);
        XLOGE("it6265_i2c_write count:%d\r\n", i);
    }

    return FALSE;
*/


    ret = XbhService::getModuleInterface()->setI2cData(Device_I2CBus, addr, offset, 0x01, length, (const XBH_U8 *)buffer) == 0 ? TRUE : FALSE;

    if ( 0 == ret )
    {
        pr_err("ERROR: it6265_i2c_write %02x %02x\r\n", addr, offset);
        return false;
    }
    else
    {
//         #if _SHOW_IO
//         int i;
// //        pr_info( "i2c_write success %02X %02X ", addr, offset, length );
//         for( i=0 ; i<length ; i++ )
//         {
//             pr_info( "it6265_i2c_write %02X %02X %d buffer %02X ", addr, offset, length, buffer[i]);
//         }
//         pr_info( "\r\n");
//         #endif
    }
    return true;

}

XBH_U8 It6265::it6265_i2c_read(u8 addr, u8 offset, u8 length, u8 *buffer)
{
    XBH_U8 ret = XBH_FAILURE;
    XBH_U8 readBuff[128] = {0};
    int i, j;
/*
    for(i = 0;i < 3;i++){
        usleep(500 * 1000);

    //  ret = i2c_block_dma_read(g_i2c_dev, addr, &offset, 1, buffer, length);
        ret = XbhService::getModuleInterface()->getI2cData(Device_I2CBus, addr, offset, 0x01, length, readBuff) == 0 ? TRUE : FALSE;
        usleep(5);
        if(ret){
            for (j = 0; j < length; j++){
                buffer[i] = readBuff[i];
                XLOGD("buffer is %02X", buffer[i]);
            }
            return TRUE;
        }
        XLOGE("it6265_i2c_read count:%d\r\n", i);
    }
    //pr_err("i2c read %02x %02x %d, ret=%d\r\r\n", (int)addr, (int)offset, (int)length, ret);
    XLOGE("ERROR:it6265_i2c_read: addr=0x%x,offset=0x%x,length=0x%x,buffer[0]=0x%x\n", addr, offset, length, buffer[0]);
    return FALSE;
*/


    ret = XbhService::getModuleInterface()->getI2cData(Device_I2CBus, addr, offset, 0x01, length, readBuff) == 0 ? TRUE : FALSE;
    if(ret){
        for (i = 0; i < length; i++){
            buffer[i] = readBuff[i];
        }
//      return TRUE;
    }

//    ret = XbhService::getModuleInterface()->getI2cData(Device_I2CBus, addr, offset, 0x01, length, buffer) == 0 ? TRUE : FALSE;
/*
    if(ret){
        for (j = 0; j < length; j++){
            buffer[i] = readBuff[i];
        //    XLOGD("buffer is %02X", buffer[i]);
        }
    }
*/
    if ( 0 == ret  )
    {
        pr_err("ERROR: it6265_i2c_read %02x %02x\r\n", addr, offset);
        return false;
    }
    else
    {
//         #if _SHOW_IO
//         int i;
// //        pr_info( "i2c_read %02X %02X %d = ", addr, offset, length );

//         for( i=0 ; i<length ; i++ )
//         {
//             pr_info( "it6265_i2c_read %02X %02X %d buffer %02X ", addr, offset, length, buffer[i]);
//         }
//         pr_info( "\r\n");
//         #endif
    }
    return true;

}

//
// call back function examples
//
//-------------------- it6265_io.c end   ---------------------------

XBH_BOOL It6265::checkIte6265Timing(void)
{
    XBH_U32 hdestartreg = 0x0000;   //0x1337_03
    XBH_U32 htotalreg = 0x0000;     //0x1337_05
    XBH_CHAR check_hdmiout_res[10] = {0};

    XBH_U32 mResolution = getHdmiOutResolution();
    switch(mResolution) {
        case 1:
            // XbhService::getPlatformInterface()->GetRegistersValue(0x1337, 0x03, &hdestartreg);
            // XbhService::getPlatformInterface()->GetRegistersValue(0x1337, 0x05, &htotalreg);
            if((hdestartreg != 0x0108) || (htotalreg != 0x0671))
            {
                XLOGE("check 720p timing fail");
                setIte6265Timing720p();
                return XBH_FALSE;
            }
            break;
        case 2:
            // XbhService::getPlatformInterface()->GetRegistersValue(0x1337, 0x03, &hdestartreg);
            // XbhService::getPlatformInterface()->GetRegistersValue(0x1337, 0x05, &htotalreg);
            if((hdestartreg != 0x00C1) || (htotalreg != 0x0897))
            {
                XLOGE("check 1080p timing fail");
                setIte6265Timing1080p();
                return XBH_FALSE;
            }
            break;
    }

    return XBH_TRUE;
}

//-------------------- It6265.cpp start ---------------------------
It6265::It6265(XBH_U32 i2cBus, XBH_U32 ResetGPIO, XBH_U32 ResetLevel)
{
    XLOGD("It6265");

    Device_I2CBus = i2cBus;
    Reset_GPIO = ResetGPIO;
    Reset_Level = ResetLevel;
}

It6265::~It6265()
{

}

void It6265::start()
{
    XLOGD("It6265::start");

    XLOGD("It6265 reset chip !!! \n");
    XbhService::getPlatformInterface()->setGpioOutputValue(Reset_GPIO, !Reset_Level);
    usleep(50*1000);
    XbhService::getPlatformInterface()->setGpioOutputValue(Reset_GPIO, Reset_Level);

    XbhMWThread::run();
}

void It6265::run(const void* arg)
{
    pthread_setname_np(pthread_self(), "IT6265Task");
    while (1) {
        switch(m_step)
        {
            //polling for device ready
            case STEP_READY:
                if ( it6265_is_device_ready() ) {
                    XLOGD("it6265 is ready");
                    property_set("sys.xbh.ite6265.enable", "true");
                    m_step = STEP_INIT;
                }
                else
                {
                    XLOGD("it6265 is ready fail");
                    usleep(50 * 1000);
                }
                break;

            //init it6265
            case STEP_INIT:
                pr_info("STEP_INIT start, m_step = %d\r\n", m_step);
                XLOGD("init it6265 chip");

                it6265_chip_init();         // it6265_get_timing_by_vic(2);    -- 这个配置成了720*480*60hz
                checkIte6265Timing();       // 1080 timming

                setHdmiOutEnable(getHdmiOutEnable());
                setHdmiOutHdcpEnable(getHdmiOutHdcpEnable());
                setHdmiOutSoundEnable(getHdmiOutSoundEnable());
                // setHdmiOutResolution(getHdmiOutResolution());
                m_step = STEP_RUNING;
                pr_info("STEP_INIT end, m_step = %d\r\n", m_step);
                break;

            //handling IRQ
            case STEP_RUNING:
                XLOGD("step running, isHdmioutOn = %d\r\n", isHdmioutOn);
                //Do IRQ when it is ON
                if(isHdmioutOn) {
                    // XbhService::getPlatformInterface()->GetRegistersValue(0x1032, 0x6A, &regValue);
                    // if(regValue & 0x02)
                    // {
                        // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x6A, 0x00);
                    // }
                    //XLOGD(" it6265 irq");
                    pr_info("STEP_running \r\n");
                    if(getHdmiOutSoundEnable() == HDMI_OUT_OFF){
                        setHdmiOutSoundEnable(HDMI_OUT_OFF);
                    }
                    it6265_irq();
                }
                break;
        }
        if(tmpOutNum == 100){
            usleep(1000 * 1000);
        } else {
            tmpOutNum++;
            usleep(50 * 1000);
        }
    }
}

XBH_VOID It6265::setHdmiOutEnable(XBH_U32 onOff)
{
    pr_info("setHdmiOutEnable: isHdmioutOn = %d, onOff = %d, HDMI_OUT_ON = %d, onOff == HDMI_OUT_ON = %d\r\n", isHdmioutOn, onOff, HDMI_OUT_ON, onOff == HDMI_OUT_ON);
    isHdmioutOn = onOff == HDMI_OUT_ON;
    pr_info("setHdmiOutEnable: isHdmioutOn = %d\r\n", isHdmioutOn);
    property_set("vendor.mstar.dualdisp.mode", onOff == HDMI_OUT_ON ? "2" : "0");
    hdmitxset(0x88, 0x02, onOff == HDMI_OUT_OFF ? 0x02 : 0x00); // HDMI OUT enable
    hdmitxset(0x19, 0x07, onOff == HDMI_OUT_OFF ? 0x00 : 0x07); // HDMI interrupt

    tmpOutNum = 0; //clear zero
}

XBH_U32 It6265::getHdmiOutEnable()
{
    // 9679不支持 getBootEnv功能
    // XBH_CHAR value[10] = {0};
    // if(XbhService::getPlatformInterface()->getBootEnv("hdmi_out_mute_mode", value) == XBH_SUCCESS){
    //     XLOGD("[%s:%d] %s", __func__, __LINE__, value);
    //     return (XBH_U32)atoi(value);
    // }

    // xbh
    #ifdef hdmi_out_mute_mode
        return hdmi_out_mute_mode;
    #endif

    return HDMI_OUT_OFF;
}

XBH_VOID It6265::setHdmiOutSoundEnable(XBH_U32 onOff)
{
    // if(onOff == HDMI_OUT_ON)
    // {
    //     XbhService::getPlatformInterface()->GetRegistersValue(0x112D, 0x05, &regValue);
    //     if(regValue & (1 << 15))
    //     {
    //         regValue &= ~(1 << 15);
    //         XbhService::getPlatformInterface()->SetRegistersValue(0x112D, 0x05, regValue);
    //     }
    // }
    // else
    // {
    //     XbhService::getPlatformInterface()->GetRegistersValue(0x112D, 0x05, &regValue);
    //     if(!(regValue & (1 << 15)))
    //     {
    //         regValue |= (1 << 15);
    //         XbhService::getPlatformInterface()->SetRegistersValue(0x112D, 0x05, regValue);
    //     }
    // }
}

XBH_U32 It6265::getHdmiOutSoundEnable()
{
    // 9679不支持 getBootEnv功能
    // XBH_CHAR value[10] = {0};
    // if(XbhService::getPlatformInterface()->getBootEnv("hdmi_out_sound_mute_mode", value) == XBH_SUCCESS){
    //     XLOGD("[%s:%d] %s", __func__, __LINE__, value);
    //     return (XBH_U32)atoi(value);
    // }

    // xbh
    #ifdef hdmi_out_sound_mute_mode
        return hdmi_out_sound_mute_mode;
    #endif

    return HDMI_OUT_OFF;
}

XBH_VOID It6265::setHdmiOutHdcpEnable(XBH_U32 onOff)
{
    g_dev.cfg.EnHDCP = onOff;
    //it6265_hdcp_enable(hdcpEnable);
}

XBH_U32 It6265::getHdmiOutHdcpEnable()
{
    // 9679不支持 getBootEnv功能
    // XBH_CHAR value[10] = {0};
    // if(XbhService::getPlatformInterface()->getBootEnv("hdmi_out_hdcp_enable", value) == XBH_SUCCESS){
    //     XLOGD("[%s:%d] %s", __func__, __LINE__, value);
    //     return (XBH_U32)atoi(value);
    // }

    // xbh
    #ifdef hdmi_out_hdcp_enable
        return hdmi_out_hdcp_enable;
    #endif

    return HDMI_OUT_HDCP_OFF;
}

XBH_VOID It6265::setHdmiOutResolution(XBH_U32 resolution)
{

    switch(resolution) {
        case HDMI_OUT_480P:
            XLOGD("HDMI OUT 480p");
            setIte6265Timing480p();
            it6265_set_output_format(VF_480p);
            break;
        case HDMI_OUT_720P:
            XLOGD("HDMI OUT 720p");
            setIte6265Timing720p();
            it6265_set_output_format(VF_720p);
            break;
        case HDMI_OUT_1080P:
            XLOGD("HDMI OUT 1080p");
            setIte6265Timing1080p();
            it6265_set_output_format(VF_1080p);
            break;
        case HDMI_OUT_RESET:
        default:
            XLOGD("HDMI OUT reset timing");
            resetIte6265Timing();
            break;
    }
}

XBH_U32 It6265::getHdmiOutResolution()
{
    // 9679不支持 getBootEnv功能
    // XBH_CHAR value[10] = {0};
    // if(XbhService::getPlatformInterface()->getBootEnv("hdmi_out_resolution", value) == XBH_SUCCESS){
    //     XLOGD("[%s:%d] %s", __func__, __LINE__, value);
    //     return (XBH_U32)atoi(value);
    // }

    // xbh
    #ifdef hdmi_out_resolution
        return hdmi_out_resolution;
    #endif

    return HDMI_OUT_1080P;
}

XBH_S32 It6265::setIte6265Timing480p()
{
    XLOGD("It6265 setIte6265Timing480p\n");
    // XbhService::getPlatformInterface()->setGpioOutputValue(PAD_TCON1, 0);
    // usleep(200 * 1000);

    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x2F, 0x0802);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x6c, 0xb220);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x6d, 0x1280);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x6e, 0x0031);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x6F, 0x0014);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x5A, 0x8000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x03, 0x0014);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x15, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x01, 0x0502);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x02, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x04, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x2C, 0x0602);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x2E, 0x9010);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x30, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x31, 0x0301);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x32, 0x0002);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x33, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x34, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x35, 0x0020);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x36, 0x8000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x37, 0x0008);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x38, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x39, 0x1000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x60, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x67, 0xFFFF);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x71, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x0F, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1031, 0x10, 0x0300);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1523, 0x33, 0x0104);

    // //480 timing
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x01, 0x0004);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x02, 0x0041);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x03, 0x007E);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x04, 0x034D);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x05, 0x0359);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x07, 0x020C);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0E, 0x0009);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0F, 0x000E);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0A, 0x002D);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0B, 0x020C);

    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x01, 0x2FF0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x02, 0x03FF);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x03, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x04, 0x0000);

    // //inv h-sync v-sync
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x45, 0x000C);

    // XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x6A, 0x0000);

    // XbhService::getPlatformInterface()->setGpioOutputValue(PAD_TCON1, 1);
    //usleep(500 * 1000);
    //XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x6A, 0x00);
    return XBH_SUCCESS;
}

XBH_S32 It6265::setIte6265Timing720p()
{
    XLOGD("It6265 setIte6265Timing720p\n");
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x01, 0x0004);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x02, 0x002B);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x03, 0x0108);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x04, 0x0607);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x05, 0x0671);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x07, 0x02ED);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0A, 0x001B);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0B, 0x02EA);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0E, 0x0002);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0F, 0x0006);

    //usleep(500 * 1000);
    //XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x6A, 0x00);
    return XBH_SUCCESS;
}

XBH_S32 It6265::setIte6265Timing1080p()
{
    XLOGD("It6265 setIte6265Timing1080p\n");
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x01, 0x0001);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x02, 0x002B);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x03, 0x00C1);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x04, 0x083F);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x05, 0x0897);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x07, 0x0464);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0A, 0x0029);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0B, 0x0460);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0E, 0x0000);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0F, 0x0004);

    //usleep(500 * 1000);
    //XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x6A, 0x00);

    return XBH_SUCCESS;
}

XBH_S32 It6265::resetIte6265Timing()
{
    XLOGD("It6265 resetIte6265Timing\n");
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x01, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x02, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x03, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x04, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x05, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x07, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0A, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0B, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0E, 0x0);
    // XbhService::getPlatformInterface()->SetRegistersValue(0x1337, 0x0F, 0x0);

    //usleep(500 * 1000);
    //XbhService::getPlatformInterface()->SetRegistersValue(0x1032, 0x6A, 0x00);

    return XBH_SUCCESS;
}
//-------------------- It6265.cpp end ---------------------------