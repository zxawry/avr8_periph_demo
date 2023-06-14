// file: main.c
// author: awry

#include "config.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ds1307.h"
#include "ssd1306.h"
#include "convert.h"

static inline void periph_init(void);
static void display_date_time(void);
static void update_date_time(char * string);

static inline void periph_init(void)
{
	cli();
	usart_init();
	sei();

	ds1307_init();
	ssd1306_init();
}

static void display_date_time(void)
{
	char string[32];

	ds1307_get_time(string);

	xputs(string);
	xputc('\n');

	string[10] = '\0';

	ssd1306_set_page(0x02, 0x07);
	ssd1306_set_column(0x18, 0x67);
	ssd1306_draw_string(string, 0x00);

	ssd1306_set_page(0x05, 0x07);
	ssd1306_set_column(0x20, 0x5f);
	ssd1306_draw_string(string + 11, 0x00);

	ssd1306_set_page(0x00, 0x07);
	ssd1306_set_column(0x00, 0x7f);
}

static void update_date_time(char * string)
{
	ds1307_set_time(string);
	display_date_time();
}

int main(void)
{
	char buffer[128];

	periph_init();

	xputs_P(PSTR("\n\nWelcome to AVR shell!\n\n"));

	for (;;) {
		xputs_P(PSTR("avr$ "));
		if (xgets(buffer, 128) && buffer[0] != '\n') {
			switch (buffer[0]) {
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
				ssd1306_draw_string(buffer + 2, 0x00);
				break;
			case 'p':
				ssd1306_set_page(str_to_bin(buffer + 2), str_to_bin(buffer + 5));
				break;
			case 'c':
				ssd1306_set_column(str_to_bin(buffer + 2), str_to_bin(buffer + 5));
				break;
			case 'd':
				display_date_time();
				break;
			case 'u':
				update_date_time(buffer + 2);
				break;
			default:
				// command not found
				xputs_P(PSTR("Unknown command!\n"));
			}
		}
	}

	return 0;
}
