#ifndef TWI_H
#define TWI_H

#include <stdint.h>

#ifndef TWI_FREQ
#define TWI_FREQ 100000UL
#endif

#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 128
#endif

void twi_init(void);
void twi_write(uint8_t address, uint8_t * data, uint8_t length, uint8_t resume);
void twi_read(uint8_t address, uint8_t * data, uint8_t length, uint8_t resume);
uint8_t twi_wait(void);

#endif
