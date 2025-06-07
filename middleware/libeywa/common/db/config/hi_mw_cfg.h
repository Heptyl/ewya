/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#ifndef __HI_MW_CFG_H__
#define __HI_MW_CFG_H__

#include "hi_mw_ini_provider.h"
#include "hi_mw_mutex.h"

class HiCfgFact {
public:
    static INIProvider *Get();

private:
    static INIProvider *mpProvider;
    static HiMutex mmtxProvider;
};

#endif /* __HI_MW_CFG_H__ */

