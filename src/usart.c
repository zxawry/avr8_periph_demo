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
	UDR0 = queue_dequeue(&_usart_tx_buffer);

	if (queue_is_empty(&_usart_tx_buffer))
		UCSR0B &= ~_BV(UDRIE0);
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	char data = UDR0;

	if (!queue_is_full(&_usart_rx_buffer))
		queue_enqueue(&_usart_rx_buffer, data);
}

void usart_init(void)
{
	UBRR0L = (F_CPU / (16UL * USART_BAUD)) - 1;
	UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

	queue_init(&_usart_rx_buffer);
	queue_init(&_usart_tx_buffer);
}

int usart_putc(char data)
{
	if (queue_is_full(&_usart_tx_buffer))
		return -1;

	queue_enqueue(&_usart_tx_buffer, data);
	UCSR0B |= _BV(UDRIE0);

	return 0;
}

int usart_getc(void)
{
	if (queue_is_empty(&_usart_rx_buffer))
		return -1;

	char data = queue_dequeue(&_usart_rx_buffer);

	return (int) data;
}
