#ifndef __UNIT_TESTS_H__
#define __UNIT_TESTS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
	size_t passed;
	size_t failed;
} specs_state_t;

#define TIMEOUT_LIMIT 5 /* seconds */

extern specs_state_t __specs__;

#define TEST_OK 1
#define TEST_KO 0
#define TEST_FUNCTION __attribute__((constructor))

#define BWHT "\033[1;37m"
#define CRESET "\033[0m"

#define BYEL "\033[1;33m"
#define BRED "\033[1;31m"

#define __test_start__                                                      \
	fprintf(stderr, "Launching %s\nFunction: %s%s%s\n", __FILE__, BWHT, \
		"__PRETTY_FUNCTION__", CRESET);

#define __test_end__ fprintf(stderr, "\n");

bool run_test(bool (*test)(void), const char *title);

#define ASSERT_NUM_EQUAL(actual, expected, format_specifier)                                    \
	do {                                                                                    \
		__typeof__(actual) _actual = (actual);                                          \
		__typeof__(expected) _expected = (expected);                                    \
		if (_actual != _expected) {                                                     \
			printf("\n"                                                             \
			       "  - line (%s%d%s)\n    +\tActual: %s%s%s == '" format_specifier \
			       "'\n"                                                            \
			       "    - Expected: " format_specifier "' -> ",                     \
			       BYEL, __LINE__, CRESET, BRED, #actual, CRESET,                   \
			       actual, expected);                                               \
			return (false);                                                         \
		}                                                                               \
	} while (0)

#define ASSERT_STR_EQUAL(actual, expected)                                    \
	do {                                                                  \
		if (strcmp(actual, expected) != 0) {                          \
			printf("\n"                                           \
			       "  - line (%s%d%s)\n    +\tActual: '%s%s%s'\n" \
			       "    - Expected: '%s' ",                       \
			       BYEL, __LINE__, CRESET, BRED, actual, CRESET,  \
			       expected);                                     \
			return (false);                                       \
		}                                                             \
	} while (0)

#define ASSERT_IS_NULL(actual)                                                \
	do {                                                                  \
		if (actual) {                                                 \
			printf("\n"                                           \
			       "  - line (%s%d%s)\n    +\tActual: %s%p%s\n"   \
			       "    - Expected: NULL ",                       \
			       BYEL, __LINE__, CRESET, BRED, actual, CRESET); \
			return (false);                                       \
		}                                                             \
	} while (0)

#define ASSERT_IS_NOT_NULL(actual)                          \
	do {                                                \
		if (!actual) {                              \
			printf("\n"                         \
			       "  - line (%s%d%s)\n "       \
			       "    - Expected: NOT NULL ", \
			       BYEL, __LINE__, CRESET);     \
			return (false);                     \
		}                                           \
	} while (0)

#endif /* __UNIT_TESTS_H__*/
