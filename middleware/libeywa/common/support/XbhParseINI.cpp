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
#include "XbhParseINI.h"

XbhParseINI::XbhParseINI()
{
}

XbhParseINI::~XbhParseINI()
{
}

IniEntry* XbhParseINI::parseINI(const char* filePath, int* entryCount)
{
    FILE* file = fopen(filePath, "r");
    if (!file)
    {
        XLOGE("%s %d Failed to open file: %s \n", __func__, __LINE__, filePath);
        return NULL;
    }

    IniEntry* entries = NULL;
    int count = 0;
    char line[MAX_LINE_LENGTH];

    char* currentSection = NULL;

    while (fgets(line, sizeof(line), file))
    {
        // 移除行尾的换行符（LF和CRLF）
        int lineLength = strlen(line);
        if (lineLength > 0 && line[lineLength - 1] == '\n')
        {
            line[lineLength - 1] = '\0';
            if (lineLength > 1 && line[lineLength - 2] == '\r')
            {
                line[lineLength - 2] = '\0';
            }
        }

        // 解析节名
        if (line[0] == '[' && line[strlen(line) - 1] == ']')
        {
            line[strlen(line) - 1] = '\0';
            currentSection = strdup(line + 1);
            entries = (IniEntry*)realloc(entries, (count + 1) * sizeof(IniEntry));
            if (!entries)
            {
                XLOGE("%s %d realloc error !!!!!! \n", __func__, __LINE__);
                fclose(file);
                free(currentSection);
                return NULL;
            }
            strcpy(entries[count].section, currentSection);
            entries[count].key[0] = '\0';
            entries[count].value[0] = '\0';
            count++;
        }
        // 解析键值对
        else if (currentSection && strchr(line, '='))
        {
            char* separator = strchr(line, '=');
            *separator = '\0';

            char* key = strdup(line);
            char* value = strdup(separator + 1);

            // 移除键和值前后的空格
            char* keyStart = key;
            char* keyEnd = key + strlen(key) - 1;
            while (*keyStart == ' ' || *keyStart == '\t')
            {
                keyStart++;
            }
            while (*keyEnd == ' ' || *keyEnd == '\t')
            {
                *keyEnd = '\0';
                keyEnd--;
            }

            char* valueStart = value;
            char* valueEnd = value + strlen(value) - 1;
            while (*valueStart == ' ' || *valueStart == '\t')
            {
                valueStart++;
            }
            while (*valueEnd == ' ' || *valueEnd == '\t')
            {
                *valueEnd = '\0';
                valueEnd--;
            }

            strcpy(entries[count - 1].key, keyStart);
            strcpy(entries[count - 1].value, valueStart);

            free(key);
            free(value);
        }
    }

    fclose(file);
    free(currentSection);
    *entryCount = count;
    return entries;
}