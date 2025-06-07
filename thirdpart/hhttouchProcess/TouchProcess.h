#ifndef _TouchProcess_
#define _TouchProcess_
#ifdef __cplusplus
extern "C" {
#endif

void InitProcess(const char* path,  int baudrate);
int setTouchState(int type, int state);
int getTouchState(int type);
int setNonThroughTouchRegion(int id, int x, int y, int w, int h,int is4kUi);
int removeNonThroughTouchRegion(int id,int x, int y, int w, int h, int is4kUi);
int removeNonThroughTouchRegionById(int id);
int removeAllNonThroughTouchRegion();
int isTouchPanelOk();
int setScreenOffsetHV(int offsetX,int offsetY,int is4kUi);
int setPalmType(int type);




bool findHuaXin(void);
bool connectExtendTouchUSB();
bool disconnectExtendTouchUSB();
bool connectMdTouchUSB();
void *extTouchDataProcess_thread(void *arg);
void *touchDataProcess_thread(void *arg);
void *findAndOpenHidRawDeviceThread(void *arg);
void *getZaagPanelInfoThread(void *arg);
int ReadData(int fd,int n);
int EnableRawData(int fd);
int EnableRawDataWidth(int fd);
int ResetUSB(int fd);
int GetFWVersion(int fd);
bool detectAndOpenDevice(void);
char* zaag_drvinfo(int fd);
bool connectTouchPanel(void);
void getJagePanelVersionInfo();
bool executeShellCmd(const char * cmd);
bool openTTouchRawDataUsb(void);
int write_data_to_touch(uint8_t *cmd, uint8_t len);




#ifdef __cplusplus
}
#endif
#endif
