/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/


#define LOG_NDEBUG 0
#define LOG_TAG    "HiMW_db"

#include "hi_mw_db.h"
#include "hi_mw_db_def.h"
#include "hi_mw_common.h"
#include "hi_mw_log.h"

#include "hi_mw_error.h"

/* 数据布局:   uuid, length, data, checksum; uuid,length,data,checksum ... */
#define SAVE_LENGTH(u32Length)  (sizeof(HI_U32) + sizeof(HI_U32) + (u32Length) + sizeof(HI_U32))
#define LENG_POS(pData)         ((pData) + sizeof(HI_U32))
#define DATA_POS(pData)         ((pData) + sizeof(HI_U32) + sizeof(HI_U32))
// 添加uuid和length字段的长度，uuid,lenght,data一起计算得到checksum
#define CKSUM_LENGTH(u32Length) ((u32Length) + 2 * sizeof(HI_U32))

static HI_U32 CalcCheckSum(const HI_U8 *buf, HI_U32 bufLen)
{
    HI_U32 u32CheckSum = 0;

    for (HI_U32 i = 0; i < bufLen; i++) {
        u32CheckSum += buf[i];
    }

    return u32CheckSum;
}

HI_S32 HiDB::QueryVersion(HI_U16 &u16Version)
{
    HI_S32 s32Ret;
    HI_U32 u32Size = 0;

    if (m_pStorage == HI_NULL) {
        LOGE("null pointer.");
        return HI_FAILURE;
    }

    s32Ret = m_pStorage->Size(u32Size);
    if (s32Ret == HI_FAILURE) {
        LOGE("Error! can't get file size!");
        return HI_MW_DB_FILE_EER;
    }
    if (u32Size <= VERSION_LENGTH) {
        LOGW("First boot!");
        return HI_MW_DB_FIRSTBOOT_EER;
    }
    s32Ret = m_pStorage->Read(0, (HI_U8 *)&u16Version, VERSION_LENGTH);
    m_u16Version = u16Version;

    return s32Ret;
}

HI_S32 HiDB::InsertVersion(HI_U16 u16Version)
{
    HI_S32 s32Ret;

    if (m_pStorage == NULL) {
        LOGE("m_pVideoPlay is null");
        return HI_FAILURE;
    }

    s32Ret = m_pStorage->Write(0, (HI_U8 *)&u16Version, VERSION_LENGTH);
    m_u16Version = u16Version;

    return s32Ret;
}

HI_S32 HiDB::GetDataSize(HI_U32 &u32Size)
{
    HI_S32 s32Ret;

    if (m_pStorage == NULL) {
        LOGE("m_pVideoPlay is null");
        return HI_FAILURE;
    }

    s32Ret = m_pStorage->Size(u32Size);
    if (s32Ret == HI_FAILURE) {
        LOGE("Error! can't get file size!");
        return HI_MW_DB_FILE_EER;
    }

    return s32Ret;
}

HI_S32 HiDB::ReadDataAttr(HI_U32 &u32Pos, HI_MW_LOAD_ATTR_S &loadAttr)
{
    HI_S32 s32Ret;

    if (u32Pos + sizeof(loadAttr.u32Id) + sizeof(loadAttr.u32Length) > loadAttr.u32Size) {
        LOGE("Data error! u32Pos:0x%x", u32Pos);
        return HI_MW_DB_INIT_EER;
    }

    if (m_pStorage == NULL) {
        LOGE("m_pVideoPlay is null");
        return HI_FAILURE;
    }
    s32Ret = m_pStorage->Read(u32Pos, (HI_U8 *)(loadAttr.au8Data), sizeof(loadAttr.u32Id) + sizeof(loadAttr.u32Length));
    if (s32Ret != HI_SUCCESS) {
        LOGE("Data error! u32Pos:0x%x", u32Pos);
        return HI_MW_DB_INIT_EER;
    }

    loadAttr.u32Id = *(HI_U32 *)(loadAttr.au8Data);
    loadAttr.u32Length = *(HI_U32 *)(loadAttr.au8Data + 4); /* xxx is 4 */

    if (loadAttr.u32Length == 0) {
        LOGE("Data error! Length = 0, uuid:0x%x, Pos:0x%x", loadAttr.u32Id, u32Pos);
        return HI_MW_DB_INIT_EER;
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::ReadData(HI_U32 &u32Pos, HI_MW_LOAD_ATTR_S &loadAttr)
{
    HI_S32 s32Ret;

    if (u32Pos + 8 + loadAttr.u32Length + sizeof(loadAttr.u32Checksum) > loadAttr.u32Size) { /* offset is 8 */
        LOGE("Data error! u32Pos:0x%x", u32Pos);
        return HI_MW_DB_INIT_EER;
    }

    if (loadAttr.u32Length > (UINT_MAX - (sizeof(HI_U32) + sizeof(HI_U32) + sizeof(HI_U32)))) {
        LOGE("%d overflow", loadAttr.u32Length);
        delete[] loadAttr.pData;
        loadAttr.pData = HI_NULL;
        return HI_MW_DB_INIT_EER;
    }

    loadAttr.pData = new (std::nothrow) HI_U8[SAVE_LENGTH(loadAttr.u32Length)];
    if (loadAttr.pData == HI_NULL) {
        LOGE("Error, Malloc data failed!");
        return HI_MW_COM_MEMORY_EER;
    }

    if (m_pStorage == NULL) {
        LOGE("m_pVideoPlay is null");
        return HI_FAILURE;
    }
    s32Ret = m_pStorage->Read(u32Pos + 8, /* offset is 8 */
        DATA_POS(loadAttr.pData), loadAttr.u32Length + sizeof(loadAttr.u32Checksum));
    if (s32Ret != HI_SUCCESS) {
        LOGE("Data error! u32Pos:0x%x", u32Pos);
        delete[] loadAttr.pData;
        loadAttr.pData = HI_NULL;
        return HI_MW_DB_INIT_EER;
    }

    *(HI_U32 *)(loadAttr.pData) = loadAttr.u32Id;
    *(HI_U32 *)LENG_POS(loadAttr.pData) = loadAttr.u32Length;
    GetLengthFromID(loadAttr.u32Id >> ATTR_BW, loadAttr.u32NewLength);

    return HI_SUCCESS;
}

HI_S32 HiDB::VerifyData(HI_MW_LOAD_ATTR_S &loadAttr)
{
    // FIX codexbug.need verify !!
    if (loadAttr.pData == HI_NULL) {
        LOGE("loadAttr.pData is null");
        return HI_FAILURE;
    }

    loadAttr.u32Checksum = CalcCheckSum(loadAttr.pData, CKSUM_LENGTH(loadAttr.u32Length));
    loadAttr.u32Verify = VALIDFLAG + (loadAttr.u32Checksum & 0xFFFF);

    // 先检查数据有效性，如果数据无效且长度也变化了，则将该id对应的结构体恢复ini默认，此时length也会更新
    // 如果数据有效性通过，则检查数据长度
    if ((loadAttr.u32Verify != (*(HI_U32 *)(DATA_POS(loadAttr.pData) + loadAttr.u32Length))) ||
        (loadAttr.u32Length < loadAttr.u32NewLength)) {
        LOGW("u32Length=%d, u32NewLength=%d, id = 0x%x, %s",
            loadAttr.u32Length, loadAttr.u32NewLength, loadAttr.u32Id >> ATTR_BW,
            loadAttr.u32Length < loadAttr.u32NewLength ? "struct has new member" : "data is invalid");

        // 即使是数据无效也要用pNewData, 因为可能恰好此数据有新增成员，
        // restoreDbAttr得到的数据长度变大，pData无法存放
        loadAttr.pNewData = new (std::nothrow) HI_U8[SAVE_LENGTH(loadAttr.u32NewLength)];
        if (loadAttr.pNewData == HI_NULL) {
            LOGE("new failed");
            return HI_FAILURE;
        }
        // 从db.ini拿取默认数据
        RestoreDbAttr(loadAttr.u32Id, (HI_U8 *)DATA_POS(loadAttr.pNewData));

        // 如果结构体有新增成员，先从db.ini拿取默认数据
        if (loadAttr.u32Length < loadAttr.u32NewLength) {
            // 把前length长度的数据用db文件中的替换，保持之前设置的数据有效
            errno_t rc = memcpy_s(DATA_POS(loadAttr.pNewData), loadAttr.u32NewLength,
                DATA_POS(loadAttr.pData), loadAttr.u32Length);
            if (rc != EOK) {
                LOGE("memcpy_s_func failed rc = %d", rc);
                return HI_FAILURE;
            }
        }

        // 填充uuid，length
        *(HI_U32 *)(loadAttr.pNewData) = loadAttr.u32Id;
        *(HI_U32 *)LENG_POS(loadAttr.pNewData) = loadAttr.u32NewLength;
        // update checksum
        loadAttr.u32Checksum = CalcCheckSum(loadAttr.pNewData, CKSUM_LENGTH(loadAttr.u32NewLength));
        loadAttr.u32Verify = VALIDFLAG + (loadAttr.u32Checksum & 0xFFFF);
        (*(HI_U32 *)(DATA_POS(loadAttr.pNewData) + loadAttr.u32NewLength)) = loadAttr.u32Verify;

        if (loadAttr.pData != HI_NULL) {
            delete[] loadAttr.pData;
            loadAttr.pData = HI_NULL;
        }

        loadAttr.pData = loadAttr.pNewData;
        m_bReWrite = HI_TRUE;
    } else if (loadAttr.u32Length > loadAttr.u32NewLength) {
        LOGW("u32Length=%d, u32NewLength=%d, id = 0x%x, struct delete some member",
            loadAttr.u32Length, loadAttr.u32NewLength, loadAttr.u32Id >> ATTR_BW);

        RestoreDbAttr(loadAttr.u32Id, (HI_U8 *)DATA_POS(loadAttr.pData));

        // 填充length
        (*(HI_U32 *)LENG_POS(loadAttr.pData)) = loadAttr.u32NewLength;
        // update checksum
        loadAttr.u32Checksum = CalcCheckSum(loadAttr.pData, CKSUM_LENGTH(loadAttr.u32NewLength));
        loadAttr.u32Verify = VALIDFLAG + (loadAttr.u32Checksum & 0xFFFF);
        (*(HI_U32 *)(DATA_POS(loadAttr.pData) + loadAttr.u32NewLength)) = loadAttr.u32Verify;

        m_bReWrite = HI_TRUE;
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::LoadData(HI_U32 &u32Pos, HI_MW_LOAD_ATTR_S &loadAttr)
{
    /* load Info data to memory */
    Data *data = new (std::nothrow) Data;
    if (data == HI_NULL) {
        LOGE("Error, Malloc data failed!");
        delete[] loadAttr.pData;
        loadAttr.pData = HI_NULL;
        return HI_MW_COM_MEMORY_EER;
    }
    data->pu8Meta = loadAttr.pData;
    data->u32Pos = u32Pos;

    /* load Attr data to memory */
    Info *info = HI_NULL;
    if (m_mapData.find(loadAttr.u32Id >> ATTR_BW) == m_mapData.end()) {
        info = new (std::nothrow) Info;
        if (info == HI_NULL) {
            LOGE("Error, Malloc data failed!");
            delete[] loadAttr.pData;
            loadAttr.pData = HI_NULL;
            delete data;
            data = HI_NULL;
            return HI_MW_COM_MEMORY_EER;
        }
        info->bSave = HI_TRUE;
        m_mapData[loadAttr.u32Id >> ATTR_BW] = info;
    } else {
        info = m_mapData[loadAttr.u32Id >> ATTR_BW];
    }
    info->data.push_back(data);

    return HI_SUCCESS;
}

/*
 1.单板开机时，如果存在db文件就调用load()将db文件中的数据加载到内存，如果不存在db文件则调用insert()
从ini读取默认数据到内存，并生成db文件。
 2.加载过程会检查[每个数据节点]的checksum校验和数据长度检查
   1). checksum校验失败: 直接从ini加载该数据节点的默认值，直接替换此数据节点。
如果此数据节点对应的结构体有新增成员，则此数据节点的数据长度也会更新；
   2). 如果checksum校验成功，则进一步检查数据长度：如果newLength > oldLength，
则说明此数据节点对应的结构体有新增成员，则先从ini读取此节点的默认值，
       再把此数据节点的前面oldLength长度的数据用db原有的数据覆盖，以保证之前设置依然有效，
       其后面的(newLength-oldLength)部分用ini默认值赋值。
 3.约束: 对于支持在线升级，要求不删除原有用户数据的规格，必须满足以下约束条件。
 若不需支持此规格，则不需满足以下约束条件。
   1).新增数据库数据项时，只允许在数据节点对应的结构体的最后面追加新成员，不能在前面插入新的结构体成员。
   2).不能替换结构体已有的成员，否则会导致数据错乱。
   3).不能删除结构体已有的成员，如果删除结构体中现有的成员，则会导致该结构体对应的数据节点被替换为ini默认值。
*/
HI_S32 HiDB::Load()
{
    HI_S32 s32Ret;
    HI_MW_LOAD_ATTR_S loadAttr = {};

    HiRWLock::AutoWLock l(m_lockMap);

    /* 数据是否重新加载 */
    if (m_mapData.size() != 0) {
        LOGE("Error! Repeat load!");
        return HI_MW_COM_ALREADY_EER;
    }

    if (m_pStorage == HI_NULL) {
        LOGE("m_pStorage is HI_NULL.");
        return HI_FAILURE;
    }

    /* 读取版本信息 */
    s32Ret = m_pStorage->Size(loadAttr.u32Size);
    if (s32Ret != HI_SUCCESS) {
        LOGE("Error! can't get file size!");
        return HI_MW_DB_FILE_EER;
    }

    if (loadAttr.u32Size <= VERSION_LENGTH) {
        LOGW("First boot!");
        return HI_MW_DB_FIRSTBOOT_EER;
    }
    s32Ret = m_pStorage->Read(0, (HI_U8 *)&m_u16Version, VERSION_LENGTH);

    /* 读取NVM 数据加载到内存中 */
    for (HI_U32 u32Pos = VERSION_LENGTH; u32Pos < loadAttr.u32Size;) {
        /* 1. read uuid and data length */
        s32Ret = ReadDataAttr(u32Pos, loadAttr);
        if (s32Ret != HI_SUCCESS) {
            LOGE("ReadDataAttr fail, s32Ret:0x%x", s32Ret);
            return s32Ret;
        }

        /* 2. read data */
        s32Ret = ReadData(u32Pos, loadAttr);
        if (s32Ret != HI_SUCCESS) {
            LOGE("ReadData fail, s32Ret:0x%x", s32Ret);
            return s32Ret;
        }

        /* 3. Verify data */
        VerifyData(loadAttr);

        /* 4. Load data to memory */
        LoadData(u32Pos, loadAttr);
        if (s32Ret != HI_SUCCESS) {
            LOGE("LoadData fail, s32Ret:0x%x", s32Ret);
            return s32Ret;
        }

        /* 5. move datapoint to next data */
        u32Pos += 8 + loadAttr.u32Length + sizeof(loadAttr.u32Verify); /* offset is 8 */
    }

    if (m_bReWrite == HI_TRUE) {
        Rewrite();
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::Unload()
{
    HiRWLock::AutoWLock l(m_lockMap);

    map<HI_U32, Info *>::iterator it;
    map<HI_U32, Info *>::iterator start = m_mapData.begin();
    map<HI_U32, Info *>::iterator end = m_mapData.end();

    for (it = start; it != end; ++it) {
        /* 卸载内存中的数据 */
        if (it->second != HI_NULL) {
            // HiRWLock::AutoWLock l(it->second->lock);
            list<Data *>::iterator p;
            for (p = it->second->data.begin(); p != it->second->data.end();) {
                /* 卸载it->first 属性对应的所有数据 */
                {
                    // HiRWLock::AutoWLock lock((*p)->lock);
                    delete (*p)->pu8Meta;
                    (*p)->pu8Meta = NULL;
                }
                delete (*p);
                *p = NULL;
                it->second->data.erase(p++);
            }
        }
        delete (it->second);
        it->second = NULL;
    }
    m_mapData.clear();

    return HI_SUCCESS;
}

HI_S32 HiDB::CreateRecord(HI_U32 u32Id)
{
    HiRWLock::AutoWLock l(m_lockMap);

    map<HI_U32, Info *>::iterator it = m_mapData.find(u32Id);
    if (it != m_mapData.end()) {
        LOGW("warning, the record has exist!");
        return HI_SUCCESS;
    }

    m_mapData[u32Id] = HI_NULL;

    return HI_SUCCESS;
}

HI_S32 HiDB::HasRecord(HI_U32 u32Id, HI_BOOL *pbExist)
{
    if (u32Id > ATTR_MAX || pbExist == NULL) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }

    HiRWLock::AutoWLock l(m_lockMap);

    map<HI_U32, Info *>::iterator it = m_mapData.find(u32Id);
    *pbExist = (it == m_mapData.end() ? HI_FALSE : HI_TRUE);

    return HI_SUCCESS;
}

HI_S32 HiDB::DestroyRecord(HI_U32 u32Id)
{
    HiRWLock::AutoWLock l(m_lockMap);

    map<HI_U32, Info *>::iterator it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, destroy record error!");
        return HI_MW_DB_RECORD_EER;
    }

    m_mapData.erase(u32Id);

    return HI_SUCCESS;
}

HI_S32 HiDB::EnableSync(HI_U32 u32Id, HI_BOOL bFlush)
{
    if (u32Id > ATTR_MAX || m_pStorage == NULL) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }

    HiRWLock::AutoWLock all(m_lockMap);
    map<HI_U32, Info *>::iterator it;

    /* 必须创建该条记录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, The u32Id = 0x%x is not exist, please create this record first "
            "with CreateRecord function!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }

    /* 数据没有插入到内存 */
    if (it->second == HI_NULL) {
        Info *info = new (std::nothrow) Info;
        if (info == HI_NULL) {
            LOGE("Error, Malloc data failed!");
            return HI_MW_COM_MEMORY_EER;
        }
        info->bSave = HI_TRUE;
        m_mapData[u32Id] = info;

        return HI_SUCCESS;
    }

    /* 数据已经插入到内存 */
    HiRWLock::AutoWLock l(it->second->lock);
    if (it->second->bSave) {
        LOGW("The current status is right!");

        return HI_MW_COM_ALREADY_EER;
    }

    it->second->bSave = HI_TRUE;
    if (!bFlush) {
        return HI_SUCCESS;
    }

    /* 数据已经插入到内存，刷新一遍数据 */
    list<Data *>::iterator p;
    for (p = it->second->data.begin(); p != it->second->data.end(); ++p) {
        HiRWLock::AutoWLock lockData((*p)->lock);
        if ((*p)->pu8Meta == HI_NULL) {
            LOGE("Error, Data is null!");
            return HI_MW_DB_DATALIST_EER;
        }
        /* pos将不为初始值，插入数据后初次刷新确定pos位置 */
        if ((*p)->u32Pos == INVALIDPOS) {
            (*p)->u32Pos = NewPos();
        }
        m_pStorage->Write((*p)->u32Pos, (*p)->pu8Meta, SAVE_LENGTH(*(HI_U32 *)LENG_POS((*p)->pu8Meta)));
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::DisableSync(HI_U32 u32Id)
{
    if (u32Id > ATTR_MAX) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }

    HiRWLock::AutoRLock all(m_lockMap);

    map<HI_U32, Info *>::iterator it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, The u32Id = 0x%x has't been create!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    HiRWLock::AutoWLock l(it->second->lock);
    it->second->bSave = HI_FALSE;

    return HI_SUCCESS;
}

HI_S32 HiDB::GetRecords(list<HI_U32> &listRecords)
{
    HiRWLock::AutoRLock l(m_lockMap);

    map<HI_U32, Info *>::iterator it;
    map<HI_U32, Info *>::iterator start = m_mapData.begin();
    map<HI_U32, Info *>::iterator end = m_mapData.end();
    for (it = start; it != end; ++it) {
        listRecords.push_back(it->first);
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::Insert(HI_U32 u32Id, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Num, HI_BOOL bSync)
{
    if (u32Id > ATTR_MAX || pData == NULL || u32Num > NUMMAX) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    HiRWLock::AutoWLock all(m_lockMap);
    map<HI_U32, Info *>::iterator it;
    Info *info = HI_NULL;
    HI_U32 u32CurrentNum;

    /* 判断是否创建纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to insert data!", u32Id);
        return HI_MW_COM_PARAM_EER;
    }

    info = it->second;
    if (info == HI_NULL) {
        info = new (std::nothrow) Info;
        if (info == HI_NULL) {
            LOGE("Error, Malloc data failed!");
            return HI_MW_COM_MEMORY_EER;
        }
        info->bSave = bSync;
        m_mapData[u32Id] = info;
    }

    //   HiRWLock::AutoWLock l(info->lock);
    /* 插入数据数量是否超出负载能力 */
    u32CurrentNum = info->data.size();
    if ((u32Num + u32CurrentNum) > NUMMAX) {
        LOGE("Error, num to insert overflow!");
        return HI_MW_COM_PARAM_EER;
    }

    /* 插入数据 */
    HI_S32 ret;
    for (HI_U32 i = 0; i < u32Num; i++) {
        ret = InsertData(u32Id, pData, u32Length, info, u32CurrentNum + i);
        if (ret != HI_SUCCESS) {
            LOGE("InsertData 0x%x failed", u32Id);
            return ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::Erase(HI_U32 u32Id, HI_U32 u32Row)
{
    if (u32Id > ATTR_MAX) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    map<HI_U32, Info *>::iterator it;

    /* lock all data */
    HiRWLock::AutoWLock l(m_lockMap);

    /* 判断是否有该条纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to erase any data!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    if (it->second == HI_NULL) {
        LOGE("Error, The u32Id (%x) record number is 0!", u32Id);
        return HI_MW_DB_DATALIST_EER;
    }

    /* lock meta data */
    list<Data *>::iterator p = it->second->data.begin();
    list<Data *>::iterator next;
    if (it->second->data.size() <= u32Row) {
        LOGE("Error, The u32Id record number is lower that u32Row!");
        return HI_MW_DB_DATALIST_EER;
    }
    advance(p, u32Row);
    next = p;
    ++next;
    /* 删除当前条，把后面的条数号-1 */
    while (next != it->second->data.end()) {
        *(HI_U32 *)((*next)->pu8Meta) = *(HI_U32 *)((*next)->pu8Meta) - 1;
        ++next;
    }
    if ((*p)->pu8Meta != HI_NULL) {
        delete[](*p)->pu8Meta;
        (*p)->pu8Meta = HI_NULL;
    }
    it->second->data.erase(p);

    if (m_pStorage == HI_NULL) {
        LOGE("m_pStorage is HI_NULL.");
        return HI_FAILURE;
    }
    m_pStorage->Clear();

    Rewrite();

    return HI_SUCCESS;
}

HI_S32 HiDB::Erase(HI_U32 u32Id)
{
    if (u32Id > ATTR_MAX) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }

    /* lock all data */
    HiRWLock::AutoWLock l(m_lockMap);

    map<HI_U32, Info *>::iterator it;

    /* 判断是否有该条纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to erase any data!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    if (it->second == HI_NULL) {
        LOGE("Error, The u32Id (%x) record number is 0!", u32Id);
        return HI_MW_DB_DATALIST_EER;
    }

    /* lock meta data */
    list<Data *>::iterator iter = it->second->data.begin();
    for (; iter != it->second->data.end(); iter++) {
        if ((*iter)->pu8Meta != HI_NULL) {
            delete[](*iter)->pu8Meta;
            (*iter)->pu8Meta = HI_NULL;
        }
    }
    it->second->data.clear();

    if (m_pStorage == HI_NULL) {
        LOGE("m_pStorage is HI_NULL.");
        return HI_FAILURE;
    }
    m_pStorage->Clear();
    Rewrite();

    return HI_SUCCESS;
}

HI_S32 HiDB::Clear()
{
    Unload();

    if (m_pStorage == HI_NULL) {
        LOGE("m_pStorage is HI_NULL.");
        return HI_FAILURE;
    }

    return m_pStorage->Clear();
}

HI_S32 HiDB::Query(HI_U32 u32Id, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row)
{
    if (u32Id > ATTR_MAX || pData == NULL || u32Row > NUMMAX) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    HiRWLock::AutoRLock l(m_lockMap);
    map<HI_U32, Info *>::iterator it;

    /* 判断是否有该条纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to query any data!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    if (it->second == HI_NULL) {
        LOGE("Error, The u32Id (0x%x) record number is 0!", u32Id);
        return HI_MW_DB_DATALIST_EER;
    }
    HiRWLock::AutoRLock rDataLock(it->second->lock);
    list<Data *>::iterator p = it->second->data.begin();
    if (it->second->data.size() <= u32Row) {
        LOGE("Error, The u32Id record number is lower than u32Row!--- u32Id = [0x%x], u32Row = [%d] ---",
            u32Id, u32Row);

        return HI_MW_DB_DATALIST_EER;
    }

    advance(p, u32Row);

    HiRWLock::AutoRLock rMetaLock((*p)->lock);
    if ((*(HI_U32 *)LENG_POS((*p)->pu8Meta)) != u32Length) {
        LOGW("warning, id (0x%x) Input parameter u32Length old,new(%d,%d) invalid!",
             u32Id, (*(HI_U32 *)LENG_POS((*p)->pu8Meta)), u32Length);
        if ((*(HI_U32 *)LENG_POS((*p)->pu8Meta)) < u32Length) {
            u32Length = (*(HI_U32 *)LENG_POS((*p)->pu8Meta));
        }
    }
    errno_t rc = memcpy_s(pData, u32Length, DATA_POS((*p)->pu8Meta), u32Length);
    if (rc != EOK) {
        LOGE("memcpy_s_func failed rc = %d", rc);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::Query(HI_U32 u32Id, HI_U8 *pData, HI_U32 u32Length, HI_U32 u32StartRow, HI_U32 u32EndRow)
{
    if (u32Id > ATTR_MAX || pData == NULL || u32StartRow > NUMMAX || u32EndRow > NUMMAX || u32StartRow > u32EndRow) {
        LOGE("Error, Input parameter invalid! Id:0x%x, pData:%p, StartRow:%d, EndRow:%d", u32Id, pData, u32StartRow,
             u32EndRow);
        return HI_MW_COM_PARAM_EER;
    }
    HiRWLock::AutoRLock l(m_lockMap);
    map<HI_U32, Info *>::iterator it;

    /* 判断是否有该条纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to query any data!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    if (it->second == HI_NULL) {
        LOGE("Error, The u32Id (0x%x) record number is 0!", u32Id);
        return HI_MW_DB_DATALIST_EER;
    }
    HiRWLock::AutoRLock rDataLock(it->second->lock);
    list<Data *>::iterator p = it->second->data.begin();
    if (it->second->data.size() <= u32StartRow) {
        LOGE("Error, The u32Id record number is lower than u32Row!--- u32Id = [0x%x], u32Row = [%d] ---", u32Id,
             u32EndRow);

        return HI_MW_DB_DATALIST_EER;
    }

    advance(p, u32StartRow);
    for (HI_U32 i = u32StartRow; i <= u32EndRow && p != it->second->data.end(); i++) {
        HiRWLock::AutoRLock rMetaLock((*p)->lock);
        if ((*(HI_U32 *)LENG_POS((*p)->pu8Meta)) != u32Length) {
            LOGW("warning, id (0x%x) Input parameter u32Length old,new(%d,%d) invalid!",
                 u32Id, (*(HI_U32 *)LENG_POS((*p)->pu8Meta)), u32Length);
            if ((*(HI_U32 *)LENG_POS((*p)->pu8Meta)) < u32Length) {
                u32Length = (*(HI_U32 *)LENG_POS((*p)->pu8Meta));
            }
        }
        errno_t rc = memcpy_s(pData, u32Length, DATA_POS((*p)->pu8Meta), u32Length);
        if (rc != EOK) {
            LOGE("memcpy_s_func failed rc = %d", rc);
            return HI_FAILURE;
        }
        pData += u32Length;
        ++p;
    }
    return HI_SUCCESS;
}

HI_S32 HiDB::Update(HI_U32 u32Id, const HI_U8 *pData, HI_U32 u32Length, HI_U32 u32Row)
{
    if (u32Id > ATTR_MAX || pData == NULL || u32Row > NUMMAX) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    HiRWLock::AutoRLock l(m_lockMap);
    map<HI_U32, Info *>::iterator it;

    /* 判断是否有该条纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to update any data!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    if (it->second == HI_NULL) {
        LOGE("Error, The u32Id (0x%x) record number is 0!", u32Id);
        return HI_MW_DB_DATALIST_EER;
    }
    HiRWLock::AutoRLock rDataLock(it->second->lock);
    list<Data *>::iterator p = it->second->data.begin();
    if (it->second->data.size() <= u32Row) {
        LOGE("Error, The u32Id record number is lower than u32Row!--- u32Id = [0x%x], u32Row = [%d] ---",
            u32Id, u32Row);
        return HI_MW_DB_DATALIST_EER;
    }

    advance(p, u32Row);

    HiRWLock::AutoWLock wMetaLock((*p)->lock);
    if (u32Length != (*(HI_U32 *)LENG_POS((*p)->pu8Meta))) {
        LOGE("Error, Input parameter u32Length old,new(%d,%d) invalid!",
             (*(HI_U32 *)LENG_POS((*p)->pu8Meta)), u32Length);
        return HI_MW_COM_PARAM_EER;
    }

    HI_U32 u32Checksum;
    HI_U32 u32Verify;
    errno_t rc = memcpy_s(DATA_POS((*p)->pu8Meta), u32Length, pData, u32Length);
    if (rc != EOK) {
        LOGE("memcpy_s_func failed rc = %d", rc);
        return HI_FAILURE;
    }
    u32Checksum = CalcCheckSum((*p)->pu8Meta, CKSUM_LENGTH(u32Length));
    u32Verify = VALIDFLAG + (u32Checksum & 0xFFFF);
    (*(HI_U32 *)(DATA_POS((*p)->pu8Meta) + u32Length)) = u32Verify;
    if (it->second->bSave) {
        if (m_pStorage == HI_NULL) {
            LOGE("m_pStorage is HI_NULL.");
            return HI_FAILURE;
        }

        m_pStorage->Write((*p)->u32Pos, (*p)->pu8Meta, SAVE_LENGTH(u32Length));
    }

    return HI_SUCCESS;
}

HI_S32 HiDB::GetRecordNum(HI_U32 u32Id, HI_U32 *pu32Num)
{
    if (u32Id > ATTR_MAX || pu32Num == NULL) {
        LOGE("Error, Input parameter invalid!");
        return HI_MW_COM_PARAM_EER;
    }
    HiRWLock::AutoRLock l(m_lockMap);
    map<HI_U32, Info *>::iterator it;

    /* 判断是否有该条纪录 */
    it = m_mapData.find(u32Id);
    if (it == m_mapData.end()) {
        LOGE("Error, Create u32Id = 0x%x record with CreateRecord function before to GetRecordNum!", u32Id);
        return HI_MW_DB_RECORD_EER;
    }
    if (it->second == HI_NULL) {
        LOGE("Error, The u32Id (%x) record number is 0!", u32Id);
        return HI_MW_DB_DATALIST_EER;
    }
    HiRWLock::AutoRLock rDataLock(it->second->lock);
    *pu32Num = it->second->data.size();

    return HI_SUCCESS;
}

HI_U32 HiDB::NewPos()
{
    HI_U32 u32Pos = VERSION_LENGTH;
    HI_U32 u32Length = 0;
    map<HI_U32, Info *>::iterator it;
    map<HI_U32, Info *>::iterator start = m_mapData.begin();
    map<HI_U32, Info *>::iterator end = m_mapData.end();

    for (it = start; it != end; ++it) {
        list<Data *>::iterator p;
        if (it->second == HI_NULL) {
            continue;
        }
        for (p = it->second->data.begin(); p != it->second->data.end(); ++p) {
            /* 找到当前存储的最大位置 */
            if (((*p)->u32Pos != INVALIDPOS) && ((*p)->u32Pos >= u32Pos)) {
                u32Pos = (*p)->u32Pos;
                u32Length = SAVE_LENGTH(*(HI_U32 *)((*p)->pu8Meta + sizeof(HI_U32)));
            }
        }
    }

    return u32Pos + u32Length;
}

HI_VOID HiDB::Rewrite()
{
    if (m_pStorage == HI_NULL) {
        LOGE("m_pStorage is HI_NULL.");
        return;
    }

    InsertVersion(m_u16Version);

    map<HI_U32, Info *>::iterator it;
    map<HI_U32, Info *>::iterator start = m_mapData.begin();
    map<HI_U32, Info *>::iterator end = m_mapData.end();
    HI_U32 u32Pos = VERSION_LENGTH;

    for (it = start; it != end; ++it) {
        if (it->second != HI_NULL) {
            list<Data *>::iterator p;
            for (p = it->second->data.begin(); p != it->second->data.end(); ++p) {
                (*p)->u32Pos = u32Pos;

                m_pStorage->Write ((*p)->u32Pos, (*p)->pu8Meta,
                    SAVE_LENGTH(*(HI_U32 *)((*p)->pu8Meta + sizeof(HI_U32))));
                u32Pos += SAVE_LENGTH (*(HI_U32 *)((*p)->pu8Meta + sizeof(HI_U32)));
            }
        }
    }
}

HI_VOID HiDB::PrintAttrs()
{
    HI_U32 u32Size = 0;
    map<HI_U32, Info *>::iterator it;
    map<HI_U32, Info *>::iterator start = m_mapData.begin();
    map<HI_U32, Info *>::iterator end = m_mapData.end();
    for (it = start; it != end; ++it) {
        list<Data *>::iterator p;
        if (it->second != HI_NULL) {
            u32Size = it->second->data.size();
            LOGI("[attr, size] = [0x%x, %d]", it->first, u32Size);
            for (p = it->second->data.begin(); p != it->second->data.end(); ++p) {
                LOGI ("[id, length] = [0x%x, %d]", *(HI_U32 *)((*p)->pu8Meta),
                    *(HI_U32 *)((*p)->pu8Meta + sizeof(HI_U32)));
            }
        } else {
            u32Size = 0;
        }
    }
}

HI_S32 HiDB::InsertData(HI_U32 u32Id, const HI_U8 *pData, HI_U32 u32Length, Info *info, HI_U32 idex)
{
    if (pData == NULL || info == HI_NULL) {
        LOGE("null pointer.");
        return HI_FAILURE;
    }

    HI_U32 u32Checksum;
    HI_U32 u32Verify;
    HI_U8 *addr = new (std::nothrow) HI_U8[SAVE_LENGTH(u32Length)];
    if (addr == HI_NULL) {
        LOGI("Error, Malloc data failed!");
        return HI_MW_COM_MEMORY_EER;
    }
    *(HI_U32 *)addr = (u32Id << ATTR_BW) + idex;
    *(HI_U32 *)LENG_POS(addr) = u32Length;
    errno_t rc = memcpy_s(DATA_POS(addr), u32Length, pData, u32Length);
    if (rc != EOK) {
        LOGE("memcpy_s_func failed rc = %d", rc);
        delete[] addr;
        addr = HI_NULL;
        return HI_FAILURE;
    }
    u32Checksum = CalcCheckSum(addr, CKSUM_LENGTH(u32Length));
    u32Verify = VALIDFLAG + (u32Checksum & 0xFFFF);
    (*(HI_U32 *)(DATA_POS(addr) + u32Length)) = u32Verify;

    Data *data = new (std::nothrow) Data;
    if (data == HI_NULL) {
        LOGI("Error, Malloc data failed!");
        delete[] addr;
        addr = HI_NULL;
        return HI_MW_COM_MEMORY_EER;
    }
    data->u32Pos = INVALIDPOS;
    data->pu8Meta = addr;

    if (info->bSave) {
        data->u32Pos = NewPos();

        if (m_pStorage == HI_NULL) {
            LOGE("m_pStorage is HI_NULL.");
            delete[] addr;
            delete data;
            return HI_FAILURE;
        }
        m_pStorage->Write(data->u32Pos, data->pu8Meta, SAVE_LENGTH(u32Length));
    }
    info->data.push_back(data);
    return HI_SUCCESS;
}

