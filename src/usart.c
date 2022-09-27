// file usart.c
// author: awry

#include "config.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "usart.h"
#include "queue.h"

static queue_t _usart_tx_buffer;
static queue_t _usart_rx_buffer;

ISR(USART_UDRE_vect, ISR_BLOCK)
{
	UDR = queue_dequeue(&_usart_tx_buffer);

	if (queue_is_empty(&_usart_tx_buffer))
		UCSRB &= ~_BV(UDRIE);
}

ISR(USART_RXC_vect, ISR_BLOCK)
{
	char data = UDR;

	if (!queue_is_full(&_usart_rx_buffer))
		queue_enqueue(&_usart_rx_buffer, data);
}

void usart_init(void)
{
	UBRRL = (F_CPU / (16UL * USART_BAUD)) - 1;
	UCSRB = _BV(RXCIE) | _BV(RXEN) | _BV(TXEN);
	UCSRC = _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0);

	queue_init(&_usart_rx_buffer);
	queue_init(&_usart_tx_buffer);
}

int usart_putc(char data)
{
	if (queue_is_full(&_usart_tx_buffer))
		return -1;

	queue_enqueue(&_usart_tx_buffer, data);
	UCSRB |= _BV(UDRIE);

	return 0;
}

int usart_getc(void)
{
	if (queue_is_empty(&_usart_rx_buffer))
		return -1;

	char data = queue_dequeue(&_usart_rx_buffer);

	return (int)data;
}
