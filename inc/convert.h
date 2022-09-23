// file: convert.h
// author: awry

#ifndef CONVERT_H_
#define CONVERT_H_

#include <stdint.h>

#define str_to_bcd(x) str_to_bin(x)

uint8_t asc_to_bin(char asc);
uint8_t bcd_to_bin(uint8_t bcd);
uint8_t bin_to_bcd(uint8_t bin);

__attribute__((always_inline))
inline uint8_t str_to_bin(const char *str);

inline uint8_t str_to_bin(const char *str)
{
	uint8_t bin;

	bin = asc_to_bin(str[0]) << 4;
	bin += asc_to_bin(str[1]);

	return bin;
}

#endif				// CONVERT_H_
