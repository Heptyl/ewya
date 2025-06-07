/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#ifndef __REG_ACCESS_H__
#define __REG_ACCESS_H__

#include <stdint.h>
#include "system_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_REG_ACCESS_WRITE_SIZE			256
#define MAX_REG_ACCESS_READ_SIZE			256
#define MAX_I2C_CHANNEL						4

uint32_t lf9129_device_write(system_reg_access *p_access);
uint32_t lf9129_device_read(system_reg_access *p_access);
uint32_t device_read_wait(system_reg_access *p_access);

uint32_t device_block_write(system_block_access *p_access);
uint32_t device_block_read(system_block_access *p_access);

//uint32_t reg_open(void);
//uint32_t reg_close(void);
//uint32_t reg_info(uint32_t *chnum, uint32_t *ids, uint32_t *cur_idx);
//uint32_t reg_select(uint32_t idx);
uint32_t reg_write(uint8_t slaveaddr, uint8_t regaddr, uint32_t len, uint8_t *data);
uint32_t reg_read(uint8_t slaveaddr, uint8_t regaddr, uint32_t len, uint8_t *data);
uint32_t reg_read8(uint8_t slaveaddr, uint8_t regaddr, uint8_t *data);
uint32_t reg_write8(uint8_t slaveaddr, uint8_t regaddr, uint8_t data);
uint32_t reg_mask_read8(uint8_t slaveaddr, uint8_t regaddr, uint8_t *data, uint8_t mask);
uint32_t reg_mask_write8(uint8_t slaveaddr, uint8_t regaddr, uint8_t data, uint8_t mask);
uint32_t reg_read16(uint8_t slaveaddr, uint8_t regaddr, uint16_t *data);
uint32_t reg_write16(uint8_t slaveaddr, uint8_t regaddr, uint16_t data);
uint32_t reg_mask_read16(uint8_t slaveaddr, uint8_t regaddr, uint16_t *data, uint16_t mask);
uint32_t reg_mask_write16(uint8_t slaveaddr, uint8_t regaddr, uint16_t data, uint16_t mask);
uint32_t reg_read32(uint8_t slaveaddr, uint8_t regaddr, uint32_t *data);
uint32_t reg_write32(uint8_t slaveaddr, uint8_t regaddr, uint32_t data);
uint32_t reg_mask_read32(uint8_t slaveaddr, uint8_t regaddr, uint32_t *data, uint32_t mask);
uint32_t reg_mask_write32(uint8_t slaveaddr, uint8_t regaddr, uint32_t data, uint32_t mask);
char *dec2bin(uint8_t dec, char *bin);

void LF9129_Pl_Delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif // __REG_ACCESS_H__

