/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/

#include "hi_mw_cfg.h"

INIProvider *HiCfgFact::mpProvider = NULL;
HiMutex HiCfgFact::mmtxProvider;

INIProvider *HiCfgFact::Get()
{
    HiMutex::Autolock _l(mmtxProvider);
    if (mpProvider == NULL) {
        mpProvider = new INIProvider();
    }
    return mpProvider;
}
