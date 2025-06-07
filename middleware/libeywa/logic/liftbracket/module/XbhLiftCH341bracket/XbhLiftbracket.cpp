#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhLiftbracket"
#include "XbhLog.h"

#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "cutils/properties.h"
#include "errno.h"
#include <iostream>
#include <dirent.h>

#include "XbhLiftbracket.h"

int mFdm = -1;

const XBH_U8 LIFTBRACKET_CMD_COLUMN_UP[] = {0X00, 0X00, 0XAA, 0X10, 0X10, 0X00, 0X00, 0X55};                    //上升
const XBH_U8 LIFTBRACKET_CMD_COLUMN_DOWN[] = {0X00, 0X00, 0XAA, 0X10, 0X20, 0X00, 0X00, 0X55};                  //下降
const XBH_U8 LIFTBRACKET_CMD_COLUMN_STOP[] = {0X00, 0X00, 0XAA, 0X10, 0X40, 0X00, 0X00, 0X55};                  //停止
const XBH_U8 LIFTBRACKET_CMD_SET_TO_MEMORY1_COLUMN[] = {0X00, 0X00, 0XAA, 0X10, 0X31, 0X00, 0X00, 0X55};        //调整到记忆1
const XBH_U8 LIFTBRACKET_CMD_SET_TO_MEMORY2_COLUMN[] = {0X00, 0X00, 0XAA, 0X10, 0X32, 0X00, 0X00, 0X55};        //调整到记忆2
const XBH_U8 LIFTBRACKET_CMD_SET_MEMORY1_COLUMN[] = {0X00, 0X00, 0XAA, 0X10, 0X61, 0X00, 0X00, 0X55};           //设置当前高度为记忆1
const XBH_U8 LIFTBRACKET_CMD_SET_MEMORY2_COLUMN[] = {0X00, 0X00, 0XAA, 0X10, 0X62, 0X00, 0X00, 0X55};           //设置当前高度为记忆2

char product_id[10] = "1a86:7523";

XbhLiftbracket* XbhLiftbracket::mInstance = NULL;

//---------------- custom device interface start ----------------

static const char USB_DIR[] = "/sys/bus/usb/devices";

XBH_S32 XbhLiftbracket::get_usb_pvid_exist(void)
{
    DIR *dir;
    XBH_S32 ret = -1;
    struct dirent *next;
    FILE *fp = NULL;

    dir = opendir(USB_DIR);
    if (!dir)
    {
        return -1;
    }

    while ((next = readdir(dir)) != NULL)
    {
        char line[256];
        char uevent_file[256] = {0};
        sprintf(uevent_file, "%s/%s/uevent", USB_DIR, next->d_name);

        fp = fopen(uevent_file, "r");
        if (NULL == fp)
        {
            continue;
        }
        while (fgets(line, sizeof(line), fp))
        {
            char *pos = NULL;
            XBH_S32 product_vid, product_did, product_bcdev;
            char temp[10] = {0};
            pos = strstr(line, "PRODUCT=");

            if (NULL != pos)
            {
                if (sscanf(pos + 8, "%x/%x/%x", &product_vid, &product_did, &product_bcdev) <= 0)
                {
                    continue;
                }
                sprintf(temp, "%04x:%04x", product_vid, product_did);
                XLOGD("temp : %s", temp);
                if (0 == strncmp(temp, product_id, 9)) {
                ret = 1;
                break;
                }
            }
        if (ret > 0) break;
        }
        fclose(fp);
        if (ret > 0) break;
    }
    closedir(dir);

    return ret;
}

void XbhLiftbracket::init()
{

}

/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
XBH_BOOL XbhLiftbracket::openSerialPort()
{
    struct termios newtio, oldtio;
    int ret = -1;
    if (mFdm < 0) {
        mFdm=open(LIFTBRACKET_DEV, O_RDWR | O_NOCTTY);
        if (mFdm == -1) {
            return false;
        }
        if (ret == tcgetattr(mFdm,&oldtio)) {
            return false;
        }
        memcpy(&newtio, &oldtio, sizeof(newtio));
        cfmakeraw(&newtio);
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        tcflush(mFdm,TCIFLUSH);
        if ((tcsetattr(mFdm,TCSANOW,&newtio))!=0)
        {
            return false;
        }
    }

    return XBH_TRUE;
}

XBH_S32 XbhLiftbracket::readCommand(int fd, XBH_U8* pbuf, XBH_S32 len)
{
    XBH_S32 ret = XBH_FAILURE;

    XLOGD("readCommand mFd = %d\n", fd);

    if(fd != XBH_FAILURE)
    {
        ret = read(fd, pbuf, len);
        XLOGD("readCommand ret = %d errno = %s\n", ret, strerror(errno));
    }

    return ret;
}

XBH_U8 XbhLiftbracket::sendCmd(int fd, const XBH_U8* cmd, int len){
    int ret = -1;
    if(fd != -1){
        ret = write(fd, cmd, len);
        if(ret < 0)
            XLOGE("Send cmd fail");
    }
    return ret;

}

XBH_S32 XbhLiftbracket::setLiftbracketOperate(int operat)
{
    int ret = -1;
    if (mFdm < 0)
    {
        openSerialPort();
    }
    if (mFdm > 0)
    {
        switch (operat)
        {
            case LIFTBRACKET_UP:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_COLUMN_UP, 8);
                break;
            case LIFTBRACKET_DOWM:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_COLUMN_DOWN, 8);
                break;
            case LIFTBRACKET_STOP:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_COLUMN_STOP, 8);
                break;
            case LIFTBRACKET_SET_TO_MEMORY1:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_SET_TO_MEMORY1_COLUMN, 8);
                break;
            case LIFTBRACKET_SET_TO_MEMORY2:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_SET_TO_MEMORY2_COLUMN, 8);
                break;
            case LIFTBRACKET_SET_MEMORY1_COLUMN:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_SET_MEMORY1_COLUMN, 8);
                break;
            case LIFTBRACKET_SET_MEMORY2_COLUMN:
                ret = sendCmd(mFdm, LIFTBRACKET_CMD_SET_MEMORY2_COLUMN, 8);
                break;
            default:
                break;
        }
        close(mFdm);
        mFdm = -1;
    }
    else
    {
        XLOGD("%s open dev error!!!", __FUNCTION__);
    }
    return ret;
}

XBH_S32 XbhLiftbracket::getLiftbracketExist(int *status)
{
    if (get_usb_pvid_exist() > 0)
    {
        if (access(LIFTBRACKET_DEV, F_OK) == 0)
        {
            *status = 1;
            return XBH_TRUE;
        }
        else
        {
            XLOGD("%s dev is not exist!!!", __FUNCTION__);
            return XBH_FALSE;

        }
    }
    else
    {
        XLOGD("%s pid vid is not exist!!!", __FUNCTION__);
        return XBH_FALSE;
    }
    *status = 0;
    return XBH_TRUE;
}

XbhLiftbracket::XbhLiftbracket()
{

}

XbhLiftbracket::~XbhLiftbracket()
{

}


XbhLiftbracket* XbhLiftbracket::getInstance()
{
    if (!mInstance)
    {
        mInstance = new XbhLiftbracket();
    }
    return mInstance;
}

