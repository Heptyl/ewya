#include <utils/Log.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string>

#define TAG "xbh_mw@K2_daemon"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)

int main(int argc, char **argv)
{
    int status = 0;
    if (argc < 2)
    {
        LOGD("lack of params, please start as: ./k2_daemon /vendor/bin/k2_touch/k2_ir_touch \n");
        exit(1);
    }
    
    //LOGD("argv[1]:%s\n",argv[1]);
    char *program = strrchr(argv[1], '/'); 
    if (program == NULL)
    {
        LOGD("err params, please start as: ./k2_daemon /vendor/bin/k2_touch/k2_ir_touch \n");
        exit(1);
    }
    program +=1;
    //LOGD("program:%s\n",program);

    while (true)
    {
        pid_t pid = fork();
        if (pid < 0) {
            LOGD("WatchDog fork err=%d:%s\n",errno, strerror(errno));
            sleep(5);
            continue;
        }
        if (pid == 0) {
            LOGD("[WatchDog] StartProcess %s ok!\n", argv[1]);
            if(execl(argv[1],program,(char*)NULL) == -1)
            {
                LOGD("execl failed!\n");
            }
            exit(-1);
        }
        LOGD("[WatchDog] chirend pid=%d ok!\n", pid);

        if (pid > 0) {
            waitpid(pid, &status, 0);
            LOGD("[WatchDog] pid=%d\n", pid);
            sleep(5);
        }
    }
    return 0;
}