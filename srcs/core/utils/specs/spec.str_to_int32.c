#include "xre_unit.h"
#include "xre_utils.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>

static bool __test_001__(void) {
	int32_t dst;
	char   *ret;

	ret = str_to_int32("0", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("1", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 1, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("12-", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 12, "%d");
	ASSERT_STR_EQUAL(ret, "-");

	ret = str_to_int32("123456789 a", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 123456789, "%d");
	ASSERT_STR_EQUAL(ret, "a");

	ret = str_to_int32("2147483647", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, INT_MAX, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("-2147483648 - ", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, INT_MIN, "%d");
	ASSERT_STR_EQUAL(ret, "- ");

	ASSERT_IS_NULL(
		str_to_int32("2147483648", &dst)
	);

	ASSERT_IS_NULL(
		str_to_int32("-2147483649", &dst)
	);
	return (true);
}

static bool __test_002__(void) {
	int32_t dst;
	char   *ret;

	ret = str_to_int32("0x0", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0x0, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0x1", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0x1, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0XC", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0xC, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0x7fffffff ok", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0x7fffffff, "%d");
	ASSERT_STR_EQUAL(ret, "ok");

	ret = str_to_int32("-0X16cD8", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, -93400, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("-0x80000000", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, INT_MIN, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0x7fffffff", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, INT_MAX, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ASSERT_IS_NULL(
		str_to_int32("-0x80000001", &dst)
	);

	ASSERT_IS_NULL(
		str_to_int32("0x80000000", &dst)
	);

	return (true);
}

static bool __test_003__(void) {
	int32_t dst;
	char   *ret;

	ret = str_to_int32("0b0", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0x0, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0b1", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0x1, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0B1100", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0xC, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0b1111111111111111111111111111111\n\t", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, 0x7fffffff, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("-0b10110110011011000 -", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, -93400, "%d");
	ASSERT_STR_EQUAL(ret, "-");

	ret = str_to_int32("-0b10000000000000000000000000000000", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, INT_MIN, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ret = str_to_int32("0b1111111111111111111111111111111", &dst);
	ASSERT_IS_NOT_NULL(ret);
	ASSERT_NUM_EQUAL(dst, INT_MAX, "%d");
	ASSERT_STR_EQUAL(ret, "");

	ASSERT_IS_NULL(
		str_to_int32("-0b10000000000000000000000000000001", &dst)
	);

	ASSERT_IS_NULL(
		str_to_int32("0b10000000000000000000000000000000'", &dst)
	);

	return (true);
}

TEST_FUNCTION void xre_str_to_int32_specs(void) {
	__test_start__;

	run_test(&__test_001__, "decimal numbers");
	run_test(&__test_002__, "hexadecimal numbers");
	run_test(&__test_003__, "binary numbers");

	__test_end__;
}	