#include "array.h"
#include "unit_tests.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

static bool __test_001__(void) {
	return (true);
}

TEST_FUNCTION void array_allocations_specs(void) {
	__test_start__;

	run_test(&__test_001__, "array initial alloc");

	__test_end__;
}
