#ifndef TWI_H
#define TWI_H

#include <stdint.h>

#include "queue.h"

#ifndef TWI_FREQ
#define TWI_FREQ 100000UL
#endif

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 128
#endif

#define TWI_NOSTOP 0x01
#define TWI_NOSTART 0x02

void twi_init(void);

int twi_transfer(uint8_t addr, uint8_t * data, uint8_t len, uint8_t flags);

#define twi_write_bytes(addr, data, len, flags) \
	twi_transfer((addr << 1) | 0, data, len, flags)

#define twi_read_bytes(addr, data, len, flags) \
	twi_transfer((addr << 1) | 1, data, len, flags)

#endif