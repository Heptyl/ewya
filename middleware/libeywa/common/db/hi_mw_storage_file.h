/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/
#ifndef __MW_STORAGE_FILE_H__
#define __MW_STORAGE_FILE_H__

#include <string>
#include <list>
#include <stdio.h>

#include "hi_type.h"
#include "hi_mw_mutex.h"
#include "hi_mw_event.h"

using namespace std;

class HiStorageFile {
    struct HI_SAVE_DATA_S {
        HI_U32 addrPos;
        HI_U8 *pData;
        HI_U8 id;
        HI_U32 size;
        HI_U32 times;
    };

public:
    /* 构造函数 */
    HiStorageFile();
    ~HiStorageFile();
    HI_S32 Init(const string &path);
    HI_S32 InitCreateThd(const string &path);
    HI_S32 Deinit();
    HI_S32 Read(HI_U32 pos, HI_U8 *data, HI_U32 size);
    HI_S32 Write(HI_U32 pos, const HI_U8 *data, HI_U32 size);
    HI_S32 Clear();
    HI_S32 Size(HI_U32 &u32FileSize);

private:
    /* 如果都在这里设定初值，Hisi V200编译器不支持，所以前四个静态常量移到实现文件里设初值。 */
    static const HI_U32 EVENT_CLOSE_FILE;
    static const HI_U32 EVENT_CLEAR_FILE;
    static const HI_U32 FILE_ID_MASK;
    static const HI_U32 FEEDBACK_EVT_CLOSE_FILE;
    static const HI_U32 FEEDBACK_EVT_CLOSE_THREAD;
    static const HI_U32 FEEDBACK_EVT_CLEAR_FILE;
    static const HI_U8 MAX_FILE = 16;

    HI_U8 mFileId;

    static string mPath[HiStorageFile::MAX_FILE];
    static FILE *mFileArray[HiStorageFile::MAX_FILE];
    static HI_BOOL mInit;
    static HiMutex mLock;
    static pthread_t mSaveDataThreadID;

    static Event<HI_U32> *mEvt;
    static Event<HI_U32> *mFeedbackEvt;
    static list<HI_SAVE_DATA_S> mSaveListWait;
    static list<HI_SAVE_DATA_S> mSaveListDo;

    static HI_VOID *SaveDataThread(HI_VOID *arg);
    static HI_S32 Save(HI_U8 id, HI_U32 pos, const HI_U8 *data, HI_U32 size);

    static HI_S32 CreateEvent(HI_VOID);

    static HI_S32 DestroyEvent(HI_VOID);

    static HI_S32 DoEvent(HI_U32 event);
    static HI_S32 DoEventSub(HI_S32 &s32FileId, HI_CHAR *acResolvedPath, HI_S32 acResolvedPathLen);
};

inline HiStorageFile::HiStorageFile()
{
    mFileId = 0;
}

inline HiStorageFile::~HiStorageFile(){}

#endif

