#ifndef _Included_SerialProcess
#define _Included_SerialProcess
#ifdef __cplusplus
extern "C" {
#endif

bool openSerialPort(const char *path,int baudrate);
int sendTouchData(unsigned const char* pbuf,int len);
int sendCommand(const unsigned char* cmd,int len);
int readCommand(unsigned char* pbuf, int len);
int getSerialFd(void);

#ifdef __cplusplus
}
#endif
#endif
