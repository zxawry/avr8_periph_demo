#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>

#include "twi.h"

// first byte of buffer at index 0 is occupied with:
// device_address | read/write
// last byte of buffer at index length + 2 is occupied with:
// resume_flag which determines if a stop condition is required
// actuall data bytes sit at indeces from 1 up to length + 1

static volatile uint8_t busy;
static struct {
	uint8_t buffer[TWI_BUFFER_LENGTH];
	uint8_t length;
	uint8_t index;
	uint8_t *data;
} transmission;

void twi_init(void)
{
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
	TWSR = 0;		// prescaler = 1

	busy = 0;

	sei();

	TWCR = _BV(TWEN);
}

uint8_t twi_wait(void)
{
	while (busy) ;
	return transmission.buffer[0];
}

void twi_start(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);
}

void twi_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

void twi_ack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

void twi_nack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
}

void twi_send(uint8_t data)
{
	TWDR = data;
}

void twi_recv(void)
{
	transmission.buffer[transmission.index++] = TWDR;
}

void twi_reply(void)
{
	if (transmission.index < (transmission.length - 1)) {
		twi_ack();
	} else {
		twi_nack();
	}
}

void twi_done(void)
{
	uint8_t *src = &transmission.buffer[1];
	uint8_t *dst = transmission.data;
	uint8_t len = transmission.length - 1;
	memcpy(dst, src, len);

	busy = 0;
}

void twi_write(uint8_t address, uint8_t * data, uint8_t length, uint8_t resume)
{
	twi_wait();

	busy = 1;

	transmission.buffer[0] = (address << 1) | TW_WRITE;
	transmission.buffer[length + 1] = resume;
	transmission.length = length + 1;
	transmission.index = 0;
	transmission.data = data;

	uint8_t *dst = &transmission.buffer[1];
	memcpy(dst, data, length);

	twi_start();
}

void twi_read(uint8_t address, uint8_t * data, uint8_t length, uint8_t resume)
{
	twi_wait();

	busy = 1;

	transmission.buffer[0] = (address << 1) | TW_READ;
	transmission.buffer[length + 1] = resume;
	transmission.length = length + 1;
	transmission.index = 0;
	transmission.data = data;

	twi_start();
}

ISR(TWI_vect, ISR_BLOCK)
{
	switch (TW_STATUS) {
	case TW_START:
	case TW_REP_START:
	case TW_MT_SLA_ACK:
	case TW_MT_DATA_ACK:
		if (transmission.index < transmission.length) {
			twi_send(transmission.buffer[transmission.index++]);
			twi_nack();
		} else {
			if (!transmission.buffer[transmission.length + 1]) {
				twi_stop();
			}
			twi_done();
		}
		break;

	case TW_MR_DATA_ACK:
		twi_recv();
	case TW_MR_SLA_ACK:
		twi_reply();
		break;

	case TW_MR_DATA_NACK:
		twi_recv();
	case TW_MT_SLA_NACK:
	case TW_MR_SLA_NACK:
	case TW_MT_DATA_NACK:
	default:
		twi_stop();
		twi_done();
		break;
	}
}
