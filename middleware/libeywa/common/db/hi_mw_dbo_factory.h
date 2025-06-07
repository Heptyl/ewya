/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __MW_DBO_FACTORY_H__
#define __MW_DBO_FACTORY_H__

#include "hi_mw_dbo.h"
#include "hi_mw_mutex.h"

class HiDBOFact {
public:
    static HiDBO *GetDBO();

private:
    static HiMutex mLock;
    static HiDBO *mCustomer;
};
#endif
