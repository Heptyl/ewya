/*
 * Copyright 2018, Rockchip Electronics Co., Ltd
 * callen.cai, <callen.cai@rock-chips.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef ID_ATTESTATION_H_
#define ID_ATTESTATION_H_

#include <android/log.h>
#include <string.h>
#include <stdbool.h>
#include <err.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#define DEBUG   0
#define LOG_TAG    "id_attestation"

#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#define PROP_LEN   32
#define PROP_LEN_EXT   64

typedef struct AttestationIds_ {
    uint8_t brand[PROP_LEN];
    uint8_t device[PROP_LEN];
    uint8_t product[PROP_LEN];
    uint8_t serial[PROP_LEN_EXT];
    uint8_t imei[PROP_LEN];
    uint8_t second_imei[PROP_LEN];
    uint8_t meid[PROP_LEN];
    uint8_t manufacturer[PROP_LEN_EXT];
    uint8_t model[PROP_LEN_EXT];
} AttestationIds;

void printAttestationIds(const AttestationIds* ids);

#endif