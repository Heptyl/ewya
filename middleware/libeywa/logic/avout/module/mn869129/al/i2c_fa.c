/*
 * Copyright (C) 2015-2020 Socionext Inc.
 */
#include <stdlib.h>
//#include <linux/string.h>

#include "i2c_fa.h"
#include "reg_access.h"
#include "hv_print.h"
#include "hv_sleep.h"

#define TIMEOUT_COUNT	300
// -----------------------------------------------------------------------------
uint32_t i2cr(cmds_i2c *cmd)
{
	//int32_t reg_read(uint8_t slaveaddr, uint8_t regaddr, uint32_t len, uint8_t *data)
	//			//i2cr:  <slv> <off> [size]
	//			//       read data from <slv>:<off> address up to [size].

	uint8_t slv, off;
	uint16_t len;
	uint8_t data[MAX_REG_ACCESS_READ_SIZE];
	int32_t ret;
	uint16_t sta, end;
	char bin[8+1];
	uint16_t i;

	slv = cmd->opts.saddr;
	off = cmd->opts.oaddr;
	len = cmd->opts.size;
	if (len == 0) {
		len = 1;
	}
	//MES("i2cr %02x %02x %02x", slv, off, len);
	if(len > MAX_REG_ACCESS_READ_SIZE){
		//WAR("The specified size exceeded the maximum (%d).", MAX_REG_ACCESS_READ_SIZE);
		//WAR("Only %d bytes are read.", MAX_REG_ACCESS_READ_SIZE);
		len = MAX_REG_ACCESS_READ_SIZE;
	}
	ret = reg_read(slv, off, len, data);
	if(ret){
		return ret;
	}
	// dump read data
	if(len == 1){
		//MES("[%02X:%02X] 0x%02X : BIN:%s", slv, off, data[0], dec2bin(data[0], bin));
	}else{
		sta = off & 0xf0;
		end = ((off + len - 1) & 0xf0) + 0x0f;
		end += (off + len - 1) > 0xff ? 0xf0 : 0x00;

		//MES("        +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +A +B +C +D +E +F");
		for(i=sta; i<=end; i++){
			if((i % 0x10) == 0x00){
				//MESn("[%02X:%02X] ", slv, i);
			}
			if(i >= off && i < (off+len)){
				//MESn("%02X ", data[(i-off)]);
			}else{
				//MESn("-- ");
			}
			if((i % 0x10) == 0x07){
				//MESn(" ");
			}
			if((i % 0x10) == 0x0f){
				//MESn("\n");
			}
		}
	}

	return 0;
}
// -----------------------------------------------------------------------------
uint32_t i2cw(cmds_i2c *cmd)
{
	uint8_t slv, off;
	uint32_t len;
	uint8_t data[MAX_REG_ACCESS_WRITE_SIZE];
	int32_t ret;
	uint32_t i;

	slv = cmd->opts.saddr;
	off = cmd->opts.oaddr;
	len = cmd->opts.size;
	//MESn("i2cw %02x %02x %02x", slv, off, len);
	for(i=0; i<len; i++){
		data[i] = cmd->opts.data[i];
		//MESn(" %02x", data[i]);
	}
	//MES("");
	ret = reg_write(slv, off, len, data);

	return ret;
}
// -----------------------------------------------------------------------------
uint32_t i2cp(cmds_i2c *cmd)
{
	uint8_t slv1, off1, slv2, off2, p_slv, p_off, p_rd, p_wr;
	uint8_t data[1];
	int32_t ret;

	slv1  = cmd->opts.saddr;
	off1  = cmd->opts.oaddr;
	slv2  = cmd->opts.saddr2;
	off2  = cmd->opts.oaddr2;
	p_slv = cmd->opts.p_saddr;
	p_off = cmd->opts.p_oaddr;
	p_rd  = cmd->opts.p_rd;
	p_wr  = cmd->opts.p_wr;
	//MES("i2cp %02x %02x %02x %02x %02x %02x %02x %02x",
	//		slv1, off1, slv2, off2, p_slv, p_off, p_rd, p_wr);

	ret  = reg_write(p_slv, p_off, 1, &p_rd);
	ret |= reg_read(slv1, off1, 1, data);
	ret |= reg_write(p_slv, p_off, 1, &p_wr);
	ret |= reg_write(slv2, off2, 1, data);

	return ret;
}
// -----------------------------------------------------------------------------
uint32_t i2c_sleep(cmds_i2c *cmd)
{
	uint32_t ms;

	ms = cmd->opts.msec;
	//MES("sleep %u", (unsigned int)ms);
	SLEEP_Delay_ms(ms);
	return 0;
}
// -----------------------------------------------------------------------------
uint32_t i2c_wait(cmds_i2c *cmd)
{
	uint8_t slv, off, msk, dat, read_dat;
	int32_t ret;
	int32_t timeout_count;

	slv = cmd->opts.saddr;
	off = cmd->opts.oaddr;
	msk = cmd->opts.mask;
	dat = cmd->opts.data[0];
	//MES("wait %02x %02x %02x %02x", slv, off, msk, dat);
	ret = 0;
	read_dat = !dat;
	timeout_count = 0;
	while(1){
		ret = reg_read(slv, off, 1, &read_dat);
		if(ret){
			break;
		}
		//MES("expected:%02X, read:%02X", (dat & msk), (read_dat & msk));
		if((dat & msk) == (read_dat & msk)){
			ret = 0;
			break;
		}
		if(timeout_count >= TIMEOUT_COUNT){
			ret = -1;
			break;
		}
		timeout_count++;
		SLEEP_Delay_ms(10);
	}
	return ret;
}
// -----------------------------------------------------------------------------
