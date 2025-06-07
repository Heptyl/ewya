/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_cfg_ini"

#include "hi_mw_cfg_ini.h"
#include <strings.h>
#include <ctype.h>
#include <exception>
#include "securec.h"

HiCfgINI::HiCfgINI(const HI_CHAR *inifile) :
    mDictionary(NULL), m_bHiCfgException(HI_FALSE)
{
    if (inifile == NULL) {
        LOGE("inifile pointer is null.");
        m_bHiCfgException = HI_TRUE;
    } else {
        mDictionary = iniparser_load(inifile);
        if (mDictionary == NULL) {
            LOGE("ini file open failure.");
            m_bHiCfgException = HI_TRUE;
        }
    }
}

HiCfgINI::~HiCfgINI()
{
    if (mDictionary != HI_NULL) {
        iniparser_freedict(mDictionary);
    }
}

HI_BOOL HiCfgINI::GetCfgExceptionStatus()
{
    return m_bHiCfgException;
}

HI_BOOL HiCfgINI::IsFieldExist(const std::string &field)
{
    return (HI_BOOL)iniparser_find_entry(mDictionary, const_cast<char *>(field.c_str()));
}

Array<std::string> HiCfgINI::GetSectionList()
{
    HI_S32 u32Num = iniparser_getnsec(mDictionary);
    Array<std::string> secList;
    const char *pChar = NULL;
    for (HI_S32 i = 0; i < u32Num; i++) {
        pChar = iniparser_getsecname(mDictionary, i);
        if (pChar != NULL) {
            secList.addTail(pChar);
        }
    }

    return secList;
}

HI_S32 HiCfgINI::ParseInt(const std::string &field)
{
    return iniparser_getint(mDictionary, field.c_str(), 0);
}

HI_DOUBLE HiCfgINI::ParseDouble(const std::string &field)
{
    return iniparser_getdouble(mDictionary, const_cast<char *>(field.c_str()), 0.0);
}

HI_BOOL HiCfgINI::ParseBoolean(const std::string &field)
{
    return (HI_BOOL)iniparser_getboolean(mDictionary, field.c_str(), 0);
}

std::string HiCfgINI::ParseString(const std::string &field)
{
    return iniparser_getstring(mDictionary, field.c_str(), (char *)"");
}

Array<HI_S32> StringToIntArray(const HI_CHAR *str)
{
    Array<HI_S32> array;
    HI_CHAR *p = const_cast<HI_CHAR *>(str);

    while (*str != '\0') {
        while (*str == ',' || isspace((int)*str)) {
            str++;
        }
        p = const_cast<HI_CHAR *>(str);
        while (*p != '\0' && *p != ',') {
            p++;
        }

        HI_CHAR tmp[32] = { 0 }; /* array length is 32 */

        if (p > str) {
            errno_t rc = strncpy_s(tmp, sizeof(tmp), str, p - str);
            if (rc != EOK) {
                LOGE("strncpy_s failed, rc = %d", rc);
                return array;
            }
            HI_S32 value;
            HI_CHAR *pszEnd = NULL;
            HI_S32 base = 10; /* base is 10 */
            if (!strncasecmp(tmp, "0x", 2)) { /* xxx is 2 */
                base = 16; /* base is 16 */
            }
            value = strtol(tmp, &pszEnd, base);
            if (tmp == pszEnd) {
                printf("strtol:%s is not figure\n", tmp);
            }
            array.addTail(value);
        }

        if (*p == '\0') {
            break;
        }
        str = ++p;
    }

    return array;
}

Array<HI_S32> HiCfgINI::ParseIntArray(const std::string &field)
{
    Array<HI_S32> array;

    // 解析私有格式
    const HI_CHAR *str = iniparser_getstring(mDictionary, field.c_str(), (char *)"");
    if (str != NULL) {
        array = StringToIntArray(str);
    }
    return array;
}

Array<HI_DOUBLE> StringToDoubleArray(const HI_CHAR *str)
{
    Array<HI_DOUBLE> array;
    HI_CHAR *p = const_cast<HI_CHAR *>(str);

    while (*str != '\0') {
        while (*str == ',') {
            str++;
        }
        p = const_cast<HI_CHAR *>(str);
        while (*p != '\0' && *p != ',') {
            p++;
        }

        HI_CHAR tmp[32] = { 0 }; /* array length is 32 */

        if (p > str) {
            errno_t rc = strncpy_s(tmp, sizeof(tmp), str, p - str);
            if (rc != EOK) {
                LOGE("strncpy_s failed, rc = %d", rc);
                return array;
            }
            HI_DOUBLE value;
            HI_CHAR *pszEnd = NULL;
            value = strtod(tmp, &pszEnd);
            if (tmp == pszEnd) {
                printf("strtod:%s is not figure", tmp);
            }
            array.addTail(value);
        }

        if (*p == '\0') {
            break;
        }
        str = ++p;
    }
    return array;
}

Array<HI_DOUBLE> HiCfgINI::ParseDoubleArray(const std::string &field)
{
    Array<HI_DOUBLE> array;

    // 解析私有格式
    const HI_CHAR *str = iniparser_getstring(mDictionary, field.c_str(), (char *)"");
    if (str != NULL) {
        array = StringToDoubleArray(str);
    }
    return array;
}

Array<HI_BOOL> StringToBooleanArray(const HI_CHAR *str)
{
    Array<HI_BOOL> array;
    HI_CHAR *p = const_cast<HI_CHAR *>(str);

    while (*str != '\0') {
        while (*str == ',' || isspace((int)*str)) {
            str++;
        }
        p = const_cast<HI_CHAR *>(str);
        while (*p != '\0' && *p != ',') {
            p++;
        }

        HI_CHAR tmp[32] = { 0 }; /* array length is 32 */

        if (p > str) {
            errno_t rc = strncpy_s(tmp, sizeof(tmp), str, p - str);
            if (rc != EOK) {
                LOGE("strncpy_s failed, rc = %d", rc);
                return array;
            }
            if (!strcasecmp(tmp, "y") || !strcasecmp(tmp, "yes") || !strcasecmp(tmp, "true") || !strcasecmp(tmp, "1")) {
                array.addTail(HI_TRUE);
            }
            if (!strcasecmp(tmp, "n") || !strcasecmp(tmp, "no") || !strcasecmp(tmp, "false") || !strcasecmp(tmp, "0")) {
                array.addTail(HI_FALSE);
            }
        }

        if (*p == '\0') {
            break;
        }
        str = ++p;
    }

    return array;
}

Array<HI_BOOL> HiCfgINI::ParseBooleanArray(const std::string &field)
{
    Array<HI_BOOL> array;

    // 解析私有格式
    const HI_CHAR *str = iniparser_getstring(mDictionary, field.c_str(), (char *)"");
    if (str != NULL) {
        array = StringToBooleanArray(str);
    }
    return array;
}

Array<std::string> StringToStringArray(const HI_CHAR *str)
{
    Array<std::string> array;
    HI_CHAR *p = const_cast<HI_CHAR *>(str);

    while (*str != '\0') {
        while (*str == ',') {
            str++;
        }
        p = const_cast<HI_CHAR *>(str);
        while (*p != '\0' && *p != ',') {
            p++;
        }

        HI_CHAR tmp[256] = { 0 }; /* array length is 256 */

        if (p > str) {
            errno_t rc = strncpy_s(tmp, sizeof(tmp), str, p - str);
            if (rc != EOK) {
                LOGE("strncpy_s failed, rc = %d", rc);
                return array;
            }
            array.addTail(tmp);
        }

        if (*p == '\0') {
            break;
        }
        str = ++p;
    }

    return array;
}

Array<std::string> HiCfgINI::ParseStringArray(const std::string &field)
{
    Array<std::string> array;

    // 解析私有格式
    const HI_CHAR *str = iniparser_getstring(mDictionary, field.c_str(), (char *)"");
    if (str != NULL) {
        array = StringToStringArray(str);
    }
    return array;
}


