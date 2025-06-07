#ifndef SYS_WRITE_H
#define SYS_WRITE_H

#include "XbhLog.h"
#include <cutils/properties.h>
#include <string>
#include <vector>
#include "XbhMutex.h"

#define MAX_STR_LEN         4096
class XbhSysOpt {
public:
    XbhSysOpt();
    ~XbhSysOpt();
    static std::vector<std::string> ls(const std::string &path);
    static XBH_BOOL getProperty(const std::string& key, std::string& value);
    static XBH_BOOL getPropertyString(const std::string&key, std::string& value, const std::string& def);
    static XBH_U32 getPropertyInt(const std::string& key, int32_t def);
    static XBH_U64 getPropertyLong(const std::string& key, int64_t def);
    static XBH_BOOL getPropertyBoolean(const std::string& key, bool def);
    static XBH_VOID setProperty(const std::string& key, const std::string& value);
    static XBH_BOOL readSysfs(const std::string& path, std::string& value);
    static XBH_BOOL writeSysfs(const std::string& path, const std::string& value);
    static std::string trimString(const std::string& str);
    static XbhSysOpt* getInstance();
    // 设置byte中第n位为1
    static XBH_U8 set_bit(XBH_U8& byte, XBH_U8 n);
    // 清除byte中第n位
    static XBH_U8 clear_bit(XBH_U8& byte, XBH_U8 n);

private:
    static XBH_BOOL writeSys(const char *path, const char *val, const int size);
    static XBH_VOID readSys(const char *path, char *buf, int& count, bool needOriginalData);
    static XBH_U32 readSys(const char *path, char *buf, int count);

    static XbhSysOpt*   mInstance;
    static XbhMutex     mLock;
};

#endif // SYS_WRITE_H
