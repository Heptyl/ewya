/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/
#ifndef __HI_MW_INI_PROVIDER_H__
#define __HI_MW_INI_PROVIDER_H__

#include "hi_mw_cfg_ini.h"

#include <string>
#include <map>
#include <vector>
#include "hi_file.h"

using std::string;

class INIProvider {
public:
    HI_S32 ParseInt(const string &fil);
    Array<HI_S32> ParseIntArray(const string &fil);

    HI_DOUBLE ParseDouble(const string &fil);
    Array<HI_DOUBLE> ParseDoubleArray(const string &fil);

    HI_BOOL ParseBoolean(const string &fil);
    Array<HI_BOOL> ParseBooleanArray(const string &fil);

    string ParseString(const string &fil);
    Array<string> ParseStringArray(const string &fil);

    HI_VOID DumpCache(FILE *fp);

    // insert ATV programs into m_Cache, ATV programs store in file
    HI_S32 InsertAtvChannelList(string fileName);
    // erase ATV programs from m_Cache, ATV programs store in file
    HI_VOID EraseAtvChannelList(string fileName);

public:
    INIProvider();
    ~INIProvider();

private:
    HI_VOID LoadIniFile();
    HI_VOID InitCacheMap(string inifile);
    HI_VOID AddCacheMap(string inifile);
    HI_VOID EraseCacheMap(string inifile);
    HI_S32 CacheMapLoad(HiCfgINI *pINI, HiFile *fp);
    HI_S32 CacheMapUnload(HiCfgINI *pINI, HiFile *fp);

private:
    std::map<string, string> m_Cache;
    std::map<string, string> m_IniFile;
    std::vector<string> mIniPathVec;
};

#endif /* __HI_MW_INI_PROVIDER_H__ */
