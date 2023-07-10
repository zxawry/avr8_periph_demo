// file: main.c
// author: awry

#include "config.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ds1307.h"
#include "ds18b20.h"
#include "ssd1306.h"
#include "convert.h"
#include "zerofs.h"

static inline void periph_init(void);
static void display_status(void);

static volatile uint8_t flag = 0x00;

ISR(INT0_vect, ISR_BLOCK)
{
	flag = 0xff;
}

static inline void periph_init(void)
{
	cli();
	usart_init();

	// set INT0 to trigger on rising edge
	MCUCR |= _BV(ISC01) | _BV(ISC00);

	// enable INT0 interrupt service routine
	GICR |= _BV(INT0);

	sei();

	ds1307_init();
	ds18b20_init();
	ssd1306_init();
	zerofs_init();
}

static void display_status(void)
{
	static uint8_t counter = 0;
	char string[33];

	string[27] = ' ';
	string[28] = ' ';
	string[29] = ' ';
	string[30] = ' ';
	string[31] = '\n';
	string[32] = '\0';

	ds1307_get_time(string);

	string[10] = '\0';

	ssd1306_set_page(0x01, 0x07);
	ssd1306_set_column(0x18, 0x67);
	ssd1306_draw_string(string, 0x00);

	string[10] = ' ';

	ssd1306_set_page(0x03, 0x07);
	ssd1306_set_column(0x20, 0x5f);
	ssd1306_draw_string(string + 11, 0x00);

	if (ds18b20_is_busy())
		return;

	string[19] = ' ';

	ds18b20_get_temperature(string + 20);
	ds18b20_start_conversion();

	string[26] = '\0';

	ssd1306_set_page(0x05, 0x07);
	ssd1306_set_column(0x28, 0x5f);
	ssd1306_draw_string(string + 20, 0x00);

	string[26] = ' ';

	ssd1306_set_page(0x00, 0x07);
	ssd1306_set_column(0x00, 0x7f);

	if (zerofs_is_busy() || (counter++ & 0x3f))
		return;

	zerofs_append(string, 32);
}

int main(void)
{
	uint8_t i;
	char *p;
	char buffer[128 + 1];

	// make sure no read past size.
	buffer[128] = 0;	// NULL

	periph_init();

	xputs_P(PSTR("\n\nWelcome to AVR shell!\n\n"));
	xputs_P(PSTR("avr$ "));

	// clear ssd1306 display.
	for (i = 0; i < 8 * 16; i++) {
		ssd1306_draw_ascii(' ', 0x00);
	}

	for (;;) {
		if (flag) {
			flag = 0x00;
			display_status();
		}
		if (xgets_I(buffer, 128)) {
			p = buffer;
			switch (*p++) {
			case 'd':
				switch (*p++) {
				case 'i':
					ssd1306_inverse_display();
					break;
				case 'n':
					ssd1306_normal_display();
					break;
				case 'w':
					ssd1306_draw_ascii(' ', 0xff);
					break;
				case 'b':
					ssd1306_draw_ascii(' ', 0x00);
					break;
				case 's':
					ssd1306_draw_string(p + 1, 0x00);
					break;
				case 'p':
					ssd1306_set_page(str_to_bin(p + 1),
							 str_to_bin(p + 4));
					break;
				case 'c':
					ssd1306_set_column(str_to_bin(p + 1),
							   str_to_bin(p + 4));
					break;
				}
				break;
			case 'f':
				switch (*p++) {
				case 'i':
					if (zerofs_init() == 0) {
						xputs_P(PSTR
							("zerofs initialised.\n"));
					}
					break;
				case 'c':
					if (zerofs_create() == 0) {
						xputs_P(PSTR
							("zerofs created.\n"));
					}
					break;
				case 'm':
					if (zerofs_mount() == 0) {
						xputs_P(PSTR
							("zerofs mounted.\n"));
					}
					break;
				case 'r':
					if (zerofs_read
					    (buffer, atol(p + 1), 128) == 0) {
						put_dump(buffer, 128);
						xputs(buffer);
						xputc('\n');
					}
					break;
				case 's':
					if (zerofs_print(atol(p + 1)) != 0) {
						xputs_P(PSTR
							("zerofs print failed.\n"));
					}
					break;
				}
				break;
			case 't':
				switch (*p++) {
				case 'd':
					display_status();
					break;
				case 'u':
					ds1307_set_time(p + 1);
					break;
				}
				break;
			}
			xputs_P(PSTR("avr$ "));
		}
	}

	return 0;
}
