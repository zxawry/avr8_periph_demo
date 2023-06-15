// file: main.c
// author: awry

#include "config.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ds1307.h"
#include "ssd1306.h"
#include "convert.h"

static inline void periph_init(void);
static void display_date_time(void);

static uint8_t flag = 0;

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
	ssd1306_init();
}

static void display_date_time(void)
{
	char string[32];

	ds1307_get_time(string);

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

int main(void)
{
	char buffer[128];

	periph_init();

	xputs_P(PSTR("\n\nWelcome to AVR shell!\n\n"));
	xputs_P(PSTR("avr$ "));

	for (;;) {
		if (flag == 0xff) {
			flag = 0x00;
			display_date_time();
		}
		if (xgets_I(buffer, 128)) {
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
				ds1307_set_time(buffer + 2);
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
