/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
 */

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_hi_file"

#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include "hi_file.h"
#include "hi_mw_log.h"
#include "hi_mw_common.h"

HI_BOOL IsBlank(const HI_CHAR *str)
{
    HI_CHAR *p = const_cast<HI_CHAR *>(str);
    if (p == HI_NULL) {
        LOGE("null pointer");
        return HI_FALSE;
    }

    while (*p != '\0') {
        if (!isspace(*p)) {
            return HI_FALSE;
        }
        ++p;
    }

    return HI_TRUE;
}

HiFile::HiFile():m_ptr(HI_NULL), m_u32PreRowPos(0), m_bIOException(HI_FALSE)
{
}

HI_VOID HiFile::Init(const string &strFile, const string &mode)
{
    HI_CHAR acResolvedPath[PATH_MAX] = { 0 };
    realpath(strFile.c_str(), acResolvedPath);
    if (strlen(strFile.c_str()) > PATH_MAX - 1 || 0 == strlen(acResolvedPath)) {
        LOGE("file path:%s error!", strFile.c_str());
        m_bIOException = HI_TRUE;
    }

    m_ptr = fopen(acResolvedPath, mode.c_str());
    if (m_ptr == HI_NULL) {
        LOGE("open %s failure.", strFile.c_str());
        m_bIOException = HI_TRUE;
    }
}

HI_BOOL HiFile::GetIOExceptionStatus()
{
    return m_bIOException;
}

HI_S32 HiFile::Chmod(mode_t mode)
{
    HI_S32 ret = HI_SUCCESS;
    if (m_ptr != HI_NULL) {
        int fd;
        fd = fileno(m_ptr);
        if (fd == -1 || fchmod(fd, mode) != HI_SUCCESS) {
            LOGE("chmod %o error", mode);
            ret = HI_FAILURE;
        }
    } else {
        LOGE("m_ptr is null pointer!");
        return HI_FAILURE;
    }
    return ret;
}

HI_BOOL HiFile::IsEmpty() const
{
    HI_S32 s32Ret;
    if (m_ptr != HI_NULL) {
        s32Ret = fseek(m_ptr, 0, SEEK_END);  // non-portable
        if (s32Ret != HI_SUCCESS) {
            LOGE("fseek fail, pos:%d, ret:0x%x", SEEK_END, s32Ret);
        }

        HI_U32 size = static_cast<HI_U32>(ftell(m_ptr));
        s32Ret = fseek(m_ptr, 0, SEEK_SET);
        if (s32Ret != HI_SUCCESS) {
            LOGE("fseek fail, pos:%d, ret:0x%x", SEEK_SET, s32Ret);
        }

        return size ? HI_FALSE : HI_TRUE;
    } else {
        LOGE("m_ptr is null pointer!");
        return HI_TRUE;
    }
}

HI_BOOL HiFile::IsEOF() const
{
    if (m_ptr != HI_NULL) {
        return feof(m_ptr) ? HI_TRUE : HI_FALSE;
    } else {
        LOGE("m_ptr is null pointer!");
        return HI_TRUE;
    }
}

HI_S32 HiFile::RollBack() const
{
    if (m_ptr == HI_NULL) {
        return HI_FAILURE;
    }
    return fseek(m_ptr, m_u32PreRowPos, SEEK_SET) ? HI_FAILURE : HI_SUCCESS;
}

string HiFile::NextRow() const
{
    HI_CHAR row[STRING_MAX] = { 0 };

    if (m_ptr != HI_NULL) {
        const_cast<HiFile *>(this)->m_u32PreRowPos =
            static_cast<HI_U32>(ftell(m_ptr));

        HI_CHAR *str = fgets(row, sizeof(row), m_ptr);

        if (str != HI_NULL && *str != '#' && *str != '\r' && !IsBlank(str)) {
            LOGV("read OK");
        } else if (str == HI_NULL && !feof(m_ptr)) {
            LOGE("read err.");
            *row = '\0';
        } else {
            *row = '\0';
        }
    }

    return row;
}

HI_S32 HiFile::PutsRow(const string &strRow)
{
    if (strRow.empty()) {
        return HI_FAILURE;
    }

    const string strDest(strRow + "\r\n");
    HI_S32 s32Ret;
    if (m_ptr != HI_NULL) {
        HI_S32 s32Pos = ftell(m_ptr);
        s32Ret = fseek(m_ptr, 0, SEEK_END);
        if (s32Ret != HI_SUCCESS) {
            LOGE("fseek file:%s fail, s32Pos:0, ret:0x%x", strDest.c_str(), s32Ret);
            return s32Ret;
        }

        HI_S32 s32Ret;
        s32Ret = fputs(strDest.c_str(), m_ptr);
        if (s32Ret != HI_SUCCESS) {
            LOGE("fputs file:%s fail, ret:0x%x", strDest.c_str(), s32Ret);
            return s32Ret;
        }

        s32Ret = fsync(fileno(m_ptr));
        if (s32Ret != HI_SUCCESS) {
            LOGE("fsync file:%s fail, ret:0x%x", strDest.c_str(), s32Ret);
            return s32Ret;
        }

        if (s32Pos < 0) {
            LOGE("Pos:%d is negative number", s32Pos);
            return HI_FAILURE;
        }
        s32Ret = fseek(m_ptr, s32Pos, SEEK_SET);
        if (s32Ret != HI_SUCCESS) {
            LOGE("fseek file:%s fail, s32Pos:0x%x, ret:0x%x", strDest.c_str(), s32Pos, s32Ret);
            return s32Ret;
        }

        return (strlen(strDest.c_str()) != static_cast<HI_U32>(s32Ret)) ? HI_FAILURE : HI_SUCCESS;
    } else {
        LOGE("m_ptr is null pointer!");
        return HI_FAILURE;
    }
}

HI_S32 HiFile::Reset()
{
    HI_S32 ret;

    if (m_ptr == HI_NULL) {
        return HI_FAILURE;
    }

    ret = fseek(m_ptr, 0, SEEK_SET);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    m_u32PreRowPos = 0;

    return ftruncate(fileno(m_ptr), 0);
}

HI_VOID HiFile::Rewind()
{
    if (m_ptr == HI_NULL) {
        return;
    }
    ::rewind(m_ptr);
}

HI_S32 HiFile::Write(const HI_U8 *pu8Block, HI_U32 u32Size)
{
    if (pu8Block == HI_NULL || !u32Size || m_ptr == HI_NULL) {
        return HI_FAILURE;
    }

    HI_U32 u32Ret = fwrite(pu8Block, u32Size, 1, m_ptr);

    return 1 == u32Ret ? HI_SUCCESS : HI_FAILURE;
}

HI_S32 HiFile::Read(HI_U8 *pu8Block, HI_U32 u32Size)
{
    if (pu8Block == HI_NULL || !u32Size || m_ptr == HI_NULL) {
        return HI_FAILURE;
    }

    HI_U32 u32Ret = fread(pu8Block, u32Size, 1, m_ptr);

    return 1 == u32Ret ? HI_SUCCESS : HI_FAILURE;
}

HI_S32 HiFile::Sync()
{
    if (m_ptr == HI_NULL) {
        return HI_FAILURE;
    }

    return fsync(fileno(m_ptr));
}

HI_S32 HiFile::Flush()
{
    if (m_ptr == HI_NULL) {
        return HI_FAILURE;
    }

    return fflush(m_ptr);
}

HiFile::~HiFile()
{
    if (m_ptr != HI_NULL) {
        fclose(m_ptr);
        m_ptr = HI_NULL;
    }
}

