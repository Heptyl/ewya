/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_ini_provider"

#include "hi_mw_ini_provider.h"

#include <ctype.h>
#include "hi_mw_log.h"
#include "hi_mw_common.h"
//#include "HiCustUtils.h"

#define MAX_PATH_LEN 128

static const HI_U32 STR_MAX = 256;

// cfg file prefix of cust or vendor
const HI_CHAR* g_CfgFiles[] = {
    "/cust",
    "/vendor",
};

static const int CFG_FILES_COUNT =
    (sizeof(g_CfgFiles) / sizeof(g_CfgFiles[0]));

// get the highest priority cfg file
static HI_CHAR* GetCfgFile(const HI_CHAR *pathSuffix, HI_CHAR buf[MAX_PATH_LEN])
{
    if ((buf == NULL) || (pathSuffix == NULL)) {
        return nullptr;
    }
    *buf = '\0';
    for (int i = 0; i < CFG_FILES_COUNT; i++) {
        if (snprintf_s(buf, MAX_PATH_LEN, MAX_PATH_LEN - 1, "%s/%s", g_CfgFiles[i], pathSuffix) == -1) {
            LOGE("failed to snprintf_s buf\n");
            *buf = '\0';
            continue;
        }

        if (access(buf, F_OK) == 0) {
            break; // find it
        }
        *buf = '\0'; // not found, destroy the path
    }
    return (*buf != '\0') ? buf : nullptr;
}


static HI_S32 RemoveSpace(HI_CHAR *strDest, HI_U32 length)
{
    if (strDest == HI_NULL || !strlen(strDest)) {
        return HI_FAILURE;
    }

    HI_CHAR *p = strDest;
    HI_CHAR *q = strDest;

    while (isspace(*p)) {
        p++;
    }
    while (*q != '\0') {
        q++;
    }
    if (p != q) { /* 至少有一个非空格字符，否则全是空格 */
        while (isspace(*--q))
            ;
        *++q = '\0';
    }

    if (p != strDest) {
        while (*p != '\0') {
            *strDest++ = *p++;
        }
        *strDest = '\0';
    }

    return HI_SUCCESS;
}

static HI_S32 CheckSection(string row, Array<string> sections)
{
    HI_CHAR buffer[STR_MAX] = { 0 };

    if (row.length() >= STR_MAX) {
        LOGE("str size:%d too large, max len:%d", row.length(), STR_MAX);
        return HI_FAILURE;
    }
    errno_t rc = strncpy_s(buffer, sizeof(buffer), row.data(), row.length());
    if (rc != EOK) {
        LOGE("strncpy_s failed, rc = %d", rc);
        return HI_FAILURE;
    }

    HI_S32 ret = RemoveSpace(buffer, STR_MAX);
    if (ret != HI_SUCCESS) {
        LOGE("RemoveSpace faild, ret:0x%x", ret);
        return ret;
    }

    for (HI_U32 i = 0; i < sections.length(); i++) {
        string tmp = "[";
        tmp += sections[i];
        tmp += "]";

        if (!strncasecmp(tmp.data(), buffer, tmp.length())) {
            return i;
        }
    }
    return HI_FAILURE;
}

static HI_S32 CheckKey(string &row)
{
    HI_U32 size = row.find_first_of('=');
    if (size == string::npos) {
        return HI_FAILURE;
    }

    HI_CHAR buffer[STR_MAX] = { 0 };
    errno_t rc = strncpy_s(buffer, sizeof(buffer), row.substr(0, size).c_str(), sizeof(buffer) - 1);
    if (rc != EOK) {
        LOGE("strncpy_s failed, rc = %d", rc);
        return HI_FAILURE;
    }

    HI_S32 ret = RemoveSpace(buffer, STR_MAX);
    if (ret != HI_SUCCESS) {
        LOGE("RemoveSpace faild, ret:0x%x", ret);
        return ret;
    }

    row = buffer;

    return HI_SUCCESS;
}

INIProvider::INIProvider()
{
    LOGI("load ini file start...");
    LoadIniFile();
    LOGI("load ini file end...");
}

INIProvider::~INIProvider()
{
    LOGI("desctruct INIProvider!");
}

HI_VOID INIProvider::LoadIniFile()
{
    HiCfgINI *pINI = NULL;
    HI_CHAR filePath[PATH_MAX + 1] = {};
    HI_CHAR *iniPath = HI_NULL;

    // get the path of cfg.ini
    iniPath = GetCfgFile(INI_CFG_INI_PATH, filePath);
    LOGI("hi_get_one_cfg_file. cfgIniPath:%s", iniPath);
    if ((iniPath == HI_NULL) || (strlen(filePath) > PATH_MAX)) {
        LOGI("hi_get_one_cfg_file failed. %s", filePath);
        return;
    }
    mIniPathVec.push_back(string(iniPath));
#if 0
    // get the path of db.ini
    iniPath = GetCfgFile(INI_DB_INI_PATH, filePath);
    LOGI("hi_get_one_cfg_file. dbIniPath:%s", iniPath);
    if ((iniPath == HI_NULL) || (strlen(filePath) > PATH_MAX)) {
        LOGI("hi_get_one_cfg_file failed. %s", filePath);
        return;
    }
    mIniPathVec.push_back(string(iniPath));

    // get the path of aq.ini
    iniPath = GetCfgFile(INI_AQ_INI_PATH, filePath);
    LOGI("hi_get_one_cfg_file. aqIniPath:%s", iniPath);
    if ((iniPath == HI_NULL) || (strlen(filePath) > PATH_MAX)) {
        LOGI("hi_get_one_cfg_file failed. %s", filePath);
        return;
    }
    mIniPathVec.push_back(string(iniPath));
#endif
    // load the ini files
    std::vector<string>::iterator itVec = mIniPathVec.begin();
    for (; itVec != mIniPathVec.end(); itVec++) {
        LOGI("the ini path:%s", (*itVec).c_str());
        InitCacheMap(*itVec);
    }

    LOGI("Update keys finish! all ini configs have been loaded into Cache!");
}

HI_VOID INIProvider::InitCacheMap(string inifile)
{
    Array<string> fileArray = StringToStringArray(inifile.c_str());
    string fileName = fileArray[0];
    HI_U32 fileVersion = atoi(fileArray[1].c_str());

    AddCacheMap(fileName);
}

HI_S32 INIProvider::CacheMapLoad(HiCfgINI *pINI, HiFile *fp)
{
    LOGI("loading keys to m_Cache...");

    string row = "";
    string sec = "";
    Array<string> sections = pINI->GetSectionList();

    while (!fp->IsEOF()) {
        row = fp->NextRow();
        if (row.empty() || !row.length()) {
            continue;
        }
        HI_S32 index = CheckSection(row, sections);
        if (index != HI_FAILURE) {
            sec = sections[index];
            continue;
        }
        HI_S32 s32Ret = CheckKey(row);
        if (s32Ret != HI_SUCCESS) {
            continue;
        }
        if (sec.length()) {
            string fil = sec + ":" + row;
            string val = pINI->ParseString(fil);
            std::pair<string, string> pr(fil, val);
            m_Cache.insert(pr);
        }
    }

    return HI_SUCCESS;
}

HI_VOID INIProvider::AddCacheMap(string inifile)
{
    LOGI("start loading keys in ini file to m_Cache, file:%s", inifile.c_str());
    HiCfgINI *pINI = new HiCfgINI(inifile.c_str());
    if (pINI == NULL) {
        LOGE("pINI NULL");
        return;
    }
    if (pINI->GetCfgExceptionStatus()) {
        LOGE("inifile pointer is null or ini file open failure.");
        delete pINI;
        pINI = NULL;
        return;
    }

    HiFile *fp = new HiFile();
    if (fp == NULL) {
        LOGE("new HiFile() failure");
        if (pINI != NULL) {
            delete pINI;
            pINI = NULL;
        }
        return;
    }
    fp->Init(inifile);
    if (fp->GetIOExceptionStatus()) {
        LOGE("file path error or open file failure");
        if (pINI != NULL) {
            delete pINI;
            pINI = NULL;
        }
        delete fp;
        fp = NULL;
        return;
    }

    CacheMapLoad(pINI, fp);

    if (fp != NULL) {
        delete fp;
        fp = NULL;
    }

    if (pINI != NULL) {
        delete pINI;
        pINI = NULL;
    }
}

HI_S32 INIProvider::CacheMapUnload(HiCfgINI *pINI, HiFile *fp)
{
    LOGI("uploading keys for m_Cache...");

    string row = "";
    string sec = "";
    Array<string> sections = pINI->GetSectionList();

    while (!fp->IsEOF()) {
        row = fp->NextRow();
        if (row.empty() || !row.length()) {
            continue;
        }
        HI_S32 index = CheckSection(row, sections);
        if (index != HI_FAILURE) {
            sec = sections[index];
            continue;
        }
        HI_S32 s32Ret = CheckKey(row);
        if (s32Ret != HI_SUCCESS) {
            continue;
        }

        if (!sec.length()) {
            continue;
        }

        string fil = sec + ":" + row;
        std::map<string, string>::iterator iter = m_Cache.begin();
        for (; iter != m_Cache.end();) {
            if (fil == iter->first) {
                m_Cache.erase(iter++);
            } else {
                ++iter;
            }
        }
    }

    return HI_SUCCESS;
}

HI_VOID INIProvider::EraseCacheMap(string inifile)
{
    LOGI("starting erase key in ini file from m_Cache, file:%s", inifile.c_str());

    HiCfgINI *pINI = new HiCfgINI(inifile.c_str());
    if (pINI == NULL) {
        LOGE("new HiCfgINI() failure");
        return;
    }
    if (pINI->GetCfgExceptionStatus()) {
        LOGE("inifile pointer is null or ini file open failure.");
        delete pINI;
        pINI = NULL;
        return;
    }

    HiFile *fp = new HiFile();
    if (fp == NULL) {
        LOGE("new HiFile() failure");
        if (pINI != NULL) {
            delete pINI;
            pINI = NULL;
        }
        return;
    }
    fp->Init(inifile);

    if (fp->GetIOExceptionStatus()) {
        LOGE("file path error or open file failure");
        if (pINI != NULL) {
            delete pINI;
            pINI = NULL;
        }
        delete fp;
        fp = NULL;
        return;
    }

    CacheMapUnload(pINI, fp);

    if (fp != NULL) {
        delete fp;
        fp = NULL;
    }

    if (pINI != NULL) {
        delete pINI;
        pINI = NULL;
    }
}

HI_VOID INIProvider::DumpCache(FILE *fp)
{
    std::map<string, string>::iterator it = m_Cache.begin();
    HI_U32 u32Count = 0;
    LOGI("dump cache ...");

    if (fp != NULL) {
        while (it != m_Cache.end()) {
            fprintf(fp, "%s\t\t%s\n", it->first.c_str(), it->second.c_str());
            ++it;
            u32Count++;
        }
    } else {
        while (it != m_Cache.end()) {
            LOGI("%s\t\t%s", it->first.c_str(), it->second.c_str());
            ++it;
            u32Count++;
        }
    }
    LOGI("dump cache count:%d!", u32Count);
}

HI_S32 INIProvider::InsertAtvChannelList(string fileName)
{
    HiCfgINI *pINI = NULL;

    LOGI("start loading Atv default channel list ini file: %s", fileName.c_str());
    pINI = new HiCfgINI(fileName.c_str());
    if (pINI == NULL) {
        LOGE("pINI NULL");
        return HI_FAILURE;
    }
    if (pINI->GetCfgExceptionStatus()) {
        LOGE("inifile pointer is null or ini file open failure.");
        delete pINI;
        pINI = NULL;
        return HI_FAILURE;
    }

    HI_U32 u32Count;
    Array<string> sections = pINI->GetSectionList();
    u32Count = sections.length();

    AddCacheMap(fileName);

    delete pINI;
    pINI = NULL;
    return u32Count;
}
HI_VOID INIProvider::EraseAtvChannelList(string fileName)
{
    LOGI("erase Atv default Channel List ini file: %s", fileName.c_str());

    return EraseCacheMap(fileName);
}

HI_S32 INIProvider::ParseInt(const string &fil)
{
    Array<HI_S32> array = ParseIntArray(fil);
    if (array.empty()) {
        LOGE("No val at fil:%s", fil.c_str());
        return 0;
    }

    return array[0];
}

Array<HI_S32> INIProvider::ParseIntArray(const string &fil)
{
    std::map<string, string>::iterator it = m_Cache.find(fil);
    if (m_Cache.end() == it) {
        LOGE("No this fil:%s", fil.c_str());
        return Array<HI_S32>();
    }

    return StringToIntArray(it->second.c_str());
}

HI_DOUBLE INIProvider::ParseDouble(const string &fil)
{
    Array<HI_DOUBLE> array = ParseDoubleArray(fil);
    if (array.empty()) {
        LOGE("No val at fil:%s", fil.c_str());
        return 0.0;
    }

    return array[0];
}

Array<HI_DOUBLE> INIProvider::ParseDoubleArray(const string &fil)
{
    std::map<string, string>::iterator it = m_Cache.find(fil);
    if (m_Cache.end() == it) {
        LOGE("No this fil:%s", fil.c_str());
        return Array<HI_DOUBLE>();
    }

    return StringToDoubleArray(it->second.c_str());
}

HI_BOOL INIProvider::ParseBoolean(const string &fil)
{
    Array<HI_BOOL> array = ParseBooleanArray(fil);
    if (array.empty()) {
        LOGE("No val at fil:%s", fil.c_str());
        return HI_FALSE;
    }

    return array[0];
}

Array<HI_BOOL> INIProvider::ParseBooleanArray(const string &fil)
{
    std::map<string, string>::iterator it = m_Cache.find(fil);
    if (m_Cache.end() == it) {
        LOGE("No this fil:%s", fil.c_str());
        return Array<HI_BOOL>();
    }

    return StringToBooleanArray(it->second.c_str());
}

string INIProvider::ParseString(const string &fil)
{
    Array<string> array = ParseStringArray(fil);
    if (array.empty()) {
        LOGE("No val at fil:%s", fil.c_str());
        return "";
    }

    return array[0];
}

Array<string> INIProvider::ParseStringArray(const string &fil)
{
    std::map<string, string>::iterator it = m_Cache.find(fil);
    if (m_Cache.end() == it) {
        LOGE("No this fil:%s", fil.c_str());
        return Array<string>();
    }

    return StringToStringArray(it->second.c_str());
}
