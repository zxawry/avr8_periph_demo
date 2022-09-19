// file: main.c
// author: awry

#include "config.h"

#define __ASSERT_USE_STDERR
#include <assert.h>

#include "usart.h"

#include "tests.h"

int main(void)
{
	usart_init();

	fputs("\nRUNNING TESTS ...\n", stderr);

	assert(run_tests() == 0);

	fputs("\nALL TESTS PASSED!\n", stderr);

	for (;;) {
	}

	return 0;
}
