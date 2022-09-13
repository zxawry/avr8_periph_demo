// file: main.c
// author: awry

#include "config.h"

#include <stdlib.h>

#include <avr/interrupt.h>

#include "usart.h"
#include "serio.h"
#include "ds1307.h"
//#include "twi.h"

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
	struct tm time;

	ds1307_get_time(&time);
	isotime_r(&time, string);
	xputs("current time is -> ");
	xputs(string);
	xputc('\n');
}

static void modify_current_time(void)
{
	char string[32];
	struct tm time;
	uint32_t epoch;

	xputs("set current epoch -> ");
	xgets(string, 32);

	if (string[0] == '\n') {
		xputs("no changes made.\n");
		return;
	}

	epoch = atol(string);
	gmtime_r(&epoch, &time);

	ds1307_set_time(&time);
	isotime_r(&time, string);
	xputs("set current time to -> ");
	xputs(string);
	xputc('\n');
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
