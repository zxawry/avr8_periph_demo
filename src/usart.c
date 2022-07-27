// file usart.c
// author: awry

#include "config.h"

#include <avr/io.h>

#include "usart.h"

void usart_init(void)
{
	UBRR0L = (F_CPU / (16UL * USART_BAUD)) - 1;
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

int usart_putc(char data)
{
	if (bit_is_clear(UCSR0A, UDRE0))
		return -1;
	UDR0 = data;

	return 0;
}

int usart_getc(void)
{
	if (bit_is_clear(UCSR0A, RXC0))
		return -1;
	if (UCSR0A & _BV(FE0))
		return -1;
	if (UCSR0A & _BV(DOR0))
		return -1;
	char data = UDR0;
	return data;
}
