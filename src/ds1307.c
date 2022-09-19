// file: ds1307.c
// author: awry

#include "ds1307.h"

#include "twi.h"

// debug
//#include "serio.h"

#define DS1307_TWI_ADDR (0x68)

static uint8_t byte_to_bcd(int8_t x)
{
	uint8_t y = 0;

	while (x >= 10) {
		y += 0x10;
		x -= 10;
	}

	return y + x;

	//return ((x) + (6 * ((x) / 10)));
}

static int8_t bcd_to_byte(uint8_t x)
{
	int8_t y = x & 0x0f;

	while (x >= 0x10) {
		y += 10;
		x -= 0x10;
	}

	return y;

	//return ((x) - (6 * ((x) >> 4)));
}

void ds1307_init(void)
{
	twi_init();

	//twi_write_bytes(DS1307_TWI_ADDR, (uint8_t *) "\0", 1, 0x00); 
}

void ds1307_set_time(const struct tm *time)
{
	uint8_t tmp[7];

	// tmp[0] <= 7.ch 6-4.seconds-10 3-0.seconds
	tmp[0] = byte_to_bcd(time->tm_sec);

	// tmp[1] <= 6-4.minutes-10 3-0.minutes
	tmp[1] = byte_to_bcd(time->tm_min);

	// tmp[2] <= 6.mode-12/24 5-4.hours-10 3-0.hours
	tmp[2] = byte_to_bcd(time->tm_hour);

	// tmp[3] <= 2-0.day
	tmp[3] = byte_to_bcd(time->tm_wday + 1);

	// tmp[4] <= 5-4.date-10 3-0.date
	tmp[4] = byte_to_bcd(time->tm_mday);

	// tmp[5] <= 4.month-10 3-0.month
	tmp[5] = byte_to_bcd(time->tm_mon + 1);

	// tmp[6] <= 7-4.year-10 3-0.year
	tmp[6] = byte_to_bcd((int8_t) (time->tm_year - 100));

	twi_write_bytes(DS1307_TWI_ADDR, (uint8_t *) "\0", 1, TWI_NOSTOP);
	twi_write_bytes(DS1307_TWI_ADDR, tmp, 7, TWI_NOSTART);

	//xputs("wrote to ds1307:\n");
	//put_dump(tmp, 7);
}

void ds1307_get_time(struct tm *time)
{
	uint8_t tmp[7];

	twi_write_bytes(DS1307_TWI_ADDR, (uint8_t *) "\0", 1, TWI_NOSTOP);
	twi_read_bytes(DS1307_TWI_ADDR, tmp, 7, 0x00);

	time->tm_sec = bcd_to_byte(tmp[0]);
	time->tm_min = bcd_to_byte(tmp[1]);
	time->tm_hour = bcd_to_byte(tmp[2]);
	time->tm_wday = bcd_to_byte(tmp[3]) - 1;
	time->tm_mday = bcd_to_byte(tmp[4]);
	time->tm_mon = bcd_to_byte(tmp[5]) - 1;
	time->tm_year = (int16_t) bcd_to_byte(tmp[6]) + 100;

	//xputs("read from ds1307:\n");
	//put_dump(tmp, 7);
}
