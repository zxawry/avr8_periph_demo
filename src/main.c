// file: main.c
// author: awry

#include "config.h"

#define __ASSERT_USE_STDERR
#include <assert.h>

#include "usart.h"

int main(void)
{
	usart_init();

	assert(0 == 1);

	for (;;) {
	}

	return 0;
}
