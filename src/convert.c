// file: convert.c
// author: awry

#include "convert.h"

#define _subtract_(x, y)				\
	__asm__ __volatile__(				\
		"subi	%[r], %[k]"		"\n\t"	\
		: [r] "=d" (x)				\
		: "0" (x), [k] "M" (y)			\
	);

uint8_t asc_to_bin(char asc)
{
	_subtract_(asc, 0x61);
	if (asc < 6)
		return asc + 0xa;

	_subtract_(asc, 0xcf);
	if (asc < 10)
		return asc;

	return 0;
}

uint8_t str_to_bin(const char *str)
{
	uint8_t bin = 0;

	bin = asc_to_bin(str[0]) << 4;
	bin += asc_to_bin(str[1]);

	return bin;
}

uint8_t bcd_to_bin(uint8_t bcd)
{
	uint8_t bin = bcd & 0x0f;

	while (bcd >= 0x10) {
		bin += 10;
		bcd -= 0x10;
	}

	return bin;
}

uint8_t bin_to_bcd(uint8_t bin)
{
	uint8_t bcd = 0;

	while (bin >= 10) {
		bcd += 0x10;
		bin -= 10;
	}

	return bcd + bin;
}
