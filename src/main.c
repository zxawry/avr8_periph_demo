// file: main.c
// author: awry

#include "config.h"

#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "zerofs.h"

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
	uint8_t res;
	uint32_t i;
	char buffer[128];
	char *p;

	DWORD sector;

	periph_init();

	xputs_P(PSTR("\n\nWelcome to AVR shell!\n\n"));
	xputs_P(PSTR("avr$ "));

	for (;;) {
		if (xgets_I(buffer, 128)) {
			switch (buffer[0]) {
			case 'i':
				res = zerofs_init();
				put_dump(&res, 1);
				break;
			case 'm':
				res = zerofs_mount();
				put_dump(&res, 1);
				break;
			case 'c':
				res = zerofs_create();
				put_dump(&res, 1);
				break;
			case 'r':
				i = atol(buffer + 2);
				res = zerofs_read(buffer, i, 128);
				put_dump(&res, 1);
				put_dump(buffer, 128);
				buffer[127] = 0;
				xputs(buffer);
				xputc('\n');
				break;
			case 'w':
				p = buffer + 1;
				while (*++p != ' ');
				*p++ = 0; // NULL
				put_dump(p, 128);
				i = atol(buffer + 2);
				res = zerofs_write(p, i, strlen(p));
				put_dump(&res, 1);
				break;
			case 'a':
				p = buffer + 2;
				put_dump(p, 128);
				res = zerofs_append(p, strlen(p));
				put_dump(&res, 1);
				break;
			case 's':
				p = buffer + 2;
				while (*p++ != '\n');
				*p = 0;
				sector = atol(buffer + 2);
				res = zerofs_print(sector);
				put_dump(&res, 1);
				break;
			default:
				// command not found
				if (buffer[0])
					xputs_P(PSTR("Unknown command!\n"));
			}
			xputs_P(PSTR("avr$ "));
		}
	}

	return 0;
}
