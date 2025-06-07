#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "SerialProcess.h"

#include <utils/Log.h>

#define TAG "TouchProcess"
#define LOGD(...) //__android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // ����LOGD����
#define LOGI(...) //__android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // ����LOGI����
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // ����LOGI����

//#define DEBUG_DATA2MCU
#define TEST_SERIAL_READ_WRITE  0

int sendTouchData(unsigned const char* pbuf,int len);

int mFd = -1;


#if TEST_SERIAL_READ_WRITE
long totalSize = 0;
void *seiral_test_thread(void *arg)
{
    unsigned char data[64] = {0xAB,0xAC,0xAD,0xAE,0x00,0x7F,0xFF,0xCF};
    int ret;

    while (1)
    {
      	ret = sendTouchData(data,64);
		if(ret>0)
		{
			totalSize += ret;
			LOGE("write(mFd:%d)  %d  ok,  totalSize = %ld",mFd, ret,totalSize);
		}
        usleep(10*1000);
    }

   
    return NULL;
}
#endif


static speed_t getBaudrate(int baudrate)
{
    switch(baudrate) {
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


/*
 * Class:     android_serialport_SerialPort
 * Method:    open
 * Signature: (Ljava/lang/String;II)Ljava/io/FileDescriptor;
 */
bool openSerialPort(const char *path,int baudrate)
{
	mFd = open(path, O_RDWR|O_NOCTTY|O_NONBLOCK);
	if (-1 == mFd)
	{   
		LOGE("TouchProcess  Can't Open Serial Port!!!  errno = %d",errno);
		return false;
	}   
	
    speed_t speed;
    struct termios cfg;
    int ret = 0;
    speed = getBaudrate(baudrate);
    if (speed == B0)
    {
		LOGE("TouchProcess  Can't getBaudrate");   
		return false;   
    }

    ret = tcgetattr(mFd, &cfg);
    if (ret != 0)
    {
        close(mFd);
        mFd = -1;
        return false;
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
		return false;
    }

	LOGE("openSerialPort:  %s, baudrate = %d ok",path,baudrate);

	#if TEST_SERIAL_READ_WRITE
		pthread_t tesstThread;
	    pthread_attr_t process_attr;
	    pthread_attr_init(&process_attr);
	    pthread_attr_setdetachstate(&process_attr, PTHREAD_CREATE_DETACHED);

	    if (pthread_create(&tesstThread, &process_attr, seiral_test_thread, (void *)NULL) != 0)
	    {
	        LOGI("Failed to create seiral_test_thread thread!!!!!!!");
	    }
	
	#endif

    return true;
}

//unsigned char Data[67];
int checksum = 0;
int sendTouchData(unsigned const char* pbuf,int len)
{
	int ret = -1;
	if(mFd != -1)
    {
        ret = write(mFd,pbuf,len);
        if(ret <= 0)
        {
            LOGE("sendTouchData (mFd = %d ) write error!!!  ret = %d, errno = %d \n",mFd,ret,errno); 
        }
		#ifdef DEBUG_DATA2MCU
		else
		{
			LOGE("write uart ok(mFd = %d). ret=%d",mFd,ret);
		}
		#endif
    }
	return ret;
}

int sendCommand(const unsigned char* cmd,int len)
{
	
	if(mFd != -1)
    {
		int ret = write(mFd,cmd,len);
	    if(ret <= 0)
	    {
	        LOGE("sendCommand  error!!!  errno = %d \n",errno); 
			return -1;
	    }else{
	    	int i = 0;
			LOGI("sendCommand: write ret = %d\n",ret); 
			for(i = 0;i< len; i++){
				LOGI("%02x\n",cmd[i]); 
			}
		}

		return 0;
	}

	return -1;
}

int readCommand(unsigned char* pbuf, int len)
{
    int ret = -1;

    if(mFd != -1)
    {
        ret = read(mFd, pbuf, len);
    }
    return ret;
}

int getSerialFd(void)
{
	return mFd;
}



