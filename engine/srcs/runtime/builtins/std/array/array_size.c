#include "xre_builtin.h"
#include "xre_operations.h"
#include "xre_memory.h"
#include "xre_assert.h"
#include "array.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_array_size)
{
	__return_val_if_fail__(self, false);

	static object_t top;
	static int32_t data;

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	stack_pop(&top);

	if (!(top.flags & FLAG_READABLE)) {
		return (trigger_error_on(__left_branch, XRE_UNREADABLE_ERROR),
			false);
	}

	if (!(top.flags & FLAG_SEQUENCE)) {
		return (trigger_error_on(__left_branch,
					 XRE_UNEXPECTED_TYPE_ERROR),
			false);
	}

	data = array_size((array_t *)top.data.ptr);

	return (stack_push_flagged(self, object_create_register(data),
				   FLAG_READABLE | FLAG_MUTABLE));

	return (true);
}
