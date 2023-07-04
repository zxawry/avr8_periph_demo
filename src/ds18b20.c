// file: ds18b20.c
// author: awry

#include "ds18b20.h"

#include <avr/io.h>
#include <util/delay.h>

#include <convert.h>

#define DS18B20_CMD_CONVERT            (0x44)
#define DS18B20_CMD_RSCRATCHPAD        (0xbe)
#define DS18B20_CMD_WRITESCRATCHPAD    (0x4e)
#define DS18B20_CMD_COPYSCRATCHPAD     (0x48)
#define DS18B20_CMD_RECALLE            (0xb8)
#define DS18B20_CMD_RPWRSPPLY          (0xb4)
#define DS18B20_CMD_SEARCHROM          (0xf0)
#define DS18B20_CMD_READROM            (0x33)
#define DS18B20_CMD_MATCHROM           (0x55)
#define DS18B20_CMD_SEARCHROM          (0xf0)
#define DS18B20_CMD_ALARMSEARCH        (0xec)
#define DS18B20_CMD_SKIPROM            (0xcc)

#define DS18B20_DELAY_RESET            (480U)
#define DS18B20_DELAY_PRESENCE         (60U)
#define DS18B20_DELAY_SLOT             (60U)
#define DS18B20_SAMPLE_TIME            (10U)
#define DS18B20_DELAY_CONVERT          (750000U)
#define DS18B20_DELAY_RW_PULSE         (1U)
#define DS18B20_DELAY_R_RECOVER        (DS18B20_DELAY_SLOT - DS18B20_SAMPLE_TIME)

// send a reset pulse to the bus.
static uint8_t _reset(void);

// send one bit to the bus.
static void _send(const uint8_t data);

// receive one bit from the bus.
static uint8_t _recv(void);

// write one byte to the bus.
static int _write(const uint8_t data);

// read one byte from the bus.
static int _read(uint8_t * data);

static uint8_t _reset(void)
{
	uint8_t res;

	// pull the bus down.
	DDRB |= _BV(PB0);
	PORTB &= ~_BV(PB0);

	// wait for 480us.
	_delay_us(DS18B20_DELAY_RESET);

	// release the bus.
	DDRB &= ~_BV(PB0);

	// wait for 60us.
	_delay_us(DS18B20_DELAY_PRESENCE);

	// sample the bus.
	res = PINB & _BV(PB0);

	// wait for 480us.
	_delay_us(DS18B20_DELAY_RESET);

	return res;
}

static void _send(const uint8_t data)
{
	// pull the bus down.
	DDRB |= _BV(PB0);
	PORTB &= ~_BV(PB0);
		
	if (data) {
		// release the bus.
		DDRB &= ~_BV(PB0);

		_delay_us(DS18B20_DELAY_SLOT);
	} else {
		_delay_us(DS18B20_DELAY_SLOT);

		// release the bus.
		DDRB &= ~_BV(PB0);
	}
			
	// wait for recovery time.
	_delay_us(1);
}

static uint8_t _recv(void)
{
	uint8_t b;

	// pull the bus down.
	DDRB |= _BV(PB0);
	PORTB &= ~_BV(PB0);

	// release the bus.
	DDRB &= ~_BV(PB0);

	// wait for 30us.
	_delay_us(DS18B20_SAMPLE_TIME);

	// sample the bus.
	b = PINB & _BV(PB0);

	// wait for recovery time.
	_delay_us(DS18B20_DELAY_R_RECOVER);

	return b;
}

static int _write(const uint8_t data)
{
	uint8_t i;

	for (i = 1; i > 0; i <<= 1) {
		_send(data & i);
	}

	return i;
}

static int _read(uint8_t * data)
{
	uint8_t i;

	*data = 0;

	for (i = 1; i > 0; i <<= 1) {
		if (_recv()) {
			*data |= i;
		}
	}

	return i;
}

int ds18b20_init(void)
{
	if (_reset())
		return 1;
	if (_write(DS18B20_CMD_SKIPROM))
		return 1;
	if (_write(DS18B20_CMD_WRITESCRATCHPAD))
		return 1;

	if (_write(0x00))
		return 1;
	if (_write(0x00))
		return 1;
	if (_write(0x3f))
		return 1;

	if (_reset())
		return 1;
	if (_write(DS18B20_CMD_SKIPROM))
		return 1;
	if (_write(DS18B20_CMD_COPYSCRATCHPAD))
		return 1;

	while (_recv() == 0) {
	}

	return 0;
}

int ds18b20_get_temperature(char * str)
{
	uint8_t b1;
	uint8_t b2;

	if (_reset())
		return 1;
	if (_write(DS18B20_CMD_SKIPROM))
		return 1;
	if (_write(DS18B20_CMD_CONVERT))
		return 1;

	while (_recv() == 0) {
	}

	if (_reset())
		return 1;
	if (_write(DS18B20_CMD_SKIPROM))
		return 1;
	if (_write(DS18B20_CMD_RSCRATCHPAD))
		return 1;

	if (_read(&b1))
		return 1;

	if (_read(&b2))
		return 1;

	tmp_to_str((int16_t)(b2 << 8 | b1), str);

	return 0;
}
