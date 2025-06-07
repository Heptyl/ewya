/*
 * Copyright (c) 2015-2017 Socionext Inc. All rights reserved.
 */
#include <stdlib.h>
//#include <string.h>

#include "reg_access.h"
#include "hv_error.h"
#include "hv_sleep.h"

#define SWAP_LITTLE_ENDIAN

#define TIMEOUT_COUNT    300
// -----------------------------------------------------------------------------
char * dec2bin(uint8_t dec, char *bin)
{
    int32_t i, m;

    for(i=0, m=0x80; m>0; i++, m >>= 1){
        bin[i] = (m & dec) ? '1' : '0';
    }
    bin[8]='\0';
    return bin;
}
// -----------------------------------------------------------------------------
uint32_t lf9129_device_write(system_reg_access *p_access)
{
    uint32_t i;
    uint32_t ret;
    uint32_t data, mask;
    uint8_t saddr, oaddr;

    ret = ERR_NG;
    for(i=0; i<p_access->count; i++){
        saddr = (uint8_t)((p_access->p_com[i].addr >> 8) & 0x000000FF); //0x10 0x0e 0x00 0xff
        saddr += (uint8_t)(p_access->p_port->chip_num * 0x00000030);
        oaddr = (uint8_t)((p_access->p_com[i].addr >> 0) & 0x000000FF);
        data = p_access->p_com[i].data;
        mask = p_access->p_com[i].mask;
        if(mask > 0x00000000 && mask < 0x000000FF){            // 0x00000000 - 0x000000FE
            ret = reg_mask_write8(saddr, oaddr, (uint8_t)(data & 0x000000FF), (uint8_t)mask);
        }else if(mask == 0x000000FF){                        // 0x000000FF
            ret = reg_write8(saddr, oaddr, (uint8_t)(data & 0x000000FF));
        }else if(mask > 0x000000FF && mask < 0xFFFFFFFF){    // 0x00000100 - 0xFFFFFFFE
            ret = reg_mask_write32(saddr, oaddr, data, mask);
        }else if(mask == 0xFFFFFFFF){                        // 0xFFFFFFFF
            ret = reg_write32(saddr, oaddr, data);
        }
        if(ret){
            break;
        }
    }
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t lf9129_device_read(system_reg_access *p_access)
{
    uint32_t i;
    uint32_t ret;
    uint32_t data, mask;
    uint8_t saddr, oaddr;

    data = 0;
    ret = ERR_NG;
    for(i=0; i<p_access->count; i++){
        saddr = (uint8_t)((p_access->p_com[i].addr >> 8) & 0x000000FF);
        saddr += (uint8_t)(p_access->p_port->chip_num * 0x00000030);
        oaddr = (uint8_t)((p_access->p_com[i].addr >> 0) & 0x000000FF);
        mask = p_access->p_com[i].mask;
        if(mask <= 0x000000FF){                                // 0x00000000 - 0x000000FF
            ret = reg_mask_read8(saddr ,oaddr, (uint8_t *)&data, mask);
        }else{                                                // 0x00000100 - 0xFFFFFFFF
            ret = reg_mask_read32(saddr, oaddr, &data, mask);
        }
        if(ret){
            break;
        }
        p_access->p_com[i].data = data;
    }
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t device_read_wait(system_reg_access *p_access)
{
    uint32_t ret;
    int32_t timeout_count;
    uint32_t data, mask, expd;
    uint8_t saddr, oaddr;

    saddr = (uint8_t)((p_access->p_com[0].addr >> 8) & 0x000000FF);
    saddr += (uint8_t)(p_access->p_port->chip_num * 0x00000030);
    oaddr = (uint8_t)((p_access->p_com[0].addr >> 0) & 0x000000FF);
    mask = p_access->p_com[0].mask;
    expd = p_access->p_com[0].data & mask;

    ret = ERR_NG;
    data = !expd;
    timeout_count = 0;
    while(1){
        if(mask <= 0x000000FF){                                // 0x00000000 - 0x000000FF
            ret = reg_mask_read8(saddr, oaddr, (uint8_t *)&data, mask);
        }else{                                                // 0x00000100 - 0xFFFFFFFF
            ret = reg_mask_read32(saddr, oaddr, &data, mask);
        }
        if(ret){
            break;
        }
        if(data == expd){
            ret = ERR_OK;
            break;
        }
        if(timeout_count >= TIMEOUT_COUNT){
            ret = ERR_TIMEOUT;
            break;
        }
        timeout_count++;
        SLEEP_Delay_ms(10);
    }
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t device_block_write(system_block_access *p_access)
{
    uint32_t ret;
    uint8_t saddr, oaddr;

    saddr = (uint8_t)((p_access->addr >> 8) & 0x000000FF);
    saddr += (uint8_t)(p_access->p_port->chip_num * 0x00000030);
    oaddr = (uint8_t)((p_access->addr >> 0) & 0x000000FF);
    // MES(" MYH %s, %d, write slaveaddr: 0x%x, regaddr: 0x%x, data: 0x%x", __func__, __LINE__, saddr, oaddr, p_access->p_data);
    ret = reg_write(saddr, oaddr, p_access->length, p_access->p_data);

    return ret;
}
// -----------------------------------------------------------------------------
uint32_t device_block_read(system_block_access *p_access)
{
    uint32_t ret;
    uint8_t saddr, oaddr;

    saddr = (uint8_t)((p_access->addr >> 8) & 0x000000FF);
    saddr += (uint8_t)(p_access->p_port->chip_num * 0x00000030);
    oaddr = (uint8_t)((p_access->addr >> 0) & 0x000000FF);
    // MES(" MYH %s, %d, readslaveaddr: 0x%x, regaddr: 0x%x", __func__, __LINE__, saddr, oaddr);
    ret = reg_read(saddr, oaddr, p_access->length, p_access->p_data);

    return ret;
}
// -----------------------------------------------------------------------------
uint32_t reg_read8(uint8_t slaveaddr, uint8_t regaddr, uint8_t *data)
{
    uint32_t ret;
    uint8_t bin=0;

    // MES(" MYH %s, %d, readslaveaddr: 0x%x, regaddr: 0x%x", __func__, __LINE__, slaveaddr, regaddr);
    ret = reg_read(slaveaddr, regaddr, 1, &bin);
    if(ret){
        return ret;
    }
    *data = bin;
    return ERR_OK;
}
// -----------------------------------------------------------------------------
uint32_t reg_write8(uint8_t slaveaddr, uint8_t regaddr, uint8_t data)
{
    uint32_t ret;

    // MES(" MYH %s, %d, write slaveaddr: 0x%x, regaddr: 0x%x, data: 0x%x", __func__, __LINE__, slaveaddr, regaddr, data);
    ret = reg_write(slaveaddr, regaddr, 1, &data);
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t reg_mask_read8(uint8_t slaveaddr, uint8_t regaddr, uint8_t *data, uint8_t mask)
{
    uint32_t ret;
    uint8_t bin=0;

    ret = reg_read8(slaveaddr, regaddr, &bin);
    if(ret){
        return ret;
    }
    *data = bin & mask;
    return ERR_OK;
}
// -----------------------------------------------------------------------------
uint32_t reg_mask_write8(uint8_t slaveaddr, uint8_t regaddr, uint8_t data, uint8_t mask)
{
    uint32_t ret;
    uint8_t bin=0, md1, md2;

    ret = reg_read8(slaveaddr, regaddr, &bin);
    if(ret){
        return ret;
    }
    md1 = bin & ~mask;
    md2 = data & mask;
    bin = md1 | md2;
    ret = reg_write8(slaveaddr, regaddr, bin);
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t reg_read16(uint8_t slaveaddr, uint8_t regaddr, uint16_t *data)
{
    uint32_t ret;
    uint8_t bin[2]={0};

    // MES(" MYH %s, %d, readslaveaddr: 0x%x, regaddr: 0x%x", __func__, __LINE__, slaveaddr, regaddr);
    ret = reg_read(slaveaddr, regaddr, 2, bin);
    if(ret){
        return ret;
    }
#ifdef SWAP_LITTLE_ENDIAN
    *data = (((uint16_t)bin[0] <<  8) & 0xFF00) |
            (((uint16_t)bin[1] <<  0) & 0x00FF);
#else
    *data = (((uint16_t)bin[0] <<  0) & 0x00FF) |
            (((uint16_t)bin[1] <<  8) & 0xFF00);
#endif
    return ERR_OK;
}
// -----------------------------------------------------------------------------
uint32_t reg_write16(uint8_t slaveaddr, uint8_t regaddr, uint16_t data)
{
    uint32_t ret;
    uint8_t bin[2]={0};

#ifdef SWAP_LITTLE_ENDIAN
    bin[0] = (uint8_t)((data >>  8) & 0x00FF);
    bin[1] = (uint8_t)((data >>  0) & 0x00FF);
#else
    bin[0] = (uint8_t)((data >>  0) & 0x00FF);
    bin[1] = (uint8_t)((data >>  8) & 0x00FF);
#endif

    // MES(" MYH %s, %d, write slaveaddr: 0x%x, regaddr: 0x%x, data: 0x%x", __func__, __LINE__, slaveaddr, regaddr, data);
    ret = reg_write(slaveaddr, regaddr, 2, bin);
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t reg_mask_read16(uint8_t slaveaddr, uint8_t regaddr, uint16_t *data, uint16_t mask)
{
    uint32_t ret;
    uint16_t bin=0;

    ret = reg_read16(slaveaddr, regaddr, &bin);
    if(ret){
        return ret;
    }
    *data = bin & mask;
    return ERR_OK;
}
// -----------------------------------------------------------------------------
uint32_t reg_mask_write16(uint8_t slaveaddr, uint8_t regaddr, uint16_t data, uint16_t mask)
{
    uint32_t ret;
    uint16_t bin=0, md1, md2;

    ret = reg_read16(slaveaddr, regaddr, &bin);
    if(ret){
        return ret;
    }
    md1 = bin & ~mask;
    md2 = data & mask;
    bin = md1 | md2;
    ret = reg_write16(slaveaddr, regaddr, bin);
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t reg_read32(uint8_t slaveaddr, uint8_t regaddr, uint32_t *data)
{
    uint32_t ret;
    uint8_t bin[4]={0};

    // MES(" MYH %s, %d, readslaveaddr: 0x%x, regaddr: 0x%x", __func__, __LINE__, slaveaddr, regaddr);
    ret = reg_read(slaveaddr, regaddr, 4, bin);
    if(ret){
        return ret;
    }
#ifdef SWAP_LITTLE_ENDIAN
    *data = (((uint32_t)bin[0] << 24) & 0xFF000000) |
            (((uint32_t)bin[1] << 16) & 0x00FF0000) |
            (((uint32_t)bin[2] <<  8) & 0x0000FF00) |
            (((uint32_t)bin[3] <<  0) & 0x000000FF);
#else
    *data = (((uint32_t)bin[0] <<  0) & 0x000000FF) |
            (((uint32_t)bin[1] <<  8) & 0x0000FF00) |
            (((uint32_t)bin[2] << 16) & 0x00FF0000) |
            (((uint32_t)bin[3] << 24) & 0xFF000000);
#endif
    return ERR_OK;
}
// -----------------------------------------------------------------------------
uint32_t reg_write32(uint8_t slaveaddr, uint8_t regaddr, uint32_t data)
{
    uint32_t ret;
    uint8_t bin[4]={0};

#ifdef SWAP_LITTLE_ENDIAN
    bin[0] = (uint8_t)((data >> 24) & 0x000000FF);
    bin[1] = (uint8_t)((data >> 16) & 0x000000FF);
    bin[2] = (uint8_t)((data >>  8) & 0x000000FF);
    bin[3] = (uint8_t)((data >>  0) & 0x000000FF);
#else
    bin[0] = (uint8_t)((data >>  0) & 0x000000FF);
    bin[1] = (uint8_t)((data >>  8) & 0x000000FF);
    bin[2] = (uint8_t)((data >> 16) & 0x000000FF);
    bin[3] = (uint8_t)((data >> 24) & 0x000000FF);
#endif

    // MES(" MYH %s, %d, write slaveaddr: 0x%x, regaddr: 0x%x, data: 0x%x", __func__, __LINE__, slaveaddr, regaddr, data);
    ret = reg_write(slaveaddr, regaddr, 4, bin);
    return ret;
}
// -----------------------------------------------------------------------------
uint32_t reg_mask_read32(uint8_t slaveaddr, uint8_t regaddr, uint32_t *data, uint32_t mask)
{
    uint32_t ret;
    uint32_t bin=0;

    ret = reg_read32(slaveaddr, regaddr, &bin);
    if(ret){
        return ret;
    }
    *data = bin & mask;
    return ERR_OK;
}
// -----------------------------------------------------------------------------
uint32_t reg_mask_write32(uint8_t slaveaddr, uint8_t regaddr, uint32_t data, uint32_t mask)
{
    uint32_t ret;
    uint32_t bin=0, md1, md2;

    ret = reg_read32(slaveaddr, regaddr, &bin);
    if(ret){
        return ret;
    }
    md1 = bin & ~mask;
    md2 = data & mask;
    bin = md1 | md2;
    ret = reg_write32(slaveaddr, regaddr, bin);
    return ret;
}


void LF9129_Pl_Delay_ms(uint32_t ms)
{
     SLEEP_Delay_ms(ms);
}
// -----------------------------------------------------------------------------
