// file: main.c
// author: awry

#include "config.h"

//#define __ASSERT_USE_STDERR
#include <assert.h>

#include "usart.h"

#include "queue_test.h"

int main(void)
{
	usart_init();

	fputs("\nRUNNING TESTS... \n", stderr);

	assert(queue_test_main() == 0);

	fputs("\nALL TESTS PASSED.\n", stderr);

	for (;;) {
	}

	return 0;
}
