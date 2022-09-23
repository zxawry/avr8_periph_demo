// file: convert.c
// author: awry

#include "convert.h"

uint8_t asc_to_bin(char asc)
{
	if ((asc >= 'a') && (asc <= 'f'))
		return asc - 'a' + 0x0a;

	if ((asc >= '0') && (asc <= '9'))
		return asc - '0';

	return 0;
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