/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
* Description:
* Author:
* Create:
*/
#ifndef __HI_MW_ERROR_H__
#define __HI_MW_ERROR_H__

/* common error */
#define HI_MW_COM_PARAM_EER   (HI_S32)(0x80000001) /* input parameter error */
#define HI_MW_COM_INIT_ERR    (HI_S32)(0x80000002) /* input init error */
#define HI_MW_COM_MEMORY_EER  (HI_S32)(0x80000003) /* malloc data error */
#define HI_MW_COM_ALREADY_EER (HI_S32)(0x80000004) /* alread execute, successful status */

/* database error */
#define HI_MW_DB_COMMON_EER    (HI_S32)(0x80100000) /* common error */
#define HI_MW_DB_FIRSTBOOT_EER (HI_S32)(0x80100001) /* data firstboot error */
#define HI_MW_DB_INIT_EER      (HI_S32)(0x80100002) /* data Init error */
#define HI_MW_DB_FILE_EER      (HI_S32)(0x80100003) /* file error */
#define HI_MW_DB_DATA_EER      (HI_S32)(0x80100004) /* data error */
#define HI_MW_DB_DATALIST_EER  (HI_S32)(0x80100005) /* no the uuid data */
#define HI_MW_DB_RECORD_EER    (HI_S32)(0x80100006) /* record error */


#endif
