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

int twi_read_bytes(uint8_t address, uint8_t * data, uint8_t length,
		   uint8_t flags);
int twi_write_bytes(uint8_t address, uint8_t * data, uint8_t length,
		    uint8_t flags);

#endif
