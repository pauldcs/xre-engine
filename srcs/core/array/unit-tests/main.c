#include "unit_tests.h"
#include <stdio.h>

int main(void) {
  fprintf(stdout,
          "TESTS SUMMARY:\n"
          "   tests failed: %zu\n"
          "   tests passed: %zu\n"
          "          total: %zu\n"
          "\n",
          __specs__.failed, __specs__.passed,
          __specs__.failed + __specs__.passed);
  return (0);
}
