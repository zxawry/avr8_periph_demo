// file: ds18b20.h
// author: awry

#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>

int ds18b20_init(void);
int ds18b20_write(uint8_t data);
int ds18b20_read(uint8_t * data);
int ds18b20_temperature(int16_t * temperature);

#endif
