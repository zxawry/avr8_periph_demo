// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"

void put_dump(const char * input);

int wputc(char c);
int wgetc(void);
int wputs(const char *s);
int wgets(char *b, int n);

int main(void)
{
	cli();
	usart_init();
	sei();

	char buffer[128];

	wputs("process started...\n");

	for (;;) {
		//wputc('>');
		wputs("$ ");
		if (wgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				wputs(buffer);
			}
		}
		//put_dump(buffer);
	}

	return 0;
}

int wputc(char c)
{
	if (c == '\a')
		return 0;
	if (c == '\n')
		wputc('\r');

	int e;
	do {
		e = usart_putc(c);
	} while (e < 0);

	return e;
}

int wputs(const char *s)
{
	while (*s)
		wputc(*s++);
	return 0;
}

int wgetc(void)
{
	int c;
	do {
		c = usart_getc();
	} while (c < 0);

	if (c == '\r')
		c = '\n';
	if (c == '\n')
		wputc(c);

	return c;
}

int wgets(char *b, int n)
{
	int c, i = 0;

	for (;;) {
		c = wgetc();

		if (c < 0)
			break;
		if (c == '\n') {
			b[i++] = c;
			break;
		}
		if ((c == '\b' || c == '\x7f') && i > 0) {
			i--;
			wputc('\b');
			wputc(' ');
			wputc('\b');
			continue;
		}
		if (((c >= ' ' && c <= '\x7e') || c >= '\xa0') && i < n - 1) {
			b[i++] = c;
			wputc(c);
		}
	}

	b[i] = '\0';
	return (int) (c == '\n');
}

void put_dump(const char * input)
{
	char c, h, l;
	wputc('[');
	wputc(' ');
	while (*input) {
		c = *input++;
		h = (c & 0xF0) >> 4;
		if (h > 9) h += 0x27;
		l = c & 0x0F;
		if (l > 9) l += 0x27;
		wputc(h + '0');
		wputc(l + '0');
		wputc(' ');
	}
	wputc(']');
	wputc('\n');
}
