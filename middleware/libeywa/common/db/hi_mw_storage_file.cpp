/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_storage_file"

#include "hi_mw_storage_file.h"

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "hi_mw_log.h"
#include "hi_mw_common.h"

using namespace std;

const HI_U32 HiStorageFile::EVENT_CLOSE_FILE = 0x10;
const HI_U32 HiStorageFile::EVENT_CLEAR_FILE = 0x40;
const HI_U32 HiStorageFile::FILE_ID_MASK = 0x0F;
const HI_U32 HiStorageFile::FEEDBACK_EVT_CLOSE_FILE = 0x10;
const HI_U32 HiStorageFile::FEEDBACK_EVT_CLOSE_THREAD = 0x20;
const HI_U32 HiStorageFile::FEEDBACK_EVT_CLEAR_FILE = 0x40;

Event<HI_U32> *HiStorageFile::mEvt = HI_NULL;
Event<HI_U32> *HiStorageFile::mFeedbackEvt = HI_NULL;

string HiStorageFile::mPath[HiStorageFile::MAX_FILE];
FILE *HiStorageFile::mFileArray[HiStorageFile::MAX_FILE] = { HI_NULL };
HI_BOOL HiStorageFile::mInit = HI_FALSE;
HiMutex HiStorageFile::mLock;
pthread_t HiStorageFile::mSaveDataThreadID;

list<HiStorageFile::HI_SAVE_DATA_S> HiStorageFile::mSaveListWait;
list<HiStorageFile::HI_SAVE_DATA_S> HiStorageFile::mSaveListDo;

HI_S32 HiStorageFile::Init(const string &path)
{
    HI_S32 s32Ret;
    HI_U8 id = 0;
    HiMutex::Autolock lock(mLock);

    for (; id < MAX_FILE && mFileArray[id] != HI_NULL; id++) {
        /* find not use id */
    }

    if (id == MAX_FILE) {
        LOGE("Error! %d files have be open!", MAX_FILE);

        return HI_FAILURE;
    }
    mFileId = id;
#if 0
    HI_CHAR acResolvedPath[PATH_MAX] = {};
    realpath(path.c_str(), acResolvedPath);
    if (strlen(path.c_str()) > PATH_MAX - 1 || 0 == strlen(acResolvedPath)) {
        LOGE("file path:%s error!", path.c_str());
        return HI_FAILURE;
    }
#else
    // 文件可能不存在，S和Q存在差异，采用判断文件前级目录是否存在进行操作
    HI_S32 delimiterPos = path.rfind('/');
    HI_CHAR acResolvedPath[PATH_MAX] = {};
    if (delimiterPos != std::string::npos
        && realpath(path.substr(0, delimiterPos).c_str(), acResolvedPath) == HI_NULL) {
        LOGE("Error! %s realpath failed!", path.substr(0, delimiterPos).c_str());
        return HI_FAILURE;
    }
    s32Ret = sprintf_s(acResolvedPath + strlen(acResolvedPath), PATH_MAX - strlen(acResolvedPath), "%s",
        delimiterPos == std::string::npos ? path.c_str() : path.substr(delimiterPos).c_str());
    if (s32Ret == HI_FAILURE) {
        LOGE("sprintf_s failed! s32Ret = %d", s32Ret);
        return HI_FAILURE;
    }
#endif
    mFileArray[mFileId] = fopen(acResolvedPath, "r+b");
    if (mFileArray[mFileId] == HI_NULL) {
        mFileArray[mFileId] = fopen(acResolvedPath, "w+b");
        if (mFileArray[mFileId] == HI_NULL) {
            LOGE("Error! %s open failed!", acResolvedPath);
            return HI_FAILURE;
        }
    }

    s32Ret = InitCreateThd(path);

    return s32Ret;
}

HI_S32 HiStorageFile::InitCreateThd(const string &path)
{
    HI_S32 s32Ret = HI_FAILURE;
    int fd = fileno(mFileArray[mFileId]);
    if (fd == -1 || fchmod(fd, 0770) != HI_SUCCESS) { /* fd is -1, xxx is 0770 */
        LOGW("chmod 0777 for db file fail");
    }

    mPath[mFileId] = path;
    if (mInit == HI_FALSE) {
        if (CreateEvent() != HI_SUCCESS) {
            fclose(mFileArray[mFileId]);
            mFileArray[mFileId] = HI_NULL;
            return HI_FAILURE;
        }

        s32Ret = pthread_create(&mSaveDataThreadID, HI_NULL, SaveDataThread, HI_NULL);
        if (s32Ret != HI_SUCCESS) {
            fclose(mFileArray[mFileId]);
            mFileArray[mFileId] = HI_NULL;
            DestroyEvent();
            LOGE("Error,create save data thread fail, ret:0x%x.", s32Ret);
            return HI_FAILURE;
        }
        mInit = HI_TRUE;
    }
    return HI_SUCCESS;
}

HI_S32 HiStorageFile::Deinit()
{
    HI_U32 u32Evt = 0;
    HI_S32 s32Ret;

    mLock.lock();
    if ((mInit == HI_FALSE) || (mFileArray[mFileId] == HI_NULL)) {
        mLock.unlock();
        return HI_FAILURE;
    }

    s32Ret = mEvt->send(EVENT_CLOSE_FILE + mFileId);
    RET_CHECK_ERR_LOG(s32Ret, "send CloseFileEvent fail, ret:0x%x", s32Ret);

    mLock.unlock();
    mFeedbackEvt->wait(&u32Evt);
    if (u32Evt == FEEDBACK_EVT_CLOSE_THREAD) {
        return pthread_join(mSaveDataThreadID, NULL);
    }

    if (u32Evt == FEEDBACK_EVT_CLOSE_FILE) {
        LOGI("File close!");
    }

    return HI_SUCCESS;
}

HI_S32 HiStorageFile::Read(HI_U32 pos, HI_U8 *pData, HI_U32 size)
{
    if (pData == HI_NULL) {
        LOGE("pData is null ptr");
        return HI_FAILURE;
    }
    HiMutex::Autolock lock(mLock);
    HI_S32 s32Ret;

    if (mFileArray[mFileId] == HI_NULL) {
        LOGE("Error,file %s is not open.", mPath[mFileId].c_str());
        return HI_FAILURE;
    }

    s32Ret = fseek(mFileArray[mFileId], pos, SEEK_SET);
    if (s32Ret != HI_SUCCESS) {
        LOGE("Error, seek file %s fail, pos:0x%x, size:%d.", mPath[mFileId].c_str(), pos, size);
        return HI_FAILURE;
    }

    HI_S32 s32ReadSize = static_cast<HI_S32>(fread(pData, 1, size, mFileArray[mFileId]));
    if (s32ReadSize != (HI_S32)size) {
        errno_t rc = memset_s(pData, size, 0, size);
        if (rc != EOK) {
            LOGE("memset_s_func failed, rc = %d", rc);
            return HI_FAILURE;
        }
        LOGE("Error, fread file %s fail, pos:0x%x, size:%d.", mPath[mFileId].c_str(), pos, size);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 HiStorageFile::Write(HI_U32 pos, const HI_U8 *pData, HI_U32 size)
{
    HiMutex::Autolock lock(mLock);

    if (mFileArray[mFileId] == HI_NULL || pData == HI_NULL) {
        LOGE("Error,file is null or pData is null , fileID:%d", mFileId);
        return HI_FAILURE;
    }
    errno_t rc;
    /* 合并连续写同一属性数据 */
    list<HI_SAVE_DATA_S>::iterator it;
    for (it = mSaveListWait.begin(); it != mSaveListWait.end(); ++it) {
        if ((it->addrPos == pos) && (it->id == mFileId)) {
            rc = memcpy_s(it->pData, size, pData, size);
            if (rc != EOK) {
                LOGE("memcpy_s_func failed rc = %d", rc);
                return HI_FAILURE;
            }
            it->times++;

            return HI_SUCCESS;
        }
    }

    HI_SAVE_DATA_S stSaveData = { 0, 0, 0, 0, 1 };
    stSaveData.pData = new (std::nothrow) HI_U8[size];
    if (stSaveData.pData == HI_NULL) {
        LOGE("Error,no memory!");
        return HI_FAILURE;
    }
    rc = memcpy_s(stSaveData.pData, size, pData, size);
    if (rc != EOK) {
        LOGE("memcpy_s_func failed rc = %d", rc);
        delete[] stSaveData.pData;
        stSaveData.pData = HI_NULL;
        return HI_FAILURE;
    }
    stSaveData.addrPos = pos;
    stSaveData.size = size;
    stSaveData.id = mFileId;

    mSaveListWait.push_back(stSaveData);

    return HI_SUCCESS;
}

HI_S32 HiStorageFile::Clear()
{
    HI_U32 u32Evt = 0;
    HI_S32 s32Ret;

    mLock.lock();

    if (mFileArray[mFileId] == HI_NULL) {
        mLock.unlock();
        return HI_FAILURE;
    }

    s32Ret = mEvt->send(EVENT_CLEAR_FILE + mFileId);
    RET_CHECK_ERR_LOG(s32Ret, "send ClearFileEvent fail, ret:0x%x", s32Ret);

    mLock.unlock();
    LOGI("clear data!");
    mFeedbackEvt->wait(&u32Evt, 2000); /* waittime is 2000 */

    return HI_SUCCESS;
}

HI_S32 HiStorageFile::Size(HI_U32 &u32FileSize)
{
    HI_S32 s32Ret;
    HiMutex::Autolock lock(mLock);

    if (mFileArray[mFileId] == HI_NULL) {
        LOGE("Error! -11111 failed to get file size!");
        return HI_FAILURE;
    }
    s32Ret = fseek(mFileArray[mFileId], 0, SEEK_END);
    RET_CHECK_ERR_LOG(s32Ret, "fseek position fail, ret:0x%x", s32Ret);

    HI_S64 s64Size = ftell(mFileArray[mFileId]);
    if (s64Size < 0) {
        LOGE("Error! failed to get file size!");
        return HI_FAILURE;
    }
    u32FileSize = (HI_U32)((HI_U64)s64Size & 0xFFFFFFFF);

    return HI_SUCCESS;
}

HI_VOID *HiStorageFile::SaveDataThread(HI_VOID *arg)
{
    HI_S32 s32Ret;
    HI_U32 u32Evt = 0;
    static list<HiStorageFile::HI_SAVE_DATA_S>::iterator start;
    static list<HiStorageFile::HI_SAVE_DATA_S>::iterator end;

    while (1) {
        u32Evt = 0;
        s32Ret = mEvt->wait(&u32Evt, 500); /* waittime is 500 */
        if (s32Ret != HI_SUCCESS) {
            LOGE("mEvt->wait error, ret:0x%x", s32Ret);
            return HI_NULL;
        }

        mLock.lock();
        for (start = mSaveListWait.begin(), end = mSaveListWait.begin(); end != mSaveListWait.end(); ++end) {
            if (end->times > 1) {
                end->times = 1;
                mSaveListDo.splice(mSaveListDo.end(), mSaveListWait, start, end);
                start = end;
                ++start;
            }
        }
        mSaveListDo.splice(mSaveListDo.end(), mSaveListWait, start, end);
        mLock.unlock();
        while (!mSaveListDo.empty()) {
            Save(mSaveListDo.front().id, mSaveListDo.front().addrPos,
                mSaveListDo.front().pData, mSaveListDo.front().size);
            if (mSaveListDo.front().pData) {
                delete[] mSaveListDo.front().pData;
                mSaveListDo.front().pData = NULL;
            }
            mSaveListDo.pop_front();
        }

        s32Ret = DoEvent(u32Evt);
        if (s32Ret != HI_SUCCESS) {
            return HI_NULL;
        }

        HI_U8 i;
        for (i = 0; i < MAX_FILE; i++) {
            if (mFileArray[i] != HI_NULL) {
                break;
            }
        }
        if (i == MAX_FILE) {
            break;
        }
    }

    return HI_NULL;
}

HI_S32 HiStorageFile::Save(HI_U8 id, HI_U32 addrPos, const HI_U8 *pData, HI_U32 size)
{
    HI_S32 s32Ret;
    if (mFileArray[id] == HI_NULL) {
        LOGE("mFileArray[id] is null ptr");
        return HI_FAILURE;
    }

    s32Ret = fseek(mFileArray[id], addrPos, SEEK_SET);
    if (s32Ret != HI_SUCCESS) {
        LOGE("fseek mFileArray[%d]:%s, addrPos:%ld, size:%d, ret:0x%x",
            id, mPath[id].c_str(), addrPos, size, s32Ret);
        return s32Ret;
    }

    HI_U32 s32Size = fwrite(pData, 1, size, mFileArray[id]);
    if (s32Size != size) {
        LOGE("Error, fileId %d, data position %d!, s32Ret:0x%x", id, addrPos, s32Ret);
    }

    s32Ret = fflush(mFileArray[id]);
    if (s32Ret != HI_SUCCESS) {
        LOGE("fflush mFileArray[%d]:%s failure, size:%d, s32Ret:0x%x",
            id, mPath[id].c_str(), size, s32Ret);
        return s32Ret;
    }

    HI_S32 fd = fileno(mFileArray[id]);
    s32Ret = fsync(fd);
    if (s32Ret != HI_SUCCESS) {
        LOGE("fsync mFileArray[%d]:%s failure, size:%d, s32Ret:0x%x",
            id, mPath[id].c_str(), size, s32Ret);
        return s32Ret;
    }
    return HI_SUCCESS;
}


HI_S32 HiStorageFile::CreateEvent(HI_VOID)
{
    mEvt = new (std::nothrow) Event<HI_U32>;
    if (mEvt == HI_NULL) {
        LOGE("Error,no memory!");

        return HI_FAILURE;
    }

    mFeedbackEvt = new (std::nothrow) Event<HI_U32>;
    if (mFeedbackEvt == HI_NULL) {
        delete mEvt;
        mEvt = HI_NULL;
        LOGE("Error,no memory!");

        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

HI_S32 HiStorageFile::DestroyEvent(HI_VOID)
{
    if (mEvt != NULL) {
        delete mEvt;
        mEvt = NULL;
    }
    if (mFeedbackEvt != NULL) {
        delete mFeedbackEvt;
        mFeedbackEvt = NULL;
    }

    return HI_SUCCESS;
}

HI_S32 HiStorageFile::DoEvent(HI_U32 event)
{
    HI_S32 s32Ret;
    if ((event & EVENT_CLOSE_FILE) == EVENT_CLOSE_FILE) {
        HI_S32 s32FileId = (event & FILE_ID_MASK);
        HiMutex::Autolock lock(mLock);

        if (s32FileId < MAX_FILE) {
            if (mFileArray[s32FileId] != HI_NULL) {
                fclose(mFileArray[s32FileId]);
                mFileArray[s32FileId] = HI_NULL;
            }
        }

        HI_U8 i;
        for (i = 0; i < MAX_FILE; i++) {
            if (mFileArray[i] != HI_NULL) {
                break;
            }
        }

        if (i == MAX_FILE) { /* all files are closed */
            LOGI("send close thread event.");
            s32Ret = mFeedbackEvt->send(FEEDBACK_EVT_CLOSE_THREAD);
            if (s32Ret != HI_SUCCESS) {
                LOGE("mFeedbackEvt->send error, ret:0x%x", s32Ret);
            }
            return s32Ret;
        } else {
            LOGI("send close file event.");
            s32Ret = mFeedbackEvt->send(FEEDBACK_EVT_CLOSE_FILE);
            if (s32Ret != HI_SUCCESS) {
                LOGE("mFeedbackEvt->send error, ret:0x%x", s32Ret);
                return s32Ret;
            }
        }
    }

    HI_CHAR acResolvedPath[PATH_MAX];
    errno_t rc = memset_s(&acResolvedPath, sizeof(acResolvedPath), 0, sizeof(acResolvedPath));
    if (rc != EOK) {
        LOGE("memset_s_func failed, rc = %d", rc);
        return HI_FAILURE;
    }

    if ((event & EVENT_CLEAR_FILE) == EVENT_CLEAR_FILE) {
        HI_S32 s32FileId = (event & FILE_ID_MASK);
        HI_S32 ret = DoEventSub(s32FileId, acResolvedPath, PATH_MAX);
        if (ret != HI_SUCCESS) {
            LOGE("DoEventSub failed, ret:0x%x", ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HiStorageFile::DoEventSub(HI_S32 &s32FileId, HI_CHAR *acResolvedPath, HI_S32 acResolvedPathLen)
{
    if (acResolvedPath == HI_NULL) {
        LOGE("acResolvedPath is null ptr");
        return HI_FAILURE;
    }

    if (s32FileId < MAX_FILE) {
        realpath(mPath[s32FileId].c_str(), acResolvedPath);
        if (strlen(mPath[s32FileId].c_str()) > FILE_PATH_MAX_LEN || 0 == strlen(acResolvedPath)) {
            LOGE("file path:%s error!", mPath[s32FileId].c_str());
            return HI_FAILURE;
        }
        if (mFileArray[s32FileId] != HI_NULL) {
            LOGI("send reopen file event.");
            mFileArray[s32FileId] = freopen(acResolvedPath, "w+b", mFileArray[s32FileId]);
            HI_S32 s32Ret = mFeedbackEvt->send(FEEDBACK_EVT_CLEAR_FILE);
            if (s32Ret != HI_SUCCESS) {
                LOGE("mFeedbackEvt->send ClearFile error, ret:0x%x", s32Ret);
                return s32Ret;
            }
        }
    }
    return HI_SUCCESS;
}

