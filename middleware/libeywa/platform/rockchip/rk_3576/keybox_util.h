/*
 * Copyright (c) 2016, Rockchip Technology Co., Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef KEYBOX_UTIL_H_
#define KEYBOX_UTIL_H_


#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "id_attestation.h"
#define VERSION_STRING             "Version_1.0_20240118"
#define TEEC_SUCCESS                       0x00000000
#define TEEC_ERROR_ITEM_NOT_FOUND          0xFFFF0008
#define TEE_ERROR_SECURITY                 0xFFFF000F

 //==============================================================================
 //WIDEVINE KEYBOX API
 //==============================================================================

  /**
   * read widevine keybox
   *
   * @param[out] device_id: keybox of device id field,32 bytes.
   * @param[out] system_id: keybox of system id field,4 bytes.
   *
   * @retval 0 success
   * @retval -1 or other value, failed
   */
int rk_read_wv_keybox(uint8_t* device_id, uint8_t* system_id);

/**
 * install widevine keybox
 *
 * @param[in] keybox_path: the encrypted widevine keybox file path.
 *
 * @retval 0 success
 * @retval -1 or other value, failed
 */
int rk_install_wv_keybox(const char* keybox_path);

/**
 * delete widevine keybox
 *
 * @retval 0 success
 * @retval -1 or other value, failed
 */
int rk_delete_wv_keybox();

//==============================================================================
//ATTESTATION KEYBOX API
//==============================================================================
/**
  * read attestation keybox
  *
  * @param[out] device_id: keybox device id field.
  * @param[out] device_id_len: length of keybox device id field.
  *
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_read_attestation_keybox(uint8_t* device_id,int *device_id_len);

/**
  * read attestation keybox
  * for compatiable,window OS toolbox write keybox without device_id,so we can get keybox length
  * to check is keybox it written.
  * so,if keybox_len is equal 3,keybox has written,others is not writen,even if
  * device_id may be null and device_id_len is 0.

  * @param[out] keybox_len: keybox length field.
  * @param[out] device_id: keybox device id field.
  * @param[out] device_id_len: length of keybox device id field.
  *
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_read_attestation_keybox_ex(int *keybox_len, uint8_t* device_id, int* device_id_len);

/**
 * install attestation keybox
 *
 * @param[in] keybox_file: the encrypted attestation keybox file path.
 *
 * @retval 0 success
 * @retval -1 or other value, failed
 */
int rk_install_attestation_keybox(const char* keybox_file);

/**
 * install attestation keybox
 *
 * @param[in] keybox_buf: the encrypted attestation keybox buffer.
 * @param[in] keybox_len: the encrypted attestation keybox length.
 *
 * @retval 0 success
 * @retval -1 or other value, failed
 */
int rk_install_attestation_keybox_ex(const uint8_t* keybox_buf, size_t keybox_len);

/**
 * delete attestation keybox
 *
 * @retval 0 success
 * @retval -1 or other value, failed
 */
int rk_delete_attestation_keybox();

//==============================================================================
//ID ATTESTATION API
//==============================================================================
/**
  * set attestation ids
  *
  * @param[in] attestation_ids: attestation_ids to be set.
  *
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_set_id_attestation(AttestationIds* attestation_ids);

/**
  * check attestation ids
  * @param[out] ids_len: device ID attestation digest length.success:sizeof(AttestationIds)
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_check_id_attestation(int *ids_len);

/**
  * set attestation ids
  *
  * @param[in] attestation_ids: attestation_ids to verify as store.
  * @param[out] ids_len: device ID attestation digest length. success:sizeof(AttestationIds) , failed:other values
  * @param[out] consistency: the input device ID attestation digest and stored digest compare result.success:0 ,failed:ohter values
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_check_id_attestation_ex(AttestationIds* attestation_ids,int *ids_len,int *consistency);

//==============================================================================
//AVB LOCK API
//==============================================================================
/**
  * set avb lock state
  *
  * @param[in] lock_state: the name of lock state.
  * @param[in] lock_state_length: length of the lock state name.
  * @param[in] state: the state to set. 1:lock,0:unlock
  *
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_set_avb_lock(uint8_t* lock_state, size_t lock_state_length, int state);

/**
  * get avb lock state
  *
  * @param[in] lock_state: the name of lock state.
  * @param[in] lock_state_length: length of the lock state name.
  * @param[out] state: the state get. 1:lock,0:unlock
  *
  * @retval 0 success
  * @retval -1 or other value, failed
  */
int rk_get_avb_lock(uint8_t* lock_state, size_t lock_state_length, int* state);

#endif //KEYBOX_UTIL_H_