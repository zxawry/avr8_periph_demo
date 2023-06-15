// file: serio.c
// author: awry

#include "serio.h"

#include "usart.h"

#define S(b) (b >> 4)
#define M(b) (b & 0x0F)
#define C(b) ((b > 9) ? (b + 0x57) : (b + 0x30))
#define L(b) C(M(b))
#define H(b) C(M(S(b)))

void put_dump(uint8_t * b, uint8_t n, uint8_t t)
{
	uint8_t e;
	uint8_t i = 0;

	while (n > 0 && (t != 0xff || b[i] != '\0')) {
		xputc('0');
		xputc('x');
		xputc(H(i));
		xputc(L(i));
		xputc('\t');
		e = (n < 8) ? n : 8;
		for (; e > 0; e--, i++, n--) {
			if (t == 0xff && b[i] == '\0')
				break;
			xputc(H(b[i]));
			xputc(L(b[i]));
			xputc(' ');
		}
		xputc('\n');
	}
}

int xputc(char c)
{
	int e;

	if (c == '\a')
		return 0;
	if (c == '\n')
		xputc('\r');

	do {
		e = usart_putc(c);
	} while (e < 0);

	return e;
}

int xputs_P(PGM_P s)
{
	char c;

	while ((c = pgm_read_byte(s++)))
		xputc(c);
	return 0;
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
	int c;
	int i = 0;

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

int xgets_I(char *b, int n)
{
	static int i = 0;
	int c;

	c = usart_getc();

	if ((c == '\b' || c == '\x7f') && i > 0) {
		i--;
		xputc('\b');
		xputc(' ');
		xputc('\b');
		return 0;
	}
	if (((c >= ' ' && c <= '\x7e') || c >= '\xa0') && i < n - 2) {
		b[i++] = c;
		xputc(c);
		return 0;
	}
	if (c == '\r')
		c = '\n';
	if (c == '\n') {
		xputc(c);
		b[i++] = c;
		b[i] = '\0';
		i = 0;
		return (int)(c == '\n');
	}

	return 0;
}
