#ifndef _Included_SerialProcess
#define _Included_SerialProcess
#ifdef __cplusplus
extern "C" {
#endif

int openSerialPort(const char *path,int baudrate);
int sendTouchData(unsigned const char* pbuf,int len);
int sendCommand(const unsigned char* cmd,int len);
int readCommand(unsigned char* pbuf, int len);
ssize_t treadn(int fd, unsigned char *buf, size_t nbytes, unsigned int timout);

#ifdef __cplusplus
}
#endif
#endif
