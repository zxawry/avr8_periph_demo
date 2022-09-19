// file: tests.c
// author: awry

#define __ASSERT_USE_STDERR
#include <assert.h>

#include "tests.h"

int run_tests(void)
{
	assert(0 == 1);

	return 0;
}
