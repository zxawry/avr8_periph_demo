// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "twi.h"

#define TWI_DEV_ADDR (0x50)

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
	if (hex >= 'a' && hex <= 'f')
		return hex - 'a' + 10;

	if (hex >= '0' && hex <= '9')
		return hex - '0';

	return 0;
}

int main(void)
{
	periph_init();

	char buffer[128];

	xputs("process started...\n");

	uint8_t rx_data[256];
	uint8_t tx_data[256];

	for(uint16_t i = 0; i < 256; i++)
		tx_data[i] = (uint8_t) i;

	uint8_t len = 0;
	uint8_t mem[2] = { 0, 0, };

	for (;;) {
		xputs("$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				// addr ln f
				// 00c0 10 w
				// 00c0 10 r

				mem[0] = conv_hex_to_dec(buffer[0]) << 4;
				mem[0] += conv_hex_to_dec(buffer[1]);
				mem[1] = conv_hex_to_dec(buffer[2]) << 4;
				mem[1] += conv_hex_to_dec(buffer[3]);

				len = conv_hex_to_dec(buffer[5]) << 4;
				len += conv_hex_to_dec(buffer[6]);

				twi_write_bytes(TWI_DEV_ADDR, mem, 2, TWI_NOSTOP);
				put_dump(mem, 2);

				if (buffer[8] == 'r') {
					twi_read_bytes(TWI_DEV_ADDR, rx_data, len, 0x00);
					put_dump(rx_data, len);
				} else if (buffer[8] == 'w') {
					twi_write_bytes(TWI_DEV_ADDR, tx_data, len, TWI_NOSTART);
					put_dump(rx_data, len);
				}
			}
		}
	}

	return 0;
}
