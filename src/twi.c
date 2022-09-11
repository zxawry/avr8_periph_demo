#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>

#include "twi.h"

#include "serio.h"
#include "queue.h"

// first byte of buffer at index 0 is occupied with:
// device_address | read/write
// last byte of buffer at index length + 2 is occupied with:
// resume_flag which determines if a stop condition is required
// actuall data bytes sit at indeces from 1 up to length + 1

static volatile uint8_t busy;
static volatile uint8_t twsr;

static connection_t connection;

queue_t _debug_buffer;

void twi_init(void)
{
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
	TWSR = 0;

	// bit position 3 indicates idle state
	busy = 0;
	twsr = _BV(3);

	TWCR = _BV(TWEN);

	queue_init(&_debug_buffer);
}

uint8_t twi_wait(void)
{
	while (busy) ;
	return twsr;
	//while (!(twsr & _BV(3))) ;
	//return twsr & ~_BV(3);
}

static inline void twi_start(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE) | _BV(TWSTA);
}

static inline void twi_stop(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

static inline void twi_ack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

static inline void twi_nack(void)
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWIE);
}

static inline void twi_send(uint8_t data)
{
	TWDR = data;
}

static inline void twi_recv(void)
{
	connection.buffer[connection.index++] = TWDR;
}

/*
void twi_reply(void)
{
	if (connection.index < connection.length) {
		twi_ack();
	} else {
		twi_nack();
	}
}
*/

static void twi_done(void)
{
	busy = 0;
	twsr |= _BV(3);
}

void twi_write(uint8_t address, uint8_t * data, uint8_t length, uint8_t resume)
{
	twi_wait();

	queue_init(&_debug_buffer);

	busy = 1;
	twsr &= ~_BV(3);

	connection.address = address | TW_WRITE;
	connection.buffer = data;
	connection.length = length;
	connection.resume = resume;
	connection.index = 0;

	twi_start();
}

void twi_read(uint8_t address, uint8_t * data, uint8_t length, uint8_t resume)
{
	twi_wait();

	queue_init(&_debug_buffer);

	busy = 1;
	twsr &= ~_BV(3);

	connection.address = address | TW_READ;
	connection.buffer = data;
	connection.length = length;
	connection.resume = resume;
	connection.index = 0;

	twi_start();
}

#define S(b) (b >> 4)
#define M(b) (b & 0x0F)
#define C(b) ((b > 9) ? (b + 0x57) : (b + 0x30))
#define L(b) C(M(b))
#define H(b) C(M(S(b)))

ISR(TWI_vect, ISR_BLOCK)
{
	twsr = TW_STATUS;

	queue_enqueue(&_debug_buffer,(char) twsr);

	switch (twsr) {

	case TW_START: // 0x08
	case TW_REP_START: // 0x10

		// send address and wait for ack or nack

		// sla+w/r -> TWDR
		twi_send(connection.address);

		// TWSTA=0 TWSTO=0 TWINT=1 TWEA=x -> TWCR
		twi_nack();

		break;

	case TW_MT_SLA_ACK: // 0x18
	case TW_MT_DATA_ACK: // 0x28

		// as long as there is data to be written
		if (connection.index < connection.length) {

			// send data and wait for ack or nack

			// data -> TWDR
			twi_send(connection.buffer[connection.index++]);

			// TWSTA=0 TWSTO=0 TWINT=1 TWEA=x -> TWCR
			twi_nack();

		// no data left, send stop if no operation is needed
		// else next r/w operation will be a repeated start
		} else {

			// no need to keep alive
			if (!connection.resume) {
				// stop current transmission

				// TWSTA=0 TWSTO=1 TWINT=1 TWEA=x -> TWCR
				twi_stop();
			}
			twi_done();
		}
		break;

	case TW_MR_DATA_ACK: // 0x50

		// TWDR -> buffer
		twi_recv();
		// falls through

	case TW_MR_SLA_ACK: // 0x40

		// if more data should be read send ack else nack
		if (connection.index < (connection.length - 1)) {

			// TWSTA=0 TWSTO=0 TWINT=1 TWEA=1 -> TWCR
			twi_ack();
		} else {

			// TWSTA=0 TWSTO=0 TWINT=1 TWEA=0 -> TWCR
			twi_nack();
		}
		break;

	case TW_MR_DATA_NACK: // 0x58

		// slave has no more data to send

		// TWDR -> buffer
		twi_recv();
		// falls through

	case TW_MT_SLA_NACK: // 0x20
	case TW_MR_SLA_NACK: // 0x48
	case TW_MT_DATA_NACK: // 0x30
	default:
		// should stop the connection
		// the value of twsr indicates errors

		// TWSTA=0 TWSTO=1 TWINT=1 TWEA=x -> TWCR
		twi_stop();

		twi_done();
		break;
	}
}
