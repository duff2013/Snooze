/*
 *  bitband.h
 *  Teensy 3.x/LC
 *
 */

#ifndef __BITBAND_H__
#define __BITBAND_H__

#define BITBAND_ADDR(reg, bit) (((uint32_t)&(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)
#define BITBAND_U32(reg, bit) (*(volatile uint32_t *)BITBAND_ADDR((reg), (bit)))
#define BITBAND_U8(reg, bit) (*(volatile uint8_t *)BITBAND_ADDR((reg), (bit)))

#endif
