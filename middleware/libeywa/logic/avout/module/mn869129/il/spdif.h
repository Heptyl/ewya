/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __SPDIF_H__
#define __SPDIF_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef system_audio spdifrx_audio;
typedef system_mute spdiftx_mute;
//------------------------------------------------------------------------------
uint32_t spdifrx_init(system_port port);
uint32_t spdifrx_exit(system_port port);
uint32_t spdifrx_get_audio_format(spdifrx_audio *p_audio);

uint32_t spdiftx_init(system_port port);
uint32_t spdiftx_exit(system_port port);
uint32_t spdiftx_audio_mute(spdiftx_mute *p_mute);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __SPDIF_H__
