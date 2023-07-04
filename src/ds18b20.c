// file: ds18b20.c
// author: awry

#include "ds18b20.h"

#include <avr/io.h>
#include <util/delay.h>

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

static uint8_t _init(void);
static uint8_t _read(void);
static void _write(const uint8_t b);

static uint8_t _init(void)
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

static uint8_t _read(void)
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

static void _write(const uint8_t data)
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

int ds18b20_init(void)
{
	if (_init())
		return 1;

	return 0;
}

int ds18b20_write(const uint8_t data)
{
	uint8_t i;

	for (i = 1; i > 0; i <<= 1) {
		_write(data & i);
	}

	return i;
}

int ds18b20_read(uint8_t * data)
{
	uint8_t i;

	*data = 0;

	for (i = 1; i > 0; i <<= 1) {
		if (_read()) {
			*data |= i;
		}
	}

	return i;
}

int ds18b20_temperature(int16_t * temperature)
{
	uint8_t data;

	*temperature = 0;

	if (ds18b20_init())
		return 1;
	if (ds18b20_write(DS18B20_CMD_SKIPROM))
		return 1;
	if (ds18b20_write(DS18B20_CMD_CONVERT))
		return 1;

	//_delay_us(DS18B20_DELAY_CONVERT + 250);
	while (_read() == 0) {
	}

	if (ds18b20_init())
		return 1;
	if (ds18b20_write(DS18B20_CMD_SKIPROM))
		return 1;
	if (ds18b20_write(DS18B20_CMD_RSCRATCHPAD))
		return 1;

	if (ds18b20_read(&data))
		return 1;
	*temperature = (int16_t) data;

	if (ds18b20_read(&data))
		return 1;
	*temperature |= (int16_t) data << 8;

	return 0;
}
