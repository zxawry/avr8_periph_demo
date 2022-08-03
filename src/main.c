// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "twi.h"

inline void periph_init(void);

inline void periph_init(void)
{
	cli();
	usart_init();
	twi_init();
	sei();
}

int main(void)
{
	periph_init();

	char buffer[128];
	uint8_t data[128];

	xputs("process started...\n");

	for (;;) {
		xputs("$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				xputs(buffer);
				twi_read(0x68, data, 0x40, 0x00);
				twi_wait();
				put_dump(data, 0x40);
			}
		}
	}

	return 0;
}
