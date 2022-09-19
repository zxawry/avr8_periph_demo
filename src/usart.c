// file usart.c
// author: awry

#include "config.h"

#include <stdio.h>
#include <avr/io.h>

#include "usart.h"

static FILE _usart_stream;

void usart_init(void)
{
	UBRRL = (F_CPU / (16UL * USART_BAUD)) - 1;
	UCSRB = _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

	fdev_setup_stream(&_usart_stream, usart_putc, NULL, _FDEV_SETUP_WRITE);
	stderr = &_usart_stream;
}

int usart_putc(char data, FILE * stream)
{
	if (data == '\n')
		usart_putc('\r', stream);

	loop_until_bit_is_set(UCSRA, UDRE);

	UDR = data;

	return 0;
}

int usart_getc(FILE * stream)
{
	(void)stream;

	loop_until_bit_is_set(UCSRA, RXC);

	if (bit_is_set(UCSRA, FE))
		return _FDEV_EOF;
	if (bit_is_set(UCSRA, DOR))
		return _FDEV_ERR;

	char data = UDR;
	return data;
}
