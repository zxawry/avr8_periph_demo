// file: main.c
// author: awry

#include "config.h"
#include "usart.h"
#include "xprintf.h"

int main(void)
{
	xdev_in(usart_getc);
	xdev_out(usart_putc);
	usart_init();

	xputs("process started...\r\n");

	char buffer[256];

	for (;;) {
		xputc('>');
		if (xgets(buffer, 256)) {
			if (buffer[0] != '\n') {
				xputs(buffer);
			}
		}
	}

	return 0;
}
