#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>

#include "twi.h"

queue_t _debug_buffer;

static volatile uint8_t _busy;
static volatile uint8_t _cnt;
static volatile uint8_t _len;
static volatile uint8_t *_data;

static int _start(void);
static void _stop(void);

ISR(TWI_vect, ISR_BLOCK)
{
	uint8_t i = TW_STATUS;

	queue_enqueue(&_debug_buffer, i);

	switch (i) {
	case TW_MT_SLA_ACK:	// 0x18
	case TW_MT_DATA_ACK:	// 0x28
		if (_cnt < _len) {
			TWDR = _data[_cnt++];
			TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
			break;
		}
		goto exit;
	case TW_MR_DATA_ACK:	// 0x50
		_data[_cnt++] = TWDR;
		// falls through
	case TW_MR_SLA_ACK:	// 0x40
		if (_cnt < _len - 1) {
			TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWIE) | _BV(TWEN);
		} else {
			TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
		}
		break;
	case TW_MR_DATA_NACK:	// 0x58
		_data[_cnt++] = TWDR;
		// falls through
	default:
 exit:		TWCR = _BV(TWEN);
		_busy = 0;
	}
}

void twi_init(void)
{
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
	TWSR = 0;

	TWCR = _BV(TWEN);

	_busy = 0;
	_cnt = 0;
	_len = 0;
	_data = NULL;
}

int twi_read_bytes(uint8_t addr, uint8_t * data, uint8_t len, uint8_t flags)
{
	queue_init(&_debug_buffer);

	/* send start condition and slave address */
	if (!(flags & TWI_NOSTART)) {
		if (_start() < 0) {
			return -1;
		}
	}

	_busy = 0xff;

	_cnt = 0;
	_len = len;
	_data = data;

	if (!(flags & TWI_NOSTART)) {
		TWDR = (addr << 1) | TW_READ;
		TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
	} else {
		TWCR = _BV(TWIE) | _BV(TWEN);
	}

	while (_busy) {
	}

	/* end transmission */
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

int twi_write_bytes(uint8_t addr, uint8_t * data, uint8_t len, uint8_t flags)
{
	queue_init(&_debug_buffer);

	/* start transmission and send slave address */
	if (!(flags & TWI_NOSTART)) {
		if (_start() < 0) {
			return -1;
		}
	}

	_busy = 0xff;

	_cnt = 0;
	_len = len;
	_data = data;

	if (!(flags & TWI_NOSTART)) {
		TWDR = (addr << 1) | TW_WRITE;
		TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
	} else {
		TWCR = _BV(TWIE) | _BV(TWEN);
	}

	while (_busy) {
	}

	/* end transmission */
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

static int _start(void)
{
	/* Reset TWI Interrupt Flag and transmit START condition */
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

	/* Wait for TWINT Flag set. This indicates that the START has been
	 * transmitted, and ACK/NACK has been received.*/
	loop_until_bit_is_set(TWCR, TWINT);

	/* Check value of TWI Status Register. Mask prescaler bits.
	 * If status different from START go to ERROR */
	switch (TW_STATUS) {
	case TW_START:
	case TW_REP_START:
		break;
	default:
		_stop();
		return -1;
	}

	return 0;
}

static void _stop(void)
{
	/* Reset TWI Interrupt Flag and transmit STOP condition */
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);

	/* Wait for STOP Flag reset. This indicates that STOP has been transmitted. */
	loop_until_bit_is_clear(TWCR, TWSTO);

	// disable TWI peripheral
	TWCR = 0;
}
