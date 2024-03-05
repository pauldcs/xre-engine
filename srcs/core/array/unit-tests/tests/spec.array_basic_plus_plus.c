#include "array.h"
#include "unit_tests.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct {
	const char *_str;
	size_t _len;
} test_1_structure;

bool is_longer_than_four(const void *elem) {
	const test_1_structure *p = elem;
	
	if (p->_len > 4)
		return (false);
	return (true);
}

static bool __test_001__(void) {
	test_1_structure buf[] = {
		{"hello", 5},
		{"test", 4},
		{"bonjour", 7},
		{"foo", 3}
	};
	
	array_t *a = array_borrow_buffer((void **)&buf, sizeof(buf), sizeof(test_1_structure), 4, NULL);

	assert(a->_nmemb == 4);
	
	array_t *filtered = array_filter(a, &is_longer_than_four);

	assert(filtered->_nmemb == 2);
	
	return (true);
}


TEST_FUNCTION void array_basic_plus_plus_specs(void) {
	__test_start__;

	run_test(&__test_001__, "array filter tests");

	__test_end__;
}
