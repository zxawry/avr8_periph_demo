#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>

#include "twi.h"

#include "serio.h"

static int _start(uint8_t addr, uint8_t rw_flag);
static void _stop(void);

void twi_init(void)
{
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
	TWSR = 0;

	TWCR = _BV(TWEN);
}

int twi_read_bytes(uint8_t addr, uint8_t * data, uint8_t len, uint8_t flags)
{
	uint8_t *my_data = data;

	/* send start condition and slave address */
	if (!(flags & TWI_NOSTART)) {
		if (_start(addr, TW_READ) < 0) {
			return -1;
		}
	}

	for (uint8_t i = 0; i < len; i++) {
		/* Send NACK for last received byte */
		if ((len - i) == 1) {
			TWCR = _BV(TWEN) | _BV(TWINT);
		} else {
			TWCR = _BV(TWEA) | _BV(TWEN) | _BV(TWINT);
		}

		/* Wait for TWINT Flag set. This indicates that DATA has been received. */
		loop_until_bit_is_set(TWCR, TWINT);

		/* receive data byte */
		my_data[i] = TWDR;
	}

	/* end transmission */
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

int twi_write_bytes(uint8_t addr, uint8_t * data, uint8_t len, uint8_t flags)
{
	/* start transmission and send slave address */
	if (!(flags & TWI_NOSTART)) {
		if (_start(addr, TW_WRITE) < 0) {
			return -1;
		}
	}

	/* send out data bytes */
	for (int i = 0; i < len; i++) {
		/* Load DATA into TWDR Register.
		 * Clear TWINT bit in TWCR to start transmission of data */
		TWDR = data[i];
		TWCR = _BV(TWINT) | _BV(TWEN);

		/* Wait for TWINT Flag set. This indicates that DATA has been transmitted. */
		loop_until_bit_is_set(TWCR, TWINT);

		/* Check value of TWI Status Register. Mask prescaler bits. If status
		 * different from MT_DATA_ACK, return number of transmitted bytes */
		if ((TW_STATUS) != TW_MT_DATA_ACK) {
			return -2;
		}
	}

	/* end transmission */
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

static int _start(uint8_t addr, uint8_t rw_flag)
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

	/* Load ADDRESS and R/W Flag into TWDR Register.
	 * Clear TWINT bit in TWCR to start transmission of ADDRESS */
	TWDR = (addr << 1) | rw_flag;
	TWCR = _BV(TWINT) | _BV(TWEN);

	/* Wait for TWINT Flag set. This indicates that ADDRESS has been transmitted. */
	loop_until_bit_is_set(TWCR, TWINT);

	/* Check value of TWI Status Register. Mask prescaler bits.
	 * If status different from ADDRESS ACK go to ERROR */
	switch (TW_STATUS) {
	case TW_MT_SLA_ACK:
	case TW_MR_SLA_ACK:
		break;
	default:
		_stop();
		return -2;
	}

	return 0;
}

static void _stop(void)
{
	/* Reset TWI Interrupt Flag and transmit STOP condition */
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
	/* Wait for STOP Flag reset. This indicates that STOP has been transmitted. */
	loop_until_bit_is_clear(TWCR, TWSTO);
	TWCR = 0;
}
