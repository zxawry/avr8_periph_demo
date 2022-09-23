// file: main.c
// author: awry

#include "config.h"

#include <stdlib.h>

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ds1307.h"

static inline void periph_init(void);
static void display_current_time(void);
static void modify_current_time(void);

static inline void periph_init(void)
{
	cli();
	usart_init();
	ds1307_init();
	sei();
}

static void display_current_time(void)
{
	char string[32];

	ds1307_get_time(string);

	xputs("current time is -> ");
	xputs(string);
	xputc('\n');
}

static void modify_current_time(void)
{
	char string[32];

	xputs("set current time to -> ");
	xgets(string, 32);

	if (string[0] == '\n') {
		xputs("no changes made.\n");
		return;
	}

	ds1307_set_time(string);

	xputs("time updated.\n");
}

int main(void)
{
	periph_init();

	char buffer[128];

	xputs("\n\nWelcome to AVR shell!\n\n");

	display_current_time();
	modify_current_time();

	for (;;) {
		xputs("avr$ ");
		if (xgets(buffer, 128)) {
			if (buffer[0] != '\n') {
				display_current_time();
			}
		}
	}

	return 0;
}
