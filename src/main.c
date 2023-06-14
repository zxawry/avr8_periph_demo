// file: main.c
// author: awry

#include "config.h"

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ssd1306.h"
#include "convert.h"

static inline void periph_init(void);

static inline void periph_init(void)
{
	cli();
	usart_init();
	sei();

	ssd1306_init();
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
			default:
				// command not found
				xputs_P(PSTR("Unknown command\n"));
			}
		}
	}

	return 0;
}
