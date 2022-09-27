// file: serio.c
// author: awry

#include "serio.h"

#include "usart.h"

int xputc(char c)
{
	if (c == '\a')
		return 0;
	if (c == '\n')
		xputc('\r');

	int e;
	do {
		e = usart_putc(c);
	} while (e < 0);

	return e;
}

int xputs(const char *s)
{
	while (*s)
		xputc(*s++);
	return 0;
}

int xgetc(void)
{
	int c;
	do {
		c = usart_getc();
	} while (c < 0);

	if (c == '\r')
		c = '\n';
	if (c == '\n')
		xputc(c);

	return c;
}

int xgets(char *b, int n)
{
	int c, i = 0;

	for (;;) {
		c = xgetc();

		if (c < 0)
			break;
		if (c == '\n') {
			b[i++] = c;
			break;
		}
		if ((c == '\b' || c == '\x7f') && i > 0) {
			i--;
			xputc('\b');
			xputc(' ');
			xputc('\b');
			continue;
		}
		if (((c >= ' ' && c <= '\x7e') || c >= '\xa0') && i < n - 1) {
			b[i++] = c;
			xputc(c);
		}
	}

	b[i] = '\0';
	return (int)(c == '\n');
}

void put_dump(const char *input)
{
	char c, h, l;
	xputc('[');
	xputc(' ');
	while (*input) {
		c = *input++;
		h = (c & 0xF0) >> 4;
		if (h > 9)
			h += 0x27;
		l = c & 0x0F;
		if (l > 9)
			l += 0x27;
		xputc(h + '0');
		xputc(l + '0');
		xputc(' ');
	}
	xputc(']');
	xputc('\n');
}
