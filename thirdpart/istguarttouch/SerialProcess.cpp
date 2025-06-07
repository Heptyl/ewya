#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "SerialProcess.h"
#include <utils/Log.h>

#define TAG "xbh_mw@SerialProcess_istuarttouch"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)

int mFd = -1;
static speed_t getBaudrate(int baudrate)
{
    switch (baudrate)
    {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return B0;
    }
}

int openSerialPort(const char *path, int baudrate)
{
    mFd = open(path, O_RDWR | O_NOCTTY /* | O_NONBLOCK */);
    if (-1 == mFd)
    {
        LOGE("TouchProcess  Can't Open Serial Port!!!  errno = %d", errno);
        return -1;
    }

    speed_t speed;
    struct termios cfg;
    int ret = 0;
    speed = getBaudrate(baudrate);
    if (speed == B0)
    {
        LOGE("TouchProcess  Can't getBaudrate");
        close(mFd);
        mFd = -1;
        return -1;
    }

    ret = tcgetattr(mFd, &cfg);
    if (ret != 0)
    {
        close(mFd);
        mFd = -1;
        return -1;
    }

    cfmakeraw(&cfg);
    memset(&cfg, 0, sizeof(struct termios));
    cfg.c_cflag = speed | CS8 | CREAD | CLOCAL;
    cfg.c_iflag = IGNPAR;
    cfg.c_oflag = 0;
    cfg.c_lflag = 0;
    cfg.c_cc[VINTR] = 0;
    cfg.c_cc[VQUIT] = 0;
    cfg.c_cc[VERASE] = 0;
    cfg.c_cc[VKILL] = 0;
    cfg.c_cc[VEOF] = 0;
    cfg.c_cc[VTIME] = 0;
    cfg.c_cc[VMIN] = 1;
    cfg.c_cc[VSWTC] = 0;
    cfg.c_cc[VSTART] = 0;
    cfg.c_cc[VSTOP] = 1;
    cfg.c_cc[VSUSP] = 0;
    cfg.c_cc[VEOL] = 0;
    cfg.c_cc[VREPRINT] = 0;
    cfg.c_cc[VDISCARD] = 0;
    cfg.c_cc[VWERASE] = 0;
    cfg.c_cc[VLNEXT] = 0;
    cfg.c_cc[VEOL2] = 0;

    tcflush(mFd, TCIFLUSH);
    ret = tcsetattr(mFd, TCSANOW, &cfg);
    if (ret != 0)
    {
        close(mFd);
        mFd = -1;
        LOGE("TouchProcess  Can't tcsetattr");
        return -1;
    }

    LOGI("openSerialPort:  %s, baudrate = %d ok", path, baudrate);
    return mFd;
}

// unsigned char Data[67];
int checksum = 0;
int sendTouchData(unsigned const char *pbuf, int len)
{
    int ret = -1;
    if (mFd != -1)
    {
        ret = write(mFd, pbuf, len);
        if (ret <= 0)
        {
            LOGE("sendTouchData (mFd = %d ) write error!!!  ret = %d, errno = %d \n", mFd, ret, errno);
        }
    }
    return ret;
}

void log_cmd(const unsigned char *cmd, int len)
{
    char output[64 * 4 + 1] = {0}; // 分配内存：64字节 * 4字符 + 1终止符

    const char hex[] = "0123456789ABCDEF"; // 十六进制字符表

    for (int i = 0; i < len; ++i)
    {
        unsigned char byte = cmd[i];             // 处理为无符号字节
        unsigned char high = (byte >> 4) & 0x0F; // 高四位
        unsigned char low = byte & 0x0F;         // 低四位

        // 填充到目标字符串
        output[i * 4] = '\\';
        output[i * 4 + 1] = 'x';
        output[i * 4 + 2] = hex[high];
        output[i * 4 + 3] = hex[low];
    }

    LOGI("echo -en  \"%s\" > /dev/ttyS", output);
    return;
}

int sendCommand(const unsigned char *cmd, int len)
{
    // log_cmd(cmd, len);
    if (mFd != -1)
    {
        int ret = write(mFd, cmd, len);
        if (ret <= 0)
        {
            LOGE("sendCommand  error!!!  errno = %d \n", errno);
            return -1;
        }
        // else
        //{
        //     int i = 0;
        //     LOGI("sendCommand: write ret = %d\n",ret);
        //     for(i = 0;i< len; i++)
        //     {
        //         LOGI("%02x\n",cmd[i]);
        //     }
        // }
        return 0;
    }
    return -1;
}

int readCommand(unsigned char *pbuf, int len)
{
    int ret = -1;

    if (mFd != -1)
    {
        ret = read(mFd, pbuf, len);
    }
    return ret;
}

ssize_t tread(int fd, unsigned char *buf, size_t nbytes, unsigned int timout)
{
#if 0
    int nfds;
    fd_set  readfds;
    struct timeval  tv;
    tv.tv_sec = timout;
    tv.tv_usec = 0;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    nfds = select(fd+1, &readfds, NULL, NULL, &tv);
    if (nfds <= 0)
    {
        if (nfds == 0)
        {
            //LOGI("treadn: error, ETIME\n"); 
            errno = ETIME;
        }
        return(-1);
    }
#endif
    return (read(fd, buf, nbytes));
}

ssize_t treadn(int fd, unsigned char *buf, size_t nbytes, unsigned int timout)
{
    size_t nleft;
    ssize_t nread;
    nleft = nbytes;
    while (nleft > 0)
    {
        if ((nread = tread(fd, buf, nleft, timout)) < 0)
        {
            if (nleft == nbytes)
            {
                LOGI("treadn: error, return -1\n");
                return (-1); /* error, return -1 */
            }
            else
            {
                LOGI("treadn: error, return amount read so far\n");
                break; /* error, return amount read so far */
            }
        }
        else if (nread == 0)
        {
            LOGI("treadn: error, EOF\n");
            break; /* EOF */
        }
        nleft -= nread;
        buf += nread;
    }
    return (nbytes - nleft); /* return >= 0 */
}