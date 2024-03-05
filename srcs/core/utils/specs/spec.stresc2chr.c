#include "xre_unit.h"
#include "xre_utils.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

static bool __test_001__(void) {
	const char str[128];

	memcpy((void *)str, "\\x68\\x65\\x6c\\x6c\\x6f", 21);
	stresc2chr((char *)str);

	//printf("%s\n", str);

	return (true);
}


TEST_FUNCTION void xre_stresc2chr_specs(void) {
	__test_start__;

	run_test(&__test_001__, "Decode hex encoded hello");

	__test_end__;
}	