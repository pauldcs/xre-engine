#include "dynstr.h"
#include "unit_tests.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static bool __test_001__(void) {
  dynstr_t *str = dynstr_create(0);

  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)1, "%zu");
  ASSERT_NUM_EQUAL(str->_cap, (size_t)8, "%zu");

  dynstr_append(str, "this", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)5, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this");

  dynstr_append(str, " is", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)8, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this is");

  dynstr_append(str, " a", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)10, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this is a");

  dynstr_append(str, " test", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)15, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this is a test");

  dynstr_append(str, " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)55, "%zu");
  ASSERT_STR_EQUAL(str->_ptr,
                   "this is a test !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
  dynstr_kill(str);
  return (true);
}

static bool __test_002__(void) {
  dynstr_t *str = dynstr_assign("this a test", -1);

  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)12, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this a test");

  (void)dynstr_inject(str, 5, "is ", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)15, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this is a test");

  (void)dynstr_inject(str, 0, "-> ", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)18, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "-> this is a test");

  (void)dynstr_inject(str, 17, " ?", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)20, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "-> this is a test ?");

  dynstr_kill(str);
  return (true);
}

static bool __test_003__(void) {
  dynstr_t *str = dynstr_assign("this is a unit test", -1);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)20, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this is a unit test");

  (void)dynstr_wipe(str, 5, 8);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)17, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "this a unit test");

  (void)dynstr_wipe(str, 0, 5);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)12, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "a unit test");

  (void)dynstr_wipe(str, 6, 11);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)7, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "a unit");

  (void)dynstr_wipe(str, 2, 6);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)3, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "a ");

  (void)dynstr_wipe(str, 0, 2);
  ASSERT_NUM_EQUAL(str->_nmemb, (size_t)1, "%zu");
  ASSERT_STR_EQUAL(str->_ptr, "");

  dynstr_kill(str);
  return (true);
}

// static bool __test_004__(void) {
// 	dynstr_t *str = dynstr_assign("this is a test");

// 	dynstr_kill(str);
// 	return (true);
// }

TEST_FUNCTION void dynstr_specs(void) {
  __test_start__;

  run_test(&__test_001__, "Basic dynamic string + appending");
  run_test(&__test_002__, "Inject method tests");
  run_test(&__test_003__, "Wipe method tests");
  // run_test(&__test_004__, "other method tests");
  __test_end__;
}
