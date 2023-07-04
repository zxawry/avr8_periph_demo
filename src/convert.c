// file: convert.c
// author: awry

#include "convert.h"

#include <avr/pgmspace.h>

const uint8_t FRACTION_LOOKUP_TABLE[][2] PROGMEM = {
	"00",			// 0000
	"00",			// 0001
	"00",			// 0010
	"00",			// 0011
	"25",			// 0100
	"25",			// 0101
	"25",			// 0110
	"25",			// 0111
	"50",			// 1000
	"50",			// 1001
	"50",			// 1010
	"50",			// 1011
	"75",			// 1100
	"75",			// 1101
	"75",			// 1110
	"75",			// 1111
};

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

uint8_t str_to_bin(const char *str)
{
	uint8_t bin;

	bin = asc_to_bin(str[0]) << 4;
	bin += asc_to_bin(str[1]);

	return bin;
}

void bcd_to_str(uint8_t bcd, char *str)
{
	str[0] = (bcd >> 4) + '0';
	str[1] = (bcd & 15) + '0';
}

void tmp_to_str(int16_t tmp, char *str)
{
	uint8_t i;
	uint8_t d;
	uint8_t f;

	if (tmp & 0xf800) {
		tmp = ~tmp + 1;
		*str++ = '-';
	} else {
		*str++ = '+';
	}

	d = (uint8_t) ((tmp & 0x0ff0) >> 4);
	f = (uint8_t) (tmp & 0x000f);

	i = 0;
	while (d >= 100) {
		d -= 100;
		i++;
	}
	if (i > 0)
		*str++ = i + '0';

	i = 0;
	while (d >= 10) {
		d -= 10;
		i++;
	}
	if (!((*(str - 1) == '-' || *(str - 1) == '+') && i == 0))
		*str++ = i + '0';

	*str++ = d + '0';

	*str++ = '.';

	*str++ = pgm_read_byte(&(FRACTION_LOOKUP_TABLE[f][0]));
	*str++ = pgm_read_byte(&(FRACTION_LOOKUP_TABLE[f][1]));

	*str++ = 0;		// NULL
}
