#include "array.h"
#include "unit_tests.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool __test_001__(void)
{
	array_t *v = array_create(sizeof(const char *), 10, NULL);
	assert(array_insert(v, 0, __PTRIZE_STR__("333")));
	assert(array_insert(v, 0, __PTRIZE_STR__("222")));
	assert(array_insert(v, 0, __PTRIZE_STR__("111")));
	assert(array_insert(v, 3, __PTRIZE_STR__("666")));
	assert(array_insert(v, 3, __PTRIZE_STR__("555")));
	assert(array_insert(v, 3, __PTRIZE_STR__("444")));
	assert(array_insert(v, 6, __PTRIZE_STR__("777")));
	assert(array_insert(v, 0, __PTRIZE_STR__("000")));
	array_swap_elems(v, 0, 7);
	array_swap_elems(v, 1, 6);
	assert(!strcmp(*(char **)array_at(v, 0), "777"));
	assert(!strcmp(*(char **)array_at(v, 1), "666"));
	assert(!strcmp(*(char **)array_at(v, 2), "222"));
	assert(!strcmp(*(char **)array_at(v, 3), "333"));
	assert(!strcmp(*(char **)array_at(v, 4), "444"));
	assert(!strcmp(*(char **)array_at(v, 5), "555"));
	assert(!strcmp(*(char **)array_at(v, 6), "111"));
	assert(!strcmp(*(char **)array_at(v, 7), "000"));
	array_kill(v);
	return (true);
}

static bool __test_002__(void)
{
	array_t *v = array_create(sizeof(int32_t), 10, NULL);
	assert(array_push(v, __PTRIZE_ST32__(0)));
	assert(array_push(v, __PTRIZE_ST32__(111)));
	assert(array_push(v, __PTRIZE_ST32__(222)));
	assert(array_push(v, __PTRIZE_ST32__(333)));
	assert(array_push(v, __PTRIZE_ST32__(444)));
	assert(array_push(v, __PTRIZE_ST32__(555)));
	array_evict(v, 5);
	array_evict(v, 0);
	array_evict(v, 2);
	assert(array_insert(v, 2, __PTRIZE_ST32__(777)));
	array_evict(v, 1);
	array_swap_elems(v, 0, 2);
	array_swap_elems(v, 1, 2);
	assert(*(int32_t *)array_at(v, 0) == 444);
	assert(*(int32_t *)array_at(v, 1) == 111);
	assert(*(int32_t *)array_at(v, 2) == 777);
	array_kill(v);
	return (true);
}

static bool __test_003__(void)
{
	array_t *v = array_create(sizeof(int64_t), 10, NULL);

	assert(array_push(v, __PTRIZE_ST64__(0)));
	assert(array_push(v, __PTRIZE_ST64__(1)));
	assert(array_push(v, __PTRIZE_ST64__(2)));
	assert(array_push(v, __PTRIZE_ST64__(3)));
	assert(array_push(v, __PTRIZE_ST64__(4)));
	assert(array_push(v, __PTRIZE_ST64__(5)));
	array_pop(v, array_access(v, 0));
	array_pop(v, array_access(v, 1));
	array_pop(v, array_access(v, 2));
	assert(array_push(v, __PTRIZE_ST64__(2)));
	assert(array_push(v, __PTRIZE_ST64__(0)));
	assert(array_insert(v, 4, __PTRIZE_ST64__(1)));
	assert(array_size(v) == 6);
	assert(*(int32_t *)array_at(v, 0) == 5);
	assert(*(int32_t *)array_at(v, 1) == 4);
	assert(*(int32_t *)array_at(v, 2) == 3);
	assert(*(int32_t *)array_at(v, 3) == 2);
	assert(*(int32_t *)array_at(v, 4) == 1);
	assert(*(int32_t *)array_at(v, 5) == 0);
	array_pop(v, NULL);
	array_pop(v, NULL);
	array_pop(v, NULL);
	array_pop(v, NULL);
	array_pop(v, NULL);
	array_pop(v, NULL);
	assert(!array_size(v));
	array_kill(v);
	return (true);
}

static bool __test_004__(void)
{
	array_t *v = array_create(sizeof(int64_t), 10, NULL);

	assert(array_pushf(v, __PTRIZE_ST64__(0)));
	assert(array_pushf(v, __PTRIZE_ST64__(1)));
	assert(array_pushf(v, __PTRIZE_ST64__(2)));
	assert(array_pushf(v, __PTRIZE_ST64__(3)));
	assert(array_pushf(v, __PTRIZE_ST64__(4)));
	assert(array_pushf(v, __PTRIZE_ST64__(5)));
	assert(*(int32_t *)array_at(v, 0) == 5);
	assert(*(int32_t *)array_at(v, 1) == 4);
	assert(*(int32_t *)array_at(v, 2) == 3);
	assert(*(int32_t *)array_at(v, 3) == 2);
	assert(*(int32_t *)array_at(v, 4) == 1);
	assert(*(int32_t *)array_at(v, 5) == 0);
	assert(array_size(v) == 6);
	array_popf(v, NULL);
	array_popf(v, NULL);
	array_popf(v, NULL);
	assert(*(int32_t *)array_at(v, 0) == 2);
	assert(*(int32_t *)array_at(v, 1) == 1);
	assert(*(int32_t *)array_at(v, 2) == 0);
	assert(array_size(v) == 3);
	array_popf(v, NULL);
	array_popf(v, NULL);
	array_popf(v, NULL);
	assert(array_size(v) == 0);
	array_kill(v);
	return (true);
}

static bool __test_005__(void)
{
	array_t *v = array_create(sizeof(int64_t), 10, NULL);

	assert(array_push(v, __PTRIZE_ST64__(0)));
	assert(array_push(v, __PTRIZE_ST64__(1)));
	assert(array_push(v, __PTRIZE_ST64__(2)));
	assert(array_push(v, __PTRIZE_ST64__(3)));
	assert(array_push(v, __PTRIZE_ST64__(4)));
	assert(array_push(v, __PTRIZE_ST64__(5)));

	array_t *pull = array_pull(v, 1, 3);
	assert(array_size(pull) == 3);

	assert(*(int64_t *)array_at(pull, 0) == 1);
	assert(*(int64_t *)array_at(pull, 1) == 2);
	assert(*(int64_t *)array_at(pull, 2) == 3);
	array_kill(pull);

	pull = array_pull(v, -1, 1);
	assert(array_size(pull) == 5);

	assert(*(int64_t *)array_at(pull, 0) == 5);
	assert(*(int64_t *)array_at(pull, 1) == 4);
	assert(*(int64_t *)array_at(pull, 2) == 3);
	assert(*(int64_t *)array_at(pull, 3) == 2);
	assert(*(int64_t *)array_at(pull, 4) == 1);
	array_kill(pull);

	pull = array_pull(v, 2, 0);
	assert(array_size(pull) == 3);

	assert(*(int64_t *)array_at(pull, 0) == 2);
	assert(*(int64_t *)array_at(pull, 1) == 1);
	assert(*(int64_t *)array_at(pull, 2) == 0);
	array_kill(pull);

	pull = array_pull(v, -1, -5);
	assert(array_size(pull) == 5);

	assert(*(int64_t *)array_at(pull, 0) == 5);
	assert(*(int64_t *)array_at(pull, 1) == 4);
	assert(*(int64_t *)array_at(pull, 2) == 3);
	assert(*(int64_t *)array_at(pull, 3) == 2);
	assert(*(int64_t *)array_at(pull, 4) == 1);
	array_kill(pull);

	pull = array_pull(v, 2, 2);
	assert(array_size(pull) == 1);

	assert(*(int64_t *)array_at(pull, 0) == 2);
	array_kill(pull);

	pull = array_pull(v, 3, -1);
	assert(array_size(pull) == 3);

	assert(*(int64_t *)array_at(pull, 0) == 3);
	assert(*(int64_t *)array_at(pull, 1) == 4);
	assert(*(int64_t *)array_at(pull, 2) == 5);
	array_kill(pull);

	array_kill(v);
	return (true);
}

static bool __test_006__(void)
{
	array_t *v = array_create(sizeof(char), 10, NULL);

	assert(array_append(v, "Hello", 5));
	assert(array_append(v, ", ", 2));
	assert(array_append(v, "World!\0", 7));
	assert(!strcmp((char *)array_at(v, 0), "Hello, World!"));

	array_kill(v);

	v = array_create(sizeof(char), 0, NULL);

	assert(array_append(v, "h", 1));
	assert(array_append(v, "e", 1));
	assert(array_append(v, "l", 1));
	assert(array_append(v, "l", 1));
	assert(array_append(v, "o", 1));
	assert(array_append(v, " ", 1));
	assert(array_append(v, "world", 6));
	assert(array_inject(v, 5, " my dear", 8));
	assert(array_size(v) == 20);
	assert(!strcmp(v->_ptr, "hello my dear world"));

	array_kill(v);
	return (true);
}

static bool __test_007__(void)
{
	array_t *v = array_create(sizeof(char), 10, NULL);

	assert(array_append(v, "0123456789abcdef", 17));
	assert(array_size(v) == 17);
	array_wipe(v, 10, 17);
	assert(array_size(v) == 10);
	assert(array_append(v, "ABCDEF", 7));
	assert(array_size(v) == 17);
	assert(!strcmp(v->_ptr, "0123456789ABCDEF"));
	array_wipe(v, 0, 1);
	assert(array_size(v) == 16);
	assert(!strcmp(v->_ptr, "123456789ABCDEF"));
	array_wipe(v, 2, 3);
	assert(array_size(v) == 15);
	assert(!strcmp(v->_ptr, "12456789ABCDEF"));
	array_wipe(v, 0, 14);
	assert(array_size(v) == 1);
	assert(!strcmp(v->_ptr, ""));

	array_kill(v);
	return (true);
}

static bool __test_008__(void)
{
	array_t *v = array_create(sizeof(char), 10, NULL);

	assert(array_inject(v, 0, "0123456789abcdef", 17));
	assert(array_size(v) == 17);
	assert(!strcmp(v->_ptr, "0123456789abcdef"));
	assert(array_inject(v, 0, "_", 1));
	assert(array_size(v) == 18);
	assert(!strcmp(v->_ptr, "_0123456789abcdef"));
	assert(array_inject(v, 1, "_", 1));
	assert(array_size(v) == 19);
	assert(!strcmp(v->_ptr, "__0123456789abcdef"));

	array_kill(v);
	return (true);
}

static bool __test_009__(void)
{
	array_t *v1 = array_create(sizeof(char), 1, NULL);
	array_t *v2 = array_create(sizeof(char), 1, NULL);
	
	array_append(v1, "0123456789abcdef", 17);
	array_append(v2, "!!!", 3);

	array_concat(v1, v2);

	assert(array_size(v1) == 20);

	array_kill(v1);
	array_kill(v2);

	return (true);
}

TEST_FUNCTION void array_normal_specs(void)
{
	__test_start__;

	run_test(&__test_001__, "Creating array only insert");
	run_test(&__test_002__, "push/evict basics");
	run_test(&__test_003__, "push/pop basics");
	run_test(&__test_004__, "pushf/popf basics");
	run_test(&__test_005__, "array_pull tests");
	run_test(&__test_006__, "array_append tests");
	run_test(&__test_007__, "array_wipe tests");
	run_test(&__test_008__, "array_inject tests");
	run_test(&__test_009__, "array_concat tests");

	__test_end__;
}
