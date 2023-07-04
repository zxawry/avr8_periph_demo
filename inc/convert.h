// file: convert.h
// author: awry

#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdint.h>

#define str_to_bcd(x) str_to_bin(x)

uint8_t asc_to_bin(char asc);
uint8_t bcd_to_bin(uint8_t bcd);
uint8_t bin_to_bcd(uint8_t bin);
uint8_t str_to_bin(const char *str);
void bcd_to_str(uint8_t bcd, char *str);
void tmp_to_str(int16_t tmp, char *str);

#endif				// CONVERT_H_
