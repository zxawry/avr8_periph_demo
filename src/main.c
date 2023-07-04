// file: main.c
// author: awry

#include "config.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usart.h"
#include "serio.h"
#include "ds18b20.h"
#include "convert.h"

static inline void periph_init(void);

static inline void periph_init(void)
{
	cli();
	usart_init();
	sei();
}

int main(void)
{
	//uint8_t i;
	//uint8_t data;
	int16_t temperature;
	char buffer[128];

	periph_init();

	xputc('\n');
/*
	uint16_t tmp;
	uint16_t i;

	for (tmp = 0; tmp < 0x1000; tmp++) {
		for (i = 0x8000; i > 0; i >>= 1) {
			if (tmp & i) {
				xputc('1');
			} else {
				xputc('0');
			}
		}
		xputc('\t');
		tmp_to_str((int16_t) tmp, buffer);
		xputs(buffer);
		xputc('\n');
	}

	int16_t tmp;

	tmp = 0x07d0;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0x0550;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0x0191;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0x00a2;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0x0008;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0x0000;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0xfff8;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0xff5e;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0xfe6f;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');

	tmp = 0xfc90;
	tmp_to_str(tmp, buffer);
	xputs(buffer);
	xputc('\n');
*/
	//xputs_P(PSTR("\n\nWelcome to AVR shell!\n\n"));
	//xputs_P(PSTR("avr$ "));

	for (;;) {
		if (ds18b20_temperature(&temperature)) {
			xputs_P(PSTR("DS18B20: failed to read temperature.\n"));
		} else {
			tmp_to_str(temperature, buffer);
			xputs(buffer);
			xputc('\n');
		}
	}

	return 0;
}
