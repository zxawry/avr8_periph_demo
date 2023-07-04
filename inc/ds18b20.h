// file: ds18b20.h
// author: awry

#ifndef DS18B20_H
#define DS18B20_H

#include <stdint.h>

int ds18b20_init(void);
int ds18b20_is_busy(void);
int ds18b20_start_conversion(void);
int ds18b20_get_temperature(char * str);

#endif
