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
	BYTE data[128];
	//UINT count;
	//UINT offset;
	//DWORD sector;

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
			case 'r':
				res = disk_readp(data, 1, 0, 128);
				put_dump(&res, 1, 0x00);
				put_dump(data, 128, 0x00);
				data[127] = 0;
				xputs((char *) data);
				xputc('\n');
				break;
			case 'w':
				res = disk_writep((uint8_t *) buffer + 2, strlen(buffer + 2));
				put_dump(&res, 1, 0x00);
				break;
			case 's':
				res = disk_writep(0, 1);
				put_dump(&res, 1, 0x00);
				break;
			case 'e':
				res = disk_writep(0, 0);
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
