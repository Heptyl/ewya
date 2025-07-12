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
#include <linux/uinput.h>
//#include <cutils/properties.h>
#include "cutils/properties.h"


#include "string.h"
#include <cstring>
#include <dirent.h>


//#include "IniFile.h"

//#include "iniparser.h"

#define LIBUSB_SUPPORT 0

#if LIBUSB_SUPPORT
#include "libusb.h"
#endif

#include <utils/Log.h>
#include "TouchProcess.h"
//===========================
//for debug
#define LOG_NDEBUG 0
#define LOG_TAG "xbh_mw@libtimelinkh3touch"

#define DEBUG_DATA   0
#define DEBUG_CACHE  0
#define DEBUG_REPORT 0
#define DEBUG_TOUCHBACK 0

//===========================


//==================================
//touch raw data
//package head

//point status value
#define POINT_STATUS_INVALID  0x00

//package struct
#define PACK_ID_OFFSET				0
#define POINT_DATA_START_OFFSET		1
#define VALID_POINT_NUM_OFFSET		61

//point data struct
#define POINT_STATUS_OFFSET	0  //offset in point data
#define POINT_ID_OFFSET		1

#define TEN_BYTES  10
#define BUF_LEN 64

#define MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH  50
#define MAX_POINT_NUMS_SUPPORT_FOR_ANDROID      50
#define MAX(x,y)  ((x)<(y)?(y):(x))


//=================================

//touch hardware id

#define TOUCH_APLE_VID  0x0d48
#define TOUCH_APLE_PID  0x001d



#define MT_TOOL_FINGER		0
#define MT_TOOL_PEN			1
#define MT_TOOL_ERASER		2
#define MT_TOOL_NONE		3
#define MT_TOOL_TYPE_MAX	MT_TOOL_NONE

#define TIMEOUT_READ_TOUCH_RAWDATA_USECOND   20*1000
#define TIMEOUT_FLUSH_LAST_DATA_USECOND      10*1000

#define HID_GADGET_F_HID_RK_NODE            "/dev/hidg_rk0"
#define HID_GADGET_REPORT_DESC              "/config/usb_gadget/g1/functions/hid_rk.gs11/report_desc"
#define HID_GADGET_REPROT_FEATURE_LENGTH    "/config/usb_gadget/g1/functions/hid_rk.gs11/report_feature_length"
#define HID_GADGET_REPORT_FEATURE           "/config/usb_gadget/g1/functions/hid_rk.gs11/report_feature"
#define REPORT_DESC_BUF_SIZE 4096

#define PROP_TOUCHBACK_REPORTDESC_LEN  "vendor.xbh.touchback_reportdesc.len"
#define PROP_TOUCHBACK_DRIVER_NODE_FD  "vendor.xbh.touchback.fd"
#define PROP_TOUCHSCREEN_HIDRAW_NODE   "vendor.xbh.touch.rawdata_node"
#define PROP_TOUCHSCREEN_VID_PID       "vendor.xbh.touch.vidpid"
#define PROP_TOUCHSCREEN_HIDRAW_DATA_FD  "vendor.xbh.touch.rawdata_fd"
#define PROP_TOUCHPROCESS_VERSION      "vendor.xbh.touchprocess.ver"




#define TOUCH_STATE_DISABLE 0
#define TOUCH_STATE_ENABLE  1

bool gSourceTouchEnabled = TOUCH_STATE_DISABLE;

unsigned char * g_pReportDesc = NULL;
int g_report_desc_size = 0;
int gSetTouchScreenReportDescFlag = 0;
int eagainFlag = 0;
int g_cacheResetHid = 0;
int panelNameLength = 27;
bool g_getStatusUp = false;

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

int g_touchDevId = 0;

int g_pctouchDevId = -1;
#define TOUCH_REPORT_IN_KERNEL_DISABLE_CONTROL_PATH "/sys/class/touch/hidinput/report_disable"

//touch config toolbar
#define TOUCH_CONFIG_FILE "/system/etc/touch_config.ini"


int TOOLBAR_TYPE = -1;
bool checkEraseStatus = false;
bool checkPenStatus = false;

int maxAndroidTouchPointNum = 20; // default 20
int startPointId = 1;
int maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum - 1;


bool reportAndroidTouchInKernel = true;

int dateLen = 10;
int slotLen = 6;
int haveFingerNum = 0;
int indexNum = 0;
int startPointPenId = 0;
int gCacheFd = 0;

struct CurrPoint{
    float x;
    float y;
};
struct CurrPoint mCurrPoint,CalibrPoint[4];

struct TouchPoint{
    unsigned char status;
    unsigned char id;
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
    unsigned int toolType;
};

#define MAX_CONTACTS 	MAX(MAX_POINT_NUMS_SUPPORT_FOR_SOURCETOUCH,MAX_POINT_NUMS_SUPPORT_FOR_ANDROID)
#define BYTES_NUM_ONE_POINT 10
struct TouchPoint mPoints[MAX_CONTACTS] ;
struct TouchPoint originPoints[MAX_CONTACTS] ; // come from Touch device

struct CacheTouchData{
    unsigned char points[MAX_CONTACTS*BYTES_NUM_ONE_POINT] = { 0xFF };
    int mapPenId[4];
    int penIsDown[5];    //1:down,0:up, max 4 pens and 1 eraser:penIsDown[0],penIsDown[1],penIsDown[2],penIsDown[3],penIsDown[4] for eraser
    bool haveFingerDown;
} mCacheTouchData;

struct CacheFingerTouchStatus {
    unsigned char id;
    bool isDown[MAX_CONTACTS] = { false };
} cacheFingerTouchStatus;

static bool gAreAllPointsUp = false;

struct pointStatus{
    unsigned char isDown;  // 0: up, 1: down
};
struct pointStatus recPointStatus[MAX_CONTACTS];
unsigned int syncFlag = 0;

static char ps[MAX_CONTACTS] = {0};  //press status:0:up,1:down

char mCacheData[2] = {0};


struct pointIdStatus{
    unsigned char id;
    unsigned char statusValue;
    unsigned char isDown;  // 0: up, 1: down
};


pthread_t sthread;
pthread_t sthreadHid;


#define MAX_PATH_LEN  128
#define MAX_MINOR       16

#define TP_MAX_XC 32767
#define TP_MAX_YC 32767


#define UNUSED(x) (void)x


static int g_isTouchConnected = 0;
static bool haveInsmodTouchDriver = false;
static struct input_event event;
static int uinp_fd = 0;


//==========================
//global vars
int totalValidPointNums = 0;
int gotValidPointNums = 0;
int g_touchDataFd = -1;
int g_isTouchOpened = 0;
int gMaxPointNumOnePackage = 6; //default is 6
int gMaxPointNumOnePackageForTouchButton = 6; //default is 6
int gMaxPackageNumsForSourceTouch = 4; // default max is 24 points
int gBytesNumOfOnePoint = 10;
int gFingerPackageLen = 62;
int gPenPackageLen = 13;
int gPenReport1 = 0x10;
int gPenReport2 = 0x11;
int gTouchButtonPointNumsOffset = 61;
int gBufErrorStatus = 0x50;

int gReportId = -1;
int gFingerReportId = 0x02;
int gFingerAP9ReportId = 0x01;
int gFingerAP10NewReportId = 0x03;
int gPenReportIdMin = 0x10;
int gPenReportIdMax = 0x13;
int gEraserReportId = 0x14;
int gPenNumMax = 2; //2 pen
int gSmallPen = 0x00;
int gMiddlePen = 0x01;

int gPointStatusInvalid = 0x00;
int gFingerInactive  = 0x00;
int gFingerDown      = 0x07;
int gFingerMove      = 0x07;
int gFingerUp        = 0x04;
int gPenDown         = 0x02;
int gPenMove         = 0x03;
int gPenUp           = 0x00;
int gEraserDown      = 0x06;
int gEraserMove      = 0x0e;
int gEraserUp        = 0x00;
int gApPenDown       = 0x20;
int gAPEraseDown     = 0x06;
int gAPEraseMove     = 0x0e;
int gAPEraseUp       = 0x00;
int gAPSmallPen      = 0x00;
int gAPErasePen      = 0x02;

int gPointStateMove    = -1;
int gMaxPointsForAndroid = 10;
int gTotalPointNum = 0;
int gPointDataStartOffset = 1;
int gPointStatusOffset = 0;
int g_isTouchDeviceConnected = 0;
bool g_isHaveTouchDriver = false;
int gValidPointNumPos = 61;
char touchVidPid[10] = {'0','0','0','0',':','0','0','0','0','\0'};
char rawDataNode[16] = {0};
long connectCount = 0;
bool isAP10TouchCheck = false;
bool isAP10NewAgreement = false;
int gCacheHidTime = 5;

#if DEBUG_TOUCHBACK
long long gRdLen = 0;
long long gWrLen = 0;
#endif

#if 0
static int getReportDescriptor(const int fd)
{
    if(fd < 0)
    {
        ALOGE("getReportDescriptor  Invalid input parameter !!!");
        return 0;
    }

    // get size of the report descriptor
    struct hidraw_report_descriptor rpt_desc;
    if (ioctl(fd, HIDIOCGRDESCSIZE, &rpt_desc.size) < 0)
    {
        ALOGE("getReportDescriptor: Failed to get report descriptor size, error:%s",strerror(errno));
        return 0;
    }

    // get the report descriptor
    if (ioctl(fd, HIDIOCGRDESC, &rpt_desc) < 0)
    {
        ALOGE("getReportDescriptor: Failed to get report descriptor, error:%s",strerror(errno));
        return 0;
    }

    #if DEBUG_DATA
    for (int i = 0; i < rpt_desc.size; i++)
    {
        ALOGE("%02x ", rpt_desc.value[i]);
    }
    #endif

    if(rpt_desc.size > 0)
    {
        g_pReportDesc = (unsigned char *)malloc(rpt_desc.size * sizeof(unsigned char));
        if(g_pReportDesc != NULL)
        {
            memcpy(g_pReportDesc,rpt_desc.value,rpt_desc.size);
            g_report_desc_size = rpt_desc.size;
            ALOGE("getReportDescriptor (Size: %d), g_pReportDesc[0] = 0x%02x\n", g_report_desc_size, g_pReportDesc[0]);
            return rpt_desc.size;
        }
    }

    ALOGE("getReportDescriptor failed !!!\n");
    return 0;
}
#endif

//get hid report descriptor to point desc and return the length
static int getReportDescriptor(const int fd, unsigned char * desc)
{
    if(fd < 0 || desc == NULL)
    {
        ALOGE("getReportDescriptor  Invalid input parameter !!!");
        return -1;
    }

    // get size of the report descriptor
    struct hidraw_report_descriptor rpt_desc;
    if (ioctl(fd, HIDIOCGRDESCSIZE, &rpt_desc.size) < 0)
    {
        ALOGE("getReportDescriptor: Failed to get report descriptor size, error:%s",strerror(errno));
        return -1;
    }

    // get the report descriptor
    if (ioctl(fd, HIDIOCGRDESC, &rpt_desc) < 0)
    {
        ALOGE("getReportDescriptor: Failed to get report descriptor, error:%s",strerror(errno));
        return -1;
    }

    #if DEBUG_DATA
    for (int i = 0; i < rpt_desc.size; i++)
    {
        ALOGE("%02x ", rpt_desc.value[i]);
    }
    #endif

    if(rpt_desc.size > 0)
    {
        memcpy(desc,rpt_desc.value,rpt_desc.size);
        return rpt_desc.size;
    }

    ALOGE("getReportDescriptor failed !!!\n");
    return -1;
}



static int getMaxTouchPointNumber(int fd)
{
    int max_contacts  = 255;

    if(fd < 0)
    {
        ALOGE("getMaxTouchPointNumber  fd < 0");
        return 0;
    }

    max_contacts = MAX_CONTACTS; //for touch back, we use a fiexed value
    ALOGE("getMaxTouchPointNumber max_contacts = %d",max_contacts);

    return max_contacts;
}

static void touchReportInKernelEnable(bool enable)
{

    char *disable;
    if(enable)
    {
        disable = "0";
    }
    else
    {
        disable = "1";
    }

    int reportCtrlFd = open(TOUCH_REPORT_IN_KERNEL_DISABLE_CONTROL_PATH, O_RDWR);

    if(reportCtrlFd > 0)
    {
        int iRet = write(reportCtrlFd, disable, 1);
        if(iRet < 0)
        {
            ALOGE("write fd: %d failed, err: %s",reportCtrlFd,strerror(errno));
        }
        else
        {
            ALOGE("touch report in kernel, disable=%s ok, iRet = %d",disable,iRet);
        }
    }
    else
    {
        ALOGE("open %s failed, err: %s",TOUCH_REPORT_IN_KERNEL_DISABLE_CONTROL_PATH,strerror(errno));
    }
}

static bool WriteToFile(const char * path, const unsigned char * data, size_t len)
{
    int fd_hid = 0;
    int byte_write;

    if(path == NULL || data == NULL || len <= 0)
    {
        ALOGE("WriteToFile param is invalid, data is NULL or len <= 0");
        return false;
    }

    ALOGE("WriteToFile: data:0x%x ... 0x%x, len =%d ",data[0],data[len-1],len);

    fd_hid = open(path, O_WRONLY|O_NONBLOCK);

    if (fd_hid > 0)
    {
        byte_write = write(fd_hid, data, len);
        if (byte_write != len)
        {
            ALOGE("write data to %s failed!!!  error=%s ",path,strerror(errno));
            close(fd_hid);
            return false;
        }

        ALOGE("WriteToFile: len = %d,  0x%x ... 0x%x",len,data[0],data[len-1]);
        close(fd_hid);
    }
    else
    {
        ALOGE("open %s failed!!!  error=%s ",path,strerror(errno));
        return false;
    }

    return true;
}


static bool setTouchScreenReportDescAndFeature(const int hidraw_fd)
{
    bool bRet = false;
    int i = 0;
    int usage_page = 0;
    int usage = 0;

	unsigned char desc[4096] = {0};
    int len = getReportDescriptor(hidraw_fd, desc);
    while (i < len)
    {
        unsigned char item = desc[i];
        unsigned char tag =  (item >> 4) & 0x0F;  // upper 4 bits
        unsigned char type = (item >> 2) & 0x03;  // Middle 2 bits
        unsigned char size = item & 0x03;         // low 2 bits

        // data size
        size_t data_size = 0;
        if (size == 0) data_size = 0;
        else if (size == 1) data_size = 1;
        else if (size == 2) data_size = 2;
        else data_size = 4;

        // Usage Page (Global)
        if (type == 0x01 && tag == 0x00)
        {
            if (data_size == 1)
            {
                usage_page = desc[i + 1];
            }
            else if (data_size == 2)
            {
                usage_page = desc[i + 1] | (desc[i + 2] << 8);
            }
        }

        // Usage (Local)
        if (type == 0x02 && tag == 0x00)
        {
            if (data_size == 1)
            {
                usage = desc[i + 1];
            }
            else if (data_size == 2)
            {
                usage = desc[i + 1] | (desc[i + 2] << 8);
            }

            // Touch screen or finger?
            if (usage_page == 0x0D && (usage == 0x04 || usage == 0x22))
            {
                int maxTouchPoints = getMaxTouchPointNumber(hidraw_fd);
                unsigned char feature_report[2]= {0x07,(unsigned char)maxTouchPoints};
                unsigned char feature_report_len[1] = {'2'};

                //write report descriptor and feature report:(0x07 max_touch_points) and its lenght to f_hid_rk driver
                bRet =  WriteToFile(HID_GADGET_REPORT_FEATURE,feature_report,sizeof(feature_report)/sizeof(unsigned char));
                bRet &= WriteToFile(HID_GADGET_REPROT_FEATURE_LENGTH, feature_report_len, sizeof(feature_report_len)/sizeof(unsigned char));
                bRet &= WriteToFile(HID_GADGET_REPORT_DESC,desc,len);

                if(!bRet)
                {
                    ALOGE("setReportDescAndFeature failed  !!!");
                }
                else
                {
                    char str[32] = {0};
                    sprintf(str, "%d", len);
                    property_set(PROP_TOUCHBACK_REPORTDESC_LEN,str);

                    return true;
                }
				
                break;
            }
        }

        i += 1 + data_size; // go  to next item
    }

	property_set(PROP_TOUCHBACK_REPORTDESC_LEN,"-1");
    return bRet;
}

static bool detectAndConnectTouchDevice()
{
    int i,ret = false;
    int fd = -1;
    char fileName[16]="0";
    struct hidraw_devinfo dinfo;
    memset(&dinfo, 0, sizeof(dinfo));
    int vid = 0;
    int pid = 0;
    int desc_size;
    char panelName[256] = {0};
    struct hidraw_report_descriptor desc;

    if(g_touchDataFd > 0)
    {
        close(g_touchDataFd);
        property_set("vendor.xbh.touch.rawdata_fd","-1");
        property_set("vendor.xbh.touch.rawdata_node","");
        property_set("vendor.xbh.touch.vidpid","");

        connectCount++;
        char count[64]= {0};
        sprintf(count,"%ld",connectCount);
        property_set("vendor.xbh.touch.reconnect_count",count);

        g_touchDataFd = -1;
        usleep(200*1000);
    }

    g_isTouchOpened = 0;
    
        for (int i = 0; i < MAX_MINOR; i++)
        {
            sprintf(fileName, "/dev/hidraw%d", i);
            fd = open(fileName, O_RDWR);
            if (fd >= 0)
            {
                if (ioctl(fd, HIDIOCGRAWINFO, &dinfo) == 0 && ioctl(fd, HIDIOCGRAWNAME(sizeof(panelName)), panelName) > 0)
                {
                    vid = dinfo.vendor & 0xFFFF;
                    pid = dinfo.product & 0xFFFF;

                    //for touch back
                    if(gSetTouchScreenReportDescFlag == 0)
                    {
                        if(setTouchScreenReportDescAndFeature(fd))
                        {
                            gSetTouchScreenReportDescFlag = 1;
                        }
                    }

                    if (vid == TOUCH_APLE_VID && pid == TOUCH_APLE_PID && (strlen(panelName) < panelNameLength)) {
                        ALOGE("check to this Panel prome status");
                        g_touchDataFd = fd;
                        gCacheFd = i;
                        g_cacheResetHid = i;
                        isAP10TouchCheck = true;

                        g_isTouchOpened = 1;
                        g_isHaveTouchDriver = false;

                        //touch data format
                        gReportId = 0x02;
                        gFingerDown      = 0x07;
                        gFingerMove      = 0x07;
                        gFingerUp        = 0x04;
                        gPenDown         = 0x02;
                        gPenMove         = 0x03;
                        gPenUp           = 0x00;
                        gEraserDown      = 0x06;
                        gEraserMove      = 0x0e;
                        gEraserUp        = 0x00;
                        gValidPointNumPos = 61;
                        gMaxPointNumOnePackage = 5;
                        startPointId = 0;
                        gBytesNumOfOnePoint = 12;
                        gFingerReportId = 0x02;
                        gFingerPackageLen = 64;
                        gPenPackageLen = 13;
                        gPenReportIdMin = 0x10;
                        gPenNumMax = 4;
                        startPointPenId = 20;
                        gEraserReportId = 0x14;
                        maxAndroidTouchPointNum = 50;
                        maxAndroidTouchPointId = startPointId + maxAndroidTouchPointNum;
                        reportAndroidTouchInKernel = false;
                        dateLen = 12;
                        slotLen = 5;
                        indexNum = 1;

                        property_set(PROP_TOUCHSCREEN_HIDRAW_NODE,fileName);
                        memset(rawDataNode,0,sizeof(rawDataNode));
                        strcpy(rawDataNode,fileName);
                        sprintf(touchVidPid, "%04x:%04x",vid,pid);
                        property_set(PROP_TOUCHSCREEN_VID_PID,touchVidPid);

                        char deviceNodeFd[32] = {0};
                        sprintf(deviceNodeFd, "%d", g_touchDataFd);
                        property_set(PROP_TOUCHSCREEN_HIDRAW_DATA_FD,deviceNodeFd);
                        touchReportInKernelEnable(reportAndroidTouchInKernel);
                        ioctl(fd, HIDIOCGRDESCSIZE, &desc_size);
                        desc.size = desc_size;
                        ioctl(fd, HIDIOCGRDESC, &desc);
                        if (desc.value[1] != gPointStatusInvalid) {
                            continue;
                        }
                        ret = true;
                        return ret;
                    }
                    else
                    {
                        //detected other touch screen
                        if(gSetTouchScreenReportDescFlag == 1)
                        {
                            g_touchDataFd = fd;
                            char deviceNodeFd[32] = {0};
   	                    sprintf(deviceNodeFd, "%d", g_touchDataFd);
    	                    property_set(PROP_TOUCHSCREEN_HIDRAW_DATA_FD,deviceNodeFd);
						
    		    	    property_set(PROP_TOUCHSCREEN_HIDRAW_NODE,fileName);
			    char tpVidPid[32] = {0};
	                    sprintf(tpVidPid, "%04x:%04x",vid,pid);
	                    property_set(PROP_TOUCHSCREEN_VID_PID,tpVidPid);
 
                            return true;
                        }
                    }
                }
                else
                {
                    close(fd);
                    ALOGE("Get hidraw info of %s failed !!!",fileName);
                }
                close(fd);
            }
        }
    return ret;
}

void *touchNotifyAddWatch(void *arg)
{

    while (true) {
    static time_t last_check = 0;
    static int last_count = 0;
    
    DIR *dir = opendir("/dev");
    if (!dir) return (void *)0; 
    
    int current_count = 0;
    struct dirent *ent;
    
    while ((ent = readdir(dir)) != NULL) {
        if (strstr(ent->d_name, "hidraw")) {
            current_count++;
        }
    }
    closedir(dir);
    
    time_t now = time(NULL);
    if (current_count > last_count && now - last_check < 5) {
        detectAndConnectTouchDevice();
    }
    
    last_count = current_count;
    last_check = now;
    }
    return (void *)0;
}

static void createTouchScreen()
{
    int ret;
    struct uinput_user_dev uinp;
    struct input_event event;
    char value[PROPERTY_VALUE_MAX] = {0};

    ALOGE("createTouchScreen...");
    uinp_fd = open("/dev/uinput", O_WRONLY|O_NONBLOCK);
    if(uinp_fd <= 0) {
        ALOGE("TouchProcess   Unable to open /dev/uinput  !!!!!!!!!!!!! error:%s\n",strerror(errno));
        return;
    }

    ALOGE("uinp_fd = %d",uinp_fd);

    // configure touch device event properties
    memset(&uinp, 0, sizeof(uinp));
    strncpy(uinp.name, "XBHTouchScreen", UINPUT_MAX_NAME_SIZE);
    uinp.id.version = 4;
    // should be 0x0000:0x0000
    // uinp.id.vendor = TOUCH_APLE_VID;
    // uinp.id.product = TOUCH_APLE_PID;
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

    //Save Android USB PID VID, Hide bezel raw event in EventHub
    snprintf(value, sizeof(value), "%d", TOUCH_APLE_PID);
    ret = property_set("persist.vendor.xbh.bezel.an.pid", value);
    if (ret)
    {
        ALOGE("set persist.vendor.xbh.bezel.an.pid %s failed, ret = %d", value, ret);
    }

    snprintf(value, sizeof(value), "%d", TOUCH_APLE_VID);
    ret = property_set("persist.vendor.xbh.bezel.an.vid", value);
    if (ret)
    {
        ALOGE("set persist.vendor.xbh.bezel.an.vid %s failed, ret = %d", value, ret);
    }

    ALOGE("create XBHTouchScreen ok!");

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
        ALOGE("TouchProcess   nibiru uinput errormag info :%s\n",mesg);
        return false;
    }
    return true;
}





static void resetTouchCacheData()
{
    int i;
    memset(&mCacheTouchData,0,sizeof(mCacheTouchData));
    for(i = 0; i < maxAndroidTouchPointNum + 1; i++)
    {
        mCacheTouchData.points[gBytesNumOfOnePoint*i + 1] = 0xFF;//reset point id
    }

    for(i = 0;i<gPenNumMax;i++)
    {
        mCacheTouchData.mapPenId[i] = 0xFF;
        mCacheData[i] = 0;
    }
    if (isAP10TouchCheck) {
        mCacheTouchData.penIsDown[0] = 0;
        mCacheTouchData.penIsDown[1] = 0;
        mCacheTouchData.penIsDown[2] = 0;
        mCacheTouchData.penIsDown[3] = 0;
        mCacheTouchData.penIsDown[4] = 0;
    }
}

//touchCachBuffer: 20x10 = 200 byts
static void parseAndReportTouch(void)
{
    int i;
    int flagOnce = 0;
    unsigned char pointStatus;

    for(i = 0; i < maxAndroidTouchPointNum; i++)
    {
        mPoints[i].status = mCacheTouchData.points[dateLen * i] & 0x0F;
        mPoints[i].id     = mCacheTouchData.points[dateLen * i + 1];
        mPoints[i].x      = mCacheTouchData.points[dateLen * i + 3] * 256 + mCacheTouchData.points[dateLen * i + 2];
        mPoints[i].y      = mCacheTouchData.points[dateLen * i + 5] * 256 + mCacheTouchData.points[dateLen * i + 4];
        mPoints[i].width  = mCacheTouchData.points[dateLen * i + 7] * 256 + mCacheTouchData.points[dateLen * i + 6];
        mPoints[i].height = mCacheTouchData.points[dateLen * i + 9] * 256 + mCacheTouchData.points[dateLen * i + 8];

        //status byte high 4bit, 0x1x: pen, 0x2x: fingler,0x3x:erase
        int toolType = (mCacheTouchData.points[dateLen * i] & 0xF0) >> 4;
        if(toolType == 0x2)
        {
            mPoints[i].toolType = MT_TOOL_FINGER;
        }
        else if(toolType == 0x3)
        {
            mPoints[i].toolType = MT_TOOL_ERASER;
        }
        else if(toolType == 0x1)
        {
            mPoints[i].toolType = MT_TOOL_PEN;
        }
        else
        {
            mPoints[i].toolType = MT_TOOL_FINGER;
        }

        pointStatus = mPoints[i].status;
        if ((mCacheData[0] == gPenReportIdMin && mCacheTouchData.points[dateLen * i] >= gPenReportIdMin)) {
            for(int j = (maxAndroidTouchPointNum - 1) ; j >= 0; j--) {
                if (mCacheTouchData.points[dateLen * j + 1] == 0xFF && flagOnce < 1 && ((i + 1) > mCacheTouchData.mapPenId[0])) {
                    mPoints[i].id = j + 1;
                    mCacheTouchData.points[dateLen * i + 1] = j + 1;
                    flagOnce = 1;
                    break;
                 }
            }
        }


        if(pointStatus == gFingerDown || pointStatus == gFingerMove)
        {
            if(mPoints[i].id <= maxAndroidTouchPointId)
            {
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id - startPointId);
                //device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, mPoints[i].id);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_X, mPoints[i].x);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_POSITION_Y, mPoints[i].y);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MAJOR, mPoints[i].width);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOUCH_MINOR, mPoints[i].height);

                if(mPoints[i].toolType == MT_TOOL_ERASER)
                {
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, 0x141);
                    device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 1);
                }
                else
                {
                    device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TOOL_TYPE, mPoints[i].toolType);//pen,finger
                }

                device_writeEvent(uinp_fd, EV_KEY, BTN_TOUCH, 1);
            }
        }
        else if(pointStatus == gFingerUp)
        {
            if(mPoints[i].id <= maxAndroidTouchPointId)
            {
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id - startPointId);
                //device_writeEvent(uinp_fd, EV_ABS, ABS_MT_SLOT, mPoints[i].id);
                device_writeEvent(uinp_fd, EV_ABS, ABS_MT_TRACKING_ID, -1);  // report up

                if(mPoints[i].toolType == MT_TOOL_ERASER)
                {
                    device_writeEvent(uinp_fd, EV_KEY, BTN_TOOL_RUBBER, 0);
                    resetTouchCacheData();
                }
            }
        }
    }

    //one point or multi points down or up,only report EV_SYN-SYN_REPORT one time
    device_writeEvent(uinp_fd, EV_SYN, SYN_REPORT, 0);
    memset(mPoints,0,maxAndroidTouchPointNum*sizeof(TouchPoint));

    #ifdef ENABLE_GET_TIME
    gettimeofday(&tvNow, NULL);
    tms4 = (int64_t)tvNow.tv_sec*1000 + tvNow.tv_usec/1000;
    ALOGE("tms4=%lld(%d)",tms4,tms4-tms3);
    #endif
}

static void allTouchPointUp() {
    int i;
    for(i = 0; i < maxAndroidTouchPointNum; i++)
    {
        int toolType = (mCacheTouchData.points[dateLen * i] & 0xF0) >> 4;
        if(toolType == 0x2)
        {
            mCacheTouchData.points[gBytesNumOfOnePoint*i] = 0x04;//reset finger up
        }
        else if(toolType == 0x3)
        {
            mCacheTouchData.points[gBytesNumOfOnePoint*i] = 0x30 + gFingerUp;//reset erase up
        }
        else if(toolType == 0x1)
        {
            mCacheTouchData.points[gBytesNumOfOnePoint*i] = 0x10 + gFingerUp;//reset pen up
        }
        else
        {
            mCacheTouchData.points[gBytesNumOfOnePoint*i] = 0x04;//reset default up
        }
    }

}

int readTouchRawData(int fd,unsigned char *pBuf)
{
    int ret = 0 ;
    int num = 0 ;
    if (pBuf != NULL)
    {
        fd_set read_fdset;
        struct  timeval timeout;
        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);
        timeout.tv_sec = 0;
        timeout.tv_usec = TIMEOUT_READ_TOUCH_RAWDATA_USECOND;
        do
        {
            ret = select(fd + 1 , &read_fdset,  NULL ,  NULL , &timeout);
        }
        while(ret <  0 && errno == EINTR);

        //ALOGE("Hxread, out, select%d\n",ret);
        if  (ret == 0)
        {
            ret = - 1 ;
            errno = ETIMEDOUT;
            if (eagainFlag > 0 && !mCacheTouchData.haveFingerDown && !g_getStatusUp) {
                eagainFlag = 0;
                checkEraseStatus = false;
                checkPenStatus = false;
                allTouchPointUp();
                parseAndReportTouch();
                resetTouchCacheData();
            }
        }
        else if(ret == 1)
        {
            num = read(fd,pBuf,BUF_LEN);
            eagainFlag = 1;
            errno = 0;
            return num;
        }
    }
    return  ret;
}
/*
cache the raw data to buffer in finger data format
//07 id xL xH yL yH wL wH hL hH
//17 id xL xH yL yH wL wH hL hH
//27 id xL xH yL yH wL wH hL hH
//37 id xL xH yL yH wL wH hL hH
//04 id xL xH yL yH wL wH hL hH
//14 id xL xH yL yH wL wH hL hH
//24 id xL xH yL yH wL wH hL hH
//34 id xL xH yL yH wL wH hL hH
*/
void cacheTouchData(unsigned char* buf,int len)
{
    int i,j;
    bool bHasFingerDown = false;

    if(buf != NULL)
    {
        if((buf[0] == gFingerReportId) && (len == gFingerPackageLen))
        {
            //cache the finger data by point id
            for(j = 0;j < gMaxPointNumOnePackage;j++)
            {

                int pointId      = buf[dateLen * j + 2];
                int pointStatus  = buf[dateLen * j + 1];

                if(pointStatus != gFingerInactive)
                {
                    for(i = 0; i < maxAndroidTouchPointNum; i++) {
                        //update or add finger data. cache array index: 0~19, point id: 1~20
                        if(((mCacheTouchData.points[dateLen * (pointId-startPointId)+1] == buf[dateLen * j + 2]) && mCacheTouchData.mapPenId[0] != (buf[dateLen * j + 2] -1))
                        || (mCacheTouchData.points[dateLen * (pointId-startPointId)+1] == 0xFF) && (buf[dateLen * j + 1] == gFingerDown))
                        {
                            memcpy(mCacheTouchData.points+dateLen*(pointId-startPointId), buf+dateLen*j + 1, gBytesNumOfOnePoint);
                        } else if ((pointId < (maxAndroidTouchPointNum - 1)) && !isAP10TouchCheck) {
                            pointId = buf[dateLen * j + 2] + 1;
                            memcpy(mCacheTouchData.points+dateLen*(pointId-startPointId), buf+dateLen*j + 1, gBytesNumOfOnePoint);
                        }

                    }

                    if(pointStatus == gFingerDown || pointStatus == gFingerMove)
                    {
                        bHasFingerDown = true;
                    }
                }
            }

            if(!bHasFingerDown)
            {
                //all fingers up now
                mCacheTouchData.haveFingerDown = false;
            }
            else
            {
                mCacheTouchData.haveFingerDown = true;
            }
        }
        else if(((buf[0] >= gPenReportIdMin) && (buf[0] <= gPenReportIdMin + gPenNumMax -1))
                && (len == gPenPackageLen))
        {
            if(buf[1] == gPenUp)
            {
                mCacheTouchData.penIsDown[buf[0]-gPenReportIdMin] = 0;
            }
            else
            {
                mCacheTouchData.penIsDown[buf[0]-gPenReportIdMin] = 1;
            }

            if(isAP10TouchCheck) {
                for(i = startPointPenId; i < maxAndroidTouchPointNum; i++)
                {
                    if(buf[1] == gPenDown) //add pen data
                    {
                        if(mCacheTouchData.points[dateLen * i + 1] == 0xFF)
                        {
                            memcpy(mCacheTouchData.points+dateLen*i,   buf,2);    //2 bytes: HEAD STATUS
                            memcpy(mCacheTouchData.points+dateLen*i+2, buf+3, 8); //8 bytes: x,y,w,h

                            mCacheTouchData.points[dateLen * i] = 0x10+gFingerDown;

                            //give a finger id to pen
                            mCacheTouchData.points[dateLen * i + 1] = i + startPointId + indexNum; //point id: 1~20, cache index:0~19

                            //record the finger id for pen
                            mCacheTouchData.mapPenId[buf[0] - gPenReportIdMin] = i; //index in cache array

                            break;

                        }
                    }
                    else //pen move or up
                    {
                        if (mCacheTouchData.mapPenId[0] != mCacheTouchData.points[dateLen * i + 1]
                            && mCacheTouchData.mapPenId[1] != mCacheTouchData.points[dateLen * i + 1]
                            && mCacheTouchData.mapPenId[2] != mCacheTouchData.points[dateLen * i + 1]
                            && mCacheTouchData.mapPenId[3] != mCacheTouchData.points[dateLen * i + 1] ) {

                            int index4pen = mCacheTouchData.mapPenId[buf[0] - gPenReportIdMin];

                            if(buf[1] == gPenMove)
                            {
                                mCacheTouchData.points[dateLen * index4pen] = 0x10+gFingerMove;
                            }
                            else
                            {
                                mCacheTouchData.points[dateLen * index4pen] = 0x10+gFingerUp;
                                if (mCacheTouchData.penIsDown[1] == 1 && buf[0] == gPenReportIdMin) {
                                    mCacheTouchData.mapPenId[0] = 0xFF;
                                } else if (mCacheTouchData.penIsDown[1] == 0 && buf[0] == gPenReportIdMin + 1) {
                                    mCacheTouchData.mapPenId[1] = 0xFF;
                                }
                            }

                            memcpy(mCacheTouchData.points+dateLen*index4pen+2, buf+3, 8); //8 bytes: x,y,w,h


                            break;
                        }
                    }
                }
            } else {
                for(i = 0; i < maxAndroidTouchPointNum; i++)
                {
                    if(buf[1] == gPenDown) //add pen data
                    {
                        if(mCacheTouchData.points[dateLen * i + 1] == 0xFF && !cacheFingerTouchStatus.isDown[i + 1])
                        {
                            memcpy(mCacheTouchData.points+dateLen*i,   buf,2);    //2 bytes: HEAD STATUS
                            memcpy(mCacheTouchData.points+dateLen*i+2, buf+3, 8); //8 bytes: x,y,w,h

                            mCacheTouchData.points[dateLen * i] = 0x10+gFingerDown;

                            //give a finger id to pen
                            mCacheTouchData.points[dateLen * i + 1] = i + startPointId; //point id: 1~20, cache index:0~19

                            //record the finger id for pen
                            mCacheTouchData.mapPenId[buf[0] - gPenReportIdMin] = i; //index in cache array

                            break;

                        }
                    }
                    else //pen move or up
                    {
                        if (mCacheTouchData.mapPenId[0] != mCacheTouchData.points[dateLen * i + 1]
                            && mCacheTouchData.mapPenId[1] != mCacheTouchData.points[dateLen * i + 1]
                            && mCacheTouchData.mapPenId[2] != mCacheTouchData.points[dateLen * i + 1]
                            && mCacheTouchData.mapPenId[3] != mCacheTouchData.points[dateLen * i + 1]) {

                            int index4pen = mCacheTouchData.mapPenId[buf[0] - gPenReportIdMin];

                            if(buf[1] == gPenMove)
                            {
                                mCacheTouchData.points[dateLen * index4pen] = 0x10+gFingerMove;
                            }
                            else
                            {
                                mCacheTouchData.points[dateLen * index4pen] = 0x10+gFingerUp;
                                if (mCacheTouchData.penIsDown[1] == 1 && buf[0] == gPenReportIdMin) {
                                    mCacheTouchData.mapPenId[0] = 0xFF;
                                } else if (mCacheTouchData.penIsDown[1] == 0 && buf[0] == gPenReportIdMin + 1) {
                                    mCacheTouchData.mapPenId[1] = 0xFF;
                                }
                            }

                            memcpy(mCacheTouchData.points+dateLen*index4pen+2, buf+3, 8); //8 bytes: x,y,w,h


                            break;
                        }
                    }
                }

            }

        }
        else if((buf[0] == gEraserReportId) && (len = gPenPackageLen))
        {
            if(buf[1] == gEraserUp)
            {
                mCacheTouchData.penIsDown[4] = 0;
            }
            else
            {
                mCacheTouchData.penIsDown[4] = 1;
            }

            for(i = 0; i < maxAndroidTouchPointNum; i++)
            {
                if(buf[1] == gEraserDown) //add eraser data
                {
                    if(mCacheTouchData.points[dateLen * i+1] == 0xFF)
                    {
                        memcpy(mCacheTouchData.points+dateLen * i,   buf,2);    //2 bytes: HEAD STATUS
                        memcpy(mCacheTouchData.points+dateLen * i+2, buf+3, 8); //8 bytes: x,y,w,h

                        mCacheTouchData.points[dateLen * i] = 0x30+gFingerDown;

                        //give a finger id to pen
                        mCacheTouchData.points[dateLen * i+1] = i + startPointId + indexNum; //point id: 1~20, cache index:0~19

                        //record the finger id for pen
                        mCacheTouchData.mapPenId[buf[0] - gPenReportIdMin] = i; //index in cache array


                        break;
                    }
                }
                else //eraser move or up
                {
                    int index4pen = mCacheTouchData.mapPenId[buf[0] - gPenReportIdMin];

                    if(buf[1] == gEraserMove)
                    {
                        mCacheTouchData.points[dateLen * index4pen] = 0x30+gFingerMove;
                    }
                    else
                    {
                        mCacheTouchData.points[dateLen * index4pen] = 0x30+gFingerUp;
                    }

                    memcpy(mCacheTouchData.points+dateLen * index4pen+2, buf+3, 8); //8 bytes: x,y,w,h

                    break;
                }
            }
        }


        if(mCacheTouchData.haveFingerDown == false
           && mCacheTouchData.penIsDown[0] == 0
           && mCacheTouchData.penIsDown[1] == 0
           && mCacheTouchData.penIsDown[2] == 0
           && mCacheTouchData.penIsDown[3] == 0
           && mCacheTouchData.penIsDown[4] == 0)
        {
            ALOGE("all points up, report them now");
            checkEraseStatus = false;
            checkPenStatus = false;
            allTouchPointUp();
            parseAndReportTouch();
            resetTouchCacheData();

            #if DEBUG_TOUCHBACK
                gWrLen = 0;
                gRdLen = 0;
            #endif
        }

    }

}


void *touchDataProcess_thread(void *arg)
{
    UNUSED(arg);
    int i;
    int len = -1;
    unsigned char buf[DATA_BUF_LEN] = {0};

    ALOGE("touchDataProcess_thread started");

    while(1)
    {
        if (property_get_bool("vendor.xbh.aple.touchupdate.release.state",false)) {
            ALOGE("touch is set to update over to this");
            sleep(10);
            detectAndConnectTouchDevice();
            property_set("vendor.xbh.aple.touchupdate.release.state","false");
        }
        if(g_touchDataFd >= 0)
        {
            len = readTouchRawData(g_touchDataFd, buf);
            //for touch back data
            if(len > 0)
            {
                #if DEBUG_TOUCHBACK
                gRdLen = gRdLen + len;
                #endif

                if (g_pctouchDevId < 0)
                {
                    g_pctouchDevId = open(HID_GADGET_F_HID_RK_NODE, O_WRONLY|O_NONBLOCK);
                    if(g_pctouchDevId < 0)
                    {
                        //ALOGE("TouchProcess Unable to open %s !!! error:%s\n",HID_GADGET_F_HID_RK_NODE,strerror(errno));
                    }
                    else
                    {
                        char str[32] = {0};
                        sprintf(str, "%d", g_pctouchDevId);
                        property_set(PROP_TOUCHBACK_DRIVER_NODE_FD,str);

                        if(gSourceTouchEnabled)
                        {
                            //send touch back data to PC
                            int res = write(g_pctouchDevId, buf, len);
                            if(res < 0)
                            {
                                usleep(5 * 1000); //try again after 5ms
                                res = write(g_pctouchDevId, buf, len);

                                #if DEBUG_TOUCHBACK
                                if(res > 0)
                                {
                                    gWrLen = gWrLen + res;
                                }
                                #endif
                            }
                            #if DEBUG_TOUCHBACK
                            else
                            {
                                gWrLen = gWrLen + res;
                            }
                            #endif
                        }
                        #if DEBUG_TOUCHBACK
                        else
                        {
                             ALOGE("touchDataProcess_thread: gSourceTouchEnabled=%d",gSourceTouchEnabled);
                        }
                        #endif

                    }
                }
                else
                {
                    if(gSourceTouchEnabled)
                    {
                        //send touch back data to PC
                        int res = write(g_pctouchDevId, buf, len);
                        if(res < 0)
                        {
                            //ALOGE("write: len = %d errno = %d, %s",len, errno, strerror(errno));
                            if(errno == ESHUTDOWN)
                            {
                                close(g_pctouchDevId);
                                g_pctouchDevId = open(HID_GADGET_F_HID_RK_NODE, O_WRONLY|O_NONBLOCK);
                                //ALOGE("re-open: g_pctouchDevId = %d",g_pctouchDevId);

                                char str[32] = {0};
                                sprintf(str, "%d", g_pctouchDevId);
                                property_set(PROP_TOUCHBACK_DRIVER_NODE_FD,str);
                            }

                            if(g_pctouchDevId > 0)
                            {
                                usleep(5 * 1000); //try again after 5ms
                                res = write(g_pctouchDevId, buf, len);
                                if(res  < 0)
                                {
                                    //ALOGE("re-write  len = %d, res = %d",len, res);
                                }
                                #if DEBUG_TOUCHBACK
                                else
                                {
                                    gWrLen = gWrLen + res;
                                }
                                #endif
                            }
                        }
                        #if DEBUG_TOUCHBACK
                        else
                        {
                            gWrLen = gWrLen + res;
                        }

                        ALOGE("gRdLen = %lld, gWrLen = %lld",gRdLen, gWrLen);
                        #endif
                    }
                    #if DEBUG_TOUCHBACK
                    else
                    {
                         ALOGE("touchDataProcess_thread: gSourceTouchEnabled=%d",gSourceTouchEnabled);
                    }
                    #endif
                }
            }
        }
        else
        {
            //have not opened touch device after restarting
            if(!g_isHaveTouchDriver)
            {
                while(!detectAndConnectTouchDevice())
                {
                    sleep(1);
                }
            }

            continue;
        }


        if(len < 0)
        {
            // I/O error or no such file, touch panel problem,up status maybe not send
                if(errno == ENOENT || errno == EIO || errno == ENODEV
                    || ((errno == 0) && (access(rawDataNode,F_OK) == -1)))
                {
                    //have no touch driver, hotplug touch panel
                        if(!g_isHaveTouchDriver)
                        {
                            while(!detectAndConnectTouchDevice())
                            {
                                sleep(1);
                            }
                        }
                }

        }
        else  // len >= 0
        {
            #ifdef SAVE_TOUCHDATA2FILE
                if((buf[0]== gPackageId)
                { //get touch data
                    #ifdef ENABLE_GET_TIME
                        gettimeofday(&tvNow, NULL);
                        int64_t stamp_ms = (int64_t)tvNow.tv_sec*1000 + tvNow.tv_usec/1000;
                        //ALOGE("tv_sec = %u, tv_usec = %u",tvNow.tv_sec,tvNow.tv_usec);
                        ALOGE("stamp_ms=%lld",stamp_ms);
                        buf[67] = stamp_ms & 0xff;
                        //ALOGE("buf[67]=%d",buf[67]);
                        buf[66] = (stamp_ms >> 8)& 0xff;
                        //ALOGE("buf[66]=%d",buf[66]);
                        buf[65] = (stamp_ms >> 16)& 0xff;
                        //ALOGE("buf[65]=%d",buf[65]);
                        buf[64] = (stamp_ms >> 24)& 0xff;
                        //ALOGE("buf[64]=%d",buf[64]);
                        #endif

                        int wLen = write(dataFd,buf,DATA_BUF_LEN);
                        if(wLen < 0)
                        {
                            ALOGE("write touchData file failed!!!, errno = %d,%s",errno,strerror(errno));
                        }
                    }
                    #endif
        }

        //Android touch report
        if(len > 0 && !reportAndroidTouchInKernel)
        {
             /*
             1.first finger or pen package will be cached. Then read the first send package again report the cached touch.
             (1) pen is the first data which will be sent from touch panel if using pen, like: 10,11,... 02
             (2) when pen or finger is exist, erase data 0x14 will not be sent,
             and if use pen or finger when using eraser,  will stop sending eraser data.
             2.All cached data are UP status, will report the cached data.
             */
            //0XFF:The meaning is to report the maximum data of the touch box
            if (isAP10TouchCheck) {
                if (buf[0] == 0x05 || buf[0] >= 0x50) {
                    gValidPointNumPos = 61;
                    gMaxPointNumOnePackage = 6;
                    startPointId = 0;
                    startPointPenId = 20;
                    gBytesNumOfOnePoint = 10;
                    gFingerReportId = 0x02;
                    gFingerPackageLen = 64;
                    gPenReportIdMin = 0x50;
                    gPenNumMax = 2;
                    gEraserReportId = 0x54;
                    dateLen = 10;
                    slotLen = 6;
                    indexNum = 1;
                    gApPenDown = 0x50;
                    gPenPackageLen = 15;
                    isAP10NewAgreement = true;
                    gBufErrorStatus = 0x60;
                    gFingerAP10NewReportId = 0x05;
                    int timeStatus = 0;

                    if (len > gPenPackageLen) {
                        if (buf[0] == gFingerAP9ReportId || buf[0] == gFingerAP10NewReportId) {
                            buf[0] = gFingerReportId;
                        }
                    } else {
                        if (buf[2] == gAPErasePen) {
                            g_getStatusUp = true;
                        } else {
                            g_getStatusUp = false;
                        }

                        if (buf[0] == gApPenDown && buf[1] != gEraserDown && buf[1] != gAPEraseMove && buf[2] == gAPSmallPen && !checkEraseStatus) {
                             buf[0] = gPenReportIdMin;
                             checkPenStatus = true;
                        } else if (buf[0] == (gApPenDown + 1) && buf[1] != gAPEraseDown && buf[1] != gAPEraseMove && buf[2] == gAPSmallPen && !checkEraseStatus && checkPenStatus) {
                             buf[0] = gPenReportIdMin + 1;
                        } else if (buf[0] == gApPenDown && (buf[1] == gAPEraseDown || buf[1] == gAPEraseMove) && buf[2] > gAPSmallPen) {
                             buf[0] = gEraserReportId;
                             checkEraseStatus = true;
                        } else if (buf[0] == gApPenDown && buf[1] == gAPEraseUp && checkEraseStatus && buf[2] > gAPSmallPen) {
                             buf[0] = gEraserReportId;
                             checkEraseStatus = false;
                        } else {
                             buf[0] = gBufErrorStatus;
                        }

                    }
                } else if (buf[0] == 0x03 || buf[0] >= 0x30) {
                    gValidPointNumPos = 61;
                    gMaxPointNumOnePackage = 6;
                    startPointId = 0;
                    startPointPenId = 20;
                    gBytesNumOfOnePoint = 10;
                    gFingerReportId = 0x02;
                    gFingerPackageLen = 64;
                    gPenPackageLen = 14;
                    gPenReportIdMin = 0x30;
                    gPenNumMax = 4;
                    gEraserReportId = 0x34;
                    dateLen = 10;
                    slotLen = 6;
                    indexNum = 1;
                    gApPenDown = 0x30;
                    gPenPackageLen = 15;
                    isAP10NewAgreement = true;

                    if (len > gPenPackageLen) {
                        for(int i = 0; i < slotLen; i++) {
                            if (buf[0] == gFingerAP9ReportId || buf[0] == gFingerAP10NewReportId) {
                                buf[0] = gFingerReportId;
                            }
                            if (buf[dateLen * i + 1] == gFingerUp && mCacheTouchData.points[((dateLen * buf[dateLen *i + 2]) - startPointId) + 1] == 0xFF) {
                                buf[dateLen * i + 1] = gFingerDown;
                                cacheFingerTouchStatus.isDown[buf[dateLen *i + 2] + 1] = true;
                            } else if (buf[dateLen * i + 1] == gFingerUp && mCacheTouchData.points[((dateLen * buf[dateLen *i + 2]) - startPointId) + 1] != 0xFF) {
                                buf[dateLen * i + 1] = gFingerUp;
                                cacheFingerTouchStatus.isDown[buf[dateLen *i + 2] + 1] = false;
                            }
                        }
                    } else {
                        if (buf[0] == gApPenDown && buf[1] != gEraserDown && buf[1] != gAPEraseMove && buf[2] == gAPSmallPen && !checkEraseStatus) {
                             buf[0] = gPenReportIdMin;
                             checkPenStatus = true;
                        } else if (buf[0] == (gApPenDown + 1) && buf[1] != gAPEraseDown && buf[1] != gAPEraseMove && buf[2] == gAPSmallPen && !checkEraseStatus && checkPenStatus) {
                             buf[0] = gPenReportIdMin + 1;
                        } else if (buf[0] == gApPenDown && (buf[1] == gAPEraseDown || buf[1] == gAPEraseMove) && buf[2] > gAPSmallPen) {
                             buf[0] = gEraserReportId;
                             checkEraseStatus = true;
                        } else if (buf[0] == gApPenDown && buf[1] == gAPEraseUp && checkEraseStatus && buf[2] > gAPSmallPen) {
                             buf[0] = gEraserReportId;
                             checkEraseStatus = false;
                        } else {
                             buf[0] = gBufErrorStatus;
                        }
                    }
               }
           } 

            int flag = 0;
            mCacheData[0] = buf[0];
            mCacheData[1] = buf[1];

            if ((buf[0] == gPenReportIdMin) && ((mCacheTouchData.penIsDown[0] + mCacheTouchData.penIsDown[1]) <= 1) && (mCacheTouchData.mapPenId[0] > 0 && mCacheTouchData.mapPenId[1] > 0) && (mCacheTouchData.mapPenId[0] < maxAndroidTouchPointNum && mCacheTouchData.mapPenId[1] < maxAndroidTouchPointNum)) {
                flag = 1;
                allTouchPointUp();
                parseAndReportTouch();
                resetTouchCacheData();
            }

            if (buf[0] == gPenReportIdMin && ((mCacheTouchData.penIsDown[0] + mCacheTouchData.penIsDown[1]) > 1) && (mCacheTouchData.mapPenId[0] == 0xFF || mCacheTouchData.mapPenId[1] == 0xFF)) {
                flag = 1;
                allTouchPointUp();
                parseAndReportTouch();
                resetTouchCacheData();
            }

            if(buf[0] == gPenReportIdMin && (mCacheTouchData.penIsDown[0] == 1) && flag < 1)
            {
                //read pen 0x10,it had cached pen 0x10
                parseAndReportTouch();
                cacheTouchData(buf,len);
            }
            else if((buf[0] == gPenReportIdMin+1) && (mCacheTouchData.penIsDown[1] == 1)
                     && (mCacheTouchData.penIsDown[0] == 0) && flag < 1)
            {
                //read pen 0x11, it had cached pen 0x11 but no pen 0x10
                parseAndReportTouch();
                cacheTouchData(buf,len);
            }
            else if((buf[0] == gFingerReportId) && mCacheTouchData.haveFingerDown
                     && (buf[gValidPointNumPos] >0)
                     && (mCacheTouchData.penIsDown[0] == 0)
                     && (mCacheTouchData.penIsDown[1] == 0) && flag < 1)
                     //&& (mCacheTouchData.penIsDown[2] == 0)
                     //&& (mCacheTouchData.penIsDown[3] == 0))
            {

                //had cached finger data and only have finger data,read the first finger package again
                parseAndReportTouch();
                cacheTouchData(buf,len);
            }
            else if((buf[0] == gEraserReportId) && (mCacheTouchData.penIsDown[4] == 1) && flag < 1)
            {
                parseAndReportTouch();
                cacheTouchData(buf,len);
            }
            else if((buf[0] == gFingerReportId
                     || (buf[0] >= gPenReportIdMin && buf[0] < gPenReportIdMin+gPenNumMax)
                     || buf[0] == gEraserReportId) && flag < 1)
            {
                cacheTouchData(buf,len);
            }

            flag = 0;
        }
    }


    return (void *)0;
}



void initTouchProcess(void)
{
    int ret;
    bool bRet = false;

    resetTouchCacheData();
    detectAndConnectTouchDevice();
    createTouchScreen();

    pthread_attr_t process_attr;
    pthread_attr_init(&process_attr);
    pthread_attr_setdetachstate(&process_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&sthread, &process_attr, touchDataProcess_thread, (void *)NULL) != 0)
    {
        ALOGE("Failed to create TouchDataProcess thread!!!!!!!");
        return ;
    }

    /*pthread_attr_t process_attrHid;
    pthread_attr_init(&process_attrHid);
    pthread_attr_setdetachstate(&process_attrHid, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&sthreadHid, &process_attrHid, touchNotifyAddWatch, (void *)NULL) != 0) {
        ALOGE("pthread_create touchNotifyAddWatch failed");
        return;
    }*/

    property_set("vendor.xbh.touchDataprocess.running","true");
    ALOGE("create Touch Data Process ok.");

    property_set(PROP_TOUCHPROCESS_VERSION,"2025.04.14");
    ALOGE("initTouchProcess ok.");

}
