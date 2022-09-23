#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>

#include "twi.h"

static volatile uint8_t _busy;

static uint8_t _cnt;
static uint8_t _len;
static uint8_t *_data;

static int _start(void);
static void _stop(void);

ISR(TWI_vect, ISR_BLOCK)
{
	uint8_t twsr = TW_STATUS;

	switch (twsr) {
	case TW_MR_DATA_ACK:	// 0x50
	case TW_MR_DATA_NACK:	// 0x58
		_data[_cnt++] = TWDR;
		if (twsr == TW_MR_DATA_NACK)
			goto exit;
		// falls through
	case TW_MR_SLA_ACK:	// 0x40
		if (_cnt < _len - 1) {
			TWCR = _BV(TWINT) | _BV(TWEA) | _BV(TWIE) | _BV(TWEN);
		} else {
			TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
		}
		break;
	case TW_MT_SLA_ACK:	// 0x18
	case TW_MT_DATA_ACK:	// 0x28
		if (_cnt < _len) {
			TWDR = _data[_cnt++];
			TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
			break;
		}
		// falls through
	exit: default:
		TWCR = _BV(TWEN);
		_busy = 0;
	}
}

void twi_init(void)
{
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
	TWSR = 0;

	_busy = 0;

	_cnt = 0;
	_len = 0;
	_data = NULL;
}

int twi_transfer(uint8_t addr, uint8_t * data, uint8_t len, uint8_t flags)
{
	// start transmission
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
		TWDR = addr;
		TWCR = _BV(TWINT) | _BV(TWIE) | _BV(TWEN);
	} else {
		TWCR = _BV(TWIE) | _BV(TWEN);
	}

	while (_busy) {
	}

	// end transmission
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

static int _start(void)
{
	// reset twi interrupt flag and transmit start condition
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);

	// wait for start condition to be transmitted
	loop_until_bit_is_set(TWCR, TWINT);

	// check value of twi status register
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
	// reset twi interrupt flag and transmit stop condition
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);

	// wait for stop condition to be transmitted
	loop_until_bit_is_clear(TWCR, TWSTO);

	// disable twi peripheral
	TWCR = 0;
}
