// file: ds1307.c
// author: awry

#include "ds1307.h"

#include "convert.h"
#include "twi.h"

#define DS1307_TWI_ADDR (0x68)

static void _str_to_reg(const char *str, uint8_t * reg);
static void _reg_to_str(const uint8_t * reg, char *str);

static void _str_to_reg(const char *str, uint8_t * reg)
{
	reg[6] = str_to_bcd(&str[2]);	// year
	reg[5] = str_to_bcd(&str[5]);	// mon
	reg[4] = str_to_bcd(&str[8]);	// mday
	reg[3] = 1;		// wday
	reg[2] = str_to_bcd(&str[11]);	// hour
	reg[1] = str_to_bcd(&str[14]);	// min
	reg[0] = str_to_bcd(&str[17]);	// sec
}

static void _reg_to_str(const uint8_t * reg, char *str)
{
	bcd_to_str(reg[6], &str[2]);
	bcd_to_str(reg[5], &str[5]);
	bcd_to_str(reg[4], &str[8]);
	bcd_to_str(reg[2], &str[11]);
	bcd_to_str(reg[1], &str[14]);
	bcd_to_str(reg[0], &str[17]);

	str[0] = '2';
	str[1] = '0';
	str[4] = '-';
	str[7] = '-';
	str[10] = ' ';
	str[13] = ':';
	str[16] = ':';
	str[19] = '\0';
}

void ds1307_init(void)
{
	twi_init();

	// pinging the device here is suggested
}

void ds1307_set_time(const char *str)
{
	uint8_t reg[7];

	_str_to_reg(str, reg);

	twi_write_bytes(DS1307_TWI_ADDR, (uint8_t *) "\0", 1, TWI_NOSTOP);
	twi_write_bytes(DS1307_TWI_ADDR, reg, 7, TWI_NOSTART);
}

void ds1307_get_time(char *str)
{
	uint8_t reg[7];

	twi_write_bytes(DS1307_TWI_ADDR, (uint8_t *) "\0", 1, TWI_NOSTOP);
	twi_read_bytes(DS1307_TWI_ADDR, reg, 7, 0x00);

	_reg_to_str(reg, str);
}
