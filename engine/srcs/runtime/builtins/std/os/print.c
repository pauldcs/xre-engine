#include "xre_builtin.h"
#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_print)
{
	__return_val_if_fail__(self, false);

	static object_t *top;

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	top = (object_t *)stack_top();

	if (!__stack_top_check_flags(FLAG_READABLE)) {
		return (trigger_error_on(__left_branch, XRE_UNREADABLE_ERROR),
			false);
	}

	top->repr(top->data.ptr);
	(void)fprintf(stderr, "\n");

	return (true);
}
