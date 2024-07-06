#include "dynstr.h"
#include "xre_assert.h"
#include "xre_builtin.h"
#include "xre_memory.h"
#include "xre_operations.h"
#include <stdbool.h>

XRE_BUILTIN_FUNCTION(builtin_str_size)
{
	__return_val_if_fail__(self, false);

	static object_t top;
	static int32_t data;

	if (!__br_eval(__left_branch)) {
		return (false);
	}

	stack_pop(&top);

	if (!(top.attrs & ATTR_READABLE)) {
		return (set_current_error(__left_branch, XRE_UNREADABLE_ERROR),
			false);
	}

	if (!(top.attrs & ATTR_STRING)) {
		return (set_current_error(__left_branch,
					  XRE_UNEXPECTED_TYPE_ERROR),
			false);
	}

	data = dynstr_len((dynstr_t *)top.data.ptr);

	return (stack_push_enable_attrs(self, object_create_register(data),
					ATTR_READABLE | ATTR_MUTABLE));

	return (true);
}
