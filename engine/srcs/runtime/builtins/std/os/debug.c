#include "xre_builtin.h"
#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_debug) {
	__return_val_if_fail__(self, false);

	static object_t *top;

	if (!BR_EVAL((LEFT_BRANCH))) {
		return (false);
	}

	top = (object_t *)stack_top();
	if (!STACK_TOP_CHECK_FLAGS(FLAG_READABLE)) {
		return (trigger_error_on(LEFT_BRANCH, XRE_UNREADABLE_ERROR),
			false);
	}

	top->repr(top->data.ptr);
	(void)fprintf(stderr, " debug\n");

	STACK_TOP_DISABLE_FLAGS(FLAG_READABLE);
  return (true);
}
