// file: ssd1306.c
// author: awry

#include "ssd1306.h"

#include "twi.h"

#include <stdlib.h>	// NULL
#include <avr/pgmspace.h>

#define SSD1306_TWI_ADDR (0x3C)

void _ssd1306_send(uint8_t cmd, uint8_t flags);

const uint8_t SSD1306_INIT_SEQ[] PROGMEM = {
	SSD1306_DISPLAY_OFF,                                         // 0xAE = Set Display OFF
	SSD1306_SET_MUX_RATIO, 63,                                   // 0xA8 - 64MUX for 128 x 64 version
	                                                             //      - 32MUX for 128 x 32 version
	SSD1306_MEMORY_ADDR_MODE, 0x00,                              // 0x20 = Set Memory Addressing Mode
	                                                             // 0x00 - Horizontal Addressing Mode
	                                                             // 0x01 - Vertical Addressing Mode
	                                                             // 0x02 - Page Addressing Mode (RESET)
	SSD1306_SET_COLUMN_ADDR, START_COLUMN_ADDR, END_COLUMN_ADDR, // 0x21 = Set Column Address, 0 - 127
	SSD1306_SET_PAGE_ADDR, START_PAGE_ADDR, END_PAGE_ADDR,       // 0x22 = Set Page Address, 0 - 7
	SSD1306_SET_START_LINE,                                      // 0x40
	SSD1306_DISPLAY_OFFSET, 0x00,                                // 0xD3
	SSD1306_SEG_REMAP_OP,                                        // 0xA0 / remap 0xA1
	SSD1306_COM_SCAN_DIR_OP,                                     // 0xC0 / remap 0xC8
	SSD1306_COM_PIN_CONF, 0x12,                                  // 0xDA, 0x12 - Disable COM Left/Right remap, Alternative COM pin configuration
	                                                             //       0x12 - for 128 x 64 version
	                                                             //       0x02 - for 128 x 32 version
	SSD1306_SET_CONTRAST, 0x7F,                                  // 0x81, 0x7F - reset value (max 0xFF)
	SSD1306_DIS_ENT_DISP_ON,                                     // 0xA4
	SSD1306_DIS_NORMAL,                                          // 0xA6
	SSD1306_SET_OSC_FREQ, 0x80,                                  // 0xD5, 0x80 => D=1; DCLK = Fosc / D <=> DCLK = Fosc
	SSD1306_SET_PRECHARGE, 0xc2,                                 // 0xD9, higher value less blinking
	                                                             // 0xC2, 1st phase = 2 DCLK,  2nd phase = 13 DCLK
	SSD1306_VCOM_DESELECT, 0x20,                                 // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
	SSD1306_SET_CHAR_REG, 0x14,                                  // 0x8D, Enable charge pump during display on
	SSD1306_DISPLAY_ON                                           // 0xAF = Set Display ON
};

#define SSD1306_INIT_SEQ_LEN (31)

void ssd1306_init(void)
{
	uint8_t i;
	uint8_t cmd;

	twi_init();

	twi_write_bytes(SSD1306_TWI_ADDR, NULL, 0, TWI_NOSTOP);

	for (i = 0; i < SSD1306_INIT_SEQ_LEN; i++) {
		cmd = pgm_read_byte(&(SSD1306_INIT_SEQ[i]));
		_ssd1306_send(cmd, TWI_NOSTART | TWI_NOSTOP);
	}

	twi_write_bytes(SSD1306_TWI_ADDR, NULL, 0, TWI_NOSTART);
}

void ssd1306_normal_display(void)
{
	_ssd1306_send(SSD1306_DIS_NORMAL, 0x00);
}

void ssd1306_inverse_display(void)
{
	_ssd1306_send(SSD1306_DIS_INVERSE, 0x00);
}

void ssd1306_draw_column(uint8_t data)
{
	uint8_t i;
	uint8_t cmd;

	cmd = SSD1306_DATA_STREAM;
	twi_write_bytes(SSD1306_TWI_ADDR, &cmd, 1, TWI_NOSTOP);

	for (i = 0; i < 8; i++) {
		twi_write_bytes(SSD1306_TWI_ADDR, &data, 1, TWI_NOSTART | TWI_NOSTOP);
	}

	twi_write_bytes(SSD1306_TWI_ADDR, NULL, 0, TWI_NOSTART);
}

void _ssd1306_send(uint8_t cmd, uint8_t flags)
{
	uint8_t reg[2] = { SSD1306_COMMAND, cmd };

	twi_write_bytes(SSD1306_TWI_ADDR, reg, 2, flags);
}
