/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_DB_H__
#define __HI_MW_DB_H__

#include <string>
#include <map>

#include "hi_type.h"
#include "hi_mw_storage_file.h"
#include "hi_mw_mutex.h"
#include "hi_mw_rwlock.h"
#include "hi_mw_log.h"

using namespace std;

struct HI_MW_LOAD_ATTR_S {
    HI_U32 u32Size;
    HI_U8 au8Data[8];  // 存uuid和length size is 8
    HI_U32 u32Id;
    HI_U32 u32Length;
    HI_U8 *pData;
    HI_U32 u32NewLength;
    HI_U8 *pNewData;
    HI_U32 u32Checksum;
    HI_U32 u32Verify;
};

class HiDB {
public:
    explicit HiDB(HiStorageFile *storage);
    virtual ~HiDB(){}

    /* 加载NVM 中指定的NVM 数据到内存 */
    HI_S32 QueryVersion(HI_U16 &u16Version);

    /* 插入版本号 */
    HI_S32 InsertVersion(HI_U16 u16Version);

    /* 获取数据大小 */
    HI_S32 GetDataSize(HI_U32 &u32Size);

    /* 加载NVM 中指定的NVM 数据到内存 */
    HI_S32 Load();

    /* 删除内存中数据对应所有内存数据 */
    HI_S32 Unload();

    /* 创建记录项，创建要存储的记录项 */
    HI_S32 CreateRecord(HI_U32 u32Id);

    /* 销毁记录项 */
    HI_S32 DestroyRecord(HI_U32 u32Id);

    /* 查找记录是否存在 */
    HI_S32 HasRecord(HI_U32 u32Id, HI_BOOL *pbExist);

    /* 获取记录项 */
    HI_S32 GetRecords(list<HI_U32> &listRecords);

    /* 插入数据，默认插入一条数据, 插入的数据同步到NVM 中。 */
    HI_S32 Insert(HI_U32 u32Id, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Num = 1, HI_BOOL bSync = HI_TRUE);

    /* 删除enAttr 属性中的第u32Row 条记录, 该函数会删除NVM 中对应的数据，执行时间较长 */
    HI_S32 Erase(HI_U32 u32Id, HI_U32 u32Row);

    /* 删除enAttr 属性中的所有记录, 该函数会删除NVM 中对应的数据，执行时间较长 */
    HI_S32 Erase(HI_U32 u32Id);

    /* 清除所有数据，包括内存中和NVM 中的数据 */
    HI_S32 Clear();

    /* 数据将同步存储在NVM 中, bFlush 为真时 立即刷新一遍数据 */
    HI_S32 EnableSync(HI_U32 u32Id, HI_BOOL bFlush = HI_FALSE);

    /* 数据只存内存，将不在NVM 中保存 */
    HI_S32 DisableSync(HI_U32 u32Id);

    /* 获取记录条数 */
    HI_S32 GetRecordNum(HI_U32 u32Id, HI_U32 *pu32Num);

    /* 查询enAttr 属性中第u32Row 条的数据 */
    HI_S32 Query(HI_U32 u32Id, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row = 0);
    /* 查询enAttr 属性中第u32StartRow 条到u32EndRow条的数据 */
    HI_S32 Query(HI_U32 u32Id, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32StartRow, HI_U32 u32EndRow);
    /* 更新enAttr 属性第u32Row 条数据 */
    HI_S32 Update(HI_U32 u32Id, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row = 0);

private:
    HI_VOID Rewrite();

    HI_U32 NewPos();

    HI_VOID PrintAttrs();

    HI_S32 ReadDataAttr(HI_U32 &u32Pos, HI_MW_LOAD_ATTR_S &loadAttr);
    HI_S32 ReadData(HI_U32 &u32Pos, HI_MW_LOAD_ATTR_S &loadAttr);
    HI_S32 VerifyData(HI_MW_LOAD_ATTR_S &loadAttr);
    HI_S32 LoadData(HI_U32 &u32Pos, HI_MW_LOAD_ATTR_S &loadAttr);

private:
    /* 基本数据单元 */
    struct Data {
        HiRWLock lock;
        HI_U8 *pu8Meta;
        HI_U32 u32Pos;
    };

    /* 属性对应的所有数据 */
    struct Info {
        HiRWLock lock;
        HI_BOOL bSave;
        list<Data *> data;
    };

    HI_S32 InsertData(HI_U32 u32Id, const HI_U8 *pData, HI_U32 u32Length, Info *info, HI_U32 idex);

private:
    static const HI_U32 ATTR_BW = 16;
    static const HI_U32 VERSION_LENGTH = 2;
    static const HI_U32 ATTR_MAX = 0xFFFF;
    static const HI_U32 NUMMAX = 0xFFFF;
    static const HI_U32 INVALID_VERSION = 0xFFFF;
    static const HI_U32 INVALIDPOS = -1;
    static const HI_U32 VALIDFLAG = 0xAAAA0000;

private:
    HiRWLock m_lockMap;
    map<HI_U32, Info *> m_mapData;
    HiStorageFile *m_pStorage;
    HI_U16 m_u16Version;
    HI_BOOL m_bReWrite;
};

inline HiDB::HiDB(HiStorageFile *storage) :
    m_pStorage(storage), m_u16Version(INVALID_VERSION), m_bReWrite(HI_FALSE) {};

#endif
