// file: serio.c
// author: awry

#include "serio.h"

#include "usart.h"

#define S(b) (b >> 4)
#define M(b) (b & 0x0F)
#define C(b) ((b > 9) ? (b + 0x57) : (b + 0x30))
#define L(b) C(M(b))
#define H(b) C(M(S(b)))

void put_dump(void *b, uint8_t n)
{
	uint8_t e;
	uint8_t i;
	uint8_t *p;

	i = 0;
	p = (uint8_t *) b;
	while (i < n) {
		xputc('0');
		xputc('x');
		xputc(H(i));
		xputc(L(i));
		xputc('\t');
		for (e = 0, i = i; e < 8; e++, i++) {
			xputc(' ');
			xputc((i < n) ? H(p[i]) : ' ');
			xputc((i < n) ? L(p[i]) : ' ');
		}
		xputc('\t');
		for (e = 0, i -= 8; e < 8; e++, i++) {
			if (i < n)
				xputc(((p[i] >= 0x20) && (p[i] < 0x7F))
				      ? p[i] : '.');
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
			//b[i++] = c;
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
	if (((c >= ' ' && c <= '\x7e') || c >= '\xa0') && i < n - 1) {
		b[i++] = c;
		xputc(c);
		return 0;
	}
	if (c == '\r')
		c = '\n';
	if (c == '\n') {
		xputc(c);
		//b[i++] = c;
		b[i] = '\0';
		i = 0;
		return (int)(c == '\n');
	}

	return 0;
}
