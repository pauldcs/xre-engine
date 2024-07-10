#include "array.h"
#include "xre_assert.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
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

	if (!__object_has_attr(&top, ATTR_READABLE)) {
		return (set_current_error(__left_branch, XRE_UNREADABLE_ERROR),
			false);
	}

	if (!__object_has_attr(&top, ATTR_SEQUENCE)) {
		return (set_current_error(__left_branch,
					  XRE_UNEXPECTED_TYPE_ERROR),
			false);
	}

	data = array_size((array_t *)top.data.ptr);

	return (stack_push_enable_attrs(self, object_register_create(data),
					ATTR_READABLE | ATTR_MUTABLE));

	return (true);
}
