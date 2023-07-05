// file: main.c
// author: awry

#include "config.h"

#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "serio.h"
#include "convert.h"

#include "lib/pff.h"
#include "lib/diskio.h"

static inline void periph_init(void);

static inline void periph_init(void)
{
	cli();
	usart_init();
	sei();
}

int main(void)
{
	BYTE res;
	UINT s;
	FATFS fs;

	char buffer[128];

	periph_init();

	xputs_P(PSTR("\n\nWelcome to AVR shell!\n\n"));
	xputs_P(PSTR("avr$ "));

	for (;;) {
		if (xgets_I(buffer, 128)) {
			switch (buffer[0]) {
			case 'i':
				res = disk_initialize();
				put_dump(&res, 1, 0x00);
				break;
			case 'm':
				res = pf_mount(&fs);
				put_dump(&res, 1, 0x00);
				break;
			case 'o':
				res = pf_open(buffer + 2);
				put_dump(&res, 1, 0x00);
				break;
			case 'r':
				res = pf_read(0, 128, &s);
				put_dump(&res, 1, 0x00);
				break;
			case 'w':
				res = pf_write(buffer + 2, strlen(buffer + 2), &s);
				put_dump(&res, 1, 0x00);
				break;
			case 'e':
				res = pf_write(0, 0, &s);
				put_dump(&res, 1, 0x00);
				break;
			case 's':
				res = pf_lseek(str_to_bin(buffer + 2));
				put_dump(&res, 1, 0x00);
				break;
			case '\n':
				// empty command line
				break;
			default:
				// command not found
				xputs_P(PSTR("Unknown command!\n"));
			}
			xputs_P(PSTR("avr$ "));
		}
	}

	return 0;
}
