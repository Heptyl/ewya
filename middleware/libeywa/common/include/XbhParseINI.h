#ifndef INI_PARSE_H
#define INI_PARSE_H

#include "XbhLog.h"
#include <cutils/properties.h>
#include <string>
#include <vector>

#define MAX_LINE_LENGTH 256

typedef struct {
    char section[50];
    char key[50];
    char value[50];
} IniEntry;

class XbhParseINI {
public:
    XbhParseINI();
    ~XbhParseINI();
    static IniEntry* parseINI(const char* path, int* count);
};

#endif // SYS_WRITE_H