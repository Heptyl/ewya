/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#include "hi_mw_dbo_factory.h"

HiDBO *HiDBOFact::mCustomer = HI_NULL;
HiMutex HiDBOFact::mLock;

HiDBO *HiDBOFact::GetDBO()
{
    HiMutex::Autolock _l(mLock);

    if (mCustomer == HI_NULL) {
        mCustomer = new HiDBO();
    }

    return mCustomer;
}
