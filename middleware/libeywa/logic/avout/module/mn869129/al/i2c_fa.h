/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#ifndef __I2C_FA_H__
#define __I2C_FA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "opts.h"

uint32_t i2cr(cmds_i2c *cmd);
uint32_t i2cw(cmds_i2c *cmd);
uint32_t i2cp(cmds_i2c *cmd);
uint32_t i2c_sleep(cmds_i2c *cmd);
uint32_t i2c_wait(cmds_i2c *cmd);

#ifdef __cplusplus
}
#endif

#endif // __I2C_FA_FUNC_H__
