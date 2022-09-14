// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "twi.h"

static inline void periph_init(void);
static uint8_t conv_hex_to_dec(const char hex);

static inline void periph_init(void)
{
	cli();
	usart_init();
	twi_init();
	sei();
}

static uint8_t conv_hex_to_dec(const char hex)
{
	uint8_t dec = 0;

	if (hex >= '0' && hex <= '9')
		dec = hex - '0';
	else if (hex >= 'a' && hex <= 'f')
		dec = hex - 'a' + 10;

	return dec;
}

int main(void)
{
	periph_init();

	char buffer[128];

	uint8_t tx_data[16] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	};

	uint8_t rx_data[16] = {
		0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
		0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	};

	uint8_t mem_addr[2] = { 0x00, 0x00, };

	xputs("process started...\n");

	twi_write_bytes(0x50, mem_addr, 2, TWI_NOSTOP);
	put_dump_null((uint8_t *) _debug_buffer.data, 64);
	xputs("set device address pointer to 0x0000\n");
	twi_write_bytes(0x50, tx_data, 16, TWI_NOSTART);
	put_dump_null((uint8_t *) _debug_buffer.data, 64);
	xputs("wrote 16 bytes to eeprom device\n");
	put_dump(tx_data, 16);

	twi_write_bytes(0x50, mem_addr, 2, TWI_NOSTOP);
	put_dump_null((uint8_t *) _debug_buffer.data, 64);
	xputs("set device address pointer to 0x0000\n");
	twi_read_bytes(0x50, rx_data, 16, 0x00);
	put_dump_null((uint8_t *) _debug_buffer.data, 64);
	xputs("read 16 bytes from eeprom device\n");
	put_dump(rx_data, 16);

	for (;;) {
		xputs("$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				mem_addr[0] = conv_hex_to_dec(buffer[0]) << 4;
				mem_addr[0] += conv_hex_to_dec(buffer[1]);
				mem_addr[1] = conv_hex_to_dec(buffer[2]) << 4;
				mem_addr[1] += conv_hex_to_dec(buffer[3]);
				twi_write_bytes(0x50, mem_addr, 2, TWI_NOSTOP);
				put_dump_null((uint8_t *) _debug_buffer.data,
					      64);
				put_dump(mem_addr, 2);
				twi_read_bytes(0x50, rx_data, 16, 0x00);
				put_dump_null((uint8_t *) _debug_buffer.data,
					      64);
				put_dump(rx_data, 16);
			}
		}
	}

	return 0;
}
