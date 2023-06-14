// file: ssd1306.c
// author: awry

#include "ssd1306.h"
#include "ssd1306_internal.h"

#include "twi.h"

#include <stdlib.h>	// NULL

static void _command(uint8_t command, uint8_t flags);

void ssd1306_init(void)
{
	uint8_t i;
	uint8_t command;

	twi_init();

	twi_write_bytes(SSD1306_TWI_ADDR, NULL, 0, TWI_NOSTOP);

	for (i = 0; i < SSD1306_INIT_SEQ_LEN; i++) {
		command = pgm_read_byte(&(SSD1306_INIT_SEQ[i]));
		_command(command, TWI_NOSTART | TWI_NOSTOP);
	}

	twi_write_bytes(SSD1306_TWI_ADDR, NULL, 0, TWI_NOSTART);
}

void ssd1306_normal_display(void)
{
	_command(SSD1306_DIS_NORMAL, 0x00);
}

void ssd1306_inverse_display(void)
{
	_command(SSD1306_DIS_INVERSE, 0x00);
}

void ssd1306_draw_block(uint8_t * data)
{
	uint8_t command;

	command = SSD1306_DATA_STREAM;
	twi_write_bytes(SSD1306_TWI_ADDR, &command, 1, TWI_NOSTOP);
	twi_write_bytes(SSD1306_TWI_ADDR, data, 8, TWI_NOSTART);
}

void ssd1306_draw_ascii(char ascii, uint8_t mask)
{
	uint8_t i;
	uint8_t data[8];

	if (ascii - 32 < 0 || ascii - 32 > 95) {
		return ;
	}

	for (i = 0; i < 8; i ++) {
		data[i] = pgm_read_byte(&(SSD1306_ASCII_FONT[ascii - 32][i])) ^ mask;
	}

	ssd1306_draw_block(data);
}

void ssd1306_draw_string(char * str, uint8_t mask)
{
	while (*str)
		ssd1306_draw_ascii(*str++, mask);
}

void ssd1306_set_page(uint8_t start, uint8_t end)
{
	if (start > 7 || end > 7) {
		return ;
	}

	_command(SSD1306_SET_PAGE_ADDR, TWI_NOSTOP);
	_command(start, TWI_NOSTART | TWI_NOSTOP);
	_command(end, TWI_NOSTART);
}

void ssd1306_set_column(uint8_t start, uint8_t end)
{
	if (start > 127 || end > 127) {
		return ;
	}

	_command(SSD1306_SET_COLUMN_ADDR, TWI_NOSTOP);
	_command(start, TWI_NOSTART | TWI_NOSTOP);
	_command(end, TWI_NOSTART);
}

static void _command(uint8_t command, uint8_t flags)
{
	uint8_t packet[2] = { SSD1306_COMMAND, command };

	twi_write_bytes(SSD1306_TWI_ADDR, packet, 2, flags);
}
