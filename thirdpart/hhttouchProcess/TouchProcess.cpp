#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
/* Unix */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <fcntl.h>
/* Linux */
#include <linux/hidraw.h>
#include <linux/version.h>
#include <errno.h>
#include <pthread.h>
#include <linux/input.h>
#include <linux/uinput.h>
//#include <cutils/properties.h>
#include "cutils/properties.h"


#include <unistd.h>
#include "string.h"

#include "TouchProcess.h"
#include "SerialProcess.h"
#include "IniFile.h"

#include "iniparser.h"

#include "hht_touch_device.h"

#define LIBUSB_SUPPORT 0

#if LIBUSB_SUPPORT
#include "libusb.h"
#endif

#include <utils/Log.h>
//===========================
//for debug
#define TAG "TouchProcess"
#define LOGD(...) //__android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // ???LOGD???#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)
#define DEBUG_PR 0
#define SAVE_BOOT_COUNT 0
//===========================

#define TOUCH_DATA_NODE  "/dev/hht_touch"
#define TOUCH_DRIVER_INFO_HH "/sys/bus/usb/drivers/hht/drvinfo"
#define TOUCH_IST_D_DATA_NODE  "/dev/isolution-touchD"
#define CMD_INSMOD_FCT_TOUCH_DRIVER    "insmod vendor/lib/modules/IstTouch.ko"
#define CMD_INSMOD_HUAXIN_TOUCH_DRIVER "insmod /system/lib/modules/MD_Touch_USB_HX.ko"
#define CMD_CHMOD_FCT_TOUCH_DATA_NODE  "chmod 0644 /dev/isolution-touchX"
#define CMD_CHMOD_FCT_TOUCH_D_DATA_NODE  "chmod 0644 /dev/isolution-touchD"

#define CMD_CHOWN_FCT_TOUCH_D_DATA_NODE  "chown system.system /dev/isolution-touchD"
#define CMD_CHOWN_FCT_TOUCH_DATA_NODE  "chown system.system /dev/isolution-touchX"

#define HX_TOUCH_DATA_NODE "/dev/hidraw-hx0"
#define CMD_CHMOD_HX_TOUCH_DATA_NODE  "chmod 0644 /dev/hidraw-hx0"
#define CMD_CHOWN_HX_TOUCH_DATA_NODE  "chown system.system /dev/hidraw-hx0"

#define TIMEOUT_FOR_TOUCH_RETURN  100   // ms

#define TOUCH_RESET_BY_POWEROFFON  0   //是否支持触摸框hidraw节点没有时，通过对触摸框进行断电上电恢复

//==================================
//touch raw data
//package head
#define PACK_ID_HUAXIN 0xfb
#define PACK_ID_FCT    0x02
#define PACK_ID_JIAGE  0x19
#define PACK_ID_HHT_T  0x02
#define PACK_ID_SONGLI 0x01

//point status value
#define POINT_STATUS_INVALID  0x00

#define POINT_STATUS_DOWN_IST 0x07
#define POINT_STATUS_UP_IST   0x04

#define POINT_ST_MOVE_JIAGE 	  0x07
#define POINT_ST_DOWN_OR_UP_JIAGE 0x04

//package struct
#define RAW_TOUCH_PACKAGE_LEN_IST_F 62
#define PACK_ID_OFFSET				0
#define POINT_DATA_START_OFFSET		1
#define VALID_POINT_NUM_OFFSET		61

//point data struct
#define POINT_STATUS_OFFSET	0  //offset in point data
#define POINT_ID_OFFSET		1

#define TEN_BYTES  10
#define BUF_LEN 64

#define MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH  42
#define MAX_POINT_NUMS_SUPPORT_FOR_ANDROID      20
#define MAX(x,y)  ((x)<(y)?(y):(x))

//=================================
//touch panel vid/pid
#define HUAXIN_VENDOR     0x1FF7
#define HUAXIN_PRODUCT    0x0001

#define HUAXIN_NEW_VENDOR    0x2FE9
#define HUAXIN_ME_PRODUCT    0x0030

#define FCT_VID 0x28e1
#define FCT_PID 0xb100

#define HHT_VID  0x2757

#define TOUCH_IST_D_VID 0x28e1
#define TOUCH_IST_D_PID 0xb100

#define TOUCH_IST_Z_PID 0x0114

#define TOUCH_IST_E_VID 0x2757
#define TOUCH_IST_E_PID 0x0117

#define TOUCH_IST_F_VID 0x2757
#define TOUCH_IST_F_PID 0x010A
#define TOUCH_IST_F_PID_2 0x0112


#define TOUCH_HUAXIN_SG_VID 0x2757
#define TOUCH_HUAXIN_SG_PID 0x0202

#define TOUCH_HUAXIN_DE_VID 0x2757
#define TOUCH_HUAXIN_DE_PID 0x0203

#define TOUCH_HUAXIN_HE6_HH_VID 0x2757
#define TOUCH_HUAXIN_HE6_HH_PID 0x0206

#define TOUCH_HUAXIN_HE6_P0209_VID 0x2757
#define TOUCH_HUAXIN_HE6_P0209_PID 0x0209

#define TOUCH_HUAXIN_SX_VID 0x2757
#define TOUCH_HUAXIN_SX_PID 0x0207

#define TOUCH_KASA_KT3_VID 0x2757
#define TOUCH_KASA_KT3_PID 0x0406

#define TOUCH_HONGHE_HC_VID 0x2757
#define TOUCH_HONGHE_HC_PID 0x0503

#define TOUCH_TIMELINK_HH_VID 0x2757
#define TOUCH_TIMELINK_HH_PID 0x0706

#define TOUCH_TIMELINK_ORIGIN_VID 0x1a86
#define TOUCH_TIMELINK_ORIGIN_PID 0x55d3

#define JIAGE_VID  0xAAEC
#define JIAGE_PID  0xC031

//TTouch transfer data to PC
#define TTOUCH_VID  0x2757
#define TTOUCH_PID  0x0501

//TTouch touch raw data
#define TTOUCH_RW_VID  0x2986
#define TTOUCH_RW_PID  0x007f

//嵩立免驱双USB电容触摸屏
#define TOUCH_SONGLI_VID 0x2757
#define TOUCH_SONGLI_PID 0x0402

#define TOUCH_SONGLI_PID2 0x0407

//其他电容触控方案,免驱双U接口，协议同富创红外双USB
#define TOUCH_HHT_VID 	0x2757
#define TOUCH_HHT_PID_1 0x0401
#define TOUCH_HHT_PID_2 0x0502
#define TOUCH_HHT_PID_3 0x0801
#define TOUCH_HHT_PID_4 0x0901

//from usb driver
#define HHT_TTOUCH_VID_PID 	"2757:0501"
#define IST_TOUCHX_VID_PID  "28e1:b100"
#define HUAXIN_ME_VID_PID   "2757:0201"
#define JIAGE_VID_PID       "aaec:c031"
#define IST_F_VID_PID 		"2757:0107"
#define SONGLI_VID_PID		"2757:0401"
#define IST_TOUCHF_VID_PID  "2757:010a"
#define IST_TOUCHD_VID_PID  "28e1:b100"

//LG InCell panel Touch
#define LG_INCELL_VID     0x1FD2
#define LG_INCELL_43_PID  0x9201
#define LG_INCELL_TOUCH_DESC_SIZE 400
#define LG_INCELL_TOUCH_PACKAGE_ID 0x10
#define LG_INCELL_TOUCH_DOWN 0x01
#define LG_INCELL_TOUCH_UP   0x00
#define LG_INCELL_TOUCH_MAX_POINTS 0x14
#define LG_INCELL_TOUCH_PACKAGE_LEN 204
#define LG_INCELL_TOUCH_POINTS_NUM_ONE_PACKAGE 20
#define LG_INCELL_TOUCH_TOTAL_POINT_NUM_OFFSET (LG_INCELL_TOUCH_PACKAGE_LEN-3)
#define LG_INCELL_TOUCH_PACKAGE_TO_MCU_LEN (LG_INCELL_TOUCH_PACKAGE_LEN +5)
#define LG_INCELL_TOUCH_PACKAGE_TO_MCU_END_OFFSET (LG_INCELL_TOUCH_PACKAGE_TO_MCU_LEN-1)
#define LG_INCELL_TOUCH_PACKAGE_TO_MCU_TOTAL_POINT_NUM_OFFSET (LG_INCELL_TOUCH_PACKAGE_TO_MCU_LEN-3)

//===================================

#define ENABLE_HEARTBEAT_PACK 1
#define READ_TIMEOUT_MAX   1  //1s


#define TOUCH_DEV_ID_IST 	   0
#define TOUCH_DEV_ID_HUXIIAN   1
#define TOUCH_DEV_ID_JIAGE     2
#define TOUCH_DEV_ID_TTOUCH    3
#define TOUCH_DEV_ID_LGINCELL  4
#define TOUCH_DEV_ID_HONGHE    5

#define USB_INTERFACE       0   // Interface number should be changed to match the number of the touch forwarding bulk
#define USB_ENDPOINT        4	// Endpoint must match touch forwarding bulk
#define USB_ENDPOINT_IN	    (LIBUSB_ENDPOINT_IN  | USB_ENDPOINT)   /* endpoint address */
#define USB_ENDPOINT_OUT	(LIBUSB_ENDPOINT_OUT | USB_ENDPOINT)   /* endpoint address */
#define BULK_WRITE_TIMEOUT  0  // always no time out

/*
 * MT_TOOL types
 */
#define MT_TOOL_FINGER		0
#define MT_TOOL_PEN			1
#define MT_TOOL_ERASER		2
#define MT_TOOL_NONE		3
#define MT_TOOL_HC_ERASER	4
#define MT_TOOL_TYPE_MAX	MT_TOOL_NONE

#define TOUCH_HUAXIN_WIDTH_VALUE_A    55   // width <= 55: pen
#define TOUCH_HUAXIN_WIDTH_VALUE_B    190  // 55 < width <= 190:finger; width > 190: Erase

#define TOUCH_FCT_WIDTH_VALUE_A    24   // width <= 24: pen
#define TOUCH_FCT_WIDTH_VALUE_B    160  // 24 < width <= 160:finger; width > 160: Erase


#define TOUCH_STATE_ENABLE  1
#define TOUCH_STATE_DISABLE 0
#define SOURCE_TOUCH  0
#define ANDROID_TOUCH 1
#define PC_STD_KEY 3
#define OPC_SHORTCUTKEY_LEN 11

#define TP_POWER_CTRL_GPIO  101
#define MD_PACKAGE_LEN      204


//#define USE_ZONE_ID


//pc standard key: cmd 0x00 key
static char PC_SDT_KEY_PAGEUP[3]   = {0x00,0x00,0x4B};
static char PC_SDT_KEY_PAGEDOWN[3] = {0x00,0x00,0x4E};
static char PC_SDT_KEY_ALT_F4[3]   = {0xE2,0x00,0x3D};


unsigned char uart_buf[MD_PACKAGE_LEN+3] = {0};



//CMDS
static char CMD_GET_TOUCH_FIRMWARE_VERSION[64]={0xA1,0xAA,0x23,0x3C,0x55,0xAA,0x11,0x02,0x00,0x00,0x12};

//cmds only for SPI double usb
static char CMD_STOP_USB_TOUCH_FOR_SOURCE[64] ={0xA1,0xAA,0x23,0x3C,0x55,0xaa,0x03,0x02,0x00,0x00,0x04};
static char CMD_START_USB_TOUCH_FOR_SOURCE[64]={0xA1,0xAA,0x23,0x3C,0x55,0xaa,0x02,0x02,0x00,0x00,0x03};

//0xA1  0xAA  0x23  0x3c + uartHotKeyCmd
//uartHotKeyCmd=0x55 0xaa 0xe0 len keyflag keyno keystatus pointxL pointxH pointyL pointyH widthL widthH heightL heightH checksum
//keyInfo(11): keyflag keyno keystatus pointxL pointxH pointyL pointyH widthL widthH heightL heightH
static char CMD_SPI_USB_OPS_SHORTCUT_KEY_PACK[64] = {0xA1,0xAA,0x23,0x3C,0x55,0xaa,0xe0,0x0b};

//0xA1 0xAA 0x23 0x3c + uartStdPcKeyCmd
//uartStdPcKeyCmd=0x55 0xaa 0x20 0x02 0xaa keyvalue checksum
static char CMD_SPI_USB_STD_PC_KEY_PACK[64]  = {0xA1,0xAA,0x23,0x3C,0x55,0xaa,0x20,0x02,0xaa};

//直发模式的信号源触控下移指令
//0xA1 0xAA 0x23 0x3C 0x55 0xaa 0xe1 0x02 0x00 status checksum
static char CMD_SET_SOURCE_TOUCH_OFFSET[64] = {0xA1,0xAA,0x23,0x3C,0x55,0xaa,0xe1,0x02,0x00};


//cmds for normal double usb touch
//0xA1 0xAA 0x22 0x3c 0xFE 0xC2 keyflag keyno keystatus pointxL  pointxH  pointyL pointyH widthLwidthH heightL heightH
static char CMD_OPS_SHORTCUT_KEY_PACK[64] = {0xA1,0xAA,0x22,0x3C,0xFE,0xC2};

//cmds for std pc key on normal double usb touch
//0xA1 0xAA 0x21 0x03 cmd  0x00  key
static char CMD_STD_PC_KEY_PACK[64]  = {0xA1,0xAA,0x21,0x03};

//分辨率键
//双USB触摸框
static char CMD_RESOLUTION_KEY_PACK[64]         = {0xA1,0xAA,0x22,0x3C,0xFE,0xC1,0x30,0x00,0x00,0x00,0x01,0x31};
//串口转USB的双U方案
static char CMD_SPI_USB_RESOLUTION_KEY_PACK[64] = {0xA1,0xAA,0x23,0x3C,0x55,0xaa,0x20,0x02,0xaa,0x01,0xcc};


//获取富创通触摸框基本信息命令，包含型号
static char CMD_GET_TOUCH_INFO[64]={0xA1,0x55,0xC1,0x01};

//手掌防误触开关
//打开:不上报手掌触摸
static char CMD_SET_PALM_ENABLE[64]  = {0x64,0x0B,0x01,0x01};
//关闭：上报板擦
static char CMD_SET_PALM_DISABLE[64] = {0x64,0x0B,0x01,0x00};

//串口转USB的双U触摸方案, 防误触功能：
//开启防误触，手掌不上报
static char CMD_SPI_USB_PALM_FUNCTION_REJECTION[64] = {0xA1,0xAA,0x23,0x3C,0x55,0xAA,0xE3,0x01,0x01,0xE4};
//关闭防误触，手掌上报为板擦
static char CMD_SPI_USB_PALM_FUNCTION_ERASER[64]    = {0xA1,0xAA,0x23,0x3C,0x55,0xAA,0xE3,0x01,0x00,0xE3};

//only for parse touch data
//#define SAVE_TOUCHDATA2FILE
#ifdef SAVE_TOUCHDATA2FILE
int dataFd = -1;
#else
#define DATA_BUF_LEN 64
#endif


//#define SAVE_PCTOUCHDATA2FILE
#ifdef SAVE_PCTOUCHDATA2FILE
int pcTouchDataFd = -1;
#endif

//#define ENABLE_GET_TIME
#ifdef ENABLE_GET_TIME
#define DATA_BUF_LEN 68
struct timeval tvNow;
int64_t tms0,tms1,tms2,tms3,tms4,tms5;
#else
#define DATA_BUF_LEN 64
#endif

int g_touchDevId = -1;
bool isFoundTouchId = false;
int gTouchType = 2; //1; //usb+MCU

#if LIBUSB_SUPPORT
libusb_context *m_libusb_ctx = NULL;
libusb_device_handle *m_libusb_handle = NULL;
#endif

int gLibUsbInited = 0;
int gLibUsbOpened = 0;

int extUsbFd = 0;
int usbFd = 0;

int gTouchFirmFd = -1; // for Jiage no driver touch device
int gTouchForPcFd = -1; // for TTouch
int uinp_fd = 0;
int global_tracking_id = -1;
bool isCalibrationMode = false;
int touchMode = 0;

double CALREFX1 = 4095.0;//32767/8
double CALREFX2 = 28671.0;//32767*7/8

double CALREFY1 = 4095.0;//32767/8
double CALREFY3 =  28671.0;//32767*7/8

double g_X_Cal_Rate,g_Y_Cal_Rate,g_X_Cal_Offset,g_Y_Cal_Offset;

//touch config toolbar
#define TOUCH_CONFIG_FILE "/system/etc/touch_config.ini"
unsigned int tbLeftXmin;
unsigned int tbLeftXmax;
unsigned int tbRightXmin;
unsigned int tbRightXmax;
unsigned int tbkY[30];

int mX_InsideCompass = 0;
int mY_InsideCompass = 0;

//int needInterceptPcTouch = 0;
bool isFirstPointDownInsideCompass = false;
bool isBeginTouch = true;
bool isPointDown  = false;  // point status is down?
int pointSt = 0;

unsigned int readErrCount = 0;
unsigned int noTouchCount = 0;

int TOOLBAR_TYPE = -1;

bool gGotZaagPanelInfoOK = false;
bool isGotZaagPanelInfoThreadRunning = false;

bool is658B_Board = false;

int maxAndroidTouchPointNum = 20; // default 20
int startPointId = 0;
int maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum - 1;

/**
0: all touch on
1: android touch off  pc touch off( but touch control key in  in toolbay not disabled)
2: only pc touch off
3: only android touch off
4: all touch disabled
*/
int touchStatus = 0;
int disabledTouchByChildLock = 0; // child lock enable would disable touch after next reboot
int isTouchLocked = 0;
int sourceTouchState  = TOUCH_STATE_ENABLE;
int androidTouchState = TOUCH_STATE_ENABLE;
bool isTouchDataForwardFlag = true; // touch data for PC is forward by main board or not
bool reportAndroidTouchInkernel = true;



bool needCalibration = false;   //use calibration apk
bool gUseDefaultParams = false; //
bool gUseConfigParams = false;  // have touch_config.ini in /syste/etc/
bool calibrateFlag = false;  // do Calibrate on Android Board
/*struct CurrPoint{
    unsigned int x;
    unsigned int y;
};
*/

struct CurrPoint{
    float x;
    float y;
};
struct CurrPoint mCurrPoint,CalibrPoint[4];

struct TouchPoint{
    unsigned char id;
    unsigned char status;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
	unsigned int toolType;
};

#define MAX_CONTACTS 	MAX(MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH,MAX_POINT_NUMS_SUPPORT_FOR_ANDROID)
struct TouchPoint mPoints[MAX_CONTACTS] ;
struct TouchPoint originPoints[MAX_CONTACTS] ; // come from Touch device

struct pointStatus{
	unsigned char isDown;  // 0: up, 1: down
};
struct pointStatus recPointStatus[MAX_CONTACTS];
unsigned int syncFlag = 0;

static char ps[MAX_CONTACTS] = {0};  //press status:0:up,1:down


struct pointIdStatus{
	unsigned char id;
	unsigned char statusValue;
	unsigned char isDown;  // 0: up, 1: down
};
struct pointIdStatus curPtStatus[MAX_CONTACTS];
struct pointIdStatus recPtStatus[MAX_CONTACTS];
struct pointIdStatus pcPtStatus[MAX_CONTACTS];

#define MAX_PENETRATION_REGION  15
#define NO_SUPPORT_ID  -1

/*
struct penetrationRegion{
	int id;
	int used;
	float x1; //top-left point
	float y1;
	float x2; //bottom-right point
	float y2;
};

struct penetrationRegion pr[MAX_PENETRATION_REGION];
*/

struct stMaskZone{
	int id;
	int used;
	int x1; //top-left point
	int y1;
	int x2; //bottom-right point
	int y2;
	int w;
	int h;
};

struct stMaskZone maskzone[MAX_PENETRATION_REGION];

//touch toolbar key for Jiage Touch
static char prePointStatusValue = 0; // 4:down or up, 7: move, 0:invalid
static char prePointStatus = 0; // 0: up, 1: down

pthread_t openHidRawDevThread;

pthread_t extTouchthread;
bool isExtTouchThreadRunning = false;
pthread_t sthread;
pthread_t readZaagPanelInfoThread;
#define MAX_PATH_LEN  128
#define MAX_MINOR       16

#define TP_MAX_XC 32767
#define TP_MAX_YC 32767

//Android UI 2K: 1920x1080; 4K 3840x2160
#define MAX_DISPLAY_UHD_WIDTH  3840.0
#define MAX_DISPLAY_UHD_HEIGHT 2160.0

#define MAX_DISPLAY_FHD_WIDTH  1920.0
#define MAX_DISPLAY_FHD_HEIGHT 1080.0

#define TP_MAX_YC_HALF  (TP_MAX_YC/2)

//add for MD touch
#define MD_PACKAGE_LEN      			64
#define VALID_POINT_NUM_POS 			61
#define POINT_STATUS_DOWN 		  	0x03
#define POINT_STATUS_UP	  		  	0x02

#define TOUCH_ACTION_DOWN_INTO     0
#define TOUCH_ACTION_MOVE_INTO     1
#define TOUCH_ACTION_RELEASE       2


#define EXT_TOUCH_PACKAGE_LEN      			64

#define UNUSED(x) (void)x


static int g_isTouchConnected = 0;
static bool haveInsmodTouchDriver = false;
static struct input_event event;

int err_no = 0;

//for monitor Touch Panel
static int readTimeoutCount = 0;
static int checkPackCount = 0;
unsigned char checkCmd[64] = {0xfc,0xea,0x00};
#define MAX_READ_TOMEOUT_COUNT  10000

int haveToolbar = 0;
bool gTTouchServiceRunning = false;

//==========================
//全局变量定义
int g_TouchDevFd = -1;  //触摸框设备的描述符，平台可以通过写数据到这个描述符
int g_TpHidRawFd = -1;  //平台读取触摸原始数据的描述符
int g_WriteReadTpFd = -1; //有驱和无驱触摸，提供的读触摸数据节点与触摸框通信的节点不一样。统一用这个描述符作为通信及转发触摸的节点
int g_touchDataFd = -1; // 读取触摸坐标数据与转发触摸数据的设备节点
int g_touchCtrlFd = -1; // 与触摸框通信的设备节点：发送指令，按键等，接收触摸框返回响应消息等
int g_isTouchOpened = 0;
int gMaxPointNumOnePackage = 6; //default is 6
int gMaxPackageNumsForSourceTouch = 4; // default max is 24 points
int gBytesNumOfOnePoint = 10;
int gPackageLen = 64;
int gPackageId = -1;
int gPointStateInvalid  = 0;
int gPointStateDrop    =  4; //红外框：点下落，但还没有完全落下
int gPointStateDown    =  7; //下落后
int gPointStateUp      =  4; //抬起
int gPointStateMove    = -1;
int gMaxPointsForAndroid = 10;
int gTotalPointNum = 0;
int gTotalPointNumOffset = 61;
int gPointDataStartOffset = 1; // 点数据在包中的偏移位置
int gPointStatusOffset = 0;    // 点状态字节在点数据的偏移位置
int g_isTouchDeviceConnected = 0; // 触摸设备是否OK
bool g_isHaveTouchDriver = false;
bool isUartToUsbTouch = false;  //是否加专门MCU实现的spi双USB触摸
bool g_isUseMultitouchDriverReport = false;
bool haveGotTouchInfo = false; //获取富创通触摸框型号
int gOffsetX = 0; //触控坐标偏移X坐标
int gOffsetY = 0; //触控坐标偏移Y坐标
int gTotalPointNumsInRawData = 0;
int gTotalValidPointsNums = 0;
int totalPackagesForRawData = 0;
int packageNums = 0;
int packageIndex = 0;
int gotPointNums = 0;

int totalValidPointNums =0;
int gotValidPointNums = 0;
int g_isUart2MCU = 0;
int g_isIstFtouch = 0;

int gValidPointNumPos = 61;
char touchVidPid[10] = {'0','0','0','0',':','0','0','0','0','\0'};

//鸿合触摸框SDK相关定义
int g_isHhtSdk = 0;
auto g_hht_device = hht::touch::Device::instance();
using Channel = hht::touch::Channel;
using Tool    = hht::touch::ToolType;

struct TouchPointToSource {
    unsigned char id;
    unsigned char curStatus;   // current status
	unsigned char preStatus;   // previous status
	unsigned char disableFlag; // if send to source or not
};

struct TouchPointToSource  pointsToSource[MAX_CONTACTS]; // points data for sending to source device (PC)
struct TouchPointToSource *pTouchPoints = pointsToSource;

static float lcdPosX2TpPosX (int Xlcd,int is4kUi){
	float Xl = (float)Xlcd;
	float Xtp;

	if(is4kUi){
	    Xtp = (Xl/MAX_DISPLAY_UHD_WIDTH)*(32767.0);
	}else{
	    Xtp = (Xl/MAX_DISPLAY_FHD_WIDTH)*(32767.0);
	}

	//LOGD("Xtp = %.2f",Xtp);
	return Xtp;
}

static float lcdPosY2TpPosY (int Ylcd,int is4kUi){
	float Yl = (float)Ylcd;
	float Ytp;
	if(is4kUi){
	    Ytp = (Yl/MAX_DISPLAY_UHD_HEIGHT)*(32767.0);
	}else{
	    Ytp = (Yl/MAX_DISPLAY_FHD_HEIGHT)*(32767.0);
	}

	//LOGD("Ytp = %.2f",Ytp);
	return Ytp;
}

static int timeoutRead(int fd,unsigned char *pBuf,int nLen,unsigned int wait_ms)
{
	int ret = 0 ;
	int num = 0 ;
	if (wait_ms > 0 && pBuf != NULL && nLen > 0)
	{
		fd_set read_fdset;
		struct  timeval timeout;
		FD_ZERO(&read_fdset);
		FD_SET(fd, &read_fdset);
		timeout.tv_sec = 0;
		timeout.tv_usec = wait_ms*1000;
		do
		{
			ret = select(fd + 1 , &read_fdset,  NULL ,  NULL , &timeout);
		    //LOGI("select,   ret = %d (errno = %d)\n",ret,errno);
		}
		while(ret <  0 && errno == EINTR);

		//LOGI("timeoutRead, out, select%d\n",ret);
		if  (ret == 0)
		{
			ret = - 1 ;
			errno = ETIMEDOUT;
			//LOGI("timeoutRead, select ret = %d, timeout\n",ret);
		}
		else if(ret == 1)
		{
			num = read(fd,pBuf,nLen);
			errno = 0;
			return num;
		}
	}
	return  ret;
}

static bool findTouchDevice(void){
    bool ret = false;
	char buf[1024]={0};
    FILE *ptr;

    if((ptr=popen("lsusb", "r"))!=NULL)
    {
        while(fgets(buf, 1024, ptr)!=NULL)
        {
		   LOGI("buf=%s",buf);
		   char *r = strstr(buf,"2757");
		   if(r != NULL)
		   {
               ret = true;
			   break;
           }
        }

        pclose(ptr);
        ptr = NULL;
    }
    else
    {
        LOGI("findTouchDevice: popen %s error\n");
    }

	LOGI("findTouchDevice: ret = %d\n",ret);
	return ret;
}

static bool isHaveTouchDriver()
{
	char touchDriver[PROPERTY_VALUE_MAX] = {0};
	property_get("vendor.xbh.touchdriver.enable", touchDriver, "false");
	LOGI("touchDriver = %s\n",touchDriver);

	if(strcmp(touchDriver,"true") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

static bool isUseMultiTouchDriverReport()
{
	char bUseMultiTouch[PROPERTY_VALUE_MAX] = {0};
	property_get("persist.vendor.xbh.multitouch.enable", bUseMultiTouch, "false");
	LOGI("touchDriver = %s\n",bUseMultiTouch);

	if(strcmp(bUseMultiTouch,"true") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

static void initTouchParams(int g_touchDevId)
{
	LOGI("initTouchParams:  g_touchDevId = %d",g_touchDevId);
	if(g_touchDevId == TOUCH_DEV_ID_IST)
	{
		gPackageLen = 64; //F_touch: 62;
		gPackageId = 0x02;
		gPointStateDown    = 0x07;
		gPointStateUp	   = 0x04;
		gTotalPointNumOffset = 61;
	}

}

static bool sendCmdToTouchByUsb(int fd,char * cmd){
	int len;
	if(fd < 0 || cmd == NULL){
		LOGI("sendCmdToTouchByUsb: fd < 0  or cmd == NULL !!!!!!");
		return false;
	}

    if ((len = write(fd, cmd, 64)) < 0)
    {
    	char * mesg = strerror(errno);
    	LOGI("sendCmdToTouchByUsb : error=%s\n",mesg);
    	return false;
    }
	LOGI("(fd=%d),len=%d, sendCmdToTouchByUsb ok",fd,len);
    return true;
}

//0xA1,0xAA,0x23,0x3C,0x55 0xaa 0x40 0x08 x1 x2 y1 y2 w1 w2 h1 h2 checksum
static bool sendTouchZonePackage(int id, int x,int y, int w, int h, int flag){
	LOGI("sendTouchZonePackage: id=%d, x=%d, y=%d, w=%d, h=%d, flag=%d", id, x, y, w, h, flag);

	bool ret = false;
	char xL = 0;
	char xH = 0;
	char yL = 0;
	char yH = 0;
	char wL = 0;
	char wH = 0;
	char hL = 0;
	char hH = 0;
	char CMD_TOUCH_ZONE[64] = {0xA1,0xAA,0x23,0x3C,0x55,0xAA,0x40,0x08,0x00};

	float xF = 0;
	float yF = 0;
	float wF = 0;
	float hF = 0;

	int xTp = 0;
	int yTp = 0;
	int wTp = 0;
	int hTp = 0;

	int i;
	int checkSum = 0;

	xF = (float)x;
	yF = (float)y;
	wF = (float)w;
	hF = (float)h;

	//LOGI("xF:%.2f,yF:%.2f,wF:%.2f,hF:%.2f",xF,yF,wF,hF);

	xTp = (int)(xF + 0.5f);
	yTp = (int)(yF + 0.5f);
	wTp = (int)(wF + 0.5f);
	hTp = (int)(hF + 0.5f);

	//LOGI("xTp:%d,yTp:%d,wTp:%d,hTp:%d",xTp,yTp,wTp,hTp);

	xL = (char)(xTp & 0xff);
	xH = (char)((xTp >> 8) & 0xff);
	yL = (char)(yTp & 0xff);
	yH = (char)((yTp >> 8) & 0xff);

	wL = (char)(wTp & 0xff);
	wH = (char)((wTp >> 8) & 0xff);
	hL = (char)(hTp & 0xff);
	hH = (char)((hTp >> 8) & 0xff);

	if(flag == 0){
		CMD_TOUCH_ZONE[6] = 0x41; // del touch zone
	}else{
		CMD_TOUCH_ZONE[6] = 0x40; // set touch zone
	}

	CMD_TOUCH_ZONE[8] 	= xL;
	CMD_TOUCH_ZONE[9] 	= xH;
	CMD_TOUCH_ZONE[10]  = yL;
	CMD_TOUCH_ZONE[11]  = yH;
	CMD_TOUCH_ZONE[12]  = wL;
	CMD_TOUCH_ZONE[13]  = wH;
	CMD_TOUCH_ZONE[14]  = hL;
	CMD_TOUCH_ZONE[15]  = hH;

	LOGI("xL:%02x xH:%02x wL:%02x wH:%02x; yL:%02x yH:%02x hL:%02x hH:%02x",CMD_TOUCH_ZONE[8],CMD_TOUCH_ZONE[9],CMD_TOUCH_ZONE[12],CMD_TOUCH_ZONE[13],
		CMD_TOUCH_ZONE[10],CMD_TOUCH_ZONE[11],CMD_TOUCH_ZONE[14],CMD_TOUCH_ZONE[15]);

	for(i=4;i<16;i++){
		checkSum += CMD_TOUCH_ZONE[i];
	}
	CMD_TOUCH_ZONE[16] = (char)(checkSum & 0xff);
	//LOGI("TP_ZONE: %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",CMD_TOUCH_ZONE[4],CMD_TOUCH_ZONE[5]
	//	,CMD_TOUCH_ZONE[6],CMD_TOUCH_ZONE[7],CMD_TOUCH_ZONE[8],CMD_TOUCH_ZONE[9],CMD_TOUCH_ZONE[10]
	//	,CMD_TOUCH_ZONE[11],CMD_TOUCH_ZONE[12],CMD_TOUCH_ZONE[13],CMD_TOUCH_ZONE[14],CMD_TOUCH_ZONE[15]
	//	,CMD_TOUCH_ZONE[16]);
	ret = sendCmdToTouchByUsb(g_touchCtrlFd, CMD_TOUCH_ZONE);
	if(!ret){
		LOGI("sendTouchZonePackage failed !!!");
	}
	return ret;
}

//富创F框，转发模式下，USB2转发来自USB1的触摸数据；触摸框上电默认模式为直接发送模式
static bool setTouchUSB2ToForwardMode(){
	bool ret = false;
	char cmdForwardDataMode[64] = {0x06,0xA8,0x01,0x00};
	ret = sendCmdToTouchByUsb(g_touchCtrlFd,cmdForwardDataMode);

	if(ret){
		LOGI("setTouchUSB2ToForwardMode ok!");
		property_set("vendor.xbh.sourcetouch.forwardmode","true");
	}
	return ret;
}

//open Touch device of FCT
bool openTouchDeviceWithDrv()
{
    int devIdx = 0;
	int fd = -1;
	LOGI("openTouchDeviceWithDrv =====");

	fd = open(TOUCH_DATA_NODE, O_RDWR);
    if (fd >= 0)
    {
		LOGI("Opened TouchX OK.\n");
        g_isTouchConnected = 1;
		g_touchDataFd = fd; //读取与转发触摸数据
		LOGI("Touch Panel: g_touchDataFd = %d, g_isTouchConnected = %d\n",g_touchDataFd,g_isTouchConnected);
		property_set("vendor.xbh.product.touchRawDataNode",TOUCH_DATA_NODE);
        return true;
    }else{
    	g_isTouchConnected = 0;
		LOGI("open %s failed !!!!!, errno = %d ",TOUCH_DATA_NODE,errno);
		return false;
	}

}

static bool getTouchPanelFirmwareVersion(int fd){
	int i;
	int len = -1;
	unsigned char buf[64] = {0};

	LOGI("getTouchPanelFirmwareVersion: fd = %d",fd);

	if(fd < 0){
		LOGI("getTouchPanelFirmwareVersion:  fd < 0 !!!!");
		return false;
	}

	char touchFwVer[PROPERTY_VALUE_MAX] = {0};
	property_get("persist.vendor.xbh.tp.version", touchFwVer, "null");
	LOGI("saved touchFwVer = %s\n",touchFwVer);


	if(!sendCmdToTouchByUsb(fd,CMD_GET_TOUCH_FIRMWARE_VERSION)){
		LOGI("getTouchPanelFirmwareVersion: send cmd failed !!!!");
		return false;
	}

	/* 触摸返回，鸿合协议：0xA1 0xAA 0x23 cmdLen 0x55 0xaa 0xa1 verLen X1 X2 …Xn checksum
		1. cmdLen = 60
		2. checksum:0x55 ~ Xn的累加和的低字节
	*/
	len = timeoutRead(fd,buf,64,TIMEOUT_FOR_TOUCH_RETURN);
	if(len <= 0)
	{
		LOGI("read touch fw version timeout !!!");
	}
	else
	{
		LOGI("len = %d, verLen = %d,cmd=0x%02x",len,buf[7],buf[2]);
		if(buf[0] == 0xA1 && buf[1] == 0xAA && buf[2] == 0x23 && buf[3] <= 0x3C
			&& buf[4] == 0x55 && buf[5] == 0xAA && buf[6] == 0xA1 && buf[7] > 0)
		{
			int verLen = buf[7];
			char touchPanelFwVer[16] = {0};
			for(i=0;(i < verLen) && (i< 56);i++)
			{
				 //LOGI("%c",buf[i+8]);
				 sprintf(touchPanelFwVer+i, "%c", buf[i+8]);
			}

			if(strcmp(touchFwVer,touchPanelFwVer) == 0)
			{
				LOGI("get touch fw version:(%s) is same as the saved version:%s",touchPanelFwVer,touchFwVer);
			}
			else
			{
				property_set("persist.vendor.xbh.tp.version",touchPanelFwVer);
				LOGI("update touch fw version: %s",touchPanelFwVer);
			}
			LOGI("receive touch version package ok");
            return true;
		}
		else
		{
			LOGI("recevied package error: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
				buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6]);
		}
    }

	return false;
}

//check have touch driver or not for FCT touch
//D touch have touch driver and F touch have no driver
static int isHaveDriverByTouchModel(int fd)
{
	int ret = -1;
	int i;
	int len = -1;
	unsigned char buf[64] = {0};

	LOGI("isHaveDriverByTouchModel: fd = %d",fd);

	if(fd < 0)
	{
		LOGI("isHaveDriverByTouchModel:  fd < 0 !!!!");
		return -1;
	}

	if(!sendCmdToTouchByUsb(fd,CMD_GET_TOUCH_INFO))
	{
		LOGI("isHaveDriverByTouchModel: send cmd failed !!!!");
		return -1;
	}

	len = timeoutRead(fd,buf,64,TIMEOUT_FOR_TOUCH_RETURN);
	if(len <= 0)
	{
		LOGI("read touch info timeout 300 ms !!!");
	}
	else
	{
		if(buf[0] == 0xA1 && buf[1] == 0x55 && buf[2] == 0xC1 && buf[3] == 0x01 && len == 64)
		{
			char touchModelId = (char)buf[10];
			char touchModel[8] = {0};
			LOGI("touchModelId = 0x%x",touchModelId);
			sprintf(touchModel, "%c",touchModelId);

			LOGI("touchModel=%s",touchModel);

			char touchPanelSize[8] = {0};
			sprintf(touchPanelSize, "%c%c",buf[11],buf[12]);
			property_set("vendor.xbh.touch.model",touchModel);
			property_set("vendor.xbh.touch.size",touchPanelSize);

			if(touchModelId == 'D' || touchModelId == 'd'
				|| touchModelId == 'X' || touchModelId == 'x')
			{
				return 1;
			}
			else if(touchModelId == 'F' || touchModelId == 'f')
			{
				return 0;
			}
		}
		else
		{
			LOGI("recevied package error: 0x%02x 0x%02x 0x%02x 0x%02x",
				buf[0],buf[1],buf[2],buf[3]);
		}
	}
	return ret;
}


//0: palm erase; 1:palm rejection
static bool isPalmRejectionEnabled(void)
{
	char prop[PROPERTY_VALUE_MAX] = {0};
	property_get("persist.vendor.xbh.palmtouchtype", prop, "0");
	LOGI("isPalmRejectionEnabled = %s\n",prop);

	if(strcmp(prop,"1") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

static bool detectAndOpenTouchDevice(){
	int i,ret = false;
	int fd = -1;
	char fileName[16]="0";
	struct hidraw_devinfo dinfo;
	int vid = 0;
	int pid = 0;

	if(g_touchCtrlFd>0)
	{
		close(g_touchCtrlFd);
		g_touchCtrlFd = -1;
		usleep(200*1000);
	}

	if(g_touchDataFd >= 0){
		close(g_touchDataFd);
		gTouchFirmFd = -1;
		gGotZaagPanelInfoOK = false;
		g_touchDataFd = -1;
		usleep(200*1000);
	}

	g_isTouchOpened = 0;

	//LOGI("detectAndOpenTouchDevice...");

	for (int i = 0; i < MAX_MINOR; i++)
	{
		sprintf(fileName, "/dev/hidraw%d", i);
		fd = open(fileName, O_RDWR);
		if (fd >= 0)
		{
			if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0)
			{
				vid = dinfo.vendor & 0xFFFF;
				pid = dinfo.product & 0xFFFF;
				//LOGI("vid = 0x%04x, pid = 0x%04x,  %s",vid,pid,fileName);
				if (((vid == HHT_VID) && (pid == TOUCH_IST_Z_PID))
					||((vid == TOUCH_IST_D_VID)&&(pid == TOUCH_IST_D_PID))
					||((vid == TOUCH_IST_E_VID)&&(pid == TOUCH_IST_E_PID)))
				{
					LOGI("found IST touch device");
					g_touchDevId = TOUCH_DEV_ID_IST;
					g_touchCtrlFd = fd; //用于触摸框通信，发送按键、指令
					property_set("vendor.xbh.product.touchDevNode",fileName);

					bool bRet = openTouchDeviceWithDrv();
					if(bRet){
						LOGI("found IST touchZ and open raw data node ok, g_touchDataFd = %d",g_touchDataFd);
						g_isTouchOpened = 1;
						g_isHaveTouchDriver = true;

						gPackageLen = 64;
						gPackageId = 0x02;
						gPointStateDown    = 0x07;
						gPointStateUp      = 0x04;
						gTotalPointNumOffset = 61;
						startPointId = 1;
						isTouchDataForwardFlag = true;
						maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
						property_set("vendor.xbh.sourcetouch.forwardmode","true");
						sprintf(touchVidPid, "%04x:%04x",vid,pid);
						gMaxPackageNumsForSourceTouch = (MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH - 1)/gMaxPointNumOnePackage + 1;

						ret = true;
						break;
					}

				}
				else if (((vid == HHT_VID) && (pid == TOUCH_HUAXIN_HE6_HH_PID))
							|| ((vid == TOUCH_HUAXIN_HE6_P0209_VID) && (pid == TOUCH_HUAXIN_HE6_P0209_PID)))
				{
					LOGI("found Huaxin HE6 touch device");
					g_touchDevId = TOUCH_DEV_ID_HUXIIAN;
					g_touchCtrlFd = fd; //用于触摸框通信，发送按键、指令
					property_set("vendor.xbh.product.touchDevNode",fileName);

					bool bRet = openTouchDeviceWithDrv();
					if(bRet){
						LOGI("Opened Huaxin HE6 raw data node ok, g_touchDataFd = %d",g_touchDataFd);
						g_isTouchOpened = 1;
						g_isHaveTouchDriver = true;

						gPackageLen = 64;
						gPackageId = 0x02;
						gPointStateDown    = 0x07;
						gPointStateUp	   = 0x04;
						gTotalPointNumOffset = 61;
						startPointId = 1;
						isTouchDataForwardFlag = true;
						maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
						property_set("vendor.xbh.sourcetouch.forwardmode","true");
						sprintf(touchVidPid, "%04x:%04x",vid,pid);
						gMaxPackageNumsForSourceTouch = (MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH - 1)/gMaxPointNumOnePackage + 1;

						ret = true;
						break;
					}

				}
				else if(((vid == TOUCH_IST_F_VID) && ((pid == TOUCH_IST_F_PID) || (pid == TOUCH_IST_F_PID_2)))
						|| ((vid == TOUCH_HUAXIN_SG_VID) && (pid == TOUCH_HUAXIN_SG_PID)))
				{
					g_touchDevId = TOUCH_DEV_ID_IST;
					g_isHaveTouchDriver = false;
					reportAndroidTouchInkernel = false;
					LOGI("found IST touchF");
					if(g_touchCtrlFd < 0)
					{
						g_touchCtrlFd = fd;
						LOGI("found IST F_touch device node , g_touchCtrlFd = %d, fileName=%s",g_touchCtrlFd,fileName);
						setTouchUSB2ToForwardMode();

						property_set("vendor.xbh.product.touchDevNode",fileName);

						getTouchPanelFirmwareVersion(g_touchCtrlFd);

						continue;  // continue find the node for touch raw data
					}
					else if(g_touchDataFd < 0)
					{
						g_touchDataFd= fd; //for touch raw data
						LOGI("found IST F_touch raw data node, g_touchDataFd = %d, fileName = %s",g_touchDataFd, fileName);
						g_isTouchOpened = 1;

						gPackageLen = 62;
						gPackageId = 0x02;
						gPointStateDown    = 0x07;
						gPointStateUp	   = 0x04;
						gTotalPointNumOffset = 61;

						isTouchDataForwardFlag = true;
						startPointId = 1;
						maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;

						isUartToUsbTouch = false;

						g_isIstFtouch = 1;

						property_set("vendor.xbh.product.touchRawDataNode",fileName);

						sprintf(touchVidPid, "%04x:%04x",vid,pid);
						property_set("vendor.xbh.product.touch.VidPid",touchVidPid);
						property_set("vendor.xbh.touch.model","F");

						ret = true;
						break;
					}

				}
				else if((vid == TOUCH_HUAXIN_SX_VID) && (pid == TOUCH_HUAXIN_SX_PID))
				{
					g_touchDevId = TOUCH_DEV_ID_HUXIIAN;
					g_isHaveTouchDriver = false;
					reportAndroidTouchInkernel = false;
					LOGI("found HUAXIN SX_touch");
					if(g_touchCtrlFd < 0)
					{
						g_touchCtrlFd = fd;
						LOGI("found HUAXIN SX_touch device node , g_touchCtrlFd = %d, fileName=%s",g_touchCtrlFd,fileName);
						setTouchUSB2ToForwardMode();

						property_set("vendor.xbh.product.touchDevNode",fileName);

						getTouchPanelFirmwareVersion(g_touchCtrlFd);

						continue;  // continue find the node for touch raw data
					}
					else if(g_touchDataFd < 0)
					{
						g_touchDataFd= fd; //for touch raw data
						LOGI("found HUAXIN SX_touch raw data node, g_touchDataFd = %d, fileName = %s",g_touchDataFd, fileName);
						g_isTouchOpened = 1;

						gPackageLen = 62;
						gPackageId = 0x02;
						gPointStateDown    = 0x07;
						gPointStateUp	   = 0x04;
						gTotalPointNumOffset = 61;

						isTouchDataForwardFlag = true;
						startPointId = 1;
						maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;

						isUartToUsbTouch = false;

						property_set("vendor.xbh.product.touchRawDataNode",fileName);

						sprintf(touchVidPid, "%04x:%04x",vid,pid);
						property_set("vendor.xbh.product.touch.VidPid",touchVidPid);
						property_set("vendor.xbh.touch.model","SX");

						ret = true;
						break;
					}
				}
				else if (((vid == TOUCH_HHT_VID)
							&& ((pid == TOUCH_SONGLI_PID)
								|| (pid == TOUCH_SONGLI_PID2)
								|| (pid == TOUCH_HHT_PID_1)
								|| (pid == TOUCH_HHT_PID_2)
								|| (pid == TOUCH_HHT_PID_3)
								|| (pid == TOUCH_HHT_PID_4)
								|| (pid == TOUCH_KASA_KT3_PID)
								|| (pid == TOUCH_TIMELINK_HH_PID)))
							|| (vid == TOUCH_TIMELINK_ORIGIN_VID && pid == TOUCH_TIMELINK_ORIGIN_PID))
				{ //电容双USB触摸
					if(g_touchCtrlFd < 0)
					{
						g_touchCtrlFd = fd; //与触摸框通信的节点，发送指令、按键
						property_set("vendor.xbh.product.touchDevNode",fileName);
						continue;  // continue find the node for touch raw data
					}
					else if(g_touchDataFd < 0)
					{
						g_touchDataFd = fd;    // 用于读取触摸数据

						gPackageLen = 62;
						gPackageId = 0x02;
						gPointStateDown    = 0x07;
						gPointStateUp	   = 0x04;
						gTotalPointNumOffset = 61;

						isTouchDataForwardFlag = false; //给PC的usb触摸数据是触摸框直接发送模式
						startPointId = 1;
						maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
						property_set("vendor.xbh.sourcetouch.forwardmode","false");
						property_set("vendor.xbh.product.touchRawDataNode",fileName);
						sprintf(touchVidPid, "%04x:%04x",vid,pid);
						property_set("vendor.xbh.product.touch.VidPid",touchVidPid);
						getTouchPanelFirmwareVersion(g_touchCtrlFd);
						g_isTouchDeviceConnected = 1;
						g_isTouchOpened = 1;
						isUartToUsbTouch = true;
						ret = true;
						break;
					}

				}//电容双USB触摸控
				else if ((vid == JIAGE_VID) && (pid == JIAGE_PID) )
				{
					g_touchDevId = 2; // Jiage touch
					if(gTouchFirmFd < 0) {
						gTouchFirmFd = fd;
						LOGI("found Jiage touch firmware device node , gTouchFirmFd = %d, fileName=%s",gTouchFirmFd,fileName);

						if(gTouchFirmFd>0 && !gGotZaagPanelInfoOK){
							getJagePanelVersionInfo();
						}

						continue;  // continue find the node for touch firmware info

					}else if(g_touchCtrlFd < 0){
						g_touchDevId = 2; // Jiage touch
						g_touchCtrlFd = fd; //for touch data device
						LOGI("found Jiage touch, g_touchCtrlFd = %d, fileName = %s",g_touchCtrlFd, fileName);
						//initTouchConfig(g_touchDevId);
						property_set("vendor.xbh.svc.touch","JiaGe");
						g_isTouchConnected = 1;
						ret = true;
						break;
					}
				}
				else  if (((vid == TOUCH_HUAXIN_DE_VID) && (pid == TOUCH_HUAXIN_DE_VID)))
				{
					LOGI("found HuaXin DE Touch~~~~~ gTouchType = %d",gTouchType);
					g_touchDevId = 1; // Huaxin touch
					ret = true;
					break;
				}
				else if ((vid == TTOUCH_RW_VID) && (pid == TTOUCH_RW_PID))
				{
					g_touchDevId = TOUCH_DEV_ID_TTOUCH;
					g_TouchDevFd = fd; //for reading touch raw data from touch
					LOGI("found Ttouch, g_TouchDevFd = %d, fileName = %s",g_TouchDevFd, fileName);
					g_isTouchOpened = 1;
					startPointId = 1;
					maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
					ret = true;
					break;
				}else if(vid == LG_INCELL_VID){
					LOGI("found LG InCell Touch");
					g_touchDevId = TOUCH_DEV_ID_LGINCELL;
					g_touchDataFd= fd; //for touch raw data
					g_isTouchOpened = 1;
					gPackageLen = 64;
					gPackageId = LG_INCELL_TOUCH_PACKAGE_ID;
					gPointStateDown    = 0x01;
					gPointStateUp	   = 0x00;
					gTotalPointNumOffset = LG_INCELL_TOUCH_TOTAL_POINT_NUM_OFFSET;
					startPointId = 1;
					g_isUart2MCU = 1;
					isTouchDataForwardFlag = true;
					maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
					property_set("vendor.xbh.sourcetouch.forwardmode","true");
					sprintf(touchVidPid, "%04x:%04x",vid,pid);
					property_set("vendor.xbh.product.touchDevNode",fileName);
					ret = true;
					break;
				}else if ((vid == HHT_VID) && (pid == TOUCH_HONGHE_HC_PID))
				{
					LOGI("found HongHe HC touch device");
					g_touchDevId = TOUCH_DEV_ID_HONGHE;
					g_touchDataFd = fd; //用于触摸框通信，发送按键、指令
					property_set("vendor.xbh.product.touchDevNode",fileName);

					g_isTouchOpened = 1;
					g_isHaveTouchDriver = false;
					gPackageLen = 64;
					gPackageId = 0x02;
					gPointStateDown    = 0x07;
					gPointStateUp	   = 0x04;
					gTotalPointNumOffset = 61;
					startPointId = 1;
					isTouchDataForwardFlag = false;
					maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
					property_set("vendor.xbh.sourcetouch.forwardmode","false");
					sprintf(touchVidPid, "%04x:%04x",vid,pid);
					gMaxPackageNumsForSourceTouch = (MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH - 1)/gMaxPointNumOnePackage + 1;
					ret = true;

					g_isHhtSdk = 1;
					sourceTouchState = TOUCH_STATE_DISABLE;

					if (!g_hht_device) {
						LOGI("Can't get hht_touch");
						ret = false;
					}

					if (g_hht_device->open()) {
						LOGI("Open hht_touch failed!!!");
						ret = false;
					}

					LOGI("setToolEvent HC PenNib Finger Eraser");
					if (g_hht_device->setToolEvent(
							Channel::System | Channel::Output,
							Tool::PenNib | Tool::Finger | Tool::Eraser | Tool::PenCap)) {
						LOGI("setToolEvent failed!!!");
						ret = false;
					}

					if (g_hht_device->setEraserCode(MT_TOOL_HC_ERASER)) {
						LOGI("setEraserCode failed!!!");
						ret = false;
					}

					LOGI("setPenSize(18, 70)");
					if (g_hht_device->setPenSize(18, 70)) {
						LOGI("setPenSize failed!!!");
						ret = false;
					}
				}
			}
			close(fd);
		}
	}

	//reset status
	char prop[PROPERTY_VALUE_MAX] = {'\0'};
	if((property_get("vendor.xbh.product.touchDevNode", prop, NULL) > 0)
		&& (strcmp(prop,"null") != 0)) {
		property_set("vendor.xbh.product.touchDevNode","null");
		property_set("vendor.xbh.product.touchRawDataNode","null");
		property_set("vendor.xbh.product.touch.VidPid","0000:0000");
	}

	if(ret)
	{
		if(g_isHhtSdk)
		{
			if(isPalmRejectionEnabled())
			{
				if(g_hht_device->setPalmRejection(Channel::System | Channel::Output, true) != 0) {
					LOGI("send palm rejection cmd faild !!!!!, try again");
					usleep(50*1000);
					if(g_hht_device->setPalmRejection(Channel::System | Channel::Output, true) != 0) {
						ret = false;
					}
				}
			}
		} else {
			if(isPalmRejectionEnabled() && (g_touchCtrlFd > 0))
			{
				ret &= sendCmdToTouchByUsb(g_touchCtrlFd,CMD_SET_PALM_ENABLE);
				if(!ret)
				{
					LOGI("send palm rejection cmd faild !!!!!, try again");
					usleep(50*1000);
					ret = sendCmdToTouchByUsb(g_touchCtrlFd,CMD_SET_PALM_ENABLE);
				}
			}
		}

		LOGI("detectAndOpenTouchDevice:");
		LOGI("    g_touchDevId = %d",g_touchDevId);
		LOGI("    g_touchDataFd = %d",g_touchDataFd);
		LOGI("    g_isHaveTouchDriver = %d",g_isHaveTouchDriver);
		LOGI("    g_isTouchOpened = %d",g_isTouchOpened);
		LOGI("    gPackageLen = %d",gPackageLen);
		LOGI("    gMaxPointNumOnePackage = %d",gMaxPointNumOnePackage);
		LOGI("    gPackageId = %d",gPackageId);
		LOGI("    gPointStateDown = %d",gPointStateDown);
		LOGI("    gPointStateUp = %d",gPointStateUp);
		LOGI("    gTotalPointNumOffset = %d",gTotalPointNumOffset);
		LOGI("    startPointId = %d",startPointId);
		LOGI("    isTouchDataForwardFlag = %d",isTouchDataForwardFlag);
		LOGI("    gMaxPackageNumsForSourceTouch = %d",gMaxPackageNumsForSourceTouch);
	}
	else
	{
		LOGI("detectAndOpenTouchDevice:    faild !!!!!!!!!!!");
	}
	LOGI("detectAndOpenTouchDevice:    ret = %d",ret);
	return ret;
}

static bool isTouchDisconnect(void){
	bool ret = false;
	FILE *fp;
	char str[10];
	//get vid:pid from driver
	fp = fopen("/sys/class/usbnode/usbdev/touch_device_id" , "r");
	if(fp == NULL) {
		LOGI("open touch_device_id failed !!!!\n");
	}else{
		char * p = fgets(str, sizeof(str), fp);
		//LOGI("touch_device_id = %s",str);
		if(p != NULL){
			if(strcmp(str,"0000:0000") == 0){
				ret = true;
			}
		}
	}
	if(fp != NULL){
		fclose(fp);
	}
	if(ret){
		LOGI("touch disconnected");
	}
	return ret;
}

static int initTouchDeviceId(void){
	int ret = -1;
	FILE *fp;
	char str[9];

	//get vid:pid from driver
	fp = fopen("/sys/class/usbnode/usbdev/touch_device_id" , "r");
	if(fp == NULL) {
		LOGI("open touch_device_id failed !!!!\n");
	}else{
		char * p = fgets(str, 9, fp);
		LOGI("touch_device_id=%s",str);
		if(p != NULL){
			if(strcmp(str,HHT_TTOUCH_VID_PID) == 0){
				ret = TOUCH_DEV_ID_TTOUCH;
			}else if((strcmp(str,IST_TOUCHX_VID_PID) == 0)
				|| (strcmp(str,IST_F_VID_PID) == 0)){
				ret = TOUCH_DEV_ID_IST;
			}else if(strcmp(str,HUAXIN_ME_VID_PID) == 0){
				ret = TOUCH_DEV_ID_HUXIIAN;
			}else if(strcmp(str,JIAGE_VID_PID)){
				ret = TOUCH_DEV_ID_JIAGE;
			}else{
				LOGI("not detect support touch in usb driver !!!!!");
			}
		}
		fclose(fp);
	}

	if(ret >= 0){
		LOGI("intTouchDeviceId: g_touchDevId=%d",ret);
	}
	return ret;
}

static int isFoundTTouch(){
	int ret = 0;
	FILE *fp;
	char str[10];
	fp = fopen("/sys/class/usbnode/usbdev/touch_device_id" , "r");
	if(fp == NULL) {
		LOGI("open touch_device_id failed:  %s\n",strerror(errno));
	}else{
		char * p = fgets(str, 10, fp);
		ALOGD("[TouchDet] touchid=%s",str);
		if(p != NULL && (strcmp(str,"2757:0501") == 0)) {
			property_set("vendor.xbh.svc.touch", "TTouch");
			LOGI("Found T-Touch ~~~~~\n");
			ret = 1;
		}
		fclose(fp);
	}
	return ret;
}

bool isFoundTtouchDev(){
	bool bRet = false;
	FILE *fp = NULL;
	char data[128] = {0};

	fp = popen("lsusb", "r");
	if (fp == NULL)
	{
		LOGI("isFoundTtouchDev: popen error!\n");
		return false;
	}

	while (fgets(data, sizeof(data), fp) != NULL)
	{
		LOGI("%s", data);
		if(strstr(data, "2757:0501") != NULL){
			bRet = true;
			break;
		}
		memset(data,0,sizeof(data));
	}
	pclose(fp);

	return bRet;

}

static bool isTTouchProgramRunning(){
	char prop[PROPERTY_VALUE_MAX] = "0";
	if((property_get("vendor.xbh.ttouch.program.running", prop, NULL) != 0)
		&& (strcmp(prop,"true") == 0)) {
		return true;
	}

	return false;
}

static int tryGetTouchFwVersion()
{
    if (g_touchCtrlFd > 0)
    {
        unsigned char cmd[64] = {0xfc,0xa3,0x00,0x00,0x00,0x00};
        int ret = write(g_touchCtrlFd,cmd,64);
		if(ret < 0){
			LOGI("write file fail !!!!(get tourch firmware version)");
			return 0;
		}
		return 1;
	}
	return 0;
}

bool SendEAdata(int gfvs_fd)
{
	int ret = 0;
	if (gfvs_fd > 0)
	{
		unsigned char cmd[64] = {0xfc,0xea,0x00,0x00,0x00,0x00};
		ret = write(gfvs_fd,cmd,64);
		if( ret == -1)
		{
			LOGI("SendEAdata failed !!!! gfvs_fd:%d, errno=%d(%s)\n",gfvs_fd,errno,strerror(errno));
			return false;
		}
		//LOGI("send SendEAdata ok");
		return true;
	}
	return false;
}

bool GetEAdata(int gfvs_fd)
{
	int ret = 0;
	if (gfvs_fd > 0)
	{
		int i = 0;
		unsigned char buf[64] = {0};
		if (SendEAdata(gfvs_fd) == false){
			LOGI("GetEAdata:  SendEAdata error!!!! \n");
			return false;
		}

		for (i=0;i<100;i++)
		{
			usleep(1000);
			memset(buf,0,64);
			ret = read(gfvs_fd,buf,64);
			if (ret == -1)
			{
				LOGI("GetEAdata: read fail !!!!  gfvs_fd:%d, errno = %d(%s)\n",gfvs_fd,errno,strerror(errno));
				return false;
			}
			if (buf[0] == 0xfc && buf[1] == 0xea && buf[2] == 0x80)
			{
				//LOGI("GetEAdata=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
				return true;
			}
		}
	}
	LOGI("GetEAdata fail !!!!  %d;%d;%s\n",ret,errno,strerror(errno));
	return false;
}

static int checkTouchDeviceActive(){
	if (g_touchCtrlFd > 0)
	{
		LOGI("checkTouchDeviceActive, g_touchCtrlFd = %d\n",g_touchCtrlFd);
		int ret = write(g_touchCtrlFd,checkCmd,64);
		if(ret < 0){
			LOGI("checkTouchDeviceActive  write file fail!!!");
			return 0;
		}

		return 1;
	}
	return 0;
}

int ReadData(int fd,int n)
{
    int count = 0;
    if (fd > 0) {
        int i = 0;
        int j = 0;
        unsigned char buf[64] = {0};
        for (i=0;i<(n*100);i++)
        {
            usleep(1000);
            memset(buf,0,64);
            read(fd,buf,64);
            if (buf[0] == PACK_ID_HUAXIN)
            {
                LOGD("TouchProcess  ReadData=");
                for(j=0;j<64;j++)
                {
                    LOGD("TouchProcess  %02x,",buf[j]);
                }
                count++;
                if (count > n)
                    break;
            }
        }
    }
    return count;
}

void sendKeyEvent(unsigned short keyCode, int isDown){
	if(uinp_fd <= 0){
	  LOGI("sendKeyEvent: uinp_fd <= 0\n");
	  return;
	}
	LOGI("sendKeyEvent: keyCode = %d, isDown = %d\n",keyCode,isDown);
	/* report key event */
	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, NULL);
	event.type = EV_KEY;
	event.code = keyCode;
	event.value = isDown;
	write(uinp_fd, &event, sizeof(event));

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(uinp_fd, &event, sizeof(event));

}


//get from init.svc.touch
static int getTouchDevice()
{
	int id = -1;
	char prop[PROPERTY_VALUE_MAX] = "0";
	if(property_get("vendor.xbh.svc.touch", prop, NULL) != 0) {
		if(strcmp(prop,"TTouch") == 0){
			LOGI("use TTouch");
			id = 3;
		}else if(strcmp(prop,"IST") == 0){
			LOGI("use FCT Touch");
			id = 0;
		}else if(strcmp(prop,"Huaxin") == 0){
			LOGI("use Huaxin Touch");
			id = 1;
		}else if(strcmp(prop,"JiaGe") == 0){
			LOGI("use JiaGe Touch");
			id = 2;
		}
	}
	return id;
}


static void initPanelSize()
{
	char prop[PROPERTY_VALUE_MAX] = {0};
	if(property_get("ro.vendor.xbh.panelSize", prop, NULL) != 0) {
		if(strcmp(prop,"86") == 0){
			LOGI("initPanelSize: panel size: 86");
			TOOLBAR_TYPE = 2;
		}else if(strcmp(prop,"75") == 0){
			LOGI("initPanelSize: panel size: 75");
			TOOLBAR_TYPE = 1;
		}else if(strcmp(prop,"65") == 0){
			LOGI("initPanelSize: panel size: 65");
			TOOLBAR_TYPE = 0;
		}else{
			LOGI("initPanelSize: not specified panel size!!!");
			TOOLBAR_TYPE = -1;
		}
	}else{
		LOGI("initPanelSize: unknown panel size !!!");
		TOOLBAR_TYPE = -1;
	}

}

static int isTouchDisabledByChildLock()
{
	int iRet;
	int isChildLockEnabled = 0, isDefTouchDisabled = 0;
	char childLockState[PROPERTY_VALUE_MAX] = "000";
	if(property_get("persist.vendor.xbh.childLock", childLockState, NULL) != 0) {
		if(strcmp(childLockState,"true") == 0){
			LOGI("child lock is enabled!!!");
			isChildLockEnabled = 1;
		}else if(strcmp(childLockState,"false") == 0){
			LOGI("child lock is disabled !!!");
			isChildLockEnabled = 0;
		}else{
			LOGI("default isChildLockEnabled = %d",isChildLockEnabled);
			isChildLockEnabled = 0;
		}
	}else{
		LOGI("persist.vendor.xbh.childLock is null,  isChildLockEnabled = %d",isChildLockEnabled);
		isChildLockEnabled = 0;
	}

	char defTouchState[PROPERTY_VALUE_MAX] = "0";
	if(property_get("persist.vendor.xbh.defTouchState", defTouchState, NULL) != 0) {
		if(strcmp(defTouchState,"2") == 0){
			LOGI("touch disabled by default");
			isDefTouchDisabled = 1;
		}else if(strcmp(defTouchState,"1") == 0){
			LOGI("touch enabled by default");
			isDefTouchDisabled = 0;
		}else{
			LOGI("default isDefTouchDisabled = %d",isDefTouchDisabled);
			isDefTouchDisabled = 0;
		}
	}else{
		isDefTouchDisabled = 0;
		LOGI("persist.vendor.xbh.defTouchState is null, isDefTouchDisabled = %d",isDefTouchDisabled);
	}

	LOGI("isChildLockEnabled = %d, isDefTouchDisabled = %d",isChildLockEnabled,isDefTouchDisabled);

	if(isChildLockEnabled == 1){ // enable child lock
		char strStatus[8]={0};
	    sprintf(strStatus, "%d", 4);
		LOGI("strStatus = %s",strStatus);
		property_set("vendor.xbh.procuct.touchStatus",strStatus);
		iRet = 1;
	}else if(isDefTouchDisabled == 1){ // only disable touch
		char strStatus[8]={0};
	    sprintf(strStatus, "%d", 1);
		LOGI("strStatus = %s",strStatus);
		property_set("vendor.xbh.procuct.touchStatus",strStatus);
		iRet = 1;
	}else{
		iRet = 0;
	}
	LOGI("isTouchDisabledByChildLock:  iRet = %d",iRet);
	return iRet;
}

static void createTouchScreen()
{
    struct uinput_user_dev uinp;
    struct input_event event;

	LOGI("createTouchScreen...");
    uinp_fd = open("/dev/uinput", O_WRONLY|O_NONBLOCK);
    if(uinp_fd < 0) {
        LOGI("TouchProcess   Unable to open /dev/uinput");
        LOGI("err: %s", strerror(errno));
        return;
    }

    // configure touch device event properties
    memset(&uinp, 0, sizeof(uinp));
    strncpy(uinp.name, "XBHTouchScreen", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    uinp.id.bustype = BUS_USB;
    uinp.absmin[ABS_MT_SLOT] = 0;
    uinp.absmax[ABS_MT_SLOT] = MAX_CONTACTS;
    uinp.absmin[ABS_MT_TOUCH_MAJOR] = 0;
    uinp.absmax[ABS_MT_TOUCH_MAJOR] = TP_MAX_XC;
    uinp.absmin[ABS_MT_POSITION_X] = 0;
    uinp.absmax[ABS_MT_POSITION_X] = TP_MAX_XC;
    uinp.absmin[ABS_MT_POSITION_Y] = 0;
    uinp.absmax[ABS_MT_POSITION_Y] = TP_MAX_YC;
    uinp.absmin[ABS_MT_TRACKING_ID] = 0;
    uinp.absmax[ABS_MT_TRACKING_ID] = 65535;
	uinp.absmin[ABS_MT_TOOL_TYPE] = 0;
    uinp.absmax[ABS_MT_TOOL_TYPE] = MT_TOOL_TYPE_MAX + 1;
    // Setup the uinput device
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY);
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOOL_RUBBER);
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL);
    // Touch
    ioctl (uinp_fd, UI_SET_EVBIT,  EV_ABS);
    ioctl (uinp_fd, UI_SET_ABSBIT, ABS_MT_SLOT);
    ioctl (uinp_fd, UI_SET_ABSBIT, ABS_MT_TOUCH_MAJOR);
    ioctl (uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_X);
    ioctl (uinp_fd, UI_SET_ABSBIT, ABS_MT_POSITION_Y);
    ioctl (uinp_fd, UI_SET_ABSBIT, ABS_MT_TRACKING_ID);
	ioctl (uinp_fd, UI_SET_ABSBIT, ABS_MT_TOOL_TYPE);
    ioctl (uinp_fd, UI_SET_PROPBIT, INPUT_PROP_DIRECT);


    /* Create input device into input sub-system */
    write(uinp_fd, &uinp, sizeof(uinp));
    ioctl(uinp_fd, UI_DEV_CREATE);
	LOGI("create XBHTouchScreen ok!");
}

static bool device_writeEvent(int fd, uint16_t type, uint16_t keycode, int32_t value)
{
    struct input_event ev;
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = type;
    ev.code = keycode;
    ev.value = value;
    if (write(fd, &ev, sizeof(struct input_event)) < 0)
    {
    	char * mesg = strerror(errno);
    	LOGD("TouchProcess   nibiru uinput errormag info :%s\n",mesg);
    	return false;
    }
    return true;
}

bool isAllPointsRelease(){
	int i;
	for(i=0;i<MAX_CONTACTS;i++){
		if(recPointStatus[i].isDown != 0){
			return false;
		}
	}
	//LOGI("all record points have release");
	return true;
}

static bool writeToUsb(int fd,char *msg)
{
   	int len;
    if ((len = write(fd, msg, 64)) < 0)
    {
    	char * mesg = strerror(errno);
    	LOGI("TouchProcess  writeToUsb :%s\n",mesg);
    	return false;
    }
	LOGI("(fd=%d),len=%d, writeToUsb ok",fd,len);
    return true;
}

//0: usb+uart, 1: usb+MCU; 2: usb+usb, 3: single usb
static int initTouchType(){
	int touchType = 2; //default is usb+MCU
	char prop[PROPERTY_VALUE_MAX] = {0};
	if(property_get("persist.vendor.xbh.touchType", prop, NULL) != 0) {
		if(strcmp(prop,"2") == 0){
			touchType = 2;
		}else if(strcmp(prop,"1") == 0){
			touchType = 1;
		}
	}
	return touchType;
}

static int initTouchConfig(int g_touchDevId){
	int i;
	char itemName[128] = {0};
	int configTpSize;
	int ret = -1;

	dictionary *ini;

	LOGI("initTouchConfig: g_touchDevId = %d",g_touchDevId);

	if(g_touchDevId < 0){
		LOGI("invalid g_touchDevId !!!!!");
		return -1;
	}

	ini = iniparser_load(TOUCH_CONFIG_FILE);
	if(ini == NULL){
		LOGI("initTouchConfig: can not iniparser_load %s",TOUCH_CONFIG_FILE);
		return -1;
	}

	configTpSize = iniparser_getint(ini, "TOUCH_PANEL_CONFIG:size", -1);
	LOGI("initTouchConfig: g_touchDevId = %d,configTpSize = %d",g_touchDevId,configTpSize);

	if(ini != NULL){
		iniparser_freedict(ini);
	}

	if(ret == 0){
		LOGI("initTouchConfig OK, g_touchDevId = %d",g_touchDevId);
	}

	return ret;
}

static bool addMaskZone(int x1,int y1,int x2,int y2){
	bool ret = false;
	int i = 0;
	for(i=0;i< MAX_PENETRATION_REGION;i++){
		if(maskzone[i].used == 0){
			maskzone[i].used = 1;
			maskzone[i].x1 = x1;
			maskzone[i].y1 = y1;
			maskzone[i].x2 = x2;
			maskzone[i].y2 = y2;
			ret = true;
			break;
		}
	}

	if(i == MAX_PENETRATION_REGION){
		LOGI("have no unused item !!! max is %d !!!!",MAX_PENETRATION_REGION);
	}else{
		LOGI("addMaskZone:%d, (%d,%d)~(%d,%d)",i,x1,y1,x2,y2);
	}

	/*
	for(i=0;i< MAX_PENETRATION_REGION;i++){
		if(maskzone[i].used == 1){
			LOGI("MaskZone %d: (%d,%d)~(%d,%d)",i,maskzone[i].x1,maskzone[i].y1,maskzone[i].x2,maskzone[i].y2);
		}
	}
	*/

	return ret;
}

static bool deleteMaskZone(int x1,int y1,int x2,int y2){
	bool ret = false;
	int i = 0;
	for(i=0;i< MAX_PENETRATION_REGION;i++){
		if(maskzone[i].used == 1
			&& maskzone[i].x1 == x1
			&& maskzone[i].y1 == y1
			&& maskzone[i].x2 == x2
			&& maskzone[i].y2 == y2){
			maskzone[i].used = 0;
			ret = true;
			break;
		}
	}

	if(i == MAX_PENETRATION_REGION){
		LOGI("not found the mask zone item !!!");
	}else{
		LOGI("deleteMaskZone: %d,%d,%d,%d",x1,y1,x2,y2);
	}

	return ret;
}

static int sendTouchDataToUsb(unsigned const char* pbuf,int len)
{
	int ret = -1;

	if(g_touchDataFd > 0)
    {
		ret = write(g_touchDataFd,pbuf,len);
        if(ret <= 0)
        {
            LOGI("sendTouchDataToUsb (g_touchDataFd = %d )write error!!!  ret = %d, errno = %d \n",g_touchDataFd,ret,errno);
        }
		#if 0
		else
		{
			LOGI("send PC touch, ret=%d, %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
				ret,pbuf[0],pbuf[1],pbuf[2],pbuf[3],pbuf[4],pbuf[5],pbuf[6],pbuf[7],pbuf[8],pbuf[9],pbuf[10],pbuf[11],pbuf[12]);
		}
		#endif
    }else{
		LOGI("g_touchDataFd < 0 !!!!");
	}

	return ret;
}

bool setupTouchDriver(int touchDeviceId)
{
	bool bRet = false;
	LOGI("setupTouchDriver: touchDeviceId = %d",touchDeviceId);

	return bRet;
}

bool openTTouchRawDataUsb(void)
{
	int i;
	int fd = -1;
	char fileName[16]="0";
	struct hidraw_devinfo dinfo;

	LOGI("openTTouchRawDataUsb...");

	for (int i = 0; i < MAX_MINOR; i++)
    {
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_RDWR);
        if (fd >= 0)
        {
            if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0)
            {
				LOGI("vid = 0x%04x, pid = 0x%04x,  %s",dinfo.vendor & 0xFFFF,dinfo.product & 0xFFFF,fileName);
				if (((dinfo.vendor & 0xFFFF) == TTOUCH_RW_VID) && ((dinfo.product & 0xFFFF) == TTOUCH_RW_PID) )
                {
					g_TouchDevFd = fd;
					LOGI("g_TouchDevFd = %d",g_TouchDevFd);

					LOGI("Open  TTouch ok");
					g_touchDevId = TOUCH_DEV_ID_TTOUCH;
					g_isTouchConnected = 1;

					startPointId = 1;
					maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum -1;
					LOGI("openTTouchRawDataUsb ok");

                	return true;
                }
            }

            close(fd);
			fd = -1;
        }
    }

	LOGI("openTTouchRawDataUsb failed !!!");
	return false;
}

bool isTouchConnected(int vid,int pid)
{
	struct hidraw_devinfo dinfo;
    char fileName[MAX_PATH_LEN];
	int fd;

    for (int i = 0; i < MAX_MINOR; i++)
    {
        sprintf(fileName, "/dev/hidraw%d", i);
        fd = open(fileName, O_RDWR);
        if (fd >= 0)
        {
            if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0)
            {
				//LOGI("isTouchConnected: 0x%04X, 0x%04X\n",dinfo.vendor,dinfo.product);
				if (((dinfo.vendor & 0x0FFFF) == vid) && ((dinfo.product & 0x0FFFF) == pid))
                {
                    //LOGI("isTouchConnected: 0x%04X,0x%04X, ( %s ) sucess!!!!!\n",vid,pid,fileName);
					close(fd);
                    return true;
                }
            }
            close(fd);
        }
    }
    return false;
}


int readTouchRawData(int fd,unsigned char *pBuf,int nLen,unsigned int wait_seconds)
{
	int ret = 0 ;
	int num = 0 ;
	if (wait_seconds > 0 && pBuf != NULL && nLen > 0)
	{
		fd_set read_fdset;
		struct  timeval timeout;
		FD_ZERO(&read_fdset);
		FD_SET(fd, &read_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do
		{
			ret = select(fd + 1 , &read_fdset,  NULL ,  NULL , &timeout);
		    //LOGI("select,   ret = %d (errno = %d)\n",ret,errno);
		}
		while(ret <  0 && errno == EINTR);

		//LOGI("Hxread, out, select%d\n",ret);
		if  (ret == 0)
		{
			ret = - 1 ;
			errno = ETIMEDOUT;
			//LOGI("Hxread, select ret = %d, timeout\n",ret);
		}
		else if(ret == 1)
		{
			num = read(fd,pBuf,nLen);
			errno = 0;
			return num;
		}
	}
	return  ret;
}


//判断触摸点是否落在安卓OSD区域内
static bool isPointDownInsideOsdRegion(int index,unsigned char buf[])
{
	int i;
	int ptX,ptY;

	if(buf == NULL || index > gMaxPointNumOnePackage){
		LOGI("error !!!! buf == NULL, or index > 5");
		return false;
	}

	i = index;
	ptX = buf[10*i + 4]*256 + buf[10*i + 3];
	ptY = buf[10*i + 6]*256 + buf[10*i + 5];


	if(DEBUG_PR) LOGI("i:%d,   %5d, %5d",i,ptX,ptY);

	for(i=0;i<MAX_PENETRATION_REGION;i++){
		if(DEBUG_PR) LOGI("maskzone[%d]: (%5d, %5d) ~ (%5d, %5d)", i,maskzone[i].x1, maskzone[i].y1 + gOffsetY, maskzone[i].x2, maskzone[i].y2 + gOffsetY);
		if(maskzone[i].used
		    && (ptX >= maskzone[i].x1 && ptX <= maskzone[i].x2)
			&& (ptY >= maskzone[i].y1) && (ptY <= maskzone[i].y2)){
			LOGI("(%d,%d) down in Region(%d)",ptX,ptY,i);
			return true;
		}

	}

	return false;
}


unsigned char filterBuf[60*7]  = {0x00};
unsigned char   sendBuf[60*7]  = {0x00};
bool isFirstMoveToUpZone = true;

static void processPointScreenDown(int x, int y, int pointIndex, unsigned char filterBuf[],int action){
	int i;
	int newY = 0;
	int newX = 0;

	if((gOffsetY <= 0) && (gOffsetX <= 0))
	{
		LOGI("processPointLocatInScreenUpZone: it is not screen down now !!!!");
		return;
	}

	newY = y - gOffsetY;
	//LOGI("processPointScreenDown:  y = %d newY = %d,  gOffsetY = %d",y,newY,gOffsetY);
	if(newY >= 0)
	{
		filterBuf[packageIndex*60+pointIndex*10+4] = newY % 256;
		filterBuf[packageIndex*60+pointIndex*10+5] = newY >> 8;
		isFirstMoveToUpZone = true;
	}
	else // in Screen Up Zone
	{
		if(action == TOUCH_ACTION_DOWN_INTO) // down into
		{
			filterBuf[packageIndex*60+pointIndex*10] = 0; //set to invalid point ,filter this point
			gTotalValidPointsNums--;
		}
		else if(action == TOUCH_ACTION_MOVE_INTO) // move into
		{
			if(isFirstMoveToUpZone)
			{
				filterBuf[packageIndex*60+pointIndex*10] = 0x04; //release
				filterBuf[packageIndex*60+pointIndex*10+4] = 0;
				filterBuf[packageIndex*60+pointIndex*10+5] = 0;
				isFirstMoveToUpZone = false;
			}
			else
			{
				filterBuf[packageIndex*60+pointIndex*10] = 0; //set to invalid point ,filter this point
				gTotalValidPointsNums--;
			}

		}
		else if(TOUCH_ACTION_RELEASE)
		{
			filterBuf[packageIndex*60+pointIndex*10] = 0; //set to invalid point ,filter this point
			gTotalValidPointsNums--;
		}

	}

	//screen move right
	newX = x - gOffsetX;
	if(newX >= 0)
	{
		filterBuf[packageIndex*60+pointIndex*10+2] = newX % 256;
		filterBuf[packageIndex*60+pointIndex*10+3] = newX >> 8;
	}
	else
	{
		if(action == TOUCH_ACTION_DOWN_INTO) // down into
		{
			filterBuf[packageIndex*60+pointIndex*10] = 0; //set to invalid point ,filter this point
			gTotalValidPointsNums--;
		}
		else if(action == TOUCH_ACTION_MOVE_INTO) // move into
		{
			filterBuf[packageIndex*60+pointIndex*10] = 0x04; //release
		}
	}

}


//raw data buf to filter buf whitch set point state to 0x00 if it down in mask zone
static bool filterPointsForSourceTouch(unsigned char buf[],const int packageIndex,const int totalRawPointNums){
	int i;
	int ptId;
	int ptStatus;
	bool haveUpPoint = false;
	bool haveMovePoint = false;
	int touchPackageOffset = 0;
	int x = 0;
	int y = 0;
	int xL = 0;
	int xH = 0;
	int yL = 0;
	int yH = 0;
	int totalPointNums = 0;

	//LOGI("filter: packageIndex = %d, totalRawPointNums = %d",packageIndex, totalRawPointNums);
	if(buf == NULL || packageIndex < 0 || packageIndex >= gMaxPackageNumsForSourceTouch )
	{
		LOGI("filterPointsForSourceTouch: buf is NULL !!!, or packageIndex invalid !!!!");
		return false;
	}

	//LOGI("buf: %02x %02x, x= %d , y= %d ",buf[1],buf[2],buf[3]+buf[4]*256,buf[5]+buf[6]*256);

	if(packageIndex == 0) //the first package
	{
		memset(filterBuf,0,60*gMaxPackageNumsForSourceTouch);
		gTotalValidPointsNums = totalRawPointNums;
	}

	memcpy(filterBuf+packageIndex*60,buf+1,60); // get raw data

	//LOGI("filter: gTotalValidPointsNums = %d",gTotalValidPointsNums);

	//parse one package, which point is need filter
	for(i = 0; i < gMaxPointNumOnePackage; i++)
    {
		ptId =  buf[(10*i)+2];
		ptStatus= buf[(10*i)+1] & 0x0F;
		pointsToSource[ptId].curStatus  = ptStatus;
		pointsToSource[ptId].id = ptId;

		//LOGI("id: %02d: pre: %02x, cur:%02x", ptId, pointsToSource[ptId].preStatus, pointsToSource[ptId].curStatus);

		// point down?
		if(pointsToSource[ptId].curStatus == 0x07) //0x07
		{
			if(pointsToSource[ptId].preStatus == 0x00) // 0x00 -> 0x07: down
			{
				//LOGI("down,  0x00 -> 0x07");
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					filterBuf[packageIndex*60+(10*i)] = 0;
					gTotalValidPointsNums--;

					LOGI("first down, disable pt[%d]",ptId);
				}

				if(gOffsetY > 0 || gOffsetX > 0)
				{
					yL = buf[i*10+5];
					yH = buf[i*10+6];
					y = yL + (yH << 8);

					xL =  buf[i*10+3];
					xH =  buf[i*10+4];

					x = xL + (xH << 8);
					processPointScreenDown(x,y,i,filterBuf,TOUCH_ACTION_DOWN_INTO);
				}

			}
			else if(pointsToSource[ptId].preStatus == 0x04) // 0x04 -> 0x07: down
			{
				//LOGI("down,  0x04 -> 0x07");
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					filterBuf[packageIndex*60+(10*i)] = 0;
					gTotalValidPointsNums--;
					LOGI("first down, disable pt[%d]",ptId);
				}

				if(gOffsetY > 0 || gOffsetX > 0)
				{
					yL = buf[i*10+5];
					yH = buf[i*10+6];
					y = yL + (yH << 8);

					xL =  buf[i*10+3];
					xH =  buf[i*10+4];

					x = xL + (xH << 8);
					processPointScreenDown(x,y,i,filterBuf,TOUCH_ACTION_DOWN_INTO);
				}

			}
			else if(pointsToSource[ptId].preStatus == 0x07) // 0x07 -> 0x07: move
			{
				//LOGI("move,  0x07 -> 0x07");
				if(pointsToSource[ptId].disableFlag == 1)
			    {
					filterBuf[packageIndex*60+(10*i)] = 0;
					gTotalValidPointsNums--;
					//LOGI("move, disable pt[%d]",ptId);
			    }

				if(gOffsetY > 0 || gOffsetX > 0)
				{
					yL = buf[i*10+5];
					yH = buf[i*10+6];
					y = yL + (yH << 8);

					xL =  buf[i*10+3];
					xH =  buf[i*10+4];

					x = xL + (xH << 8);
					processPointScreenDown(x,y,i,filterBuf,TOUCH_ACTION_MOVE_INTO);
				}

			}

	   }
	   else if(pointsToSource[ptId].curStatus == 0x04) // drop or up
	   {
			if(pointsToSource[ptId].preStatus == 0x07) // 0x07 -> 0x04: up
			{
	   			//LOGI("up,	0x07 -> 0x04");
				if(pointsToSource[ptId].disableFlag == 1)
			    {
					filterBuf[packageIndex*60+(10*i)] = 0;
					pointsToSource[ptId].disableFlag = 0;
					gTotalValidPointsNums--;
					LOGI("up, disable pt[%d]",ptId);
			    }

				if(gOffsetY > 0 || gOffsetX > 0)
				{
					yL = buf[i*10+5];
					yH = buf[i*10+6];
					y = yL + (yH << 8);

					xL =  buf[i*10+3];
					xH =  buf[i*10+4];

					x = xL + (xH << 8);
					processPointScreenDown(x,y,i,filterBuf,TOUCH_ACTION_RELEASE);
				}

			}
			else if(pointsToSource[ptId].preStatus == 0x00) // 0x00 -> 0x04: drop
			{

				//LOGI("up,	0x00 -> 0x04");
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					filterBuf[packageIndex*60+(10*i)] = 0;
					gTotalValidPointsNums--;

					//LOGI("first drop, disable pt[%d]",ptId);
				}

				if(gOffsetY > 0 || gOffsetX > 0)
				{
					yL = buf[i*10+5];
					yH = buf[i*10+6];
					y = yL + (yH << 8);

					xL =  buf[i*10+3];
					xH =  buf[i*10+4];

					x = xL + (xH << 8);
					processPointScreenDown(x,y,i,filterBuf,TOUCH_ACTION_DOWN_INTO);
				}
			}
			else if(pointsToSource[ptId].preStatus == 0x04) // 0x04 -> 0x04: drop
			{
				//LOGI("drop,	0x04 -> 0x04");
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					filterBuf[packageIndex*60+(10*i)] = 0;
					gTotalValidPointsNums--;

					LOGI("first drop, disable pt[%d]",ptId);
				}

				if(gOffsetY > 0 || gOffsetX > 0)
				{
					yL = buf[i*10+5];
					yH = buf[i*10+6];
					y = yL + (yH << 8);

					xL =  buf[i*10+3];
					xH =  buf[i*10+4];

					x = xL + (xH << 8);
					processPointScreenDown(x,y,i,filterBuf,TOUCH_ACTION_DOWN_INTO);
				}
			}


	   }

	    pointsToSource[ptId].preStatus = ptStatus;

	}

	//LOGI("filter: validPtNums = %d",gTotalValidPointsNums);
	//LOGI("filter: %02x %02x %02x %02x",filterBuf[packageIndex*60+0],filterBuf[packageIndex*60+1],
	//	filterBuf[packageIndex*60+4],filterBuf[packageIndex*60+5]);

	return true;
}


static int sendFilterPointsToSource(unsigned char *filterBuf,int totalRawPointNums,int totalValidPointsNums)
{
	int i,j;
	int ret = -1;
	unsigned char tmpBuf[64]={0};
	int touchPackageOffset = 0;

	int totalPackageNums = 0;
	char * src  =  (char *)filterBuf;
	char * dest =  (char *)sendBuf;

	//LOGI("sendFilterPointsToSource: totalValidPointsNums = %d",totalValidPointsNums);
	if(filterBuf == NULL || totalValidPointsNums <= 0)
	{
		LOGI("sendTouchDataToSource: filterBuf is NULL !!! totalValidPointsNums == 0 !!!!");
		return -1;
	}

	//LOGI("filterBuf: %02x %02x, %02x %02x, %02x %02x, %02x %02x",*src,*(src+1),*(src+60),*(src+61),
	//	*(src+180),*(src+181),*(src+200),*(src+201));

	totalPackageNums = (totalValidPointsNums - 1)/gMaxPointNumOnePackage + 1;

	//LOGI("sFPT:  totalValidPointsNums = %d, totalPackageNums = %d",totalValidPointsNums,totalPackageNums);
	//LOGI("send: totalRawPointNums = %d,tValidPtsNums = %d, tPackNums = %d",
	//	totalRawPointNums,totalValidPointsNums,totalPackageNums);

	memset(sendBuf,0,60*gMaxPackageNumsForSourceTouch);
	for(i = 0;i < totalRawPointNums; i++)
	{
		if(*src != 0x00 )
		{
			memcpy(dest,src,10);
			dest = dest + 10;
		}

		src = src + 10;
	}


	if(!g_isHaveTouchDriver) // IST touchF
	{
		if(g_isIstFtouch == 1) {
			tmpBuf[0] = 0x06;
			tmpBuf[1] = 0x18;
		} else {
			tmpBuf[0] = 0x64;
			tmpBuf[1] = 0x00;
		}
		tmpBuf[2] = 0x02;
		touchPackageOffset = 3;

		for(i =0; i < totalPackageNums;i++)
		{
			//LOGI("sendBuf: %02x %02x ...",*(sendBuf+i*60),*(sendBuf+i*60+1));
			memcpy(tmpBuf+touchPackageOffset,sendBuf+i*60,60); //F_touch: 06 18 02 xx ....
			if(i == 0)
			{
				tmpBuf[63] = totalValidPointsNums;
			}
			else
			{
				tmpBuf[63] = 0;
			}
			//LOGI("tmpBuf: %02x %02x ... %02x",tmpBuf[3],tmpBuf[4],tmpBuf[63]);
			ret = sendTouchDataToUsb(tmpBuf, 64);

		}
	}
	else
	{
		touchPackageOffset = 0;
		for(i =0; i < totalPackageNums;i++)
		{
			tmpBuf[0] = 0x02;
			memcpy(tmpBuf+1,sendBuf+i*60,60); //F_touch: 02 xx ....
			if(i == 0)
			{
				tmpBuf[61] = totalValidPointsNums;
			}
			else
			{
				tmpBuf[61] = 0;
			}
			ret = sendTouchDataToUsb(tmpBuf, 64);
		}
	}
	return ret;
}

//buf: save raw data read from hidraw node
static int sendTouchDataToSource(unsigned char buf[])
{
	int i = 0;
	int ptId;
	int ptStatus;
	bool haveUpPoint = false;
	bool haveMovePoint = false;
	unsigned char tmpBuf[BUF_LEN];
	int touchPackageOffset = 0;
	int x = 0;
	int y = 0;
	int xL = 0;
	int xH = 0;
	int yL = 0;
	int yH = 0;
	int totalPointNums = 0;

	if(buf == NULL)
	{
		LOGI("sendTouchDataToSource: buf is NULL !!!");
		return -1;
	}

	memset(tmpBuf, 0, BUF_LEN);
	if(g_touchDevId == TOUCH_DEV_ID_IST)
	{
		if(!g_isHaveTouchDriver) // IST touchF
		{
			tmpBuf[0] = 0x06;
			tmpBuf[1] = 0x18;
			touchPackageOffset = 2;
			memcpy(tmpBuf+touchPackageOffset,buf,gPackageLen); //F_touch: 06 18 02 xx ....
		}
		else
		{
			touchPackageOffset = 0;
			memcpy(tmpBuf+touchPackageOffset,buf,gPackageLen);
		}
	}

	totalPointNums = buf[10*i+touchPackageOffset+gTotalPointNumOffset];

	for(i = 0; i < gMaxPointNumOnePackage; i++)
	{
		ptId =  buf[(10*i)+2];
		ptStatus= buf[(10*i)+1] & 0x0F;
		pointsToSource[ptId].curStatus  = ptStatus;
		pointsToSource[ptId].id = ptId;

		//LOGI("gOffsetX = %d, gOffsetY = %d",gOffsetX,gOffsetY);
		//screen move down?
		if(gOffsetY > 0){
			yL = buf[i*10+5];
			yH = buf[i*10+6];
			if((y = yL + (yH << 8) - gOffsetY) >= 0){
				tmpBuf[i*10+5+touchPackageOffset] = y%256;
				tmpBuf[i*10+6+touchPackageOffset] = y >> 8;
			}else{
				tmpBuf[i*10+1+touchPackageOffset] = 0; //set to invalid point
			}
		}

		//screen move right?
		if(gOffsetX > 0){
			xL =  buf[i*10+3];
			xH =  buf[i*10+4];
			if((x = xL + (xH << 8) - gOffsetX) >= 0){
				tmpBuf[i*10+3+touchPackageOffset] = x%256;
				tmpBuf[i*10+4+touchPackageOffset] = x >> 8;
			}else{
				tmpBuf[i*10+1+touchPackageOffset] = 0; //set to invalid point
			}
		}

		//LOGI("%02d: preStatus = 0x%02x, curStatus = 0x%02x", ptId, pointsToSource[ptId].preStatus, pointsToSource[ptId].curStatus);

		// point down?
		if(pointsToSource[ptId].curStatus == 0x07) //0x07
		{
			if(pointsToSource[ptId].preStatus == 0x00) // 0x00 -> 0x07: down
			{
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
					LOGI("first down, set pt[%d].status ===> 0",ptId);
				}
			}
			else if(pointsToSource[ptId].preStatus == 0x04) // 0x04 -> 0x07: down
			{
				/*
				if(pointsToSource[ptId].disableFlag == 1)
				{
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
					//LOGI("move, set pt[%d].status ===> 0",ptId);
				}
				*/
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
					LOGI("first down, set pt[%d].status ===> 0",ptId);
				}
			}
			else if(pointsToSource[ptId].preStatus == 0x07) // 0x07 -> 0x07: move
			{
				if(pointsToSource[ptId].disableFlag == 1)
			    {
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
			    }
			}
		}
		else if(pointsToSource[ptId].curStatus == 0x04) // drop or up
		{
			if(pointsToSource[ptId].preStatus == 0x07) // 0x07 -> 0x04: up
			{
				if(pointsToSource[ptId].disableFlag == 1)
				{
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
					pointsToSource[ptId].disableFlag = 0;
				}
				pointsToSource[ptId].preStatus = ptStatus;
			}
			else if(pointsToSource[ptId].preStatus == 0x00) // 0x00 -> 0x04: drop
			{
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
					LOGI("first down, set pt[%d].status ===> 0",ptId);
				}
			}
			else if(pointsToSource[ptId].preStatus == 0x04) // 0x04 -> 0x04: drop
			{
				if(isPointDownInsideOsdRegion(i,buf))
				{
					pointsToSource[ptId].disableFlag = 1;
					tmpBuf[(10*i)+1+touchPackageOffset] = 0;
					LOGI("first down, set pt[%d].status ===> 0",ptId);
				}
			}
		}
		pointsToSource[ptId].preStatus = ptStatus;
	}
	return sendTouchDataToUsb(tmpBuf, BUF_LEN);
}

static void parseAndReportToAndroid(unsigned char buf[])
{
	int i;
	if (buf[0] == gPackageId)
	{
		if(buf[gTotalPointNumOffset]<=6 && buf[gTotalPointNumOffset]>0) // only one package, 1~6 points
		{

			for(i = 0; i < 6; i++)
			{
				mPoints[i].status = buf[(10*i)+1] & 0x0F;
				mPoints[i].id = buf[(10*i)+2];
				mPoints[i].x = buf[10*i + 4] * 256 + buf[10*i + 3];
				mPoints[i].y = buf[10*i + 6] * 256 + buf[10*i + 5];



				mPoints[i].width = buf[10*i + 8] * 256 + buf[10*i + 7];
				mPoints[i].height = buf[10*i + 10] * 256 + buf[10*i + 9];


				//1: pen, 2: fingler,3:erase
				int toolType = (buf[(10*i)+1] & 0xF0) >> 4;
				if(toolType == 2){
					mPoints[i].toolType = MT_TOOL_FINGER;
				}else if(toolType == 3){
					mPoints[i].toolType = MT_TOOL_ERASER;
				}else if(toolType == 1) {
					mPoints[i].toolType = MT_TOOL_PEN;
				}else{
					mPoints[i].toolType = MT_TOOL_FINGER;
				}

				#if 0
				if(mPoints[i].status != 0){
					LOGI("raw: (id:%02d)(%05d, %05d),st:%02x, w = %03d, h = %03d, toolType = %d",
						mPoints[i].id,mPoints[i].x,mPoints[i].y,mPoints[i].status,
						mPoints[i].width,mPoints[i].height,mPoints[i].toolType);
				}
				#endif
			}

			//report all valid point data
			for(i=0;i< 6 ;i++)
			{
				//multi-touch-protocol Type B:
				if(g_touchDevId == TOUCH_DEV_ID_JIAGE)
				{
					if(mPoints[i].status== 0x04){	//press or up
						if(ps[mPoints[i].id] == 0){ //press
							ps[mPoints[i].id] = 1;
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, mPoints[i].id); // report down
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
							//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, ((reportY = mPoints[i].y-touchY_increment)>=0?reportY:0));
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);

							if(mPoints[i].toolType == MT_TOOL_ERASER)
							{
								LOGI("report BTN_TOOL_RUBBER 1");
								device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, 0x141);
								device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 1);
							}
							else
							{
								device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger
							}

							recPointStatus[mPoints[i].id].isDown = 1;
							//LOGI("p:%d,%d",mPoints[i].x,mPoints[i].y);
						}else{
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);  // report up
							recPointStatus[mPoints[i].id].isDown = 0;
							ps[mPoints[i].id] = 0;
						}
					}else if(mPoints[i].status == 0x07){ //move
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);

						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
						//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, ((reportY = mPoints[i].y-touchY_increment)>=0?reportY:0));
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);
						recPointStatus[mPoints[i].id].isDown = 1;
						ps[mPoints[i].id] = 1;
						//LOGI("p:%d,%d",mPoints[i].x,mPoints[i].y);
					}

				}
				else //Huaxin or FCT Touch
				{
					if(mPoints[i].status == 0x03 || mPoints[i].status == 0x07)  // touch status: down
					{
						if(mPoints[i].id <= maxAndroidTouchPointId)
					    {
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, mPoints[i].id); // report down
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
							//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, ((reportY = mPoints[i].y-touchY_increment)>=0?reportY:0));
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);

							//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger or erase
							if(mPoints[i].toolType == MT_TOOL_ERASER)
							{
								LOGI("report BTN_TOOL_RUBBER 1");
								device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, 0x141);
								device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 1);
							}
							else
							{
								device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger
							}

							recPointStatus[mPoints[i].id].isDown = 1;
							//LOGI("1-dn: %02d %d,%d",mPoints[i].id,mPoints[i].x,mPoints[i].x);
							device_writeEvent(uinp_fd, EV_KEY, BTN_TOUCH, 1);

						}

					}
					else if(mPoints[i].status == 0x02 || mPoints[i].status == 0x04)  // touch status: up
					{
						if(mPoints[i].id <= maxAndroidTouchPointId){
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);  // report up

							recPointStatus[mPoints[i].id].isDown = 0;
							//LOGI("1-up:0x%02X",mPoints[i].id);

							if(mPoints[i].toolType == MT_TOOL_ERASER)
							{
								LOGI("report BTN_TOOL_RUBBER 0");
								device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 0);
							}

						}

					}
				}
			}

			//one point or multi points down or up,only report EV_SYN-SYN_REPORT one time
			device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
			memset(mPoints,0,MAX_CONTACTS*sizeof(TouchPoint));

			#ifdef ENABLE_GET_TIME
			gettimeofday(&tvNow, NULL);
			tms4 = (int64_t)tvNow.tv_sec*1000 + tvNow.tv_usec/1000;
			LOGI("tms4=%lld(%d)",tms4,tms4-tms3);
			#endif
		}
		else //buf[gTotalPointNumOffset] > 6 or buf[gTotalPointNumOffset] == 0
		{
			// point > 6 , include 2 package
			if(g_touchDevId == TOUCH_DEV_ID_JIAGE)
			{
				if(buf[gTotalPointNumOffset] > 6)
				{
					// the first package update 0~5 point data
					for(i = 0; i < 6; i++)
					{
						mPoints[i].status = buf[(10*i)+1] & 0x0F;
						mPoints[i].id = buf[(10*i)+2];
						mPoints[i].x = buf[10*i + 4]*256 + buf[10*i + 3];
						mPoints[i].y = buf[10*i + 6]*256 + buf[10*i + 5];

						int toolType = (buf[(10*i)+1] & 0xF0) >> 4;
						if(toolType == 2){
							mPoints[i].toolType = MT_TOOL_FINGER;
						}else if(toolType == 3){
							mPoints[i].toolType = MT_TOOL_ERASER;
						}else if(toolType == 1){
							mPoints[i].toolType = MT_TOOL_PEN;
						}else{
							mPoints[i].toolType = MT_TOOL_FINGER;
						}

						originPoints[i].x = mPoints[i].x;
						originPoints[i].y = mPoints[i].y;

						if(calibrateFlag && (gUseDefaultParams || needCalibration || gUseConfigParams))
						{
							mPoints[i].x = g_X_Cal_Rate * mPoints[i].x + g_X_Cal_Offset;
							mPoints[i].y = g_Y_Cal_Rate * mPoints[i].y + g_Y_Cal_Offset;

							if(mPoints[i].x >= 32768){
								mPoints[i].x = 32767;
							}

							if(mPoints[i].y >= 32768){
								mPoints[i].y = 32767;
							}
						}
						mPoints[i].width = buf[10*i + 8]*256 + buf[10*i + 7];
						mPoints[i].height = buf[10*i + 10]*256 + buf[10*i + 9];

						#if 0
						if(mPoints[i].status != 0){
							LOGI("raw: (id:%02d)(%05d, %05d),st:%02x, width = %03d, toolType = %d",mPoints[i].id,mPoints[i].x,mPoints[i].y,buf[(10*i)+1],mPoints[i].width,mPoints[i].toolType);
						}
						#endif

					}

				}
				else if(buf[gTotalPointNumOffset] == 0)
				{
					// the second package update 6~12 point data
					for(i = 0; i < 6; i++)
					{
						//record data
						mPoints[6+i].status = buf[(10*i)+1] & 0x0F;
						mPoints[6+i].id = buf[(10*i)+2];
						mPoints[6+i].x = buf[10*i + 4]*256 + buf[10*i + 3];
						mPoints[6+i].y = buf[10*i + 6]*256 + buf[10*i + 5];

						int toolType = (buf[(10*i)+1] & 0xF0) >> 4;
						if(toolType == 2){
						mPoints[i].toolType = MT_TOOL_FINGER;
						}else if(toolType == 3){
							mPoints[i].toolType = MT_TOOL_ERASER;
						}else if(toolType == 1){
							mPoints[i].toolType = MT_TOOL_PEN;
						}else{
							mPoints[i].toolType = MT_TOOL_FINGER;
						}


						originPoints[6+i].x = mPoints[6+i].x;
						originPoints[6+i].y = mPoints[6+i].y;

						if(calibrateFlag && (gUseDefaultParams || needCalibration || gUseConfigParams))
						{
							mPoints[6+i].x = g_X_Cal_Rate * mPoints[6+i].x + g_X_Cal_Offset;
							mPoints[6+i].y = g_Y_Cal_Rate * mPoints[6+i].y + g_Y_Cal_Offset;

							if(mPoints[6+i].x >= 32768){
								mPoints[6+i].x = 32767;
							}

							if(mPoints[6+i].y >= 32768){
								mPoints[6+i].y = 32767;
							}
						}
						mPoints[6+i].width = buf[10*i + 8]*256 + buf[10*i + 7];
						mPoints[6+i].height = buf[10*i + 10]*256 + buf[10*i + 9];

						#if 0
						if(mPoints[i].status != 0){
							LOGI("raw: (id:%02d)(%05d, %05d),st:%02x, width = %03d, toolType = %d",mPoints[i].id,mPoints[i].x,mPoints[i].y,buf[(10*i)+1],mPoints[i].width,mPoints[i].toolType);
						}
						#endif
					}
				}

				for(i=0;i< MAX_CONTACTS ;i++)
				{
					//multi-touch-protocol Type B:
					if(mPoints[i].status== 0x04){	//press or up
						if(ps[mPoints[i].id] == 0){ //press
							ps[mPoints[i].id] = 1;
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, mPoints[i].id); // report down
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
							//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, ((reportY = mPoints[i].y-touchY_increment)>=0?reportY:0));
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);

							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger or erase

							recPointStatus[mPoints[i].id].isDown = 1;
						}else{
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);  // report up
							recPointStatus[mPoints[i].id].isDown = 0;
							ps[mPoints[i].id] = 0;
						}
					}else if(mPoints[i].status == 0x07){ //move
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);

						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
						//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, ((reportY = mPoints[i].y-touchY_increment)>=0?reportY:0));
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
						device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);
						recPointStatus[mPoints[i].id].isDown = 1;
						ps[mPoints[i].id] = 1;
					}
				}
				//one point or multi points down or up,only report EV_SYN-SYN_REPORT one time
				device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
				memset(mPoints,0,MAX_CONTACTS*sizeof(TouchPoint));

		    }
			else
			{
				if(buf[gTotalPointNumOffset] > 6) // the first package
				{
					gotValidPointNums = 0;
					totalValidPointNums = buf[gTotalPointNumOffset];
					//LOGI("totalValidPointNums = %d ",totalValidPointNums);
					int pointState = 0;
					// the first package update 0~5 point data
					for(i = 0; i < 6; i++)
					{
						pointState = buf[(10*i)+1] & 0x0F;
						if(pointState != 0)
						{
							mPoints[gotValidPointNums].status = pointState;
							mPoints[gotValidPointNums].id = buf[(10*i)+2];
							mPoints[gotValidPointNums].x = buf[10*i + 4] * 256 + buf[10*i + 3];
							mPoints[gotValidPointNums].y = buf[10*i + 6] * 256 + buf[10*i + 5];

							originPoints[gotValidPointNums].x = mPoints[gotValidPointNums].x;
							originPoints[gotValidPointNums].y = mPoints[gotValidPointNums].y;

							if(calibrateFlag && (gUseDefaultParams || needCalibration || gUseConfigParams))
							{
								mPoints[gotValidPointNums].x = g_X_Cal_Rate * mPoints[gotValidPointNums].x + g_X_Cal_Offset;
								mPoints[gotValidPointNums].y = g_Y_Cal_Rate * mPoints[gotValidPointNums].y + g_Y_Cal_Offset;

								if(mPoints[gotValidPointNums].x >= 32768){
									mPoints[gotValidPointNums].x = 32767;
								}

								if(mPoints[gotValidPointNums].y >= 32768){
									mPoints[gotValidPointNums].y = 32767;
								}
							}
							mPoints[gotValidPointNums].width = buf[10*i + 8] * 256 + buf[10*i + 7];
							mPoints[gotValidPointNums].height = buf[10*i + 10] * 256 + buf[10*i + 9];

							#if 1 // use Touch raw data for pen/finger
							//1: pen, 2: fingler,3:erase
							int toolType = (buf[(10*i)+1] & 0xF0) >> 4;
							if(toolType == 2){
								mPoints[gotValidPointNums].toolType = MT_TOOL_FINGER;
							}else if(toolType == 3){
								mPoints[gotValidPointNums].toolType = MT_TOOL_ERASER;
							}else if(toolType == 1) {
								mPoints[gotValidPointNums].toolType = MT_TOOL_PEN;
							}else{
								mPoints[gotValidPointNums].toolType = MT_TOOL_FINGER;
							}
							#else // use width byte data for pen/finger

							#endif

							#if 0
							LOGI("got pt: (id:%02d)(%05d, %05d),st:%02x, width = %03d, height = %03d, toolType = %d",mPoints[gotValidPointNums].id,
								mPoints[gotValidPointNums].x,mPoints[gotValidPointNums].y,mPoints[gotValidPointNums].status,
								mPoints[gotValidPointNums].width,mPoints[gotValidPointNums].height,mPoints[gotValidPointNums].toolType);
							#endif

							gotValidPointNums++;


						}
					}
				}
				else //buf[gTotalPointNumOffset] == 0, the left package
				{
					int pointState = 0;
					for(i = 0; i < 6; i++)
					{
						pointState = buf[(10*i)+1] & 0x0F;
						if(pointState != 0)
						{
							//record data
							mPoints[gotValidPointNums].status = pointState;
							mPoints[gotValidPointNums].id = buf[(10*i)+2];
							mPoints[gotValidPointNums].x = buf[10*i + 4] * 256 + buf[10*i + 3];
							mPoints[gotValidPointNums].y = buf[10*i + 6] * 256 + buf[10*i + 5];

							originPoints[gotValidPointNums].x = mPoints[gotValidPointNums].x;
							originPoints[gotValidPointNums].y = mPoints[gotValidPointNums].y;

							if(calibrateFlag && (gUseDefaultParams || needCalibration || gUseConfigParams))
							{
								mPoints[gotValidPointNums].x = g_X_Cal_Rate * mPoints[gotValidPointNums].x + g_X_Cal_Offset;
								mPoints[gotValidPointNums].y = g_Y_Cal_Rate * mPoints[gotValidPointNums].y + g_Y_Cal_Offset;

								if(mPoints[gotValidPointNums].x >= 32768){
									mPoints[gotValidPointNums].x = 32767;
								}

								if(mPoints[gotValidPointNums].y >= 32768){
									mPoints[gotValidPointNums].y = 32767;
								}
							}
							mPoints[gotValidPointNums].width = buf[10*i + 8]*256 + buf[10*i + 7];
							mPoints[gotValidPointNums].height = buf[10*i + 10]*256 + buf[10*i + 9];

							#if 1 // use Touch raw data for pen/finger
							//1: pen, 2: fingler,3:erase
							int toolType = (buf[(10*i)+1] & 0xF0) >> 4;
							if(toolType == 2){
								mPoints[gotValidPointNums].toolType = MT_TOOL_FINGER;
							}else if(toolType == 3){
								mPoints[gotValidPointNums].toolType = MT_TOOL_ERASER;
							}else if(toolType == 1) {
								mPoints[gotValidPointNums].toolType = MT_TOOL_PEN;
							}else{
								mPoints[gotValidPointNums].toolType = MT_TOOL_FINGER;
							}
							#else // use width byte data for pen/finger

							#endif

							#if 0
							LOGI("got pt: (id:%02d)(%05d, %05d),st:%02x, width = %03d, height = %03d, toolType = %d",mPoints[gotValidPointNums].id,
								mPoints[gotValidPointNums].x,mPoints[gotValidPointNums].y,mPoints[gotValidPointNums].status,
								mPoints[gotValidPointNums].width,mPoints[gotValidPointNums].height,mPoints[gotValidPointNums].toolType);
							#endif

							gotValidPointNums++;
						}
					}

					if(gotValidPointNums == totalValidPointNums)
					{
						//report touch to Android
						for(i=0;i< gotValidPointNums;i++)
						{
							//multi-touch-protocol Type B:
							if(mPoints[i].status == 0x03 || mPoints[i].status == 0x07){ //hx , fct, touch status: down
								if(mPoints[i].id <= maxAndroidTouchPointId){
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, mPoints[i].id);
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
									//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, ((reportY = mPoints[i].y-touchY_increment)>=0?reportY:0));
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);

									//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger or erase
									if(mPoints[i].toolType == MT_TOOL_ERASER)
									{
										//LOGI("report BTN_TOOL_RUBBER 1");
										device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, 0x141);
										device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 1);
									}
									else
									{
										device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger
									}

									recPointStatus[mPoints[i].id].isDown = 1;
									//LOGI("dn:0x%02X",mPoints[i].id);
								}
								//LOGI("id=%d,toolType=%d",mPoints[i].id,mPoints[i].toolType);
								//device_writeEvent(uinp_fd, EV_KEY, BTN_TOUCH, 1);
								//device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
							}
							else if(mPoints[i].status == 0x02 || mPoints[i].status == 0x04) //hx,fct, touch status: up
							{
								if(mPoints[i].id <= maxAndroidTouchPointId){

									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
									device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);   // report up
									recPointStatus[mPoints[i].id].isDown = 0;
									//LOGI("up:0x%02X",mPoints[i].id);

									if(mPoints[i].toolType == MT_TOOL_ERASER)
									{
										LOGI("report BTN_TOOL_RUBBER 0");
										//device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, 0x141);
										device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 0);
									}
								}
								//device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
							}
						}

						//one point or multi points down or up,only report EV_SYN-SYN_REPORT one time
						device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
						memset(mPoints,0,MAX_CONTACTS*sizeof(TouchPoint));
					}
				}

					#ifdef ENABLE_GET_TIME
					gettimeofday(&tvNow, NULL);
					tms5 = (int64_t)tvNow.tv_sec*1000 + tvNow.tv_usec/1000;
					LOGI("tms5=%lld(%lld)",tms5,tms5-tms3);
					#endif
			}
		}
	}
}


void touchDateProcess_LgInCellMultiPackage(unsigned char * rawDataBuf)
{
    unsigned char *p = NULL;

    if (rawDataBuf[0] != LG_INCELL_TOUCH_PACKAGE_ID)
    {
        LOGI("touchDateProcess_LgInCellMultiPackage package id error !!!!");
        return;
    }

    //touch data for PC, repack serial pack and send to MCU

    int k;
    int ptId;
    int ptStatus;

    //pase and repack touch data to PC
    uart_buf[0]  = 0x1F;
    uart_buf[1]  = 0xF7;
    uart_buf[2]  = 0x43;
    uart_buf[3]  = 0x00;
    uart_buf[4]  = 0x02;//package id
    uart_buf[66] = 0x00;//checksum

    memcpy(&uart_buf[5], &rawDataBuf[1], 61);

    p = &uart_buf[5];

    for(k = 0; k < 6; k++)
    {
        ptId = p[1];
        ptStatus= p[0] & 0x0F;
        pointsToSource[ptId].curStatus  = ptStatus;
        pointsToSource[ptId].id = ptId;

        if (ptId > 0) {
            // point down?
            if(pointsToSource[ptId].curStatus == LG_INCELL_TOUCH_DOWN)
            {
                if(pointsToSource[ptId].preStatus != LG_INCELL_TOUCH_DOWN) // point down
                {
                    if(isPointDownInsideOsdRegion(k, rawDataBuf))
                    {
                        pointsToSource[ptId].disableFlag = 1;
                        p[0] = LG_INCELL_TOUCH_UP;
                        LOGI("first down, set pt[%d].status ===> 0",ptId);
                    }
                }
                else // move
                {
                    if(pointsToSource[ptId].disableFlag == 1)
                    {
                        p[0] = LG_INCELL_TOUCH_UP;
                        //LOGI("move, set pt[%d].status ===> 0", ptId);
                    }
                }
            }
            else // up or invalid status
            {
                if(pointsToSource[ptId].disableFlag == 1)
                {
                    if(pointsToSource[ptId].curStatus == LG_INCELL_TOUCH_UP) //up
                    {
                        p[0] = LG_INCELL_TOUCH_UP;
                    }
                    pointsToSource[ptId].disableFlag = 0;
                }
            }
            if (p[0] == LG_INCELL_TOUCH_DOWN) {
                p[0] = 0x03;
            } else {
                p[0] = 0x02;
            }
            pointsToSource[ptId].preStatus = ptStatus;
        } else {
            p[1] = 0xff;
        }
        p += 10;
    }

    //update checksum
    for(k = 0; k < 66; k++)
    {
        /* point id: 0~n
        if(k%10 == 6 && uart_buf[k] >= 1){
            uart_buf[k] = uart_buf[k] - 1;
        }
        */
        uart_buf[66] += uart_buf[k];
    }

    #ifdef SAVE_PCTOUCHDATA2FILE
    int iRet = sendTouchData(uart_buf, 67); // send data to MCU for PC touch

    if(iRet > 0 && pcTouchDataFd > 0){
        int ret = write(pcTouchDataFd,uart_buf, LG_INCELL_TOUCH_PACKAGE_TO_MCU_LEN);
        if(ret<=0){
            LOGI("write uart_buf to file failed!!!!");
        }
    }
    #else
    sendTouchData(uart_buf, 67); // send data to MCU for PC touch
    #endif

    #ifdef ENABLE_GET_TIME
    gettimeofday(&tvNow, NULL);
    tms3 = (int64_t)tvNow.tv_sec*1000 + tvNow.tv_usec/1000;
    LOGI("tms3=%lld(%lld)",tms3,tms3-tms2);
    #endif
}

void *touchDataProcess_thread(void *arg)
{
	UNUSED(arg);
	int i;
	int len = -1;
	unsigned char buf[DATA_BUF_LEN] = {0};
	//unsigned char usb_buf[DATA_BUF_LEN] = {0};
	memset(pointsToSource,0,sizeof(TouchPointToSource)*MAX_CONTACTS);
	LOGI("touchDataProcess_thread start ....");
	while(1)
	{
		if(g_touchDataFd >= 0)
		{
			len = readTouchRawData(g_touchDataFd, buf, BUF_LEN, READ_TIMEOUT_MAX);
		}
		else  // have not connect Touch Panel after enter system
		{
			while(!detectAndOpenTouchDevice()){
				//LOGI("touch not connected, detect ...");
				#if TOUCH_RESET_BY_POWEROFFON
				if(findTouchDevice()){
					LOGI("found touch, but have no hidraw node of it !!!");
					touchPowerOffOn();
				}
				#endif
				//sleep(3);
				sleep(1);
			}

			#if LIBUSB_SUPPORT
			if(g_touchDevId == TOUCH_DEV_ID_TTOUCH){
				// have not inited libusb
				if(gLibUsbInited == 0){
					initUsbContext();
				}

				// have not opened libusb device
				if(gLibUsbOpened == 0){
					openTTouchUsbToPc();
				}
			}
			#endif
			continue;
		}

		//LOGI("len = %d, readTouchRawData errno = %d (%s)",len, errno,strerror(errno));
		if(len == -1)
		{
			//LOGI("len = %d, readTouchRawData err: %s",len, strerror(errno));
			// I/O error or no such file, touch panel problem,up status maybe not send
			if(errno == ENOENT || errno == EIO || errno == ENODEV)
			{
				LOGI("read touch error !!!  err: %s",strerror(errno));
			    readErrCount++;
				if(readErrCount == 1){
					for(i=0;i<MAX_CONTACTS;i++){
						if(recPointStatus[i].isDown){
							LOGI("read touch data error!!! force release point %d",i);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, i);
						    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);
							recPointStatus[i].isDown = 0;
							syncFlag = 1;
						}
					}
					if(syncFlag == 1){
						device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
						syncFlag = 0;
						//memset(mPoints,0,MAX_CONTACTS*sizeof(TouchPoint));
						memset(recPointStatus,0,MAX_CONTACTS*sizeof(pointStatus));
					}
				}
				//hotplug for Jiage, Huaxin Touch or TTouch
				while(!detectAndOpenTouchDevice()){
					sleep(3);
				}
				#if LIBUSB_SUPPORT
				if(g_touchDevId == TOUCH_DEV_ID_TTOUCH){
					deinitUsbContext();
					initUsbContext();
					openTTouchUsbToPc();
				}
				#endif
			}
			else if(errno == ETIMEDOUT)
			{  //read timeout(1s),means not touch operation
				readErrCount = 0;
				noTouchCount++;
				if(noTouchCount >= 2){
					for(i=0;i<MAX_CONTACTS;i++){
						if(recPointStatus[i].isDown){
							LOGI("no touch, force release point %d",i);
							device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, i);
						    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);
							recPointStatus[i].isDown = 0;
							syncFlag = 1;
						}
					}

					if(syncFlag == 1){
						device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
						syncFlag = 0;
						//memset(mPoints,0,MAX_CONTACTS*sizeof(TouchPoint));
						memset(recPointStatus,0,MAX_CONTACTS*sizeof(pointStatus));
					}
				}
				//LOGI("touchVidPid = %s, disconnected?\n",touchVidPid);
				//FCT TouchD check, is it disconnect ?
				if((strcmp(touchVidPid,IST_TOUCHD_VID_PID) == 0)
					&& g_isHaveTouchDriver
					&& isTouchDisconnect()){
					while(false == detectAndOpenTouchDevice())
					{
						#if TOUCH_RESET_BY_POWEROFFON
						if(findTouchDevice()){
							LOGI("found touch, but have no hidraw node of it !!!");
							touchPowerOffOn();
						}
						#endif
						sleep(2);
					}
				}
			}
			else if(errno == 0)  // read len==-1, errno==0
			{
				//LOGI("touchVidPid = %s disconnect ?\n",touchVidPid);
				//免驱触摸框：富创通F框，嵩立双USB电容触摸，开机时已经连接,后来断开了设备
				if(!g_isHaveTouchDriver)
				{
					//try find device ...
					while(false == detectAndOpenTouchDevice())
					{
						#if TOUCH_RESET_BY_POWEROFFON
						if(findTouchDevice()){
							LOGI("found touch, but have no hidraw node of it !!!");
							touchPowerOffOn();
						}
						#endif
						sleep(2);
					}
				}
			}
			continue; //read data fail or have no data, continue try to read

		}
		else  // len >= 0
		{
			#if 0
			// FCT Touch: not connect touch or no touch, len=0,errno = 0
			if((len == 0) && (g_touchDevId == TOUCH_DEV_ID_IST)){
				if(isTouchConnected(TOUCH_IST_F_VID,TOUCH_IST_F_VID)
					|| isTouchConnected(FCT_VID,FCT_PID)){
					g_isTouchDeviceConnected = 1;
				}else{
					g_isTouchDeviceConnected = 0;
				}

				continue;
			}
			#endif

			readErrCount = 0;
			if(buf[0] == 0xfb)
			{
				noTouchCount = 0;
			}

			if(g_touchDevId == TOUCH_DEV_ID_HUXIIAN)
			{
				//LOGI("len = %d, buf[0] = 0x%02x",len,buf[0]);
				err_no = 0;
			}

			#ifdef SAVE_TOUCHDATA2FILE
				if(len == MD_PACKAGE_LEN && (buf[0]==PACK_ID_HUAXIN || buf[0]==PACK_ID_FCT ||buf[0]==PACK_ID_JIAGE)){ //get touch data
					#ifdef ENABLE_GET_TIME
						gettimeofday(&tvNow, NULL);
						int64_t stamp_ms = (int64_t)tvNow.tv_sec*1000 + tvNow.tv_usec/1000;
						//LOGI("tv_sec = %u, tv_usec = %u",tvNow.tv_sec,tvNow.tv_usec);
						LOGI("stamp_ms=%lld",stamp_ms);
						buf[67] = stamp_ms & 0xff;
						//LOGI("buf[67]=%d",buf[67]);
						buf[66] = (stamp_ms >> 8)& 0xff;
						//LOGI("buf[66]=%d",buf[66]);
						buf[65] = (stamp_ms >> 16)& 0xff;
						//LOGI("buf[65]=%d",buf[65]);
						buf[64] = (stamp_ms >> 24)& 0xff;
						//LOGI("buf[64]=%d",buf[64]);
					#endif

					int wLen = write(dataFd,buf,DATA_BUF_LEN);
					if(wLen < 0){
						LOGI("write touchData file failed!!!, errno = %d,%s",errno,strerror(errno));
					}

				}
			#endif
		}

		if(!isTouchLocked && (sourceTouchState == TOUCH_STATE_ENABLE) && isTouchDataForwardFlag)
		{
			//Source touch for Forwardmode
			if(g_isUart2MCU)
			{
                if (len < LG_INCELL_TOUCH_PACKAGE_LEN)
                {
                    touchDateProcess_LgInCellMultiPackage(buf);
                    continue;
                }
			}

			//if(isTouchDataForwardFlag && sourceTouchState == TOUCH_STATE_ENABLE)
			{
				int totalPointNums = buf[gTotalPointNumOffset];
				//LOGI("totalPointNmus = %d",totalPointNmus);
				if(totalPointNums > 0 && totalPointNums <= 6) // only one package
				{
					gTotalPointNumsInRawData = totalPointNums;
					packageIndex = 0;
					packageNums  = 1;
					filterPointsForSourceTouch(buf,packageIndex,gTotalPointNumsInRawData);
					if(gTotalValidPointsNums > 0)
					{
						//len = sendTouchDataToSource(buf);
						len = sendFilterPointsToSource(filterBuf,gTotalPointNumsInRawData,gTotalValidPointsNums);
						if(len <= 0 )
						{
							LOGI("sendTouchDataToSource  error !!!");
						}

					}
				}
				else if(totalPointNums > 6) // muiltiple package, and this is the first package
				{
					gTotalPointNumsInRawData = totalPointNums;
					packageIndex = 0;
					filterPointsForSourceTouch(buf,packageIndex,gTotalPointNumsInRawData);
					packageNums = (totalPointNums - 1)/gMaxPointNumOnePackage + 1;
				}
				else // 0, not the first package
				{
					packageIndex++;
					filterPointsForSourceTouch(buf,packageIndex,gTotalValidPointsNums);
					if((packageIndex == packageNums-1) && (gTotalValidPointsNums > 0))
					{
						len = sendFilterPointsToSource(filterBuf,gTotalPointNumsInRawData,gTotalValidPointsNums);
						if(len <= 0 )
						{
							LOGI("sendTouchDataToSource  error !!!");
						}
					}
				}
			}
		}

		//Android touch
		if(!reportAndroidTouchInkernel
		    &&androidTouchState == TOUCH_STATE_ENABLE
			&& (!g_isUseMultitouchDriverReport)){
			parseAndReportToAndroid(buf);
		}
	}
	return (void *)0;
}

//hidraw device maybe changed on Huaxin and IST touch
void * findAndOpenHidRawDeviceThread(void *arg)
{
	UNUSED(arg);
	LOGI("findAndOpenHidRawDeviceThread:  g_TpHidRawFd = %d",g_TpHidRawFd);
	while(g_touchCtrlFd < 0){
		detectAndOpenTouchDevice();
		sleep(1);
	}
	LOGI("exit reOpenHidRawDeviceThread.");
	return (void *)0;
}

int EnableRawData(int fd)
{
    if (fd > 0)
    {
        int i = 0;
        unsigned char cmd[64] = {0xfc,0x30,0x01,0x01,0x00,0x01};
        unsigned char buf[64] = {0};
        write(fd,cmd,64);
        for (i=0;i<100;i++)
        {
            usleep(1000);
            memset(buf,0,64);
            read(fd,buf,64);
            if (buf[0] == 0xfc && buf[1] == 0xfe && buf[2] == 0x81 && buf[3] == 0x02 && buf[5] == 0x30 && buf[6] == 0x01)
            {
                LOGD("TouchProcess  EnableRawData=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
                return 1;
            }
        }
    }
    return 0;
}

int EnableRawDataWidth(int fd)
{
    if (fd > 0)
    {
        int i = 0;
        unsigned char cmd[64] = {0xfc,0x30,0x06,0x00,0x00,0x01};
        unsigned char buf[64] = {0};
        write(fd,cmd,64);

        for (i=0;i<100;i++)
        {
            usleep(1000);
            memset(buf,0,64);
            read(fd,buf,64);
            if (buf[0] == 0xfc && buf[1] == 0xfe && buf[2] == 0x81 && buf[3] == 0x02 && buf[5] == 0x30 && buf[6] == 0x06)
            {
                LOGD("TouchProcess  EnableRawDataWidth=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
                return 1;
            }
        }
    }
    return 0;
}

int ResetUSB(int fd)
{
    if (fd > 0)
    {
        unsigned char cmd[64] = {0xfc,0xa8,0x00,0x00,0x00,0x00};
        unsigned char buf[64] = {0};
        write(fd,cmd,64);
        LOGD("TouchProcess  ResetUSB\r\n");
        return 1;
    }
    return 0;
}

int GetFWVersion(int fd)
{
    if (fd > 0)
    {
        int i = 0;
        unsigned char cmd[64] = {0xfc,0xa3,0x00,0x00,0x00,0x00};
        unsigned char buf[64] = {0};
        write(fd,cmd,64);
        for (i=0;i<100;i++)
        {
            usleep(1000);
            memset(buf,0,64);
            read(fd,buf,64);
            if (buf[0] == 0xfc && buf[1] == 0xa3 && buf[2] == 0x80)
            {
                LOGI("TouchProcess  GetFWVersion=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
                return 1;
            }
        }
    }
	LOGI("GetFWVersion failed !!!!");
    return 0;
}


// get Jiage Touch device firmware info
char* zaag_drvinfo(int fd)
{
	int ret;
	unsigned char pack1[] = { 0x0c,0x61,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	unsigned char pack2[] = { 0x0c,0x61,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	unsigned char buf[64];
	unsigned char devinfo[128] = {0};
	static char drvinfo[32];

	drvinfo[0] = '\0';

	LOGI("zaag_drvinfo: fd=%d",fd);
	LOGI("write pack1...");

	//write_pack1
	ret = write(fd, pack1, 0x0c);
	if (ret <= 0){
		LOGI("write pack1 failed!!!!!  errno=%d",errno);
		return drvinfo;
	}

	LOGI("read pack1...");
	//read_pack1
	//ret = read(fd, buf, 64);
	ret = timeoutRead(fd, buf, 64,1);
	if (ret <= 0){
		LOGI("read pack1  failed!!!!!  timeout,errno=%d",errno);
		return drvinfo;
	}
	memcpy(devinfo, buf+6, 58);

	LOGI("write pack2...");

	//write_pack2
	ret = write(fd, pack2, 0x0c);
	if (ret <= 0){
		LOGI("write pack2 failed!!!!!  errno=%d",errno);
		return drvinfo;
	}

	LOGI("read pack2...");
	//read_pack2
	//ret = read(fd, buf, 64);
	ret = timeoutRead(fd, buf, 64,1); //time out: 1s
	if (ret <= 0){
		LOGI("read pack2 failed!!!!!  errno=%d",errno);
		return drvinfo;
	}
	memcpy(devinfo+58, buf+6, 58);

	unsigned int zbc_ver = *(unsigned int*)(&devinfo[42]);
	unsigned int zts_ver = *(unsigned int*)(&devinfo[58]);
	sprintf(drvinfo, "%02d.%02d.%02d.%02d",
		zbc_ver/100%100,
		zbc_ver%100,
		zts_ver/100%100,
		zts_ver%100);

	gGotZaagPanelInfoOK = true;

	return drvinfo;
}

void getJagePanelVersionInfo()
{
	char jiageTouchVersion[128]={0};
	char * ptDrvinfo = NULL;

	LOGI("getJagePanelVersionInfo...");

	if(gTouchFirmFd>0){
		ptDrvinfo = zaag_drvinfo(gTouchFirmFd);
	}

	if(gGotZaagPanelInfoOK){
		LOGI("gGotZaagPanelInfoOK = %d",gGotZaagPanelInfoOK);
		memcpy(jiageTouchVersion,ptDrvinfo,32);
		LOGI("jiageTouchVersion=%s",jiageTouchVersion);
		property_set("persist.vendor.xbh.touchpanel.version",jiageTouchVersion);
	}

}


void InitProcess(const char* path, int baudrate)
{
    int ret;
    bool bRet = false;
    LOGI("InitProcess   2021.12.01");
    g_isUseMultitouchDriverReport = isUseMultiTouchDriverReport();
    gTouchType = initTouchType();
    LOGI("g_isUseMultitouchDriverReport = %d, gTouchType = %d",g_isUseMultitouchDriverReport,gTouchType);
    if(gTouchType == 1){
        LOGI("=====> USB + MCU <=====");
        openSerialPort(path, baudrate);
        LOGD("openSerialPort(%s, %d)", path, baudrate);
    }

	detectAndOpenTouchDevice();

	//memset(pr,0,sizeof(pr));
	memset(maskzone,0,sizeof(maskzone));

	createTouchScreen();
    pthread_attr_t process_attr;
    pthread_attr_init(&process_attr);
    pthread_attr_setdetachstate(&process_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&sthread, &process_attr, touchDataProcess_thread, (void *)NULL) != 0)
    {
        LOGI("Failed to create TouchProcess thread!!!!!!!");
        return ;
    }
	property_set("vendor.xbh.touchprocess.running","true");
}

int isTouchPanelOk()
{
	int ret = 0; // not ok
	ret = g_isTouchOpened;
	LOGI("isTouchPanelOk eywa ret = %d",ret);
	return ret;
}

int setScreenOffsetHV(int offsetX,int offsetY,int is4kUi){
	int ret = -1;
	float tpOffsetX = 0.0f;
	float tpOffsetY = 0.0f;
	float maxWidth;
	float maxHeight;

	if(is4kUi){
	    maxWidth  = MAX_DISPLAY_UHD_WIDTH;
	    maxHeight = MAX_DISPLAY_UHD_HEIGHT;
	}else{
	    maxWidth  = MAX_DISPLAY_FHD_WIDTH;
	    maxHeight = MAX_DISPLAY_FHD_HEIGHT;
	}

	LOGI("setScreenOffsetHV: offsetX = %d, offsetY = %d",offsetX,offsetY);
	if(offsetX < 0 || offsetY < 0 || offsetX > maxWidth || offsetY > maxHeight){
		LOGI("setScreenOffsetHV: param is invalid");
		return -1;
	}

	if(isTouchDataForwardFlag){
		gOffsetX = (int)(offsetX*TP_MAX_XC/maxWidth);
		gOffsetY = (int)(offsetY*TP_MAX_YC/maxHeight);
		ret = 0;
		LOGI("setScreenOffsetHV: gOffsetX = %d, gOffsetY = %d",gOffsetX,gOffsetY);
	}else{
		LOGI("g_isHhtSdk = %d", g_isHhtSdk);

		if(g_isHhtSdk) {
			int x_scale  = 1;
			int x_offset = 0;
			int y_scale  = 1;
			if(offsetY == 0) {
				ret = g_hht_device->setTransform(Channel::Output, 1, 0, 1, 0);
				LOGI("setScreenOffsetHV: reset touch offset");
			} else {
				int yOffsetValue = (int)((-1)*offsetY*TP_MAX_YC/maxHeight);
				ret = g_hht_device->setTransform(Channel::Output, x_scale, x_offset, y_scale, yOffsetValue);
				LOGI("setScreenOffsetHV: yOffsetValue = %d",yOffsetValue);
			}
		} else {
			int yOffsetValue = 0;
			int checkSum = 0;
			int i = 0;
			if(offsetY == 0){
				CMD_SET_SOURCE_TOUCH_OFFSET[9] = (char)0;
				LOGI("setScreenOffsetHV: reset touch offset");
			}else{
				yOffsetValue =  maxHeight/offsetY;
				CMD_SET_SOURCE_TOUCH_OFFSET[9] = (char)((yOffsetValue-1) & 0xFF);
				LOGI("setScreenOffsetHV: yOffsetValue = %d",yOffsetValue);
			}
			for(i=4;i<10;i++){
				checkSum = checkSum + CMD_SET_SOURCE_TOUCH_OFFSET[i];
			}
			CMD_SET_SOURCE_TOUCH_OFFSET[10] = (char)(checkSum & 0xFF);
			bool bRet = sendCmdToTouchByUsb(g_touchCtrlFd, CMD_SET_SOURCE_TOUCH_OFFSET);
			if(bRet){
				ret = 0;
			}else{
				LOGI("send cmd for touch offset failed!!!");
			}
		}
	}
	return ret;
}


int setTouchState(int type, int state)
{
	LOGI("setTouchState: type = %d, state = %d",type,state);

	if(type < SOURCE_TOUCH || type > ANDROID_TOUCH || state < TOUCH_STATE_DISABLE || state > TOUCH_STATE_ENABLE){
		LOGI("setTouchState: param is invalid !!!!");
		return -1;
	}

	if(type == SOURCE_TOUCH){
		if(state == TOUCH_STATE_DISABLE){
			if(isTouchDataForwardFlag){
				sourceTouchState = TOUCH_STATE_DISABLE;
			}else{
				if(g_isHhtSdk) {
					if(g_hht_device->setChannel(Channel::Output, false) != 0) {
						LOGI("CMD_STOP_USB_TOUCH_FOR_SOURCE failed !!!!");
					}
				} else {
					if(sendCmdToTouchByUsb(g_touchCtrlFd, CMD_STOP_USB_TOUCH_FOR_SOURCE)){
						sourceTouchState = TOUCH_STATE_DISABLE;
					}else{
						LOGI("CMD_STOP_USB_TOUCH_FOR_SOURCE failed !!!!");
					}
				}
			}
		}else{
			if(isTouchDataForwardFlag){
				sourceTouchState = TOUCH_STATE_ENABLE;
			}else{
				if(g_isHhtSdk) {
					//直发模式sourceTouchState设置为false
					if(g_hht_device->setChannel(Channel::Output, true) != 0){
						LOGI("CMD_START_USB_TOUCH_FOR_SOURCE failed !!!!");
					}
				} else {
					if(sendCmdToTouchByUsb(g_touchCtrlFd, CMD_START_USB_TOUCH_FOR_SOURCE)){
						sourceTouchState = TOUCH_STATE_ENABLE;
					}else{
						LOGI("CMD_START_USB_TOUCH_FOR_SOURCE failed !!!!");
					}
				}
			}
		}
	}else if(type == ANDROID_TOUCH){
		if(state == TOUCH_STATE_DISABLE){
			androidTouchState = TOUCH_STATE_DISABLE;
		}else{
			androidTouchState = TOUCH_STATE_ENABLE;
		}
	}
	return 0;
}

int getTouchState(int type)
{
	int ret = TOUCH_STATE_ENABLE;
	//LOGI("getTouchState: type = %d",type);

	if(type < 0 || type > 1){
		LOGI("getTouchState: param is invalid !!!!");
		return -1;
	}

	if(type == SOURCE_TOUCH){
		ret = sourceTouchState;
	}else if(type == ANDROID_TOUCH){
		ret = androidTouchState;
	}

	//LOGI("getTouchState: ret = %d",ret);

	return ret;
}

int setNonThroughTouchRegion(int id, int x, int y, int w, int h,int is4kUi)
{
	int x2 = 0;
	int y2 = 0;
	int ret = 0;
	float leftTopX = 0.0f;
	float leftTopY = 0.0f;
	float rightBottomX = 0.0f;
	float rightBottomY = 0.0f;

	int ltX = 0;
	int ltY = 0;
	int rbX = 0;
	int rbY = 0;
    LOGI("");
	LOGI("set zone id:%d, (%5d, %5d,  %5d, %5d)",id,x,y,w,h);

	if(id < NO_SUPPORT_ID || id > MAX_PENETRATION_REGION ){
		LOGI("set retgion: id must between 0~%d  !!!!",MAX_PENETRATION_REGION);
		return -1;
	}

	if(isTouchDataForwardFlag){
    	if(id > -1){
    		maskzone[id].x1 = lcdPosX2TpPosX(x,is4kUi);
    		maskzone[id].y1 = lcdPosY2TpPosY(y,is4kUi);

    		x2 = x + w;
    		y2 = y + h;

    		maskzone[id].x2 = lcdPosX2TpPosX(x2,is4kUi);
    		maskzone[id].y2 = lcdPosY2TpPosY(y2,is4kUi);

    		maskzone[id].used = 1;
    		LOGI("set      id:%d, (%5d, %5d)~(%5d, %5d)",id,maskzone[id].x1,maskzone[id].y1,maskzone[id].x2,maskzone[id].y2);
		}
		else{
    		leftTopX = lcdPosX2TpPosX(x,is4kUi);
    		leftTopY = lcdPosY2TpPosY(y,is4kUi);

    		//leftTopX = x;
    		//leftTopY = y;

    		x2 = x + w;
    		y2 = y + h;

    		rightBottomX = lcdPosX2TpPosX(x2,is4kUi);
    		rightBottomY = lcdPosY2TpPosY(y2,is4kUi);

    		//rightBottomX = x2;
    		//rightBottomY = y2;

    		rightBottomX = rightBottomX + 0.5f;
    		rightBottomY = rightBottomY + 0.5f;

    		ltX = (int)leftTopX;
    		ltY = (int)leftTopY;
    		rbX = ((int)rightBottomX>32767)?32767:((int)rightBottomX);
    		rbY = ((int)rightBottomY>32767)?32767:((int)rightBottomY);
    		LOGI("rm              (%5d, %5d)~(%5d, %5d)",ltX,ltY,rbX,rbY);

    		addMaskZone(ltX, ltY, rbX, rbY);
		}
	}else{ // 电容双USB
		if(id > -1) {
			bool bRet = false;
			int tpLeftTopX = (int)lcdPosX2TpPosX(x,is4kUi);
			int tpLeftTopY = (int)lcdPosY2TpPosY(y,is4kUi);

			int tpWidth  = (int)lcdPosX2TpPosX(w,is4kUi);
			int tpHeight = (int)lcdPosY2TpPosY(h,is4kUi);

			int tpRightBottomX = (int)tpLeftTopX + tpWidth;
			int tpRightBottomY = (int)tpLeftTopY + tpHeight;

			//LOGI("set tp x:%5d y:%5d w:%5d h:%5d , is4kUi = %d. (%5d,%5d)~(%5d,%5d)",tpLeftTopX,tpLeftTopY,tpWidth,tpHeight,is4kUi,tpLeftTopX,
			//	tpLeftTopY,tpLeftTopX+tpWidth,tpLeftTopY+tpHeight);

			if(g_isHhtSdk) {
				ret = g_hht_device->setMaskZone(id, tpLeftTopX, tpLeftTopY, tpRightBottomX, tpRightBottomY);
				if(ret == -1){
					LOGI("set zone, failed !!!!! ret = %d",ret);
				}
			} else {
				bRet = sendTouchZonePackage(id,tpLeftTopX,tpLeftTopY,tpWidth,tpHeight,1);
				if(!bRet){
					ret = -1;
					LOGI("set zone, failed !!!!! ret = %d",ret);
				} else {
					if(tpLeftTopX != maskzone[id].x1 || tpLeftTopY != maskzone[id].y1 ||
						tpWidth != maskzone[id].w || tpHeight != maskzone[id].h) {
						bool bRet = false;

						// New area, no need to delete the old area
						if(maskzone[id].used == 0) {
							maskzone[id].x1 = (int)lcdPosX2TpPosX(x,is4kUi);
							maskzone[id].y1 = (int)lcdPosY2TpPosY(y,is4kUi);

							x2 = x + w;
							y2 = y + h;

							maskzone[id].x2 = (int)lcdPosX2TpPosX(x2,is4kUi);
							maskzone[id].y2 = (int)lcdPosY2TpPosY(y2,is4kUi);

							maskzone[id].w = (int)lcdPosX2TpPosX(w,is4kUi);
							maskzone[id].h = (int)lcdPosY2TpPosY(h,is4kUi);

							maskzone[id].used = 1;

							return ret;
						}

						int rmWidth  = maskzone[id].w;
						int rmHeight = maskzone[id].h;

						//LOGI("rm  tp x:%5d y:%5d w:%5d h:%5d , is4kUi = %d. (%5d,%5d)~(%5d,%5d)",tpLeftTopX,tpLeftTopY,tpWidth,tpHeight,is4kUi,tpLeftTopX,
						//	tpLeftTopY,tpLeftTopX+tpWidth,tpLeftTopY+tpHeight);

						// LOGI("setNonThroughTouchRegion: rm zone:  (%5d, %5d)~(%5d, %5d)",maskzone[id].x1,maskzone[id].y1,maskzone[id].x2,maskzone[id].y2);
						bRet = sendTouchZonePackage(id,maskzone[id].x1,maskzone[id].y1,rmWidth,rmHeight,0);
						if(!bRet) {
							ret = -1;
							LOGI("setNonThroughTouchRegion: rm zone:  (%5d, %5d)~(%5d, %5d)  failed !!!!",maskzone[id].x1,maskzone[id].y1,rmWidth,rmHeight);
						} else {
							maskzone[id].x1 = (int)lcdPosX2TpPosX(x,is4kUi);
							maskzone[id].y1 = (int)lcdPosY2TpPosY(y,is4kUi);

							x2 = x + w;
							y2 = y + h;

							maskzone[id].x2 = (int)lcdPosX2TpPosX(x2,is4kUi);
							maskzone[id].y2 = (int)lcdPosY2TpPosY(y2,is4kUi);

							maskzone[id].w = (int)lcdPosX2TpPosX(w,is4kUi);
							maskzone[id].h = (int)lcdPosY2TpPosY(h,is4kUi);

							maskzone[id].used = 1;
						}
					}
				}
			}

		}
	}
	//LOGI("set retgion: ret = %d",ret);
	return ret;
}

int removeNonThroughTouchRegion(int id,int x, int y, int w, int h,int is4kUi)
{
	int x2 = 0;
	int y2 = 0;
	int ret = 0;

	float leftTopX = 0.0f;
	float leftTopY = 0.0f;
	float rightBottomX = 0.0f;
	float rightBottomY = 0.0f;

	int ltX = 0;
	int ltY = 0;
	int rbX = 0;
	int rbY = 0;

    LOGI("");
	LOGI("rm  zone id:%d, (%5d, %5d,  %5d, %5d)",id,x,y,w,h);

	if(id < NO_SUPPORT_ID || id > MAX_PENETRATION_REGION){
		LOGI("remove region failed!!!  id must between 0~%d",MAX_PENETRATION_REGION);
		return -1;
	}

	if(isTouchDataForwardFlag){
		if(id > NO_SUPPORT_ID){
    		LOGI("rm       id:%d, (%5d, %5d)~(%5d, %5d)",id,maskzone[id].x1,maskzone[id].y1,maskzone[id].x2,maskzone[id].y2);
    		maskzone[id].used = 0;
    		maskzone[id].x1 = 0;
    		maskzone[id].y1 = 0;
    		maskzone[id].x2 = 0;
    		maskzone[id].y2 = 0;
		}
		else{
    		leftTopX = lcdPosX2TpPosX(x,is4kUi);
    		leftTopY = lcdPosY2TpPosY(y,is4kUi);

    		//leftTopX = x;
    		//leftTopY = y;

    		x2 = x + w;
    		y2 = y + h;

    		rightBottomX = lcdPosX2TpPosX(x2,is4kUi);
    		rightBottomY = lcdPosY2TpPosY(y2,is4kUi);

    		//rightBottomX = x2;
    		//rightBottomY = y2;

    		rightBottomX = rightBottomX + 0.5f;
    		rightBottomY = rightBottomY + 0.5f;

    		ltX = (int)leftTopX;
    		ltY = (int)leftTopY;
    		rbX = ((int)rightBottomX>32767)?32767:((int)rightBottomX);
    		rbY = ((int)rightBottomY>32767)?32767:((int)rightBottomY);
    		LOGI("rm                (%5d, %5d)~(%5d, %5d)",ltX,ltY,rbX,rbY);

    		deleteMaskZone(ltX, ltY, rbX, rbY);
		}

	}else{ // 电容双USB
		bool bRet = false;

		int tpLeftTopX = (int)lcdPosX2TpPosX(x,is4kUi);
    	int tpLeftTopY = (int)lcdPosY2TpPosY(y,is4kUi);

    	int tpWidth  = (int)lcdPosX2TpPosX(w,is4kUi);
    	int tpHeight = (int)lcdPosY2TpPosY(h,is4kUi);

		maskzone[id].used = 0;
		maskzone[id].x1 = 0;
		maskzone[id].y1 = 0;
		maskzone[id].x2 = 0;
		maskzone[id].y2 = 0;
		maskzone[id].w = 0;
		maskzone[id].h = 0;

		//LOGI("rm  tp x:%5d y:%5d w:%5d h:%5d , is4kUi = %d. (%5d,%5d)~(%5d,%5d)",tpLeftTopX,tpLeftTopY,tpWidth,tpHeight,is4kUi,tpLeftTopX,
		//	tpLeftTopY,tpLeftTopX+tpWidth,tpLeftTopY+tpHeight);

		if(g_isHhtSdk) {
			ret = g_hht_device->unsetMaskZone(id);
			if(ret != 0) {
				LOGI("rm zone: id=%d (%5d, %5d)~(%5d, %5d)  failed !!!!",id, tpLeftTopX,tpLeftTopY,tpWidth,tpHeight);
			}
		} else {
			bRet = sendTouchZonePackage(id,tpLeftTopX,tpLeftTopY,tpWidth,tpHeight,0);
			if(!bRet){
				ret = -1;
				LOGI("rm zone:  (%5d, %5d)~(%5d, %5d)  failed !!!!",tpLeftTopX,tpLeftTopY,tpWidth,tpHeight);
			}
		}
	}

	return ret;
}

int removeNonThroughTouchRegionById(int id)
{
	int x2 = 0;
	int y2 = 0;
	int ret = 0;

	float leftTopX = 0.0f;
	float leftTopY = 0.0f;
	float rightBottomX = 0.0f;
	float rightBottomY = 0.0f;

	int ltX = 0;
	int ltY = 0;
	int rbX = 0;
	int rbY = 0;

	LOGI("del retgion: id: %d",id);
	if(id < NO_SUPPORT_ID || id > MAX_PENETRATION_REGION){
		LOGI("remove region failed!!!  id must between 0~%d",MAX_PENETRATION_REGION);
		return -1;
	}

	if(isTouchDataForwardFlag){
		maskzone[id].used = 0;
		maskzone[id].x1 = 0;
		maskzone[id].y1 = 0;
		maskzone[id].x2 = 0;
		maskzone[id].y2 = 0;
		//LOGI("id:%d, (%5d, %5d),(%5d, %5d)",id,maskzone[id].x1,maskzone[id].y1,maskzone[id].x2,maskzone[id].y2);

	}else{ // 电容双USB
	    /* Todo
		no support
		*/
	}
	//LOGI("del retgion: %d,  ret = %d",id,ret);
	return ret;
}


int removeAllNonThroughTouchRegion()
{
	LOGI("remove all touch region");
	memset(maskzone,0,sizeof(maskzone));
	return 0;
}

/*
* 设置手掌触控类型
*  type:
*       0 - 板擦，
*       1 - 误触（不上报此手掌触摸）
*/
int setPalmType(int type)
{
    bool bRet = false;
    LOGI("setPalmType: %d",type);

    if(isTouchDataForwardFlag){
        if(type == 1){
            bRet = sendCmdToTouchByUsb(g_touchCtrlFd,CMD_SET_PALM_ENABLE);
        }else if(type == 0){
            bRet = sendCmdToTouchByUsb(g_touchCtrlFd,CMD_SET_PALM_DISABLE);
        }
    } else {
		if(g_isHhtSdk) {
			if(type == 1) {
				if(g_hht_device->setPalmRejection(Channel::System | Channel::Output, true) == 0){
					bRet = true;
				}
			} else if(type == 0) {
				if(g_hht_device->setPalmRejection(Channel::System | Channel::Output, false) == 0){
					bRet = true;
				}
			}
		} else {
			if(type == 1) {
				bRet = sendCmdToTouchByUsb(g_touchCtrlFd, CMD_SPI_USB_PALM_FUNCTION_REJECTION);
			} else if(type == 0) {
				bRet = sendCmdToTouchByUsb(g_touchCtrlFd, CMD_SPI_USB_PALM_FUNCTION_ERASER);
			}
		}
    }

    if(bRet){
        return 0;
    }else{
        LOGI("setPalmType: bRet = %d,  errno = %d",bRet,errno);
        return -1;
    }

}

