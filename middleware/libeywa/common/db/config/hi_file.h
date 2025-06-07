/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/


#ifndef __HI_FILE_H__
#define __HI_FILE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include "hi_type.h"

#define STRING_MAX 256

using namespace std;

extern HI_BOOL IsBlank(const HI_CHAR *str);

class HiFile {
public:
    HiFile();
    ~HiFile();
    HI_VOID Init(const string &strFile, const string &mode = "r");
    HI_BOOL GetIOExceptionStatus();
    HI_S32 Chmod(mode_t mode);

    HI_BOOL IsEmpty() const;
    HI_BOOL IsEOF() const;
    HI_S32 RollBack() const;
    string NextRow() const;
    HI_S32 PutsRow(const string &strRow);
    HI_S32 Reset();
    HI_VOID Rewind();
    HI_S32 Write(const HI_U8 *pu8Block, HI_U32 u32Size);
    HI_S32 Read(HI_U8 *pu8Block, HI_U32 u32Size);
    HI_S32 Sync();
    HI_S32 Flush();

private:
    FILE *m_ptr;
    HI_U32 m_u32PreRowPos;
    HI_BOOL m_bIOException;
    HiFile(const HiFile &rhs);
    HiFile &operator=(const HiFile &rhs);
};

#endif

