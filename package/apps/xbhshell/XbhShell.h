#ifndef __XBH_SHELL__
#define __XBH_SHELL__

#include <log/log.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <jni.h>
#include <cutils/sockets.h>
#include <sys/wait.h>
#include <sys/un.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  "XBH_Shell", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "XBH_Shell", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "XBH_Shell", __VA_ARGS__)

#define SOCKET_NAME "xbhshell_server"

//端口
#define PORT 35468

int executeShell(const char* shell);
int readn(int fd, void *buf, int count);
void fasync_init();
int uinput_ir_init();
void send_ir_keycode(int keyCode, int down);
int uinput_keypad_init();
void send_keypad_keycode(int keyCode, int down);
#endif