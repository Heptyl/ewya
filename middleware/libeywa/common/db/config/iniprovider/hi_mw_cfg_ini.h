/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_CFG_INI__
#define __HI_MW_CFG_INI__

#include <string>
#include "iniparser.h"
#include "hi_type.h"
#include "hi_mw_array.h"

class HiCfgINI {
public:
    HI_BOOL IsFieldExist(const std::string &field);
    Array<std::string> GetSectionList();

    HI_S32 ParseInt(const std::string &field);
    Array<HI_S32> ParseIntArray(const std::string &field);

    HI_DOUBLE ParseDouble(const std::string &field);
    Array<HI_DOUBLE> ParseDoubleArray(const std::string &field);

    HI_BOOL ParseBoolean(const std::string &field);
    Array<HI_BOOL> ParseBooleanArray(const std::string &field);

    std::string ParseString(const std::string &field);
    Array<std::string> ParseStringArray(const std::string &field);

public:
    explicit HiCfgINI(const HI_CHAR *inifile);
    ~HiCfgINI();
    HI_BOOL GetCfgExceptionStatus();

private:
    friend class HiCfgParse;

private:
    dictionary *mDictionary;
    HI_BOOL m_bHiCfgException;

    HiCfgINI(const HiCfgINI &rhs);
    HiCfgINI &operator=(const HiCfgINI &rhs);
};

Array<HI_S32> StringToIntArray(const HI_CHAR *str);
Array<HI_DOUBLE> StringToDoubleArray(const HI_CHAR *str);
Array<HI_BOOL> StringToBooleanArray(const HI_CHAR *str);
Array<std::string> StringToStringArray(const HI_CHAR *str);

#endif
