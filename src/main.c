// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"

inline void periph_init(void);

inline void periph_init(void)
{
	cli();
	usart_init();
	sei();
}

int main(void)
{
	periph_init();

	char buffer[128];

	xputs("process started...\n");

	for (;;) {
		xputs("$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				xputs(buffer);
			}
		}
	}

	return 0;
}
