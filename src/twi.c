#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <string.h>

#include "twi.h"

#include "serio.h"

static int _start(uint8_t address, uint8_t rw_flag);
static int _write(const uint8_t * data, int length);
static void _stop(void);

void twi_init(void)
{
	TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;
	TWSR = 0;

	TWCR = _BV(TWEN);
}

int twi_read_bytes(uint8_t address, uint8_t * data, uint8_t length,
		   uint8_t flags)
{
	uint8_t *my_data = data;

	/* send start condition and slave address */
	if (!(flags & TWI_NOSTART)) {
		if (_start(address, TW_READ) < 0) {
			return -1;
		}
	}

	for (uint8_t i = 0; i < length; i++) {
		/* Send NACK for last received byte */
		if ((length - i) == 1) {
			TWCR = (1 << TWEN) | (1 << TWINT);
		} else {
			TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWINT);
		}

		/* Wait for TWINT Flag set. This indicates that DATA has been received. */
		while (!(TWCR & (1 << TWINT))) {
		}

		/* receive data byte */
		my_data[i] = TWDR;
	}

	/* end transmission */
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

int twi_write_bytes(uint8_t address, uint8_t * data, uint8_t length,
		    uint8_t flags)
{
	/* start transmission and send slave address */
	if (!(flags & TWI_NOSTART)) {
		if (_start(address, TW_WRITE) < 0) {
			return -1;
		}
	}

	/* send out data bytes */
	if (_write(data, length) < 0) {
		return -2;
	}

	/* end transmission */
	if (!(flags & TWI_NOSTOP)) {
		_stop();
	}

	return 0;
}

static int _start(uint8_t address, uint8_t rw_flag)
{
	/* Reset TWI Interrupt Flag and transmit START condition */
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	/* Wait for TWINT Flag set. This indicates that the START has been
	 * transmitted, and ACK/NACK has been received.*/
	while (!(TWCR & (1 << TWINT))) {
	}

	/* Check value of TWI Status Register. Mask prescaler bits.
	 * If status different from START go to ERROR */
	if ((TWSR & 0xF8) == TW_START) {
	} else if ((TWSR & 0xF8) == TW_REP_START) {
	} else {
		_stop();
		return -1;
	}

	/* Load ADDRESS and R/W Flag into TWDR Register.
	 * Clear TWINT bit in TWCR to start transmission of ADDRESS */
	TWDR = (address << 1) | rw_flag;
	TWCR = (1 << TWINT) | (1 << TWEN);

	/* Wait for TWINT Flag set. This indicates that ADDRESS has been transmitted. */
	while (!(TWCR & (1 << TWINT))) {
	}

	/* Check value of TWI Status Register. Mask prescaler bits.
	 * If status different from ADDRESS ACK go to ERROR */
	if ((TWSR & 0xF8) == TW_MT_SLA_ACK) {
	} else if ((TWSR & 0xF8) == TW_MR_SLA_ACK) {
	} else {
		_stop();
		return -2;
	}

	return 0;
}

/* TODO : const uint8_t data instead of *data */
static int _write(const uint8_t * data, int length)
{
	for (int i = 0; i < length; i++) {
		/* Load DATA into TWDR Register.
		 * Clear TWINT bit in TWCR to start transmission of data */
		TWDR = data[i];
		TWCR = (1 << TWINT) | (1 << TWEN);

		/* Wait for TWINT Flag set. This indicates that DATA has been transmitted. */
		while (!(TWCR & (1 << TWINT))) {
		}

		/* Check value of TWI Status Register. Mask prescaler bits. If status
		 * different from MT_DATA_ACK, return number of transmitted bytes */
		if ((TWSR & 0xF8) != TW_MT_DATA_ACK) {
			return -1;
		} else {
		}
	}

	return 0;
}

static void _stop(void)
{
	/* Reset TWI Interrupt Flag and transmit STOP condition */
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	/* Wait for STOP Flag reset. This indicates that STOP has been transmitted. */
	while (TWCR & (1 << TWSTO)) {
	}
	TWCR = 0;
}
