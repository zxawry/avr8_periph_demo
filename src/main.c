// file: main.c
// author: awry

#include "config.h"

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "twi.h"

inline void periph_init(void);

inline void periph_init(void)
{
	cli();
	usart_init();
	twi_init();
	sei();
}

int main(void)
{
	periph_init();

			//  addr  secs  mins  hour  days  date  mont  year  cntl
        uint8_t tmp[65] = { 0x00,
				  0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x10,
				  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
				  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
				  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
				  0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
				  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
				  0x38, 0x09, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
			  };

	char buffer[128];
	uint8_t data[128];

	xputs("process started...\n");

	xputs("setting up device\n");
	twi_write(0xd0, tmp, 65, 0);
	twi_wait();
	//put_dump((uint8_t *)_debug_buffer.data, QUEUE_SIZE);

	//xputs("reset device counter\n");
	//twi_write(0xd0, dummy, 1, 0);
	twi_write(0xd0, (uint8_t *) "\08" , 1, 0);
	twi_wait();
	put_dump_null((uint8_t *)_debug_buffer.data, QUEUE_SIZE);

	//xputs("read from device\n");
	//twi_read(0xd0, data, 56, 0);
	//twi_wait();
	//put_dump_null((uint8_t *)_debug_buffer.data, QUEUE_SIZE);
	//put_dump(data, 56);

	for (;;) {
		xputs("$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				//xputs(buffer);
				xputs("read from device\n");
				uint8_t i;
				uint8_t * p = data;
				for (i = 0; i < 64; i++) {
					twi_read(0xd0, p++, 1, 0);
					twi_wait();
					//put_dump_null((uint8_t *)_debug_buffer.data, QUEUE_SIZE);
				}
				put_dump(data, 64);
			}
		}
	}

	return 0;
}
