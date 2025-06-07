#define LOG_NDEBUG 0
#define XBH_LOG_TAG "xbh_mw@XbhSysOpt"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "XbhSysOpt.h"

XbhSysOpt*              XbhSysOpt::mInstance = NULL;
XbhMutex                XbhSysOpt::mLock;

XbhSysOpt::XbhSysOpt()
{
}

XbhSysOpt::~XbhSysOpt()
{
}

XbhSysOpt* XbhSysOpt::getInstance()
{
    XbhMutex::Autolock _l(mLock);
    if (!mInstance)
    {
        mInstance = new XbhSysOpt();
    }
    return mInstance;
}

std::vector<std::string> XbhSysOpt::ls(const std::string &path)
{
    std::vector<std::string> dirfiles;
    DIR *pDirOfPath = nullptr;
    pDirOfPath = opendir(path.c_str());
    if(!pDirOfPath)
    {
        return dirfiles;
    }
    struct dirent * ptr = nullptr;
    while((ptr = readdir(pDirOfPath)) != nullptr)
    {
        dirfiles.push_back(ptr->d_name);
    }
    closedir(pDirOfPath);
    return dirfiles;
}

XBH_BOOL XbhSysOpt::getProperty(const std::string& key, std::string& value)
{
    return getPropertyString(key, value, "");
}

XBH_BOOL XbhSysOpt::getPropertyString(const std::string& key, std::string& value, const std::string& def)
{
    char val[PROPERTY_VALUE_MAX]={0};
    property_get(key.c_str(), val, def.c_str());
    value = std::string(val);
    return XBH_TRUE;
}

XBH_U32 XbhSysOpt::getPropertyInt(const std::string& key, int32_t def)
{
    int64_t val = getPropertyLong(key,(int64_t)def);
    return static_cast<int32_t>(val);
}

XBH_U64 XbhSysOpt::getPropertyLong(const std::string& key, int64_t def)
{
    int len;
    char buf[PROPERTY_VALUE_MAX] = {0};
    char* end;
    int64_t result = def;
    len = property_get(key.c_str(), buf, "");
    if (len > 0)
    {
        result = strtoll(buf, &end, 0);
        if (end == buf)
        {
            result = def;
        }
    }
    return result;
}

XBH_BOOL XbhSysOpt::getPropertyBoolean(const std::string& key, bool def)
{
    int len;
    char buf[PROPERTY_VALUE_MAX] = {0};
    bool result = def;
    len = property_get(key.c_str(), buf, "");
    if (len == 1)
    {
        char ch = buf[0];
        if (ch == '0' || ch == 'n')
            result = false;
        else if (ch == '1' || ch == 'y')
            result = true;
    }
    else if (len > 1) 
    {
        if (!strcmp(buf, "no") || !strcmp(buf, "false") || !strcmp(buf, "off"))
        {
            result = false;
        }
        else if (!strcmp(buf, "yes") || !strcmp(buf, "true") || !strcmp(buf, "on"))
        {
            result = true;
        }
    }
    return result;
}

XBH_VOID XbhSysOpt::setProperty(const std::string& key, const std::string& value)
{
    int err = property_set(key.c_str(), value.c_str());
    if (err < 0) 
    {
        XLOGE("failed to set system property %s\n", key.c_str());
    }
}

XBH_BOOL XbhSysOpt::readSysfs(const std::string& path, std::string& value)
{
    char buf[MAX_STR_LEN + 1] = {0};
    int count = readSys(path.c_str(), (char*)buf, MAX_STR_LEN);
    if(count < 0)
    {
        return false;
    }
    value = std::string(buf,count);
    return true;
}

XBH_BOOL XbhSysOpt::writeSysfs(const std::string& path, const std::string& value)
{
    return writeSys(path.c_str(), value.c_str(), value.length());
}

XBH_BOOL XbhSysOpt::writeSys(const char *path, const char *val, const int size)
{
    int fd = -1;
    if(path == nullptr || val == nullptr || size <= 0)
    {
        return false;
    }
    if ((fd = open(path, O_WRONLY)) < 0) 
    {
        XLOGE("writeSysFs, open %s fail.", path);
        return false;
    }
    int writedsize = 0;
    while(writedsize < size)
    {
        auto len = write(fd, val + writedsize, size - writedsize);
        if(len <= 0)
        {
            break;
        }
        writedsize += len;
    }
    close(fd);
    return (writedsize==size);;
}

XBH_U32 XbhSysOpt::readSys(const char *path, char *buf, int count)
{
    int fd, len = -1;

    if ( NULL == buf )
    {
        XLOGE("buf is NULL");
        return len;
    }

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        XLOGE("readSys, open %s fail. Error info [%s]", path, strerror(errno));
        return len;
    }

    len = read(fd, buf, count);

    if (len < 0)
    {
        XLOGE("read error: %s, %s\n", path, strerror(errno));
    }
    close(fd);
    return len;
}

XBH_VOID XbhSysOpt::readSys(const char *path, char *buf, int& count, bool needOriginalData) 
{
    int fd = -1, len;
    int buffersize = count;
    count = 0;
    if ( NULL == buf ) 
    {
        XLOGE("buf is NULL");
        return;
    }

    if ((fd = open(path, O_RDONLY)) < 0) 
    {
        XLOGE("readSysFs, open %s fail. Error info [%s]", path, strerror(errno));
        goto exit;
    }

    len = read(fd, buf, buffersize);

    if (len < 0) 
    {
        XLOGE("read error: %s, %s\n", path, strerror(errno));
        goto exit;
    }
    count = len;
    if (!needOriginalData) 
    {
        int i , j;

        for (i = 0, j = 0; i <= len - 1; i++) 
        {
            /*change '\0' to 0x20(spacing), otherwise the string buffer will be cut off
             * if the last char is '\0' should not replace it
             */
            if (0x0 == buf[i] && i < len - 1) 
            {
                buf[i] = 0x20;
                XLOGD("read buffer index:%d is a 0x0, replace to spacing \n", i);
            }

            /* delete all the character of '\n' */
            if (0x0a != buf[i]) 
            {
                buf[j++] = buf[i];
            }
        }
        buf[j] = 0x0;
        count = j;
    }
    XLOGI("read %s, result length:%d, val:%s\n", path, len, buf);
exit:
    close(fd);
}

/**
 * 去除字符串尾部的空格和换行符
 * str 输入字符串
 */
std::string XbhSysOpt::trimString(const std::string& str)
{
    std::string trimmed = str;
    trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), trimmed.end());
    return trimmed;
}

// 设置byte中第n位为1
XBH_U8 XbhSysOpt::set_bit(XBH_U8& byte, XBH_U8 n)
{
    byte = byte | (1 << n);
    return byte;
}

// 清除byte中第n位
XBH_U8 XbhSysOpt::clear_bit(XBH_U8& byte, XBH_U8 n)
{
    byte = byte & ~(1 << n);
    return byte;
}


