// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ssd1306.h"

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
	periph_init();

	char buffer[128];

	xputs("\n\nWelcome to AVR shell!\n\n");

	for (;;) {
		xputs("avr$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] == 'i') {
				ssd1306_inverse_display();
			} else if (buffer[0] == 'n') {
				ssd1306_normal_display();
			} else if (buffer[0] == 'b') {
				ssd1306_draw_column(0xff);
			} else if (buffer[0] == 'c') {
				ssd1306_draw_column(0x00);
			}
		}
	}

	return 0;
}
