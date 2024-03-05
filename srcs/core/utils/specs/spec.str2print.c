#include "xre_unit.h"
#include "xre_utils.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

static bool __test_001__(void) {
	const char str[128];
  const char buf[128];

	memcpy((void *)str, "\xff\xff\xff\xff", 5);
	stresc2chr((char *)str);

  char *res = str2print((char *)buf, 128, str);
	//printf("%s\n", res);

	return (true);
}


TEST_FUNCTION void xre_str2print_specs(void) {
	__test_start__;

	run_test(&__test_001__, "Decode raw bytes into octal");

	__test_end__;
}
