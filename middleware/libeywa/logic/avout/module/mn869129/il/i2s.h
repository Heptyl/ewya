/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#ifndef __I2S_H__
#define __I2S_H__

#include <stdint.h>
#include "system_common.h"

#pragma pack(1)
//------------------------------------------------------------------------------
typedef system_mute i2stx_mute;
//------------------------------------------------------------------------------
uint32_t i2srx_init(system_port port);
uint32_t i2srx_exit(system_port port);

uint32_t i2stx_init(system_port port);
uint32_t i2stx_exit(system_port port);
uint32_t i2stx_audio_mute(i2stx_mute *p_mute);
//------------------------------------------------------------------------------
#pragma pack()

#endif // __I2S_H__
