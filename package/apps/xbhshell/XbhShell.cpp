#include "XbhShell.h"
/**
 * 帧结构
 * 包头4个字节
 * 0xf0
 * 0x0f
 * 0xfe
 * 0x01
 * 数据长度2个字节, len = 0x01 << 8 + 0x02
 * 0x01
 * 0x02
 *
 * data ...
 *
 * 结尾
 * 0xCC
*/

//接收到的命令, 限制命令长度为1024个字符以内
char recvbuf[1024];
//shell 返回的命令
char* g_RecBuf;

//readn函数包装了read函数，用于读取定长包
int readn(int fd, void *buf, int count)
{
    int nleft = count;//所要读取的字节数
    int nread;
    char *bufp = (char *)buf;//指向buf缓冲区

    while (nleft > 0)//读取数据，若读取到buf的字节与请求的字节相同，则直接返回请求字节数
    {
        if ((nread = read(fd, bufp, nleft)) < 0)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }

        else if (nread == 0) //对方关闭或者已经读到eof
            return count - nleft;//返回实际读取的字节数，在此应该小于请求读取的字节数

        bufp += nread;
        nleft -= nread;
    }
    return count;
}

#define CHUNK_SIZE 4096  // 每次读取的块大小（4KB）
int executeShell(const char* shell)
{
    LOGD("XBH-SHELL excute shell--->>:%s ", shell);
    FILE *pp = popen(shell, "r");
    if (!pp) {
        LOGD("XBH-SHELL execute shell:%s fail!", shell);
        return 0;
    }
    char *resbuf = NULL;    // 动态缓冲区
    size_t buf_size = 0;    // 缓冲区当前容量
    size_t total_read = 0;  // 已读取的总字节数
    int ret;                // fread返回值

    while (1)
    {
        // 检查是否需要扩展缓冲区
        if (total_read >= buf_size)
        {
            // 初始分配或翻倍扩容（减少realloc次数）
            size_t new_size = (buf_size == 0) ? CHUNK_SIZE : buf_size * 2;
            char *new_buf = (char*)realloc(resbuf, new_size);
            if (!new_buf)
            {
                LOGD("XBH-SHELL realloc failed: %s", strerror(errno));
                pclose(pp);
                free(resbuf);
                return 0;
            }
            resbuf = new_buf;
            buf_size = new_size;
        }

        // 读取一块数据（从当前位置开始，读取CHUNK_SIZE字节）
        ret = fread(resbuf + total_read, 1, CHUNK_SIZE, pp);
        LOGD("XBH-SHELL ret = %d", ret);
        if (ret == 0)
        {
            if (ferror(pp))
            {  // 读取错误
                LOGD("XBH-SHELL fread error: %s", strerror(errno));
                pclose(pp);
                free(resbuf);
                return 0;
            }
            else
            {
                // 正常结束（到达EOF）
                break;
            }
        }
        total_read += ret;
    }
    LOGD("XBH-SHELL total_read = %zu", total_read);
    if(total_read > 0)
    {
        g_RecBuf = new char[total_read];
        memset(g_RecBuf, 0, total_read);
        memcpy(g_RecBuf, resbuf, total_read);
        //LOGD("XBH-SHELL excute end--->>:%s ", g_RecBuf);
    }
    //清理资源
    pclose(pp);       // 等待命令结束并获取退出状态
    free(resbuf);     // 释放动态缓冲区
    return total_read;
}

int main()
{
    fasync_init();
    uinput_ir_init();
    uinput_keypad_init();

    LOGD("shell srtart !!!!!!");
    int max_connect_number = 4;
    int listen_fd = -1;
    int client_sockfd = -1;
    int result;
    
    struct sockaddr addr;
    socklen_t alen;
    alen = sizeof(addr);

    //获取init.rc里面配置的名为xbhshell_server的socket
    listen_fd = android_get_control_socket(SOCKET_NAME);
    if(listen_fd < 0)
    {
        LOGE("Failed to get socket '" SOCKET_NAME "' errno:%d \n", errno); 
        exit(-1);
    }
    LOGD("android_get_control_socket success\n");
    //开始监听xbhshell_server，并设置最大监听数
    result = listen(listen_fd, max_connect_number);
    if(result < 0 )
    {
        LOGE("listen error\n");
        exit(-1);
    }
    LOGD("listen !!!!!!");
    fcntl(listen_fd, F_SETFD, FD_CLOEXEC);
    //等待Socket客户端发启连接请求
    while(1)
    {
        LOGD("accept !!!!!!");
        if((client_sockfd = accept(listen_fd, &addr, &alen) ) < 0)
        {
            LOGD("connect error");
        }
        else
        {
            fcntl(client_sockfd, F_SETFD, FD_CLOEXEC);
            LOGD("create connection successfully\n");
            int sendLen = 0;
            //读取帧头6个字节
            int size = readn(client_sockfd, recvbuf, 6);
            //LOGD("readn size = %d \n", size);
            if(size == 6)
            {
                //获取data数据的长度
                int len = (recvbuf[4] << 8) + recvbuf[5];
                //LOGD("readn len = %d ", len);
                memset(recvbuf, 0 ,sizeof(recvbuf));
                //读取data 数据和结束符
                int len2 = readn(client_sockfd, recvbuf, len + 1);
                //LOGD("readn len2 = %d ", len2);
                if(len2 == (len + 1))
                {
                    if(recvbuf[len] == 0xCC)
                    {
                        //将结束符设置为\0
                        recvbuf[len] = '\0';
                        //执行shell
                        sendLen = executeShell(recvbuf);
                        LOGD("readn sendLen = %d ", sendLen);
                    }
                    else
                    {
                        LOGD("readn recvbuf[%d] = %c ", len, recvbuf[len]);
                    }
                }
            }

            //设置所有数据的长度 data + 7个字节
            char* tt = new char[sendLen + 7];
            //设置帧头和结束符
            tt[0] = 0xf0;
            tt[1] = 0x0f;
            tt[2] = 0xfe;
            tt[3] = 0x01;
            tt[4] = sendLen >> 8;
            tt[5] = sendLen & 0xff;
            LOGD("send msg sendLen = %d tt[4] = %d tt[5] = %d \r\n", sendLen, tt[4], tt[5]);
            if(sendLen > 0)
            {
                //从第6个字节开始写入data
                memcpy(tt + 6, g_RecBuf, sendLen);
            }
            tt[sendLen + 6] = 0xCC;
            LOGD("send msg to client begin");
            if(send(client_sockfd, tt, sendLen + 7, 0) == -1)
            {
                LOGD("send msg to client erorr !!!!!!");
            }
            LOGD("send msg to client end");
            delete[] tt;
            tt = nullptr;
            delete[] g_RecBuf;
            g_RecBuf = nullptr;
            //delay 100ms
            usleep(1000 * 100);
            close(client_sockfd);
            LOGD("over!! ");
        }
    }
    close(listen_fd);
    return 0;
}
