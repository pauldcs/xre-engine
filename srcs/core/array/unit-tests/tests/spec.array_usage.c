#include "array.h"
#include "unit_tests.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

static void bubble_sort(array_t *array) {
  size_t size = array_size(array);

  while (true) {
    bool sorted = true;
    size_t i = 0;
    while (i + 1 < size) {
      if (*(int *)array_at(array, i) > *(int *)array_at(array, i + 1)) {
        array_swap_elems(array, i, i + 1);
        sorted = false;
      }
      i++;
    }
    if (sorted)
      break;
  }
}

static int *generate_random_numbers(void) {
  static int randomNumbers[1000];
  srand(time(NULL));
  for (int i = 0; i < 1000; ++i) {
    randomNumbers[i] = rand() % 1000;
  }
  return randomNumbers;
}

static bool __test_001__(void) {
  array_t *arr = array_create(sizeof(int), 0, NULL);
  int *randnums = generate_random_numbers();
  size_t i = 1000;
  array_append(arr, randnums, i);

  bubble_sort(arr);
  //array_trace(arr);

  size_t j = 0;
  while (j + 1 < 1000) {
    if (*(int *)array_at(arr, j) > *(int *)array_at(arr, j + 1)) {
      return (false);
    }
    j++;
  }
  return (true);
}

TEST_FUNCTION void array_usage_specs(void) {
  __test_start__;

  run_test(&__test_001__, "push 10000 1 by 1 and bubble sort them");

  __test_end__;
}
